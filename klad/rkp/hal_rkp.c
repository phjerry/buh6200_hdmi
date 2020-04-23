/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
* Description :hpp rom keyladder.
* Author : Hisilicon security team
* Created : 2019-03-13
*/
#include "hal_rkp.h"

#include <linux/delay.h>

#include "drv_rkp.h"
#include "drv_rkp_reg.h"
#include "drv_rkp_define.h"

#ifdef HI_TEE_SUPPORT
#define  RKP_IRQ_NUMBER            (179 + 32)
#define  RKP_IRQ_NAME              "int_rkp_ree"
#else
#define  RKP_IRQ_NUMBER            (178 + 32)
#define  RKP_IRQ_NAME              "int_rkp_tee"
#endif

#define LOOP_MAX 1000 /* Waiting for logic completion count. */
#define DELEY_TIME 1 /* Delay time, xxx us. */
#define TIME_OUT 1000 /* Interrupt time out. */

hi_u32  rkp_get_offset(hi_u32 addr)
{
    if ((addr - RKP_REG_BASE < 0x200) || (addr - RKP_REG_BASE > 0x600)) {
        return 0;
    }
#ifndef HI_TEE_SUPPORT
    return RKP_TCPU_OFFSET;
#else
    return RKP_ACPU_OFFSET;
#endif
}

static void _rkp_write_reg(const hi_u32 addr, const hi_u32 val)
{
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();

    reg_write((mgmt->io_base + (addr + rkp_get_offset(addr) - RKP_REG_BASE)), val);
    HI_INFO_KLAD("klad w 0x%08x 0x%08x\n", addr + rkp_get_offset(addr), val);
    return;
}

static hi_u32 _rkp_read_reg(const hi_u32 addr)
{
    hi_u32 val = 0;
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();

    reg_read((mgmt->io_base + (addr + rkp_get_offset(addr) - RKP_REG_BASE)), val);
    HI_INFO_KLAD("klad r 0x%08x 0x%08x\n", addr + rkp_get_offset(addr), val);
    return val;
}

static hi_u32 rkp_read_calc_start(void)
{
    rkp_calc_start_tee reg_v;
    reg_v.u32 = _rkp_read_reg(RKP_CALC_START);
    return reg_v.bits.tee_calc_start;
}


void rkp_write_reg(const hi_u32 addr, const hi_u32 val)
{
    _rkp_write_reg(addr, val);
    return;
}

hi_u32 rkp_read_reg(const hi_u32 addr)
{
    return _rkp_read_reg(addr);
}

hi_u32 rkp_is_idle(void)
{
    if (rkp_read_calc_start() == HI_FALSE) {
        return HI_SUCCESS;
    }
    return HI_FAILURE;
}

#ifdef HI_INT_SUPPORT
static hi_s32 hal_rkp_wait_condition(const hi_void *param)
{
    return *(hi_u32 *)param == 0 ? HI_FALSE : HI_TRUE;
}
#endif

hi_s32 rkp_wait_idle(void)
{
#ifdef HI_INT_SUPPORT
    hi_s32 ret;
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();

    osal_mutex_lock(&mgmt->rkp_lock);

    ret = osal_wait_timeout_interruptible(&mgmt->rkp_wait_queue, hal_rkp_wait_condition,
                                          &mgmt->rkp_wait_cond, TIME_OUT);
    mgmt->rkp_wait_cond = 0;

    osal_mutex_unlock(&mgmt->rkp_lock);

    if (ret == 0) {
        HI_ERR_KLAD("rkp is busy, IRQ : %d, status : 0x%08x\n", RKP_IRQ_NUMBER, rkp_read_reg(RKP_ERROR_STATUS));
        return HI_ERR_KLAD_WAIT_TIMEOUT;
    } else {
        HI_INFO_KLAD("cost time %dms, jiffies=%d.\n", TIME_OUT - osal_get_tickcount(),
                     osal_msecs_to_jiffies(TIME_OUT) - ret);
    }
#else
    hi_u32 time_out = LOOP_MAX;

    while (time_out--) {
        if (rkp_read_calc_start() == 0x0) {
            break;
        }
        osal_udelay(DELEY_TIME);
    }
    if (time_out == 0) {
        HI_ERR_KLAD("rkp_wait_idle timeout!");
        return HI_FAILURE;
    }
#endif
    return HI_SUCCESS;
}

void rkp_slot_choose(hi_rkp_slot_choose slot)
{
    hi_u32 reg_slot_choose;

#ifndef HI_TEE_SUPPORT
    rkp_slot_choose_tee tee_reg;
    tee_reg.u32 = _rkp_read_reg(RKP_SLOT_CHOOSE);
    tee_reg.bits.tee_slot_choose = slot;
    reg_slot_choose = tee_reg.u32;
#else
    rkp_slot_choose_ree ree_reg;
    ree_reg.u32 = _rkp_read_reg(RKP_SLOT_CHOOSE);
    ree_reg.bits.ree_slot_choose = slot;
    reg_slot_choose = ree_reg.u32;
#endif
    _rkp_write_reg(RKP_SLOT_CHOOSE, reg_slot_choose);
}

void rkp_klad_sel(hi_rkp_klad_sel klad_sel, hi_rkp_klad_type_sel klad_type_sel)
{
    hi_u32 reg_klad_choose;

#ifndef HI_TEE_SUPPORT
    rkp_klad_choose_tee tee_reg;
    tee_reg.u32 = _rkp_read_reg(RKP_KLAD_CHOOSE);
    tee_reg.bits.tee_klad_sel = klad_sel;
    reg_klad_choose = tee_reg.u32;
#else
    rkp_klad_choose_ree ree_reg;
    ree_reg.u32 = _rkp_read_reg(RKP_KLAD_CHOOSE);
    ree_reg.bits.ree_klad_sel = klad_sel;
    reg_klad_choose = ree_reg.u32;
#endif
    unused(klad_type_sel);
    _rkp_write_reg(RKP_KLAD_CHOOSE, reg_klad_choose);
}


void rkp_cas_kdf_static_disable(hi_rkp_cas_kdf_static_sel cas_kdf_static)
{
#ifndef HI_TEE_SUPPORT
    hi_u32 reg_cas_kdf_disable;
    rkp_cas_kdf_static_disable_tee tee_reg;
    tee_reg.u32 = _rkp_read_reg(RKP_CAS_KDF_STATIC_DISABLE);
    tee_reg.bits.tee_cas_kdf_static_choose = cas_kdf_static;
    reg_cas_kdf_disable = tee_reg.u32;
    _rkp_write_reg(RKP_CAS_KDF_STATIC_DISABLE, reg_cas_kdf_disable);
#endif
}

void rkp_vendor_id(hi_u32 vendor_id)
{
    _rkp_write_reg(RKP_VENDOR_ID, vendor_id);
}

hi_s32 rkp_module_id(hi_u32 module_id3, hi_u32 module_id2, hi_u32 module_id1, hi_u32 module_id0)
{
    hi_u32 tmp;

    _rkp_write_reg(RKP_MODULE_ID_0, module_id0);
    _rkp_write_reg(RKP_MODULE_ID_1, module_id1);
    _rkp_write_reg(RKP_MODULE_ID_2, module_id2);
    _rkp_write_reg(RKP_MODULE_ID_3, module_id3);

    tmp = _rkp_read_reg(RKP_MODULE_ID_0);
    if (tmp != module_id0) {
        HI_ERR_KLAD("Invalid module id 0, w 0x%08x, r 0x%08x\n", module_id0, tmp);
        return HI_ERR_KLAD_RKP_INVALID_MODULE_ID;
    }
    tmp = _rkp_read_reg(RKP_MODULE_ID_1);
    if (tmp != module_id1) {
        HI_ERR_KLAD("Invalid module id 1, w 0x%08x, r 0x%08x\n", module_id1, tmp);
        return HI_ERR_KLAD_RKP_INVALID_MODULE_ID;
    }
    tmp = _rkp_read_reg(RKP_MODULE_ID_2);
    if (tmp != module_id2) {
        HI_ERR_KLAD("Invalid module id 2, w 0x%08x, r 0x%08x\n", module_id2, tmp);
        return HI_ERR_KLAD_RKP_INVALID_MODULE_ID;
    }
    tmp = _rkp_read_reg(RKP_MODULE_ID_3);
    if (tmp != module_id3) {
        HI_ERR_KLAD("Invalid module id 3, w 0x%08x, r 0x%08x\n", module_id3, tmp);
        return HI_ERR_KLAD_RKP_INVALID_MODULE_ID;
    }
    return HI_SUCCESS;
}

void rkp_unique_type(hi_u32 unique_type)
{
    hi_u32 reg_unique_type;

#ifndef HI_TEE_SUPPORT
    rkp_unique_type_tee tee_reg;
    tee_reg.u32 = _rkp_read_reg(RKP_UNIQUE_TYPE);
    tee_reg.bits.tee_unique_type = unique_type;
    reg_unique_type = tee_reg.u32;
#else
    rkp_unique_type_ree ree_reg;
    ree_reg.u32 = _rkp_read_reg(RKP_UNIQUE_TYPE);
    ree_reg.bits.ree_unique_type = unique_type;
    reg_unique_type = ree_reg.u32;
#endif
    _rkp_write_reg(RKP_UNIQUE_TYPE, reg_unique_type);
}

void rkp_fixed_rk_data(hi_u16 fixed_rk_data)
{
    hi_u32 reg_fixed_rk_data;

#ifndef HI_TEE_SUPPORT
    rkp_fixed_rk_data_tee tee_reg;
    tee_reg.u32 = _rkp_read_reg(RKP_FIXED_RK_DATA);
    tee_reg.bits.tee_fixed_rk_data = fixed_rk_data;
    reg_fixed_rk_data = tee_reg.u32;
#else
    rkp_fixed_rk_data_ree ree_reg;
    ree_reg.u32 = _rkp_read_reg(RKP_FIXED_RK_DATA);
    ree_reg.bits.ree_fixed_rk_data = fixed_rk_data;
    reg_fixed_rk_data = ree_reg.u32;
#endif
    _rkp_write_reg(RKP_FIXED_RK_DATA, reg_fixed_rk_data);
}

void rkp_fixed_rk_key(hi_u16 fixed_rk_key)
{
    hi_u32 reg_fixed_rk_key;

#ifndef HI_TEE_SUPPORT
    rkp_fixed_rk_key_tee tee_reg;
    tee_reg.u32 = _rkp_read_reg(RKP_FIXED_RK_KEY);
    tee_reg.bits.tee_fixed_rk_key = fixed_rk_key;
    reg_fixed_rk_key = tee_reg.u32;
#else
    rkp_fixed_rk_key_ree ree_reg;
    ree_reg.u32 = _rkp_read_reg(RKP_FIXED_RK_KEY);
    ree_reg.bits.ree_fixed_rk_key = fixed_rk_key;
    reg_fixed_rk_key = ree_reg.u32;
#endif
    _rkp_write_reg(RKP_FIXED_RK_KEY, reg_fixed_rk_key);
}

void rkp_fixed_rk_key_sel(hi_u16 fixed_rk_key_sel)
{
    hi_u32 reg_fixed_rk_key_sel;

#ifndef HI_TEE_SUPPORT
    rkp_fixed_rk_key_sel_tee tee_reg;
    tee_reg.u32 = _rkp_read_reg(RKP_FIXED_RK_KEY_SEL);
    tee_reg.bits.tee_fixed_rk_key_sel = fixed_rk_key_sel;
    reg_fixed_rk_key_sel = tee_reg.u32;
#else
    rkp_fixed_rk_key_sel_ree ree_reg;
    ree_reg.u32 = _rkp_read_reg(RKP_FIXED_RK_KEY_SEL);
    ree_reg.bits.ree_fixed_rk_key_sel = fixed_rk_key_sel;
    reg_fixed_rk_key_sel = ree_reg.u32;
#endif
    _rkp_write_reg(RKP_FIXED_RK_KEY_SEL, reg_fixed_rk_key_sel);
}

void rkp_calc_start(hi_rkp_command_type command_type)
{
    hi_u32 reg_calc_start;
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();

#ifndef HI_TEE_SUPPORT
    rkp_calc_start_tee tee_reg;
    tee_reg.u32 = _rkp_read_reg(RKP_CALC_START);
    tee_reg.bits.tee_command_type = command_type;
    tee_reg.bits.tee_calc_start = 1;
    reg_calc_start = tee_reg.u32;
#else
    rkp_calc_start_ree ree_reg;
    ree_reg.u32 = _rkp_read_reg(RKP_CALC_START);
    ree_reg.bits.ree_command_type = command_type;
    ree_reg.bits.ree_calc_start = 1;
    reg_calc_start = ree_reg.u32;
#endif
    timestamp(&mgmt->time_ctl_b);
    _rkp_write_reg(RKP_CALC_START, reg_calc_start);
    timestamp(&mgmt->time_ctl_e);
}

hi_u32 rkp_read_int_enable(void)
{
    hi_u32 reg_int_enable;

#ifndef HI_TEE_SUPPORT
    rkp_int_enable_tee tee_reg;
    tee_reg.u32 = _rkp_read_reg(RKP_INT_ENABLE);
    reg_int_enable = tee_reg.bits.tee_int_enable;
#else
    rkp_int_enable_ree ree_reg;
    ree_reg.u32 = _rkp_read_reg(RKP_INT_ENABLE);
    reg_int_enable = ree_reg.bits.ree_int_enable;
#endif
    return reg_int_enable;
}

hi_s32 rkp_check_error(void)
{
    hi_u32 reg;

    reg = _rkp_read_reg(RKP_DEBUG_INFO_3);
    if (reg != 0x01) {
        HI_ERR_KLAD("rkp debug info 3 = 0x%08x\n", reg);
        return HI_FAILURE;
    }
    reg = _rkp_read_reg(RKP_ROBUST_ALARM);
    if (reg != 0) {
        HI_ERR_KLAD("rkp robust alarm = 0x%08x\n", reg);
        return HI_FAILURE;
    }
    reg = _rkp_read_reg(RKP_ERROR_STATUS);
    if ((reg & 0xfffffffe) != 0) {
        HI_ERR_KLAD("rkp error status = 0x%08x\n", reg);
        return err_code_gen(RKP_ERR, reg);
    }
    return HI_SUCCESS;
}

hi_s32 rkp_get_klad_level(hi_u32 module_id_0)
{
    rkp_module_id_0 reg;

    reg.u32 = module_id_0;
    /* any cpu use the same bits. */
#ifndef HI_TEE_SUPPORT
    return reg.tee.bits.tee_stage;
#else
    return reg.ree.bits.ree_stage;
#endif
}

#ifdef HI_INT_SUPPORT

static void _rkp_int_update(hi_u32 int_enable)
{
    hi_u32 reg_int_enable;

#ifndef HI_TEE_SUPPORT
    rkp_int_enable_tee tee_reg;
    tee_reg.bits.tee_int_enable = int_enable;
    reg_int_enable = tee_reg.u32;
#else
    rkp_int_enable_ree ree_reg;
    ree_reg.bits.ree_int_enable = int_enable;
    reg_int_enable = ree_reg.u32;
#endif
    _rkp_write_reg(RKP_INT_ENABLE, reg_int_enable);
    return;
}

hi_void rkp_int_en(hi_void)
{
    return _rkp_int_update(HI_TRUE);
}

hi_void rkp_int_dis(hi_void)
{
    return _rkp_int_update(HI_FALSE);
}

hi_void rkp_int_clr(hi_void)
{
    hi_u32 reg_raw_enable;

#ifndef HI_TEE_SUPPORT
    rkp_raw_int_tee tee_reg;
    tee_reg.bits.tee_rkp_raw_int = HI_TRUE;
    reg_raw_enable = tee_reg.u32;
#else
    rkp_raw_int_ree ree_reg;
    ree_reg.bits.ree_rkp_raw_int = HI_TRUE;
    reg_raw_enable = ree_reg.u32;
#endif
    _rkp_write_reg(RKP_RAW_INT, reg_raw_enable);
    return;
}

hi_u32 rkp_get_int_stat(hi_void)
{
    return _rkp_read_reg(RKP_INT_STATUS);
}

#endif

