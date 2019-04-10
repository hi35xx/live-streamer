/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * video-stream.h
 * Copyright (C) 2017 Watson Xu <xuhuashan@gmail.com>
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

#ifndef _MEDIA_STREAM_H_
#define _MEDIA_STREAM_H_

#include <set>
#include <media-common.h>
#include <media-element.h>

namespace Ipcam {
namespace Media {

enum VideoEncodingType { H264, MJPEG, JPEG, MPEG4 };
enum AudioEncodingType { ADPCM, LPCM, G711A, G711U, G726, AAC };

class StreamBuffer;
class H264StreamBuffer;
class JPEGStreamBuffer;

class StreamSource;
class VideoStreamSource;
class H264VideoStreamSource;
class JPEGVideoStreamSource;
class AudioStreamSource;
class StreamSink;

#define STREAM_SOURCE(o)			dynamic_cast<Ipcam::Media::StreamSource*>(o)
#define VIDEO_STREAM_SOURCE(o)		dynamic_cast<Ipcam::Media::VideoStreamSource*>(o)
#define H264_VIDEO_STREAM_SOURCE(o)	dynamic_cast<Ipcam::Media::H264VideoStreamSource*>(o)
#define JPEG_VIDEO_STREAM_SOURCE(o)	dynamic_cast<Ipcam::Media::JPEGVideoStreamSource*>(o)
#define AUDIO_STREAM_SOURCE(o)		dynamic_cast<Ipcam::Media::AudioStreamSource*>(o)

#define STREAM_SINK(o)				dynamic_cast<Ipcam::Media::StreamSink*>(o)
#define AUDIO_STREAM_SINK(o)		dynamic_cast<Ipcam::Media::AudioStreamSink*>(o)

struct StreamBuffer
{
	struct timeval	tstamp;
	uint32_t		pack_count;
	struct Pack {
		uint8_t*	addr;
		uint32_t	len;
	} *pack;
};

struct H264StreamBuffer : public StreamBuffer
{
	bool keyframe;
};

struct JPEGStreamBuffer : public StreamBuffer
{
	uint8_t qfactor;
	uint8_t width;
	uint8_t height;
};

class StreamSource
{
public:
	virtual ~StreamSource();

	virtual void attach(StreamSink* sink);
	virtual void detach(StreamSink* sink);

	virtual void play() = 0;
	virtual void stop() = 0;
	virtual void pause() = 0;
	virtual void resume() = 0;

protected:
	void streamData(StreamBuffer* buffer);

private:
	typedef std::set<StreamSink*> StreamSinkSet;
	StreamSinkSet _sinks;
};

class StreamSink
{
public:
	virtual void streamData(StreamBuffer* buffer) = 0;
	virtual void play();
	virtual void stop();
	virtual void pause();
	virtual void resume();
};

class VideoStreamSource : public StreamSource
{
public:
	virtual ~VideoStreamSource();

	virtual VideoEncodingType encoding() = 0;
	virtual uint32_t bitrate() = 0;
	virtual Resolution resolution() = 0;
	virtual uint32_t framerate() = 0;
};

class H264VideoStreamSource : virtual public VideoStreamSource
{
public:
	virtual ~H264VideoStreamSource();

	virtual void requestIDR() = 0;
};

class JPEGVideoStreamSource : virtual public VideoStreamSource
{
public:
	virtual ~JPEGVideoStreamSource();
};

class AudioStreamSource : public StreamSource
{
public:
	virtual ~AudioStreamSource();

	virtual AudioEncodingType encoding() = 0;
	virtual uint32_t bitrate() = 0;
	virtual uint32_t channels() = 0;
	virtual uint32_t samplerate() = 0;
};

class AudioStreamSink : public StreamSink
{
public:
	virtual AudioEncodingType encoding() = 0;
	virtual uint32_t bitrate() = 0;
	virtual uint32_t channels() = 0;
	virtual uint32_t samplerate() = 0;
};

} //namespace Media
} //namespace Ipcam

#endif // _MEDIA_STREAM_H_

