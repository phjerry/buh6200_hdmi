/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "linux_osal.h"
#include "tee_client_comm.h"
#include <teek_client_api.h>
#include "tee_private.h"
#include "dbg.h"

static TEEC_Context g_tee_context;
static TEEC_Session g_tee_session;

#ifdef PRODUCT_STB
static TEEC_UUID g_vfmw_uuid = {
    0x3c2bfc84,
    0xc03c,
    0x11e6,
    { 0xa4, 0xa6, 0xce, 0xc0, 0xc9, 0x32, 0xce, 0x01 }
};
#else
static TEEC_UUID g_vfmw_uuid = {
    0x0D0D0D0D,
    0x0D0D,
    0x0D0D,
    { 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D }
};
#endif

/* ----------------------------INTERNAL FUNCTION------------------------------- */
static hi_s32 comm_prepare_params(hi_u32 cmd_id, TEEC_Operation *option, hi_void *args)
{
    tee_vfmw_cmd_params *tee_cmd_param = HI_NULL;
    switch (cmd_id) {
        case VFMW_CMD_ID_INIT:
            VFMW_ASSERT_RET((args != HI_NULL), HI_FAILURE);
            option->paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
            option->started = 1;
            option->params[0].value.a = __pa(args);
            option->params[0].value.b = TEEC_VALUE_UNDEF;
            break;

        case VFMW_CMD_ID_EXIT:
        case VFMW_CMD_ID_RESUME:
        case VFMW_CMD_ID_SUSPEND:
            option->paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_OUTPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
            option->started = 1;
            option->params[0].value.a = 0;
            option->params[0].value.b = TEEC_VALUE_UNDEF;
            break;

        case VFMW_CMD_ID_CONTROL:
            VFMW_ASSERT_RET((args != HI_NULL), HI_FAILURE);
            tee_cmd_param = (tee_vfmw_cmd_params *)args;
            option->started = 1;
            option->paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE);
            option->params[0].value.a = tee_cmd_param->chan_id;
            option->params[0].value.b = tee_cmd_param->cmd_id;
            option->params[1].value.a = tee_cmd_param->arg_phy_addr;
            option->params[1].value.b = tee_cmd_param->param_length;
            break;

        case VFMW_CMD_ID_THREAD:
            option->paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE, TEEC_NONE);
            option->started = 1;
            break;

        case VFMW_CMD_ID_READ_PROC:
            VFMW_ASSERT_RET((args != HI_NULL), HI_FAILURE);
            option->paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE);
            option->started = 1;
            option->params[0].value.a = ((tee_vfmw_proc_params *)args)->module_id;
            option->params[0].value.b = ((tee_vfmw_proc_params *)args)->chan_id;
            break;

        case VFMW_CMD_ID_WRITE_PROC:
            VFMW_ASSERT_RET((args != HI_NULL), HI_FAILURE);
            option->paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE);
            option->started = 1;
            option->params[0].value.a = ((tee_vfmw_proc_params *)args)->module_id;
            option->params[0].value.b = ((tee_vfmw_proc_params *)args)->WriteCount;
            break;

        default:
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void comm_prepare_return_value(hi_u32 cmd_id, TEEC_Operation *option, hi_u32 *ret)
{
    switch (cmd_id) {
        case VFMW_CMD_ID_RESUME:
        case VFMW_CMD_ID_SUSPEND:
        case VFMW_CMD_ID_CONTROL:
        case VFMW_CMD_ID_WRITE_PROC:
        case VFMW_CMD_ID_INIT:
        case VFMW_CMD_ID_EXIT:
            *ret = option->params[0].value.a;
            break;

        default:
            break;
    }

    return;
}

hi_s32 tc_comm_init(hi_void)
{
    TEEC_Result result;
    TEEC_Operation session_operation;
    hi_u8 package_name[] = "hisi_vfmw_sec";
    hi_u32 vfmw_sec_id = 0;

    result = TEEK_InitializeContext(HI_NULL, &g_tee_context);
    if (result != TEEC_SUCCESS) {
        dprint(PRN_ERROR, "TEEK_InitializeContext Failed!\n");
        return HI_FAILURE;
    }

    VFMW_CHECK_SEC_FUNC(memset_s(&session_operation, sizeof(TEEC_Operation), 0, sizeof(TEEC_Operation)));

    session_operation.started = 1;
    session_operation.cancel_flag = 0;
    session_operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE,
                                                    TEEC_NONE,
                                                    TEEC_MEMREF_TEMP_INPUT,
                                                    TEEC_MEMREF_TEMP_INPUT);
    session_operation.params[2].tmpref.buffer = (void *)(&vfmw_sec_id); /* 2: array index */
    session_operation.params[2].tmpref.size = sizeof(vfmw_sec_id); /* 2: array index */
    session_operation.params[3].tmpref.buffer = (void *)(package_name); /* 3: array index */
    session_operation.params[3].tmpref.size = OS_STRLEN(package_name) + 1; /* 3: array index */

    result = TEEK_OpenSession(&g_tee_context, &g_tee_session, &g_vfmw_uuid, TEEC_LOGIN_IDENTIFY, HI_NULL,
                              &session_operation, HI_NULL);
    if (result != TEEC_SUCCESS) {
        dprint(PRN_ERROR, "TEEK_OpenSession Failed!\n");
        TEEK_FinalizeContext(&g_tee_context);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void tc_comm_deinit(hi_void)
{
    TEEK_CloseSession(&g_tee_session);
    TEEK_FinalizeContext(&g_tee_context);
}

/* ----------------------------EXTERNAL FUNCTION---------------------------- */
hi_s32 tc_comm_send_command(hi_u32 cmd_id, hi_void *param, hi_u32 *ret_val)
{
    hi_u32 ret;
    TEEC_Operation operation;
    TEEC_Result result;

    ret = comm_prepare_params(cmd_id, &operation, param);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "Prepare Params Error!\n");
        return HI_FAILURE;
    }

    result = TEEK_InvokeCommand(&g_tee_session, cmd_id, &operation, HI_NULL);
    if (result != TEEC_SUCCESS) {
        dprint(PRN_ERROR, "Call Command:%d Failed!\n", cmd_id);
        return HI_FAILURE;
    }

    comm_prepare_return_value(cmd_id, &operation, ret_val);

    return HI_SUCCESS;
}
