/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2009-12-17
 */

#ifndef __DRV_MODULE_H__
#define __DRV_MODULE_H__

#include <linux/fs.h>

#include "hi_type.h"

hi_s32 drv_module_init(hi_void);
hi_void drv_module_exit(hi_void);
hi_s32 drv_module_ioctl(hi_u32 cmd, hi_void *arg, hi_void *private_data);

#ifdef HI_PROC_SUPPORT
hi_s32  drv_module_add_proc(hi_void);
hi_void drv_module_remove_proc(hi_void);
#else
static inline hi_s32 drv_module_add_proc(hi_void)
{
    return HI_SUCCESS;
}

static inline hi_void drv_module_remove_proc(hi_void)
{
}
#endif

#endif  /* __DRV_MODULE_H__ */

