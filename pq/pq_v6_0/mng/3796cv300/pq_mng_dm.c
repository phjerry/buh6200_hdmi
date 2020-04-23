/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: pq mng dm api
 * Author: pq
 * Create: 2019-01-1
 */

#include <linux/string.h>

#include "drv_pq_table.h"
#include "pq_mng_dm.h"

#define MAX_VALUE_4BIT 16
static const hi_u8 g_dm_dir_str_gain_lut[8] = { 8, 8, 8, 8, 8, 8, 6, 5 }; /* 8: size */
static const hi_u8 g_dm_dir_str_lut[16] = { 8, 8, 8, 8, 8, 8, 8, 6,       /* 16: size */
                                            1, 1, 0, 0, 0, 0, 0, 0 };

static const hi_u8 g_dm_dir_str_lut_split[16] = { 4, 4, 4, 4, 4, 4, 4, 3, /* 16:size */
                                                  1, 1, 0, 0, 0, 0, 0, 0 };
static const hi_u8 g_dm_sw_wht_lut[5] = { 4, 8, 16, 32, 64 };             /* 5: array size */

static alg_dm g_sg_st_pqdm_info;
hi_u32 g_dm_str = 8;

dm_base_info g_dm_inf[VPSS_HANDLE_NUM];

/* 获取DR模块的初始参数值 */
static hi_s32 dm_init_value(dm_param *dr_value)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(dr_value);

    dr_value->opp_ang_ctrst_t = 20;       /* u8,[0,255]; default value:20 */
    dr_value->mn_dir_opp_ctrst_t = 18;    /* u8,[0,255];       default value:18 */
    dr_value->sw_trsnt_lt = 40;           /* u8,[0,255];       default value:40 */
    dr_value->sw_trsnt_lt10bit = 160;     /* u10,[0,1023];     default value:160 */
    dr_value->lsw_ratio = 4;              /* u3,[2,6];         default value:4 */
    dr_value->limit_lsw_ratio = 4;        /* u3,[2,6];         default value:4 */
    dr_value->mmflr = 50;                 /* s9,[-256,255];    default value:50;   MMFLR > MMFSR */
    dr_value->mmflr10bit = 200;           /* s11,[-1024,1023]; default value:200;  MMFLR10bit > MMFSR10bit */
    dr_value->mmfsr = 0;                  /* s9,[-256,255];    default value:0 */
    dr_value->mmfsr10bit = 0;             /* s11,[-1024,1023]; default value:0 */
    dr_value->mm_flimit_en = 1;  /* u1,[0,1]; default value:1;  1: enable the MMF limit condition; 0: disable */
    dr_value->mmf_set = 0;       /* u1,[0,1];  default value:0; 0:MMF[0,3], 1:MMF[1,2] */
    dr_value->init_val_step = 0; /* u2,[0,3];  default value:0 */

    dr_value->limit_t = 18;               /* u8,[0,255];   default value:18 */
    dr_value->limit_t10bit = 72;          /* u10,[0,1023]; default value:72 */
    dr_value->lim_res_blend_str1 = 7;     /* u4,[0,8];     default value:7; */
    dr_value->lim_res_blend_str2 = 7;     /* u4,[0,8];     default value:7; */
    dr_value->dir_blend_str = 7;          /* u4,[0,8];     default value:7; */

    dr_value->lw_ctrst_t       = 30;      /* u8,[0,255];   default value:30 */
    dr_value->lw_ctrst_t10bit  = 120;     /* u10,[0,1023]; default value:120 */
    dr_value->sw_trsnt_st      = 0;       /* u8,[0,255];   default value:0 */
    dr_value->sw_trsnt_st10bit = 0;       /* u10,[0,1023]; default value:0 */

    dr_value->grad_sub_ratio = 24;        /* u5,[0,31]; default value:24 */
    dr_value->ctrst_thresh = 2;           /* u2,[0,3];  default value:2 */
    dr_value->opp_ang_ctrst_div = 2;      /* u2,[1,3];  default value:2 */
    dr_value->dm_global_str = 8;          /* u4,[0,15]; default value:8 */

    dr_value->dir_str_gain_lut = g_dm_dir_str_gain_lut;          /* u4,[0,8]; */
    dr_value->dir_str_lut      = g_dm_dir_str_lut;               /* u4,[0,8]; */
    dr_value->sw_wht_lut       = g_dm_sw_wht_lut;                /* u7,[0,127]; */

    return HI_SUCCESS;
}

hi_s32 pq_mng_init_dm(pq_bin_param *pq_param, hi_bool para_use_table_default)
{
    dm_param param_value_dr;
    hi_u32 handle = 0;
    hi_s32 ret;

    if (g_sg_st_pqdm_info.init == HI_TRUE) {
        return HI_SUCCESS;
    }

    g_sg_st_pqdm_info.enable = HI_FALSE;
    g_sg_st_pqdm_info.strength = PQ_ALG_DEFAULT_VALUE;
    g_sg_st_pqdm_info.demo_enable = HI_FALSE;

    ret = dm_init_value(&param_value_dr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("DM parameter init error!\n");
        g_sg_st_pqdm_info.init = HI_FALSE;
        return HI_FAILURE;
    }

    ret = pq_table_init_phy_list(handle, HI_PQ_MODULE_DM, PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("dm init_phy_list error\n");
        g_sg_st_pqdm_info.init = HI_FALSE;
        return HI_FAILURE;
    }

    g_sg_st_pqdm_info.init = HI_TRUE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_deinit_dm(hi_void)
{
    if (g_sg_st_pqdm_info.init == HI_FALSE) {
        return HI_SUCCESS;
    }

    g_sg_st_pqdm_info.init = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_dm_en(hi_bool on_off)
{
    hi_s32 ret;
    hi_u32 handle = 0;
    pq_source_mode source_mode = pq_comm_get_source_mode();
    pq_output_mode output_mode = pq_comm_get_output_mode();

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqdm_info.init);

    if ((source_mode > PQ_SOURCE_MODE_4K)
        || (pq_comm_get_moudle_ctrl(HI_PQ_MODULE_DM, source_mode, output_mode) == HI_FALSE)) {
        on_off = HI_FALSE;
    }

    g_sg_st_pqdm_info.enable = on_off;

    if (pq_comm_get_alg_type_id(HI_PQ_MODULE_DM) == REG_TYPE_VPSS) {
        for (handle = 0; handle < VPSS_HANDLE_NUM; handle++) {
            if (pq_hal_check_vpss_valid(handle) == HI_SUCCESS) {
                ret = pq_hal_enable_dm(handle, on_off);
                PQ_CHECK_RETURN_SUCCESS(ret);
            }
        }
    } else {
        HI_ERR_PQ("DM regester error !\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_dm_en(hi_bool *on_off)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);
    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqdm_info.init);

    *on_off = g_sg_st_pqdm_info.enable;

    return HI_SUCCESS;
}

/* strength: 0~100 */
hi_s32 pq_mng_set_dm_strength(hi_u32 strength)
{
    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqdm_info.init);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(strength, PQ_ALG_MAX_VALUE);

    g_dm_str = strength * MAX_VALUE_4BIT / PQ_ALG_MAX_VALUE;
    if (g_dm_str >= MAX_VALUE_4BIT) {
        g_dm_str = MAX_VALUE_4BIT - 1;
    };
    g_sg_st_pqdm_info.strength = strength;

    pq_hal_set_dm_strength(0, g_dm_str);

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_dm_strength(hi_u32 *strength)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(strength);

    *strength = g_sg_st_pqdm_info.strength;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_dm_demo_en(hi_bool on_off)
{
    hi_u32 handle = 0;
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqdm_info.init);

    g_sg_st_pqdm_info.demo_enable = on_off;

    if (pq_comm_get_alg_type_id(HI_PQ_MODULE_DM) == REG_TYPE_VPSS) {
        for (handle = 0; handle < VPSS_HANDLE_NUM; ++handle) {
            if (pq_hal_check_vpss_valid(handle) == HI_SUCCESS) {
                ret = pq_hal_enable_dm_demo(handle, on_off);
                PQ_CHECK_RETURN_SUCCESS(ret);
            }
        }
    } else {
        HI_ERR_PQ("DM regester error !\n");
        return HI_FAILURE;
    }

    return ret;
}

hi_s32 pq_mng_get_dm_demo_en(hi_bool *on_off)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);

    *on_off = g_sg_st_pqdm_info.demo_enable;

    return HI_SUCCESS;
}

hi_s32 pq_mng_dm_param_refresh(hi_u32 handle_no, hi_u32 height, hi_u32 width, hi_u32 f_rate)
{
    return HI_SUCCESS;
}

hi_s32 pq_mng_update_dm_cfg(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_dm_output_reg *dm_api_out)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_stt_info);
    PQ_CHECK_NULL_PTR_RE_FAIL(dm_api_out);

    pq_hal_set_dm_api_reg(vpss_stt_info->handle_id, dm_api_out);

    return HI_SUCCESS;
}

hi_s32 pq_mng_dm_api_init(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_dm_input_info *dm_api_input)
{
    dm_api_input->y_width = vpss_stt_info->width;
    dm_api_input->y_height = vpss_stt_info->height;
    dm_api_input->dm_global_str = g_dm_str;

    memset(dm_api_input->rgmv_x_buf, 0, sizeof(dm_api_input->rgmv_x_buf));
    memset(dm_api_input->rgmv_y_buf, 0, sizeof(dm_api_input->rgmv_y_buf));
    memset(dm_api_input->rgmv_mag_buf, 0, sizeof(dm_api_input->rgmv_mag_buf));
    memset(dm_api_input->rgmv_mag_buf, 0, sizeof(dm_api_input->rgmv_mag_buf));
    memset(dm_api_input->transband, 0, sizeof(dm_api_input->transband));

    return HI_SUCCESS;
}

hi_s32 pq_mng_update_dm_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_dm_input_info *dm_api_input)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_stt_info);
    PQ_CHECK_NULL_PTR_RE_FAIL(dm_api_input);

    ret = pq_mng_dm_api_init(vpss_stt_info, dm_api_input);
    PQ_CHECK_RETURN_SUCCESS(ret);
    ret = pq_hal_update_dm_stt_info(vpss_stt_info, dm_api_input);
    PQ_CHECK_RETURN_SUCCESS(ret);
    return ret;
}

static pq_alg_funcs g_dm_funcs = {
    .init = pq_mng_init_dm,
    .deinit = pq_mng_deinit_dm,
    .set_enable = pq_mng_set_dm_en,
    .get_enable = pq_mng_get_dm_en,
    .set_strength = pq_mng_set_dm_strength,
    .get_strength = pq_mng_get_dm_strength,
    .set_demo = pq_mng_set_dm_demo_en,
    .get_demo = pq_mng_get_dm_demo_en,
    .update_dm_stt_info = pq_mng_update_dm_stt_info,
    .update_dm_cfg = pq_mng_update_dm_cfg,
};

hi_s32 pq_mng_register_dm(pq_reg_type type)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_DM, type, PQ_BIN_ADAPT_MULTIPLE, "dm", &g_dm_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_dm(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_DM);

    return ret;
}

