/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:osal_math
* Author: seg
* Create: 2019-10-11
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
#include <linux/sched/clock.h>
#else
#include <linux/sched.h>
#endif
#include <linux/rtc.h>
#include "hi_osal.h"

int osal_hrtimer_create(osal_hrtimer *hrtimer)
{
    return -1;
}
int osal_hrtimer_start(osal_hrtimer *hrtimer)
{
    return -1;
}
int osal_hrtimer_destory(osal_hrtimer *hrtimer)
{
    return -1;
}

int osal_timer_init(osal_timer *timer)
{
    struct timer_list *t = NULL;

    if (timer == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    t = (struct timer_list *)kmalloc(sizeof(struct timer_list), GFP_KERNEL);
    if (t == NULL) {
        osal_printk("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }

    init_timer(t);
    timer->timer = t;
    return 0;
}
EXPORT_SYMBOL(osal_timer_init);

int osal_timer_set(osal_timer *timer, unsigned long interval)
{
    struct timer_list *t = NULL;
    if ((timer == NULL) || (timer->timer == NULL) || (timer->handler == NULL) || (interval == 0)) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    t = timer->timer;
    t->function = timer->handler;
    t->data = timer->data;
    return mod_timer(t, jiffies + msecs_to_jiffies(interval) - 1);
}
EXPORT_SYMBOL(osal_timer_set);

int osal_timer_del(osal_timer *timer)
{
    struct timer_list *t = NULL;

    if ((timer == NULL) || (timer->timer == NULL) || (timer->handler == NULL)) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    t = timer->timer;
    return del_timer_sync(t);
}
EXPORT_SYMBOL(osal_timer_del);

int osal_timer_destory(osal_timer *timer)
{
    struct timer_list *t = NULL;

    if ((timer == NULL) || (timer->timer == NULL)) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    t = timer->timer;
    del_timer(t);
    kfree(t);
    timer->timer = NULL;
    return 0;
}
EXPORT_SYMBOL(osal_timer_destory);

unsigned long osal_msleep(unsigned int msecs)
{
    return msleep_interruptible(msecs);
}
EXPORT_SYMBOL(osal_msleep);

void osal_msleep_uninterruptible(unsigned int msecs)
{
    msleep(msecs);
}
EXPORT_SYMBOL(osal_msleep_uninterruptible);

void osal_udelay(unsigned int usecs)
{
    udelay(usecs);
}
EXPORT_SYMBOL(osal_udelay);

void osal_mdelay(unsigned int msecs)
{
    mdelay(msecs);
}
EXPORT_SYMBOL(osal_mdelay);

unsigned int osal_get_tickcount()
{
    return jiffies_to_msecs(jiffies);
}
EXPORT_SYMBOL(osal_get_tickcount);

unsigned long long osal_sched_clock()
{
    return sched_clock();
}
EXPORT_SYMBOL(osal_sched_clock);

void osal_get_timeofday(osal_timeval *tv)
{
    struct timeval t;

    if (tv == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }
    do_gettimeofday(&t);

    tv->tv_sec = t.tv_sec;
    tv->tv_usec = t.tv_usec;
}
EXPORT_SYMBOL(osal_get_timeofday);

unsigned long long osal_get_jiffies(void)
{
    return jiffies;
}
EXPORT_SYMBOL(osal_get_jiffies);

int osal_time_after(unsigned long a, unsigned long b)
{
    return (a > b);
}
EXPORT_SYMBOL(osal_time_after);

int osal_time_before(unsigned long a, unsigned long b)
{
    return (b > a);
}
EXPORT_SYMBOL(osal_time_before);

int osal_get_pid(void)
{
    return (int)task_tgid_nr(current);
}
EXPORT_SYMBOL(osal_get_pid);


void osal_rtc_time_to_tm(unsigned long time, osal_rtc_time *tm)
{
    struct rtc_time _tm = { 0 };

    rtc_time_to_tm(time, &_tm);

    tm->tm_sec = _tm.tm_sec;
    tm->tm_min = _tm.tm_min;
    tm->tm_hour = _tm.tm_hour;
    tm->tm_mday = _tm.tm_mday;
    tm->tm_mon = _tm.tm_mon;
    tm->tm_year = _tm.tm_year;
    tm->tm_wday = _tm.tm_wday;
    tm->tm_yday = _tm.tm_yday;
    tm->tm_isdst = _tm.tm_isdst;
}
EXPORT_SYMBOL(osal_rtc_time_to_tm);

void osal_rtc_tm_to_time(osal_rtc_time *tm, unsigned long *time)
{
    struct rtc_time _tm;
    _tm.tm_sec = tm->tm_sec;
    _tm.tm_min = tm->tm_min;
    _tm.tm_hour = tm->tm_hour;
    _tm.tm_mday = tm->tm_mday;
    _tm.tm_mon = tm->tm_mon;
    _tm.tm_year = tm->tm_year;
    _tm.tm_wday = tm->tm_wday;
    _tm.tm_yday = tm->tm_yday;
    _tm.tm_isdst = tm->tm_isdst;

    rtc_tm_to_time(&_tm, time);
}
EXPORT_SYMBOL(osal_rtc_tm_to_time);


int osal_rtc_valid_tm(osal_rtc_time *tm)
{
    struct rtc_time _tm;
    _tm.tm_sec = tm->tm_sec;
    _tm.tm_min = tm->tm_min;
    _tm.tm_hour = tm->tm_hour;
    _tm.tm_mday = tm->tm_mday;
    _tm.tm_mon = tm->tm_mon;
    _tm.tm_year = tm->tm_year;
    _tm.tm_wday = tm->tm_wday;
    _tm.tm_yday = tm->tm_yday;
    _tm.tm_isdst = tm->tm_isdst;

    return rtc_valid_tm(&_tm);
}
EXPORT_SYMBOL(osal_rtc_valid_tm);
