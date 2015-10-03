/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
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
    const char*         name;
    const char*         module_path;
    ISP_IMAGE_ATTR_S*   isp_image_attr;
    ISP_INPUT_TIMING_S* isp_input_timing;
    VI_DEV_ATTR_S*      vi_dev_attr;
};

/***************************************************************************
 * Aptina AR0130 
 ***************************************************************************/

static ISP_IMAGE_ATTR_S aptina_ar0130_image_attr = {
    .u16Width           = 1280,
    .u16Height          = 720,
    .u16FrameRate       = 30,
    .enBayer            = BAYER_GRBG
};

static ISP_INPUT_TIMING_S aptina_ar0130_input_timing = {
    .enWndMode          = ISP_WIND_NONE
};

static VI_DEV_ATTR_S aptina_ar0130_dev_attr = {
    .enIntfMode     = VI_MODE_DIGITAL_CAMERA,
    .enWorkMode     = VI_WORK_MODE_1Multiplex,
    .au32CompMask   = { 0xFFF00000, 0x0 },
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
            .u32HsyncHfb = 0,  .u32HsyncAct = 1280, .u32HsyncHbb = 0,
            .u32VsyncVfb = 0,  .u32VsyncVact = 720, .u32VsyncVbb = 0,
            .u32VsyncVbfb = 0, .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
        }
    },    
    .enDataPath         = VI_PATH_ISP,
    .enInputDataType    = VI_DATA_TYPE_RGB
};

static HIMPP_SENSOR_CONFIG aptina_ar0130_sensor_config = {
    .name               = "ar0130",
    .module_path        = "libsns_ar0130_720p.so",
    .isp_image_attr     = &aptina_ar0130_image_attr,
    .isp_input_timing   = &aptina_ar0130_input_timing,
    .vi_dev_attr        = &aptina_ar0130_dev_attr
};


/***************************************************************************
 * Aptina MT9M034 
 ***************************************************************************/

static ISP_IMAGE_ATTR_S aptina_mt9m034_image_attr = {
    .u16Width           = 1280,
    .u16Height          = 720,
    .u16FrameRate       = 30,
    .enBayer            = BAYER_GRBG
};

static ISP_INPUT_TIMING_S aptina_mt9m034_input_timing = {
    .enWndMode          = ISP_WIND_NONE
};

static VI_DEV_ATTR_S aptina_mt9m034_dev_attr = {
    .enIntfMode     = VI_MODE_DIGITAL_CAMERA,
    .enWorkMode     = VI_WORK_MODE_1Multiplex,
    .au32CompMask   = { 0xFFF00000, 0x0 },
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
            .u32HsyncHfb = 0,  .u32HsyncAct = 1280, .u32HsyncHbb = 0,
            .u32VsyncVfb = 0,  .u32VsyncVact = 720, .u32VsyncVbb = 0,
            .u32VsyncVbfb = 0, .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
        }
    },    
    .enDataPath         = VI_PATH_ISP,
    .enInputDataType    = VI_DATA_TYPE_RGB
};

static HIMPP_SENSOR_CONFIG aptina_mt9m034_sensor_config = {
    .name               = "mt9m034",
    .module_path        = "libsns_9m034.so",
    .isp_image_attr     = &aptina_mt9m034_image_attr,
    .isp_input_timing   = &aptina_mt9m034_input_timing,
    .vi_dev_attr        = &aptina_mt9m034_dev_attr
};


/***************************************************************************
 * GalaxyCore GC1004 
 ***************************************************************************/

static ISP_IMAGE_ATTR_S galaxycore_gc1004_image_attr = {
    .u16Width           = 1280,
    .u16Height          = 720,
    .u16FrameRate       = 30,
    .enBayer            = BAYER_RGGB
};

static ISP_INPUT_TIMING_S galaxycore_gc1004_input_timing = {
    .enWndMode          = ISP_WIND_NONE,
    .u16HorWndStart     = 0,
    .u16HorWndLength    = 1280,
    .u16VerWndStart     = 0,
    .u16VerWndLength    = 720
};

static VI_DEV_ATTR_S galaxycore_gc1004_dev_attr = {
    .enIntfMode     = VI_MODE_DIGITAL_CAMERA,
    .enWorkMode     = VI_WORK_MODE_1Multiplex,
    .au32CompMask   = { 0xFFC00000, 0x0 },
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
    .enInputDataType    = VI_DATA_TYPE_RGB
};

static HIMPP_SENSOR_CONFIG galaxycore_gc1004_sensor_config = {
    .name               = "gc1004",
    .module_path        = "libsns_gc1004.so",
    .isp_image_attr     = &galaxycore_gc1004_image_attr,
    .isp_input_timing   = &galaxycore_gc1004_input_timing,
    .vi_dev_attr        = &galaxycore_gc1004_dev_attr
};


/***************************************************************************
 * Sony ICX692
 ***************************************************************************/

static ISP_IMAGE_ATTR_S sony_icx692_image_attr = {
    .u16Width           = 1280,
    .u16Height          = 720,
    .u16FrameRate       = 30,
    .enBayer            = BAYER_GRBG
};

static ISP_INPUT_TIMING_S sony_icx692_input_timing = {
    .enWndMode          = ISP_WIND_ALL,
    .u16HorWndStart     = 200,
    .u16HorWndLength    = 1280,
    .u16VerWndStart     = 2,
    .u16VerWndLength    = 720
};

static VI_DEV_ATTR_S sony_icx692_dev_attr = {
    .enIntfMode     = VI_MODE_DIGITAL_CAMERA,
    .enWorkMode     = VI_WORK_MODE_1Multiplex,
    .au32CompMask   = { 0xFFF00000, 0x0 },
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
            .u32HsyncHfb = 0,  .u32HsyncAct = 1280, .u32HsyncHbb = 0,
            .u32VsyncVfb = 0,  .u32VsyncVact = 720, .u32VsyncVbb = 0,
            .u32VsyncVbfb = 0, .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
        }
    },    
    .enDataPath         = VI_PATH_ISP,
    .enInputDataType    = VI_DATA_TYPE_RGB
};

static HIMPP_SENSOR_CONFIG sony_icx692_sensor_config = {
    .name               = "icx692",
    .module_path        = "libsns_icx692.so",
    .isp_image_attr     = &sony_icx692_image_attr,
    .isp_input_timing   = &sony_icx692_input_timing,
    .vi_dev_attr        = &sony_icx692_dev_attr
};


/***************************************************************************
 * Sony IMX104
 ***************************************************************************/

static ISP_IMAGE_ATTR_S sony_imx104_image_attr = {
    .u16Width           = 1280,
    .u16Height          = 720,
    .u16FrameRate       = 30,
    .enBayer            = BAYER_GBRG
};

static ISP_INPUT_TIMING_S sony_imx104_input_timing = {
    .enWndMode          = ISP_WIND_ALL,
    .u16HorWndStart     = 72,
    .u16HorWndLength    = 1280,
    .u16VerWndStart     = 20,
    .u16VerWndLength    = 720
};

static VI_DEV_ATTR_S sony_imx104_dev_attr = {
    .enIntfMode     = VI_MODE_DIGITAL_CAMERA,
    .enWorkMode     = VI_WORK_MODE_1Multiplex,
    .au32CompMask   = { 0xFFC00000, 0x0 },
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
            .u32HsyncHfb = 408, .u32HsyncAct = 1280, .u32HsyncHbb = 0,
            .u32VsyncVfb = 6,   .u32VsyncVact = 720, .u32VsyncVbb = 6,
            .u32VsyncVbfb = 0,  .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
        }
    },
    .enDataPath         = VI_PATH_ISP,
    .enInputDataType    = VI_DATA_TYPE_RGB
};

static HIMPP_SENSOR_CONFIG sony_imx104_sensor_config = {
    .name               = "imx104",
    .module_path        = "libsns_imx104.so",
    .isp_image_attr     = &sony_imx104_image_attr,
    .isp_input_timing   = &sony_imx104_input_timing,
    .vi_dev_attr        = &sony_imx104_dev_attr
};


/***************************************************************************
 * Sony IMX138
 ***************************************************************************/

static ISP_IMAGE_ATTR_S sony_imx138_image_attr = {
    .u16Width           = 1280,
    .u16Height          = 720,
    .u16FrameRate       = 30,
    .enBayer            = BAYER_GBRG
};

static ISP_INPUT_TIMING_S sony_imx138_input_timing = {
    .enWndMode          = ISP_WIND_ALL,
    .u16HorWndStart     = 72,
    .u16HorWndLength    = 1280,
    .u16VerWndStart     = 20,
    .u16VerWndLength    = 720
};

static VI_DEV_ATTR_S sony_imx138_dev_attr = {
    .enIntfMode     = VI_MODE_DIGITAL_CAMERA,
    .enWorkMode     = VI_WORK_MODE_1Multiplex,
    .au32CompMask   = { 0xFFF00000, 0x0 },
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
            .u32HsyncHfb = 0,  .u32HsyncAct = 1280, .u32HsyncHbb = 0,
            .u32VsyncVfb = 0,  .u32VsyncVact = 720, .u32VsyncVbb = 0,
            .u32VsyncVbfb = 0, .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
        }
    },    
    .enDataPath         = VI_PATH_ISP,
    .enInputDataType    = VI_DATA_TYPE_RGB
};

static HIMPP_SENSOR_CONFIG sony_imx138_sensor_config = {
    .name               = "imx138",
    .module_path        = "libsns_imx138.so",
    .isp_image_attr     = &sony_imx138_image_attr,
    .isp_input_timing   = &sony_imx138_input_timing,
    .vi_dev_attr        = &sony_imx138_dev_attr
};


/***************************************************************************
 * Sony IMX122
 ***************************************************************************/

static ISP_IMAGE_ATTR_S sony_imx122_image_attr = {
    .u16Width           = 1920,
    .u16Height          = 1080,
    .u16FrameRate       = 30,
    .enBayer            = BAYER_RGGB
};

static ISP_INPUT_TIMING_S sony_imx122_input_timing = {
    .enWndMode          = ISP_WIND_ALL,
    .u16HorWndStart     = 200,
    .u16HorWndLength    = 1920,
    .u16VerWndStart     = 18,
    .u16VerWndLength    = 1080
};

static VI_DEV_ATTR_S sony_imx122_dev_attr = {
    .enIntfMode     = VI_MODE_DIGITAL_CAMERA,
    .enWorkMode     = VI_WORK_MODE_1Multiplex,
    .au32CompMask   = { 0xFFF00000, 0x0 },
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
        .stTimingBlank = {
            .u32HsyncHfb = 0,  .u32HsyncAct = 1920,  .u32HsyncHbb = 0,
            .u32VsyncVfb = 0,  .u32VsyncVact = 1080, .u32VsyncVbb = 0,
            .u32VsyncVbfb = 0, .u32VsyncVbact = 0,   .u32VsyncVbbb = 0
        }
    },
    .enDataPath         = VI_PATH_ISP,
    .enInputDataType    = VI_DATA_TYPE_RGB
};

static HIMPP_SENSOR_CONFIG sony_imx122_sensor_config = {
    .name               = "imx122",
    .module_path        = "libsns_imx122.so",
    .isp_image_attr     = &sony_imx122_image_attr,
    .isp_input_timing   = &sony_imx122_input_timing,
    .vi_dev_attr        = &sony_imx122_dev_attr
};


/***************************************************************************
 * Sony IMX236
 ***************************************************************************/

static ISP_IMAGE_ATTR_S sony_imx236_image_attr = {
    .u16Width           = 1920,
    .u16Height          = 1080,
    .u16FrameRate       = 30,
    .enBayer            = BAYER_RGGB
};

static ISP_INPUT_TIMING_S sony_imx236_input_timing = {
    .enWndMode          = ISP_WIND_ALL,
    .u16HorWndStart     = 96,
    .u16HorWndLength    = 1920,
    .u16VerWndStart     = 12,
    .u16VerWndLength    = 1080
};

static VI_DEV_ATTR_S sony_imx236_dev_attr = {
    .enIntfMode     = VI_MODE_DIGITAL_CAMERA,
    .enWorkMode     = VI_WORK_MODE_1Multiplex,
    .au32CompMask   = { 0xFFF00000, 0x0 },
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
            .u32HsyncHfb    = 0, .u32HsyncAct   = 1920, .u32HsyncHbb    = 0,
            .u32VsyncVfb    = 0, .u32VsyncVact  = 1080, .u32VsyncVbb    = 0,
            .u32VsyncVbfb   = 0, .u32VsyncVbact = 0,    .u32VsyncVbbb   = 0
        }
    },
    .enDataPath         = VI_PATH_ISP,
    .enInputDataType    = VI_DATA_TYPE_RGB
};

static HIMPP_SENSOR_CONFIG sony_imx236_sensor_config = {
    .name               = "imx236",
    .module_path        = "libsns_imx236.so",
    .isp_image_attr     = &sony_imx236_image_attr,
    .isp_input_timing   = &sony_imx236_input_timing,
    .vi_dev_attr        = &sony_imx236_dev_attr
};


/***************************************************************************
 * Omni OV9712
 ***************************************************************************/

static ISP_IMAGE_ATTR_S omni_ov9712_image_attr = {
    .u16Width           = 1280,
    .u16Height          = 720,
    .u16FrameRate       = 30,
    .enBayer            = BAYER_BGGR
};

static ISP_INPUT_TIMING_S omni_ov9712_input_timing = {
    .enWndMode          = ISP_WIND_NONE
};

static VI_DEV_ATTR_S omni_ov9712_dev_attr = {
    .enIntfMode     = VI_MODE_DIGITAL_CAMERA,
    .enWorkMode     = VI_WORK_MODE_1Multiplex,
    .au32CompMask   = { 0xFFC00000, 0x0 },
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
            .u32HsyncHfb = 408, .u32HsyncAct = 1280, .u32HsyncHbb = 0,
            .u32VsyncVfb = 6,   .u32VsyncVact = 720, .u32VsyncVbb = 6,
            .u32VsyncVbfb = 0,  .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
        }
    },
    .enDataPath         = VI_PATH_ISP,
    .enInputDataType    = VI_DATA_TYPE_RGB
};

static HIMPP_SENSOR_CONFIG omni_ov9712_sensor_config = {
    .name               = "ov9712",
    .module_path        = "libsns_ov9712.so",
    .isp_image_attr     = &omni_ov9712_image_attr,
    .isp_input_timing   = &omni_ov9712_input_timing,
    .vi_dev_attr        = &omni_ov9712_dev_attr
};


/***************************************************************************
 * Aptina MT9P006
 ***************************************************************************/

static ISP_IMAGE_ATTR_S aptina_mt9p006_image_attr = {
    .u16Width           = 1920,
    .u16Height          = 1080,
    .u16FrameRate       = 30,
    .enBayer            = BAYER_GRBG
};

static ISP_INPUT_TIMING_S aptina_mt9p006_input_timing = {
    .enWndMode          = ISP_WIND_ALL,
    .u16HorWndStart     = 0,
    .u16HorWndLength    = 1920,
    .u16VerWndStart     = 2,
    .u16VerWndLength    = 1080
};

static VI_DEV_ATTR_S aptina_mt9p006_dev_attr = {
    .enIntfMode     = VI_MODE_DIGITAL_CAMERA,
    .enWorkMode     = VI_WORK_MODE_1Multiplex,
    .au32CompMask   = { 0xFFF00000, 0x0 },
    .enScanMode     = VI_SCAN_PROGRESSIVE,
    /*AdChnId*/
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
            .u32HsyncHfb = 0,  .u32HsyncAct = 1920,  .u32HsyncHbb = 0,
            .u32VsyncVfb = 0,  .u32VsyncVact = 1080, .u32VsyncVbb = 0,
            .u32VsyncVbfb = 0, .u32VsyncVbact = 0,   .u32VsyncVbbb = 0
        }
    },
    .enDataPath         = VI_PATH_ISP,
    .enInputDataType    = VI_DATA_TYPE_RGB    
};

static HIMPP_SENSOR_CONFIG aptina_mt9p006_sensor_config = {
    .name               = "mt9p006",
    .module_path        = "libsns_mt9p006.so",
    .isp_image_attr     = &aptina_mt9p006_image_attr,
    .isp_input_timing   = &aptina_mt9p006_input_timing,
    .vi_dev_attr        = &aptina_mt9p006_dev_attr
};


/***************************************************************************
 * SOI H22
 ***************************************************************************/

static ISP_IMAGE_ATTR_S soi_h22_image_attr = {
    .u16Width           = 1280,
    .u16Height          = 720,
    .u16FrameRate       = 30,
    .enBayer            = BAYER_BGGR
};

static ISP_INPUT_TIMING_S soi_h22_input_timing = {
    .enWndMode          = ISP_WIND_NONE
};

static VI_DEV_ATTR_S soi_h22_dev_attr = {
    .enIntfMode     = VI_MODE_DIGITAL_CAMERA,
    .enWorkMode     = VI_WORK_MODE_1Multiplex,
    .au32CompMask   = { 0x3FF00000, 0x0 },
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
            .u32HsyncHfb = 408, .u32HsyncAct = 1280, .u32HsyncHbb = 0,
            .u32VsyncVfb = 6,   .u32VsyncVact = 720, .u32VsyncVbb = 6,
            .u32VsyncVbfb = 0,  .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
        }
    },
    .enDataPath         = VI_PATH_ISP,
    .enInputDataType    = VI_DATA_TYPE_RGB
};

static HIMPP_SENSOR_CONFIG soi_h22_sensor_config = {
    .name               = "soih22",
    .module_path        = "libsns_soih22.so",
    .isp_image_attr     = &soi_h22_image_attr,
    .isp_input_timing   = &soi_h22_input_timing,
    .vi_dev_attr        = &soi_h22_dev_attr
};


/***************************************************************************
 * HIMAX 1375
 ***************************************************************************/

static ISP_IMAGE_ATTR_S himax_1375_image_attr = {
    .u16Width           = 1280,
    .u16Height          = 720,
    .u16FrameRate       = 30,
    .enBayer            = BAYER_BGGR
};

static ISP_INPUT_TIMING_S himax_1375_input_timing = {
    .enWndMode          = ISP_WIND_ALL,
    .u16HorWndStart     = 0,
    .u16HorWndLength    = 1280,
    .u16VerWndStart     = 4,
    .u16VerWndLength    = 720
};

static VI_DEV_ATTR_S himax_1375_dev_attr = {
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
            .u32HsyncHfb = 408, .u32HsyncAct = 1280, .u32HsyncHbb = 0,
            .u32VsyncVfb = 6,   .u32VsyncVact = 720, .u32VsyncVbb = 6,
            .u32VsyncVbfb = 0,  .u32VsyncVbact = 0,  .u32VsyncVbbb = 0
        }
    },
    .enDataPath         = VI_PATH_ISP,
    .enInputDataType    = VI_DATA_TYPE_RGB
};

static HIMPP_SENSOR_CONFIG himax_1375_sensor_config = {
    .name               = "himax1375",
    .module_path        = "libsns_himax1375.so",
    .isp_image_attr     = &himax_1375_image_attr,
    .isp_input_timing   = &himax_1375_input_timing,
    .vi_dev_attr        = &himax_1375_dev_attr
};


/***************************************************************************
 * Aptina AR0330
 ***************************************************************************/

static ISP_IMAGE_ATTR_S aptina_ar0330_image_attr = {
    .u16Width           = 1920,
    .u16Height          = 1080,
    .u16FrameRate       = 30,
    .enBayer            = BAYER_GRBG
};

static ISP_INPUT_TIMING_S aptina_ar0330_input_timing = {
    .enWndMode          = ISP_WIND_ALL,
    .u16HorWndStart     = 0,
    .u16HorWndLength    = 1920,
    .u16VerWndStart     = 0,
    .u16VerWndLength    = 1080
};

static VI_DEV_ATTR_S aptina_ar0330_dev_attr = {
    .enIntfMode     = VI_MODE_DIGITAL_CAMERA,
    .enWorkMode     = VI_WORK_MODE_1Multiplex,
    .au32CompMask   = { 0xFFF00000, 0x0 },
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
            .u32HsyncHfb = 0,  .u32HsyncAct  = 1920, .u32HsyncHbb = 0,
            .u32VsyncVfb = 0,  .u32VsyncVact = 1080, .u32VsyncVbb = 0,
            .u32VsyncVbfb = 0, .u32VsyncVbact = 0,   .u32VsyncVbbb = 0
        }
    },
    .enDataPath         = VI_PATH_ISP,
    .enInputDataType    = VI_DATA_TYPE_RGB
};

static HIMPP_SENSOR_CONFIG aptina_ar0330_sensor_config = {
    .name               = "ar0330",
    .module_path        = "libsns_ar0330.so",
    .isp_image_attr     = &aptina_ar0330_image_attr,
    .isp_input_timing   = &aptina_ar0330_input_timing,
    .vi_dev_attr        = &aptina_ar0330_dev_attr
};


/***************************************************************************
 * PixelPlus 3100K
 ***************************************************************************/

static ISP_IMAGE_ATTR_S pixelplus_3100k_image_attr = {
    .u16Width           = 1280,
    .u16Height          = 720,
    .u16FrameRate       = 30,
    .enBayer            = BAYER_BGGR
};

static ISP_INPUT_TIMING_S pixelplus_3100k_input_timing = {
    .enWndMode          = ISP_WIND_NONE
};

static VI_DEV_ATTR_S pixelplus_3100k_dev_attr = {
    .enIntfMode     = VI_MODE_DIGITAL_CAMERA,
    .enWorkMode     = VI_WORK_MODE_1Multiplex,
    .au32CompMask   = { 0xFFC00000, 0x0 },
    .enScanMode     = VI_SCAN_PROGRESSIVE,
    .s32AdChnId     = { -1, -1, -1, -1 },
    .enDataSeq      = VI_INPUT_DATA_YUYV,
    .stSynCfg           = {
        .enVsync        = VI_VSYNC_PULSE,
        .enVsyncNeg     = VI_VSYNC_NEG_HIGH,
        .enHsync        = VI_HSYNC_VALID_SINGNAL,
        .enHsyncNeg     = VI_HSYNC_NEG_HIGH,
        .enVsyncValid   = VI_VSYNC_VALID_SINGAL,
        .enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
        .stTimingBlank  = {
            .u32HsyncHfb    = 0, .u32HsyncAct   = 1280, .u32HsyncHbb    = 0,
            .u32VsyncVfb    = 0, .u32VsyncVact  = 720,  .u32VsyncVbb    = 0,
            .u32VsyncVbfb   = 0, .u32VsyncVbact = 0,    .u32VsyncVbbb   = 0
        }
    },
    .enDataPath         = VI_PATH_ISP,
    .enInputDataType    = VI_DATA_TYPE_RGB
};

static HIMPP_SENSOR_CONFIG pixelplus_3100k_sensor_config = {
    .name               = "po3100k",
    .module_path        = "libsns_po3100k.so",
    .isp_image_attr     = &pixelplus_3100k_image_attr,
    .isp_input_timing   = &pixelplus_3100k_input_timing,
    .vi_dev_attr        = &pixelplus_3100k_dev_attr
};

HimppVideoSensorMap himpp_video_sensor_map;

class HimppVideoSensorInitializer
{
public:
    HimppVideoSensorInitializer()
    {
#define addSensor(sensor) himpp_video_sensor_map.emplace(sensor.name, &sensor)
        addSensor(aptina_ar0130_sensor_config);
        addSensor(aptina_mt9m034_sensor_config);
        addSensor(galaxycore_gc1004_sensor_config);
        addSensor(sony_icx692_sensor_config);
        addSensor(sony_imx104_sensor_config);
        addSensor(sony_imx138_sensor_config);
        addSensor(sony_imx122_sensor_config);
        addSensor(sony_imx236_sensor_config);
        addSensor(omni_ov9712_sensor_config);
        addSensor(aptina_mt9p006_sensor_config);
        addSensor(soi_h22_sensor_config);
        addSensor(himax_1375_sensor_config);
        addSensor(aptina_ar0330_sensor_config);
        addSensor(pixelplus_3100k_sensor_config);
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

HimppVideoSensor::operator ISP_IMAGE_ATTR_S* ()
{
    if (sensor_config)
        return sensor_config->isp_image_attr;
    return NULL;
}

HimppVideoSensor::operator ISP_INPUT_TIMING_S* ()
{
    if (sensor_config)
        return sensor_config->isp_input_timing;
    return NULL;
}

HimppVideoSensor::operator VI_DEV_ATTR_S* ()
{
    if (sensor_config)
        return sensor_config->vi_dev_attr;
    return NULL;
}
