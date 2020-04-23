/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description:  encoder.
 * Author: sdk
 * Create: 2019-07-18
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/delay.h>

/* add include here */
#include <linux/version.h>

#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/vmalloc.h>

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>

#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/seq_file.h>
#include <linux/list.h>

#include <asm/uaccess.h>

#include <asm/io.h>

#include <linux/ioport.h>
#include <linux/string.h>

#include "drv_venc_queue_mng.h"
#include "hi_venc_type.h"
#include "hi_errno.h"
#include "hi_drv_mem.h"
#include "linux/hisilicon/securec.h"

#define MAX_QUEUE_DEPTH    200

static void venc_drv_mng_queue_stop(queue_info *queue)
{
    unsigned long flags;
    struct list_head *pos = NULL;
    struct list_head *n = NULL;

    HI_INFO_VENC("msg prepare to stop!\n");

    /* flush msg list */
    venc_drv_osal_lock(queue->msg_lock, &flags);
    queue->stop = 1;
    if (!list_empty(&queue->head)) {
        HI_INFO_VENC("msg queue not empty, flush it!\n");
        /*== for (pos = (head)->next,n = pos->next; pos != (head);pos = n, n = pos->next) */
        list_for_each_safe(pos, n, &queue->head) {
            list_del(pos);
            list_add_tail(pos, &queue->free);
        }
    }
    venc_drv_osal_unlock(queue->msg_lock, &flags);
    if (queue->block_wait) {
        osal_wait_wakeup(&queue->wait);
    }
    HI_INFO_VENC("Queue Stop ok!\n");
    return;
}

queue_info* venc_drv_mng_queue_init(hi_u32 depth, hi_bool omx, hi_bool block)
{
    queue_info  *queue = NULL;
    queue_data  *data = NULL;
    hi_u32 num;

    if ((depth > MAX_QUEUE_DEPTH) || (depth == 0)) {
        HI_ERR_VENC("Invalid Param!\n");
        return NULL;
    }
    num = depth;
    queue = (queue_info*)HI_VMALLOC(HI_ID_VENC, sizeof(queue_info));
    if (queue == NULL) {
        HI_ERR_VENC("Get MEM Failed\n");
        return NULL;
    }
    VENC_CHECK_NEQ_RET(memset_s(queue, sizeof(queue_info), 0, sizeof(queue_info)), HI_SUCCESS, NULL);

    venc_drv_osal_lock_create(&queue->msg_lock);
    INIT_LIST_HEAD(&queue->head);
    INIT_LIST_HEAD(&queue->free);

    if (block) {
        osal_wait_init(&queue->wait);    /* just for omxvenc */
    }

    queue->stop = 0;

    /* alloc element for seg stream */
    data  = (queue_data *)HI_VMALLOC(HI_ID_VENC, sizeof(queue_data) * num);
    if (data == NULL) {
        HI_ERR_VENC("Get MEM Failed\n");
        HI_VFREE(HI_ID_VENC, queue);
        return NULL;
    }
    VENC_CHECK_NEQ_RET(memset_s(data, sizeof(queue_data) * num, 0, sizeof(queue_data) * num), HI_SUCCESS, NULL);

    queue->alloc_data = data;
    for (; num; data++, num--) {
        list_add(&data->list, &queue->free);
    }

    queue->queue_num = depth;
    queue->omx      = omx;
    queue->block_wait = block;
    queue->sleep    = 0;
    queue->stm_lock = 0;

    return queue;
}

hi_s32 venc_drv_mng_queue_deinit(queue_info *queue)
{
    hi_u32 time_cnt = 0;
    if (queue == NULL) {
        HI_ERR_VENC("Invalid Param!\n");
        return HI_ERR_VENC_NULL_PTR;
    }

    while ((queue->sleep) && (time_cnt < 1000)) { /* 1000:time limit */
        time_cnt++;
        osal_msleep_uninterruptible(10); /* sleep 10s */
    }

    if (time_cnt == 1000) { /* 1000:time limit */
        HI_ERR_VENC("wait MngQueueDeinit time out!!Force to stop the channel\n");
    }

    venc_drv_mng_queue_stop(queue);

    HI_VFREE(HI_ID_VENC, queue->alloc_data);
    HI_VFREE(HI_ID_VENC, queue);

    return 0;
}

hi_s32 venc_drv_mng_queue_empty(queue_info *queue)
{
    if (queue == NULL) {
        return 1;
    }

    return list_empty(&queue->head);
}

hi_s32 venc_drv_mng_queue_full(queue_info *queue)
{
    if (queue == NULL) {
        return 1;
    }

    return list_empty(&queue->free);
}

static hi_s32 venc_drv_mng_queue_pending(const hi_void *param)
{
    hi_u32 has_msg;
    hi_u32 stopped;
    unsigned long flags;
    const queue_info *queue = (const queue_info *)param;

    if (queue == NULL) {
        return 1;
    }

    venc_drv_osal_lock(queue->msg_lock, &flags);
    has_msg = !list_empty(&queue->head);
    stopped = queue->stop;

    venc_drv_osal_unlock(queue->msg_lock, &flags);

    return has_msg || stopped;
}

static hi_s32 check_queue(queue_info *queue)
{
    if (queue->stop) {
        HI_INFO_VENC("msg queue stoped\n");
        return HI_FAILURE;
    }

    if (list_empty(&queue->head)) {
        HI_INFO_VENC("no free msg dequeue!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 dqueue_frame(queue_info *queue,  hi_void *data, hi_u32 len, queue_data *msg)
{
    if (queue->omx) {
        if (len < sizeof(hi_venc_msg_info)) {
            HI_ERR_VENC("len out of range:%u\n", len);
            return HI_FAILURE;
        }
        VENC_CHECK_NEQ_RET(memcpy_s(data, len, &msg->frm_info.msg_info_omx, sizeof(hi_venc_msg_info)),
            HI_SUCCESS, HI_FAILURE);
    } else {
        if (len < sizeof(hi_drv_video_frame)) {
            HI_ERR_VENC("len out of range:%u\n", len);
            return HI_FAILURE;
        }
        VENC_CHECK_NEQ_RET(memcpy_s(data, len, &msg->frm_info.queue_info, sizeof(hi_drv_video_frame)),
            HI_SUCCESS, HI_FAILURE);
    }

    return HI_SUCCESS;
}
hi_s32 venc_drv_mng_dequeue(queue_info *queue,  hi_void *data, hi_u32 len)
{
    unsigned long flags;
    queue_data *msg = NULL;
    hi_s32 ret;

    if (queue == NULL || data == NULL) {
        HI_ERR_VENC("Invalid Param\n");
        return HI_ERR_VENC_NULL_PTR;
    }

    if (queue->block_wait) {
        queue->sleep = 1;
        /*  msg_pending(queue)为真则退出阻塞的等待! */
        ret = osal_wait_timeout_interruptible(&queue->wait, venc_drv_mng_queue_pending, queue,
            osal_msecs_to_jiffies(30)); /* 30 beats */
        if (ret <= 0) {  /* ret >0 is success */
            queue->sleep = 0;
            /* HI_ERR_VENC("dequeue timeout, failed, ret = %d\n", ret); */
            return HI_FAILURE;
        }
        queue->sleep = 0;

        venc_drv_osal_lock(queue->msg_lock, &flags);
        ret = check_queue(queue);
        if (ret != HI_SUCCESS) {
            venc_drv_osal_unlock(queue->msg_lock, &flags);
            return HI_FAILURE;
        }

        msg = list_first_entry(&queue->head, queue_data, list);
        list_del(&msg->list);

        VENC_CHECK_NEQ_RET(memcpy_s(data, sizeof(hi_venc_msg_info), &msg->frm_info.msg_info_omx,
            sizeof(hi_venc_msg_info)), HI_SUCCESS, HI_FAILURE);

        list_add(&msg->list, &queue->free);
        venc_drv_osal_unlock(queue->msg_lock, &flags);
    } else {
        venc_drv_osal_lock(queue->msg_lock, &flags);
        ret = check_queue(queue);
        if (ret != HI_SUCCESS) {
            venc_drv_osal_unlock(queue->msg_lock, &flags);
            return HI_FAILURE;
        }

        msg = list_first_entry(&queue->head, queue_data, list);
        list_del(&msg->list);

        ret = dqueue_frame(queue, data, len, msg);
        if (ret != HI_SUCCESS) {
            venc_drv_osal_unlock(queue->msg_lock, &flags);
            return HI_FAILURE;
        }

        list_add(&msg->list, &queue->free);
        venc_drv_osal_unlock(queue->msg_lock, &flags);
    }
    return HI_SUCCESS;
}

static hi_s32 check_status_valid(queue_info *queue, hi_bool is_omx, hi_void *data, hi_u32 msgcode, hi_u32 status)
{
    /* must make sure queue != null before call this function! */
    if (queue->stop) {
        HI_INFO_VENC("msg queue stopped!\n");
        return HI_FAILURE;
    }

    if (venc_drv_mng_queue_full(queue)) {
        HI_INFO_VENC("no free msg left!\n");
        return HI_FAILURE;
    }

    if (queue->omx != is_omx) {
        HI_ERR_VENC("queue->omx(%d) and omx(%d) not match!\n", queue->omx, is_omx);
        return HI_FAILURE;
    }

    if (is_omx) {
        if ((msgcode == HI_VENC_MSG_RESP_INPUT_DONE) || (msgcode == HI_VENC_MSG_RESP_OUTPUT_DONE)) {
            if (data == NULL) {
                HI_ERR_VENC("msgcode = %d input Null prt!\n", msgcode);
                return HI_FAILURE;
            }
        }
    } else {
        if (data == NULL) {
            HI_ERR_VENC("msgcode = %d input Null prt!\n", msgcode);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 venc_drv_mng_queue(queue_info *queue, hi_bool is_omx, hi_void *data, hi_u32 msgcode, hi_u32 status, hi_u32 len)
{
    unsigned long flags;
    queue_data *msg = NULL;

    if (queue == NULL) {
        HI_ERR_VENC("Invalid Param\n");
        return HI_ERR_VENC_NULL_PTR;
    }

    venc_drv_osal_lock(queue->msg_lock, &flags);

    if (check_status_valid(queue, is_omx, data, msgcode, status) != HI_SUCCESS) {
        goto error;
    }

    msg = list_first_entry(&queue->free, queue_data, list);

    list_del(&msg->list);

    if (queue->omx) {
        msg->frm_info.msg_info_omx.status_code = status;
        msg->frm_info.msg_info_omx.msg_code     = msgcode;
        if ((msgcode == HI_VENC_MSG_RESP_INPUT_DONE) || (msgcode == HI_VENC_MSG_RESP_OUTPUT_DONE)) {
            if (len > sizeof(hi_venc_user_buf) || data == NULL) {
                HI_ERR_VENC("illegal len:%u or NULL pointer data:%p\n", len, data);
                return HI_FAILURE;
            }
            VENC_CHECK_NEQ_RET(memcpy_s(&msg->frm_info.msg_info_omx.buf, sizeof(hi_venc_user_buf), data, len),
                HI_SUCCESS, HI_FAILURE);
        }
    } else {
        if (len > sizeof(hi_drv_video_frame) || data == NULL) {
                HI_ERR_VENC("illegal len:%u or NULL pointer data:%p\n", len, data);
                return HI_FAILURE;
        }
        VENC_CHECK_NEQ_RET(memcpy_s(&msg->frm_info.queue_info, sizeof(hi_drv_video_frame), data, len),
            HI_SUCCESS, HI_FAILURE);
    }

    msg->to_omx = is_omx;

    list_add_tail(&msg->list, &queue->head);

    venc_drv_osal_unlock(queue->msg_lock, &flags);

    if (queue->block_wait) {
        osal_wait_wakeup(&queue->wait);
    }
    return HI_SUCCESS;

error:
    venc_drv_osal_unlock(queue->msg_lock, &flags);

    return HI_FAILURE;
}

hi_void venc_drv_mng_move_queue_to_dequeue(queue_info *queue, queue_info *dequeue)
{
    hi_u32 count = 0;
    hi_drv_video_frame frame_info;

    while ((!venc_drv_mng_queue_empty(queue)) && (count < 100)) { /* 100:count limit */
        count++;
        if (venc_drv_mng_dequeue(queue, &frame_info, sizeof(hi_drv_video_frame)) != HI_SUCCESS) {
            HI_ERR_VENC("Mng Dequeue is err!\n");
        }
        if (venc_drv_mng_queue(dequeue, 0, &frame_info, 0, 0, sizeof(hi_drv_video_frame)) != HI_SUCCESS) {
            HI_ERR_VENC("Mng queue is err!\n");
        }
    }
}

