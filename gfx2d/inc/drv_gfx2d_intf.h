/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: gfx2d dev manage
 * Author: sdk
 * Create: 2019-10-23
 */

#ifndef __SOURCE_MSP_DRV_GFX2D_INTF_H_
#define __SOURCE_MSP_DRV_GFX2D_INTF_H_

#include <linux/device.h>
#include "hi_osal.h"

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 drv_gfx2d_pm_register(hi_void);
hi_void drv_gfx2d_pm_unregister(hi_void);
hi_s32 drv_gfx2d_dev_suspend(struct device *dev);
hi_s32 drv_gfx2d_dev_resume(struct device *dev);
hi_s32 drv_gfx2d_dev_resume_early(struct device *dev);
hi_s32 drv_gfx2d_dev_open(struct inode *finode, struct file *ffile);
hi_s32 drv_gfx2d_dev_colse(struct inode *finode, struct file *ffile);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOURCE_MSP_DRV_GFX2D_INTF_H_ */
