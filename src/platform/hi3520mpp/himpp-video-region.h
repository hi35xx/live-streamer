/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-region.h
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

#ifndef _HIMPP_VIDEO_REGION_H_
#define _HIMPP_VIDEO_REGION_H_

#include <himpp-object.h>
#include <hi_comm_region.h>

class HimppVideoRegion
{
public:
	HimppVideoRegion(HimppVideoObject *source, RGN_HANDLE handle);
	~HimppVideoRegion();

	bool enable();
    bool disable();
	bool isEnabled() { return _enabled; }
	uint32_t id() { return (uint32_t)_rgn_handle; }

	SIZE_S getSize();
	bool setSize(SIZE_S size);
	HI_U32 getFgColor();
	bool setFgColor(HI_U32 fgcolor);
	HI_U32 getBgColor();
	bool setBgColor(HI_U32 bgcolor);

	POINT_S getPosition();
	bool setPosition(POINT_S pos);
	HI_U32 getFgAlpha();
	bool setFgAlpha(HI_U32 fgalpha);
	HI_U32 getBgAlpha();
	bool setBgAlpha(HI_U32 bgalpha);
	HI_U32 getLayer();
	bool setLayer(HI_U32 layer);
	HI_BOOL getInvertColor();
	bool setInvertColor(HI_BOOL invert_color);

	BITMAP_S *getBitmap(HI_U32 w, HI_U32 h);
	void setBitmap(BITMAP_S *bitmap);
private:
	HimppVideoObject *_source;
	bool		_enabled;
	RGN_HANDLE	_rgn_handle;

	SIZE_S		_size;
	HI_U32		_fgcolor;
	HI_U32		_bgcolor;
	POINT_S		_position;
	HI_U32		_fgalpha;
	HI_U32		_bgalpha;
	HI_U32		_layer;
	HI_BOOL		_invert_color;
	BITMAP_S	_bitmap;
};

#endif // _HIMPP_VIDEO_REGION_H_

