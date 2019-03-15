/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * LiveStreamOutput.cpp
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

#include <GroupsockHelper.hh>
#include <RTCP.hh>
#include <MediaSink.hh>
#include <SimpleRTPSource.hh>

#include "LiveStreamOutput.hh"


#define AUDIO_BUFFER_SIZE		2048

class LiveAudioSink : public MediaSink {
public:
	static LiveAudioSink* createNew(UsageEnvironment& env, StreamSink& sink);
	virtual ~LiveAudioSink();

protected:
	LiveAudioSink(UsageEnvironment& env, StreamSink &sink);

	Boolean continuePlaying();
	static void afterGettingFrame(void* clientData, unsigned frameSize,
	                              unsigned numTruncatedBytes,
	                              struct timeval presentationTime,
	                              unsigned durationInMicroseconds);
	void afterGettingFrame(unsigned frameSize,
	                       unsigned numTruncatedBytes,
	                       struct timeval presentationTime);

protected:
	StreamSink &fStreamSink;
	u_int8_t* fReceiveBuffer;
};

LiveAudioSink* LiveAudioSink::createNew(UsageEnvironment& env, StreamSink& sink)
{
	return new LiveAudioSink(env, sink);
}

LiveAudioSink::LiveAudioSink(UsageEnvironment& env, StreamSink& sink)
: MediaSink(env), fStreamSink(sink)
{
	fReceiveBuffer = new u_int8_t[AUDIO_BUFFER_SIZE];
}

LiveAudioSink::~LiveAudioSink()
{
	delete [] fReceiveBuffer;
}

Boolean LiveAudioSink::continuePlaying()
{
	if (fSource == NULL) return False;

	fSource->getNextFrame(fReceiveBuffer, AUDIO_BUFFER_SIZE,
	                      afterGettingFrame, this,
	                      onSourceClosure, this);

	return True;
}

void LiveAudioSink::afterGettingFrame(void* clientData, unsigned frameSize,
                                      unsigned numTruncatedBytes,
                                      struct timeval presentationTime,
                                      unsigned /*durationInMicroseconds*/)
{
	LiveAudioSink* sink = (LiveAudioSink*)clientData;
	sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime);
}

void LiveAudioSink::afterGettingFrame(unsigned frameSize,
                                      unsigned numTruncatedBytes,
                                      struct timeval presentationTime)
{
	StreamBuffer::Pack pack = {
		.addr = fReceiveBuffer,
		.len = frameSize
	};
	StreamBuffer buffer = {
		.tstamp = {0},
		.pack_count = 1,
		.pack = &pack,
	};

	fStreamSink.streamData(&buffer);

	continuePlaying();
}

void afterPlaying(void* /*clientData*/)
{
}

AudioOutputStream::AudioOutputStream
(UsageEnvironment& env, AudioStreamSink& sink, struct in_addr &addr, portNumBits num)
: streamSink(sink), streamStarted(false),
  rtpGroupsock(env, addr, Port(num), 0)
{
	rtpSink = LiveAudioSink::createNew(env, streamSink);

	rtpSource = SimpleRTPSource::createNew(env, &rtpGroupsock, 96,
	                                       streamSink.samplerate(),
	                                       "audio/PCMA", 0, False);

	rtpSink->startPlaying(*rtpSource, afterPlaying, this);
}

AudioOutputStream::~AudioOutputStream()
{
	stop();

	if (rtpSink) Medium::close(rtpSink);
	if (rtpSource) Medium::close(rtpSource);
}

void AudioOutputStream::play()
{
	if (!streamStarted) {
		streamSink.play();
		streamStarted = true;
	}
}

void AudioOutputStream::stop()
{
	if (streamStarted) {
		streamSink.stop();
		streamStarted = false;
	}
}
