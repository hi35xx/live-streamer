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
	class Imaging : public DefaultVideoSource::Imaging
	{
	public:
		Imaging(HimppViDev& videv);
		~Imaging();

		// implementation of VideoSource::Imaging
		uint32_t		getBrightness();
		void			setBrightness(uint32_t value);
		uint32_t		getContrast();
		void			setContrast(uint32_t value);
		uint32_t		getChroma();
		void			setChroma(uint32_t value);
		uint32_t		getSaturation();
		void			setSaturation(uint32_t value);
	private:
		friend class HimppViDev;
		uint32_t		_brightness;
		uint32_t		_contrast;
		uint32_t		_chroma;
		uint32_t		_saturation;
	};

public:
	HimppViDev(HimppVideoElement* source, VI_DEV devid);
	~HimppViDev();

	// implementation of HimppVideoElement
	//MPP_CHN_S* bindSource();
	virtual Resolution	resolution();
	virtual uint32_t	framerate();

	// implementation of VideoSource
	virtual uint32_t	getFrameRate();
	virtual void		setFrameRate(uint32_t value);
	virtual Resolution	getResolution();
	virtual void		setResolution(Resolution value);
	VideoSource::Imaging& imaging();

	VI_DEV deviceId() { return _devid; }

protected:
	void doEnableElement();
	void doDisableElement();

private:
	Imaging				_imaging;
	VI_DEV				_devid;
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
			uint32_t	getRatio();
			void		setRatio(uint32_t value);
		public:
			LDCMode		_ldc_mode;
			uint32_t	_ldc_ratio;
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
		ROTATE_E		_rotate;
	};

public:
	HimppViChan(HimppVideoElement* source, VI_CHN chnid);
	~HimppViChan();

	// implementation of HimppVideoElement
	MPP_CHN_S* bindSource();
	virtual Resolution	resolution();
	virtual uint32_t	framerate();

	// implementation of VideoSource
	VideoSource::Imaging& imaging();
	virtual uint32_t	getFrameRate();
	virtual void		setFrameRate(uint32_t value);
	virtual Resolution	getResolution();
	virtual void		setResolution(Resolution value);

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
};

#endif // _HIMPP_VIDEO_VIU_H_
