/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:KS driver in register level.
 * Author: Linux SDK team
 * Create: 2019/06/22
 */
#include "hal_ks.h"

#include "hi_osal.h"
#include "drv_ks_define.h"
#include "drv_ks_func.h"
#include "hal_ks_reg.h"


static hi_void _ks_write_reg(const hi_u32 addr, const hi_u32 val)
{
    struct ks_mgmt *mgmt = __get_ks_mgmt();

    reg_write((mgmt->io_base + (addr - KC_REG_BASE)), val);
    HI_INFO_KS("ks w 0x%08x 0x%08x\n", addr, val);
    return;
}

static hi_u32 _ks_read_reg(const hi_u32 addr)
{
    hi_u32 val = 0;
    struct ks_mgmt *mgmt = __get_ks_mgmt();

    reg_read((mgmt->io_base + (addr - KC_REG_BASE)), val);
    HI_INFO_KS("ks r 0x%08x 0x%08x\n", addr, val);
    return val;
}

static hi_u32 _ks_get_flush_status(hi_void)
{
#ifdef HI_TEE_SUPPORT
    return _ks_read_reg(KC_REE_FLUSH_BUSY);
#else
    return _ks_read_reg(KC_TEE_FLUSH_BUSY);
#endif
}

static hi_bool _ks_is_busy(hi_void)
{
    /* bit 0 is 1 means there is a key slot flushed by the current cpu */
    return ((_ks_get_flush_status() & 0x1) != 0x0) ? HI_TRUE : HI_FALSE;
}

static hi_bool _ks_flush_failed(hi_void)
{
    /* bit 1 is 1 means current cpu flush the target key slot due to time out */
    return ((_ks_get_flush_status() & 0x2) != 0x0) ? HI_TRUE : HI_FALSE;
}

#define LOOP_MAX 1000
#define DELAY_US 1

static hi_s32 _ks_flush_wait(hi_void)
{
    hi_u32 time_out = LOOP_MAX;

    while (time_out--) {
        if (_ks_is_busy() == HI_FALSE) {
            break;
        }
        osal_udelay(DELAY_US);
    }
    if (time_out == 0) {
        return HI_ERR_KS_STAT_TIME_OUT;
    }
    if (_ks_flush_failed()) {
        return HI_ERR_KS_FLUSH_TIME_OUT;
    }
    return HI_SUCCESS;
}

static hi_bool _ks_is_tscipher(const hi_keyslot_type slot_ind)
{
    if (slot_ind == HI_KEYSLOT_TYPE_TSCIPHER) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

ks_slot_stat hal_ks_status(const hi_keyslot_type slot_ind, const hi_u32 slot_num)
{
    kc_rd_lock_status stat;
    kc_rd_slot_num slot;

    slot.u32 = _ks_read_reg(KC_RD_SLOT_NUM);
    slot.bits.tscipher_slot_ind = _ks_is_tscipher(slot_ind);
    slot.bits.slot_num_cfg = slot_num;
    _ks_write_reg(KC_RD_SLOT_NUM, slot.u32);

    stat.u32 = _ks_read_reg(KC_RD_LOCK_STATUS);
    return (ks_slot_stat)stat.bits.rd_lock_status;
}

hi_s32 hal_ks_lock(const hi_keyslot_type slot_ind, const hi_u32 slot_num)
{
    ks_slot_stat state;

#ifdef HI_TEE_SUPPORT
    kc_ree_lock_cmd ree_reg;
    if (_ks_is_busy()) {
        return HI_ERR_KS_BUSY;
    }
    ree_reg.u32 = _ks_read_reg(KC_REE_LOCK_CMD);
    ree_reg.bits.ree_lock_cmd = 1;
    ree_reg.bits.ree_tscipher_ind = _ks_is_tscipher(slot_ind);
    ree_reg.bits.ree_key_slot_num = slot_num;
    _ks_write_reg(KC_REE_LOCK_CMD, ree_reg.u32);

    state = hal_ks_status(slot_ind, slot_num);
    if (state != KS_STAT_REE_LOCK) {
        return HI_ERR_KS_LOCKED_CPUX + state;
    }
#else
    kc_tee_lock_cmd tee_reg;
    if (_ks_is_busy()) {
        return HI_ERR_KS_BUSY;
    }
    tee_reg.u32 = _ks_read_reg(KC_TEE_LOCK_CMD);
    tee_reg.bits.tee_lock_cmd = 1;
    tee_reg.bits.tee_tscipher_ind = _ks_is_tscipher(slot_ind);
    tee_reg.bits.tee_key_slot_num = slot_num;
    _ks_write_reg(KC_TEE_LOCK_CMD, tee_reg.u32);

    state = hal_ks_status(slot_ind, slot_num);
    if (state != KS_STAT_TEE_LOCK) {
        return HI_ERR_KS_LOCKED_CPUX + state;
    }
#endif
    return HI_SUCCESS;
}

hi_s32 hal_ks_unlock(const hi_keyslot_type slot_ind, const hi_u32 slot_num)
{
#ifdef HI_TEE_SUPPORT
    kc_ree_lock_cmd ree_reg;
    if (_ks_is_busy()) {
        return HI_ERR_KS_BUSY;
    }
    ree_reg.u32 = _ks_read_reg(KC_REE_LOCK_CMD);
    ree_reg.bits.ree_lock_cmd = 0;
    ree_reg.bits.ree_tscipher_ind = _ks_is_tscipher(slot_ind);
    ree_reg.bits.ree_key_slot_num = slot_num;
    _ks_write_reg(KC_REE_LOCK_CMD, ree_reg.u32);
#else
    kc_tee_lock_cmd tee_reg;
    if (_ks_is_busy()) {
        return HI_ERR_KS_BUSY;
    }
    tee_reg.u32 = _ks_read_reg(KC_TEE_LOCK_CMD);
    tee_reg.bits.tee_lock_cmd = 0;
    tee_reg.bits.tee_tscipher_ind = _ks_is_tscipher(slot_ind);
    tee_reg.bits.tee_key_slot_num = slot_num;
    _ks_write_reg(KC_TEE_LOCK_CMD, tee_reg.u32);
#endif
    return _ks_flush_wait();
}

