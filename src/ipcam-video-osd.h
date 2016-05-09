/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ipcam-video-osd.h
 * Copyright (C) 2016 Watson Xu <xuhuashan@gmail.com>
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

#ifndef _IPCAM_VIDEO_OSD_H_
#define _IPCAM_VIDEO_OSD_H_

#include "ipcam-media-iface.h"

#ifndef _SDL_TTF_H
struct TTF_Font;
#endif

namespace Ipcam {
namespace Media {

class IpcamVideoOSD
{
public:
	enum OSDType { TEXT_OSD, DATETIME_OSD, IMAGE_OSD };
public:
	IpcamVideoOSD(IpcamRuntime &runtime, IVideoOSD *osd);
	~IpcamVideoOSD();

	OSDType			getType();
	void			setType(OSDType val);
	IVideoOSD::Position	getPosition();
	void			setPosition(IVideoOSD::Position val);
	IVideoOSD::Size	getSize();
	void			setSize(IVideoOSD::Size val);
	uint32_t		getBackgroundColor();
	void			setBackgroundColor(uint32_t val);
	uint32_t		getForegroundAlpha();
	void			setForegroundAlpha(uint32_t val);
	uint32_t		getBackgroundAlpha();
	void			setBackgroundAlpha(uint32_t val);
	bool			getInvertColor();
	void			setInvertColor(bool val);

	std::string		getText();
    void    		setText(std::string text);
    std::string		getFontName();
    void    		setFontName(std::string fontname);
    uint32_t		getFontSize();
    void    		setFontSize(uint32_t size);
    uint32_t		getFontColor();
    void    		setFontColor(uint32_t color);

    std::string		getImagePath();
    void    		setImagePath(std::string path);

private:
	IpcamRuntime&   _runtime;
	IVideoOSD*		_video_osd;
	TTF_Font*		_ttf_font;

	ev::timer		_timer;

	OSDType			_osd_type;
	std::string		_text;
	std::string		_font_name;
	uint32_t		_font_size;
	uint32_t		_font_color;
	std::string		_image_path;

	std::string getPlainText(std::string text);
	void timeout_handler(ev::timer &w, int revents);
	bool loadImage(std::string path);
	void renderText(std::string text);
};

} // namespace Media
} // namespace Ipcam

#endif // _IPCAM_VIDEO_OSD_H_

