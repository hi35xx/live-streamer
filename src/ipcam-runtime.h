/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ipcam-runtime.h
 * Copyright (C) 2016 Watson Xu <xuhuashan@gmail.com>
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

#ifndef _IPCAM_RUNTIME_H_
#define _IPCAM_RUNTIME_H_

#include <list>
#include <ev++.h>
#include <ipcam-media-iface.h>

using namespace Ipcam::Media;

class RTSPServer;

class ServerMediaSession;
class ServerMediaSubsession;

namespace DBus
{
class Connection;
class AudioSource;
class AudioEncoder;
class VideoSource;
class VideoEncoder;
}

typedef ServerMediaSession* RTSPStream;
typedef ServerMediaSubsession* RTSPSubstream;

class IpcamRuntime
{
public:
	IpcamRuntime(ev::default_loop &loop, RTSPServer *rtspServer, DBus::Connection *conn);
	~IpcamRuntime();

	RTSPStream addRTSPStream(IVideoStream *video, IAudioStream *audio);
	void addAudioSourceInterface(IAudioSource *audio_source);
	void addAudioEncoderInterface(IAudioEncoder *audio_encoder);
	void addVideoSourceInterface(IVideoSource *video_source);
	void addVideoEncoderInterface(IVideoEncoder *video_encoder);

	ev::default_loop &mainloop() { return _loop; }
	DBus::Connection &dbus_conn() { return *_dbus_connection; }
private:
	class IpcamRuntimePriv;
	IpcamRuntimePriv	*_priv;

	ev::default_loop	&_loop;
	RTSPServer          *_rtsp_server;
	DBus::Connection    *_dbus_connection;

	typedef std::list<ServerMediaSession*> IpcamStreamList;
	typedef std::list<std::unique_ptr<DBus::AudioSource>> IpcamAudioSourceList;
	typedef std::list<std::unique_ptr<DBus::AudioEncoder>> IpcamAudioEncoderList;
	typedef std::list<std::unique_ptr<DBus::VideoSource>> IpcamVideoSourceList;
	typedef std::list<std::unique_ptr<DBus::VideoEncoder>> IpcamVideoEncoderList;
	IpcamStreamList			_stream_list;
	IpcamAudioSourceList	_audio_source_list;
	IpcamAudioEncoderList   _audio_encoder_list;
	IpcamVideoSourceList	_video_source_list;
	IpcamVideoEncoderList   _video_encoder_list;
};

#endif // _IPCAM_RUNTIME_H_

