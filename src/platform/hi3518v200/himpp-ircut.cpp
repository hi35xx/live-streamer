/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-ircut.cpp
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

#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unordered_map>

#include "himpp-ircut.h"


IODevice::IODevice()
  : filedesc(-1)
{
}

IODevice::IODevice(const char* name, DIR dir)
  : filedesc(-1)
{
	initialize(name, dir);
}

IODevice::~IODevice()
{
	if (filedesc >= 0)
		close(filedesc);
}

int IODevice::initialize(const char* name, DIR dir)
{
	char path[256];

	if (filedesc >= 0) {
		close(filedesc);
		filedesc = -1;
	}

	// GPIO device
	if(strncasecmp(name, "gpio", 4) == 0) {
		if (strlen(name) <= 4) return -1;

		unsigned long nr = strtoul(&name[4], NULL, 0);

		snprintf(path, sizeof(path), "/sys/class/gpio/gpio%lu/direction", nr);
		// export GPIO if necessary
		if (access(path, F_OK) != 0) {
			int efd = open("/sys/class/gpio/export", O_WRONLY);
			if (efd >= 0) {
				dprintf(efd, "%lu", nr);
				close(efd);
			}
		}

		int dfd = open(path, O_WRONLY);
		if (dfd >= 0) {
			const char *value = (dir == OUT) ? "out" : "in";
			write(dfd, value, strlen(value));
			close(dfd);
		}

		snprintf(path, sizeof(path), "/sys/class/gpio/gpio%lu/value", nr);
		int oflags = (dir == OUT) ? O_RDWR : O_RDONLY;
		filedesc = open(path, oflags);
		if (filedesc < 0) {
			fprintf(stderr, "Open GPIO device '%s[%s]' failed\n", name, path);
		}
		return filedesc;
	}

	// Otherwise, hwmon device
	snprintf(path, sizeof(path), "/sys/class/hwmon/%s", name);
	int oflags = (dir == OUT) ? O_RDWR : O_RDONLY;
	filedesc = open(path, oflags);
	if (filedesc < 0) {
		fprintf(stderr, "Open device '%s[%s]' failed\n", name, path);
	}
	return filedesc;
}

int IODevice::setValue(int value)
{
	if (filedesc < 0) return -1;

	return dprintf(filedesc, "%d", value);
}

int IODevice::getValue(int& value)
{
	if (filedesc < 0) return -1;

	char buf[32];
	lseek(filedesc, 0, SEEK_SET);
	int retval = read(filedesc, buf, sizeof(buf));
	if (retval > 0) {
		value = strtol(buf, NULL, 0);
	}

	return retval;
}


//////////////////////////////////////////////////////////////////////////////
// HimppIrCut::Imaging::IrCutFilter
//////////////////////////////////////////////////////////////////////////////

HimppIrCut::Imaging::IrCutFilter::IrCutFilter(Imaging& imaging)
  : DefaultVideoSource::Imaging::IrCutFilter(dynamic_cast<DefaultVideoSource::Imaging&>(imaging))
{
}

HimppIrCut::Imaging::IrCutFilter::~IrCutFilter()
{
}

IrCutFilterMode HimppIrCut::Imaging::IrCutFilter::getMode()
{
	return dynamic_cast<HimppIrCut&>(imaging().videoSource()).getMode();
}

void HimppIrCut::Imaging::IrCutFilter::setMode(IrCutFilterMode value)
{
	dynamic_cast<HimppIrCut&>(imaging().videoSource()).setMode(value);
}

int32_t HimppIrCut::Imaging::IrCutFilter::getThreshold()
{
	return dynamic_cast<HimppIrCut&>(imaging().videoSource()).getThreshold();
}

void HimppIrCut::Imaging::IrCutFilter::setThreshold(int32_t value)
{
	dynamic_cast<HimppIrCut&>(imaging().videoSource()).setThreshold(value);
}

uint32_t HimppIrCut::Imaging::IrCutFilter::getHysteresis()
{
	return dynamic_cast<HimppIrCut&>(imaging().videoSource()).getHysteresis();
}

void HimppIrCut::Imaging::IrCutFilter::setHysteresis(uint32_t value)
{
	dynamic_cast<HimppIrCut&>(imaging().videoSource()).setHysteresis(value);
}

uint32_t HimppIrCut::Imaging::IrCutFilter::getBrightness()
{
	return dynamic_cast<HimppIrCut&>(imaging().videoSource()).getBrightness();
}

void HimppIrCut::Imaging::IrCutFilter::setBrightness(uint32_t value)
{
	dynamic_cast<HimppIrCut&>(imaging().videoSource()).setBrightness(value);
}


//////////////////////////////////////////////////////////////////////////////
// HimppIrCut::Imaging
//////////////////////////////////////////////////////////////////////////////

HimppIrCut::Imaging::Imaging(HimppIrCut& ircut)
  : DefaultVideoSource::Imaging(dynamic_cast<DefaultVideoSource&>(ircut)),
    _ircutfilter(*this)
{
}

HimppIrCut::Imaging::~Imaging()
{
}

VideoSource::Imaging::IrCutFilter& HimppIrCut::Imaging::ircutfilter()
{
	return _ircutfilter;
}


//////////////////////////////////////////////////////////////////////////////
// HimppIrCut
//////////////////////////////////////////////////////////////////////////////

HimppIrCut::HimppIrCut(HimppVideoElement* source, std::unordered_map<std::string,std::string> params)
  : VideoElement(VIDEO_ELEMENT(source)),
    HimppVideoElement(source), DefaultVideoSource(DEFAULT_VIDEO_SOURCE(source)),
    _imaging(*this), _mode(IRCUT_AUTO), _threshold(100), _hysteresis(10), _brightness(255),
    _debounce_count(0), _status(IRCUT_OFF)
{
	_sensor_timer.set(1.0, 1.0);
	_sensor_timer.set<HimppIrCut, &HimppIrCut::sensor_timer_handler>(this);
	_ircut_timer.set(0.5, 0.0);
	_ircut_timer.set<HimppIrCut, &HimppIrCut::ircut_timer_handler>(this);
	_ircut_timer.start();

	std::unordered_map<std::string, std::string>::iterator pit;
	if ((pit = params.find("sensor")) != params.end()) {
		std::string& value = pit->second;
		if (_sensor_dev.initialize(value.c_str(), IODevice::IN) < 0) {
			fprintf(stderr, "Open sensor device '%s' failed\n", value.c_str());
		}
	}
	if ((pit = params.find("ircutp")) != params.end()) {
		std::string& value = pit->second;
		if (_ircutp_dev.initialize(value.c_str(), IODevice::OUT) < 0) {
			fprintf(stderr, "Open IRCUT+ device '%s' failed\n", value.c_str());
		}
		_ircutp_dev.setValue(0);
	}
	if ((pit = params.find("ircutn")) != params.end()) {
		std::string& value = pit->second;
		if (_ircutn_dev.initialize(value.c_str(), IODevice::OUT) < 0) {
			fprintf(stderr, "Open IRCUT- device '%s' failed\n", value.c_str());
		}
		_ircutn_dev.setValue(0);
	}
	if ((pit = params.find("irleden")) != params.end()) {
		std::string& value = pit->second;
		if (_irleden_dev.initialize(value.c_str(), IODevice::OUT) < 0) {
			fprintf(stderr, "Open IRLED-EN device '%s' failed\n", value.c_str());
		}
	}
	if ((pit = params.find("irledbr")) != params.end()) {
		std::string& value = pit->second;
		if (_irledbr_dev.initialize(value.c_str(), IODevice::OUT) < 0) {
			fprintf(stderr, "Open IRLED-BRIGHT device '%s' failed\n", value.c_str());
		}
	}
}

HimppIrCut::~HimppIrCut()
{
}

MPP_CHN_S* HimppIrCut::bindSource()
{
	HimppVideoElement *vs = HIMPP_VIDEO_ELEMENT(source());
	return vs ? vs->bindSource() : NULL;
}

VideoSource::Imaging& HimppIrCut::imaging()
{
	return dynamic_cast<VideoSource::Imaging&>(_imaging);
}

IrCutFilterMode HimppIrCut::getMode()
{
	return _mode;
}

void HimppIrCut::setMode(IrCutFilterMode value)
{
	_sensor_timer.stop();
	if (is_enabled()) {
		switch (value) {
		case IRCUT_ON:
			ircut_on();
			break;
		case IRCUT_OFF:
			ircut_off();
			break;
		case IRCUT_AUTO:
			if (_sensor_dev.is_open()) {
				_sensor_timer.start();
			}
			break;
		default:
			throw IpcamError("Invalid IrCut Mode");
			break;
		}
	}
	_mode = value;
}

int32_t HimppIrCut::getThreshold()
{
	return _threshold;
}

void HimppIrCut::setThreshold(int32_t value)
{
	_threshold = value;
}

uint32_t HimppIrCut::getHysteresis()
{
	return _hysteresis;
}

void HimppIrCut::setHysteresis(uint32_t value)
{
	_hysteresis = value;
}

uint32_t HimppIrCut::getBrightness()
{
	return _brightness;
}

void HimppIrCut::setBrightness(uint32_t value)
{
	if (is_enabled()) {
		if (_irledbr_dev.setValue(value) < 0) {
			throw IpcamError("Failed to set IR-LED brightness");
		}
	}
	_brightness = value;
}

void HimppIrCut::doEnableElement()
{
	setMode(_mode);
	_irledbr_dev.setValue(_brightness);
}

void HimppIrCut::doDisableElement()
{
	_sensor_timer.stop();
	ircut_off();
}

void HimppIrCut::sensor_timer_handler(ev::timer& w, int revents)
{
	int retval, inval;

	if ((retval = _sensor_dev.getValue(inval)) > 0) {
		if (_status == IRCUT_ON && inval > _threshold + (int)_hysteresis) {
			if (++_debounce_count >= 3) {
				ircut_off();
				_debounce_count = 0;
			}
		}
		else if (_status == IRCUT_OFF && inval < _threshold - (int)_hysteresis) {
			if (++_debounce_count >= 3) {
				ircut_on();
				_debounce_count = 0;
			}
		}
		else {
			_debounce_count = 0;
		}
	}
}

void HimppIrCut::ircut_timer_handler(ev::timer& w, int revents)
{
	_ircutp_dev.setValue(0);
	_ircutn_dev.setValue(0);
}

void HimppIrCut::ircut_on()
{
	DefaultVideoSource* vs = DEFAULT_VIDEO_SOURCE(source());
	if (vs != NULL) {
		try {
			vs->imaging().ircutfilter().setMode(IRCUT_ON);
		} catch (...) {}
	}

	_ircutn_dev.setValue(0);
	_ircutp_dev.setValue(1);
	_irleden_dev.setValue(1);
	_status = IRCUT_ON;

	_ircut_timer.stop();
	_ircut_timer.set(0.5, 0.0);
	_ircut_timer.start();
}

void HimppIrCut::ircut_off()
{
	DefaultVideoSource* vs = DEFAULT_VIDEO_SOURCE(source());
	if (vs != NULL) {
		try {
			vs->imaging().ircutfilter().setMode(IRCUT_OFF);
		} catch (...) {}
	}

	_ircutp_dev.setValue(0);
	_ircutn_dev.setValue(1);
	_irleden_dev.setValue(0);
	_status = IRCUT_OFF;

	_ircut_timer.stop();
	_ircut_timer.set(0.5, 0.0);
	_ircut_timer.start();
}
