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

IpcamVideoSource::IpcamVideoSource
(DBus::Connection &connection, std::string obj_path, IVideoSource *video_source)
  : DBus::ObjectAdaptor(connection, obj_path),
    _video_source(video_source)
{
    assert(video_source != NULL);
}

void IpcamVideoSource::on_get_property
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
    value.clear();
    DBus::MessageIter mi = value.writer();

    if (interface.name() == "ipcam.Media.VideoSource") {
        if (property == "Resolution") {
            std::string s = (std::string)_video_source->getResolution();
            mi.append_string(s.c_str());
        }
        else if (property == "Framerate") {
            mi.append_uint32(_video_source->getFramerate());
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.Imaging") {
        IVideoSource::Imaging *imaging = _video_source->getImaging();
        if (!imaging) throw DBus::ErrorFailed("requested interface not support");

        if (property == "Brightness") {
            mi.append_uint32(imaging->getBrightness());
        }
        else if (property == "Contrast") {
            mi.append_uint32(imaging->getContrast());
        }
        else if (property == "Chroma") {
            mi.append_uint32(imaging->getChroma());
        }
        else if (property == "Saturation") {
            mi.append_uint32(imaging->getSaturation());
        }
        else if (property == "Sharpness") {
            mi.append_uint32(imaging->getSharpness());
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.Imaging.Backlight") {
        IVideoSource::Imaging *imaging = _video_source->getImaging();
        if (!imaging) throw DBus::ErrorFailed("requested interface not support");
        IVideoSource::Imaging::Backlight *blc = imaging->getBacklight();
        if (!blc) throw DBus::ErrorFailed("requested interface not support");

        if (property == "Mode") {
            mi.append_uint32((uint32_t)blc->getMode());
        }
        else if (property == "Level") {
            mi.append_uint32(blc->getLevel());
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.Imaging.Focus") {
        IVideoSource::Imaging *imaging = _video_source->getImaging();
        if (!imaging) throw DBus::ErrorFailed("requested interface not support");
        IVideoSource::Imaging::Focus *focus = imaging->getFocus();
        if (!focus) throw DBus::ErrorFailed("requested interface not support");

        if (property == "AutoFocusMode") {
            mi.append_uint32((uint32_t)focus->getAutoFocusMode());
        }
        else if (property == "DefaultSpeed") {
            mi.append_uint32(focus->getDefaultSpeed());
        }
        else if (property == "NearLimit") {
            mi.append_uint32(focus->getNearLimit());
        }
        else if (property == "FarLimit") {
            mi.append_uint32(focus->getFarLimit());
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.Imaging.Exposure") {
        IVideoSource::Imaging *imaging = _video_source->getImaging();
        if (!imaging) throw DBus::ErrorFailed("requested interface not support");
        IVideoSource::Imaging::Exposure *exposure = imaging->getExposure();
        if (!exposure) throw DBus::ErrorFailed("requested interface not support");

        if (property == "Mode") {
            mi.append_uint32((uint32_t)exposure->getMode());
        }
        else if (property == "Priority") {
            mi.append_uint32(exposure->getPriority());
        }
        else if (property == "MinExposureTime") {
            mi.append_uint32(exposure->getMinExposureTime());
        }
        else if (property == "MaxExposureTime") {
            mi.append_uint32(exposure->getMaxExposureTime());
        }
        else if (property == "MinGain") {
            mi.append_uint32(exposure->getMinGain());
        }
        else if (property == "MaxGain") {
            mi.append_uint32(exposure->getMaxGain());
        }
        else if (property == "ExposureTime") {
            mi.append_uint32(exposure->getExposureTime());
        }
        else if (property == "Gain") {
            mi.append_uint32(exposure->getGain());
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.Imaging.WhiteBalance") {
        IVideoSource::Imaging *imaging = _video_source->getImaging();
        if (!imaging) throw DBus::ErrorFailed("requested interface not support");
        IVideoSource::Imaging::WhiteBalance *wb = imaging->getWhiteBalance();
        if (!wb) throw DBus::ErrorFailed("requested interface not support");

        if (property == "Mode") {
            mi.append_uint32((uint32_t)wb->getMode());
        }
        else if (property == "CrGain") {
            mi.append_uint32(wb->getCrGain());
        }
        else if (property == "CbGain") {
            mi.append_uint32(wb->getCbGain());
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.Imaging.WideDynamicRange") {
        IVideoSource::Imaging *imaging = _video_source->getImaging();
        if (!imaging) throw DBus::ErrorFailed("requested interface not support");
        IVideoSource::Imaging::WideDynamicRange *wdr = imaging->getWideDynamicRange();
        if (!wdr) throw DBus::ErrorFailed("requested interface not support");

        if (property == "Mode") {
            mi.append_uint32((uint32_t)wdr->getMode());
        }
        else if (property == "Level") {
            mi.append_uint32(wdr->getLevel());
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.LDC") {
        IVideoSource::Imaging *imaging = _video_source->getImaging();
        if (!imaging) throw DBus::ErrorFailed("requested interface not support");
        IVideoSource::Imaging::LDC *ldc = imaging->getLDC();
        if (!ldc) throw DBus::ErrorFailed("requested interface not support");

        if (property == "Mode") {
            mi.append_uint32(ldc->getMode());
        }
        else if (property == "Ratio") {
            mi.append_uint32(ldc->getRatio());
        }
    }
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
            _video_source->setFramerate((uint32_t)value);
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.Imaging") {
        IVideoSource::Imaging *imaging = _video_source->getImaging();
        if (!imaging) throw DBus::ErrorFailed("requested interface not support");

        if (property == "Brightness") {
            imaging->setBrightness((uint32_t)value);
        }
        else if (property == "Contrast") {
            imaging->setContrast((uint32_t)value);
        }
        else if (property == "Chroma") {
            imaging->setChroma((uint32_t)value);
        }
        else if (property == "Saturation") {
            imaging->setSaturation((uint32_t)value);
        }
        else if (property == "Sharpness") {
            imaging->setSharpness((uint32_t)value);
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.Imaging.Backlight") {
        IVideoSource::Imaging *imaging = _video_source->getImaging();
        if (!imaging) throw DBus::ErrorFailed("requested interface not support");
        IVideoSource::Imaging::Backlight *blc = imaging->getBacklight();
        if (!blc) throw DBus::ErrorFailed("requested interface not support");

        if (property == "Mode") {
            blc->setMode((IVideoSource::Imaging::Backlight::BacklightMode)(uint32_t)value);
        }
        else if (property == "Level") {
            blc->setLevel((uint32_t)value);
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.Imaging.Focus") {
        IVideoSource::Imaging *imaging = _video_source->getImaging();
        if (!imaging) throw DBus::ErrorFailed("requested interface not support");
        IVideoSource::Imaging::Focus *focus = imaging->getFocus();
        if (!focus) throw DBus::ErrorFailed("requested interface not support");

        if (property == "AutoFocusMode") {
            focus->setAutoFocusMode((IVideoSource::Imaging::Focus::AutoFocusMode)(uint32_t)value);
        }
        else if (property == "DefaultSpeed") {
            focus->setDefaultSpeed((uint32_t)value);
        }
        else if (property == "NearLimit") {
            focus->setNearLimit((uint32_t)value);
        }
        else if (property == "FarLimit") {
            focus->setFarLimit((uint32_t)value);
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.Imaging.Exposure") {
        IVideoSource::Imaging *imaging = _video_source->getImaging();
        if (!imaging) throw DBus::ErrorFailed("requested interface not support");
        IVideoSource::Imaging::Exposure *exposure = imaging->getExposure();
        if (!exposure) throw DBus::ErrorFailed("requested interface not support");

        if (property == "Mode") {
            exposure->setMode((IVideoSource::Imaging::Exposure::ExposureMode)(uint32_t)value);
        }
        else if (property == "Priority") {
            exposure->setPriority((IVideoSource::Imaging::Exposure::ExposurePriority)(uint32_t)value);
        }
        else if (property == "MinExposureTime") {
            exposure->setMinExposureTime((uint32_t)value);
        }
        else if (property == "MaxExposureTime") {
            exposure->setMaxExposureTime((uint32_t)value);
        }
        else if (property == "MinGain") {
            exposure->setMinGain((uint32_t)value);
        }
        else if (property == "MaxGain") {
            exposure->setMaxGain((uint32_t)value);
        }
        else if (property == "ExposureTime") {
            exposure->setExposureTime((uint32_t)value);
        }
        else if (property == "Gain") {
            exposure->setGain((uint32_t)value);
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.Imaging.WhiteBalance") {
        IVideoSource::Imaging *imaging = _video_source->getImaging();
        if (!imaging) throw DBus::ErrorFailed("requested interface not support");
        IVideoSource::Imaging::WhiteBalance *wb = imaging->getWhiteBalance();
        if (!wb) throw DBus::ErrorFailed("requested interface not support");

        if (property == "Mode") {
            wb->setMode((IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode)(uint32_t)value);
        }
        else if (property == "CrGain") {
            wb->setCrGain((uint32_t)value);
        }
        else if (property == "CbGain") {
            wb->setCbGain((uint32_t)value);
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.Imaging.WideDynamicRange") {
        IVideoSource::Imaging *imaging = _video_source->getImaging();
        if (!imaging) throw DBus::ErrorFailed("requested interface not support");
        IVideoSource::Imaging::WideDynamicRange *wdr = imaging->getWideDynamicRange();
        if (!wdr) throw DBus::ErrorFailed("requested interface not support");

        if (property == "Mode") {
            wdr->setMode((IVideoSource::Imaging::WideDynamicRange::WDRMode)(uint32_t)value);
        }
        else if (property == "Level") {
            wdr->setLevel((uint32_t)value);
        }
    }
    else if (interface.name() == "ipcam.Media.VideoSource.LDC") {
        IVideoSource::Imaging *imaging = _video_source->getImaging();
        if (!imaging) throw DBus::ErrorFailed("requested interface not support");
        IVideoSource::Imaging::LDC *ldc = imaging->getLDC();
        if (!ldc) throw DBus::ErrorFailed("requested interface not support");

        if (property == "Mode") {
            ldc->setMode((uint32_t)value);
        }
        else if (property == "Ratio") {
            ldc->setRatio((uint32_t)value);
        }
    }
}
