/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "mdc_client_adpt_log.h"
#include "mdc_client_adpt.h"

hi_s32 mdc_log_read_client_adapt(hi_void *buf, hi_s32 chan_id)
{
    hi_s32 used = 0;
    hi_u32 len = 0;
    hi_u32 i;
    mdc_adpt_ctx *ctx = mdc_adpt_get_ctx();
    mdc_adpt_ops *ops = ctx->ops;

    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_HEAD, "MDC-CLIEN-ADPT");
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_D, "total_mdc_num", ctx->mdc_num);
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_X_X,
                 "comm_mem", ctx->comm_share_mem, "comm_size", ctx->comm_share_mem_size);
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_HEAD, "MDC-DEV");

    if (ctx->mdc_num > 0 && ops != HI_NULL) {
        for (i = 0; i < ctx->mdc_num; i++) {
            OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_D, "mdc_num", i);
            OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_X_D, "base_addr", ops->reg_base_addr, "bootup", ops->boot_up_ok);
            ops[i].dump_reg(buf, len, &used);
        }
    }

    return HI_SUCCESS;
}
