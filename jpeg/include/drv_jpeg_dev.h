/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description:jpeg device operate function define
 */

#ifndef __DRV_JPEG_DEV_H__
#define __DRV_JPEG_DEV_H__

#include <linux/device.h>
#include "hi_osal.h"
#include "hi_drv_dev.h"
#include "drv_jpeg_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define JPEGDEVNAME "jpeg"

typedef struct {
    hi_bool is_lock;
    hi_bool is_suspend;
    hi_bool is_resume;
    hi_u32 dev_open_times;
    hi_u32 system_abnormal_times;
    osal_spinlock decode_lock;
    osal_wait wait_intrrupt;
    osal_wait wait_mutex;
    hi_jpeg_dec_state interrupt_state;
} drv_jpeg_mgr;

hi_s32 drv_jpeg_pm_register(hi_void);
hi_void drv_jpeg_pm_unregister(hi_void);
hi_slong drv_jpeg_dev_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg);
#ifdef CONFIG_COMPAT
hi_slong drv_jpeg_dev_compat_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_JPEG_DEV_H__ */
