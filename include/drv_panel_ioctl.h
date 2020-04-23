/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel ioctl module
* Author: sdk
* Create: 2019-11-20
*/

#ifndef __DRV_PANEL_IOCTL_H__
#define __DRV_PANEL_IOCTL_H__

#include "drv_panel_struct.h"
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define PANEL_ATTR_LIST_MAX     19 /* 19 is the max len of panel attr list */
#define PANEL_NAME_MAX          48 /* 19 is the max len of panel name */

typedef struct {
    hi_u32 panel_signal_list[PANEL_ATTR_LIST_MAX];
    hi_u32 valid_data_cnt;
} panel_signal_list;

typedef struct {
    panel_id_index id;
    hi_bool power_on;
} panel_power_on;

typedef struct {
    panel_id_index id;
    hi_bool enable;
} panel_bl_power_on;

typedef struct {
    panel_id_index id;
    hi_bool enable;
} panel_dynamic_bl_enable;

typedef struct {
    panel_id_index id;
    hi_bool enable;
} panel_spread_enable;

typedef struct {
    panel_id_index id;
    panel_aspect_info aspect;
} panel_aspect;

typedef struct {
    panel_id_index id;
    hi_drv_panel_3d_type type_3d;
} panel_3d_type_info;

typedef struct {
    panel_id_index id;
    hi_drv_panel_range bl_range;
} panel_bl_range;

typedef struct {
    panel_id_index id;
    hi_u32 level;
} panel_bl_level;

typedef struct {
    panel_id_index id;
    hi_u32 level;
} panel_dim_level;

typedef struct {
    panel_id_index id;
    panel_dim_strength_info  dim_strength;
} panel_dim_strength;

typedef struct {
    panel_id_index id;
    hi_drv_panel_ldm_demo_mode  ld_demo_mode;
} panel_ldm_demo;

typedef struct {
    panel_id_index id;
    panel_signal_list  signal_mode;
} panel_signal;

typedef struct {
    panel_id_index id;
    hi_u32 com_voltage;
} panel_voltage;

typedef struct {
    panel_id_index id;
    hi_u32 drv_current;
} panel_current;

typedef struct {
    panel_id_index id;
    hi_u32 panel_emp;
} panel_emp_info;

typedef struct {
    panel_id_index id;
    hi_u32 spread_ratio;
} panel_spread_ratio;

typedef struct {
    panel_id_index id;
    hi_u32 spread_freq;
} panel_spread_freq;

typedef struct {
    panel_id_index id;
    hi_drv_panel_range ratio_range;
} panel_ratio_range;

typedef struct {
    panel_id_index id;
    panel_name name;
} panel_name_info;

typedef struct {
    panel_id_index id;
    hi_drv_panel_bit_depth bit_depth;
} panel_bit_depth_info;

typedef struct {
    panel_id_index id;
    hi_drv_panel_lvds_attr lvds_attr;
} panel_lvds;

typedef struct {
    panel_id_index id;
    hi_drv_panel_intf_type intf_type;
} panel_intf_type_info;

typedef enum {
    PANEL_IOC_SET_POWERON = 0,
    PANEL_IOC_GET_POWERON,
    PANEL_IOC_SET_BACKLIGHT_POWERON,
    PANEL_IOC_GET_BACKLIGHT_POWERON,
    PANEL_IOC_GET_ASPECT,
    PANEL_IOC_GET_3D_TYPE,
    PANEL_IOC_SET_DYNAMICBL_BACKLIGHT_ENABLE,
    PANEL_IOC_GET_DYNAMICBL_BACKLIGHT_ENABLE,
    PANEL_IOC_GET_BACKLIGHT_RANGE,
    PANEL_IOC_SET_BACKLIGHT_LEVEL,
    PANEL_IOC_GET_BACKLIGHT_LEVEL,
    PANEL_IOC_GET_DIM_STRGTH_INFO,
    PANEL_IOC_SET_DIM_STRGTH_LEVEL,
    PANEL_IOC_GET_DIM_STRGTH_LEVEL,
    PANEL_IOC_SET_DIM_DEMO_MODE,
    PANEL_IOC_GET_DIM_DEMO_MODE,
    PANEL_IOC_GET_VOLTAGE_RANGE,
    PANEL_IOC_SET_VOLTAGE,
    PANEL_IOC_GET_VOLTAGE,
    PANEL_IOC_GET_CURRENT_RANGE,
    PANEL_IOC_SET_CURRENT,
    PANEL_IOC_GET_CURRENT,
    PANEL_IOC_GET_EMPHASIS_RANGE,
    PANEL_IOC_SET_EMPHASIS,
    PANEL_IOC_GET_EMPHASIS,
    PANEL_IOC_SET_SPREAD_ENABLE,
    PANEL_IOC_GET_SPREAD_ENABLE,
    PANEL_IOC_GET_SPREAD_RATIO_RANGE,
    PANEL_IOC_SET_SPREAD_RATIO,
    PANEL_IOC_GET_SPREAD_RATIO,
    PANEL_IOC_GET_SPREAD_FREQ_RANGE,
    PANEL_IOC_SET_SPREAD_FREQ,
    PANEL_IOC_GET_SPREAD_FREQ,
    PANEL_IOC_GET_PANEL_NAME,
    PANEL_IOC_SET_BIT_DEPTH,
    PANEL_IOC_GET_BIT_DEPTH,
    PANEL_IOC_SET_LVDS_ATTR,
    PANEL_IOC_GET_LVDS_ATTR,
    PANEL_IOC_GET_INTF_TYPE,
} panel_ioc;

#define PANEL_CMD_SET_POWERON           _IOW(HI_ID_PANEL,  PANEL_IOC_SET_POWERON, panel_power_on)
#define PANEL_CMD_GET_POWERON           _IOWR(HI_ID_PANEL, PANEL_IOC_GET_POWERON, panel_power_on)
#define PANEL_CMD_SET_BACKLIGHT_POWERON _IOW(HI_ID_PANEL,  PANEL_IOC_SET_BACKLIGHT_POWERON, panel_bl_power_on)
#define PANEL_CMD_GET_BACKLIGHT_POWERON _IOWR(HI_ID_PANEL, PANEL_IOC_GET_BACKLIGHT_POWERON, panel_bl_power_on)
#define PANEL_CMD_GET_ASPECT            _IOWR(HI_ID_PANEL, PANEL_IOC_GET_ASPECT, panel_aspect)
#define PANEL_CMD_GET_3D_TYPE           _IOWR(HI_ID_PANEL, PANEL_IOC_GET_3D_TYPE, hi_drv_panel_3d_type)
#define PANEL_CMD_SET_DYNAMICBL_BACKLIGHT_ENABLE  \
                _IOW(HI_ID_PANEL,  PANEL_IOC_SET_DYNAMICBL_BACKLIGHT_ENABLE, panel_dynamic_bl_enable)
#define PANEL_CMD_GET_DYNAMICBL_BACKLIGHT_ENABLE  \
                _IOWR(HI_ID_PANEL, PANEL_IOC_GET_DYNAMICBL_BACKLIGHT_ENABLE, panel_dynamic_bl_enable)
#define PANEL_CMD_GET_BACKLIGHT_RANGE   _IOWR(HI_ID_PANEL, PANEL_IOC_GET_BACKLIGHT_RANGE, panel_bl_range)
#define PANEL_CMD_SET_BACKLIGHT_LEVEL   _IOW(HI_ID_PANEL,  PANEL_IOC_SET_BACKLIGHT_LEVEL, panel_bl_level)
#define PANEL_CMD_GET_BACKLIGHT_LEVEL   _IOWR(HI_ID_PANEL, PANEL_IOC_GET_BACKLIGHT_LEVEL, panel_bl_level)
#define PANEL_CMD_GET_DIM_STRGTH_INFO   \
                _IOWR(HI_ID_PANEL, PANEL_IOC_GET_DIM_STRGTH_INFO, panel_dim_strength)
#define PANEL_CMD_SET_DIM_STRGTH_LEVEL  _IOW(HI_ID_PANEL,  PANEL_IOC_SET_DIM_STRGTH_LEVEL, panel_dim_level)
#define PANEL_CMD_GET_DIM_STRGTH_LEVEL  _IOWR(HI_ID_PANEL, PANEL_IOC_GET_DIM_STRGTH_LEVEL, panel_dim_level)
#define PANEL_CMD_SET_DIM_DEMO_MODE     _IOW(HI_ID_PANEL,  PANEL_IOC_SET_DIM_DEMO_MODE, panel_ldm_demo)
#define PANEL_CMD_GET_DIM_DEMO_MODE     _IOWR(HI_ID_PANEL, PANEL_IOC_GET_DIM_DEMO_MODE, panel_ldm_demo)
#define PANEL_CMD_GET_VOLTAGE_RANGE     _IOWR(HI_ID_PANEL, PANEL_IOC_GET_VOLTAGE_RANGE, panel_signal)
#define PANEL_CMD_SET_VOLTAGE           _IOW(HI_ID_PANEL,  PANEL_IOC_SET_VOLTAGE, panel_voltage)
#define PANEL_CMD_GET_VOLTAGE           _IOWR(HI_ID_PANEL, PANEL_IOC_GET_VOLTAGE, panel_voltage)
#define PANEL_CMD_GET_CURRENT_RANGE     _IOWR(HI_ID_PANEL, PANEL_IOC_GET_CURRENT_RANGE, panel_signal)
#define PANEL_CMD_SET_CURRENT           _IOW(HI_ID_PANEL,  PANEL_IOC_SET_CURRENT, panel_current)
#define PANEL_CMD_GET_CURRENT           _IOWR(HI_ID_PANEL, PANEL_IOC_GET_CURRENT, panel_current)
#define PANEL_CMD_GET_EMPHASIS_RANGE    _IOWR(HI_ID_PANEL, PANEL_IOC_GET_EMPHASIS_RANGE, panel_signal)
#define PANEL_CMD_SET_EMPHASIS          _IOW(HI_ID_PANEL,  PANEL_IOC_SET_EMPHASIS, panel_emp_info)
#define PANEL_CMD_GET_EMPHASIS          _IOWR(HI_ID_PANEL, PANEL_IOC_GET_EMPHASIS, panel_emp_info)
#define PANEL_CMD_SET_SPREAD_ENABLE     _IOW(HI_ID_PANEL,  PANEL_IOC_SET_SPREAD_ENABLE, panel_spread_enable)
#define PANEL_CMD_GET_SPREAD_ENABLE     _IOWR(HI_ID_PANEL, PANEL_IOC_GET_SPREAD_ENABLE, panel_spread_enable)
#define PANEL_CMD_GET_SPREAD_RATIO_RANGE _IOWR(HI_ID_PANEL, PANEL_IOC_GET_SPREAD_RATIO_RANGE, panel_ratio_range)
#define PANEL_CMD_SET_SPREAD_RATIO      _IOW(HI_ID_PANEL,  PANEL_IOC_SET_SPREAD_RATIO, panel_spread_ratio)
#define PANEL_CMD_GET_SPREAD_RATIO      _IOWR(HI_ID_PANEL, PANEL_IOC_GET_SPREAD_RATIO, panel_spread_ratio)
#define PANEL_CMD_GET_SPREAD_FREQ_RANGE _IOWR(HI_ID_PANEL, PANEL_IOC_GET_SPREAD_FREQ_RANGE, panel_signal)
#define PANEL_CMD_SET_SPREAD_FREQ       _IOW(HI_ID_PANEL,  PANEL_IOC_SET_SPREAD_FREQ, panel_spread_freq)
#define PANEL_CMD_GET_SPREAD_FREQ       _IOWR(HI_ID_PANEL, PANEL_IOC_GET_SPREAD_FREQ, panel_spread_freq)
#define PANEL_CMD_GET_PANEL_NAME        _IOWR(HI_ID_PANEL, PANEL_IOC_GET_PANEL_NAME, panel_name_info)
#define PANEL_CMD_SET_BIT_DEPTH         _IOW(HI_ID_PANEL,  PANEL_IOC_SET_BIT_DEPTH, panel_bit_depth_info)
#define PANEL_CMD_GET_BIT_DEPTH         _IOWR(HI_ID_PANEL, PANEL_IOC_GET_BIT_DEPTH, panel_bit_depth_info)
#define PANEL_CMD_SET_LVDS_ATTR         _IOW(HI_ID_PANEL,  PANEL_IOC_SET_LVDS_ATTR, panel_lvds)
#define PANEL_CMD_GET_LVDS_ATTR         _IOWR(HI_ID_PANEL, PANEL_IOC_GET_LVDS_ATTR, panel_lvds)
#define PANEL_CMD_GET_INTF_TYPE         _IOWR(HI_ID_PANEL, PANEL_IOC_GET_INTF_TYPE, panel_intf_type_info)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_PANEL_IOCTL_H__ */

