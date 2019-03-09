/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * LiveStreamOutput.h
 * Copyright (C) 2019 Watson Xu <watson@localhost.localdomain>
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

#ifndef _LIVESTREAMOUTPUT_H_
#define _LIVESTREAMOUTPUT_H_

#include <GroupsockHelper.hh>
#include <UsageEnvironment.hh>
#include <media-stream.h>

using namespace Ipcam::Media;

class LiveAudioSink;

class AudioOutputStream
{
public:
	AudioOutputStream(UsageEnvironment& env, AudioStreamSink& sink,
	                  struct in_addr &addr, portNumBits num);
	~AudioOutputStream();

	void play();
	void stop();

private:
	AudioStreamSink& streamSink;
	bool streamStarted;
	Groupsock rtpGroupsock;
	Groupsock rtcpGroupsock;
	RTPSource* rtpSource;
	RTCPInstance* rtcpInstance;
	LiveAudioSink* rtpSink;
};

#endif // _LIVESTREAMOUTPUT_H_

