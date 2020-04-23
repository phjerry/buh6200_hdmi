/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drv jpeg mutex control
 */

#include "drv_jpeg_mutex.h"

hi_void jpeg_dec_mutex(osal_spinlock *lock, hi_ulong *lock_flag)
{
    if ((lock != NULL) && (lock->lock != NULL) && (lock_flag != NULL)) {
        osal_spin_lock_irqsave(lock, lock_flag);
    }
}

hi_void jpeg_dec_unmutex(osal_spinlock *lock, hi_ulong *lock_flag)
{
    if ((lock != NULL) && (lock->lock != NULL) && (lock_flag != NULL)) {
        osal_spin_unlock_irqrestore(lock, lock_flag);
    }
}
