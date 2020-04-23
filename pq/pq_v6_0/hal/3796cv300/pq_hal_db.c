/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: db hal layer source file
 * Author: pq
 * Create:
 */

#include "hi_vpss_register.h"
#include "pq_hal_db.h"
#include "pq_hal_comm.h"

#define DB_BORDER_SPLIT 60 /* 60 is (1920 / 32) */

hi_s32 pq_hal_db_get_detect_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, dbd_input_info *db_detect_info)
{
    vpss_stt_reg_type *stt_info_reg = (vpss_stt_reg_type *)vpss_stt_info->vpss_stt_reg;
    vpss_reg_type *vpss_reg = HI_NULL;
    hi_u32 i;

    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_stt_info);
    PQ_CHECK_NULL_PTR_RE_FAIL(db_detect_info);

    vpss_reg = pq_hal_get_vpss_reg(vpss_stt_info->handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    db_detect_info->y_width = vpss_stt_info->width;
    db_detect_info->y_height = vpss_stt_info->height;

    if (vpss_stt_info->width <= PQ_WIDTH_1920) {
        for (i = 0; i < DB_BORDER_SPLIT; i++) {
            db_detect_info->border_index_buf_hy[i] = stt_info_reg->vpss_wstt_db_border[i].u32;
        }

        for (i = 90; i < 128; i++) { /* buf_vy store from 90 to 128. */
            db_detect_info->border_index_buf_vy[i - 90] = stt_info_reg->vpss_wstt_db_border[i].u32; /* 90 is offset. */
        }
    } else {
        for (i = 0; i < 128; i++) { /* buf_hy store in 128 buf when width over 1920. */
            db_detect_info->border_index_buf_hy[i] = stt_info_reg->vpss_wstt_db_border[i].u32;
            db_detect_info->border_index_buf_vy[i] = 0;
        }
    }

    for (i = 0; i < 2048; i++) { /* 2048 is buf array. */
        db_detect_info->str_row_hy[i] = stt_info_reg->vpss_wstt_db_hy_counter[i].u32;
    }

    db_detect_info->hy_en = vpss_reg->vpss_hipp_snr_reg57.bits.mndet_lumhor_en;
    db_detect_info->vy_en = vpss_reg->vpss_hipp_snr_reg57.bits.mndet_lumver_en;
    db_detect_info->detblk_en = vpss_reg->vpss_hipp_snr_reg57.bits.mndet_en;
    db_detect_info->detsize_hy = stt_info_reg->vpss_wstt_db_hy_size.bits.hy_size;
    db_detect_info->detsize_vy = stt_info_reg->vpss_wstt_db_vy_size.bits.vy_size;
    db_detect_info->bord_str_hy = stt_info_reg->vpss_wstt_db_bord_str.bits.bord_str_hy;
    db_detect_info->counter_border_hy = stt_info_reg->vpss_wstt_db_hy_size.bits.hy_counter;
    db_detect_info->counter_border_vy = stt_info_reg->vpss_wstt_db_vy_size.bits.vy_counter;
    db_detect_info->test_blknum_hy = vpss_reg->vpss_hipp_snr_reg306.bits.test_blk_number_hy;
    db_detect_info->test_blknum_vy = vpss_reg->vpss_hipp_snr_reg306.bits.test_blk_number_vy;
    /* get global motion from tnr moudle. */
    db_detect_info->glb_motion = vpss_reg->vpss_hipp_tnr_reg145.bits.global_motion;

    return HI_SUCCESS;
}

static hi_s32 pq_hal_db_get_filter_h_str_fade_lut(hi_u32 handle_id, hi_s32 *lum_h_str_fade_lut)
{
    vpss_reg_type *vpss_reg = HI_NULL;
    vpss_reg = pq_hal_get_vpss_reg(handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    lum_h_str_fade_lut[0] = vpss_reg->vpss_hipp_snr_reg52.bits.lum_h_str_fade_lut_0;   /* 0 is index. */
    lum_h_str_fade_lut[1] = vpss_reg->vpss_hipp_snr_reg52.bits.lum_h_str_fade_lut_1;   /* 1 is index. */
    lum_h_str_fade_lut[2] = vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_2;   /* 2 is index. */
    lum_h_str_fade_lut[3] = vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_3;   /* 3 is index. */
    lum_h_str_fade_lut[4] = vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_4;   /* 4 is index. */
    lum_h_str_fade_lut[5] = vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_5;   /* 5 is index. */
    lum_h_str_fade_lut[6] = vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_6;   /* 6 is index. */
    lum_h_str_fade_lut[7] = vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_7;   /* 7 is index. */
    lum_h_str_fade_lut[8] = vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_8;   /* 8 is index. */
    lum_h_str_fade_lut[9] = vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_9;   /* 9 is index. */
    lum_h_str_fade_lut[10] = vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_10; /* 10 is index. */
    lum_h_str_fade_lut[11] = vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_11; /* 11 is index. */

    return HI_SUCCESS;
}

static hi_s32 pq_hal_db_get_filter_hor_delta_lut(hi_u32 handle_id, hi_s32 *lum_hor_delta_lut)
{
    vpss_reg_type *vpss_reg = HI_NULL;
    vpss_reg = pq_hal_get_vpss_reg(handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    lum_hor_delta_lut[0] = vpss_reg->vpss_hipp_snr_reg45.bits.lum_hor_delta_lut_0;   /* 0 is index. */
    lum_hor_delta_lut[1] = vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_1;   /* 1 is index. */
    lum_hor_delta_lut[2] = vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_2;   /* 2 is index. */
    lum_hor_delta_lut[3] = vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_3;   /* 3 is index. */
    lum_hor_delta_lut[4] = vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_4;   /* 4 is index. */
    lum_hor_delta_lut[5] = vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_5;   /* 5 is index. */
    lum_hor_delta_lut[6] = vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_6;   /* 6 is index. */
    lum_hor_delta_lut[7] = vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_7;   /* 7 is index. */
    lum_hor_delta_lut[8] = vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_8;   /* 8 is index. */
    lum_hor_delta_lut[9] = vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_9;   /* 9 is index. */
    lum_hor_delta_lut[10] = vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_10; /* 10 is index. */
    lum_hor_delta_lut[11] = vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_11; /* 11 is index. */
    lum_hor_delta_lut[12] = vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_12; /* 12 is index. */
    lum_hor_delta_lut[13] = vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_13; /* 13 is index. */
    lum_hor_delta_lut[14] = vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_14; /* 14 is index. */
    lum_hor_delta_lut[15] = vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_15; /* 15 is index. */
    lum_hor_delta_lut[16] = vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_16; /* 16 is index. */

    return HI_SUCCESS;
}

static hi_s32 pq_hal_db_get_filter_v_str_fade_lut(hi_u32 handle_id, hi_s32 *lum_v_str_fade_lut)
{
    vpss_reg_type *vpss_reg = HI_NULL;
    vpss_reg = pq_hal_get_vpss_reg(handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    lum_v_str_fade_lut[0] = vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_0;   /* 0 is index. */
    lum_v_str_fade_lut[1] = vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_1;   /* 1 is index. */
    lum_v_str_fade_lut[2] = vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_2;   /* 2 is index. */
    lum_v_str_fade_lut[3] = vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_3;   /* 3 is index. */
    lum_v_str_fade_lut[4] = vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_4;   /* 4 is index. */
    lum_v_str_fade_lut[5] = vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_5;   /* 5 is index. */
    lum_v_str_fade_lut[6] = vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_6;   /* 6 is index. */
    lum_v_str_fade_lut[7] = vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_7;   /* 7 is index. */
    lum_v_str_fade_lut[8] = vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_8;   /* 8 is index. */
    lum_v_str_fade_lut[9] = vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_9;   /* 9 is index. */
    lum_v_str_fade_lut[10] = vpss_reg->vpss_hipp_snr_reg55.bits.lum_v_str_fade_lut_10; /* 10 is index. */
    lum_v_str_fade_lut[11] = vpss_reg->vpss_hipp_snr_reg55.bits.lum_v_str_fade_lut_11; /* 11 is index. */

    return HI_SUCCESS;
}

static hi_s32 pq_hal_db_get_filter_ver_delta_lut(hi_u32 handle_id, hi_s32 *lum_ver_delta_lut)
{
    vpss_reg_type *vpss_reg = HI_NULL;
    vpss_reg = pq_hal_get_vpss_reg(handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    lum_ver_delta_lut[0] = vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_0;   /* 0 is index. */
    lum_ver_delta_lut[1] = vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_1;   /* 1 is index. */
    lum_ver_delta_lut[2] = vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_2;   /* 2 is index. */
    lum_ver_delta_lut[3] = vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_3;   /* 3 is index. */
    lum_ver_delta_lut[4] = vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_4;   /* 4 is index. */
    lum_ver_delta_lut[5] = vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_5;   /* 5 is index. */
    lum_ver_delta_lut[6] = vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_6;   /* 6 is index. */
    lum_ver_delta_lut[7] = vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_7;   /* 7 is index. */
    lum_ver_delta_lut[8] = vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_8;   /* 8 is index. */
    lum_ver_delta_lut[9] = vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_9;   /* 9 is index. */
    lum_ver_delta_lut[10] = vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_10; /* 10 is index. */
    lum_ver_delta_lut[11] = vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_11; /* 11 is index. */
    lum_ver_delta_lut[12] = vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_12; /* 12 is index. */
    lum_ver_delta_lut[13] = vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_13; /* 13 is index. */
    lum_ver_delta_lut[14] = vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_14; /* 14 is index. */
    lum_ver_delta_lut[15] = vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_15; /* 15 is index. */
    lum_ver_delta_lut[16] = vpss_reg->vpss_hipp_snr_reg50.bits.lum_ver_delta_lut_16; /* 16 is index. */

    return HI_SUCCESS;
}

static hi_s32 pq_hal_db_get_filter_hor_info(hi_u32 handle_id, db_input_info *db_filter_info)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    db_filter_info->db_lumhor_en = vpss_reg->vpss_hipp_snr_reg0.bits.db_lumhor_en;

    db_filter_info->lum_hor_scale_ratio = vpss_reg->vpss_hipp_snr_reg44.bits.lum_hor_scale_ratio;
    db_filter_info->lum_hor_filter_sel = vpss_reg->vpss_hipp_snr_reg44.bits.lum_hor_filter_sel;
    db_filter_info->lum_hor_txt_winsize = vpss_reg->vpss_hipp_snr_reg45.bits.lum_hor_txt_winsize;
    db_filter_info->max_lum_hor_db_dist = vpss_reg->vpss_hipp_snr_reg52.bits.max_lum_hor_db_dist;

    db_filter_info->lum_hor_adj_gain = vpss_reg->vpss_hipp_snr_reg52.bits.lum_hor_adj_gain;
    db_filter_info->lum_hor_hf_var_core = vpss_reg->vpss_hipp_snr_reg50.bits.lum_hor_hf_var_core;
    db_filter_info->lum_hor_hf_var_gain1 = vpss_reg->vpss_hipp_snr_reg50.bits.lum_hor_hf_var_gain1;
    db_filter_info->lum_hor_hf_var_gain2 = vpss_reg->vpss_hipp_snr_reg51.bits.lum_hor_hf_var_gain2;
    db_filter_info->lum_hor_bord_adj_gain = vpss_reg->vpss_hipp_snr_reg51.bits.lum_hor_bord_adj_gain;
    db_filter_info->lum_hor_hf_diff_core = vpss_reg->vpss_hipp_snr_reg50.bits.lum_hor_hf_diff_core;
    db_filter_info->lum_hor_hf_diff_gain1 = vpss_reg->vpss_hipp_snr_reg50.bits.lum_hor_hf_diff_gain1;
    db_filter_info->lum_hor_hf_diff_gain2 = vpss_reg->vpss_hipp_snr_reg50.bits.lum_hor_hf_diff_gain2;

    return HI_SUCCESS;
}

static hi_s32 pq_hal_db_get_filter_ver_info(hi_u32 handle_id, db_input_info *db_filter_info)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    db_filter_info->db_lumver_en = vpss_reg->vpss_hipp_snr_reg0.bits.db_lumver_en;
    db_filter_info->lum_ver_scale_ratio = vpss_reg->vpss_hipp_snr_reg44.bits.lum_ver_scale_ratio;
    db_filter_info->lum_ver_txt_winsize = vpss_reg->vpss_hipp_snr_reg45.bits.lum_ver_txt_winsize;
    db_filter_info->lum_ver_adj_gain = vpss_reg->vpss_hipp_snr_reg52.bits.lum_ver_adj_gain;
    db_filter_info->lum_ver_hf_var_core = vpss_reg->vpss_hipp_snr_reg51.bits.lum_ver_hf_var_core;
    db_filter_info->lum_ver_hf_var_gain1 = vpss_reg->vpss_hipp_snr_reg51.bits.lum_ver_hf_var_gain1;
    db_filter_info->lum_ver_hf_var_gain2 = vpss_reg->vpss_hipp_snr_reg51.bits.lum_ver_hf_var_gain2;
    db_filter_info->lum_ver_bord_adj_gain = vpss_reg->vpss_hipp_snr_reg51.bits.lum_ver_bord_adj_gain;

    return HI_SUCCESS;
}

static hi_s32 pq_hal_db_get_filter_other_info(hi_u32 handle_id, db_input_info *db_filter_info)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    db_filter_info->input_port = HI_DRV_SOURCE_HDMI;
    db_filter_info->db_smooth_detnum_en = vpss_reg->vpss_hipp_snr_reg41.bits.db_smooth_detnum_en;
    db_filter_info->dir_smooth_mode = vpss_reg->vpss_hipp_snr_reg52.bits.dir_smooth_mode;

    db_filter_info->grad_sub_ratio = vpss_reg->vpss_hipp_snr_reg41.bits.grad_sub_ratio;
    db_filter_info->db_ctrst_thresh = vpss_reg->vpss_hipp_snr_reg41.bits.db_ctrst_thresh;
    db_filter_info->ctrst_adj_core = vpss_reg->vpss_hipp_snr_reg51.bits.ctrst_adj_core;
    db_filter_info->ctrst_adj_gain1 = vpss_reg->vpss_hipp_snr_reg52.bits.ctrst_adj_gain1;
    db_filter_info->ctrst_adj_gain2 = vpss_reg->vpss_hipp_snr_reg52.bits.ctrst_adj_gain2;

    db_filter_info->db_adptflt_mode = vpss_reg->vpss_hipp_snr_reg41.bits.db_adptflt_mode;
    db_filter_info->db_adptflt_mode_final_sel = vpss_reg->vpss_hipp_snr_reg57.bits.db_adptflt_mode_final_sel;

    db_filter_info->min_gm_thres = vpss_reg->vpss_hipp_snr_reg56.bits.min_globalmotion_thres;
    db_filter_info->min_bs_hy_thres1 = vpss_reg->vpss_hipp_snr_reg56.bits.min_bordstr_hy_thres1;
    db_filter_info->min_bs_hy_thres2 = vpss_reg->vpss_hipp_snr_reg56.bits.min_bordstr_hy_thres2;
    db_filter_info->max_bs_hy_thres = vpss_reg->vpss_hipp_snr_reg55.bits.max_bordstr_hy_thres;

    db_filter_info->global_static_protect_en = vpss_reg->vpss_hipp_snr_reg41.bits.globalstatic_protect_en;
    db_filter_info->glb_motion = vpss_reg->vpss_hipp_tnr_reg145.bits.global_motion;

    return HI_SUCCESS;
}

hi_s32 pq_hal_db_get_filter_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, db_input_info *db_filter_info)
{
    hi_s32 ret;
    vpss_reg_type *vpss_reg = HI_NULL;
    hi_u32 handle_id;

    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_stt_info);
    PQ_CHECK_NULL_PTR_RE_FAIL(db_filter_info);

    handle_id = vpss_stt_info->handle_id;
    vpss_reg = pq_hal_get_vpss_reg(handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    db_filter_info->bit_depth = DB_BIT_DEPTH;
    db_filter_info->y_width = vpss_stt_info->width;
    db_filter_info->y_height = vpss_stt_info->height;
    db_filter_info->db_en = vpss_reg->vpss_hipp_snr_reg0.bits.db_en;

    ret = pq_hal_db_get_filter_hor_info(handle_id, db_filter_info);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_db_get_filter_h_str_fade_lut(handle_id, &db_filter_info->lum_h_str_fade_lut[0]); /* 0:index */
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_db_get_filter_hor_delta_lut(handle_id, &db_filter_info->lum_hor_delta_lut[0]); /* 0:index */
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_db_get_filter_ver_info(handle_id, db_filter_info);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_db_get_filter_v_str_fade_lut(handle_id, &db_filter_info->lum_v_str_fade_lut[0]); /* 0:index */
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_db_get_filter_ver_delta_lut(handle_id, &db_filter_info->lum_ver_delta_lut[0]); /* 0:index */
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_db_get_filter_other_info(handle_id, db_filter_info);
    PQ_CHECK_RETURN_SUCCESS(ret);

    return HI_SUCCESS;
}

hi_s32 pq_hal_db_set_detect_api_reg(hi_drv_pq_vpss_stt_info *vpss_stt_info, dbd_output_info *detect_output_reg)
{
    vpss_reg_type *vpss_reg = HI_NULL;
    hi_u32 *reg_ptr = HI_NULL;
    hi_u32 i;

    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_stt_info);
    PQ_CHECK_NULL_PTR_RE_FAIL(detect_output_reg);

    vpss_reg = pq_hal_get_vpss_reg(vpss_stt_info->handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg306.bits.test_blk_number_hy = detect_output_reg->test_blknum_hy;
    vpss_reg->vpss_hipp_snr_reg306.bits.test_blk_number_vy = detect_output_reg->test_blknum_vy;

    if (vpss_stt_info->width <= PQ_WIDTH_1920) {
        reg_ptr = (hi_u32 *)&vpss_reg->vpss_hipp_snr_reg178.u32;
        for (i = 0; i < DB_BORDER_SPLIT; i++) {
            *reg_ptr = detect_output_reg->using_border_index_buf_hy[i];
            reg_ptr++;
        }

        reg_ptr = (hi_u32 *)&vpss_reg->vpss_hipp_snr_reg178.u32 + 90;        /* 90 is offset. */
        for (i = 90; i < 128; i++) {                                         /* buf_vy store from 90 to 128. */
            *reg_ptr = detect_output_reg->using_border_index_buf_vy[i - 90]; /* 90 is offset. */
            reg_ptr++;
        }
    } else {
        reg_ptr = (hi_u32 *)&vpss_reg->vpss_hipp_snr_reg178.u32;
        for (i = 0; i < 128; i++) { /* buf_hy store in 128 buf when width over 1920. */
            *reg_ptr = detect_output_reg->using_border_index_buf_hy[i];
            reg_ptr++;
        }
    }

    return HI_SUCCESS;
}

static hi_void pq_hal_db_set_filter_api_hor_fade_lut(vpss_reg_type *vpss_reg, hi_s32 *lum_h_str_fade_lut)
{
    vpss_reg->vpss_hipp_snr_reg52.bits.lum_h_str_fade_lut_0 = lum_h_str_fade_lut[0];   /* 0 is index. */
    vpss_reg->vpss_hipp_snr_reg52.bits.lum_h_str_fade_lut_1 = lum_h_str_fade_lut[1];   /* 1 is index. */
    vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_2 = lum_h_str_fade_lut[2];   /* 2 is index. */
    vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_3 = lum_h_str_fade_lut[3];   /* 3 is index. */
    vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_4 = lum_h_str_fade_lut[4];   /* 4 is index. */
    vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_5 = lum_h_str_fade_lut[5];   /* 5 is index. */
    vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_6 = lum_h_str_fade_lut[6];   /* 6 is index. */
    vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_7 = lum_h_str_fade_lut[7];   /* 7 is index. */
    vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_8 = lum_h_str_fade_lut[8];   /* 8 is index. */
    vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_9 = lum_h_str_fade_lut[9];   /* 9 is index. */
    vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_10 = lum_h_str_fade_lut[10]; /* 10isindex. */
    vpss_reg->vpss_hipp_snr_reg53.bits.lum_h_str_fade_lut_11 = lum_h_str_fade_lut[11]; /* 11isindex. */

    return;
}

static hi_void pq_hal_db_set_filter_api_hor_delta_lut(vpss_reg_type *vpss_reg, hi_s32 *lum_hor_delta_lut)
{
    vpss_reg->vpss_hipp_snr_reg45.bits.lum_hor_delta_lut_0 = lum_hor_delta_lut[0];   /* 0 is index. */
    vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_1 = lum_hor_delta_lut[1];   /* 1 is index. */
    vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_2 = lum_hor_delta_lut[2];   /* 2 is index. */
    vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_3 = lum_hor_delta_lut[3];   /* 3 is index. */
    vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_4 = lum_hor_delta_lut[4];   /* 4 is index. */
    vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_5 = lum_hor_delta_lut[5];   /* 5 is index. */
    vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_6 = lum_hor_delta_lut[6];   /* 6 is index. */
    vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_7 = lum_hor_delta_lut[7];   /* 7 is index. */
    vpss_reg->vpss_hipp_snr_reg46.bits.lum_hor_delta_lut_8 = lum_hor_delta_lut[8];   /* 8 is index. */
    vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_9 = lum_hor_delta_lut[9];   /* 9 is index. */
    vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_10 = lum_hor_delta_lut[10]; /* 10 is index. */
    vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_11 = lum_hor_delta_lut[11]; /* 11 is index. */
    vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_12 = lum_hor_delta_lut[12]; /* 12 is index. */
    vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_13 = lum_hor_delta_lut[13]; /* 13 is index. */
    vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_14 = lum_hor_delta_lut[14]; /* 14 is index. */
    vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_15 = lum_hor_delta_lut[15]; /* 15 is index. */
    vpss_reg->vpss_hipp_snr_reg47.bits.lum_hor_delta_lut_16 = lum_hor_delta_lut[16]; /* 16 is index. */

    return;
}

static hi_s32 pq_hal_db_set_filter_api_hor(hi_u32 handle_id, db_output_info *filter_output_reg)
{
    vpss_reg_type *vpss_reg = HI_NULL;
    vpss_reg = pq_hal_get_vpss_reg(handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg0.bits.db_lumhor_en = filter_output_reg->db_lumhor_en;

    pq_hal_db_set_filter_api_hor_fade_lut(vpss_reg, &filter_output_reg->lum_h_str_fade_lut[0]); /* 0 is index. */
    pq_hal_db_set_filter_api_hor_delta_lut(vpss_reg, &filter_output_reg->lum_hor_delta_lut[0]); /* 0 is index. */

    vpss_reg->vpss_hipp_snr_reg44.bits.lum_hor_scale_ratio = filter_output_reg->lum_hor_scale_ratio;
    vpss_reg->vpss_hipp_snr_reg44.bits.lum_hor_filter_sel = filter_output_reg->lum_hor_filter_sel;
    vpss_reg->vpss_hipp_snr_reg45.bits.lum_hor_txt_winsize = filter_output_reg->lum_hor_txt_winsize;
    vpss_reg->vpss_hipp_snr_reg52.bits.max_lum_hor_db_dist = filter_output_reg->max_lum_hor_db_dist;

    vpss_reg->vpss_hipp_snr_reg52.bits.lum_hor_adj_gain = filter_output_reg->lum_hor_adj_gain;
    vpss_reg->vpss_hipp_snr_reg50.bits.lum_hor_hf_var_core = filter_output_reg->lum_hor_hf_var_core;
    vpss_reg->vpss_hipp_snr_reg50.bits.lum_hor_hf_var_gain1 = filter_output_reg->lum_hor_hf_var_gain1;
    vpss_reg->vpss_hipp_snr_reg51.bits.lum_hor_hf_var_gain2 = filter_output_reg->lum_hor_hf_var_gain2;
    vpss_reg->vpss_hipp_snr_reg51.bits.lum_hor_bord_adj_gain = filter_output_reg->lum_hor_bord_adj_gain;
    vpss_reg->vpss_hipp_snr_reg50.bits.lum_hor_hf_diff_core = filter_output_reg->lum_hor_hf_diff_core;

    vpss_reg->vpss_hipp_snr_reg50.bits.lum_hor_hf_diff_gain1 = filter_output_reg->lum_hor_hf_diff_gain1;
    vpss_reg->vpss_hipp_snr_reg50.bits.lum_hor_hf_diff_gain2 = filter_output_reg->lum_hor_hf_diff_gain2;
    vpss_reg->vpss_hipp_snr_reg45.bits.lum_h_blk_size = filter_output_reg->blksize_lum_h;

    return HI_SUCCESS;
}

static hi_void pq_hal_db_set_filter_api_ver_fade_lut(vpss_reg_type *vpss_reg, hi_s32 *lum_v_str_fade_lut)
{
    vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_0 = lum_v_str_fade_lut[0];   /* 0 is index. */
    vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_1 = lum_v_str_fade_lut[1];   /* 1 is index. */
    vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_2 = lum_v_str_fade_lut[2];   /* 2 is index. */
    vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_3 = lum_v_str_fade_lut[3];   /* 3 is index. */
    vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_4 = lum_v_str_fade_lut[4];   /* 4 is index. */
    vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_5 = lum_v_str_fade_lut[5];   /* 5 is index. */
    vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_6 = lum_v_str_fade_lut[6];   /* 6 is index. */
    vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_7 = lum_v_str_fade_lut[7];   /* 7 is index. */
    vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_8 = lum_v_str_fade_lut[8];   /* 8 is index. */
    vpss_reg->vpss_hipp_snr_reg54.bits.lum_v_str_fade_lut_9 = lum_v_str_fade_lut[9];   /* 9 is index. */
    vpss_reg->vpss_hipp_snr_reg55.bits.lum_v_str_fade_lut_10 = lum_v_str_fade_lut[10]; /* 10isindex. */
    vpss_reg->vpss_hipp_snr_reg55.bits.lum_v_str_fade_lut_11 = lum_v_str_fade_lut[11]; /* 11isindex. */

    return;
}

static hi_void pq_hal_db_set_filter_api_ver_delta_lut(vpss_reg_type *vpss_reg, hi_s32 *lum_ver_delta_lut)
{
    vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_0 = lum_ver_delta_lut[0];   /* 0 is index. */
    vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_1 = lum_ver_delta_lut[1];   /* 1 is index. */
    vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_2 = lum_ver_delta_lut[2];   /* 2 is index. */
    vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_3 = lum_ver_delta_lut[3];   /* 3 is index. */
    vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_4 = lum_ver_delta_lut[4];   /* 4 is index. */
    vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_5 = lum_ver_delta_lut[5];   /* 5 is index. */
    vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_6 = lum_ver_delta_lut[6];   /* 6 is index. */
    vpss_reg->vpss_hipp_snr_reg48.bits.lum_ver_delta_lut_7 = lum_ver_delta_lut[7];   /* 7 is index. */
    vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_8 = lum_ver_delta_lut[8];   /* 8 is index. */
    vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_9 = lum_ver_delta_lut[9];   /* 9 is index. */
    vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_10 = lum_ver_delta_lut[10]; /* 10 is index. */
    vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_11 = lum_ver_delta_lut[11]; /* 11 is index. */
    vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_12 = lum_ver_delta_lut[12]; /* 12 is index. */
    vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_13 = lum_ver_delta_lut[13]; /* 13 is index. */
    vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_14 = lum_ver_delta_lut[14]; /* 14 is index. */
    vpss_reg->vpss_hipp_snr_reg49.bits.lum_ver_delta_lut_15 = lum_ver_delta_lut[15]; /* 15 is index. */
    vpss_reg->vpss_hipp_snr_reg50.bits.lum_ver_delta_lut_16 = lum_ver_delta_lut[16]; /* 16 is index. */

    return;
}

static hi_s32 pq_hal_db_set_filter_api_ver(hi_u32 handle_id, db_output_info *filter_output_reg)
{
    vpss_reg_type *vpss_reg = HI_NULL;
    vpss_reg = pq_hal_get_vpss_reg(handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg0.bits.db_lumver_en = filter_output_reg->db_lumver_en;

    pq_hal_db_set_filter_api_ver_fade_lut(vpss_reg, &filter_output_reg->lum_v_str_fade_lut[0]); /* 0 is index. */
    pq_hal_db_set_filter_api_ver_delta_lut(vpss_reg, &filter_output_reg->lum_ver_delta_lut[0]); /* 0 is index. */

    vpss_reg->vpss_hipp_snr_reg44.bits.lum_ver_scale_ratio = filter_output_reg->lum_ver_scale_ratio;
    vpss_reg->vpss_hipp_snr_reg45.bits.lum_ver_txt_winsize = filter_output_reg->lum_ver_txt_winsize;
    vpss_reg->vpss_hipp_snr_reg52.bits.lum_ver_adj_gain = filter_output_reg->lum_ver_adj_gain;
    vpss_reg->vpss_hipp_snr_reg51.bits.lum_ver_hf_var_core = filter_output_reg->lum_ver_hf_var_core;
    vpss_reg->vpss_hipp_snr_reg51.bits.lum_ver_hf_var_gain1 = filter_output_reg->lum_ver_hf_var_gain1;
    vpss_reg->vpss_hipp_snr_reg51.bits.lum_ver_hf_var_gain2 = filter_output_reg->lum_ver_hf_var_gain2;
    vpss_reg->vpss_hipp_snr_reg51.bits.lum_ver_bord_adj_gain = filter_output_reg->lum_ver_bord_adj_gain;
    vpss_reg->vpss_hipp_snr_reg45.bits.lum_v_blk_size = filter_output_reg->blksize_lum_v;

    return HI_SUCCESS;
}

hi_s32 pq_hal_db_set_filter_api_reg(hi_drv_pq_vpss_stt_info *vpss_stt_info, db_output_info *filter_output_reg)
{
    hi_s32 ret;
    vpss_reg_type *vpss_reg = HI_NULL;
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_stt_info);
    PQ_CHECK_NULL_PTR_RE_FAIL(filter_output_reg);

    vpss_reg = pq_hal_get_vpss_reg(vpss_stt_info->handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    ret = pq_hal_db_set_filter_api_hor(vpss_stt_info->handle_id, filter_output_reg);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_db_set_filter_api_ver(vpss_stt_info->handle_id, filter_output_reg);
    PQ_CHECK_RETURN_SUCCESS(ret);

    vpss_reg->vpss_hipp_snr_reg41.bits.grad_sub_ratio = filter_output_reg->grad_sub_ratio;
    vpss_reg->vpss_hipp_snr_reg41.bits.db_ctrst_thresh = filter_output_reg->db_ctrst_thresh;
    vpss_reg->vpss_hipp_snr_reg51.bits.ctrst_adj_core = filter_output_reg->ctrst_adj_core;
    vpss_reg->vpss_hipp_snr_reg52.bits.ctrst_adj_gain1 = filter_output_reg->ctrst_adj_gain1;
    vpss_reg->vpss_hipp_snr_reg52.bits.ctrst_adj_gain2 = filter_output_reg->ctrst_adj_gain2;
    vpss_reg->vpss_hipp_snr_reg52.bits.dir_smooth_mode = filter_output_reg->dir_smooth_mode;

    vpss_reg->vpss_hipp_snr_reg57.bits.db_adptflt_mode_final_sel = filter_output_reg->db_adptflt_mode_final_sel;

    return HI_SUCCESS;
}

hi_s32 pq_hal_db_set_demo_enable(hi_u32 handle_id, hi_bool on_off)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg1.bits.demo_en = on_off;

    return HI_SUCCESS;
}

hi_s32 pq_hal_db_set_demo_mode(hi_u32 handle_id, pq_demo_mode demo_mode)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg1.bits.demo_mode = demo_mode;

    return HI_SUCCESS;
}

hi_s32 pq_hal_db_set_demo_pos(hi_u32 handle_id, hi_u32 x_pos)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg1.bits.demo_wx = x_pos;

    return HI_SUCCESS;
}

hi_s32 pq_hal_db_set_strength(hi_u32 handle_id, hi_u32 strength)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg45.bits.global_db_str_lum = strength;

    return HI_SUCCESS;
}

hi_s32 pq_hal_db_set_en(hi_u32 handle_id, hi_bool enable)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg0.bits.db_en = enable;

    return HI_SUCCESS;
}

hi_s32 pq_hal_db_set_boarder_flag(hi_drv_pq_vpss_stt_info *vpss_stt_info)
{
    vpss_reg_type *vpss_reg = HI_NULL;
    hi_u32 *reg_ptr = HI_NULL;
    hi_u32 i;

    vpss_reg = pq_hal_get_vpss_reg(vpss_stt_info->handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    reg_ptr = (hi_u32 *)&vpss_reg->vpss_hipp_snr_reg178.u32;

    for (i = 0; i < 128; i++) { /* 128 is flag array. */
        *reg_ptr = 0x80808080;  /* 0x80808080 set boarder flag. */
        reg_ptr++;
    }

    return HI_SUCCESS;
}
