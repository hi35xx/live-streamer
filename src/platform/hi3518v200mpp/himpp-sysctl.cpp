/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-sysctl.cpp
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

#include <string.h>
#include <mpi_vb.h>
#include <mpi_sys.h>
#include "himpp-sysctl.h"

HimppSysctl::HimppSysctl(HI_U32 align_width, HI_U32 max_pool_cnt)
    : HimppObject(NULL), alignWidth(align_width), maxPoolCount(max_pool_cnt)
{
}

HimppSysctl::~HimppSysctl()
{
}

void HimppSysctl::addVideoBuffer(HI_U32 blksiz, HI_U32 blkcnt)
{
    buffers.emplace(buffers.end(), blksiz, blkcnt);
}

void HimppSysctl::setAlignWidth(HI_U32 value)
{
    alignWidth = value;
}

void HimppSysctl::setMaxPoolCount(HI_U32 value)
{
    maxPoolCount = value;
}

bool HimppSysctl::enableObject()
{
    HI_S32 s32Ret = HI_FAILURE;

    VB_CONF_S vbconf;
    memset(&vbconf, 0, sizeof(vbconf));
    vbconf.u32MaxPoolCnt = maxPoolCount;
    int i = 0;
    for (auto &b : buffers) {
        vbconf.astCommPool[i].u32BlkSize = b.blksiz;
        vbconf.astCommPool[i].u32BlkCnt = b.blkcnt;
        i++;
    }

    HI_MPI_VB_Exit();
    HI_MPI_SYS_Exit();

    s32Ret = HI_MPI_VB_SetConf(&vbconf);
    if (s32Ret != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VB_SetConf failed [%#x]\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VB_Init();
    if (s32Ret != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VB_Init failed [%#x]\n", s32Ret);
        return HI_FAILURE;
    }

    MPP_SYS_CONF_S sysconf = {
        .u32AlignWidth = alignWidth
    };
    s32Ret = HI_MPI_SYS_SetConf(&sysconf);
    if (s32Ret != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_SYS_SetConf failed [%#x]\n", s32Ret);
        goto err_vb_cleanup;
    }

    s32Ret = HI_MPI_SYS_Init();
    if (s32Ret != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_SYS_Init failed [%#x]\n", s32Ret);
        goto err_vb_cleanup;
    }
    return true;

err_vb_cleanup:
    HI_MPI_VB_Exit();
    return false;
}

bool HimppSysctl::disableObject()
{
    HI_S32 s32Ret;

    if ((s32Ret = HI_MPI_SYS_Exit()) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_SYS_Exit failed [%#x]\n", s32Ret);
    }
    if ((s32Ret = HI_MPI_VB_Exit()) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_VB_Exit failed [%#x]\n", s32Ret);
    }
    if ((s32Ret = HI_MPI_SYS_CloseFd()) != HI_SUCCESS) {
        HIMPP_PRINT("HI_MPI_SYS_CloseFd failed [%#x]\n", s32Ret);
    }

    return true;
}

HimppSysctl::operator MPP_CHN_S* ()
{
    return NULL;
}
