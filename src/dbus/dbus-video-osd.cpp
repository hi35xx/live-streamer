/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * dbus-video-osd.cc
 * Copyright (C) 2016 Watson Xu <xuhuashan@gmail.com>
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

#include "dbus-video-osd.h"

namespace DBus {

#define OSD_INTERFACE	   "ipcam.Media.OSD"

#define DEFINE_PROP(prop, get, set) \
    _prop_handler.emplace(std::piecewise_construct, \
        std::forward_as_tuple(prop), \
        std::forward_as_tuple(get, set))

VideoOSD::VideoOSD
(IpcamRuntime &runtime, std::string obj_path, IVideoOSD *video_osd)
  : IpcamBase(runtime, obj_path),
    _video_osd(runtime, video_osd)
{
    assert(video_osd != NULL);

	// Handler of ipcam.Media.OSD
	DEFINE_PROP(OSD_INTERFACE ".Type",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32((uint32_t)_video_osd.getType());
        },
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
            _video_osd.setType((IpcamVideoOSD::OSDType)(uint32_t)value);
        });
	DEFINE_PROP(OSD_INTERFACE ".Size",
		([this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
			IVideoOSD::Size size = _video_osd.getSize();
			DBus::Struct<uint32_t, uint32_t> s;
			DBus::MessageIter mi = value.writer();
			s._1 = size.w;
			s._2 = size.h;
            mi << s;
        }),
		([this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
			DBus::Struct<uint32_t, uint32_t> s = value;
			IVideoOSD::Size size(s._1, s._2);
			_video_osd.setSize(size);
        }));
	DEFINE_PROP(OSD_INTERFACE ".Position",
		([this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
			IVideoOSD::Position pos = _video_osd.getPosition();
			DBus::Struct<int32_t, int32_t> s;
			DBus::MessageIter mi = value.writer();
			s._1 = pos.x;
			s._2 = pos.y;
            mi << s;
        }),
		([this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
			DBus::Struct<int32_t, int32_t> s = value;
			IVideoOSD::Position pos(s._1, s._2);
			_video_osd.setPosition(pos);
        }));
	DEFINE_PROP(OSD_INTERFACE ".BackgroundColor",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32((uint32_t)_video_osd.getBackgroundColor());
        },
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
            _video_osd.setBackgroundColor((uint32_t)value);
        });
	DEFINE_PROP(OSD_INTERFACE ".BackgroundAlpha",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32((uint32_t)_video_osd.getBackgroundAlpha());
        },
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
            _video_osd.setBackgroundAlpha((uint32_t)value);
        });
	DEFINE_PROP(OSD_INTERFACE ".ForegroundAlpha",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32((uint32_t)_video_osd.getForegroundAlpha());
        },
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
            _video_osd.setForegroundAlpha((uint32_t)value);
        });
	DEFINE_PROP(OSD_INTERFACE ".InvertColor",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_bool((uint32_t)_video_osd.getInvertColor());
        },
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
            _video_osd.setInvertColor((bool)value);
        });
	DEFINE_PROP(OSD_INTERFACE ".Text",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_string(_video_osd.getText().c_str());
        },
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
			std::string s = value;
            _video_osd.setText(s);
        });
	DEFINE_PROP(OSD_INTERFACE ".FontName",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_string(_video_osd.getFontName().c_str());
        },
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
			std::string s = value;
            _video_osd.setFontName(s);
        });
	DEFINE_PROP(OSD_INTERFACE ".FontColor",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_video_osd.getFontColor());
        },
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
            _video_osd.setFontColor((uint32_t)value);
        });
	DEFINE_PROP(OSD_INTERFACE ".ImagePath",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_string(_video_osd.getImagePath().c_str());
        },
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
			std::string s = value;
            _video_osd.setImagePath(s);
        });
}

void VideoOSD::do_property_get
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
    value.clear();

    auto iter = _prop_handler.find(interface.name() + "." + property);
    if (iter == _prop_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second.Get(interface, property, value);
}

void VideoOSD::do_property_set
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
    auto iter = _prop_handler.find(interface.name() + "." + property);
    if (iter == _prop_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second.Set(interface, property, value);
}

} // namespace DBus
