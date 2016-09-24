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
    typedef IVideoSource::Imaging::Exposure::ExposureMode ExposureMode;
    typedef IVideoSource::Imaging::Exposure::ExposurePriority ExposurePriority;
    typedef IVideoSource::Imaging::WhiteBalance::WhiteBalanceMode WhiteBalanceMode;
    typedef IVideoSource::Imaging::WideDynamicRange::WDRMode WDRMode;
    // Exposure
    class Exposure
    {
    public:
        Exposure(HimppVideoISP &video_isp);

        void setExposureMode(ExposureMode mode);
        ExposureMode getExposureMode();
        void setExposurePriority(ExposurePriority priority);
        ExposurePriority getExposurePriority();
        void setMinExposureTime(uint32_t value);
        uint32_t getMinExposureTime();
        void setMaxExposureTime(uint32_t value);
        uint32_t getMaxExposureTime();
        void setMinGain(uint32_t value);
        uint32_t getMinGain();
        void setMaxGain(uint32_t value);
        uint32_t getMaxGain();
        void setMinIris(uint32_t value);
        uint32_t getMinIris();
        void setMaxIris(uint32_t value);
        uint32_t getMaxIris();
        void setExposureTime(uint32_t value);
        uint32_t getExposureTime();
        void setGain(uint32_t value);
        uint32_t getGain();
        void setIris(uint32_t value);
        uint32_t getIris();
    private:
        friend class HimppVideoISP;

        bool isEnabled() { return _video_isp.isEnabled(); }
        HimppVideoISP &     _video_isp;
        ExposureMode        _mode;
        ExposurePriority    _priority;
        uint32_t            _min_exp_time;
        uint32_t            _max_exp_time;
        uint32_t            _min_gain;
        uint32_t            _max_gain;
        uint32_t            _min_iris;
        uint32_t            _max_iris;
        uint32_t            _exp_time;
        uint32_t            _gain;
        uint32_t            _iris;
    };
    // WhiteBalance
    class WhiteBalance
    {
    public:
        WhiteBalance(HimppVideoISP &video_isp);

        void setWBMode(WhiteBalanceMode value);
        WhiteBalanceMode getWBMode();
        void setCbGain(uint32_t value);
        uint32_t getCbGain();
        void setCrGain(uint32_t value);
        uint32_t getCrGain();
        void setRGain(uint32_t value);
        uint32_t getRGain();
        void setGGain(uint32_t value);
        uint32_t getGGain();
        void setBGain(uint32_t value);
        uint32_t getBGain();
    private:
        friend class HimppVideoISP;

        bool isEnabled() { return _video_isp.isEnabled(); }
        HimppVideoISP &     _video_isp;
        WhiteBalanceMode    _mode;
        uint32_t            _cb_gain;
        uint32_t            _cr_gain;
        uint32_t            _r_gain;
        uint32_t            _g_gain;
        uint32_t            _b_gain;
    };
    // WideDynamicRange
    class WideDynamicRange
    {
    public:
        WideDynamicRange(HimppVideoISP &video_isp);

        void setWDRMode(WDRMode value);
        WDRMode getWDRMode();
        void setLevel(uint32_t value);
        uint32_t getLevel();
    private:
        friend class HimppVideoISP;

        bool isEnabled() { return _video_isp.isEnabled(); }
        HimppVideoISP&      _video_isp;
        WDRMode             _mode;
        uint32_t            _level;
    };

public:
    HimppVideoISP(HimppVideoSensor *sensor);
    ~HimppVideoISP();

    bool setResolution(ImageResolution &res);
    ImageResolution getResolution();
    bool setFramerate(uint32_t fps);
    uint32_t getFramerate();

    Exposure *getExposure() { return &_exposure; }
    WhiteBalance *getWhiteBalance() { return &_whitebalance; }
    WideDynamicRange *getWideDynamicRange() { return &_widedynamicrange; }

    ISP_DEV ispDev() { return isp_dev; }
protected:
    bool enableObject();
    bool disableObject();
    operator MPP_CHN_S* ();
private:
    HimppVideoSensor  *video_sensor;
    HimppSensorModule sensor_module;
    ISP_DEV         isp_dev;
    pthread_t       isp_thread;

    Exposure        _exposure;
    WhiteBalance    _whitebalance;
    WideDynamicRange _widedynamicrange;

    bool loadSensorModule();
    bool unloadSensorModule();
    bool registerAlgorithm();
    bool unregisterAlgorithm();

    static void *isp_thread_routine(void *arg);
};

#endif // _HIMPP_VIDEO_ISP_H_

