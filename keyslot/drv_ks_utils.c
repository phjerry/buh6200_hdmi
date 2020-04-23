/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
* Description: klad basic utils impl.
* Create: 2019-10-22
*/
#include "drv_ks_utils.h"

#include <linux/kernel.h>
#include "linux/bitmap.h"

#include "hi_drv_mem.h"
#include "drv_ks_define.h"
#include "drv_ks_func.h"

/*
 * KLAD export functions.
 */
static struct ks_session_ops g_ks_session_ops;

static inline hi_s32 get_hw_session(struct ks_session *hw_session)
{
    WARN_ON(&g_ks_session_ops != hw_session->ops);
    WARN_ON(osal_atomic_read(&hw_session->ref_count) == 0);

    if (osal_atomic_inc_return(&hw_session->ref_count) == 0) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static inline hi_void put_hw_session(struct ks_session *hw_session)
{
    WARN_ON(osal_atomic_read(&hw_session->ref_count) == 0);

    if (osal_atomic_dec_return(&hw_session->ref_count) == 0) {
        HI_DEBUG_KS("@hw_session(0x%p) released.\n", hw_session);
        hi_free(hw_session);
        hw_session = HI_NULL;
    }
}

hi_s32 ks_session_create(struct ks_session **new_hw_session)
{
    hi_s32 ret;
    struct ks_session *hw_session = HI_NULL;

    WARN_ON(new_hw_session == HI_NULL);

    hw_session = hi_malloc(sizeof(struct ks_session));
    if (hw_session == HI_NULL) {
        HI_ERR_KS("alloc hw_session memory failed.\n");
        return HI_ERR_KLAD_NO_MEMORY;
    }

    if (memset_s(hw_session, sizeof(struct ks_session), 0x0, sizeof(struct ks_session)) != EOK) {
        return HI_ERR_KS_MEM_MAP;
    }

    ret = osal_atomic_init(&hw_session->ref_count);
    if (ret != 0) {
        return HI_ERR_KS_NOT_INIT;
    }

    osal_atomic_set(&hw_session->ref_count, 1);
    osal_mutex_init(&hw_session->lock);

    hw_session->state = KS_SESSION_ACTIVED;
    hw_session->ops = &g_ks_session_ops;
    hw_session->slot_used = 0;
    hw_session->slot_num = HI_KEYSLOT_TSCIPHER_MAX + HI_KEYSLOT_MCIPHER_MAX + HI_KEYSLOT_HMAC_MAX;

    *new_hw_session = hw_session;

    HI_DEBUG_KS("@hw_session(0x%p) created.\n", hw_session);

    return HI_SUCCESS;
}

hi_s32 ks_session_add_slot(struct ks_session *hw_session, const hi_keyslot_type slot_ind, hi_u32 slot_num)
{
    hi_s32 ret;

    WARN_ON(hw_session == HI_NULL);
    WARN_ON(slot_ind >= HI_KEYSLOT_TYPE_MAX);

    ret = get_hw_session(hw_session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = hw_session->ops->add_slot(hw_session, slot_ind, slot_num);

    put_hw_session(hw_session);
out:
    return ret;
}

hi_s32 ks_session_del_slot(struct ks_session *hw_session, const hi_keyslot_type slot_ind, hi_u32 slot_num)
{
    hi_s32 ret;

    WARN_ON(hw_session == HI_NULL);
    WARN_ON(slot_ind >= HI_KEYSLOT_TYPE_MAX);

    ret = get_hw_session(hw_session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = hw_session->ops->del_slot(hw_session, slot_ind, slot_num);

    put_hw_session(hw_session);
out:
    return ret;
}

hi_s32 ks_session_destroy(struct ks_session *hw_session)
{
    hi_s32 ret;

    WARN_ON(hw_session == HI_NULL);

    ret = get_hw_session(hw_session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* close the mgmt depend on release all resource firstly. */
    hw_session->state = KS_SESSION_INACTIVED;
    hw_session->ops->release(hw_session);

    hw_session->ops = HI_NULL;

    ret = osal_atomic_dec_return(&hw_session->ref_count);
    if (ret != 0) {
        ret = HI_FAILURE;
        goto out;
    }

    put_hw_session(hw_session);

    osal_atomic_destory(&hw_session->ref_count);
    osal_mutex_destory(&hw_session->lock);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 ks_session_add_slot_impl(struct ks_session *hw_session, const hi_keyslot_type slot_ind, hi_u32 slot_num)
{
    hi_s32 ret;
    hi_u32 id;

    __mutex_lock(&hw_session->lock);

    if (hw_session->state != KS_SESSION_ACTIVED) {
        ret = HI_FAILURE;
        goto out;
    }

    if (((slot_ind == HI_KEYSLOT_TYPE_TSCIPHER) && (slot_num >= HI_KEYSLOT_TSCIPHER_MAX)) ||
        ((slot_ind == HI_KEYSLOT_TYPE_MCIPHER) && (slot_num >= HI_KEYSLOT_MCIPHER_MAX)) ||
        ((slot_ind == HI_KEYSLOT_TYPE_HMAC) && (slot_num >= HI_KEYSLOT_MCIPHER_MAX + HI_KEYSLOT_HMAC_MAX))) {
        ret = HI_ERR_KS_INVALID_PARAM;
        goto out;
    }
    if (slot_ind == HI_KEYSLOT_TYPE_TSCIPHER) {
        id = slot_num;
    } else if (slot_ind == HI_KEYSLOT_TYPE_MCIPHER) {
        id = HI_KEYSLOT_TSCIPHER_MAX + slot_num;
    } else if (slot_ind == HI_KEYSLOT_TYPE_HMAC) {
        id = HI_KEYSLOT_TSCIPHER_MAX + HI_KEYSLOT_MCIPHER_MAX + slot_num;
    } else {
        ret = HI_ERR_KS_INVALID_PARAM;
        goto out;
    }
    set_bit(id, hw_session->slot_bitmap);
    hw_session->slot_used++;
    HI_DEBUG_KS("@hw_session(0x%p) add slot %d, total %d.\n", hw_session, id, hw_session->slot_used);
    ret = HI_SUCCESS;

out:
    __mutex_unlock(&hw_session->lock);

    return ret;
}

static hi_s32 ks_session_del_slot_impl(struct ks_session *hw_session, const hi_keyslot_type slot_ind, hi_u32 slot_num)
{
    hi_s32 ret;
    hi_u32 id;
    unsigned long mask;
    unsigned long *p = HI_NULL;

    __mutex_lock(&hw_session->lock);
    if (hw_session->state != KS_SESSION_ACTIVED) {
        ret = HI_FAILURE;
        goto out;
    }

    if (((slot_ind == HI_KEYSLOT_TYPE_TSCIPHER) && (slot_num >= HI_KEYSLOT_TSCIPHER_MAX)) ||
        ((slot_ind == HI_KEYSLOT_TYPE_MCIPHER) && (slot_num >= HI_KEYSLOT_MCIPHER_MAX)) ||
        ((slot_ind == HI_KEYSLOT_TYPE_HMAC) && (slot_num >= HI_KEYSLOT_MCIPHER_MAX + HI_KEYSLOT_HMAC_MAX))) {
        ret = HI_ERR_KS_INVALID_PARAM;
        goto out;
    }
    if (slot_ind == HI_KEYSLOT_TYPE_TSCIPHER) {
        id = slot_num;
    } else if (slot_ind == HI_KEYSLOT_TYPE_MCIPHER) {
        id = HI_KEYSLOT_TSCIPHER_MAX + slot_num;
    } else if (slot_ind == HI_KEYSLOT_TYPE_HMAC) {
        id = HI_KEYSLOT_TSCIPHER_MAX + HI_KEYSLOT_MCIPHER_MAX + slot_num;
    } else {
        ret = HI_ERR_KS_INVALID_PARAM;
        goto out;
    }

    mask = BIT_MASK(id);
    p = ((unsigned long *)hw_session->slot_bitmap) + BIT_WORD(id);
    if (!(*p & mask)) {
        HI_ERR_KS("invalid slot table id(%d).\n", id);
        ret = HI_ERR_KS_INVALID_PARAM;
        goto out;
    }

    clear_bit(id, hw_session->slot_bitmap);
    hw_session->slot_used--;
    HI_DEBUG_KS("@hw_session(0x%p) del slot %d, total %d.\n", hw_session, slot_num, hw_session->slot_used);
    ret = HI_SUCCESS;
out:
    __mutex_unlock(&hw_session->lock);

    return ret;
}

static hi_void ks_session_unlock_hw(hi_s32 nn)
{
    hi_s32 ret;
    hi_keyslot_type slot_ind;
    hi_u32 slot_num;

    if (nn > HI_KEYSLOT_TSCIPHER_MAX + HI_KEYSLOT_MCIPHER_MAX + HI_KEYSLOT_HMAC_MAX) {
        print_err_hex(nn);
        return;
    }

    if (nn > HI_KEYSLOT_TSCIPHER_MAX + HI_KEYSLOT_MCIPHER_MAX) {
        slot_ind = HI_KEYSLOT_TYPE_HMAC;
        slot_num = nn - HI_KEYSLOT_TSCIPHER_MAX - HI_KEYSLOT_MCIPHER_MAX;
    } else if (nn > HI_KEYSLOT_TSCIPHER_MAX) {
        slot_ind = HI_KEYSLOT_TYPE_MCIPHER;
        slot_num = nn - HI_KEYSLOT_TSCIPHER_MAX;
    } else {
        slot_ind = HI_KEYSLOT_TYPE_TSCIPHER;
        slot_num = nn;
    }
    ret = ks_mgmt_unlock(slot_ind, slot_num);
    if (ret != HI_SUCCESS) {
        print_err_func_hex3(ks_mgmt_unlock, slot_ind, slot_num, ret);
    }
}

static hi_void ks_session_release_impl(struct ks_session *hw_session)
{
    hi_s32 nn = -1; /* initial value */

    WARN_ON(&g_ks_session_ops != hw_session->ops);
    WARN_ON(hw_session->state != KS_SESSION_INACTIVED);

    __mutex_lock(&hw_session->lock);

    while ((nn = find_next_bit(hw_session->slot_bitmap, hw_session->slot_num,
        nn + 1)) < hw_session->slot_num) {
        ks_session_unlock_hw(nn);
        clear_bit(nn, hw_session->slot_bitmap);
        hw_session->slot_used--;
        HI_DEBUG_KS("@hw_session(0x%p) release slot %d, total %d.\n", hw_session, nn, hw_session->slot_used);
    }
    __mutex_unlock(&hw_session->lock);
}

static struct ks_session_ops g_ks_session_ops = {
    .add_slot  = ks_session_add_slot_impl,
    .del_slot  = ks_session_del_slot_impl,
    .release   = ks_session_release_impl,
};

