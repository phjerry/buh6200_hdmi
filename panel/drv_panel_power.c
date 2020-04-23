/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel power module
* Author: sdk
* Create: 2019-04-03
*/

#include "drv_panel_define.h"
#include "drv_panel_adapter.h"
#include "drv_panel_vbo.h"
#include "drv_panel.h"
#include "drv_panel_combophy.h"
#include "hal_panel_combophy.h"
#include "hal_panel_vbo.h"

#ifdef __DISP_PLATFORM_BOOT__
#include <common.h>
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/************************************************************************************
                                panel power function
************************************************************************************/
hi_s32 drv_panel_set_panel_tcon_power(hi_bool power_on)
{
    hi_s32 ret = drv_panel_set_gpio_output_vol(((0 * EVERY_GPIO_GROUP_8_BITS) + 6), /* 0,6 is default param */
        power_on ? HI_TRUE : HI_FALSE);

    return ret;
}

hi_s32 drv_panel_pull_gpio(panel_advance_info *advance_info_p)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 gpio_num;
    hi_u32 gpio_temp = 0;
    hi_u8 u8_gpio_group = 0;
    hi_u8 u8_gpio_bit = 0;
    hi_u8 u8_gpio_state = 0;

    if (advance_info_p->gpio_info.gpio_num == 0) {
        return ret;
    }

    gpio_num = advance_info_p->gpio_info.gpio_num;
    for (gpio_temp = 0; gpio_temp < gpio_num; gpio_temp++) {
        u8_gpio_group = advance_info_p->gpio_info.gpio_data[gpio_temp][0]; /* 0:GPIO NUM0 */
        u8_gpio_bit = advance_info_p->gpio_info.gpio_data[gpio_temp][1];   /* 1:GPIO NUM1 */
        u8_gpio_state = advance_info_p->gpio_info.gpio_data[gpio_temp][2]; /* 2:GPIO NUM2 */

        ret = drv_panel_set_gpio_output_vol((hi_u32)((u8_gpio_group * EVERY_GPIO_GROUP_8_BITS) + u8_gpio_bit),
                                            (hi_bool)u8_gpio_state);
    }

    return ret;
}

hi_s32 drv_panel_set_panel_bl_power(hi_bool ldm_scene, hi_bool power_on)
{
    hi_log_info("---set backlight on :%d, ldm_scene:%d---\n", power_on, ldm_scene);

#ifdef PANEL_LDM_SUPPORT
    if (ldm_scene && !PANEL_LDM_MCU_EXIST) {
        if (power_on) {
            /* only 7027 bl_driver need set GPIO status, before cfg */
            panel_drv_set_bl_driver_gpio_status();

            panel_drv_set_gpio_output_vol(HI_BOARD_PANEL_BL_POWER_GPIONUM,
                                          power_on ? HI_BOARD_PANEL_BL_POWER_ON : HI_BOARD_PANEL_BL_POWER_OFF);

            panel_drv_set_bl_driver_cfg((hi_void *)&ldm_scene);

            panel_drv_set_ldm_gradule_change(PANEL_LDM_BLDRIVER_DEF_LEVEL);
            panel_msleep(PANEL_MSLEEP_20);
            panel_drv_set_bl_driver_power(HI_TRUE);
        } else {
            panel_drv_set_gpio_output_vol(HI_BOARD_PANEL_BL_POWER_GPIONUM,
                                          power_on ? HI_BOARD_PANEL_BL_POWER_ON : HI_BOARD_PANEL_BL_POWER_OFF);

            panel_drv_set_bl_driver_power(HI_FALSE);
        }
    } else {
#endif

        drv_panel_set_gpio_output_vol(((0 * EVERY_GPIO_GROUP_8_BITS) + 6), /* 0,6 is default param */
                                      power_on ? HI_TRUE : HI_FALSE);
#ifdef PANEL_LDM_SUPPORT
    }
#endif

    return HI_SUCCESS;
}

hi_s32 drv_panel_get_panel_tcon_power(hi_bool *power_on)
{
    return drv_panel_get_gpio_output_vol(((0 * EVERY_GPIO_GROUP_8_BITS) + 6), power_on); /* 0,6 is default param */
}

hi_s32 drv_panel_get_panel_bl_power(hi_bool *power_on_p)
{
    hi_bool power_on = HI_FALSE;
    hi_s32 ret = drv_panel_get_gpio_output_vol(((0 * EVERY_GPIO_GROUP_8_BITS) + 6), &power_on); /* 0,6:default param */
    *power_on_p = power_on ? HI_FALSE : HI_TRUE;
    return ret;
}

hi_s32 drv_panel_custom_send_panel_i2c_timer(hi_void *arg)
{
    drv_panel_contex *panel_ctx_p = HI_NULL;
    drv_panel_get_panel_ctx(&panel_ctx_p);

    hi_log_info("---send I2C---\n");
    drv_panel_custom_send_panel_i2c(panel_ctx_p->image_info.cur_index,
                                    &panel_ctx_p->image_info.advance_info);
    return HI_SUCCESS;
}
hi_void drv_panel_pull_gpio_timer(hi_void)
{
    drv_panel_contex *panel_ctx_p = HI_NULL;
    drv_panel_get_panel_ctx(&panel_ctx_p);
    drv_panel_pull_gpio(&panel_ctx_p->image_info.advance_info);
}

hi_s32 drv_panel_set_intf_power(hi_drv_panel_intf_type intf_type, hi_bool power_on)
{
    hi_s32 ret = HI_SUCCESS;
    drv_panel_contex *panel_ctx_p = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx_p);

    switch (intf_type) {
        case HI_PANEL_INTF_TYPE_LVDS:
            break;
        case HI_PANEL_INTF_TYPE_VBONE:
            hal_panel_set_vbo_lockn_sel();
            hal_panel_set_vbo_htpn_mode();
            hal_panel_set_vbo_frm_mask();
            drv_panel_set_vbo_power(power_on);
            break;
        case HI_PANEL_INTF_TYPE_MINLVDS:
            break;
        case HI_PANEL_INTF_TYPE_EPI:
        case HI_PANEL_INTF_TYPE_ISP:
        case HI_PANEL_INTF_TYPE_CSPI:
        case HI_PANEL_INTF_TYPE_CEDS:
        case HI_PANEL_INTF_TYPE_CHPI:
        case HI_PANEL_INTF_TYPE_USIT:
        case HI_PANEL_INTF_TYPE_CMPI:
            break;
        default:
            break;
    }

    if (power_on == HI_TRUE) {
        hal_panel_set_combo_aphy_enable(HI_TRUE);
        drv_panel_set_combo_phy_reset(HI_TRUE);
        if (drv_panel_check_combo_phy_pll_lock() != HI_SUCCESS) {
            hi_log_err("check phy pll lock failed.\n");
        }
        drv_panel_set_combo_phy_reset(HI_FALSE);
        hal_panel_set_aphy_pre_emp_power_on(HI_TRUE);
        hal_panel_set_aphy_lane_power_on(power_on, panel_ctx_p->lane_used);
    } else {
        hal_panel_set_aphy_lane_power_on(power_on, panel_ctx_p->lane_used);
        hal_panel_set_aphy_pre_emp_power_on(HI_FALSE);
        hal_panel_set_combo_aphy_enable(HI_FALSE);
    }

    return ret;
}

hi_s32 drv_panel_set_intf_power_timer(hi_void *arg)
{
    hi_log_info("---intf power---\n");

    return HI_SUCCESS;
}

hi_s32 drv_panel_set_panel_bl_power_timer(hi_void *arg)
{
    hi_u32 timer = 0;
    hi_log_info("---set backlight on---\n");

    if (drv_panel_set_panel_bl_power(HI_FALSE, HI_TRUE) != HI_SUCCESS) {
        hi_log_err("set panel bl power failed.\n");
        return HI_FAILURE;
    }

    if (drv_panel_get_time_stamp_ms(&timer) != HI_SUCCESS) {
        hi_log_err("set stamp time failed.\n");
        return HI_FAILURE;
    }

    hi_log_info("---set backlight on timer:%d---\n", timer);
    return HI_SUCCESS;
}

#ifdef PANEL_LDM_SUPPORT
hi_s32 drv_panel_set_bl_d_river_power_timer(hi_void *arg)
{
    hi_log_info("---set backlight on---\n");

    drv_panel_set_bl_driver_power(HI_FALSE);

    drv_panel_set_gpio_output_vol(HI_BOARD_PANEL_BL_POWER_GPIONUM,
                                  HI_TRUE ? HI_BOARD_PANEL_BL_POWER_ON : HI_BOARD_PANEL_BL_POWER_OFF);

    hi_log_info("bl_driver config start!\n");
    /* wait bldriver power stable */
    panel_msleep(PANEL_MSLEEP_20);
    panel_drv_set_bl_driver_cfg(arg);

    /* STR scene */
#ifndef __DISP_PLATFORM_BOOT__
    drv_panel_set_ldm_gradule_change(PANEL_LDM_BLDRIVER_DEF_LEVEL);
    panel_msleep(PANEL_MSLEEP_20);
#endif
    drv_panel_set_bl_driver_power(HI_TRUE);
    return HI_SUCCESS;
}
#endif

static hi_void drv_panel_i2c_send_and_set_delay_before_intf_enable(panel_advance_info *advance_info_p,
    drv_panel_image *panel_image_p, hi_bool power_on, hi_u32 tcon_to_power_diff)
{
    hi_u32 intf_delay_time = 0;
    hi_u32 i2c_delay_time = 0;
    hi_u32 i2c_start_time_stamp = 0;
    hi_u32 i2c_end_time_stamp = 0;
    hi_u32 i2c_send_end_time;
    hi_u32 i2c_cost_time;

    /* check i2c delay time */
    if (panel_image_p->advance_info.i2c_info.tcon_to_i2c_delay > tcon_to_power_diff) {
        i2c_delay_time = panel_image_p->advance_info.i2c_info.tcon_to_i2c_delay - tcon_to_power_diff;
        panel_msleep(i2c_delay_time);
    }

    /* start send i2c and caculate time cost */
    hi_log_info("start to send I2C cmd,tcon poweron to I2C cmd sending delay: <%d ms.>\n",
                tcon_to_power_diff + i2c_delay_time);
    drv_panel_get_time_stamp_ms(&i2c_start_time_stamp);
    /* i2c send */
    drv_panel_custom_send_panel_i2c(panel_image_p->cur_index, advance_info_p);
    drv_panel_get_time_stamp_ms(&i2c_end_time_stamp);
    i2c_cost_time = i2c_end_time_stamp - i2c_start_time_stamp;

    /* check intf enable delay time */
    i2c_send_end_time = tcon_to_power_diff + i2c_delay_time + i2c_cost_time;
    if (panel_image_p->reg_panel_info.intf_signal_on_delay > i2c_send_end_time) {
        intf_delay_time = panel_image_p->reg_panel_info.intf_signal_on_delay - i2c_send_end_time;
        panel_msleep(intf_delay_time);
    }

    /* start intf enable */
    hi_log_info("intf enable, tcon poweron to intf enable delay: <%d ms.>\n",
                i2c_send_end_time + intf_delay_time);
    drv_panel_pull_gpio(advance_info_p);
    /* WORKING: drv_panel_set_intf_power(power_on) */
    panel_msleep(panel_image_p->reg_panel_info.bl_on_delay);
    hi_log_info("backlight on,intf enable to backlight on delay: <%d ms.>\n",
                panel_image_p->reg_panel_info.bl_on_delay);
}

static hi_void drv_panel_i2c_send_and_set_delay_after_intf_enable(panel_advance_info *advance_info_p,
    drv_panel_image *panel_image_p, hi_bool power_on, hi_u32 tcon_to_power_diff)
{
    hi_u32 intf_delay_time = 0;
    hi_u32 intf_time_stamp = 0;
    hi_u32 i2c_delay_time = 0;
    hi_u32 i2c_end_time_stamp = 0;
    hi_u32 bl_delay_time = 0;
    hi_u32 i2c_to_intf_time;

    /* check intf enable delay time */
    if (panel_image_p->reg_panel_info.intf_signal_on_delay > tcon_to_power_diff) {
        intf_delay_time = panel_image_p->reg_panel_info.intf_signal_on_delay - tcon_to_power_diff;
        panel_msleep(intf_delay_time);
    }

    /* start intf enable */
    hi_log_info("intf enable, tcon poweron to intf enable delay: <%d ms.>\n",
                tcon_to_power_diff + intf_delay_time);
    drv_panel_pull_gpio(advance_info_p);
    /* WORKING: drv_panel_set_intf_power(power_on) */
    drv_panel_get_time_stamp_ms(&intf_time_stamp);

    /* check i2c send delay time */
    if (advance_info_p->i2c_info.tcon_to_i2c_delay > (tcon_to_power_diff + intf_delay_time)) {
        i2c_delay_time = advance_info_p->i2c_info.tcon_to_i2c_delay - (tcon_to_power_diff + intf_delay_time);
        panel_msleep(i2c_delay_time);
    }

    /* start send i2c */
    hi_log_info("start to send I2C cmd,tcon poweron to I2C cmd sending delay: <%d ms.>\n",
                tcon_to_power_diff + intf_delay_time + i2c_delay_time);
    /* i2c send */
    drv_panel_custom_send_panel_i2c(panel_image_p->cur_index, advance_info_p);
    drv_panel_get_time_stamp_ms(&i2c_end_time_stamp);

    /* check backlight power delay time */
    i2c_to_intf_time = i2c_end_time_stamp - intf_time_stamp;
    if (panel_image_p->reg_panel_info.bl_on_delay > i2c_to_intf_time) {
        bl_delay_time = panel_image_p->reg_panel_info.bl_on_delay - i2c_to_intf_time;
        panel_msleep(bl_delay_time);
    }

    hi_log_info("backlight on,intf enable to backlight on delay: <%d ms.>\n",
                i2c_to_intf_time + bl_delay_time);
}

static hi_void drv_panel_power_set_delay(hi_drv_panel_info *base_info,
    panel_advance_info *advance_info_p, hi_bool power_on, hi_u32 tcon_to_power_diff)
{
    hi_u32 intf_delay_time = 0;
    hi_u32 bl_delay_time;
    drv_panel_contex *panel_ctx_p = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx_p);

    /* check intf enable delay */
    if (panel_ctx_p->image_info.reg_panel_info.intf_signal_on_delay > tcon_to_power_diff) {
        intf_delay_time = panel_ctx_p->image_info.reg_panel_info.intf_signal_on_delay - tcon_to_power_diff;
        panel_msleep(intf_delay_time);
    }

    hi_log_info("intf enable, tcon poweron to intf enable delay: <%d ms.>\n",
                tcon_to_power_diff + intf_delay_time);
    /* befote intf enable, if panel.img have gpio to pull, pull it */
    drv_panel_pull_gpio(advance_info_p);
    /* intf enable */
    /* WORKING: drv_panel_set_intf_power(power_on) */
    bl_delay_time = panel_ctx_p->image_info.reg_panel_info.bl_on_delay;
#ifdef PANEL_LDM_SUPPORT
    if (advance_info_p->ldm_info.ldm_en && !PANEL_LDM_MCU_EXIST) {
        if (bl_delay_time > PANEL_LDM_BLON_DELAY_STANDARD) {
            bl_delay_time = bl_delay_time - PANEL_LDM_BLON_DELAY_STANDARD;
        } else {
            bl_delay_time = PANEL_LDM_BLON_DELAY_TIME;
        }
    }
#endif
    panel_msleep(bl_delay_time);

    hi_log_info("backlight on,intf enable to backlight on delay: <%d ms.>\n", bl_delay_time);
}

static hi_void drv_panel_set_tcon_to_power_diff_and_tcon_power(hi_bool tcon_status,
    hi_u32 *tcon_to_power_diff, hi_bool power_on)
{
    if (tcon_status == HI_TRUE) {
#ifdef __DISP_PLATFORM_BOOT__
        *tcon_to_power_diff = 1500; /* 1500: default param */
#else
        *tcon_to_power_diff = 900; /* 900: default param */
#endif
    } else {
        drv_panel_set_panel_tcon_power(power_on);
    }
}

/************************************************************************************
                                panel power function
************************************************************************************/
hi_s32 drv_panel_set_panel_power(drv_panel_image *panel_image_p, hi_bool power_on)
{
    hi_drv_panel_info *base_info = &panel_image_p->base_info;
    panel_advance_info *advance_info_p = &panel_image_p->advance_info;
    hi_u32 tcon_to_power_diff = 0;
    hi_bool tcon_status = HI_TRUE;

    if (power_on) {
        drv_panel_custom_pull_gpio(panel_image_p->cur_index);
        /* if tcon_power_on early, set tcon to this time delay is HI_BOARD_PANEL_XXX_TCON_TO_NEXT_MSDELAY */
        if (drv_panel_get_panel_tcon_power(&tcon_status) != HI_SUCCESS) {
            hi_log_err("get panel tcon power is failed, tcon status:%d\n", tcon_status);
            return HI_FAILURE;
        }

        /* tcon power to panel power here diff default to 1500ms,
        test as panel_drv_set_gpio_output_vol(HI_BOARD_PANEL_TCON_POWER_GPIONUM, HI_BOARD_PANEL_TCON_POWER_OFF) */
        drv_panel_set_tcon_to_power_diff_and_tcon_power(tcon_status, &tcon_to_power_diff, power_on);

        /* send_i2c point panel.img have I2C to send */
        if (advance_info_p->i2c_info.i2c_cmd_len != 0) {
            /* i2c send before intf enable */
            if (panel_image_p->reg_panel_info.intf_signal_on_delay > advance_info_p->i2c_info.tcon_to_i2c_delay) {
                drv_panel_i2c_send_and_set_delay_before_intf_enable(advance_info_p, panel_image_p,
                                                                    power_on, tcon_to_power_diff);
            } else {
                drv_panel_i2c_send_and_set_delay_after_intf_enable(advance_info_p, panel_image_p,
                                                                   power_on, tcon_to_power_diff);
            }
        } else {
            drv_panel_power_set_delay(base_info, advance_info_p, power_on, tcon_to_power_diff);
        }
#ifdef PANEL_LDM_SUPPORT
        if (advance_info_p->ldm_info.ldm_en && !PANEL_LDM_MCU_EXIST) {
            drv_panel_set_bl_driver_gpio_status();
            drv_panel_set_panel_bl_power(HI_FALSE, power_on);
            panel_msleep(PANEL_MSLEEP_50);
            drv_panel_set_bl_d_river_power_timer((hi_void *)&bl_delay_time);
        } else {
#endif
            drv_panel_set_panel_bl_power(HI_FALSE, power_on);
#ifdef PANEL_LDM_SUPPORT
        }
#endif
    } else {
        drv_panel_set_panel_bl_power(HI_FALSE, power_on);

        panel_msleep(panel_image_p->reg_panel_info.bl_off_delay);
        /* WORKING: drv_panel_set_intf_power(power_on) */
        panel_msleep(panel_image_p->reg_panel_info.intf_signal_off_delay);
        drv_panel_set_panel_tcon_power(power_on);
    }
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_panel_power_on(hi_drv_panel_intf_type intf_type, hi_bool *poweron_p)
{
    hi_s32 ret;
    drv_panel_contex *panel_ctx_p = HI_NULL;
    drv_panel_power_state *power_state_p = HI_NULL;

    drv_panel_get_panel_ctx(&panel_ctx_p);

    power_state_p = &panel_ctx_p->power_state;

    ret = drv_panel_get_panel_tcon_power(&power_state_p->tcon_enable);
    if (ret != HI_SUCCESS) {
        hi_log_err("panel_drv_get_tcon_power error!\n");
        return ret;
    }

    switch (intf_type) {
        case HI_PANEL_INTF_TYPE_LVDS:
            break;
        case HI_PANEL_INTF_TYPE_VBONE:
            drv_panel_get_vbo_power(&power_state_p->intf_enable);
            break;
        case HI_PANEL_INTF_TYPE_MINLVDS:
            break;
        case HI_PANEL_INTF_TYPE_EPI:
        case HI_PANEL_INTF_TYPE_ISP:
        case HI_PANEL_INTF_TYPE_CSPI:
        case HI_PANEL_INTF_TYPE_CEDS:
        case HI_PANEL_INTF_TYPE_CHPI:
        case HI_PANEL_INTF_TYPE_CMPI:
        case HI_PANEL_INTF_TYPE_USIT:
            power_state_p->intf_enable = HI_TRUE;
            break;
        default:
            power_state_p->intf_enable = HI_FALSE;
            break;
    }

    ret = drv_panel_get_panel_bl_power(&power_state_p->back_light_enable);
    if (ret != HI_SUCCESS) {
        hi_log_err("panel_drv_get_backlight_power error!\n");
        return ret;
    }

    power_state_p->power_on = HI_FALSE;
    if (power_state_p->tcon_enable && power_state_p->intf_enable &&
        power_state_p->back_light_enable) {
        power_state_p->power_on = HI_TRUE;
    }

    *poweron_p = power_state_p->power_on;

    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

