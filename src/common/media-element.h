/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * media-element.h
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

#ifndef _MEDIA_ELEMENT_H_
#define _MEDIA_ELEMENT_H_

#include <atomic>
#include <media-common.h>

namespace Ipcam {
namespace Media {

class MediaElement;
class VideoElement;
class AudioElement;

#define MEDIA_ELEMENT(o)	dynamic_cast<MediaElement*>(o)
#define VIDEO_ELEMENT(o)	dynamic_cast<VideoElement*>(o)
#define AUDIO_ELEMENT(o)	dynamic_cast<AudioElement*>(o)

// Base for A/V componments
class MediaElement
{
public:
	virtual ~MediaElement();

	virtual void enable();				// enable streaming
	virtual void disable();				// disable streaming

	bool is_enabled() { return (_enable_count > 0); }
	inline MediaElement* source() { return _source; }

protected:
	MediaElement(MediaElement* source);

	virtual void doEnableElement() = 0;
	virtual void doDisableElement() = 0;

private:
	MediaElement*		_source;
	std::atomic<int>	_enable_count;
};

// Base for video compoments
class VideoElement : public MediaElement
{
public:
	virtual ~VideoElement();

	virtual Resolution resolution();
	virtual uint32_t framerate();

protected:
	VideoElement(VideoElement* upstream);
};

// Base for audio componments
class AudioElement : public MediaElement
{
public:
	virtual ~AudioElement();

	virtual uint32_t samplerate();
	virtual uint32_t num_channels();

protected:
	AudioElement(AudioElement* upstream);
};

} // namespace Media
} // namespace Ipcam

#endif // _MEDIA_ELEMENT_H_

