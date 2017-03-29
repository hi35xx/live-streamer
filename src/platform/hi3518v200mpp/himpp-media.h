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

#include <bitset>

#include <himpp-sysctl.h>
#include <himpp-video-sensor.h>
#include <himpp-video-isp.h>
#include <himpp-video-viu.h>
#include <himpp-video-vpss.h>
#include <himpp-video-venc.h>
#include <himpp-audio.h>

#include <ipcam-media-iface.h>
#include <ipcam-runtime.h>

using namespace Ipcam::Media;

class Hi3518mppMedia;

class HimppVideoSource : public IVideoSource
{
public:
    class Imaging : public IVideoSource::Imaging
    {
    public:
        class AntiFlicker : public IVideoSource::Imaging::AntiFlicker
        {
        public:
            typedef IVideoSource::Imaging::AntiFlicker::AntiFlickerMode AntiFlickerMode;
        public:
            AntiFlicker(Hi3518mppMedia &media);

            AntiFlickerMode getMode();
            void            setMode(AntiFlickerMode value);
            uint32_t        getFrequency();
            void            setFrequency(uint32_t value);
        private:
            Hi3518mppMedia& _media;
        };
        class Exposure : public IVideoSource::Imaging::Exposure
        {
        public:
            typedef IVideoSource::Imaging::Exposure::ExposureMode ExposureMode;
            typedef IVideoSource::Imaging::Exposure::ExposurePriority ExposurePriority;
        public:
            Exposure(Hi3518mppMedia &media);

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
            Hi3518mppMedia& _media;
        };
        class WhiteBalance : public IVideoSource::Imaging::WhiteBalance
        {
        public:
            typedef IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode WhiteBalanceMode;
        public:
            WhiteBalance(Hi3518mppMedia &media);

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
            Hi3518mppMedia& _media;
        };
        class WideDynamicRange : public IVideoSource::Imaging::WideDynamicRange
        {
        public:
            typedef IVideoSource::Imaging::WideDynamicRange::WDRMode WDRMode;
        public:
            WideDynamicRange(Hi3518mppMedia &media);

            WDRMode     getMode();
            void        setMode(WDRMode value);
            uint32_t    getLevel();
            void        setLevel(uint32_t value);
        private:
            Hi3518mppMedia& _media;
        };
        class LDC : public IVideoSource::Imaging::LDC
        {
        public:
            typedef IVideoSource::Imaging::LDC::LDCMode LDCMode;
        public:
            LDC(Hi3518mppMedia &media);

            LDCMode     getMode();
            void        setMode(LDCMode value);
            uint32_t    getRatio();
            void        setRatio(uint32_t value);
        private:
            Hi3518mppMedia& _media;
        };
        class Gamma : public IVideoSource::Imaging::Gamma
        {
        public:
            Gamma(Hi3518mppMedia &media);

            std::vector<uint32_t>& getCurveData();
            void            setCurveData(std::vector<uint32_t>& value);
        private:
            Hi3518mppMedia& _media;
        };
    public:
        Imaging(Hi3518mppMedia &media);

        bool        getMirror();
        void        setMirror(bool value);
        bool        getFlip();
        void        setFlip(bool value);
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
        IVideoSource::Imaging::IrCutFilterMode getIrCutFilterMode();
        void        setIrCutFilterMode(IVideoSource::Imaging::IrCutFilterMode value);

        IVideoSource::Imaging::AntiFlicker&      getAntiFlicker();
        IVideoSource::Imaging::Backlight&        getBacklight();
        IVideoSource::Imaging::Focus&            getFocus();
        IVideoSource::Imaging::Exposure&         getExposure();
        IVideoSource::Imaging::WhiteBalance&     getWhiteBalance();
        IVideoSource::Imaging::WideDynamicRange& getWideDynamicRange();
        IVideoSource::Imaging::LDC&              getLDC();
        IVideoSource::Imaging::Gamma&            getGamma();
    private:
        Hi3518mppMedia&     _media;
        AntiFlicker         _anti_flicker;
        Exposure            _exposure;
        WhiteBalance        _white_balance;
        WideDynamicRange    _wide_dynamic_range;
        LDC                 _ldc;
        Gamma               _gamma;
    };
public:
    HimppVideoSource(Hi3518mppMedia &media);
    ~HimppVideoSource();

    // video source method
    void        setFramerate(uint32_t value);
    uint32_t    getFramerate();
    void        setResolution(ImageResolution &res);
    ImageResolution getResolution();
    // imaging method
    IVideoSource::Imaging&    getImaging();
private:
    Hi3518mppMedia&     _media;
    Imaging         _imaging;
};

class HimppVideoOSD : public IVideoOSD
{
public:
    HimppVideoOSD(Hi3518mppMedia &media, HimppVencChan &venc_chan, uint32_t id);
    ~HimppVideoOSD();

    bool getVisible();
    void setVisible(bool val);
    Position getPosition();
    void setPosition(Position pos);
    Size getSize();
    void setSize(Size size);
    uint32_t getForegroundColor();
    void setForegroundColor(uint32_t val);
    uint32_t getBackgroundColor();
    void setBackgroundColor(uint32_t val);
    uint32_t getForegroundAlpha();
    void setForegroundAlpha(uint32_t val);
    uint32_t getBackgroundAlpha();
    void setBackgroundAlpha(uint32_t val);
    bool getInvertColor();
    void setInvertColor(bool val);

    SDL_Surface *getSurface(uint16_t w, uint16_t h);
    void putSurface(SDL_Surface *surf);
private:
    Hi3518mppMedia &_media;
    HimppVencChan &_venc_chan;
    HimppVideoRegion _region;
    SDL_Surface *_surface;
};

class HimppH264VideoEncoder : public IH264VideoEncoder
{
public:
    HimppH264VideoEncoder(Hi3518mppMedia &media, HimppVencChan &venc_chan);
    ~HimppH264VideoEncoder();

    // video encoder method
    IVideoEncoder::EncodingType getEncoding();
    ImageResolution getResolution();
    void setResolution(ImageResolution &resolution);
    uint32_t  getFramerate();
    void setFramerate(uint32_t fps);
    IVideoEncoder::RateCtrlMode getRcMode();
    void setRcMode(IVideoEncoder::RateCtrlMode mode);
    uint32_t  getBitrate();
    void setBitrate(uint32_t kbps);
    // h264 method
    IH264VideoEncoder::H264Profile getProfile();
    void setProfile(IH264VideoEncoder::H264Profile profile);
    uint32_t  getGovLength();
    void setGovLength(uint32_t gop);

    IVideoOSD *CreateOSD(uint32_t index);
private:
    Hi3518mppMedia&     _media;
    HimppVencChan&      _venc_chan;
};

class HimppVideoStream : public IH264VideoStream, public IJPEGVideoStream
{
public:
    HimppVideoStream(IpcamRuntime *runtime, HimppVencChan &venc_chan);
    ~HimppVideoStream();

    IVideoEncoder::EncodingType getEncoding();
    uint32_t getBitrate();
    bool registerConsumer(StreamDataConsumer *consumer);
    bool unregisterConsumer(StreamDataConsumer *consumer);
    void enableStreaming();
    void disableStreaming();
    void requestIDR();

private:
    void watch_handler(ev::io &w, int revents);

private:
    IpcamRuntime*       _runtime;
    HimppVencChan&      _venc_chan;
    ev::io              _io;
    StreamDataConsumer* _consumer;
};


class HimppAudioSource : public IAudioSource
{
public:
    HimppAudioSource(Hi3518mppMedia &media);
    ~HimppAudioSource();

    uint32_t        getChannels();
private:
    Hi3518mppMedia&     _media;
};

class HimppAudioEncoder : public IAudioEncoder
{
public:
    HimppAudioEncoder(Hi3518mppMedia &media, HimppAencChan &aenc_chan);
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
    Hi3518mppMedia&    _media;
    HimppAencChan&     _aenc_chan;
};

class HimppAudioStream : public IAudioStream
{
public:
    HimppAudioStream(IpcamRuntime *runtime, HimppAiDev &ai_dev, HimppAencChan &venc_chan);
    ~HimppAudioStream();

    IAudioEncoder::EncodingType getEncoding();
    uint32_t getSampleRate();
    bool registerConsumer(StreamDataConsumer *consumer);
    bool unregisterConsumer(StreamDataConsumer *consumer);
    void enableStreaming();
    void disableStreaming();

private:
    void watch_handler(ev::io &w, int revents);

private:
    IpcamRuntime*       _runtime;
    HimppAiDev&         _ai_dev;
    HimppAencChan&      _aenc_chan;
    ev::io              _io;
    StreamDataConsumer* _consumer;
};


class Hi3518mppMedia
{
public:
    Hi3518mppMedia(IpcamRuntime*, std::string);
    ~Hi3518mppMedia();

    RGN_HANDLE allocRegionHandle();
    void freeRegionHandle(RGN_HANDLE handle);

private:
    IpcamRuntime       *_runtime;
    std::bitset<RGN_HANDLE_MAX> _region_bitmap;

    // Video Objects
    HimppSysctl         _sysctl;
    HimppVideoSensor&   _sensor;
    HimppViDev          _vi_dev0;
    HimppViChan         _vi_chan0;
    HimppVpssGroup      _vpss_group0;
    HimppVpssChan       _vpss_chan0;
    HimppVencChan       _venc_chan0;
    HimppVencChan       _venc_chan1;
    HimppVencChan       _venc_chan2;

    // Audio Objects
    HimppAudioCodec     _acodec;
    HimppAiDev          _ai_dev0;
    HimppAiChan         _ai_chan0;
    HimppAencChan       _aenc_chan0;
    HimppAencChan       _aenc_chan1;

    friend class HimppVideoSource;
    friend class HimppH264VideoEncoder;
    friend class HimppAudioSource;
    friend class HimppAudioEncoder;

    HimppVideoStream        _video_stream0;
    HimppVideoStream        _video_stream1;
    HimppVideoStream        _video_stream2;
    HimppAudioStream        _audio_stream0;
    HimppAudioStream        _audio_stream1;

    HimppVideoSource        _video_source0;
    HimppH264VideoEncoder   _video_encoder0;
    HimppH264VideoEncoder   _video_encoder1;

    HimppAudioSource        _audio_source0;
    HimppAudioEncoder       _audio_encoder0;
    HimppAudioEncoder       _audio_encoder1;

    ServerMediaSession* addSMS(std::string stream_path,
                               HimppH264VideoEncoder* v_encoder,
                               HimppAudioEncoder* a_encoder);
};

#endif // _HIMPP_MEDIA_H_
