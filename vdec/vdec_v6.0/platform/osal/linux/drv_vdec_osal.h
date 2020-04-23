#ifndef __DRV_VDEC_OSAL_H__
#define __DRV_VDEC_OSAL_H__

#include <linux/platform_device.h>
#include <linux/seq_file.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>
#include "hi_type.h"
#include "linux/hisilicon/securec.h"
#include "hi_osal.h"

#define VDEC_OSAL_INIT_ATOMIC(x) ATOMIC_INIT(x)

#define UINT_2_PTR(x) (hi_void*)((uintptr_t)(x))

#define PROC_PRINT(fmt...) osal_proc_print(fmt)
#define COPY_FROM_USER(to, from, len) copy_from_user(to, from, len)

typedef struct {
    osal_wait wait;
    hi_s32 flag;
} kern_event;

typedef struct seq_file proc_file;
typedef kern_event vdec_osal_event;

typedef osal_semaphore vdec_osal_sema;
typedef osal_atomic vdec_osal_atomic;

hi_s32 vdec_osal_atomic_init(vdec_osal_atomic *var);
hi_void vdec_osal_atomic_exit(vdec_osal_atomic *var);
hi_s32 vdec_osal_atomic_inc_return(vdec_osal_atomic *var);
hi_s32 vdec_osal_atomic_dec_return(vdec_osal_atomic *var);
hi_void vdec_osal_atomic_inc(vdec_osal_atomic *var);
hi_void vdec_osal_atomic_dec(vdec_osal_atomic *var);
hi_s32 vdec_osal_atomic_read(vdec_osal_atomic *var);
hi_void vdec_osal_atomic_set(vdec_osal_atomic *var, hi_s32 value);

hi_void vdec_osal_sema_init(vdec_osal_sema *s);
hi_s32 vdec_osal_sema_down(vdec_osal_sema *s);
hi_void vdec_osal_sema_up(vdec_osal_sema *s);
hi_void vdec_osal_sema_exit(vdec_osal_sema *s);


hi_s32 vdec_osal_event_init(vdec_osal_event *s);
hi_s32 vdec_osal_event_wait(vdec_osal_event *s, hi_s32 ms_wait_time);
hi_void vdec_osal_event_give(vdec_osal_event *s);
hi_void vdec_osal_event_exit(vdec_osal_event *s);

hi_handle vdec_osal_get_mem_fd(hi_void *buf);
hi_u32 vdec_osal_get_time(hi_void);

#endif
