/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_ip_para.h"
#include "hal_vdp_test_zme.h"
#include "hal_vdp_reg_para.h"
#include "hal_vdp_reg_cvfir.h"
#include "hal_vdp_comm.h"


#define ZME_PHASE_NUM 17
#define ZME_MAX_INDEX 8

static hi_void vGetZmeInstPara(hi_u32 id, vdp_coef_in_para *zme_in_para)
{
    /* hinst0  vinst0  cvfir  hint1  vinst1  hcds */
    hi_u32 zme_y_en[] = { 1,  1,  1,  1,  1,  0 };
    hi_u32 zme_c_en[] = { 1,  1,  1,  1,  1,  1 };
    hi_u32 zme_y_tap[] = { 8,  6,  8,  8,  4,  4 };
    hi_u32 zme_c_tap[] = { 8,  6,  8,  8,  4,  4 };
    hi_u32 zme_dw[] = { 10, 10, 10, 8,  8,  8 };
    hi_u32 zme_cw[] = { 10, 10, 10, 10, 10, 10 };
    hi_u32 zme_phase[] = { 32, 32, 32, 32, 32, 32 };
    hi_u32 inst_id = 0;
    hi_u32 offset = 0;
    hi_u32 is_hzme = 0;
    hi_u8 *coef_addr = NULL;

    if (id == (hi_u32)VDP_ZME_ID_HZME_V0) {
        inst_id = 0;
        offset = (0x32600 - 0x3500) / 4; /* 4 is a addr offset div */
        is_hzme = 1;
        coef_addr = g_vdp_coef_buf_addr.vir_addr[VDP_COEF_BUF_8K_HZME];
    } else if (id == (hi_u32)VDP_ZME_ID_VZME_V0) {
        inst_id = 1;
        offset = (0x32b00 - 0x3600) / 4; /* 4 is a addr offset div */
        is_hzme = 0;
        coef_addr = g_vdp_coef_buf_addr.vir_addr[VDP_COEF_BUF_8K_VZME];
    }

    zme_in_para->offset = offset;
    zme_in_para->coef_addr = coef_addr;
    zme_in_para->zme_y_en = zme_y_en[inst_id];
    zme_in_para->zme_c_en = zme_c_en[inst_id];
    zme_in_para->zme_dw = zme_dw[inst_id];
    zme_in_para->zme_cw = zme_cw[inst_id];
    zme_in_para->zme_phase = zme_phase[inst_id];
    zme_in_para->zme_coeff_norm = zme_in_para->zme_cw - 1;

    if (is_hzme) {
        zme_in_para->zme_tap_hl = zme_y_tap[inst_id];
        zme_in_para->zme_tap_hc = zme_c_tap[inst_id];
        zme_in_para->zme_tap_vl = 6; /* 6 is a tap */
        zme_in_para->zme_tap_vc = 6; /* 6 is a tap */
    } else {
        zme_in_para->zme_tap_hl = 6; /* 6 is a tap */
        zme_in_para->zme_tap_hc = 6; /* 6 is a tap */
        zme_in_para->zme_tap_vl = zme_y_tap[inst_id];
        zme_in_para->zme_tap_vc = zme_c_tap[inst_id];
    }
}

static hi_void vdp_func_getzmecoef(hi_u32 id, vdp_cofe_pq *pq_cofe)
{
    hi_u32 i;
    hi_u32 j;
    vdp_coef_in_para zme_in_para;

    memset(&zme_in_para, 0, sizeof(vdp_coef_in_para));
    vGetZmeInstPara(id, &zme_in_para);

    for (i = 0; i < pq_cofe->phase; i++) {
        for (j = 0; j < pq_cofe->tap; j++) {
            if (zme_in_para.zme_cw > 10) { /* 10 is a offset */
                (pq_cofe->gen_lut)[i][j] =
                    (pq_cofe->typ_lut)[i][j] << (zme_in_para.zme_cw - 10); /* 10 is a offset */
            } else {
                (pq_cofe->gen_lut)[i][j] =
                    (pq_cofe->typ_lut)[i][j] >> (10 - zme_in_para.zme_cw); /* 10 is a offset */
            }
        }
    }
}

#define TAP8_UPPDATE_NORM512                      \
    {                                             \
        { -16, 0,   145, 254, 145, 0,  -16, 0 },     \
        { -16, -2,  140, 253, 151, 3,  -17, 0 },    \
        { -15, -5,  135, 253, 157, 5,  -18, 0 },    \
        { -14, -7,  129, 252, 162, 8,  -18, 0 },    \
        { -13, -9,  123, 252, 167, 11, -19, 0 },   \
        { -13, -11, 118, 250, 172, 15, -19, 0 },  \
        { -12, -12, 112, 250, 177, 18, -20, -1 }, \
        { -11, -14, 107, 247, 183, 21, -20, -1 }, \
        { -10, -15, 101, 245, 188, 25, -21, -1 }, \
        { -9,  -16, 96,  243, 192, 29, -21, -2 },   \
        { -8,  -18, 90,  242, 197, 33, -22, -2 },   \
        { -8,  -19, 85,  239, 202, 37, -22, -2 },   \
        { -7,  -19, 80,  236, 206, 41, -22, -3 },   \
        { -7,  -20, 75,  233, 210, 46, -22, -3 },   \
        { -6,  -21, 69,  230, 215, 50, -22, -3 },   \
        { -5,  -21, 65,  226, 219, 55, -22, -5 },   \
        { -5,  -21, 60,  222, 222, 60, -21, -5 },   \
    };

#define TAP6_UPDATE_512                       \
    {                                         \
        { -31, 104, 362, 104, -31, 4,  0, 0 }, \
        { -30, 94,  362, 114, -32, 4,  0, 0 },  \
        { -29, 84,  361, 125, -32, 3,  0, 0 },  \
        { -28, 75,  359, 136, -33, 3,  0, 0 },  \
        { -27, 66,  356, 147, -33, 3,  0, 0 },  \
        { -25, 57,  353, 158, -33, 2,  0, 0 },  \
        { -24, 49,  349, 169, -33, 2,  0, 0 },  \
        { -22, 41,  344, 180, -32, 1,  0, 0 },  \
        { -20, 33,  339, 191, -31, 0,  0, 0 },  \
        { -19, 26,  333, 203, -30, -1, 0, 0 }, \
        { -17, 19,  327, 214, -29, -2, 0, 0 }, \
        { -16, 13,  320, 225, -27, -3, 0, 0 }, \
        { -14, 7,   312, 236, -25, -4, 0, 0 },  \
        { -13, 1,   305, 246, -22, -5, 0, 0 },  \
        { -11, -4,  295, 257, -19, -6, 0, 0 }, \
        { -10, -8,  286, 267, -16, -7, 0, 0 }, \
        { -9,  -12, 277, 277, -12, -9, 0, 0 }, \
    };

static hi_s16 g_hzme_hl_coef_typ8[ZME_PHASE_NUM][ZME_MAX_INDEX] = TAP8_UPPDATE_NORM512;
static hi_s16 g_hzme_hc_coef_typ8[ZME_PHASE_NUM][ZME_MAX_INDEX] = TAP8_UPPDATE_NORM512;
static hi_s16 g_zme_vl_coef_typ6[ZME_PHASE_NUM][ZME_MAX_INDEX] = TAP6_UPDATE_512;
static hi_s16 g_zme_vc_coef_typ6[ZME_PHASE_NUM][ZME_MAX_INDEX] = TAP6_UPDATE_512;

// for rm
static hi_s16 g_zme_vl_coef_rm[ZME_PHASE_NUM][ZME_MAX_INDEX];
static hi_s16 g_zme_vc_coef_rm[ZME_PHASE_NUM][ZME_MAX_INDEX];

// for ddr
static hi_s16 g_zme_vl_coef_ddr[ZME_PHASE_NUM][ZME_MAX_INDEX];
static hi_s16 g_zme_vc_coef_ddr[ZME_PHASE_NUM][ZME_MAX_INDEX];

// for rm
static hi_s16 g_hzme_hl_coef_rm[ZME_PHASE_NUM][ZME_MAX_INDEX];
static hi_s16 g_hzme_hc_coef_rm[ZME_PHASE_NUM][ZME_MAX_INDEX];

// for ddr
static hi_s16 g_hzme_hl_coef_ddr[ZME_PHASE_NUM][ZME_MAX_INDEX];
static hi_s16 g_hzme_hc_coef_ddr[ZME_PHASE_NUM][ZME_MAX_INDEX];

static hi_void vdp_drv_setvzmecoef(hi_u32 u32ID, hi_s16 *pvl_coef, hi_s16 *pvc_coef)
{
    vdp_coef_send_cfg stCoefSend;
    hi_u8 *addr = HI_NULL;

    struct file *fp_vzme_coef = NULL;
    void *p_coef_array[1] = { pvl_coef };
    hi_u32 lut_length[1] = { 136 }; /* 136 is a coef length */
    hi_u32 coef_bit_length[1] = { 16 }; /* 16 is a coef length */
    vdp_coef_in_para stZmeInstPara = { 0 };

    vGetZmeInstPara(u32ID, &stZmeInstPara);
    addr = stZmeInstPara.coef_addr;

    stCoefSend.coef_addr = addr;
    stCoefSend.sti_type = VDP_STI_FILE_COEF_VZME;
    stCoefSend.fp_coef = fp_vzme_coef;
    stCoefSend.lut_num = 1;
    stCoefSend.burst_num = 1;
    stCoefSend.cycle_num = 8; /* 8 is a coef cycle num */
    stCoefSend.p_coef_array = p_coef_array;
    stCoefSend.lut_length = lut_length;
    stCoefSend.coef_bit_length = coef_bit_length;
    stCoefSend.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = vdp_ip_coef_sendcoef(&stCoefSend);
    p_coef_array[0] = pvc_coef;
    lut_length[0] = 136; /* 136 is a coef length */
    coef_bit_length[0] = 16; /* 16 is a coef length */

    stCoefSend.coef_addr = addr;
    stCoefSend.cycle_num = 8; /* 8 is a coef cycle num */
    stCoefSend.p_coef_array = p_coef_array;
    stCoefSend.lut_length = lut_length;
    stCoefSend.coef_bit_length = coef_bit_length;
    stCoefSend.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = vdp_ip_coef_sendcoef(&stCoefSend);
}

static hi_void vdp_func_zmeexchangelut(hi_u32 phase, hi_u32 tap, hi_s16 old_lut[ZME_PHASE_NUM][ZME_MAX_INDEX],
    hi_s16 new_lut[ZME_PHASE_NUM][ZME_MAX_INDEX])
{
    hi_u32 ii, jj;

    for (ii = 0; ii < phase; ii++) {
        for (jj = 0; jj < tap; jj++) {
            new_lut[ii][jj] = old_lut[ii][jj];
        }
    }
}

static hi_void vdp_drv_sethzmecoef(hi_u32 id, hi_s16 *phl_coef, hi_s16 *phc_coef)
{
    vdp_coef_send_cfg stCoefSend;
    hi_u8 *addr = 0;

    struct file *fp_hzme_h_coef = NULL;
    void *p_coef_array[1] = { phl_coef };
    hi_u32 lut_length[1] = { 136 }; /* 136 is a coef length */
    hi_u32 coef_bit_length[1] = { 16 }; /* 16 is a coef length */
    vdp_coef_in_para stZmeInstPara = { 0 };

    vGetZmeInstPara(id, &stZmeInstPara);
    addr = stZmeInstPara.coef_addr;

    stCoefSend.coef_addr = addr;
    stCoefSend.sti_type = VDP_STI_FILE_COEF_HZME;
    stCoefSend.fp_coef = fp_hzme_h_coef;
    stCoefSend.lut_num = 1;
    stCoefSend.burst_num = 1;
    stCoefSend.cycle_num = 8; /* 8 is a cycle_num */
    stCoefSend.p_coef_array = p_coef_array;
    stCoefSend.lut_length = lut_length;
    stCoefSend.coef_bit_length = coef_bit_length;
    stCoefSend.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = vdp_ip_coef_sendcoef(&stCoefSend);
    p_coef_array[0] = phc_coef;
    lut_length[0] = 136; /* 136 is a coef length */
    coef_bit_length[0] = 16; /* 16 is a coef length */

    stCoefSend.coef_addr = addr;
    stCoefSend.cycle_num = 8; /* 8 is a cycle_num */
    stCoefSend.p_coef_array = p_coef_array;
    stCoefSend.lut_length = lut_length;
    stCoefSend.coef_bit_length = coef_bit_length;
    stCoefSend.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = vdp_ip_coef_sendcoef(&stCoefSend);
}

hi_void vdp_zme_test_8kzme(hi_u32 layer, vdp_zme_image_info *info)
{
    vdp_coef_in_para zme_inst_para;
    vdp_cofe_pq pq_cofe;
    hi_u32 v_id, h_id;

    if (layer == VDP_LAYER_VID0) {
        v_id = VDP_ZME_ID_VZME_V0;
        h_id = VDP_ZME_ID_HZME_V0;
    } else {
        return;
    }

    /* V cofe */
    memset(&zme_inst_para, 0, sizeof(vdp_coef_in_para));
    vGetZmeInstPara(v_id, &zme_inst_para);

    pq_cofe.phase = ZME_PHASE_NUM;
    pq_cofe.tap = zme_inst_para.zme_tap_vl;

    pq_cofe.typ_lut = g_zme_vl_coef_typ6;
    pq_cofe.gen_lut = g_zme_vl_coef_rm;
    vdp_func_getzmecoef(v_id, &pq_cofe);
    vdp_func_zmeexchangelut(pq_cofe.phase, pq_cofe.tap, g_zme_vl_coef_rm, g_zme_vl_coef_ddr);

    pq_cofe.phase = ZME_PHASE_NUM;
    pq_cofe.tap = zme_inst_para.zme_tap_vc;
    pq_cofe.typ_lut = g_zme_vc_coef_typ6;
    pq_cofe.gen_lut = g_zme_vc_coef_rm;
    vdp_func_getzmecoef(v_id, &pq_cofe);
    vdp_func_zmeexchangelut(pq_cofe.phase, pq_cofe.tap, g_zme_vc_coef_rm, g_zme_vc_coef_ddr);

    vdp_drv_setvzmecoef(v_id, (hi_s16 *)g_zme_vl_coef_ddr, (hi_s16 *)g_zme_vc_coef_ddr);

    /* H cofe */
    memset(&zme_inst_para, 0, sizeof(vdp_coef_in_para));
    vGetZmeInstPara(h_id, &zme_inst_para);

    pq_cofe.phase = ZME_PHASE_NUM;
    pq_cofe.tap = zme_inst_para.zme_tap_hl;
    pq_cofe.typ_lut = g_hzme_hl_coef_typ8;
    pq_cofe.gen_lut = g_hzme_hl_coef_rm;
    vdp_func_getzmecoef(h_id, &pq_cofe);
    vdp_func_zmeexchangelut(pq_cofe.phase, pq_cofe.tap, g_hzme_hl_coef_rm, g_hzme_hl_coef_ddr);

    pq_cofe.phase = ZME_PHASE_NUM;
    pq_cofe.tap = zme_inst_para.zme_tap_hc;
    pq_cofe.typ_lut = g_hzme_hc_coef_typ8;
    pq_cofe.gen_lut = g_hzme_hc_coef_rm;
    vdp_func_getzmecoef(h_id, &pq_cofe);
    vdp_func_zmeexchangelut(pq_cofe.phase, pq_cofe.tap, g_hzme_hc_coef_rm, g_hzme_hc_coef_ddr);

    vdp_drv_sethzmecoef(h_id, (hi_s16 *)g_hzme_hl_coef_ddr, (hi_s16 *)g_hzme_hc_coef_ddr);

    return;
}

