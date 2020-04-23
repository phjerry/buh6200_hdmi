/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq hal dei api
 * Author: pq
 * Create: 2016-01-1
 */

#include "hi_type.h"
#include "hi_osal.h"
#include "pq_hal_dei.h"
#include "drv_pq.h"

/* NOTE: VPSS_REG_OFF%4==0 NodeAddr need 16 align */
#define PQ_SCAN_REG_OFF     ((0x2000 + 15) / 16 * 4)
#define PQ_SCAN_REG_OFF_USE (PQ_SCAN_REG_OFF * 4)

static hi_u32 g_sg_u32_sigma = 0;
static hi_u32 g_sg_u32_dei_global_motion = 0;
static hi_s32 g_sg_s32_h_move_scene_wt = 0;
static hi_s32 g_sg_s32_v_move_scene_wt = 0;

hi_u32 g_dei_mean_value_array[VPSS_HANDLE_NUM] = {0};

hi_s32 pq_hal_enable_dei_demo(hi_u32 handle_no, hi_bool on_off)
{
    vpss_reg_type *vpss_vir_reg = HI_NULL;

    vpss_vir_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_vir_reg);

    vpss_vir_reg->vpss_hipp_dei_demo.bits.dei_demo_en = on_off;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_dei_demo_mode(hi_u32 handle_no, pq_dei_demo_mode mode)
{
    vpss_reg_type *vpss_vir_reg = HI_NULL;

    vpss_vir_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_vir_reg);

    if (mode == DEI_DEMO_ENABLE_L) { /* enable_l: L-mc_enable; R-mc_disable */
        vpss_vir_reg->vpss_hipp_dei_demo.bits.dei_demo_mode_l = PQ_MCDI_OUTPUT;
        vpss_vir_reg->vpss_hipp_dei_demo.bits.dei_demo_mode_r = PQ_MCDI_DISABLE;
    } else { /* enable_r: R-mc_enable; L-mc_disable */
        vpss_vir_reg->vpss_hipp_dei_demo.bits.dei_demo_mode_l = PQ_MCDI_DISABLE;
        vpss_vir_reg->vpss_hipp_dei_demo.bits.dei_demo_mode_r = PQ_MCDI_OUTPUT;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_dei_demo_mode_coor(hi_u32 handle_no, hi_u32 x_pos)
{
    vpss_reg_type *vpss_vir_reg = HI_NULL;

    vpss_vir_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_vir_reg);

    vpss_vir_reg->vpss_hipp_dei_demo.bits.dei_demo_border = x_pos;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_mc_only_enable(hi_u32 handle_no, hi_bool on_off)
{
    vpss_reg_type *vpss_vir_reg = HI_NULL;

    vpss_vir_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_vir_reg);

    vpss_vir_reg->vpss_hipp_mc0.bits.mc_only = on_off;

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_ma_only_enable(hi_u32 handle_no, hi_bool *on_off)
{
    vpss_reg_type *vpss_vir_reg = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);
    vpss_vir_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_vir_reg);

    *on_off = vpss_vir_reg->vpss_hipp_mc0.bits.ma_only;

    return HI_SUCCESS;
}
/*****************************************************************************/
hi_s32 pq_hal_set_dei_vir_reg(hi_void)
{
    hi_drv_pq_dbg_vpss_vir_soft *vir_soft_reg = HI_NULL;
    vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(vir_soft_reg);

    vir_soft_reg->dei_reg.k_y_mcw = 16;                /* 16 : num */
    vir_soft_reg->dei_reg.k_c_mcw = 16;                /* 16 : num */
    vir_soft_reg->dei_reg.k_y_mcbld = 16;              /* 16 : num */
    vir_soft_reg->dei_reg.k_c_mcbld = 16;              /* 16 : num */
    vir_soft_reg->dei_reg.fld_cnt_adjust_en = 1;       /* 1 : num */
    vir_soft_reg->dei_reg.fld_cnt_x = 6;               /* 6 : num */
    vir_soft_reg->dei_reg.fld_cnt_k = 127;             /* 127: num */
    vir_soft_reg->dei_reg.gs_graph_x0 = 64;            /* 64 : num */
    vir_soft_reg->dei_reg.gs_graph_x1 = 128;           /* 128 : num */
    vir_soft_reg->dei_reg.shift_gs_graph = 6;          /* 6: num */
    vir_soft_reg->dei_reg.even_odd_diff_cnt_core = 10; /* 10 : num */
    vir_soft_reg->dei_reg.art_scene_cnt_core = 30;     /* 30 : num */
    vir_soft_reg->dei_reg.still_scene_cnt_core = 30;   /* 30 : num */
    vir_soft_reg->dei_reg.still_bg_scene_cnt_core = 0;
    vir_soft_reg->dei_reg.fea_scene_cnt_core = 30; /* 30 : num */

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_dei_me_param(hi_drv_pq_vpss_stt_info *info_in, drv_pq_me_in_parm *me_param)
{
    vpss_scan_stt_reg_type *scan_stt_info = (vpss_scan_stt_reg_type *)info_in->scan_stt_reg;
    vpss_reg_type *vpss_reg = pq_hal_get_vpss_reg(info_in->handle_id);

    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    me_param->rgt_bndy = vpss_reg->vpss_hipp_me_bndy.bits.rgt_bndy;
    me_param->lft_bndy = vpss_reg->vpss_hipp_me_bndy.bits.lft_bndy;
    me_param->top_bndy = vpss_reg->vpss_hipp_me_bndy.bits.top_bndy;
    me_param->bot_bndy = vpss_reg->vpss_hipp_me_bndy.bits.bot_bndy;

    me_param->glbmv[0].num = scan_stt_info->vpss_me_glbmv_hist_0.bits.me_glbmv0_num;
    me_param->glbmv[1].num = scan_stt_info->vpss_me_glbmv_hist_1.bits.me_glbmv1_num;

    me_param->glbmv[0].good_mv_cnt = scan_stt_info->vpss_me_glbmv_hist_2.bits.me_glbmv_goodmv_cnt;
    me_param->glbmv[1].good_mv_cnt = scan_stt_info->vpss_me_glbmv_hist_2.bits.me_glbmv_goodmv_cnt;

    /* signed  number should suit size */
    me_param->glbmv[0].mv.x = s8_to_s32(scan_stt_info->vpss_me_glbmv_hist_0.bits.me_glbmv0_mvx);
    me_param->glbmv[0].mv.y = s7_to_s32(scan_stt_info->vpss_me_glbmv_hist_0.bits.me_glbmv0_mvy);

    me_param->glbmv[1].mv.x = s8_to_s32(scan_stt_info->vpss_me_glbmv_hist_1.bits.me_glbmv1_mvx);
    me_param->glbmv[1].mv.y = s7_to_s32(scan_stt_info->vpss_me_glbmv_hist_1.bits.me_glbmv1_mvy);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_dei_up_param(hi_drv_pq_dei_drv_uparm *dei_up)
{
    hi_drv_pq_dbg_vpss_vir_soft *vir_reg = pq_hal_get_vpss_vir_soft_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(vir_reg);

    dei_up->art_scene_cnt_core = vir_reg->dei_reg.art_scene_cnt_core;
    dei_up->k_y_mcw = vir_reg->dei_reg.k_y_mcw;
    dei_up->k_c_mcw = vir_reg->dei_reg.k_c_mcw;
    dei_up->k_y_mcbld = vir_reg->dei_reg.k_y_mcbld;
    dei_up->k_c_mcbld = vir_reg->dei_reg.k_c_mcbld;
    dei_up->gs_graph_x0 = vir_reg->dei_reg.gs_graph_x0;
    dei_up->gs_graph_x1 = vir_reg->dei_reg.gs_graph_x1;
    dei_up->shift_gs_graph = vir_reg->dei_reg.shift_gs_graph;
    dei_up->even_odd_diff_cnt_core = vir_reg->dei_reg.even_odd_diff_cnt_core;
    dei_up->still_scene_cnt_core = vir_reg->dei_reg.still_scene_cnt_core;
    dei_up->still_bg_scene_cnt_core = vir_reg->dei_reg.still_bg_scene_cnt_core;
    dei_up->fea_scene_cnt_core = vir_reg->dei_reg.fea_scene_cnt_core;
    dei_up->fld_cnt_adjust_en = vir_reg->dei_reg.fld_cnt_adjust_en;
    dei_up->fld_cnt_x = vir_reg->dei_reg.fld_cnt_x;
    dei_up->fld_cnt_k = vir_reg->dei_reg.fld_cnt_k;
    return HI_SUCCESS;
}

hi_void pq_hal_set_sigma(hi_u32 sigma)
{
    g_sg_u32_sigma = sigma;
}

hi_s32 pq_hal_update_dei_global_motion(hi_drv_pq_vpss_stt_info *info_in, drv_pq_dei_input_info *dei_api_input)
{
    hi_s32 i = 0;
    hi_u64 global_motion_num_sum = 0;
    hi_u32 total_pxl_num;
    hi_s32 mt_state[5] = {0}; /* 5 : num */
    vpss_stt_reg_type *stt_info = (vpss_stt_reg_type *)info_in->vpss_stt_reg;

    PQ_CHECK_NULL_PTR_RE_FAIL(info_in);

    total_pxl_num = (info_in->width * info_in->height) / 2; /* 2 : num */

    mt_state[0] = stt_info->vpss_wstt_mt_stat0.bits.mt_stat0;
    mt_state[1] = stt_info->vpss_wstt_mt_stat1.bits.mt_stat1;
    mt_state[2] = stt_info->vpss_wstt_mt_stat2.bits.mt_stat2;                            /* 2 : num */
    mt_state[3] = stt_info->vpss_wstt_mt_stat3.bits.mt_stat3;                            /* 3 : num */
    mt_state[4] = total_pxl_num - mt_state[0] - mt_state[1] - mt_state[2] - mt_state[3]; /* 2/3/4 : num */

    for (i = 1; i < 5; i++) { /* 5 : num */
        global_motion_num_sum += mt_state[i];
    }

    if (global_motion_num_sum == 0) {
        global_motion_num_sum = 1;
    }

    total_pxl_num = total_pxl_num / 4095 == 0 ? 1 : total_pxl_num / 4095; /* 4095 : num */

    g_sg_u32_dei_global_motion = osal_div64_u64(global_motion_num_sum, total_pxl_num);

    dei_api_input->frm_global_motion = g_sg_u32_dei_global_motion;

    return HI_SUCCESS;
}

hi_void pq_hal_get_dei_di_param(hi_drv_pq_vpss_stt_info *info_in, drv_pq_di_in_parm *di_param)
{
    vpss_stt_reg_type *stt_info = (vpss_stt_reg_type *)info_in->vpss_stt_reg;
    vpss_reg_type *vpss_reg = pq_hal_get_vpss_reg(info_in->handle_id);

    di_param->max_err_line = stt_info->vpss_wstt_hipp_di_film4.bits.max_err_line;
    di_param->total_err_line = stt_info->vpss_wstt_hipp_di_film5.bits.total_err_line;
    di_param->blk_fea_cnt_frame = stt_info->vpss_wstt_hipp_di_film2.bits.blk_fea_cnt_frame;
    di_param->pxl_fea_cnt_frame = stt_info->vpss_wstt_hipp_di_film3.bits.pxl_fea_cnt_frame;

    di_param->film_video_line_num = stt_info->vpss_wstt_hipp_di_film1.bits.film_video_line_num;
    di_param->film_video_max_num = stt_info->vpss_wstt_hipp_di_film0.bits.film_video_max_num;
    di_param->not_found_fea_cnt = stt_info->vpss_wstt_hipp_di_slight2.bits.not_found_fea_cnt;
    ;
    di_param->still_cnt = stt_info->vpss_wstt_hipp_di_slight0.bits.still_cnt;
    di_param->still_fea_cnt = stt_info->vpss_wstt_hipp_di_slight1.bits.still_fea_cnt;

    di_param->jitter_big_mv_num[0] = stt_info->vpss_wstt_hipp_di_jitter2.bits.jitter_big_mv_num0;
    di_param->jitter_big_mv_num[1] = stt_info->vpss_wstt_hipp_di_jitter2.bits.jitter_big_mv_num1;
    di_param->jitter_big_mv_num[2] = stt_info->vpss_wstt_hipp_di_jitter3.bits.jitter_big_mv_num2; /* 2 : num */
    di_param->jitter_big_mv_num[3] = stt_info->vpss_wstt_hipp_di_jitter3.bits.jitter_big_mv_num3; /* 3 : num */

    di_param->jitter_small_mv_num[0] = stt_info->vpss_wstt_hipp_di_jitter0.bits.jitter_small_mv_num0;
    di_param->jitter_small_mv_num[1] = stt_info->vpss_wstt_hipp_di_jitter0.bits.jitter_small_mv_num1;
    di_param->jitter_small_mv_num[2] = stt_info->vpss_wstt_hipp_di_jitter1.bits.jitter_small_mv_num2; /* 2 : num */
    di_param->jitter_small_mv_num[3] = stt_info->vpss_wstt_hipp_di_jitter1.bits.jitter_small_mv_num3; /* 3 : num */

    di_param->jitter_area_mc_fea_cnt = stt_info->vpss_wstt_hipp_di_fusion0.bits.jitter_area_mc_fea_cnt;
    di_param->non_jitter_area_mc_fea_cnt = stt_info->vpss_wstt_hipp_di_fusion1.bits.non_jitter_area_mc_fea_cnt;

    di_param->sigma_cnt = g_sg_u32_sigma;

    di_param->scd_value = vpss_reg->vpss_hipp_tnr_reg36.bits.scene_change_info;

    memcpy(di_param->hist_bin, &(stt_info->vpss_wstt_combhistobin6523_00.u32), sizeof(hi_s32) * 64); /* 64 : num */

    return;
}

static hi_s32 pq_hal_set_dei_other_vir_out(hi_drv_pq_dei_api_output_reg *dei_api_out)
{
    hi_u32 i = 0;
    hi_drv_pq_dbg_vpss_vir_soft *vir_reg = pq_hal_get_vpss_vir_soft_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(vir_reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(dei_api_out);

    vir_reg->dei_reg.v_pan_scan_cnt = dei_api_out->dei_drv_param.v_pan_scan_cnt;
    vir_reg->dei_reg.v_pan_scan_wt = dei_api_out->dei_drv_param.v_pan_scan_wt;
    vir_reg->dei_reg.v_cont_not_pan_cnt = dei_api_out->dei_drv_param.v_cont_not_pan_cnt;
    vir_reg->dei_reg.h_pan_scan_cnt = dei_api_out->dei_drv_param.h_pan_scan_cnt;
    vir_reg->dei_reg.h_pan_scan_wt = dei_api_out->dei_drv_param.h_pan_scan_wt;
    vir_reg->dei_reg.h_cont_not_pan_cnt = dei_api_out->dei_drv_param.h_cont_not_pan_cnt;
    vir_reg->dei_reg.fast_move_cnt = dei_api_out->dei_drv_param.fast_move_cnt;
    vir_reg->dei_reg.fast_move_wt = dei_api_out->dei_drv_param.fast_move_wt;
    vir_reg->dei_reg.cont_not_fast_move_cnt = dei_api_out->dei_drv_param.cont_not_fast_move_cnt;
    vir_reg->dei_reg.h_move_scene_wt = dei_api_out->dei_drv_param.h_move_scene_wt;
    vir_reg->dei_reg.v_move_scene_wt = dei_api_out->dei_drv_param.v_move_scene_wt;
    vir_reg->dei_reg.still_bg_scene_cnt = dei_api_out->dei_drv_param.still_bg_scene_cnt;
    vir_reg->dei_reg.still_bg_scene_wt = dei_api_out->dei_drv_param.still_bg_scene_wt;
    vir_reg->dei_reg.cont_not_still_bg_scene_cnt = dei_api_out->dei_drv_param.cont_not_still_bg_scene_cnt;
    vir_reg->dei_reg.g_score = dei_api_out->dei_drv_param.graph_score;

    for (i = 0; i < 5; i++) { /* 5 : num */
        vir_reg->dei_reg.max_err_line[i] = dei_api_out->dei_drv_param.max_err_line[i];
        vir_reg->dei_reg.total_err_line[i] = dei_api_out->dei_drv_param.total_err_line[i];
        vir_reg->dei_reg.blk_fea_cnt_frame[i] = dei_api_out->dei_drv_param.blk_fea_cnt_frame[i];
        vir_reg->dei_reg.pxl_fea_cnt_frame[i] = dei_api_out->dei_drv_param.pxl_fea_cnt_frame[i];
    }
    vir_reg->dei_reg.fld_cnt = dei_api_out->dei_drv_param.fld_cnt;

    g_sg_s32_h_move_scene_wt = dei_api_out->dei_drv_param.h_move_scene_wt;
    g_sg_s32_v_move_scene_wt = dei_api_out->dei_drv_param.v_move_scene_wt;

    return HI_SUCCESS;
}

static hi_s32 pq_hal_set_dei_vir_out(hi_drv_pq_dei_api_output_reg *dei_api_out)
{
    hi_s32 ret;
    hi_drv_pq_dbg_vpss_vir_soft *vir_reg = pq_hal_get_vpss_vir_soft_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(vir_reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(dei_api_out);

    vir_reg->dei_reg.add_32_blk_cnt = dei_api_out->dei_drv_param.add_32_blk_cnt;
    vir_reg->dei_reg.film_like_32_cnt = dei_api_out->dei_drv_param.film_like_32_cnt;
    vir_reg->dei_reg.film_like_22_cnt = dei_api_out->dei_drv_param.film_like_22_cnt;
    vir_reg->dei_reg.film_like_cnt = dei_api_out->dei_drv_param.film_like_cnt;
    vir_reg->dei_reg.add_32_pxl_cnt = dei_api_out->dei_drv_param.add_32_pxl_cnt;
    vir_reg->dei_reg.film_like_22_main_cnt = dei_api_out->dei_drv_param.film_like_22_main_cnt;
    vir_reg->dei_reg.film_like_22_ass_cnt = dei_api_out->dei_drv_param.film_like_22_ass_cnt;
    vir_reg->dei_reg.not_found_scene_cnt = dei_api_out->dei_drv_param.not_found_scene_cnt;
    vir_reg->dei_reg.not_found_scene_wt = dei_api_out->dei_drv_param.not_found_scene_wt;
    vir_reg->dei_reg.tb_luma_diff_cnt = dei_api_out->dei_drv_param.tb_luma_diff_cnt;
    vir_reg->dei_reg.tb_luma_diff_wt = dei_api_out->dei_drv_param.tb_luma_diff_wt;
    vir_reg->dei_reg.still_scene_cnt = dei_api_out->dei_drv_param.still_scene_cnt;
    vir_reg->dei_reg.still_scene_wt = dei_api_out->dei_drv_param.still_scene_wt;
    vir_reg->dei_reg.cont_not_still_scene_cnt = dei_api_out->dei_drv_param.cont_not_still_scene_cnt;
    vir_reg->dei_reg.art_scene_cnt = dei_api_out->dei_drv_param.art_scene_cnt;
    vir_reg->dei_reg.art_scene_wt = dei_api_out->dei_drv_param.art_scene_wt;
    vir_reg->dei_reg.cont_not_art_scene_cnt = dei_api_out->dei_drv_param.cont_not_art_scene_cnt;
    vir_reg->dei_reg.jitter_scene_cnt = dei_api_out->dei_drv_param.jitter_scene_cnt;
    vir_reg->dei_reg.jitter_scene_wt = dei_api_out->dei_drv_param.jitter_scene_wt;
    vir_reg->dei_reg.cont_not_jitter_scene_cnt = dei_api_out->dei_drv_param.cont_not_jitter_scene_cnt;
    vir_reg->dei_reg.zoom_scene_cnt = dei_api_out->dei_drv_param.zoom_scene_cnt;
    vir_reg->dei_reg.zoom_scene_wt = dei_api_out->dei_drv_param.zoom_scene_wt;
    vir_reg->dei_reg.cont_not_zoom_scene_cnt = dei_api_out->dei_drv_param.cont_not_zoom_scene_cnt;
    vir_reg->dei_reg.jitter_fea_scene_cnt = dei_api_out->dei_drv_param.jitter_fea_scene_cnt;
    vir_reg->dei_reg.jitter_fea_scene_wt = dei_api_out->dei_drv_param.jitter_fea_scene_wt;
    vir_reg->dei_reg.non_jitter_fea_scene_cnt = dei_api_out->dei_drv_param.non_jitter_fea_scene_cnt;
    vir_reg->dei_reg.non_jitter_fea_scene_wt = dei_api_out->dei_drv_param.non_jitter_fea_scene_wt;

    ret = pq_hal_set_dei_other_vir_out(dei_api_out);

    return ret;
}

hi_s32 pq_hal_update_dei_hd_cfg(hi_u32 handle_no, hi_drv_pq_dei_api_output_reg *dei_api_out)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    if (dei_api_out->dei_drv_param.frm_width < PQ_WIDTH_1280 &&
        dei_api_out->dei_drv_param.frm_height < PQ_HEIGHT_720) {
        vpss_reg->vpss_hipp_mc_cvbs07.bits.check_sub_title = dei_api_out->di.check_sub_title;

        vpss_reg->vpss_hipp_mc_cvbs05.bits.fea_bot_bndy = dei_api_out->di.fea_bot_bndy;
        vpss_reg->vpss_hipp_mc_cvbs06.bits.fea_lft_bndy = dei_api_out->di.fea_lft_bndy;
        vpss_reg->vpss_hipp_mc_cvbs06.bits.fea_rgt_bndy = dei_api_out->di.fea_rgt_bndy;
        vpss_reg->vpss_hipp_mc_cvbs05.bits.fea_top_bndy = dei_api_out->di.fea_top_bndy;

        vpss_reg->vpss_hipp_mc_film0.bits.film_like_line_mode = dei_api_out->di.film_like_line_mode;
        vpss_reg->vpss_hipp_mc_film0.bits.film_like_weight = dei_api_out->di.film_like_weight;

        vpss_reg->vpss_hipp_mc_slight2.bits.gain_bld_zmvsad = dei_api_out->di.gain_bld_zmv_sad;
        vpss_reg->vpss_hipp_mc_slight4.bits.non_jitter_fea_core_wt = dei_api_out->di.jitter_fea_core_wt;
        vpss_reg->vpss_hipp_mc_slight2.bits.ma_merge_en = dei_api_out->di.ma_merge_en;
        vpss_reg->vpss_hipp_mc0.bits.ma_only = dei_api_out->di.ma_only;
        vpss_reg->vpss_hipp_mc_slight0.bits.mc_fea_max_wt = dei_api_out->di.mc_fea_max_wt;
        vpss_reg->vpss_hipp_mc_slight0.bits.mc_fea_min_wt = dei_api_out->di.mc_fea_min_wt;

        vpss_reg->vpss_hipp_mc_cvbs08.bits.mc_mid_dlt_en = dei_api_out->di.mc_mid_dlt_en;

        vpss_reg->vpss_hipp_mc_cvbs04.bits.mc_not_confirm_wt = dei_api_out->di.mc_not_confirm_wt;
        vpss_reg->vpss_hipp_mc_cvbs02.bits.mt_adj_fea_en = dei_api_out->di.mt_adj_fea_en;
        vpss_reg->vpss_hipp_mc_slight4.bits.non_jitter_fea_core_wt = dei_api_out->di.non_jitter_fea_core_wt;
        vpss_reg->vpss_hipp_mc_slight2.bits.slight_fea_max_wt = dei_api_out->di.slight_fea_max_wt;
        vpss_reg->vpss_hipp_mc_cvbs05.bits.slight_fea_mv_core_th = dei_api_out->di.slight_fea_mv_core_th;

        vpss_reg->vpss_hipp_mc_cvbs04.bits.slight_fea_mv_med_en = dei_api_out->di.slight_fea_mv_med_en;

        /* solve cornfield */
        vpss_reg->vpss_hipp_ma_mt14.bits.set_pre_info_mode = dei_api_out->di.set_pre_info_mode;
        vpss_reg->vpss_hipp_ma_mt5.bits.max_motion_uv_en = dei_api_out->di.max_motion_uv_en;
        vpss_reg->vpss_hipp_ma_mt5.bits.max_motion_y_en = dei_api_out->di.max_motion_y_en;

        /* solve scdchange */
        vpss_reg->vpss_hipp_ma_mt14.bits.his_motion_en = dei_api_out->di.his_motion_en;
        vpss_reg->vpss_hipp_ma_mt14.bits.rec_mode_en = dei_api_out->di.rec_mode_en;

        /* solve hmoveword */
        vpss_reg->vpss_hipp_ma_lai5.bits.edge_str_limit_scale = dei_api_out->di.edge_str_limit_scale;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_update_dei_cfg(hi_u32 handle_no, hi_drv_pq_dei_api_output_reg *dei_api_out)
{
    hi_s32 ret;
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_mc37.bits.k_c_mcbld = dei_api_out->di.k_c_mcbld;
    vpss_reg->vpss_hipp_mc37.bits.k_c_mcw = dei_api_out->di.k_c_mcw;
    vpss_reg->vpss_hipp_mc36.bits.k_y_mcbld = dei_api_out->di.k_y_mcbld;
    vpss_reg->vpss_hipp_mc36.bits.k_y_mcw = dei_api_out->di.k_y_mcw;
    vpss_reg->vpss_hipp_mc_slight0.bits.mc_fea_en = dei_api_out->di.mc_fea_en;
    vpss_reg->vpss_hipp_mc25.bits.g0_mag_wnd_mcw = dei_api_out->di.mag_wnd_mcw_g0;
    vpss_reg->vpss_hipp_mc25.bits.k0_mag_wnd_mcw = dei_api_out->di.mag_wnd_mcw_k0;
    vpss_reg->vpss_hipp_mc25.bits.k1_mag_wnd_mcw = dei_api_out->di.mag_wnd_mcw_k1;
    vpss_reg->vpss_hipp_mc25.bits.x0_mag_wnd_mcw = dei_api_out->di.mag_wnd_mcw_x0;
    vpss_reg->vpss_hipp_mc24.bits.g0_smag_wnd_mcw = dei_api_out->di.smag_wnd_mcw_g0;
    vpss_reg->vpss_hipp_mc24.bits.k0_smag_wnd_mcw = dei_api_out->di.smag_wnd_mcw_k0;
    vpss_reg->vpss_hipp_mc24.bits.x0_smag_wnd_mcw = dei_api_out->di.smag_wnd_mcw_x0;
    vpss_reg->vpss_hipp_mc24.bits.k1_smag_wnd_mcw = dei_api_out->di.smag_wnd_mcw_k1;
    vpss_reg->vpss_hipp_mc26.bits.g0_sad_wnd_mcw = dei_api_out->di.sad_wnd_mcw_g0;
    vpss_reg->vpss_hipp_mc26.bits.k0_sad_wnd_mcw = dei_api_out->di.sad_wnd_mcw_k0;
    vpss_reg->vpss_hipp_mc26.bits.k1_sad_wnd_mcw = dei_api_out->di.sad_wnd_mcw_k1;
    vpss_reg->vpss_hipp_mc26.bits.x0_sad_wnd_mcw = dei_api_out->di.sad_wnd_mcw_x0;
    vpss_reg->vpss_hipp_mc_slight3.bits.mc_fea_non_jitter_core_th = dei_api_out->di.mc_fea_non_jitter_core_th;
    vpss_reg->vpss_hipp_mc_slight3.bits.mc_fea_jitter_core_th = dei_api_out->di.mc_fea_jitter_core_th;
    vpss_reg->vpss_hipp_mc_slight1.bits.slight_fea_vdiff_core_th = dei_api_out->di.slight_fea_vdiff_core_th;
    vpss_reg->vpss_hipp_mc_slight1.bits.slight_fea_hdiff_core_th = dei_api_out->di.slight_fea_hdiff_core_th;
    vpss_reg->vpss_hipp_mc_slight1.bits.slight_fea_core_th = dei_api_out->di.slight_fea_core_th;
    vpss_reg->vpss_hipp_mc_slight2.bits.slight_fea_mc_gain = dei_api_out->di.slight_fea_mc_gain;
    vpss_reg->vpss_hipp_mc_cvbs02.bits.mt_adj_fea_zmvsad_add = dei_api_out->di.mt_adj_fea_zmvsad_add;
    vpss_reg->vpss_hipp_ma_mt14.bits.rec_mode_fld_motion_step_1 = dei_api_out->di.rec_mode_fld_mt_step1;
    vpss_reg->vpss_hipp_ma_mt9.bits.fld_motion_gain = dei_api_out->di.fld_motion_gain;

    ret = pq_hal_update_dei_hd_cfg(handle_no, dei_api_out);
    PQ_CHECK_RETURN_SUCCESS(ret);
    ret = pq_hal_set_dei_vir_out(dei_api_out);

    return ret;
}

hi_s32 pq_hal_update_me_bndy(hi_drv_pq_vpss_stt_info *info_in)
{
    vpss_reg_type *vpss_reg = HI_NULL;
    vpss_stt_reg_type *stt_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(info_in->handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    stt_reg = (vpss_stt_reg_type *)info_in->vpss_stt_reg;

    if (vpss_reg->vpss_ctrl.bits.lbd_en == HI_TRUE) {
        vpss_reg->vpss_hipp_me_bndy.bits.top_bndy =
            stt_reg->vpss_wstt_lbd_stt_space_0.bits.space_top > 24 ? 24 : /* 24: num */
            stt_reg->vpss_wstt_lbd_stt_space_0.bits.space_top;
        vpss_reg->vpss_hipp_me_bndy.bits.bot_bndy =
            stt_reg->vpss_wstt_lbd_stt_space_0.bits.space_bot > 24 ? 24 : /* 24: num */
            stt_reg->vpss_wstt_lbd_stt_space_0.bits.space_bot;

        vpss_reg->vpss_hipp_me_bndy.bits.lft_bndy =
            stt_reg->vpss_wstt_lbd_stt_space_1.bits.space_left > 24 ? 24 : /* 24: num */
            stt_reg->vpss_wstt_lbd_stt_space_1.bits.space_left;
        vpss_reg->vpss_hipp_me_bndy.bits.rgt_bndy =
            stt_reg->vpss_wstt_lbd_stt_space_1.bits.space_right > 24 ? 24 : /* 24: num */
            stt_reg->vpss_wstt_lbd_stt_space_1.bits.space_right;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_move_scene_wt(hi_s32 *ps32_h_move_scene_wt, hi_s32 *ps32_v_move_scene_wt)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(ps32_h_move_scene_wt);
    PQ_CHECK_NULL_PTR_RE_FAIL(ps32_v_move_scene_wt);

    *ps32_h_move_scene_wt = g_sg_s32_h_move_scene_wt;
    *ps32_v_move_scene_wt = g_sg_s32_v_move_scene_wt;

    return HI_SUCCESS;
}

hi_s32 pq_hal_update_field_flag(hi_u32 handle_no, hi_u32 frm_num)
{
    vpss_reg_type *vpss_reg = HI_NULL;
    hi_bool flag = (frm_num % 2 == 0) ? 1 : 0; /* 2: num */

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_me_updt_5.bits.field_flag = flag;
    *(hi_u32 *)((hi_u8 *)&vpss_reg->vpss_hipp_me_updt_5.u32 + 1 * PQ_DEI_ME_FLAG_OFFSET) =
    vpss_reg->vpss_hipp_me_updt_5.u32;
    *(hi_u32 *)((hi_u8 *)&vpss_reg->vpss_hipp_me_updt_5.u32 + 2 * PQ_DEI_ME_FLAG_OFFSET) = /* 2:num */
    vpss_reg->vpss_hipp_me_updt_5.u32;
    *(hi_u32 *)((hi_u8 *)&vpss_reg->vpss_hipp_me_updt_5.u32 + 3 * PQ_DEI_ME_FLAG_OFFSET) = /* 3:num */
    vpss_reg->vpss_hipp_me_updt_5.u32;
    *(hi_u32 *)((hi_u8 *)&vpss_reg->vpss_hipp_me_updt_5.u32 + 4 * PQ_DEI_ME_FLAG_OFFSET) = /* 4:num */
    vpss_reg->vpss_hipp_me_updt_5.u32;

    return HI_SUCCESS;
}

hi_s32 pq_hal_update_dei_rc_info(hi_u32 handle_no, hi_u32 width, hi_u32 height)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_mc30.bits.r1_mc = height / 2 - 1 - 2; /* 2: num */
    vpss_reg->vpss_hipp_mc30.bits.c1_mc = width - 1 - 4; /* 4: num */
    vpss_reg->vpss_hipp_mc46.bits.r1_0mvsad = height / 2 - 1; /* 2: num */
    vpss_reg->vpss_hipp_mc46.bits.c1_0mvsad = width - 1;

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_graph_psb(hi_s32 *graph_psb)
{
    vdp_regs_type *vdp_reg = HI_NULL;
    u_xdp_2dscale_graph_psb xdp_2dscale_graph_psb;

    vdp_reg = pq_hal_get_vdp_set_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(vdp_reg);

    xdp_2dscale_graph_psb.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_psb.u32)));
    *graph_psb = xdp_2dscale_graph_psb.bits.graph_psb;

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_dci_mean_value(hi_u32 handle, hi_u32 *mean_value)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(mean_value);
    *mean_value = g_dei_mean_value_array[handle];

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_dei_stt_info(hi_drv_pq_vpss_stt_info *info_in, drv_pq_dei_input_info *dei_api_input)
{
    hi_s32 ret;
    hi_drv_pq_vpss_timming vpss_timming = {0};

    dei_api_input->frame_height = info_in->height;
    dei_api_input->frame_width = info_in->width;

    dei_api_input->blk_siz_h = 8; /* 8: num */
    dei_api_input->blk_siz_v = 4; /* 4: num */

    dei_api_input->me_width = info_in->width;
    dei_api_input->me_height = info_in->height / 2; /* 2: num */
    dei_api_input->handle = info_in->handle_id;

    drv_pq_get_vpss_timming(&vpss_timming);
    dei_api_input->src_width = (hi_s32)vpss_timming.width;
    dei_api_input->src_height = (hi_s32)vpss_timming.height;
#ifndef PQ_DPT_V900_SUPPORT
    dei_api_input->input_src = HI_DRV_SOURCE_HDMI;
#else
    dei_api_input->input_src = info_in->input_src;
#endif

    ret = pq_hal_update_dei_global_motion(info_in, dei_api_input);
    PQ_CHECK_RETURN_SUCCESS(ret);
#ifdef PQ_ALG_DCI
    ret = pq_hal_get_dci_mean_value(info_in->handle_id, &dei_api_input->dci_mean_value);
    PQ_CHECK_RETURN_SUCCESS(ret);
#else
    dei_api_input->dci_mean_value = 0;
#endif

    pq_hal_get_dei_di_param(info_in, &dei_api_input->di);
    ret = pq_hal_get_dei_up_param(&dei_api_input->dei_update);
    PQ_CHECK_RETURN_SUCCESS(ret);
#ifdef PQ_2DSCALER_SUPPORT
    ret = pq_hal_get_graph_psb(&dei_api_input->graph_psb);
#else
    dei_api_input->graph_psb = 0;
#endif

    PQ_CHECK_RETURN_SUCCESS(ret);
    ret = pq_hal_get_dei_me_param(info_in, &dei_api_input->me);
    PQ_CHECK_RETURN_SUCCESS(ret);
    ret = pq_hal_update_me_bndy(info_in);
    PQ_CHECK_RETURN_SUCCESS(ret);

    dei_api_input->film_type_from_fmd = dei_api_input->vpss_cfg_info.ifmd_playback.film_mode;

    return HI_SUCCESS;
}
