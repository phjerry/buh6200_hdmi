/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: mipi dsc ctrl module
* Author: sdk
* Create: 2019-11-20
*/
#ifndef __MIPI_DSC_CTRL_H__
#define __MIPI_DSC_CTRL_H__

#include <linux/types.h>
#include "mipi_dsc_reg.h"
#include "mipi_dsc_helper.h"

/* DSC current status */
typedef struct {
    hi_bool dsc_enable;
    hi_bool dsc_ck_gt_en;
} mipi_dsc_status;

typedef struct {
    hi_u32 buffer_fullnes_exceed_num;
    hi_u32 buffer_fullnes_over_flow_num;
    hi_u32 buffer_fullnes_under_flow_num;
    hi_u32 buffer_fullnes_err_type;
} rc_err_set;

typedef struct {
    hi_u8 version;
    mipi_dsc_status status;
    dsc_cfg_s cfg_s;
    para_input_s inpara;
    rc_err_set rc_err_count;
} mipi_dsc_controller;

typedef enum {
    MIPI_DSC_ID_0      = 0x0,
    MIPI_DSC_ID_MAX
} mipi_dsc_id;

hi_s32 hi_mipi_dsc_init(mipi_dsc_id dsc_id);

hi_s32 hi_mipi_dsc_enable(mipi_dsc_id dsc_id, para_input_s *inpara);

hi_s32 hi_mipi_dsc_disable(mipi_dsc_id dsc_id);

hi_s32 hi_mipi_set_dsc_config(mipi_dsc_id dsc_id, const dsc_cfg_s *dsc_cfg);

hi_s32 hi_mipi_dsc_get_status(mipi_dsc_id dsc_id);

void hi_mipi_dsc_deinit(mipi_dsc_id dsc_id);

#endif

