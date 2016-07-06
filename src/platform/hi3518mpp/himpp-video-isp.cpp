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
	  _exposure(*this), _whitebalance(*this)
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
	if ((s32Ret = HI_MPI_AE_Register(&stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AE_Register failed [%#x]\n", s32Ret);
		return false;
	}

	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
	if ((s32Ret = HI_MPI_AWB_Register(&stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AWB_Register failed [%#x]\n", s32Ret);
		return false;
	}

	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AF_LIB_NAME);
	if ((s32Ret = HI_MPI_AF_Register(&stLib)) != HI_SUCCESS) {
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
	if ((s32Ret = HI_MPI_AE_UnRegister(&stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AE_UnRegister failed [%#x]\n", s32Ret);
		return false;
	}

	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
	if ((s32Ret = HI_MPI_AWB_UnRegister(&stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AWB_UnRegister failed [%#x]\n", s32Ret);
		goto err_unreg_ae;
	}

	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AF_LIB_NAME);
	if ((s32Ret = HI_MPI_AF_UnRegister(&stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AF_UnRegister failed [%#x]\n", s32Ret);
		goto err_unreg_awb;
	}

	return true;

err_unreg_awb:
	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
	HI_MPI_AWB_UnRegister(&stLib);
err_unreg_ae:
	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AE_LIB_NAME);
	HI_MPI_AE_UnRegister(&stLib);

	return false;
}

void *HimppVideoISP::isp_thread_routine(void *arg)
{
	prctl(PR_SET_NAME, (unsigned long)"ISP");

	return (void*)HI_MPI_ISP_Run();
}

bool HimppVideoISP::enableObject()
{
	HI_S32 s32Ret;
	ISP_IMAGE_ATTR_S *pstImageAttr = *video_sensor;
	ISP_INPUT_TIMING_S *pstInputTiming = *video_sensor;

	if (!loadSensorModule())
		return false;

	if (!registerAlgorithm())
		goto err_unload_mod;

	if ((s32Ret = HI_MPI_ISP_Init()) != HI_SUCCESS) {
		goto err_unreg_alg;
	}

	if (HI_MPI_ISP_SetImageAttr(pstImageAttr) != HI_SUCCESS) {
		goto err_exit_isp;
	}

	if (HI_MPI_ISP_SetInputTiming(pstInputTiming) != HI_SUCCESS) {
		goto err_exit_isp;
	}

	if (pthread_create(&isp_thread, 0, isp_thread_routine, NULL))
		goto err_exit_isp;

	ISP_OP_TYPE_E exp_opmode;
	exp_opmode = (_exposure._mode == IVideoSource::Imaging::Exposure::AUTO_EXPOSURE) ?
		OP_TYPE_AUTO : OP_TYPE_MANUAL;
	if (HI_MPI_ISP_SetExposureType(exp_opmode) != HI_SUCCESS) {
		fprintf(stderr, "HI_MPI_ISP_SetExposureType failed\n");
    }

	// initialize Exposure
	ISP_AE_ATTR_EX_S ae_attr;
	if (HI_MPI_ISP_GetAEAttrEx(&ae_attr) == HI_SUCCESS) {
        ae_attr.enAEMode = (_exposure._priority == IVideoSource::Imaging::Exposure::LOWNOISE_PRIORITY) ? \
                            AE_MODE_LOW_NOISE : AE_MODE_FRAME_RATE;
		ae_attr.u32ExpTimeMin = _exposure._min_exp_time;
		ae_attr.u32ExpTimeMax = _exposure._max_exp_time;
		ae_attr.u32AGainMin = _exposure._min_gain;
		ae_attr.u32AGainMax = _exposure._max_gain;

		if (HI_MPI_ISP_SetAEAttrEx(&ae_attr) != HI_SUCCESS) {
			fprintf(stderr, "HI_MPI_ISP_SetAEAttrEx failed\n");
		}
	}

	ISP_ME_ATTR_EX_S me_attr;
	if (HI_MPI_ISP_GetMEAttrEx(&me_attr) == HI_SUCCESS) {
		me_attr.u32ExpLine = _exposure._exp_time;
		me_attr.s32AGain = _exposure._gain;
		if (HI_MPI_ISP_SetMEAttrEx(&me_attr) != HI_SUCCESS) {
			fprintf(stderr, "HI_MPI_ISP_SetMEAttrEx failed\n");
		}
	}

	// Initialize WhiteBalance
	ISP_OP_TYPE_E wb_opmode;
	wb_opmode = (_whitebalance._mode == IVideoSource::Imaging::WhiteBalance::AUTO_WB) ?
		OP_TYPE_AUTO : OP_TYPE_MANUAL;
	if (HI_MPI_ISP_SetWBType(wb_opmode) != HI_SUCCESS) {
		fprintf(stderr, "HI_MPI_ISP_SetWBType failed\n");
    }

	if (_whitebalance._mode == IVideoSource::Imaging::WhiteBalance::AUTO_WB) {
		ISP_AWB_ATTR_S awb_attr;
		if (HI_MPI_ISP_GetAWBAttr(&awb_attr) == HI_SUCCESS) {
			awb_attr.u8BGStrength = (HI_U8)_whitebalance._cb_gain;
			awb_attr.u8RGStrength = (HI_U8)_whitebalance._cr_gain;
			if (HI_MPI_ISP_SetAWBAttr(&awb_attr) != HI_SUCCESS)
				fprintf(stderr, "HI_MPI_ISP_SetWBType failed\n");
		}
	}
	else {
		ISP_MWB_ATTR_S mwb_attr;
		if (HI_MPI_ISP_GetMWBAttr(&mwb_attr) == HI_SUCCESS) {
			mwb_attr.u16Rgain = (HI_U16)_whitebalance._r_gain;
			mwb_attr.u16Ggain = (HI_U16)_whitebalance._g_gain;
			mwb_attr.u16Bgain = (HI_U16)_whitebalance._b_gain;
			if (HI_MPI_ISP_SetMWBAttr(&mwb_attr) != HI_SUCCESS)
				throw IpcamError("Cannot set MWB attr");
        }
	}

	return true;

err_exit_isp:
	HI_MPI_ISP_Exit();
err_unreg_alg:
	unregisterAlgorithm();
err_unload_mod:
	unloadSensorModule();

	return false;
}

bool HimppVideoISP::disableObject()
{
	HI_MPI_ISP_Exit();
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
	ISP_IMAGE_ATTR_S *attr = *video_sensor;
	return ImageResolution(attr->u16Width, attr->u16Height);
}

bool HimppVideoISP::setFramerate(uint32_t fps)
{
	return false;
}

uint32_t HimppVideoISP::getFramerate()
{
	ISP_IMAGE_ATTR_S *attr = *video_sensor;
	return attr->u16FrameRate;
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
	  _min_exp_time(2), _max_exp_time(8191),
	  _min_gain(1024), _max_gain(16384),
	  _min_iris(0), _max_iris(0),
	  _exp_time(512), _gain(2048), _iris(0)
{
}

void HimppVideoISP::Exposure::setExposureMode(ExposureMode mode)
{
	if (isEnabled()) {
		ISP_OP_TYPE_E exp_mode =
			(mode == IVideoSource::Imaging::Exposure::AUTO_EXPOSURE) ?
			OP_TYPE_AUTO : OP_TYPE_MANUAL;
		if (HI_MPI_ISP_SetExposureType(exp_mode) != HI_SUCCESS)
			throw IpcamError("set ExposureMode failed");
	}
	_mode = mode;
}

HimppVideoISP::ExposureMode HimppVideoISP::Exposure::getExposureMode()
{
	if (isEnabled()) {
		ISP_OP_TYPE_E exp_mode;
		if (HI_MPI_ISP_GetExposureType(&exp_mode) != HI_SUCCESS)
			throw IpcamError("get exposure MinGain failed");

		ExposureMode mode;
		mode = (exp_mode == OP_TYPE_AUTO) ?
				IVideoSource::Imaging::Exposure::AUTO_EXPOSURE :
				IVideoSource::Imaging::Exposure::MANUAL_EXPOSURE;
		return mode;
	}
	return _mode;
}

void HimppVideoISP::Exposure::setExposurePriority(ExposurePriority priority)
{
	if (isEnabled()) {
		ISP_AE_ATTR_EX_S ae_attr;
		if (HI_MPI_ISP_GetAEAttrEx(&ae_attr) != HI_SUCCESS)
			throw IpcamError("get ExposurePriority failed");

		ae_attr.enAEMode = \
			(priority == IVideoSource::Imaging::Exposure::LOWNOISE_PRIORITY) ?
				AE_MODE_LOW_NOISE : AE_MODE_FRAME_RATE;
		if (HI_MPI_ISP_SetAEAttrEx(&ae_attr) != HI_SUCCESS)
			throw IpcamError("set ExposurePriority failed");
	}
	_priority = priority;
}

HimppVideoISP::ExposurePriority HimppVideoISP::Exposure::getExposurePriority()
{
	if (isEnabled()) {
		ISP_AE_ATTR_EX_S ae_attr;
		if (HI_MPI_ISP_GetAEAttrEx(&ae_attr) != HI_SUCCESS)
			throw IpcamError("get exposure MinGain failed");

		ExposurePriority priority;
		priority = (ae_attr.enAEMode == AE_MODE_LOW_NOISE) ?
					IVideoSource::Imaging::Exposure::LOWNOISE_PRIORITY :
					IVideoSource::Imaging::Exposure::FRAMERATE_PRIORITY;
		return priority;
	}
	return _priority;
}

void HimppVideoISP::Exposure::setMinExposureTime(uint32_t value)
{
	if (isEnabled()) {
		ISP_AE_ATTR_EX_S ae_attr;
		if (HI_MPI_ISP_GetAEAttrEx(&ae_attr) != HI_SUCCESS)
			throw IpcamError("get MinExposureTime failed");

		ae_attr.u32ExpTimeMin = value;
		if (HI_MPI_ISP_SetAEAttrEx(&ae_attr) != HI_SUCCESS)
			throw IpcamError("set MinExposureTime failed");
	}
	_min_exp_time = value;
}

uint32_t HimppVideoISP::Exposure::getMinExposureTime(void)
{
	if (isEnabled()) {
		ISP_AE_ATTR_EX_S ae_attr;
		if (HI_MPI_ISP_GetAEAttrEx(&ae_attr) != HI_SUCCESS)
			throw IpcamError("get exposure MinGain failed");
		return ae_attr.u32ExpTimeMin;
	}
	return _min_exp_time;
}

void HimppVideoISP::Exposure::setMaxExposureTime(uint32_t value)
{
	if (isEnabled()) {
		ISP_AE_ATTR_EX_S ae_attr;
		if (HI_MPI_ISP_GetAEAttrEx(&ae_attr) != HI_SUCCESS)
			throw IpcamError("get MaxExposureTime failed");

		ae_attr.u32ExpTimeMax = value;
		HI_MPI_ISP_SetAEAttrEx(&ae_attr);
		if (HI_MPI_ISP_GetAEAttrEx(&ae_attr) != HI_SUCCESS)
			throw IpcamError("set MaxExposureTime failed");
	}
	_max_exp_time = value;

}

uint32_t HimppVideoISP::Exposure::getMaxExposureTime(void)
{
	if (isEnabled()) {
		ISP_AE_ATTR_EX_S ae_attr;
		if (HI_MPI_ISP_GetAEAttrEx(&ae_attr) != HI_SUCCESS)
			throw IpcamError("get exposure MinGain failed");
		return ae_attr.u32ExpTimeMax;
	}
	return _max_exp_time;
}

void HimppVideoISP::Exposure::setMinGain(uint32_t value)
{
	if (isEnabled()) {
		ISP_AE_ATTR_EX_S ae_attr;
		if (HI_MPI_ISP_GetAEAttrEx(&ae_attr) != HI_SUCCESS)
			throw IpcamError("get MinGain failed");

		ae_attr.u32AGainMin = value;
		if (HI_MPI_ISP_SetAEAttrEx(&ae_attr) != HI_SUCCESS)
			throw IpcamError("set MinGain failed");
	}
	_min_gain = value;
}

uint32_t HimppVideoISP::Exposure::getMinGain(void)
{
	if (isEnabled()) {
		ISP_AE_ATTR_EX_S ae_attr;
		if (HI_MPI_ISP_GetAEAttrEx(&ae_attr) != HI_SUCCESS)
			throw IpcamError("get exposure MinGain failed");
		return ae_attr.u32AGainMin;
	}
	return _min_gain;
}

void HimppVideoISP::Exposure::setMaxGain(uint32_t value)
{
	if (isEnabled()) {
		ISP_AE_ATTR_EX_S ae_attr;
		if (HI_MPI_ISP_GetAEAttrEx(&ae_attr) != HI_SUCCESS)
			throw IpcamError("get MaxGain failed");

		ae_attr.u32AGainMax = value;
		if (HI_MPI_ISP_SetAEAttrEx(&ae_attr) != HI_SUCCESS)
			throw IpcamError("set MaxGain failed");
	}
	_max_gain = value;
}

uint32_t HimppVideoISP::Exposure::getMaxGain(void)
{
	if (isEnabled()) {
		ISP_AE_ATTR_EX_S ae_attr;
		if (HI_MPI_ISP_GetAEAttrEx(&ae_attr) != HI_SUCCESS)
			throw IpcamError("get MinGain failed");
		return ae_attr.u32AGainMax;
	}
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
		ISP_ME_ATTR_EX_S me_attr;
		if (HI_MPI_ISP_GetMEAttrEx(&me_attr) != HI_SUCCESS)
			throw IpcamError("get ME attr failed");

		me_attr.u32ExpLine = value;
		me_attr.bManualExpLineEnable = HI_TRUE;
		if (HI_MPI_ISP_SetMEAttrEx(&me_attr) != HI_SUCCESS)
			throw IpcamError("set ME attr failed");
	}
	_exp_time = value;
}

uint32_t HimppVideoISP::Exposure::getExposureTime(void)
{
	if (isEnabled()) {
		ISP_ME_ATTR_EX_S me_attr;
		if (HI_MPI_ISP_GetMEAttrEx(&me_attr) != HI_SUCCESS)
			throw IpcamError("get ME attr failed");
		return me_attr.u32ExpLine;
	}
	return _exp_time;
}

void HimppVideoISP::Exposure::setGain(uint32_t value)
{
	if (isEnabled()) {
		ISP_ME_ATTR_EX_S me_attr;
		if (HI_MPI_ISP_GetMEAttrEx(&me_attr) != HI_SUCCESS)
			throw IpcamError("get ME attr failed");

		me_attr.s32AGain = value;
		me_attr.bManualAGainEnable = HI_TRUE;
		if (HI_MPI_ISP_SetMEAttrEx(&me_attr) != HI_SUCCESS)
			throw IpcamError("set ME attr failed");
	}
	_gain = value;
}

uint32_t HimppVideoISP::Exposure::getGain(void)
{
	if (isEnabled()) {
		ISP_ME_ATTR_EX_S me_attr;
		if (HI_MPI_ISP_GetMEAttrEx(&me_attr) != HI_SUCCESS)
			throw IpcamError("get ME attr failed");
		return me_attr.s32AGain;
	}
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
		ISP_OP_TYPE_E wbtype = (value == IVideoSource::Imaging::WhiteBalance::AUTO_WB) ?
			OP_TYPE_AUTO : OP_TYPE_MANUAL;
		if (HI_MPI_ISP_SetWBType(wbtype) != HI_SUCCESS) {
			throw IpcamError("set WhiteBalance mode failed");
		}
	}
}

HimppVideoISP::WhiteBalanceMode HimppVideoISP::WhiteBalance::getWBMode()
{
	if (isEnabled()) {
		ISP_OP_TYPE_E wbmode;
		if (HI_MPI_ISP_GetWBType(&wbmode) == HI_SUCCESS) {
			WhiteBalanceMode mode = (wbmode == OP_TYPE_AUTO) ?
				IVideoSource::Imaging::WhiteBalance::AUTO_WB :
				IVideoSource::Imaging::WhiteBalance::MANUAL_WB;
			return mode;
		}
		throw IpcamError("get WhiteBalance mode failed");
	}
	return _mode;
}

void HimppVideoISP::WhiteBalance::setCbGain(uint32_t value)
{
	if (isEnabled()) {
		ISP_AWB_ATTR_S awb_attr;
		if (HI_MPI_ISP_GetAWBAttr(&awb_attr) != HI_SUCCESS)
			throw IpcamError("Cannot get AWB attr");

		awb_attr.u8BGStrength = (HI_U8)value;
		if (HI_MPI_ISP_SetAWBAttr(&awb_attr) != HI_SUCCESS)
			throw IpcamError("Cannot set AWB attr");
	}
	_cb_gain = value;
}

uint32_t HimppVideoISP::WhiteBalance::getCbGain()
{
	if (isEnabled()) {
		ISP_AWB_ATTR_S awb_attr;
		if (HI_MPI_ISP_GetAWBAttr(&awb_attr) != HI_SUCCESS)
			throw IpcamError("Cannot get AWB mode");
		return awb_attr.u8BGStrength;
	}
	return _cb_gain;
}

void HimppVideoISP::WhiteBalance::setCrGain(uint32_t value)
{
	if (isEnabled()) {
		ISP_AWB_ATTR_S awb_attr;
		if (HI_MPI_ISP_GetAWBAttr(&awb_attr) != HI_SUCCESS)
			throw IpcamError("Cannot get AWB attr");

		awb_attr.u8RGStrength = (HI_U8)value;
		if (HI_MPI_ISP_SetAWBAttr(&awb_attr) != HI_SUCCESS)
			throw IpcamError("Cannot set AWB attr");
	}
	_cr_gain = value;
}

uint32_t HimppVideoISP::WhiteBalance::getCrGain()
{
	if (isEnabled()) {
		if (_mode == IVideoSource::Imaging::WhiteBalance::AUTO_WB) {
			ISP_AWB_ATTR_S awb_attr;
			if (HI_MPI_ISP_GetAWBAttr(&awb_attr) != HI_SUCCESS)
				throw IpcamError("Cannot get AWB mode");
			return awb_attr.u8RGStrength;
		}
	}

	return _cr_gain;
}


void HimppVideoISP::WhiteBalance::setRGain(uint32_t value)
{
	if (isEnabled()) {
		ISP_MWB_ATTR_S mwb_attr;
		if (HI_MPI_ISP_GetMWBAttr(&mwb_attr) != HI_SUCCESS)
			throw IpcamError("Cannot get MWB attr");

		mwb_attr.u16Rgain = (HI_U16)value;
		if (HI_MPI_ISP_SetMWBAttr(&mwb_attr) != HI_SUCCESS)
			throw IpcamError("Cannot set MWB attr");
	}
	_r_gain = value;
}

uint32_t HimppVideoISP::WhiteBalance::getRGain()
{
	if (isEnabled()) {
		ISP_MWB_ATTR_S mwb_attr;
		if (HI_MPI_ISP_GetMWBAttr(&mwb_attr) != HI_SUCCESS)
			throw IpcamError("Cannot get MWB mode");
		return mwb_attr.u16Rgain;
	}
	return _r_gain;
}

void HimppVideoISP::WhiteBalance::setGGain(uint32_t value)
{
	if (isEnabled()) {
		ISP_MWB_ATTR_S mwb_attr;
		if (HI_MPI_ISP_GetMWBAttr(&mwb_attr) != HI_SUCCESS)
			throw IpcamError("Cannot get MWB attr");

		mwb_attr.u16Ggain = (HI_U16)value;
		if (HI_MPI_ISP_SetMWBAttr(&mwb_attr) != HI_SUCCESS)
			throw IpcamError("Cannot set MWB attr");
	}
	_g_gain = value;
}

uint32_t HimppVideoISP::WhiteBalance::getGGain()
{
	if (isEnabled()) {
		ISP_MWB_ATTR_S mwb_attr;
		if (HI_MPI_ISP_GetMWBAttr(&mwb_attr) != HI_SUCCESS)
			throw IpcamError("Cannot get MWB mode");
		return mwb_attr.u16Ggain;
	}
	return _g_gain;
}

void HimppVideoISP::WhiteBalance::setBGain(uint32_t value)
{
	if (isEnabled()) {
		ISP_MWB_ATTR_S mwb_attr;
		if (HI_MPI_ISP_GetMWBAttr(&mwb_attr) != HI_SUCCESS)
			throw IpcamError("Cannot get MWB attr");

		mwb_attr.u16Bgain = (HI_U16)value;
		if (HI_MPI_ISP_SetMWBAttr(&mwb_attr) != HI_SUCCESS)
			throw IpcamError("Cannot set MWB attr");
	}
	_b_gain = value;
}

uint32_t HimppVideoISP::WhiteBalance::getBGain()
{
	if (isEnabled()) {
		ISP_MWB_ATTR_S mwb_attr;
		if (HI_MPI_ISP_GetMWBAttr(&mwb_attr) != HI_SUCCESS)
			throw IpcamError("Cannot get MWB mode");
		return mwb_attr.u16Bgain;
	}
	return _b_gain;
}

