/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * audio-encoder.cpp
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

#include "audio-encoder.h"


namespace Ipcam {
namespace Media {

AudioEncoder::~AudioEncoder()
{
}

AudioEncodingType AudioEncoder::getEncoding()
{
	throw IpcamError(property_not_implemented);
}

uint32_t AudioEncoder::getBitrate()
{
	throw IpcamError(property_not_implemented);
}

void AudioEncoder::setBitrate(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t AudioEncoder::getSampleRate()
{
	throw IpcamError(property_not_implemented);
}

void AudioEncoder::setSampleRate(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

} //namespace Media
} //namespace Ipcam
