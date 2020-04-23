/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header file of drv_dvfs_proc
 */

#ifndef __DRV_DVFS_PROC_H__
#define __DRV_DVFS_PROC_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 dvfs_register_proc(hi_void);
hi_void dvfs_remove_proc(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __DRV_DVFS_PROC_H__ */
