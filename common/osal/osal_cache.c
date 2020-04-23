/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:osal_math
* Author: seg
* Create: 2019-10-11
*/

#include "hi_osal.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/cacheflush.h>
#include <linux/dma-direction.h>


void osal_flush_dcache_area(void *kvirt, unsigned long phys_addr, unsigned long size)
{
    /* flush dcache byaddr */
}
EXPORT_SYMBOL(osal_flush_dcache_area);

