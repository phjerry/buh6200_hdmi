/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description:
 * Author: xuming
 * Create: 2019-5-16
 */

#ifndef __DRV_PROC_H__
#define __DRV_PROC_H__

#include "hi_type.h"

#ifdef HI_PROC_SUPPORT
hi_s32  drv_proc_init(hi_void);
hi_void drv_proc_exit(hi_void);
#else
static inline hi_s32 drv_proc_init(hi_void)
{
    return HI_SUCCESS;
}

static inline hi_void drv_proc_exit(hi_void)
{
}
#endif

#endif  /* __DRV_PROC_H__ */

