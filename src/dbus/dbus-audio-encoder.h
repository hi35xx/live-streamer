/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * dbus-audio-encoder.h
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

#ifndef _DBUS_AUDIO_ENCODER_H_
#define _DBUS_AUDIO_ENCODER_H_

#include <functional>
#include <unordered_map>

#include "audio-encoder-server-glue.h"

using namespace Ipcam::Media;

namespace DBus {

class AudioEncoder :
  public ipcam::Media::AudioEncoder_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::PropertiesAdaptor,
  public DBus::ObjectAdaptor
{
public:
	AudioEncoder(IpcamRuntime &runtime, std::string obj_path, IAudioEncoder *encoder);
	void on_get_property
		 (DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value);
	void on_set_property
		 (DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value);
protected:
	IpcamRuntime&  _runtime;
	IAudioEncoder* _audio_encoder;
    typedef std::function<void(IAudioEncoder&, DBus::InterfaceAdaptor&, const std::string&, DBus::Variant&)> PropertyGet;
    typedef std::function<void(IAudioEncoder&, DBus::InterfaceAdaptor&, const std::string&, const DBus::Variant&)> PropertySet;
    class PropertyHandler {
    public:
        PropertyHandler(PropertyGet get, PropertySet set)
            : Get(get), Set(set) {}
        PropertyGet Get;
        PropertySet Set;
    };
    std::unordered_map<std::string, PropertyHandler> _prop_handler;
};

} // namespace DBus

#endif // _DBUS_AUDIO_ENCODER_H_

