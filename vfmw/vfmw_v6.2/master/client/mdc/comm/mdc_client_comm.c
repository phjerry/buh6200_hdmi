/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "mdc_client_comm.h"
#include "dbg.h"
#include "vfmw_sys.h"
#include "vfmw_proc.h"
#ifdef VFMW_PROC_SUPPORT
#include "mdc_client_proc.h"
#endif
#ifdef VFMW_IPCM_SUPPORT
#include "mdc_client_ipcm.h"
#endif
#ifdef VFMW_MAILBOX_SUPPORT
#include "mdc_client_mailbox.h"
#endif

#define COMM_TIMER_US_PERSENT 12
#define COMM_TIME_OUT_MS      5000 /* FPGA NEED MORE */
#define COMM_TIME_OUT_TIMES   4

STATIC mdc_client_comm_ops g_mdc_client_comm[] = {
#ifdef VFMW_IPCM_SUPPORT
    {
        .init = ipcm_init,
        .deinit = ipcm_deinit,
        .transmit = ipcm_sendmsg,
        .receive = ipcm_readmsg,
        .set_notify_cb = ipcm_set_notify_cb,
    },
#endif
#ifdef VFMW_MAILBOX_SUPPORT
    {
        .init = mdc_clt_mailbox_init,
        .deinit = mdc_clt_mailbox_deinit,
        .transmit = mdc_clt_mailbox_send,
        .receive = mdc_clt_mailbox_read,
        .set_notify_cb = mdc_clt_mailbox_set_notify_cb,
    },
#endif
};

static mdc_client_comm_ctx g_mdc_client_comm_ctx;

#define mdc_client_comm_get_ops(ops) \
    do {                     \
        (ops) = &g_mdc_client_comm[0]; \
    } while (0)

mdc_client_comm_ctx *mdc_client_comm_get_ctx(hi_void)
{
    return &g_mdc_client_comm_ctx;
}

/* ----------------------------INTERNAL FUNCTION------------------------------- */
hi_s32 mdc_client_comm_read(hi_void *data, hi_u32 len);
STATIC hi_s32 mdc_client_comm_local_isr(hi_void)
{
    mdc_client_comm_ctx *ctx = HI_NULL;
    mdc_msg *msg = HI_NULL;
    comm_cmd_param param;
    hi_s32 ret;

    ctx = mdc_client_comm_get_ctx();

    ret = mdc_client_comm_read(ctx->buf, MDC_DATA_MAX_LEN);
    if (ret == HI_SUCCESS) {
        msg = (mdc_msg *)ctx->buf;

        if (msg->msg_base.ops.data_type == MDC_DATA_TYPE_ACK) {
            if (ctx->time_out_flag != HI_TRUE) {
                ctx->ack_info.cmd_ack = msg->msg_base.ret_val;
                ctx->ack_info.length = msg->msg_base.args_length;
                if (msg->msg_base.ops.args_in_msg) {
                    ctx->ack_info.args = msg->msg_args.mdc_args_buf;
                } else {
                    ctx->ack_info.args = HI_NULL;
                }
                OS_SEMA_UP(ctx->sema);
            }
        } else {
            param.chan_id = (hi_s32)msg->msg_base.chan_id;
            param.service_id = msg->msg_base.service_id;
            param.command_id = msg->msg_base.command_id;
            param.args = msg->msg_args.mdc_args_buf;
            param.length = msg->msg_base.args_length;
            ctx->report_func(&param);
        }
    }

    return HI_SUCCESS;
}

hi_s32 mdc_client_comm_check_state(hi_void)
{
    mdc_client_comm_ctx *ctx;
    hi_s32 pre;
    hi_s32 cur;
    hi_s32 diff;

    ctx = mdc_client_comm_get_ctx();
    cur = OS_GET_TIME_MS();
    VFMW_ASSERT_RET(ctx != HI_NULL, HI_FAILURE);

    pre = ctx->send_time_stamp + ctx->time_out_count * COMM_TIME_OUT_MS;
    diff = cur - pre;
    if (pre != 0 && diff > COMM_TIME_OUT_MS && ctx->time_out_flag == HI_FALSE) {
        ctx->time_out_count++;
        dprint(PRN_ALWS, "mdc no response for %d ms %d/%d\n",
               COMM_TIME_OUT_MS,  ctx->time_out_count,  COMM_TIME_OUT_TIMES);
        if (ctx->time_out_count >= COMM_TIME_OUT_TIMES) {
            ctx->time_out_flag = HI_TRUE;
            ctx->ack_info.cmd_ack = HI_FAILURE;
            OS_SEMA_UP(ctx->sema);
            dprint(PRN_ALWS, "mdc comm time out %d ms\n", COMM_TIME_OUT_MS * COMM_TIME_OUT_TIMES);
        }
    }

    return HI_SUCCESS;
}

/* ----------------------------EXTERNAL FUNCTION---------------------------- */
hi_s32 mdc_client_comm_open(mdc_comm_intf func)
{
    mdc_client_comm_ctx *ctx;
    hi_s32 ret;

    ctx = mdc_client_comm_get_ctx();
    VFMW_CHECK_SEC_FUNC(memset_s(ctx, sizeof(mdc_client_comm_ctx), 0, sizeof(mdc_client_comm_ctx)));

    mdc_client_comm_get_ops(ctx->ops);

    ret = ctx->ops->init();

    ctx->report_func = func;

    OS_SEMA_INIT(&ctx->sema);

    OS_SEMA_DOWN(ctx->sema);

    OS_SEMA_INIT(&ctx->send_sema);

    ctx->sys_cnt_base_vir = OS_KMAP_REG(SYS_CNT_REG_BASE, 0x10);

    ctx->ops->set_notify_cb(mdc_client_comm_local_isr);

    return ret;
}

hi_s32 mdc_client_comm_close(hi_void)
{
    mdc_client_comm_ctx *ctx;
    hi_s32 ret;

    ctx = mdc_client_comm_get_ctx();

    OS_KUNMAP_REG(ctx->sys_cnt_base_vir);

    OS_SEMA_EXIT(ctx->send_sema);

    OS_SEMA_EXIT(ctx->sema);

    ret = ctx->ops->deinit();

    VFMW_CHECK_SEC_FUNC(memset_s(ctx, sizeof(mdc_client_comm_ctx), 0, sizeof(mdc_client_comm_ctx)));

    return ret;
}

hi_s32 mdc_client_comm_init(hi_void)
{
    return HI_SUCCESS;
}

hi_s32 mdc_client_comm_exit(hi_void)
{
    return HI_SUCCESS;
}

hi_s32 mdc_client_comm_send(hi_void *data, hi_u32 len, hi_bool need_ack)
{
    mdc_client_comm_ctx *ctx;
    hi_s32 ret;
    mdc_msg *msg = (mdc_msg *)data;

    ctx = mdc_client_comm_get_ctx();

    msg->msg_base.ops.need_ack = need_ack;
    msg->msg_base.ops.unid = OS_GET_UNID();

    if (need_ack) {
        if (ctx->time_out_flag == HI_TRUE || ctx->time_out_count != 0) {
            dprint(PRN_ERROR, "last time send time out %d ms!!!", COMM_TIME_OUT_MS * ctx->time_out_count);
            ctx->time_out_flag = HI_FALSE;
            ctx->time_out_count = 0;
        }
    }

    ctx->send_pos[ctx->send_cnt % MDC_DBG_CNT] = OS_GET_TIME_US();
    VFMW_CHECK_SEC_FUNC(memcpy_s(&ctx->send_data[ctx->send_cnt % MDC_DBG_CNT],
                                 sizeof(mdc_msg_base), msg, sizeof(mdc_msg_base)));
    ctx->get_ack_pos[ctx->send_cnt % MDC_DBG_CNT] = 0;
    if (!need_ack) {
        ctx->send_cnt++;
    }

    ret = ctx->ops->transmit(data, len);
    if (ret == len && need_ack) {
        ctx->send_time_stamp = OS_GET_TIME_MS();
        OS_SEMA_DOWN(ctx->sema);
        ctx->send_time_stamp = 0;
        ctx->get_ack_pos[ctx->send_cnt % MDC_DBG_CNT] = OS_GET_TIME_US();
        ctx->send_cnt++;
        if (ctx->ack_info.args != HI_NULL && ctx->ack_info.length != 0) {
            VFMW_CHECK_SEC_FUNC(memcpy_s(msg->msg_args.mdc_args_buf,
                                         sizeof(msg->msg_args.mdc_args_buf), ctx->ack_info.args, ctx->ack_info.length));
        }
        return ctx->ack_info.cmd_ack;
    }

    return ret == len ? HI_SUCCESS : HI_FAILURE;
}

hi_s32 mdc_client_comm_read(hi_void *data, hi_u32 len)
{
    hi_s32 ret;
    mdc_client_comm_ctx *ctx = HI_NULL;
    mdc_msg_base *msg = HI_NULL;

    ctx = mdc_client_comm_get_ctx();

    ret = ctx->ops->receive(data, len);
    if (ret > 0) {
        msg = (mdc_msg_base *)data;
        ctx->recv_pos[ctx->recv_cnt % MDC_DBG_CNT] = OS_GET_TIME_US();
        VFMW_CHECK_SEC_FUNC(memcpy_s(&ctx->recv_data[ctx->recv_cnt % MDC_DBG_CNT],
                                     sizeof(mdc_msg_base), msg, sizeof(mdc_msg_base)));
        ctx->recv_cnt++;
    }

    return ret > 0 ? HI_SUCCESS : HI_FAILURE;
}

hi_s32 mdc_client_comm_send_cmd(comm_cmd_param *param)
{
    hi_s32 ret;
    hi_u32 send_length;
    mdc_msg msg = {{0}, {{0}}};
    mdc_client_comm_ctx *ctx;

    ctx = mdc_client_comm_get_ctx();

    OS_SEMA_DOWN(ctx->send_sema);

    msg.msg_base.chan_id = (hi_s16)param->chan_id;
    msg.msg_base.service_id = (hi_u16)param->service_id;
    msg.msg_base.command_id = (hi_u16)param->command_id;

    if (param->length <= MDC_MSG_ARGS_SIZE_MAX) {
        if (param->length != 0) {
            VFMW_CHECK_SEC_FUNC(memcpy_s(msg.msg_args.mdc_args_buf,
                                         sizeof(msg.msg_args.mdc_args_buf), param->args, param->length));
            msg.msg_base.args_length = param->length;
            msg.msg_base.ops.args_in_msg = 1;
        }
        send_length = MDC_MSG_BASE_LEN + param->length;
    } else {
        dprint(PRN_ERROR, "%s length 0x%x too large!!!\n", __func__, param->length);
        OS_SEMA_UP(ctx->send_sema);
        return HI_FAILURE;
    }

    ret = mdc_client_comm_send(&msg, send_length, 0);

    OS_SEMA_UP(ctx->send_sema);

    return ret;
}

hi_s32 mdc_client_comm_send_cmd_block(comm_cmd_param *param)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32  send_length;
    mdc_msg msg = {{0}, {{0}}};
    mdc_client_comm_ctx *ctx;

    ctx = mdc_client_comm_get_ctx();

    msg.msg_base.chan_id = (hi_s16)param->chan_id;
    msg.msg_base.service_id = (hi_u16)param->service_id;
    msg.msg_base.command_id = (hi_u16)param->command_id;

    OS_SEMA_DOWN(ctx->send_sema);

    if (param->length <= MDC_MSG_ARGS_SIZE_MAX) {
        if (param->length != 0) {
            VFMW_CHECK_SEC_FUNC(memcpy_s(msg.msg_args.mdc_args_buf,
                                         sizeof(msg.msg_args.mdc_args_buf), param->args, param->length));
            msg.msg_base.args_length = param->length;
            msg.msg_base.ops.args_in_msg = 1;
        }
        send_length = MDC_MSG_BASE_LEN + param->length;
    } else if (param->length <= param->comm_mem_length && param->valid_comm_mem != HI_NULL) {
        VFMW_CHECK_SEC_FUNC(memcpy_s(param->valid_comm_mem, param->length, param->args, param->length));
        msg.msg_base.args_length = param->length;
        msg.msg_base.ops.args_in_shr = 1;
        send_length = MDC_MSG_BASE_LEN;
    } else {
        dprint(PRN_ERROR, "%s args_length 0x%x, comm_mem = 0x%p, comm_mem_length 0x%x error!!!\n",
               __func__, param->length, param->valid_comm_mem, param->comm_mem_length);
        goto out;
    }

    ret = mdc_client_comm_send(&msg, send_length, 1);

    if (msg.msg_base.ops.args_in_msg == 1) {
        VFMW_CHECK_SEC_FUNC(memcpy_s(param->args, param->length, msg.msg_args.mdc_args_buf, param->length));
    } else if (msg.msg_base.ops.args_in_shr == 1) {
        VFMW_CHECK_SEC_FUNC(memcpy_s(param->args, param->length, param->valid_comm_mem, param->length));
    } else {
    }

out:
    OS_SEMA_UP(ctx->send_sema);

    return ret;
}
