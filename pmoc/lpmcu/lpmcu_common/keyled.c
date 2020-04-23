/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: keyled module
 */

#include "keyled.h"
#include "base.h"
#include "lpmcu_ram_config.h"

hi_u8 g_keyled_wakeup_key;
extern hi_void keyled_isr_usr(hi_void);
extern hi_void keyled_init_usr(hi_void);
extern hi_void keyled_disable_usr(hi_void);
extern hi_void timer_display_usr(hi_void);
extern hi_void chan_display_usr(hi_u32_data channum);
extern hi_void no_display_usr(hi_void);
extern hi_void dbg_display_usr(hi_u16 val);

hi_void timer_display(hi_void)
{
    timer_display_usr();

    return;
}

hi_void chan_display(hi_void)
{
    hi_u32_data channum;
    hi_u32_data reg_data;

    reg_data.val32 = ram_get(LPMCU_KEYLED_DISPLAY_VALUE_ADDR);
    channum.val32 = reg_data.val32;

    chan_display_usr(channum);

    return;
}

hi_void no_display(hi_void)
{
    no_display_usr();

    return;
}

hi_void keyled_early_display(hi_void)
{
    if (g_keyled_display_type == TIME_DISPLAY) {
        timer_display();
    } else if (g_keyled_display_type == DIGITAL_DISPLAY) {
        chan_display();
    } else {
        no_display();
    }

    return;
}

hi_void dbg_display(hi_u16 val)
{
    dbg_display_usr(val);
    return;
}

hi_void keyled_init(hi_void)
{
    keyled_init_usr();

    return;
}

hi_void keyled_disable(hi_void)
{
    keyled_disable_usr();

    return;
}

hi_void keyled_isr(hi_void)
{
    keyled_isr_usr();
    return;
}

