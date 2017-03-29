/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
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

#include <SimpleRTPSink.hh>
#include <H264VideoRTPSink.hh>
#include <H264VideoStreamDiscreteFramer.hh>
#include <JPEGVideoSource.hh>
#include <JPEGVideoRTPSink.hh>

#include "LiveStreamInput.hh"


class LiveH264StreamSource: public FramedSource, public StreamDataConsumer
{
public:
    static LiveH264StreamSource* createNew(UsageEnvironment& env, IVideoStream& stream);

    int &referenceCount() { return fReferenceCount; }

    void streamData(FrameData& data, void* info);
protected:
    LiveH264StreamSource(UsageEnvironment& env, IVideoStream& stream);
    // called only by createNew(), or by subclass constructors
    virtual ~LiveH264StreamSource();

private:
    // redefined virtual functions:
    virtual void doGetNextFrame();
    virtual void doStopGettingFrames();

private:
    IVideoStream &fVideoStream;
    int fReferenceCount;
};

//////////////////////////////////////////////////////////////////////////////
// LiveH264StreamSource
//////////////////////////////////////////////////////////////////////////////

LiveH264StreamSource*
LiveH264StreamSource::createNew(UsageEnvironment& env, IVideoStream& stream)
{
    return new LiveH264StreamSource(env, stream);
}

LiveH264StreamSource::LiveH264StreamSource(UsageEnvironment& env, IVideoStream& stream)
: FramedSource(env), fVideoStream(stream), fReferenceCount(1)
{
    fVideoStream.registerConsumer(this);
}

LiveH264StreamSource::~LiveH264StreamSource()
{
    fVideoStream.unregisterConsumer(this);
}

void LiveH264StreamSource::streamData(FrameData& data, void* info)
{
    // check if we're ready for the data
    if (!isCurrentlyAwaitingData()) {
        envir() << "WARN: LiveH264StreamSource is not ready for data yet\n";
        return;
    }

    fPresentationTime = data.tstamp;
    fFrameSize = 0;
    for (unsigned i = 0; i < data.pack_count; i++) {
        uint8_t *ptr = data.pack[i].addr;
        uint32_t len = data.pack[i].len;

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

    fVideoStream.disableStreaming();

    // After delivering the data, inform the reader that it is now available:
    FramedSource::afterGetting(this);
}

void LiveH264StreamSource::doGetNextFrame()
{
    fVideoStream.enableStreaming();
}

void LiveH264StreamSource::doStopGettingFrames()
{
    fVideoStream.disableStreaming();
}


//////////////////////////////////////////////////////////////////////////////
// LiveH264VideoServerMediaSubsession
//////////////////////////////////////////////////////////////////////////////

LiveH264VideoServerMediaSubsession*
LiveH264VideoServerMediaSubsession::createNew(UsageEnvironment& env, IH264VideoStream& stream)
{
  return new LiveH264VideoServerMediaSubsession(env, stream);
}

LiveH264VideoServerMediaSubsession
::LiveH264VideoServerMediaSubsession(UsageEnvironment& env, IH264VideoStream& stream)
    : OnDemandServerMediaSubsession(env, True /* always reuse the first source */),
      fVideoStream(stream), fStreamSource(NULL),
      fAuxSDPLine(NULL), fDoneFlag(0), fDummyRTPSink(NULL)
{
}

LiveH264VideoServerMediaSubsession::~LiveH264VideoServerMediaSubsession()
{
    delete[] fAuxSDPLine;
}

static void afterPlayingDummy(void* clientData) {
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

static void checkForAuxSDPLine(void* clientData)
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
        fAuxSDPLine = strDup(dasl);
        fDummyRTPSink = NULL;

        // Signal the event loop that we're done:
        setDoneFlag();
    } else if (!fDoneFlag) {
        // try again after a brief delay:
        int uSecsToDelay = 100000; // 100 ms
        nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsToDelay,
                (TaskFunc*)checkForAuxSDPLine, this);
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
        fDummyRTPSink->startPlaying(*inputSource, afterPlayingDummy, this);

        // Check whether the sink's 'auxSDPLine()' is ready:
        checkForAuxSDPLine(this);
    }

    envir().taskScheduler().doEventLoop(&fDoneFlag);

    return fAuxSDPLine;
}

FramedSource* LiveH264VideoServerMediaSubsession
::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate)
{
    estBitrate = fVideoStream.getBitrate(); // kbps, estimate

    // StreamSource has been already created
    if (fStreamSource) {
        LiveH264StreamSource *liveSource =
            dynamic_cast<LiveH264StreamSource*>(fStreamSource->inputSource());
        liveSource->referenceCount()++;
        return fStreamSource;
    }

    // Create the video source:
    LiveH264StreamSource *source = LiveH264StreamSource::createNew(envir(), fVideoStream);
    fStreamSource = H264VideoStreamDiscreteFramer::createNew(envir(), source);
    return fStreamSource;
}

void LiveH264VideoServerMediaSubsession::closeStreamSource(FramedSource *inputSource)
{
    // Sanity check, should not happend
    if (fStreamSource != inputSource) {
        Medium::close(inputSource);
        return;
    }

    LiveH264StreamSource *liveSource =
        dynamic_cast<LiveH264StreamSource*>(fStreamSource->inputSource());
    if (--liveSource->referenceCount() == 0) {
        Medium::close(fStreamSource);
        fStreamSource = NULL;
    }
}

RTPSink* LiveH264VideoServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char rtpPayloadTypeIfDynamic,
		   FramedSource* /*inputSource*/)
{
    H264VideoRTPSink *rtp_sink = H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
    rtp_sink->setPacketSizes(1000, 1456 * 10);
    return rtp_sink;
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
    fVideoStream.requestIDR();
}

void LiveH264VideoServerMediaSubsession
::pauseStream(unsigned clientSessionId, void* streamToken)
{
    OnDemandServerMediaSubsession::pauseStream(clientSessionId, streamToken);
}


//////////////////////////////////////////////////////////////////////////////
// LiveJPEGStreamSource
//////////////////////////////////////////////////////////////////////////////

class LiveJPEGStreamSource: public JPEGVideoSource, public StreamDataConsumer
{
public:
  static LiveJPEGStreamSource* createNew(UsageEnvironment& env, IVideoStream& stream);

protected:
  LiveJPEGStreamSource(UsageEnvironment& env, IVideoStream& stream);
  virtual ~LiveJPEGStreamSource();

  void streamData(FrameData& data, void* info);

protected: // redefined virtual functions
  virtual void doGetNextFrame();
  virtual void doStopGettingFrames();
  virtual u_int8_t type();
  virtual u_int8_t qFactor();
  virtual u_int8_t width();
  virtual u_int8_t height();

private:
  IVideoStream& fVideoStream;
  u_int8_t fLastQFactor;
  u_int8_t fLastWidth, fLastHeight; // actual dimensions / 8
};

LiveJPEGStreamSource*
LiveJPEGStreamSource::createNew(UsageEnvironment& env, IVideoStream& stream)
{
    return new LiveJPEGStreamSource(env, stream);
}

LiveJPEGStreamSource
::LiveJPEGStreamSource(UsageEnvironment& env, IVideoStream& stream)
  : JPEGVideoSource(env),
    fVideoStream(stream), fLastQFactor(0),
    fLastWidth(0), fLastHeight(0)
{
    fVideoStream.registerConsumer(this);
}

LiveJPEGStreamSource::~LiveJPEGStreamSource()
{
    fVideoStream.unregisterConsumer(this);
}

void LiveJPEGStreamSource::doGetNextFrame()
{
    fVideoStream.enableStreaming();
}

void LiveJPEGStreamSource::doStopGettingFrames()
{
    fVideoStream.disableStreaming();
}

void LiveJPEGStreamSource::streamData(FrameData& data, void* info)
{
    // check if we're ready for the data
    if (!isCurrentlyAwaitingData()) {
        envir() << "WARN: LiveJPEGStreamSource is not ready for data yet\n";
        return;
    }

    fPresentationTime = data.tstamp;
    fFrameSize = 0;

    JPEGFrameInfo *jpeginfo = static_cast<JPEGFrameInfo*>(info);
    fLastQFactor = jpeginfo->qfactor;
    fLastWidth = jpeginfo->width;
    fLastHeight = jpeginfo->height;

    for (uint32_t i = 0; i < data.pack_count; i++) {
        uint8_t *ptr = data.pack[i].addr;
        uint32_t len = data.pack[i].len;

        if ((ptr == NULL) || (len == 0)) break;

        if (fFrameSize + len < fMaxSize) {
            memmove(&fTo[fFrameSize], ptr, len);
            fFrameSize += len;
        } else {
            fNumTruncatedBytes += len;
        }
    }

    fVideoStream.disableStreaming();

    // After delivering the data, inform the reader that it is now available:
    FramedSource::afterGetting(this);
}

u_int8_t LiveJPEGStreamSource::type()
{
    return 1;
}

u_int8_t LiveJPEGStreamSource::qFactor()
{
    return fLastQFactor;//255; // indicates that quantization tables are dynamic
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
::createNew(UsageEnvironment& env, IJPEGVideoStream& stream)
{
    return new LiveJPEGVideoServerMediaSubsession(env, stream);
}

LiveJPEGVideoServerMediaSubsession
::LiveJPEGVideoServerMediaSubsession(UsageEnvironment& env, IJPEGVideoStream& stream)
  : OnDemandServerMediaSubsession(env, True /* reuse the first source */),
    fVideoStream(stream)
{
}

LiveJPEGVideoServerMediaSubsession::~LiveJPEGVideoServerMediaSubsession()
{
}

FramedSource* LiveJPEGVideoServerMediaSubsession
::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate)
{
    estBitrate = fVideoStream.getBitrate(); // kbps, estimate

    OutPacketBuffer::maxSize = 400000;

    return LiveJPEGStreamSource::createNew(envir(), fVideoStream);
}

RTPSink* LiveJPEGVideoServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
                   unsigned char /*rtpPayloadTypeIfDynamic*/,
                   FramedSource* /*inputSource*/)
{
    return JPEGVideoRTPSink::createNew(envir(), rtpGroupsock);
}


////////////////////////////////////////////////////////////////////////////////
// Audio
////////////////////////////////////////////////////////////////////////////////

class LiveAudioStreamSource: public FramedSource, public StreamDataConsumer
{
public:
    static LiveAudioStreamSource* createNew(UsageEnvironment& env, IAudioStream& stream);

    void streamData(FrameData& data, void* info);
protected:
    LiveAudioStreamSource(UsageEnvironment& env, IAudioStream& stream);
    // called only by createNew(), or by subclass constructors
    virtual ~LiveAudioStreamSource();

private:
    // redefined virtual functions:
    virtual void doGetNextFrame();
    virtual void doStopGettingFrames();

private:
    IAudioStream &fAudioStream;
};

LiveAudioStreamSource*
LiveAudioStreamSource::createNew(UsageEnvironment& env, IAudioStream& stream)
{
    return new LiveAudioStreamSource(env, stream);
}

LiveAudioStreamSource
::LiveAudioStreamSource(UsageEnvironment& env, IAudioStream& stream)
    : FramedSource(env), fAudioStream(stream)
{
    fAudioStream.registerConsumer(this);
}

LiveAudioStreamSource::~LiveAudioStreamSource()
{
    fAudioStream.unregisterConsumer(this);
}

void LiveAudioStreamSource::streamData(FrameData& data, void* info)
{
    // check if we're ready for data
    if (!isCurrentlyAwaitingData()) {
        envir() << "WARN: LiveAudioStreamSource is not ready for data yet\n";
        return;
    }

    fPresentationTime = data.tstamp;
    fFrameSize = 0;
    for (int i = 0; i < (int)data.pack_count; i++) {
        uint8_t *ptr = data.pack[i].addr;
        uint32_t len = data.pack[i].len;

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

    fAudioStream.disableStreaming();

    // After delivering the data, inform the reader that it is now available:
    FramedSource::afterGetting(this);
}

void LiveAudioStreamSource::doGetNextFrame()
{
    fAudioStream.enableStreaming();
}

void LiveAudioStreamSource::doStopGettingFrames()
{
    fAudioStream.disableStreaming();
}


LiveAudioServerMediaSubsession* LiveAudioServerMediaSubsession
::createNew(UsageEnvironment& env, IAudioStream& stream)
{
    return new LiveAudioServerMediaSubsession(env, stream);
}

LiveAudioServerMediaSubsession
::LiveAudioServerMediaSubsession(UsageEnvironment& env, IAudioStream& stream)
    : OnDemandServerMediaSubsession(env, True /* always reuse the first source */),
      fAudioStream(stream)
{
}

LiveAudioServerMediaSubsession::~LiveAudioServerMediaSubsession()
{
}

FramedSource* LiveAudioServerMediaSubsession
::createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate)
{
    estBitrate = 64; // kbps, estimate

    // Create the audio source:
    LiveAudioStreamSource *source = LiveAudioStreamSource::createNew(envir(), fAudioStream);

    return source;
}

RTPSink* LiveAudioServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
                   unsigned char rtpPayloadTypeIfDynamic,
				   FramedSource* inputSource)
{
    uint32_t samplerate = fAudioStream.getSampleRate();
    uint32_t nr_chans = 1;
    const char *mimeType = "";
    unsigned char payloadFormatCode = rtpPayloadTypeIfDynamic;

    IAudioEncoder::EncodingType encoding;
    encoding = fAudioStream.getEncoding();
    switch (encoding) {
    case IAudioEncoder::ADPCM:
        mimeType = "DVI4";
        break;
    case IAudioEncoder::LPCM:
        mimeType = "L16";
        break;
    case IAudioEncoder::G711A:
        mimeType = "PCMA";
        break;
    case IAudioEncoder::G711U:
        mimeType = "PCMU";
        break;
    case IAudioEncoder::G726:
        mimeType = "G726-40";
        break;
    }

    RTPSink *rtp_sink
        = SimpleRTPSink::createNew(envir(), rtpGroupsock,
                                   payloadFormatCode, samplerate,
                                   "audio", mimeType, nr_chans);
    return rtp_sink;
}
