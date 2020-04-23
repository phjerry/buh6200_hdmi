/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: timer module
 */

#include "timer.h"
#include "base.h"
#include "keyled.h"

hi_u8 g_time_hour = 0;
hi_u8 g_time_minute = 0;
hi_u8 g_time_second = 0;
hi_u8 g_time_cnt = 0;
hi_u8 g_time_mod = 0;
hi_u8 g_time_dot = 0;
hi_u8 g_keyled_display_type = 0;

hi_u32_data g_wait_time = {0xffffffff};

hi_void timer_init(hi_void)
{
    hi_u32 reg_value;

    /* 1. disable timer0 and set the 32-bit count */
    reg_set(TIMER_CONTROL, 0x02);

    /* 2. set the Timer Load */
    /* For the unstandard panel, the led refresh/key scan must be set by yourself */
    /* CNcomment: 客户前面板必须人为调用led刷新/key扫描 */
#if (defined KEYLED_CT1642)
    g_time_mod = 50;
    reg_value = 0x00075300;
#elif (defined KEYLED_CT1642_INNER) || (defined KEYLED_74HC164)
    g_time_mod = 1;
    reg_value = 0x016e3600;
#else
    g_time_mod = 8;
    reg_value = 0x002dc6c0;
#endif
    reg_set(TIMER_LOAD, reg_value);

    /* 3. set Timer control */
    /* one frequency-division */
    /* CNcomment:1分频 */
    reg_set(TIMER_CONTROL, 0x62);

    /* 4. clear int */
    reg_set(TIMER_INTCLR, 0x01);

    return;
}

hi_void timer_isr(hi_void)
{
    hi_u32_data reg_data;

    if (reg_get(TIMER_RIS)) {
        /* disable interrupt */
        reg_data.val32 = reg_get(TIMER_CONTROL);
        reg_data.val8[0] &= 0xdf;
        reg_set(TIMER_CONTROL, reg_data.val32);

        /* clear interrupt */
        reg_set(TIMER_INTCLR, 0x01);

        /* enable interrupt */
        reg_data.val32 = reg_get(TIMER_CONTROL);
        reg_data.val8[0] |= 0x20;
        reg_set(TIMER_CONTROL, reg_data.val32);

        g_time_cnt++;

        if (g_time_cnt != g_time_mod) {
            /* Only ct1642 need to refresh LED Data 20ms once */
#ifdef KEYLED_CT1642
            if (g_keyled_display_type == TIME_DISPLAY) {
                timer_display();
            } else if (g_keyled_display_type == DIGITAL_DISPLAY) {
                chan_display();
            }
#endif
            return;
        }

        g_time_cnt = 0;

        g_time_second++;
        if (g_time_second >= 60) {
            g_time_second -= 60;
            g_time_minute++;
        }

        if (g_time_minute >= 60) {
            g_time_minute -= 60;
            g_time_hour++;
        }

        if (g_time_hour >= 24) {
            g_time_hour = 0;
        }

        if (g_keyled_display_type == TIME_DISPLAY) {
            g_time_dot = (1 - g_time_dot);
            timer_display();
        } else if (g_keyled_display_type == DIGITAL_DISPLAY) {
#ifdef KEYLED_CT1642
            chan_display();
#endif
        } else {
            /* do not need to do anything */
        }

        /* time for waking up */
        if (g_wait_time.val32 != 0) {
            g_wait_time.val32--;
        }

        if (g_switch_suspend_type_time > 0) {
            g_switch_suspend_type_time--;
        }

        if (g_wait_time.val32 == 0) {
            g_wakeup_type = HI_PMOC_WAKEUP_TYPE_TIMEOUT;
            g_resume_flag = HI_TRUE;
        }
    }

    return;
}

hi_void timer_enable(hi_void)
{
    hi_u32_data reg_data;

    reg_data.val32 = reg_get(TIMER_CONTROL);
    reg_data.val8[0] |= 0x80;
    reg_set(TIMER_CONTROL, reg_data.val32);

    return;
}

hi_void timer_disable(hi_void)
{
    hi_u32_data reg_data;

    reg_data.val32 = reg_get(TIMER_CONTROL);
    reg_data.val8[0] &= 0x7f;
    reg_set(TIMER_CONTROL, reg_data.val32);

    return;
}

