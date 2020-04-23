/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: dci mng function.
 * Author: pq
 * Create: 2019-01-01
 */

#include "drv_pq_define.h"
#include "drv_pq_comm.h"
#include "drv_pq_table.h"
#include "pq_hal_comm.h"

#include "pq_mng_dci.h"
#include "pq_hal_dci.h"

#define GAIN_ADDR       0x445fc
#define MAX_GAIN_VALUE  63
#define MIN_INPUT_WIDTH 256

static alg_dci g_dci_info = { 0 };
static drv_pq_mem_info g_dci_buf_addr = { 0 };
static pq_dci_cfg_proc g_dci_proc_info = { 0 };

static hi_s32 pq_mng_init_dci(pq_bin_param *param, hi_bool b_para_use_table_default)
{
    hi_s32 ret;
    drv_pq_mem_attr attr = { 0 };

    if (param == HI_NULL) {
        HI_ERR_PQ("Invalid para, null pointer.\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    if (g_dci_info.is_init == HI_TRUE) {
        HI_WARN_PQ("dci alg has been already initialized.\n");
        return HI_SUCCESS;
    }

    /* init default table! */
    ret = pq_table_init_phy_list(0, HI_PQ_MODULE_DCI, PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
    PQ_CHECK_RETURN_SUCCESS(ret);

    attr.name = "pq_dci_coef";
    attr.size = DCI_COEF_SIZE;
    attr.mode = OSAL_MMZ_TYPE; /* just cma support */
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = pq_comm_mem_alloc(&attr, &g_dci_buf_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("g_dci_buf_addr malloc failed\n");
        return HI_FAILURE;
    }

    ret = pq_comm_mem_flush(&g_dci_buf_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("g_cle_buf_addr flush failed\n");
        goto dci_deinit;
    }

    ret = pq_hal_dci_distribute_addr(g_dci_buf_addr.vir_addr, g_dci_buf_addr.phy_addr);
    if (ret != HI_SUCCESS) {
        goto dci_deinit;
    }

    g_dci_info.is_init = HI_TRUE;

    return HI_SUCCESS;

dci_deinit:
    if (g_dci_buf_addr.vir_addr != HI_NULL) {
        pq_comm_mem_free((drv_pq_mem_info *)(&(g_dci_buf_addr)));
        g_dci_buf_addr.vir_addr = HI_NULL;
        g_dci_buf_addr.phy_addr = 0;
    }

    pq_hal_dci_reset_addr();
    g_dci_info.is_init = HI_FALSE;
    return HI_FAILURE;
}

static hi_s32 pq_mng_deinit_dci(hi_void)
{
    if (g_dci_info.is_init == HI_FALSE) {
        HI_WARN_PQ("dci alg not init.\n");
        return HI_SUCCESS;
    }

    pq_hal_dci_reset_addr();

    /* release dci coefficient memory */
    if (g_dci_buf_addr.vir_addr != HI_NULL) {
        pq_comm_mem_free((drv_pq_mem_info *)(&(g_dci_buf_addr)));
        g_dci_buf_addr.vir_addr = HI_NULL;
        g_dci_buf_addr.phy_addr = 0;
    }

    memset(&g_dci_info, 0x0, sizeof(g_dci_info));
    g_dci_info.is_init = HI_FALSE;

    return HI_SUCCESS;
}

static hi_void pq_mng_dci_link_policy(hi_bool sr_4k_en, hi_bool sr_8k_en, pq_dci_link_type *link_type)
{
    if ((sr_4k_en != HI_TRUE) && (sr_8k_en != HI_TRUE)) {
        *link_type = DCI_LINK_AFTER_SR;
    } else {
        *link_type = DCI_LINK_BEFORE_SR;
    }

    return;
}

static hi_s32 pq_mng_dci_policy(pq_dci_ref_reso *dci_reso, pq_output_mode output_mode, hi_bool *dci_en)
{
    pq_dci_link_type dci_band_type;

    PQ_CHECK_NULL_PTR_RE_FAIL(dci_reso);
    PQ_CHECK_NULL_PTR_RE_FAIL(dci_en);

    /* close dci when output format is over 8K@60 */
    if (output_mode == PQ_OUTPUT_MODE_8K120) {
        *dci_en = HI_FALSE;
        HI_WARN_PQ("format is over 8K@60, close dci!\n");
        return HI_SUCCESS;
    }

    pq_mng_dci_link_policy(dci_reso->sr_4k_en, dci_reso->sr_8k_en, &dci_band_type);

    /* close dci when input width is below 256 */
    if ((dci_band_type == DCI_LINK_BEFORE_SR) && (dci_reso->sr_4k_in_width < MIN_INPUT_WIDTH)) {
        *dci_en = HI_FALSE;
        HI_WARN_PQ("input width is below 256, close dci!\n");
        return HI_SUCCESS;
    }

    if ((dci_band_type == DCI_LINK_AFTER_SR) && (dci_reso->sr_8k_out_width < MIN_INPUT_WIDTH)) {
        *dci_en = HI_FALSE;
        HI_WARN_PQ("input width is below 256, close dci!\n");
        return HI_SUCCESS;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_dci_cfg_info(alg_dci *dci_raw_data, pq_dci_link_type *link_type,
                                      hi_u32 *in_w, hi_u32 *in_h, hi_u32 *act_demo_pos)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(dci_raw_data);
    PQ_CHECK_NULL_PTR_RE_FAIL(link_type);
    PQ_CHECK_NULL_PTR_RE_FAIL(in_w);
    PQ_CHECK_NULL_PTR_RE_FAIL(in_h);
    PQ_CHECK_NULL_PTR_RE_FAIL(act_demo_pos);

    pq_mng_dci_link_policy(dci_raw_data->dci_ref_reso.sr_4k_en, dci_raw_data->dci_ref_reso.sr_8k_en, link_type);
    if (*link_type == DCI_LINK_AFTER_SR) {
        *in_w = dci_raw_data->dci_ref_reso.sr_8k_out_width;
        *in_h = dci_raw_data->dci_ref_reso.sr_8k_out_height;
        *act_demo_pos = dci_raw_data->demo_pos;
    } else {
        *in_w = dci_raw_data->dci_ref_reso.sr_4k_in_width;
        *in_h = dci_raw_data->dci_ref_reso.sr_4k_in_height;
        if (dci_raw_data->dci_ref_reso.sr_8k_out_width == 0) {
            HI_ERR_PQ("8K SR out w cannot be zero!\n");
            return HI_ERR_PQ_INVALID_PARA;
        }
        *act_demo_pos = (dci_raw_data->demo_pos * dci_raw_data->dci_ref_reso.sr_4k_in_width) /
            dci_raw_data->dci_ref_reso.sr_8k_out_width;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_dci_en(hi_bool enable)
{
    hi_s32 ret;
    pq_output_mode output_mode = pq_comm_get_output_mode();

    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    if (enable) {
        ret = pq_mng_dci_policy(&g_dci_info.dci_ref_reso, output_mode, &enable);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    ret = pq_hal_dci_set_en(enable);
    PQ_CHECK_RETURN_SUCCESS(ret);

    g_dci_info.enable = enable;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_dci_en(hi_bool *enable)
{
    if (enable == HI_NULL) {
        HI_ERR_PQ("Invalid para, null pointer.\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    *enable = g_dci_info.enable;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_dci_demo_en(hi_bool demo_en)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    ret = pq_hal_dci_set_demo_en(demo_en);
    PQ_CHECK_RETURN_SUCCESS(ret);

    g_dci_info.demo_en = demo_en;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_dci_demo_en(hi_bool *demo_en)
{
    if (demo_en == HI_NULL) {
        HI_ERR_PQ("Null pointer!\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    *demo_en = g_dci_info.demo_en;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_dci_demo_mode(pq_demo_mode demo_mode)
{
    hi_s32 ret;
    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    ret = pq_hal_dci_set_demo_mode(demo_mode);
    PQ_CHECK_RETURN_SUCCESS(ret);

    g_dci_info.demo_mode = demo_mode;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_dci_demo_mode(pq_demo_mode *demo_mode)
{
    if (demo_mode == HI_NULL) {
        HI_ERR_PQ("Null pointer!\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    *demo_mode = g_dci_info.demo_mode;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_dci_demo_pos(hi_u32 x_pos)
{
    hi_s32 ret;
    pq_dci_cfg dci_cfg;
    alg_dci tmp_info;

    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    tmp_info = g_dci_info;
    tmp_info.demo_pos = x_pos;

    ret = pq_mng_get_dci_cfg_info(&tmp_info, &dci_cfg.link_type, &dci_cfg.width, &dci_cfg.height, &dci_cfg.demo_pos);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_dci_set_demo_pos(dci_cfg.width, dci_cfg.height, dci_cfg.demo_pos);
    PQ_CHECK_RETURN_SUCCESS(ret);

    g_dci_info.demo_pos = x_pos;
    return HI_SUCCESS;
}

static hi_u64 pq_mng_get_dci_mixed_key(hi_u64 addr, hi_u8 lsb, hi_u8 msb, hi_u8 src_mode, hi_u8 out_mode)
{
    return (addr << 23) | (lsb  << 18) | (msb  << 13) |  /* 23,18,13 are offset bits. */
           (src_mode << 9) | (out_mode << 5) | ((hi_u32)HI_PQ_MODULE_DCI); /* 9,5 are offset bits. */
}

static hi_s32 pq_mng_get_dci_gain(hi_u32 *gain0, hi_u32 *gain1, hi_u32 *gain2)
{
    hi_s32 ret;
    hi_u64 key;
    pq_source_mode source_mode = pq_comm_get_source_mode();
    pq_output_mode output_mode = pq_comm_get_output_mode();

    pq_mixed_key_reg *mixed_key_reg_array = pq_table_get_mixed_key_reg_array();

    key = pq_mng_get_dci_mixed_key(GAIN_ADDR, 0, 5, source_mode, output_mode); /* 0:lsb, 5:msb */
    ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, gain0);
    if (ret != HI_SUCCESS) {
        key = pq_mng_get_dci_mixed_key(GAIN_ADDR, 0, 5, PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO); /* 0:lsb, 5:msb */
        ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, gain0);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("find default global gain0 value failed!\n");
            return ret;
        }
    }

    key = pq_mng_get_dci_mixed_key(GAIN_ADDR, 6, 11, source_mode, output_mode); /* 6:lsb, 11:msb */
    ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, gain1);
    if (ret != HI_SUCCESS) {
        key = pq_mng_get_dci_mixed_key(GAIN_ADDR, 6, 11, PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO); /* 6:lsb, 11:msb */
        ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, gain1);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("find default global gain1 value failed!\n");
            return ret;
        }
    }

    key = pq_mng_get_dci_mixed_key(GAIN_ADDR, 12, 17, source_mode, output_mode); /* 12:lsb, 17:msb */
    ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, gain2);
    if (ret != HI_SUCCESS) {
        key = pq_mng_get_dci_mixed_key(GAIN_ADDR, 12, 17, PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO); /* 12:lsb, 17:msb */
        ret = pq_table_search_reg_value(mixed_key_reg_array, PHY_REG_MAX, key, gain2);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("find default global gain2 value failed!\n");
            return ret;
        }
    }

    return HI_SUCCESS;
}

static hi_void pq_mng_adjust_dci_max_gain(hi_u32 *gain0, hi_u32 *gain1, hi_u32 *gain2)
{
    *gain0 = (*gain0 > MAX_GAIN_VALUE) ? MAX_GAIN_VALUE : *gain0;
    *gain1 = (*gain1 > MAX_GAIN_VALUE) ? MAX_GAIN_VALUE : *gain1;
    *gain2 = (*gain2 > MAX_GAIN_VALUE) ? MAX_GAIN_VALUE : *gain2;
    return;
}

static hi_s32 pq_mng_set_dci_strength(hi_u32 strength)
{
    hi_s32 ret;
    hi_u32 table_gain0;
    hi_u32 global_gain0;
    hi_u32 table_gain1;
    hi_u32 global_gain1;
    hi_u32 table_gain2;
    hi_u32 global_gain2;

    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    if (strength > PQ_ALG_MAX_VALUE) {
        HI_ERR_PQ("strength->%d is invalid!\n", strength);
        return HI_ERR_PQ_INVALID_PARA;
    }

    ret = pq_mng_get_dci_gain(&table_gain0, &table_gain1, &table_gain2);
    PQ_CHECK_RETURN_SUCCESS(ret);

    /* table gain is equal to 50% strength, so should re-calculate gain according to new strength. */
    global_gain0 = (strength * table_gain0 * 2) / 100; /* 2,100 are convert rate */
    global_gain1 = (strength * table_gain1 * 2) / 100; /* 2,100 are convert rate */
    global_gain2 = (strength * table_gain2 * 2) / 100; /* 2,100 are convert rate */

    pq_mng_adjust_dci_max_gain(&global_gain0, &global_gain1, &global_gain2);
    ret = pq_hal_dci_set_strength(global_gain0, global_gain1, global_gain2);
    PQ_CHECK_RETURN_SUCCESS(ret);

    g_dci_info.strength = strength;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_dci_strength(hi_u32 *strength)
{
    if (strength == HI_NULL) {
        HI_ERR_PQ("Null pointer!\n");
        return HI_ERR_PQ_NULL_PTR;
    }

    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    *strength = g_dci_info.strength;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_dci_histgram(hi_pq_dci_histgram *dci_hist)
{
    hi_s32 ret;
    pq_dci_cfg dci_cfg;
    hi_u32 win_size;
    PQ_CHECK_NULL_PTR_RE_FAIL(dci_hist);
    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    ret = pq_mng_get_dci_cfg_info(&g_dci_info, &dci_cfg.link_type, &dci_cfg.width, &dci_cfg.height, &dci_cfg.demo_pos);
    PQ_CHECK_RETURN_SUCCESS(ret);

    win_size = dci_cfg.width * dci_cfg.height;
    if (win_size == 0 || dci_hist->dci_histgram_32.histgram_site >= DCI_HISTGRAM_SITE_BUTT) {
        HI_ERR_PQ("win_size->%d is invalid or histgram_site type is invalid.\n", win_size);
        return HI_ERR_PQ_INVALID_PARA;
    }

    ret = pq_hal_dci_get_histgram(dci_hist, win_size);

    return ret;
}

static hi_s32 pq_mng_set_dci_reso(pq_dci_ref_reso *dci_reso)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(dci_reso);
    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    g_dci_info.dci_ref_reso.sr_4k_en = dci_reso->sr_4k_en;
    g_dci_info.dci_ref_reso.sr_8k_en = dci_reso->sr_8k_en;
    g_dci_info.dci_ref_reso.sr_4k_in_height = dci_reso->sr_4k_in_height;
    g_dci_info.dci_ref_reso.sr_4k_in_width  = dci_reso->sr_4k_in_width;
    g_dci_info.dci_ref_reso.sr_8k_out_height = dci_reso->sr_8k_out_height;
    g_dci_info.dci_ref_reso.sr_8k_out_width  = dci_reso->sr_8k_out_width;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_update_proc_info(alg_dci *dci_cfg_info, pq_dci_link_type link_type, pq_dci_cfg_proc *dci_proc_info)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(dci_cfg_info);
    PQ_CHECK_NULL_PTR_RE_FAIL(dci_proc_info);

    dci_proc_info->dci_enable = dci_cfg_info->enable;
    dci_proc_info->dci_reso   = dci_cfg_info->dci_ref_reso;
    dci_proc_info->link_type  = link_type;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_update_dci_cfg(hi_void)
{
    hi_s32 ret;
    pq_dci_cfg dci_cfg;

    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    ret = pq_mng_get_dci_cfg_info(&g_dci_info, &dci_cfg.link_type, &dci_cfg.width, &dci_cfg.height, &dci_cfg.demo_pos);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_mng_update_proc_info(&g_dci_info, dci_cfg.link_type, &g_dci_proc_info);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_dci_update_cfg(&dci_cfg);
    PQ_CHECK_RETURN_SUCCESS(ret);

    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_dci_proc(pq_dci_cfg_proc *dci_proc)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(dci_proc);
    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    *dci_proc = g_dci_proc_info;
    return HI_SUCCESS;
}

static hi_s32 pq_mng_get_dci_mean_value(hi_u32 *mean_value)
{
    hi_s32 ret;
    PQ_CHECK_NULL_PTR_RE_FAIL(mean_value);
    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    ret = pq_hal_dci_get_mean_value(mean_value);

    return ret;
}

static hi_s32 pq_mng_set_dci_scd(hi_bool scene_change_en)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    ret = pq_hal_dci_set_scd_en(scene_change_en);
    PQ_CHECK_RETURN_SUCCESS(ret);

    g_dci_info.scd_en = scene_change_en;

    return HI_SUCCESS;
}


static hi_s32 pq_mng_get_dci_scd(hi_bool *scene_change_en)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(scene_change_en);
    PQ_CHECK_INIT_RE_FAIL(g_dci_info.is_init);

    *scene_change_en = g_dci_info.scd_en;

    return HI_SUCCESS;
}

static pq_alg_funcs g_dci_funcs = {
    .init               = pq_mng_init_dci,
    .deinit             = pq_mng_deinit_dci,
    .set_enable         = pq_mng_set_dci_en,
    .get_enable         = pq_mng_get_dci_en,
    .set_demo           = pq_mng_set_dci_demo_en,
    .get_demo           = pq_mng_get_dci_demo_en,
    .set_demo_mode      = pq_mng_set_dci_demo_mode,
    .get_demo_mode      = pq_mng_get_dci_demo_mode,
    .set_demo_mode_coor = pq_mng_set_dci_demo_pos,
    .set_strength       = pq_mng_set_dci_strength,
    .get_strength       = pq_mng_get_dci_strength,
    .get_dci_histgram   = pq_mng_get_dci_histgram,
    .set_dci_reso       = pq_mng_set_dci_reso,
    .update_dci_cfg     = pq_mng_update_dci_cfg,
    .get_dci_proc_info  = pq_mng_get_dci_proc,
    .get_dci_mean_value = pq_mng_get_dci_mean_value,
    .set_dci_scd        = pq_mng_set_dci_scd,
    .get_dci_scd        = pq_mng_get_dci_scd,
};

hi_s32 pq_mng_register_dci(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_DCI, REG_TYPE_VDP, PQ_BIN_ADAPT_MULTIPLE, "dci", &g_dci_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_dci(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_DCI);

    return ret;
}
