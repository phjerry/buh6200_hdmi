/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
* Description :hisilicon keyladder driver.
* Author : Hisilicon security team
* Created : 2019-02-20
*/
#include "hal_rkp.h"
#include "drv_rkp.h"
#include "drv_rkp_define.h"
#include "drv_rkp_dbg.h"
#include "drv_klad_timestamp_logic.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static hi_s32 __rkp_check_rst(hi_void)
{
    hi_s32 ret;
#ifdef HI_KLAD_PERF_SUPPORT
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();
    struct klad_timestamp_logic t;

    timestamp(&t.time_wait_b);
#endif

    ret = rkp_wait_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_wait_idle, ret);
        goto out;
    }
#ifdef HI_KLAD_PERF_SUPPORT
    timestamp(&t.time_wait_e);

    t.tag = RKP;

    t.time_ctl_b.tv_sec = mgmt->time_ctl_b.tv_sec;
    t.time_ctl_b.tv_nsec = mgmt->time_ctl_b.tv_nsec;

    t.time_ctl_e.tv_sec = mgmt->time_ctl_e.tv_sec;
    t.time_ctl_e.tv_nsec = mgmt->time_ctl_e.tv_nsec;

    t.time_int_b.tv_sec = mgmt->time_int_b.tv_sec;
    t.time_int_b.tv_nsec = mgmt->time_int_b.tv_nsec;

    t.time_int_e.tv_sec = mgmt->time_int_e.tv_sec;
    t.time_int_e.tv_nsec = mgmt->time_int_e.tv_nsec;

    klad_timestamp_logic_queue(&t);
#endif

    ret = rkp_check_error();
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_check_error, ret);
        goto out;
    }
out:
    return ret;
}

/*
 * RKP deob definition
 */
hi_s32 rkp_deob_start(rkp_deob *deob)
{
    hi_s32 ret;
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();

    if (deob == NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }
    osal_mutex_lock(&mgmt->lock);

    ret = rkp_is_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_is_idle, ret);
        goto out;
    }
    rkp_slot_choose(deob->rootkey_slot);
    rkp_calc_start(HI_RKP_COMMAND_TYPE_DEOB);

    ret = __rkp_check_rst();
    if (ret != HI_SUCCESS) {
        print_err_func(__rkp_check_rst, ret);
        goto out;
    }
    ret = HI_SUCCESS;
out:
    osal_mutex_unlock(&mgmt->lock);
    return ret;
}

hi_s32 rkp_deob_process(rkp_deob *deob)
{
    hi_s32 ret;

    if (deob == NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }
    ret = rkp_deob_start(deob);
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_deob_start, ret);
    }

    return ret;
}

/*
 * RKP inte definition
 */
hi_s32 rkp_inte_start(rkp_inte *inte, rkp_inte_rst *inte_rst)
{
    hi_s32 ret;
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();

    if (inte == NULL || inte_rst == NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }

    osal_mutex_lock(&mgmt->lock);

    ret = rkp_is_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_is_idle, ret);
        goto out;
    }

    rkp_slot_choose(inte->rootkey_slot);
    rkp_calc_start(HI_RKP_COMMAND_TYPE_INTE);

    ret = __rkp_check_rst();
    if (ret != HI_SUCCESS) {
        print_err_func(__rkp_check_rst, ret);
        goto out;
    }

    inte_rst->check_sum = rkp_get_checksum(inte->rootkey_slot);
    ret = HI_SUCCESS;
out:
    osal_mutex_unlock(&mgmt->lock);
    return ret;
}

/*
 * RKP deob+inte definition
 */
hi_s32 rkp_deob_inte_start(rkp_deob_inte *deob_inte, rkp_deob_inte_rst *deob_inte_rst)
{
    hi_s32 ret;
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();
    rkp_calc_crc_0 crc_0;
    rkp_calc_crc_1 crc_1;

    crc_0.u32 = 0;
    crc_1.u32 = 0;
    if (deob_inte == NULL || deob_inte_rst == NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }
    osal_mutex_lock(&mgmt->lock);

    ret = rkp_is_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_is_idle, ret);
        goto out;
    }
    rkp_slot_choose(deob_inte->deob.rootkey_slot);
    rkp_calc_start(HI_RKP_COMMAND_TYPE_DEOB_INTE);

    ret = __rkp_check_rst();
    if (ret != HI_SUCCESS) {
        print_err_func(__rkp_check_rst, ret);
        goto out;
    }
    rkp_get_crc(&crc_0, &crc_1);
    deob_inte_rst->deob_rst.vmask_crc = crc_0.bits.calc_vmask_crc;
    deob_inte_rst->deob_rst.clear_rk_crc = crc_0.bits.calc_clear_rk_crc;
    deob_inte_rst->inte_rst.check_sum = rkp_get_checksum(deob_inte->inte.rootkey_slot);

    ret = HI_SUCCESS;
out:
    osal_mutex_unlock(&mgmt->lock);
    return ret;
}

/*
 * Check module id.
 */
hi_s32 rkp_kdf_verify_module_id(rkp_kdf *kdf)
{
#ifdef HI_TEE_SUPPORT
    /* REE register. */
    if ((kdf->module_id3.u32 != 0) ||
        (kdf->module_id2.ree.bits.reserved_0 != 0) || (kdf->module_id2.ree.bits.reserved_1 != 0) ||
        (kdf->module_id1.ree.bits.reserved_0 != 0) || (kdf->module_id1.ree.bits.reserved_1 != 0) ||
        (kdf->module_id1.ree.bits.reserved_2 != 0) || (kdf->module_id0.ree.bits.reserved_0 != 0) ||
        (kdf->module_id0.ree.bits.reserved_1 != 0) || (kdf->module_id0.ree.bits.reserved_2 != 0) ||
        (kdf->module_id0.ree.bits.reserved_3 != 0) || (kdf->module_id0.ree.bits.reserved_4 != 0)) {
        return HI_ERR_KLAD_RKP_INVALID_MODULE_ID;
    }
#else
    /* TEE register. */
    if ((kdf->module_id3.u32 != 0) ||
        (kdf->module_id2.tee.bits.reserved_0 != 0) || (kdf->module_id2.tee.bits.reserved_1 != 0) ||
        (kdf->module_id1.tee.bits.reserved_0 != 0) || (kdf->module_id1.tee.bits.reserved_1 != 0) ||
        (kdf->module_id1.tee.bits.reserved_2 != 0) || (kdf->module_id0.tee.bits.reserved_0 != 0) ||
        (kdf->module_id0.tee.bits.reserved_1 != 0) || (kdf->module_id0.tee.bits.reserved_2 != 0) ||
        (kdf->module_id0.tee.bits.reserved_3 != 0) || (kdf->module_id0.tee.bits.reserved_4 != 0) ||
        (kdf->module_id0.tee.bits.reserved_5 != 0)) {
        return HI_ERR_KLAD_RKP_INVALID_MODULE_ID;
    }
#endif
    return HI_SUCCESS;
}

/*
 * RKP fixed rk + kdf definition
 */
hi_s32 rkp_fix_eff_rk_start(rkp_fix_rk_kdf *fix_rk_kdf)
{
    hi_s32 ret;
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();

    if (fix_rk_kdf == NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }
    osal_mutex_lock(&mgmt->lock);

    ret = rkp_is_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_is_idle, ret);
        goto out;
    }
    rkp_slot_choose(fix_rk_kdf->fix_rk.rootkey_slot);
    rkp_fixed_rk_data(fix_rk_kdf->fix_rk.fix_rk_data);
    rkp_fixed_rk_key(fix_rk_kdf->fix_rk.fix_rk_key);
    rkp_fixed_rk_key_sel(fix_rk_kdf->fix_rk.fix_rk_key_sel);

    /* klad_type_sel: only tpp takes effect. */
    rkp_klad_sel(fix_rk_kdf->klad.klad_sel, fix_rk_kdf->klad.klad_type_sel);
    rkp_cas_kdf_static_disable((hi_rkp_cas_kdf_static_sel)fix_rk_kdf->kdf.cas_kdf_static);
    rkp_vendor_id(fix_rk_kdf->kdf.vendor_id);
    ret = rkp_module_id(fix_rk_kdf->kdf.module_id3.u32, fix_rk_kdf->kdf.module_id2.u32,
                        fix_rk_kdf->kdf.module_id1.u32, fix_rk_kdf->kdf.module_id0.u32);
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_module_id, ret);
        goto out;
    }
    rkp_unique_type(fix_rk_kdf->kdf.unique_type);
    rkp_calc_start(HI_RKP_COMMAND_TYPE_FIXED_RK_KDF);

    ret = __rkp_check_rst();
    if (ret != HI_SUCCESS) {
        print_err_func(__rkp_check_rst, ret);
        goto out;
    }
    ret = HI_SUCCESS;
out:
    osal_mutex_unlock(&mgmt->lock);
    return ret;
}

hi_s32 rkp_fix_eff_rk_process(rkp_fix_rk_kdf *fix_rk_kdf, hi_u32 com_kl_num)
{
    hi_s32 ret;

    if (fix_rk_kdf == NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }

    fix_rk_kdf->klad.klad_sel = (hi_rkp_klad_sel)com_kl_num;
    ret = rkp_fix_eff_rk_start(fix_rk_kdf);
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_fix_eff_rk_start, ret);
    }

    return ret;
}

/*
 * RKP deob + kdf definition
 */
hi_s32 rkp_eff_rk_start(rkp_deob_kdf *deob_kdf)
{
    hi_s32 ret;
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();

    if (deob_kdf == NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }
    osal_mutex_lock(&mgmt->lock);

    ret = rkp_is_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_is_idle, ret);
        goto out;
    }
    ret = rkp_kdf_verify_module_id(&deob_kdf->kdf);
    if (ret != HI_SUCCESS) {
        print_err_hex4(deob_kdf->kdf.module_id0.u32, deob_kdf->kdf.module_id1.u32, deob_kdf->kdf.module_id2.u32, ret);
        return ret;
    }
    rkp_slot_choose(deob_kdf->deob.rootkey_slot);
    rkp_klad_sel(deob_kdf->klad.klad_sel, deob_kdf->klad.klad_type_sel);
    rkp_cas_kdf_static_disable((hi_rkp_cas_kdf_static_sel)deob_kdf->kdf.cas_kdf_static);
    rkp_vendor_id(deob_kdf->kdf.vendor_id);
    ret = rkp_module_id(deob_kdf->kdf.module_id3.u32, deob_kdf->kdf.module_id2.u32,
                        deob_kdf->kdf.module_id1.u32, deob_kdf->kdf.module_id0.u32);
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_module_id, ret);
        goto out;
    }
    rkp_unique_type(deob_kdf->kdf.unique_type);
    rkp_calc_start(HI_RKP_COMMAND_TYPE_EFF_RK_GEN);

    ret = __rkp_check_rst();
    if (ret != HI_SUCCESS) {
        print_err_func(__rkp_check_rst, ret);
        goto out;
    }
    ret = HI_SUCCESS;
out:
    osal_mutex_unlock(&mgmt->lock);
    return ret;
}

hi_s32 rkp_eff_rk_process(rkp_deob_kdf *deob_kdf, hi_rkp_klad_sel klad_sel)
{
    hi_s32 ret;

    if (deob_kdf == NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }

    deob_kdf->klad.klad_sel = klad_sel;
    ret = rkp_eff_rk_start(deob_kdf);
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_fix_eff_rk_start, ret);
    }
    return HI_SUCCESS;
}

hi_s32 rkp_eff_rk_jtag_start(rkp_deob_kdf *deob_kdf)
{
    hi_s32 ret;
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();

    if (deob_kdf == NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }
    osal_mutex_lock(&mgmt->lock);

    ret = rkp_is_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_is_idle, ret);
        goto out;
    }

    ret = rkp_module_id(deob_kdf->kdf.module_id3.u32, deob_kdf->kdf.module_id2.u32,
                        deob_kdf->kdf.module_id1.u32, deob_kdf->kdf.module_id0.u32);
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_module_id, ret);
        goto out;
    }
    rkp_calc_start(HI_RKP_COMMAND_TYPE_TEE_USR_JTAG);

    ret = __rkp_check_rst();
    if (ret != HI_SUCCESS) {
        print_err_func(__rkp_check_rst, ret);
        goto out;
    }
    ret = HI_SUCCESS;
out:
    osal_mutex_unlock(&mgmt->lock);
    return ret;
}

hi_s32 rkp_eff_rk_jtag_process(rkp_deob_kdf *deob_kdf)
{
    hi_s32 ret;

    if (deob_kdf == NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }
    ret = rkp_eff_rk_jtag_start(deob_kdf);
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_fix_eff_rk_start, ret);
    }
    return HI_SUCCESS;
}

static struct rkp_mgmt g_rkp_mgmt = {
    .lock      = {0},
    .state     = RKP_MGMT_CLOSED,
};

struct rkp_mgmt *__get_rkp_mgmt(hi_void)
{
    return &g_rkp_mgmt;
}

#ifdef HI_INT_SUPPORT
static irqreturn_t _rkp_isr(hi_s32 irq, hi_void *id)
{
    hi_s32 int_stat;
    struct rkp_mgmt *mgmt = (struct rkp_mgmt *)id;

    if (id == HI_NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }
    timestamp(&mgmt->time_int_b);

    int_stat = rkp_get_int_stat();

    rkp_int_dis();
    rkp_int_clr();

    if (int_stat & 0x1) {
        mgmt->rkp_wait_cond = 0x1;
        osal_wait_wakeup(&mgmt->rkp_wait_queue);
    }
    rkp_int_en();
    timestamp(&mgmt->time_int_e);
    return IRQ_HANDLED;
}
#endif

static hi_u32 __hkl_otp_shadow_read(hi_u32 addr)
{
    hi_u32 value;
    hi_u8 *vir_addr = HI_NULL;

    if (addr < OTP_SHADOW_REG_BASE || addr > OTP_SHADOW_REG_BASE + OTP_SHADOW_REG_RANGE) {
        print_err_hex(addr);
        return 0;
    }

    vir_addr = (hi_u8 *)osal_ioremap_nocache(addr, 0x10);
    if (vir_addr == HI_NULL) {
        print_err_hex(addr);
        return 0;
    }

    reg_read(vir_addr, value);
    HI_INFO_KLAD("klad r 0x%08x 0x%08x\n", addr, value);
    osal_iounmap(vir_addr);

    return value;
}

hi_u32 otp_shadow_read(hi_u32 addr)
{
    return __hkl_otp_shadow_read(addr);
}

/*
* For common keyladder, rootkey slot is define by customer.
* We find the first cas_slot satisfies the conditions:
* 1: rootkey & crc burn correct
* 2: ca_owner_id is zero(NON CAS SLOT)
* 3: rd_disable is zero(ROOTKEY is can be used)
* 4: secret_route_enable is zero(send key to KDF, to HKL but SKL)
* If customer want to use other cas slot, only can be achieved by DYM keyladder.
*/
hi_s32 rkp_cas_slot_find(hi_rootkey_select *root_slot)
{
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();
    hi_u32 slot;

    for (slot = 0; slot <= HI_RKP_SLOT_CHOOSE_SLOT7; slot++) {
        if ((mgmt->rk_flag.cas_slot[slot].rk_ready == HI_TRUE) &&
            (mgmt->rk_flag.cas_slot[slot].flag.bits.ca_owner_id == 0) &&
            (mgmt->rk_flag.cas_slot[slot].flag.bits.rk_disable == 0) &&
            (mgmt->rk_flag.cas_slot[slot].flag.bits.secret_route_enable == 0)) {
            *root_slot = HI_CAS_ROOTKEY_SLOT0 + slot;
            return HI_SUCCESS;
        }
    }
    print_err_code(HI_ERR_KLAD_NOT_FIND_RK_SLOT);
    return HI_ERR_KLAD_NOT_FIND_RK_SLOT;
}

hi_void __rkp_mgmt_rk_init(hi_void)
{
    hi_u32 slot;
    rkp_slotx_crc_rst reg;
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();

    mgmt->rk_flag.cas_slot[0x0].flag.u32 = __hkl_otp_shadow_read(OTP_SHW_CAS_SLOT0_FLAG);
    mgmt->rk_flag.cas_slot[0x1].flag.u32 = __hkl_otp_shadow_read(OTP_SHW_CAS_SLOT0_FLAG);
    mgmt->rk_flag.cas_slot[0x2].flag.u32 = __hkl_otp_shadow_read(OTP_SHW_CAS_SLOT0_FLAG);
    mgmt->rk_flag.cas_slot[0x3].flag.u32 = __hkl_otp_shadow_read(OTP_SHW_CAS_SLOT0_FLAG);
    mgmt->rk_flag.cas_slot[0x4].flag.u32 = __hkl_otp_shadow_read(OTP_SHW_CAS_SLOT0_FLAG);
    mgmt->rk_flag.cas_slot[0x5].flag.u32 = __hkl_otp_shadow_read(OTP_SHW_CAS_SLOT0_FLAG);
    mgmt->rk_flag.cas_slot[0x6].flag.u32 = __hkl_otp_shadow_read(OTP_SHW_CAS_SLOT0_FLAG);
    mgmt->rk_flag.cas_slot[0x7].flag.u32 = __hkl_otp_shadow_read(OTP_SHW_CAS_SLOT0_FLAG);
    mgmt->rk_flag.stbm_slot.flag.u32 = __hkl_otp_shadow_read(OTP_SHW_STBM_SLOT_FLAG);
    mgmt->rk_flag.boot_slot.flag.u32 = __hkl_otp_shadow_read(OTP_SHW_BOOT_SLOT_FLAG);

    for (slot = 0; slot <= HI_RKP_SLOT_CHOOSE_SLOT7; slot++) {
        reg.u32 = rkp_read_reg(RKP_SLOT0_CRC_RST + slot * 0x4);
        if ((reg.bits.slotx_crc_ok == 1) && (reg.bits.slotx_owner_id_correct == 1)) {
            mgmt->rk_flag.cas_slot[slot].rk_ready = HI_TRUE;
        } else {
            mgmt->rk_flag.cas_slot[slot].rk_ready = HI_FALSE;
        }
    }
    reg.u32 = rkp_read_reg(RKP_BOOT_CRC_RST);
    if ((reg.bits.slotx_crc_ok == 1) && (reg.bits.slotx_owner_id_correct == 1)) {
        mgmt->rk_flag.boot_slot.rk_ready = HI_TRUE;
    } else {
        mgmt->rk_flag.boot_slot.rk_ready = HI_FALSE;
    }

    reg.u32 = rkp_read_reg(RKP_STBM_CRC_RST);
    if (reg.bits.slotx_crc_ok == 1) {
        mgmt->rk_flag.stbm_slot.rk_ready = HI_TRUE;
    } else {
        mgmt->rk_flag.stbm_slot.rk_ready = HI_FALSE;
    }
}

#if HI_INT_SUPPORT
hi_void rkp_int_deinit(hi_void)
{
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();

    __mutex_lock(&mgmt->lock);

    rkp_int_clr();
    rkp_int_dis();

    __mutex_unlock(&mgmt->lock);
}

hi_void rkp_int_init(hi_void)
{
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();

    __mutex_lock(&mgmt->lock);

    rkp_int_en();
    rkp_int_clr();

    __mutex_unlock(&mgmt->lock);
}
#endif

hi_s32 rkp_mgmt_init(hi_void)
{
    hi_s32 ret;
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();

    mgmt->io_base = osal_ioremap_nocache(RKP_REG_BASE, RKP_REG_RANGE);
    if (mgmt->io_base == HI_NULL) {
        ret = HI_ERR_KLAD_MEM_MAP;
        goto out;
    }

    ret = osal_mutex_init(&mgmt->lock);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    __rkp_mgmt_rk_init();

#ifdef HI_INT_SUPPORT
    osal_mutex_init(&mgmt->rkp_lock);
    osal_wait_init(&mgmt->rkp_wait_queue);
    mgmt->rkp_wait_cond = 0;

    if (hi_drv_osal_request_irq(RKP_IRQ_NUMBER, (irq_handler_t)_rkp_isr, IRQF_SHARED,
                                RKP_IRQ_NAME, (hi_void *)mgmt) != 0) {
        HI_ERR_KLAD("osal_request irq(%d) failed.\n", RKP_IRQ_NUMBER);
        ret = HI_FAILURE;
        goto out;
    }

    if (hi_drv_sys_set_irq_affinity(HI_ID_KLAD, RKP_IRQ_NUMBER, RKP_IRQ_NAME) != HI_SUCCESS) {
        HI_WARN_KLAD("set klad irq affinity failed.\n");
    }

    rkp_int_init();
#endif

    mgmt->state = RKP_MGMT_OPENED;
    return  HI_SUCCESS;
out:
    mgmt->state = RKP_MGMT_CLOSED;
    return ret;
}

hi_void rkp_mgmt_exit(hi_void)
{
    struct rkp_mgmt *mgmt = __get_rkp_mgmt();

#ifdef HI_INT_SUPPORT
    rkp_int_deinit();
    hi_drv_osal_free_irq(RKP_IRQ_NUMBER, RKP_IRQ_NAME, (hi_void *)mgmt);
    mgmt->rkp_wait_cond = 0;
    osal_wait_destroy(&mgmt->rkp_wait_queue);
    osal_mutex_destory(&mgmt->rkp_lock);
#endif
    mgmt->state = RKP_MGMT_CLOSED;
    osal_mutex_destory(&mgmt->lock);
    osal_iounmap(mgmt->io_base);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

