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
(DBus::Connection &connection, std::string obj_path, Ipcam::Media::VideoEncoder *encoder)
  : DBus::ObjectAdaptor(connection, obj_path), _video_encoder(encoder)
{
    assert(encoder != NULL);

    Encoding = (uint32_t)encoder->getEncoding();
    Resolution = encoder->getResolution();
    RateControlMode = (uint32_t)encoder->getRcMode();
    FrameRate = encoder->getFramerate();
    EncodingInterval = 0;
    Bitrate = encoder->getBitrate();
}

void IpcamVideoEncoder::on_set_property
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
    if (interface.name() == "ipcam.Media.VideoEncoder") {
        if (property == "Resolution") {
            std::string s = value;
            Ipcam::Media::ImageResolution res(s);
            _video_encoder->setResolution(res);
        }
    }
    else if (interface.name() == "ipcam.Media.VideoEncoder.RateControl") {
        if (property == "RateControlMode") {
            _video_encoder->setRcMode((Ipcam::Media::RC_MODE)(uint32_t)value);
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
(DBus::Connection &connection, std::string obj_path, Ipcam::Media::H264VideoEncoder *encoder)
  : IpcamVideoEncoder(connection, obj_path, dynamic_cast<Ipcam::Media::VideoEncoder*>(encoder))
{
    assert(encoder != NULL);

    H264Profile = encoder->getProfile();
    GovLength = encoder->getGovLength();
}

void IpcamH264VideoEncoder::on_set_property
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
    if (interface.name() == "ipcam.Media.VideoEncoder.H264") {
        Ipcam::Media::H264VideoEncoder *h264encoder
            = dynamic_cast<Ipcam::Media::H264VideoEncoder*>(_video_encoder);
        if (property == "H264Profile") {
            h264encoder->setProfile((Ipcam::Media::H264_PROFILE)(uint32_t)value);
        }
        else if (property == "GovLength") {
            h264encoder->setGovLength(value);
        }
    }
    else {
        IpcamVideoEncoder::on_set_property(interface, property, value);
    }
}
