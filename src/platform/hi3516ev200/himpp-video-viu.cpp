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

#include <hi_mipi.h>
#include <mpi_sys.h>
#include <mpi_vi.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <himpp-common.h>
#include <himpp-video-sensor.h>
#include "himpp-video-viu.h"


//////////////////////////////////////////////////////////////////////////////
// HimppViDev
//////////////////////////////////////////////////////////////////////////////

HimppViDev::HimppViDev(HimppVideoElement* source, VI_DEV devid, std::string sensor)
  : VideoElement(VIDEO_ELEMENT(source)),
    HimppVideoElement(source), DefaultVideoSource(DEFAULT_VIDEO_SOURCE(source)),
    _video_sensor(&himpp_video_sensor_map.at(sensor)),
    _devid(devid), _online_mode(VI_ONLINE_VPSS_ONLINE),
    _resolution(0, 0), _framerate(0)
{
}

HimppViDev::~HimppViDev()
{
}

uint32_t HimppViDev::getFrameRate()
{
	ISP_PUB_ATTR_S *pstPubAttr = *_video_sensor;
	return (uint32_t)pstPubAttr->f32FrameRate;
}

void HimppViDev::setFrameRate(uint32_t value)
{
	if (is_enabled()) {
		// TODO: implementation
	}
	_framerate = value;
}

Resolution HimppViDev::getResolution()
{
	if (_resolution.valid())
		return _resolution;

	ISP_PUB_ATTR_S *pstPubAttr = *_video_sensor;
	return Resolution(pstPubAttr->stWndRect.u32Width, pstPubAttr->stWndRect.u32Height);
}

void HimppViDev::setResolution(Resolution value)
{
	if (is_enabled()) {
		// TODO: implementation
	}
	_resolution = value;
}

bool HimppViDev::initializeMipi()
{
	HI_S32 fd;
	lane_divide_mode_t lane_devide_mode = LANE_DIVIDE_MODE_0;
	combo_dev_t devno = _devid;
	sns_clk_source_t snsclkdev = 0;
	sns_rst_source_t snsrstdev = 0;
	combo_dev_attr_t *combo_dev_attr = *_video_sensor;

	fd = open("/dev/hi_mipi", O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "open /dev/hi_mipi failed\n");
		return false;
	}

#define MIPI_IOCTL(cmd, param) \
	if (ioctl(fd, HI_##cmd, param)) { \
		fprintf(stderr, #cmd " failed\n"); \
		close(fd); \
		return false; \
	}

	MIPI_IOCTL(MIPI_SET_HS_MODE, &lane_devide_mode);
	MIPI_IOCTL(MIPI_ENABLE_MIPI_CLOCK, &devno);
	MIPI_IOCTL(MIPI_RESET_MIPI, &devno);
	MIPI_IOCTL(MIPI_ENABLE_SENSOR_CLOCK, &snsclkdev);
	MIPI_IOCTL(MIPI_RESET_SENSOR, &snsrstdev);
	MIPI_IOCTL(MIPI_SET_DEV_ATTR, combo_dev_attr);
	MIPI_IOCTL(MIPI_UNRESET_MIPI, &devno);
	MIPI_IOCTL(MIPI_UNRESET_SENSOR, &snsrstdev);

#undef MIPI_IOCTL

	close(fd);
	return true;
}

void HimppViDev::cleanupMipi()
{
	HI_S32 fd;
	combo_dev_t devno = _devid;
	sns_clk_source_t snsclkdev = 0;
	sns_rst_source_t snsrstdev = 0;

	fd = open("/dev/hi_mipi", O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "open /dev/hi_mipi failed\n");
		return;
	}

#define MIPI_IOCTL(cmd, param) \
	if (ioctl(fd, HI_##cmd, param)) { \
		fprintf(stderr, #cmd " failed\n"); \
		close(fd); \
		return; \
	}

	MIPI_IOCTL(MIPI_RESET_SENSOR, &snsrstdev);
	MIPI_IOCTL(MIPI_DISABLE_SENSOR_CLOCK, &snsclkdev);
	MIPI_IOCTL(MIPI_RESET_MIPI, &devno);
	MIPI_IOCTL(MIPI_DISABLE_MIPI_CLOCK, &devno);

#undef MIPI_IOCTL

	close(fd);
}

void HimppViDev::doEnableElement()
{
	HI_S32 s32Ret;
	VI_DEV_ATTR_S *dev_attr;

	VI_MOD_PARAM_S param;
	if ((s32Ret = HI_MPI_VI_GetModParam(&param)) == HI_SUCCESS) {
		param.s32DetectErrFrame = 0;
		param.u32DropErrFrame = 0;
		param.enViVbSource = VB_SOURCE_COMMON;
		if ((s32Ret = HI_MPI_VI_SetModParam(&param)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VI_SetModParam failed [%#x]\n", s32Ret);
		}
	}
	else {
		HIMPP_PRINT("HI_MPI_VI_GetModParam failed [%#x]\n", s32Ret);
	}

	VI_VPSS_MODE_S online_mode;
	if ((s32Ret = HI_MPI_SYS_GetVIVPSSMode(&online_mode)) == HI_SUCCESS) {
		online_mode.aenMode[0] = _online_mode;
		if ((s32Ret = HI_MPI_SYS_SetVIVPSSMode(&online_mode)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_SYS_SetVIVPSSMode failed [%#x]\n", s32Ret);
		}
	} else {
		HIMPP_PRINT("HI_MPI_SYS_GetVIVPSSMode failed [%#x]\n", s32Ret);
	}

	if (!initializeMipi()) {
		throw IpcamError("Failed to initialize MIPI");
	}

	dev_attr = *_video_sensor;
	if ((s32Ret = HI_MPI_VI_SetDevAttr(_devid, dev_attr)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VI_SetDevAttr %d failed [%#x]\n", _devid, s32Ret);
		throw IpcamError("Failed to enable vi dev");
	}

	if ((s32Ret = HI_MPI_VI_EnableDev(_devid)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VI_EnableDev %d failed [%#x]\n", _devid, s32Ret);
		throw IpcamError("Failed to enable vi dev");
	}

	// bind vi dev pipe
	VI_DEV_BIND_PIPE_S dev_bind_pipe = {
		.u32Num = 1,
		.PipeId = { 0 }
	};
	if ((s32Ret = HI_MPI_VI_SetDevBindPipe(0, &dev_bind_pipe)) != HI_SUCCESS) {
		HI_MPI_VI_DisableDev(_devid);
		HIMPP_PRINT("HI_MPI_VI_SetDevBindPipe %d failed [%#x]\n", _devid, s32Ret);
		throw IpcamError("Failed to bind pipe");
	}
	// create pipe
	VI_PIPE_ATTR_S *pipe_attr = *_video_sensor;
	if ((s32Ret = HI_MPI_VI_CreatePipe(0, pipe_attr)) != HI_SUCCESS) {
		HI_MPI_VI_DisableDev(_devid);
		HIMPP_PRINT("HI_MPI_VI_CreatePipe %d failed [%#x]\n", _devid, s32Ret);
		throw IpcamError("Failed to create pipe");
	}
	ISP_PUB_ATTR_S *pstPubAttr = *_video_sensor;
	Resolution inres(pstPubAttr->stWndRect.u32Width, pstPubAttr->stWndRect.u32Height);
	if (_resolution.valid() && _resolution != inres) {
		CROP_INFO_S crop;
		crop.bEnable = HI_TRUE;
		crop.stRect.s32X = (inres.width() - _resolution.width()) / 2;
		crop.stRect.s32Y = (inres.height() - _resolution.height()) / 2;
		crop.stRect.u32Width = _resolution.width();
		crop.stRect.u32Height = _resolution.height();
		if ((s32Ret = HI_MPI_VI_SetPipeCrop(0, &crop)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VI_SetPipeCrop %d failed [%#x]\n", _devid, s32Ret);
		}
	}
	if ((s32Ret = HI_MPI_VI_StartPipe(0)) != HI_SUCCESS) {
		HI_MPI_VI_DisableDev(_devid);
		HIMPP_PRINT("HI_MPI_VI_StartPipe %d failed [%#x]\n", _devid, s32Ret);
		throw IpcamError("Failed to start pipe");
	}
}

void HimppViDev::doDisableElement()
{
	HI_MPI_VI_StopPipe(0);
	HI_MPI_VI_DestroyPipe(0);
	HI_MPI_VI_DisableDev(_devid);

	cleanupMipi();
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
		VI_LDCV3_ATTR_S ldc_attr;
		if ((s32Ret = HI_MPI_VI_GetChnLDCV3Attr(vichan.channelId(), 0, &ldc_attr)) != HI_SUCCESS)
			throw IpcamError("failed to get LDC attr\n");

		ldc_attr.bEnable = (value == LDC_ON) ? HI_TRUE : HI_FALSE;

		if ((s32Ret = HI_MPI_VI_SetChnLDCV3Attr(vichan.channelId(), 0, &ldc_attr)) != HI_SUCCESS)
			throw IpcamError("failed to set LDC attr\n");
	}

	_ldc_mode = value;
}

uint32_t HimppViChan::Imaging::LDC::getRatio()
{
	return _ldc_ratio;
}

void HimppViChan::Imaging::LDC::setRatio(uint32_t value)
{
	HimppViChan& vichan = dynamic_cast<HimppViChan&>(imaging().videoSource());
	if (vichan.is_enabled()) {
		HI_S32 s32Ret;
		VI_LDCV3_ATTR_S ldc_attr;
		if ((s32Ret = HI_MPI_VI_GetChnLDCV3Attr(0, vichan.channelId(), &ldc_attr)) != HI_SUCCESS)
			throw IpcamError("failed to get LDC attr\n");

		ldc_attr.stAttr.s32DistortionRatio = value;

		if ((s32Ret = HI_MPI_VI_SetChnLDCV3Attr(0, vichan.channelId(), &ldc_attr)) != HI_SUCCESS)
			throw IpcamError("failed to set LDC attr\n");
	}

	_ldc_ratio = value;
}

//////////////////////////////////////////////////////////////////////////////
// HimppViChan::Imaging
//////////////////////////////////////////////////////////////////////////////

HimppViChan::Imaging::Imaging(HimppViChan& vichan)
  : DefaultVideoSource::Imaging(dynamic_cast<DefaultVideoSource&>(vichan)),
    _ldc(*this), _mirror(false), _flip(false), _rotate(ROTATION_0)
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
		if ((s32Ret = HI_MPI_VI_GetChnAttr(0, vichan.channelId(), &attr)) != HI_SUCCESS) {
			throw IpcamError("Failed to get property");
		}
		attr.bMirror = (HI_BOOL)value;
		if ((s32Ret = HI_MPI_VI_SetChnAttr(0, vichan.channelId(), &attr)) != HI_SUCCESS) {
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
		if ((s32Ret = HI_MPI_VI_GetChnAttr(0, vichan.channelId(), &attr)) != HI_SUCCESS) {
			throw IpcamError("Failed to get property");
		}
		attr.bFlip = (HI_BOOL)value;
		if ((s32Ret = HI_MPI_VI_SetChnAttr(0, vichan.channelId(), &attr)) != HI_SUCCESS) {
			throw IpcamError("Failed to set property");
		}
	}

	_flip = value;
}

uint32_t HimppViChan::Imaging::getRotateAngle()
{
	switch (_rotate) {
	case ROTATION_0:
		return 0;
	case ROTATION_90:
		return 90;
	case ROTATION_180:
		return 180;
	case ROTATION_270:
		return 270;
	default:
		break;
	}
	return 0;
}

void HimppViChan::Imaging::setRotateAngle(uint32_t value)
{
	ROTATION_E rotate;
	switch (value) {
	case 0:
		rotate = ROTATION_0;
		break;
	case 90:
		rotate = ROTATION_90;
		break;
	case 180:
		rotate = ROTATION_180;
		break;
	case 270:
		rotate = ROTATION_270;
		break;
	default:
		throw IpcamError("Only support rotate 0/90/180/270 degree");
	}

	HimppViChan& vichan = dynamic_cast<HimppViChan&>(videoSource());
	if (vichan.is_enabled()) {
		ROTATION_E old_val = _rotate;
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
    _imaging(*this), _chnid(chnid), _resolution(0, 0), _framerate(0),
    _xoffset(-1), _yoffset(-1)
{
	for (MediaElement* p = source; p; p = p->source()) {
		HimppViDev* videv = HIMPP_VI_DEV(p);
		if (videv != nullptr) {
			_mpp_chn.enModId = HI_ID_VI;
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

uint32_t HimppViChan::getFrameRate()
{
	if (_framerate > 0)
		return _framerate;

	return HIMPP_VIDEO_ELEMENT(source())->framerate();
}

void HimppViChan::setFrameRate(uint32_t value)
{
	if (is_enabled()) {
		// TODO: implementation
	}
	_framerate = value;
}

Resolution HimppViChan::getResolution()
{
	if (_resolution.valid())
		return _resolution;

	return HIMPP_VIDEO_ELEMENT(source())->resolution();
}

void HimppViChan::setResolution(Resolution value)
{
	if (is_enabled()) {
		// TODO: implementation
	}
	_resolution = value;
}

void HimppViChan::setCropOffset(int32_t xoffset, int32_t yoffset)
{
	_xoffset = xoffset;
	_yoffset = yoffset;
}

void HimppViChan::doEnableElement()
{
	HI_U32 s32Ret;
	Resolution in_res = HIMPP_VIDEO_ELEMENT(source())->resolution();

	VI_VPSS_MODE_S online_mode = { { 0, 0 } };
	if ((s32Ret = HI_MPI_SYS_GetVIVPSSMode(&online_mode)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_SYS_GetViVpssMode failed [%#x]\n", s32Ret);
	}

	if (_chnid < VI_MAX_PHY_CHN_NUM) {
		// Physical channel
		VI_CHN_ATTR_S attr;
		attr.stSize.u32Width = in_res.width();
		attr.stSize.u32Height = in_res.height();
		attr.enPixelFormat = HIMPP_PIXEL_FORMAT;
		attr.enDynamicRange = DYNAMIC_RANGE_SDR8;
		attr.enVideoFormat = VIDEO_FORMAT_LINEAR;
		attr.enCompressMode = COMPRESS_MODE_NONE;
		attr.bMirror = _imaging._mirror ? HI_TRUE : HI_FALSE;
		attr.bFlip = _imaging._flip ? HI_TRUE : HI_FALSE;
		attr.u32Depth = 0;
		if (_framerate > 0) {
			attr.stFrameRate.s32SrcFrameRate = HIMPP_VIDEO_ELEMENT(source())->framerate();
			attr.stFrameRate.s32DstFrameRate = _framerate;
		} else {
			attr.stFrameRate.s32SrcFrameRate = -1;
			attr.stFrameRate.s32DstFrameRate = -1;
		}

		if ((s32Ret = HI_MPI_VI_SetChnAttr(0, _chnid, &attr)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VI_SetChnAttr %d failed [%#x]\n",
			            _chnid, s32Ret);
			throw IpcamError("Failed to enable vi chn");
		}

		// Rotate/LDC functionality in viu only available in offline mode
		VI_VPSS_MODE_E mode = online_mode.aenMode[0];
		if ((mode == VI_ONLINE_VPSS_OFFLINE) || (mode == VI_OFFLINE_VPSS_OFFLINE)) {
			if (_resolution.valid() && _resolution != in_res) {
				VI_CROP_INFO_S crop;
				crop.bEnable = HI_TRUE;
				crop.enCropCoordinate = VI_CROP_ABS_COOR;
				crop.stCropRect.s32X = _xoffset >= 0 ? _xoffset : (in_res.width() - _resolution.width()) / 2;
				crop.stCropRect.s32Y = _yoffset >= 0 ? _yoffset : (in_res.height() - _resolution.height()) / 2;
				crop.stCropRect.u32Width = _resolution.width();
				crop.stCropRect.u32Height = _resolution.height();
				if ((s32Ret = HI_MPI_VI_SetChnCrop(0, _chnid, &crop)) != HI_SUCCESS) {
					HIMPP_PRINT("HI_MPI_VI_SetChnCrop %d failed [%#x]\n", _chnid, s32Ret);
					throw IpcamError("Failed to set vi chn crop\n");
				}
			}
			if ((s32Ret = HI_MPI_VI_SetChnRotation(0, _chnid, _imaging._rotate)) != HI_SUCCESS) {
				HIMPP_PRINT("HI_MPI_VI_SetRotate %d failed [%#x]\n",
				            _chnid, s32Ret);
			}

			VI_LDCV3_ATTR_S ldc_attr;
			memset(&ldc_attr, 0, sizeof(ldc_attr));
			ldc_attr.bEnable = (_imaging._ldc._ldc_mode == LDC_ON) ?
				HI_TRUE : HI_FALSE;
			ldc_attr.stAttr.s32CenterXOffset = 0;
			ldc_attr.stAttr.s32CenterYOffset = 0;
			ldc_attr.stAttr.s32DistortionRatio = _imaging._ldc._ldc_ratio;
			if ((s32Ret = HI_MPI_VI_SetChnLDCV3Attr(0, _chnid, &ldc_attr)) != HI_SUCCESS) {
				HIMPP_PRINT("HI_MPI_VI_SetLDCV3Attr %d failed [%#x]\n",
				            _chnid, s32Ret);
			}
		}
	}
	else {	// Extended channel
		HimppViChan* phychan = HIMPP_VI_CHAN(source());
		if (!phychan)
			throw IpcamError("EXTCHN must bind to PHYCHN");

		VI_EXT_CHN_ATTR_S extattr;
		extattr.enSource = VI_EXT_CHN_SOURCE_TAIL;
		extattr.s32BindChn = phychan->channelId();
		extattr.stSize.u32Width = _resolution.width();
		extattr.stSize.u32Height = _resolution.height();
		if (_framerate > 0) {
			extattr.stFrameRate.s32SrcFrameRate = phychan->framerate();
			extattr.stFrameRate.s32DstFrameRate = _framerate;
		} else {
			extattr.stFrameRate.s32SrcFrameRate = -1;
			extattr.stFrameRate.s32DstFrameRate = -1;
		}
		extattr.enPixFormat = HIMPP_PIXEL_FORMAT;
		extattr.enDynamicRange = DYNAMIC_RANGE_SDR8;
		extattr.enCompressMode = COMPRESS_MODE_NONE;
		extattr.u32Depth = 0;
		if ((s32Ret = HI_MPI_VI_SetExtChnAttr(0, _chnid, &extattr)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VI_SetExtChnAttr %d failed [%#x]\n",
			            _chnid, s32Ret);
			throw IpcamError("HI_MPI_VI_SetExtChnAttr() failed");
		}
	}

	if ((s32Ret = HI_MPI_VI_EnableChn(0, _chnid)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VI_EnableChn %d failed [%#x]\n",
					_chnid, s32Ret);
		throw IpcamError("Failed to enable vichn");
	}
}

void HimppViChan::doDisableElement()
{
	HI_S32 s32Ret;
	if ((s32Ret = HI_MPI_VI_DisableChn(0, _chnid)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VI_DisableChn %d failed [%#x]\n",
					_chnid, s32Ret);
	}
}

VideoSource::Imaging& HimppViChan::imaging()
{
	return dynamic_cast<VideoSource::Imaging&>(_imaging);
}
