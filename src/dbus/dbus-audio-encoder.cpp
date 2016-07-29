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

#include <ipcam-runtime.h>

#include "dbus-audio-encoder.h"

namespace DBus {

#define AUDIOENCODER_INTERFACE	  "ipcam.Media.AudioEncoder"

AudioEncoder::AudioEncoder
(IpcamRuntime &runtime, std::string obj_path, IAudioEncoder *encoder)
  : DBus::ObjectAdaptor(runtime.dbus_conn(), obj_path),
	_runtime(runtime), _audio_encoder(encoder)
{
	assert(encoder != NULL);

    // Get handler of ipcam.Media.AudioEncoder
    _prop_get_handler[AUDIOENCODER_INTERFACE ".Encoding"] = 
        [](IAudioEncoder &aenc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32((uint32_t)aenc.getEncoding());
        };
    _prop_get_handler[AUDIOENCODER_INTERFACE ".Bitrate"] = 
        [](IAudioEncoder &aenc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32((uint32_t)aenc.getBitrate());
        };
    _prop_get_handler[AUDIOENCODER_INTERFACE ".SampleRate"] = 
        [](IAudioEncoder &aenc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32((uint32_t)aenc.getSampleRate());
        };

    // Set handler of ipcam.Media.AudioEncoder
    _prop_set_handler[AUDIOENCODER_INTERFACE ".Encoding"] = 
        [](IAudioEncoder &aenc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            aenc.setEncoding((IAudioEncoder::EncodingType)(uint32_t)value);
        };
    _prop_set_handler[AUDIOENCODER_INTERFACE ".Bitrate"] = 
        [](IAudioEncoder &aenc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            aenc.setBitrate((uint32_t)value);
        };
    _prop_set_handler[AUDIOENCODER_INTERFACE ".SampleRate"] = 
        [](IAudioEncoder &aenc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            aenc.setSampleRate((uint32_t)value);
        };
}

void AudioEncoder::on_get_property
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
	value.clear();
	DBus::MessageIter mi = value.writer();

	if (interface.name() == "ipcam.Media.AudioEncoder") {
		if (property == "Encoding") {
			mi.append_uint32((uint32_t)_audio_encoder->getEncoding());
		}
		if (property == "Bitrate") {
			mi.append_uint32(_audio_encoder->getBitrate());
		}
		else if (property == "SampleRate") {
			mi.append_uint32(_audio_encoder->getSampleRate());
		}
	}
}

void AudioEncoder::on_set_property
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
	if (interface.name() == "ipcam.Media.AudioEncoder") {
		if (property == "Encoding") {
			IAudioEncoder::EncodingType encoding;
			encoding = (IAudioEncoder::EncodingType)(uint32_t)value;
			_audio_encoder->setEncoding(encoding);
		}
		if (property == "Bitrate") {
			_audio_encoder->setBitrate(value);
		}
		else if (property == "SampleRate") {
			_audio_encoder->setSampleRate(value);
		}
	}
}

} //namespace DBus
