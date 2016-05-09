/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-venc.h
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

#ifndef _HIMPP_VIDEO_VENC_H_
#define _HIMPP_VIDEO_VENC_H_

#include <list>
#include <himpp-object.h>
#include <mpi_venc.h>

#include <ipcam-media-iface.h>
#include "himpp-video-region.h"

using namespace Ipcam::Media;

class HimppVencChan: public HimppVideoObject 
{
public:
    HimppVencChan(HimppVideoObject *, VENC_GRP, VENC_CHN);
    ~HimppVencChan();

    // virtual functions
    bool setEncoding(IVideoEncoder::EncodingType);
    IVideoEncoder::EncodingType getEncoding();
    bool setH264Profile(IH264VideoEncoder::H264Profile);
    IH264VideoEncoder::H264Profile getProfile();
    bool setRcMode(IVideoEncoder::RateCtrlMode);
    IVideoEncoder::RateCtrlMode getRcMode();
    bool setBitrate(uint32_t);
    uint32_t getBitrate();
    bool setGop(uint32_t);
    uint32_t getGop();

    operator MPP_CHN_S* ();
    bool setResolution(ImageResolution &res);
    ImageResolution getResolution();
    bool setFramerate(uint32_t fps);
    uint32_t getFramerate();

    void addVideoRegion(HimppVideoRegion *region);
    void delVideoRegion(HimppVideoRegion *region);

    // local functions
    VENC_GRP groupId() { return _grpid; }
    VENC_CHN channelId() { return _chnid; }
protected:
    bool enableObject();
    bool disableObject();
private:
    MPP_CHN_S                   _mpp_chn;
    VENC_GRP                    _grpid;
    VENC_CHN                    _chnid;
    IVideoEncoder::EncodingType _encoding;
    IH264VideoEncoder::H264Profile _h264profile;
    ImageResolution             _resolution;
    uint32_t                    _framerate;
    IVideoEncoder::RateCtrlMode _rcmode;
    uint32_t                    _bitrate;
    uint32_t                    _gop;
    GROUP_CROP_CFG_S            _crop_cfg;

    std::list<HimppVideoRegion*> _regions;

    bool prepareVencChnAttr(VENC_CHN_ATTR_S &attr);
};

#endif // _HIMPP_VIDEO_VENC_H_

