/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: adsp OS abstraction layer
 * Author: Audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 CSEC Rectification
 *          2019-10-30 delete global variable
 */

#include "hi_adsp_debug.h"
#include "adsp_osal.h"
#include "adsp_osal_ext.h"
#include "hi_drv_audio.h"
#include "hi_osal.h"

hi_slong adsp_msleep(hi_u32 time_ms)
{
    return osal_msleep(time_ms);
}

hi_s32 adsp_memset(hi_void *s, hi_s32 c, size_t count)
{
    return memset_s(s, count, c, count);
}

hi_s32 adsp_memcpy(hi_void *dest, const hi_void *src, size_t count)
{
    return memcpy_s(dest, count, src, count);
}

hi_s32 adsp_memmove(hi_void *dest, const hi_void *src, size_t count)
{
    return memmove_s(dest, count, src, count);
}

hi_u32 adsp_get_time_ms(hi_void)
{
    return hi_drv_sys_get_time_stamp_ms();
}

hi_void *adsp_kmalloc(hi_u32 module_id, hi_u32 size)
{
    if (size == 0) {
        return HI_NULL;
    }

    return osal_kmalloc(module_id, size, OSAL_GFP_KERNEL);
}

hi_void adsp_kfree(hi_u32 module_id, const hi_void *addr)
{
    if (addr != HI_NULL) {
        osal_kfree(module_id, addr);
    }
}

hi_void *adsp_vmalloc(hi_u32 module_id, hi_u32 size)
{
    if (size == 0) {
        return HI_NULL;
    }

    return osal_vmalloc(module_id, size);
}

hi_void adsp_vfree(hi_u32 module_id, const hi_void *addr)
{
    if (addr != HI_NULL) {
        osal_vfree(module_id, addr);
    }
}

hi_void *adsp_ioremap(hi_u32 cookie, hi_u32 size)
{
    return (hi_void *)osal_ioremap_nocache(cookie, size);
}

hi_void adsp_iounmap(hi_void *addr)
{
    if (addr != HI_NULL) {
        osal_iounmap(addr);
    }
}

hi_void adsp_schedule_task(hi_u32 sleep_ns)
{
    osal_kthread_schedule(sleep_ns);
}

hi_void adsp_set_task_uninterrupt(hi_void)
{
    osal_kthread_set_uninterrupt();
}

hi_void adsp_set_task_running(hi_void)
{
    osal_kthread_set_running();
}

hi_void adsp_destory_task(hi_void *task)
{
    osal_task *kthread = (osal_task *)task;
    if (kthread != HI_NULL) {
        /* always use kthread_should_stop to check when use kthread_stop */
        osal_kthread_destroy(kthread, 1);
    }
}

hi_s32 adsp_create_task(hi_void **task, const hi_char *task_name, hi_s32 (*thread_fn)(hi_void *data))
{
    osal_task *kthread = osal_kthread_create(thread_fn, HI_NULL, task_name, 0);
    if (kthread == HI_NULL) {
        return HI_FAILURE;
    }

    *task = (hi_void *)kthread;

    return HI_SUCCESS;
}

hi_bool adsp_task_should_stop(hi_void)
{
    return !!osal_kthread_should_stop();
}

static osal_timer g_adsp_timer[TIMER_ID_MAX];

/* add timer */
hi_void adsp_add_timer(osal_timer_id id, hi_void (*timer_fun)(hi_ulong data), hi_u32 ms)
{
    if (id >= TIMER_ID_MAX) {
        return;
    }

    osal_timer_init(&g_adsp_timer[id]);
    g_adsp_timer[id].handler = timer_fun;
    g_adsp_timer[id].data = 0;
    osal_timer_set(&g_adsp_timer[id], ms);
}

/* modify timer */
hi_void adsp_mod_timer(osal_timer_id id, hi_u32 ms)
{
    if (id >= TIMER_ID_MAX) {
        return;
    }

    osal_timer_set(&g_adsp_timer[id], ms);
}

/* delete timer */
hi_void adsp_del_timer(osal_timer_id id)
{
    if (id >= TIMER_ID_MAX) {
        return;
    }

    osal_timer_del(&g_adsp_timer[id]);
    osal_timer_destory(&g_adsp_timer[id]);
}

/* delete timer & sync */
hi_void adsp_del_timer_sync(osal_timer_id id)
{
    if (id >= TIMER_ID_MAX) {
        return;
    }

    osal_timer_del(&g_adsp_timer[id]);
    osal_timer_destory(&g_adsp_timer[id]);
}

static osal_spinlock g_adsp_spin_lock[SPIN_LOCK_ID_MAX];

/* init spin lock */
hi_void adsp_spin_init(osal_spin_lock_id id)
{
    if (id >= SPIN_LOCK_ID_MAX) {
        return;
    }

    osal_spin_lock_init(&g_adsp_spin_lock[id]);
}

/* destroy spin lock */
hi_void adsp_spin_deinit(osal_spin_lock_id id)
{
    if (id >= SPIN_LOCK_ID_MAX) {
        return;
    }

    osal_spin_lock_destory(&g_adsp_spin_lock[id]);
}

/* spin lock */
hi_void adsp_spin_lock(osal_spin_lock_id id)
{
    if (id >= SPIN_LOCK_ID_MAX) {
        return;
    }

    osal_spin_lock(&g_adsp_spin_lock[id]);
}

/* spin unlock */
hi_void adsp_spin_unlock(osal_spin_lock_id id)
{
    if (id >= SPIN_LOCK_ID_MAX) {
        return;
    }

    osal_spin_unlock(&g_adsp_spin_lock[id]);
}

hi_void adsp_kneon_begin(hi_void)
{
    osal_kneon_begin();
}

hi_void adsp_kneon_end(hi_void)
{
    osal_kneon_end();
}

