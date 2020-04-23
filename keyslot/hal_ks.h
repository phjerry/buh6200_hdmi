/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:KS driver in register level.
 * Author: Linux SDK team
 * Create: 2019/06/22
 */
#ifndef __HAL_KS_H__
#define __HAL_KS_H__

#include "drv_ks_define.h"
#include "drv_ioctl_ks.h"
#include "hal_ks_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    KS_STAT_UN_LOCK     = 0x0,
    KS_STAT_REE_LOCK    = 0x1,
    KS_STAT_TEE_LOCK    = 0x2,
    KS_STAT_HPP_LOCK    = 0x3,
    KS_STAT_MAX
} ks_slot_stat;

hi_s32 hal_ks_init(hi_void);
hi_void hal_ks_deinit(hi_void);
hi_s32 hal_ks_lock(const hi_keyslot_type slot_ind, const hi_u32 slot_num);
hi_s32 hal_ks_unlock(const hi_keyslot_type slot_ind, const hi_u32 slot_num);
ks_slot_stat hal_ks_status(const hi_keyslot_type slot_ind, const hi_u32 slot_num);

#ifdef __cplusplus
}
#endif
#endif /* __HAL_KS_H__ */
