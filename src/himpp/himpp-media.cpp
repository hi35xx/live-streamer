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


//////////////////////////////////////////////////////////////////////////////
// HimppVideoSource
//////////////////////////////////////////////////////////////////////////////

HimppVideoSource::HimppVideoSource(HimppMedia &media)
    : _media(media), _imaging(media)
{
}

HimppVideoSource::~HimppVideoSource()
{
}

void HimppVideoSource::setFramerate(uint32_t value)
{
    throw IpcamError("Readonly property");
}

uint32_t HimppVideoSource::getFramerate()
{
    return _media._vpss_chan.getFramerate();
}

ImageResolution HimppVideoSource::getResolution()
{
    return _media._vpss_chan.getResolution();
}

void HimppVideoSource::setResolution(ImageResolution &res)
{
    throw IpcamError("Readonly property");
}

IVideoSource::Imaging* HimppVideoSource::getImaging()
{
    return dynamic_cast<IVideoSource::Imaging*>(&_imaging);
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoSource::Imaging
//////////////////////////////////////////////////////////////////////////////

HimppVideoSource::Imaging::Imaging(HimppMedia &media)
    : _media(media), _exposure(media), _white_balance(media)
{
}

uint32_t HimppVideoSource::Imaging::getBrightness()
{
    return _media._vi_dev.getBrightness();
}

void HimppVideoSource::Imaging::setBrightness(uint32_t value)
{
    _media._vi_dev.setBrightness(value);
}

uint32_t HimppVideoSource::Imaging::getContrast()
{
    return _media._vi_dev.getContrast();
}

void HimppVideoSource::Imaging::setContrast(uint32_t value)
{
    _media._vi_dev.setContrast(value);
}

uint32_t HimppVideoSource::Imaging::getChroma()
{
    return _media._vi_dev.getChroma();
}

void HimppVideoSource::Imaging::setChroma(uint32_t value)
{
    _media._vi_dev.setChroma(value);
}

uint32_t HimppVideoSource::Imaging::getSaturation()
{
    return _media._vi_dev.getSaturation();
}

void HimppVideoSource::Imaging::setSaturation(uint32_t value)
{
    _media._vi_dev.setSaturation(value);
}

uint32_t HimppVideoSource::Imaging::getSharpness()
{
    return 0;
}

void HimppVideoSource::Imaging::setSharpness(uint32_t value)
{
    throw IpcamError("not implemented");
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
    return dynamic_cast<IVideoSource::Imaging::WhiteBalance*>(&_white_balance);
}

IVideoSource::Imaging::WideDynamicRange* HimppVideoSource::Imaging::getWideDynamicRange()
{
    return NULL;
}

IVideoSource::Imaging::LDC* HimppVideoSource::Imaging::getLDC()
{
    return NULL;
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoSource::Imaging::Exposure
//////////////////////////////////////////////////////////////////////////////

HimppVideoSource::Imaging::Exposure::Exposure(HimppMedia &media)
    : _media(media)
{
}

IVideoSource::Imaging::Exposure::ExposureMode
HimppVideoSource::Imaging::Exposure::getMode()
{
    HimppVideoISP *isp = (HimppVideoISP*)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->getExposureMode();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void
HimppVideoSource::Imaging::Exposure::setMode(IVideoSource::Imaging::Exposure::ExposureMode value)
{
    HimppVideoISP *isp = (HimppVideoISP*)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->setExposureMode(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}

IVideoSource::Imaging::Exposure::ExposurePriority
HimppVideoSource::Imaging::Exposure::getPriority()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->getExposurePriority();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void
HimppVideoSource::Imaging::Exposure::setPriority(IVideoSource::Imaging::Exposure::ExposurePriority value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->setExposurePriority(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMinExposureTime()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->getMinExposureTime();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMinExposureTime(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->setMinExposureTime(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMaxExposureTime()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->getMaxExposureTime();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMaxExposureTime(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->setMaxExposureTime(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMinGain()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->getMinGain();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMinGain(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->setMinGain(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMaxGain()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->getMaxGain();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMaxGain(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->setMaxGain(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMinIris()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->getMinIris();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMinIris(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->setMinIris(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMaxIris()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->getMaxIris();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMaxIris(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->setMaxIris(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getExposureTime()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->getExposureTime();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setExposureTime(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->setExposureTime(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getGain()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->getGain();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setGain(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->setGain(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getIris()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->getIris();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setIris(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::Exposure *exposure = isp->getExposure();
        if (exposure) {
            return exposure->setIris(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoSource::Imaging::WhiteBalance
//////////////////////////////////////////////////////////////////////////////
HimppVideoSource::Imaging::WhiteBalance::WhiteBalance(HimppMedia &media)
    : _media(media)
{
}

IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode
HimppVideoSource::Imaging::WhiteBalance::getMode()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
        if (wb) {
            return wb->getWBMode();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setMode(IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
        if (wb) {
            return wb->setWBMode(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getCbGain()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
        if (wb) {
            return wb->getCbGain();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setCbGain(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
        if (wb) {
            return wb->setCbGain(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getCrGain()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
        if (wb) {
            return wb->getCrGain();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setCrGain(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
        if (wb) {
            return wb->setCrGain(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getRGain()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
        if (wb) {
            return wb->getRGain();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setRGain(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
        if (wb) {
            return wb->setRGain(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getGGain()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
        if (wb) {
            return wb->getGGain();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setGGain(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
        if (wb) {
            return wb->setGGain(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getBGain()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
        if (wb) {
            return wb->getBGain();
        }
    }

    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setBGain(uint32_t value)
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev;
    if (isp) {
        HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
        if (wb) {
            return wb->setBGain(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoEncoder
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

void HimppVideoEncoder::setResolution(ImageResolution &resolution)
{
    if (!_venc_chan.setResolution(resolution))
        throw IpcamError("set resolution failed");
}

uint32_t  HimppVideoEncoder::getFramerate()
{
    return _venc_chan.getFramerate();
}

void HimppVideoEncoder::setFramerate(uint32_t fps)
{
    if (!_venc_chan.setFramerate(fps))
        throw IpcamError("set framerate failed");
}

IVideoEncoder::RateCtrlMode HimppVideoEncoder::getRcMode()
{
    return _venc_chan.getRcMode();
}

void HimppVideoEncoder::setRcMode(IVideoEncoder::RateCtrlMode mode)
{
    if (!_venc_chan.setRcMode(mode))
        throw IpcamError("set rate control mode failed");
}

uint32_t HimppVideoEncoder::getBitrate()
{
    return _venc_chan.getBitrate();
}

void HimppVideoEncoder::setBitrate(uint32_t kbps)
{
    if (!_venc_chan.setBitrate(kbps))
        throw IpcamError("set bitrate failed");
}

IH264VideoEncoder::H264Profile HimppVideoEncoder::getProfile()
{
    return _venc_chan.getProfile();
}

void HimppVideoEncoder::setProfile(IH264VideoEncoder::H264Profile profile)
{
    if (!_venc_chan.setH264Profile(profile))
        throw IpcamError("set h264 profile failed");
}

uint32_t  HimppVideoEncoder::getGovLength()
{
    return _venc_chan.getGop();
}

void HimppVideoEncoder::setGovLength(uint32_t gop)
{
    if (!_venc_chan.setGop(gop))
        throw IpcamError("set gop failed");
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
// HimppAudioSource
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
// HimppAudioEncoder
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

void HimppAudioEncoder::setEncoding(IAudioEncoder::EncodingType value)
{
    if (!_media._aenc_chan0.setEncoding(value))
        throw IpcamError("set audio encoding failed");
}

uint32_t HimppAudioEncoder::getBitrate()
{
    return _media._aenc_chan0.getBitrate();
}

void HimppAudioEncoder::setBitrate(uint32_t value)
{
    if (!_media._aenc_chan0.setBitrate(value))
        throw IpcamError("set audio bitrate failed");
}

uint32_t HimppAudioEncoder::getSampleRate()
{
    return _media._ai_dev0.getSampleRate();
}

void HimppAudioEncoder::setSampleRate(uint32_t value)
{
    if (!_media._ai_dev0.setSampleRate(value))
        throw IpcamError("set audio sample rate failed");
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
// HimppMedia
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
