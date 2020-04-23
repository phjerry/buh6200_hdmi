/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_osal_tee
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_tee.h"

#if !defined(TEE_CIPHER_TA_NONSUPPORT) && defined(HI_TEE_SUPPORT) && !defined(CHIP_TRNG_SUPPORT)

#include "teek_client_api.h"

#define TEE_CIPHER_TA_NAME        "hisi_drv_cipher"
#define TEE_CIPHER_TA_UID         0

static hi_u32 s_tee_init = HI_FALSE;

/*! tee mutex */
static crypto_mutex tee_mutex;

#define osal_tee_lock()   \
    do {
        ret = crypto_mutex_lock(&tee_mutex);  \
        if (ret != HI_SUCCESS)        \
        {\
            hi_log_error("error, tee lock failed\n");\
            hi_log_func_exit();\
            return ret;\
        }\
    } while (0)

#define osal_tee_unlock()   crypto_mutex_unlock(&tee_mutex)

static const TEEC_UUID s_stTeecCipherUUID = {
    0x04ae2ac0, \
    0x01e8, \
    0x4587, \
    {0xb3, 0xda, 0x38, 0xf5, 0x98, 0x46, 0xbc, 0x57}\
};

static TEEC_Session    session;
static TEEC_Context    context;
static TEEC_Operation  operation;

hi_s32 drv_osal_tee_open_session(void)
{
    hi_u32 root_id = 0;
    hi_s32 ret;
    TEEC_Result teec_result;

    hi_log_func_enter();

    crypto_mutex_init(&tee_mutex);

    ret = memset_s(&context, sizeof(context), 0, sizeof(context));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

    ret = memset_s(&operation, sizeof(operation), 0, sizeof(operation));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

    ret = memset_s(&session, sizeof(session), 0, sizeof(session));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

    teec_result = TEEK_InitializeContext(NULL, &context);
    if (teec_result != HI_SUCCESS) {
        hi_log_print_func_err(TEEK_InitializeContext, teec_result);
        return teec_result;
    }

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(
                               TEEC_NONE,
                               TEEC_NONE,
                               TEEC_MEMREF_TEMP_INPUT,
                               TEEC_MEMREF_TEMP_INPUT);

    operation.params[2].tmpref.buffer = (hi_void *)(&root_id); /* buffers of params 2 */
    operation.params[2].tmpref.size = sizeof(root_id);         /* buffer size of 2 */
    operation.params[3].tmpref.buffer = (hi_void *)(TEE_CIPHER_TA_NAME); /* buffers of params 3 */
    operation.params[3].tmpref.size = strlen(TEE_CIPHER_TA_NAME) + 1;    /* buffer size of 3 */

    teec_result = TEEK_OpenSession(&context, &session, &s_stTeecCipherUUID,
        TEEC_LOGIN_IDENTIFY, NULL, &operation, NULL);
    if (teec_result != HI_SUCCESS) {
        TEEK_FinalizeContext(&context);
        hi_log_print_func_err(TEEK_OpenSession, teec_result);
        return teec_result;
    }

    s_tee_init = HI_TRUE;

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_osal_tee_command(hi_u32 command, void *params, hi_u32 paramsize)
{
    TEEC_Result teec_result = HI_FAILURE;
    hi_s32 ret = HI_FAILURE;

    hi_log_func_enter();

    if (s_tee_init != HI_TRUE) {
        hi_log_print_err_code(HI_FAILURE);
        return HI_FAILURE;
    }

    osal_tee_lock();

    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INOUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.buffer = params;
    operation.params[0].tmpref.size   = paramsize;

    hi_log_info("send tee command: 0x%x\n", command);
    teec_result = TEEK_InvokeCommand(&session, command, &operation, NULL);
    if (teec_result != HI_SUCCESS) {
        hi_log_print_func_err(TEEK_InvokeCommand, teec_result);
        osal_tee_unlock();
        return teec_result;
    }

    osal_tee_unlock();

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_osal_tee_close_session(void)
{
    hi_log_func_enter();

    if (s_tee_init == HI_FALSE) {
        return HI_SUCCESS;
    }

    TEEK_CloseSession(&session);
    TEEK_FinalizeContext(&context);
    crypto_mutex_destroy(&tee_mutex);
    s_tee_init = HI_FALSE;

    hi_log_func_exit();
    return HI_SUCCESS;
}

#else

hi_s32 drv_osal_tee_open_session(void)
{
    return HI_SUCCESS;
}

hi_s32 drv_osal_tee_command(hi_u32 command, void *params, hi_u32 paramsize)
{
    hi_log_error("Unsupport communication with TEE\n");

    return HI_FAILURE;
}

hi_s32 drv_osal_tee_close_session(void)
{
    return HI_SUCCESS;
}

#endif

