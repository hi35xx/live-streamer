/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * LiveStreamInput.cpp
 * Copyright (C) 2015 Watson Xu <xuhuashan@gmail.com>
 *
 * rtsp-streamer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * rtsp-streamer is distributed in the hope that it will be useful, but
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

#include <SimpleRTPSink.hh>
#include <H264VideoRTPSink.hh>
#include <H264VideoStreamDiscreteFramer.hh>
#include <H265VideoRTPSink.hh>
#include <H265VideoStreamDiscreteFramer.hh>
#include <JPEGVideoSource.hh>
#include <JPEGVideoRTPSink.hh>
#include <MPEG4GenericRTPSink.hh>

#ifdef HAVE_VO_AACENC_SUPPORT
#include <AACAudioEncoder.hh>
#endif

#include "LiveStreamInput.hh"

using namespace Ipcam::Media;


class LiveH264StreamSource: public FramedSource, public StreamSink
{
public:
	static LiveH264StreamSource* createNew(UsageEnvironment& env, H264VideoStreamSource* stream);

	void streamData(StreamBuffer* buffer);
protected:
	LiveH264StreamSource(UsageEnvironment& env, H264VideoStreamSource* source);
	// called only by createNew(), or by subclass constructors
	virtual ~LiveH264StreamSource();
	// redefined virtual functions:
	virtual void doGetNextFrame();
	virtual void doStopGettingFrames();

private:
	H264VideoStreamSource* fStreamSource;
};

//////////////////////////////////////////////////////////////////////////////
// LiveH264StreamSource
//////////////////////////////////////////////////////////////////////////////

LiveH264StreamSource*
LiveH264StreamSource::createNew(UsageEnvironment& env, H264VideoStreamSource* source)
{
	return new LiveH264StreamSource(env, source);
}

LiveH264StreamSource::LiveH264StreamSource
(UsageEnvironment& env, H264VideoStreamSource* source)
: FramedSource(env), fStreamSource(source)
{
	fStreamSource->attach(this);
	try {
		fStreamSource->play();
	}
	catch (IpcamError& e) {
		fStreamSource->detach(this);
		handleClosure();
	}
}

LiveH264StreamSource::~LiveH264StreamSource()
{
	fStreamSource->detach(this);
	fStreamSource->stop();
}

void LiveH264StreamSource::streamData(StreamBuffer* buffer)
{
	// check if we're ready for the data
	if (!isCurrentlyAwaitingData()) {
		envir() << "WARN: LiveH264StreamSource is not ready for data yet\n";
		return;
	}

	H264StreamBuffer* h264buffer = static_cast<H264StreamBuffer*>(buffer);

	fPresentationTime = h264buffer->tstamp;
	fFrameSize = 0;
	for (unsigned i = 0; i < h264buffer->pack_count; i++) {
		uint8_t *ptr = h264buffer->pack[i].addr;
		uint32_t len = h264buffer->pack[i].len;

		if ((ptr == NULL) || (len == 0)) break;

		if (len >= 4 && ptr[0] == 0x00 && ptr[1] == 0x00
		    && ptr[2] == 0x00 && ptr[3] == 0x01)
		{
			ptr += 4;
			len -= 4;
		}

		if (fFrameSize + len < fMaxSize) {
			memmove(&fTo[fFrameSize], ptr, len);
			fFrameSize += len;
		} else {
			fNumTruncatedBytes += len;
		}
	}

	fStreamSource->pause();

	// After delivering the data, inform the reader that it is now available:
	FramedSource::afterGetting(this);
}

void LiveH264StreamSource::doGetNextFrame()
{
	fStreamSource->resume();
}

void LiveH264StreamSource::doStopGettingFrames()
{
	fStreamSource->pause();
}


//////////////////////////////////////////////////////////////////////////////
// LiveH264VideoServerMediaSubsession
//////////////////////////////////////////////////////////////////////////////

LiveH264VideoServerMediaSubsession*
LiveH264VideoServerMediaSubsession::createNew(UsageEnvironment& env, H264VideoStreamSource* streamsource)
{
	return new LiveH264VideoServerMediaSubsession(env, streamsource);
}

LiveH264VideoServerMediaSubsession
::LiveH264VideoServerMediaSubsession(UsageEnvironment& env, H264VideoStreamSource* streamsource)
  : OnDemandServerMediaSubsession(env, True /* always reuse the first source */),
    fVideoStreamSource(streamsource),
    fAuxSDPLine(NULL), fDoneFlag(0), fDummyRTPSink(NULL)
{
}

LiveH264VideoServerMediaSubsession::~LiveH264VideoServerMediaSubsession()
{
	delete[] fAuxSDPLine;
}

static void H264AfterPlayingDummy(void* clientData) {
	LiveH264VideoServerMediaSubsession* subsess = (LiveH264VideoServerMediaSubsession*)clientData;
	subsess->afterPlayingDummy1();
}

void LiveH264VideoServerMediaSubsession::afterPlayingDummy1()
{
	// Unschedule any pending 'checking' task:
	envir().taskScheduler().unscheduleDelayedTask(nextTask());
	// Signal the event loop that we're done:
	setDoneFlag();
}

static void H264CheckForAuxSDPLine(void* clientData)
{
	LiveH264VideoServerMediaSubsession* subsess = (LiveH264VideoServerMediaSubsession*)clientData;
	subsess->checkForAuxSDPLine1();
}

void LiveH264VideoServerMediaSubsession::checkForAuxSDPLine1()
{
	char const* dasl;

	if (fAuxSDPLine != NULL) {
		// Signal the event loop that we're done:
		setDoneFlag();
	} else if (fDummyRTPSink != NULL && (dasl = fDummyRTPSink->auxSDPLine()) != NULL) {
		char framerate[32], dimension[32];
		Resolution res = fVideoStreamSource->resolution();
		sprintf(framerate, "a=framerate:%d\r\n", fVideoStreamSource->framerate());
		sprintf(dimension, "a=x-dimensions:%d,%d\r\n", res.width(), res.height());
		fAuxSDPLine = new char[strlen(dasl) + strlen(framerate) + strlen(dimension) + 1];
		if (fAuxSDPLine != NULL) {
			sprintf(fAuxSDPLine, "%s%s%s", dasl, framerate, dimension);
		}
		fDummyRTPSink = NULL;

		// Signal the event loop that we're done:
		setDoneFlag();
	} else if (!fDoneFlag) {
		// try again after a brief delay:
		int uSecsToDelay = 100000; // 100 ms
		nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsToDelay,
		                                                         (TaskFunc*)H264CheckForAuxSDPLine, this);
	}
}

char const* LiveH264VideoServerMediaSubsession
::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource)
{
	if (fAuxSDPLine != NULL) return fAuxSDPLine; // it's already been set up (for a previous client)

	if (fDummyRTPSink == NULL) { // we're not already setting it up for another, concurrent stream
		// Note: For H264 video files, the 'config' information ("profile-level-id" and "sprop-parameter-sets") isn't known
		// until we start reading the file.  This means that "rtpSink"s "auxSDPLine()" will be NULL initially,
		// and we need to start reading data from our file until this changes.
		fDummyRTPSink = rtpSink;

		// Start reading the file:
		fDummyRTPSink->startPlaying(*inputSource, H264AfterPlayingDummy, this);

		// Check whether the sink's 'auxSDPLine()' is ready:
		H264CheckForAuxSDPLine(this);
	}

	envir().taskScheduler().doEventLoop(&fDoneFlag);

	return fAuxSDPLine;
}

FramedSource* LiveH264VideoServerMediaSubsession
::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate)
{
	estBitrate = fVideoStreamSource->bitrate(); // kbps, estimate

	// Create the video source:
	LiveH264StreamSource *source = LiveH264StreamSource::createNew(envir(), fVideoStreamSource);
	return H264VideoStreamDiscreteFramer::createNew(envir(), source);
}

RTPSink* LiveH264VideoServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char rtpPayloadTypeIfDynamic,
		   FramedSource* /*inputSource*/)
{
	H264VideoRTPSink *rtpsink = H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
	rtpsink->setPacketSizes(1000, 1456 * 8);
	return rtpsink;
}

void LiveH264VideoServerMediaSubsession
::startStream(unsigned clientSessionId, void* streamToken,
              TaskFunc* rtcpRRHandler,
              void* rtcpRRHandlerClientData,
              unsigned short& rtpSeqNum,
              unsigned& rtpTimestamp,
              ServerRequestAlternativeByteHandler* serverRequestAlternativeByteHandler,
              void* serverRequestAlternativeByteHandlerClientData)
{
	OnDemandServerMediaSubsession::startStream(clientSessionId, streamToken, rtcpRRHandler,
	                                           rtcpRRHandlerClientData,
	                                           rtpSeqNum,
	                                           rtpTimestamp,
	                                           serverRequestAlternativeByteHandler,
	                                           serverRequestAlternativeByteHandlerClientData);
	fVideoStreamSource->requestIDR();
}

void LiveH264VideoServerMediaSubsession
::pauseStream(unsigned clientSessionId, void* streamToken)
{
	OnDemandServerMediaSubsession::pauseStream(clientSessionId, streamToken);
}


class LiveH265StreamSource: public FramedSource, public StreamSink
{
public:
	static LiveH265StreamSource* createNew(UsageEnvironment& env, H264VideoStreamSource* stream);

	void streamData(StreamBuffer* buffer);
protected:
	LiveH265StreamSource(UsageEnvironment& env, H264VideoStreamSource* source);
	// called only by createNew(), or by subclass constructors
	virtual ~LiveH265StreamSource();
	// redefined virtual functions:
	virtual void doGetNextFrame();
	virtual void doStopGettingFrames();

private:
	H264VideoStreamSource* fStreamSource;
};

//////////////////////////////////////////////////////////////////////////////
// LiveH265StreamSource
//////////////////////////////////////////////////////////////////////////////

LiveH265StreamSource*
LiveH265StreamSource::createNew(UsageEnvironment& env, H264VideoStreamSource* source)
{
	return new LiveH265StreamSource(env, source);
}

LiveH265StreamSource::LiveH265StreamSource
(UsageEnvironment& env, H264VideoStreamSource* source)
: FramedSource(env), fStreamSource(source)
{
	fStreamSource->attach(this);
	try {
		fStreamSource->play();
	}
	catch (IpcamError& e) {
		fStreamSource->detach(this);
		handleClosure();
	}
}

LiveH265StreamSource::~LiveH265StreamSource()
{
	fStreamSource->detach(this);
	fStreamSource->stop();
}

void LiveH265StreamSource::streamData(StreamBuffer* buffer)
{
	// check if we're ready for the data
	if (!isCurrentlyAwaitingData()) {
		envir() << "WARN: LiveH265StreamSource is not ready for data yet\n";
		return;
	}

	H264StreamBuffer* h265buffer = static_cast<H264StreamBuffer*>(buffer);

	fPresentationTime = h265buffer->tstamp;
	fFrameSize = 0;
	for (unsigned i = 0; i < h265buffer->pack_count; i++) {
		uint8_t *ptr = h265buffer->pack[i].addr;
		uint32_t len = h265buffer->pack[i].len;

		if ((ptr == NULL) || (len == 0)) break;

		if (len >= 4 && ptr[0] == 0x00 && ptr[1] == 0x00
		    && ptr[2] == 0x00 && ptr[3] == 0x01)
		{
			ptr += 4;
			len -= 4;
		}

		if (fFrameSize + len < fMaxSize) {
			memmove(&fTo[fFrameSize], ptr, len);
			fFrameSize += len;
		} else {
			fNumTruncatedBytes += len;
		}
	}

	fStreamSource->pause();

	// After delivering the data, inform the reader that it is now available:
	FramedSource::afterGetting(this);
}

void LiveH265StreamSource::doGetNextFrame()
{
	fStreamSource->resume();
}

void LiveH265StreamSource::doStopGettingFrames()
{
	fStreamSource->pause();
}


//////////////////////////////////////////////////////////////////////////////
// LiveH265VideoServerMediaSubsession
//////////////////////////////////////////////////////////////////////////////

LiveH265VideoServerMediaSubsession*
LiveH265VideoServerMediaSubsession::createNew(UsageEnvironment& env, H264VideoStreamSource* streamsource)
{
	return new LiveH265VideoServerMediaSubsession(env, streamsource);
}

LiveH265VideoServerMediaSubsession
::LiveH265VideoServerMediaSubsession(UsageEnvironment& env, H264VideoStreamSource* streamsource)
  : OnDemandServerMediaSubsession(env, True /* always reuse the first source */),
    fVideoStreamSource(streamsource),
    fAuxSDPLine(NULL), fDoneFlag(0), fDummyRTPSink(NULL)
{
}

LiveH265VideoServerMediaSubsession::~LiveH265VideoServerMediaSubsession()
{
	delete[] fAuxSDPLine;
}

static void H265AfterPlayingDummy(void* clientData) {
	LiveH265VideoServerMediaSubsession* subsess = (LiveH265VideoServerMediaSubsession*)clientData;
	subsess->afterPlayingDummy1();
}

void LiveH265VideoServerMediaSubsession::afterPlayingDummy1()
{
	// Unschedule any pending 'checking' task:
	envir().taskScheduler().unscheduleDelayedTask(nextTask());
	// Signal the event loop that we're done:
	setDoneFlag();
}

static void H265CheckForAuxSDPLine(void* clientData)
{
	LiveH265VideoServerMediaSubsession* subsess = (LiveH265VideoServerMediaSubsession*)clientData;
	subsess->checkForAuxSDPLine1();
}

void LiveH265VideoServerMediaSubsession::checkForAuxSDPLine1()
{
	char const* dasl;

	if (fAuxSDPLine != NULL) {
		// Signal the event loop that we're done:
		setDoneFlag();
	} else if (fDummyRTPSink != NULL && (dasl = fDummyRTPSink->auxSDPLine()) != NULL) {
		char framerate[32], dimension[32];
		Resolution res = fVideoStreamSource->resolution();
		sprintf(framerate, "a=framerate:%d\r\n", fVideoStreamSource->framerate());
		sprintf(dimension, "a=x-dimensions:%d,%d\r\n", res.width(), res.height());
		fAuxSDPLine = new char[strlen(dasl) + strlen(framerate) + strlen(dimension) + 1];
		if (fAuxSDPLine != NULL) {
			sprintf(fAuxSDPLine, "%s%s%s", dasl, framerate, dimension);
		}
		fDummyRTPSink = NULL;

		// Signal the event loop that we're done:
		setDoneFlag();
	} else if (!fDoneFlag) {
		// try again after a brief delay:
		int uSecsToDelay = 100000; // 100 ms
		nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsToDelay,
		                                                         (TaskFunc*)H265CheckForAuxSDPLine, this);
	}
}

char const* LiveH265VideoServerMediaSubsession
::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource)
{
	if (fAuxSDPLine != NULL) return fAuxSDPLine; // it's already been set up (for a previous client)

	if (fDummyRTPSink == NULL) { // we're not already setting it up for another, concurrent stream
		// Note: For H265 video files, the 'config' information ("profile-level-id" and "sprop-parameter-sets") isn't known
		// until we start reading the file.  This means that "rtpSink"s "auxSDPLine()" will be NULL initially,
		// and we need to start reading data from our file until this changes.
		fDummyRTPSink = rtpSink;

		// Start reading the file:
		fDummyRTPSink->startPlaying(*inputSource, H265AfterPlayingDummy, this);

		// Check whether the sink's 'auxSDPLine()' is ready:
		H265CheckForAuxSDPLine(this);
	}

	envir().taskScheduler().doEventLoop(&fDoneFlag);

	return fAuxSDPLine;
}

FramedSource* LiveH265VideoServerMediaSubsession
::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate)
{
	estBitrate = fVideoStreamSource->bitrate(); // kbps, estimate

	// Create the video source:
	LiveH265StreamSource *source = LiveH265StreamSource::createNew(envir(), fVideoStreamSource);
	return H265VideoStreamDiscreteFramer::createNew(envir(), source);
}

RTPSink* LiveH265VideoServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char rtpPayloadTypeIfDynamic,
		   FramedSource* /*inputSource*/)
{
	H265VideoRTPSink *rtpsink = H265VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
	rtpsink->setPacketSizes(1000, 1456 * 8);
	return rtpsink;
}

void LiveH265VideoServerMediaSubsession
::startStream(unsigned clientSessionId, void* streamToken,
              TaskFunc* rtcpRRHandler,
              void* rtcpRRHandlerClientData,
              unsigned short& rtpSeqNum,
              unsigned& rtpTimestamp,
              ServerRequestAlternativeByteHandler* serverRequestAlternativeByteHandler,
              void* serverRequestAlternativeByteHandlerClientData)
{
	OnDemandServerMediaSubsession::startStream(clientSessionId, streamToken, rtcpRRHandler,
	                                           rtcpRRHandlerClientData,
	                                           rtpSeqNum,
	                                           rtpTimestamp,
	                                           serverRequestAlternativeByteHandler,
	                                           serverRequestAlternativeByteHandlerClientData);
	fVideoStreamSource->requestIDR();
}

void LiveH265VideoServerMediaSubsession
::pauseStream(unsigned clientSessionId, void* streamToken)
{
	OnDemandServerMediaSubsession::pauseStream(clientSessionId, streamToken);
}


//////////////////////////////////////////////////////////////////////////////
// LiveJPEGStreamSource
//////////////////////////////////////////////////////////////////////////////

class LiveJPEGStreamSource: public JPEGVideoSource, public StreamSink
{
public:
	static LiveJPEGStreamSource* createNew(UsageEnvironment& env, JPEGVideoStreamSource* source);

	// implementation of StreamSink
	void streamData(StreamBuffer* buffer);
	void play();
	void stop();

protected:
	LiveJPEGStreamSource(UsageEnvironment& env, JPEGVideoStreamSource* source);
	virtual ~LiveJPEGStreamSource();

	// implementation of JPEGVideoSource
	virtual void doGetNextFrame();
	virtual void doStopGettingFrames();

	virtual u_int8_t type();
	virtual u_int8_t qFactor();
	virtual u_int8_t width();
	virtual u_int8_t height();

private:
	JPEGVideoStreamSource* fStreamSource;
	bool fStreamStarted;
	u_int8_t fLastQFactor;
	u_int8_t fLastWidth, fLastHeight; // actual dimensions / 8
};

LiveJPEGStreamSource*
LiveJPEGStreamSource::createNew(UsageEnvironment& env, JPEGVideoStreamSource* source)
{
	return new LiveJPEGStreamSource(env, source);
}

LiveJPEGStreamSource
::LiveJPEGStreamSource(UsageEnvironment& env, JPEGVideoStreamSource* source)
: JPEGVideoSource(env), fStreamSource(source), fStreamStarted(false),
  fLastQFactor(0), fLastWidth(0), fLastHeight(0)
{
	fStreamSource->attach(this);
}

LiveJPEGStreamSource::~LiveJPEGStreamSource()
{
	stop();
	fStreamSource->detach(this);
}

void LiveJPEGStreamSource::doGetNextFrame()
{
	fStreamSource->resume();
}

void LiveJPEGStreamSource::doStopGettingFrames()
{
	fStreamSource->pause();
}

void LiveJPEGStreamSource::streamData(StreamBuffer* buffer)
{
	// check if we're ready for the data
	if (!isCurrentlyAwaitingData()) {
		envir() << "WARN: LiveJPEGStreamSource is not ready for data yet\n";
		return;
	}

	JPEGStreamBuffer* jpegbuffer = static_cast<JPEGStreamBuffer*>(buffer);

	fPresentationTime = jpegbuffer->tstamp;
	fFrameSize = 0;

	fLastQFactor = jpegbuffer->qfactor;
	fLastWidth = jpegbuffer->width;
	fLastHeight = jpegbuffer->height;

	// Skip the JPEG header at jpegbuffer->pack[0]
	for (uint32_t i = 1; i < jpegbuffer->pack_count; i++) {
		uint8_t *ptr = jpegbuffer->pack[i].addr;
		uint32_t len = jpegbuffer->pack[i].len;

		if ((ptr == NULL) || (len == 0)) break;

		if (fFrameSize + len < fMaxSize) {
			memmove(&fTo[fFrameSize], ptr, len);
			fFrameSize += len;
		} else {
			fNumTruncatedBytes += len;
		}
	}

	fStreamSource->pause();

	// After delivering the data, inform the reader that it is now available:
	FramedSource::afterGetting(this);
}

void LiveJPEGStreamSource::play()
{
	if (!fStreamStarted) {
		fStreamSource->play();
		fStreamSource->resume();
		fStreamStarted = true;
	}
}

void LiveJPEGStreamSource::stop()
{
	if (fStreamStarted) {
		fStreamSource->stop();
		fStreamStarted = false;
	}
}

u_int8_t LiveJPEGStreamSource::type()
{
	return 1;
}

u_int8_t LiveJPEGStreamSource::qFactor()
{
	return fLastQFactor; // 255 indicates that quantization tables are dynamic
}

u_int8_t LiveJPEGStreamSource::width()
{
	return fLastWidth;
}

u_int8_t LiveJPEGStreamSource::height()
{
	return fLastHeight;
}


//////////////////////////////////////////////////////////////////////////////
// LiveJPEGVideoServerMediaSubsession
//////////////////////////////////////////////////////////////////////////////

LiveJPEGVideoServerMediaSubsession* LiveJPEGVideoServerMediaSubsession
::createNew(UsageEnvironment& env, JPEGVideoStreamSource* streamsource)
{
	return new LiveJPEGVideoServerMediaSubsession(env, streamsource);
}

LiveJPEGVideoServerMediaSubsession
::LiveJPEGVideoServerMediaSubsession(UsageEnvironment& env, JPEGVideoStreamSource* streamsource)
  : OnDemandServerMediaSubsession(env, True /* reuse the first source */),
    fVideoStreamSource(streamsource)
{
}

LiveJPEGVideoServerMediaSubsession::~LiveJPEGVideoServerMediaSubsession()
{
}

FramedSource* LiveJPEGVideoServerMediaSubsession
::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate)
{
	estBitrate = fVideoStreamSource->bitrate(); // kbps, estimate

	return LiveJPEGStreamSource::createNew(envir(), fVideoStreamSource);
}

void LiveJPEGVideoServerMediaSubsession
::closeStreamSource(FramedSource* inputSource)
{
	((LiveJPEGStreamSource*)inputSource)->stop();
	OnDemandServerMediaSubsession::closeStreamSource(inputSource);
}

RTPSink* LiveJPEGVideoServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
                   unsigned char /*rtpPayloadTypeIfDynamic*/,
                   FramedSource* /*inputSource*/)
{
	JPEGVideoRTPSink* rtpsink = JPEGVideoRTPSink::createNew(envir(), rtpGroupsock);
	rtpsink->setPacketSizes(1000, 1456 * 8);
	return rtpsink;
}

void LiveJPEGVideoServerMediaSubsession
::startStream(unsigned clientSessionId, void* streamToken,
              TaskFunc* rtcpRRHandler,
              void* rtcpRRHandlerClientData,
              unsigned short& rtpSeqNum,
              unsigned& rtpTimestamp,
              ServerRequestAlternativeByteHandler* serverRequestAlternativeByteHandler,
              void* serverRequestAlternativeByteHandlerClientData)
{
	OnDemandServerMediaSubsession::startStream(clientSessionId, streamToken, rtcpRRHandler,
	                                           rtcpRRHandlerClientData,
	                                           rtpSeqNum,
	                                           rtpTimestamp,
	                                           serverRequestAlternativeByteHandler,
	                                           serverRequestAlternativeByteHandlerClientData);

	StreamState* streamState = (StreamState*)streamToken;
	LiveJPEGStreamSource* source = (LiveJPEGStreamSource*)streamState->mediaSource();

	try {
		source->play();
	} catch (IpcamError& e) {
		fprintf(stderr, "Failed to start JPEG stream: %s\n", e.what());
	}
}

void LiveJPEGVideoServerMediaSubsession
::pauseStream(unsigned clientSessionId, void* streamToken)
{
	OnDemandServerMediaSubsession::pauseStream(clientSessionId, streamToken);

	StreamState* streamState = (StreamState*)streamToken;
	LiveJPEGStreamSource* source = (LiveJPEGStreamSource*)streamState->mediaSource();

	try {
		source->stop();
	} catch (IpcamError& e) {
		fprintf(stderr, "Failed to stop JPEG stream: %s\n", e.what());
	}
}


////////////////////////////////////////////////////////////////////////////////
// Audio
////////////////////////////////////////////////////////////////////////////////

class LiveAudioStreamSource: public FramedSource, public StreamSink
{
public:
	static LiveAudioStreamSource* createNew(UsageEnvironment& env, AudioStreamSource* source);

	void streamData(StreamBuffer* buffer);
protected:
	LiveAudioStreamSource(UsageEnvironment& env, AudioStreamSource* source);
	// called only by createNew(), or by subclass constructors
	virtual ~LiveAudioStreamSource();
	// redefined virtual functions:
	virtual void doGetNextFrame();
	virtual void doStopGettingFrames();

private:
	AudioStreamSource* fStreamSource;
};

LiveAudioStreamSource* LiveAudioStreamSource::createNew
(UsageEnvironment& env, AudioStreamSource* source)
{
	return new LiveAudioStreamSource(env, source);
}

LiveAudioStreamSource::LiveAudioStreamSource
(UsageEnvironment& env, AudioStreamSource* source)
  : FramedSource(env), fStreamSource(source)
{
	fStreamSource->attach(this);
	try {
		fStreamSource->play();
	}
	catch (IpcamError& e) {
		fStreamSource->detach(this);
		handleClosure();
	}
}

LiveAudioStreamSource::~LiveAudioStreamSource()
{
	fStreamSource->detach(this);
	fStreamSource->stop();
}

void LiveAudioStreamSource::streamData(StreamBuffer* buffer)
{
	// check if we're ready for data
	if (!isCurrentlyAwaitingData()) {
		envir() << "WARN: LiveAudioStreamSource is not ready for data yet\n";
		return;
	}

	fPresentationTime = buffer->tstamp;
	fFrameSize = 0;
	for (int i = 0; i < (int)buffer->pack_count; i++) {
		uint8_t *ptr = buffer->pack[i].addr;
		uint32_t len = buffer->pack[i].len;

		if ((ptr == NULL) || (len == 0))
			break;

		if (fFrameSize + len < fMaxSize) {
			memmove(&fTo[fFrameSize], ptr, len);
			fFrameSize += len;
		}
		else {
			fNumTruncatedBytes += len;
		}
	}

	fStreamSource->pause();

	// After delivering the data, inform the reader that it is now available:
	FramedSource::afterGetting(this);
}

void LiveAudioStreamSource::doGetNextFrame()
{
	fStreamSource->resume();
}

void LiveAudioStreamSource::doStopGettingFrames()
{
	fStreamSource->pause();
}


LiveAudioServerMediaSubsession* LiveAudioServerMediaSubsession
::createNew(UsageEnvironment& env, AudioStreamSource* streamsource)
{
	return new LiveAudioServerMediaSubsession(env, streamsource);
}

LiveAudioServerMediaSubsession
::LiveAudioServerMediaSubsession(UsageEnvironment& env, AudioStreamSource* streamsource)
  : OnDemandServerMediaSubsession(env, True /* always reuse the first source */),
    fAudioStreamSource(streamsource)
{
}

LiveAudioServerMediaSubsession::~LiveAudioServerMediaSubsession()
{
}

FramedSource* LiveAudioServerMediaSubsession
::createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate)
{
	estBitrate = fAudioStreamSource->bitrate(); // kbps, estimate

	unsigned packetMaxSize = OutPacketBuffer::maxSize;
	OutPacketBuffer::maxSize = 4 * 1024;
	// Create the audio source:
	FramedSource* source = LiveAudioStreamSource::createNew(envir(), fAudioStreamSource);
#ifdef HAVE_VO_AACENC_SUPPORT
	if (fAudioStreamSource->encoding() == AAC) {
		uint32_t sampleRate = fAudioStreamSource->samplerate();
		uint32_t channels = fAudioStreamSource->channels();
		source = AACAudioEncoder
			::createNew(envir(), source, channels, sampleRate, estBitrate);
	}
#endif
	OutPacketBuffer::maxSize = packetMaxSize;

	return source;
}

static unsigned const samplingFrequencyTable[16] = {
  96000, 88200, 64000, 48000,
  44100, 32000, 24000, 22050,
  16000, 12000, 11025, 8000,
  7350, 0, 0, 0
};

RTPSink* LiveAudioServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
                   unsigned char rtpPayloadTypeIfDynamic,
				   FramedSource* inputSource)
{
	uint32_t samplerate = fAudioStreamSource->samplerate();
	uint32_t nr_chans = 1;
	unsigned char payloadFormatCode = rtpPayloadTypeIfDynamic;

	RTPSink *rtp_sink = NULL;
	AudioEncodingType encoding;
	encoding = fAudioStreamSource->encoding();

	if (encoding == AAC) {
		char config_str[5];
		uint8_t profile = 1;
		uint8_t sample_rate_index = 11;
		for (int i = 0; i <= 15; i++) {
			if (samplerate == samplingFrequencyTable[i]) {
				sample_rate_index = i;
				break;
			}
		}
		unsigned char config[2];
		config[0] = ((profile + 1) << 3) | (sample_rate_index >> 1);
		config[1] = (sample_rate_index << 7) | (nr_chans << 3);
		sprintf(config_str, "%02X%02X", config[0], config[1]);
		rtp_sink = MPEG4GenericRTPSink::
			createNew(envir(), rtpGroupsock, payloadFormatCode, samplerate,
			          "audio", "AAC-hbr", config_str, nr_chans);
	} else {
		const char *mimeType = "";
		switch (encoding) {
		case ADPCM:
			mimeType = "DVI4";
			break;
		case LPCM:
			mimeType = "L16";
			break;
		case G711A:
			mimeType = "PCMA";
			break;
		case G711U:
			mimeType = "PCMU";
			break;
		case G726:
			mimeType = "G726-40";
			break;
		default:
			break;
		}
		rtp_sink = SimpleRTPSink
			::createNew(envir(), rtpGroupsock,
			            payloadFormatCode, samplerate,
			            "audio", mimeType, nr_chans);
	}

	return rtp_sink;
}
