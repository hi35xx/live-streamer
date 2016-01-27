/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ipcam-runtime.cpp
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

#include <LiveStreamInput.hh>
#include <LiveStreamInput.hh>
#include "ipcam-runtime.h"

const char *VIDEO_SOURCE_SERVER_PATH = "/ipcam/Media/VideoSource";
const char *VIDEO_ENCODER_SERVER_PATH = "/ipcam/Media/VideoEncoder";
const char *AUDIO_SOURCE_SERVER_PATH = "/ipcam/Media/AudioSource";
const char *AUDIO_ENCODER_SERVER_PATH = "/ipcam/Media/AudioEncoder";

IpcamRuntime::IpcamRuntime
(ev::default_loop &loop, RTSPServer *rtspServer, DBus::Connection *conn)
	: _loop(loop), _rtsp_server(rtspServer), _dbus_connection(conn)
{
}

IpcamRuntime::~IpcamRuntime()
{
}

RTSPStream IpcamRuntime::addRTSPStream
(IVideoStream *video, IAudioStream *audio)
{
    char const* description = "RTSP/RTP stream from live-streamer";
    ServerMediaSession *sms;
	std::string stream_path = std::to_string(_stream_list.size());
    sms = ServerMediaSession::createNew(_rtsp_server->envir(),
                                        stream_path.c_str(),
                                        stream_path.c_str(),
                                        description);
    if (sms) {
        if (video) {
            LiveVideoServerMediaSubsession* vsmss;
            vsmss = LiveVideoServerMediaSubsession
                ::createNew(_rtsp_server->envir(), *video);
            sms->addSubsession(vsmss);
        }
		if (audio) {
			LiveAudioServerMediaSubsession* asmss;
			asmss = LiveAudioServerMediaSubsession
				::createNew(_rtsp_server->envir(), *audio);
			sms->addSubsession(asmss);
		}

		_rtsp_server->addServerMediaSession(sms);

		_stream_list.push_back(sms);
    }

	return sms;
}

void IpcamRuntime::addAudioSourceInterface(IAudioSource *audio_source)
{
	int index = _audio_source_list.size();
	std::string obj_path = std::string(AUDIO_SOURCE_SERVER_PATH)
		+ std::to_string(index);
    _audio_source_list.emplace_back(*_dbus_connection, obj_path, audio_source);
}

void IpcamRuntime::addAudioEncoderInterface(IAudioEncoder *audio_encoder)
{
	int index = _audio_encoder_list.size();
	std::string obj_path = std::string(AUDIO_ENCODER_SERVER_PATH)
		+ std::to_string(index);
    _audio_encoder_list.emplace_back(*_dbus_connection, obj_path, audio_encoder);
}

void IpcamRuntime::addVideoSourceInterface(IVideoSource *video_source)
{
	int index = _video_source_list.size();
	std::string obj_path = std::string(VIDEO_SOURCE_SERVER_PATH)
		+ std::to_string(index);
    _video_source_list.emplace_back(*_dbus_connection, obj_path, video_source);
}

void IpcamRuntime::addVideoEncoderInterface(IVideoEncoder *video_encoder)
{
	int index = _video_encoder_list.size();
	std::string obj_path = std::string(VIDEO_ENCODER_SERVER_PATH)
		+ std::to_string(index);
    _video_encoder_list.emplace_back(*_dbus_connection, obj_path, video_encoder);
}
