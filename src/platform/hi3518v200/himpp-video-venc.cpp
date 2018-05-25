/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#include <ev++.h>
#include <mpi_sys.h>
#include <mpi_venc.h>
#include <mpi_region.h>

#include <himpp-common.h>
#include <himpp-video-region.h>
#include "himpp-video-venc.h"

extern ev::default_loop mainloop;

HimppVencChan::HimppVencChan
(HimppVideoElement* source, VideoEncodingType encoding, VENC_CHN chnid)
  : VideoElement(VIDEO_ELEMENT(source)),
    HimppVideoElement(source),
    _chnid(chnid),
    _encoding(encoding),
    _h264profile(HIGH),
    _rcmode(VBR),
    _resolution(source->resolution()),
    _framerate(source->framerate()),
    _bitrate(2048),
    _gop(_framerate * 2),
    _min_qp(16),
    _max_qp(51),
	_refmode(1, 0, true),
	_intrarefresh(false, false, 11, 51),
	_io(mainloop)
{
	_crop_cfg.bEnable = HI_FALSE;
	_crop_cfg.stRect.u32Width = 0;
	_crop_cfg.stRect.u32Height = 0;
	_crop_cfg.stRect.s32X = 0;
	_crop_cfg.stRect.s32Y = 0;

	_mpp_chn.enModId = HI_ID_VENC;
	_mpp_chn.s32DevId = 0;
	_mpp_chn.s32ChnId = chnid;
}

HimppVencChan::~HimppVencChan()
{
}

VideoEncodingType HimppVencChan::encoding()
{
	return _encoding;
}

uint32_t HimppVencChan::bitrate()
{
	return _bitrate;
}

void HimppVencChan::attach(StreamSink* sink)
{
	VideoStreamSource::attach(sink);
}

void HimppVencChan::detach(StreamSink* sink)
{
	VideoStreamSource::detach(sink);
}

void HimppVencChan::play()
{
	enable();
}

void HimppVencChan::stop()
{
	disable();
}

void HimppVencChan::resume()
{
	_io.set<HimppVencChan, &HimppVencChan::watch_handler>(this);
	_io.set(HI_MPI_VENC_GetFd(channelId()), ev::READ);
	_io.start();
}

void HimppVencChan::pause()
{
	_io.stop();
}

VideoEncodingType HimppVencChan::getEncoding()
{
	return _encoding;
}

void HimppVencChan::watch_handler(ev::io &w, int revents)
{
	if (!(revents & ev::READ))
		return;

	HI_S32 s32Ret;
	int chnid = channelId();

	VENC_CHN_STAT_S stChnStat;
	s32Ret = HI_MPI_VENC_Query(chnid, &stChnStat);
	if (s32Ret != HI_SUCCESS || stChnStat.u32CurPacks == 0)
		return;

	VENC_PACK_S stPack[stChnStat.u32CurPacks];
	VENC_STREAM_S stStream = {
		.pstPack = stPack,
		.u32PackCount = stChnStat.u32CurPacks,
		.u32Seq = 0
	};

	s32Ret = HI_MPI_VENC_GetStream(chnid, &stStream, 0);
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("Get video stream %d failed [%#x]\n", chnid, s32Ret);
		return;
	}

	StreamBuffer *sbuf = NULL;
	JPEGStreamBuffer jpegbuffer;
	H264StreamBuffer h264buffer;

	switch (encoding()) {
	case H264:
		h264buffer.keyframe = (stStream.stH264Info.enRefType == BASE_IDRSLICE);
		sbuf = &h264buffer;
		break;
	case JPEG:
	case MJPEG:
		jpegbuffer.qfactor = stStream.stJpegInfo.u32Qfactor;
		jpegbuffer.width = resolution().width() >> 3;
		jpegbuffer.height = resolution().height() >> 3;
		sbuf = &jpegbuffer;
		break;
	default:
		HIMPP_PRINT("Unsupported video encoding %d\n", encoding());
		break;
	}

	if (sbuf) {
		StreamBuffer::Pack stream_data_pack[stStream.u32PackCount];
		memset(&stream_data_pack, 0, sizeof(stream_data_pack));
		sbuf->pack_count = stStream.u32PackCount;
		sbuf->pack = stream_data_pack;
		gettimeofday(&sbuf->tstamp, NULL);

		for (int i = 0; i < (int)stStream.u32PackCount; i++) {
			VENC_PACK_S *pstPack = &stStream.pstPack[i];
			sbuf->pack[i].addr = pstPack->pu8Addr + pstPack->u32Offset;
			sbuf->pack[i].len = pstPack->u32Len - pstPack->u32Offset;
		}

		streamData(sbuf);
	}

	s32Ret = HI_MPI_VENC_ReleaseStream(chnid, &stStream);
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("Release video stream %d failed [%#x]\n", chnid, s32Ret);
	}
}

void HimppVencChan::setH264Profile(H264Profile profile)
{
	if (is_enabled()) {
		H264Profile oldval = _h264profile;
		doDisableElement();
		try {
			_h264profile = profile;
			doEnableElement();
		} catch (IpcamError& e) {
			_h264profile = oldval;
			doEnableElement();
			throw e;
		}
	}
	_h264profile = profile;
}

H264Profile HimppVencChan::getH264Profile()
{
	return _h264profile;
}

void HimppVencChan::setRcMode(RateCtrlMode mode)
{
	if (is_enabled()) {
		RateCtrlMode oldval = _rcmode;
		doDisableElement();
		try {
			_rcmode = mode;
			doEnableElement();
		} catch (IpcamError& e) {
			_rcmode = oldval;
			doEnableElement();
			throw e;
		}
	}
	_rcmode = mode;
}

RateCtrlMode HimppVencChan::getRcMode()
{
	return _rcmode;
}

void HimppVencChan::setBitrate(uint32_t value)
{
	if (is_enabled()) {
		uint32_t oldval = _bitrate;
		doDisableElement();
		try {
			_bitrate = value;
			doEnableElement();
		} catch (IpcamError& e) {
			_bitrate = oldval;
			doEnableElement();
			throw e;
		}
	}
	_bitrate = value;
}

uint32_t HimppVencChan::getBitrate()
{
	return _bitrate;
}

void HimppVencChan::setGovLength(uint32_t value)
{
	if (is_enabled()) {
		uint32_t oldval = _gop;
		doDisableElement();
		try {
			_gop = value;
			doEnableElement();
		} catch (IpcamError& e) {
			_gop = oldval;
			doEnableElement();
			throw e;
		}
	}
	_gop = value;
}

uint32_t HimppVencChan::getGovLength()
{
	return _gop;
}

void HimppVencChan::setMinQP(uint32_t value)
{
	if (is_enabled()) {
		uint32_t oldval = _min_qp;
		doDisableElement();
		try {
			_min_qp = value;
			doEnableElement();
		} catch (IpcamError& e) {
			_min_qp = oldval;
			doEnableElement();
			throw e;
		}
	}
	_min_qp = value;
}

uint32_t HimppVencChan::getMinQP()
{
	return _min_qp;
}

void HimppVencChan::setMaxQP(uint32_t value)
{
	if (is_enabled()) {
		uint32_t oldval = _max_qp;
		doDisableElement();
		try {
			_max_qp = value;
			doEnableElement();
		} catch (IpcamError& e) {
			_max_qp = oldval;
			doEnableElement();
			throw e;
		}
	}
	_max_qp = value;
}

uint32_t HimppVencChan::getMaxQP()
{
	return _max_qp;
}

void HimppVencChan::setFrameRefMode(FrameRefMode value)
{
	if (is_enabled()) {
		FrameRefMode oldval = _refmode;
		doDisableElement();
		try {
			_refmode = value;
			doEnableElement();
		} catch (IpcamError& e) {
			_refmode = oldval;
			doEnableElement();
			throw e;
		}
	}
	_refmode = value;
}

H264VideoEncoder::FrameRefMode HimppVencChan::getFrameRefMode()
{
	if (is_enabled()) {
		VENC_PARAM_REF_S stRefParam;
		HI_S32 s32Ret;
		if ((s32Ret = HI_MPI_VENC_GetRefParam(_chnid, &stRefParam)) == HI_SUCCESS) {
			_refmode = FrameRefMode(stRefParam.u32Base, stRefParam.u32Enhance, stRefParam.bEnablePred);
		} else {
			HIMPP_PRINT("getSkipRefMode(%d) failed [%#x]\n", _chnid, s32Ret);
		}
	}
	return _refmode;
}

void HimppVencChan::setIntraRefresh(IntraRefreshParam value)
{
	if (is_enabled()) {
		IntraRefreshParam oldval = _intrarefresh;
		doDisableElement();
		try {
			_intrarefresh = value;
			doEnableElement();
		} catch (IpcamError& e) {
			_intrarefresh = oldval;
			doEnableElement();
			throw e;
		}
	}
	_intrarefresh = value;
}

H264VideoEncoder::IntraRefreshParam HimppVencChan::getIntraRefresh()
{
	if (is_enabled()) {
		VENC_PARAM_INTRA_REFRESH_S stIntraRefresh;
		HI_S32 s32Ret;
		if ((s32Ret = HI_MPI_VENC_GetIntraRefresh(_chnid, &stIntraRefresh)) == HI_SUCCESS) {
			_intrarefresh = IntraRefreshParam (stIntraRefresh.bRefreshEnable, stIntraRefresh.bISliceEnable, stIntraRefresh.u32RefreshLineNum, stIntraRefresh.u32ReqIQp);
		} else {
			HIMPP_PRINT("getSkipRefMode(%d) failed [%#x]\n", _chnid, s32Ret);
		}
	}
	return _intrarefresh;
}

void HimppVencChan::requestIDR()
{
	if (is_enabled()) {
		HI_S32 s32Ret = HI_MPI_VENC_RequestIDR(_chnid, HI_TRUE);
		if (s32Ret != HI_SUCCESS) {
			HIMPP_PRINT("HI_MPI_VENC_RequestIDR %d failed [%#x]\n",
			            _chnid, s32Ret);
		}
	}
}

VideoOSD* HimppVencChan::CreateOSD()
{
	RGN_HANDLE handle = himpp_region_allocator.allocHandle();
	if (handle == (RGN_HANDLE)-1)
		throw IpcamError("Failed to allocate region handle");

	HimppVideoRegion* region(new HimppVideoRegion(this, handle));
	if (region == nullptr) {
		himpp_region_allocator.freeHandle(handle);
		throw IpcamError("Failed to create video osd");
	}

	_osds.insert(SOFT_RENDER_VIDEO_OSD(region));

	if (is_enabled()) {
		region->enable();
	}

	return region;
}

void HimppVencChan::DeleteOSD(VideoOSD* osd)
{
	_osds.erase(SOFT_RENDER_VIDEO_OSD(osd));
}

MPP_CHN_S* HimppVencChan::bindSource()
{
	return &_mpp_chn;
}

Resolution HimppVencChan::resolution()
{
	return _resolution;
}

uint32_t HimppVencChan::framerate()
{
	return _framerate;
}

// Calculate the crop configuration
static void calc_crop_cfg(Resolution& in, Resolution& out, VENC_CROP_CFG_S& crop_cfg)
{
	RECT_S &rect = crop_cfg.stRect;
	if (in.width() * out.height() > out.width() * in.height()) {
		// crop width
		crop_cfg.bEnable = HI_TRUE;
		rect.u32Height = in.height();
		rect.u32Width = out.width() * in.height() / out.height();
		rect.s32X = ((in.width() - rect.u32Width) / 2) & 0xFFFFFFF0;
		rect.s32Y = 0;
	}
	else if (in.width() * out.height() < out.width() * in.height()) {
		// crop height
		crop_cfg.bEnable = HI_TRUE;
		rect.u32Width = in.width();
		rect.u32Height = out.height() * in.width() / out.width();
		rect.s32X = 0;
		rect.s32Y = (in.height() - rect.u32Height) / 2;
	}
	else {
		// crop is not necessary
		crop_cfg.bEnable = HI_FALSE;
		rect.u32Width = 0;
		rect.u32Height = 0;
		rect.s32X = 0;
		rect.s32Y = 0;
	}
}

void HimppVencChan::setResolution(Resolution value)
{
	Resolution in = HIMPP_VIDEO_ELEMENT(source())->resolution();
	// Sanity check
	if ((value.width() > in.width()) || (value.height() > in.height())) {
		throw IpcamError("Resolution must not larger than input");
	}

	if (is_enabled()) {
		Resolution oldres = _resolution;
		doDisableElement();
		try {
			_resolution = value;
			doEnableElement();
		} catch (IpcamError& e) {
			_resolution = oldres;
			doEnableElement();
			throw e;
		}
	}
	_resolution = value;
}

Resolution HimppVencChan::getResolution()
{
	return _resolution;
}

void HimppVencChan::setFramerate(uint32_t value)
{
	uint32_t newgop = ((_gop + _framerate / 2) / _framerate) * value;
	if (is_enabled()) {
		uint32_t oldval = _framerate;
		uint32_t oldgop = _gop;
		doDisableElement();
		try {
			_framerate = value;
			_gop = newgop;
			doEnableElement();
		} catch (IpcamError& e) {
			_framerate = oldval;
			_gop = oldgop;
			doEnableElement();
			throw e;
		}
	}
	_framerate = value;
	_gop = newgop;
}

uint32_t HimppVencChan::getFramerate()
{
	return _framerate;
}

void HimppVencChan::prepareRcAttr(VENC_RC_ATTR_S &attr)
{
	HI_U32 stattime;
	stattime = _gop / _framerate;
	stattime = stattime > 0 ? stattime : 1;

	switch (_encoding) {
	case H264:
		switch (_rcmode) {
		case CBR:
			attr.enRcMode = VENC_RC_MODE_H264CBR;
			attr.stAttrH264Cbr.u32Gop = _gop;
			attr.stAttrH264Cbr.u32StatTime = stattime;
			attr.stAttrH264Cbr.u32SrcFrmRate = VIDEO_ELEMENT(source())->framerate();
			attr.stAttrH264Cbr.fr32DstFrmRate = _framerate;
			attr.stAttrH264Cbr.u32BitRate = _bitrate;
			attr.stAttrH264Cbr.u32FluctuateLevel = 0;
			break;
		case VBR:
			attr.enRcMode = VENC_RC_MODE_H264VBR;
			attr.stAttrH264Vbr.u32Gop = _gop;
			attr.stAttrH264Vbr.u32StatTime = stattime;
			attr.stAttrH264Vbr.u32SrcFrmRate = VIDEO_ELEMENT(source())->framerate();
			attr.stAttrH264Vbr.fr32DstFrmRate = _framerate;
			attr.stAttrH264Vbr.u32MinQp = _min_qp;
			attr.stAttrH264Vbr.u32MaxQp = _max_qp;
			attr.stAttrH264Vbr.u32MaxBitRate = _bitrate;
			break;
		case FIXQP:
			attr.enRcMode = VENC_RC_MODE_H264FIXQP;
			attr.stAttrH264FixQp.u32Gop = _gop;
			attr.stAttrH264FixQp.u32SrcFrmRate = VIDEO_ELEMENT(source())->framerate();
			attr.stAttrH264FixQp.fr32DstFrmRate = _framerate;
			attr.stAttrH264FixQp.u32IQp = 20;
			attr.stAttrH264FixQp.u32PQp = 23;
			break;
		default:
			HIMPP_PRINT("Unsupported RC mode[%d]\n", _rcmode);
			throw IpcamError("Unsupported RC mode");
		}
		break;
	case MJPEG:
		switch (_rcmode) {
		case CBR:
			attr.enRcMode = VENC_RC_MODE_MJPEGCBR;
			attr.stAttrMjpegeCbr.u32StatTime = 1;
			attr.stAttrMjpegeCbr.u32SrcFrmRate = VIDEO_ELEMENT(source())->framerate();
			attr.stAttrMjpegeCbr.fr32DstFrmRate = _framerate;
			attr.stAttrMjpegeCbr.u32BitRate = _bitrate;
			attr.stAttrMjpegeCbr.u32FluctuateLevel = 0;
			break;
		case VBR:
			attr.enRcMode = VENC_RC_MODE_MJPEGVBR;
			attr.stAttrMjpegeVbr.u32StatTime = 1;
			attr.stAttrMjpegeVbr.u32SrcFrmRate = VIDEO_ELEMENT(source())->framerate();
			attr.stAttrMjpegeVbr.fr32DstFrmRate = _framerate;
			attr.stAttrMjpegeVbr.u32MaxBitRate = _bitrate;
			attr.stAttrMjpegeVbr.u32MinQfactor = 50;
			attr.stAttrMjpegeVbr.u32MaxQfactor = 95;
			break;
		case FIXQP:
			attr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;
			attr.stAttrMjpegeFixQp.u32SrcFrmRate = VIDEO_ELEMENT(source())->framerate();
			attr.stAttrMjpegeFixQp.fr32DstFrmRate = _framerate;
			attr.stAttrMjpegeFixQp.u32Qfactor = 90;
			break;
		default:
			HIMPP_PRINT("Unsupported RC mode[%d]\n", _rcmode);
			throw IpcamError("Unsupported RC mode");
		}
		break;
	case JPEG:
		break;
	default:
		HIMPP_PRINT("Unsupported encoding code[%d]\n", _encoding);
		throw IpcamError("Unsupported encoding");
	}
}

void HimppVencChan::prepareChnAttr(VENC_CHN_ATTR_S &attr)
{
	HI_U32 stattime;
	stattime = _gop / _framerate;
	stattime = stattime > 0 ? stattime : 1;
	
	switch (_encoding) {
	case H264:
		attr.stVeAttr.enType = PT_H264;
		attr.stVeAttr.stAttrH264e.u32MaxPicWidth = ROUNDUP16(_resolution.width());
		attr.stVeAttr.stAttrH264e.u32MaxPicHeight = ROUNDUP16(_resolution.height());
		attr.stVeAttr.stAttrH264e.u32BufSize = ROUNDUP64(_resolution.width()) * ROUNDUP64(_resolution.height());
		attr.stVeAttr.stAttrH264e.u32PicWidth = _resolution.width();
		attr.stVeAttr.stAttrH264e.u32PicHeight = _resolution.height();
		attr.stVeAttr.stAttrH264e.u32Profile = _h264profile;
		attr.stVeAttr.stAttrH264e.bByFrame = HI_FALSE;
		attr.stVeAttr.stAttrH264e.u32BFrameNum = 0;
		attr.stVeAttr.stAttrH264e.u32RefNum = 1;
		break;
	case MJPEG:
		attr.stVeAttr.enType = PT_MJPEG;
		attr.stVeAttr.stAttrMjpeg.u32MaxPicWidth = ROUNDUP16(_resolution.width());
		attr.stVeAttr.stAttrMjpeg.u32MaxPicHeight = ROUNDUP16(_resolution.height());
		attr.stVeAttr.stAttrMjpeg.u32BufSize = ROUNDUP64(_resolution.width()) * ROUNDUP64(_resolution.height());
		attr.stVeAttr.stAttrMjpeg.u32PicWidth = _resolution.width();
		attr.stVeAttr.stAttrMjpeg.u32PicHeight = _resolution.height();
		attr.stVeAttr.stAttrMjpeg.bByFrame = HI_TRUE;
		break;
	case JPEG:
		attr.stVeAttr.enType = PT_JPEG;
		attr.stVeAttr.stAttrJpeg.u32MaxPicWidth = ROUNDUP16(_resolution.width());
		attr.stVeAttr.stAttrJpeg.u32MaxPicHeight = ROUNDUP16(_resolution.height());
		attr.stVeAttr.stAttrJpeg.u32BufSize = ROUNDUP64(_resolution.width()) * ROUNDUP64(_resolution.height());
		attr.stVeAttr.stAttrJpeg.u32PicWidth = _resolution.width();
		attr.stVeAttr.stAttrJpeg.u32PicHeight = _resolution.height();
		attr.stVeAttr.stAttrJpeg.bByFrame = HI_TRUE;
		attr.stVeAttr.stAttrJpeg.bSupportDCF = HI_FALSE;
		break;
	default:
		HIMPP_PRINT("Unsupported encoding code[%d]\n", _encoding);
		throw IpcamError("Unsupported encoding");
	}
	// initialize RateControl attributes
	prepareRcAttr(attr.stRcAttr);
}

void HimppVencChan::doEnableElement()
{
	HI_S32 s32Ret;
	VENC_CROP_CFG_S dis_crop = { .bEnable = HI_FALSE };

	VENC_CHN_ATTR_S attr;
	prepareChnAttr(attr);
	if ((s32Ret = HI_MPI_VENC_CreateChn(_chnid, &attr)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VENC_CreateChn %d faild [%#x]\n",
					_chnid, s32Ret);
		throw IpcamError("Failed to create VENC channel");
	}

	if ((s32Ret = HI_MPI_VENC_SetMaxStreamCnt(_chnid, 2)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_VENC_SetMaxStreamCnt %d failed [%#x]\n",
					_chnid, s32Ret);
	}

	if (_encoding == H264) {
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

		VENC_PARAM_REF_S stRefParam;
		if ((s32Ret = HI_MPI_VENC_GetRefParam(_chnid, &stRefParam)) == HI_SUCCESS) {
			stRefParam.u32Base = _refmode.Base;
			stRefParam.u32Enhance = _refmode.Enhanced;
			stRefParam.bEnablePred = (HI_BOOL)_refmode.EnablePred;
			if ((s32Ret = HI_MPI_VENC_SetRefParam(_chnid, &stRefParam)) != HI_SUCCESS) {
				HIMPP_PRINT("HI_MPI_VENC_SetRefParam(%d) failed [%#x]\n",
				            _chnid, s32Ret);
			}
		} else {
			HIMPP_PRINT("HI_MPI_VENC_GetRefParam(%d) failed [%#x]\n",
			            _chnid, s32Ret);
		}

		VENC_PARAM_INTRA_REFRESH_S stIntraRefresh;
		if ((s32Ret = HI_MPI_VENC_GetIntraRefresh(_chnid, &stIntraRefresh)) == HI_SUCCESS) {
			stIntraRefresh.bRefreshEnable = (HI_BOOL)_intrarefresh.EnableRefresh;
			stIntraRefresh.bISliceEnable = (HI_BOOL)_intrarefresh.EnableISlice;
			stIntraRefresh.u32RefreshLineNum = _intrarefresh.RefreshLineNum;
			stIntraRefresh.u32ReqIQp = _intrarefresh.ReqIQp;
			if ((s32Ret = HI_MPI_VENC_SetIntraRefresh(_chnid, &stIntraRefresh)) != HI_SUCCESS) {
				HIMPP_PRINT("HI_MPI_VENC_SetIntraRefresh(%d) failed [%#x]\n",
				            _chnid, s32Ret);
			}
		} else {
			HIMPP_PRINT("HI_MPI_VENC_GetIntraRefresh(%d) failed [%#x]\n",
			            _chnid, s32Ret);
		}
	}

	Resolution in = HIMPP_VIDEO_ELEMENT(source())->resolution();
	calc_crop_cfg(in, _resolution, _crop_cfg);
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
	if ((s32Ret = HI_MPI_SYS_Bind(HIMPP_VIDEO_ELEMENT(source())->bindSource(), &dst_chn)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_SYS_Bind %d failed [%#x]\n",
					_chnid, s32Ret);
		goto err_stop_recv_pic;
	}

	for (auto it : _osds) {
		it->enable();
	}

	return;

err_stop_recv_pic:
	HI_MPI_VENC_StopRecvPic(_chnid);
err_disable_crop:
	HI_MPI_VENC_SetCrop(_chnid, &dis_crop);
err_destroy_chn:
	HI_MPI_VENC_DestroyChn(_chnid);

	throw IpcamError("Failed to enable VENC streaming");
}

void HimppVencChan::doDisableElement()
{
	HimppVideoElement* src = HIMPP_VIDEO_ELEMENT(source());
	HI_S32 s32Ret;
	VENC_CROP_CFG_S dis_crop = { .bEnable = HI_FALSE };

	for (auto it : _osds) {
		if (it->is_enabled())
			it->disable();
	}

	MPP_CHN_S dst_chn = {
		.enModId = HI_ID_VENC,
		.s32DevId = 0,
		.s32ChnId = _chnid
	};
	if ((s32Ret = HI_MPI_SYS_UnBind(src->bindSource(), &dst_chn)) != HI_SUCCESS) {
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
}
