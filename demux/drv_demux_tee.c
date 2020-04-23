/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux tee function impl.
 * Author: sdk
 * Create: 2017-05-31
 */
#include <linux/kernel.h>
#include <linux/string.h>
#include "linux/mutex.h"
#include "linux/delay.h"
#include "linux/bitmap.h"
#include "linux/io.h"
#include "linux/interrupt.h"
#include "linux/printk.h"
#include "linux/ratelimit.h"

#include "linux/hisilicon/securec.h"
#include "teek_client_api.h"

#include "hi_debug.h"
#include "hi_type.h"

#include "hi_drv_module.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "hi_drv_mem.h"

#include "drv_demux_func.h"
#include "drv_demux_tee.h"
#include "drv_demux_utils.h"

/*
 * demux mgmt helper functions.
 */
#ifdef DMX_TEE_SUPPORT
#define DMX_REE_TEE_MAGIC        (0x5AA5)
#define DMX_REE_TEE_VERSION      "dmx_ver_01.01.01.00"
hi_s32 dmx_teec_init(struct dmx_r_teec *rteec)
{
    hi_s32 ret;
    TEEC_UUID dmx_task_uuid = {
        0xca0b4c78, \
        0xbcf2, \
        0x11e6, \
        {0xa4, 0xa6, 0xce, 0xc0, 0xc9, 0x32, 0xce, 0x01}
    };
    TEEC_Operation operation;
    hi_char general_session_name[] = "tee_dmx_general_session";
    dmx_ree_tee_version ree_tee_ver = {0};
    hi_u32 root_id = 0;
    if (osal_mutex_init(&rteec->lock) != 0) {
        HI_ERR_DEMUX("rteec lock init failed.");
    }
    osal_mutex_lock(&rteec->lock);

    if (unlikely(rteec->connected == HI_TRUE)) {
        ret = HI_SUCCESS;
        goto out0;
    }

    ret = TEEK_InitializeContext(NULL, &rteec->context);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("Initialise dmx teec context failed(0x%x)", ret);
        goto out1;
    }

    ree_tee_ver.magic = DMX_REE_TEE_MAGIC;
    ree_tee_ver.head_size = sizeof(dmx_ree_tee_version);
    if (memcpy_s(ree_tee_ver.version, sizeof(ree_tee_ver.version), DMX_REE_TEE_VERSION, sizeof(DMX_REE_TEE_VERSION))) {
        HI_ERR_DEMUX("memcpy_s failed!\n");
        ret = HI_FAILURE;
        goto out2;
    }
    if (memset_s(&operation, sizeof(TEEC_Operation), 0x0, sizeof(TEEC_Operation))) {
        HI_ERR_DEMUX("memset_s failed!\n");
        ret = HI_FAILURE;
        goto out2;
    }
    operation.started = 1;
    operation.cancel_flag = 0;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_MEMREF_TEMP_INOUT, TEEC_MEMREF_TEMP_INPUT,
                                            TEEC_MEMREF_TEMP_INPUT);
    operation.params[1].tmpref.buffer = (void *)(&ree_tee_ver);
    operation.params[1].tmpref.size = ree_tee_ver.head_size;
    operation.params[2].tmpref.buffer = (void *)(&root_id); /* index 2 */
    operation.params[2].tmpref.size = sizeof(root_id); /* index 2 */
    operation.params[3].tmpref.buffer = (void *)(general_session_name); /* index 3 */
    operation.params[3].tmpref.size = strlen(general_session_name) + 1; /* index 3 */

    ret = TEEK_OpenSession(&rteec->context, &rteec->session, &dmx_task_uuid, TEEC_LOGIN_IDENTIFY, NULL, &operation,
                           NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("open dmx teec session failed(0x%x)", ret);
        goto out2;
    }

    if (memcmp(ree_tee_ver.version, DMX_REE_TEE_VERSION, sizeof(DMX_REE_TEE_VERSION))) {
        HI_ERR_DEMUX("dmx ree version[%s] and tee version[%s] unmatched!\n", DMX_REE_TEE_VERSION, ree_tee_ver.version);
        ret = HI_FAILURE;
        goto out3;
    }

    rteec->connected = HI_TRUE;

    osal_mutex_unlock(&rteec->lock);

    return HI_SUCCESS;

out3:
    TEEK_CloseSession(&rteec->session);
out2:
    TEEK_FinalizeContext(&rteec->context);
out1:
    rteec->connected = HI_FALSE;
out0:
    osal_mutex_unlock(&rteec->lock);

    return ret;
}

hi_void dmx_teec_deinit(struct dmx_r_teec *rteec)
{
    osal_mutex_lock(&rteec->lock);

    if (rteec->connected == HI_TRUE) {
        rteec->connected = HI_FALSE;

        TEEK_CloseSession(&rteec->session);

        TEEK_FinalizeContext(&rteec->context);
    }

    osal_mutex_unlock(&rteec->lock);
    osal_mutex_destory(&rteec->lock);
}

hi_s32 dmx_mgmt_send_cmd_to_ta(hi_u32 cmd_id, TEEC_Operation *operation, hi_u32 *ret_origin)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();

    osal_mutex_lock(&rmgmt->teec.lock);

    if (unlikely(rmgmt->teec.connected == HI_FALSE)) {
        ret = dmx_teec_init(&rmgmt->teec);
        if (ret != HI_SUCCESS) {
            goto out;
        }
    }

    ret = TEEK_InvokeCommand(&rmgmt->teec.session, cmd_id, operation, ret_origin);

out:
    osal_mutex_unlock(&rmgmt->teec.lock);
    return ret;
}

hi_void dmx_teec_open(hi_void)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    TEEC_Operation operation = {0};

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_INIT, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_INIT to TA failed(0x%x).\n", ret);
    }

    return;
}

hi_void dmx_teec_close(hi_void)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    TEEC_Operation operation = {0};

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_DEINIT, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_DEINIT to TA failed(0x%x).\n", ret);
    }

    return;
}

hi_s32 dmx_tee_create_ramport(hi_u32 ram_id, hi_u32 buf_size, hi_u32 flush_buf_size, hi_u32 dsc_buf_size,
    dmx_tee_ramport_info *ramport_info_ptr)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    dmx_tee_ramport_info tee_ramport_info = {0};
    TEEC_Operation operation = {0};

    DMX_NULL_POINTER_RETURN(ramport_info_ptr);

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE);

    operation.params[0].value.a = ram_id;
    operation.params[0].value.b = buf_size;
    operation.params[1].value.a = flush_buf_size;
    operation.params[1].value.b = dsc_buf_size;

    operation.params[2].tmpref.buffer = (hi_void *)&tee_ramport_info; /* index 2 */
    operation.params[2].tmpref.size   = sizeof(dmx_tee_ramport_info); /* index 2 */

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_CREATE_RAMPORT, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_CREATE_RAMPORT to TA failed(0x%x).\n", ret);
        ret = HI_FAILURE;
        goto out;
    }

    ramport_info_ptr->buf_handle = tee_ramport_info.buf_handle;
    ramport_info_ptr->buf_phy_addr = tee_ramport_info.buf_phy_addr;
    ramport_info_ptr->buf_size = tee_ramport_info.buf_size;
    ramport_info_ptr->flush_buf_phy_addr = tee_ramport_info.flush_buf_phy_addr;
    ramport_info_ptr->flush_buf_size = tee_ramport_info.flush_buf_size;
    ramport_info_ptr->dsc_buf_phy_addr = tee_ramport_info.dsc_buf_phy_addr;
    ramport_info_ptr->dsc_buf_size = tee_ramport_info.dsc_buf_size;

    HI_DBG_DEMUX("Buf handle[%#x], buf phy addr[%#llx], buf size[%#x], flush buf addr[%#llx], flush buf size,"
                 " dsc buf addr[%#llx], dsc buf size[%#x],\n",
                 ramport_info_ptr->buf_handle, ramport_info_ptr->buf_phy_addr, ramport_info_ptr->buf_size,
                 ramport_info_ptr->flush_buf_phy_addr, ramport_info_ptr->flush_buf_size,
                 ramport_info_ptr->dsc_buf_phy_addr, ramport_info_ptr->dsc_buf_size);

out:
    return ret;
}

hi_void dmx_tee_destroy_ramport(hi_u32 ram_id, const dmx_tee_ramport_info *ramport_info_ptr)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    dmx_tee_ramport_info tee_ramport_info = {0};
    TEEC_Operation operation = {0};

    DMX_NULL_POINTER_VOID(ramport_info_ptr);

    tee_ramport_info.buf_handle = ramport_info_ptr->buf_handle;
    tee_ramport_info.buf_phy_addr = ramport_info_ptr->buf_phy_addr;
    tee_ramport_info.buf_size = ramport_info_ptr->buf_size;

    tee_ramport_info.flush_buf_phy_addr = ramport_info_ptr->flush_buf_phy_addr;
    tee_ramport_info.flush_buf_size = ramport_info_ptr->flush_buf_size;

    tee_ramport_info.dsc_buf_phy_addr = ramport_info_ptr->dsc_buf_phy_addr;
    tee_ramport_info.dsc_buf_size = ramport_info_ptr->dsc_buf_size;

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE);

    operation.params[0].value.a = ram_id;

    operation.params[1].tmpref.buffer = (hi_void *)&tee_ramport_info;
    operation.params[1].tmpref.size   = sizeof(dmx_tee_ramport_info);

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_DESTROY_RAMPORT, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_DESTROY_RAMPORT to TA failed(0x%x).\n", ret);
        return;
    }

    HI_DBG_DEMUX("Buf handle[%#x], buf phy addr[%#llx], buf size[%#x], flush buf addr[%#llx], flush buf size,"
                 " dsc buf addr[%#llx], dsc buf size[%#x],\n",
                 ramport_info_ptr->buf_handle, ramport_info_ptr->buf_phy_addr, ramport_info_ptr->buf_size,
                 ramport_info_ptr->flush_buf_phy_addr, ramport_info_ptr->flush_buf_size,
                 ramport_info_ptr->dsc_buf_phy_addr, ramport_info_ptr->dsc_buf_size);

    return;
}

hi_s32 dmx_tee_set_ramport_dsc(hi_u32 ram_id, const dmx_tee_ramport_dsc *ram_port_dsc)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    dmx_tee_ramport_dsc tee_ramport_dsc = {0};
    TEEC_Operation operation = {0};

    DMX_NULL_POINTER_RETURN(ram_port_dsc);

    tee_ramport_dsc = *ram_port_dsc;

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE);

    operation.params[0].value.a = ram_id;

    operation.params[1].tmpref.buffer = (hi_void *)&tee_ramport_dsc;
    operation.params[1].tmpref.size   = sizeof(dmx_tee_ramport_dsc);

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_SET_RAMPORT_DSC, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_SET_RAMPORT_DSC to TA failed(0x%x).\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 dmx_tee_create_play_chan(hi_u32 chan_id, dmx_play_type play_type, hi_u32 buf_size,
    dmx_tee_mem_info *buf_info_ptr)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    dmx_tee_mem_info tee_mem_info = {0};
    TEEC_Operation operation = {0};

    DMX_NULL_POINTER_RETURN(buf_info_ptr);

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE);

    operation.params[0].value.a = chan_id;
    operation.params[0].value.b = play_type;
    operation.params[1].value.a = buf_size;

    operation.params[2].tmpref.buffer = (hi_void *)&tee_mem_info; /* index 2 */
    operation.params[2].tmpref.size   = sizeof(dmx_tee_mem_info); /* index 2 */

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_CREATE_PLAY_CHAN, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_CREATE_PLAY_CHAN to TA failed(0x%x).\n", ret);
        ret = HI_FAILURE;
        goto out;
    }

    buf_info_ptr->handle = tee_mem_info.handle;
    buf_info_ptr->buf_phy_addr = tee_mem_info.buf_phy_addr;
    buf_info_ptr->buf_size = tee_mem_info.buf_size;
    buf_info_ptr->buf_id = tee_mem_info.buf_id;

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_size);

out:
    return ret;
}

hi_void dmx_tee_destroy_play_chan(hi_u32 chan_id, dmx_play_type play_type, const dmx_tee_mem_info *buf_info_ptr)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    dmx_tee_mem_info tee_mem_info = {0};
    TEEC_Operation operation = {0};

    DMX_NULL_POINTER_VOID(buf_info_ptr);

    tee_mem_info.handle = buf_info_ptr->handle;
    tee_mem_info.buf_phy_addr = buf_info_ptr->buf_phy_addr;
    tee_mem_info.buf_size = buf_info_ptr->buf_size;
    tee_mem_info.buf_id = buf_info_ptr->buf_id;

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE);

    operation.params[0].value.a = chan_id;
    operation.params[0].value.b = play_type;

    operation.params[1].tmpref.buffer = (hi_void *)&tee_mem_info;
    operation.params[1].tmpref.size   = sizeof(dmx_tee_mem_info);

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_DESTROY_PLAY_CHAN, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_DESTROY_PLAY_CHAN to TA failed(0x%x).\n", ret);
        return;
    }

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_size);

    return;
}

hi_s32 dmx_tee_attach_play_chan(hi_u32 chan_id, dmx_play_type play_type, hi_u32 raw_pidch_id,
    hi_u32 master_raw_pidch_id)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    TEEC_Operation operation = {0};

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE);

    operation.params[0].value.a = chan_id;
    operation.params[0].value.b = play_type;

    operation.params[1].value.a = raw_pidch_id;
    operation.params[1].value.b = master_raw_pidch_id;

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_ATTACH_PLAY_CHAN, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_CREATE_PLAY_CHAN to TA failed(0x%x).\n", ret);
        ret = HI_FAILURE;
        goto out;
    }

out:
    return ret;
}

hi_void dmx_tee_detach_play_chan(hi_u32 chan_id, dmx_play_type play_type, hi_u32 raw_pidch_id)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    TEEC_Operation operation = {0};

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE);

    operation.params[0].value.a = chan_id;
    operation.params[0].value.b = play_type;

    operation.params[1].value.a = raw_pidch_id;

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_DETACH_PLAY_CHAN, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_DETACH_PLAY_CHAN to TA failed(0x%x).\n", ret);
        return;
    }

    return;
}

hi_s32 dmx_tee_create_rec_chan(hi_u32 chan_id, hi_u32 buf_size, dmx_tee_mem_info *buf_info_ptr)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    dmx_tee_mem_info tee_mem_info = {0};
    TEEC_Operation operation = {0};

    DMX_NULL_POINTER_RETURN(buf_info_ptr);

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);

    operation.params[0].value.a = chan_id;
    operation.params[0].value.b = buf_size;

    operation.params[1].tmpref.buffer = (hi_void *)&tee_mem_info;
    operation.params[1].tmpref.size   = sizeof(dmx_tee_mem_info);

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_CREATE_REC_CHAN, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_CREATE_PLAY_CHAN to TA failed(0x%x).\n", ret);
        ret = HI_FAILURE;
        goto out;
    }

    buf_info_ptr->handle = tee_mem_info.handle;
    buf_info_ptr->buf_phy_addr = tee_mem_info.buf_phy_addr;
    buf_info_ptr->buf_size = tee_mem_info.buf_size;
    buf_info_ptr->buf_id = tee_mem_info.buf_id;

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_size);

out:
    return ret;
}

hi_void dmx_tee_destroy_rec_chan(hi_u32 chan_id, const dmx_tee_mem_info *buf_info_ptr)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    dmx_tee_mem_info tee_mem_info = {0};
    TEEC_Operation operation = {0};

    DMX_NULL_POINTER_VOID(buf_info_ptr);

    tee_mem_info.handle = buf_info_ptr->handle;
    tee_mem_info.buf_phy_addr = buf_info_ptr->buf_phy_addr;
    tee_mem_info.buf_size = buf_info_ptr->buf_size;
    tee_mem_info.buf_id = buf_info_ptr->buf_id;

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE);

    operation.params[0].value.a = chan_id;

    operation.params[1].tmpref.buffer = (hi_void *)&tee_mem_info;
    operation.params[1].tmpref.size   = sizeof(dmx_tee_mem_info);

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_DESTROY_REC_CHAN, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_DESTROY_REC_CHAN to TA failed(0x%x).\n", ret);
        return;
    }

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_size);

    return;
}

hi_s32 dmx_tee_attach_rec_chan(const dmx_tee_rec_attach_info *rec_attach_ptr)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    TEEC_Operation operation = {0};
    dmx_tee_rec_attach_info rec_attach_info = {0};

    DMX_NULL_POINTER_RETURN(rec_attach_ptr);

    rec_attach_info = *rec_attach_ptr;

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    operation.params[0].tmpref.buffer = (hi_void *)&rec_attach_info;
    operation.params[0].tmpref.size   = sizeof(dmx_tee_rec_attach_info);

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_ATTACH_REC_CHAN, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_CREATE_PLAY_CHAN to TA failed(0x%x).\n", ret);
        ret = HI_FAILURE;
        goto out;
    }

out:
    return ret;
}

hi_void dmx_tee_detach_rec_chan(const dmx_tee_rec_detach_info *rec_detach_ptr)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    TEEC_Operation operation = {0};
    dmx_tee_rec_detach_info rec_detach_info = {0};

    DMX_NULL_POINTER_VOID(rec_detach_ptr);

    rec_detach_info = *rec_detach_ptr;

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    operation.params[0].tmpref.buffer = (hi_void *)&rec_detach_info;
    operation.params[0].tmpref.size   = sizeof(dmx_tee_rec_attach_info);

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_DETACH_REC_CHAN, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_DESTROY_PLAY_CHAN to TA failed(0x%x).\n", ret);
        return;
    }

    return;
}

hi_s32 dmx_tee_update_play_buf_read_idx(hi_u32 buf_id, dmx_play_type play_type, hi_u32 read_idx)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    TEEC_Operation operation = {0};

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE);

    operation.params[0].value.a = buf_id;
    operation.params[0].value.b = play_type;
    operation.params[1].value.a = read_idx;

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_UPDATE_PLAY_READ_IDX, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_UPDATE_PLAY_READ_IDXs to TA failed(0x%x).\n", ret);
        ret = HI_FAILURE;
        goto out;
    }

out:
    return ret;
}

hi_s32 dmx_tee_update_rec_buf_read_idx(hi_u32 buf_id, hi_u32 read_idx)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    TEEC_Operation operation = {0};

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    operation.params[0].value.a = buf_id;
    operation.params[0].value.b = read_idx;

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_UPDATE_REC_READ_IDX, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_UPDATE_REC_READ_IDX to TA failed(0x%x).\n", ret);
        ret = HI_FAILURE;
        goto out;
    }

out:
    return ret;
}

hi_s32 dmx_tee_acquire_bufid(hi_u32 *bufid)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    TEEC_Operation operation = {0};

    DMX_NULL_POINTER_RETURN(bufid);

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_OUTPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_ACQUIRE_SECBUF_ID, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_ACQUIRE_SECBUF_ID to TA failed(0x%x).\n", ret);
        ret = HI_FAILURE;
        goto out;
    }

    *bufid = operation.params[0].value.a;

out:
    return ret;
}

hi_void dmx_tee_release_bufid(const hi_u32 bufid)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    TEEC_Operation operation = {0};

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    operation.params[0].value.a = bufid;

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_RELEASE_SECBUF_ID, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_UNREGISTER_SECBUF to TA failed(0x%x).\n", ret);
        return;
    }

    return;
}

hi_s32 dmx_tee_config_buf(hi_u32 chan_id, dmx_play_type chan_type)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    TEEC_Operation operation = {0};

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    operation.params[0].value.a = chan_id;
    operation.params[0].value.b = chan_type;

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_CONFIG_SECBUF, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_CONFIG_SECBUF to TA failed(0x%x).\n", ret);
        ret = HI_FAILURE;
        goto out;
    }

out:
    return ret;
}

hi_void dmx_tee_deconfig_buf(hi_u32 chan_id, dmx_play_type chan_type)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    TEEC_Operation operation = {0};

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    operation.params[0].value.a = chan_id;
    operation.params[0].value.b = chan_type;

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_DECONFIG_SECBUF, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_DECONFIG_SECBUF to TA failed(0x%x).\n", ret);
        return;
    }

    return;
}

hi_void dmx_tee_detach_raw_pidch(hi_u32 raw_pidch)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    TEEC_Operation operation = {0};

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    operation.params[0].value.a = raw_pidch;

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_DETACH_RAW_PIDCH, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_UNREGISTER_SECBUF to TA failed(0x%x).\n", ret);
        return;
    }

    return;
}

hi_s32 dmx_tee_fixup_hevc_es_index(hi_u32 rec_id, hi_u32 idx_pid, hi_u32 parse_offset, const findex_scd *pst_fidx,
    dmx_index_data *cur_frame)
{
    hi_s32 ret;
    struct dmx_mgmt *rmgmt = get_dmx_mgmt();
    TEEC_Operation operation = {0};

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_INPUT,
                                            TEEC_MEMREF_TEMP_OUTPUT);
    operation.params[0].value.a = rec_id;
    operation.params[1].value.a = idx_pid;
    operation.params[1].value.b = parse_offset;
    operation.params[2].tmpref.buffer = (hi_void *)pst_fidx; /* index 2 */
    operation.params[2].tmpref.size   = sizeof(findex_scd); /* index 2 */

    operation.params[3].tmpref.buffer = (hi_void *)cur_frame; /* index 3 */
    operation.params[3].tmpref.size = sizeof(dmx_index_data); /* index 3 */

    ret = rmgmt->ops->send_cmd_to_ta(TEEC_CMD_FIXUP_HEVC_INDEX, &operation, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("send TEEC_CMD_FIXUP_HEVC_INDEX to TA failed(0x%x).\n", ret);
        ret = HI_FAILURE;
        goto out;
    }

out:
    return ret;
}
#endif

