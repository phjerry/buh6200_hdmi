/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description:
 * Author:
 * Created: 2013-12-16
 */
#ifndef __DRV_FRONTEND_EXT_H__
#define __DRV_FRONTEND_EXT_H__

#include "hi_type.h"
#ifdef __KERNEL__
#include "hi_drv_dev.h"
#endif

hi_s32 fe_drv_module_init(hi_void);
hi_void fe_drv_module_exit(hi_void);

#endif

