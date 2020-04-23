/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: set pq zme coef
 * Author: pq
 * Create: 2016-01-1
 */

#include "pq_hal_zme_set_coef.h"
#include "hal_pq_ip_coef.h"

hi_s16 g_zme_4k_hl_coef[17][8] = {0}; /* 17:phase size, 8: tap size */
hi_s16 g_zme_4k_hc_coef[17][8] = {0}; /* 17:phase size, 8: tap size */
hi_s16 g_zme_4k_vl_coef[17][8] = {0}; /* 17:phase size, 8: tap size */
hi_s16 g_zme_4k_vc_coef[17][8] = {0}; /* 17:phase size, 8: tap size */

hi_s16 g_zme_8k_hl_coef[17][8] = {0}; /* 17:phase size, 8: tap size */
hi_s16 g_zme_8k_hc_coef[17][8] = {0}; /* 17:phase size, 8: tap size */
hi_s16 g_zme_8k_vl_coef[17][8] = {0}; /* 17:phase size, 8: tap size */
hi_s16 g_zme_8k_vc_coef[17][8] = {0}; /* 17:phase size, 8: tap size */

hi_s32 g_sr_lsth_coef_horz[1560] = {0}; /* 1560:SR coef size */
hi_s32 g_sr_lsth_coef[1560] = {0}; /* 1560:SR coef size */
hi_s32 g_sr_c_coef_horz[1560] = {0}; /* 1560:SR coef size */
hi_s32 g_sr_c_coef[1560] = {0}; /* 1560:SR coef size */
hi_s32 g_sr_tree_cluster[150 * 3 * 3] = {0}; /* 150 * 3 * 3:SR tree size */
hi_s32 g_sr_tree_reg_mtr[256 * 16 * 16] = {0}; /* 256 * 16 * 16:SR tree size */

static hi_void pq_zme_exchange_lut_scaler(hi_u32 phase, hi_u32 tap,
    hi_s16 old_lut[17][8], hi_s16 new_lut[17][8])  /* 17:phase size, 8: tap size */
{
    hi_u32 i, j;

    for (i = 0; i < phase; i++) {
        for (j = 0; j < tap; j++) {
            new_lut[i][j] = old_lut[i][tap - 1 - j];
        }
    }
}

static hi_void pq_get_zme_inst_para(hi_u32 id, pq_coef_in_para* zme_in_para, hi_u8* coef_addr_h, hi_u8* coef_addr_v)
{
    /* hinst0  vinst0  cvfir  hint1  vinst1  hcds */
    hi_u32 zme_y_en[]       = { 1,  1,  1,  1,  1,  0};
    hi_u32 zme_c_en[]       = { 1,  1,  1,  1,  1,  1};
    hi_u32 zme_y_tap[]      = { 8,  6,  8,  8,  4,  4};
    hi_u32 zme_c_tap[]      = { 8,  6,  8,  8,  4,  4};
    hi_u32 zme_dw[]         = {10, 10, 10,  8,  8,  8};
    hi_u32 zme_cw[]         = {10, 10, 10, 10, 10, 10};
    hi_u32 zme_phase[]      = {32, 32, 32, 32, 32, 32};
    hi_u32 zme_y_rat[]      = { 0,  0,  0,  0,  0,  0};
    hi_u32 zme_c_rat[]      = { 0,  0,  0,  0,  0,  0};
    hi_u32 zme_y_coef_fix[] = { 0,  0,  0,  0,  0,  0};
    hi_u32 zme_y_coef_set[] = { 0,  0,  0,  0,  0,  0};
    hi_u32 zme_c_coef_fix[] = { 0,  0,  0,  0,  0,  0};
    hi_u32 zme_c_coef_set[] = { 0,  0,  0,  0,  0,  0};

    hi_u32 inst_id = 0;
    hi_u32 offset = 0;
    hi_u32 is_hzme = 0;
    hi_u8* coef_addr = NULL;

    PQ_CHECK_NULL_PTR_RE_NULL(coef_addr_h);
    PQ_CHECK_NULL_PTR_RE_NULL(coef_addr_v);
    PQ_CHECK_NULL_PTR_RE_NULL(zme_in_para);

    if (id == (hi_u32)PQ_ZME_ID_HZME_V0) {
        inst_id = 0;
        offset = (0x32600 - 0x3500) / 4; /* 0x32600/0x3500: offset, 4: byte */
        is_hzme = 1;
        coef_addr = coef_addr_h;
    } else if (id == (hi_u32)PQ_ZME_ID_VZME_V0) {
        inst_id = 1;
        offset = (0x32b00 - 0x3600) / 4; /* 0x32b00/0x3600: offset, 4: byte */
        is_hzme = 0;
        coef_addr = coef_addr_v;
    } else if (id == (hi_u32)PQ_ZME_ID_CVFIR) {
        inst_id = 1;
        offset = 0;
        is_hzme = 0;
        coef_addr = 0;
    }

    zme_in_para->offset = offset;
    zme_in_para->coef_addr = coef_addr;
    zme_in_para->max_iw = 8192; /* 8192: max in width */
    zme_in_para->max_ih = 8192; /* 8192: max height */
    zme_in_para->max_ow = 7680; /* 7680: max out width */
    zme_in_para->max_oh = 7680; /* 7680: max out height */

    zme_in_para->zme_y_en = zme_y_en[inst_id];
    zme_in_para->zme_c_en = zme_c_en[inst_id];
    zme_in_para->zme_dw = zme_dw[inst_id];
    zme_in_para->zme_cw = zme_cw[inst_id];
    zme_in_para->zme_phase = zme_phase[inst_id];

    zme_in_para->zme_coeff_norm = zme_in_para->zme_cw - 1;

    zme_in_para->zme_y_coef_fix = zme_y_coef_fix[inst_id];
    zme_in_para->zme_y_coef_set = zme_y_coef_set[inst_id];
    zme_in_para->zme_c_coef_fix = zme_c_coef_fix[inst_id];
    zme_in_para->zme_c_coef_set = zme_c_coef_set[inst_id];

    if (is_hzme) {
        zme_in_para->zme_hl_rat = zme_y_rat[inst_id];
        zme_in_para->zme_hc_rat = zme_c_rat[inst_id];
        zme_in_para->zme_vl_rat = (1 << 12);  /* 12: ratio spcesion */
        zme_in_para->zme_vc_rat = (1 << 12);  /* 12: ratio spcesion */
        zme_in_para->zme_tap_hl = zme_y_tap[inst_id];
        zme_in_para->zme_tap_hc = zme_c_tap[inst_id];
        zme_in_para->zme_tap_vl = 6; /* 6: tap */
        zme_in_para->zme_tap_vc = 6; /* 6: tap */
    } else {
        zme_in_para->zme_hl_rat = (1 << 12);  /* 12: ratio spcesion */
        zme_in_para->zme_hc_rat = (1 << 12);  /* 12: ratio spcesion */
        zme_in_para->zme_vl_rat = zme_y_rat[inst_id];
        zme_in_para->zme_vc_rat = zme_c_rat[inst_id];
        zme_in_para->zme_tap_hl = 6;  /* 6: tap */
        zme_in_para->zme_tap_hc = 6; /* 6: tap */
        zme_in_para->zme_tap_vl = zme_y_tap[inst_id];
        zme_in_para->zme_tap_vc = zme_c_tap[inst_id];
    }
}

static hi_void pq_get_zme_coef(hi_u32 id, pq_zme_coef *pq_cofe, hi_u8 *coef_addr_h, hi_u8 *coef_addr_v)
{
    hi_u32 i;
    hi_u32 j;
    pq_coef_in_para zme_in_para;

    memset(&zme_in_para, 0, sizeof(pq_coef_in_para));
    pq_get_zme_inst_para(id, &zme_in_para, coef_addr_h, coef_addr_v);

    for (i = 0; i < pq_cofe->phase; i++) {
        for (j = 0; j < pq_cofe->tap; j++) {
            if (zme_in_para.zme_cw > 10) { /* 10: specison */
                (pq_cofe->gen_lut)[i][j] = (pq_cofe->typ_lut)[i][j] << (zme_in_para.zme_cw - 10); /* 10: specison */
            } else {
                (pq_cofe->gen_lut)[i][j] = (pq_cofe->typ_lut)[i][j] >> (10 - zme_in_para.zme_cw); /* 10: specison */
            }
        }
    }
}

#define TAP8_2DSCALE_NORM512_MAX               \
    {                                          \
        511, 511, 511, 511, 511, 511, 511, 511 \
    }
#define TAP8_2DSCALE_NORM512_MIN                               \
    {                                                          \
        - 512, -512, -512, -512, -512, -512, -512, -512 \
    }

#define TAP6_2DSCALE_NORM512_MAX           \
    {                                      \
        0, 0, 511, 511, 511, 511, 511, 511 \
    }
#define TAP6_2DSCALE_NORM512_MIN                 \
    {                                            \
        0, 0, -512, -512, -512, -512, -512, -512 \
    }

#define TAP4_2DSCALE_NORM512_MAX       \
    {                                  \
        0, 0, 0, 0, 511, 511, 511, 511 \
    }
#define TAP4_2DSCALE_NORM512_MIN           \
    {                                      \
        0, 0, 0, 0, -512, -512, -512, -512 \
    }

#define TAP6_2DSCALE_NORM64_MAX      \
    {                                \
        0, 0, 15, 31, 63, 63, 31, 15 \
    }
#define TAP6_2DSCALE_NORM64_MIN        \
    {                                  \
        0, 0, -16, -32, 0, 0, -32, -16 \
    }

#define TAP5_2DSCALE_NORM64_MAX     \
    {                               \
        0, 0, 0, 31, 63, 63, 31, 15 \
    }
#define TAP5_2DSCALE_NORM64_MIN      \
    {                                \
        0, 0, 0, -32, 0, 0, -32, -16 \
    }

#define TAP4_2DSCALE_NORM64_MAX    \
    {                              \
        0, 0, 0, 0, 31, 63, 63, 31 \
    }
#define TAP4_2DSCALE_NORM64_MIN    \
    {                              \
        0, 0, 0, 0, -32, 0, 0, -32 \
    }

#define TAP3_2DSCALE_NORM64_MAX \
    {                           \
        0, 0, 0, 0, 63, 0, 0, 0 \
    }
#define TAP3_2DSCALE_NORM64_MIN \
    {                           \
        0, 0, 0, 0, 0, 0, 0, 0  \
    }

static hi_s16 g_pq_2dscale_hl_max_val[8] = TAP8_2DSCALE_NORM512_MAX; /* 8: coef len */
static hi_s16 g_pq_2dscale_hc_max_val[8] = TAP8_2DSCALE_NORM512_MAX; /* 8: coef len */
static hi_s16 g_pq_2dscale_vl_max_val[8] = TAP6_2DSCALE_NORM512_MAX; /* 8: coef len */
static hi_s16 g_pq_2dscale_vc_max_val[8] = TAP6_2DSCALE_NORM512_MAX; /* 8: coef len */
static hi_s16 g_pq_2dscale_hl_min_val[8] = TAP8_2DSCALE_NORM512_MIN; /* 8: coef len */
static hi_s16 g_pq_2dscale_hc_min_val[8] = TAP8_2DSCALE_NORM512_MIN; /* 8: coef len */
static hi_s16 g_pq_2dscale_vl_min_val[8] = TAP6_2DSCALE_NORM512_MIN; /* 8: coef len */
static hi_s16 g_pq_2dscale_vc_min_val[8] = TAP6_2DSCALE_NORM512_MIN; /* 8: coef len */

hi_s16 g_pq_2dscale_hl_coef_typ[17][8]; /* 17:phase size, 8: tap size */
hi_s16 g_pq_2dscale_hc_coef_typ[17][8]; /* 17:phase size, 8: tap size */
hi_s16 g_pq_2dscale_vl_coef_typ[17][8]; /* 17:phase size, 8: tap size */
hi_s16 g_pq_2dscale_vc_coef_typ[17][8]; /* 17:phase size, 8: tap size */

/*  for rm */
static hi_s16 g_pq_2dscale_hl_coef_rm[17][8]; /* 17:phase size, 8: tap size */
static hi_s16 g_pq_2dscale_hc_coef_rm[17][8]; /* 17:phase size, 8: tap size */
static hi_s16 g_pq_2dscale_vl_coef_rm[17][8]; /* 17:phase size, 8: tap size */
static hi_s16 g_pq_2dscale_vc_coef_rm[17][8]; /* 17:phase size, 8: tap size */

/*  for ddr */
static hi_s16 g_pq_2dscale_hl_coef_ddr[17][8]; /* 17:phase size, 8: tap size */
static hi_s16 g_pq_2dscale_hc_coef_ddr[17][8]; /* 17:phase size, 8: tap size */
static hi_s16 g_pq_2dscale_vl_coef_ddr[17][8]; /* 17:phase size, 8: tap size */
static hi_s16 g_pq_2dscale_vc_coef_ddr[17][8]; /* 17:phase size, 8: tap size */

static hi_s32 pq_zme_set_2dscaler_coef_h(hi_s16 *phl_coef, hi_s16 *phc_coef, hi_u8 *addr)
{
    pq_coef_send_cfg stCoefSend;

    struct file* fp_xdp_2dscale_h_coef = NULL;
    void* p_coef_array[1] = { phl_coef };
    hi_u32 lut_length[1] = { 136 };
    hi_u32 coef_bit_length[1] = { 16 };

    PQ_CHECK_NULL_PTR_RE_FAIL(phl_coef);
    PQ_CHECK_NULL_PTR_RE_FAIL(phc_coef);
    PQ_CHECK_NULL_PTR_RE_FAIL(addr);

    stCoefSend.coef_addr = addr;
    stCoefSend.sti_type = PQ_STI_FILE_COEF_HZME;
    stCoefSend.fp_coef = fp_xdp_2dscale_h_coef;
    stCoefSend.lut_num = 1;
    stCoefSend.burst_num = 1;
    stCoefSend.cycle_num = 8; /* 8:cycle_num */
    stCoefSend.p_coef_array = p_coef_array;
    stCoefSend.lut_length = lut_length;
    stCoefSend.coef_bit_length = coef_bit_length;
    stCoefSend.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = pq_send_coef2ddr(&stCoefSend);
    p_coef_array[0] = phc_coef;
    lut_length[0] = 136; /* 136:lut_length */
    coef_bit_length[0] = 16; /* 16:coef_bit_length */

    stCoefSend.coef_addr = addr;
    stCoefSend.cycle_num = 8; /* 8:cycle_num */
    stCoefSend.p_coef_array = p_coef_array;
    stCoefSend.lut_length = lut_length;
    stCoefSend.coef_bit_length = coef_bit_length;
    stCoefSend.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = pq_send_coef2ddr(&stCoefSend);

    return HI_SUCCESS;
}

static hi_s32 pq_zme_set_2dscaler_coef_v(hi_s16 *pvl_coef_lut, hi_s16 *pvc_coef_lut, hi_u8 *addr)
{
    pq_coef_send_cfg stCoefSend;

    void* p_coef_array[1] = { pvl_coef_lut };
    hi_u32 lut_length[1] = { 136 }; /* 136:lut_length */
    hi_u32 coef_bit_length[1] = { 16 }; /* 16:bit len */

    struct file* fp_xdp_2dscale_v_coef = NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(pvl_coef_lut);
    PQ_CHECK_NULL_PTR_RE_FAIL(pvc_coef_lut);
    PQ_CHECK_NULL_PTR_RE_FAIL(addr);

    stCoefSend.coef_addr = addr;
    stCoefSend.sti_type = PQ_STI_FILE_COEF_VZME;
    stCoefSend.fp_coef = fp_xdp_2dscale_v_coef;
    stCoefSend.lut_num = 1;
    stCoefSend.burst_num = 1;
    stCoefSend.cycle_num = 8; /* 8:cycle_num */
    stCoefSend.p_coef_array = p_coef_array;
    stCoefSend.lut_length = lut_length;
    stCoefSend.coef_bit_length = coef_bit_length;
    stCoefSend.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = pq_send_coef2ddr(&stCoefSend);

    p_coef_array[0] = pvc_coef_lut;
    lut_length[0] = 136; /* 136:lut_length */
    coef_bit_length[0] = 16; /* 16:bit len */

    stCoefSend.coef_addr = addr;
    stCoefSend.sti_type = PQ_STI_FILE_COEF_VZME;
    stCoefSend.fp_coef = fp_xdp_2dscale_v_coef;
    stCoefSend.lut_num = 1;
    stCoefSend.burst_num = 1;
    stCoefSend.cycle_num = 8; /* 8:cycle_num */
    stCoefSend.p_coef_array = p_coef_array;
    stCoefSend.lut_length = lut_length;
    stCoefSend.coef_bit_length = coef_bit_length;
    stCoefSend.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = pq_send_coef2ddr(&stCoefSend);

    return HI_SUCCESS;
}

hi_void pq_hal_set_4kzme_coef(hi_u32 offset, hi_u8 *coef_addr_h, hi_u8 *coef_addr_v)
{
    pq_zme_coef pq_cofe;
    hi_bool tap_reduce_en;
    vdp_regs_type* vdp_reg = HI_NULL;
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_NULL(coef_addr_h);
    PQ_CHECK_NULL_PTR_RE_NULL(coef_addr_v);

    vdp_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_NULL(vdp_reg);

    tap_reduce_en = vdp_reg->xdp_2dscale_vinfo.bits.tap_reduce_en;

    /* H cofe */
    pq_cofe.phase = 17; /* 17:phase */
    pq_cofe.tap = 8; /* 8: tap */
    pq_cofe.typ_lut = g_zme_4k_hl_coef;
    pq_cofe.gen_lut = g_pq_2dscale_hl_coef_rm;
    pq_cofe.max_val = g_pq_2dscale_hl_max_val;
    pq_cofe.min_val = g_pq_2dscale_hl_min_val;

    pq_get_zme_coef(0, &pq_cofe, coef_addr_h, coef_addr_v);
    pq_zme_exchange_lut_scaler(pq_cofe.phase, pq_cofe.tap, g_pq_2dscale_hl_coef_rm, g_pq_2dscale_hl_coef_ddr);

    pq_cofe.phase = 17; /* 17:phase */
    pq_cofe.tap = 8; /* 8: tap */
    pq_cofe.typ_lut = g_zme_4k_hc_coef;
    pq_cofe.gen_lut = g_pq_2dscale_hc_coef_rm;
    pq_cofe.max_val = g_pq_2dscale_hc_max_val;
    pq_cofe.min_val = g_pq_2dscale_hc_min_val;

    pq_get_zme_coef(0, &pq_cofe, coef_addr_h, coef_addr_v);
    pq_zme_exchange_lut_scaler(pq_cofe.phase, pq_cofe.tap, g_pq_2dscale_hc_coef_rm, g_pq_2dscale_hc_coef_ddr);
    ret = pq_zme_set_2dscaler_coef_h((hi_s16*)g_pq_2dscale_hl_coef_ddr,
                                     (hi_s16*)g_pq_2dscale_hc_coef_ddr, coef_addr_h);
    PQ_CHECK_SUCCESS_RETURN_NULL(ret);

    /* V cofe */
    pq_cofe.phase = 17; /* 17:phase */
    pq_cofe.tap = 8; /* 8: tap */
    pq_cofe.typ_lut = g_zme_4k_vl_coef;
    pq_cofe.gen_lut = g_pq_2dscale_vl_coef_rm;
    pq_cofe.max_val = g_pq_2dscale_vl_max_val;
    pq_cofe.min_val = g_pq_2dscale_vl_min_val;

    pq_get_zme_coef(0, &pq_cofe, coef_addr_h, coef_addr_v);
    pq_zme_exchange_lut_scaler(pq_cofe.phase, pq_cofe.tap, g_pq_2dscale_vl_coef_rm, g_pq_2dscale_vl_coef_ddr);

    pq_cofe.phase = 17; /* 17:phase */
    pq_cofe.tap = 8; /* 8: tap */
    pq_cofe.typ_lut = g_zme_4k_vc_coef;
    pq_cofe.gen_lut = g_pq_2dscale_vc_coef_rm;
    pq_cofe.max_val = g_pq_2dscale_vc_max_val;
    pq_cofe.min_val = g_pq_2dscale_vc_min_val;

    pq_get_zme_coef(0, &pq_cofe, coef_addr_h, coef_addr_v);
    pq_zme_exchange_lut_scaler(pq_cofe.phase, pq_cofe.tap, g_pq_2dscale_vc_coef_rm, g_pq_2dscale_vc_coef_ddr);
    ret = pq_zme_set_2dscaler_coef_v((hi_s16*)g_pq_2dscale_vl_coef_ddr,
                                     (hi_s16*)g_pq_2dscale_vc_coef_ddr, coef_addr_v);
    PQ_CHECK_SUCCESS_RETURN_NULL(ret);
}

#define TAP3_LBL_NORM512_MAX          \
    {                                 \
        0, 511, 511, 511, 0, 0, 0, 0, \
    }
#define TAP3_LBL_NORM512_MIN             \
    {                                    \
        0, -512, -512, -512, 0, 0, 0, 0, \
    }

#define TAP2_LBL_NORM512_MAX       \
    {                              \
        0, 511, 511, 0, 0, 0, 0, 0 \
    }
#define TAP2_LBL_NORM512_MIN         \
    {                                \
        0, -512, -512, 0, 0, 0, 0, 0 \
    }

#define TAP8_NORM512_MAX                       \
    {                                          \
        511, 511, 511, 511, 511, 511, 511, 511 \
    }
#define TAP8_NORM512_MIN                                       \
    {                                                          \
        - 512, -512, -512, -512, -512, -512, -512, -512 \
    }

#define TAP6_NORM512_MAX                   \
    {                                      \
        511, 511, 511, 511, 511, 511, 0, 0 \
    }
#define TAP6_NORM512_MIN                                 \
    {                                                    \
        - 512, -512, -512, -512, -512, -512, 0, 0 \
    }

#define TAP4_NORM512_MAX               \
    {                                  \
        511, 511, 511, 511, 0, 0, 0, 0 \
    }
#define TAP4_NORM512_MIN                          \
    {                                             \
        - 512, -512, -512, -512, 0 0, 0, 0 \
    }

#define TAP6_NORM64_MAX              \
    {                                \
        15, 31, 63, 63, 31, 15, 0, 0 \
    }
#define TAP6_NORM64_MIN                        \
    {                                          \
        - 16, -32, 0, 0, -32, -16, 0, 0 \
    }

#define TAP5_NORM64_MAX             \
    {                               \
        0, 31, 63, 63, 31, 15, 0, 0 \
    }
#define TAP5_NORM64_MIN              \
    {                                \
        0, -32, 0, 0, -32, -16, 0, 0 \
    }

#define TAP4_NORM64_MAX            \
    {                              \
        31, 63, 63, 31, 0, 0, 0, 0 \
    }
#define TAP4_NORM64_MIN                    \
    {                                      \
        - 32, 0, 0, -32, 0, 0, 0, 0 \
    }

static hi_s16 g_pq_hzme_hl_max_val[8] = TAP8_NORM512_MAX;  /* 8:coef size */
static hi_s16 g_pq_hzme_hc_max_val[8] = TAP8_NORM512_MAX; /* 8:coef size */
static hi_s16 g_pq_hzme_hl_min_val[8] = TAP8_NORM512_MIN; /* 8:coef size */
static hi_s16 g_pq_hzme_hc_min_val[8] = TAP8_NORM512_MIN; /* 8:coef size */

static hi_s16 g_pq_zme_vl_max_val[8] = TAP6_NORM512_MAX; /* 8:coef size */
static hi_s16 g_pq_zme_vc_max_val[8] = TAP6_NORM512_MAX; /* 8:coef size */
static hi_s16 g_pq_zme_vl_min_val[8] = TAP6_NORM512_MIN; /* 8:coef size */
static hi_s16 g_pq_zme_vc_min_val[8] = TAP6_NORM512_MIN; /* 8:coef size */

/* for rm */
static hi_s16 g_pq_zme_vl_coef_rm[17][8]; /* 17:phase size, 8: tap size */
static hi_s16 g_pq_zme_vc_coef_rm[17][8]; /* 17:phase size, 8: tap size */

/* for ddr */
static hi_s16 g_pq_zme_vl_coef_ddr[17][8]; /* 17:phase size, 8: tap size */
static hi_s16 g_pq_zme_vc_coef_ddr[17][8]; /* 17:phase size, 8: tap size */

/* for rm */
static hi_s16 g_pq_hzme_hl_coef_rm[17][8]; /* 17:phase size, 8: tap size */
static hi_s16 g_pq_hzme_hc_coef_rm[17][8]; /* 17:phase size, 8: tap size */

/* for ddr */
static hi_s16 g_pq_hzme_hl_coef_ddr[17][8]; /* 17:phase size, 8: tap size */
static hi_s16 g_pq_hzme_hc_coef_ddr[17][8]; /* 17:phase size, 8: tap size */

static hi_s32 pq_set_zme_coef_v(hi_u32 u32ID, hi_s16* pvl_coef, hi_s16* pvc_coef,
                                hi_u8* coef_addr_h, hi_u8* coef_addr_v)
{
    pq_coef_send_cfg stCoefSend;
    hi_u8* addr = 0;

    struct file* fp_vzme_coef = NULL;
    void* p_coef_array[1] = { pvl_coef };
    hi_u32 lut_length[1] = { 136 }; /* 136: lut len */
    hi_u32 coef_bit_length[1] = { 16 }; /* 16: bit len */
    pq_coef_in_para stZmeInstPara = { 0 };

    PQ_CHECK_NULL_PTR_RE_FAIL(pvl_coef);
    PQ_CHECK_NULL_PTR_RE_FAIL(pvc_coef);
    PQ_CHECK_NULL_PTR_RE_FAIL(coef_addr_h);
    PQ_CHECK_NULL_PTR_RE_FAIL(coef_addr_v);

    pq_get_zme_inst_para(u32ID, &stZmeInstPara, coef_addr_h, coef_addr_v);
    addr = stZmeInstPara.coef_addr;

    stCoefSend.coef_addr = addr;
    stCoefSend.sti_type = PQ_STI_FILE_COEF_VZME;
    stCoefSend.fp_coef = fp_vzme_coef;
    stCoefSend.lut_num = 1;
    stCoefSend.burst_num = 1;
    stCoefSend.cycle_num = 8; /* 8: cycle_num */
    stCoefSend.p_coef_array = p_coef_array;
    stCoefSend.lut_length = lut_length;
    stCoefSend.coef_bit_length = coef_bit_length;
    stCoefSend.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = pq_send_coef2ddr(&stCoefSend);
    p_coef_array[0] = pvc_coef;
    lut_length[0] = 136; /* 136: lut len */
    coef_bit_length[0] = 16; /* 16: bit len */

    stCoefSend.coef_addr = addr;
    stCoefSend.cycle_num = 8; /* 8: cycle_num */
    stCoefSend.p_coef_array = p_coef_array;
    stCoefSend.lut_length = lut_length;
    stCoefSend.coef_bit_length = coef_bit_length;
    stCoefSend.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = pq_send_coef2ddr(&stCoefSend);

    return HI_SUCCESS;
}

static hi_void pq_zme_exchange_lut(hi_u32 phase, hi_u32 tap,
                                   hi_s16 old_lut[17][8], hi_s16 new_lut[17][8])  /* 17:phase size, 8: tap size */
{
    hi_u32 ii, jj;

    for (ii = 0; ii < phase; ii++) {
        for (jj = 0; jj < tap; jj++) {
            new_lut[ii][jj] = old_lut[ii][jj];
        }
    }
}

static hi_s32 pq_set_zme_coef_h(hi_u32 id, hi_s16 *phl_coef, hi_s16 *phc_coef, hi_u8 *coef_addr_h, hi_u8 *coef_addr_v)
{
    pq_coef_send_cfg stCoefSend;
    hi_u8* addr = 0;

    struct file* fp_hzme_h_coef = NULL;
    void* p_coef_array[1] = { phl_coef };
    hi_u32 lut_length[1] = { 136 }; /* 136: lut len */
    hi_u32 coef_bit_length[1] = { 16 }; /* 16: bit len */
    pq_coef_in_para stZmeInstPara = { 0 };

    PQ_CHECK_NULL_PTR_RE_FAIL(phl_coef);
    PQ_CHECK_NULL_PTR_RE_FAIL(phc_coef);
    PQ_CHECK_NULL_PTR_RE_FAIL(coef_addr_h);
    PQ_CHECK_NULL_PTR_RE_FAIL(coef_addr_v);

    pq_get_zme_inst_para(id, &stZmeInstPara, coef_addr_h, coef_addr_v);
    addr = stZmeInstPara.coef_addr;

    stCoefSend.coef_addr = addr;
    stCoefSend.sti_type = PQ_STI_FILE_COEF_HZME;
    stCoefSend.fp_coef = fp_hzme_h_coef;
    stCoefSend.lut_num = 1;
    stCoefSend.burst_num = 1;
    stCoefSend.cycle_num = 8; /* 8: cycle_num */
    stCoefSend.p_coef_array = p_coef_array;
    stCoefSend.lut_length = lut_length;
    stCoefSend.coef_bit_length = coef_bit_length;
    stCoefSend.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = pq_send_coef2ddr(&stCoefSend);
    p_coef_array[0] = phc_coef;
    lut_length[0] = 136;  /* 136:lut len */
    coef_bit_length[0] = 16;  /* 16:bit len */

    stCoefSend.coef_addr = addr;
    stCoefSend.cycle_num = 8; /* 8: cycle_num */
    stCoefSend.p_coef_array = p_coef_array;
    stCoefSend.lut_length = lut_length;
    stCoefSend.coef_bit_length = coef_bit_length;
    stCoefSend.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = pq_send_coef2ddr(&stCoefSend);

    return HI_SUCCESS;
}

hi_void pq_hal_set_8kzme_coef(pq_8kzme_type layer, hi_u8 *coef_addr_h, hi_u8 *coef_addr_v)
{
    pq_coef_in_para zme_inst_para;
    pq_zme_coef pq_cofe;
    hi_u32 v_id, h_id;
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_NULL(coef_addr_h);
    PQ_CHECK_NULL_PTR_RE_NULL(coef_addr_v);

    if (layer == PQ_8KZME_TYPE_VID1) {
        v_id = PQ_ZME_ID_VZME_V1;
        h_id = PQ_ZME_ID_HZME_V1;
    } else if (layer == PQ_8KZME_TYPE_VID0) {
        v_id = PQ_ZME_ID_VZME_V0;
        h_id = PQ_ZME_ID_HZME_V0;
    } else if (layer == PQ_8KZME_TYPE_WBC) {
        v_id = PQ_ZME_ID_VZME_WD;
        h_id = PQ_ZME_ID_HZME_WD;
    } else {
        return ;
    }

    /* V cofe */
    memset(&zme_inst_para, 0, sizeof(pq_coef_in_para));
    pq_get_zme_inst_para(v_id, &zme_inst_para, coef_addr_h, coef_addr_v);

    pq_cofe.phase = 17;  /* 17:phase */
    pq_cofe.tap = zme_inst_para.zme_tap_vl;
    pq_cofe.typ_lut = g_zme_8k_vl_coef;
    pq_cofe.gen_lut = g_pq_zme_vl_coef_rm;
    pq_cofe.max_val = g_pq_zme_vl_max_val;
    pq_cofe.min_val = g_pq_zme_vl_min_val;
    pq_get_zme_coef(v_id, &pq_cofe, coef_addr_h, coef_addr_v);
    pq_zme_exchange_lut(pq_cofe.phase, pq_cofe.tap, g_pq_zme_vl_coef_rm, g_pq_zme_vl_coef_ddr);

    pq_cofe.phase = 17; /* 17:phase */
    pq_cofe.tap = zme_inst_para.zme_tap_vc;
    pq_cofe.typ_lut = g_zme_8k_vc_coef;
    pq_cofe.gen_lut = g_pq_zme_vc_coef_rm;
    pq_cofe.max_val = g_pq_zme_vc_max_val;
    pq_cofe.min_val = g_pq_zme_vc_min_val;
    pq_get_zme_coef(v_id, &pq_cofe, coef_addr_h, coef_addr_v);
    pq_zme_exchange_lut(pq_cofe.phase, pq_cofe.tap, g_pq_zme_vc_coef_rm, g_pq_zme_vc_coef_ddr);
    ret = pq_set_zme_coef_v(v_id, (hi_s16*)g_pq_zme_vl_coef_ddr,
                            (hi_s16*)g_pq_zme_vc_coef_ddr, coef_addr_h, coef_addr_v);
    PQ_CHECK_SUCCESS_RETURN_NULL(ret);
    /* H cofe */
    memset(&zme_inst_para, 0, sizeof(pq_coef_in_para));
    pq_get_zme_inst_para(h_id, &zme_inst_para, coef_addr_h, coef_addr_v);

    pq_cofe.phase = 17; /* 17:phase */
    pq_cofe.tap = zme_inst_para.zme_tap_hl;
    pq_cofe.typ_lut = g_zme_8k_hl_coef;
    pq_cofe.gen_lut = g_pq_hzme_hl_coef_rm;
    pq_cofe.max_val = g_pq_hzme_hl_max_val;
    pq_cofe.min_val = g_pq_hzme_hl_min_val;
    pq_get_zme_coef(h_id, &pq_cofe, coef_addr_h, coef_addr_v);
    pq_zme_exchange_lut(pq_cofe.phase, pq_cofe.tap, g_pq_hzme_hl_coef_rm, g_pq_hzme_hl_coef_ddr);

    pq_cofe.phase = 17; /* 17:phase */
    pq_cofe.tap = zme_inst_para.zme_tap_hc;
    pq_cofe.typ_lut = g_zme_8k_hc_coef;
    pq_cofe.gen_lut = g_pq_hzme_hc_coef_rm;
    pq_cofe.max_val = g_pq_hzme_hc_max_val;
    pq_cofe.min_val = g_pq_hzme_hc_min_val;
    pq_get_zme_coef(h_id, &pq_cofe, coef_addr_h, coef_addr_v);
    pq_zme_exchange_lut(pq_cofe.phase, pq_cofe.tap, g_pq_hzme_hc_coef_rm, g_pq_hzme_hc_coef_ddr);
    ret = pq_set_zme_coef_h(h_id, (hi_s16*)g_pq_hzme_hl_coef_ddr,
                            (hi_s16*)g_pq_hzme_hc_coef_ddr, coef_addr_h, coef_addr_v);
    PQ_CHECK_SUCCESS_RETURN_NULL(ret);

    if (layer == PQ_8KZME_TYPE_WBC) {
        h_id = PQ_ZME_ID_HCDS_WD;
        memset(&zme_inst_para, 0, sizeof(pq_coef_in_para));
        pq_get_zme_inst_para(h_id, &zme_inst_para, coef_addr_h, coef_addr_v);

        pq_cofe.phase = 17; /* 17:phase */
        pq_cofe.tap = zme_inst_para.zme_tap_hl;
        pq_cofe.typ_lut = g_zme_8k_hl_coef;
        pq_cofe.gen_lut = g_pq_hzme_hl_coef_rm;
        pq_cofe.max_val = g_pq_hzme_hl_max_val;
        pq_cofe.min_val = g_pq_hzme_hl_min_val;
        pq_get_zme_coef(h_id, &pq_cofe, coef_addr_h, coef_addr_v);
        pq_zme_exchange_lut(pq_cofe.phase, pq_cofe.tap, g_pq_hzme_hl_coef_rm, g_pq_hzme_hl_coef_ddr);

        pq_cofe.phase = 17; /* 17:phase */
        pq_cofe.tap = zme_inst_para.zme_tap_hc;
        pq_cofe.typ_lut = g_zme_8k_hc_coef;
        pq_cofe.gen_lut = g_pq_hzme_hc_coef_rm;
        pq_cofe.max_val = g_pq_hzme_hc_max_val;
        pq_cofe.min_val = g_pq_hzme_hc_min_val;
        pq_get_zme_coef(h_id, &pq_cofe, coef_addr_h, coef_addr_v);
        pq_zme_exchange_lut(pq_cofe.phase, pq_cofe.tap, g_pq_hzme_hc_coef_rm, g_pq_hzme_hc_coef_ddr);
        pq_set_zme_coef_h(h_id, (hi_s16*)g_pq_hzme_hl_coef_ddr,
                          (hi_s16*)g_pq_hzme_hc_coef_ddr, coef_addr_h, coef_addr_v);
    }

    return;
}

static hi_s32 g_hipp_lsth_coef_new[1560]; /* 1560: sr coef size */
static hi_s32 g_hipp_c_coef_new[1560]; /* 1560: sr coef size */

typedef struct {
    hi_s32* luta;
    hi_s32* lutb;
    hi_s32* lutc;
    hi_s32* lutd;
} pq_sr_coef_array;

static hi_s32 pq_set_hipp_sr_coef(hi_u32 id, pq_sr_coef_array* sr_coef_array,
                                  hi_u32 mode, hi_u8* addr)
{
    struct file* fp_hipp_sr_coef = NULL;
    pq_coef_send_cfg coef_send;

    hi_u32 lut_length[1];
    hi_u32 coef_bit_length[1];

    hi_void* p_coef_array_a[1];
    hi_void* p_coef_array_b[1];
    hi_void* p_coef_array_d[1];
    hi_void* p_coef_array_c[1];

    PQ_CHECK_NULL_PTR_RE_FAIL(sr_coef_array);
    PQ_CHECK_NULL_PTR_RE_FAIL(addr);

    /* lut a */
    lut_length[0] = 1560; /* 1560:lut len */
    p_coef_array_a[0] = sr_coef_array->luta;
    coef_bit_length[0] = 12; /* 12:bit len */
    coef_send.coef_addr = addr;
    coef_send.sti_type = PQ_STI_FILE_COEF_HIPP_SR;
    coef_send.fp_coef = fp_hipp_sr_coef;
    coef_send.lut_num = 1;
    coef_send.burst_num = 1;
    coef_send.cycle_num = 10;  /* 10:cycle_num */
    coef_send.p_coef_array = p_coef_array_a;
    coef_send.lut_length = lut_length;
    coef_send.coef_bit_length = coef_bit_length;
    coef_send.data_type = DRV_COEF_DATA_TYPE_S32;
    addr = pq_send_coef2ddr(&coef_send);

    /* lut b */
    lut_length[0] = 1560; /* 1560:lut len */
    p_coef_array_b[0] = sr_coef_array->lutb;
    coef_bit_length[0] = 12; /* 12:bit len */
    coef_send.coef_addr = addr;
    coef_send.sti_type = PQ_STI_FILE_COEF_HIPP_SR;
    coef_send.fp_coef = fp_hipp_sr_coef;
    coef_send.lut_num = 1;
    coef_send.burst_num = 1;
    coef_send.cycle_num = 10;  /* 10:cycle_num */
    coef_send.p_coef_array = p_coef_array_b;
    coef_send.lut_length = lut_length;
    coef_send.coef_bit_length = coef_bit_length;
    coef_send.data_type = DRV_COEF_DATA_TYPE_S32;
    addr = pq_send_coef2ddr(&coef_send);

    if (mode == 0) {
        p_coef_array_c[0] = sr_coef_array->lutc;
        lut_length[0] = 510 * 3 * 3; /* 510 * 3 * 3:SR tree len */
        coef_bit_length[0] = 16; /* 16:bit len */

        coef_send.coef_addr = addr;
        coef_send.sti_type = PQ_STI_FILE_COEF_HIPP_SR;
        coef_send.fp_coef = fp_hipp_sr_coef;
        coef_send.lut_num = 1;
        coef_send.burst_num = 1;
        coef_send.cycle_num = 8;  /* 8:cycle_num */
        coef_send.p_coef_array = p_coef_array_c;
        coef_send.lut_length = lut_length;
        coef_send.coef_bit_length = coef_bit_length;
        coef_send.data_type = DRV_COEF_DATA_TYPE_S32;
        addr = pq_send_coef2ddr(&coef_send);

        p_coef_array_d[0] = sr_coef_array->lutd;
        lut_length[0] = 256 * 16 * 16; /* 256 * 16 * 16:SR tree len */
        coef_bit_length[0] = 21; /* 21:bit len */

        coef_send.coef_addr = addr;
        coef_send.sti_type = PQ_STI_FILE_COEF_HIPP_SR;
        coef_send.fp_coef = fp_hipp_sr_coef;
        coef_send.lut_num = 1;
        coef_send.burst_num = 1;
        coef_send.cycle_num = 6;  /* 6:cycle_num */
        coef_send.p_coef_array = p_coef_array_d;
        coef_send.lut_length = lut_length;
        coef_send.coef_bit_length = coef_bit_length;
        coef_send.data_type = DRV_COEF_DATA_TYPE_S32;
        addr = pq_send_coef2ddr(&coef_send);
    }

    return HI_SUCCESS;
}

static hi_void pq_set_sr_coef(hi_u32 id, hi_u32 offset, hi_u8 *addr)
{
    pq_coef_gen_cfg stCoefGen;
    u_hipp_sr_ctrl st_hipp_sr_ctrl;
    pq_sr_coef_array sr_coef_array;
    vdp_regs_type* vdp_reg = HI_NULL;
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_NULL(addr);

    vdp_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_NULL(vdp_reg);

    st_hipp_sr_ctrl.u32 = *(&(vdp_reg->hipp_sr_ctrl.u32) + offset);

    /* ============================= lsth_coef ================================= */
    if ((st_hipp_sr_ctrl.bits.scale_mode == 3) ||  /* 3:sr mode */
        (st_hipp_sr_ctrl.bits.scale_mode == 4)) {  /* 4:sr mode */
        stCoefGen.p_coef = g_sr_lsth_coef_horz;
    } else {
        stCoefGen.p_coef = g_sr_lsth_coef;
    }

    stCoefGen.p_coef_new = g_hipp_lsth_coef_new;
    stCoefGen.length = 1560; /* 1560:SR size */
    stCoefGen.coef_data_type = DRV_COEF_DATA_TYPE_S32;
    stCoefGen.coef_max = ((1 << (12 - 1)) - 1); /* 12:specision */
    stCoefGen.coef_min = (1 << 12); /* 12:specision */
    pq_drv_gen_coef(&stCoefGen);

    /* ============================= c_coef ================================= */
    if ((st_hipp_sr_ctrl.bits.scale_mode == 3) ||  /* 3:sr mode */
        (st_hipp_sr_ctrl.bits.scale_mode == 4)) {  /* 4:sr mode */
        stCoefGen.p_coef = g_sr_c_coef_horz;
    } else {
        stCoefGen.p_coef = g_sr_c_coef;
    }

    stCoefGen.p_coef_new = g_hipp_c_coef_new;
    stCoefGen.length = 1560; /* 1560:SR size */
    stCoefGen.coef_data_type = DRV_COEF_DATA_TYPE_S32;
    stCoefGen.coef_max = ((1 << (12 - 1)) - 1); /* 12:specision */
    stCoefGen.coef_min = (1 << 12); /* 12:specision */
    pq_drv_gen_coef(&stCoefGen);

    /* ============================= tree cluster ================================= */
    stCoefGen.p_coef = g_sr_tree_cluster;
    stCoefGen.p_coef_new = g_sr_tree_cluster;
    stCoefGen.length = 510 * 3 * 3; /* 510 * 3 * 3:SR tree size */
    stCoefGen.coef_data_type = DRV_COEF_DATA_TYPE_S32;
    stCoefGen.coef_max = ((1 << (15 - 1)) - 1); /* 15:specision */
    stCoefGen.coef_min = -(1 << (15 - 1)); /* 15:specision */
    pq_drv_gen_coef(&stCoefGen);

    /* ============================= tree regMtr ================================= */
    stCoefGen.p_coef = g_sr_tree_reg_mtr;
    stCoefGen.p_coef_new = g_sr_tree_reg_mtr;
    stCoefGen.length = 256 * 16 * 16; /* 256 * 16 * 16:SR tree size */
    stCoefGen.coef_data_type = DRV_COEF_DATA_TYPE_S32;
    stCoefGen.coef_max = ((1 << (17 - 1)) - 1); /* 17:specision */
    stCoefGen.coef_min = -(1 << (17 - 1)); /* 17:specision */
    pq_drv_gen_coef(&stCoefGen);

    sr_coef_array.luta = g_hipp_lsth_coef_new;
    sr_coef_array.lutb = g_hipp_c_coef_new;
    sr_coef_array.lutc = g_sr_tree_cluster;
    sr_coef_array.lutd = g_sr_tree_reg_mtr;
    ret = pq_set_hipp_sr_coef(id, &sr_coef_array, 0, addr);
    PQ_CHECK_SUCCESS_RETURN_NULL(ret);

    return;
}

hi_void pq_hal_set_4ksr_coef(hi_u32 offset, hi_u8 *addr)
{
    pq_set_sr_coef(HIPP_CLM_ID_V0_4K, 0x1000 / 4, addr); /* 4: byte size */
    return;
}

hi_void pq_hal_set_8ksr_coef(hi_u32 offset, hi_u8 *addr)
{
    pq_set_sr_coef(HIPP_CLM_ID_V0_8K, 0, addr);
    return;
}
