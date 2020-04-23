/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: deshoot
 * Author: pq
 * Create: 2019-11-4
 */

#include "pq_mng_deshoot.h"
#include "pq_hal_deshoot.h"
#include "drv_pq_table.h"
#include "pq_hal_comm.h"

#define MAX_BLEND_GAIN 16

static alg_ds g_ds_info;

hi_s32 pq_mng_init_deshoot(pq_bin_param* pq_param, hi_bool para_use_table_default)
{
    hi_u32 handle_no = 0;
    hi_s32 ret;

    if (g_ds_info.init == HI_TRUE) {
        return HI_SUCCESS;
    }

    g_ds_info.enable = HI_TRUE;
    g_ds_info.demo_enable = HI_FALSE;
    g_ds_info.demo_mode = PQ_DEMO_ENABLE_R;

    /* single DS */
    ret = pq_table_init_phy_list(handle_no, HI_PQ_MODULE_DS, PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("deshoot init_phy_list error\n");
        g_ds_info.init = HI_FALSE;

        return ret;
    }
    g_ds_info.init = HI_TRUE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_deinit_deshoot(hi_void)
{

    if (g_ds_info.init == HI_FALSE) {
        return HI_SUCCESS;
    }

    g_ds_info.init = HI_FALSE;
    return HI_SUCCESS;
}

hi_s32 pq_mng_set_deshoot_en(hi_bool on_off)
{
    hi_u32 handle_no;
    hi_s32 ret;
    pq_source_mode source_mode = pq_comm_get_source_mode();
    pq_output_mode output_mode = pq_comm_get_output_mode();

    PQ_CHECK_INIT_RE_FAIL(g_ds_info.init);

    g_ds_info.enable = on_off;

    if ((source_mode < PQ_SOURCE_MODE_MAX) &&
        (pq_comm_get_moudle_ctrl(HI_PQ_MODULE_DS, source_mode, output_mode) == HI_FALSE)) {
        g_ds_info.enable = HI_FALSE;
    }

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (pq_hal_check_vpss_valid(handle_no) == HI_SUCCESS) {
            ret = pq_hal_enable_deshoot(handle_no, g_ds_info.enable);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_deshoot_en(hi_bool *on_off)
{
    PQ_CHECK_INIT_RE_FAIL(g_ds_info.init);
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);

    *on_off = g_ds_info.enable;

    return HI_SUCCESS;
}

static hi_u64 pq_mng_get_ds_mixed_key(hi_u64 addr, hi_u8 lsb, hi_u8 msb, hi_u8 src_mode, hi_u8 out_mode)
{
    return (addr << 23) | (lsb  << 18) | (msb  << 13) |  /* 23,18,13 are offset bits. */
           (src_mode << 9) | (out_mode << 5) | ((hi_u32)HI_PQ_MODULE_DS); /* 9,5 are offset bits. */
}

static hi_s32 pq_mng_get_ds_blendgainov(pq_source_mode source_mode, pq_output_mode output_mode, hi_u32 *basic_str)
{
    hi_s32 ret;
    hi_u64 key;
    pq_mixed_key_reg *mixed_key_reg_array = pq_table_get_mixed_key_reg_array();

    PQ_CHECK_NULL_PTR_RE_FAIL(basic_str);

    key = pq_mng_get_ds_mixed_key(0xa110, 27, 31, source_mode, output_mode); /* 0xa110,27,31 is ds_blendgainov */
    ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, basic_str);
    if (ret != HI_SUCCESS) {
        /* get strength=100% value from default table. */
        key = pq_mng_get_ds_mixed_key(0xa110, 27, 31, /* 0xa110,27,31 is ds_blendgainov */
                                      PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
        ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, basic_str);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("find default ds_blendgainov value failed!\n");
            return ret;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_ds_blendgainud(pq_source_mode source_mode, pq_output_mode output_mode, hi_u32 *basic_str)
{
    hi_s32 ret;
    hi_u64 key;
    pq_mixed_key_reg *mixed_key_reg_array = pq_table_get_mixed_key_reg_array();

    PQ_CHECK_NULL_PTR_RE_FAIL(basic_str);

    key = pq_mng_get_ds_mixed_key(0xa120, 25, 29, source_mode, output_mode); /* 0xa120,25,29 is ds_blendgainud */
    ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, basic_str);
    if (ret != HI_SUCCESS) {
        /* get strength=100% value from default table. */
        key = pq_mng_get_ds_mixed_key(0xa120, 25, 29, /* 0xa120,25,29 is ds_blendgainud */
                                      PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
        ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, basic_str);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("find default ds_blendgainov value failed!\n");
            return ret;
        }
    }

    return HI_SUCCESS;
}

/* strength[0,100] different from other algorithms 100 is the default and the strongest effect */
hi_s32 pq_mng_set_deshoot_strength(hi_u32 strength)
{
    hi_s32 ret;
    hi_u32 handle_no = 0;
    hi_u32 default_blendgainov;
    hi_u32 default_blendgainud;
    hi_u32 blendgainov;
    hi_u32 blendgainud;
    pq_source_mode source_mode = pq_comm_get_source_mode();
    pq_output_mode output_mode = pq_comm_get_output_mode();

    PQ_CHECK_INIT_RE_FAIL(g_ds_info.init);

    if (strength > PQ_ALG_MAX_VALUE) {
        HI_ERR_PQ("strength->%d is invalid!\n", strength);
        return HI_FAILURE;
    }

    /* query basic gain from default table. */
    ret = pq_mng_get_ds_blendgainov(source_mode, output_mode, &default_blendgainov);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_mng_get_ds_blendgainov failed!\n");
        return ret;
    }

    /* query basic gain from default table. */
    ret = pq_mng_get_ds_blendgainud(source_mode, output_mode, &default_blendgainud);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_mng_get_ds_blendgainud failed!\n");
        return ret;
    }

    blendgainov = (strength * default_blendgainov + PQ_ALG_DEFAULT_VALUE - 1) / PQ_ALG_DEFAULT_VALUE;
    blendgainud = (strength * default_blendgainud + PQ_ALG_DEFAULT_VALUE - 1) / PQ_ALG_DEFAULT_VALUE;

    /* adjust actual ds strength. */
    blendgainov = (blendgainov > MAX_BLEND_GAIN) ? MAX_BLEND_GAIN : blendgainov;
    blendgainud = (blendgainud > MAX_BLEND_GAIN) ? MAX_BLEND_GAIN : blendgainud;

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; ++handle_no) {
        if (pq_hal_check_vpss_valid(handle_no) == HI_SUCCESS) {
            ret = pq_hal_deshoot_set_strength(handle_no, blendgainov, blendgainud);
            if (ret != HI_SUCCESS) {
                HI_ERR_PQ("set vpss->%d strength->%d failed!\n", handle_no, strength);
                return ret;
            }
        }
    }

    g_ds_info.strength = strength;

    return HI_SUCCESS;
}

/* strength[0,100] */
hi_s32 pq_mng_get_deshoot_strength(hi_u32 *strength)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(strength);
    PQ_CHECK_INIT_RE_FAIL(g_ds_info.init);

    *strength = g_ds_info.strength; /* 0~100 */

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_deshoot_demo_en(hi_bool on_off)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 handle_no;

    PQ_CHECK_INIT_RE_FAIL(g_ds_info.init);

    g_ds_info.demo_enable= on_off;

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (pq_hal_check_vpss_valid(handle_no) == HI_SUCCESS) {
            ret = pq_hal_enable_deshoot_demo(handle_no, on_off);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

hi_s32 pq_mng_get_deshoot_demo_en(hi_bool* on_off)
{
    PQ_CHECK_INIT_RE_FAIL(g_ds_info.init);
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);

    *on_off = g_ds_info.demo_enable;
    return HI_SUCCESS;
}

hi_s32 pq_mng_set_deshoot_demo_mode(pq_demo_mode mode)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 handle_no;

    PQ_CHECK_INIT_RE_FAIL(g_ds_info.init);

    g_ds_info.demo_mode = mode;

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (pq_hal_check_vpss_valid(handle_no) == HI_SUCCESS) {
            ret = pq_hal_set_deshoot_demo_mode(handle_no, mode);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

hi_s32 pq_mng_get_deshoot_demo_mode(pq_demo_mode* demo_mode)
{
    PQ_CHECK_INIT_RE_FAIL(g_ds_info.init);
    PQ_CHECK_NULL_PTR_RE_FAIL(demo_mode);

    *demo_mode = g_ds_info.demo_mode;
    return HI_SUCCESS;
}

hi_s32 pq_mng_set_deshoot_demo_mode_coor(hi_u32 x_pos)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 handle_no;

    PQ_CHECK_INIT_RE_FAIL(g_ds_info.init);

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (pq_hal_check_vpss_valid(handle_no) == HI_SUCCESS) {
            ret = pq_hal_set_deshoot_demo_coor(handle_no, x_pos);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

static pq_alg_funcs g_ds_funcs = {
    .init = pq_mng_init_deshoot,
    .deinit = pq_mng_deinit_deshoot,
    .set_enable = pq_mng_set_deshoot_en,
    .get_enable = pq_mng_get_deshoot_en,
    .set_strength = pq_mng_set_deshoot_strength,
    .get_strength = pq_mng_get_deshoot_strength,
    .set_demo = pq_mng_set_deshoot_demo_en,
    .get_demo = pq_mng_get_deshoot_demo_en,
    .set_demo_mode = pq_mng_set_deshoot_demo_mode,
    .get_demo_mode = pq_mng_get_deshoot_demo_mode,
    .set_demo_mode_coor = pq_mng_set_deshoot_demo_mode_coor,
};

hi_s32 pq_mng_register_ds(pq_reg_type type)
{
    hi_s32 ret = HI_SUCCESS;

    ret = pq_comm_alg_register(HI_PQ_MODULE_DS, type, PQ_BIN_ADAPT_MULTIPLE, "ds", &g_ds_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_ds(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_DS);

    return ret;
}
