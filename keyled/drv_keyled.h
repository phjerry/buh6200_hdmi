/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:
 */

#ifndef __KEYLED_H__
#define __KEYLED_H__

#include "hi_type.h"
#include "drv_keyled_ioctl.h"

#define KEYBUF_LEN 8

/* state of the key */
#define KEY_PRESS (0x0)           /* key pressed */
#define KEY_HOLD (0x01)           /* key holden */
#define KEY_RELEASE (0x02)        /* key release */

#define DEFAULT_BLOCKTIME 500   /* 500ms */

typedef enum {
    KEYLED_TYPE_CT1642 = 0x0,   /* keyled type: CT1642 */
    KEYLED_TYPE_FD650,          /* keyled type: FD650 */
    KEYLED_TYPE_GPIOKEY,        /* keyled type: GPIOKEY */
    KEYLED_TYPE_MAX
} hi_keyled_type;

/* blink frequency level of the LED */
typedef enum {
    KEYLED_LEVEL_1 = 0x01,   /* level 1, slowest */
    KEYLED_LEVEL_2,          /* level 2 */
    KEYLED_LEVEL_3,          /* level 3 */
    KEYLED_LEVEL_4,          /* level 4 */
    KEYLED_LEVEL_5,          /* level 5, fastest */
    KEYLED_LEVEL_MAX
} hi_keyled_level;

/* blink sequence of LEDs */
typedef enum {
    KEYLED_LIGHT_1 = 0x01,   /* the first LED blinks. */
    KEYLED_LIGHT_2,          /* the second LED blinks. */
    KEYLED_LIGHT_3,          /* the third LED blinks. */
    KEYLED_LIGHT_4,          /* the fourth LED blinks. */
    KEYLED_LIGHT_ALL,        /* all LEDs blink. */
    KEYLED_LIGHT_NONE,       /* all LEDs do not blink. */
    KEYLED_LIGHT_MAX
} hi_keyled_light;

/* display time of each LED */
typedef struct {
    hi_u32 hour;           /* hour */
    hi_u32 minute;         /* minute */
} hi_keyled_time, *hi_keyled_time_ptr;

typedef enum {
    KEYLED_STATUS_KEYOFF_LEDOFF = 0,
    KEYLED_STATUS_KEYON,
    KEYLED_STATUS_LEDON,
    KEYLED_STATUS_KEYON_LEDON
} keyled_status;

typedef struct {
    hi_u32 key_buf_size;          /* size of  keybuf */
    hi_u32 key_buf_head;          /* head of keybuf */
    hi_u32 key_buf_tail;          /* tail of keybuf */
    hi_u32 key_come_num;          /* the top num of keybuf */
    hi_u32 key_read_num;          /* number that the keybuf can read */
    hi_u32 repeat_key_time_ms;    /* whether it is the time we think it is repeat key */
    hi_u32 is_repeat_key_enable;  /* whether the  function of repeat key  is enable */
    hi_u32 is_up_key_enable;      /* whether the function of key up is enable */
    hi_u32 block_time;
    hi_keyled_light flash_pin;
    hi_keyled_level flash_level;  /* level of the LED twinkle */
    hi_keyled_type keyled_type;
    hi_keyled_time led_time;
    hi_u32 display_code;          /* the number that the LED is displaying */

} keyled_proc_info;


typedef struct {
    hi_u32 keyled_code;
    hi_u32 keyled_state;
} keyled_key_info;

typedef struct {
    hi_u32            head, tail;
    keyled_key_info   buf[KEYBUF_LEN];
    hi_u32            buf_len;
    hi_u32            key_come;
    hi_u32            key_read;

    hi_u32 enable_repeat_key;
    hi_u32 repeat_key_delay_time;

    hi_u32 enable_key_up;
    hi_u32 block_time;
    hi_keyled_mode keyled_mode;
    hi_keyled_light flash_pin;
    hi_keyled_level flash_level;
} keyled_dev_attr;

typedef struct {
    hi_bool is_display_normal_data;
    hi_bool is_display_time_data;
    hi_bool dotflag;
    hi_u8   led_display_num;
    hi_u8   led_data[5];
    hi_keyled_time time_data;
} keyled_suspend_save;


#define KEYLED_BUF_HEAD g_keyled_dev.buf[g_keyled_dev.head]
#define KEYLED_BUF_TAIL g_keyled_dev.buf[g_keyled_dev.tail]
#define KEYLED_BUF_LAST g_keyled_dev.buf[(g_keyled_dev.head == 0) ? \
                                         (g_keyled_dev.buf_len - 1) : (g_keyled_dev.head - 1)]
#define KEYLED_INC_BUF(x, len) (((x) + 1) % (len))

/* keyled operation */
typedef struct {
    hi_s32(*keyled_key_open)(hi_void);
    hi_s32(*keyled_key_close)(hi_void);
    hi_s32(*keyled_key_reset)(hi_void);
    hi_s32(*keyled_key_get_value)(hi_u32 *presss_tatus_ptr, hi_u32 *key_id_ptr);
    hi_s32(*keyled_key_set_block_time)(hi_u32 block_time_ms);
    hi_s32(*keyled_key_set_repeat_time)(hi_u32 repeat_time_ms);
    hi_s32(*keyled_key_enable_repeat_key)(hi_u32 repeat_key);
    hi_s32(*keyled_key_enable_key_up)(hi_u32 enable_keyup);

    hi_s32(*keyled_led_open)(hi_void);
    hi_s32(*keyled_led_close)(hi_void);
    hi_s32(*keyled_led_display)(hi_u32 code_value);
    hi_s32(*keyled_led_display_time)(hi_keyled_time keyled_time);
    hi_s32(*keyled_led_set_flash_pin)(hi_keyled_light pin);
    hi_s32(*keyled_led_set_flash_freq)(hi_keyled_level level);
    hi_s32(*keyled_led_display_led)(hi_keyled_display led_data);

    hi_s32(*keyled_set_mode)(hi_keyled_mode keyled_mode);

    hi_s32(*keyled_led_set_lock_indicator)(hi_bool block);

    /* get the proc information of the module */
    hi_s32(*keyled_get_proc_info)(keyled_proc_info *proc_info);

    hi_s32(*keyled_suspend)(hi_void);
    hi_s32(*keyled_resume)(hi_void);
} keyled_operation;

#endif