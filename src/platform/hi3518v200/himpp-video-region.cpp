/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-region.cpp
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

#include <string.h>
#include <SDL/SDL.h>
#include <himpp-common.h>
#include <mpi_region.h>
#include "himpp-video-region.h"

//////////////////////////////////////////////////////////////////////////////
// HimppRegionAllocator
//////////////////////////////////////////////////////////////////////////////

#define HIMPP_VIDEO_ELEMENT_PTR(o)	dynamic_cast<HimppVideoElement*>(o)

HimppRegionAllocator himpp_region_allocator;

HimppRegionAllocator::HimppRegionAllocator()
{
}

HimppRegionAllocator::~HimppRegionAllocator()
{
}

RGN_HANDLE HimppRegionAllocator::allocHandle()
{
	for(RGN_HANDLE i = 0; i < _region_bmp.size(); i++) {
		if (!_region_bmp[i]) {
			_region_bmp.set(i);
			return i;
		}
	}

	return (RGN_HANDLE)-1;
}

void HimppRegionAllocator::freeHandle(RGN_HANDLE handle)
{
	if (handle < _region_bmp.size())
		_region_bmp.reset(handle);
}

//////////////////////////////////////////////////////////////////////////////
// HimppVideoRegion
//////////////////////////////////////////////////////////////////////////////

static inline HI_S32 argb1555_to_lum(HI_U32 color)
{
	HI_U8 r, g, b;
	r = (color >> 12) & 0xf1;
	g = (color >> 7 ) & 0xf1;
	b = (color << 3 ) & 0xf1;

	return ((299 * r + 587 * g + 114 * b) / 1000);
}

HimppVideoRegion::HimppVideoRegion(HimppVideoElement* container, RGN_HANDLE handle)
  : MediaElement(NULL), SoftRenderVideoOSD(container),
    _rgn_handle(handle), _visible(false), _position(0, 0), _size(4, 4),
    _fgcolor(0xffffff), _bgcolor(0), _fgalpha(128), _bgalpha(0),
    _layer(0), _invert_color(false), _surface(NULL)
{
	_bitmap.enPixelFormat = PIXEL_FORMAT_RGB_1555;
	_bitmap.u32Width = _size.w;
	_bitmap.u32Height = _size.h;
	size_t nlen = _bitmap.u32Width * _bitmap.u32Height * 2;
	_bitmap.pData = malloc(nlen);
	memset(_bitmap.pData, 0, nlen);

	Uint32 Amask = 0x01 << 15;
	Uint32 Rmask = 0x1f << 10;
	Uint32 Gmask = 0x1f << 5;
	Uint32 Bmask = 0x1f << 0;
	_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 0, 0, 16,
	                                Rmask, Gmask, Bmask, Amask);
}

HimppVideoRegion::~HimppVideoRegion()
{
	if (is_enabled())
		disable();

	if (_surface)
		SDL_FreeSurface(_surface);
}

void HimppVideoRegion::doEnableElement()
{
	HI_S32 s32Ret = HI_FALSE;
	RGN_ATTR_S rgn_attr;
	RGN_CHN_ATTR_S chn_attr;

	rgn_attr.enType = OVERLAY_RGN;
	rgn_attr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_1555;
	rgn_attr.unAttr.stOverlay.stSize.u32Width = _size.w;
	rgn_attr.unAttr.stOverlay.stSize.u32Height = _size.h;
	rgn_attr.unAttr.stOverlay.u32BgColor = _bgcolor;

	s32Ret = HI_MPI_RGN_Create(_rgn_handle, &rgn_attr);
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_RGN_Create(%d) failed [%#x]\n",
		        _rgn_handle, s32Ret);
		throw IpcamError("Failed to create RGN");
	}

	memset(&chn_attr, 0, sizeof(chn_attr));
	chn_attr.bShow = (HI_BOOL)_visible;
	chn_attr.enType = OVERLAY_RGN;
	chn_attr.unChnAttr.stOverlayChn.stPoint.s32X = _position.x;
	chn_attr.unChnAttr.stOverlayChn.stPoint.s32Y = _position.y;
	chn_attr.unChnAttr.stOverlayChn.u32BgAlpha = _bgalpha;
	chn_attr.unChnAttr.stOverlayChn.u32FgAlpha = _fgalpha;
	chn_attr.unChnAttr.stOverlayChn.u32Layer = _layer;

	chn_attr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = HI_FALSE;
	chn_attr.unChnAttr.stOverlayChn.stQpInfo.s32Qp = 0;
	chn_attr.unChnAttr.stOverlayChn.stQpInfo.bQpDisable = HI_FALSE;

	chn_attr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width = 16;
	chn_attr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height = 16;
	chn_attr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;
	chn_attr.unChnAttr.stOverlayChn.stInvertColor.enChgMod =
		(argb1555_to_lum(_fgcolor) > 128) ? MORETHAN_LUM_THRESH : LESSTHAN_LUM_THRESH;
	chn_attr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = (HI_BOOL)_invert_color;

	s32Ret = HI_MPI_RGN_AttachToChn(_rgn_handle, HIMPP_VIDEO_ELEMENT_PTR(container())->bindSource(), &chn_attr);
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_RGN_AttachToChn(%d) failed [%#x]\n",
		        _rgn_handle, s32Ret);

		HI_MPI_RGN_Destroy(_rgn_handle);

		throw IpcamError("Failed to attach to chn");
	}

	s32Ret = HI_MPI_RGN_SetBitMap(_rgn_handle, &_bitmap);
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_RGN_SetBitMap(%d) failed [%#x]\n",
		            _rgn_handle, s32Ret);
	}
}

void HimppVideoRegion::doDisableElement()
{
	HI_S32 s32Ret;

	s32Ret = HI_MPI_RGN_DetachFromChn(_rgn_handle, HIMPP_VIDEO_ELEMENT_PTR(container())->bindSource());
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_RGN_DetachFrmChn(%d) failed [%#x]\n",
		        _rgn_handle, s32Ret);
	}

	s32Ret = HI_MPI_RGN_Destroy(_rgn_handle);
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_RGN_Destroy(%d) failed [%#x]\n",
		        _rgn_handle, s32Ret);
	}
}

bool HimppVideoRegion::getVisible()
{
	return _visible;
}

void HimppVideoRegion::setVisible(bool value)
{
	if (is_enabled()) {
		HI_S32 s32Ret;
		RGN_CHN_ATTR_S chn_attr;

		s32Ret = HI_MPI_RGN_GetDisplayAttr(_rgn_handle, HIMPP_VIDEO_ELEMENT_PTR(container())->bindSource(), &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_GetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			throw IpcamError("Failed to get display attribute");
		}
		chn_attr.bShow = (HI_BOOL)value;
		s32Ret = HI_MPI_RGN_SetDisplayAttr(_rgn_handle, HIMPP_VIDEO_ELEMENT_PTR(container())->bindSource(), &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_SetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			throw IpcamError("Failed to set display attribute");
		}
	}

	_visible = (HI_BOOL)value;
}

Size HimppVideoRegion::getSize()
{
	return _size;
}

void HimppVideoRegion::setSize(Size size)
{
	if (_invert_color) {
		size.w = (size.w + 0x0f) & ~0x0f;
		size.h = (size.h + 0x0f) & ~0x0f;
	}
	else {
		size.w = (size.w + 1) & ~0x1;
		size.h = (size.h + 1) & ~0x1;
	}

	if ((size.w == _size.w) &&
	    (size.h == _size.h))
		return;

	size_t nlen = size.w * size.h * 2;
	void *pixels = realloc(_bitmap.pData, nlen);
	if (pixels == NULL)
		throw IpcamError("Out of memory");

	_bitmap.u32Width = size.w;
	_bitmap.u32Height = size.h;
	memset(pixels, 0, nlen);
	_bitmap.pData = pixels;
	_size = size;

	if (is_enabled()) {
		disable();
		enable();
	}
}

uint32_t HimppVideoRegion::getForegroundColor()
{
	return _fgcolor;
}

void HimppVideoRegion::setForegroundColor(uint32_t value)
{
	_fgcolor = value;

	if (_invert_color && is_enabled()) {
		HI_S32 s32Ret;
		RGN_CHN_ATTR_S chn_attr;

		s32Ret = HI_MPI_RGN_GetDisplayAttr(_rgn_handle, HIMPP_VIDEO_ELEMENT_PTR(container())->bindSource(), &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_GetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			throw IpcamError("Failed to get display attribute");
		}
		chn_attr.unChnAttr.stOverlayChn.stInvertColor.enChgMod =
			(argb1555_to_lum(_fgcolor) > 128) ? MORETHAN_LUM_THRESH : LESSTHAN_LUM_THRESH;
		s32Ret = HI_MPI_RGN_SetDisplayAttr(_rgn_handle, HIMPP_VIDEO_ELEMENT_PTR(container())->bindSource(), &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_SetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			throw IpcamError("Failed to set display attribute");
		}
	}
}

uint32_t HimppVideoRegion::getBackgroundColor()
{
	return _bgcolor;
}

void HimppVideoRegion::setBackgroundColor(uint32_t value)
{
	if (is_enabled()) {
		RGN_ATTR_S rgn_attr;
		HI_S32 s32Ret;

		s32Ret = HI_MPI_RGN_GetAttr(_rgn_handle, &rgn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_GetAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			throw IpcamError("Failed to get region attribute");
		}
		rgn_attr.unAttr.stOverlay.u32BgColor = value;
		s32Ret = HI_MPI_RGN_SetAttr(_rgn_handle, &rgn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_SetAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			throw IpcamError("Failed to set region attribute");
		}
	}

	_bgcolor = value;
}

Position HimppVideoRegion::getPosition()
{
	return _position;
}

void HimppVideoRegion::setPosition(Position value)
{
	if (_invert_color) {
		value.x = (value.x + 0x08) & ~0x0f;
		value.y = (value.y + 0x08) & ~0x0f;
	}
	else {
		value.x = (value.x + 2) & ~0x3;
		value.y = (value.y + 2) & ~0x3;
	}

	if (is_enabled()) {
		HI_S32 s32Ret;
		RGN_CHN_ATTR_S chn_attr;

		s32Ret = HI_MPI_RGN_GetDisplayAttr(_rgn_handle, HIMPP_VIDEO_ELEMENT_PTR(container())->bindSource(), &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_GetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			throw IpcamError("Failed to get display attribute");
		}
		chn_attr.unChnAttr.stOverlayChn.stPoint.s32X = value.x;
		chn_attr.unChnAttr.stOverlayChn.stPoint.s32Y = value.y;
		s32Ret = HI_MPI_RGN_SetDisplayAttr(_rgn_handle, HIMPP_VIDEO_ELEMENT_PTR(container())->bindSource(), &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_SetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			throw IpcamError("Failed to set display attribute");
		}
	}

	_position = value;
}

uint32_t HimppVideoRegion::getForegroundAlpha()
{
	return _fgalpha;
}

void HimppVideoRegion::setForegroundAlpha(uint32_t value)
{
	if (is_enabled()) {
		HI_S32 s32Ret;
		RGN_CHN_ATTR_S chn_attr;

		s32Ret = HI_MPI_RGN_GetDisplayAttr(_rgn_handle, HIMPP_VIDEO_ELEMENT_PTR(container())->bindSource(), &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_GetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			throw IpcamError("Failed to get display attribute");
		}
		chn_attr.unChnAttr.stOverlayChn.u32FgAlpha = value;
		s32Ret = HI_MPI_RGN_SetDisplayAttr(_rgn_handle, HIMPP_VIDEO_ELEMENT_PTR(container())->bindSource(), &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_SetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			throw IpcamError("Failed to set display attribute");
		}
	}
	_fgalpha = value;
}

uint32_t HimppVideoRegion::getBackgroundAlpha()
{
	return _bgalpha;
}

void HimppVideoRegion::setBackgroundAlpha(uint32_t value)
{
	if (is_enabled()) {
		HI_S32 s32Ret;
		RGN_CHN_ATTR_S chn_attr;

		s32Ret = HI_MPI_RGN_GetDisplayAttr(_rgn_handle, HIMPP_VIDEO_ELEMENT_PTR(container())->bindSource(), &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_GetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			throw IpcamError("Failed to get display attribute");
		}
		chn_attr.unChnAttr.stOverlayChn.u32BgAlpha = value;
		s32Ret = HI_MPI_RGN_SetDisplayAttr(_rgn_handle, HIMPP_VIDEO_ELEMENT_PTR(container())->bindSource(), &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_SetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			throw IpcamError("Failed to set display attribute");
		}
	}
	_bgalpha = value;
}

bool HimppVideoRegion::getInvertColor()
{
	return _invert_color;
}

void HimppVideoRegion::setInvertColor(bool value)
{
	if (is_enabled()) {
		disable();
		if (value && !_invert_color) {
			setSize(_size);
			setPosition(_position);
		}
		enable();
	}
	else {
		if (value && !_invert_color) {
			setSize(_size);
			setPosition(_position);
		}
	}
	_invert_color = value;
}

void HimppVideoRegion::updateDisplay(SDL_Surface* surface)
{
	if (!_surface) // Sanity check
		return;

	_surface->flags |= SDL_PREALLOC;
	_surface->pixels = _bitmap.pData;
	_surface->w = _bitmap.u32Width;
	_surface->h = _bitmap.u32Height;
	_surface->pitch = _bitmap.u32Width * 2;
	SDL_SetClipRect(_surface, NULL);

	SDL_Rect srect = { .x = 0, .y = 0, .w = (Uint16)surface->w, .h = (Uint16)surface->h };
	SDL_Rect drect = {
		.x = (Sint16)((_surface->w - surface->w) / 2),
		.y = (Sint16)((_surface->h - surface->h) / 2),
		.w = (Uint16)_surface->w,
		.h = (Uint16)_surface->h
	};
	SDL_FillRect(_surface, NULL, 0);
	SDL_BlitSurface(surface, &srect, _surface, &drect);

	if (is_enabled()) {
		HI_S32 s32Ret;
		s32Ret = HI_MPI_RGN_SetBitMap(_rgn_handle, &_bitmap);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_SetBitMap(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
		}
	}
}
