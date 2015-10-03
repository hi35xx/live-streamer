/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-media.h
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

#ifndef _HIMPP_MEDIA_H_
#define _HIMPP_MEDIA_H_

#include <himpp-sysctl.h>
#include <himpp-video-sensor.h>
#include <himpp-video-isp.h>
#include <himpp-video-viu.h>
#include <himpp-video-vpss.h>
#include <himpp-video-venc.h>

#include <MediaSink.hh>
#include <OnDemandServerMediaSubsession.hh>

#include <ipcam-media.h>

using namespace Ipcam::Media;

class HimppMedia;

class HimppVideoSource : public Ipcam::Media::VideoSource
{
public:
    HimppVideoSource(HimppMedia &media);
    ~HimppVideoSource();

    // video source method
    bool        setFramerate(uint32_t value);
    uint32_t    getFramerate();
    bool        setResolution(ImageResolution &res);
    ImageResolution getResolution();
    // imaging method
    int         getBrightness();
    bool        setBrightness(int value);
    int         getContrast();
    bool        setContrast(int value);
    int         getChroma();
    bool        setChroma(int value);
    int         getSaturation();
    bool        setSaturation(int value);
    int         getSharpness();
    bool        setSharpness(int value);
private:
    HimppMedia&         _media;
};

class HimppVideoEncoder : public Ipcam::Media::H264VideoEncoder
{
public:
    HimppVideoEncoder(HimppVencChan &venc_chan);
    ~HimppVideoEncoder();

    // video encoder method
    VIDEO_ENCODING getEncoding();
    ImageResolution getResolution();
    bool setResolution(ImageResolution &resolution);
    uint32_t  getFramerate();
    bool setFramerate(uint32_t fps);
    // h264 method
    RC_MODE getRcMode();
    bool setRcMode(RC_MODE mode);
    uint32_t  getBitrate();
    bool setBitrate(uint32_t kbps);
    H264_PROFILE getProfile();
    bool setProfile(H264_PROFILE profile);
    uint32_t  getGovLength();
    bool setGovLength(uint32_t gop);
    // local methods
    VENC_GRP groupId() { return _venc_chan.groupId(); }
    VENC_CHN channelId() { return _venc_chan.channelId(); }
    int fileDescriptor();

    bool enable();
    bool disable();
private:
    HimppVencChan&      _venc_chan;
};

class HimppMedia
{
public:
    HimppMedia(UsageEnvironment*, std::string);
    ~HimppMedia();

    std::list<HimppVideoSource*> getVideoSourceList();
    std::list<HimppVideoEncoder*> getVideoEncoderList();
    std::list<ServerMediaSession*> getServerMediaSessionList();
private:
    UsageEnvironment*   _env;
    std::list<ServerMediaSession*> _sms_list;

    HimppSysctl         _sysctl;
    HimppVideoSensor&   _sensor;
    HimppViDev          _vi_dev;
    HimppViChan         _vi_chan;
    HimppVpssGroup      _vpss_group;
    HimppVpssChan       _vpss_chan;
    HimppVencChan       _venc_chan0;
    HimppVencChan       _venc_chan1;

    friend class HimppVideoSource;
    friend class HimppVideoEncoder;

    HimppVideoSource    _video_source;
    HimppVideoEncoder   _video_encoder0;
    HimppVideoEncoder   _video_encoder1;

    ServerMediaSession* addSMS(HimppVideoEncoder& encoder);
};

#endif // _HIMPP_MEDIA_H_

