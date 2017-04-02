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

HimppMedia::HimppMedia(IpcamRuntime *runtime, std::unordered_map<std::string, std::string>& args)
  : _runtime(runtime),
    _sysctl(HIMPP_SYS_ALIGN_WIDTH, HIMPP_PIXEL_FORMAT)
{
#define NEW_ELEMENT(e)	({ _elements.emplace_back(dynamic_cast<MediaElement*>(e)); e; })
	HimppVideoISP* isp0    = NEW_ELEMENT(new HimppVideoISP(NULL, "sc1135"));
	HimppViDev* videv0     = NEW_ELEMENT(new HimppViDev(isp0, 0));
	HimppViChan* vichn0    = NEW_ELEMENT(new HimppViChan(videv0, 0));
	HimppVpssGroup* vpgrp0 = NEW_ELEMENT(new HimppVpssGroup(vichn0, 0));
	HimppVpssChan* vpchn0  = NEW_ELEMENT(new HimppVpssChan(vpgrp0, 0));
	HimppVencChan* vechn0  = NEW_ELEMENT(new HimppVencChan(vpchn0, H264, 0));
	HimppVpssChan* vpchn1  = NEW_ELEMENT(new HimppVpssChan(vpgrp0, 1));
	HimppVencChan* vechn1  = NEW_ELEMENT(new HimppVencChan(vpchn1, MJPEG, 1));

	HimppAudioCodec* acodec0 = NEW_ELEMENT(new HimppAudioCodec());
	HimppAiDev* aidev0     = NEW_ELEMENT(new HimppAiDev(acodec0, 0));
	HimppAiChan* aichn0    = NEW_ELEMENT(new HimppAiChan(aidev0, 0));
	HimppAencChan* aechn0  = NEW_ELEMENT(new HimppAencChan(aichn0, 0));

	runtime->addVideoSource(VIDEO_SOURCE(vpchn0));
	runtime->addVideoEncoder(VIDEO_ENCODER(vechn0));
	runtime->addRTSPStream("0", VIDEO_STREAM_SOURCE(vechn0), AUDIO_STREAM_SOURCE(aechn0));

	Resolution dim1("720P");
	vechn1->setResolution(dim1);
	vechn1->setFramerate(10);
	runtime->addVideoEncoder(VIDEO_ENCODER(vechn1));
	runtime->addRTSPStream("1", VIDEO_STREAM_SOURCE(vechn1), NULL);

	Resolution dim = HIMPP_VIDEO_ELEMENT(videv0)->resolution();
	_sysctl.addVideoBuffer(dim.width() * dim.height() * 3 / 2, 8);
	_sysctl.addVideoBuffer(196 * 4, 2);

	_sysctl.enable();
}

HimppMedia::~HimppMedia()
{
	_sysctl.disable();
	_elements.clear();
}
