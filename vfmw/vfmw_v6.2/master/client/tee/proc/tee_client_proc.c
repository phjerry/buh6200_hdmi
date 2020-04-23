/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "tee_client_proc.h"
#include "vfmw_osal.h"
#include "tee_private.h"
#include "tee_client_comm.h"
#include "dbg.h"
#include "client_event_log.h"
#include "client_image_log.h"
#include "client_stream_log.h"

typedef struct {
    hi_s8 *proc;
} clt_proc_entry;

static clt_proc_entry g_clt_proc_entry;

static clt_proc_entry *proc_get_entry(hi_void)
{
    return &g_clt_proc_entry;
}

hi_s32 clt_proc_init(hi_s8 *proc)
{
    clt_proc_entry *entry = proc_get_entry();

    VFMW_CHECK_SEC_FUNC(memset_s(entry, sizeof(clt_proc_entry), 0, sizeof(clt_proc_entry)));

    entry->proc = proc;

    return HI_SUCCESS;
}

hi_void clt_proc_exit(hi_void)
{
    clt_proc_entry *entry = proc_get_entry();

    entry->proc = HI_NULL;
}

hi_s32 clt_proc_read(log_module module, hi_void *p, hi_s32 chan_id)
{
    hi_s32 used_size = 0;
    hi_s32 ret = HI_FAILURE;
    hi_u32 ret_val = 0;
    clt_proc_entry *entry = proc_get_entry();
    tee_vfmw_proc_params params;
    params.module_id = module;
    params.chan_id = chan_id;

    if (entry->proc != HI_NULL) {
        ret = tc_comm_send_command(VFMW_CMD_ID_READ_PROC, &params, &ret_val);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "Read Tee Proc Failed!\n");

            return HI_FAILURE;
        }

        OS_DUMP_PROC(p, 0, &used_size, 1, entry->proc);
    }

    return HI_SUCCESS;
}

hi_s32 clt_proc_write(log_module module, cmd_str_ptr cmd, hi_s32 count)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 ret_val = 0;
    tee_vfmw_proc_params params;
    clt_proc_entry *entry = proc_get_entry();

    params.module_id = module;
    params.write_count = count;

    if (entry->proc != HI_NULL) {
        VFMW_CHECK_SEC_FUNC(memcpy_s(entry->proc, CMD_STR_LEN, (*cmd), CMD_STR_LEN));

        ret = tc_comm_send_command(VFMW_CMD_ID_WRITE_PROC, &params, &ret_val);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "Read Tee Proc Failed!\n");

            return HI_FAILURE;
        }
    }

    return count;
}

hi_s32 clt_proc_read_share_mem(hi_void *p, hi_s32 chan_id)
{
    (hi_void)clt_evt_log_read(p, chan_id);
    (hi_void)clt_img_log_read(p, chan_id);
    (hi_void)clt_stm_log_read(p, chan_id);

    return HI_SUCCESS;
}