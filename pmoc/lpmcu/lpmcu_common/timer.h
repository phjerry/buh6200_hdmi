/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: timer header file
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include "hi_type.h"

#define TIMER_LOAD (REG_AON_TIMERS_BASE + 0x0)
#define TIMER_VALUE (REG_AON_TIMERS_BASE + 0x04)
#define TIMER_CONTROL (REG_AON_TIMERS_BASE + 0x08)
#define TIMER_INTCLR (REG_AON_TIMERS_BASE + 0x0c)
#define TIMER_RIS (REG_AON_TIMERS_BASE + 0x10)
#define TIMER_MIS (REG_AON_TIMERS_BASE + 0x14)
#define TIMER_BGLOAD (REG_AON_TIMERS_BASE + 0x18)

extern hi_u8 g_time_hour;
extern hi_u8 g_time_minute;
extern hi_u8 g_time_second;
extern hi_u8 g_time_dot;
extern hi_u8 g_keyled_display_type;
extern hi_u32_data g_wait_time;

/* function */
extern hi_void timer_init(hi_void);
extern hi_void timer_enable(hi_void);
extern hi_void timer_disable(hi_void);
extern hi_void timer_isr(hi_void);

#endif
