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

#include <bitset>
#include <himpp-base.h>
#include <video-osd.h>
#include <hi_comm_region.h>

using namespace Ipcam::Media;

class HimppVideoRegion;

#define HIMPP_VIDEO_REGION(o)	dynamic_cast<HimppVideoRegion*>(o)

class HimppRegionAllocator
{
public:
	HimppRegionAllocator();
	~HimppRegionAllocator();

	RGN_HANDLE allocHandle();
	void freeHandle(RGN_HANDLE handle);
private:
	std::bitset<RGN_HANDLE_MAX> _region_bmp;
};

class HimppVideoRegion : public SoftRenderVideoOSD
{
public:
	HimppVideoRegion(HimppVideoElement* container, RGN_HANDLE handle);
	~HimppVideoRegion();

	void		doEnableElement();
	void		doDisableElement();

	uint32_t	regionId() { return (uint32_t)_rgn_handle; }

	bool		getVisible();
	void		setVisible(bool value);
	Size		getSize();
	void		setSize(Size value);
	uint32_t	getForegroundColor();
	void		setForegroundColor(uint32_t value);
	uint32_t	getBackgroundColor();
	void		setBackgroundColor(uint32_t value);

	Position	getPosition();
	void		setPosition(Position value);
	uint32_t	getForegroundAlpha();
	void		setForegroundAlpha(uint32_t value);
	uint32_t	getBackgroundAlpha();
	void		setBackgroundAlpha(uint32_t value);
	bool		getInvertColor();
	void		setInvertColor(bool value);

	void		updateDisplay(SDL_Surface* surface);

private:
	RGN_HANDLE	_rgn_handle;
	bool		_visible;
	Position	_position;
	Size		_size;
	uint32_t	_fgcolor;
	uint32_t	_bgcolor;
	uint32_t	_fgalpha;
	uint32_t	_bgalpha;
	uint32_t	_layer;
	bool		_invert_color;
	BITMAP_S	_bitmap;
	SDL_Surface* _surface;
};

extern HimppRegionAllocator himpp_region_allocator;

#endif // _HIMPP_VIDEO_REGION_H_

