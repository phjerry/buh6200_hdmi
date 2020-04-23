/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: mipi tx module
* Author: sdk
* Create: 2019-11-20
*/
#ifndef __MIPI_TX_HAL_H__
#define __MIPI_TX_HAL_H__

#define MAX_CTRL_NUM   2
#define MAX_LANE_NUM   4 /* 4lane in one mipi ctr */

#include "hi_type.h"
#include "drv_mipi_tx_ioctl.h"
#include "drv_disp_ext.h"
#include "hi_drv_mipi.h"

typedef struct {
    unsigned int vall_det;
    unsigned int vact_det;
    unsigned int hall_det;
    unsigned int hact_det;
} mipi_tx_dev_phy;

typedef enum {
    MIPI_DEV_TYPE_CTR0 = 0x0,
    MIPI_DEV_TYPE_CTR1,
    MIPI_DEV_TYPE_CTR0_CTR1,
    MIPI_DEV_TYPE_CTR0_DSC,
    MIPI_DEV_TYPE_MAX
} mipi_dev_type;

typedef enum {
    MIPI_RGB_16_BIT = 0x0,
    MIPI_RGB_18_BIT,
    MIPI_RGB_24_BIT,
    MIPI_YUV420_8_BIT_NORMAL,
    MIPI_YUV420_8_BIT_LEGACY,
    MIPI_YUV422_8_BIT,
    MIPI_DSI_COMPRESS,
    MIPI_MAX
} mipi_output_format;

typedef struct {
    hi_u16  vid_pkt_size;
    hi_u16  vid_hsa_pixels;
    hi_u16  vid_hbp_pixels;
    hi_u16  vid_hfp_pixels;
    hi_u16  vid_hline_pixels; /* HSA+HBP+HACT+HFP */
    hi_u16  vid_vsa_lines;
    hi_u16  vid_vbp_lines;
    hi_u16  vid_vfp_lines;
    hi_u16  vid_active_lines;
    hi_u16  edpi_cmd_size; /* set default to 0, looks like this is the momory size of write cmd, check how to use it */
} mipi_sync_info;

typedef struct { /* all the information should fill in this struct */
    hi_drv_mipi_id        id; /* initialize to MAX */
    hi_s16                lane_id[MAX_LANE_NUM]; /* lane_id: -1 - disable */
    hi_mipi_output_mode   output_mode;
    hi_mipi_video_mode    video_mode;
    mipi_output_format    output_format;
    mipi_sync_info        sync_info;
    hi_u32                phy_data_rate; /* mbps, need to be calculated separately(add a function) */
    hi_u32                pixel_clk; /* KHz */
    hi_drv_mipi_mode      mipi_mode;
} mipi_tx_dev_cfg;

hi_s32 mipi_tx_drv_set_phy_cfg(hi_s32 index, const mipi_tx_dev_cfg *p_dev_cfg);
hi_s32 mipi_tx_drv_set_controller_cfg(hi_s32 index, const mipi_tx_dev_cfg *p_dev_cfg);

hi_s32 mipi_tx_drv_set_cmd_info(mipi_cmd_info *cmd_info);
hi_s32 mipi_tx_drv_get_attr(mipi_attr_pair *attr_pair);
hi_s32 mipi_tx_drv_set_attr(const mipi_attr_pair *attr_pair);

hi_void mipi_tx_drv_enable_input(hi_drv_mipi_id id, hi_mipi_output_mode output_mode);
hi_void mipi_tx_drv_disable_input(hi_drv_mipi_id id, hi_mipi_output_mode output_mode);

hi_s32 mipi_tx_drv_init(mipi_dev_type mipi_mode);
hi_void mipi_tx_drv_exit(hi_void);

hi_void mipi_tx_unregister_irq(mipi_dev_type dev_type);
hi_s32 mipi_tx_register_irq(mipi_dev_type dev_type);

hi_void read_phy_reg(hi_u32 index, hi_u32 len);
hi_void set_phy_reg(hi_u32 index, hi_u32 addr, hi_u8 value);

#endif
