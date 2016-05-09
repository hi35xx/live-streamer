/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * dbus-audio-source.cpp
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

#include "dbus-audio-source.h"

namespace DBus {

AudioSource::AudioSource
(IpcamRuntime &runtime, std::string obj_path, IAudioSource *source)
  : DBus::ObjectAdaptor(runtime.dbus_conn(), obj_path),
	_runtime(runtime), _audio_source(source)
{
	assert(source != NULL);
}

void AudioSource::on_get_property
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
	value.clear();
	DBus::MessageIter mi = value.writer();

	if (interface.name() == "ipcam.Media.AudioSource") {
		if (property == "Channels") {
			mi.append_uint32((uint32_t)_audio_source->getChannels());
		}
	}
}

void AudioSource::on_set_property
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
	if (interface.name() == "ipcam.Media.AudioSource") {
		if (property == "Channels") {
			//__audio_source->setChannels(value);
		}
	}
}

} // namespace DBus
