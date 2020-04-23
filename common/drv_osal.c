/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2013-07-25
 */

#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "hi_debug.h"
#include "hi_type.h"

#include "hi_drv_osal.h"
#include "hi_drv_sys.h"

hi_s32 hi_drv_osal_request_irq(hi_u32 irq, irq_handler_t handler, hi_ulong flags, const hi_char *name, hi_void *dev)
{
    hi_s32 __irq = irq;
    hi_s32 ret;
    hi_ulong __flags = flags;

    __irq = hi_drv_sys_get_irq_by_name((hi_char *)name);
    if (__irq == -1) {
        HI_PRINT("hi_drv_sys_get_irq_by_name:%s failed.\n", name);
        return -1;
    }

    ret = request_irq(__irq, handler, __flags, name, dev);
    if (ret) {
        HI_PRINT("request_irq failed,[name:%s, irq:%d, flag:%ld],ret:%d \n", name, irq, flags, ret);
    }

    return ret;
}

hi_void hi_drv_osal_free_irq(hi_u32 irq, const hi_char *name, hi_void *dev)
{
    hi_s32 __irq = irq;

    __irq = hi_drv_sys_get_irq_by_name((hi_char *)name);
    if (__irq == -1) {
        return;
    }

    free_irq(__irq, dev);
}

EXPORT_SYMBOL(hi_drv_osal_request_irq);
EXPORT_SYMBOL(hi_drv_osal_free_irq);

