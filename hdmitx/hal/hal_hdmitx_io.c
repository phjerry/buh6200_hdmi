/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmitx register read&wirte function.
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#include <hi_type.h>
#include "hi_osal.h"
#include "hal_hdmitx_io.h"

#define RIGHT_SHIFT_MAX 32

static inline hi_u32 get_right_shift_cnt(hi_u32 bit_mask)
{
    hi_u32 i;

    for (i = 0; i < RIGHT_SHIFT_MAX; i++) {
        if (bit_mask & (1 << i)) {
            break;
        }
    }

    return i;
}

hi_u32 hdmi_readl(void __iomem *base, hi_u32 offset)
{
    hi_u32 value;

    value = osal_readl(base + offset);
    return value;
}

hi_u32 hdmi_read_bits(void __iomem *base, hi_u32 offset, hi_u32 bit_mask)
{
    hi_u32 reg_val;
    hi_u32 right_shift_cnt;

    right_shift_cnt = get_right_shift_cnt(bit_mask);
    reg_val = hdmi_readl(base, offset);
    return (reg_val & bit_mask) >> right_shift_cnt;
}

hi_void hdmi_writel(void __iomem *base, hi_u32 offset, hi_u32 value)
{
    osal_writel(value, base + offset);
}

hi_void hdmi_write_bits(void __iomem *base, hi_u32 offset, hi_u32 bit_mask, hi_u32 val)
{
    hi_u32 shift_cnt;
    hi_u32 reg_val;

    shift_cnt = get_right_shift_cnt(bit_mask);
    reg_val = hdmi_readl(base, offset);
    reg_val &= ~bit_mask;
    reg_val |= (val << shift_cnt) & bit_mask;
    hdmi_writel(base, offset, reg_val);
}

