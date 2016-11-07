/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * dbus-video-source.cpp
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
 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dbus-c++/dbus.h>
#include <ipcam-runtime.h>

#include "dbus-video-source.h"

namespace DBus {

#define VIDEOSOURCE_INTERFACE   "ipcam.Media.VideoSource"
#define IMAGING_INTERFACE       "ipcam.Media.VideoSource.Imaging"
#define ANTIFLICKER_INTERFACE   "ipcam.Media.VideoSource.Imaging.AntiFlicker"
#define BLC_INTERFACE           "ipcam.Media.VideoSource.Imaging.BacklightCompensation"
#define FOCUS_INTERFACE         "ipcam.Media.VideoSource.Imaging.Focus"
#define EXPOSURE_INTERFACE      "ipcam.Media.VideoSource.Imaging.Exposure"
#define WB_INTERFACE            "ipcam.Media.VideoSource.Imaging.WhiteBalance"
#define WDR_INTERFACE           "ipcam.Media.VideoSource.Imaging.WideDynamicRange"
#define LDC_INTERFACE           "ipcam.Media.VideoSource.Imaging.LDC"
#define GAMMA_INTERFACE         "ipcam.Media.VideoSource.Imaging.Gamma"

#define DEFINE_PROP(prop, get, set) \
    _prop_handler.emplace(std::piecewise_construct, \
        std::forward_as_tuple(prop), \
        std::forward_as_tuple(get, set))

VideoSource::VideoSource
(IpcamRuntime &runtime, std::string obj_path, IVideoSource *video_source)
  : IpcamBase(runtime, obj_path),
    _video_source(video_source)
{

    // Handler of ipcam.Media.VideoSource
    DEFINE_PROP(VIDEOSOURCE_INTERFACE ".Resolution",
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            DBus::MessageIter wi = value.writer();
            wi << (std::string)_video_source->getResolution();
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            std::string s = value;
            ImageResolution r(s);
            _video_source->setResolution(r);
        });
    DEFINE_PROP(VIDEOSOURCE_INTERFACE ".Framerate",
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            DBus::MessageIter wi = value.writer();
            wi << _video_source->getFramerate();
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->setFramerate((uint32_t)value);
        });
    // Handler of ipcam.Media.VideoSource.Imaging
    DEFINE_PROP(IMAGING_INTERFACE ".Mirror",
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            DBus::MessageIter wi = value.writer();
            wi << _video_source->getImaging().getMirror();
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().setMirror((bool)value);
        });
    DEFINE_PROP(IMAGING_INTERFACE ".Flip",
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            DBus::MessageIter wi = value.writer();
            wi << _video_source->getImaging().getFlip();
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().setFlip((bool)value);
        });
    DEFINE_PROP(IMAGING_INTERFACE ".Brightness",
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            DBus::MessageIter wi = value.writer();
            wi << _video_source->getImaging().getBrightness();
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().setBrightness((uint32_t)value);
        });
    DEFINE_PROP(IMAGING_INTERFACE ".Contrast", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getContrast());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().setContrast((uint32_t)value);
        });
    DEFINE_PROP(IMAGING_INTERFACE ".Chroma",
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getChroma());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().setChroma((uint32_t)value);
        });
    DEFINE_PROP(IMAGING_INTERFACE ".Saturation",
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getSaturation());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().setSaturation((uint32_t)value);
        });
    DEFINE_PROP(IMAGING_INTERFACE ".Sharpness",
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getSharpness());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().setSharpness((uint32_t)value);
        });
    DEFINE_PROP(IMAGING_INTERFACE ".IrCutFilterMode",
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getIrCutFilterMode());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().setIrCutFilterMode((IVideoSource::Imaging::IrCutFilterMode)(uint32_t)value);
        });
    // Handler of ipcam.Media.VideoSource.Imaging.AntiFlicker
    DEFINE_PROP(ANTIFLICKER_INTERFACE ".Mode", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getAntiFlicker().getMode());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getAntiFlicker().setMode((IVideoSource::Imaging::AntiFlicker::AntiFlickerMode)(uint32_t)value);
        });
    DEFINE_PROP(ANTIFLICKER_INTERFACE ".Frequency", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getAntiFlicker().getFrequency());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getAntiFlicker().setFrequency((uint32_t)value);
        });
    // Handler of ipcam.Media.VideoSource.Imaging.Backlight
    DEFINE_PROP(BLC_INTERFACE ".Mode", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getBacklight().getMode());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getBacklight().setMode((IVideoSource::Imaging::Backlight::BacklightMode)(uint32_t)value);
        });
    DEFINE_PROP(BLC_INTERFACE ".Level", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getBacklight().getLevel());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getBacklight().setLevel((uint32_t)value);
        });
    // Handler of ipcam.Media.VideoSource.Imaging.Focus
    DEFINE_PROP(FOCUS_INTERFACE ".AutoFocusMode", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getFocus().getAutoFocusMode());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getFocus().setAutoFocusMode((IVideoSource::Imaging::Focus::AutoFocusMode)(uint32_t)value);
        });
    DEFINE_PROP(FOCUS_INTERFACE ".DefaultSpeed", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getFocus().getDefaultSpeed());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getFocus().setDefaultSpeed((uint32_t)value);
        });
    DEFINE_PROP(FOCUS_INTERFACE ".NearLimit", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getFocus().getNearLimit());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getFocus().setNearLimit((uint32_t)value);
        });
    DEFINE_PROP(FOCUS_INTERFACE ".FarLimit", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getFocus().getFarLimit());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getFocus().setFarLimit((uint32_t)value);
        });
    // Handler of ipcam.Media.VideoSource.Imaging.Exposure
    DEFINE_PROP(EXPOSURE_INTERFACE ".Mode", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getExposure().getMode());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getExposure().setMode((IVideoSource::Imaging::Exposure::ExposureMode)(uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".Priority", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getExposure().getPriority());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getExposure().setPriority((IVideoSource::Imaging::Exposure::ExposurePriority)(uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".MinExposureTime", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getExposure().getMinExposureTime());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getExposure().setMinExposureTime((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".MaxExposureTime", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getExposure().getMaxExposureTime());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getExposure().setMaxExposureTime((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".MinGain", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getExposure().getMinGain());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getExposure().setMinGain((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".MaxGain", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getExposure().getMaxGain());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getExposure().setMaxGain((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".MinIris", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getExposure().getMinIris());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getExposure().setMinIris((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".MaxIris", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getExposure().getMaxIris());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getExposure().setMaxIris((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".ExposureTime", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getExposure().getExposureTime());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getExposure().setExposureTime((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".Gain", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getExposure().getGain());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getExposure().setGain((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".Iris", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getExposure().getIris());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getExposure().setIris((uint32_t)value);
        });
    // Handler of ipcam.Media.VideoSource.Imaging.WhiteBalance
    DEFINE_PROP(WB_INTERFACE ".Mode", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getWhiteBalance().getMode());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getWhiteBalance().setMode((IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode)(uint32_t)value);
        });
    DEFINE_PROP(WB_INTERFACE ".CrGain", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getWhiteBalance().getCrGain());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getWhiteBalance().setCrGain((uint32_t)value);
        });
    DEFINE_PROP(WB_INTERFACE ".CbGain", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getWhiteBalance().getCbGain());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getWhiteBalance().setCbGain((uint32_t)value);
        });
    DEFINE_PROP(WB_INTERFACE ".RGain", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getWhiteBalance().getRGain());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getWhiteBalance().setRGain((uint32_t)value);
        });
    DEFINE_PROP(WB_INTERFACE ".GGain", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getWhiteBalance().getGGain());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getWhiteBalance().setGGain((uint32_t)value);
        });
    DEFINE_PROP(WB_INTERFACE ".BGain", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getWhiteBalance().getBGain());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getWhiteBalance().setBGain((uint32_t)value);
        });
    // Handler of ipcam.Media.VideoSource.Imaging.WideDynamicRange
    DEFINE_PROP(WDR_INTERFACE ".Mode", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getWideDynamicRange().getMode());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getWideDynamicRange().setMode((IVideoSource::Imaging::WideDynamicRange::WDRMode)(uint32_t)value);
        });
    DEFINE_PROP(WDR_INTERFACE ".Level", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getWideDynamicRange().getLevel());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getWideDynamicRange().setLevel((uint32_t)value);
        });
    // Handler of ipcam.Media.VideoSource.Imaging.LDC
    DEFINE_PROP(LDC_INTERFACE ".Mode", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getLDC().getMode());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getLDC().setMode((IVideoSource::Imaging::LDC::LDCMode)(uint32_t)value);
        });
    DEFINE_PROP(LDC_INTERFACE ".Ratio", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_source->getImaging().getLDC().getRatio());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            _video_source->getImaging().getLDC().setRatio((uint32_t)value);
        });
    // Handler of ipcam.Media.VideoSource.Imaging.LDC
    DEFINE_PROP(GAMMA_INTERFACE ".CurveData", 
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            std::vector<uint32_t>& curve = _video_source->getImaging().getGamma().getCurveData();
            DBus::MessageIter mi = value.writer();
            mi << curve;
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            std::vector<uint32_t> curve = value;
            _video_source->getImaging().getGamma().setCurveData(curve);
        });
}

void VideoSource::do_property_get
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
    value.clear();

    auto iter = _prop_handler.find(interface.name() + "." + property);
    if (iter == _prop_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second.Get(interface, property, value);
}

void VideoSource::do_property_set
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
    auto iter = _prop_handler.find(interface.name() + "." + property);
    if (iter == _prop_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second.Set(interface, property, value);
}

} // namespace DBus
