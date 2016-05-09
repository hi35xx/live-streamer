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
#include <himpp-common.h>
#include <mpi_region.h>
#include "himpp-video-region.h"

static inline HI_S32 argb1555_to_lum(HI_U32 color)
{
	HI_U8 r, g, b;
	r = (color >> 12) & 0xf1;
	g = (color >> 7 ) & 0xf1;
	b = (color << 3 ) & 0xf1;

	return ((299 * r + 587 * g + 114 * b) / 1000);
}

HimppVideoRegion::HimppVideoRegion(HimppVideoObject *source, RGN_HANDLE handle)
	:_source(source), _rgn_handle(handle), _enabled(false),
	 _bgcolor(0), _fgalpha(128), _bgalpha(0),
	 _layer(0), _invert_color(HI_FALSE)
{
	_size.u32Width = 4;
	_size.u32Height = 4;
	_position.s32X = 0;
	_position.s32Y = 0;

	_bitmap.enPixelFormat = PIXEL_FORMAT_RGB_1555;
	_bitmap.u32Width = _size.u32Width;
	_bitmap.u32Height = _size.u32Height;
	size_t nlen = _bitmap.u32Width * _bitmap.u32Height * 2;
	_bitmap.pData = malloc(nlen);
	memset(_bitmap.pData, 0, nlen);
}

HimppVideoRegion::~HimppVideoRegion()
{
	if (isEnabled())
		disable();
}

bool HimppVideoRegion::enable()
{
	HI_S32 s32Ret = HI_FALSE;
	RGN_ATTR_S rgn_attr;
	RGN_CHN_ATTR_S chn_attr;

	if (_enabled)
		return true;

	rgn_attr.enType = OVERLAY_RGN;
	rgn_attr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_1555;
	rgn_attr.unAttr.stOverlay.stSize.u32Width = _size.u32Width;
	rgn_attr.unAttr.stOverlay.stSize.u32Height = _size.u32Height;
	rgn_attr.unAttr.stOverlay.u32BgColor = _bgcolor;

	s32Ret = HI_MPI_RGN_Create(_rgn_handle, &rgn_attr);
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_RGN_Create(%d) failed [%#x]\n",
		        _rgn_handle, s32Ret);
		return false;
	}

	memset(&chn_attr, 0, sizeof(chn_attr));
	chn_attr.bShow = HI_TRUE;
	chn_attr.enType = OVERLAY_RGN;
	chn_attr.unChnAttr.stOverlayChn.stPoint.s32X = _position.s32X;
	chn_attr.unChnAttr.stOverlayChn.stPoint.s32Y = _position.s32Y;
	chn_attr.unChnAttr.stOverlayChn.u32BgAlpha = _bgalpha;
	chn_attr.unChnAttr.stOverlayChn.u32FgAlpha = _fgalpha;
	chn_attr.unChnAttr.stOverlayChn.u32Layer = _layer;

	chn_attr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = HI_FALSE;
	chn_attr.unChnAttr.stOverlayChn.stQpInfo.s32Qp = 0;

	chn_attr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width = 16;
	chn_attr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height = 16;
	chn_attr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;
	chn_attr.unChnAttr.stOverlayChn.stInvertColor.enChgMod =
		(argb1555_to_lum(_fgcolor) > 128) ? MORETHAN_LUM_THRESH : LESSTHAN_LUM_THRESH;
	chn_attr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = _invert_color;

	MPP_CHN_S *chn = (MPP_CHN_S*)*_source;
	s32Ret = HI_MPI_RGN_AttachToChn(_rgn_handle, (MPP_CHN_S *)*_source, &chn_attr);
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_RGN_AttachToChn(%d) failed [%#x]\n",
		        _rgn_handle, s32Ret);

		HI_MPI_RGN_Destroy(_rgn_handle);

		return false;
	}

	s32Ret = HI_MPI_RGN_SetBitMap(_rgn_handle, &_bitmap);
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_RGN_SetBitMap(%d) failed [%#x]\n",
		            _rgn_handle, s32Ret);
	}

	_enabled = true;

	return true;
}

bool HimppVideoRegion::disable()
{
	HI_S32 s32Ret;

	if (!isEnabled())
		return true;

	s32Ret = HI_MPI_RGN_DetachFrmChn(_rgn_handle, (MPP_CHN_S *)*_source);
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_RGN_DetachFrmChn(%d) failed [%#x]\n",
		        _rgn_handle, s32Ret);
	}

	s32Ret = HI_MPI_RGN_Destroy(_rgn_handle);
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_RGN_Destroy(%d) failed [%#x]\n",
		        _rgn_handle, s32Ret);
	}

	_enabled = false;

	return true;
}

SIZE_S HimppVideoRegion::getSize()
{
	return _size;
}

bool HimppVideoRegion::setSize(SIZE_S size)
{
	if (_invert_color) {
		size.u32Width = (size.u32Width + 0x0f) & ~0x0f;
		size.u32Height = (size.u32Height + 0x0f) & ~0x0f;
	}
	else {
		size.u32Width = (size.u32Width + 1) & ~0x1;
		size.u32Height = (size.u32Height + 1) & ~0x1;
	}

	if ((size.u32Width == _size.u32Width) &&
	    (size.u32Height == _size.u32Height))
		return true;

	size_t nlen = size.u32Width * size.u32Height * 2;
	void *pixels = realloc(_bitmap.pData, nlen);
	if (pixels == NULL)
		return false;

	_bitmap.u32Width = size.u32Width;
	_bitmap.u32Height = size.u32Height;
	memset(pixels, 0, nlen);
	_bitmap.pData = pixels;
	_size = size;

	if (isEnabled()) {
		disable();
		enable();
	}

	return true;
}

HI_U32 HimppVideoRegion::getFgColor()
{
	return _fgcolor;
}

bool HimppVideoRegion::setFgColor(HI_U32 fgcolor)
{
	_fgcolor = fgcolor;

	if (_invert_color && isEnabled()) {
		HI_S32 s32Ret;
		RGN_CHN_ATTR_S chn_attr;

		s32Ret = HI_MPI_RGN_GetDisplayAttr(_rgn_handle, (MPP_CHN_S *)*_source, &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_GetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			return false;
		}
		chn_attr.unChnAttr.stOverlayChn.stInvertColor.enChgMod =
			(argb1555_to_lum(_fgcolor) > 128) ? MORETHAN_LUM_THRESH : LESSTHAN_LUM_THRESH;
		s32Ret = HI_MPI_RGN_SetDisplayAttr(_rgn_handle, (MPP_CHN_S *)*_source, &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_SetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			return false;
		}
	}

	return true;
}

HI_U32 HimppVideoRegion::getBgColor()
{
	return _bgcolor;
}

bool HimppVideoRegion::setBgColor(HI_U32 bgcolor)
{
	if (isEnabled()) {
		RGN_ATTR_S rgn_attr;
		HI_S32 s32Ret;

		s32Ret = HI_MPI_RGN_GetAttr(_rgn_handle, &rgn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_GetAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			return false;
		}
		rgn_attr.unAttr.stOverlay.u32BgColor = bgcolor;
		s32Ret = HI_MPI_RGN_SetAttr(_rgn_handle, &rgn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_SetAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			return false;
		}
	}

	_bgcolor = bgcolor;

	return true;
}

POINT_S HimppVideoRegion::getPosition()
{
	return _position;
}

bool HimppVideoRegion::setPosition(POINT_S pos)
{

	if (_invert_color) {
		pos.s32X = (pos.s32X + 0x08) & ~0x0f;
		pos.s32Y = (pos.s32Y + 0x08) & ~0x0f;
	}
	else {
		pos.s32X = (pos.s32X + 2) & ~0x3;
		pos.s32Y = (pos.s32Y + 2) & ~0x3;
	}

	if (isEnabled()) {
		HI_S32 s32Ret;
		RGN_CHN_ATTR_S chn_attr;

		s32Ret = HI_MPI_RGN_GetDisplayAttr(_rgn_handle, (MPP_CHN_S *)*_source, &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_GetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			return false;
		}
		chn_attr.unChnAttr.stOverlayChn.stPoint = pos;
		s32Ret = HI_MPI_RGN_SetDisplayAttr(_rgn_handle, (MPP_CHN_S *)*_source, &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_SetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			return false;
		}
	}
	_position = pos;

	return true;
}

HI_U32 HimppVideoRegion::getFgAlpha()
{
	return _fgalpha;
}

bool HimppVideoRegion::setFgAlpha(HI_U32 alpha)
{
	if (isEnabled()) {
		HI_S32 s32Ret;
		RGN_CHN_ATTR_S chn_attr;

		s32Ret = HI_MPI_RGN_GetDisplayAttr(_rgn_handle, (MPP_CHN_S *)*_source, &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_GetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			return false;
		}
		chn_attr.unChnAttr.stOverlayChn.u32FgAlpha = alpha;
		s32Ret = HI_MPI_RGN_SetDisplayAttr(_rgn_handle, (MPP_CHN_S *)*_source, &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_SetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			return false;
		}
	}
	_fgalpha = alpha;

	return true;
}

HI_U32 HimppVideoRegion::getBgAlpha()
{
	return _bgalpha;
}

bool HimppVideoRegion::setBgAlpha(HI_U32 alpha)
{
	if (isEnabled()) {
		HI_S32 s32Ret;
		RGN_CHN_ATTR_S chn_attr;

		s32Ret = HI_MPI_RGN_GetDisplayAttr(_rgn_handle, (MPP_CHN_S *)*_source, &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_GetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			return false;
		}
		chn_attr.unChnAttr.stOverlayChn.u32BgAlpha = alpha;
		s32Ret = HI_MPI_RGN_SetDisplayAttr(_rgn_handle, (MPP_CHN_S *)*_source, &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_SetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			return false;
		}
	}
	_bgalpha = alpha;

	return true;
}

HI_U32 HimppVideoRegion::getLayer()
{
	return _layer;
}

bool HimppVideoRegion::setLayer(HI_U32 layer)
{
	if (isEnabled()) {
		HI_S32 s32Ret;
		RGN_CHN_ATTR_S chn_attr;

		s32Ret = HI_MPI_RGN_GetDisplayAttr(_rgn_handle, (MPP_CHN_S *)*_source, &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_GetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			return false;
		}
		chn_attr.unChnAttr.stOverlayChn.u32Layer = layer;
		s32Ret = HI_MPI_RGN_SetDisplayAttr(_rgn_handle, (MPP_CHN_S *)*_source, &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_SetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			return false;
		}
	}
	_layer = layer;

	return true;
}

HI_BOOL HimppVideoRegion::getInvertColor()
{
	return _invert_color;
}

bool HimppVideoRegion::setInvertColor(HI_BOOL invert_color)
{
	if (invert_color && !_invert_color) {
		_invert_color = HI_TRUE;
		setSize(_size);
		setPosition(_position);
		_invert_color = HI_FALSE;
	}

	if (isEnabled()) {
		HI_S32 s32Ret;
		RGN_CHN_ATTR_S chn_attr;

		s32Ret = HI_MPI_RGN_GetDisplayAttr(_rgn_handle, (MPP_CHN_S *)*_source, &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_GetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			return false;
		}
		chn_attr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = invert_color;
		chn_attr.unChnAttr.stOverlayChn.stInvertColor.enChgMod =
			(argb1555_to_lum(_fgcolor) < 128) ? MORETHAN_LUM_THRESH : LESSTHAN_LUM_THRESH;
		s32Ret = HI_MPI_RGN_SetDisplayAttr(_rgn_handle, (MPP_CHN_S *)*_source, &chn_attr);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_SetDisplayAttr(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
			return false;
		}
	}
	_invert_color = invert_color;

	return true;
}

BITMAP_S *HimppVideoRegion::getBitmap(HI_U32 w, HI_U32 h)
{
	if (_invert_color) {
		w = (w + 0x0f) & ~0x0f;
		h = (h + 0x0f) & ~0x0f;
	}
	else {
		w = (w + 1) & ~0x1;
		h = (h + 1) & ~0x1;
	}

	// need to inflate the region size
	if ((w > _size.u32Width) || (h > _size.u32Height)) {
		SIZE_S s;
		s.u32Width = w > _size.u32Width ? w : _size.u32Width;
		s.u32Height = h > _size.u32Height ? h : _size.u32Height;

		setSize(s);
	}

	return &_bitmap;
}

void HimppVideoRegion::setBitmap(BITMAP_S *bmp)
{
	HI_S32 s32Ret;

	if (isEnabled()) {
		s32Ret = HI_MPI_RGN_SetBitMap(_rgn_handle, bmp);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_RGN_SetBitMap(%d) failed [%#x]\n",
			            _rgn_handle, s32Ret);
		}
	}
}
