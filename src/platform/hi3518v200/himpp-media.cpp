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
    _sysctl(HIMPP_SYS_ALIGN_WIDTH)
{
	for (auto arg : args) {
		if (arg.first == "pipe") {
			MediaElement* elem = buildElementPipe(arg.second);
			if (!elem) {
				fprintf(stderr, "pipe build failed: [%s]\n", arg.second.c_str());
			}
		}
		if (arg.first == "vsrc") {
			MediaElementMap::iterator it = _elements.find(arg.second);
			if (it != _elements.end()) {
				VideoSource* vsrc = VIDEO_SOURCE(it->second.get());
				if (vsrc) {
					runtime->addVideoSource(vsrc);
				}
			} else {
				fprintf(stderr, "VideoSource %s not found\n", arg.second.c_str());
			}
		}
		else if (arg.first == "venc") {
			MediaElementMap::iterator it = _elements.find(arg.second);
			if (it != _elements.end()) {
				VideoEncoder* venc = VIDEO_ENCODER(it->second.get());
				if (venc) {
					runtime->addVideoEncoder(venc);
				}
			} else {
				fprintf(stderr, "VideoEncoder %s not found\n", arg.second.c_str());
			}
		}
		else if (arg.first == "asrc") {
			MediaElementMap::iterator it = _elements.find(arg.second);
			if (it != _elements.end()) {
				AudioSource* asrc = AUDIO_SOURCE(it->second.get());
				if (asrc) {
					runtime->addAudioSource(asrc);
				}
			} else {
				fprintf(stderr, "AudioSource %s not found\n", arg.second.c_str());
			}
		}
		else if (arg.first == "aenc") {
			MediaElementMap::iterator it = _elements.find(arg.second);
			if (it != _elements.end()) {
				AudioEncoder* aenc = AUDIO_ENCODER(it->second.get());
				if (aenc) {
					runtime->addAudioEncoder(aenc);
				}
			} else {
				fprintf(stderr, "AudioEncoder %s not found\n", arg.second.c_str());
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

	_sysctl.addVideoBuffer(196 * 4, 2);
	_sysctl.enable();

	// enable elements with MEFLAGS_INITIAL_ENABLED flag
	for (auto &eit : _elements) {
		MediaElement* e = eit.second.get();
		if (e->flags() & MEFLAGS_INITIAL_ENABLED) {
			e->enable();
		}
	}
}

HimppMedia::~HimppMedia()
{
	// disable elements with MEFLAGS_INITIAL_ENABLED flag
	for (auto &eit : _elements) {
		MediaElement* e = eit.second.get();
		if (e->flags() & MEFLAGS_INITIAL_ENABLED) {
			e->disable();
		}
	}

	_sysctl.disable();
	_elements.clear();
}

#define add_element(var, name, constructor) ({				\
	MediaElementMap::iterator it = _elements.find(name);	\
	bool needs_create = (it == _elements.end());			\
	if (needs_create) {										\
		var = new constructor;								\
		_elements.emplace(name, var);						\
	} else {												\
		var = it->second.get();								\
	}														\
	needs_create;											\
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
				if (dpos == std::string::npos) {
					params.emplace(*it, std::string());
				} else {
					params.emplace(it->substr(0, dpos), it->substr(dpos + 1));
				}
			}
		}

		if (name.compare(0, 3, "isp") == 0) {
			auto sensor_it = params.find("sensor");
			if (sensor_it == params.end()) {
				std::cout << name << ": " << "sensor must be specified" << std::endl;
				break;
			}
			add_element(last_element, name, HimppVideoISP(HIMPP_VIDEO_ELEMENT(last_element), sensor_it->second));
		}
		else if (name.compare(0, 5, "videv") == 0) {
			if (!last_element && (_elements.find(name) == _elements.end())) break;
			uint32_t index = std::stoul(name.substr(5));
			if (add_element(last_element, name, HimppViDev(HIMPP_VIDEO_ELEMENT(last_element), index))) {
				std::unordered_map<std::string, std::string>::iterator pit;
				if ((pit = params.find("resolution")) != params.end()) {
					Resolution res(pit->second);
					HIMPP_VI_DEV(last_element)->setResolution(res);
				}
				if ((pit = params.find("framerate")) != params.end()) {
					uint32_t framerate = std::stoi(pit->second);
					HIMPP_VI_DEV(last_element)->setFrameRate(framerate);
				}

				uint32_t vbcnt = 3;		// default value if 'vbcnt' option not present
				if ((pit = params.find("vbcnt")) != params.end()) {
					int32_t val = std::stoi(pit->second);
					if (val > 0) { vbcnt = val; }
				}
				Resolution dim = HIMPP_VIDEO_ELEMENT(last_element)->resolution();
				uint32_t blksiz = (dim.width() + 16) * dim.height() * 3 / 2;
				if (blksiz && vbcnt) _sysctl.addVideoBuffer(blksiz, vbcnt);
			}
		}
		else if (name.compare(0, 5, "vichn") == 0) {
			if (!last_element && (_elements.find(name) == _elements.end())) break;
			if (name.size() < 8) break;
			uint32_t index = std::stoul(name.substr(7));
			if (add_element(last_element, name, HimppViChan(HIMPP_VIDEO_ELEMENT(last_element), index))) {
				std::unordered_map<std::string, std::string>::iterator pit;
				if ((pit = params.find("resolution")) != params.end()) {
					Resolution res(pit->second);
					HIMPP_VI_CHAN(last_element)->setResolution(res);
				}
				if ((pit = params.find("framerate")) != params.end() ||
				    (pit = params.find("fr")) != params.end()) {
					uint32_t framerate = std::stoi(pit->second);
					HIMPP_VI_CHAN(last_element)->setFrameRate(framerate);
				}

				uint32_t vbcnt = 0;		// default value if 'vbcnt' option not present
				if ((pit = params.find("vbcnt")) != params.end()) {
					int32_t val = std::stoi(pit->second);
					if (val > 0) { vbcnt = val; }
				}
				Resolution dim = HIMPP_VIDEO_ELEMENT(last_element)->resolution();
				uint32_t blksiz = (dim.width() + 16) * dim.height() * 3 / 2;
				if (blksiz && vbcnt) _sysctl.addVideoBuffer(blksiz, vbcnt);
			}
		}
		else if (name.compare(0, 5, "vpgrp") == 0) {
			if (!last_element && (_elements.find(name) == _elements.end())) break;
			uint32_t index = std::stoul(name.substr(5));
			add_element(last_element, name, HimppVpssGroup(HIMPP_VIDEO_ELEMENT(last_element), index));
		}
		else if (name.compare(0, 5, "vpchn") == 0) {
			if (!last_element && (_elements.find(name) == _elements.end())) break;
			if (name.size() < 8) break;
			uint32_t index = std::stoul(name.substr(7));
			if (add_element(last_element, name, HimppVpssChan(HIMPP_VIDEO_ELEMENT(last_element), index))) {
				std::unordered_map<std::string, std::string>::iterator pit;
				if ((pit = params.find("resolution")) != params.end()) {
					Resolution res(pit->second);
					HIMPP_VPSS_CHAN(last_element)->setResolution(res);
				}
				if ((pit = params.find("framerate")) != params.end() ||
				    (pit = params.find("fr")) != params.end()) {
					uint32_t framerate = std::stoi(pit->second);
					HIMPP_VPSS_CHAN(last_element)->setFrameRate(framerate);
				}

				uint32_t vbcnt = 0;		// default value if 'vbcnt' option not present
				if ((pit = params.find("vbcnt")) != params.end()) {
					int32_t val = std::stoi(pit->second);
					if (val > 0) { vbcnt = val; }
				}
				Resolution dim = HIMPP_VIDEO_ELEMENT(last_element)->resolution();
				uint32_t blksiz = (dim.width() + 16) * dim.height() * 3 / 2;
				if (blksiz && vbcnt) _sysctl.addVideoBuffer(blksiz, vbcnt);
			}
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

			if (add_element(last_element, name, HimppVencChan(HIMPP_VIDEO_ELEMENT(last_element), encoding, index))) {
				if ((pit = params.find("resolution")) != params.end()) {
					Resolution res(pit->second);
					HIMPP_VENC_CHAN(last_element)->setResolution(res);
				}

				if ((pit = params.find("framerate")) != params.end() ||
					(pit = params.find("fr")) != params.end()) {
					uint32_t framerate = std::stoi(pit->second);
					HIMPP_VENC_CHAN(last_element)->setFramerate(framerate);
				}

				if ((pit = params.find("bitrate")) != params.end() ||
				    (pit = params.find("br")) != params.end()) {
					uint32_t bitrate = std::stoul(pit->second);
					HIMPP_VENC_CHAN(last_element)->setBitrate(bitrate);
				}
			}
		}
		else if (name.compare(0, 6, "acodec") == 0) {
			add_element(last_element, name, HimppAudioCodec());
		}
		else if (name.compare(0, 5, "aidev") == 0) {
			if (!last_element && (_elements.find(name) == _elements.end())) break;
			uint32_t index = std::stoul(name.substr(5));
			add_element(last_element, name, HimppAiDev(HIMPP_AUDIO_ELEMENT(last_element), index));
		}
		else if (name.compare(0, 5, "aichn") == 0) {
			if (!last_element && (_elements.find(name) == _elements.end())) break;
			if (name.size() < 8) break;
			uint32_t index = std::stoul(name.substr(7));
			add_element(last_element, name, HimppAiChan(HIMPP_AUDIO_ELEMENT(last_element), index));
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
			add_element(last_element, name, HimppAencChan(HIMPP_AUDIO_ELEMENT(last_element), encoding, index));
		}

		std::unordered_map<std::string, std::string>::iterator pit;
		pit = params.find("enable");
		if (pit != params.end()) {
			last_element->flags() |= MEFLAGS_INITIAL_ENABLED;
		}
	}

	return last_element;
}
