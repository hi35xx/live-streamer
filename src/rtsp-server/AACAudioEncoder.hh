/*
 * AACAudioEncoder.hh
 * Copyright (C) 2015 Watson Xu <xuhuashan@gmail.com>
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
// "LIVE555 Streaming Media" interface to an AAC audio encoder.
// C++ header

#ifndef  _AAC_AUDIO_ENCODER_HH
#define _AAC_AUDIO_ENCODER_HH

#include "FramedFilter.hh"

class AACAudioEncoder: public FramedFilter {
public:
  static AACAudioEncoder* createNew(UsageEnvironment& env,
				    FramedSource* inputPCMSource,
				    unsigned numChannels, unsigned samplingRate,
				    unsigned outputKbps);

protected:
  AACAudioEncoder(UsageEnvironment& env, FramedSource* inputPCMSource,
		  unsigned numChannels, unsigned samplingRate, unsigned outputKbps);
      // called only by createNew()
  virtual ~AACAudioEncoder();

private:
  // redefined virtual functions:
  virtual void doGetNextFrame();

private:
  static void afterGettingFrame(void* clientData, unsigned frameSize,
                                unsigned numTruncatedBytes,
                                struct timeval presentationTime,
                                unsigned durationInMicroseconds);
  void afterGettingFrame1(unsigned frameSize,
                          unsigned numTruncatedBytes,
                          struct timeval presentationTime,
                          unsigned durationInMicroseconds);

private:
  void* fEncoderHandle;
  unsigned long fNumSamplesPerFrame;
  unsigned fMicrosecondsPerFrame;
  double fMicrosecondsPerByte;
  unsigned char* fInputSampleBuffer;
  unsigned fInputSampleBufferSize;
  unsigned fInputSampleBufferBytesDesired;
  unsigned fInputSampleBufferBytesUsed;
  struct timeval fLastInputDataPresentationTime;
};

#endif
