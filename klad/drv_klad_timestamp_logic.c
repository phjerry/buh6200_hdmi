/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: DFX for klad
 * Author: Hisilicon hisecurity team
 * Create: 2019-08-07
 */

#include "drv_klad_timestamp_logic.h"
#include "hi_drv_mem.h"
#include "drv_klad_proc.h"

#ifdef HI_KLAD_PERF_SUPPORT

typedef struct queue_node {
    struct klad_timestamp_logic data;
    struct queue_node *next;
} queue_node, *queue_ptr;

typedef struct {
    osal_mutex lock;
    hi_u32 length;
    queue_ptr front;
    queue_ptr rear;
} queue_list;

static queue_list g_klad_queue;

static queue_list *drv_klad_get_queue(hi_void)
{
    return &g_klad_queue;
}

hi_u32 klad_timestamp_logic_queue_init(hi_void)
{
    queue_list *q = drv_klad_get_queue();

    osal_mutex_init(&q->lock);
    q->length = 0;
    q->front = q->rear = hi_malloc(sizeof(queue_node));
    if (q->front == HI_NULL) {
        HI_ERR_KLAD("Memory allocate failed\n");
        return HI_FAILURE;
    }
    q->front->next = NULL;
    return HI_SUCCESS;
}

hi_u32 klad_timestamp_logic_queue_destory(hi_void)
{
    queue_list *q = drv_klad_get_queue();

    if (osal_mutex_lock(&q->lock) != HI_SUCCESS) {
        HI_ERR_KLAD("Lock mutex failed.\n");
        return HI_FAILURE;
    }

    while (q->front) {
        q->rear = q->front->next;
        hi_free(q->front);
        q->front = q->rear;
    }
    osal_mutex_unlock(&q->lock);
    osal_mutex_destory(&q->lock);
    return HI_SUCCESS;
}

hi_u32 klad_timestamp_logic_queue_clean(hi_void)
{
    queue_list *q = drv_klad_get_queue();
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

static hi_void __timetamp_print1(struct seq_file *p, const hi_char *str, const hi_char *str2, struct time_ns *time_b)
{
    HI_PROC_PRINT(p, "%-8s %-32s,%06ld.%09ld s\n", str, str2, time_b->tv_sec, time_b->tv_nsec);
}

static hi_void __timetamp_print2(struct seq_file *p, const hi_char *str, const hi_char *str2, struct time_ns *time_b,
                                 struct time_ns *time_e)
{
    if (time_b->tv_sec ==  time_e->tv_sec) {
        HI_PROC_PRINT(p, "%-8s %-32s,%06ld.%09ld s, cost:%03ld.%06ld ms\n",
                      str, str2, time_e->tv_sec, time_e->tv_nsec,
                      (time_e->tv_nsec - time_b->tv_nsec) / TIME_MS2NS,
                      (time_e->tv_nsec - time_b->tv_nsec) % TIME_MS2NS);
    } else {
        HI_PROC_PRINT(p, "%-8s %-32s,%06ld.%09ld s, cost:%03ld.%06ld ms\n",
                      str, str2, time_e->tv_sec, time_e->tv_nsec,
                      ((time_e->tv_sec - time_b->tv_sec) * TIME_S2NS +
                       time_e->tv_nsec - time_b->tv_nsec) / TIME_MS2NS,
                      ((time_e->tv_sec - time_b->tv_sec) * TIME_S2NS +
                       time_e->tv_nsec - time_b->tv_nsec) % TIME_MS2NS);
    }
}

static hi_void __timestamp_queue_print(struct klad_timestamp_logic *t, struct seq_file *p)
{
    hi_char str[0x40] = {0};

    if (t->tag < RKP) {
        HI_PROC_PRINT(p, "lock tag = 0x%x.\n", t->tag);
        memcpy_s(str, sizeof(str), "lock", strlen("lock"));
    }
    if (t->tag == RKP) {
        HI_PROC_PRINT(p, "rkp tag = 0x%x.\n", t->tag);
        memcpy_s(str, sizeof(str), "rkp", strlen("rkp"));
    }
    if (t->tag >= ALG_IP) {
        HI_PROC_PRINT(p, "alg_ip tag = 0x%x.\n", t->tag);
        memcpy_s(str, sizeof(str), "alg_ip", strlen("alg_ip"));
    }

    __timetamp_print1(p, (const hi_char *)str, "time_ctl_b", &t->time_ctl_b);

    __timetamp_print2(p, (const hi_char *)str, "time_ctl_b->time_ctl_e", &t->time_ctl_b, &t->time_ctl_e);
#ifdef HI_INT_SUPPORT
    __timetamp_print2(p, (const hi_char *)str, "time_ctl_b->time_int_b", &t->time_ctl_b, &t->time_int_b);

    __timetamp_print2(p, (const hi_char *)str, "time_ctl_b->time_int_e", &t->time_ctl_b, &t->time_int_e);
#endif
    __timetamp_print2(p, (const hi_char *)str, "time_ctl_b->time_wait_b", &t->time_ctl_b, &t->time_wait_b);

    __timetamp_print2(p, (const hi_char *)str, "time_ctl_b->time_wait_e", &t->time_ctl_b, &t->time_wait_e);
}

hi_u32 klad_timestamp_logic_dump(struct seq_file *sf)
{
    queue_node *p = HI_NULL_PTR;
    hi_u32 count = 0;
    queue_list *mq = drv_klad_get_queue();
    struct klad_timestamp_logic *p_data = HI_NULL_PTR;

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

static hi_void __drv_klad_queue_set_value(queue_ptr p, struct klad_timestamp_logic *timestamp)
{
    hi_s32 ret;

    ret = memcpy_s(&p->data, sizeof(struct klad_timestamp_logic), timestamp, sizeof(struct klad_timestamp_logic));
    if (ret != EOK) {
        /* do nothing. */
    }
    p->next = NULL;
}

hi_void klad_timestamp_logic_queue(struct klad_timestamp_logic *timestamp)
{
    queue_list *mq = drv_klad_get_queue();

    osal_mutex_lock(&mq->lock);
    if (mq->length >= QUEUE_MAX_SIZE) {
        queue_ptr p = HI_NULL_PTR;
        if (mq->front == mq->rear) {
            goto out;
        }
        p = mq->front->next;
        mq->front->next = p->next;
        if (mq->rear == p) {
            mq->rear = mq->front;
        }
        __drv_klad_queue_set_value(p, timestamp);

        mq->rear->next = p;
        mq->rear = p;
    } else {
        queue_ptr s = hi_malloc(sizeof(queue_node));
        if (s == HI_NULL) {
            HI_ERR_KLAD("memory allocate failed\n");
            goto out;
        }
        __drv_klad_queue_set_value(s, timestamp);

        mq->rear->next = s;
        mq->rear = s;
        mq->length++;
    }
out:
    osal_mutex_unlock(&mq->lock);

    return;
}
#else
hi_u32 klad_timestamp_logic_queue_init(hi_void)
{
    return 0;
}
hi_u32 klad_timestamp_logic_queue_destory(hi_void)
{
    return 0;
}
hi_u32 klad_timestamp_logic_queue_clean(hi_void)
{
    return 0;
}
hi_u32 klad_timestamp_logic_dump(struct seq_file *sf)
{
    return 0;
}
hi_void klad_timestamp_logic_queue(struct klad_timestamp_logic *timestamp)
{
    return 0;
}
#endif

