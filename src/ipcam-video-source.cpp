/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ipcam-video-source.cpp
 * Copyright (C) 2015 Watson Xu <xuhuashan@gmail.com>
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

#include <ipcam-media.h>
#include "ipcam-video-source.h"

using namespace Ipcam::Media;

IpcamVideoSource::IpcamVideoSource
(DBus::Connection &connection, std::string obj_path, Ipcam::Media::VideoSource *video_source)
  : DBus::ObjectAdaptor(connection, obj_path),
    _video_source(video_source)
{
    assert(video_source != NULL);

    Framerate = 30;
    Resolution = "1080P";
    Brightness = 0;
    Saturation = 0;
    Contrast = 0;
    Sharpness = 0;
}

void IpcamVideoSource::on_set_property
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
    if (interface.name() == "ipcam.Media.VideoSource") {
        if (property == "Resolution") {
            std::string s = value;
            ImageResolution r(s);
            _video_source->setResolution(r);
        }
        else if (property == "Framerate") {
            _video_source->setFramerate((int)value);
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.Imaging") {
        if (property == "Brightness") {
            _video_source->setBrightness((int)value);
        }
        else if (property == "Contrast") {
            _video_source->setContrast((int)value);
        }
        else if (property == "Chroma") {
            _video_source->setChroma((int)value);
        }
        else if (property == "Saturation") {
            _video_source->setSaturation((int)value);
        }
        else if (property == "Sharpness") {
            _video_source->setSharpness((int)value);
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.LDC") {
        if (property == "Enable") {
            //_video_source->setLDCEnable((bool)value);
        }
        else if (property == "Mode") {
            //_video_source->setLDCMode((uint32_t)value);
        }
        else if (property == "CenterXOffset") {
            //_video_source->setLDCCenterXOffset((int32_t)value);
        }
        else if (property == "CenterYOffset") {
            //_video_source->setLDCCenterYOffset((uint32_t)value);
        }
        else if (property == "Ratio") {
            //_video_source->setLDCRatio((uint32_t)value);
        }
    }
}
