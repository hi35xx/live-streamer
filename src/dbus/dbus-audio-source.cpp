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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dbus-c++/dbus.h>
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
  : IpcamBase(runtime, obj_path),
    _audio_source(source)
{
	assert(source != NULL);

    // Handler of ipcam.Media.Source
    DEFINE_PROP(AUDIOSOURCE_INTERFACE ".Channels",
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(_audio_source->getChannels());
        },
        [this](DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            throw DBus::ErrorFailed("Readonly property");
        });
}

void AudioSource::do_property_get
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
	value.clear();

    auto iter = _prop_handler.find(interface.name() + "." + property);
    if (iter == _prop_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second.Get(interface, property, value);
}

void AudioSource::do_property_set
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
    auto iter = _prop_handler.find(interface.name() + "." + property);
    if (iter == _prop_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second.Set(interface, property, value);
}

} // namespace DBus
