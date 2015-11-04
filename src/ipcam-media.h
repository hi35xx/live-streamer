/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ipcam-media-interface.h
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

#ifndef _IPCAM_MEDIA_INTERFACE_H_
#define _IPCAM_MEDIA_INTERFACE_H_

#include <stdbool.h>
#include <string>
#include <dbus-c++/error.h>

namespace Ipcam
{
namespace Interface
{

typedef DBus::ErrorFailed   IpcamError;

class ImageResolution
{
public:
    ImageResolution(uint32_t w, uint32_t h);
    ImageResolution(std::string resolution);
    operator std::string ();

    uint16_t Width;
    uint16_t Height;
};

class IVideoSource
{
public:
    class Imaging
    {
    public:
        class Backlight             // Backlight Compensation
        {
        public:
            enum BacklightMode { BACKLIGHT_OFF, BACKLIGHT_ON };
        public:
            virtual BacklightMode   getMode() = 0;
            virtual void            setMode(BacklightMode value) = 0;
            virtual uint32_t        getLevel() = 0;
            virtual void            setLevel(uint32_t value) = 0;
        };
        class Focus                 // Focus Configuration
        {
        public:
            enum AutoFocusMode { AUTO_FOCUS, MANUAL_FOCUS };
        public:
            virtual AutoFocusMode   getAutoFocusMode() = 0;
            virtual void            setAutoFocusMode(AutoFocusMode value) = 0;
            virtual uint32_t        getDefaultSpeed() = 0;
            virtual void            setDefaultSpeed(uint32_t value) = 0;
            virtual uint32_t        getNearLimit() = 0;
            virtual void            setNearLimit(uint32_t value) = 0;
            virtual uint32_t        getFarLimit() = 0;
            virtual void            setFarLimit(uint32_t value) = 0;
        };
        class Exposure              // Exposure
        {
        public:
            enum ExposureMode { AUTO_EXPOSURE, MANUAL_EXPOSURE };
            enum ExposurePriority { LOWNOISE_PRIORITY, FRAMERATE_PRIORITY };
        public:
            virtual ExposureMode    getMode() = 0;
            virtual void            setMode(ExposureMode value) = 0;
            virtual ExposurePriority getPriority() = 0;
            virtual void            setPriority(ExposurePriority value) = 0;
            virtual uint32_t        getMinExposureTime() = 0;
            virtual void            setMinExposureTime(uint32_t value) = 0;
            virtual uint32_t        getMaxExposureTime() = 0;
            virtual void            setMaxExposureTime(uint32_t value) = 0;
            virtual uint32_t        getMinGain() = 0;
            virtual void            setMinGain(uint32_t value) = 0;
            virtual uint32_t        getMaxGain() = 0;
            virtual void            setMaxGain(uint32_t value) = 0;
            virtual uint32_t        getMinIris() = 0;
            virtual void            setMinIris(uint32_t value) = 0;
            virtual uint32_t        getMaxIris() = 0;
            virtual void            setMaxIris(uint32_t value) = 0;
            virtual uint32_t        getExposureTime() = 0;
            virtual void            setExposureTime(uint32_t value) = 0;
            virtual uint32_t        getGain() = 0;
            virtual void            setGain(uint32_t value) = 0;
            virtual uint32_t        getIris() = 0;
            virtual void            setIris(uint32_t value) = 0;
        };
        class WhiteBalance          // White Balance
        {
        public:
            enum WhiteBalanceMode { AUTO_WB, MANUAL_WB };
        public:
            virtual WhiteBalanceMode getMode() = 0;
            virtual void            setMode(WhiteBalanceMode value) = 0;
            virtual uint32_t        getCrGain() = 0;
            virtual void            setCrGain(uint32_t value) = 0;
            virtual uint32_t        getCbGain() = 0;
            virtual void            setCbGain(uint32_t value) = 0;
        };
        class WideDynamicRange      // Wide Dynamic Range
        {
        public:
            enum WDRMode { WDR_OFF, WDR_ON };
        public:
            virtual WDRMode         getMode() = 0;
            virtual void            setMode(WDRMode value) = 0;
            virtual uint32_t        getLevel() = 0;
            virtual void            setLevel(uint32_t value) = 0;
        };
        class LDC                   // Lens Distortion Correction
        {
        public:
            enum LDCMode { LDC_OFF, LDC_ON };
        public:
            virtual uint32_t        getMode() = 0;
            virtual void            setMode(uint32_t value) = 0;
            virtual uint32_t        getRatio() = 0;
            virtual void            setRatio(uint32_t value) = 0;
        };
    public:
        virtual int32_t             getBrightness() = 0;
        virtual void                setBrightness(int32_t value) = 0;
        virtual int32_t             getContrast() = 0;
        virtual void                setContrast(int32_t value) = 0;
        virtual int32_t             getChroma() = 0;
        virtual void                setChroma(int32_t value) = 0;
        virtual int32_t             getSaturation() = 0;
        virtual void                setSaturation(int32_t value) = 0;
        virtual int32_t             getSharpness() = 0;
        virtual void                setSharpness(int32_t value) = 0;

        virtual Backlight*          getBacklight() = 0;
        virtual Focus*              getFocus() = 0;
        virtual Exposure*           getExposure() = 0;
        virtual WhiteBalance*       getWhiteBalance() = 0;
        virtual WideDynamicRange*   getWideDynamicRange() = 0;
        virtual LDC*                getLDC() = 0;
    };
public:
    virtual uint32_t    getFramerate() = 0;
    virtual void        setFramerate(uint32_t value) = 0;
    virtual ImageResolution getResolution() = 0;
    virtual void        setResolution(ImageResolution &value) = 0;

    virtual Imaging*    getImaging() = 0;
};


class IVideoEncoder
{
public:
    enum EncodingType { H264, MJPEG, JPEG, MPEG4 };
    enum RateCtrlMode { CBR, VBR, FIXQP };
public:
    virtual EncodingType getEncoding() = 0;
    virtual ImageResolution getResolution() = 0;
    virtual void setResolution(ImageResolution &resolution) = 0;
    virtual RateCtrlMode getRcMode() = 0;
    virtual void setRcMode(RateCtrlMode mode) = 0;
    virtual uint32_t  getFramerate() = 0;
    virtual void setFramerate(uint32_t fps) = 0;
    virtual uint32_t  getBitrate() = 0;
    virtual void setBitrate(uint32_t kbps) = 0;
};

class IH264VideoEncoder : public IVideoEncoder
{
public:
    enum H264Profile { BASELINE, MAIN, HIGH, SVC_T };
public:
    virtual H264Profile getProfile() = 0;
    virtual void setProfile(H264Profile profile) = 0;
    virtual uint32_t  getGovLength() = 0;
    virtual void setGovLength(uint32_t gop) = 0;
};

class IAudioSource
{
public:
    virtual uint32_t        getChannels() = 0;
};

class IAudioEncoder
{
public:
    enum EncodingType { ADPCM, LPCM, G711A, G711U, G726 };
public:
    virtual EncodingType    getEncoding() = 0;
    virtual void            setEncoding(EncodingType value) = 0;
    virtual uint32_t        getBitrate() = 0;
    virtual void            setBitrate(uint32_t value) = 0;
    virtual uint32_t        getSampleRate() = 0;
    virtual void            setSampleRate(uint32_t value) = 0;
};


} // namespace Interface
} // namespace Ipcam

#endif // _IPCAM_MEDIA_INTERFACE_H_

