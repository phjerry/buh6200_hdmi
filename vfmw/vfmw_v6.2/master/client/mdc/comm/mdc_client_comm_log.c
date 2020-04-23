/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "mdc_client_comm_log.h"
#include "mdc_client_comm.h"

hi_s32 mdc_log_read_client_comm(hi_void *buf, hi_s32 chan_id)
{
    mdc_client_comm_ctx *ctx = mdc_client_comm_get_ctx();
    hi_u32 used = 0;
    hi_s32 len = 0;
    mdc_msg_base *msg = HI_NULL;
    hi_u32 i;

    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_HEAD, "MDC-CLT-COMM");

    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_HEAD, "TRANSMIT MSG");
    OS_DUMP_PROC(buf, len, &used, 0, "  %-8s %-8s %-8s %-8s %-8s %-8s %-8s \n",
                 "unid", "SID", "CID", "need_ack", "args_len", "ret_val", "cost(us)");

    for (i = 0; i < MDC_DBG_CNT; i++) {
        msg = &ctx->send_data[i];
        OS_DUMP_PROC(buf, len, &used, 0, "  %-8d %-8d %-8d %-8d %-8d %-8d %-8d \n", msg->ops.unid,
                     msg->service_id, msg->command_id, msg->ops.need_ack, msg->args_length, msg->ret_val,
                     ctx->get_ack_pos[i] == 0 ? 0 : ctx->get_ack_pos[i] - ctx->send_pos[i]);
    }

    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_HEAD, "RECEIVE MSG");
    OS_DUMP_PROC(buf, len, &used, 0, "  %-8s %-8s %-8s %-8s %-8s %-8s \n",
                 "unid", "SID", "CID", "msg_type", "args_len", "pos(us)");

    for (i = 0; i < MDC_DBG_CNT; i++) {
        msg = &ctx->recv_data[i];
        OS_DUMP_PROC(buf, len, &used, 0, "  %-8d %-8d %-8d %-8d %-8d %-8d \n", msg->ops.unid,
                     msg->service_id, msg->command_id, msg->ops.data_type, msg->args_length, ctx->recv_pos[i]);
    }

    return HI_SUCCESS;
}
