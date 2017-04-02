/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * audio-source.h
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

#ifndef _AUDIO_SOURCE_H_
#define _AUDIO_SOURCE_H_

#include <media-common.h>

namespace Ipcam {
namespace Media {

class AudioSource;

#define AUDIO_SOURCE(o)		dynamic_cast<Ipcam::Media::AudioSource*>(o)

class AudioSource
{
public:
	virtual ~AudioSource();

	virtual uint32_t getChannels() = 0;

protected:
	AudioSource();
};

} //namespace Media
} //namespace Ipcam

#endif // _AUDIO_SOURCE_H_

