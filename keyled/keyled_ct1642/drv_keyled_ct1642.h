/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2008-2019. All rights reserved.
 * Description:
 */

#include "drv_keyled_ioctl.h"
#include "drv_keyled.h"

#define LED_BIT_COUNT   4

#define STB_KEY_MENU    1
#define STB_KEY_OK      2
#define STB_KEY_UP      3
#define STB_KEY_DOWN    4
#define STB_KEY_LEFT    5
#define STB_KEY_RIGHT   6
#define STB_KEY_EXIT    7
#define STB_KEY_BUTT    8
#define STB_KEY_NULL    0
#define STB_KEY_HOLD    0x5a

#ifdef HI_KEYLED_CT1642_CLOCK_GPIO
#define CLK_CT1642 HI_KEYLED_CT1642_CLOCK_GPIO
#define DAT_CT1642 HI_KEYLED_CT1642_DAT_GPIO
#define KEY_CT1642 HI_KEYLED_CT1642_KEY_GPIO
#else
#define CLK_CT1642 0x2a
#define DAT_CT1642 0x2c
#define KEY_CT1642 0x2f
#endif

#define SCAN_INTV_CT1642    (100)
#define KEY_MACRO_NO (0xff)

hi_s32 keyled_key_open_ct1642(hi_void);
hi_s32 keyled_key_close_ct1642(hi_void);
hi_s32 keyled_key_reset_ct1642(hi_void);
hi_s32 keyled_key_get_value_ct1642(hi_u32 *press_status_ptr, hi_u32 *key_id_ptr);
hi_s32 keyled_key_set_block_time_ct1642(hi_u32 block_time_ms);
hi_s32 keyled_key_set_repeat_time_ct1642(hi_u32 repeat_time_ms);
hi_s32 keyled_key_enable_repeat_key_ct1642(hi_u32 enable_repkey);
hi_s32 keyled_key_enable_key_up_ct1642(hi_u32 enable_keyup);

hi_s32 keyled_led_open_ct1642(hi_void);
hi_s32 keyled_led_close_ct1642(hi_void);
hi_s32 keyled_led_display_ct1642(hi_u32 code_value);
hi_s32 keyled_led_display_time_ct1642(hi_keyled_time display_time);
hi_s32 keyled_led_set_flash_pin_ct1642(hi_keyled_light pin);
hi_s32 keyled_led_set_flash_freq_ct1642(hi_keyled_level level);

hi_s32 keyled_get_proc_info_ct1642(keyled_proc_info *proc_info);

hi_s32 keyled_suspend_ct1642(hi_void);
hi_s32 keyled_resume_ct1642(hi_void);

