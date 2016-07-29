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

#include <ipcam-runtime.h>

#include "dbus-video-osd.h"

namespace DBus {

#define OSD_INTERFACE	   "ipcam.Media.OSD"

VideoOSD::VideoOSD
(IpcamRuntime &runtime, std::string obj_path, IVideoOSD *video_osd)
  : DBus::ObjectAdaptor(runtime.dbus_conn(), obj_path),
	_runtime(runtime), _video_osd(runtime, video_osd)
{
    assert(video_osd != NULL);

	// Get Handlers
	_prop_get_handler[OSD_INTERFACE ".Type"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32((uint32_t)osd.getType());
        };
	_prop_get_handler[OSD_INTERFACE ".Size"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
			IVideoOSD::Size size = osd.getSize();
			DBus::Struct<uint32_t, uint32_t> s;
			s._1 = size.w;
			s._2 = size.h;
			DBus::MessageIter mi = value.writer();
            mi << s;
        };
	_prop_get_handler[OSD_INTERFACE ".Position"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
			IVideoOSD::Position pos = osd.getPosition();
			DBus::Struct<int32_t, int32_t> s;
			s._1 = pos.x;
			s._2 = pos.y;
			DBus::MessageIter mi = value.writer();
            mi << s;
        };
	_prop_get_handler[OSD_INTERFACE ".BackgroundColor"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32((uint32_t)osd.getBackgroundColor());
        };
	_prop_get_handler[OSD_INTERFACE ".BackgroundAlpha"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32((uint32_t)osd.getBackgroundAlpha());
        };
	_prop_get_handler[OSD_INTERFACE ".ForegroundAlpha"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32((uint32_t)osd.getForegroundAlpha());
        };
	_prop_get_handler[OSD_INTERFACE ".InvertColor"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_bool((uint32_t)osd.getInvertColor());
        };
	_prop_get_handler[OSD_INTERFACE ".Text"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_string(osd.getText().c_str());
        };
	_prop_get_handler[OSD_INTERFACE ".FontName"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_string(osd.getFontName().c_str());
        };
	_prop_get_handler[OSD_INTERFACE ".FontColor"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(osd.getFontColor());
        };
	_prop_get_handler[OSD_INTERFACE ".ImagePath"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
        {
            value.writer().append_string(osd.getImagePath().c_str());
        };

	// Set Handlers
	_prop_set_handler[OSD_INTERFACE ".Type"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
            osd.setType((IpcamVideoOSD::OSDType)(uint32_t)value);
        };
	_prop_set_handler[OSD_INTERFACE ".Size"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
			DBus::Struct<uint32_t, uint32_t> s = value;
			IVideoOSD::Size size(s._1, s._2);
			osd.setSize(size);
        };
	_prop_set_handler[OSD_INTERFACE ".Position"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
			DBus::Struct<int32_t, int32_t> s = value;
			IVideoOSD::Position pos(s._1, s._2);
			osd.setPosition(pos);
        };
	_prop_set_handler[OSD_INTERFACE ".BackgroundColor"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
            osd.setBackgroundColor((uint32_t)value);
        };
	_prop_set_handler[OSD_INTERFACE ".BackgroundAlpha"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
            osd.setBackgroundAlpha((uint32_t)value);
        };
	_prop_set_handler[OSD_INTERFACE ".ForegroundAlpha"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
            osd.setForegroundAlpha((uint32_t)value);
        };
	_prop_set_handler[OSD_INTERFACE ".InvertColor"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
            osd.setInvertColor((bool)value);
        };
	_prop_set_handler[OSD_INTERFACE ".Text"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
			std::string s = value;
            osd.setText(s);
        };
	_prop_set_handler[OSD_INTERFACE ".FontName"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
			std::string s = value;
            osd.setFontName(s);
        };
	_prop_set_handler[OSD_INTERFACE ".FontColor"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
            osd.setFontColor((uint32_t)value);
        };
	_prop_set_handler[OSD_INTERFACE ".ImagePath"] = 
		[](IpcamVideoOSD &osd, DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
        {
			std::string s = value;
            osd.setImagePath(s);
        };
}

VideoOSD::~VideoOSD()
{
}

void VideoOSD::on_get_property
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
    value.clear();

    auto iter = _prop_get_handler.find(interface.name() + "." + property);
    if (iter == _prop_get_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second(_video_osd, interface, property, value);
}

void VideoOSD::on_set_property
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
    auto iter = _prop_set_handler.find(interface.name() + "." + property);
    if (iter == _prop_set_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second(_video_osd, interface, property, value);
}

} // namespace DBus
