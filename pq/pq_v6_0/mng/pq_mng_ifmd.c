/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: pq mng ifmd api
 * Author: pq
 * Create: 2019-01-1
 */

#include "hi_type.h"
#include "pq_mng_ifmd.h"
#include "pq_hal_fmd.h"
#include "pq_mng_fod_alg.h"

pq_fmd_alg g_pq_ifmd_info = {0};

hi_s32 pq_mng_init_ifmd(pq_bin_param* pq_param, hi_bool para_use_table_default)
{
    if (g_pq_ifmd_info.is_ifmd_init == HI_TRUE) {
        return HI_SUCCESS;
    }

    pq_hal_set_ifmd_vir_reg();
    g_pq_ifmd_info.is_ifmd_init = HI_TRUE;
    g_pq_ifmd_info.is_ifmd_enable = HI_TRUE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_deinit_ifmd(hi_void)
{
    if (g_pq_ifmd_info.is_ifmd_init == HI_FALSE) {
        return HI_SUCCESS;
    }

    g_pq_ifmd_info.is_ifmd_init = HI_FALSE;
    g_pq_ifmd_info.is_ifmd_enable = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_init_fod_param(hi_u32 handle)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_pq_ifmd_info.is_ifmd_init);
    PQ_CHECK_OVER_RANGE_RE_FAIL(handle, VPSS_HANDLE_NUM);

    ret = pq_mng_fod_init_param(handle);

    return ret;
}

hi_s32 pq_mng_set_fmd_debug_en(hi_bool on_off)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_pq_ifmd_info.is_ifmd_init);

    ret = pq_hal_set_ifmd_debug_en(on_off);

    return ret;
}

hi_s32 pq_mng_set_fmd_en(hi_bool on_off)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_pq_ifmd_info.is_ifmd_init);

    ret = pq_hal_enable_ifmd(on_off);
    g_pq_ifmd_info.is_ifmd_enable = on_off;

    return ret;
}

hi_s32 pq_mng_get_fmd_en(hi_bool *on_off)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_pq_ifmd_info.is_ifmd_init);
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);

    *on_off = g_pq_ifmd_info.is_ifmd_enable;

    return ret;
}

hi_s32 pq_mng_update_fod_ifmd_stt(hi_drv_pq_vpss_stt_info* vpss_stt_info, hi_drv_pq_vpss_cfg_info* vpss_cfg_info)
{
    hi_s32 ret;
    hi_s32 field_order = 0;

    PQ_CHECK_INIT_RE_FAIL(g_pq_ifmd_info.is_ifmd_init);
    PQ_CHECK_OVER_RANGE_RE_FAIL(vpss_stt_info->handle_id, VPSS_HANDLE_NUM);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_stt_info);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_cfg_info);

    ret = pq_mng_field_order_dect(vpss_stt_info, &field_order);

    if (field_order == 2) { /* 2: field ordr */
        vpss_cfg_info->ifmd_playback.field_order = HI_DRV_PQ_FIELD_UNKOWN;
        pq_hal_set_bt_mode(vpss_stt_info->handle_id, vpss_stt_info->vdec_info.field_order);
    } else if (field_order == 1) {
        vpss_cfg_info->ifmd_playback.field_order = HI_DRV_PQ_FIELD_BOTTOM_FIRST;
        pq_hal_set_bt_mode(vpss_stt_info->handle_id, field_order);
    } else if (field_order == 0) {
        vpss_cfg_info->ifmd_playback.field_order = HI_DRV_PQ_FIELD_TOP_FIRST;
        pq_hal_set_bt_mode(vpss_stt_info->handle_id, field_order);
    }

    pq_mng_update_ifmd_api(vpss_stt_info, &vpss_cfg_info->ifmd_playback);

    return ret;
}

static pq_alg_funcs g_fmd_funcs = {
    .init = pq_mng_init_ifmd,
    .deinit = pq_mng_deinit_ifmd,
    .set_enable = pq_mng_set_fmd_en,
    .get_enable = pq_mng_get_fmd_en,
    .init_fod_param = pq_mng_init_fod_param,
    .init_ifmd_param = pq_mng_ifmd_sofeware_para_init,
    .update_fod_ifmd_stt = pq_mng_update_fod_ifmd_stt,
    .get_fod_dect_info = pq_mng_get_fod_detect_info,
    .set_force_fod_mode = pq_mng_set_fod_mode,
    .get_force_fod_mode = pq_mng_get_fod_mode,
    .set_debug_en = pq_mng_set_fmd_debug_en,
};

hi_s32 pq_mng_register_fmd(pq_reg_type type)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_FMD, type, PQ_BIN_ADAPT_SINGLE, "fmd", &g_fmd_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_fmd()
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_FMD);

    return ret;
}


