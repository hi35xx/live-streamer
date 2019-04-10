/*
 * AACAudioEncoder.cpp
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
// "LIVE555 Streaming Media" interface to an AAC audio encoder.
// Implementation

#include "AACAudioEncoder.hh"
extern "C" {
#include <vo-aacenc/voAAC.h>
#include <vo-aacenc/cmnMemory.h>
}

static VO_AUDIO_CODECAPI codec_api = { 0 };
static VO_MEM_OPERATOR mem_operator = { 0 };

AACAudioEncoder* AACAudioEncoder
::createNew(UsageEnvironment& env, FramedSource* inputPCMSource,
	    unsigned numChannels, unsigned samplingRate, unsigned outputKbps) {
  return new AACAudioEncoder(env, inputPCMSource, numChannels, samplingRate, outputKbps);
}

#ifndef MILLION
#define MILLION 1000000
#endif

AACAudioEncoder
::AACAudioEncoder(UsageEnvironment& env, FramedSource* inputPCMSource,
		  unsigned numChannels, unsigned samplingRate, unsigned outputKbps)
  : FramedFilter(env, inputPCMSource) {
  fNumSamplesPerFrame = numChannels * 1024;

  // Note: "fNumSamplesPerFrame" is already multiplied by "numChannels"
  fMicrosecondsPerFrame = (MILLION*fNumSamplesPerFrame)/(numChannels*samplingRate);
  fMicrosecondsPerByte
    = (1.0*MILLION)/(samplingRate*numChannels*sizeof (unsigned short));
  fInputSampleBufferBytesDesired = fNumSamplesPerFrame*sizeof (unsigned short);
  fInputSampleBufferSize = 4*fInputSampleBufferBytesDesired;
  fInputSampleBuffer = new unsigned char[fInputSampleBufferSize];
  fInputSampleBufferBytesUsed = 0;

  fLastInputDataPresentationTime.tv_sec = 0;
  fLastInputDataPresentationTime.tv_usec = 0;

  // Set remaining parameters of the encoder:
  voGetAACEncAPI(&codec_api);
  mem_operator.Alloc = cmnMemAlloc;
  mem_operator.Copy = cmnMemCopy;
  mem_operator.Free = cmnMemFree;
  mem_operator.Set = cmnMemSet;
  mem_operator.Check = cmnMemCheck;
  VO_CODEC_INIT_USERDATA user_data;
  user_data.memflag = VO_IMF_USERMEMOPERATOR;
  user_data.memData = &mem_operator;
  codec_api.Init(&fEncoderHandle, VO_AUDIO_CodingAAC, &user_data);

  AACENC_PARAM params = { 0 };
  params.sampleRate = samplingRate;
  params.bitRate = outputKbps * 1000;
  params.nChannels = numChannels;
  params.adtsUsed = 0;
  if (codec_api.SetParam(fEncoderHandle, VO_PID_AAC_ENCPARAM, &params) != VO_ERR_NONE) {
	  envir() << "Unable to set encoding parameters\n";
  }
}

AACAudioEncoder::~AACAudioEncoder() {
  codec_api.Uninit(fEncoderHandle);
  delete[] fInputSampleBuffer;
}

void AACAudioEncoder::doGetNextFrame() {
  // If we have enough samples in order to encode a frame, do this now:
  if (fInputSampleBufferBytesUsed >= fInputSampleBufferBytesDesired) {
    VO_CODECBUFFER input, output;
    VO_AUDIO_OUTPUTINFO output_info;
    input.Buffer = (uint8_t*)fInputSampleBuffer;
    input.Length = fInputSampleBufferBytesDesired;
    codec_api.SetInputData(fEncoderHandle, &input);
    output.Buffer = fTo;
    output.Length = fMaxSize;
    VO_U32 retval = codec_api.GetOutputData(fEncoderHandle, &output, &output_info);
    if (retval != VO_ERR_NONE) {
      fFrameSize = 0;
      fNumTruncatedBytes = fNumSamplesPerFrame;
	} else {
      fFrameSize = output.Length;
      fNumTruncatedBytes = 0;
	}

    // Shift the data that we just encoded, plus any remaining samples,
    // down the the start of the buffer.  (The data that we just encoded will be the overlap
    // buffer for the next encoding.)
    fInputSampleBufferBytesUsed -= fInputSampleBufferBytesDesired;
    memmove(fInputSampleBuffer,
	    &fInputSampleBuffer[fInputSampleBufferBytesDesired],
	    fInputSampleBufferBytesUsed);

    // Complete delivery to the client:
    fPresentationTime = fLastInputDataPresentationTime;
    //fDurationInMicroseconds = fMicrosecondsPerFrame;
    fDurationInMicroseconds = 0; // because audio capture is bursty, check for it ASAP
    afterGetting(this);
  } else {
    // Read more samples from our source, then try again:
    unsigned maxBytesToRead
      = fInputSampleBufferSize - fInputSampleBufferBytesUsed;
    fInputSource
      ->getNextFrame(&fInputSampleBuffer[fInputSampleBufferBytesUsed],
		     maxBytesToRead,
		     afterGettingFrame, this,
		     FramedSource::handleClosure, this);
  }
}

void AACAudioEncoder
::afterGettingFrame(void* clientData, unsigned frameSize,
                    unsigned numTruncatedBytes,
                    struct timeval presentationTime,
                    unsigned durationInMicroseconds) {
  AACAudioEncoder* source = (AACAudioEncoder*)clientData;
  source->afterGettingFrame1(frameSize, numTruncatedBytes,
                             presentationTime, durationInMicroseconds);
}

void AACAudioEncoder
::afterGettingFrame1(unsigned frameSize, unsigned numTruncatedBytes,
                     struct timeval presentationTime, unsigned durationInMicroseconds) {
  // Adjust presentationTime to allow for the data that's still in our buffer:
  int uSecondsAdjustment = (int)(fInputSampleBufferBytesUsed*fMicrosecondsPerByte);
  presentationTime.tv_sec -= uSecondsAdjustment/MILLION;
  uSecondsAdjustment %= MILLION;
  if (presentationTime.tv_usec < uSecondsAdjustment) {
    --presentationTime.tv_sec;
    presentationTime.tv_usec += MILLION;
  }
  presentationTime.tv_usec -= uSecondsAdjustment;

  // Don't allow the presentation time to decrease:
  if (presentationTime.tv_sec > fLastInputDataPresentationTime.tv_sec ||
      (presentationTime.tv_sec == fLastInputDataPresentationTime.tv_sec &&
       presentationTime.tv_usec > fLastInputDataPresentationTime.tv_usec)) {
    fLastInputDataPresentationTime = presentationTime;
  }
  fInputSampleBufferBytesUsed += frameSize;  

  // Try again to encode and deliver data to the sink:
  doGetNextFrame();
}
