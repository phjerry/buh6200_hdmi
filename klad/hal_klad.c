/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:KLAD driver in register level.
 * Author: Linux SDK team
 * Create: 2019/06/22
 */
#include "hal_klad.h"

#include "drv_hkl.h"

#include <linux/delay.h>

#include "drv_klad_hw_define.h"
#ifdef HI_PROC_SUPPORT
#include "drv_klad_proc.h"
#endif
#include "drv_klad_reg.h"
#include "drv_klad_timestamp_logic.h"

typedef enum {
    HKL_LOCK_STATE_TEE           = 0xa5,
    HKL_LOCK_STATE_REE           = 0xaa,
    HKL_LOCK_STATE_MAX
} hkl_lock_state;

#define LOOP_MAX 1000 /* Waiting for logic completion count. */
#define DELEY_TIME 1 /* Delay time, xxx us. */
#define TIME_OUT 1000 /* Interrupt time out. */
#define U8TOU32(u8_din0, u8_din1, u8_din2, u8_din3, u32_dout) \
    do { \
        (u32_dout) = ((hi_u32)(u8_din3) << 24) | ((hi_u32)(u8_din2) << 16) | ((hi_u32)(u8_din1) << 8) | (u8_din0); \
    } while (0)

hi_void hkl_write_reg(const hi_u32 addr, const hi_u32 val)
{
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    reg_write((mgmt->io_base + (addr - KLAD_REG_BASE)), val);
    HI_INFO_KLAD("klad w 0x%08x 0x%08x\n", addr, val);
    return;
}

hi_u32 hkl_read_reg(const hi_u32 addr)
{
    hi_u32 val = 0;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    reg_read((mgmt->io_base + (addr - KLAD_REG_BASE)), val);
    HI_INFO_KLAD("klad r 0x%08x 0x%08x\n", addr, val);

    return val;
}


static hi_u8 __hkl_get_lock_num(hkl_lock_klad type)
{
    switch (type) {
        case HKL_LOCK_KLAD_COM:
            return 0x0;
        case HKL_LOCK_KLAD_NONCE:
            return 0x1;
        case HKL_LOCK_KLAD_CLR:
            return 0x2;
        case HKL_LOCK_KLAD_FP:
            return 0x3;
        case HKL_LOCK_KLAD_TA:
            return 0x4;
        case HKL_LOCK_KLAD_CSGK2:
            return 0x5;
        default:
            return 0xf;
    }
}

static void _hkl_lock_ctrl_proces(hkl_lock_klad klad_type)
{
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    hkl_lock_ctrl lock_ctl;
    lock_ctl.u32 = hkl_read_reg(HKL_LOCK_CTRL);
    lock_ctl.bits.kl_lock = 1;
    lock_ctl.bits.kl_lock_num = __hkl_get_lock_num(klad_type);
    timestamp(&mgmt->time_ctl_b);
    hkl_write_reg(HKL_LOCK_CTRL, lock_ctl.u32);

    timestamp(&mgmt->time_ctl_e);
    return;
}

static void _hkl_unlock_ctrl_proces(hkl_lock_klad klad_type, unsigned int com_unlock_num)
{
    hkl_unlock_ctrl unlock_ctl;
    unlock_ctl.u32 = hkl_read_reg(HKL_UNLOCK_CTRL);
    unlock_ctl.bits.kl_unlock = 1;
    unlock_ctl.bits.kl_unlock_num = __hkl_get_lock_num(klad_type);
    if (klad_type == HKL_LOCK_KLAD_COM) {
        unlock_ctl.bits.kl_com_unlock_num = com_unlock_num;
    }
    hkl_write_reg(HKL_UNLOCK_CTRL, unlock_ctl.u32);
    return ;
}

static hi_u32 _hkl_get_busy(void)
{
    hkl_state reg_v;
    reg_v.u32 = hkl_read_reg(HKL_STATE);
    return reg_v.bits.kl_busy;
}

hi_u32 hkl_is_idle(void)
{
    if (_hkl_get_busy() == 0x02) { /* 0x01:busy, 0x02:idle, others:invalid */
        return HI_SUCCESS;
    }
    return HI_FAILURE;
}

static hi_s32 drv_klad_wait_condition(const hi_void *param)
{
    return *(hi_u32 *)param == 0 ? HI_FALSE : HI_TRUE;
}

hi_s32 hkl_wait_idle(void)
{
#ifdef HI_INT_SUPPORT
    hi_s32 ret;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    ret = osal_wait_timeout_interruptible(&mgmt->kl_alg_ip.wait_queue, drv_klad_wait_condition,
                                          &mgmt->kl_alg_ip.wait_cond, TIME_OUT);
    mgmt->kl_alg_ip.wait_cond = 0;
    if (ret == 0) {
        HI_ERR_KLAD("core is busy, IRQ : %d, status : 0x%08x\n", HKL_IRQ_NUMBER,
            hkl_read_reg(HKL_ERROR), hkl_read_reg(HKL_INT_RAW), hkl_read_reg(HKL_INT_RAW));
        return HI_ERR_KLAD_WAIT_TIMEOUT;
    } else {
        HI_INFO_KLAD("cost time %dms, jiffies=%d.\n", TIME_OUT - osal_get_tickcount(),
                     osal_msecs_to_jiffies(TIME_OUT) - ret);
    }

#else
    hi_u32 time_out = LOOP_MAX;

    while (time_out--) {
        if (_hkl_get_busy() == 0b10) {
            break;
        }
        osal_udelay(DELEY_TIME);
    }
    if (time_out == 0) {
        HI_ERR_KLAD("hkl_wait_idle timeout!");
        return HI_FAILURE;
    }
#endif
    return HI_SUCCESS;
}

void hkl_int_enable(hi_u32 int_enable)
{
    hkl_int_en reg_int_enable;

    reg_int_enable.u32 = 0;
    reg_int_enable.bits.kl_int_en = int_enable;
    hkl_write_reg(HKL_INT_EN, reg_int_enable.u32);
    return;
}

void hkl_set_addr(hi_u16 addr, hi_u32 is_odd)
{
    hkl_key_addr key_addr;

    key_addr.u32 = hkl_read_reg(HKL_KEY_ADDR);
    key_addr.bits.key_addr = (addr << 1) | is_odd;
    hkl_write_reg(HKL_KEY_ADDR, key_addr.u32);
    return;
}

void hkl_set_cfg(hi_u32 dec_support, hi_u32 enc_support, hi_u8 port, hi_u8 dsc_code)
{
    hkl_key_cfg reg;

    reg.u32 = hkl_read_reg(HKL_KEY_CFG);
    reg.bits.key_dec = dec_support;
    reg.bits.key_enc = enc_support;
    reg.bits.dsc_code = dsc_code;
    reg.bits.port_sel = port;
    hkl_write_reg(HKL_KEY_CFG, reg.u32);
    return;
}

void hkl_set_sec_cfg(hi_u32 dest_sec, hi_u32 dest_nsec,
                     hi_u32 src_sec, hi_u32 src_nsec, hi_u32 key_sec)
{
    hkl_key_sec_cfg reg;

    reg.u32 = hkl_read_reg(HKL_KEY_SEC_CFG);
    reg.bits.hpp_only = 0;
    reg.bits.dest_sec = dest_sec;
    reg.bits.dest_nsec = dest_nsec;
    reg.bits.src_sec = src_sec;
    reg.bits.src_nsec = src_nsec;
    reg.bits.key_sec = key_sec;
    hkl_write_reg(HKL_KEY_SEC_CFG, reg.u32);
    return;
}

hi_s32 hkl_set_data_in(hi_u8 *data, hi_u32 len)
{
    hi_u32 i = 0;
    hi_u32 din = 0;

    if (len != HKL_KEY_LEN) {
        return HI_FAILURE;
    }
    for (i = 0; i < HKL_KEY_LEN; i += 0x4) {
        U8TOU32(data[i], data[i + 0x1], data[i + 0x2], data[i + 0x3], din);
        hkl_write_reg(HKL_DATA_IN_0 + i, din);
    }
    return HI_SUCCESS;
}

void hkl_set_com_ctrl(hi_u8 com_kl_num, hi_u8 level, hi_u8 alg, hi_u32 to_ta_kl, hi_u32 key_abort)
{
    hkl_com_ctrl reg;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    reg.u32 = hkl_read_reg(HKL_COM_CTRL(com_kl_num));
    reg.bits.kl_com_start = 1;
    reg.bits.kl_com_level_sel = level;
    reg.bits.kl_com_alg_sel = alg;
    reg.bits.kl_com_ta_sel = to_ta_kl;
    reg.bits.kl_com_key_abort = key_abort;
    timestamp(&mgmt->time_ctl_b);
    hkl_write_reg(HKL_COM_CTRL(com_kl_num), reg.u32);
    timestamp(&mgmt->time_ctl_e);
    return ;
}

#define print_com_klad_debug(com_kl_num, reg, value) \
    do { \
        HI_ERR_KLAD("[com klad %d]%s:%-16s=0x%x\n", (com_kl_num), #reg, #value, reg.bits.value);\
    } while (0)

#define print_reg_err(reg, value) \
    do { \
        if(reg.bits.value != 0) { \
            HI_ERR_KLAD("%s:%-16s=0x%x\n", #reg, #value, reg.bits.value);\
        }\
    } while (0)

hi_u8 hkl_get_crc(void)
{
    hkl_crc crc;
    crc.u32 = hkl_read_reg(HKL_CRC);
    return crc.bits.kl_crc;
}

void hkl_dump_crc(void)
{
    hkl_crc crc;
    crc.u32 = hkl_read_reg(HKL_CRC);
    HI_INFO_KLAD("crc:\t 0x%x\n", crc.bits.kl_crc);
    return;
}

void hkl_get_klad_lock_info(struct hkl_lock_stat *stat)
{
    hi_u32 kl_index;
    for (kl_index = 0; kl_index < KLAD_COM_CNT; kl_index++) {
        stat->com_lock_stat[kl_index] = (hi_u8)(hkl_read_reg(HKL_COM_LOCK_STAT(kl_index)) & 0xff);
    }
    stat->ta_lock_stat = (hi_u8)(hkl_read_reg(HKL_TA_LOCK_STAT) & 0xff);
    stat->fp_lock_stat = (hi_u8)(hkl_read_reg(HKL_FP_LOCK_STAT) & 0xff);
    stat->nonce_lock_stat = (hi_u8)(hkl_read_reg(HKL_NONCE_LOCK_STAT) & 0xff);
    stat->clr_lock_stat = (hi_u8)(hkl_read_reg(HKL_CLR_LOCK_STAT) & 0xff);
    return;
}

static void _hkl_dump_hkl_com_status(hi_u8 com_kl_num, hkl_com_status com_status)
{
    print_com_klad_debug(com_kl_num, com_status, kl_com_rk_rdy);
    print_com_klad_debug(com_kl_num, com_status, kl_com_lv1_rdy);
    print_com_klad_debug(com_kl_num, com_status, kl_com_lv2_rdy);
    print_com_klad_debug(com_kl_num, com_status, kl_com_lv3_rdy);
    print_com_klad_debug(com_kl_num, com_status, kl_com_lv4_rdy);
    print_com_klad_debug(com_kl_num, com_status, kl_com_lv5_rdy);
    print_com_klad_debug(com_kl_num, com_status, reserved_0);
}

static void _hkl_dump_com_rk_info(hi_u8 com_kl_num, hkl_com_rk_info com_rk_info)
{
    print_com_klad_debug(com_kl_num, com_rk_info, ca_owner_id);
    print_com_klad_debug(com_kl_num, com_rk_info, csa2_spec_check_en);
    print_com_klad_debug(com_kl_num, com_rk_info, reserved_0);
}

static void _hkl_dump_com_rk_tag0(hi_u8 com_kl_num, hkl_com_rk_tag0 com_rk_tag0)
{
    print_com_klad_debug(com_kl_num, com_rk_tag0, reserved_0);
    print_com_klad_debug(com_kl_num, com_rk_tag0, key_decrypt_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag0, key_encrypt_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag0, scipher_port_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag0, mcipher_port_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag0, reserved_1);
    print_com_klad_debug(com_kl_num, com_rk_tag0, tscipher_port_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag0, sm4_engine_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag0, tdes_engine_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag0, aes_engine_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag0, csa3_engine_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag0, csa2_engine_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag0, multi2_engine_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag0, sm3_engine_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag0, sha_hmac_engine_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag0, reserved_2);
    print_com_klad_debug(com_kl_num, com_rk_tag0, kl_stage);
    print_com_klad_debug(com_kl_num, com_rk_tag0, reserved_3);
}

static void _hkl_dump_com_rk_tag1(hi_u8 com_kl_num, hkl_com_rk_tag1 com_rk_tag1)
{
    print_com_klad_debug(com_kl_num, com_rk_tag1, kl_aes_algo_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag1, kl_tdes_algo_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag1, kl_sm4_algo_enable);
    print_com_klad_debug(com_kl_num, com_rk_tag1, reserved_0);
}

static void _hkl_dump_com_rk_tag2(hi_u8 com_kl_num, hkl_com_rk_tag2 com_rk_tag2)
{
    print_com_klad_debug(com_kl_num, com_rk_tag2, reserved_0);
    print_com_klad_debug(com_kl_num, com_rk_tag2, flash_prot_en);
    print_com_klad_debug(com_kl_num, com_rk_tag2, nonce_en);
    print_com_klad_debug(com_kl_num, com_rk_tag2, c2_checksum_en);
    print_com_klad_debug(com_kl_num, com_rk_tag2, cm_checksum_en);
    print_com_klad_debug(com_kl_num, com_rk_tag2, hdcp_rk);
    print_com_klad_debug(com_kl_num, com_rk_tag2, reserved_1);
}

void hkl_dump_com_tag(hi_u8 com_kl_num)
{
    hkl_com_status com_stat;
    hkl_com_rk_info com_rk_info;
    hkl_com_rk_tag0 tag0;
    hkl_com_rk_tag1 tag1;
    hkl_com_rk_tag2 tag2;

    com_stat.u32 = hkl_read_reg(HKL_COM_STAT(com_kl_num));
    _hkl_dump_hkl_com_status(com_kl_num, com_stat);
    com_rk_info.u32 = hkl_read_reg(HKL_COM_RK_INFO(com_kl_num));
    _hkl_dump_com_rk_info(com_kl_num, com_rk_info);
    tag0.u32 = hkl_read_reg(HKL_COM_RK_TAG_0(com_kl_num));
    _hkl_dump_com_rk_tag0(com_kl_num, tag0);
    tag1.u32 = hkl_read_reg(HKL_COM_RK_TAG_1(com_kl_num));
    _hkl_dump_com_rk_tag1(com_kl_num, tag1);
    tag2.u32 = hkl_read_reg(HKL_COM_RK_TAG_2(com_kl_num));
    _hkl_dump_com_rk_tag2(com_kl_num, tag2);
}

static void _hkl_dump_hkl_err(hkl_error hkl_err)
{
    print_reg_err(hkl_err, level_sel_err);
    print_reg_err(hkl_err, algo_sel_err);
    print_reg_err(hkl_err, port_sel_err);
    print_reg_err(hkl_err, dsc_code_err);
    print_reg_err(hkl_err, dec_enc_err);
    print_reg_err(hkl_err, kl_disable_err);
    print_reg_err(hkl_err, key_size_err);
    print_reg_err(hkl_err, rk_busy_err);
    print_reg_err(hkl_err, reserved_0);
    print_reg_err(hkl_err, rk_rdy_err);
    print_reg_err(hkl_err, lvl_rdy_err);
    print_reg_err(hkl_err, lv2_rdy_err);
    print_reg_err(hkl_err, lv3_rdy_err);
    print_reg_err(hkl_err, lv4_rdy_err);
    print_reg_err(hkl_err, reserved_1);
    print_reg_err(hkl_err, tdes_key_err);
    print_reg_err(hkl_err, cw_check_err);
    print_reg_err(hkl_err, reserved_2);
    print_reg_err(hkl_err, stage_cfg_err);
    print_reg_err(hkl_err, rk_access_err);
    print_reg_err(hkl_err, ta_hash_err);
    print_reg_err(hkl_err, ta_rslt_rdy_err);
    print_reg_err(hkl_err, ta_access_err);
    print_reg_err(hkl_err, reserved_3);
}

static void _hkl_dump_kc_err(hkl_kc_error hkl_kc_err)
{
    print_reg_err(hkl_kc_err, tpp_access_err);
    print_reg_err(hkl_kc_err, hpp_access_err);
    print_reg_err(hkl_kc_err, tee_access_err);
    print_reg_err(hkl_kc_err, ree_access_err);
    print_reg_err(hkl_kc_err, csa2_hardonly_err);
    print_reg_err(hkl_kc_err, csa3_hardonly_err);
    print_reg_err(hkl_kc_err, aes_hardonly_err);
    print_reg_err(hkl_kc_err, sm4_hardonly_err);
    print_reg_err(hkl_kc_err, tdes_hardonly_err);
    print_reg_err(hkl_kc_err, multi2_hardonly_err);
    print_reg_err(hkl_kc_err, csa2_dis_err);
    print_reg_err(hkl_kc_err, csa3_dis_err);
    print_reg_err(hkl_kc_err, aes_dis_err);
    print_reg_err(hkl_kc_err, des_dis_err);
    print_reg_err(hkl_kc_err, sm4_dis_err);
    print_reg_err(hkl_kc_err, tdes_dis_err);
    print_reg_err(hkl_kc_err, multi2_dis_err);
    print_reg_err(hkl_kc_err, asa_dis_err);
    print_reg_err(hkl_kc_err, buffer_security_err);
    print_reg_err(hkl_kc_err, reserved_0);
    print_reg_err(hkl_kc_err, enc_dec_err);
    print_reg_err(hkl_kc_err, send_time_out);
    print_reg_err(hkl_kc_err, reserved_1);
}

static void _hkl_dump_err(hkl_error hkl_err, hkl_kc_error hkl_kc_err)
{
    _hkl_dump_hkl_err(hkl_err);
    _hkl_dump_kc_err(hkl_kc_err);
}

hi_s32 hkl_check_err(void)
{
    hkl_error hkl_err;
    hkl_kc_error kc_err;

    hkl_err.u32 = hkl_read_reg(HKL_ERROR);
    kc_err.u32 = hkl_read_reg(HKL_KC_ERROR);
    if ((kc_err.u32 == 0) && (hkl_err.u32 == 0)) {
        hkl_dump_crc();
        return HI_SUCCESS;
    }
    _hkl_dump_err(hkl_err, kc_err);
    if (hkl_err.u32 != 0) {
        return err_code_gen(KL_ERR, hkl_err.u32);
    }
    if (kc_err.u32 != 0) {
        return err_code_gen(KC_ERR, kc_err.u32);
    }
    return HI_FAILURE;
}

static hi_s32 _hkl_com_lock_wait_idle(hkl_com_lock_info *lock_info)
{
#ifdef HI_INT_SUPPORT
    hi_s32 ret;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    ret = osal_wait_timeout_interruptible(&mgmt->kl_com_lock.wait_queue, drv_klad_wait_condition,
                                          &mgmt->kl_com_lock.wait_cond, TIME_OUT);
    mgmt->kl_com_lock.wait_cond = 0;
    lock_info->u32 = mgmt->kl_com_lock.lock_info;

    if (ret == 0) {
        HI_ERR_KLAD("com is busy, IRQ : %d, status : 0x%08x 0x%08x 0x%08x 0x%08x\n",
            HKL_IRQ_NUMBER, hkl_read_reg(HKL_COM_LOCK_INFO),
            hkl_read_reg(HKL_INT), hkl_read_reg(HKL_INT_RAW), hkl_read_reg(HKL_INT_EN));
        return HI_ERR_KLAD_WAIT_TIMEOUT;
    } else {
        HI_INFO_KLAD("cost time %dms, jiffies=%d.\n", TIME_OUT - osal_get_tickcount(),
                     osal_msecs_to_jiffies(TIME_OUT) - ret);
    }

#else
    hi_u32 time_out = LOOP_MAX;

    while (--time_out) {
        lock_info->u32 = hkl_read_reg(HKL_COM_LOCK_INFO);
        if (lock_info->bits.kl_com_lock_busy == 0b10) {
            break;
        }
        osal_udelay(DELEY_TIME);
    }
    if (time_out == 0) {
        HI_ERR_KLAD("hkl lock timeout!");
        return HI_FAILURE;
    }
#endif
    return HI_SUCCESS;
}

static hi_s32 _hkl_get_com_lock_num(hi_u32 *kl_index)
{
    hi_s32 ret;
    hkl_com_lock_info lock_info;
#ifdef HI_KLAD_PERF_SUPPORT
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();
    struct klad_timestamp_logic t;

    timestamp(&t.time_wait_b);
#endif

    lock_info.u32 = 0;
    ret = _hkl_com_lock_wait_idle(&lock_info);
    if (ret  != HI_SUCCESS) {
        print_err_func(_hkl_com_lock_wait_idle, ret);
        return ret;
    }

#ifdef HI_KLAD_PERF_SUPPORT
    timestamp(&t.time_wait_e);

    t.tag = COM_LOCK;

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

    if (lock_info.bits.kl_com_lock_fail == 0b10) {
        *kl_index = lock_info.bits.kl_com_lock_num;
        return HI_SUCCESS;
    }
    return HI_FAILURE;
}

static hi_s32 _hkl_check_com_unlock_info(void)
{
    hkl_com_lock_info lock_info;

    lock_info.u32 = hkl_read_reg(HKL_COM_LOCK_INFO);
    if (lock_info.bits.kl_com_unlock_fail == 0x1) {
        HI_ERR_KLAD("unlock failed.\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_check_com_lock_stat(hi_u32 kl_index)
{
    hkl_com_lock_status lock_stat;

    lock_stat.u32 = hkl_read_reg(HKL_COM_LOCK_STAT(kl_index));
#ifdef HI_TEE_SUPPORT
    if (lock_stat.bits.kl_com_lock_stat != HKL_LOCK_STATE_REE) {
#else
    if (lock_stat.bits.kl_com_lock_stat != HKL_LOCK_STATE_TEE) {
#endif
        HI_ERR_KLAD("get lock_state = 0x%x\n", lock_stat.bits.kl_com_lock_stat);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_com_lock(hi_u32 *com_kl_num)
{
    hi_s32 ret;

    if (com_kl_num == HI_NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }
    _hkl_lock_ctrl_proces(HKL_LOCK_KLAD_COM);
    ret = _hkl_get_com_lock_num(com_kl_num);
    if (ret  != HI_SUCCESS) {
        print_err_func(_hkl_get_com_lock_num, ret);
        return ret;
    }

    ret = hkl_check_com_lock_stat(*com_kl_num);
    if (ret  != HI_SUCCESS) {
        print_err_func(hkl_check_com_lock_stat, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 hkl_com_unlock(hi_u32 com_kl_num)
{
    hi_s32 ret;

    _hkl_unlock_ctrl_proces(HKL_LOCK_KLAD_COM, com_kl_num);

    ret = _hkl_check_com_unlock_info();
    if (ret  != HI_SUCCESS) {
        print_err_func(hkl_check_com_lock_stat, ret);
        return ret;
    }
    return HI_SUCCESS;
}

void hkl_set_fp_lev1(hi_u8 is_dec, hi_u32 com_kl_num)
{
    hkl_fp_ctrl reg;
    hkl_fp_rk_sel rk_sel;

    rk_sel.u32 = hkl_read_reg(HKL_FP_RK_SEL);
    rk_sel.bits.kl_fp_rk_sel = com_kl_num;
    hkl_write_reg(HKL_FP_RK_SEL, rk_sel.u32);

    reg.u32 = hkl_read_reg(HKL_FP_CTRL);
    reg.bits.kl_fp_start = 1;
    reg.bits.kl_fp_level_sel = 0;
    reg.bits.kl_fp_dec_sel = is_dec;
    hkl_write_reg(HKL_FP_CTRL, reg.u32);
    return;
}

void hkl_set_fp_lev2(void)
{
    hkl_fp_ctrl reg;

    reg.u32 = hkl_read_reg(HKL_FP_CTRL);
    reg.bits.kl_fp_start = 1;
    reg.bits.kl_fp_level_sel = 1;
    hkl_write_reg(HKL_FP_CTRL, reg.u32);
    return;
}

hi_s32 hkl_set_fp_rd_disable(void)
{
    hkl_fp_dec_ctrl reg;

    reg.u32 = hkl_read_reg(HKL_FP_DEC_CTRL);
    reg.bits.kl_fp_dec_rd_dis = 0x05;
    hkl_write_reg(HKL_FP_DEC_CTRL, reg.u32);

    reg.u32 = hkl_read_reg(HKL_FP_DEC_CTRL);
    if (reg.bits.kl_fp_dec_rd_dis != 0x05) {
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_generate_fp_dec_key(hi_u32 *rst0, hi_u32 *rst1, hi_u32 *rst2, hi_u32 *rst3)
{
    hkl_fp_status reg;

    reg.u32 = hkl_read_reg(HKL_FP_STAT);
    if (reg.bits.kl_fp_lv1_dec_rdy == 0x01) {
        *rst0 = hkl_read_reg(HKL_FP_DEC_RSLT_0);
        *rst1 = hkl_read_reg(HKL_FP_DEC_RSLT_1);
        *rst2 = hkl_read_reg(HKL_FP_DEC_RSLT_2);
        *rst3 = hkl_read_reg(HKL_FP_DEC_RSLT_3);
        return HI_SUCCESS;
    }
    return HI_FAILURE;
}

static hi_s32 _hkl_fp_lock_wait_idle(hkl_fp_lock_info *lock_info)
{
#ifdef HI_INT_SUPPORT
    hi_s32 ret;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    ret = osal_wait_timeout_interruptible(&mgmt->kl_fp_lock.wait_queue, drv_klad_wait_condition,
                                          &mgmt->kl_fp_lock.wait_cond, TIME_OUT);
    mgmt->kl_com_lock.wait_cond = 0;
    lock_info->u32 = mgmt->kl_fp_lock.lock_info;

    if (ret == 0) {
        HI_ERR_KLAD("rkp is busy, IRQ : %d, status : 0x%08x\n", HKL_IRQ_NUMBER, hkl_read_reg(HKL_FP_LOCK_INFO));
        return HI_ERR_KLAD_WAIT_TIMEOUT;
    } else {
        HI_INFO_KLAD("cost time %dms, jiffies=%d.\n", TIME_OUT - osal_get_tickcount(),
                     osal_msecs_to_jiffies(TIME_OUT) - ret);
    }
#else
    hi_u32 time_out = LOOP_MAX;

    while (time_out--) {
        lock_info->u32 = hkl_read_reg(HKL_FP_LOCK_INFO);
        if (lock_info->bits.kl_fp_lock_busy == 0b10) {
            break;
        }
        osal_udelay(DELEY_TIME);
    }
    if (time_out == 0) {
        HI_ERR_KLAD("hkl lock timeout!");
        return HI_FAILURE;
    }
#endif
    return HI_SUCCESS;
}

static hi_s32 _hkl_get_fp_lock(void)
{
    hi_s32 ret;
    hkl_fp_lock_info lock_info;

    lock_info.u32 = 0;
    ret = _hkl_fp_lock_wait_idle(&lock_info);
    if (ret  != HI_SUCCESS) {
        print_err_func(hkl_check_com_lock_stat, ret);
        return ret;
    }

    if (lock_info.bits.kl_fp_lock_fail == 0b10) {
        return HI_SUCCESS;
    }
    return HI_FAILURE;
}

static hi_s32 _hkl_check_fp_unlock_info(void)
{
    hkl_fp_lock_info lock_info;

    lock_info.u32 = hkl_read_reg(HKL_FP_LOCK_INFO);
    if (lock_info.bits.kl_fp_unlock_fail == 0x1) {
        HI_ERR_KLAD("unlock failed.\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_check_fp_lock_stat(void)
{
    hkl_fp_lock_status lock_stat;

    lock_stat.u32 = hkl_read_reg(HKL_FP_LOCK_STAT);
#ifdef HI_TEE_SUPPORT
    if (lock_stat.bits.kl_fp_lock_stat != HKL_LOCK_STATE_REE) {
#else
    if (lock_stat.bits.kl_fp_lock_stat != HKL_LOCK_STATE_TEE) {
#endif
        HI_ERR_KLAD("unlocked or locked by other CPU.\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_fp_lock(void)
{
    hi_s32 ret;

    _hkl_lock_ctrl_proces(HKL_LOCK_KLAD_FP);
    ret = _hkl_get_fp_lock();
    if (ret  != HI_SUCCESS) {
        print_err_func(_hkl_get_fp_lock, ret);
        return ret;
    }
    ret = hkl_check_fp_lock_stat();
    if (ret  != HI_SUCCESS) {
        print_err_func(hkl_check_fp_lock_stat, ret);
        return ret;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_fp_unlock(void)
{
    hi_s32 ret;

    _hkl_unlock_ctrl_proces(HKL_LOCK_KLAD_FP, 0);
    ret = _hkl_check_fp_unlock_info();
    if (ret  != HI_SUCCESS) {
        print_err_func(hkl_check_fp_lock_stat, ret);
        return ret;
    }
    return HI_SUCCESS;
}

static hi_s32 _hkl_ta_lock_wait_idle(hkl_ta_lock_info *lock_info)
{
#ifdef HI_INT_SUPPORT
    hi_s32 ret;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    ret = osal_wait_timeout_interruptible(&mgmt->kl_ta_lock.wait_queue, drv_klad_wait_condition,
                                          &mgmt->kl_ta_lock.wait_cond, TIME_OUT);
    mgmt->kl_ta_lock.wait_cond = 0;
    lock_info->u32 = mgmt->kl_ta_lock.lock_info;

    if (ret == 0) {
        HI_ERR_KLAD("rkp is busy, IRQ : %d, status : 0x%08x\n", HKL_IRQ_NUMBER, hkl_read_reg(HKL_TA_LOCK_INFO));
        return HI_ERR_KLAD_WAIT_TIMEOUT;
    } else {
        HI_INFO_KLAD("cost time %dms, jiffies=%d.\n", TIME_OUT - osal_get_tickcount(),
                     osal_msecs_to_jiffies(TIME_OUT) - ret);
    }

    lock_info->u32 = hkl_read_reg(HKL_TA_LOCK_INFO);
#else
    hi_u32 time_out = LOOP_MAX;

    while (time_out--) {
        lock_info->u32 = hkl_read_reg(HKL_TA_LOCK_INFO);
        if (lock_info->bits.kl_ta_lock_busy == 0b10) {
            break;
        }
        osal_udelay(DELEY_TIME);
    }
    if (time_out == 0) {
        HI_ERR_KLAD("hkl lock timeout!");
        return HI_FAILURE;
    }
#endif
    return HI_SUCCESS;
}

static hi_s32 _hkl_get_ta_lock(void)
{
    hi_s32 ret;
    hkl_ta_lock_info lock_info;

    lock_info.u32 = 0;
    ret = _hkl_ta_lock_wait_idle(&lock_info);
    if (ret  != HI_SUCCESS) {
        print_err_func(_hkl_ta_lock_wait_idle, ret);
        return ret;
    }
    if (lock_info.bits.kl_ta_lock_fail == 0b10) {
        return HI_SUCCESS;
    }
    return HI_FAILURE;
}

static hi_s32 _hkl_check_ta_unlock_info(void)
{
    hkl_ta_lock_info lock_info;

    lock_info.u32 = hkl_read_reg(HKL_TA_LOCK_INFO);
    if (lock_info.bits.kl_ta_unlock_fail == 0x1) {
        HI_ERR_KLAD("unlock failed.\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_check_ta_lock_stat(void)
{
    hkl_ta_lock_status lock_stat;

    lock_stat.u32 = hkl_read_reg(HKL_TA_LOCK_STAT);
#ifdef HI_TEE_SUPPORT
    if (lock_stat.bits.kl_ta_lock_stat != HKL_LOCK_STATE_REE) {
#else
    if (lock_stat.bits.kl_ta_lock_stat != HKL_LOCK_STATE_TEE) {
#endif
        HI_ERR_KLAD("unlocked or locked by other CPU.\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_ta_lock(void)
{
    hi_s32 ret;

    _hkl_lock_ctrl_proces(HKL_LOCK_KLAD_TA);
    ret = _hkl_get_ta_lock();
    if (ret  != HI_SUCCESS) {
        print_err_func(_hkl_get_ta_lock, ret);
        return ret;
    }

    ret = hkl_check_ta_lock_stat();
    if (ret  != HI_SUCCESS) {
        print_err_func(hkl_check_ta_lock_stat, ret);
        return ret;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_ta_unlock(void)
{
    hi_s32 ret;

    _hkl_unlock_ctrl_proces(HKL_LOCK_KLAD_TA, 0);
    ret = _hkl_check_ta_unlock_info();
    if (ret  != HI_SUCCESS) {
        print_err_func(hkl_check_ta_lock_stat, ret);
        return ret;
    }
    return HI_SUCCESS;
}

static hi_s32 _hkl_nonce_lock_wait_idle(hkl_nonce_lock_info *lock_info)
{
#ifdef HI_INT_SUPPORT
    hi_s32 ret;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    ret = osal_wait_timeout_interruptible(&mgmt->kl_nonce_lock.wait_queue, drv_klad_wait_condition,
                                          &mgmt->kl_nonce_lock.wait_cond, TIME_OUT);
    mgmt->kl_nonce_lock.wait_cond = 0;
    lock_info->u32 = mgmt->kl_nonce_lock.lock_info;

    if (ret == 0) {
        HI_ERR_KLAD("rkp is busy, IRQ : %d, status : 0x%08x\n", HKL_IRQ_NUMBER, hkl_read_reg(HKL_NONCE_LOCK_INFO));
        return HI_ERR_KLAD_WAIT_TIMEOUT;
    } else {
        HI_INFO_KLAD("cost time %dms, jiffies=%d.\n", TIME_OUT - osal_get_tickcount(),
                     osal_msecs_to_jiffies(TIME_OUT) - ret);
    }

    lock_info->u32 = hkl_read_reg(HKL_NONCE_LOCK_INFO);
#else
    hi_u32 time_out = LOOP_MAX;

    while (time_out--) {
        lock_info->u32 = hkl_read_reg(HKL_NONCE_LOCK_INFO);
        if (lock_info->bits.kl_nonce_lock_busy == 0b10) {
            break;
        }
        osal_udelay(DELEY_TIME);
    }
    if (time_out == 0) {
        HI_ERR_KLAD("hkl lock timeout!");
        return HI_FAILURE;
    }
#endif
    return HI_SUCCESS;
}

static hi_s32 _hkl_get_nonce_lock(void)
{
    hi_s32 ret;
    hkl_nonce_lock_info lock_info;

    lock_info.u32 = 0;
    ret = _hkl_nonce_lock_wait_idle(&lock_info);
    if (ret  != HI_SUCCESS) {
        print_err_func(_hkl_nonce_lock_wait_idle, ret);
        return ret;
    }
    if (lock_info.bits.kl_nonce_lock_fail == 0b10) {
        return HI_SUCCESS;
    }
    return HI_FAILURE;
}

static hi_s32 _hkl_check_nonce_unlock_info(void)
{
    hkl_nonce_lock_info lock_info;

    lock_info.u32 = hkl_read_reg(HKL_NONCE_LOCK_INFO);
    if (lock_info.bits.kl_nonce_unlock_fail == 0x1) {
        HI_ERR_KLAD("unlock failed.\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_check_nonce_lock_stat(void)
{
    hkl_nonce_lock_status lock_stat;

    lock_stat.u32 = hkl_read_reg(HKL_NONCE_LOCK_STAT);
#ifdef HI_TEE_SUPPORT
    if (lock_stat.bits.kl_nonce_lock_stat != HKL_LOCK_STATE_REE) {
#else
    if (lock_stat.bits.kl_nonce_lock_stat != HKL_LOCK_STATE_TEE) {
#endif
        HI_ERR_KLAD("unlocked or locked by other CPU.\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_nonce_lock(void)
{
    hi_s32 ret;

    _hkl_lock_ctrl_proces(HKL_LOCK_KLAD_NONCE);
    ret = _hkl_get_nonce_lock();
    if (ret  != HI_SUCCESS) {
        print_err_func(_hkl_nonce_lock_wait_idle, ret);
        return ret;
    }
    ret = hkl_check_nonce_lock_stat();
    if (ret  != HI_SUCCESS) {
        print_err_func(_hkl_nonce_lock_wait_idle, ret);
        return ret;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_nonce_unlock(void)
{
    hi_s32 ret;

    _hkl_unlock_ctrl_proces(HKL_LOCK_KLAD_NONCE, 0);
    ret = _hkl_check_nonce_unlock_info();
    if (ret  != HI_SUCCESS) {
        print_err_func(_hkl_nonce_lock_wait_idle, ret);
        return ret;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_set_clr_ctrl(hi_s32 key_size, hi_bool is_iv)
{
    hkl_clr_ctrl reg;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    if (key_size > 0x03) {
        return HI_FAILURE;
    }
    reg.u32 = hkl_read_reg(HKL_CLR_CTRL);
    reg.bits.kl_clr_start = 1;
    reg.bits.kl_clr_iv_sel = is_iv;
    reg.bits.kl_clr_key_size = key_size;
    timestamp(&mgmt->time_ctl_b);
    hkl_write_reg(HKL_CLR_CTRL, reg.u32);
    timestamp(&mgmt->time_ctl_e);

    return HI_SUCCESS;
}

static hi_s32 _hkl_clrcw_lock_wait_idle(hkl_clr_lock_info *lock_info)
{
#ifdef HI_INT_SUPPORT
    hi_s32 ret;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    ret = osal_wait_timeout_interruptible(&mgmt->kl_clr_lock.wait_queue, drv_klad_wait_condition,
                                          &mgmt->kl_clr_lock.wait_cond, TIME_OUT);
    mgmt->kl_clr_lock.wait_cond = 0;
    lock_info->u32 = mgmt->kl_clr_lock.lock_info;

    if (ret == 0) {
        HI_ERR_KLAD("hkl is busy, IRQ : %d, status : 0x%08x\n", HKL_IRQ_NUMBER, hkl_read_reg(HKL_CLR_LOCK_INFO));
        return HI_ERR_KLAD_WAIT_TIMEOUT;
    } else {
        HI_INFO_KLAD("cost time %dms, jiffies=%d.\n", TIME_OUT - osal_get_tickcount(),
                     osal_msecs_to_jiffies(TIME_OUT) - ret);
    }

    lock_info->u32 = hkl_read_reg(HKL_CLR_LOCK_INFO);
#else
    hi_u32 time_out = LOOP_MAX;

    while (time_out--) {
        lock_info->u32 = hkl_read_reg(HKL_CLR_LOCK_INFO);
        if (lock_info->bits.kl_clr_lock_busy == 0b10) {
            break;
        }
        osal_udelay(DELEY_TIME);
    }
    if (time_out == 0) {
        HI_ERR_KLAD("hkl lock timeout!");
        return HI_FAILURE;
    }
#endif
    return HI_SUCCESS;
}

static hi_s32 _hkl_get_clrcw_lock(void)
{
    hi_s32 ret;
    hkl_clr_lock_info lock_info;
#ifdef HI_KLAD_PERF_SUPPORT
    struct klad_timestamp_logic t;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    timestamp(&t.time_wait_b);
#endif
    lock_info.u32 = 0;
    ret = _hkl_clrcw_lock_wait_idle(&lock_info);
    if (ret  != HI_SUCCESS) {
        print_err_func(_hkl_clrcw_lock_wait_idle, ret);
        return ret;
    }

#ifdef HI_KLAD_PERF_SUPPORT
    timestamp(&t.time_wait_e);
    t.tag = CLR_LOCK;

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

    if (lock_info.bits.kl_clr_lock_fail == 0b10) {
        return HI_SUCCESS;
    }
    return HI_FAILURE;
}

static hi_s32 _hkl_check_clrcw_unlock_info(void)
{
    hkl_clr_lock_info lock_info;

    lock_info.u32 = hkl_read_reg(HKL_CLR_LOCK_INFO);
    if (lock_info.bits.kl_clr_unlock_fail == 0x1) {
        HI_ERR_KLAD("unlock failed.\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_check_clrcw_lock_stat(void)
{
    hkl_clr_lock_status lock_stat;

    lock_stat.u32 = hkl_read_reg(HKL_CLR_LOCK_STAT);
#ifdef HI_TEE_SUPPORT
    if (lock_stat.bits.kl_clr_lock_stat != HKL_LOCK_STATE_REE) {
#else
    if (lock_stat.bits.kl_clr_lock_stat != HKL_LOCK_STATE_TEE) {
#endif
        HI_ERR_KLAD("unlocked or locked by other CPU.\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_clrcw_lock(void)
{
    hi_s32 ret;

    _hkl_lock_ctrl_proces(HKL_LOCK_KLAD_CLR);
    ret = _hkl_get_clrcw_lock();
    if (ret  != HI_SUCCESS) {
        print_err_func(_hkl_get_clrcw_lock, ret);
        return ret;
    }
    ret = hkl_check_clrcw_lock_stat();
    if (ret  != HI_SUCCESS) {
        print_err_func(hkl_check_clrcw_lock_stat, ret);
        return ret;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_clrcw_unlock(void)
{
    hi_s32 ret;

    _hkl_unlock_ctrl_proces(HKL_LOCK_KLAD_CLR, 0);
    ret = _hkl_check_clrcw_unlock_info();
    if (ret  != HI_SUCCESS) {
        print_err_func(_hkl_check_clrcw_unlock_info, ret);
        return ret;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_set_csgk2_ctrl(void)
{
    hkl_csgk2_ctrl reg;

    reg.u32 = hkl_read_reg(HKL_CSGK2_CTRL);
    reg.bits.kl_csgk2_start = 1;
    hkl_write_reg(HKL_CSGK2_CTRL, reg.u32);
    return HI_SUCCESS;
}

static hi_s32 _hkl_csgk2_lock_wait_idle(hkl_csgk2_lock_info *lock_info)
{
#ifdef HI_INT_SUPPORT
    hi_s32 ret;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    ret = osal_wait_timeout_interruptible(&mgmt->kl_csgk2_lock.wait_queue, drv_klad_wait_condition,
                                          &mgmt->kl_csgk2_lock.wait_cond, TIME_OUT);
    mgmt->kl_csgk2_lock.wait_cond = 0;
    lock_info->u32 = mgmt->kl_csgk2_lock.lock_info;

    if (ret == 0) {
        HI_ERR_KLAD("rkp is busy, IRQ : %d, status : 0x%08x\n", HKL_IRQ_NUMBER, hkl_read_reg(HKL_CSGK2_LOCK_INFO));
        return HI_ERR_KLAD_WAIT_TIMEOUT;
    } else {
        HI_INFO_KLAD("cost time %dms, jiffies=%d.\n", TIME_OUT - osal_get_tickcount(),
                     osal_msecs_to_jiffies(TIME_OUT) - ret);
    }
    lock_info->u32 = hkl_read_reg(HKL_CSGK2_LOCK_INFO);
#else
    hi_u32 time_out = LOOP_MAX;

    while (time_out--) {
        lock_info->u32 = hkl_read_reg(HKL_CSGK2_LOCK_INFO);
        if (lock_info->bits.kl_csgk2_lock_busy == 0b10) {
            break;
        }
        osal_udelay(DELEY_TIME);
    }
    if (time_out == 0) {
        HI_ERR_KLAD("hkl lock timeout!");
        return HI_FAILURE;
    }
#endif
    return HI_SUCCESS;
}

static hi_s32 _hkl_get_csgk2_lock(void)
{
    hi_s32 ret;
    hkl_csgk2_lock_info lock_info;

    lock_info.u32 = 0;
    ret = _hkl_csgk2_lock_wait_idle(&lock_info);
    if (ret  != HI_SUCCESS) {
        print_err_func(_hkl_csgk2_lock_wait_idle, ret);
        return ret;
    }
    if (lock_info.bits.kl_csgk2_lock_fail == 0b10) {
        return HI_SUCCESS;
    }
    return HI_FAILURE;
}

static hi_s32 _hkl_check_csgk2_unlock_info(void)
{
    hkl_csgk2_lock_info lock_info;

    lock_info.u32 = hkl_read_reg(HKL_CSGK2_LOCK_INFO);
    if (lock_info.bits.kl_csgk2_unlock_fail == 0x1) {
        HI_ERR_KLAD("unlock failed.\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_check_csgk2_lock_stat(void)
{
    hkl_csgk2_lock_status lock_stat;

    lock_stat.u32 = hkl_read_reg(HKL_CSGK2_LOCK_STATUS);
#ifdef HI_TEE_SUPPORT
    if (lock_stat.bits.kl_csgk2_lock_stat != HKL_LOCK_STATE_REE) {
#else
    if (lock_stat.bits.kl_csgk2_lock_stat != HKL_LOCK_STATE_TEE) {
#endif
        HI_ERR_KLAD("unlocked or locked by other CPU.\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_csgk2_lock(void)
{
    hi_s32 ret;

    _hkl_lock_ctrl_proces(HKL_LOCK_KLAD_CSGK2);
    ret = _hkl_get_csgk2_lock();
    if (ret  != HI_SUCCESS) {
        print_err_func(_hkl_get_csgk2_lock, ret);
        return ret;
    }
    ret = hkl_check_csgk2_lock_stat();
    if (ret  != HI_SUCCESS) {
        print_err_func(hkl_check_csgk2_lock_stat, ret);
        return ret;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_csgk2_unlock(void)
{
    hi_s32 ret;

    _hkl_unlock_ctrl_proces(HKL_LOCK_KLAD_CSGK2, 0);
    ret = _hkl_check_csgk2_unlock_info();
    if (ret  != HI_SUCCESS) {
        print_err_func(_hkl_check_csgk2_unlock_info, ret);
        return ret;
    }
    return HI_SUCCESS;
}

hi_s32 hkl_set_csgk2_disable(void)
{
    hkl_csgk2_disable reg;
    hkl_csgk2_disable_lock reg1;

    reg.u32 = hkl_read_reg(HKL_CSGK2_DISABLE);
    reg1.u32 = hkl_read_reg(HKL_CSGK2_DISABLE_LOCK);
    if (reg1.bits.kl_csgk2_dis_lock == 0x1) {
        if (reg.bits.kl_csgk2_dis != 0x0a) {
            return HI_SUCCESS;
        }
        return HI_FAILURE;
    }
    reg.bits.kl_csgk2_dis = 0xf;
    hkl_write_reg(HKL_FP_DEC_CTRL, reg.u32);

    reg1.bits.kl_csgk2_dis_lock = 0x1;
    hkl_write_reg(HKL_CSGK2_DISABLE_LOCK, reg1.u32);

    reg.u32 = hkl_read_reg(HKL_CSGK2_DISABLE);
    if (reg.bits.kl_csgk2_dis != 0xff) {
        return HI_FAILURE;
    }
    reg1.u32 = hkl_read_reg(HKL_CSGK2_DISABLE_LOCK);
    if (reg1.bits.kl_csgk2_dis_lock != 0x01) {
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

void hkl_get_cipher_key(void)
{
    hkl_read_reg(KLAD_REG_BASE + 0xf20);
    hkl_read_reg(KLAD_REG_BASE + 0xf24);
    hkl_read_reg(KLAD_REG_BASE + 0xf28);
    hkl_read_reg(KLAD_REG_BASE + 0xf2c);
    hkl_read_reg(KLAD_REG_BASE + 0xf30);
    hkl_read_reg(KLAD_REG_BASE + 0xf34);
    hkl_read_reg(KLAD_REG_BASE + 0xf38);
    hkl_read_reg(KLAD_REG_BASE + 0xf3c);
}

#ifdef HI_INT_SUPPORT

hi_void hkl_int_write_reg(const hi_u32 addr, const hi_u32 val)
{
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    reg_write((mgmt->io_base + (addr - KLAD_REG_BASE)), val);
    HI_INFO_KLAD("int w 0x%08x 0x%08x\n", addr, val);
    return;
}

hi_u32 hkl_int_read_reg(const hi_u32 addr)
{
    hi_u32 val = 0;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    reg_read((mgmt->io_base + (addr - KLAD_REG_BASE)), val);
    HI_INFO_KLAD("int r 0x%08x 0x%08x\n", addr, val);

    return val;
}

hi_void hal_klad_lock_int_en(hi_void)
{
    hkl_int_en int_reg;

    int_reg.u32 = hkl_int_read_reg(HKL_INT_EN);
    int_reg.bits.kl_lock_int_en = HI_TRUE;
    hkl_int_write_reg(HKL_INT_EN, int_reg.u32);
    return;
}

hi_void hal_klad_lock_int_dis(hi_void)
{
    hkl_int_en int_reg;

    int_reg.u32 = hkl_int_read_reg(HKL_INT_EN);
    int_reg.bits.kl_lock_int_en = HI_FALSE;
    hkl_int_write_reg(HKL_INT_EN, int_reg.u32);
    return;
}

hi_void hal_klad_csgk2_lock_int_clr(hi_void)
{
    hkl_int_raw raw;

    raw.u32 = hkl_int_read_reg(HKL_INT_RAW);
    raw.bits.kl_csgk2_lock_int_raw = HI_TRUE;
    hkl_int_write_reg(HKL_INT_RAW, raw.u32);
    return;
}

hi_void hal_klad_ta_lock_int_clr(hi_void)
{
    hkl_int_raw raw;

    raw.u32 = hkl_int_read_reg(HKL_INT_RAW);
    raw.bits.kl_ta_lock_int_raw = HI_TRUE;
    hkl_int_write_reg(HKL_INT_RAW, raw.u32);
    return;
}

hi_void hal_klad_fp_lock_int_clr(hi_void)
{
    hkl_int_raw raw;

    raw.u32 = hkl_int_read_reg(HKL_INT_RAW);
    raw.bits.kl_fp_lock_int_raw = HI_TRUE;
    hkl_int_write_reg(HKL_INT_RAW, raw.u32);
    return;
}

hi_void hal_klad_clr_lock_int_clr(hi_void)
{
    hkl_int_raw raw;

    raw.u32 = hkl_int_read_reg(HKL_INT_RAW);
    raw.bits.kl_clr_lock_int_raw = HI_TRUE;
    hkl_int_write_reg(HKL_INT_RAW, raw.u32);
    return;
}

hi_void hal_klad_nonce_lock_int_clr(hi_void)
{
    hkl_int_raw raw;

    raw.u32 = hkl_int_read_reg(HKL_INT_RAW);
    raw.bits.kl_nonce_lock_int_raw = HI_TRUE;
    hkl_int_write_reg(HKL_INT_RAW, raw.u32);
    return;
}

hi_void hal_klad_com_lock_int_clr(hi_void)
{
    hkl_int_raw raw;

    raw.u32 = hkl_int_read_reg(HKL_INT_RAW);
    raw.bits.kl_com_lock_int_raw = HI_TRUE;
    hkl_int_write_reg(HKL_INT_RAW, raw.u32);
    return;
}

hi_void hal_klad_lock_int_clr(hi_void)
{
    hal_klad_csgk2_lock_int_clr();
    hal_klad_ta_lock_int_clr();
    hal_klad_fp_lock_int_clr();
    hal_klad_clr_lock_int_clr();
    hal_klad_nonce_lock_int_clr();
    hal_klad_com_lock_int_clr();
}

hi_void hal_klad_int_en(hi_void)
{
    hkl_int_en int_reg;

    int_reg.u32 = hkl_read_reg(HKL_INT_EN);
    int_reg.bits.kl_int_en = HI_TRUE;
    hkl_write_reg(HKL_INT_EN, int_reg.u32);
    return;
}

hi_void hal_klad_int_dis(hi_void)
{
    hkl_int_en int_reg;

    int_reg.u32 = hkl_read_reg(HKL_INT_EN);
    int_reg.bits.kl_int_en = HI_FALSE;
    hkl_write_reg(HKL_INT_EN, int_reg.u32);
    return;
}

hi_void hal_klad_int_clr(hi_void)
{
    hkl_int_raw raw;

    raw.u32 = hkl_read_reg(HKL_INT_RAW);
    raw.bits.kl_int_raw = HI_TRUE;
    hkl_write_reg(HKL_INT_RAW, raw.u32);
    return;
}

hi_u32 hal_klad_get_int_stat(hi_void)
{
    return hkl_read_reg(HKL_INT);
}
#endif

