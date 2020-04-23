/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: sharpen mng layer function file
 * Author: pq
 * Create: 2019-11-11
 */


#include "drv_pq_table.h"
#include "pq_mng_sharpen.h"
#include "pq_hal_sharpen.h"
#include "pq_hal_comm.h"

#define SHARP_IN_OUT_SCALE_MIN 15
#define MAX_SHARP_PEAK_RATIO   1023
#define MAX_SHARP_LTI_RATIO    1023

static alg_sharpen g_sharpen_info = { 0 };
static drv_pq_mem_info g_sharp_buf_addr = { 0 };
static pq_sharp_cfg_proc g_sharp_proc_info = { 0 };

static hi_s32 pq_mng_init_sharpen(pq_bin_param *param, hi_bool para_use_table_default)
{
    hi_s32 ret;
    drv_pq_mem_attr attr = { 0 };

    PQ_CHECK_NULL_PTR_RE_FAIL(param);

    if (g_sharpen_info.is_init == HI_TRUE) {
        HI_WARN_PQ("sharpen alg has been already initialized.\n");
        return HI_SUCCESS;
    }

    /* init default table! */
    ret = pq_table_init_phy_list(0, HI_PQ_MODULE_SHARPNESS, PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
    PQ_CHECK_RETURN_SUCCESS(ret);

    attr.name = "pq_sharp_coef";
    attr.size = SHARPEN_BUF_SIZE;
    attr.mode = OSAL_MMZ_TYPE; /* just smmu support */
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = pq_comm_mem_alloc(&attr, &g_sharp_buf_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("g_sharp_buf_addr malloc failed\n");
        return HI_FAILURE;
    }

    ret = pq_comm_mem_flush(&g_sharp_buf_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("g_sharp_buf_addr flush failed\n");
        goto sharp_deinit;
    }

    ret = pq_hal_sharp_init_hal(g_sharp_buf_addr.vir_addr, g_sharp_buf_addr.phy_addr);
    if (ret != HI_SUCCESS) {
        goto sharp_deinit;
    }

    g_sharpen_info.is_init = HI_TRUE;

    return HI_SUCCESS;

sharp_deinit:
    if (g_sharp_buf_addr.vir_addr != HI_NULL) {
        pq_comm_mem_free((drv_pq_mem_info *)(&(g_sharp_buf_addr)));
        g_sharp_buf_addr.vir_addr = HI_NULL;
        g_sharp_buf_addr.phy_addr = 0;
    }

    pq_hal_sharp_reset_addr();
    g_sharpen_info.is_init = HI_FALSE;
    return HI_FAILURE;
}

/* deinit sharp moudle */
static hi_s32 pq_mng_deinit_sharpen(hi_void)
{
    if (g_sharpen_info.is_init == HI_FALSE) {
        HI_WARN_PQ("sharpen alg is not init.\n");
        return HI_SUCCESS;
    }

    pq_hal_sharp_reset_addr();

    /* release sharp coefficient memory */
    if (g_sharp_buf_addr.vir_addr != HI_NULL) {
        pq_comm_mem_free ((drv_pq_mem_info *)(&(g_sharp_buf_addr)));
        g_sharp_buf_addr.vir_addr = HI_NULL;
        g_sharp_buf_addr.phy_addr = 0;
    }

    memset(&g_sharp_proc_info, 0x0, sizeof(g_sharp_proc_info));
    memset(&g_sharpen_info, 0x0, sizeof(g_sharpen_info));
    g_sharpen_info.is_init = HI_FALSE;

    return HI_SUCCESS;
}

static hi_u64 pq_mng_get_sharp_mixed_key(hi_u64 addr, hi_u8 lsb, hi_u8 msb, hi_u8 src_mode, hi_u8 out_mode)
{
    return (addr << 23) | (lsb  << 18) | (msb  << 13) |  /* 23,18,13 are offset bits. */
           (src_mode << 9) | (out_mode << 5) | ((hi_u32)HI_PQ_MODULE_SHARPNESS); /* 9,5 are offset bits. */
}

static hi_s32 pq_mng_get_sharp_reg_gain(pq_source_mode source_mode, pq_output_mode output_mode,
                                        hi_u32 *peak_ratio, hi_u32 *lti_ratio)
{
    hi_s32 ret;
    hi_u64 key;
    pq_mixed_key_reg *mixed_key_reg_array = pq_table_get_mixed_key_reg_array();

    key = pq_mng_get_sharp_mixed_key(0x406e4, 12, 21,  /* 0x406e4:peakratio addr, 12:lsb, 21:msb. */
                                     source_mode, output_mode);
    ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, peak_ratio);
    if (ret != HI_SUCCESS) {
        key = pq_mng_get_sharp_mixed_key(0x406e4, 12, 21,  /* 0x406e4:peakratio addr, 12:lsb, 21:msb. */
                                         PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
        ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, peak_ratio);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("find default sharpen peakratio value failed!\n");
            return ret;
        }
    }

    key = pq_mng_get_sharp_mixed_key(0x406dc, 18, 27,  /* 0x406dc:ltiratio addr, 18:lsb, 27:msb. */
                                     source_mode, output_mode);
    ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, lti_ratio);
    if (ret != HI_SUCCESS) {
        key = pq_mng_get_sharp_mixed_key(0x406dc, 18, 27,  /* 0x406dc:ltiratio addr, 18:lsb, 27:msb. */
                                         PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
        ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, lti_ratio);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("find default sharpen ltiratio value failed!\n");
            return ret;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_sharpen_str(hi_u32 strength)
{
    hi_s32 ret;
    pq_source_mode source_mode = pq_comm_get_source_mode();
    pq_output_mode output_mode = pq_comm_get_output_mode();
    hi_u32 peak_ratio;
    hi_u32 lti_ratio;
    hi_u32 table_peak_ratio;
    hi_u32 table_lti_ratio;

    PQ_CHECK_INIT_RE_FAIL(g_sharpen_info.is_init);

    if (strength > PQ_ALG_MAX_VALUE) {
        HI_ERR_PQ("strength->%d is invalid!\n", strength);
        return HI_ERR_PQ_INVALID_PARA;
    }

    /* query basic gain from default table. */
    ret = pq_mng_get_sharp_reg_gain(source_mode, output_mode, &table_peak_ratio, &table_lti_ratio);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_mng_get_sharp_reg_gain failed, ret->%#x !\n", ret);
        return ret;
    }

    /* table gain is equal to 50% strength, so should re-calculate gain according to new strength. */
    peak_ratio = (strength * table_peak_ratio * 2) / 100; /* 2,100 are convert rate */
    lti_ratio = (strength * table_lti_ratio * 2) / 100; /* 2,100 are convert rate */

    /* adjust actual sharpen strength. */
    peak_ratio = (peak_ratio > MAX_SHARP_PEAK_RATIO) ? MAX_SHARP_PEAK_RATIO : peak_ratio;
    lti_ratio = (lti_ratio > MAX_SHARP_LTI_RATIO) ? MAX_SHARP_LTI_RATIO : lti_ratio;

    /* set hal cfg */
    ret = pq_hal_sharp_set_str(peak_ratio, lti_ratio);
    PQ_CHECK_RETURN_SUCCESS(ret);

    g_sharpen_info.strength = strength;
    return HI_SUCCESS;
}

/* get sharpen strength, range: 0~100 */
static hi_s32 pq_mng_get_sharpen_str(hi_u32* sharp_str)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(sharp_str);
    PQ_CHECK_INIT_RE_FAIL(g_sharpen_info.is_init);

    *sharp_str = g_sharpen_info.strength;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_sharp_get_min_scale(hi_u32 in_w, hi_u32 in_h, hi_u32 out_w, hi_u32 out_h, hi_u32 *scale_ratio)
{
    hi_u32 scale_w;
    hi_u32 scale_h;

    PQ_CHECK_ZERO_RE_FAIL(out_w);
    PQ_CHECK_ZERO_RE_FAIL(out_h);
    PQ_CHECK_NULL_PTR_RE_FAIL(scale_ratio);

    scale_w = (in_w * 10) / out_w; /* 10: avoid accuracy loss. */
    scale_h = (in_h * 10) / out_h; /* 10: avoid accuracy loss. */

    *scale_ratio = (scale_w < scale_h) ? scale_w : scale_h;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_sharpen_policy(pq_sharpen_reso *sharp_reso, hi_bool *enable)
{
    hi_s32 ret;
    hi_u32  scale_ratio;
    PQ_CHECK_NULL_PTR_RE_FAIL(sharp_reso);
    PQ_CHECK_NULL_PTR_RE_FAIL(enable);

    /* MIN(VIDEO_IW/VIDEO_OW,VIDEO_IH/VIDEO_OH)>1.5 should close sharpen. */
    ret = pq_mng_sharp_get_min_scale(sharp_reso->video_in_width, sharp_reso->video_in_height,
                                     sharp_reso->video_out_width, sharp_reso->video_out_height, &scale_ratio);
    PQ_CHECK_RETURN_SUCCESS(ret);
    /* record min scale_ratio */
    g_sharp_proc_info.sharp_min_scale_ratio = scale_ratio;

    if (scale_ratio > SHARP_IN_OUT_SCALE_MIN) {
        *enable = HI_FALSE;
        return HI_SUCCESS;
    }

    /* moudle_in_reso <= FHD, open sharpen; otherwise close sharpen. */
    if ((sharp_reso->sharpen_in_width > PQ_WIDTH_1920) || (sharp_reso->sharpen_in_height > PQ_HEIGHT_1080)) {
        *enable = HI_FALSE;
        return HI_SUCCESS;
    }

    /* actual_out_reso / moudle_inreso < 85%, close sharpen. */
    if ((sharp_reso->video_out_width * 100 / sharp_reso->sharpen_in_width) < 85 ||  /* 100:avoid accuracy loss,85% */
        (sharp_reso->video_out_height * 100 / sharp_reso->sharpen_in_height) < 85) { /* 100:avoid accuracy loss,85% */
        *enable = HI_FALSE;
        return HI_SUCCESS;
    }

    *enable = HI_TRUE;
    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_sharpen_en(hi_bool on_off)
{
    PQ_CHECK_INIT_RE_FAIL(g_sharpen_info.is_init);

    /* record status. */
    g_sharpen_info.enable = on_off;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_sharpen_en(hi_bool *on_off)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);
    PQ_CHECK_INIT_RE_FAIL(g_sharpen_info.is_init);

    *on_off = g_sharpen_info.enable;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_sharpen_demo_en(hi_bool on_off)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_sharpen_info.is_init);

    /* set hal cfg. */
    ret = pq_hal_sharp_set_demo_en(on_off);
    PQ_CHECK_RETURN_SUCCESS(ret);

    g_sharpen_info.demo_en = on_off;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_sharpen_demo_en(hi_bool *on_off)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);
    PQ_CHECK_INIT_RE_FAIL(g_sharpen_info.is_init);

    *on_off = g_sharpen_info.demo_en;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_sharpen_demo_mode(pq_demo_mode mode)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_sharpen_info.is_init);

    if (mode >= PQ_DEMO_MAX) {
        HI_ERR_PQ("mode->%d is invalid.\n", mode);
        return HI_ERR_PQ_INVALID_PARA;
    }

    /* set hal cfg */
    ret = pq_hal_sharp_set_demo_mode(mode);
    PQ_CHECK_RETURN_SUCCESS(ret);

    g_sharpen_info.demo_mode = mode;
    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_sharpen_demo_mode(pq_demo_mode *demo_mode)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(demo_mode);
    PQ_CHECK_INIT_RE_FAIL(g_sharpen_info.is_init);

    *demo_mode = g_sharpen_info.demo_mode;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_sharpen_demo_pos(hi_u32 x_pos)
{
    hi_s32 ret;
    hi_u32 hal_demo_pos;

    PQ_CHECK_INIT_RE_FAIL(g_sharpen_info.is_init);

    hal_demo_pos = (x_pos * g_sharpen_info.sharpen_reso.sharpen_in_width) /
                    g_sharpen_info.sharpen_reso.video_out_width;
    ret = pq_hal_sharp_set_demo_pos(hal_demo_pos);
    PQ_CHECK_RETURN_SUCCESS(ret);

    g_sharpen_info.demo_pos = x_pos;
    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_sharpen_reso(const pq_sharpen_reso *sharp_reso)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(sharp_reso);
    PQ_CHECK_INIT_RE_FAIL(g_sharpen_info.is_init);

    g_sharpen_info.sharpen_reso = *sharp_reso;
    g_sharp_proc_info.sharp_reso = *sharp_reso;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_update_sharp_cfg(hi_void)
{
    hi_s32 ret;
    PQ_CHECK_INIT_RE_FAIL(g_sharpen_info.is_init);

    if (g_sharpen_info.enable == HI_TRUE) {
        /* sharpen open policy. */
        ret = pq_mng_sharpen_policy(&g_sharpen_info.sharpen_reso, &g_sharpen_info.enable);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    /* set sharpen enable. */
    ret = pq_hal_sharp_set_en(g_sharpen_info.enable);
    PQ_CHECK_RETURN_SUCCESS(ret);
    g_sharp_proc_info.sharp_enable = g_sharpen_info.enable;

    ret = pq_hal_sharp_update_cfg();

    return ret;
}

static hi_s32 pq_mng_get_sharp_proc_info(pq_sharp_cfg_proc *sharp_proc_info)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(sharp_proc_info);
    PQ_CHECK_INIT_RE_FAIL(g_sharpen_info.is_init);

    *sharp_proc_info = g_sharp_proc_info;
    return HI_SUCCESS;
}

static hi_s32 pq_mng_sharp_ddr_regread(uintptr_t reg_addr, hi_u32 *value)
{
    hi_s32 ret;
    PQ_CHECK_NULL_PTR_RE_FAIL(value);
    PQ_CHECK_INIT_RE_FAIL(g_sharpen_info.is_init);

    ret = pq_hal_sharp_ddr_regread(reg_addr, value);

    return ret;
}

static hi_s32 pq_mng_sharp_ddr_regwrite(uintptr_t reg_addr, hi_u32 value)
{
    hi_s32 ret;
    PQ_CHECK_INIT_RE_FAIL(g_sharpen_info.is_init);

    ret = pq_hal_sharp_ddr_regwrite(reg_addr, value);

    return ret;
}

static pq_alg_funcs g_sharpen_funcs = {
    .init = pq_mng_init_sharpen,
    .deinit = pq_mng_deinit_sharpen,
    .set_enable = pq_mng_set_sharpen_en,
    .get_enable = pq_mng_get_sharpen_en,
    .set_demo = pq_mng_set_sharpen_demo_en,
    .get_demo = pq_mng_get_sharpen_demo_en,
    .set_demo_mode = pq_mng_set_sharpen_demo_mode,
    .get_demo_mode = pq_mng_get_sharpen_demo_mode,
    .set_demo_mode_coor = pq_mng_set_sharpen_demo_pos,
    .set_strength = pq_mng_set_sharpen_str,
    .get_strength = pq_mng_get_sharpen_str,
    .set_sharp_reso = pq_mng_set_sharpen_reso,
    .update_sharp_cfg = pq_mng_update_sharp_cfg,
    .get_sharp_proc_info = pq_mng_get_sharp_proc_info,
    .sharp_ddr_regread = pq_mng_sharp_ddr_regread,
    .sharp_ddr_regwrite = pq_mng_sharp_ddr_regwrite,
};

hi_s32 pq_mng_register_sharp(pq_reg_type type)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_SHARPNESS, type, PQ_BIN_ADAPT_MULTIPLE, "sharp", &g_sharpen_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_sharp(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_SHARPNESS);

    return ret;
}

