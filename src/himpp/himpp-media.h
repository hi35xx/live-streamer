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
#include <himpp-audio.h>

#include <MediaSink.hh>
#include <OnDemandServerMediaSubsession.hh>

#include <ipcam-media.h>

using namespace Ipcam::Interface;

class HimppMedia;

class HimppVideoSource : public IVideoSource
{
public:
    class Imaging : public IVideoSource::Imaging
    {
    public:
        class Exposure : public IVideoSource::Imaging::Exposure
        {
        public:
            typedef IVideoSource::Imaging::Exposure::ExposureMode ExposureMode;
            typedef IVideoSource::Imaging::Exposure::ExposurePriority ExposurePriority;
        public:
            Exposure(HimppMedia &media);

            ExposureMode getMode();
            void        setMode(ExposureMode value);
            ExposurePriority getPriority();
            void        setPriority(ExposurePriority value);
            uint32_t    getMinExposureTime();
            void        setMinExposureTime(uint32_t value);
            uint32_t    getMaxExposureTime();
            void        setMaxExposureTime(uint32_t value);
            uint32_t    getMinGain();
            void        setMinGain(uint32_t value);
            uint32_t    getMaxGain();
            void        setMaxGain(uint32_t value);
            uint32_t    getMinIris();
            void        setMinIris(uint32_t value);
            uint32_t    getMaxIris();
            void        setMaxIris(uint32_t value);
            uint32_t    getExposureTime();
            void        setExposureTime(uint32_t value);
            uint32_t    getGain();
            void        setGain(uint32_t value);
            uint32_t    getIris();
            void        setIris(uint32_t value);
        private:
            HimppMedia& _media;
        };
        class WhiteBalance : public IVideoSource::Imaging::WhiteBalance
        {
        public:
            typedef IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode WhiteBalanceMode;
        public:
            WhiteBalance(HimppMedia &media);

            WhiteBalanceMode getMode();
            void        setMode(WhiteBalanceMode value);
            uint32_t    getCbGain();
            void        setCbGain(uint32_t value);
            uint32_t    getCrGain();
            void        setCrGain(uint32_t value);
            uint32_t    getRGain();
            void        setRGain(uint32_t value);
            uint32_t    getGGain();
            void        setGGain(uint32_t value);
            uint32_t    getBGain();
            void        setBGain(uint32_t value);
        private:
            HimppMedia& _media;
        };
    public:
        Imaging(HimppMedia &media);

        uint32_t    getBrightness();
        void        setBrightness(uint32_t value);
        uint32_t    getContrast();
        void        setContrast(uint32_t value);
        uint32_t    getChroma();
        void        setChroma(uint32_t value);
        uint32_t    getSaturation();
        void        setSaturation(uint32_t value);
        uint32_t    getSharpness();
        void        setSharpness(uint32_t value);

        IVideoSource::Imaging::Backlight*        getBacklight();
        IVideoSource::Imaging::Focus*            getFocus();
        IVideoSource::Imaging::Exposure*         getExposure();
        IVideoSource::Imaging::WhiteBalance*     getWhiteBalance();
        IVideoSource::Imaging::WideDynamicRange* getWideDynamicRange();
        IVideoSource::Imaging::LDC*              getLDC();
    private:
        HimppMedia&     _media;
        Exposure        _exposure;
        WhiteBalance    _white_balance;
    };
public:
    HimppVideoSource(HimppMedia &media);
    ~HimppVideoSource();

    // video source method
    void        setFramerate(uint32_t value);
    uint32_t    getFramerate();
    void        setResolution(ImageResolution &res);
    ImageResolution getResolution();
    // imaging method
    IVideoSource::Imaging*    getImaging();
private:
    HimppMedia&     _media;
    Imaging         _imaging;
};

class HimppVideoEncoder : public IH264VideoEncoder
{
public:
    HimppVideoEncoder(HimppVencChan &venc_chan);
    ~HimppVideoEncoder();

    // video encoder method
    IVideoEncoder::EncodingType getEncoding();
    ImageResolution getResolution();
    void setResolution(ImageResolution &resolution);
    uint32_t  getFramerate();
    void setFramerate(uint32_t fps);
    // h264 method
    IVideoEncoder::RateCtrlMode getRcMode();
    void setRcMode(IVideoEncoder::RateCtrlMode mode);
    uint32_t  getBitrate();
    void setBitrate(uint32_t kbps);
    IH264VideoEncoder::H264Profile getProfile();
    void setProfile(IH264VideoEncoder::H264Profile profile);
    uint32_t  getGovLength();
    void setGovLength(uint32_t gop);
    // local methods
    VENC_GRP groupId() { return _venc_chan.groupId(); }
    VENC_CHN channelId() { return _venc_chan.channelId(); }
    int fileDescriptor();

    bool enable();
    bool disable();
private:
    HimppVencChan&      _venc_chan;
};


class HimppAudioSource : public IAudioSource
{
public:
    HimppAudioSource(HimppMedia &media);
    ~HimppAudioSource();

    uint32_t        getChannels();
private:
    HimppMedia&     _media;
};

class HimppAudioEncoder : public IAudioEncoder
{
public:
    HimppAudioEncoder(HimppMedia &media, HimppAencChan &aenc_chan);
    ~HimppAudioEncoder();

    AENC_CHN channelId() { return _aenc_chan.channelId(); }
    int fileDescriptor();

    IAudioEncoder::EncodingType getEncoding();
    void                setEncoding(IAudioEncoder::EncodingType value);
    uint32_t            getBitrate();
    void                setBitrate(uint32_t value);
    uint32_t            getSampleRate();
    void                setSampleRate(uint32_t value);

    bool enable();
    bool disable();
private:
    HimppMedia&        _media;
    HimppAencChan&     _aenc_chan;
};


class HimppMedia
{
public:
    HimppMedia(UsageEnvironment*, std::string);
    ~HimppMedia();

    // dbus interface
    std::list<HimppVideoSource*> getVideoSourceList();
    std::list<HimppVideoEncoder*> getVideoEncoderList();
    std::list<HimppAudioSource*> getAudioSourceList();
    std::list<HimppAudioEncoder*> getAudioEncoderList();
    // live555 interface
    std::list<ServerMediaSession*> getServerMediaSessionList();
private:
    UsageEnvironment*   _env;
    std::list<ServerMediaSession*> _sms_list;

    // Video Objects
    HimppSysctl         _sysctl;
    HimppVideoSensor&   _sensor;
    HimppViDev          _vi_dev;
    HimppViChan         _vi_chan;
    HimppVpssGroup      _vpss_group;
    HimppVpssChan       _vpss_chan;
    HimppVencChan       _venc_chan0;
    HimppVencChan       _venc_chan1;

    // Audio Objects
    HimppAudioCodec     _acodec;
    HimppAiDev          _ai_dev0;
    HimppAiChan         _ai_chan0;
    HimppAencChan       _aenc_chan0;

    friend class HimppVideoSource;
    friend class HimppVideoEncoder;
    friend class HimppAudioSource;
    friend class HimppAudioEncoder;

    HimppVideoSource    _video_source0;
    HimppVideoEncoder   _video_encoder0;
    HimppVideoEncoder   _video_encoder1;

    HimppAudioSource    _audio_source0;
    HimppAudioEncoder   _audio_encoder0;

    ServerMediaSession* addSMS(std::string stream_path,
                               HimppVideoEncoder* v_encoder,
                               HimppAudioEncoder* a_encoder);
};

#endif // _HIMPP_MEDIA_H_
