/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "mdc_client_log.h"
#include "mdc_client.h"

hi_s32 mdc_log_read_client(hi_void *buf, hi_s32 chan_id)
{
    hi_s32 used = 0;
    hi_u32 len = 0;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_HEAD, "MDC-CLIENT");
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_D_D,
                 "is_inited", ctx->inited, "ctx_size", sizeof(mdc_ctrl_ctx_c));
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_X, "ree_pt_addr", ctx->ree_pt_addr);
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_X_X,
                 "seg_bug_size", ctx->seg_buf_size, "dev_hal_size", ctx->dev_hal_size);
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_X_X,
                 "chan_max_size", ctx->chan_max_size, "chan_shr_size", ctx->chan_share_mem_size);
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_D_D,
        "nsec_chan_num", ctx->created_chan_num[MDC_INDEX_NOR], "sec_chan_num", ctx->created_chan_num[MDC_INDEX_SEC]);
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_X_X,
                 "comm_mem", ctx->comm_share_mem, "comm_size", ctx->comm_share_mem_size);
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_D_D,
                 "wait_time", ctx->wait_time, "pro_evt_time", ctx->process_evt_time);
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_D_D,
                 "get_stm_time", ctx->get_stm_time, "rls_stm_time", ctx->rls_stm_time);
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_D, "chk_sta_time", ctx->chk_state_time);
    OS_DUMP_PROC(buf, len, &used, 0, "\n");

    return HI_SUCCESS;
}
