/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2013-07-25
 */

#ifndef __HI_DRV_OSAL_H__
#define __HI_DRV_OSAL_H__

#include <linux/semaphore.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

#include "hi_type.h"

#define HI_OSAL_DECLARE_MUTEX(x) DEFINE_SEMAPHORE(x)
#define HI_OSAL_INIT_MUTEX(x) sema_init(x, 1)

hi_s32  hi_drv_osal_request_irq(hi_u32 irq, irq_handler_t handler, hi_ulong flags, const hi_char *name, hi_void *dev);
hi_void hi_drv_osal_free_irq(hi_u32 irq, const hi_char *name, hi_void *dev);

#endif

