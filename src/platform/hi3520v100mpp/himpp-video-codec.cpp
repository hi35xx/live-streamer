/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-codec.cpp
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <himpp-common.h>
#include "himpp-video-codec.h"

#include <nvp1918.h>

#define NVP1918_FILE    "/dev/nc_vdec"

static VI_DEV_ATTR_S devattr_bt656_d1_4mux = {
    .enIntfMode = VI_MODE_BT656,
    .enWorkMode = VI_WORK_MODE_4Multiplex,
    .au32CompMask = { 0xFF000000, 0x0 },
    .enScanMode = VI_SCAN_INTERLACED,
    .s32AdChnId = { -1, -1, -1, -1 },
    .enDataSeq = VI_INPUT_DATA_VUVU,
    .stSynCfg = { },
    .enDataPath = VI_PATH_BYPASS,
    .enInputDataType = VI_DATA_TYPE_YUV,
    .bDataRev = HI_TRUE,
};

HimppVideoCodecNVP1918::HimppVideoCodecNVP1918()
	: HimppVideoCodec(NULL), _codec_fd(-1),
	  _resolution(720, 480), _framerate(30)
{
}

HimppVideoCodecNVP1918::~HimppVideoCodecNVP1918()
{
}

VI_DEV_ATTR_S* HimppVideoCodecNVP1918::viDevAttr()
{
	return &devattr_bt656_d1_4mux;
}

bool HimppVideoCodecNVP1918::setResolution(ImageResolution &res)
{
	return false;
}

ImageResolution HimppVideoCodecNVP1918::getResolution()
{
	return _resolution;
}

bool HimppVideoCodecNVP1918::setFramerate(uint32_t value)
{
	return false;
}

uint32_t HimppVideoCodecNVP1918::getFramerate()
{
	return _framerate;
}

void HimppVideoCodecNVP1918::setBrightness(uint32_t ch, uint32_t val)
{
	if (_codec_fd < 0)
		throw IpcamError("CODEC not opened");

	vdec_video_adjust v_adj = {
		.ch = ch,
		.value = val
	};
	if (ioctl(_codec_fd, IOC_VDEC_SET_BRIGHTNESS, &v_adj))
		throw IpcamError("Set nvp1918 brightness failed\n");
}

void HimppVideoCodecNVP1918::setContrast(uint32_t ch, uint32_t val)
{
	if (_codec_fd < 0)
		throw IpcamError("CODEC not opened");

	vdec_video_adjust v_adj = {
		.ch = ch,
		.value = val
	};
	if (ioctl(_codec_fd, IOC_VDEC_SET_CONTRAST, &v_adj))
		throw IpcamError("Set nvp1918 contrast failed\n");
}

void HimppVideoCodecNVP1918::setChroma(uint32_t ch, uint32_t val)
{
	if (_codec_fd < 0)
		throw IpcamError("CODEC not opened");

	vdec_video_adjust v_adj = {
		.ch = ch,
		.value = val
	};
	if (ioctl(_codec_fd, IOC_VDEC_SET_HUE, &v_adj))
		throw IpcamError("Set nvp1918 chroma failed\n");
}

void HimppVideoCodecNVP1918::setSaturation(uint32_t ch, uint32_t val)
{
	if (_codec_fd < 0)
		throw IpcamError("CODEC not opened");

	vdec_video_adjust v_adj = {
		.ch = ch,
		.value = val
	};
	if (ioctl(_codec_fd, IOC_VDEC_SET_SATURATION, &v_adj))
		throw IpcamError("Set nvp1918 saturation failed\n");
}

void HimppVideoCodecNVP1918::setSharpness(uint32_t ch, uint32_t val)
{
	if (_codec_fd < 0)
		throw IpcamError("CODEC not opened");

	vdec_video_adjust v_adj = {
		.ch = ch,
		.value = val
	};
	if (ioctl(_codec_fd, IOC_VDEC_SET_SHARPNESS, &v_adj))
		throw IpcamError("Set nvp1918 sharpness failed\n");
}

bool HimppVideoCodecNVP1918::enableObject()
{
    _codec_fd = open(NVP1918_FILE, O_RDWR);
    if (_codec_fd < 0) {
        HIMPP_PRINT("Open %s failed\n", NVP1918_FILE);
        return false;
    }

    vdec_video_mode vmode = {
        .chip = 0,
        .mode = VDEC_NTSC,
    };
    if (ioctl(_codec_fd, IOC_VDEC_SET_VIDEO_MODE, &vmode)) {
        HIMPP_PRINT("Set nvp1918 video mode failed\n");
        close(_codec_fd);
        return false;
    }

	return true;
}

bool HimppVideoCodecNVP1918::disableObject()
{
	if (_codec_fd != -1)
		close(_codec_fd);

	return true;
}
