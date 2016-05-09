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
#include <ipcam-media-iface.h>

using namespace Ipcam::Media;

class LiveVideoServerMediaSubsession: public OnDemandServerMediaSubsession {
public:
  static LiveVideoServerMediaSubsession*
  createNew(UsageEnvironment& env, IVideoStream& stream);

  // Used to implement "getAuxSDPLine()":
  void checkForAuxSDPLine1();
  void afterPlayingDummy1();

protected:
  LiveVideoServerMediaSubsession(UsageEnvironment& env, IVideoStream& stream);
      // called only by createNew();
  virtual ~LiveVideoServerMediaSubsession();

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
  IVideoStream &fVideoStream;
  char* fAuxSDPLine;
  char fDoneFlag; // used when setting up "fAuxSDPLine"
  RTPSink* fDummyRTPSink; // ditto
};


class LiveAudioServerMediaSubsession: public OnDemandServerMediaSubsession {
public:
  static LiveAudioServerMediaSubsession*
  createNew(UsageEnvironment& env, IAudioStream& stream);

protected:
  LiveAudioServerMediaSubsession(UsageEnvironment& env, IAudioStream& stream);
  virtual ~LiveAudioServerMediaSubsession();

protected: // redefined virtual functions
  virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
					      unsigned& estBitrate);
  virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
                                    unsigned char rtpPayloadTypeIfDynamic,
				    FramedSource* inputSource);

private:
  IAudioStream &fAudioStream;
};

#endif //__LIVE_STREAM_INPUT_HH
