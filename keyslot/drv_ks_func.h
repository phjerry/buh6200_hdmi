/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:KS comon macro and API.
 * Author: Linux SDK team
 * Create: 2019/10/30
 */
#ifndef __DRV_KS_FUNC_H__
#define __DRV_KS_FUNC_H__

#include <linux/uaccess.h>
#include "hi_type.h"
#include "hi_osal.h"
#include "drv_ks_define.h"
#include "hi_drv_keyslot.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

/*
 * global klad resource managation.
 */
enum ks_mgmt_state {
    KS_MGMT_CLOSED = 0x0,
    KS_MGMT_OPENED,
};
struct ks_mgmt;

struct ks_mgmt_ops {
    hi_s32(*init)(struct ks_mgmt *mgmt);
    hi_s32(*exit)(struct ks_mgmt *mgmt);

    hi_s32(*lock)(struct ks_mgmt *mgmt, hi_keyslot_type slot_ind, hi_u32 slot_num);
    hi_s32(*auto_lock)(struct ks_mgmt *mgmt, hi_keyslot_type slot_ind, hi_u32 *slot_num);
    hi_s32(*unlock)(struct ks_mgmt *mgmt, hi_keyslot_type slot_ind, hi_u32 slot_num);

    /* debug helper */
    hi_void(*show_info)(struct ks_mgmt *mgmt);

    hi_s32(*suspend)(struct ks_mgmt *mgmt);
    hi_s32(*resume)(struct ks_mgmt *mgmt);
};

struct ks_mgmt {
    osal_mutex             lock;
    enum   ks_mgmt_state   state;
    osal_atomic            ref_count;
    struct ks_mgmt_ops     *ops;

    hi_u32                 ts_slot_used;
    hi_u32                 ts_slot_num;
    DECLARE_BITMAP(ts_slot_bitmap, HI_KEYSLOT_TSCIPHER_MAX);

    hi_u32                 mc_slot_used;
    hi_u32                 mc_slot_num;
    DECLARE_BITMAP(mc_slot_bitmap, HI_KEYSLOT_MCIPHER_MAX);

    hi_bool                hmac_locked;
    hi_u32                 hmac_id;

    hi_void                *io_base;
};

hi_s32 ks_mgmt_init(hi_void);
hi_void ks_mgmt_exit(hi_void);
hi_s32 ks_mgmt_suspend(hi_void);
hi_s32 ks_mgmt_resume(hi_void);
hi_s32 ks_mgmt_lock(hi_keyslot_type slot_ind, hi_u32 slot_num);
hi_s32 ks_mgmt_auto_lock(hi_keyslot_type slot_ind, hi_u32 *slot_num);
hi_s32 ks_mgmt_unlock(hi_keyslot_type slot_ind, hi_u32 slot_num);
hi_void ks_mgmt_show(hi_void);

struct ks_mgmt *__get_ks_mgmt(hi_void);
struct ks_mgmt *get_ks_mgmt(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef __DRV_KS_FUNC_H__ */
