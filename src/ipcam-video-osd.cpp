/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ipcam-video-osd.cpp
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

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <fontconfig/fontconfig.h>
#include <fontconfig/fcfreetype.h>

#include <ev++.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

#include <ipcam-runtime.h>

#include "ipcam-video-osd.h"

static inline uint32_t argb8888_to_argb1555(uint32_t c)
{
	uint8_t a, r, g, b;
	a = (c >> 27) & 0x1f;
	r = (c >> 19) & 0x1f;
	g = (c >> 11) & 0x1f;
	b = (c >> 3 ) & 0x1f;
	return (((!!a & 0x1) << 15) | (r << 10) | (g << 5) | (b << 0));
}

namespace Ipcam {
namespace Media {

static TTF_Font *getFont(const char *name)
{
	FcInit();

	FcPattern *p = FcNameParse((const FcChar8 *) name);
	FcConfigSubstitute(0, p, FcMatchPattern);
	FcDefaultSubstitute(p);

	FcResult r = FcResultMatch;
	FcPattern *m = FcFontMatch(0, p, &r);
	FcPatternDestroy(p);
	if (r != FcResultMatch) {
		fprintf(stderr, "get_font: no match\n");
		return 0;
	}

	FcChar8 *fontname = FcNameUnparse(m);
	char *colon = strchr((const char *) fontname, ':');
	if (colon)
		*colon = '\0';
	printf("Found font '%s'\n", fontname);

	FcChar8 *filename;
	r = FcPatternGetString(m, FC_FILE, 0, &filename);
	if (r == FcResultMatch) {
		double pixelsize;
		r = FcPatternGetDouble(m, FC_PIXEL_SIZE, 0, &pixelsize);
		if (r != FcResultMatch)
			pixelsize = 16;

		int index;
		r = FcPatternGetInteger(m, FC_INDEX, 0, &index);
		if (r != FcResultMatch)
			index = 0;

		TTF_Font *font = TTF_OpenFontIndex((const char *)filename,
		                                   (int)pixelsize,
		                                   index);

		FcPatternDestroy(m);

		return font;
	}

	FcPatternDestroy(m);

	fprintf(stderr, "get_font: cannot open font\n");
	return 0;
}

IpcamVideoOSD::IpcamVideoOSD(IpcamRuntime &runtime, IVideoOSD *osd)
  : _runtime(runtime), _video_osd(osd),
    _ttf_font(NULL), _timer(runtime.mainloop()),
    _osd_type(TEXT_OSD)
{
	if(!TTF_WasInit() && TTF_Init()==-1) {
		printf("TTF_Init: %s\n", TTF_GetError());
	}
}

IpcamVideoOSD::~IpcamVideoOSD()
{
	delete _video_osd;
	if (_ttf_font) {
		TTF_CloseFont(_ttf_font);
		_ttf_font = NULL;
	}
}

std::string IpcamVideoOSD::getPlainText(std::string text)
{
	char ostr[200];
	time_t t;
	struct tm *tmp;

	switch (_osd_type) {
	case TEXT_OSD:
		return _text;
	case DATETIME_OSD:
		t = time(NULL);
		tmp = localtime(&t);
		if (text.empty())
			text = "%c";
		if (tmp && (strftime(ostr, sizeof(ostr), text.c_str(), tmp) > 0))
			return std::string(ostr);
	}

	return std::string();
}

void IpcamVideoOSD::timeout_handler(ev::timer &w, int revents)
{
	renderText(getPlainText(_text));
}

IpcamVideoOSD::OSDType IpcamVideoOSD::getType()
{
	return _osd_type;
}

void IpcamVideoOSD::setType(OSDType val)
{
	if (val == _osd_type)
		return;

	setSize(IVideoOSD::Size(16, 16));

	if (val == DATETIME_OSD) {
		_timer.set(1.0, 1.0);
		_timer.set<IpcamVideoOSD, &IpcamVideoOSD::timeout_handler>(this);
		_timer.start();
	}
	else if (_osd_type == DATETIME_OSD) {
		_timer.stop();
	}

	_osd_type = val;
}

IVideoOSD::Position	IpcamVideoOSD::getPosition()
{
	return _video_osd->getPosition();
}

void IpcamVideoOSD::setPosition(IVideoOSD::Position val)
{
	_video_osd->setPosition(val);
}

IVideoOSD::Size	IpcamVideoOSD::getSize()
{
	return _video_osd->getSize();
}

void IpcamVideoOSD::setSize(IVideoOSD::Size val)
{
	_video_osd->setSize(val);
}

uint32_t IpcamVideoOSD::getBackgroundColor()
{
	return _video_osd->getBackgroundColor();
}

void IpcamVideoOSD::setBackgroundColor(uint32_t val)
{
	_video_osd->setBackgroundColor(argb8888_to_argb1555(val));
}

uint32_t IpcamVideoOSD::getForegroundAlpha()
{
	return _video_osd->getForegroundAlpha();
}

void IpcamVideoOSD::setForegroundAlpha(uint32_t val)
{
	_video_osd->setForegroundAlpha(val);
}

uint32_t IpcamVideoOSD::getBackgroundAlpha()
{
	return _video_osd->getBackgroundAlpha();
}

void IpcamVideoOSD::setBackgroundAlpha(uint32_t val)
{
	_video_osd->setBackgroundAlpha(val);
}

bool IpcamVideoOSD::getInvertColor()
{
	return _video_osd->getInvertColor();
}

void IpcamVideoOSD::setInvertColor(bool val)
{
	_video_osd->setInvertColor(val);
}


std::string	IpcamVideoOSD::getText()
{
	return _text;
}

void IpcamVideoOSD::setText(std::string text)
{
	_text = text;
	renderText(getPlainText(_text));
}

std::string	IpcamVideoOSD::getFontName()
{
	return _font_name;
}

void IpcamVideoOSD::setFontName(std::string fontname)
{
	_font_name = fontname;

	if (_ttf_font) {
		TTF_CloseFont(_ttf_font);
		_ttf_font = NULL;
	}
	_ttf_font = getFont(_font_name.c_str());

	if (_ttf_font)
		renderText(getPlainText(_text));
}

uint32_t IpcamVideoOSD::getFontColor()
{
	return _font_color;
}

void IpcamVideoOSD::setFontColor(uint32_t color)
{
	_font_color = color;
	_video_osd->setForegroundColor(argb8888_to_argb1555(color));
	renderText(getPlainText(_text));
}

std::string	IpcamVideoOSD::getImagePath()
{
	return _image_path;
}

void IpcamVideoOSD::setImagePath(std::string path)
{
	if ((_osd_type == IMAGE_OSD) && loadImage(path)) {
		_image_path = path;
	}
}

bool IpcamVideoOSD::loadImage(std::string path)
{
	SDL_Surface *img_surf = IMG_Load(path.c_str());
	if (!img_surf) {
		fprintf(stderr, "IMG_Load('%s') failed\n", path.c_str());
		return false;
	}
	SDL_SetAlpha(img_surf, 0, 0);

	_video_osd->setSize(IVideoOSD::Size(img_surf->w, img_surf->h));

	SDL_Surface *dst_surf = _video_osd->getSurface(img_surf->w, img_surf->h);
	if (!dst_surf) {
		SDL_FreeSurface(img_surf);
		return false;
	}

	SDL_Rect src_rect = { .x = 0, .y = 0, .w = (Uint16)img_surf->w, .h = (Uint16)img_surf->h };
	SDL_Rect dst_rect = { .x = 0, .y = 0, .w = (Uint16)dst_surf->w, .h = (Uint16)dst_surf->h };
	SDL_FillRect(dst_surf, &dst_rect, 0);
	dst_rect.x = (Sint16)((dst_surf->w - img_surf->w) / 2);
	dst_rect.y = (Sint16)((dst_surf->h - img_surf->h) / 2);
	SDL_BlitSurface(img_surf, &src_rect, dst_surf, &dst_rect);
	SDL_FreeSurface(img_surf);

	_video_osd->putSurface(dst_surf);

	return true;
}

void IpcamVideoOSD::renderText(std::string text)
{
	if (_osd_type == IMAGE_OSD)
		return;

	if (!_ttf_font) {
		_ttf_font = getFont(_font_name.c_str());
		if (!_ttf_font)
			return;
	}

	SDL_Color color = {
		.r = (Uint8)((_font_color >> 16) & 0xff),
		.g = (Uint8)((_font_color >> 8) & 0xff),
		.b = (Uint8)((_font_color >> 0) & 0xff)
	};


	SDL_Surface *txt_surf = TTF_RenderUTF8_Solid(_ttf_font, text.c_str(), color);
	if (!txt_surf)
		return;

	SDL_Surface *dst_surf = _video_osd->getSurface(txt_surf->w, txt_surf->h);
	if (!dst_surf) {
		SDL_FreeSurface(txt_surf);
		return;
	}

	SDL_Rect src_rect = { .x = 0, .y = 0, .w = (Uint16)txt_surf->w, .h = (Uint16)txt_surf->h };
	SDL_Rect dst_rect = { .x = 0, .y = 0, .w = (Uint16)dst_surf->w, .h = (Uint16)dst_surf->h };
	SDL_FillRect(dst_surf, &dst_rect, 0);
	dst_rect.x = (Sint16)((dst_surf->w - txt_surf->w) / 2);
	dst_rect.y = (Sint16)((dst_surf->h - txt_surf->h) / 2);
	SDL_BlitSurface(txt_surf, &src_rect, dst_surf, &dst_rect);

	SDL_FreeSurface(txt_surf);

	_video_osd->putSurface(dst_surf);
}


} // namespace Media
} // namespace Ipcam
