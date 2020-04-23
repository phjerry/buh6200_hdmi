/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel module
* Author: sdk
* Create: 2019-11-20
*/

#ifndef __DRV_PANEL_STRUCT_H__
#define __DRV_PANEL_STRUCT_H__

#include "hi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PANEL_NAME_MAX_LEN 48

typedef enum {
    PANEL_ID_0,
    PANEL_ID_1,
    PANEL_ID_MAX,
} panel_id_index;

typedef enum {
    HI_PANEL_BIT_DEPTH_6BIT,
    HI_PANEL_BIT_DEPTH_8BIT,
    HI_PANEL_BIT_DEPTH_10BIT,
    HI_PANEL_BIT_DEPTH_12BIT,
    HI_PANEL_BIT_DEPTH_16BIT,
    HI_PANEL_BIT_DEPTH_MAX,
} hi_drv_panel_bit_depth;

typedef enum {
    HI_PANEL_INTF_TYPE_LVDS,
    HI_PANEL_INTF_TYPE_VBONE,
    HI_PANEL_INTF_TYPE_MINLVDS,
    HI_PANEL_INTF_TYPE_EPI,
    HI_PANEL_INTF_TYPE_ISP,
    HI_PANEL_INTF_TYPE_CEDS,
    HI_PANEL_INTF_TYPE_CHPI,
    HI_PANEL_INTF_TYPE_CMPI,
    HI_PANEL_INTF_TYPE_CSPI,
    HI_PANEL_INTF_TYPE_USIT,
    HI_PANEL_INTF_TYPE_MIPI,
    HI_PANEL_INTF_TYPE_MAX,
} hi_drv_panel_intf_type;


typedef enum {
    PANEL_LVDS_LINK_MAP_PIXEL0,
    PANEL_LVDS_LINK_MAP_PIXEL1,
    PANEL_LVDS_LINK_MAP_MAX,
} hi_drv_panel_lvds_link_map;

typedef struct {
    hi_drv_panel_lvds_link_map link_map0;
    hi_drv_panel_lvds_link_map link_map1;
} hi_drv_panel_lvds_link;

typedef enum {
    PANEL_LVDS_FORMAT_VESA,
    PANEL_LVDS_FORMAT_JEIDA,
    PANEL_LVDS_FORMAT_FP,
    PANEL_LVDS_FORMAT_MAX,
} hi_drv_panel_lvds_format;

typedef struct {
    hi_drv_panel_lvds_link lvds_link;
    hi_drv_panel_lvds_format lvds_fmt;
} hi_drv_panel_lvds_attr;

typedef enum {
    PANEL_LDM_DEMO_MODE_OFF,
    PANEL_LDM_DEMO_MODE_HORSELIGHT,
    PANEL_LDM_DEMO_MODE_RIGHT_SCREEN,
    PANEL_LDM_DEMO_MODE_LEFT_SCREEN,
    PANEL_LDM_DEMO_MODE_TOP_SCREEN,
    PANEL_LDM_DEMO_MODE_BOTTOM_SCREEN,
    PANEL_LDM_DEMO_MODE_WHITE_SCREEN,
    PANEL_LDM_DEMO_MODE_RIGHT_WHITE_SCREEN,
    PANEL_LDM_DEMO_MODE_LEFT_WHITE_SCREEN,
    PANEL_LDM_DEMO_MODE_TOP_WHITE_SCREEN,
    PANEL_LDM_DEMO_MODE_BOTTOM_WHITE_SCREEN,
    PANEL_LDM_DEMO_MODE_MAX,
} hi_drv_panel_ldm_demo_mode;

typedef struct {
    hi_u16 min;
    hi_u16 max;
} hi_drv_panel_range;

typedef enum {
    PANEL_DIM_0D = 0,
    PANEL_DIM_LOCAL,
    PANEL_DIM_MAX,
} panel_dim_type;

typedef struct {
    panel_dim_type dim_type;
    hi_drv_panel_range dim_range;
} panel_dim_strength_info;

typedef enum {
    PANEL_ASPECT_RATIO_4_3,
    PANEL_ASPECT_RATIO_16_9,
    PANEL_ASPECT_RATIO_14_9,
    PANEL_ASPECT_RATIO_21_9,
    PANEL_ASPECT_RATIO_16_10,
    PANEL_ASPECT_RATIO_MAX,
} panel_aspect_ratio;

typedef struct {
    hi_u16 width;
    hi_u16 height;
    panel_aspect_ratio aspect;
} panel_aspect_info;

typedef enum {
    HI_DRV_PANEL_3D_TYPE_2D,
    HI_DRV_PANEL_3D_TYPE_FS,
    HI_DRV_PANEL_3D_TYPE_LBL,
    HI_DRV_PANEL_3D_TYPE_MAX,
} hi_drv_panel_3d_type;

typedef struct {
    hi_char name[PANEL_NAME_MAX_LEN];
    hi_u8 size;
} panel_name;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_PANEL_STRUCT_H__ */

