/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-vpss-group.cpp
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
#include "himpp-video-vpss.h"


HimppVpssGroup::HimppVpssGroup(HimppVideoObject *source, VPSS_GRP grp)
    : HimppVideoObject(source), _grpid(grp)
{
}

HimppVpssGroup::~HimppVpssGroup()
{
}

HimppVpssGroup::operator MPP_CHN_S* ()
{
    return NULL;
}

bool HimppVpssGroup::setResolution(ImageResolution &res)
{
    return false;
}

ImageResolution HimppVpssGroup::getResolution()
{
    return videoSource()->getResolution();
}

bool HimppVpssGroup::setFramerate(uint32_t fps)
{
    return false;
}

uint32_t HimppVpssGroup::getFramerate()
{
    return videoSource()->getFramerate();
}

bool HimppVpssGroup::enableObject()
{
    HI_S32 s32Ret = HI_FAILURE;

    ImageResolution res = videoSource()->getResolution();
    HI_VPSS_GRP_ATTR_S attr;
    attr.u32MaxW = res.Width;
    attr.u32MaxH = res.Height;
    attr.bDrEn = HI_FALSE;
    attr.bDbEn = HI_FALSE;
    attr.bIeEn = HI_FALSE;
    attr.bNrEn = HI_TRUE;
    attr.bHistEn = HI_TRUE;
    attr.enDieMode = VPSS_DIE_MODE_AUTO;
    attr.enPixFmt = HIMPP_PIXEL_FORMAT;
    if ((s32Ret = HI_MPI_VPSS_CreateGrp(_grpid, &attr)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VPSS_CreateGrp %d failed [%#x]\n",
                    _grpid, s32Ret);
        return false;
    }

    if ((s32Ret = HI_MPI_VPSS_StartGrp(_grpid)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VPSS_CreateGrp %d failed [%#x]\n",
                    _grpid, s32Ret);
        goto err_destroy_grp;
    }

    MPP_CHN_S dst_chn;
    dst_chn.enModId = HI_ID_VPSS;
    dst_chn.s32DevId = _grpid;
    dst_chn.s32ChnId = 0;
    if ((s32Ret = HI_MPI_SYS_Bind(*source(), &dst_chn)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_SYS_Bind %d failed [%#x]\n",
                    _grpid, s32Ret);
        goto err_stop_grp;
    }

    return true;

err_stop_grp:
    HI_MPI_VPSS_StopGrp(_grpid);
err_destroy_grp:
    HI_MPI_VPSS_DestroyGrp(_grpid);

    return false;
}

bool HimppVpssGroup::disableObject()
{
    HI_S32 s32Ret;

    MPP_CHN_S dst_chn;
    dst_chn.enModId = HI_ID_VPSS;
    dst_chn.s32DevId = _grpid;
    dst_chn.s32ChnId = 0;
    if ((s32Ret = HI_MPI_SYS_UnBind(*source(), &dst_chn)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_SYS_UnBind %d failed [%#x]\n",
                    _grpid, s32Ret);
    }

    s32Ret = HI_MPI_VPSS_StopGrp(_grpid);
    if (s32Ret != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VPSS_StopGrp %d failed [%#x]\n",
                    _grpid, s32Ret);
    }

    s32Ret = HI_MPI_VPSS_DestroyGrp(_grpid);
    if (s32Ret != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VPSS_DestroyGrp %d failed [%#x]\n",
                    _grpid, s32Ret);
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////

HimppVpssChan::HimppVpssChan(HimppVpssGroup *group, VPSS_CHN chn)
    : HimppVideoObject(group), _vpss_group(group), _chnid(chn),
      _resolution(group->getResolution()),
      _framerate(group->getFramerate())
{
    _mpp_chn.enModId = HI_ID_VPSS;
    _mpp_chn.s32DevId = group->getGroupId();
    _mpp_chn.s32ChnId = chn;

    if (chn < VPSS_MAX_PHY_CHN_NUM)
        _type = VPSS_CHN_TYPE_PHY;
    else if (chn == VPSS_BYPASS_CHN)
        _type = VPSS_CHN_TYPE_BYPASS;
    else
        _type = VPSS_CHN_TYPE_EXT;
}

HimppVpssChan::~HimppVpssChan()
{
}

HimppVpssChan::operator MPP_CHN_S* ()
{
    return &_mpp_chn;
}

bool HimppVpssChan::setResolution(ImageResolution &res)
{
    _resolution = res;

    return true;
}

ImageResolution HimppVpssChan::getResolution()
{
    return _resolution;
}

bool HimppVpssChan::setFramerate(uint32_t fps)
{
    _framerate = fps;

    return true;
}

uint32_t HimppVpssChan::getFramerate()
{
    return _framerate;
}

bool HimppVpssChan::enableObject()
{
    int grp = _vpss_group->getGroupId();
    int chn = _chnid;
    VPSS_CHN_ATTR_S chn_attr;
    VPSS_CHN_MODE_S chn_mode;
    VPSS_EXT_CHN_ATTR_S ext_chn_attr;
    HI_S32 s32Ret;

    switch (_type) {
    case VPSS_CHN_TYPE_PHY:
        chn_attr.bFrameEn = HI_FALSE;
        chn_attr.bSpEn = HI_TRUE;
        if ((s32Ret = HI_MPI_VPSS_SetChnAttr(grp, chn, &chn_attr)) != HI_SUCCESS) {
            HIMPP_PRINT("HI_MPI_VPSS_SetChnAttr %d-%d failed %#x\n",
                        grp, chn, s32Ret);
            return false;
        }
        chn_mode.enChnMode = VPSS_CHN_MODE_USER;
        chn_mode.u32Width = _resolution.Width;
        chn_mode.u32Height = _resolution.Height;
        chn_mode.bDouble = HI_FALSE;
        chn_mode.enPixelFormat = HIMPP_PIXEL_FORMAT;
        if ((s32Ret = HI_MPI_VPSS_SetChnMode(grp, chn, &chn_mode)) != HI_SUCCESS) {
            HIMPP_PRINT("HI_MPI_VPSS_SetChnMode %d-%d failed %#x\n",
                        grp, chn, s32Ret);
            return false;
        }
        break;
    case VPSS_CHN_TYPE_EXT:
        ext_chn_attr.s32BindChn = 0;
        ext_chn_attr.s32SrcFrameRate = _framerate;
        if ((s32Ret = HI_MPI_VPSS_SetExtChnAttr(grp, chn, &ext_chn_attr)) != HI_SUCCESS) {
            HIMPP_PRINT("HI_MPI_VPSS_SetExtChnAttr %d-%d failed %#x\n",
                        grp, chn, s32Ret);
            return false;
        }
        break;
    default:
        break;
    }

    if ((s32Ret = HI_MPI_VPSS_EnableChn(grp, chn)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VPSS_EnableChn %d-%d failed [%#x]\n",
                    grp, chn, s32Ret);
        return false;
    }

    return true;
}

bool HimppVpssChan::disableObject()
{
    int grp = _vpss_group->getGroupId();
    int chn = _chnid;
    HI_S32 s32Ret;

    if ((s32Ret = HI_MPI_VPSS_DisableChn(grp, chn)) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VPSS_DisableChn %d-%d failed [%#x]\n",
                    grp, chn, s32Ret);
        return false;
    }

    return true;
}
