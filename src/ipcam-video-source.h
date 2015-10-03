/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ipcam-video-source.h
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

#ifndef _IPCAM_VIDEO_SOURCE_H_
#define _IPCAM_VIDEO_SOURCE_H_

#include <dbus-c++/dbus.h>

#include "ipcam-media.h"
#include "video-source-server-glue.h"

class IpcamVideoSource :
  public ipcam::Media::VideoSource_adaptor,
  public ipcam::Media::VideoSource::Imaging_adaptor,
  public ipcam::Media::VideoSource::Imaging::LDC_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::PropertiesAdaptor,
  public DBus::ObjectAdaptor
{
public:
	IpcamVideoSource(DBus::Connection &connection, std::string obj_path, Ipcam::Media::VideoSource *video_source);

	void on_set_property
		(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value);
protected:
    Ipcam::Media::VideoSource *_video_source;
};

#endif // _IPCAM_VIDEO_SOURCE_H_

