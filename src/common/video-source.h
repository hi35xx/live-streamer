/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * video-source.h
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

#ifndef _VIDEO_SOURCE_H_
#define _VIDEO_SOURCE_H_

#include <media-common.h>
#include <media-element.h>

namespace Ipcam {
namespace Media {

enum AntiFlickerMode { ANTIFLICKER_OFF, ANTIFLICKER_NORMAL, ANTIFLICKER_AUTO };
enum BLCMode { BACKLIGHT_OFF, BACKLIGHT_ON };
enum FocusMode { AUTO_FOCUS, MANUAL_FOCUS };
enum ExposureMode { AUTO_EXPOSURE, MANUAL_EXPOSURE };
enum ExposurePriority { LOWNOISE_PRIORITY, FRAMERATE_PRIORITY };
enum WhiteBalanceMode { AUTO_WB, MANUAL_WB };
enum WDRMode { WDR_OFF, WDR_ON };
enum LDCMode { LDC_OFF, LDC_ON };
enum IrCutFilterMode { IRCUT_ON, IRCUT_OFF, IRCUT_AUTO };
enum VNRMode { VNR_OFF, VNR_MANUAL, VNR_AUTO };	   // Video Noise Reduction Mode

typedef std::vector<uint32_t> GammaCurveData;

typedef DBus::Struct<uint32_t, int32_t, int32_t>	NrParamTableItem;
typedef std::vector<NrParamTableItem>				NrParamTable;

class VideoSource;
class DefaultVideoSource;

#define VIDEO_SOURCE(p)			dynamic_cast<Ipcam::Media::VideoSource*>(p)
#define DEFAULT_VIDEO_SOURCE(p)	dynamic_cast<Ipcam::Media::DefaultVideoSource*>(p)

// Control interface for video source
class VideoSource
{
public:
	class Imaging
	{
	public:
		// Anti-Flicker
		class AntiFlicker
		{
		public:
			AntiFlicker(Imaging& imaging);
			virtual ~AntiFlicker();
			virtual AntiFlickerMode	getMode();
			virtual void			setMode(AntiFlickerMode value);
			virtual uint32_t		getFrequency();
			virtual void			setFrequency(uint32_t value);

			Imaging& imaging() { return _imaging; }
		private:
			Imaging& _imaging;
		};
		// Backlight Compensation
		class Backlight
		{
		public:
			Backlight(Imaging& imaging);
			virtual ~Backlight();
			virtual BLCMode			getMode();
			virtual void			setMode(BLCMode value);
			virtual uint32_t		getLevel();
			virtual void			setLevel(uint32_t value);

			Imaging& imaging() { return _imaging; }
		private:
			Imaging& _imaging;
		};
		// Focus
		class Focus
		{
		public:
			Focus(Imaging& imaging);
			virtual ~Focus();
			virtual FocusMode		getMode();
			virtual void			setMode(FocusMode value);
			virtual uint32_t		getDefaultSpeed();
			virtual void			setDefaultSpeed(uint32_t value);
			virtual uint32_t		getNearLimit();
			virtual void			setNearLimit(uint32_t value);
			virtual uint32_t		getFarLimit();
			virtual void			setFarLimit(uint32_t value);

			Imaging& imaging() { return _imaging; }
		private:
			Imaging& _imaging;
		};
		// Exposure
		class Exposure
		{
		public:
			Exposure(Imaging& imaging);
			virtual ~Exposure();
			virtual ExposureMode	getMode();
			virtual void			setMode(ExposureMode value);
			virtual ExposurePriority getPriority();
			virtual void			setPriority(ExposurePriority value);
			virtual uint32_t		getMinExposureTime();
			virtual void			setMinExposureTime(uint32_t value);
			virtual uint32_t		getMaxExposureTime();
			virtual void			setMaxExposureTime(uint32_t value);
			virtual uint32_t		getMinGain();
			virtual void			setMinGain(uint32_t value);
			virtual uint32_t		getMaxGain();
			virtual void			setMaxGain(uint32_t value);
			virtual uint32_t		getMinIris();
			virtual void			setMinIris(uint32_t value);
			virtual uint32_t		getMaxIris();
			virtual void			setMaxIris(uint32_t value);
			virtual uint32_t		getExposureTime();
			virtual void			setExposureTime(uint32_t value);
			virtual uint32_t		getCompensation();
			virtual void			setCompensation(uint32_t value);
			virtual uint32_t		getGain();
			virtual void			setGain(uint32_t value);
			virtual uint32_t		getIris();
			virtual void			setIris(uint32_t value);

			Imaging& imaging() { return _imaging; }
		private:
			Imaging& _imaging;
		};
		// White Balance
		class WhiteBalance
		{
		public:
			WhiteBalance(Imaging& imaging);
			virtual ~WhiteBalance();
			virtual WhiteBalanceMode getMode();
			virtual void			setMode(WhiteBalanceMode value);
			virtual uint32_t		getCrGain();
			virtual void			setCrGain(uint32_t value);
			virtual uint32_t		getCbGain();
			virtual void			setCbGain(uint32_t value);
			virtual uint32_t		getRGain();
			virtual void			setRGain(uint32_t value);
			virtual uint32_t		getGGain();
			virtual void			setGGain(uint32_t value);
			virtual uint32_t		getBGain();
			virtual void			setBGain(uint32_t value);

			Imaging& imaging() { return _imaging; }
		private:
			Imaging& _imaging;
		};
		// Wide Dynamic Range
		class WideDynamicRange
		{
		public:
			WideDynamicRange(Imaging& imaging);
			virtual ~WideDynamicRange();
			virtual WDRMode			getMode();
			virtual void			setMode(WDRMode value);
			virtual uint32_t		getLevel();
			virtual void			setLevel(uint32_t value);

			Imaging& imaging() { return _imaging; }
		private:
			Imaging& _imaging;
		};
		// Lens Distortion Correction
		class LDC
		{
		public:
			LDC(Imaging& imaging);
			virtual ~LDC();
			virtual LDCMode			getMode();
			virtual void			setMode(LDCMode value);
			virtual uint32_t		getRatio();
			virtual void			setRatio(uint32_t value);

			Imaging& imaging() { return _imaging; }
		private:
			Imaging& _imaging;
		};
		class Gamma
		{
		public:
			Gamma(Imaging& imaging);
			virtual ~Gamma();
			virtual GammaCurveData&	getCurveData();
			virtual void			setCurveData(GammaCurveData& value);

			Imaging& imaging() { return _imaging; }
		private:
			Imaging& _imaging;
		};
		// NoiseReduction
		class NoiseReduction
		{
		public:
			NoiseReduction(Imaging& imaging);
			virtual ~NoiseReduction();
			virtual VNRMode			getMode();
			virtual void			setMode(VNRMode value);
			virtual uint32_t		getLevel();
			virtual void			setLevel(uint32_t value);
			virtual NrParamTable&	getParamTable();
			virtual void			setParamTable(NrParamTable& value);

			Imaging& imaging() { return _imaging; }
		private:
			Imaging& _imaging;
		};

	public:
		Imaging(VideoSource& videosource);
		virtual ~Imaging();

		virtual bool				getMirror();
		virtual void				setMirror(bool value);
		virtual bool				getFlip();
		virtual void				setFlip(bool value);
		virtual uint32_t			getBrightness();
		virtual void				setBrightness(uint32_t value);
		virtual uint32_t			getContrast();
		virtual void				setContrast(uint32_t value);
		virtual uint32_t			getChroma();
		virtual void				setChroma(uint32_t value);
		virtual uint32_t			getSaturation();
		virtual void				setSaturation(uint32_t value);
		virtual uint32_t			getSharpness();
		virtual void				setSharpness(uint32_t value);
		virtual IrCutFilterMode		getIrCutFilterMode();
		virtual void				setIrCutFilterMode(IrCutFilterMode value);

		virtual AntiFlicker&		antiflicker();
		virtual Backlight&			backlight();
		virtual Focus&				focus();
		virtual Exposure&			exposure();
		virtual WhiteBalance&		whitebalance();
		virtual WideDynamicRange&	widedynamicrange();
		virtual LDC&				ldc();
		virtual Gamma&				gamma();
		virtual NoiseReduction&		noisereduction();

		VideoSource& videoSource() { return _video_source; }
	private:
		VideoSource& _video_source;
	};

public:
	virtual ~VideoSource();
	virtual uint32_t				getFrameRate();
	virtual void					setFrameRate(uint32_t value);
	virtual Resolution				getResolution();
	virtual void					setResolution(Resolution value);

	virtual Imaging&				imaging();

protected:
	VideoSource();
};

// Default VideoSource implementation
//   every call to this class will be hand over to it's upstream object,
//   except the derived-class implement the functionality.

class DefaultVideoSource : virtual public VideoElement, public VideoSource
{
public:
	class Imaging : public VideoSource::Imaging
	{
	public:
		// Anti-Flicker
		class AntiFlicker : public VideoSource::Imaging::AntiFlicker
		{
		public:
			AntiFlicker(Imaging& imaging);
			virtual ~AntiFlicker();
			virtual AntiFlickerMode	getMode();
			virtual void			setMode(AntiFlickerMode value);
			virtual uint32_t		getFrequency();
			virtual void			setFrequency(uint32_t value);
		};
		// Backlight Compensation
		class Backlight : public VideoSource::Imaging::Backlight
		{
		public:
			Backlight(Imaging& imaging);
			virtual ~Backlight();
			virtual BLCMode			getMode();
			virtual void			setMode(BLCMode value);
			virtual uint32_t		getLevel();
			virtual void			setLevel(uint32_t value);
		};
		// Focus
		class Focus : public VideoSource::Imaging::Focus
		{
		public:
			Focus(Imaging& imaging);
			virtual ~Focus();
			virtual FocusMode		getMode();
			virtual void			setMode(FocusMode value);
			virtual uint32_t		getDefaultSpeed();
			virtual void			setDefaultSpeed(uint32_t value);
			virtual uint32_t		getNearLimit();
			virtual void			setNearLimit(uint32_t value);
			virtual uint32_t		getFarLimit();
			virtual void			setFarLimit(uint32_t value);
		};
		// Exposure
		class Exposure : public VideoSource::Imaging::Exposure
		{
		public:
			Exposure(Imaging& imaging);
			virtual ~Exposure();
			virtual ExposureMode	getMode();
			virtual void			setMode(ExposureMode value);
			virtual ExposurePriority getPriority();
			virtual void			setPriority(ExposurePriority value);
			virtual uint32_t		getMinExposureTime();
			virtual void			setMinExposureTime(uint32_t value);
			virtual uint32_t		getMaxExposureTime();
			virtual void			setMaxExposureTime(uint32_t value);
			virtual uint32_t		getMinGain();
			virtual void			setMinGain(uint32_t value);
			virtual uint32_t		getMaxGain();
			virtual void			setMaxGain(uint32_t value);
			virtual uint32_t		getMinIris();
			virtual void			setMinIris(uint32_t value);
			virtual uint32_t		getMaxIris();
			virtual void			setMaxIris(uint32_t value);
			virtual uint32_t		getCompensation();
			virtual void			setCompensation(uint32_t value);
			virtual uint32_t		getExposureTime();
			virtual void			setExposureTime(uint32_t value);
			virtual uint32_t		getGain();
			virtual void			setGain(uint32_t value);
			virtual uint32_t		getIris();
			virtual void			setIris(uint32_t value);
		};
		// White Balance
		class WhiteBalance : public VideoSource::Imaging::WhiteBalance
		{
		public:
			WhiteBalance(Imaging& imaging);
			virtual ~WhiteBalance();
			virtual WhiteBalanceMode getMode();
			virtual void			setMode(WhiteBalanceMode value);
			virtual uint32_t		getCrGain();
			virtual void			setCrGain(uint32_t value);
			virtual uint32_t		getCbGain();
			virtual void			setCbGain(uint32_t value);
			virtual uint32_t		getRGain();
			virtual void			setRGain(uint32_t value);
			virtual uint32_t		getGGain();
			virtual void			setGGain(uint32_t value);
			virtual uint32_t		getBGain();
			virtual void			setBGain(uint32_t value);
		};
		// Wide Dynamic Range
		class WideDynamicRange : public VideoSource::Imaging::WideDynamicRange
		{
		public:
			WideDynamicRange(Imaging& imaging);
			virtual ~WideDynamicRange();
			virtual WDRMode			getMode();
			virtual void			setMode(WDRMode value);
			virtual uint32_t		getLevel();
			virtual void			setLevel(uint32_t value);
		};
		// Lens Distortion Correction
		class LDC : public VideoSource::Imaging::LDC
		{
		public:
			LDC(Imaging& imaging);
			virtual ~LDC();
			virtual LDCMode			getMode();
			virtual void			setMode(LDCMode value);
			virtual uint32_t		getRatio();
			virtual void			setRatio(uint32_t value);
		};
		// Gamma
		class Gamma : public VideoSource::Imaging::Gamma
		{
		public:
			Gamma(Imaging& imaging);
			virtual ~Gamma();
			virtual GammaCurveData&	getCurveData();
			virtual void			setCurveData(GammaCurveData& value);
		};
		// NoiseReduction
		class NoiseReduction : public VideoSource::Imaging::NoiseReduction
		{
		public:
			NoiseReduction(Imaging& imaging);
			virtual ~NoiseReduction();
			virtual VNRMode			getMode();
			virtual void			setMode(VNRMode value);
			virtual uint32_t		getLevel();
			virtual void			setLevel(uint32_t value);
			virtual NrParamTable&	getParamTable();
			virtual void			setParamTable(NrParamTable& value);
		};

	public:
		Imaging(DefaultVideoSource& videosource);
		virtual ~Imaging();

		virtual bool				getMirror();
		virtual void				setMirror(bool value);
		virtual bool				getFlip();
		virtual void				setFlip(bool value);
		virtual uint32_t			getBrightness();
		virtual void				setBrightness(uint32_t value);
		virtual uint32_t			getContrast();
		virtual void				setContrast(uint32_t value);
		virtual uint32_t			getChroma();
		virtual void				setChroma(uint32_t value);
		virtual uint32_t			getSaturation();
		virtual void				setSaturation(uint32_t value);
		virtual uint32_t			getSharpness();
		virtual void				setSharpness(uint32_t value);
		virtual IrCutFilterMode		getIrCutFilterMode();
		virtual void				setIrCutFilterMode(IrCutFilterMode value);

		virtual VideoSource::Imaging::AntiFlicker&		antiflicker();
		virtual VideoSource::Imaging::Backlight&		backlight();
		virtual VideoSource::Imaging::Focus&			focus();
		virtual VideoSource::Imaging::Exposure&			exposure();
		virtual VideoSource::Imaging::WhiteBalance&		whitebalance();
		virtual VideoSource::Imaging::WideDynamicRange&	widedynamicrange();
		virtual VideoSource::Imaging::LDC&				ldc();
		virtual VideoSource::Imaging::Gamma&			gamma();
		virtual VideoSource::Imaging::NoiseReduction&	noisereduction();
	};

public:
	virtual ~DefaultVideoSource();
	virtual uint32_t				getFrameRate();
	virtual void					setFrameRate(uint32_t value);
	virtual Resolution				getResolution();
	virtual void					setResolution(Resolution value);

	// derrived from VideoElement
	virtual Resolution				resolution();
	virtual uint32_t				framerate();

	virtual VideoSource::Imaging&	imaging();

protected:
	DefaultVideoSource(DefaultVideoSource* source);
};

} // namespace Media
} // namespace Ipcam

#endif // _VIDEO_SOURCE_H_

