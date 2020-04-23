/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description:jpeg device open/close/suspend/resume define
 */

#ifndef __DRV_JPEG_INTF_H__
#define __DRV_JPEG_INTF_H__

#include <linux/device.h>
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 drv_jpeg_dev_open(struct inode *finode, struct file *ffile);
hi_s32 drv_jpeg_dev_close(struct inode *finode, struct file *ffile);
hi_s32 drv_jpeg_dev_suspend(struct device *dev);
hi_s32 drv_jpeg_dev_resume(struct device *dev);
hi_s32 drv_jpeg_dev_resume_early(struct device *dev);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_JPEG_INTF_H__ */
