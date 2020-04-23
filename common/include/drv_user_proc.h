/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:
 * Author: xuming
 * Create: 2019-5-14
 */

#ifndef __DRV_USER_PROC_H__
#define __DRV_USER_PROC_H__

#include "hi_type.h"

#ifdef HI_PROC_SUPPORT
hi_s32  drv_user_proc_init(hi_void);
hi_void drv_user_proc_exit(hi_void);
#else
static inline hi_s32 drv_user_proc_init(hi_void)
{
    return HI_SUCCESS;
}

static inline hi_void drv_user_proc_exit(hi_void)
{
    return;
}
#endif

hi_s32 drv_user_proc_open(hi_void *file);
hi_s32 drv_user_proc_close(hi_void *file);
hi_s32 drv_user_proc_ioctl(hi_u32 cmd, hi_void *arg, hi_void *file);

#ifdef CONFIG_COMPAT
hi_s32 drv_user_proc_compat_ioctl(hi_u32 cmd, hi_void *arg, hi_void *file);
#endif

#endif  /* __DRV_USER_PROC_H__ */

