/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#include "himpp-media.h"


//////////////////////////////////////////////////////////////////////////////
// HimppVideoSource
//////////////////////////////////////////////////////////////////////////////

HimppVideoSource::HimppVideoSource(Hi3518mppMedia &media)
  : _media(media), _imaging(media)
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

HimppVideoSource::Imaging::Imaging(Hi3518mppMedia &media)
  : _media(media), _exposure(media), _white_balance(media)
{
}

bool HimppVideoSource::Imaging::getMirror()
{
	return _media._vi_chan0.getMirror();
}

void HimppVideoSource::Imaging::setMirror(bool value)
{
	return _media._vi_chan0.setMirror(value);
}

bool HimppVideoSource::Imaging::getFlip()
{
	return _media._vi_chan0.getFlip();
}

void HimppVideoSource::Imaging::setFlip(bool value)
{
	return _media._vi_chan0.setFlip(value);
}

uint32_t HimppVideoSource::Imaging::getBrightness()
{
	return _media._vi_dev0.getBrightness();
}

void HimppVideoSource::Imaging::setBrightness(uint32_t value)
{
	_media._vi_dev0.setBrightness(value);
}

uint32_t HimppVideoSource::Imaging::getContrast()
{
	return _media._vi_dev0.getContrast();
}

void HimppVideoSource::Imaging::setContrast(uint32_t value)
{
	_media._vi_dev0.setContrast(value);
}

uint32_t HimppVideoSource::Imaging::getChroma()
{
	return _media._vi_dev0.getChroma();
}

void HimppVideoSource::Imaging::setChroma(uint32_t value)
{
	_media._vi_dev0.setChroma(value);
}

uint32_t HimppVideoSource::Imaging::getSaturation()
{
	return _media._vi_dev0.getSaturation();
}

void HimppVideoSource::Imaging::setSaturation(uint32_t value)
{
	_media._vi_dev0.setSaturation(value);
}

uint32_t HimppVideoSource::Imaging::getSharpness()
{
	throw IpcamError("Not implemented");
}

void HimppVideoSource::Imaging::setSharpness(uint32_t value)
{
	throw IpcamError("Not implemented");
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


//////////////////////////////////////////////////////////////////////////////
// HimppVideoSource::Imaging::Exposure
//////////////////////////////////////////////////////////////////////////////

HimppVideoSource::Imaging::Exposure::Exposure(Hi3518mppMedia &media)
  : _media(media)
{
}

IVideoSource::Imaging::Exposure::ExposureMode
HimppVideoSource::Imaging::Exposure::getMode()
{
	HimppVideoISP *isp = (HimppVideoISP*)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->getExposureMode();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void
HimppVideoSource::Imaging::Exposure::setMode(IVideoSource::Imaging::Exposure::ExposureMode value)
{
	HimppVideoISP *isp = (HimppVideoISP*)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->setExposureMode(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}

IVideoSource::Imaging::Exposure::ExposurePriority
HimppVideoSource::Imaging::Exposure::getPriority()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->getExposurePriority();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void
HimppVideoSource::Imaging::Exposure::setPriority(IVideoSource::Imaging::Exposure::ExposurePriority value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->setExposurePriority(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMinExposureTime()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->getMinExposureTime();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMinExposureTime(uint32_t value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->setMinExposureTime(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMaxExposureTime()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->getMaxExposureTime();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMaxExposureTime(uint32_t value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->setMaxExposureTime(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMinGain()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->getMinGain();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMinGain(uint32_t value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->setMinGain(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMaxGain()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->getMaxGain();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMaxGain(uint32_t value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->setMaxGain(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMinIris()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->getMinIris();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMinIris(uint32_t value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->setMinIris(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getMaxIris()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->getMaxIris();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setMaxIris(uint32_t value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->setMaxIris(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getExposureTime()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->getExposureTime();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setExposureTime(uint32_t value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->setExposureTime(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getGain()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->getGain();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setGain(uint32_t value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->setGain(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::Exposure::getIris()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->getIris();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::Exposure::setIris(uint32_t value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::Exposure *exposure = isp->getExposure();
		if (exposure) {
			return exposure->setIris(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoSource::Imaging::WhiteBalance
//////////////////////////////////////////////////////////////////////////////
HimppVideoSource::Imaging::WhiteBalance::WhiteBalance(Hi3518mppMedia &media)
  : _media(media)
{
}

IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode
HimppVideoSource::Imaging::WhiteBalance::getMode()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
		if (wb) {
			return wb->getWBMode();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setMode(IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
		if (wb) {
			return wb->setWBMode(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getCbGain()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
		if (wb) {
			return wb->getCbGain();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setCbGain(uint32_t value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
		if (wb) {
			return wb->setCbGain(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getCrGain()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
		if (wb) {
			return wb->getCrGain();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setCrGain(uint32_t value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
		if (wb) {
			return wb->setCrGain(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getRGain()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
		if (wb) {
			return wb->getRGain();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setRGain(uint32_t value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
		if (wb) {
			return wb->setRGain(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getGGain()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
		if (wb) {
			return wb->getGGain();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setGGain(uint32_t value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
		if (wb) {
			return wb->setGGain(value);
		}
	}

	throw IpcamError("Requested interface not supported");
}

uint32_t HimppVideoSource::Imaging::WhiteBalance::getBGain()
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
		if (wb) {
			return wb->getBGain();
		}
	}

	throw IpcamError("Requested interface not supported");
}

void HimppVideoSource::Imaging::WhiteBalance::setBGain(uint32_t value)
{
	HimppVideoISP *isp = (HimppVideoISP *)_media._vi_dev0;
	if (isp) {
		HimppVideoISP::WhiteBalance *wb = isp->getWhiteBalance();
		if (wb) {
			return wb->setBGain(value);
		}
	}

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
(Hi3518mppMedia &media, HimppVencChan& venc_chan)
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
// HimppVideoStream
//////////////////////////////////////////////////////////////////////////////

HimppVideoStream::HimppVideoStream(IpcamRuntime *runtime, HimppVencChan &venc_chan)
  : _runtime(runtime), _venc_chan(venc_chan),
    _io(runtime->mainloop()), _consumer(NULL)
{
}

HimppVideoStream::~HimppVideoStream()
{
}

IVideoEncoder::EncodingType HimppVideoStream::getEncoding()
{
	return _venc_chan.getEncoding();
}

uint32_t HimppVideoStream::getBitrate()
{
	return _venc_chan.getBitrate();
}

bool HimppVideoStream::registerConsumer(StreamDataConsumer *consumer)
{
	_consumer = consumer;

	return _venc_chan.enable();
}

bool HimppVideoStream::unregisterConsumer(StreamDataConsumer *consumer)
{
	bool ret = true;

	_consumer = NULL;

	if (_venc_chan.isEnabled())
		ret = _venc_chan.disable();

	return ret;
}

void HimppVideoStream::enableStreaming()
{
	_io.set<HimppVideoStream, &HimppVideoStream::watch_handler>(this);
	_io.set(HI_MPI_VENC_GetFd(_venc_chan.channelId()), ev::READ);
	_io.start();
}

void HimppVideoStream::disableStreaming()
{
	_io.stop();
}

void HimppVideoStream::watch_handler(ev::io &w, int revents)
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
		//HIMPP_PRINT("Get video stream %d failed [%#x]\n", chnid, s32Ret);
		return;
	}

	StreamDataConsumer::StreamData::Pack stream_data_pack;
	memset(&stream_data_pack, 0, sizeof(stream_data_pack));
	StreamDataConsumer::StreamData stream_data;
	stream_data.pack_count = 0;
	stream_data.pack = &stream_data_pack;
	gettimeofday(&stream_data.tstamp, NULL);

	HI_U8 *ptr = stStream.pstPack[0].pu8Addr;
	HI_U32 len = stStream.pstPack[0].u32Len;

	if ((ptr != NULL) && (len != 0)) {
		stream_data.pack[0].addr = ptr;
		stream_data.pack[0].len = len;
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
// HimppAudioSource
//////////////////////////////////////////////////////////////////////////////

HimppAudioSource::HimppAudioSource(Hi3518mppMedia &media)
  : _media(media)
{
}

HimppAudioSource::~HimppAudioSource()
{
}

uint32_t HimppAudioSource::getChannels()
{
	return 1;
}


//////////////////////////////////////////////////////////////////////////////
// HimppAudioEncoder
//////////////////////////////////////////////////////////////////////////////

HimppAudioEncoder
::HimppAudioEncoder(Hi3518mppMedia& media, HimppAencChan& aenc_chan)
  : _media(media), _aenc_chan(aenc_chan)
{
}

HimppAudioEncoder::~HimppAudioEncoder()
{
}

IAudioEncoder::EncodingType
HimppAudioEncoder::getEncoding()
{
	return _media._aenc_chan0.getEncoding();
}

void HimppAudioEncoder::setEncoding(IAudioEncoder::EncodingType value)
{
	if (!_media._aenc_chan0.setEncoding(value))
		throw IpcamError("set audio encoding failed");
}

uint32_t HimppAudioEncoder::getBitrate()
{
	return _media._aenc_chan0.getBitrate();
}

void HimppAudioEncoder::setBitrate(uint32_t value)
{
	if (!_media._aenc_chan0.setBitrate(value))
		throw IpcamError("set audio bitrate failed");
}

uint32_t HimppAudioEncoder::getSampleRate()
{
	return _media._ai_dev0.getSampleRate();
}

void HimppAudioEncoder::setSampleRate(uint32_t value)
{
	if (!_media._ai_dev0.setSampleRate(value))
		throw IpcamError("set audio sample rate failed");
}

bool HimppAudioEncoder::enable()
{
	if (_aenc_chan.isEnabled())
		return true;

	return _aenc_chan.enable();
}

bool HimppAudioEncoder::disable()
{
	if (!_aenc_chan.isEnabled())
		return true;

	return _aenc_chan.disable();
}


//////////////////////////////////////////////////////////////////////////////
// HimppAudioStream
//////////////////////////////////////////////////////////////////////////////

HimppAudioStream::HimppAudioStream
(IpcamRuntime *runtime, HimppAiDev &ai_dev, HimppAencChan &aenc_chan)
  : _runtime(runtime), _ai_dev(ai_dev), _aenc_chan(aenc_chan),
    _io(runtime->mainloop()), _consumer(NULL)
{
}

HimppAudioStream::~HimppAudioStream()
{
}

IAudioEncoder::EncodingType HimppAudioStream::getEncoding()
{
	return _aenc_chan.getEncoding();
}

uint32_t HimppAudioStream::getSampleRate()
{
	return _ai_dev.getSampleRate();
}

bool HimppAudioStream::registerConsumer(StreamDataConsumer *consumer)
{
	_consumer = consumer;

	return _aenc_chan.enable();
}

bool HimppAudioStream::unregisterConsumer(StreamDataConsumer *consumer)
{
	bool ret = true;

	_consumer = NULL;

	if (_aenc_chan.isEnabled())
		ret = _aenc_chan.disable();

	return ret;
}

void HimppAudioStream::enableStreaming()
{
	_io.set<HimppAudioStream, &HimppAudioStream::watch_handler>(this);
	_io.set(HI_MPI_AENC_GetFd(_aenc_chan.channelId()), ev::READ);
	_io.start();
}

void HimppAudioStream::disableStreaming()
{
	_io.stop();
}

void HimppAudioStream::watch_handler(ev::io &w, int revents)
{
	if (!(revents & ev::READ))
		return;

	int chnid = _aenc_chan.channelId();

	AUDIO_STREAM_S stStream;
	HI_S32 s32Ret = HI_MPI_AENC_GetStream(chnid, &stStream, HI_FALSE);
	if (HI_SUCCESS != s32Ret) {
		HIMPP_PRINT("Get audio stream %d failed [%#x]\n", chnid, s32Ret);
		return;
	}

	if (stStream.u32Len > 4) {
		StreamDataConsumer::StreamData::Pack stream_data_pack;
		memset(&stream_data_pack, 0, sizeof(stream_data_pack));

		StreamDataConsumer::StreamData stream_data;
		stream_data.pack_count = 1;
		stream_data.pack = &stream_data_pack;
		gettimeofday(&stream_data.tstamp, NULL);

		stream_data.pack[0].addr = stStream.pStream + 4;
		stream_data.pack[0].len = stStream.u32Len - 4;

		if (_consumer)
			_consumer->streamData(stream_data);
	}

	s32Ret = HI_MPI_AENC_ReleaseStream(chnid, &stStream);
	if (HI_SUCCESS != s32Ret) {
		HIMPP_PRINT("Release audio stream %d failed [%#x]\n", chnid, s32Ret);
	}
}


//////////////////////////////////////////////////////////////////////////////
// Hi3518mppMedia
//////////////////////////////////////////////////////////////////////////////

Hi3518mppMedia::Hi3518mppMedia(IpcamRuntime *runtime, std::string sensor_name)
  : _runtime(runtime),
    _sysctl(HIMPP_SYS_ALIGN_WIDTH, HIMPP_PIXEL_FORMAT),
    _sensor(himpp_video_sensor_map.at(sensor_name)),
    _vi_dev0(dynamic_cast<HimppObject*>(&_sysctl), &_sensor, 0),
    _vi_chan0(&_vi_dev0, 0),
    _vpss_group0(&_vi_chan0, 0),
    _vpss_chan0(&_vpss_group0, 0),
    _venc_chan0(dynamic_cast<HimppVideoObject*>(&_vpss_chan0), 0, 0),
    _venc_chan1(dynamic_cast<HimppVideoObject*>(&_vpss_chan0), 1, 1),
    _acodec(AUDIO_SAMPLE_RATE_8000),
    _ai_dev0(&_sysctl, &_acodec, 0), _ai_chan0(&_ai_dev0, 0),
    _aenc_chan0(&_ai_chan0, 0), _aenc_chan1(&_ai_chan0, 1),
    _video_stream0(runtime, _venc_chan0),
    _video_stream1(runtime, _venc_chan1),
    _audio_stream0(runtime, _ai_dev0, _aenc_chan0),
    _audio_stream1(runtime, _ai_dev0, _aenc_chan1),
    _video_source0(*this),
    _video_encoder0(*this, _venc_chan0),
    _video_encoder1(*this, _venc_chan1),
    _audio_source0(*this),
    _audio_encoder0(*this, _aenc_chan0),
    _audio_encoder1(*this, _aenc_chan1)
{
	ImageResolution ri = _vi_dev0.getResolution();
	ImageResolution r0 = _venc_chan0.getResolution();
	ImageResolution r1("D1");
	_venc_chan0.setEncoding(IVideoEncoder::H264);
	//_venc_chan0.setFramerate(25);
	_venc_chan1.setEncoding(IVideoEncoder::H264);
	_venc_chan1.setResolution(r1);
	_venc_chan1.setFramerate(15);
	_sysctl.addVideoBuffer(ri.Width * ri.Height * 3 / 2, 3);
	_sysctl.addVideoBuffer(r0.Width * r0.Height * 3 / 2, 3);
	_sysctl.addVideoBuffer(r1.Width * r1.Height * 3 / 2, 2);
	_sysctl.addVideoBuffer(196 * 4, 2);

	_sysctl.enable();

	runtime->addRTSPStream(&_video_stream0, &_audio_stream0);
	runtime->addRTSPStream(&_video_stream1, &_audio_stream1);

	runtime->addVideoSourceInterface(&_video_source0);

	runtime->addVideoEncoderInterface(&_video_encoder0);
	runtime->addVideoEncoderInterface(&_video_encoder1);

	runtime->addAudioSourceInterface(&_audio_source0);
	runtime->addAudioEncoderInterface(&_audio_encoder0);
	runtime->addAudioEncoderInterface(&_audio_encoder1);
}

Hi3518mppMedia::~Hi3518mppMedia()
{
	if (_venc_chan0.isEnabled())
		_venc_chan0.disable();
	if (_venc_chan1.isEnabled())
		_venc_chan1.disable();

	_sysctl.disable();
}
