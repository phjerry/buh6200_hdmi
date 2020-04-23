/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description:cle hal function.
 * Author: pq
 * Create: 2019-09-21
 */

#include "pq_hal_comm.h"
#include "pq_hal_cle_regset.h"
#include "pq_hal_cle.h"
#include "hal_pq_ip_coef.h"

static vdp_regs_type *g_vdp_cle_reg = HI_NULL;
/* VDP CLE 4k/8k PARA DDR */
static hi_u64 g_vdp_4k_cle_reg_vir_addr = HI_NULL;
static hi_u64 g_vdp_8k_cle_reg_vir_addr = HI_NULL;

static pq_cle_coef_addr g_cle_hal_coef_buf = { 0 };
static cle_split_cfg g_cle_split_cfg = { 0 };
static cle_split_out g_cle_split_out = { 0 };

static const hi_u16 g_cle_color_lut[CLE_COLOR_LUT_ROW_NUM][CLE_COLOR_LUT_COL_NUM] = {
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 8,  8,  16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 }
};

hi_u32 pq_hal_cle_ddr_regread(uintptr_t reg_addr)
{
    hi_u64 addr;

    if ((g_vdp_cle_reg == HI_NULL) || (g_vdp_4k_cle_reg_vir_addr == HI_NULL) ||
        (g_vdp_8k_cle_reg_vir_addr == HI_NULL)) {
        HI_ERR_PQ("Invalid addr!\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    if ((hi_u64)reg_addr < (hi_u64)(&(g_vdp_cle_reg->voctrl.u32))) {
        HI_ERR_PQ("ptr->%#x is invalid!\n", reg_addr);
        return HI_ERR_PQ_INVALID_PARA;
    }

    addr = VDP_REGS_ADDR + ((hi_u64)reg_addr - (hi_u64)(&(g_vdp_cle_reg->voctrl.u32)));

    if ((addr >= CLE_4K_PARA_REG_START) && (addr <= CLE_4K_PARA_REG_END)) {
        /* 4k cle para reg */
        addr = (addr - CLE_4K_PARA_REG_START + g_vdp_4k_cle_reg_vir_addr);
    } else if ((addr >= CLE_8K_PARA_REG_START) && (addr <= CLE_8K_PARA_REG_END)) {
        /* 8k cle para reg */
        addr = (addr - CLE_8K_PARA_REG_START + g_vdp_8k_cle_reg_vir_addr);
    }

    return *((volatile hi_u32 *)addr);
}

hi_s32 pq_hal_cle_ddr_regwrite(uintptr_t reg_addr, hi_u32 value)
{
    hi_u32 *value_addr;
    hi_u64 addr;

    if ((g_vdp_cle_reg == HI_NULL) || (g_vdp_4k_cle_reg_vir_addr == HI_NULL) ||
        (g_vdp_8k_cle_reg_vir_addr == HI_NULL)) {
        HI_ERR_PQ("Invalid addr!\n");
        return HI_FAILURE;
    }

    if ((hi_u64)reg_addr < (hi_u64)(&(g_vdp_cle_reg->voctrl.u32))) {
        HI_ERR_PQ("ptr->%#x is invalid!\n", reg_addr);
        return HI_FAILURE;
    }

    addr = VDP_REGS_ADDR + ((hi_u64)reg_addr - (hi_u64)(&(g_vdp_cle_reg->voctrl.u32)));

    if ((addr >= CLE_4K_PARA_REG_START) && (addr <= CLE_4K_PARA_REG_END)) {
        /* 4k cle para reg */
        addr = (addr - CLE_4K_PARA_REG_START + g_vdp_4k_cle_reg_vir_addr);
        value_addr = (hi_u32 *)addr;
        *value_addr = value;

    } else if ((addr >= CLE_8K_PARA_REG_START) && (addr <= CLE_8K_PARA_REG_END)) {
        /* 8k cle para reg */
        addr = (addr - CLE_8K_PARA_REG_START + g_vdp_8k_cle_reg_vir_addr);
        value_addr = (hi_u32 *)addr;
        *value_addr = value;
    }

    return HI_SUCCESS;
}

hi_void pq_hal_set_4k_cle_reg_vir_addr(uintptr_t vir_addr)
{
    g_vdp_4k_cle_reg_vir_addr = (hi_u64)vir_addr;
    return;
}

hi_u64 pq_hal_get_4k_cle_reg_vir_addr(hi_void)
{
    return g_vdp_4k_cle_reg_vir_addr;
}

hi_void pq_hal_set_8k_cle_reg_vir_addr(uintptr_t vir_addr)
{
    g_vdp_8k_cle_reg_vir_addr = (hi_u64)vir_addr;
    return;
}

hi_u64 pq_hal_get_8k_cle_reg_vir_addr(hi_void)
{
    return g_vdp_8k_cle_reg_vir_addr;
}

hi_s32 pq_hal_cle_reset_addr(hi_void)
{
    g_vdp_cle_reg = HI_NULL;
    memset(&g_cle_hal_coef_buf, 0x0, sizeof(g_cle_hal_coef_buf));
    return HI_SUCCESS;
}

static hi_s32 pq_hal_cle_set_para_addr(const pq_cle_coef_addr *cle_coef_buf_addr)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_cle_reg);

    pq_reg_cle_para_addr_v0_chn20(g_vdp_cle_reg, cle_coef_buf_addr->phy_addr[CLE_COEF_BUF_4K_SR1]);
    pq_reg_cle_para_addr_v0_chn22(g_vdp_cle_reg, cle_coef_buf_addr->phy_addr[CLE_COEF_BUF_8K_SR1]);

    return HI_SUCCESS;
}

static hi_void pq_hal_ip_assign_typical_mode(cle_ip_cfg *ip_cfg, hi_u32 width, hi_u32 height)
{
    ip_cfg->lti_limit_clip_en = 0;
    ip_cfg->shp_clut_rd_en = 0;
    ip_cfg->clut_addr = 0;
    ip_cfg->clut_rdata = 0;
    ip_cfg->face_num = 20; /* 20 :a number */
    ip_cfg->face_confidence_en = 1;

    ip_cfg->cle_ck_gt_en = 0;
    ip_cfg->peak_limit_clip_en = 0;

    ip_cfg->contrastbit = 0;
    ip_cfg->contrastthr = 0;
    ip_cfg->flatbit = 0;
    ip_cfg->flatthrl = 0;
    ip_cfg->a1slp = 0;
    ip_cfg->kneg = 0;
    ip_cfg->a1thrl = 0;
    ip_cfg->gradmonoslp = 0;
    ip_cfg->gradmonomin = 0;
    ip_cfg->gradmonomax = 0;
    ip_cfg->pixmonothr = 0;

    ip_cfg->ltiratio = 0;
    ip_cfg->ltigain = 0;
    ip_cfg->stcv_sclwgtratio = 0;
    ip_cfg->skin_yvalue_mixratio = 0;

    ip_cfg->facepos0_1 = 0;
    ip_cfg->facepos0_0 = 0;
    ip_cfg->facepos0_4 = 1;
    ip_cfg->facepos0_3 = 8;  /* 8 :a number */
    ip_cfg->facepos0_2 = 10; /* 10 :a number */
    ip_cfg->facepos1_1 = 0;
    ip_cfg->facepos1_0 = 10; /* 10 :a number */
    ip_cfg->facepos1_4 = 1;
    ip_cfg->facepos1_3 = 8;  /* 8 :a number */
    ip_cfg->facepos1_2 = 20; /* 20 :a number */
    ip_cfg->facepos2_1 = 8;  /* 8 :a number */
    ip_cfg->facepos2_0 = 0;
    ip_cfg->facepos2_4 = 2;  /* 2 :a number */
    ip_cfg->facepos2_3 = 16; /* 16 :a number */
    ip_cfg->facepos2_2 = 10; /* 10 :a number */
    ip_cfg->facepos3_1 = 8;  /* 8 :a number */
    ip_cfg->facepos3_0 = 10; /* 10 :a number */
    ip_cfg->facepos3_4 = 3;  /* 3 :a number */
    ip_cfg->facepos3_3 = 16; /* 16 :a number */
    ip_cfg->facepos3_2 = 20; /* 20 :a number */
    ip_cfg->facepos4_1 = 0;
    ip_cfg->facepos4_0 = width - 20; /* 20 :a number */
    ip_cfg->facepos4_4 = 4;          /* 4 :a number */
    ip_cfg->facepos4_3 = 8;          /* 8 :a number */
    ip_cfg->facepos4_2 = width - 10; /* 10 :a number */
    ip_cfg->facepos5_1 = 0;
    ip_cfg->facepos5_0 = width - 10; /* 10 :a number */
    ip_cfg->facepos5_4 = 6;          /* 6 :a number */
    ip_cfg->facepos5_3 = 8;          /* 8 :a number */
    ip_cfg->facepos5_2 = width;
    ip_cfg->facepos6_1 = 8;          /* 8 :a number */
    ip_cfg->facepos6_0 = width - 20; /* 20 :a number */
    ip_cfg->facepos6_4 = 6;          /* 6 :a number */
    ip_cfg->facepos6_3 = 16;         /* 16 :a number */
    ip_cfg->facepos6_2 = width - 10; /* 10 :a number */
    ip_cfg->facepos7_1 = 8;          /* 8 :a number */
    ip_cfg->facepos7_0 = width - 10; /* 10 :a number */
    ip_cfg->facepos7_4 = 6;          /* 6 :a number */
    ip_cfg->facepos7_3 = 16;         /* 16 :a number */
    ip_cfg->facepos7_2 = width;
    ip_cfg->facepos8_1 = height / 4;      /* 4 :a number */
    ip_cfg->facepos8_0 = width / 4;       /* 4 :a number */
    ip_cfg->facepos8_4 = 7;               /* 7 :a number */
    ip_cfg->facepos8_3 = height / 2;      /* 2 :a number */
    ip_cfg->facepos8_2 = width / 2 - 10;  /* 2 :a number 10 :a number */
    ip_cfg->facepos9_1 = height / 4;      /* 4 :a number */
    ip_cfg->facepos9_0 = width / 2 - 10;  /* 2 :a number 10 :a number */
    ip_cfg->facepos9_4 = 8;               /* 8 :a number */
    ip_cfg->facepos9_3 = height / 2;      /* 2 :a number */
    ip_cfg->facepos9_2 = width * 3 / 4;   /* 3 :a number 4 :a number */
    ip_cfg->facepos10_1 = height / 2;     /* 2 :a number */
    ip_cfg->facepos10_0 = width / 4;      /* 4 :a number */
    ip_cfg->facepos10_4 = 9;              /* 9 :a number */
    ip_cfg->facepos10_3 = height * 3 / 4; /* 3 :a number 4 :a number */
    ip_cfg->facepos10_2 = width / 2 + 10; /* 2 :a number 10 :a number */
    ip_cfg->facepos11_1 = height / 2;     /* 2 :a number */
    ip_cfg->facepos11_0 = width / 2 + 10; /* 2 :a number 10 :a number */
    ip_cfg->facepos11_4 = 10;             /* 10 :a number */
    ip_cfg->facepos11_3 = height * 3 / 4; /* 3 :a number 4 :a number */
    ip_cfg->facepos11_2 = width * 3 / 4;  /* 3 :a number 4 :a number */
    ip_cfg->facepos12_1 = height - 16;    /* 16 :a number */
    ip_cfg->facepos12_0 = 0;
    ip_cfg->facepos12_4 = 11;          /* 11 :a number */
    ip_cfg->facepos12_3 = height - 8;  /* 8 :a number */
    ip_cfg->facepos12_2 = 10;          /* 10 :a number */
    ip_cfg->facepos13_1 = height - 16; /* 16 :a number */
    ip_cfg->facepos13_0 = 10;          /* 10 :a number */
    ip_cfg->facepos13_4 = 12;          /* 12 :a number */
    ip_cfg->facepos13_3 = height - 8;  /* 8 :a number */
    ip_cfg->facepos13_2 = 20;          /* 20 :a number */
    ip_cfg->facepos14_1 = height - 8;  /* 8 :a number */
    ip_cfg->facepos14_0 = 0;
    ip_cfg->facepos14_4 = 13; /* 13 :a number */
    ip_cfg->facepos14_3 = height;
    ip_cfg->facepos14_2 = 10;         /* 10 :a number */
    ip_cfg->facepos15_1 = height - 8; /* 8 :a number */
    ip_cfg->facepos15_0 = 10;         /* 10 :a number */
    ip_cfg->facepos15_4 = 14;         /* 14 :a number */
    ip_cfg->facepos15_3 = height;
    ip_cfg->facepos15_2 = 20;          /* 20 :a number */
    ip_cfg->facepos16_1 = height - 16; /* 16 :a number */
    ip_cfg->facepos16_0 = width - 20;  /* 20 :a number */
    ip_cfg->facepos16_4 = 15;          /* 15 :a number */
    ip_cfg->facepos16_3 = height - 8;  /* 8 :a number */
    ip_cfg->facepos16_2 = width - 10;  /* 10 :a number */
    ip_cfg->facepos17_1 = height - 16; /* 16 :a number */
    ip_cfg->facepos17_0 = width - 10;  /* 10 :a number */
    ip_cfg->facepos17_4 = 16;          /* 16 :a number */
    ip_cfg->facepos17_3 = height - 8;  /* 8 :a number */
    ip_cfg->facepos17_2 = width;
    ip_cfg->facepos18_1 = height - 8; /* 8 :a number */
    ip_cfg->facepos18_0 = width - 20; /* 20 :a number */
    ip_cfg->facepos18_4 = 16;         /* 16 :a number */
    ip_cfg->facepos18_3 = height;
    ip_cfg->facepos18_2 = width - 10; /* 10 :a number */
    ip_cfg->facepos19_1 = height - 8; /* 8 :a number */
    ip_cfg->facepos19_0 = width - 10; /* 10 :a number */
    ip_cfg->facepos19_4 = 16;         /* 16 :a number */
    ip_cfg->facepos19_3 = height;
    ip_cfg->facepos19_2 = width;

    return;
}

static hi_void pq_hal_assign_split_cfg(cle_split_cfg *split_cfg, const cle_attr_cfg *cfg, const cle_ip_cfg *ip_cfg)
{
    split_cfg->demo_en = cfg->demo_en;
    split_cfg->demo_pos = cfg->demo_pos;
    split_cfg->demo_mode = cfg->demo_mode;
    split_cfg->iw = cfg->img_width;

    split_cfg->facepos0[0] = ip_cfg->facepos0_0;
    split_cfg->facepos2[0] = ip_cfg->facepos0_2;
    split_cfg->facepos0[1] = ip_cfg->facepos1_0;
    split_cfg->facepos2[1] = ip_cfg->facepos1_2;
    split_cfg->facepos0[2] = ip_cfg->facepos2_0; /* 2 :a number */
    split_cfg->facepos2[2] = ip_cfg->facepos2_2; /* 2 :a number */
    split_cfg->facepos0[3] = ip_cfg->facepos3_0; /* 3 :a number */
    split_cfg->facepos2[3] = ip_cfg->facepos3_2; /* 3 :a number */
    split_cfg->facepos0[4] = ip_cfg->facepos4_0; /* 4 :a number */
    split_cfg->facepos2[4] = ip_cfg->facepos4_2; /* 4 :a number */
    split_cfg->facepos0[5] = ip_cfg->facepos5_0; /* 5 :a number */
    split_cfg->facepos2[5] = ip_cfg->facepos5_2; /* 5 :a number */
    split_cfg->facepos0[6] = ip_cfg->facepos6_0; /* 6 :a number */
    split_cfg->facepos2[6] = ip_cfg->facepos6_2; /* 6 :a number */
    split_cfg->facepos0[7] = ip_cfg->facepos7_0; /* 7 :a number */
    split_cfg->facepos2[7] = ip_cfg->facepos7_2; /* 7 :a number */
    split_cfg->facepos0[8] = ip_cfg->facepos8_0; /* 8 :a number */
    split_cfg->facepos2[8] = ip_cfg->facepos8_2; /* 8 :a number */
    split_cfg->facepos0[9] = ip_cfg->facepos9_0; /* 9 :a number */
    split_cfg->facepos2[9] = ip_cfg->facepos9_2; /* 9 :a number */

    split_cfg->facepos0[10] = ip_cfg->facepos10_0; /* 10 :a number */
    split_cfg->facepos2[10] = ip_cfg->facepos10_2; /* 10 :a number */
    split_cfg->facepos0[11] = ip_cfg->facepos11_0; /* 11 :a number */
    split_cfg->facepos2[11] = ip_cfg->facepos11_2; /* 11 :a number */
    split_cfg->facepos0[12] = ip_cfg->facepos12_0; /* 12 :a number */
    split_cfg->facepos2[12] = ip_cfg->facepos12_2; /* 12 :a number */
    split_cfg->facepos0[13] = ip_cfg->facepos13_0; /* 13 :a number */
    split_cfg->facepos2[13] = ip_cfg->facepos13_2; /* 13 :a number */
    split_cfg->facepos0[14] = ip_cfg->facepos14_0; /* 14 :a number */
    split_cfg->facepos2[14] = ip_cfg->facepos14_2; /* 14 :a number */
    split_cfg->facepos0[15] = ip_cfg->facepos15_0; /* 15 :a number */
    split_cfg->facepos2[15] = ip_cfg->facepos15_2; /* 15 :a number */
    split_cfg->facepos0[16] = ip_cfg->facepos16_0; /* 16 :a number */
    split_cfg->facepos2[16] = ip_cfg->facepos16_2; /* 16 :a number */
    split_cfg->facepos0[17] = ip_cfg->facepos17_0; /* 17 :a number */
    split_cfg->facepos2[17] = ip_cfg->facepos17_2; /* 17 :a number */
    split_cfg->facepos0[18] = ip_cfg->facepos18_0; /* 18 :a number */
    split_cfg->facepos2[18] = ip_cfg->facepos18_2; /* 18 :a number */
    split_cfg->facepos0[19] = ip_cfg->facepos19_0; /* 19 :a number */
    split_cfg->facepos2[19] = ip_cfg->facepos19_2; /* 19 :a number */

    return;
}

static hi_void pq_hal_cle_split(pq_cle_id id, cle_split_out *out, const cle_split_cfg *cfg, hi_u32 overlap)
{
    hi_u32 index;
    hi_u32 demo_en;
    hi_u32 demo_pos;
    hi_u32 demo_mode;
    hi_u32 iw;
    hi_u32 left_width;
    hi_u32 face_pos0[20]; /* 20 :a number */
    hi_u32 face_pos2[20]; /* 20 :a number */

    if (id == CLE_ID_V0_4K) {
        return;
    }

    demo_en = cfg->demo_en;
    demo_pos = cfg->demo_pos;
    demo_mode = cfg->demo_mode;
    iw = cfg->iw;
    for (index = 0; index < 20; index++) { /* 20 :a number */
        face_pos0[index] = cfg->facepos0[index];
        face_pos2[index] = cfg->facepos2[index];
    }
    left_width = (cfg->iw / 2 + 11) / 12 * 12; /* 2 :a number 11 :a number 12 :a number 12 :a number */

    if (demo_en == 0) {
        for (index = 0; index < 2; index++) { /* 2 :a number */
            out->demo_en[index] = demo_en;
            out->demo_pos[index] = demo_pos;
            out->demo_mode[index] = demo_mode;
        }
    } else {
        if (demo_mode == 0) {
            if (demo_pos <= left_width) {
                out->demo_en[0] = demo_en;
                out->demo_pos[0] = demo_pos;
                out->demo_mode[0] = demo_mode;
                out->demo_en[1] = 0;
                out->demo_pos[1] = demo_pos;
                out->demo_mode[1] = demo_mode;
            } else {
                out->demo_en[0] = demo_en;
                out->demo_pos[0] = 0;
                out->demo_mode[0] = 1;
                out->demo_en[1] = demo_en;
                out->demo_pos[1] = demo_pos - left_width + overlap;
                out->demo_mode[1] = demo_mode;
            }
        } else {
            if (demo_pos < left_width) {
                out->demo_en[0] = demo_en;
                out->demo_pos[0] = demo_pos;
                out->demo_mode[0] = demo_mode;
                out->demo_en[1] = demo_en;
                out->demo_pos[1] = 0;
                out->demo_mode[1] = demo_mode;
            } else {
                out->demo_en[0] = 0;
                out->demo_pos[0] = demo_pos;
                out->demo_mode[0] = demo_mode;
                out->demo_en[1] = demo_en;
                out->demo_pos[1] = demo_pos - left_width + overlap;
                out->demo_mode[1] = demo_mode;
            }
        }
    }

    /* cle process. */
    for (index = 0; index < 20; index++) { /* 20 :a number */
        if ((face_pos0[index] <= left_width) && (face_pos2[index] <= left_width)) {
            out->facepos0[index][0] = face_pos0[index];
            out->facepos2[index][0] = face_pos2[index];
            out->facepos0[index][1] = 8191; /* 8191 :a number */
            out->facepos2[index][1] = 8191; /* 8191 :a number */
        } else if ((face_pos0[index] >= left_width) && (face_pos2[index] >= left_width)) {
            out->facepos0[index][0] = 8191; /* 8191 :a number */
            out->facepos2[index][0] = 8191; /* 8191 :a number */
            out->facepos0[index][1] = face_pos0[index] - left_width + overlap;
            out->facepos2[index][1] = face_pos2[index] - left_width + overlap;
        } else if ((face_pos0[index] <= left_width) && (face_pos2[index] >= left_width)) {
            out->facepos0[index][0] = face_pos0[index];
            out->facepos2[index][0] = left_width;
            out->facepos0[index][1] = overlap;
            out->facepos2[index][1] = face_pos2[index] - left_width + overlap;
        }
    }

    return;
}

static hi_void pq_hal_set_cle_mode(vdp_regs_type *vdp_reg, pq_cle_id id, cle_mode mode, const cle_attr_cfg *cfg)
{
    static cle_ip_cfg ip_cfg = { 0 };
    hi_u32 offset_addr = 0;
    cle_split_cfg *split_cfg = &g_cle_split_cfg;
    cle_split_out *split_out = &g_cle_split_out;

    switch (mode) {
        case CLE_MODE_TYPICAL:
            pq_hal_ip_assign_typical_mode(&ip_cfg, cfg->img_width, cfg->img_height);
            break;
        case CLE_MODE_RAND_COEF:
        case CLE_MODE_PARA_MAX:
        case CLE_MODE_PARA_MIN:
        case CLE_MODE_SCENE:
            /* NO USE */
            memset(&ip_cfg, 0x0, sizeof(ip_cfg));
            break;
        default:
            memset(&ip_cfg, 0x0, sizeof(ip_cfg));
            break;
    }

    /* split cfg: step 1, assign split cfg */
    pq_hal_assign_split_cfg(split_cfg, cfg, &ip_cfg);
    /* split cfg: step 2, assign split out */
    pq_hal_cle_split(id, split_out, split_cfg, 32); /* 32 :a number */

    if (id == CLE_ID_V0_8K) {
        offset_addr = CLE_REG_8K_OFFSET_ADDR;
    } else {
        offset_addr = CLE_REG_4K_OFFSET_ADDR;
        pq_reg_cle_set_demo_pos(vdp_reg, offset_addr, cfg->demo_pos);
    }

    pq_reg_cle_set_lti_limit_clip_en(vdp_reg, offset_addr, cfg->lti_limit_clip_en);
    pq_reg_cle_set_enltih(vdp_reg, offset_addr, cfg->lti_en);
    pq_reg_cle_set_cle_en(vdp_reg, offset_addr, HI_TRUE);

    pq_reg_cle_set_shp_clut_rd_en(vdp_reg, offset_addr, ip_cfg.shp_clut_rd_en);
    pq_reg_cle_set_clut_addr(vdp_reg, offset_addr, ip_cfg.clut_addr);
    pq_reg_cle_set_clut_rdata(vdp_reg, offset_addr, ip_cfg.clut_rdata);
    pq_reg_cle_set_face_num(vdp_reg, offset_addr, ip_cfg.face_num);
    pq_reg_cle_set_face_confidence_en(vdp_reg, offset_addr, ip_cfg.face_confidence_en);

    pq_reg_cle_set_cle_ck_gt_en(vdp_reg, offset_addr, cfg->cle_ck_gt_en);
    pq_reg_cle_set_peak_limit_clip_en(vdp_reg, offset_addr, cfg->peak_limit_clip_en);

    pq_reg_cle_set_contrastbit(vdp_reg, offset_addr, ip_cfg.contrastbit);
    pq_reg_cle_set_contrastthr(vdp_reg, offset_addr, ip_cfg.contrastthr);
    pq_reg_cle_set_flatbit(vdp_reg, offset_addr, ip_cfg.flatbit);
    pq_reg_cle_set_flatthrl(vdp_reg, offset_addr, ip_cfg.flatthrl);
    pq_reg_cle_set_a1slp(vdp_reg, offset_addr, ip_cfg.a1slp);
    pq_reg_cle_set_kneg(vdp_reg, offset_addr, ip_cfg.kneg);
    pq_reg_cle_set_a1thrl(vdp_reg, offset_addr, ip_cfg.a1thrl);
    pq_reg_cle_set_gradmonoslp(vdp_reg, offset_addr, ip_cfg.gradmonoslp);
    pq_reg_cle_set_gradmonomin(vdp_reg, offset_addr, ip_cfg.gradmonomin);
    pq_reg_cle_set_gradmonomax(vdp_reg, offset_addr, ip_cfg.gradmonomax);
    pq_reg_cle_set_pixmonothr(vdp_reg, offset_addr, ip_cfg.pixmonothr);

    pq_reg_cle_set_ltiratio(vdp_reg, offset_addr, ip_cfg.ltiratio);
    pq_reg_cle_set_ltigain(vdp_reg, offset_addr, ip_cfg.ltigain);

    pq_reg_cle_set_stcv_sclwgtratio(vdp_reg, offset_addr, ip_cfg.stcv_sclwgtratio);
    pq_reg_cle_set_skin_yvalue_mixratio(vdp_reg, offset_addr, ip_cfg.skin_yvalue_mixratio);

    pq_reg_cle_set_facepos01(vdp_reg, offset_addr, ip_cfg.facepos0_1);
    pq_reg_cle_set_facepos00(vdp_reg, offset_addr, ip_cfg.facepos0_0);
    pq_reg_cle_set_facepos04(vdp_reg, offset_addr, ip_cfg.facepos0_4);
    pq_reg_cle_set_facepos03(vdp_reg, offset_addr, ip_cfg.facepos0_3);
    pq_reg_cle_set_facepos02(vdp_reg, offset_addr, ip_cfg.facepos0_2);
    pq_reg_cle_set_facepos11(vdp_reg, offset_addr, ip_cfg.facepos1_1);
    pq_reg_cle_set_facepos10(vdp_reg, offset_addr, ip_cfg.facepos1_0);
    pq_reg_cle_set_facepos14(vdp_reg, offset_addr, ip_cfg.facepos1_4);
    pq_reg_cle_set_facepos13(vdp_reg, offset_addr, ip_cfg.facepos1_3);
    pq_reg_cle_set_facepos12(vdp_reg, offset_addr, ip_cfg.facepos1_2);
    pq_reg_cle_set_facepos21(vdp_reg, offset_addr, ip_cfg.facepos2_1);
    pq_reg_cle_set_facepos20(vdp_reg, offset_addr, ip_cfg.facepos2_0);
    pq_reg_cle_set_facepos24(vdp_reg, offset_addr, ip_cfg.facepos2_4);
    pq_reg_cle_set_facepos23(vdp_reg, offset_addr, ip_cfg.facepos2_3);
    pq_reg_cle_set_facepos22(vdp_reg, offset_addr, ip_cfg.facepos2_2);
    pq_reg_cle_set_facepos31(vdp_reg, offset_addr, ip_cfg.facepos3_1);
    pq_reg_cle_set_facepos30(vdp_reg, offset_addr, ip_cfg.facepos3_0);
    pq_reg_cle_set_facepos34(vdp_reg, offset_addr, ip_cfg.facepos3_4);
    pq_reg_cle_set_facepos33(vdp_reg, offset_addr, ip_cfg.facepos3_3);
    pq_reg_cle_set_facepos32(vdp_reg, offset_addr, ip_cfg.facepos3_2);
    pq_reg_cle_set_facepos41(vdp_reg, offset_addr, ip_cfg.facepos4_1);
    pq_reg_cle_set_facepos40(vdp_reg, offset_addr, ip_cfg.facepos4_0);
    pq_reg_cle_set_facepos44(vdp_reg, offset_addr, ip_cfg.facepos4_4);
    pq_reg_cle_set_facepos43(vdp_reg, offset_addr, ip_cfg.facepos4_3);
    pq_reg_cle_set_facepos42(vdp_reg, offset_addr, ip_cfg.facepos4_2);
    pq_reg_cle_set_facepos51(vdp_reg, offset_addr, ip_cfg.facepos5_1);
    pq_reg_cle_set_facepos50(vdp_reg, offset_addr, ip_cfg.facepos5_0);
    pq_reg_cle_set_facepos54(vdp_reg, offset_addr, ip_cfg.facepos5_4);
    pq_reg_cle_set_facepos53(vdp_reg, offset_addr, ip_cfg.facepos5_3);
    pq_reg_cle_set_facepos52(vdp_reg, offset_addr, ip_cfg.facepos5_2);
    pq_reg_cle_set_facepos61(vdp_reg, offset_addr, ip_cfg.facepos6_1);
    pq_reg_cle_set_facepos60(vdp_reg, offset_addr, ip_cfg.facepos6_0);
    pq_reg_cle_set_facepos64(vdp_reg, offset_addr, ip_cfg.facepos6_4);
    pq_reg_cle_set_facepos63(vdp_reg, offset_addr, ip_cfg.facepos6_3);
    pq_reg_cle_set_facepos62(vdp_reg, offset_addr, ip_cfg.facepos6_2);
    pq_reg_cle_set_facepos71(vdp_reg, offset_addr, ip_cfg.facepos7_1);
    pq_reg_cle_set_facepos70(vdp_reg, offset_addr, ip_cfg.facepos7_0);
    pq_reg_cle_set_facepos74(vdp_reg, offset_addr, ip_cfg.facepos7_4);
    pq_reg_cle_set_facepos73(vdp_reg, offset_addr, ip_cfg.facepos7_3);
    pq_reg_cle_set_facepos72(vdp_reg, offset_addr, ip_cfg.facepos7_2);
    pq_reg_cle_set_facepos81(vdp_reg, offset_addr, ip_cfg.facepos8_1);
    pq_reg_cle_set_facepos80(vdp_reg, offset_addr, ip_cfg.facepos8_0);
    pq_reg_cle_set_facepos84(vdp_reg, offset_addr, ip_cfg.facepos8_4);
    pq_reg_cle_set_facepos83(vdp_reg, offset_addr, ip_cfg.facepos8_3);
    pq_reg_cle_set_facepos82(vdp_reg, offset_addr, ip_cfg.facepos8_2);
    pq_reg_cle_set_facepos91(vdp_reg, offset_addr, ip_cfg.facepos9_1);
    pq_reg_cle_set_facepos90(vdp_reg, offset_addr, ip_cfg.facepos9_0);
    pq_reg_cle_set_facepos94(vdp_reg, offset_addr, ip_cfg.facepos9_4);
    pq_reg_cle_set_facepos93(vdp_reg, offset_addr, ip_cfg.facepos9_3);
    pq_reg_cle_set_facepos92(vdp_reg, offset_addr, ip_cfg.facepos9_2);
    pq_reg_cle_set_facepos101(vdp_reg, offset_addr, ip_cfg.facepos10_1);
    pq_reg_cle_set_facepos100(vdp_reg, offset_addr, ip_cfg.facepos10_0);
    pq_reg_cle_set_facepos104(vdp_reg, offset_addr, ip_cfg.facepos10_4);
    pq_reg_cle_set_facepos103(vdp_reg, offset_addr, ip_cfg.facepos10_3);
    pq_reg_cle_set_facepos102(vdp_reg, offset_addr, ip_cfg.facepos10_2);
    pq_reg_cle_set_facepos111(vdp_reg, offset_addr, ip_cfg.facepos11_1);
    pq_reg_cle_set_facepos110(vdp_reg, offset_addr, ip_cfg.facepos11_0);
    pq_reg_cle_set_facepos114(vdp_reg, offset_addr, ip_cfg.facepos11_4);
    pq_reg_cle_set_facepos113(vdp_reg, offset_addr, ip_cfg.facepos11_3);
    pq_reg_cle_set_facepos112(vdp_reg, offset_addr, ip_cfg.facepos11_2);
    pq_reg_cle_set_facepos121(vdp_reg, offset_addr, ip_cfg.facepos12_1);
    pq_reg_cle_set_facepos120(vdp_reg, offset_addr, ip_cfg.facepos12_0);
    pq_reg_cle_set_facepos124(vdp_reg, offset_addr, ip_cfg.facepos12_4);
    pq_reg_cle_set_facepos123(vdp_reg, offset_addr, ip_cfg.facepos12_3);
    pq_reg_cle_set_facepos122(vdp_reg, offset_addr, ip_cfg.facepos12_2);
    pq_reg_cle_set_facepos131(vdp_reg, offset_addr, ip_cfg.facepos13_1);
    pq_reg_cle_set_facepos130(vdp_reg, offset_addr, ip_cfg.facepos13_0);
    pq_reg_cle_set_facepos134(vdp_reg, offset_addr, ip_cfg.facepos13_4);
    pq_reg_cle_set_facepos133(vdp_reg, offset_addr, ip_cfg.facepos13_3);
    pq_reg_cle_set_facepos132(vdp_reg, offset_addr, ip_cfg.facepos13_2);
    pq_reg_cle_set_facepos141(vdp_reg, offset_addr, ip_cfg.facepos14_1);
    pq_reg_cle_set_facepos140(vdp_reg, offset_addr, ip_cfg.facepos14_0);
    pq_reg_cle_set_facepos144(vdp_reg, offset_addr, ip_cfg.facepos14_4);
    pq_reg_cle_set_facepos143(vdp_reg, offset_addr, ip_cfg.facepos14_3);
    pq_reg_cle_set_facepos142(vdp_reg, offset_addr, ip_cfg.facepos14_2);
    pq_reg_cle_set_facepos151(vdp_reg, offset_addr, ip_cfg.facepos15_1);
    pq_reg_cle_set_facepos150(vdp_reg, offset_addr, ip_cfg.facepos15_0);
    pq_reg_cle_set_facepos154(vdp_reg, offset_addr, ip_cfg.facepos15_4);
    pq_reg_cle_set_facepos153(vdp_reg, offset_addr, ip_cfg.facepos15_3);
    pq_reg_cle_set_facepos152(vdp_reg, offset_addr, ip_cfg.facepos15_2);
    pq_reg_cle_set_facepos161(vdp_reg, offset_addr, ip_cfg.facepos16_1);
    pq_reg_cle_set_facepos160(vdp_reg, offset_addr, ip_cfg.facepos16_0);
    pq_reg_cle_set_facepos164(vdp_reg, offset_addr, ip_cfg.facepos16_4);
    pq_reg_cle_set_facepos163(vdp_reg, offset_addr, ip_cfg.facepos16_3);
    pq_reg_cle_set_facepos162(vdp_reg, offset_addr, ip_cfg.facepos16_2);
    pq_reg_cle_set_facepos171(vdp_reg, offset_addr, ip_cfg.facepos17_1);
    pq_reg_cle_set_facepos170(vdp_reg, offset_addr, ip_cfg.facepos17_0);
    pq_reg_cle_set_facepos174(vdp_reg, offset_addr, ip_cfg.facepos17_4);
    pq_reg_cle_set_facepos173(vdp_reg, offset_addr, ip_cfg.facepos17_3);
    pq_reg_cle_set_facepos172(vdp_reg, offset_addr, ip_cfg.facepos17_2);
    pq_reg_cle_set_facepos181(vdp_reg, offset_addr, ip_cfg.facepos18_1);
    pq_reg_cle_set_facepos180(vdp_reg, offset_addr, ip_cfg.facepos18_0);
    pq_reg_cle_set_facepos184(vdp_reg, offset_addr, ip_cfg.facepos18_4);
    pq_reg_cle_set_facepos183(vdp_reg, offset_addr, ip_cfg.facepos18_3);
    pq_reg_cle_set_facepos182(vdp_reg, offset_addr, ip_cfg.facepos18_2);
    pq_reg_cle_set_facepos191(vdp_reg, offset_addr, ip_cfg.facepos19_1);
    pq_reg_cle_set_facepos190(vdp_reg, offset_addr, ip_cfg.facepos19_0);
    pq_reg_cle_set_facepos194(vdp_reg, offset_addr, ip_cfg.facepos19_4);
    pq_reg_cle_set_facepos193(vdp_reg, offset_addr, ip_cfg.facepos19_3);
    pq_reg_cle_set_facepos192(vdp_reg, offset_addr, ip_cfg.facepos19_2);

    if (id == CLE_ID_V0_8K) {
        /* left chn */
        pq_reg_cle_set_lti_limit_clip_en(vdp_reg, offset_addr + 0x500 / 4, cfg->lti_limit_clip_en); /* 4 :a number */
        pq_reg_cle_set_enltih(vdp_reg, offset_addr + 0x500 / 4, cfg->lti_en);                       /* 4 :a number */
        pq_reg_cle_set_cle_en(vdp_reg, offset_addr + CLE_REG_8K_LEFT_OFFSET, HI_TRUE);
        pq_reg_cle_set_facepos01(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos0_1);   /* 4 :a number */
        pq_reg_cle_set_facepos04(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos0_4);   /* 4 :a number */
        pq_reg_cle_set_facepos03(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos0_3);   /* 4 :a number */
        pq_reg_cle_set_facepos11(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos1_1);   /* 4 :a number */
        pq_reg_cle_set_facepos14(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos1_4);   /* 4 :a number */
        pq_reg_cle_set_facepos13(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos1_3);   /* 4 :a number */
        pq_reg_cle_set_facepos21(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos2_1);   /* 4 :a number */
        pq_reg_cle_set_facepos24(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos2_4);   /* 4 :a number */
        pq_reg_cle_set_facepos23(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos2_3);   /* 4 :a number */
        pq_reg_cle_set_facepos31(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos3_1);   /* 4 :a number */
        pq_reg_cle_set_facepos34(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos3_4);   /* 4 :a number */
        pq_reg_cle_set_facepos33(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos3_3);   /* 4 :a number */
        pq_reg_cle_set_facepos41(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos4_1);   /* 4 :a number */
        pq_reg_cle_set_facepos44(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos4_4);   /* 4 :a number */
        pq_reg_cle_set_facepos43(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos4_3);   /* 4 :a number */
        pq_reg_cle_set_facepos51(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos5_1);   /* 4 :a number */
        pq_reg_cle_set_facepos54(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos5_4);   /* 4 :a number */
        pq_reg_cle_set_facepos53(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos5_3);   /* 4 :a number */
        pq_reg_cle_set_facepos61(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos6_1);   /* 4 :a number */
        pq_reg_cle_set_facepos64(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos6_4);   /* 4 :a number */
        pq_reg_cle_set_facepos63(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos6_3);   /* 4 :a number */
        pq_reg_cle_set_facepos71(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos7_1);   /* 4 :a number */
        pq_reg_cle_set_facepos74(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos7_4);   /* 4 :a number */
        pq_reg_cle_set_facepos73(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos7_3);   /* 4 :a number */
        pq_reg_cle_set_facepos81(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos8_1);   /* 4 :a number */
        pq_reg_cle_set_facepos84(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos8_4);   /* 4 :a number */
        pq_reg_cle_set_facepos83(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos8_3);   /* 4 :a number */
        pq_reg_cle_set_facepos91(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos9_1);   /* 4 :a number */
        pq_reg_cle_set_facepos94(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos9_4);   /* 4 :a number */
        pq_reg_cle_set_facepos93(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos9_3);   /* 4 :a number */
        pq_reg_cle_set_facepos101(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos10_1); /* 4 :a number */
        pq_reg_cle_set_facepos104(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos10_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos103(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos10_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos111(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos11_1); /* 4 :a number */
        pq_reg_cle_set_facepos114(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos11_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos113(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos11_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos121(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos12_1); /* 4 :a number */
        pq_reg_cle_set_facepos124(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos12_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos123(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos12_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos131(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos13_1); /* 4 :a number */
        pq_reg_cle_set_facepos134(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos13_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos133(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos13_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos141(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos14_1); /* 4 :a number */
        pq_reg_cle_set_facepos144(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos14_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos143(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos14_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos151(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos15_1); /* 4 :a number */
        pq_reg_cle_set_facepos154(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos15_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos153(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos15_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos161(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos16_1); /* 4 :a number */
        pq_reg_cle_set_facepos164(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos16_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos163(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos16_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos171(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos17_1); /* 4 :a number */
        pq_reg_cle_set_facepos174(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos17_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos173(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos17_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos181(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos18_1); /* 4 :a number */
        pq_reg_cle_set_facepos184(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos18_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos183(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos18_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos191(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos19_1); /* 4 :a number */
        pq_reg_cle_set_facepos194(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos19_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos193(vdp_reg, offset_addr + 0x500 / 4, ip_cfg.facepos19_3); /* 4 :a number 3 :a number */

        pq_reg_cle_set_demo_en(vdp_reg, offset_addr + 0x500 / 4, split_out->demo_en[0]);     /* 4 :a number */
        pq_reg_cle_set_demo_mode(vdp_reg, offset_addr + 0x500 / 4, split_out->demo_mode[0]); /* 4 :a number */
        pq_reg_cle_set_demo_pos(vdp_reg, offset_addr + 0x500 / 4, split_out->demo_pos[0]);   /* 4 :a number */

        pq_reg_cle_set_facepos00(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[0][0]);   /* 4 :a number */
        pq_reg_cle_set_facepos02(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[0][0]);   /* 4 :a number */
        pq_reg_cle_set_facepos10(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[1][0]);   /* 4 :a number */
        pq_reg_cle_set_facepos12(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[1][0]);   /* 4 :a number */
        pq_reg_cle_set_facepos20(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[2][0]);   /* 4 :a number 2 :a number */
        pq_reg_cle_set_facepos22(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[2][0]);   /* 4 :a number 2 :a number */
        pq_reg_cle_set_facepos30(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[3][0]);   /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos32(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[3][0]);   /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos40(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[4][0]);   /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos42(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[4][0]);   /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos50(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[5][0]);   /* 4 :a number 5 :a number */
        pq_reg_cle_set_facepos52(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[5][0]);   /* 4 :a number 5 :a number */
        pq_reg_cle_set_facepos60(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[6][0]);   /* 4 :a number 6 :a number */
        pq_reg_cle_set_facepos62(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[6][0]);   /* 4 :a number 6 :a number */
        pq_reg_cle_set_facepos70(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[7][0]);   /* 4 :a number 7 :a number */
        pq_reg_cle_set_facepos72(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[7][0]);   /* 4 :a number 7 :a number */
        pq_reg_cle_set_facepos80(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[8][0]);   /* 4 :a number 8 :a number */
        pq_reg_cle_set_facepos82(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[8][0]);   /* 4 :a number 8 :a number */
        pq_reg_cle_set_facepos90(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[9][0]);   /* 4 :a number 9 :a number */
        pq_reg_cle_set_facepos92(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[9][0]);   /* 4 :a number 9 :a number */
        pq_reg_cle_set_facepos100(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[10][0]); /* 4 :a number 10 :a number */
        pq_reg_cle_set_facepos102(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[10][0]); /* 4 :a number 2 :a number 10 :a number */
        pq_reg_cle_set_facepos110(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[11][0]); /* 4 :a number 11 :a number */
        pq_reg_cle_set_facepos112(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[11][0]); /* 4 :a number 2 :a number 11 :a number */
        pq_reg_cle_set_facepos120(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[12][0]); /* 4 :a number 12 :a number */
        pq_reg_cle_set_facepos122(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[12][0]); /* 4 :a number 2 :a number 12 :a number */
        pq_reg_cle_set_facepos130(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[13][0]); /* 4 :a number 13 :a number */
        pq_reg_cle_set_facepos132(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[13][0]); /* 4 :a number 2 :a number 13 :a number */
        pq_reg_cle_set_facepos140(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[14][0]); /* 4 :a number 14 :a number */
        pq_reg_cle_set_facepos142(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[14][0]); /* 4 :a number 2 :a number 14 :a number */
        pq_reg_cle_set_facepos150(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[15][0]); /* 4 :a number 15 :a number */
        pq_reg_cle_set_facepos152(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[15][0]); /* 4 :a number 2 :a number 15 :a number */
        pq_reg_cle_set_facepos160(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[16][0]); /* 4 :a number 16 :a number */
        pq_reg_cle_set_facepos162(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[16][0]); /* 4 :a number 2 :a number 16 :a number */
        pq_reg_cle_set_facepos170(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[17][0]); /* 4 :a number 17 :a number */
        pq_reg_cle_set_facepos172(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[17][0]); /* 4 :a number 2 :a number 17 :a number */
        pq_reg_cle_set_facepos180(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[18][0]); /* 4 :a number 18 :a number */
        pq_reg_cle_set_facepos182(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[18][0]); /* 4 :a number 2 :a number 18 :a number */
        pq_reg_cle_set_facepos190(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos0[19][0]); /* 4 :a number 19 :a number */
        pq_reg_cle_set_facepos192(vdp_reg, offset_addr + 0x500 / 4, split_out->facepos2[19][0]); /* 4 :a number 2 :a number 19 :a number */

        /* right chn */
        pq_reg_cle_set_lti_limit_clip_en(vdp_reg, offset_addr + 0xA00 / 4, cfg->lti_limit_clip_en); /* 4 :a number */
        pq_reg_cle_set_enltih(vdp_reg, offset_addr + 0xA00 / 4, cfg->lti_en);                       /* 4 :a number */
        pq_reg_cle_set_cle_en(vdp_reg, offset_addr + CLE_REG_8K_RIGHT_OFFSET, HI_TRUE);
        pq_reg_cle_set_facepos01(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos0_1);   /* 4 :a number */
        pq_reg_cle_set_facepos04(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos0_4);   /* 4 :a number */
        pq_reg_cle_set_facepos03(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos0_3);   /* 4 :a number */
        pq_reg_cle_set_facepos11(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos1_1);   /* 4 :a number */
        pq_reg_cle_set_facepos14(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos1_4);   /* 4 :a number */
        pq_reg_cle_set_facepos13(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos1_3);   /* 4 :a number */
        pq_reg_cle_set_facepos21(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos2_1);   /* 4 :a number */
        pq_reg_cle_set_facepos24(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos2_4);   /* 4 :a number */
        pq_reg_cle_set_facepos23(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos2_3);   /* 4 :a number */
        pq_reg_cle_set_facepos31(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos3_1);   /* 4 :a number */
        pq_reg_cle_set_facepos34(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos3_4);   /* 4 :a number */
        pq_reg_cle_set_facepos33(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos3_3);   /* 4 :a number */
        pq_reg_cle_set_facepos41(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos4_1);   /* 4 :a number */
        pq_reg_cle_set_facepos44(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos4_4);   /* 4 :a number */
        pq_reg_cle_set_facepos43(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos4_3);   /* 4 :a number */
        pq_reg_cle_set_facepos51(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos5_1);   /* 4 :a number */
        pq_reg_cle_set_facepos54(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos5_4);   /* 4 :a number */
        pq_reg_cle_set_facepos53(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos5_3);   /* 4 :a number */
        pq_reg_cle_set_facepos61(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos6_1);   /* 4 :a number */
        pq_reg_cle_set_facepos64(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos6_4);   /* 4 :a number */
        pq_reg_cle_set_facepos63(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos6_3);   /* 4 :a number */
        pq_reg_cle_set_facepos71(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos7_1);   /* 4 :a number */
        pq_reg_cle_set_facepos74(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos7_4);   /* 4 :a number */
        pq_reg_cle_set_facepos73(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos7_3);   /* 4 :a number */
        pq_reg_cle_set_facepos81(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos8_1);   /* 4 :a number */
        pq_reg_cle_set_facepos84(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos8_4);   /* 4 :a number */
        pq_reg_cle_set_facepos83(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos8_3);   /* 4 :a number */
        pq_reg_cle_set_facepos91(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos9_1);   /* 4 :a number */
        pq_reg_cle_set_facepos94(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos9_4);   /* 4 :a number */
        pq_reg_cle_set_facepos93(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos9_3);   /* 4 :a number */
        pq_reg_cle_set_facepos101(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos10_1); /* 4 :a number */
        pq_reg_cle_set_facepos104(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos10_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos103(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos10_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos111(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos11_1); /* 4 :a number */
        pq_reg_cle_set_facepos114(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos11_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos113(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos11_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos121(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos12_1); /* 4 :a number */
        pq_reg_cle_set_facepos124(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos12_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos123(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos12_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos131(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos13_1); /* 4 :a number */
        pq_reg_cle_set_facepos134(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos13_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos133(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos13_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos141(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos14_1); /* 4 :a number */
        pq_reg_cle_set_facepos144(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos14_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos143(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos14_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos151(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos15_1); /* 4 :a number */
        pq_reg_cle_set_facepos154(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos15_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos153(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos15_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos161(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos16_1); /* 4 :a number */
        pq_reg_cle_set_facepos164(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos16_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos163(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos16_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos171(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos17_1); /* 4 :a number */
        pq_reg_cle_set_facepos174(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos17_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos173(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos17_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos181(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos18_1); /* 4 :a number */
        pq_reg_cle_set_facepos184(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos18_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos183(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos18_3); /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos191(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos19_1); /* 4 :a number */
        pq_reg_cle_set_facepos194(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos19_4); /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos193(vdp_reg, offset_addr + 0xA00 / 4, ip_cfg.facepos19_3); /* 4 :a number 3 :a number */

        pq_reg_cle_set_demo_en(vdp_reg, offset_addr + 0xA00 / 4, split_out->demo_en[1]);     /* 4 :a number */
        pq_reg_cle_set_demo_mode(vdp_reg, offset_addr + 0xA00 / 4, split_out->demo_mode[1]); /* 4 :a number */
        pq_reg_cle_set_demo_pos(vdp_reg, offset_addr + 0xA00 / 4, split_out->demo_pos[1]);   /* 4 :a number */

        pq_reg_cle_set_facepos00(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[0][1]);   /* 4 :a number */
        pq_reg_cle_set_facepos02(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[0][1]);   /* 4 :a number */
        pq_reg_cle_set_facepos10(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[1][1]);   /* 4 :a number */
        pq_reg_cle_set_facepos12(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[1][1]);   /* 4 :a number */
        pq_reg_cle_set_facepos20(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[2][1]);   /* 4 :a number 2 :a number */
        pq_reg_cle_set_facepos22(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[2][1]);   /* 4 :a number 2 :a number */
        pq_reg_cle_set_facepos30(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[3][1]);   /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos32(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[3][1]);   /* 4 :a number 3 :a number */
        pq_reg_cle_set_facepos40(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[4][1]);   /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos42(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[4][1]);   /* 4 :a number 4 :a number */
        pq_reg_cle_set_facepos50(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[5][1]);   /* 4 :a number 5 :a number */
        pq_reg_cle_set_facepos52(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[5][1]);   /* 4 :a number 5 :a number */
        pq_reg_cle_set_facepos60(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[6][1]);   /* 4 :a number 6 :a number */
        pq_reg_cle_set_facepos62(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[6][1]);   /* 4 :a number 6 :a number */
        pq_reg_cle_set_facepos70(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[7][1]);   /* 4 :a number 7 :a number */
        pq_reg_cle_set_facepos72(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[7][1]);   /* 4 :a number 7 :a number */
        pq_reg_cle_set_facepos80(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[8][1]);   /* 4 :a number 8 :a number */
        pq_reg_cle_set_facepos82(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[8][1]);   /* 4 :a number 8 :a number */
        pq_reg_cle_set_facepos90(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[9][1]);   /* 4 :a number 9 :a number */
        pq_reg_cle_set_facepos92(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[9][1]);   /* 4 :a number 9 :a number */
        pq_reg_cle_set_facepos100(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[10][1]); /* 4 :a number 10 :a number */
        pq_reg_cle_set_facepos102(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[10][1]); /* 4 :a number 2 :a number 10 :a number */
        pq_reg_cle_set_facepos110(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[11][1]); /* 4 :a number 11 :a number */
        pq_reg_cle_set_facepos112(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[11][1]); /* 4 :a number 2 :a number 11 :a number */
        pq_reg_cle_set_facepos120(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[12][1]); /* 4 :a number 12 :a number */
        pq_reg_cle_set_facepos122(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[12][1]); /* 4 :a number 2 :a number 12 :a number */
        pq_reg_cle_set_facepos130(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[13][1]); /* 4 :a number 13 :a number */
        pq_reg_cle_set_facepos132(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[13][1]); /* 4 :a number 2 :a number 13 :a number */
        pq_reg_cle_set_facepos140(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[14][1]); /* 4 :a number 14 :a number */
        pq_reg_cle_set_facepos142(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[14][1]); /* 4 :a number 2 :a number 14 :a number */
        pq_reg_cle_set_facepos150(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[15][1]); /* 4 :a number 15 :a number */
        pq_reg_cle_set_facepos152(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[15][1]); /* 4 :a number 2 :a number 15 :a number */
        pq_reg_cle_set_facepos160(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[16][1]); /* 4 :a number 16 :a number */
        pq_reg_cle_set_facepos162(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[16][1]); /* 4 :a number 2 :a number 16 :a number */
        pq_reg_cle_set_facepos170(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[17][1]); /* 4 :a number 17 :a number */
        pq_reg_cle_set_facepos172(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[17][1]); /* 4 :a number 2 :a number 17 :a number */
        pq_reg_cle_set_facepos180(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[18][1]); /* 4 :a number 18 :a number */
        pq_reg_cle_set_facepos182(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[18][1]); /* 4 :a number 2 :a number 18 :a number */
        pq_reg_cle_set_facepos190(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos0[19][1]); /* 4 :a number 19 :a number */
        pq_reg_cle_set_facepos192(vdp_reg, offset_addr + 0xA00 / 4, split_out->facepos2[19][1]); /* 4 :a number 2 :a number 19 :a number */
    }

    return;
}

static hi_void pq_hal_cle_send_coef(pq_cle_id id, hi_u8 *color_lut)
{
    pq_coef_send_cfg coef_send = { 0 };
    hi_u8 *addr;
    hi_void *coef_array[1] = { color_lut };
    hi_void *coef_array_para[1];
    hi_u32 lut_lenght = 256;
    hi_u32 coef_bit_len = 8;

    if (id == CLE_ID_V0_4K) {
        addr = g_cle_hal_coef_buf.vir_addr[CLE_COEF_BUF_4K_SR1];
        coef_array_para[0] = (hi_u32 *)g_cle_hal_coef_buf.vir_addr[CLE_COEF_BUF_4K_REG];
    } else {
        addr = g_cle_hal_coef_buf.vir_addr[CLE_COEF_BUF_8K_SR1];
        coef_array_para[0] = (hi_u32 *)g_cle_hal_coef_buf.vir_addr[CLE_COEF_BUF_8K_REG];
    }

    coef_send.coef_addr = addr;
    coef_send.sti_type = PQ_STI_FILE_COEF_CLE;
    coef_send.fp_coef = HI_NULL;
    coef_send.lut_num = 1;
    coef_send.burst_num = 1;
    coef_send.cycle_num = 16; /* 16 :a number */
    coef_send.p_coef_array = coef_array;
    coef_send.lut_length = &lut_lenght;
    coef_send.coef_bit_length = &coef_bit_len;
    coef_send.data_type = DRV_COEF_DATA_TYPE_U8;
    addr = pq_send_coef2ddr(&coef_send);

    lut_lenght = 109;  /* 109 :a number */
    coef_bit_len = 32; /* 32 :a number */
    coef_send.coef_addr = addr;
    coef_send.cycle_num = 4; /* 4 :a number */
    coef_send.p_coef_array = coef_array_para;
    coef_send.lut_length = &lut_lenght;
    coef_send.coef_bit_length = &coef_bit_len;
    coef_send.data_type = DRV_COEF_DATA_TYPE_U32;
    pq_send_coef2ddr(&coef_send);

    return;
}

static hi_void pq_hal_set_cle_coef(pq_cle_id id, pq_rm_coef_mode mode)
{
    hi_u32 row;
    hi_u32 col;
    hi_u8 color_lut_tmp[CLE_COLOR_LUT_ROW_NUM * CLE_COLOR_LUT_COL_NUM];
    hi_u8 color_lut[CLE_COLOR_LUT_ROW_NUM * CLE_COLOR_LUT_COL_NUM];
    pq_coef_gen_cfg coef_gen = { 0 };

    /* step1, gen coef. */
    for (row = 0; row < CLE_COLOR_LUT_ROW_NUM; row++) {
        for (col = 0; col < CLE_COLOR_LUT_COL_NUM; col++) {
            color_lut_tmp[row * CLE_COLOR_LUT_COL_NUM + col] = (hi_u8)g_cle_color_lut[row][col];
        }
    }

    coef_gen.p_coef = color_lut_tmp;
    coef_gen.p_coef_new = color_lut;
    coef_gen.length = CLE_COLOR_LUT_ROW_NUM * CLE_COLOR_LUT_COL_NUM;
    coef_gen.coef_data_type = DRV_COEF_DATA_TYPE_U8;
    coef_gen.coef_data_mode = mode;
    coef_gen.coef_max = ((1 << 5) - 1); /* 5 :a number */
    coef_gen.coef_min = 0;
    pq_drv_gen_coef(&coef_gen);

    /* step2, send coef. */
    pq_hal_cle_send_coef(id, color_lut);

    return;
}

hi_s32 pq_hal_cle_distribute_addr(hi_u8 *vir_addr, dma_addr_t phy_addr)
{
    hi_s32 ret;

    if (vir_addr == HI_NULL || phy_addr == HI_NULL) {
        return HI_FAILURE;
    }

    g_vdp_cle_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_cle_reg);

    g_cle_hal_coef_buf.vir_addr[CLE_COEF_BUF_4K_SR1] = vir_addr;
    g_cle_hal_coef_buf.phy_addr[CLE_COEF_BUF_4K_SR1] = phy_addr;

    g_cle_hal_coef_buf.vir_addr[CLE_COEF_BUF_8K_SR1] = g_cle_hal_coef_buf.vir_addr[CLE_COEF_BUF_4K_SR1] + \
                                                       COEF_SIZE_4K_SR1;
    g_cle_hal_coef_buf.phy_addr[CLE_COEF_BUF_8K_SR1] = g_cle_hal_coef_buf.phy_addr[CLE_COEF_BUF_4K_SR1] + \
                                                       COEF_SIZE_4K_SR1;

    /* assign ddr buf for loading para to reg. */
    g_cle_hal_coef_buf.vir_addr[CLE_COEF_BUF_4K_REG] = g_cle_hal_coef_buf.vir_addr[CLE_COEF_BUF_8K_SR1] + \
                                                       COEF_SIZE_8K_SR1;
    g_cle_hal_coef_buf.phy_addr[CLE_COEF_BUF_4K_REG] = g_cle_hal_coef_buf.phy_addr[CLE_COEF_BUF_8K_SR1] + \
                                                       COEF_SIZE_8K_SR1;

    g_cle_hal_coef_buf.vir_addr[CLE_COEF_BUF_8K_REG] = g_cle_hal_coef_buf.vir_addr[CLE_COEF_BUF_4K_REG] + \
                                                       COEF_SIZE_4K_CLE_REG;
    g_cle_hal_coef_buf.phy_addr[CLE_COEF_BUF_8K_REG] = g_cle_hal_coef_buf.phy_addr[CLE_COEF_BUF_4K_REG] + \
                                                       COEF_SIZE_4K_CLE_REG;

    pq_hal_set_4k_cle_reg_vir_addr((uintptr_t)g_cle_hal_coef_buf.vir_addr[CLE_COEF_BUF_4K_REG]);
    pq_hal_set_8k_cle_reg_vir_addr((uintptr_t)g_cle_hal_coef_buf.vir_addr[CLE_COEF_BUF_8K_REG]);

    ret = pq_hal_cle_set_para_addr(&g_cle_hal_coef_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_hal_cle_set_para_addr fail!\n");
        pq_hal_cle_reset_addr();
        return ret;
    }

    return HI_SUCCESS;
}

static hi_void pq_hal_disable_cle(vdp_regs_type *vdp_reg, pq_cle_id id)
{
    hi_u64 offset_addr = 0;

    if (id == CLE_ID_V0_4K) {
        offset_addr = CLE_REG_4K_OFFSET_ADDR;
    } else {
        offset_addr = CLE_REG_8K_OFFSET_ADDR;
        pq_reg_cle_set_cle_en(vdp_reg, offset_addr + CLE_REG_8K_LEFT_OFFSET, HI_FALSE);
        pq_reg_cle_set_cle_en(vdp_reg, offset_addr + CLE_REG_8K_RIGHT_OFFSET, HI_FALSE);
    }

    pq_reg_cle_set_cle_en(vdp_reg, offset_addr, HI_FALSE);

    return;
}

static hi_s32 pq_hal_set_cle(const alg_cle *cle_cfg, const cle_attr_cfg *cfg)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_cle_reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(cfg);

    if (cle_cfg->cle_reso[CLE_ID_V0_4K].cle_enable) {
        /* set 4k cle mode. */
        pq_hal_set_cle_mode(g_vdp_cle_reg, CLE_ID_V0_4K, CLE_MODE_TYPICAL, cfg);
        /* set 4k cle coef. */
        pq_hal_set_cle_coef(CLE_ID_V0_4K, VDP_RM_COEF_MODE_TYP);
        /* regup 4k cle para. */
        pq_reg_cle_setparaupv0chn(g_vdp_cle_reg, CLE_COEF_BUF_4K_SR1);
    } else {
        pq_hal_disable_cle(g_vdp_cle_reg, CLE_ID_V0_4K);
    }

    if (cle_cfg->cle_reso[CLE_ID_V0_8K].cle_enable) {
        /* set 8k cle mode */
        pq_hal_set_cle_mode(g_vdp_cle_reg, CLE_ID_V0_8K, CLE_MODE_TYPICAL, cfg);
        pq_reg_cle_set_8k_cle_para_src(g_vdp_cle_reg, HI_FALSE);
        /* set 8k cle coef */
        pq_hal_set_cle_coef(CLE_ID_V0_8K, VDP_RM_COEF_MODE_TYP);
        /* regup 8k cle para. */
        pq_reg_cle_setparaupv0chn(g_vdp_cle_reg, CLE_COEF_BUF_8K_SR1);
    } else {
        pq_hal_disable_cle(g_vdp_cle_reg, CLE_ID_V0_8K);
    }

    return HI_SUCCESS;
}

static hi_void pq_hal_assign_cle_attr(const alg_cle *cle_cfg, cle_attr_cfg *cle_attr)
{
    if (cle_cfg->cle_reso[CLE_ID_V0_8K].cle_enable) {
        cle_attr->img_width = cle_cfg->cle_reso[CLE_ID_V0_8K].cle_in_width;
        cle_attr->img_height = cle_cfg->cle_reso[CLE_ID_V0_8K].cle_in_height;
    } else {
        cle_attr->img_width = cle_cfg->cle_reso[CLE_ID_V0_4K].cle_in_width;
        cle_attr->img_height = cle_cfg->cle_reso[CLE_ID_V0_4K].cle_in_height;
    }

    cle_attr->demo_pos = cle_cfg->demo_pos;

    cle_attr->cle_ck_gt_en = 0;
    cle_attr->demo_en = cle_cfg->demo_en;
    cle_attr->demo_mode = cle_cfg->demo_mode;
    cle_attr->lti_en = 0;

    cle_attr->lti_limit_clip_en = 1;
    cle_attr->peak_limit_clip_en = 1;

    return;
}

hi_s32 pq_hal_cle_update_cfg(const alg_cle *cle_cfg)
{
    hi_s32 ret;
    cle_attr_cfg cle_attr = { 0 };

    if (cle_cfg == HI_NULL) {
        HI_ERR_PQ("cle_cfg is NULL Ptr!\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    pq_hal_assign_cle_attr(cle_cfg, &cle_attr);

    /* set para-update addr. */
    ret = pq_hal_cle_set_para_addr(&g_cle_hal_coef_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("regist para addr fail!\n");
        return ret;
    }

    ret = pq_hal_set_cle(cle_cfg, &cle_attr);

    return ret;
}

hi_s32 pq_hal_cle_set_strength(pq_cle_id id, hi_u32 strength, hi_u32 table_raisr, hi_u32 table_nlsr,
    hi_u32 table_peak)
{
    hi_u32 cfg_raisr;
    hi_u32 cfg_nlsr;
    hi_u32 cfg_peak;
    hi_u64 offset_addr;

    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_cle_reg);

    /* table gain is equal to 50% strength, so should re-calculate gain according to new strength. */
    cfg_raisr = (strength * table_raisr * 2) / 100; /* 2,100 are convert rate */
    cfg_nlsr = (strength * table_nlsr * 2) / 100;   /* 2,100 are convert rate */
    cfg_peak = (strength * table_peak * 2) / 100;   /* 2,100 are convert rate */

    if (id == CLE_ID_V0_4K) {
        offset_addr = CLE_REG_4K_OFFSET_ADDR;
    } else {
        offset_addr = CLE_REG_8K_OFFSET_ADDR;
        /* left chn */
        pq_reg_cle_set_raisr_gain(g_vdp_cle_reg, offset_addr + CLE_REG_8K_LEFT_OFFSET, cfg_raisr);
        pq_reg_cle_set_nlsr_gain(g_vdp_cle_reg, offset_addr + CLE_REG_8K_LEFT_OFFSET, cfg_nlsr);
        pq_reg_cle_set_peakgain(g_vdp_cle_reg, offset_addr + CLE_REG_8K_LEFT_OFFSET, cfg_peak);
        /* right chn */
        pq_reg_cle_set_raisr_gain(g_vdp_cle_reg, offset_addr + CLE_REG_8K_RIGHT_OFFSET, cfg_raisr);
        pq_reg_cle_set_nlsr_gain(g_vdp_cle_reg, offset_addr + CLE_REG_8K_RIGHT_OFFSET, cfg_nlsr);
        pq_reg_cle_set_peakgain(g_vdp_cle_reg, offset_addr + CLE_REG_8K_RIGHT_OFFSET, cfg_peak);
    }

    pq_reg_cle_set_raisr_gain(g_vdp_cle_reg, offset_addr, cfg_raisr);
    pq_reg_cle_set_nlsr_gain(g_vdp_cle_reg, offset_addr, cfg_nlsr);
    pq_reg_cle_set_peakgain(g_vdp_cle_reg, offset_addr, cfg_peak);

    return HI_SUCCESS;
}

hi_s32 pq_hal_cle_set_demo_en(hi_bool enable)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_cle_reg);

    /* set 4k cle demo status, 8k cle demo config in update_cfg function. */
    pq_reg_cle_set_demo_en(g_vdp_cle_reg, CLE_REG_4K_OFFSET_ADDR, enable);

    return HI_SUCCESS;
}

hi_s32 pq_hal_cle_set_demo_mode(pq_demo_mode demo_mode)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_cle_reg);

    /* set 4k cle demo status, 8k cle demo mode config in update_cfg function. */
    pq_reg_cle_set_demo_mode(g_vdp_cle_reg, CLE_REG_4K_OFFSET_ADDR, demo_mode);

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_cle_demo_pos(hi_u32 x_pos)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_cle_reg);
    /* set 4k cle pos, 8k cle demo pos config in update_cfg function. */
    pq_reg_cle_set_demo_pos(g_vdp_cle_reg, CLE_REG_4K_OFFSET_ADDR, x_pos);

    return HI_SUCCESS;
}