/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel module
* Author: sdk
* Create: 2019-11-20
*/

#ifndef __DISP_PLATFORM_BOOT__
#include "hi_drv_sys.h"
#endif

#include "drv_panel.h"
#include "drv_panel_power.h"
#include "drv_panel_param.h"
#include "drv_panel_vbo.h"
#include "drv_panel_pwm.h"
#include "drv_panel_0ddim.h"
#include "drv_panel_combophy.h"
#include "hal_panel_combophy.h"
#include "hal_panel_comm.h"
#include "hal_panel_vbo.h"
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static hi_s32 drv_panel_parse_panel_img(drv_panel_image *image_info_p)
{
    image_info_p->cur_index = DEFAULT_PANEL_INDEX;
    drv_panel_get_default_panel_info(image_info_p->cur_index,
                                     &image_info_p->total_num,
                                     &image_info_p->base_info);
    image_info_p->def_bl_info.max_level[0] = PWM_RANGE_MAX;
    image_info_p->def_bl_info.min_level[0] = PWM_RANGE_MIN;
    image_info_p->def_bl_info.def_level[0] = PWM_DEFAULT_LEVEL;

    return HI_SUCCESS;
}

static hi_s32 drv_panel_get_image_info(drv_panel_image *image_info_p)
{
    hi_s32 ret;

    panel_check_func_ret(drv_panel_parse_panel_img(image_info_p), drv_panel_parse_panel_img, ret);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void drv_panel_update_intf_ctx(hi_drv_panel_info *base_info, drv_panel_contex *panel_ctx_p)
{
    drv_panel_intf_attr *intf_ctx_p = &(panel_ctx_p->intf_ctx);

    intf_ctx_p->intf_type = base_info->intf_type;
    intf_ctx_p->bit_width = base_info->data_bit_depth;

    if (base_info->intf_type == HI_PANEL_INTF_TYPE_VBONE) {
        /* WORKING: drv_panel_update_vbo_attr */
        drv_panel_update_complete_vbo_attr(base_info, &intf_ctx_p->vbo_comp_attr);
    }
}

static hi_void drv_panel_update_frm_rate_info(hi_drv_panel_info *base_info, drv_panel_frame_rate_info *frm_rate_info)
{
    frm_rate_info->frm_rate = base_info->timing_60hz.pixel_clk /
                              base_info->timing_60hz.htotal /
                              base_info->timing_60hz.vtotal;
    frm_rate_info->timing_p = &base_info->timing_60hz;
    frm_rate_info->dim_freq = base_info->backlight_freq_60hz;
}

static hi_void drv_panel_updata_panel_cfg_info(drv_panel_contex *panel_ctx_p, hi_drv_panel_cfg *panel_cfg_p)
{
    hi_drv_panel_info *base_info = &panel_ctx_p->image_info.base_info;

    panel_cfg_p->width = base_info->width;
    panel_cfg_p->height = base_info->height;
    panel_cfg_p->frame_rate = base_info->timing_60hz.pixel_clk /
                              base_info->timing_60hz.htotal /
                              base_info->timing_60hz.vtotal * 1000; /* 1000 times */
    /* WORKING: vrr_support */
    panel_cfg_p->type_3d = base_info->type_3d;
    if (memcpy_s(&panel_cfg_p->timing, sizeof(panel_cfg_p->timing), &base_info->timing_60hz,
        sizeof(base_info->timing_60hz)) != EOK) {
        hi_log_err("panel cfg memcpy timing failed.\n");
        return;
    }

    /* WORKING: timing_change_info */
    if (memcpy_s(&panel_cfg_p->timing_sync_info, sizeof(panel_cfg_p->timing_sync_info), &base_info->timing_sync_info,
        sizeof(base_info->timing_sync_info)) != EOK) {
        hi_log_err("panel cfg memcpy timing_sync_info failed.\n");
        return;
    }

    panel_cfg_p->intf_type = base_info->intf_type;
    /* WORKING: byte_num */
    /* WORKING: link_type */
    /* WORKING: flip_type */
    panel_cfg_p->division_type = base_info->division_type;
    if (memcpy_s(&panel_cfg_p->pair_info, sizeof(panel_cfg_p->pair_info), &base_info->pair_info,
        sizeof(base_info->pair_info)) != EOK) {
        hi_log_err("panel cfg memcpy timing_sync_info failed.\n");
        return;
    }
    panel_cfg_p->bit_width = base_info->data_bit_depth;

#ifdef PANEL_TMP
    printk("panel_config.width = %d\n", panel_cfg_p->width);
    printk("panel_config.height = %d\n", panel_cfg_p->height);
    printk("panel_config.frame_rate = %d\n", panel_cfg_p->frame_rate);
    printk("panel_config.timing.pixel_clk = %d\n", panel_cfg_p->timing.pixel_clk);
    printk("panel_config.timing.htotal = %d\n", panel_cfg_p->timing.htotal);
    printk("panel_config.timing.hsync_fp = %d\n", panel_cfg_p->timing.hsync_fp);
    printk("panel_config.timing.hsync_width = %d\n", panel_cfg_p->timing.hsync_width);
    printk("panel_config.timing.vtotal = %d\n", panel_cfg_p->timing.vtotal);
    printk("panel_config.timing.vsync_fp = %d\n", panel_cfg_p->timing.vsync_fp);
#endif

    return;
}

static hi_void drv_panel_perpare_pwm_attr(hi_u32 dim_freq, drv_panel_image *image_info_p, drv_panel_pwm_attr *attr_p)
{
    hi_bool multiple_50hz;
    hi_bool multiple_60hz;

    /* check frame_freq is the multiple of frame_rate */
    multiple_50hz = multiple_check(image_info_p->base_info.backlight_freq_50hz,
                                   DRV_PANEL_FRAME_RATE_50HZ / OUT_FRAME_RATE_PRECISION);

    multiple_60hz = multiple_check(image_info_p->base_info.backlight_freq_60hz,
                                   DRV_PANEL_FRAME_RATE_60HZ / OUT_FRAME_RATE_PRECISION);

    attr_p->dynamic_mode = HI_FALSE;
    attr_p->pwm_duty = image_info_p->def_bl_info.def_level[0];
    attr_p->pwm_freq = dim_freq;
    attr_p->vsync_raise = HI_TRUE;
    attr_p->enable = HI_TRUE;
    /* WORKING: pwm_postive */
    attr_p->sync_whole_mode = HI_TRUE;

    if (multiple_50hz && multiple_60hz) {
        attr_p->sync_whole_mode = HI_FALSE;
    }
}

static hi_s32 drv_panel_update_panel_ctx(drv_panel_contex *panel_ctx_p)
{
    hi_s32 ret;

    hi_drv_panel_pwm_type pwm_type;
    drv_panel_pwm_attr attr[HI_DRV_PANEL_PWM_TYPE_BUTT];
    hi_drv_panel_info *base_info = &(panel_ctx_p->image_info.base_info);
    drv_panel_def_bl_info *def_bl_info_p = &(panel_ctx_p->image_info.def_bl_info);
    drv_panel_frame_rate_info *frm_rate_info_p = &(panel_ctx_p->frm_rate_info);

    drv_panel_update_intf_ctx(base_info, panel_ctx_p);
    drv_panel_update_frm_rate_info(base_info, frm_rate_info_p);
    drv_panel_updata_panel_cfg_info(panel_ctx_p, &panel_ctx_p->panel_cfg);
    drv_panel_set_0d_dim_init(def_bl_info_p->def_level[0]);

    for (pwm_type = HI_DRV_PANEL_PWM_TYPE_PWM; pwm_type <= HI_DRV_PANEL_PWM_TYPE_PWM1D3; pwm_type++) {
        drv_panel_perpare_pwm_attr(frm_rate_info_p->dim_freq, &panel_ctx_p->image_info, &attr[pwm_type]);
        panel_check_func_ret(drv_panel_pwm_init(pwm_type, &attr[pwm_type]), drv_panel_pwm_init, ret);
        if (ret != HI_SUCCESS) {
            return HI_FAILURE;
        }
    }

#ifdef PANEL_TMP
    /* 6.updata 3D info */
    if (base_info->disp_3d_type == HI_DRV_DISP_STEREO_FIELD_ALTE) {
        drv_panel_init3_d_fs_signal_info(&panel_ctx_p->st3_d_fs_sig_info);
    }

    /* 7.power on function and common spi cs use gpio, so get struct function first */
    panel_check_func_ret(drv_panel_get_gpio_function(), drv_panel_get_gpio_function);
    panel_check_func_ret(drv_panel_get_pq_export_function(), drv_panel_get_pq_export_function);
#endif

    return HI_SUCCESS;
}

hi_s32 drv_panel_init_ctx(drv_panel_contex *panel_ctx_p)
{
    hi_s32 ret;

    ret = drv_panel_get_image_info(&panel_ctx_p->image_info);
    if (ret != HI_SUCCESS) {
        hi_log_err("PANEL drv_get_image_info failed!\n");
        return HI_FAILURE;
    }

    ret = drv_panel_update_panel_ctx(panel_ctx_p);
    if (ret != HI_SUCCESS) {
        hi_log_err("PANEL drv_update_panel_ctx failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_panel_set_intf_cfg(drv_panel_contex *panel_ctx_p)
{
    hi_drv_panel_info *base_info = &panel_ctx_p->image_info.base_info;

    if (panel_ctx_p->intf_ctx.intf_type >= HI_PANEL_INTF_TYPE_MAX) {
        hi_log_err("not support interface type!\n");
        return HI_FAILURE;
    }

    if (panel_ctx_p->intf_ctx.intf_type == HI_PANEL_INTF_TYPE_VBONE) {
        drv_panel_set_vbo_cfg(&panel_ctx_p->intf_ctx.vbo_comp_attr, &panel_ctx_p->combo_attr);
    }

    drv_panel_combo_phy_digit_cfg(base_info);
    drv_panel_combo_phy_clk_cfg(panel_ctx_p);

    return HI_SUCCESS;
}

static hi_s32 drv_panel_set_crg_cfg(hi_void)
{
    drv_panel_contex *panel_ctx_p = HI_NULL;
    hi_drv_panel_intf_type panel_intf_type;

    drv_panel_get_panel_ctx(&panel_ctx_p);

    if (panel_ctx_p->image_info.base_info.intf_type >= HI_PANEL_INTF_TYPE_MAX) {
        hi_log_err("intf type is not correct\n");
        return HI_FAILURE;
    }

    panel_intf_type = panel_ctx_p->image_info.base_info.intf_type;

    if (panel_intf_type == HI_PANEL_INTF_TYPE_VBONE) {
        hal_panel_set_vbo_clk();
    }

    return HI_SUCCESS;
}

hi_s32 drv_panel_config_hardware(drv_panel_contex *panel_ctx)
{
    hi_s32 ret;
    hi_drv_panel_info *base_info = &panel_ctx->image_info.base_info;
    hi_drv_panel_pwm_type pwm_type;

    panel_check_func_ret(panel_reg_init(), panel_reg_init, ret);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    hal_panel_set_ioshare();
    hal_panel_set_lockn();
    hal_panel_set_htpdn();
    drv_panel_set_combo_phy_reset(HI_TRUE);

    panel_check_func_ret(drv_panel_set_intf_cfg(panel_ctx), drv_panel_set_intf_cfg, ret);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    /* WORKING: drv_panel_set_chn_spread */
    for (pwm_type = HI_DRV_PANEL_PWM_TYPE_PWM; pwm_type <= HI_DRV_PANEL_PWM_TYPE_PWM1D3; pwm_type++) {
        panel_check_func_ret(drv_panel_set_pwm_cfg(pwm_type), drv_panel_set_pwm_cfg, ret);
        if (ret != HI_SUCCESS) {
            return HI_FAILURE;
        }
    }
    /* WORKING: drv_panel_get_panel_bl_info */
    /* WORKING: LDM */
    panel_check_func_ret(drv_panel_set_0d_dim_cfg(base_info->width, base_info->height), drv_panel_set_0d_dim_cfg, ret);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    /* WORKING: drv_panel_pq_set_backlight_info */
    panel_check_func_ret(drv_panel_set_crg_cfg(), drv_panel_set_crg_cfg, ret);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    panel_check_func_ret(drv_panel_set_intf_power(base_info->intf_type, HI_TRUE), drv_panel_set_intf_power, ret);

    return HI_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

