/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-ircut.h
 * Copyright (C) 2018 Watson Xu <xuhuashan@gmail.com>
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

#ifndef _HIMPP_IRCUT_H_
#define _HIMPP_IRCUT_H_

#include <ev++.h>
#include <himpp-base.h>
#include <video-source.h>

class HimppIrCut;

#define HIMPP_IRCUT(o)		dynamic_cast<HimppIrCut*>(o)

class IODevice
{
public:
	enum DIR { IN, OUT };

	IODevice();
	IODevice(const char* name, DIR dir);
	virtual ~IODevice();

	bool	is_open() { return filedesc >= 0; }
	int		initialize(const char* name, DIR dir);
	int		setValue(int value);
	int		getValue(int& value);

private:
	int		filedesc;
};

class HimppIrCut: public HimppVideoElement, public DefaultVideoSource
{
public:
	class Imaging : public DefaultVideoSource::Imaging
	{
	public:
		class IrCutFilter : public DefaultVideoSource::Imaging::IrCutFilter
		{
		public:
			IrCutFilter(Imaging& imaging);
			virtual ~IrCutFilter();

			IrCutFilterMode	getMode();
			void			setMode(IrCutFilterMode value);
			int32_t			getThreshold();
			void			setThreshold(int32_t value);
			uint32_t		getHysteresis();
			void			setHysteresis(uint32_t value);
			uint32_t		getBrightness();
			void			setBrightness(uint32_t value);
		};
	public:
		Imaging(HimppIrCut& ircut);
		~Imaging();

		// implementation of VideoSource::Imaging
		VideoSource::Imaging::IrCutFilter& ircutfilter();
	public:
		IrCutFilter		_ircutfilter;
	};
public:
	HimppIrCut(HimppVideoElement* source, std::unordered_map<std::string,std::string> params);
	virtual ~HimppIrCut();

	// implementation of HimppVideoElement
	MPP_CHN_S* bindSource();

	// implementation of VideoSource
	VideoSource::Imaging& imaging();

	IrCutFilterMode		getMode();
	void				setMode(IrCutFilterMode value);
	int32_t				getThreshold();
	void				setThreshold(int32_t value);
	uint32_t			getHysteresis();
	void				setHysteresis(uint32_t value);
	uint32_t			getBrightness();
	void				setBrightness(uint32_t value);

protected:
	void doEnableElement();
	void doDisableElement();

private:
	void sensor_timer_handler(ev::timer& w, int revents);
	void ircut_timer_handler(ev::timer& w, int revents);
	void ircut_on();
	void ircut_off();

private:
	Imaging				_imaging;

	IrCutFilterMode 	_mode;
	int32_t				_threshold;
	uint32_t			_hysteresis;
	uint32_t			_brightness;
	int					_debounce_count;
	IrCutFilterMode		_status;
	IODevice			_sensor_dev;		// Sensor ADC input
	IODevice			_ircutp_dev;		// IRCUT+
	IODevice			_ircutn_dev;		// IRCUT-
	IODevice			_irleden_dev;		// IRLED enable
	IODevice			_irledbr_dev;		// IRLED brightness
	ev::timer			_sensor_timer;
	ev::timer			_ircut_timer;
};

#endif // _HIMPP_IRCUT_H_

