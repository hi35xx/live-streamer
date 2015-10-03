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

#include <himpp-common.h>
#include "himpp-video-viu.h"


HimppViDev::HimppViDev(HimppVideoSensor *sensor, VI_DEV vi_dev)
    : HimppVideoObject(NULL), _video_sensor(sensor),
      _devid(vi_dev), _video_isp(NULL)
{

    _brightness = 50;
    _contrast = 50;
    _chroma = 50;
    _saturation = 50;
}

HimppViDev::~HimppViDev()
{
}

bool HimppViDev::setBrightness(int32_t val)
{
    _brightness = val;

    if (isEnabled()) {
        HI_S32 s32Ret;
        VI_CSC_ATTR_S csc_attr;
        csc_attr.enViCscType = VI_CSC_TYPE_709;
        csc_attr.u32LumaVal = _brightness;
        csc_attr.u32ContrVal = _contrast;
        csc_attr.u32HueVal = _chroma;
        csc_attr.u32SatuVal = _saturation;
        if ((s32Ret = HI_MPI_VI_SetCSCAttr(_devid, &csc_attr)) != HI_SUCCESS) {
            HIMPP_PRINT("HI_MPI_VI_SetCSCAttr %d failed [%#x]\n", _devid, s32Ret);
            return false;
        }
    }

    return true;
}

bool HimppViDev::setContrast(int32_t val)
{
    _contrast = val;

    if (isEnabled()) {
        HI_S32 s32Ret;
        VI_CSC_ATTR_S csc_attr;
        csc_attr.enViCscType = VI_CSC_TYPE_709;
        csc_attr.u32LumaVal = _brightness;
        csc_attr.u32ContrVal = _contrast;
        csc_attr.u32HueVal = _chroma;
        csc_attr.u32SatuVal = _saturation;
        if ((s32Ret = HI_MPI_VI_SetCSCAttr(_devid, &csc_attr)) != HI_SUCCESS) {
            HIMPP_PRINT("HI_MPI_VI_SetCSCAttr %d failed [%#x]\n", _devid, s32Ret);
            return false;
        }
    }

    return true;
}

bool HimppViDev::setChroma(int32_t val)
{
    _chroma = val;

    if (isEnabled()) {
        HI_S32 s32Ret;
        VI_CSC_ATTR_S csc_attr;
        csc_attr.enViCscType = VI_CSC_TYPE_709;
        csc_attr.u32LumaVal = _brightness;
        csc_attr.u32ContrVal = _contrast;
        csc_attr.u32HueVal = _chroma;
        csc_attr.u32SatuVal = _saturation;
        if ((s32Ret = HI_MPI_VI_SetCSCAttr(_devid, &csc_attr)) != HI_SUCCESS) {
            HIMPP_PRINT("HI_MPI_VI_SetCSCAttr %d failed [%#x]\n", _devid, s32Ret);
            return false;
        }
    }

    return true;
}

bool HimppViDev::setSaturation(int32_t val)
{
    _saturation = val;

    if (isEnabled()) {
        HI_S32 s32Ret;
        VI_CSC_ATTR_S csc_attr;
        csc_attr.enViCscType = VI_CSC_TYPE_709;
        csc_attr.u32LumaVal = _brightness;
        csc_attr.u32ContrVal = _contrast;
        csc_attr.u32HueVal = _chroma;
        csc_attr.u32SatuVal = _saturation;
        if ((s32Ret = HI_MPI_VI_SetCSCAttr(_devid, &csc_attr)) != HI_SUCCESS) {
            HIMPP_PRINT("HI_MPI_VI_SetCSCAttr %d failed [%#x]\n", _devid, s32Ret);
            return false;
        }
    }

    return true;
}

bool HimppViDev::enableObject()
{
    HI_S32 s32Ret;
    VI_DEV_ATTR_S *dev_attr;

    _video_isp = new HimppVideoISP(_video_sensor);
    if (!_video_isp)
        return false;

    if (!_video_isp->enable())
        goto err_free_isp;

    dev_attr = *_video_sensor;
    if ((s32Ret = HI_MPI_VI_SetDevAttr(_devid, dev_attr)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VI_SetDevAttr %d failed [%#x]\n", _devid, s32Ret);
        goto err_disable_isp;
    }

    if ((s32Ret = HI_MPI_VI_EnableDev(_devid)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VI_EnableDev %d failed [%#x]\n", _devid, s32Ret);
        goto err_disable_isp;
    }

    VI_CSC_ATTR_S csc_attr;
    csc_attr.enViCscType = VI_CSC_TYPE_709;
    csc_attr.u32LumaVal = _brightness;
    csc_attr.u32ContrVal = _contrast;
    csc_attr.u32HueVal = _chroma;
    csc_attr.u32SatuVal = _saturation;
    if ((s32Ret = HI_MPI_VI_SetCSCAttr(_devid, &csc_attr)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VI_SetCSCAttr %d failed [%#x]\n", _devid, s32Ret);
    }

    return true;

err_disable_isp:
    _video_isp->disable();

err_free_isp:
    delete _video_isp;
    _video_isp = NULL;

    return false;
}

bool HimppViDev::disableObject()
{
    HI_MPI_VI_DisableDev(_devid);
    _video_isp->disable();
    delete _video_isp;
    _video_isp = NULL;

    return true;
}

bool HimppViDev::setResolution(ImageResolution &res)
{
    return false;
}

ImageResolution HimppViDev::getResolution()
{
    ISP_IMAGE_ATTR_S *attr = *_video_sensor;
    return ImageResolution(attr->u16Width, attr->u16Height);
}

bool HimppViDev::setFramerate(uint32_t fps)
{
    return false;
}

uint32_t HimppViDev::getFramerate()
{
    ISP_IMAGE_ATTR_S *attr = *_video_sensor;
    return attr->u16FrameRate;
}

HimppViDev::operator MPP_CHN_S* ()
{
    return NULL;
}

//////////////////////////////////////////////////////////////////////////////

HimppViChan::HimppViChan(HimppViDev *vi_dev, VI_CHN vi_chn)
    : HimppVideoObject(vi_dev),
      _vi_dev(vi_dev), _chnid(vi_chn), _rotate(ROTATE_NONE)
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

    _ldc_attr.bEnable = HI_FALSE;
    _ldc_attr.stAttr.enViewType = LDC_VIEW_TYPE_ALL;
    _ldc_attr.stAttr.s32CenterXOffset = 0;
    _ldc_attr.stAttr.s32CenterYOffset = 0;
    _ldc_attr.stAttr.s32Ratio = 255;
}

HimppViChan::~HimppViChan()
{
}

bool HimppViChan::enableObject()
{
    HI_U32 s32Ret;

    if ((s32Ret = HI_MPI_VI_SetChnAttr(_chnid, &_chn_attr)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VI_SetChnAttr %d failed [%#x]\n",
                    _chnid, s32Ret);
        return false;
    }

    if ((s32Ret = HI_MPI_VI_SetRotate(_chnid, _rotate)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VI_SetRotate %d failed [%#x]\n",
                    _chnid, s32Ret);
        return false;
    }

    if ((s32Ret = HI_MPI_VI_EnableChn(_chnid)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VI_EnableChn %d failed [%#x]\n",
                    _chnid, s32Ret);
        return false;
    }

    if (_chnid < VIU_MAX_PHYCHN_NUM) {
        if ((s32Ret = HI_MPI_VI_SetLDCAttr(_chnid, &_ldc_attr)) != HI_SUCCESS) {
            HIMPP_PRINT("HI_MPI_VI_SetLDCAttr %d failed [%#x]\n",
                        _chnid, s32Ret);
        }
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
