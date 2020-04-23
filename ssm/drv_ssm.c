/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: SSM function file for Hisilicon SSM
 * Author: ssm group
 * Create: 2019/12/11
 * Notes:
 */

#include "hi_osal.h"
#include "drv_ssm.h"
#include "linux/hisilicon/securec.h"
#include "linux/ion.h"
#include "linux/dma-buf.h"
#ifdef SSM_TEE_SUPPORT
#include "teek_client_api.h"
#endif

#define HI_FATAL_SSM(fmt...)     printk(fmt);
#define HI_ERROR_SSM(fmt...)     printk(fmt);
#define SSM_TEEC_NAME "tee_ssm_session"

#ifdef SSM_TEE_SUPPORT
static TEEC_Context g_teec_context = {0};
TEEC_Session g_teec_session = {0};
TEEC_UUID g_teec_uuid = {
    0x90ae48e5,
    0xc757,
    0x44a7,
    {
        0xb5, 0x13, 0xde, 0x4b, 0x2b, 0x14, 0xa0, 0x7c
    }
};

#endif

typedef enum {
    TEEC_CMD_SSM_CREATE = 0,
    TEEC_CMD_SSM_DESTROY,
    TEEC_CMD_SSM_ADD_RESOUCE,
    TEEC_CMD_SSM_ATTACH_BUFFER,
    TEEC_CMD_SSM_GET_INTENT,
    TEEC_CMD_SSM_IOMMU_CONFIG,
    TEEC_CMD_SSM_INIT,
#ifdef SSM_TEST_SUPPORT
    TEEC_CMD_SSM_CHECK_BUF = 0xff,
#endif
} DRV_SSM_TEEC_CMD_ID;

typedef struct {
    hi_handle               session_handle;
    hi_drv_ssm_buffer_id    buf_id;
    hi_u64                  buf_smmu_addr;
    hi_u32                  buf_len;
    hi_handle               module_handle;
} SSM_TEEK_ATTACH_BUF_CTL;

#ifdef SSM_TEST_SUPPORT
typedef struct {
    hi_handle               session_handle;
    hi_handle               module_handle;
    hi_drv_ssm_buffer_id    buf_id;
    hi_u64                  buf_smmu_addr;
    hi_u64                  buf_len;
} SSM_TEEK_CHECK_BUF_CTL;
#endif

#define SSM_MAX_SESSION_NUM 16

typedef struct {
    struct file *filp;
    hi_handle ssm_handle;
} ssm_info;

typedef struct {
    hi_u32 ssm_num;
    ssm_info chan_info[SSM_MAX_SESSION_NUM];
} ssm_dev_handler;

static ssm_dev_handler g_ssm_dev = {0};
static osal_mutex g_ssm_lock = {0};

hi_s32 do_ssm_destroy(hi_handle ssm_handle)
{
#ifdef SSM_TEE_SUPPORT
    TEEC_Operation operation = {0};
    teec_result    result;
    hi_u32         origin = 0;
    hi_u32         i = 0;

    operation.started = 1;
    operation.params[0].value.a = (hi_u32)ssm_handle;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    result = TEEK_InvokeCommand(&g_teec_session,
                                TEEC_CMD_SSM_DESTROY,
                                &operation,
                                &origin);
    if (result != TEEC_SUCCESS) {
        HI_ERROR_SSM("TEEC invoke destroy fail:%x\n", result);
        return result;
    }

    (hi_void)osal_mutex_lock(&g_ssm_lock);
    for (i = 0; i < SSM_MAX_SESSION_NUM; i++) {
        if (g_ssm_dev.chan_info[i].ssm_handle != ssm_handle) {
            continue;
        }

        g_ssm_dev.chan_info[i].ssm_handle = 0;
        g_ssm_dev.chan_info[i].filp = HI_NULL;
        g_ssm_dev.ssm_num--;
        break;
    }
    (hi_void)osal_mutex_unlock(&g_ssm_lock);

    return HI_SUCCESS;
#else
    return HI_FAILURE;
#endif
}

hi_s32 drv_ssm_teec_init(hi_void)
{
#ifdef SSM_TEE_SUPPORT
    TEEC_Result     teec_ret;
    TEEC_Operation  teec_operation      = {0};
    uint32_t        teec_ret_origion    = 0;
    hi_u32          root_id             = 0;
    hi_s32          ret;

    ret = osal_mutex_init(&g_ssm_lock);
    if (ret != HI_SUCCESS) {
        HI_ERROR_SSM("cannot init osal lock! : 0x%x\n", ret);
        return ret;
    }

    teec_ret = TEEK_InitializeContext(HI_NULL, &g_teec_context);
    if (teec_ret != TEEC_SUCCESS) {
        HI_FATAL_SSM("teec init content fail:%x\n", teec_ret);
        return teec_ret;
    }

    teec_operation.started = 1;
    teec_operation.cancel_flag = 0;
    teec_operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT);
    teec_operation.params[2].tmpref.buffer = (void *)&root_id; /* 2 is offset */
    teec_operation.params[2].tmpref.size = sizeof(root_id); /* 2 is offset */
    teec_operation.params[3].tmpref.buffer = (void *)SSM_TEEC_NAME; /* 3 is offset */
    teec_operation.params[3].tmpref.size = strlen(SSM_TEEC_NAME) + 1; /* 3 is offset */

    teec_ret = TEEK_OpenSession(&g_teec_context,
                                &g_teec_session,
                                &g_teec_uuid,
                                TEEC_LOGIN_IDENTIFY,
                                HI_NULL,
                                &teec_operation,
                                &teec_ret_origion);
    if (teec_ret != TEEC_SUCCESS) {
        HI_FATAL_SSM("TEEK_OpenSession fail:%x\n", teec_ret);
        TEEK_FinalizeContext(&g_teec_context);
    }

    ret = memset_s(&teec_operation, sizeof(TEEC_Operation), 0, sizeof(TEEC_Operation));
    if (ret != HI_SUCCESS) {
        HI_FATAL_SSM("memset teec_operation  fail\n");
        TEEK_CloseSession(&g_teec_session);
        TEEK_FinalizeContext(&g_teec_context);
        return HI_FAILURE;
    }

    teec_operation.started = 1;
    teec_operation.params[0].value.a = (hi_u32)(0); /* no actual use, teec not support null param */
    teec_operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    teec_ret = TEEK_InvokeCommand(&g_teec_session, TEEC_CMD_SSM_INIT, &teec_operation, &teec_ret_origion);
    if (teec_ret != TEEC_SUCCESS) {
        HI_ERROR_SSM("TEEC invoke fail:%x\n", teec_ret);
        TEEK_CloseSession(&g_teec_session);
        TEEK_FinalizeContext(&g_teec_context);
        return teec_ret;
    }

    return HI_SUCCESS;
#else
    return HI_FAILURE;
#endif
}


hi_void drv_ssm_teec_deinit(hi_void)
{
#ifdef SSM_TEE_SUPPORT
    (hi_void)TEEK_CloseSession(&g_teec_session);
    (hi_void)TEEK_FinalizeContext(&g_teec_context);
#endif
    return;
}

hi_s32 drv_ssm_teec_create(struct file *ssm_filp, hi_ssm_intent intent, hi_handle *handle)
{
#ifdef SSM_TEE_SUPPORT
    TEEC_Operation operation = {0};
    teec_result    result;
    hi_u32         origin = 0;
    hi_u32         i;

    SSM_CHECK_POINTER_RETURN_IF_FAIL(handle);

    operation.started = 1;
    operation.params[0].value.a = (hi_u32)(intent);
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    result = TEEK_InvokeCommand(&g_teec_session, TEEC_CMD_SSM_CREATE, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        HI_ERROR_SSM("TEEC invoke fail:%x\n", result);
        return result;
    }
    (*handle) = operation.params[0].value.b;

    (hi_void)osal_mutex_lock(&g_ssm_lock);
    for (i = 0; i < SSM_MAX_SESSION_NUM; i++) {
        if (g_ssm_dev.chan_info[i].ssm_handle != 0) {
            continue;
        }

        g_ssm_dev.chan_info[i].ssm_handle = operation.params[0].value.b;
        g_ssm_dev.chan_info[i].filp = ssm_filp;
        g_ssm_dev.ssm_num++;

        break;
    }
    (hi_void)osal_mutex_unlock(&g_ssm_lock);

    if (i >= SSM_MAX_SESSION_NUM) {
        HI_ERROR_SSM("drv_ssm_teec_create fail:%x\n", result);
        (hi_void)do_ssm_destroy(*handle);
        *handle = HI_INVALID_HANDLE;
        return HI_FAILURE;
    }

    return HI_SUCCESS;
#else
    return HI_FAILURE;
#endif
}

hi_s32 drv_ssm_teec_destroy(hi_handle ssm_handle)
{
    return do_ssm_destroy(ssm_handle);
}

hi_s32 drv_ssm_teec_add_resource(hi_handle ssm_handle, hi_ssm_resource_info res_info)
{
#ifdef SSM_TEE_SUPPORT
    TEEC_Operation operation = {0};
    teec_result    result;
    hi_u32         origin = 0;

    operation.started = 1;
    operation.params[0].value.a = (hi_u32)ssm_handle;
    operation.params[1].tmpref.buffer = (void *)&res_info;
    operation.params[1].tmpref.size = sizeof(res_info);
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE);
    result = TEEK_InvokeCommand(&g_teec_session,
                                TEEC_CMD_SSM_ADD_RESOUCE,
                                &operation,
                                &origin);
    if (result != TEEC_SUCCESS) {
        HI_ERROR_SSM("TEEC invoke add resource fail:%x\n", result);
        return result;
    }

    return HI_SUCCESS;
#else
    return HI_FAILURE;
#endif
}

hi_s32 do_teec_attach_buffer(SSM_TEEK_ATTACH_BUF_CTL *teek_attach_info, hi_u64 *addr)
{
    TEEC_Operation          operation = {0};
    teec_result             result;
    hi_u32                  origin = 0;

    operation.started = 1;
    operation.params[0].tmpref.buffer = (void *)teek_attach_info;
    operation.params[0].tmpref.size = sizeof(SSM_TEEK_ATTACH_BUF_CTL);
    operation.params[1].value.a = 0;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INOUT, TEEC_NONE, TEEC_NONE);
    result = TEEK_InvokeCommand(&g_teec_session,
                                TEEC_CMD_SSM_ATTACH_BUFFER,
                                &operation,
                                &origin);
    if (result != TEEC_SUCCESS) {
        HI_ERROR_SSM("TEEC invoke attach buffer fail:%x\n", result);
        return result;
    }

    (*addr) = operation.params[1].value.b;

    return HI_SUCCESS;
}

hi_s32 drv_ssm_teec_attach_buffer(hi_ssm_buffer_attach_info attach_info, hi_u64 *addr)
{
#ifdef SSM_TEE_SUPPORT
    SSM_TEEK_ATTACH_BUF_CTL teek_attach_info = {0};
    struct dma_buf         *dma_buf_addr = HI_NULL;
    hi_s32                  ret;
    dma_addr_t              smmu_addr;

    teek_attach_info.session_handle = attach_info.session_handle;
    teek_attach_info.buf_id = attach_info.buf_id;
    teek_attach_info.module_handle = attach_info.module_handle;

    dma_buf_addr = osal_mem_handle_get(attach_info.dma_buf_handle, HI_ID_SSM);
    if (dma_buf_addr == HI_NULL) {
        HI_ERROR_SSM("get dma buf addr fail\n");
        return HI_FAILURE;
    }

    smmu_addr = osal_mem_secsmmu_map(dma_buf_addr, 0);
    if (smmu_addr == 0) {
        HI_ERROR_SSM("get smmu addr fail\n");
        return HI_FAILURE;
    }

    (hi_void)osal_mem_secsmmu_unmap(dma_buf_addr, smmu_addr, 0);

    teek_attach_info.buf_smmu_addr = smmu_addr;
    teek_attach_info.buf_len = dma_buf_addr->size;

    ret = do_teec_attach_buffer(&teek_attach_info, addr);
    if (ret != HI_SUCCESS) {
        HI_ERROR_SSM("tec attach buffer\n");
        return ret;
    }

    return HI_SUCCESS;
#else
    return HI_FAILURE;
#endif
}

hi_s32 drv_ssm_teec_get_intent(hi_handle ssm_handle, hi_ssm_intent *get_intent)
{
#ifdef SSM_TEE_SUPPORT
    TEEC_Operation operation = {0};
    teec_result    result;
    hi_u32         origin = 0;

    operation.started = 1;
    operation.params[0].value.a = (hi_u32)(ssm_handle);
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    result = TEEK_InvokeCommand(&g_teec_session,
                                TEEC_CMD_SSM_GET_INTENT,
                                &operation,
                                &origin);
    if (result != TEEC_SUCCESS) {
        HI_ERROR_SSM("TEEC invoke get intent fail:%x\n", result);
        return result;
    }

    (*get_intent) = operation.params[0].value.b;

    return HI_SUCCESS;
#else
    return HI_FAILURE;
#endif
}

#ifdef SSM_TEST_SUPPORT
hi_s32 drv_ssm_teec_check_buffer(hi_ssm_buffer_check_info check_info)
{
#ifdef SSM_TEE_SUPPORT
    SSM_TEEK_CHECK_BUF_CTL  teek_check_info = {0};
    struct dma_buf         *dma_buf_addr = HI_NULL;
    dma_addr_t              smmu_addr;
    TEEC_Operation operation = {0};
    teec_result    result;
    hi_u32         origin = 0;

    dma_buf_addr = osal_mem_handle_get(check_info.dma_buf_handle, HI_ID_SSM);
    if (dma_buf_addr == HI_NULL) {
        HI_ERROR_SSM("get dma buf addr fail\n");
        return HI_FAILURE;
    }

    smmu_addr = osal_mem_secsmmu_map(dma_buf_addr, 0);
    if (smmu_addr == 0) {
        HI_ERROR_SSM("get smmu addr fail\n");
        return HI_FAILURE;
    }

    (hi_void)osal_mem_secsmmu_unmap(dma_buf_addr, smmu_addr, 0);

    teek_check_info.session_handle = check_info.session_handle;
    teek_check_info.module_handle = check_info.module_handle;
    teek_check_info.buf_id = check_info.buf_id;
    teek_check_info.buf_smmu_addr = smmu_addr;
    teek_check_info.buf_len = dma_buf_addr->size;

    operation.started = 1;
    operation.params[0].tmpref.buffer = (void *)&teek_check_info;
    operation.params[0].tmpref.size = sizeof(SSM_TEEK_CHECK_BUF_CTL);
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    result = TEEK_InvokeCommand(&g_teec_session,
                                TEEC_CMD_SSM_CHECK_BUF,
                                &operation,
                                &origin);
    if (result != TEEC_SUCCESS) {
        HI_ERROR_SSM("TEEC invoke get intent fail:%x\n", result);
        return result;
    }

    return HI_SUCCESS;
#else
    return HI_FAILURE;
#endif
}
#endif

hi_s32 drv_ssm_teec_iommu_config(hi_logic_mod_id module_id)
{
#ifdef SSM_TEE_SUPPORT
    TEEC_Operation operation = {0};
    teec_result    result;
    hi_u32         origin = 0;

    operation.started = 1;
    operation.params[0].value.a = (hi_u32)(module_id);
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    result = TEEK_InvokeCommand(&g_teec_session,
                                TEEC_CMD_SSM_IOMMU_CONFIG,
                                &operation,
                                &origin);
    if (result != TEEC_SUCCESS) {
        HI_ERROR_SSM("TEEC invoke set iommu tag fail:%x\n", result);
        return result;
    }

    return HI_SUCCESS;
#else
    return HI_FAILURE;
#endif
}

hi_s32 hi_drv_ssm_iommu_config(hi_logic_mod_id mod_id)
{
    return drv_ssm_teec_iommu_config(mod_id);
}

hi_s32 hi_drv_ssm_attach_buffer(hi_drv_ssm_buf_attach_info attach_info, hi_u64 *sec_info_addr)
{
    SSM_TEEK_ATTACH_BUF_CTL attach_ctl = {0};
    dma_addr_t              smmu_addr;

    SSM_CHECK_POINTER_RETURN_IF_FAIL(sec_info_addr);

    attach_ctl.buf_id = attach_info.buf_id;
    attach_ctl.module_handle = attach_info.module_handle;
    attach_ctl.session_handle = attach_info.session_handle;

    smmu_addr = osal_mem_secsmmu_map(attach_info.dma_buf_addr, 0);
    if (smmu_addr == 0) {
        HI_ERROR_SSM("get smmu addr fail\n");
        return HI_FAILURE;
    }

    (hi_void)osal_mem_secsmmu_unmap(attach_info.dma_buf_addr, smmu_addr, 0);

    attach_ctl.buf_smmu_addr = smmu_addr;
    attach_ctl.buf_len = attach_info.dma_buf_addr->size;

    return do_teec_attach_buffer(&attach_ctl, sec_info_addr);
}

hi_void drv_ssm_error_handler(struct file *ssm_filp)
{
    hi_u32 i = 0;
    (hi_void)osal_mutex_lock(&g_ssm_lock);

    for (i = 0; i < SSM_MAX_SESSION_NUM; i++) {
        if (g_ssm_dev.chan_info[i].filp != ssm_filp) {
            continue;
        }

        (hi_void)do_ssm_destroy(g_ssm_dev.chan_info[i].ssm_handle);
    }

    (hi_void)osal_mutex_unlock(&g_ssm_lock);

    return;
}


EXPORT_SYMBOL(hi_drv_ssm_attach_buffer);
EXPORT_SYMBOL(hi_drv_ssm_iommu_config);


