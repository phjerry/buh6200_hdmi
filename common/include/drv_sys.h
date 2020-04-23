/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:
 * Author: xuming
 * Create: 2019-5-14
 */

#ifndef __DRV_SYS_H__
#define __DRV_SYS_H__

#include "hi_type.h"

#ifdef HI_PROC_SUPPORT
hi_s32  drv_sys_add_proc(hi_void);
hi_void drv_sys_remove_proc(hi_void);
#else
static inline hi_s32 drv_sys_add_proc(hi_void)
{
    return HI_SUCCESS;
}

static inline hi_void drv_sys_remove_proc(hi_void)
{
}
#endif

hi_s32 drv_sys_init(hi_void);
hi_void drv_sys_exit(hi_void);
hi_s32 drv_sys_ioctl(hi_u32 cmd, hi_void *arg, hi_void *private_data);

#endif  /* __DRV_SYS_H__ */

