/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: dev suspend & resume manage
 * Author: sdk
 * Create: 2019-10-23
 */

#ifndef __SOURCE_MSP_DRV_TDE_INTF_H_
#define __SOURCE_MSP_DRV_TDE_INTF_H_

#include <linux/device.h>
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 drv_tde_pm_register(hi_void);
hi_void drv_tde_pm_unregister(hi_void);
hi_s32 drv_tde_suspend(struct device *dev);
hi_s32 drv_tde_resume(struct device *dev);
hi_s32 drv_tde_resume_early(struct device *dev);
hi_s32 drv_tde_dev_open(struct inode *finode, struct file *ffile);
hi_s32 drv_tde_dev_release(struct inode *finode, struct file *ffile);
long drv_tde_dev_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg);
hi_s32 drv_tde_tasklet_init(hi_void);
hi_void drv_tde_tasklet_dinit(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOURCE_MSP_DRV_TDE_INTF_H_ */
