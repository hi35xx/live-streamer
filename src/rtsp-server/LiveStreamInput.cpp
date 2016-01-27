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


class LiveVideoStreamSource: public FramedSource, public LiveStreamDataPump
{
public:
    static LiveVideoStreamSource* createNew(UsageEnvironment& env, IVideoStream& stream);

public:
    EventTriggerId eventTriggerId;

    void pushData();
protected:
    LiveVideoStreamSource(UsageEnvironment& env, IVideoStream& stream);
    // called only by createNew(), or by subclass constructors
    virtual ~LiveVideoStreamSource();

private:
    // redefined virtual functions:
    virtual void doGetNextFrame();
    //virtual void doStopGettingFrames(); // optional
    static void backgroundHandler(void *clientData, int mask);
    static void deliverFrame0(void* clientData);
    void deliverFrame();

private:
    IVideoStream &fVideoStream;
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
: FramedSource(env), fVideoStream(stream), eventTriggerId(0)
{
    TaskScheduler &scheduler = envir().taskScheduler();

    eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);

    fVideoStream.startStreaming(this);
}

LiveVideoStreamSource::~LiveVideoStreamSource()
{
    fVideoStream.stopStreaming();
    // Reclaim our 'event trigger'
    envir().taskScheduler().deleteEventTrigger(eventTriggerId);
}

void LiveVideoStreamSource::pushData()
{
    //envir().taskScheduler().triggerEvent(eventTriggerId, this);
    deliverFrame();
}

void LiveVideoStreamSource::doGetNextFrame()
{
    // This function is called (by our 'downstream' object) when it asks for new data.

    // Note: If, for some reason, the source device stops being readable (e.g., it gets closed), then you do the following:
    if (0 /* the source stops being readable */ /*%%% TO BE WRITTEN %%%*/) {
        handleClosure(this);
        return;
    }

    // If a new frame of data is immediately available to be delivered, then do this now:
    if (0 /* a new frame of data is immediately available to be delivered*/ /*%%% TO BE WRITTEN %%%*/) {
        deliverFrame();
    }

    // No new data is immediately available to be delivered.  We don't do anything more here.
    // Instead, our event trigger must be called (e.g., from a separate thread) when new data becomes available.
}

void LiveVideoStreamSource::deliverFrame0(void* clientData)
{
    if (clientData)
        ((LiveVideoStreamSource*)clientData)->deliverFrame();
}

void LiveVideoStreamSource::deliverFrame()
{
    // This function is called when new frame data is available from the device.
    // We deliver this data by copying it to the 'downstream' object, using the following parameters (class members):
    // 'in' parameters (these should *not* be modified by this function):
    //     fTo: The frame data is copied to this address.
    //         (Note that the variable "fTo" is *not* modified.  Instead,
    //          the frame data is copied to the address pointed to by "fTo".)
    //     fMaxSize: This is the maximum number of bytes that can be copied
    //         (If the actual frame is larger than this, then it should
    //          be truncated, and "fNumTruncatedBytes" set accordingly.)
    // 'out' parameters (these are modified by this function):
    //     fFrameSize: Should be set to the delivered frame size (<= fMaxSize).
    //     fNumTruncatedBytes: Should be set iff the delivered frame would have been
    //         bigger than "fMaxSize", in which case it's set to the number of bytes
    //         that have been omitted.
    //     fPresentationTime: Should be set to the frame's presentation time
    //         (seconds, microseconds).  This time must be aligned with 'wall-clock time' - i.e., the time that you would get
    //         by calling "gettimeofday()".
    //     fDurationInMicroseconds: Should be set to the frame's duration, if known.
    //         If, however, the device is a 'live source' (e.g., encoded from a camera or microphone), then we probably don't need
    //         to set this variable, because - in this case - data will never arrive 'early'.
    // Note the code below.

    if (!isCurrentlyAwaitingData()) return; // we're not ready for the data yet

    fFrameSize = fVideoStream.getStreamData(fTo, fMaxSize,
                                            fNumTruncatedBytes,
                                            fPresentationTime);

    if (fFrameSize == 0) return;

    // After delivering the data, inform the reader that it is now available:
    FramedSource::afterGetting(this);
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
      fAuxSDPLine(NULL), fDoneFlag(0), fDummyRTPSink(NULL),
      fVideoStream(stream)
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

    // Create the video source:
    LiveVideoStreamSource *source = LiveVideoStreamSource::createNew(envir(), fVideoStream);
    return H264VideoStreamDiscreteFramer::createNew(envir(), source);
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


class LiveAudioStreamSource: public FramedSource, public LiveStreamDataPump
{
public:
    static LiveAudioStreamSource* createNew(UsageEnvironment& env, IAudioStream& stream);

public:
    EventTriggerId eventTriggerId; 

    void pushData();
protected:
    LiveAudioStreamSource(UsageEnvironment& env, IAudioStream& stream);
    // called only by createNew(), or by subclass constructors
    virtual ~LiveAudioStreamSource();

private:
    // redefined virtual functions:
    virtual void doGetNextFrame();
    //virtual void doStopGettingFrames(); // optional
    static void backgroundHandler(void *clientData, int mask);
    static void deliverFrame0(void* clientData);
    void deliverFrame();

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
    : FramedSource(env), fAudioStream(stream), eventTriggerId(0)
{
    eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);

    fAudioStream.startStreaming(this);
}

LiveAudioStreamSource::~LiveAudioStreamSource()
{
    fAudioStream.stopStreaming();
    // Reclaim our 'event trigger'
    envir().taskScheduler().deleteEventTrigger(eventTriggerId);
}

void LiveAudioStreamSource::pushData()
{
    //envir().taskScheduler().triggerEvent(eventTriggerId, this);
    deliverFrame();
}

void LiveAudioStreamSource::doGetNextFrame() {
    // This function is called (by our 'downstream' object) when it asks for new data.

    // Note: If, for some reason, the source device stops being readable (e.g., it gets closed), then you do the following:
    if (0 /* the source stops being readable */ /*%%% TO BE WRITTEN %%%*/) {
        handleClosure(this);
        return;
    }

    // If a new frame of data is immediately available to be delivered, then do this now:
    if (0 /* a new frame of data is immediately available to be delivered*/ /*%%% TO BE WRITTEN %%%*/) {
        deliverFrame();
    }

    // No new data is immediately available to be delivered.  We don't do anything more here.
    // Instead, our event trigger must be called (e.g., from a separate thread) when new data becomes available.
}

void LiveAudioStreamSource::deliverFrame0(void* clientData)
{
    if (clientData)
        ((LiveAudioStreamSource*)clientData)->deliverFrame();
}

void LiveAudioStreamSource::deliverFrame()
{
    if (!isCurrentlyAwaitingData()) return; // we're not ready for the data yet

    fFrameSize = fAudioStream.getStreamData(fTo, fMaxSize,
                                            fNumTruncatedBytes,
                                            fPresentationTime);
    if (fFrameSize == 0) return;

    // After delivering the data, inform the reader that it is now available:
    FramedSource::afterGetting(this);
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
