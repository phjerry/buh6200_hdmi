/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __VFMW_SOS_HEADER__
#define __VFMW_SOS_HEADER__

#include <tee_os_hal.h>
#include "drv_tee_mem.h"
#include "vfmw.h"
#include "vfmw_osal.h"

#define OS_MAX_CHAN      (VFMW_CHAN_NUM)
#define OS_MAX_SPIN_LOCK (OS_MAX_CHAN * 4 * 1)

typedef hi_s32 wait_qhead;
typedef hi_s32 spin_lock;

typedef enum {
    MEM_MAP_TYPE_SEC = 0,
    MEM_MAP_TYPE_NSEC,
    MEM_MAP_TYPE_NCACHE,
    MEM_MAP_TYPE_CACHE,
} mem_map_type;

typedef struct {
    wait_qhead queue_head;
    hi_s32 flag;
} kern_event;

typedef struct {
    spin_lock irq_lock;
    unsigned long irq_lockflags;
    hi_s32 is_init;
} kern_irq_lock;

typedef hi_s32 tee_file;
typedef hi_s32 tee_sema;
typedef hi_void *tee_task;
typedef kern_event tee_event;
typedef kern_irq_lock tee_irq_spin_lock;

extern hi_void *__asm_memmove(hi_void *dest, const hi_void *src, hi_u32 n);
extern hi_void *__asm_memcpy(hi_void *dest, const hi_void *src, hi_u32 n);

hi_s32 tee_check_vfmw_uuid(hi_void);
hi_u32 tee_mmap_section(hi_u32 phy_addr, hi_s32 size, hi_u32 sec_mode, hi_u32 is_cached);
hi_s32 tee_unmap_section(hi_u32 vir_addr, hi_s32 size);
hi_void tee_wakeup_ree(hi_void);

#endif
