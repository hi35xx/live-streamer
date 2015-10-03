/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ipcam-media.h
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

#ifndef _IPCAM_MEDIA_H_
#define _IPCAM_MEDIA_H_

#include <stdbool.h>
#include <string>

namespace Ipcam
{

namespace Media
{


class ImageResolution
{
public:
    ImageResolution(uint32_t w, uint32_t h);
    ImageResolution(std::string resolution);
    operator std::string ();

    uint16_t Width;
    uint16_t Height;
};

class Imaging
{
public:
    virtual int         getBrightness() = 0;
    virtual bool        setBrightness(int value) = 0;
    virtual int         getContrast() = 0;
    virtual bool        setContrast(int value) = 0;
    virtual int         getChroma() = 0;
    virtual bool        setChroma(int value) = 0;
    virtual int         getSaturation() = 0;
    virtual bool        setSaturation(int value) = 0;
    virtual int         getSharpness() = 0;
    virtual bool        setSharpness(int value) = 0;
#if 0
    virtual bool        getLDCEnabled() = 0;
    virtual bool        setLDCEnabled(bool value) = 0;
    virtual uint32_t    getLDCRatio() = 0;
    virtual bool        setLDCRatio(uint32_t value) = 0;
#endif
};

class VideoSource : public Imaging
{
public:
    virtual uint32_t    getFramerate() = 0;
    virtual bool        setFramerate(uint32_t value) = 0;
    virtual ImageResolution getResolution() = 0;
    virtual bool        setResolution(ImageResolution &value) = 0;
};

enum VIDEO_ENCODING { H264, MJPEG, JPEG, MPEG4 };
enum RC_MODE { CBR, VBR, FIXQP };

class VideoEncoder
{
public:
    virtual VIDEO_ENCODING getEncoding() = 0;
    virtual ImageResolution getResolution() = 0;
    virtual bool setResolution(ImageResolution &resolution) = 0;
    virtual RC_MODE getRcMode() = 0;
    virtual bool setRcMode(RC_MODE mode) = 0;
    virtual uint32_t  getFramerate() = 0;
    virtual bool setFramerate(uint32_t fps) = 0;
    virtual uint32_t  getBitrate() = 0;
    virtual bool setBitrate(uint32_t kbps) = 0;
};

enum H264_PROFILE { BASELINE, MAIN, HIGH, SVC_T };

class H264VideoEncoder : public VideoEncoder
{
public:
    virtual H264_PROFILE getProfile() = 0;
    virtual bool setProfile(H264_PROFILE profile) = 0;
    virtual uint32_t  getGovLength() = 0;
    virtual bool setGovLength(uint32_t gop) = 0;
};

class AudioSource
{
public:
};

class AudioEncoder
{
public:
};


} // Media

} // Ipcam

#endif // _IPCAM_MEDIA_H_

