/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * dbus-video-encoder.h
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

#ifndef _DBUS_VIDEO_ENCODER_H_
#define _DBUS_VIDEO_ENCODER_H_

#include <map>
#include <dbus-c++/dbus.h>
#include <dbus-c++/error.h>
#include "ipcam-media-iface.h"
#include "dbus-video-osd.h"

#include "video-encoder-server-glue.h"

using namespace Ipcam::Media;

namespace DBus {

class VideoEncoder : 
  public ipcam::Media::VideoEncoder_adaptor,
  public ipcam::Media::VideoEncoder::RateControl_adaptor,
  public ipcam::Media::VideoEncoder::OSD_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::PropertiesAdaptor,
  public DBus::ObjectAdaptor
{
public:
    VideoEncoder(IpcamRuntime &runtime, std::string obj_path, IVideoEncoder *encoder);

	void on_get_property
		(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value);
	void on_set_property
		(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value);

    ::DBus::Path CreateOSD(const uint32_t& index);
    void DeleteOSD(const uint32_t& index);
    std::map<uint32_t, ::DBus::Path> GetOSDs();
protected:
    IpcamRuntime &_runtime;
    IVideoEncoder *_video_encoder;
    typedef std::function<void(IVideoEncoder&, DBus::InterfaceAdaptor&, const std::string&, DBus::Variant&)> PropertyGet;
    typedef std::function<void(IVideoEncoder&, DBus::InterfaceAdaptor&, const std::string&, const DBus::Variant&)> PropertySet;
    class PropertyHandler {
    public:
        PropertyHandler(PropertyGet get, PropertySet set)
            : Get(get), Set(set) {}
        PropertyGet Get;
        PropertySet Set;
    };
    std::unordered_map<std::string, PropertyHandler> _prop_handler;
private:
    std::map<uint32_t, VideoOSD> _osds;
};

class H264VideoEncoder : 
  public VideoEncoder,
  public ipcam::Media::VideoEncoder::H264_adaptor
{
public:
    H264VideoEncoder(IpcamRuntime &runtime, std::string obj_path, IH264VideoEncoder *encoder);
};

} // namespace DBus

#endif // _DBUS_VIDEO_ENCODER_H_

