/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drv jpeg mutex control define
 */

#ifndef __DRV_JPEG_MUTEX_H__
#define __DRV_JPEG_MUTEX_H__

#include "hi_osal.h"
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void jpeg_dec_mutex(osal_spinlock *lock, hi_ulong *lock_flag);
hi_void jpeg_dec_unmutex(osal_spinlock *lock, hi_ulong *lock_flag);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
