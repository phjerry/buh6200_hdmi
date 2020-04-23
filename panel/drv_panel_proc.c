/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel proc module
* Author: sdk
* Create: 2019-11-23
*/
#ifndef __DISP_PLATFORM_BOOT__

#include "hi_type.h"
#include "hi_drv_panel.h"
#include "drv_panel.h"
#include "drv_panel_proc.h"
#include "drv_panel_define.h"
#include "panel_version.h"
#include "hi_osal.h"

#ifdef PANEL_TMP
#include "drv_panel_ldm_proc.h"
#include "drv_panel_0ddim.h"
#include "drv_panel_pwm.h"
#include "hi_board.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define DRV_PANEL_PROC_BUFF_SIZE_MAX    256

DEFINE_SEMAPHORE(g_panel_mutex);

static hi_void drv_panel_proc_help_explanation(hi_drv_panel_info *base_info)
{
    osal_printk("\n------------------------------------------------PANEL"
        "---------------------------------------------------------\n");
    osal_printk("echo command    para1  para2   path               explanation\n");

#ifdef PANEL_TMP
    if (base_info->resolution_type == HI_DRV_PANEL_TYPE_FHD
        || base_info->resolution_type == HI_DRV_PANEL_TYPE_UHD) {
        osal_printk("echo fixrate    [0~3]         >/proc/msp/panel    "
            "fix out frm_rate           (0:off   1:50_hz    2:60_hz    3:other)\n");
    } else if (HI_DRV_PANEL_TYPE_FHD_4K_60_MISC == base_info->resolution_type) {
        osal_printk("echo miscfhd4k  [0~4]         >/proc/msp/panel    "
            "fix misc_fhd 4K frm_rate    (0:off   1:fhd50_hz 2:fhd60_hz 3:4_k50_hz  4:4_k60_hz)\n");
    } else if (HI_DRV_PANEL_TYPE_4K_30_60_MISC == base_info->resolution_type) {
        osal_printk("echo misc4k     [0-5]         >/proc/msp/panel    "
            "fix misc4_k frm_rate        (0:off   1:4_k24_hz  2:4_k25_hz  3:4_k30_hz  4:4_k50_hz  5:4_k60_hz)\n");
    } else if (HI_DRV_PANEL_TYPE_FHD_4K_30_60_MISC == base_info->resolution_type) {
        osal_printk("echo misc       [0~7]         >/proc/msp/panel    "
            "fix misc_all frm_rate       (0:off   1:fhd50_hz 2:fhd60_hz 3:4_k24_hz  4:4_k25_hz  "
            "5:4_k30_hz  6:4_k50_hz  7:4_k60_hz)\n");
    }
#endif

    osal_printk("echo power      [0/1]         >/proc/msp/panel    "
        "set panel power on/off    (0:power off 1:power on)\n");
    osal_printk("echo tcon       [0/1]         >/proc/msp/panel    "
        "set tcon power            (0:power off 1:power on) \n");
    osal_printk("echo intf       [0/1]         >/proc/msp/panel    "
        "set VBO/LVDS intf enable  (0:disable   1:enable) \n");
    osal_printk("echo blenable   [0/1]         >/proc/msp/panel    "
        "set backlight enable      (0:disable   1:enable) \n");
    osal_printk("echo bllevel    [0~max]       >/proc/msp/panel    "
        "set backlight level range [0~255] \n");
#ifdef PANEL_TMP
    drv_panel_pwm_proc_help();
#endif
    osal_printk("echo division   [0~3]         >/proc/msp/panel    "
        "set division type         (0:one_oe    1:one       2:two     3:four)\n");
    osal_printk("echo dimen      [0/1]         >/proc/msp/panel    "
        "set 0D/local dim enable   (0:disable   1:enable) \n");
}

static hi_void drv_panel_proc_help_tim_dbg(drv_panel_contex *panel_ctx)
{
    osal_printk("----------------------------------------------TIM DBG"
        "-------------------------------------------------------\n");
    osal_printk("echo dbgtimen   [0/1]         >/proc/msp/panel    "
        "set debug timming enable  (0:disable   1:enable)\n");
    if (panel_ctx->dbg_tim_info.dbg_tim_en) {
        osal_printk("echo dbgtimreint  1           >/proc/msp/panel    "
            "set debug timming reinit\n");
        osal_printk("echo dbgtimuse  [0/1]         >/proc/msp/panel    "
            "set use debug timming info(0:not use   1:use)\n");
        osal_printk("echo dbgclk     [0~600M]      >/proc/msp/panel    "
            "set debug timming clk     [0~600M]\n");
        osal_printk("echo dbghtotal  [width~]      >/proc/msp/panel    "
            "set debug htotal, must bigger than resolution width\n");
        osal_printk("echo dbgwidth   [0~htotal]    >/proc/msp/panel    "
            "set debug width, must smaller than htotal\n");
        osal_printk("echo dbghsfp    [0~htotal]    >/proc/msp/panel    "
            "set debug hsync_fp, must smaller than htotal\n");
        osal_printk("echo dbghswidth [0~htotal]    >/proc/msp/panel    "
            "set debug hsync_width, must smaller than htotal\n");
        osal_printk("echo dbgvtotal  [height~]     >/proc/msp/panel    "
            "set debug vtotal, must bigger than resolution height\n");
        osal_printk("echo dbgheight  [0~vtotal]    >/proc/msp/panel    "
            "set debug width, must smaller than vtotal\n");
        osal_printk("echo dbgvsfp    [0~vtotal]    >/proc/msp/panel    "
            "set debug vsync_fp, must smaller than vtotal\n");
        osal_printk("echo dbgvswidth [0~vtotal]    >/proc/msp/panel    "
            "set debug vsync_width, must smaller than vtotal\n");

        osal_printk("echo dbghsfpaut   1           >/proc/msp/panel    "
            "set debug hsync_fp auto\n");
        osal_printk("echo dbgvsfpaut   1           >/proc/msp/panel    "
            "set debug vsync_fp auto\n");
    }
}

static hi_void drv_panel_proc_help_vbo_dbg(hi_void)
{
    osal_printk("----------------------------------------------VBO CMD"
        "-------------------------------------------------------\n");
    osal_printk("echo vtstmd     [0~5]         >/proc/msp/panel    "
        "set vbo test mode         (0:off        1:over_turn_data 2:K28.5+data)\n");
    osal_printk("                                                  "
        "                          (3:K28.5-data 4:increase_data 5:prbs10_data)\n");
    osal_printk("echo vlockndbg  [0/1]         >/proc/msp/panel    "
        "set vbo LOCKN high         (0:sw_low         1:sw_high   2:hw_mode)\n");

    osal_printk("echo vbyte      [3/4/5]       >/proc/msp/panel    "
        "set vbo byte number       (3:8bit  4:10bit   5:12bit) \n");
    osal_printk("echo vdata      [0~6]         >/proc/msp/panel    "
        "set vbo data mode         (0:30bit444 1:36bit444 2:24bit444 3:18bit444)\n");
    osal_printk("                                                  "
        "                          (4:24bit422 5:20bit422 6:16bit422) \n");
    osal_printk("echo vbitwidth  [0/1]         >/proc/msp/panel    "
        "set vbo bitwidth          (0:8bit        1:10bit)\n");
    osal_printk("echo colorbar   [0/1]         >/proc/msp/panel    "
        "set colorbar enabel       (0:disable colorbar 1:enable colorbar\n");
}

static hi_void drv_panel_proc_help_lvds_dbg(hi_void)
{
    osal_printk("---------------------------------------------LVDS CMD"
        "-------------------------------------------------------\n");
    osal_printk("echo llinkmod   [0~3]         >/proc/msp/panel    "
        "set lvds linkmode         (0:1link 1:2link 2:4link  3:8link) \n");
    osal_printk("echo ldatafmt   [0/1/2/3]     >/proc/msp/panel    "
        "set lvds map              (0:VESA  1:JEIDA 2:FP     3:other) \n");
    osal_printk("echo lbitwidth  [0/1]         >/proc/msp/panel    "
        "set lvds bitwidth         (0:10bit 1:8bit) \n");

    osal_printk("echo lsyncout   [0/1]         >/proc/msp/panel    "
        "set lvds syncout enable   (0:disable       1:enable)\n");
    osal_printk("echo ldatainv   [0/1]         >/proc/msp/panel    "
        "set lvds data  invert     (0:invert        1:normal)\n");
    osal_printk("echo llinkmap   [1~4] [0~3]   >/proc/msp/panel    "
        "set lvds link map         (1:link1, 2:link2, 3:link3, 4:link4)"
        "(0:pixel0, 1:pixel1, 2:pixel2, 3:pixel3)\n");
}

static hi_void drv_panel_proc_help_tcon_dbg(hi_void)
{
    osal_printk("---------------------------------------------TCON CMD"
        "-------------------------------------------------------\n");
    osal_printk("echo sdlock     [0/1]         >/proc/msp/panel    "
        "set sdlock                (0:not sdlock     1:sdlock\n");
    osal_printk("echo tconbist      [0/16]         >/proc/msp/panel    "
        "Set Tcon bist   (0:disable       1~16 pattern) \n");
}

static hi_void drv_panel_proc_help_other_dbg(hi_void)
{
    osal_printk("---------------------------------------------other CMD"
        "-------------------------------------------------------\n");
    osal_printk("echo hsyncinv   [0/1]         >/proc/msp/panel    "
        "set hsync signal invert   (0:invert        1:normal)\n");
    osal_printk("echo vsyncinv   [0/1]         >/proc/msp/panel    "
        "set vsync signal invert   (0:invert        1:normal)\n");
    osal_printk("echo deinv      [0/1]         >/proc/msp/panel    "
        "set de signal    invert   (0:invert        1:normal)\n");
    osal_printk("echo cfgstat    [0/1/2]       >/proc/msp/panel    "
        "set dim strength          (0:weak          1:mid 2:strong)\n");
    osal_printk("echo current    [0~6]         >/proc/msp/panel    "
        "set drvcurrent            (0:200MV    1:250MV   2:300MV  3:350MV)\n");
    osal_printk("                                                  "
        "                          (4:400MV    5:450MV   6:500MV)\n");
    osal_printk("echo voltage    [0~14]        >/proc/msp/panel    "
        "set voltage               (0:400MV    1:450MV   2:500MV   3:550MV)\n");
    osal_printk("                                                  "
        "                         (4:600MV    5:650MV   6:700MV   7:800MV)\n");
    osal_printk("                                                  "
        "                          (8:900MV    9:1000MV  10:1100MV 11:1150MV)\n");
    osal_printk("                                                  "
        "                          (12:1200MV  13:1250MV  14:1300MV)\n");
    osal_printk("echo emp        [0~9]         >/proc/msp/panel    "
        "set emphasis              (0:0db  1:1db  2:2db  3:3db  4:4db)\n");
    osal_printk("                                                  "
        "                         (5:5db  6:6db  7:7db  8:8db  9:9db)\n");

    osal_printk("echo spreaden   [0/1]         >/proc/msp/panel    "
        "set spread enable         (0:disable   1:enable)\n");
    osal_printk("echo sfreq      [2~15]        >/proc/msp/panel    "
        "set spreadfreq            (lvds and vbone range from 2 to 5)\n");
    osal_printk("                                                  "
        "                         (tcon range from 2 to 15)\n");
    osal_printk("echo sratio     [0~31]        >/proc/msp/panel    "
        "set spreadratio           ([1~31])\n");
    osal_printk("echo pnswap     [0/1]         >/proc/msp/panel    "
        "set pnswap                (0:normal         1:swap\n");
#ifdef PANEL_TMP
#ifdef PANEL_LDM_SUPPORT
    osal_printk("echo doblyen    [0/1]         >/proc/msp/panel    "
        "set dobly scene enable    (0:disable        1:enable)\n");
    osal_printk("echo doblylevel [0~255]       >/proc/msp/panel    set dobly scene backlight level\n");
#endif
#endif
}

static int drv_panel_proc_help(hi_void)
{
    drv_panel_contex *panel_ctx = HI_NULL;
    hi_drv_panel_info *base_info = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    base_info = &panel_ctx->image_info.base_info;
    drv_panel_proc_help_explanation(base_info);
    drv_panel_proc_help_tim_dbg(panel_ctx);

    if (panel_ctx->image_info.base_info.intf_type == HI_PANEL_INTF_TYPE_VBONE) {
        drv_panel_proc_help_vbo_dbg();
    } else if (panel_ctx->image_info.base_info.intf_type == HI_PANEL_INTF_TYPE_LVDS) {
        drv_panel_proc_help_lvds_dbg();
    } else {
        drv_panel_proc_help_tcon_dbg();
    }

    if (panel_ctx->image_info.advance_info.ldm_info.ldm_en) {
#ifdef PANEL_TMP
#if PANEL_LDM_SUPPORT
        drv_panel_ldm_proc_help();
#if PANEL_LDM_USE_COM_SPI
        drv_panel_com_spi_proc_help();
#else
        drv_panel_ldm_spi_proc_help();
#endif
        if (!PANEL_LDM_MCU_EXIST) {
            drv_panel_bl_driver_proc_help();
        }
#endif
#endif
    }

    drv_panel_proc_help_other_dbg();

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_fix_rate(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;
    hi_drv_panel_info *base_info = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);
    base_info = &panel_ctx->image_info.base_info;

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */

    switch (value) {
        case HI_DRV_PANEL_FIXRATE_TYPE_NONE:
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_NONE;
            hi_log_warn("fixrate success: off\n");
            break;
        case HI_DRV_PANEL_FIXRATE_TYPE_FHD_50HZ:
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_UHD_50HZ;
            if (base_info->resolution_type == HI_DRV_PANEL_TYPE_FHD) {
                base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_FHD_50HZ;
            }
            hi_log_warn("fixrate to 50_hz\n");
            break;
        case HI_DRV_PANEL_FIXRATE_TYPE_FHD_60HZ:
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_UHD_60HZ;
            if (base_info->resolution_type == HI_DRV_PANEL_TYPE_FHD) {
                base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_FHD_60HZ;
            }
            hi_log_warn("fixrate to 60_hz\n");
            break;
        case HI_DRV_PANEL_FIXRATE_TYPE_FHD_OTHER:
            if (base_info->support_other) {
                base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_UHD_OTHER;
                if (base_info->resolution_type == HI_DRV_PANEL_TYPE_FHD) {
                    base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_FHD_OTHER;
                }
                hi_log_warn("fixrate to other\n");
            } else {
                hi_log_err("fix_rate_type is other, but support other flag is FALSE!\n");
            }
            break;
        default:
            hi_log_err("fix_rate_type parameter ERROR, correct range [0~3].\n");
            hi_err_print_u32(value);
            break;
    }
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_misc_fhd4_k(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 misc_value;
    drv_panel_contex *panel_ctx = HI_NULL;
    hi_drv_panel_info *base_info = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);
    base_info = &panel_ctx->image_info.base_info;

    misc_value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */

    switch (misc_value) {
        case HI_DRV_PANEL_FIXRATE_TYPE_NONE:
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_NONE;
            hi_log_warn("fixrate success: off\n");
            break;
        case HI_DRV_PANEL_FIXRATE_TYPE_FHD_50HZ:
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_FHD_50HZ;
            hi_log_warn("fixrate to FHD@50_hz\n");
            break;
        case HI_DRV_PANEL_FIXRATE_TYPE_FHD_60HZ:
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_FHD_60HZ;
            hi_log_warn("fixrate to FHD@60_hz\n");
            break;
        case HI_DRV_PANEL_FIXRATE_TYPE_UHD_50HZ:
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_UHD_50HZ;
            hi_log_warn("fixrate to UHD@50_hz\n");
            break;
        case HI_DRV_PANEL_FIXRATE_TYPE_UHD_60HZ:
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_UHD_60HZ;
            hi_log_warn("fixrate to UHD@50_hz\n");
            break;
        default:
            hi_log_err("parameter ERROR, correct range [0~4].\n");
            break;

    }
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_misc4_k(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 misc_4k_value
    drv_panel_contex *panel_ctx = HI_NULL;
    hi_drv_panel_info *base_info = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);
    base_info = &panel_ctx->image_info.base_info;

    hi_u32 misc_4k_value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */

    switch (misc_4k_value) {
        case HI_DRV_PANEL_FIXRATE_TYPE_NONE:
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_NONE;
            hi_log_warn("fixrate success: off\n");
            break;
        case HI_DRV_PANEL_FIXRATE_TYPE_UHD_24HZ:
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_UHD_24HZ;
            hi_log_warn("fixrate to UHD@24_hz\n");
            break;
        case HI_DRV_PANEL_FIXRATE_TYPE_UHD_25HZ:
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_UHD_25HZ;
            hi_log_warn("fixrate to UHD@25_hz\n");
            break;
        case HI_DRV_PANEL_FIXRATE_TYPE_UHD_30HZ:
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_UHD_30HZ;
            hi_log_warn("fixrate to UHD@30_hz\n");
            break;
        case HI_DRV_PANEL_FIXRATE_TYPE_UHD_50HZ:
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_UHD_50HZ;
            hi_log_warn("fixrate to UHD@50_hz\n");
            break;
        case HI_DRV_PANEL_FIXRATE_TYPE_UHD_60HZ:
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_UHD_60HZ;
            hi_log_warn("fixrate to UHD@60_hz\n");
            break;
        default:
            hi_log_err("parameter ERROR, correct range [0~5].\n");
            break;
    }
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_misc(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;
    hi_drv_panel_info *base_info = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);
    base_info = &panel_ctx->image_info.base_info;

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */

    switch (value) {
        case 0: /* 0: misc fix rate type adapt */
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_NONE;
            hi_log_warn("fixrate success: off\n");
            break;
        case 1: /* 1: misc fix rate type adapt */
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_FHD_50HZ;
            hi_log_warn("fixrate to FHD@50_hz\n");
            break;
        case 2: /* 2: misc fix rate type adapt */
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_FHD_60HZ;
            hi_log_warn("fixrate to FHD@60_hz\n");
            break;
        case 3: /* 3: misc fix rate type adapt */
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_UHD_24HZ;
            hi_log_warn("fixrate to UHD@24_hz\n");
            break;
        case 4: /* 4: misc fix rate type adapt */
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_UHD_25HZ;
            hi_log_warn("fixrate to UHD@25_hz\n");
            break;
        case 5: /* 5: misc fix rate type adapt */
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_UHD_30HZ;
            hi_log_warn("fixrate to UHD@30_hz\n");
            break;
        case 6: /* 6: misc fix rate type adapt */
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_UHD_50HZ;
            hi_log_warn("fixrate to UHD@50_hz\n");
            break;
        case 7: /* 7: misc fix rate type adapt */
            base_info->panel_fix_rate_type = HI_DRV_PANEL_FIXRATE_TYPE_UHD_60HZ;
            hi_log_warn("fixrate to UHD@60_hz\n");
            break;
        default:
            hi_log_err("parameter ERROR, correct range [0~7].\n");
            break;
    }
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_panel_power(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value > PANEL_BOOL_MAX_VALUE) {
        hi_log_err("parameter ERROR, correct range [0~1].\n");
        return HI_FAILURE;
    }
#ifdef PANEL_TMP
    panel_power_on panel_power;
    panel_power.id = PANEL_ID_0;
    panel_power.power_on = (hi_bool)value;

    drv_panel_set_power_on(panel_power.id, panel_power.power_on);
    hi_log_warn("set panel power:power_on\n");
    hi_warn_print_u32(panel_power.power_on);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_tcon_power(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value > PANEL_BOOL_MAX_VALUE) {
        hi_log_err("parameter ERROR, correct range [0~1].\n");
        return HI_FAILURE;
    }

    drv_panel_set_tcon_power((hi_bool)value);
    hi_log_warn("tcon success: tconpower=value\n");
    hi_warn_print_u32(value);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_intf_power(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 value;

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value > PANEL_BOOL_MAX_VALUE) {
        hi_log_err("parameter ERROR, correct range [0~1].\n");
        return HI_FAILURE;
    }
#ifdef PANEL_TMP
    hi_drv_panel_set_intf_power((hi_bool)value);
#endif
    hi_log_warn("intf success: intfenable enable=value\n");
    hi_warn_print_u32(value);

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_backlight_power(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value > PANEL_BOOL_MAX_VALUE) {
        hi_log_err("parameter ERROR, correct range [0~1].\n");
        return HI_FAILURE;
    }
#ifdef PANEL_TMP
    panel_bl_power_on panel_bl_power;
    panel_bl_power.id = PANEL_ID_0;
    panel_bl_power.enable = (hi_bool)value;

    drv_panel_set_backlight_power(panel_bl_power.id, panel_bl_power.enable);
    hi_log_warn("blen success: backlight_power enable=value\n");
    hi_warn_print_u32(panel_bl_power.enable);
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dbg_timming_enable(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (((value) != HI_TRUE) && ((value) != HI_FALSE)) {
        hi_log_err("parameter ERROR, correct range [0~1]!\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }

    panel_ctx->dbg_tim_info.dbg_tim_en = (hi_bool)value;
    hi_log_warn("dbgtimen success: debug timming enable=value\n");
    hi_warn_print_u32(value);

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dbg_timming_re_init(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    drv_panel_contex *panel_ctx = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    panel_ctx->dbg_tim_info.dbg_tim_re_init = HI_TRUE;
    hi_log_warn("dbgtimreint success: debug timming reinit\n");

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dbg_timming_use(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */

    if (((value) != HI_TRUE) && ((value) != HI_FALSE)) {
        hi_log_err("parameter ERROR, correct range [0~1]!\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }

    if ((hi_bool)value) {
        panel_ctx->dbg_tim_info.out_frm_frame = count_out_frame_rate(panel_ctx->dbg_tim_info.pixel_clk,
                                                                       panel_ctx->dbg_tim_info.htotal,
                                                                       panel_ctx->dbg_tim_info.vtotal);
    }

    panel_ctx->dbg_tim_info.use_debg_tim = (hi_bool)value;
    hi_log_warn("dbgtimuse success: debug timming use=value\n");
    hi_warn_print_u32(value);

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dbg_hor_timming_auto(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 hsync_fp_max;
    hi_u32 u16_hsync_fp = 0;
    drv_panel_contex *panel_ctx = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    panel_ctx->dbg_tim_info.u16_hsync_fp = PANEL_HORIZONTAL_TIM_MIN;
    hsync_fp_max = panel_ctx->dbg_tim_info.htotal - panel_ctx->dbg_tim_info.width
                    - panel_ctx->dbg_tim_info.u16_hsync_width;

    while (panel_ctx->dbg_tim_info.u16_hsync_fp != hsync_fp_max) {
        u16_hsync_fp = panel_ctx->dbg_tim_info.u16_hsync_fp;
        hi_log_err("current hsync_fp, hsync_fp_maxn");
        hi_err_print_u32(u16_hsync_fp);
        hi_err_print_u32(hsync_fp_max);
        panel_msleep(PANEL_MSLEEP_1000);
        panel_ctx->dbg_tim_info.u16_hsync_fp += PANEL_HORIZONTAL_TIM_MIN;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dbg_ver_timming_auto(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 vsync_fp_max;
    drv_panel_contex *panel_ctx = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    panel_ctx->dbg_tim_info.u16_vsync_fp = PANEL_VERICAL_TIM_MIN;
    vsync_fp_max = panel_ctx->dbg_tim_info.vtotal - panel_ctx->dbg_tim_info.height
                    - panel_ctx->dbg_tim_info.u16_vsync_width;

    while (panel_ctx->dbg_tim_info.u16_vsync_fp != vsync_fp_max) {
        hi_log_err("current vsync_fp, max\n");
        hi_err_print_u32(panel_ctx->dbg_tim_info.u16_vsync_fp);
        hi_err_print_u32(vsync_fp_max);
        panel_msleep(PANEL_MSLEEP_1000);
        panel_ctx->dbg_tim_info.u16_vsync_fp++;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dbg_timming_clk(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value > PANEL_PIXELCLK_MAX) {
        hi_log_err("parameter_error, correct range [0~PANEL_PIXELCLK_MAX].\n");
        hi_err_print_u32(value);
        hi_err_print_u32(PANEL_PIXELCLK_MAX);
        return HI_FAILURE;
    }

    panel_ctx->dbg_tim_info.pixel_clk = value;
    hi_log_warn("dbgclk success: debug timming clk=value\n");
    hi_warn_print_u32(value);

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dbg_timming_htotal(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    /* htotal must bigger than hresolution and must the mutpile 8 */
    if ((value <= panel_ctx->dbg_tim_info.width) || !drv_panel_check_eight_align_num(value)) {
        hi_log_err("parameter ERROR, correct range and align 8.\n");
        hi_err_print_u32(value);
        hi_err_print_u32(panel_ctx->dbg_tim_info.width);
        return HI_FAILURE;
    }

    panel_ctx->dbg_tim_info.htotal = value;
    hi_log_warn("dbghtotal success: debug timming htotal=value\n");
    hi_warn_print_u32(value);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dbg_timming_width(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if ((value >= panel_ctx->dbg_tim_info.htotal) || !drv_panel_check_eight_align_num(value)) {
        hi_log_err("parameter ERROR, correct range [32~htotal] and align 8.\n");
        hi_err_print_u32(value);
        hi_err_print_u32(panel_ctx->dbg_tim_info.htotal);
        return HI_FAILURE;
    }

    panel_ctx->dbg_tim_info.width = value;
    hi_log_warn("dbgwidth success: debug timming width=value\n");
    hi_warn_print_u32(value);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dbg_timming_hsync_fp(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;
    hi_u32 temp;

    drv_panel_get_panel_ctx(&panel_ctx);

    temp = panel_ctx->dbg_tim_info.htotal - panel_ctx->dbg_tim_info.width
              - panel_ctx->dbg_tim_info.u16_hsync_width;
    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if ((value < PANEL_HORIZONTAL_TIM_MIN) || (value > temp) || !drv_panel_check_eight_align_num(value)) {
        hi_log_err("parameter ERROR, correct range [8~temp].\n");
        hi_err_print_u32(value);
        hi_err_print_u32(temp);
        return HI_FAILURE;
    }

    panel_ctx->dbg_tim_info.u16_hsync_fp = value;
    hi_log_warn("dbghsfp success: debug timming hsyncfp=value\n");
    hi_warn_print_u32(value);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dbg_timming_hsync_width(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;
    hi_u32 temp;

    drv_panel_get_panel_ctx(&panel_ctx);

    temp = panel_ctx->dbg_tim_info.htotal - panel_ctx->dbg_tim_info.width;

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if ((value < PANEL_HORIZONTAL_TIM_MIN) || (value > temp) || !drv_panel_check_eight_align_num(value)) {
        hi_log_err("parameter_error, correct range [8~temp].\n");
        hi_err_print_u32(value);
        hi_err_print_u32(temp);
        return HI_FAILURE;
    }

    panel_ctx->dbg_tim_info.u16_hsync_width = value;
    hi_log_warn("dbghswidth success: debug timming hsyncwidth=value\n");
    hi_warn_print_u32(value);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dbg_timming_vtotal(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value <= panel_ctx->dbg_tim_info.height) {
        hi_log_err("parameter ERROR, correct range (height~].\n");
        hi_err_print_u32(value);
        hi_err_print_u32(panel_ctx->dbg_tim_info.height);
        return HI_FAILURE;
    }

    panel_ctx->dbg_tim_info.vtotal = value;
    hi_log_warn("dbgvtotal success: debug timming vtotal=value\n");
    hi_warn_print_u32(value);

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dbg_timming_height(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value > panel_ctx->dbg_tim_info.vtotal) {
        hi_log_err("parameter ERROR, correct range [0~vtotal].\n");
        hi_err_print_u32(value);
        hi_err_print_u32(panel_ctx->dbg_tim_info.vtotal);
        return HI_FAILURE;
    }

    panel_ctx->dbg_tim_info.height = value;
    hi_log_warn("dbgheight success: debug timming height=value\n");
    hi_warn_print_u32(value);

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dbg_timming_vsync_fp(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;
    hi_u32 temp;

    drv_panel_get_panel_ctx(&panel_ctx);

    temp = panel_ctx->dbg_tim_info.vtotal - panel_ctx->dbg_tim_info.height
              - panel_ctx->dbg_tim_info.u16_vsync_width;
    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value > temp) {
        hi_log_err("parameter ERROR, correct range [0~temp].\n");
        hi_err_print_u32(value);
        hi_err_print_u32(temp);
        return HI_FAILURE;
    }

    panel_ctx->dbg_tim_info.u16_vsync_fp = value;
    hi_log_warn("dbgvsfp success: debug timming vsyncfp=value\n");
    hi_warn_print_u32(value);

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dbg_timming_vsync_width(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;
    hi_u32 temp;

    drv_panel_get_panel_ctx(&panel_ctx);

    temp = panel_ctx->dbg_tim_info.vtotal - panel_ctx->dbg_tim_info.height;
    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value > temp) {
        hi_log_err("parameter ERROR, correct range [0~temp].\n");
        hi_err_print_u32(value);
        hi_err_print_u32(temp);
        return HI_FAILURE;
    }

    panel_ctx->dbg_tim_info.u16_vsync_width = value;
    hi_log_warn("dbgvswidth success: debug timming vsyncwidth=value\n");
    hi_warn_print_u32(value);

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_spread_enable(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;
    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value > PANEL_BOOL_MAX_VALUE) {
        hi_log_err("parameter[%d] ERROR, correct range [0~1].\n", value);
        return HI_FAILURE;
    }

    drv_panel_get_panel_ctx(&panel_ctx);
    hal_panel_set_spread_enable((hi_bool)value);
    hi_log_warn("spread_enable success: spread status=value\n");
    hi_warn_print_u32(value);

    if ((panel_ctx->image_info.base_info.intf_type == HI_PANEL_INTF_TYPE_LVDS)
        || (panel_ctx->image_info.base_info.intf_type == HI_PANEL_INTF_TYPE_VBONE)) {
        panel_ctx->intf_ctx.intf_attr.spread_enable = (hi_bool)value;
    } else {
        panel_ctx->image_info.tcon_info.ssc_en = (hi_bool)value;
    }
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_vbo_byte_num(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_drv_panel_vbo_byte_num byte_num;
    hi_char *byte_num_p[] = { "none", "none", "none", "3_byte", "4_byte", "5_byte", "butt" };

    byte_num = (hi_drv_panel_vbo_byte_num)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (byte_num >= HI_DRV_PANEL_VBO_BYTE_NUM_BUTT) {
        hi_log_err("parameter ERROR, correct range [3~5].\n");
        hi_err_print_str(byte_num_p[byte_num]);
        return HI_FAILURE;
    }

    drv_panel_set_vbo_byte_num(byte_num);
    hi_log_warn("vbyte success: vbo byte number=byte_num\n");
    hi_warn_print_str(byte_num_p[byte_num]);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_vbo_data_mode(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_drv_panel_vbo_data_mode data_mode;
    hi_char *data_mode_p[] = {
        "30BIT444", "36BIT444", "24BIT444", "18BIT444",
        "24BIT422", "20BIT422", "16BIT422", "butt"
    };
    data_mode = (hi_drv_panel_vbo_data_mode)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (data_mode >= HI_DRV_PANEL_VBO_DATAMODE_BUTT) {
        hi_log_err("parameter RROR, correct range [0~6].\n");
        hi_err_print_str((hi_u32)data_mode);
        return HI_FAILURE;
    }

    drv_panel_set_vbo_data_mode(data_mode);
    hi_log_warn("vdata success: vbo data mode=data_mode\n");
    hi_warn_print_str(data_mode_p[data_mode]);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_vbo_lockn_dbg(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    hi_char *lockn_dbg[] = { "low", "high", "hardware mode", "butt" };

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value > PANEL_LOCKN_DBG_MAX_VALUE) {
        hi_log_err("parameter ERROR, correct range [0~2].\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    } else if (value == PANEL_LOCKN_DBG_MAX_VALUE) {
        drv_panel_set_lockn_sw_mode(HI_FALSE);
    } else {
        drv_panel_set_lockn_sw_mode(HI_TRUE);
        drv_panel_set_lockn_high((hi_bool)value);
    }
    hi_log_warn("lockndbg success: lockndbg=value\n");
    hi_warn_print_str(lockn_dbg[value]);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_vbo_bit_width(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    hi_char *bit_width_p[] = { "8bit", "10bit" };

    drv_panel_contex *panel_ctx = HI_NULL;
    drv_panel_get_panel_ctx(&panel_ctx);

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value > 1) {
        hi_log_err("parameter ERROR, correct range [0~1].\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }

    if (panel_ctx->image_info.base_info.intf_type != HI_DRV_PANEL_INTFTYPE_VBONE) {
        hi_log_err("only VBO support to change bitwidth!\n");
        return HI_FAILURE;
    }

    hal_panel_set_vbo_enable(HI_FALSE);
    panel_ctx->image_info.base_info.data_bit_depth = value;
    if (value == 0) {
        panel_ctx->image_info.base_info.vbo_attr.data_mode = HI_DRV_PANEL_VBO_DATAMODE_24BIT444;
        panel_ctx->image_info.base_info.vbo_attr.vbo_byte_num = HI_DRV_PANEL_VBO_BYTE_NUM_3;
        hal_panel_set_vbo_data_mode(HI_DRV_PANEL_VBO_DATAMODE_24BIT444);
        hal_panel_set_vbo_byte_num(HI_DRV_PANEL_VBO_BYTE_NUM_3);
    } else {
        panel_ctx->image_info.base_info.vbo_attr.data_mode = HI_DRV_PANEL_VBO_DATAMODE_30BIT444;
        panel_ctx->image_info.base_info.vbo_attr.vbo_byte_num = HI_DRV_PANEL_VBO_BYTE_NUM_4;
        hal_panel_set_vbo_data_mode(HI_DRV_PANEL_VBO_DATAMODE_30BIT444);
        hal_panel_set_vbo_byte_num(HI_DRV_PANEL_VBO_BYTE_NUM_4);
    }

    hal_panel_set_vbo_enable(HI_TRUE);
    hi_log_warn("set_vbo_bit_width success: vbo_bit_width=value\n");
    hi_warn_print_str(bit_width_p[value]);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_vbo_test_mode(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    hi_bool tst_enable = HI_FALSE;
    hi_drv_panel_vbo_test_mode tst_mode = HI_DRV_PANEL_VBO_TEST_MODE_OVERTURN_DATA;
    hi_char *vb1_test_mode[] = { "over_turn_data", "K28.5+data", "K28.5-data",
                                "increase_data", "prbs10_data", "off"
    };

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value > HI_DRV_PANEL_VBO_TEST_MODE_BUTT) {
        hi_log_err("parameter ERROR, correct range [0~5].\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }

    switch (value) {
        case HI_DRV_PANEL_VBO_TEST_MODE_OVERTURN_DATA:
            tst_enable = HI_TRUE;
            break;
        case HI_DRV_PANEL_VBO_TEST_MODE_K285PLUS_DATA:
            tst_mode = HI_DRV_PANEL_VBO_TEST_MODE_K285PLUS_DATA;
            tst_enable = HI_TRUE;
            break;
        case HI_DRV_PANEL_VBO_TEST_MODE_K285REDUCE_DATA:
            tst_mode = HI_DRV_PANEL_VBO_TEST_MODE_K285REDUCE_DATA;
            tst_enable = HI_TRUE;
            break;
        case HI_DRV_PANEL_VBO_TEST_MODE_INCREASE_DATA:
            tst_mode = HI_DRV_PANEL_VBO_TEST_MODE_INCREASE_DATA;
            tst_enable = HI_TRUE;
            break;
        case HI_DRV_PANEL_VBO_TEST_MODE_PRBS10_DATA:
            tst_mode = HI_DRV_PANEL_VBO_TEST_MODE_PRBS10_DATA;
            tst_enable = HI_TRUE;
            break;
        default:
            tst_enable = HI_FALSE;
            break;
    }

    hi_log_warn("vtstmd success: vbo test mode\n");
    hi_warn_print_u32(tst_enable);
    hi_warn_print_str(vb1_test_mode[value]);
    drv_panel_set_vbo_test_mode_enable(tst_enable);
    drv_panel_set_vbo_test_mode(tst_mode);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_vbo_color_bar(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value > PANEL_BOOL_MAX_VALUE) {
        hi_log_err("parameter ERROR, correct range [0~1].\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }

    drv_panel_set_vbo_color_bar((hi_bool)value);
    hi_log_warn("set_color_bar success: color_bar value? open: close\n");
    hi_warn_print_u32(value);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_lvds_link_mode(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_panel_link_type link_mode;
    hi_char *link_mode_p[] = { "1LINK", "2LINK", "4LINK", "8LINK", "16LINK", "butt" };
    drv_panel_contex *panel_ctx = HI_NULL;
    drv_panel_get_panel_ctx(&panel_ctx);

    HI_UNUSED(link_mode_p[0]);

    link_mode = (hi_drv_panel_link_type)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (link_mode >= HI_DRV_PANEL_LINKTYPE_MAX) {
        hi_log_err("parameter ERROR, correct range [0~3]!\n");
        hi_err_print_u32((hi_u32)link_mode);
        return HI_FAILURE;
    }

    panel_ctx->panel_cfg.link_type = link_mode;
#ifdef PANEL_TMP
    drv_panel_set_lvds_link_mode(link_mode);
#endif
    hi_log_warn("llink success: lvds link mode:link_mode\n");
    hi_warn_print_str(link_mode_p[link_mode]);

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_lvds_data_format(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_panel_lvds_format fmt;
#ifdef PANEL_TMP
    hi_char *fmt_p[] = { "VESA", "JEIDA", "FP", "but" };
#endif
    fmt = (hi_drv_panel_lvds_format)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */

    if (fmt >= PANEL_LVDS_FORMAT_MAX) {
        hi_log_err("parameter ERROR, correct range [0~2]!\n");
        hi_err_print_u32((hi_u32)fmt);
        return HI_FAILURE;
    }
#ifdef PANEL_TMP
    panel_lvds panel_lvds_info = {0};
    panel_lvds_info.id = PANEL_ID_0;
    drv_panel_get_lvds_attr(panel_lvds_info.id, &panel_lvds_info.lvds_attr);

    panel_lvds_info.lvds_attr.lvds_fmt = fmt;
    drv_panel_set_lvds_attr(panel_lvds_info.id, &panel_lvds_info.lvds_attr);

    hi_log_warn("ldatafmt success: lvds map:fmt\n");
    hi_warn_print_str(fmt_p[fmt]);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_lvds_bitwidth(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_drv_panel_bit_width bit_width;
    hi_char *bit_width_p[] = { "8_bit", "10_bit", "12_bit", "but" };

    bit_width = (hi_drv_panel_bit_width)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (bit_width >= HI_DRV_PANEL_BITWIDTH_BUTT) {
        hi_log_err("parameter ERROR, correct range [0~2]!\n");
        hi_err_print_u32((hi_u32)bit_width);
        return HI_FAILURE;
    }

    drv_panel_set_lvds_bit_mode(bit_width);
    hi_log_warn("lbitwidth success: lvds bitwidth:bit_width\n");
    hi_warn_print_str(bit_width_p[bit_width]);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_lvds_sync_out_enable(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value > PANEL_BOOL_MAX_VALUE) {
        hi_log_err("parameter ERROR, correct range [0~1].\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }

    drv_panel_set_lvds_sync_out_enable((hi_bool)value);
    hi_log_warn("syncout success: syncout=value?  enable: disable\n");
    hi_warn_print_u32(value);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_lvds_data_invert(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* 1 is index */
    if (value > PANEL_BOOL_MAX_VALUE) {
        hi_log_err("parameter ERROR, correct range [0~1].\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }

    drv_panel_set_lvds_invert(HI_DRV_PANEL_LVDS_INV_DATA, (hi_bool)value);
    hi_log_warn("datainv success: lvds data invert:value? invert: normal\n");
    hi_warn_print_u32(value);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_lvds_link_map(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 link;
    drv_panel_contex *panel_ctx = HI_NULL;
    hi_drv_panel_info *base_info = HI_NULL;
    hi_drv_panel_lvds_linkmap_type link_map;
    hi_drv_panel_lvds_linkmap link_map_p = {0};
    hi_char *link_map_ch[] = { "PIXEL0", "PIXEL1", "PIXEL2", "PIXEL3", "butt" };
    drv_panel_get_panel_ctx(&panel_ctx);
    base_info = &panel_ctx->image_info.base_info;

    link = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN));
    if ((link < 1) || (link > 4)) {  /* lvds total 4 links: link[1~4] */
        hi_log_err("parameter ERROR, correct range link[1~4].\n");
        hi_err_print_u32(link);
        return HI_FAILURE;
    }
    link_map = (hi_drv_panel_lvds_linkmap_type)(osal_strtol(arg1_p[2], NULL, PANEL_PARSE_PROC_LEN)); /* 2 is index */
    if (link_map > HI_DRV_PANEL_LVDS_LINKMAP_BUTT) {
        hi_log_err("parameter ERROR, correct range [0~3].\n");
        hi_err_print_u32(link_map);
        return HI_FAILURE;
    }

    drv_panel_get_link_map(&link_map_p);

    /* modify link map */
    if (link == 1) {        /* link1 */
        link_map_p.link1 = link_map;
        base_info->lvds_attr.lvds_link_map.link1 = link_map;
    } else if (link == 2) { /* link2 */
        link_map_p.link2 = link_map;
        base_info->lvds_attr.lvds_link_map.link2 = link_map;
    } else if (link == 3) { /* link3 */
        link_map_p.link3 = link_map;
        base_info->lvds_attr.lvds_link_map.link3 = link_map;
    } else {                /* link4 */
        link_map_p.link4 = link_map;
        base_info->lvds_attr.lvds_link_map.link4 = link_map;
    }

    drv_panel_set_link_map(&link_map_p);
    hi_log_warn("llink map success: lvds link map:link_map\n");
    hi_warn_print_str(link_map_ch[link_map]);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dim_enable(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */
    panel_check_bool_value(value);

    drv_panel_get_panel_ctx(&panel_ctx);

    hi_drv_panel_set_dim_enable(value);

    hi_log_warn("dimen success: dimming enable:value ? TRUE : FALSE\n");
    hi_warn_print_u32(value);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dobly_en_test(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */

    if (((value) != HI_TRUE) && ((value) != HI_FALSE)) {
        hi_log_err("parameter ERROR, correct range [0~1]!\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }

    drv_panel_get_panel_ctx(&panel_ctx);

    panel_ctx->dolby_en_test_flg = value;

    hi_log_warn("dobly_en_test success: dobly_en_test:value ? TRUE : FALSE\n");
    hi_warn_print_u32(value);

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_dobly_level_test(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */
    if (value > DIM_BACKLIGHT_MAX) {
        hi_log_err("parameter ERROR, correct range [0~max]!\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }

    drv_panel_get_panel_ctx(&panel_ctx);

    panel_ctx->dolby_test_level = value;

    hi_log_warn("dobly_level success: dobly_level:value\n");
    hi_warn_print_u32(value);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_tconbist(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    HI_CHAR *pBistType[] = {"Off", "White", "Black", "Red", "Green", "Blue", "White edge with black", "chessboard",
                            "Colorbar", "Horizontal gradient gray of Red", "Horizontal gradient gray of Green",
                            "Horizontal gradient gray of Blue", "Horizontal gradient gray of White",
                            "Vertical gradient gray of Red", "Vertical gradient gray of Green",
                            "Vertical gradient gray of Blue", "Vertical gradient gray of White"};

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */
    if (value >= 17) /* l7 is index */
    {
        hi_log_err("parameter ERROR, Correct range [0~16].\n");
        return HI_FAILURE;
    }
    if (value == 0) /* 0 is index */
    {
        hal_panel_set_tconbist_en(HI_FALSE);
        return HI_FAILURE;
    }

    hal_panel_set_tconbist_en(HI_TRUE);
    hal_panel_set_tcon_patnum(value-1); /* l is index */
    hi_log_warn("tcon bist success: bist type is %s\n", pBistType[value]);
#endif
    return HI_SUCCESS;
}


static hi_s32 drv_panel_proc_set_backlight_level(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 level;
    drv_panel_contex *panel_ctx = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    level = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */
#ifdef PANEL_TMP
    hi_s32 ret;
    panel_bl_level panel_bl = {0};
    panel_bl.id = PANEL_ID_0;
    panel_bl.level = level;
    ret = drv_panel_set_backlight_level(panel_bl.id, panel_bl.level);
    if (ret != HI_SUCCESS) {
        hi_log_err("parameter ERROR\n");
        hi_err_print_u32(level);
        return HI_FAILURE;
    }

    hi_log_warn("set_bl_level success: bl_level:level\n");
    hi_warn_print_u32(level);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_hsync_invert(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */
    if (((value) != HI_TRUE) && ((value) != HI_FALSE)) {
        hi_log_err("parameter ERROR, correct range [0~1]!\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }

    panel_ctx->panel_cfg.hsync_invert = (hi_bool)value;
    hi_log_warn("syncinv success: hsync invert:value? invert : normal\n");
    hi_warn_print_u32(value);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_vsync_invert(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */
    if (((value) != HI_TRUE) && ((value) != HI_FALSE)) {
        hi_log_err("parameter ERROR, correct range [0~1]!\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }

    panel_ctx->panel_cfg.vsync_invert = (hi_bool)value;
    hi_log_warn("syncinv success: vsync invert:value? invert : normal\n");
    hi_warn_print_u32(value);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_de_invert(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    drv_panel_contex *panel_ctx = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */
    if (((value) != HI_TRUE) && ((value) != HI_FALSE)) {
        hi_log_err("parameter ERROR, correct range [0~1]!\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }

    panel_ctx->panel_cfg.de_invert = (hi_bool)value;
    hi_log_warn("deinv success: de invert:value? invert : normal\n");
    hi_warn_print_u32(value);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_division_type(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    hi_char *division_type_p[] = { "one_oe", "one", "two", "four", "butt" };
    drv_panel_contex *panel_ctx = HI_NULL;
    hi_drv_panel_cfg *panel_cfg_p = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    panel_cfg_p = &panel_ctx->panel_cfg;

    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */
    if (value >= HI_DRV_PANEL_DIVISION_MAX) {
        hi_log_err("parameter ERROR, correct range [0~3].\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }

    panel_cfg_p->division_type = (hi_drv_panel_division_type)value;
    hal_panel_set_vbo_lane_swap(panel_cfg_p->division_type,
                                panel_ctx->image_info.base_info.panel_link_type);
    hi_log_warn("division success: panel division_type=value\n");
    hi_warn_print_str(division_type_p[value]);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_drv_current(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u8 i = 0;
    hi_drv_panel_current drv_current;
    hi_char *drv_current_p[] = { "200MV", "250MV", "300MV", "350MV", "400MV", "450MV", "500MV"
                                 "butt"
    };
    drv_panel_contex *panel_ctx = HI_NULL;
    drv_panel_get_panel_ctx(&panel_ctx);
    drv_current = (hi_drv_panel_current)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */
    if (drv_current >= HI_DRV_PANEL_CURRENT_BUTT) {
        hi_log_err("parameter ERROR, correct range [0~6].\n");
        hi_err_print_u32((hi_u32)drv_current);
        return HI_FAILURE;
    }

    for (i = 0; i < LANE_NUM; i++) {
        panel_ctx->combo_attr.au8_drv_current[i] = (hi_u8)drv_current;
        panel_ctx->image_info.tcon_info.au32_drv_current[i] = drv_current;
    }
    hal_panel_set_phy_drv_current(panel_ctx->image_info.base_info.intf_type,
                                  (const hi_u8 *)panel_ctx->combo_attr.au8_drv_current);
    hal_panel_set_phy_pre_emphasis(panel_ctx->image_info.base_info.intf_type,
                                   (const hi_u8 *)panel_ctx->combo_attr.au8_drv_current,
                                   (const hi_u8 *)panel_ctx->combo_attr.au8_pre_emphasis);
    hi_log_warn("drvcurrent success: drvcurrent=drv_current\n");
    hi_warn_print_str(drv_current_p[drv_current]);
#endif
    return HI_SUCCESS;
}
static hi_s32 drv_panel_proc_set_voltage(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u8 i = 0;
    hi_drv_panel_voltage voltage;
    hi_char *voltage_p[] = {
            "400MV", "450MV", "500MV", "550MV", "600MV", "650MV", "700MV", "800MV",
            "900MV", "1000MV", "1100MV", "1150MV", "1200MV", "1250MV", "1300MV", "butt"
    };

    drv_panel_contex *panel_ctx = HI_NULL;
    drv_panel_get_panel_ctx(&panel_ctx);
    voltage = (hi_drv_panel_voltage)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */
    if (voltage >= HI_DRV_PANEL_VOLTAGE_BUTT) {
        hi_log_err("parameter ERROR, correct range [0~11].\n");
        hi_err_print_u32((hi_u32)voltage);
        return HI_FAILURE;
    }
    for (i = 0; i < LANE_NUM; i++) {
        panel_ctx->combo_attr.au8_com_voltage[i] = (hi_u8)voltage;
        panel_ctx->image_info.tcon_info.au32_com_vlotage[i] = voltage;
    }
    hal_panel_set_phy_com_voltage(panel_ctx->combo_attr.au8_com_voltage, LANE_NUM);
    hi_log_warn("voltage success: voltage=voltage\n");
    hi_warn_print_str(voltage_p[voltage]);
#endif
    return HI_SUCCESS;
}
static hi_s32 drv_panel_proc_set_emphasis(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u8 i = 0;
    hi_drv_panel_emp emphasis;
    hi_char *emp_p[] = {
        "0DB", "1DB", "2DB", "3DB", "4DB",
        "5DB", "6DB", "7DB", "8DB", "9DB", "butt"
    };
    drv_panel_contex *panel_ctx = HI_NULL;
    drv_panel_get_panel_ctx(&panel_ctx);
    emphasis = (hi_drv_panel_emp)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */
    if (emphasis >= HI_DRV_PANEL_EMP_BUTT) {
        hi_log_err("parameter_error, correct range [0~9].\n");
        hi_err_print_u32((hi_u32)emphasis);
        return HI_FAILURE;
    }

    for (i = 0; i < LANE_NUM; i++) {
        panel_ctx->combo_attr.au8_pre_emphasis[i] = (hi_u8)emphasis;
        panel_ctx->image_info.tcon_info.au32_pre_emphasis[i] = emphasis;
    }
    hal_panel_set_phy_pre_emphasis(panel_ctx->image_info.base_info.intf_type,
                                   (const hi_u8 *)panel_ctx->combo_attr.au8_drv_current,
                                   (const hi_u8 *)panel_ctx->combo_attr.au8_pre_emphasis);

    hi_log_warn("emphasis success: emphasis=emphasis\n");
    hi_warn_print_str(emp_p[emphasis]);
#endif
    return HI_SUCCESS;
}
static hi_s32 drv_panel_proc_set_spead_freq(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_char *spread_lvds_p[] = { "93.75KHZ", "62.5KHZ", "46.875KHZ", "37.5KHZ ", "butt" };
    hi_char *spread_vbo_p[] = { "46.875KHZ", "31.25KHZ", "23.438KHZ", "18.75KHZ", "93P875KHZ",
        "62P5KHZ", "37P5KHZ", "26P786KHZ", "20P833KHZ", "butt"
    };
    hi_char *spread_tcon_p[] = { "93.75KHZ", "62.5KHZ", "46.875KHZ", "37.5KHZ",
        "31.25KHZ", "26.7857KHZ", "23.4375KHZ", "20.833KHZ", "18.75KHZ",
        "17.045KHZ", "15.625KHZ", "14.423KHZ", "13.393KHZ", "12.5KHZ", "butt"
    };
    hi_u32 spread = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */
    drv_panel_contex *panel_ctx = HI_NULL;
    drv_panel_get_panel_ctx(&panel_ctx);
    if (panel_ctx->image_info.base_info.intf_type == HI_DRV_PANEL_INTFTYPE_LVDS) {
        if (spread >= HI_DRV_PANEL_LVDS_SSFREQ_BUTT || spread < HI_DRV_PANEL_LVDS_SSFREQ_93P75KHZ) {
            hi_log_err("parameter_error, LVDS correct range [2~5].\n");
            hi_err_print_u32(spread);
            return HI_FAILURE;
        } else {
            hi_log_warn("spread_freq success: spread_freq\n");
            hi_warn_print_str(spread_lvds_p[spread - HI_DRV_PANEL_LVDS_SSFREQ_93P75KHZ]);
            panel_ctx->intf_ctx.intf_attr.lvds_spread_freq = (hi_drv_panel_lvds_ssfreq)spread;
        }
    } else if (panel_ctx->image_info.base_info.intf_type == HI_DRV_PANEL_INTFTYPE_VBONE) {
        if (spread >= HI_DRV_PANEL_VBO_SSFREQ_BUTT || spread < HI_DRV_PANEL_VBO_SSFREQ_46P875KHZ) {
            hi_log_err("parameter ERROR, VBONE correct range [2~10].\n");
            hi_err_print_u32(spread);
            return HI_FAILURE;
        } else {
            hi_log_warn("spread_freq success: spread_freq\n");
            hi_warn_print_str(spread_vbo_p[spread - HI_DRV_PANEL_VBO_SSFREQ_46P875KHZ]);
            panel_ctx->intf_ctx.intf_attr.vbo_spread_freq = (hi_drv_panel_vbo_ssfreq)spread;
        }
    } else {
        if (spread >= HI_DRV_TCON_SSFREQ_BUTT || spread < HI_DRV_TCON_SSFREQ_93P75KHZ) {
            hi_log_err("parameter ERROR, TCON correct range [2~15].\n");
            hi_err_print_u32(spread);
            return HI_FAILURE;
        } else {
            hi_log_warn("spread_freq success: spread_freq\n");
            hi_warn_print_str(spread_tcon_p[spread - HI_DRV_TCON_SSFREQ_93P75KHZ]);
            panel_ctx->image_info.tcon_info.ssc_freq = spread;
        }
    }
    hal_panel_set_spread_freq(panel_ctx->image_info.base_info.intf_type, spread);
#endif
    return HI_SUCCESS;
}
static hi_s32 drv_panel_proc_set_spead_ratio(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 spread_ratio;
    drv_panel_contex *panel_ctx = HI_NULL;
    drv_panel_get_panel_ctx(&panel_ctx);
    spread_ratio = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */
    if (spread_ratio > PANEL_SPREAD_RATIO_MAX) {
        hi_log_err("parameter ERROR, correct range [0~31].\n");
        hi_err_print_u32(spread_ratio);
        return HI_FAILURE;
    }
    if (panel_ctx->image_info.base_info.intf_type == HI_DRV_PANEL_INTFTYPE_LVDS) {
        hi_log_warn("spread_ratio success: spread_ratio=%d\n", spread_ratio);
        panel_ctx->intf_ctx.intf_attr.lvds_spread_ratio = spread_ratio;
    } else if (panel_ctx->image_info.base_info.intf_type == HI_DRV_PANEL_INTFTYPE_VBONE) {
        hi_log_warn("spread_ratio success: spread_ratio\n");
        hi_warn_print_u32(spread_ratio);
        panel_ctx->intf_ctx.intf_attr.vbo_spread_ratio = spread_ratio;
    } else {
        hi_log_warn("spread_ratio success: spread_ratio\n");
        hi_warn_print_u32(spread_ratio);
        panel_ctx->image_info.tcon_info.ssc_ratio = spread_ratio;
    }
    hal_panel_set_spread_ratio(spread_ratio);
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_phy_port_pn_swap(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
    hi_u32 value;
    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */

    hal_panel_set_phy_port_pn_swap(value);
    hi_log_warn("pn_swap success: value? swap : not swap\n");
    hi_warn_print_u32(value);
#endif
    return HI_SUCCESS;
}
static hi_s32 drv_panel_proc_set_tcon_force_sdlock(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 value;
    value = (hi_u32)(osal_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN)); /* l is index */
    if (value > PANEL_BOOL_MAX_VALUE) {
        hi_log_err("parameter ERROR, correct range [0~1].\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }
#ifdef PANEL_TMP
    hal_panel_set_tcon_force_sdlock((hi_bool)value);
#endif
    hi_log_warn("sdlock success: value? sdlock : not sdlock\n");
    hi_warn_print_u32(value);

    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_data_width(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP

#if PANEL_LDM_SUPPORT
#if PANEL_LDM_USE_COM_SPI
    hi_s32 ret;
    ret = drv_panel_com_spi_proc_set_data_width(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_com_spi_proc_set_data_width failed\n");
        return HI_FAILURE;
    }

#else
    hi_s32 ret;
    ret = drv_panel_ldm_spi_proc_set_data_width(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_data_width failed\n");
        return HI_FAILURE;
    }

#endif
#endif
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_clk_phase(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
#if PANEL_LDM_SUPPORT
#if PANEL_LDM_USE_COM_SPI
    hi_s32 ret;
    ret = drv_panel_com_spi_proc_set_clk_phase(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_com_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#else
    hi_s32 ret;
    ret = drv_panel_ldm_spi_proc_set_clk_phase(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }

#endif
#endif
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_clkfreq(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
#if PANEL_LDM_SUPPORT
#if PANEL_LDM_USE_COM_SPI
    hi_s32 ret;
    ret = drv_panel_com_spi_proc_set_clkfreq(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_com_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }

#else

    hi_s32 ret;
    ret = drv_panel_ldm_spi_proc_set_clkfreq(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }

#endif
#endif
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_panel_proc_set_clk_pol(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#ifdef PANEL_TMP
#if PANEL_LDM_SUPPORT
#if PANEL_LDM_USE_COM_SPI
    hi_s32 ret;
    ret = drv_panel_com_spi_proc_set_clk_pol(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_com_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }

#else

    hi_s32 ret;
    ret = drv_panel_ldm_spi_proc_set_clk_pol(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }

#endif
#endif
#endif
    return HI_SUCCESS;
}

#ifdef PANEL_TMP
static hi_s32 drv_panel_ldm_set_lcd_enable(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{

#if PANEL_LDM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_ldm_proc_set_lcd_enable(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_set_led_enable(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_ldm_proc_set_led_enable(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_set_dbg_level(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_ldm_proc_set_dbg_level(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_set_print_origion_data(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_ldm_proc_set_print_origion_data(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_set_print_map_data(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_ldm_proc_set_print_map_data(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_set_print_protocol_data(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_ldm_proc_set_print_protocol_data(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_set_print_data(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_ldm_proc_set_print_data(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_set_send_data(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_ldm_proc_set_send_data(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_set_send_test_data(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_ldm_proc_set_send_test_data(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_set_bl_level_increase(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_ldm_proc_set_bl_level_increase(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_set_light_change_speed(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_ldm_proc_set_light_change_speed(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_set_demo(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_ldm_proc_set_demo(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_set_map_fun(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_ldm_proc_set_map_fun(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_set_pq_map_enable(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_ldm_proc_set_pq_map_enable(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_set_dimension(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_ldm_proc_set_dimension(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_com_spi_test(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_USE_COM_SPI
    hi_s32 ret;
    ret = drv_panel_com_spi_send_read_test(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_spi_set_data_pol(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
#if PANEL_LDM_USE_COM_SPI
    return HI_SUCCESS;

#else
    hi_s32 ret;
    ret = drv_panel_ldm_spi_proc_set_data_pol(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_spi_set_vsync2_cs_time(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
#if PANEL_LDM_USE_COM_SPI
    return HI_SUCCESS;

#else
    hi_s32 ret;
    ret = drv_panel_ldm_spi_proc_set_vsync2_cs_time(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_spi_set_cs2_data_time(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
#if PANEL_LDM_USE_COM_SPI
    return HI_SUCCESS;

#else
    hi_s32 ret;
    ret = drv_panel_ldm_spi_proc_set_cs2_data_time(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_spi_set_data2_data_time(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
#if PANEL_LDM_USE_COM_SPI
    return HI_SUCCESS;

#else
    hi_s32 ret;
    ret = drv_panel_ldm_spi_proc_set_data2_data_time(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_ldm_spi_set_data2_cs_time(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_LDM_SUPPORT
#if PANEL_LDM_USE_COM_SPI
    return HI_SUCCESS;

#else
    hi_s32 ret;
    ret = drv_panel_ldm_spi_proc_set_data2_cs_time(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_bl_driver_osal_print_data(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_BLDRIVER_SUPPORT
    hi_s32 ret;
    ret = drv_panel_bl_driver_osal_proc_print_data(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_bl_driver_read_state(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_BLDRIVER_SUPPORT
    hi_s32 ret;
    ret = drv_panel_bl_driver_proc_read_state(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_set_proc_gradule_change(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_BLDRIVER_SUPPORT
    hi_s32 ret;
    ret = drv_panel_proc_set_ldm_gradule_change(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_pwm_set_enable(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_PWM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_pwm_proc_set_enable(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_pwm_set_dynamic_mode(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_PWM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_pwm_proc_set_pwm_attr(arg_p, arg1_p, private, DYNAMIC_MODE);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_pwm_set_duty(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_PWM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_pwm_proc_set_duty(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_pwm_set_freq(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_PWM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_pwm_proc_set_freq(arg_p, arg1_p, private);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_pwm_set_signal_invert(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_PWM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_pwm_proc_set_pwm_attr(arg_p, arg1_p, private, SIG_INV_MODE);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_panel_pwm_set_sync_whole_mode(hi_u32 arg_p,
    char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
#if PANEL_PWM_SUPPORT
    hi_s32 ret;
    ret = drv_panel_pwm_proc_set_pwm_attr(arg_p, arg1_p, private, SYNC_WHOLE_MODE);
    if (ret != HI_SUCCESS) {
        hi_log_err("drv_panel_ldm_spi_proc_set_clk_phase failed\n");
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}
#endif

static hi_s32 drv_panel_proc_help_(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    drv_panel_proc_help();
    return HI_SUCCESS;
}

static osal_proc_cmd g_st_proc_info[] = {
    /* debug timming part */
    { "help",        drv_panel_proc_help_ },
    { "dbgtimen",    drv_panel_proc_set_dbg_timming_enable },
    { "dbgtimreint", drv_panel_proc_set_dbg_timming_re_init },
    { "dbgtimuse",   drv_panel_proc_set_dbg_timming_use },
    { "dbgclk",      drv_panel_proc_set_dbg_timming_clk },
    { "dbghtotal",   drv_panel_proc_set_dbg_timming_htotal },
    { "dbgwidth",    drv_panel_proc_set_dbg_timming_width },
    { "dbghsfp",     drv_panel_proc_set_dbg_timming_hsync_fp },
    { "dbghswidth",  drv_panel_proc_set_dbg_timming_hsync_width },
    { "dbgvtotal",   drv_panel_proc_set_dbg_timming_vtotal },
    { "dbgheight",   drv_panel_proc_set_dbg_timming_height },
    { "dbgvsfp",     drv_panel_proc_set_dbg_timming_vsync_fp },
    { "dbgvswidth",  drv_panel_proc_set_dbg_timming_vsync_width },
    { "dbghsfpaut",  drv_panel_proc_set_dbg_hor_timming_auto },
    { "dbgvsfpaut",  drv_panel_proc_set_dbg_ver_timming_auto },

    /* debug VBO part */
    { "vtstmd",    drv_panel_proc_set_vbo_test_mode },
    { "vbyte",     drv_panel_proc_set_vbo_byte_num },
    { "vdatamod",  drv_panel_proc_set_vbo_data_mode },
    { "vlockndbg", drv_panel_proc_set_vbo_lockn_dbg },
    { "vbitwidth", drv_panel_proc_set_vbo_bit_width },
    { "colorbar",  drv_panel_proc_set_vbo_color_bar },
    { "hsyncinv",  drv_panel_proc_set_hsync_invert },
    { "vsyncinv",  drv_panel_proc_set_vsync_invert },
    { "deinv",     drv_panel_proc_set_de_invert },
    { "division",  drv_panel_proc_set_division_type },

    /* debug LVDS part */
    { "llinkmod",  drv_panel_proc_set_lvds_link_mode },
    { "ldatafmt",  drv_panel_proc_set_lvds_data_format },
    { "lbitwidth", drv_panel_proc_set_lvds_bitwidth },
    { "lsyncout",  drv_panel_proc_set_lvds_sync_out_enable },
    { "ldatainv",  drv_panel_proc_set_lvds_data_invert },
    { "llinkmap",  drv_panel_proc_set_lvds_link_map },

    /* debug TCON part */
    { "pnswap",    drv_panel_proc_set_phy_port_pn_swap },
    { "sdlock",    drv_panel_proc_set_tcon_force_sdlock },

    /* debug common part */
    { "fixrate",    drv_panel_proc_set_fix_rate },
    { "miscfhd4k",  drv_panel_proc_set_misc_fhd4_k },
    { "misc4k",     drv_panel_proc_set_misc4_k },
    { "misc",       drv_panel_proc_set_misc },
    { "power",      drv_panel_proc_set_panel_power },
    { "tcon",       drv_panel_proc_set_tcon_power },
    { "intf",       drv_panel_proc_set_intf_power },
    { "blenable",   drv_panel_proc_set_backlight_power },
    { "bllevel",    drv_panel_proc_set_backlight_level },
    { "current",    drv_panel_proc_set_drv_current },
    { "voltage",    drv_panel_proc_set_voltage },
    { "emp",        drv_panel_proc_set_emphasis },
    { "spreaden",   drv_panel_proc_set_spread_enable },
    { "sfreq",      drv_panel_proc_set_spead_freq },
    { "sratio",     drv_panel_proc_set_spead_ratio },
    { "dimen",      drv_panel_proc_set_dim_enable },
    { "doblyen",    drv_panel_proc_set_dobly_en_test },
    { "doblylevel", drv_panel_proc_set_dobly_level_test },
    { "tconbist",   drv_panel_proc_set_tconbist },

     /* debug LDM part */
    { "bitwidth",         drv_panel_proc_set_data_width },
    { "clkphase",         drv_panel_proc_set_clk_phase },
    { "clkfreq",          drv_panel_proc_set_clkfreq },
    { "nosigclkh",        drv_panel_proc_set_clk_pol },
#ifdef PANEL_TMP
    { "ldmlcden",         drv_panel_ldm_set_lcd_enable },
    { "ldmleden",         drv_panel_ldm_set_led_enable },
    { "dbglevel",         drv_panel_ldm_set_dbg_level },
    { "printorig",        drv_panel_ldm_set_print_origion_data },
    { "printmap",         drv_panel_ldm_set_print_map_data },
    { "printprot",        drv_panel_ldm_set_print_protocol_data },
    { "dimprint",         drv_panel_ldm_set_print_data },
    { "ldmsend",          drv_panel_ldm_set_send_data },
    { "testdata",         drv_panel_ldm_set_send_test_data },
    { "levelinctst",      drv_panel_ldm_set_bl_level_increase },
    { "lightchangespeed", drv_panel_ldm_set_light_change_speed },
    { "ldmdemo",          drv_panel_ldm_set_demo },
    { "ldmmapchose",      drv_panel_ldm_set_map_fun },
    { "pqmapen",          drv_panel_ldm_set_pq_map_enable },
    { "ldmdimension",     drv_panel_ldm_set_dimension },

    /* debug LDM with com spi part */
    { "sendread",         drv_panel_com_spi_test },
    /* debug LDM with ldm spi part */
    { "nosigdoh",         drv_panel_ldm_spi_set_data_pol },
    { "vsync2cst",        drv_panel_ldm_spi_set_vsync2_cs_time },
    { "cs2datat",         drv_panel_ldm_spi_set_cs2_data_time },
    { "data2datat",       drv_panel_ldm_spi_set_data2_data_time },
    { "data2cst",         drv_panel_ldm_spi_set_data2_cs_time },
    /* debug bl part */
    { "printdata",        drv_panel_bl_driver_osal_print_data },
    { "readstate",        drv_panel_bl_driver_read_state },
    { "gradulechange",    drv_panel_set_proc_gradule_change },
    /* debug pwm part */
    { "pwmen",            drv_panel_pwm_set_enable },
    { "pwmdymode",        drv_panel_pwm_set_dynamic_mode },
    { "pwmduty",          drv_panel_pwm_set_duty },
    { "pwmfreq",          drv_panel_pwm_set_freq },
    { "pwminvert",        drv_panel_pwm_set_signal_invert },
    { "syncwhole",        drv_panel_pwm_set_sync_whole_mode },
#endif
};

static hi_char *g_p_intf_type[] = { "LVDS", "VBO", "MLVDS", "EPI", "CEDS", "ISP", "USIT",
    "CSPI", "CMPI", "CHPI", "CHPI_H", "BUTT" };
static hi_char *g_p_3d_format[] = {
    "2D", "FPK", "SBSH", "TAB", "FS",
    "PR_LR", "SBSF", "LDEPT", "GDEPT", "PR_RL", "BUTT"
};
#ifdef PANEL_TMP
static hi_char *g_p_res_type[] = { "FHD", "UHD", "FHD4K30", "FHD4K60", "4K3060",
                                   "FHD4K3060", "8K", "8K4K3060", "5K2K", "BUTT" };
#endif
static hi_char *g_p_link_type[] = { "1_link",  "2_link",  "4_link",   "8_link",   "16_link", "BUTT" };
static hi_char *g_p_bit_width[] = { "8BIT",   "10BIT",  "12BIT",   "6BIT", "BUTT" };
#ifdef PANEL_TMP
static hi_char *g_p_vbo_mode[] = { "30B444", "36B444", "24B444",  "18B444",  "24B422", "20B422",    "16B422", "BUTT" };
#endif
static hi_char *g_p_vbo_byte[] = { "none",   "none",   "none",    "3_byte",   "4_byte",  "5_byte",     "BUTT" };
#ifdef PANEL_TMP
static hi_char *g_p_vbo_cur[] = { "200MV",  "250MV",  "300MV",   "350MV",   "400MV",
    "450MV", "500MV", "BUTT" };
static hi_char *g_p_vbo_emp[] = { "0DB", "1DB", "2DB", "3DB", "4DB", "5DB", "6DB",
    "7DB", "8DB", "9DB", "BUTT" };
#endif
static hi_char *g_p_lvds_fmt[] = { "vesa",   "jeida",  "FP",      "BUTT" };
static hi_char *g_p_lvds_cur[] = {
    "200MV", "250MV", "300MV", "350MV",
    "400MV", "450MV", "500MV", "BUTT" };
static hi_char *g_p_lvds_vol[] = {
    "400MV", "450MV", "500MV", "550MV", "600MV", "650MV", "700MV", "800MV",
    "900MV", "1000MV", "1100MV", "1150MV", "1200MV", "1250MV", "1300MV", "1400MV", "1500MV", "BUTT" };
static hi_char *g_p_lvds_link[] = { "pixel0", "pixel1", "pixel2", "pixel3", "BUTT" };
static hi_char *g_p_chg_mode[] = { "instant", "v_gradual", "h_gradual", "clk_gradual", "BUTT" };
static hi_char *g_p_lvds_sp_freq[] = { "none", "none", "93.75_k_hz", "62.5_k_hz", "46.87_k_hz", "37.5_k_hz", "BUTT" };
static hi_char *g_p_lvds_real_sp_freq[] = {
    "none", "none", "46.875_k_hz", "46.875_k_hz",
    "46.875_k_hz", "31.25_k_hz", "BUTT" };
static hi_char *g_p_vbo_sp_freq[] = { "none", "none", "46.87_k_hz", "31.25_k_hz",
                                      "23.43_k_hz", "18.75_k_hz", "93.75_k_hz",
                                      "62.5_k_hz", "37.5_k_hz", "26.786_k_hz", "20.833_k_hz", "BUTT" };
static hi_char *g_p_vbo_real_sp_freq[] = {
    "none", "none", "46.875_k_hz", "31.25_k_hz", "23.4375_k_hz", "18.75_k_hz",
    "46.875_k_hz", "46.875_k_hz", "31.25_k_hz", "23.4375_k_hz", "18.75_k_hz", "BUTT" };
static hi_char *g_p_tcon_sp_freq[] = {
    "none", "none", "93.75KHZ", "62.5KHZ", "46.875KHZ", "37.5KHZ", "31.25KHZ", "26.7857KHZ", "23.4375KHZ",
    "20.833KHZ", "18.75KHZ", "17.045KHZ", "15.625KHZ", "14.423KHZ", "13.393KHZ", "12.5KHZ", "BUTT" };
static hi_char *g_p_div_mode[] = { "one_oe", "one", "two", "four", "BUTT" };
static hi_char *g_p_flip_type[] = { "none", "FLIP", "MIRROR", "FLIP_MIRROR", "BUTT" };
#ifdef PANEL_TMP

static hi_char *g_p_fix_rate_type[] = {
    "none", "FHD50", "FHD60", "fhd_other", "UHD50", "UHD60", "uhd_other", "FHD48HZ",
    "UHD24HZ", "UHD25HZ", "UHD30HZ", "UHD48HZ", "BUTT"
};

static hi_char *g_p_tcon_port[] = { "1_port", "2_port", "4_port", "6_port", "8_port", "12_port", "BUTT" };
static hi_char *g_p_tcon_pair[] = { "1_pair", "2_pair", "3_pair", "6_pair", "BUTT" };
#endif
static hi_char *g_p_emphasis[] = {
    "0DB", "1DB", "2DB", "3DB", "4DB", "5DB",
    "6DB", "7DB", "8DB", "9DB", "BUTT"
};
static hi_char *g_p_com_voltage[] = {
    "400MV", "450MV", "500MV", "550MV", "600MV", "650MV", "700MV", "800MV",
    "900MV", "1000MV", "1100MV", "1150MV", "1200MV", "1250MV", "1300MV", "1400MV", "1500MV", "BUTT"
};
static hi_char *g_p_drv_current[] = { "200MV", "250MV", "300MV", "350MV", "400MV", "450MV", "500MV", "BUTT" };
static hi_char *g_pchar_bool[] = { "FALSE", "TRUE" };

static hi_void drv_panel_proc_info_print(hi_void *s, hi_drv_panel_info *base_info, drv_panel_contex *panel_ctx)
{
    osal_proc_print(s, "\n============================================panel proc info"
        "=============================================\n");
    osal_proc_print(s, "%-12s:%-90s|\n",
        "panel_name", base_info->name);
    osal_proc_print(s, "%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|\n",
        "cut_idx", panel_ctx->image_info.cur_index,
        "tot_num", panel_ctx->image_info.total_num,
        "res_width", base_info->width,
        "res_high", base_info->height);
    osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|\n",
        "intf_type", g_p_intf_type[base_info->intf_type],
        "3d_type", g_p_3d_format[base_info->type_3d],
        "res_type", "",
        "max_rate", "");
    osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|\n",
        "link_info", "",
        "bit_width", g_p_bit_width[base_info->data_bit_depth],
        "sup_other", "",
        "", "");
    osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|\n",
        "div_type", g_p_div_mode[base_info->division_type],
        "fix_type", "",
        "flip_type", "",
        "", "");
}

static hi_void drv_panel_proc_intf_info_print(hi_void *s, hi_drv_panel_info *base_info)
{
    osal_proc_print(s, "-----------------------------------------------intf info"
               "------------------------------------------------\n");
    if (base_info->intf_type == HI_PANEL_INTF_TYPE_VBONE) {
#ifdef PANEL_TMP
        osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|\n",
            "lr_swap", g_pchar_bool[base_info->vbo_attr.group_swap],
            "l_swap", g_pchar_bool[base_info->vbo_attr.left_internal_swap],
            "r_swap", g_pchar_bool[base_info->vbo_attr.right_internal_swap],
            "bit0_at_h", g_pchar_bool[base_info->vbo_attr.bit0_at_high_bit]);
        osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|\n",
            "data_mode", g_p_vbo_mode[base_info->vbo_attr.data_mode],
            "byte", g_p_vbo_byte[base_info->vbo_attr.vbo_byte_num],
            "current", g_p_vbo_cur[base_info->vbo_attr.vbo_current],
            "emphasis", g_p_vbo_emp[base_info->vbo_attr.vbo_emphasis]);
#endif
    } else if (base_info->intf_type == HI_PANEL_INTF_TYPE_LVDS) {
        osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|\n",
            "lvds_fmt", g_p_lvds_fmt[base_info->lvds_attr.lvds_fmt],
            "current", g_p_lvds_cur[base_info->signal_attr.drv_current],
            "com_vol", g_p_lvds_vol[base_info->signal_attr.com_voltage],
            "", "");
        osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|\n",
            "link1", g_p_lvds_link[base_info->lvds_attr.lvds_link.link_map0],
            "link2", g_p_lvds_link[base_info->lvds_attr.lvds_link.link_map1]);
    } else {
    }
}

#ifdef PANEL_TMP
static hi_void drv_panel_proc_other_tim_info_print(hi_void *s, hi_drv_panel_info *base_info)
{
    osal_proc_print(s, "%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|%-12s:%-12s|\n",
        "other_ht", base_info->timing_other.htotal,
        "other_vt", base_info->timing_other.vtotal,
        "other_clk", base_info->timing_other.pixel_clk,
        "", "");
    osal_proc_print(s, "%-12s:%-12d|%-12s:%-12d|%-12s:%-12s|%-10s:%-12s|\n",
        "other_hfp", base_info->timing_other.hsync_fp,
        "other_vfp", base_info->timing_other.vsync_fp,
        "", "",
        "", "");
}
#endif

static hi_void drv_panel_proc_base_tim_info_print(hi_void *s, hi_drv_panel_info *base_info)
{
    osal_proc_print(s, "%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|%-12s:%-12s|\n",
        "50HT", base_info->timing_50hz.htotal,
        "50VT", base_info->timing_50hz.vtotal,
        "50_clk", base_info->timing_50hz.pixel_clk,
        "", "");
    osal_proc_print(s, "%-12s:%-12d|%-12s:%-12d|%-12s:%-12s|%-12s:%-12s|\n",
        "50HFP", base_info->timing_50hz.hsync_fp,
        "50VFP", base_info->timing_50hz.vsync_fp,
        "", "",
        "", "");
    osal_proc_print(s, "%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|%-12s:%-12s|\n",
        "60HT", base_info->timing_60hz.htotal,
        "60VT", base_info->timing_60hz.vtotal,
        "60_clk", base_info->timing_60hz.pixel_clk,
        "", "");
    osal_proc_print(s, "%-12s:%-12d|%-12s:%-12d|%-12s:%-12s|%-12s:%-12s|\n",
        "60HFP", base_info->timing_60hz.hsync_fp,
        "60VFP", base_info->timing_60hz.vsync_fp,
        "", "",
        "", "");
#ifdef PANEL_TMP
    osal_proc_print(s, "%-12s:%-12d|%-12s:%-12d|%-12s:%-12s|%-12s:%-12s|\n",
        "hsync_w", base_info->hsync_width,
        "vsync_w", base_info->vsync_width,
        "", "",
        "", "");
    osal_proc_print(s, "%-12s:%-12s|%-12s:%-12d|%-12s:%-12s|%-12s:%-12s|\n",
        "tim_chg_mod", g_p_chg_mode[base_info->tim_change_type],
        "tim_chg_stp", base_info->change_step,
        "hsync_out", g_pchar_bool[base_info->h_sync_output],
        "vsync_out", g_pchar_bool[base_info->v_sync_output]);
    osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|\n",
        "hsyn_neg", g_pchar_bool[base_info->h_sync_negative],
        "vsyn_neg", g_pchar_bool[base_info->v_sync_negative],
        "de_neg", g_pchar_bool[base_info->de_negative],
        "", "");
    osal_proc_print(s, "%-12s:%-12s|%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|\n",
        "pwm_postive", g_pchar_bool[base_info->pwm_postive],
        "other_freq", base_info->dimming_freq_other,
        "50_freq", base_info->dimming_freq50hz,
        "60_freq", base_info->dimming_freq60hz);
    osal_proc_print(s, "%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|\n",
        "int_on_delay", base_info->intf_signal_on_delay,
        "bl_on_delay", base_info->bl_on_delay,
        "int_of_delay", base_info->bl_off_delay,
        "tco_of_delay", base_info->intf_signal_off_delay);
#endif
}

static hi_void drv_panel_proc_power_info_print(hi_void *s, drv_panel_contex *panel_ctx)
{
    osal_proc_print(s, "-------------------------------------------panel power stats"
               "--------------------------------------------\n");

    osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|\n",
        "tcon_pow_on", g_pchar_bool[panel_ctx->power_state.tcon_enable],
        "intf_enable", g_pchar_bool[panel_ctx->power_state.intf_enable],
        "bl_enable", g_pchar_bool[panel_ctx->power_state.back_light_enable],
        "power_on", g_pchar_bool[panel_ctx->power_state.power_on]);
}

static hi_void drv_panel_proc_intf_attr_print(hi_void *s,
    hi_drv_panel_info *base_info, drv_panel_contex *panel_ctx)
{
    osal_proc_print(s, "---------------------------------------------intf attr info"
        "---------------------------------------------\n");
    if (base_info->intf_type == HI_PANEL_INTF_TYPE_LVDS) {
        osal_proc_print(s, "%-12s:%-12s|%-12s:%-12d|%-12s:%-12s|%-12s:%-12s|\n",
            "spread_en", g_pchar_bool[panel_ctx->intf_ctx.intf_attr.spread_enable],
            "sp_ratio", panel_ctx->intf_ctx.intf_attr.lvds_spread_ratio,
            "sp_freq", g_p_lvds_sp_freq[panel_ctx->intf_ctx.intf_attr.lvds_spread_freq],
            "real_sp_freq", g_p_lvds_real_sp_freq[panel_ctx->intf_ctx.intf_attr.lvds_spread_freq]);
        osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|\n",
            "current", g_p_drv_current[panel_ctx->combo_attr.au8_drv_current[0]], /* 0 is index */
            "com_voltage", g_p_com_voltage[panel_ctx->combo_attr.au8_com_voltage[0]], "", "", "", ""); /* 0 is index */
    } else if (base_info->intf_type == HI_PANEL_INTF_TYPE_VBONE) {
        osal_proc_print(s, "%-12s:%-12s|%-12s:%-12d|%-12s:%-12s|%-12s:%-12s|\n",
            "spread_en", g_pchar_bool[panel_ctx->intf_ctx.intf_attr.spread_enable],
            "sp_ratio", panel_ctx->intf_ctx.intf_attr.vbo_spread_ratio,
            "sp_freq", g_p_vbo_sp_freq[panel_ctx->intf_ctx.intf_attr.vbo_spread_freq],
            "real_sp_freq", g_p_vbo_real_sp_freq[panel_ctx->intf_ctx.intf_attr.vbo_spread_freq]);
        osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12d|\n",
            "current", g_p_drv_current[panel_ctx->combo_attr.au8_drv_current[0]], /* 0 is index */
            "emphasis", g_p_emphasis[panel_ctx->combo_attr.au8_pre_emphasis[0]], /* 0 is index */
            "cdr_stat", panel_ctx->intf_ctx.vbo_lock_stat.cdr_lock_state ? "lock" : "unlock",
            "cdr_unlk_cnt", panel_ctx->intf_ctx.vbo_lock_stat.cdr_un_lock_cnt);
    } else {
        osal_proc_print(s, "%-12s:%-12s|%-12s:%-12d|%-12s:%-12s|%-12s:%-12s|\n",
            "spread_en", g_pchar_bool[panel_ctx->image_info.tcon_info.ssc_en],
            "sp_ratio", panel_ctx->image_info.tcon_info.ssc_ratio,
            "sp_freq", g_p_tcon_sp_freq[panel_ctx->image_info.tcon_info.ssc_freq],
            "current", g_p_drv_current[panel_ctx->combo_attr.au8_drv_current[0]]); /* 0 is index */
        osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|\n",
            "emphasis", g_p_emphasis[panel_ctx->combo_attr.au8_pre_emphasis[0]], /* 0 is index */
            "com_voltage", g_p_com_voltage[panel_ctx->combo_attr.au8_com_voltage[0]], "", "", "", ""); /* 0 is index */
    }

    osal_proc_print(s, "%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|%-12s:%-12s|\n",
        "bl_min", panel_ctx->image_info.def_bl_info.min_level[0], /* 0 is index */
        "bl_max", panel_ctx->image_info.def_bl_info.max_level[0], /* 0 is index */
        "bl_def", panel_ctx->image_info.def_bl_info.def_level[0], "", ""); /* 0 is index */
}

static hi_void drv_panel_proc_cfg_info_print(hi_void *s,
    hi_drv_panel_info *base_info, drv_panel_contex *panel_ctx)
{
    hi_u32 val;

    osal_proc_print(s, "---------------------------------------------panel cfg info"
        "---------------------------------------------\n");
    osal_proc_print(s, "%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|%-12s:%-12s|\n",
        "width", panel_ctx->panel_cfg.width,
        "height", panel_ctx->panel_cfg.height,
        "frm_rate", get_out_frame_rate(panel_ctx->panel_cfg.frame_rate),
        "3d_type", g_p_3d_format[panel_ctx->panel_cfg.type_3d]);
    osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|\n",
        "intf_type", g_p_intf_type[panel_ctx->panel_cfg.intf_type],
        "link_type", g_p_link_type[panel_ctx->panel_cfg.link_type],
        "byte", g_p_vbo_byte[panel_ctx->panel_cfg.byte_num], "", "");
    osal_proc_print(s, "%-12s:%-12s|%-12s:%-12d|%-12s:%-12s|%-12s:%-12s|\n",
        "tim_chg_mod", g_p_chg_mode[panel_ctx->panel_cfg.timing_change_info.type],
        "tim_chg_stp", panel_ctx->panel_cfg.timing_change_info.step,
        "div_type", g_p_div_mode[panel_ctx->panel_cfg.division_type],
        "flip_type", g_p_flip_type[panel_ctx->panel_cfg.flip_type]);
    osal_proc_print(s, "%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|\n",
        "htotal", panel_ctx->panel_cfg.timing.htotal,
        "vtotal", panel_ctx->panel_cfg.timing.vtotal,
        "clk", panel_ctx->panel_cfg.timing.pixel_clk,
        "hsync_fp", panel_ctx->panel_cfg.timing.hsync_fp);
    val = panel_ctx->panel_cfg.timing.htotal - panel_ctx->panel_cfg.width
             - panel_ctx->panel_cfg.timing.hsync_fp - panel_ctx->panel_cfg.timing.hsync_width;
    osal_proc_print(s, "%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|\n",
        "hsync_w", panel_ctx->panel_cfg.timing.hsync_width,
        "hsync_bp", val,
        "isr_start", panel_ctx->isr_start_line,
        "isr_finish", panel_ctx->isr_finish_line);
    val = panel_ctx->panel_cfg.timing.vtotal - panel_ctx->panel_cfg.height
             - panel_ctx->panel_cfg.timing.vsync_fp - panel_ctx->panel_cfg.timing.vsync_width;
    osal_proc_print(s, "%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|\n",
        "vsync_fp", panel_ctx->panel_cfg.timing.vsync_fp,
        "vsync_w", panel_ctx->panel_cfg.timing.vsync_width,
        "vsync_bp", val,
        "fun_call_cnt", panel_ctx->cfg_state.func_call_cnt);
    if ((base_info->intf_type >= HI_PANEL_INTF_TYPE_MINLVDS)
     && (base_info->intf_type <= HI_PANEL_INTF_TYPE_CHPI)) {
        osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|\n",
            "port", "",
            "pair", "", "", "", "", "");
    }
}

static hi_void drv_panel_proc_disp_expect_info_print(hi_void *s, drv_panel_contex *panel_ctx)
{
    osal_proc_print(s, "--------------------------------------------disp expect info"
        "--------------------------------------------\n");
    osal_proc_print(s, "%-12s:%-6d%-6d|%-12s:%-12d|%-12s:%-12d|%-12s:%-12d|\n",
        "res(W/H)", panel_ctx->disp_expect_info.disp_expect_width,
        panel_ctx->disp_expect_info.disp_expect_height,
        "frm_rate", get_out_frame_rate(panel_ctx->disp_expect_info.src_frm_rate),
        "dolby", panel_ctx->disp_expect_info.dolby_vison,
        "dolby_level", panel_ctx->disp_expect_info.dolby_expect_back_light);
}

hi_s32 drv_panel_proc_read(hi_void *s, hi_void *data)
{
    hi_s32 ret;

    drv_panel_contex *panel_ctx = HI_NULL;
    hi_drv_panel_info *base_info = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx);

    base_info = &panel_ctx->image_info.base_info;
    ret = down_interruptible(&g_panel_mutex);

    drv_panel_proc_info_print(s, base_info, panel_ctx);
    drv_panel_proc_intf_info_print(s, base_info);
#ifdef PANEL_TMP
    if (base_info->support_other) {
        drv_panel_proc_other_tim_info_print(s, base_info);
    }
#endif
    drv_panel_proc_base_tim_info_print(s, base_info);
    drv_panel_proc_power_info_print(s, panel_ctx);
    drv_panel_proc_intf_attr_print(s, base_info, panel_ctx);
#ifdef PANEL_TMP
    drv_panel_pwm_proc_read(s);
#endif
    drv_panel_proc_cfg_info_print(s, base_info, panel_ctx);
    drv_panel_proc_disp_expect_info_print(s, panel_ctx);
#ifdef PANEL_TMP
    if (panel_ctx->image_info.advance_info.localdiming_support) {
#if PANEL_LDM_SUPPORT
        drv_panel_ldm_proc_read(s);
#if PANEL_LDM_USE_COM_SPI
        drv_panel_com_spi_proc_read();
#else
        drv_panel_ldm_spi_proc_read(s);
#endif
        if (!PANEL_LDM_MCU_EXIST) {
            drv_panel_bl_driver_proc_read(s);
        }
#endif
    } else {
        drv_panel_0d_dim_proc_read(s);
    }
#endif
    osal_proc_print(s, "--------------------------------------panel commit time "
        "%s-----------------------------\n", g_panel_date);

    ret = HI_SUCCESS;
    up(&g_panel_mutex);

    return ret;
}

hi_s32 drv_panel_proc_add(hi_void)
{
    osal_proc_entry *panel_proc_entry;

    panel_proc_entry = osal_proc_add("panel", strlen("panel"));
    if (panel_proc_entry == HI_NULL) {
        hi_log_err("panel add proc failed!\n");
        return HI_FAILURE;
    }

    panel_proc_entry->read = drv_panel_proc_read;
    panel_proc_entry->cmd_cnt = sizeof(g_st_proc_info) / sizeof(osal_proc_cmd);
    panel_proc_entry->cmd_list = g_st_proc_info;
    panel_proc_entry->private = NULL;

    return HI_SUCCESS;
}

hi_void drv_panel_proc_del(hi_void)
{
    osal_proc_remove("panel", strlen("panel"));
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
