/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include <linux/string.h>

#include "pq_mng_gfxhdr.h"
#include "pq_hal_comm.h"

static hi_bool g_gfx_hdr_init_flag = HI_FALSE;

hi_s32 pq_mng_init_gfx_hdr(pq_bin_param *pq_param, hi_bool default_code)
{
    hi_s32 ret;

    if (g_gfx_hdr_init_flag == HI_TRUE) {
        return HI_SUCCESS;
    }

    PQ_CHECK_NULL_PTR_RE_FAIL(pq_param);

    ret = pq_hal_init_gfxhdr(pq_param, default_code);
    if (ret != HI_SUCCESS) {
        g_gfx_hdr_init_flag = HI_FALSE;

        return ret;
    }

    g_gfx_hdr_init_flag = HI_TRUE;

    return ret;
}

hi_s32 pq_mng_deinit_gfx_hdr(hi_void)
{
    if (g_gfx_hdr_init_flag == HI_FALSE) {
        return HI_SUCCESS;
    }

    pq_hal_deinit_gfxhdr();

    g_gfx_hdr_init_flag = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_gfx_hdr_cfg(hi_drv_pq_gfx_layer gfx_layer, drv_pq_internal_gfx_hdr_info *hdr_info)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_info);
    PQ_CHECK_OVER_RANGE_RE_FAIL(gfx_layer, HI_DRV_PQ_GFX_LAYER_MAX);

    ret = pq_hal_get_gfxhdr_cfg(gfx_layer, hdr_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("get gfxhdr cfg is error!\n");
        return HI_FAILURE;
    }

    ret = pq_hal_set_gfxhdr_cfg(gfx_layer, hdr_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("set gfxhdr cfg is error!\n");
    }

    return ret;
}

static hi_s32 pq_mng_set_gfx_hdr_csc_setting(hi_pq_image_param *pic_setting)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(pic_setting);
    PQ_CHECK_INIT_RE_FAIL(g_gfx_hdr_init_flag);

    ret = pq_hal_set_gfxhdr_csc_setting(pic_setting);

    return ret;
}

static hi_s32 pq_mng_set_gfx_hdr_offset(hi_pq_hdr_offset *hdr_offset_para)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_offset_para);
    PQ_CHECK_INIT_RE_FAIL(g_gfx_hdr_init_flag);

    ret = pq_hal_set_gfxhdr_offset(hdr_offset_para);

    return ret;
}

static hi_s32 pq_mng_set_gfx_hdr_tmcurve(hi_pq_hdr_offset *hdr_offset_para)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_offset_para);
    PQ_CHECK_INIT_RE_FAIL(g_gfx_hdr_init_flag);

    ret = pq_hal_set_gfxhdr_tm_curve(hdr_offset_para);

    return ret;
}

static pq_alg_funcs g_gfx_hdr_funcs = {
    .init = pq_mng_init_gfx_hdr,
    .deinit = pq_mng_deinit_gfx_hdr,
    .set_gfx_hdr_cfg = pq_mng_set_gfx_hdr_cfg,
    .set_hdr_csc_video_setting = pq_mng_set_gfx_hdr_csc_setting,
    .set_hdr_offset = pq_mng_set_gfx_hdr_offset,
    .set_hdr_tm_curve = pq_mng_set_gfx_hdr_tmcurve,
};

hi_s32 pq_mng_register_gfx_hdr(pq_reg_type type)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_GFXHDR, type, PQ_BIN_ADAPT_SINGLE, "gfxhdr", &g_gfx_hdr_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_gfx_hdr(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_GFXHDR);

    return ret;
}

