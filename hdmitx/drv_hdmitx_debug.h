/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header file of debug submodule
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-10-29
 */

#ifndef __DRV_HDMITX_DEBUG_H__
#define __DRV_HDMITX_DEBUG_H__

#include "hi_type.h"
#include "hi_osal.h"

#define DEBUG_CMD_NUM_MAX  1024
#define DEBUG_MAX_ARGV_NUM 10
#define STRING_TO_DEC      10

typedef struct {
    osal_proc_cmd *cmd_list;
    hi_u32 cmd_cnt;
} cmd_msg;

struct hdmi_debug_cmd_arg {
    hi_u32 hdmi_id;
    hi_s8 *argv[DEBUG_MAX_ARGV_NUM];
    hi_u32 argc;
    hi_u32 remain_len;
};

typedef hi_s32 (*CMD_FUNC)(struct hisilicon_hdmi *hdmi, struct hdmi_debug_cmd_arg *cmd_arg);

struct hdmi_debug_cmd_info {
    hi_s8 *name;
    hi_s8 *short_name;
    CMD_FUNC cmd_func;
    hi_s8 *comment_help;
};

struct debug_avmute {
    hi_u32 wait_bef_stop;
    hi_u32 wait_aft_start;
};

struct hdmi_debug_info {
    struct debug_avmute d_avmute;
};

struct hdmitx_debug_msg {
    hi_u32 sub_cmd;
    void *data;
};

enum debug_ext_cmd_list {
    DEBUG_CMD_AVMUTE,
    DEBUG_CMD_CBAR,
    DEBUG_CMD_SCDC,
    DEBUG_CMD_HDMI_MODE,
    DEBUG_CMD_RC,
    DEBUG_CMD_DE,
    DEBUG_CMD_NULL_PACKET,
    DEBUG_CMD_FIFO_THRESHOLD,
};

enum sub_cmd_num {
    CMD_NUM_0,
    CMD_NUM_1,
    CMD_NUM_2,
    CMD_NUM_3,
    CMD_NUM_4,
    CMD_NUM_5,
    CMD_NUM_6,
    CMD_NUM_7,
    CMD_NUM_8,
    CMD_NUM_9,
    CMD_NUM_MAX
};

hi_s32 drv_hdmitx_debug_init(struct hisilicon_hdmi *hdmi);
hi_void drv_hdmitx_debug_deinit(struct hisilicon_hdmi *hdmi);
cmd_msg *drv_hdmitx_debug_get_cmd_list(hi_void);
#endif
