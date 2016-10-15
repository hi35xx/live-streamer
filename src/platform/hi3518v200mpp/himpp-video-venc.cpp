/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-venc.cpp
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

#include <himpp-common.h>
#include "himpp-video-venc.h"

HimppVencChan::HimppVencChan(HimppVideoObject *source, VENC_GRP grp, VENC_CHN chn)
    : HimppVideoObject(source), _chnid(chn),
      _encoding(IVideoEncoder::H264),
      _h264profile(IH264VideoEncoder::HIGH),
      _rcmode(IVideoEncoder::VBR),
      _resolution(source->getResolution()),
      _framerate(source->getFramerate()),
      _bitrate(2048),
      _gop(30)
{
    _crop_cfg.bEnable = HI_FALSE;
    _crop_cfg.stRect.u32Width = 0;
    _crop_cfg.stRect.u32Height = 0;
    _crop_cfg.stRect.s32X = 0;
    _crop_cfg.stRect.s32Y = 0;

    _mpp_chn.enModId = HI_ID_VENC;
    _mpp_chn.s32DevId = chn;
    _mpp_chn.s32ChnId = 0;
}

HimppVencChan::~HimppVencChan()
{
}

bool HimppVencChan::setEncoding(IVideoEncoder::EncodingType encoding)
{
    _encoding = encoding;

    return true;
}

IVideoEncoder::EncodingType HimppVencChan::getEncoding()
{
    return _encoding;
}

bool HimppVencChan::setH264Profile(IH264VideoEncoder::H264Profile profile)
{
    _h264profile = profile;

    if (isEnabled()) {
        if (!disableObject())
            return false;
        if (!enableObject())
            return false;
    }

    return true;
}

IH264VideoEncoder::H264Profile HimppVencChan::getProfile()
{
    return _h264profile;
}

bool HimppVencChan::setRcMode(IH264VideoEncoder::RateCtrlMode mode)
{
    _rcmode = mode;

    if (isEnabled()) {
        if (!disableObject())
            return false;
        if (!enableObject())
            return false;
    }

    return true;
}

IH264VideoEncoder::RateCtrlMode HimppVencChan::getRcMode()
{
    return _rcmode;
}

bool HimppVencChan::setBitrate(uint32_t value)
{
    _bitrate = value;

    if (isEnabled()) {
        if (!disableObject())
            return false;
        if (!enableObject())
            return false;
    }

    return true;
}

uint32_t HimppVencChan::getBitrate()
{
    return _bitrate;
}

bool HimppVencChan::setGop(uint32_t value)
{
    _gop = value;

    if (isEnabled()) {
        if (!disableObject())
            return false;
        if (!enableObject())
            return false;
    }

    return true;
}

uint32_t HimppVencChan::getGop()
{
    return _gop;
}

HimppVencChan::operator MPP_CHN_S* ()
{
    return &_mpp_chn;
}

bool HimppVencChan::setResolution(ImageResolution &res)
{
    _resolution = res;

    // Calculate the crop configuration
    ImageResolution &out = res;
    ImageResolution in = videoSource()->getResolution();
    RECT_S &rect = _crop_cfg.stRect;
    if (in.Width * out.Height > out.Width * in.Height) {
        // crop width
        _crop_cfg.bEnable = HI_TRUE;
        rect.u32Height = in.Height;
        rect.u32Width = out.Width * in.Height / out.Height;
        rect.s32X = ((in.Width - rect.u32Width) / 2) & 0xFFFFFFF0;
        rect.s32Y = 0;
    }
    else if (in.Width * out.Height < out.Width * in.Height) {
        // crop height
        _crop_cfg.bEnable = HI_TRUE;
        rect.u32Width = in.Width;
        rect.u32Height = out.Height * in.Width / out.Width;
        rect.s32X = 0;
        rect.s32Y = (in.Height - rect.u32Height) / 2;
    }
    else {
        // crop is not necessary
        _crop_cfg.bEnable = HI_FALSE;
        rect.u32Width = 0;
        rect.u32Height = 0;
        rect.s32X = 0;
        rect.s32Y = 0;
    }

    if (isEnabled()) {
        disableObject();
        enableObject();
    }

    return true;
}

ImageResolution HimppVencChan::getResolution()
{
    return _resolution;
}

bool HimppVencChan::setFramerate(uint32_t fps)
{
    _framerate = fps;

    if (isEnabled()) {
        disableObject();
        enableObject();
    }

    return true;
}

uint32_t HimppVencChan::getFramerate()
{
    return _framerate;
}

void HimppVencChan::addVideoRegion(HimppVideoRegion *region)
{
    _regions.push_back(region);
    if (isEnabled())
        region->enable();
}

void HimppVencChan::delVideoRegion(HimppVideoRegion *region)
{
    _regions.remove(region);
}

bool HimppVencChan::prepareVencChnAttr(VENC_CHN_ATTR_S &attr)
{
    HI_U32 stattime;
    stattime = _gop / _framerate;
    stattime = stattime > 0 ? stattime : 1;

    switch (_encoding) {
    case IVideoEncoder::H264:
        attr.stVeAttr.enType = PT_H264;
        attr.stVeAttr.stAttrH264e.u32MaxPicWidth = _resolution.Width;
        attr.stVeAttr.stAttrH264e.u32MaxPicHeight = _resolution.Height;
        attr.stVeAttr.stAttrH264e.u32BufSize = _resolution.Width * _resolution.Height * 2;
        attr.stVeAttr.stAttrH264e.u32PicWidth = _resolution.Width;
        attr.stVeAttr.stAttrH264e.u32PicHeight = _resolution.Height;
        attr.stVeAttr.stAttrH264e.u32Profile = _h264profile;
        attr.stVeAttr.stAttrH264e.bByFrame = HI_FALSE;
        attr.stVeAttr.stAttrH264e.u32BFrameNum = 0;
        attr.stVeAttr.stAttrH264e.u32RefNum = 1;
        // Rate Control Attribute
        switch (_rcmode) {
        case IVideoEncoder::CBR:
            attr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
            attr.stRcAttr.stAttrH264Cbr.u32Gop = _gop;
            attr.stRcAttr.stAttrH264Cbr.u32StatTime = stattime;
            attr.stRcAttr.stAttrH264Cbr.u32SrcFrmRate = videoSource()->getFramerate();
            attr.stRcAttr.stAttrH264Cbr.fr32DstFrmRate = _framerate;
            attr.stRcAttr.stAttrH264Cbr.u32BitRate = _bitrate;
            attr.stRcAttr.stAttrH264Cbr.u32FluctuateLevel = 0;
            break;
        case IVideoEncoder::VBR:
            attr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
            attr.stRcAttr.stAttrH264Vbr.u32Gop = _gop;
            attr.stRcAttr.stAttrH264Vbr.u32StatTime = stattime;
            attr.stRcAttr.stAttrH264Vbr.u32SrcFrmRate = videoSource()->getFramerate();
            attr.stRcAttr.stAttrH264Vbr.fr32DstFrmRate = _framerate;
            attr.stRcAttr.stAttrH264Vbr.u32MinQp = 24;
            attr.stRcAttr.stAttrH264Vbr.u32MaxQp = 45;
            attr.stRcAttr.stAttrH264Vbr.u32MaxBitRate = _bitrate;
            break;
        case IVideoEncoder::FIXQP:
            attr.stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
            attr.stRcAttr.stAttrH264FixQp.u32Gop = _gop;
            attr.stRcAttr.stAttrH264FixQp.u32SrcFrmRate = videoSource()->getFramerate();
            attr.stRcAttr.stAttrH264FixQp.fr32DstFrmRate = _framerate;
            attr.stRcAttr.stAttrH264FixQp.u32IQp = 20;
            attr.stRcAttr.stAttrH264FixQp.u32PQp = 23;
            break;
        default:
            HIMPP_PRINT("Unsupported RC mode[%d]\n", _rcmode);
            return false;
        }
        break;
    case IVideoEncoder::MJPEG:
        attr.stVeAttr.enType = PT_MJPEG;
        attr.stVeAttr.stAttrMjpeg.u32MaxPicWidth = _resolution.Width;
        attr.stVeAttr.stAttrMjpeg.u32MaxPicHeight = _resolution.Height;
        attr.stVeAttr.stAttrMjpeg.u32BufSize = _resolution.Width * _resolution.Height * 2;
        attr.stVeAttr.stAttrMjpeg.u32PicWidth = _resolution.Width;
        attr.stVeAttr.stAttrMjpeg.u32PicHeight = _resolution.Height;
        attr.stVeAttr.stAttrMjpeg.bByFrame = HI_TRUE;
        // Rate Control Attribute
        switch (_rcmode) {
        case IVideoEncoder::CBR:
            attr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
            attr.stRcAttr.stAttrMjpegeCbr.u32StatTime = 1;
            attr.stRcAttr.stAttrMjpegeCbr.u32SrcFrmRate = videoSource()->getFramerate();
            attr.stRcAttr.stAttrMjpegeCbr.fr32DstFrmRate = _framerate;
            attr.stRcAttr.stAttrMjpegeCbr.u32BitRate = _bitrate;
            attr.stRcAttr.stAttrMjpegeCbr.u32FluctuateLevel = 0;
            break;
        case IVideoEncoder::VBR:
            attr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGVBR;
            attr.stRcAttr.stAttrMjpegeVbr.u32StatTime = 1;
            attr.stRcAttr.stAttrMjpegeVbr.u32SrcFrmRate = videoSource()->getFramerate();
            attr.stRcAttr.stAttrMjpegeVbr.fr32DstFrmRate = _framerate;
            attr.stRcAttr.stAttrMjpegeVbr.u32MinQfactor = 50;
            attr.stRcAttr.stAttrMjpegeVbr.u32MaxQfactor = 95;
            break;
        case IVideoEncoder::FIXQP:
            attr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;
            attr.stRcAttr.stAttrMjpegeFixQp.u32SrcFrmRate = videoSource()->getFramerate();
            attr.stRcAttr.stAttrMjpegeFixQp.fr32DstFrmRate = _framerate;
            attr.stRcAttr.stAttrMjpegeFixQp.u32Qfactor = 90;
            break;
        default:
            HIMPP_PRINT("Unsupported RC mode[%d]\n", _rcmode);
            return false;
        }
        break;
    case IVideoEncoder::JPEG:
        attr.stVeAttr.enType = PT_JPEG;
        attr.stVeAttr.stAttrJpeg.u32MaxPicWidth = _resolution.Width;
        attr.stVeAttr.stAttrJpeg.u32MaxPicHeight = _resolution.Height;
        attr.stVeAttr.stAttrJpeg.u32BufSize = _resolution.Width * _resolution.Height * 2;
        attr.stVeAttr.stAttrJpeg.u32PicWidth = _resolution.Width;
        attr.stVeAttr.stAttrJpeg.u32PicHeight = _resolution.Height;
        attr.stVeAttr.stAttrJpeg.bByFrame = HI_TRUE;
        attr.stVeAttr.stAttrJpeg.bSupportDCF = HI_FALSE;
        break;
    default:
        HIMPP_PRINT("Unsupported encoding code[%d]\n", _encoding);
        return false;
    }
    return true;
}

bool HimppVencChan::enableObject()
{
    HI_S32 s32Ret;
    VENC_CROP_CFG_S dis_crop = { .bEnable = HI_FALSE };

    VENC_CHN_ATTR_S attr;
    prepareVencChnAttr(attr);
    if ((s32Ret = HI_MPI_VENC_CreateChn(_chnid, &attr)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VENC_CreateChn %d faild [%#x]\n",
                    _chnid, s32Ret);
        return false;
    }

    if ((s32Ret = HI_MPI_VENC_SetMaxStreamCnt(_chnid, 4)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VENC_SetMaxStreamCnt %d failed [%#x]\n",
                    _chnid, s32Ret);
    }

    VENC_PARAM_H264_VUI_S stVui;
    if ((s32Ret = HI_MPI_VENC_GetH264Vui(_chnid, &stVui)) == HI_SUCCESS) {
        stVui.stVuiTimeInfo.timing_info_present_flag = 1;
        stVui.stVuiTimeInfo.num_units_in_tick = 1;
        stVui.stVuiTimeInfo.time_scale = _framerate * 2;
        if ((s32Ret = HI_MPI_VENC_SetH264Vui(_chnid, &stVui)) != HI_SUCCESS) {
            HIMPP_PRINT("HI_MPI_VENC_SetH264Vui(%d) failed [%#x]\n",
                        _chnid, s32Ret);
        }
    }

    if ((s32Ret = HI_MPI_VENC_SetCrop(_chnid, &_crop_cfg)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VENC_SetCrop [%d] faild [%#x]!\n",
                     _chnid, s32Ret);
        goto err_destroy_chn;
    }

    if ((s32Ret = HI_MPI_VENC_StartRecvPic(_chnid)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VENC_StartRecvPic %d failed [%#x]\n",
                    _chnid, s32Ret);
        goto err_disable_crop;
    }

    MPP_CHN_S dst_chn;
    dst_chn.enModId = HI_ID_VENC;
    dst_chn.s32DevId = 0;
    dst_chn.s32ChnId = _chnid;
    if ((s32Ret = HI_MPI_SYS_Bind(*source(), &dst_chn)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_SYS_Bind %d failed [%#x]\n",
                    _chnid, s32Ret);
        goto err_stop_recv_pic;
    }

    for (auto it : _regions) {
        it->enable();
    }

    return true;

err_stop_recv_pic:
    HI_MPI_VENC_StopRecvPic(_chnid);
err_disable_crop:
    HI_MPI_VENC_SetCrop(_chnid, &dis_crop);
err_destroy_chn:
    HI_MPI_VENC_DestroyChn(_chnid);

    return false;
}

bool HimppVencChan::disableObject()
{
    HI_S32 s32Ret;
    VENC_CROP_CFG_S dis_crop = { .bEnable = HI_FALSE };

    for (auto it : _regions) {
        it->disable();
    }

    MPP_CHN_S dst_chn = {
        .enModId = HI_ID_VENC,
        .s32DevId = 0,
        .s32ChnId = _chnid
    };
    if ((s32Ret = HI_MPI_SYS_UnBind(*source(), &dst_chn)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_SYS_UnBind %d failed [%#x]\n",
                    _chnid, s32Ret);
    }

    if ((s32Ret = HI_MPI_VENC_StopRecvPic(_chnid)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VENC_StopRecvPic %d failed [%#x]\n",
                    _chnid, s32Ret);
    }

    if ((s32Ret = HI_MPI_VENC_SetCrop(_chnid, &dis_crop)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VENC_SetCrop [%d] failed [%#x]!\n",
                     _chnid, s32Ret);
    }

    if ((s32Ret = HI_MPI_VENC_DestroyChn(_chnid)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VENC_DestroyChn %d failed [%#x]\n",
                    _chnid, s32Ret);
    }

    return true;
}
