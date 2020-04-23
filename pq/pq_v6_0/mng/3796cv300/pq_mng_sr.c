/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description:
 * Author: pq
 * Create: 2019-09-29
 */
#include "drv_pq_define.h"
#include "drv_pq_comm.h"
#include "drv_pq_table.h"
#include "pq_hal_comm.h"
#include "pq_mng_sr.h"
#include "pq_hal_sr.h"

/* ************************ global variable definition ************************* */
static hi_bool g_sr_init_flag = HI_FALSE;
static hi_bool g_sr_enable = HI_TRUE;

/***************************************************************************************
* func          : pq_mng_init_sr
* description   : sr initialization
* param[in]     : pq_bin_param, para_use_table_default flag.
* retval        : NA
* others:       : NA
***************************************************************************************/
static hi_s32 pq_mng_init_sr(pq_bin_param *param, hi_bool para_use_table_default)
{
    hi_s32 ret;

    if (g_sr_init_flag) {
        return HI_SUCCESS;
    }

    /* init default table! */
    ret = pq_table_init_phy_list(0, HI_PQ_MODULE_SR, PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_sr_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_hal_sr_init err!\n");
        return ret;
    }

    g_sr_init_flag = HI_TRUE;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_deinit_sr(hi_void)
{
    hi_s32 ret;

    if (g_sr_init_flag == HI_FALSE) {
        return HI_SUCCESS;
    }

    ret = pq_hal_sr_deinit();
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_hal_sr_deinit err!\n");
        return ret;
    }

    g_sr_init_flag = HI_FALSE;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_sr_scale_mode(pq_sr_id id, hi_u32 scale_mode)
{
    hi_s32 ret;

    if (g_sr_init_flag != HI_TRUE) {
        HI_ERR_PQ("SR is not init!\n");
        return HI_FAILURE;
    }

    if (id >= SR_ID_MAX) {
        HI_ERR_PQ("sr id->%d is invalid!\n", id);
        return HI_FAILURE;
    }

    if (scale_mode > 8 || scale_mode < 0) { /* 0~8 is scale mode. */
        HI_ERR_PQ("sr scale mode->%d is invalid!\n", scale_mode);
        return HI_FAILURE;
    }

    ret = pq_hal_sr_set_scale_mode(id, scale_mode);

    return ret;
}

static hi_s32 pq_mng_set_sr_enable(hi_bool on_or_off)
{
    if (g_sr_init_flag != HI_TRUE) {
        HI_ERR_PQ("SR is not init!\n");
        return HI_FAILURE;
    }

    g_sr_enable = on_or_off;
    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_sr_enable(hi_bool *on_or_off)
{
    if (g_sr_init_flag != HI_TRUE) {
        HI_ERR_PQ("SR is not init!\n");
        return HI_FAILURE;
    }

    *on_or_off = g_sr_enable;

    return HI_SUCCESS;
}

static pq_alg_funcs g_sr_funcs = {
    .init       = pq_mng_init_sr,
    .deinit     = pq_mng_deinit_sr,
    .set_enable = pq_mng_set_sr_enable,
    .get_enable = pq_mng_get_sr_enable,
    .set_sr_scale_mode = pq_mng_set_sr_scale_mode,
};

hi_s32 pq_mng_register_sr(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_SR, REG_TYPE_VDP, PQ_BIN_ADAPT_SINGLE, "sr", &g_sr_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_sr(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_SR);

    return ret;
}
