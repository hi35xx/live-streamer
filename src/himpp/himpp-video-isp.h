/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
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
#include <himpp-object.h>
#include <himpp-video-sensor.h>

typedef HI_VOID (*SENSOR_INIT_FUNC)(void);
typedef HI_S32  (*SENSOR_REGISTER_FUNC)(void);
typedef HI_S32  (*SENSOR_UNREGISTER_FUNC)(void);

struct HimppSensorModule
{
    void*       handle;
    SENSOR_INIT_FUNC        sensor_init;
    SENSOR_REGISTER_FUNC    sensor_register;
    SENSOR_UNREGISTER_FUNC  sensor_unregister;
};

class HimppVideoISP: public HimppVideoObject 
{
public:
    HimppVideoISP(HimppVideoSensor *sensor);
    ~HimppVideoISP();

    bool setResolution(ImageResolution &res);
    ImageResolution getResolution();
    bool setFramerate(uint32_t fps);
    uint32_t getFramerate();
    bool setMinExpTime(uint32_t value);
    uint32_t getMinExpTime(void);
    bool setMaxExpTime(uint32_t value);
    uint32_t getMaxExpTime(void);
    bool setMinGain(uint32_t value);
    uint32_t getMinGain(void);
    bool setMaxGain(uint32_t value);
    uint32_t getMaxGain(void);
#if 0
    bool setExpTime(uint32_t value);
    uint32_t getExpTime(void);
    bool setGain(uint32_t value);
    uint32_t getGain(void);
#endif
protected:
    bool enableObject();
    bool disableObject();
    operator MPP_CHN_S* ();
private:
    HimppVideoSensor  *video_sensor;
    HimppSensorModule sensor_module;
    pthread_t isp_thread;

    bool loadSensorModule();
    bool unloadSensorModule();
    bool registerAlgorithm();
    bool unregisterAlgorithm();

    static void *isp_thread_routine(void *arg);
};

#endif // _HIMPP_VIDEO_ISP_H_

