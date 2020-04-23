/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq dei api
 * Author: pq
 * Create: 2016-01-1
 */

#include "hi_drv_mem.h"
#include "pq_mng_dei.h"
#include "drv_pq_table.h"

static alg_dei g_pq_dei_proc_info;

/* init DEI */
hi_s32 pq_mng_init_dei(pq_bin_param *pq_param, hi_bool para_use_table_default)
{
    hi_s32 ret;
    hi_u32 handle_no = 0;

    if (g_pq_dei_proc_info.init == HI_TRUE) {
        return HI_SUCCESS;
    }

    g_pq_dei_proc_info.enable = HI_TRUE;
    g_pq_dei_proc_info.fr_count_mc = 0;
    g_pq_dei_proc_info.demo_enable = HI_FALSE;
    g_pq_dei_proc_info.demo_mode = PQ_DEMO_ENABLE_R;
    g_pq_dei_proc_info.mc_only_enable = HI_FALSE;

    ret = pq_table_init_phy_list(handle_no, HI_PQ_MODULE_DEI, PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("dei init_phy_list error\n");
        g_pq_dei_proc_info.init = HI_FALSE;

        return HI_FAILURE;
    }

    g_pq_dei_proc_info.init = HI_TRUE;

    return HI_SUCCESS;
}

/* deinit DEI */
hi_s32 pq_mng_deinit_dei(hi_void)
{
    if (g_pq_dei_proc_info.init == HI_FALSE) {
        return HI_SUCCESS;
    }

    g_pq_dei_proc_info.init = HI_FALSE;

    return HI_SUCCESS;
}

/* enable or disable dei demo */
hi_s32 pq_mng_set_dei_demo_en(hi_bool on_off)
{
    hi_s32 ret;
    hi_u32 handle_no;

    PQ_CHECK_INIT_RE_FAIL(g_pq_dei_proc_info.init);

    g_pq_dei_proc_info.demo_enable = on_off;

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (pq_hal_check_vpss_valid(handle_no) == HI_SUCCESS) {
            ret = pq_hal_enable_dei_demo(handle_no, on_off);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

hi_s32 pq_mng_get_dei_demo_en(hi_bool *on_off)
{
    PQ_CHECK_INIT_RE_FAIL(g_pq_dei_proc_info.init);
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);

    *on_off = g_pq_dei_proc_info.demo_enable;
    return HI_SUCCESS;
}

/* set dei demo mode */
hi_s32 pq_mng_set_dei_demo_mode(pq_demo_mode mode)
{
    hi_s32 ret;
    hi_u32 handle_no;

    PQ_CHECK_INIT_RE_FAIL(g_pq_dei_proc_info.init);

    g_pq_dei_proc_info.demo_mode = mode;

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (pq_hal_check_vpss_valid(handle_no) == HI_SUCCESS) {
            ret = pq_hal_set_dei_demo_mode(handle_no, (pq_dei_demo_mode)mode);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

/* get dei demo mode */
hi_s32 pq_mng_get_dei_demo_mode(pq_demo_mode *pen_mode)
{
    PQ_CHECK_INIT_RE_FAIL(g_pq_dei_proc_info.init);
    PQ_CHECK_NULL_PTR_RE_FAIL(pen_mode);

    *pen_mode = g_pq_dei_proc_info.demo_mode;

    return HI_SUCCESS;
}

/* set vpss market coordinate */
hi_s32 pq_mng_set_dei_demo_mode_coor(hi_u32 x_pos)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 handle_no;

    PQ_CHECK_INIT_RE_FAIL(g_pq_dei_proc_info.init);

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (pq_hal_check_vpss_valid(handle_no)  == HI_SUCCESS) {
            ret = pq_hal_set_dei_demo_mode_coor(handle_no, x_pos);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

hi_s32 pq_mng_dei_get_dci_mean_value(hi_u32 *mean_value)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(mean_value);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DCI, get_dci_mean_value)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DCI)->get_dci_mean_value(mean_value);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_update_dei_stt_info(hi_drv_pq_vpss_stt_info *info_in, drv_pq_dei_input_info *dei_api_input)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_pq_dei_proc_info.init);
    PQ_CHECK_NULL_PTR_RE_FAIL(info_in);
    PQ_CHECK_NULL_PTR_RE_FAIL(dei_api_input);

    ret = pq_mng_dei_get_dci_mean_value(&g_dei_mean_value_array[info_in->handle_id]);
    PQ_CHECK_RETURN_SUCCESS(ret);
    ret = pq_hal_get_dei_stt_info(info_in, dei_api_input);

    return ret;
}

hi_s32 pq_mng_update_dei_cfg(hi_u32 handle_no, hi_drv_pq_dei_api_output_reg *dei_api_out)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_pq_dei_proc_info.init);
    PQ_CHECK_OVER_RANGE_RE_FAIL(handle_no, VPSS_HANDLE_NUM);
    PQ_CHECK_NULL_PTR_RE_FAIL(dei_api_out);

    ret = pq_hal_update_dei_cfg(handle_no, dei_api_out);

    return ret;
}

hi_s32 pq_mng_update_dei_rc_info(hi_u32 handle_no, hi_u32 width, hi_u32 height)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_pq_dei_proc_info.init);
    PQ_CHECK_OVER_RANGE_RE_FAIL(handle_no, VPSS_HANDLE_NUM);

    ret = pq_hal_update_dei_rc_info(handle_no, width, height);

    return ret;
}

hi_s32 pq_mng_update_field_flag(hi_u32 handle_no, hi_u32 frm_num)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_pq_dei_proc_info.init);
    PQ_CHECK_OVER_RANGE_RE_FAIL(handle_no, VPSS_HANDLE_NUM);

    ret = pq_hal_update_field_flag(handle_no, frm_num);

    return ret;
}

hi_s32 pq_mng_set_mc_only_enable(hi_bool on_off)
{
    hi_u32 handle;

    PQ_CHECK_INIT_RE_FAIL(g_pq_dei_proc_info.init);

    g_pq_dei_proc_info.mc_only_enable = on_off;
    for (handle = 0; handle < VPSS_HANDLE_NUM; handle++) {
        if (HI_SUCCESS == pq_hal_check_vpss_valid(handle)) {
            pq_hal_set_mc_only_enable(handle, g_pq_dei_proc_info.mc_only_enable);
        }
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_mc_only_enable(hi_bool *on_off)
{
    PQ_CHECK_INIT_RE_FAIL(g_pq_dei_proc_info.init);
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);

    *on_off = g_pq_dei_proc_info.mc_only_enable;

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_ma_only_enable(hi_bool *on_off)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_pq_dei_proc_info.init);
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);

    ret = pq_hal_get_ma_only_enable(0, on_off);

    return ret;
}


static pq_alg_funcs g_dei_funcs = {
    .init = pq_mng_init_dei,
    .deinit = pq_mng_deinit_dei,
    .set_demo = pq_mng_set_dei_demo_en,
    .get_demo = pq_mng_get_dei_demo_en,
    .set_demo_mode = pq_mng_set_dei_demo_mode,
    .get_demo_mode = pq_mng_get_dei_demo_mode,
    .set_demo_mode_coor = pq_mng_set_dei_demo_mode_coor,
    .update_dei_stt_info = pq_mng_update_dei_stt_info,
    .update_dei_cfg = pq_mng_update_dei_cfg,
    .update_dei_rc_info = pq_mng_update_dei_rc_info,
    .update_field_flag = pq_mng_update_field_flag,
    .set_mc_only_enable = pq_mng_set_mc_only_enable,
    .get_mc_only_enable = pq_mng_get_mc_only_enable,
    .get_ma_only_enable = pq_mng_get_ma_only_enable,
};

hi_s32 pq_mng_register_dei(pq_reg_type type)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_DEI, type, PQ_BIN_ADAPT_MULTIPLE, "dei", &g_dei_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_dei(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_DEI);

    return ret;
}


