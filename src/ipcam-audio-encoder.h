/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ipcam-audio-encoder.h
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

#ifndef _IPCAM_AUDIO_ENCODER_H_
#define _IPCAM_AUDIO_ENCODER_H_

#include <dbus-c++/dbus.h>
#include "ipcam-media.h"
#include "audio-encoder-server-glue.h"

using namespace Ipcam::Interface;

class IpcamAudioEncoder :
  public ipcam::Media::AudioEncoder_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::PropertiesAdaptor,
  public DBus::ObjectAdaptor
{
public:
	IpcamAudioEncoder(DBus::Connection &connection, std::string obj_path, IAudioEncoder *encoder);
	void on_get_property
		 (DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value);
	void on_set_property
		 (DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value);
protected:
	IAudioEncoder* _audio_encoder;
};

#endif // _IPCAM_AUDIO_ENCODER_H_

