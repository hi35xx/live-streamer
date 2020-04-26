/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-viu.h
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

#ifndef _HIMPP_VIDEO_VIU_H_
#define _HIMPP_VIDEO_VIU_H_

#include <himpp-base.h>
#include <video-source.h>

class HimppViDev;
class HimppViChan;

#define HIMPP_VI_DEV(o)		dynamic_cast<HimppViDev*>(o)
#define HIMPP_VI_CHAN(o)	dynamic_cast<HimppViChan*>(o)

class HimppViDev : public HimppVideoElement, public DefaultVideoSource
{
public:
	HimppViDev(HimppVideoElement* source, VI_DEV devid, std::string sensor);
	~HimppViDev();

	// implementation of HimppVideoElement
	//MPP_CHN_S* bindSource();

	// implementation of VideoSource
	virtual uint32_t	getFrameRate();
	virtual void		setFrameRate(uint32_t value);
	virtual Resolution	getResolution();
	virtual void		setResolution(Resolution value);

	HimppVideoSensor*	sensor() { return _video_sensor; }
	VI_DEV deviceId() { return _devid; }

	VI_VPSS_MODE_E		getOnlineMode() { return _online_mode; }
	void				setOnlineMode(VI_VPSS_MODE_E mode) { _online_mode = mode; }

protected:
	void doEnableElement();
	void doDisableElement();

private:
	bool initializeMipi();
	void cleanupMipi();

private:
	HimppVideoSensor*   _video_sensor;
	VI_DEV				_devid;
	VI_VPSS_MODE_E		_online_mode;
	Resolution			_resolution;
	uint32_t			_framerate;
};

class HimppViChan : public HimppVideoElement, public DefaultVideoSource
{
public:
	class Imaging : public DefaultVideoSource::Imaging
	{
	public:
		class LDC : public DefaultVideoSource::Imaging::LDC
		{
		public:
			LDC(Imaging& imaging);
			~LDC();

			// implementation of VideoSource::Imaging::LDC
			LDCMode		getMode();
			void		setMode(LDCMode value);
			int32_t		getRatio();
			void		setRatio(int32_t value);
		public:
			LDCMode		_ldc_mode;
			int32_t		_ldc_ratio;
		};
	public:
		Imaging(HimppViChan& vichan);
		~Imaging();

		// implementation of VideoSource::Imaging
		bool			getMirror();
		void			setMirror(bool value);
		bool			getFlip();
		void			setFlip(bool value);
		uint32_t		getRotateAngle();
		void			setRotateAngle(uint32_t value);

		VideoSource::Imaging::LDC& ldc();
	public:
		LDC				_ldc;
		bool			_mirror;
		bool			_flip;
		ROTATION_E		_rotate;
	};

public:
	HimppViChan(HimppVideoElement* source, VI_CHN chnid);
	~HimppViChan();

	// implementation of HimppVideoElement
	MPP_CHN_S* bindSource();

	// implementation of VideoSource
	VideoSource::Imaging& imaging();
	virtual uint32_t	getFrameRate();
	virtual void		setFrameRate(uint32_t value);
	virtual Resolution	getResolution();
	virtual void		setResolution(Resolution value);

	virtual void		setCropOffset(int32_t x, int32_t y);

	VI_CHN channelId() { return _chnid; }

protected:
	void doEnableElement();
	void doDisableElement();

private:
	Imaging			_imaging;
	VI_CHN			_chnid;
	MPP_CHN_S		_mpp_chn;
	Resolution		_resolution;
	uint32_t		_framerate;
	int32_t			_xoffset;
	int32_t			_yoffset;
};

#endif // _HIMPP_VIDEO_VIU_H_
