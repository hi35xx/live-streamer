/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#include <mpi_venc.h>
#include <media-common.h>
#include <himpp-base.h>
#include <media-stream.h>
#include <video-encoder.h>

using namespace Ipcam::Media;

class HimppVencChan;

#define HIMPP_VENC_CHAN(o)  dynamic_cast<HimppVencChan*>(o)

class HimppVencChan :
	public HimppVideoElement,
	public H264VideoEncoder,
	public JPEGVideoEncoder,
	public H264VideoStreamSource,
	public JPEGVideoStreamSource
{
public:
	HimppVencChan(HimppVideoElement* source, VideoEncodingType encoding, VENC_CHN chnid);
	~HimppVencChan();

	// implementation of HimppElement
	MPP_CHN_S*			bindSource();
	// implementation of HimppVideoElement/VideoStreamSource
	Resolution			resolution();
	uint32_t			framerate();

	// implementation of VideoStreamSource
	VideoEncodingType   encoding();
	uint32_t			bitrate();
	void				attach(StreamSink* sink);
	void				detach(StreamSink* sink);
	void				play();
	void				stop();
	void				resume();
	void				pause();

	// implementation of VideoEncoder
	VideoEncodingType   getEncoding();
	Resolution			getResolution();
	void				setResolution(Resolution value);
	uint32_t			getFramerate();
	void				setFramerate(uint32_t value);
	void				setRcMode(RateCtrlMode value);
	RateCtrlMode		getRcMode();
	void				setBitrate(uint32_t value);
	uint32_t			getBitrate();

	// implentation of H264VideoEncoder
	void				setH264Profile(H264Profile value);
	H264Profile			getH264Profile();
	void				setGovLength(uint32_t value);
	uint32_t			getGovLength();
	void				setMinQP(uint32_t value);
	uint32_t			getMinQP();
	void				setMaxQP(uint32_t value);
	uint32_t			getMaxQP();
	void				setFrameRefMode(FrameRefMode value);
	FrameRefMode		getFrameRefMode();
	void				setIntraRefresh(IntraRefreshParam value);
	IntraRefreshParam	getIntraRefresh();

	void				requestIDR();

	VideoOSD*			CreateOSD();
	void				DeleteOSD(VideoOSD*);

	// local functions
	VENC_CHN channelId() { return _chnid; }

protected:
	// implementation of MediaElement
	void doEnableElement();
	void doDisableElement();

private:
	MPP_CHN_S					_mpp_chn;
	VENC_CHN					_chnid;
	VideoEncodingType			_encoding;
	H264Profile					_h264profile;
	RateCtrlMode				_rcmode;
	Resolution					_resolution;
	uint32_t					_framerate;
	uint32_t					_bitrate;
	uint32_t					_gop;
	uint32_t					_min_qp;
	uint32_t					_max_qp;
	VENC_CROP_CFG_S				_crop_cfg;
	FrameRefMode				_refmode;
	IntraRefreshParam			_intrarefresh;

	ev::io						_io;

	std::set<SoftRenderVideoOSD*>	_osds;

	void watch_handler(ev::io& w, int revents);

	void prepareRcAttr(VENC_RC_ATTR_S &rcattr);
	void prepareChnAttr(VENC_CHN_ATTR_S &chnattr);
};

#endif // _HIMPP_VIDEO_VENC_H_
