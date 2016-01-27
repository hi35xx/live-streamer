/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ipcam-media-iface.cpp
 * Copyright (C) 2015 Watson Xu <watson@localhost.localdomain>
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

#include <stdio.h>
#include <stdint.h>
#include <sstream>
#include <map>
#include <regex>

#include <ipcam-media-iface.h>

namespace Ipcam
{
namespace Interface
{

struct ImageResolutionEntry
{
    const char *name;
    uint16_t width;
    uint16_t height;
};

static ImageResolutionEntry image_resolution_table[] = {
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
    { .name =  "WVGA", .width = 854,  .height = 480  },
    { .name = "WSXGA", .width = 1680, .height = 1050 },
    { .name = "720P",  .width = 1280, .height = 720  },
    { .name = "1080P", .width = 1920, .height = 1080 },
    { .name = NULL,    .width = 0,    .height = 0    }
};

class ImageResolutionHelper
{
public:
    ImageResolutionHelper();
    ~ImageResolutionHelper();
    ImageResolutionEntry *FindFromString(const std::string &);
    ImageResolutionEntry *FindFromResolution(const ImageResolution &);
private:
    typedef std::map<std::string, ImageResolutionEntry*> StrToResMap;
    typedef std::map<uint32_t, ImageResolutionEntry*> ResToStrMap;
    StrToResMap str2res_map;
    ResToStrMap res2str_map;
};

ImageResolutionHelper::ImageResolutionHelper()
{
    ImageResolutionEntry *entry = image_resolution_table;

    while(entry->name != NULL) {
        std::string skey = entry->name;
        uint32_t ikey = (uint32_t)entry->width << 16 | entry->height;
        str2res_map[skey] = entry;
        res2str_map[ikey] = entry;
        entry++;
    }
}

ImageResolutionHelper::~ImageResolutionHelper()
{
    str2res_map.clear();
    res2str_map.clear();
}

ImageResolutionEntry *ImageResolutionHelper::FindFromString(const std::string &s)
{
    return str2res_map[s];
}

ImageResolutionEntry *ImageResolutionHelper::FindFromResolution(const ImageResolution &resolution)
{
    uint32_t key = (uint32_t)resolution.Width << 16 | resolution.Height;
    return res2str_map[key];
}


static ImageResolutionHelper image_resolution_helper;


ImageResolution::ImageResolution(uint32_t w, uint32_t h)
  : Width(w), Height(h)
{
}

ImageResolution::ImageResolution(std::string resolution)
  : Width(0), Height(0)
{
    auto entry = image_resolution_helper.FindFromString(resolution);
    if (entry) {
        Width = entry->width;
        Height = entry->height;
        return;
    }

    static std::regex re("([0-9]+)x([0-9]+)");
    std::cmatch cm;
    if (std::regex_match(resolution.c_str(), cm, re)) {
        Width = std::stoul(cm[1]);
        Height = std::stoul(cm[2]);
    }
}

ImageResolution::operator std::string()
{
    auto entry = image_resolution_helper.FindFromResolution(*this);
    if (entry) {
        return entry->name;
    }
    return std::to_string(Width) + "x" + std::to_string(Height);
}


} // namespace Interface
} // namespace Ipcam
