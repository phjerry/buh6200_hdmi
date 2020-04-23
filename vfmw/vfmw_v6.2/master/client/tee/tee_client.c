/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "tee_client.h"
#include "comm_share.h"
#include "client_image.h"
#include "client_event.h"
#include "client_state.h"
#include "client_stream.h"
#include "tee_client_comm.h"
#ifdef VFMW_PROC_SUPPORT
#include "tee_client_proc.h"
#endif
#include "tee_private.h"
#include "vfmw_osal.h"
#include "dbg.h"
#include <linux/vmalloc.h>

#ifdef PRODUCT_STB
#define SECURE_NOTIFY_IRQ_NUM (146 + 32)
#else
#define SECURE_NOTIFY_IRQ_NUM (21 + 32)
#endif

typedef enum {
    CHAN_INVALID = 0,
    CHAN_START,
    CHAN_STOP,
    CHAN_BUTT,
} chan_state;

typedef enum {
    THREAD_INVALID = 0,
    THREAD_SLEEP,
    THREAD_RUN,
    THREAD_EXIT,
    THREAD_BUTT,
} thread_state;

typedef struct {
    hi_s32 sec_mode;
    hi_s32 output_img_en;
    hi_s32 chan_id;
    chan_state chan_state;
    OS_SEMA chan_sema;
    OS_FILE *raw_file;
    OS_LOCK chan_lock;
    vfmw_shr_chn_mem chan_share_mem;
    hi_s32 valid;
} chan_context;

typedef struct {
    hi_s32 env_set_up;
    hi_s32 init_cnt;
    shr_glb_mem glb_share_mem;
    hi_s32 print_enable;
    hi_s32 seg_by_pass;
    chan_context chan_context[VFMW_CHAN_NUM];
    hi_s32 inst_num;
    thread_state thread_state;
    vfmw_chan_option *chan_option;
    hi_s8 *proc;
    OS_EVENT thread_event;
    hi_s32 notify_irq_handle;
    hi_s32 print_time;
} tee_context;

typedef struct {
    tee_context ctx;
    OS_SEMA sem;
} tee_entry;

static tee_entry g_tee_entry;

/* ----------------------------INTERNAL FUNCTION------------------------------- */
static INLINE tee_entry *client_get_entry(hi_void)
{
    return &g_tee_entry;
}

static hi_void client_init_tee_context(tee_entry *entry)
{
    VFMW_CHECK_SEC_FUNC(memset_s(&entry->ctx, sizeof(tee_context), 0, sizeof(tee_context)));
}

static hi_void client_init_tee_sema(tee_entry *entry)
{
    OS_SEMA_INIT(&entry->sem);
}

static hi_void client_alloc_tee_sema(tee_entry *entry)
{
    OS_SEMA_DOWN(entry->sem);
}

static hi_void client_release_tee_sema(tee_entry *entry)
{
    OS_SEMA_UP(entry->sem);
}

static hi_void client_exit_tee_sema(tee_entry *entry)
{
    OS_SEMA_EXIT(entry->sem);
}

static hi_void client_init_entry(tee_entry *entry)
{
    client_init_tee_context(entry);
    entry->ctx.seg_by_pass = 1;

    return;
}

static hi_void client_part_glb_share_mem(shr_glb_mem *mem)
{
    mem->chan_option_phy_addr = mem->glb_shr_mem_desc.phy_addr;
    mem->chan_option_vir_addr = mem->glb_shr_mem_desc.vir_addr;
    if (SHR_PROC_SIZE > 0) {
        mem->proc_phy_addr = mem->chan_option_phy_addr + SHR_CHAN_OPTION_SIZE;
        mem->proc_vir_addr = mem->chan_option_vir_addr + SHR_CHAN_OPTION_SIZE;
    }

    return;
}

static hi_s32 client_alloc_glb_share_mem(tee_entry *entry)
{
    hi_s32 ret;
    vfmw_mem_desc *mem = &entry->ctx.glb_share_mem.glb_shr_mem_desc;
    hi_s32 mem_size = 0;

    mem_size += SHR_GLBMEM_SIZE;

    ret = OS_KMEM_ALLOC("VFMW_Glb_Shr_Buf", mem_size, 4, 0, mem); /* 4: align size */
    if (ret != OSAL_OK) {
        dprint(PRN_ERROR, "VFMW_Global_Share_Buf Alloc Failed!\n");
        return HI_FAILURE;
    }

    client_part_glb_share_mem(&entry->ctx.glb_share_mem);

    entry->ctx.chan_option = (vfmw_chan_option *)UINT64_PTR(entry->ctx.glb_share_mem.chan_option_vir_addr);
    entry->ctx.proc = (hi_s8 *)UINT64_PTR(entry->ctx.glb_share_mem.proc_vir_addr);
#ifdef VFMW_PROC_SUPPORT
    (hi_void)clt_proc_init(entry->ctx.proc);
#endif

    return HI_SUCCESS;
}

static hi_void client_free_glb_share_mem(tee_entry *entry)
{
    OS_KMEM_FREE(&entry->ctx.glb_share_mem.glb_shr_mem_desc);
    VFMW_CHECK_SEC_FUNC(memset_s(&entry->ctx.glb_share_mem, sizeof(shr_glb_mem), 0, sizeof(shr_glb_mem)));
    clt_proc_exit();
    entry->ctx.chan_option = HI_NULL;
}

static hi_s32 client_vfmw_thread(hi_void *args)
{
    hi_s32 next_inst_id;
    hi_s32 ret;

    static hi_u32 begin_time = 0;
    static hi_u32 start_time = 0;
    static hi_u32 end_time = 0;
    static hi_u32 total_time = 0;
    static hi_u32 count = 0;

    tee_entry *entry = client_get_entry();
    hi_u32 last_start_time = OS_GET_TIME_US();
    hi_u32 current_time = 0;

    while (1) {
        if (entry->ctx.print_time) {
            current_time = OS_GET_TIME_US();
            if (current_time - last_start_time > 1000 * 15) {  /* 1000, 15: time cost , ms */
                dprint(PRN_ALWS, "NS: last time sleep exceed cost:   %d   us\n", current_time - last_start_time);
            }
        }

        client_alloc_tee_sema(entry);

        switch (entry->ctx.thread_state) {
            case THREAD_RUN:
                break;

            case THREAD_SLEEP:
                goto sleep;

            case THREAD_EXIT:
                goto exit;

            default:
                break;
        }

        for (next_inst_id = 0; next_inst_id < VFMW_CHAN_NUM; next_inst_id++) {
            if (entry->ctx.chan_context[next_inst_id].chan_state == CHAN_START) {
                OS_SEMA_DOWN(entry->ctx.chan_context[next_inst_id].chan_sema);
                clt_stm_read(next_inst_id);
                OS_SEMA_UP(entry->ctx.chan_context[next_inst_id].chan_sema);
            }
        }

        if (entry->ctx.print_time) {
            start_time = OS_GET_TIME_US();
            if (count == 0) {
                begin_time = start_time;
            }
        }

        ret = tc_comm_send_command(VFMW_CMD_ID_THREAD, HI_NULL, HI_NULL);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "Call VFMW_CMD_ID_THREAD Failed!\n");
            client_release_tee_sema(entry);

            return HI_FAILURE;
        }

        if (entry->ctx.print_time) {
            end_time = OS_GET_TIME_US();
            total_time += end_time - start_time;
            count++;

            if (end_time - begin_time >= 1000000) { /* 1000000: time cost , us */
                dprint(PRN_ALWS, "NS: Invoke Total:   %d   count: %d  Avg:    %d     us\n",
                       total_time, count, total_time / count);
                total_time = begin_time = start_time = end_time = 0;
                count = 0;
            }

            if (end_time - start_time >= 1000 * 10) { /* 1000¡ê? 10: time cost , ms */
                dprint(PRN_ALWS, "NS: Invoke exceed cost:  %d  start:  %d   end:   %d   us\n",
                       end_time - start_time, start_time, end_time);
            }
        }

        for (next_inst_id = 0; next_inst_id < VFMW_CHAN_NUM; next_inst_id++) {
            if (entry->ctx.chan_context[next_inst_id].chan_state == CHAN_START) {
                OS_SEMA_DOWN(entry->ctx.chan_context[next_inst_id].chan_sema);
                (hi_void)clt_evt_process(next_inst_id);
                (hi_void)clt_stm_release(next_inst_id);
                OS_SEMA_UP(entry->ctx.chan_context[next_inst_id].chan_sema);
            }
        }

sleep:
        last_start_time = OS_GET_TIME_US();

        if (entry->ctx.print_time) {
            if (last_start_time - current_time >= 1000 * 10) { /* 1000¡ê? 10: time cost , ms */
                dprint(PRN_ALWS, "NS: run NS thread exceed cost:  %d \n", last_start_time - current_time);
            }
        }
        client_release_tee_sema(entry);
        OS_EVENT_WAIT(entry->ctx.thread_event, 10); /* 10: time wait , ms */
    }

exit:
    client_release_tee_sema(entry);
    entry->ctx.thread_state = THREAD_INVALID;

    return HI_SUCCESS;
}

static hi_s32 client_init_thread(tee_entry *entry)
{
    hi_void *handle = HI_NULL;

    OS_EVENT_INIT(&entry->ctx.thread_event, 0);

    handle = OS_CREATE_THREAD(client_vfmw_thread, HI_NULL, "SecVideoDec");
    if (handle == HI_NULL) {
        dprint(PRN_ERROR, "Create Thread Failed!\n");

        return HI_FAILURE;
    }

    entry->ctx.thread_state = THREAD_SLEEP;

    return HI_SUCCESS;
}

static hi_s32 client_notify_irq_handler(hi_s32 irq, hi_void *dev_id)
{
    tee_entry *entry = client_get_entry();

    OS_EVENT_GIVE(entry->ctx.thread_event);

    return 1;
}

static hi_s32 client_request_irq(tee_entry *entry)
{
    hi_s32 ret;
    hi_s32 irq_flag = 0x80;

    ret = OS_REQUEST_IRQ(SECURE_NOTIFY_IRQ_NUM, client_notify_irq_handler, irq_flag, "SecInvokeirq",
                         &entry->ctx.notify_irq_handle);
    if (ret != OSAL_OK) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void client_free_irq(tee_entry *entry)
{
    OS_FREE_IRQ(SECURE_NOTIFY_IRQ_NUM, "SecInvokeirq", &entry->ctx.notify_irq_handle);
}

static hi_void client_destroy_vfmw_thread(tee_entry *entry)
{
    hi_s32 i = 0;

    entry->ctx.thread_state = THREAD_EXIT;

    for (i = 0; i < 50; i++) { /* 50 :a number */
        if (entry->ctx.thread_state == THREAD_INVALID) {
            break;
        } else {
            OS_MSLEEP(10); /* 10 :a number */
        }
    }
}

static hi_void client_prepare_init_args(tee_entry *entry, vfmw_init_param *args, vfmw_init_param *src_args)
{
    VFMW_CHECK_SEC_FUNC(memcpy_s(args, sizeof(vfmw_init_param), src_args, sizeof(vfmw_init_param)));
    VFMW_CHECK_SEC_FUNC(memset_s(&args->ext_hal_mem, sizeof(vfmw_mem_desc), 0, sizeof(vfmw_mem_desc)));

    args->share_data.glb_shr_base_addr = entry->ctx.glb_share_mem.glb_shr_mem_desc.phy_addr;
    args->share_data.glb_shr_mem_length = entry->ctx.glb_share_mem.glb_shr_mem_desc.length;
    args->share_data.chan_option_addr = entry->ctx.glb_share_mem.chan_option_phy_addr;
    args->share_data.proc_addr = entry->ctx.glb_share_mem.proc_phy_addr;
    args->share_data.print_word = entry->ctx.print_enable;
    args->share_data.seg_bypass_enbale = entry->ctx.seg_by_pass;
}

static hi_s32 client_init_modules(hi_void)
{
    hi_s32 ret;

    (hi_void)clt_img_init();
    (hi_void)clt_evt_init();
    (hi_void)clt_stm_init();
    (hi_void)clt_sta_init();

    ret = tc_comm_init();

    return ret;
}

static hi_s32 client_is_local_command(vfmw_cid cmd_id)
{
    if (cmd_id == VFMW_CID_GET_CHAN_STATE
        || cmd_id == VFMW_CID_SET_STREAM_INTF) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 client_local_control(hi_s32 chan_id, vfmw_cid cmd_id, hi_void *args, hi_u32 param_length)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd_id) {
        case VFMW_CID_GET_CHAN_STATE:
            ret = clt_sta_get_chan_state(chan_id, (vfmw_chan_info *)args);
            break;

        case VFMW_CID_SET_STREAM_INTF:
            ret = clt_stm_set_intf(chan_id, (vfmw_stream_intf *)args);
            break;

        default:
            break;
    }

    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "LocalControl:%d exec Error!\n", cmd_id);
    }

    return ret;
}

static hi_void client_part_chan_share_mem(vfmw_shr_chn_mem *chan_share_mem, hi_u32 image_size, hi_u32 stream_size,
                                          hi_u32 event_size)
{
    chan_share_mem->img_phy_addr = chan_share_mem->chan_shr_mem_desc.phy_addr;
    chan_share_mem->img_vir_addr = chan_share_mem->chan_shr_mem_desc.vir_addr;
    chan_share_mem->stm_phy_addr = chan_share_mem->chan_shr_mem_desc.phy_addr + image_size;
    chan_share_mem->stm_vir_addr = chan_share_mem->chan_shr_mem_desc.vir_addr + image_size;
    chan_share_mem->evt_phy_addr = chan_share_mem->chan_shr_mem_desc.phy_addr + image_size + stream_size;
    chan_share_mem->evt_vir_addr = chan_share_mem->chan_shr_mem_desc.vir_addr + image_size + stream_size;
    chan_share_mem->sta_phy_addr = chan_share_mem->chan_shr_mem_desc.phy_addr + image_size + stream_size + event_size;
    chan_share_mem->sta_vir_addr = chan_share_mem->chan_shr_mem_desc.vir_addr + image_size + stream_size + event_size;
}

static hi_s32 client_alloc_chan_share_mem(vfmw_shr_chn_mem *chan_share_mem)
{
    hi_u32 image_size;
    hi_u32 stat_size;
    hi_u32 stream_size;
    hi_u32 event_size;
    hi_u32 chan_share_mem_size;
    char mem_name[50] = { 0 }; /* 50: array size */
    static hi_s32 count = 0;

    image_size = SHR_IMG_LIST_SIZE;
    stat_size = SHR_STA_MEM_SIZE;
    stream_size = SHR_STM_LIST_SIZE;
    event_size = SHR_EVT_LIST_SIZE;
    chan_share_mem_size = image_size + stat_size + stream_size + event_size;

    if (snprintf_s(mem_name, sizeof(mem_name), sizeof(mem_name), "VFMW_Chan_Shr_Buf_%d", count) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    if (OSAL_OK != OS_KMEM_ALLOC(mem_name, chan_share_mem_size, 4, 0, /* 4: align */
                                 &chan_share_mem->chan_shr_mem_desc)) {
        dprint(PRN_ERROR, "Alloc Chan Share Mem Failed!\n");

        return HI_FAILURE;
    }
    client_part_chan_share_mem(chan_share_mem, image_size, stream_size, event_size);

    count++;

    return HI_SUCCESS;
}

static hi_s32 client_prepare_remote_param(tee_entry *entry, hi_s32 chan_id, vfmw_cid cmd_id, hi_void *args)
{
    hi_s64 *array = HI_NULL;
    hi_s8 *arg_param = HI_NULL;
    hi_s32 ret = HI_FAILURE;

    if (cmd_id == VFMW_CID_CREATE_CHAN
        || cmd_id == VFMW_CID_GET_CHAN_MEMSIZE) {
        array = (hi_s64 *)args;
        arg_param = UINT64_PTR(array[1]);
        VFMW_CHECK_SEC_FUNC(memcpy_s(entry->ctx.chan_option,
                                     sizeof(vfmw_chan_option), arg_param, sizeof(vfmw_chan_option)));
    }

    if (cmd_id == VFMW_CID_CREATE_CHAN) {
        ret = client_alloc_chan_share_mem(&entry->ctx.chan_option->tmp_chan_shr_mem);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "client_alloc_chan_share_mem Failed!\n");

            return HI_FAILURE;
        }
    }

    if (cmd_id == VFMW_CID_CFG_CHAN) {
        if (args != HI_NULL) {
            ((vfmw_chan_cfg *)args)->sec_flag = 1;
        }
    }

    return HI_SUCCESS;
}

static hi_void client_bind_chan_share_mem(hi_s32 chan_id, vfmw_shr_chn_mem *chan_share_mem)
{
    (hi_void)clt_img_bind_queue(chan_id, UINT64_PTR(chan_share_mem->img_vir_addr));
    (hi_void)clt_stm_bind_queue(chan_id, UINT64_PTR(chan_share_mem->stm_vir_addr));
    (hi_void)clt_evt_bind_queue(chan_id, UINT64_PTR(chan_share_mem->evt_vir_addr));
    (hi_void)clt_sta_bind_queue(chan_id, UINT64_PTR(chan_share_mem->sta_vir_addr));

    return;
}

static hi_void client_unbind_chan_share_mem(hi_s32 chan_id)
{
    clt_img_unbind_queue(chan_id);
    clt_stm_unbind_queue(chan_id);
    clt_evt_unbind_queue(chan_id);
    clt_sta_unbind_queue(chan_id);
}

static hi_void client_free_chan_share_mem(vfmw_shr_chn_mem *chan_share_mem)
{
    if (chan_share_mem->chan_shr_mem_desc.phy_addr != 0) {
        OS_KMEM_FREE(&chan_share_mem->chan_shr_mem_desc);
        VFMW_CHECK_SEC_FUNC(memset_s(chan_share_mem, sizeof(vfmw_shr_chn_mem), 0, sizeof(vfmw_shr_chn_mem)));
    }
}

static hi_void client_process_create_result(tee_entry *entry, hi_s32 chan_id, hi_void *args, hi_s32 remote_result)
{
    hi_s64 *array = (hi_s64 *)args;
    hi_s8 *arg_param = HI_NULL;

    if (remote_result == HI_SUCCESS) {
        hi_s32 ret_chan_id = chan_id;
        VFMW_CHECK_SEC_FUNC(memset_s(&entry->ctx.chan_context[ret_chan_id],
                                     sizeof(chan_context), 0, sizeof(chan_context)));
        entry->ctx.chan_context[ret_chan_id].chan_id = ret_chan_id;
        entry->ctx.chan_context[ret_chan_id].sec_mode = HI_TRUE;

        OS_SEMA_INIT(&entry->ctx.chan_context[ret_chan_id].chan_sema);
        OS_SPIN_LOCK_INIT(&entry->ctx.chan_context[ret_chan_id].chan_lock);
        entry->ctx.inst_num++;

        VFMW_CHECK_SEC_FUNC(memcpy_s(&entry->ctx.chan_context[ret_chan_id].chan_share_mem, sizeof(vfmw_shr_chn_mem),
                                     &entry->ctx.chan_option->tmp_chan_shr_mem, sizeof(vfmw_shr_chn_mem)));
        VFMW_CHECK_SEC_FUNC(memset_s(&entry->ctx.chan_option->tmp_chan_shr_mem,
                                     sizeof(vfmw_mem_desc), 0, sizeof(vfmw_mem_desc)));

        client_bind_chan_share_mem(ret_chan_id, &entry->ctx.chan_context[ret_chan_id].chan_share_mem);

        entry->ctx.chan_context[ret_chan_id].valid = HI_TRUE;
    } else {
        if (entry->ctx.chan_option->tmp_chan_shr_mem.chan_shr_mem_desc.phy_addr != 0) {
            OS_KMEM_FREE(&entry->ctx.chan_option->tmp_chan_shr_mem.chan_shr_mem_desc);
            VFMW_CHECK_SEC_FUNC(memset_s(&entry->ctx.chan_option->tmp_chan_shr_mem,
                                         sizeof(vfmw_shr_chn_mem), 0, sizeof(vfmw_shr_chn_mem)));
        }
    }

    arg_param = UINT64_PTR(array[1]);
    if (arg_param != HI_NULL) {
        VFMW_CHECK_SEC_FUNC(memcpy_s(arg_param, sizeof(vfmw_chan_option),
                                     entry->ctx.chan_option, sizeof(vfmw_chan_option)));
    }
}

static hi_void client_process_get_mem_size_result(tee_entry *entry, hi_void *args)
{
    hi_s64 *array = (hi_s64 *)args;
    hi_s8 *arg_param = HI_NULL;

    arg_param = UINT64_PTR(array[1]);
    if (arg_param != HI_NULL) {
        VFMW_CHECK_SEC_FUNC(memcpy_s(arg_param, sizeof(vfmw_chan_option),
                                     entry->ctx.chan_option, sizeof(vfmw_chan_option)));
    }
}

static hi_void client_process_start_result(tee_entry *entry, hi_s32 chan_id)
{
    entry->ctx.chan_context[chan_id].chan_state = CHAN_START;
    entry->ctx.chan_context[chan_id].output_img_en = 1;
    entry->ctx.thread_state = THREAD_RUN;
}

static hi_void client_process_stop_result(tee_entry *entry, hi_s32 chan_id)
{
    entry->ctx.chan_context[chan_id].chan_state = CHAN_STOP;
}

static hi_void client_process_destroy_result(tee_entry *entry, hi_s32 chan_id)
{
    OS_SEMA_DOWN(entry->ctx.chan_context[chan_id].chan_sema);

    (hi_void)clt_evt_reset_queue(chan_id);

    (hi_void)clt_stm_reset_queue(chan_id);

    client_unbind_chan_share_mem(chan_id);

    client_free_chan_share_mem(&entry->ctx.chan_context[chan_id].chan_share_mem);

    entry->ctx.inst_num--;
    if (entry->ctx.inst_num <= 0) {
        entry->ctx.inst_num = 0;
        entry->ctx.thread_state = THREAD_SLEEP;
    }

    if (entry->ctx.chan_context[chan_id].raw_file != HI_NULL) {
        OS_FCLOSE(entry->ctx.chan_context[chan_id].raw_file);
        entry->ctx.chan_context[chan_id].raw_file = HI_NULL;
    }

    entry->ctx.chan_context[chan_id].chan_state = CHAN_INVALID;
    entry->ctx.chan_context[chan_id].valid = HI_FALSE;

    OS_SEMA_UP(entry->ctx.chan_context[chan_id].chan_sema);

    OS_SEMA_EXIT(entry->ctx.chan_context[chan_id].chan_sema);
    OS_SPIN_LOCK_EXIT(entry->ctx.chan_context[chan_id].chan_lock);
}

static hi_void client_process_reset_result(tee_entry *entry, hi_s32 chan_id, hi_s32 with_option, hi_void *args)
{
    vfmw_chan_reset_option *reset_option = (vfmw_chan_reset_option *)args;
    if (with_option == 0 || reset_option->keep_bs == 0) {
        OS_SEMA_DOWN(entry->ctx.chan_context[chan_id].chan_sema);

        (hi_void)clt_evt_reset_queue(chan_id);
        (hi_void)clt_stm_reset_queue(chan_id);

        OS_SEMA_UP(entry->ctx.chan_context[chan_id].chan_sema);
    }
}

static hi_void reset_process_config_result(hi_s32 chan_id)
{
    clt_evt_chn_cfg cfg;

    cfg.sec_flag = 1;
    (hi_void)clt_evt_config_chan(chan_id, &cfg);

    return;
}

static hi_void reset_process_remote_result(tee_entry *entry, hi_s32 chan_id, vfmw_cid cmd_id, hi_void *args,
                                           hi_s32 remote_result)
{
    switch (cmd_id) {
        case VFMW_CID_CREATE_CHAN:
            client_process_create_result(entry, chan_id, args, remote_result);
            break;

        case VFMW_CID_GET_CHAN_MEMSIZE:
            client_process_get_mem_size_result(entry, args);
            break;

        case VFMW_CID_START_CHAN:
            client_process_start_result(entry, chan_id);
            break;

        case VFMW_CID_STOP_CHAN:
            client_process_stop_result(entry, chan_id);
            break;

        case VFMW_CID_DESTROY_CHAN:
            client_process_destroy_result(entry, chan_id);
            break;

        case VFMW_CID_RESET_CHAN:
            client_process_reset_result(entry, chan_id, HI_FALSE, args);
            break;

        case VFMW_CID_RESET_CHAN_WITH_OPTION:
            client_process_reset_result(entry, chan_id, HI_TRUE, args);
            break;
        case VFMW_CID_CFG_CHAN:
            reset_process_config_result(chan_id);
            break;

        default:
            break;
    }

    return;
}

static hi_void client_prepare_environment(tee_entry *entry, hi_s32 chan_id, vfmw_cid cmd_id)
{
    hi_ulong flag;

    if (cmd_id == VFMW_CID_RESET_CHAN
        || cmd_id == VFMW_CID_RESET_CHAN_WITH_OPTION) {
        OS_SPIN_LOCK(entry->ctx.chan_context[chan_id].chan_lock, &flag);
        (hi_void)clt_img_reset_queue(chan_id);
        OS_SPIN_UNLOCK(entry->ctx.chan_context[chan_id].chan_lock, &flag);
    }
}

static hi_void client_clear_environment(tee_entry *entry, vfmw_cid cmd_id)
{
}

static hi_s32 client_process_remote_control(tee_entry *entry, hi_s32 chan_id, vfmw_cid cmd_id, hi_void *args,
                                            hi_s32 *ret_val, hi_u32 param_length)
{
    UADDR arg_phy_addr;
    hi_s32 ret;
    tee_vfmw_cmd_params cmd_params;

    arg_phy_addr = (args == HI_NULL) ? 0 : (__pa(args));

    VFMW_CHECK_SEC_FUNC(memset_s(&cmd_params, sizeof(tee_vfmw_cmd_params), 0, sizeof(tee_vfmw_cmd_params)));

    cmd_params.chan_id = chan_id;
    cmd_params.cmd_id = cmd_id;
    cmd_params.arg_phy_addr = arg_phy_addr;
    cmd_params.param_length = param_length;
    ret = tc_comm_send_command(VFMW_CMD_ID_CONTROL, (hi_void *)(&cmd_params), ret_val);

    return ret;
}

static hi_s32 client_remote_control(tee_entry *entry, hi_s32 chan_id, vfmw_cid cmd_id, hi_void *args,
                                    hi_u32 param_length)
{
    hi_s32 ret;
    hi_u32 ret_val = HI_FAILURE;

    static hi_u32 begin_time = 0;
    static hi_u32 start_time = 0;
    static hi_u32 end_time = 0;
    static hi_u32 total_time = 0;
    static hi_u32 count = 0;

    client_prepare_environment(entry, chan_id, cmd_id);

    ret = client_prepare_remote_param(entry, chan_id, cmd_id, args);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "PrepareParam Failed!\n");

        client_clear_environment(entry, cmd_id);

        return HI_FAILURE;
    }

    if (entry->ctx.print_time) {
        start_time = OS_GET_TIME_US();
        begin_time = (count == 0) ? start_time : begin_time;
    }

    (hi_void)client_process_remote_control(entry, chan_id, cmd_id, args, &ret_val, param_length);

    if (entry->ctx.print_time) {
        end_time = OS_GET_TIME_US();
        total_time += end_time - start_time;
        count++;

        if (end_time - begin_time >= 1000000) { /* 1000000: cost time, us */
            dprint(PRN_ALWS, "Control Total: %d, count: %d, Avg: %d us\n", total_time, count, total_time / count);
            total_time = begin_time = start_time = end_time = 0;
            count = 0;
        }
    }

    reset_process_remote_result(entry, chan_id, cmd_id, args, ret_val);

    client_clear_environment(entry, cmd_id);

    return ret_val;
}
/*
static hi_void Client_PrintAllQueue(hi_s32 chan_id)
{
    clt_img_print(chan_id);
    clt_stm_print(chan_id);
} */
/* ----------------------------EXTERNAL FUNCTION---------------------------- */
hi_s32 tee_client_init(hi_void *args, hi_u32 len)
{
    hi_s32 ret;
    hi_u32 ret_val = HI_FAILURE;
    vfmw_init_param init_args;
    vfmw_init_param *ops = (vfmw_init_param *)args;
    tee_entry *entry = client_get_entry();

    dprint(PRN_DBG, "Tee VFMW Init Start.\n");

    client_alloc_tee_sema(entry);

    if (ops == HI_NULL || ops->fn_callback == HI_NULL) {
        dprint(PRN_ERROR, "%s: Param Invalid!\n", __func__);
        client_release_tee_sema(entry);

        return HI_FAILURE;
    }

    if (entry->ctx.env_set_up) {
        dprint(PRN_ERROR, "Tee VFMW already init(%d), return OK.\n", entry->ctx.init_cnt);
        entry->ctx.init_cnt++;
        ret = clt_evt_set_call_back(ops->adpt_type, ops->fn_callback);
        client_release_tee_sema(entry);

        return ret;
    }

    ret = client_init_modules();
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "client_init_modules Error!\n");
        client_release_tee_sema(entry);

        return HI_FAILURE;
    }

    client_init_entry(entry);

    ret = client_alloc_glb_share_mem(entry);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "GlobalShareMemory Alloc Failed!\n");
        goto InitFree0;
    }

    client_prepare_init_args(entry, &init_args, ops);

    ret = tc_comm_send_command(VFMW_CMD_ID_INIT, &init_args, &ret_val);
    if (ret != HI_SUCCESS || ret_val != HI_SUCCESS) {
        dprint(PRN_ERROR, "Call VFMW_CMD_ID_INIT Error!\n");
        goto InitFree1;
    }

    ret = clt_evt_set_call_back(ops->adpt_type, ops->fn_callback);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "Set CallBack Failed!\n");
    }

    ret = client_init_thread(entry);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "ThreadInit Error!\n");
        goto InitFree2;
    }

    ret = client_request_irq(entry);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "Request irq Failed!\n");
        goto InitFree3;
    }
    entry->ctx.init_cnt = 1;
    entry->ctx.env_set_up = HI_TRUE;

    dprint(PRN_DBG, "TEE VFMW Init Success.\n");
    client_release_tee_sema(entry);

    return HI_SUCCESS;

InitFree3:
    client_destroy_vfmw_thread(entry);
InitFree2:
    tc_comm_send_command(VFMW_CMD_ID_EXIT, HI_NULL, &ret_val);
InitFree1:
    client_free_glb_share_mem(entry);
InitFree0:
    tc_comm_deinit();
    client_release_tee_sema(entry);

    return HI_FAILURE;
}

hi_s32 tee_client_get_image(hi_s32 chan_id, vfmw_image *image)
{
    hi_s32 ret;
    hi_ulong flag;
    tee_entry *entry = client_get_entry();

    VFMW_ASSERT_RET((chan_id >= 0 && chan_id < VFMW_CHAN_NUM), HI_FAILURE);
    VFMW_ASSERT_RET((image != HI_NULL), HI_FAILURE);

    OS_SPIN_LOCK(entry->ctx.chan_context[chan_id].chan_lock, &flag);
    ret = clt_img_get(chan_id, image);
    OS_SPIN_UNLOCK(entry->ctx.chan_context[chan_id].chan_lock, &flag);

    return ret;
}

hi_s32 tee_client_release_image(hi_s32 chan_id, const vfmw_image *image)
{
    hi_s32 ret;

    VFMW_ASSERT_RET((chan_id >= 0 && chan_id < VFMW_CHAN_NUM), HI_FAILURE);
    VFMW_ASSERT_RET((image != HI_NULL), HI_FAILURE);

    ret = clt_img_release(chan_id, image->image_id);

    return ret;
}

hi_s32 tee_client_suspend(hi_void *args)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 ret_val = HI_FAILURE;
    tee_entry *entry = client_get_entry();

    client_alloc_tee_sema(entry);

    if (entry->ctx.env_set_up) {
        ret = tc_comm_send_command(VFMW_CMD_ID_SUSPEND, HI_NULL, &ret_val);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "InvokeCommand VFMW_CMD_ID_VDEC_SUSPEND Failed!\n");
        }

        entry->ctx.thread_state = THREAD_SLEEP;
    }

    client_release_tee_sema(entry);

    return ret_val;
}

hi_s32 tee_client_resume(hi_void *args)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 ret_val = HI_FAILURE;
    tee_entry *entry = client_get_entry();

    client_alloc_tee_sema(entry);

    if (entry->ctx.env_set_up) {
        ret = tc_comm_send_command(VFMW_CMD_ID_RESUME, HI_NULL, &ret_val);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "InvokeCommand VFMW_CMD_ID_VDEC_SUSPEND Failed!\n");
        }

        entry->ctx.thread_state = THREAD_RUN;
    }

    client_release_tee_sema(entry);

    return ret_val;
}

hi_s32 tee_client_control(hi_s32 chan_id, vfmw_cid cmd_id, hi_void *args, hi_u32 param_length)
{
    hi_s32 ret;
    tee_entry *entry = client_get_entry();

    VFMW_ASSERT_RET((chan_id >= 0 && chan_id < VFMW_CHAN_NUM), HI_FAILURE);

    client_alloc_tee_sema(entry);

    if (entry->ctx.env_set_up == HI_FALSE) {
        dprint(PRN_ERROR, "%s: secure decoder not inited yet!\n", __func__);
        client_release_tee_sema(entry);

        return HI_FAILURE;
    }

    if (cmd_id != VFMW_CID_CREATE_CHAN && cmd_id != VFMW_CID_ALLOC_SEC_BUF
        && cmd_id != VFMW_CID_FREE_SEC_BUF && entry->ctx.chan_context[chan_id].valid != HI_TRUE) {
        client_release_tee_sema(entry);

        return HI_FAILURE;
    }

    if (client_is_local_command(cmd_id)) {
        ret = client_local_control(chan_id, cmd_id, args, param_length);
    } else {
        ret = client_remote_control(entry, chan_id, cmd_id, args, param_length);
    }
    /*
    if (cmd_id == VFMW_CID_RESET_CHAN_WITH_OPTION
        || cmd_id == VFMW_CID_RESET_CHAN)
    {
        Client_PrintAllQueue(chan_id);
    } */
    client_release_tee_sema(entry);

    return ret;
}

hi_s32 tee_client_exit(hi_void *args)
{
    hi_s32 ret;
    hi_u32 ret_val = HI_FAILURE;
    tee_entry *entry = client_get_entry();

    client_alloc_tee_sema(entry);

    if (entry->ctx.env_set_up == HI_FALSE) {
        dprint(PRN_ERROR, "TEE VFMW not init, return OK.\n");
        client_release_tee_sema(entry);

        return HI_SUCCESS;
    }

    if (entry->ctx.inst_num > 0) {
        dprint(PRN_ERROR, "TEE inst_num(%d) > 0, return OK.\n", entry->ctx.inst_num);
        client_release_tee_sema(entry);

        return HI_SUCCESS;
    }

    client_destroy_vfmw_thread(entry);

    ret = tc_comm_send_command(VFMW_CMD_ID_EXIT, HI_NULL, &ret_val);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "Call VFMW_CMD_ID_EXIT Failed!\n");
    }

    client_free_irq(entry);

    tc_comm_deinit();

    client_free_glb_share_mem(entry);

    entry->ctx.init_cnt = 0;
    entry->ctx.env_set_up = HI_FALSE;

    client_release_tee_sema(entry);

    return ret_val;
}

hi_void tee_client_open(hi_void)
{
    tee_entry *entry = client_get_entry();
    client_init_tee_sema(entry);
}

hi_void tee_client_close(hi_void)
{
    /* todo proc */
    tee_entry *entry = client_get_entry();
    client_exit_tee_sema(entry);
}
