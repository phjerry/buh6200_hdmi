/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */
#ifndef __DRV_VENC_QUEUE_MNG_H__
#define __DRV_VENC_QUEUE_MNG_H__

#include <linux/list.h>
#include <linux/interrupt.h>
#include "hi_venc_type.h"
#include "hi_drv_video.h"
#include "drv_venc_osal_ext.h"

typedef struct {
    struct list_head list;  /* msg payload */
    hi_bool  to_omx;
    union {
    hi_drv_video_frame queue_info;
    hi_venc_msg_info msg_info_omx;              /* for omxvenc */
    } frm_info;
} queue_data;

typedef struct {
    osal_spinlock *msg_lock;
    osal_wait wait;

    hi_void* alloc_data;              /* 实际为  queue_data* 类型 */
    hi_u32   queue_num;
    hi_s32   stop;
    struct list_head head;
    struct list_head free;

    volatile hi_bool sleep;
    volatile hi_bool stm_lock;       /* 线程与stop处理流程互斥 sdk */

    hi_bool omx;                   /* 标志该队列是不是for omxvenc */
    hi_bool block_wait;
} queue_info;

queue_info* venc_drv_mng_queue_init(hi_u32 depth, hi_bool omx, hi_bool block);

hi_s32 venc_drv_mng_queue_deinit(queue_info* queue);

hi_s32 venc_drv_mng_dequeue(queue_info* queue, hi_void *data, hi_u32 len);

hi_s32 venc_drv_mng_queue(queue_info *queue, hi_bool to_omx, hi_void *data, hi_u32 msg_code,
    hi_u32 status, hi_u32 len);

hi_s32 venc_drv_mng_queue_empty(queue_info* queue);

hi_s32 venc_drv_mng_queue_full(queue_info* queue);

hi_void venc_drv_mng_move_queue_to_dequeue(queue_info *queue, queue_info *dequeue);

#endif /* __DRV_VENC_QUEUE_MNG_H__ */
