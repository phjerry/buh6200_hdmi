/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: encoder
* Author: sdk
* Create: 2019-08-13
*/

#ifndef __DRV_VENC_QUEUE_H__
#define __DRV_VENC_QUEUE_H__

#include "hi_type.h"

typedef struct {
    ULONG *base; /* queue base addr    */
    hi_s32 max_len; /* queue max length   */
    hi_s32 tail; /* queue tail pointer */
    hi_s32 head; /* queue head pointer */
    hi_s32 real_len; /* queue real lentth  */
} drv_venc_queue;

hi_void drv_venc_queue_init(drv_venc_queue *que, hi_void *base, hi_s32 max_len);
hi_s32 drv_venc_queue_put_to_head(drv_venc_queue *que, ULONG data);
hi_s32 drv_venc_queue_get_from_head(drv_venc_queue *que, ULONG *data);
hi_s32 drv_venc_queue_put_to_tail(drv_venc_queue *que, ULONG data);
hi_s32 drv_venc_queue_get_from_tail(drv_venc_queue *que, ULONG *data);
hi_s32 drv_venc_queue_search(drv_venc_queue *que, ULONG *data, hi_s32 *index);
hi_s32 drv_venc_queue_search_from_head(drv_venc_queue *que, ULONG *data, hi_s32 index);
hi_s32 drv_venc_queue_search_from_tail(drv_venc_queue *que, ULONG *data, hi_s32 index);
hi_s32 drv_venc_queue_get_real_len(drv_venc_queue *que);
hi_bool drv_venc_queue_is_full(drv_venc_queue *que);
hi_s32 drv_venc_queue_get_from_head_for_index(drv_venc_queue *que, ULONG *data, hi_s32 index);
hi_bool drv_venc_queue_is_empty(drv_venc_queue *que);

#endif