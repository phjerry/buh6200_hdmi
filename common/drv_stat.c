/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2008-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2008-12-16
 */

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_osal.h"

#include "hi_drv_mem.h"
#include "hi_drv_module.h"
#include "hi_drv_stat.h"
#include "drv_common_ioctl.h"

#define HI_STAT_PROC_NAME   "stat"
#define HI_EVENT_PROC_NAME  "low_delay_statistics"

typedef struct {
    struct timeval tv;
    hi_u32 time;
} stat_isr_time;

typedef struct {
    hi_u32 event_ms;
    hi_u32 val1;
    hi_u32 val2;
} stat_event_time;

static stat_isr_time g_isr_time[HI_STAT_ISR_MAX] __attribute__((unused)) = {{{0}}};

static hi_bool g_isr_enable  __attribute__((unused)) = HI_FALSE ;

static stat_event_time g_event_time[HI_STAT_EVENT_MAX]  __attribute__((unused));

hi_s32 drv_stat_ioctl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    switch (cmd) {
        case STAT_CMPI_EVENT: {
            stat_event_info *event_info;

            event_info = (stat_event_info *)arg;
            if (event_info->event >= HI_STAT_EVENT_MAX) {
                return HI_FAILURE;
            }

            hi_drv_stat_event(event_info->event, event_info->value);
            break;
        }
        case STAT_CMPI_LD_EVENT: {
            hi_drv_stat_ld_notify_event((hi_stat_ld_event_info *)arg);
            break;
        }
        default:
            HI_ERR_SYS("unknown command 0x%x\n", cmd);
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#ifdef HI_PROC_SUPPORT
static hi_s32 drv_stat_proc_read(hi_void *s, hi_void *arg)
{
    osal_proc_print(s, "----------- host isr stat -----------\n");
    if (g_isr_enable == HI_FALSE) {
        osal_proc_print(s, "isr stat is disabled!\n");
    } else {
        osal_proc_print(s, "audio isr time = %u us\n", g_isr_time[HI_STAT_ISR_AUDIO].time);
        osal_proc_print(s, "video isr time = %u us\n", g_isr_time[HI_STAT_ISR_VIDEO].time);
        osal_proc_print(s, "demux isr time = %u us\n", g_isr_time[HI_STAT_ISR_DEMUX].time);
        osal_proc_print(s, "sync isr time  = %u us\n", g_isr_time[HI_STAT_ISR_SYNC].time);
        osal_proc_print(s, "vo isr time    = %u us\n", g_isr_time[HI_STAT_ISR_VO].time);
        osal_proc_print(s, "tde isr time   = %u us\n", g_isr_time[HI_STAT_ISR_TDE].time);
    }

    osal_proc_print(s, "----------- host event stat ----------\n");

    osal_proc_print(s, "KEYIN          = %-10u (keyvalue 0x%x)\n", g_event_time[HI_STAT_EVENT_KEY_IN].event_ms,
                    g_event_time[HI_STAT_EVENT_KEY_IN].val1);
    osal_proc_print(s, "KEYOUT         = %-10u (keyvalue 0x%x)\n", g_event_time[HI_STAT_EVENT_KEY_OUT].event_ms,
                    g_event_time[HI_STAT_EVENT_KEY_OUT].val1);
    osal_proc_print(s, "ASTOP          = %-10u\n", g_event_time[HI_STAT_EVENT_AUD_STOP].event_ms);
    osal_proc_print(s, "VSTOP          = %-10u\n", g_event_time[HI_STAT_EVENT_VID_STOP].event_ms);
    osal_proc_print(s, "CONNECT        = %-10u\n", g_event_time[HI_STAT_EVENT_CONNECT].event_ms);
    osal_proc_print(s, "LOCKED         = %-10u\n", g_event_time[HI_STAT_EVENT_LOCKED].event_ms);
    osal_proc_print(s, "ASTART         = %-10u\n", g_event_time[HI_STAT_EVENT_AUD_START].event_ms);
    osal_proc_print(s, "VSTART         = %-10u\n", g_event_time[HI_STAT_EVENT_VID_START].event_ms);
    osal_proc_print(s, "CWSET          = %-10u\n", g_event_time[HI_STAT_EVENT_CW_SET].event_ms);
    osal_proc_print(s, "STREAMIN       = %-10u\n", g_event_time[HI_STAT_EVENT_STREAM_IN].event_ms);
    osal_proc_print(s, "ISTREAMGET     = %-10u (size %d)\n", g_event_time[HI_STAT_EVENT_ISTREAM_GET].event_ms,
                    g_event_time[HI_STAT_EVENT_ISTREAM_GET].val1);
    osal_proc_print(s, "FRAMEDECED     = %-10u\n", g_event_time[HI_STAT_EVENT_IFRAME_OUT].event_ms);

    osal_proc_print(s, "VPSSGET        = %-10u\n", g_event_time[HI_STAT_EVENT_VPSS_GET_FRM].event_ms);
    osal_proc_print(s, "VPSSOUT        = %-10u\n", g_event_time[HI_STAT_EVENT_VPSS_OUT_FRM].event_ms);
    osal_proc_print(s, "AVPLAYGET      = %-10u\n", g_event_time[HI_STAT_EVENT_AVPLAY_GET_FRM].event_ms);

    osal_proc_print(s, "PRESYNC        = %-10u\n", g_event_time[HI_STAT_EVENT_PRE_SYNC].event_ms);
    osal_proc_print(s, "BUFREADY       = %-10u (type %d)\n", g_event_time[HI_STAT_EVENT_BUF_READY].event_ms,
                    g_event_time[HI_STAT_EVENT_BUF_READY].val1);
    osal_proc_print(s, "FRAMESYNCOK    = %-10u\n", g_event_time[HI_STAT_EVENT_FRAME_SYNC_OK].event_ms);

    osal_proc_print(s, "VOGET          = %-10u\n", g_event_time[HI_STAT_EVENT_VO_GET_FRM].event_ms);

    osal_proc_print(s, "IFRAMEINTER    = %-10u\n", g_event_time[HI_STAT_EVENT_IFRAME_INTER].val1);
    osal_proc_print(s, "TOTAL          = %-10u\n",
                    g_event_time[HI_STAT_EVENT_VO_GET_FRM].event_ms - g_event_time[HI_STAT_EVENT_KEY_IN].event_ms);

    return 0;
}
#endif

/* low delay statistics */
struct stat_ld_event_ext {
    struct list_head list;
    hi_stat_ld_event_info event_info;
};

struct stat_ld_event_queue {
    hi_s32                   head;
    hi_s32                   tail;
    hi_s32                   active;
    spinlock_t               lock;
    struct stat_ld_event_ext queue[HI_STAT_MAX_EVENT_QUEUE_SIZE];
};

struct stat_ld_event_head {
    enum {
        ON = 0,
        OFF = 1,
    } state;
    struct mutex               mutex;
    hi_stat_ld_scenes          scenes;
    hi_handle                  filter_handle;
    struct stat_ld_event_queue event_queue[HI_STAT_LD_MAX_EVENT_NUM];
};

static struct stat_ld_event_head g_ld_event_head = {
    .state       = OFF,
    .mutex       = __MUTEX_INITIALIZER(g_ld_event_head.mutex),
    .event_queue = {
        [0 ...(HI_STAT_LD_MAX_EVENT_NUM - 1)] = {
            .lock = __SPIN_LOCK_UNLOCKED(event_queue.lock),
        }
    },
};

#define traverse_scenes_each_event(index, ld_event, scenes)                                                            \
    for ((index) = 0, (ld_event) = g_scenes_desc[scenes][(index)];                                                     \
         (index) < HI_STAT_LD_MAX_EVENT_NUM && (scenes) < HI_STAT_LD_SCENES_MAX && (ld_event) != HI_STAT_LD_EVENT_MAX; \
         (index)++, (ld_event) = g_scenes_desc[scenes][(index) < HI_STAT_LD_MAX_EVENT_NUM ? (index) : 0])

#define traverse_each_event_queue(index, event_queue)                        \
    for ((index) = 0, (event_queue) = &(g_ld_event_head.event_queue[index]); \
         (index) < HI_STAT_LD_MAX_EVENT_NUM;                                 \
         (index)++, (event_queue) = &(g_ld_event_head.event_queue[index]))

#define traverse_queue_valid_entry(index, event_ext, event_queue)                     \
    for ((index) = (event_queue)->head, (event_ext) = &((event_queue)->queue[index]); \
         (event_queue)->tail != (index) % HI_STAT_MAX_EVENT_QUEUE_SIZE;               \
         (index) = ((index) + 1) % HI_STAT_MAX_EVENT_QUEUE_SIZE, (event_ext) = &((event_queue)->queue[index]))

/* reset event statistics state and queue. */
static hi_void drv_stat_reset_nolock(hi_void)
{
    hi_u32 index;
    hi_ulong flag;
    struct stat_ld_event_queue *event_queue = HI_NULL;

    g_ld_event_head.state = OFF;

    /* reset ring queue */
    traverse_each_event_queue(index, event_queue) {
        spin_lock_irqsave(&(event_queue->lock), flag);
        event_queue->active = 0;
        event_queue->head = 0;
        event_queue->tail = 0;
        spin_unlock_irqrestore(&(event_queue->lock), flag);
    }
}

/* start new scenes statistics, reset first. */
hi_s32 hi_drv_stat_ld_start(hi_stat_ld_scenes scenes, hi_handle filter_handle)
{
    hi_u32 index;
    hi_stat_ld_event ld_event;
    hi_ulong flag;

    if (!(scenes < HI_STAT_LD_SCENES_MAX)) {
        HI_ERR_STAT("invalid scenes parameter.\n");
        return HI_FAILURE;
    }

    mutex_lock(&(g_ld_event_head.mutex));

    /* disable and reset current statistics */
    drv_stat_reset_nolock();

    /* save new scenes_id and filter */
    g_ld_event_head.scenes = scenes;
    g_ld_event_head.filter_handle = filter_handle;

    /* set event queue active tag according to scenes */
    traverse_scenes_each_event(index, ld_event, scenes) {
        struct stat_ld_event_queue *event_queue = &(g_ld_event_head.event_queue[ld_event]);
        spin_lock_irqsave(&(event_queue->lock), flag);
        event_queue->active = 1;
        spin_unlock_irqrestore(&(event_queue->lock), flag);
    }

    /* enable statistics */
    g_ld_event_head.state = ON;

    mutex_unlock(&(g_ld_event_head.mutex));
    return HI_SUCCESS;
}

/* stop current scenes statistics. */
hi_void hi_drv_stat_ld_stop(hi_void)
{
    mutex_lock(&(g_ld_event_head.mutex));
    drv_stat_reset_nolock();
    mutex_unlock(&(g_ld_event_head.mutex));
}

#ifdef HI_PROC_SUPPORT
/* capture event statistics snapshot. */
static hi_s32 drv_stat_capture_snapshot(struct stat_ld_event_head *event_head)
{
    if (!event_head) {
        return HI_FAILURE;
    }

    mutex_lock(&(g_ld_event_head.mutex));
    if (g_ld_event_head.state == OFF) {
        goto out;
    }

    /* duplicate entire event queue */
    memcpy(event_head, &g_ld_event_head, sizeof(struct stat_ld_event_head));

    mutex_unlock(&(g_ld_event_head.mutex));
    return HI_SUCCESS;

out:
    mutex_unlock(&(g_ld_event_head.mutex));
    return HI_FAILURE;
}

/* display event head name. */
static hi_void drv_stat_show_event_name(hi_void *s, struct stat_ld_event_head *head)
{
    hi_u32 index;
    hi_stat_ld_event ld_event;

    if (!s || !head) {
        return;
    }

    osal_proc_print(s, "%8s    ", " ");

    traverse_scenes_each_event(index, ld_event, head->scenes) {
        osal_proc_print(s, "%10s    ", g_event_name[ld_event]);
    }

    osal_proc_print(s, "%10s    ", "Total_diff");
    osal_proc_print(s, "\n");
}

/* relink all event entry from queue into one list. */
static hi_s32 drv_stat_ld_relink_event(struct stat_ld_event_head *head, struct list_head *relink_head)
{
    hi_u32 i, j;
    hi_stat_ld_event ld_event;
    struct stat_ld_event_queue *event_queue = HI_NULL;
    struct stat_ld_event_ext *event_ext = HI_NULL;

    if (!head || !relink_head) {
        return HI_FAILURE;
    }

    INIT_LIST_HEAD(relink_head);

    traverse_scenes_each_event(i, ld_event, head->scenes) {
        event_queue = &(head->event_queue[ld_event]);

        traverse_queue_valid_entry(j, event_ext, event_queue) {
            if (head->filter_handle == event_ext->event_info.handle) {
                list_add_tail(&(event_ext->list), relink_head);
            } else {
                HI_ERR_STAT("id:%u, handle:0x%x, frame:%u, time:%u dismatch filter handle(0x%x)\n",
                            event_ext->event_info.ld_event, event_ext->event_info.handle,
                            event_ext->event_info.frame, event_ext->event_info.time, head->filter_handle);
            }
        }
    }

    return HI_SUCCESS;
}

/* display collected detail data. */
static hi_s32 drv_stat_show_event_detail_data(hi_void *s, struct stat_ld_event_head *head)
{
    hi_u32 cur_key, i, j;
    hi_bool calc_diff_time;
    struct list_head relink_head;
    hi_stat_ld_event ld_event;
    struct stat_ld_event_queue *event_queue = HI_NULL;
    struct stat_ld_event_ext *event_ext = HI_NULL;
    struct stat_ld_event_ext *first_event_ext = HI_NULL;
    struct stat_ld_event_ext *last_event_ext = HI_NULL;

    if (head == HI_NULL) {
        HI_ERR_STAT("head is null.\n");
        return HI_FAILURE;
    }

    /* relink new event list */
    if (drv_stat_ld_relink_event(head, &relink_head) != HI_SUCCESS) {
        HI_ERR_STAT("relink event list failed.\n");
        goto out;
    }

    while (!list_empty(&(relink_head))) {
        calc_diff_time = HI_TRUE;
        event_ext = list_first_entry(&(relink_head), struct stat_ld_event_ext, list);
        first_event_ext = event_ext;
        cur_key = event_ext->event_info.frame;

        osal_proc_print(s, "%08d    ", cur_key);

        traverse_scenes_each_event(i, ld_event, head->scenes) {
            hi_bool found = HI_FALSE;
            event_queue = &(head->event_queue[ld_event]);

            traverse_queue_valid_entry(j, event_ext, event_queue) {
                if (head->filter_handle == event_ext->event_info.handle && cur_key == event_ext->event_info.frame &&
                        !list_empty(&(event_ext->list))) {
                    osal_proc_print(s, "%10d    ", event_ext->event_info.time);
                    last_event_ext = event_ext;

                    /* this evt is still in the queue, we should escape it next time. see prev list_empty checking */
                    list_del_init(&(event_ext->list));
                    found = HI_TRUE;
                    break;
                }
            }

            if (found == HI_FALSE) {
                osal_proc_print(s, "%10s    ", "-");
                calc_diff_time = HI_FALSE;
            }
        }

        /* calculate time difference from first event occur to final event finished */
        if (calc_diff_time == HI_TRUE) {
            osal_proc_print(s, "%10d    ",
                            last_event_ext ? last_event_ext->event_info.time - first_event_ext->event_info.time : 0);
        } else {
            osal_proc_print(s, "%10s    ", "-");
        }

        osal_proc_print(s, "\n");
    }
    return HI_SUCCESS;

out:
    return HI_FAILURE;
}

/* show statistics data by 'cat /proc/msp/low_delay_statistics' command. */
static hi_s32 drv_stat_ld_proc_read(hi_void *s, hi_void *arg)
{
    struct stat_ld_event_head *event_head;

    event_head = HI_KZALLOC(HI_ID_STAT, sizeof(struct stat_ld_event_head), GFP_KERNEL);
    if (event_head == HI_NULL) {
        HI_ERR_STAT("create dup event head failed.\n");
        goto out;
    }

    if (drv_stat_capture_snapshot(event_head) != HI_SUCCESS) {
        osal_proc_print(s, "low delay statistics disabled now.\n");
        goto out;
    }

    drv_stat_show_event_name(s, event_head);
    if (drv_stat_show_event_detail_data(s, event_head) != HI_SUCCESS) {
        HI_ERR_STAT("drv_stat_show_event_detail_data failed!\n");
        goto out;
    }

out:
    if (event_head != HI_NULL) {
        HI_KFREE(HI_ID_STAT, event_head);
        event_head = HI_NULL;
    }
    return HI_SUCCESS;
}
#endif

/* add new event into low delay framework. */
hi_void hi_drv_stat_ld_notify_event(hi_stat_ld_event_info *event_info)
{
    hi_ulong flag;
    struct stat_ld_event_queue *event_queue = HI_NULL;
    hi_stat_ld_event_info *dup_event_info = HI_NULL;

    if (unlikely(!event_info)) {
        return;
    }

    if (likely(g_ld_event_head.state == OFF)) {
        return;
    }

    if (event_info->handle != g_ld_event_head.filter_handle) {
        HI_DBG_STAT("filter invalid evt[name:'%s', handle:0x%x, frame:%u, time:%u].\n",
                    g_event_name[event_info->ld_event], event_info->handle, event_info->frame, event_info->time);
        goto out;
    }

    event_queue = &(g_ld_event_head.event_queue[event_info->ld_event]);

    /* event is invalid for current scenes */
    if (!event_queue->active) {
        HI_DBG_STAT("filter inactive evt[name:'%s', handle:0x%x, frame:%u, time:%u].\n",
                    g_event_name[event_info->ld_event], event_info->handle, event_info->frame, event_info->time);
        goto out;
    }

    spin_lock_irqsave(&(event_queue->lock), flag);

    /* ring queue is full */
    if (((event_queue->tail + 1) % HI_STAT_MAX_EVENT_QUEUE_SIZE == event_queue->head)) {
        event_queue->head = (event_queue->head + 1) % HI_STAT_MAX_EVENT_QUEUE_SIZE;
    }

    /* add event into ring queue */
    dup_event_info = &((event_queue->queue[event_queue->tail]).event_info);
    memcpy(dup_event_info, event_info, sizeof(hi_stat_ld_event_info));

    event_queue->tail = (event_queue->tail + 1) % HI_STAT_MAX_EVENT_QUEUE_SIZE;

    spin_unlock_irqrestore(&(event_queue->lock), flag);

out:
    return;
}

#ifdef HI_PROC_SUPPORT
hi_s32 drv_stat_add_proc(hi_void)
{
    osal_proc_entry *item  = HI_NULL;

    item = osal_proc_add(HI_STAT_PROC_NAME, strlen(HI_STAT_PROC_NAME));
    if (item == HI_NULL) {
        HI_ERR_STAT("can't add moudle for stat.\n");
        return HI_FAILURE;
    }
    item->read = drv_stat_proc_read;

    item = osal_proc_add(HI_EVENT_PROC_NAME, strlen(HI_EVENT_PROC_NAME));
    if (item == HI_NULL) {
        HI_ERR_STAT("can't register low delay statistics interface.\n");
        osal_proc_remove(HI_STAT_PROC_NAME, strlen(HI_STAT_PROC_NAME));
        return HI_FAILURE;
    }
    item->read = drv_stat_ld_proc_read;

    return HI_SUCCESS;
}

hi_void drv_stat_remove_proc(hi_void)
{
    osal_proc_remove(HI_EVENT_PROC_NAME, strlen(HI_EVENT_PROC_NAME));
    osal_proc_remove(HI_STAT_PROC_NAME, strlen(HI_STAT_PROC_NAME));

    return;
}
#endif

#if defined(HI_STAT_ISR_SUPPORTED)
hi_void hi_drv_stat_isr_reset(hi_void)
{
    memset((hi_void *)g_isr_time, 0, sizeof(g_isr_time));
}

hi_void hi_drv_stat_isr_enable(hi_void)
{
    g_isr_enable = HI_TRUE;
}

hi_void hi_drv_stat_isr_disable(hi_void)
{
    g_isr_enable = HI_FALSE;
}

hi_void hi_drv_stat_isr_begin(hi_stat_isr isr)
{
    if (g_isr_enable == HI_FALSE) {
        return;
    }

    if (isr < HI_STAT_ISR_MAX) {
        do_gettimeofday(&(g_isr_time[isr].tv));
    }
}

hi_void hi_drv_stat_isr_end(hi_stat_isr isr)
{
    if (g_isr_enable == HI_FALSE) {
        return;
    }

    if (isr < HI_STAT_ISR_MAX) {
        struct timeval tv1, tv2;
        hi_u32 time = 0;

        tv1 = g_isr_time[isr].tv;

        do_gettimeofday(&tv2);

        if (tv2.tv_sec > tv1.tv_sec) {
            time = (tv2.tv_sec - tv1.tv_sec) * 1000000 + tv2.tv_usec - tv1.tv_usec; /* multiply 1000000 to get us */
        } else {
            time = tv2.tv_usec - tv1.tv_usec;
        }

        if (time > g_isr_time[isr].time) {
            g_isr_time[isr].time = time;
        }
    }
}
#endif

hi_void hi_drv_stat_event(hi_stat_event event, hi_u32 value)
{
    hi_u64 sys_time;

    if (event >= HI_STAT_EVENT_MAX) {
        return;
    }

    sys_time = osal_sched_clock();
    do_div(sys_time, 1000000); /* 1000000us */

    switch (event) {
        case HI_STAT_EVENT_KEY_IN:
        case HI_STAT_EVENT_KEY_OUT:
        case HI_STAT_EVENT_ISTREAM_GET:
        case HI_STAT_EVENT_BUF_READY:
        case HI_STAT_EVENT_FRAME_SYNC_OK: {
            g_event_time[event].val1 = value;
            break;
        }
        case HI_STAT_EVENT_CONNECT: {
            g_event_time[HI_STAT_EVENT_LOCKED].event_ms = 0;
            break;
        }
        case HI_STAT_EVENT_VID_START: {
            hi_s32 index;
            for (index = HI_STAT_EVENT_CW_SET; index < HI_STAT_EVENT_MAX; index++) {
                g_event_time[index].event_ms = 0;
                g_event_time[index].val1 = 0;
                g_event_time[index].val2 = 0;
            }
            break;
        }
        case HI_STAT_EVENT_CW_SET:
        case HI_STAT_EVENT_LOCKED: {
            if (g_event_time[event].event_ms != 0) {
                return;
            }
            break;
        }
        case HI_STAT_EVENT_IFRAME_INTER: {
            g_event_time[event].val1 = value - g_event_time[event].val2;
            g_event_time[event].val2 = value;
            break;
        }
        default:
            break;
    }

    g_event_time[event].event_ms = (hi_u32)sys_time;
    return;
}

EXPORT_SYMBOL(hi_drv_stat_ld_start);
EXPORT_SYMBOL(hi_drv_stat_ld_stop);
EXPORT_SYMBOL(hi_drv_stat_ld_notify_event);
EXPORT_SYMBOL(hi_drv_stat_event);

#if defined(HI_STAT_ISR_SUPPORTED)
EXPORT_SYMBOL(hi_drv_stat_isr_begin);
EXPORT_SYMBOL(hi_drv_stat_isr_end);
#endif

