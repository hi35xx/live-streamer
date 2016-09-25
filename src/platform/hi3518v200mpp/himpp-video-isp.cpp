/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-isp.cpp
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

#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/prctl.h>

#include <hi_comm_isp.h>
#include <hi_ae_comm.h>
#include <hi_af_comm.h>
#include <hi_awb_comm.h>
#include <mpi_ae.h>
#include <mpi_af.h>
#include <mpi_awb.h>

#include <himpp-common.h>
#include "himpp-video-isp.h"


//////////////////////////////////////////////////////////////////////////////
// HimppVideoISP
//////////////////////////////////////////////////////////////////////////////

HimppVideoISP::HimppVideoISP(HimppVideoSensor *sensor)
	: HimppVideoObject(NULL), video_sensor(sensor),
      isp_dev(0), isp_thread(-1),
	  _exposure(*this), _whitebalance(*this),
      _widedynamicrange(*this), _gamma(*this)
{
	memset(&sensor_module, 0, sizeof(sensor_module));
}

HimppVideoISP::~HimppVideoISP()
{
}

bool HimppVideoISP::loadSensorModule()
{
	const std::string &path = video_sensor->getModulePath();

	sensor_module.handle = dlopen(path.c_str(), RTLD_LAZY);
	if (sensor_module.handle == NULL) {
		HIMPP_PRINT("Failed to load sensor module '%s'\n", path.c_str());
		return false;
	}

	sensor_module.sensor_init = (SENSOR_INIT_FUNC)dlsym(sensor_module.handle, "sensor_init");
	sensor_module.sensor_register = (SENSOR_REGISTER_FUNC)dlsym(sensor_module.handle, "sensor_register_callback");
	sensor_module.sensor_unregister = (SENSOR_UNREGISTER_FUNC)dlsym(sensor_module.handle, "sensor_unregister_callback");

	if (!sensor_module.sensor_init
		|| !sensor_module.sensor_register
		|| !sensor_module.sensor_unregister) {
		HIMPP_PRINT("Incorrect sensor module.\n");
		goto err_dlclose;
	}

	if (sensor_module.sensor_register() != HI_SUCCESS) {
		HIMPP_PRINT("Failed to register sensor\n");
		goto err_dlclose;
	}

	return true;

err_dlclose:
	dlclose(sensor_module.handle);
	sensor_module.handle = NULL;
	sensor_module.sensor_init = NULL;
	sensor_module.sensor_register = NULL;
	sensor_module.sensor_unregister = NULL;

	return false;
}

bool HimppVideoISP::unloadSensorModule()
{
	if (sensor_module.handle) {
		sensor_module.sensor_unregister();
		dlclose(sensor_module.handle);
		sensor_module.handle = NULL;
		sensor_module.sensor_init = NULL;
		sensor_module.sensor_register = NULL;
		sensor_module.sensor_unregister = NULL;
	}
	return true;
}

bool HimppVideoISP::registerAlgorithm()
{
	ALG_LIB_S stLib;
	HI_S32 s32Ret;

	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AE_LIB_NAME);
	if ((s32Ret = HI_MPI_AE_Register(isp_dev, &stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AE_Register failed [%#x]\n", s32Ret);
		return false;
	}

	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
	if ((s32Ret = HI_MPI_AWB_Register(isp_dev, &stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AWB_Register failed [%#x]\n", s32Ret);
		return false;
	}

	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AF_LIB_NAME);
	if ((s32Ret = HI_MPI_AF_Register(isp_dev, &stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AF_Register failed [%#x]\n", s32Ret);
		return false;
	}

	return true;
}

bool HimppVideoISP::unregisterAlgorithm()
{
	ALG_LIB_S stLib;
	HI_S32 s32Ret;

	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AE_LIB_NAME);
	if ((s32Ret = HI_MPI_AE_UnRegister(isp_dev, &stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AE_UnRegister failed [%#x]\n", s32Ret);
	}

	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
	if ((s32Ret = HI_MPI_AWB_UnRegister(isp_dev, &stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AWB_UnRegister failed [%#x]\n", s32Ret);
	}

	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AF_LIB_NAME);
	if ((s32Ret = HI_MPI_AF_UnRegister(isp_dev, &stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AF_UnRegister failed [%#x]\n", s32Ret);
	}

	return true;
}

void *HimppVideoISP::isp_thread_routine(void *arg)
{
	prctl(PR_SET_NAME, (unsigned long)"ISP");

	return (void*)HI_MPI_ISP_Run(0);
}

bool HimppVideoISP::enableObject()
{
	HI_S32 s32Ret;
	ISP_PUB_ATTR_S *pstPubAttr = *video_sensor;
    ISP_WDR_MODE_S stWdrMode;

	if (!loadSensorModule())
		return false;

	if (!registerAlgorithm())
		goto err_unload_mod;

    /* isp mem init */
	if ((s32Ret = HI_MPI_ISP_MemInit(isp_dev)) != HI_SUCCESS)
		goto err_unreg_alg;

	/* isp set wdr mode */
	stWdrMode.enWDRMode = WDR_MODE_NONE;
	if ((s32Ret = HI_MPI_ISP_SetWDRMode(isp_dev, &stWdrMode)) != HI_SUCCESS)
		goto err_unreg_alg;

	if (HI_MPI_ISP_SetPubAttr(isp_dev, pstPubAttr) != HI_SUCCESS)
		goto err_unreg_alg;

	if ((s32Ret = HI_MPI_ISP_Init(isp_dev)) != HI_SUCCESS)
		goto err_unreg_alg;

	if (pthread_create(&isp_thread, 0, isp_thread_routine, NULL))
		goto err_exit_isp;

	// initialize Exposure
	ISP_EXPOSURE_ATTR_S exp_attr;
	if (HI_MPI_ISP_GetExposureAttr(isp_dev, &exp_attr) == HI_SUCCESS) {
        exp_attr.enOpType = (_exposure._mode == IVideoSource::Imaging::Exposure::AUTO_EXPOSURE) ?
            OP_TYPE_AUTO : OP_TYPE_MANUAL;

        exp_attr.stAuto.enAEMode = (_exposure._priority == IVideoSource::Imaging::Exposure::LOWNOISE_PRIORITY) ? \
            AE_MODE_SLOW_SHUTTER : AE_MODE_FIX_FRAME_RATE;
		exp_attr.stAuto.stExpTimeRange.u32Min = _exposure._min_exp_time;
		exp_attr.stAuto.stExpTimeRange.u32Max = _exposure._max_exp_time;
		exp_attr.stAuto.stAGainRange.u32Min = _exposure._min_gain;
		exp_attr.stAuto.stAGainRange.u32Max = _exposure._max_gain;

		exp_attr.stManual.u32ExpTime = _exposure._exp_time;
		exp_attr.stManual.u32AGain = _exposure._gain;

		if (HI_MPI_ISP_SetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS) {
			fprintf(stderr, "HI_MPI_ISP_SetAEAttrEx failed\n");
		}
	}

	// Initialize WhiteBalance
    ISP_WB_ATTR_S wb_attr;
    if (HI_MPI_ISP_GetWBAttr(isp_dev, &wb_attr) == HI_SUCCESS) {
        wb_attr.enOpType = (_whitebalance._mode == IVideoSource::Imaging::WhiteBalance::AUTO_WB) ?
            OP_TYPE_AUTO : OP_TYPE_MANUAL;

        wb_attr.stAuto.u8BGStrength = (HI_U8)_whitebalance._cb_gain;
        wb_attr.stAuto.u8RGStrength = (HI_U8)_whitebalance._cr_gain;

        wb_attr.stManual.u16Rgain = (HI_U16)_whitebalance._r_gain;
        wb_attr.stManual.u16Grgain = (HI_U16)_whitebalance._g_gain;
        wb_attr.stManual.u16Bgain = (HI_U16)_whitebalance._b_gain;
        if (HI_MPI_ISP_SetWBAttr(isp_dev, &wb_attr) != HI_SUCCESS)
            fprintf(stderr, "HI_MPI_ISP_SetWBType failed\n");
    }

    return true;

err_exit_isp:
	HI_MPI_ISP_Exit(isp_dev);
err_unreg_alg:
	unregisterAlgorithm();
err_unload_mod:
	unloadSensorModule();

	return false;
}

bool HimppVideoISP::disableObject()
{
	HI_MPI_ISP_Exit(isp_dev);
	unregisterAlgorithm();
	unloadSensorModule();

	pthread_join(isp_thread, 0);

	return true;
}

bool HimppVideoISP::setResolution(ImageResolution &res)
{
	return false;
}

ImageResolution HimppVideoISP::getResolution()
{
	ISP_PUB_ATTR_S *attr = *video_sensor;
	return ImageResolution(attr->stWndRect.u32Width, attr->stWndRect.u32Height);
}

bool HimppVideoISP::setFramerate(uint32_t fps)
{
	return false;
}

uint32_t HimppVideoISP::getFramerate()
{
	ISP_PUB_ATTR_S *attr = *video_sensor;
	return (uint32_t)attr->f32FrameRate;
}

HimppVideoISP::operator MPP_CHN_S* ()
{
	return NULL;
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoISP::Exposure
//////////////////////////////////////////////////////////////////////////////

HimppVideoISP::Exposure::Exposure(HimppVideoISP &video_isp)
	: _video_isp(video_isp),
	  _mode(IVideoSource::Imaging::Exposure::AUTO_EXPOSURE),
	  _priority(IVideoSource::Imaging::Exposure::FRAMERATE_PRIORITY),
	  _min_exp_time(2), _max_exp_time(80000),
	  _min_gain(1024), _max_gain(80000),
	  _min_iris(0), _max_iris(0),
	  _exp_time(512), _gain(2048), _iris(0)
{
}

void HimppVideoISP::Exposure::setExposureMode(ExposureMode mode)
{
	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
        ISP_EXPOSURE_ATTR_S exp_attr;
        if (HI_MPI_ISP_GetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
            throw IpcamError("get exposure attr failed");
		exp_attr.enOpType =
			(mode == IVideoSource::Imaging::Exposure::AUTO_EXPOSURE) ?
			OP_TYPE_AUTO : OP_TYPE_MANUAL;
		if (HI_MPI_ISP_SetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_mode = mode;
}

HimppVideoISP::ExposureMode HimppVideoISP::Exposure::getExposureMode()
{
	return _mode;
}

void HimppVideoISP::Exposure::setExposurePriority(ExposurePriority priority)
{
	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
        ISP_EXPOSURE_ATTR_S exp_attr;
        if (HI_MPI_ISP_GetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
            throw IpcamError("get ExposurePriority failed");
		exp_attr.stAuto.enAEMode =
			(priority == IVideoSource::Imaging::Exposure::LOWNOISE_PRIORITY) ?
				AE_MODE_SLOW_SHUTTER : AE_MODE_FIX_FRAME_RATE;
		if (HI_MPI_ISP_SetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
			throw IpcamError("set ExposurePriority failed");
	}
	_priority = priority;
}

HimppVideoISP::ExposurePriority HimppVideoISP::Exposure::getExposurePriority()
{
	return _priority;
}

void HimppVideoISP::Exposure::setMinExposureTime(uint32_t value)
{
	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
        ISP_EXPOSURE_ATTR_S exp_attr;
        if (HI_MPI_ISP_GetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
            throw IpcamError("get exposure attr failed");

		exp_attr.stAuto.stExpTimeRange.u32Min = value;
		if (HI_MPI_ISP_SetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_min_exp_time = value;
}

uint32_t HimppVideoISP::Exposure::getMinExposureTime(void)
{
	return _min_exp_time;
}

void HimppVideoISP::Exposure::setMaxExposureTime(uint32_t value)
{
	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
        ISP_EXPOSURE_ATTR_S exp_attr;
        if (HI_MPI_ISP_GetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
            throw IpcamError("get exposure attr failed");

		exp_attr.stAuto.stExpTimeRange.u32Max = value;
		if (HI_MPI_ISP_SetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_max_exp_time = value;

}

uint32_t HimppVideoISP::Exposure::getMaxExposureTime(void)
{
	return _max_exp_time;
}

void HimppVideoISP::Exposure::setMinGain(uint32_t value)
{
	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
        ISP_EXPOSURE_ATTR_S exp_attr;
        if (HI_MPI_ISP_GetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
            throw IpcamError("get exposure attr failed");

		exp_attr.stAuto.stAGainRange.u32Min = value;
		if (HI_MPI_ISP_SetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_min_gain = value;
}

uint32_t HimppVideoISP::Exposure::getMinGain(void)
{
	return _min_gain;
}

void HimppVideoISP::Exposure::setMaxGain(uint32_t value)
{
	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
        ISP_EXPOSURE_ATTR_S exp_attr;
        if (HI_MPI_ISP_GetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
            throw IpcamError("get exposure attr failed");

		exp_attr.stAuto.stAGainRange.u32Max = value;
		if (HI_MPI_ISP_SetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_max_gain = value;
}

uint32_t HimppVideoISP::Exposure::getMaxGain(void)
{
	return _max_gain;
}

void HimppVideoISP::Exposure::setMinIris(uint32_t value)
{
	if (isEnabled()) {
	}
	_min_iris = value;
}

uint32_t HimppVideoISP::Exposure::getMinIris(void)
{
	if (isEnabled()) {
	}
	return _min_iris;
}

void HimppVideoISP::Exposure::setMaxIris(uint32_t value)
{
	if (isEnabled()) {
	}
	_max_iris = value;
}

uint32_t HimppVideoISP::Exposure::getMaxIris(void)
{
	if (isEnabled()) {
	}
	return _max_iris;
}

void HimppVideoISP::Exposure::setExposureTime(uint32_t value)
{
	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
        ISP_EXPOSURE_ATTR_S exp_attr;
        if (HI_MPI_ISP_GetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
            throw IpcamError("get exposure attr failed");

		exp_attr.stManual.u32ExpTime = value;
		if (HI_MPI_ISP_SetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_exp_time = value;
}

uint32_t HimppVideoISP::Exposure::getExposureTime(void)
{
	return _exp_time;
}

void HimppVideoISP::Exposure::setGain(uint32_t value)
{
	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
        ISP_EXPOSURE_ATTR_S exp_attr;
        if (HI_MPI_ISP_GetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
            throw IpcamError("get exposure attr failed");

		exp_attr.stManual.u32AGain = value;
		if (HI_MPI_ISP_SetExposureAttr(isp_dev, &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_gain = value;
}

uint32_t HimppVideoISP::Exposure::getGain(void)
{
	return _gain;
}

void HimppVideoISP::Exposure::setIris(uint32_t value)
{
	if (isEnabled()) {
	}
	_iris = value;
}

uint32_t HimppVideoISP::Exposure::getIris(void)
{
	if (isEnabled()) {
	}
	return _iris;
}

//////////////////////////////////////////////////////////////////////////////
// HimppVideoISP::WhiteBalance
//////////////////////////////////////////////////////////////////////////////

HimppVideoISP::WhiteBalance::WhiteBalance(HimppVideoISP &video_isp)
	: _video_isp(video_isp),
	  _mode(IVideoSource::Imaging::WhiteBalance::AUTO_WB),
	  _cb_gain(128), _cr_gain(128),
	  _r_gain(256), _g_gain(256), _b_gain(256)
{
}

void HimppVideoISP::WhiteBalance::setWBMode(WhiteBalanceMode value)
{
	_mode = value;

	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
		ISP_WB_ATTR_S wb_attr;
		if (HI_MPI_ISP_GetWBAttr(isp_dev, &wb_attr) != HI_SUCCESS)
            throw IpcamError("get whitebalance attr failed");

        wb_attr.enOpType = \
            (value == IVideoSource::Imaging::WhiteBalance::AUTO_WB) ?
            OP_TYPE_AUTO : OP_TYPE_MANUAL;
        if (HI_MPI_ISP_SetWBAttr(isp_dev, &wb_attr) != HI_SUCCESS)
            throw IpcamError("get whitebalance attr failed");
	}
}

HimppVideoISP::WhiteBalanceMode HimppVideoISP::WhiteBalance::getWBMode()
{
	return _mode;
}

void HimppVideoISP::WhiteBalance::setCbGain(uint32_t value)
{
	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
		ISP_WB_ATTR_S wb_attr;
		if (HI_MPI_ISP_GetWBAttr(isp_dev, &wb_attr) != HI_SUCCESS)
            throw IpcamError("get whitebalance attr failed");

        wb_attr.stAuto.u8BGStrength = value;
        if (HI_MPI_ISP_SetWBAttr(isp_dev, &wb_attr) != HI_SUCCESS)
            throw IpcamError("get whitebalance attr failed");
	}
	_cb_gain = value;
}

uint32_t HimppVideoISP::WhiteBalance::getCbGain()
{
	return _cb_gain;
}

void HimppVideoISP::WhiteBalance::setCrGain(uint32_t value)
{
	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
		ISP_WB_ATTR_S wb_attr;
		if (HI_MPI_ISP_GetWBAttr(isp_dev, &wb_attr) != HI_SUCCESS)
            throw IpcamError("get whitebalance attr failed");

        wb_attr.stAuto.u8RGStrength = value;
        if (HI_MPI_ISP_SetWBAttr(isp_dev, &wb_attr) != HI_SUCCESS)
            throw IpcamError("get whitebalance attr failed");
	}
	_cr_gain = value;
}

uint32_t HimppVideoISP::WhiteBalance::getCrGain()
{
	return _cr_gain;
}


void HimppVideoISP::WhiteBalance::setRGain(uint32_t value)
{
	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
		ISP_WB_ATTR_S wb_attr;
		if (HI_MPI_ISP_GetWBAttr(isp_dev, &wb_attr) != HI_SUCCESS)
            throw IpcamError("get whitebalance attr failed");

        wb_attr.stManual.u16Rgain = value;
        if (HI_MPI_ISP_SetWBAttr(isp_dev, &wb_attr) != HI_SUCCESS)
            throw IpcamError("get whitebalance attr failed");
	}
	_r_gain = value;
}

uint32_t HimppVideoISP::WhiteBalance::getRGain()
{
	return _r_gain;
}

void HimppVideoISP::WhiteBalance::setGGain(uint32_t value)
{
	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
		ISP_WB_ATTR_S wb_attr;
		if (HI_MPI_ISP_GetWBAttr(isp_dev, &wb_attr) != HI_SUCCESS)
            throw IpcamError("get whitebalance attr failed");

        wb_attr.stManual.u16Grgain = value;
        if (HI_MPI_ISP_SetWBAttr(isp_dev, &wb_attr) != HI_SUCCESS)
            throw IpcamError("get whitebalance attr failed");
	}
	_g_gain = value;
}

uint32_t HimppVideoISP::WhiteBalance::getGGain()
{
	return _g_gain;
}

void HimppVideoISP::WhiteBalance::setBGain(uint32_t value)
{
	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
		ISP_WB_ATTR_S wb_attr;
		if (HI_MPI_ISP_GetWBAttr(isp_dev, &wb_attr) != HI_SUCCESS)
            throw IpcamError("get whitebalance attr failed");

        wb_attr.stManual.u16Bgain = value;
        if (HI_MPI_ISP_SetWBAttr(isp_dev, &wb_attr) != HI_SUCCESS)
            throw IpcamError("get whitebalance attr failed");
	}
	_b_gain = value;
}

uint32_t HimppVideoISP::WhiteBalance::getBGain()
{
	return _b_gain;
}

//////////////////////////////////////////////////////////////////////////////
// HimppVideoISP::WideDynamicRange
//////////////////////////////////////////////////////////////////////////////

HimppVideoISP::WideDynamicRange::WideDynamicRange(HimppVideoISP &video_isp)
	: _video_isp(video_isp),
	  _mode(IVideoSource::Imaging::WideDynamicRange::WDR_OFF),
	  _level(0)
{
}

void HimppVideoISP::WideDynamicRange::setWDRMode(WDRMode value)
{
	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
		ISP_DRC_ATTR_S wdr_attr;
		if (HI_MPI_ISP_GetDRCAttr(isp_dev, &wdr_attr) != HI_SUCCESS)
            throw IpcamError("get WDR attr failed");

        wdr_attr.bEnable = \
            (value == IVideoSource::Imaging::WideDynamicRange::WDR_ON) ?
                HI_TRUE : HI_FALSE;
        if (HI_MPI_ISP_SetDRCAttr(isp_dev, &wdr_attr) != HI_SUCCESS)
            throw IpcamError("get WDR attr failed");
	}
	_mode = value;
}

HimppVideoISP::WDRMode HimppVideoISP::WideDynamicRange::getWDRMode()
{
	return _mode;
}

void HimppVideoISP::WideDynamicRange::setLevel(uint32_t value)
{
	if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
		ISP_DRC_ATTR_S wdr_attr;
		if (HI_MPI_ISP_GetDRCAttr(isp_dev, &wdr_attr) != HI_SUCCESS)
            throw IpcamError("get WDR attr failed");

        //wdr_attr.bEnable = 
        //    (value == IVideoSource::Imaging::WideDynamicRange::WDR_ON) ?
        //        HI_TRUE : HI_FALSE;
        if (HI_MPI_ISP_SetDRCAttr(isp_dev, &wdr_attr) != HI_SUCCESS)
            throw IpcamError("get WDR attr failed");
	}
	_level = value;
}

uint32_t HimppVideoISP::WideDynamicRange::getLevel()
{
	return _level;
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoISP::Gamma
//////////////////////////////////////////////////////////////////////////////

HimppVideoISP::Gamma::Gamma(HimppVideoISP &video_isp)
  : _video_isp(video_isp)
{
}

void HimppVideoISP::Gamma::setCurveData(std::vector<uint32_t>& value)
{
    ISP_GAMMA_ATTR_S gamma_attr;

    if ((value.size() != 0) && (value.size() != ARRAY_SIZE(gamma_attr.u16Table)))
        throw IpcamError("invalid gamma data");

    if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
        gamma_attr.bEnable = HI_TRUE;
        if (value.size() > 0) {
            gamma_attr.enCurveType = ISP_GAMMA_CURVE_USER_DEFINE;
            for (unsigned i = 0; i < value.size(); i++) {
                gamma_attr.u16Table[i] = value[i];
            }
        }
        else {
            gamma_attr.enCurveType = ISP_GAMMA_CURVE_DEFAULT;
            memset(gamma_attr.u16Table, 0, sizeof(gamma_attr.u16Table));
        }
        if (HI_MPI_ISP_SetGammaAttr(isp_dev, &gamma_attr) != HI_SUCCESS)
            throw IpcamError("failed to set gamma attr");
    }
    _curve_data = value;
}

std::vector<uint32_t>& HimppVideoISP::Gamma::getCurveData()
{
    if (isEnabled()) {
        ISP_DEV isp_dev = _video_isp.ispDev();
        ISP_GAMMA_ATTR_S gamma_attr;

        if (HI_MPI_ISP_GetGammaAttr(isp_dev, &gamma_attr) != HI_SUCCESS)
            throw IpcamError("failed to get gamma attr");

        _curve_data.resize(ARRAY_SIZE(gamma_attr.u16Table));
        for (unsigned i = 0; i < _curve_data.size(); i++) {
            _curve_data[i] = gamma_attr.u16Table[i];
        }
    }

    return _curve_data;
}
