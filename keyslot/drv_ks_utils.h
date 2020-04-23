/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
* Description: keyslot basic utils impl.
* Create: 2019-10-22
*/

#ifndef __DRV_KS_UTILS_H__
#define __DRV_KS_UTILS_H__

#include <linux/mutex.h>

#include "hi_osal.h"
#include "hi_drv_keyslot.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

struct ks_session;

/*
 * handles associated with the session.
 */
enum  ks_session_state {
    KS_SESSION_INACTIVED = 0x0,  /* support remove slot only. */
    KS_SESSION_ACTIVED,          /* support add or remove slot. */
};

struct ks_session_ops {
    hi_s32(*add_slot)(struct ks_session *session, const hi_keyslot_type slot_ind, hi_u32 slot_num);
    hi_s32(*del_slot)(struct ks_session *session, const hi_keyslot_type slot_ind, hi_u32 slot_num);
    hi_void(*release)(struct ks_session *session);  /* release all R obj from session. */
    hi_s32(*suspend)(struct ks_session *session);
    hi_s32(*resume)(struct ks_session *session);
};

struct ks_session {
    osal_atomic                  ref_count;
    osal_mutex                   lock;
    enum   ks_session_state      state;

    hi_u32                       slot_used;
    hi_u32                       slot_num;
    DECLARE_BITMAP(slot_bitmap, HI_KEYSLOT_TSCIPHER_MAX + HI_KEYSLOT_MCIPHER_MAX + HI_KEYSLOT_HMAC_MAX);

    struct ks_session_ops   *ops;
};

hi_s32  ks_session_create(struct ks_session **new_hw_session);
hi_s32  ks_session_add_slot(struct ks_session *hw_session, const hi_keyslot_type slot_ind, hi_u32 slot_num);
hi_s32  ks_session_del_slot(struct ks_session *hw_session, const hi_keyslot_type slot_ind, hi_u32 slot_num);
hi_s32  ks_session_destroy(struct ks_session *hw_session);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_KS_UTILS_H__

