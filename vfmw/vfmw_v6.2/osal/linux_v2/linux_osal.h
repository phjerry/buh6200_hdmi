/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __LINUX_OSAL__
#define __LINUX_OSAL__

#include "vfmw.h"
#include "hi_drv_sys.h"
#include "hi_drv_mem.h"
#include <asm/cacheflush.h>
#include <linux/kthread.h>
#include <linux/hrtimer.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/wait.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/poll.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/ioctl.h>
#include <linux/semaphore.h>
#include <linux/random.h>
#include "hi_osal.h"

typedef struct {
    osal_wait wait;
    hi_s32 flag;
} kern_event;

typedef struct {
    osal_spinlock irq_lock;
    int is_init;
} kern_irq_lock;


typedef struct task_struct vfmw_osal_task;
typedef  kern_event  osal_event;
typedef  hi_u32  osal_irq_lock;
typedef  kern_irq_lock  osal_irq_spin_lock;
typedef  void  *osal_file;
typedef  osal_semaphore  osal_sema;

#endif
