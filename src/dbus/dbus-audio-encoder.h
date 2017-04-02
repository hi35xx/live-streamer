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

#include "dbus-ipcam-base.h"
#include "audio-encoder-server-glue.h"

using namespace Ipcam::Media;

namespace DBus {

class AudioEncoder :
  public ipcam::Media::AudioEncoder_adaptor,
  public IpcamBase
{
public:
	AudioEncoder(IpcamRuntime &runtime, std::string obj_path, Ipcam::Media::AudioEncoder* encoder);
	void do_property_get
		 (DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value);
	void do_property_set
		 (DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value);
protected:
	Ipcam::Media::AudioEncoder* _audio_encoder;
};

} // namespace DBus

#endif // _DBUS_AUDIO_ENCODER_H_

