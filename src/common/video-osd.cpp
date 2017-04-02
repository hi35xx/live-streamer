/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * video-osd.cpp
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

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <fontconfig/fontconfig.h>
#include <fontconfig/fcfreetype.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

#include "video-osd.h"

namespace Ipcam {
namespace Media {

//////////////////////////////////////////////////////////////////////////////
// VideoOSD
//////////////////////////////////////////////////////////////////////////////

VideoOSD::VideoOSD()
{
}

VideoOSD::~VideoOSD()
{
}

OSDType VideoOSD::getType()
{
	throw IpcamError(property_not_implemented);
}

void VideoOSD::setType(OSDType value)
{
	throw IpcamError(property_not_implemented);
}

bool VideoOSD::getVisible()
{
	throw IpcamError(property_not_implemented);
}

void VideoOSD::setVisible(bool value)
{
	throw IpcamError(property_not_implemented);
}

Position VideoOSD::getPosition()
{
	throw IpcamError(property_not_implemented);
}

void VideoOSD::setPosition(Position value)
{
	throw IpcamError(property_not_implemented);
}

Size VideoOSD::getSize()
{
	throw IpcamError(property_not_implemented);
}

void VideoOSD::setSize(Size value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoOSD::getForegroundColor()
{
	throw IpcamError(property_not_implemented);
}

void VideoOSD::setForegroundColor(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoOSD::getBackgroundColor()
{
	throw IpcamError(property_not_implemented);
}

void VideoOSD::setBackgroundColor(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoOSD::getForegroundAlpha()
{
	throw IpcamError(property_not_implemented);
}

void VideoOSD::setForegroundAlpha(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoOSD::getBackgroundAlpha()
{
	throw IpcamError(property_not_implemented);
}

void VideoOSD::setBackgroundAlpha(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

bool VideoOSD::getInvertColor()
{
	throw IpcamError(property_not_implemented);
}

void VideoOSD::setInvertColor(bool value)
{
	throw IpcamError(property_not_implemented);
}


std::string VideoOSD::getText()
{
	throw IpcamError(property_not_implemented);
}

void VideoOSD::setText(std::string value)
{
	throw IpcamError(property_not_implemented);
}

std::string VideoOSD::getFontName()
{
	throw IpcamError(property_not_implemented);
}

void VideoOSD::setFontName(std::string value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoOSD::getFontSize()
{
	throw IpcamError(property_not_implemented);
}

void VideoOSD::setFontSize(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoOSD::getFontColor()
{
	throw IpcamError(property_not_implemented);
}

void VideoOSD::setFontColor(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

std::string VideoOSD::getImagePath()
{
	throw IpcamError(property_not_implemented);
}

void VideoOSD::setImagePath(std::string value)
{
	throw IpcamError(property_not_implemented);
}

//////////////////////////////////////////////////////////////////////////////
// SoftRenderVideoOSD
//////////////////////////////////////////////////////////////////////////////

static inline uint32_t argb8888_to_argb1555(uint32_t c)
{
	uint8_t a, r, g, b;
	a = (c >> 27) & 0x1f;
	r = (c >> 19) & 0x1f;
	g = (c >> 11) & 0x1f;
	b = (c >> 3 ) & 0x1f;
	return (((!!a & 0x1) << 15) | (r << 10) | (g << 5) | (b << 0));
}

static TTF_Font *getFontByName(const char *name)
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
	char *colon = strchr((char *) fontname, ':');
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

SoftRenderVideoOSD::SoftRenderVideoOSD(VideoElement* container)
  : MediaElement(NULL), _container(container),
    _ttf_font(NULL), _timer(), _osd_type(TEXT_OSD)
{
	if (!TTF_WasInit() && TTF_Init() == -1) {
		fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
	}
}

SoftRenderVideoOSD::~SoftRenderVideoOSD()
{
	if (_ttf_font) {
		TTF_CloseFont(_ttf_font);
		_ttf_font = NULL;
	}
}

std::string SoftRenderVideoOSD::getPlainText(std::string text)
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
	default:
		break;
	}

	return std::string();
}

void SoftRenderVideoOSD::timeout_handler(ev::timer &w, int revents)
{
	renderText(getPlainText(_text));
}

OSDType SoftRenderVideoOSD::getType()
{
	return _osd_type;
}

void SoftRenderVideoOSD::setType(OSDType value)
{
	if (value == _osd_type)
		return;

	if (value == DATETIME_OSD) {
		_timer.set(1.0, 1.0);
		_timer.set<SoftRenderVideoOSD, &SoftRenderVideoOSD::timeout_handler>(this);
		_timer.start();
	}
	else if (value == DATETIME_OSD) {
		_timer.stop();
	}

	_osd_type = value;
}

std::string SoftRenderVideoOSD::getText()
{
	return _text;
}

void SoftRenderVideoOSD::setText(std::string value)
{
	if (is_enabled()) {
		renderText(getPlainText(value));
	}
	_text = value;
}

std::string SoftRenderVideoOSD::getFontName()
{
	return _font_name;
}

void SoftRenderVideoOSD::setFontName(std::string value)
{
	TTF_Font* font = getFontByName(value.c_str());
	if (font == NULL)
		throw IpcamError("Font not found");

	if (_ttf_font) {
		TTF_CloseFont(_ttf_font);
	}
	_ttf_font = font;
	_font_name = value;
	if (is_enabled()) {
		renderText(getPlainText(value));
	}
}

uint32_t SoftRenderVideoOSD::getFontColor()
{
	return _font_color;
}

void SoftRenderVideoOSD::setFontColor(uint32_t value)
{
	_font_color = value;
	setForegroundColor(argb8888_to_argb1555(value));
	renderText(getPlainText(_text));
}

std::string SoftRenderVideoOSD::getImagePath()
{
	return _image_path;
}

void SoftRenderVideoOSD::setImagePath(std::string value)
{
	if (_osd_type == IMAGE_OSD) {
		loadImage(value);
		_image_path = value;
	}
}

void SoftRenderVideoOSD::loadImage(std::string path)
{
	SDL_Surface *surface = IMG_Load(path.c_str());
	if (!surface) {
		fprintf(stderr, "IMG_Load('%s') failed\n", path.c_str());
		throw IpcamError("Failed to load image");
	}
	SDL_SetAlpha(surface, 0, 0);

	updateDisplay(surface);

	SDL_FreeSurface(surface);
}

void SoftRenderVideoOSD::renderText(std::string text)
{
	if (_osd_type == IMAGE_OSD)
		return;

	if (!_ttf_font) {
		_ttf_font = getFontByName(_font_name.c_str());
		if (!_ttf_font)
			return;
	}

	SDL_Color color = {
		.r = (Uint8)((_font_color >> 16) & 0xff),
		.g = (Uint8)((_font_color >> 8) & 0xff),
		.b = (Uint8)((_font_color >> 0) & 0xff)
	};


	SDL_Surface *surface = TTF_RenderUTF8_Solid(_ttf_font, text.c_str(), color);
	if (!surface)
		return;

	updateDisplay(surface);

	SDL_FreeSurface(surface);
}

} // namespace Media
} // namespace Ipcam
