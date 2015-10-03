/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-media.cpp
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

#include <H264VideoRTPSink.hh>
#include <H264VideoStreamDiscreteFramer.hh>

#include <HimppStreamInput.hh>

#include "himpp-media.h"

HimppVideoSource::HimppVideoSource(HimppMedia &media)
    : _media(media)
{
}

HimppVideoSource::~HimppVideoSource()
{
}

bool HimppVideoSource::setFramerate(uint32_t value)
{
    return false;
}

uint32_t HimppVideoSource::getFramerate()
{
    return _media._vpss_chan.getFramerate();
}

ImageResolution HimppVideoSource::getResolution()
{
    return _media._vpss_chan.getResolution();
}

bool HimppVideoSource::setResolution(ImageResolution &res)
{
    return false;
}

int HimppVideoSource::getBrightness()
{
}

bool HimppVideoSource::setBrightness(int value)
{
    return _media._vi_dev.setBrightness(value);
}

int HimppVideoSource::getContrast()
{
}

bool HimppVideoSource::setContrast(int value)
{
    return _media._vi_dev.setContrast(value);
}

int HimppVideoSource::getChroma()
{
}

bool HimppVideoSource::setChroma(int value)
{
    return _media._vi_dev.setChroma(value);
}

int HimppVideoSource::getSaturation()
{
}

bool HimppVideoSource::setSaturation(int value)
{
    return _media._vi_dev.setSaturation(value);
}

int HimppVideoSource::getSharpness()
{
    return 0;
}

bool HimppVideoSource::setSharpness(int value)
{
    return false; //_media._vpss_group.setSharpness(value);
}

//////////////////////////////////////////////////////////////////////////////

HimppVideoEncoder::HimppVideoEncoder(HimppVencChan& venc_chan)
    : _venc_chan(venc_chan)
{
}

HimppVideoEncoder::~HimppVideoEncoder()
{
}

VIDEO_ENCODING HimppVideoEncoder::getEncoding()
{
    return _venc_chan.getEncoding();
}

ImageResolution HimppVideoEncoder::getResolution()
{
    return _venc_chan.getResolution();
}

bool HimppVideoEncoder::setResolution(ImageResolution &resolution)
{
    return _venc_chan.setResolution(resolution);
}

uint32_t  HimppVideoEncoder::getFramerate()
{
    return _venc_chan.getFramerate();
}

bool HimppVideoEncoder::setFramerate(uint32_t fps)
{
    return _venc_chan.setFramerate(fps);
}

RC_MODE HimppVideoEncoder::getRcMode()
{
    return _venc_chan.getRcMode();
}

bool HimppVideoEncoder::setRcMode(RC_MODE mode)
{
    return _venc_chan.setRcMode(mode);
}

uint32_t HimppVideoEncoder::getBitrate()
{
    return _venc_chan.getBitrate();
}

bool HimppVideoEncoder::setBitrate(uint32_t kbps)
{
    return _venc_chan.setBitrate(kbps);
}

H264_PROFILE HimppVideoEncoder::getProfile()
{
    return _venc_chan.getProfile();
}

bool HimppVideoEncoder::setProfile(H264_PROFILE profile)
{
    return _venc_chan.setH264Profile(profile);
}

uint32_t  HimppVideoEncoder::getGovLength()
{
    return _venc_chan.getGop();
}

bool HimppVideoEncoder::setGovLength(uint32_t gop)
{
    return _venc_chan.setGop(gop);
}

int HimppVideoEncoder::fileDescriptor()
{
    return HI_MPI_VENC_GetFd(_venc_chan.channelId());
}

bool HimppVideoEncoder::enable()
{
    return _venc_chan.enable();
}

bool HimppVideoEncoder::disable()
{
    return _venc_chan.disable();
}

//////////////////////////////////////////////////////////////////////////////

HimppMedia::HimppMedia(UsageEnvironment *env, std::string sensor_name)
    : _env(env),
      _sensor(himpp_video_sensor_map.at(sensor_name)),
      _vi_dev(&_sensor, 0), _vi_chan(&_vi_dev, 0),
      _vpss_group(&_vi_chan, 0), _vpss_chan(&_vpss_group, 0),
      _venc_chan0(dynamic_cast<HimppVideoObject*>(&_vpss_chan), 0, 0),
      _venc_chan1(dynamic_cast<HimppVideoObject*>(&_vpss_chan), 1, 1),
      _video_source(*this), _video_encoder0(_venc_chan0), _video_encoder1(_venc_chan1)
{
    ImageResolution r0 = _venc_chan0.getResolution();
    ImageResolution r1("VGA");
    _venc_chan0.setFramerate(25);
    _venc_chan1.setResolution(r1);
    _venc_chan1.setFramerate(25);
    _sysctl.addVideoBuffer(r0.Width * r0.Height * 3 / 2, 8);
    _sysctl.addVideoBuffer(r1.Width * r1.Height * 3 / 2, 4);
    _sysctl.addVideoBuffer(196 * 4, 4);
    _sysctl.enable();

    _sms_list.push_back(addSMS(_video_encoder0));
    _sms_list.push_back(addSMS(_video_encoder1));
}

ServerMediaSession *HimppMedia::addSMS(HimppVideoEncoder& encoder)
{
    ServerMediaSession *sms;
    VENC_CHN chnid = encoder.channelId();
    char const* description = "RTSP/RTP stream from live-streamer";
    std::string streamPath = std::to_string(chnid);
    sms = ServerMediaSession::createNew(*_env,
                                        streamPath.c_str(),
                                        streamPath.c_str(),
                                        description);
    if (sms) {
        HimppServerMediaSubsession* h264smss;
        h264smss = HimppServerMediaSubsession::createNew(*_env, encoder);
        sms->addSubsession(h264smss);
    }

    return sms;
}

HimppMedia::~HimppMedia()
{
    if (_venc_chan0.isEnabled())
        _venc_chan0.disable();
    if (_venc_chan1.isEnabled())
        _venc_chan1.disable();
    _sysctl.disable();
}

std::list<HimppVideoSource*> HimppMedia::getVideoSourceList()
{
    std::list<HimppVideoSource*> l;
    l.push_back(&_video_source);

    return l;
}

std::list<HimppVideoEncoder*> HimppMedia::getVideoEncoderList()
{
    std::list<HimppVideoEncoder*> l;
    l.push_back(&_video_encoder0);
    l.push_back(&_video_encoder1);

    return l;
}

std::list<ServerMediaSession*> HimppMedia::getServerMediaSessionList()
{
    return _sms_list;
}
