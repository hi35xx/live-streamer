/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#include <mpi_sys.h>

#include <himpp-common.h>
#include "himpp-video-vpss.h"


HimppVpssGroup::HimppVpssGroup(HimppVideoElement* source, VPSS_GRP grp)
: VideoElement(VIDEO_ELEMENT(source)), HimppVideoElement(source),
DefaultVideoSource(DEFAULT_VIDEO_SOURCE(source)), _grpid(grp)
{
}

HimppVpssGroup::~HimppVpssGroup()
{
}

void HimppVpssGroup::doEnableElement()
{
	HI_S32 s32Ret = HI_FAILURE;
	HimppVideoElement* src = HIMPP_VIDEO_ELEMENT(source());
	if (src == NULL) throw ("source element is null");  // sanity check

	Resolution dim = src->resolution();
	VPSS_GRP_ATTR_S attr;
	attr.u32MaxW = dim.width();
	attr.u32MaxH = dim.height();
	attr.bDciEn = HI_FALSE;
	attr.bIeEn = HI_FALSE;
	attr.bNrEn = HI_TRUE;
	attr.bHistEn = HI_FALSE;
	attr.enDieMode = VPSS_DIE_MODE_NODIE;
	attr.enPixFmt = HIMPP_PIXEL_FORMAT;
	if ((s32Ret = HI_MPI_VPSS_CreateGrp(_grpid, &attr)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VPSS_CreateGrp %d failed [%#x]\n",
		            _grpid, s32Ret);
		throw IpcamError("failed to enable vpss group");
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
	if ((s32Ret = HI_MPI_SYS_Bind(src->bindSource(), &dst_chn)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_SYS_Bind %d failed [%#x]\n",
		            _grpid, s32Ret);
		goto err_stop_grp;
	}

	return;

	err_stop_grp:
		HI_MPI_VPSS_StopGrp(_grpid);
	err_destroy_grp:
		HI_MPI_VPSS_DestroyGrp(_grpid);

		throw IpcamError("Failed to enable vpss group");
}

void HimppVpssGroup::doDisableElement()
{
	HimppVideoElement* src = HIMPP_VIDEO_ELEMENT(source());
	HI_S32 s32Ret;

	MPP_CHN_S dst_chn;
	dst_chn.enModId = HI_ID_VPSS;
	dst_chn.s32DevId = _grpid;
	dst_chn.s32ChnId = 0;
	if ((s32Ret = HI_MPI_SYS_UnBind(src->bindSource(), &dst_chn)) != HI_SUCCESS) {
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
}

//////////////////////////////////////////////////////////////////////////////

HimppVpssChan::HimppVpssChan(HimppVideoElement* source, VPSS_CHN chn)
  : VideoElement(VIDEO_ELEMENT(source)),
    HimppVideoElement(source),
    DefaultVideoSource(DEFAULT_VIDEO_SOURCE(source)),
    _chnid(chn)
{
	HimppVpssGroup* group = HIMPP_VPSS_GROUP(source);

	_mpp_chn.enModId = HI_ID_VPSS;
	_mpp_chn.s32DevId = group->groupId();
	_mpp_chn.s32ChnId = chn;

	if (chn < VPSS_MAX_PHY_CHN_NUM)
		_type = VPSS_CHN_TYPE_PHY;
	else
		_type = VPSS_CHN_TYPE_EXT;
}

HimppVpssChan::~HimppVpssChan()
{
}

MPP_CHN_S* HimppVpssChan::bindSource()
{
	return &_mpp_chn;
}

void HimppVpssChan::doEnableElement()
{
	HimppVpssGroup* group = HIMPP_VPSS_GROUP(source());
	Resolution dim = resolution();
	int grp = group->groupId();
	int chn = _chnid;
	VPSS_CHN_ATTR_S chn_attr;
	VPSS_CHN_MODE_S chn_mode;
	VPSS_EXT_CHN_ATTR_S ext_chn_attr;
	HI_S32 s32Ret;

	switch (_type) {
	case VPSS_CHN_TYPE_PHY:
		chn_attr.bSpEn = HI_FALSE;
		chn_attr.bBorderEn = HI_FALSE;
		chn_attr.bMirror = HI_FALSE;
		chn_attr.bFlip = HI_FALSE;
		chn_attr.s32SrcFrameRate = -1;
		chn_attr.s32DstFrameRate = -1;
		if ((s32Ret = HI_MPI_VPSS_SetChnAttr(grp, chn, &chn_attr)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VPSS_SetChnAttr %d-%d failed %#x\n",
			            grp, chn, s32Ret);
			throw IpcamError("Failed to enable vpss channel");
		}
		chn_mode.enChnMode = VPSS_CHN_MODE_USER;
		chn_mode.u32Width = dim.width();
		chn_mode.u32Height = dim.height();
		chn_mode.bDouble = HI_FALSE;
		chn_mode.enPixelFormat = HIMPP_PIXEL_FORMAT;
		chn_mode.enCompressMode = COMPRESS_MODE_NONE;
		if ((s32Ret = HI_MPI_VPSS_SetChnMode(grp, chn, &chn_mode)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VPSS_SetChnMode %d-%d failed %#x\n",
			            grp, chn, s32Ret);
			throw IpcamError("Failed to enable vpss channel");
		}
		break;
	case VPSS_CHN_TYPE_EXT:
		ext_chn_attr.s32BindChn = 0;
		ext_chn_attr.s32SrcFrameRate = framerate();
		if ((s32Ret = HI_MPI_VPSS_SetExtChnAttr(grp, chn, &ext_chn_attr)) != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VPSS_SetExtChnAttr %d-%d failed %#x\n",
			            grp, chn, s32Ret);
			throw IpcamError("Failed to enable vpss channel");
		}
		break;
	default:
		break;
	}

	if ((s32Ret = HI_MPI_VPSS_EnableChn(grp, chn)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VPSS_EnableChn %d-%d failed [%#x]\n",
		            grp, chn, s32Ret);
		throw IpcamError("Failed to enable vpss channel");
	}
}

void HimppVpssChan::doDisableElement()
{
	HimppVpssGroup* group = HIMPP_VPSS_GROUP(source());
	int grp = group->groupId();
	int chn = _chnid;
	HI_S32 s32Ret;

	if ((s32Ret = HI_MPI_VPSS_DisableChn(grp, chn)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VPSS_DisableChn %d-%d failed [%#x]\n",
		            grp, chn, s32Ret);
	}
}
