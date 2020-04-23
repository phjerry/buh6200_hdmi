/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmitx register read&wirte header file.
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __HAL_HDMITX_IO_H__
#define __HAL_HDMITX_IO_H__

#include <linux/io.h>
#include "hi_type.h"

hi_u32 hdmi_readl(void __iomem *base, hi_u32 offset);
void hdmi_writel(void __iomem *base, hi_u32 offset, hi_u32 value);

#define hdmi_clr(s, r, v)       hdmi_writel((s), (r), hdmi_readl((s), (r)) & ~(v))
#define hdmi_set(s, r, v)       hdmi_writel((s), (r), hdmi_readl((s), (r)) | (v))
#define hdmi_clrset(s, r, m, v) hdmi_writel((s), (r), (hdmi_readl((s), (r)) & ~(m)) | (v))

hi_u32 hdmi_read_bits(void __iomem *base, hi_u32 offset, hi_u32 bit_mask);
hi_void hdmi_write_bits(void __iomem *base, hi_u32 offset, hi_u32 bit_mask, hi_u32 val);

#endif /* __HAL_HDMITX_IO_H__ */
