/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: vdp kernel thread
 * Author: pq
 * Create: 2016-01-1
 */

#include "linux/hisilicon/securec.h"
#include <linux/poll.h>
#include "drv_pq_vdp_thread.h"
#include "drv_pq_comm.h"
#include "drv_pq_comm.h"
#include "pq_hal_comm.h"

drv_pq_vdp_context g_drv_pq_vdp_context = {0};
#define DRV_PQ_GET_VDP_CONTEXT() (&g_drv_pq_vdp_context);
#define MAX_VDP_TIMEOUT_TIMES 4

static hi_void drv_pq_swap_vdp_input_buff(hi_void)
{
    hi_s32 i = 0;
    drv_pq_vdp_alg_info *temp_pq_alg_info;
    drv_pq_vdp_context *pst_pq_vdp_ctx = DRV_PQ_GET_VDP_CONTEXT();
    temp_pq_alg_info = &pst_pq_vdp_ctx->vdp_alg_info;
    for (i = 0; i < VDP_ALG_BUFFER_NUM; i++) {
        if (temp_pq_alg_info->input_buf == (hi_drv_pq_vdp_api_input *)
            temp_pq_alg_info->vdp_in_buff[i].mmu_buff.vir_addr) {
            if (i == VDP_ALG_BUFFER_NUM - 1) {
                temp_pq_alg_info->input_buf =
                (hi_drv_pq_vdp_api_input *)temp_pq_alg_info->vdp_in_buff[0].mmu_buff.vir_addr;
            } else {
                temp_pq_alg_info->input_buf =
                    (hi_drv_pq_vdp_api_input *)temp_pq_alg_info->vdp_in_buff[i + 1].mmu_buff.vir_addr;
            }
            break;
        }
    }
    return;
}

static hi_void drv_pq_swap_vdp_output_buff(hi_void)
{
    hi_s32 i;
    drv_pq_vdp_alg_info *temp_pq_alg_info;
    drv_pq_vdp_context *pst_pq_common_ctx = DRV_PQ_GET_VDP_CONTEXT();
    temp_pq_alg_info = &pst_pq_common_ctx->vdp_alg_info;
    for (i = 0; i < VDP_ALG_BUFFER_NUM; i++) {
        if (temp_pq_alg_info->output_buf == (hi_drv_pq_vdp_api_output *)
            temp_pq_alg_info->vdp_out_buff[i].mmu_buff.vir_addr) {
            if (i == VDP_ALG_BUFFER_NUM - 1) {
                temp_pq_alg_info->output_buf =
                    (hi_drv_pq_vdp_api_output *)temp_pq_alg_info->vdp_out_buff[0].mmu_buff.vir_addr;
            } else {
                temp_pq_alg_info->output_buf =
                    (hi_drv_pq_vdp_api_output *)temp_pq_alg_info->vdp_out_buff[i + 1].mmu_buff.vir_addr;
            }
            break;
        }
    }
    return;
}

hi_s32 drv_pq_init_vdp_context(hi_void)
{
    hi_s32 i = 0;
    errno_t err;
    drv_pq_vdp_context *pst_pq_vdp_ctx = DRV_PQ_GET_VDP_CONTEXT();

    err = memset_s(&pst_pq_vdp_ctx->vdp_alg_info, sizeof(drv_pq_vdp_alg_info),
        0, sizeof(drv_pq_vdp_alg_info));
    if (err != HI_SUCCESS) {
        return HI_FAILURE;
    }

    for (i = 0; i < VDP_ALG_BUFFER_NUM; i++) {
        pst_pq_vdp_ctx->vdp_alg_info.vdp_in_buff[i].exist = HI_FALSE;
        pst_pq_vdp_ctx->vdp_alg_info.vdp_out_buff[i].exist = HI_FALSE;
        pst_pq_vdp_ctx->vdp_alg_info.vdp_in_buff[i].fd = -1;
        pst_pq_vdp_ctx->vdp_alg_info.vdp_out_buff[i].fd = -1;
    }
    pst_pq_vdp_ctx->vdp_alg_info.handle_enable = HI_FALSE;
    pst_pq_vdp_ctx->vdp_alg_info.is_vdp_thread_exist = HI_FALSE;

    init_waitqueue_head(&pst_pq_vdp_ctx->vdp_alg_info.pq_poll_wait_out_queue);
    pst_pq_vdp_ctx->vdp_alg_info.wake_poll_out_process = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 drv_pq_init_vdp_thread(hi_void)
{
    hi_s32 ret;

    ret = drv_pq_init_vdp_context();

    return ret;
}

hi_void drv_pq_deinit_vdp_context(hi_void)
{
    drv_pq_vdp_context *pst_pq_vdp_ctx = DRV_PQ_GET_VDP_CONTEXT();

    pst_pq_vdp_ctx->vdp_alg_info.alg_lost = 0;
    pst_pq_vdp_ctx->vdp_alg_info.alg_run_count = 0;

    return;
}

hi_s32 drv_pq_get_vdp_alg_thread_status(hi_bool *pb_thread_exist)
{
    drv_pq_vdp_context *pst_pq_vdp_ctx = DRV_PQ_GET_VDP_CONTEXT();
    if (pb_thread_exist == HI_NULL) {
        return HI_FAILURE;
    }

    *pb_thread_exist = pst_pq_vdp_ctx->vdp_alg_info.is_vdp_thread_exist;

    return HI_SUCCESS;
}

hi_s32 drv_pq_set_vdp_alg_thread_status(hi_bool b_thread_exist)
{
    drv_pq_vdp_context *pst_pq_vdp_ctx = DRV_PQ_GET_VDP_CONTEXT();

    pst_pq_vdp_ctx->vdp_alg_info.is_vdp_thread_exist = b_thread_exist;

    return HI_SUCCESS;
}

static hi_void free_vdp_internal(hi_void)
{
    hi_s32 i = 0;

    drv_pq_vdp_context *pst_pq_vdp_ctx = DRV_PQ_GET_VDP_CONTEXT();

    for (i = 0; i < VDP_ALG_BUFFER_NUM; i++) {
        if (pst_pq_vdp_ctx->vdp_alg_info.vdp_in_buff[i].exist == HI_TRUE &&
            pst_pq_vdp_ctx->vdp_alg_info.vdp_in_buff[i].mmu_buff.vir_addr != HI_NULL) {
            pq_comm_mem_free(&pst_pq_vdp_ctx->vdp_alg_info.vdp_in_buff[i].mmu_buff);
            pst_pq_vdp_ctx->vdp_alg_info.vdp_in_buff[i].exist = HI_FALSE;
        }

        if (pst_pq_vdp_ctx->vdp_alg_info.vdp_out_buff[i].exist == HI_TRUE &&
            pst_pq_vdp_ctx->vdp_alg_info.vdp_out_buff[i].mmu_buff.vir_addr != HI_NULL) {
            pq_comm_mem_free(&pst_pq_vdp_ctx->vdp_alg_info.vdp_out_buff[i].mmu_buff);
            pst_pq_vdp_ctx->vdp_alg_info.vdp_out_buff[i].exist = HI_FALSE;
        }
    }

    pst_pq_vdp_ctx->vdp_alg_info.input_buf = HI_NULL;
    pst_pq_vdp_ctx->vdp_alg_info.output_buf = HI_NULL;
}

static hi_s32 alloc_vdp_in_buf(int buf_id)
{
    hi_s32 ret;
    errno_t err;
    drv_pq_buff_vdp *pst_buffer = HI_NULL;

    drv_pq_mem_attr attr = { 0 };
    drv_pq_vdp_context *pst_pq_vdp_ctx = DRV_PQ_GET_VDP_CONTEXT();

    pst_buffer = &pst_pq_vdp_ctx->vdp_alg_info.vdp_in_buff[buf_id];
    if (pst_buffer->exist != HI_TRUE) {
        if (buf_id == 0) {
            attr.name = "vdp_input_buf0";
        } else {
            attr.name = "vdp_input_buf1";
        }
        attr.size = sizeof(hi_drv_pq_vdp_api_input);
        attr.mode =  OSAL_NSSMMU_TYPE; /* just smmu support */
        attr.is_map_viraddr = HI_TRUE;
        attr.is_cache = HI_FALSE;
        ret = pq_comm_mem_alloc(&attr, &(pst_buffer->mmu_buff));
        if (ret != HI_SUCCESS) {
            return HI_FAILURE;
        }

        ret = pq_comm_mem_flush(&(pst_buffer->mmu_buff));
        if (ret != HI_SUCCESS) {
            free_vdp_internal();
            return HI_FAILURE;
        }

        pst_buffer->exist = HI_TRUE;
        err = memset_s(pst_buffer->mmu_buff.vir_addr, sizeof(hi_drv_pq_vdp_api_input), 0,
            sizeof(hi_drv_pq_vdp_api_input));
        if (err != HI_SUCCESS) {
            free_vdp_internal();
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 alloc_vdp_out_buf(hi_s32 buf_id)
{
    hi_s32 ret;
    errno_t err;
    drv_pq_buff_vdp *pst_buffer = HI_NULL;
    drv_pq_mem_attr attr = { 0 };
    drv_pq_vdp_context *pst_pq_vdp_ctx = DRV_PQ_GET_VDP_CONTEXT();

    pst_buffer = &pst_pq_vdp_ctx->vdp_alg_info.vdp_out_buff[buf_id];

    if (pst_buffer->exist != HI_TRUE) {
        if (buf_id == 0) {
            attr.name = "vdp_output_buf0";
        } else {
            attr.name = "vdp_output_buf1";
        }
        attr.size = sizeof(hi_drv_pq_vdp_api_output);
        attr.mode =  OSAL_NSSMMU_TYPE; /* just smmu support */
        attr.is_map_viraddr = HI_TRUE;
        attr.is_cache = HI_FALSE;
        ret = pq_comm_mem_alloc(&attr, &(pst_buffer->mmu_buff));
        if (ret != HI_SUCCESS) {
            return HI_FAILURE;
        }
        ret = pq_comm_mem_flush(&(pst_buffer->mmu_buff));
        if (ret != HI_SUCCESS) {
            free_vdp_internal();
            return HI_FAILURE;
        }

        pst_buffer->exist = HI_TRUE;
        err = memset_s(pst_buffer->mmu_buff.vir_addr, sizeof(hi_drv_pq_vdp_api_output), 0,
            sizeof(hi_drv_pq_vdp_api_output));
        if (err != HI_SUCCESS) {
            free_vdp_internal();
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 drv_pq_alloc_vdp_api()
{
    hi_s32 ret, i;
    drv_pq_vdp_context *pst_pq_vdp_ctx = DRV_PQ_GET_VDP_CONTEXT();

    for (i = 0; i < VDP_ALG_BUFFER_NUM; i++) {
        ret = alloc_vdp_in_buf(i);
        if (ret != HI_SUCCESS) {
            return HI_FAILURE;
        }

        ret = alloc_vdp_out_buf(i);
        if (ret != HI_SUCCESS) {
            return HI_FAILURE;
        }
    }
    pst_pq_vdp_ctx->vdp_alg_info.input_buf =
        (hi_drv_pq_vdp_api_input *)pst_pq_vdp_ctx->vdp_alg_info.vdp_in_buff[0].mmu_buff.vir_addr;
    pst_pq_vdp_ctx->vdp_alg_info.output_buf =
        (hi_drv_pq_vdp_api_output *)pst_pq_vdp_ctx->vdp_alg_info.vdp_out_buff[0].mmu_buff.vir_addr;

    pst_pq_vdp_ctx->vdp_alg_info.handle_enable = HI_TRUE;
    return HI_SUCCESS;
}

hi_void drv_pq_free_vdp_api(hi_void)
{
    free_vdp_internal();
    return;
}

hi_void drv_pq_deinit_vdp_thread(hi_void)
{
    hi_s32 ret;

    ret = drv_pq_vdp_wakeup_thread(HI_TRUE);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("drv_pq_vdp_wakeup_thread eroor!\n");
    }

    drv_pq_free_vdp_api();
    drv_pq_deinit_vdp_context();

    return;
}

void *drv_pq_vdp_request_buffer(drv_pq_vdp_buf_type type)
{
    drv_pq_vdp_context *pst_pq_vdp_ctx = DRV_PQ_GET_VDP_CONTEXT();

    if (type == VDP_BUF_IN) {
        if (pst_pq_vdp_ctx->vdp_alg_info.input_buf == HI_NULL) {
            return HI_NULL;
        }

        if (pst_pq_vdp_ctx->vdp_alg_info.input_buf->is_ready == HI_FALSE) {
            return (void *)pst_pq_vdp_ctx->vdp_alg_info.input_buf;
        } else {
            return HI_NULL;
        }
    } else if (type == VDP_BUF_OUT) {
        if (pst_pq_vdp_ctx->vdp_alg_info.output_buf == HI_NULL) {
            return HI_NULL;
        }

        if (pst_pq_vdp_ctx->vdp_alg_info.output_buf->is_ready == HI_TRUE) {
            return (void *)pst_pq_vdp_ctx->vdp_alg_info.output_buf;
        } else {
            return HI_NULL;
        }
    } else {
        return HI_NULL;
    }
}

hi_s32 drv_pq_vdp_update_buffer(drv_pq_vdp_buf_type type)
{
    drv_pq_vdp_context *pst_pq_vdp_ctx = DRV_PQ_GET_VDP_CONTEXT();

    if (type == VDP_BUF_IN) {
        if (pst_pq_vdp_ctx->vdp_alg_info.input_buf == HI_NULL) {
            return HI_FAILURE;
        }

        if (pst_pq_vdp_ctx->vdp_alg_info.input_buf->is_ready == HI_FALSE) {
            pst_pq_vdp_ctx->vdp_alg_info.input_buf->is_ready = HI_TRUE;
            drv_pq_swap_vdp_input_buff();
        }
    } else if (type == VDP_BUF_OUT) {
        if (pst_pq_vdp_ctx->vdp_alg_info.output_buf == HI_NULL) {
            return HI_FAILURE;
        }

        if (pst_pq_vdp_ctx->vdp_alg_info.output_buf->is_ready == HI_TRUE) {
            pst_pq_vdp_ctx->vdp_alg_info.output_buf->is_ready = HI_FALSE;
            drv_pq_swap_vdp_output_buff();
            pst_pq_vdp_ctx->vdp_alg_info.alg_run_count++;
        } else {
            pst_pq_vdp_ctx->vdp_alg_info.alg_lost++;
            HI_ERR_PQ("vdp alg result from api is not ready!\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 drv_pq_vdp_wakeup_thread(hi_bool released)
{
    hi_u32 times;
    drv_pq_vdp_context *pst_pq_vdp_ctx = DRV_PQ_GET_VDP_CONTEXT();
    if (released) {
        pst_pq_vdp_ctx->is_stop = HI_TRUE;
    }
    pst_pq_vdp_ctx->vdp_alg_info.wake_poll_out_process = HI_TRUE;
    wake_up(&pst_pq_vdp_ctx->vdp_alg_info.pq_poll_wait_out_queue);

    if (released) {
        times = 0;
        while (pst_pq_vdp_ctx->vdp_alg_info.is_vdp_thread_exist == HI_TRUE && times < MAX_VDP_TIMEOUT_TIMES) {
            osal_msleep_uninterruptible(1000); /* 1000 is the time to wait user space end */
            times++;
        }

        if (times == MAX_VDP_TIMEOUT_TIMES) {
            HI_ERR_PQ("vdp user thread destroy timeout when unload ko!\n");
            return HI_FAILURE;
        }

        /* reset vdp memory sign */
        pst_pq_vdp_ctx->vdp_alg_info.input_buf = HI_NULL;
        pst_pq_vdp_ctx->vdp_alg_info.output_buf = HI_NULL;
        pst_pq_vdp_ctx->vdp_alg_info.handle_enable = HI_FALSE;
    }

    return HI_SUCCESS;
}

hi_u32 drv_pq_vdp_poll(struct file *filp, struct poll_table_struct *wait)
{
    hi_u32 poll_mask = 0;
    drv_pq_vdp_context *pst_pq_vdp_ctx = DRV_PQ_GET_VDP_CONTEXT();
    if ((wait == HI_NULL) || (filp == HI_NULL)) {
        return poll_mask;
    }

    poll_wait(filp, &pst_pq_vdp_ctx->vdp_alg_info.pq_poll_wait_out_queue, wait);
    if (pst_pq_vdp_ctx->vdp_alg_info.wake_poll_out_process == HI_TRUE) {
        poll_mask |= POLLOUT | POLLWRNORM;
    }

    return poll_mask;
}

hi_s32 drv_pq_get_vdp_thread_info(hi_drv_pq_vdp_thread_info *pst_vdp_info)
{
    hi_s32 i = 0;
    drv_pq_vdp_context *pst_pq_vdp_ctx = DRV_PQ_GET_VDP_CONTEXT();
    if (pst_vdp_info == HI_NULL) {
        return HI_FAILURE;
    }

    for (i = 0; i < VDP_ALG_BUFFER_NUM; i++) {
        if (pst_pq_vdp_ctx->vdp_alg_info.vdp_in_buff[i].fd == -1 &&
            pst_pq_vdp_ctx->vdp_alg_info.vdp_in_buff[i].exist) {
            pst_pq_vdp_ctx->vdp_alg_info.vdp_in_buff[i].fd =
                hi_dma_buf_fd(pst_pq_vdp_ctx->vdp_alg_info.vdp_in_buff[i].mmu_buff.dma_buf, O_CLOEXEC);
        }

        if (pst_pq_vdp_ctx->vdp_alg_info.vdp_out_buff[i].exist &&
            pst_pq_vdp_ctx->vdp_alg_info.vdp_out_buff[i].fd == -1) {
            pst_pq_vdp_ctx->vdp_alg_info.vdp_out_buff[i].fd =
                hi_dma_buf_fd(pst_pq_vdp_ctx->vdp_alg_info.vdp_out_buff[i].mmu_buff.dma_buf, O_CLOEXEC);
        }

        pst_vdp_info->vdp_thread_status.read_addr_fd[i] =
            pst_pq_vdp_ctx->vdp_alg_info.vdp_in_buff[i].fd;
        pst_vdp_info->vdp_thread_status.write_addr_fd[i] =
            pst_pq_vdp_ctx->vdp_alg_info.vdp_out_buff[i].fd;
    }
    pst_vdp_info->is_stop = pst_pq_vdp_ctx->is_stop;
    pst_vdp_info->vdp_thread_status.is_enable = pst_pq_vdp_ctx->vdp_alg_info.handle_enable;
    if (pst_pq_vdp_ctx->vdp_alg_info.wake_poll_out_process == HI_TRUE) {
        pst_pq_vdp_ctx->vdp_alg_info.wake_poll_out_process = HI_FALSE;
    }
    return HI_SUCCESS;
}
