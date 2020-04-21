/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * LiveStreamInput.hh
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
#ifndef __LIVE_STREAM_INPUT_HH
#define __LIVE_STREAM_INPUT_HH

#include <MediaSink.hh>
#include <OnDemandServerMediaSubsession.hh>
#include <media-stream.h>

using namespace Ipcam::Media;

class LiveH264VideoServerMediaSubsession: public OnDemandServerMediaSubsession
{
public:
	static LiveH264VideoServerMediaSubsession*
		createNew(UsageEnvironment& env, H264VideoStreamSource* stream);

	// Used to implement "getAuxSDPLine()":
	void checkForAuxSDPLine1();
	void afterPlayingDummy1();

	void startStream(unsigned clientSessionId, void* streamToken,
	                 TaskFunc* rtcpRRHandler,
	                 void* rtcpRRHandlerClientData,
	                 unsigned short& rtpSeqNum,
	                 unsigned& rtpTimestamp,
	                 ServerRequestAlternativeByteHandler* serverRequestAlternativeByteHandler,
	                 void* serverRequestAlternativeByteHandlerClientData);
	void pauseStream(unsigned clientSessionId, void* streamToken);

protected:
	LiveH264VideoServerMediaSubsession(UsageEnvironment& env, H264VideoStreamSource* stream);
	// called only by createNew();
	virtual ~LiveH264VideoServerMediaSubsession();

	void setDoneFlag() { fDoneFlag = ~0; }

protected: // redefined virtual functions
	virtual char const* getAuxSDPLine(RTPSink* rtpSink,
	                                  FramedSource* inputSource);

	virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
	                                            unsigned& estBitrate);
	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
	                                  unsigned char rtpPayloadTypeIfDynamic,
	                                  FramedSource* inputSource);

private:
	H264VideoStreamSource* fVideoStreamSource;
	char* fAuxSDPLine;
	char fDoneFlag; // used when setting up "fAuxSDPLine"
	RTPSink* fDummyRTPSink; // ditto
};


class LiveH265VideoServerMediaSubsession: public OnDemandServerMediaSubsession
{
public:
	static LiveH265VideoServerMediaSubsession*
		createNew(UsageEnvironment& env, H264VideoStreamSource* stream);

	// Used to implement "getAuxSDPLine()":
	void checkForAuxSDPLine1();
	void afterPlayingDummy1();

	void startStream(unsigned clientSessionId, void* streamToken,
	                 TaskFunc* rtcpRRHandler,
	                 void* rtcpRRHandlerClientData,
	                 unsigned short& rtpSeqNum,
	                 unsigned& rtpTimestamp,
	                 ServerRequestAlternativeByteHandler* serverRequestAlternativeByteHandler,
	                 void* serverRequestAlternativeByteHandlerClientData);
	void pauseStream(unsigned clientSessionId, void* streamToken);

protected:
	LiveH265VideoServerMediaSubsession(UsageEnvironment& env, H264VideoStreamSource* stream);
	// called only by createNew();
	virtual ~LiveH265VideoServerMediaSubsession();

	void setDoneFlag() { fDoneFlag = ~0; }

protected: // redefined virtual functions
	virtual char const* getAuxSDPLine(RTPSink* rtpSink,
	                                  FramedSource* inputSource);

	virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
	                                            unsigned& estBitrate);
	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
	                                  unsigned char rtpPayloadTypeIfDynamic,
	                                  FramedSource* inputSource);

private:
	H264VideoStreamSource* fVideoStreamSource;
	char* fAuxSDPLine;
	char fDoneFlag; // used when setting up "fAuxSDPLine"
	RTPSink* fDummyRTPSink; // ditto
};


class LiveJPEGVideoServerMediaSubsession: public OnDemandServerMediaSubsession
{
public:
	static LiveJPEGVideoServerMediaSubsession*
		createNew(UsageEnvironment& env, JPEGVideoStreamSource* stream);

	void startStream(unsigned clientSessionId, void* streamToken,
	                 TaskFunc* rtcpRRHandler,
	                 void* rtcpRRHandlerClientData,
	                 unsigned short& rtpSeqNum,
	                 unsigned& rtpTimestamp,
	                 ServerRequestAlternativeByteHandler* serverRequestAlternativeByteHandler,
	                 void* serverRequestAlternativeByteHandlerClientData);
	void pauseStream(unsigned clientSessionId, void* streamToken);

protected:
	LiveJPEGVideoServerMediaSubsession(UsageEnvironment& env, JPEGVideoStreamSource* stream);
	virtual ~LiveJPEGVideoServerMediaSubsession();

private:
	virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
	                                            unsigned& estBitrate);
	virtual void closeStreamSource(FramedSource* inputSource);
	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
	                                  unsigned char rtpPayloadTypeIfDynamic,
	                                  FramedSource* inputSource);
private:
	JPEGVideoStreamSource*	fVideoStreamSource;
};


class LiveAudioServerMediaSubsession: public OnDemandServerMediaSubsession
{
public:
	static LiveAudioServerMediaSubsession*
	createNew(UsageEnvironment& env, AudioStreamSource* stream);

protected:
	LiveAudioServerMediaSubsession(UsageEnvironment& env, AudioStreamSource* stream);
	virtual ~LiveAudioServerMediaSubsession();

protected: // redefined virtual functions
	virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
	                                            unsigned& estBitrate);
	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
	                                  unsigned char rtpPayloadTypeIfDynamic,
	                                  FramedSource* inputSource);

private:
	AudioStreamSource* fAudioStreamSource;
};

#endif //__LIVE_STREAM_INPUT_HH
