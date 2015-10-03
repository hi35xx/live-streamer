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


HimppVideoISP::HimppVideoISP(HimppVideoSensor *sensor)
    : HimppVideoObject(NULL), video_sensor(sensor)
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
        //goto err_exit_isp;
    }

    if (HI_MPI_ISP_SetInputTiming(pstInputTiming) != HI_SUCCESS) {
        goto err_exit_isp;
    }

    if (pthread_create(&isp_thread, 0, isp_thread_routine, NULL))
        goto err_exit_isp;

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