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

#define AUDIOSOURCE_INTERFACE	   "ipcam.Media.AudioSource"

#define DEFINE_PROP(prop, get, set) \
    _prop_handler.emplace(std::piecewise_construct, \
        std::forward_as_tuple(prop), \
        std::forward_as_tuple(get, set))

AudioSource::AudioSource
(IpcamRuntime &runtime, std::string obj_path, IAudioSource *source)
  : DBus::ObjectAdaptor(runtime.dbus_conn(), obj_path),
	_runtime(runtime), _audio_source(source)
{
	assert(source != NULL);

    // Get handler of ipcam.Media.Source
    DEFINE_PROP(AUDIOSOURCE_INTERFACE ".Channels",
        [](IAudioSource &asrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(asrc.getChannels());
        },
        [](IAudioSource &asrc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            throw DBus::ErrorFailed("Readonly property");
        });
}

void AudioSource::on_get_property
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
	value.clear();

    auto iter = _prop_handler.find(interface.name() + "." + property);
    if (iter == _prop_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second.Get(*_audio_source, interface, property, value);
}

void AudioSource::on_set_property
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
    auto iter = _prop_handler.find(interface.name() + "." + property);
    if (iter == _prop_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second.Set(*_audio_source, interface, property, value);
}

} // namespace DBus
