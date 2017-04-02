/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * media-common.h
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

#ifndef _MEDIA_COMMON_H_
#define _MEDIA_COMMON_H_

#include <string>
#include <dbus-c++/dbus.h>

typedef DBus::ErrorFailed   IpcamError;

namespace Ipcam {
namespace Media {

extern const char *property_not_implemented;
extern const char *interface_not_implemented;

class Resolution
{
public:
	Resolution(uint32_t w, uint32_t h);
	Resolution(std::string dimension);

	operator std::string ();

	uint32_t width() const { return _width; }
	uint32_t height() const { return _height; }

private:
	uint32_t _width;
	uint32_t _height;
};

struct Position {
	int             x;
	int             y;
	Position(int x, int y) : x(x), y(y) {}
};

struct Size {
	uint32_t        w;
	uint32_t        h;
	Size(uint32_t w, uint32_t h) : w(w), h(h) {}
};

} // namespace Media
} // namespace Ipcam

#endif // _MEDIA_COMMON_H_

