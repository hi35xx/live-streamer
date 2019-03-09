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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fstream>
#include <RTSPServer.hh>
#include <ServerMediaSession.hh>
#include <FramedFilter.hh>
#include <LiveStreamInput.hh>
#include <LiveStreamOutput.hh>

#include <dbus-c++/dbus.h>
#include <ipcam-runtime.h>

#include <dbus-audio-source.h>
#include <dbus-audio-encoder.h>
#include <dbus-video-source.h>
#include <dbus-video-encoder.h>

const char *VIDEO_SOURCE_SERVER_PATH = "/ipcam/Media/VideoSource";
const char *VIDEO_ENCODER_SERVER_PATH = "/ipcam/Media/VideoEncoder";
const char *AUDIO_SOURCE_SERVER_PATH = "/ipcam/Media/AudioSource";
const char *AUDIO_ENCODER_SERVER_PATH = "/ipcam/Media/AudioEncoder";

IpcamRuntime::IpcamRuntime
(std::string config_name, ev::default_loop &loop, RTSPServer *rtspServer, DBus::Connection *conn)
  : _loop(loop), _rtsp_server(rtspServer), _dbus_connection(conn)
#ifdef HAVE_JSONCPP_SUPPORT
    , _config_name(config_name), _config_dirty(false), _config_timer(loop)
#endif
{
#ifdef HAVE_JSONCPP_SUPPORT
	if (_config_name.size() > 0) {
		std::ifstream ifs(config_name.c_str());
		if (ifs.is_open()) {
			Json::CharReaderBuilder rbuilder;
			rbuilder["collectComments"] = true;
			std::string errs;
			if (!Json::parseFromStream(rbuilder, ifs, &_config_root, &errs))
				fprintf(stderr, "%s", errs.c_str());
		}
	}
#endif
}

IpcamRuntime::~IpcamRuntime()
{
#ifdef HAVE_JSONCPP_SUPPORT
	if (_config_dirty && _config_name.size() > 0) {
		std::ofstream ofs(_config_name.c_str());
		if (ofs.is_open()) {
			Json::StreamWriterBuilder wbuilder;
			wbuilder["commentStyle"] = "All";
			wbuilder["indentation"] = "  ";
			std::unique_ptr<Json::StreamWriter> writer(wbuilder.newStreamWriter());
			writer->write(_config_root, &ofs);
		}
		_config_dirty = false;
	}
#endif
}

RTSPStream IpcamRuntime::addRTSPStream
(const std::string& path, VideoStreamSource* video, AudioStreamSource* audio)
{
	char const* description = "RTSP/RTP stream from live-streamer";
	ServerMediaSession *sms;
	sms = ServerMediaSession::createNew(_rtsp_server->envir(),
	                                    path.c_str(),
	                                    path.c_str(),
	                                    description);
	if (sms) {
		if (video) {
			ServerMediaSubsession* vsmss = NULL;
			switch (video->encoding()) {
			case H264:
				vsmss = LiveH264VideoServerMediaSubsession
					::createNew(_rtsp_server->envir(), 
					            H264_VIDEO_STREAM_SOURCE(video));
				break;
			case JPEG:
			case MJPEG:
				vsmss = LiveJPEGVideoServerMediaSubsession
					::createNew(_rtsp_server->envir(),
					            JPEG_VIDEO_STREAM_SOURCE(video));
				break;
			default:
				break;
			}
			if (vsmss) {
				sms->addSubsession(vsmss);
			}
		}
		if (audio) {
			LiveAudioServerMediaSubsession* asmss;
			asmss = LiveAudioServerMediaSubsession
				::createNew(_rtsp_server->envir(), audio);
			sms->addSubsession(asmss);
		}

		_rtsp_server->addServerMediaSession(sms);

		_stream_list.push_back(sms);
	}

	return sms;
}

void IpcamRuntime::addAudioSource(AudioSource* audio_source)
{
	int index = _audio_source_list.size();
	std::string obj_path = std::string(AUDIO_SOURCE_SERVER_PATH)
		+ std::to_string(index);
	_audio_source_list.emplace_back(new DBus::AudioSource(*this, obj_path, audio_source));
}

void IpcamRuntime::addAudioEncoder(AudioEncoder* audio_encoder)
{
	int index = _audio_encoder_list.size();
	std::string obj_path = std::string(AUDIO_ENCODER_SERVER_PATH)
		+ std::to_string(index);
	_audio_encoder_list.emplace_back(new DBus::AudioEncoder(*this, obj_path, audio_encoder));
}

void IpcamRuntime::addVideoSource(VideoSource* video_source)
{
	int index = _video_source_list.size();
	std::string obj_path = std::string(VIDEO_SOURCE_SERVER_PATH)
		+ std::to_string(index);
	_video_source_list.emplace_back(new DBus::VideoSource(*this, obj_path, video_source));
}

void IpcamRuntime::addVideoEncoder(VideoEncoder* video_encoder)
{
	int index = _video_encoder_list.size();
	std::string obj_path = std::string(VIDEO_ENCODER_SERVER_PATH)
		+ std::to_string(index);

	switch (video_encoder->getEncoding()) {
	case H264:
		_video_encoder_list.emplace_back(new DBus::H264VideoEncoder(*this, obj_path, H264_VIDEO_ENCODER(video_encoder)));
		break;
	default:
		_video_encoder_list.emplace_back(new DBus::VideoEncoder(*this, obj_path, video_encoder));
		break;
	}
}

void IpcamRuntime::addAudioOutputStream(in_addr addr, uint16_t portNum, AudioStreamSink* streamSink)
{
	UsageEnvironment &envir = _rtsp_server->envir();

	_aout_stream_list.emplace_back(new AudioOutputStream(envir, *streamSink, addr, portNum));
}

#ifdef HAVE_JSONCPP_SUPPORT
void IpcamRuntime::LoadConfig()
{
	for (auto &asrc : _audio_source_list)
		asrc->LoadConfig();
	for (auto &aenc : _audio_encoder_list)
		aenc->LoadConfig();
	for (auto &vsrc : _video_source_list)
		vsrc->LoadConfig();
	for (auto &venc : _video_encoder_list)
		venc->LoadConfig();
}

void IpcamRuntime::config_timer_handler(ev::timer &w, int revents)
{
	if (_config_dirty && _config_name.size() > 0) {
		std::ofstream ofs(_config_name.c_str());
		if (ofs.is_open()) {
			Json::StreamWriterBuilder wbuilder;
			wbuilder["commentStyle"] = "All";
			wbuilder["indentation"] = "  ";
			std::unique_ptr<Json::StreamWriter> writer(wbuilder.newStreamWriter());
			writer->write(_config_root, &ofs);
		}
		_config_dirty = false;
	}
}

void IpcamRuntime::SaveConfig()
{
	_config_dirty = true;
	_config_timer.set(20.0, 0);
	_config_timer.set<IpcamRuntime, &IpcamRuntime::config_timer_handler>(this);
	_config_timer.start();
}
#endif
