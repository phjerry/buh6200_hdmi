/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:OTP driver in register level.
 * Author: Linux SDK team
 * Create: 2019/06/22
 */
#include "hal_otp.h"

#include "hi_osal.h"
#include "drv_otp_define.h"
#ifdef HI_PROC_SUPPORT
#include "drv_otp_proc.h"
#endif
#include "drv_ioctl_otp.h"
#include "drv_otp.h"
#include "hal_otp_reg.h"

#define LOOP_MAX 100000 /* Waiting for logic completion count. */
#define DELEY_TIME_CNT 10 /* Delay time, xxx us. */

/* static function */
static hi_void _otp_write_reg(hi_u32 addr, hi_u32 val)
{
    struct otp_mgmt *mgmt = __get_otp_mgmt();

    reg_write((mgmt->io_base + (addr - PERM_REG_BASE)), val);
    HI_INFO_OTP("W 0x%x 0x%x\n", addr, val);
    return;
}

static hi_u32 _otp_read_reg(hi_u32 addr)
{
    hi_u32 val = 0;
    struct otp_mgmt *mgmt = __get_otp_mgmt();

    reg_read((mgmt->io_base + (addr - PERM_REG_BASE)), val);
    HI_INFO_OTP("R 0x%x 0x%x\n", addr, val);
    return val;
}

static hi_s32 _otp_check_error_status(hi_void)
{
    otp_ctr_st0 st0;

    st0.u32 = _otp_read_reg(OTP_CTR_ST0);
    if (st0.bits.otp_init_rdy == 0) {
        print_dbg_hex3(OTP_CTR_ST0, st0.u32, HI_ERR_OTP_NOT_INITRDY);
        return HI_ERR_OTP_NOT_INITRDY;
    }
    if (st0.bits.err == 1) {
        print_dbg_hex3(OTP_CTR_ST0, st0.u32, HI_ERR_OTP_PROG_PERM);
        return HI_ERR_OTP_PROG_PERM;
    }
    if (st0.bits.prm_rd_fail == 1) {
        print_dbg_hex3(OTP_CTR_ST0, st0.u32, HI_ERR_OTP_FAIL_PRMRD);
        return HI_ERR_OTP_FAIL_PRMRD;
    }
    if (st0.bits.rd_fail == 1) {
        print_dbg_hex3(OTP_CTR_ST0, st0.u32, HI_ERR_OTP_FAIL_RD);
        return HI_ERR_OTP_FAIL_RD;
    }
    if (st0.bits.prog_disable == 1) {
        print_dbg_hex3(OTP_CTR_ST0, st0.u32, HI_ERR_OTP_DISABLE_PROG);
        return HI_ERR_OTP_DISABLE_PROG;
    }
    return HI_SUCCESS;
}

static hi_s32 _otp_wait_ctrl_idle(hi_void)
{
    otp_rw_ctrl reg;
    hi_u32 cnt = 0;

    reg.u32 = _otp_read_reg(OTP_RW_CTRL);
    while ((reg.bits.start == HI_TRUE) && (cnt++ < LOOP_MAX)) {
        reg.u32 = _otp_read_reg(OTP_RW_CTRL);
        osal_udelay(DELEY_TIME_CNT);
    }
    if (cnt >= LOOP_MAX) {
        print_err_hex4(OTP_RW_CTRL, reg.u32, cnt, HI_ERR_OTP_DISABLE_PROG);
        return HI_ERR_OTP_TIMEOUT;
    }
    return HI_SUCCESS;
}

hi_s32 hal_otp_init(hi_void)
{
    struct otp_mgmt *mgmt = __get_otp_mgmt();

    mgmt->io_base = osal_ioremap_nocache(PERM_REG_BASE, PERM_REG_RANGE);
    if (mgmt->io_base == HI_NULL) {
        print_err_hex3(PERM_REG_BASE, PERM_REG_RANGE, HI_ERR_OTP_MEM_MAP);
        return HI_ERR_OTP_MEM_MAP;
    }
    return HI_SUCCESS;
}

hi_void hal_otp_deinit(hi_void)
{
    struct otp_mgmt *mgmt = __get_otp_mgmt();

    osal_iounmap(mgmt->io_base);
    return;
}

hi_s32 hal_otp_reset(hi_void)
{
    otp_ctr_st0 otp_ctr_st0;
    hi_u32 cnt = 0;

    OTP_FUNC_ENTER();
    _otp_write_reg(OTP_SH_UPDATE, 0x1); /* Shadow register update request.0:no request; 1:request */

    otp_ctr_st0.u32 = _otp_read_reg(OTP_CTR_ST0);
    while ((otp_ctr_st0.bits.soft_req_otp_rdy == HI_FALSE) && (cnt++ < LOOP_MAX)) {
        otp_ctr_st0.u32 = _otp_read_reg(OTP_CTR_ST0);
        osal_udelay(DELEY_TIME_CNT);
    }
    if (cnt >= LOOP_MAX) {
        print_err_hex4(OTP_CTR_ST0, otp_ctr_st0.u32, cnt, HI_ERR_OTP_TIMEOUT);
        return HI_ERR_OTP_TIMEOUT;
    }
    OTP_FUNC_EXIT();
    return HI_SUCCESS;
}

hi_s32 hal_otp_read(hi_u32 addr, hi_u32 *value)
{
    hi_s32 ret;
    otp_rw_ctrl reg_ctr;
    /* ****** proc virtualotp begin ******* */
#ifdef HI_PROC_SUPPORT
    ret = fake_otp_virtual_read(addr, value);
    if (ret != HI_ERR_OTP_NON_FAKE_MODE) {
        return ret;
    }
#endif
    /* ****** proc virtualotp end   ******* */
    if (value == HI_NULL) {
        print_err_code(HI_ERR_OTP_PTR_NULL);
        return HI_ERR_OTP_PTR_NULL;
    }
    _otp_write_reg(OTP_RADDR, addr);

    reg_ctr.bits.wr_sel = OTP_OPT_READ;
    reg_ctr.bits.start = HI_TRUE;
    _otp_write_reg(OTP_RW_CTRL, reg_ctr.u32);

    ret = _otp_wait_ctrl_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(_otp_wait_ctrl_idle, ret);
        return ret;
    }
    *value = _otp_read_reg(OTP_RDATA);

    return _otp_check_error_status();
}

hi_s32 hal_otp_write(hi_u32 addr, hi_u32 value)
{
    hi_s32 ret;
    hi_u32 i;

    if (IS_WORD_ALIGN(addr)) {
        print_err_code(HI_ERR_OTP_INVALID_PARA);
        return HI_ERR_OTP_INVALID_PARA;
    }
    for (i = 0; i < WORD_SIZE; i++) {
        /* Only genarate 8 bits */
        ret = hal_otp_write_byte(addr + i, (value >> (BYTE_WIDTH * i)) & 0xff);
        if (ret != HI_SUCCESS) {
            print_err_func_hex3(hal_otp_write_byte, addr + i, (value >> (BYTE_WIDTH * i)) & 0xff, ret);
            return ret;
        }
    }
    return HI_SUCCESS;
}

hi_s32 hal_otp_read_byte(hi_u32 addr, hi_u8 *value)
{
    hi_s32 ret;
    hi_u32 v = 0;

    if (value == HI_NULL) {
        print_err_code(HI_ERR_OTP_PTR_NULL);
        return HI_ERR_OTP_PTR_NULL;
    }
    ret = hal_otp_read(WORD_ALIGN(addr), &v);
    if (ret != HI_SUCCESS) {
        print_err_func_hex2(hal_otp_read, WORD_ALIGN(addr), ret);
        return ret;
    }
    *value = BYTE_GEN(v, addr);
    return HI_SUCCESS;
}

hi_s32 hal_otp_write_byte(hi_u32 addr, hi_u8 value)
{
    hi_s32 ret;
    otp_rw_ctrl reg_ctr;
    /* ****** proc virtualotp begin ******* */
#ifdef HI_PROC_SUPPORT
    ret = fake_otp_virtual_write_byte(addr, value);
    if (ret == HI_SUCCESS) {
        return ret;
    } else if (ret != HI_ERR_OTP_NON_FAKE_MODE) {
        print_err_func_hex3(fake_otp_virtual_write_byte, addr, value, ret);
        return ret;
    } else {
    }
#endif
    /* ****** proc virtualotp end   ******* */
    _otp_write_reg(OTP_WADDR, addr);
    _otp_write_reg(OTP_WDATA, value);

    _otp_write_reg(OTP_RW_CTRL, 0x5);
    reg_ctr.bits.wr_sel = OTP_OPT_WRITE;
    reg_ctr.bits.start = HI_TRUE;
    _otp_write_reg(OTP_RW_CTRL, reg_ctr.u32);

    ret = _otp_wait_ctrl_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(_otp_wait_ctrl_idle, ret);
        return ret;
    }
    return _otp_check_error_status();
}

hi_s32 hal_otp_write_bit(hi_u32 addr, hi_u32 bit_pos)
{
    hi_s32 ret;
    hi_u8 data;

    if (bit_pos >= BYTE_WIDTH) {
        print_err_hex2(bit_pos, HI_ERR_OTP_INVALID_PARA);
        return HI_ERR_OTP_INVALID_PARA;
    }
    data = (1 << bit_pos);
    ret = hal_otp_write_byte(addr, data);
    if (ret != HI_SUCCESS) {
        print_err_func_hex3(hal_otp_write_byte, addr, data, ret);
        return ret;
    }
    return HI_SUCCESS;
}

hi_s32 hal_otp_read_bits_onebyte(hi_u32 addr, hi_u32 start_bit, hi_u32 bit_width, hi_u8 *value)
{
    hi_s32 ret;
    hi_u8 data = 0;

    if (start_bit + bit_width > BYTE_WIDTH) {
        print_err_hex3(start_bit, bit_width, HI_ERR_OTP_INVALID_PARA);
        return HI_ERR_OTP_INVALID_PARA;
    }
    ret = hal_otp_read_byte(addr, &data);
    if (ret != HI_SUCCESS) {
        print_err_func_hex2(hal_otp_read_byte, addr, ret);
        return ret;
    }
    data &= GEN_MASK(start_bit + bit_width - 1, start_bit);
    *value = data >> start_bit;

    return HI_SUCCESS;
}

hi_s32 hal_otp_write_bits_onebyte(hi_u32 addr, hi_u32 start_bit, hi_u32 bit_width, hi_u8 value)
{
    hi_s32 ret;
    hi_u8 data;

    if (start_bit + bit_width > BYTE_WIDTH) {
        print_err_hex3(start_bit, bit_width, HI_ERR_OTP_INVALID_PARA);
        return HI_ERR_OTP_INVALID_PARA;
    }
    data = (value << start_bit) & GEN_MASK(start_bit + bit_width - 1, start_bit);
    ret = hal_otp_write_byte(addr, data);
    if (ret != HI_SUCCESS) {
        print_err_func_hex3(hal_otp_write_byte, addr, data, ret);
        return ret;
    }
    return HI_SUCCESS;
}

