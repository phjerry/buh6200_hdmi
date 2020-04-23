/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
 * Description:
 */

#ifndef __KEYLED_STD_H__
#define __KEYLED_STD_H__

#include "drv_keyled.h"

#define KEYLED_IRQ (84 + 32)

#define KEYLED_MINOR 0
#define KEYLED_PHY_ADDR (0x851000)

#define KEY_NUM (8)

#define DOT_FLASH_TIME (570)

#define KEY_TYPE_CT1642  1

/* define key state */
#define KEY_PRESS (0x0)
#define KEY_HOLD (0x01)
#define KEY_RELEASE (0x02)

/* LEDC_CONTROL */
#define KEYPAD_ENABLE (1 << 3)
#define KEYPAD_PRESS_INTR (1 << 1)
#define KEYPAD_RELEASE_INTR (1 << 0)

/* LEDC_KEYDATA */
#define KEY_7_PRESS (1 << 7)
#define KEY_6_PRESS (1 << 6)
#define KEY_5_PRESS (1 << 5)
#define KEY_4_PRESS (1 << 4)
#define KEY_3_PRESS (1 << 3)
#define KEY_2_PRESS (1 << 2)
#define KEY_1_PRESS (1 << 1)
#define KEY_0_PRESS (1 << 0)

#define KEY_76comb_PRESS (KEY_7_PRESS|KEY_6_PRESS)
#define KEY_75comb_PRESS (KEY_7_PRESS|KEY_5_PRESS)
#define KEY_74comb_PRESS (KEY_7_PRESS|KEY_4_PRESS)
#define KEY_73comb_PRESS (KEY_7_PRESS|KEY_3_PRESS)
#define KEY_72comb_PRESS (KEY_7_PRESS|KEY_2_PRESS)
#define KEY_71comb_PRESS (KEY_7_PRESS|KEY_1_PRESS)
#define KEY_70comb_PRESS (KEY_7_PRESS|KEY_0_PRESS)
#define KEY_65comb_PRESS (KEY_6_PRESS|KEY_5_PRESS)
#define KEY_64comb_PRESS (KEY_6_PRESS|KEY_4_PRESS)
#define KEY_63comb_PRESS (KEY_6_PRESS|KEY_3_PRESS)
#define KEY_62comb_PRESS (KEY_6_PRESS|KEY_2_PRESS)
#define KEY_61comb_PRESS (KEY_6_PRESS|KEY_1_PRESS)
#define KEY_60comb_PRESS (KEY_6_PRESS|KEY_0_PRESS)
#define KEY_54comb_PRESS (KEY_5_PRESS|KEY_4_PRESS)
#define KEY_53comb_PRESS (KEY_5_PRESS|KEY_3_PRESS)
#define KEY_52comb_PRESS (KEY_5_PRESS|KEY_2_PRESS)
#define KEY_51comb_PRESS (KEY_5_PRESS|KEY_1_PRESS)
#define KEY_50comb_PRESS (KEY_5_PRESS|KEY_0_PRESS)
#define KEY_43comb_PRESS (KEY_4_PRESS|KEY_3_PRESS)
#define KEY_42comb_PRESS (KEY_4_PRESS|KEY_2_PRESS)
#define KEY_41comb_PRESS (KEY_4_PRESS|KEY_1_PRESS)
#define KEY_40comb_PRESS (KEY_4_PRESS|KEY_0_PRESS)
#define KEY_32comb_PRESS (KEY_3_PRESS|KEY_2_PRESS)
#define KEY_31comb_PRESS (KEY_3_PRESS|KEY_1_PRESS)
#define KEY_30comb_PRESS (KEY_3_PRESS|KEY_0_PRESS)
#define KEY_21comb_PRESS (KEY_2_PRESS|KEY_1_PRESS)
#define KEY_20comb_PRESS (KEY_2_PRESS|KEY_0_PRESS)
#define KEY_10comb_PRESS (KEY_1_PRESS|KEY_0_PRESS)
#define KEYPAD_ERROR     (1000)

#define SYSTEM_CLK (24)                /* Unit:MHz 108M*/
#define CLKTIM_DEFAULT (1000) /* (250)       Unit:KHz 500*/
#define FRETIM_DEFAULT (200) /* (650)        Unit:Hz 200H*/
#define FLASHTIM_DEFAULT (60)          /* Unit:Hz 10Hz*/
#define KEY_DEFAULT (25) /* (240)     Unit:Hz 4Hz */
#define SYSTIM_DEFAULT (5)             /* 1 divide */

#define FLASH_LEVEL1 (1)
#define FLASH_LEVEL2 (2)
#define FLASH_LEVEL3 (3)
#define FLASH_LEVEL4 (4)
#define FLASH_LEVEL5 (5)

#define LEDC_CONTROL (0x000)       /* LED control register */
#define LEDC_CONFIG (0x004)        /* LED configure register  */
#define LEDC_KEYINT (0x008)        /* Key sample interrupt register */
#define LEDC_KEYDATA (0x00C)       /* LED sample state */
#define LEDC_CLKTIM (0x010)        /* LedClk signal high/low level parameter */
#define LEDC_FRETIM (0x014)        /* LED scan frequency */
#define LEDC_FLASHTIM (0x018)      /* LED flash frequency */
#define LEDC_KEYTIM (0x01C)        /* Key scan frequency */
#define LEDC_DATA1 (0x020)
#define LEDC_DATA2 (0x024)
#define LEDC_DATA3 (0x028)
#define LEDC_DATA4 (0x02C)
#define LEDC_DATA5 (0x030)        /* */
#define LEDC_SYSTIM (0x34)        /* config system frequeny divide 0~15 to compatible 1*8 keyscan rate */

/* Ledc control */
#define LED_ENABLE  (1 << 9)        /* LED module enable */
#define LEDC_ENABLE (1 << 8)        /* LED lighten enable */
#define LEDC_FLASH_4 (1 << 7)       /* 4th LED falsh enable */
#define LEDC_FLASH_3 (1 << 6)       /* 3rd LED falsh enable */
#define LEDC_FLASH_2 (1 << 5)       /* 2nd LED falsh enable */
#define LEDC_FLASH_1 (1 << 4)       /* 1st LED falsh enable */
#define LEDC_FLASH_5 (1 << 2)       /* the fifth LED flashed enable */
#define FLASH_ENABLE_4 (LEDC_FLASH_1 | LEDC_FLASH_2 | LEDC_FLASH_3 | LEDC_FLASH_4) /* falsh LED1~LED4 */

/* ledc config */
#define LEDC_KEY_TYPE  (1 << 5)             /* select keypad type: 0:74HC164; 1:CT1642 */
#define KEY_SCAN_LEVEL      (1 << 4)        /* set 1*8 key output and sampling LEVEL 1: high level 0: low level */
#define LED_NUM             (1 << 3)        /* select LED number 1: 5LED 0: 4LED */
#define KEY_SCAN_MODE       (1 << 2)        /* KEY scan mode 1: 8*1  0: 4*2 */
#define LEDC_ANODE (1 << 1)
#define LEDC_HIGHT_LEVEL (1 << 0)


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
hi_s32 keyled_led_displayled_ct1642(hi_keyled_display led_data);

hi_s32 keyled_suspend_ct1642(hi_void);
hi_s32 keyled_resume_ct1642(hi_void);


#endif



