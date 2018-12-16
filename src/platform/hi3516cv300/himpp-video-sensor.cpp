/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-sensor.cpp
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

#include <memory.h>
#include "himpp-video-sensor.h"


struct HIMPP_SENSOR_CONFIG
{
	 const char*		name;
	 const char*		module_path;
	 combo_dev_attr_t*	combo_dev_attr;
	 ISP_PUB_ATTR_S*	isp_pub_attr;
	 VI_DEV_ATTR_S*		vi_dev_attr;
};

/***************************************************************************
 * Aptina AR0237
 ***************************************************************************/

static combo_dev_attr_t aptina_ar0237_combo_dev_attr = {
	.devno = 0,
	.input_mode = INPUT_MODE_HISPI, {
		.lvds_attr = {
			.img_size = { 1920, 1080 },
			.raw_data_type = RAW_DATA_12BIT,
			.wdr_mode = HI_WDR_MODE_NONE,
			.sync_mode = LVDS_SYNC_MODE_SOF,
			.vsync_type = {},
			.fid_type = {},
			.data_endian = LVDS_ENDIAN_LITTLE,
			.sync_code_endian = LVDS_ENDIAN_LITTLE,
			.lane_id = { 0, 1, 2, 3 },
			.sync_code = {
				{	// PYH0_lane0
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x043, 0x047, 0x041, 0x045 },
					{ 0x023, 0x027, 0x021, 0x025 },
					{ 0x083, 0x087, 0x081, 0x085 }
				},
				{	// PYH0_lane1
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x043, 0x047, 0x041, 0x045 },
					{ 0x023, 0x027, 0x021, 0x025 },
					{ 0x083, 0x087, 0x081, 0x085 }
				},
				{	// PYH0_lane2
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x043, 0x047, 0x041, 0x045 },
					{ 0x023, 0x027, 0x021, 0x025 },
					{ 0x083, 0x087, 0x081, 0x085 }
				},
				{	// PYH0_lane3
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x043, 0x047, 0x041, 0x045 },
					{ 0x023, 0x027, 0x021, 0x025 },
					{ 0x083, 0x087, 0x081, 0x085 }
				}
			}
		}
	}
};

static ISP_PUB_ATTR_S aptina_ar0237_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_GRBG
};

static VI_DEV_ATTR_S aptina_ar0237_dev_attr = {
	.enIntfMode     = VI_MODE_MIPI,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32CompMask   = { 0xFFF00000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.s32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_INPUT_DATA_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_PULSE,
		.enVsyncNeg     = VI_VSYNC_NEG_LOW,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 0, .u32HsyncAct = 1280, .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,   .u32VsyncVact = 720, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0,  .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
		}
	},
	.enDataPath         = VI_PATH_ISP,
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataRev           = HI_FALSE,
	.stDevRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080
	}
};

static HIMPP_SENSOR_CONFIG aptina_ar0237_sensor_config = {
	.name               = "ar0237",
	.module_path        = "libsns_ar0237.so",
	.combo_dev_attr     = &aptina_ar0237_combo_dev_attr,
	.isp_pub_attr       = &aptina_ar0237_pub_attr,
	.vi_dev_attr        = &aptina_ar0237_dev_attr
};

/***************************************************************************
 * OmniVision OV2718
 ***************************************************************************/

static combo_dev_attr_t omnivision_ov2718_combo_dev_attr = {
	.devno = 0,
	.input_mode = INPUT_MODE_MIPI, {
		.mipi_attr = {
			.raw_data_type = RAW_DATA_12BIT,
			.wdr_mode = HI_MIPI_WDR_MODE_VC,
			.lane_id = { 0, 1, 2, 3 }
		}
	}
};

static ISP_PUB_ATTR_S omnivision_ov2718_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR
};

static VI_DEV_ATTR_S omnivision_ov2718_dev_attr = {
	.enIntfMode     = VI_MODE_MIPI,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32CompMask   = { 0xFFF00000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.s32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_INPUT_DATA_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_PULSE,
		.enVsyncNeg     = VI_VSYNC_NEG_LOW,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 0, .u32HsyncAct = 1280, .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,   .u32VsyncVact = 720, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0,  .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
		}
	},
	.enDataPath         = VI_PATH_ISP,
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataRev           = HI_FALSE,
	.stDevRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080
	}
};

static HIMPP_SENSOR_CONFIG omnivision_ov2718_sensor_config = {
	.name               = "ov2718",
	.module_path        = "libsns_ov2718.so",
	.combo_dev_attr     = &omnivision_ov2718_combo_dev_attr,
	.isp_pub_attr       = &omnivision_ov2718_pub_attr,
	.vi_dev_attr        = &omnivision_ov2718_dev_attr
};

/***************************************************************************
 * Sony IMX290
 ***************************************************************************/

static combo_dev_attr_t sony_imx290_combo_dev_attr = {
	.devno = 0,
	.input_mode = INPUT_MODE_MIPI, {
		.mipi_attr = {
			.raw_data_type = RAW_DATA_10BIT,
			.wdr_mode = HI_MIPI_WDR_MODE_DOL,
			.lane_id = { 0, 1, 2, 3 }
		}
	}
};

static ISP_PUB_ATTR_S sony_imx290_pub_attr = {
	.stWndRect          = {
		.s32X           = 200,
		.s32Y           = 20,
		.u32Width       = 1920,
		.u32Height      = 1080,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_RGGB
};

static VI_DEV_ATTR_S sony_imx290_dev_attr = {
	.enIntfMode     = VI_MODE_MIPI,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32CompMask   = { 0xFFF0000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.s32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_INPUT_DATA_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_PULSE,
		.enVsyncNeg     = VI_VSYNC_NEG_HIGH,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 0, .u32HsyncAct = 1920, .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,   .u32VsyncVact = 1080, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0,  .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
		}
	},
	.enDataPath         = VI_PATH_ISP,
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataRev           = HI_FALSE,
	.stDevRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080
	}
};

static HIMPP_SENSOR_CONFIG sony_imx290_sensor_config = {
	.name               = "imx290",
	.module_path        = "libsns_imx290.so",
	.combo_dev_attr     = &sony_imx290_combo_dev_attr,
	.isp_pub_attr       = &sony_imx290_pub_attr,
	.vi_dev_attr        = &sony_imx290_dev_attr
};

/***************************************************************************
 * Sony IMX323
 ***************************************************************************/

static combo_dev_attr_t sony_imx323_combo_dev_attr = {
	.devno = 0,
	.input_mode = INPUT_MODE_CMOS,
	{}
};

static ISP_PUB_ATTR_S sony_imx323_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_RGGB
};

static VI_DEV_ATTR_S sony_imx323_dev_attr = {
	.enIntfMode     = VI_MODE_DIGITAL_CAMERA,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32CompMask   = { 0xFFF0000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.s32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_INPUT_DATA_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_PULSE,
		.enVsyncNeg     = VI_VSYNC_NEG_HIGH,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 0, .u32HsyncAct = 1920, .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,   .u32VsyncVact = 1080, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0,  .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
		}
	},
	.enDataPath         = VI_PATH_ISP,
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataRev           = HI_FALSE,
	.stDevRect          = {
		.s32X           = 200,
		.s32Y           = 20,
		.u32Width       = 1920,
		.u32Height      = 1080
	}
};

static HIMPP_SENSOR_CONFIG sony_imx323_sensor_config = {
	.name               = "imx323",
	.module_path        = "libsns_imx323.so",
	.combo_dev_attr     = &sony_imx323_combo_dev_attr,
	.isp_pub_attr       = &sony_imx323_pub_attr,
	.vi_dev_attr        = &sony_imx323_dev_attr
};

/***************************************************************************
 * SmartSens SC4236
 ***************************************************************************/

static combo_dev_attr_t smartsens_sc4236_combo_dev_attr = {
	.devno = 0,
	.input_mode = INPUT_MODE_CMOS,
	{}
};

static ISP_PUB_ATTR_S smartsens_sc4236_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1536,
		.u32Height      = 1536,
	},
	.f32FrameRate       = 15,
	.enBayer            = BAYER_BGGR
};

static VI_DEV_ATTR_S smartsens_sc4236_dev_attr = {
	.enIntfMode     = VI_MODE_DIGITAL_CAMERA,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32CompMask   = { 0xFFF0000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.s32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_INPUT_DATA_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_PULSE,
		.enVsyncNeg     = VI_VSYNC_NEG_HIGH,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_NORM_PULSE,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 0,  .u32HsyncAct = 1536,  .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,  .u32VsyncVact = 1536, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0, .u32VsyncVbact = 0,   .u32VsyncVbbb = 0
		}
	},
	.enDataPath         = VI_PATH_ISP,
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataRev           = HI_FALSE,
	.stDevRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1536,
		.u32Height      = 1536
	}
};

static HIMPP_SENSOR_CONFIG smartsens_sc4236_sensor_config = {
	.name               = "sc4236",
	.module_path        = "libsns_sc4236.so",
	.combo_dev_attr     = &smartsens_sc4236_combo_dev_attr,
	.isp_pub_attr       = &smartsens_sc4236_pub_attr,
	.vi_dev_attr        = &smartsens_sc4236_dev_attr
};


HimppVideoSensorMap himpp_video_sensor_map;

class HimppVideoSensorInitializer
{
public:
	HimppVideoSensorInitializer()
	{
#define addSensor(sensor) himpp_video_sensor_map.emplace(sensor.name, &sensor)
		addSensor(aptina_ar0237_sensor_config);
		addSensor(omnivision_ov2718_sensor_config);
		addSensor(sony_imx290_sensor_config);
		addSensor(sony_imx323_sensor_config);
		addSensor(smartsens_sc4236_sensor_config);
#undef addSensor
	}
};

static HimppVideoSensorInitializer __himpp_video_sensor_initializer;


HimppVideoSensor::HimppVideoSensor(HIMPP_SENSOR_CONFIG *config)
: sensor_config(config)
{
}

HimppVideoSensor::~HimppVideoSensor()
{
}

std::string HimppVideoSensor::getSensorName()
{
	if (sensor_config)
		return sensor_config->name;
	return std::string();
}

std::string HimppVideoSensor::getModulePath()
{
	if (sensor_config)
		return sensor_config->module_path;
	return std::string();
}

HimppVideoSensor::operator combo_dev_attr_t* ()
{
	if (sensor_config)
		return sensor_config->combo_dev_attr;
	return NULL;
}

HimppVideoSensor::operator ISP_PUB_ATTR_S* ()
{
	if (sensor_config)
		return sensor_config->isp_pub_attr;
	return NULL;
}

HimppVideoSensor::operator VI_DEV_ATTR_S* ()
{
	if (sensor_config)
		return sensor_config->vi_dev_attr;
	return NULL;
}
