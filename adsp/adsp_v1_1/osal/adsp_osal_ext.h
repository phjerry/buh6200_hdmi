/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: adsp OS abstraction layer ext header file
 * Author: Audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 CSEC Rectification
 */

#ifndef __ADSP_OSAL_EXT_H__
#define __ADSP_OSAL_EXT_H__

#ifdef __XTENSA__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* directly call function in hifi */
#define adsp_msleep            msleep
#define adsp_memset            memset
#define adsp_memcpy            memcpy
#define adsp_memmove           memmove
#define adsp_kmalloc(id, size) malloc((size))
#define adsp_kfree(id, addr)   free((addr))
#define adsp_vmalloc(id, size) malloc((size))
#define adsp_vfree(id, addr)   free((addr))

#define adsp_kneon_begin()
#define adsp_kneon_end()

#define adsp_spin_init(id)
#define adsp_spin_deinit(id)
#define adsp_spin_lock(id)
#define adsp_spin_unlock(id)
#else

/* OS abstraction layer of kernel */
#include "hi_drv_audio.h"

typedef enum {
    SPIN_LOCK_ID_0 = 0,
    SPIN_LOCK_ID_1 = 1,
    SPIN_LOCK_ID_2 = 2,
    SPIN_LOCK_ID_3 = 3,

    SPIN_LOCK_ID_MAX,
} osal_spin_lock_id;

typedef enum {
    TIMER_ID_0 = 0,

    TIMER_ID_MAX,
} osal_timer_id;

hi_slong adsp_msleep(hi_u32 time_ms);

#ifdef __aarch64__
hi_s32 adsp_memset(hi_void *s, hi_s32 c, hi_ulong count);
hi_s32 adsp_memcpy(hi_void *dest, const hi_void *src, hi_ulong count);
hi_s32 adsp_memmove(hi_void *dest, const hi_void *src, hi_ulong count);
#else
hi_s32 adsp_memset(hi_void *s, hi_s32 c, hi_u32 count);
hi_s32 adsp_memcpy(hi_void *dest, const hi_void *src, hi_u32 count);
hi_s32 adsp_memmove(hi_void *dest, const hi_void *src, hi_u32 count);
#endif

hi_u32 adsp_get_time_ms(hi_void);

hi_void *adsp_kmalloc(hi_u32 module_id, hi_u32 size);
hi_void adsp_kfree(hi_u32 module_id, const hi_void *addr);
hi_void *adsp_vmalloc(hi_u32 module_id, hi_u32 size);
hi_void adsp_vfree(hi_u32 module_id, const hi_void *addr);
hi_void *adsp_ioremap(hi_u32 cookie, hi_u32 size);
hi_void adsp_iounmap(hi_void *addr);

hi_void adsp_schedule_task(hi_u32 sleep_ns);
hi_void adsp_set_task_uninterrupt(hi_void);
hi_void adsp_set_task_running(hi_void);
hi_void adsp_destory_task(hi_void *task);
hi_s32  adsp_create_task(hi_void **task, const hi_char *task_name, hi_s32 (*thread_fn)(hi_void *data));
hi_bool adsp_task_should_stop(hi_void);

hi_void adsp_add_timer(osal_timer_id id, hi_void (*timer_fun)(hi_ulong data), hi_u32 ms);
hi_void adsp_mod_timer(osal_timer_id id, hi_u32 ms);
hi_void adsp_del_timer(osal_timer_id id);
hi_void adsp_del_timer_sync(osal_timer_id id);

hi_void adsp_spin_init(osal_spin_lock_id id);
hi_void adsp_spin_deinit(osal_spin_lock_id id);
hi_void adsp_spin_lock(osal_spin_lock_id id);
hi_void adsp_spin_unlock(osal_spin_lock_id id);

hi_void adsp_kneon_begin(hi_void);
hi_void adsp_kneon_end(hi_void);

#endif  /* __XTENSA__ */
#endif  /* __ADSP_OSAL_EXT_H__ */
