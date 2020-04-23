/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq mng tnr api
 * Author: pq
 * Create: 2016-01-1
 */

#include "drv_pq_table.h"
#include "pq_mng_tnr.h"
#include "pq_hal_comm.h"
#include "pq_hal_tnr.h"

static alg_tnr g_sg_st_pqtnr_info;
hi_s32 pq_mng_init_tnr(pq_bin_param *pq_param, hi_bool para_use_table_default)
{
    hi_u32 handle_no = 0;
    hi_s32 ret;

    if (g_sg_st_pqtnr_info.init == HI_TRUE) {
        return HI_SUCCESS;
    }

    g_sg_st_pqtnr_info.enable = HI_TRUE;
    g_sg_st_pqtnr_info.strength = PQ_ALG_DEFAULT_VALUE;
    g_sg_st_pqtnr_info.demo_enable = HI_FALSE;
    g_sg_st_pqtnr_info.demo_mode = PQ_DEMO_ENABLE_R;

    /* TNR use HD para, MCNR get SD para, resset later */
    ret = pq_table_init_phy_list(handle_no, HI_PQ_MODULE_TNR, PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("TNR init_phy_list error\n");
        g_sg_st_pqtnr_info.init = HI_FALSE;

        return HI_FAILURE;
    }

    ret = pq_hal_set_nr_vir_reg();
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("set nr vir reg error\n");
        g_sg_st_pqtnr_info.init = HI_FALSE;

        return HI_FAILURE;
    }

    g_sg_st_pqtnr_info.init = HI_TRUE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_deinit_tnr(hi_void)
{
    if (g_sg_st_pqtnr_info.init == HI_FALSE) {
        return HI_SUCCESS;
    }

    g_sg_st_pqtnr_info.init = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_tnr_en(hi_bool on_off)
{
    hi_u32 handle_no;
    pq_source_mode source_mode = pq_comm_get_source_mode();
    pq_output_mode output_mode = pq_comm_get_output_mode();

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqtnr_info.init);

    g_sg_st_pqtnr_info.enable = on_off;
    if ((source_mode < PQ_SOURCE_MODE_MAX)
        && (HI_FALSE == pq_comm_get_moudle_ctrl(HI_PQ_MODULE_TNR, source_mode, output_mode))) {
        g_sg_st_pqtnr_info.enable = HI_FALSE;
    }

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (HI_SUCCESS == pq_hal_check_vpss_valid(handle_no)) {
            pq_hal_enable_tnr(handle_no, g_sg_st_pqtnr_info.enable);
        }
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_tnr_en(hi_bool *on_off)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);

    *on_off = g_sg_st_pqtnr_info.enable;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_tnr_demo_en(hi_bool on_off)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 handle_no;

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqtnr_info.init);

    g_sg_st_pqtnr_info.demo_enable = on_off;

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (HI_SUCCESS == pq_hal_check_vpss_valid(handle_no)) {
            ret = pq_hal_enable_tnr_demo(handle_no, on_off);
        }
    }

    return ret;
}

hi_s32 pq_mng_get_tnr_demo_en(hi_bool *on_off)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);

    *on_off = g_sg_st_pqtnr_info.demo_enable;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_tnr_demo_mode(pq_demo_mode mode)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 handle_no;

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqtnr_info.init);

    g_sg_st_pqtnr_info.demo_mode = mode;

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (HI_SUCCESS == pq_hal_check_vpss_valid(handle_no)) {
            ret = pq_hal_set_tnr_demo_mode(handle_no, (tnr_demo_mode)mode);
        }
    }

    return ret;
}

hi_s32 pq_mng_get_tnr_demo_mode(pq_demo_mode *pen_mode)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(pen_mode);

    *pen_mode = g_sg_st_pqtnr_info.demo_mode;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_tnr_strength(hi_u32 tnr_str)
{
    hi_u32 handle_no;
    hi_s32 ret = HI_SUCCESS;

    if (tnr_str > PQ_ALG_MAX_VALUE) {
        HI_ERR_PQ("the value[%u] of setting NR strength is out of range!\n", tnr_str);
        return HI_FAILURE;
    }

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqtnr_info.init);

    g_sg_st_pqtnr_info.strength = tnr_str;

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (HI_SUCCESS == pq_hal_check_vpss_valid(handle_no)) {
            ret = pq_hal_set_tnr_str(handle_no, tnr_str);
        }
    }

    return ret;
}

hi_s32 pq_mng_get_tnr_strength(hi_u32 *tnr_str)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(tnr_str);

    *tnr_str = g_sg_st_pqtnr_info.strength;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_tnr_demo_mode_coor(hi_u32 x_pos)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 handle_no;

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqtnr_info.init);

    for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
        if (HI_SUCCESS == pq_hal_check_vpss_valid(handle_no)) {
            ret = pq_hal_set_tnr_demo_mode_coor(handle_no, x_pos);
        }
    }

    return ret;
}
#ifdef PQ_DPT_V900_SUPPORT
hi_s32 pq_mng_update_dci_histgram(pq_tnr_dci_status *dci_histgram)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(dci_histgram);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DCI, get_enable)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DCI)->get_enable(&(dci_histgram->dci_en));
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DCI, get_dci_histgram)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DCI)->get_dci_histgram(&(dci_histgram->dci_histgram));
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    return HI_SUCCESS;
}
#endif

hi_s32 pq_mng_update_tnr_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_nr_input_info *nr_api_input)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqtnr_info.init);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_stt_info);

#ifdef PQ_DPT_V900_SUPPORT
    ret = pq_mng_update_dci_histgram(&(g_tnr_dci_info[vpss_stt_info->handle_id]));
    PQ_CHECK_RETURN_SUCCESS(ret);
#endif

    ret = pq_hal_update_tnr_stt_info(vpss_stt_info, nr_api_input);

    return ret;
}

hi_s32 pq_mng_update_tnr_cfg(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_nr_api_output_reg *nr_output_reg)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqtnr_info.init);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_stt_info);

    ret = pq_hal_update_tnr_cfg(vpss_stt_info->handle_id, nr_output_reg);

    return ret;
}

static pq_alg_funcs g_tnr_funcs = {
    .init = pq_mng_init_tnr,
    .deinit = pq_mng_deinit_tnr,
    .set_enable = pq_mng_set_tnr_en,
    .get_enable = pq_mng_get_tnr_en,
    .set_demo = pq_mng_set_tnr_demo_en,
    .get_demo = pq_mng_get_tnr_demo_en,
    .set_demo_mode = pq_mng_set_tnr_demo_mode,
    .get_demo_mode = pq_mng_get_tnr_demo_mode,
    .set_strength = pq_mng_set_tnr_strength,
    .get_strength = pq_mng_get_tnr_strength,
    .set_demo_mode_coor = pq_mng_set_tnr_demo_mode_coor,
    .update_tnr_stt_info = pq_mng_update_tnr_stt_info,
    .update_tnr_cfg = pq_mng_update_tnr_cfg,
};

hi_s32 pq_mng_register_tnr(pq_reg_type type)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_TNR, type, PQ_BIN_ADAPT_MULTIPLE, "tnr", &g_tnr_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_tnr(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_TNR);

    return ret;
}

