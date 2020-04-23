/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: sharpen hal layer function file
 * Author: pq
 * Create: 2019-11-11
 */


#include "pq_hal_comm.h"
#include "hal_pq_ip_coef.h"
#include "pq_hal_sharpen.h"
#include "pq_hal_sharpen_regset.h"

#define LUT_LENGTH 256

static vdp_regs_type *g_vdp_sharp_reg = HI_NULL;
/* vdp sharpen reg addr */
static hi_u64 g_vdp_sharpen_reg_vir_addr = HI_NULL;
static hi_u8 *g_send_coef_offset_addr = HI_NULL;

static pq_sharp_coef_addr g_sharp_hal_coef_buf = { 0 };
static hi_u8 g_color_lut[LUT_LENGTH];
static hi_u8 g_color_lut_new[LUT_LENGTH];

static hi_u8 g_sharp_color_lut[16][16] = { /* 16:row and col */
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 0,  16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 }
};

hi_s32 pq_hal_sharp_reset_addr(hi_void)
{
    g_vdp_sharp_reg = HI_NULL;
    g_vdp_sharpen_reg_vir_addr = HI_NULL;
    memset(&g_sharp_hal_coef_buf, 0x0, sizeof(g_sharp_hal_coef_buf));

    return HI_SUCCESS;
}

static hi_void pq_hal_sharp_set_reg_vir_addr(uintptr_t vir_addr)
{
    g_vdp_sharpen_reg_vir_addr = (hi_u64)vir_addr;
    return;
}

static hi_s32 pq_hal_sharp_send_lut_coef(hi_void)
{
    hi_u32 i, j;
    pq_coef_gen_cfg coef_gen = { 0 };
    pq_coef_send_cfg coef_send = { 0 };
    hi_u32 lut_lenght = LUT_LENGTH;
    hi_u32 coef_bit_len = 8; /* 8:bit_len */
    hi_void *coef_array[1];

    if (g_sharp_hal_coef_buf.vir_addr[SHARPEN_COEF_BUF_FHD] == HI_NULL) {
        HI_ERR_PQ("g_sharp_hal_coef_buf is null!\n");
        return HI_ERR_PQ_INVALID_PARA;
    }

    for (i = 0; i < 16; i++) {  /* 16:row of lut. */
        for (j = 0; j < 16; j++) {  /* 16:column of lut. */
            g_color_lut[i * 16 + j] = g_sharp_color_lut[i][j];
        }
    }

    coef_gen.p_coef         = g_color_lut;
    coef_gen.p_coef_new     = g_color_lut_new;
    coef_gen.length         = LUT_LENGTH;
    coef_gen.coef_data_type = DRV_COEF_DATA_TYPE_U8;
    pq_drv_gen_coef(&coef_gen);

    coef_array[0] = g_color_lut_new;
    coef_send.coef_addr        = g_sharp_hal_coef_buf.vir_addr[SHARPEN_COEF_BUF_FHD];
    coef_send.sti_type         = PQ_STI_FILE_COEF_SHARPEN;
    coef_send.fp_coef          = HI_NULL;
    coef_send.lut_num          = 1;
    coef_send.burst_num        = 1;
    coef_send.cycle_num        = 16; /* 16:cycle_num */
    coef_send.p_coef_array     = coef_array;
    coef_send.lut_length       = &lut_lenght;
    coef_send.coef_bit_length  = &coef_bit_len;
    coef_send.data_type        = DRV_COEF_DATA_TYPE_U8;
    g_send_coef_offset_addr = pq_send_coef2ddr(&coef_send);

    return HI_SUCCESS;
}

hi_s32 pq_hal_sharp_init_hal(hi_u8 *vir_addr, dma_addr_t phy_addr)
{
    hi_s32 ret;

    if (vir_addr == HI_NULL || phy_addr == HI_NULL) {
        HI_ERR_PQ("vir_addr or phy_addr invalid!\n");
        return HI_ERR_PQ_INVALID_PARA;
    }

    g_vdp_sharp_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_sharp_reg);

    g_sharp_hal_coef_buf.vir_addr[SHARPEN_COEF_BUF_FHD] = vir_addr;
    g_sharp_hal_coef_buf.phy_addr[SHARPEN_COEF_BUF_FHD] = phy_addr;

    g_sharp_hal_coef_buf.vir_addr[SHARPEN_COEF_BUF_DDR_REG] = g_sharp_hal_coef_buf.vir_addr[SHARPEN_COEF_BUF_FHD] + \
                                                              SHARPEN_COEF_SIZE;
    g_sharp_hal_coef_buf.phy_addr[SHARPEN_COEF_BUF_DDR_REG] = g_sharp_hal_coef_buf.phy_addr[SHARPEN_COEF_BUF_FHD] + \
                                                              SHARPEN_COEF_SIZE;

    pq_reg_sharp_para_addr_v0_chn14(g_vdp_sharp_reg, g_sharp_hal_coef_buf.phy_addr[SHARPEN_COEF_BUF_FHD]);
    pq_hal_sharp_set_reg_vir_addr((uintptr_t)g_sharp_hal_coef_buf.vir_addr[SHARPEN_COEF_BUF_DDR_REG]);

    /* send static lut coef. */
    ret = pq_hal_sharp_send_lut_coef();
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_hal_sharp_send_lut_coef fail!\n");
        pq_hal_sharp_reset_addr();
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_sharp_ddr_regread(uintptr_t reg_addr, hi_u32 *reg_value)
{
    hi_u64 addr;

    *reg_value = 0;

    PQ_CHECK_NULL_PTR_RE_FAIL(reg_value);
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_sharp_reg);

    if (g_vdp_sharpen_reg_vir_addr == HI_NULL) {
        HI_ERR_PQ("g_vdp_sharpen_reg_vir_addr is null!\n");
        return HI_ERR_PQ_INVALID_PARA;
    }

    if ((hi_u64)reg_addr < (hi_u64)(&(g_vdp_sharp_reg->voctrl.u32))) {
        HI_ERR_PQ("ptr->%#x is invalid!\n", reg_addr);
        return HI_ERR_PQ_INVALID_PARA;
    }

    addr = VDP_REGS_ADDR + ((hi_u64)reg_addr - (hi_u64)(&(g_vdp_sharp_reg->voctrl.u32)));
    if ((addr < SHARP_VIR_REG_START) || (addr > SHARP_VIR_REG_END)) {
        HI_ERR_PQ("ptr->%#x is invalid!\n", reg_addr);
        return HI_ERR_PQ_INVALID_PARA;
    }

    addr = (addr - SHARP_VIR_REG_START + g_vdp_sharpen_reg_vir_addr);
    *reg_value = *((volatile hi_u32 *)addr);
    return HI_SUCCESS;
}

hi_s32 pq_hal_sharp_ddr_regwrite(uintptr_t reg_addr, hi_u32 value)
{
    hi_u32 *value_addr;
    hi_u64 addr;

    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_sharp_reg);

    if (g_vdp_sharpen_reg_vir_addr == HI_NULL) {
        HI_ERR_PQ("g_vdp_sharpen_reg_vir_addr is null!\n");
        return HI_ERR_PQ_INVALID_PARA;
    }

    if ((hi_u64)reg_addr < (hi_u64)(&(g_vdp_sharp_reg->voctrl.u32))) {
        HI_ERR_PQ("ptr->%#x is invalid!\n", reg_addr);
        return HI_ERR_PQ_INVALID_PARA;
    }

    addr = VDP_REGS_ADDR + ((hi_u64)reg_addr - (hi_u64)(&(g_vdp_sharp_reg->voctrl.u32)));
    if ((addr < SHARP_VIR_REG_START) || (addr > SHARP_VIR_REG_END)) {
        HI_ERR_PQ("ptr->%#x is invalid!\n", reg_addr);
        return HI_ERR_PQ_INVALID_PARA;
    }

    addr = (addr - SHARP_VIR_REG_START + g_vdp_sharpen_reg_vir_addr);
    value_addr  = (hi_u32 *)addr;
    *value_addr = value;

    return HI_SUCCESS;
}

hi_s32 pq_hal_sharp_set_en(hi_bool on_or_off)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_sharp_reg);

    pq_reg_sharp_set_en(g_vdp_sharp_reg, 0, on_or_off);

    return HI_SUCCESS;
}

hi_s32 pq_hal_sharp_set_demo_en(hi_bool on_or_off)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_sharp_reg);

    pq_reg_sharp_set_demo_en(g_vdp_sharp_reg, 0, on_or_off);

    return HI_SUCCESS;
}

hi_s32 pq_hal_sharp_set_demo_mode(pq_demo_mode demo_mode)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_sharp_reg);

    pq_reg_sharp_set_demo_mode(g_vdp_sharp_reg, 0, demo_mode);

    return HI_SUCCESS;
}

hi_s32 pq_hal_sharp_set_demo_pos(hi_u32 x_pos)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_sharp_reg);

    pq_reg_sharp_set_demo_pos(g_vdp_sharp_reg, 0, x_pos);

    return HI_SUCCESS;
}

hi_s32 pq_hal_sharp_set_str(hi_u32 peak_ratio, hi_u32 lti_ratio)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_sharp_reg);

    pq_reg_sharp_set_peak_ratio(g_vdp_sharp_reg, 0, peak_ratio);

    pq_reg_sharp_set_lti_ratio(g_vdp_sharp_reg, 0, lti_ratio);

    return HI_SUCCESS;
}

static hi_s32 pq_hal_sharp_send_reg_coef(hi_u8 *send_coef_offset_addr, hi_u32 *reg_lut)
{
    pq_coef_send_cfg coef_send = { 0 };
    hi_u32 lut_lenght;
    hi_u32 coef_bit_len;
    hi_void *coef_array[1];

    PQ_CHECK_NULL_PTR_RE_FAIL(send_coef_offset_addr);
    PQ_CHECK_NULL_PTR_RE_FAIL(reg_lut);

    coef_array[0] = reg_lut;
    lut_lenght = SHARPEN_REG_DDR_NUM;   /* 70:reg num */
    coef_bit_len = 32; /* 32:bit_len */

    coef_send.coef_addr        = send_coef_offset_addr;
    coef_send.sti_type         = PQ_STI_FILE_COEF_SHARPEN;
    coef_send.fp_coef          = HI_NULL;
    coef_send.lut_num          = 1;
    coef_send.burst_num        = 1;
    coef_send.cycle_num        = 4; /* 4:cycle_num */
    coef_send.p_coef_array     = coef_array;
    coef_send.lut_length       = &lut_lenght;
    coef_send.coef_bit_length  = &coef_bit_len;
    coef_send.data_type        = DRV_COEF_DATA_TYPE_U32;
    pq_send_coef2ddr(&coef_send);

    return HI_SUCCESS;
}

hi_s32 pq_hal_sharp_update_cfg(hi_void)
{
    hi_s32 ret;
    vdp_regs_type *vdp_reg;

    vdp_reg = g_vdp_sharp_reg;
    PQ_CHECK_NULL_PTR_RE_FAIL(vdp_reg);

    /* set coef */
    ret = pq_hal_sharp_send_reg_coef(g_send_coef_offset_addr, (hi_u32 *)g_vdp_sharpen_reg_vir_addr);
    PQ_CHECK_RETURN_SUCCESS(ret);
    /* para update */
    pq_reg_sharp_setparaupv0chn(vdp_reg, SHARPEN_COEF_BUF_FHD);

    return HI_SUCCESS;
}


