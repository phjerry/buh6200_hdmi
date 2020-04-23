/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
* Description: mipi kernel export interface definition.
* Author: Hisilicon hisecurity group
* Create: 2019-07-13
 */
#ifndef __HI_DRV_MIPI_H__
#define __HI_DRV_MIPI_H__

#include "hi_type.h"
#include "hi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define CMD_MAX_NUM_PANEL  24
#define CMD_MAX_NUM_ARG  24
#define MAX_LANE_NUM 4

typedef enum {
    HI_DRV_MIPI_ID_0 = 0,
    HI_DRV_MIPI_ID_1 = 1,
    HI_MIPI_ID_MAX
} hi_drv_mipi_id;

typedef enum {
    HI_DRV_MIPI_MODE_SINGLE = 0,
    HI_DRV_MIPI_MODE_SINGLE_DSC,
    HI_DRV_MIPI_MODE_DOUBLE,
    HI_DRV_MIPI_MODE_MAX
} hi_drv_mipi_mode;

typedef enum {
    OUTPUT_MODE_CSI       = 0x0, /* csi mode */
    OUTPUT_MODE_DSI_VIDEO = 0x1, /* dsi video mode */
    OUTPUT_MODE_DSI_CMD   = 0x2, /* dsi command mode */
    OUTPUT_MODE_MAX
} hi_mipi_output_mode;

typedef enum {
    BURST_MODE                 = 0x0,
    NON_BURST_MODE_SYNC_PULSES = 0x1,
    NON_BURST_MODE_SYNC_EVENTS = 0x2,
    VIDEO_MODE_MAX
} hi_mipi_video_mode;

typedef struct {
    hi_bool is_support;
    hi_bool status;
} hi_mipi_dsc_status;

typedef struct {
    hi_u32 cmd_type; /* cmd type: ref to dcs specificatiion */
    hi_u32 arg_count; /* mumber of cmd parameters: >=0 <= 25 */
    hi_u8 value[CMD_MAX_NUM_ARG]; /* CMD_MAX_NUM_ARG = 25, specify cmd parameters */
} panel_cmd;

typedef struct {
    hi_s16 lane_id[MAX_LANE_NUM]; /* lane_id: -1 - disable */
    hi_mipi_output_mode output_mode;
    hi_mipi_video_mode video_mode;
    hi_drv_mipi_mode mipi_mode;
    hi_u32 bpp; /* >= 16*8 <=48*12-1 */
    hi_u32 bri_percent; /* >0 <=100 */
    hi_u32 cmd_count; /* >=0 <= 30 */
    panel_cmd cmds[CMD_MAX_NUM_PANEL];
} hi_drv_mipi_attr_set;

typedef struct {
    hi_drv_mipi_id id;
    hi_drv_mipi_attr_set attr_from_pdm;
} hi_drv_mipi_pdm_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
