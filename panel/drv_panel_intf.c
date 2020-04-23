/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel intf module
* Author: sdk
* Create: 2019-11-20
*/

#ifndef __DISP_PLATFORM_BOOT__
#include "hi_module.h"
#include "hi_drv_dev.h"
#include "linux/hisilicon/securec.h"
#include "drv_panel_ioctl.h"
#include "drv_panel_define.h"
#include "drv_panel.h"
#include "drv_panel_proc.h"
#include "drv_panel_adapter.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static hi_s32 drv_panel_ioctl_set_power_on(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_power_on *power_on = HI_NULL;

    power_on = (panel_power_on *)arg;

    ret = drv_panel_set_power_on(power_on->id, power_on->power_on);
    hi_log_info("call panel CMD_PANEL_SET_POWERON!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_power_on(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_power_on *power_on = HI_NULL;

    power_on = (panel_power_on *)arg;

    ret = drv_panel_get_power_on(power_on->id, &power_on->power_on);
    hi_log_info("call panel CMD_PANEL_GET_POWERON!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_set_backlight_power(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_bl_power_on *bl_enable = HI_NULL;

    bl_enable = (panel_bl_power_on *)arg;

    ret = drv_panel_set_backlight_power(bl_enable->id, bl_enable->enable);
    hi_log_info("call panel PANEL_CMD_SET_BACKLIGHT_POWERON!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_backlight_power(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_bl_power_on *bl_enable = HI_NULL;

    bl_enable = (panel_bl_power_on *)arg;

    ret = drv_panel_get_backlight_power(bl_enable->id, &bl_enable->enable);
    hi_log_info("call panel PANEL_CMD_GET_BACKLIGHT_POWERON!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_aspect(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_aspect *aspect_attr = HI_NULL;

    aspect_attr = (panel_aspect *)arg;

    ret = drv_panel_get_aspect(aspect_attr->id, &aspect_attr->aspect);
    hi_log_info("call panel PANEL_CMD_GET_ASPECT_ATTR!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_3d_type(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_3d_type_info *panel_3d_attr = HI_NULL;

    panel_3d_attr = (panel_3d_type_info *)arg;

    ret = drv_panel_get_3d_type(panel_3d_attr->id, &panel_3d_attr->type_3d);
    hi_log_info("call panel PANEL_CMD_GET_3D_TYPE_ATTR!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_set_dynamic_bl_enable(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_dynamic_bl_enable *dy_bl_enable = HI_NULL;

    dy_bl_enable = (panel_dynamic_bl_enable *)arg;

    ret = drv_panel_set_dynamic_bl_enable(dy_bl_enable->id, dy_bl_enable->enable);
    hi_log_info("call panel PANEL_CMD_SET_DYNAMICBL_BACKLIGHT_ENABLE!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_dynamic_bl_enable(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_dynamic_bl_enable *dy_bl_enable = HI_NULL;

    dy_bl_enable = (panel_dynamic_bl_enable *)arg;

    ret = drv_panel_get_dynamic_bl_enable(dy_bl_enable->id, &dy_bl_enable->enable);
    hi_log_info("call panel PANEL_CMD_GET_DYNAMICBL_BACKLIGHT_ENABLE!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_backlight_range(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_bl_range *bl_range = HI_NULL;

    bl_range = (panel_bl_range *)arg;

    ret = drv_panel_get_backlight_range(bl_range->id, &bl_range->bl_range);
    hi_log_info("call panel PANEL_CMD_GET_BACKLIGHT_RANGE!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_set_backlight_level(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_bl_level *bl_level = HI_NULL;

    bl_level = (panel_bl_level *)arg;

    ret = drv_panel_set_backlight_level(bl_level->id, bl_level->level);
    hi_log_info("call panel PANEL_CMD_SET_BACKLIGHT_LEVEL!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_backlight_level(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_bl_level *bl_level = HI_NULL;

    bl_level = (panel_bl_level *)arg;

    ret = drv_panel_get_backlight_level(bl_level->id, &bl_level->level);
    hi_log_info("call panel CMD_PANEL_GET_BACKLIGHT_LEVEL!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_dim_strength_info(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_dim_strength *dim_str_info = HI_NULL;

    dim_str_info = (panel_dim_strength *)arg;

    ret = drv_panel_get_dim_strength_info(dim_str_info->id, &dim_str_info->dim_strength);
    hi_log_info("call panel PANEL_CMD_GET_DIM_STRGTH_INFO!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_set_dim_strength_level(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_dim_level *dim_str_level = HI_NULL;

    dim_str_level = (panel_dim_level *)arg;

    ret = drv_panel_set_dim_strength_level(dim_str_level->id, dim_str_level->level);
    hi_log_info("call panel PANEL_CMD_SET_DIM_STRGTH_LEVEL!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_dim_strength_level(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_dim_level *dim_str_level = HI_NULL;

    dim_str_level = (panel_dim_level *)arg;

    ret = drv_panel_get_dim_strength_level(dim_str_level->id, &dim_str_level->level);
    hi_log_info("call panel PANEL_CMD_GET_DIM_STRGTH_LEVEL!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_set_ldm_demo_mode(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_ldm_demo *demo_mode = HI_NULL;

    demo_mode = (panel_ldm_demo *)arg;

    ret = drv_panel_set_ldm_demo_mode(demo_mode->id, demo_mode->ld_demo_mode);
    hi_log_info("call panel PANEL_CMD_SET_DIM_DEMO_MODE!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_ldm_demo_mode(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_ldm_demo *demo_mode = HI_NULL;

    demo_mode = (panel_ldm_demo *)arg;

    ret = drv_panel_get_ldm_demo_mode(demo_mode->id, &demo_mode->ld_demo_mode);
    hi_log_info("call panel PANEL_CMD_GET_DIM_DEMO_MODE!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_voltage_range(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_signal *vol_range = HI_NULL;

    vol_range = (panel_signal *)arg;

    ret = drv_panel_get_voltage_range(vol_range->id, &vol_range->signal_mode);
    hi_log_info("call panel PANEL_CMD_GET_VOLTAGE_RANGE!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_set_voltage(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_voltage *voltage = HI_NULL;

    voltage = (panel_voltage *)arg;

    ret = drv_panel_set_voltage(voltage->id, voltage->com_voltage);
    hi_log_info("call panel CMD_PANEL_SET_VOLTAGE!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_voltage(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_voltage *voltage = HI_NULL;

    voltage = (panel_voltage *)arg;

    ret = drv_panel_get_voltage(voltage->id, &voltage->com_voltage);
    hi_log_info("call panel CMD_PANEL_GET_VOLTAGE!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_current_range(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_signal *curr_range = HI_NULL;

    curr_range = (panel_signal *)arg;

    ret = drv_panel_get_current_range(curr_range->id, &curr_range->signal_mode);
    hi_log_info("call panel PANEL_CMD_GET_DRVCURRENT_RANGE!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_set_current(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_current *current_tmp = HI_NULL;

    current_tmp = (panel_current *)arg;

    ret = drv_panel_set_current(current_tmp->id, current_tmp->drv_current);
    hi_log_info("call panel PANEL_CMD_SET_DRVCURRENT!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_current(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_current *current_tmp = HI_NULL;

    current_tmp = (panel_current *)arg;

    ret = drv_panel_get_current(current_tmp->id, &current_tmp->drv_current);
    hi_log_info("call panel PANEL_CMD_GET_DRVCURRENT!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_emphasis_range(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_signal *emp_range = HI_NULL;

    emp_range = (panel_signal *)arg;

    ret = drv_panel_get_emphasis_range(emp_range->id, &emp_range->signal_mode);
    hi_log_info("call panel PANEL_CMD_GET_DRVCURRENT_RANGE!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_set_emphasis(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_emp_info *emphasis = HI_NULL;

    emphasis = (panel_emp_info *)arg;

    ret = drv_panel_set_emphasis(emphasis->id, emphasis->panel_emp);
    hi_log_info("call panel PANEL_CMD_SET_EMPHASIS!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_emphasis(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_emp_info *emphasis = HI_NULL;

    emphasis = (panel_emp_info *)arg;

    ret = drv_panel_get_emphasis(emphasis->id, &emphasis->panel_emp);
    hi_log_info("call panel PANEL_CMD_GET_EMPHASIS!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_set_spread_enable(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_spread_enable *spread_enable = HI_NULL;

    spread_enable = (panel_spread_enable *)arg;

    ret = drv_panel_set_spread_enable(spread_enable->id, spread_enable->enable);
    hi_log_info("call panel PANEL_CMD_SET_SPREAD_ENABLE!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_spread_enable(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_spread_enable *spread_enable = HI_NULL;

    spread_enable = (panel_spread_enable *)arg;

    ret = drv_panel_get_spread_enable(spread_enable->id, &spread_enable->enable);
    hi_log_info("call panel PANEL_CMD_GET_SPREAD_ENABLE!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_spread_ratio_range(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_ratio_range *ratio_range = HI_NULL;

    ratio_range = (panel_ratio_range *)arg;

    ret = drv_panel_get_spread_ratio_range(ratio_range->id, &ratio_range->ratio_range);
    hi_log_info("call panel PANEL_CMD_GET_SPREAD_RATIO_RANGE!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_set_spread_ratio(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_spread_ratio *spread_ratio = HI_NULL;

    spread_ratio = (panel_spread_ratio *)arg;

    ret = drv_panel_set_spread_ratio(spread_ratio->id, spread_ratio->spread_ratio);
    hi_log_info("call panel PANEL_CMD_GET_SPREAD_RATIO!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_spread_ratio(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_spread_ratio *spread_ratio = HI_NULL;

    spread_ratio = (panel_spread_ratio *)arg;

    ret = drv_panel_get_spread_ratio(spread_ratio->id, &spread_ratio->spread_ratio);
    hi_log_info("call panel PANEL_CMD_GET_SPREAD_RATIO!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_spread_freq_range(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_signal *freq_range = HI_NULL;

    freq_range = (panel_signal *)arg;

    ret = drv_panel_get_spread_freq_range(freq_range->id, &freq_range->signal_mode);
    hi_log_info("call panel PANEL_CMD_GET_SPREAD_FREQ_RANGE!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_set_spread_freq(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_spread_freq *spread_freq = HI_NULL;

    spread_freq = (panel_spread_freq *)arg;

    ret = drv_panel_set_spread_freq(spread_freq->id, spread_freq->spread_freq);
    hi_log_info("call panel PANEL_CMD_SET_SPREAD_FREQ!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_spread_freq(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_spread_freq *spread_freq = HI_NULL;

    spread_freq = (panel_spread_freq *)arg;

    ret = drv_panel_get_spread_freq(spread_freq->id, &spread_freq->spread_freq);
    hi_log_info("call panel PANEL_CMD_GET_SPREAD_FREQ!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_panel_name(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_name_info *panel_name = HI_NULL;

    panel_name = (panel_name_info *)arg;

    ret = drv_panel_get_panel_name(panel_name->id, &panel_name->name);
    hi_log_info("call panel PANEL_CMD_GET_PANEL_NAME!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_set_bit_depth(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_bit_depth_info *bit_depth = HI_NULL;

    bit_depth = (panel_bit_depth_info *)arg;

    ret = drv_panel_set_bit_depth(bit_depth->id, bit_depth->bit_depth);
    hi_log_info("call panel PANEL_CMD_SET_BIT_DEPTH!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_bit_depth(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_bit_depth_info *bit_depth = HI_NULL;

    bit_depth = (panel_bit_depth_info *)arg;

    ret = drv_panel_get_bit_depth(bit_depth->id, &bit_depth->bit_depth);
    hi_log_info("call panel PANEL_CMD_GET_BIT_DEPTH!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_set_lvds_attr(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_lvds *lvds_info = HI_NULL;

    lvds_info = (panel_lvds *)arg;

    ret = drv_panel_set_lvds_attr(lvds_info->id, &lvds_info->lvds_attr);
    hi_log_info("call panel PANEL_CMD_SET_LVDS_ATTR!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_lvds_attr(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_lvds *lvds_info = HI_NULL;

    lvds_info = (panel_lvds *)arg;

    ret = drv_panel_get_lvds_attr(lvds_info->id, &lvds_info->lvds_attr);
    hi_log_info("call panel PANEL_CMD_GET_LVDS_ATTR!\n");

    return ret;
}

static hi_s32 drv_panel_ioctl_get_interface_type(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    panel_intf_type_info *intf_type = HI_NULL;

    intf_type = (panel_intf_type_info *)arg;

    ret = drv_panel_get_interface_type(intf_type->id, &intf_type->intf_type);
    hi_log_info("call panel PANEL_CMD_GET_INTF_TYPE!\n");

    return ret;
}

static osal_ioctl_cmd g_panel_cmd_list[] = {
    { PANEL_CMD_SET_POWERON,           drv_panel_ioctl_set_power_on },
    { PANEL_CMD_GET_POWERON,           drv_panel_ioctl_get_power_on },
    { PANEL_CMD_SET_BACKLIGHT_POWERON, drv_panel_ioctl_set_backlight_power },
    { PANEL_CMD_GET_BACKLIGHT_POWERON, drv_panel_ioctl_get_backlight_power },
    { PANEL_CMD_GET_ASPECT,            drv_panel_ioctl_get_aspect },
    { PANEL_CMD_GET_3D_TYPE,           drv_panel_ioctl_get_3d_type },
    { PANEL_CMD_SET_DYNAMICBL_BACKLIGHT_ENABLE,  drv_panel_ioctl_set_dynamic_bl_enable },
    { PANEL_CMD_GET_DYNAMICBL_BACKLIGHT_ENABLE,  drv_panel_ioctl_get_dynamic_bl_enable },
    { PANEL_CMD_GET_BACKLIGHT_RANGE,   drv_panel_ioctl_get_backlight_range },
    { PANEL_CMD_SET_BACKLIGHT_LEVEL,   drv_panel_ioctl_set_backlight_level },
    { PANEL_CMD_GET_BACKLIGHT_LEVEL,   drv_panel_ioctl_get_backlight_level },
    { PANEL_CMD_GET_DIM_STRGTH_INFO,   drv_panel_ioctl_get_dim_strength_info },
    { PANEL_CMD_SET_DIM_STRGTH_LEVEL,  drv_panel_ioctl_set_dim_strength_level },
    { PANEL_CMD_GET_DIM_STRGTH_LEVEL,  drv_panel_ioctl_get_dim_strength_level },
    { PANEL_CMD_SET_DIM_DEMO_MODE,     drv_panel_ioctl_set_ldm_demo_mode },
    { PANEL_CMD_GET_DIM_DEMO_MODE,     drv_panel_ioctl_get_ldm_demo_mode },
    { PANEL_CMD_GET_VOLTAGE_RANGE,     drv_panel_ioctl_get_voltage_range },
    { PANEL_CMD_SET_VOLTAGE,           drv_panel_ioctl_set_voltage },
    { PANEL_CMD_GET_VOLTAGE,           drv_panel_ioctl_get_voltage },
    { PANEL_CMD_GET_CURRENT_RANGE,     drv_panel_ioctl_get_current_range },
    { PANEL_CMD_SET_CURRENT,           drv_panel_ioctl_set_current },
    { PANEL_CMD_GET_CURRENT,           drv_panel_ioctl_get_current },
    { PANEL_CMD_GET_EMPHASIS_RANGE,    drv_panel_ioctl_get_emphasis_range },
    { PANEL_CMD_SET_EMPHASIS,          drv_panel_ioctl_set_emphasis },
    { PANEL_CMD_GET_EMPHASIS,          drv_panel_ioctl_get_emphasis },
    { PANEL_CMD_SET_SPREAD_ENABLE,     drv_panel_ioctl_set_spread_enable },
    { PANEL_CMD_GET_SPREAD_ENABLE,     drv_panel_ioctl_get_spread_enable },
    { PANEL_CMD_GET_SPREAD_RATIO_RANGE, drv_panel_ioctl_get_spread_ratio_range },
    { PANEL_CMD_SET_SPREAD_RATIO,      drv_panel_ioctl_set_spread_ratio },
    { PANEL_CMD_GET_SPREAD_RATIO,      drv_panel_ioctl_get_spread_ratio },
    { PANEL_CMD_GET_SPREAD_FREQ_RANGE, drv_panel_ioctl_get_spread_freq_range },
    { PANEL_CMD_SET_SPREAD_FREQ,       drv_panel_ioctl_set_spread_freq },
    { PANEL_CMD_GET_SPREAD_FREQ,       drv_panel_ioctl_get_spread_freq },
    { PANEL_CMD_GET_PANEL_NAME,        drv_panel_ioctl_get_panel_name },
    { PANEL_CMD_SET_BIT_DEPTH,         drv_panel_ioctl_set_bit_depth },
    { PANEL_CMD_GET_BIT_DEPTH,         drv_panel_ioctl_get_bit_depth },
    { PANEL_CMD_SET_LVDS_ATTR,         drv_panel_ioctl_set_lvds_attr },
    { PANEL_CMD_GET_LVDS_ATTR,         drv_panel_ioctl_get_lvds_attr },
    { PANEL_CMD_GET_INTF_TYPE,         drv_panel_ioctl_get_interface_type },
};

hi_s32 drv_panel_suspend(hi_void *private_data)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_resume(hi_void *private_data)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_lowpower_enter(hi_void *private_data)
{
    HI_PRINT("panel lowpower enter start\n");

    HI_PRINT("panel lowpower enter end\n");

    return HI_SUCCESS;
}

hi_s32 drv_panel_lowpower_exit(hi_void *private_data)
{
    HI_PRINT("panel lowpower exit start\n");

    HI_PRINT("panel lowpower exit end\n");
    return HI_SUCCESS;
}

static osal_pmops panel_pmops = {
    .pm_suspend = drv_panel_suspend,
    .pm_resume = drv_panel_resume,
    .pm_lowpower_enter = drv_panel_lowpower_enter,
    .pm_lowpower_exit  = drv_panel_lowpower_exit,
    .pm_poweroff = NULL,
    .private_data = NULL,
};

hi_s32 panel_file_open(hi_void *private_data)
{
    return HI_SUCCESS;
}
hi_s32 panel_file_close(hi_void *private_data)
{
    return HI_SUCCESS;
}

static osal_fileops panel_fileops = {
    .open = panel_file_open,
    .read = NULL,
    .write = NULL,
    .llseek = NULL,
    .release = panel_file_close,
    .poll = NULL,
    .mmap = NULL,
    .cmd_list = g_panel_cmd_list,
    .cmd_cnt = 0,
};

static osal_dev panel_device = {
    .minor = HI_DEV_PANEL_MINOR,
    .fops = &panel_fileops,
    .pmops = &panel_pmops,
};

static hi_drv_panel_export_func s_panel_export_func = {
    .panel_isr_main = hi_drv_panel_isr_main,
    .panel_backlight_change = hi_drv_panel_backlight_change,
    .panel_localdimming_regist = hi_drv_panel_localdimming_regist,
    .panel_change_mode = hi_drv_panel_change_mode,
};

hi_s32 drv_panel_export_func_register(hi_void)
{
    hi_s32 ret;

    ret = osal_exportfunc_register((hi_u32)HI_ID_PANEL, "HI_PANEL", (hi_void *)(&s_panel_export_func));
    if (ret != HI_SUCCESS) {
        hi_log_err("osal_exportfunc_register PANEL failed\n");
        return ret;
    }

    return HI_SUCCESS;
}

hi_void drv_panel_unregister(hi_void)
{
    osal_exportfunc_unregister((hi_u32)HI_ID_PANEL);
    return;
}

hi_s32 drv_panel_mod_init(hi_void)
{
    hi_u32 time_ms = 0;
    hi_s32 ret;

    panel_fileops.cmd_cnt = sizeof(g_panel_cmd_list) / sizeof(osal_ioctl_cmd);
    printk("cmd_cnt = %d\n", panel_fileops.cmd_cnt);

    ret = snprintf_s(panel_device.name, OSAL_DEV_NAME_LEN - 1, strlen(HI_DEV_PANEL_NAME), "%s", HI_DEV_PANEL_NAME);
    if (ret < 0) {
        hi_log_err("snprintf_s failed\n");
        goto SNPRINTF_FAIL;
    }

#ifdef OSAL_SUPPORT
    panel_check_func_ret(osal_dev_register(&panel_device), osal_dev_register, ret);
    if (ret != HI_SUCCESS) {
        goto DEV_REGIST_FAIL;
    }

    panel_check_func_ret(drv_panel_proc_add(), drv_panel_proc_add, ret);
    if (ret != HI_SUCCESS) {
        goto PROC_ADD_FAIL;
    }
#endif

    panel_check_func_ret(drv_panel_init(), drv_panel_init, ret);
    if (ret != HI_SUCCESS) {
        goto INIT_FAIL;
    }

    panel_check_func_ret(drv_panel_export_func_register(), drv_panel_export_func_register, ret);
    if (ret != HI_SUCCESS) {
        goto FUN_REGIST_FAIL;
    }

    HI_PRINT("load hi_panel.ko success. \t(%s)\n", VERSION_STRING);
    drv_panel_get_time_stamp_ms(&time_ms);
    HI_PRINT("[boottime]  panel loader %d finished !\n", time_ms);

    return HI_SUCCESS;

FUN_REGIST_FAIL:
    drv_panel_deinit();
INIT_FAIL:
#ifdef OSAL_SUPPORT
    drv_panel_proc_del();
PROC_ADD_FAIL:
    osal_dev_unregister(&panel_device);
DEV_REGIST_FAIL:
#endif
SNPRINTF_FAIL:
    return HI_FAILURE;
}

hi_void drv_panel_mod_exit(hi_void)
{
    return;
}

#ifdef MODULE
module_init(drv_panel_mod_init);
module_exit(drv_panel_mod_exit);
#else
EXPORT_SYMBOL(drv_panel_mod_init);
EXPORT_SYMBOL(drv_panel_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

