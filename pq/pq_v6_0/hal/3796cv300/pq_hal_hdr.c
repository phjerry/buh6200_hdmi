/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description:
 * Author: pq
 * Create: 2016-01-1
 */

#include <linux/string.h>
#include "hi_type.h"
#include "pq_hal_comm.h"
#include "drv_pq_ext.h"
#include "pq_hal_hdr.h"
#include "hal_pq_ip_coef.h"
#include "pq_hal_hdr_coef.h"

#define HDR_MIN(x, y)           (((x) > (y)) ? (y) : (x))
#define HDR_MAX(x, y)           (((x) > (y)) ? (x) : (y))
#define HDR_CLIP3(low, high, x) (HDR_MAX(HDR_MIN((high), (x)), (low)))
typedef hi_s32 (*pf_pq_hal_gethdrpathcfg)(hi_drv_color_descript en_in_cs,
    hi_drv_color_descript en_out_cs, pq_hal_hdr_cfg *pst_pq_hdr_cfg);

typedef struct {
    hi_pq_hdr_mode hdr_mode;
    pf_pq_hal_gethdrpathcfg hdr_func;
} pq_hdr_fun;

extern pq_hdr_coef_addr g_pq_vdp_hdr_coef_buf;

static pq_hal_hdr_cfg *g_pg_st_pq_hdr_cfg = HI_NULL;
static pq_bin_param *g_sg_pst_hdr_bin_para = HI_NULL;
hi_pq_hdr_mode g_sg_en_hdr_mode = HI_PQ_HDR_MODE_HDR10_TO_SDR;

static dm_info g_dminfo = { 0 };

static hi_u32 g_sg_u32_tool_hdr_tm_mode = 0;  /* hdr param mode: 0: fix param, 1: pq bin param */
static hi_u32 g_sg_u32_tool_hdr_sm_mode = 0;  /* hdr param mode: 0: fix param, 1: pq bin param */

static hi_u16 g_sg_au16_tool_tm_scale = 8;
static hi_u16 g_sg_au16_tool_sm_scale = 8;

static hi_pq_hdr_tmap g_tool_hdr_tmap = { { 0 } };
static hi_pq_hdr_smap g_tool_hdr_smap = { { 0 } };

const hi_s16 g_rgb2yuv_scale2p = 15;
const hi_s16 g_yuv2rgb_scale2p = 13;

static void *g_hdrv1_cm_lut = HI_NULL;
static void *g_imap_degamma = HI_NULL;
static void *g_imap_gamma_lut_x = HI_NULL;
static void *g_imap_gamma_lut_a = HI_NULL;
static void *g_imap_gamma_lut_b = HI_NULL;
static void *g_tmap_tm_lut_i = HI_NULL;
static void *g_tmap_tm_lut_s = HI_NULL;
static void *g_tmap_sm_lut_i = HI_NULL;
static void *g_tmap_sm_lut_s = HI_NULL;
static void *g_tmap_smc_lut = HI_NULL;
static hi_s32 g_pq_hdr_tmapv1_lut_c_new[256]; // 256 is array length
static void *g_tmap_hlg_lut_tmapv1 = HI_NULL;

static void *g_omap_degamma = HI_NULL;
static void *g_omap_gamma_lut_x = HI_NULL;
static void *g_omap_gamma_lut_a = HI_NULL;
static void *g_omap_gamma_lut_b = HI_NULL;
/* ------------------Y2R:YUV2RGB---------------------------- */
/* YUV to RGB params: 10bit in, 10bit out */
static hi_u32 g_hdr_mode[HI_PQ_HDR_MODE_MAX] = {
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0
};
static hi_u32 g_dci_dark_str[HI_PQ_HDR_MODE_MAX] = {
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0
};
static hi_u32 g_dci_bright_str[HI_PQ_HDR_MODE_MAX] = {
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0
};
static hi_u32 g_dark_cv_str[HI_PQ_HDR_MODE_MAX] = {
    50,
    50,
    50,
    50,
    50,
    50,
    50,
    50,
    50,
    50,
    50,
    50,
};
static hi_u32 g_bright_cv_str[HI_PQ_HDR_MODE_MAX] = {
    50,
    50,
    50,
    50,
    50,
    50,
    50,
    50,
    50,
    50,
    50,
    50,
};

static hi_pq_dci_histgram g_gs_st_dci_histgram;
static hi_u32 g_dci_out_width = 3840;
static hi_u32 g_dci_out_height = 2160;

static hi_u32 g_bright = PQ_CSC_VIDEO_DEFAULT;
static hi_u32 g_contrast = PQ_CSC_VIDEO_DEFAULT;
static hi_u32 g_satu = PQ_CSC_VIDEO_DEFAULT;
static hi_u32 g_hue = PQ_CSC_VIDEO_DEFAULT;
static hi_u32 g_wcg_temperature = PQ_CSC_VIDEO_DEFAULT;

static hi_u32 g_hdr_offset_bright[HI_PQ_HDR_MODE_MAX] = {
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
};
static hi_u32 g_hdr_offset_contrast[HI_PQ_HDR_MODE_MAX] = {
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
};
static hi_u32 g_hdr_offset_satu[HI_PQ_HDR_MODE_MAX] = {
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
};
static hi_u32 g_hdr_offset_hue[HI_PQ_HDR_MODE_MAX] = {
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
};
static hi_u32 g_hdr_offset_red[HI_PQ_HDR_MODE_MAX] = {
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
};
static hi_u32 g_hdr_offset_green[HI_PQ_HDR_MODE_MAX] = {
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
};
static hi_u32 g_hdr_offset_blue[HI_PQ_HDR_MODE_MAX] = {
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
};

/*****************************HDR 基本参数结构体*********************************/
typedef struct {
    hi_u16 *lut00_r;
    hi_u16 *lut00_g;
    hi_u16 *lut00_b;
    hi_u16 *lut01_r;
    hi_u16 *lut01_g;
    hi_u16 *lut01_b;
    hi_u16 *lut1_r;
    hi_u16 *lut1_g;
    hi_u16 *lut1_b;
    hi_u16 *lut2_r;
    hi_u16 *lut2_g;
    hi_u16 *lut2_b;
    hi_u16 *lut3_r;
    hi_u16 *lut3_g;
    hi_u16 *lut3_b;
    hi_u16 *lut4_r;
    hi_u16 *lut4_g;
    hi_u16 *lut4_b;
    hi_u16 *lut5_r;
    hi_u16 *lut5_g;
    hi_u16 *lut5_b;
    hi_u16 *lut6_r;
    hi_u16 *lut6_g;
    hi_u16 *lut6_b;
    hi_u16 *lut7_r;
    hi_u16 *lut7_g;
    hi_u16 *lut7_b;
} cm_coef;

static hi_u16 g_cm_lut_r_3d0[125 * 2]; /* 125/2: lut length */
static hi_u16 g_cm_lut_r_3d1[100 * 2]; /* 100/2: lut length */
static hi_u16 g_cm_lut_r_3d2[100 * 2]; /* 100/2: lut length */
static hi_u16 g_cm_lut_r_3d3[80 * 2]; /* 80/2: lut length */
static hi_u16 g_cm_lut_r_3d4[100 * 2]; /* 100/2: lut length */
static hi_u16 g_cm_lut_r_3d5[80 * 2]; /* 80/2: lut length */
static hi_u16 g_cm_lut_r_3d6[80 * 2]; /* 80/2: lut length */
static hi_u16 g_cm_lut_r_3d7[64 * 2]; /* 64/2: lut length */

static hi_u16 g_cm_lut_g_3d0[125 * 2]; /* 125/2: lut length */
static hi_u16 g_cm_lut_g_3d1[100 * 2]; /* 100/2: lut length */
static hi_u16 g_cm_lut_g_3d2[100 * 2]; /* 100/2: lut length */
static hi_u16 g_cm_lut_g_3d3[80 * 2]; /* 80/2: lut length */
static hi_u16 g_cm_lut_g_3d4[100 * 2]; /* 100/2: lut length */
static hi_u16 g_cm_lut_g_3d5[80 * 2]; /* 80/2: lut length */
static hi_u16 g_cm_lut_g_3d6[80 * 2]; /* 80/2: lut length */
static hi_u16 g_cm_lut_g_3d7[64 * 2]; /* 64/2: lut length */

static hi_u16 g_cm_lut_b_3d0[125 * 2]; /* 125/2: lut length */
static hi_u16 g_cm_lut_b_3d1[100 * 2]; /* 100/2: lut length */
static hi_u16 g_cm_lut_b_3d2[100 * 2]; /* 100/2: lut length */
static hi_u16 g_cm_lut_b_3d3[80 * 2]; /* 80/2: lut length */
static hi_u16 g_cm_lut_b_3d4[100 * 2]; /* 100/2: lut length */
static hi_u16 g_cm_lut_b_3d5[80 * 2]; /* 80/2: lut length */
static hi_u16 g_cm_lut_b_3d6[80 * 2]; /* 80/2: lut length */
static hi_u16 g_cm_lut_b_3d7[64 * 2]; /* 64/2: lut length */

hi_u16 *g_lut00, *g_lut01, *g_lut1, *g_lut2, *g_lut3, *g_lut4, *g_lut5, *g_lut6, *g_lut7;

static hi_u16 g_lut00_r[64 * 2]; /* 64/2: lut length */
static hi_u16 g_lut01_r[64 * 2]; /* 64/2: lut length */
static hi_u16 g_lut1_r[100 * 2]; /* 100/2: lut length */
static hi_u16 g_lut2_r[100 * 2]; /* 100/2: lut length */
static hi_u16 g_lut3_r[80 * 2]; /* 80 /2: lut length */
static hi_u16 g_lut4_r[100 * 2]; /* 100/2: lut length */
static hi_u16 g_lut5_r[80 * 2]; /* 80/2: lut length */
static hi_u16 g_lut6_r[80 * 2]; /* 80/2: lut length */
static hi_u16 g_lut7_r[64 * 2]; /* 64/2: lut length */

static hi_u16 g_lut00_g[64 * 2]; /* 64/2: lut length */
static hi_u16 g_lut01_g[64 * 2]; /* 64/2: lut length */
static hi_u16 g_lut1_g[100 * 2]; /* 100/2: lut length */
static hi_u16 g_lut2_g[100 * 2]; /* 100/2: lut length */
static hi_u16 g_lut3_g[80 * 2]; /* 80/2: lut length */
static hi_u16 g_lut4_g[100 * 2]; /* 100/2: lut length */
static hi_u16 g_lut5_g[80 * 2]; /* 80/2: lut length */
static hi_u16 g_lut6_g[80 * 2]; /* 80/2: lut length */
static hi_u16 g_lut7_g[64 * 2]; /* 64/2: lut length */

static hi_u16 g_lut00_b[64 * 2]; /* 64/2: lut length */
static hi_u16 g_lut01_b[64 * 2]; /* 64/2: lut length */
static hi_u16 g_lut1_b[100 * 2]; /* 100/2: lut length */
static hi_u16 g_lut2_b[100 * 2]; /* 100/2: lut length */
static hi_u16 g_lut3_b[80 * 2]; /* 80/2: lut length */
static hi_u16 g_lut4_b[100 * 2]; /* 100/2: lut length */
static hi_u16 g_lut5_b[80 * 2]; /* 80/2: lut length */
static hi_u16 g_lut6_b[80 * 2]; /* 80/2: lut length */
static hi_u16 g_lut7_b[64 * 2]; /* 64/2: lut length */

static hi_u16 g_om_cm_lut_r[OMAP_CM_LUT_9][OMAP_CM_LUT_9][OMAP_CM_LUT_9] = {0};
static hi_u16 g_om_cm_lut_g[OMAP_CM_LUT_9][OMAP_CM_LUT_9][OMAP_CM_LUT_9] = {0};
static hi_u16 g_om_cm_lut_b[OMAP_CM_LUT_9][OMAP_CM_LUT_9][OMAP_CM_LUT_9] = {0};
#define LUT_SIZE 9
static hi_u16 g_imap_cacm_lut_r[LUT_SIZE][LUT_SIZE][LUT_SIZE] = {0};
static hi_u16 g_imap_cacm_lut_g[LUT_SIZE][LUT_SIZE][LUT_SIZE] = {0};
static hi_u16 g_imap_cacm_lut_b[LUT_SIZE][LUT_SIZE][LUT_SIZE] = {0};

static hi_u32 g_omap_depq1024_lut[DEF_PQ2L_LUT_SIZE_1024] = {0};
static hi_u32 g_omap_l2p_qlut_x[DEF_L2PQ_LUT_NODES] = {0};
static hi_u32 g_omap_l2pq_lut_a[DEF_L2PQ_LUT_NODES] = {0};
static hi_u32 g_omap_l2pq_lut_b[DEF_L2PQ_LUT_NODES] = {0};

static hi_u32 g_vhdr_cm_lut_new[32] = {0}; /* 32 : hdrv1 cm lut length */

static hi_u32 g_cm_lut_hdr2sdr[32] = {  /* 32 : hdrv1 cm lut length */
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 253, 248, 241, 233, 224, 205,
    177, 144, 110, 93, 90, 90, 90, 90
};

static hi_u32 g_cm_lut_linear[32] = {  /* 32 : hdrv1 cm lut length */
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
};

static hi_u32 g_dbhdr_gamm_l2pq_lut_x[DEF_L2PQ_LUT_NODES];
static hi_u32 g_dbhdr_gamm_l2pq_lut_a[DEF_L2PQ_LUT_NODES];
static hi_u32 g_dbhdr_gamm_l2pq_lut_b[DEF_L2PQ_LUT_NODES];
static hi_u32 g_db_hdr_degamm_pq_new[IMAP_DEGAMMA_1024];

static hi_s32 g_pq_hdr_tmapv2_tm_lut_i_new[TMAP_TM_LUT_512];
static hi_s32 g_pq_hdr_tmapv2_tm_lut_s_new[TMAP_TM_LUT_512];
static hi_s32 g_pq_hdr_tmapv2_sm_lut_i_new[TMAP_SM_LUT_512];
static hi_s32 g_pq_hdr_tmapv2_sm_lut_s_new[TMAP_SM_LUT_512];
static hi_s32 g_pq_hdr_tmapv2_sm_lut_c_new[TMAP_SMC_LUT_512];

/************************imap cacm param*****************************/
hi_u16 g_hdr_coef_cm_lutr3d0[125]; /* 125: lut length */
hi_u16 g_hdr_coef_cm_lutr3d1[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lutr3d2[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lutr3d3[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lutr3d4[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lutr3d5[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lutr3d6[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lutr3d7[64]; /* 64: lut length */

hi_u16 g_hdr_coef_cm_lutg3d0[125]; /* 125: lut length */
hi_u16 g_hdr_coef_cm_lutg3d1[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lutg3d2[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lutg3d3[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lutg3d4[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lutg3d5[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lutg3d6[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lutg3d7[64]; /* 64: lut length */

hi_u16 g_hdr_coef_cm_lutb3d0[125]; /* 125: lut length */
hi_u16 g_hdr_coef_cm_lutb3d1[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lutb3d2[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lutb3d3[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lutb3d4[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lutb3d5[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lutb3d6[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lutb3d7[64]; /* 64: lut length */

#define LUT_EVE_SIZE 5
#define LUT_ODD_SIZE 4

hi_u16 g_hdr_coef_cm_lut00_r[64]; /* 64: lut length */
hi_u16 g_hdr_coef_cm_lut01_r[64]; /* 64: lut length */
hi_u16 g_hdr_coef_cm_lut1_r[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lut2_r[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lut3_r[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lut4_r[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lut5_r[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lut6_r[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lut7_r[64]; /* 64: lut length */

hi_u16 g_hdr_coef_cm_lut00_g[64]; /* 64: lut length */
hi_u16 g_hdr_coef_cm_lut01_g[64]; /* 64: lut length */
hi_u16 g_hdr_coef_cm_lut1_g[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lut2_g[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lut3_g[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lut4_g[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lut5_g[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lut6_g[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lut7_g[64]; /* 64: lut length */

hi_u16 g_hdr_coef_cm_lut00_b[64]; /* 64: lut length */
hi_u16 g_hdr_coef_cm_lut01_b[64]; /* 64: lut length */
hi_u16 g_hdr_coef_cm_lut1_b[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lut2_b[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lut3_b[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lut4_b[100]; /* 100: lut length */
hi_u16 g_hdr_coef_cm_lut5_b[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lut6_b[80]; /* 80: lut length */
hi_u16 g_hdr_coef_cm_lut7_b[64]; /* 64: lut length */
/*****************************************************************/
hi_s32 pq_hal_set_hdr_tm_curve(hi_pq_hdr_offset *hdr_offset_para)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_offset_para);

    g_hdr_mode[hdr_offset_para->hdr_process_scene] = hdr_offset_para->hdr_mode;
    g_dci_dark_str[hdr_offset_para->hdr_process_scene] = hdr_offset_para->ac_cdark;
    g_dci_bright_str[hdr_offset_para->hdr_process_scene] = hdr_offset_para->ac_cbrigt;
    g_dark_cv_str[hdr_offset_para->hdr_process_scene] = hdr_offset_para->dark_cv;
    g_bright_cv_str[hdr_offset_para->hdr_process_scene] = hdr_offset_para->bright_cv;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_hdr_csc_setting(hi_pq_image_param *pic_setting)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(pic_setting);

    g_bright = (hi_u32)pic_setting->brightness;
    g_contrast = (hi_u32)pic_setting->contrast;
    g_satu = (hi_u32)pic_setting->hue;
    g_hue = (hi_u32)pic_setting->saturation;
    g_wcg_temperature = (hi_u32)pic_setting->wcg_temperature;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_hdr_offset(hi_pq_hdr_offset *hdr_offset_para)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_offset_para);

    g_hdr_offset_bright[hdr_offset_para->hdr_process_scene] = hdr_offset_para->bright;
    g_hdr_offset_contrast[hdr_offset_para->hdr_process_scene] = hdr_offset_para->contrast;
    g_hdr_offset_satu[hdr_offset_para->hdr_process_scene] = hdr_offset_para->satu;
    g_hdr_offset_hue[hdr_offset_para->hdr_process_scene] = hdr_offset_para->hue;
    g_hdr_offset_red[hdr_offset_para->hdr_process_scene] = hdr_offset_para->r;
    g_hdr_offset_green[hdr_offset_para->hdr_process_scene] = hdr_offset_para->g;
    g_hdr_offset_blue[hdr_offset_para->hdr_process_scene] = hdr_offset_para->b;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_hdrdci_histgram(hi_u32 dci_out_width, hi_u32 dci_out_height, hi_pq_dci_histgram *dci_histgram)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(dci_histgram);

    g_dci_out_width = dci_out_width;
    g_dci_out_height = dci_out_height;

    memcpy(&g_gs_st_dci_histgram, dci_histgram, sizeof(g_gs_st_dci_histgram));

    return HI_SUCCESS;
}

static hi_void pq_hal_get_yuv2rgb_imap_matrix(hi_drv_color_descript in_cs, hi_drv_pq_imap_cfg *imap_cfg)
{
    imap_cfg->yuv2rgb.scale2p = g_m33_yuv2rgb_scale2p - 2; /* 11 : scale2p */
    if (in_cs.color_space == HI_DRV_COLOR_CS_YUV) { /* yuv */
        if (in_cs.quantify_range == HI_DRV_COLOR_LIMITED_RANGE) { /* yuv-limit */
            if (in_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                in_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-yuv-limt */
                memcpy(imap_cfg->yuv2rgb.coef, g_m33_yuv2rgb_bt601_yl2bt709_rf, sizeof(imap_cfg->yuv2rgb.coef));
                memcpy(imap_cfg->yuv2rgb.offinrgb,
                       g_v3_yuv2rgb_off_in_rgb_bt601_yl2bt709_rf, sizeof(imap_cfg->yuv2rgb.offinrgb));
            } else if (in_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT709) {
                /* 709-yuv-limt  */
                memcpy(imap_cfg->yuv2rgb.coef, g_m33_yuv2rgb_bt709_yl2rf_2, sizeof(imap_cfg->yuv2rgb.coef));
                memcpy(imap_cfg->yuv2rgb.offinrgb,
                       g_v3_yuv2rgb_off_in_rgb_bt709_yl2rf, sizeof(imap_cfg->yuv2rgb.offinrgb));
            } else if (in_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT) {
                /* 2020-yuv-limt  */
                memcpy(imap_cfg->yuv2rgb.coef, g_m33_yuv2rgb_bt2020_yl2rf_2, sizeof(imap_cfg->yuv2rgb.coef));
                memcpy(imap_cfg->yuv2rgb.offinrgb,
                       g_v3_yuv2rgb_off_in_rgb_bt2020_yl2rf, sizeof(imap_cfg->yuv2rgb.offinrgb));
            }
        } else { /* yuv-full */
            if (in_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                in_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-yuv-full */
                memcpy(imap_cfg->yuv2rgb.coef, g_m33_yuv2rgb_bt601_yf2bt709_rf, sizeof(imap_cfg->yuv2rgb.coef));
                memcpy(imap_cfg->yuv2rgb.offinrgb,
                       g_v3_yuv2rgb_off_in_rgb_bt601_yf2bt709_rf, sizeof(imap_cfg->yuv2rgb.offinrgb));
            } else if (in_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT709) {
                /* 709-yuv-full */
                memcpy(imap_cfg->yuv2rgb.coef, g_m33_yuv2rgb_bt709_yf2rf, sizeof(imap_cfg->yuv2rgb.coef));
                memcpy(imap_cfg->yuv2rgb.offinrgb,
                       g_v3_yuv2rgb_off_in_rgb_bt709_yf2rf, sizeof(imap_cfg->yuv2rgb.offinrgb));
            } else if (in_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT) {
                /* 2020-yuv-full */
                memcpy(imap_cfg->yuv2rgb.coef, g_m33_yuv2rgb_bt2020_yf2rf, sizeof(imap_cfg->yuv2rgb.coef));
                memcpy(imap_cfg->yuv2rgb.offinrgb,
                       g_v3_yuv2rgb_off_in_rgb_bt2020_yf2rf, sizeof(imap_cfg->yuv2rgb.offinrgb));
            }
        }
    } else { /* rgb */
        if (in_cs.quantify_range == HI_DRV_COLOR_LIMITED_RANGE) { /* rgb-limit */
            if (in_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                in_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-rgb-limit */
                memcpy(imap_cfg->yuv2rgb.coef, g_m33_yuv2rgb_bt601rl2bt709rf, sizeof(imap_cfg->yuv2rgb.coef));
                memcpy(imap_cfg->yuv2rgb.offinrgb,
                       g_v3_yuv2rgb_offinrgb_bt601rl2bt709rf, sizeof(imap_cfg->yuv2rgb.offinrgb));
            } else { /* 709/2020-rgb-limit */
                memcpy(imap_cfg->yuv2rgb.coef, g_m33_yuv2rgb_rl2rf, sizeof(imap_cfg->yuv2rgb.coef));
                memcpy(imap_cfg->yuv2rgb.offinrgb,
                       g_v3_yuv2rgb_offinrgb_rl2rf, sizeof(imap_cfg->yuv2rgb.offinrgb));
            }
        } else { /* rgb-full */
            if (in_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                in_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-rgb-full */
                memcpy(imap_cfg->yuv2rgb.coef, g_m33_yuv2rgb_bt601rf2bt709rf, sizeof(imap_cfg->yuv2rgb.coef));
                memcpy(imap_cfg->yuv2rgb.offinrgb,
                       g_v3_yuv2rgb_offinrgb_zero, sizeof(imap_cfg->yuv2rgb.offinrgb));
            } else { /* 709/2020-rgb-full */
                imap_cfg->yuv2rgb.enable = HI_FALSE;
                memcpy(imap_cfg->yuv2rgb.coef, g_m33_yuv2rgb_linear, sizeof(imap_cfg->yuv2rgb.coef));
                memcpy(imap_cfg->yuv2rgb.offinrgb,
                       g_v3_yuv2rgb_offinrgb_zero, sizeof(imap_cfg->yuv2rgb.offinrgb));
            }
        }
    }
}

static hi_s32 pq_hal_get_yuv2rgb_matrix(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
                                        hi_drv_pq_imap_cfg *imap_cfg)
{
    imap_cfg->yuv2rgb.mode = 3; /* 3 : mode,BT2020CL should set 2 */
    imap_cfg->yuv2rgb.clip_max = 134215680; /* 134215680 : clip_max */
    imap_cfg->yuv2rgb.clip_min = 0;
    imap_cfg->yuv2rgb.thr_r = 0;
    imap_cfg->yuv2rgb.thr_b = 0;

    /* BT2020CL config, hi_drv_bt2020_luminace_sys_type should add BT2020CL frame info */
    memcpy(imap_cfg->yuv2rgb.dc_in, g_hdr2sdr_imap_y2r_dc_in, sizeof(imap_cfg->yuv2rgb.dc_in));

    memcpy(imap_cfg->yuv2rgb.dc_in2, g_hdr2sdr_imap_y2r_dc_in2, sizeof(imap_cfg->yuv2rgb.dc_in2));
    memcpy(imap_cfg->yuv2rgb.offinrgb2, g_hdr2sdr_imap_y2r_offinrgb2, sizeof(imap_cfg->yuv2rgb.offinrgb2));
    memcpy(imap_cfg->yuv2rgb.coef2, g_hdr2sdr_imap_y2r_coef2, sizeof(imap_cfg->yuv2rgb.coef2));

    pq_hal_get_yuv2rgb_imap_matrix(in_cs, imap_cfg);

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_imap_cacm_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
                                       hi_drv_pq_imap_cfg *imap_cfg)
{
    /* module on/off according 709/2020, pos according HDR/SDR */
    imap_cfg->cacm.demo_pos = 0;
    imap_cfg->cacm.demo_mode = 0;
    imap_cfg->cacm.bitdepth_out_mode = 3;  /* 3: bitdepth_out_mode */
    imap_cfg->cacm.bitdepth_in_mode = 3; /* 3: bitdepth_in_mode */

    switch (g_sg_en_hdr_mode) {
        case HI_PQ_HDR_MODE_HDR10_TO_SDR:
            imap_cfg->cacm.enable = HI_FALSE;
            break;
        case HI_PQ_HDR_MODE_SDR_TO_HDR10:
            memcpy(g_imap_cacm_lut_r, g_sdr2hdr_imap_cacm_lut_r, sizeof(g_imap_cacm_lut_r));
            memcpy(g_imap_cacm_lut_g, g_sdr2hdr_imap_cacm_lut_g, sizeof(g_imap_cacm_lut_g));
            memcpy(g_imap_cacm_lut_b, g_sdr2hdr_imap_cacm_lut_b, sizeof(g_imap_cacm_lut_b));
            break;
        default:
            imap_cfg->cacm.enable = HI_FALSE;
            break;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_imap_cacm_hdrv1_mode_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
    hi_drv_pq_imap_cfg *imap_cfg)
{
    /* module on/off according 709/2020, pos according HDR/SDR */
    imap_cfg->cacm.demo_pos = 0;
    imap_cfg->cacm.demo_mode = 0;
    imap_cfg->cacm.bitdepth_out_mode = 3; /* 3: bitdepth_out_mode */
    imap_cfg->cacm.bitdepth_in_mode = 3;  /* 3: bitdepth_in_mode */

    switch (g_sg_en_hdr_mode) {
        case HI_PQ_HDR_MODE_HLG_TO_SDR:
            memcpy(g_imap_cacm_lut_r, g_hdr2sdr_omap_cm_3dlut_r, sizeof(g_imap_cacm_lut_r));
            memcpy(g_imap_cacm_lut_g, g_hdr2sdr_omap_cm_3dlut_g, sizeof(g_imap_cacm_lut_g));
            memcpy(g_imap_cacm_lut_b, g_hdr2sdr_omap_cm_3dlut_b, sizeof(g_imap_cacm_lut_b));
            break;
        case HI_PQ_HDR_MODE_SDR_TO_HLG:
            memcpy(g_imap_cacm_lut_r, g_sdr2hdr_imap_cacm_lut_r, sizeof(g_imap_cacm_lut_r));
            memcpy(g_imap_cacm_lut_g, g_sdr2hdr_imap_cacm_lut_g, sizeof(g_imap_cacm_lut_g));
            memcpy(g_imap_cacm_lut_b, g_sdr2hdr_imap_cacm_lut_b, sizeof(g_imap_cacm_lut_b));
            break;
        default:
            imap_cfg->cacm.enable = HI_FALSE;
            break;
    }

    return HI_SUCCESS;
}

static hi_void pq_hal_get_rgb2lms_matrix_hdrv1_mode(hi_drv_color_descript in_cs,
    hi_drv_color_descript out_cs, hi_drv_pq_imap_cfg *imap_cfg)
{
    /* hdrv1_mode : RGB2RGB */
    imap_cfg->rgb2lms.scale2p = g_map_scale; /* 14 : scale2p */
    imap_cfg->rgb2lms.clip_min = 0;
    imap_cfg->rgb2lms.clip_max = 2621440000; /* 2621440000 is max clip */
    memcpy(imap_cfg->rgb2lms.dc_in, g_hdr2sdr_imap_r2l_dc_in, sizeof(imap_cfg->rgb2lms.dc_in));

    if ((in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT2020) &&
        (out_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT709)) {
        /* in:bt 2020, out :bt 709  */
        memcpy(imap_cfg->rgb2lms.coef, g_map_m33_2020_ncl_709, sizeof(imap_cfg->rgb2lms.coef));
    } else if ((in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT709) &&
               (out_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT2020)) {
        /* in:bt 709, out :bt 2020  */
        memcpy(imap_cfg->rgb2lms.coef, g_map_m33_709_2020_ncl, sizeof(imap_cfg->rgb2lms.coef));
    } else if ((in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT601_525 ||
                in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT601_625) &&
               (out_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT2020)) {
        /* in:bt 601, out :bt 2020  */
        memcpy(imap_cfg->rgb2lms.coef, g_map_m33_709_2020_ncl, sizeof(imap_cfg->rgb2lms.coef));
    } else if ((in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT2020) &&
               (out_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT601_525 ||
                out_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT601_625)) {
        /* in:bt 2020, out :bt 601  */
        memcpy(imap_cfg->rgb2lms.coef, g_map_m33_2020_ncl_709, sizeof(imap_cfg->rgb2lms.coef));
    } else {
        memcpy(imap_cfg->rgb2lms.coef, g_map_m33_linear, sizeof(imap_cfg->rgb2lms.coef));
    }

    if (imap_cfg->cacm.enable == HI_TRUE) {
        memcpy(imap_cfg->rgb2lms.coef, g_map_m33_linear, sizeof(imap_cfg->rgb2lms.coef));
    }

    return;
}

static hi_s32 pq_hal_get_rgb2lms_matrix(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
                                        hi_drv_pq_imap_cfg *imap_cfg)
{
    /* only support HDR2SDR and SDR2HDR */
    imap_cfg->rgb2lms.scale2p = 14; /* 14 : scale2p */
    imap_cfg->rgb2lms.clip_min = 0;

    memcpy(imap_cfg->rgb2lms.dc_in, g_hdr2sdr_imap_r2l_dc_in, sizeof(imap_cfg->rgb2lms.dc_in));

    switch (g_sg_en_hdr_mode) {
        case HI_PQ_HDR_MODE_HDR10_TO_SDR:
            imap_cfg->rgb2lms.clip_max = 2621440000; /* 2621440000 : clip_max */
            if (in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT709) {
                /* 输入709 */
                memcpy(imap_cfg->rgb2lms.coef, g_rgb2lms_709, sizeof(imap_cfg->rgb2lms.coef));
            } else if (in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT2020) {
                /* 输入2020 */
                memcpy(imap_cfg->rgb2lms.coef, g_rgb2lms_2020, sizeof(imap_cfg->rgb2lms.coef));
            } else if (in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT601_525 ||
                       in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT601_625) {
                memcpy(imap_cfg->rgb2lms.coef, g_rgb2lms_709, sizeof(imap_cfg->rgb2lms.coef));
            } else {
                memcpy(imap_cfg->rgb2lms.coef, g_rgb2lms_2020, sizeof(imap_cfg->rgb2lms.coef));
            }
            break;
        case HI_PQ_HDR_MODE_SDR_TO_HDR10:
            imap_cfg->rgb2lms.clip_max = 26214400; /* 26214400 : clip_max */
            if (imap_cfg->cacm.enable == HI_TRUE) {
                memcpy(imap_cfg->rgb2lms.coef, g_rgb2lms_2020, sizeof(imap_cfg->rgb2lms.coef));
            } else {
                if (in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT709) {
                    memcpy(imap_cfg->rgb2lms.coef, g_rgb2lms_709, sizeof(imap_cfg->rgb2lms.coef));
                } else if (in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT2020) {
                    memcpy(imap_cfg->rgb2lms.coef, g_rgb2lms_2020, sizeof(imap_cfg->rgb2lms.coef));
                } else if (in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT601_525 ||
                           in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT601_625) {
                    memcpy(imap_cfg->rgb2lms.coef, g_rgb2lms_709, sizeof(imap_cfg->rgb2lms.coef));
                } else {
                    memcpy(imap_cfg->rgb2lms.coef, g_rgb2lms_709, sizeof(imap_cfg->rgb2lms.coef));
                }
            }
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

static hi_void pq_hal_get_tmap_v1_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
                                      hi_drv_pq_imap_cfg *imap_cfg)
{
    imap_cfg->tmap_v1.scale_mix_alpha = g_tm_v1_scale_mix_alpha;
    imap_cfg->tmap_v1.mix_alpha = 0;
    imap_cfg->tmap_v1.clip_min = g_tm_v1_clip_min;
    imap_cfg->tmap_v1.clip_max = g_tm_v1_clip_max;
    imap_cfg->tmap_v1.scale_lum_cal = g_tm_v1_scale_lum_cal;
    imap_cfg->tmap_v1.scale_coef = g_tm_v1_scale_coef;

    memcpy(imap_cfg->tmap_v1.step, g_tm_v1_x_step, sizeof(imap_cfg->tmap_v1.step));
    memcpy(imap_cfg->tmap_v1.pos, g_tm_v1_x_pos, sizeof(imap_cfg->tmap_v1.pos));
    memcpy(imap_cfg->tmap_v1.num, g_tm_v1_x_num, sizeof(imap_cfg->tmap_v1.num));
    memcpy(imap_cfg->tmap_v1.lum_cal, g_tm_v1_m3_lum_cal_2020, sizeof(imap_cfg->tmap_v1.lum_cal));
    memcpy(imap_cfg->tmap_v1.dc_out, g_tm_v1_dc_out_zero, sizeof(imap_cfg->tmap_v1.dc_out));

    switch (g_sg_en_hdr_mode) {
        case HI_PQ_HDR_MODE_HDR10_TO_HLG:
            imap_cfg->tmap_v1.mix_alpha = 0;
            imap_cfg->tmap_v1.scale_coef = g_tm_v1_scale_coef_pq2hlg;
            g_tmap_hlg_lut_tmapv1 = g_tm_v1_lut_pq2hlg;
            break;
        case HI_PQ_HDR_MODE_HLG_TO_SDR:
            imap_cfg->tmap_v1.mix_alpha = 0;
            imap_cfg->tmap_v1.scale_coef = g_tm_v1_scale_coef_hlg2sdr;
            g_tmap_hlg_lut_tmapv1 = g_tm_v1_lut_hlg2sdr;
            break;
        case HI_PQ_HDR_MODE_HLG_TO_HDR10:
            imap_cfg->tmap_v1.mix_alpha = 0;
            imap_cfg->tmap_v1.scale_coef = g_tm_v1_scale_coef_hlg2pq;
            g_tmap_hlg_lut_tmapv1 = g_tm_v1_lut_hlg2pq;
            break;
        case HI_PQ_HDR_MODE_SDR_TO_HLG:
            imap_cfg->tmap_v1.mix_alpha = 0;
            imap_cfg->tmap_v1.scale_coef = g_tm_v1_scale_coef_sdr2hlg;
            g_tmap_hlg_lut_tmapv1 = g_tm_v1_lut_sdr2hlg;
            break;
        default:
            imap_cfg->tmap_v1.enable = HI_FALSE;
            imap_cfg->tmap_v1.mix_alpha = 0;
            imap_cfg->tmap_v1.scale_coef = g_tm_v1_scale_coef_linear;
            g_tmap_hlg_lut_tmapv1 = g_tm_v1_lut_linear;
            break;
    }

    return;
}

static hi_void pq_hal_get_lms2ipt_matrix_hdrv1_mode(hi_drv_color_descript in_cs,
    hi_drv_color_descript out_cs, hi_drv_pq_imap_cfg *imap_cfg)
{
    imap_cfg->lms2ipt.scale2p = g_m33_rgb2yuv_scale2p; /* 12: lms2ipt scale */
    imap_cfg->lms2ipt.clip_min_y = -32767;         /* -32767: lms2ipt clip */
    imap_cfg->lms2ipt.clip_max_y = 32767;          /* 32767: lms2ipt scale */
    imap_cfg->lms2ipt.clip_min_c = -32767;         /* -32767: lms2ipt clip */
    imap_cfg->lms2ipt.clip_max_c = 32767;          /* 32767: lms2ipt scale */

    memcpy(imap_cfg->lms2ipt.coef, g_m33_rgb2yuv_linear, sizeof(imap_cfg->lms2ipt.coef));
    memcpy(imap_cfg->lms2ipt.dc_in, g_v3_rgb2yuv_off_dc_zero, sizeof(imap_cfg->lms2ipt.dc_in));
    memcpy(imap_cfg->lms2ipt.dc_out, g_v3_rgb2yuv_off_dc_zero, sizeof(imap_cfg->lms2ipt.dc_out));

    if (out_cs.color_space == HI_DRV_COLOR_CS_YUV) { /* yuv */
        if (out_cs.quantify_range == HI_DRV_COLOR_LIMITED_RANGE) { /* yuv-limit */
            if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-yuv-limt */
                memcpy(imap_cfg->lms2ipt.coef, g_m33_rgb2yuv_bt709rf2bt601yl, sizeof(imap_cfg->lms2ipt.coef));
            } else if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT709) {
                /* 709-yuv-limt  */
                memcpy(imap_cfg->lms2ipt.coef, g_m33_rgb2yuv_bt709_rf2yl_2, sizeof(imap_cfg->lms2ipt.coef));
            } else if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT) {
                /* 2020-yuv-limt  */
                memcpy(imap_cfg->lms2ipt.coef, g_m33_rgb2yuv_bt2020_rf2yl_2, sizeof(imap_cfg->lms2ipt.coef));
            }
        } else { /* yuv-full */
            if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-yuv-full */
                memcpy(imap_cfg->lms2ipt.coef, g_m33_rgb2yuv_bt709rf2bt601yf, sizeof(imap_cfg->lms2ipt.coef));
            } else if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT709) {
                /* 709-yuv-full */
                memcpy(imap_cfg->lms2ipt.coef, g_m33_rgb2yuv_bt709rf2yf, sizeof(imap_cfg->lms2ipt.coef));
            } else if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT) {
                /* 2020-yuv-full */
                memcpy(imap_cfg->lms2ipt.coef, g_m33_rgb2yuv_bt2020rf2yf, sizeof(imap_cfg->lms2ipt.coef));
            }
        }
    } else { /* rgb */
        if (out_cs.quantify_range == HI_DRV_COLOR_LIMITED_RANGE) { /* rgb-limit */
            if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-rgb-limit */
                memcpy(imap_cfg->lms2ipt.coef, g_m33_rgb2yuv_bt709rf2bt601rl, sizeof(imap_cfg->lms2ipt.coef));
            } else {
                /* 709/2020-rgb-limit */
                memcpy(imap_cfg->lms2ipt.coef, g_m33_rgb2yuv_rf2rl, sizeof(imap_cfg->lms2ipt.coef));
            }
        } else { /* rgb-full */
            if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-rgb-full */
                memcpy(imap_cfg->lms2ipt.coef, g_m33_rgb2yuv_bt709rf2bt601rf, sizeof(imap_cfg->lms2ipt.coef));
            } else {
                /* 709/2020-rgb-full */
                imap_cfg->lms2ipt.enable = HI_FALSE;
                memcpy(imap_cfg->lms2ipt.coef, g_m33_rgb2yuv_rf2rf, sizeof(imap_cfg->lms2ipt.coef));
            }
        }
    }

    return;
}

static hi_s32 pq_hal_get_lms2ipt_matrix(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
                                        hi_drv_pq_imap_cfg *imap_cfg)
{
    imap_cfg->lms2ipt.scale2p = 12; /* 12: lms2ipt scale */
    imap_cfg->lms2ipt.clip_min_y = -32767; /* -32767: lms2ipt clip */
    imap_cfg->lms2ipt.clip_max_y = 32767; /* 32767: lms2ipt scale */
    imap_cfg->lms2ipt.clip_min_c = -32767; /* -32767: lms2ipt clip */
    imap_cfg->lms2ipt.clip_max_c = 32767; /* 32767: lms2ipt scale */

    /* 当前适用默认值，后续需要看到HDR转换类型 */
    memcpy(imap_cfg->lms2ipt.coef, g_hdr2sdr_imap_l2i_coef, sizeof(imap_cfg->lms2ipt.coef));
    memcpy(imap_cfg->lms2ipt.dc_in, g_hdr2sdr_imap_l2i_dc_in, sizeof(imap_cfg->lms2ipt.dc_in));
    memcpy(imap_cfg->lms2ipt.dc_out, g_hdr2sdr_imap_l2i_dc_out, sizeof(imap_cfg->lms2ipt.dc_out));

    switch (g_sg_en_hdr_mode) {
        case HI_PQ_HDR_MODE_HDR10_TO_SDR:
            memcpy(imap_cfg->lms2ipt.coef, g_hdr2sdr_imap_l2i_coef, sizeof(imap_cfg->lms2ipt.coef));
            memcpy(imap_cfg->lms2ipt.dc_in, g_hdr2sdr_imap_l2i_dc_in, sizeof(imap_cfg->lms2ipt.dc_in));
            memcpy(imap_cfg->lms2ipt.dc_out, g_hdr2sdr_imap_l2i_dc_out, sizeof(imap_cfg->lms2ipt.dc_out));
            break;
        case HI_PQ_HDR_MODE_SDR_TO_HDR10:
            memcpy(imap_cfg->lms2ipt.coef, g_hdr2sdr_imap_l2i_coef, sizeof(imap_cfg->lms2ipt.coef));
            memcpy(imap_cfg->lms2ipt.dc_in, g_hdr2sdr_imap_l2i_dc_in, sizeof(imap_cfg->lms2ipt.dc_in));
            memcpy(imap_cfg->lms2ipt.dc_out, g_hdr2sdr_imap_l2i_dc_out, sizeof(imap_cfg->lms2ipt.dc_out));
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_ipt2lms_matrix(hi_drv_color_descript in_cs,
                                        hi_drv_color_descript out_cs, pq_omap_cfg *omap_cfg)
{
    /* only support HDR2SDR and SDR2HDR */
    omap_cfg->ipt2lms.min = 0;
    omap_cfg->ipt2lms.max = 65535; /* 65535: max */
    omap_cfg->ipt2lms.scale2p = 15; /* 15: scale2p */
    /* 先配置默认值，omap的csc和imap的是对等的，参考做，
    如果在imap里面只跟输入相关，则在omap里面只跟输出相关 */
    memcpy(omap_cfg->ipt2lms.coef, g_hdr2sdr_omap_i2l_coef, sizeof(omap_cfg->ipt2lms.coef));
    memcpy(omap_cfg->ipt2lms.in_dc, g_hdr2sdr_omap_i2l_dc_in, sizeof(omap_cfg->ipt2lms.in_dc));
    memcpy(omap_cfg->ipt2lms.out_dc, g_hdr2sdr_omap_i2l_dc_out, sizeof(omap_cfg->ipt2lms.out_dc));

    switch (g_sg_en_hdr_mode) {
        case HI_PQ_HDR_MODE_HDR10_TO_SDR:
            memcpy(omap_cfg->ipt2lms.coef, g_hdr2sdr_omap_i2l_coef, sizeof(omap_cfg->ipt2lms.coef));
            memcpy(omap_cfg->ipt2lms.in_dc, g_hdr2sdr_omap_i2l_dc_in, sizeof(omap_cfg->ipt2lms.in_dc));
            memcpy(omap_cfg->ipt2lms.out_dc, g_hdr2sdr_omap_i2l_dc_out, sizeof(omap_cfg->ipt2lms.out_dc));
            break;
        case HI_PQ_HDR_MODE_SDR_TO_HDR10:
            memcpy(omap_cfg->ipt2lms.coef, g_hdr2sdr_omap_i2l_coef, sizeof(omap_cfg->ipt2lms.coef));
            memcpy(omap_cfg->ipt2lms.in_dc, g_hdr2sdr_omap_i2l_dc_in, sizeof(omap_cfg->ipt2lms.in_dc));
            memcpy(omap_cfg->ipt2lms.out_dc, g_hdr2sdr_omap_i2l_dc_out, sizeof(omap_cfg->ipt2lms.out_dc));
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_ipt2lms_matrix_hdrv1_mode(hi_drv_color_descript in_cs,
    hi_drv_color_descript out_cs, pq_omap_cfg *omap_cfg)
{
    /* YUV2RGB */
    omap_cfg->ipt2lms.min = 0;
    omap_cfg->ipt2lms.max = 65535;                 /* 65535: max */
    omap_cfg->ipt2lms.scale2p = g_m33_yuv2rgb_scale2p; /* 15: scale2p */

    memcpy(omap_cfg->ipt2lms.coef, g_m33_yuv2rgb_bt709_yl2rf, sizeof(omap_cfg->ipt2lms.coef));
    memcpy(omap_cfg->ipt2lms.in_dc, g_hdr2sdr_omap_i2l_dc_in, sizeof(omap_cfg->ipt2lms.in_dc));
    memcpy(omap_cfg->ipt2lms.out_dc, g_hdr2sdr_omap_i2l_dc_out, sizeof(omap_cfg->ipt2lms.out_dc));

    if (out_cs.color_space == HI_DRV_COLOR_CS_YUV) { /* yuv */
        if (out_cs.quantify_range == HI_DRV_COLOR_LIMITED_RANGE) { /* yuv-limit */
            if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-yuv-limt */
            memcpy(omap_cfg->ipt2lms.coef, g_m33_yuv2rgb_bt601yl2bt709rf_s16, sizeof(omap_cfg->ipt2lms.coef));
            } else if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT709) {
                /* 709-yuv-limt  */
                memcpy(omap_cfg->ipt2lms.coef, g_m33_yuv2rgb_bt709_yl2rf, sizeof(omap_cfg->ipt2lms.coef));
            } else if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT) {
                /* 2020-yuv-limt  */
                memcpy(omap_cfg->ipt2lms.coef, g_m33_yuv2rgb_bt2020_yl2rf, sizeof(omap_cfg->ipt2lms.coef));
            }
        } else { /* yuv-full */
            if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-yuv-full */
                memcpy(omap_cfg->ipt2lms.coef, g_m33_yuv2rgb_bt601_yf2bt709_rf, sizeof(omap_cfg->ipt2lms.coef));
            } else if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT709) {
                /* 709-yuv-full */
                memcpy(omap_cfg->ipt2lms.coef, g_m33_yuv2rgb_bt709_yf2rf, sizeof(omap_cfg->ipt2lms.coef));
            } else if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT) {
                /* 2020-yuv-full */
                memcpy(omap_cfg->ipt2lms.coef, g_m33_yuv2rgb_bt2020_yf2rf, sizeof(omap_cfg->ipt2lms.coef));
            }
        }
    } else { /* rgb */
        if (out_cs.quantify_range == HI_DRV_COLOR_LIMITED_RANGE) { /* rgb-limit */
            if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-rgb-limit */
                memcpy(omap_cfg->ipt2lms.coef, g_m33_yuv2rgb_bt601rl2bt709rf_s16, sizeof(omap_cfg->ipt2lms.coef));
            } else { /* 709/2020-rgb-limit */
                memcpy(omap_cfg->ipt2lms.coef, g_m33_yuv2rgb_rl2rf_s16, sizeof(omap_cfg->ipt2lms.coef));
            }
        } else { /* rgb-full */
            if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-rgb-full */
                memcpy(omap_cfg->ipt2lms.coef, g_m33_yuv2rgb_bt601rf2bt709rf_s16, sizeof(omap_cfg->ipt2lms.coef));
            } else { /* 709/2020-rgb-full */
                omap_cfg->ipt2lms.enable = HI_FALSE;
                memcpy(omap_cfg->ipt2lms.coef, g_m33_yuv2rgb_linear_s16, sizeof(omap_cfg->ipt2lms.coef));
            }
        }
    }

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_lms2rgb_matrix(hi_drv_color_descript in_cs,
                                        hi_drv_color_descript out_cs, pq_omap_cfg *omap_cfg)
{
    omap_cfg->lms2rgb.scale2p = g_m33_lms2rgb_scale2p; /* 12: scale2p */
    omap_cfg->lms2rgb.min = 0;
    omap_cfg->lms2rgb.max = 2621440000; /* 2621440000: max */

    switch (g_sg_en_hdr_mode) {
        case HI_PQ_HDR_MODE_HDR10_TO_SDR:
            omap_cfg->lms2rgb.scale2p = g_m33_lms2rgb_scale2p + 2; /* 2 is scale2p */
            if (omap_cfg->cm_cfg.enable == HI_TRUE) {
                memcpy(omap_cfg->lms2rgb.coef, g_lms2rgb_2020, sizeof(omap_cfg->lms2rgb.coef));
            } else {
                if (out_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT2020) {
                    /* out:bt2020 */
                    memcpy(omap_cfg->lms2rgb.coef, g_lms2rgb_2020, sizeof(omap_cfg->lms2rgb.coef));
                } else if (out_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT709) {
                    /* out:bt709 */
                    memcpy(omap_cfg->lms2rgb.coef, g_m33_lms2rgb_709, sizeof(omap_cfg->lms2rgb.coef));
                } else if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                    /* out:bt601 */
                    memcpy(omap_cfg->lms2rgb.coef, g_m33_lms2rgb_709, sizeof(omap_cfg->lms2rgb.coef));
                } else {
                    memcpy(omap_cfg->lms2rgb.coef, g_m33_lms2rgb_709, sizeof(omap_cfg->lms2rgb.coef));
                }
            }
            break;
        case HI_PQ_HDR_MODE_SDR_TO_HDR10:
            if (out_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT2020) {
                /* out:bt2020 */
                memcpy(omap_cfg->lms2rgb.coef, g_lms2rgb_2020, sizeof(omap_cfg->lms2rgb.coef));
            } else if (out_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT709) {
                /* out:bt709 */
                memcpy(omap_cfg->lms2rgb.coef, g_m33_lms2rgb_709, sizeof(omap_cfg->lms2rgb.coef));
            } else if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* out:bt601 */
                memcpy(omap_cfg->lms2rgb.coef, g_m33_lms2rgb_709, sizeof(omap_cfg->lms2rgb.coef));
            } else {
                memcpy(omap_cfg->lms2rgb.coef, g_lms2rgb_2020, sizeof(omap_cfg->lms2rgb.coef));
            }
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_omap_cacm_cfg(hi_drv_color_descript in_cs,
                                       hi_drv_color_descript out_cs, pq_omap_cfg *omap_cfg)
{
    /* oway set 3, mean 16bit in,16bit out */
    omap_cfg->cm_cfg.bitdepth_in_mode = 3; /* 3: bitdepth_in_mode */
    omap_cfg->cm_cfg.bitdepth_out_mode = 3; /* 3: bitdepth_out_mode */

    omap_cfg->cm_cfg.demo_mode = 0;
    omap_cfg->cm_cfg.demo_pos = 0;
    omap_cfg->cm_cfg.cm_pos = 0; /* omap not need it */

    switch (g_sg_en_hdr_mode) {
        case HI_PQ_HDR_MODE_HDR10_TO_SDR:
            memcpy(g_om_cm_lut_r, g_hdr2sdr_omap_cm_3dlut_r, sizeof(g_om_cm_lut_r));
            memcpy(g_om_cm_lut_g, g_hdr2sdr_omap_cm_3dlut_g, sizeof(g_om_cm_lut_g));
            memcpy(g_om_cm_lut_b, g_hdr2sdr_omap_cm_3dlut_b, sizeof(g_om_cm_lut_b));
            break;
        case HI_PQ_HDR_MODE_SDR_TO_HDR10:
            omap_cfg->cm_cfg.enable = HI_FALSE;
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_rgb2yuv_matrix(hi_drv_color_descript in_cs,
                                        hi_drv_color_descript out_cs, pq_omap_cfg *omap_cfg)
{
    omap_cfg->rgb2yuv.min = 0;
    omap_cfg->rgb2yuv.max = 4095; /* 4095: max */
    omap_cfg->rgb2yuv.scale2p = g_m33_rgb2yuv_scale2p + 4; /* 4: scale2p offset */

    memcpy(omap_cfg->rgb2yuv.in_dc, g_hdr2sdr_omap_r2y_dc_in, sizeof(omap_cfg->rgb2yuv.in_dc));

    if (out_cs.color_space == HI_DRV_COLOR_CS_YUV) { /* yuv */
        if (out_cs.quantify_range == HI_DRV_COLOR_LIMITED_RANGE) { /* yuv-limit */
            if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-yuv-limt */
                memcpy(omap_cfg->rgb2yuv.coef, g_m33_rgb2yuv_bt709rf2bt601yl_2, sizeof(omap_cfg->rgb2yuv.coef));
                memcpy(omap_cfg->rgb2yuv.out_dc, g_v3_rgb2yuv_off, sizeof(omap_cfg->rgb2yuv.out_dc));
            } else if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT709) {
                /* 709-yuv-limt  */
                memcpy(omap_cfg->rgb2yuv.coef, g_m33_rgb2yuv_bt709_rf2yl, sizeof(omap_cfg->rgb2yuv.coef));
                memcpy(omap_cfg->rgb2yuv.out_dc, g_v3_rgb2yuv_off, sizeof(omap_cfg->rgb2yuv.out_dc));
            } else if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT) {
                /* 2020-yuv-limt  */
                memcpy(omap_cfg->rgb2yuv.coef, g_m33_rgb2yuv_bt2020_rf2yl, sizeof(omap_cfg->rgb2yuv.coef));
                memcpy(omap_cfg->rgb2yuv.out_dc, g_v3_rgb2yuv_off, sizeof(omap_cfg->rgb2yuv.out_dc));
            }
        } else { /* yuv-full */
            if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-yuv-full */
                memcpy(omap_cfg->rgb2yuv.coef, g_m33_rgb2yuv_bt709rf2bt601yf_2, sizeof(omap_cfg->rgb2yuv.coef));
                memcpy(omap_cfg->rgb2yuv.out_dc, g_v3_rgb2yuv_off, sizeof(omap_cfg->rgb2yuv.out_dc));
            } else if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT709) {
                /* 709-yuv-full */
                memcpy(omap_cfg->rgb2yuv.coef, g_m33_rgb2yuv_bt709rf2yf_2, sizeof(omap_cfg->rgb2yuv.coef));
                memcpy(omap_cfg->rgb2yuv.out_dc, g_v3_rgb2yuv_off, sizeof(omap_cfg->rgb2yuv.out_dc));
            } else if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT) {
                /* 2020-yuv-full */
                memcpy(omap_cfg->rgb2yuv.coef, g_m33_rgb2yuv_bt2020rf2yf_2, sizeof(omap_cfg->rgb2yuv.coef));
                memcpy(omap_cfg->rgb2yuv.out_dc, g_v3_rgb2yuv_off, sizeof(omap_cfg->rgb2yuv.out_dc));
            }
        }
    } else { /* rgb */
        if (out_cs.quantify_range == HI_DRV_COLOR_LIMITED_RANGE) { /* rgb-limit */
            if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-rgb-limit */
                memcpy(omap_cfg->rgb2yuv.coef, g_m33_rgb2yuv_bt709rf2bt601rl_s16, sizeof(omap_cfg->rgb2yuv.coef));
                memcpy(omap_cfg->rgb2yuv.out_dc, g_v3_rgb2yuv_dcoutrl, sizeof(omap_cfg->rgb2yuv.out_dc));
            } else {
                /* 709/2020-rgb-limit */
                memcpy(omap_cfg->rgb2yuv.coef, g_m33_rgb2yuv_rf2rl_s16, sizeof(omap_cfg->rgb2yuv.coef));
                memcpy(omap_cfg->rgb2yuv.out_dc, g_v3_rgb2yuv_dcoutrl, sizeof(omap_cfg->rgb2yuv.out_dc));
            }
        } else { /* rgb-full */
            if (out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 ||
                out_cs.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) {
                /* 601-rgb-full */
                memcpy(omap_cfg->rgb2yuv.coef, g_m33_rgb2yuv_bt709rf2bt601rf_s16, sizeof(omap_cfg->rgb2yuv.coef));
                memcpy(omap_cfg->rgb2yuv.out_dc, g_v3_rgb2yuv_dczero, sizeof(omap_cfg->rgb2yuv.out_dc));
            } else {
                /* 709/2020-rgb-full */
                omap_cfg->rgb2yuv.enable = HI_FALSE;
                memcpy(omap_cfg->rgb2yuv.coef, g_m33_rgb2yuv_rf2rf_s16, sizeof(omap_cfg->rgb2yuv.coef));
                memcpy(omap_cfg->rgb2yuv.out_dc, g_v3_rgb2yuv_dczero, sizeof(omap_cfg->rgb2yuv.out_dc));
            }
        }
    }

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_hdrv1cm_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
                                     pq_hdrv1_cm_cfg *cm_cfg)
{
    cm_cfg->clip_c_max = 0; /* 对应stHiHDRCmCfg.u16ClipMaxUV */
    cm_cfg->clip_c_min = 0; /* 对应stHiHDRCmCfg.u16ClipMinUV */
    cm_cfg->scale2p = 0; /* 对应stHiHDRCmCfg.u16ScaleCoefCM */

    return HI_SUCCESS;
}

static hi_void pq_hal_get_imap_default_cfg(hi_drv_pq_imap_cfg *imap_cfg)
{
     /* default para, every scene not change it */
    imap_cfg->v0_hdr_enable = HI_TRUE;
    imap_cfg->v1_hdr_enable = HI_FALSE;

    imap_cfg->yuv2rgb.v0_enable = HI_TRUE;
    imap_cfg->yuv2rgb.v1_enable = HI_FALSE; /* PIP use, this version only use v0 */

    imap_cfg->eotfparam_eotf = 1;
    imap_cfg->eotfparam_range_min = 0;
    imap_cfg->eotfparam_range = 16383; /* 16383: range */
    imap_cfg->eotfparam_range_inv = 65540; /* 65540: range */
    imap_cfg->v3ipt_off[0] = 0;
    imap_cfg->v3ipt_off[1] = 0;
    imap_cfg->v3ipt_off[2] = 0; /* 2: index */
    imap_cfg->ipt_scale = 0;
    imap_cfg->degamma_clip_max = 0; /* 2621440000UL: clip max, logic delete it */
    imap_cfg->degamma_clip_min = 0; /* logic delete it */

    imap_cfg->in_color = 0; /* reserve reg, ret 0 */
    imap_cfg->in_bits = 1;

    /* dolby used, HIHDR close */
    imap_cfg->demo_luma_en = HI_FALSE;
    imap_cfg->ladj_chroma_weight = 0; /* ladj_en close in HIHDR, not config it */
    imap_cfg->ladj_en = HI_FALSE;

    imap_cfg->ipt_in_sel = HI_FALSE;

    imap_cfg->norm_en = HI_FALSE; /* HIHDR close it */

    imap_cfg->rshift_round_en = HI_FALSE; /* logic delete it */
    imap_cfg->rshift_en = HI_FALSE; /* logic delete it */

    return;
}

static hi_void pq_hal_get_hdr2sdr_imap_base_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
    hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE; /* HDR2SDR场景下要关闭 */
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_HDR_CM_POS0;    /* HDR2SDR场景下要关闭 */

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_FALSE;
    imap_cfg->tmap_v1.tm_pos = PQ_HDR_TM_POS0; /* 这个的绑定位置如何决策？ */

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_get_imap_default_cfg(imap_cfg);

    return;
}

static hi_void pq_hal_get_hdr2hdr_imap_base_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
    hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE; /* SDR_WCG 关闭 */
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_HDR_CM_POS0;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_FALSE;
    imap_cfg->tmap_v1.tm_pos = PQ_HDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_get_imap_default_cfg(imap_cfg);

    return;
}

static hi_void pq_hal_get_hdr2hlg_imap_base_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
    hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE; /* 默认关闭 */
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_HDR_CM_POS0;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_HDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_get_imap_default_cfg(imap_cfg);

    return;
}

static hi_void pq_hal_get_hlg2sdr_imap_base_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
    hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE;
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_HDR_CM_POS1;
    if ((in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT2020) &&
        (out_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT709)) {
        imap_cfg->cacm.enable = HI_TRUE;
    }

    if ((in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT2020) &&
        (out_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT601_525 ||
         out_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT601_625)) {
        imap_cfg->cacm.enable = HI_TRUE;
    }

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_HDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_get_imap_default_cfg(imap_cfg);

    return;
}

static hi_void pq_hal_get_hlg2hdr10_imap_base_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
    hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE; /* 默认关闭 */
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_HDR_CM_POS0;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_HDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_get_imap_default_cfg(imap_cfg);

    return;
}

static hi_void pq_hal_get_hlg2hlg_imap_base_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
    hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE; /* SDR_WCG 关闭 */
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_HDR_CM_POS0;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_FALSE;
    imap_cfg->tmap_v1.tm_pos = PQ_HDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_get_imap_default_cfg(imap_cfg);

    return;
}

static hi_void pq_hal_get_sdr2sdr_imap_base_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
    hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE; /* SDR_WCG 关闭 */
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_HDR_CM_POS0;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmapv1 */
    imap_cfg->tmap_v1.enable = HI_FALSE;
    imap_cfg->tmap_v1.tm_pos = PQ_HDR_TM_POS0; /* 这个的绑定位置如何决策？ */

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_get_imap_default_cfg(imap_cfg);

    return;
}

static hi_void pq_hal_get_sdr2hdr_imap_base_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
    hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_TRUE;
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_HDR_CM_POS0;
    /*  SDR2020输入关闭CM */
    if (in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT2020) {
        imap_cfg->cacm.enable = HI_FALSE;
    }

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_FALSE;
    imap_cfg->tmap_v1.tm_pos = PQ_HDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_get_imap_default_cfg(imap_cfg);
    return;
}

static hi_void pq_hal_get_sdr2hlg_imap_base_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
    hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE;
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_HDR_CM_POS0;
    if ((in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT709) &&
        (out_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT2020)) {
        /* in:bt709, out:bt2020 */
        imap_cfg->cacm.enable = HI_TRUE;
    }

    if ((in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT601_525 ||
         in_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT601_625) &&
        (out_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT2020)) {
        /* in:bt601, out:bt2020 */
        imap_cfg->cacm.enable = HI_TRUE;
    }

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_HDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_get_imap_default_cfg(imap_cfg);

    return;
}

static hi_void pq_hal_get_hdr10plus2hdr10plus_imap_base_cfg(hi_drv_color_descript in_cs,
    hi_drv_color_descript out_cs, hi_drv_pq_imap_cfg *imap_cfg)
{
    return;
}

static hi_void pq_hal_get_hdr10plus2hdr10_imap_base_cfg(hi_drv_color_descript in_cs,
    hi_drv_color_descript out_cs, hi_drv_pq_imap_cfg *imap_cfg)
{
    return;
}

static hi_void pq_hal_get_hdr10plus2sdr_imap_base_cfg(hi_drv_color_descript in_cs,
    hi_drv_color_descript out_cs, hi_drv_pq_imap_cfg *imap_cfg)
{
    return;
}

static hi_void pq_hal_get_hdr10plus2hlg_imap_base_cfg(hi_drv_color_descript in_cs,
    hi_drv_color_descript out_cs, hi_drv_pq_imap_cfg *imap_cfg)
{
    return;
}

static hi_void pq_hal_get_tmap_default_cfg(pq_tmap_cfg *tmap_cfg)
{
    /* default para, every scene not change it */
    tmap_cfg->s2u_en = HI_TRUE; /* alway open it */

    tmap_cfg->sclut_rd_en = HI_FALSE; /* only debug use */
    tmap_cfg->sslut_rd_en = HI_FALSE; /* only debug use */
    tmap_cfg->tslut_rd_en = HI_FALSE; /* only debug use */
    tmap_cfg->silut_rd_en = HI_FALSE; /* only debug use */
    tmap_cfg->tilut_rd_en = HI_FALSE; /* only debug use */

    tmap_cfg->rshift_en = HI_FALSE;
    tmap_cfg->rshift_bit = 0;

    tmap_cfg->para_rdata = 0; /* read only reg, debug used it, not config */

    return;
}

static hi_s32 pq_hal_get_tmap_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
    pq_tmap_cfg *tmap_cfg)
{
    switch (g_sg_en_hdr_mode) {
        case HI_PQ_HDR_MODE_HDR10_TO_SDR:
            tmap_cfg->rshift_round_en = HI_TRUE;

            tmap_cfg->smc_enable = HI_TRUE;
            tmap_cfg->c1_expan = 1382; /* 1382: c1_expan */

            break;
        case HI_PQ_HDR_MODE_SDR_TO_HDR10:
            tmap_cfg->rshift_round_en = HI_TRUE;

            tmap_cfg->smc_enable = HI_FALSE;
            tmap_cfg->c1_expan = 1 << 10; /* 10: c1_expan rshift len */
            break;
        default:
            tmap_cfg->rshift_round_en = HI_TRUE;
            tmap_cfg->smc_enable = HI_FALSE;
            tmap_cfg->c1_expan = 1 << 10; /* 10: c1_expan rshift len */
            break;
    }

    pq_hal_get_tmap_default_cfg(tmap_cfg);

    return HI_SUCCESS;
}

static hi_void pq_hal_get_omap_default_cfg(pq_omap_cfg *omap_cfg)
{
    /* default para, every scene not change it */
    omap_cfg->denorm_en = HI_FALSE; /* HIHDR close */
    omap_cfg->cvm_en = HI_TRUE; /* open forever, control severy samll module */

    omap_cfg->range_min = 0;
    omap_cfg->range_over = 65535; /* 65535: range_over */
    omap_cfg->ipt_scale = 0;
    omap_cfg->oetf = 1;

    memcpy(omap_cfg->ipt_off, g_hdr2sdr_omap_ipt_0ff, sizeof(omap_cfg->ipt_off));

    return;
}

static hi_s32 pq_hal_get_hdr2sdr_omap_base_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
    pq_omap_cfg *omap_cfg)
{
    /* ipt2lms */
    omap_cfg->ipt2lms.enable = HI_TRUE;

    /* degamma */
    omap_cfg->degamma_en = HI_TRUE;

    /* lms2rgb */
    omap_cfg->lms2rgb.enable = HI_TRUE;

    /* gamma */
    omap_cfg->gamma_en = HI_TRUE;

    /* cacm */
    omap_cfg->cm_cfg.enable = HI_TRUE; /* HDR2SDR场景下要开启 */
    /* SDR2020输出关闭CM */
    if (out_cs.color_primary == HI_DRV_COLOR_PRIMARY_BT2020) {
        omap_cfg->cm_cfg.enable = HI_FALSE;
    }

    /* rgb2yuv */
    omap_cfg->rgb2yuv.enable = HI_TRUE;

    /* according scene cofig it */
    omap_cfg->u2s_enable = HI_TRUE;
    omap_cfg->lshift_en = HI_TRUE;

    omap_cfg->out_bits = 12; /* 12: out_bits */
    omap_cfg->out_color = 1;

    pq_hal_get_omap_default_cfg(omap_cfg);

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_hdrv1_bypass_omap_base_cfg(pq_omap_cfg *omap_cfg)
{
    /* ipt2lms */
    omap_cfg->ipt2lms.enable = HI_TRUE;

    /* degamma */
    omap_cfg->degamma_en = HI_FALSE;

    /* lms2rgb */
    omap_cfg->lms2rgb.enable = HI_FALSE;

    /* gamma */
    omap_cfg->gamma_en = HI_FALSE;

    /* cacm */
    omap_cfg->cm_cfg.enable = HI_FALSE;

    /* rgb2yuv */
    omap_cfg->rgb2yuv.enable = HI_TRUE;

    /* according scene cofig it */
    omap_cfg->u2s_enable = HI_TRUE;
    omap_cfg->lshift_en = HI_TRUE;

    omap_cfg->out_bits = 12; /* 12: out_bits */
    omap_cfg->out_color = 1;

    pq_hal_get_omap_default_cfg(omap_cfg);

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_sdr2hdr_omap_base_cfg(pq_omap_cfg *omap_cfg)
{
    /* ipt2lms */
    omap_cfg->ipt2lms.enable = HI_TRUE;

    /* degamma */
    omap_cfg->degamma_en = HI_TRUE;

    /* lms2rgb */
    omap_cfg->lms2rgb.enable = HI_TRUE;

    /* gamma */
    omap_cfg->gamma_en = HI_TRUE;

    /* cacm */
    omap_cfg->cm_cfg.enable = HI_FALSE; /* SDR2HDR场景下要关闭 */

    /* rgb2yuv */
    omap_cfg->rgb2yuv.enable = HI_TRUE;

    omap_cfg->out_bits = 12; /* 12: out_bits */
    omap_cfg->out_color = 1;
    omap_cfg->u2s_enable = HI_TRUE;
    omap_cfg->lshift_en = HI_TRUE;

    pq_hal_get_omap_default_cfg(omap_cfg);

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_hdr10plus2hdr10plus_omap_base_cfg(pq_omap_cfg *omap_cfg)
{
    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_hdr10plus2hdr10_omap_base_cfg(pq_omap_cfg *omap_cfg)
{
    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_hdr10plus2sdr_omap_base_cfg(pq_omap_cfg *omap_cfg)
{
    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_hdr10plus2hlg_omap_base_cfg(pq_omap_cfg *omap_cfg)
{
    return HI_SUCCESS;
}

static hi_void pq_hdr_set_hdrv1cm_coef2ddr(hi_u8 *addr)
{
    pq_coef_gen_cfg coef;
    pq_coef_send_cfg coef_send;
    void *p_coef_array[1];

    struct file *fp_vhdr_cm_coef = NULL;
    hi_u32 lut_length = 32; /* 32: lut length */
    hi_u32 coef_bit_length = 10;

    PQ_CHECK_NULL_PTR_RE_NULL(addr);
    PQ_CHECK_NULL_PTR_RE_NULL(g_hdrv1_cm_lut);

    coef.p_coef = g_hdrv1_cm_lut;
    coef.p_coef_new = g_vhdr_cm_lut_new;
    coef.length = 32; /* 32: lut length */
    coef.coef_max = ((1 << 10) - 1); /* 10: bit */
    coef.coef_min = 0;
    coef.coef_data_type = DRV_COEF_DATA_TYPE_U32;
    pq_drv_gen_coef(&coef);

    p_coef_array[0] = g_vhdr_cm_lut_new;

    coef_send.coef_addr = addr;
    coef_send.sti_type = PQ_STI_FILE_COEF_VHDR_CM;
    coef_send.fp_coef = fp_vhdr_cm_coef;
    coef_send.lut_num = 1;
    coef_send.burst_num = 1;
    coef_send.cycle_num = 8; /* 8: cycle_num */
    coef_send.p_coef_array = p_coef_array;
    coef_send.lut_length = &lut_length;
    coef_send.coef_bit_length = &coef_bit_length;
    coef_send.data_type = DRV_COEF_DATA_TYPE_U32;

    addr = pq_send_coef2ddr(&coef_send);

    return;
}

static hi_void pq_hdr_set_imap_coef2ddr(hi_u8 *addr, hi_u8 *addr2)
{
    pq_coef_gen_cfg coef_gen;
    pq_coef_send_cfg coef_send;

    struct file *fp_dm_input_degmm_coef = HI_NULL;
    struct file *fp_dm_input_gmm_coef = HI_NULL;
    void *p_coef_array[1];

    hi_u32 lut_length = 1024; /* 1024: lut_length */
    hi_u32 coef_bit_length = 32; /* 32: coef_bit_length */
    void *p_coef_array_l2pq[] = { g_dbhdr_gamm_l2pq_lut_x, g_dbhdr_gamm_l2pq_lut_a, g_dbhdr_gamm_l2pq_lut_b };
    hi_u32 lut_length2[] = {128, 128, 128};
    hi_u32 coef_bit_length2[] = {32, 28, 16};

    PQ_CHECK_NULL_PTR_RE_NULL(addr);
    PQ_CHECK_NULL_PTR_RE_NULL(addr2);
    PQ_CHECK_NULL_PTR_RE_NULL(g_imap_degamma);
    PQ_CHECK_NULL_PTR_RE_NULL(g_imap_gamma_lut_x);
    PQ_CHECK_NULL_PTR_RE_NULL(g_imap_gamma_lut_a);
    PQ_CHECK_NULL_PTR_RE_NULL(g_imap_gamma_lut_b);

    coef_gen.p_coef = g_imap_degamma;
    coef_gen.p_coef_new = g_db_hdr_degamm_pq_new;
    coef_gen.coef_data_type = DRV_COEF_DATA_TYPE_U32;
    coef_gen.length = 1024; /* 1024: lut_length */
    coef_gen.coef_max = (hi_u32)(((hi_u64)1 << 31) - 1); /* 31: bit */
    coef_gen.coef_min = 0;
    pq_drv_gen_coef(&coef_gen);

    p_coef_array[0] = g_db_hdr_degamm_pq_new;

    coef_send.coef_addr = addr;
    coef_send.sti_type = PQ_STI_FILE_COEF_HDR2;
    coef_send.fp_coef = fp_dm_input_degmm_coef;
    coef_send.lut_num = 1;
    coef_send.burst_num = 1;
    coef_send.cycle_num = 4; /* 4: cycle_num */
    coef_send.p_coef_array = p_coef_array;
    coef_send.lut_length = &lut_length;
    coef_send.coef_bit_length = &coef_bit_length;
    coef_send.data_type = DRV_COEF_DATA_TYPE_U32;
    pq_send_coef2ddr(&coef_send);

    coef_gen.p_coef = g_imap_gamma_lut_x;
    coef_gen.p_coef_new = g_dbhdr_gamm_l2pq_lut_x;
    coef_gen.coef_data_type = DRV_COEF_DATA_TYPE_U32;
    coef_gen.length = 128; /* 128: length */
    coef_gen.coef_max = (hi_u32)(((hi_u64)1 << 31) - 1); /* 31: bit */
    coef_gen.coef_min = 0;
    pq_drv_gen_coef(&coef_gen);

    coef_gen.p_coef = g_imap_gamma_lut_a;
    coef_gen.p_coef_new = g_dbhdr_gamm_l2pq_lut_a;
    coef_gen.coef_data_type = DRV_COEF_DATA_TYPE_U32;
    coef_gen.length = 128; /* 128: length */
    coef_gen.coef_max = (1 << 28) - 1; /* 28: bit */
    coef_gen.coef_min = 0;
    pq_drv_gen_coef(&coef_gen);

    coef_gen.p_coef = g_imap_gamma_lut_b;
    coef_gen.p_coef_new = g_dbhdr_gamm_l2pq_lut_b;
    coef_gen.coef_data_type = DRV_COEF_DATA_TYPE_U32;
    coef_gen.length = 128; /* 128: length */
    coef_gen.coef_max = (1 << 16) - 1; /* 16: bit */
    coef_gen.coef_min = 0;
    pq_drv_gen_coef(&coef_gen);

    coef_send.coef_addr = addr2;
    coef_send.sti_type = PQ_STI_FILE_COEF_HDR3;
    coef_send.fp_coef = fp_dm_input_gmm_coef;
    coef_send.lut_num = 3; /* 3: lut_num */
    coef_send.burst_num = 1;
    coef_send.cycle_num = 1;
    coef_send.p_coef_array = p_coef_array_l2pq;
    coef_send.lut_length = lut_length2;
    coef_send.coef_bit_length = coef_bit_length2;
    coef_send.data_type = DRV_COEF_DATA_TYPE_U32;
    pq_send_coef2ddr(&coef_send);

    return;
}

static hi_void pq_hal_send_imap_cacm_coef2ddr(cm_coef *plutCMCoef, hi_u8 *addr)
{
    pq_coef_send_cfg coef_send;

    hi_u16 lut_eq_0[100] = { 0 }; /* 100: lut length */
    hi_u16 lut_eq_1[100] = { 0 }; /* 100: lut length */
    void *coef_array[11] = { /* 11: lut length */
        plutCMCoef->lut1_b, plutCMCoef->lut1_g,
        plutCMCoef->lut1_r, plutCMCoef->lut2_b,
        plutCMCoef->lut2_g, plutCMCoef->lut2_r,
        plutCMCoef->lut4_b, plutCMCoef->lut4_g,
        plutCMCoef->lut4_r, lut_eq_1, lut_eq_0
    };
    hi_u32 lut_length[11] = { /* 11: lut length */
        100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100
    };
    hi_u32 coef_bit_length[11] = { /* 11: lut length */
        12, 12, 12, 12, 12, 12, 12, 12, 12, 10, 10
    };

    hi_u16 lut_eq_2[80] = { 0 }; /* 80: lut length */
    hi_u16 lut_eq_3[80] = { 0 }; /* 80: lut length */
    void *coef_array1[11] = { /* 11: lut length */
        plutCMCoef->lut3_b, plutCMCoef->lut3_g,
        plutCMCoef->lut3_r, plutCMCoef->lut5_b,
        plutCMCoef->lut5_g, plutCMCoef->lut5_r,
        plutCMCoef->lut6_b, plutCMCoef->lut6_g,
        plutCMCoef->lut6_r, lut_eq_3, lut_eq_2
    };
    hi_u32 lut_length1[11] = { /* 11: lut length */
        80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80
    };
    hi_u32 coef_bit_length1[11] = { /* 11: lut length */
        12, 12, 12, 12, 12, 12, 12, 12, 12, 10, 10
    };

    hi_u16 lut_eq_4[64] = { 0 }; /* 64: lut length */
    hi_u16 lut_eq_5[64] = { 0 }; /* 64: lut length */
    void *coef_array2[11] = {  /* 11: lut length */
        plutCMCoef->lut00_b, plutCMCoef->lut00_g,
        plutCMCoef->lut00_r, plutCMCoef->lut01_b,
        plutCMCoef->lut01_g, plutCMCoef->lut01_r,
        plutCMCoef->lut7_b, plutCMCoef->lut7_g,
        plutCMCoef->lut7_r, lut_eq_5, lut_eq_4
    };
    hi_u32 lut_length2[11] = { /* 11: lut length */
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
    };
    hi_u32 coef_bit_length2[11] = { /* 11: lut length */
        12, 12, 12, 12, 12, 12, 12, 12, 12, 10, 10
    };

    PQ_CHECK_NULL_PTR_RE_NULL(addr);

    coef_send.coef_addr = addr;
    coef_send.lut_num = 11; /* 11: lut length */
    coef_send.burst_num = 1;
    coef_send.cycle_num = 1;
    coef_send.p_coef_array = coef_array;
    coef_send.lut_length = lut_length;
    coef_send.coef_bit_length = coef_bit_length;
    coef_send.data_type = DRV_COEF_DATA_TYPE_U16;
    addr = pq_send_coef2ddr(&coef_send);

    coef_send.coef_addr = addr;
    coef_send.cycle_num = 1;
    coef_send.p_coef_array = coef_array1;
    coef_send.lut_length = lut_length1;
    coef_send.coef_bit_length = coef_bit_length1;
    coef_send.data_type = DRV_COEF_DATA_TYPE_U16;
    addr = pq_send_coef2ddr(&coef_send);

    coef_send.coef_addr = addr;
    coef_send.cycle_num = 1;
    coef_send.p_coef_array = coef_array2;
    coef_send.lut_length = lut_length2;
    coef_send.coef_bit_length = coef_bit_length2;
    coef_send.data_type = DRV_COEF_DATA_TYPE_U16;
    addr = pq_send_coef2ddr(&coef_send);
}

static hi_void pq_hal_set_imap_cacm_coef2ddr(hi_u8 *addr)
{
    hi_u16 *clut00, *clut01, *clut1, *clut2, *clut3, *clut4, *clut5, *clut6, *clut7;
    cm_coef lut_cm_coef;

    hi_u16 ii_r, ii_g, ii_b;
    hi_u16 j;

    PQ_CHECK_NULL_PTR_RE_NULL(addr);

    for (ii_r = 0; ii_r < LUT_SIZE; ii_r++) {
        for (ii_g = 0; ii_g < LUT_SIZE; ii_g++) {
            for (ii_b = 0; ii_b < LUT_SIZE; ii_b++) {
                if ((ii_r % 2 == 0) && (ii_g % 2 == 0) && (ii_b % 2 == 0)) { /* 2: num */
                    g_hdr_coef_cm_lutr3d0[LUT_EVE_SIZE * LUT_EVE_SIZE * ((ii_r + 1) / 2) + /* 2: num */
                    LUT_EVE_SIZE * ((ii_g + 1) / 2) + (ii_b + 1) / 2] = /* 2: num */
                    g_imap_cacm_lut_r[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutg3d0[LUT_EVE_SIZE * LUT_EVE_SIZE * ((ii_r + 1) / 2) + /* 2: num */
                    LUT_EVE_SIZE * ((ii_g + 1) / 2) + (ii_b + 1) / 2] = /* 2: num */
                    g_imap_cacm_lut_g[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutb3d0[LUT_EVE_SIZE * LUT_EVE_SIZE * ((ii_r + 1) / 2) + /* 2: num */
                    LUT_EVE_SIZE * ((ii_g + 1) / 2) + (ii_b + 1) / 2] = /* 2: num */
                    g_imap_cacm_lut_b[ii_r][ii_g][ii_b];
                }

                if ((ii_r % 2 == 0) && (ii_g % 2 == 0) && (ii_b % 2 == 1)) { /* 2: num */
                    g_hdr_coef_cm_lutr3d1[LUT_EVE_SIZE * LUT_ODD_SIZE * ((ii_r + 1) / 2) + /* 2: num */
                    LUT_ODD_SIZE * ((ii_g + 1) / 2) + (ii_b + 0) / 2] = /* 2: num */
                    g_imap_cacm_lut_r[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutg3d1[LUT_EVE_SIZE * LUT_ODD_SIZE * ((ii_r + 1) / 2) + /* 2: num */
                    LUT_ODD_SIZE * ((ii_g + 1) / 2) + (ii_b + 0) / 2] = /* 2: num */
                    g_imap_cacm_lut_g[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutb3d1[LUT_EVE_SIZE * LUT_ODD_SIZE * ((ii_r + 1) / 2) + /* 2: num */
                    LUT_ODD_SIZE * ((ii_g + 1) / 2) + (ii_b + 0) / 2] = /* 2: num */
                    g_imap_cacm_lut_b[ii_r][ii_g][ii_b];
                }

                if ((ii_r % 2 == 0) && (ii_g % 2 == 1) && (ii_b % 2 == 0)) { /* 2: num */
                    g_hdr_coef_cm_lutr3d4[LUT_EVE_SIZE * LUT_ODD_SIZE * ((ii_r + 1) / 2) + /* 2: num */
                    LUT_EVE_SIZE * ((ii_g + 0) / 2) + (ii_b + 1) / 2] = /* 2: num */
                    g_imap_cacm_lut_r[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutg3d4[LUT_EVE_SIZE * LUT_ODD_SIZE * ((ii_r + 1) / 2) + /* 2: num */
                    LUT_EVE_SIZE * ((ii_g + 0) / 2) + (ii_b + 1) / 2] = /* 2: num */
                    g_imap_cacm_lut_g[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutb3d4[LUT_EVE_SIZE * LUT_ODD_SIZE * ((ii_r + 1) / 2) + /* 2: num */
                    LUT_EVE_SIZE * ((ii_g + 0) / 2) + (ii_b + 1) / 2] = /* 2: num */
                    g_imap_cacm_lut_b[ii_r][ii_g][ii_b];
                }

                if ((ii_r % 2 == 0) && (ii_g % 2 == 1) && (ii_b % 2 == 1)) { /* 2: num */
                    g_hdr_coef_cm_lutr3d5[LUT_ODD_SIZE * LUT_ODD_SIZE * ((ii_r + 1) / 2) + /* 2: num */
                    LUT_ODD_SIZE * ((ii_g + 0) / 2) + (ii_b + 0) / 2] = /* 2: num */
                    g_imap_cacm_lut_r[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutg3d5[LUT_ODD_SIZE * LUT_ODD_SIZE * ((ii_r + 1) / 2) + /* 2: num */
                    LUT_ODD_SIZE * ((ii_g + 0) / 2) + (ii_b + 0) / 2] = /* 2: num */
                    g_imap_cacm_lut_g[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutb3d5[LUT_ODD_SIZE * LUT_ODD_SIZE * ((ii_r + 1) / 2) + /* 2: num */
                    LUT_ODD_SIZE * ((ii_g + 0) / 2) + (ii_b + 0) / 2] = /* 2: num */
                    g_imap_cacm_lut_b[ii_r][ii_g][ii_b];
                }

                if ((ii_r % 2 == 1) && (ii_g % 2 == 0) && (ii_b % 2 == 0)) { /* 2: num */
                    g_hdr_coef_cm_lutr3d2[LUT_EVE_SIZE * LUT_EVE_SIZE * ((ii_r + 0) / 2) + /* 2: num */
                    LUT_EVE_SIZE * ((ii_g + 1) / 2) + (ii_b + 1) / 2] = /* 2: num */
                    g_imap_cacm_lut_r[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutg3d2[LUT_EVE_SIZE * LUT_EVE_SIZE * ((ii_r + 0) / 2) + /* 2: num */
                    LUT_EVE_SIZE * ((ii_g + 1) / 2) + (ii_b + 1) / 2] = /* 2: num */
                    g_imap_cacm_lut_g[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutb3d2[LUT_EVE_SIZE * LUT_EVE_SIZE * ((ii_r + 0) / 2) + /* 2: num */
                    LUT_EVE_SIZE * ((ii_g + 1) / 2) + (ii_b + 1) / 2] = /* 2: num */
                    g_imap_cacm_lut_b[ii_r][ii_g][ii_b];
                }

                if ((ii_r % 2 == 1) && (ii_g % 2 == 0) && (ii_b % 2 == 1)) { /* 2: num */
                    g_hdr_coef_cm_lutr3d3[LUT_EVE_SIZE * LUT_ODD_SIZE * ((ii_r + 0) / 2) + /* 2: num */
                    LUT_ODD_SIZE * ((ii_g + 1) / 2) + (ii_b + 0) / 2] = /* 2: num */
                    g_imap_cacm_lut_r[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutg3d3[LUT_EVE_SIZE * LUT_ODD_SIZE * ((ii_r + 0) / 2) + /* 2: num */
                    LUT_ODD_SIZE * ((ii_g + 1) / 2) + (ii_b + 0) / 2] = /* 2: num */
                    g_imap_cacm_lut_g[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutb3d3[LUT_EVE_SIZE * LUT_ODD_SIZE * ((ii_r + 0) / 2) + /* 2: num */
                    LUT_ODD_SIZE * ((ii_g + 1) / 2) + (ii_b + 0) / 2] = /* 2: num */
                    g_imap_cacm_lut_b[ii_r][ii_g][ii_b];
                }

                if ((ii_r % 2 == 1) && (ii_g % 2 == 1) && (ii_b % 2 == 0)) { /* 2: num */
                    g_hdr_coef_cm_lutr3d6[LUT_EVE_SIZE * LUT_ODD_SIZE * ((ii_r + 0) / 2) + /* 2: num */
                    LUT_EVE_SIZE * ((ii_g + 0) / 2) + (ii_b + 1) / 2] = /* 2: num */
                    g_imap_cacm_lut_r[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutg3d6[LUT_EVE_SIZE * LUT_ODD_SIZE * ((ii_r + 0) / 2) + /* 2: num */
                    LUT_EVE_SIZE * ((ii_g + 0) / 2) + (ii_b + 1) / 2] = /* 2: num */
                    g_imap_cacm_lut_g[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutb3d6[LUT_EVE_SIZE * LUT_ODD_SIZE * ((ii_r + 0) / 2) + /* 2: num */
                    LUT_EVE_SIZE * ((ii_g + 0) / 2) + (ii_b + 1) / 2] = /* 2: num */
                    g_imap_cacm_lut_b[ii_r][ii_g][ii_b];
                }

                if ((ii_r % 2 == 1) && (ii_g % 2 == 1) && (ii_b % 2 == 1)) { /* 2: num */
                    g_hdr_coef_cm_lutr3d7[LUT_ODD_SIZE * LUT_ODD_SIZE * ((ii_r + 0) / 2) + /* 2: num */
                    LUT_ODD_SIZE * ((ii_g + 0) / 2) + (ii_b + 0) / 2] = /* 2: num */
                    g_imap_cacm_lut_r[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutg3d7[LUT_ODD_SIZE * LUT_ODD_SIZE * ((ii_r + 0) / 2) + /* 2: num */
                    LUT_ODD_SIZE * ((ii_g + 0) / 2) + (ii_b + 0) / 2] = /* 2: num */
                    g_imap_cacm_lut_g[ii_r][ii_g][ii_b];
                    g_hdr_coef_cm_lutb3d7[LUT_ODD_SIZE * LUT_ODD_SIZE * ((ii_r + 0) / 2) + /* 2: num */
                    LUT_ODD_SIZE * ((ii_g + 0) / 2) + (ii_b + 0) / 2] = /* 2: num */
                    g_imap_cacm_lut_b[ii_r][ii_g][ii_b];
                }
            }
        }
    }

    clut00 = g_hdr_coef_cm_lut00_r;
    clut01 = g_hdr_coef_cm_lut01_r;
    clut1 = g_hdr_coef_cm_lut1_r;
    clut2 = g_hdr_coef_cm_lut2_r;
    clut3 = g_hdr_coef_cm_lut3_r;
    clut4 = g_hdr_coef_cm_lut4_r;
    clut5 = g_hdr_coef_cm_lut5_r;
    clut6 = g_hdr_coef_cm_lut6_r;
    clut7 = g_hdr_coef_cm_lut7_r;

    for (j = 0; j < 125; j++) { /* 125: lut len */
        if (j % 2 == 0) { /* 2: num */
            *clut00 = g_hdr_coef_cm_lutr3d0[j];
            clut00++;
        }
        if (j % 2 == 1) { /* 2: num */
            *clut01 = g_hdr_coef_cm_lutr3d0[j];
            clut01++;
        }
    }
    for (j = 0; j < 100; j++) { /* 100: lut len */
        *clut1 = g_hdr_coef_cm_lutr3d1[j];
        clut1++;
    }

    for (j = 0; j < 100; j++) { /* 100: lut len */
        *clut2 = g_hdr_coef_cm_lutr3d2[j];
        clut2++;
    }
    for (j = 0; j < 80; j++) { /* 80: lut len */
        *clut3 = g_hdr_coef_cm_lutr3d3[j];
        clut3++;
    }
    for (j = 0; j < 100; j++) { /* 100: lut len */
        *clut4 = g_hdr_coef_cm_lutr3d4[j];
        clut4++;
    }
    for (j = 0; j < 80; j++) { /* 80: lut len */
        *clut5 = g_hdr_coef_cm_lutr3d5[j];
        clut5++;
    }

    for (j = 0; j < 80; j++) { /* 80: lut len */
        *clut6 = g_hdr_coef_cm_lutr3d6[j];
        clut6++;
    }
    for (j = 0; j < 64; j++) { /* 64: lut len */
        *clut7 = g_hdr_coef_cm_lutr3d7[j];
        clut7++;
    }

    clut00 = g_hdr_coef_cm_lut00_g;
    clut01 = g_hdr_coef_cm_lut01_g;
    clut1 = g_hdr_coef_cm_lut1_g;
    clut2 = g_hdr_coef_cm_lut2_g;
    clut3 = g_hdr_coef_cm_lut3_g;
    clut4 = g_hdr_coef_cm_lut4_g;
    clut5 = g_hdr_coef_cm_lut5_g;
    clut6 = g_hdr_coef_cm_lut6_g;
    clut7 = g_hdr_coef_cm_lut7_g;

    for (j = 0; j < 125; j++) { /* 125: lut len */
        if (j % 2 == 0) { /* 2: num */
            *clut00 = g_hdr_coef_cm_lutg3d0[j];
            clut00++;
        }
        if (j % 2 == 1) { /* 2: num */
            *clut01 = g_hdr_coef_cm_lutg3d0[j];
            clut01++;
        }
    }
    for (j = 0; j < 100; j++) { /* 100: lut len */
        *clut1 = g_hdr_coef_cm_lutg3d1[j];
        clut1++;
    }

    for (j = 0; j < 100; j++) { /* 100: lut len */
        *clut2 = g_hdr_coef_cm_lutg3d2[j];
        clut2++;
    }
    for (j = 0; j < 80; j++) { /* 80: lut len */
        *clut3 = g_hdr_coef_cm_lutg3d3[j];
        clut3++;
    }
    for (j = 0; j < 100; j++) { /* 100: lut len */
        *clut4 = g_hdr_coef_cm_lutg3d4[j];
        clut4++;
    }
    for (j = 0; j < 80; j++) { /* 80: lut len */
        *clut5 = g_hdr_coef_cm_lutg3d5[j];
        clut5++;
    }

    for (j = 0; j < 80; j++) { /* 80: lut len */
        *clut6 = g_hdr_coef_cm_lutg3d6[j];
        clut6++;
    }
    for (j = 0; j < 64; j++) { /* 64: lut len */
        *clut7 = g_hdr_coef_cm_lutg3d7[j];
        clut7++;
    }

    clut00 = g_hdr_coef_cm_lut00_b;
    clut01 = g_hdr_coef_cm_lut01_b;
    clut1 = g_hdr_coef_cm_lut1_b;
    clut2 = g_hdr_coef_cm_lut2_b;
    clut3 = g_hdr_coef_cm_lut3_b;
    clut4 = g_hdr_coef_cm_lut4_b;
    clut5 = g_hdr_coef_cm_lut5_b;
    clut6 = g_hdr_coef_cm_lut6_b;
    clut7 = g_hdr_coef_cm_lut7_b;

    for (j = 0; j < 125; j++) { /* 125: lut len */
        if (j % 2 == 0) { /* 2: num */
            *clut00 = g_hdr_coef_cm_lutb3d0[j];
            clut00++;
        }
        if (j % 2 == 1) { /* 2: num */
            *clut01 = g_hdr_coef_cm_lutb3d0[j];
            clut01++;
        }
    }
    for (j = 0; j < 100; j++) { /* 100: lut len */
        *clut1 = g_hdr_coef_cm_lutb3d1[j];
        clut1++;
    }

    for (j = 0; j < 100; j++) { /* 100: lut len */
        *clut2 = g_hdr_coef_cm_lutb3d2[j];
        clut2++;
    }
    for (j = 0; j < 80; j++) { /* 80: lut len */
        *clut3 = g_hdr_coef_cm_lutb3d3[j];
        clut3++;
    }
    for (j = 0; j < 100; j++) { /* 100: lut len */
        *clut4 = g_hdr_coef_cm_lutb3d4[j];
        clut4++;
    }
    for (j = 0; j < 80; j++) { /* 80: lut len */
        *clut5 = g_hdr_coef_cm_lutb3d5[j];
        clut5++;
    }

    for (j = 0; j < 80; j++) { /* 80: lut len */
        *clut6 = g_hdr_coef_cm_lutb3d6[j];
        clut6++;
    }
    for (j = 0; j < 64; j++) { /* 64: lut len */
        *clut7 = g_hdr_coef_cm_lutb3d7[j];
        clut7++;
    }

    lut_cm_coef.lut00_r = g_hdr_coef_cm_lut00_r;
    lut_cm_coef.lut00_g = g_hdr_coef_cm_lut00_g;
    lut_cm_coef.lut00_b = g_hdr_coef_cm_lut00_b;
    lut_cm_coef.lut01_r = g_hdr_coef_cm_lut01_r;
    lut_cm_coef.lut01_g = g_hdr_coef_cm_lut01_g;
    lut_cm_coef.lut01_b = g_hdr_coef_cm_lut01_b;
    lut_cm_coef.lut1_r = g_hdr_coef_cm_lut1_r;
    lut_cm_coef.lut1_g = g_hdr_coef_cm_lut1_g;
    lut_cm_coef.lut1_b = g_hdr_coef_cm_lut1_b;
    lut_cm_coef.lut2_r = g_hdr_coef_cm_lut2_r;
    lut_cm_coef.lut2_g = g_hdr_coef_cm_lut2_g;
    lut_cm_coef.lut2_b = g_hdr_coef_cm_lut2_b;
    lut_cm_coef.lut3_r = g_hdr_coef_cm_lut3_r;
    lut_cm_coef.lut3_g = g_hdr_coef_cm_lut3_g;
    lut_cm_coef.lut3_b = g_hdr_coef_cm_lut3_b;
    lut_cm_coef.lut4_r = g_hdr_coef_cm_lut4_r;
    lut_cm_coef.lut4_g = g_hdr_coef_cm_lut4_g;
    lut_cm_coef.lut4_b = g_hdr_coef_cm_lut4_b;
    lut_cm_coef.lut5_r = g_hdr_coef_cm_lut5_r;
    lut_cm_coef.lut5_g = g_hdr_coef_cm_lut5_g;
    lut_cm_coef.lut5_b = g_hdr_coef_cm_lut5_b;
    lut_cm_coef.lut6_r = g_hdr_coef_cm_lut6_r;
    lut_cm_coef.lut6_g = g_hdr_coef_cm_lut6_g;
    lut_cm_coef.lut6_b = g_hdr_coef_cm_lut6_b;
    lut_cm_coef.lut7_r = g_hdr_coef_cm_lut7_r;
    lut_cm_coef.lut7_g = g_hdr_coef_cm_lut7_g;
    lut_cm_coef.lut7_b = g_hdr_coef_cm_lut7_b;

    pq_hal_send_imap_cacm_coef2ddr(&lut_cm_coef, addr);
}

static hi_void pq_hdr_set_tmap_coef2ddr(hi_u8 *addr)
{
    pq_coef_gen_cfg coef_gen;
    pq_coef_send_cfg coef_send;

    void *p_coef_array[] = {
        g_pq_hdr_tmapv2_tm_lut_i_new, g_pq_hdr_tmapv2_sm_lut_i_new,
        g_pq_hdr_tmapv2_tm_lut_s_new, g_pq_hdr_tmapv2_sm_lut_s_new
    };
    hi_u32 lut_length[] = { 512, 512, 512, 512 };
    hi_u32 coef_bit_length[] = { 16, 16, 16, 16 };

    struct file *fp_dm_cvm_coef = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_NULL(addr);
    PQ_CHECK_NULL_PTR_RE_NULL(g_tmap_tm_lut_i);
    PQ_CHECK_NULL_PTR_RE_NULL(g_tmap_sm_lut_i);
    PQ_CHECK_NULL_PTR_RE_NULL(g_tmap_tm_lut_s);
    PQ_CHECK_NULL_PTR_RE_NULL(g_tmap_sm_lut_s);
    PQ_CHECK_NULL_PTR_RE_NULL(g_tmap_smc_lut);

    coef_gen.p_coef = g_tmap_tm_lut_i;
    coef_gen.p_coef_new = g_pq_hdr_tmapv2_tm_lut_i_new;
    coef_gen.coef_data_type = DRV_COEF_DATA_TYPE_S32;
    coef_gen.length = 512; /* 512 : length */
    coef_gen.coef_max = (1 << 15) - 1; /* 15 : bit */
    coef_gen.coef_min = -((1 << 15) - 1); /* 15 : bit */
    pq_drv_gen_coef(&coef_gen);

    coef_gen.p_coef = g_tmap_sm_lut_i;
    coef_gen.p_coef_new = g_pq_hdr_tmapv2_sm_lut_i_new;
    pq_drv_gen_coef(&coef_gen);

    coef_gen.p_coef = g_tmap_tm_lut_s;
    coef_gen.p_coef_new = g_pq_hdr_tmapv2_tm_lut_s_new;
    pq_drv_gen_coef(&coef_gen);

    coef_gen.p_coef = g_tmap_sm_lut_s;
    coef_gen.p_coef_new = g_pq_hdr_tmapv2_sm_lut_s_new;
    pq_drv_gen_coef(&coef_gen);

    coef_send.coef_addr = addr;
    coef_send.sti_type = PQ_STI_FILE_COEF_HDR6;
    coef_send.fp_coef = fp_dm_cvm_coef;
    coef_send.lut_num = 4; /* 4: bit */
    coef_send.burst_num = 1;
    coef_send.cycle_num = 2; /* 2: cycle_num */
    coef_send.p_coef_array = p_coef_array;
    coef_send.lut_length = lut_length;
    coef_send.coef_bit_length = coef_bit_length;
    coef_send.data_type = DRV_COEF_DATA_TYPE_S32;
    addr = pq_send_coef2ddr(&coef_send);

    coef_gen.p_coef = g_tmap_smc_lut;
    coef_gen.p_coef_new = g_pq_hdr_tmapv2_sm_lut_c_new;
    coef_gen.coef_data_type = DRV_COEF_DATA_TYPE_S32;
    coef_gen.length = 512; /* 512: length */
    coef_gen.coef_max = (1 << 15) - 1; /* 15: bit */
    coef_gen.coef_min = -((1 << 15) - 1); /* 15: bit */
    pq_drv_gen_coef(&coef_gen);

    p_coef_array[0] = (hi_s32 *)g_pq_hdr_tmapv2_sm_lut_c_new;
    coef_send.coef_addr = addr;
    coef_send.sti_type = PQ_STI_FILE_COEF_HDR6;
    coef_send.fp_coef = fp_dm_cvm_coef;
    coef_send.lut_num = 1;
    coef_send.burst_num = 1;
    coef_send.cycle_num = 8; /* 8: cycle_num */
    coef_send.p_coef_array = p_coef_array;
    coef_send.lut_length = lut_length;
    coef_send.coef_bit_length = coef_bit_length;
    coef_send.data_type = DRV_COEF_DATA_TYPE_S32;
    pq_send_coef2ddr(&coef_send);

    return;
}

static hi_void pq_hdr_set_imap_tmpa_v1_coef(hi_u8 *addr)
{
    pq_coef_gen_cfg coef_gen;
    pq_coef_send_cfg coef_send;

    void *p_coef_array[4] = { /* 4 is array length */
        HI_NULL, HI_NULL,
        HI_NULL, HI_NULL
    };
    hi_u32 lut_length[] = { 512, 512, 512, 512 };
    hi_u32 coef_bit_length[] = { 16, 16, 16, 16 };

    struct file *fp_dm_cvm_coef = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_NULL(addr);
    PQ_CHECK_NULL_PTR_RE_NULL(g_tmap_tm_lut_i);
    PQ_CHECK_NULL_PTR_RE_NULL(g_tmap_sm_lut_i);
    PQ_CHECK_NULL_PTR_RE_NULL(g_tmap_tm_lut_s);
    PQ_CHECK_NULL_PTR_RE_NULL(g_tmap_sm_lut_s);
    PQ_CHECK_NULL_PTR_RE_NULL(g_tmap_smc_lut);

    coef_gen.p_coef = g_tmap_hlg_lut_tmapv1;
    coef_gen.p_coef_new = g_pq_hdr_tmapv1_lut_c_new;
    coef_gen.coef_data_type = DRV_COEF_DATA_TYPE_S32;
    coef_gen.length = 512;                /* 512: length */
    coef_gen.coef_max = (1 << 15) - 1;    /* 15: bit */
    coef_gen.coef_min = -((1 << 15) - 1); /* 15: bit */
    pq_drv_gen_coef(&coef_gen);

    p_coef_array[0] = (hi_s32 *)g_pq_hdr_tmapv1_lut_c_new;
    coef_send.coef_addr = addr;
    coef_send.sti_type = PQ_STI_FILE_COEF_HDR4;
    coef_send.fp_coef = fp_dm_cvm_coef;
    coef_send.lut_num = 1;
    coef_send.burst_num = 1;
    coef_send.cycle_num = 8; /* 8: cycle_num */
    coef_send.p_coef_array = p_coef_array;
    coef_send.lut_length = lut_length;
    coef_send.coef_bit_length = coef_bit_length;
    coef_send.data_type = DRV_COEF_DATA_TYPE_S32;
    pq_send_coef2ddr(&coef_send);

    return;
}

static hi_void pq_hdr_set_omap_coef2ddr(hi_u8 *addr1, hi_u8 *addr2, hi_u8 *addr3)
{
    pq_coef_gen_cfg coef_gen;
    pq_coef_send_cfg coef_send;

    hi_u32 lut_len;
    hi_u32 coef_bit_len;
    struct file *fp_om_output_degmm_coef = HI_NULL;
    struct file *fp_om_output_gmm_coef = HI_NULL;

    void *p_coef_array_l2pq[] = { g_omap_l2p_qlut_x, g_omap_l2pq_lut_a, g_omap_l2pq_lut_b };
    hi_u32 lut_len2[] = {128, 128, 128};
    hi_u32 coef_bit_len2[] = {32, 28, 16};

    hi_u32 j;
    hi_u32 idx_r, idx_g, idx_b;
    cm_coef lut_cm_coef;
    void *p_coef_array[1];

    struct file *fp_cm_coef = NULL;
    static hi_u16 lut_eq_0[100] = { 0 }; /* 100 : lut length */
    static hi_u16 lut_eq_1[100] = { 0 }; /* 100 : lut length */

    void *p_coef_array3[11]; /* 11 : coef array length */
    static hi_u32 lut_length[11] = {  /* 11 : coef array length */
        100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100
    };
    static hi_u32 coef_bit_length[11] = {  /* 11 : coef array length */
        12, 12, 12, 12, 12, 12, 12, 12, 12, 10, 10
    };
    hi_u16 lut_eq_4[64] = { 0 }; /* 64 : coef array length */
    hi_u16 lut_eq_5[64] = { 0 }; /* 64 : coef array length */

    void *p_coef_array2[11]; /* 11 : coef array length */
    static hi_u32 lut_length2[11] = {  /* 11 : coef array length */
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
    };
    static hi_u32 coef_bit_length2[11] = {  /* 11 : coef array length */
        12, 12, 12, 12, 12, 12, 12, 12, 12, 10, 10
    };
    hi_u16 lut_eq_2[80] = { 0 }; /* 80 : lut length */
    hi_u16 lut_eq_3[80] = { 0 }; /* 80 : lut length */

    void *p_coef_array1[11]; /* 11 : coef array length */
    static hi_u32 lut_length1[11] = { /* 11 : coef array length */
        80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80
    };
    static hi_u32 coef_bit_length1[11] = { /* 11 : coef array length */
        12, 12, 12, 12, 12, 12, 12, 12, 12, 10, 10
    };

    PQ_CHECK_NULL_PTR_RE_NULL(addr1);
    PQ_CHECK_NULL_PTR_RE_NULL(addr2);
    PQ_CHECK_NULL_PTR_RE_NULL(addr3);
    PQ_CHECK_NULL_PTR_RE_NULL(g_omap_degamma);
    PQ_CHECK_NULL_PTR_RE_NULL(g_omap_gamma_lut_x);
    PQ_CHECK_NULL_PTR_RE_NULL(g_omap_gamma_lut_a);
    PQ_CHECK_NULL_PTR_RE_NULL(g_omap_gamma_lut_b);

    coef_gen.p_coef = g_omap_degamma;
    coef_gen.p_coef_new = g_omap_depq1024_lut;
    coef_gen.coef_data_type = DRV_COEF_DATA_TYPE_U32;
    coef_gen.length = 1024; /* 1024 : length */
    coef_gen.coef_max = (hi_u32)(((hi_u64)1 << 32) - 1); /* 32 : bit */
    coef_gen.coef_min = 0;
    pq_drv_gen_coef(&coef_gen);

    lut_len = 1024; /* 1024 : length */
    coef_bit_len = 32; /* 32 : bit */

    p_coef_array[0] = g_omap_depq1024_lut;

    coef_send.coef_addr = addr1;
    coef_send.sti_type = PQ_STI_FILE_COEF_DM_DEGAMM;
    coef_send.fp_coef = fp_om_output_degmm_coef;
    coef_send.lut_num = 1;
    coef_send.burst_num = 1;
    coef_send.cycle_num = 4; /* 4 : cycle_num */
    coef_send.p_coef_array = p_coef_array;
    coef_send.lut_length = &lut_len;
    coef_send.coef_bit_length = &coef_bit_len;
    coef_send.data_type = DRV_COEF_DATA_TYPE_U32;
    pq_send_coef2ddr(&coef_send);

    coef_gen.p_coef = g_omap_gamma_lut_x;
    coef_gen.p_coef_new = g_omap_l2p_qlut_x;
    coef_gen.coef_data_type = DRV_COEF_DATA_TYPE_U32;
    coef_gen.length = 128; /* 128 : length */
    coef_gen.coef_max = (hi_u32)(((hi_u64)1 << 32) - 1); /* 32 : bit */
    coef_gen.coef_min = 0;
    pq_drv_gen_coef(&coef_gen);

    coef_gen.p_coef = g_omap_gamma_lut_a;
    coef_gen.p_coef_new = g_omap_l2pq_lut_a;
    coef_gen.coef_data_type = DRV_COEF_DATA_TYPE_U32;
    coef_gen.length = 128; /* 128 : length */
    coef_gen.coef_max = (1 << 28) - 1; /* 28 : bit */
    coef_gen.coef_min = 0;
    pq_drv_gen_coef(&coef_gen);

    coef_gen.p_coef = g_omap_gamma_lut_b;
    coef_gen.p_coef_new = g_omap_l2pq_lut_b;
    coef_gen.coef_data_type = DRV_COEF_DATA_TYPE_U32;
    coef_gen.length = 128; /* 128 : length */
    coef_gen.coef_max = (1 << 16) - 1; /* 16 : bit */
    coef_gen.coef_min = 0;
    pq_drv_gen_coef(&coef_gen);

    coef_send.coef_addr = addr2;
    coef_send.fp_coef = fp_om_output_gmm_coef;
    coef_send.p_coef_array = p_coef_array_l2pq;
    coef_send.sti_type = PQ_STI_FILE_COEF_DM_GAMM;
    coef_send.lut_num = 3; /* 3 : lut_num */
    coef_send.burst_num = 1;
    coef_send.cycle_num = 1;
    coef_send.lut_length = lut_len2;
    coef_send.coef_bit_length = coef_bit_len2;
    coef_send.data_type = DRV_COEF_DATA_TYPE_U32;
    pq_send_coef2ddr(&coef_send);

    /* cbb set h = rand() % 2; but it flicker, so not uer rand */
    for (idx_r = 0; idx_r < LUT_SIZE; idx_r++) {
        for (idx_g = 0; idx_g < LUT_SIZE; idx_g++) {
            for (idx_b = 0; idx_b < LUT_SIZE; idx_b++) {
                if ((idx_r % 2 == 0) && (idx_g % 2 == 0) && (idx_b % 2 == 0)) { /* 2 :num */
                    g_cm_lut_r_3d0[LUT_EVE_SIZE * LUT_EVE_SIZE * ((idx_r + 1) / 2) + /* 2 :num */
                                   LUT_EVE_SIZE * ((idx_g + 1) / 2) + (idx_b + 1) / 2] = /* 2 :num */
                                       g_om_cm_lut_r[idx_r][idx_g][idx_b];
                    g_cm_lut_g_3d0[LUT_EVE_SIZE * LUT_EVE_SIZE * ((idx_r + 1) / 2) + /* 2 :num */
                                   LUT_EVE_SIZE * ((idx_g + 1) / 2) + (idx_b + 1) / 2] = /* 2 :num */
                                       g_om_cm_lut_g[idx_r][idx_g][idx_b];
                    g_cm_lut_b_3d0[LUT_EVE_SIZE * LUT_EVE_SIZE * ((idx_r + 1) / 2) + /* 2 :num */
                                   LUT_EVE_SIZE * ((idx_g + 1) / 2) + (idx_b + 1) / 2] = /* 2 :num */
                                       g_om_cm_lut_b[idx_r][idx_g][idx_b];
                } else if ((idx_r % 2 == 0) && (idx_g % 2 == 0) && (idx_b % 2 == 1)) { /* 2 :num */
                    g_cm_lut_r_3d1[LUT_EVE_SIZE * LUT_ODD_SIZE * ((idx_r + 1) / 2) + /* 2 :num */
                                   LUT_ODD_SIZE * ((idx_g + 1) / 2) + (idx_b + 0) / 2] = /* 2 :num */
                                       g_om_cm_lut_r[idx_r][idx_g][idx_b];
                    g_cm_lut_g_3d1[LUT_EVE_SIZE * LUT_ODD_SIZE * ((idx_r + 1) / 2) + /* 2 :num */
                                   LUT_ODD_SIZE * ((idx_g + 1) / 2) + (idx_b + 0) / 2] = /* 2 :num */
                                       g_om_cm_lut_g[idx_r][idx_g][idx_b];
                    g_cm_lut_b_3d1[LUT_EVE_SIZE * LUT_ODD_SIZE * ((idx_r + 1) / 2) + /* 2 :num */
                                   LUT_ODD_SIZE * ((idx_g + 1) / 2) + (idx_b + 0) / 2] = /* 2 :num */
                                       g_om_cm_lut_b[idx_r][idx_g][idx_b];
                } else if ((idx_r % 2 == 0) && (idx_g % 2 == 1) && (idx_b % 2 == 0)) { /* 2 :num */
                    g_cm_lut_r_3d4[LUT_EVE_SIZE * LUT_ODD_SIZE * ((idx_r + 1) / 2) + /* 2 :num */
                                   LUT_EVE_SIZE * ((idx_g + 0) / 2) + (idx_b + 1) / 2] = /* 2 :num */
                                       g_om_cm_lut_r[idx_r][idx_g][idx_b];
                    g_cm_lut_g_3d4[LUT_EVE_SIZE * LUT_ODD_SIZE * ((idx_r + 1) / 2) + /* 2 :num */
                                   LUT_EVE_SIZE * ((idx_g + 0) / 2) + (idx_b + 1) / 2] = /* 2 :num */
                                       g_om_cm_lut_g[idx_r][idx_g][idx_b];
                    g_cm_lut_b_3d4[LUT_EVE_SIZE * LUT_ODD_SIZE * ((idx_r + 1) / 2) + /* 2 :num */
                                   LUT_EVE_SIZE * ((idx_g + 0) / 2) + (idx_b + 1) / 2] = /* 2 :num */
                                       g_om_cm_lut_b[idx_r][idx_g][idx_b];
                } else if ((idx_r % 2 == 0) && (idx_g % 2 == 1) && (idx_b % 2 == 1)) { /* 2 :num */
                    g_cm_lut_r_3d5[LUT_ODD_SIZE * LUT_ODD_SIZE * ((idx_r + 1) / 2) + /* 2 :num */
                                   LUT_ODD_SIZE * ((idx_g + 0) / 2) + (idx_b + 0) / 2] = /* 2 :num */
                                       g_om_cm_lut_r[idx_r][idx_g][idx_b];
                    g_cm_lut_g_3d5[LUT_ODD_SIZE * LUT_ODD_SIZE * ((idx_r + 1) / 2) + /* 2 :num */
                                   LUT_ODD_SIZE * ((idx_g + 0) / 2) + (idx_b + 0) / 2] = /* 2 :num */
                                       g_om_cm_lut_g[idx_r][idx_g][idx_b];
                    g_cm_lut_b_3d5[LUT_ODD_SIZE * LUT_ODD_SIZE * ((idx_r + 1) / 2) + /* 2 :num */
                                   LUT_ODD_SIZE * ((idx_g + 0) / 2) + (idx_b + 0) / 2] = /* 2 :num */
                                       g_om_cm_lut_b[idx_r][idx_g][idx_b];
                } else if ((idx_r % 2 == 1) && (idx_g % 2 == 0) && (idx_b % 2 == 0)) { /* 2 :num */
                    g_cm_lut_r_3d2[LUT_EVE_SIZE * LUT_EVE_SIZE * ((idx_r + 0) / 2) + /* 2 :num */
                                   LUT_EVE_SIZE * ((idx_g + 1) / 2) + (idx_b + 1) / 2] = /* 2 :num */
                                       g_om_cm_lut_r[idx_r][idx_g][idx_b];
                    g_cm_lut_g_3d2[LUT_EVE_SIZE * LUT_EVE_SIZE * ((idx_r + 0) / 2) + /* 2 :num */
                                   LUT_EVE_SIZE * ((idx_g + 1) / 2) + (idx_b + 1) / 2] = /* 2 :num */
                                       g_om_cm_lut_g[idx_r][idx_g][idx_b];
                    g_cm_lut_b_3d2[LUT_EVE_SIZE * LUT_EVE_SIZE * ((idx_r + 0) / 2) + /* 2 :num */
                                   LUT_EVE_SIZE * ((idx_g + 1) / 2) + (idx_b + 1) / 2] = /* 2 :num */
                                       g_om_cm_lut_b[idx_r][idx_g][idx_b];
                } else if ((idx_r % 2 == 1) && (idx_g % 2 == 0) && (idx_b % 2 == 1)) { /* 2 :num */
                    g_cm_lut_r_3d3[LUT_EVE_SIZE * LUT_ODD_SIZE * ((idx_r + 0) / 2) + /* 2 :num */
                                   LUT_ODD_SIZE * ((idx_g + 1) / 2) + (idx_b + 0) / 2] = /* 2 :num */
                                       g_om_cm_lut_r[idx_r][idx_g][idx_b];
                    g_cm_lut_g_3d3[LUT_EVE_SIZE * LUT_ODD_SIZE * ((idx_r + 0) / 2) + /* 2 :num */
                                   LUT_ODD_SIZE * ((idx_g + 1) / 2) + (idx_b + 0) / 2] = /* 2 :num */
                                       g_om_cm_lut_g[idx_r][idx_g][idx_b];
                    g_cm_lut_b_3d3[LUT_EVE_SIZE * LUT_ODD_SIZE * ((idx_r + 0) / 2) + /* 2 :num */
                                   LUT_ODD_SIZE * ((idx_g + 1) / 2) + (idx_b + 0) / 2] = /* 2 :num */
                                       g_om_cm_lut_b[idx_r][idx_g][idx_b];
                } else if ((idx_r % 2 == 1) && (idx_g % 2 == 1) && (idx_b % 2 == 0)) { /* 2 :num */
                    g_cm_lut_r_3d6[LUT_EVE_SIZE * LUT_ODD_SIZE * ((idx_r + 0) / 2) + /* 2 :num */
                                   LUT_EVE_SIZE * ((idx_g + 0) / 2) + (idx_b + 1) / 2] = /* 2 :num */
                                       g_om_cm_lut_r[idx_r][idx_g][idx_b];
                    g_cm_lut_g_3d6[LUT_EVE_SIZE * LUT_ODD_SIZE * ((idx_r + 0) / 2) + /* 2 :num */
                                   LUT_EVE_SIZE * ((idx_g + 0) / 2) + (idx_b + 1) / 2] = /* 2 :num */
                                       g_om_cm_lut_g[idx_r][idx_g][idx_b];
                    g_cm_lut_b_3d6[LUT_EVE_SIZE * LUT_ODD_SIZE * ((idx_r + 0) / 2) + /* 2 :num */
                                   LUT_EVE_SIZE * ((idx_g + 0) / 2) + (idx_b + 1) / 2] = /* 2 :num */
                                       g_om_cm_lut_b[idx_r][idx_g][idx_b];
                } else if ((idx_r % 2 == 1) && (idx_g % 2 == 1) && (idx_b % 2 == 1)) { /* 2 :num */
                    g_cm_lut_r_3d7[LUT_ODD_SIZE * LUT_ODD_SIZE * ((idx_r + 0) / 2) + /* 2 :num */
                                   LUT_ODD_SIZE * ((idx_g + 0) / 2) + (idx_b + 0) / 2] = /* 2 :num */
                                       g_om_cm_lut_r[idx_r][idx_g][idx_b];
                    g_cm_lut_g_3d7[LUT_ODD_SIZE * LUT_ODD_SIZE * ((idx_r + 0) / 2) + /* 2 :num */
                                   LUT_ODD_SIZE * ((idx_g + 0) / 2) + (idx_b + 0) / 2] = /* 2 :num */
                                       g_om_cm_lut_g[idx_r][idx_g][idx_b];
                    g_cm_lut_b_3d7[LUT_ODD_SIZE * LUT_ODD_SIZE * ((idx_r + 0) / 2) + /* 2 :num */
                                   LUT_ODD_SIZE * ((idx_g + 0) / 2) + (idx_b + 0) / 2] = /* 2 :num */
                                       g_om_cm_lut_b[idx_r][idx_g][idx_b];
                }
            }
        }
    }

    g_lut00 = g_lut00_r;
    g_lut01 = g_lut01_r;
    g_lut1 = g_lut1_r;
    g_lut2 = g_lut2_r;
    g_lut3 = g_lut3_r;
    g_lut4 = g_lut4_r;
    g_lut5 = g_lut5_r;
    g_lut6 = g_lut6_r;
    g_lut7 = g_lut7_r;

    for (j = 0; j < 125; j++) {  /* 125: lut_num */
        if (j % 2 == 0) { /* 2: num */
            *g_lut00 = g_cm_lut_r_3d0[j];
            g_lut00++;
        } else {
            *g_lut01 = g_cm_lut_r_3d0[j];
            g_lut01++;
        }
    }
    for (j = 0; j < 100; j++) { /* 100: lut lenght */
        *g_lut1 = g_cm_lut_r_3d1[j];
        g_lut1++;
    }
    for (j = 0; j < 100; j++) { /* 100: lut lenght */
        *g_lut2 = g_cm_lut_r_3d2[j];
        g_lut2++;
    }
    for (j = 0; j < 80; j++) { /* 80: lut lenght */
        *g_lut3 = g_cm_lut_r_3d3[j];
        g_lut3++;
    }
    for (j = 0; j < 100; j++) { /* 100: lut lenght */
        *g_lut4 = g_cm_lut_r_3d4[j];
        g_lut4++;
    }
    for (j = 0; j < 80; j++) { /* 80: lut lenght */
        *g_lut5 = g_cm_lut_r_3d5[j];
        g_lut5++;
    }
    for (j = 0; j < 80; j++) { /* 80: lut lenght */
        *g_lut6 = g_cm_lut_r_3d6[j];
        g_lut6++;
    }
    for (j = 0; j < 64; j++) { /* 64: lut lenght */
        *g_lut7 = g_cm_lut_r_3d7[j];
        g_lut7++;
    }

    g_lut00 = g_lut00_g;
    g_lut01 = g_lut01_g;
    g_lut1 = g_lut1_g;
    g_lut2 = g_lut2_g;
    g_lut3 = g_lut3_g;
    g_lut4 = g_lut4_g;
    g_lut5 = g_lut5_g;
    g_lut6 = g_lut6_g;
    g_lut7 = g_lut7_g;

    for (j = 0; j < 125; j++) { /* 125: lut lenght */
        if (j % 2 == 0) { /* 2: num */
            *g_lut00 = g_cm_lut_g_3d0[j];
            g_lut00++;
        } else {
            *g_lut01 = g_cm_lut_g_3d0[j];
            g_lut01++;
        }
    }
    for (j = 0; j < 100; j++) { /* 100: lut lenght */
        *g_lut1 = g_cm_lut_g_3d1[j];
        g_lut1++;
    }
    for (j = 0; j < 100; j++) { /* 100: lut lenght */
        *g_lut2 = g_cm_lut_g_3d2[j];
        g_lut2++;
    }
    for (j = 0; j < 80; j++) { /* 80: lut lenght */
        *g_lut3 = g_cm_lut_g_3d3[j];
        g_lut3++;
    }
    for (j = 0; j < 100; j++) { /* 100: lut lenght */
        *g_lut4 = g_cm_lut_g_3d4[j];
        g_lut4++;
    }
    for (j = 0; j < 80; j++) { /* 80: lut lenght */
        *g_lut5 = g_cm_lut_g_3d5[j];
        g_lut5++;
    }
    for (j = 0; j < 80; j++) { /* 80: lut lenght */
        *g_lut6 = g_cm_lut_g_3d6[j];
        g_lut6++;
    }
    for (j = 0; j < 64; j++) { /* 64: lut lenght */
        *g_lut7 = g_cm_lut_g_3d7[j];
        g_lut7++;
    }

    g_lut00 = g_lut00_b;
    g_lut01 = g_lut01_b;
    g_lut1 = g_lut1_b;
    g_lut2 = g_lut2_b;
    g_lut3 = g_lut3_b;
    g_lut4 = g_lut4_b;
    g_lut5 = g_lut5_b;
    g_lut6 = g_lut6_b;
    g_lut7 = g_lut7_b;

    for (j = 0; j < 125; j++) { /* 125: lut lenght */
        if (j % 2 == 0) { /* 2: num */
            *g_lut00 = g_cm_lut_b_3d0[j];
            g_lut00++;
        } else {
            *g_lut01 = g_cm_lut_b_3d0[j];
            g_lut01++;
        }
    }
    for (j = 0; j < 100; j++) { /* 100: lut lenght */
        *g_lut1 = g_cm_lut_b_3d1[j];
        g_lut1++;
    }
    for (j = 0; j < 100; j++) { /* 100: lut lenght */
        *g_lut2 = g_cm_lut_b_3d2[j];
        g_lut2++;
    }
    for (j = 0; j < 80; j++) { /* 80: lut lenght */
        *g_lut3 = g_cm_lut_b_3d3[j];
        g_lut3++;
    }
    for (j = 0; j < 100; j++) { /* 100: lut lenght */
        *g_lut4 = g_cm_lut_b_3d4[j];
        g_lut4++;
    }
    for (j = 0; j < 80; j++) { /* 80: lut lenght */
        *g_lut5 = g_cm_lut_b_3d5[j];
        g_lut5++;
    }
    for (j = 0; j < 80; j++) { /* 80: lut lenght */
        *g_lut6 = g_cm_lut_b_3d6[j];
        g_lut6++;
    }
    for (j = 0; j < 64; j++) { /* 64: lut lenght */
        *g_lut7 = g_cm_lut_b_3d7[j];
        g_lut7++;
    }

    lut_cm_coef.lut00_r = g_lut00_r;
    lut_cm_coef.lut00_g = g_lut00_g;
    lut_cm_coef.lut00_b = g_lut00_b;
    lut_cm_coef.lut01_r = g_lut01_r;
    lut_cm_coef.lut01_g = g_lut01_g;
    lut_cm_coef.lut01_b = g_lut01_b;
    lut_cm_coef.lut1_r = g_lut1_r;
    lut_cm_coef.lut1_g = g_lut1_g;
    lut_cm_coef.lut1_b = g_lut1_b;
    lut_cm_coef.lut2_r = g_lut2_r;
    lut_cm_coef.lut2_g = g_lut2_g;
    lut_cm_coef.lut2_b = g_lut2_b;
    lut_cm_coef.lut3_r = g_lut3_r;
    lut_cm_coef.lut3_g = g_lut3_g;
    lut_cm_coef.lut3_b = g_lut3_b;
    lut_cm_coef.lut4_r = g_lut4_r;
    lut_cm_coef.lut4_g = g_lut4_g;
    lut_cm_coef.lut4_b = g_lut4_b;
    lut_cm_coef.lut5_r = g_lut5_r;
    lut_cm_coef.lut5_g = g_lut5_g;
    lut_cm_coef.lut5_b = g_lut5_b;
    lut_cm_coef.lut6_r = g_lut6_r;
    lut_cm_coef.lut6_g = g_lut6_g;
    lut_cm_coef.lut6_b = g_lut6_b;
    lut_cm_coef.lut7_r = g_lut7_r;
    lut_cm_coef.lut7_g = g_lut7_g;
    lut_cm_coef.lut7_b = g_lut7_b;

    p_coef_array3[0] = lut_cm_coef.lut1_b;
    p_coef_array3[1] = lut_cm_coef.lut1_g;
    p_coef_array3[2] = lut_cm_coef.lut1_r; /* 2: index */
    p_coef_array3[3] = lut_cm_coef.lut2_b; /* 3: index */
    p_coef_array3[4] = lut_cm_coef.lut2_g; /* 4: index */
    p_coef_array3[5] = lut_cm_coef.lut2_r; /* 5: index */
    p_coef_array3[6] = lut_cm_coef.lut4_b; /* 6: index */
    p_coef_array3[7] = lut_cm_coef.lut4_g; /* 7: index */
    p_coef_array3[8] = lut_cm_coef.lut4_r; /* 8: index */
    p_coef_array3[9] = lut_eq_1; /* 9: index */
    p_coef_array3[10] = lut_eq_0; /* 10: index */

    p_coef_array2[0] = lut_cm_coef.lut00_b;
    p_coef_array2[1] = lut_cm_coef.lut00_g;
    p_coef_array2[2] = lut_cm_coef.lut00_r; /* 2: index */
    p_coef_array2[3] = lut_cm_coef.lut01_b; /* 3: index */
    p_coef_array2[4] = lut_cm_coef.lut01_g; /* 4: index */
    p_coef_array2[5] = lut_cm_coef.lut01_r; /* 5: index */
    p_coef_array2[6] = lut_cm_coef.lut7_b; /* 6: index */
    p_coef_array2[7] = lut_cm_coef.lut7_g; /* 7: index */
    p_coef_array2[8] = lut_cm_coef.lut7_r; /* 8: index */
    p_coef_array2[9] = lut_eq_5; /* 9: index */
    p_coef_array2[10] = lut_eq_4; /* 10: index */

    p_coef_array1[0] = lut_cm_coef.lut3_b;
    p_coef_array1[1] = lut_cm_coef.lut3_g;
    p_coef_array1[2] = lut_cm_coef.lut3_r; /* 2: index */
    p_coef_array1[3] = lut_cm_coef.lut5_b; /* 3: index */
    p_coef_array1[4] = lut_cm_coef.lut5_g; /* 4: index */
    p_coef_array1[5] = lut_cm_coef.lut5_r; /* 5: index */
    p_coef_array1[6] = lut_cm_coef.lut6_b; /* 6: index */
    p_coef_array1[7] = lut_cm_coef.lut6_g; /* 7: index */
    p_coef_array1[8] = lut_cm_coef.lut6_r; /* 8: index */
    p_coef_array1[9] = lut_eq_3; /* 9: index */
    p_coef_array1[10] = lut_eq_2;  /* 10: index */

    coef_send.coef_addr = addr3;
    coef_send.sti_type = PQ_STI_FILE_COEF_CGM;
    coef_send.fp_coef = fp_cm_coef;
    coef_send.lut_num = 11; /* 11: lut_num */
    coef_send.burst_num = 1;
    coef_send.cycle_num = 1;
    coef_send.p_coef_array = p_coef_array3;
    coef_send.lut_length = lut_length;
    coef_send.coef_bit_length = coef_bit_length;
    coef_send.data_type = DRV_COEF_DATA_TYPE_U16;
    addr3 = pq_send_coef2ddr(&coef_send);

    coef_send.coef_addr = addr3;
    coef_send.cycle_num = 1;
    coef_send.p_coef_array = p_coef_array1;
    coef_send.lut_length = lut_length1;
    coef_send.coef_bit_length = coef_bit_length1;
    coef_send.data_type = DRV_COEF_DATA_TYPE_U16;
    addr3 = pq_send_coef2ddr(&coef_send);

    coef_send.coef_addr = addr3;
    coef_send.cycle_num = 1;
    coef_send.p_coef_array = p_coef_array2;
    coef_send.lut_length = lut_length2;
    coef_send.coef_bit_length = coef_bit_length2;
    coef_send.data_type = DRV_COEF_DATA_TYPE_U16;
    pq_send_coef2ddr(&coef_send);

    return;
}

static hi_void pq_hal_get_hdr_coef(pq_hal_hdr_cfg *hdr_cfg)
{
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hdr_set_hdrv1cm_coef2ddr(g_pq_vdp_hdr_coef_buf.vir_addr[PQ_COEF_BUF_V0_HDR1]);
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_hdr_set_imap_coef2ddr(g_pq_vdp_hdr_coef_buf.vir_addr[PQ_COEF_BUF_V0_HDR2],
                                 g_pq_vdp_hdr_coef_buf.vir_addr[PQ_COEF_BUF_V0_HDR3]);
    }
    if (hdr_cfg->imap_cfg.tmap_v1.enable == HI_TRUE) {
        pq_hdr_set_imap_tmpa_v1_coef(g_pq_vdp_hdr_coef_buf.vir_addr[PQ_COEF_BUF_V0_HDR4]);
    }

    if (hdr_cfg->imap_cfg.cacm.enable == HI_TRUE) {
        pq_hal_set_imap_cacm_coef2ddr(g_pq_vdp_hdr_coef_buf.vir_addr[PQ_COEF_BUF_V0_HDR5]);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hdr_set_tmap_coef2ddr(g_pq_vdp_hdr_coef_buf.vir_addr[PQ_COEF_BUF_V0_HDR6]);
    }

    if (hdr_cfg->omap_cfg.enable == HI_TRUE) {
        pq_hdr_set_omap_coef2ddr(g_pq_vdp_hdr_coef_buf.vir_addr[PQ_COEF_BUF_OM_DEGMM],
                                 g_pq_vdp_hdr_coef_buf.vir_addr[PQ_COEF_BUF_OM_GMM],
                                 g_pq_vdp_hdr_coef_buf.vir_addr[PQ_COEF_BUF_OM_CM]);
    }

    hdr_cfg->coef_addr.hdrv1_cm = g_pq_vdp_hdr_coef_buf.phy_addr[PQ_COEF_BUF_V0_HDR1];
    hdr_cfg->coef_addr.imap_degamma_addr = g_pq_vdp_hdr_coef_buf.phy_addr[PQ_COEF_BUF_V0_HDR2];
    hdr_cfg->coef_addr.imap_gamma_addr = g_pq_vdp_hdr_coef_buf.phy_addr[PQ_COEF_BUF_V0_HDR3];
    hdr_cfg->coef_addr.imap_cacm_addr = g_pq_vdp_hdr_coef_buf.phy_addr[PQ_COEF_BUF_V0_HDR5];
    hdr_cfg->coef_addr.imap_tmapv1_addr = g_pq_vdp_hdr_coef_buf.phy_addr[PQ_COEF_BUF_V0_HDR4];

    hdr_cfg->coef_addr.tmapv2_addr = g_pq_vdp_hdr_coef_buf.phy_addr[PQ_COEF_BUF_V0_HDR6];
    hdr_cfg->coef_addr.omap_degamm_addr = g_pq_vdp_hdr_coef_buf.phy_addr[PQ_COEF_BUF_OM_DEGMM];
    hdr_cfg->coef_addr.omap_gamm_addr = g_pq_vdp_hdr_coef_buf.phy_addr[PQ_COEF_BUF_OM_GMM];
    hdr_cfg->coef_addr.omap_cm_addr = g_pq_vdp_hdr_coef_buf.phy_addr[PQ_COEF_BUF_OM_CM];

    return;
}

static hi_s32 pq_hal_get_hdr2sdr_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
                                     pq_hal_hdr_cfg *hdr_cfg)
{
    hdr_cfg->cm_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_TRUE;
    hdr_cfg->omap_cfg.enable = HI_TRUE;

    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_hdrv1_cm_lut = g_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_hal_get_hdr2sdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* yuv2rgb */
        pq_hal_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* cacm */
        pq_hal_get_imap_cacm_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* degamma */
        g_imap_degamma = g_hdr2sdr_imap_degamma;

        /* rgb2lms */
        pq_hal_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* tone_mapping tmap v1 */
        pq_hal_get_tmap_v1_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* gamma */
        g_imap_gamma_lut_x = g_hdr2sdr_imap_gamma_lut_x;
        g_imap_gamma_lut_a = g_hdr2sdr_imap_gamma_lut_a;
        g_imap_gamma_lut_b = g_hdr2sdr_imap_gamma_lut_b;

        /* lms2ipt */
        pq_hal_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_tmap_tm_lut_i = g_hdr2sdr_tmap_tm_lut_i;
        g_tmap_tm_lut_s = g_hdr2sdr_tmap_tm_lut_s;
        g_tmap_sm_lut_i = g_hdr2sdr_tmap_sm_lut_i;
        g_tmap_sm_lut_s = g_hdr2sdr_tmap_sm_lut_s;
        g_tmap_smc_lut = g_hdr2sdr_tmap_smc_lut_s;
    }

    if (hdr_cfg->omap_cfg.enable == HI_TRUE) {
        pq_hal_get_hdr2sdr_omap_base_cfg(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* ipt2lms */
        pq_hal_get_ipt2lms_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* degamma */
        g_omap_degamma = g_hdr2sdr_omap_degamma;

        /* lms2rgb */
        pq_hal_get_lms2rgb_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* gamma */
        g_omap_gamma_lut_x = g_hdr2sdr_omap_gamma_lut_x;
        g_omap_gamma_lut_a = g_hdr2sdr_omap_gamma_lut_a;
        g_omap_gamma_lut_b = g_hdr2sdr_omap_gamma_lut_b;

        /* cacm */
        pq_hal_get_omap_cacm_cfg(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* rgb2yuv */
        pq_hal_get_rgb2yuv_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);
    }

    pq_hal_get_hdr_coef(hdr_cfg);

    return HI_SUCCESS;
}

static hi_void pq_hal_get_hdrv1_mode_tm_om_bypass_cfg(hi_drv_color_descript in_cs,
    hi_drv_color_descript out_cs, pq_hal_hdr_cfg *hdr_cfg);

static hi_s32 pq_hal_get_hdr2hdr_cfg(hi_drv_color_descript in_cs, hi_drv_color_descript out_cs,
                                     pq_hal_hdr_cfg *hdr_cfg)
{
    hdr_cfg->cm_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_TRUE;
    hdr_cfg->omap_cfg.enable = HI_TRUE;

    if (osal_memncmp(&in_cs, sizeof(in_cs), &out_cs, sizeof(out_cs)) == 0) { /* set Bypass Mode */
        /* set Bypass Mode */
        hdr_cfg->cm_cfg.enable = HI_FALSE;
        hdr_cfg->imap_cfg.enable = HI_FALSE;
        hdr_cfg->tmap_cfg.enable = HI_FALSE;
        hdr_cfg->omap_cfg.enable = HI_FALSE;
        return HI_SUCCESS;
    }

    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_hdrv1_cm_lut = g_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_hal_get_hdr2hdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* yuv2rgb */
        pq_hal_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* cacm */
        pq_hal_get_imap_cacm_hdrv1_mode_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* degamma */
        g_imap_degamma = g_degmm_lut_pq_10000;

        /* rgb2lms */
        pq_hal_get_rgb2lms_matrix_hdrv1_mode(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* tone_mapping tmap v1 */
        pq_hal_get_tmap_v1_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* gamma */
        g_imap_gamma_lut_x = g_im_gmm_lut_x;
        g_imap_gamma_lut_a = g_im_gmm_lut_a_pq;
        g_imap_gamma_lut_b = g_im_gmm_lut_b_pq;

        /* lms2ipt */
        pq_hal_get_lms2ipt_matrix_hdrv1_mode(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    pq_hal_get_hdrv1_mode_tm_om_bypass_cfg(in_cs, out_cs, hdr_cfg);

    pq_hal_get_hdr_coef(hdr_cfg);
    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_hdr2hlg_cfg(hi_drv_color_descript in_cs,
                                     hi_drv_color_descript out_cs, pq_hal_hdr_cfg *hdr_cfg)
{
    hdr_cfg->cm_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_TRUE;
    hdr_cfg->omap_cfg.enable = HI_TRUE;

    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_hdrv1_cm_lut = g_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_hal_get_hdr2hlg_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* yuv2rgb */
        pq_hal_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* cacm */
        pq_hal_get_imap_cacm_hdrv1_mode_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* degamma */
        g_imap_degamma = g_degmm_lut_pq_10000;

        /* rgb2lms */
        pq_hal_get_rgb2lms_matrix_hdrv1_mode(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* tone_mapping tmap v1 */
        pq_hal_get_tmap_v1_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* gamma */
        g_imap_gamma_lut_x = g_im_gmm_lut_x;
        g_imap_gamma_lut_a = g_im_gmm_lut_a_hlg;
        g_imap_gamma_lut_b = g_im_gmm_lut_b_hlg;

        /* lms2ipt */
        pq_hal_get_lms2ipt_matrix_hdrv1_mode(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    pq_hal_get_hdrv1_mode_tm_om_bypass_cfg(in_cs, out_cs, hdr_cfg);
    pq_hal_get_hdr_coef(hdr_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_get_hlg2sdr_cfg(hi_drv_color_descript in_cs,
                              hi_drv_color_descript out_cs, pq_hal_hdr_cfg *hdr_cfg)
{
    hdr_cfg->cm_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_TRUE;
    hdr_cfg->omap_cfg.enable = HI_TRUE;

    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_hdrv1_cm_lut = g_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_hal_get_hlg2sdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* yuv2rgb */
        pq_hal_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* cacm */
        pq_hal_get_imap_cacm_hdrv1_mode_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* degamma */
        g_imap_degamma = g_degmm_lut_hlg_10000;

        /* rgb2lms */
        pq_hal_get_rgb2lms_matrix_hdrv1_mode(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* tone_mapping tmap v1 */
        pq_hal_get_tmap_v1_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* gamma */
        g_imap_gamma_lut_x = g_im_gmm_lut_x;
        g_imap_gamma_lut_a = g_im_gmm_lut_a_gmm_22;
        g_imap_gamma_lut_b = g_im_gmm_lut_b_gmm_22;

        /* lms2ipt */
        pq_hal_get_lms2ipt_matrix_hdrv1_mode(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    pq_hal_get_hdrv1_mode_tm_om_bypass_cfg(in_cs, out_cs, hdr_cfg);

    pq_hal_get_hdr_coef(hdr_cfg);
    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_hlg2hdr10_cfg(hi_drv_color_descript in_cs,
                                       hi_drv_color_descript out_cs, pq_hal_hdr_cfg *hdr_cfg)
{
    hdr_cfg->cm_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_TRUE;
    hdr_cfg->omap_cfg.enable = HI_TRUE;

    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_hdrv1_cm_lut = g_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_hal_get_hlg2hdr10_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* yuv2rgb */
        pq_hal_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* cacm */
        pq_hal_get_imap_cacm_hdrv1_mode_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* degamma */
        g_imap_degamma = g_degmm_lut_hlg_10000;

        /* rgb2lms */
        pq_hal_get_rgb2lms_matrix_hdrv1_mode(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* tone_mapping tmap v1 */
        pq_hal_get_tmap_v1_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* gamma */
        g_imap_gamma_lut_x = g_im_gmm_lut_x;
        g_imap_gamma_lut_a = g_im_gmm_lut_a_pq;
        g_imap_gamma_lut_b = g_im_gmm_lut_b_pq;

        /* lms2ipt */
        pq_hal_get_lms2ipt_matrix_hdrv1_mode(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    pq_hal_get_hdrv1_mode_tm_om_bypass_cfg(in_cs, out_cs, hdr_cfg);

    pq_hal_get_hdr_coef(hdr_cfg);
    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_hlg2hlg_cfg(hi_drv_color_descript in_cs,
                                     hi_drv_color_descript out_cs, pq_hal_hdr_cfg *hdr_cfg)
{
    hdr_cfg->cm_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_TRUE;
    hdr_cfg->omap_cfg.enable = HI_TRUE;

    if (osal_memncmp(&in_cs, sizeof(in_cs), &out_cs, sizeof(out_cs)) == 0) { /* set Bypass Mode */
        hdr_cfg->cm_cfg.enable = HI_FALSE;
        hdr_cfg->imap_cfg.enable = HI_FALSE;
        hdr_cfg->tmap_cfg.enable = HI_FALSE;
        hdr_cfg->omap_cfg.enable = HI_FALSE;
        return HI_SUCCESS;
    }

    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_hdrv1_cm_lut = g_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_hal_get_hlg2hlg_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* yuv2rgb */
        pq_hal_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* cacm */
        pq_hal_get_imap_cacm_hdrv1_mode_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* degamma */
        g_imap_degamma = g_degmm_lut_hlg_10000;

        /* rgb2lms */
        pq_hal_get_rgb2lms_matrix_hdrv1_mode(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* tone_mapping tmap v1 */
        pq_hal_get_tmap_v1_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg); /* 确认VDP HDR是否需要使用 */

        /* gamma */
        g_imap_gamma_lut_x = g_im_gmm_lut_x;
        g_imap_gamma_lut_a = g_im_gmm_lut_a_hlg;
        g_imap_gamma_lut_b = g_im_gmm_lut_b_hlg;

        /* lms2ipt */
        pq_hal_get_lms2ipt_matrix_hdrv1_mode(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    pq_hal_get_hdrv1_mode_tm_om_bypass_cfg(in_cs, out_cs, hdr_cfg);

    pq_hal_get_hdr_coef(hdr_cfg);
    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_sdr2hdr_cfg(hi_drv_color_descript in_cs,
                                     hi_drv_color_descript out_cs, pq_hal_hdr_cfg *hdr_cfg)
{
    hdr_cfg->cm_cfg.enable   = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_TRUE;
    hdr_cfg->omap_cfg.enable = HI_TRUE;

    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_hdrv1_cm_lut = g_cm_lut_hdr2sdr;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_hal_get_sdr2hdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* yuv2rgb */
        pq_hal_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* cacm */
        pq_hal_get_imap_cacm_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* degamma */
        g_imap_degamma = g_sdr2hdr_degamma_lut;

        /* rgb2lms */
        pq_hal_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* tone_mapping tmap v1 */
        pq_hal_get_tmap_v1_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg); /* 确认VDP HDR是否需要使用 */

        /* gamma */
        g_imap_gamma_lut_x = g_hdr2sdr_imap_gamma_lut_x;
        g_imap_gamma_lut_a = g_hdr2sdr_imap_gamma_lut_a;
        g_imap_gamma_lut_b = g_hdr2sdr_imap_gamma_lut_b;

        /* lms2ipt */
        pq_hal_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_tmap_tm_lut_i = g_sdr2hdr_tmapv2_tm_lut_i;
        g_tmap_tm_lut_s = g_sdr2hdr_tmapv2_unity;
        g_tmap_sm_lut_i = g_sdr2hdr_tmapv2_sm_lut_i;
        g_tmap_sm_lut_s = g_sdr2hdr_tmapv2_unity;
        g_tmap_smc_lut = g_sdr2hdr_tmapv2_unity;
    }

    if (hdr_cfg->omap_cfg.enable == HI_TRUE) {
        pq_hal_get_sdr2hdr_omap_base_cfg(&hdr_cfg->omap_cfg);

        /* ipt2lms */
        pq_hal_get_ipt2lms_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* degamma */
        g_omap_degamma = g_hdr2sdr_omap_degamma;

        /* lms2rgb */
        pq_hal_get_lms2rgb_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* gamma */
        g_omap_gamma_lut_x = g_sdr2hdr_omap_gamma_lut_x;
        g_omap_gamma_lut_a = g_sdr2hdr_omap_gamma_lut_a;
        g_omap_gamma_lut_b = g_sdr2hdr_omap_gamma_lut_b;

        /* cacm */
        pq_hal_get_omap_cacm_cfg(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* rgb2yuv */
        pq_hal_get_rgb2yuv_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);
    }

    pq_hal_get_hdr_coef(hdr_cfg);

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_sdr2hlg_cfg(hi_drv_color_descript in_cs,
                                     hi_drv_color_descript out_cs, pq_hal_hdr_cfg *hdr_cfg)
{
    hdr_cfg->cm_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_TRUE;
    hdr_cfg->omap_cfg.enable = HI_TRUE;

    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_hdrv1_cm_lut = g_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_hal_get_sdr2hlg_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* yuv2rgb */
        pq_hal_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* cacm */
        pq_hal_get_imap_cacm_hdrv1_mode_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* degamma */
        g_imap_degamma = g_degmm_lut_gmm_24_10000;

        /* rgb2lms */
        pq_hal_get_rgb2lms_matrix_hdrv1_mode(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* tone_mapping tmap v1 */
        pq_hal_get_tmap_v1_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* gamma */
        g_imap_gamma_lut_x = g_im_gmm_lut_x;
        g_imap_gamma_lut_a = g_im_gmm_lut_a_hlg;
        g_imap_gamma_lut_b = g_im_gmm_lut_b_hlg;

        /* lms2ipt */
        pq_hal_get_lms2ipt_matrix_hdrv1_mode(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    pq_hal_get_hdrv1_mode_tm_om_bypass_cfg(in_cs, out_cs, hdr_cfg);

    pq_hal_get_hdr_coef(hdr_cfg);
    return HI_SUCCESS;
}

static hi_void pq_hal_get_sdr2sdr_ajust_enable(hi_drv_color_descript in_cs,
    hi_drv_color_descript out_cs, pq_hal_hdr_cfg *hdr_cfg)
{
    if (osal_memncmp(&in_cs, sizeof(in_cs), &out_cs, sizeof(out_cs)) == 0) { /* set Bypass Mode */
        hdr_cfg->cm_cfg.enable = HI_FALSE;
        hdr_cfg->imap_cfg.enable = HI_FALSE;
        hdr_cfg->tmap_cfg.enable = HI_FALSE;
        hdr_cfg->omap_cfg.enable = HI_FALSE;
    } else if ((in_cs.color_primary != HI_DRV_COLOR_PRIMARY_BT2020) &&
               (in_cs.matrix_coef != HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT)
               && (in_cs.matrix_coef != HI_DRV_COLOR_MATRIX_COEFFS_BT2020_CONSTANT) &&
               (out_cs.color_primary != HI_DRV_COLOR_PRIMARY_BT2020) &&
               (out_cs.matrix_coef != HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT)
               && (out_cs.matrix_coef != HI_DRV_COLOR_MATRIX_COEFFS_BT2020_CONSTANT)) {
        hdr_cfg->cm_cfg.enable = HI_FALSE;
        hdr_cfg->imap_cfg.enable = HI_FALSE;
        hdr_cfg->tmap_cfg.enable = HI_FALSE;
        hdr_cfg->omap_cfg.enable = HI_FALSE;
    }

    return;
}

static hi_s32 pq_hal_get_sdr2sdr_cfg(hi_drv_color_descript in_cs,
                                     hi_drv_color_descript out_cs, pq_hal_hdr_cfg *hdr_cfg)
{
    hdr_cfg->cm_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_TRUE;
    hdr_cfg->omap_cfg.enable = HI_TRUE;

    pq_hal_get_sdr2sdr_ajust_enable(in_cs, out_cs, hdr_cfg);

    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_hdrv1_cm_lut = g_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_hal_get_sdr2sdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* yuv2rgb */
        pq_hal_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* cacm */
        pq_hal_get_imap_cacm_hdrv1_mode_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* degamma */
        g_imap_degamma = g_degmm_lut_gmm_22_10000;

        /* rgb2lms */
        pq_hal_get_rgb2lms_matrix_hdrv1_mode(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* tone_mapping tmap v1 */
        pq_hal_get_tmap_v1_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg); /* 确认VDP HDR是否需要使用 */

        /* gamma */
        g_imap_gamma_lut_x = g_im_gmm_lut_x;
        g_imap_gamma_lut_a = g_im_gmm_lut_a_gmm_22;
        g_imap_gamma_lut_b = g_im_gmm_lut_b_gmm_22;

        /* lms2ipt */
        pq_hal_get_lms2ipt_matrix_hdrv1_mode(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    pq_hal_get_hdrv1_mode_tm_om_bypass_cfg(in_cs, out_cs, hdr_cfg);

    pq_hal_get_hdr_coef(hdr_cfg);
    return HI_SUCCESS;
}

static hi_void pq_hal_get_hdrv1_mode_tm_om_bypass_cfg(hi_drv_color_descript in_cs,
    hi_drv_color_descript out_cs, pq_hal_hdr_cfg *hdr_cfg)
{
    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_tmap_tm_lut_i = g_tm_lut_i_bypass;
        g_tmap_tm_lut_s = g_sm_lut_bypass;
        g_tmap_sm_lut_i = g_sm_lut_bypass;
        g_tmap_sm_lut_s = g_sm_lut_bypass;
        g_tmap_smc_lut = g_sm_lut_bypass;
    }

    if (hdr_cfg->omap_cfg.enable == HI_TRUE) {
        pq_hal_get_hdrv1_bypass_omap_base_cfg(&hdr_cfg->omap_cfg);

        /* ipt2lms */
        pq_hal_get_ipt2lms_matrix_hdrv1_mode(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* degamma */
        g_omap_degamma = g_hdr2sdr_omap_degamma;  // bypass

        /* lms2rgb */
        pq_hal_get_lms2rgb_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);  // bypass

        /* gamma */
        g_omap_gamma_lut_x = g_hdr2sdr_omap_gamma_lut_x;  // bypass
        g_omap_gamma_lut_a = g_hdr2sdr_omap_gamma_lut_a;
        g_omap_gamma_lut_b = g_hdr2sdr_omap_gamma_lut_b;

        /* cacm */
        pq_hal_get_omap_cacm_cfg(in_cs, out_cs, &hdr_cfg->omap_cfg);  // bypass

        /* rgb2yuv */
        pq_hal_get_rgb2yuv_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);
    }

    return;
}

static hi_s32 pq_hal_get_hdr10plus2hdr10plus_cfg(hi_drv_color_descript in_cs,
    hi_drv_color_descript out_cs, pq_hal_hdr_cfg *hdr_cfg)
{
    hdr_cfg->cm_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_TRUE;
    hdr_cfg->omap_cfg.enable = HI_TRUE;

    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_hdrv1_cm_lut = g_cm_lut_hdr2sdr;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_hal_get_hdr10plus2hdr10plus_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* yuv2rgb */
        pq_hal_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* cacm */
        pq_hal_get_imap_cacm_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* degamma */
        g_imap_degamma = g_hdr2sdr_imap_degamma;

        /* rgb2lms */
        pq_hal_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* tone_mapping tmap v1 */
        pq_hal_get_tmap_v1_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg); /* 确认VDP HDR是否需要使用 */

        /* gamma */
        g_imap_gamma_lut_x = g_hdr2sdr_imap_gamma_lut_x;
        g_imap_gamma_lut_a = g_hdr2sdr_imap_gamma_lut_a;
        g_imap_gamma_lut_b = g_hdr2sdr_imap_gamma_lut_b;

        /* lms2ipt */
        pq_hal_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_tmap_tm_lut_i = g_hdr2sdr_tmap_tm_lut_i;
        g_tmap_tm_lut_s = g_hdr2sdr_tmap_tm_lut_s;
        g_tmap_sm_lut_i = g_hdr2sdr_tmap_sm_lut_i;
        g_tmap_sm_lut_s = g_hdr2sdr_tmap_sm_lut_s;
        g_tmap_smc_lut = g_hdr2sdr_tmap_smc_lut_s;
    }

    if (hdr_cfg->omap_cfg.enable == HI_TRUE) {
        pq_hal_get_hdr10plus2hdr10plus_omap_base_cfg(&hdr_cfg->omap_cfg);

        /* ipt2lms */
        pq_hal_get_ipt2lms_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* degamma */
        g_omap_degamma = g_hdr2sdr_omap_degamma;

        /* lms2rgb */
        pq_hal_get_lms2rgb_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* gamma */
        g_omap_gamma_lut_x = g_hdr2sdr_omap_gamma_lut_x;
        g_omap_gamma_lut_a = g_hdr2sdr_omap_gamma_lut_a;
        g_omap_gamma_lut_b = g_hdr2sdr_omap_gamma_lut_b;

        /* cacm */
        pq_hal_get_omap_cacm_cfg(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* rgb2yuv */
        pq_hal_get_rgb2yuv_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);
    }

    pq_hal_get_hdr_coef(hdr_cfg);
    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_hdr10plus2sdr_cfg(hi_drv_color_descript in_cs,
    hi_drv_color_descript out_cs, pq_hal_hdr_cfg *hdr_cfg)
{
    hdr_cfg->cm_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_TRUE;
    hdr_cfg->omap_cfg.enable = HI_TRUE;

    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_hdrv1_cm_lut = g_cm_lut_hdr2sdr;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_hal_get_hdr10plus2sdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* yuv2rgb */
        pq_hal_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* cacm */
        pq_hal_get_imap_cacm_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* degamma */
        g_imap_degamma = g_hdr2sdr_imap_degamma;

        /* rgb2lms */
        pq_hal_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* tone_mapping tmap v1 */
        pq_hal_get_tmap_v1_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg); /* 确认VDP HDR是否需要使用 */

        /* gamma */
        g_imap_gamma_lut_x = g_hdr2sdr_imap_gamma_lut_x;
        g_imap_gamma_lut_a = g_hdr2sdr_imap_gamma_lut_a;
        g_imap_gamma_lut_b = g_hdr2sdr_imap_gamma_lut_b;

        /* lms2ipt */
        pq_hal_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_tmap_tm_lut_i = g_hdr2sdr_tmap_tm_lut_i;
        g_tmap_tm_lut_s = g_hdr2sdr_tmap_tm_lut_s;
        g_tmap_sm_lut_i = g_hdr2sdr_tmap_sm_lut_i;
        g_tmap_sm_lut_s = g_hdr2sdr_tmap_sm_lut_s;
        g_tmap_smc_lut = g_hdr2sdr_tmap_smc_lut_s;
    }

    if (hdr_cfg->omap_cfg.enable == HI_TRUE) {
        pq_hal_get_hdr10plus2sdr_omap_base_cfg(&hdr_cfg->omap_cfg);

        /* ipt2lms */
        pq_hal_get_ipt2lms_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* degamma */
        g_omap_degamma = g_hdr2sdr_omap_degamma;

        /* lms2rgb */
        pq_hal_get_lms2rgb_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* gamma */
        g_omap_gamma_lut_x = g_hdr2sdr_omap_gamma_lut_x;
        g_omap_gamma_lut_a = g_hdr2sdr_omap_gamma_lut_a;
        g_omap_gamma_lut_b = g_hdr2sdr_omap_gamma_lut_b;

        /* cacm */
        pq_hal_get_omap_cacm_cfg(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* rgb2yuv */
        pq_hal_get_rgb2yuv_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);
    }

    pq_hal_get_hdr_coef(hdr_cfg);
    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_hdr10plus2hdr10_cfg(hi_drv_color_descript in_cs,
    hi_drv_color_descript out_cs, pq_hal_hdr_cfg *hdr_cfg)
{
    hdr_cfg->cm_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_TRUE;
    hdr_cfg->omap_cfg.enable = HI_TRUE;

    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_hdrv1_cm_lut = g_cm_lut_hdr2sdr;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_hal_get_hdr10plus2hdr10_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* yuv2rgb */
        pq_hal_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* cacm */
        pq_hal_get_imap_cacm_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* degamma */
        g_imap_degamma = g_hdr2sdr_imap_degamma;

        /* rgb2lms */
        pq_hal_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* tone_mapping tmap v1 */
        pq_hal_get_tmap_v1_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg); /* 确认VDP HDR是否需要使用 */

        /* gamma */
        g_imap_gamma_lut_x = g_hdr2sdr_imap_gamma_lut_x;
        g_imap_gamma_lut_a = g_hdr2sdr_imap_gamma_lut_a;
        g_imap_gamma_lut_b = g_hdr2sdr_imap_gamma_lut_b;

        /* lms2ipt */
        pq_hal_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_tmap_tm_lut_i = g_hdr2sdr_tmap_tm_lut_i;
        g_tmap_tm_lut_s = g_hdr2sdr_tmap_tm_lut_s;
        g_tmap_sm_lut_i = g_hdr2sdr_tmap_sm_lut_i;
        g_tmap_sm_lut_s = g_hdr2sdr_tmap_sm_lut_s;
        g_tmap_smc_lut = g_hdr2sdr_tmap_smc_lut_s;
    }

    if (hdr_cfg->omap_cfg.enable == HI_TRUE) {
        pq_hal_get_hdr10plus2hdr10_omap_base_cfg(&hdr_cfg->omap_cfg);

        /* ipt2lms */
        pq_hal_get_ipt2lms_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* degamma */
        g_omap_degamma = g_hdr2sdr_omap_degamma;

        /* lms2rgb */
        pq_hal_get_lms2rgb_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* gamma */
        g_omap_gamma_lut_x = g_hdr2sdr_omap_gamma_lut_x;
        g_omap_gamma_lut_a = g_hdr2sdr_omap_gamma_lut_a;
        g_omap_gamma_lut_b = g_hdr2sdr_omap_gamma_lut_b;

        /* cacm */
        pq_hal_get_omap_cacm_cfg(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* rgb2yuv */
        pq_hal_get_rgb2yuv_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);
    }

    pq_hal_get_hdr_coef(hdr_cfg);
    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_hdr10plus2hlg_cfg(hi_drv_color_descript in_cs,
    hi_drv_color_descript out_cs, pq_hal_hdr_cfg *hdr_cfg)
{
    hdr_cfg->cm_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_TRUE;
    hdr_cfg->omap_cfg.enable = HI_TRUE;

    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_hdrv1_cm_lut = g_cm_lut_hdr2sdr;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_hal_get_hdr10plus2hlg_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* yuv2rgb */
        pq_hal_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* cacm */
        pq_hal_get_imap_cacm_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* degamma */
        g_imap_degamma = g_hdr2sdr_imap_degamma;

        /* rgb2lms */
        pq_hal_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* tone_mapping tmap v1 */
        pq_hal_get_tmap_v1_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg); /* 确认VDP HDR是否需要使用 */

        /* gamma */
        g_imap_gamma_lut_x = g_hdr2sdr_imap_gamma_lut_x;
        g_imap_gamma_lut_a = g_hdr2sdr_imap_gamma_lut_a;
        g_imap_gamma_lut_b = g_hdr2sdr_imap_gamma_lut_b;

        /* lms2ipt */
        pq_hal_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_tmap_tm_lut_i = g_hdr2sdr_tmap_tm_lut_i;
        g_tmap_tm_lut_s = g_hdr2sdr_tmap_tm_lut_s;
        g_tmap_sm_lut_i = g_hdr2sdr_tmap_sm_lut_i;
        g_tmap_sm_lut_s = g_hdr2sdr_tmap_sm_lut_s;
        g_tmap_smc_lut = g_hdr2sdr_tmap_smc_lut_s;
    }

    if (hdr_cfg->omap_cfg.enable == HI_TRUE) {
        pq_hal_get_hdr10plus2hlg_omap_base_cfg(&hdr_cfg->omap_cfg);

        /* ipt2lms */
        pq_hal_get_ipt2lms_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* degamma */
        g_omap_degamma = g_hdr2sdr_omap_degamma;

        /* lms2rgb */
        pq_hal_get_lms2rgb_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* gamma */
        g_omap_gamma_lut_x = g_hdr2sdr_omap_gamma_lut_x;
        g_omap_gamma_lut_a = g_hdr2sdr_omap_gamma_lut_a;
        g_omap_gamma_lut_b = g_hdr2sdr_omap_gamma_lut_b;

        /* cacm */
        pq_hal_get_omap_cacm_cfg(in_cs, out_cs, &hdr_cfg->omap_cfg);

        /* rgb2yuv */
        pq_hal_get_rgb2yuv_matrix(in_cs, out_cs, &hdr_cfg->omap_cfg);
    }

    pq_hal_get_hdr_coef(hdr_cfg);

    return HI_SUCCESS;
}

pq_hdr_fun g_pf_pq_hal_get_hdr_path_cfg[HI_PQ_HDR_MODE_MAX] = {
    { HI_PQ_HDR_MODE_HDR10_TO_SDR,   pq_hal_get_hdr2sdr_cfg },
    { HI_PQ_HDR_MODE_HDR10_TO_HDR10, pq_hal_get_hdr2hdr_cfg },
    { HI_PQ_HDR_MODE_HDR10_TO_HLG,   pq_hal_get_hdr2hlg_cfg },

    { HI_PQ_HDR_MODE_HLG_TO_SDR,   pq_hal_get_hlg2sdr_cfg },
    { HI_PQ_HDR_MODE_HLG_TO_HDR10, pq_hal_get_hlg2hdr10_cfg },
    { HI_PQ_HDR_MODE_HLG_TO_HLG,   pq_hal_get_hlg2hlg_cfg },

    { HI_PQ_HDR_MODE_SDR_TO_SDR,   pq_hal_get_sdr2sdr_cfg },
    { HI_PQ_HDR_MODE_SDR_TO_HDR10, pq_hal_get_sdr2hdr_cfg },
    { HI_PQ_HDR_MODE_SDR_TO_HLG,   pq_hal_get_sdr2hlg_cfg },

    { HI_PQ_HDR_MODE_HDR10PLUS_TO_HDR10PLUS, pq_hal_get_hdr10plus2hdr10plus_cfg },
    { HI_PQ_HDR_MODE_HDR10PLUS_TO_SDR,       pq_hal_get_hdr10plus2sdr_cfg },
    { HI_PQ_HDR_MODE_HDR10PLUS_TO_HDR10,     pq_hal_get_hdr10plus2hdr10_cfg },
    { HI_PQ_HDR_MODE_HDR10PLUS_TO_HLG,       pq_hal_get_hdr10plus2hlg_cfg },
};

hi_s32 pq_hal_get_sdr_trans_mode(hi_drv_hdr_type disp_hdr, hi_pq_hdr_mode *hdr_mode)
{
    if (disp_hdr == HI_DRV_HDR_TYPE_SDR) {
        *hdr_mode = HI_PQ_HDR_MODE_SDR_TO_SDR;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HDR10) {
        *hdr_mode = HI_PQ_HDR_MODE_SDR_TO_HDR10;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HLG) {
        *hdr_mode = HI_PQ_HDR_MODE_SDR_TO_HLG;
    } else {
        HI_WARN_PQ("Unkown support hdr type, src: %d, disp : %d\n", HI_DRV_HDR_TYPE_SDR, disp_hdr);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_hdr10_trans_mode(hi_drv_hdr_type disp_hdr, hi_pq_hdr_mode *hdr_mode)
{
    if (disp_hdr == HI_DRV_HDR_TYPE_SDR) {
        *hdr_mode = HI_PQ_HDR_MODE_HDR10_TO_SDR;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HDR10) {
        *hdr_mode = HI_PQ_HDR_MODE_HDR10_TO_HDR10;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HLG) {
        *hdr_mode = HI_PQ_HDR_MODE_HDR10_TO_HLG;
    } else {
        HI_WARN_PQ("Unkown support hdr type, src: %d, disp : %d\n", HI_DRV_HDR_TYPE_HDR10, disp_hdr);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_hlg_trans_mode(hi_drv_hdr_type disp_hdr, hi_pq_hdr_mode *hdr_mode)
{
    if (disp_hdr == HI_DRV_HDR_TYPE_SDR) {
        *hdr_mode = HI_PQ_HDR_MODE_HLG_TO_SDR;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HDR10) {
        *hdr_mode = HI_PQ_HDR_MODE_HLG_TO_HDR10;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HLG) {
        *hdr_mode = HI_PQ_HDR_MODE_HLG_TO_HLG;
    } else {
        HI_WARN_PQ("Unkown support hdr type, src: %d, disp : %d\n", HI_DRV_HDR_TYPE_HLG, disp_hdr);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#ifdef HI_HDR10PLUS_SUPPORT
hi_s32 pq_hal_get_hdr10plus_trans_mode(hi_drv_hdr_type disp_hdr, hi_pq_hdr_mode *hdr_mode)
{
    if (disp_hdr == HI_DRV_HDR_TYPE_SDR) {
        *hdr_mode = HI_PQ_HDR_MODE_HDR10PLUS_TO_SDR;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HDR10) {
        *hdr_mode = HI_PQ_HDR_MODE_HDR10PLUS_TO_HDR10;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HLG) {
        *hdr_mode = HI_PQ_HDR_MODE_HDR10PLUS_TO_HLG;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HDR10PLUS) {
        *hdr_mode = HI_PQ_HDR_MODE_HDR10PLUS_TO_HDR10PLUS;
    } else {
        HI_WARN_PQ("Unkown support hdr type, src: %d, disp : %d\n", HI_DRV_HDR_TYPE_HDR10PLUS, disp_hdr);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
#endif

hi_s32 pq_hal_get_hdr_mode(hi_drv_hdr_type src_hdr, hi_drv_hdr_type disp_hdr, hi_pq_hdr_mode *hdr_mode)
{
    if (src_hdr == HI_DRV_HDR_TYPE_SDR) {
        pq_hal_get_sdr_trans_mode(disp_hdr, hdr_mode);
    } else if (src_hdr == HI_DRV_HDR_TYPE_HDR10) {
        pq_hal_get_hdr10_trans_mode(disp_hdr, hdr_mode);
    } else if (src_hdr == HI_DRV_HDR_TYPE_HLG) {
        pq_hal_get_hlg_trans_mode(disp_hdr, hdr_mode);
#ifdef HI_HDR10PLUS_SUPPORT
    } else if (src_hdr == HI_DRV_HDR_TYPE_HDR10PLUS) {
        pq_hal_get_hdr10plus_trans_mode(disp_hdr, hdr_mode);
#endif
    } else {
        HI_WARN_PQ("Unkown support hdr type, src: %d, disp : %d\n", src_hdr, disp_hdr);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_check_hdr_param(hi_drv_pq_xdr_frame_info *xdr_frame_info)
{
    PQ_CHECK_OVER_RANGE_RE_FAIL(xdr_frame_info->src_hdr_type, HI_DRV_HDR_TYPE_MAX);
    PQ_CHECK_OVER_RANGE_RE_FAIL(xdr_frame_info->disp_hdr_type, HI_DRV_HDR_TYPE_MAX);

    if (((xdr_frame_info->color_space_in.color_primary != HI_DRV_COLOR_PRIMARY_BT601_525) &&
         (xdr_frame_info->color_space_in.color_primary != HI_DRV_COLOR_PRIMARY_BT601_625) &&
         (xdr_frame_info->color_space_in.color_primary != HI_DRV_COLOR_PRIMARY_BT709) &&
         (xdr_frame_info->color_space_in.color_primary != HI_DRV_COLOR_PRIMARY_BT2020)) ||
        (xdr_frame_info->color_space_in.color_space >= HI_DRV_COLOR_CS_MAX) ||
        (xdr_frame_info->color_space_in.quantify_range >= HI_DRV_COLOR_RANGE_MAX) ||
        ((xdr_frame_info->color_space_in.matrix_coef != HI_DRV_COLOR_MATRIX_COEFFS_BT601_525) &&
         (xdr_frame_info->color_space_in.matrix_coef != HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) &&
         (xdr_frame_info->color_space_in.matrix_coef != HI_DRV_COLOR_MATRIX_COEFFS_BT709) &&
         (xdr_frame_info->color_space_in.matrix_coef != HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT) &&
         (xdr_frame_info->color_space_in.matrix_coef != HI_DRV_COLOR_MATRIX_COEFFS_BT2020_CONSTANT))) {
        HI_ERR_PQ("csc_in not support,%d, %d,%d,%d !\n", xdr_frame_info->color_space_in.color_primary,
                  xdr_frame_info->color_space_in.color_space,
                  xdr_frame_info->color_space_in.quantify_range,
                  xdr_frame_info->color_space_in.matrix_coef);
        return HI_FALSE;
    }

    if (((xdr_frame_info->color_space_out.color_primary != HI_DRV_COLOR_PRIMARY_BT601_525) &&
         (xdr_frame_info->color_space_out.color_primary != HI_DRV_COLOR_PRIMARY_BT601_625) &&
         (xdr_frame_info->color_space_out.color_primary != HI_DRV_COLOR_PRIMARY_BT709) &&
         (xdr_frame_info->color_space_out.color_primary != HI_DRV_COLOR_PRIMARY_BT2020)) ||
        (xdr_frame_info->color_space_out.color_space >= HI_DRV_COLOR_CS_MAX) ||
        (xdr_frame_info->color_space_out.quantify_range >= HI_DRV_COLOR_RANGE_MAX) ||
        ((xdr_frame_info->color_space_out.matrix_coef != HI_DRV_COLOR_MATRIX_COEFFS_BT601_525) &&
         (xdr_frame_info->color_space_out.matrix_coef != HI_DRV_COLOR_MATRIX_COEFFS_BT601_625) &&
         (xdr_frame_info->color_space_out.matrix_coef != HI_DRV_COLOR_MATRIX_COEFFS_BT709) &&
         (xdr_frame_info->color_space_out.matrix_coef != HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT) &&
         (xdr_frame_info->color_space_out.matrix_coef != HI_DRV_COLOR_MATRIX_COEFFS_BT2020_CONSTANT))) {
        HI_ERR_PQ("csc_out not support,%d, %d,%d,%d !\n", xdr_frame_info->color_space_out.color_primary,
                  xdr_frame_info->color_space_out.color_space,
                  xdr_frame_info->color_space_out.quantify_range,
                  xdr_frame_info->color_space_out.matrix_coef);
        return HI_FALSE;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_hdrcfg(hi_drv_pq_xdr_frame_info *xdr_frame_info, pq_hal_hdr_cfg *hdr_cfg)
{
    hi_s32 ret;
    hi_u32 index;
    hi_pq_hdr_mode hdr_mode = HI_PQ_HDR_MODE_SDR_TO_HDR10;

    ret = pq_hal_check_hdr_param(xdr_frame_info);
    PQ_CHECK_RETURN_SUCCESS(ret);
    g_pg_st_pq_hdr_cfg = hdr_cfg;

    g_dminfo.src_max_pq = xdr_frame_info->hdr_info.hdr10_info.mastering_info.max_display_mastering_luminance;
    g_dminfo.src_min_pq = xdr_frame_info->hdr_info.hdr10_info.mastering_info.min_display_mastering_luminance;

    ret = pq_hal_get_hdr_mode(xdr_frame_info->src_hdr_type, xdr_frame_info->disp_hdr_type, &hdr_mode);
    if (hdr_mode >= HI_PQ_HDR_MODE_MAX) {
        HI_WARN_PQ("not support hdr trans mode! \n");
    }

    g_sg_en_hdr_mode = hdr_mode;

    for (index = 0; index < HI_PQ_HDR_MODE_MAX; index++) {
        if (g_pf_pq_hal_get_hdr_path_cfg[index].hdr_mode == hdr_mode) {
            break;
        }
    }

    if (index < HI_PQ_HDR_MODE_MAX) {
        if (g_pf_pq_hal_get_hdr_path_cfg[index].hdr_func != HI_NULL) {
            ret = g_pf_pq_hal_get_hdr_path_cfg[index].hdr_func(
                      xdr_frame_info->color_space_in, xdr_frame_info->color_space_out, hdr_cfg);
        } else {
            HI_WARN_PQ("hdr_func is NULL! \n");
            return HI_FAILURE;
        }
    } else {
        HI_WARN_PQ("no such hdr mode\n");
        return HI_FAILURE;
    }

    return ret;
}

hi_s32 pq_hal_init_hdr(pq_bin_param *param, hi_bool para_use_table_default)
{
    hi_u32 step_bin_sum0 = 0;
    hi_u32 step_bin_sum1 = 0;
    hi_u32 num_bin_sum0 = 0;
    hi_u32 num_bin_sum1 = 0;
    hi_u32 pos_bin_sum0 = 0;
    hi_u32 pos_bin_sum1 = 0;
    hi_u32 tmap_bin_sum0 = 0;
    hi_u32 tmap_bin_sum1 = 0;

    hi_u32 sm_step_bin_sum0 = 0;
    hi_u32 sm_step_bin_sum1 = 0;
    hi_u32 sm_num_bin_sum0 = 0;
    hi_u32 sm_num_bin_sum1 = 0;
    hi_u32 sm_pos_bin_sum0 = 0;
    hi_u32 sm_pos_bin_sum1 = 0;
    hi_u32 sm_bin_sum0 = 0;
    hi_u32 sm_bin_sum1 = 0;
    hi_u32 i = 0;

    PQ_CHECK_NULL_PTR_RE_FAIL(param);
    g_sg_pst_hdr_bin_para = param;

    /* step 1 : check pq bin data : all zero means no effective para from bin */
    /* check pq bin data */
    for (i = 0; i < 8; i++) { /* 8: tm coef length */
        /* tmap */
        step_bin_sum0 += g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.au32_step[PQ_TOOL_HDR2SDR][i];
        step_bin_sum1 += g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.au32_step[PQ_TOOL_SDR2HDR][i];
        num_bin_sum0 += g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.au32_num[PQ_TOOL_HDR2SDR][i];
        num_bin_sum1 += g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.au32_num[PQ_TOOL_SDR2HDR][i];
        pos_bin_sum0 += g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.au32_pos[PQ_TOOL_HDR2SDR][i];
        pos_bin_sum1 += g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.au32_pos[PQ_TOOL_SDR2HDR][i];
        tmap_bin_sum0 += g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.clut[PQ_TOOL_HDR2SDR][i];
        tmap_bin_sum1 += g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.clut[PQ_TOOL_SDR2HDR][i];
        /* smap */
        sm_step_bin_sum0 += g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.au32_step[PQ_TOOL_HDR2SDR][i];
        sm_step_bin_sum1 += g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.au32_step[PQ_TOOL_SDR2HDR][i];
        sm_num_bin_sum0 += g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.au32_num[PQ_TOOL_HDR2SDR][i];
        sm_num_bin_sum1 += g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.au32_num[PQ_TOOL_SDR2HDR][i];
        sm_pos_bin_sum0 += g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.au32_pos[PQ_TOOL_HDR2SDR][i];
        sm_pos_bin_sum1 += g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.au32_pos[PQ_TOOL_SDR2HDR][i];
        sm_bin_sum0 += g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.clut[PQ_TOOL_HDR2SDR][i];
        sm_bin_sum1 += g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.clut[PQ_TOOL_SDR2HDR][i];
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_tool_set_hdrpara_mode(hi_pq_hdr_para_mode *mode)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(mode);

    g_sg_u32_tool_hdr_tm_mode = mode->hdr_tm_mode;
    g_sg_u32_tool_hdr_sm_mode = mode->hdr_sm_mode;

    return HI_SUCCESS;
}

hi_s32 pq_hal_tool_get_hdrpara_mode(hi_pq_hdr_para_mode *mode)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(mode);

    mode->hdr_tm_mode = g_sg_u32_tool_hdr_tm_mode;
    mode->hdr_sm_mode = g_sg_u32_tool_hdr_sm_mode;

    return HI_SUCCESS;
}

hi_s32 pq_hal_tool_set_hdr_tmap(hi_pq_hdr_tmap *hdr_tmap)
{
    hi_u32 i;

    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_tmap);

    memcpy(&g_tool_hdr_tmap, hdr_tmap, sizeof(g_tool_hdr_tmap));

    /* tool send clut * 2^16 */
    for (i = 0; i < 64; i++) { /* 64: lut length */
        g_tool_hdr_tmap.y_lut[i] = HDR_CLIP3(0, 65535, /* 65535: lut max */
        g_tool_hdr_tmap.y_lut[i] >> (16 - g_sg_au16_tool_tm_scale)); /* 16: scale */
    }

    switch (g_sg_en_hdr_mode) {
        case HI_PQ_HDR_MODE_HDR10_TO_SDR:
            /* tmap clut first para = 0 */
            g_tool_hdr_tmap.y_lut[0] = 0;

            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.au32_step[PQ_TOOL_HDR2SDR], g_tool_hdr_tmap.x_step,
                   sizeof(g_tool_hdr_tmap.x_step));
            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.au32_num[PQ_TOOL_HDR2SDR], g_tool_hdr_tmap.x_num,
                   sizeof(g_tool_hdr_tmap.x_num));
            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.au32_pos[PQ_TOOL_HDR2SDR], g_tool_hdr_tmap.x_pos,
                   sizeof(g_tool_hdr_tmap.x_pos));

            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.clut[PQ_TOOL_HDR2SDR], g_tool_hdr_tmap.y_lut,
                   sizeof(g_tool_hdr_tmap.y_lut));
            break;
        case HI_PQ_HDR_MODE_SDR_TO_HDR10:
            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.au32_step[PQ_TOOL_SDR2HDR], g_tool_hdr_tmap.x_step,
                   sizeof(g_tool_hdr_tmap.x_step));
            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.au32_num[PQ_TOOL_SDR2HDR], g_tool_hdr_tmap.x_num,
                   sizeof(g_tool_hdr_tmap.x_num));
            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.au32_pos[PQ_TOOL_SDR2HDR], g_tool_hdr_tmap.x_pos,
                   sizeof(g_tool_hdr_tmap.x_pos));

            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_tm_coef.clut[PQ_TOOL_SDR2HDR], g_tool_hdr_tmap.y_lut,
                   sizeof(g_tool_hdr_tmap.y_lut));
            break;
        default:
            HI_ERR_PQ("PQ tool only support hdr2_sdr & sdr2_hdr now \n");
            break;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_tool_get_hdr_tmap(hi_pq_hdr_tmap *hdr_tmap)
{
    hi_u32 i;

    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_tmap);

    memcpy(hdr_tmap, &g_tool_hdr_tmap, sizeof(g_tool_hdr_tmap));
    /* tool recieve clut * 2^16 */
    for (i = 0; i < 64; i++) { /* 64: lut length */
        hdr_tmap->y_lut[i] = hdr_tmap->y_lut[i] << (16 - g_sg_au16_tool_tm_scale); /* 16: scale */
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_tool_set_hdr_smap(hi_pq_hdr_smap *hdr_smap)
{
    hi_u32 i;
    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_smap);

    memcpy(&g_tool_hdr_smap, hdr_smap, sizeof(g_tool_hdr_smap));
    for (i = 0; i < 64; i++) { /* 64: lut length */
        g_tool_hdr_smap.y_lut[i] = HDR_CLIP3(0, 65535,  /* 65535: lut max */
                                             hdr_smap->y_lut[i] >> (16 - g_sg_au16_tool_sm_scale)); /* 16: scale */
    }

    switch (g_sg_en_hdr_mode) {
        case HI_PQ_HDR_MODE_HDR10_TO_SDR:
            /* tmap clut first para = 0 */
            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.au32_step[PQ_TOOL_HDR2SDR], g_tool_hdr_smap.x_step,
                   sizeof(g_tool_hdr_smap.x_step));
            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.au32_num[PQ_TOOL_HDR2SDR], g_tool_hdr_smap.x_num,
                   sizeof(g_tool_hdr_smap.x_num));
            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.au32_pos[PQ_TOOL_HDR2SDR], g_tool_hdr_smap.x_pos,
                   sizeof(g_tool_hdr_smap.x_pos));

            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.clut[PQ_TOOL_HDR2SDR], g_tool_hdr_smap.y_lut,
                   sizeof(g_tool_hdr_smap.y_lut));
            break;
        case HI_PQ_HDR_MODE_SDR_TO_HDR10:
            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.au32_step[PQ_TOOL_SDR2HDR], g_tool_hdr_smap.x_step,
                   sizeof(g_tool_hdr_smap.x_step));
            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.au32_num[PQ_TOOL_SDR2HDR], g_tool_hdr_smap.x_num,
                   sizeof(g_tool_hdr_smap.x_num));
            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.au32_pos[PQ_TOOL_SDR2HDR], g_tool_hdr_smap.x_pos,
                   sizeof(g_tool_hdr_smap.x_pos));

            memcpy(g_sg_pst_hdr_bin_para->coef.hdr_sm_coef.clut[PQ_TOOL_SDR2HDR], g_tool_hdr_smap.y_lut,
                   sizeof(g_tool_hdr_smap.y_lut));
            break;
        default:
            HI_ERR_PQ("PQ tool hdr_smap only support hdr2_sdr & sdr2_hdr now \n");
            break;
    }
    return HI_SUCCESS;
}

hi_s32 pq_hal_tool_get_hdr_smap(hi_pq_hdr_smap *hdr_smap)
{
    hi_u32 i;
    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_smap);

    memcpy(hdr_smap, &g_tool_hdr_smap, sizeof(g_tool_hdr_smap));

    /* tool recieve clut * 2^16 */
    for (i = 0; i < 64; i++) { /* 64: lut length */
        hdr_smap->y_lut[i] = g_tool_hdr_smap.y_lut[i] << (16 - g_sg_au16_tool_sm_scale); /* 16: scale */
    }

    return HI_SUCCESS;
}

