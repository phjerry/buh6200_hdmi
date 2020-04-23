/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2004-2019. All rights reserved.
 * Description:
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <asm/unistd.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/string.h>
#include <asm/signal.h>
#include <linux/time.h>
#include <linux/unistd.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/poll.h>

#include "drv_keyled_fd650.h"

#include "drv_keyled.h"
#include "hi_type.h"
#include "hi_drv_module.h"
#include "drv_gpio_ext.h"
#include "hi_errno.h"

#include "hi_unf_keyled.h"
#include "hi_drv_keyled.h"
#include "drv_keyled_ioctl.h"



/*
 * macro define section
 */
#define DEFAULT_REP_KEY_TIME 300

#define CMD_MODE 0x1 /* 0x1 for 5 digits 12 segs, 0 for 4 digits 13 segs */
#define CMD_DIP_OFF 0x80
#define CMD_DIP_ON 0x8F

#define KEYLED_SET_DISPLAY_MODE(index) g_display_mode_index = (index)
#define KEYLED_GET_DISPLAY_MODE(index) g_display_mode_index;

#define KEYLED_US_DELAY(x) udelay(x)

#define KEYLED_TRACE_LEVEL 9
#define KEYLED_TRACE(level, msg ...) do \
    { \
        if ((level) >= KEYLED_TRACE_LEVEL) \
        { \
            HI_INFO_KEYLED("KEYLED_TRACE:%s:%d: ", __FILE__, __LINE__); \
            HI_INFO_KEYLED(msg); \
            HI_INFO_KEYLED("\n"); \
        } \
    } while (0)

static gpio_ext_func *g_gpio_func = HI_NULL;

static keyled_dev_attr g_keyled_dev;
static wait_queue_head_t g_wait_keyleds;

static hi_bool g_dot_flag = HI_FALSE;
static struct  timer_list g_dotflash_timer;
static hi_keyled_time g_keyled_time = {0};
#define FD650_SEC_TIME 1000

DEFINE_SPINLOCK(fd650lock);

static int g_display_switch = DISPLAY_ON;
static int g_key_curr = KEY_MACRO_NO;
static int g_key_last = KEY_MACRO_NO;
static int g_tingle_check = 0;
static int g_key_detected_flag = 0;
static int g_key_code_last = KEY_MACRO_NO;
static int g_key_code_curr = KEY_MACRO_NO;
static int g_key_press_flag = 0;
static int g_cycle_cnt = 0;
static keyled_status g_fd650_status = KEYLED_STATUS_KEYOFF_LEDOFF;

static hi_u32 g_display_mode_index = 0;

static hi_u32 g_led_digital_dis_code_fd650[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f
};

static hi_u32 g_bit_flash_level = FD650_BIT_INTENS7;

/* array define section */
static hi_u8 g_disp_buf_arr[DISPLAY_REG_NUM] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00
};

/* 2 :half of KEY_COL_NUM */
static hi_u8 g_key_buf_arr[KEY_COL_NUM / 2] = {
    0x00, 0x00, 0x00, 0x00, 0x00
};

static hi_void hi_fd650_keyled_scan(hi_size_t data);
static DEFINE_TIMER(keyled_scan_timer, hi_fd650_keyled_scan, 0, 0);

/* function define section */
static hi_s32 hi_gpio_read_sda_in(hi_void)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 sda_in;

    if ((g_gpio_func == HI_NULL) || (g_gpio_func->pfn_gpio_read_bit == HI_NULL)) {
        HI_ERR_KEYLED(" g_gpio_func or g_gpio_func->pfn_gpio_direction_set_bit is NULL!\n");
        return HI_FAILURE;
    }

    ret = g_gpio_func->pfn_gpio_read_bit(FD650_SDA, &sda_in);
    if (ret == HI_FAILURE) {
        KEYLED_TRACE(8, "\n  FD650 hi_gpio_read_sda_in hi_gpio_read_bit failed\n");
        return HI_FAILURE;
    }

    return (sda_in);
}

/****************************************************************
 *
 *  Function Name:drv_keyled_fd650_start
 *
 *  Description:
 *
 *  Parameter:
 *
 *  return：
****************************************************************/
hi_void drv_keyled_fd650_start(hi_void)
{
    /* Move position, first set it to output, then set the value */
    /* CNcomment:移动位置，先设输出再拉电平 */
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

/****************************************************************
 *
 *  Function Name:drv_keyled_fd650_stop
 *
 *  Description:
 *
 *  Parameter:
 *
 *  return：
****************************************************************/
hi_void drv_keyled_fd650_stop(hi_void)
{
    /* Move position, first set it to output, then set the value */
    /* CNcomment:移动位置，先设输出再拉电平 */
    FD650_SDA_D_OUT;
    FD650_SDA_CLR;
    DELAY;
    FD650_SCL_SET;
    DELAY;
    FD650_SDA_SET;
    DELAY;

    return;
}

/****************************************************************
 *
 *  Function Name:drv_keyled_fd650_write_byte
 *
 *  Description: Write One Byte data
 *
 *  Parameter: data
 *
 *  return：
****************************************************************/
hi_void drv_keyled_fd650_write_byte(hi_u8 dat)
{
    hi_u8 i;

    FD650_SDA_D_OUT;
    for (i = 0; i != 8; i++) { /* 8bit */
        if (dat & 0x80) {
            FD650_SDA_SET;
        } else {
            FD650_SDA_CLR;
        }
        DELAY;
        FD650_SCL_SET;
        dat <<= 1;
        DELAY; /* This delay is optional */ /* CNcomment:可选延时 */
        FD650_SCL_CLR;
    }

    /* Change position, first set the value, then set it to output */
    /* CNcomment:交换位置，电平设置后再设为输出 */

    FD650_SDA_SET;
    FD650_SDA_D_IN;
    DELAY;
    FD650_SCL_SET;
    DELAY;
    FD650_SCL_CLR;

    return;
}

hi_u8 drv_keyled_fd650_read_byte(hi_void)
{
    hi_u8 dat;
    hi_u8 i;

    FD650_SDA_D_OUT;
    FD650_SDA_SET;
    FD650_SDA_D_IN;
    dat = 0;
    for (i = 0; i != 8; i++) { /* 8bit */
        DELAY;  /* This delay is optional */
        FD650_SCL_SET;
        DELAY;  /* This delay is optional */
        dat <<= 1;
        if (FD650_SDA_IN) {
            dat++;
        }
        FD650_SCL_CLR;
    }

    FD650_SDA_D_OUT;
    FD650_SDA_SET;
    DELAY;
    FD650_SCL_SET;
    DELAY;
    FD650_SCL_CLR;

    return dat;
}

hi_void drv_keyled_fd650_write(hi_u16 cmd)
{
    hi_size_t flag;

    spin_lock_irqsave(&fd650lock, flag);
    drv_keyled_fd650_start();
    /* shift right 7 digits */
    drv_keyled_fd650_write_byte(((hi_u8)(cmd >> 7) & 0x3E) | 0x40);
    drv_keyled_fd650_write_byte((hi_u8)cmd);
    drv_keyled_fd650_stop();

    spin_unlock_irqrestore(&fd650lock, flag);

    return;
}

hi_u8 drv_keyled_fd650_read(hi_void)
{
    hi_u8 keycode = 0;

    hi_size_t flag;

    spin_lock_irqsave(&fd650lock, flag);

    drv_keyled_fd650_start();
    /* shift right 7 digits */
    drv_keyled_fd650_write_byte((hi_u8)((FD650_GET_KEY >> 7) & 0x3E)| 0x01 | 0x40);
    keycode=drv_keyled_fd650_read_byte();
    drv_keyled_fd650_stop();
    if ((keycode & 0x00000040) == 0) {
        keycode = 0;
    }

    spin_unlock_irqrestore(&fd650lock, flag);

    return keycode;
}

hi_s32 drv_keyled_fd650_gpio_dirset_write(hi_void)
{
    if ((g_gpio_func != HI_NULL) && (g_gpio_func->pfn_gpio_direction_set_bit != HI_NULL)) {
        g_gpio_func->pfn_gpio_direction_set_bit(FD650_SCL, FD50_OUTPUT); /* output */
        g_gpio_func->pfn_gpio_direction_set_bit(FD650_SDA, FD50_OUTPUT); /* output */

        return 0;
    }

    HI_ERR_KEYLED(" g_gpio_func or g_gpio_func->pfn_gpio_direction_set_bit is NULL!\n");
    return HI_FAILURE;
}

hi_s32 drv_keyled_fd650_gpio_dirset_read(hi_void)
{
    if ((g_gpio_func != HI_NULL) && (g_gpio_func->pfn_gpio_direction_set_bit != HI_NULL)) {
        g_gpio_func->pfn_gpio_direction_set_bit(FD650_SCL, FD50_OUTPUT); /* output */
        g_gpio_func->pfn_gpio_direction_set_bit(FD650_SDA, FD50_INPUT); /* input */

        return 0;
    }

    HI_ERR_KEYLED(" g_gpio_func or g_gpio_func->pfn_gpio_direction_set_bit is NULL!\n");
    return HI_FAILURE;
}

hi_s32 drv_keyled_fd650_display_config(hi_u8 *buf_ptr)
{
    hi_s32 i = 0;

    if (buf_ptr == NULL) {
        KEYLED_TRACE(8, "null pointer(buf_ptr)\n");
        return -1;
    }

    for (i = 0; i < DISPLAY_REG_NUM; i++) {
        g_disp_buf_arr[i] = buf_ptr[i];
        KEYLED_TRACE(8, "g_disp_buf_arr[%d]:0x%x\n", i, g_disp_buf_arr[i]);
    }

    return 0;
}

hi_s32 drv_keyled_fd650_tx_byte(hi_u8 data)
{
    hi_size_t flag;

    spin_lock_irqsave(&fd650lock, flag);
    drv_keyled_fd650_write_byte(data);
    spin_unlock_irqrestore(&fd650lock, flag);
    return 0;
}

hi_s32 drv_keyled_fd650_write_burst(hi_u8 *buf_ptr, hi_u32 len)
{
    hi_s32 ret = HI_SUCCESS;
    hi_s32 i = 0;

    ret = drv_keyled_fd650_gpio_dirset_write();
    if (ret != HI_SUCCESS) {
        HI_ERR_KEYLED("drv_keyled_fd650_gpio_dirset_write FAILED! \n");
    }
    KEYLED_US_DELAY(1);

    if ((g_gpio_func == HI_NULL) || (g_gpio_func->pfn_gpio_write_bit == HI_NULL)) {
        HI_ERR_KEYLED(" g_gpio_func or g_gpio_func->pfn_gpio_write_bit is NULL!\n");
        return HI_FAILURE;
    }

    g_gpio_func->pfn_gpio_write_bit(FD650_SCL, 1);

    KEYLED_US_DELAY(1);

    for (i = 0; i < len; i++) {
        drv_keyled_fd650_tx_byte(buf_ptr[i]);
    }

    return 0;
}

hi_s32 keyled_fd650_write(hi_u8 data)
{
    hi_s32 ret = HI_SUCCESS;

    ret = drv_keyled_fd650_write_burst(&data, 1);
    if (ret != HI_SUCCESS) {
        HI_ERR_KEYLED("drv_keyled_fd650_write_burst failed! \n");
    }

    return 0;
}


hi_s32 keyled_fd650_rx_byte(hi_u8 *data)
{
    *data = drv_keyled_fd650_read_byte();

    return 0;
}

hi_s32 keyled_fd650_read(hi_u8 *buf_ptr, hi_u32 len)
{
    hi_u8 keycode = 0;

    if (buf_ptr == NULL) {
        KEYLED_TRACE(8, "null pointer\n");
        return HI_FAILURE;
    }

    keycode = drv_keyled_fd650_read();

    *buf_ptr = keycode;
    return 0;
}

hi_s32 keyled_fd650_display_addr_inc(hi_u8 addr, hi_u8 *buf_ptr, hi_u32 len, hi_u8 ctrl)
{
    hi_u8 disp_tmp[4] = {0};

    if (ctrl != CMD_DIP_OFF) {
        disp_tmp[0] = buf_ptr[0];
        disp_tmp[1] = buf_ptr[2];
        disp_tmp[2] = buf_ptr[4];
        disp_tmp[3] = buf_ptr[6];

        drv_keyled_fd650_write(FD650_SYSON | g_bit_flash_level);
        drv_keyled_fd650_write(FD650_DIG0 | (hi_u8)disp_tmp[0]); /* send display data */
        drv_keyled_fd650_write(FD650_DIG1 | (hi_u8)disp_tmp[1]);
        drv_keyled_fd650_write(FD650_DIG2 | (hi_u8)disp_tmp[2]);
        drv_keyled_fd650_write(FD650_DIG3 | (hi_u8)disp_tmp[3]);
    } else {
        disp_tmp[0] = buf_ptr[0];
        disp_tmp[1] = buf_ptr[2];
        disp_tmp[2] = buf_ptr[4];
        disp_tmp[3] = buf_ptr[6];
        drv_keyled_fd650_write(FD650_SYSOFF | ctrl);
        drv_keyled_fd650_write(FD650_DIG0 | (hi_u8)disp_tmp[3]);
        drv_keyled_fd650_write(FD650_DIG1 | (hi_u8)disp_tmp[2]);
        drv_keyled_fd650_write(FD650_DIG2 | (hi_u8)disp_tmp[1]);
        drv_keyled_fd650_write(FD650_DIG3 | (hi_u8)disp_tmp[0]);
    }

    return 0;
}

hi_s32 keyled_fd650_display_addr_inc_init(hi_u8 addr, hi_u8 *buf_ptr, hi_u32 len, hi_u8 ctrl)
{
    keyled_fd650_display_addr_inc(addr, buf_ptr, len, ctrl);
    return 0;
}

static hi_u32 hi_drv_keyled_key_map(hi_u32 key)
{
    KEYLED_TRACE(8, "\n hi_drv_keyled_key_map key = %d \n", key);

    switch (key) {
        case 4: /* 4 menu */
            return 3; /* map 3 */
        case 16: /* ok */
            return 5; /* map 5 */
        case 8: /* left */
            return 4; /* map 4 */
        case 32: /* right */
            return 6; /* map 6 */
        case 64: /* down */
            return 7; /* map 7 */
        case 2: /* up */
            return 2; /* map 2 */
        case 1: /* standby */
            return 1; /* map 1 */
        case 128: /* quit */
            return 8; /* map 8 */
        default:
            break;
    }

    return 0xff;
}

hi_s32 keyled_fd650_get_keycode(hi_void)
{
    if (g_key_buf_arr[0]) {
        return g_key_buf_arr[0];
    }

    return 0xff;
}

hi_s32 keyled_fd650_get_keycode_sleep(hi_void)
{
    hi_s32 ret = HI_SUCCESS;
    hi_s32 key_code = 0;

    ret = keyled_fd650_read(g_key_buf_arr, KEY_COL_NUM / 2); /* half of KEY_COL_NUM */
    if (ret != HI_SUCCESS) {
        HI_ERR_KEYLED("keyled_fd650_read failed! \n");
    }

    key_code = keyled_fd650_get_keycode();
    key_code = hi_drv_keyled_key_map(key_code);

    return key_code;
}

hi_s32 keyled_fd650_handle_keycode(hi_void)
{
    if (g_key_detected_flag == 0) { /* no key pressed in this period */
        if (g_key_code_last == KEY_MACRO_NO) { /* no key in last period */
            ;
        } else { /* has key in last period */
            if (g_key_press_flag) { /* report last key 'RELEASE', and recode no key pressed in this time */
                if (g_keyled_dev.enable_key_up) {
                    KEYLED_BUF_HEAD.keyled_code = g_key_code_last;
                    KEYLED_BUF_HEAD.keyled_state = KEY_RELEASE;
                    g_keyled_dev.head = KEYLED_INC_BUF(g_keyled_dev.head, g_keyled_dev.buf_len);

                    wake_up_interruptible(&g_wait_keyleds);
                }

                g_key_press_flag = HI_FALSE;
            } else { /* warning: key twitter */
                ;
            }

            g_key_code_last = KEY_MACRO_NO;
        }
    } else { /* has key pressed in this period */
        if (g_key_code_curr != g_key_code_last) { /* new key */
            if (g_key_code_last == KEY_MACRO_NO) { /* no key in last scan */
                ;
            } else { /* has key in last scan */
                if(g_key_press_flag) { /* last key is valid */
                    if (g_keyled_dev.enable_key_up) {
                        KEYLED_BUF_HEAD.keyled_state = KEY_RELEASE;
                        KEYLED_BUF_HEAD.keyled_code  = g_key_code_last;
                        g_keyled_dev.head = KEYLED_INC_BUF(g_keyled_dev.head, g_keyled_dev.buf_len);

                        wake_up_interruptible(&g_wait_keyleds);
                    }

                    g_key_press_flag = HI_FALSE;
                } else { /* warning: key twitter */
                    ;
                }
            }

            g_cycle_cnt = 0;
            g_key_code_last = g_key_code_curr;
        } else { /* old key */
            g_cycle_cnt++;
            if ((g_cycle_cnt == 1) && (g_key_press_flag != HI_TRUE)) {
                KEYLED_BUF_HEAD.keyled_state = KEY_PRESS;
                KEYLED_BUF_HEAD.keyled_code  = g_key_code_last;
                g_keyled_dev.head = KEYLED_INC_BUF(g_keyled_dev.head, g_keyled_dev.buf_len);

                wake_up_interruptible(&g_wait_keyleds);

                g_cycle_cnt = 0;
                g_key_press_flag = HI_TRUE;
            }
            if ((g_cycle_cnt >= g_keyled_dev.repeat_key_delay_time / (SCAN_INTV)) && (g_key_press_flag)) {
                if (g_keyled_dev.enable_repeat_key) {
                    KEYLED_BUF_HEAD.keyled_state = KEY_HOLD;
                    KEYLED_BUF_HEAD.keyled_code  = g_key_code_last;
                    g_keyled_dev.head = KEYLED_INC_BUF(g_keyled_dev.head, g_keyled_dev.buf_len);

                    wake_up_interruptible(&g_wait_keyleds);
                }

                g_cycle_cnt = 0;
                g_key_press_flag = HI_TRUE;
            }
        }
    }

    g_key_code_curr = KEY_MACRO_NO;
    g_key_detected_flag = 0;

    return HI_FAILURE;
}

hi_void keyled_fd650_enable_ledc (hi_void)
{
    g_display_switch = DISPLAY_ON;
    drv_keyled_fd650_write(FD650_SYSON);
    return;
}

hi_void keyled_fd650_disable_ledc (hi_void)
{
    g_display_switch = DISPLAY_OFF;
    drv_keyled_fd650_write(FD650_SYSOFF);
    return;
}

hi_void keyled_display_setLevel(hi_s32 level)
{
    KEYLED_TRACE(8, "\n  keyled_display_setLevel %d \n", level);
    switch (level) {
        case 1: { /* 1 is FD650_BIT_INTENS2 level */
            g_bit_flash_level = FD650_BIT_INTENS2;
            break;
        }
        case 2: { /* 1 is FD650_BIT_INTENS2 leve3 */
            g_bit_flash_level = FD650_BIT_INTENS3;
            break;
        }
        case 3: { /* 1 is FD650_BIT_INTENS2 leve4 */
            g_bit_flash_level = FD650_BIT_INTENS4;
            break;
        }
        case 4: { /* 1 is FD650_BIT_INTENS2 leve7 */
            g_bit_flash_level = FD650_BIT_INTENS7;
            break;
        }
        default: {
            g_bit_flash_level = FD650_BIT_INTENS8;
            break;
        }
    }
}

hi_void keyled_fd650_display_lockled(hi_bool lock_flag)
{
    hi_u32 ctrl = 0;

    if (lock_flag == HI_TRUE) {
        g_disp_buf_arr[6] |= 0x80;
    } else {
        g_disp_buf_arr[6] &= 0x7f;
    }

    if (g_display_switch == DISPLAY_OFF) {
        ctrl = CMD_DIP_OFF;
    } else {
        ctrl = g_bit_flash_level;
    }

    keyled_fd650_display_addr_inc(DISPLAY_REG_START_ADDR, g_disp_buf_arr, DISPLAY_REG_NUM, ctrl);
}

hi_void keyled_fd650_display_dot(hi_bool lock_flag)
{
    hi_u32 ctrl = 0;

    if (lock_flag == HI_TRUE) {
        g_disp_buf_arr[4] |= 0x80;
    } else {
        g_disp_buf_arr[4] &= 0x7f;
    }

    if (g_display_switch == DISPLAY_OFF) {
        ctrl = CMD_DIP_OFF;
    } else {
        ctrl = g_bit_flash_level;
    }

    keyled_fd650_display_addr_inc(DISPLAY_REG_START_ADDR, g_disp_buf_arr, DISPLAY_REG_NUM, ctrl);
}

hi_void keyled_fd650_display(hi_u32 low_data)
{
    hi_u32 i;
    hi_u32 ctrl = 0;

    KEYLED_TRACE(8, "\n  FD650 keyled_display \n");

    for (i = 0; i < DISPLAY_SEG_CURR; i++) {
        if (i == 0) {
            g_disp_buf_arr[(DISPLAY_SEG_CURR - i - 1) * 2] &= 0x80; /* 2 :calculation factor*/
            /* 2: calculation factor 8:8bit */
            g_disp_buf_arr[(DISPLAY_SEG_CURR - i - 1) * 2] += ((hi_u32)low_data >> (8 * i)) & 0xff;
        } else {
            /* 2: calculation factor 8:8bit */
            g_disp_buf_arr[(DISPLAY_SEG_CURR - i - 1) * 2] = ((hi_u32)low_data >> (8 * i)) & 0xff;
        }
    }

    if (g_display_switch == DISPLAY_OFF) {
        ctrl = CMD_DIP_OFF;
    } else {
        ctrl = g_bit_flash_level;
    }

    keyled_fd650_display_addr_inc(DISPLAY_REG_START_ADDR, g_disp_buf_arr, DISPLAY_REG_NUM, ctrl);
}

hi_void keyled_fd650_display_time(hi_keyled_time keyled_time, hi_bool g_dot_flag)
{
    hi_s32 i;
    hi_s32 index = 0;
    hi_u32 ctrl = 0;

    g_disp_buf_arr[0] &= 0x80;

    index = keyled_time.hour / 10; /* 10: calculation factor */
    g_disp_buf_arr[0] += g_led_digital_dis_code_fd650[index];

    index = keyled_time.hour % 10; /* 10: calculation factor */
    g_disp_buf_arr[2] = g_led_digital_dis_code_fd650[index];

    if (g_dot_flag) {
        g_disp_buf_arr[2] += 0x80;
    }

    index = keyled_time.minute / 10; /* 10: calculation factor */
    g_disp_buf_arr[4] = g_led_digital_dis_code_fd650[index];

    index = keyled_time.minute % 10; /* 10: calculation factor */
    g_disp_buf_arr[6] = g_led_digital_dis_code_fd650[index];
    for (i = 0; i < DISPLAY_SEG_CURR; i++) {
        g_disp_buf_arr[2 * i + 1] = 0x1;
    }

    if (DISPLAY_OFF == g_display_switch) {
        ctrl = CMD_DIP_OFF;
    } else {
        ctrl = g_bit_flash_level;
    }

    keyled_fd650_display_addr_inc(DISPLAY_REG_START_ADDR, g_disp_buf_arr, DISPLAY_REG_NUM, ctrl);
}

static hi_void hi_fd650_keyled_scan(hi_size_t data)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 key_code = 0;
    hi_u32 ctrl = 0;

    mod_timer(&keyled_scan_timer,jiffies + msecs_to_jiffies(SCAN_INTV));

    if (DISPLAY_OFF == g_display_switch) {
        ctrl = CMD_DIP_OFF;
    } else {
        ctrl = g_bit_flash_level;
    }

    if ((hi_u32)g_fd650_status & (hi_u32)KEYLED_STATUS_KEYON) {
        g_key_buf_arr[0] = 0;

        /* 2: calculation factor */
        ret = keyled_fd650_read(g_key_buf_arr, KEY_COL_NUM / 2);
        if (ret != HI_SUCCESS) {
            HI_ERR_KEYLED("keyled_fd650_read failed! \n");
        }

        key_code = keyled_fd650_get_keycode();

        if ((key_code == HI_FAILURE) || (key_code == 0xFF)) {
            g_key_detected_flag = 0;
            g_key_code_curr = KEY_MACRO_NO;
        } else {
            g_key_detected_flag = 1;
            g_key_code_curr = key_code;
            g_keyled_dev.key_come++;
        }

        keyled_fd650_handle_keycode();
    }

    return;
}

hi_void keyled_fd650_clear_keybuf_func(hi_void)
{
    g_keyled_dev.tail = g_keyled_dev.head;
    g_keyled_dev.key_come = 0;
    g_keyled_dev.key_read = 0;

    return;
}

static hi_s32 keyled_fd650_soft_init(hi_void)
{
    keyled_fd650_display_addr_inc_init(DISPLAY_REG_START_ADDR, g_disp_buf_arr, DISPLAY_REG_NUM, CMD_DIP_OFF);

    return HI_SUCCESS;
}

static hi_void keyled_dotflash_handler_fd650(hi_size_t data)
{
    mod_timer(&g_dotflash_timer,jiffies + msecs_to_jiffies(FD650_SEC_TIME));

    if (g_dot_flag) {
        g_dot_flag = HI_FALSE;
    } else {
        g_dot_flag = HI_TRUE;
    }

    keyled_fd650_display_time(g_keyled_time, g_dot_flag);

    return;
}

hi_s32 drv_keyled_init_fd650(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    if (g_fd650_status != KEYLED_STATUS_KEYOFF_LEDOFF) {
        return HI_SUCCESS;
    }

    ret = hi_drv_module_get_func(HI_ID_GPIO, (hi_void**)&g_gpio_func);
    if ((ret != HI_SUCCESS) || (g_gpio_func == HI_NULL)) {
        HI_ERR_KEYLED("Get GPIO function failed! \n");
        return HI_FAILURE;
    }

    init_waitqueue_head(&g_wait_keyleds);

    g_keyled_dev.head = 0;
    g_keyled_dev.tail = 0;
    g_keyled_dev.buf_len = KEYBUF_LEN;
    g_keyled_dev.enable_repeat_key = 1;
    g_keyled_dev.key_come   = 0;
    g_keyled_dev.key_read   = 0;
    g_keyled_dev.block_time = DEFAULT_BLOCKTIME;
    g_keyled_dev.repeat_key_delay_time = DEFAULT_REP_KEY_TIME;
    g_keyled_dev.enable_key_up = 1;
    g_key_curr = KEY_MACRO_NO;
    g_key_last = KEY_MACRO_NO;
    g_tingle_check = 0;

    keyled_fd650_clear_keybuf_func();
    keyled_fd650_enable_ledc();
    ret = keyled_fd650_soft_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_KEYLED("keyled_fd650_soft_init failed! \n");
    }

    del_timer_sync(&keyled_scan_timer);
    keyled_scan_timer.expires = jiffies + msecs_to_jiffies(SCAN_INTV);
    add_timer(&keyled_scan_timer);

    g_dotflash_timer.function = keyled_dotflash_handler_fd650;
    init_timer(&g_dotflash_timer);

    return HI_SUCCESS;
}

hi_s32 drv_keyled_deinit_fd650(hi_void)
{
    if (g_fd650_status == KEYLED_STATUS_KEYOFF_LEDOFF) {
        return HI_SUCCESS;
    }

    return HI_SUCCESS;
}

hi_s32 drv_keyled_key_open_fd650(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    ret = drv_keyled_init_fd650();
    if (ret != HI_SUCCESS) {
        HI_ERR_KEYLED("drv_keyled_init_fd650 failed! \n");
    }

    g_fd650_status = (hi_u32)g_fd650_status | (hi_u32)KEYLED_STATUS_KEYON;
    HI_INFO_KEYLED("start IsKeyOpen=%d\n", ((hi_u32)g_fd650_status & (hi_u32)KEYLED_STATUS_KEYON) ? 1 : 0);
    return HI_SUCCESS;
}

hi_s32 drv_keyled_key_close_fd650(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    if (!((hi_u32)g_fd650_status & (hi_u32)KEYLED_STATUS_KEYON)) {
        return HI_SUCCESS;
    }
    g_fd650_status = (hi_u32)g_fd650_status & (~(hi_u32)KEYLED_STATUS_KEYON);
    HI_INFO_KEYLED("stop IsKeyOpen=%d\n", ((hi_u32)g_fd650_status & (hi_u32)KEYLED_STATUS_KEYON) ? 1 : 0);
    del_timer_sync(&keyled_scan_timer);
    del_timer_sync(&g_dotflash_timer);

    ret = drv_keyled_deinit_fd650();
    if (ret != HI_SUCCESS) {
        HI_ERR_KEYLED("drv_keyled_init_fd650 failed! \n");
    }

    return HI_SUCCESS;
}

hi_s32 drv_keyled_key_reset_fd650(hi_void)
{
    keyled_fd650_clear_keybuf_func();

    return HI_SUCCESS;
}

hi_s32 drv_keyled_key_get_value_fd650(hi_u32 *press_status_ptr, hi_u32 *key_id_ptr)
{
    keyled_key_info keyled_to_user = { 0 };
    hi_s32 err = 0;

    if (!((hi_u32)g_fd650_status & (hi_u32)KEYLED_STATUS_KEYON)) {
        return HI_FAILURE;
    }
    if ((g_keyled_dev.head) == (g_keyled_dev.tail)) {
        if (g_keyled_dev.block_time == 0) {
            return HI_ERR_KEYLED_NO_NEW_KEY;
        }

        err = wait_event_interruptible_timeout(g_wait_keyleds, (g_keyled_dev.head != g_keyled_dev.tail),
                                               (hi_slong)(g_keyled_dev.block_time * HZ / 1000));
        if (err <= 0) { /* not wake up by received key */
            return HI_ERR_KEYLED_TIMEOUT;
        }
    }

    if (g_keyled_dev.head != g_keyled_dev.tail) {
        keyled_to_user  = KEYLED_BUF_TAIL;
        g_keyled_dev.tail = KEYLED_INC_BUF(g_keyled_dev.tail, g_keyled_dev.buf_len);
        g_keyled_dev.key_read++;

        *press_status_ptr = keyled_to_user.keyled_state;
        *key_id_ptr = keyled_to_user.keyled_code;
        return HI_SUCCESS;
    } else {
        return HI_ERR_KEYLED_NO_NEW_KEY;
    }

    return HI_SUCCESS;
}

hi_s32 drv_keyled_key_set_block_time_fd650(hi_u32 block_time_ms)
{
    g_keyled_dev.block_time = block_time_ms;
    return HI_SUCCESS;
}

hi_s32 drv_keyled_key_set_repeat_time_fd650(hi_u32 rep_time_ms)
{
    g_keyled_dev.repeat_key_delay_time = rep_time_ms;

    return HI_SUCCESS;
}

hi_s32 drv_keyled_key_enable_repeat_key_fd650(hi_u32 enable_repkey)
{
    g_keyled_dev.enable_repeat_key = enable_repkey;

    return HI_SUCCESS;
}

hi_s32 drv_keyled_key_enable_key_up_fd650(hi_u32 enable_keyup)
{
    g_keyled_dev.enable_key_up = enable_keyup;

    return HI_SUCCESS;
}

hi_s32 drv_keyled_led_open_fd650(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    ret = drv_keyled_init_fd650();
    if (ret != HI_SUCCESS) {
        HI_ERR_KEYLED("drv_keyled_init_fd650 failed! \n");
    }

    g_fd650_status = (hi_u32)g_fd650_status | (hi_u32)KEYLED_STATUS_LEDON;
    HI_INFO_KEYLED("start IsLedOpen=%d\n", ((hi_u32)g_fd650_status & (hi_u32)KEYLED_STATUS_LEDON) ? 1 : 0);

    KEYLED_SET_DISPLAY_MODE(CMD_MODE);
    g_display_switch = DISPLAY_ON;

    return HI_SUCCESS;
}

hi_s32 drv_keyled_led_close_fd650(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    if (!((hi_u32)g_fd650_status & (hi_u32)KEYLED_STATUS_LEDON)) {
        return HI_SUCCESS;
    }
    g_fd650_status = (hi_u32)g_fd650_status & (~(hi_u32)KEYLED_STATUS_LEDON);
    HI_INFO_KEYLED("stop IsLedOpen=%d\n", ((hi_u32)g_fd650_status & (hi_u32)KEYLED_STATUS_LEDON) ? 1 : 0);
    g_display_switch = DISPLAY_OFF;

    ret = drv_keyled_deinit_fd650();
    if (ret != HI_SUCCESS) {
        HI_ERR_KEYLED("drv_keyled_init_fd650 failed! \n");
    }

    return HI_SUCCESS;
}

hi_s32 drv_keyled_led_display_fd650(hi_u32 value)
{
    hi_u32 i;

    if (!((hi_u32)g_fd650_status & (hi_u32)KEYLED_STATUS_LEDON)) {
        return HI_FAILURE;
    }
    del_timer_sync(&g_dotflash_timer);

    for (i = 0; i < DISPLAY_SEG_CURR; i++) {
        if (i == 3) {
            g_disp_buf_arr[i * 2] &= 0x80;
            g_disp_buf_arr[i * 2] += ((hi_u32)value >> (8 * i)) & 0xff;
        } else {
            g_disp_buf_arr[i * 2] = ((hi_u32)value >> (8 * i)) & 0xff;
        }
    }

    if (g_display_switch == DISPLAY_OFF) {
        keyled_fd650_display_addr_inc(DISPLAY_REG_START_ADDR, g_disp_buf_arr, DISPLAY_REG_NUM, CMD_DIP_OFF);
    } else {
        keyled_fd650_display_addr_inc(DISPLAY_REG_START_ADDR, g_disp_buf_arr, DISPLAY_REG_NUM, CMD_DIP_ON);
    }

    return HI_SUCCESS;
}

hi_s32 drv_keyled_led_display_time_fd650(hi_keyled_time led_time)
{
    if (!((hi_u32)g_fd650_status & (hi_u32)KEYLED_STATUS_LEDON)) {
        return HI_FAILURE;
    }
    del_timer_sync(&g_dotflash_timer);
    g_dotflash_timer.expires = jiffies + msecs_to_jiffies(570);
    add_timer(&g_dotflash_timer);

    g_dot_flag = HI_TRUE;

    g_keyled_time = led_time;

    keyled_fd650_display_time(g_keyled_time, g_dot_flag);
    return HI_SUCCESS;
}

hi_s32 drv_keyled_led_set_flash_pin_fd650(hi_keyled_light led_flash_pin)
{
    return HI_SUCCESS;
}

hi_s32 drv_keyled_led_set_flash_freq_fd650(hi_keyled_level led_flash_level)
{
    return HI_SUCCESS;
}

hi_s32 drv_keyled_get_proc_info_fd650(keyled_proc_info *proc_info)
{
    proc_info->key_buf_size = g_keyled_dev.buf_len;
    proc_info->key_buf_head = g_keyled_dev.head;
    proc_info->key_buf_tail = g_keyled_dev.tail;
    proc_info->key_come_num = g_keyled_dev.key_come;
    proc_info->key_read_num = g_keyled_dev.key_read;
    proc_info->repeat_key_time_ms = g_keyled_dev.repeat_key_delay_time;
    proc_info->is_repeat_key_enable = g_keyled_dev.enable_repeat_key;
    proc_info->is_up_key_enable = g_keyled_dev.enable_key_up;
    proc_info->block_time = g_keyled_dev.block_time;
    proc_info->flash_pin = g_keyled_dev.flash_pin;
    proc_info->flash_level = g_keyled_dev.flash_level;
    proc_info->key_buf_tail= g_keyled_dev.tail;

    return HI_SUCCESS;
}


hi_s32 drv_keyled_suspend_fd650(hi_void)
{
    return 0;
}

hi_s32 drv_keyled_resume_fd650(hi_void)
{
    return 0;
}

hi_s32 drv_keyled_led_set_lock_indicator_fd650(hi_bool lock_flag)
{
    keyled_fd650_display_lockled(lock_flag);
    return HI_SUCCESS;
}
