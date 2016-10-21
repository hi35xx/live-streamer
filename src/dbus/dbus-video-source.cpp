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

VideoSource::VideoSource
(IpcamRuntime &runtime, std::string obj_path, IVideoSource *video_source)
  : DBus::ObjectAdaptor(runtime.dbus_conn(), obj_path),
    _runtime(runtime), _video_source(video_source)
{
    assert(video_source != NULL);

#define DEFINE_PROP(prop, get, set) \
    _prop_handler.emplace(std::piecewise_construct, \
        std::forward_as_tuple(prop), \
        std::forward_as_tuple(get, set))

    // Handler of ipcam.Media.VideoSource
    DEFINE_PROP(VIDEOSOURCE_INTERFACE ".Resolution",
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            DBus::MessageIter wi = value.writer();
            wi << (std::string)vsrc.getResolution();
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            std::string s = value;
            ImageResolution r(s);
            vsrc.setResolution(r);
        });
    DEFINE_PROP(VIDEOSOURCE_INTERFACE ".Framerate",
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            DBus::MessageIter wi = value.writer();
            wi << vsrc.getFramerate();
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.setFramerate((uint32_t)value);
        });
    // Get handler of ipcam.Media.VideoSource.Imaging
    DEFINE_PROP(IMAGING_INTERFACE ".Mirror",
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            DBus::MessageIter wi = value.writer();
            wi << vsrc.getImaging()->getMirror();
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setMirror((bool)value);
        });
    DEFINE_PROP(IMAGING_INTERFACE ".Flip",
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            DBus::MessageIter wi = value.writer();
            wi << vsrc.getImaging()->getFlip();
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setFlip((bool)value);
        });
    DEFINE_PROP(IMAGING_INTERFACE ".Brightness",
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            DBus::MessageIter wi = value.writer();
            wi << vsrc.getImaging()->getBrightness();
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setBrightness((uint32_t)value);
        });
    DEFINE_PROP(IMAGING_INTERFACE ".Contrast", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getContrast());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setContrast((uint32_t)value);
        });
    DEFINE_PROP(IMAGING_INTERFACE ".Chroma",
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getChroma());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setChroma((uint32_t)value);
        });
    DEFINE_PROP(IMAGING_INTERFACE ".Saturation",
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getSaturation());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setSaturation((uint32_t)value);
        });
    DEFINE_PROP(IMAGING_INTERFACE ".Sharpness",
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getSharpness());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setSharpness((uint32_t)value);
        });
    DEFINE_PROP(IMAGING_INTERFACE ".IrCutFilterMode",
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getIrCutFilterMode());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setIrCutFilterMode((IVideoSource::Imaging::IrCutFilterMode)(uint32_t)value);
        });
    // Get handler of ipcam.Media.VideoSource.Imaging.AntiFlicker
    DEFINE_PROP(ANTIFLICKER_INTERFACE ".Mode", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getAntiFlicker()->getMode());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getAntiFlicker()->setMode((IVideoSource::Imaging::AntiFlicker::AntiFlickerMode)(uint32_t)value);
        });
    DEFINE_PROP(ANTIFLICKER_INTERFACE ".Frequency", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getAntiFlicker()->getFrequency());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getAntiFlicker()->setFrequency((uint32_t)value);
        });
    // Get handler of ipcam.Media.VideoSource.Imaging.Backlight
    DEFINE_PROP(BLC_INTERFACE ".Mode", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getBacklight()->getMode());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getBacklight()->setMode((IVideoSource::Imaging::Backlight::BacklightMode)(uint32_t)value);
        });
    DEFINE_PROP(BLC_INTERFACE ".Level", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getBacklight()->getLevel());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getBacklight()->setLevel((uint32_t)value);
        });
    // Get handler of ipcam.Media.VideoSource.Imaging.Focus
    DEFINE_PROP(FOCUS_INTERFACE ".AutoFocusMode", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getFocus()->getAutoFocusMode());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getFocus()->setAutoFocusMode((IVideoSource::Imaging::Focus::AutoFocusMode)(uint32_t)value);
        });
    DEFINE_PROP(FOCUS_INTERFACE ".DefaultSpeed", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getFocus()->getDefaultSpeed());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getFocus()->setDefaultSpeed((uint32_t)value);
        });
    DEFINE_PROP(FOCUS_INTERFACE ".NearLimit", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getFocus()->getNearLimit());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getFocus()->setNearLimit((uint32_t)value);
        });
    DEFINE_PROP(FOCUS_INTERFACE ".FarLimit", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getFocus()->getFarLimit());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getFocus()->setFarLimit((uint32_t)value);
        });
    // Get handler of ipcam.Media.VideoSource.Imaging.Exposure
    DEFINE_PROP(EXPOSURE_INTERFACE ".Mode", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getMode());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setMode((IVideoSource::Imaging::Exposure::ExposureMode)(uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".Priority", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getPriority());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setPriority((IVideoSource::Imaging::Exposure::ExposurePriority)(uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".MinExposureTime", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getMinExposureTime());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setMinExposureTime((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".MaxExposureTime", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getMaxExposureTime());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setMaxExposureTime((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".MinGain", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getMinGain());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setMinGain((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".MaxGain", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getMaxGain());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setMaxGain((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".MinIris", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getMinIris());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setMinIris((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".MaxIris", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getMaxIris());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setMaxIris((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".ExposureTime", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getExposureTime());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setExposureTime((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".Gain", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getGain());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setGain((uint32_t)value);
        });
    DEFINE_PROP(EXPOSURE_INTERFACE ".Iris", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getIris());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setIris((uint32_t)value);
        });
    // Get handler of ipcam.Media.VideoSource.Imaging.WhiteBalance
    DEFINE_PROP(WB_INTERFACE ".Mode", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWhiteBalance()->getMode());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWhiteBalance()->setMode((IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode)(uint32_t)value);
        });
    DEFINE_PROP(WB_INTERFACE ".CrGain", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWhiteBalance()->getCrGain());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWhiteBalance()->setCrGain((uint32_t)value);
        });
    DEFINE_PROP(WB_INTERFACE ".CbGain", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWhiteBalance()->getCbGain());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWhiteBalance()->setCbGain((uint32_t)value);
        });
    DEFINE_PROP(WB_INTERFACE ".RGain", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWhiteBalance()->getRGain());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWhiteBalance()->setRGain((uint32_t)value);
        });
    DEFINE_PROP(WB_INTERFACE ".GGain", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWhiteBalance()->getGGain());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWhiteBalance()->setGGain((uint32_t)value);
        });
    DEFINE_PROP(WB_INTERFACE ".BGain", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWhiteBalance()->getBGain());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWhiteBalance()->setBGain((uint32_t)value);
        });
    // Get handler of ipcam.Media.VideoSource.Imaging.WideDynamicRange
    DEFINE_PROP(WDR_INTERFACE ".Mode", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWideDynamicRange()->getMode());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWideDynamicRange()->setMode((IVideoSource::Imaging::WideDynamicRange::WDRMode)(uint32_t)value);
        });
    DEFINE_PROP(WDR_INTERFACE ".Level", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWideDynamicRange()->getLevel());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWideDynamicRange()->setLevel((uint32_t)value);
        });
    // Get handler of ipcam.Media.VideoSource.Imaging.LDC
    DEFINE_PROP(LDC_INTERFACE ".Mode", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getLDC()->getMode());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getLDC()->setMode((IVideoSource::Imaging::LDC::LDCMode)(uint32_t)value);
        });
    DEFINE_PROP(LDC_INTERFACE ".Ratio", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getLDC()->getRatio());
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getLDC()->setRatio((uint32_t)value);
        });
    // Get handler of ipcam.Media.VideoSource.Imaging.LDC
    DEFINE_PROP(GAMMA_INTERFACE ".CurveData", 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            std::vector<uint32_t>& curve = vsrc.getImaging()->getGamma()->getCurveData();
            DBus::MessageIter mi = value.writer();
            mi << curve;
        },
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            std::vector<uint32_t> curve = value;
            vsrc.getImaging()->getGamma()->setCurveData(curve);
        });
}

void VideoSource::on_get_property
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
    value.clear();

    auto iter = _prop_handler.find(interface.name() + "." + property);
    if (iter == _prop_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second.Get(*_video_source, interface, property, value);
}

void VideoSource::on_set_property
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
    auto iter = _prop_handler.find(interface.name() + "." + property);
    if (iter == _prop_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second.Set(*_video_source, interface, property, value);
}

} // namespace DBus
