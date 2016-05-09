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

VideoOSD::VideoOSD
(IpcamRuntime &runtime, std::string obj_path, IVideoOSD *video_osd)
  : DBus::ObjectAdaptor(runtime.dbus_conn(), obj_path),
	_runtime(runtime), _video_osd(runtime, video_osd)
{
    assert(video_osd != NULL);
}

VideoOSD::~VideoOSD()
{
}

void VideoOSD::on_get_property
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
    value.clear();
    DBus::MessageIter mi = value.writer();

    if (interface.name() == "ipcam.Media.OSD") {
		if (property == "Type") {
			mi.append_uint32((uint32_t)_video_osd.getType());
		}
        else if (property == "Size") {
			IVideoOSD::Size size = _video_osd.getSize();
			DBus::Struct< uint32_t, uint32_t > s;
			s._1 = size.w;
			s._2 = size.h;
			mi << s;
        }
		else if (property == "Position") {
			IVideoOSD::Position pos = _video_osd.getPosition();
			DBus::Struct<int32_t, int32_t> s;
			s._1 = pos.x;
			s._2 = pos.y;
			mi << s;
		}
		else if (property == "BackgroundColor") {
			mi << (uint32_t)_video_osd.getBackgroundColor();
		}
		else if (property == "BackgroundAlpha") {
			mi << (uint32_t)_video_osd.getBackgroundAlpha();
		}
		else if (property == "ForegroundAlpha") {
			mi << (uint32_t)_video_osd.getForegroundAlpha();
		}
		else if (property == "InvertColor") {
			mi << (bool)_video_osd.getInvertColor();
		}
		/* Text Properties */
		if (property == "Text") {
			mi << std::string(_video_osd.getText());
		}
		else if (property == "FontName") {
			mi << std::string(_video_osd.getFontName());
		}
		else if (property == "FontColor") {
			mi << (uint32_t)_video_osd.getFontColor();
		}
		/* Image Properties */
		if (property == "ImagePath") {
			mi << std::string(_video_osd.getImagePath());
		}
	}
}

void VideoOSD::on_set_property
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
	if (interface.name() == "ipcam.Media.OSD") {
		if (property == "Type") {
			_video_osd.setType((IpcamVideoOSD::OSDType)(uint32_t)value);
		}
		else if (property == "Size") {
			DBus::Struct<uint32_t, uint32_t> s = value;

			IVideoOSD::Size size(s._1, s._2);
			_video_osd.setSize(size);
		}
		else if (property == "Position") {
			DBus::Struct<int32_t, int32_t> s = value;

			IVideoOSD::Position pos(s._1, s._2);
			_video_osd.setPosition(pos);
		}
		else if (property == "BackgroundColor") {
			_video_osd.setBackgroundColor(value);
		}
		else if (property == "BackgroundAlpha") {
			_video_osd.setBackgroundAlpha(value);
		}
		else if (property == "ForegroundAlpha") {
			_video_osd.setForegroundAlpha(value);
		}
		else if (property == "InvertColor") {
			_video_osd.setInvertColor(value);
		}
		/* Text Properties */
		if (property == "Text") {
			std::string s = value;
			_video_osd.setText(s.c_str());
		}
		else if (property == "FontName") {
			std::string s = value;
			_video_osd.setFontName(s.c_str());
		}
		else if (property == "FontColor") {
			_video_osd.setFontColor(value);
		}
		/* Image Properties */
		if (property == "ImagePath") {
			std::string s = value;
			_video_osd.setImagePath(s.c_str());
		}
	}
}

} // namespace DBus
