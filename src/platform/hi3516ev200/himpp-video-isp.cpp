/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/prctl.h>

#include <hi_comm_isp.h>
#include <hi_ae_comm.h>
#include <hi_af_comm.h>
#include <hi_awb_comm.h>
#include <hi_sns_ctrl.h>
#include <mpi_ae.h>
#include <mpi_awb.h>

#include <himpp-common.h>
#include <himpp-video-sensor.h>
#include <himpp-video-viu.h>

#include "himpp-video-isp.h"


//////////////////////////////////////////////////////////////////////////////
// HimppVideoISP
//////////////////////////////////////////////////////////////////////////////

HimppVideoISP::HimppVideoISP(HimppVideoElement* source)
  : VideoElement(VIDEO_ELEMENT(source)), HimppVideoElement(source),
    DefaultVideoSource(DEFAULT_VIDEO_SOURCE(source)),
    _imaging(*this), _isp_dev(0), _isp_thread(-1)
{
	memset(&_sensor_module, 0, sizeof(_sensor_module));
}

HimppVideoISP::~HimppVideoISP()
{
}

VideoSource::Imaging& HimppVideoISP::imaging()
{
	return dynamic_cast<VideoSource::Imaging&>(_imaging);
}

bool HimppVideoISP::loadSensorModule()
{
	const char* name = videoSensor()->getSensorName();
	const char* path = videoSensor()->getModulePath();
	const char *obj_name = videoSensor()->getObjectName();
	ALG_LIB_S aelib;
	ALG_LIB_S awblib;

	if (!path || !obj_name) {
		HIMPP_PRINT("Sensor module '%s' not found\n", name);
		return false;
	}

	_sensor_module.handle = dlopen(path, RTLD_LAZY);
	if (_sensor_module.handle == NULL) {
		HIMPP_PRINT("Failed to load sensor module '%s'\n", path);
		return false;
	}

	_sensor_module.object = (ISP_SNS_OBJ_S*)dlsym(_sensor_module.handle, obj_name);

	if (!_sensor_module.object \
	    || !_sensor_module.object->pfnRegisterCallback \
	    || !_sensor_module.object->pfnUnRegisterCallback \
	    || !_sensor_module.object->pfnSetBusInfo) {
		HIMPP_PRINT("Incorrect sensor module '%s'.\n", name);
		goto err_dlclose;
	}

	aelib.s32Id = _isp_dev;
	awblib.s32Id = _isp_dev;
	strncpy(aelib.acLibName, HI_AE_LIB_NAME, sizeof(aelib.acLibName));
	strncpy(awblib.acLibName, HI_AWB_LIB_NAME, sizeof(awblib.acLibName));
	if (_sensor_module.object->pfnRegisterCallback(_isp_dev, &aelib, &awblib) != HI_SUCCESS) {
		HIMPP_PRINT("Failed to register sensor\n");
		goto err_dlclose;
	}
#if 0
	ISP_SNS_COMMBUS_U businfo = { .s8I2cDev = 0 };
	if (_sensor_module.object->pfnSetBusInfo(_isp_dev, businfo) != HI_SUCCESS) {
		HIMPP_PRINT("Failed to set bus info\n");
		goto err_dlclose;
	}
#endif

	return true;

err_dlclose:
	dlclose(_sensor_module.handle);
	_sensor_module.handle = NULL;
	_sensor_module.object = NULL;

	return false;
}

bool HimppVideoISP::unloadSensorModule()
{
	ALG_LIB_S aelib;
	ALG_LIB_S awblib;

	if (_sensor_module.handle) {
		if (_sensor_module.object && _sensor_module.object->pfnUnRegisterCallback) {
			aelib.s32Id = _isp_dev;
			awblib.s32Id = _isp_dev;
			strncpy(aelib.acLibName, HI_AE_LIB_NAME, sizeof(aelib.acLibName));
			strncpy(awblib.acLibName, HI_AWB_LIB_NAME, sizeof(awblib.acLibName));
			_sensor_module.object->pfnUnRegisterCallback(_isp_dev, &aelib, &awblib);
		}
		dlclose(_sensor_module.handle);
		_sensor_module.handle = NULL;
	}
	return true;
}

bool HimppVideoISP::registerAlgorithm()
{
	ALG_LIB_S stLib;
	HI_S32 s32Ret;

	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AE_LIB_NAME);
	if ((s32Ret = HI_MPI_AE_Register(_isp_dev, &stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AE_Register failed [%#x]\n", s32Ret);
		return false;
	}

	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
	if ((s32Ret = HI_MPI_AWB_Register(_isp_dev, &stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AWB_Register failed [%#x]\n", s32Ret);
		return false;
	}

	/*
	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AF_LIB_NAME);
	if ((s32Ret = HI_MPI_AF_Register(_isp_dev, &stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AF_Register failed [%#x]\n", s32Ret);
		return false;
	}
	*/

	return true;
}

bool HimppVideoISP::unregisterAlgorithm()
{
	ALG_LIB_S stLib;
	HI_S32 s32Ret;

	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AE_LIB_NAME);
	if ((s32Ret = HI_MPI_AE_UnRegister(_isp_dev, &stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AE_UnRegister failed [%#x]\n", s32Ret);
	}

	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
	if ((s32Ret = HI_MPI_AWB_UnRegister(_isp_dev, &stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AWB_UnRegister failed [%#x]\n", s32Ret);
	}

	/*
	stLib.s32Id = 0;
	strcpy(stLib.acLibName, HI_AF_LIB_NAME);
	if ((s32Ret = HI_MPI_AF_UnRegister(_isp_dev, &stLib)) != HI_SUCCESS) {
		HIMPP_PRINT("HI_MPI_AF_UnRegister failed [%#x]\n", s32Ret);
	}
	*/

	return true;
}

void *HimppVideoISP::isp_thread_routine(void *arg)
{
	prctl(PR_SET_NAME, (unsigned long)"ISP");

	return (void*)HI_MPI_ISP_Run(0);
}

void HimppVideoISP::doEnableElement()
{
	HI_S32 s32Ret;
	ISP_PUB_ATTR_S ispPubAttr = *(ISP_PUB_ATTR_S*)*videoSensor();
	//ISP_WDR_MODE_S stWdrMode;

	if (!loadSensorModule())
		throw IpcamError("Failed to load sensor module");

	if (!registerAlgorithm())
		goto err_unload_mod;

	/* isp mem init */
	if ((s32Ret = HI_MPI_ISP_MemInit(_isp_dev)) != HI_SUCCESS) {
		fprintf(stderr, "HI_MPI_ISP_MemInit failed [%#x]\n", s32Ret);
		goto err_unreg_alg;
	}

	// Find the HimppViDev to get resolution and crop info
	for (auto e = source(); e; e = e->source()) {
		HimppViDev *videv = dynamic_cast<HimppViDev*>(e);
		if (videv) {
			Resolution res = videv->resolution();
			POINT_S offset = videv->getCropOffset();
			uint32_t in_x = ispPubAttr.stSnsSize.u32Width;
			uint32_t in_y = ispPubAttr.stSnsSize.u32Height;
			ispPubAttr.stWndRect.s32X = (in_x - res.width()) / 2 + offset.s32X;
			ispPubAttr.stWndRect.s32Y = (in_y - res.height()) / 2 + offset.s32Y;
			ispPubAttr.stWndRect.u32Width = res.width();
			ispPubAttr.stWndRect.u32Height = res.height();

			break;
		}
	}
	if (HI_MPI_ISP_SetPubAttr(_isp_dev, &ispPubAttr) != HI_SUCCESS)
		goto err_unreg_alg;

	if ((s32Ret = HI_MPI_ISP_Init(_isp_dev)) != HI_SUCCESS)
		goto err_unreg_alg;

	if (pthread_create(&_isp_thread, 0, isp_thread_routine, NULL))
		goto err_exit_isp;

	// initialize Exposure
	ISP_EXPOSURE_ATTR_S exp_attr;
	if (HI_MPI_ISP_GetExposureAttr(_isp_dev, &exp_attr) == HI_SUCCESS) {
		exp_attr.enOpType = (_imaging.exposure().getMode() == AUTO_EXPOSURE) ?
			OP_TYPE_AUTO : OP_TYPE_MANUAL;

		exp_attr.stAuto.enAEMode = (_imaging.exposure().getPriority() == LOWNOISE_PRIORITY) ? \
			AE_MODE_SLOW_SHUTTER : AE_MODE_FIX_FRAME_RATE;
		exp_attr.stAuto.stExpTimeRange.u32Min = _imaging.exposure().getMinExposureTime();
		exp_attr.stAuto.stExpTimeRange.u32Max = _imaging.exposure().getMaxExposureTime();
		exp_attr.stAuto.stSysGainRange.u32Min = _imaging.exposure().getMinGain();
		exp_attr.stAuto.stSysGainRange.u32Max = _imaging.exposure().getMaxGain();
		exp_attr.stAuto.u8Compensation = _imaging.exposure().getCompensation();

		switch (_imaging.antiflicker().getMode()) {
		case ANTIFLICKER_OFF:
			exp_attr.stAuto.stAntiflicker.bEnable = HI_FALSE;
			break;
		case ANTIFLICKER_NORMAL:
			exp_attr.stAuto.stAntiflicker.bEnable = HI_TRUE;
			exp_attr.stAuto.stAntiflicker.enMode = ISP_ANTIFLICKER_NORMAL_MODE;
			break;
		case ANTIFLICKER_AUTO:
			exp_attr.stAuto.stAntiflicker.bEnable = HI_TRUE;
			exp_attr.stAuto.stAntiflicker.enMode = ISP_ANTIFLICKER_AUTO_MODE;
			break;
		}
		exp_attr.stAuto.stAntiflicker.u8Frequency = _imaging.antiflicker().getFrequency();

		exp_attr.stManual.u32ExpTime = _imaging.exposure().getExposureTime();
		exp_attr.stManual.u32ISPDGain = _imaging.exposure().getGain();

		if (HI_MPI_ISP_SetExposureAttr(_isp_dev, &exp_attr) != HI_SUCCESS) {
			fprintf(stderr, "HI_MPI_ISP_SetAEAttrEx failed\n");
		}
	}

	// Initialize WhiteBalance
	ISP_WB_ATTR_S wb_attr;
	if (HI_MPI_ISP_GetWBAttr(_isp_dev, &wb_attr) == HI_SUCCESS) {
		wb_attr.enOpType = (_imaging.whitebalance().getMode() == AUTO_WB) ?
			OP_TYPE_AUTO : OP_TYPE_MANUAL;

		wb_attr.stAuto.u8BGStrength = (HI_U8)_imaging.whitebalance().getCbGain();
		wb_attr.stAuto.u8RGStrength = (HI_U8)_imaging.whitebalance().getCrGain();

		wb_attr.stManual.u16Rgain = (HI_U16)_imaging.whitebalance().getRGain();
		wb_attr.stManual.u16Grgain = (HI_U16)_imaging.whitebalance().getGGain();
		wb_attr.stManual.u16Bgain = (HI_U16)_imaging.whitebalance().getBGain();
		if (HI_MPI_ISP_SetWBAttr(_isp_dev, &wb_attr) != HI_SUCCESS)
			fprintf(stderr, "HI_MPI_ISP_SetWBType failed\n");
	}

	// Initialize WideDynamicRange
	ISP_DRC_ATTR_S wdr_attr;
	if (HI_MPI_ISP_GetDRCAttr(_isp_dev, &wdr_attr) == HI_SUCCESS) {
		wdr_attr.bEnable = \
			(_imaging.widedynamicrange().getMode() == WDR_ON) ?
				HI_TRUE : HI_FALSE;
		wdr_attr.stAuto.u16Strength = _imaging.widedynamicrange().getLevel();
		if (HI_MPI_ISP_SetDRCAttr(_isp_dev, &wdr_attr) != HI_SUCCESS)
			fprintf(stderr, "HI_MPI_ISP_SetDRCAttr failed\n");
	}

	// Initialize Gamma
	ISP_GAMMA_ATTR_S gamma_attr;
	if (_imaging.gamma().getCurveData().size() == ARRAY_SIZE(gamma_attr.u16Table)) {
		gamma_attr.bEnable = HI_TRUE;
		gamma_attr.enCurveType = ISP_GAMMA_CURVE_USER_DEFINE;
		GammaCurveData &curve_data = _imaging.gamma().getCurveData();
		for (unsigned i = 0; i < curve_data.size(); i++) {
			gamma_attr.u16Table[i] = curve_data[i];
		}
	}
	else {
		gamma_attr.bEnable = HI_TRUE;
		gamma_attr.enCurveType = ISP_GAMMA_CURVE_DEFAULT;
		memset(gamma_attr.u16Table, 0, sizeof(gamma_attr.u16Table));
	}
	if (HI_MPI_ISP_SetGammaAttr(_isp_dev, &gamma_attr) != HI_SUCCESS)
		fprintf(stderr, "HI_MPI_ISP_SetGammaAttr failed\n");

	return;

err_exit_isp:
	HI_MPI_ISP_Exit(_isp_dev);
err_unreg_alg:
	unregisterAlgorithm();
err_unload_mod:
	unloadSensorModule();

	throw IpcamError("Failed to enable ISP");
}

void HimppVideoISP::doDisableElement()
{
	HI_MPI_ISP_Exit(_isp_dev);
	pthread_join(_isp_thread, 0);
	unregisterAlgorithm();
	unloadSensorModule();
}

HimppVideoSensor* HimppVideoISP::videoSensor()
{
	HimppVideoSensor *sensor = NULL;
	for (auto el = source(); el; el = el->source()) {
		HimppViDev *videv = dynamic_cast<HimppViDev*>(el);
		if (videv) {
			sensor = videv->sensor();
			break;
		}
	}
	return sensor;
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoISP::Imaging::AntiFlicker
//////////////////////////////////////////////////////////////////////////////

HimppVideoISP::Imaging::AntiFlicker::AntiFlicker(Imaging& imaging)
  : DefaultVideoSource::Imaging::AntiFlicker(dynamic_cast<DefaultVideoSource::Imaging&>(imaging)),
    _mode(ANTIFLICKER_OFF),
    _frequency(50)
{
}

HimppVideoISP::Imaging::AntiFlicker::~AntiFlicker()
{
}

void HimppVideoISP::Imaging::AntiFlicker::setMode(AntiFlickerMode mode)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (isp.is_enabled()) {
		ISP_EXPOSURE_ATTR_S exp_attr;
		if (HI_MPI_ISP_GetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("get exposure attr failed");

		switch (mode) {
		case ANTIFLICKER_OFF:
			exp_attr.stAuto.stAntiflicker.bEnable = HI_FALSE;
			break;
		case ANTIFLICKER_NORMAL:
			exp_attr.stAuto.stAntiflicker.bEnable = HI_TRUE;
			exp_attr.stAuto.stAntiflicker.enMode = ISP_ANTIFLICKER_NORMAL_MODE;
			break;
		case ANTIFLICKER_AUTO:
			exp_attr.stAuto.stAntiflicker.bEnable = HI_TRUE;
			exp_attr.stAuto.stAntiflicker.enMode = ISP_ANTIFLICKER_AUTO_MODE;
			break;
		}

		if (HI_MPI_ISP_SetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_mode = mode;
}

AntiFlickerMode HimppVideoISP::Imaging::AntiFlicker::getMode()
{
	return _mode;
}

void HimppVideoISP::Imaging::AntiFlicker::setFrequency(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (isp.is_enabled()) {
		ISP_EXPOSURE_ATTR_S exp_attr;
		if (HI_MPI_ISP_GetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("get exposure attr failed");
		exp_attr.stAuto.stAntiflicker.u8Frequency = value;
		if (HI_MPI_ISP_SetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_frequency = value;
}

uint32_t HimppVideoISP::Imaging::AntiFlicker::getFrequency()
{
	return _frequency;
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoISP::Imaging::Exposure
//////////////////////////////////////////////////////////////////////////////

HimppVideoISP::Imaging::Exposure::Exposure(Imaging& imaging)
  : DefaultVideoSource::Imaging::Exposure(dynamic_cast<DefaultVideoSource::Imaging&>(imaging)),
    _mode(AUTO_EXPOSURE),
    _priority(LOWNOISE_PRIORITY),
    _min_exp_time(2), _max_exp_time(200000),
    _min_gain(1024), _max_gain(65536),
    _min_iris(0), _max_iris(0),
    _exp_time(8192), _compensation(56), _gain(16384), _iris(0)
{
}

HimppVideoISP::Imaging::Exposure::~Exposure()
{
}

void HimppVideoISP::Imaging::Exposure::setMode(ExposureMode mode)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (isp.is_enabled()) {
		ISP_EXPOSURE_ATTR_S exp_attr;
		if (HI_MPI_ISP_GetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("get exposure attr failed");
		exp_attr.enOpType = (mode == AUTO_EXPOSURE) ? OP_TYPE_AUTO : OP_TYPE_MANUAL;
		if (HI_MPI_ISP_SetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_mode = mode;
}

ExposureMode HimppVideoISP::Imaging::Exposure::getMode()
{
	return _mode;
}

void HimppVideoISP::Imaging::Exposure::setPriority(ExposurePriority priority)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (isp.is_enabled()) {
		ISP_EXPOSURE_ATTR_S exp_attr;
		if (HI_MPI_ISP_GetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("get ExposurePriority failed");
		exp_attr.stAuto.enAEMode =
			(priority == LOWNOISE_PRIORITY) ?
				AE_MODE_SLOW_SHUTTER : AE_MODE_FIX_FRAME_RATE;
		if (HI_MPI_ISP_SetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("set ExposurePriority failed");
	}
	_priority = priority;
}

ExposurePriority HimppVideoISP::Imaging::Exposure::getPriority()
{
	return _priority;
}

void HimppVideoISP::Imaging::Exposure::setMinExposureTime(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (isp.is_enabled()) {
		ISP_EXPOSURE_ATTR_S exp_attr;
		if (HI_MPI_ISP_GetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("get exposure attr failed");

		exp_attr.stAuto.stExpTimeRange.u32Min = value;
		if (HI_MPI_ISP_SetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_min_exp_time = value;
}

uint32_t HimppVideoISP::Imaging::Exposure::getMinExposureTime(void)
{
	return _min_exp_time;
}

void HimppVideoISP::Imaging::Exposure::setMaxExposureTime(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (isp.is_enabled()) {
		ISP_EXPOSURE_ATTR_S exp_attr;
		if (HI_MPI_ISP_GetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("get exposure attr failed");

		exp_attr.stAuto.stExpTimeRange.u32Max = value;
		if (HI_MPI_ISP_SetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_max_exp_time = value;

}

uint32_t HimppVideoISP::Imaging::Exposure::getMaxExposureTime(void)
{
	return _max_exp_time;
}

void HimppVideoISP::Imaging::Exposure::setMinGain(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (isp.is_enabled()) {
		ISP_EXPOSURE_ATTR_S exp_attr;
		if (HI_MPI_ISP_GetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("get exposure attr failed");

		exp_attr.stAuto.stSysGainRange.u32Min = value;
		if (HI_MPI_ISP_SetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_min_gain = value;
}

uint32_t HimppVideoISP::Imaging::Exposure::getMinGain(void)
{
	return _min_gain;
}

void HimppVideoISP::Imaging::Exposure::setMaxGain(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (isp.is_enabled()) {
		ISP_EXPOSURE_ATTR_S exp_attr;
		if (HI_MPI_ISP_GetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("get exposure attr failed");

		exp_attr.stAuto.stSysGainRange.u32Max = value;
		if (HI_MPI_ISP_SetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_max_gain = value;
}

uint32_t HimppVideoISP::Imaging::Exposure::getMaxGain(void)
{
	return _max_gain;
}

void HimppVideoISP::Imaging::Exposure::setMinIris(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (isp.is_enabled()) {
	}
	_min_iris = value;
}

uint32_t HimppVideoISP::Imaging::Exposure::getMinIris(void)
{
	return _min_iris;
}

void HimppVideoISP::Imaging::Exposure::setMaxIris(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (isp.is_enabled()) {
	}
	_max_iris = value;
}

uint32_t HimppVideoISP::Imaging::Exposure::getMaxIris(void)
{
	return _max_iris;
}

uint32_t HimppVideoISP::Imaging::Exposure::getCompensation(void)
{
	return _compensation;
}

void HimppVideoISP::Imaging::Exposure::setCompensation(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (isp.is_enabled()) {
		ISP_EXPOSURE_ATTR_S exp_attr;
		if (HI_MPI_ISP_GetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("get exposure attr failed");

		exp_attr.stAuto.u8Compensation = (HI_U8)value;
		if (HI_MPI_ISP_SetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_compensation = value;
}

void HimppVideoISP::Imaging::Exposure::setExposureTime(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (isp.is_enabled()) {
		ISP_EXPOSURE_ATTR_S exp_attr;
		if (HI_MPI_ISP_GetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("get exposure attr failed");

		exp_attr.stManual.u32ExpTime = value;
		if (HI_MPI_ISP_SetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_exp_time = value;
}

uint32_t HimppVideoISP::Imaging::Exposure::getExposureTime(void)
{
	return _exp_time;
}

void HimppVideoISP::Imaging::Exposure::setGain(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (isp.is_enabled()) {
		ISP_EXPOSURE_ATTR_S exp_attr;
		if (HI_MPI_ISP_GetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("get exposure attr failed");

		exp_attr.stManual.u32ISPDGain = value;
		if (HI_MPI_ISP_SetExposureAttr(isp.ispDev(), &exp_attr) != HI_SUCCESS)
			throw IpcamError("set exposure attr failed");
	}
	_gain = value;
}

uint32_t HimppVideoISP::Imaging::Exposure::getGain(void)
{
	return _gain;
}

void HimppVideoISP::Imaging::Exposure::setIris(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (isp.is_enabled()) {
	}
	_iris = value;
}

uint32_t HimppVideoISP::Imaging::Exposure::getIris(void)
{
	return _iris;
}

void HimppVideoISP::Imaging::Exposure::getStateInfo(StateInfo& state)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (!isp.is_enabled()) {
		throw IpcamError("ISP is not enabled");
	}

	ISP_EXP_INFO_S exp_info;
	if (HI_MPI_ISP_QueryExposureInfo(isp.ispDev(), &exp_info) != HI_SUCCESS)
		throw IpcamError("get exposure info failed");

	state.ExposureTime = exp_info.u32ExpTime;
	state.AGain = exp_info.u32AGain;
	state.DGain = exp_info.u32DGain;
	state.ISPDGain = exp_info.u32ISPDGain;
	state.Exposure = exp_info.u32Exposure;
	state.AverageLuminance = exp_info.u8AveLum;
	state.FrameRate = exp_info.u32Fps;
	state.ISO = exp_info.u32ISO;
}

//////////////////////////////////////////////////////////////////////////////
// HimppVideoISP::Imaging::WhiteBalance
//////////////////////////////////////////////////////////////////////////////

HimppVideoISP::Imaging::WhiteBalance::WhiteBalance(Imaging& imaging)
  : DefaultVideoSource::Imaging::WhiteBalance(dynamic_cast<DefaultVideoSource::Imaging&>(imaging)),
    _mode(AUTO_WB),
    _cb_gain(128), _cr_gain(128),
    _r_gain(256), _g_gain(256), _b_gain(256)
{
}

HimppVideoISP::Imaging::WhiteBalance::~WhiteBalance()
{
}

void HimppVideoISP::Imaging::WhiteBalance::setMode(WhiteBalanceMode value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());

	if (isp.is_enabled()) {
		ISP_WB_ATTR_S wb_attr;
		if (HI_MPI_ISP_GetWBAttr(isp.ispDev(), &wb_attr) != HI_SUCCESS)
			throw IpcamError("get whitebalance attr failed");

		wb_attr.enOpType = (value == AUTO_WB) ? OP_TYPE_AUTO : OP_TYPE_MANUAL;
		if (HI_MPI_ISP_SetWBAttr(isp.ispDev(), &wb_attr) != HI_SUCCESS)
			throw IpcamError("get whitebalance attr failed");
	}

	_mode = value;
}

WhiteBalanceMode HimppVideoISP::Imaging::WhiteBalance::getMode()
{
	return _mode;
}

void HimppVideoISP::Imaging::WhiteBalance::setCbGain(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());

	if (isp.is_enabled()) {
		ISP_WB_ATTR_S wb_attr;
		if (HI_MPI_ISP_GetWBAttr(isp.ispDev(), &wb_attr) != HI_SUCCESS)
			throw IpcamError("get whitebalance attr failed");

		wb_attr.stAuto.u8BGStrength = value;
		if (HI_MPI_ISP_SetWBAttr(isp.ispDev(), &wb_attr) != HI_SUCCESS)
			throw IpcamError("get whitebalance attr failed");
	}
	_cb_gain = value;
}

uint32_t HimppVideoISP::Imaging::WhiteBalance::getCbGain()
{
	return _cb_gain;
}

void HimppVideoISP::Imaging::WhiteBalance::setCrGain(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());

	if (isp.is_enabled()) {
		ISP_WB_ATTR_S wb_attr;
		if (HI_MPI_ISP_GetWBAttr(isp.ispDev(), &wb_attr) != HI_SUCCESS)
			throw IpcamError("get whitebalance attr failed");

		wb_attr.stAuto.u8RGStrength = value;
		if (HI_MPI_ISP_SetWBAttr(isp.ispDev(), &wb_attr) != HI_SUCCESS)
			throw IpcamError("get whitebalance attr failed");
	}
	_cr_gain = value;
}

uint32_t HimppVideoISP::Imaging::WhiteBalance::getCrGain()
{
	return _cr_gain;
}


void HimppVideoISP::Imaging::WhiteBalance::setRGain(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());

	if (isp.is_enabled()) {
		ISP_WB_ATTR_S wb_attr;
		if (HI_MPI_ISP_GetWBAttr(isp.ispDev(), &wb_attr) != HI_SUCCESS)
			throw IpcamError("get whitebalance attr failed");

		wb_attr.stManual.u16Rgain = value;
		if (HI_MPI_ISP_SetWBAttr(isp.ispDev(), &wb_attr) != HI_SUCCESS)
			throw IpcamError("get whitebalance attr failed");
	}
	_r_gain = value;
}

uint32_t HimppVideoISP::Imaging::WhiteBalance::getRGain()
{
	return _r_gain;
}

void HimppVideoISP::Imaging::WhiteBalance::setGGain(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());

	if (isp.is_enabled()) {
		ISP_WB_ATTR_S wb_attr;
		if (HI_MPI_ISP_GetWBAttr(isp.ispDev(), &wb_attr) != HI_SUCCESS)
			throw IpcamError("get whitebalance attr failed");

		wb_attr.stManual.u16Grgain = value;
		if (HI_MPI_ISP_SetWBAttr(isp.ispDev(), &wb_attr) != HI_SUCCESS)
			throw IpcamError("get whitebalance attr failed");
	}
	_g_gain = value;
}

uint32_t HimppVideoISP::Imaging::WhiteBalance::getGGain()
{
	return _g_gain;
}

void HimppVideoISP::Imaging::WhiteBalance::setBGain(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());

	if (isp.is_enabled()) {
		ISP_WB_ATTR_S wb_attr;
		if (HI_MPI_ISP_GetWBAttr(isp.ispDev(), &wb_attr) != HI_SUCCESS)
			throw IpcamError("get whitebalance attr failed");

		wb_attr.stManual.u16Bgain = value;
		if (HI_MPI_ISP_SetWBAttr(isp.ispDev(), &wb_attr) != HI_SUCCESS)
			throw IpcamError("get whitebalance attr failed");
	}
	_b_gain = value;
}

uint32_t HimppVideoISP::Imaging::WhiteBalance::getBGain()
{
	return _b_gain;
}

//////////////////////////////////////////////////////////////////////////////
// HimppVideoISP::Imaging::WideDynamicRange
//////////////////////////////////////////////////////////////////////////////

HimppVideoISP::Imaging::WideDynamicRange::WideDynamicRange(Imaging& imaging)
  : DefaultVideoSource::Imaging::WideDynamicRange(dynamic_cast<DefaultVideoSource::Imaging&>(imaging)),
	_mode(WDR_OFF), _level(0)
{
}

HimppVideoISP::Imaging::WideDynamicRange::~WideDynamicRange()
{
}

void HimppVideoISP::Imaging::WideDynamicRange::setMode(WDRMode value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());

	if (isp.is_enabled()) {
		ISP_DRC_ATTR_S wdr_attr;
		if (HI_MPI_ISP_GetDRCAttr(isp.ispDev(), &wdr_attr) != HI_SUCCESS)
			throw IpcamError("get WDR attr failed");

		wdr_attr.bEnable = (value == WDR_ON) ? HI_TRUE : HI_FALSE;
		if (HI_MPI_ISP_SetDRCAttr(isp.ispDev(), &wdr_attr) != HI_SUCCESS)
			throw IpcamError("set WDR attr failed");
	}
	_mode = value;
}

WDRMode HimppVideoISP::Imaging::WideDynamicRange::getMode()
{
	return _mode;
}

void HimppVideoISP::Imaging::WideDynamicRange::setLevel(uint32_t value)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());

	if (isp.is_enabled()) {
		ISP_DRC_ATTR_S wdr_attr;
		if (HI_MPI_ISP_GetDRCAttr(isp.ispDev(), &wdr_attr) != HI_SUCCESS)
			throw IpcamError("get WDR attr failed");

		wdr_attr.stAuto.u16Strength = (HI_U16)value;
		if (HI_MPI_ISP_SetDRCAttr(isp.ispDev(), &wdr_attr) != HI_SUCCESS)
			throw IpcamError("set WDR attr failed");
	}
	_level = value;
}

uint32_t HimppVideoISP::Imaging::WideDynamicRange::getLevel()
{
	return _level;
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoISP::Imaging::Gamma
//////////////////////////////////////////////////////////////////////////////

HimppVideoISP::Imaging::Gamma::Gamma(Imaging &imaging)
  : DefaultVideoSource::Imaging::Gamma(dynamic_cast<DefaultVideoSource::Imaging&>(imaging))
{
}

HimppVideoISP::Imaging::Gamma::~Gamma()
{
}

void HimppVideoISP::Imaging::Gamma::setCurveData(GammaCurveData& value)
{
	ISP_GAMMA_ATTR_S gamma_attr;

	if ((value.size() != 0) && (value.size() != ARRAY_SIZE(gamma_attr.u16Table)))
		throw IpcamError("invalid gamma data");

	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());
	if (isp.is_enabled()) {
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
		if (HI_MPI_ISP_SetGammaAttr(isp.ispDev(), &gamma_attr) != HI_SUCCESS)
			throw IpcamError("failed to set gamma attr");
	}
	_curve_data = value;
}

GammaCurveData& HimppVideoISP::Imaging::Gamma::getCurveData()
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(imaging().videoSource());

	if (isp.is_enabled()) {
		ISP_GAMMA_ATTR_S gamma_attr;

		if (HI_MPI_ISP_GetGammaAttr(isp.ispDev(), &gamma_attr) != HI_SUCCESS)
			throw IpcamError("failed to get gamma attr");

		_curve_data.resize(ARRAY_SIZE(gamma_attr.u16Table));
		for (unsigned i = 0; i < _curve_data.size(); i++) {
			_curve_data[i] = gamma_attr.u16Table[i];
		}
	}

	return _curve_data;
}


//////////////////////////////////////////////////////////////////////////////
// HimppVideoISP::Imaging
//////////////////////////////////////////////////////////////////////////////

HimppVideoISP::Imaging::Imaging(HimppVideoISP& video_isp)
  : DefaultVideoSource::Imaging(dynamic_cast<DefaultVideoSource&>(video_isp)),
    _antiflicker(*this), _exposure(*this), _whitebalance(*this),
    _widedynamicrange(*this), _gamma(*this),
    _brightness(50), _contrast(50), _chroma(50), _saturation(50)
{
}

HimppVideoISP::Imaging::~Imaging()
{
}

uint32_t HimppVideoISP::Imaging::getBrightness()
{
	return _brightness;
}

void HimppVideoISP::Imaging::setBrightness(uint32_t val)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(videoSource());
	if (isp.is_enabled()) {
		ISP_CSC_ATTR_S attr;
		if (HI_MPI_ISP_GetCSCAttr(isp.ispDev(), &attr) != HI_SUCCESS)
			throw IpcamError("Failed to get CSC");

		attr.u8Luma = val;
		if (HI_MPI_ISP_SetCSCAttr(isp.ispDev(), &attr) != HI_SUCCESS)
			throw IpcamError("Failed to set CSC");
	}

	_brightness = val;
}

uint32_t HimppVideoISP::Imaging::getContrast()
{
	return _contrast;
}

void HimppVideoISP::Imaging::setContrast(uint32_t val)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(videoSource());
	if (isp.is_enabled()) {
		ISP_CSC_ATTR_S attr;
		if (HI_MPI_ISP_GetCSCAttr(isp.ispDev(), &attr) != HI_SUCCESS)
			throw IpcamError("Failed to get CSC");

		attr.u8Contr = val;
		if (HI_MPI_ISP_SetCSCAttr(isp.ispDev(), &attr) != HI_SUCCESS)
			throw IpcamError("Failed to set CSC");
	}

	_contrast = val;
}

uint32_t HimppVideoISP::Imaging::getChroma()
{
	return _chroma;
}

void HimppVideoISP::Imaging::setChroma(uint32_t val)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(videoSource());
	if (isp.is_enabled()) {
		ISP_CSC_ATTR_S attr;
		if (HI_MPI_ISP_GetCSCAttr(isp.ispDev(), &attr) != HI_SUCCESS)
			throw IpcamError("Failed to get CSC");

		attr.u8Hue = val;
		if (HI_MPI_ISP_SetCSCAttr(isp.ispDev(), &attr) != HI_SUCCESS)
			throw IpcamError("Failed to set CSC");
	}

	_chroma = val;
}

uint32_t HimppVideoISP::Imaging::getSaturation()
{
	return _saturation;
}

void HimppVideoISP::Imaging::setSaturation(uint32_t val)
{
	HimppVideoISP& isp = dynamic_cast<HimppVideoISP&>(videoSource());
	if (isp.is_enabled()) {
		ISP_CSC_ATTR_S attr;
		if (HI_MPI_ISP_GetCSCAttr(isp.ispDev(), &attr) != HI_SUCCESS)
			throw IpcamError("Failed to get CSC");

		attr.u8Satu = val;
		if (HI_MPI_ISP_SetCSCAttr(isp.ispDev(), &attr) != HI_SUCCESS)
			throw IpcamError("Failed to set CSC");
	}

	_saturation = val;
}

VideoSource::Imaging::AntiFlicker& HimppVideoISP::Imaging::antiflicker()
{
	return dynamic_cast<VideoSource::Imaging::AntiFlicker&>(_antiflicker);
}

VideoSource::Imaging::Exposure& HimppVideoISP::Imaging::exposure()
{
	return dynamic_cast<VideoSource::Imaging::Exposure&>(_exposure);
}

VideoSource::Imaging::WhiteBalance& HimppVideoISP::Imaging::whitebalance()
{
	return dynamic_cast<VideoSource::Imaging::WhiteBalance&>(_whitebalance);
}

VideoSource::Imaging::WideDynamicRange&	HimppVideoISP::Imaging::widedynamicrange()
{
	return dynamic_cast<VideoSource::Imaging::WideDynamicRange&>(_widedynamicrange);
}

VideoSource::Imaging::Gamma& HimppVideoISP::Imaging::gamma()
{
	return dynamic_cast<VideoSource::Imaging::Gamma&>(_gamma);
}
