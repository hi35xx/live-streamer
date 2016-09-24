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
#define BLC_INTERFACE           "ipcam.Media.VideoSource.Imaging.BacklightCompensation"
#define FOCUS_INTERFACE         "ipcam.Media.VideoSource.Imaging.Focus"
#define EXPOSURE_INTERFACE      "ipcam.Media.VideoSource.Imaging.Exposure"
#define WB_INTERFACE            "ipcam.Media.VideoSource.Imaging.WhiteBalance"
#define WDR_INTERFACE           "ipcam.Media.VideoSource.Imaging.WideDynamicRange"
#define LDC_INTERFACE           "ipcam.Media.VideoSource.Imaging.LDC"

VideoSource::VideoSource
(IpcamRuntime &runtime, std::string obj_path, IVideoSource *video_source)
  : DBus::ObjectAdaptor(runtime.dbus_conn(), obj_path),
    _runtime(runtime), _video_source(video_source)
{
    assert(video_source != NULL);

    // Get handler of ipcam.Media.VideoSource
    _prop_get_handler[VIDEOSOURCE_INTERFACE ".Resolution"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_string(((std::string)vsrc.getResolution()).c_str());
        };
    _prop_get_handler[VIDEOSOURCE_INTERFACE ".Framerate"] =  
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getFramerate());
        };
    // Get handler of ipcam.Media.VideoSource.Imaging
    _prop_get_handler[IMAGING_INTERFACE ".Mirror"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_bool(vsrc.getImaging()->getMirror());
        };
    _prop_get_handler[IMAGING_INTERFACE ".Flip"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_bool(vsrc.getImaging()->getFlip());
        };
    _prop_get_handler[IMAGING_INTERFACE ".Brightness"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getBrightness());
        };
    _prop_get_handler[IMAGING_INTERFACE ".Contrast"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getContrast());
        };
    _prop_get_handler[IMAGING_INTERFACE ".Chroma"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getChroma());
        };
    _prop_get_handler[IMAGING_INTERFACE ".Saturation"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getSaturation());
        };
    _prop_get_handler[IMAGING_INTERFACE ".Sharpness"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getSharpness());
        };
    _prop_get_handler[IMAGING_INTERFACE ".IrCutFilterMode"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getIrCutFilterMode());
        };
    // Get handler of ipcam.Media.VideoSource.Imaging.Backlight
    _prop_get_handler[BLC_INTERFACE ".Mode"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getBacklight()->getMode());
        };
    _prop_get_handler[BLC_INTERFACE ".Level"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getBacklight()->getLevel());
        };
    // Get handler of ipcam.Media.VideoSource.Imaging.Focus
    _prop_get_handler[FOCUS_INTERFACE ".AutoFocusMode"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getFocus()->getAutoFocusMode());
        };
    _prop_get_handler[FOCUS_INTERFACE ".DefaultSpeed"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getFocus()->getDefaultSpeed());
        };
    _prop_get_handler[FOCUS_INTERFACE ".NearLimit"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getFocus()->getNearLimit());
        };
    _prop_get_handler[FOCUS_INTERFACE ".FarLimit"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getFocus()->getFarLimit());
        };
    // Get handler of ipcam.Media.VideoSource.Imaging.Exposure
    _prop_get_handler[EXPOSURE_INTERFACE ".Mode"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getMode());
        };
    _prop_get_handler[EXPOSURE_INTERFACE ".Priority"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getPriority());
        };
    _prop_get_handler[EXPOSURE_INTERFACE ".MinExposureTime"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getMinExposureTime());
        };
    _prop_get_handler[EXPOSURE_INTERFACE ".MaxExposureTime"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getMaxExposureTime());
        };
    _prop_get_handler[EXPOSURE_INTERFACE ".MinGain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getMinGain());
        };
    _prop_get_handler[EXPOSURE_INTERFACE ".MaxGain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getMaxGain());
        };
    _prop_get_handler[EXPOSURE_INTERFACE ".MinIris"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getMinIris());
        };
    _prop_get_handler[EXPOSURE_INTERFACE ".MaxIris"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getMaxIris());
        };
    _prop_get_handler[EXPOSURE_INTERFACE ".ExposureTime"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getExposureTime());
        };
    _prop_get_handler[EXPOSURE_INTERFACE ".Gain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getGain());
        };
    _prop_get_handler[EXPOSURE_INTERFACE ".Iris"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getExposure()->getIris());
        };
    // Get handler of ipcam.Media.VideoSource.Imaging.WhiteBalance
    _prop_get_handler[WB_INTERFACE ".Mode"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWhiteBalance()->getMode());
        };
    _prop_get_handler[WB_INTERFACE ".CrGain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWhiteBalance()->getCrGain());
        };
    _prop_get_handler[WB_INTERFACE ".CbGain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWhiteBalance()->getCbGain());
        };
    _prop_get_handler[WB_INTERFACE ".RGain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWhiteBalance()->getRGain());
        };
    _prop_get_handler[WB_INTERFACE ".GGain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWhiteBalance()->getGGain());
        };
    _prop_get_handler[WB_INTERFACE ".BGain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWhiteBalance()->getBGain());
        };
    // Get handler of ipcam.Media.VideoSource.Imaging.WideDynamicRange
    _prop_get_handler[WDR_INTERFACE ".Mode"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWideDynamicRange()->getMode());
        };
    _prop_get_handler[WDR_INTERFACE ".Level"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getWideDynamicRange()->getLevel());
        };
    // Get handler of ipcam.Media.VideoSource.Imaging.LDC
    _prop_get_handler[LDC_INTERFACE ".Mode"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getLDC()->getMode());
        };
    _prop_get_handler[LDC_INTERFACE ".Ratio"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(vsrc.getImaging()->getLDC()->getRatio());
        };


    // Set handler of ipcam.Media.VideoSource
    _prop_set_handler[VIDEOSOURCE_INTERFACE ".Resolution"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            std::string s = value;
            ImageResolution r(s);
            vsrc.setResolution(r);
        };
    _prop_set_handler[VIDEOSOURCE_INTERFACE ".Framerate"] =  
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.setFramerate((uint32_t)value);
        };
    // Set handler of ipcam.Media.VideoSource.Imaging
    _prop_set_handler[IMAGING_INTERFACE ".Mirror"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setMirror((bool)value);
        };
    _prop_set_handler[IMAGING_INTERFACE ".Flip"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setFlip((bool)value);
        };
    _prop_set_handler[IMAGING_INTERFACE ".Brightness"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setBrightness((uint32_t)value);
        };
    _prop_set_handler[IMAGING_INTERFACE ".Contrast"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setContrast((uint32_t)value);
        };
    _prop_set_handler[IMAGING_INTERFACE ".Chroma"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setChroma((uint32_t)value);
        };
    _prop_set_handler[IMAGING_INTERFACE ".Saturation"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setSaturation((uint32_t)value);
        };
    _prop_set_handler[IMAGING_INTERFACE ".Sharpness"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setSharpness((uint32_t)value);
        };
    _prop_set_handler[IMAGING_INTERFACE ".IrCutFilterMode"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->setIrCutFilterMode((IVideoSource::Imaging::IrCutFilterMode)(uint32_t)value);
        };
    // Set handler of ipcam.Media.VideoSource.Imaging.BacklightCompensation
    _prop_set_handler[BLC_INTERFACE ".Mode"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getBacklight()->setMode((IVideoSource::Imaging::Backlight::BacklightMode)(uint32_t)value);
        };
    _prop_set_handler[BLC_INTERFACE ".Level"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getBacklight()->setLevel((uint32_t)value);
        };
    // Set handler of ipcam.Media.VideoSource.Imaging.Focus
    _prop_set_handler[FOCUS_INTERFACE ".AutoFocusMode"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getFocus()->setAutoFocusMode((IVideoSource::Imaging::Focus::AutoFocusMode)(uint32_t)value);
        };
    _prop_set_handler[FOCUS_INTERFACE ".DefaultSpeed"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getFocus()->setDefaultSpeed((uint32_t)value);
        };
    _prop_set_handler[FOCUS_INTERFACE ".NearLimit"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getFocus()->setNearLimit((uint32_t)value);
        };
    _prop_set_handler[FOCUS_INTERFACE ".FarLimit"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getFocus()->setFarLimit((uint32_t)value);
        };
    // Set handler of ipcam.Media.VideoSource.Imaging.Exposure
    _prop_set_handler[EXPOSURE_INTERFACE ".Mode"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setMode((IVideoSource::Imaging::Exposure::ExposureMode)(uint32_t)value);
        };
    _prop_set_handler[EXPOSURE_INTERFACE ".Priority"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setPriority((IVideoSource::Imaging::Exposure::ExposurePriority)(uint32_t)value);
        };
    _prop_set_handler[EXPOSURE_INTERFACE ".MinExposureTime"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setMinExposureTime((uint32_t)value);
        };
    _prop_set_handler[EXPOSURE_INTERFACE ".MaxExposureTime"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setMaxExposureTime((uint32_t)value);
        };
    _prop_set_handler[EXPOSURE_INTERFACE ".MinGain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setMinGain((uint32_t)value);
        };
    _prop_set_handler[EXPOSURE_INTERFACE ".MaxGain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setMaxGain((uint32_t)value);
        };
    _prop_set_handler[EXPOSURE_INTERFACE ".MinIris"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setMinIris((uint32_t)value);
        };
    _prop_set_handler[EXPOSURE_INTERFACE ".MaxIris"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setMaxIris((uint32_t)value);
        };
    _prop_set_handler[EXPOSURE_INTERFACE ".ExposureTime"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setExposureTime((uint32_t)value);
        };
    _prop_set_handler[EXPOSURE_INTERFACE ".Gain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setGain((uint32_t)value);
        };
    _prop_set_handler[EXPOSURE_INTERFACE ".Iris"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getExposure()->setIris((uint32_t)value);
        };
    // Set handler of ipcam.Media.VideoSource.Imaging.WhiteBalance
    _prop_set_handler[WB_INTERFACE ".Mode"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWhiteBalance()->setMode((IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode)(uint32_t)value);
        };
    _prop_set_handler[WB_INTERFACE ".CrGain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWhiteBalance()->setCrGain((uint32_t)value);
        };
    _prop_set_handler[WB_INTERFACE ".CbGain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWhiteBalance()->setCbGain((uint32_t)value);
        };
    _prop_set_handler[WB_INTERFACE ".RGain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWhiteBalance()->setRGain((uint32_t)value);
        };
    _prop_set_handler[WB_INTERFACE ".GGain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWhiteBalance()->setGGain((uint32_t)value);
        };
    _prop_set_handler[WB_INTERFACE ".BGain"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWhiteBalance()->setBGain((uint32_t)value);
        };
    // Set handler of ipcam.Media.VideoSource.Imaging.WideDynamicRange
    _prop_set_handler[WDR_INTERFACE ".Mode"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWideDynamicRange()->setMode((IVideoSource::Imaging::WideDynamicRange::WDRMode)(uint32_t)value);
        };
    _prop_set_handler[WDR_INTERFACE ".Level"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getWideDynamicRange()->setLevel((uint32_t)value);
        };
    // Set handler of ipcam.Media.VideoSource.Imaging.LDC
    _prop_set_handler[LDC_INTERFACE ".Mode"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getLDC()->setMode((IVideoSource::Imaging::LDC::LDCMode)(uint32_t)value);
        };
    _prop_set_handler[LDC_INTERFACE ".Ratio"] = 
        [](IVideoSource &vsrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            vsrc.getImaging()->getLDC()->setRatio((uint32_t)value);
        };
}

void VideoSource::on_get_property
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
    value.clear();

    auto iter = _prop_get_handler.find(interface.name() + "." + property);
    if (iter == _prop_get_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second(*_video_source, interface, property, value);
}

void VideoSource::on_set_property
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
    auto iter = _prop_set_handler.find(interface.name() + "." + property);
    if (iter == _prop_set_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second(*_video_source, interface, property, value);
}

} // namespace DBus
