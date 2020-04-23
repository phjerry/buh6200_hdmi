/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: cle mng function.
 * Author: pq
 * Create: 2019-09-21
 */
#include "drv_pq_define.h"
#include "drv_pq_comm.h"
#include "drv_pq_table.h"
#include "pq_hal_comm.h"
#include "pq_mng_cle.h"
#include "pq_hal_cle.h"

/* ***************************** macro definition ****************************** */
#define MIN_IN_OUT_SCALE 15

/* ************************ global variable definition ************************* */
static alg_cle g_cle_info = {0};
static drv_pq_mem_info g_cle_buf_addr = {0};
static hi_bool g_cle_enable[CLE_ID_MAX] = { HI_TRUE, HI_TRUE };

/***************************************************************************************
 * func          : pq_mng_init_cle
 * description   : cle initialization
 * param[in]     : pq_bin_param, para_use_table_default flag.
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static hi_s32 pq_mng_init_cle(pq_bin_param *param, hi_bool para_use_table_default)
{
    hi_s32 ret;
    drv_pq_mem_attr attr = { 0 };

    if (param == HI_NULL) {
        HI_ERR_PQ("Invalid para, null pointer.\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    if (g_cle_info.is_init == HI_TRUE) {
        HI_WARN_PQ("cle alg has been already initialized.\n");
        return HI_SUCCESS;
    }

    /* init default table! */
    ret = pq_table_init_phy_list(0, HI_PQ_MODULE_CLE, PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
    PQ_CHECK_RETURN_SUCCESS(ret);

    attr.name = "pq_cle_coef";
    attr.size = CLE_COEF_SIZE;
    attr.mode = OSAL_MMZ_TYPE; /* just smmu support */
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = pq_comm_mem_alloc(&attr, &g_cle_buf_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("g_cle_buf_addr malloc failed\n");
        return HI_FAILURE;
    }

    ret = pq_comm_mem_flush(&g_cle_buf_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("g_cle_buf_addr flush failed\n");
        goto CLE_DEINIT;
    }

    ret = pq_hal_cle_distribute_addr(g_cle_buf_addr.vir_addr, g_cle_buf_addr.phy_addr);
    if (ret != HI_SUCCESS) {
        goto CLE_DEINIT;
    }

    g_cle_info.is_init = HI_TRUE;

    /* reset dfx status. */
    g_cle_enable[CLE_ID_V0_4K] = HI_TRUE;
    g_cle_enable[CLE_ID_V0_8K] = HI_TRUE;
    return HI_SUCCESS;

CLE_DEINIT:
    if (g_cle_buf_addr.vir_addr != HI_NULL) {
        pq_comm_mem_free((drv_pq_mem_info *)(&(g_cle_buf_addr)));
        g_cle_buf_addr.vir_addr = HI_NULL;
        g_cle_buf_addr.phy_addr = 0;
    }

    pq_hal_cle_reset_addr();
    g_cle_info.is_init = HI_FALSE;
    return HI_FAILURE;
}

static hi_s32 pq_mng_deinit_cle(hi_void)
{
    if (g_cle_info.is_init == HI_FALSE) {
        return HI_SUCCESS;
    }

    pq_hal_cle_reset_addr();

    /* release cle coefficient memory */
    if (g_cle_buf_addr.vir_addr != HI_NULL) {
        pq_comm_mem_free((drv_pq_mem_info *)(&(g_cle_buf_addr)));
        g_cle_buf_addr.vir_addr = HI_NULL;
        g_cle_buf_addr.phy_addr = 0;
    }

    memset(&g_cle_info, 0x0, sizeof(g_cle_info));
    g_cle_info.is_init = HI_FALSE;

    /* reset dfx status. */
    g_cle_enable[CLE_ID_V0_4K] = HI_TRUE;
    g_cle_enable[CLE_ID_V0_8K] = HI_TRUE;

    return HI_SUCCESS;
}

static hi_u32 pq_mng_get_min_scale(hi_u32 in_w, hi_u32 in_h, hi_u32 out_w, hi_u32 out_h)
{
    hi_u32 scale_w;
    hi_u32 scale_h;

    scale_w = (in_w * 10) / out_w; /* 10 :a number */
    scale_h = (in_h * 10) / out_h; /* 10 :a number */

    return (scale_w < scale_h) ? scale_w : scale_h;
}

static hi_s32 pq_mng_4k_cle_policy(const pq_cle_reso *cle_reso, hi_bool *enable)
{
    hi_u32  scale;
    hi_bool sharpen_en = HI_FALSE;

    PQ_CHECK_NULL_PTR_RE_FAIL(cle_reso);
    PQ_CHECK_NULL_PTR_RE_FAIL(enable);
    PQ_CHECK_ZERO_RE_FAIL(cle_reso->video_out_width);
    PQ_CHECK_ZERO_RE_FAIL(cle_reso->video_out_height);

    /* MIN(VIDEO_IW/VIDEO_OW,VIDEO_IH/VIDEO_OH)>1.5 should close 4k-cle. */
    scale = pq_mng_get_min_scale(cle_reso->video_in_width, cle_reso->video_in_height,
                                 cle_reso->video_out_width, cle_reso->video_out_height);
    if (scale > MIN_IN_OUT_SCALE) {
        *enable = HI_FALSE;
        return HI_SUCCESS;
    }

    /* moudle_in_reso = (FHD,4K], open 4k-cle; otherwise close 4k-cle. */
    if ((cle_reso->cle_in_width <= PQ_WIDTH_1920 || cle_reso->cle_in_height <= PQ_HEIGHT_1080) ||
        (cle_reso->cle_in_width > PQ_WIDTH_3840 || cle_reso->cle_in_height > PQ_HEIGHT_2160) ) {
        *enable = HI_FALSE;
        return HI_SUCCESS;
    }

#ifdef PQ_ALG_SHARPEN
    if (PQ_FUNC_CALL(HI_PQ_MODULE_SHARPNESS, get_enable)) {
        if (GET_ALG_FUN(HI_PQ_MODULE_SHARPNESS)->get_enable(&sharpen_en) != HI_SUCCESS) {
            HI_ERR_PQ("get sharpen enable failed!!\n");
            return HI_FAILURE;
        }
    }
#else
    sharpen_en = HI_FALSE;
#endif
    /* sharpen is open while 4k-clm is close, 4k-cle need to be closed as well. */
    if (cle_reso->clm_enable != HI_TRUE) {
        *enable = HI_FALSE;
        return HI_SUCCESS;
    } else if ((sharpen_en == HI_TRUE) && (cle_reso->clm_enable != HI_TRUE)) {
        *enable = HI_FALSE;
        return HI_SUCCESS;
    }

    *enable = HI_TRUE;
    return HI_SUCCESS;
}

static hi_s32 pq_mng_8k_cle_policy(const pq_cle_reso *cle_reso, hi_bool *enable)
{
    hi_u32  scale;
    hi_bool sharpen_en = HI_FALSE;

    PQ_CHECK_NULL_PTR_RE_FAIL(cle_reso);
    PQ_CHECK_NULL_PTR_RE_FAIL(enable);
    PQ_CHECK_ZERO_RE_FAIL(cle_reso->video_out_width);
    PQ_CHECK_ZERO_RE_FAIL(cle_reso->video_out_height);

    /* MIN(VIDEO_IW/VIDEO_OW,VIDEO_IH/VIDEO_OH)>1.5 should close 8k-cle. */
    scale = pq_mng_get_min_scale(cle_reso->video_in_width, cle_reso->video_in_height,
                                 cle_reso->video_out_width, cle_reso->video_out_height);
    if (scale > MIN_IN_OUT_SCALE) {
        *enable = HI_FALSE;
        return HI_SUCCESS;
    }

    /* moudle_in_reso > 4K, open 8k-cle; otherwise close 8k-cle. */
    if (cle_reso->cle_in_width <= PQ_WIDTH_3840 || cle_reso->cle_in_height <= PQ_HEIGHT_2160) {
        *enable = HI_FALSE;
        return HI_SUCCESS;
    }

    /* fmt = 8K, fmt rate <=60 open 8k-cle; otherwise close 8k-cle. */
    if (pq_comm_get_output_mode() == PQ_OUTPUT_MODE_8K120) {
        *enable = HI_FALSE;
        return HI_SUCCESS;
    }

#ifdef PQ_ALG_SHARPEN
    if (PQ_FUNC_CALL(HI_PQ_MODULE_SHARPNESS, get_enable)) {
        if (GET_ALG_FUN(HI_PQ_MODULE_SHARPNESS)->get_enable(&sharpen_en) != HI_SUCCESS) {
            HI_ERR_PQ("get sharpen enable failed!!\n");
            return HI_FAILURE;
        }
    }
#else
    sharpen_en = HI_FALSE;
#endif

    /* sharpen is open while 8k-clm is close, 8k-cle need to be closed. */
    if (cle_reso->clm_enable != HI_TRUE) {
        *enable = HI_FALSE;
        return HI_SUCCESS;
    } else if ((sharpen_en == HI_TRUE) && (cle_reso->clm_enable != HI_TRUE)) {
        *enable = HI_FALSE;
        return HI_SUCCESS;
    }

    *enable = HI_TRUE;
    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_cle_reso(pq_cle_id id, const pq_cle_reso *cle_reso)
{
    hi_bool cle_enable = HI_TRUE;

    if (id >= CLE_ID_MAX) {
        HI_ERR_PQ("Invalid para, cle id->%d is over range.\n", id);
        return HI_ERR_PQ_INVALID_PARA;
    }

    if (cle_reso == HI_NULL) {
        HI_ERR_PQ("Invalid para, null pointer.\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    if (g_cle_info.is_init == HI_FALSE) {
        HI_ERR_PQ("CLE alg is not init!\n");
        return HI_ERR_PQ_NO_INIT;
    }

    if (g_cle_enable[id] != HI_TRUE) {
        cle_enable = HI_FALSE;
    }

    if (cle_enable) {
        /* 4k cle enable policy. */
        if (id == CLE_ID_V0_4K) {
            if (pq_mng_4k_cle_policy(cle_reso, &cle_enable) != HI_SUCCESS) {
                cle_enable = HI_FALSE;
            }
        }

        /* 8k cle enable policy. */
        if (id == CLE_ID_V0_8K) {
            if (pq_mng_8k_cle_policy(cle_reso, &cle_enable) != HI_SUCCESS) {
                cle_enable = HI_FALSE;
            }
        }
    }

    g_cle_info.cle_reso[id].cle_enable = cle_enable;
    g_cle_info.cle_reso[id].clm_enable = cle_reso->clm_enable;

    g_cle_info.cle_reso[id].video_in_width = cle_reso->video_in_width;
    g_cle_info.cle_reso[id].video_in_height = cle_reso->video_in_height;
    g_cle_info.cle_reso[id].cle_in_width = cle_reso->cle_in_width;
    g_cle_info.cle_reso[id].cle_in_height = cle_reso->cle_in_height;
    g_cle_info.cle_reso[id].video_out_width = cle_reso->video_out_width;
    g_cle_info.cle_reso[id].video_out_height = cle_reso->video_out_height;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_cle_reso(pq_cle_id id, pq_cle_reso *cle_reso)
{
    if (id >= CLE_ID_MAX) {
        HI_ERR_PQ("Invalid para, cle id->%d is over range.\n", id);
        return HI_ERR_PQ_INVALID_PARA;
    }

    if (cle_reso == HI_NULL) {
        HI_ERR_PQ("Invalid para, null pointer.\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    if (g_cle_info.is_init != HI_TRUE) {
        HI_ERR_PQ("Cle is not init!\n");
        return HI_ERR_PQ_NO_INIT;
    }

    cle_reso->cle_enable = g_cle_info.cle_reso[id].cle_enable;
    cle_reso->clm_enable = g_cle_info.cle_reso[id].clm_enable;
    cle_reso->video_in_width   = g_cle_info.cle_reso[id].video_in_width;
    cle_reso->video_in_height  = g_cle_info.cle_reso[id].video_in_height;
    cle_reso->cle_in_width     = g_cle_info.cle_reso[id].cle_in_width;
    cle_reso->cle_in_height    = g_cle_info.cle_reso[id].cle_in_height;
    cle_reso->video_out_width  = g_cle_info.cle_reso[id].video_out_width;
    cle_reso->video_out_height = g_cle_info.cle_reso[id].video_out_height;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_update_cle_cfg(hi_void)
{
    if (g_cle_info.is_init != HI_TRUE) {
        HI_ERR_PQ("Cle is not init!\n");
        return HI_ERR_PQ_NO_INIT;
    }

    return pq_hal_cle_update_cfg(&g_cle_info);
}

static hi_s32 pq_mng_get_cle_cfg_by_proc(pq_cle_cfg_proc *cle_proc)
{
    hi_u32 in_w;
    hi_u32 in_h;
    hi_u32 out_w;
    hi_u32 out_h;

    PQ_CHECK_NULL_PTR_RE_FAIL(cle_proc);

    if (g_cle_info.is_init != HI_TRUE) {
        HI_ERR_PQ("Cle is not init!\n");
        return HI_ERR_PQ_NO_INIT;
    }

    if ((g_cle_info.cle_reso[CLE_ID_V0_4K].video_out_width != 0) && (g_cle_info.cle_reso[CLE_ID_V0_4K].video_out_height != 0)) {
        in_w  = g_cle_info.cle_reso[CLE_ID_V0_4K].video_in_width;
        in_h  = g_cle_info.cle_reso[CLE_ID_V0_4K].video_in_height;
        out_w = g_cle_info.cle_reso[CLE_ID_V0_4K].video_out_width;
        out_h = g_cle_info.cle_reso[CLE_ID_V0_4K].video_out_height;
        cle_proc->cle_min_scale[CLE_ID_V0_4K] = pq_mng_get_min_scale(in_w, in_h, out_w, out_h);
    } else {
        cle_proc->cle_min_scale[CLE_ID_V0_4K] = 0;
    }

    if ((g_cle_info.cle_reso[CLE_ID_V0_8K].video_out_width != 0) && (g_cle_info.cle_reso[CLE_ID_V0_8K].video_out_height != 0)) {
        in_w  = g_cle_info.cle_reso[CLE_ID_V0_8K].video_in_width;
        in_h  = g_cle_info.cle_reso[CLE_ID_V0_8K].video_in_height;
        out_w = g_cle_info.cle_reso[CLE_ID_V0_8K].video_out_width;
        out_h = g_cle_info.cle_reso[CLE_ID_V0_8K].video_out_height;
        cle_proc->cle_min_scale[CLE_ID_V0_8K] = pq_mng_get_min_scale(in_w, in_h, out_w, out_h);
    } else {
        cle_proc->cle_min_scale[CLE_ID_V0_8K] = 0;
    }

    cle_proc->cle_cfg_proc[CLE_ID_V0_4K] = g_cle_info.cle_reso[CLE_ID_V0_4K];
    cle_proc->cle_cfg_proc[CLE_ID_V0_8K] = g_cle_info.cle_reso[CLE_ID_V0_8K];

    cle_proc->cle_enable[CLE_ID_V0_4K] = g_cle_enable[CLE_ID_V0_4K];
    cle_proc->cle_enable[CLE_ID_V0_8K] = g_cle_enable[CLE_ID_V0_8K];

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_cle_enable(hi_bool on_or_off)
{
    if (g_cle_info.is_init != HI_TRUE) {
        HI_ERR_PQ("Cle is not init!\n");
        return HI_ERR_PQ_NO_INIT;
    }

    g_cle_enable[CLE_ID_V0_4K] = on_or_off;
    g_cle_enable[CLE_ID_V0_8K] = on_or_off;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_cle_enable(hi_bool *on_or_off)
{
    pq_output_mode output_mode = pq_comm_get_output_mode();

    if (on_or_off == HI_NULL) {
        HI_ERR_PQ("Invalid para, null pointer.\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    if (g_cle_info.is_init != HI_TRUE) {
        HI_ERR_PQ("Cle is not init!\n");
        return HI_ERR_PQ_NO_INIT;
    }

    if ((output_mode == PQ_OUTPUT_MODE_8K60) || (output_mode == PQ_OUTPUT_MODE_8K120)) {
        *on_or_off = g_cle_info.cle_reso[CLE_ID_V0_8K].cle_enable;
    } else {
        *on_or_off = g_cle_info.cle_reso[CLE_ID_V0_4K].cle_enable;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_mng_cle_ddr_regread(uintptr_t reg_addr, hi_u32 *value)
{
    if (g_cle_info.is_init != HI_TRUE) {
        HI_ERR_PQ("Cle is not init!\n");
        return HI_ERR_PQ_NO_INIT;
    }

    *value = pq_hal_cle_ddr_regread(reg_addr);
    return HI_SUCCESS;
}

static hi_s32 pq_mng_cle_ddr_regwrite(uintptr_t reg_addr, hi_u32 value)
{
    if (g_cle_info.is_init != HI_TRUE) {
        HI_ERR_PQ("Cle is not init!\n");
        return HI_ERR_PQ_NO_INIT;
    }

    return pq_hal_cle_ddr_regwrite(reg_addr, value);
}

static hi_s32 pq_mng_set_cle_demo(hi_bool demo_en)
{
    hi_s32 ret;

    if (g_cle_info.is_init != HI_TRUE) {
        HI_ERR_PQ("Cle is not init!\n");
        return HI_ERR_PQ_NO_INIT;
    }

    ret = pq_hal_cle_set_demo_en(demo_en);
    PQ_CHECK_RETURN_SUCCESS(ret);

    g_cle_info.demo_en = demo_en;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_cle_demo(hi_bool *demo_en)
{
    if (g_cle_info.is_init != HI_TRUE) {
        HI_ERR_PQ("Cle is not init!\n");
        return HI_ERR_PQ_NO_INIT;
    }

    if (demo_en == HI_NULL) {
        HI_ERR_PQ("Null pointer!\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    *demo_en = g_cle_info.demo_en;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_cle_demo_mode(pq_demo_mode demo_mode)
{
    hi_s32 ret;

    if (g_cle_info.is_init != HI_TRUE) {
        HI_ERR_PQ("Cle is not init!\n");
        return HI_ERR_PQ_NO_INIT;
    }

    if (demo_mode >= PQ_DEMO_MAX) {
        HI_ERR_PQ("demo_mode is invalid!\n");
        return HI_ERR_PQ_INVALID_PARA;
    }

    ret = pq_hal_cle_set_demo_mode(demo_mode);
    PQ_CHECK_RETURN_SUCCESS(ret);

    g_cle_info.demo_mode = demo_mode;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_cle_demo_mode(pq_demo_mode *demo_mode)
{
    if (g_cle_info.is_init != HI_TRUE) {
        HI_ERR_PQ("Cle is not init!\n");
        return HI_ERR_PQ_NO_INIT;
    }

    if (demo_mode == HI_NULL) {
        HI_ERR_PQ("Null pointer!\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    *demo_mode = g_cle_info.demo_mode;

    return HI_SUCCESS;
}

static hi_u64 pq_mng_get_cle_mixed_key(hi_u64 addr, hi_u8 lsb, hi_u8 msb, hi_u8 src_mode, hi_u8 out_mode)
{
    return (addr << 23) | (lsb  << 18) | (msb  << 13) |  /* 23,18,13 are offset bits. */
           (src_mode << 9) | (out_mode << 5) | ((hi_u32)HI_PQ_MODULE_CLE); /* 9,5 are offset bits. */
}

static hi_s32 pq_mng_get_4k_reg_gain(pq_source_mode source_mode, pq_output_mode output_mode,
                                     hi_u32 *raisr_gain, hi_u32 *nlsr_gain, hi_u32 *peak_gain)
{
    hi_s32 ret;
    hi_u64 key;
    pq_mixed_key_reg *mixed_key_reg_array = pq_table_get_mixed_key_reg_array();

    key = pq_mng_get_cle_mixed_key(CLE_4K_RAISR_ADDR, CLE_4K_RAISR_LSB, CLE_4K_RAISR_MSB,
                                   source_mode, output_mode);
    ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, raisr_gain);
    if (ret != HI_SUCCESS) {
        key = pq_mng_get_cle_mixed_key(CLE_4K_RAISR_ADDR, CLE_4K_RAISR_LSB, CLE_4K_RAISR_MSB,
                                       PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
        ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, raisr_gain);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("find default 4K_RAISR value failed!\n");
            return ret;
        }
    }

    key = pq_mng_get_cle_mixed_key(CLE_4K_NLSR_ADDR, CLE_4K_NLSR_LSB, CLE_4K_NLSR_MSB,
                                   source_mode, output_mode);
    ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, nlsr_gain);
    if (ret != HI_SUCCESS) {
        key = pq_mng_get_cle_mixed_key(CLE_4K_NLSR_ADDR, CLE_4K_NLSR_LSB, CLE_4K_NLSR_MSB,
                                       PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
        ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, nlsr_gain);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("find default 4K_NLSR value failed!\n");
            return ret;
        }
    }

    key = pq_mng_get_cle_mixed_key(CLE_4K_PEAK_ADDR, CLE_4K_PEAK_LSB, CLE_4K_PEAK_MSB,
                                   source_mode, output_mode);
    ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, peak_gain);
    if (ret != HI_SUCCESS) {
        key = pq_mng_get_cle_mixed_key(CLE_4K_PEAK_ADDR, CLE_4K_PEAK_LSB, CLE_4K_PEAK_MSB,
                                       PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
        ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, peak_gain);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("find default 4K_PEAK value failed!\n");
            return ret;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_8k_reg_gain(pq_source_mode source_mode, pq_output_mode output_mode,
                                     hi_u32 *raisr_gain, hi_u32 *nlsr_gain, hi_u32 *peak_gain)
{
    hi_s32 ret;
    hi_u64 key;
    pq_mixed_key_reg *mixed_key_reg_array = pq_table_get_mixed_key_reg_array();

    key = pq_mng_get_cle_mixed_key(CLE_8K_RAISR_ADDR, CLE_8K_RAISR_LSB, CLE_8K_RAISR_MSB,
                                   source_mode, output_mode);
    ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, raisr_gain);
    if (ret != HI_SUCCESS) {
        key = pq_mng_get_cle_mixed_key(CLE_8K_RAISR_ADDR, CLE_8K_RAISR_LSB, CLE_8K_RAISR_MSB,
                                       PQ_SOURCE_MODE_FHD, PQ_OUTPUT_MODE_8K60);
        ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, raisr_gain);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("find default 8K_RAISR value failed!\n");
            return ret;
        }
    }

    key = pq_mng_get_cle_mixed_key(CLE_8K_NLSR_ADDR, CLE_8K_NLSR_LSB, CLE_8K_NLSR_MSB,
                                   source_mode, output_mode);
    ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, nlsr_gain);
    if (ret != HI_SUCCESS) {
        key = pq_mng_get_cle_mixed_key(CLE_8K_NLSR_ADDR, CLE_8K_NLSR_LSB, CLE_8K_NLSR_MSB,
                                       PQ_SOURCE_MODE_FHD, PQ_OUTPUT_MODE_8K60);
        ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, nlsr_gain);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("find default 8K_NLSR value failed!\n");
            return ret;
        }
    }

    key = pq_mng_get_cle_mixed_key(CLE_8K_PEAK_ADDR, CLE_8K_PEAK_LSB, CLE_8K_PEAK_MSB,
                                   source_mode, output_mode);
    ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, peak_gain);
    if (ret != HI_SUCCESS) {
        key = pq_mng_get_cle_mixed_key(CLE_8K_PEAK_ADDR, CLE_8K_PEAK_LSB, CLE_8K_PEAK_MSB,
                                       PQ_SOURCE_MODE_FHD, PQ_OUTPUT_MODE_8K60);
        ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, peak_gain);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("find default 8K_PEAK value failed!\n");
            return ret;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_cle_strength(hi_u32 strength)
{
    hi_s32 ret;
    pq_source_mode source_mode = pq_comm_get_source_mode();
    pq_output_mode output_mode = pq_comm_get_output_mode();
    hi_u32 table_raisr_gain = 0;
    hi_u32 table_nlsr_gain = 0;
    hi_u32 table_peak_gain = 0;

    if (g_cle_info.is_init != HI_TRUE) {
        HI_ERR_PQ("Cle is not init!\n");
        return HI_ERR_PQ_NO_INIT;
    }

    if (strength > PQ_ALG_MAX_VALUE) {
        HI_ERR_PQ("strength->%d is invalid!\n", strength);
        return HI_ERR_PQ_INVALID_PARA;
    }

    ret = pq_mng_get_4k_reg_gain(source_mode, output_mode, &table_raisr_gain, &table_nlsr_gain, &table_peak_gain);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_mng_get_4k_reg_gain failed, ret->%#x !\n", ret);
        return ret;
    }

    ret = pq_hal_cle_set_strength(CLE_ID_V0_4K, strength, table_raisr_gain, table_nlsr_gain, table_peak_gain);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("set 4k cle strength failed, ret->%#x !\n", ret);
        return ret;
    }

    ret = pq_mng_get_8k_reg_gain(source_mode, output_mode, &table_raisr_gain, &table_nlsr_gain, &table_peak_gain);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_mng_get_8k_reg_gain failed, ret->%#x !\n", ret);
        return ret;
    }

    ret = pq_hal_cle_set_strength(CLE_ID_V0_8K, strength, table_raisr_gain, table_nlsr_gain, table_peak_gain);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("set 8k cle strength failed, ret->%#x !\n", ret);
        return ret;
    }

    g_cle_info.strength = strength;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_cle_strength(hi_u32 *strength)
{
    if (strength == HI_NULL) {
        HI_ERR_PQ("Null pointer!\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    if (g_cle_info.is_init != HI_TRUE) {
        HI_ERR_PQ("Cle is not init!\n");
        return HI_ERR_PQ_NO_INIT;
    }

    *strength = g_cle_info.strength;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_cle_demo_pos(hi_u32 x_pos)
{
    hi_s32 ret;

    if (g_cle_info.is_init != HI_TRUE) {
        HI_ERR_PQ("Cle is not init!\n");
        return HI_ERR_PQ_NO_INIT;
    }

    ret = pq_hal_set_cle_demo_pos(x_pos);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_hal_set_cle_demo_pos failed !\n");
        return ret;
    }

    g_cle_info.demo_pos = x_pos;

    return HI_SUCCESS;
}

static pq_alg_funcs g_cle_funcs = {
    .init           = pq_mng_init_cle,
    .deinit         = pq_mng_deinit_cle,
    .set_enable     = pq_mng_set_cle_enable,
    .get_enable     = pq_mng_get_cle_enable,
    .set_cle_reso   = pq_mng_set_cle_reso,
    .get_cle_reso   = pq_mng_get_cle_reso,
    .set_strength   = pq_mng_set_cle_strength,
    .get_strength   = pq_mng_get_cle_strength,
    .set_demo       = pq_mng_set_cle_demo,
    .get_demo       = pq_mng_get_cle_demo,
    .set_demo_mode  = pq_mng_set_cle_demo_mode,
    .get_demo_mode  = pq_mng_get_cle_demo_mode,
    .set_demo_mode_coor = pq_mng_set_cle_demo_pos,
    .update_cle_cfg = pq_mng_update_cle_cfg,
    .get_cle_cfg_by_proc = pq_mng_get_cle_cfg_by_proc,
    .cle_ddr_regread = pq_mng_cle_ddr_regread,
    .cle_ddr_regwrite = pq_mng_cle_ddr_regwrite,
};

hi_s32 pq_mng_register_cle(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_CLE, REG_TYPE_VDP, PQ_BIN_ADAPT_MULTIPLE, "cle", &g_cle_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_cle(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_CLE);

    return ret;
}
