/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: keyled_fd650 module
 */

#include "keyled.h"
#include "base.h"
#include "gpio.h"

#ifdef KEYLED_FD650

/* 1 cfg param gpio_aon number */
#define STB (23 * 8 + 5)
#define DIO (23 * 8 + 0)
#define CLK (23 * 8 + 3)

#define CMD_DIP_ON (0x8f)
#define CMD_DIP_OFF (0x80)

hi_u32_data g_disp_num;
__idata hi_u8 g_key_buf_arr[2] = {
    0x00, 0x00
};
__code hi_u8 g_led_display_dot_fd650[10] = {
    0xbf, 0x86, 0xdb, 0xcf, 0xe6, 0xed, 0xfd, 0x87, 0xff, 0xef
};
__code hi_u8 g_led_display_code_fd650[10] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f
};

extern hi_u8 hi_gpio_read_sda_in(hi_void);

#define FD650_SCL (CLK)
#define FD650_SDA (DIO)
#define HIGH (1)
#define LOW (0)

#define wait_minute_1(x) do{ \
    hi_u8 i; \
    for(i = 0; i < x; i++); \
}while(0);

#define DELAY wait_minute_1(100)
#define FD650_SCL_SET gpio_write_bit(FD650_SCL, HIGH)
#define FD650_SCL_CLR gpio_write_bit(FD650_SCL, LOW)
#define FD650_SCL_D_OUT gpio_dirset_bit(FD650_SCL, 0)
#define FD650_SDA_SET gpio_write_bit(FD650_SDA, HIGH)
#define FD650_SDA_CLR gpio_write_bit(FD650_SDA, LOW)
#define FD650_SDA_IN hi_gpio_read_sda_in()
#define FD650_SDA_D_OUT gpio_dirset_bit(FD650_SDA, 0)
#define FD650_SDA_D_IN gpio_dirset_bit(FD650_SDA, 1)

/* CNcomment:设置系统参数命令 */
#define FD650_BIT_ENABLE 0x01            /* CNcomment:开启/关闭位 */
#define FD650_BIT_SLEEP 0x04             /* CNcomment:睡眠控制位 */
#define FD650_BIT_7SEG 0x08              /* CNcomment:7段控制位 */
#define FD650_BIT_INTENS1 0x10           /* CNcomment:1级亮度 */
#define FD650_BIT_INTENS2 0x20           /* CNcomment:2级亮度 */
#define FD650_BIT_INTENS3 0x30           /* CNcomment:3级亮度 */
#define FD650_BIT_INTENS4 0x40           /* CNcomment:4级亮度 */
#define FD650_BIT_INTENS5 0x50           /* CNcomment:5级亮度 */
#define FD650_BIT_INTENS6 0x60           /* CNcomment:6级亮度 */
#define FD650_BIT_INTENS7 0x70           /* CNcomment:7级亮度 */
#define FD650_BIT_INTENS8 0x00           /* CNcomment:8级亮度 */

#define FD650_SYSOFF 0x0400                /* CNcomment:关闭显示、关闭键盘 */
#define FD650_SYSON (FD650_SYSOFF | FD650_BIT_ENABLE)           /* CNcomment:开启显示、键盘 */
#define FD650_SLEEPOFF FD650_SYSOFF       /* CNcomment:关闭睡眠 */
#define FD650_SLEEPON (FD650_SYSOFF | FD650_BIT_SLEEP)         /* CNcomment:开启睡眠 */
#define FD650_7SEG_ON (FD650_SYSON | FD650_BIT_7SEG)         /* CNcomment:开启七段模式 */
#define FD650_8SEG_ON (FD650_SYSON | 0x00)         /* CNcomment:开启八段模式 */
#define FD650_SYSON_1 (FD650_SYSON | FD650_BIT_INTENS1)         /* CNcomment:开启显示、键盘、1级亮度 */
#define FD650_SYSON_2 (FD650_SYSON | FD650_BIT_INTENS2)         /* CNcomment:开启显示、键盘、2级亮度 */
#define FD650_SYSON_3 (FD650_SYSON | FD650_BIT_INTENS3)         /* CNcomment:开启显示、键盘、3级亮度 */
#define FD650_SYSON_4 (FD650_SYSON | FD650_BIT_INTENS4)         /* CNcomment:开启显示、键盘、4级亮度 */
#define FD650_SYSON_5 (FD650_SYSON | FD650_BIT_INTENS5)         /* CNcomment:开启显示、键盘、5级亮度 */
#define FD650_SYSON_6 (FD650_SYSON | FD650_BIT_INTENS6)         /* CNcomment:开启显示、键盘、6级亮度 */
#define FD650_SYSON_7 (FD650_SYSON | FD650_BIT_INTENS7)         /* CNcomment:开启显示、键盘、7级亮度 */
#define FD650_SYSON_8 (FD650_SYSON | FD650_BIT_INTENS8)         /* CNcomment:开启显示、键盘、8级亮度 */

/* CNcomment:加载字数据命令 */
#define FD650_DIG0 0x1400
#define FD650_DIG1 0x1500
#define FD650_DIG2 0x1600
#define FD650_DIG3 0x1700
#define FD650_DOT 0x0080                     /* CNcomment:数码管小数点显示 */

/* CNcomment:读取按键代码命令 */
#define FD650_GET_KEY 0x0700                 /* CNcomment:获取按键,返回按键代码 */

hi_u8 hi_gpio_read_sda_in(hi_void)
{
    hi_u8 sda_in = 0;

    gpio_read_bit(FD650_SDA, &sda_in);

    return sda_in;
}

__xdata_reentrant hi_void fd650_start(hi_void)
{
    FD650_SDA_D_OUT;
    FD650_SDA_SET;

    FD650_SCL_D_OUT;
    FD650_SCL_SET;
    DELAY;
    FD650_SDA_CLR;
    DELAY;
    FD650_SCL_CLR;
    return;
}

__xdata_reentrant hi_void fd650_stop(hi_void)
{
    FD650_SDA_D_OUT;
    FD650_SDA_CLR;
    DELAY;
    FD650_SCL_SET;
    DELAY;
    FD650_SDA_SET;
    DELAY;
    FD650_SDA_D_IN;
    return;
}

__xdata_reentrant hi_void fd650_write_byte(hi_u8 dat)
{
    hi_u8 i;

    FD650_SDA_D_OUT;
    for (i = 0; i != 8; i++) {
        if (dat & 0x80) {
            FD650_SDA_SET;
        } else {
            FD650_SDA_CLR;
        }

        DELAY;
        FD650_SCL_SET;
        dat <<= 1;
        DELAY; /* CNcomment:可选延时 */
        FD650_SCL_CLR;
    }

    FD650_SDA_SET;
    FD650_SDA_D_IN;
    DELAY;
    FD650_SCL_SET;
    DELAY;
    FD650_SCL_CLR;

    return;
}

__xdata_reentrant hi_u8 fd650_read_byte(hi_void)
{
    hi_u8 dat;
    hi_u8 i;

    FD650_SDA_D_OUT; /* CNcomment:增加设置为输出 */
    FD650_SDA_SET;
    FD650_SDA_D_IN;
    dat = 0;
    for (i = 0; i != 8; i++) {
        DELAY;  /* CNcomment:可选延时 */
        FD650_SCL_SET;
        DELAY;  /* CNcomment:可选延时 */
        dat <<= 1;
        if (FD650_SDA_IN) {
            dat++;
        }

        FD650_SCL_CLR;
    }

    FD650_SDA_D_OUT; /* CNcomment:增加设置为输出 */
    FD650_SDA_SET;
    DELAY;
    FD650_SCL_SET;
    DELAY;
    FD650_SCL_CLR;

    return dat;
}

__xdata_reentrant hi_void fd650_write(hi_u16 cmd)
{
    fd650_start();
    fd650_write_byte((hi_u8)(((hi_u8)(cmd >> 7) & 0x3E) | 0x40));
    fd650_write_byte((hi_u8)cmd);
    fd650_stop();

    return;
}

hi_u8 keyled_fd650_get_keycode(hi_u8 raw_key)
{
    if ((raw_key & 0x7f) == 0) {
        return 0xFF;
    }

    return raw_key;
}

hi_void keyled_fd650_readkey(hi_void)
{
    hi_u8 key_code = 0;

    fd650_start();
    fd650_write_byte(((hi_u8)(FD650_GET_KEY >> 7) & 0x3E) | 0x01 | 0x40);
    key_code = fd650_read_byte();
    fd650_stop();
    if ((key_code & 0x00000040) == 0) {
        key_code = 0;
    }

    g_key_buf_arr[0] = keyled_fd650_get_keycode(key_code);

    return;
}

hi_void keyled_fd650_display_addr_inc(hi_void)
{
    fd650_write( FD650_SYSON_2);
    fd650_write( FD650_DIG0 | (hi_u8)g_disp_num.val8[3] ); /* CNcomment:发显示数据 */
    fd650_write( FD650_DIG1 | (hi_u8)g_disp_num.val8[2] );
    fd650_write( FD650_DIG2 | (hi_u8)g_disp_num.val8[1] );
    fd650_write( FD650_DIG3 | (hi_u8)g_disp_num.val8[0] );
    return;
}

hi_void no_display_usr(hi_void)
{
    g_disp_num.val32 = 0x0;

    keyled_fd650_display_addr_inc();
    return;
}

hi_void keyled_init_usr(hi_void)
{
#ifndef HI_ADVCA_RELEASE
        printf_str("FD650 Board Init \r\n");
#endif

    no_display_usr();

    return;
}

hi_void keyled_disable_usr(hi_void)
{
    no_display_usr();
    return;
}

hi_void keyled_isr_usr(hi_void)
{
    keyled_fd650_readkey();

    if (g_key_buf_arr[0] == g_keyled_wakeup_key) {
        g_wakeup_type = HI_PMOC_WAKEUP_KEYLED;
        g_resume_flag = HI_TRUE;
    }

    return;
}

hi_void chan_display_usr(hi_u32_data channum)
{
    hi_u8 index;

    index = channum.val8[3];
    g_disp_num.val8[3] = g_led_display_code_fd650[index];
    index = channum.val8[2];
    g_disp_num.val8[2] = (g_led_display_code_fd650[index] | (g_time_dot << 7));
    index = channum.val8[1];
    g_disp_num.val8[1] = g_led_display_code_fd650[index];
    index = channum.val8[0];
    g_disp_num.val8[0] = g_led_display_code_fd650[index];

    keyled_fd650_display_addr_inc();
    return;
}

hi_void timer_display_usr()
{
    hi_u32_data channum;

    channum.val8[3] = g_time_hour / 10;
    channum.val8[2] = g_time_hour % 10;
    channum.val8[1] = g_time_minute / 10;
    channum.val8[0] = g_time_minute % 10;

    chan_display_usr(channum);

    return;
}

hi_void dbg_display_usr(hi_u16 val)
{
    hi_u32_data channum;
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
