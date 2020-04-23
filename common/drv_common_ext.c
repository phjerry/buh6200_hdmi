/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: common tee function impl.
 */

#include "linux/hisilicon/securec.h"

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_osal.h"

#include "hi_drv_sys.h"

#ifdef COMMON_TEE_SUPPORT
#include "teek_client_api.h"

#define TEEC_CMD_GET_VERSION 0

typedef struct {
    TEEC_Context context;
    TEEC_Session session;
    hi_bool  connected;
    osal_mutex lock;
} common_teec;

typedef struct {
    common_teec teec;
    hi_s32(*send_cmd_to_ta)(hi_u32 cmd_id, TEEC_Operation *operation, hi_u32 *ret_origin);
} common_comm;

static hi_bool g_is_tee_init = HI_FALSE;

static hi_s32 common_send_cmd_to_ta(hi_u32 cmd_id, TEEC_Operation *operation, hi_u32 *ret_origin);

static common_comm g_common_comm = {
    .send_cmd_to_ta = common_send_cmd_to_ta,
};

static hi_s32 common_init_teec(common_comm *comm)
{
    hi_s32 ret = HI_FAILURE;
    TEEC_UUID common_task_uuid = {
        0x000ac3b0, \
        0xbf6f, \
        0x11e7, \
        {0x8f, 0x1a, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66}
    };
    TEEC_Operation operation;
    hi_char general_session_name[] = "tee_common_session";
    hi_u32 root_id = 0;

    osal_mutex_lock(&comm->teec.lock);

    if (unlikely(comm->teec.connected == HI_TRUE)) {
        ret = HI_SUCCESS;
        goto out0;
    }

    ret = TEEK_InitializeContext(NULL, &comm->teec.context);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("Initialise common teec context failed(0x%x)", ret);
        goto out1;
    }

    if (memset_s(&operation, sizeof(TEEC_Operation), 0x0, sizeof(TEEC_Operation))) {
        HI_ERR_SYS("memset_s failed\n");
    }

    operation.started = 1;
    operation.cancel_flag = 0;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT);
    operation.params[0x2].tmpref.buffer = (void *)(&root_id);
    operation.params[0x2].tmpref.size = sizeof(root_id);
    operation.params[0x3].tmpref.buffer = (void *)(general_session_name);
    operation.params[0x3].tmpref.size = strlen(general_session_name) + 1;

    ret = TEEK_OpenSession(&comm->teec.context, &comm->teec.session, &common_task_uuid, TEEC_LOGIN_IDENTIFY, NULL,
                           &operation, NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("open common teec session failed(0x%x)\n", ret);
        goto out2;
    }

    comm->teec.connected = HI_TRUE;

    osal_mutex_unlock(&comm->teec.lock);

    return HI_SUCCESS;

out2:
    TEEK_FinalizeContext(&comm->teec.context);
out1:
    comm->teec.connected = HI_FALSE;
out0:
    osal_mutex_unlock(&comm->teec.lock);
    return ret;
}

static hi_void common_deinit_teec(common_comm *comm)
{
    osal_mutex_lock(&comm->teec.lock);

    if (comm->teec.connected == HI_TRUE) {
        comm->teec.connected = HI_FALSE;

        TEEK_CloseSession(&comm->teec.session);

        TEEK_FinalizeContext(&comm->teec.context);
    }

    osal_mutex_unlock(&comm->teec.lock);
}

/* common send_cmd_to_ta */
static hi_s32 common_send_cmd_to_ta(hi_u32 cmd_id, TEEC_Operation *operation, hi_u32 *ret_origin)
{
    hi_s32 ret = HI_FAILURE;
    common_comm *comm = &g_common_comm;

    if (unlikely(comm->teec.connected == HI_FALSE)) {
        ret = common_init_teec(comm);
        if (ret != HI_SUCCESS) {
            HI_ERR_SYS("common init teec failed!\n");
            goto out;
        }
    }

    ret = TEEK_InvokeCommand(&comm->teec.session, cmd_id, operation, ret_origin);

out:
    return ret;
}

/*
 * To get sos version, it is necessary to initialize COMMON TEE first
 * The Secure OS Version format: HiSTBLinuxV100R005C00SPC050_MSID_0x02_VER_0x02_20171019_20:49:10
 */
hi_s32 hi_drv_sys_get_sos_version(hi_char *version, hi_u32 version_len)
{
    hi_s32 ret;
    TEEC_Operation operation;
    common_comm *comm = &g_common_comm;
    hi_u32 mem_size = 0;
    hi_u32 mmz_size = 0;

    ret = hi_drv_sys_get_mem_cfg(&mem_size, &mmz_size);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("get mem config failed\n");
        return HI_FAILURE;
    }

    if (memset_s(version, version_len, 0x00, version_len)) {
        HI_ERR_SYS("memset_s failed\n");
    }

    operation.started = 1;
    operation.params[0].tmpref.buffer = (hi_void *)version;
    operation.params[0].tmpref.size   = version_len;
    operation.params[1].value.a = mem_size;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INOUT, TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE);

    ret = comm->send_cmd_to_ta(TEEC_CMD_GET_VERSION, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("send TEEC_CMD_GET_VERSION to TA failed(0x%x).\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* Init Tee common CA */
hi_s32 hi_drv_tee_common_init(hi_void)
{
    if (g_is_tee_init == HI_FALSE) {
        common_comm *comm = &g_common_comm;
        osal_mutex_init(&comm->teec.lock);
        common_init_teec(comm);
        g_is_tee_init = HI_TRUE;
    }

    return HI_SUCCESS;
}

/* Exit Tee common CA */
hi_void hi_drv_tee_common_exit(hi_void)
{
    if (g_is_tee_init == HI_TRUE) {
        common_deinit_teec(&g_common_comm);
        osal_mutex_destory(&g_common_comm.teec.lock);
        g_is_tee_init = HI_FALSE;
    }
}
#else
hi_s32 hi_drv_sys_get_sos_version(hi_char *version, hi_u32 version_len)
{
    return HI_SUCCESS;
}

hi_s32 hi_drv_tee_common_init(hi_void)
{
    return HI_SUCCESS;
}

hi_void hi_drv_tee_common_exit(hi_void)
{
}
#endif

