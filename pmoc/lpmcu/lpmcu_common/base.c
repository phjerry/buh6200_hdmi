/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: common functions
 */

#include "base.h"
#include "hi_type.h"
#include "keyled.h"
#include "REG51.h"

/* param */
#define REG_READ_CMD   0x01
#define REG_WRITE_CMD  0x03

#define HEX_LEN   0x08

static hi_u32 uart_base_addr = 0x00870000;

volatile hi_u8 g_wakeup_type = 0xff;
volatile hi_u8 g_resume_flag = HI_FALSE;
hi_u8 g_dbg_mask = 0;

#if HI_LOG_ENABLE
#define trace_char(ch) do { \
    reg_set(uart_base_addr, ch); \
} while (0)

static hi_void hex_to_str(hi_u32 hex, hi_u8 *str, hi_u8 str_size)
{
    hi_u32 i;
    hi_u8 tmp;

    if (str_size < HEX_LEN) {
        return;
    }

    for (i = 0; i < HEX_LEN; i++) {
        tmp = (hex >> (28 - (i * 4))) & 0x0F;
        if (tmp >= 0 && tmp <= 9) {
            str[i] = (tmp + 0x30);
        } else if (tmp >= 10 && tmp <= 15) {
            str[i] = (tmp + 0x37);
        }
    }

    return;
}

hi_void printf_char(hi_u8 ch)
{
    hi_u32_data tmp;

    if (!(g_dbg_mask & 0x1)) {
        return;
    }

    while (1) {
        tmp.val32 = reg_get(uart_base_addr + 0x18);
        if ((tmp.val8[0] & 0x20) == 0) {
            break;
        }

        wait_minute_2(20, 20);
    }

    wait_minute_2(20, 20);

    tmp.val32 = 0;
    tmp.val8[0] = ch;
    trace_char(tmp.val32);

    return;
}

hi_void printf_val(hi_u32_data reg_data)
{
    hi_u8 i = 0;

    printf_char(10); /* line feed '\n' */
    printf_char(13); /* carriage return '\r' */
    printf_char('0');
    printf_char('x');

    for (i = 0; i < 4; i++) {
        if ((reg_data.val8[3 - i] / 16) > 9) {
            printf_char(reg_data.val8[3 - i] / 16 + 87);
        }  else {
            printf_char(reg_data.val8[3 - i] / 16 + 48);
        }

        if ((reg_data.val8[3 - i] % 16) > 9) {
            printf_char(reg_data.val8[3 - i] % 16 + 87);
        } else {
            printf_char(reg_data.val8[3 - i] % 16 + 48);
        }
    }

    printf_char(10);
    printf_char(13);

    return;
}

hi_void printf_str(const hi_char *string)
{
    const hi_char *p = string;

    for (; *p != '\0'; p++) {
        printf_char(*p);
    }

    return;
}

hi_void printf_hex(hi_u32 hex)
{
    hi_u8 str[HEX_LEN + 1] = {0};

    hex_to_str(hex, str, HEX_LEN);

    printf_str("0x");
    printf_str(str);
    printf_str("\r\n");

    return;
}
#endif

hi_u32 reg_get(hi_u32 addr)
{
    hi_u32_data tmp;
    hi_u32_data value;

    tmp.val32 = addr;

    ADDR_REG_0 = tmp.val8[0];
    ADDR_REG_1 = tmp.val8[1];
    ADDR_REG_2 = tmp.val8[2];
    ADDR_REG_3 = tmp.val8[3];

    CMD_REG = REG_READ_CMD;

    while (!(CMD_STATUS_REG & 0x02)) {
        ;
    }

    CMD_REG = 0x00;

    value.val8[0] = RD_DATA_REG_0;
    value.val8[1] = RD_DATA_REG_1;
    value.val8[2] = RD_DATA_REG_2;
    value.val8[3] = RD_DATA_REG_3;

    return value.val32;
}

hi_void reg_set(hi_u32 addr, hi_u32 value)
{
    hi_u32_data tmp_addr;
    hi_u32_data tmp_value;

    tmp_addr.val32 = addr;
    tmp_value.val32 = value;

    ADDR_REG_0 = tmp_addr.val8[0];
    ADDR_REG_1 = tmp_addr.val8[1];
    ADDR_REG_2 = tmp_addr.val8[2];
    ADDR_REG_3 = tmp_addr.val8[3];
    WR_DATA_REG_0 = tmp_value.val8[0];
    WR_DATA_REG_1 = tmp_value.val8[1];
    WR_DATA_REG_2 = tmp_value.val8[2];
    WR_DATA_REG_3 = tmp_value.val8[3];

    CMD_REG = REG_WRITE_CMD;

    while (CMD_STATUS_REG & 0x01) {
        ;
    }

    CMD_REG = 0x00;

    return ;
}

/* read and write LPMCU ram directly, not by bus */
hi_u32 ram_get(hi_u32 addr)
{
    hi_u32_data reg_data;

    reg_data.val8[0] = *((unsigned char volatile __xdata *)addr + 0);
    reg_data.val8[1] = *((unsigned char volatile __xdata *)addr + 1);
    reg_data.val8[2] = *((unsigned char volatile __xdata *)addr + 2);
    reg_data.val8[3] = *((unsigned char volatile __xdata *)addr + 3);

    return reg_data.val32;
}

hi_void ram_set(hi_u32 addr, hi_u32 value)
{
    hi_u32_data tmp_value;

    tmp_value.val32 = value;

    *((unsigned char volatile __xdata *)addr + 0) = tmp_value.val8[0];
    *((unsigned char volatile __xdata *)addr + 1) = tmp_value.val8[1];
    *((unsigned char volatile __xdata *)addr + 2) = tmp_value.val8[2];
    *((unsigned char volatile __xdata *)addr + 3) = tmp_value.val8[3];

    return ;
}

