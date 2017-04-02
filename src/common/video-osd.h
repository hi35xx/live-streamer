/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * video-osd.h
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

#ifndef _VIDEO_OSD_H_
#define _VIDEO_OSD_H_

#include <ev++.h>
#include <set>
#include <media-common.h>
#include <media-element.h>

class _TTF_Font;
typedef struct _TTF_Font TTF_Font;
class SDL_Surface;

namespace Ipcam {
namespace Media {

class VideoOSD;
class SoftRenderVideoOSD;

#define VIDEO_OSD(o)				dynamic_cast<VideoOSD*>(o)
#define SOFT_RENDER_VIDEO_OSD(o)	dynamic_cast<SoftRenderVideoOSD*>(o)

enum OSDType { TEXT_OSD, DATETIME_OSD, IMAGE_OSD };

class VideoOSD
{
public:
	virtual ~VideoOSD();

	virtual OSDType			getType();
	virtual void			setType(OSDType value);
	virtual bool			getVisible();
	virtual void			setVisible(bool value);
	virtual Position		getPosition();
	virtual void			setPosition(Position value);
	virtual Size			getSize();
	virtual void			setSize(Size value);
	virtual uint32_t		getForegroundColor();
	virtual void			setForegroundColor(uint32_t value);
	virtual uint32_t		getBackgroundColor();
	virtual void			setBackgroundColor(uint32_t value);
	virtual uint32_t		getForegroundAlpha();
	virtual void			setForegroundAlpha(uint32_t value);
	virtual uint32_t		getBackgroundAlpha();
	virtual void			setBackgroundAlpha(uint32_t value);
	virtual bool			getInvertColor();
	virtual void			setInvertColor(bool value);

	virtual std::string		getText();
	virtual void			setText(std::string value);
	virtual std::string		getFontName();
	virtual void			setFontName(std::string value);
	virtual uint32_t		getFontSize();
	virtual void			setFontSize(uint32_t value);
	virtual uint32_t		getFontColor();
	virtual void			setFontColor(uint32_t value);

	virtual std::string		getImagePath();
	virtual void			setImagePath(std::string value);

protected:
	VideoOSD();
};

class SoftRenderVideoOSD : virtual public MediaElement, public VideoOSD
{
public:
	SoftRenderVideoOSD(VideoElement* container);
	virtual ~SoftRenderVideoOSD();

	virtual OSDType			getType();
	virtual void			setType(OSDType value);
	virtual std::string		getText();
	virtual void			setText(std::string value);
	virtual std::string		getFontName();
	virtual void			setFontName(std::string value);
	virtual uint32_t		getFontColor();
	virtual void			setFontColor(uint32_t value);
	virtual std::string		getImagePath();
	virtual void			setImagePath(std::string value);

	virtual void			updateDisplay(SDL_Surface* surface) = 0;

	MediaElement*			container() { return _container; }

private:
	MediaElement*			_container;
	TTF_Font*				_ttf_font;
	ev::timer				_timer;
	OSDType					_osd_type;
	std::string				_text;
	std::string				_font_name;
	uint32_t				_font_color;
	std::string				_image_path;

	std::string				getPlainText(std::string text);
	void					timeout_handler(ev::timer& w, int revents);
	void					loadImage(std::string path);
	void					renderText(std::string text);
};

} // namespace Media
} // namespace Ipcam

#endif // _VIDEO_OSD_H_

