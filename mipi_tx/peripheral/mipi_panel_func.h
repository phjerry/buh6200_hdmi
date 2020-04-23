/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: mipi panel func module
* Author: sdk
* Create: 2019-11-20
*/
#ifndef __MIPI_PANEL_FUNC_H__
#define __MIPI_PANEL_FUNC_H__

#include "hi_type.h"

#define MAX_PANEL 2
#define MAX_CMD_LEN 20

typedef struct {
    hi_u32 cmd_type;
    hi_u32 data_type;
    hi_u32 delay;
    hi_u32 cmd_len;
    hi_u8 cmd[MAX_CMD_LEN];
} mipi_panel_cmd;

typedef enum {
    PANEL_ID_0,
    PANEL_ID_1
} panel_id;

hi_s32 get_power_on_cmd_num(panel_id id, hi_s32 *cmd_num);
hi_s32 get_power_off_cmd_num(panel_id id, hi_s32 *cmd_num);
hi_s32 get_power_on_cmd(panel_id id, mipi_panel_cmd *on_cmd, hi_s32 *cmd_len);
hi_s32 get_power_off_cmd(panel_id id, mipi_panel_cmd *off_cmd, hi_s32 *cmd_len);
hi_s32 panel_peripherial_init(panel_id id);
hi_void panel_peripherial_deinit(panel_id id);
#endif
