/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ioctl
 * Create: 2019-06-29
 */

#ifndef __DRV_HIFB_IOCTL__
#define __DRV_HIFB_IOCTL__

#include <linux/fb.h>
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 drv_hifb_compat_ioctl(struct fb_info *info, hi_u32 cmd, unsigned long arg);
hi_s32 drv_hifb_ioctl(struct fb_info *info, hi_u32 cmd, unsigned long arg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_HIFB_IOCTL__ */
