/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: dering
 * Author: pq
 * Create: 2019-11-7
 */

#include "drv_pq_table.h"
#include "pq_hal_comm.h"
#include "pq_mng_dering.h"

static alg_dr g_dr_info;

hi_s32 pq_mng_init_dering(pq_bin_param* pq_param, hi_bool para_use_table_default)
{
    hi_u32 handle_no = 0;
    hi_s32 ret;

    if (g_dr_info.init == HI_TRUE) {
        return HI_SUCCESS;
    }

    g_dr_info.enable = HI_TRUE;
    g_dr_info.demo_enable = HI_FALSE;
    g_dr_info.demo_mode = PQ_DEMO_ENABLE_R;

    ret = pq_table_init_phy_list(handle_no, HI_PQ_MODULE_DR, PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("dering init_phy_list error\n");
        g_dr_info.init = HI_FALSE;
        return HI_FAILURE;
    }
    g_dr_info.init = HI_TRUE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_deinit_dering(hi_void)
{
    if (g_dr_info.init == HI_FALSE) {
        return HI_SUCCESS;
    }

    g_dr_info.init = HI_FALSE;
    return HI_SUCCESS;
}

hi_s32 pq_mng_set_dering_en(hi_bool on_off)
{
    hi_u32 handle_no;
    hi_s32 ret;
    pq_source_mode source_mode = pq_comm_get_source_mode();
    pq_output_mode output_mode = pq_comm_get_output_mode();

    PQ_CHECK_INIT_RE_FAIL(g_dr_info.init);

    if ((source_mode < PQ_SOURCE_MODE_MAX) &&
        (pq_comm_get_moudle_ctrl(HI_PQ_MODULE_DR, source_mode, output_mode) == HI_FALSE)) {
        on_off = HI_FALSE;
    }

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (pq_hal_check_vpss_valid(handle_no) == HI_SUCCESS) {
            ret = pq_hal_set_dering_enable(handle_no, on_off);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    g_dr_info.enable = on_off;

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_dering_en(hi_bool* on_off)
{
    PQ_CHECK_INIT_RE_FAIL(g_dr_info.init);
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);

    *on_off = g_dr_info.enable;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_dering_demo_en(hi_bool on_off)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 handle_no;

    PQ_CHECK_INIT_RE_FAIL(g_dr_info.init);

    g_dr_info.demo_enable = on_off;

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (pq_hal_check_vpss_valid(handle_no) == HI_SUCCESS) {
            ret = pq_hal_set_dering_demo_enable(handle_no, on_off);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

hi_s32 pq_mng_get_dering_demo_en(hi_bool* on_off)
{
    PQ_CHECK_INIT_RE_FAIL(g_dr_info.init);
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);

    *on_off = g_dr_info.demo_enable;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_dering_demo_mode(pq_demo_mode mode)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 handle_no;

    PQ_CHECK_INIT_RE_FAIL(g_dr_info.init);
    g_dr_info.demo_mode = mode;

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (pq_hal_check_vpss_valid(handle_no) == HI_SUCCESS) {
            ret = pq_hal_set_dering_demo_mode(handle_no, mode);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

hi_s32 pq_mng_get_dering_demo_mode(pq_demo_mode* demo_mode)
{
    PQ_CHECK_INIT_RE_FAIL(g_dr_info.init);
    PQ_CHECK_NULL_PTR_RE_FAIL(demo_mode);

    *demo_mode = g_dr_info.demo_mode;

    return HI_SUCCESS;
}

/* set vpss market coordinate */
hi_s32 pq_mng_set_dering_demo_mode_coor(hi_u32 x_pos)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 handle_no;

    PQ_CHECK_INIT_RE_FAIL(g_dr_info.init);

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (pq_hal_check_vpss_valid(handle_no)  == HI_SUCCESS) {
            ret = pq_hal_set_dering_demo_coor(handle_no, x_pos);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

static pq_alg_funcs g_dr_funcs = {
    .init = pq_mng_init_dering,
    .deinit = pq_mng_deinit_dering,
    .set_enable = pq_mng_set_dering_en,
    .get_enable = pq_mng_get_dering_en,
    .set_demo = pq_mng_set_dering_demo_en,
    .get_demo = pq_mng_get_dering_demo_en,
    .set_demo_mode = pq_mng_set_dering_demo_mode,
    .get_demo_mode = pq_mng_get_dering_demo_mode,
    .set_demo_mode_coor = pq_mng_set_dering_demo_mode_coor,
};

hi_s32 pq_mng_register_dr(pq_reg_type type)
{
    hi_s32 ret = HI_SUCCESS;

    ret = pq_comm_alg_register(HI_PQ_MODULE_DR, type, PQ_BIN_ADAPT_MULTIPLE, "dr", &g_dr_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_dr(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_DR);

    return ret;
}

