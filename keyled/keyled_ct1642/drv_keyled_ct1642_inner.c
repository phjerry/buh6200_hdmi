/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
 * Description:
 */

#include <linux/vmalloc.h>
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

#include "hi_type.h"
#include "hi_drv_dev.h"
#include "hi_drv_osal.h"
#include "hi_drv_proc.h"
#include "hi_drv_sys.h"
#include "hi_unf_keyled.h"
#include "hi_drv_keyled.h"
#include "drv_keyled_ioctl.h"
#include "hi_errno.h"
#include "drv_keyled_ct1642_inner.h"

#define DEFAULT_REP_KEY_TIME   200

#ifndef HI_REG_READ
#define HI_REG_READ(addr,result)  ((result) = *(volatile unsigned int *)(addr))
#endif

#ifndef HI_REG_WRITE
#define HI_REG_WRITE(addr,result)  (*(volatile unsigned int *)(addr) = (result))
#endif

static struct semaphore sem_keyled_std;

static struct  timer_list g_sleep_timer;
static struct  timer_list g_dotflash_timer;

static hi_u32  g_key_open_cnt = 0;
static hi_u32  g_led_open_cnt = 0;
static hi_bool is_key_pressed = HI_FALSE;
static hi_u8 *g_keyled_base_addr = HI_NULL_PTR;
static hi_s32 g_keyled_timeout_cnt = 0;
static hi_handle g_keyled_irq;


/* hi_u32  LedDigDisCode[] = {0x03,0x9F,0x25,0x0d,0x99,0x49,0x41,0x1f,0x01,0x09}; */
/* led display time when arm is stand by */

/* hi_u32  LedDigDisDot[] = {0x02,0x9E,0x24,0x0c,0x98,0x48,0x40,0x1e,0x00,0x08}; */
/* led display time when arm is stand by */

static hi_u32 g_led_dig_dis_dot_ct1642[]  = { 0xfd, 0x61, 0xdb, 0xf3, 0x67, 0xb7, 0xbf, 0xe1, 0xff, 0xf7 };
static hi_u32 g_led_dig_dis_code_ct1642[] = { 0xfc, 0x60, 0xda, 0xf2, 0x66, 0xb6, 0xbe, 0xe0, 0xfe, 0xf6 };


static keyled_dev_attr g_keyled_dev;

static hi_bool g_dot_flag = HI_FALSE;

static hi_keyled_time g_keyled_time = {0};

static wait_queue_head_t g_wait_keyleds;

static keyled_suspend_save g_suspend_save_val;

static hi_void keyled_display_time(hi_keyled_time keyled_time, hi_bool dotflag);

/* config led clock's frequency */
static hi_void keyled_config_clktim(hi_u32 clk)
{
    hi_u32 reg_value = 0;
    hi_s32 clk_tim = 0;
    hi_u32 sys_tim = 0;

    if (clk == 0) {
        HI_ERR_KEYLED("prompt: clk can't be 0, set to default %d\n", CLKTIM_DEFAULT);
        clk = CLKTIM_DEFAULT;
    }

    HI_REG_READ(g_keyled_base_addr + LEDC_CONTROL, reg_value);
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value & ~LEDC_ENABLE);

    HI_REG_READ(g_keyled_base_addr + LEDC_SYSTIM, sys_tim);

    clk_tim = (SYSTEM_CLK * 1000) / (2 * (sys_tim + 1) * clk) - 1;
    if (clk_tim < 0) {
        clk_tim = 0;
    }

    HI_REG_WRITE(g_keyled_base_addr + LEDC_CLKTIM, clk_tim);

    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value);

    return;
}

/* config led refresh frequncy: example fre = 200Hz */
static hi_void keyled_config_fretim(hi_u32 fre)
{
    hi_u32 reg_value = 0;
    hi_s32 fretim = 0;
    hi_s32 clk_tim = 0;
    hi_u32 sys_tim = 0;

    if (fre == 0) {
        HI_ERR_KEYLED("prompt: fre can't be 0, set to default %d\n", FRETIM_DEFAULT);
        fre = FRETIM_DEFAULT;
    }

    HI_REG_READ(g_keyled_base_addr + LEDC_CONTROL, reg_value);
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value & ~LEDC_ENABLE);

    HI_REG_READ(g_keyled_base_addr + LEDC_CLKTIM, clk_tim);
    HI_REG_READ(g_keyled_base_addr + LEDC_SYSTIM, sys_tim);

    fretim = (SYSTEM_CLK * 1000 * 1000) / (fre * 16 * 40 * (clk_tim + 1) * (sys_tim + 1) * 5) - 1;
    if (fretim < 0) {
        fretim = 0;
    }

    HI_REG_WRITE(g_keyled_base_addr + LEDC_FRETIM, fretim);
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value);

    return;
}

/* config keypad scanning frequency: example freq= 40Hz */
static hi_void keyled_config_keytim(hi_u32 freq)
{
    hi_u32 reg_value = 0;
    hi_s32 fretim = 0;
    hi_s32 clk_tim = 0;
    hi_s32 keytim = 0;
    hi_u32 sys_tim = 0;

    if (freq == 0) {
        HI_ERR_KEYLED("prompt: freq can't be 0, set to default %d\n", KEY_DEFAULT);
        freq = KEY_DEFAULT;
    }

    HI_REG_READ(g_keyled_base_addr + LEDC_CONTROL, reg_value);
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value & ~LEDC_ENABLE);

    HI_REG_READ(g_keyled_base_addr + LEDC_SYSTIM, sys_tim);
    HI_REG_READ(g_keyled_base_addr + LEDC_CLKTIM, clk_tim);
    HI_REG_READ(g_keyled_base_addr + LEDC_FRETIM, fretim);

    keytim = (SYSTEM_CLK * 1000 * 1000) / ((fretim + 1) * (sys_tim + 1) * 16 * 40 * 5 * 8 * (clk_tim + 1) * freq) - 1;
    if (keytim < 0) {
        keytim = 0;
    }
    HI_INFO_KEYLED("keytim=%d, fretim=%d, clk_tim=%d, freq= %d, SYSTEM_CLK=%d/n",
                   keytim, fretim, clk_tim, freq, SYSTEM_CLK);

    HI_REG_WRITE(g_keyled_base_addr + LEDC_KEYTIM, keytim);

    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value);

    return;
}

/* config led flash frequency: example flash = 40Hz */
static hi_void keyled_config_flashtim(hi_u32 flash)
{
    hi_u32 reg_value = 0;
    hi_u32 flashtim = 0;

    HI_REG_READ(g_keyled_base_addr + LEDC_CONTROL, reg_value);
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value & ~LEDC_ENABLE);

    flashtim = 62 - (flash - 1) * 15;

    HI_REG_WRITE(g_keyled_base_addr + LEDC_FLASHTIM, flashtim);
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value);

    return;
}

/* disable keypad  only, not affect other hardware para */
static hi_void keyled_disable_key(hi_void)
{
    hi_u32 reg_value = 0;

    HI_REG_READ(g_keyled_base_addr + LEDC_CONTROL, reg_value);
    reg_value &= ((~KEYPAD_ENABLE) & (~LEDC_ENABLE));
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value);

    msleep(1);
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, (reg_value & (~LED_ENABLE)));

    return;
}

/* enable keypad , at the same time enabled ledc */
static hi_void keyled_enable_key(hi_void)
{
    hi_u32 reg_value = 0;

    HI_REG_READ(g_keyled_base_addr + LEDC_CONTROL, reg_value);
    reg_value |= (KEYPAD_ENABLE | KEYPAD_PRESS_INTR |
                  KEYPAD_RELEASE_INTR | LEDC_ENABLE | LED_ENABLE);
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value);

    return;
}

/* config led type and key type */
static hi_void keyled_config_type(hi_u32 led_type, hi_u32 key_type)
{
    hi_u32 reg_value = 0;

    if (led_type == HI_KEYLED_ANODE) {
        reg_value |= LEDC_ANODE;
    } else {
        reg_value &= ~LEDC_ANODE;
    }

    if (key_type == KEY_TYPE_CT1642) {
        reg_value |= LEDC_KEY_TYPE;
    } else {
        reg_value &= ~LEDC_KEY_TYPE;
    }

    reg_value |= KEY_SCAN_MODE;
    reg_value |= KEY_SCAN_LEVEL;

    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONFIG, reg_value);

    return;
}

/* enable ledc display */
static hi_void keyled_enable_ledc(hi_void)
{
    hi_u32 reg_value = 0;

    HI_REG_READ(g_keyled_base_addr + LEDC_CONTROL, reg_value);
    reg_value |= (LEDC_ENABLE | LED_ENABLE);
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value);

    return;
}

/* disable ledc display */
static hi_void keyled_disable_ledc(hi_void)
{
    hi_u32 reg_value = 0;

    HI_REG_READ(g_keyled_base_addr + LEDC_CONTROL, reg_value);
    reg_value &= ~LEDC_ENABLE;
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value);

    msleep(1);
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, (reg_value & (~LED_ENABLE)));

    return;
}

/* get key value from ledc hardware */
static hi_u32 keyled_get_key(hi_void)
{
    hi_u32 key = 0;

    if (g_keyled_base_addr == HI_NULL) {
        return HI_FAILURE;
    }

    HI_REG_READ(g_keyled_base_addr + LEDC_KEYDATA, key);

    switch (key) {
        case KEY_7_PRESS:
        case KEY_6_PRESS:
        case KEY_5_PRESS:
        case KEY_4_PRESS:
        case KEY_3_PRESS:
        case KEY_2_PRESS:
        case KEY_1_PRESS:
        case KEY_0_PRESS:
        case KEY_76comb_PRESS:
        case KEY_74comb_PRESS:
        case KEY_72comb_PRESS:
            return (key);
        default:
            return (KEYPAD_ERROR);
    }
}

/* if one key pressed and there is no intr reported in a fixed time, timeout_handler creat a holded key */
static hi_void keyled_timeout_handler(unsigned long data)
{
    hi_u32 key = 0;

    if (g_keyled_timeout_cnt > 10) {
        g_keyled_timeout_cnt = 0;
        key = keyled_get_key();
        if (KEYPAD_ERROR == (key & 0xffffffff)) {
            if (is_key_pressed == HI_FALSE) {
                wake_up_interruptible(&g_wait_keyleds);
                return;
            }
            if (g_keyled_dev.enable_key_up) {
                KEYLED_BUF_HEAD = KEYLED_BUF_LAST;
                KEYLED_BUF_HEAD.keyled_state = KEY_RELEASE;
                g_keyled_dev.head = KEYLED_INC_BUF(g_keyled_dev.head, g_keyled_dev.buf_len);
                g_keyled_dev.key_come++;

                wake_up_interruptible(&g_wait_keyleds);
            }
        } else {
            mod_timer(&g_sleep_timer, jiffies + msecs_to_jiffies(g_keyled_dev.repeat_key_delay_time));

            KEYLED_BUF_HEAD = KEYLED_BUF_LAST;
            KEYLED_BUF_HEAD.keyled_state = KEY_HOLD;
            g_keyled_dev.head = KEYLED_INC_BUF(g_keyled_dev.head, g_keyled_dev.buf_len);
            g_keyled_dev.key_come++;

            HI_INFO_KEYLED("key hold:head=%d, tail=%d\n", g_keyled_dev.head, g_keyled_dev.tail);

            wake_up_interruptible(&g_wait_keyleds);
        }
    } else {
        if (is_key_pressed == HI_FALSE) {
            wake_up_interruptible(&g_wait_keyleds);
            return;
        }

        mod_timer(&g_sleep_timer, jiffies + msecs_to_jiffies(g_keyled_dev.repeat_key_delay_time));

        KEYLED_BUF_HEAD = KEYLED_BUF_LAST;
        KEYLED_BUF_HEAD.keyled_state = KEY_HOLD;
        g_keyled_dev.head = KEYLED_INC_BUF(g_keyled_dev.head, g_keyled_dev.buf_len);
        g_keyled_dev.key_come++;

        HI_INFO_KEYLED("key hold:head=%d, tail=%d\n", g_keyled_dev.head, g_keyled_dev.tail);

        wake_up_interruptible(&g_wait_keyleds);
    }

    return;
}

static hi_void keyled_dotflash_handler(hi_size_t data)
{
    mod_timer(&g_dotflash_timer, jiffies + msecs_to_jiffies(DOT_FLASH_TIME));

    if (g_dot_flag) {
        g_dot_flag = HI_FALSE;
    } else {
        g_dot_flag = HI_TRUE;
    }

    keyled_display_time(g_keyled_time, g_dot_flag);

    return;
}

static hi_void keyled_interrupt_mask(hi_void)
{
    hi_u32 reg_value = 0;

    HI_REG_READ(g_keyled_base_addr + LEDC_CONTROL, reg_value);
    reg_value &= ~0x3;
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value);

    return;
}

static hi_void keyled_interrupt_unmask(hi_void)
{
    hi_u32 reg_value = 0;

    HI_REG_READ(g_keyled_base_addr + LEDC_CONTROL, reg_value);
    reg_value |= 0x3;
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value);

    return;
}


/* process release intr and press intr */
static irqreturn_t keyled_interrupt_handler(int irq, hi_void *dev_id)
{
    hi_u32 reg_value = 0;
    hi_u32 key = 0;

    keyled_interrupt_mask();

    HI_REG_READ(g_keyled_base_addr + LEDC_KEYINT, reg_value);

    if (reg_value & KEYPAD_PRESS_INTR) {
        HI_REG_WRITE(g_keyled_base_addr + LEDC_KEYINT, reg_value & KEYPAD_PRESS_INTR);

        key = keyled_get_key();

        if (KEYPAD_ERROR != key) {
            if (g_keyled_dev.enable_repeat_key) {
                mod_timer(&g_sleep_timer, jiffies + msecs_to_jiffies(g_keyled_dev.repeat_key_delay_time));
                g_keyled_timeout_cnt = 0;
            }

            KEYLED_BUF_HEAD.keyled_code  = key;
            KEYLED_BUF_HEAD.keyled_state = KEY_PRESS;
            is_key_pressed = HI_TRUE;
            g_keyled_dev.head = KEYLED_INC_BUF(g_keyled_dev.head, g_keyled_dev.buf_len);
            g_keyled_dev.key_come++;

            HI_INFO_KEYLED("key press:head=%d, tail=%d, key=0x%x\n", g_keyled_dev.head, g_keyled_dev.tail, key);

            wake_up_interruptible(&g_wait_keyleds);
        }
    } else if (reg_value & KEYPAD_RELEASE_INTR) {
        HI_REG_WRITE(g_keyled_base_addr + LEDC_KEYINT, reg_value & KEYPAD_RELEASE_INTR);

        HI_REG_READ(g_keyled_base_addr + LEDC_KEYDATA, key);
        if ((((~key)) & KEYLED_BUF_LAST.keyled_code) == KEYLED_BUF_LAST.keyled_code) {
            is_key_pressed = HI_FALSE;
            if (g_keyled_dev.enable_key_up && (KEYLED_BUF_LAST.keyled_state != KEY_RELEASE)) {
                KEYLED_BUF_HEAD = KEYLED_BUF_LAST;
                KEYLED_BUF_HEAD.keyled_state = KEY_RELEASE;
                g_keyled_dev.head = KEYLED_INC_BUF(g_keyled_dev.head, g_keyled_dev.buf_len);
                g_keyled_dev.key_come++;

                HI_INFO_KEYLED("key up:head=%d, tail=%d, key=0x%x\n", g_keyled_dev.head, g_keyled_dev.tail, key);

                wake_up_interruptible(&g_wait_keyleds);
            }
        }
    } else {
        HI_REG_WRITE(g_keyled_base_addr + LEDC_KEYINT, reg_value & (KEYPAD_RELEASE_INTR | KEYPAD_PRESS_INTR));

        is_key_pressed = HI_FALSE;
    }

    keyled_interrupt_unmask();

    return IRQ_HANDLED;
}

static hi_void keyled_clear_keybuf_func(hi_void)
{
    /* set tail to head is can also clear keyled buf and it will keep the value in BUF_LAST which will be usefull after wake up by power */
    g_keyled_dev.tail = g_keyled_dev.head;
    g_keyled_dev.key_come = 0;
    g_keyled_dev.key_read = 0;

    /* when wake up from low power mode, ir clear buf must do more work,as follow */
    del_timer_sync(&g_sleep_timer);
    g_keyled_timeout_cnt = 0;

    return;
}

/* led display data */
static hi_void keyled_display(hi_u32 low_data)
{
    hi_u32 data_value = 0;
    hi_u32 reg_value = 0;

    HI_REG_READ(g_keyled_base_addr + LEDC_CONTROL, reg_value);
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value & ~LEDC_ENABLE);

    data_value = low_data & 0x00ff;
    HI_REG_WRITE(g_keyled_base_addr + LEDC_DATA1, data_value);
    g_suspend_save_val.led_data[0] = data_value;

    data_value = (low_data & 0x00ff00) >> 8;
    HI_REG_WRITE(g_keyled_base_addr + LEDC_DATA2, data_value);
    g_suspend_save_val.led_data[1] = data_value;

    data_value = (low_data & 0x00ff0000) >> 16;
    HI_REG_WRITE(g_keyled_base_addr + LEDC_DATA3, data_value);
    g_suspend_save_val.led_data[2] = data_value;

    data_value = (low_data & 0xff000000) >> 24;
    HI_REG_WRITE(g_keyled_base_addr + LEDC_DATA4, data_value);
    g_suspend_save_val.led_data[3] = data_value;

    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value);

    g_suspend_save_val.is_display_normal_data = HI_TRUE;
    g_suspend_save_val.is_display_time_data = HI_FALSE;

    return;
}

/* led display time */
static hi_void keyled_display_time(hi_keyled_time keyled_time, hi_bool dotflag)
{
    hi_u32 data_value = 0;
    hi_u32 index = 0;
    hi_u32 reg_value = 0;

    HI_INFO_KEYLED("disp time:%d:%d\n", keyled_time.hour, keyled_time.minute);

    HI_REG_READ(g_keyled_base_addr + LEDC_CONTROL, reg_value);
    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value & ~LEDC_ENABLE);

    index = keyled_time.hour / 10;
    if (dotflag == HI_FALSE) {
        data_value = g_led_dig_dis_code_ct1642[index];
    } else {
        data_value = g_led_dig_dis_dot_ct1642[index];
    }
    HI_REG_WRITE(g_keyled_base_addr + LEDC_DATA1, data_value);

    index = keyled_time.hour % 10;
    if (dotflag == HI_FALSE) {
        data_value = g_led_dig_dis_code_ct1642[index];
    } else {
        data_value = g_led_dig_dis_dot_ct1642[index];
    }
    HI_REG_WRITE(g_keyled_base_addr + LEDC_DATA2, data_value);

    index = keyled_time.minute / 10;
    data_value = g_led_dig_dis_code_ct1642[index];
    HI_REG_WRITE(g_keyled_base_addr + LEDC_DATA3, data_value);

    index = keyled_time.minute % 10;
    data_value = g_led_dig_dis_code_ct1642[index];
    HI_REG_WRITE(g_keyled_base_addr + LEDC_DATA4, data_value);

    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value);

    g_suspend_save_val.is_display_normal_data = HI_FALSE;
    g_suspend_save_val.is_display_time_data = HI_TRUE;
    g_suspend_save_val.time_data = keyled_time;
    g_suspend_save_val.dotflag = dotflag;

    return;
}


/* set led light pin  */
static hi_s32 keyled_set_flashpin(hi_keyled_light pin)
{
    hi_u32 reg_value = 0;

    HI_REG_READ(g_keyled_base_addr + LEDC_CONTROL, reg_value);

    reg_value &= ~FLASH_ENABLE_4;

    switch (pin) {
        case KEYLED_LIGHT_1:
            reg_value |= LEDC_FLASH_1;
            break;
        case KEYLED_LIGHT_2:
            reg_value |= LEDC_FLASH_2;
            break;
        case KEYLED_LIGHT_3:
            reg_value |= LEDC_FLASH_3;
            break;
        case KEYLED_LIGHT_4:
            reg_value |= LEDC_FLASH_4;
            break;
        case KEYLED_LIGHT_ALL:
            reg_value |= FLASH_ENABLE_4;
            break;
        case KEYLED_LIGHT_NONE:
            reg_value &= ~FLASH_ENABLE_4;
            break;
        default:
            return HI_FAILURE;
    }

    HI_REG_WRITE(g_keyled_base_addr + LEDC_CONTROL, reg_value);

    g_keyled_dev.flash_pin = pin;

    return HI_SUCCESS;
}

/* set led flash level */
static hi_s32 keyled_set_flash(hi_keyled_level level)
{
    hi_u32 flash_freq = 0;

    switch (level) {
        case KEYLED_LEVEL_1:
            flash_freq = FLASH_LEVEL1;
            break;
        case KEYLED_LEVEL_2:
            flash_freq = FLASH_LEVEL2;
            break;
        case KEYLED_LEVEL_3:
            flash_freq = FLASH_LEVEL3;
            break;
        case KEYLED_LEVEL_4:
            flash_freq = FLASH_LEVEL4;
            break;
        case KEYLED_LEVEL_5:
            flash_freq = FLASH_LEVEL5;
            break;
        default:
            return HI_FAILURE;
    }

    keyled_config_flashtim(flash_freq);
    g_keyled_dev.flash_level = level;

    return HI_SUCCESS;
}

/*********************************************************************************************/
/*Device Manage Layer Interface*/

hi_s32 keyled_init_ct1642(hi_void)
{
    hi_s32 err = 0;

    if (g_keyled_base_addr != HI_NULL_PTR) {
        return HI_SUCCESS;
    }

    HI_OSAL_INIT_MUTEX(&sem_keyled_std);

    g_keyled_dev.buf_len = KEYBUF_LEN;

    /* remap keyled register to memory */
    g_keyled_base_addr = (hi_u8 *)ioremap_nocache(KEYLED_PHY_ADDR, KEYLED_ADDR_SIZE);
    if (g_keyled_base_addr == HI_NULL) {
        HI_ERR_KEYLED("keyled ioremap_nocache err! \n");
        return -EFAULT ;
    }

    init_waitqueue_head(&g_wait_keyleds);

    /* set timer for keyled */
    g_sleep_timer.function = keyled_timeout_handler;
    init_timer(&g_sleep_timer);

    /* set timer for keyled */
    g_dotflash_timer.function = keyled_dotflash_handler;
    init_timer(&g_dotflash_timer);

    /* init keyled data structure and members */
    keyled_clear_keybuf_func();

    /* registe keyled irq handle */
    err = hi_drv_osal_request_irq(KEYLED_IRQ, keyled_interrupt_handler, IRQF_SHARED, "keyled_ct1642", (hi_void *)&g_keyled_irq);
    if (err != 0) {
        HI_ERR_KEYLED("keyled request irq failed\n");
        return -EFAULT;
    } else {
        if (HI_SUCCESS != hi_drv_sys_set_irq_affinity(HI_ID_KEYLED, KEYLED_IRQ, "keyled_ct1642")) {
            HI_ERR_KEYLED("hi_drv_sys_set_irq_affinity failed.\n");
        }
    }

    /* disable keyled before open device */
    keyled_disable_key();

    /* clear LED DATA register */
    HI_REG_WRITE(g_keyled_base_addr + LEDC_DATA1, 0x0);
    HI_REG_WRITE(g_keyled_base_addr + LEDC_DATA2, 0x0);
    HI_REG_WRITE(g_keyled_base_addr + LEDC_DATA3, 0x0);
    HI_REG_WRITE(g_keyled_base_addr + LEDC_DATA4, 0x0);

    /*config system frequency divide to 0x6 */
    HI_REG_WRITE(g_keyled_base_addr + LEDC_SYSTIM, SYSTIM_DEFAULT);

    keyled_config_clktim(CLKTIM_DEFAULT);
    keyled_config_fretim(FRETIM_DEFAULT); /* todo:This value may need to change */
    keyled_config_keytim(KEY_DEFAULT);
    keyled_config_type(HI_KEYLED_CATHODE, KEY_TYPE_CT1642);


    g_keyled_dev.keyled_mode.key_scan_mode = HI_KEYLED_1MUL8;
    g_keyled_dev.keyled_mode.led_num = HI_KEYLED_4LED;

    HI_INFO_KEYLED(" Inint ok\n");

    return HI_SUCCESS;
}

hi_s32 keyled_deinit_ct1642(hi_void)
{
    if (g_keyled_base_addr == HI_NULL_PTR) {
        return HI_SUCCESS;
    }

    if ((g_key_open_cnt > 0) || (g_led_open_cnt > 0)) {
        return HI_SUCCESS;
    }

    /* release irq */
    hi_drv_osal_free_irq(KEYLED_IRQ, "keyled_ct1642", (hi_void *)&g_keyled_irq);
    g_keyled_base_addr = HI_NULL_PTR;

    HI_INFO_KEYLED(" Deint ok\n");

    return HI_SUCCESS;
}


hi_s32 keyled_key_open_ct1642(hi_void)
{
    hi_u32 reg_val = 0;

    HI_INFO_KEYLED(" start g_key_open_cnt=%d\n", g_key_open_cnt);
    keyled_init_ct1642();

    /* avoid device reopen */
    if (g_key_open_cnt > 0) {
        return HI_SUCCESS;
    }

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }

    g_key_open_cnt++;

    g_keyled_dev.head = 0;
    g_keyled_dev.tail = 0;
    g_keyled_dev.buf_len = KEYBUF_LEN;
    g_keyled_dev.key_come = 0;
    g_keyled_dev.key_read = 0;
    g_keyled_dev.enable_repeat_key = 1;
    g_keyled_dev.block_time = DEFAULT_BLOCKTIME;
    g_keyled_dev.repeat_key_delay_time = DEFAULT_REP_KEY_TIME;
    g_keyled_dev.enable_key_up = 1;
    g_dot_flag = HI_FALSE;
    g_keyled_timeout_cnt = 0;

    /* clear keyled interrupt status */
    HI_REG_READ(g_keyled_base_addr + LEDC_KEYINT, reg_val);
    reg_val |= 0x3;
    HI_REG_WRITE(g_keyled_base_addr + LEDC_KEYINT, reg_val);
    keyled_enable_key();

    HI_INFO_KEYLED(" ok\n");
    up(&sem_keyled_std);

    return HI_SUCCESS;
}

hi_s32 keyled_key_close_ct1642(hi_void)
{
    HI_INFO_KEYLED(" start g_key_open_cnt=%d\n", g_key_open_cnt);

    if (g_key_open_cnt == 0) {
        return HI_SUCCESS;
    }

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }

    del_timer_sync(&g_sleep_timer);

    /* clear inttrupt status */
    HI_REG_WRITE(g_keyled_base_addr + LEDC_KEYINT, 0x3);
    keyled_disable_key();

    g_key_open_cnt = 0;

    keyled_deinit_ct1642();
    HI_INFO_KEYLED(" ok\n");
    up(&sem_keyled_std);

    return HI_SUCCESS;
}

hi_s32 keyled_key_reset_ct1642(hi_void)
{
    if (g_key_open_cnt == 0) {
        HI_ERR_KEYLED("Key not open\n");
        return HI_FAILURE;
    }

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }
    keyled_clear_keybuf_func();

    up(&sem_keyled_std);
    return HI_SUCCESS;
}

hi_s32 keyled_key_get_value_ct1642(hi_u32 *press_status_ptr, hi_u32 *key_id_ptr)
{
    keyled_key_info keyled_to_user = {0};
    hi_s32 err = 0;

    if (g_key_open_cnt == 0) {
        HI_ERR_KEYLED("Key not open\n");
        return HI_FAILURE;
    }

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }
    up(&sem_keyled_std);

    if ((g_keyled_dev.head) == (g_keyled_dev.tail)) {
        if (g_keyled_dev.block_time == 0) {
            return HI_ERR_KEYLED_NO_NEW_KEY;
        }

        err = wait_event_interruptible_timeout(g_wait_keyleds, (g_keyled_dev.head != g_keyled_dev.tail),
                                               (long)(g_keyled_dev.block_time * HZ / 1000));
        if (err <= 0) {/*not wake up by received key*/
            return HI_ERR_KEYLED_TIMEOUT;
        }
    }

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }

    if (g_keyled_dev.head != g_keyled_dev.tail) {
        keyled_to_user  = KEYLED_BUF_TAIL;
        g_keyled_dev.tail = KEYLED_INC_BUF(g_keyled_dev.tail, g_keyled_dev.buf_len);
        g_keyled_dev.key_read++;

        *press_status_ptr = keyled_to_user.keyled_state;
        switch (keyled_to_user.keyled_code) {
            case KEY_7_PRESS:
                *key_id_ptr = 8;
                break;
            case KEY_6_PRESS:
                *key_id_ptr = 6;
                break;
            case KEY_5_PRESS:
                *key_id_ptr = 5;
                break;
            case KEY_4_PRESS:
                *key_id_ptr = 4;
                break;
            case KEY_3_PRESS:
                *key_id_ptr = 3;
                break;
            case KEY_2_PRESS:
                *key_id_ptr = 2;
                break;
            case KEY_1_PRESS:
                *key_id_ptr = 1;
                break;
            case KEY_0_PRESS:
                *key_id_ptr = 0;
                break;
            case KEY_76comb_PRESS:
                *key_id_ptr = 13;
                break;
            case KEY_74comb_PRESS:
                *key_id_ptr = 11;
                break;
            case KEY_72comb_PRESS:
                *key_id_ptr = 9;
                break;
            default:
                up(&sem_keyled_std);

                return (KEYPAD_ERROR);
        }

        HI_INFO_KEYLED("get a key:head=%d, tail=%d, key=0x%x, status=%d \n",
                       g_keyled_dev.head, g_keyled_dev.tail, *key_id_ptr, *press_status_ptr);
        up(&sem_keyled_std);
        return HI_SUCCESS;
    } else {
        up(&sem_keyled_std);
        return HI_ERR_KEYLED_NO_NEW_KEY;
    }
}

hi_s32 keyled_key_set_block_time_ct1642(hi_u32 block_time_ms)
{
    if (g_key_open_cnt == 0) {
        HI_ERR_KEYLED("Key not open\n");
        return HI_FAILURE;
    }

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }
    g_keyled_dev.block_time = block_time_ms;

    up(&sem_keyled_std);

    return HI_SUCCESS;
}


hi_s32 keyled_key_set_repeat_time_ct1642(hi_u32 repeat_time_ms)
{
    if (g_key_open_cnt == 0) {
        HI_ERR_KEYLED("Key not open\n");
        return HI_FAILURE;
    }

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }
    g_keyled_dev.repeat_key_delay_time = repeat_time_ms;

    up(&sem_keyled_std);

    return HI_SUCCESS;
}

hi_s32 keyled_key_enable_repeat_key_ct1642(hi_u32 enable_repkey)
{
    if (g_key_open_cnt == 0) {
        HI_ERR_KEYLED("Key not open\n");
        return HI_FAILURE;
    }

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }
    g_keyled_dev.enable_repeat_key = enable_repkey;

    up(&sem_keyled_std);

    return HI_SUCCESS;
}

hi_s32 keyled_key_enable_key_up_ct1642(hi_u32 enable_keyup)
{
    if (g_key_open_cnt == 0) {
        HI_ERR_KEYLED("Key not open\n");
        return HI_FAILURE;
    }
    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }
    g_keyled_dev.enable_key_up = enable_keyup;

    up(&sem_keyled_std);

    return HI_SUCCESS;
}

hi_s32 keyled_led_open_ct1642(hi_void)
{
    HI_INFO_KEYLED(" start g_led_open_cnt=%d\n", g_led_open_cnt);

    keyled_init_ct1642();

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }

    /* avoid device reopen */
    if (g_led_open_cnt > 0) {
        HI_ERR_KEYLED("reopen led, g_led_open_cnt=%d\n", g_led_open_cnt);
        up(&sem_keyled_std);
        return HI_SUCCESS;
    }
    g_led_open_cnt++;

    keyled_enable_ledc();
    HI_INFO_KEYLED(" ok\n");

    up(&sem_keyled_std);

    return HI_SUCCESS;
}

hi_s32 keyled_led_close_ct1642(hi_void)
{
    hi_s32 ret;

    HI_INFO_KEYLED(" start g_led_open_cnt=%d\n", g_led_open_cnt);

    if (g_led_open_cnt == 0) {
        return HI_SUCCESS;
    }

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }

    del_timer_sync(&g_dotflash_timer);

    keyled_disable_ledc();
    ret = keyled_set_flashpin(KEYLED_LIGHT_NONE);
    if (ret != HI_SUCCESS) {
        HI_ERR_KEYLED("keyled_set_flashpin failed!\n");
        return HI_FAILURE;
    }

    g_led_open_cnt = 0;

    keyled_deinit_ct1642();
    HI_INFO_KEYLED(" ok\n");

    up(&sem_keyled_std);

    return HI_SUCCESS;
}

hi_s32 keyled_led_display_ct1642(hi_u32 code_value)
{
    if (g_led_open_cnt == 0) {
        HI_ERR_KEYLED("Led not open\n");
        return HI_FAILURE;
    }

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }
    del_timer_sync(&g_dotflash_timer);
    keyled_display(code_value);
    up(&sem_keyled_std);

    return HI_SUCCESS;
}

hi_s32 keyled_led_display_time_ct1642(hi_keyled_time display_time)
{
    if (g_led_open_cnt == 0) {
        HI_ERR_KEYLED("Led not open\n");
        return HI_FAILURE;
    }

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }
    del_timer_sync(&g_dotflash_timer);
    g_dotflash_timer.expires = jiffies + msecs_to_jiffies(DOT_FLASH_TIME);
    add_timer(&g_dotflash_timer);

    g_dot_flag = HI_TRUE;
    keyled_display_time(display_time, g_dot_flag);
    g_keyled_time = display_time;

    up(&sem_keyled_std);

    return HI_SUCCESS;
}

hi_s32 keyled_led_set_flash_pin_ct1642(hi_keyled_light pin)
{
    if (g_led_open_cnt == 0) {
        HI_ERR_KEYLED("Led not open\n");
        return HI_FAILURE;
    }

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }
    keyled_set_flashpin(pin);

    up(&sem_keyled_std);

    return HI_SUCCESS;
}

hi_s32 keyled_led_set_flash_freq_ct1642(hi_keyled_level level)
{
    if (g_led_open_cnt == 0) {
        HI_ERR_KEYLED("Led not open\n");
        return HI_FAILURE;
    }

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }

    keyled_set_flash(level);
    up(&sem_keyled_std);

    return HI_SUCCESS;
}

hi_s32 keyled_get_proc_info_ct1642(keyled_proc_info *proc_info)
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
    proc_info->key_buf_tail = g_keyled_dev.tail;

    return HI_SUCCESS;
}

hi_s32 keyled_suspend_ct1642(hi_void)
{
    if ((g_led_open_cnt == 0) && (g_key_open_cnt == 0)) {
        return HI_SUCCESS;
    }

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }

    keyled_disable_key();
    keyled_disable_ledc();

    HI_REG_WRITE(g_keyled_base_addr + LEDC_KEYINT, 0x3);
    del_timer_sync(&g_sleep_timer);

    up(&sem_keyled_std);

    return HI_SUCCESS;
}

hi_s32 keyled_resume_ct1642(hi_void)
{
    hi_u32 reg_val = 0;

    if ((g_led_open_cnt == 0) && (g_key_open_cnt == 0)) {
        return HI_SUCCESS;
    }

    if (down_interruptible(&sem_keyled_std)) {
        HI_FATAL_KEYLED("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }
    /* disable keyled before open device */
    keyled_disable_key();
    keyled_config_clktim(CLKTIM_DEFAULT);
    keyled_config_fretim(FRETIM_DEFAULT);
    keyled_config_keytim(KEY_DEFAULT);
    keyled_config_type(HI_KEYLED_CATHODE, KEY_TYPE_CT1642);

    /* clear keyled interrupt status */
    HI_REG_READ(g_keyled_base_addr + LEDC_KEYINT, reg_val);
    reg_val |= 0x3;
    HI_REG_WRITE(g_keyled_base_addr + LEDC_KEYINT, reg_val);

    keyled_clear_keybuf_func();

    HI_INFO_KEYLED("g_keyled_dev.tail = %d  g_keyled_dev.head = %d \n", g_keyled_dev.tail, g_keyled_dev.head);

    keyled_set_flashpin(g_keyled_dev.flash_pin);
    keyled_set_flash(g_keyled_dev.flash_level);

    if (g_suspend_save_val.is_display_time_data == HI_TRUE) {
        keyled_display_time(g_suspend_save_val.time_data, g_suspend_save_val.dotflag);
    } else if (g_suspend_save_val.is_display_normal_data == HI_TRUE) {
        keyled_display(g_suspend_save_val.led_data[0] | g_suspend_save_val.led_data[1] << 8
                       | g_suspend_save_val.led_data[2] << 16 | g_suspend_save_val.led_data[3] << 24);
    } else {
        HI_ERR_KEYLED("Resume Display error. \n");
    }

    keyled_enable_key();
    keyled_enable_ledc();

    up(&sem_keyled_std);

    return HI_SUCCESS;
}


