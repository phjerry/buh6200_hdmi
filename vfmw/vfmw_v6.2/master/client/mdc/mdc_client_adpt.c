/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "mdc_client_adpt.h"
#include "dbg.h"
#include "vfmw_osal.h"
#include "vfmw_sys.h"
#include "mdc_client_comm.h"
#include "hal_mdc.h"

STATIC mdc_adpt_ops g_mdc_ops[] = {
    {
        .load = mdc_hal_load,
        .unload = mdc_hal_unload,
        .low_power_check = mdc_hal_is_wfi,
        .print_state = mdc_hal_print_mdc_state,
        .get_base_time = mdc_hal_get_base_time,
        .dump_reg = mdc_hal_dump_reg,
        .boot_up_ok = 0,
        .reg_base_addr = MDC0_BASE_ADDR,
    },
};

STATIC mdc_adpt_ctx g_mdc_adpt_ctx;

mdc_adpt_ctx *mdc_adpt_get_ctx(hi_void)
{
    return &g_mdc_adpt_ctx;
}

hi_s32 mdc_adpt_send(comm_cmd_param *param)
{
    mdc_adpt_ctx *ctx = mdc_adpt_get_ctx();

    /* reserved for multi mdc use mdc_index to replace 0 */
    VFMW_ASSERT_RET(ctx->ops[0].boot_up_ok == 1, HI_FAILURE);

    return mdc_client_comm_send_cmd(param);
}

hi_s32 mdc_adpt_send_block(comm_cmd_param *param)
{
    mdc_adpt_ctx *ctx = mdc_adpt_get_ctx();

    /* reserved for multi mdc use mdc_index to replace 0 */
    VFMW_ASSERT_RET(ctx->ops[0].boot_up_ok == 1, HI_FAILURE);

    return mdc_client_comm_send_cmd_block(param);
}

STATIC hi_s32 mdc_adpt_connect(mdc_comm_intf func)
{
    hi_s32 ret;

    ret = mdc_client_comm_open(func);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "%s mdc_client_comm_open failed ret = %d\n", __func__, ret);
        return ret;
    }

    return ret;
}

hi_s32 mdc_adpt_low_power_check(hi_void)
{
    hi_s32 ret = HI_SUCCESS;
    mdc_adpt_ctx *ctx = mdc_adpt_get_ctx();
    mdc_adpt_ops *ops = ctx->ops;
    hi_s32 i = 0;

    for (i = 0; i < ctx->mdc_num; i++) {
        ret |= ops[i].low_power_check();
    }

    return ret;
}

STATIC hi_s32 mdc_adpt_disconnect(hi_void)
{
    hi_s32 ret;
    comm_cmd_param param;
    mdc_adpt_ctx *ctx = mdc_adpt_get_ctx();

    param.chan_id = 0;
    param.service_id = MDC_SID_ENTER_WFI;
    param.command_id = 0;
    param.args = HI_NULL;
    param.length = 0;
    param.valid_comm_mem = ctx->comm_share_mem;
    param.comm_mem_length = ctx->comm_share_mem_size;
    ret = mdc_adpt_send(&param);
    if (ret == HI_SUCCESS) {
        (hi_void)mdc_adpt_low_power_check();
    }

    ret = mdc_client_comm_close();

    return ret;
}

STATIC hi_s32 mdc_adpt_load(hi_void)
{
    hi_s32 ret = HI_SUCCESS;
    mdc_adpt_ctx *ctx = mdc_adpt_get_ctx();
    mdc_adpt_ops *ops = ctx->ops;
    hi_s32 i = 0;

    for (i = 0; i < ctx->mdc_num; i++) {
        ret = ops[i].load();
        VFMW_ASSERT_RET_PRNT(ret != HI_FAILURE, ret, "i = %d\n", i);
    }

    return HI_SUCCESS;
}

STATIC hi_s32 mdc_adpt_unload(hi_void)
{
    hi_s32 ret = HI_SUCCESS;
    mdc_adpt_ctx *ctx = mdc_adpt_get_ctx();
    mdc_adpt_ops *ops = ctx->ops;
    hi_s32 i = 0;

    for (i = 0; i < ctx->mdc_num; i++) {
        ret = ops[i].unload();
        VFMW_ASSERT_RET_PRNT(ret != HI_FAILURE, ret, "i = %d\n", i);
        ops[i].boot_up_ok = 0;
    }

    return HI_SUCCESS;
}

hi_s32 mdc_adpt_open(mdc_comm_intf func)
{
    hi_s32 ret;
    mdc_adpt_ctx *ctx = mdc_adpt_get_ctx();

    VFMW_CHECK_SEC_FUNC(memset_s(ctx, sizeof(mdc_adpt_ctx), 0, sizeof(mdc_adpt_ctx)));

    ctx->ops = g_mdc_ops;
    ctx->mdc_num = sizeof(g_mdc_ops) / sizeof(mdc_adpt_ops);
    DPRINT_DBG("%s open %d mdc\n", __func__, ctx->mdc_num);

    ret = mdc_adpt_connect(func);
    VFMW_ASSERT_RET(ret != HI_FAILURE, ret);

    ret = mdc_adpt_load();
    VFMW_ASSERT_RET(ret != HI_FAILURE, ret);

    return ret;
}

hi_s32 mdc_adpt_close(hi_void)
{
    hi_s32 ret;

    ret = mdc_adpt_disconnect();
    VFMW_ASSERT_RET(ret != HI_FAILURE, ret);

    ret = mdc_adpt_unload();
    VFMW_ASSERT_RET(ret != HI_FAILURE, ret);

    return ret;
}

hi_s32 mdc_adpt_suspend(hi_void *args, hi_u32 length)
{
    hi_s32 ret;
    comm_cmd_param param;
    mdc_adpt_ctx *ctx = mdc_adpt_get_ctx();

    param.chan_id = 0;
    param.service_id = MDC_SID_SUSPEND;
    param.command_id = 0;
    param.args = args;
    param.length = length;
    param.valid_comm_mem = ctx->comm_share_mem;
    param.comm_mem_length = ctx->comm_share_mem_size;
    ret = mdc_adpt_send(&param);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "%s err ret = %d\n", __func__, ret);
        return ret;
    }

    ret = mdc_adpt_low_power_check();

    return ret;
}

hi_s32 mdc_adpt_resume(hi_void *args, hi_u32 length)
{
    hi_s32 ret;
    comm_cmd_param param;
    mdc_adpt_ctx *ctx = mdc_adpt_get_ctx();

    param.chan_id = 0;
    param.service_id = MDC_SID_RESUME;
    param.command_id = 0;
    param.args = args;
    param.length = length;
    param.valid_comm_mem = ctx->comm_share_mem;
    param.comm_mem_length = ctx->comm_share_mem_size;
    ret = mdc_adpt_send_block(&param);

    return ret;
}

hi_s32 mdc_adpt_init(hi_void *comm_shr_mem, hi_u32 mem_length)
{
    mdc_adpt_ctx *ctx = mdc_adpt_get_ctx();

    ctx->comm_share_mem = comm_shr_mem;
    ctx->comm_share_mem_size = mem_length;

    return mdc_client_comm_init();
}

hi_s32 mdc_adpt_exit(hi_void)
{
    mdc_adpt_ctx *ctx = mdc_adpt_get_ctx();

    ctx->comm_share_mem = HI_NULL;
    ctx->comm_share_mem_size = 0;

    return mdc_client_comm_exit();
}

hi_s32 mdc_adpt_check_state(hi_void)
{
    return mdc_client_comm_check_state();
}

hi_s32 mdc_adpt_print_state(hi_void)
{
    mdc_adpt_ctx *ctx = mdc_adpt_get_ctx();
    mdc_adpt_ops *ops = ctx->ops;
    hi_s32 i = 0;

    for (i = 0; i < ctx->mdc_num; i++) {
        ops[i].print_state();
    }

    return HI_SUCCESS;
}

hi_s32 mdc_adpt_set_boot_up(hi_u32 mdc_index)
{
    mdc_adpt_ctx *ctx = mdc_adpt_get_ctx();
    mdc_adpt_ops *ops = HI_NULL;

    if (mdc_index >= ctx->mdc_num) {
        dprint(PRN_FATAL, "%s faild mdc invalid index = %d\n", __func__, mdc_index);
        return HI_FAILURE;
    }

    ops = ctx->ops;
    ops[mdc_index].boot_up_ok = 1;

    return HI_SUCCESS;
}
