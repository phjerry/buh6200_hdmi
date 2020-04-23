/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
 * Description :Add proc node in virtual fs.
 * Author : Linux SDK team
 * Created : 2019-06-21
 */
#ifndef __DRV_KS_PROC_H__
#define __DRV_KS_PROC_H__

#include "hi_drv_proc.h"
#include "drv_ks_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 ks_register_proc(hi_void);
hi_void ks_remove_proc(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
