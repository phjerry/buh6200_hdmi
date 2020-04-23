/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: base.h for lpmcu
 */

#ifndef __BASE_H__
#define __BASE_H__

#include "hi_type.h"

#define HI_LOG_ENABLE 1

#define wait_minute_1(x) do{ \
    hi_u8 i; \
    for(i = 0; i < x; i++); \
} while (0);

#define wait_minute_2(x, y) do { \
    hi_u8 i, j; \
    for (i = 0; i < x; i++) { \
        for (j = 0; j < y; j++); \
    } \
} while (0);

#define wait_minute_3(x, y, z) do { \
    hi_u8 i, j, k; \
    for (i = 0; i < x; i++) { \
        for (j = 0; j < y; j++) { \
            for (k = 0; k < z; k++); \
        } \
    } \
} while (0);

#define PASSIVE_STANDBY 0
#define DEEP_ACTIVE_STANDBY 1

enum {
    HI_PMOC_WAKEUP_TYPE_IR = 0,
    HI_PMOC_WAKEUP_TYPE_KEYLED,
    HI_PMOC_WAKEUP_TYPE_GPIO,
    HI_PMOC_WAKEUP_TYPE_LSADC,
    HI_PMOC_WAKEUP_TYPE_UART,
    HI_PMOC_WAKEUP_TYPE_ETH,
    HI_PMOC_WAKEUP_TYPE_USB,
    HI_PMOC_WAKEUP_TYPE_VGA = 0x10,
    HI_PMOC_WAKEUP_TYPE_SCART,
    HI_PMOC_WAKEUP_TYPE_HDMIRX_PLUGIN,
    HI_PMOC_WAKEUP_TYPE_HDMIRX_CEC,
    HI_PMOC_WAKEUP_TYPE_HDMITX_CEC,
    HI_PMOC_WAKEUP_TYPE_TIMEOUT = 0x20,
    HI_PMOC_WAKEUP_TYPE_MAX
};

#define CFG_BASE_ADDR 0x00840000
#define SC_CRG_CTRL_0 (CFG_BASE_ADDR + 0x50)
#define SC_CRG_CTRL_1 0x60
#define SC_POWER_STAT (CFG_BASE_ADDR + 0x100)
#define SC_LOW_POWER_CTRL (CFG_BASE_ADDR + 0x108)
#define SC_CORE_PWRUP_COUNT (CFG_BASE_ADDR + 0x148)
#define SC_DDRPHY_LP_EN (CFG_BASE_ADDR + 0x180)
#define SC_LP_START (CFG_BASE_ADDR + 0x104)

#define REG_AON_TIMERS_BASE 0x00852000
#define REG_AON_IR_BASE 0x00850000
#define MCU_SRST_CTRL 0x60

/* variable */
extern volatile hi_u8 g_wakeup_type;
extern volatile hi_u8 g_resume_flag;

extern hi_u8 g_dbg_mask;
extern hi_u32 g_switch_suspend_type_time;

#if HI_LOG_ENABLE
extern hi_void printf_char(hi_u8 ch);
extern hi_void printf_val(hi_u32_data reg_data);
extern hi_void printf_str(const hi_char *string);
extern hi_void printf_hex(hi_u32 hex);
#else
#define printf_char
#define printf_val
#define printf_str
#define printf_hex
#endif

extern hi_u32 reg_get(hi_u32 addr);
extern hi_void reg_set(hi_u32 addr, hi_u32 value);
extern hi_u32 ram_get(hi_u32 addr);
extern hi_void ram_set(hi_u32 addr, hi_u32 value);
extern hi_void uart_init(hi_void);

#endif
