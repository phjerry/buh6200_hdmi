/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:osal_math
* Author: seg
* Create: 2019-10-11
*/

#include "hi_osal.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/barrier.h>

void osal_mb(void)
{
    mb();
}
EXPORT_SYMBOL(osal_mb);
void osal_rmb(void)
{
    rmb();
}
EXPORT_SYMBOL(osal_rmb);
void osal_wmb(void)
{
    wmb();
}
EXPORT_SYMBOL(osal_wmb);
void osal_smp_mb(void)
{
    smp_mb();
}
EXPORT_SYMBOL(osal_smp_mb);
void osal_smp_rmb(void)
{
    smp_rmb();
}
EXPORT_SYMBOL(osal_smp_rmb);
void osal_smp_wmb(void)
{
    smp_wmb();
}
EXPORT_SYMBOL(osal_smp_wmb);
void osal_isb(void)
{
    isb();
}
EXPORT_SYMBOL(osal_isb);
void osal_dsb(void)
{
#ifdef CONFIG_64BIT
    dsb(sy);
#else
    dsb();
#endif
}
EXPORT_SYMBOL(osal_dsb);
void osal_dmb(void)
{
#ifdef CONFIG_64BIT
    dmb(sy);
#else
    dmb();
#endif
}
EXPORT_SYMBOL(osal_dmb);
