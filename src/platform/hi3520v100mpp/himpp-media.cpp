/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-media.cpp
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

#include <SDL/SDL.h>
#include <himpp-video-region.h>
#include "himpp-media.h"

//////////////////////////////////////////////////////////////////////////////
// HimppVideoSource
//////////////////////////////////////////////////////////////////////////////

HimppVideoSource
::HimppVideoSource(Hi3520mppMedia &media, HimppViDev &vi_dev, HimppViChan &vi_chan)
    : _media(media), _imaging(*this), _vi_dev(vi_dev), _vi_chan(vi_chan)
{
}

HimppVideoSource::~HimppVideoSource()
{
}

void HimppVideoSource::setFramerate(uint32_t value)
{
    throw IpcamError("Readonly property");
}

uint32_t HimppVideoSource::getFramerate()
{
    return _media._vpss_chan0.getFramerate();
}

ImageResolution HimppVideoSource::getResolution()
{
    return _media._vpss_chan0.getResolution();
}

void HimppVideoSource::setResolution(ImageResolution &res)
{
    throw IpcamError("Readonly property");
}

IVideoSource::Imaging* HimppVideoSource::getImaging()
{
    return dynamic_cast<IVideoSource::Imaging*>(&_imaging);
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoSource::Imaging
//////////////////////////////////////////////////////////////////////////////

HimppVideoSource::Imaging::Imaging(HimppVideoSource &source)
    : _source(source), _exposure(source), _white_balance(source)
{
}

bool HimppVideoSource::Imaging::getMirror()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::setMirror(bool value)
{
    throw IpcamError("Requested interface not supported");
}

bool HimppVideoSource::Imaging::getFlip()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::setFlip(bool value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::getBrightness()
{
    return _source._vi_chan.getBrightness();
}

void HimppVideoSource::Imaging::setBrightness(uint32_t value)
{
    _source._vi_chan.setBrightness(value);
}

uint32_t HimppVideoSource::Imaging::getContrast()
{
    return _source._vi_chan.getContrast();
}

void HimppVideoSource::Imaging::setContrast(uint32_t value)
{
    _source._vi_chan.setContrast(value);
}

uint32_t HimppVideoSource::Imaging::getChroma()
{
    return _source._vi_chan.getChroma();
}

void HimppVideoSource::Imaging::setChroma(uint32_t value)
{
    _source._vi_chan.setChroma(value);
}

uint32_t HimppVideoSource::Imaging::getSaturation()
{
    return _source._vi_chan.getSaturation();
}

void HimppVideoSource::Imaging::setSaturation(uint32_t value)
{
    _source._vi_chan.setSaturation(value);
}

uint32_t HimppVideoSource::Imaging::getSharpness()
{
    return _source._vi_chan.getSharpness();
}

void HimppVideoSource::Imaging::setSharpness(uint32_t value)
{
    _source._vi_chan.setSharpness(value);
}

IVideoSource::Imaging::IrCutFilterMode
HimppVideoSource::Imaging::getIrCutFilterMode()
{
	throw IpcamError("Not implemented");
}

void HimppVideoSource::Imaging::setIrCutFilterMode
(IVideoSource::Imaging::IrCutFilterMode value)
{
	throw IpcamError("Not implemented");
}

IVideoSource::Imaging::Backlight* HimppVideoSource::Imaging::getBacklight()
{
	throw IpcamError("Request interface not implemented");
}

IVideoSource::Imaging::Focus* HimppVideoSource::Imaging::getFocus()
{
	throw IpcamError("Request interface not implemented");
}

IVideoSource::Imaging::Exposure* HimppVideoSource::Imaging::getExposure()
{
    return dynamic_cast<IVideoSource::Imaging::Exposure*>(&_exposure);
}

IVideoSource::Imaging::WhiteBalance* HimppVideoSource::Imaging::getWhiteBalance()
{
    return dynamic_cast<IVideoSource::Imaging::WhiteBalance*>(&_white_balance);
}

IVideoSource::Imaging::WideDynamicRange* HimppVideoSource::Imaging::getWideDynamicRange()
{
	throw IpcamError("Request interface not implemented");
}

IVideoSource::Imaging::LDC* HimppVideoSource::Imaging::getLDC()
{
	throw IpcamError("Request interface not implemented");
}

IVideoSource::Imaging::Gamma* HimppVideoSource::Imaging::getGamma()
{
	throw IpcamError("Request interface not implemented");
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoSource::Imaging::Exposure
//////////////////////////////////////////////////////////////////////////////

HimppVideoSource::Imaging::Exposure::Exposure(HimppVideoSource &source)
    : _source(source)
{
}

IVideoSource::Imaging::Exposure::ExposureMode
HimppVideoSource::Imaging::Exposure::getMode()
{
    throw IpcamError("Requested interface not supported");
}

void
HimppVideoSource::Imaging::Exposure::setMode(IVideoSource::Imaging::Exposure::ExposureMode value)
{
    throw IpcamError("Requested interface not supported");
}

IVideoSource::Imaging::Exposure::ExposurePriority
HimppVideoSource::Imaging::Exposure::getPriority()
{
    throw IpcamError("Requested interface not supported");
}

void
HimppVideoSource::Imaging::Exposure::setPriority(IVideoSource::Imaging::Exposure::ExposurePriority value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMinExposureTime()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMinExposureTime(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMaxExposureTime()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMaxExposureTime(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMinGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMinGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMaxGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMaxGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMinIris()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMinIris(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMaxIris()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMaxIris(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getExposureTime()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setExposureTime(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getIris()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setIris(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoSource::Imaging::WhiteBalance
//////////////////////////////////////////////////////////////////////////////
HimppVideoSource::Imaging::WhiteBalance::WhiteBalance(HimppVideoSource &source)
    : _source(source)
{
}

IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode
HimppVideoSource::Imaging::WhiteBalance::getMode()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setMode(IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getCbGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setCbGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getCrGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setCrGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getRGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setRGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getGGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setGGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getBGain()
{
    throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setBGain(uint32_t value)
{
    throw IpcamError("Requested interface not supported");
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoOSD
//////////////////////////////////////////////////////////////////////////////

HimppVideoOSD::HimppVideoOSD(HimppVencChan &venc_chan, uint32_t id)
  : _venc_chan(venc_chan), _region(&venc_chan, id)
{
	Uint32 Rmask = 0x1F << 10;
	Uint32 Gmask = 0x1F << 5;
	Uint32 Bmask = 0x1F << 0;
	Uint32 Amask = 0x01 << 15;
	_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 0, 0, 16,
	                                Rmask, Gmask, Bmask, Amask);

	_venc_chan.addVideoRegion(&_region);
}

HimppVideoOSD::~HimppVideoOSD()
{
	_venc_chan.delVideoRegion(&_region);

	if (_surface)
		SDL_FreeSurface(_surface);
}

IVideoOSD::Position HimppVideoOSD::getPosition()
{
	POINT_S pt = _region.getPosition();

	return Position(pt.s32X, pt.s32Y);
}

void HimppVideoOSD::setPosition(IVideoOSD::Position pos)
{
	POINT_S pt;
	pt.s32X = pos.x;
	pt.s32Y = pos.y;
	_region.setPosition(pt);
}

IVideoOSD::Size HimppVideoOSD::getSize()
{
	SIZE_S sz = _region.getSize();

	return Size(sz.u32Width, sz.u32Height);
}

void HimppVideoOSD::setSize(IVideoOSD::Size size)
{
	SIZE_S sz;
	sz.u32Width = size.w;
	sz.u32Height = size.h;
	_region.setSize(sz);
}

uint32_t HimppVideoOSD::getForegroundColor()
{
	return _region.getBgColor();
}

void HimppVideoOSD::setForegroundColor(uint32_t val)
{
	_region.setFgColor(val);
}

uint32_t HimppVideoOSD::getBackgroundColor()
{
	return _region.getBgColor();
}

void HimppVideoOSD::setBackgroundColor(uint32_t val)
{
	_region.setBgColor(val);
}

uint32_t HimppVideoOSD::getForegroundAlpha()
{
	return _region.getFgAlpha();
}

void HimppVideoOSD::setForegroundAlpha(uint32_t val)
{
	_region.setFgAlpha(val);
}

uint32_t HimppVideoOSD::getBackgroundAlpha()
{
	return _region.getBgAlpha();
}

void HimppVideoOSD::setBackgroundAlpha(uint32_t val)
{
	_region.setBgAlpha(val);
}

bool HimppVideoOSD::getInvertColor()
{
	return (bool)_region.getInvertColor();
}

void HimppVideoOSD::setInvertColor(bool val)
{
	_region.setInvertColor((HI_BOOL)val);
}

SDL_Surface *HimppVideoOSD::getSurface(uint16_t w, uint16_t h)
{
	BITMAP_S *bmp = _region.getBitmap(w, h);

	if (_surface) {
		_surface->flags |= SDL_PREALLOC;
		_surface->pixels = bmp->pData;
		_surface->w = bmp->u32Width;
		_surface->h = bmp->u32Height;
		_surface->pitch = bmp->u32Width * 2;
		SDL_SetClipRect(_surface, NULL);
	}

	return _surface;
}

void HimppVideoOSD::putSurface(SDL_Surface *surf)
{
	BITMAP_S bmp = {
		.enPixelFormat = PIXEL_FORMAT_RGB_1555,
		.u32Width = (HI_U32)surf->w,
		.u32Height = (HI_U32)surf->h,
		.pData = surf->pixels
	};

	_region.setBitmap(&bmp);
}


//////////////////////////////////////////////////////////////////////////////
// HimppH264VideoEncoder
//////////////////////////////////////////////////////////////////////////////

HimppH264VideoEncoder::HimppH264VideoEncoder
(Hi3520mppMedia &media, HimppVencChan& venc_chan)
    : _media(media), _venc_chan(venc_chan)
{
}

HimppH264VideoEncoder::~HimppH264VideoEncoder()
{
}

IVideoEncoder::EncodingType HimppH264VideoEncoder::getEncoding()
{
    return _venc_chan.getEncoding();
}

ImageResolution HimppH264VideoEncoder::getResolution()
{
    return _venc_chan.getResolution();
}

void HimppH264VideoEncoder::setResolution(ImageResolution &resolution)
{
    if (!_venc_chan.setResolution(resolution))
        throw IpcamError("set resolution failed");
}

uint32_t  HimppH264VideoEncoder::getFramerate()
{
    return _venc_chan.getFramerate();
}

void HimppH264VideoEncoder::setFramerate(uint32_t fps)
{
    if (!_venc_chan.setFramerate(fps))
        throw IpcamError("set framerate failed");
}

IVideoEncoder::RateCtrlMode HimppH264VideoEncoder::getRcMode()
{
    return _venc_chan.getRcMode();
}

void HimppH264VideoEncoder::setRcMode(IVideoEncoder::RateCtrlMode mode)
{
    if (!_venc_chan.setRcMode(mode))
        throw IpcamError("set rate control mode failed");
}

uint32_t HimppH264VideoEncoder::getBitrate()
{
    return _venc_chan.getBitrate();
}

void HimppH264VideoEncoder::setBitrate(uint32_t kbps)
{
    if (!_venc_chan.setBitrate(kbps))
        throw IpcamError("set bitrate failed");
}

IH264VideoEncoder::H264Profile HimppH264VideoEncoder::getProfile()
{
    return _venc_chan.getProfile();
}

void HimppH264VideoEncoder::setProfile(IH264VideoEncoder::H264Profile profile)
{
    if (!_venc_chan.setH264Profile(profile))
        throw IpcamError("set h264 profile failed");
}

uint32_t  HimppH264VideoEncoder::getGovLength()
{
    return _venc_chan.getGop();
}

void HimppH264VideoEncoder::setGovLength(uint32_t gop)
{
    if (!_venc_chan.setGop(gop))
        throw IpcamError("set gop failed");
}


IVideoOSD *HimppH264VideoEncoder::CreateOSD(uint32_t id)
{
    return new HimppVideoOSD(_venc_chan, id);
}


//////////////////////////////////////////////////////////////////////////////
// HimppH264VideoStream
//////////////////////////////////////////////////////////////////////////////

HimppH264VideoStream::HimppH264VideoStream
(IpcamRuntime *runtime, HimppVencChan &venc_chan)
    : _runtime(runtime), _venc_chan(venc_chan),
      _io(runtime->mainloop()), _consumer(NULL)
{
}

HimppH264VideoStream::~HimppH264VideoStream()
{
}

IVideoEncoder::EncodingType HimppH264VideoStream::getEncoding()
{
    return _venc_chan.getEncoding();
}

uint32_t HimppH264VideoStream::getBitrate()
{
    return _venc_chan.getBitrate();
}

bool HimppH264VideoStream::registerConsumer(StreamDataConsumer *consumer)
{
    _consumer = consumer;

    return _venc_chan.enable();
}

bool HimppH264VideoStream::unregisterConsumer(StreamDataConsumer *consumer)
{
    _consumer = consumer;

    return _venc_chan.disable();
}

void HimppH264VideoStream::enableStreaming()
{
	_io.set<HimppH264VideoStream, &HimppH264VideoStream::watch_handler>(this);
	_io.set(HI_MPI_VENC_GetFd(_venc_chan.channelId()), ev::READ);
	_io.start();
}

void HimppH264VideoStream::disableStreaming()
{
	_io.stop();
}

void HimppH264VideoStream::watch_handler(ev::io &w, int revents)
{
	if (!(revents & ev::READ))
		return;

	int chnid = _venc_chan.channelId();

	VENC_PACK_S stPack;
	VENC_STREAM_S stStream = {
		.pstPack = &stPack,
		.u32PackCount = 1,
		.u32Seq = 0
	};

	HI_S32 s32Ret = HI_MPI_VENC_GetStream(chnid, &stStream, HI_FALSE);
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("Get video stream %d failed [%#x]\n", chnid, s32Ret);
		return;
	}

#define PACK_SIZE   ARRAY_SIZE(stStream.pstPack[0].pu8Addr)
	StreamDataConsumer::StreamData::Pack stream_data_pack[PACK_SIZE];
	memset(&stream_data_pack, 0, sizeof(stream_data_pack));
	StreamDataConsumer::StreamData stream_data;
	stream_data.pack_count = 0;
	stream_data.pack = stream_data_pack;
	gettimeofday(&stream_data.tstamp, NULL);

	for (int i = 0; i < (int)PACK_SIZE; i++) {
		HI_U8 *ptr = stStream.pstPack[0].pu8Addr[i];
		HI_U32 len = stStream.pstPack[0].u32Len[i];

		if ((ptr == NULL) || (len == 0))
			break;

		stream_data.pack[i].addr = ptr;
		stream_data.pack[i].len = len;
		stream_data.pack_count++;
	}

	if (_consumer) {
		_consumer->streamData(stream_data);
	}

	s32Ret = HI_MPI_VENC_ReleaseStream(chnid, &stStream);
	if (s32Ret != HI_SUCCESS) {
		HIMPP_PRINT("Release video stream %d failed [%#x]\n", chnid, s32Ret);
	}
}


//////////////////////////////////////////////////////////////////////////////
// Hi3520mppMedia
//////////////////////////////////////////////////////////////////////////////

Hi3520mppMedia
::Hi3520mppMedia(IpcamRuntime *runtime, std::string sensor_name)
    : _runtime(runtime),
      _vi_dev0(&_codec, 0),
      _vi_chan0(&_vi_dev0, &_codec, 0), _vi_chan1(&_vi_dev0, &_codec, 1),
      _vi_chan2(&_vi_dev0, &_codec, 2), _vi_chan3(&_vi_dev0, &_codec, 3),
      _vpss_group0(&_vi_chan0, 0), _vpss_group1(&_vi_chan1, 1),
      _vpss_group2(&_vi_chan1, 2), _vpss_group3(&_vi_chan3, 3),
      _vpss_chan0(&_vpss_group0, 0), _vpss_chan1(&_vpss_group1, 1),
      _vpss_chan2(&_vpss_group2, 2), _vpss_chan3(&_vpss_group3, 3),
      _venc_chan0(&_vpss_chan0, 0, 0),
      _venc_chan1(&_vpss_chan1, 1, 1),
      _venc_chan2(&_vpss_chan2, 2, 2),
      _venc_chan3(&_vpss_chan3, 3, 3),
      _video_stream0(runtime, _venc_chan0),
      _video_stream1(runtime, _venc_chan1),
      _video_stream2(runtime, _venc_chan2),
      _video_stream3(runtime, _venc_chan3),
      _video_source0(*this, _vi_dev0, _vi_chan0),
      _video_source1(*this, _vi_dev0, _vi_chan1),
      _video_source2(*this, _vi_dev0, _vi_chan2),
      _video_source3(*this, _vi_dev0, _vi_chan3),
      _video_encoder0(*this, _venc_chan0), _video_encoder1(*this, _venc_chan1),
      _video_encoder2(*this, _venc_chan2), _video_encoder3(*this, _venc_chan3)
{
    ImageResolution r0 = _venc_chan0.getResolution();
    ImageResolution r1 = _venc_chan1.getResolution();
    ImageResolution r2 = _venc_chan2.getResolution();
    ImageResolution r3 = _venc_chan3.getResolution();
    _venc_chan0.setEncoding(IVideoEncoder::H264);
    _venc_chan0.setResolution(r0);
    _venc_chan1.setEncoding(IVideoEncoder::H264);
    _venc_chan1.setResolution(r1);
    _venc_chan2.setEncoding(IVideoEncoder::H264);
    _venc_chan2.setResolution(r2);
    _venc_chan3.setEncoding(IVideoEncoder::H264);
    _venc_chan3.setResolution(r3);
    _sysctl.addVideoBuffer(r0.Width * r0.Height * 3 / 2, 4);
    _sysctl.addVideoBuffer(r1.Width * r1.Height * 3 / 2, 4);
    _sysctl.addVideoBuffer(r2.Width * r2.Height * 3 / 2, 4);
    _sysctl.addVideoBuffer(r3.Width * r3.Height * 3 / 2, 4);
    _sysctl.addVideoBuffer(196 * 4, 4);
    _sysctl.enable();

    // register rtsp streaming
    _runtime->addRTSPStream(&_video_stream0, NULL);
    _runtime->addRTSPStream(&_video_stream1, NULL);
    _runtime->addRTSPStream(&_video_stream2, NULL);
    _runtime->addRTSPStream(&_video_stream3, NULL);

    // register video source control interface
    _runtime->addVideoSourceInterface(&_video_source0);
    _runtime->addVideoSourceInterface(&_video_source1);
    _runtime->addVideoSourceInterface(&_video_source2);
    _runtime->addVideoSourceInterface(&_video_source3);

    // register video encoder control interface
    _runtime->addVideoEncoderInterface(&_video_encoder0);
    _runtime->addVideoEncoderInterface(&_video_encoder1);
    _runtime->addVideoEncoderInterface(&_video_encoder2);
    _runtime->addVideoEncoderInterface(&_video_encoder3);
}

Hi3520mppMedia::~Hi3520mppMedia()
{
    if (_venc_chan0.isEnabled())
        _venc_chan0.disable();
    if (_venc_chan1.isEnabled())
        _venc_chan1.disable();
    if (_venc_chan2.isEnabled())
        _venc_chan2.disable();
    if (_venc_chan3.isEnabled())
        _venc_chan3.disable();
    _sysctl.disable();
}
