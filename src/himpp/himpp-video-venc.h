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

#include <himpp-object.h>
#include <mpi_venc.h>

#include <ipcam-media.h>

using namespace Ipcam::Media;

class HimppVencChan: public HimppVideoObject 
{
public:
    //enum ENCODING { H264, MJPEG, JPEG, MPEG4 };
    //enum RC_MODE { CBR, VBR, FIXQP };
    //enum H264PROFILE { BASELINE = 0, MAIN = 1, HIGH = 2, SVC_T = 3 };

    HimppVencChan(HimppVideoObject *, VENC_GRP, VENC_CHN);
    ~HimppVencChan();

    // virtual functions
    bool setEncoding(VIDEO_ENCODING);
    VIDEO_ENCODING getEncoding();
    bool setH264Profile(H264_PROFILE);
    H264_PROFILE getProfile();
    bool setRcMode(RC_MODE);
    RC_MODE getRcMode();
    bool setBitrate(uint32_t);
    uint32_t getBitrate();
    bool setGop(uint32_t);
    uint32_t getGop();

    operator MPP_CHN_S* ();
    bool setResolution(ImageResolution &res);
    ImageResolution getResolution();
    bool setFramerate(uint32_t fps);
    uint32_t getFramerate();

    // local functions
    VENC_GRP groupId() { return _grpid; }
    VENC_CHN channelId() { return _chnid; }
protected:
    bool enableObject();
    bool disableObject();
private:
    VENC_GRP            _grpid;
    VENC_CHN            _chnid;
    VIDEO_ENCODING      _encoding;
    H264_PROFILE         _h264profile;
    ImageResolution     _resolution;
    uint32_t            _framerate;
    RC_MODE             _rcmode;
    uint32_t            _bitrate;
    uint32_t            _gop;
    GROUP_CROP_CFG_S    _crop_cfg;

    bool prepareVencChnAttr(VENC_CHN_ATTR_S &attr);
};

#endif // _HIMPP_VIDEO_VENC_H_

