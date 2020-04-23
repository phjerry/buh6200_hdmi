/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2004-2019. All rights reserved.
 * Description:
 */

#ifndef __KEYLED_FD650_H__
#define __KEYLED_FD650_H__

#include "drv_keyled.h"

#ifdef HI_KEYLED_FD650_CLOCK_GPIO
#define CLK_FD650  HI_KEYLED_FD650_CLOCK_GPIO
#define DINOUT_FD650  HI_KEYLED_FD650_DINOUT_GPIO
#else
#define CLK_FD650  0x2b
#define DINOUT_FD650  0x2d
#endif

#define FD650_SCL       (CLK_FD650)
#define FD650_SDA       (DINOUT_FD650)
#define HIGH    (1)
#define LOW     (0)
#define FD50_OUTPUT LOW
#define FD50_INPUT HIGH

#define DELAY  udelay(1)

#define FD650_SCL_SET do { \
    if ((g_gpio_func != HI_NULL) && (g_gpio_func->pfn_gpio_write_bit != HI_NULL)) \
        g_gpio_func->pfn_gpio_write_bit(FD650_SCL, HIGH); \
    } while (0)

#define FD650_SCL_CLR do { \
    if ((g_gpio_func != HI_NULL) && (g_gpio_func->pfn_gpio_write_bit != HI_NULL))\
        g_gpio_func->pfn_gpio_write_bit(FD650_SCL, LOW);\
    } while (0)

#define FD650_SCL_D_OUT do { \
    if ((g_gpio_func != HI_NULL) && (g_gpio_func->pfn_gpio_direction_set_bit != HI_NULL)) \
        g_gpio_func->pfn_gpio_direction_set_bit(FD650_SCL, FD50_OUTPUT); \
    } while (0)

#define FD650_SDA_SET do { \
    if ((g_gpio_func != HI_NULL) && (g_gpio_func->pfn_gpio_write_bit != HI_NULL)) \
        g_gpio_func->pfn_gpio_write_bit(FD650_SDA, HIGH); \
    } while (0)

#define FD650_SDA_CLR do { \
    if ((g_gpio_func != HI_NULL) && (g_gpio_func->pfn_gpio_write_bit != HI_NULL)) \
        g_gpio_func->pfn_gpio_write_bit(FD650_SDA, LOW); \
    } while (0)

#define FD650_SDA_IN         hi_gpio_read_sda_in()

#define FD650_SDA_D_OUT do { \
    if ((g_gpio_func != HI_NULL) && (g_gpio_func->pfn_gpio_direction_set_bit != HI_NULL)) \
        g_gpio_func->pfn_gpio_direction_set_bit(FD650_SDA, FD50_OUTPUT); \
    } while (0)

#define FD650_SDA_D_IN do { \
    if ((g_gpio_func != HI_NULL) && (g_gpio_func->pfn_gpio_direction_set_bit != HI_NULL)) \
        g_gpio_func->pfn_gpio_direction_set_bit(FD650_SDA, FD50_INPUT); \
    } while (0)

/* System parameter command */
/* CNcomment:设置系统参数命令 */
#define FD650_BIT_ENABLE     0x01   /* Open/close bit            */     /* CNcomment: 开启/关闭位 */
#define FD650_BIT_SLEEP      0x04   /* Sleep control bit         */     /* CNcomment: 睡眠控制位  */
#define FD650_BIT_7SEG       0x08   /* Seven segment control bit */     /* CNcomment: 7段控制位   */
#define FD650_BIT_INTENS1    0x10   /* Level 1 light             */     /* CNcomment: 1级亮度     */
#define FD650_BIT_INTENS2    0x20   /* Level 2 light             */     /* CNcomment: 2级亮度     */
#define FD650_BIT_INTENS3    0x30   /* Level 3 light             */     /* CNcomment: 3级亮度     */
#define FD650_BIT_INTENS4    0x40   /* Level 4 light             */     /* CNcomment: 4级亮度     */
#define FD650_BIT_INTENS5    0x50   /* Level 5 light             */     /* CNcomment: 5级亮度     */
#define FD650_BIT_INTENS6    0x60   /* Level 6 light             */     /* CNcomment: 6级亮度     */
#define FD650_BIT_INTENS7    0x70   /* Level 7 light             */     /* CNcomment: 7级亮度     */
#define FD650_BIT_INTENS8    0x00   /* Level 8 light             */     /* CNcomment: 8级亮度     */

#define FD650_SYSOFF     0x0400                              /* Close led and key */ /* CNcomment:关闭显示、关闭键盘 */
#define FD650_SYSON      ( FD650_SYSOFF | FD650_BIT_ENABLE ) /* Open led and key  */ /* CNcomment:开启显示、键盘 */
#define FD650_SLEEPOFF    FD650_SYSOFF                       /* Close sleep  */ /* CNcomment:关闭睡眠 */
#define FD650_SLEEPON    ( FD650_SYSOFF | FD650_BIT_SLEEP )  /* Open sleep  */ /* CNcomment:开启睡眠 */
#define FD650_7SEG_ON    ( FD650_SYSON | FD650_BIT_7SEG )    /* Open seven segment mode */
#define FD650_8SEG_ON    ( FD650_SYSON | 0x00 )              /* Open eight segment mode */
#define FD650_SYSON_1    ( FD650_SYSON | FD650_BIT_INTENS1 ) /* Open led, key, level 1 light */
#define FD650_SYSON_2    ( FD650_SYSON | FD650_BIT_INTENS2 ) /* Open led, key, level 2 light */
#define FD650_SYSON_3    ( FD650_SYSON | FD650_BIT_INTENS3 ) /* Open led, key, level 3 light */
#define FD650_SYSON_4    ( FD650_SYSON | FD650_BIT_INTENS4 ) /* Open led, key, level 4 light */
#define FD650_SYSON_5    ( FD650_SYSON | FD650_BIT_INTENS5 ) /* Open led, key, level 5 light */
#define FD650_SYSON_6    ( FD650_SYSON | FD650_BIT_INTENS6 ) /* Open led, key, level 6 light */
#define FD650_SYSON_7    ( FD650_SYSON | FD650_BIT_INTENS7 ) /* Open led, key, level 7 light */
#define FD650_SYSON_8    ( FD650_SYSON | FD650_BIT_INTENS8 ) /* Open led, key, level 8 light */

/* Data command */
/* CNcomment:加载字数据命令 */
#define FD650_DIG0        0x1400
#define FD650_DIG1        0x1500
#define FD650_DIG2        0x1600
#define FD650_DIG3        0x1700
#define FD650_DOT         0x0080

/* Reading key command */
/* CNcomment:读取按键代码命令 */
#define FD650_GET_KEY     0x0700

#define SCAN_INTV   (30) /* ms */

#define DISPLAY_ON  1
#define DISPLAY_OFF  0
#define KEY_MACRO_NO (0xff)
#define DISPLAY_REG_NUM     (14)
#define DISPLAY_SEG_CURR     (4)
#define KEY_COL_NUM     10
#define DISPLAY_REG_START_ADDR  (0x00)


hi_s32 drv_keyled_key_open_fd650(hi_void);
hi_s32 drv_keyled_key_close_fd650(hi_void);
hi_s32 drv_keyled_key_reset_fd650(hi_void);
hi_s32 drv_keyled_key_get_value_fd650(hi_u32 *press_status_ptr, hi_u32 *key_id_ptr);
hi_s32 drv_keyled_key_set_block_time_fd650(hi_u32 block_time_ms);
hi_s32 drv_keyled_key_set_repeat_time_fd650(hi_u32 rep_time_ms);
hi_s32 drv_keyled_key_enable_repeat_key_fd650(hi_u32 enable_repkey);
hi_s32 drv_keyled_key_enable_key_up_fd650(hi_u32 enable_keyup);

hi_s32 drv_keyled_led_open_fd650(hi_void);
hi_s32 drv_keyled_led_close_fd650(hi_void);
hi_s32 drv_keyled_led_display_fd650(hi_u32 code_value);
hi_s32 drv_keyled_led_display_time_fd650(hi_keyled_time led_time);
hi_s32 drv_keyled_led_set_flash_pin_fd650(hi_keyled_light led_flash_pin);
hi_s32 drv_keyled_led_set_flash_freq_fd650(hi_keyled_level led_flash_level);
hi_s32 drv_keyled_led_set_lock_indicator_fd650(hi_bool lock_flag);

hi_s32 drv_keyled_get_proc_info_fd650(keyled_proc_info *proc_info);

hi_s32 drv_keyled_suspend_fd650(hi_void);
hi_s32 drv_keyled_resume_fd650(hi_void);

#endif

