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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <media-stream.h>
#include <video-source.h>
#include <video-encoder.h>
#include <audio-source.h>
#include <audio-encoder.h>

#ifdef HAVE_JSONCPP_SUPPORT
#include <json/json.h>
#endif

using namespace Ipcam::Media;

class RTSPServer;

class ServerMediaSession;
class ServerMediaSubsession;
class AudioOutputStream;

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
	IpcamRuntime(std::string config_file, ev::default_loop &loop, RTSPServer *rtspServer, DBus::Connection *conn);
	~IpcamRuntime();

	RTSPStream addRTSPStream(const std::string& path, VideoStreamSource* video, AudioStreamSource* audio);
	void addAudioSource(AudioSource* audio_source);
	void addAudioEncoder(AudioEncoder* audio_encoder);
	void addVideoSource(VideoSource* video_source);
	void addVideoEncoder(VideoEncoder* video_encoder);

	void addAudioOutputStream(in_addr addr, uint16_t portNum, AudioStreamSink* sink);

	ev::default_loop &mainloop() { return _loop; }
	DBus::Connection &dbus_conn() { return *_dbus_connection; }
#ifdef HAVE_JSONCPP_SUPPORT
	Json::Value &config_root() { return _config_root; }
	void LoadConfig();
	void SaveConfig();
#endif

private:
	ev::default_loop&		_loop;
	RTSPServer*				_rtsp_server;
	DBus::Connection*		_dbus_connection;
#ifdef HAVE_JSONCPP_SUPPORT
	Json::Value				_config_root;
	std::string				_config_name;
	bool					_config_dirty;
	ev::timer				_config_timer;
	void config_timer_handler(ev::timer& w, int revents);
#endif

	typedef std::list<ServerMediaSession*> IpcamStreamList;
	typedef std::list<std::unique_ptr<DBus::AudioSource>>	IpcamAudioSourceList;
	typedef std::list<std::unique_ptr<DBus::AudioEncoder>>	IpcamAudioEncoderList;
	typedef std::list<std::unique_ptr<DBus::VideoSource>>	IpcamVideoSourceList;
	typedef std::list<std::unique_ptr<DBus::VideoEncoder>>	IpcamVideoEncoderList;
	typedef std::list<std::unique_ptr<AudioOutputStream>>  IpcamAudioOutStreamList;

	IpcamStreamList			_stream_list;
	IpcamAudioSourceList	_audio_source_list;
	IpcamAudioEncoderList   _audio_encoder_list;
	IpcamVideoSourceList	_video_source_list;
	IpcamVideoEncoderList   _video_encoder_list;
	IpcamAudioOutStreamList _aout_stream_list;
};

#endif // _IPCAM_RUNTIME_H_

