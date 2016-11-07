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

#ifndef _HI3520MPP_MEDIA_H_
#define _HI3520MPP_MEDIA_H_

#include <bitset>

#include <himpp-sysctl.h>
#include <himpp-video-codec.h>
#include <himpp-video-viu.h>
#include <himpp-video-vpss.h>
#include <himpp-video-venc.h>

#include <ipcam-media-iface.h>
#include <ipcam-runtime.h>

using namespace Ipcam::Media;

class Hi3520mppMedia;

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
            Exposure(HimppVideoSource &source);

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
            HimppVideoSource& _source;
        };
        class WhiteBalance : public IVideoSource::Imaging::WhiteBalance
        {
        public:
            typedef IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode WhiteBalanceMode;
        public:
            WhiteBalance(HimppVideoSource &source);

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
            HimppVideoSource& _source;
        };
    public:
        Imaging(HimppVideoSource &source);

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
        HimppVideoSource& _source;
        Exposure        _exposure;
        WhiteBalance    _white_balance;
    };
public:
    HimppVideoSource(Hi3520mppMedia &media, HimppViDev &vi_dev, HimppViChan &vi_chan);
    ~HimppVideoSource();

    // video source method
    void        setFramerate(uint32_t value);
    uint32_t    getFramerate();
    void        setResolution(ImageResolution &res);
    ImageResolution getResolution();
    // imaging method
    IVideoSource::Imaging&    getImaging();
private:
    Hi3520mppMedia& _media;
    Imaging         _imaging;
    HimppViDev&     _vi_dev;
    HimppViChan&    _vi_chan;
};

class HimppVideoOSD : public IVideoOSD
{
public:
    HimppVideoOSD(Hi3520mppMedia &media, HimppVencChan &venc_chan, uint32_t id);
    ~HimppVideoOSD();

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
    Hi3520mppMedia &_media;
    HimppVencChan &_venc_chan;
    HimppVideoRegion _region;
    SDL_Surface *_surface;
};

class HimppH264VideoEncoder : public IH264VideoEncoder
{
public:
    HimppH264VideoEncoder(Hi3520mppMedia &media, HimppVencChan &venc_chan);
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
    Hi3520mppMedia&     _media;
    HimppVencChan&      _venc_chan;
};


class HimppH264VideoStream : public IVideoStream
{
public:
    HimppH264VideoStream(IpcamRuntime *runtime, HimppVencChan &venc_chan);
    ~HimppH264VideoStream();

    IVideoEncoder::EncodingType getEncoding();
    uint32_t getBitrate();
    bool registerConsumer(StreamDataConsumer *consumer);
    bool unregisterConsumer(StreamDataConsumer *consumer);
    void enableStreaming();
    void disableStreaming();
private:
    IpcamRuntime*       _runtime;
    HimppVencChan&      _venc_chan;
    ev::io              _io;
    StreamDataConsumer* _consumer;

    void watch_handler(ev::io &w, int revents);
};


class Hi3520mppMedia
{
public:
    Hi3520mppMedia(IpcamRuntime *runtime, std::string);
    ~Hi3520mppMedia();

    RGN_HANDLE allocRegionHandle();
    void freeRegionHandle(RGN_HANDLE handle);

private:
    IpcamRuntime        *_runtime;
    std::bitset<RGN_HANDLE_MAX> _region_bitmap;

    // Video Objects
    HimppSysctl         _sysctl;
    HimppVideoCodecNVP1918 _codec;
    HimppViDev          _vi_dev0;
    HimppViChan         _vi_chan0;
    HimppViChan         _vi_chan1;
    HimppViChan         _vi_chan2;
    HimppViChan         _vi_chan3;
    HimppVpssGroup      _vpss_group0;
    HimppVpssGroup      _vpss_group1;
    HimppVpssGroup      _vpss_group2;
    HimppVpssGroup      _vpss_group3;
    HimppVpssChan       _vpss_chan0;
    HimppVpssChan       _vpss_chan1;
    HimppVpssChan       _vpss_chan2;
    HimppVpssChan       _vpss_chan3;
    HimppVencChan       _venc_chan0;
    HimppVencChan       _venc_chan1;
    HimppVencChan       _venc_chan2;
    HimppVencChan       _venc_chan3;

    friend class HimppVideoSource;
    friend class HimppH264VideoEncoder;

    // Video stream interface
    HimppH264VideoStream    _video_stream0;
    HimppH264VideoStream    _video_stream1;
    HimppH264VideoStream    _video_stream2;
    HimppH264VideoStream    _video_stream3;
    // Video source control interface
    HimppVideoSource        _video_source0;
    HimppVideoSource        _video_source1;
    HimppVideoSource        _video_source2;
    HimppVideoSource        _video_source3;
    // Video encoder control interface
    HimppH264VideoEncoder   _video_encoder0;
    HimppH264VideoEncoder   _video_encoder1;
    HimppH264VideoEncoder   _video_encoder2;
    HimppH264VideoEncoder   _video_encoder3;
};

#endif // _HIMPP_MEDIA_H_
