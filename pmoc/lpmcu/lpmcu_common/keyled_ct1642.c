/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: keyled_ct1642 module
 */

#include "keyled.h"
#include "base.h"
#include "gpio.h"

#ifdef KEYLED_CT1642

/* 1 cfg param */
#define CT1642_CLK     (23 * 8 + 2) /* GPIO23_2 number */
#define CT1642_DAT     (23 * 8 + 4) /* GPIO23_4 number */
#define CT1642_KEY     (23 * 8 + 7) /* GPIO23_7 number */

#define  FALSE 0
#define  TRUE  1

#define wait_minute_1(x) do { \
    hi_u8 i; \
    for(i = 0; i < x; i++); \
} while (0);

/* variable */
hi_u32_data g_disp_num;

__idata hi_u8 g_key_buf_ct1642;
__code const hi_u8 g_led_display_code_ct1642[16] = {
    0xfc, 0x60, 0xda, 0xf2, 0x66, 0xb6, 0xbe,
    0xe0, 0xfe, 0xf6, 0xee, 0x3e, 0x9c, 0x7a,
    0x9e, 0x8e
};

#define  GPIO_CLOCK_SET(val)    gpio_write_bit(CT1642_CLK, val)
#define  GPIO_DATA_SET(val)     gpio_write_bit(CT1642_DAT, val)
#define  GPIO_KEY_GET(val)      gpio_read_bit(CT1642_KEY, &val)

hi_void keyled_ct1642_gpio_dirset(hi_void)
{
     /* config  gpio  direction */
     /* CNcomment:gpio方向设定 */
    gpio_dirset_bit(CT1642_DAT, 0);
    gpio_dirset_bit(CT1642_CLK, 0);
    gpio_dirset_bit(CT1642_KEY, 1);

    return;
}

__xdata_reentrant hi_void ct1642_send_bit_data(hi_u8 v_character, hi_u8 v_position)
{
    hi_u8 bit_posotion;
    hi_u8 bit_character = v_character;
    hi_u8 i;

    switch (v_position) {
        case 0: {
            bit_posotion = 0xe;
            bit_character |= 0x01; /* to light on D1 */
            break;
        }
        case 1: {
            bit_posotion = 0xd;
            break;
        }
        case 2: {
            bit_posotion = 0xb;
            break;
        }
        case 3: {
            bit_posotion = 0x7;
            break;
        }
        case 4: {
            bit_posotion = 0xf;
            break;
        }
        default: {
            bit_posotion = 0xf;
            bit_character = 0x00;
        }
    }

    for (i = 0; i < 4; i++) { /* To choose which LED to show, 4 bits, 1:off; 0:on */
        GPIO_CLOCK_SET(FALSE);
        if ((bit_posotion << i) & 0x08) {
            GPIO_DATA_SET(TRUE);
        } else {
            GPIO_DATA_SET(FALSE);
        }
        GPIO_CLOCK_SET(TRUE);
    }

    for (i = 0; i < 6; i++) {  /* To jump 6 meaningless bits  */
        GPIO_CLOCK_SET(FALSE);
        GPIO_CLOCK_SET(TRUE);
    }

    for(i = 0; i < 8; i++) {
        GPIO_CLOCK_SET(FALSE);
        if ((bit_character << i) & 0x80) {
            GPIO_DATA_SET(TRUE);
        } else {
            GPIO_DATA_SET(FALSE);
        }

        GPIO_CLOCK_SET(TRUE);
    }

    /* when clock is high, put the 18 bits data to flip-latch */
    GPIO_DATA_SET(FALSE);
    GPIO_DATA_SET(TRUE);

    /* output the 18 bits data */
    GPIO_CLOCK_SET(FALSE);
    GPIO_DATA_SET(FALSE);
    GPIO_DATA_SET(TRUE);

    return;
}

hi_void keyled_ct1642_readkey(hi_void)
{
    hi_u8 nkey_press = 0;

    /* Before read key , need to close LED */
    ct1642_send_bit_data(0xff,0x04);
    wait_minute_1(1);

    GPIO_KEY_GET(nkey_press);
    if (nkey_press == 1) {
        /* suppose the last right key is designed for power */
        /* CNcomment:假定最右的那个键是待机键               */
        ct1642_send_bit_data(1 << 7, 0x04);
        wait_minute_1(1);
        nkey_press = 0;

        GPIO_KEY_GET(nkey_press);
        if (nkey_press == 1) {
            g_key_buf_ct1642 = 0x8;
            return;
        }
    }
    g_key_buf_ct1642 = 0;

    return;
}

__xdata_reentrant hi_void keyled_ct1642_display_addr_inc(hi_void)
{
    hi_u8 i = 0;

    for(i = 0; i < 4; i++) { /* 4 - segment */
        ct1642_send_bit_data(g_disp_num.val8[3 - i], i);
        wait_minute_1(5);
    }

    return ;
}

hi_void no_display_usr(hi_void)
{
    g_disp_num.val32 = 0x0;

    keyled_ct1642_display_addr_inc();

    return;
}


hi_void keyled_init_usr(hi_void)
{
    keyled_ct1642_gpio_dirset();

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
    hi_u8 key = 0xff;

    keyled_ct1642_readkey();
    key = g_key_buf_ct1642;

    if(key == g_keyled_wakeup_key) {
        g_wakeup_type = HI_PMOC_WAKEUP_KEYLED;
        g_resume_flag = HI_TRUE;
    }

    return;
}

hi_void chan_display_usr(hi_u32_data channum)
{
    hi_u8 index;

    index = channum.val8[3];
    g_disp_num.val8[3] = g_led_display_code_ct1642[index];

    index = channum.val8[2];
    g_disp_num.val8[2] = (g_led_display_code_ct1642[index] | (g_time_dot << 0));

    index = channum.val8[1]; /* (1 th) element of array */
    g_disp_num.val8[1] = g_led_display_code_ct1642[index];

    index = channum.val8[0]; /* (0 th) element of array */
    g_disp_num.val8[0] = g_led_display_code_ct1642[index];

    keyled_ct1642_display_addr_inc();

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


hi_void dbg_display_usr(hi_u16 val)
{
    hi_u32_data  channum;
    hi_u8 i;

    if (val > 9999) {
        return;
    }

    for (i = 0; i < 4; i++) { /* 4 - segment */
        channum.val8[i] = (val % 10);
        val = val / 10;
    }

    chan_display_usr(channum);

    return;
}

#endif

