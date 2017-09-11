/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * video-source.cpp
 * Copyright (C) 2017 Watson Xu <xuhuashan@gmail.com>
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

#include "video-source.h"

namespace Ipcam {
namespace Media {

//////////////////////////////////////////////////////////////////////////////
// VideoSource::Imaging::AntiFlicker
//////////////////////////////////////////////////////////////////////////////

VideoSource::Imaging::AntiFlicker::AntiFlicker(Imaging& imaging)
  : _imaging(imaging)
{
}

VideoSource::Imaging::AntiFlicker::~AntiFlicker()
{
}

AntiFlickerMode VideoSource::Imaging::AntiFlicker::getMode()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::AntiFlicker::setMode(AntiFlickerMode value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::AntiFlicker::getFrequency()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::AntiFlicker::setFrequency(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

//////////////////////////////////////////////////////////////////////////////
// VideoSource::Imaging::Backlight
//////////////////////////////////////////////////////////////////////////////

VideoSource::Imaging::Backlight::Backlight(Imaging& imaging)
  : _imaging(imaging)
{
}

VideoSource::Imaging::Backlight::~Backlight()
{
}

BLCMode VideoSource::Imaging::Backlight::getMode()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Backlight::setMode(BLCMode value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::Backlight::getLevel()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Backlight::setLevel(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

//////////////////////////////////////////////////////////////////////////////
// VideoSource::Imaging::Focus
//////////////////////////////////////////////////////////////////////////////

VideoSource::Imaging::Focus::Focus(Imaging& imaging)
  : _imaging(imaging)
{
}

VideoSource::Imaging::Focus::~Focus()
{
}

FocusMode VideoSource::Imaging::Focus::getMode()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Focus::setMode(FocusMode value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::Focus::getDefaultSpeed()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Focus::setDefaultSpeed(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::Focus::getNearLimit()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Focus::setNearLimit(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::Focus::getFarLimit()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Focus::setFarLimit(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

//////////////////////////////////////////////////////////////////////////////
// VideoSource::Imaging::Exposure
//////////////////////////////////////////////////////////////////////////////

VideoSource::Imaging::Exposure::Exposure(Imaging& imaging)
  : _imaging(imaging)
{
}

VideoSource::Imaging::Exposure::~Exposure()
{
}

ExposureMode VideoSource::Imaging::Exposure::getMode()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Exposure::setMode(ExposureMode value)
{
	throw IpcamError(property_not_implemented);
}

ExposurePriority VideoSource::Imaging::Exposure::getPriority()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Exposure::setPriority(ExposurePriority value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::Exposure::getMinExposureTime()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Exposure::setMinExposureTime(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::Exposure::getMaxExposureTime()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Exposure::setMaxExposureTime(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::Exposure::getCompensation()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Exposure::setCompensation(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::Exposure::getMinGain()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Exposure::setMinGain(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::Exposure::getMaxGain()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Exposure::setMaxGain(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::Exposure::getMinIris()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Exposure::setMinIris(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::Exposure::getMaxIris()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Exposure::setMaxIris(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::Exposure::getExposureTime()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Exposure::setExposureTime(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::Exposure::getGain()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Exposure::setGain(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::Exposure::getIris()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Exposure::setIris(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

//////////////////////////////////////////////////////////////////////////////
// VideoSource::Imaging::WhiteBalance
//////////////////////////////////////////////////////////////////////////////

VideoSource::Imaging::WhiteBalance::WhiteBalance(Imaging& imaging)
  : _imaging(imaging)
{
}

VideoSource::Imaging::WhiteBalance::~WhiteBalance()
{
}

WhiteBalanceMode VideoSource::Imaging::WhiteBalance::getMode()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::WhiteBalance::setMode(WhiteBalanceMode value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::WhiteBalance::getCrGain()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::WhiteBalance::setCrGain(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::WhiteBalance::getCbGain()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::WhiteBalance::setCbGain(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::WhiteBalance::getRGain()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::WhiteBalance::setRGain(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::WhiteBalance::getGGain()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::WhiteBalance::setGGain(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::WhiteBalance::getBGain()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::WhiteBalance::setBGain(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

//////////////////////////////////////////////////////////////////////////////
// VideoSource::Imaging::WideDynamicRange
//////////////////////////////////////////////////////////////////////////////

VideoSource::Imaging::WideDynamicRange::WideDynamicRange(Imaging& imaging)
  : _imaging(imaging)
{
}

VideoSource::Imaging::WideDynamicRange::~WideDynamicRange()
{
}

WDRMode VideoSource::Imaging::WideDynamicRange::getMode()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::WideDynamicRange::setMode(WDRMode value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::WideDynamicRange::getLevel()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::WideDynamicRange::setLevel(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

//////////////////////////////////////////////////////////////////////////////
// VideoSource::Imaging::LDC
//////////////////////////////////////////////////////////////////////////////

VideoSource::Imaging::LDC::LDC(Imaging& imaging)
  : _imaging(imaging)
{
}

VideoSource::Imaging::LDC::~LDC()
{
}

LDCMode VideoSource::Imaging::LDC::getMode()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::LDC::setMode(LDCMode value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::LDC::getRatio()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::LDC::setRatio(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

//////////////////////////////////////////////////////////////////////////////
// VideoSource::Imaging::Gamma
//////////////////////////////////////////////////////////////////////////////

VideoSource::Imaging::Gamma::Gamma(Imaging& imaging)
  : _imaging(imaging)
{
}

VideoSource::Imaging::Gamma::~Gamma()
{
}

GammaCurveData& VideoSource::Imaging::Gamma::getCurveData()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::Gamma::setCurveData(GammaCurveData& value)
{
	throw IpcamError(property_not_implemented);
}

//////////////////////////////////////////////////////////////////////////////
// VideoSource::Imaging::NoiseReduction
//////////////////////////////////////////////////////////////////////////////

VideoSource::Imaging::NoiseReduction::NoiseReduction(Imaging& imaging)
  : _imaging(imaging)
{
}

VideoSource::Imaging::NoiseReduction::~NoiseReduction()
{
}

VNRMode VideoSource::Imaging::NoiseReduction::getMode()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::NoiseReduction::setMode(VNRMode value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::NoiseReduction::getLevel()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::NoiseReduction::setLevel(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

NrParamTable& VideoSource::Imaging::NoiseReduction::getParamTable()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::NoiseReduction::setParamTable(NrParamTable& value)
{
	throw IpcamError(property_not_implemented);
}

//////////////////////////////////////////////////////////////////////////////
// VideoSource::Imaging
//////////////////////////////////////////////////////////////////////////////

VideoSource::Imaging::Imaging(VideoSource& videosource)
  : _video_source(videosource)
{
}

VideoSource::Imaging::~Imaging()
{
}

bool VideoSource::Imaging::getMirror()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::setMirror(bool value)
{
	throw IpcamError(property_not_implemented);
}

bool VideoSource::Imaging::getFlip()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::setFlip(bool value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::getBrightness()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::setBrightness(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::getContrast()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::setContrast(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::getChroma()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::setChroma(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::getSaturation()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::setSaturation(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

uint32_t VideoSource::Imaging::getSharpness()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::setSharpness(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

IrCutFilterMode VideoSource::Imaging::getIrCutFilterMode()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::Imaging::setIrCutFilterMode(IrCutFilterMode value)
{
	throw IpcamError(property_not_implemented);
}

VideoSource::Imaging::AntiFlicker& VideoSource::Imaging::antiflicker()
{
	throw IpcamError(interface_not_implemented);
}

VideoSource::Imaging::Backlight& VideoSource::Imaging::backlight()
{
	throw IpcamError(interface_not_implemented);
}

VideoSource::Imaging::Focus& VideoSource::Imaging::focus()
{
	throw IpcamError(interface_not_implemented);
}

VideoSource::Imaging::Exposure& VideoSource::Imaging::exposure()
{
	throw IpcamError(interface_not_implemented);
}

VideoSource::Imaging::WhiteBalance& VideoSource::Imaging::whitebalance()
{
	throw IpcamError(interface_not_implemented);
}

VideoSource::Imaging::WideDynamicRange& VideoSource::Imaging::widedynamicrange()
{
	throw IpcamError(interface_not_implemented);
}

VideoSource::Imaging::LDC& VideoSource::Imaging::ldc()
{
	throw IpcamError(interface_not_implemented);
}

VideoSource::Imaging::Gamma& VideoSource::Imaging::gamma()
{
	throw IpcamError(interface_not_implemented);
}

VideoSource::Imaging::NoiseReduction& VideoSource::Imaging::noisereduction()
{
	throw IpcamError(interface_not_implemented);
}

//////////////////////////////////////////////////////////////////////////////
// VideoSource
//////////////////////////////////////////////////////////////////////////////

VideoSource::VideoSource()
{
}

VideoSource::~VideoSource()
{
}

uint32_t VideoSource::getFrameRate()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::setFrameRate(uint32_t value)
{
	throw IpcamError(property_not_implemented);
}

Resolution VideoSource::getResolution()
{
	throw IpcamError(property_not_implemented);
}

void VideoSource::setResolution(Resolution value)
{
	throw IpcamError(property_not_implemented);
}

VideoSource::Imaging& VideoSource::imaging()
{
	throw IpcamError(interface_not_implemented);
}


//////////////////////////////////////////////////////////////////////////////
// DefaultVideoSource:
//   forward every method call to it's source object
//   except the derived class implement the functionality.
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// DefaultVideoSource::Imaging::AntiFlicker
//////////////////////////////////////////////////////////////////////////////

DefaultVideoSource::Imaging::AntiFlicker::AntiFlicker(Imaging& imaging)
  : VideoSource::Imaging::AntiFlicker(dynamic_cast<VideoSource::Imaging&>(imaging))
{
}

DefaultVideoSource::Imaging::AntiFlicker::~AntiFlicker()
{
}

AntiFlickerMode DefaultVideoSource::Imaging::AntiFlicker::getMode()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().antiflicker().getMode();
}

void DefaultVideoSource::Imaging::AntiFlicker::setMode(AntiFlickerMode value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().antiflicker().setMode(value);
}

uint32_t DefaultVideoSource::Imaging::AntiFlicker::getFrequency()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().antiflicker().getFrequency();
}

void DefaultVideoSource::Imaging::AntiFlicker::setFrequency(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().antiflicker().setFrequency(value);
}

//////////////////////////////////////////////////////////////////////////////
// DefaultVideoSource::Imaging::Backlight
//////////////////////////////////////////////////////////////////////////////

DefaultVideoSource::Imaging::Backlight::Backlight(Imaging& imaging)
  : VideoSource::Imaging::Backlight(dynamic_cast<VideoSource::Imaging&>(imaging))
{
}

DefaultVideoSource::Imaging::Backlight::~Backlight()
{
}

BLCMode DefaultVideoSource::Imaging::Backlight::getMode()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().backlight().getMode();
}

void DefaultVideoSource::Imaging::Backlight::setMode(BLCMode value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().backlight().setMode(value);
}

uint32_t DefaultVideoSource::Imaging::Backlight::getLevel()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().backlight().getLevel();
}

void DefaultVideoSource::Imaging::Backlight::setLevel(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().backlight().setLevel(value);
}

//////////////////////////////////////////////////////////////////////////////
// DefaultVideoSource::Imaging::Focus
//////////////////////////////////////////////////////////////////////////////

DefaultVideoSource::Imaging::Focus::Focus(Imaging& imaging)
  : VideoSource::Imaging::Focus(dynamic_cast<VideoSource::Imaging&>(imaging))
{
}

DefaultVideoSource::Imaging::Focus::~Focus()
{
}

FocusMode DefaultVideoSource::Imaging::Focus::getMode()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().focus().getMode();
}

void DefaultVideoSource::Imaging::Focus::setMode(FocusMode value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().focus().setMode(value);
}

uint32_t DefaultVideoSource::Imaging::Focus::getDefaultSpeed()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().focus().getDefaultSpeed();
}

void DefaultVideoSource::Imaging::Focus::setDefaultSpeed(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().focus().setDefaultSpeed(value);
}

uint32_t DefaultVideoSource::Imaging::Focus::getNearLimit()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().focus().getNearLimit();
}

void DefaultVideoSource::Imaging::Focus::setNearLimit(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().focus().setNearLimit(value);
}

uint32_t DefaultVideoSource::Imaging::Focus::getFarLimit()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().focus().getFarLimit();
}

void DefaultVideoSource::Imaging::Focus::setFarLimit(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().focus().setFarLimit(value);
}

//////////////////////////////////////////////////////////////////////////////
// DefaultVideoSource::Imaging::Exposure
//////////////////////////////////////////////////////////////////////////////

DefaultVideoSource::Imaging::Exposure::Exposure(Imaging& imaging)
  : VideoSource::Imaging::Exposure(dynamic_cast<VideoSource::Imaging&>(imaging))
{
}

DefaultVideoSource::Imaging::Exposure::~Exposure()
{
}

ExposureMode DefaultVideoSource::Imaging::Exposure::getMode()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().exposure().getMode();
}

void DefaultVideoSource::Imaging::Exposure::setMode(ExposureMode value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().exposure().setMode(value);
}

ExposurePriority DefaultVideoSource::Imaging::Exposure::getPriority()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().exposure().getPriority();
}

void DefaultVideoSource::Imaging::Exposure::setPriority(ExposurePriority value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().exposure().setPriority(value);
}

uint32_t DefaultVideoSource::Imaging::Exposure::getMinExposureTime()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().exposure().getMinExposureTime();
}

void DefaultVideoSource::Imaging::Exposure::setMinExposureTime(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().exposure().setMinExposureTime(value);
}

uint32_t DefaultVideoSource::Imaging::Exposure::getMaxExposureTime()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().exposure().getMaxExposureTime();
}

void DefaultVideoSource::Imaging::Exposure::setMaxExposureTime(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().exposure().setMaxExposureTime(value);
}

uint32_t DefaultVideoSource::Imaging::Exposure::getMinGain()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().exposure().getMinGain();
}

void DefaultVideoSource::Imaging::Exposure::setMinGain(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().exposure().setMinGain(value);
}

uint32_t DefaultVideoSource::Imaging::Exposure::getMaxGain()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().exposure().getMaxGain();
}

void DefaultVideoSource::Imaging::Exposure::setMaxGain(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().exposure().setMaxGain(value);
}

uint32_t DefaultVideoSource::Imaging::Exposure::getCompensation()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().exposure().getCompensation();
}

void DefaultVideoSource::Imaging::Exposure::setCompensation(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().exposure().setCompensation(value);
}

uint32_t DefaultVideoSource::Imaging::Exposure::getMinIris()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().exposure().getMinIris();
}

void DefaultVideoSource::Imaging::Exposure::setMinIris(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().exposure().setMinIris(value);
}

uint32_t DefaultVideoSource::Imaging::Exposure::getMaxIris()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().exposure().getMaxIris();
}

void DefaultVideoSource::Imaging::Exposure::setMaxIris(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().exposure().setMaxIris(value);
}

uint32_t DefaultVideoSource::Imaging::Exposure::getExposureTime()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().exposure().getExposureTime();
}

void DefaultVideoSource::Imaging::Exposure::setExposureTime(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().exposure().setExposureTime(value);
}

uint32_t DefaultVideoSource::Imaging::Exposure::getGain()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().exposure().getGain();
}

void DefaultVideoSource::Imaging::Exposure::setGain(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().exposure().setGain(value);
}

uint32_t DefaultVideoSource::Imaging::Exposure::getIris()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().exposure().getIris();
}

void DefaultVideoSource::Imaging::Exposure::setIris(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().exposure().setIris(value);
}

//////////////////////////////////////////////////////////////////////////////
// DefaultVideoSource::Imaging::WhiteBalance
//////////////////////////////////////////////////////////////////////////////

DefaultVideoSource::Imaging::WhiteBalance::WhiteBalance(Imaging& imaging)
  : VideoSource::Imaging::WhiteBalance(dynamic_cast<VideoSource::Imaging&>(imaging))
{
}

DefaultVideoSource::Imaging::WhiteBalance::~WhiteBalance()
{
}

WhiteBalanceMode DefaultVideoSource::Imaging::WhiteBalance::getMode()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().whitebalance().getMode();
}

void DefaultVideoSource::Imaging::WhiteBalance::setMode(WhiteBalanceMode value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().whitebalance().setMode(value);
}

uint32_t DefaultVideoSource::Imaging::WhiteBalance::getCrGain()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().whitebalance().getCrGain();
}

void DefaultVideoSource::Imaging::WhiteBalance::setCrGain(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().whitebalance().setCrGain(value);
}

uint32_t DefaultVideoSource::Imaging::WhiteBalance::getCbGain()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().whitebalance().getCbGain();
}

void DefaultVideoSource::Imaging::WhiteBalance::setCbGain(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().whitebalance().setCbGain(value);
}

uint32_t DefaultVideoSource::Imaging::WhiteBalance::getRGain()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().whitebalance().getRGain();
}

void DefaultVideoSource::Imaging::WhiteBalance::setRGain(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().whitebalance().setRGain(value);
}

uint32_t DefaultVideoSource::Imaging::WhiteBalance::getGGain()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().whitebalance().getGGain();
}

void DefaultVideoSource::Imaging::WhiteBalance::setGGain(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().whitebalance().setGGain(value);
}

uint32_t DefaultVideoSource::Imaging::WhiteBalance::getBGain()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().whitebalance().getBGain();
}

void DefaultVideoSource::Imaging::WhiteBalance::setBGain(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().whitebalance().setBGain(value);
}

//////////////////////////////////////////////////////////////////////////////
// DefaultVideoSource::Imaging::WideDynamicRange
//////////////////////////////////////////////////////////////////////////////

DefaultVideoSource::Imaging::WideDynamicRange::WideDynamicRange(Imaging& imaging)
  : VideoSource::Imaging::WideDynamicRange(dynamic_cast<VideoSource::Imaging&>(imaging))
{
}

DefaultVideoSource::Imaging::WideDynamicRange::~WideDynamicRange()
{
}

WDRMode DefaultVideoSource::Imaging::WideDynamicRange::getMode()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().widedynamicrange().getMode();
}

void DefaultVideoSource::Imaging::WideDynamicRange::setMode(WDRMode value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().widedynamicrange().setMode(value);
}

uint32_t DefaultVideoSource::Imaging::WideDynamicRange::getLevel()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().widedynamicrange().getLevel();
}

void DefaultVideoSource::Imaging::WideDynamicRange::setLevel(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().widedynamicrange().setLevel(value);
}

//////////////////////////////////////////////////////////////////////////////
// DefaultVideoSource::Imaging::LDC
//////////////////////////////////////////////////////////////////////////////

DefaultVideoSource::Imaging::LDC::LDC(Imaging& imaging)
  : VideoSource::Imaging::LDC(dynamic_cast<VideoSource::Imaging&>(imaging))
{
}

DefaultVideoSource::Imaging::LDC::~LDC()
{
}

LDCMode DefaultVideoSource::Imaging::LDC::getMode()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().ldc().getMode();
}

void DefaultVideoSource::Imaging::LDC::setMode(LDCMode value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().ldc().setMode(value);
}

uint32_t DefaultVideoSource::Imaging::LDC::getRatio()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().ldc().getRatio();
}

void DefaultVideoSource::Imaging::LDC::setRatio(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().ldc().setRatio(value);
}

//////////////////////////////////////////////////////////////////////////////
// DefaultVideoSource::Imaging::Gamma
//////////////////////////////////////////////////////////////////////////////

DefaultVideoSource::Imaging::Gamma::Gamma(Imaging& imaging)
  : VideoSource::Imaging::Gamma(dynamic_cast<VideoSource::Imaging&>(imaging))
{
}

DefaultVideoSource::Imaging::Gamma::~Gamma()
{
}

GammaCurveData& DefaultVideoSource::Imaging::Gamma::getCurveData()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().gamma().getCurveData();
}

void DefaultVideoSource::Imaging::Gamma::setCurveData(GammaCurveData& value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().gamma().setCurveData(value);
}

//////////////////////////////////////////////////////////////////////////////
// DefaultVideoSource::Imaging::NoiseReduction
//////////////////////////////////////////////////////////////////////////////

DefaultVideoSource::Imaging::NoiseReduction::NoiseReduction(Imaging& imaging)
  : VideoSource::Imaging::NoiseReduction(dynamic_cast<VideoSource::Imaging&>(imaging))
{
}

DefaultVideoSource::Imaging::NoiseReduction::~NoiseReduction()
{
}

VNRMode DefaultVideoSource::Imaging::NoiseReduction::getMode()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().noisereduction().getMode();
}

void DefaultVideoSource::Imaging::NoiseReduction::setMode(VNRMode value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().noisereduction().setMode(value);
}

uint32_t DefaultVideoSource::Imaging::NoiseReduction::getLevel()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().noisereduction().getLevel();
}

void DefaultVideoSource::Imaging::NoiseReduction::setLevel(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().noisereduction().setLevel(value);
}

NrParamTable& DefaultVideoSource::Imaging::NoiseReduction::getParamTable()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().noisereduction().getParamTable();
}

void DefaultVideoSource::Imaging::NoiseReduction::setParamTable(NrParamTable& value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(imaging().videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().noisereduction().setParamTable(value);
}

//////////////////////////////////////////////////////////////////////////////
// DefaultVideoSource::Imaging
//////////////////////////////////////////////////////////////////////////////

DefaultVideoSource::Imaging::Imaging(DefaultVideoSource& videosource)
  : VideoSource::Imaging(dynamic_cast<VideoSource&>(videosource))
{
}

DefaultVideoSource::Imaging::~Imaging()
{
}

bool DefaultVideoSource::Imaging::getMirror()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().getMirror();
}

void DefaultVideoSource::Imaging::setMirror(bool value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().setMirror(value);
}

bool DefaultVideoSource::Imaging::getFlip()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().getFlip();
}

void DefaultVideoSource::Imaging::setFlip(bool value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().setFlip(value);
}

uint32_t DefaultVideoSource::Imaging::getBrightness()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().getBrightness();
}

void DefaultVideoSource::Imaging::setBrightness(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().setBrightness(value);
}

uint32_t DefaultVideoSource::Imaging::getContrast()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().getContrast();
}

void DefaultVideoSource::Imaging::setContrast(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().setContrast(value);
}

uint32_t DefaultVideoSource::Imaging::getChroma()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().getChroma();
}

void DefaultVideoSource::Imaging::setChroma(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().setChroma(value);
}

uint32_t DefaultVideoSource::Imaging::getSaturation()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().getSaturation();
}

void DefaultVideoSource::Imaging::setSaturation(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().setSaturation(value);
}

uint32_t DefaultVideoSource::Imaging::getSharpness()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	return source->imaging().getSharpness();
}

void DefaultVideoSource::Imaging::setSharpness(uint32_t value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(property_not_implemented);
	source->imaging().setSharpness(value);
}

IrCutFilterMode DefaultVideoSource::Imaging::getIrCutFilterMode()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(interface_not_implemented);
	return source->imaging().getIrCutFilterMode();
}

void DefaultVideoSource::Imaging::setIrCutFilterMode(IrCutFilterMode value)
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(interface_not_implemented);
	return source->imaging().setIrCutFilterMode(value);
}

VideoSource::Imaging::AntiFlicker& DefaultVideoSource::Imaging::antiflicker()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(interface_not_implemented);
	return source->imaging().antiflicker();
}

VideoSource::Imaging::Backlight& DefaultVideoSource::Imaging::backlight()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(interface_not_implemented);
	return source->imaging().backlight();
}

VideoSource::Imaging::Focus& DefaultVideoSource::Imaging::focus()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(interface_not_implemented);
	return source->imaging().focus();
}

VideoSource::Imaging::Exposure& DefaultVideoSource::Imaging::exposure()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(interface_not_implemented);
	return source->imaging().exposure();
}

VideoSource::Imaging::WhiteBalance& DefaultVideoSource::Imaging::whitebalance()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(interface_not_implemented);
	return source->imaging().whitebalance();
}

VideoSource::Imaging::WideDynamicRange& DefaultVideoSource::Imaging::widedynamicrange()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(interface_not_implemented);
	return source->imaging().widedynamicrange();
}

VideoSource::Imaging::LDC& DefaultVideoSource::Imaging::ldc()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(interface_not_implemented);
	return source->imaging().ldc();
}

VideoSource::Imaging::Gamma& DefaultVideoSource::Imaging::gamma()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(interface_not_implemented);
	return source->imaging().gamma();
}

VideoSource::Imaging::NoiseReduction& DefaultVideoSource::Imaging::noisereduction()
{
	DefaultVideoSource* source = DEFAULT_VIDEO_SOURCE(dynamic_cast<DefaultVideoSource&>(videoSource()).source());
	if (source == NULL) throw IpcamError(interface_not_implemented);
	return source->imaging().noisereduction();
}

//////////////////////////////////////////////////////////////////////////////
// DefaultVideoSource
//////////////////////////////////////////////////////////////////////////////

DefaultVideoSource::DefaultVideoSource(DefaultVideoSource* source)
  : VideoElement(source)
{
}

DefaultVideoSource::~DefaultVideoSource()
{
}

uint32_t DefaultVideoSource::getFrameRate()
{
	VideoSource* vs = VIDEO_SOURCE(source());
	if (vs == NULL) throw IpcamError(property_not_implemented);
	return vs->getFrameRate();
}

void DefaultVideoSource::setFrameRate(uint32_t value)
{
	VideoSource* vs = VIDEO_SOURCE(source());
	if (vs == NULL) throw IpcamError(property_not_implemented);
	vs->setFrameRate(value);
}

Resolution DefaultVideoSource::getResolution()
{
	VideoSource* vs = VIDEO_SOURCE(source());
	if (vs == NULL) throw IpcamError(property_not_implemented);
	return vs->getResolution();
}

void DefaultVideoSource::setResolution(Resolution value)
{
	VideoSource* vs = VIDEO_SOURCE(source());
	if (vs == NULL) throw IpcamError(property_not_implemented);
	vs->setResolution(value);
}

VideoSource::Imaging& DefaultVideoSource::imaging()
{
	VideoSource* vs = VIDEO_SOURCE(source());
	if (vs == NULL) throw IpcamError(property_not_implemented);
	return vs->imaging();
}

Resolution DefaultVideoSource::resolution()
{
	return getResolution();
}

uint32_t DefaultVideoSource::framerate()
{
	return getFrameRate();
}


} // namespace Media
} // namespace Ipcam