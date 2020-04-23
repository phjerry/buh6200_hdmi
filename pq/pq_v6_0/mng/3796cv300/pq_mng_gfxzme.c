/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description:
 * Author: pq
 * Create: 2016-01-1
 */

#include "pq_mng_gfxzme.h"
#include "pq_mng_gfxzme_coef.h"

/* **************************** macro definition ***************************** */
#define VOU_BIT_VALUE(a) (a)

/* 图形层缩放系数只需要亮度系数，色度不需要;
* 水平亮度为8tap8phase，垂直方向是4tap16phase，但为了保持8对齐，
* 逻辑上要求垂直方向调整为8tap16phase
*/
#define GZME_MMU_SIZE     ((64 + 128) * 2 * 2 * 7)
#define SHARPEN_MAX_WIDTH 3840
#define MAX_GP_NUM        3

static hi_bool g_gfx_zme_init_flag = HI_FALSE;
static hi_bool g_gs_b_resume[MAX_GP_NUM] = { HI_FALSE, HI_FALSE, HI_FALSE };
static pq_gzme_mem g_gs_st_gp_zme; /** ZME coef buffer **/
gfx_zme_strategy_proc g_gfx_zme_get_proc = {0};
gfx_zme_strategy_proc g_gfx_zme_set_proc = {0};

/***************************************************************************************
* func          : pq_mng_g_zme_load_coef_h
* description   : c_ncomment: 水平ZME系数 c_nend\n
* param[in]     : hi_void
* retval        : NA
* others:       : NA
***************************************************************************************/
static hi_s32 pq_mng_gzme_load_coef_h(pq_gzme_coef_ratio coef_ratio, hi_u8 *addr)
{
    hi_s16 table_tmp[GZME_HOR_TABLE_SIZE] = { 0 };
    const hi_s16 *ps16_l = HI_NULL;
    hi_s16 *ps16_dst = HI_NULL;
    hi_u32 table_tmp_size;
    hi_u32 i;
    hi_s32 j;

    ps16_l = g_pq_gfx_zme_reduce_coef[coef_ratio][PQ_GZME_COEF_8T8P_LH];
    ps16_dst = table_tmp;

    for (i = 0; i < GZME_PHASE_8; i++) {
        for (j = GZME_TAP_8 - 1; j >= 0; j--) {
            *ps16_dst++ = *(ps16_l + i * GZME_TAP_8 + j);
        }
    }

    table_tmp_size = GZME_HOR_TABLE_SIZE * 2; /* 2: s16 is 2 byte */
    PQ_CHECK_NULL_PTR_RE_FAIL(addr);
    memcpy(addr, table_tmp, table_tmp_size);

    return HI_SUCCESS;
}
/***************************************************************************************
* func          : pq_mng_g_zme_load_coef_v
* description   : c_ncomment: 垂直ZME系数 c_nend\n
* param[in]     : hi_void
* retval        : NA
* others:       : NA
***************************************************************************************/
static hi_s32 pq_mng_gzme_load_coef_v(pq_gzme_coef_ratio coef_ratio, hi_u8 *addr)
{
    hi_s16 table_tmp[GZME_VER_TABLE_SIZE] = { 0 };
    hi_u32 table_tmp_size;
    const hi_s16 *ps16_l = HI_NULL;
    hi_s16 *ps16_dst = HI_NULL;
    hi_u32 i;
    hi_s32 j;

    ps16_l = g_pq_gfx_zme_reduce_coef[coef_ratio][PQ_GZME_COEF_4T16P_LV];
    ps16_dst = table_tmp;

    /* load vertical zme(luma and chroma) coef */
    for (i = 0; i < GZME_PHASE_16; i++) { /* 64 is ver table size, load 4 coef every time */
        for (j = GZME_TAP_8 - 1; j >= 0; j--) {
            *ps16_dst++ = *(ps16_l + i * GZME_TAP_8 + j);
        }
    }

    table_tmp_size = GZME_VER_TABLE_SIZE * 2; /* 2: s16 is 2 byte */
    PQ_CHECK_NULL_PTR_RE_FAIL(addr);
    memcpy(addr, table_tmp, table_tmp_size); /* luma and chroma 2 coef */

    return HI_SUCCESS;
}


/***************************************************************************************
* func          : pq_mng_g_zme_load_sd_coef_h
* description   : c_ncomment: SD水平ZME系数 c_nend\n
* param[in]     : hi_void
* retval        : NA
* others:       : NA
***************************************************************************************/
static hi_s32 pq_mng_gzme_load_sdcoef_h(pq_gzme_coef_ratio coef_ratio, hi_u8 *addr)
{
    hi_s16 table_tmp[GZME_HOR_TABLE_SIZE] = { 0 };
    const hi_s16 *ps16_l = HI_NULL;
    hi_s16 *ps16_dst = HI_NULL;
    hi_u32 table_tmp_size;
    hi_u32 i;
    hi_s32 j;

    PQ_CHECK_OVER_RANGE_RE_FAIL(coef_ratio, PQ_GZME_COEF_RATIO_MAX);
    ps16_l = g_pq_gfx_sd_zme_coef[coef_ratio][PQ_GZME_COEF_8T8P_LH];
    ps16_dst = table_tmp;

    for (i = 0; i < GZME_PHASE_8; i++) {
        for (j = GZME_TAP_8 - 1; j >= 0; j--) {
            *ps16_dst++ = *(ps16_l + i * GZME_TAP_8 + j);
        }
    }

    table_tmp_size = GZME_HOR_TABLE_SIZE * 2; /* 2: s16 is 2 byte */
    PQ_CHECK_NULL_PTR_RE_FAIL(addr);
    memcpy(addr, table_tmp, table_tmp_size);

    return HI_SUCCESS;
}

/***************************************************************************************
* func          : pq_mng_g_zme_load_sd_coef_v
* description   : c_ncomment: SD垂直ZME系数 c_nend\n
* param[in]     : hi_void
* retval        : NA
* others:       : NA
***************************************************************************************/
static hi_s32 pq_mng_gzme_load_sdcoef_v(pq_gzme_coef_ratio coef_ratio, hi_u8 *addr)
{
    hi_s16 table_tmp[GZME_VER_TABLE_SIZE] = { 0 };
    hi_u32 table_tmp_size;
    const hi_s16 *ps16_l = HI_NULL;
    hi_s16 *ps16_dst = HI_NULL;
    hi_u32 i;
    hi_s32 j;

    PQ_CHECK_OVER_RANGE_RE_FAIL(coef_ratio, PQ_GZME_COEF_RATIO_MAX);

    ps16_l = g_pq_gfx_sd_zme_coef[coef_ratio][PQ_GZME_COEF_4T16P_LV];
    ps16_dst = table_tmp;

    /* load vertical zme(luma and chroma) coef */
    for (i = 0; i < GZME_PHASE_16; i++) { /* 64 is ver table size, load 4 coef every time */
        for (j = GZME_TAP_8 - 1; j >= 0; j--) {
            *ps16_dst++ = *(ps16_l + i * GZME_TAP_8 + j);
        }
    }

    table_tmp_size = GZME_VER_TABLE_SIZE * 2; /* 2: s16 is 2 byte */
    PQ_CHECK_NULL_PTR_RE_FAIL(addr);

    memcpy(addr, table_tmp, table_tmp_size); /* luma and chroma 2 coef */

    return HI_SUCCESS;
}

/***************************************************************************************
* func          : pq_mng_g_zme_load_coef_hv
* description   : c_ncomment: load zme coef c_nend\n
* param[in]     : hi_void
* retval        : NA
* others:       : NA
***************************************************************************************/
static hi_void pq_mng_gzme_load_coef_hv(pq_gzme_mem *g_zme_coef_mem)
{
    hi_u8 *cur_addr = 0;
    hi_u32 num_size_h = GZME_HOR_COEF_SIZE * 2;  /* 2: s16 is 2 byte */
    hi_u32 num_size_v = GZME_VER_COEF_SIZE * 2;  /* 2: s16 is 2 byte */
    hi_u64 phy_addr;
    hi_u32 ratio;
    hi_u64 *addr_tmp = NULL;
    hi_s32 ret;

    cur_addr = g_zme_coef_mem->m_buf.vir_addr;
    phy_addr = g_zme_coef_mem->m_buf.phy_addr;
    addr_tmp = &(g_zme_coef_mem->zme_coef_addr.zme_coef_addr_hl_1);

    /* 8 tap 8 phase for horizontal, 4 tap 16 phase for vertical */
    for (ratio = 0; ratio < PQ_GZME_COEF_RATIO_MAX; ratio++) {
        ret = pq_mng_gzme_load_coef_h((pq_gzme_coef_ratio)ratio, cur_addr);
        PQ_CHECK_SUCCESS_RETURN_NULL(ret);
        *addr_tmp++ = phy_addr;
        phy_addr += num_size_h;
        cur_addr += num_size_h;

        ret = pq_mng_gzme_load_coef_v((pq_gzme_coef_ratio)ratio, cur_addr);
        PQ_CHECK_SUCCESS_RETURN_NULL(ret);
        *addr_tmp++ = phy_addr;
        phy_addr += num_size_v;
        cur_addr += num_size_v;
    }

    addr_tmp = &(g_zme_coef_mem->zme_coef_addr.zme_sd_coef_addr_hl_1);
    /* 8 tap 8 phase for horizontal */
    for (ratio = 0; ratio < PQ_GZME_COEF_RATIO_MAX; ratio++) {
        ret = pq_mng_gzme_load_sdcoef_h((pq_gzme_coef_ratio)ratio, cur_addr);
        PQ_CHECK_SUCCESS_RETURN_NULL(ret);
        *addr_tmp++ = phy_addr;
        phy_addr += num_size_h;
        cur_addr += num_size_h;

        /* 4 tap 16 phase for vertical */
        ret = pq_mng_gzme_load_sdcoef_v((pq_gzme_coef_ratio)ratio, cur_addr);
        PQ_CHECK_SUCCESS_RETURN_NULL(ret);
        *addr_tmp++ = phy_addr;
        phy_addr += num_size_v;
        cur_addr += num_size_v;
    }

    return;
}

static hi_void pq_mng_gzme_enable_set(hi_drv_pq_gfx_zme_in *zme_drv_para, hi_drv_pq_gfx_zme_common_out *zme_rtl_para)
{
    PQ_CHECK_NULL_PTR_RE_NULL(zme_drv_para);
    PQ_CHECK_NULL_PTR_RE_NULL(zme_rtl_para);

    /* config zme enable */
    zme_rtl_para->zme_fir_en.zme_h_en =
        (zme_drv_para->zme_width_in == zme_drv_para->zme_width_out) ? HI_FALSE : HI_TRUE;

    if ((zme_drv_para->zme_height_in == zme_drv_para->zme_height_out)
        && (zme_drv_para->zme_frame_in == zme_drv_para->zme_frame_out)) {
        zme_rtl_para->zme_fir_en.zme_v_en = HI_FALSE;
    } else {
        zme_rtl_para->zme_fir_en.zme_v_en = HI_TRUE;
    }

    if (zme_rtl_para->zme_fir_en.zme_h_en == HI_TRUE || zme_rtl_para->zme_fir_en.zme_v_en == HI_TRUE) {
        zme_rtl_para->zme_fir_en.zme_h_en = HI_TRUE;
        zme_rtl_para->zme_fir_en.zme_v_en = HI_TRUE;
    }

    return;
}

static hi_void pq_mng_gzme_ratio_set(hi_drv_pq_gfx_zme_in *zme_drv_para, hi_drv_pq_gfx_zme_common_out *zme_rtl_para)
{
    PQ_CHECK_NULL_PTR_RE_NULL(zme_drv_para);
    PQ_CHECK_NULL_PTR_RE_NULL(zme_rtl_para);

    /* config zme ratio, pay attention: this vertical ratio may not be the real ratio
       logic would calculate the real ratio dci_ording to the input and output frame format. */
    PQ_CHECK_ZERO_RE_NULL(zme_drv_para->zme_width_out);
    zme_rtl_para->zme_ratio.zme_ratio_h = zme_drv_para->zme_width_in * PQ_G_HZME_PRECISION /
                                           zme_drv_para->zme_width_out;
    zme_rtl_para->zme_ratio.zme_ratio_hl = zme_rtl_para->zme_ratio.zme_ratio_h;
    zme_rtl_para->zme_ratio.zme_ratio_hc = zme_rtl_para->zme_ratio.zme_ratio_h;

    PQ_CHECK_ZERO_RE_NULL(zme_drv_para->zme_height_out);
    zme_rtl_para->zme_ratio.zme_ratio_v = zme_drv_para->zme_height_in * PQ_G_VZME_PRECISION /
                                           zme_drv_para->zme_height_out;
    zme_rtl_para->zme_ratio.zme_ratio_vl = zme_rtl_para->zme_ratio.zme_ratio_v;
    zme_rtl_para->zme_ratio.zme_ratio_vc = zme_rtl_para->zme_ratio.zme_ratio_v;

    return;
}

static hi_void pq_mng_gzme_offset_set(hi_drv_pq_gfx_zme_in *zme_drv_para, hi_drv_pq_gfx_zme_common_out *zme_rtl_para)
{
    hi_u32 v_zme_real_ratio;

    PQ_CHECK_NULL_PTR_RE_NULL(zme_drv_para);
    PQ_CHECK_NULL_PTR_RE_NULL(zme_rtl_para);

    /* calculate luma real zme resolution of input and output */
    if ((zme_drv_para->zme_frame_in == HI_DRV_PQ_FRM_FRAME) && (zme_drv_para->zme_frame_out != HI_DRV_PQ_FRM_FRAME)) {
        v_zme_real_ratio = zme_rtl_para->zme_ratio.zme_ratio_v * 2; /* real ratio need multi 2 */
    } else {
        v_zme_real_ratio = zme_rtl_para->zme_ratio.zme_ratio_v;
    }

    /* calculate horizontal zme offset */
    zme_rtl_para->zme_offset.zme_offset_hl = 0;
    zme_rtl_para->zme_offset.zme_offset_hc = 0;

    /* calculate vertical zme offset: offset is the real offset! */
    zme_rtl_para->zme_offset.zme_offset_v_tp = 0;
    if ((zme_drv_para->zme_frame_in != HI_DRV_PQ_FRM_FRAME) &&
        (zme_drv_para->zme_frame_out != HI_DRV_PQ_FRM_FRAME)) {
        zme_rtl_para->zme_offset.zme_offset_v_btm = ((hi_s32)v_zme_real_ratio - PQ_G_VZME_PRECISION) / 2; /* divi 2 */
    } else if ((zme_drv_para->zme_frame_in == HI_DRV_PQ_FRM_FRAME) &&
               (zme_drv_para->zme_frame_out != HI_DRV_PQ_FRM_FRAME)) {
        zme_rtl_para->zme_offset.zme_offset_v_btm = (hi_s32)v_zme_real_ratio / 2;  /* divi 2 */
    } else {
        zme_rtl_para->zme_offset.zme_offset_v_btm = 0;
    }

    return;
}

static hi_void pq_mng_gzme_med_set(hi_drv_pq_gfx_zme_in *zme_drv_para, hi_drv_pq_gfx_zme_common_out *zme_rtl_para)
{
    PQ_CHECK_NULL_PTR_RE_NULL(zme_drv_para);
    PQ_CHECK_NULL_PTR_RE_NULL(zme_rtl_para);

    /* config zme median filter enable: if filter tap > 2 and upscaler, median filter enable */
    if (zme_rtl_para->zme_ratio.zme_ratio_hl < PQ_G_HZME_PRECISION) {
        zme_rtl_para->zme_med_fir_en.zme_med_ha_en = 1;
        zme_rtl_para->zme_med_fir_en.zme_med_hl_en = 1;
        zme_rtl_para->zme_med_fir_en.zme_med_hc_en = 1;
    } else {
        zme_rtl_para->zme_med_fir_en.zme_med_ha_en = 0;
        zme_rtl_para->zme_med_fir_en.zme_med_hl_en = 0;
        zme_rtl_para->zme_med_fir_en.zme_med_hc_en = 0;
    }

    if (zme_rtl_para->zme_ratio.zme_ratio_vl < PQ_G_VZME_PRECISION) {
        zme_rtl_para->zme_med_fir_en.zme_med_va_en = 1;
        zme_rtl_para->zme_med_fir_en.zme_med_vl_en = 1;
        zme_rtl_para->zme_med_fir_en.zme_med_vc_en = 1;
    } else {
        zme_rtl_para->zme_med_fir_en.zme_med_va_en = 0;
        zme_rtl_para->zme_med_fir_en.zme_med_vl_en = 0;
        zme_rtl_para->zme_med_fir_en.zme_med_vc_en = 0;
    }

    return;
}

static hi_void pq_mng_gzme_comn_set(hi_drv_pq_gfx_zme_in *zme_drv_para, hi_drv_pq_gfx_zme_common_out *zme_rtl_para)
{
    PQ_CHECK_NULL_PTR_RE_NULL(zme_drv_para);
    PQ_CHECK_NULL_PTR_RE_NULL(zme_rtl_para);

    pq_mng_gzme_enable_set(zme_drv_para, zme_rtl_para);

    zme_rtl_para->zme_order = HI_DRV_PQ_ZME_VER_HOR;
    pq_mng_gzme_ratio_set(zme_drv_para, zme_rtl_para);
    pq_mng_gzme_offset_set(zme_drv_para, zme_rtl_para);

    /* modified by sdk for opening sharp module 2013/6/22 himedia problem */
    zme_rtl_para->zme_fir_mode.zme_fir_ha = HI_DRV_PQ_ZME_FIR;
    zme_rtl_para->zme_fir_mode.zme_fir_h = HI_DRV_PQ_ZME_FIR;

    /* considering anti-flicker for SD, the situation of 1:1 vertical scaler also need low-pass filter */
    zme_rtl_para->zme_fir_mode.zme_fir_va = HI_DRV_PQ_ZME_FIR;
    zme_rtl_para->zme_fir_mode.zme_fir_v = HI_DRV_PQ_ZME_FIR;

    pq_mng_gzme_med_set(zme_drv_para, zme_rtl_para);

    return;
}

static hi_s32 pq_mng_get_gfx_hlfir_coef(pq_gzme_coef_addr *zme_coef_addr, hi_u32 ratio, hi_u64 *coef_addr)
{
    hi_u32 tmp_ratio;

    PQ_CHECK_ZERO_RE_FAIL(ratio);
    tmp_ratio = GZME_WIDTH_MAX * GZME_WIDTH_MAX / ratio;

    if (tmp_ratio > GZME_WIDTH_MAX) {
        *coef_addr = zme_coef_addr->zme_coef_addr_hl_1;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_85) {
        *coef_addr = zme_coef_addr->zme_coef_addr_hl_e1;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_75) {
        *coef_addr = zme_coef_addr->zme_coef_addr_hl_075;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_50) {
        *coef_addr = zme_coef_addr->zme_coef_addr_hl_05;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_33) {
        *coef_addr = zme_coef_addr->zme_coef_addr_hl_033;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_25) {
        *coef_addr = zme_coef_addr->zme_coef_addr_hl_025;
    } else {
        *coef_addr = zme_coef_addr->zme_coef_addr_hl_0;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_gfx_hcfir_coef(pq_gzme_coef_addr *zme_coef_addr, hi_u32 ratio, hi_u64 *coef_addr)
{
    hi_u32 tmp_ratio;

    PQ_CHECK_ZERO_RE_FAIL(ratio);
    tmp_ratio = GZME_WIDTH_MAX * GZME_WIDTH_MAX / ratio;

    if (tmp_ratio > GZME_WIDTH_MAX) {
        *coef_addr = zme_coef_addr->zme_coef_addr_hc_1;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_85) {
        *coef_addr = zme_coef_addr->zme_coef_addr_hc_e1;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_75) {
        *coef_addr = zme_coef_addr->zme_coef_addr_hc_075;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_50) {
        *coef_addr = zme_coef_addr->zme_coef_addr_hc_05;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_33) {
        *coef_addr = zme_coef_addr->zme_coef_addr_hc_033;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_25) {
        *coef_addr = zme_coef_addr->zme_coef_addr_hc_025;
    } else {
        *coef_addr = zme_coef_addr->zme_coef_addr_hc_0;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_gfx_vlfir_coef(pq_gzme_coef_addr *zme_coef_addr, hi_u32 ratio, hi_u64 *coef_addr)
{
    hi_u32 tmp_ratio;

    PQ_CHECK_ZERO_RE_FAIL(ratio);

    tmp_ratio = GZME_WIDTH_MAX * GZME_WIDTH_MAX / ratio;

    if (tmp_ratio > GZME_WIDTH_MAX) {
        *coef_addr = zme_coef_addr->zme_coef_addr_vl_1;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_85) {
        *coef_addr = zme_coef_addr->zme_coef_addr_vl_e1;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_75) {
        *coef_addr = zme_coef_addr->zme_coef_addr_vl_075;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_50) {
        *coef_addr = zme_coef_addr->zme_coef_addr_vl_05;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_33) {
        *coef_addr = zme_coef_addr->zme_coef_addr_vl_033;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_25) {
        *coef_addr = zme_coef_addr->zme_coef_addr_vl_025;
    } else {
        *coef_addr = zme_coef_addr->zme_coef_addr_vl_0;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_gfx_vcfir_coef(pq_gzme_coef_addr *zme_coef_addr, hi_u32 ratio, hi_u64 *coef_addr)
{
    hi_u32 tmp_ratio;

    PQ_CHECK_ZERO_RE_FAIL(ratio);
    tmp_ratio = GZME_WIDTH_MAX * GZME_WIDTH_MAX / ratio;

    if (tmp_ratio > GZME_WIDTH_MAX) {
        *coef_addr = zme_coef_addr->zme_coef_addr_vc_1;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_85) {
        *coef_addr = zme_coef_addr->zme_coef_addr_vc_e1;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_75) {
        *coef_addr = zme_coef_addr->zme_coef_addr_vc_075;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_50) {
        *coef_addr = zme_coef_addr->zme_coef_addr_vc_05;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_33) {
        *coef_addr = zme_coef_addr->zme_coef_addr_vc_033;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_25) {
        *coef_addr = zme_coef_addr->zme_coef_addr_vc_025;
    } else {
        *coef_addr = zme_coef_addr->zme_coef_addr_vc_0;
    }

    return HI_SUCCESS;
}


static hi_s32 pq_mng_get_gfx_hlfir_sdcoef(pq_gzme_coef_addr *zme_coef_addr, hi_u32 ratio, hi_u64 *coef_addr)
{
    hi_u32 tmp_ratio;

    PQ_CHECK_ZERO_RE_FAIL(ratio);
    tmp_ratio = GZME_WIDTH_MAX * GZME_WIDTH_MAX / ratio;

    if (tmp_ratio > GZME_WIDTH_MAX) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_hl_1;
    } else if (tmp_ratio == GZME_WIDTH_MAX) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_hl_e1;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_75) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_hl_075;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_50) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_hl_05;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_33) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_hl_033;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_25) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_hl_025;
    } else {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_hl_0;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_gfx_vlfir_sdcoef(pq_gzme_coef_addr *zme_coef_addr, hi_u32 ratio, hi_u64 *coef_addr)
{
    hi_u32 tmp_ratio;

    PQ_CHECK_ZERO_RE_FAIL(ratio);
    tmp_ratio = GZME_WIDTH_MAX * GZME_WIDTH_MAX / ratio;

    if (tmp_ratio > GZME_WIDTH_MAX) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_vl_1;
    } else if (tmp_ratio == GZME_WIDTH_MAX) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_vl_e1;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_75) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_vl_075;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_50) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_vl_05;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_33) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_vl_033;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_25) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_vl_025;
    } else {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_vl_0;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_gfx_vcfir_sdcoef(pq_gzme_coef_addr *zme_coef_addr, hi_u32 ratio, hi_u64 *coef_addr)
{
    hi_u32 tmp_ratio;

    PQ_CHECK_ZERO_RE_FAIL(ratio);
    tmp_ratio = GZME_WIDTH_MAX * GZME_WIDTH_MAX / ratio;

    if (tmp_ratio > GZME_WIDTH_MAX) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_vc_1;
    } else if (tmp_ratio == GZME_WIDTH_MAX) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_vc_e1;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_75) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_vc_075;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_50) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_vc_05;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_33) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_vc_033;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_25) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_vc_025;
    } else {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_vc_0;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_gfx_hcfir_sdcoef(pq_gzme_coef_addr *zme_coef_addr, hi_u32 ratio, hi_u64 *coef_addr)
{
    hi_u32 tmp_ratio;

    PQ_CHECK_ZERO_RE_FAIL(ratio);

    tmp_ratio = GZME_WIDTH_MAX * GZME_WIDTH_MAX / ratio;

    if (tmp_ratio > GZME_WIDTH_MAX) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_hc_1;
    } else if (tmp_ratio == GZME_WIDTH_MAX) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_hc_e1;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_75) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_hc_075;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_50) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_hc_05;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_33) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_hc_033;
    } else if (tmp_ratio >= GZME_WIDTH_RATIO_25) {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_hc_025;
    } else {
        *coef_addr = zme_coef_addr->zme_sd_coef_addr_hc_0;
    }

    return HI_SUCCESS;
}

static hi_void pq_mng_gzme_hdset(hi_drv_pq_gfx_zme_in *zme_drv_para, hi_drv_pq_gfx_zme_out *zme_rtl_para)
{
    hi_s32 ret;

    ret = pq_mng_get_gfx_hlfir_coef(
              &(g_gs_st_gp_zme.zme_coef_addr),
              (zme_rtl_para->gfx_zme_common_out.zme_ratio.zme_ratio_hl) >> PQ_PRECISION_GZMERATIONH_12BITOFFSET,
              &(zme_rtl_para->zme_coef_addr.zme_coef_hl_addr));
    PQ_CHECK_SUCCESS_RETURN_NULL(ret);

    ret = pq_mng_get_gfx_hcfir_coef(
              &(g_gs_st_gp_zme.zme_coef_addr),
              (zme_rtl_para->gfx_zme_common_out.zme_ratio.zme_ratio_hc) >> PQ_PRECISION_GZMERATIONH_12BITOFFSET,
              &(zme_rtl_para->zme_coef_addr.zme_coef_hc_addr));
    PQ_CHECK_SUCCESS_RETURN_NULL(ret);

    ret = pq_mng_get_gfx_vlfir_coef(
              &(g_gs_st_gp_zme.zme_coef_addr),
              zme_rtl_para->gfx_zme_common_out.zme_ratio.zme_ratio_vl,
              &(zme_rtl_para->zme_coef_addr.zme_coef_vl_addr));
    PQ_CHECK_SUCCESS_RETURN_NULL(ret);

    ret = pq_mng_get_gfx_vcfir_coef(
              &(g_gs_st_gp_zme.zme_coef_addr),
              zme_rtl_para->gfx_zme_common_out.zme_ratio.zme_ratio_vc,
              &(zme_rtl_para->zme_coef_addr.zme_coef_vc_addr));
    PQ_CHECK_SUCCESS_RETURN_NULL(ret);

    return;
}

static hi_void pq_mng_gzme_sdset(hi_drv_pq_gfx_zme_in *zme_drv_para, hi_drv_pq_gfx_zme_out *zme_rtl_para)
{
        hi_s32 ret = HI_SUCCESS;

    if (zme_drv_para->is_deflicker == HI_TRUE) {
        ret = pq_mng_get_gfx_hlfir_sdcoef(
                  &(g_gs_st_gp_zme.zme_coef_addr),
                  zme_rtl_para->gfx_zme_common_out.zme_ratio.zme_ratio_hl,
                  &(zme_rtl_para->zme_coef_addr.zme_coef_hl_addr));
        PQ_CHECK_SUCCESS_RETURN_NULL(ret);

        ret = pq_mng_get_gfx_hcfir_sdcoef(
                  &(g_gs_st_gp_zme.zme_coef_addr),
                  zme_rtl_para->gfx_zme_common_out.zme_ratio.zme_ratio_hc,
                  &(zme_rtl_para->zme_coef_addr.zme_coef_hc_addr));
        PQ_CHECK_SUCCESS_RETURN_NULL(ret);

        ret = pq_mng_get_gfx_vlfir_sdcoef(
                  &(g_gs_st_gp_zme.zme_coef_addr),
                  zme_rtl_para->gfx_zme_common_out.zme_ratio.zme_ratio_vl,
                  &(zme_rtl_para->zme_coef_addr.zme_coef_vl_addr));
        PQ_CHECK_SUCCESS_RETURN_NULL(ret);

        ret = pq_mng_get_gfx_vcfir_sdcoef(
                  &(g_gs_st_gp_zme.zme_coef_addr),
                  zme_rtl_para->gfx_zme_common_out.zme_ratio.zme_ratio_vc,
                  &(zme_rtl_para->zme_coef_addr.zme_coef_vc_addr));
        PQ_CHECK_SUCCESS_RETURN_NULL(ret);

    } else {
        ret = pq_mng_get_gfx_hlfir_coef(
                  &(g_gs_st_gp_zme.zme_coef_addr),
                  (zme_rtl_para->gfx_zme_common_out.zme_ratio.zme_ratio_hl) >> PQ_PRECISION_GZMERATIONH_12BITOFFSET,
                  &(zme_rtl_para->zme_coef_addr.zme_coef_hl_addr));
        PQ_CHECK_SUCCESS_RETURN_NULL(ret);

        ret = pq_mng_get_gfx_hcfir_coef(
                  &(g_gs_st_gp_zme.zme_coef_addr),
                  (zme_rtl_para->gfx_zme_common_out.zme_ratio.zme_ratio_hc) >> PQ_PRECISION_GZMERATIONH_12BITOFFSET,
                  &(zme_rtl_para->zme_coef_addr.zme_coef_hc_addr));
        PQ_CHECK_SUCCESS_RETURN_NULL(ret);

        ret = pq_mng_get_gfx_vlfir_coef(
                  &(g_gs_st_gp_zme.zme_coef_addr),
                  zme_rtl_para->gfx_zme_common_out.zme_ratio.zme_ratio_vl,
                  &(zme_rtl_para->zme_coef_addr.zme_coef_vl_addr));
        PQ_CHECK_SUCCESS_RETURN_NULL(ret);

        ret = pq_mng_get_gfx_vcfir_coef(
                  &(g_gs_st_gp_zme.zme_coef_addr),
                  zme_rtl_para->gfx_zme_common_out.zme_ratio.zme_ratio_vc,
                  &(zme_rtl_para->zme_coef_addr.zme_coef_vc_addr));
        PQ_CHECK_SUCCESS_RETURN_NULL(ret);
    }

    return;
}

hi_s32 pq_mng_set_gfx_resume(hi_bool default_code)
{
    PQ_CHECK_INIT_RE_FAIL(g_gfx_zme_init_flag);

    g_gs_b_resume[HI_DRV_PQ_GFX_LAYER_GP0] = HI_TRUE;
    g_gs_b_resume[HI_DRV_PQ_GFX_LAYER_GP1] = HI_TRUE;

    return HI_SUCCESS;
}

hi_void pq_mng_pixel_align(hi_drv_pq_gfx_zme_in *gfx_zme_in, hi_drv_pq_gfx_zme_out *gfx_zme_out)
{
    PQ_CHECK_NULL_PTR_RE_NULL(gfx_zme_in);
    PQ_CHECK_NULL_PTR_RE_NULL(gfx_zme_out);

    gfx_zme_in->zme_width_in = (gfx_zme_in->zme_width_in / GZME_PIXEL_ALIGN) * GZME_PIXEL_ALIGN;
    gfx_zme_in->zme_height_in = (gfx_zme_in->zme_height_in / GZME_PIXEL_ALIGN) * GZME_PIXEL_ALIGN;
    gfx_zme_in->zme_width_out = (gfx_zme_in->zme_width_out / GZME_PIXEL_ALIGN) * GZME_PIXEL_ALIGN;
    gfx_zme_in->zme_height_out = (gfx_zme_in->zme_height_out / GZME_PIXEL_ALIGN) * GZME_PIXEL_ALIGN;

    gfx_zme_out->gfx_zme_common_out.zme_width_in = gfx_zme_in->zme_width_in;
    gfx_zme_out->gfx_zme_common_out.zme_height_in = gfx_zme_in->zme_height_in;
    gfx_zme_out->gfx_zme_common_out.zme_width_out =  gfx_zme_in->zme_width_out;
    gfx_zme_out->gfx_zme_common_out.zme_height_out = gfx_zme_in->zme_height_out;

    return;
}

hi_void pq_mng_gfx_zme_reso_check(hi_drv_pq_gfx_layer gfx_layer, hi_drv_pq_gfx_zme_in *gfx_zme_in)
{
    if (gfx_layer == HI_DRV_PQ_GFX_LAYER_GP0) {
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(gfx_zme_in->zme_width_in, PQ_WIDTH_3840);
        PQ_CHECK_NUM_LOW_RANGE_RE_NULL(gfx_zme_in->zme_width_in, PQ_WIDTH_256);
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(gfx_zme_in->zme_height_in, PQ_HEIGHT_2160);
        PQ_CHECK_NUM_LOW_RANGE_RE_NULL(gfx_zme_in->zme_height_in, PQ_HEIGHT_64);
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(gfx_zme_in->zme_width_out, PQ_WIDTH_7680);
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(gfx_zme_in->zme_height_out, PQ_HEIGHT_4320);
    } else if (gfx_layer == HI_DRV_PQ_GFX_LAYER_GP1) {
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(gfx_zme_in->zme_width_in, PQ_WIDTH_1920);
        PQ_CHECK_NUM_LOW_RANGE_RE_NULL(gfx_zme_in->zme_width_in, PQ_WIDTH_32);
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(gfx_zme_in->zme_height_in, PQ_HEIGHT_1080);
        PQ_CHECK_NUM_LOW_RANGE_RE_NULL(gfx_zme_in->zme_height_in, PQ_HEIGHT_32);
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(gfx_zme_in->zme_width_out, PQ_WIDTH_4096);
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(gfx_zme_in->zme_height_out, PQ_HEIGHT_2160);
    }

    return;
}

hi_void pq_mng_proc_ajust_gfx_zme_stragegy(hi_drv_pq_gfx_zme_out *gfx_zme_out)
{
    hi_drv_pq_gfx_zme_common_out *out;

    out = &(gfx_zme_out->gfx_zme_common_out);
    if (g_gfx_zme_set_proc.gfx_zme_fir_update ==  HI_TRUE) {
        if (g_gfx_zme_set_proc.gfx_zme_fir_mode ==  HI_DRV_PQ_ZME_FIR) {
            /* modified by sdk for opening sharp module 2013/6/22 himedia problem */
            out->zme_fir_mode.zme_fir_ha = HI_DRV_PQ_ZME_FIR;
            out->zme_fir_mode.zme_fir_h = HI_DRV_PQ_ZME_FIR;

            /* considering anti-flicker for SD, the situation of 1:1 vertical scaler also need low-pass filter */
            out->zme_fir_mode.zme_fir_va = HI_DRV_PQ_ZME_FIR;
            out->zme_fir_mode.zme_fir_v = HI_DRV_PQ_ZME_FIR;
        } else {
            /* modified by sdk for opening sharp module 2013/6/22 himedia problem */
            out->zme_fir_mode.zme_fir_ha = HI_DRV_PQ_ZME_COPY;
            out->zme_fir_mode.zme_fir_h = HI_DRV_PQ_ZME_COPY;

            /* considering anti-flicker for SD, the situation of 1:1 vertical scaler also need low-pass filter */
            out->zme_fir_mode.zme_fir_va = HI_DRV_PQ_ZME_COPY;
            out->zme_fir_mode.zme_fir_v = HI_DRV_PQ_ZME_COPY;
        }

        out->zme_med_fir_en.zme_med_ha_en = g_gfx_zme_set_proc.gfx_zme_med_en;
        out->zme_med_fir_en.zme_med_hl_en = g_gfx_zme_set_proc.gfx_zme_med_en;
        out->zme_med_fir_en.zme_med_hc_en = g_gfx_zme_set_proc.gfx_zme_med_en;
        out->zme_med_fir_en.zme_med_va_en = g_gfx_zme_set_proc.gfx_zme_med_en;
        out->zme_med_fir_en.zme_med_vl_en = g_gfx_zme_set_proc.gfx_zme_med_en;
        out->zme_med_fir_en.zme_med_vc_en = g_gfx_zme_set_proc.gfx_zme_med_en;
    }

    if (g_gfx_zme_set_proc.gfx_zme_offset_update == HI_TRUE) {
        out->zme_offset.zme_offset_hl = g_gfx_zme_set_proc.gfx_zme_out.zme_offset.zme_offset_hl;
        out->zme_offset.zme_offset_hc = g_gfx_zme_set_proc.gfx_zme_out.zme_offset.zme_offset_hc;
        out->zme_offset.zme_offset_v_tp = g_gfx_zme_set_proc.gfx_zme_out.zme_offset.zme_offset_v_tp;
        out->zme_offset.zme_offset_v_btm = g_gfx_zme_set_proc.gfx_zme_out.zme_offset.zme_offset_v_btm;
    }

    memcpy(&g_gfx_zme_get_proc.gfx_zme_out, &(gfx_zme_out->gfx_zme_common_out),
           sizeof(hi_drv_pq_gfx_zme_common_out));
    g_gfx_zme_get_proc.gfx_zme_fir_mode = gfx_zme_out->gfx_zme_common_out.zme_fir_mode.zme_fir_h;
    g_gfx_zme_get_proc.gfx_zme_med_en = gfx_zme_out->gfx_zme_common_out.zme_med_fir_en.zme_med_hl_en;

    return;
}

hi_s32 pq_mng_get_gfx_zme_strategy(hi_drv_pq_gfx_layer gfx_layer, hi_drv_pq_gfx_zme_in *gfx_zme_in,
                                   hi_drv_pq_gfx_zme_out *gfx_zme_out)
{
    hi_s32 ret = HI_SUCCESS;
    hi_bool change;
    static hi_drv_pq_gfx_zme_in zme_para[HI_DRV_PQ_GFX_LAYER_MAX] = { 0 };
    static hi_drv_pq_gfx_zme_out zme_out[HI_DRV_PQ_GFX_LAYER_MAX] = { 0 };
    hi_drv_pq_gfx_zme_in gfx_zme_real_in = { 0 };

    PQ_CHECK_OVER_RANGE_RE_FAIL(gfx_layer, HI_DRV_PQ_GFX_LAYER_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(gfx_zme_in);
    PQ_CHECK_INIT_RE_FAIL(g_gfx_zme_init_flag);

    pq_mng_gfx_zme_reso_check(gfx_layer, gfx_zme_in);

    change = (zme_para[gfx_layer].zme_width_in != gfx_zme_in->zme_width_in)
             || (zme_para[gfx_layer].zme_height_in != gfx_zme_in->zme_height_in)
             || (zme_para[gfx_layer].zme_width_out != gfx_zme_in->zme_width_out)
             || (zme_para[gfx_layer].zme_height_out != gfx_zme_in->zme_height_out)
             || (zme_para[gfx_layer].zme_frame_in != gfx_zme_in->zme_frame_in)
             || (zme_para[gfx_layer].zme_frame_out != gfx_zme_in->zme_frame_out)
             || (zme_para[gfx_layer].is_deflicker != gfx_zme_in->is_deflicker);

    if ((change == HI_FALSE) && (g_gs_b_resume[HI_DRV_PQ_GFX_LAYER_GP0] == HI_FALSE) &&
        (g_gs_b_resume[HI_DRV_PQ_GFX_LAYER_GP1] == HI_FALSE)) {
        memcpy(gfx_zme_out, &zme_out[gfx_layer],sizeof(hi_drv_pq_gfx_zme_out));

        return HI_SUCCESS;
    }
    g_gs_b_resume[gfx_layer] = HI_FALSE;

    /* copy input param to local */
    memcpy(&gfx_zme_real_in, gfx_zme_in, sizeof(hi_drv_pq_gfx_zme_in));
    pq_mng_pixel_align(&gfx_zme_real_in, gfx_zme_out);

    pq_mng_gzme_comn_set(&gfx_zme_real_in, &(gfx_zme_out->gfx_zme_common_out));

    if (HI_DRV_PQ_GFX_LAYER_GP0 == gfx_layer) {
        pq_mng_gzme_hdset(&gfx_zme_real_in, gfx_zme_out);
    } else if (HI_DRV_PQ_GFX_LAYER_GP1 == gfx_layer) {
        pq_mng_gzme_sdset(&gfx_zme_real_in, gfx_zme_out);
    }

    memcpy(&zme_para[gfx_layer], gfx_zme_in, sizeof(hi_drv_pq_gfx_zme_in));
    memcpy(&zme_out[gfx_layer], gfx_zme_out, sizeof(hi_drv_pq_gfx_zme_out));
    g_gfx_zme_get_proc.gfx_layer = gfx_layer;
    pq_mng_proc_ajust_gfx_zme_stragegy(gfx_zme_out);

    return ret;
}

hi_s32 pq_mng_init_gfx_zme(pq_bin_param *param, hi_bool para_use_table_default)
{
    hi_s32 ret;
    drv_pq_mem_attr attr = { 0 };

    if (g_gfx_zme_init_flag) {
        return HI_SUCCESS;
    }

    attr.name = "pq_gfx_zme_coef";
    attr.size = GZME_MMU_SIZE;
    attr.mode =  OSAL_MMZ_TYPE; /* just smmu support */
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = pq_comm_mem_alloc(&attr, &(g_gs_st_gp_zme.m_buf));
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_gfx_zme_coef MMZ malloc failed\n");
        g_gfx_zme_init_flag = HI_FALSE;

        return HI_FAILURE;
    }

    pq_comm_mem_flush(&(g_gs_st_gp_zme.m_buf));

    /* load zme coefficient into the memory */
    pq_mng_gzme_load_coef_hv(&g_gs_st_gp_zme);

    g_gfx_zme_init_flag = HI_TRUE;
    g_gfx_zme_set_proc.gfx_zme_fir_mode = HI_DRV_PQ_ZME_FIR;
    g_gfx_zme_set_proc.gfx_zme_med_en = HI_FALSE;
    g_gfx_zme_set_proc.gfx_zme_offset_update = HI_FALSE;
    g_gfx_zme_set_proc.gfx_zme_fir_update = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_deinit_gfx_zme(hi_void)
{
    if (g_gfx_zme_init_flag == HI_FALSE) {
        return HI_SUCCESS;
    }

    /* release zme coefficient memory */
    if (g_gs_st_gp_zme.m_buf.vir_addr != HI_NULL) {
        pq_comm_mem_free((drv_pq_mem_info *)(&(g_gs_st_gp_zme.m_buf)));
        g_gs_st_gp_zme.m_buf.vir_addr = HI_NULL;
        g_gs_st_gp_zme.m_buf.phy_addr = 0;
    }

    g_gfx_zme_init_flag = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_strategy_by_proc(gfx_zme_strategy_proc strategy)
{
    memcpy(&g_gfx_zme_set_proc, &strategy, sizeof(gfx_zme_strategy_proc));

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_strategy_by_proc(gfx_zme_strategy_proc *strategy)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(strategy);

    memcpy(strategy, &g_gfx_zme_get_proc, sizeof(gfx_zme_strategy_proc));

    return HI_SUCCESS;
}

static pq_alg_funcs g_gfx_zme_funcs = {
    .init = pq_mng_init_gfx_zme,
    .deinit = pq_mng_deinit_gfx_zme,
    .get_gfx_zme_strategy = pq_mng_get_gfx_zme_strategy,
    .set_resume = pq_mng_set_gfx_resume,
    .set_strategy_by_proc = pq_mng_set_strategy_by_proc,
    .get_strategy_by_proc = pq_mng_get_strategy_by_proc,
};

hi_s32 pq_mng_register_gfx_zme(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_GFXZME, REG_TYPE_ALL, PQ_BIN_ADAPT_SINGLE, "gfx zme",
                               &g_gfx_zme_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_gfx_zme()
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_GFXZME);

    return ret;
}
