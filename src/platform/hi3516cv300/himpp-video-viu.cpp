/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-viu.cpp
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
#include <mpi_vi.h>

#include <unistd.h>
#include <himpp-common.h>
#include <himpp-video-isp.h>
#include "himpp-video-viu.h"


//////////////////////////////////////////////////////////////////////////////
// HimppViDev::Imaging::IrCutFilter
//////////////////////////////////////////////////////////////////////////////

HimppViDev::Imaging::IrCutFilter::IrCutFilter(Imaging& imaging)
  : DefaultVideoSource::Imaging::IrCutFilter(dynamic_cast<DefaultVideoSource::Imaging&>(imaging)),
    _mode(IRCUT_OFF)
{
}

HimppViDev::Imaging::IrCutFilter::~IrCutFilter()
{
}

IrCutFilterMode HimppViDev::Imaging::IrCutFilter::getMode()
{
	return _mode;
}

void HimppViDev::Imaging::IrCutFilter::setMode(IrCutFilterMode value)
{
	HimppViDev& videv = dynamic_cast<HimppViDev&>(imaging().videoSource());
	if (videv.is_enabled()) {
		VI_CSC_ATTR_S csc_attr;
		if (HI_MPI_VI_GetCSCAttr(videv.deviceId(), &csc_attr) != HI_SUCCESS)
			throw IpcamError("get CSC attr failed");

		uint32_t saturation = ((HimppViDev::Imaging&)imaging())._saturation;
		csc_attr.u32SatuVal = (value == IRCUT_ON) ? 0 : saturation;
		if (HI_MPI_VI_SetCSCAttr(videv.deviceId(), &csc_attr) != HI_SUCCESS)
			throw IpcamError("set CSC attr failed");
	}

	_mode = value;
}


//////////////////////////////////////////////////////////////////////////////
// HimppViDev::Imaging
//////////////////////////////////////////////////////////////////////////////

HimppViDev::Imaging::Imaging(HimppViDev& videv)
  : DefaultVideoSource::Imaging(dynamic_cast<DefaultVideoSource&>(videv)),
    _brightness(50), _contrast(50), _chroma(50), _saturation(50),
    _ircutfilter(*this)
{
}

HimppViDev::Imaging::~Imaging()
{
}

uint32_t HimppViDev::Imaging::getBrightness()
{
	return _brightness;
}

void HimppViDev::Imaging::setBrightness(uint32_t val)
{
	HimppViDev& videv = dynamic_cast<HimppViDev&>(videoSource());
	if (videv.is_enabled()) {
		VI_CSC_ATTR_S csc_attr;
		if (HI_MPI_VI_GetCSCAttr(videv.deviceId(), &csc_attr) != HI_SUCCESS)
			throw IpcamError("get CSC attr failed");

		csc_attr.u32LumaVal = val;
		if (HI_MPI_VI_SetCSCAttr(videv.deviceId(), &csc_attr) != HI_SUCCESS)
			throw IpcamError("set CSC attr failed");
	}

	_brightness = val;
}

uint32_t HimppViDev::Imaging::getContrast()
{
	return _contrast;
}

void HimppViDev::Imaging::setContrast(uint32_t val)
{
	HimppViDev& videv = dynamic_cast<HimppViDev&>(videoSource());
	if (videv.is_enabled()) {
		VI_CSC_ATTR_S csc_attr;
		if (HI_MPI_VI_GetCSCAttr(videv.deviceId(), &csc_attr) != HI_SUCCESS)
			throw IpcamError("get CSC attr failed");

		csc_attr.u32ContrVal = val;
		if (HI_MPI_VI_SetCSCAttr(videv.deviceId(), &csc_attr) != HI_SUCCESS)
			throw IpcamError("set CSC attr failed");
	}

	_contrast = val;
}

uint32_t HimppViDev::Imaging::getChroma()
{
	return _chroma;
}

void HimppViDev::Imaging::setChroma(uint32_t val)
{
	HimppViDev& videv = dynamic_cast<HimppViDev&>(videoSource());
	if (videv.is_enabled()) {
		VI_CSC_ATTR_S csc_attr;
		if (HI_MPI_VI_GetCSCAttr(videv.deviceId(), &csc_attr) != HI_SUCCESS)
			throw IpcamError("get CSC attr failed");

		csc_attr.u32HueVal = val;
		if (HI_MPI_VI_SetCSCAttr(videv.deviceId(), &csc_attr) != HI_SUCCESS)
			throw IpcamError("set CSC attr failed");
	}

	_chroma = val;
}

uint32_t HimppViDev::Imaging::getSaturation()
{
	return _saturation;
}

void HimppViDev::Imaging::setSaturation(uint32_t val)
{
	HimppViDev& videv = dynamic_cast<HimppViDev&>(videoSource());
	if (videv.is_enabled() && (_ircutfilter._mode != IRCUT_ON)) {
		VI_CSC_ATTR_S csc_attr;
		if (HI_MPI_VI_GetCSCAttr(videv.deviceId(), &csc_attr) != HI_SUCCESS)
			throw IpcamError("get CSC attr failed");

		csc_attr.u32SatuVal = val;
		if (HI_MPI_VI_SetCSCAttr(videv.deviceId(), &csc_attr) != HI_SUCCESS)
			throw IpcamError("set CSC attr failed");
	}

	_saturation = val;
}

VideoSource::Imaging::IrCutFilter& HimppViDev::Imaging::ircutfilter()
{
	return _ircutfilter;
}


//////////////////////////////////////////////////////////////////////////////
// HimppViDev
//////////////////////////////////////////////////////////////////////////////

HimppViDev::HimppViDev(HimppVideoElement* source, VI_DEV devid)
  : VideoElement(VIDEO_ELEMENT(source)),
    HimppVideoElement(source), DefaultVideoSource(DEFAULT_VIDEO_SOURCE(source)),
    _imaging(*this), _devid(devid),
    _resolution(0, 0), _framerate(0),
    _xoffset(0), _yoffset(0)
{
}

HimppViDev::~HimppViDev()
{
}

Resolution HimppViDev::resolution()
{
	if (_resolution.valid())
		return _resolution;

	return HIMPP_VIDEO_ELEMENT(source())->resolution();
}

uint32_t HimppViDev::framerate()
{
	return HIMPP_VIDEO_ELEMENT(source())->framerate();
}

uint32_t HimppViDev::getFrameRate()
{
	return framerate();
}

void HimppViDev::setFrameRate(uint32_t value)
{
	uint32_t ifr = HIMPP_VIDEO_ELEMENT(source())->framerate();
	if (value > ifr)
		throw IpcamError("FrameRate larger than input");

	if (is_enabled()) {
		// TODO: implementation
	}
	_framerate = value;
}

Resolution HimppViDev::getResolution()
{
	return resolution();
}

void HimppViDev::setResolution(Resolution value)
{
	Resolution inres = HIMPP_VIDEO_ELEMENT(source())->resolution();
	if (value.width() > inres.width() || value.height() > inres.height())
		throw IpcamError("Resolution larger than input");

	if (is_enabled()) {
		// TODO: implementation
	}
	_resolution = value;
}

void HimppViDev::setCropOffset(int32_t xoffset, int32_t yoffset)
{
	_xoffset = xoffset;
	_yoffset = yoffset;
}

VideoSource::Imaging& HimppViDev::imaging()
{
	return dynamic_cast<VideoSource::Imaging&>(_imaging);
}

void HimppViDev::doEnableElement()
{
	HI_S32 s32Ret;
	VI_DEV_ATTR_S *dev_attr;

	VI_MOD_PARAM_S param;
	if ((s32Ret = HI_MPI_VI_GetModParam(&param)) == HI_SUCCESS) {
		param.bLumaExtendEn = HI_TRUE;
		param.bContrastModeEn = HI_TRUE;
		if ((s32Ret = HI_MPI_VI_SetModParam(&param)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VI_SetModParam failed [%#x]\n", s32Ret);
		}
	}
	else {
		HIMPP_PRINT("HI_MPI_VI_SetModParam failed [%#x]\n", s32Ret);
	}

	dev_attr = HIMPP_VIDEO_ISP(source())->videoInputConfig();
	if (_resolution.valid()) {
		RECT_S& rect = dev_attr->stDevRect;
		uint32_t w = _resolution.width();
		uint32_t h = _resolution.height();
		int32_t dx2 = (rect.u32Width - w) / 2;
		int32_t dy2 = (rect.u32Height - h) / 2;
		int32_t xoff = MAX2(MIN2(_xoffset, dx2), -dx2);
		int32_t yoff = MAX2(MIN2(_yoffset, dy2), -dy2);
		rect.s32X = rect.s32X + dx2 + xoff;
		rect.s32Y = rect.s32Y + dy2 + yoff;
		rect.u32Width = w;
		rect.u32Height = h;
	}
	if ((s32Ret = HI_MPI_VI_SetDevAttr(_devid, dev_attr)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VI_SetDevAttr %d failed [%#x]\n", _devid, s32Ret);
		throw IpcamError("Failed to enable vi dev");
	}

	if ((s32Ret = HI_MPI_VI_EnableDev(_devid)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VI_EnableDev %d failed [%#x]\n", _devid, s32Ret);
		throw IpcamError("Failed to enable vi dev");
	}

	VI_CSC_ATTR_S csc_attr;
	if ((s32Ret = HI_MPI_VI_GetCSCAttr(_devid, &csc_attr)) == HI_SUCCESS) {
		csc_attr.u32LumaVal = _imaging._brightness;
		csc_attr.u32ContrVal = _imaging._contrast;
		csc_attr.u32HueVal = _imaging._chroma;
		csc_attr.u32SatuVal = _imaging._ircutfilter._mode == IRCUT_ON ? 0 : _imaging._saturation;
		if ((s32Ret = HI_MPI_VI_SetCSCAttr(_devid, &csc_attr)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VI_SetCSCAttr %d failed [%#x]\n", _devid, s32Ret);
		}
	}
	else {
		HIMPP_PRINT("HI_MPI_VI_GetCSCAttr %d failed [%#x]\n", _devid, s32Ret);
	}
}

void HimppViDev::doDisableElement()
{
	HI_MPI_VI_DisableDev(_devid);
}


//////////////////////////////////////////////////////////////////////////////
// HimppViChan::Imaging::LDC
//////////////////////////////////////////////////////////////////////////////
HimppViChan::Imaging::LDC::LDC(Imaging& imaging)
  : DefaultVideoSource::Imaging::LDC(dynamic_cast<DefaultVideoSource::Imaging&>(imaging)),
    _ldc_mode(LDC_OFF), _ldc_ratio(0)
{
}

HimppViChan::Imaging::LDC::~LDC()
{
}

LDCMode HimppViChan::Imaging::LDC::getMode()
{
	return _ldc_mode;
}

void HimppViChan::Imaging::LDC::setMode(LDCMode value)
{
	HimppViChan& vichan = dynamic_cast<HimppViChan&>(imaging().videoSource());
	if (vichan.is_enabled()) {
		HI_S32 s32Ret;
		VI_LDC_ATTR_S ldc_attr;
		if ((s32Ret = HI_MPI_VI_GetLDCAttr(vichan.channelId(), &ldc_attr)) != HI_SUCCESS)
			throw IpcamError("failed to get LDC attr\n");

		ldc_attr.bEnable = (value == LDC_ON) ? HI_TRUE : HI_FALSE;

		if ((s32Ret = HI_MPI_VI_SetLDCAttr(vichan.channelId(), &ldc_attr)) != HI_SUCCESS)
			throw IpcamError("failed to set LDC attr\n");
	}

	_ldc_mode = value;
}

int32_t HimppViChan::Imaging::LDC::getRatio()
{
	return _ldc_ratio;
}

void HimppViChan::Imaging::LDC::setRatio(int32_t value)
{
	HimppViChan& vichan = dynamic_cast<HimppViChan&>(imaging().videoSource());
	if (vichan.is_enabled()) {
		HI_S32 s32Ret;
		VI_LDC_ATTR_S ldc_attr;
		if ((s32Ret = HI_MPI_VI_GetLDCAttr(vichan.channelId(), &ldc_attr)) != HI_SUCCESS)
			throw IpcamError("failed to get LDC attr\n");

		ldc_attr.stAttr.s32Ratio = value;

		if ((s32Ret = HI_MPI_VI_SetLDCAttr(vichan.channelId(), &ldc_attr)) != HI_SUCCESS)
			throw IpcamError("failed to set LDC attr\n");
	}

	_ldc_ratio = value;
}

//////////////////////////////////////////////////////////////////////////////
// HimppViChan::Imaging
//////////////////////////////////////////////////////////////////////////////

HimppViChan::Imaging::Imaging(HimppViChan& vichan)
  : DefaultVideoSource::Imaging(dynamic_cast<DefaultVideoSource&>(vichan)),
    _ldc(*this), _mirror(false), _flip(false), _rotate(ROTATE_NONE)
{
}

HimppViChan::Imaging::~Imaging()
{
}

bool HimppViChan::Imaging::getMirror()
{
	return _mirror;
}

void HimppViChan::Imaging::setMirror(bool value)
{
	HimppViChan& vichan = dynamic_cast<HimppViChan&>(videoSource());
	if (vichan.is_enabled()) {
		HI_U32 s32Ret;
		VI_CHN_ATTR_S attr;
		if ((s32Ret = HI_MPI_VI_GetChnAttr(vichan.channelId(), &attr)) != HI_SUCCESS) {
			throw IpcamError("Failed to get property");
		}
		attr.bMirror = (HI_BOOL)value;
		if ((s32Ret = HI_MPI_VI_SetChnAttr(vichan.channelId(), &attr)) != HI_SUCCESS) {
			throw IpcamError("Failed to set property");
		}
	}

	_mirror = value;
}

bool HimppViChan::Imaging::getFlip()
{
	return _flip;
}

void HimppViChan::Imaging::setFlip(bool value)
{
	HimppViChan& vichan = dynamic_cast<HimppViChan&>(videoSource());
	if (vichan.is_enabled()) {
		HI_U32 s32Ret;
		VI_CHN_ATTR_S   attr;
		if ((s32Ret = HI_MPI_VI_GetChnAttr(vichan.channelId(), &attr)) != HI_SUCCESS) {
			throw IpcamError("Failed to get property");
		}
		attr.bFlip = (HI_BOOL)value;
		if ((s32Ret = HI_MPI_VI_SetChnAttr(vichan.channelId(), &attr)) != HI_SUCCESS) {
			throw IpcamError("Failed to set property");
		}
	}

	_flip = value;
}

uint32_t HimppViChan::Imaging::getRotateAngle()
{
	switch (_rotate) {
	case ROTATE_NONE:
		return 0;
	case ROTATE_90:
		return 90;
	case ROTATE_180:
		return 180;
	case ROTATE_270:
		return 270;
	default:
		break;
	}
	return 0;
}

void HimppViChan::Imaging::setRotateAngle(uint32_t value)
{
	ROTATE_E rotate;
	switch (value) {
	case 0:
		rotate = ROTATE_NONE;
		break;
	case 90:
		rotate = ROTATE_90;
		break;
	case 180:
		rotate = ROTATE_180;
		break;
	case 270:
		rotate = ROTATE_270;
		break;
	default:
		throw IpcamError("Only support rotate 0/90/180/270 degree");
	}

	HimppViChan& vichan = dynamic_cast<HimppViChan&>(videoSource());
	if (vichan.is_enabled()) {
		ROTATE_E old_val = _rotate;
		vichan.doDisableElement();
		try {
			_rotate = rotate;
			vichan.doEnableElement();
		} catch (IpcamError& e) {
			_rotate = old_val;
			vichan.doEnableElement();
			throw e;
		}
	}
	_rotate = rotate;
}

VideoSource::Imaging::LDC& HimppViChan::Imaging::ldc()
{
	return dynamic_cast<VideoSource::Imaging::LDC&>(_ldc);
}

//////////////////////////////////////////////////////////////////////////////
// HimppViChan
//////////////////////////////////////////////////////////////////////////////

HimppViChan::HimppViChan(HimppVideoElement* source, VI_CHN chnid)
  : VideoElement(VIDEO_ELEMENT(source)), HimppVideoElement(source),
    DefaultVideoSource(DEFAULT_VIDEO_SOURCE(source)),
    _imaging(*this), _chnid(chnid), _resolution(0, 0), _framerate(0)
{
	for (MediaElement* p = source; p; p = p->source()) {
		HimppViDev* videv = HIMPP_VI_DEV(p);
		if (videv != nullptr) {
			_mpp_chn.enModId = HI_ID_VIU;
			_mpp_chn.s32DevId = videv->deviceId();
			_mpp_chn.s32ChnId = _chnid;
			break;
		}
	}
}

HimppViChan::~HimppViChan()
{
}

MPP_CHN_S* HimppViChan::bindSource()
{
	return &_mpp_chn;
}

Resolution HimppViChan::resolution()
{
	if (_resolution.valid())
		return _resolution;

	return HIMPP_VIDEO_ELEMENT(source())->resolution();
}

uint32_t HimppViChan::framerate()
{
	if (_framerate > 0)
		return _framerate;

	return HIMPP_VIDEO_ELEMENT(source())->framerate();
}

uint32_t HimppViChan::getFrameRate()
{
	return framerate();
}

void HimppViChan::setFrameRate(uint32_t value)
{
	if (_chnid < VIU_MAX_PHYCHN_NUM)
		throw IpcamError("Cannot set framerate on physical channel");

	if (is_enabled()) {
		// TODO: implementation
	}
	_framerate = value;
}

Resolution HimppViChan::getResolution()
{
	return resolution();
}

void HimppViChan::setResolution(Resolution value)
{
	if (_chnid < VIU_MAX_PHYCHN_NUM)
		throw IpcamError("Cannot set resolution on physical channel");

	if (is_enabled()) {
		// TODO: implementation
	}
	_resolution = value;
}

void HimppViChan::doEnableElement()
{
	HI_U32 s32Ret;
	Resolution in_dim = HIMPP_VIDEO_ELEMENT(source())->resolution();

	HI_U32 online_mode = 0;
	if ((s32Ret = HI_MPI_SYS_GetViVpssMode(&online_mode)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_SYS_GetViVpssMode failed [%#x]\n", s32Ret);
		online_mode = 0;
	}

	if (_chnid < VIU_MAX_PHYCHN_NUM) {
		// Physical channel
		VI_CHN_ATTR_S attr;
		attr.stCapRect.s32X = 0;
		attr.stCapRect.s32Y = 0;
		attr.stCapRect.u32Width = in_dim.width();
		attr.stCapRect.u32Height = in_dim.height();
		attr.stDestSize.u32Width = in_dim.width();
		attr.stDestSize.u32Height = in_dim.height();
		attr.enCapSel = VI_CAPSEL_BOTH;
		attr.enPixFormat = HIMPP_PIXEL_FORMAT;
		attr.bMirror = (HI_BOOL)_imaging._mirror;
		attr.bFlip = (HI_BOOL)_imaging._flip;
		if (_framerate > 0) {
			attr.s32SrcFrameRate = HIMPP_VIDEO_ELEMENT(source())->framerate();
			attr.s32DstFrameRate = _framerate;
		} else {
			attr.s32SrcFrameRate = -1;
			attr.s32DstFrameRate = -1;
		}
		attr.enCompressMode = COMPRESS_MODE_NONE;

		if ((s32Ret = HI_MPI_VI_SetChnAttr(_chnid, &attr)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VI_SetChnAttr %d failed [%#x]\n",
			            _chnid, s32Ret);
			throw IpcamError("Failed to enable vi chn");
		}

		// Rotate/LDC functionality in viu only available in offline mode
		if (!online_mode) {
			if ((s32Ret = HI_MPI_VI_SetRotate(_chnid, _imaging._rotate)) != HI_SUCCESS) {
				HIMPP_PRINT("HI_MPI_VI_SetRotate %d failed [%#x]\n",
				            _chnid, s32Ret);
			}

			VI_LDC_ATTR_S ldc_attr;
			ldc_attr.bEnable = (_imaging._ldc._ldc_mode == LDC_ON) ?
				HI_TRUE : HI_FALSE;
			ldc_attr.stAttr.enViewType = LDC_VIEW_TYPE_ALL;
			ldc_attr.stAttr.s32CenterXOffset = 0;
			ldc_attr.stAttr.s32CenterYOffset = 0;
			ldc_attr.stAttr.s32Ratio = _imaging._ldc._ldc_ratio;

			if ((s32Ret = HI_MPI_VI_SetLDCAttr(_chnid, &ldc_attr)) != HI_SUCCESS) {
				HIMPP_PRINT("HI_MPI_VI_SetLDCAttr %d failed [%#x]\n",
				            _chnid, s32Ret);
			}
		}
	}
	else {	// Extended channel
		if (online_mode)
			throw IpcamError("Cannot create EXTCHN for online mode");

		HimppViChan* phychan = HIMPP_VI_CHAN(source());
		if (!phychan)
			throw IpcamError("EXTCHN must bind to PHYCHN");

		VI_EXT_CHN_ATTR_S extattr;
		extattr.s32BindChn = phychan->channelId();
		if (_resolution.valid()) {
			extattr.stDestSize.u32Width = _resolution.width();
			extattr.stDestSize.u32Height = _resolution.height();
		} else {
			extattr.stDestSize.u32Width = in_dim.width();
			extattr.stDestSize.u32Height = in_dim.height();
		}
		if (_framerate > 0) {
			extattr.s32SrcFrameRate = phychan->framerate();
			extattr.s32DstFrameRate = _framerate;
		} else {
			extattr.s32SrcFrameRate = -1;
			extattr.s32DstFrameRate = -1;
		}
		extattr.enPixFormat = HIMPP_PIXEL_FORMAT;
		extattr.enCompressMode = COMPRESS_MODE_NONE;
		if ((s32Ret = HI_MPI_VI_SetExtChnAttr(_chnid, &extattr)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VI_SetExtChnAttr %d failed [%#x]\n",
			            _chnid, s32Ret);
			throw IpcamError("HI_MPI_VI_SetExtChnAttr() failed");
		}
	}

	if ((s32Ret = HI_MPI_VI_EnableChn(_chnid)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VI_EnableChn %d failed [%#x]\n",
					_chnid, s32Ret);
		throw IpcamError("Failed to enable vichn");
	}
}

void HimppViChan::doDisableElement()
{
	HI_S32 s32Ret;
	if ((s32Ret = HI_MPI_VI_DisableChn(_chnid)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VI_DisableChn %d failed [%#x]\n",
					_chnid, s32Ret);
	}
}

VideoSource::Imaging& HimppViChan::imaging()
{
	return dynamic_cast<VideoSource::Imaging&>(_imaging);
}
