/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-vpss-group.cpp
 * Copyright (C) 2015 Watson Xu <xuhuashan@gmail.com>
 *
 * live-streamer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * live-streamer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <mpi_sys.h>
#include <himpp-common.h>
#include "himpp-video-vpss.h"

HimppVpssGroup::HimppVpssGroup(HimppVideoElement* source, VPSS_GRP grp)
  : VideoElement(VIDEO_ELEMENT(source)), HimppVideoElement(source),
    DefaultVideoSource(DEFAULT_VIDEO_SOURCE(source)),
    _imaging(*this), _grpid(grp), _isp(NULL)
{
}

HimppVpssGroup::~HimppVpssGroup()
{
}

void HimppVpssGroup::timeout_handler(ev::timer& w, int revents)
{
	NrParamTable &nrtable = _imaging.noisereduction().getParamTable();
	if (nrtable.size() == 0)
		return;

	// get ISO value from ISP module
	uint32_t iso;
	try {
		VideoSource::Imaging::Exposure &vs_exp = _isp->imaging().exposure();
		auto isp_exp = dynamic_cast<HimppVideoISP::Imaging::Exposure*>(&vs_exp);
		if (isp_exp == NULL)
			throw IpcamError("Exposure interface not implemented");

		// get ExpStateInfo
		HimppVideoISP::Imaging::Exposure::StateInfo exp_state;
		isp_exp->getStateInfo(exp_state);

		iso = exp_state.ISO;
	}
	catch (IpcamError &e) {
		HIMPP_PRINT("Get ExpStateInfo failed: %s\n", e.what());
		return;
	}

	NrParamTableItem nritem = nrtable.back();
	for (auto& item : nrtable) {
		if (iso < item._1) {
			nritem = item;
			break;
		}
	}
	setNRStrength(nritem._2, nritem._3);
}

void HimppVpssGroup::enableNR(bool enable)
{
	HI_S32 s32Ret = HI_FAILURE;
	VPSS_GRP_ATTR_S grpattr;

	// enable/disable noise reduction
	if ((s32Ret = HI_MPI_VPSS_GetGrpAttr(groupId(), &grpattr)) == HI_SUCCESS) {
		grpattr.bNrEn = enable ? HI_TRUE : HI_FALSE;
		if ((s32Ret = HI_MPI_VPSS_SetGrpAttr(groupId(), &grpattr)) != HI_SUCCESS) {
			throw IpcamError("Failed to disable noise reduction");
		}
	} else {
		throw IpcamError("Failed to get vpss group attributes");
	}
}

void HimppVpssGroup::enableAutoNR(bool enable)
{
	// start the timer
	if (_isp) {
		if (enable) {
			_timer.set(1.0, 0.1);
			_timer.set<HimppVpssGroup, &HimppVpssGroup::timeout_handler>(this);
			_timer.start();
		} else {
			_timer.stop();
		}
	} else {
		throw IpcamError("Auto NR need the ISP instance");
	}
}

void HimppVpssGroup::setNRStrength(int32_t YSFStr, int32_t YTFStr)
{
	HI_S32 s32Ret = HI_FAILURE;
	VPSS_NR_PARAM_U nrparam;
	if ((s32Ret = HI_MPI_VPSS_GetNRParam(_grpid, &nrparam)) == HI_SUCCESS) {
		nrparam.stNRParam_V1.s32YSFStr = YSFStr;
		nrparam.stNRParam_V1.s32YTFStr = YTFStr;
		if ((s32Ret = HI_MPI_VPSS_SetNRParam(_grpid, &nrparam)) != HI_SUCCESS) {
			throw IpcamError("Failed to set NR strength");
		}
	}
	else {
		throw IpcamError("Failed to get NR strength");
	}
}

void HimppVpssGroup::doEnableElement()
{
	HI_S32 s32Ret = HI_FAILURE;
	HimppVideoElement* src = HIMPP_VIDEO_ELEMENT(source());
	if (src == NULL) throw ("source element is null");  // sanity check

	Resolution dim = src->resolution();
	VPSS_GRP_ATTR_S attr;
	attr.u32MaxW = dim.width();
	attr.u32MaxH = dim.height();
	attr.bDciEn = HI_FALSE;
	attr.bIeEn = HI_FALSE;
	attr.bNrEn = HI_FALSE;
	attr.bHistEn = HI_FALSE;
	attr.enDieMode = VPSS_DIE_MODE_NODIE;
	attr.enPixFmt = HIMPP_PIXEL_FORMAT;
	if ((s32Ret = HI_MPI_VPSS_CreateGrp(_grpid, &attr)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VPSS_CreateGrp %d failed [%#x]\n",
		            _grpid, s32Ret);
		throw IpcamError("failed to enable vpss group");
	}

	if ((s32Ret = HI_MPI_VPSS_StartGrp(_grpid)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VPSS_CreateGrp %d failed [%#x]\n",
		            _grpid, s32Ret);
		goto err_destroy_grp;
	}

	MPP_CHN_S dst_chn;
	dst_chn.enModId = HI_ID_VPSS;
	dst_chn.s32DevId = _grpid;
	dst_chn.s32ChnId = 0;
	if ((s32Ret = HI_MPI_SYS_Bind(src->bindSource(), &dst_chn)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_SYS_Bind %d failed [%#x]\n",
		            _grpid, s32Ret);
		goto err_stop_grp;
	}

	// Find the isp instance from the source element chain
	for (VideoElement *e = this; e != NULL; e = VIDEO_ELEMENT(e->source())) {
		HimppVideoISP *isp = dynamic_cast<HimppVideoISP*>(e);
		if (isp) {
			_isp = isp;
			break;
		}
	}

	// initialize noise reduction
	try {
		int32_t YSFStr, YTFStr;
		Imaging::NoiseReduction &nr = dynamic_cast<Imaging::NoiseReduction&>(_imaging.noisereduction());
		switch (nr.getMode()) {
		case VNR_OFF:
			{
				enableNR(false);
			}
			break;
		case VNR_MANUAL:
			{
				enableNR(true);
				uint32_t level = nr.getLevel();
				NrParamTableItem item = nr.getParamTable().at(level);
				YSFStr = item._2;
				YTFStr = item._3;
				setNRStrength(YSFStr, YTFStr);
			}
			break;
		case VNR_AUTO:
			{
				enableNR(true);
				enableAutoNR(true);
			}
			break;
		}
	} catch (IpcamError& e) {
		HIMPP_PRINT("Failed to initialize NoiseReduction");
	}

	return;

	err_stop_grp:
		HI_MPI_VPSS_StopGrp(_grpid);
	err_destroy_grp:
		HI_MPI_VPSS_DestroyGrp(_grpid);

		throw IpcamError("Failed to enable vpss group");
}

void HimppVpssGroup::doDisableElement()
{
	HimppVideoElement* src = HIMPP_VIDEO_ELEMENT(source());
	HI_S32 s32Ret;

	if (_isp && _imaging.noisereduction().getMode() == VNR_AUTO) {
		enableAutoNR(false);
	}

	MPP_CHN_S dst_chn;
	dst_chn.enModId = HI_ID_VPSS;
	dst_chn.s32DevId = _grpid;
	dst_chn.s32ChnId = 0;
	if ((s32Ret = HI_MPI_SYS_UnBind(src->bindSource(), &dst_chn)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_SYS_UnBind %d failed [%#x]\n",
		            _grpid, s32Ret);
	}

	s32Ret = HI_MPI_VPSS_StopGrp(_grpid);
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VPSS_StopGrp %d failed [%#x]\n",
		            _grpid, s32Ret);
	}

	s32Ret = HI_MPI_VPSS_DestroyGrp(_grpid);
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VPSS_DestroyGrp %d failed [%#x]\n",
		            _grpid, s32Ret);
	}
}

VideoSource::Imaging& HimppVpssGroup::imaging()
{
	return _imaging;
}

//////////////////////////////////////////////////////////////////////////////
// HimppVideoISP::Imaging::NoiseReduction
//////////////////////////////////////////////////////////////////////////////

HimppVpssGroup::Imaging::NoiseReduction::NoiseReduction(Imaging &imaging)
  : DefaultVideoSource::Imaging::NoiseReduction(dynamic_cast<DefaultVideoSource::Imaging&>(imaging)),
    _mode(VNR_OFF), _level(0)
{
}

HimppVpssGroup::Imaging::NoiseReduction::~NoiseReduction()
{
}

VNRMode HimppVpssGroup::Imaging::NoiseReduction::getMode()
{
	return _mode;
}

void HimppVpssGroup::Imaging::NoiseReduction::setMode(VNRMode value)
{
	HimppVpssGroup &group = dynamic_cast<HimppVpssGroup&>(imaging().videoSource());

	if (_mode == value)
		return;

	if (group.is_enabled()) {
		switch (value) {
		case VNR_OFF:
			// stop the auto NR timer if necessary
			if (_mode == VNR_AUTO) {
				group.enableAutoNR(false);
			}
			// disable noise reduction
			group.enableNR(false);
			break;
		case VNR_MANUAL:
			// stop the auto NR timer if necessary
			if (_mode == VNR_AUTO) {
				group.enableAutoNR(false);
			}
			// enable noise reduction
			group.enableNR(true);
			if (_level < _param_table.size()) {
				uint32_t tabsize = _param_table.size();
				if (tabsize > 0) {
					uint32_t lvl = _level < tabsize ? _level : tabsize - 1;
					NrParamTableItem &item = _param_table.at(lvl);
					group.setNRStrength(item._2, item._3);
				}
			}
			break;
		case VNR_AUTO:
			// enable noise reduction
			group.enableNR(true);
			group.enableAutoNR(true);
			break;
		}
	}
	_mode = value;
}

uint32_t HimppVpssGroup::Imaging::NoiseReduction::getLevel()
{
	return _level;
}

void HimppVpssGroup::Imaging::NoiseReduction::setLevel(uint32_t value)
{
	HimppVpssGroup &group = dynamic_cast<HimppVpssGroup&>(imaging().videoSource());

	uint32_t tabsize = _param_table.size();
	if (group.is_enabled() && (_mode == VNR_MANUAL) && (tabsize > 0)) {
		uint32_t lvl = value < tabsize ? value : tabsize - 1;
		NrParamTableItem &item = _param_table.at(lvl);
		group.setNRStrength(item._2, item._3);
	}
	_level = value;
}

NrParamTable& HimppVpssGroup::Imaging::NoiseReduction::getParamTable()
{
	return _param_table;
}

void ::HimppVpssGroup::Imaging::NoiseReduction::setParamTable(NrParamTable& value)
{
	if (value.size() == 0) {
		throw IpcamError("NR table should not be empty");
	}

	HimppVpssGroup &group = dynamic_cast<HimppVpssGroup&>(imaging().videoSource());
	if (group.is_enabled() && _mode == VNR_MANUAL) {
		uint32_t lvl = _level < value.size() ? _level : value.size() - 1;
		NrParamTableItem &item = value.at(lvl);
		group.setNRStrength(item._2, item._3);
	}

	_param_table = value;
}

//////////////////////////////////////////////////////////////////////////////
// HimppVideoISP::Imaging::NoiseReduction
//////////////////////////////////////////////////////////////////////////////

HimppVpssGroup::Imaging::Imaging(HimppVpssGroup& group)
  : DefaultVideoSource::Imaging(dynamic_cast<HimppVpssGroup&>(group)),
    _noisereduction(*this)
{
}

HimppVpssGroup::Imaging::~Imaging()
{
}

VideoSource::Imaging::NoiseReduction& HimppVpssGroup::Imaging::noisereduction()
{
	return _noisereduction;
}

//////////////////////////////////////////////////////////////////////////////
// HimppVpssChan
//////////////////////////////////////////////////////////////////////////////

HimppVpssChan::HimppVpssChan(HimppVideoElement* source, VPSS_CHN chn)
  : VideoElement(VIDEO_ELEMENT(source)),
    HimppVideoElement(source),
    DefaultVideoSource(DEFAULT_VIDEO_SOURCE(source)),
    _grpid(0), _chnid(chn), _resolution(0, 0), _framerate(0)
{
	HimppVpssGroup* group = HIMPP_VPSS_GROUP(source);
	HimppVpssChan* chan = HIMPP_VPSS_CHAN(source);
	if (group) {
		_grpid = group->groupId();
	} else if (chan) {
		_grpid = chan->groupId();
	}

	_mpp_chn.enModId = HI_ID_VPSS;
	_mpp_chn.s32DevId = _grpid;
	_mpp_chn.s32ChnId = chn;

	if (chn < VPSS_MAX_PHY_CHN_NUM) {
		_type = VPSS_CHN_TYPE_PHY;
	} else {
		_type = VPSS_CHN_TYPE_EXT;
	}
}

HimppVpssChan::~HimppVpssChan()
{
}

MPP_CHN_S* HimppVpssChan::bindSource()
{
	return &_mpp_chn;
}

Resolution HimppVpssChan::resolution()
{
	if (_resolution.valid())
		return _resolution;

	return HIMPP_VIDEO_ELEMENT(source())->resolution();
}

uint32_t HimppVpssChan::framerate()
{
	if (_framerate > 0)
		return _framerate;

	return HIMPP_VIDEO_ELEMENT(source())->framerate();
}

uint32_t HimppVpssChan::getFrameRate()
{
	return framerate();
}

void HimppVpssChan::setFrameRate(uint32_t value)
{
	uint32_t ifr = HIMPP_VIDEO_ELEMENT(source())->framerate();
	if (value > ifr)
		throw IpcamError("FrameRate too large");

	if (is_enabled()) {
		// TODO: implementation
	}

	_framerate = value;
}

Resolution HimppVpssChan::getResolution()
{
	return resolution();
}

void HimppVpssChan::setResolution(Resolution value)
{
	uint32_t w = value.width(), h = value.height();
	if (w < VPSS_MIN_IMAGE_WIDTH || w > VPSS_MAX_IMAGE_WIDTH ||
	    h < VPSS_MIN_IMAGE_HEIGHT || h > VPSS_MAX_IMAGE_HEIGHT)
		throw IpcamError("Invalid resolution");

	if (is_enabled()) {
		// TODO: implementation
	}

	_resolution = value;
}

void HimppVpssChan::doEnableElement()
{
	Resolution idim = HIMPP_VIDEO_ELEMENT(source())->resolution();
	uint32_t ifr = HIMPP_VIDEO_ELEMENT(source())->framerate();
	VPSS_CHN_ATTR_S chn_attr;
	VPSS_CHN_MODE_S chn_mode;
	VPSS_EXT_CHN_ATTR_S ext_chn_attr;
	HI_S32 s32Ret;

	switch (_type) {
	case VPSS_CHN_TYPE_PHY:
		chn_attr.bSpEn = HI_FALSE;
		chn_attr.bBorderEn = HI_FALSE;
		chn_attr.bMirror = HI_FALSE;
		chn_attr.bFlip = HI_FALSE;
		if (_framerate > 0) {
			chn_attr.s32SrcFrameRate = ifr;
			chn_attr.s32DstFrameRate = _framerate;
		} else {
			chn_attr.s32SrcFrameRate = -1;
			chn_attr.s32DstFrameRate = -1;
		}
		if ((s32Ret = HI_MPI_VPSS_SetChnAttr(_grpid, _chnid, &chn_attr)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VPSS_SetChnAttr %d-%d failed %#x\n",
			            _grpid, _chnid, s32Ret);
			throw IpcamError("Failed to enable vpss channel");
		}
		chn_mode.enChnMode = VPSS_CHN_MODE_USER;
		if (_resolution.valid()) {
			chn_mode.u32Width = _resolution.width();
			chn_mode.u32Height = _resolution.height();
		} else {
			chn_mode.u32Width = idim.width();
			chn_mode.u32Height = idim.height();
		}
		chn_mode.bDouble = HI_FALSE;
		chn_mode.enPixelFormat = HIMPP_PIXEL_FORMAT;
		chn_mode.enCompressMode = COMPRESS_MODE_NONE;
		if ((s32Ret = HI_MPI_VPSS_SetChnMode(_grpid, _chnid, &chn_mode)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VPSS_SetChnMode %d-%d failed %#x\n",
			            _grpid, _chnid, s32Ret);
			throw IpcamError("Failed to enable vpss channel");
		}
		break;
	case VPSS_CHN_TYPE_EXT:
		if (!HIMPP_VPSS_CHAN(source()))
			throw IpcamError("Not connect to VPSS channel");

		ext_chn_attr.s32BindChn = HIMPP_VPSS_CHAN(source())->channelId();
		if (_framerate > 0) {
			ext_chn_attr.s32SrcFrameRate = ifr;
			ext_chn_attr.s32DstFrameRate = _framerate;
		} else {
			ext_chn_attr.s32SrcFrameRate = -1;
			ext_chn_attr.s32DstFrameRate = -1;
		}
		if (_resolution.valid()) {
			ext_chn_attr.u32Width = _resolution.width();
			ext_chn_attr.u32Height = _resolution.height();
		} else {
			ext_chn_attr.u32Width = idim.width();
			ext_chn_attr.u32Height = idim.height();
		}
		ext_chn_attr.enPixelFormat = HIMPP_PIXEL_FORMAT;
		ext_chn_attr.enCompressMode = COMPRESS_MODE_NONE;
		if ((s32Ret = HI_MPI_VPSS_SetExtChnAttr(_grpid, _chnid, &ext_chn_attr)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VPSS_SetExtChnAttr %d-%d failed %#x\n",
			            _grpid, _chnid, s32Ret);
			throw IpcamError("Failed to enable vpss channel");
		}
		break;
	default:
		break;
	}

	if ((s32Ret = HI_MPI_VPSS_EnableChn(_grpid, _chnid)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VPSS_EnableChn %d-%d failed [%#x]\n",
		            _grpid, _chnid, s32Ret);
		throw IpcamError("Failed to enable vpss channel");
	}
}

void HimppVpssChan::doDisableElement()
{
	HI_S32 s32Ret;

	if ((s32Ret = HI_MPI_VPSS_DisableChn(_grpid, _chnid)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VPSS_DisableChn %d-%d failed [%#x]\n",
		            _grpid, _chnid, s32Ret);
	}
}
