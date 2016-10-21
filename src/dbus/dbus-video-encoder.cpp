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

#define VIDEOENCODER_INTERFACE      "ipcam.Media.VideoEncoder"
#define RATECONTROL_INTERFACE       "ipcam.Media.VideoEncoder.RateControl"
#define H264_INTERFACE              "ipcam.Media.VideoEncoder.H264"

#define DEFINE_PROP(prop, get, set) \
    _prop_handler.emplace(std::piecewise_construct, \
        std::forward_as_tuple(prop), \
        std::forward_as_tuple(get, set))

VideoEncoder::VideoEncoder
(IpcamRuntime &runtime, std::string obj_path, IVideoEncoder *encoder)
  : DBus::ObjectAdaptor(runtime.dbus_conn(), obj_path),
    _runtime(runtime), _video_encoder(encoder)
{
    assert(encoder != NULL);

    // Get handler of ipcam.Media.VideoEncoder
    DEFINE_PROP(VIDEOENCODER_INTERFACE ".Encoding",
        [](IVideoEncoder &venc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32((uint32_t)venc.getEncoding());
        },
        [](IVideoEncoder &venc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            throw DBus::ErrorFailed("Readonly property");
        });
    DEFINE_PROP(VIDEOENCODER_INTERFACE ".Resolution",
        [](IVideoEncoder &venc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_string(((std::string)venc.getResolution()).c_str());
        },
        [](IVideoEncoder &venc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            std::string s = value;
            ImageResolution r(s);
            venc.setResolution(r);
        });
    // Get handler of ipcam.Media.VideoEncoder.RateControl
    DEFINE_PROP(RATECONTROL_INTERFACE ".RateControlMode",
        [](IVideoEncoder &venc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(venc.getRcMode());
        },
        [](IVideoEncoder &venc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            venc.setRcMode((IVideoEncoder::RateCtrlMode)(uint32_t)value);
        });
    DEFINE_PROP(RATECONTROL_INTERFACE ".FrameRate",
        [](IVideoEncoder &venc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(venc.getFramerate());
        },
        [](IVideoEncoder &venc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            venc.setFramerate((uint32_t)value);
        });
    DEFINE_PROP(RATECONTROL_INTERFACE ".Bitrate",
        [](IVideoEncoder &venc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            value.writer().append_uint32(venc.getBitrate());
        },
        [](IVideoEncoder &venc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            venc.setBitrate((uint32_t)value);
        });
}

void VideoEncoder::on_get_property
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
    value.clear();

    auto iter = _prop_handler.find(interface.name() + "." + property);
    if (iter == _prop_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second.Get(*_video_encoder, interface, property, value);
}

void VideoEncoder::on_set_property
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
    auto iter = _prop_handler.find(interface.name() + "." + property);
    if (iter == _prop_handler.end())
        throw DBus::ErrorFailed("Requested interface or property not found");
    iter->second.Set(*_video_encoder, interface, property, value);
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

    // Get handler of ipcam.Media.VideoEncoder
    DEFINE_PROP(H264_INTERFACE ".H264Profile",
        [](IVideoEncoder &venc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            IH264VideoEncoder *h264venc
                = dynamic_cast<IH264VideoEncoder*>(&venc);
            value.writer().append_uint32((uint32_t)h264venc->getProfile());
        },
        [](IVideoEncoder &venc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            IH264VideoEncoder *h264venc
                = dynamic_cast<IH264VideoEncoder*>(&venc);
            h264venc->setProfile((IH264VideoEncoder::H264Profile)(uint32_t)value);
        });
    DEFINE_PROP(H264_INTERFACE ".GovLength",
        [](IVideoEncoder &venc, DBus::InterfaceAdaptor &interface,
           const std::string &property, DBus::Variant &value)
        {
            IH264VideoEncoder *h264venc
                = dynamic_cast<IH264VideoEncoder*>(&venc);
            value.writer().append_uint32((uint32_t)h264venc->getGovLength());
        },
        [](IVideoEncoder &venc, DBus::InterfaceAdaptor &interface,
           const std::string &property, const DBus::Variant &value)
        {
            IH264VideoEncoder *h264venc
                = dynamic_cast<IH264VideoEncoder*>(&venc);
            h264venc->setGovLength((uint32_t)value);
        });
}

} // namespace DBus
