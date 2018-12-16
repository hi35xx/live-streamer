/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-isp.h
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

#ifndef _HIMPP_VIDEO_ISP_H_
#define _HIMPP_VIDEO_ISP_H_

#include <string>
#include <pthread.h>
#include <himpp-base.h>
#include <video-source.h>

class HimppVideoSensor;
class HimppVideoISP;

struct hiVI_DEV_ATTR_S;

#define HIMPP_VIDEO_ISP(o)		dynamic_cast<HimppVideoISP*>(o)

typedef HI_VOID (*SENSOR_INIT_FUNC)(void);
typedef HI_S32  (*SENSOR_REGISTER_FUNC)(void);
typedef HI_S32  (*SENSOR_UNREGISTER_FUNC)(void);

struct HimppSensorModule
{
	void*					handle;
	SENSOR_INIT_FUNC		sensor_init;
	SENSOR_REGISTER_FUNC	sensor_register;
	SENSOR_UNREGISTER_FUNC	sensor_unregister;
};

class HimppVideoISP: public HimppVideoElement, public DefaultVideoSource 
{
public:
	class Imaging : public DefaultVideoSource::Imaging
	{
	public:
		// AntiFlicker
		class AntiFlicker : public DefaultVideoSource::Imaging::AntiFlicker
		{
		public:
			AntiFlicker(Imaging& imaging);
			~AntiFlicker();

			AntiFlickerMode		getMode();
			void				setMode(AntiFlickerMode value);
			uint32_t			getFrequency();
			void				setFrequency(uint32_t value);
		private:
			AntiFlickerMode		_mode;
			uint32_t			_frequency;
		};
		// Exposure
		class Exposure : public DefaultVideoSource::Imaging::Exposure
		{
		public:
			struct StateInfo {
				uint32_t		ExposureTime;
				uint32_t		AGain;
				uint32_t		DGain;
				uint32_t		ISPDGain;
				uint32_t		Exposure;
				uint32_t		Histogram5[5];
				uint32_t		AverageLuminance;
				uint32_t		FrameRate;
				uint32_t		ISO;
			};
		public:
			Exposure(Imaging& imaging);
			~Exposure();

			ExposureMode		getMode();
			void				setMode(ExposureMode value);
			ExposurePriority	getPriority();
			void				setPriority(ExposurePriority value);
			uint32_t			getMinExposureTime();
			void				setMinExposureTime(uint32_t value);
			uint32_t			getMaxExposureTime();
			void				setMaxExposureTime(uint32_t value);
			uint32_t			getMinGain();
			void				setMinGain(uint32_t value);
			uint32_t			getMaxGain();
			void				setMaxGain(uint32_t value);
			uint32_t			getMinIris();
			void				setMinIris(uint32_t value);
			uint32_t			getMaxIris();
			void				setMaxIris(uint32_t value);
			uint32_t			getCompensation();
			void				setCompensation(uint32_t value);
			uint32_t			getExposureTime();
			void				setExposureTime(uint32_t value);
			uint32_t			getGain();
			void				setGain(uint32_t value);
			uint32_t			getIris();
			void				setIris(uint32_t value);

			void				getStateInfo(StateInfo& info);
		private:
			ExposureMode		_mode;
			ExposurePriority	_priority;
			uint32_t			_min_exp_time;
			uint32_t			_max_exp_time;
			uint32_t			_min_gain;
			uint32_t			_max_gain;
			uint32_t			_min_iris;
			uint32_t			_max_iris;
			uint32_t			_exp_time;
			uint32_t			_compensation;
			uint32_t			_gain;
			uint32_t			_iris;
		};
		// WhiteBalance
		class WhiteBalance : public DefaultVideoSource::Imaging::WhiteBalance
		{
		public:
			WhiteBalance(Imaging& imaging);
			~WhiteBalance();

			void				setMode(WhiteBalanceMode value);
			WhiteBalanceMode	getMode();
			void				setCbGain(uint32_t value);
			uint32_t			getCbGain();
			void				setCrGain(uint32_t value);
			uint32_t			getCrGain();
			void				setRGain(uint32_t value);
			uint32_t			getRGain();
			void				setGGain(uint32_t value);
			uint32_t			getGGain();
			void				setBGain(uint32_t value);
			uint32_t			getBGain();
		private:
			WhiteBalanceMode	_mode;
			uint32_t			_cb_gain;
			uint32_t			_cr_gain;
			uint32_t			_r_gain;
			uint32_t			_g_gain;
			uint32_t			_b_gain;
		};
		// WideDynamicRange
		class WideDynamicRange : public DefaultVideoSource::Imaging::WideDynamicRange
		{
		public:
			WideDynamicRange(Imaging& imaging);
			~WideDynamicRange();

			void				setMode(WDRMode value);
			WDRMode				getMode();
			void				setLevel(uint32_t value);
			uint32_t			getLevel();
		private:
			WDRMode				_mode;
			uint32_t			_level;
		};
		// Gamma
		class Gamma : public DefaultVideoSource::Imaging::Gamma
		{
		public:
			Gamma(Imaging& imaging);
			~Gamma();

			void				setCurveData(GammaCurveData& value);
			GammaCurveData&		getCurveData();
		private:
			GammaCurveData		_curve_data;
		};

	public:
		Imaging(HimppVideoISP& video_isp);
		~Imaging();

		// implementation of VideoSource::Imaging
		VideoSource::Imaging::AntiFlicker&		antiflicker();
		VideoSource::Imaging::Exposure&			exposure();
		VideoSource::Imaging::WhiteBalance&		whitebalance();
		VideoSource::Imaging::WideDynamicRange&	widedynamicrange();
		VideoSource::Imaging::Gamma&			gamma();

	private:
		AntiFlicker			_antiflicker;
		Exposure			_exposure;
		WhiteBalance		_whitebalance;
		WideDynamicRange	_widedynamicrange;
		Gamma				_gamma;
	};

public:
	HimppVideoISP(HimppVideoElement* source, std::string sensor);
	~HimppVideoISP();

	// implementation of VideoSource
	Resolution				getResolution();
	uint32_t				getFrameRate();
	VideoSource::Imaging&   imaging();

	hiVI_DEV_ATTR_S*		videoInputConfig();

	ISP_DEV					ispDev() { return _isp_dev; }

protected:
	void doEnableElement();
	void doDisableElement();

private:
	Imaging				_imaging;
	HimppVideoSensor*   _video_sensor;
	HimppSensorModule   _sensor_module;
	ISP_DEV				_isp_dev;
	pthread_t			_isp_thread;

	bool initializeMipi();
	bool loadSensorModule();
	bool unloadSensorModule();
	bool registerAlgorithm();
	bool unregisterAlgorithm();

	static void *isp_thread_routine(void *arg);
};

#endif // _HIMPP_VIDEO_ISP_H_

