/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "mdc_client_mailbox.h"
#include "dbg.h"
#include "mdc_private.h"
#include "vfmw_sys.h"
#include "linux/hisilicon/hi_drv_mbx.h"

typedef struct {
    hi_s32 handle;
    notify_cb func;
} mailbox_clt_ctx_info;

#define TIME_OUT_SEND 0x200000 /* per 100us */
#define TIME_OUT_READ 5000

static mailbox_clt_ctx_info g_mailbox_clt_ctx = {0};

#define get_clt_mailbox_ctx(ctx)  \
        do {                        \
            (ctx) = &g_mailbox_clt_ctx; \
        } while (0)

static hi_void mdc_clt_mailbox_isr_process(hi_u32 irq, hi_void *dev_id)
{
    mailbox_clt_ctx_info *ctx = HI_NULL;

    get_clt_mailbox_ctx(ctx);

    if (ctx->func != HI_NULL) {
        ctx->func();
    }

    return;
}

hi_s32 mdc_clt_mailbox_init(hi_void)
{
    hi_s32 ret;
    mailbox_clt_ctx_info *ctx;

    get_clt_mailbox_ctx(ctx);

    VFMW_CHECK_SEC_FUNC(memset_s(ctx, sizeof(mailbox_clt_ctx_info), 0x00, sizeof(mailbox_clt_ctx_info)));

    ret = hi_drv_mbx_open(HI_MBX_ACPU2VMCU0_VFMW);
    if (ret < 0) {
        dprint(PRN_ERROR, "drv_mailbox_open %d failed\n", ret);
        return ret;
    }

    ctx->handle = ret;

    ret = hi_drv_mbx_register_irq_callback(ctx->handle, mdc_clt_mailbox_isr_process, HI_NULL);
    if (ret < 0) {
        dprint(PRN_ERROR, "mailbox_register_callback %d failed\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 mdc_clt_mailbox_deinit(hi_void)
{
    hi_s32 ret;
    mailbox_clt_ctx_info *ctx;

    get_clt_mailbox_ctx(ctx);

    ret = hi_drv_mbx_close(ctx->handle);
    if (ret < 0) {
        dprint(PRN_ERROR, "drv_mailbox_close %d failed\n", ret);
    }

    VFMW_CHECK_SEC_FUNC(memset_s(ctx, sizeof(mailbox_clt_ctx_info), 0x00, sizeof(mailbox_clt_ctx_info)));

    return ret;
}

hi_s32 mdc_clt_mailbox_send(hi_void *data, hi_u32 length)
{
    hi_s32 ret;
    mailbox_clt_ctx_info *ctx;
    hi_u32 tran_len = 0;

    get_clt_mailbox_ctx(ctx);

    ret = hi_drv_mbx_tx(ctx->handle, data, length, &tran_len, TIME_OUT_SEND);
    if (ret < 0 || tran_len != length) {
        dprint(PRN_ERROR, "drv_mailbox_tx ret = 0x%x length = %d tran_len = %d failed\n",
               ret, length, tran_len);
    }

    return tran_len;
}

hi_s32 mdc_clt_mailbox_read(hi_void *data, hi_u32 length)
{
    hi_s32 ret;
    mailbox_clt_ctx_info *ctx;
    hi_u32 recv_len = 0;

    get_clt_mailbox_ctx(ctx);
    ret = hi_drv_mbx_rx(ctx->handle, data, length, &recv_len, TIME_OUT_READ);
    if (ret < 0) {
        dprint(PRN_ERROR, "drv_mailbox_rx ret = 0x%x length = %d recv_len = %d failed\n",
               ret, length, recv_len);
    }

    return recv_len;
}

hi_s32 mdc_clt_mailbox_set_notify_cb(notify_cb func)
{
    mailbox_clt_ctx_info *ctx;

    get_clt_mailbox_ctx(ctx);

    ctx->func = func;

    return 0;
}
