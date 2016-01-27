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

HimppVideoSource
::HimppVideoSource(Hi3520mppMedia &media, HimppViDev &vi_dev, HimppViChan &vi_chan)
    : _media(media), _imaging(*this), _vi_dev(vi_dev), _vi_chan(vi_chan)
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

HimppVideoSource::Imaging::Imaging(HimppVideoSource &source)
    : _source(source), _exposure(source), _white_balance(source)
{
}

bool HimppVideoSource::Imaging::getMirror()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::setMirror(bool value)
{
    throw IpcamError("Requested interface not supported");
}

bool HimppVideoSource::Imaging::getFlip()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::setFlip(bool value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::getBrightness()
{
    return _source._vi_chan.getBrightness();
}

void HimppVideoSource::Imaging::setBrightness(uint32_t value)
{
    _source._vi_chan.setBrightness(value);
}

uint32_t HimppVideoSource::Imaging::getContrast()
{
    return _source._vi_chan.getContrast();
}

void HimppVideoSource::Imaging::setContrast(uint32_t value)
{
    _source._vi_chan.setContrast(value);
}

uint32_t HimppVideoSource::Imaging::getChroma()
{
    return _source._vi_chan.getChroma();
}

void HimppVideoSource::Imaging::setChroma(uint32_t value)
{
    _source._vi_chan.setChroma(value);
}

uint32_t HimppVideoSource::Imaging::getSaturation()
{
    return _source._vi_chan.getSaturation();
}

void HimppVideoSource::Imaging::setSaturation(uint32_t value)
{
    _source._vi_chan.setSaturation(value);
}

uint32_t HimppVideoSource::Imaging::getSharpness()
{
    return _source._vi_chan.getSharpness();
}

void HimppVideoSource::Imaging::setSharpness(uint32_t value)
{
    _source._vi_chan.setSharpness(value);
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

HimppVideoSource::Imaging::Exposure::Exposure(HimppVideoSource &source)
    : _source(source)
{
}

IVideoSource::Imaging::Exposure::ExposureMode
HimppVideoSource::Imaging::Exposure::getMode()
{
    throw IpcamError("Requested interface not supported");
}

void
HimppVideoSource::Imaging::Exposure::setMode(IVideoSource::Imaging::Exposure::ExposureMode value)
{
    throw IpcamError("Requested interface not supported");
}

IVideoSource::Imaging::Exposure::ExposurePriority
HimppVideoSource::Imaging::Exposure::getPriority()
{
    throw IpcamError("Requested interface not supported");
}

void
HimppVideoSource::Imaging::Exposure::setPriority(IVideoSource::Imaging::Exposure::ExposurePriority value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMinExposureTime()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMinExposureTime(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMaxExposureTime()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMaxExposureTime(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMinGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMinGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMaxGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMaxGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMinIris()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMinIris(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMaxIris()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMaxIris(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getExposureTime()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setExposureTime(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getIris()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setIris(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoSource::Imaging::WhiteBalance
//////////////////////////////////////////////////////////////////////////////
HimppVideoSource::Imaging::WhiteBalance::WhiteBalance(HimppVideoSource &source)
    : _source(source)
{
}

IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode
HimppVideoSource::Imaging::WhiteBalance::getMode()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setMode(IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getCbGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setCbGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getCrGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setCrGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getRGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setRGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getGGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setGGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getBGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setBGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}


//////////////////////////////////////////////////////////////////////////////
// HimppH264VideoEncoder
//////////////////////////////////////////////////////////////////////////////

HimppH264VideoEncoder::HimppH264VideoEncoder
(Hi3520mppMedia &media, HimppVencChan& venc_chan)
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

int HimppH264VideoEncoder::fileDescriptor()
{
    return HI_MPI_VENC_GetFd(_venc_chan.channelId());
}


//////////////////////////////////////////////////////////////////////////////
// HimppH264VideoStream
//////////////////////////////////////////////////////////////////////////////

HimppH264VideoStream::HimppH264VideoStream
(IpcamRuntime *runtime, HimppVencChan &venc_chan)
    : _runtime(runtime), _venc_chan(venc_chan),
      _io(runtime->mainloop())
{
}

HimppH264VideoStream::~HimppH264VideoStream()
{
}

IVideoEncoder::EncodingType HimppH264VideoStream::getEncoding()
{
    return _venc_chan.getEncoding();
}

uint32_t HimppH264VideoStream::getBitrate()
{
    return _venc_chan.getBitrate();
}

unsigned int HimppH264VideoStream
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
#define ARRAY_SIZE(x)		(sizeof(x)/sizeof(x[0]))
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

bool HimppH264VideoStream::startStreaming(LiveStreamDataPump *pump)
{
    _pump = pump;

    if (_venc_chan.isEnabled())
        return true;

    if (_venc_chan.enable()) {
        _io.set<HimppH264VideoStream, &HimppH264VideoStream::watch_handler>(this);
        _io.set(HI_MPI_VENC_GetFd(_venc_chan.channelId()), ev::READ);
        _io.start();

        return true;
    }
    return false;
}

bool HimppH264VideoStream::stopStreaming()
{
    _pump = NULL;

    _io.stop();

    if (!_venc_chan.isEnabled())
        return true;

    return _venc_chan.disable();
}

void HimppH264VideoStream::watch_handler(ev::io &w, int revents)
{
    if (revents & ev::READ && _pump)
        _pump->pushData();
}


//////////////////////////////////////////////////////////////////////////////
// Hi3520mppMedia
//////////////////////////////////////////////////////////////////////////////

Hi3520mppMedia
::Hi3520mppMedia(IpcamRuntime *runtime, std::string sensor_name)
    : _runtime(runtime),
      _vi_dev0(&_codec, 0),
      _vi_chan0(&_vi_dev0, &_codec, 0), _vi_chan1(&_vi_dev0, &_codec, 1),
      _vi_chan2(&_vi_dev0, &_codec, 2), _vi_chan3(&_vi_dev0, &_codec, 3),
      _vpss_group0(&_vi_chan0, 0), _vpss_group1(&_vi_chan1, 1),
      _vpss_group2(&_vi_chan1, 2), _vpss_group3(&_vi_chan3, 3),
      _vpss_chan0(&_vpss_group0, 0), _vpss_chan1(&_vpss_group1, 1),
      _vpss_chan2(&_vpss_group2, 2), _vpss_chan3(&_vpss_group3, 3),
      _video_stream0(runtime, _venc_chan0),
      _video_stream1(runtime, _venc_chan1),
      _video_stream2(runtime, _venc_chan2),
      _video_stream3(runtime, _venc_chan3),
      _venc_chan0(&_vpss_chan0, 0, 0),
      _venc_chan1(&_vpss_chan1, 1, 1),
      _venc_chan2(&_vpss_chan2, 2, 2),
      _venc_chan3(&_vpss_chan3, 3, 3),
      _video_source0(*this, _vi_dev0, _vi_chan0),
      _video_source1(*this, _vi_dev0, _vi_chan1),
      _video_source2(*this, _vi_dev0, _vi_chan2),
      _video_source3(*this, _vi_dev0, _vi_chan3),
      _video_encoder0(*this, _venc_chan0), _video_encoder1(*this, _venc_chan1),
      _video_encoder2(*this, _venc_chan2), _video_encoder3(*this, _venc_chan3)
{
    ImageResolution r0 = _venc_chan0.getResolution();
    ImageResolution r1 = _venc_chan1.getResolution();
    ImageResolution r2 = _venc_chan2.getResolution();
    ImageResolution r3 = _venc_chan3.getResolution();
    _venc_chan0.setEncoding(IVideoEncoder::H264);
    _venc_chan0.setResolution(r0);
    _venc_chan1.setEncoding(IVideoEncoder::H264);
    _venc_chan1.setResolution(r1);
    _venc_chan2.setEncoding(IVideoEncoder::H264);
    _venc_chan2.setResolution(r2);
    _venc_chan3.setEncoding(IVideoEncoder::H264);
    _venc_chan3.setResolution(r3);
    _sysctl.addVideoBuffer(r0.Width * r0.Height * 3 / 2, 4);
    _sysctl.addVideoBuffer(r1.Width * r1.Height * 3 / 2, 4);
    _sysctl.addVideoBuffer(r2.Width * r2.Height * 3 / 2, 4);
    _sysctl.addVideoBuffer(r3.Width * r3.Height * 3 / 2, 4);
    _sysctl.addVideoBuffer(196 * 4, 4);
    _sysctl.enable();

    // register rtsp streaming
    _runtime->addRTSPStream(&_video_stream0, NULL);
    _runtime->addRTSPStream(&_video_stream1, NULL);
    _runtime->addRTSPStream(&_video_stream2, NULL);
    _runtime->addRTSPStream(&_video_stream3, NULL);

    // register video source control interface
    _runtime->addVideoSourceInterface(&_video_source0);
    _runtime->addVideoSourceInterface(&_video_source1);
    _runtime->addVideoSourceInterface(&_video_source2);
    _runtime->addVideoSourceInterface(&_video_source3);

    // register video encoder control interface
    _runtime->addVideoEncoderInterface(&_video_encoder0);
    _runtime->addVideoEncoderInterface(&_video_encoder1);
    _runtime->addVideoEncoderInterface(&_video_encoder2);
    _runtime->addVideoEncoderInterface(&_video_encoder3);
}

Hi3520mppMedia::~Hi3520mppMedia()
{
    if (_venc_chan0.isEnabled())
        _venc_chan0.disable();
    if (_venc_chan1.isEnabled())
        _venc_chan1.disable();
    if (_venc_chan2.isEnabled())
        _venc_chan2.disable();
    if (_venc_chan3.isEnabled())
        _venc_chan3.disable();
    _sysctl.disable();
}
