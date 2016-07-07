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

#include <alloca.h>

#include <SimpleRTPSink.hh>
#include <H264VideoRTPSink.hh>
#include <H264VideoStreamDiscreteFramer.hh>

#include "LiveStreamInput.hh"


class LiveVideoStreamSource: public FramedSource, public StreamDataConsumer
{
public:
    static LiveVideoStreamSource* createNew(UsageEnvironment& env, IVideoStream& stream);

    int &referenceCount() { return fReferenceCount; }

    void streamData(StreamData &data);
protected:
    LiveVideoStreamSource(UsageEnvironment& env, IVideoStream& stream);
    // called only by createNew(), or by subclass constructors
    virtual ~LiveVideoStreamSource();

private:
    // redefined virtual functions:
    virtual void doGetNextFrame();
    virtual void doStopGettingFrames();

private:
    IVideoStream &fVideoStream;
    int fReferenceCount;
};

//////////////////////////////////////////////////////////////////////////////
// LiveVideoStreamSource
//////////////////////////////////////////////////////////////////////////////

LiveVideoStreamSource*
LiveVideoStreamSource::createNew(UsageEnvironment& env, IVideoStream& stream)
{
    return new LiveVideoStreamSource(env, stream);
}

LiveVideoStreamSource::LiveVideoStreamSource(UsageEnvironment& env, IVideoStream& stream)
: FramedSource(env), fVideoStream(stream), fReferenceCount(1)
{
    fVideoStream.registerConsumer(this);
}

LiveVideoStreamSource::~LiveVideoStreamSource()
{
    fVideoStream.unregisterConsumer(this);
}

void LiveVideoStreamSource::streamData(StreamData &data)
{
    // check if we're ready for the data
    if (!isCurrentlyAwaitingData()) {
        envir() << "WARN: LiveVideoStreamSource is not ready for data yet\n";
        return;
    }

    fPresentationTime = data.tstamp;
    fFrameSize = 0;
    for (int i = 0; i < (int)data.pack_count; i++) {
        uint8_t *ptr = data.pack[i].addr;
        uint32_t len = data.pack[i].len;

        if ((ptr == NULL) || (len == 0))
            break;

        if (len >= 4 && ptr[0] == 0x00 && ptr[1] == 0x00
            && ptr[2] == 0x00 && ptr[3] == 0x01)
        {
            ptr += 4;
            len -= 4;
        }

        if (fFrameSize + len < fMaxSize) {
            memmove(&fTo[fFrameSize], ptr, len);
            fFrameSize += len;
        }
        else {
            fNumTruncatedBytes += len;
        }
    }

    fVideoStream.disableStreaming();

    // After delivering the data, inform the reader that it is now available:
    FramedSource::afterGetting(this);
}

void LiveVideoStreamSource::doGetNextFrame()
{
    fVideoStream.enableStreaming();
}

void LiveVideoStreamSource::doStopGettingFrames()
{
    fVideoStream.disableStreaming();
}


//////////////////////////////////////////////////////////////////////////////
// LiveVideoServerMediaSubsession
//////////////////////////////////////////////////////////////////////////////

LiveVideoServerMediaSubsession*
LiveVideoServerMediaSubsession::createNew(UsageEnvironment& env, IVideoStream& stream)
{
  return new LiveVideoServerMediaSubsession(env, stream);
}

LiveVideoServerMediaSubsession
::LiveVideoServerMediaSubsession(UsageEnvironment& env, IVideoStream& stream)
    : OnDemandServerMediaSubsession(env, True /* always reuse the first source */),
      fVideoStream(stream), fStreamSource(NULL),
      fAuxSDPLine(NULL), fDoneFlag(0), fDummyRTPSink(NULL)
{
}

LiveVideoServerMediaSubsession::~LiveVideoServerMediaSubsession()
{
    delete[] fAuxSDPLine;
}

static void afterPlayingDummy(void* clientData) {
    LiveVideoServerMediaSubsession* subsess = (LiveVideoServerMediaSubsession*)clientData;
    subsess->afterPlayingDummy1();
}

void LiveVideoServerMediaSubsession::afterPlayingDummy1()
{
    // Unschedule any pending 'checking' task:
    envir().taskScheduler().unscheduleDelayedTask(nextTask());
    // Signal the event loop that we're done:
    setDoneFlag();
}

static void checkForAuxSDPLine(void* clientData)
{
    LiveVideoServerMediaSubsession* subsess = (LiveVideoServerMediaSubsession*)clientData;
    subsess->checkForAuxSDPLine1();
}

void LiveVideoServerMediaSubsession::checkForAuxSDPLine1()
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

char const* LiveVideoServerMediaSubsession
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

FramedSource* LiveVideoServerMediaSubsession
::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate)
{
    estBitrate = fVideoStream.getBitrate(); // kbps, estimate

    // StreamSource has been already created
    if (fStreamSource) {
        LiveVideoStreamSource *liveSource =
            dynamic_cast<LiveVideoStreamSource*>(fStreamSource->inputSource());
        liveSource->referenceCount()++;
        return fStreamSource;
    }

    // Create the video source:
    LiveVideoStreamSource *source = LiveVideoStreamSource::createNew(envir(), fVideoStream);
    fStreamSource = H264VideoStreamDiscreteFramer::createNew(envir(), source);
    return fStreamSource;
}

void LiveVideoServerMediaSubsession::closeStreamSource(FramedSource *inputSource)
{
    // Sanity check, should not happend
    if (fStreamSource != inputSource) {
        Medium::close(inputSource);
        return;
    }

    LiveVideoStreamSource *liveSource =
        dynamic_cast<LiveVideoStreamSource*>(fStreamSource->inputSource());
    if (--liveSource->referenceCount() == 0) {
        Medium::close(fStreamSource);
        fStreamSource = NULL;
    }
}

RTPSink* LiveVideoServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char rtpPayloadTypeIfDynamic,
		   FramedSource* /*inputSource*/)
{
    H264VideoRTPSink *rtp_sink = H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
    rtp_sink->setPacketSizes(1000, 1456 * 10);
    return rtp_sink;
}


////////////////////////////////////////////////////////////////////////////////
// Audio
////////////////////////////////////////////////////////////////////////////////


class LiveAudioStreamSource: public FramedSource, public StreamDataConsumer
{
public:
    static LiveAudioStreamSource* createNew(UsageEnvironment& env, IAudioStream& stream);

    void streamData(StreamData &data);
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

void LiveAudioStreamSource::streamData(StreamData &data)
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
