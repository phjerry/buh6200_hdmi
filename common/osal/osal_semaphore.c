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
#include <linux/semaphore.h>
#include <linux/slab.h>

int osal_sem_init(osal_semaphore *sem, int val)
{
    struct semaphore *p = NULL;

    if (sem == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    p = kmalloc(sizeof(struct semaphore), GFP_KERNEL);
    if (p == NULL) {
        osal_printk("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }

    sema_init(p, val);
    sem->sem = p;

    return 0;
}
EXPORT_SYMBOL(osal_sem_init);

int osal_sem_down(osal_semaphore *sem)
{
    struct semaphore *p = NULL;

    if (sem == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    p = (struct semaphore *)(sem->sem);
    down(p);

    return 0;
}
EXPORT_SYMBOL(osal_sem_down);

int osal_sem_down_interruptible(osal_semaphore *sem)
{
    struct semaphore *p = NULL;

    if (sem == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    p = (struct semaphore *)(sem->sem);

    return down_interruptible(p);
}
EXPORT_SYMBOL(osal_sem_down_interruptible);

int osal_sem_trydown(osal_semaphore *sem)
{
    struct semaphore *p = NULL;

    if (sem == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    p = (struct semaphore *)(sem->sem);

    return down_trylock(p);
}
EXPORT_SYMBOL(osal_sem_trydown);

void osal_sem_up(osal_semaphore *sem)
{
    struct semaphore *p = NULL;

    if (sem == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }

    p = (struct semaphore *)(sem->sem);
    up(p);
}
EXPORT_SYMBOL(osal_sem_up);
void osal_sem_destory(osal_semaphore *sem)
{
    struct semaphore *p = NULL;

    if (sem == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }

    p = (struct semaphore *)(sem->sem);
    kfree(p);
    sem->sem = NULL;
}
EXPORT_SYMBOL(osal_sem_destory);
