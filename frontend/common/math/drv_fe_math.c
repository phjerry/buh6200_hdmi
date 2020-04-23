/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: frontend math function
 * Author: SDK
 * Created: 2017-06-30
 */
#include "drv_fe_math.h"

static hi_u32 g_snr_log_table[] = {
    0, 100,
    2, 104,
    4, 109,
    6, 114,
    8, 120,
    10, 125,
    12, 131,
    14, 138,
    16, 144,
    18, 151,
    20, 158,
    22, 166,
    24, 173,
    26, 182,
    28, 190,
    30, 199,
    32, 208
};

static hi_u32 left_table(int in_value)
{
    hi_u32 out_value = 0;
    hi_u32 iter = 0;
    hi_u32 table_size = sizeof(g_snr_log_table) / sizeof(int);

    for (iter = 0; iter < table_size; iter += 2) { /* 2: 索引号 */
        if ((iter + 1) < table_size) {
            if (in_value <= g_snr_log_table[iter + 1]) {
                out_value = g_snr_log_table[iter];
                return out_value;
            }
        }
    }

    out_value = 32; /* 32: 超出部分，默认返回32 */
    return out_value;
}

/* 100 * log10(in_value) */
hi_u32 drv_fe_log10_convert(hi_u32 in_value)
{
    hi_u8 index = 0;
    hi_u32 tmp = 1;
    hi_u32 in_value_left = 1;
    hi_u32 log_value;
    hi_u32 d_value;

    if (in_value == 0) {
        return 0;
    }

    do {
        tmp = tmp << 1;
        if (in_value < tmp) {
            break;
        }
    } while (++index < 32); /* 32: Max 32 bits */

    log_value = 301 * index / 10; /* 301: for calculate log, 10: for calculate log */

    d_value = (1 << index);
    if (d_value != 0) {
        in_value_left = in_value * 100 / d_value; /* 100: kernel not support float, multiply 100 */
    }

    log_value = log_value + left_table(in_value_left);

    return log_value;
}

/* 100 * log10(a/b) */
hi_s32 drv_fe_div_log10_convert(hi_u32 a, hi_u32 b)
{
    hi_u8 index = 0;
    hi_u32 tmp;
    hi_u32 in_value_left = 1;
    hi_u32 log_value, d_value, div;
    hi_s32 sig = 1;

    if (b == 0) {
        return 0;
    }

    if (a < b) {
        tmp = a;
        a = b;
        b = tmp;
        sig = -1;
    }

    div = a / b;

    do {
        if (div < (2 << index)) {
            break;
        }
    } while (++index < 32); /* 32: Max 32 bits */

    log_value = 301 * index / 10; /* 301: for calculate log, 10: for calculate log */

    d_value = (1 << index);
    if (d_value != 0) {
        in_value_left = a * 100 / b / d_value; /* 100: kernel not support float, multiply 100 */
    }

    log_value = log_value + left_table(in_value_left);

    return sig * log_value;
}

hi_void drv_fe_ber_convert(hi_u32 a, hi_u32 b, hi_drv_frontend_scientific_num *num)
{
    hi_s32 power = 0;

    if (num == NULL) {
        return;
    }
    if (b == 0 || a == 0) {
        num->integer_val = 0;
        num->decimal_val = 0;
        num->power = 0;
        return;
    }
    while (a < b) {
        a *= 10;
        power++;
    }
    num->integer_val = a / b;
    num->decimal_val = a * 1000 / b % 1000; /* 1000: for decimal value */
    num->power = -power;
}

