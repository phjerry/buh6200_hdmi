/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:osal_math
* Author: seg
* Create: 2019-10-11
*/

#include "hi_osal.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>


void osal_panic(const char *fmt, const char *fun, int line, const char *cond)
{
    panic(fmt, fun, line, cond);
}
EXPORT_SYMBOL(osal_panic);

void osal_dump_stack(void)
{
    dump_stack();
}
EXPORT_SYMBOL(osal_dump_stack);
