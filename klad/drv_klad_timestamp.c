/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: DFX for klad
 * Author: Hisilicon hisecurity team
 * Create: 2019-08-07
 */

#include "drv_klad_timestamp.h"
#include "hi_drv_mem.h"
#include "drv_klad_proc.h"
#include "hi_osal.h"

#ifdef HI_KLAD_PERF_SUPPORT

typedef struct queue_node {
    queue_elem_type data;
    struct queue_node *next;
} queue_node, *queue_ptr;

typedef struct {
    osal_mutex lock;
    hi_u32 length;
    queue_ptr front;
    queue_ptr rear;
} link_queue;

static link_queue g_klad_queue;

link_queue *drv_klad_get_queue(hi_void)
{
    return &g_klad_queue;
}

hi_u32 klad_timestamp_queue_init(hi_void)
{
    hi_s32 ret;
    link_queue *q = drv_klad_get_queue();

    osal_mutex_init(&q->lock);
    q->length = 0;

    q->rear = osal_kmalloc(HI_ID_KLAD, sizeof(queue_node), OSAL_GFP_KERNEL);
    if (q->rear == HI_NULL) {
        HI_ERR_KLAD("Memory allocate failed\n");
        return HI_ERR_KLAD_NO_MEMORY;
    }

    ret = memset_s(q->rear, sizeof(queue_node), 0, sizeof(queue_node));
    if (ret != EOK) {
        HI_ERR_KLAD("Call memset_s failed.\n");
        return HI_FAILURE;
    }

    q->front = q->rear;
    q->front->next = NULL;
    return HI_SUCCESS;
}

hi_u32 klad_timestamp_queue_destory(hi_void)
{
    link_queue *q = drv_klad_get_queue();

    osal_mutex_lock(&q->lock);

    while (q->front) {
        q->rear = q->front->next;
        hi_free(q->front);
        q->front = q->rear;
    }
    osal_mutex_unlock(&q->lock);
    osal_mutex_destory(&q->lock);
    return HI_SUCCESS;
}

hi_u32 klad_timestamp_queue_clean(hi_void)
{
    link_queue *q = drv_klad_get_queue();

    queue_node *p = HI_NULL_PTR;
    queue_node *t = HI_NULL_PTR;
    osal_mutex_lock(&q->lock);

    q->rear = q->front;
    p = q->front->next;
    q->front->next = NULL;
    while (p) {
        t = p;
        p = p->next;
        hi_free(t);
    }
    q->length = 0;

    osal_mutex_unlock(&q->lock);
    return HI_SUCCESS;
}

static hi_void __timetamp_print1(struct seq_file *p, const hi_char *str, struct time_ns *time_b)
{
    HI_PROC_PRINT(p, "%-16s,%06ld.%09ld s\n", str, time_b->tv_sec, time_b->tv_nsec);
}

static hi_void __timetamp_print2(struct seq_file *p, const hi_char *str, struct time_ns *time_b,
                                 struct time_ns *time_e, struct time_ns *time_new)
{
    /* if time_e < time_new, means time_e is a inivalid time, drop it. */
    if ((time_e->tv_sec < time_new->tv_sec) ||
            ((time_e->tv_sec == time_new->tv_sec) && (time_e->tv_nsec < time_new->tv_nsec))) {
        HI_PROC_PRINT(p, "%-16s,%06ld.%09ld s, invalid timetamp.\n", str, time_e->tv_sec, time_e->tv_nsec);
        return;
    }
    /* if time_new < time_b, means lost part of the time. */
    if ((time_new->tv_sec < time_b->tv_sec) ||
            ((time_new->tv_sec == time_b->tv_sec) && (time_new->tv_nsec < time_b->tv_nsec))) {
        HI_PROC_PRINT(p, "%-16s,%06ld.%09ld s->%06ld.%09ld s time lost.\n", "",
                      time_b->tv_sec, time_b->tv_nsec, time_new->tv_sec, time_new->tv_nsec);
    }

    if (time_new->tv_sec ==  time_e->tv_sec) {
        HI_PROC_PRINT(p, "%-16s,%06ld.%09ld s, cost:%03ld.%06ld ms\n",
                      str, time_e->tv_sec, time_e->tv_nsec,
                      (time_e->tv_nsec - time_new->tv_nsec) / TIME_MS2NS,
                      (time_e->tv_nsec - time_new->tv_nsec) % TIME_MS2NS);
    } else {
        HI_PROC_PRINT(p, "%-16s,%06ld.%09ld s, cost:%03ld.%06ld ms\n",
                      str, time_e->tv_sec, time_e->tv_nsec,
                      ((time_e->tv_sec - time_new->tv_sec) * TIME_S2NS +
                       time_e->tv_nsec - time_new->tv_nsec) / TIME_MS2NS,
                      ((time_e->tv_sec - time_new->tv_sec) * TIME_S2NS +
                       time_e->tv_nsec - time_new->tv_nsec) % TIME_MS2NS);
    }
    /* update time_new */
    time_new->tv_sec = time_e->tv_sec;
    time_new->tv_nsec = time_e->tv_nsec;
}

static hi_void __timestamp_queue_print(queue_elem_type *c, struct seq_file *p)
{
    struct klad_timestamp *t = &c->timestamp;
    struct time_ns time_new;
    struct time_ns time_begin;

    HI_PROC_PRINT(p, "keyladder   :handle 0x%x, index=%d.\n", c->klad_handle, c->klad_hw_id);

    time_new.tv_sec = t->hw_in.tv_sec;
    time_new.tv_nsec = t->hw_in.tv_nsec;

    __timetamp_print1(p, "hw_in", &t->hw_in);

    __timetamp_print2(p, "create_in", &t->hw_in, &t->create_in, &time_new);

    __timetamp_print2(p, "create_out", &t->create_in, &t->create_out, &time_new);

    __timetamp_print2(p, "open_in", &t->create_out, &t->open_in, &time_new);

    __timetamp_print2(p, "open_out", &t->open_in, &t->open_out, &time_new);

    __timetamp_print2(p, "rkp_in", &t->open_out, &t->rkp_in, &time_new);

    __timetamp_print2(p, "rkp_out", &t->rkp_in, &t->rkp_out, &time_new);

    __timetamp_print2(p, "start_in", &t->rkp_out, &t->start_in, &time_new);

    __timetamp_print2(p, "start_out", &t->start_in, &t->start_out, &time_new);

    __timetamp_print2(p, "close_in", &t->start_out, &t->close_in, &time_new);

    __timetamp_print2(p, "close_out", &t->close_in, &t->close_out, &time_new);

    __timetamp_print2(p, "destroy_in", &t->close_out, &t->destroy_in, &time_new);

    __timetamp_print2(p, "destroy_out", &t->destroy_in, &t->destroy_out, &time_new);

    __timetamp_print2(p, "hw_out", &t->destroy_out, &t->hw_out, &time_new);

    /* get the whole time. */
    time_begin.tv_sec = t->create_in.tv_sec;
    time_begin.tv_nsec = t->create_in.tv_nsec;
    __timetamp_print2(p, "whole time", &t->create_in, &time_new, &time_begin);
}

hi_u32 klad_timestamp_dump(struct seq_file *sf)
{
    queue_node *p = HI_NULL_PTR;
    hi_u32 count = 0;
    link_queue *mq = drv_klad_get_queue();
    queue_elem_type *p_data = HI_NULL_PTR;

    if (mq == HI_NULL) {
        return HI_FAILURE;
    }

    HI_PROC_PRINT(sf, "the timestamp list as following:\n");
    HI_PROC_PRINT(sf, "============================================================\n");
    p = mq->front->next;
    while (p) {
        HI_PROC_PRINT(sf, "\n============================================================\n");
        HI_PROC_PRINT(sf, "index: %03d\n", count);
        count++;
        p_data = &p->data;
        __timestamp_queue_print(p_data, sf);
        p = p->next;
    }
    HI_PROC_PRINT(sf, "============================================================\n\n");
    return HI_SUCCESS;
}

static hi_void __drv_klad_queue_set_value(queue_ptr p, hi_u32 klad_hw_id, hi_u32 klad_handle,
                                          struct klad_timestamp *timestamp)
{
    hi_s32 ret;

    p->data.klad_hw_id = klad_hw_id;
    p->data.klad_handle = klad_handle;
    ret = memcpy_s(&p->data.timestamp, sizeof(struct klad_timestamp),
                   timestamp, sizeof(struct klad_timestamp));
    if (ret != EOK) {
        /* do nothing. */
    }
    p->next = NULL;
}

hi_void klad_timestamp_queue(hi_u32 klad_hw_id, hi_u32 klad_handle, struct klad_r_base *base)
{
    link_queue *mq = drv_klad_get_queue();

    osal_mutex_lock(&mq->lock);
    HI_DBG_KLAD("queue hw_id %d, handle 0x%x,  base %p\n", klad_hw_id, klad_handle, base);
    if (mq->length >= QUEUE_MAX_SIZE) {
        queue_node *p = HI_NULL_PTR;
        if (mq->front == mq->rear) {
            goto out;
        }
        p = mq->front->next;
        mq->front->next = p->next;
        if (mq->rear == p) {
            mq->rear = mq->front;
        }
        __drv_klad_queue_set_value(p, klad_hw_id, klad_handle, &base->timestamp);

        mq->rear->next = p;
        mq->rear = p;
    } else {
        queue_ptr s = osal_kmalloc(HI_ID_KLAD, sizeof(queue_node), OSAL_GFP_KERNEL);
        if (s == HI_NULL) {
            HI_ERR_KLAD("memory allocate failed\n");
            goto out;
        }

        if (memset_s(s, sizeof(queue_node), 0, sizeof(queue_node)) != EOK) {
            HI_ERR_KLAD("Call memset_s failed.\n");
            goto out;
        }

        __drv_klad_queue_set_value(s, klad_hw_id, klad_handle, &base->timestamp);

        mq->rear->next = s;
        mq->rear = s;
        mq->length++;
    }
out:
    osal_mutex_unlock(&mq->lock);

    return;
}
#else
hi_u32 klad_timestamp_queue_init(hi_void)
{
    return 0;
}
hi_u32 klad_timestamp_queue_destory(hi_void)
{
    return 0;
}
hi_u32 klad_timestamp_queue_clean(hi_void)
{
    return 0;
}
hi_u32 klad_timestamp_dump(struct seq_file *sf)
{
    return 0;
}
hi_void klad_timestamp_queue(hi_u32 klad_hw_id, hi_u32 klad_handle, struct klad_timestamp *timestamp)
{
    return 0;
}
#endif

