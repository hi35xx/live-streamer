/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
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

#include <himpp-object.h>
#include <himpp-video-sensor.h>
#include <himpp-video-isp.h>

class HimppViDev: public HimppVideoObject 
{
public:
    HimppViDev(HimppVideoSensor *, VI_DEV);
    ~HimppViDev();

    operator MPP_CHN_S* ();
    operator HimppVideoISP* ();
    bool setResolution(ImageResolution &);
    ImageResolution getResolution();
    bool setFramerate(uint32_t);
    uint32_t getFramerate();

    VI_DEV getDeviceId() { return _devid; }
    uint32_t getBrightness();
    void     setBrightness(uint32_t);
    uint32_t getContrast();
    void     setContrast(uint32_t);
    uint32_t getChroma();
    void     setChroma(uint32_t);
    uint32_t getSaturation();
    void     setSaturation(uint32_t);
protected:
    bool enableObject();
    bool disableObject();
private:
    HimppVideoSensor*   _video_sensor;
    HimppVideoISP       _video_isp;
    VI_DEV              _devid;
    uint32_t            _brightness;
    uint32_t            _contrast;
    uint32_t            _chroma;
    uint32_t            _saturation;
    uint32_t            _sharpness;
};

class HimppViChan: public HimppVideoObject
{
public:
    HimppViChan(HimppViDev *, VI_CHN);
    ~HimppViChan();

    operator MPP_CHN_S* ();
    bool setResolution(ImageResolution &);
    ImageResolution getResolution();
    bool setFramerate(uint32_t);
    uint32_t getFramerate();

    ROTATE_E getRotate();
    void setRotate(ROTATE_E);
    bool getMirror();
    void setMirror(bool);
    bool getFlip();
    void setFlip(bool);
protected:
    bool enableObject();
    bool disableObject();
private:
    HimppViDev*     _vi_dev;
    VI_CHN          _chnid;
    MPP_CHN_S       _mpp_chn;
    VI_LDC_ATTR_S   _ldc_attr;
    ROTATE_E        _rotate;
    bool            _mirror;
    bool            _flip;
};

#endif // _HIMPP_VIDEO_VIU_H_

