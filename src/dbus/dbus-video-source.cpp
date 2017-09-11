/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#define VIDEOSOURCE_INTERFACE	"ipcam.Media.VideoSource"
#define IMAGING_INTERFACE		"ipcam.Media.VideoSource.Imaging"
#define ANTIFLICKER_INTERFACE	"ipcam.Media.VideoSource.Imaging.AntiFlicker"
#define BLC_INTERFACE			"ipcam.Media.VideoSource.Imaging.BacklightCompensation"
#define FOCUS_INTERFACE			"ipcam.Media.VideoSource.Imaging.Focus"
#define EXPOSURE_INTERFACE		"ipcam.Media.VideoSource.Imaging.Exposure"
#define WB_INTERFACE			"ipcam.Media.VideoSource.Imaging.WhiteBalance"
#define WDR_INTERFACE			"ipcam.Media.VideoSource.Imaging.WideDynamicRange"
#define LDC_INTERFACE			"ipcam.Media.VideoSource.Imaging.LDC"
#define GAMMA_INTERFACE			"ipcam.Media.VideoSource.Imaging.Gamma"
#define NR_INTERFACE			"ipcam.Media.VideoSource.Imaging.NoiseReduction"

#define DEFINE_PROP(prop, get, set) \
	_prop_handler.emplace(std::piecewise_construct, \
		std::forward_as_tuple(prop), \
		std::forward_as_tuple(get, set))

VideoSource::VideoSource
(IpcamRuntime &runtime, std::string obj_path, Ipcam::Media::VideoSource* video_source)
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
			Ipcam::Media::Resolution r(s);
			_video_source->setResolution(r);
		});
	DEFINE_PROP(VIDEOSOURCE_INTERFACE ".Framerate",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			DBus::MessageIter wi = value.writer();
			wi << _video_source->getFrameRate();
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->setFrameRate((uint32_t)value);
		});
	// Handler of ipcam.Media.VideoSource.Imaging
	DEFINE_PROP(IMAGING_INTERFACE ".Mirror",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			DBus::MessageIter wi = value.writer();
			wi << _video_source->imaging().getMirror();
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().setMirror((bool)value);
		});
	DEFINE_PROP(IMAGING_INTERFACE ".Flip",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			DBus::MessageIter wi = value.writer();
			wi << _video_source->imaging().getFlip();
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().setFlip((bool)value);
		});
	DEFINE_PROP(IMAGING_INTERFACE ".Brightness",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			DBus::MessageIter wi = value.writer();
			wi << _video_source->imaging().getBrightness();
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().setBrightness((uint32_t)value);
		});
	DEFINE_PROP(IMAGING_INTERFACE ".Contrast", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().getContrast());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().setContrast((uint32_t)value);
		});
	DEFINE_PROP(IMAGING_INTERFACE ".Chroma",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().getChroma());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().setChroma((uint32_t)value);
		});
	DEFINE_PROP(IMAGING_INTERFACE ".Saturation",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().getSaturation());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().setSaturation((uint32_t)value);
		});
	DEFINE_PROP(IMAGING_INTERFACE ".Sharpness",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().getSharpness());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().setSharpness((uint32_t)value);
		});
	DEFINE_PROP(IMAGING_INTERFACE ".IrCutFilterMode",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().getIrCutFilterMode());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().setIrCutFilterMode((Ipcam::Media::IrCutFilterMode)(uint32_t)value);
		});
	// Handler of ipcam.Media.VideoSource.Imaging.AntiFlicker
	DEFINE_PROP(ANTIFLICKER_INTERFACE ".Mode", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().antiflicker().getMode());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().antiflicker().setMode((AntiFlickerMode)(uint32_t)value);
		});
	DEFINE_PROP(ANTIFLICKER_INTERFACE ".Frequency", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().antiflicker().getFrequency());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().antiflicker().setFrequency((uint32_t)value);
		});
	// Handler of ipcam.Media.VideoSource.Imaging.Backlight
	DEFINE_PROP(BLC_INTERFACE ".Mode", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().backlight().getMode());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().backlight().setMode((BLCMode)(uint32_t)value);
		});
	DEFINE_PROP(BLC_INTERFACE ".Level", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().backlight().getLevel());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().backlight().setLevel((uint32_t)value);
		});
	// Handler of ipcam.Media.VideoSource.Imaging.Focus
	DEFINE_PROP(FOCUS_INTERFACE ".AutoFocusMode", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().focus().getMode());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().focus().setMode((FocusMode)(uint32_t)value);
		});
	DEFINE_PROP(FOCUS_INTERFACE ".DefaultSpeed", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().focus().getDefaultSpeed());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().focus().setDefaultSpeed((uint32_t)value);
		});
	DEFINE_PROP(FOCUS_INTERFACE ".NearLimit", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().focus().getNearLimit());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().focus().setNearLimit((uint32_t)value);
		});
	DEFINE_PROP(FOCUS_INTERFACE ".FarLimit", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().focus().getFarLimit());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().focus().setFarLimit((uint32_t)value);
		});
	// Handler of ipcam.Media.VideoSource.Imaging.Exposure
	DEFINE_PROP(EXPOSURE_INTERFACE ".Mode", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().exposure().getMode());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().exposure().setMode((ExposureMode)(uint32_t)value);
		});
	DEFINE_PROP(EXPOSURE_INTERFACE ".Priority", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().exposure().getPriority());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().exposure().setPriority((ExposurePriority)(uint32_t)value);
		});
	DEFINE_PROP(EXPOSURE_INTERFACE ".MinExposureTime", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().exposure().getMinExposureTime());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().exposure().setMinExposureTime((uint32_t)value);
		});
	DEFINE_PROP(EXPOSURE_INTERFACE ".MaxExposureTime", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().exposure().getMaxExposureTime());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().exposure().setMaxExposureTime((uint32_t)value);
		});
	DEFINE_PROP(EXPOSURE_INTERFACE ".MinGain", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().exposure().getMinGain());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().exposure().setMinGain((uint32_t)value);
		});
	DEFINE_PROP(EXPOSURE_INTERFACE ".MaxGain", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().exposure().getMaxGain());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().exposure().setMaxGain((uint32_t)value);
		});
	DEFINE_PROP(EXPOSURE_INTERFACE ".MinIris", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().exposure().getMinIris());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().exposure().setMinIris((uint32_t)value);
		});
	DEFINE_PROP(EXPOSURE_INTERFACE ".MaxIris", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().exposure().getMaxIris());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().exposure().setMaxIris((uint32_t)value);
		});
	DEFINE_PROP(EXPOSURE_INTERFACE ".ExposureTime", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().exposure().getExposureTime());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().exposure().setExposureTime((uint32_t)value);
		});
	DEFINE_PROP(EXPOSURE_INTERFACE ".Compensation", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().exposure().getCompensation());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().exposure().setCompensation((uint32_t)value);
		});
	DEFINE_PROP(EXPOSURE_INTERFACE ".Gain", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().exposure().getGain());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().exposure().setGain((uint32_t)value);
		});
	DEFINE_PROP(EXPOSURE_INTERFACE ".Iris", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().exposure().getIris());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().exposure().setIris((uint32_t)value);
		});
	// Handler of ipcam.Media.VideoSource.Imaging.WhiteBalance
	DEFINE_PROP(WB_INTERFACE ".Mode", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().whitebalance().getMode());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().whitebalance().setMode((WhiteBalanceMode)(uint32_t)value);
		});
	DEFINE_PROP(WB_INTERFACE ".CrGain", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().whitebalance().getCrGain());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().whitebalance().setCrGain((uint32_t)value);
		});
	DEFINE_PROP(WB_INTERFACE ".CbGain", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().whitebalance().getCbGain());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().whitebalance().setCbGain((uint32_t)value);
		});
	DEFINE_PROP(WB_INTERFACE ".RGain", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().whitebalance().getRGain());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().whitebalance().setRGain((uint32_t)value);
		});
	DEFINE_PROP(WB_INTERFACE ".GGain", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().whitebalance().getGGain());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().whitebalance().setGGain((uint32_t)value);
		});
	DEFINE_PROP(WB_INTERFACE ".BGain", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().whitebalance().getBGain());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().whitebalance().setBGain((uint32_t)value);
		});
	// Handler of ipcam.Media.VideoSource.Imaging.WideDynamicRange
	DEFINE_PROP(WDR_INTERFACE ".Mode", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().widedynamicrange().getMode());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().widedynamicrange().setMode((WDRMode)(uint32_t)value);
		});
	DEFINE_PROP(WDR_INTERFACE ".Level", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().widedynamicrange().getLevel());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().widedynamicrange().setLevel((uint32_t)value);
		});
	// Handler of ipcam.Media.VideoSource.Imaging.LDC
	DEFINE_PROP(LDC_INTERFACE ".Mode", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().ldc().getMode());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().ldc().setMode((LDCMode)(uint32_t)value);
		});
	DEFINE_PROP(LDC_INTERFACE ".Ratio", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_source->imaging().ldc().getRatio());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_source->imaging().ldc().setRatio((uint32_t)value);
		});
	// Handler of ipcam.Media.VideoSource.Imaging.LDC
	DEFINE_PROP(GAMMA_INTERFACE ".CurveData", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			std::vector<uint32_t>& curve = _video_source->imaging().gamma().getCurveData();
			DBus::MessageIter mi = value.writer();
			mi << curve;
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			std::vector<uint32_t> curve = value;
			_video_source->imaging().gamma().setCurveData(curve);
		});
	// Handler of ipcam.Media.VideoSource.Imaging.NoiseReduction
	DEFINE_PROP(NR_INTERFACE ".Mode", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			VNRMode mode = _video_source->imaging().noisereduction().getMode();
			DBus::MessageIter mi = value.writer();
			mi << (uint32_t)mode;
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			VNRMode mode = (VNRMode)(uint32_t)value;
			_video_source->imaging().noisereduction().setMode(mode);
		});
	DEFINE_PROP(NR_INTERFACE ".Level", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			uint32_t level = _video_source->imaging().noisereduction().getLevel();
			DBus::MessageIter mi = value.writer();
			mi << (uint32_t)level;
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			uint32_t level = (uint32_t)value;
			_video_source->imaging().noisereduction().setLevel(level);
		});
	DEFINE_PROP(NR_INTERFACE ".ParamTable", 
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			auto& table = _video_source->imaging().noisereduction().getParamTable();
			DBus::MessageIter mi = value.writer();
			mi << table;
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			NrParamTable table = value;
			_video_source->imaging().noisereduction().setParamTable(table);
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
