/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:Key slot driver.Provide all the kernel API and ioctl API.
 * Author : Linux SDK team
 * Create: 2019/10/30
 */

#include "drv_ks_func.h"

#include <linux/mm.h>

#include "linux/hisilicon/securec.h"
#include "hi_osal.h"
#include "drv_ioctl_ks.h"
#include "hal_ks.h"
#include "hi_drv_keyslot.h"

hi_s32 ks_mgmt_init(hi_void)
{
    struct ks_mgmt *mgmt = __get_ks_mgmt();

    return mgmt->ops->init(mgmt);
}

hi_void ks_mgmt_exit(hi_void)
{
    struct ks_mgmt *mgmt = get_ks_mgmt();

    if (mgmt->ops->exit(mgmt) != HI_SUCCESS) {
        mgmt->ops->show_info(mgmt);

        /*
         * hw_session must release all resource.
         */
        HI_FATAL_KS("ks mgmt exit with error!\n");
    }
}

hi_s32 ks_mgmt_suspend(hi_void)
{
    struct ks_mgmt *mgmt = __get_ks_mgmt();

    return mgmt->ops->suspend(mgmt);
}

hi_s32 ks_mgmt_resume(hi_void)
{
    struct ks_mgmt *mgmt = __get_ks_mgmt();

    return mgmt->ops->resume(mgmt);
}

hi_s32 ks_mgmt_lock(hi_keyslot_type slot_ind, hi_u32 slot_num)
{
    struct ks_mgmt *mgmt = __get_ks_mgmt();

    return mgmt->ops->lock(mgmt, slot_ind, slot_num);
}

hi_s32 ks_mgmt_auto_lock(hi_keyslot_type slot_ind, hi_u32 *slot_num)
{
    struct ks_mgmt *mgmt = __get_ks_mgmt();

    return mgmt->ops->auto_lock(mgmt, slot_ind, slot_num);
}

hi_s32 ks_mgmt_unlock(hi_keyslot_type slot_ind, hi_u32 slot_num)
{
    struct ks_mgmt *mgmt = __get_ks_mgmt();

    return mgmt->ops->unlock(mgmt, slot_ind, slot_num);
}

hi_void ks_mgmt_show(hi_void)
{
    struct ks_mgmt *mgmt = __get_ks_mgmt();

    mgmt->ops->show_info(mgmt);
}

static hi_s32 __ks_mgmt_init_impl(struct ks_mgmt *mgmt)
{
    WARN_ON(osal_atomic_read(&mgmt->ref_count) != 0);
    WARN_ON(mgmt->state != KS_MGMT_CLOSED);

    mgmt->ts_slot_used = 0;
    mgmt->ts_slot_num = HI_KEYSLOT_TSCIPHER_MAX;
    bitmap_zero(mgmt->ts_slot_bitmap, mgmt->ts_slot_num);

    mgmt->mc_slot_used = 0;
    mgmt->mc_slot_num = HI_KEYSLOT_MCIPHER_MAX;
    bitmap_zero(mgmt->mc_slot_bitmap, mgmt->mc_slot_num);

    mgmt->hmac_locked = HI_FALSE;

    mgmt->io_base = osal_ioremap_nocache(KC_REG_BASE, KC_REG_RANGE);
    if (mgmt->io_base == HI_NULL) {
        return HI_ERR_KS_MEM_MAP;
    }

    mgmt->state = KS_MGMT_OPENED;

    return HI_SUCCESS;
}

static hi_s32 ks_mgmt_init_impl(struct ks_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_init(&mgmt->lock);

    __mutex_lock(&mgmt->lock);

    if (osal_atomic_init(&mgmt->ref_count) == HI_FAILURE) {
        goto out;
    }

    if (mgmt->state == KS_MGMT_CLOSED) {
        ret = __ks_mgmt_init_impl(mgmt);
        if (ret == HI_SUCCESS) {
            if (osal_atomic_inc_return(&mgmt->ref_count) == HI_FAILURE) {
                ret = HI_FAILURE;
                goto out;
            }
        }
    } else if (mgmt->state == KS_MGMT_OPENED) {
        WARN_ON(osal_atomic_read(&mgmt->ref_count) == 0);

        if (osal_atomic_inc_return(&mgmt->ref_count) == HI_FAILURE) {
            ret = HI_FAILURE;
            goto out;
        }

        ret = HI_SUCCESS;
    } else {
        HI_FATAL_KS("mgmt state unknown, mgmt state is:%u\n", mgmt->state);
    }

out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 __ks_mgmt_exit_impl(struct ks_mgmt *mgmt)
{
    WARN_ON(osal_atomic_read(&mgmt->ref_count) != 1);
    WARN_ON(mgmt->state != KS_MGMT_OPENED);

    osal_iounmap(mgmt->io_base);

    mgmt->state = KS_MGMT_CLOSED;

    return HI_SUCCESS;
}

static hi_s32 __ks_mgmt_rel_and_exit(struct ks_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;

    if (mgmt->state == KS_MGMT_OPENED) {
        WARN_ON(osal_atomic_read(&mgmt->ref_count) == 0);

        if (osal_atomic_read(&mgmt->ref_count) == 1) {
            ret = __ks_mgmt_exit_impl(mgmt);
            if (ret != HI_SUCCESS) {
                goto out;
            }

            if (osal_atomic_dec_return(&mgmt->ref_count) == HI_FAILURE) {
                ret = HI_FAILURE;
                goto out;
            }
        } else {
            if (osal_atomic_dec_return(&mgmt->ref_count) == HI_FAILURE) {
                ret = HI_FAILURE;
                goto out;
            }

            ret = HI_SUCCESS;
        }
    } else if (mgmt->state == KS_MGMT_CLOSED) {
        WARN_ON(osal_atomic_read(&mgmt->ref_count));

        ret = HI_SUCCESS;
    } else {
        HI_FATAL_KS("mgmt state unknown, mgmt state is:%u\n", mgmt->state);
    }

    osal_atomic_destory(&mgmt->ref_count);

out:
    return ret;
}

#define TEN_MSECS 10

static hi_s32 ks_mgmt_exit_impl(struct ks_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long start = osal_get_jiffies();
    unsigned long end = start + HZ; /* 1s */

    do {
        __mutex_lock(&mgmt->lock);

        ret = __ks_mgmt_rel_and_exit(mgmt);

        __mutex_unlock(&mgmt->lock);

        osal_msleep_uninterruptible(TEN_MSECS);
    } while (ret != HI_SUCCESS && time_in_range((unsigned long)osal_get_jiffies(), start, end));

    osal_mutex_destory(&mgmt->lock);
    return ret;
}

static hi_s32 __ks_mgmt_add_slot(struct ks_mgmt *mgmt, const hi_keyslot_type slot_ind, hi_u32 slot_num)
{
    if (slot_ind == HI_KEYSLOT_TYPE_TSCIPHER) {
        set_bit(slot_num, mgmt->ts_slot_bitmap);
        mgmt->ts_slot_used++;
    } else if (slot_ind == HI_KEYSLOT_TYPE_MCIPHER) {
        set_bit(slot_num, mgmt->mc_slot_bitmap);
        mgmt->mc_slot_used++;
    } else if (slot_ind == HI_KEYSLOT_TYPE_HMAC) {
        mgmt->hmac_locked = HI_TRUE;
        mgmt->hmac_id = slot_num;
    } else {
        return HI_ERR_KS_INVALID_PARAM;
    }
    return HI_SUCCESS;
}

static hi_s32 __ks_mgmt_del_slot(struct ks_mgmt *mgmt, const hi_keyslot_type slot_ind, hi_u32 slot_num)
{
    unsigned long mask;
    unsigned long *p = HI_NULL;

    mask = BIT_MASK(slot_num);

    if (slot_ind == HI_KEYSLOT_TYPE_TSCIPHER) {
        p = ((unsigned long *)mgmt->ts_slot_bitmap) + BIT_WORD(slot_num);
        if (!(*p & mask)) {
            HI_ERR_KS("invalid slot table id(%d).\n", slot_num);
            return HI_ERR_KS_INVALID_PARAM;
        }
        clear_bit(slot_num, mgmt->ts_slot_bitmap);
        mgmt->ts_slot_used--;
    } else if (slot_ind == HI_KEYSLOT_TYPE_MCIPHER) {
        p = ((unsigned long *)mgmt->mc_slot_bitmap) + BIT_WORD(slot_num);
        if (!(*p & mask)) {
            HI_ERR_KS("invalid slot table id(%d).\n", slot_num);
            return HI_ERR_KS_INVALID_PARAM;
        }
        clear_bit(slot_num, mgmt->mc_slot_bitmap);
        mgmt->mc_slot_used--;
    } else if (slot_ind == HI_KEYSLOT_TYPE_HMAC) {
        mgmt->hmac_locked = HI_FALSE;
        mgmt->hmac_id = 0;
    } else {
        return HI_ERR_KS_INVALID_PARAM;
    }
    return HI_SUCCESS;
}

static hi_s32 __drv_ks_slot_check(const hi_keyslot_type slot_ind, const hi_u32 slot_num)
{
    if (slot_ind >= HI_KEYSLOT_TYPE_MAX) {
        return HI_ERR_KS_INVALID_PARAM;
    }
    if (slot_ind == HI_KEYSLOT_TYPE_TSCIPHER) {
        if (slot_num >= HI_KEYSLOT_TSCIPHER_MAX) {
            return HI_ERR_KS_INVALID_PARAM;
        }
    } else if(slot_ind == HI_KEYSLOT_TYPE_MCIPHER) {
        if (slot_num >= HI_KEYSLOT_MCIPHER_MAX) {
            return HI_ERR_KS_INVALID_PARAM;
        }
    } else {
        if ((slot_num < HI_KEYSLOT_MCIPHER_MAX) || (slot_num >= HI_KEYSLOT_MCIPHER_MAX + HI_KEYSLOT_HMAC_MAX)) {
            return HI_ERR_KS_INVALID_PARAM;
        }
    }
    return HI_SUCCESS;
}

static hi_s32 __drv_ks_unlock(const hi_keyslot_type slot_ind, const hi_u32 slot_num)
{
    hi_s32 ret;
    ks_slot_stat state;

    ret = __drv_ks_slot_check(slot_ind, slot_num);
    if (ret != HI_SUCCESS) {
        print_err_func_hex3(__drv_ks_slot_check, slot_ind, slot_num, ret);
        return ret;
    }
    state = hal_ks_status(slot_ind, slot_num);
#ifdef HI_TEE_SUPPORT
    if (state != KS_STAT_REE_LOCK) {
        print_err_hex4(slot_ind, slot_num, state, HI_ERR_KS_LOCKED_CPUX + state);
        return HI_ERR_KS_LOCKED_CPUX + state;
    }
#else
    if (state != KS_STAT_TEE_LOCK) {
        print_err_hex4(slot_ind, slot_num, state, HI_ERR_KS_LOCKED_CPUX + state);
        return HI_ERR_KS_LOCKED_CPUX + state;
    }
#endif
    return hal_ks_unlock(slot_ind, slot_num);
}

static hi_s32 __drv_ks_auto_lock(const hi_keyslot_type slot_ind, hi_u32 *slot_num)
{
    hi_u32 slot_end;
    hi_u32 start_slot = 0;

    if (slot_num == HI_NULL) {
        print_err_code(HI_ERR_KS_PTR_NULL);
        return HI_ERR_KS_PTR_NULL;
    }
    if (slot_ind >= HI_KEYSLOT_TYPE_MAX) {
        print_err_hex2(slot_ind, HI_ERR_KS_INVALID_PARAM);
        return HI_ERR_KS_INVALID_PARAM;
    }

    if (slot_ind == HI_KEYSLOT_TYPE_TSCIPHER) {
        slot_end = start_slot + HI_KEYSLOT_TSCIPHER_MAX;
    } else if (slot_ind == HI_KEYSLOT_TYPE_MCIPHER) {
        slot_end = start_slot + HI_KEYSLOT_MCIPHER_MAX;
    } else {
        start_slot = HI_KEYSLOT_MCIPHER_MAX;
        slot_end = start_slot + HI_KEYSLOT_HMAC_MAX;
    }

    for (; start_slot < slot_end ; start_slot++) {
        if (hal_ks_status(slot_ind, start_slot) != KS_STAT_UN_LOCK) {
            continue;
        }
        if (hal_ks_lock(slot_ind, start_slot) != HI_SUCCESS) {
            continue;
        }
        *slot_num = start_slot;
        return HI_SUCCESS;
    }
    print_err_hex2(start_slot, HI_ERR_KS_AUTO_LOCK_FAILED);
    return HI_ERR_KS_AUTO_LOCK_FAILED;
}

static hi_s32 __drv_ks_lock(const hi_keyslot_type slot_ind, hi_u32 slot_num)
{
    hi_s32 ret;
    ks_slot_stat state;

    ret = __drv_ks_slot_check(slot_ind, slot_num);
    if (ret != HI_SUCCESS) {
        print_err_func_hex3(__drv_ks_slot_check, slot_ind, slot_num, ret);
        return ret;
    }
    if (slot_ind >= HI_KEYSLOT_TYPE_MAX) {
        print_err_hex3(slot_ind, slot_num, HI_ERR_KS_INVALID_PARAM);
        return HI_ERR_KS_INVALID_PARAM;
    }

    state = hal_ks_status(slot_ind, slot_num);
    if (state != KS_STAT_UN_LOCK) {
        print_err_func_hex3(hal_ks_status, slot_ind, slot_num, state);
        return HI_ERR_KS_LOCKED_CPUX + state;
    }
    return hal_ks_lock(slot_ind, slot_num);
}

hi_s32 ks_mgmt_lock_impl(struct ks_mgmt *mgmt, hi_keyslot_type slot_ind, hi_u32 slot_num)
{
    hi_s32 ret;

    __mutex_lock(&mgmt->lock);

    if (mgmt->state != KS_MGMT_OPENED) {
        ret = HI_ERR_KS_NOT_INIT;
        print_err_hex2(mgmt->state, ret);
        goto out;
    }

    ret = __drv_ks_lock(slot_ind, slot_num);
    if (ret != HI_SUCCESS) {
        print_err_func_hex3(__drv_ks_lock, slot_ind, slot_num, ret);
        goto out;
    }

    ret = __ks_mgmt_add_slot(mgmt, slot_ind, slot_num);

out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 ks_mgmt_auto_lock_impl(struct ks_mgmt *mgmt, hi_keyslot_type slot_ind, hi_u32 *slot_num)
{
    hi_s32 ret;

    __mutex_lock(&mgmt->lock);

    if (mgmt->state != KS_MGMT_OPENED) {
        ret = HI_ERR_KS_NOT_INIT;
        print_err_hex2(mgmt->state, ret);
        goto out;
    }

    ret = __drv_ks_auto_lock(slot_ind, slot_num);
    if (ret != HI_SUCCESS) {
        print_err_func_hex3(__drv_ks_auto_lock, slot_ind, *slot_num, ret);
        goto out;
    }
    ret = __ks_mgmt_add_slot(mgmt, slot_ind, *slot_num);
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 ks_mgmt_unlock_impl(struct ks_mgmt *mgmt, hi_keyslot_type slot_ind, hi_u32 slot_num)
{
    hi_s32 ret;

    __mutex_lock(&mgmt->lock);

    if (mgmt->state != KS_MGMT_OPENED) {
        ret = HI_ERR_KS_NOT_INIT;
        print_err_hex2(mgmt->state, ret);
        goto out;
    }

    ret = __drv_ks_unlock(slot_ind, slot_num);
    if (ret != HI_SUCCESS) {
        print_err_func_hex3(__drv_ks_unlock, slot_ind, slot_num, ret);
        goto out;
    }
    ret = __ks_mgmt_del_slot(mgmt, slot_ind, slot_num);
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_void __ks_mgmt_show_info_impl(struct ks_mgmt *mgmt)
{
    hi_s32 nn = -1; /* initial value */

    HI_PRINT("SW OBJ\n");
    while ((nn = find_next_bit(mgmt->ts_slot_bitmap, mgmt->ts_slot_num,
        nn + 1)) < mgmt->ts_slot_num) {
        HI_PRINT("TSCIPHER: index=%03d locked", nn);
    }

    nn = -1;
    while ((nn = find_next_bit(mgmt->mc_slot_bitmap, mgmt->mc_slot_num,
        nn + 1)) < mgmt->mc_slot_num) {
        HI_PRINT("MCIPHER : index=%03d locked", nn);
    }

    if (mgmt->hmac_locked == HI_TRUE) {
        HI_PRINT("HMAC     : index=%03d locked", mgmt->hmac_id);
    }
}

static hi_void ks_mgmt_show_info_impl(struct ks_mgmt *mgmt)
{
    __mutex_lock(&mgmt->lock);

    if (mgmt->state != KS_MGMT_OPENED) {
        print_err_hex(mgmt->state);
        goto out;
    }

    if (osal_atomic_inc_return(&mgmt->ref_count) == HI_FAILURE) {
        goto out;
    }

    __ks_mgmt_show_info_impl(mgmt);

    if (osal_atomic_dec_return(&mgmt->ref_count) == HI_FAILURE) {
        goto out;
    }

out:
    __mutex_unlock(&mgmt->lock);
}

static hi_s32 __ks_mgmt_suspend_impl(struct ks_mgmt *mgmt)
{
    hi_s32 ret;
    hi_s32 nn = -1; /* initial value */

    while ((nn = find_next_bit(mgmt->ts_slot_bitmap, mgmt->ts_slot_num,
        nn + 1)) < mgmt->ts_slot_num) {
        ret = __drv_ks_unlock(HI_KEYSLOT_TYPE_TSCIPHER, nn);
        if (ret != HI_SUCCESS) {
            print_err_hex3(HI_KEYSLOT_TYPE_TSCIPHER, nn, ret);
        }
    }

    nn = -1;
    while ((nn = find_next_bit(mgmt->mc_slot_bitmap, mgmt->mc_slot_num,
        nn + 1)) < mgmt->mc_slot_num) {
        ret = __drv_ks_unlock(HI_KEYSLOT_TYPE_MCIPHER, nn);
        if (ret != HI_SUCCESS) {
            print_err_hex3(HI_KEYSLOT_TYPE_MCIPHER, nn, ret);
        }
    }

    if (mgmt->hmac_locked == HI_TRUE) {
        ret = __drv_ks_unlock(HI_KEYSLOT_TYPE_HMAC, mgmt->hmac_id);
        if (ret != HI_SUCCESS) {
            print_err_hex3(HI_KEYSLOT_TYPE_HMAC, mgmt->hmac_id, ret);
        }
    }

    return HI_SUCCESS;
}

static hi_s32 ks_mgmt_suspend_impl(struct ks_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;

    __mutex_lock(&mgmt->lock);

    if (mgmt->state != KS_MGMT_OPENED) {
        ret = HI_ERR_KS_NOT_INIT;
        print_err_hex2(mgmt->state, ret);
        goto out;
    }

    if (osal_atomic_inc_return(&mgmt->ref_count) == HI_FAILURE) {
        goto out;
    }

    ret = __ks_mgmt_suspend_impl(mgmt);

    if (osal_atomic_dec_return(&mgmt->ref_count) == HI_FAILURE) {
        goto out;
    }

out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 __ks_mgmt_resume_impl(struct ks_mgmt *mgmt)
{
    hi_s32 ret;
    hi_s32 nn = -1; /* initial value */

    while ((nn = find_next_bit(mgmt->ts_slot_bitmap, mgmt->ts_slot_num,
        nn + 1)) < mgmt->ts_slot_num) {
        ret = __drv_ks_lock(HI_KEYSLOT_TYPE_TSCIPHER, nn);
        if (ret != HI_SUCCESS) {
            print_err_hex3(HI_KEYSLOT_TYPE_TSCIPHER, nn, ret);
        }
    }

    nn = -1;
    while ((nn = find_next_bit(mgmt->mc_slot_bitmap, mgmt->mc_slot_num,
        nn + 1)) < mgmt->mc_slot_num) {
        ret = __drv_ks_lock(HI_KEYSLOT_TYPE_MCIPHER, nn);
        if (ret != HI_SUCCESS) {
            print_err_hex3(HI_KEYSLOT_TYPE_MCIPHER, nn, ret);
        }
    }

    if (mgmt->hmac_locked == HI_TRUE) {
        ret = __drv_ks_lock(HI_KEYSLOT_TYPE_HMAC, mgmt->hmac_id);
        if (ret != HI_SUCCESS) {
            print_err_hex3(HI_KEYSLOT_TYPE_HMAC, mgmt->hmac_id, ret);
        }
    }

    return HI_SUCCESS;
}

static hi_s32 ks_mgmt_resume_impl(struct ks_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;

    __mutex_lock(&mgmt->lock);

    if (mgmt->state != KS_MGMT_OPENED) {
        ret = HI_ERR_KS_NOT_INIT;
        print_err_hex2(mgmt->state, ret);
        goto out;
    }

    if (osal_atomic_inc_return(&mgmt->ref_count) == HI_FAILURE) {
        goto out;
    }

    ret = __ks_mgmt_resume_impl(mgmt);

    if (osal_atomic_dec_return(&mgmt->ref_count) == HI_FAILURE) {
        goto out;
    }

out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static struct ks_mgmt_ops g_ks_mgmt_ops = {
    .init                = ks_mgmt_init_impl,
    .exit                = ks_mgmt_exit_impl,

    .lock                = ks_mgmt_lock_impl,
    .auto_lock           = ks_mgmt_auto_lock_impl,
    .unlock              = ks_mgmt_unlock_impl,

    .show_info           = ks_mgmt_show_info_impl,

    .suspend             = ks_mgmt_suspend_impl,
    .resume              = ks_mgmt_resume_impl,

};

static struct ks_mgmt g_ks_mgmt = {
    .lock      = {0},
    .ref_count = {0},
    .state     = KS_MGMT_CLOSED,
    .ops       = &g_ks_mgmt_ops,
};

struct ks_mgmt *__get_ks_mgmt(hi_void)
{
    return &g_ks_mgmt;
}

struct ks_mgmt *get_ks_mgmt(hi_void)
{
    struct ks_mgmt *mgmt = __get_ks_mgmt();

    WARN_ON(osal_atomic_read(&mgmt->ref_count) == 0);

    return mgmt;
}

