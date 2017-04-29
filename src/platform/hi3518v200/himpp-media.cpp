/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-media.cpp
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

#include <algorithm>
#include <ev++.h>
#include <himpp-video-region.h>
#include <himpp-video-sensor.h>
#include <himpp-video-isp.h>
#include <himpp-video-viu.h>
#include <himpp-video-vpss.h>
#include <himpp-video-venc.h>
#include <himpp-audio.h>

#include "himpp-media.h"


//////////////////////////////////////////////////////////////////////////////
// HimppMedia
//////////////////////////////////////////////////////////////////////////////

static std::vector<std::string> split(const std::string& s, char delim)
{
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string l;
	while (getline(ss, l, delim)) {
		result.push_back(l);
	}
	return result;
}

HimppMedia::HimppMedia(IpcamRuntime *runtime, PlatformArguments& args)
  : _runtime(runtime),
    _sysctl(HIMPP_SYS_ALIGN_WIDTH, HIMPP_PIXEL_FORMAT)
{
	for (auto arg : args) {
		if (arg.first == "vsrc") {
			VideoSource* vsrc = VIDEO_SOURCE(buildElementPipe(arg.second));
			if (vsrc) {
				runtime->addVideoSource(vsrc);
			}
		}
		else if (arg.first == "venc") {
			VideoEncoder* venc = VIDEO_ENCODER(buildElementPipe(arg.second));
			if (venc) {
				runtime->addVideoEncoder(venc);
			}
		}
		else if (arg.first == "asrc") {
			AudioSource* asrc = AUDIO_SOURCE(buildElementPipe(arg.second));
			if (asrc) {
				runtime->addAudioSource(asrc);
			}
		}
		else if (arg.first == "aenc") {
			AudioEncoder* aenc = AUDIO_ENCODER(buildElementPipe(arg.second));
			if (aenc) {
				runtime->addAudioEncoder(aenc);
			}
		}
		else if (arg.first == "stream") {
			VideoStreamSource* video = NULL;
			AudioStreamSource* audio = NULL;
			std::size_t sep = arg.second.find(':');
			if (sep != std::string::npos) {
				std::string sname = arg.second.substr(0, sep);
				std::vector<std::string> tracks = split(arg.second.substr(sep + 1), ',');

				if (tracks.size() > 0) {
					MediaElementMap::iterator vit = _elements.find(tracks[0]);
					if (vit != _elements.end()) {
						video = dynamic_cast<VideoStreamSource*>(vit->second.get());
					}
				}

				if (tracks.size() > 1) {
					MediaElementMap::iterator ait = _elements.find(tracks[1]);
					if (ait != _elements.end()) {
						audio = dynamic_cast<AudioStreamSource*>(ait->second.get());
					}
				}

				if (video || audio)
					_runtime->addRTSPStream(sname, video, audio);
			}
		}
	}

	for (auto &e : _elements) {
		HimppViDev* videv = dynamic_cast<HimppViDev*>(e.second.get());
		if (videv) {
			Resolution dim = HIMPP_VIDEO_ELEMENT(videv)->resolution();
			_sysctl.addVideoBuffer(dim.width() * dim.height() * 3 / 2, 8);
		}
	}
	_sysctl.addVideoBuffer(196 * 4, 2);

	_sysctl.enable();
}

HimppMedia::~HimppMedia()
{
	_sysctl.disable();
	_elements.clear();
}

#define add_element(name, constructor) ({					\
	MediaElementMap::iterator it = _elements.find(name);	\
	MediaElement* e;										\
	if (it == _elements.end()) {							\
		e = new constructor;								\
		_elements.emplace(name, e);							\
	} else {												\
		e = it->second.get();								\
	}														\
	e;														\
})

MediaElement* HimppMedia::buildElementPipe(const std::string& description)
{
	MediaElement* last_element = NULL;
	std::vector<std::string> elem_descs = split(description, '|');

	for (auto elem_desc : elem_descs) {
		std::size_t colon_pos = elem_desc.find(':');
		std::string name = elem_desc.substr(0, colon_pos);
		std::unordered_map<std::string,std::string> params;
		if (colon_pos != std::string::npos) {
			std::string pdesc = elem_desc.substr(colon_pos + 1);
			std::vector<std::string> vp = split(pdesc, ',');
			for (auto it = vp.begin(); it != vp.end(); it++) {
				std::size_t dpos = it->find('=');
				if (dpos == std::string::npos) continue;
				params.emplace(it->substr(0, dpos), it->substr(dpos + 1));
			}
		}

		if (name.compare(0, 3, "isp") == 0) {
			auto sensor_it = params.find("sensor");
			if (sensor_it == params.end()) {
				std::cout << name << ": " << "sensor must be specified" << std::endl;
				break;
			}
			last_element = add_element(name, HimppVideoISP(HIMPP_VIDEO_ELEMENT(last_element), sensor_it->second));
		}
		else if (name.compare(0, 5, "videv") == 0) {
			if (!last_element && (_elements.find(name) == _elements.end())) break;
			uint32_t index = std::stoul(name.substr(5));
			last_element = add_element(name, HimppViDev(HIMPP_VIDEO_ELEMENT(last_element), index));
		}
		else if (name.compare(0, 5, "vichn") == 0) {
			if (!last_element && (_elements.find(name) == _elements.end())) break;
			uint32_t index = std::stoul(name.substr(5));
			last_element = add_element(name, HimppViChan(HIMPP_VIDEO_ELEMENT(last_element), index));
		}
		else if (name.compare(0, 5, "vpgrp") == 0) {
			if (!last_element && (_elements.find(name) == _elements.end())) break;
			uint32_t index = std::stoul(name.substr(5));
			last_element = add_element(name, HimppVpssGroup(HIMPP_VIDEO_ELEMENT(last_element), index));
		}
		else if (name.compare(0, 5, "vpchn") == 0) {
			if (!last_element && (_elements.find(name) == _elements.end())) break;
			uint32_t index = std::stoul(name.substr(5));
			last_element = add_element(name, HimppVpssChan(HIMPP_VIDEO_ELEMENT(last_element), index));
		}
		else if (name.compare(0, 5, "vechn") == 0) {
			if (!last_element && (_elements.find(name) == _elements.end())) break;
			uint32_t index = std::stoul(name.substr(5));
			VideoEncodingType encoding = H264;
			std::unordered_map<std::string, std::string>::iterator pit;

			pit = params.find("encoding");
			if (pit != params.end()) {
				if (pit->second == "H264") {
					encoding = H264;
				} else if (pit->second == "JPEG") {
					encoding = JPEG;
				} else if (pit->second == "MJPEG") {
					encoding = MJPEG;
				} else {
					std::cerr << name << ": " << "invalid encoding \"" << pit->second << "\"." << std::endl;
					break;
				}
			} else {
				std::cout << name << ": " << "encoding not specified, using default(H264)" << std::endl;
			}

			last_element = add_element(name, HimppVencChan(HIMPP_VIDEO_ELEMENT(last_element), encoding, index));

			pit = params.find("resolution");
			if (pit != params.end()) {
				Resolution res(pit->second);
				HIMPP_VENC_CHAN(last_element)->setResolution(res);
			}

			pit = params.find("framerate");
			if (pit != params.end()) {
				uint32_t framerate = std::stoi(pit->second);
				HIMPP_VENC_CHAN(last_element)->setFramerate(framerate);
			}
		}
		else if (name.compare(0, 6, "acodec") == 0) {
			last_element = add_element(name, HimppAudioCodec());
		}
		else if (name.compare(0, 5, "aidev") == 0) {
			if (!last_element && (_elements.find(name) == _elements.end())) break;
			uint32_t index = std::stoul(name.substr(5));
			last_element = add_element(name, HimppAiDev(HIMPP_AUDIO_ELEMENT(last_element), index));
		}
		else if (name.compare(0, 5, "aichn") == 0) {
			if (!last_element && (_elements.find(name) == _elements.end())) break;
			uint32_t index = std::stoul(name.substr(5));
			last_element = add_element(name, HimppAiChan(HIMPP_AUDIO_ELEMENT(last_element), index));
		}
		else if (name.compare(0, 5, "aechn") == 0) {
			if (!last_element && (_elements.find(name) == _elements.end())) break;
			uint32_t index = std::stoul(name.substr(5));
			AudioEncodingType encoding = G711A;
			std::unordered_map<std::string, std::string>::iterator pit;

			pit = params.find("encoding");
			if (pit != params.end()) {
				if (pit->second == "ADPCM") {
					encoding = ADPCM;
				} else if (pit->second == "LPCM") {
					encoding = LPCM;
				} else if (pit->second == "G711A") {
					encoding = G711A;
				} else if (pit->second == "G711U") {
					encoding = G711U;
				} else if (pit->second == "G726") {
					encoding = G726;
				} else {
					std::cerr << name << ": " << "invalid encoding \"" << pit->second << "\"." << std::endl;
					break;
				}
			} else {
				std::cout << name << ": " << "encoding not specified, using default(G711A)" << std::endl;
			}
			last_element = add_element(name, HimppAencChan(HIMPP_AUDIO_ELEMENT(last_element), encoding, index));
		}
	}

	return last_element;
}
