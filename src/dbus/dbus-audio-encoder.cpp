/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * dbus-audio-encoder.cpp
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

#include "dbus-audio-encoder.h"

namespace DBus {

#define AUDIOENCODER_INTERFACE	  "ipcam.Media.AudioEncoder"

#define DEFINE_PROP(prop, get, set) \
	_prop_handler.emplace(std::piecewise_construct, \
		std::forward_as_tuple(prop), \
		std::forward_as_tuple(get, set))

AudioEncoder::AudioEncoder
(IpcamRuntime &runtime, std::string obj_path, Ipcam::Media::AudioEncoder* encoder)
  : IpcamBase(runtime, obj_path),
    _audio_encoder(encoder)
{
	assert(encoder != NULL);

	// Handler of ipcam.Media.AudioEncoder
	DEFINE_PROP(AUDIOENCODER_INTERFACE ".Encoding",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32((uint32_t)_audio_encoder->getEncoding());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			throw IpcamError("Readonly property");
		});
	DEFINE_PROP(AUDIOENCODER_INTERFACE ".Bitrate",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32((uint32_t)_audio_encoder->getBitrate());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_audio_encoder->setBitrate((uint32_t)value);
		});
	DEFINE_PROP(AUDIOENCODER_INTERFACE ".SampleRate",
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, DBus::Variant &value)
		{
			value.writer().append_uint32((uint32_t)_audio_encoder->getSampleRate());
		},
		[this](DBus::InterfaceAdaptor &interface,
		   const std::string &property, const DBus::Variant &value)
		{
			_audio_encoder->setSampleRate((uint32_t)value);
		});
}

void AudioEncoder::do_property_get
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
	value.clear();

	auto iter = _prop_handler.find(interface.name() + "." + property);
	if (iter == _prop_handler.end())
		throw DBus::ErrorFailed("Requested interface or property not found");
	iter->second.Get(interface, property, value);
}

void AudioEncoder::do_property_set
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
	auto iter = _prop_handler.find(interface.name() + "." + property);
	if (iter == _prop_handler.end())
		throw DBus::ErrorFailed("Requested interface or property not found");
	iter->second.Set(interface, property, value);
}

} //namespace DBus
