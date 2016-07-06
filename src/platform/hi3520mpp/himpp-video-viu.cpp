/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-viu.cpp
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <himpp-common.h>
#include "himpp-video-viu.h"
#include "himpp-video-codec.h"

HimppViDev::HimppViDev(HimppVideoObject *source, VI_DEV vi_dev)
    : HimppVideoObject(source),
      _devid(vi_dev)
{
}

HimppViDev::~HimppViDev()
{
}

bool HimppViDev::enableObject()
{
    HI_S32 s32Ret;
    VI_DEV_ATTR_S dev_attr;

    HimppVideoCodec *vcodec = dynamic_cast<HimppVideoCodec*>(videoSource());
    if (vcodec == NULL || vcodec->viDevAttr() == NULL)
        return false;

    dev_attr = *vcodec->viDevAttr();
    uint32_t mask = 0xFF000000;
    dev_attr.au32CompMask[0] = mask >> ((_devid % 4) * 8);
    if ((s32Ret = HI_MPI_VI_SetDevAttr(_devid, &dev_attr)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VI_SetDevAttr %d failed [%#x]\n", _devid, s32Ret);
        return false;
    }

    if ((s32Ret = HI_MPI_VI_EnableDev(_devid)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VI_EnableDev %d failed [%#x]\n", _devid, s32Ret);
        return false;
    }

    return true;
}

bool HimppViDev::disableObject()
{
    HI_MPI_VI_DisableDev(_devid);

    return true;
}

bool HimppViDev::setResolution(ImageResolution &res)
{
    return false;
}

ImageResolution HimppViDev::getResolution()
{
    HimppVideoObject *obj = videoSource();
    return obj->getResolution();
}

bool HimppViDev::setFramerate(uint32_t fps)
{
    return false;
}

uint32_t HimppViDev::getFramerate()
{
    HimppVideoObject *obj = videoSource();
    return obj->getFramerate();
}

HimppViDev::operator MPP_CHN_S* ()
{
    return NULL;
}

//////////////////////////////////////////////////////////////////////////////

HimppViChan::HimppViChan(HimppViDev *vi_dev, HimppVideoObject *codec, VI_CHN vi_chn)
    : HimppVideoObject(vi_dev),
      _vi_dev(vi_dev), _codec(codec), _chnid(vi_chn)
{
    ImageResolution in_res = _vi_dev->getResolution();

    _mpp_chn.enModId = HI_ID_VIU;
    _mpp_chn.s32DevId = vi_dev->getDeviceId();
    _mpp_chn.s32ChnId = _chnid;

    _chn_attr.stCapRect.s32X = 0;
    _chn_attr.stCapRect.s32Y = 0;
    _chn_attr.stCapRect.u32Width = in_res.Width;
    _chn_attr.stCapRect.u32Height = in_res.Height;
    _chn_attr.stDestSize.u32Width = in_res.Width;
    _chn_attr.stDestSize.u32Height = in_res.Height;
    _chn_attr.enCapSel = VI_CAPSEL_BOTH;
    _chn_attr.enPixFormat = HIMPP_PIXEL_FORMAT;
    _chn_attr.bMirror = HI_FALSE;
    _chn_attr.bFlip = HI_FALSE;
    _chn_attr.bChromaResample = HI_FALSE;
    _chn_attr.s32SrcFrameRate = -1;
    _chn_attr.s32FrameRate = -1;

    _brightness = 50;
    _contrast = 50;
    _chroma = 50;
    _saturation = 50;
    _sharpness = 50;
}

HimppViChan::~HimppViChan()
{
}

uint32_t HimppViChan::getBrightness()
{
    return _brightness;
}

void HimppViChan::setBrightness(uint32_t val)
{
    _brightness = val;

    if (isEnabled()) {
        HimppVideoCodec *codec = dynamic_cast<HimppVideoCodec*>(_codec);
        if (codec) {
            codec->setBrightness(_chnid, val);
        }
    }
}

uint32_t HimppViChan::getContrast()
{
    return _contrast;
}

void HimppViChan::setContrast(uint32_t val)
{
    _contrast = val;

    if (isEnabled()) {
        HimppVideoCodec *codec = dynamic_cast<HimppVideoCodec*>(_codec);
        if (codec) {
            codec->setContrast(_chnid, val);
        }
    }
}

uint32_t HimppViChan::getChroma()
{
    return _chroma;
}

void HimppViChan::setChroma(uint32_t val)
{
    _chroma = val;

    if (isEnabled()) {
        HimppVideoCodec *codec = dynamic_cast<HimppVideoCodec*>(_codec);
        if (codec) {
            codec->setChroma(_chnid, val);
        }
    }
}

uint32_t HimppViChan::getSaturation()
{
    return _saturation;
}

void HimppViChan::setSaturation(uint32_t val)
{
    _saturation = val;

    if (isEnabled()) {
        HimppVideoCodec *codec = dynamic_cast<HimppVideoCodec*>(_codec);
        if (codec) {
            codec->setSaturation(_chnid, val);
        }
    }
}

uint32_t HimppViChan::getSharpness()
{
    return _sharpness;
}

void HimppViChan::setSharpness(uint32_t val)
{
    _sharpness = val;

    if (isEnabled()) {
        HimppVideoCodec *codec = dynamic_cast<HimppVideoCodec*>(_codec);
        if (codec) {
            codec->setSharpness(_chnid, val);
        }
    }
}

bool HimppViChan::enableObject()
{
    HI_U32 s32Ret;

    if ((s32Ret = HI_MPI_VI_SetChnAttr(_chnid, &_chn_attr)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VI_SetChnAttr %d failed [%#x]\n",
                    _chnid, s32Ret);
        return false;
    }

    if ((s32Ret = HI_MPI_VI_EnableChn(_chnid)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VI_EnableChn %d failed [%#x]\n",
                    _chnid, s32Ret);
        return false;
    }

    return true;
}

bool HimppViChan::disableObject()
{
    return (HI_MPI_VI_DisableChn(_chnid) == HI_SUCCESS);
}

bool HimppViChan::setResolution(ImageResolution &res)
{
    return false;
}

ImageResolution HimppViChan::getResolution()
{
    return ImageResolution(_chn_attr.stDestSize.u32Width, _chn_attr.stDestSize.u32Height);
}

bool HimppViChan::setFramerate(uint32_t fps)
{
    return false;
}

uint32_t HimppViChan::getFramerate()
{
    if (_chn_attr.s32FrameRate > 0)
        return _chn_attr.s32FrameRate;
    return _vi_dev->getFramerate();
}

HimppViChan::operator MPP_CHN_S* ()
{
    return &_mpp_chn;
}
