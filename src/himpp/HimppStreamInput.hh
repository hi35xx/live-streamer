/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * HimppStreamInput.hh
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
#ifndef __H264_LIVE_STREAM_INPUT_HH
#define __H264_LIVE_STREAM_INPUT_HH

#include <MediaSink.hh>
#include <OnDemandServerMediaSubsession.hh>
#include <himpp-media.h>


class HimppVideoServerMediaSubsession: public OnDemandServerMediaSubsession {
public:
  static HimppVideoServerMediaSubsession*
  createNew(UsageEnvironment& env, HimppVideoEncoder& encoder);

  // Used to implement "getAuxSDPLine()":
  void checkForAuxSDPLine1();
  void afterPlayingDummy1();

protected:
  HimppVideoServerMediaSubsession(UsageEnvironment& env, HimppVideoEncoder& encoder);
      // called only by createNew();
  virtual ~HimppVideoServerMediaSubsession();

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
  HimppVideoEncoder &fVideoEncoder;
  char* fAuxSDPLine;
  char fDoneFlag; // used when setting up "fAuxSDPLine"
  RTPSink* fDummyRTPSink; // ditto
};


class HimppAudioServerMediaSubsession: public OnDemandServerMediaSubsession {
public:
  static HimppAudioServerMediaSubsession*
  createNew(UsageEnvironment& env, HimppAudioEncoder& encoder);

protected:
  HimppAudioServerMediaSubsession(UsageEnvironment& env, HimppAudioEncoder& encoder);
  virtual ~HimppAudioServerMediaSubsession();

protected: // redefined virtual functions
  virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
					      unsigned& estBitrate);
  virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
                                    unsigned char rtpPayloadTypeIfDynamic,
				    FramedSource* inputSource);

private:
  HimppAudioEncoder &fAudioEncoder;
};

#endif
