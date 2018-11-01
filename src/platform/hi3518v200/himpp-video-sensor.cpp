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
 * Aptina MT9M034
 ***************************************************************************/

static combo_dev_attr_t aptina_mt9m034_combo_dev_attr = {
	.input_mode = INPUT_MODE_CMOS_33V
};

static ISP_PUB_ATTR_S aptina_mt9m034_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1280,
		.u32Height      = 720,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_GRBG
};

static VI_DEV_ATTR_S aptina_mt9m034_dev_attr = {
	.enIntfMode     = VI_MODE_DIGITAL_CAMERA,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32CompMask   = { 0xFFF0000, 0x0 },
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
			.u32HsyncHfb = 370, .u32HsyncAct = 1280, .u32HsyncHbb = 0,
			.u32VsyncVfb = 6,   .u32VsyncVact = 720, .u32VsyncVbb = 6,
			.u32VsyncVbfb = 0,  .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
		}
	},
	.enDataPath         = VI_PATH_ISP,
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataRev           = HI_FALSE,
	.stDevRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1280,
		.u32Height      = 720
	}
};

static HIMPP_SENSOR_CONFIG aptina_mt9m034_sensor_config = {
	.name               = "mt9m034",
	.module_path        = "libsns_9m034.so",
	.combo_dev_attr     = &aptina_mt9m034_combo_dev_attr,
	.isp_pub_attr       = &aptina_mt9m034_pub_attr,
	.vi_dev_attr        = &aptina_mt9m034_dev_attr
};

/***************************************************************************
 * Aptina AR0130
 ***************************************************************************/

static combo_dev_attr_t aptina_ar0130_combo_dev_attr = {
	.input_mode = INPUT_MODE_CMOS_33V
};

static ISP_PUB_ATTR_S aptina_ar0130_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1280,
		.u32Height      = 720,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_GRBG
};

static VI_DEV_ATTR_S aptina_ar0130_dev_attr = {
	.enIntfMode     = VI_MODE_DIGITAL_CAMERA,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32CompMask   = { 0xFFF0000, 0x0 },
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
			.u32HsyncHfb = 370, .u32HsyncAct = 1280, .u32HsyncHbb = 0,
			.u32VsyncVfb = 6,   .u32VsyncVact = 720, .u32VsyncVbb = 6,
			.u32VsyncVbfb = 0,  .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
		}
	},
	.enDataPath         = VI_PATH_ISP,
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataRev           = HI_FALSE,
	.stDevRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1280,
		.u32Height      = 720
	}
};

static HIMPP_SENSOR_CONFIG aptina_ar0130_sensor_config = {
	.name               = "ar0130",
	.module_path        = "libsns_ar0130.so",
	.combo_dev_attr     = &aptina_ar0130_combo_dev_attr,
	.isp_pub_attr       = &aptina_ar0130_pub_attr,
	.vi_dev_attr        = &aptina_ar0130_dev_attr
};

/***************************************************************************
 * Aptina AR0230
 ***************************************************************************/

static combo_dev_attr_t aptina_ar0230_combo_dev_attr = {
	.input_mode = INPUT_MODE_HISPI, {
		.lvds_attr = {
			.img_size = { 1920, 1080 },
			HI_WDR_MODE_NONE,
			LVDS_SYNC_MODE_SOL,
			RAW_DATA_12BIT,
			LVDS_ENDIAN_LITTLE,
			LVDS_ENDIAN_LITTLE,
			.lane_id = { 0, 1, 2, 3, -1, -1, -1, -1 },
			.sync_code = {
				{
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 }
				},
				{
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 }
				},
				{
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 }
				},
				{
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 }
				},
				{
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 }
				},
				{
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005}
				},
				{
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 }
				},
				{
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 },
					{ 0x003, 0x007, 0x001, 0x005 }
				}
			}
		}
	}
};

static ISP_PUB_ATTR_S aptina_ar0230_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_GRBG
};

static VI_DEV_ATTR_S aptina_ar0230_dev_attr = {
	.enIntfMode     = VI_MODE_HISPI,
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

static HIMPP_SENSOR_CONFIG aptina_ar0230_sensor_config = {
	.name               = "ar0230",
	.module_path        = "libsns_ar0230.so",
	.combo_dev_attr     = &aptina_ar0230_combo_dev_attr,
	.isp_pub_attr       = &aptina_ar0230_pub_attr,
	.vi_dev_attr        = &aptina_ar0230_dev_attr
};

/***************************************************************************
 * OmniVision OV2718
 ***************************************************************************/

static combo_dev_attr_t omnivision_ov2718_combo_dev_attr = {
	.input_mode = INPUT_MODE_MIPI, {
		.mipi_attr = {
			.raw_data_type = RAW_DATA_12BIT,
			.lane_id = { 0, 1, 2, 3, -1, -1, -1, -1 }
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
	.f32FrameRate       = 25,
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
 * OmniVision OV9712
 ***************************************************************************/

static combo_dev_attr_t omnivision_ov9712_combo_dev_attr = {
	.input_mode = INPUT_MODE_CMOS_33V,
};

static ISP_PUB_ATTR_S omnivision_ov9712_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1280,
		.u32Height      = 720,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR
};

static VI_DEV_ATTR_S omnivision_ov9712_dev_attr = {
	.enIntfMode     = VI_MODE_DIGITAL_CAMERA,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32CompMask   = { 0xFFC0000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.s32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_INPUT_DATA_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_FIELD,
		.enVsyncNeg     = VI_VSYNC_NEG_HIGH,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 370, .u32HsyncAct = 1280, .u32HsyncHbb = 0,
			.u32VsyncVfb = 6,   .u32VsyncVact = 720, .u32VsyncVbb = 6,
			.u32VsyncVbfb = 0,  .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
		}
	},
	.enDataPath         = VI_PATH_ISP,
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataRev           = HI_FALSE,
	.stDevRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1280,
		.u32Height      = 720
	}
};

static HIMPP_SENSOR_CONFIG omnivision_ov9712_sensor_config = {
	.name               = "ov9712",
	.module_path        = "libsns_ov9712.so",
	.combo_dev_attr     = &omnivision_ov9712_combo_dev_attr,
	.isp_pub_attr       = &omnivision_ov9712_pub_attr,
	.vi_dev_attr        = &omnivision_ov9712_dev_attr
};

/***************************************************************************
 * OmniVision OV9732
 ***************************************************************************/

static combo_dev_attr_t omnivision_ov9732_combo_dev_attr = {
	.input_mode = INPUT_MODE_CMOS_33V,
};

static ISP_PUB_ATTR_S omnivision_ov9732_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1280,
		.u32Height      = 720,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR
};

static VI_DEV_ATTR_S omnivision_ov9732_dev_attr = {
	.enIntfMode     = VI_MODE_DIGITAL_CAMERA,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32CompMask   = { 0xFFC0000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.s32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_INPUT_DATA_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_FIELD,
		.enVsyncNeg     = VI_VSYNC_NEG_HIGH,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 370, .u32HsyncAct = 1280, .u32HsyncHbb = 0,
			.u32VsyncVfb = 6,   .u32VsyncVact = 720, .u32VsyncVbb = 6,
			.u32VsyncVbfb = 0,  .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
		}
	},
	.enDataPath         = VI_PATH_ISP,
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataRev           = HI_FALSE,
	.stDevRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1280,
		.u32Height      = 720
	}
};

static HIMPP_SENSOR_CONFIG omnivision_ov9732_sensor_config = {
	.name               = "ov9732",
	.module_path        = "libsns_ov9732.so",
	.combo_dev_attr     = &omnivision_ov9732_combo_dev_attr,
	.isp_pub_attr       = &omnivision_ov9732_pub_attr,
	.vi_dev_attr        = &omnivision_ov9732_dev_attr
};

/***************************************************************************
 * OmniVision OV9750
 ***************************************************************************/

static combo_dev_attr_t omnivision_ov9750_combo_dev_attr = {
	.input_mode = INPUT_MODE_MIPI, {
		.mipi_attr = {
			.raw_data_type = RAW_DATA_12BIT,
			.lane_id = { 0, 1, -1, -1, -1, -1, -1, -1 }
		}
	}
};

static ISP_PUB_ATTR_S omnivision_ov9750_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1280,
		.u32Height      = 720,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR
};

static VI_DEV_ATTR_S omnivision_ov9750_dev_attr = {
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
		.u32Width       = 1280,
		.u32Height      = 720
	}
};

static HIMPP_SENSOR_CONFIG omnivision_ov9750_sensor_config = {
	.name               = "ov9750",
	.module_path        = "libsns_ov9750.so",
	.combo_dev_attr     = &omnivision_ov9750_combo_dev_attr,
	.isp_pub_attr       = &omnivision_ov9750_pub_attr,
	.vi_dev_attr        = &omnivision_ov9750_dev_attr
};

/***************************************************************************
 * OmniVision OV9752
 ***************************************************************************/

static combo_dev_attr_t omnivision_ov9752_combo_dev_attr = {
	.input_mode = INPUT_MODE_MIPI, {
		.mipi_attr = {
			.raw_data_type = RAW_DATA_12BIT,
			.lane_id = { 0, 1, -1, -1, -1, -1, -1, -1 }
		}
	}
};

static ISP_PUB_ATTR_S omnivision_ov9752_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1280,
		.u32Height      = 720,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR
};

static VI_DEV_ATTR_S omnivision_ov9752_dev_attr = {
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
		.u32Width       = 1280,
		.u32Height      = 720
	}
};

static HIMPP_SENSOR_CONFIG omnivision_ov9752_sensor_config = {
	.name               = "ov9752",
	.module_path        = "libsns_ov9752.so",
	.combo_dev_attr     = &omnivision_ov9752_combo_dev_attr,
	.isp_pub_attr       = &omnivision_ov9752_pub_attr,
	.vi_dev_attr        = &omnivision_ov9752_dev_attr
};

/***************************************************************************
 * Panasonic MN34222
 ***************************************************************************/

static combo_dev_attr_t panasonic_mn34222_combo_dev_attr = {
	.input_mode = INPUT_MODE_MIPI, {
		.mipi_attr = {
			.raw_data_type = RAW_DATA_12BIT,
			.lane_id = { 0, 1, -1, -1, -1, -1, -1, -1 }
		}
	}
};

static ISP_PUB_ATTR_S panasonic_mn34222_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_GRBG
};

static VI_DEV_ATTR_S panasonic_mn34222_dev_attr = {
	.enIntfMode     = VI_MODE_LVDS,
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

static HIMPP_SENSOR_CONFIG panasonic_mn34222_sensor_config = {
	.name               = "mn34222",
	.module_path        = "libsns_mn34222.so",
	.combo_dev_attr     = &panasonic_mn34222_combo_dev_attr,
	.isp_pub_attr       = &panasonic_mn34222_pub_attr,
	.vi_dev_attr        = &panasonic_mn34222_dev_attr
};

/***************************************************************************
 * SmartSens SC1135
 ***************************************************************************/

static combo_dev_attr_t smartsens_sc1135_combo_dev_attr = {
	.input_mode = INPUT_MODE_CMOS_18V
};

static ISP_PUB_ATTR_S smartsens_sc1135_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1280,
		.u32Height      = 960,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR
};

static VI_DEV_ATTR_S smartsens_sc1135_dev_attr = {
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
			.u32HsyncHfb = 0, .u32HsyncAct = 1280, .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,   .u32VsyncVact = 960, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0,  .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
		}
	},
	.enDataPath         = VI_PATH_ISP,
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataRev           = HI_FALSE,
	.stDevRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1280,
		.u32Height      = 960
	}
};

static HIMPP_SENSOR_CONFIG smartsens_sc1135_sensor_config = {
	.name               = "sc1135",
	.module_path        = "libsns_sc1135.so",
	.combo_dev_attr     = &smartsens_sc1135_combo_dev_attr,
	.isp_pub_attr       = &smartsens_sc1135_pub_attr,
	.vi_dev_attr        = &smartsens_sc1135_dev_attr
};

/***************************************************************************
 * SmartSens SC1145
 ***************************************************************************/

static combo_dev_attr_t smartsens_sc1145_combo_dev_attr = {
	.input_mode = INPUT_MODE_CMOS_18V
};

static ISP_PUB_ATTR_S smartsens_sc1145_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1280,
		.u32Height      = 720,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR
};

static VI_DEV_ATTR_S smartsens_sc1145_dev_attr = {
	.enIntfMode     = VI_MODE_DIGITAL_CAMERA,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32CompMask   = { 0xFFC0000, 0x0 },
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
		.u32Width       = 1280,
		.u32Height      = 720
	}
};

static HIMPP_SENSOR_CONFIG smartsens_sc1145_sensor_config = {
	.name               = "sc1145",
	.module_path        = "libsns_sc1145.so",
	.combo_dev_attr     = &smartsens_sc1145_combo_dev_attr,
	.isp_pub_attr       = &smartsens_sc1145_pub_attr,
	.vi_dev_attr        = &smartsens_sc1145_dev_attr
};

/***************************************************************************
 * SmartSens SC1235
 ***************************************************************************/

static combo_dev_attr_t smartsens_sc1235_combo_dev_attr = {
	.input_mode = INPUT_MODE_CMOS_18V
};

static ISP_PUB_ATTR_S smartsens_sc1235_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1280,
		.u32Height      = 960,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR
};

static VI_DEV_ATTR_S smartsens_sc1235_dev_attr = {
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
			.u32HsyncHfb = 0, .u32HsyncAct = 1280, .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,   .u32VsyncVact = 960, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0,  .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
		}
	},
	.enDataPath         = VI_PATH_ISP,
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataRev           = HI_FALSE,
	.stDevRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1280,
		.u32Height      = 960
	}
};

static HIMPP_SENSOR_CONFIG smartsens_sc1235_sensor_config = {
	.name               = "sc1235",
	.module_path        = "libsns_sc1235.so",
	.combo_dev_attr     = &smartsens_sc1235_combo_dev_attr,
	.isp_pub_attr       = &smartsens_sc1235_pub_attr,
	.vi_dev_attr        = &smartsens_sc1235_dev_attr
};

/***************************************************************************
 * SmartSens SC2135
 ***************************************************************************/

static combo_dev_attr_t smartsens_sc2135_combo_dev_attr = {
	.input_mode = INPUT_MODE_CMOS_18V
};

static ISP_PUB_ATTR_S smartsens_sc2135_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR
};

static VI_DEV_ATTR_S smartsens_sc2135_dev_attr = {
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

static HIMPP_SENSOR_CONFIG smartsens_sc2135_sensor_config = {
	.name               = "sc2135",
	.module_path        = "libsns_sc2135.so",
	.combo_dev_attr     = &smartsens_sc2135_combo_dev_attr,
	.isp_pub_attr       = &smartsens_sc2135_pub_attr,
	.vi_dev_attr        = &smartsens_sc2135_dev_attr
};

/***************************************************************************
 * SmartSens SC2235
 ***************************************************************************/

static combo_dev_attr_t smartsens_sc2235_combo_dev_attr = {
	.input_mode = INPUT_MODE_CMOS_18V
};

static ISP_PUB_ATTR_S smartsens_sc2235_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080,
	},
	.f32FrameRate       = 25,
	.enBayer            = BAYER_BGGR
};

static VI_DEV_ATTR_S smartsens_sc2235_dev_attr = {
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

static HIMPP_SENSOR_CONFIG smartsens_sc2235_sensor_config = {
	.name               = "sc2235",
	.module_path        = "libsns_sc2235.so",
	.combo_dev_attr     = &smartsens_sc2235_combo_dev_attr,
	.isp_pub_attr       = &smartsens_sc2235_pub_attr,
	.vi_dev_attr        = &smartsens_sc2235_dev_attr
};

/***************************************************************************
 * Sony IMX222
 ***************************************************************************/

static combo_dev_attr_t sony_imx222_combo_dev_attr = {
	.input_mode = INPUT_MODE_CMOS_33V
};

static ISP_PUB_ATTR_S sony_imx222_pub_attr = {
	.stWndRect          = {
		.s32X           = 200,
		.s32Y           = 20,
		.u32Width       = 1920,
		.u32Height      = 1080,
	},
	.f32FrameRate       = 30,
	.enBayer            = BAYER_RGGB
};

static VI_DEV_ATTR_S sony_imx222_dev_attr = {
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

static HIMPP_SENSOR_CONFIG sony_imx222_sensor_config = {
	.name               = "imx222",
	.module_path        = "libsns_imx222.so",
	.combo_dev_attr     = &sony_imx222_combo_dev_attr,
	.isp_pub_attr       = &sony_imx222_pub_attr,
	.vi_dev_attr        = &sony_imx222_dev_attr
};


HimppVideoSensorMap himpp_video_sensor_map;

class HimppVideoSensorInitializer
{
public:
	HimppVideoSensorInitializer()
	{
#define addSensor(sensor) himpp_video_sensor_map.emplace(sensor.name, &sensor)
		addSensor(aptina_mt9m034_sensor_config);
		addSensor(aptina_ar0130_sensor_config);
		addSensor(aptina_ar0230_sensor_config);
		addSensor(omnivision_ov2718_sensor_config);
		addSensor(omnivision_ov9712_sensor_config);
		addSensor(omnivision_ov9732_sensor_config);
		addSensor(omnivision_ov9750_sensor_config);
		addSensor(omnivision_ov9752_sensor_config);
		addSensor(panasonic_mn34222_sensor_config);
		addSensor(smartsens_sc1135_sensor_config);
		addSensor(smartsens_sc1145_sensor_config);
		addSensor(smartsens_sc1235_sensor_config);
		addSensor(smartsens_sc2135_sensor_config);
		addSensor(smartsens_sc2235_sensor_config);
		addSensor(sony_imx222_sensor_config);
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
