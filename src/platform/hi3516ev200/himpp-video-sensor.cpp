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
	 const char*		object_name;
	 combo_dev_attr_t*	combo_dev_attr;
	 VI_DEV_ATTR_S*		vi_dev_attr;
	 VI_PIPE_ATTR_S*	vi_pipe_attr;
	 ISP_PUB_ATTR_S*	isp_pub_attr;
};

/***************************************************************************
 * GALAXYCORE GC2053
 ***************************************************************************/
static combo_dev_attr_t galaxycore_gc2053_combo_dev_attr = {
	.devno = 0,
	.input_mode = INPUT_MODE_MIPI,
	.data_rate = MIPI_DATA_RATE_X1,
	.img_rect = { 0, 0, 1920, 1080 },
	{
		.mipi_attr = {
			.input_data_type = DATA_TYPE_RAW_10BIT,
			.wdr_mode = HI_MIPI_WDR_MODE_NONE,
			.lane_id = { 0, 1, -1, -1 }
		}
	}
};

static VI_DEV_ATTR_S galaxycore_gc2053_dev_attr = {
	.enIntfMode     = VI_MODE_MIPI,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32ComponentMask   = { 0xFFC00000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.as32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_DATA_SEQ_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_PULSE,
		.enVsyncNeg     = VI_VSYNC_NEG_LOW,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 0,  .u32HsyncAct = 1280,  .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,  .u32VsyncVact = 720, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0, .u32VsyncVbact = 0,   .u32VsyncVbbb = 0
		}
	},
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataReverse       = HI_FALSE,
	.stSize             = { 1920, 1080 },
	.stBasAttr         = {
		.stSacleAttr    = {
			.stBasSize = { 1920, 1080 },
		},
		.stRephaseAttr  = {
			.enHRephaseMode = VI_REPHASE_MODE_NONE,
			.enVRephaseMode = VI_REPHASE_MODE_NONE,
		}
	},
	.stWDRAttr          = {
		.enWDRMode      = WDR_MODE_NONE,
		.u32CacheLine   = 1080
	},
	.enDataRate		= DATA_RATE_X1
};

static VI_PIPE_ATTR_S galaxycore_gc2053_pipe_attr = {
	.enPipeBypassMode   = VI_PIPE_BYPASS_NONE,
	.bYuvSkip           = HI_FALSE,
	.bIspBypass         = HI_FALSE,
	.u32MaxW            = 1920,
	.u32MaxH            = 1080,
	.enPixFmt           = PIXEL_FORMAT_RGB_BAYER_10BPP,
	.enCompressMode     = COMPRESS_MODE_NONE,
	.enBitWidth         = DATA_BITWIDTH_10,
	.bNrEn              = HI_TRUE,
	.stNrAttr           = {
		.enPixFmt       = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
		.enBitWidth     = DATA_BITWIDTH_8,
		.enNrRefSource  = VI_NR_REF_FROM_RFR,
		.enCompressMode = COMPRESS_MODE_NONE,
	},
	.bSharpenEn         = HI_FALSE,
	.stFrameRate        = { -1, -1 }
};

static ISP_PUB_ATTR_S galaxycore_gc2053_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080,
	},
	.stSnsSize          = { 2304, 1296 },
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR,
	.enWDRMode          = WDR_MODE_NONE,
	.u8SnsMode          = 0,
};

static HIMPP_SENSOR_CONFIG galaxycore_gc2053_sensor_config = {
	.name               = "gc2053",
	.module_path        = "libsns_gc2053.so",
	.object_name		= "stSnsGc2053Obj",
	.combo_dev_attr     = &galaxycore_gc2053_combo_dev_attr,
	.vi_dev_attr        = &galaxycore_gc2053_dev_attr,
	.vi_pipe_attr       = &galaxycore_gc2053_pipe_attr,
	.isp_pub_attr       = &galaxycore_gc2053_pub_attr,
};

/***************************************************************************
 * OMNIVISION OS05A
 ***************************************************************************/
static combo_dev_attr_t omnivision_os05a_combo_dev_attr = {
	.devno = 0,
	.input_mode = INPUT_MODE_MIPI,
	.data_rate = MIPI_DATA_RATE_X1,
	.img_rect = { 0, 0, 2688, 1944 },
	{
		.mipi_attr = {
			.input_data_type = DATA_TYPE_RAW_12BIT,
			.wdr_mode = HI_MIPI_WDR_MODE_NONE,
			.lane_id = { 0, 1, 2, 3 }
		}
	}
};

static VI_DEV_ATTR_S omnivision_os05a_dev_attr = {
	.enIntfMode     = VI_MODE_MIPI,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32ComponentMask   = { 0xFFC00000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.as32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_DATA_SEQ_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_PULSE,
		.enVsyncNeg     = VI_VSYNC_NEG_LOW,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 0,  .u32HsyncAct = 1280,  .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,  .u32VsyncVact = 720, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0, .u32VsyncVbact = 0,   .u32VsyncVbbb = 0
		}
	},
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataReverse       = HI_FALSE,
	.stSize             = { 2688, 1944 },
	.stBasAttr         = {
		.stSacleAttr    = {
			.stBasSize = { 2688, 1944 },
		},
		.stRephaseAttr  = {
			.enHRephaseMode = VI_REPHASE_MODE_NONE,
			.enVRephaseMode = VI_REPHASE_MODE_NONE,
		}
	},
	.stWDRAttr          = {
		.enWDRMode      = WDR_MODE_NONE,
		.u32CacheLine   = 1944
	},
	.enDataRate		= DATA_RATE_X1
};

static VI_PIPE_ATTR_S omnivision_os05a_pipe_attr = {
	.enPipeBypassMode   = VI_PIPE_BYPASS_NONE,
	.bYuvSkip           = HI_FALSE,
	.bIspBypass         = HI_FALSE,
	.u32MaxW            = 2688,
	.u32MaxH            = 1944,
	.enPixFmt           = PIXEL_FORMAT_RGB_BAYER_12BPP,
	.enCompressMode     = COMPRESS_MODE_LINE,
	.enBitWidth         = DATA_BITWIDTH_12,
	.bNrEn              = HI_FALSE,
	.stNrAttr           = {
		.enPixFmt       = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
		.enBitWidth     = DATA_BITWIDTH_8,
		.enNrRefSource  = VI_NR_REF_FROM_RFR,
		.enCompressMode = COMPRESS_MODE_NONE,
	},
	.bSharpenEn         = HI_FALSE,
	.stFrameRate        = { -1, -1 }
};

static ISP_PUB_ATTR_S omnivision_os05a_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 2688,
		.u32Height      = 1944,
	},
	.stSnsSize          = { 2688, 1944 },
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR,
	.enWDRMode          = WDR_MODE_NONE,
	.u8SnsMode          = 0,
};

static HIMPP_SENSOR_CONFIG omnivision_os05a_sensor_config = {
	.name               = "os05a",
	.module_path        = "libsns_os05a.so",
	.object_name		= "stSnsOs05aObj",
	.combo_dev_attr     = &omnivision_os05a_combo_dev_attr,
	.vi_dev_attr        = &omnivision_os05a_dev_attr,
	.vi_pipe_attr       = &omnivision_os05a_pipe_attr,
	.isp_pub_attr       = &omnivision_os05a_pub_attr,
};

/***************************************************************************
 * SMARTSENS SC4236
 ***************************************************************************/

static combo_dev_attr_t smartsens_sc4236_combo_dev_attr = {
	.devno = 0,
	.input_mode = INPUT_MODE_MIPI,
	.data_rate = MIPI_DATA_RATE_X1,
	.img_rect = { 0, 0, 2304, 1296 },
	{
		.mipi_attr = {
			.input_data_type = DATA_TYPE_RAW_10BIT,
			.wdr_mode = HI_MIPI_WDR_MODE_NONE,
			.lane_id = { 0, 1, -1, -1 }
		}
	}
};

static VI_DEV_ATTR_S smartsens_sc4236_dev_attr = {
	.enIntfMode     = VI_MODE_MIPI,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32ComponentMask   = { 0xFFC00000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.as32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_DATA_SEQ_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_PULSE,
		.enVsyncNeg     = VI_VSYNC_NEG_LOW,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 0,  .u32HsyncAct = 1280,  .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,  .u32VsyncVact = 720, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0, .u32VsyncVbact = 0,   .u32VsyncVbbb = 0
		}
	},
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataReverse       = HI_FALSE,
	.stSize             = { 2304, 1296 },
	.stBasAttr         = {
		.stSacleAttr    = {
			.stBasSize = { 2304, 1296 },
		},
		.stRephaseAttr  = {
			.enHRephaseMode = VI_REPHASE_MODE_NONE,
			.enVRephaseMode = VI_REPHASE_MODE_NONE,
		}
	},
	.stWDRAttr          = {
		.enWDRMode      = WDR_MODE_NONE,
		.u32CacheLine   = 1296
	},
	.enDataRate		= DATA_RATE_X1
};

static VI_PIPE_ATTR_S smartsens_sc4236_pipe_attr = {
	.enPipeBypassMode   = VI_PIPE_BYPASS_NONE,
	.bYuvSkip           = HI_FALSE,
	.bIspBypass         = HI_FALSE,
	.u32MaxW            = 2304,
	.u32MaxH            = 1296,
	.enPixFmt           = PIXEL_FORMAT_RGB_BAYER_10BPP,
	.enCompressMode     = COMPRESS_MODE_NONE,
	.enBitWidth         = DATA_BITWIDTH_10,
	.bNrEn              = HI_FALSE,
	.stNrAttr           = {
		.enPixFmt       = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
		.enBitWidth     = DATA_BITWIDTH_8,
		.enNrRefSource  = VI_NR_REF_FROM_RFR,
		.enCompressMode = COMPRESS_MODE_NONE,
	},
	.bSharpenEn         = HI_FALSE,
	.stFrameRate        = { -1, -1 }
};

static ISP_PUB_ATTR_S smartsens_sc4236_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 2304,
		.u32Height      = 1296,
	},
	.stSnsSize          = { 2304, 1296 },
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR,
	.enWDRMode          = WDR_MODE_NONE,
	.u8SnsMode          = 0,
};

static HIMPP_SENSOR_CONFIG smartsens_sc4236_sensor_config = {
	.name               = "sc4236",
	.module_path        = "libsns_sc4236.so",
	.object_name		= "stSnsSc4236Obj",
	.combo_dev_attr     = &smartsens_sc4236_combo_dev_attr,
	.vi_dev_attr        = &smartsens_sc4236_dev_attr,
	.vi_pipe_attr       = &smartsens_sc4236_pipe_attr,
	.isp_pub_attr       = &smartsens_sc4236_pub_attr,
};

/***************************************************************************
 * SMARTSENS SC2231
 ***************************************************************************/
static combo_dev_attr_t smartsens_sc2231_combo_dev_attr = {
	.devno = 0,
	.input_mode = INPUT_MODE_MIPI,
	.data_rate = MIPI_DATA_RATE_X1,
	.img_rect = { 0, 0, 1920, 1080 },
	{
		.mipi_attr = {
			.input_data_type = DATA_TYPE_RAW_10BIT,
			.wdr_mode = HI_MIPI_WDR_MODE_NONE,
			.lane_id = { 0, 1, -1, -1 }
		}
	}
};

static VI_DEV_ATTR_S smartsens_sc2231_dev_attr = {
	.enIntfMode     = VI_MODE_MIPI,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32ComponentMask   = { 0xFFC00000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.as32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_DATA_SEQ_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_PULSE,
		.enVsyncNeg     = VI_VSYNC_NEG_LOW,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 0,  .u32HsyncAct = 1920,  .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,  .u32VsyncVact = 1080, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0, .u32VsyncVbact = 0,   .u32VsyncVbbb = 0
		}
	},
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataReverse       = HI_FALSE,
	.stSize             = { 1920, 1080 },
	.stBasAttr         = {
		.stSacleAttr    = {
			.stBasSize = { 1920, 1080 },
		},
		.stRephaseAttr  = {
			.enHRephaseMode = VI_REPHASE_MODE_NONE,
			.enVRephaseMode = VI_REPHASE_MODE_NONE,
		}
	},
	.stWDRAttr          = {
		.enWDRMode      = WDR_MODE_NONE,
		.u32CacheLine   = 1080
	},
	.enDataRate		= DATA_RATE_X1
};

static VI_PIPE_ATTR_S smartsens_sc2231_pipe_attr = {
	.enPipeBypassMode   = VI_PIPE_BYPASS_NONE,
	.bYuvSkip           = HI_FALSE,
	.bIspBypass         = HI_FALSE,
	.u32MaxW            = 1920,
	.u32MaxH            = 1080,
	.enPixFmt           = PIXEL_FORMAT_RGB_BAYER_12BPP,
	.enCompressMode     = COMPRESS_MODE_NONE,
	.enBitWidth         = DATA_BITWIDTH_12,
	.bNrEn              = HI_FALSE,
	.stNrAttr           = {
		.enPixFmt       = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
		.enBitWidth     = DATA_BITWIDTH_8,
		.enNrRefSource  = VI_NR_REF_FROM_RFR,
		.enCompressMode = COMPRESS_MODE_NONE,
	},
	.bSharpenEn         = HI_FALSE,
	.stFrameRate        = { -1, -1 }
};

static ISP_PUB_ATTR_S smartsens_sc2231_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080,
	},
	.stSnsSize          = { 1920, 1080 },
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR,
	.enWDRMode          = WDR_MODE_NONE,
	.u8SnsMode          = 0,
};

static HIMPP_SENSOR_CONFIG smartsens_sc2231_sensor_config = {
	.name               = "sc2231",
	.module_path        = "libsns_sc2231.so",
	.object_name		= "stSnsSc2231Obj",
	.combo_dev_attr     = &smartsens_sc2231_combo_dev_attr,
	.vi_dev_attr        = &smartsens_sc2231_dev_attr,
	.vi_pipe_attr       = &smartsens_sc2231_pipe_attr,
	.isp_pub_attr       = &smartsens_sc2231_pub_attr,
};

/***************************************************************************
 * SMARTSENS SC2235
 ***************************************************************************/
static combo_dev_attr_t smartsens_sc2235_combo_dev_attr = {
	.devno = 0,
	.input_mode = INPUT_MODE_CMOS,
	.data_rate = MIPI_DATA_RATE_X1,
	.img_rect = { 0, 0, 1920, 1080 },
	{
		.mipi_attr = {
			.input_data_type = DATA_TYPE_RAW_10BIT,
			.wdr_mode = HI_MIPI_WDR_MODE_NONE,
			.lane_id = { 0, 1, 2, 3 }
		}
	}
};

static VI_DEV_ATTR_S smartsens_sc2235_dev_attr = {
	.enIntfMode     = VI_MODE_MIPI,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32ComponentMask   = { 0xFFF00000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.as32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_DATA_SEQ_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_FIELD,
		.enVsyncNeg     = VI_VSYNC_NEG_HIGH,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_NORM_PULSE,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 0,  .u32HsyncAct = 1920,  .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,  .u32VsyncVact = 1080, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0, .u32VsyncVbact = 0,   .u32VsyncVbbb = 0
		}
	},
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataReverse       = HI_FALSE,
	.stSize             = { 1920, 1080 },
	.stBasAttr         = {
		.stSacleAttr    = {
			.stBasSize = { 1920, 1080 },
		},
		.stRephaseAttr  = {
			.enHRephaseMode = VI_REPHASE_MODE_NONE,
			.enVRephaseMode = VI_REPHASE_MODE_NONE,
		}
	},
	.stWDRAttr          = {
		.enWDRMode      = WDR_MODE_NONE,
		.u32CacheLine   = 1080
	},
	.enDataRate		= DATA_RATE_X1
};

static VI_PIPE_ATTR_S smartsens_sc2235_pipe_attr = {
	.enPipeBypassMode   = VI_PIPE_BYPASS_NONE,
	.bYuvSkip           = HI_FALSE,
	.bIspBypass         = HI_FALSE,
	.u32MaxW            = 1920,
	.u32MaxH            = 1080,
	.enPixFmt           = PIXEL_FORMAT_RGB_BAYER_10BPP,
	.enCompressMode     = COMPRESS_MODE_NONE,
	.enBitWidth         = DATA_BITWIDTH_10,
	.bNrEn              = HI_FALSE,
	.stNrAttr           = {
		.enPixFmt       = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
		.enBitWidth     = DATA_BITWIDTH_8,
		.enNrRefSource  = VI_NR_REF_FROM_RFR,
		.enCompressMode = COMPRESS_MODE_NONE,
	},
	.bSharpenEn         = HI_FALSE,
	.stFrameRate        = { -1, -1 }
};

static ISP_PUB_ATTR_S smartsens_sc2235_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080,
	},
	.stSnsSize          = { 1920, 1080 },
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR,
	.enWDRMode          = WDR_MODE_NONE,
	.u8SnsMode          = 0,
};

static HIMPP_SENSOR_CONFIG smartsens_sc2235_sensor_config = {
	.name               = "sc2235",
	.module_path        = "libsns_sc2235.so",
	.object_name		= "stSnsSc2235Obj",
	.combo_dev_attr     = &smartsens_sc2235_combo_dev_attr,
	.vi_dev_attr        = &smartsens_sc2235_dev_attr,
	.vi_pipe_attr       = &smartsens_sc2235_pipe_attr,
	.isp_pub_attr       = &smartsens_sc2235_pub_attr,
};

/***************************************************************************
 * SMARTSENS SC3235
 ***************************************************************************/
static combo_dev_attr_t smartsens_sc3235_combo_dev_attr = {
	.devno = 0,
	.input_mode = INPUT_MODE_MIPI,
	.data_rate = MIPI_DATA_RATE_X1,
	.img_rect = { 0, 0, 2304, 1296 },
	{
		.mipi_attr = {
			.input_data_type = DATA_TYPE_RAW_10BIT,
			.wdr_mode = HI_MIPI_WDR_MODE_NONE,
			.lane_id = { 0, 1, -1, -1 }
		}
	}
};

static VI_DEV_ATTR_S smartsens_sc3235_dev_attr = {
	.enIntfMode     = VI_MODE_MIPI,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32ComponentMask   = { 0xFFC00000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.as32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_DATA_SEQ_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_PULSE,
		.enVsyncNeg     = VI_VSYNC_NEG_LOW,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 0,  .u32HsyncAct = 1280,  .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,  .u32VsyncVact = 720, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0, .u32VsyncVbact = 0,   .u32VsyncVbbb = 0
		}
	},
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataReverse       = HI_FALSE,
	.stSize             = { 2304, 1296 },
	.stBasAttr         = {
		.stSacleAttr    = {
			.stBasSize = { 2304, 1296 },
		},
		.stRephaseAttr  = {
			.enHRephaseMode = VI_REPHASE_MODE_NONE,
			.enVRephaseMode = VI_REPHASE_MODE_NONE,
		}
	},
	.stWDRAttr          = {
		.enWDRMode      = WDR_MODE_NONE,
		.u32CacheLine   = 1296
	},
	.enDataRate		= DATA_RATE_X1
};

static VI_PIPE_ATTR_S smartsens_sc3235_pipe_attr = {
	.enPipeBypassMode   = VI_PIPE_BYPASS_NONE,
	.bYuvSkip           = HI_FALSE,
	.bIspBypass         = HI_FALSE,
	.u32MaxW            = 2304,
	.u32MaxH            = 1296,
	.enPixFmt           = PIXEL_FORMAT_RGB_BAYER_10BPP,
	.enCompressMode     = COMPRESS_MODE_NONE,
	.enBitWidth         = DATA_BITWIDTH_10,
	.bNrEn              = HI_FALSE,
	.stNrAttr           = {
		.enPixFmt       = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
		.enBitWidth     = DATA_BITWIDTH_8,
		.enNrRefSource  = VI_NR_REF_FROM_RFR,
		.enCompressMode = COMPRESS_MODE_NONE,
	},
	.bSharpenEn         = HI_FALSE,
	.stFrameRate        = { -1, -1 }
};

static ISP_PUB_ATTR_S smartsens_sc3235_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 2304,
		.u32Height      = 1296,
	},
	.stSnsSize          = { 2304, 1296 },
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR,
	.enWDRMode          = WDR_MODE_NONE,
	.u8SnsMode          = 0,
};

static HIMPP_SENSOR_CONFIG smartsens_sc3235_sensor_config = {
	.name               = "sc3235",
	.module_path        = "libsns_sc3235.so",
	.object_name		= "stSnsSc3235Obj",
	.combo_dev_attr     = &smartsens_sc3235_combo_dev_attr,
	.vi_dev_attr        = &smartsens_sc3235_dev_attr,
	.vi_pipe_attr       = &smartsens_sc3235_pipe_attr,
	.isp_pub_attr       = &smartsens_sc3235_pub_attr,
};

/***************************************************************************
 * SOI JXF37
 ***************************************************************************/
static combo_dev_attr_t soi_f37_combo_dev_attr = {
	.devno = 0,
	.input_mode = INPUT_MODE_MIPI,
	.data_rate = MIPI_DATA_RATE_X1,
	.img_rect = { 0, 0, 1920, 1080 },
	{
		.mipi_attr = {
			.input_data_type = DATA_TYPE_RAW_10BIT,
			.wdr_mode = HI_MIPI_WDR_MODE_NONE,
			.lane_id = { 0, 1, -1, -1 }
		}
	}
};

static VI_DEV_ATTR_S soi_f37_dev_attr = {
	.enIntfMode     = VI_MODE_MIPI,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32ComponentMask   = { 0xFFC00000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.as32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_DATA_SEQ_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_PULSE,
		.enVsyncNeg     = VI_VSYNC_NEG_LOW,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 0,  .u32HsyncAct = 1920,  .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,  .u32VsyncVact = 1080, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0, .u32VsyncVbact = 0,   .u32VsyncVbbb = 0
		}
	},
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataReverse       = HI_FALSE,
	.stSize             = { 1920, 1080 },
	.stBasAttr         = {
		.stSacleAttr    = {
			.stBasSize = { 1920, 1080 },
		},
		.stRephaseAttr  = {
			.enHRephaseMode = VI_REPHASE_MODE_NONE,
			.enVRephaseMode = VI_REPHASE_MODE_NONE,
		}
	},
	.stWDRAttr          = {
		.enWDRMode      = WDR_MODE_NONE,
		.u32CacheLine   = 1080
	},
	.enDataRate		= DATA_RATE_X1
};

static VI_PIPE_ATTR_S soi_f37_pipe_attr = {
	.enPipeBypassMode   = VI_PIPE_BYPASS_NONE,
	.bYuvSkip           = HI_FALSE,
	.bIspBypass         = HI_FALSE,
	.u32MaxW            = 1920,
	.u32MaxH            = 1080,
	.enPixFmt           = PIXEL_FORMAT_RGB_BAYER_10BPP,
	.enCompressMode     = COMPRESS_MODE_NONE,
	.enBitWidth         = DATA_BITWIDTH_10,
	.bNrEn              = HI_FALSE,
	.stNrAttr           = {
		.enPixFmt       = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
		.enBitWidth     = DATA_BITWIDTH_8,
		.enNrRefSource  = VI_NR_REF_FROM_RFR,
		.enCompressMode = COMPRESS_MODE_NONE,
	},
	.bSharpenEn         = HI_FALSE,
	.stFrameRate        = { -1, -1 }
};

static ISP_PUB_ATTR_S soi_f37_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080,
	},
	.stSnsSize          = { 1920, 1080 },
	.f32FrameRate       = 30,
	.enBayer            = BAYER_BGGR,
	.enWDRMode          = WDR_MODE_NONE,
	.u8SnsMode          = 0,
};

static HIMPP_SENSOR_CONFIG soi_f37_sensor_config = {
	.name               = "f37",
	.module_path        = "libsns_f37.so",
	.object_name		= "stSnsSoiF37Obj",
	.combo_dev_attr     = &soi_f37_combo_dev_attr,
	.vi_dev_attr        = &soi_f37_dev_attr,
	.vi_pipe_attr       = &soi_f37_pipe_attr,
	.isp_pub_attr       = &soi_f37_pub_attr,
};

/***************************************************************************
 * SONY IMX307
 ***************************************************************************/
static combo_dev_attr_t sony_imx307_combo_dev_attr = {
	.devno = 0,
	.input_mode = INPUT_MODE_MIPI,
	.data_rate = MIPI_DATA_RATE_X1,
	.img_rect = { 0, 0, 1920, 1080 },
	{
		.mipi_attr = {
			.input_data_type = DATA_TYPE_RAW_12BIT,
			.wdr_mode = HI_MIPI_WDR_MODE_NONE,
			.lane_id = { 0, 1, 2, 3 }
		}
	}
};

static VI_DEV_ATTR_S sony_imx307_dev_attr = {
	.enIntfMode     = VI_MODE_MIPI,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32ComponentMask   = { 0xFFF00000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.as32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_DATA_SEQ_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_PULSE,
		.enVsyncNeg     = VI_VSYNC_NEG_LOW,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 0,  .u32HsyncAct = 1280,  .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,  .u32VsyncVact = 720, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0, .u32VsyncVbact = 0,   .u32VsyncVbbb = 0
		}
	},
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataReverse       = HI_FALSE,
	.stSize             = { 1920, 1080 },
	.stBasAttr         = {
		.stSacleAttr    = {
			.stBasSize = { 1920, 1080 },
		},
		.stRephaseAttr  = {
			.enHRephaseMode = VI_REPHASE_MODE_NONE,
			.enVRephaseMode = VI_REPHASE_MODE_NONE,
		}
	},
	.stWDRAttr          = {
		.enWDRMode      = WDR_MODE_NONE,
		.u32CacheLine   = 1080
	},
	.enDataRate		= DATA_RATE_X1
};

static VI_PIPE_ATTR_S sony_imx307_pipe_attr = {
	.enPipeBypassMode   = VI_PIPE_BYPASS_NONE,
	.bYuvSkip           = HI_FALSE,
	.bIspBypass         = HI_FALSE,
	.u32MaxW            = 1920,
	.u32MaxH            = 1080,
	.enPixFmt           = PIXEL_FORMAT_RGB_BAYER_12BPP,
	.enCompressMode     = COMPRESS_MODE_NONE,
	.enBitWidth         = DATA_BITWIDTH_12,
	.bNrEn              = HI_FALSE,
	.stNrAttr           = {
		.enPixFmt       = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
		.enBitWidth     = DATA_BITWIDTH_8,
		.enNrRefSource  = VI_NR_REF_FROM_RFR,
		.enCompressMode = COMPRESS_MODE_NONE,
	},
	.bSharpenEn         = HI_FALSE,
	.stFrameRate        = { -1, -1 }
};

static ISP_PUB_ATTR_S sony_imx307_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080,
	},
	.stSnsSize          = { 1920, 1080 },
	.f32FrameRate       = 30,
	.enBayer            = BAYER_RGGB,
	.enWDRMode          = WDR_MODE_NONE,
	.u8SnsMode          = 0,
};

static HIMPP_SENSOR_CONFIG sony_imx307_sensor_config = {
	.name               = "imx307",
	.module_path        = "libsns_imx307.so",
	.object_name		= "stSnsImx307Obj",
	.combo_dev_attr     = &sony_imx307_combo_dev_attr,
	.vi_dev_attr        = &sony_imx307_dev_attr,
	.vi_pipe_attr       = &sony_imx307_pipe_attr,
	.isp_pub_attr       = &sony_imx307_pub_attr,
};

/***************************************************************************
 * SONY IMX327
 ***************************************************************************/
static combo_dev_attr_t sony_imx327_combo_dev_attr = {
	.devno = 0,
	.input_mode = INPUT_MODE_MIPI,
	.data_rate = MIPI_DATA_RATE_X1,
	.img_rect = { 0, 0, 1920, 1080 },
	{
		.mipi_attr = {
			.input_data_type = DATA_TYPE_RAW_12BIT,
			.wdr_mode = HI_MIPI_WDR_MODE_NONE,
			.lane_id = { 0, 1, 2, 3 }
		}
	}
};

static VI_DEV_ATTR_S sony_imx327_dev_attr = {
	.enIntfMode     = VI_MODE_MIPI,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32ComponentMask   = { 0xFFF00000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.as32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_DATA_SEQ_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_PULSE,
		.enVsyncNeg     = VI_VSYNC_NEG_LOW,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 0,  .u32HsyncAct = 1280,  .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,  .u32VsyncVact = 720, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0, .u32VsyncVbact = 0,   .u32VsyncVbbb = 0
		}
	},
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataReverse       = HI_FALSE,
	.stSize             = { 1920, 1080 },
	.stBasAttr         = {
		.stSacleAttr    = {
			.stBasSize = { 1920, 1080 },
		},
		.stRephaseAttr  = {
			.enHRephaseMode = VI_REPHASE_MODE_NONE,
			.enVRephaseMode = VI_REPHASE_MODE_NONE,
		}
	},
	.stWDRAttr          = {
		.enWDRMode      = WDR_MODE_NONE,
		.u32CacheLine   = 1080
	},
	.enDataRate		= DATA_RATE_X1
};

static VI_PIPE_ATTR_S sony_imx327_pipe_attr = {
	.enPipeBypassMode   = VI_PIPE_BYPASS_NONE,
	.bYuvSkip           = HI_FALSE,
	.bIspBypass         = HI_FALSE,
	.u32MaxW            = 1920,
	.u32MaxH            = 1080,
	.enPixFmt           = PIXEL_FORMAT_RGB_BAYER_12BPP,
	.enCompressMode     = COMPRESS_MODE_NONE,
	.enBitWidth         = DATA_BITWIDTH_12,
	.bNrEn              = HI_FALSE,
	.stNrAttr           = {
		.enPixFmt       = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
		.enBitWidth     = DATA_BITWIDTH_8,
		.enNrRefSource  = VI_NR_REF_FROM_RFR,
		.enCompressMode = COMPRESS_MODE_NONE,
	},
	.bSharpenEn         = HI_FALSE,
	.stFrameRate        = { -1, -1 }
};

static ISP_PUB_ATTR_S sony_imx327_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 1920,
		.u32Height      = 1080,
	},
	.stSnsSize          = { 1920, 1080 },
	.f32FrameRate       = 30,
	.enBayer            = BAYER_RGGB,
	.enWDRMode          = WDR_MODE_NONE,
	.u8SnsMode          = 0,
};

static HIMPP_SENSOR_CONFIG sony_imx327_sensor_config = {
	.name               = "imx327",
	.module_path        = "libsns_imx327.so",
	.object_name		= "stSnsImx327Obj",
	.combo_dev_attr     = &sony_imx327_combo_dev_attr,
	.vi_dev_attr        = &sony_imx327_dev_attr,
	.vi_pipe_attr       = &sony_imx327_pipe_attr,
	.isp_pub_attr       = &sony_imx327_pub_attr,
};

/***************************************************************************
 * SONY IMX335
 ***************************************************************************/
static combo_dev_attr_t sony_imx335_combo_dev_attr = {
	.devno = 0,
	.input_mode = INPUT_MODE_MIPI,
	.data_rate = MIPI_DATA_RATE_X1,
	.img_rect = { 0, 0, 2592, 1944 },
	{
		.mipi_attr = {
			.input_data_type = DATA_TYPE_RAW_12BIT,
			.wdr_mode = HI_MIPI_WDR_MODE_NONE,
			.lane_id = { 0, 1, 2, 3 }
		}
	}
};

static VI_DEV_ATTR_S sony_imx335_dev_attr = {
	.enIntfMode     = VI_MODE_MIPI,
	.enWorkMode     = VI_WORK_MODE_1Multiplex,
	.au32ComponentMask   = { 0xFFF00000, 0x0 },
	.enScanMode     = VI_SCAN_PROGRESSIVE,
	.as32AdChnId     = { -1, -1, -1, -1 },
	.enDataSeq      = VI_DATA_SEQ_YUYV,
	.stSynCfg       = {
		.enVsync        = VI_VSYNC_PULSE,
		.enVsyncNeg     = VI_VSYNC_NEG_LOW,
		.enHsync        = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg     = VI_HSYNC_NEG_HIGH,
		.enVsyncValid   = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
		.stTimingBlank  = {
			.u32HsyncHfb = 0,  .u32HsyncAct = 1280,  .u32HsyncHbb = 0,
			.u32VsyncVfb = 0,  .u32VsyncVact = 720, .u32VsyncVbb = 0,
			.u32VsyncVbfb = 0, .u32VsyncVbact = 0,   .u32VsyncVbbb = 0
		}
	},
	.enInputDataType    = VI_DATA_TYPE_RGB,
	.bDataReverse       = HI_FALSE,
	.stSize             = { 2592, 1944 },
	.stBasAttr         = {
		.stSacleAttr    = {
			.stBasSize = { 2592, 1944 },
		},
		.stRephaseAttr  = {
			.enHRephaseMode = VI_REPHASE_MODE_NONE,
			.enVRephaseMode = VI_REPHASE_MODE_NONE,
		}
	},
	.stWDRAttr          = {
		.enWDRMode      = WDR_MODE_NONE,
		.u32CacheLine   = 1944
	},
	.enDataRate		= DATA_RATE_X1
};

static VI_PIPE_ATTR_S sony_imx335_pipe_attr = {
	.enPipeBypassMode   = VI_PIPE_BYPASS_NONE,
	.bYuvSkip           = HI_FALSE,
	.bIspBypass         = HI_FALSE,
	.u32MaxW            = 2592,
	.u32MaxH            = 1944,
	.enPixFmt           = PIXEL_FORMAT_RGB_BAYER_12BPP,
	.enCompressMode     = COMPRESS_MODE_NONE,
	.enBitWidth         = DATA_BITWIDTH_12,
	.bNrEn              = HI_FALSE,
	.stNrAttr           = {
		.enPixFmt       = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
		.enBitWidth     = DATA_BITWIDTH_8,
		.enNrRefSource  = VI_NR_REF_FROM_RFR,
		.enCompressMode = COMPRESS_MODE_NONE,
	},
	.bSharpenEn         = HI_FALSE,
	.stFrameRate        = { -1, -1 }
};

static ISP_PUB_ATTR_S sony_imx335_pub_attr = {
	.stWndRect          = {
		.s32X           = 0,
		.s32Y           = 0,
		.u32Width       = 2592,
		.u32Height      = 1944,
	},
	.stSnsSize          = { 2592, 1944 },
	.f32FrameRate       = 30,
	.enBayer            = BAYER_RGGB,
	.enWDRMode          = WDR_MODE_NONE,
	.u8SnsMode          = 0,
};

static HIMPP_SENSOR_CONFIG sony_imx335_sensor_config = {
	.name               = "imx335",
	.module_path        = "libsns_imx335.so",
	.object_name		= "stSnsImx335Obj",
	.combo_dev_attr     = &sony_imx335_combo_dev_attr,
	.vi_dev_attr        = &sony_imx335_dev_attr,
	.vi_pipe_attr       = &sony_imx335_pipe_attr,
	.isp_pub_attr       = &sony_imx335_pub_attr,
};


HimppVideoSensorMap himpp_video_sensor_map;

class HimppVideoSensorInitializer
{
public:
	HimppVideoSensorInitializer()
	{
#define addSensor(sensor) himpp_video_sensor_map.emplace(sensor.name, &sensor)
		addSensor(galaxycore_gc2053_sensor_config);
		addSensor(omnivision_os05a_sensor_config);
		addSensor(smartsens_sc4236_sensor_config);
		addSensor(smartsens_sc2231_sensor_config);
		addSensor(smartsens_sc2235_sensor_config);
		addSensor(smartsens_sc3235_sensor_config);
		addSensor(soi_f37_sensor_config);
		addSensor(sony_imx307_sensor_config);
		addSensor(sony_imx327_sensor_config);
		addSensor(sony_imx335_sensor_config);
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

const char* HimppVideoSensor::getSensorName()
{
	if (sensor_config)
		return sensor_config->name;
	return NULL;
}

const char* HimppVideoSensor::getModulePath()
{
	if (sensor_config)
		return sensor_config->module_path;
	return NULL;
}

const char* HimppVideoSensor::getObjectName()
{
	if (sensor_config)
		return sensor_config->object_name;
	return NULL;
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

HimppVideoSensor::operator VI_PIPE_ATTR_S* ()
{
	if (sensor_config)
		return sensor_config->vi_pipe_attr;
	return NULL;
}
