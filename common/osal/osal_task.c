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
#include <linux/printk.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0))
#include <linux/sched/types.h>
#endif

osal_task *osal_kthread_create(osal_kthread_handler handler, void *data, const char *name, unsigned int stack_size)
{
    struct task_struct *k = NULL;
    errno_t err;

    osal_task *p = (osal_task *)kmalloc(sizeof(osal_task), GFP_KERNEL);
    if (p == NULL) {
        osal_printk("%s - kmalloc error!\n", __FUNCTION__);
        return NULL;
    }

    err = memset_s(p, sizeof(osal_task), 0, sizeof(osal_task));
    if (err != EOK) {
        kfree(p);
        osal_printk("memset_s is failed.\n");
        return NULL;
    }

    k = kthread_run(handler, data, name);
    if (IS_ERR(k)) {
        osal_printk("%s - kthread create error!\n", __FUNCTION__);
        kfree(p);
        return NULL;
    }
    p->task = k;
    return p;
}
EXPORT_SYMBOL(osal_kthread_create);

void osal_kthread_set_priority(osal_task *task, osal_task_priority priority)
{
    struct sched_param param;

    switch (priority) {
        case OSAL_TASK_PRIORITY_HIGH:
            param.sched_priority = 99; /* 99 is priority */
            break;
        case OSAL_TASK_PRIORITY_MIDDLE:
            param.sched_priority = 50; /* 50 is priority */
            break;
        case OSAL_TASK_PRIORITY_LOW:
            param.sched_priority = 10; /* 10 is priority */
            break;
        default:
            return;
    }

    sched_setscheduler(task->task, SCHED_RR, &param);
}
EXPORT_SYMBOL(osal_kthread_set_priority);

void osal_kthread_set_affinity(osal_task *task, int cpu_mask)
{
    struct cpumask cpumask_set;

    if (task == NULL) {
        return;
    }

    if (cpu_mask == 0) {
        return;
    }

    cpumask_clear(&cpumask_set);

    ((OSAL_CPU_0 & (unsigned int)cpu_mask) == 0) ?
        0 : cpumask_set_cpu(0, &cpumask_set);

    ((OSAL_CPU_1 & (unsigned int)cpu_mask) == 0) ?
        0 : cpumask_set_cpu(1, &cpumask_set);

    ((OSAL_CPU_2 & (unsigned int)cpu_mask) == 0) ?
        0 : cpumask_set_cpu(2, &cpumask_set); /* cpu2 */

    ((OSAL_CPU_3 & (unsigned int)cpu_mask) == 0) ?
        0 : cpumask_set_cpu(3, &cpumask_set); /* cpu3 */

    /* kthread bind mask */
}
EXPORT_SYMBOL(osal_kthread_set_affinity);

int osal_kthread_should_stop(void)
{
    return kthread_should_stop();
}
EXPORT_SYMBOL(osal_kthread_should_stop);

void osal_kthread_destroy(osal_task *task, unsigned int stop_flag)
{
    if (task == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }
    /* note: When you call the Kthread_stop function, the thread function cannot be finished, otherwise it will oops. */
    if (stop_flag != 0) {
        kthread_stop ((struct task_struct *)(task->task));
    }
    task->task = NULL;
    kfree(task);
}
EXPORT_SYMBOL(osal_kthread_destroy);

void osal_kthread_schedule(unsigned int sleep_ns)
{
    ktime_t wait_time;
    wait_time = ns_to_ktime(sleep_ns);
    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_hrtimeout(&wait_time, HRTIMER_MODE_REL);
}
EXPORT_SYMBOL(osal_kthread_schedule);

void osal_kthread_set_uninterrupt(void)
{
    current->flags |= PF_NOFREEZE;
    set_current_state(TASK_UNINTERRUPTIBLE);
}
EXPORT_SYMBOL(osal_kthread_set_uninterrupt);

void osal_kthread_set_running(void)
{
    set_current_state(TASK_RUNNING);
}
EXPORT_SYMBOL(osal_kthread_set_running);

void osal_kneon_begin(void)
{
#ifdef CONFIG_NEON
    kernel_neon_begin();
#endif
}
EXPORT_SYMBOL(osal_kneon_begin);

void osal_kneon_end(void)
{
#ifdef CONFIG_NEON
    kernel_neon_end();
#endif
}
EXPORT_SYMBOL(osal_kneon_end);
