/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __MDC_CLIENT_COMM_H__
#define __MDC_CLIENT_COMM_H__

#include "vfmw.h"
#include "mdc_private.h"

typedef struct {
    hi_s32 (*init) (hi_void);
    hi_s32 (*deinit) (hi_void);
    hi_s32 (*transmit) (hi_void *, hi_u32);
    hi_s32 (*receive) (hi_void *, hi_u32);
    hi_s32 (*set_notify_cb)(notify_cb func);
} mdc_client_comm_ops;

typedef struct {
    hi_s32 cmd_ack;
    hi_void *args;
    hi_u32 length;
} mdc_client_comm_ack_info;

typedef struct {
    mdc_client_comm_ack_info ack_info;
    hi_u32 send_time_stamp;
    hi_bool time_out_flag;
    hi_s32 time_out_count;
    hi_u8 *sys_cnt_base_vir; /* map mtimer to be a time sync cnt to test delay */
    OS_SEMA sema;
    OS_SEMA send_sema;
    mdc_client_comm_ops *ops;
    mdc_comm_intf report_func;
    hi_void *tmp_comm_share_mem;
    hi_u8 buf[MDC_DATA_MAX_LEN];

    /* proc info */
    hi_u32 send_cnt;
    hi_u32 send_pos[MDC_DBG_CNT];
    hi_u32 get_ack_pos[MDC_DBG_CNT];
    mdc_msg_base send_data[MDC_DBG_CNT];
    hi_u32 recv_cnt;
    mdc_msg_base recv_data[MDC_DBG_CNT];
    hi_u32 recv_pos[MDC_DBG_CNT];
} mdc_client_comm_ctx;

mdc_client_comm_ctx *mdc_client_comm_get_ctx(hi_void);
hi_s32 mdc_client_comm_open(mdc_comm_intf func);
hi_s32 mdc_client_comm_close(hi_void);
hi_s32 mdc_client_comm_init(hi_void);
hi_s32 mdc_client_comm_exit(hi_void);
hi_s32 mdc_client_comm_send_cmd(comm_cmd_param *param);
hi_s32 mdc_client_comm_send_cmd_block(comm_cmd_param *param);
hi_s32 mdc_client_comm_check_state(hi_void);

#endif
