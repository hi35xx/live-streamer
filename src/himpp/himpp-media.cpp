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
    : _media(media), _imaging(media)
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

IVideoSource::Imaging* HimppVideoSource::getImaging()
{
    return dynamic_cast<IVideoSource::Imaging*>(&_imaging);
}

HimppVideoSource::Imaging::Imaging(HimppMedia &media)
    : _media(media), _exposure(media)
{
}

int HimppVideoSource::Imaging::getBrightness()
{
    return _media._vi_dev.getBrightness();
}

bool HimppVideoSource::Imaging::setBrightness(int value)
{
    return _media._vi_dev.setBrightness(value);
}

int HimppVideoSource::Imaging::getContrast()
{
    return _media._vi_dev.getContrast();
}

bool HimppVideoSource::Imaging::setContrast(int value)
{
    return _media._vi_dev.setContrast(value);
}

int HimppVideoSource::Imaging::getChroma()
{
    return _media._vi_dev.getChroma();
}

bool HimppVideoSource::Imaging::setChroma(int value)
{
    return _media._vi_dev.setChroma(value);
}

int HimppVideoSource::Imaging::getSaturation()
{
    return _media._vi_dev.getSaturation();
}

bool HimppVideoSource::Imaging::setSaturation(int value)
{
    return _media._vi_dev.setSaturation(value);
}

int HimppVideoSource::Imaging::getSharpness()
{
    return 0;
}

bool HimppVideoSource::Imaging::setSharpness(int value)
{
    return false; //_media._vpss_group.setSharpness(value);
}

IVideoSource::Imaging::Backlight* HimppVideoSource::Imaging::getBacklight()
{
    return NULL;
}

IVideoSource::Imaging::Focus* HimppVideoSource::Imaging::getFocus()
{
    return NULL;
}

IVideoSource::Imaging::Exposure* HimppVideoSource::Imaging::getExposure()
{
    return dynamic_cast<IVideoSource::Imaging::Exposure*>(&_exposure);
}

IVideoSource::Imaging::WhiteBalance* HimppVideoSource::Imaging::getWhiteBalance()
{
    return NULL;
}

IVideoSource::Imaging::WideDynamicRange* HimppVideoSource::Imaging::getWideDynamicRange()
{
    return NULL;
}

IVideoSource::Imaging::LDC* HimppVideoSource::Imaging::getLDC()
{
    return NULL;
}



HimppVideoSource::Imaging::Exposure::Exposure(HimppMedia &media)
    : _media(media)
{
}

IVideoSource::Imaging::Exposure::ExposureMode
HimppVideoSource::Imaging::Exposure::getMode()
{
    return IVideoSource::Imaging::Exposure::AUTO_EXPOSURE;
}

bool
HimppVideoSource::Imaging::Exposure::setMode(IVideoSource::Imaging::Exposure::ExposureMode value)
{
    return false;
}

IVideoSource::Imaging::Exposure::ExposurePriority
HimppVideoSource::Imaging::Exposure::getPriority()
{
    return IVideoSource::Imaging::Exposure::LOWNOISE_PRIORITY;
}

bool
HimppVideoSource::Imaging::Exposure::setPriority(IVideoSource::Imaging::Exposure::ExposurePriority value)
{
    return false;
}

uint32_t HimppVideoSource::Imaging::Exposure::getMinExposureTime()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp)
        return isp->getMinExpTime();

    return (uint32_t)-1;
}

bool HimppVideoSource::Imaging::Exposure::setMinExposureTime(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp)
        return isp->setMinExpTime(value);

    return false;
}

uint32_t HimppVideoSource::Imaging::Exposure::getMaxExposureTime()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp)
        return isp->getMaxExpTime();

    return (uint32_t)-1;
}

bool HimppVideoSource::Imaging::Exposure::setMaxExposureTime(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp)
        return isp->setMaxExpTime(value);

    return false;
}

uint32_t HimppVideoSource::Imaging::Exposure::getMinGain()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp)
        return isp->getMinGain();

    return (uint32_t)-1;
}

bool HimppVideoSource::Imaging::Exposure::setMinGain(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp)
        return isp->setMinGain(value);

    return false;
}

uint32_t HimppVideoSource::Imaging::Exposure::getMaxGain()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp)
        return isp->getMaxGain();

    return (uint32_t)-1;
}

bool HimppVideoSource::Imaging::Exposure::setMaxGain(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp)
        return isp->setMaxGain(value);

    return false;
}

uint32_t HimppVideoSource::Imaging::Exposure::getExposureTime()
{
    return (uint32_t)-1;
}

bool HimppVideoSource::Imaging::Exposure::setExposureTime(uint32_t value)
{
    return false;
}

uint32_t HimppVideoSource::Imaging::Exposure::getGain()
{
    return (uint32_t)-1;
}

bool HimppVideoSource::Imaging::Exposure::setGain(uint32_t value)
{
    return false;
}

//////////////////////////////////////////////////////////////////////////////

HimppVideoEncoder::HimppVideoEncoder(HimppVencChan& venc_chan)
    : _venc_chan(venc_chan)
{
}

HimppVideoEncoder::~HimppVideoEncoder()
{
}

IVideoEncoder::EncodingType HimppVideoEncoder::getEncoding()
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

IVideoEncoder::RateCtrlMode HimppVideoEncoder::getRcMode()
{
    return _venc_chan.getRcMode();
}

bool HimppVideoEncoder::setRcMode(IVideoEncoder::RateCtrlMode mode)
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

IH264VideoEncoder::H264Profile HimppVideoEncoder::getProfile()
{
    return _venc_chan.getProfile();
}

bool HimppVideoEncoder::setProfile(IH264VideoEncoder::H264Profile profile)
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
    if (_venc_chan.isEnabled())
        return true;

    return _venc_chan.enable();
}

bool HimppVideoEncoder::disable()
{
    if (!_venc_chan.isEnabled())
        return true;

    return _venc_chan.disable();
}


//////////////////////////////////////////////////////////////////////////////

HimppAudioSource::HimppAudioSource(HimppMedia &media)
    : _media(media)
{
}

HimppAudioSource::~HimppAudioSource()
{
}

uint32_t HimppAudioSource::getChannels()
{
    return 1;
}


//////////////////////////////////////////////////////////////////////////////

HimppAudioEncoder
::HimppAudioEncoder(HimppMedia& media, HimppAencChan& aenc_chan)
    : _media(media), _aenc_chan(aenc_chan)
{
}

HimppAudioEncoder::~HimppAudioEncoder()
{
}

int HimppAudioEncoder::fileDescriptor()
{
    return HI_MPI_AENC_GetFd(_aenc_chan.channelId());
}

IAudioEncoder::EncodingType
HimppAudioEncoder::getEncoding()
{
    return _media._aenc_chan0.getEncoding();
}

bool
HimppAudioEncoder::setEncoding(IAudioEncoder::EncodingType value)
{
    return _media._aenc_chan0.setEncoding(value);
}

uint32_t HimppAudioEncoder::getBitrate()
{
    return _media._aenc_chan0.getBitrate();
}

bool HimppAudioEncoder::setBitrate(uint32_t value)
{
    return _media._aenc_chan0.setBitrate(value);
}

uint32_t HimppAudioEncoder::getSampleRate()
{
    return _media._ai_dev0.getSampleRate();
}

bool HimppAudioEncoder::setSampleRate(uint32_t value)
{
    return _media._ai_dev0.setSampleRate(value);
}

bool HimppAudioEncoder::enable()
{
    if (_aenc_chan.isEnabled())
        return true;

    return _aenc_chan.enable();
}

bool HimppAudioEncoder::disable()
{
    if (!_aenc_chan.isEnabled())
        return true;

    return _aenc_chan.disable();
}


//////////////////////////////////////////////////////////////////////////////

HimppMedia::HimppMedia(UsageEnvironment *env, std::string sensor_name)
    : _env(env),
      _sensor(himpp_video_sensor_map.at(sensor_name)),
      _vi_dev(&_sensor, 0), _vi_chan(&_vi_dev, 0),
      _vpss_group(&_vi_chan, 0), _vpss_chan(&_vpss_group, 0),
      _venc_chan0(dynamic_cast<HimppVideoObject*>(&_vpss_chan), 0, 0),
      _venc_chan1(dynamic_cast<HimppVideoObject*>(&_vpss_chan), 1, 1),
      _acodec(), _ai_dev0(&_acodec, 0), _ai_chan0(&_ai_dev0, 0), _aenc_chan0(&_ai_chan0, 0),
      _video_source0(*this), _video_encoder0(_venc_chan0), _video_encoder1(_venc_chan1),
      _audio_source0(*this), _audio_encoder0(*this, _aenc_chan0)
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

    _sms_list.push_back(addSMS("0", &_video_encoder0, &_audio_encoder0));
    _sms_list.push_back(addSMS("1", &_video_encoder1, NULL));
}

ServerMediaSession *HimppMedia::addSMS(std::string stream_path,
                                       HimppVideoEncoder* vencoder,
                                       HimppAudioEncoder* aencoder)
{
    char const* description = "RTSP/RTP stream from live-streamer";
    ServerMediaSession *sms;
    sms = ServerMediaSession::createNew(*_env,
                                        stream_path.c_str(),
                                        stream_path.c_str(),
                                        description);
    if (sms) {
        if (vencoder) {
            HimppVideoServerMediaSubsession* video_smss;
            video_smss = HimppVideoServerMediaSubsession::createNew(*_env, *vencoder);
            sms->addSubsession(video_smss);
        }
        if (aencoder) {
            HimppAudioServerMediaSubsession* audio_smss;
            audio_smss = HimppAudioServerMediaSubsession::createNew(*_env, *aencoder);
            sms->addSubsession(audio_smss);
        }
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
    l.push_back(&_video_source0);

    return l;
}

std::list<HimppVideoEncoder*> HimppMedia::getVideoEncoderList()
{
    std::list<HimppVideoEncoder*> l;
    l.push_back(&_video_encoder0);
    l.push_back(&_video_encoder1);

    return l;
}

std::list<HimppAudioSource*> HimppMedia::getAudioSourceList()
{
    std::list<HimppAudioSource*> l;
    l.push_back(&_audio_source0);

    return l;
}

std::list<HimppAudioEncoder*> HimppMedia::getAudioEncoderList()
{
    std::list<HimppAudioEncoder*> l;
    l.push_back(&_audio_encoder0);

    return l;
}

std::list<ServerMediaSession*> HimppMedia::getServerMediaSessionList()
{
    return _sms_list;
}
