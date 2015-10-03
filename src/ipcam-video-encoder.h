/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ipcam-video-encoder.h
 * Copyright (C) 2015 Watson Xu <watson@localhost.localdomain>
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

#ifndef _IPCAM_VIDEO_ENCODER_H_
#define _IPCAM_VIDEO_ENCODER_H_

#include <dbus-c++/dbus.h>
#include "ipcam-media.h"
#include "video-encoder-server-glue.h"

class IpcamVideoEncoder : 
  public ipcam::Media::VideoEncoder_adaptor,
  public ipcam::Media::VideoEncoder::RateControl_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::PropertiesAdaptor,
  public DBus::ObjectAdaptor
{
public:
    IpcamVideoEncoder(DBus::Connection &connection, std::string obj_path, Ipcam::Media::VideoEncoder *encoder);

	void on_set_property
		(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value);
protected:
    Ipcam::Media::VideoEncoder *_video_encoder;
};

class IpcamH264VideoEncoder : 
  public IpcamVideoEncoder,
  public ipcam::Media::VideoEncoder::H264_adaptor
{
public:
    IpcamH264VideoEncoder(DBus::Connection &connection, std::string obj_path, Ipcam::Media::H264VideoEncoder *encoder);

	void on_set_property
		(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value);
};

#endif // _IPCAM_VIDEO_ENCODER_H_

