/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:osal_math
* Author: seg
* Create: 2019-10-11
*/

#include "hi_osal.h"
#include "linux/hisilicon/securec.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/atomic.h>
#include <linux/printk.h>
#include <linux/slab.h>

int osal_atomic_init(osal_atomic *atomic)
{
    errno_t err;
    atomic_t *p = NULL;

    if (atomic == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    p = (atomic_t *)kmalloc(sizeof(atomic_t), GFP_KERNEL);
    if (p == NULL) {
        osal_printk("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }

    err = memset_s(p, sizeof(atomic_t), 0, sizeof(atomic_t));
    if (err != EOK) {
        kfree(p);
        p = NULL;
        osal_printk("memset_s is failed.\n");
        return -1;
    }

    atomic->atomic = p;
    return 0;
}
EXPORT_SYMBOL(osal_atomic_init);

void osal_atomic_destory(osal_atomic *atomic)
{
    if (atomic == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }

    kfree(atomic->atomic);
    atomic->atomic = NULL;
}
EXPORT_SYMBOL(osal_atomic_destory);

int osal_atomic_read(osal_atomic *atomic)
{
    atomic_t *p = NULL;

    if (atomic == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    p = (atomic_t *)(atomic->atomic);

    return atomic_read(p);
}
EXPORT_SYMBOL(osal_atomic_read);

void osal_atomic_set(osal_atomic *atomic, int i)
{
    atomic_t *p = NULL;

    if (atomic == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }

    p = (atomic_t *)(atomic->atomic);
    atomic_set(p, i);
}
EXPORT_SYMBOL(osal_atomic_set);

int osal_atomic_inc_return(osal_atomic *atomic)
{
    atomic_t *p = NULL;

    if (atomic == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    p = (atomic_t *)(atomic->atomic);

    return atomic_inc_return(p);
}
EXPORT_SYMBOL(osal_atomic_inc_return);

int osal_atomic_dec_return(osal_atomic *atomic)
{
    atomic_t *p = NULL;

    if (atomic == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    p = (atomic_t *)(atomic->atomic);

    return atomic_dec_return(p);
}
EXPORT_SYMBOL(osal_atomic_dec_return);
