/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: db mng layer source file
 * Author: pq
 * Create:
 */

#include "drv_pq_table.h"
#include "pq_mng_db.h"

#define MAX_GLOBAL_DB_STR_LUM  15

static alg_db g_pq_db_info = { 0 };

hi_s32 pq_mng_init_db(pq_bin_param *param, hi_bool para_use_table_default)
{
    hi_s32 ret;
    if (param == HI_NULL) {
        HI_ERR_PQ("Invalid para, null pointer.\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    if (g_pq_db_info.is_init == HI_TRUE) {
        HI_WARN_PQ("db alg has been already initialized.\n");
        return HI_SUCCESS;
    }

    /* init default table! */
    ret = pq_table_init_phy_list(0, HI_PQ_MODULE_DB, PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
    PQ_CHECK_RETURN_SUCCESS(ret);

    g_pq_db_info.is_init = HI_TRUE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_deinit_db(hi_void)
{
    if (g_pq_db_info.is_init == HI_FALSE) {
        HI_WARN_PQ("db alg not initialized.\n");
        return HI_SUCCESS;
    }

    g_pq_db_info.is_init = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_db_en(hi_bool on_off)
{
    hi_s32 ret;
    hi_u32 handle_no = 0;

    PQ_CHECK_INIT_RE_FAIL(g_pq_db_info.is_init);

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; ++handle_no) {
        if (pq_hal_check_vpss_valid(handle_no) == HI_SUCCESS) {
            ret = pq_hal_db_set_en(handle_no, on_off);
            if (ret != HI_SUCCESS) {
                HI_ERR_PQ("set vpss->%d enable->%d failed!\n", handle_no, on_off);
                return ret;
            }
        }
    }

    g_pq_db_info.enable = on_off;
    return HI_SUCCESS;
}

hi_s32 pq_mng_get_db_en(hi_bool *on_off)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);
    PQ_CHECK_INIT_RE_FAIL(g_pq_db_info.is_init);

    *on_off = g_pq_db_info.enable;

    return HI_SUCCESS;
}

static hi_u64 pq_mng_get_db_mixed_key(hi_u64 addr, hi_u8 lsb, hi_u8 msb, hi_u8 src_mode, hi_u8 out_mode)
{
    return (addr << 23) | (lsb  << 18) | (msb  << 13) |  /* 23,18,13 are offset bits. */
           (src_mode << 9) | (out_mode << 5) | ((hi_u32)HI_PQ_MODULE_DB); /* 9,5 are offset bits. */
}

static hi_s32 pq_mng_get_db_basic_strength(pq_source_mode source_mode, pq_output_mode output_mode, hi_u32 *basic_str)
{
    hi_s32 ret;
    hi_u64 key;
    pq_mixed_key_reg *mixed_key_reg_array = pq_table_get_mixed_key_reg_array();

    PQ_CHECK_NULL_PTR_RE_FAIL(basic_str);

    key = pq_mng_get_db_mixed_key(0xa0b4, 18, 21, source_mode, output_mode); /* 0xa0b4,18,21 is global_db_str_lum */
    ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, basic_str);
    if (ret != HI_SUCCESS) {
        /* get strength=50% value from default table. */
        key = pq_mng_get_db_mixed_key(0xa0b4, 18, 21, /* 0xa0b4,18,21 is global_db_str_lum */
                                      PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
        ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, basic_str);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("find default global_db_str_lum value failed!\n");
            return ret;
        }
    }

    return HI_SUCCESS;
}

/* strength[0,100] */
hi_s32 pq_mng_set_db_strength(hi_u32 strength)
{
    hi_s32 ret;
    hi_u32 base_strength;
    hi_u32 handle_no = 0;
    hi_u32 db_str;
    pq_source_mode source_mode = pq_comm_get_source_mode();
    pq_output_mode output_mode = pq_comm_get_output_mode();

    PQ_CHECK_INIT_RE_FAIL(g_pq_db_info.is_init);

    if (strength > PQ_ALG_MAX_VALUE) {
        HI_ERR_PQ("strength->%d is invalid!\n", strength);
        return HI_ERR_PQ_INVALID_PARA;
    }

    ret = pq_mng_get_db_basic_strength(source_mode, output_mode, &base_strength);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_mng_get_db_basic_strength failed!\n");
        return ret;
    }

    db_str = (strength * base_strength * 2) / 100; /* 2,100: base_strength is 50% strength defaultly. */
    /* adjust actual value to valid rang. */
    if (db_str > MAX_GLOBAL_DB_STR_LUM) {
        db_str = MAX_GLOBAL_DB_STR_LUM;
    }

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; ++handle_no) {
        if (pq_hal_check_vpss_valid(handle_no) == HI_SUCCESS) {
            ret = pq_hal_db_set_strength(handle_no, db_str);
            if (ret != HI_SUCCESS) {
                HI_ERR_PQ("set vpss->%d strength->%d failed!\n", handle_no, strength);
                return ret;
            }
        }
    }

    g_pq_db_info.strength = strength;

    return HI_SUCCESS;
}

/* strength[0,100] */
hi_s32 pq_mng_get_db_strength(hi_u32 *strength)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(strength);
    PQ_CHECK_INIT_RE_FAIL(g_pq_db_info.is_init);

    *strength = g_pq_db_info.strength; /* 0~100 */

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_db_demo_en(hi_bool on_off)
{
    hi_u32 handle_no = 0;
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_INIT_RE_FAIL(g_pq_db_info.is_init);

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; ++handle_no) {
        if (pq_hal_check_vpss_valid(handle_no) == HI_SUCCESS) {
            ret = pq_hal_db_set_demo_enable(handle_no, on_off);
            if (ret != HI_SUCCESS) {
                HI_ERR_PQ("set vpss->%d demo enable->%d failed!\n", handle_no, on_off);
                return ret;
            }
        }
    }

    g_pq_db_info.demo_enable = on_off;

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_db_demo_en(hi_bool *on_off)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);
    PQ_CHECK_INIT_RE_FAIL(g_pq_db_info.is_init);

    *on_off = g_pq_db_info.demo_enable;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_db_demo_mode(pq_demo_mode mode)
{
    hi_u32 handle_no;
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_INIT_RE_FAIL(g_pq_db_info.is_init);

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (pq_hal_check_vpss_valid(handle_no) == HI_SUCCESS) {
            ret = pq_hal_db_set_demo_mode(handle_no, mode);
            if (ret != HI_SUCCESS) {
                HI_ERR_PQ("set vpss->%d demo mode->%d failed!\n", handle_no, mode);
                return ret;
            }
        }
    }

    g_pq_db_info.demo_mode = mode;

    return ret;
}

hi_s32 pq_mng_get_db_demo_mode(pq_demo_mode *demo_mode)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(demo_mode);
    PQ_CHECK_INIT_RE_FAIL(g_pq_db_info.is_init);

    *demo_mode = g_pq_db_info.demo_mode;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_db_demo_mode_coor(hi_u32 x_pos)
{
    hi_s32 ret;
    hi_u32 handle_no;
    PQ_CHECK_INIT_RE_FAIL(g_pq_db_info.is_init);

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (pq_hal_check_vpss_valid(handle_no) == HI_SUCCESS) {
            ret = pq_hal_db_set_demo_pos(handle_no, x_pos);
            if (ret != HI_SUCCESS) {
                HI_ERR_PQ("set vpss->%d demo pos->%d failed!\n", handle_no, x_pos);
                return ret;
            }
        }
    }

    g_pq_db_info.demo_pos = x_pos;
    return HI_SUCCESS;
}

hi_s32 pq_mng_update_db_cfg(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_db_output_info *db_api_output_info)
{
    hi_s32 ret;

    if (vpss_stt_info == HI_NULL || db_api_output_info == HI_NULL) {
        HI_ERR_PQ("Invalid para, null pointer.\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    PQ_CHECK_INIT_RE_FAIL(g_pq_db_info.is_init);

#ifndef PQ_SOFTWARE_ALG_SUPPORT
    ret = pq_hal_db_set_boarder_flag(vpss_stt_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_hal_db_set_boarder_flag failed! ret->%#x\n", ret);
        return ret;
    }
#else
    ret = pq_hal_db_set_detect_api_reg(vpss_stt_info, &db_api_output_info->db_det_output_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("set db detect info failed! ret->%#x\n", ret);
        return ret;
    }

    ret = pq_hal_db_set_filter_api_reg(vpss_stt_info, &db_api_output_info->db_filter_output_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("set db filter info failed! ret->%#x\n", ret);
        return ret;
    }
#endif

    return HI_SUCCESS;
}

hi_s32 pq_mng_update_db_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_db_input_info *db_api_input_info)
{
    hi_s32 ret;

    if (vpss_stt_info == HI_NULL || db_api_input_info == HI_NULL) {
        HI_ERR_PQ("Invalid para, null pointer.\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    PQ_CHECK_INIT_RE_FAIL(g_pq_db_info.is_init);

    db_api_input_info->handle = vpss_stt_info->handle_id;
    db_api_input_info->width  = vpss_stt_info->width;
    db_api_input_info->height = vpss_stt_info->height;
    db_api_input_info->bit_depth = DB_BIT_DEPTH;

    ret = pq_hal_db_get_detect_info(vpss_stt_info, &db_api_input_info->db_det_input_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("get db detect info failed! ret->%#x\n", ret);
        return ret;
    }

    ret = pq_hal_db_get_filter_info(vpss_stt_info, &db_api_input_info->db_filter_input_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("get db detect info failed! ret->%#x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

static pq_alg_funcs g_db_funcs = {
    .init               = pq_mng_init_db,
    .deinit             = pq_mng_deinit_db,
    .set_enable         = pq_mng_set_db_en,
    .get_enable         = pq_mng_get_db_en,
    .set_strength       = pq_mng_set_db_strength,
    .get_strength       = pq_mng_get_db_strength,
    .set_demo           = pq_mng_set_db_demo_en,
    .get_demo           = pq_mng_get_db_demo_en,
    .set_demo_mode      = pq_mng_set_db_demo_mode,
    .get_demo_mode      = pq_mng_get_db_demo_mode,
    .set_demo_mode_coor = pq_mng_set_db_demo_mode_coor,
    .update_db_stt_info = pq_mng_update_db_stt_info,
    .update_db_cfg      = pq_mng_update_db_cfg,
};

hi_s32 pq_mng_register_db(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_DB, REG_TYPE_VPSS, PQ_BIN_ADAPT_MULTIPLE, "db", &g_db_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_db(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_DB);

    return ret;
}


