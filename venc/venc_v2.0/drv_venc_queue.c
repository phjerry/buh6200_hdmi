/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-04-22
 */
#include "public.h"
#include "drv_venc_efl.h"
#include "drv_venc_queue.h"

#define HI_QUEUE_ASSERT_RETURN(condition)\
do { \
    if (!(condition)) { \
        HI_ERR_VENC("assert warning\n"); \
        return HI_FAILURE;\
    } \
} while (0)
#define HI_QUEUE_ASSERT(condition)                  \
do {                                                \
    if (!(condition)) {                               \
        HI_ERR_VENC("assert warning\n");            \
        return;                                     \
    }                                               \
} while (0)


hi_void drv_venc_queue_init(drv_venc_queue *que, hi_void *base, hi_s32 max_len)
{
    HI_QUEUE_ASSERT(que != HI_NULL);
    HI_QUEUE_ASSERT(base != HI_NULL);
    HI_QUEUE_ASSERT(max_len > 0);

    que->base = (ULONG *)base;
    que->max_len = max_len;
    que->tail = 0;
    que->head = 0;
    que->real_len = 0;

    return;
}

hi_s32 drv_venc_queue_put_to_head(drv_venc_queue *que, ULONG data)
{
    hi_s32 head;
    hi_s32 real_len;

    HI_QUEUE_ASSERT_RETURN(que != HI_NULL);

    head = que->head;
    real_len = que->real_len;

    if (real_len < que->max_len) {
        que->base[head] = data;
        if ((++head) >= que->max_len) {
            head = 0;
        }
        que->head = head;
        que->real_len = (real_len + 1);

        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

hi_s32 drv_venc_queue_get_from_head(drv_venc_queue *que, ULONG *data)
{
    hi_s32 head;
    hi_s32 real_len;

    HI_QUEUE_ASSERT_RETURN(que != HI_NULL);
    HI_QUEUE_ASSERT_RETURN(data != HI_NULL);

    head = que->head;
    real_len = que->real_len;

    if (real_len > 0) {
        if ((--head) < 0) {
            head += que->max_len;
        }
        *data = que->base[head];
        que->head = head;
        que->real_len = (real_len - 1);

        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

hi_s32 drv_venc_queue_put_to_tail(drv_venc_queue *que, ULONG data)
{
    hi_s32 tail;
    hi_s32 real_len;

    HI_QUEUE_ASSERT_RETURN(que != HI_NULL);

    tail = que->tail;
    real_len = que->real_len;

    if (real_len < que->max_len) {
        if ((--tail) < 0) {
            tail += que->max_len;
        }
        que->base[tail] = data;

        que->tail = tail;
        que->real_len = (real_len + 1);

        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

hi_s32 drv_venc_queue_get_from_tail(drv_venc_queue *que, ULONG *data)
{
    hi_s32 tail;
    hi_s32 real_len;

    HI_QUEUE_ASSERT_RETURN(que != HI_NULL);
    HI_QUEUE_ASSERT_RETURN(data != HI_NULL);

    tail = que->tail;
    real_len = que->real_len;

    if (real_len > 0) {
        *data = que->base[tail];
        if ((++tail) >= que->max_len) {
            tail = 0;
        }
        que->tail = tail;
        que->real_len = (real_len - 1);

        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

hi_s32 drv_venc_queue_search(drv_venc_queue *que, ULONG *data, hi_s32 *index)
{
    hi_s32 tail;
    hi_s32 real_len;

    HI_QUEUE_ASSERT_RETURN(que != HI_NULL);
    HI_QUEUE_ASSERT_RETURN(data != HI_NULL);
    HI_QUEUE_ASSERT_RETURN(index != HI_NULL);

    tail = que->tail + (*index);
    real_len = que->real_len;

    if ((real_len > 0) && ((*index) < real_len)) {
        if ((tail) >= que->max_len) {
            tail -= que->max_len;
        }

        *data = que->base[tail];

        (*index)++;

        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

hi_s32 drv_venc_queue_search_from_head(drv_venc_queue *que, ULONG *data, hi_s32 index)
{
    hi_s32 head;
    hi_s32 real_len;

    HI_QUEUE_ASSERT_RETURN(que != HI_NULL);
    HI_QUEUE_ASSERT_RETURN(data != HI_NULL);
    HI_QUEUE_ASSERT_RETURN(index >= 0);

    head = que->head - index - 1;
    real_len = que->real_len;

    if ((real_len > 0) && (index < real_len)) {
        if ((head) < 0) {
            head += que->max_len;
        }

        *data = que->base[head];

        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

hi_s32 drv_venc_queue_search_from_tail(drv_venc_queue *que, ULONG *data, hi_s32 index)
{
    hi_s32 tail;
    hi_s32 real_len;

    HI_QUEUE_ASSERT_RETURN(que != HI_NULL);
    HI_QUEUE_ASSERT_RETURN(data != HI_NULL);
    HI_QUEUE_ASSERT_RETURN(index >= 0);

    tail = que->tail + index;
    real_len = que->real_len;

    if ((real_len > 0) && (index < real_len)) {
        if ((tail) >= que->max_len) {
            tail -= que->max_len;
        }

        *data = que->base[tail];

        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

hi_s32 drv_venc_queue_get_real_len(drv_venc_queue *que)
{
    HI_QUEUE_ASSERT_RETURN(que != HI_NULL);

    return que->real_len;
}

hi_bool drv_venc_queue_is_full(drv_venc_queue *que)
{
    HI_QUEUE_ASSERT_RETURN(que != HI_NULL);

    return (hi_bool)(que->real_len == que->max_len);
}

hi_s32 drv_venc_queue_get_from_head_for_index(drv_venc_queue *que, ULONG *data, hi_s32 index)
{
    hi_s32 head, tail;
    hi_s32 real_len;
    hi_s32 i = 0;

    HI_QUEUE_ASSERT_RETURN(que != HI_NULL);
    HI_QUEUE_ASSERT_RETURN(data != HI_NULL);

    head = que->head - index - 1;
    real_len = que->real_len;
    tail = que->tail;

    if ((real_len > 0) && (index < real_len)) {
        if ((head) < 0) {
            head += que->max_len;
        }

        *data = que->base[head];
        que->real_len = (real_len - 1);

        if (head > tail) {
            for (i = head; i > tail; i--) {
                que->base[i] = que->base[i - 1];
            }
            if ((++tail) >= que->max_len) {
                tail = 0;
            }
            que->tail = tail;
        } else if (head < tail) {
            for (i = head; i > 0; i--) {
                que->base[i] = que->base[i - 1];
            }
            que->base[0] = que->base[que->max_len - 1];
            for (i = que->max_len - 1; i > tail; i--) {
                que->base[i] = que->base[i - 1];
            }
            if ((++tail) >= que->max_len) {
                tail = 0;
            }
            que->tail = tail;
        } else {
            if ((++tail) >= que->max_len) {
                tail = 0;
            }
            que->tail = tail;
        }

        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

hi_bool drv_venc_queue_is_empty(drv_venc_queue *que)
{
    HI_QUEUE_ASSERT_RETURN(que != HI_NULL);

    return (hi_bool)(que->real_len == 0);
}



