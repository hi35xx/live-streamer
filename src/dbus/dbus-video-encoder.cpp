/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * dbus-video-encoder.cpp
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

#include <string>
#include <ipcam-runtime.h>
#include "dbus-video-encoder.h"

namespace DBus {

VideoEncoder::VideoEncoder
(IpcamRuntime &runtime, std::string obj_path, IVideoEncoder *encoder)
  : DBus::ObjectAdaptor(runtime.dbus_conn(), obj_path),
    _runtime(runtime), _video_encoder(encoder)
{
    assert(encoder != NULL);
}

void VideoEncoder::on_get_property
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

void VideoEncoder::on_set_property
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

DBus::Path VideoEncoder::CreateOSD(const uint32_t& index)
{
    std::map<uint32_t, VideoOSD>::iterator it = _osds.find(index);
    if (it != _osds.end())
        throw IpcamError("OSD already exists");

    std::string obj_path = path() + "/OSD/" + std::to_string(index);
    IVideoOSD *video_osd = _video_encoder->CreateOSD(index);
    if (!video_osd)
        throw IpcamError("Create OSD instance failed");

    _osds.emplace(std::piecewise_construct,
                  std::forward_as_tuple(index),
                  std::forward_as_tuple(_runtime, obj_path, video_osd));

    return obj_path;
}

void VideoEncoder::DeleteOSD(const uint32_t& index)
{
    _osds.erase(index);
}

std::map< uint32_t, ::DBus::Path > VideoEncoder::GetOSDs()
{
    std::map<uint32_t, DBus::Path> result;
    for (auto it = _osds.begin(); it != _osds.end(); it++) {
        uint32_t index = it->first;
        DBus::Path venc_path = path();
        std::string obj_path = path() + "/" + std::to_string(index);
        result.emplace(index, obj_path);
    }
    return result;
}

H264VideoEncoder::H264VideoEncoder
(IpcamRuntime &runtime, std::string obj_path, IH264VideoEncoder *encoder)
  : VideoEncoder(runtime, obj_path, dynamic_cast<IVideoEncoder*>(encoder))
{
    assert(encoder != NULL);
}

void H264VideoEncoder::on_get_property
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
        VideoEncoder::on_get_property(interface, property, value);
    }
}

void H264VideoEncoder::on_set_property
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
        VideoEncoder::on_set_property(interface, property, value);
    }
}

} // namespace DBus
