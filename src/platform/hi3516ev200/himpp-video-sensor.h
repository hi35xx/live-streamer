/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-sensor.h
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

#ifndef _HIMPP_VIDEO_SENSOR_H_
#define _HIMPP_VIDEO_SENSOR_H_

#include <string>
#include <unordered_map>
#include <hi_mipi.h>
#include <mpi_isp.h>
#include <mpi_vi.h>

struct HIMPP_SENSOR_CONFIG;

class HimppVideoSensor 
{
public:
	HimppVideoSensor(HIMPP_SENSOR_CONFIG *config);
	~HimppVideoSensor();

	const char* getSensorName();
	const char* getModulePath();
	const char* getObjectName();

	operator combo_dev_attr_t* ();
	operator ISP_PUB_ATTR_S* ();
	operator VI_DEV_ATTR_S* ();
	operator VI_PIPE_ATTR_S* ();
private:
	HIMPP_SENSOR_CONFIG *sensor_config;
};

typedef std::unordered_map<std::string, HimppVideoSensor> HimppVideoSensorMap;

extern HimppVideoSensorMap himpp_video_sensor_map;

#endif // _HIMPP_VIDEO_SENSOR_H_

