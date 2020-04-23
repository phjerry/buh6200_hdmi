/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
 * Description:
 */

#include <linux/errno.h>
#include <linux/stddef.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/relay.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/cpu.h>
#include <linux/splice.h>
#include <asm/io.h>
#include <mach/platform.h>
#include <mach/hardware.h>
#include <asm/memory.h>
#include <asm/setup.h>

#include "hi_type.h"
#include "drv_base_ext.h"
#include "drv_gpio_ext.h"
#include "hi_drv_keyled.h"
#include "hi_unf_keyled.h"
#include "drv_keyled_ct1642.h"
#include "linux/hisilicon/securec.h"

extern hi_s32 keyled_key_open_ct1642(hi_void);
extern hi_s32 keyled_key_close_ct1642(hi_void);

extern hi_s32 keyled_led_open_ct1642(hi_void);
extern hi_s32 keyled_led_close_ct1642(hi_void);
extern hi_s32 keyled_led_display_ct1642(hi_u32 code_value);
extern hi_s32 keyled_led_display_time_ct1642(hi_unf_keyled_time keyled_time);
extern hi_s32 keyled_led_set_flash_pin_ct1642(hi_unf_keyled_light pin);
extern hi_s32 keyled_led_set_flash_freq_ct1642(hi_unf_keyled_level level);

/*     --7--
 *    |     |
 *   2|     |6
 *    |--1--    0
 *    |     |
 *   3|     |5
 *     --4--
 */


hi_u8 num_tbl[] = { 0xfc, 0x60, 0xda, 0xf2, 0x66, 0xb6, 0xbe, 0xe0, 0xfe, 0xf6 }; /* ok */

/* only 9 of the capital letters are effective ('C' 'E' 'F' 'H' 'L' 'O' 'P' 'S' 'U') */
/* CNcomment:大写字母只显示9个有效('C'、'E'、'F'、'H'、'L'、'O'、'P'、'S'、'U') */
hi_u8 chr_tbl_up[] = {
    0xee, 0xfe, 0x9c, 0xfc, 0x9e, 0x8e, 0x00,
    0x6e, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00,
    0xfc, 0xce, 0x00, 0x00, 0xb6, 0x00, 0x7c,
    0x00, 0x00, 0x00, 0x00, 0x00
};

/* only 15 of the smalll letters are effective (b c d E, g h i, l n o, P q S(5) t, u) */
/* CNcomment:小写字母只显示15个有效(b c d E, g h i, l n o, P q S(5) t, u) */
hi_u8 chr_tbl_low[] = {
    0x00, 0x3e, 0x1a, 0x7a, 0xde, 0x00, 0xf6,
    0x2e, 0x20, 0x00, 0x00, 0x60, 0x00, 0x2a,
    0x3a, 0xce, 0xe6, 0x00, 0xb6, 0x1e, 0x38,
    0x00, 0x00, 0x00, 0x00, 0x00
};

static hi_char led_display_str[5] = {"boot"};

hi_s32 keyled_get_display_para(void)
{
    hi_s8 *s = HI_NULL;
    hi_s8 *p = HI_NULL;
    hi_s8 *cmd_line = HI_NULL;

    cmd_line = kmalloc(COMMAND_LINE_SIZE, GFP_KERNEL);
    if (!cmd_line) {
        HI_ERR_KEYLED("KEYLED_GetDisplayPara request mem failure \n");
        return HI_FAILURE;
    }

    if (memset_s(cmd_line, COMMAND_LINE_SIZE, 0, COMMAND_LINE_SIZE)) {
        HI_ERR_KEYLED("memset_s failed! \n");
        kfree(cmd_line);
        cmd_line = HI_NULL;
        return HI_FAILURE;
    }
    strlcpy(cmd_line, saved_command_line, COMMAND_LINE_SIZE);
    p = strstr(cmd_line, "LedDisplay=");
    if (p) {
        s = strsep((char **)&p, "=");
        if (s) {
            s = strsep((char **)&p, " ");
            if (s) {
                if (memcpy_s(led_display_str, sizeof(led_display_str), s, sizeof(led_display_str))) {
                    HI_ERR_KEYLED("memcpy_s failed! \n");
                    kfree(cmd_line);
                    cmd_line = HI_NULL;
                    return HI_FAILURE;
                }
            }
        }
    }

    kfree(cmd_line);
    cmd_line = HI_NULL;

    return HI_SUCCESS;
}

hi_s32 keyled_display_string(hi_char *string)
{
    hi_u32 tmp_data = 0;
    hi_u8 display_code[5] = {0};
    hi_u8 j;

    if (string == HI_NULL) {
        return HI_FAILURE;
    }

    if (memcpy_s(display_code, sizeof(display_code), string,  sizeof(display_code) - 1)) {
        HI_ERR_KEYLED("memcpy_s failed! \n");
        return HI_FAILURE;
    }

    for (j = 0; j < 4; j++) {
        if ((display_code[j] >= 'A') && (display_code[j] <= 'Z')) {
            display_code[j] = chr_tbl_up[display_code[j] - 'A'];
        } else if ((display_code[j] >= 'a') && (display_code[j] <= 'z')) {
            display_code[j] = chr_tbl_low[display_code[j] - 'a'];
        } else if ((display_code[j] >= '0') && (display_code[j] <= '9')) {
            display_code[j] = num_tbl[display_code[j] - '0'];
        } else if (display_code[j] == '-') {
            display_code[j] = 0x02;
        } else if (display_code[j] == '_') {
            display_code[j] = 0x10;
        } else if (display_code[j] == ':' || display_code[j] == '.') {
            display_code[j] = 0x01;
        } else { /* FIXME: may be should add symbol ':' and '.' */
            display_code[j] = 0x0; /* default: not display */
        }
    }

    tmp_data = (display_code[3] << 24) | (display_code[2] << 16) | (display_code[1] << 8) | (display_code[0]);

    keyled_led_display_ct1642(tmp_data);
    return HI_SUCCESS;
}

hi_s32 keyled_param_init(void)
{
    hi_s32 ret;
    ret = keyled_key_open_ct1642();

    ret |= keyled_led_open_ct1642();

    ret |= keyled_get_display_para();

    return ret;
}

static hi_s32 keyled_display_init(void)
{
    hi_s32 ret;

#ifndef HI_MCE_SUPPORT
    ret = hi_drv_common_init();
    if (ret) {
        HI_ERR_KEYLED("common init err\n");
    } else {
        HI_INFO_KEYLED("common init ok\n");
    }

    ret = hi_drv_gpio_init();
    if (ret) {
        HI_ERR_KEYLED("gpio init err\n");
    } else {
        HI_INFO_KEYLED("gpio init ok\n");
    }
#endif

    ret = keyled_param_init();
    if (ret) {
        HI_ERR_KEYLED("keyled Param Init err !");
    } else {
        HI_INFO_KEYLED("keyled Param Init ok\n");
    }

    ret = keyled_display_string(led_display_str);
    if (ret) {
        HI_ERR_KEYLED("keyled display string param err !");
    }

    HI_INFO_KEYLED("<<<<<< keyled_display   end <<<<<<\n");
    return HI_SUCCESS;
}

core_initcall(keyled_display_init);

