/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "mdc_client.h"
#include "vfmw_define.h"
#include "dbg.h"
#include "vfmw_osal.h"
#include "comm_share.h"
#include "client_image.h"
#include "client_event.h"
#include "client_state.h"
#include "client_stream.h"
#include "vfmw_pdt.h"
#include "vmm_ext.h"
#ifdef VFMW_PROC_SUPPORT
#include "mdc_client_proc.h"
#endif
#include "mdc_client_adpt.h"

#define MDC_MAX_NAME_LEN      50
#define MDC_MEM_MIN_ALIGN     4
#define MDC_MAX_REF_CNT       0x7fffffff
#define MDC_UN_NORMAL_REF_CNT 2

#ifdef VFMW_SEC_SUPPORT
#define MDC_SEC_ADDR_ATTR_CNT 6
#endif

#ifndef BUILD_BUG_ON
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
#endif

STATIC mdc_ctrl_ctx_c g_mdc_client_ctx;

mdc_ctrl_ctx_c *mdc_get_ctx(hi_void)
{
    return &g_mdc_client_ctx;
}

/* ----------------------------INTERNAL FUNCTION------------------------------- */
STATIC hi_void mdc_client_process_event(hi_void)
{
    hi_s32 id = 0;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    for (id = 0; id < VFMW_CHAN_NUM; id++) {
        if (ctx->chan_ctx[id].chan_state == CHAN_START) {
            OS_SEMA_DOWN(ctx->chan_ctx[id].sema);
            (hi_void)clt_evt_process(id);
            OS_SEMA_UP(ctx->chan_ctx[id].sema);
        }
    }

    return;
}

STATIC hi_void mdc_client_get_stream(hi_void)
{
    hi_s32 id = 0;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();;

    for (id = 0; id < VFMW_CHAN_NUM; id++) {
        if (ctx->chan_ctx[id].chan_state == CHAN_START) {
            OS_SEMA_DOWN(ctx->chan_ctx[id].sema);
            clt_stm_read(id);
            OS_SEMA_UP(ctx->chan_ctx[id].sema);
        }
    }

    return;
}

STATIC hi_void mdc_client_release_stream(hi_void)
{
    hi_s32 id = 0;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    for (id = 0; id < VFMW_CHAN_NUM; id++) {
        if (ctx->chan_ctx[id].chan_state == CHAN_START) {
            OS_SEMA_DOWN(ctx->chan_ctx[id].sema);
            (hi_void)clt_stm_release(id);
            OS_SEMA_UP(ctx->chan_ctx[id].sema);
        }
    }

    return;
}

STATIC hi_s32 mdc_client_set_call_back(vfmw_adpt_type type, fun_vfmw_event_report report)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    VFMW_ASSERT_RET(report != HI_NULL, HI_FAILURE);

    switch (type) {
        case ADPT_TYPE_VDEC:
            ctx->call_back.event_report_vdec = report;
            break;

        case ADPT_TYPE_OMX:
            ctx->call_back.event_report_omxvdec = report;
            break;

        default:
            dprint(PRN_ERROR, "%s Unkown Adapter Type: %d\n", __func__, type);
            return HI_FAILURE;
    }

    clt_evt_set_call_back(type, report);

    return HI_SUCCESS;
}

STATIC hi_s32 mdc_client_thread_func(hi_void)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    hi_u32 begin;
    hi_u32 end;

    DPRINT_INFO("Mdc client thread Start !\n");

    while (ctx->thread_ctx.state != MDC_THREAD_STA_EXIT) {
        while (ctx->thread_ctx.state == MDC_THREAD_STA_START) {
            begin = OS_GET_TIME_US();
            OS_EVENT_WAIT(ctx->thread_ctx.event, MDC_THREAD_WAIT_TIME);
            end = OS_GET_TIME_US();
            ctx->wait_time = end - begin;
            ctx->thread_ctx.work_pos = MDC_THREAD_WORKING;

            begin = OS_GET_TIME_US();
            mdc_client_process_event();
            end = OS_GET_TIME_US();
            ctx->process_evt_time = end - begin;

            begin = OS_GET_TIME_US();
            mdc_client_get_stream();
            end = OS_GET_TIME_US();
            ctx->get_stm_time = end - begin;

            begin = OS_GET_TIME_US();
            mdc_client_release_stream();
            end = OS_GET_TIME_US();
            ctx->rls_stm_time = end - begin;

            begin = OS_GET_TIME_US();
            mdc_adpt_check_state();
            end = OS_GET_TIME_US();
            ctx->chk_state_time = end - begin;
            ctx->thread_ctx.work_pos = MDC_THREAD_SLEEPING;
        }

        while (ctx->thread_ctx.state == MDC_THREAD_STA_STOP) {
            OS_MSLEEP(MDC_THREAD_WAIT_TIME);
        }
    }

    ctx->thread_ctx.state = MDC_THREAD_STA_BUTT;

    return HI_SUCCESS;
}

STATIC hi_s32 mdc_client_is_local(hi_s32 cmd)
{
    if (cmd == VFMW_CID_GET_CHAN_STATE
        || cmd == VFMW_CID_SET_STREAM_INTF
        || cmd == VFMW_CID_GET_CAPABILITY) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

STATIC hi_s32 mdc_client_local_control(hi_s32 chan_id, hi_s32 cmd, hi_void *args, hi_u32 param_length)
{
    hi_s32 ret = HI_FAILURE;

    VFMW_ASSERT_RET(args != HI_NULL, ret);

    switch (cmd) {
        case VFMW_CID_GET_CHAN_STATE:
            ret = clt_sta_get_chan_state(chan_id, (vfmw_chan_info *)args);
            break;

        case VFMW_CID_SET_STREAM_INTF:
            ret = clt_stm_set_intf(chan_id, (vfmw_stream_intf *)args);
            break;

        case VFMW_CID_GET_CAPABILITY:
            ret = pdt_get_attr(ATTR_GET_CAP, (vfmw_cap *)args);
            break;

        default:
            dprint(PRN_ERROR, "%s: unsupport command %d\n", __func__, cmd);
    }

    return ret;
}

STATIC hi_void mdc_client_part_chan(vfmw_shr_chn_mem *mem)
{
    vfmw_mem_desc *desc = &mem->chan_shr_mem_desc;

    /* chan_comm_share_mem must be the first part of shr mem */
    mem->share_comm_phy_addr = desc->phy_addr;
    mem->share_comm_vir_addr = desc->vir_addr;
    mem->img_phy_addr = mem->share_comm_phy_addr + MDC_COMM_CHAN_SHARE_MEM_LENGTH;
    mem->img_vir_addr = mem->share_comm_vir_addr + MDC_COMM_CHAN_SHARE_MEM_LENGTH;
    mem->stm_phy_addr = mem->img_phy_addr + SHR_IMG_LIST_SIZE;
    mem->stm_vir_addr = mem->img_vir_addr + SHR_IMG_LIST_SIZE;
    mem->evt_phy_addr = mem->stm_phy_addr + SHR_STM_LIST_SIZE;
    mem->evt_vir_addr = mem->stm_vir_addr + SHR_STM_LIST_SIZE;
    mem->sta_phy_addr = mem->evt_phy_addr + SHR_EVT_LIST_SIZE;
    mem->sta_vir_addr = mem->evt_vir_addr + SHR_EVT_LIST_SIZE;

    DPRINT_INFO("%s img_phy_addr 0x%x stm_phy_addr 0x%x evt_phy_addr 0x%x sta_phy_addr 0x%x share_comm_phy_addr 0x%x\n",
        __func__, mem->img_phy_addr, mem->stm_phy_addr, mem->evt_phy_addr, mem->sta_phy_addr, mem->share_comm_phy_addr);
}

STATIC hi_s32 mdc_client_alloc_chan_ctx(vfmw_mem_desc *mem)
{
    hi_u32 chan_mem_size;
    char name[MDC_MAX_NAME_LEN] = { 0 };
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    hi_s32 ret;

    chan_mem_size = ctx->seg_buf_size + ctx->chan_max_size - ctx->chan_share_mem_size;

    if (snprintf_s(name, sizeof(name), sizeof(name),
                   "vfmw_chan_ctx_mem_buf_%d", ctx->chan_ctx_mem_cnt % VFMW_CHAN_NUM) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    ret = OS_KMEM_ALLOC(name, chan_mem_size, MDC_MEM_MIN_ALIGN, 0, mem);
    VFMW_ASSERT_RET(ret != HI_FAILURE, ret);

    ctx->chan_ctx_mem_cnt++;

    return HI_SUCCESS;
}

STATIC hi_s32 mdc_client_alloc_chan_share(vfmw_shr_chn_mem *mem)
{
    char name[MDC_MAX_NAME_LEN] = { 0 };
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    hi_s32 ret;

    VFMW_CHECK_SEC_FUNC(memset_s(&mem->chan_shr_mem_desc, sizeof(vfmw_mem_desc), 0x00, sizeof(vfmw_mem_desc)));

    if (snprintf_s(name, sizeof(name), sizeof(name),
                   "vfmw_chan_share_mem_buf_%d", ctx->chan_share_mem_cnt % VFMW_CHAN_NUM) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    ret = OS_KMEM_ALLOC(name, ctx->chan_share_mem_size, MDC_MEM_MIN_ALIGN, 0, &mem->chan_shr_mem_desc);
    VFMW_ASSERT_RET(ret != HI_FAILURE, ret);

    mdc_client_part_chan(mem);

    ctx->chan_share_mem_cnt++;

    return HI_SUCCESS;
}

STATIC hi_s32 mdc_client_alloc_dev(vfmw_mem_desc *mem)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    hi_s32 ret;

    ret = OS_KMEM_ALLOC("dev_hal_mem", ctx->dev_hal_size, MDC_MEM_MIN_ALIGN, 0, mem);
    VFMW_ASSERT_RET(ret != HI_FAILURE, ret);

    return HI_SUCCESS;
}

STATIC hi_void mdc_client_free(vfmw_mem_desc *mem)
{
    if (mem->phy_addr != 0) {
        OS_KMEM_FREE(mem);
        VFMW_CHECK_SEC_FUNC(memset_s(mem, sizeof(vfmw_mem_desc), 0, sizeof(vfmw_mem_desc)));
    }

    return;
}

STATIC hi_void mdc_client_bind_share(hi_s32 chan_id, vfmw_shr_chn_mem *chan_share_mem)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    (hi_void)clt_img_bind_queue(chan_id, UINT64_PTR(chan_share_mem->img_vir_addr));
    (hi_void)clt_stm_bind_queue(chan_id, UINT64_PTR(chan_share_mem->stm_vir_addr));
    (hi_void)clt_evt_bind_queue(chan_id, UINT64_PTR(chan_share_mem->evt_vir_addr));
    (hi_void)clt_sta_bind_queue(chan_id, UINT64_PTR(chan_share_mem->sta_vir_addr));

    ctx->chan_ctx[chan_id].chan_comm_share_mem = UINT64_PTR(chan_share_mem->share_comm_vir_addr);
    ctx->chan_ctx[chan_id].chan_comm_share_mem_size = MDC_COMM_CHAN_SHARE_MEM_LENGTH;

    return;
}

STATIC hi_void mdc_client_unbind_share(hi_s32 chan_id)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    clt_img_unbind_queue(chan_id);
    clt_stm_unbind_queue(chan_id);
    clt_evt_unbind_queue(chan_id);
    clt_sta_unbind_queue(chan_id);

    ctx->chan_ctx[chan_id].chan_comm_share_mem = HI_NULL;
    ctx->chan_ctx[chan_id].chan_comm_share_mem_size = 0;

    return;
}

STATIC hi_void mdc_client_prepare_base(hi_s32 chan_id, vfmw_cid cmd_id)
{
    hi_ulong flag;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    if (cmd_id == VFMW_CID_RESET_CHAN || cmd_id == VFMW_CID_RESET_CHAN_WITH_OPTION || cmd_id == VFMW_CID_FLUSH_CHAN) {
        VFMW_ASSERT(ctx->chan_ctx[chan_id].valid == HI_TRUE);
        OS_SPIN_LOCK(ctx->chan_ctx[chan_id].chan_lock, &flag);
        (hi_void)clt_img_reset_queue(chan_id);
        OS_SPIN_UNLOCK(ctx->chan_ctx[chan_id].chan_lock, &flag);
    }
}

STATIC hi_void mdc_client_fill_create_param(hi_s32 chan_id, vfmw_shr_chn_mem *chan_share_mem, hi_s32 is_sec)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    BUILD_BUG_ON(sizeof(mdc_create_param) > MDC_MSG_ARGS_SIZE_MAX);

    ctx->chan_ctx[chan_id].create_param.phyaddr = chan_share_mem->chan_shr_mem_desc.phy_addr;
    ctx->chan_ctx[chan_id].create_param.length = chan_share_mem->chan_shr_mem_desc.length;
    ctx->chan_ctx[chan_id].create_param.mode = chan_share_mem->chan_shr_mem_desc.mode;
    ctx->chan_ctx[chan_id].create_param.is_sec = is_sec;
}

STATIC hi_s32 mdc_client_bef_create(hi_s32 chan_id, hi_void **args, hi_u32 *len)
{
    hi_s32 ret = HI_FAILURE;
    hi_s64 *array = HI_NULL;
    vfmw_chan_option *opt = HI_NULL;
    hi_s32 is_sec;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    VFMW_ASSERT_RET(*len >= (sizeof(hi_s64) + sizeof(hi_s64)), ret);

    VFMW_CHECK_SEC_FUNC(memset_s(&ctx->chan_ctx[chan_id], sizeof(mdc_client_chan), 0, sizeof(mdc_client_chan)));

    array = (hi_s64 *)*args;
    VFMW_CHECK_SEC_FUNC(memcpy_s((hi_s8 *)ctx->tmp_create_share_mem + chan_id * MDC_CREATE_ARGS_LENGTH,
                                 sizeof(vfmw_chan_option), array, sizeof(vfmw_chan_option)));

    opt = (vfmw_chan_option *)array;
    ctx->chan_ctx[chan_id].chan_opt_external = opt;

    is_sec = (opt->is_sec_mode != 0) ? 1 : 0;
    ctx->chan_ctx[chan_id].is_sec = is_sec;
    ctx->chan_ctx[chan_id].vdec_handle = opt->vdec_handle;
    ctx->chan_ctx[chan_id].ssm_handle = opt->ssm_handle;

    ctx->seg_buf_size = VFMW_ALIGN_UP(opt->seg_buf_size, VFMW_MEM_ALIGN);
    opt->chan_ctx_mem.mode = (is_sec == 1) ? MEM_MMU_SEC : 0;
    opt->chan_ctx_mem.vdec_handle = ctx->chan_ctx[chan_id].vdec_handle;
    opt->chan_ctx_mem.ssm_handle = ctx->chan_ctx[chan_id].ssm_handle;
    ret = mdc_client_alloc_chan_ctx(&opt->chan_ctx_mem);
    VFMW_ASSERT_RET(ret == HI_SUCCESS, HI_FAILURE);

    ret = mdc_client_alloc_chan_share(&opt->tmp_chan_shr_mem);
    VFMW_ASSERT_GOTO(ret == HI_SUCCESS, out2);

    if (ctx->created_chan_num[is_sec] == 0) {
        opt->dev_hal_mem.mode = (is_sec == 1) ? MEM_MMU_SEC : 0;
        opt->dev_hal_mem.vdec_handle = ctx->chan_ctx[chan_id].vdec_handle;
        opt->dev_hal_mem.ssm_handle = ctx->chan_ctx[chan_id].ssm_handle;
        ret = mdc_client_alloc_dev(&opt->dev_hal_mem);
        VFMW_ASSERT_GOTO(ret == HI_SUCCESS, out1);
    } else {
        opt->dev_hal_mem.phy_addr = 0;
    }

    mdc_client_bind_share(chan_id, &opt->tmp_chan_shr_mem);

    mdc_client_fill_create_param(chan_id, &opt->tmp_chan_shr_mem, is_sec);

    VFMW_CHECK_SEC_FUNC(memcpy_s(ctx->chan_ctx[chan_id].chan_comm_share_mem,
                                 sizeof(vfmw_chan_option), opt, sizeof(vfmw_chan_option)));

    *args = &ctx->chan_ctx[chan_id].create_param;
    *len = sizeof(ctx->chan_ctx[chan_id].create_param);

    return HI_SUCCESS;

out1:
    mdc_client_free(&opt->tmp_chan_shr_mem.chan_shr_mem_desc);
out2:
    mdc_client_free(&opt->chan_ctx_mem);

    VFMW_CHECK_SEC_FUNC(memset_s((hi_s8 *)ctx->tmp_create_share_mem + chan_id * MDC_CREATE_ARGS_LENGTH,
                                 MDC_CREATE_ARGS_LENGTH, 0x00, MDC_CREATE_ARGS_LENGTH));

    return ret;
}

STATIC hi_s32 mdc_client_bef_config(hi_s32 chan_id, hi_void *args, hi_u32 len)
{
    vfmw_chan_cfg *cfg = (vfmw_chan_cfg *)args;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    VFMW_ASSERT_RET((args != HI_NULL) && (len == sizeof(vfmw_chan_cfg)), HI_FAILURE);

    cfg->sec_flag = ctx->chan_ctx[chan_id].is_sec;

    return HI_SUCCESS;
}

STATIC hi_s32 mdc_client_bef_remote(hi_s32 chan_id, hi_s32 cmd, hi_void **args, hi_u32 *len)
{
    hi_s32 ret = HI_FAILURE;
    vfmw_cid cmd_id = (vfmw_cid)cmd;

    switch (cmd_id) {
        case VFMW_CID_CREATE_CHAN:
            ret = mdc_client_bef_create(chan_id, args, len);
            break;
        case VFMW_CID_CFG_CHAN:
            ret = mdc_client_bef_config(chan_id, *args, *len);
            break;
        default:
            DPRINT_DBG("cmd %d no need prepare\n", cmd_id);
            ret = HI_SUCCESS;
    }

    return ret;
}

STATIC hi_void mdc_client_aft_create(hi_s32 chan_id, hi_void **args, hi_u32 len)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    vfmw_chan_option *opt = HI_NULL;
    hi_s32 ack_value;
    hi_s32 is_sec;

    VFMW_ASSERT(args != HI_NULL);
    VFMW_ASSERT(*args != HI_NULL);

    opt = ctx->chan_ctx[chan_id].chan_opt_external;
    ack_value = ctx->chan_ctx[chan_id].ack_value;
    is_sec = ctx->chan_ctx[chan_id].is_sec;

    if (ack_value == HI_SUCCESS) {
        VFMW_CHECK_SEC_FUNC(memcpy_s(opt, sizeof(vfmw_chan_option),
                                     ctx->chan_ctx[chan_id].chan_comm_share_mem, sizeof(vfmw_chan_option)));

        ctx->chan_ctx[chan_id].chan_id = chan_id;
        ctx->chan_ctx[chan_id].is_sec = is_sec;

        OS_SEMA_INIT(&ctx->chan_ctx[chan_id].sema);
        OS_SPIN_LOCK_INIT(&ctx->chan_ctx[chan_id].chan_lock);

        VFMW_CHECK_SEC_FUNC(memcpy_s(&ctx->chan_ctx[chan_id].chan_ctx_mem,
                                     sizeof(vfmw_mem_desc), &opt->chan_ctx_mem, sizeof(vfmw_mem_desc)));
        VFMW_CHECK_SEC_FUNC(memcpy_s(&ctx->chan_ctx[chan_id].chan_share_mem,
                                     sizeof(vfmw_shr_chn_mem), &opt->tmp_chan_shr_mem, sizeof(vfmw_shr_chn_mem)));

        ctx->chan_ctx[chan_id].valid = HI_TRUE;

        if (ctx->created_chan_num[is_sec] == 0) {
            VFMW_CHECK_SEC_FUNC(memcpy_s(&ctx->dev_hal_mem[is_sec],
                                         sizeof(vfmw_mem_desc), &opt->dev_hal_mem, sizeof(vfmw_mem_desc)));
        }

        ctx->created_chan_num[is_sec]++;
    } else {
        mdc_client_unbind_share(chan_id);
        mdc_client_free(&opt->tmp_chan_shr_mem.chan_shr_mem_desc);
        mdc_client_free(&opt->chan_ctx_mem);
        if (ctx->created_chan_num[is_sec] == 0) {
            mdc_client_free(&opt->dev_hal_mem);
        }
    }

    opt->chan_id = chan_id;

    return;
}

STATIC hi_void mdc_client_aft_get_mem_size(hi_void *args, hi_u32 len)
{
}

STATIC hi_void mdc_client_aft_config(hi_s32 chan_id, hi_void *args, hi_u32 len)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    vfmw_chan_cfg *chan_cfg = (vfmw_chan_cfg *)args;

    VFMW_ASSERT(chan_cfg != HI_NULL);

    if (chan_cfg->private != 0 && ctx->chan_ctx[chan_id].ack_value == HI_SUCCESS) {
        clt_stm_set_intf(chan_id, (vfmw_stream_intf *)UINT64_PTR(chan_cfg->private));
    }
}

STATIC hi_void mdc_client_aft_start(hi_s32 chan_id)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    hi_s32 ack_value;

    ack_value = ctx->chan_ctx[chan_id].ack_value;

    if (ack_value == HI_SUCCESS) {
        ctx->chan_ctx[chan_id].chan_state = CHAN_START;
    }
}

STATIC hi_void mdc_client_aft_stop(hi_s32 chan_id)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    hi_s32 ack_value;

    ack_value = ctx->chan_ctx[chan_id].ack_value;

    if (ack_value == HI_SUCCESS) {
        ctx->chan_ctx[chan_id].chan_state = CHAN_STOP;
    }
}

STATIC hi_void mdc_client_aft_destroy(hi_s32 chan_id)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    hi_s32 is_sec;

    VFMW_ASSERT(ctx->chan_ctx[chan_id].valid != HI_FALSE);

    OS_SEMA_DOWN(ctx->chan_ctx[chan_id].sema);

    (hi_void)clt_evt_reset_queue(chan_id);

    (hi_void)clt_stm_reset_queue(chan_id);

    mdc_client_unbind_share(chan_id);

    mdc_client_free(&ctx->chan_ctx[chan_id].chan_share_mem.chan_shr_mem_desc);

    mdc_client_free(&ctx->chan_ctx[chan_id].chan_ctx_mem);

    is_sec = ctx->chan_ctx[chan_id].is_sec;
    ctx->created_chan_num[is_sec]--;

    if (ctx->created_chan_num[is_sec] <= 0) {
        mdc_client_free(&ctx->dev_hal_mem[is_sec]);
        ctx->created_chan_num[is_sec] = 0;
    }

    ctx->chan_ctx[chan_id].chan_state = CHAN_INVALID;
    ctx->chan_ctx[chan_id].valid = HI_FALSE;

    OS_SEMA_UP(ctx->chan_ctx[chan_id].sema);

    OS_SEMA_EXIT(ctx->chan_ctx[chan_id].sema);
    OS_SPIN_LOCK_EXIT(ctx->chan_ctx[chan_id].chan_lock);

    return;
}

STATIC hi_void mdc_client_aft_reset(hi_s32 chan_id, hi_s32 option, hi_void *args, hi_u32 len)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    VFMW_ASSERT(ctx->chan_ctx[chan_id].valid == HI_TRUE);

    OS_SEMA_DOWN(ctx->chan_ctx[chan_id].sema);

    (hi_void)clt_evt_reset_queue(chan_id);

    (hi_void)clt_stm_reset_queue(chan_id);

    OS_SEMA_UP(ctx->chan_ctx[chan_id].sema);

    return;
}

STATIC hi_void mdc_client_aft_flush_stream(hi_s32 chan_id)
{
    hi_s32 ret;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    OS_SEMA_DOWN(ctx->chan_ctx[chan_id].sema);

    ret = clt_stm_reset_queue(chan_id);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "flush stream fail!\n");
    }

    OS_SEMA_UP(ctx->chan_ctx[chan_id].sema);

    return;
}

STATIC hi_void mdc_client_aft_remote(hi_s32 chan_id, vfmw_cid cmd_id, hi_void **args, hi_u32 len)
{
    switch (cmd_id) {
        case VFMW_CID_CREATE_CHAN:
            mdc_client_aft_create(chan_id, args, len);
            break;

        case VFMW_CID_GET_CHAN_MEMSIZE:
            mdc_client_aft_get_mem_size(*args, len);
            break;

        case VFMW_CID_CFG_CHAN:
            mdc_client_aft_config(chan_id, *args, len);
            break;

        case VFMW_CID_START_CHAN:
            mdc_client_aft_start(chan_id);
            break;

        case VFMW_CID_STOP_CHAN:
            mdc_client_aft_stop(chan_id);
            break;

        case VFMW_CID_DESTROY_CHAN:
            mdc_client_aft_destroy(chan_id);
            break;

        case VFMW_CID_FLUSH_CHAN:
        case VFMW_CID_RESET_CHAN:
            mdc_client_aft_reset(chan_id, HI_FALSE, *args, len);
            break;

        case VFMW_CID_RESET_CHAN_WITH_OPTION:
            mdc_client_aft_reset(chan_id, HI_TRUE, *args, len);
            break;
        case VFMW_CID_RELEASE_STREAM:
            mdc_client_aft_flush_stream(chan_id);
            break;

        default:
            DPRINT_DBG("noting to do\n");
    }

    return;
}

STATIC hi_s32 mdc_client_remote_control(hi_s32 chan_id, hi_s32 cmd, hi_void *args, hi_u32 len)
{
    hi_s32 ret;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    comm_cmd_param param;

    mdc_client_prepare_base(chan_id, cmd);

    ret = mdc_client_bef_remote(chan_id, cmd, &args, &len);
    VFMW_ASSERT_RET(ret != HI_FAILURE, ret);

    param.chan_id = chan_id;
    param.service_id = MDC_SID_CONTROL;
    param.command_id = cmd;
    param.args = args;
    param.length = len;
    param.valid_comm_mem = ctx->chan_ctx[chan_id].chan_comm_share_mem;
    param.comm_mem_length = ctx->chan_ctx[chan_id].chan_comm_share_mem_size;
    ret = mdc_adpt_send_block(&param);

    ctx->chan_ctx[chan_id].ack_value = ret;

    mdc_client_aft_remote(chan_id, cmd, &args, len);

    return ret;
}

STATIC hi_s32 mdc_client_create_thread(hi_void)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    hi_s32 ret;

    ret = OS_EVENT_INIT(&ctx->thread_ctx.event, 0);
    VFMW_ASSERT_RET(ret != HI_FAILURE, ret);

    ctx->thread_ctx.state = MDC_THREAD_STA_START;
    ctx->thread_ctx.work_pos = MDC_THREAD_WORKING;
    ctx->thread_ctx.run_cnt = 0;
    ctx->thread_ctx.handle = OS_CREATE_THREAD(mdc_client_thread_func, HI_NULL, "hi_vfmw_mdc");
    VFMW_ASSERT_GOTO(ctx->thread_ctx.handle != HI_NULL, out);

    return HI_SUCCESS;

out:
    OS_EVENT_EXIT(ctx->thread_ctx.event);
    return HI_FAILURE;
}

STATIC hi_s32 mdc_client_destroy_thread(hi_void)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    hi_u32 try_time = 0;

    VFMW_ASSERT_RET(ctx->thread_ctx.handle != HI_NULL, HI_FAILURE);

    ctx->thread_ctx.state = MDC_THREAD_STA_EXIT;
    OS_EVENT_GIVE(ctx->thread_ctx.event);

    while (ctx->thread_ctx.state != MDC_THREAD_STA_BUTT) {
        if (try_time++ >= MDC_THREAD_DESTROY_TRY_TIME) {
            break;
        } else {
            OS_MSLEEP(1);
        }
    }
    OS_EVENT_EXIT(ctx->thread_ctx.event);

    VFMW_ASSERT_RET(try_time < MDC_THREAD_DESTROY_TRY_TIME, HI_FAILURE);

    return HI_SUCCESS;
}

STATIC hi_void mdc_client_part_glb_share(shr_glb_mem *mem)
{
    if (SHR_PROC_SIZE > 0) {
        mem->proc_phy_addr = mem->glb_shr_mem_desc.phy_addr;
        mem->proc_vir_addr = mem->glb_shr_mem_desc.vir_addr;
    }

    mem->comm_shrmem_phy_addr = mem->glb_shr_mem_desc.phy_addr + SHR_PROC_SIZE;
    mem->comm_shrmem_vir_addr = mem->glb_shr_mem_desc.vir_addr + SHR_PROC_SIZE;

    mem->create_args_phy_addr = mem->glb_shr_mem_desc.phy_addr + SHR_PROC_SIZE + MDC_COMM_SHARE_MEM_LENGTH;
    mem->create_args_vir_addr = mem->glb_shr_mem_desc.vir_addr + SHR_PROC_SIZE + MDC_COMM_SHARE_MEM_LENGTH;

    return;
}

STATIC hi_s32 mdc_client_alloc_glb_share_mem(hi_void)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    hi_s32 size;
    hi_s32 ret;

    size = VFMW_ALIGN_UP(SHR_GLBMEM_SIZE, MDC_MEM_ALIGN_SIZE);

    ret = OS_KMEM_ALLOC("vfmw_glb_share_mem", size, MDC_MEM_MIN_ALIGN, 0, &ctx->glb_share_mem.glb_shr_mem_desc);
    VFMW_ASSERT_RET(ret != HI_FAILURE, ret);

    DPRINT_DBG("vfmw_glb_share_mem vir 0x%x phy 0x%x size 0x%x GlobalMemSize 0x%x\n",
               (hi_u32)ctx->glb_share_mem.glb_shr_mem_desc.vir_addr, ctx->glb_share_mem.glb_shr_mem_desc.phy_addr,
               ctx->glb_share_mem.glb_shr_mem_desc.length, size);

    mdc_client_part_glb_share(&ctx->glb_share_mem);

    ctx->proc = (hi_s8 *)UINT64_PTR(ctx->glb_share_mem.proc_vir_addr);
    ctx->comm_share_mem = (hi_void *)UINT64_PTR(ctx->glb_share_mem.comm_shrmem_vir_addr);
    ctx->comm_share_mem_size = MDC_COMM_SHARE_MEM_LENGTH;
    ctx->tmp_create_share_mem = UINT64_PTR(ctx->glb_share_mem.create_args_vir_addr);
    ctx->tmp_create_share_mem_size = MDC_COMM_TMP_SHR_MEM_LENGTH;

    DPRINT_DBG("%s proc 0x%x comm_shr 0x%x create_shr 0x%x\n", __func__,
               ctx->proc, ctx->comm_share_mem, ctx->tmp_create_share_mem);

    return HI_SUCCESS;
}

STATIC hi_void mdc_client_free_glb_share_mem(hi_void)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    mdc_client_free(&ctx->glb_share_mem.glb_shr_mem_desc);

    ctx->proc = HI_NULL;
    ctx->comm_share_mem = HI_NULL;
    ctx->comm_share_mem_size = 0;
    ctx->tmp_create_share_mem = HI_NULL;
    ctx->tmp_create_share_mem_size = 0;

    ctx->dev_hal_size = 0;
    ctx->chan_max_size = 0;

    return;
}

STATIC hi_void mdc_client_init_modules(hi_void)
{
    (hi_void)clt_img_init();
    (hi_void)clt_evt_init();
    (hi_void)clt_stm_init();
    (hi_void)clt_sta_init();

    return;
}

STATIC hi_s32 mdc_client_get_chan_shr_mem_size(hi_void)
{
    hi_u32 chan_shr_mem_size;

    chan_shr_mem_size = SHR_IMG_LIST_SIZE + SHR_STA_MEM_SIZE + SHR_STM_LIST_SIZE + SHR_EVT_LIST_SIZE +
                        MDC_COMM_CHAN_SHARE_MEM_LENGTH;

    chan_shr_mem_size = VFMW_ALIGN_UP(chan_shr_mem_size, MDC_MEM_ALIGN_SIZE);

    return chan_shr_mem_size;
}

STATIC hi_void mdc_client_get_init_param(hi_void)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    UADDR page_table_base_addr = 0;
    UADDR err_rd_addr = 0;
    UADDR err_wr_add = 0;

    OS_GET_MMU_TABLE(&page_table_base_addr, &err_rd_addr, &err_wr_add);
    DPRINT_DBG("%s CbTtbr 0x%x ErrRdAddr 0x%x ErrWrAddr 0x%x\n",
               __func__, page_table_base_addr, err_rd_addr, err_wr_add);

    ctx->ree_pt_addr = page_table_base_addr;

    DPRINT_INFO("%s tee CbTtbr 0x%llx \n", __func__, ctx->tee_pt_addr);

    ctx->chan_share_mem_size = mdc_client_get_chan_shr_mem_size();
    VFMW_CHECK_SEC_FUNC(memset_s(ctx->created_chan_num,
                                 MDC_INDEX_MAX * sizeof(hi_s32), 0, MDC_INDEX_MAX * sizeof(hi_s32)));

    return;
}

STATIC hi_s32 mdc_client_init_local(hi_void)
{
    hi_s32 ret;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    mdc_client_init_modules();

    ret = mdc_client_alloc_glb_share_mem();
    VFMW_ASSERT_RET(ret != HI_FAILURE, ret);

    ret = mdc_adpt_init(ctx->comm_share_mem, ctx->comm_share_mem_size);
    VFMW_ASSERT_RET(ret != HI_FAILURE, ret);

    ret = mdc_client_create_thread();
    VFMW_ASSERT_GOTO(ret != HI_FAILURE, out);

    mdc_client_get_init_param();

#ifdef VFMW_PROC_SUPPORT
    (hi_void)mdc_clt_proc_init(ctx->proc, ctx->comm_share_mem, ctx->comm_share_mem_size);
#endif
    return ret;
out:
    mdc_client_free_glb_share_mem();

    return ret;
}

STATIC hi_s32 mdc_client_deinit_local(hi_void)
{
    hi_s32 ret;

    ret = mdc_client_destroy_thread();
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "mdc_client_destroy_thread ret %d\n", ret);
    }

#ifdef VFMW_PROC_SUPPORT
    (hi_void)mdc_clt_proc_exit();
#endif

    ret = mdc_adpt_exit();
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "mdc_adpt_deinit ret %d\n", ret);
    }

    mdc_client_free_glb_share_mem();

    return ret;
}

STATIC hi_void mdc_client_init_time(mdc_init_param *init)
{
    hi_u64 arm_us, mdc_tick;
    mdc_adpt_ctx *ctx = HI_NULL;

    ctx = mdc_adpt_get_ctx();
    if (ctx->ops == HI_NULL ||
        ctx->ops->get_base_time == HI_NULL) {
        dprint(PRN_ERROR, "%s ops err\n", __func__);
        return;
    }

    ctx->ops->get_base_time(&arm_us, &mdc_tick);
    init->arm_time_us = arm_us;
    init->mdc_time_tick = mdc_tick;
}

STATIC hi_s32 mdc_client_init_server(hi_void *args)
{
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    mdc_init_param init = {0};
    comm_cmd_param param;
    hi_s32 ret;

    BUILD_BUG_ON(sizeof(mdc_init_param) > MDC_MSG_ARGS_SIZE_MAX);

    mdc_client_init_time(&init);

    init.phyaddr     = ctx->glb_share_mem.glb_shr_mem_desc.phy_addr;
    init.length      = ctx->glb_share_mem.glb_shr_mem_desc.length;
    init.mode        = ctx->glb_share_mem.glb_shr_mem_desc.mode;
    init.ree_pt_addr = ctx->ree_pt_addr;

    VFMW_CHECK_SEC_FUNC(memcpy_s(ctx->comm_share_mem, sizeof(vfmw_init_param), args, sizeof(vfmw_init_param)));

    param.chan_id = 0;
    param.service_id = MDC_SID_INIT;
    param.command_id = 0;
    param.args = &init;
    param.length = sizeof(init);
    param.valid_comm_mem = HI_NULL;
    param.comm_mem_length = 0;
    ret = mdc_adpt_send_block(&param);
    if (ret != HI_SUCCESS) {
        (hi_void)mdc_adpt_print_state();
    }

    ctx->chan_max_size = init.chan_max_size;
    ctx->dev_hal_size = init.hal_max_size;

    VFMW_CHECK_SEC_FUNC(memcpy_s(args, sizeof(vfmw_init_param), ctx->comm_share_mem, sizeof(vfmw_init_param)));

    DPRINT_INFO("%s ctx->chan_max_size = 0x%x, ctx->dev_hal_size = 0x%x",
                __func__, ctx->chan_max_size, ctx->dev_hal_size);

    return ret;
}

STATIC hi_s32 mdc_client_deinit_server(hi_void)
{
    hi_s32 ret;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    comm_cmd_param param;

    param.chan_id = 0;
    param.service_id = MDC_SID_DEINIT;
    param.command_id = 0;
    param.args = HI_NULL;
    param.length = 0;
    param.valid_comm_mem = ctx->comm_share_mem;
    param.comm_mem_length = ctx->comm_share_mem_size;

    ret = mdc_adpt_send_block(&param);

    return ret;
}

STATIC mdc_mem_type mdc_client_get_mem_type(hi_s64 unique_id)
{
    return (mdc_mem_type)((unique_id >> 16) & 0xFF); /* 16 :a number */
}

STATIC hi_s32 mdc_client_get_mem_name(mdc_mem_type type, hi_s8 **buf_name)
{
    hi_s8 *mem_name[MDC_MEM_TYPE_MAX] = {
        "VFMW_DecBuf",
        "VFMW_DispBuf",
        "VFMW_PmvBuf",
        "VFMW_NorBuf",
        "VFMW_SecBuf"
    };

    if (type < MDC_MEM_TYPE_MAX) {
        *buf_name = mem_name[type];
        return HI_SUCCESS;
    } else {
        *buf_name = HI_NULL;
        return HI_FAILURE;
    }
}

STATIC hi_s32 mdc_client_alloc_mem_process(mdc_mem_param *param)
{
    hi_s32 ret;
    vmm_buffer buf;
    vmm_cmd_prio proir;
    hi_s8 *buf_name = HI_NULL;

    VFMW_CHECK_SEC_FUNC(memset_s(&buf, sizeof(vmm_buffer), 0, sizeof(vmm_buffer)));

    ret = mdc_client_get_mem_name(mdc_client_get_mem_type(param->unique_id), &buf_name);

    if (snprintf_s(buf.buf_name, sizeof(buf.buf_name), sizeof(buf.buf_name), "%s", buf_name) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    buf.buf_name[sizeof(buf.buf_name)-1] = '\0';
    buf.cache   = param->is_cache;
    buf.size    = param->size;
    buf.priv_id  = param->unique_id;
    buf.mem_type = VMM_MEM_TYPE_SYS_ADAPT;
    buf.sec_flag = param->sec_flag;
    buf.map = param->is_map;
    buf.dma_buf = param->dma_buf;
    buf.vdec_handle = param->vdec_handle;
    buf.ssm_handle = param->ssm_handle;

    proir = (param->phyaddr == 1) ? VMM_CMD_PRIO_MAX : VMM_CMD_PRIO_MIN;
    ret = vmm_send_command(VMM_CMD_ALLOC, proir, &buf);
    if (ret != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s ERROR:  %s phy 0x%x failed!\n",
               __func__, buf.buf_name, buf.start_phy_addr);
    }

    return ret;
}

STATIC hi_s32 mdc_client_free_mem_process(mdc_mem_param *param)
{
    hi_s32 ret;
    vmm_buffer buf;
    hi_s8 *buf_name = HI_NULL;

    VFMW_CHECK_SEC_FUNC(memset_s(&buf, sizeof(vmm_buffer), 0, sizeof(vmm_buffer)));

    ret = mdc_client_get_mem_name(mdc_client_get_mem_type(param->unique_id), &buf_name);

    if (snprintf_s(buf.buf_name, sizeof(buf.buf_name), sizeof(buf.buf_name), "%s", buf_name) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    buf.buf_name[sizeof(buf.buf_name)-1] = '\0';
    buf.start_phy_addr = param->phyaddr;
    buf.start_vir_addr = param->vir_addr;
    buf.cache   = param->is_cache;
    buf.size    = param->size;
    buf.priv_id  = param->unique_id;
    buf.mem_type = VMM_MEM_TYPE_SYS_ADAPT;
    buf.sec_flag = param->sec_flag;
    buf.map = param->is_map;
    buf.dma_buf = param->dma_buf;

    ret = vmm_send_command(VMM_CMD_RELEASE, VMM_CMD_PRIO_MIN, &buf);
    if (ret != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s ERROR: %s phy 0x%x failed!\n",
               __func__, buf.buf_name, buf.start_phy_addr);
    }

    return ret;
}

STATIC hi_s32 mdc_client_shakehand_process(MDC_SID_MEM_CID cmd_id, mdc_shake_param *param)
{
    (hi_void)cmd_id; /* reserved for other func */

    return mdc_adpt_set_boot_up(param->mdc_index);
}

STATIC hi_s32 mdc_client_mem_process(hi_s32 chan_id, MDC_SID_MEM_CID cmd, mdc_mem_param *param)
{
    hi_s32 ret = HI_FAILURE;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    switch (cmd) {
        case MDC_CID_FRM_MEM_ALLOC:
            param->vdec_handle = ctx->chan_ctx[chan_id].vdec_handle;
            param->ssm_handle = ctx->chan_ctx[chan_id].ssm_handle;
            ret = mdc_client_alloc_mem_process(param);
            break;
        case MDC_CID_FRM_MEM_FREE:
            ret = mdc_client_free_mem_process(param);
            break;
        default:
            DPRINT_DBG("%s command_id %d invalid\n", __func__, cmd);
    }

    return ret;
}

STATIC hi_s32 mdc_client_data_report_cb(comm_cmd_param *param)
{
    hi_s32 ret = HI_FAILURE;
    MDC_SERVICE_ID serivce_id = (MDC_SERVICE_ID)param->service_id;

    switch (serivce_id) {
        case MDC_SID_MEM: {
            ret = mdc_client_mem_process(param->chan_id, (MDC_SID_MEM_CID)param->command_id, param->args);
            break;
        }
        case MDC_SID_SHAKE_HAND: {
            ret = mdc_client_shakehand_process((MDC_SID_MEM_CID)param->command_id, (mdc_shake_param *)param->args);
            break;
        }
        default:
            DPRINT_DBG("%s service_id %d invalid\n", __func__, serivce_id);
            break;
    }

    return ret;
}

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
hi_s32 mdc_client_open(hi_void)
{
    hi_s32 ret;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    VFMW_CHECK_SEC_FUNC(memset_s(ctx, sizeof(mdc_ctrl_ctx_c), 0, sizeof(mdc_ctrl_ctx_c)));

    ret = mdc_clt_proc_open();
    VFMW_ASSERT_RET(ret == HI_SUCCESS, ret);

    ret = mdc_adpt_open(mdc_client_data_report_cb);

    return ret;
}

hi_s32 mdc_client_close(hi_void)
{
    hi_s32 ret;

    ret = mdc_adpt_close();

    mdc_clt_proc_close();

    return ret;
}

hi_s32 mdc_client_init(hi_void *args, hi_u32 length)
{
    hi_s32 ret = HI_FAILURE;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();
    vfmw_init_param *param = (vfmw_init_param *)args;

    VFMW_ASSERT_RET(param != HI_NULL, ret);
    VFMW_ASSERT_RET(length == sizeof(vfmw_init_param), ret);

    if (ctx->ref_cnt > 0) {
        goto exit;
    }

    ret = mdc_client_init_local();
    VFMW_ASSERT_RET(ret != HI_FAILURE, ret);

    ret = mdc_client_init_server(args);
    VFMW_ASSERT_GOTO(ret != HI_FAILURE, exit2);

    ret = mdc_client_set_call_back(param->adpt_type, param->fn_callback);
    VFMW_ASSERT_GOTO(ret != HI_FAILURE, exit1);

    ctx->inited = HI_TRUE;

exit:
    ret = HI_SUCCESS;
    ctx->ref_cnt++;
    VFMW_ASSERT_RET_PRNT(ctx->ref_cnt <= MDC_MAX_REF_CNT, HI_FAILURE, "%s ref_cnt %d invalid!\n", __func__,
                         ctx->ref_cnt);
    return ret;
exit1:
    mdc_client_deinit_server();
exit2:
    mdc_client_deinit_local();

    return ret;
}

hi_s32 mdc_client_exit(hi_void *args, hi_u32 length)
{
    hi_s32 ret = HI_FAILURE;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    if (ctx->ref_cnt == 0) {
        goto exit;
    } else if (ctx->ref_cnt >= MDC_UN_NORMAL_REF_CNT) {
        ret = HI_SUCCESS;
        goto exit;
    }
    ctx->inited = HI_FALSE;

    mdc_client_deinit_server();

    ret = mdc_client_deinit_local();
    VFMW_ASSERT_RET(ret != HI_FAILURE, ret);

exit:
    if (ret == HI_SUCCESS) {
        ctx->ref_cnt--;
        VFMW_ASSERT_RET_PRNT(ctx->ref_cnt >= 0, HI_FAILURE, "%s ref_cnt %d invalid!\n", __func__, ctx->ref_cnt);
    }

    return ret;
}

hi_s32 mdc_client_control(hi_s32 chan_id, hi_s32 cmd, hi_void *args, hi_u32 length)
{
    hi_s32 ret = HI_FAILURE;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    DPRINT_INFO("%s chan_id = %d cmd = %d\n", __func__, chan_id, cmd);

    VFMW_ASSERT_RET(chan_id >= 0 && chan_id < VFMW_CHAN_NUM && cmd < VFMW_CID_MAX, ret);

    if (mdc_client_is_local(cmd) == HI_TRUE) {
        ret = mdc_client_local_control(chan_id, cmd, args, length);
    } else {
        VFMW_ASSERT_RET(ctx->inited == HI_TRUE, ret);
        ret = mdc_client_remote_control(chan_id, cmd, args, length);
    }

    return ret;
}

hi_s32 mdc_client_suspend(hi_void *args, hi_u32 length)
{
    return mdc_adpt_suspend(args, length);
}

hi_s32 mdc_client_resume(hi_void *args, hi_u32 length)
{
    return mdc_adpt_resume(args, length);
}

hi_s32 mdc_client_get_image(hi_s32 chan_id, vfmw_image *image)
{
    hi_s32 ret = HI_FAILURE;
    hi_ulong flag = 0;
    mdc_ctrl_ctx_c *ctx = mdc_get_ctx();

    VFMW_ASSERT_RET(image != HI_NULL, ret);
    VFMW_ASSERT_RET(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, ret);
    VFMW_ASSERT_RET(ctx->chan_ctx[chan_id].valid == HI_TRUE, ret);

    OS_SPIN_LOCK(ctx->chan_ctx[chan_id].chan_lock, &flag);
    ret = clt_img_get(chan_id, image);
    OS_SPIN_UNLOCK(ctx->chan_ctx[chan_id].chan_lock, &flag);

    return ret;
}

hi_s32 mdc_client_rel_image(hi_s32 chan_id, const vfmw_image *image)
{
    hi_s32 ret = HI_FAILURE;

    VFMW_ASSERT_RET(image != HI_NULL, ret);
    VFMW_ASSERT_RET(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, ret);

    ret = clt_img_release(chan_id, image->image_id);
#ifdef VFMW_MVC_SUPPORT
    if (ret == HI_SUCCESS && image->is_3d == 1) {
        ret = clt_img_release(chan_id, image->image_id_1);
    }
#endif

    return ret;
}

hi_s32 mdc_client_check_image(hi_s32 chan_id, vfmw_image *image)
{
    hi_s32 ret = HI_FAILURE;

    VFMW_ASSERT_RET(image != HI_NULL, ret);
    VFMW_ASSERT_RET(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, ret);

    ret = clt_img_check(chan_id, image);

    return ret;
}


