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
    _imaging(*this), _grpid(grp), _resolution(0, 0), _framerate(0)
{
}

HimppVpssGroup::~HimppVpssGroup()
{
}

void HimppVpssGroup::setNRStrength(int32_t YSFStr, int32_t YTFStr)
{
	throw IpcamError("Failed to get NR strength");
}

uint32_t HimppVpssGroup::getFrameRate()
{
	if (_framerate > 0)
		return _framerate;

	return HIMPP_VIDEO_ELEMENT(source())->framerate();
}

void HimppVpssGroup::setFrameRate(uint32_t value)
{
	if (is_enabled()) {
		// TODO: implementation
	}
	_framerate = value;
}

Resolution HimppVpssGroup::getResolution()
{
	if (_resolution.valid())
		return _resolution;

	return HIMPP_VIDEO_ELEMENT(source())->resolution();
}

void HimppVpssGroup::setResolution(Resolution value)
{
	if (is_enabled()) {
		// TODO: implementation
	}
	_resolution = value;
}

void HimppVpssGroup::doEnableElement()
{
	HI_S32 s32Ret = HI_FAILURE;
	HimppVideoElement* src = HIMPP_VIDEO_ELEMENT(source());
	if (src == NULL) throw ("source element is null");  // sanity check

	Resolution in_res = src->resolution();
	VPSS_GRP_ATTR_S attr;
	memset(&attr, 0, sizeof(attr));
	attr.u32MaxW = in_res.width();
	attr.u32MaxH = in_res.height();
	attr.enPixelFormat = HIMPP_PIXEL_FORMAT;
	attr.enDynamicRange = DYNAMIC_RANGE_SDR8;
	attr.bNrEn = HI_TRUE;
	attr.stNrAttr.enNrType = VPSS_NR_TYPE_VIDEO;
	attr.stNrAttr.enNrMotionMode = NR_MOTION_MODE_NORMAL;
	attr.stNrAttr.enCompressMode = COMPRESS_MODE_FRAME;
	attr.stFrameRate.s32SrcFrameRate = -1;
	attr.stFrameRate.s32DstFrameRate = -1;
	if ((s32Ret = HI_MPI_VPSS_CreateGrp(_grpid, &attr)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VPSS_CreateGrp %d failed [%#x]\n",
		            _grpid, s32Ret);
		throw IpcamError("failed to enable vpss group");
	}

	if (_resolution.valid()) {
		VPSS_CROP_INFO_S crop;
		crop.bEnable = HI_TRUE;
		crop.enCropCoordinate = VPSS_CROP_ABS_COOR;
		crop.stCropRect.s32X = (in_res.width() - _resolution.width()) / 2;
		crop.stCropRect.s32Y = (in_res.height() - _resolution.height()) / 2;
		crop.stCropRect.u32Width = _resolution.width();
		crop.stCropRect.u32Height = _resolution.height();
		if ((s32Ret = HI_MPI_VPSS_SetGrpCrop(_grpid, &crop)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VPSS_SetGrpCrop %d failed [%#x]\n",
			            _grpid, s32Ret);
		}
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

	// initialize noise reduction
	try {
		Imaging::NoiseReduction &nr = dynamic_cast<Imaging::NoiseReduction&>(_imaging.noisereduction());
		switch (nr.getMode()) {
		case VNR_OFF:
			break;
		case VNR_MANUAL:
			break;
		case VNR_AUTO:
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
			break;
		case VNR_MANUAL:
			break;
		case VNR_AUTO:
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
	for (MediaElement* p = source; p; p = p->source()) {
		HimppVpssGroup* group = HIMPP_VPSS_GROUP(p);
		if (p != nullptr) {
			_grpid = group->groupId();
			_mpp_chn.enModId = HI_ID_VPSS;
			_mpp_chn.s32DevId = _grpid;
			_mpp_chn.s32ChnId = chn;
			break;
		}
	}

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

uint32_t HimppVpssChan::getFrameRate()
{
	if (_framerate > 0)
		return _framerate;

	return HIMPP_VIDEO_ELEMENT(source())->framerate();
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
	if (_resolution.valid())
		return _resolution;

	return HIMPP_VIDEO_ELEMENT(source())->resolution();
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
	Resolution inres = HIMPP_VIDEO_ELEMENT(source())->resolution();
	uint32_t ifr = HIMPP_VIDEO_ELEMENT(source())->framerate();
	VPSS_CHN_ATTR_S chn_attr;
	VPSS_EXT_CHN_ATTR_S ext_chn_attr;
	HI_S32 s32Ret;

	switch (_type) {
	case VPSS_CHN_TYPE_PHY:
		chn_attr.enChnMode = VPSS_CHN_MODE_USER;
		chn_attr.u32Width = resolution().width();
		chn_attr.u32Height = resolution().height();
		chn_attr.enVideoFormat = VIDEO_FORMAT_LINEAR;
		chn_attr.enPixelFormat = HIMPP_PIXEL_FORMAT;
		chn_attr.enDynamicRange = DYNAMIC_RANGE_SDR8;
		chn_attr.enCompressMode = COMPRESS_MODE_NONE;
		chn_attr.u32Depth = 0;
		chn_attr.bMirror = HI_FALSE;
		chn_attr.bFlip = HI_FALSE;
		chn_attr.stAspectRatio.enMode = ASPECT_RATIO_NONE;
		if (_framerate > 0) {
			chn_attr.stFrameRate.s32SrcFrameRate = ifr;
			chn_attr.stFrameRate.s32DstFrameRate = _framerate;
		} else {
			chn_attr.stFrameRate.s32SrcFrameRate = -1;
			chn_attr.stFrameRate.s32DstFrameRate = -1;
		}
		if ((s32Ret = HI_MPI_VPSS_SetChnAttr(_grpid, _chnid, &chn_attr)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VPSS_SetChnAttr %d-%d failed %#x\n",
			            _grpid, _chnid, s32Ret);
			throw IpcamError("Failed to enable vpss channel");
		}
		break;
	case VPSS_CHN_TYPE_EXT:
		if (!HIMPP_VPSS_CHAN(source()))
			throw IpcamError("Not connect to VPSS channel");

		ext_chn_attr.s32BindChn = HIMPP_VPSS_CHAN(source())->channelId();
		if (_framerate > 0) {
			ext_chn_attr.stFrameRate.s32SrcFrameRate = ifr;
			ext_chn_attr.stFrameRate.s32DstFrameRate = _framerate;
		} else {
			ext_chn_attr.stFrameRate.s32SrcFrameRate = -1;
			ext_chn_attr.stFrameRate.s32DstFrameRate = -1;
		}
		if (_resolution.valid()) {
			ext_chn_attr.u32Width = _resolution.width();
			ext_chn_attr.u32Height = _resolution.height();
		} else {
			ext_chn_attr.u32Width = inres.width();
			ext_chn_attr.u32Height = inres.height();
		}
		ext_chn_attr.enVideoFormat = VIDEO_FORMAT_LINEAR;
		ext_chn_attr.enPixelFormat = HIMPP_PIXEL_FORMAT;
		ext_chn_attr.enDynamicRange = DYNAMIC_RANGE_SDR8;
		ext_chn_attr.enCompressMode = COMPRESS_MODE_NONE;
		ext_chn_attr.u32Depth = 0;
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
