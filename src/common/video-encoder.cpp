/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * video-encoder.cpp
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

#include "video-encoder.h"

namespace Ipcam {
namespace Media {

//////////////////////////////////////////////////////////////////////////////
// VideoEncoder
//////////////////////////////////////////////////////////////////////////////

VideoEncoder::VideoEncoder()
{
}

VideoEncoder::~VideoEncoder()
{
}

VideoEncodingType VideoEncoder::getEncoding()
{
	throw IpcamError(property_not_implemented);
}

Resolution VideoEncoder::getResolution()
{
	throw IpcamError(property_not_implemented);
}

void VideoEncoder::setResolution(Resolution value)
{
	throw IpcamError(property_not_implemented);
}

RateCtrlMode VideoEncoder::getRcMode()
{
	throw IpcamError(property_not_implemented);
}

void VideoEncoder::setRcMode(RateCtrlMode value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoEncoder::getFramerate()
{
	throw IpcamError(property_not_implemented);
}

void VideoEncoder::setFramerate(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoEncoder::getBitrate()
{
	throw IpcamError(property_not_implemented);
}

void VideoEncoder::setBitrate(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

VideoOSD* VideoEncoder::CreateOSD()
{
	throw IpcamError(property_not_implemented);
}

void VideoEncoder::DeleteOSD(VideoOSD* osd)
{
	throw IpcamError(property_not_implemented);
}

//////////////////////////////////////////////////////////////////////////////
// H264VideoEncoder
//////////////////////////////////////////////////////////////////////////////

H264VideoEncoder::H264VideoEncoder()
{
}

H264VideoEncoder::~H264VideoEncoder()
{
}

H264Profile H264VideoEncoder::getH264Profile()
{
	throw IpcamError(property_not_implemented);
}

void H264VideoEncoder::setH264Profile(H264Profile value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t H264VideoEncoder::getGovLength()
{
	throw IpcamError(property_not_implemented);
}

void H264VideoEncoder::setGovLength(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

void H264VideoEncoder::setFrameRefMode(FrameRefMode value)
{
	throw IpcamError(property_not_implemented);
}

H264VideoEncoder::FrameRefMode H264VideoEncoder::getFrameRefMode()
{
	throw IpcamError(property_not_implemented);
}

void H264VideoEncoder::setIntraRefresh(IntraRefreshParam value)
{
	throw IpcamError(property_not_implemented);
}

H264VideoEncoder::IntraRefreshParam H264VideoEncoder::getIntraRefresh()
{
	throw IpcamError(property_not_implemented);
}

//////////////////////////////////////////////////////////////////////////////
// JPEGVideoEncoder
//////////////////////////////////////////////////////////////////////////////

JPEGVideoEncoder::JPEGVideoEncoder()
{
}

JPEGVideoEncoder::~JPEGVideoEncoder()
{
}

} // namespace Media
} // namespace Ipcam
