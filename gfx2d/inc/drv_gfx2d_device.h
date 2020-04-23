/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: device node operation header
 * Author: sdk
 * Create: 2018-12-10
 */

#ifndef __SOURCE_MSP_DRV_GFX2D_INC_DEVICE__
#define __SOURCE_MSP_DRV_GFX2D_INC_DEVICE__

#include "hi_type.h"
#include "hi_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_slong drv_gfx2d_dev_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg);
#ifdef CONFIG_COMPAT
hi_slong drv_gfx2d_dev_compat_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOURCE_MSP_DRV_GFX2D_INC_DEVICE__ */
