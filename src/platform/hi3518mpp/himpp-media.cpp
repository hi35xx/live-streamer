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

#include "himpp-media.h"


//////////////////////////////////////////////////////////////////////////////
// HimppVideoSource
//////////////////////////////////////////////////////////////////////////////

HimppVideoSource::HimppVideoSource(Hi3518mppMedia &media)
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
    return _media._vpss_chan0.getFramerate();
}

ImageResolution HimppVideoSource::getResolution()
{
    return _media._vpss_chan0.getResolution();
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

HimppVideoSource::Imaging::Imaging(Hi3518mppMedia &media)
    : _media(media), _exposure(media), _white_balance(media)
{
}

bool HimppVideoSource::Imaging::getMirror()
{
    return _media._vi_chan0.getMirror();
}

void HimppVideoSource::Imaging::setMirror(bool value)
{
    return _media._vi_chan0.setMirror(value);
}

bool HimppVideoSource::Imaging::getFlip()
{
    return _media._vi_chan0.getFlip();
}

void HimppVideoSource::Imaging::setFlip(bool value)
{
    return _media._vi_chan0.setFlip(value);
}

uint32_t HimppVideoSource::Imaging::getBrightness()
{
    return _media._vi_dev0.getBrightness();
}

void HimppVideoSource::Imaging::setBrightness(uint32_t value)
{
    _media._vi_dev0.setBrightness(value);
}

uint32_t HimppVideoSource::Imaging::getContrast()
{
    return _media._vi_dev0.getContrast();
}

void HimppVideoSource::Imaging::setContrast(uint32_t value)
{
    _media._vi_dev0.setContrast(value);
}

uint32_t HimppVideoSource::Imaging::getChroma()
{
    return _media._vi_dev0.getChroma();
}

void HimppVideoSource::Imaging::setChroma(uint32_t value)
{
    _media._vi_dev0.setChroma(value);
}

uint32_t HimppVideoSource::Imaging::getSaturation()
{
    return _media._vi_dev0.getSaturation();
}

void HimppVideoSource::Imaging::setSaturation(uint32_t value)
{
    _media._vi_dev0.setSaturation(value);
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

HimppVideoSource::Imaging::Exposure::Exposure(Hi3518mppMedia &media)
    : _media(media)
{
}

IVideoSource::Imaging::Exposure::ExposureMode
HimppVideoSource::Imaging::Exposure::getMode()
{
    HimppVideoISP *isp = (HimppVideoISP*)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP*)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
HimppVideoSource::Imaging::WhiteBalance::WhiteBalance(Hi3518mppMedia &media)
    : _media(media)
{
}

IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode
HimppVideoSource::Imaging::WhiteBalance::getMode()
{
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
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
    HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
    if (isp) {
        HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
        if (wb) {
            return wb->setBGain(value);
        }
    }

    throw IpcamError("Requested interface not supported");
}


//////////////////////////////////////////////////////////////////////////////
// HimppH264VideoEncoder
//////////////////////////////////////////////////////////////////////////////

HimppH264VideoEncoder::HimppH264VideoEncoder
(Hi3518mppMedia &media, HimppVencChan& venc_chan)
    : _media(media), _venc_chan(venc_chan)
{
}

HimppH264VideoEncoder::~HimppH264VideoEncoder()
{
}

IVideoEncoder::EncodingType HimppH264VideoEncoder::getEncoding()
{
    return _venc_chan.getEncoding();
}

ImageResolution HimppH264VideoEncoder::getResolution()
{
    return _venc_chan.getResolution();
}

void HimppH264VideoEncoder::setResolution(ImageResolution &resolution)
{
    if (!_venc_chan.setResolution(resolution))
        throw IpcamError("set resolution failed");
}

uint32_t  HimppH264VideoEncoder::getFramerate()
{
    return _venc_chan.getFramerate();
}

void HimppH264VideoEncoder::setFramerate(uint32_t fps)
{
    if (!_venc_chan.setFramerate(fps))
        throw IpcamError("set framerate failed");
}

IVideoEncoder::RateCtrlMode HimppH264VideoEncoder::getRcMode()
{
    return _venc_chan.getRcMode();
}

void HimppH264VideoEncoder::setRcMode(IVideoEncoder::RateCtrlMode mode)
{
    if (!_venc_chan.setRcMode(mode))
        throw IpcamError("set rate control mode failed");
}

uint32_t HimppH264VideoEncoder::getBitrate()
{
    return _venc_chan.getBitrate();
}

void HimppH264VideoEncoder::setBitrate(uint32_t kbps)
{
    if (!_venc_chan.setBitrate(kbps))
        throw IpcamError("set bitrate failed");
}

IH264VideoEncoder::H264Profile HimppH264VideoEncoder::getProfile()
{
    return _venc_chan.getProfile();
}

void HimppH264VideoEncoder::setProfile(IH264VideoEncoder::H264Profile profile)
{
    if (!_venc_chan.setH264Profile(profile))
        throw IpcamError("set h264 profile failed");
}

uint32_t  HimppH264VideoEncoder::getGovLength()
{
    return _venc_chan.getGop();
}

void HimppH264VideoEncoder::setGovLength(uint32_t gop)
{
    if (!_venc_chan.setGop(gop))
        throw IpcamError("set gop failed");
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoStream
//////////////////////////////////////////////////////////////////////////////

HimppVideoStream::HimppVideoStream
(IpcamRuntime *runtime, HimppVencChan &venc_chan)
    : _runtime(runtime), _venc_chan(venc_chan),
      _io(runtime->mainloop())
{
}

HimppVideoStream::~HimppVideoStream()
{
}

IVideoEncoder::EncodingType HimppVideoStream::getEncoding()
{
    return _venc_chan.getEncoding();
}

uint32_t HimppVideoStream::getBitrate()
{
    return _venc_chan.getBitrate();
}

unsigned int HimppVideoStream
::getStreamData(unsigned char* buf, unsigned int bufsiz,
                unsigned int &truncated, struct timeval &tstamp)
{
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;
    unsigned int frame_size;
    HI_S32 s32Ret;

    int chnid = _venc_chan.channelId();

    s32Ret = HI_MPI_VENC_Query(chnid, &stStat);
    if (HI_SUCCESS != s32Ret || !stStat.u32CurPacks) {
        return 0;
    }

    stStream.pstPack = (VENC_PACK_S*)alloca(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
    stStream.u32PackCount = stStat.u32CurPacks;
    stStream.u32Seq = 0;
    memset(&stStream.stH264Info, 0, sizeof(VENC_STREAM_INFO_H264_S));
    s32Ret = HI_MPI_VENC_GetStream(chnid, &stStream, HI_FALSE);
    if (HI_SUCCESS != s32Ret) {
        fprintf(stderr, "HI_MPI_VENC_GetStream %d failed [%#x]\n",
                chnid, s32Ret);
        return 0;
    }

    gettimeofday(&tstamp, NULL);

    frame_size = 0;
    for (int i = 0; i < stStream.u32PackCount; i++) {
        for (int j = 0; j < ARRAY_SIZE(stStream.pstPack[i].pu8Addr); j++) {
            HI_U8 *p = stStream.pstPack[i].pu8Addr[j];
            HI_U32 len = stStream.pstPack[i].u32Len[j];

            if (len == 0)
                continue;

            if (len >= 3 && p[0] == 0x00 && p[1] == 0x00 && p[2] == 0x01) {
                p += 3;
                len -= 3;
            }
            if (len >= 4 && p[0] == 0x00 && p[1] == 0x00 && p[2] == 0x00 && p[3] == 0x01) {
                p += 4;
                len -= 4;
            }

            if (frame_size + len < bufsiz) {
                memmove(&buf[frame_size], p, len);
                frame_size += len;
            }
            else {
                truncated += len;
            }
        }
    }

    s32Ret = HI_MPI_VENC_ReleaseStream(chnid, &stStream);
    if (HI_SUCCESS != s32Ret) {
        fprintf(stderr, "HI_MPI_VENC_ReleaseStream failed [%#x]\n",
                s32Ret);
    }

    return frame_size;
}

bool HimppVideoStream::startStreaming(LiveStreamDataPump *pump)
{
    _pump = pump;

    if (_venc_chan.isEnabled())
        return true;

    if (_venc_chan.enable()) {
        _io.set<HimppVideoStream, &HimppVideoStream::watch_handler>(this);
        _io.set(HI_MPI_VENC_GetFd(_venc_chan.channelId()), ev::READ);
        _io.start();

        return true;
    }
    return false;
}

bool HimppVideoStream::stopStreaming()
{
    _pump = NULL;

    _io.stop();

    if (!_venc_chan.isEnabled())
        return true;

    return _venc_chan.disable();
}

void HimppVideoStream::watch_handler(ev::io &w, int revents)
{
    if (revents & ev::READ && _pump) {
        _pump->pushData();
    }
}


//////////////////////////////////////////////////////////////////////////////
// HimppAudioSource
//////////////////////////////////////////////////////////////////////////////

HimppAudioSource::HimppAudioSource(Hi3518mppMedia &media)
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
::HimppAudioEncoder(Hi3518mppMedia& media, HimppAencChan& aenc_chan)
    : _media(media), _aenc_chan(aenc_chan)
{
}

HimppAudioEncoder::~HimppAudioEncoder()
{
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
// HimppAudioStream
//////////////////////////////////////////////////////////////////////////////

HimppAudioStream::HimppAudioStream
(IpcamRuntime *runtime, HimppAiDev &ai_dev, HimppAencChan &aenc_chan)
    : _runtime(runtime), _ai_dev(ai_dev), _aenc_chan(aenc_chan),
      _io(runtime->mainloop())
{
}

HimppAudioStream::~HimppAudioStream()
{
}

IAudioEncoder::EncodingType HimppAudioStream::getEncoding()
{
    return _aenc_chan.getEncoding();
}

uint32_t HimppAudioStream::getSampleRate()
{
    return _ai_dev.getSampleRate();
}

unsigned int HimppAudioStream
::getStreamData(unsigned char* buf, unsigned int bufsiz,
                unsigned int &truncated, struct timeval &tstamp)
{
    AUDIO_STREAM_S stStream;
    unsigned int frame_size;
    HI_S32 s32Ret;

    int chnid = _aenc_chan.channelId();

    s32Ret = HI_MPI_AENC_GetStream(chnid, &stStream, HI_FALSE);
    if (HI_SUCCESS != s32Ret) {
        fprintf(stderr, "HI_MPI_AENC_GetStream %d failed [%#x]\n",
                chnid, s32Ret);
        return 0;
    }

    gettimeofday(&tstamp, NULL);

    frame_size = 0;
    if (stStream.u32Len <= bufsiz + 4) {
        frame_size = stStream.u32Len - 4;
        truncated = 0;
    }
    else {
        frame_size = bufsiz;
        truncated = stStream.u32Len - 4 - bufsiz;
    }
    memmove(&buf[0], stStream.pStream + 4, frame_size);

    s32Ret = HI_MPI_AENC_ReleaseStream(chnid, &stStream);
    if (HI_SUCCESS != s32Ret) {
        fprintf(stderr, "HI_MPI_AENC_ReleaseStream failed [%#x]\n", s32Ret);
    }

    return frame_size;
}

bool HimppAudioStream::startStreaming(LiveStreamDataPump *pump)
{
    _pump = pump;

    if (_aenc_chan.isEnabled())
        return true;

    if (_aenc_chan.enable()) {
        _io.set<HimppAudioStream, &HimppAudioStream::watch_handler>(this);
        _io.set(HI_MPI_AENC_GetFd(_aenc_chan.channelId()), ev::READ);
        _io.start();

        return true;
    }
    return false;
}

bool HimppAudioStream::stopStreaming()
{
    _pump = NULL;

    _io.stop();

    if (!_aenc_chan.isEnabled())
        return true;

    return _aenc_chan.disable();
}

void HimppAudioStream::watch_handler(ev::io &w, int revents)
{
    if (revents & ev::READ && _pump) {
        _pump->pushData();
    }
}


//////////////////////////////////////////////////////////////////////////////
// Hi3518mppMedia
//////////////////////////////////////////////////////////////////////////////

Hi3518mppMedia::Hi3518mppMedia(IpcamRuntime *runtime, std::string sensor_name)
    : _runtime(runtime),
      _sensor(himpp_video_sensor_map.at(sensor_name)),
      _vi_dev0(&_sensor, 0), _vi_chan0(&_vi_dev0, 0),
      _vpss_group0(&_vi_chan0, 0),
      _vpss_chan0(&_vpss_group0, 0),
      _venc_chan0(dynamic_cast<HimppVideoObject*>(&_vpss_chan0), 0, 0),
      _venc_chan1(dynamic_cast<HimppVideoObject*>(&_vpss_chan0), 1, 1),
      _acodec(), _ai_dev0(&_acodec, 0), _ai_chan0(&_ai_dev0, 0),
      _aenc_chan0(&_ai_chan0, 0), _aenc_chan1(&_ai_chan0, 1),
      _video_stream0(runtime, _venc_chan0),
      _video_stream1(runtime, _venc_chan1),
      _audio_stream0(runtime, _ai_dev0, _aenc_chan0),
      _audio_stream1(runtime, _ai_dev0, _aenc_chan1),
      _video_source0(*this),
      _video_encoder0(*this, _venc_chan0),
      _video_encoder1(*this, _venc_chan1),
      _audio_source0(*this),
      _audio_encoder0(*this, _aenc_chan0),
      _audio_encoder1(*this, _aenc_chan1)
{
    ImageResolution ri = _vi_dev0.getResolution();
    ImageResolution r0 = _venc_chan0.getResolution();
    ImageResolution r1(640, 360);
    _venc_chan0.setEncoding(IVideoEncoder::H264);
    //_venc_chan0.setFramerate(20);
    _venc_chan1.setEncoding(IVideoEncoder::H264);
    _venc_chan1.setResolution(r1);
    //_venc_chan1.setFramerate(15);
    _sysctl.addVideoBuffer(ri.Width * ri.Height * 3 / 2, 4);
    _sysctl.addVideoBuffer(r0.Width * r0.Height * 3 / 2, 2);
    _sysctl.addVideoBuffer(r1.Width * r1.Height * 3 / 2, 2);
    _sysctl.addVideoBuffer(196 * 4, 4);
    _sysctl.enable();

    runtime->addRTSPStream(&_video_stream0, &_audio_stream0);
    runtime->addRTSPStream(&_video_stream1, &_audio_stream1);

    runtime->addVideoSourceInterface(&_video_source0);

    runtime->addVideoEncoderInterface(&_video_encoder0);
    runtime->addVideoEncoderInterface(&_video_encoder1);

    runtime->addAudioSourceInterface(&_audio_source0);
    runtime->addAudioEncoderInterface(&_audio_encoder0);
    runtime->addAudioEncoderInterface(&_audio_encoder1);
}

Hi3518mppMedia::~Hi3518mppMedia()
{
    if (_venc_chan0.isEnabled())
        _venc_chan0.disable();
    if (_venc_chan1.isEnabled())
        _venc_chan1.disable();
    _sysctl.disable();
}
