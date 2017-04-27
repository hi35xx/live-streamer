/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * dbus-video-encoder.cpp
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dbus-c++/dbus.h>
#include <ipcam-runtime.h>

#include "dbus-video-encoder.h"

namespace DBus {

#define VIDEOENCODER_INTERFACE		"ipcam.Media.VideoEncoder"
#define RATECONTROL_INTERFACE		"ipcam.Media.VideoEncoder.RateControl"
#define H264_INTERFACE				"ipcam.Media.VideoEncoder.H264"

#define DEFINE_PROP(prop, get, set) \
	_prop_handler.emplace(std::piecewise_construct, \
		std::forward_as_tuple(prop), \
		std::forward_as_tuple(get, set))

VideoEncoder::VideoEncoder
(IpcamRuntime &runtime, std::string obj_path, Ipcam::Media::VideoEncoder* encoder)
  : IpcamBase(runtime, obj_path),
    _video_encoder(encoder)
{
	assert(encoder != NULL);

	// Handler of ipcam.Media.VideoEncoder
	DEFINE_PROP(VIDEOENCODER_INTERFACE ".Encoding",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32((uint32_t)_video_encoder->getEncoding());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			throw DBus::ErrorFailed("Readonly property");
		});
	DEFINE_PROP(VIDEOENCODER_INTERFACE ".Resolution",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_string(((std::string)_video_encoder->getResolution()).c_str());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			std::string s = value;
			Ipcam::Media::Resolution r(s);
			_video_encoder->setResolution(r);
		});
	// Handler of ipcam.Media.VideoEncoder.RateControl
	DEFINE_PROP(RATECONTROL_INTERFACE ".RateControlMode",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_encoder->getRcMode());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_encoder->setRcMode((RateCtrlMode)(uint32_t)value);
		});
	DEFINE_PROP(RATECONTROL_INTERFACE ".FrameRate",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_encoder->getFramerate());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_encoder->setFramerate((uint32_t)value);
		});
	DEFINE_PROP(RATECONTROL_INTERFACE ".Bitrate",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32(_video_encoder->getBitrate());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_video_encoder->setBitrate((uint32_t)value);
		});
}

void VideoEncoder::do_property_get
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
	value.clear();

	auto iter = _prop_handler.find(interface.name() + "." + property);
	if (iter == _prop_handler.end())
		throw DBus::ErrorFailed("Requested interface or property not found");
	iter->second.Get(interface, property, value);
}

void VideoEncoder::do_property_set
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
	auto iter = _prop_handler.find(interface.name() + "." + property);
	if (iter == _prop_handler.end())
		throw DBus::ErrorFailed("Requested interface or property not found");
	iter->second.Set(interface, property, value);
}

#ifdef HAVE_JSONCPP_SUPPORT
void VideoEncoder::LoadConfig()
{
	IpcamBase::LoadConfig();

	Json::Value& root = _runtime.config_root();
	// Load OSD configuration according the config
	for (int i = 0; i < 8; i++) {
		std::string opath(path() + "/OSD/" + std::to_string(i));
		if (!root.isMember(opath))
			continue;

		VideoOSDTable::iterator oit = _osds.find(i);
		if (oit == _osds.end()) {
			uint32_t index = CreateOSD();
			oit = _osds.find(index);
		}
		VideoOSD& o = oit->second;
		o.LoadConfig();
	}
}
#endif

uint32_t VideoEncoder::CreateOSD()
{
	uint32_t index = 0;
	while (_osds.find(index) != _osds.end()) index++;
	std::string obj_path = path() + "/OSD/" + std::to_string(index);
	Ipcam::Media::VideoOSD* video_osd = _video_encoder->CreateOSD();
	if (!video_osd)
		throw IpcamError("Create OSD instance failed");

	_osds.emplace(std::piecewise_construct,
				  std::forward_as_tuple(index),
				  std::forward_as_tuple(_runtime, obj_path, video_osd));

	return index;
}

void VideoEncoder::DeleteOSD(const uint32_t& index)
{
	_osds.erase(index);
}

std::map< uint32_t, Path > VideoEncoder::GetOSDs()
{
	std::map<uint32_t, Path> result;
	for (auto it = _osds.begin(); it != _osds.end(); it++) {
		uint32_t index = it->first;
		Path venc_path = path();
		std::string obj_path = path() + "/OSD/" + std::to_string(index);
		result.emplace(index, obj_path);
	}
	return result;
}

H264VideoEncoder::H264VideoEncoder
(IpcamRuntime &runtime, std::string obj_path, Ipcam::Media::H264VideoEncoder* encoder)
  : DBus::VideoEncoder(runtime, obj_path, VIDEO_ENCODER(encoder))
{
	assert(encoder != NULL);

	// Handler of ipcam.Media.VideoEncoder
	DEFINE_PROP(H264_INTERFACE ".H264Profile",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			Ipcam::Media::H264VideoEncoder* h264venc = H264_VIDEO_ENCODER(_video_encoder);
			value.writer().append_uint32((uint32_t)h264venc->getH264Profile());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			Ipcam::Media::H264VideoEncoder* h264venc = H264_VIDEO_ENCODER(_video_encoder);
			h264venc->setH264Profile((Ipcam::Media::H264Profile)(uint32_t)value);
		});
	DEFINE_PROP(H264_INTERFACE ".GovLength",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			Ipcam::Media::H264VideoEncoder* h264venc = H264_VIDEO_ENCODER(_video_encoder);
			value.writer().append_uint32((uint32_t)h264venc->getGovLength());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			Ipcam::Media::H264VideoEncoder* h264venc = H264_VIDEO_ENCODER(_video_encoder);
			h264venc->setGovLength((uint32_t)value);
		});
}

} // namespace DBus
