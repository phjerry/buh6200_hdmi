/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: keyled_ct1642_inner module
 */

#include "keyled.h"
#include "base.h"

#ifdef KEYLED_CT1642_INNER

#define KEYLED_PHY_ADDR  0x00851000
/* LED control register */   /* CNcomment:控制寄存器 */
#define LEDC_CONTROL (KEYLED_PHY_ADDR + 0x0)
/* LED config register */   /* CNcomment:LED显示配置寄存器 */
#define LEDC_CONFIG (KEYLED_PHY_ADDR + 0x4)
/* Key interrupt register */  /* CNcomment: 键盘采样中断寄存器 */
#define LEDC_KEYINT (KEYLED_PHY_ADDR + 0x8)
/* Status of keypad sampling in LED module */  /* CNcomment: LED模块采样键盘状态 */
#define LEDC_KEYDATA (KEYLED_PHY_ADDR + 0xc)
/* Parameter of LEDClk signal about the electic high or low
 * CNcomment: LedClk信号高低电平时间参数
 */
#define LEDC_CLKTIM (KEYLED_PHY_ADDR + 0x10)
/* Frequency of LED fresh */  /* CNcomment: LED刷新频率 */
#define LEDC_FRETIM (KEYLED_PHY_ADDR + 0x14)
/* Frequency of LED flash */  /* CNcomment:LED闪烁频率 */
#define LEDC_FLASHTIM (KEYLED_PHY_ADDR + 0x18)
/* Frequency of key scan */  /* CNcomment: 键盘扫描频率 */
#define LEDC_KEYTIM (KEYLED_PHY_ADDR + 0x1c)
/* The first data register for LED display */  /* CNcomment: 第一个LED显示数据寄存器 */
#define LEDC_DATA1 (KEYLED_PHY_ADDR + 0x20)
/* The second data register for LED display */ /* CNcomment:第二个LED显示数据寄存器 */
#define LEDC_DATA2 (KEYLED_PHY_ADDR + 0x24)
/* The third data register for LED display */  /* CNcomment: 第三个LED显示数据寄存器 */
#define LEDC_DATA3 (KEYLED_PHY_ADDR + 0x28)
/* The fourth data register for LED display */  /* CNcomment: 第四个LED显示数据寄存器 */
#define LEDC_DATA4 (KEYLED_PHY_ADDR + 0x2c)
#define LEDC_SYSTIM (KEYLED_PHY_ADDR + 0x34)

/* 2 Control param */
/* 2.1 Key control */ /* CNcomment:按键控制 */
/* Enable key scan */  /* CNcomment: 键盘扫描使能 */
#define KEYPAD_ENABLE 0x00000008
/* Keypad press down interrupt mask */ /* CNcomment:键盘按下中断屏蔽 */
#define KEYPAD_PRESS_INTR_MASK 0x00000002
/* keypad release interrupt mask */ /* CNcomment: 键盘释放中断屏蔽 */
#define KEYPAD_RELEASE_INTR_MASK 0x00000001
/* Keypad press down interrupt */ /* CNcomment:键盘按下中断 */
#define KEYPAD_PRESS_INTR 0x00000002
/* keypad release interrupt */ /* CNcomment: 键盘释放中断 */
#define KEYPAD_RELEASE_INTR 0x00000001


/* 2.2 key data */
#define KEY_7_PRESS 0x80           /* (1 << 7) */
#define KEY_6_PRESS 0x40           /* (1 << 6) */
#define KEY_5_PRESS 0x20           /* (1 << 5) */
#define KEY_4_PRESS 0x10           /* (1 << 4) */
#define KEY_3_PRESS 0x08           /* (1 << 3) */
#define KEY_2_PRESS 0x04           /* (1 << 2) */
#define KEY_1_PRESS 0x02           /* (1 << 1) */
#define KEY_0_PRESS 0x01           /* (1 << 0) */
#define KEYPAD_ERROR 1000

/* 2.3 led control */
#define LED_ENABLE  0x00000200    /* Enable LED */  /* CNcomment:LED模块使能 */
#define LEDC_ENABLE 0x00000100    /* Enable LED display */  /* CNcomment:LED显示使能 */
#define LEDC_FLASH_4 0x00000080   /* Enable the fourth LED flash */  /* CNcomment: 第四LED管闪烁使能 */
#define LEDC_FLASH_3 0x00000040   /* Enable the third LED flash */  /* CNcomment: 第三LED管闪烁使能 */
#define LEDC_FLASH_2 0x00000020   /* Enable the second LED flash */  /* CNcomment:第二LED管闪烁使能 */
#define LEDC_FLASH_1 0x00000010   /* Enable the first LED flash */  /* CNcomment: 第一LED管闪烁使能 */
#define LEDC_FLASH_ALL 0x000000f0 /* Enable all LED flash */  /* CNcomment:全部LED管闪烁 */

/* 2.4  led config */
#define LEDC_ANODE 0x00000002            /* (1 << 1) */
#define LEDC_HIGHT_LEVEL 0x00000001      /* (1 << 0) */

/* 2.5  def clk */
#define SYSTEM_CLK  24        /* unit::MHz def:24MHz */   /* CNcomment:单位:MHz def:24MHz */
#define CLKTIM_DEFAULT  250   /* unit:KHz min:210KHz */   /* CNcomment:单位:KHz min:210KHz */
#define FRETIM_DEFAULT  650   /* unit:Hz  min:108Hz */   /* CNcomment:单位:Hz  min:108Hz */
#define FLASHTIM_DEFAULT  60  /* unit:Hz 10Hz */   /* CNcomment:单位:Hz 10Hz */
#define FLASHLEV_DEFAULT  1   /* 1means  the slowest while 5 means fastest */ /* CNcomment: 1 最慢  5 最快 */
#define KEY_DEFAULT  60       /* unit:Hz  min:0.5Hz */   /* CNcomment:单位:Hz  min:0.5Hz */

/* variable */
__code const hi_u8 g_led_dig_display_code_ct1642[] = {
    0xfc, 0x60, 0xda, 0xf2, 0x66, 0xb6, 0xbe, 0xe0, 0xfe, 0xf6, 0x0
};
static hi_u8 g_key_press_flag_ct1642 = 0;
hi_void chan_display_usr(hi_u32_data channum);

hi_void keyled_init_usr(hi_void)
{
    hi_u32_data reg_data;
    hi_u32_data reg_addr;

    g_key_press_flag_ct1642 = 0;

    /* 0 */
    reg_set(LEDC_CONTROL, 0x0);

    /* 1.0  led */
    reg_set(LEDC_CONFIG, 0x34); /* CT1642 | 8x1 ; */

    /* 1.1 */
    reg_set(LEDC_CLKTIM, 0x1);

    /* 1.2 */
    reg_set(LEDC_FRETIM, 0x2);

    /* 1.3 */
    reg_set(LEDC_KEYTIM, 0x0);

    reg_set(LEDC_SYSTIM, 0x5);

    /* Clean all interrupts */  /* CNcomment: 清所有中断 */
    reg_set(LEDC_KEYINT, 0x5);

    /* Unmask key press&release interrupt */
    reg_data.val32 = (KEYPAD_RELEASE_INTR_MASK | KEYPAD_PRESS_INTR_MASK);
    reg_set(LEDC_CONTROL, reg_data.val32);

    /* Enable Module/LED/KEY */
    reg_addr.val32 = LEDC_CONTROL;
    reg_data.val32 = reg_get(reg_addr.val32);
    reg_data.val32 |= (LED_ENABLE | LEDC_ENABLE | KEYPAD_ENABLE);
    reg_set(reg_addr.val32, reg_data.val32);

    return;
}

hi_void no_display_usr(hi_void)
{
    hi_u32_data  channum;
    hi_u32_data reg_data;
    hi_u32_data reg_addr;

    channum.val32 = 0x0a0a0a0a;

    chan_display_usr(channum);

    /* disable LED */
    reg_addr.val32 = LEDC_CONTROL;
    reg_data.val32 = reg_get(reg_addr.val32);
    reg_data.val32 &= ~(LEDC_ENABLE);
    reg_set(reg_addr.val32, reg_data.val32);

    return;
}

hi_void keyled_disable_usr(hi_void)
{
    hi_u32_data reg_data;
    hi_u32_data reg_addr;

    no_display_usr();

    /* disable KEY, not disable Module to avoid LED leftover */
    reg_addr.val32 = LEDC_CONTROL;
    reg_data.val32 = reg_get(reg_addr.val32);
    reg_data.val32 &= ~(KEYPAD_ENABLE);
    reg_set(reg_addr.val32, reg_data.val32);

    return;
}

hi_void keyled_isr_usr(hi_void)
{
    hi_u8 key = 0;
    hi_u8 interrupt_value = 0;
    hi_u32_data reg_data;

    reg_data.val32 = reg_get(LEDC_KEYINT);
    interrupt_value = reg_data.val8[0] & (KEYPAD_RELEASE_INTR | KEYPAD_PRESS_INTR);

    if (interrupt_value) {
        /* mask press/release key interrupt */
        reg_data.val32 = reg_get(LEDC_CONTROL);
        reg_data.val32 &= ~(KEYPAD_RELEASE_INTR_MASK | KEYPAD_PRESS_INTR_MASK);
        reg_set(LEDC_CONTROL, reg_data.val32);

        if (interrupt_value & KEYPAD_PRESS_INTR) {
            /* get key value */
            reg_data.val32 = reg_get(LEDC_KEYDATA);
            switch (reg_data.val8[0]) {
                case KEY_7_PRESS:
                    key = 8;
                    break;
                case KEY_6_PRESS:
                    key = 6;
                    break;
                case KEY_5_PRESS:
                    key = 5;
                    break;
                case KEY_4_PRESS:
                    key = 4;
                    break;
                case KEY_3_PRESS:
                    key = 3;
                    break;
                case KEY_2_PRESS:
                    key = 2;
                    break;
                case KEY_1_PRESS:
                    key = 1;
                    break;
                case KEY_0_PRESS:
                    key = 0;
                    break;
                default:
                    key = 0xff;
                    break;
            }
            if (key == g_keyled_wakeup_key) {
                g_key_press_flag_ct1642 = 1;
            }
        } else if ((g_key_press_flag_ct1642 == 1) && (interrupt_value & KEYPAD_RELEASE_INTR)) {
            g_wakeup_type = HI_PMOC_WAKEUP_TYPE_KEYLED;
            g_resume_flag = HI_TRUE;
        }

        /* clear press and release key int */
        reg_data.val32 |= (KEYPAD_RELEASE_INTR | KEYPAD_PRESS_INTR);
        reg_set(LEDC_KEYINT, reg_data.val32);
        /* unmask press/release key interrupt */
        if (g_resume_flag != HI_TRUE) {
            reg_data.val32 = reg_get(LEDC_CONTROL);
            reg_data.val32 |= (KEYPAD_PRESS_INTR_MASK | KEYPAD_RELEASE_INTR_MASK);
            reg_set(LEDC_CONTROL, reg_data.val32);
        }
    }

    return;
}

hi_void timer_display_usr(hi_void)
{
    hi_u32_data channum;

    channum.val8[3] = g_time_hour / 10;
    channum.val8[2] = g_time_hour % 10;
    channum.val8[1] = g_time_minute / 10;
    channum.val8[0] = g_time_minute % 10;

    chan_display_usr(channum);

    return;
}

hi_void chan_display_usr(hi_u32_data channum)
{
    hi_u8 index;
    hi_u32_data reg_data;
    hi_u32_data reg_addr;

    reg_addr.val32 = LEDC_CONTROL;
    reg_data.val32 = reg_get(reg_addr.val32);
    reg_data.val32 &= ~(LEDC_ENABLE);
    reg_set(reg_addr.val32, reg_data.val32);

    index = channum.val8[3];
    reg_data.val32 = 0;
    reg_data.val8[0] = g_led_dig_display_code_ct1642[index] | (g_time_dot << 0);
    reg_addr.val32 = LEDC_DATA1;
    reg_set(reg_addr.val32, reg_data.val32);

    index = channum.val8[2];
    reg_data.val32 = 0;
    reg_data.val8[0] = g_led_dig_display_code_ct1642[index] | (g_time_dot << 0);
    reg_addr.val32 = LEDC_DATA2;
    reg_set(reg_addr.val32, reg_data.val32);

    index = channum.val8[1];
    reg_data.val32 = 0;
    reg_data.val8[0] = g_led_dig_display_code_ct1642[index];
    reg_addr.val32 = LEDC_DATA3;
    reg_set(reg_addr.val32, reg_data.val32);

    index = channum.val8[0];
    reg_data.val32 = 0;
    reg_data.val8[0] = g_led_dig_display_code_ct1642[index];
    reg_addr.val32 = LEDC_DATA4;
    reg_set(reg_addr.val32, reg_data.val32);

    /* 2 LEDC_ENABLE; */
    reg_addr.val32 = LEDC_CONTROL;
    reg_data.val32 = reg_get(reg_addr.val32);
    reg_data.val32 |= (LEDC_ENABLE);
    reg_set(reg_addr.val32, reg_data.val32);

    return;
}

hi_void dbg_display_usr(hi_u16 val)
{
    hi_u32_data  channum;
    hi_u8 i;

    if (val > 9999) {
        return;
    }

    for (i = 0; i < 4; i++) {
        channum.val8[i] = (val % 10);
        val = val / 10;
    }

    chan_display_usr(channum);

    return;
}

#endif
