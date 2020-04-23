/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "mdc_client_proc.h"
#include "mdc_client_adpt.h"

#include "mdc_client_log.h"
#include "mdc_client_comm_log.h"
#include "mdc_client_adpt_log.h"
#include "client_event_log.h"
#include "client_image_log.h"
#include "client_stream_log.h"
#include "vfmw_sys_mdc.h"
#include "mdc_private.h"
#include "vfmw_osal.h"
#include "dbg.h"

typedef struct {
    OS_SEMA sema;
    hi_s8 *proc;
    hi_void *comm_shr_mem;
    hi_u32 mem_length;
} mdc_client_proc_entry;

static mdc_client_proc_entry g_mdc_clt_proc_entry;

static mdc_client_proc_entry *mdc_clt_proc_get_entry(hi_void)
{
    return &g_mdc_clt_proc_entry;
}

STATIC hi_s32 mdc_log_read_clt(hi_void *buf, hi_s32 chan_id)
{
    hi_s32 ret;

    ret = mdc_log_read_client(buf, chan_id);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "read_client Proc Failed!\n");
    }

    ret = mdc_log_read_client_adapt(buf, chan_id);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "read_client_adapt Proc Failed!\n");
    }

    ret = mdc_log_read_client_comm(buf, chan_id);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "read_client_comm Proc Failed!\n");
    }

    return ret;
}

hi_s32 mdc_log_get_log(hi_void *p)
{
    hi_s32 used_size = 0;
    mdc_log_option *option = OS_PHYS_TO_VIRT(MDC_LOG_OUTPUT_ADDR);
    hi_s8 *log_vir_addr = (hi_s8 *)option;

    if (option == HI_NULL) {
        return HI_FAILURE;
    }

    if (option->magic1 == MDC_LOG_MAGIC1 && option->magic2 == MDC_LOG_MAGIC2) {
        if (option->write_head_s != option->write_tail_s) {
            *(log_vir_addr + option->write_head_s) = '\0';
            OS_DUMP_PROC(p, 0, &used_size, 1, log_vir_addr + option->read_c);
            option->read_c = option->write_head_s;
        } else {
            if (option->read_c < option->write_head_s) {
                *(log_vir_addr + option->write_head_s) = '\0';
                OS_DUMP_PROC(p, 0, &used_size, 1, log_vir_addr + option->read_c);
                option->read_c = option->write_head_s;
            } else {
                *(log_vir_addr + option->end_pos) = '\0';
                OS_DUMP_PROC(p, 0, &used_size, 1, log_vir_addr + option->read_c);
                *(log_vir_addr + option->write_head_s) = '\0';
                OS_DUMP_PROC(p, 0, &used_size, 1, log_vir_addr + sizeof(mdc_log_option));
                option->read_c = option->write_head_s;
            }
        }
    }

    return HI_SUCCESS;
}

hi_s32 mdc_clt_proc_open(hi_void)
{
    mdc_client_proc_entry *entry = mdc_clt_proc_get_entry();

    VFMW_CHECK_SEC_FUNC(memset_s(entry, sizeof(mdc_client_proc_entry), 0, sizeof(mdc_client_proc_entry)));

    OS_SEMA_INIT(&entry->sema);

    return HI_SUCCESS;
}

hi_void mdc_clt_proc_close(hi_void)
{
    mdc_client_proc_entry *entry = mdc_clt_proc_get_entry();

    OS_SEMA_EXIT(entry->sema);

    VFMW_CHECK_SEC_FUNC(memset_s(entry, sizeof(mdc_client_proc_entry), 0, sizeof(mdc_client_proc_entry)));

    return;
}

hi_s32 mdc_clt_proc_init(hi_s8 *proc, hi_void *comm_shr_mem, hi_u32 mem_length)
{
    mdc_client_proc_entry *entry = mdc_clt_proc_get_entry();

    OS_SEMA_DOWN(entry->sema);

    entry->proc = proc;
    entry->comm_shr_mem = comm_shr_mem;
    entry->mem_length = mem_length;

    OS_SEMA_UP(entry->sema);

    return HI_SUCCESS;
}

hi_void mdc_clt_proc_exit(hi_void)
{
    mdc_client_proc_entry *entry = mdc_clt_proc_get_entry();

    OS_SEMA_DOWN(entry->sema);

    entry->proc = HI_NULL;
    entry->comm_shr_mem = HI_NULL;
    entry->mem_length = 0;

    OS_SEMA_UP(entry->sema);

    return;
}

hi_s32 mdc_clt_proc_read(hi_s32 module, hi_void *p, hi_s32 chan_id)
{
    hi_s32 used_size = 0;
    hi_s32 ret = HI_SUCCESS;
    mdc_proc_read_param proc = {0};
    comm_cmd_param param;
    mdc_client_proc_entry *entry = mdc_clt_proc_get_entry();

    OS_SEMA_DOWN(entry->sema);

    if (entry->proc != HI_NULL) {
        proc.module = module;
        param.chan_id = chan_id;
        param.service_id = MDC_SID_DFX;
        param.command_id = MDC_CID_PROC_READ;
        param.args = &proc;
        param.length = sizeof(proc);
        param.valid_comm_mem = entry->comm_shr_mem;
        param.comm_mem_length = entry->mem_length;
        ret = mdc_adpt_send_block(&param);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "Read Mcu Proc Failed!\n");
            OS_SEMA_UP(entry->sema);
            return HI_FAILURE;
        }
        OS_DUMP_PROC(p, 0, &used_size, 1, entry->proc);
    }

    OS_SEMA_UP(entry->sema);

    return ret;
}

hi_s32 mdc_clt_proc_write(hi_s32 module, cmd_str_ptr cmd, hi_s32 count)
{
    hi_s32 ret;
    mdc_proc_write_param proc = {0};
    comm_cmd_param param;
    mdc_client_proc_entry *entry = mdc_clt_proc_get_entry();

    proc.module = module;
    proc.count = count;
    VFMW_CHECK_SEC_FUNC(memcpy_s(proc.cmd_str, CMD_STR_LEN, cmd, CMD_STR_LEN));
    param.chan_id = 0;
    param.service_id = MDC_SID_DFX;
    param.command_id = MDC_CID_PROC_WRITE;
    param.args = &proc;
    param.length = sizeof(proc);
    param.valid_comm_mem = entry->comm_shr_mem;
    param.comm_mem_length = entry->mem_length;
    ret = mdc_adpt_send_block(&param);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "Write Mcu Proc Failed!\n");
    }

    return ret;
}

hi_s32 mdc_clt_proc_read_mdc(hi_void *p, hi_s32 chan_id)
{
    hi_s32 ret;

    /* client info */
    ret = mdc_log_read_clt(p, chan_id);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "Read mdc client proc failed!");
    }

    /* server info */
    ret = mdc_clt_proc_read(LOG_MODULE_MDC, p, chan_id);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "Read mdc server proc failed!");
    }

    return HI_SUCCESS;
}

hi_s32 mdc_clt_proc_write_mdc(cmd_str_ptr cmd_str, hi_u32 cmd_cnt)
{
    hi_s32 ret;

    ret = mdc_clt_proc_write(LOG_MODULE_MDC, cmd_str, cmd_cnt);

    return ret;
}
