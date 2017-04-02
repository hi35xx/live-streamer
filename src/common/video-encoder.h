/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * video-encoder.h
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

#ifndef _VIDEO_ENCODER_H_
#define _VIDEO_ENCODER_H_

#include <media-common.h>
#include <media-element.h>
#include <video-osd.h>
#include <media-stream.h>

namespace Ipcam {
namespace Media {

enum RateCtrlMode { CBR, VBR, FIXQP };
enum H264Profile { BASELINE, MAIN, HIGH, SVC_T };

class VideoEncoder;
class H264VideoEncoder;
class JPEGVideoEncoder;

#define VIDEO_ENCODER(o)		dynamic_cast<Ipcam::Media::VideoEncoder*>(o)
#define H264_VIDEO_ENCODER(o)	dynamic_cast<Ipcam::Media::H264VideoEncoder*>(o)
#define JPEG_VIDEO_ENCODER(o)	dynamic_cast<Ipcam::Media::JPEGVideoEncoder*>(o)

// control interface for video encoder
class VideoEncoder
{
public:
	virtual ~VideoEncoder();

	virtual VideoEncodingType	getEncoding();
	virtual Resolution			getResolution();
	virtual void				setResolution(Resolution value);
	virtual uint32_t			getFramerate();
	virtual void				setFramerate(uint32_t value);
	virtual RateCtrlMode		getRcMode();
	virtual void				setRcMode(RateCtrlMode value);
	virtual uint32_t			getBitrate();
	virtual void				setBitrate(uint32_t value);

	virtual VideoOSD*			CreateOSD();
	virtual void				DeleteOSD(VideoOSD* osd);

protected:
	VideoEncoder();
};

class H264VideoEncoder : public virtual VideoEncoder
{
public:
	virtual ~H264VideoEncoder();

	virtual H264Profile			getH264Profile();
	virtual void				setH264Profile(H264Profile value);
	virtual uint32_t			getGovLength();
	virtual void				setGovLength(uint32_t value);

protected:
	H264VideoEncoder();
};

class JPEGVideoEncoder : virtual public VideoEncoder
{
public:
	virtual ~JPEGVideoEncoder();

protected:
	JPEGVideoEncoder();
};

} // namespace Media
} // namespace Ipcam

#endif // _VIDEO_ENCODER_H_

