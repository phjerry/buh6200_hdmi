/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: pq mng acm
 * Author: pq
 * Create: 2019-10-26
 */

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_drv_mem.h"
#include "hi_drv_file.h"
#include "hi_drv_pq.h"

#include "pq_hal_comm.h"
#include "pq_hal_acm.h"
#include "pq_mng_acm.h"
#include "pq_mng_acm_coef.h"

#define CROSS_LEN_OFFSET   2
#define FLESH_TONE_HUE_START 22
#define FLESH_TONE_HUE_END   26
#define SAT_BY_H_MAX_OFFSET 255
#define SAT_BY_H_MIN_OFFSET (-255)


drv_pq_mem_info g_acm_buf_addr;
static pq_acm_info g_acm_info = { 0 };
static hi_s32 g_sat_gain[COLOR_SEGMENTATION_MAX] = {0};
static hi_pq_six_base_color g_six_base_color = {50, 50, 50, 50, 50, 50};
static hi_pq_fleshtone g_flesh_level = HI_PQ_FLESHTONE_GAIN_MID; /* ACM肤色强度 */

static hi_pq_color_spec_mode g_color_spec_mode = HI_PQ_COLOR_MODE_RECOMMEND;
static color_segmentation g_find_next_color_cross[COLOR_SEGMENTATION_MAX] = {
    FLESH,
    CYAN,
    MAGENTA,
    MAGENTA,
    BLUE1,
    RED,
    GREEN,
    YELLOW
};

hi_s32 pq_mng_set_acm_en(hi_bool on_off)
{
    vdp_regs_type* vdp_reg = HI_NULL;
    hi_bool source_is_hdr = (g_acm_info.hdr_type_in == HI_DRV_HDR_TYPE_HDR10 ||
#ifdef HI_HDR10PLUS_SUPPORT
                             g_acm_info.hdr_type_in == HI_DRV_HDR_TYPE_HDR10PLUS ||
#endif
                             g_acm_info.hdr_type_in == HI_DRV_HDR_TYPE_HLG) ? HI_TRUE : HI_FALSE;
    hi_bool output_is_hdr = (g_acm_info.hdr_type_out == HI_DRV_HDR_TYPE_HDR10 ||
#ifdef HI_HDR10PLUS_SUPPORT
                             g_acm_info.hdr_type_out == HI_DRV_HDR_TYPE_HDR10PLUS ||
#endif
                             g_acm_info.hdr_type_out == HI_DRV_HDR_TYPE_HLG) ? HI_TRUE : HI_FALSE;

    PQ_CHECK_INIT_RE_FAIL(g_acm_info.is_init);

    vdp_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(vdp_reg);

    if ((source_is_hdr == HI_FALSE) && (output_is_hdr == HI_TRUE)) {
        pq_hal_set_acm_link(vdp_reg, PQ_ACM_LINK_V0);
    } else {
        pq_hal_set_acm_link(vdp_reg, PQ_ACM_LINK_VP);
    }
    if ((source_is_hdr == HI_TRUE) && (output_is_hdr == HI_TRUE)) {
        pq_hal_set_acm_enable(vdp_reg, HI_FALSE);
        g_acm_info.enable = HI_FALSE;
    } else {
        pq_hal_set_acm_enable(vdp_reg, on_off);
        g_acm_info.enable = on_off;
    }
    return HI_SUCCESS;
}

hi_s32 pq_mng_get_acm_en(hi_u32 *on_off)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);
    PQ_CHECK_INIT_RE_FAIL(g_acm_info.is_init);

    *on_off = g_acm_info.enable;

    return HI_SUCCESS;
}

/* strength[0,100] */
hi_s32 pq_mng_set_acm_strength(hi_u32 strength)
{
    vdp_regs_type *vdp_reg = HI_NULL;
    hi_u32 luma;
    hi_u32 hue;
    hi_u32 sat;

    PQ_CHECK_INIT_RE_FAIL(g_acm_info.is_init);

    if (strength > PQ_ALG_MAX_VALUE) {
        HI_ERR_PQ("strength->%d is invalid!\n", strength);
        return HI_FAILURE;
    }

    vdp_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(vdp_reg);

    luma = strength / 50 * 255;    /* 50 255: cal luma gain */
    hue  = strength / 50 * 300;    /* 50 300: cal hue gain */
    sat  = strength / 50 * 255;    /* 50 255: cal sat gain */

#ifdef PQ_ACM_GAIN_SUPPORT
    ret = pq_hal_set_acm_gain(vdp_reg, luma, hue, sat);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("set acm strength->%d failed!\n", strength);
        return ret;
    }

    g_acm_info.strength = strength;
#endif

    return HI_SUCCESS;
}

/* strength[0,100] */
hi_s32 pq_mng_get_acm_strength(hi_u32 *strength)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(strength);
    PQ_CHECK_INIT_RE_FAIL(g_acm_info.is_init);

    *strength = g_acm_info.strength; /* 0~100 */

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_color(hi_void)
{
    hi_s32 ret;
    acm_effect_param *color_param = HI_NULL;
    vdp_regs_type *vdp_reg = HI_NULL;

    vdp_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(vdp_reg);

    color_param = (acm_effect_param *)pq_kmalloc(sizeof(acm_effect_param));
    if (color_param == HI_NULL) {
        HI_ERR_PQ("alloc color_param mem is failed!\n");
        return HI_FAILURE;
    }

    memcpy(color_param->yhs_by_h, g_yhs_by_h, sizeof(g_yhs_by_h));
    memcpy(color_param->yhs_by_hs_gain, g_yhs_by_hs_gain, sizeof(g_yhs_by_hs_gain));
    memcpy(color_param->yhs_by_hy_gain, g_yhs_by_hy_gain, sizeof(g_yhs_by_hy_gain));

    color_param->hue_gain = HUE_GAIN;
    color_param->sat_gain = SAT_GAIN;
    color_param->luma_gain = LUMA_GAIN;

    ret = pq_hal_acm_set_param(color_param);
    pq_kfree_safe(color_param);
    if (ret == HI_FAILURE) {
        HI_ERR_PQ("acm_hal_set_param failure\n");
        return ret;
    }

    pq_hal_set_acm_stretch(vdp_reg, 1); /* input pixel range limit 0: Y 64~940 C 64~960 1: Y 0~1023, C 0~1023 */
    pq_hal_set_acm_clip_range(vdp_reg, 1); /* output pixel range limit  0: Y 64~940 C 64~960 1: Y 0~1023, C 0~1023 */
    pq_hal_set_acm_cbcr_threshold(vdp_reg, 0); /* threshold of adjustmen enable  0~255 */

    return ret;
}

hi_s32 pq_mng_init_acm(pq_bin_param *param, hi_bool para_use_table_default)
{
    hi_s32 ret;
    drv_pq_mem_attr attr = { 0 };

    if (param == HI_NULL) {
        HI_ERR_PQ("Invalid para, null pointer.\n");
        return HI_FAILURE;
    }
    if (g_acm_info.is_init == HI_TRUE) {
        HI_WARN_PQ("acm alg has been already initialized.\n");
        return HI_SUCCESS;
    }

    attr.name = "pq_acm_coef";
    attr.size = ACM_COEF_SIZE;
    attr.mode = OSAL_MMZ_TYPE; /* just smmu support */
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = pq_comm_mem_alloc(&attr, &g_acm_buf_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("g_acm_buf_addr malloc failed\n");
        return HI_FAILURE;
    }

    ret = pq_comm_mem_flush(&g_acm_buf_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("g_acm_buf_addr flush failed\n");
        goto PQ_ACM_DEINIT;
    }

    ret = pq_hal_acm_distribute_addr(g_acm_buf_addr.vir_addr, g_acm_buf_addr.phy_addr);
    if (ret != HI_SUCCESS) {
        goto PQ_ACM_DEINIT;
    }

    ret = pq_mng_set_color();
    if (ret != HI_SUCCESS) {
        goto PQ_ACM_DEINIT;
    }

    g_acm_info.is_init = HI_TRUE;

    return ret;

PQ_ACM_DEINIT : {
    if (g_acm_buf_addr.vir_addr != HI_NULL) {
        pq_comm_mem_free((drv_pq_mem_info*)(&(g_acm_buf_addr)));
        g_acm_buf_addr.vir_addr = HI_NULL;
        g_acm_buf_addr.phy_addr = 0;
    }
    pq_hal_acm_reset_addr();
    g_acm_info.is_init = HI_FALSE;
    return HI_FAILURE;
    }
}

hi_s32 pq_mng_deinit_acm(hi_void)
{
    if (g_acm_info.is_init == HI_FALSE) {
        return HI_SUCCESS;
    }

    if (g_acm_buf_addr.vir_addr != HI_NULL) {
        pq_comm_mem_free ((drv_pq_mem_info*)(&(g_acm_buf_addr)));
        g_acm_buf_addr.vir_addr = HI_NULL;
        g_acm_buf_addr.phy_addr = 0;
    }
    pq_hal_acm_reset_addr();

    g_acm_info.is_init = HI_FALSE;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_acm_demo(hi_bool demo_en)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_acm_info.is_init);

    ret = pq_hal_acm_set_demo_en(demo_en);
    PQ_CHECK_RETURN_SUCCESS(ret);

    g_acm_info.demo_en = demo_en;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_acm_demo(hi_bool *demo_en)
{
    PQ_CHECK_INIT_RE_FAIL(g_acm_info.is_init);

    if (demo_en == HI_NULL) {
        HI_ERR_PQ("Null pointer!\n");
        return HI_FAILURE;
    }

    *demo_en = g_acm_info.demo_en;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_acm_demo_mode(pq_demo_mode demo_mode)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_acm_info.is_init);

    if (demo_mode >= PQ_DEMO_MAX) {
        HI_ERR_PQ("demo_mode is invalid!\n");
        return HI_FAILURE;
    }

    ret = pq_hal_acm_set_demo_mode(demo_mode);
    PQ_CHECK_RETURN_SUCCESS(ret);

    g_acm_info.demo_mode = demo_mode;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_acm_demo_mode(pq_demo_mode *demo_mode)
{
    PQ_CHECK_INIT_RE_FAIL(g_acm_info.is_init);

    if (demo_mode == HI_NULL) {
        HI_ERR_PQ("Null pointer!\n");
        return HI_FAILURE;
    }

    *demo_mode = g_acm_info.demo_mode;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_acm_set_demo_pos(hi_u32 x_pos)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_acm_info.is_init);

    ret = pq_hal_acm_set_demo_pos(x_pos);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_hal_set_acm_demo_pos failed !\n");
        return ret;
    }

    g_acm_info.demo_pos = x_pos;

    return HI_SUCCESS;
}

static hi_void pq_mng_set_source_output_type(hi_drv_hdr_type source_type, hi_drv_hdr_type output_type)
{
    g_acm_info.hdr_type_in = source_type;
    g_acm_info.hdr_type_out = output_type;
    return;
}

static hi_void pq_mng_cal_color_boundary_sat_by_hue(color_segmentation color_type, acm_effect_param *acm_param)
{
    hi_u32 i;
    hi_u32 hue_start;
    hi_u32 hue_end;
    hi_u32 cross_len;

    if (color_type >= COLOR_SEGMENTATION_MAX) {
        HI_ERR_PQ("color_type is over range!\n");
        return;
    }

    hue_start = g_color_cross_range[color_type].hue_start;
    hue_end = g_color_cross_range[color_type].hue_end;

    if (hue_start == 0) {
        return;
    }
    if (hue_start > hue_end || hue_start >= COLOR_H_NUM || hue_end >= COLOR_H_NUM - 1) {
        HI_ERR_PQ("index error!\n");
        return;
    }

    for (i = 0; i < COLOR_H_NUM; i++) {
        if (acm_param->yhs_by_h[PQ_COLOR_YHS_S][i] > SAT_BY_H_MAX_OFFSET ||
            acm_param->yhs_by_h[PQ_COLOR_YHS_S][i] < SAT_BY_H_MIN_OFFSET) {
            HI_ERR_PQ("over range!\n");
            return;
        }
    }

    cross_len = hue_end - hue_start;

    for (i = 0; i <= cross_len; i++) {
        acm_param->yhs_by_h[PQ_COLOR_YHS_S][hue_start + i] =
            (hi_s16)(acm_param->yhs_by_h[PQ_COLOR_YHS_S][hue_start - 1] +
                     (i + 1) * (acm_param->yhs_by_h[PQ_COLOR_YHS_S][hue_end + 1] -
                                acm_param->yhs_by_h[PQ_COLOR_YHS_S][hue_start - 1]) /
                     (cross_len + CROSS_LEN_OFFSET));

        acm_clip(acm_param->yhs_by_h[PQ_COLOR_YHS_S][hue_start + i], SAT_BY_H_MAX_OFFSET);
    }

    return;
}

static hi_void pq_mng_cal_color_section_sat_by_hue(color_segmentation color_type, acm_effect_param *acm_param)
{
    hi_u32 i;
    hi_u32 hue_start;
    hi_u32 hue_end;
    hi_s32 sat;

    if (color_type >= COLOR_SEGMENTATION_MAX) {
        HI_ERR_PQ("color_type is over range!\n");
        return;
    }
    sat = g_sat_gain[color_type];

    hue_start = g_color_range[color_type].hue_start;
    hue_end = g_color_range[color_type].hue_end;

    if (hue_start > hue_end || hue_start >= COLOR_H_NUM || hue_end >= COLOR_H_NUM) {
        HI_ERR_PQ("index error!\n");
        return;
    }

    for (i = 0; i < COLOR_H_NUM; i++) {
        if (acm_param->yhs_by_h[PQ_COLOR_YHS_S][i] > SAT_BY_H_MAX_OFFSET ||
            acm_param->yhs_by_h[PQ_COLOR_YHS_S][i] < SAT_BY_H_MIN_OFFSET) {
            HI_ERR_PQ("over range!\n");
            return;
        }
    }

    if (sat != 0) {
        for (i = hue_start; i <= hue_end; i++) {
            if (sat > 0) {
                acm_param->yhs_by_h[PQ_COLOR_YHS_S][i] += sat * (SAT_BY_H_MAX_OFFSET -
                        acm_param->yhs_by_h[PQ_COLOR_YHS_S][i]) / PQ_ALG_DEFAULT_VALUE;
                acm_clip(acm_param->yhs_by_h[PQ_COLOR_YHS_S][i], SAT_BY_H_MAX_OFFSET);
            } else {
                acm_param->yhs_by_h[PQ_COLOR_YHS_S][i] += sat * (SAT_BY_H_MAX_OFFSET +
                        acm_param->yhs_by_h[PQ_COLOR_YHS_S][i]) / PQ_ALG_DEFAULT_VALUE;
                acm_clip(acm_param->yhs_by_h[PQ_COLOR_YHS_S][i], SAT_BY_H_MAX_OFFSET);
            }
        }
    }

    return;
}

static hi_void pq_mng_adjust_color_sat_by_hue(acm_effect_param *acm_param)
{
    color_segmentation color_type;

    for (color_type = RED; color_type <= FLESH; color_type++) {
        /* 先求中心点的值 */
        pq_mng_cal_color_section_sat_by_hue(color_type, acm_param);

        /*  再求边界点的值 */
        /*  边界点的亮度、色调、饱和度调节(依赖于两边点的值，与UI无直接联系，被依赖的点与UI直接相关) */
        /*  某段颜色调整，其两边的边界点都随之变化.由于颜色点定义时是按R，G，B，C，M，Y，F的顺序， */
        /*  与实际颜色分布的先后顺序不同,需要通过find数组找到 */
        if (g_sat_gain[color_type] != 0) {
            pq_mng_cal_color_boundary_sat_by_hue(color_type, acm_param);
            pq_mng_cal_color_boundary_sat_by_hue(g_find_next_color_cross[color_type], acm_param);
        }
    }
}

static hi_s32 pq_mng_set_six_base_color(hi_pq_six_base_color *color_offset)
{
    hi_s32 ret;
    acm_effect_param *acm_param = HI_NULL;

    PQ_CHECK_INIT_RE_FAIL(g_acm_info.is_init);
    PQ_CHECK_NULL_PTR_RE_FAIL(color_offset);

    if ((color_offset->red > PQ_ALG_MAX_VALUE) || (color_offset->green > PQ_ALG_MAX_VALUE) ||
        (color_offset->blue > PQ_ALG_MAX_VALUE) || (color_offset->cyan > PQ_ALG_MAX_VALUE) ||
        (color_offset->magenta > PQ_ALG_MAX_VALUE) || (color_offset->yellow > PQ_ALG_MAX_VALUE)) {
        HI_ERR_PQ("out of range\n");
        return HI_FAILURE;
    }

    g_six_base_color.red = color_offset->red;
    g_six_base_color.green = color_offset->green;
    g_six_base_color.blue = color_offset->blue;
    g_six_base_color.cyan = color_offset->cyan;
    g_six_base_color.magenta = color_offset->magenta;
    g_six_base_color.yellow = color_offset->yellow;

    g_sat_gain[RED] = color_offset->red - PQ_ALG_DEFAULT_VALUE;
    g_sat_gain[GREEN] = color_offset->green - PQ_ALG_DEFAULT_VALUE;
    g_sat_gain[BLUE1] = color_offset->blue - PQ_ALG_DEFAULT_VALUE;
    g_sat_gain[BLUE2] = color_offset->blue - PQ_ALG_DEFAULT_VALUE;
    g_sat_gain[CYAN] = color_offset->cyan - PQ_ALG_DEFAULT_VALUE;
    g_sat_gain[MAGENTA] = color_offset->magenta - PQ_ALG_DEFAULT_VALUE;
    g_sat_gain[YELLOW] = color_offset->yellow - PQ_ALG_DEFAULT_VALUE;

    acm_param = (acm_effect_param *)pq_kmalloc(sizeof(acm_effect_param));
    if (acm_param == HI_NULL) {
        HI_ERR_PQ("alloc color_param mem is failed!\n");
        return HI_FAILURE;
    }
    memcpy(acm_param->yhs_by_h, g_yhs_by_h, sizeof(g_yhs_by_h));
    pq_mng_adjust_color_sat_by_hue(acm_param);
    memcpy(acm_param->yhs_by_hs_gain, g_yhs_by_hs_gain, sizeof(g_yhs_by_hs_gain));
    memcpy(acm_param->yhs_by_hy_gain, g_yhs_by_hy_gain, sizeof(g_yhs_by_hy_gain));
    acm_param->hue_gain = HUE_GAIN;
    acm_param->sat_gain = SAT_GAIN;
    acm_param->luma_gain = LUMA_GAIN;

    ret = pq_hal_acm_set_param(acm_param);
    pq_kfree_safe(acm_param);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("acm_hal_set_param failure\n");
        return ret;
    }
    return HI_SUCCESS;
}

hi_s32 pq_mng_get_six_base_color(hi_pq_six_base_color *color_offset)
{
    PQ_CHECK_INIT_RE_FAIL(g_acm_info.is_init);
    PQ_CHECK_NULL_PTR_RE_FAIL(color_offset);

    color_offset->red = g_six_base_color.red;
    color_offset->green = g_six_base_color.green;
    color_offset->blue = g_six_base_color.blue;
    color_offset->cyan = g_six_base_color.cyan;
    color_offset->magenta = g_six_base_color.magenta;
    color_offset->yellow = g_six_base_color.yellow;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_flesh_tone_level(hi_pq_fleshtone flesh_level)
{
    hi_s32 ret;
    acm_effect_param *acm_coef = HI_NULL;

    if (flesh_level >= HI_PQ_FLESHTONE_GAIN_MAX) {
        HI_ERR_PQ("[%d]Flesh tone level range error!\n", flesh_level);
        return HI_FAILURE;
    }

    PQ_CHECK_INIT_RE_FAIL(g_acm_info.is_init);

    acm_coef = (acm_effect_param *)pq_kmalloc(sizeof(acm_effect_param));
    PQ_CHECK_NULL_PTR_RE_FAIL(acm_coef);

    memcpy(acm_coef->yhs_by_h, g_flesh_enhance, sizeof(g_flesh_enhance));

    memcpy(acm_coef->yhs_by_hs_gain, g_yhs_by_hs_gain, sizeof(g_yhs_by_hs_gain));
    memcpy(acm_coef->yhs_by_hy_gain, g_yhs_by_hy_gain, sizeof(g_yhs_by_hy_gain));
    acm_coef->hue_gain = HUE_GAIN;
    acm_coef->sat_gain = SAT_GAIN;
    acm_coef->luma_gain = LUMA_GAIN;

    ret = pq_hal_acm_set_param(acm_coef);
    pq_kfree_safe(acm_coef);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_hal_acm_set_param failure\n");
        return HI_FAILURE;
    }
    g_flesh_level = flesh_level;
    return HI_SUCCESS;
}

hi_s32 pq_mng_get_flesh_tone_level(hi_u32 *flesh_level)
{
    PQ_CHECK_INIT_RE_FAIL(g_acm_info.is_init);
    PQ_CHECK_NULL_PTR_RE_FAIL(flesh_level);
    *flesh_level = g_flesh_level;
    return HI_SUCCESS;
}

hi_s32 pq_mng_set_color_enhance_mode(hi_pq_color_spec_mode type)
{
    hi_s32 ret;
    acm_effect_param *color_enhance_param = HI_NULL;

    PQ_CHECK_INIT_RE_FAIL(g_acm_info.is_init);

    if (type >= HI_PQ_COLOR_MODE_MAX) {
        HI_ERR_PQ("[%d]Color Enhance Mode Set error!\n", type);
        return HI_FAILURE;
    }
    color_enhance_param = (acm_effect_param *)pq_kmalloc(sizeof(acm_effect_param));
    PQ_CHECK_NULL_PTR_RE_FAIL(color_enhance_param);
    memset(color_enhance_param, 0, sizeof(acm_effect_param));
    memcpy(color_enhance_param->yhs_by_h, g_yhs_by_h, sizeof(g_yhs_by_h));
    memcpy(color_enhance_param->yhs_by_hs_gain, g_yhs_by_hs_gain, sizeof(g_yhs_by_hs_gain));
    memcpy(color_enhance_param->yhs_by_hy_gain, g_yhs_by_hy_gain, sizeof(g_yhs_by_hy_gain));
    color_enhance_param->hue_gain = HUE_GAIN;
    color_enhance_param->sat_gain = SAT_GAIN;
    color_enhance_param->luma_gain = LUMA_GAIN;

    switch (type) {
        case HI_PQ_COLOR_MODE_RECOMMEND:
            break;
        case HI_PQ_COLOR_MODE_BLUE:
            memcpy(color_enhance_param->yhs_by_h, g_blue_enhance_mode, sizeof(g_blue_enhance_mode));
            break;
        case HI_PQ_COLOR_MODE_GREEN:
            memcpy(color_enhance_param->yhs_by_h, g_green_enhance_mode, sizeof(g_green_enhance_mode));
            break;
        case HI_PQ_COLOR_MODE_BG:
            memcpy(color_enhance_param->yhs_by_h, g_bg_enhance_mode, sizeof(g_bg_enhance_mode));
            break;
        case HI_PQ_COLOR_MODE_ORIGINAL:
            memset(color_enhance_param, 0, sizeof(acm_effect_param));
            break;
        default:
            break;
    }

    ret = pq_hal_acm_set_param(color_enhance_param);
    pq_kfree_safe(color_enhance_param);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_hal_acm_set_param failure\n");
        return HI_FAILURE;
    }

    g_color_spec_mode = type;
    return ret;
}

hi_s32 pq_mng_get_color_enhance_mode(hi_pq_color_spec_mode *mode)
{
    PQ_CHECK_INIT_RE_FAIL(g_acm_info.is_init);
    PQ_CHECK_NULL_PTR_RE_FAIL(mode);
    *mode = g_color_spec_mode;
    return HI_SUCCESS;
}

static pq_alg_funcs g_acm_funcs = {
    .init = pq_mng_init_acm,
    .deinit = pq_mng_deinit_acm,
    .set_enable = pq_mng_set_acm_en,
    .get_enable = pq_mng_get_acm_en,
    .set_strength = pq_mng_set_acm_strength,
    .get_strength = pq_mng_get_acm_strength,
    .set_vdp_hdr_type = pq_mng_set_source_output_type,
    .set_demo = pq_mng_set_acm_demo,
    .get_demo = pq_mng_get_acm_demo,
    .set_demo_mode = pq_mng_set_acm_demo_mode,
    .get_demo_mode = pq_mng_get_acm_demo_mode,
    .set_demo_mode_coor = pq_mng_acm_set_demo_pos,
    .set_six_base_color = pq_mng_set_six_base_color,
    .get_six_base_color = pq_mng_get_six_base_color,
    .set_flesh_tone_level = pq_mng_set_flesh_tone_level,
    .get_flesh_tone_level = pq_mng_get_flesh_tone_level,
    .set_color_enhance_mode = pq_mng_set_color_enhance_mode,
    .get_color_enhance_mode = pq_mng_get_color_enhance_mode,
};

hi_s32 pq_mng_register_acm(pq_reg_type type)
{
    hi_s32 ret = HI_SUCCESS;

    ret = pq_comm_alg_register(HI_PQ_MODULE_ACM, type, PQ_BIN_ADAPT_SINGLE, "acm", &g_acm_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_acm(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_ACM);

    return ret;
}

