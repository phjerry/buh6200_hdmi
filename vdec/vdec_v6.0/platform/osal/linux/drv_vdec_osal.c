#include "drv_vdec_osal.h"
#include <linux/sched/clock.h>

hi_s32 vdec_osal_atomic_init(vdec_osal_atomic *var)
{
    return osal_atomic_init(var);
}

hi_void vdec_osal_atomic_exit(vdec_osal_atomic *var)
{
    osal_atomic_destory(var);
}

hi_s32 vdec_osal_atomic_inc_return(vdec_osal_atomic *var)
{
    return osal_atomic_inc_return(var);
}

hi_s32 vdec_osal_atomic_dec_return(vdec_osal_atomic *var)
{
    return osal_atomic_dec_return(var);
}

hi_void vdec_osal_atomic_inc(vdec_osal_atomic *var)
{
    (hi_void)osal_atomic_inc_return(var);
}

hi_void vdec_osal_atomic_dec(vdec_osal_atomic *var)
{
    (hi_void)osal_atomic_dec_return(var);
}

hi_s32 vdec_osal_atomic_read(vdec_osal_atomic *var)
{
    return osal_atomic_read(var);
}

hi_void vdec_osal_atomic_set(vdec_osal_atomic *var, hi_s32 value)
{
    osal_atomic_set(var, value);
}

hi_s32 vdec_osal_event_init(vdec_osal_event *s)
{
    return osal_wait_init(&s->wait);
}

static hi_s32 vdec_osal_wait_call_back(const hi_void *args)
{
    vdec_osal_event *event = NULL;

    event = (vdec_osal_event *)args;

    return event->flag == 0 ? 0 : 1;
}

hi_s32 vdec_osal_event_wait(vdec_osal_event *s, hi_s32 ms_wait_time)
{
    hi_s32 ret;

    ret = osal_wait_timeout_interruptible(&s->wait,
        vdec_osal_wait_call_back, s, ms_wait_time);
    s->flag = 0;

    return ret;
}

hi_void vdec_osal_event_give(vdec_osal_event *s)
{
    osal_wait_wakeup(&(s->wait));
}

hi_void vdec_osal_event_exit(vdec_osal_event *s)
{
    osal_wait_destroy(&(s->wait));
}

hi_void vdec_osal_sema_init(vdec_osal_sema *s)
{
    osal_sem_init(s, 1);
}

hi_void vdec_osal_sema_exit(vdec_osal_sema *s)
{
    osal_sem_destory(s);
}

hi_s32 vdec_osal_sema_down(vdec_osal_sema *s)
{
    if (osal_sem_down_interruptible(s)) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void vdec_osal_sema_up(vdec_osal_sema *s)
{
    osal_sem_up(s);
}

hi_u32 vdec_osal_get_time(hi_void)
{
    hi_u32 curr_us;
    hi_u64 sys_time;

    sys_time = osal_sched_clock();
    curr_us = osal_div_u64(sys_time, 1000); /* 1000 ns to 1 us */

    return curr_us;
}

