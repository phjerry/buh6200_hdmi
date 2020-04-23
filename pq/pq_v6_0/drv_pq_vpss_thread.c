/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: struct define
 * Author: pq
 * Create: 2016-01-1
 */

#include "linux/hisilicon/securec.h"
#include <linux/poll.h>
#include "drv_pq_vpss_thread.h"
#include "drv_pq_comm.h"
#include "pq_hal_comm.h"

drv_pq_vpss_context g_drv_pq_vpss_context = {0};
#define DRV_PQ_GET_VPSS_CONTEXT() (&g_drv_pq_vpss_context);
#define MAX_VPSS_TIMEOUT_TIMES 10

hi_s32 drv_pq_init_vpss_context(hi_void)
{
    errno_t err;
    hi_u32 i = 0;
    hi_s32 j = 0;
    drv_pq_vpss_context *pst_pq_vpss_ctx = DRV_PQ_GET_VPSS_CONTEXT();

    err = memset_s(&pst_pq_vpss_ctx->vpss_alg_info, sizeof(drv_pq_vpss_alg_info),
        0, sizeof(drv_pq_vpss_alg_info));
    if (err != HI_SUCCESS) {
        return HI_FAILURE;
    }
    for (i = 0; i < VPSS_HANDLE_NUM; i++) {
        for (j = 0; j < ALG_BUFFER_NUM; j++) {
            pst_pq_vpss_ctx->vpss_alg_info.vpss_in_buff[j][i].exist = HI_FALSE;
            pst_pq_vpss_ctx->vpss_alg_info.vpss_out_buff[j][i].exist = HI_FALSE;
            pst_pq_vpss_ctx->vpss_alg_info.vpss_in_buff[j][i].fd = -1;
            pst_pq_vpss_ctx->vpss_alg_info.vpss_out_buff[j][i].fd = -1;
        }

        pst_pq_vpss_ctx->vpss_alg_info.handle_enable[i] = HI_FALSE;
    }

    pst_pq_vpss_ctx->vpss_alg_info.is_vpss_thread_exist = HI_FALSE;
    pst_pq_vpss_ctx->vpss_alg_info.alg_lost = 0;
    pst_pq_vpss_ctx->vpss_alg_info.alg_run_count = 0;

    init_waitqueue_head(&pst_pq_vpss_ctx->vpss_alg_info.pq_poll_wait_queue);
    pst_pq_vpss_ctx->vpss_alg_info.wake_poll_process = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 drv_pq_init_vpss_thread(hi_void)
{
    hi_s32 ret;

    ret = drv_pq_init_vpss_context();

    return ret;
}

hi_void drv_pq_deinit_vpss_context(hi_void)
{
    drv_pq_vpss_context *pst_pq_vpss_ctx = DRV_PQ_GET_VPSS_CONTEXT();

    pst_pq_vpss_ctx->vpss_alg_info.alg_lost = 0;
    pst_pq_vpss_ctx->vpss_alg_info.alg_run_count = 0;

    return;
}

hi_void drv_pq_deinit_vpss_thread(hi_void)
{
    hi_s32 ret;

    ret = drv_pq_wakeup_thread(HI_TRUE);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("drv_pq_wakeup_thread eroor!\n");
    }

    drv_pq_free_vpss_api();
    drv_pq_deinit_vpss_context();

    return;
}

hi_void drv_pq_swap_vpss_input_buff(hi_u32 handle)
{
    hi_s32 i = 0;
    drv_pq_vpss_alg_info *temp_pq_alg_info;
    drv_pq_vpss_context *pst_pq_vpss_ctx = DRV_PQ_GET_VPSS_CONTEXT();
    temp_pq_alg_info = &pst_pq_vpss_ctx->vpss_alg_info;
    for (i = 0; i < ALG_BUFFER_NUM; i++) {
        if (temp_pq_alg_info->input_buf[handle] == (hi_drv_pq_vpss_api_input *)
            temp_pq_alg_info->vpss_in_buff[i][handle].mmu_buff.vir_addr) {
            if (i == ALG_BUFFER_NUM - 1) {
                temp_pq_alg_info->input_buf[handle] =
                (hi_drv_pq_vpss_api_input *)temp_pq_alg_info->vpss_in_buff[0][handle].mmu_buff.vir_addr;
            } else {
                temp_pq_alg_info->input_buf[handle] =
                        (hi_drv_pq_vpss_api_input *)temp_pq_alg_info->vpss_in_buff[i + 1][handle].mmu_buff.vir_addr;
            }
            break;
        }
    }
    return;
}
hi_void drv_pq_swap_vpss_output_buff(hi_u32 handle)
{
    hi_s32 i;
    drv_pq_vpss_alg_info *temp_pq_alg_info;
    drv_pq_vpss_context *pst_pq_common_ctx = DRV_PQ_GET_VPSS_CONTEXT();
    temp_pq_alg_info = &pst_pq_common_ctx->vpss_alg_info;
    for (i = 0; i < ALG_BUFFER_NUM; i++) {
        if (temp_pq_alg_info->output_buf[handle] == (hi_drv_pq_vpss_api_output *)
            temp_pq_alg_info->vpss_out_buff[i][handle].mmu_buff.vir_addr) {
            if (i == ALG_BUFFER_NUM - 1) {
                temp_pq_alg_info->output_buf[handle] =
                    (hi_drv_pq_vpss_api_output *)temp_pq_alg_info->vpss_out_buff[0][handle].mmu_buff.vir_addr;
            } else {
                temp_pq_alg_info->output_buf[handle] =
                    (hi_drv_pq_vpss_api_output *)temp_pq_alg_info->vpss_out_buff[i + 1][handle].mmu_buff.vir_addr;
            }
            break;
        }
    }
    return;
}

hi_s32 drv_pq_get_alg_thread_status(hi_bool *pb_thread_exist)
{
    drv_pq_vpss_context *pst_pq_vpss_ctx = DRV_PQ_GET_VPSS_CONTEXT();
    if (pb_thread_exist == HI_NULL) {
        return HI_FAILURE;
    }

    *pb_thread_exist = pst_pq_vpss_ctx->vpss_alg_info.is_vpss_thread_exist;

    return HI_SUCCESS;
}

hi_s32 drv_pq_set_alg_thread_status(hi_bool b_thread_exist)
{
    drv_pq_vpss_context *pst_pq_vpss_ctx = DRV_PQ_GET_VPSS_CONTEXT();

    pst_pq_vpss_ctx->vpss_alg_info.is_vpss_thread_exist = b_thread_exist;

    return HI_SUCCESS;
}

static hi_void vpss_free_internal(hi_void)
{
    hi_u32 handle_no;
    hi_s32 i = 0;

    drv_pq_vpss_context *pst_pq_vpss_ctx = DRV_PQ_GET_VPSS_CONTEXT();

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        for (i = 0; i < ALG_BUFFER_NUM; i++) {
            if (pst_pq_vpss_ctx->vpss_alg_info.vpss_in_buff[i][handle_no].exist == HI_TRUE &&
                pst_pq_vpss_ctx->vpss_alg_info.vpss_in_buff[i][handle_no].mmu_buff.phy_addr != 0) {
                pq_comm_mem_free(&pst_pq_vpss_ctx->vpss_alg_info.vpss_in_buff[i][handle_no].mmu_buff);
                pst_pq_vpss_ctx->vpss_alg_info.vpss_in_buff[i][handle_no].exist = HI_FALSE;
            }

            if (pst_pq_vpss_ctx->vpss_alg_info.vpss_out_buff[i][handle_no].exist == HI_TRUE &&
                pst_pq_vpss_ctx->vpss_alg_info.vpss_out_buff[i][handle_no].mmu_buff.phy_addr != 0) {
                pq_comm_mem_free(&pst_pq_vpss_ctx->vpss_alg_info.vpss_out_buff[i][handle_no].mmu_buff);
                pst_pq_vpss_ctx->vpss_alg_info.vpss_out_buff[i][handle_no].exist = HI_FALSE;
            }
        }

        pst_pq_vpss_ctx->vpss_alg_info.input_buf[handle_no] = HI_NULL;
        pst_pq_vpss_ctx->vpss_alg_info.output_buf[handle_no] = HI_NULL;
    }
}

static hi_s32 alloc_vpss_out_buf(hi_s32 buf_id, hi_u32 handle_no)
{
    hi_s32 ret;
    errno_t err;
    drv_pq_buff *pst_buffer = HI_NULL;
    drv_pq_mem_attr attr = { 0 };

    drv_pq_vpss_context *pst_pq_vpss_ctx = DRV_PQ_GET_VPSS_CONTEXT();

    pst_buffer = &pst_pq_vpss_ctx->vpss_alg_info.vpss_out_buff[buf_id][handle_no];
    if (pst_buffer->exist != HI_TRUE) {
        if (buf_id == 0) {
            attr.name = "vpss_output_buf0";
        } else {
            attr.name = "vpss_output_buf1";
        }
        attr.size = sizeof(hi_drv_pq_vpss_api_output);
        attr.mode =  OSAL_NSSMMU_TYPE; /* just smmu support */
        attr.is_map_viraddr = HI_TRUE;
        attr.is_cache = HI_FALSE;
        ret = pq_comm_mem_alloc(&attr, &(pst_buffer->mmu_buff));
        if (ret != HI_SUCCESS) {
            return HI_FAILURE;
        }
        ret = pq_comm_mem_flush(&(pst_buffer->mmu_buff));
        if (ret != HI_SUCCESS) {
            vpss_free_internal();
            return HI_FAILURE;
        }

        pst_buffer->exist = HI_TRUE;
        err = memset_s(pst_buffer->mmu_buff.vir_addr, sizeof(hi_drv_pq_vpss_api_output), 0,
            sizeof(hi_drv_pq_vpss_api_output));
        if (err != HI_SUCCESS) {
            vpss_free_internal();
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 alloc_vpss_in_buf(int buf_id, hi_u32 handle_no)
{
    hi_s32 ret;
    errno_t err;
    drv_pq_buff *pst_buffer = HI_NULL;
    drv_pq_mem_attr attr = { 0 };

    drv_pq_vpss_context *pst_pq_vpss_ctx = DRV_PQ_GET_VPSS_CONTEXT();

    pst_buffer = &pst_pq_vpss_ctx->vpss_alg_info.vpss_in_buff[buf_id][handle_no];
    if (pst_buffer->exist != HI_TRUE) {
        if (buf_id == 0) {
            attr.name = "vpss_input_buf0";
        } else {
            attr.name = "vpss_input_buf1";
        }
        attr.size = sizeof(hi_drv_pq_vpss_api_input);
        attr.mode =  OSAL_NSSMMU_TYPE; /* just smmu support */
        attr.is_map_viraddr = HI_TRUE;
        attr.is_cache = HI_FALSE;
        ret = pq_comm_mem_alloc(&attr, &(pst_buffer->mmu_buff));
        if (ret != HI_SUCCESS) {
            return HI_FAILURE;
        }
        ret = pq_comm_mem_flush(&(pst_buffer->mmu_buff));
        if (ret != HI_SUCCESS) {
            vpss_free_internal();
            return HI_FAILURE;
        }

        pst_buffer->exist = HI_TRUE;
        err = memset_s(pst_buffer->mmu_buff.vir_addr, sizeof(hi_drv_pq_vpss_api_input), 0,
            sizeof(hi_drv_pq_vpss_api_input));
        if (err != HI_SUCCESS) {
            vpss_free_internal();
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

hi_s32 drv_pq_alloc_vpss_api(hi_u32 handle_no)
{
    hi_s32 i, ret;
    hi_drv_pq_vpss_api_input *vpss_input_buf = HI_NULL;
    hi_drv_pq_vpss_api_output *vpss_output_buf = HI_NULL;
    drv_pq_vpss_context *pst_pq_vpss_ctx = DRV_PQ_GET_VPSS_CONTEXT();

    if (handle_no >= VPSS_HANDLE_NUM) {
        return HI_FAILURE;
    }

    for (i = 0; i < ALG_BUFFER_NUM; i++) {
        ret = alloc_vpss_in_buf(i, handle_no);
        if (ret != HI_SUCCESS) {
            return HI_FAILURE;
        }

        ret = alloc_vpss_out_buf(i, handle_no);
        if (ret != HI_SUCCESS) {
            return HI_FAILURE;
        }
    }

    for (i = 0; i < ALG_BUFFER_NUM; i++) {
        vpss_input_buf = (hi_drv_pq_vpss_api_input *)pst_pq_vpss_ctx->vpss_alg_info.vpss_in_buff[i][handle_no].mmu_buff.vir_addr;
        PQ_CHECK_NULL_PTR_RE_FAIL(vpss_input_buf);
        vpss_input_buf->is_ready = HI_FALSE;
        vpss_input_buf->handle = handle_no;
        vpss_input_buf->index = 0;
        vpss_output_buf = (hi_drv_pq_vpss_api_output *)pst_pq_vpss_ctx->vpss_alg_info.vpss_out_buff[i][handle_no].mmu_buff.vir_addr;
        PQ_CHECK_NULL_PTR_RE_FAIL(vpss_output_buf);
        vpss_output_buf->is_ready = HI_FALSE;
        vpss_output_buf->handle = handle_no;
        vpss_output_buf->index = 0;
    }

    pst_pq_vpss_ctx->vpss_alg_info.input_buf[handle_no] =
        (hi_drv_pq_vpss_api_input *)pst_pq_vpss_ctx->vpss_alg_info.vpss_in_buff[0][handle_no].mmu_buff.vir_addr;
    pst_pq_vpss_ctx->vpss_alg_info.output_buf[handle_no] =
        (hi_drv_pq_vpss_api_output *)pst_pq_vpss_ctx->vpss_alg_info.vpss_out_buff[0][handle_no].mmu_buff.vir_addr;

    pst_pq_vpss_ctx->vpss_alg_info.handle_enable[handle_no] = HI_TRUE;
    return HI_SUCCESS;
}

hi_void drv_pq_free_vpss_api(hi_void)
{
    vpss_free_internal();
    return;
}

void *drv_pq_vpss_request_buffer(drv_pq_vpss_buf_type type, hi_u32 handle)
{
    void *buf_pointer = HI_NULL;
    drv_pq_vpss_context *pst_pq_vpss_ctx = DRV_PQ_GET_VPSS_CONTEXT();

    if (handle >= VPSS_HANDLE_NUM) {
        return HI_NULL;
    }

    if (type == VPSS_BUF_IN) {
        if (pst_pq_vpss_ctx->vpss_alg_info.input_buf[handle] == HI_NULL) {
            return HI_NULL;
        }

        buf_pointer = (void *)pst_pq_vpss_ctx->vpss_alg_info.input_buf[handle];
        pst_pq_vpss_ctx->vpss_alg_info.input_buf[handle]->handle = handle;
    } else if (type == VPSS_BUF_OUT) {
        if (pst_pq_vpss_ctx->vpss_alg_info.output_buf[handle] == HI_NULL) {
            return HI_NULL;
        }

        pst_pq_vpss_ctx->vpss_alg_info.output_buf[handle]->handle = handle;
        buf_pointer = (void *)pst_pq_vpss_ctx->vpss_alg_info.output_buf[handle];
    }

    return buf_pointer;
}

hi_s32 drv_pq_vpss_update_buffer(drv_pq_vpss_buf_type type, hi_u32 handle)
{
    drv_pq_vpss_context *pst_pq_vpss_ctx = DRV_PQ_GET_VPSS_CONTEXT();

    if (handle >= VPSS_HANDLE_NUM) {
        return HI_FAILURE;
    }

    if (type == VPSS_BUF_IN) {
        if (pst_pq_vpss_ctx->vpss_alg_info.input_buf[handle]->is_ready == HI_FALSE) {
            pst_pq_vpss_ctx->vpss_alg_info.input_buf[handle]->is_ready = HI_TRUE;
            drv_pq_swap_vpss_input_buff(handle);
        }
    } else if (type == VPSS_BUF_OUT) {
        if (pst_pq_vpss_ctx->vpss_alg_info.output_buf[handle]->is_ready == HI_TRUE) {
            pst_pq_vpss_ctx->vpss_alg_info.output_buf[handle]->is_ready = HI_FALSE;
            drv_pq_swap_vpss_output_buff(handle);
            pst_pq_vpss_ctx->vpss_alg_info.alg_run_count++;
        } else {
            pst_pq_vpss_ctx->vpss_alg_info.alg_lost++;
            HI_ERR_PQ("alg result from api is not ready!\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 drv_pq_wakeup_thread(hi_bool released)
{
    hi_u32 times;
    hi_u32 handle_no = 0;
    drv_pq_vpss_context *pst_pq_vpss_ctx = DRV_PQ_GET_VPSS_CONTEXT();

    if (released) {
        /* reset vpss memory sign */
        pst_pq_vpss_ctx->is_stop = HI_TRUE;
    } else {
        pst_pq_vpss_ctx->is_stop = HI_FALSE;
    }

    pst_pq_vpss_ctx->vpss_alg_info.wake_poll_process = HI_TRUE;
    wake_up(&pst_pq_vpss_ctx->vpss_alg_info.pq_poll_wait_queue);

    if (released) {
        times = 0;
        while (pst_pq_vpss_ctx->vpss_alg_info.is_vpss_thread_exist == HI_TRUE && times < MAX_VPSS_TIMEOUT_TIMES) {
            osal_msleep_uninterruptible(1000); /* 1000 is time to wait user thread end */
            times++;
        }

        if (times == MAX_VPSS_TIMEOUT_TIMES) {
            HI_ERR_PQ("vpss user thread destroy timeout when unload ko!\n");
            return HI_FAILURE;
        }

        for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
            pst_pq_vpss_ctx->vpss_alg_info.handle_enable[handle_no] = HI_FALSE;
            pst_pq_vpss_ctx->vpss_alg_info.input_buf[handle_no] = HI_NULL;
            pst_pq_vpss_ctx->vpss_alg_info.output_buf[handle_no] = HI_NULL;
        }
    }

    return HI_SUCCESS;
}

hi_u32 drv_pq_vpss_poll(struct file *filp, struct poll_table_struct *wait)
{
    hi_u32 poll_mask = 0;
    drv_pq_vpss_context *pst_pq_vpss_ctx = DRV_PQ_GET_VPSS_CONTEXT();

    if ((wait == HI_NULL) || (filp == HI_NULL)) {
        return poll_mask;
    }

    poll_wait(filp, &pst_pq_vpss_ctx->vpss_alg_info.pq_poll_wait_queue, wait);
    if (pst_pq_vpss_ctx->vpss_alg_info.wake_poll_process == HI_TRUE) {
        poll_mask |= POLLIN | POLLRDNORM;
    }

    return poll_mask;
}

hi_s32 drv_pq_get_vpss_thread_info(hi_drv_pq_vpss_thread_info *pst_vpss_info)
{
    hi_u32 handle_no = 0;
    hi_s32 i = 0;
    drv_pq_vpss_context *pst_pq_vpss_ctx = DRV_PQ_GET_VPSS_CONTEXT();

    if (pst_vpss_info == HI_NULL) {
        return HI_FAILURE;
    }

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        for (i = 0; i < ALG_BUFFER_NUM; i++) {
            if (pst_pq_vpss_ctx->vpss_alg_info.vpss_in_buff[i][handle_no].exist == HI_TRUE &&
                pst_pq_vpss_ctx->vpss_alg_info.vpss_in_buff[i][handle_no].fd == -1) {
                pst_pq_vpss_ctx->vpss_alg_info.vpss_in_buff[i][handle_no].fd =
                    hi_dma_buf_fd(pst_pq_vpss_ctx->vpss_alg_info.vpss_in_buff[i][handle_no].mmu_buff.dma_buf,
                        O_CLOEXEC);
            }

            if (pst_pq_vpss_ctx->vpss_alg_info.vpss_out_buff[i][handle_no].exist == HI_TRUE &&
                pst_pq_vpss_ctx->vpss_alg_info.vpss_out_buff[i][handle_no].fd == -1) {
                pst_pq_vpss_ctx->vpss_alg_info.vpss_out_buff[i][handle_no].fd =
                    hi_dma_buf_fd(pst_pq_vpss_ctx->vpss_alg_info.vpss_out_buff[i][handle_no].mmu_buff.dma_buf,
                        O_CLOEXEC);
            }
            pst_vpss_info->vpss_thread_status[handle_no].read_addr_fd[i] =
                pst_pq_vpss_ctx->vpss_alg_info.vpss_in_buff[i][handle_no].fd;
            pst_vpss_info->vpss_thread_status[handle_no].write_addr_fd[i] =
                pst_pq_vpss_ctx->vpss_alg_info.vpss_out_buff[i][handle_no].fd;
        }

        pst_vpss_info->vpss_thread_status[handle_no].handle = handle_no;
        pst_vpss_info->vpss_thread_status[handle_no].is_enable =
            pst_pq_vpss_ctx->vpss_alg_info.handle_enable[handle_no];
    }

    pst_vpss_info->is_stop = pst_pq_vpss_ctx->is_stop;
    if (pst_pq_vpss_ctx->vpss_alg_info.wake_poll_process == HI_TRUE) {
        pst_pq_vpss_ctx->vpss_alg_info.wake_poll_process = HI_FALSE;
    }

    return HI_SUCCESS;
}

drv_pq_vpss_alg_info g_vpss_alg = {0};
hi_s32 drv_pq_proc_get_vpss_thread_info(hi_u32 handle, drv_pq_vpss_thread_proc *thread_proc)
{
    drv_pq_vpss_context *pst_pq_vpss_ctx = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(thread_proc);
    pst_pq_vpss_ctx = DRV_PQ_GET_VPSS_CONTEXT();
    g_vpss_alg = pst_pq_vpss_ctx->vpss_alg_info;

    thread_proc->handle_enable = g_vpss_alg.handle_enable[handle];
    thread_proc->is_in_exist[0] = g_vpss_alg.vpss_in_buff[0][handle].exist;
    thread_proc->is_in_exist[1] = g_vpss_alg.vpss_in_buff[1][handle].exist;
    thread_proc->is_out_exist[0] = g_vpss_alg.vpss_out_buff[0][handle].exist;
    thread_proc->is_out_exist[1] = g_vpss_alg.vpss_out_buff[1][handle].exist;

    if (g_vpss_alg.input_buf[handle] != HI_NULL) {
        thread_proc->is_input_alloc = HI_TRUE;
        thread_proc->is_input_ready = g_vpss_alg.input_buf[handle]->is_ready;
        thread_proc->input_index = g_vpss_alg.input_buf[handle]->index;
        thread_proc->alg_in_ready[PQ_VPSS_ALG_NR] = g_vpss_alg.input_buf[handle]->nr_api_input.is_nr_ready;
        thread_proc->alg_in_ready[PQ_VPSS_ALG_DEI] = g_vpss_alg.input_buf[handle]->dei_api_input.is_dei_ready;
        thread_proc->alg_in_ready[PQ_VPSS_ALG_DB] = g_vpss_alg.input_buf[handle]->db_api_input.is_db_ready;
        thread_proc->alg_in_ready[PQ_VPSS_ALG_DM] = g_vpss_alg.input_buf[handle]->dm_api_input.is_dm_ready;
    } else {
        thread_proc->is_input_alloc = HI_FALSE;
        thread_proc->is_input_ready = HI_FALSE;
        thread_proc->input_index = HI_FALSE;
        thread_proc->alg_in_ready[PQ_VPSS_ALG_NR] = HI_FALSE;
        thread_proc->alg_in_ready[PQ_VPSS_ALG_DEI] = HI_FALSE;
        thread_proc->alg_in_ready[PQ_VPSS_ALG_DB] = HI_FALSE;
        thread_proc->alg_in_ready[PQ_VPSS_ALG_DM] = HI_FALSE;
    }

    if (g_vpss_alg.output_buf[handle] != HI_NULL) {
        thread_proc->is_output_alloc = HI_TRUE;
        thread_proc->is_output_ready = g_vpss_alg.output_buf[handle]->is_ready;
        thread_proc->output_index = g_vpss_alg.output_buf[handle]->index;
        thread_proc->alg_out_ready[PQ_VPSS_ALG_NR] = g_vpss_alg.output_buf[handle]->nr_api_out.is_nr_output_ready;
        thread_proc->alg_out_ready[PQ_VPSS_ALG_DEI] = g_vpss_alg.output_buf[handle]->dei_api_out.is_dei_output_ready;
        thread_proc->alg_out_ready[PQ_VPSS_ALG_DB] = g_vpss_alg.output_buf[handle]->db_api_out.is_db_output_ready;
        thread_proc->alg_out_ready[PQ_VPSS_ALG_DM] = g_vpss_alg.output_buf[handle]->dm_api_out.is_dm_output_ready;
    } else {
        thread_proc->is_output_alloc = HI_FALSE;
        thread_proc->is_output_ready = HI_FALSE;
        thread_proc->output_index = HI_FALSE;
        thread_proc->alg_out_ready[PQ_VPSS_ALG_NR] = HI_FALSE;
        thread_proc->alg_out_ready[PQ_VPSS_ALG_DEI] = HI_FALSE;
        thread_proc->alg_out_ready[PQ_VPSS_ALG_DB] = HI_FALSE;
        thread_proc->alg_out_ready[PQ_VPSS_ALG_DM] = HI_FALSE;
    }

    return HI_SUCCESS;
}
