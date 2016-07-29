/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * dbus-audio-source.h
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

#ifndef _DBUS_AUDIO_SOURCE_H_
#define _DBUS_AUDIO_SOURCE_H_

#include <functional>
#include <unordered_map>

#include "audio-source-server-glue.h"

using namespace Ipcam::Media;

namespace DBus {

class AudioSource :
  public ipcam::Media::AudioSource_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::PropertiesAdaptor,
  public DBus::ObjectAdaptor
{
public:
	AudioSource(IpcamRuntime &runtime, std::string obj_path, IAudioSource *source);
	void on_get_property
		 (DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value);
	void on_set_property
		 (DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value);
protected:
	IpcamRuntime& _runtime;
	IAudioSource* _audio_source;
    typedef std::function<void(IAudioSource&, DBus::InterfaceAdaptor&, const std::string&, DBus::Variant&)> PropertyGetHandler;
    typedef std::function<void(IAudioSource&, DBus::InterfaceAdaptor&, const std::string&, const DBus::Variant&)> PropertySetHandler;
    std::unordered_map<std::string, PropertyGetHandler> _prop_get_handler;
    std::unordered_map<std::string, PropertySetHandler> _prop_set_handler;
};

} // namespace DBus

#endif // _DBUS_AUDIO_SOURCE_H_

