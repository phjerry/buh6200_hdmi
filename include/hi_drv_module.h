/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2012/6/19
 */

#ifndef __HI_DRV_MODULE_H__
#define __HI_DRV_MODULE_H__

#include "hi_type.h"
#include "hi_module.h"

hi_s32 hi_drv_module_unregister(const hi_u32 module_id);
hi_s32 hi_drv_module_get_func(const hi_u32 module_id, hi_void **func);

#ifdef __KERNEL__

#include <linux/seq_file.h>

hi_s32 hi_drv_module_register(const hi_u32 module_id, const hi_char *name, hi_void *func, struct file *file);

#endif

#endif /* __HI_DRV_MODULE_H__ */

