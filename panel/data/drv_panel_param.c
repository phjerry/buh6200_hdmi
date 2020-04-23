/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel data module
* Author: sdk
* Create: 2019-04-03
 */

#include "hi_type.h"
#include "hi_drv_panel.h"
#include "drv_panel_define.h"
#include "linux/hisilicon/securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static hi_drv_panel_info g_st_panel_info_array[] = {
    {
        .name = "lvds_auo_0_1920x1080_60hz",
        .width = 1920,                            /* 1920: default width */
        .height = 1080,                           /* 1080: default height */
        .intf_type = HI_PANEL_INTF_TYPE_VBONE,
        .data_bit_depth = HI_PANEL_BIT_DEPTH_8BIT,

        .timing_50hz.pixel_clk = 148500000,       /* 148500000: default param */
        .timing_50hz.htotal = 2200,               /* 2200: default param */
        .timing_50hz.hsync_fp = 100,              /* 100: default param */
        .timing_50hz.hsync_width = 40,            /* 40: default param */
        .timing_50hz.vtotal = 1125,               /* 1125: default param */
        .timing_50hz.vsync_fp = 10,               /* 10: default param */
        .timing_50hz.vsync_width = 5,             /* 5: default param */

        .timing_60hz.pixel_clk = 148500000,       /* 148500000: default param */
        .timing_60hz.htotal = 2200,               /* 2200: default param */
        .timing_60hz.hsync_fp = 100,              /* 100: default param */
        .timing_60hz.hsync_width = 40,            /* 40: default param */
        .timing_60hz.vtotal = 1125,               /* 1125: default param */
        .timing_60hz.vsync_fp = 10,               /* 10: default param */
        .timing_60hz.vsync_width = 5,             /* 5: default param */

        .time_sequence.intf_on_delay = 30,        /* 30: default param */
        .time_sequence.blacklight_on_delay = 700, /* 700: default param */
        .time_sequence.intf_off_delay = 30,       /* 30: default param */
        .time_sequence.tcon_off_delay = 30,       /* 30: default param */

        .division_type = HI_DRV_PANEL_DIVISION_ONE_OE,
        .pair_info.port_use_cnt = 1,              /* 1: default param */
        .pair_info.pair_use_cnt = 2,              /* 2: default param */
        .pair_info.pair_pn_swap = 0,
        .pair_info.reserved = 0,

        .signal_attr.com_voltage = PANEL_VOLTAGE_1200MV,
        .signal_attr.drv_current = PANEL_CURRENT_300MV,
        .signal_attr.emphasis = PANEL_EMP_0DB,
        .signal_attr.reserved = 0,

        .lvds_attr.lvds_link.link_map0 = PANEL_LVDS_LINK_MAP_PIXEL0,
        .lvds_attr.lvds_link.link_map0 = PANEL_LVDS_LINK_MAP_PIXEL1,
        .lvds_attr.lvds_fmt = PANEL_LVDS_FORMAT_VESA,

        .timing_sync_info.hsync_output = HI_TRUE,
        .timing_sync_info.vsync_output = HI_TRUE,
        .timing_sync_info.hsync_negative = HI_FALSE,
        .timing_sync_info.vsync_negative = HI_FALSE,

        .spread_info.spread_enable = HI_TRUE,
        .spread_info.reserved[0] = 0,
        .spread_info.reserved[1] = 0,             /* 1: default param */
        .spread_info.reserved[2] = 0,             /* 2: default param */
        .spread_info.spread_ratio = 0,
        .spread_info.spread_freq = 5,             /* 5: default param */

        .type_3d = HI_DRV_PANEL_3D_TYPE_2D,
        .blacklight_bit_width = HI_PANEL_BIT_DEPTH_8BIT,
        .backlight_range.min = 0,
        .backlight_range.max = 10000,             /* 10000: default param */
        .backlight_level = 10000,                 /* 10000: default param */
        .backlight_freq_50hz = 30000,             /* 30000: default param */
        .backlight_freq_60hz = 30000,              /* 30000: default param */

        .reserved0[0] = 0,
        .reserved0[1] = 0,                        /* 1: default param */
        .reserved0[2] = 0,                        /* 2: default param */
        .reserved0[3] = 0,                        /* 3: default param */
        .reserved0[4] = 0,                        /* 4: default param */
        .reserved0[5] = 0,                        /* 5: default param */
        .reserved0[6] = 0,                        /* 6: default param */
        .reserved0[7] = 0,                        /* 7: default param */
        .reserved0[8] = 0,                        /* 8: default param */
        .reserved0[9] = 0,                        /* 9: default param */
        .reserved0[10] = 0,                       /* 10: default param */
        .reserved0[11] = 0,                       /* 11: default param */
        .reserved0[12] = 0,                       /* 12: default param */
        .reserved0[13] = 0,                       /* 13: default param */
    },

    {
        .name = "vbo_auo_0_7680x4320_60hz",
        .width = 7680,                            /* 1920: default width */
        .height = 4320,                           /* 1080: default height */
        .intf_type = HI_PANEL_INTF_TYPE_VBONE,
        .data_bit_depth = HI_PANEL_BIT_DEPTH_8BIT,

        .timing_50hz.pixel_clk = 475200000,       /* 475200000: default param */
        .timing_50hz.htotal = 8800,               /* 8800: default param */
        .timing_50hz.hsync_fp = 352,              /* 352: default param */
        .timing_50hz.hsync_width = 176,           /* 176: default param */
        .timing_50hz.vtotal = 4500,               /* 4500: default param */
        .timing_50hz.vsync_fp = 16,               /* 16: default param */
        .timing_50hz.vsync_width = 20,            /* 20: default param */

        .timing_60hz.pixel_clk = 475200000,       /* 475200000: default param */
        .timing_60hz.htotal = 8800,               /* 8800: default param */
        .timing_60hz.hsync_fp = 352,              /* 352: default param */
        .timing_60hz.hsync_width = 176,           /* 176: default param */
        .timing_60hz.vtotal = 4500,               /* 4500: default param */
        .timing_60hz.vsync_fp = 16,               /* 16: default param */
        .timing_60hz.vsync_width = 20,            /* 20: default param */

        .time_sequence.intf_on_delay = 30,        /* 30: default param */
        .time_sequence.blacklight_on_delay = 700, /* 700: default param */
        .time_sequence.intf_off_delay = 30,       /* 30: default param */
        .time_sequence.tcon_off_delay = 30,       /* 30: default param */

        .division_type = HI_DRV_PANEL_DIVISION_ONE_OE,
        .pair_info.port_use_cnt = 1,              /* 1: default param */
        .pair_info.pair_use_cnt = 2,              /* 2: default param */
        .pair_info.pair_pn_swap = 0,
        .pair_info.reserved = 0,

        .signal_attr.com_voltage = PANEL_VOLTAGE_1200MV,
        .signal_attr.drv_current = PANEL_CURRENT_300MV,
        .signal_attr.emphasis = PANEL_EMP_0DB,
        .signal_attr.reserved = 0,

        .lvds_attr.lvds_link.link_map0 = PANEL_LVDS_LINK_MAP_PIXEL0,
        .lvds_attr.lvds_link.link_map0 = PANEL_LVDS_LINK_MAP_PIXEL1,
        .lvds_attr.lvds_fmt = PANEL_LVDS_FORMAT_VESA,

        .timing_sync_info.hsync_output = HI_TRUE,
        .timing_sync_info.vsync_output = HI_TRUE,
        .timing_sync_info.hsync_negative = HI_FALSE,
        .timing_sync_info.vsync_negative = HI_FALSE,

        .spread_info.spread_enable = HI_TRUE,
        .spread_info.reserved[0] = 0,
        .spread_info.reserved[1] = 0,             /* 1: default param */
        .spread_info.reserved[2] = 0,             /* 2: default param */
        .spread_info.spread_ratio = 0,
        .spread_info.spread_freq = 5,             /* 5: default param */

        .type_3d = HI_DRV_PANEL_3D_TYPE_2D,
        .blacklight_bit_width = HI_PANEL_BIT_DEPTH_8BIT,
        .backlight_range.min = 0,
        .backlight_range.max = 10000,             /* 10000: default param */
        .backlight_level = 10000,                 /* 10000: default param */
        .backlight_freq_50hz = 30000,             /* 30000: default param */
        .backlight_freq_60hz = 30000,             /* 30000: default param */

        .reserved0[0] = 0,
        .reserved0[1] = 0,                        /* 1: default param */
        .reserved0[2] = 0,                        /* 2: default param */
        .reserved0[3] = 0,                        /* 3: default param */
        .reserved0[4] = 0,                        /* 4: default param */
        .reserved0[5] = 0,                        /* 5: default param */
        .reserved0[6] = 0,                        /* 6: default param */
        .reserved0[7] = 0,                        /* 7: default param */
        .reserved0[8] = 0,                        /* 8: default param */
        .reserved0[9] = 0,                        /* 9: default param */
        .reserved0[10] = 0,                       /* 10: default param */
        .reserved0[11] = 0,                       /* 11: default param */
        .reserved0[12] = 0,                       /* 12: default param */
        .reserved0[13] = 0,                       /* 13: default param */
    },

};

hi_s32 drv_panel_get_default_panel_info(hi_u32 def_index, hi_u32 *total_num, hi_drv_panel_info *panel_info)
{
    hi_s32 ret;
    hi_u32 param_cnt = sizeof(g_st_panel_info_array) / sizeof(hi_drv_panel_info);

    if (def_index >= param_cnt) {
        hi_log_err("get default panel info failed, index must smaller than param_cnt!\n");
        return HI_FAILURE;
    }

    if ((panel_info == HI_NULL) || (total_num == HI_NULL)) {
        hi_log_err("get default panel info failed, point is NULL!\n");
        return HI_FAILURE;
    }

    *total_num = param_cnt;
    ret = memcpy_s(panel_info, sizeof(hi_drv_panel_info), &g_st_panel_info_array[def_index],
                   sizeof(hi_drv_panel_info));
    if (ret != HI_SUCCESS) {
        hi_log_err("panel_drv_get_default_panel_info failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

