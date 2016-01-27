/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ipcam-video-encoder.cpp
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

#include "ipcam-video-encoder.h"


IpcamVideoEncoder::IpcamVideoEncoder
(DBus::Connection &connection, std::string obj_path, IVideoEncoder *encoder)
  : DBus::ObjectAdaptor(connection, obj_path), _video_encoder(encoder)
{
    assert(encoder != NULL);
}

void IpcamVideoEncoder::on_get_property
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
    value.clear();
    DBus::MessageIter mi = value.writer();

    if (interface.name() == "ipcam.Media.VideoEncoder") {
        if (property == "Resolution") {
            std::string s = (std::string)_video_encoder->getResolution();
            mi.append_string(s.c_str());
        }
    }
    else if (interface.name() == "ipcam.Media.VideoEncoder.RateControl") {
        if (property == "RateControlMode") {
            mi.append_uint32((uint32_t)_video_encoder->getRcMode());
        }
        else if (property == "FrameRate") {
            mi.append_uint32(_video_encoder->getFramerate());
        }
        else if (property == "Bitrate") {
            mi.append_uint32(_video_encoder->getBitrate());
        }
    }
}

void IpcamVideoEncoder::on_set_property
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
    if (interface.name() == "ipcam.Media.VideoEncoder") {
        if (property == "Resolution") {
            std::string s = value;
            ImageResolution res(s);
            _video_encoder->setResolution(res);
        }
    }
    else if (interface.name() == "ipcam.Media.VideoEncoder.RateControl") {
        if (property == "RateControlMode") {
            _video_encoder->setRcMode((IVideoEncoder::RateCtrlMode)(uint32_t)value);
        }
        else if (property == "FrameRate") {
            _video_encoder->setFramerate(value);
        }
        else if (property == "Bitrate") {
            _video_encoder->setBitrate(value);
        }
    }
}

IpcamH264VideoEncoder::IpcamH264VideoEncoder
(DBus::Connection &connection, std::string obj_path, IH264VideoEncoder *encoder)
  : IpcamVideoEncoder(connection, obj_path, dynamic_cast<IVideoEncoder*>(encoder))
{
    assert(encoder != NULL);
}

void IpcamH264VideoEncoder::on_get_property
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
    value.clear();
    DBus::MessageIter mi = value.writer();

    if (interface.name() == "ipcam.Media.VideoEncoder.H264") {
        IH264VideoEncoder *h264encoder
            = dynamic_cast<IH264VideoEncoder*>(_video_encoder);
        if (property == "H264Profile") {
            mi.append_uint32((uint32_t)h264encoder->getProfile());
        }
        else if (property == "GovLength") {
            mi.append_uint32((uint32_t)h264encoder->getGovLength());
        }
    }
    else {
        IpcamVideoEncoder::on_get_property(interface, property, value);
    }
}

void IpcamH264VideoEncoder::on_set_property
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
    if (interface.name() == "ipcam.Media.VideoEncoder.H264") {
        IH264VideoEncoder *h264encoder
            = dynamic_cast<IH264VideoEncoder*>(_video_encoder);
        if (property == "H264Profile") {
            h264encoder->setProfile((IH264VideoEncoder::H264Profile)(uint32_t)value);
        }
        else if (property == "GovLength") {
            h264encoder->setGovLength(value);
        }
    }
    else {
        IpcamVideoEncoder::on_set_property(interface, property, value);
    }
}
