/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * media-common.cpp
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

#include <regex>

#include "media-common.h"

namespace Ipcam {
namespace Media {

struct ResolutionEntry
{
	const char *name;
	uint16_t width;
	uint16_t height;
};

static ResolutionEntry _resolution_table[] = {
	{ .name =  "QCIF", .width = 176,  .height = 144  },
	{ .name =  "CIF",  .width = 352,  .height = 288  },
	{ .name =  "D1",   .width = 720,  .height = 576  },
	{ .name =  "960H", .width = 960,  .height = 576  },
	{ .name =  "2CIF", .width = 360,  .height = 576  },
	{ .name =  "QVGA", .width = 320,  .height = 240  },
	{ .name =  "VGA",  .width = 640,  .height = 480  },
	{ .name =  "XGA",  .width = 1024, .height = 760  },
	{ .name =  "SXGA", .width = 1400, .height = 1050 },
	{ .name =  "UXGA", .width = 1600, .height = 1200 },
	{ .name =  "QXGA", .width = 2048, .height = 1536 },
	{ .name =  "WVGA", .width = 800,  .height = 480  },
	{ .name = "WSXGA", .width = 1680, .height = 1050 },
	{ .name = "720P",  .width = 1280, .height = 720  },
	{ .name = "1080P", .width = 1920, .height = 1080 },
	{ .name = NULL,    .width = 0,    .height = 0    }
};

const char *property_not_implemented = "Requested property not implemented";
const char *interface_not_implemented = "Requested interface not implemented";

Resolution::Resolution(uint32_t w, uint32_t h)
  : _width(w), _height(h)
{
}

Resolution::Resolution(std::string resolution)
  : _width(0), _height(0)
{
	ResolutionEntry *ent = _resolution_table;

	while (ent->name) {
		if (strcasecmp(resolution.c_str(), ent->name) == 0) {
			_width = ent->width;
			_height = ent->height;
			return;
		}
		ent++;
	}

	static std::regex re("([0-9]+)[xX]([0-9]+)");
	std::cmatch cm;
	if (std::regex_match(resolution.c_str(), cm, re)) {
		_width = std::stoul(cm[1]);
		_height = std::stoul(cm[2]);
	}
}

Resolution::operator std::string ()
{
	ResolutionEntry *ent = _resolution_table;

	while (ent->name) {
		if (ent->width == _width && ent->height == _height)
			return std::string(ent->name);
		ent++;
	}

	return std::to_string(_width) + "x" + std::to_string(_height);
}

} // namespace Media
} // namespace Ipcam
