/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: PVR tee process.
 * Author: sdk
 * Create: 2019-08-16
 */

#ifdef HI_TEE_PVR_SUPPORT
#include "hi_osal.h"
#include <linux/module.h>
#include "hi_type.h"
#include "hi_drv_module.h"
#include "hi_errno.h"
#include "drv_pvr_tee_func.h"
#include "drv_pvr_debug.h"

static TEEC_UUID g_pvr_uuid = {
    0x569985fe,
    0xbac0,
    0x11e6,
    {
        0xa4, 0xa6, 0xce, 0xc0, 0xc9, 0x32, 0xce, 0x01
    }
};

static pvr_drv_teec_info g_teec_info = {
    .init_cnt = 0
};

static pvr_drv_tee_rec_chan g_tee_rec_chan[PVR_TEE_REC_MAX_CHN_NUM];

hi_s32 pvr_teec_init(hi_void)
{
    hi_u32 i;
    hi_u32 root_id = 0;
    TEEC_Operation operation = {0};
    hi_s32 ret;
    hi_char session_name[] = "hisi_pvr_client";

    hi_pvr_debug_enter();

    g_teec_info.init_cnt++;
    if (g_teec_info.init_cnt > 1) {
        hi_info_pvr("pvr tee has already initialized(%d)!\n", g_teec_info.init_cnt);
        return  HI_SUCCESS;
    }

    for (i = 0; i < PVR_TEE_REC_MAX_CHN_NUM; i++) {
        g_tee_rec_chan[i].is_use = HI_FALSE;
        g_tee_rec_chan[i].owner = 0;
        g_tee_rec_chan[i].tee_id = 0xFFFF;
    }

    ret = TEEK_InitializeContext(HI_NULL, &g_teec_info.context);
    if (ret != HI_SUCCESS) {
        hi_err_pvr("Initialise pvr teec context failed(0x%x)", ret);
        g_teec_info.init_cnt--;
        return ret;
    }

    operation.started = 1;
    operation.cancel_flag = 0;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT);
    operation.params[2].tmpref.buffer = (hi_void *)(&root_id);
    operation.params[2].tmpref.size = sizeof(root_id);
    operation.params[3].tmpref.buffer = (hi_void *)(session_name);
    operation.params[3].tmpref.size = strlen(session_name) + 1;

    ret = (hi_s32)TEEK_OpenSession(&g_teec_info.context, &g_teec_info.session,
        &g_pvr_uuid, TEEC_LOGIN_IDENTIFY, HI_NULL, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        hi_err_pvr("open pvr teec session failed(0x%x)", ret);
        g_teec_info.init_cnt--;
        TEEK_FinalizeContext(&g_teec_info.context);
        return ret;
    }

    hi_info_pvr("open pvr teec session success!\n");
    hi_pvr_debug_exit();

    return HI_SUCCESS;
}

hi_s32 pvr_teec_deinit(hi_void)
{
    hi_u32 i;

    hi_pvr_debug_enter();

    if (g_teec_info.init_cnt <= 0) {
        hi_info_pvr("Not Init the PVR Tee!\n");
        return HI_SUCCESS;
    }

    g_teec_info.init_cnt--;
    if (g_teec_info.init_cnt > 0) {
        hi_info_pvr("There is other instance running!\n");
        return HI_SUCCESS;
    }

    for (i = 0; i < PVR_TEE_REC_MAX_CHN_NUM; i++) {
        g_tee_rec_chan[i].is_use = HI_FALSE;
        g_tee_rec_chan[i].owner = 0;
        g_tee_rec_chan[i].tee_id = 0xFFFF;
    }

    TEEK_CloseSession(&g_teec_info.session);
    TEEK_FinalizeContext(&g_teec_info.context);
    hi_info_pvr("TEEC deinit success!\n");

    hi_pvr_debug_exit();

    return HI_SUCCESS;
}

hi_s32 pvr_tee_release(hi_void *private_data)
{
    hi_u32 i;
    pvr_tee_gen_value_args args = {{0}};
    pid_t pid = (pid_t)(unsigned long)((struct pvr_intf**)private_data);

    hi_pvr_debug_enter();

    for (i = 0; i < PVR_TEE_REC_MAX_CHN_NUM; i++) {
        if ((g_tee_rec_chan[i].is_use == HI_TRUE) && (pid == g_tee_rec_chan[i].owner)) {
            args.args[0] = g_tee_rec_chan[i].tee_id;
            (hi_void)pvr_tee_rec_close_chn(&args);
        }
    }

    hi_pvr_debug_exit();

    return HI_SUCCESS;
}

/*
 * Input: args->u32Args[0] -- the buffer length of the buffer which will be allocated on the TEE
 * Output:args->u32Args[7] -- tee channel id
 */
hi_s32 pvr_tee_rec_open_chn(pvr_tee_gen_value_args *args)
{
    hi_u32 i;
    hi_s32 ret;
    TEEC_Operation operation = {0};
    hi_u32 origin = 0;
    pvr_drv_tee_rec_chan *chan = HI_NULL;
    hi_u32 length = args->args[0];

    hi_pvr_debug_enter();

    for (i = 0; i < PVR_TEE_REC_MAX_CHN_NUM; i++) {
        if (g_tee_rec_chan[i].is_use != HI_TRUE) {
            chan = &g_tee_rec_chan[i];
            break;
        }
    }

    if (chan == HI_NULL) {
        hi_err_pvr("no tee channel left!\n");
        return HI_ERR_PVR_NO_CHN_LEFT;
    }

    if ((length % REE_PVR_BUFFER_ALIGN) != 0) {
        hi_warn_pvr("buffer length(%u) is not aligned by %u\n", length, (hi_u32)REE_PVR_BUFFER_ALIGN);
        length = (length % REE_PVR_BUFFER_ALIGN) * REE_PVR_BUFFER_ALIGN + REE_PVR_BUFFER_ALIGN;
    }

    if ((length < REE_PVR_BUFFER_MIN_LEN) || (length > REE_PVR_BUFFER_MAX_LEN)) {
        hi_err_pvr("buffer length(%u) is too short(%u) or too long(%u)", length, REE_PVR_BUFFER_MIN_LEN,
            REE_PVR_BUFFER_MAX_LEN);
        return HI_ERR_PVR_INVALID_PARA;
    }

    operation.started = 1;
    operation.params[0].value.a = 0;
    operation.params[0].value.b = length;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    ret = (hi_s32)TEEK_InvokeCommand(&g_teec_info.session, (hi_u32)TEE_PVR_CMD_REC_OPEN_CHANNEL, &operation, &origin);
    if (ret != HI_SUCCESS) {
        hi_err_pvr("open tee rec channel faild:ret=0x%x, origin:%d\n", ret, origin);
        return ret;
    }

    chan->is_use = HI_TRUE;
    chan->owner = task_tgid_nr(current);
    chan->tee_id = operation.params[0].value.a;
    args->args[7] = operation.params[0].value.a;

    hi_pvr_debug_exit();
    return HI_SUCCESS;
}

/*
 * input: args->u32Args[0] -- the tee channel id.
 * output: none
 */
hi_s32 pvr_tee_rec_close_chn(pvr_tee_gen_value_args *args)
{
    hi_u32 origin = 0;
    TEEC_Operation operation = {0};
    hi_s32 ret;
    hi_u32 idx;
    hi_u32 ch_id = args->args[0];
    pvr_drv_tee_rec_chan *chan = HI_NULL;

    hi_pvr_debug_enter();

    for (idx = 0; idx < PVR_TEE_REC_MAX_CHN_NUM; idx++) {
        if ((g_tee_rec_chan[idx].is_use == HI_TRUE) && (g_tee_rec_chan[idx].tee_id == (ch_id))) {
            chan = &g_tee_rec_chan[idx];
            break;
        }
    }

    if (idx >= PVR_TEE_REC_MAX_CHN_NUM) {
        hi_err_pvr("Invalid tee channel(%d)\n", ch_id);
        return HI_ERR_PVR_INVALID_PARA;
    }

    operation.started = 1;
    operation.params[0].value.a = ch_id;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    ret = (hi_s32)TEEK_InvokeCommand(&g_teec_info.session, (hi_u32)TEE_PVR_CMD_REC_CLOSE_CHANNEL, &operation, &origin);
    if (ret != HI_SUCCESS) {
        hi_err_pvr("close tee rec channel faild:ret=0x%x, origin:%d\n", ret, origin);
        return ret;
    }

    chan->is_use = HI_FALSE;
    chan->owner = 0;
    chan->tee_id = 0xFFFF;

    hi_pvr_debug_exit();
    return HI_SUCCESS;
}

/*
 * input: args->u32Args[0] -- the tee channel id.
 *          args->u32Args[1] -- smmu addr of ree buffer.
 *          args->u32Args[2] -- the size of ree buffer.
 * output: args->u32Args[7] -- the valid length of ree buffer.
 */
hi_s32 pvr_tee_rec_copy_to_ree(pvr_tee_gen_value_args *args)
{
    hi_u32 origin = 0;
    TEEC_Operation operation = {0};
    hi_s32 ret;
    hi_u32 ch_id = args->args[0];

    hi_pvr_debug_enter();

    operation.started = 1;
    operation.params[0].value.a = ch_id;
    operation.params[1].value.a = args->args[1];
    operation.params[1].value.b = args->args[2];
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INOUT, TEEC_NONE, TEEC_NONE);

    ret = (hi_s32)TEEK_InvokeCommand(&g_teec_info.session, (hi_u32)TEE_PVR_CMD_REC_COPY_TO_REE, &operation, &origin);
    if (ret != HI_SUCCESS) {
        hi_err_pvr("copy to ree faild:ret=0x%x, origin:%d\n", ret, origin);
        return ret;
    }

    args->args[7] = operation.params[1].value.b;

    hi_pvr_debug_exit();
    return HI_SUCCESS;
}

/*
 * input: args->u32Args[0] -- tee channel id
 *          args->u32Args[1] -- smmu address pf ree
 * output: none
 */
hi_s32 pvr_tee_rec_process_data(pvr_tee_gen_value_args *args)
{
    hi_u32 origin = 0;
    TEEC_Operation operation = {0};
    hi_s32 ret;
    hi_u32 ch_id = args->args[0];

    hi_pvr_debug_enter();

    operation.started = 1;
    operation.params[0].value.a = ch_id;
    operation.params[0].value.b = args->args[1];
    operation.params[1].value.a = args->args[2];
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INOUT, TEEC_NONE, TEEC_NONE);

    ret = (hi_s32)TEEK_InvokeCommand(&g_teec_info.session, (hi_u32)TEE_PVR_CMD_REC_PROCESS_DATA, &operation, &origin);
    if (ret != HI_SUCCESS) {
        hi_err_pvr("process ts data faild:ret=0x%x, origin:%d\n", ret, origin);
        return ret;
    }

    hi_pvr_debug_exit();
    return HI_SUCCESS;
}


hi_s32 pvr_tee_rec_get_state(pvr_tee_gen_value_args *args)
{
    return HI_SUCCESS;
}

/*
 * input: args->u32Args[0] -- tee channel id
 * input:  args->u32Args[1] --the max expected length
 * ouput:  args->u32Args[7] --the address of buffer
 * ouput:  args->u32Args[6] --the valided length
 */
hi_s32 pvr_tee_rec_get_addr_info(pvr_tee_gen_value_args *args)
{
    hi_u32 origin = 0;
    TEEC_Operation operation = {0};
    hi_s32 ret;
    hi_u32 ch_id = args->args[0];

    hi_pvr_debug_enter();

    operation.started = 1;
    operation.params[0].value.a = ch_id;
    operation.params[0].value.b = args->args[1];
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INOUT, TEEC_VALUE_INOUT, TEEC_NONE);

#if defined(CHIP_TYPE_HI3798MV200) && defined(HI_ADVCA_TYPE_VERIMATRIX_ULTRA)
    ret = (hi_s32)TEEK_InvokeCommand(&g_teec_info.session, (hi_u32)TEE_PVR_CMD_REC_GET_ADDRINFO, &operation, &origin);
#else
    ret = HI_SUCCESS;
#endif
    if (ret != HI_SUCCESS) {
        hi_err_pvr("get tee data info faild:ret=0x%x, origin:%d\n", ret, origin);
        return ret;
    }

    /* fix the compilation error:unused variable origin */
    origin = origin;

    args->args[7] = operation.params[1].value.a;
    args->args[6] = operation.params[1].value.b;
    hi_pvr_debug_exit();
    return HI_SUCCESS;
}

/*
 * input: args->u32Args[0] -- SMMU address of REE buffer
 *          args->u32Args[1] -- SMMU address of TEE buffer
 *          args->u32Args[2] -- the length of data
 */
hi_s32 pvr_tee_play_copy_from_ree(pvr_tee_gen_value_args *args)
{
    hi_u32 origin = 0;
    TEEC_Operation operation = {0};
    hi_s32 ret;

    hi_pvr_debug_enter();

    operation.started = 1;
    operation.params[0].value.a = args->args[0];
    operation.params[0].value.b = args->args[1];
    operation.params[1].value.a = args->args[2];
    operation.params[1].value.b = args->args[3];
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INOUT, TEEC_NONE, TEEC_NONE);

    ret = (hi_s32)TEEK_InvokeCommand(&g_teec_info.session, (hi_u32)TEE_PVR_CMD_PLAY_COPY_FROM_REE, &operation, &origin);
    if (ret != HI_SUCCESS) {
        hi_err_pvr("copy play data to tee faild:ret=0x%x, origin:%d\n", ret, origin);
        return ret;
    }

    hi_pvr_debug_exit();
    return HI_SUCCESS;
}

/*
 * input: args->u32Args[0] -- SMMU address of data buffer
 *          args->u32Args[1] -- u32HeadOffset
 *          args->u32Args[2] -- u32EndOffset
 *          args->u32Args[3] -- u32HeadToAdd
 *          args->u32Args[4] -- u32EndToAdd
 */
hi_s32 pvr_tee_play_process_data(pvr_tee_gen_value_args *args)
{
    hi_u32 origin = 0;
    TEEC_Operation operation = {0};
    hi_s32 ret;

    hi_pvr_debug_enter();

    operation.started = 1;
    operation.params[0].value.a = args->args[0];
    operation.params[0].value.b = args->args[1];
    operation.params[1].value.a = args->args[2];
    operation.params[1].value.b = args->args[3];
    operation.params[2].value.a = args->args[4];
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_INPUT, TEEC_VALUE_INPUT, TEEC_NONE);

    ret = (hi_s32)TEEK_InvokeCommand(&g_teec_info.session, (hi_u32)TEE_PVR_CMD_PLAY_PROCESS_DATA, &operation, &origin);
    if (ret != HI_SUCCESS) {
        hi_err_pvr("process play ts data faild:ret=0x%x, origin:%d\n", ret, origin);
        return ret;
    }

    hi_pvr_debug_exit();
    return HI_SUCCESS;
}

hi_s32 hi_drv_pvr_teec_ioctl(hi_void *private_data, hi_u32 cmd, hi_void *arg)
{
    hi_s32 ret;

    hi_pvr_debug_enter();

    if (arg == HI_NULL) {
        hi_err_pvr("null pinter for input!\n");
        return HI_ERR_PVR_NUL_PTR;
    }

    switch (cmd) {
        case CMD_PVR_TEE_REC_OPEN:
            ret = pvr_tee_rec_open_chn(arg);
            break;
        case CMD_PVR_TEE_REC_CLOSE:
            ret = pvr_tee_rec_close_chn(arg);
            break;
        case CMD_PVR_TEE_REC_COPY_REE_TEST:
            ret = pvr_tee_rec_copy_to_ree(arg);
            break;
        case CMD_PVR_TEE_REC_PROCESS_TS_DATA:
            ret = pvr_tee_rec_process_data(arg);
            break;
        case CMD_PVR_TEE_REC_GET_STATE:
            ret = pvr_tee_rec_get_state(arg);
            break;
        case CMD_PVR_TEE_REC_GET_ADDRINFO:
            ret = pvr_tee_rec_get_addr_info(arg);
            break;
        case CMD_PVR_TEE_PLAY_COPY_FROM_REE:
            ret = pvr_tee_play_copy_from_ree(arg);
            break;
        case CMD_PVR_TEE_PLAY_PROCESS_DATA:
            ret = pvr_tee_play_process_data(arg);
            break;
        default:
            ret = HI_ERR_PVR_INVALID_PARA;
            hi_err_pvr("invalid cmd id(%d)\n", cmd);
            break;
    }

    if (ret != HI_SUCCESS) {
        hi_err_pvr("process failed,cmd id is:0x%08x, ret = 0x%08x\n", cmd, ret);
        return ret;
    }

    hi_pvr_debug_exit();
    return HI_SUCCESS;
}
#endif
