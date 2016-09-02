/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-codec.hh
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

#ifndef _HIMPP_VIDEO_CODEC_H_
#define _HIMPP_VIDEO_CODEC_H_

#include <ipcam-media-iface.h>
#include <himpp-object.h>
#include <hi_comm_vi.h>
#include <mpi_vi.h>

class HimppVideoCodec: public HimppVideoObject 
{
public:
	HimppVideoCodec(HimppVideoObject *source)
		: HimppVideoObject(source) {}

	virtual VI_DEV_ATTR_S* viDevAttr() = 0;

	virtual void     setBrightness(uint32_t ch, uint32_t val) = 0;
	virtual void     setContrast(uint32_t ch, uint32_t val) = 0;
	virtual void     setChroma(uint32_t ch, uint32_t val) = 0;
	virtual void     setSaturation(uint32_t ch, uint32_t val) = 0;
	virtual void     setSharpness(uint32_t ch, uint32_t val) = 0;
};

class HimppVideoCodecNVP1918 : public HimppVideoCodec
{
public:
    HimppVideoCodecNVP1918();
    ~HimppVideoCodecNVP1918();

    operator MPP_CHN_S* () { return NULL; }
    bool setResolution(ImageResolution &);
    ImageResolution getResolution();
    bool setFramerate(uint32_t);
    uint32_t getFramerate();

	virtual VI_DEV_ATTR_S* viDevAttr();

	virtual void     setBrightness(uint32_t ch, uint32_t val);
	virtual void     setContrast(uint32_t ch, uint32_t val);
	virtual void     setChroma(uint32_t ch, uint32_t val);
	virtual void     setSaturation(uint32_t ch, uint32_t val);
	virtual void     setSharpness(uint32_t ch, uint32_t val);
protected:
    bool enableObject();
    bool disableObject();
private:
	int             _codec_fd;
	ImageResolution _resolution;
	uint32_t        _framerate;
};

#endif // _HIMPP_VIDEO_CODEC_H_
