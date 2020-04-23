/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: pq ifmd alg api
 * Author: pq
 * Create: 2019-01-1
 */

#include "drv_pq_comm.h"
#include "pq_mng_ifmd_alg.h"
#include "hi_vpss_register.h"
#include "pq_hal_dei.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* math function define */
#define clip0(x)                (((x) < 0) ? 0 : (x))
#define clip1(high, x)          (MAX2(MIN2((x), high), 0))
#define ifmd_clip3(min, max, x) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define ifmd_round(a, rup, rdn) ((a) > (rup) ? (rup) : ((a) < (rdn) ? (rdn) : (a)))

#define MAX_S32_VALUE_DIV_1000 2147483

static hi_u32 g_film_mode = 0;

/* ifmd param */
static ifmd_total_soft_para_s g_ifmd_total_soft_para = { { { 0 } } };
static ifmd_hard2soft_api g_ifmd_hard_to_soft_api = { { 0 } };

hi_s8 g_sv_cadence_32_tf[10] = { -1, 1, 0, 0, 0, -1, 1, 0, 0, 0 }; /* 10 index */
hi_s8 g_kf_cadence_32_tf[10] = { 0,  0, 1, 0, 1, 0,  0, 1, 0, 1 };   /* 10 index */
hi_s8 g_md_cadence_32_tf[10] = { 0,  1, 0, 1, 0, 0,  1, 0, 1, 0 };   /* 10 index */

hi_s8 g_sv_cadence_2332_tf[10] = { 0, 0, -1, 1, 0, -1, 1, 0, 0, 0 }; /* 10 index */
hi_s8 g_kf_cadence_2332_tf[10] = { 0, 1, 0,  0, 1, 0,  0, 1, 0, 1 };   /* 10 index */
hi_s8 g_md_cadence_2332_tf[10] = { 1, 0, 0,  1, 0, 0,  1, 0, 1, 0 };   /* 10 index */

hi_s8 g_sv_cadence_2332_tbf[10] = { 0, -1, 1, 0, -1, 1, 0, 0, 0, 0 }; /* 10 index */
hi_s8 g_kf_cadence_2332_tbf[10] = { 1, 0,  0, 1, 0,  0, 1, 0, 1, 0 };   /* 10 index */
hi_s8 g_md_cadence_2332_tbf[10] = { 0, 0,  1, 0, 0,  1, 0, 1, 0, 1 };   /* 10 index */

hi_s8 g_sv_cadence_32322_tf[12] = { -1, 1, 0, 0, 0, -1, 1, 0, 0, 0, 0, 0 }; /* 12 index */
hi_s8 g_kf_cadence_32322_tf[12] = { 0,  0, 1, 0, 1, 0,  0, 1, 0, 1, 0, 1 };   /* 12 index */
hi_s8 g_md_cadence_32322_tf[12] = { 0,  1, 0, 1, 0, 0,  1, 0, 1, 0, 1, 0 };   /* 12 index */

hi_s8 g_sv_cadence_32322_tbf[12] = { 1, 0, 0, 0, -1, 1, 0, 0, 0, 0, 0, -1 }; /* 12 index */
hi_s8 g_kf_cadence_32322_tbf[12] = { 0, 1, 0, 1, 0,  0, 1, 0, 1, 0, 1, 0 };   /* 12 index */
hi_s8 g_md_cadence_32322_tbf[12] = { 1, 0, 1, 0, 0,  1, 0, 1, 0, 1, 0, 0 };   /* 12 index */

hi_s8 g_sv_cadence_11_2_3_tf[50] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 50 index */
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, -1, 1, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, -1, 1, 0};

hi_s8 g_kf_cadence_11_2_3_tf[50] = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, /* 50 index */
                                     0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                                     0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
                                     1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
                                     1, 0, 1, 0, 1, 0, 1, 0, 0, 1};

hi_s8 g_md_cadence_11_2_3_tf[50] = { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, /* 50 index */
                                     1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
                                     1, 0, 0, 1, 0, 1, 0, 1, 0, 1,
                                     0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                                     0, 1, 0, 1, 0, 1, 0, 0, 1, 0};

// 对2:2:2:2:2:2:2:2:2:2:2:3模式（此处统一简写为11_2_3）进行特殊处理，应用sigma参数作为判断标准。
const hi_s32 g_ca_sigma_cadence_11_2_3_tf[50] = { /* 50 index */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};

pq_ifmd_fod g_ca_fod_modes_mode[TOTAL_MODES][2] = {    /* 2 index */
    { TF, 0 },
    { TF, TBF },
    { TF, TBF },
    { TF, 0 },
};

typedef struct {
    hi_s32 min_sigma[4]; /* 4 index */
    hi_s32 min_sigma_pos[4];   /* 4 index */
    hi_s32 *p_sigma;
    hi_s32 sigma_alion_pos;
}pq_ifmd_subtitle;

const hi_s8 g_ac_ss_thr_tf[TOTAL_MODES] = { 0,  0,  0,  0 };
const hi_s8 g_ac_fmv_thr_tf[TOTAL_MODES] = { 0,  0,  0,  0 };
const hi_s8 g_ca_period[TOTAL_MODES] = { 10, 10, 12, 50 };
const hi_s8 g_ca_period_tk[TOTAL_MODES] = { 20, 20, 24, 100 };
const hi_s8 g_ca_num_fo_cases[TOTAL_MODES] = { 1,  2,  2,  1 };
const hi_s8 g_ca_key_frame_range[TOTAL_MODES] = { 3,  3,  3,  3 };

/**********Fod Param**********************/
#define FIELD_TOP_FIRST    0
#define FIELD_BOTTOM_FIRST 1
#define FIELD_UNKNOWN      2

#define REF_TOP_FIRST    0
#define REF_BOTTOM_FIRST 1

#define FOD_ENABLE       1
#define FOD_TOP_FIRST    2
#define FOD_BOTTOM_FIRST 3

/* definition of thresholds */
// #define NOISE_THR        20
#define SADDIFF_ACC_THR  1000
#define FIELD_ORDER_THR  200
#define SINGLE_FRAME_MAX 200

#define optm_alg_abs(x)              (((x) < 0) ? -(x) : (x))
#define optm_alg_min2(x, y)          (((x) < (y)) ? (x) : (y))
#define optm_alg_max2(x, y)          (((x) > (y)) ? (x) : (y))
#define optm_alg_min3(x, y, z)       (optm_alg_min2(optm_alg_min2((x), (y)), (z)))
#define optm_alg_clip3(low, high, x) (optm_alg_max2(optm_alg_min2((x), (high)), (low)))
#define optm_alg_round(x)            ((((x) % 10) > 4) ? (((x) / 10 + 1) * 10) : (x))

hi_s32 ifmd_get_chd_of_2fields(ifmd_scd_status_s *p_scd_detector_status)
{
    hi_u8 i;
    hi_s32 chd = 0;
    hi_s32 *p0, *p1;
    p0 = p_scd_detector_status->hist_of_1st_frame;
    p1 = p_scd_detector_status->hist_of_2nd_frame;

    for (i = 0; i < 64; i++) { /* 64 index */
        if (p1[i]) {
            chd += ((p1[i] - p0[i]) * (p1[i] - p0[i]) / p1[i]);
        }
    }

    return chd;
}

/* 模式调试开关 */
hi_void ifmd_final_mode_switch(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                               ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_s32 mode = sp_ifmd_32_soft_status->final_lock_mode_dei / 100; /* 100 mask */

    if (sp_ifmd_32_soft_status->final_lock_mode_dei) {
        if (sp_ifmd_32_soft_ctrl->mode_32_en == 0 && mode == MODE_32) {
            sp_ifmd_32_soft_status->final_lock_mode_dei = 0;
        }

        if (sp_ifmd_32_soft_ctrl->mode_2332_en == 0 && mode == MODE_2332) {
            sp_ifmd_32_soft_status->final_lock_mode_dei = 0;
        }

        if (sp_ifmd_32_soft_ctrl->mode_32322_en == 0 && mode == MODE_32322) {
            sp_ifmd_32_soft_status->final_lock_mode_dei = 0;
        }

        if (sp_ifmd_32_soft_ctrl->mode_11_2_3_en == 0 && mode == MODE_11_2_3) {
            sp_ifmd_32_soft_status->final_lock_mode_dei = 0;
        }
    }

    if (sp_ifmd_32_soft_status->b_is_progressive && sp_ifmd_32_soft_ctrl->mode_22_en == 0) {
        sp_ifmd_32_soft_status->b_is_progressive = 0;
    }

    if (sp_ifmd_32_soft_status->final_om_dei && sp_ifmd_32_soft_ctrl->mode_55_64_87_en == 0) {
        sp_ifmd_32_soft_status->final_om_dei = 0;
    }
}

/* Read Ifmd stt param */
hi_void pq_alg_get_ifmd_stt_param(vpss_stt_reg_type *pst_stt_reg, vpss_scan_stt_reg_type *pst_scan_stt_reg)
{
    memcpy(g_ifmd_hard_to_soft_api.histo_of_newest_field,
           &(pst_stt_reg->vpss_wstt_s32_histoofnewestfield00.u32), sizeof(hi_s32) * 64); /* 64: num */
    memcpy(g_ifmd_hard_to_soft_api.comb_histo_bin03,
           &(pst_stt_reg->vpss_wstt_combhistobin6503_00.u32), sizeof(hi_s32) * 64);    /* 64 index */
    memcpy(g_ifmd_hard_to_soft_api.comb_histo_bin12,
           &(pst_stt_reg->vpss_wstt_combhistobin6512_00.u32), sizeof(hi_s32) * 64);    /* 64  index */
    memcpy(g_ifmd_hard_to_soft_api.comb_histo_bin23,
           &(pst_stt_reg->vpss_wstt_combhistobin6523_00.u32), sizeof(hi_s32) * 64);    /* 64  index */
    memcpy(g_ifmd_hard_to_soft_api.histo_bin,
           &(pst_stt_reg->vpss_wstt_ahistobin00.u32), sizeof(hi_s32) * 64);    /* 64  index */

    g_ifmd_hard_to_soft_api.pcc_match = pst_stt_reg->vpss_wstt_pccmatch.bits.pccmatch;
    g_ifmd_hard_to_soft_api.pcc_non_match = pst_stt_reg->vpss_wstt_pccnonmatch.bits.pccnonmatch;
    g_ifmd_hard_to_soft_api.pcc_crss = pst_stt_reg->vpss_wstt_pcccrss.bits.pcccrss;
    g_ifmd_hard_to_soft_api.pcc_fwd_tkr = pst_stt_reg->vpss_wstt_pccfwdtkr.bits.pccfwdtkr;
    g_ifmd_hard_to_soft_api.pcc_bwd_tkr = pst_stt_reg->vpss_wstt_pccbwdtkr.bits.pccbwdtkr;

    g_ifmd_hard_to_soft_api.match_um = pst_stt_reg->vpss_wstt_matchum.bits.matchum;
    g_ifmd_hard_to_soft_api.match_um2 = pst_stt_reg->vpss_wstt_matchum2.bits.matchum2;
    g_ifmd_hard_to_soft_api.non_match_um = pst_stt_reg->vpss_wstt_nonmatchum.bits.nonmatchum;
    g_ifmd_hard_to_soft_api.non_match_um2 = pst_stt_reg->vpss_wstt_nonmatchum2.bits.nonmatchum2;

    g_ifmd_hard_to_soft_api.frm_it_diff = pst_stt_reg->vpss_wstt_s32_frmitdiff.bits.s32frmitdiff;
    g_ifmd_hard_to_soft_api.frm_it_diff_be = pst_stt_reg->vpss_wstt_s32_frmitdiffbe.bits.s32frmitdiffbe;
    g_ifmd_hard_to_soft_api.max_err_line = pst_stt_reg->vpss_wstt_hipp_di_film4.bits.max_err_line;

    g_ifmd_hard_to_soft_api.gmv0_x = s8_to_s32(pst_scan_stt_reg->vpss_me_glbmv_hist_0.bits.me_glbmv0_mvx);
    g_ifmd_hard_to_soft_api.gmv0_y = s7_to_s32(pst_scan_stt_reg->vpss_me_glbmv_hist_0.bits.me_glbmv0_mvy);
    g_ifmd_hard_to_soft_api.gmv0_num = pst_scan_stt_reg->vpss_me_glbmv_hist_0.bits.me_glbmv0_num;

    g_ifmd_hard_to_soft_api.gmv1_x = s8_to_s32(pst_scan_stt_reg->vpss_me_glbmv_hist_1.bits.me_glbmv1_mvx);
    g_ifmd_hard_to_soft_api.gmv1_y = s7_to_s32(pst_scan_stt_reg->vpss_me_glbmv_hist_1.bits.me_glbmv1_mvy);
    g_ifmd_hard_to_soft_api.gmv1_num = pst_scan_stt_reg->vpss_me_glbmv_hist_1.bits.me_glbmv1_num;
}

hi_s32 pq_mng_update_ifmd_api(hi_drv_pq_vpss_stt_info *pst_info_in, hi_drv_pq_ifmd_playback *sp_ifmd_result)
{
    hi_s32 s32_ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(pst_info_in);
    PQ_CHECK_NULL_PTR_RE_FAIL(sp_ifmd_result);
    PQ_CHECK_NULL_PTR_RE_FAIL(pst_info_in->vpss_stt_reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(pst_info_in->scan_stt_reg);

    // 读取硬件层数据
    pq_alg_get_ifmd_stt_param((vpss_stt_reg_type *)pst_info_in->vpss_stt_reg,
                              (vpss_scan_stt_reg_type *)pst_info_in->scan_stt_reg);

    /* the main interface of ifmd alg */
    s32_ret = ifmd_get_playback_config(sp_ifmd_result);

    if (s32_ret != HI_SUCCESS) {
        HI_ERR_PQ("\n   get ifmd play back failed!\n");
        return HI_FAILURE;
    }

    /* onfig ifmd alg result to logic */
    s32_ret = pq_hal_update_ifmd_api_reg(pst_info_in->handle_id, sp_ifmd_result, pst_info_in);

    if (s32_ret != HI_SUCCESS) {
        HI_ERR_PQ("\n   update ifmd alg param failed!\n");
        return HI_FAILURE;
    }

    pq_hal_ifmd_white_block_ctrl(pst_info_in->handle_id, sp_ifmd_result);
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : OptmIFMDAlgGetPlayBackConfig
 功能描述  : API的总入口，用于根据检测和回放延时来获取新的回放配置参数
 返 回 值  :
 调用函数  :
 被调函数  :

 *****************************************************************************/
hi_s32 ifmd_get_playback_config(hi_drv_pq_ifmd_playback *sp_ifmd_result)
{
    // 利用从硬件得到的数据，进行软件层的判断；
    hi_s32 s32_ret;
    ifmd_32_soft_status_s *sp_ifmd_32_soft_status = HI_NULL;
    ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl = HI_NULL;

    ifmd_22_soft_status_s *sp_ifmd_22_softs_tatus = HI_NULL;
    ifmd_22_soft_ctrl_s *sp_ifmd_22_soft_ctrl = HI_NULL;

    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(pst_vir_soft_reg);

    sp_ifmd_32_soft_status = &(g_ifmd_total_soft_para.s_ifmd_32_soft_status);
    sp_ifmd_32_soft_ctrl = &(g_ifmd_total_soft_para.s_ifmd_32_soft_ctrl);
    sp_ifmd_22_softs_tatus = &(g_ifmd_total_soft_para.s_ifmd_22_soft_status);
    sp_ifmd_22_soft_ctrl = &(g_ifmd_total_soft_para.s_ifmd_22_soft_ctrl);

    sp_ifmd_32_soft_ctrl->mode_32_en = pst_vir_soft_reg->ifmd_reg.mode32_en;
    sp_ifmd_32_soft_ctrl->mode_2332_en = pst_vir_soft_reg->ifmd_reg.mode2332_en;
    sp_ifmd_32_soft_ctrl->mode_32322_en = pst_vir_soft_reg->ifmd_reg.mode32322_en;
    sp_ifmd_32_soft_ctrl->mode_22_en = pst_vir_soft_reg->ifmd_reg.mode11_2_3_en;
    sp_ifmd_32_soft_ctrl->mode_55_64_87_en = pst_vir_soft_reg->ifmd_reg.mode22_en;
    sp_ifmd_32_soft_ctrl->mode_11_2_3_en = pst_vir_soft_reg->ifmd_reg.mode55_64_87_en;

    // 更新数据
    ifmd_update_character_buffs(&g_ifmd_hard_to_soft_api, sp_ifmd_32_soft_status, sp_ifmd_32_soft_ctrl,
                                sp_ifmd_22_softs_tatus, sp_ifmd_22_soft_ctrl);

    // 场景切换检测
    s32_ret = ifmd_check_scd(&g_ifmd_hard_to_soft_api, sp_ifmd_32_soft_status,
                             sp_ifmd_32_soft_ctrl->ifmd_det_buff_size);
    if (s32_ret != HI_SUCCESS) {
        HI_ERR_PQ("\n   ifmd_check_scd failed!\n");
        return HI_FAILURE;
    }
    // 非22模式检测
    sp_ifmd_32_soft_status->final_lock_mode_dei = ifmd_detect_pulldown_mode_phase(sp_ifmd_32_soft_status,
                                                                                  sp_ifmd_32_soft_ctrl);

    // 22模式检测
    sp_ifmd_32_soft_status->b_is_progressive =
    ifmd_detect_22_pld(&g_ifmd_hard_to_soft_api, sp_ifmd_22_softs_tatus,
        sp_ifmd_22_soft_ctrl, sp_ifmd_32_soft_status, sp_ifmd_32_soft_ctrl);

    // 低帧率检测55、64、87
    sp_ifmd_32_soft_status->final_om_dei = ifmd_detect_om_mode_phase(sp_ifmd_32_soft_status, sp_ifmd_32_soft_ctrl);

    // 模式调试开关
    ifmd_final_mode_switch(sp_ifmd_32_soft_status, sp_ifmd_32_soft_ctrl);

    s32_ret = ifmd_get_final_result(sp_ifmd_32_soft_status, sp_ifmd_22_softs_tatus, sp_ifmd_32_soft_ctrl,
                                    sp_ifmd_result);
    if (s32_ret != HI_SUCCESS) {
        HI_ERR_PQ("\n   ifmd_get_final_result failed!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 ifmd_get_final_result(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                             ifmd_22_soft_status_s *sp_ifmd_22_softs_tatus, ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl,
                             hi_drv_pq_ifmd_playback *sp_ifmd_result)
{
    hi_u32 index = 0;
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(pst_vir_soft_reg);
    // record the result into lock state buffer
    if (sp_ifmd_32_soft_status->final_lock_mode_dei) {
        sp_ifmd_32_soft_status->final_dei_mode_include_22 = sp_ifmd_32_soft_status->final_lock_mode_dei;
        // 获取非22模式的匹配方向和关键帧信息
        ifmd_get_non22_mode_key_frame_state(sp_ifmd_32_soft_status, sp_ifmd_32_soft_ctrl, sp_ifmd_result);
    } else if (sp_ifmd_32_soft_status->b_is_progressive) {
        /* 800, default value */
        sp_ifmd_32_soft_status->final_dei_mode_include_22 = 800 + /* 800 offset */
                sp_ifmd_32_soft_status->b_is_progressive;
        /* 获取22 模式匹配方向和关键帧 */
        sp_ifmd_result->key_frame = ifmd_get_22_mode_key_frame_state(
        sp_ifmd_32_soft_status, sp_ifmd_22_softs_tatus,
        sp_ifmd_result, sp_ifmd_32_soft_ctrl->polarity_last);
    } else if (sp_ifmd_32_soft_status->final_om_dei == 1) {
        sp_ifmd_32_soft_status->final_dei_mode_include_22 = 500; /* 500, default value */
        index = (sp_ifmd_32_soft_status->field_num) % BUFF_SIZE;
        sp_ifmd_result->key_frame =
            sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[index];
        sp_ifmd_result->dir_mch =
            sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[index];
    } else if (sp_ifmd_32_soft_status->final_om_dei == 2) {      /* NTSC -> PAL  2, mode index */
        sp_ifmd_32_soft_status->final_dei_mode_include_22 = 600; /* 600, default value */
        sp_ifmd_result->key_frame = 1;
        sp_ifmd_result->dir_mch = -1;
    } else {
        sp_ifmd_32_soft_status->final_dei_mode_include_22 = 0;
        sp_ifmd_result->key_frame = 1;
        sp_ifmd_result->dir_mch = -1;
    }

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug_en) {
        HI_ERR_PQ(" I:%3d,%2d\n", sp_ifmd_32_soft_status->final_dei_mode_include_22, sp_ifmd_result->dir_mch);
    }

    if (sp_ifmd_32_soft_status->final_dei_mode_include_22 > 0 &&
        sp_ifmd_32_soft_status->final_dei_mode_include_22 < 11) { /* 11 threshold value */
        sp_ifmd_result->film_mode = 32;  /* 32 mode value */
    } else if (sp_ifmd_32_soft_status->final_dei_mode_include_22 > 100 &&  /* 100, threshold value */
               sp_ifmd_32_soft_status->final_dei_mode_include_22 < 200) {  /* 200, threshold value */
        sp_ifmd_result->film_mode = 2332; /* 2332 mode value */
    } else if (sp_ifmd_32_soft_status->final_dei_mode_include_22 > 200 &&  /* 200,  threshold value */
               sp_ifmd_32_soft_status->final_dei_mode_include_22 < 300) {  /* 300, threshold value */
        sp_ifmd_result->film_mode = 32322; /* 32322 mode value */
    } else if (sp_ifmd_32_soft_status->final_dei_mode_include_22 > 300 &&  /* 300,  threshold value */
               sp_ifmd_32_soft_status->final_dei_mode_include_22 < 400) {  /* 400, threshold value */
        sp_ifmd_result->film_mode = 1123;  /* 1123 mode value */
    } else if (sp_ifmd_32_soft_status->final_dei_mode_include_22 >= 500 && /* 500, threshold value */
               sp_ifmd_32_soft_status->final_dei_mode_include_22 < 600) {  /* 600, threshold value */
        sp_ifmd_result->film_mode = 556487;  /* 556487 unnormal mode value */
    } else if (sp_ifmd_32_soft_status->final_dei_mode_include_22 == 600) { /* 600 threshold value */
        sp_ifmd_result->film_mode = 600;  /* 600 unnormal mode value */
    } else if (sp_ifmd_32_soft_status->final_dei_mode_include_22 > 800 &&  /* 800, threshold value */
               sp_ifmd_32_soft_status->final_dei_mode_include_22 < 900) {  /* 900, threshold value */
        sp_ifmd_result->film_mode = 22; /* 22 mode value */
    } else {
        sp_ifmd_result->film_mode = 0;
    }

    if (sp_ifmd_result->dir_mch != -1) {  /* (FilmType) */
        sp_ifmd_result->die_out_sel = 1;
    } else {
        sp_ifmd_result->die_out_sel = 0;
    }

    sp_ifmd_32_soft_ctrl->polarity_last = 1 - sp_ifmd_32_soft_ctrl->polarity_last;

    if (sp_ifmd_32_soft_status->field_num == 6600) { /* 6600, threshold */
        sp_ifmd_32_soft_status->field_num = sp_ifmd_32_soft_status->field_num + 1 - 3300; /* 3300, threshold */
    } else {
        sp_ifmd_32_soft_status->field_num = sp_ifmd_32_soft_status->field_num + 1;
    }

    g_film_mode = sp_ifmd_result->film_mode;

    return HI_SUCCESS;
}
/*************************************************
Function:           ifmd_locker_scheme
Description:        一个通用的模式相位的锁定策略，寻找最大计数器的相位，与相关阈值进行比较，确定是否锁定某个相位
Calls:             N/A
Called By:         IP()等
Input:             当前模式下各相位的锁定和退锁计数器的状态，
Output:            锁定模式
Return:            锁定模式
Others:            该锁定器是通用的，可同时用于FOD、Pulldown检测
*************************************************/
hi_s32 ifmd_scheme_for_mode_lock(pq_ifmd_pulldown_mode curr_cadence_mode,
                                 ifmd_32_soft_status_s* sp_ifmd_32_soft_status, hi_s32 num_total_modes,
                                 ifmd_32_soft_ctrl_s* sp_ifmd_32_soft_ctrl)
{
    hi_s32 i;
    hi_s32 mode_lock = 0;
    hi_s32 max_mode_counter = 0;
    hi_s32 max_mode_counter_mode = 0;
    hi_s32 lock_thresh = sp_ifmd_32_soft_ctrl->a_lock_counter_thds[curr_cadence_mode];
    hi_s32 unlock_thresh = sp_ifmd_32_soft_ctrl->a_unlock_counter_thds[curr_cadence_mode];
    hi_s32 unlock_counter = sp_ifmd_32_soft_status->unlock_counter[curr_cadence_mode];
    hi_s8 *p_mode_counters = sp_ifmd_32_soft_status->mode_counters[curr_cadence_mode];
    hi_s32 scene_change_counter = sp_ifmd_32_soft_status->scene_change_counter;
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(sp_ifmd_32_soft_status);
    PQ_CHECK_NULL_PTR_RE_FAIL(sp_ifmd_32_soft_ctrl);
    PQ_CHECK_NULL_PTR_RE_FAIL(p_mode_counters);
    PQ_CHECK_NULL_PTR_RE_FAIL(pst_vir_soft_reg);
    unlock_thresh = (scene_change_counter ? 0 : unlock_thresh);

    // lock scheme
    for (i = 0; i < num_total_modes; i++) {
        if (p_mode_counters[i] > max_mode_counter) {
            max_mode_counter = p_mode_counters[i];
            max_mode_counter_mode = i;
        }
    }

    if (max_mode_counter > lock_thresh) {  // lock a mode phase
        // iMax_mode_counter_mode+1, corresponding to the value of enum , see the header file
        mode_lock = max_mode_counter_mode;
    } else if (unlock_counter > unlock_thresh) {  // nothing locked
        mode_lock = 0;
    }

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug32_en[2]) { /* 2, index */
        if (curr_cadence_mode == MODE_32) {
            HI_ERR_PQ("iMaxCnt:%2d,", max_mode_counter);
        }
    }

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug2332_en[1]) {
        if (curr_cadence_mode == MODE_2332) {
            HI_ERR_PQ("iMaxCnt:%2d,", max_mode_counter);
        }
    }

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug32322_en[1]) {
        if (curr_cadence_mode == MODE_32322) {
            HI_ERR_PQ("iMaxCnt:%2d,", max_mode_counter);
        }
    }

    return mode_lock;
}

hi_void ifmd_scheme_for_mode_unlock(pq_ifmd_pulldown_mode curr_cadence_mode,
                                    ifmd_32_soft_status_s* sp_ifmd_32_soft_status, hi_s32 num_total_modes,
                                    ifmd_32_soft_ctrl_s* sp_ifmd_32_soft_ctrl)
{
    hi_s32 i;
    hi_s32 unlock_thresh = sp_ifmd_32_soft_ctrl->a_unlock_counter_thds[curr_cadence_mode];
    hi_s32 unlock_counter = sp_ifmd_32_soft_status->unlock_counter[curr_cadence_mode];
    hi_s8 *p_mode_counters = sp_ifmd_32_soft_status->mode_counters[curr_cadence_mode];
    hi_s32 scene_change_counter = sp_ifmd_32_soft_status->scene_change_counter;
    hi_s32 locked_mode = sp_ifmd_32_soft_status->mode_locked[curr_cadence_mode];

    unlock_thresh = (scene_change_counter ? 0 : unlock_thresh);

    if (unlock_counter > unlock_thresh) {
        // unlock the mode_lock
        for (i = 0; i < num_total_modes; i++) {  // clear the mode counters

            if (!(i && p_mode_counters[i] < (unlock_counter + 1) &&
                  p_mode_counters[i] > unlock_counter / 2)) { /* 2, ratio */
                p_mode_counters[i] = 0;
            }
        }  // clear the current mode detector counter
    } else if (!locked_mode && unlock_counter > unlock_thresh) {
        p_mode_counters[0] = 0;
    }

    return;
}

hi_s32 ifmd_locker_scheme(pq_ifmd_pulldown_mode curr_cadence_mode, ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                          hi_s32 num_total_modes, ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_s32 mode_lock;

    mode_lock = ifmd_scheme_for_mode_lock(curr_cadence_mode, sp_ifmd_32_soft_status, num_total_modes,
                                          sp_ifmd_32_soft_ctrl);

    ifmd_scheme_for_mode_unlock(curr_cadence_mode, sp_ifmd_32_soft_status, num_total_modes, sp_ifmd_32_soft_ctrl);

    return mode_lock;
}

/*************************************************
Function:       AdjustModeCounters
Description:    一个通用的计数器调整函数
Calls:          N/A
Called By:      IP()等
Input:          多个模式计数器指针，总共模式个数(注意模式0应为所有实际模式都不符模式，也可以锁定)，已锁定模式，当前和前一次检测状态，计数器调整步长，退锁计数器指针
Output:         锁定模式
Return:         锁定模式
Others:         该锁定器是通用的，可同时用于FOD、Pulldown检测
*************************************************/
hi_void ifmd_adjust_counters(hi_s32 curr_cadence_mode, ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                             hi_s32 curr_trend)
{
    hi_s32 i;
    hi_s32 delta_count = 1;
    hi_s8 *p_mode_counters = HI_NULL;
    ifmd_cad_mode_para_set_s p_cmd_para_set = { 0 };
    hi_s32 num_total_modes;

    PQ_CHECK_NULL_PTR_RE_NULL(sp_ifmd_32_soft_status);
    p_cmd_para_set = sp_ifmd_32_soft_status->s_cmd_para_set;
    num_total_modes = p_cmd_para_set.phases_each_mode * p_cmd_para_set.num_fod_cases + 1;
    p_mode_counters = sp_ifmd_32_soft_status->mode_counters[curr_cadence_mode];
    PQ_CHECK_NULL_PTR_RE_NULL(p_mode_counters);
    p_mode_counters[curr_trend] = ifmd_round(p_mode_counters[curr_trend] + delta_count, MODE_UP_BOUND,
                                             MODE_DOWN_BOUND);

    for (i = 0; i < num_total_modes; i++) {
        if (i != curr_trend) {
            p_mode_counters[i] = ifmd_round(p_mode_counters[i] - delta_count, MODE_UP_BOUND, MODE_DOWN_BOUND);
        }
    }

    return;
}

hi_void ifmd_adjust_for_unlock(hi_s32 curr_cadence_mode, ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                               hi_s32 curr_trend)
{
    hi_s32 i;
    hi_s32 delta_count = 1;
    hi_s8 *p_mode_counters = HI_NULL;
    hi_u16 locked_mode = sp_ifmd_32_soft_status->mode_locked[curr_cadence_mode];
    hi_u16 last_trend = sp_ifmd_32_soft_status->last_trend[curr_cadence_mode];
    hi_u8 *p_unlock_counter = &(sp_ifmd_32_soft_status->unlock_counter[curr_cadence_mode]);
    hi_u8 non_monotony = sp_ifmd_32_soft_status->flag_adjust;
    ifmd_cad_mode_para_set_s p_cmd_para_set = { 0 };
    hi_s32 num_total_modes;

    PQ_CHECK_NULL_PTR_RE_NULL(sp_ifmd_32_soft_status);
    p_cmd_para_set = sp_ifmd_32_soft_status->s_cmd_para_set;
    num_total_modes = p_cmd_para_set.phases_each_mode * p_cmd_para_set.num_fod_cases + 1;

    p_mode_counters = sp_ifmd_32_soft_status->mode_counters[curr_cadence_mode];
    PQ_CHECK_NULL_PTR_RE_NULL(p_mode_counters);

    if (last_trend != 0 || locked_mode) {
        (*p_unlock_counter) = ifmd_round((*p_unlock_counter) + delta_count, MODE_UP_BOUND, MODE_DOWN_BOUND);

        if (non_monotony) {
            for (i = 1; i < num_total_modes; i++) {
                p_mode_counters[i] = 0;
            }

            (*p_unlock_counter) = MODE_UP_BOUND;
        }
    } else {
        (*p_unlock_counter) = 0;
    }

    return;
}

hi_void ifmd_adjust_for_lock(hi_s32 curr_cadence_mode, ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                             hi_s32 curr_trend)
{
    hi_s32 delta_count = 1;
    hi_s8 *p_mode_counters = HI_NULL;
    hi_u16 locked_mode = sp_ifmd_32_soft_status->mode_locked[curr_cadence_mode];
    hi_u16 last_trend = sp_ifmd_32_soft_status->last_trend[curr_cadence_mode];
    hi_u8 *p_unlock_counter = &(sp_ifmd_32_soft_status->unlock_counter[curr_cadence_mode]);
    ifmd_cad_mode_para_set_s p_cmd_para_set = { 0 };
    hi_s32 plus_unlock_cnt_flag;

    PQ_CHECK_NULL_PTR_RE_NULL(sp_ifmd_32_soft_status);
    p_cmd_para_set = sp_ifmd_32_soft_status->s_cmd_para_set;
    p_mode_counters = sp_ifmd_32_soft_status->mode_counters[curr_cadence_mode];
    PQ_CHECK_NULL_PTR_RE_NULL(p_mode_counters);

    plus_unlock_cnt_flag = (!locked_mode && curr_trend != last_trend)
                           || (!locked_mode && p_mode_counters[0] > MODE_COUNTER_THRD);

    if (plus_unlock_cnt_flag) {
        (*p_unlock_counter) = ifmd_round((*p_unlock_counter) + delta_count, MODE_UP_BOUND, MODE_DOWN_BOUND);
    } else if (!locked_mode && last_trend == curr_trend) {
        (*p_unlock_counter) = 0;
    } else {
        (*p_unlock_counter) = ifmd_round((*p_unlock_counter) - delta_count, MODE_UP_BOUND, MODE_DOWN_BOUND);
    }
    return;
}

hi_void ifmd_adjust_mode_counters(hi_s32 curr_cadence_mode, ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                                  hi_s32 curr_trend)
{
    PQ_CHECK_NULL_PTR_RE_NULL(sp_ifmd_32_soft_status);
    ifmd_adjust_counters(curr_cadence_mode, sp_ifmd_32_soft_status, curr_trend);

    if (!curr_trend) {
        ifmd_adjust_for_unlock(curr_cadence_mode, sp_ifmd_32_soft_status, curr_trend);
    } else {
        ifmd_adjust_for_lock(curr_cadence_mode, sp_ifmd_32_soft_status, curr_trend);
    }

    return;
}

hi_s32 ifmd_get_min_value(hi_s32 *p_value, hi_s32 range, hi_s32 start_pos, hi_s32 buffer_size)
{
    hi_s32 i, temp;
    hi_s32 min = MAX_INT;

    PQ_CHECK_ZERO_RE_FAIL(buffer_size);
    for (i = 0; i < range; i++) {
        temp = p_value[(start_pos + buffer_size - i) % buffer_size];

        if (min > temp) {
            min = temp;
        }
    }

    return min;
}

hi_s32 ifmd_get_max_value(hi_s32 *p_value, hi_s32 range, hi_s32 start_pos, hi_s32 buffer_size)
{
    hi_s32 i, temp;
    hi_s32 max = MIN_INT;

    PQ_CHECK_ZERO_RE_FAIL(buffer_size);

    for (i = 0; i < range; i++) {
        temp = p_value[(start_pos + buffer_size - i) % buffer_size];

        if (max < temp) {
            max = temp;
        }
    }

    return max;
}

hi_s32 ifmd_get_2max_value(hi_s32 *p_value, hi_s32 *p_2max, hi_s32 range, hi_s32 start_pos, hi_s32 buffer_size)
{
    hi_s32 i, temp;
    hi_s32 max1 = 0;
    hi_s32 max2 = 0;

    PQ_CHECK_ZERO_RE_FAIL(buffer_size);
    for (i = 0; i < range; i++) {
        temp = p_value[(start_pos + buffer_size - i) % buffer_size];

        if (max1 < temp) {
            max2 = max1;
            max1 = temp;
        } else if (max2 < temp) {
            max2 = temp;
        }
    }

    p_2max[0] = max1;
    p_2max[1] = max2;

    return HI_SUCCESS;
}

hi_s32 ifmd_get_3max_value(hi_s32 *p_value, hi_s32 *p_3max, hi_s32 range, hi_s32 start_pos, hi_s32 buffer_size)
{
    hi_s32 i, temp;
    hi_s32 max1 = 0;
    hi_s32 max2 = 0;
    hi_s32 max3 = 0;

    PQ_CHECK_ZERO_RE_FAIL(buffer_size);
    for (i = 0; i < range; i++) {
        temp = p_value[(start_pos + buffer_size - i) % buffer_size];

        if (max1 < temp) {
            max3 = max2;
            max2 = max1;
            max1 = temp;
        } else if (max2 < temp) {
            max3 = max2;
            max2 = temp;
        } else if (max3 < temp) {
            max3 = temp;
        }
    }

    p_3max[0] = max1;
    p_3max[1] = max2;
    p_3max[2] = max3; /* 2, index */

    return HI_SUCCESS;
}

hi_s32 ifmd_get_3min_value(hi_s32 *p_value, hi_s32 *p_3min, hi_s32 range, hi_s32 start_pos, hi_s32 buffer_size)
{
    hi_s32 i, temp;
    hi_s32 min1 = MAX_INT;
    hi_s32 min2 = MAX_INT;
    hi_s32 min3 = MAX_INT;

    PQ_CHECK_ZERO_RE_FAIL(buffer_size);
    for (i = 0; i < range; i++) {
        temp = p_value[(start_pos + buffer_size - i) % buffer_size];

        if (min1 > temp) {
            min3 = min2;
            min2 = min1;
            min1 = temp;
        } else if (min2 > temp) {
            min3 = min2;
            min2 = temp;
        } else if (min3 > temp) {
            min3 = temp;
        }
    }

    p_3min[0] = min1;
    p_3min[1] = min2;
    p_3min[2] = min3; /* 2, index */

    return HI_SUCCESS;
}

/*************************************************
Functions:      使用硬件部分计算所得帧间卡方直方图差(Chi square histogram difference)，进行场景切换检测。
Description:    缓存多帧的CHD信息，
Calls:
Called By:
Called By:      main()
Input:          用于对比的两帧图像、当前对比场序号，硬件统计信息
Output:         是否有场景切换发生，1-是，0-否
Return:         是否有场景切换发生，1-是，0-否
Others:         该输出结果可以用于FOD，也可以用于Pulldown Detection,最新场是否发生了场景切换
*************************************************/
hi_s32 ifmd_check_scd(ifmd_hard2soft_api *sp_ifmd_hard_to_soft_api, ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                      hi_s32 buff_size)
{
    hi_s32 mode_locked = sp_ifmd_32_soft_status->final_lock_mode_dei;
    hi_s32 field_num = sp_ifmd_32_soft_status->field_num;
    hi_s32 *p_sigma = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma;
    hi_s32 *p_comb = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb;
    hi_s32 check_range = mode_locked ? (g_ca_period[mode_locked / 100] + 1) : 9; /* 100, mask, 9, thr */
    hi_s32 max_sigma_1, max_sigma_2, max_sigma_3;
    hi_s32 sigma_3max[3] = { 0 }; /* 3, index */
    hi_s32 max_comb_1, max_comb_2;
    hi_s32 comb_2max[2] = { 0 }; /* 2, index */
    hi_s32 sigma_sc_flag, comb_sc_flag;
    hi_s32 s32_ret;

    s32_ret = ifmd_get_3max_value(p_sigma, sigma_3max, check_range, field_num + 1, buff_size);
    if (s32_ret != HI_SUCCESS) {
        HI_ERR_PQ("\n   ifmd_get_3max_value failed!\n");
        return HI_FAILURE;
    }

    s32_ret = ifmd_get_2max_value(p_comb, comb_2max, check_range, field_num + 1, buff_size);
    if (s32_ret != HI_SUCCESS) {
        HI_ERR_PQ("\n   ifmd_get_2max_value failed!\n");
        return HI_FAILURE;
    }

    max_sigma_1 = sigma_3max[0];
    max_sigma_2 = sigma_3max[1];
    max_sigma_3 = sigma_3max[2]; /* 2, index */

    max_comb_1 = comb_2max[0];
    max_comb_2 = comb_2max[1];

    sigma_sc_flag = (max_sigma_3) &&
                    (max_sigma_2 * 10 / max_sigma_3 <= 12) && /* 10, ratio, 12, threshold */
                    (p_sigma[(field_num + 1) % buff_size] == max_sigma_1) &&
                    (max_sigma_1 * 10 / max_sigma_2 >= 15);                      /* 10, ratio, 15, threshold */
    comb_sc_flag = (max_comb_1 * 10 / (max_comb_2 ? max_comb_2 : 1000) >= 20) && /* 10, ratio, 1000, 20, threshold */
                   (p_comb[(field_num + 1) % buff_size] == max_comb_1);

    if (sigma_sc_flag && comb_sc_flag) {  // scene changing
        sp_ifmd_32_soft_status->scene_change = 1;
    } else {
        sp_ifmd_32_soft_status->scene_change = 0;
    }

    sp_ifmd_32_soft_status->scene_change_counter--;
    sp_ifmd_32_soft_status->scene_change_counter =
    sp_ifmd_32_soft_status->scene_change ? (SCD_COUNT_DOWN_MAX) :
    ifmd_round(sp_ifmd_32_soft_status->scene_change_counter, SCD_COUNT_DOWN_MAX, 0);

    return HI_SUCCESS;
}

hi_s32 ifmd_check_diploid_relation(ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl, hi_s32 num1, hi_s32 num2)
{
    hi_s32 vary;

    hi_s32 thr0 = sp_ifmd_32_soft_ctrl->sv_buff_ctrl.sigma_thr_0;
    hi_s32 thr1 = sp_ifmd_32_soft_ctrl->sv_buff_ctrl.sigma_thr_1;
    hi_s32 thr2 = sp_ifmd_32_soft_ctrl->sv_buff_ctrl.sigma_thr_2;
    hi_s32 thr3 = sp_ifmd_32_soft_ctrl->sv_buff_ctrl.sigma_thr_3;
    hi_s32 thr4 = sp_ifmd_32_soft_ctrl->sv_buff_ctrl.sigma_thr_4;

    num1 = MAX2(num1, 1);
    num2 = MIN2(num2, MAX_S32_VALUE_DIV_1000);

    // sigma changes intensely between adjacent fields, which means the sequence might be a pulldown one
    vary = ((num1 >= thr4 && num2 * 1000 / num1 >                                                /* 1000, ratio */
             (sp_ifmd_32_soft_ctrl->sv_buff_ctrl.low_times * 1000 - 500)) || /* 1000, 500, threshold */
            (num1 >= thr3 && num2 * 1000 / num1 >                                                /* 1000, ratio */
             (sp_ifmd_32_soft_ctrl->sv_buff_ctrl.low_times * 1000 - 250)) || /* 1000, 250, threshold */
            (num1 >= thr2 && num1 < thr3 && num2 / num1 >= sp_ifmd_32_soft_ctrl->sv_buff_ctrl.low_times) ||
            (num1 >= thr1 && num1 < thr2 && num2 / num1 >= sp_ifmd_32_soft_ctrl->sv_buff_ctrl.mid_times) ||
            (num1 >= thr0 && num1 < thr1 && num2 / num1 >= sp_ifmd_32_soft_ctrl->sv_buff_ctrl.high_times) ||
            (num1 <= thr0 && num2 > thr1));

    return vary;
}
/*************************************************
Function:       每场更新状态信息
Description:    根据每场硬件输出的结果，对缓冲状态进行更新，该缓冲中的数据可同时用于FOD或pulldown检测
Calls:
Called By:
Called By:      UpdateCharacterBuffs()
Input:          sigma缓冲，当前缓冲位置序号，缓冲大小
Output:         是否有sigma的跳跃，-1：向下跳跃，0：无跳跃，1：向上跳跃
Return:         是否有sigma的跳跃，-1：向下跳跃，0：无跳跃，1：向上跳跃
Others:         该输出结果可以用于FOD，也可以用于Pulldown Detection
*************************************************/
hi_s32 ifmd_check_sigma_vary(hi_s32 *p_sigma, hi_s32 sigma_buff_idx, ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_s32 sigma_vary = 0;
    hi_s32 max_sigma;
    hi_s32 buff_size = sp_ifmd_32_soft_ctrl->ifmd_det_buff_size;
    hi_s32 pre = (sigma_buff_idx + buff_size - 1) % buff_size;
    hi_s32 cur = sigma_buff_idx % buff_size;

    hi_s32 no_vary_flag;
    hi_s32 neg_vary_flag;
    hi_s32 pos_vary_flag;

    max_sigma = ifmd_get_max_value(p_sigma, 9, sigma_buff_idx, buff_size); /* 9, length */

    no_vary_flag = ((p_sigma[pre] == 0) || (p_sigma[cur] == 0)) &&
                   (pq_abs(p_sigma[pre] - p_sigma[cur]) < 40); /* 40, sigma threshold */
    neg_vary_flag = (p_sigma[cur] == 0) && (p_sigma[pre] != 0) &&
                    (max_sigma > 1000) && (p_sigma[pre] - p_sigma[cur] >= 40); /* 1000, 40, sigma threshold */
    pos_vary_flag = (p_sigma[pre] == 0) && (p_sigma[cur] != 0) &&
                    (max_sigma > 1000) && (p_sigma[cur] - p_sigma[pre] >= 40); /* 1000, 40, sigma threshold */

    if (no_vary_flag) {
        sigma_vary = 0;
    } else if (neg_vary_flag) {
        sigma_vary = -1;
    } else if (pos_vary_flag) {
        sigma_vary = 1;
    } else if ((p_sigma[cur] != 0) && (p_sigma[pre] != 0)) {
        sigma_vary = ifmd_check_diploid_relation(sp_ifmd_32_soft_ctrl, p_sigma[cur], p_sigma[pre]) ? -1 : 0;
        sigma_vary = ifmd_check_diploid_relation(sp_ifmd_32_soft_ctrl, p_sigma[pre], p_sigma[cur]) ? 1 : sigma_vary;
    }

    return sigma_vary;
}

hi_void ifmd_check_cv_feature(ifmd_32_soft_status_s *sp_ifmd_32_soft_status, hi_s32 buff_idx_next,
                              hi_s32 buff_size, ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_s32 i;
    hi_s32 key_frame_range = 10;
    hi_s32 temp_cv = 0;
    hi_s32 temp_nxt_cv = 0;
    hi_s32 kf_cnt_btw_period = 0;
    hi_s32 other_mode_cv_cnt = 0;
    hi_s32 big_small_cnt_btw_period = 0;
    hi_s32 max_comb;
    hi_s32 min_comb;
    hi_s32 *p_field_comb = HI_NULL;
    hi_s32 min_comb_5frm[3] = { 0 }; /* 3, index */
    hi_s32 max_comb_15frm;
    hi_s32 mean_comb;
    hi_u32 tmp_buff_idx;
    hi_u32 tmp_buff_next_idx;
    hi_s32 s32_ret;

    /* 保证max_comb大于0 */
    max_comb = MAX2(1, sp_ifmd_32_soft_status->max_comb);  /* 1, max_comb threshold */
    min_comb = MAX2(20, sp_ifmd_32_soft_status->min_comb); /* 20, min_comb threshold */

    buff_size = MAX2(1, buff_size);

    if ((min_comb <= 1000 && min_comb > 500 && max_comb / min_comb > 8) || /* 1000, 500, comb thr, 8, comb ratio */
        (min_comb <= 500 && max_comb / min_comb > 5)) {                    /* 500, comb thr, 5, comb ratio thr */
        for (i = 0; i < key_frame_range; i++) {
            tmp_buff_idx = (buff_idx_next - key_frame_range + 1 + i + buff_size) % buff_size;
            tmp_buff_idx = ifmd_clip3(0, BUFF_SIZE - 1, tmp_buff_idx);
            temp_cv = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[tmp_buff_idx];

            if (temp_cv) {
                kf_cnt_btw_period++;
            }
        }
    }

    for (i = 0; i < 15; i++) { /* 15, index */
        tmp_buff_idx = (buff_idx_next - key_frame_range + 1 + i + buff_size) % buff_size;
        tmp_buff_idx = ifmd_clip3(0, BUFF_SIZE - 1, tmp_buff_idx);
        temp_cv = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[tmp_buff_idx];

        if (temp_cv && max_comb > 500 && max_comb / min_comb >= 4) { /* 500, 4, threshold */
            other_mode_cv_cnt++;
        }
    }
    sp_ifmd_32_soft_status->sp_frame_diff_characters.other_mode_cv_cnt = other_mode_cv_cnt;

    p_field_comb = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb;

    s32_ret = ifmd_get_3min_value(p_field_comb, min_comb_5frm, 5, /* 5, num */
                                  sp_ifmd_32_soft_status->field_num + 1, buff_size);
    if (s32_ret != HI_SUCCESS) {
        HI_ERR_PQ("\n   ifmd_get_3min_value failed!\n");
        return;
    }

    max_comb_15frm = ifmd_get_max_value(p_field_comb, 15,  // 15: make sure 87mode contain 2 keyframe
                                        sp_ifmd_32_soft_status->field_num + 1, buff_size);
    mean_comb = (min_comb_5frm[0] + min_comb_5frm[1] + min_comb_5frm[2]) / 3; /* 0, 1, 2, index, 3, num */

    // s3: 剔除掉comb的大值不够大或者小值过大的情况，临时退出电影模式！
    if (p_field_comb[buff_idx_next] > max_comb_15frm / 10 && /* 10, ratio */
        p_field_comb[buff_idx_next] < max_comb_15frm / 2 &&  /* 2, ratio */
        max_comb_15frm < 15000) { /* 810: max_comb_15frm < 15000 for cofee, 10000~40000 100~800 100~800 0 0 0 */
        sp_ifmd_32_soft_status->sp_frame_diff_characters.kf_cnt_btw_period = -1;
    } else {
        sp_ifmd_32_soft_status->sp_frame_diff_characters.kf_cnt_btw_period = kf_cnt_btw_period;
    }

    for (i = 0; i < key_frame_range; i += 2) { /* 2, step */
        tmp_buff_idx = (buff_idx_next - key_frame_range + 1 + i + buff_size) % buff_size;
        tmp_buff_idx = ifmd_clip3(0, BUFF_SIZE - 1, tmp_buff_idx);
        tmp_buff_next_idx = (buff_idx_next - key_frame_range + 2 + i + buff_size) % buff_size; /* 2, index offset */
        tmp_buff_next_idx = ifmd_clip3(0, BUFF_SIZE - 1, tmp_buff_next_idx);
        temp_cv = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[tmp_buff_idx];
        temp_nxt_cv = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[tmp_buff_next_idx];

        if ((temp_cv == 0 && temp_nxt_cv == 1) || (temp_cv == 1 && temp_nxt_cv == 0)) {
            big_small_cnt_btw_period++;
        }
    }

    sp_ifmd_32_soft_status->sp_frame_diff_characters.big_small_cnt_btw_period = big_small_cnt_btw_period;
}

hi_s8 ifmd_check_comb_vary(ifmd_32_soft_status_s *sp_ifmd_32_soft_status, hi_s32 buff_idx_next, hi_s32 buff_size,
                           ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_s32 max_comb = MIN_INT;
    hi_s32 min_comb = MAX_INT;
    hi_s32 temp_comb;
    hi_s32 comb_mean_thd;
    hi_s32 i;
    hi_s32 key_frame_range = 5;
    hi_s32 comb_vary;
    hi_u32 index = 0;

    PQ_CHECK_ZERO_RE_FAIL(buff_size);
    for (i = 0; i < key_frame_range; i++) {
        index = (buff_idx_next - i + buff_size) % buff_size;
        temp_comb = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[index];

        if (temp_comb > max_comb) {
            max_comb = temp_comb;
        }

        if (temp_comb < min_comb) {
            min_comb = temp_comb;
        }
    }

    comb_mean_thd = min_comb + ((max_comb - min_comb) / 4); /* 4, ratio */
    max_comb = MAX2(1, max_comb);

    comb_vary = (sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[buff_idx_next] > comb_mean_thd) &&
                (min_comb * 1000 / max_comb < 800); /* 1000, ratio, 800, threshold */

    for (i = 0; i < key_frame_range + 1; i++) {  /*  for film mode 87 */
        index = (buff_idx_next - i + buff_size) % buff_size;
        temp_comb = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[index];

        if (temp_comb > max_comb) {
            max_comb = temp_comb;
        }

        if (temp_comb < min_comb) {
            min_comb = temp_comb;
        }
    }
    sp_ifmd_32_soft_status->max_comb = max_comb;
    sp_ifmd_32_soft_status->min_comb = min_comb;

    return comb_vary;
}

hi_s8 ifmd_check_comb_vary_32(ifmd_32_soft_status_s *sp_ifmd_32_soft_status, hi_s32 buff_idx_next,
                              hi_s32 buff_size,
                              ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_s32 max_comb = MIN_INT;
    hi_s32 max_comb2 = MIN_INT;
    hi_s32 min_comb = MAX_INT;
    hi_s32 temp_comb;
    hi_s32 comb_mean_thd;
    hi_s32 i;
    hi_s32 key_frame_range = 5;
    hi_s32 comb_vary;
    hi_u32 index = 0;

    PQ_CHECK_ZERO_RE_FAIL(buff_size);
    for (i = 0; i < key_frame_range; i++) {
        index = (buff_idx_next - i + buff_size) % buff_size;
        temp_comb = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[index];

        if (temp_comb > max_comb) {
            max_comb2 = max_comb;
            max_comb = temp_comb;
        } else if (temp_comb > max_comb2) {
            max_comb2 = temp_comb;
        }

        if (temp_comb < min_comb) {
            min_comb = temp_comb;
        }
    }

    comb_mean_thd = min_comb + ((max_comb2 - min_comb) / 4); /* 4, ratio */
    max_comb = MAX2(1, max_comb);

    comb_vary = (sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[buff_idx_next] > comb_mean_thd);

    return comb_vary;
}

hi_s8 ifmd_check_el_vary(ifmd_32_soft_status_s *sp_ifmd_32_soft_status, hi_s32 buff_idx_next, hi_s32 buff_size,
                         ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_s32 max_el = MIN_INT;
    hi_s32 max_el2 = MIN_INT;
    hi_s32 min_el = MAX_INT;
    hi_s32 temp_el;
    hi_s32 el_mean_thd;
    hi_s32 i;
    hi_s32 key_frame_range = 5;
    hi_s32 el_vary;

    for (i = 0; i < key_frame_range; i++) {
        temp_el = sp_ifmd_32_soft_status->max_err_line[4 + i]; /* 4, index offset */

        if (temp_el > max_el) {
            max_el2 = max_el;
            max_el = temp_el;
        } else if (temp_el > max_el2) {
            max_el2 = temp_el;
        }

        if (temp_el < min_el) {
            min_el = temp_el;
        }
    }

    el_mean_thd = min_el + ((max_el2 - min_el) / 4); /* 4, ratio */

    el_vary = (sp_ifmd_32_soft_status->max_err_line[4] >= 9); /* 4, index, 9, threshold */

    return el_vary;
}

/*************************************************
Function:       ifmd_calc_sigma
Description:    计算sigma值，公式为sigma=1*bin1+2*(bin2+bin3)+4*(bin4+bin5+...+bin7)+8*(bin8+bin9+...+bin15)
Calls:          N/A
Called By:      CalcFrameDiffParas()
Input:          已计算好的直方图，因此需要在GetHistogramDifference()函数后执行
Output:         sigma值
Return:         sigma值
Others:
*************************************************/
hi_s32 ifmd_calc_sigma(hi_s32 *sg_a_hist_bin, ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                       ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_s32 histo_1 = 0;
    hi_s32 histo_23 = 0;
    hi_s32 histo_47 = 0;
    hi_s32 histo_8t;
    hi_s32 sigma;
    hi_s32 m;

    // 解决sonyno1 收音机错锁成64 87电影模式。更改统计bin的起始位置，由8改为2
    for (m = sp_ifmd_32_soft_ctrl->sigma_bin_start; m < 32 / 2; m++) {  // m=2 /*  32 / 2, end bin */
        histo_1 += sg_a_hist_bin[m];
    }

    for (m = 32 / 2; m < 64 / 2; m++) { /* 32 / 2, start  bin, 64 / 2, end bin */
        histo_23 += sg_a_hist_bin[m];
    }

    for (m = 64 / 2; m < 128 / 2 - 1; m++) { /* 64 / 2, start  bin, 128 / 2, end bin */
        histo_47 += sg_a_hist_bin[m];
    }

    histo_8t = 0;  // 解决CadenceB序列突然出现的白色箭头，会导致在复制场的sigma值偏大

    sigma = histo_1 + 2 * histo_23 + 4 * histo_47 + 8 * histo_8t; /* 2, 4, 8, bin weight */
    sp_ifmd_32_soft_status->sp_frame_diff_characters.hist_bin_1 = histo_1;
    sp_ifmd_32_soft_status->sp_frame_diff_characters.hist_bin_2 = histo_23;
    sp_ifmd_32_soft_status->sp_frame_diff_characters.hist_bin_3 = histo_47;
    sp_ifmd_32_soft_status->sp_frame_diff_characters.hist_bin_4 = histo_8t;
    sp_ifmd_32_soft_status->sp_frame_diff_characters.sigma = sigma;
    return sigma;
}

hi_s32 ifmd_calc_sigma_no_weight(hi_s32 *a_hist_bin, ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_s32 sigma = 0;
    hi_s32 m;
    // 解决sonyno1 收音机错锁成64 87电影模式。更改统计bin的起始位置，由8改为2
    for (m = sp_ifmd_32_soft_ctrl->comb_bin_start; m < 64; m++) { /* 64,  end bin */
        sigma += a_hist_bin[m];
    }

    return sigma;
}

/*************************************************
Function:       ifmd_calc_non_monotony_32
Description:    计算NonMonotony值，公式为non_monotony=0, if bin(i+1)<bini, i=1,2,...,14,15,  non_monotony=1, otherwise.
Calls:          N/A
Called By:      CalcFrameDiffParas()
Input:          已计算好的直方图，因此需要在GetHistogramDifference()函数后执行
Output:         non_monotony值
Return:         non_monotony值
Others:         重点函数
*************************************************/
hi_s32 ifmd_calc_non_monotony_32(ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl, hi_s32 *sg_a_hist_bin,
                                 ifmd_32_soft_status_s *sp_ifmd_32_soft_status, hi_s32 field_num, hi_s32 buff_size)
{
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(pst_vir_soft_reg);
    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug32_en[0]) {
        HI_ERR_PQ("EL:%3d,%2d,%2d, ", sp_ifmd_32_soft_status->max_err_line[4], /* 4, index */
                  sp_ifmd_32_soft_status->gmv_0_x, sp_ifmd_32_soft_status->gmv_0_y);
        // HI_ERR_PQ("%3d,", sp_ifmd_32_soft_status->max_err_line[4]);
    }

    return (sp_ifmd_32_soft_status->max_err_line[4] >= sp_ifmd_32_soft_ctrl->err_line_thr) && /* 4, index */
           !(sp_ifmd_32_soft_status->roma_flag[0] || sp_ifmd_32_soft_status->roma_flag[1]);
}

/*************************************************
Function:       ifmd_calc_non_monotony_2332
Description:    计算NonMonotony值，公式为non_monotony=0, if bin(i+1)<bini, i=1,2,...,14,15,  non_monotony=1, otherwise.
Calls:          N/A
Called By:      CalcFrameDiffParas()
Input:          已计算好的直方图，因此需要在GetHistogramDifference()函数后执行
Output:         non_monotony值
Return:         non_monotony值
Others:         重点函数
*************************************************/
hi_s32 ifmd_calc_non_monotony_2332(ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl, hi_s32 *sg_a_hist_bin,
                                   ifmd_32_soft_status_s *sp_ifmd_32_soft_status, hi_s32 field_num, hi_s32 buff_size)
{
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(pst_vir_soft_reg);
    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug2332_en[0]) {
        HI_ERR_PQ("EL:%3d, ", sp_ifmd_32_soft_status->max_err_line[9]); /* 9, index */
    }

    return sp_ifmd_32_soft_status->max_err_line[9] >= sp_ifmd_32_soft_ctrl->err_line_thr; /* 9, index */
}

/*************************************************
Function:       ifmd_calc_non_monotony_32322
Description:    计算NonMonotony值，公式为non_monotony=0, if bin(i+1)<bini, i=1,2,...,14,15,  non_monotony=1, otherwise.
Calls:          N/A
Called By:      CalcFrameDiffParas()
Input:          已计算好的直方图，因此需要在GetHistogramDifference()函数后执行
Output:         non_monotony值
Return:         non_monotony值
Others:         重点函数
*************************************************/
hi_s32 ifmd_calc_non_monotony_32322(ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl, hi_s32 *sg_a_hist_bin,
                                    ifmd_32_soft_status_s *sp_ifmd_32_soft_status, hi_s32 field_num, hi_s32 buff_size)
{
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(pst_vir_soft_reg);
    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug32322_en[0]) {
        HI_ERR_PQ("EL:%3d, ", sp_ifmd_32_soft_status->max_err_line[11]); /* 11, index */
    }

    return sp_ifmd_32_soft_status->max_err_line[11] >= sp_ifmd_32_soft_ctrl->err_line_thr; /* 11, index */
}

/*************************************************
Function:       ifmd_calc_non_monotony
Description:    计算NonMonotony值，公式为non_monotony=0, if bin(i+1)<bini, i=1,2,...,14,15,  non_monotony=1, otherwise.
Calls:          N/A
Called By:      CalcFrameDiffParas()
Input:          已计算好的直方图，因此需要在GetHistogramDifference()函数后执行
Output:         non_monotony值
Return:         non_monotony值
Others:         重点函数
*************************************************/
hi_s32 ifmd_calc_non_monotony(ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl, hi_s32 *sg_a_hist_bin,
                              ifmd_32_soft_status_s *sp_ifmd_32_soft_status, hi_s32 field_num, hi_s32 buff_size)
{
    hi_s32 i, non_monotony;

    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(pst_vir_soft_reg);
    non_monotony = 0;

    for (i = sp_ifmd_32_soft_ctrl->nm_buff_ctrl.non_monotony_hist_range_start; i <
         sp_ifmd_32_soft_ctrl->nm_buff_ctrl.non_monotony_hist_range_end;
         i++) {
        if ((sg_a_hist_bin[i] - sg_a_hist_bin[i - 1]) >= sp_ifmd_32_soft_ctrl->nm_buff_ctrl.non_monotony_thr) {
            // should be monotone decreasing, so if increase appears, monotony is positive
            non_monotony += (sg_a_hist_bin[i] - sg_a_hist_bin[i - 1]);
        }
    }

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug32_en[0] || pst_vir_soft_reg->ifmd_reg.ifmd_debug32322_en[0] ||
        pst_vir_soft_reg->ifmd_reg.ifmd_debug2332_en[0]) {
        // HI_ERR_PQ("NonCnt:%5d, ", non_monotony);
    }

    return (non_monotony > 25); /* 25, threshold */
}

/*************************************************
Function:       ifmd_update_character_buffs
Description:    缓存多场的sigma、i_non_monotony、sigma_vary、StillFrame、triValueMcomb03MinusMcomb12参数，每场/帧进行状态刷新
Calls:          ifmd_check_sigma_vary(), jump(), still_like()
Called By:      main()
Input:          等待更新的当前缓冲位置序号，缓冲大小，场景切换信息，可访问的第一场的极性信息（顶底场），硬件统计信息
Output:         更新了的缓存状态
Return:         N/A
Others:         该输出结果可以用于FOD，也可以用于Pulldown Detection
*************************************************/
hi_void ifmd_update_character_buffs(ifmd_hard2soft_api *sp_ifmd_hard_to_soft_api,
    ifmd_32_soft_status_s *sp_ifmd_32_soft_status, ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl,
    ifmd_22_soft_status_s *sp_ifmd_22_softs_tatus, ifmd_22_soft_ctrl_s *sp_ifmd_22_soft_ctrl)
{
    hi_s32 sigma_buff_idx_next;
    hi_s32 field_num;
    hi_s32 buff_size;
    hi_s32 scene_change_counter;
    hi_s32 sigma_buff_idx;
    hi_s32 i = 0;
    hi_s32 patch0, patch1;

    hi_s32 ref_fld_polarity = sp_ifmd_32_soft_ctrl->polarity_last;
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();
    hi_s32 h_move_scene_wt, v_move_scene_wt;

    PQ_CHECK_NULL_PTR_RE_NULL(pst_vir_soft_reg);
    pq_hal_get_move_scene_wt(&h_move_scene_wt, &v_move_scene_wt);

    for (i = 19; i > 0; i--) { /* 19, index */
        sp_ifmd_32_soft_status->max_err_line[i] = sp_ifmd_32_soft_status->max_err_line[i - 1];
    }
    sp_ifmd_32_soft_status->max_err_line[0] = g_ifmd_hard_to_soft_api.max_err_line;

    sp_ifmd_32_soft_status->gmv_0_x = g_ifmd_hard_to_soft_api.gmv0_x;
    sp_ifmd_32_soft_status->gmv_0_y = g_ifmd_hard_to_soft_api.gmv0_y;
    sp_ifmd_32_soft_status->gmv_0_num = g_ifmd_hard_to_soft_api.gmv0_num;
    sp_ifmd_32_soft_status->gmv_1_x = g_ifmd_hard_to_soft_api.gmv1_x;
    sp_ifmd_32_soft_status->gmv_1_y = g_ifmd_hard_to_soft_api.gmv1_y;
    sp_ifmd_32_soft_status->gmv_1_num = g_ifmd_hard_to_soft_api.gmv1_num;

    patch0 = pq_abs(sp_ifmd_32_soft_status->gmv_0_x) < 3 && sp_ifmd_32_soft_status->gmv_0_y <= -3; /* 3, -3,  thr */
    patch1 = sp_ifmd_32_soft_ctrl->err_line_thr == 15 &&                                        /* 15, thr */
             sp_ifmd_32_soft_status->min_comb > 20000 &&                                        /* 20000, thr */
             sp_ifmd_32_soft_status->gmv_0_y == 0 && sp_ifmd_32_soft_status->gmv_0_x < -12;     /* -12, mvx threshold */

    sp_ifmd_32_soft_status->roma_flag[1] = sp_ifmd_32_soft_status->roma_flag[0];
    sp_ifmd_32_soft_status->roma_flag[0] = patch0 || patch1;

    if (sp_ifmd_32_soft_status->gmv_0_y == -4) { /* -4, mvy threshold */
        sp_ifmd_32_soft_status->y_cnt = 10;      /* 10, limit value */
    } else {
        sp_ifmd_32_soft_status->y_cnt = MAX2(sp_ifmd_32_soft_status->y_cnt - 1, 0);
    }

    if (sp_ifmd_32_soft_status->gmv_0_x >= 8) { /* 8, mvx threshold */
        sp_ifmd_32_soft_status->x_cnt = 10;     /* 10, limit value */
    } else {
        sp_ifmd_32_soft_status->x_cnt = MAX2(sp_ifmd_32_soft_status->x_cnt - 1, 0);
    }

    // changhong: himedia box -> pic music
    if (sp_ifmd_32_soft_status->y_cnt > 0 && sp_ifmd_32_soft_status->x_cnt > 0 &&
        sp_ifmd_32_soft_status->min_comb >= 1000) { /* 1000, thr */
        sp_ifmd_32_soft_status->music_flag = 1;
    } else {
        sp_ifmd_32_soft_status->music_flag = 0;
    }

    // konkia: menu on pic music
    if (sp_ifmd_32_soft_status->gmv_0_x == 0 && sp_ifmd_32_soft_status->gmv_0_y == 0 &&
        sp_ifmd_32_soft_status->max_err_line[4] <= 2 && /* 4, index, 2, thr */
        sp_ifmd_32_soft_status->min_comb >= 1000) {     /* 1000, thr */
        sp_ifmd_32_soft_status->music_menu_flag = 1;
    } else {
        sp_ifmd_32_soft_status->music_menu_flag = 0;
    }

    field_num = sp_ifmd_32_soft_status->field_num;
    buff_size = sp_ifmd_32_soft_ctrl->ifmd_det_buff_size;
    scene_change_counter = sp_ifmd_32_soft_status->scene_change_counter;

    sigma_buff_idx = field_num % buff_size;
    sigma_buff_idx_next = (sigma_buff_idx + 1) % buff_size;
    sp_ifmd_32_soft_status->sp_frame_diff_characters.sigma =
        ifmd_calc_sigma(sp_ifmd_hard_to_soft_api->histo_bin, sp_ifmd_32_soft_status, sp_ifmd_32_soft_ctrl);
    sp_ifmd_32_soft_status->sp_frame_diff_characters.non_monotony =
        ifmd_calc_non_monotony(sp_ifmd_32_soft_ctrl, sp_ifmd_hard_to_soft_api->histo_bin,
                               sp_ifmd_32_soft_status, field_num, buff_size);
    sp_ifmd_32_soft_status->sp_frame_diff_characters.non_monotony_32 =
        ifmd_calc_non_monotony_32(sp_ifmd_32_soft_ctrl, sp_ifmd_hard_to_soft_api->histo_bin,
                                  sp_ifmd_32_soft_status, field_num, buff_size);
    sp_ifmd_32_soft_status->sp_frame_diff_characters.non_monotony_32322 =
        ifmd_calc_non_monotony_32322(sp_ifmd_32_soft_ctrl, sp_ifmd_hard_to_soft_api->histo_bin,
                                     sp_ifmd_32_soft_status, field_num, buff_size);
    sp_ifmd_32_soft_status->sp_frame_diff_characters.non_monotony_2332 =
        ifmd_calc_non_monotony_2332(sp_ifmd_32_soft_ctrl, sp_ifmd_hard_to_soft_api->histo_bin,
                                    sp_ifmd_32_soft_status, field_num, buff_size);

    pq_hal_set_sigma(sp_ifmd_32_soft_status->sp_frame_diff_characters.sigma);

    // 记录sigma  NonMonotony值
    sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma[sigma_buff_idx_next] =
        sp_ifmd_32_soft_status->sp_frame_diff_characters.sigma;
    sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma_non_monotony[sigma_buff_idx_next] =
        sp_ifmd_32_soft_status->sp_frame_diff_characters.non_monotony;
    sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma_non_monotony_32[sigma_buff_idx_next] =
        sp_ifmd_32_soft_status->sp_frame_diff_characters.non_monotony_32;
    sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma_non_monotony_32322[sigma_buff_idx_next] =
        sp_ifmd_32_soft_status->sp_frame_diff_characters.non_monotony_32322;
    sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma_non_monotony_2332[sigma_buff_idx_next] =
        sp_ifmd_32_soft_status->sp_frame_diff_characters.non_monotony_2332;

    // 记录场景切换位置信息
    sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sc_counter[sigma_buff_idx] = scene_change_counter;

    if (ref_fld_polarity == BOTTOM_FIELD_REF) {  // 更新帧级数据
        sp_ifmd_22_softs_tatus->pld_22_ctx.frame_match_um = sp_ifmd_hard_to_soft_api->match_um;
        sp_ifmd_22_softs_tatus->pld_22_ctx.frame_non_match_um = sp_ifmd_hard_to_soft_api->non_match_um;
        sp_ifmd_22_softs_tatus->pld_22_ctx.frame_match_um_2 = sp_ifmd_hard_to_soft_api->match_um2;
        sp_ifmd_22_softs_tatus->pld_22_ctx.frame_non_match_um_2 = sp_ifmd_hard_to_soft_api->non_match_um2;

        sp_ifmd_22_softs_tatus->pld_22_ctx_be.frame_match_um = sp_ifmd_hard_to_soft_api->non_match_um;
        sp_ifmd_22_softs_tatus->pld_22_ctx_be.frame_non_match_um = sp_ifmd_hard_to_soft_api->match_um;

        sp_ifmd_22_softs_tatus->pld_22_ctx.frame_match_weave = sp_ifmd_hard_to_soft_api->pcc_match;
        sp_ifmd_22_softs_tatus->pld_22_ctx.frame_non_match_weave = sp_ifmd_hard_to_soft_api->pcc_non_match;
        sp_ifmd_22_softs_tatus->pld_22_ctx.frame_pcc_crss = sp_ifmd_hard_to_soft_api->pcc_crss;
        sp_ifmd_22_softs_tatus->pld_22_ctx.frame_match_tkr = sp_ifmd_hard_to_soft_api->pcc_fwd_tkr;
        sp_ifmd_22_softs_tatus->pld_22_ctx.frame_non_match_tkr = sp_ifmd_hard_to_soft_api->pcc_bwd_tkr;

        sp_ifmd_32_soft_status->s_frame_comb.m_comb_23 =
            ifmd_calc_sigma_no_weight(sp_ifmd_hard_to_soft_api->comb_histo_bin23, sp_ifmd_32_soft_ctrl);
        sp_ifmd_32_soft_status->s_frame_comb.m_comb_03 =
            ifmd_calc_sigma_no_weight(sp_ifmd_hard_to_soft_api->comb_histo_bin03, sp_ifmd_32_soft_ctrl);
        sp_ifmd_32_soft_status->s_frame_comb.m_comb_12 =
            ifmd_calc_sigma_no_weight(sp_ifmd_hard_to_soft_api->comb_histo_bin12, sp_ifmd_32_soft_ctrl);

        if (sp_ifmd_32_soft_ctrl->mmm_buff_ctrl.mmm_divi_thr != 0) {
            if (sp_ifmd_32_soft_status->s_frame_comb.m_comb_03 >=
                MAX2((sp_ifmd_32_soft_status->s_frame_comb.m_comb_12 + sp_ifmd_32_soft_status->s_frame_comb.m_comb_12 /
                      sp_ifmd_32_soft_ctrl->mmm_buff_ctrl.mmm_divi_thr),
                     sp_ifmd_32_soft_status->s_frame_comb.m_comb_12 +
                     sp_ifmd_32_soft_ctrl->mmm_buff_ctrl.mmm_add_thr)) {
                sp_ifmd_32_soft_status->sp_frame_diff_characters.a_mcomb_03_minus_mcomb_12[sigma_buff_idx] = 1;
            } else if (MAX2(sp_ifmd_32_soft_status->s_frame_comb.m_comb_03 +
                sp_ifmd_32_soft_status->s_frame_comb.m_comb_03 / sp_ifmd_32_soft_ctrl->mmm_buff_ctrl.mmm_divi_thr,
                sp_ifmd_32_soft_status->s_frame_comb.m_comb_03 + sp_ifmd_32_soft_ctrl->mmm_buff_ctrl.mmm_add_thr) <=
                sp_ifmd_32_soft_status->s_frame_comb.m_comb_12) {
                sp_ifmd_32_soft_status->sp_frame_diff_characters.a_mcomb_03_minus_mcomb_12[sigma_buff_idx] = -1;
            } else {
                sp_ifmd_32_soft_status->sp_frame_diff_characters.a_mcomb_03_minus_mcomb_12[sigma_buff_idx] = 0;
            }
        }

        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[sigma_buff_idx_next] =
            sp_ifmd_32_soft_status->s_frame_comb.m_comb_23;
        // 判断并记录当前是否有帧间的sigma跳跃性，该特性是pulldown序列特有的性质
        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma_vary[sigma_buff_idx] =
            ifmd_check_sigma_vary(sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma, sigma_buff_idx,
                                  sp_ifmd_32_soft_ctrl);
        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[sigma_buff_idx_next] =
            ifmd_check_comb_vary(sp_ifmd_32_soft_status, sigma_buff_idx_next, buff_size, sp_ifmd_32_soft_ctrl);
        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary_32[sigma_buff_idx_next] =
            ifmd_check_comb_vary_32(sp_ifmd_32_soft_status, sigma_buff_idx_next, buff_size, sp_ifmd_32_soft_ctrl);
        sp_ifmd_32_soft_status->sp_frame_diff_characters.el_vary[sigma_buff_idx_next] =
            ifmd_check_el_vary(sp_ifmd_32_soft_status, 4, 20, sp_ifmd_32_soft_ctrl); /* 4, 20, length */

        ifmd_check_cv_feature(sp_ifmd_32_soft_status, sigma_buff_idx_next, buff_size, sp_ifmd_32_soft_ctrl);
    } else {
        sp_ifmd_22_softs_tatus->pld_22_ctx_be.frame_match_um = sp_ifmd_hard_to_soft_api->match_um;
        sp_ifmd_22_softs_tatus->pld_22_ctx_be.frame_non_match_um = sp_ifmd_hard_to_soft_api->non_match_um;
        sp_ifmd_22_softs_tatus->pld_22_ctx_be.frame_match_um_2 = sp_ifmd_hard_to_soft_api->match_um2;
        sp_ifmd_22_softs_tatus->pld_22_ctx_be.frame_non_match_um_2 = sp_ifmd_hard_to_soft_api->non_match_um2;

        sp_ifmd_22_softs_tatus->pld_22_ctx.frame_match_um = sp_ifmd_hard_to_soft_api->non_match_um;
        sp_ifmd_22_softs_tatus->pld_22_ctx.frame_non_match_um = sp_ifmd_hard_to_soft_api->match_um;

        sp_ifmd_22_softs_tatus->pld_22_ctx_be.frame_match_weave = sp_ifmd_hard_to_soft_api->pcc_match;
        sp_ifmd_22_softs_tatus->pld_22_ctx_be.frame_non_match_weave = sp_ifmd_hard_to_soft_api->pcc_non_match;
        sp_ifmd_22_softs_tatus->pld_22_ctx_be.frame_pcc_crss = sp_ifmd_hard_to_soft_api->pcc_crss;

        sp_ifmd_22_softs_tatus->pld_22_ctx_be.frame_match_tkr = sp_ifmd_hard_to_soft_api->pcc_fwd_tkr;
        sp_ifmd_22_softs_tatus->pld_22_ctx_be.frame_non_match_tkr = sp_ifmd_hard_to_soft_api->pcc_bwd_tkr;

        sp_ifmd_32_soft_status->s_frame_comb.m_comb_23 =
            ifmd_calc_sigma_no_weight(sp_ifmd_hard_to_soft_api->comb_histo_bin23, sp_ifmd_32_soft_ctrl);
        sp_ifmd_32_soft_status->s_frame_comb.m_comb_03 =
            ifmd_calc_sigma_no_weight(sp_ifmd_hard_to_soft_api->comb_histo_bin03, sp_ifmd_32_soft_ctrl);
        sp_ifmd_32_soft_status->s_frame_comb.m_comb_12 =
            ifmd_calc_sigma_no_weight(sp_ifmd_hard_to_soft_api->comb_histo_bin12, sp_ifmd_32_soft_ctrl);

        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma_vary[sigma_buff_idx] =
            ifmd_check_sigma_vary(sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma, sigma_buff_idx,
                                  sp_ifmd_32_soft_ctrl);

        if (sp_ifmd_32_soft_ctrl->mmm_buff_ctrl.mmm_divi_thr != 0) {
            if (sp_ifmd_32_soft_status->s_frame_comb.m_comb_03 >=
                MAX2((sp_ifmd_32_soft_status->s_frame_comb.m_comb_12 +
                      sp_ifmd_32_soft_status->s_frame_comb.m_comb_12 /
                      sp_ifmd_32_soft_ctrl->mmm_buff_ctrl.mmm_divi_thr),
                     sp_ifmd_32_soft_status->s_frame_comb.m_comb_12 +
                     sp_ifmd_32_soft_ctrl->mmm_buff_ctrl.mmm_add_thr)) {
                sp_ifmd_32_soft_status->sp_frame_diff_characters.a_mcomb_03_minus_mcomb_12[sigma_buff_idx] = 1;
            } else if (MAX2(sp_ifmd_32_soft_status->s_frame_comb.m_comb_03 +
                sp_ifmd_32_soft_status->s_frame_comb.m_comb_03 / sp_ifmd_32_soft_ctrl->mmm_buff_ctrl.mmm_divi_thr,
                sp_ifmd_32_soft_status->s_frame_comb.m_comb_03 + sp_ifmd_32_soft_ctrl->mmm_buff_ctrl.mmm_add_thr) <=
                sp_ifmd_32_soft_status->s_frame_comb.m_comb_12) {
                sp_ifmd_32_soft_status->sp_frame_diff_characters.a_mcomb_03_minus_mcomb_12[sigma_buff_idx] = -1;
            } else {
                sp_ifmd_32_soft_status->sp_frame_diff_characters.a_mcomb_03_minus_mcomb_12[sigma_buff_idx] = 0;
            }
        }

        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[sigma_buff_idx_next] =
            sp_ifmd_32_soft_status->s_frame_comb.m_comb_23;
        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[sigma_buff_idx_next] =
            ifmd_check_comb_vary(sp_ifmd_32_soft_status, sigma_buff_idx_next, buff_size, sp_ifmd_32_soft_ctrl);
        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary_32[sigma_buff_idx_next] =
            ifmd_check_comb_vary_32(sp_ifmd_32_soft_status, sigma_buff_idx_next, buff_size, sp_ifmd_32_soft_ctrl);
        sp_ifmd_32_soft_status->sp_frame_diff_characters.el_vary[sigma_buff_idx_next] =
            ifmd_check_el_vary(sp_ifmd_32_soft_status, 4, 20, sp_ifmd_32_soft_ctrl); /* 4, 20, length */

        ifmd_check_cv_feature(sp_ifmd_32_soft_status, sigma_buff_idx_next, buff_size, sp_ifmd_32_soft_ctrl);
    }

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug_en) {
        HI_ERR_PQ("C:%6d,", sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[sigma_buff_idx]);
    }
    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug11_2_3_en[2]) { /* 2, index */
        HI_ERR_PQ("%d, ", sp_ifmd_32_soft_status->sp_frame_diff_characters.el_vary[sigma_buff_idx]);
        HI_ERR_PQ("%d, ", sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary_32[sigma_buff_idx]);
    }

    return;
}

/*************************************************
Function:       ifmd_calc_pcc_linear_comp
Description:    用逐个对比，以相同元素个数的线性函数表示的相关性计算公式计算相关性，其中涉及有除法运算
Calls:          N/A
Called By:      各种模式的Detector()
Input:          计算相关性的双方数据，计算时相互对齐的位置，长度信息
Output:         相关性计算结果，取值区间-1000~1000，对应于原始相关性公式的-1.0~1.0
Return:         相关性计算结果，取值区间-1000~1000，对应于原始相关性公式的-1.0~1.0
Others:         该计算仅用于Pulldown
*************************************************/
hi_s32 ifmd_calc_pcc_linear_comp(const hi_s8 *p_ref, hi_s8 *p_to_be_compare, hi_u16 *p_sc_counter,
                                 ifmd_pcc_calc_pos_s pcc_calc_parm)
{
    hi_s32 i, ref_pos, to_be_com_pos, t_to_be_com, t_ref;
    hi_s32 diff_count;
    hi_s32 pcc_result;

    if (p_ref == HI_NULL || p_to_be_compare == HI_NULL) {
        return -1 * DOUBLE_TO_INT_MULTIPLE;
    }

    diff_count = 0;

    for (i = 0; i < pcc_calc_parm.length_ref; i++) {
        ref_pos = (pcc_calc_parm.ref_start_pos + i) % pcc_calc_parm.length_ref;
        to_be_com_pos = (pcc_calc_parm.to_be_com_start_pos + i) % pcc_calc_parm.length_to_be_comp;
        t_to_be_com = p_sc_counter[to_be_com_pos] ? p_ref[ref_pos] : p_to_be_compare[to_be_com_pos];
        t_ref = p_ref[ref_pos];

        if (t_to_be_com != t_ref) {
            diff_count++;
        }
    }

    /* 2, ratio */
    pcc_result = 1 * DOUBLE_TO_INT_MULTIPLE -
    2 * DOUBLE_TO_INT_MULTIPLE * diff_count / pcc_calc_parm.length_ref; /* 2 :num */
    return (pcc_result * 10); /* 10, mask */
}

/*************************************************
Functions:      判断某一cadence模式各相位中的最匹配相位。各cadence通用。
Description:    为每种cadence模式内多场优先顺序下的各相位进行判断比较，求出最匹配相位。对各个cadence都通用。
Calls:
Called By:
Called By:      11_2_3模式的Detector()
Input:          计算相关性的sigma数组，用于存储最小值位置的指针，对比时的起始位置、缓冲长度信息。参考cadence可仅仅用一个相位信息来表示。
Output:         相关性计算结果，取值区间-1000~1000，对应于原始相关性公式的-1.0~1.0
Return:         相关性计算结果，取值区间-1000~1000，对应于原始相关性公式的-1.0~1.0
Others:         该计算仅用于Pulldown中对11_2_3模式相位的检测
*************************************************/
hi_s32 ifmd_calc_sigma_pcc_linear_comp_11_2_3(hi_s32 *p_field_diff_measure, hi_s32 *p_min_pos,
                                              hi_s32 field_diff_start_pos, hi_s32 buff_size, hi_s32 phase)
{
    hi_s32 i, j, k;
    hi_s32 pcc_diff_cur_mode;
    hi_s32 min_pos;
    hi_s32 min_diff = MAX_INT;
    k = 0;
    pcc_diff_cur_mode = 0;

    PQ_CHECK_ZERO_RE_FAIL(buff_size);
    for (i = 0; i <= 25;) { /* 25, phase num */
        min_diff = MAX_INT;
        min_pos = 0;

        for (j = 0; j < 25; j++) { /* 25, phase num */
            k = (field_diff_start_pos + i + j + buff_size) % buff_size;

            if (p_field_diff_measure[k] < min_diff) {
                min_diff = p_field_diff_measure[k];
                min_pos = i + j + 2; /* 2, index offset */
            }
        }

        // liwenrong modified3
        if (0 == g_ca_sigma_cadence_11_2_3_tf[(min_pos +
            phase * 2 + 0 + field_diff_start_pos + 50) % 50]) { /* 2, period , 50 period length */
            pcc_diff_cur_mode++;
        } else {
            pcc_diff_cur_mode--;
        }

        if (i == 0) {
            i += 13; /* 13, index offset */
        } else {
            i += 12; /* 12, index offset */
        }
    }

    pcc_diff_cur_mode = pcc_diff_cur_mode * DOUBLE_TO_INT_MULTIPLE / 3; /* 3, mask */
    *p_min_pos = min_pos;

    return (pcc_diff_cur_mode * 10); /* 10, mask */
}

/*************************************************
Function:       ifmd_set_cmd_para_sets_for_each_cmode
Description:    为每种cadence模式设定进行相位判断时的参数。
Calls:          N/A
Called By:      ifmd_check_sigma_vary_cadence()
Input:          当前要设定的cadence模式、需要设定的参数结构体指针。
Output:         设定完毕的参数结构体指针
Return:         无
Others:         该计算仅用于Pulldown
*************************************************/
hi_s32 ifmd_set_cmd_para_sets_for_each_cmode(pq_ifmd_pulldown_mode curr_cadence_mode,
                                             ifmd_32_soft_status_s *sp_ifmd_32_soft_status)
{
    ifmd_cad_mode_para_set_s *p_cmd_para_set = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(sp_ifmd_32_soft_status);

    p_cmd_para_set = &(sp_ifmd_32_soft_status->s_cmd_para_set);
    p_cmd_para_set->period = g_ca_period[curr_cadence_mode];
    p_cmd_para_set->period_tk = g_ca_period_tk[curr_cadence_mode];
    p_cmd_para_set->phases_each_mode = p_cmd_para_set->period / 2; /* 2, half period */
    p_cmd_para_set->num_fod_cases = g_ca_num_fo_cases[curr_cadence_mode];
    p_cmd_para_set->fod_modes = g_ca_fod_modes_mode[curr_cadence_mode];
    p_cmd_para_set->ss_thr_tf = g_ac_ss_thr_tf[curr_cadence_mode];
    p_cmd_para_set->fmv_thr_tf = g_ac_fmv_thr_tf[curr_cadence_mode];

    switch (curr_cadence_mode) {
        case MODE_32:
            p_cmd_para_set->p_sv_cadence_tf = g_sv_cadence_32_tf;
            p_cmd_para_set->p_kf_cadence_tf = g_kf_cadence_32_tf;
            p_cmd_para_set->p_md_cadence_tf = g_md_cadence_32_tf;
            break;

        case MODE_2332:
            p_cmd_para_set->p_sv_cadence_tf = g_sv_cadence_2332_tf;
            p_cmd_para_set->p_kf_cadence_tf = g_kf_cadence_2332_tf;
            p_cmd_para_set->p_md_cadence_tf = g_md_cadence_2332_tf;
            p_cmd_para_set->p_sv_cadence_tbf = g_sv_cadence_2332_tbf;
            p_cmd_para_set->p_kf_cadence_tbf = g_kf_cadence_2332_tbf;
            p_cmd_para_set->p_md_cadence_tbf = g_md_cadence_2332_tbf;
            break;

        case MODE_32322:
            p_cmd_para_set->p_sv_cadence_tf = g_sv_cadence_32322_tf;
            p_cmd_para_set->p_kf_cadence_tf = g_kf_cadence_32322_tf;
            p_cmd_para_set->p_md_cadence_tf = g_md_cadence_32322_tf;
            p_cmd_para_set->p_sv_cadence_tbf = g_sv_cadence_32322_tbf;
            p_cmd_para_set->p_kf_cadence_tbf = g_kf_cadence_32322_tbf;
            p_cmd_para_set->p_md_cadence_tbf = g_md_cadence_32322_tbf;
            break;

        case MODE_11_2_3:
            p_cmd_para_set->p_sv_cadence_tf = g_sv_cadence_11_2_3_tf;
            p_cmd_para_set->p_kf_cadence_tf = g_kf_cadence_11_2_3_tf;
            p_cmd_para_set->p_md_cadence_tf = g_md_cadence_11_2_3_tf;
            break;

        default:
            break;
    }

    p_cmd_para_set->p_md_cadence[MODE_32][0] = g_md_cadence_32_tf;
    p_cmd_para_set->p_md_cadence[MODE_32][1] = g_md_cadence_32_tf;
    p_cmd_para_set->p_md_cadence[MODE_2332][0] = g_md_cadence_2332_tf;
    p_cmd_para_set->p_md_cadence[MODE_2332][1] = g_md_cadence_2332_tbf;
    p_cmd_para_set->p_md_cadence[MODE_32322][0] = g_md_cadence_32322_tf;
    p_cmd_para_set->p_md_cadence[MODE_32322][1] = g_md_cadence_32322_tbf;
    p_cmd_para_set->p_md_cadence[MODE_11_2_3][0] = g_md_cadence_11_2_3_tf;
    p_cmd_para_set->p_md_cadence[MODE_11_2_3][1] = g_md_cadence_11_2_3_tf;

    return HI_SUCCESS;
}

/*************************************************
Function:       ifmd_check_mode_phase
Description:    为每种cadence模式设定进行相位判断时的参数。
Calls:          IFMD_CalcPCCOrig()、ifmd_calc_sigma_pcc_linear_comp_11_2_3()、ifmd_calc_pcc_linear_comp()、IFMD_CalcStillFramePCCLinearComp()等计算各种PCC值的函数
Called By:      ifmd_check_sigma_vary_cadence()
Input:          当前要设定的cadence模式、参数结构体指针、场序号、周期内FMV跳转特征、最大PCC及相位信息记录结构体指针、序列的统计特征。
Output:         最大PCC及相位信息记录
Return:         无
Others:         该计算仅用于Pulldown
*************************************************/
hi_void ifmd_check_mode_phase(pq_ifmd_pulldown_mode curr_cadence_mode, hi_s32 buff_size,
                              ifmd_max_pcc_and_mode_s* ps_max_pcc_and_mode,
                              ifmd_32_soft_status_s* sp_ifmd_32_soft_status)
{
    hi_s32 i;
    hi_s32 fod_mode_for_pulldown = 0;
    hi_s32 tmp0_pcc[3][2] = { { 0, 0 }, { 0, 0 }, { 0, 0 } };  /* [4];//3 //for PCC_types, 2(4) for FODModes */
    hi_s32 tmp1_pcc[3][2] = { { 0, 0 }, { 0, 0 }, { 0, 0 } }; /* 3, 2, index */
    hi_s32 tmp2_pcc[3][2] = { { 0, 0 }, { 0, 0 }, { 0, 0 } }; /* 3, 2, index */
    hi_s32 tmp_pcc[3][2] = { { 0, 0 }, { 0, 0 }, { 0, 0 } };  /* 3, 2, index */
    hi_s32 min_pos_mode_11_2_3;
    hi_s32 max_pcc = MIN_INT;
    hi_s32 max_pcc0 = MIN_INT;
    hi_s32 max_pcc1 = MIN_INT;
    hi_s32 max_pcc2 = MIN_INT;
    ifmd_cad_mode_para_set_s *p_cmd_para_set = HI_NULL;
    pq_ifmd_fod fod = 0;
    ifmd_pcc_calc_pos_s pcc_calc_parm;
    hi_s32 field_counter = sp_ifmd_32_soft_status->field_num;
    hi_s32 sigma_buff_idx;
    hi_s32 sigma_buff_next_idx;
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    PQ_CHECK_ZERO_RE_NULL(buff_size);
    PQ_CHECK_NULL_PTR_RE_NULL(pst_vir_soft_reg);
    sigma_buff_idx = field_counter % buff_size;
    sigma_buff_next_idx = (field_counter + 1) % buff_size;
    p_cmd_para_set = &(sp_ifmd_32_soft_status->s_cmd_para_set);

    if (curr_cadence_mode == MODE_11_2_3) {
        if (sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[sigma_buff_idx] > 8000 && /* 8000, thre */
            sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[sigma_buff_next_idx] > 8000) { /* 8000, thr */
            sp_ifmd_32_soft_status->mask_1123_cnt = 20;   /* 20, limit value */
        } else {
            sp_ifmd_32_soft_status->mask_1123_cnt = MAX2(sp_ifmd_32_soft_status->mask_1123_cnt - 1, 0);
        }
    }

    /* 各模式相位的sigma_vary相位相关性计算、
    所有模式相位的MMM和StillFrame相位相关性计算 */
    for (fod_mode_for_pulldown = 0; fod_mode_for_pulldown < 2; fod_mode_for_pulldown++) { /* 2, phase period */
        fod = p_cmd_para_set->fod_modes[fod_mode_for_pulldown];

        switch (fod) {
            case NONE:  // 0:
                break;

            case TF:
                for (i = 0; i < p_cmd_para_set->phases_each_mode; i++) {
                    if (curr_cadence_mode != MODE_11_2_3) {  // 11_2_3模式与其他模式使用不同的检测方式。
                        p_cmd_para_set->period = MAX2(1, p_cmd_para_set->period);
                        pcc_calc_parm.ref_start_pos =
                            (field_counter + 1 + i * 2) % p_cmd_para_set->period; /* 2, period */
                        pcc_calc_parm.to_be_com_start_pos =
                            (sigma_buff_idx - p_cmd_para_set->period - 1 + buff_size) % buff_size;
                        pcc_calc_parm.length_ref = p_cmd_para_set->period;
                        pcc_calc_parm.length_to_be_comp = buff_size;
                        tmp0_pcc[PCC_KF][0] = ifmd_calc_pcc_linear_comp(p_cmd_para_set->p_kf_cadence_tf,
                            sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary,
                            sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sc_counter, pcc_calc_parm);

                        tmp1_pcc[PCC_KF][0] = ifmd_calc_pcc_linear_comp(p_cmd_para_set->p_kf_cadence_tf,
                            sp_ifmd_32_soft_status->sp_frame_diff_characters.el_vary,
                            sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sc_counter, pcc_calc_parm);

                        tmp2_pcc[PCC_KF][0] = ifmd_calc_pcc_linear_comp(p_cmd_para_set->p_kf_cadence_tf,
                            sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary_32,
                            sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sc_counter, pcc_calc_parm);

                        if (curr_cadence_mode == MODE_32 && (sp_ifmd_32_soft_status->music_flag ||
                                                             sp_ifmd_32_soft_status->music_menu_flag)) {
                            if (tmp0_pcc[PCC_KF][0] > max_pcc0) {
                                max_pcc0 = tmp0_pcc[PCC_KF][0];
                                ps_max_pcc_and_mode->max_pcc = tmp0_pcc[PCC_KF][0];
                                ps_max_pcc_and_mode->max_pcc_mode = 1;
                            }
                            if (tmp1_pcc[PCC_KF][0] > max_pcc1) {
                                max_pcc1 = tmp1_pcc[PCC_KF][0];
                                ps_max_pcc_and_mode->max_pcc = tmp1_pcc[PCC_KF][0];
                                ps_max_pcc_and_mode->max_pcc_mode = 1;
                            }
                            if (tmp2_pcc[PCC_KF][0] > max_pcc2) {
                                max_pcc2 = tmp2_pcc[PCC_KF][0];
                                ps_max_pcc_and_mode->max_pcc = tmp2_pcc[PCC_KF][0];
                                ps_max_pcc_and_mode->max_pcc_mode = 1;
                            }
                        } else {
                            if (tmp0_pcc[PCC_KF][0] > max_pcc0) {
                                max_pcc0 = tmp0_pcc[PCC_KF][0];
                                ps_max_pcc_and_mode->max_pcc = tmp0_pcc[PCC_KF][0];
                                ps_max_pcc_and_mode->max_pcc_mode = 1;
                            }
                        }
                    } else {
                        tmp_pcc[PCC_SIGMA_VARY][TF - 1] = ifmd_calc_sigma_pcc_linear_comp_11_2_3(
			    sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma,
			    &min_pos_mode_11_2_3, field_counter - p_cmd_para_set->period, buff_size, i);

                        // big comb>8000
                        if (tmp_pcc[PCC_SIGMA_VARY][0] > max_pcc && sp_ifmd_32_soft_status->mask_1123_cnt == 0) {
                            max_pcc = tmp_pcc[PCC_SIGMA_VARY][0];
                            ps_max_pcc_and_mode->max_pcc = tmp_pcc[PCC_SIGMA_VARY][0];
                            ps_max_pcc_and_mode->max_pcc_mode = 1;
                        }
                    }
                }

                if (curr_cadence_mode == MODE_32 && (sp_ifmd_32_soft_status->music_flag ||
                                                     sp_ifmd_32_soft_status->music_menu_flag)) {
                    max_pcc = MAX2(MAX2(max_pcc0, max_pcc1), max_pcc2);
                    ps_max_pcc_and_mode->max_pcc = max_pcc;

                    if (max_pcc == max_pcc1 && sp_ifmd_32_soft_status->el_32_mask == 0) {
                        sp_ifmd_32_soft_status->el_32_confirm = 1;
                        sp_ifmd_32_soft_status->cv_2_confirm = 0;
                        sp_ifmd_32_soft_status->el_32_mask = MAX2(sp_ifmd_32_soft_status->el_32_mask - 1, 0);
                    } else if (max_pcc == max_pcc2) {
                        sp_ifmd_32_soft_status->el_32_confirm = 0;
                        sp_ifmd_32_soft_status->cv_2_confirm = 1;
                        sp_ifmd_32_soft_status->el_32_mask = MAX2(sp_ifmd_32_soft_status->el_32_mask - 1, 0);
                    } else if (max_pcc == max_pcc0) {
                        sp_ifmd_32_soft_status->el_32_confirm = 0;
                        sp_ifmd_32_soft_status->cv_2_confirm = 0;
                        sp_ifmd_32_soft_status->el_32_mask = MIN2(sp_ifmd_32_soft_status->el_32_mask + 1,
                                                                  20); /* 20, limit */
                    } else {
                        sp_ifmd_32_soft_status->el_32_confirm = 0;
                        sp_ifmd_32_soft_status->cv_2_confirm = 0;
                        sp_ifmd_32_soft_status->el_32_mask = MAX2(sp_ifmd_32_soft_status->el_32_mask - 1, 0);
                    }
                }

                if (pst_vir_soft_reg->ifmd_reg.ifmd_debug11_2_3_en[0]) {
                    if (curr_cadence_mode == MODE_11_2_3) {
                        HI_ERR_PQ("PCC:%d,Phase:%d,", ps_max_pcc_and_mode->max_pcc, ps_max_pcc_and_mode->max_pcc_mode);
                    }
                }
                break;

            case TBF:
                for (i = 0; i < p_cmd_para_set->phases_each_mode; i++) {
                    p_cmd_para_set->period = MAX2(1, p_cmd_para_set->period);
                    pcc_calc_parm.ref_start_pos = (field_counter + 1 + i * 2) % p_cmd_para_set->period; /* 2, period */
                    pcc_calc_parm.to_be_com_start_pos =
                        (sigma_buff_idx - p_cmd_para_set->period - 1 + buff_size) % buff_size;
                    pcc_calc_parm.length_ref = p_cmd_para_set->period;
                    pcc_calc_parm.length_to_be_comp = buff_size;
                    tmp0_pcc[PCC_KF][1] = ifmd_calc_pcc_linear_comp(p_cmd_para_set->p_kf_cadence_tbf,
                        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary,
                        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sc_counter, pcc_calc_parm);
                    tmp1_pcc[PCC_KF][1] = ifmd_calc_pcc_linear_comp(p_cmd_para_set->p_kf_cadence_tbf,
                        sp_ifmd_32_soft_status->sp_frame_diff_characters.el_vary,
                        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sc_counter, pcc_calc_parm);
                    tmp2_pcc[PCC_KF][1] = ifmd_calc_pcc_linear_comp(p_cmd_para_set->p_kf_cadence_tbf,
                        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary_32,
                        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sc_counter, pcc_calc_parm);

                    if (curr_cadence_mode == MODE_32 && (sp_ifmd_32_soft_status->music_flag ||
                                                         sp_ifmd_32_soft_status->music_menu_flag)) {
                        if (tmp0_pcc[PCC_KF][1] > max_pcc0) {
                            max_pcc0 = tmp0_pcc[PCC_KF][1];
                            ps_max_pcc_and_mode->max_pcc = tmp0_pcc[PCC_KF][1];
                            ps_max_pcc_and_mode->max_pcc_mode = 1;
                        }
                        if (tmp1_pcc[PCC_KF][1] > max_pcc1) {
                            max_pcc1 = tmp1_pcc[PCC_KF][1];
                            ps_max_pcc_and_mode->max_pcc = tmp1_pcc[PCC_KF][1];
                            ps_max_pcc_and_mode->max_pcc_mode = 1;
                        }
                        if (tmp2_pcc[PCC_KF][1] > max_pcc2) {
                            max_pcc2 = tmp2_pcc[PCC_KF][1];
                            ps_max_pcc_and_mode->max_pcc = tmp2_pcc[PCC_KF][1];
                            ps_max_pcc_and_mode->max_pcc_mode = 1;
                        }
                    } else {
                        if (tmp0_pcc[PCC_KF][1] > max_pcc0) {
                            max_pcc0 = tmp0_pcc[PCC_KF][1];
                            ps_max_pcc_and_mode->max_pcc = tmp0_pcc[PCC_KF][1];
                            ps_max_pcc_and_mode->max_pcc_mode = 1;
                        }
                    }
                }

                if (curr_cadence_mode == MODE_32 && (sp_ifmd_32_soft_status->music_flag ||
                                                     sp_ifmd_32_soft_status->music_menu_flag)) {
                    max_pcc = MAX2(MAX2(max_pcc0, max_pcc1), max_pcc2);
                    ps_max_pcc_and_mode->max_pcc = max_pcc;

                    if (max_pcc == max_pcc1 && sp_ifmd_32_soft_status->el_32_mask == 0) {
                        sp_ifmd_32_soft_status->el_32_confirm = 1;
                        sp_ifmd_32_soft_status->cv_2_confirm = 0;
                        sp_ifmd_32_soft_status->el_32_mask = MAX2(sp_ifmd_32_soft_status->el_32_mask - 1, 0);
                    } else if (max_pcc == max_pcc2) {
                        sp_ifmd_32_soft_status->el_32_confirm = 0;
                        sp_ifmd_32_soft_status->cv_2_confirm = 1;
                        sp_ifmd_32_soft_status->el_32_mask = MAX2(sp_ifmd_32_soft_status->el_32_mask - 1, 0);
                    } else if (max_pcc == max_pcc0) {
                        sp_ifmd_32_soft_status->el_32_confirm = 0;
                        sp_ifmd_32_soft_status->cv_2_confirm = 0;
                        sp_ifmd_32_soft_status->el_32_mask = MIN2(sp_ifmd_32_soft_status->el_32_mask + 1,
                                                                  20); /* 20, limit */
                    } else {
                        sp_ifmd_32_soft_status->el_32_confirm = 0;
                        sp_ifmd_32_soft_status->cv_2_confirm = 0;
                        sp_ifmd_32_soft_status->el_32_mask = MAX2(sp_ifmd_32_soft_status->el_32_mask - 1, 0);
                    }
                }
                break;

            default:
                break;
        }
    }

    return;
}

hi_void ifmd_get_4min_value_and_pos(pq_ifmd_subtitle *fmd_subtitle,
                                    hi_s32 range, hi_s32 buff_size)
{
    hi_s32 i;

    for (i = 0; i < range; i++) {
        if (fmd_subtitle->p_sigma[(fmd_subtitle->sigma_alion_pos + buff_size - i) % buff_size] <= fmd_subtitle->min_sigma[0]) {
            fmd_subtitle->min_sigma[3] = fmd_subtitle->min_sigma[2];         /* 2, 3 index */
            fmd_subtitle->min_sigma_pos[3] = fmd_subtitle->min_sigma_pos[2]; /* 2, 3 index */
            fmd_subtitle->min_sigma[2] = fmd_subtitle->min_sigma[1];         /* 2, 1 index */
            fmd_subtitle->min_sigma_pos[2] = fmd_subtitle->min_sigma_pos[1]; /* 2, 1 index */
            fmd_subtitle->min_sigma[1] = fmd_subtitle->min_sigma[0];
            fmd_subtitle->min_sigma_pos[1] = fmd_subtitle->min_sigma_pos[0];
            fmd_subtitle->min_sigma[0] = fmd_subtitle->p_sigma[(fmd_subtitle->sigma_alion_pos + buff_size - i) % buff_size];
            fmd_subtitle->min_sigma_pos[0] = i;
        } else if (fmd_subtitle->p_sigma[(fmd_subtitle->sigma_alion_pos + buff_size - i) % buff_size] <= fmd_subtitle->min_sigma[1]) {
            fmd_subtitle->min_sigma[3] = fmd_subtitle->min_sigma[2];         /* 3, 2 index */
            fmd_subtitle->min_sigma_pos[3] = fmd_subtitle->min_sigma_pos[2]; /* 3, 2 index */
            fmd_subtitle->min_sigma[2] = fmd_subtitle->min_sigma[1];         /* 2, 1 index */
            fmd_subtitle->min_sigma_pos[2] = fmd_subtitle->min_sigma_pos[1]; /* 2, 1 index */
            fmd_subtitle->min_sigma[1] = fmd_subtitle->p_sigma[(fmd_subtitle->sigma_alion_pos + buff_size - i) % buff_size];
            fmd_subtitle->min_sigma_pos[1] = i;
        } else if (fmd_subtitle->p_sigma[(fmd_subtitle->sigma_alion_pos + buff_size - i) % buff_size] <= fmd_subtitle->min_sigma[2]) { /* 2, index */
            fmd_subtitle->min_sigma[3] = fmd_subtitle->min_sigma[2];                                                     /* 2, 3 index */
            fmd_subtitle->min_sigma_pos[3] = fmd_subtitle->min_sigma_pos[2];                                             /* 2, 3 index */
            fmd_subtitle->min_sigma[2] = fmd_subtitle->p_sigma[(fmd_subtitle->sigma_alion_pos + buff_size - i) % buff_size];           /* 2 index */
            fmd_subtitle->min_sigma_pos[2] = i;                                                            /* 2 index */
        } else if (fmd_subtitle->p_sigma[(fmd_subtitle->sigma_alion_pos + buff_size - i) % buff_size] < fmd_subtitle->min_sigma[3]) {  /* 3 index */
            fmd_subtitle->min_sigma[3] = fmd_subtitle->p_sigma[(fmd_subtitle->sigma_alion_pos + buff_size - i) % buff_size];           /* 3 index */
            fmd_subtitle->min_sigma_pos[3] = i;                                                   /* 3 index */
        } else {
            continue;
        }
    }
}

/*************************************************
Function:       IFMD_CheckSubtitle_Ifmd
Description:    为每种cadence模式查看最近的周期内有无字幕出现（静态或滚动）。
Calls:          N/A
Called By:      ifmd_check_sigma_vary_cadence()
Input:          当前的cadence模式，序列的周期性特征，序列的统计特征，当前场序号。
Output:         最近周期内的字幕情况：有滚动字幕、     有静态字幕或者滚动字幕的开始（配合将当前处理场是否为有字幕场进行记录）、无字幕检出。
Return:         最近周期内的字幕情况：有滚动字幕返回2，有静态字幕或者滚动字幕的开始返回1，无字幕检出返回0（0也可能是静态字幕稳定期的表现）。
Others:         该计算仅用于Pulldown,获取的是最新场（第四场）是否有字幕
*************************************************/
hi_s32 ifmd_check_subtitle(pq_ifmd_pulldown_mode curr_cadence_mode, hi_s32 buff_size,
                           ifmd_32_soft_status_s *sp_ifmd_32_soft_status)
{
    pq_ifmd_subtitle fmd_subtitle = {0};
    hi_s32 monotony_tk;
    ifmd_cad_mode_para_set_s *sp_cmd_para_set = HI_NULL;
    hi_s32 curr_align_offset = 0;
    hi_s32 b_start_flag;
    hi_s32 i;
    hi_s32 monotony_tk_tmp = 0;
    hi_u32 index0 = 0;
    hi_u32 index1 = 0;
    hi_u32 index2 = 0;
    hi_u32 index3 = 0;

    PQ_CHECK_ZERO_RE_FAIL(buff_size);
    fmd_subtitle.sigma_alion_pos = (sp_ifmd_32_soft_status->field_num + 1) % buff_size;
    fmd_subtitle.p_sigma = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma;
    sp_cmd_para_set = &(sp_ifmd_32_soft_status->s_cmd_para_set);

    ifmd_get_4min_value_and_pos(&fmd_subtitle, sp_cmd_para_set->period_tk, buff_size);

    if (curr_cadence_mode == MODE_32) {
        for (i = 0; i < 10; i++) { /* 10 index */
            index0 = (fmd_subtitle.sigma_alion_pos + buff_size - i) % buff_size;
            monotony_tk_tmp += sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma_non_monotony_32[index0];
        }
        monotony_tk = monotony_tk_tmp - 4; /* 4, step */
    } else if (curr_cadence_mode == MODE_32322) {
        for (i = 0; i < 12; i++) { /* 12 index */
            index0 = (fmd_subtitle.sigma_alion_pos + buff_size - i) % buff_size;
            monotony_tk_tmp += sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma_non_monotony_32322[index0];
        }

        monotony_tk = monotony_tk_tmp - 5; /* 5 threshold */
    } else if (curr_cadence_mode == MODE_2332) {
        for (i = 0; i < 10; i++) { /* 10 index */
            index0 = (fmd_subtitle.sigma_alion_pos + buff_size - i) % buff_size;
            monotony_tk_tmp += sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma_non_monotony_2332[index0];
        }

        monotony_tk = monotony_tk_tmp - 4; /* 4 threshold */
    } else {
        index0 = (fmd_subtitle.sigma_alion_pos + buff_size - fmd_subtitle.min_sigma_pos[0]) % buff_size;
        index1 = (fmd_subtitle.sigma_alion_pos + buff_size - fmd_subtitle.min_sigma_pos[1]) % buff_size;
        index2 = (fmd_subtitle.sigma_alion_pos + buff_size - fmd_subtitle.min_sigma_pos[2]) % buff_size; /* 2, index */
        index3 = (fmd_subtitle.sigma_alion_pos + buff_size - fmd_subtitle.min_sigma_pos[3]) % buff_size; /* 3, index */
        monotony_tk = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma_non_monotony[index0] +
        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma_non_monotony[index1] +
        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma_non_monotony[index2] +
        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma_non_monotony[index3];
    }

    b_start_flag = (fmd_subtitle.min_sigma_pos[0] == curr_align_offset || fmd_subtitle.min_sigma_pos[1] == curr_align_offset ||
                    fmd_subtitle.min_sigma_pos[2] == curr_align_offset || fmd_subtitle.min_sigma_pos[3] == curr_align_offset) && /* 2, 3, index */
                   sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma_non_monotony[(fmd_subtitle.sigma_alion_pos -
                           curr_align_offset + buff_size) % buff_size];

    /* 对当前是静态字幕的起始做一个标记, 以便对相应场单独做deinterlace */
    if (monotony_tk == 1) {
        sp_ifmd_32_soft_status->s_still_or_rolling_subtitle[curr_cadence_mode].is_still_or_rolling_subtitle = 1;

        if (b_start_flag) {
            sp_ifmd_32_soft_status->s_still_or_rolling_subtitle[curr_cadence_mode].b_start = 1;
        } else {
            sp_ifmd_32_soft_status->s_still_or_rolling_subtitle[curr_cadence_mode].b_start = 0;
        }
    } else if (monotony_tk >= 2) {  /* 当前是动态字幕, 2, threshold */
        sp_ifmd_32_soft_status->s_still_or_rolling_subtitle[curr_cadence_mode].is_still_or_rolling_subtitle =
            monotony_tk;

        if (b_start_flag) {
            sp_ifmd_32_soft_status->s_still_or_rolling_subtitle[curr_cadence_mode].b_start = 1;
        }
    } else {
        sp_ifmd_32_soft_status->s_still_or_rolling_subtitle[curr_cadence_mode].is_still_or_rolling_subtitle = 0;
        sp_ifmd_32_soft_status->s_still_or_rolling_subtitle[curr_cadence_mode].b_start = 0;
    }

    return sp_ifmd_32_soft_status->s_still_or_rolling_subtitle[curr_cadence_mode].is_still_or_rolling_subtitle;
}

/*************************************************
Function:        ifmd_check_sigma_vary_cadence
Description:    为每种cadence模式设定进行相位判断时的参数。
Calls:              ifmd_set_cmd_para_sets_for_each_cmode(), ifmd_check_mode_phase()
Called By:       各种cadence的detector()
Input:             当前的cadence模式，序列的统计特征，当前帧序号、场极性信息，当前cadence的模式判断状态信息（相位模式计数器等）。
Output:           最匹配相位以及匹配度值，当前cadence的模式判断状态信息也会相应改变（相位模式计数器等）。
Return:           无
Others:           该计算仅用于Pulldown
*************************************************/
hi_s32 ifmd_check_sigma_vary_cadence(pq_ifmd_pulldown_mode curr_cadence_mode,
     ifmd_32_soft_status_s *sp_ifmd_32_soft_status, ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    ifmd_max_pcc_and_mode_s s_max_pcc_and_mode;
    ifmd_cad_mode_para_set_s *p_cmd_para_set = HI_NULL;
    hi_s32 field_num;
    hi_s32 buff_size;
    hi_s32 is_still_or_rolling_subtitle = 0;
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();
    hi_s32 s32_ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(pst_vir_soft_reg);
    s_max_pcc_and_mode.max_pcc = MIN_INT;
    s_max_pcc_and_mode.max_pcc_mode = 0;

    buff_size = sp_ifmd_32_soft_ctrl->ifmd_det_buff_size;
    field_num = sp_ifmd_32_soft_status->field_num;

    s32_ret = ifmd_set_cmd_para_sets_for_each_cmode(curr_cadence_mode, sp_ifmd_32_soft_status);
    if (s32_ret != HI_SUCCESS) {
        HI_ERR_PQ("\n   ifmd_get_2max_value failed!\n");
        return HI_FAILURE;
    }

    p_cmd_para_set = &(sp_ifmd_32_soft_status->s_cmd_para_set);
    p_cmd_para_set->pcc_thre_ori = PCC_THRE_COMB_CADENCE;

    // only after the difference of the first (period) fields have been calculated, the detection can begin
    if (field_num > p_cmd_para_set->period) {
        ifmd_check_mode_phase(curr_cadence_mode, buff_size, &s_max_pcc_and_mode, sp_ifmd_32_soft_status);

        if (field_num >= p_cmd_para_set->period_tk) {
            is_still_or_rolling_subtitle = ifmd_check_subtitle(curr_cadence_mode, buff_size, sp_ifmd_32_soft_status);
        }

        // mode of subtile differs from the pulldown mode of background!
        if (is_still_or_rolling_subtitle >= 2 || sp_ifmd_32_soft_status->text_cnt > 0) { /* 2, threshold */
            s_max_pcc_and_mode.max_pcc = 0;
            sp_ifmd_32_soft_status->flag_adjust = 1;
        }

        if (curr_cadence_mode == MODE_32) {
            if (is_still_or_rolling_subtitle >= 2) {                                                /* 2, threshold */
                sp_ifmd_32_soft_status->text_cnt = MIN2(sp_ifmd_32_soft_status->text_cnt + 1, 100); /* 100, lmt value */
            } else {
                sp_ifmd_32_soft_status->text_cnt = MAX2(sp_ifmd_32_soft_status->text_cnt - 1, 0);
            }
        }

        if (pst_vir_soft_reg->ifmd_reg.ifmd_debug32_en[1]) {
            if (curr_cadence_mode == MODE_32) {
                HI_ERR_PQ("%4d,%d,", s_max_pcc_and_mode.max_pcc, sp_ifmd_32_soft_status->text_cnt);
            }
        }
        if (pst_vir_soft_reg->ifmd_reg.ifmd_debug2332_en[0]) {
            if (curr_cadence_mode == MODE_2332) {
                HI_ERR_PQ("mpcc:%4d, st:%d, ", s_max_pcc_and_mode.max_pcc, is_still_or_rolling_subtitle);
            }
        }
        if (pst_vir_soft_reg->ifmd_reg.ifmd_debug32322_en[0]) {
            if (curr_cadence_mode == MODE_32322) {
                HI_ERR_PQ("mpcc:%4d, st:%d, ", s_max_pcc_and_mode.max_pcc, is_still_or_rolling_subtitle);
            }
        }

        if (s_max_pcc_and_mode.max_pcc > p_cmd_para_set->pcc_thre_ori) {
            // 当前判为s_max_pcc_and_mode.max_pcc所对应模式，调整各计数器
            ifmd_adjust_mode_counters(curr_cadence_mode, sp_ifmd_32_soft_status, s_max_pcc_and_mode.max_pcc_mode);

            sp_ifmd_32_soft_status->last_trend[curr_cadence_mode] = (s_max_pcc_and_mode.max_pcc_mode);
            sp_ifmd_32_soft_status->flag_adjust = 0;

        } else {
            // 当前不判为当前检测cadence的任何相位模式，调整各计数器
            ifmd_adjust_mode_counters(curr_cadence_mode, sp_ifmd_32_soft_status, MODE_NONE);

            sp_ifmd_32_soft_status->last_trend[curr_cadence_mode] = MODE_NONE;
            sp_ifmd_32_soft_status->flag_adjust = 0;
            sp_ifmd_32_soft_status->s_still_or_rolling_subtitle[curr_cadence_mode].b_start = 1;
        }

        if (pst_vir_soft_reg->ifmd_reg.ifmd_debug32_en[2]) { /* 2  index */
            if (curr_cadence_mode == MODE_32) {
                HI_ERR_PQ("pccm:%d, mcnt:%3d, ", s_max_pcc_and_mode.max_pcc_mode,
                          sp_ifmd_32_soft_status->mode_counters[MODE_32][s_max_pcc_and_mode.max_pcc_mode]);
            }
        }
        if (pst_vir_soft_reg->ifmd_reg.ifmd_debug2332_en[1]) {
            if (curr_cadence_mode == MODE_2332) {
                HI_ERR_PQ("pccm:%d, mcnt:%3d, ", s_max_pcc_and_mode.max_pcc_mode,
                          sp_ifmd_32_soft_status->mode_counters[MODE_2332][s_max_pcc_and_mode.max_pcc_mode]);
            }
        }
        if (pst_vir_soft_reg->ifmd_reg.ifmd_debug32322_en[1]) {
            if (curr_cadence_mode == MODE_32322) {
                HI_ERR_PQ("pccm:%d, mcnt:%3d, ", s_max_pcc_and_mode.max_pcc_mode,
                          sp_ifmd_32_soft_status->mode_counters[MODE_32322][s_max_pcc_and_mode.max_pcc_mode]);
            }
        }
    }

    return s_max_pcc_and_mode.max_pcc;
}

/*************************************************
Functions:      为每种已知cadence模式检测最匹配相位。
Description:    为每种已知cadence模式检测最多四种场序下的最匹配相位。最匹配相位可能是某种相位，也可能是0（表示没有特别匹配的相位）。
Calls:
Called By:
Input:          序列的统计特征，当前帧序号、场极性信息。
Output:         最匹配相位以及匹配度值。
Return:
Others:         该计算仅用于Pulldown
*************************************************/
/*************************************************
Function:       PulldownDetection32CadenceMode
Description:    检测当前序列是否有可能是32Pulldown（TForBF），若是，给出其相位信息和PCC值。
Calls:          CheckSigmaVaryCadence(), LockerScheme()
Called By:      DetectPulldownModePhase()
Input:          当前检测帧序号、场相位，场景切换信息，以及缓存序列特征的缓冲大小
Output:         最匹配相位信息，以及最匹配相位的PCC值
Return:         最匹配相位信息1~5: mode32的TF1~5, 6~10: mode32的BF1~5，0: not mode32 or not locked
Others:         该计算仅用于Pulldown
*************************************************/
hi_s32 ifmd_pulldown_detection_32_cadence_mode(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                                               ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_s32 total_phases = 10;
    hi_s32 curr_cadence_mode = MODE_32;
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(pst_vir_soft_reg);
    sp_ifmd_32_soft_status->d_max_pcc_per_mode[curr_cadence_mode] =
    (hi_u16)ifmd_check_sigma_vary_cadence(curr_cadence_mode, sp_ifmd_32_soft_status, sp_ifmd_32_soft_ctrl);

    // lock scheme
    sp_ifmd_32_soft_status->mode_locked[curr_cadence_mode] =
    ifmd_locker_scheme(curr_cadence_mode, sp_ifmd_32_soft_status, total_phases + 1, sp_ifmd_32_soft_ctrl);

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug32_en[2]) { /* 2  index */
        HI_ERR_PQ("UC=%d,ML=%d,", sp_ifmd_32_soft_status->unlock_counter[curr_cadence_mode],
                  sp_ifmd_32_soft_status->mode_locked[curr_cadence_mode]);
    }

    return sp_ifmd_32_soft_status->mode_locked[curr_cadence_mode];
}

/*************************************************
Function:       ifmd_pulldown_detection_2332_cadence_mode
Description:    检测当前序列是否有可能是2332Pulldown（TF、BF、BTF或TBF），若是，给出其相位信息和PCC值。
Calls:          ifmd_check_sigma_vary_cadence(), ifmd_locker_scheme()
Called By:      ifmd_detect_pulldown_mode_phase()
Input:          当前检测帧序号、场相位，场景切换信息，以及缓存序列特征的缓冲大小
Output:         最匹配相位信息，以及最匹配相位的PCC值
Return:         最匹配相位信息1~5: mode2332的TF1~5, 6~10: mode2332的BF1~5，
11~15:mode2332的BTF1~5，15~20: mode2332的TBF1~5，0: not mode2332 or not locked
Others:         该计算仅用于Pulldown
*************************************************/
hi_s32 ifmd_pulldown_detection_2332_cadence_mode(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                                                 ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_s32 total_phases = 20;
    hi_s32 curr_cadence_mode = MODE_2332;
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(pst_vir_soft_reg);
    sp_ifmd_32_soft_status->d_max_pcc_per_mode[curr_cadence_mode] =
    (hi_u16)ifmd_check_sigma_vary_cadence(curr_cadence_mode, sp_ifmd_32_soft_status, sp_ifmd_32_soft_ctrl);

    /* lock scheme */
    sp_ifmd_32_soft_status->mode_locked[curr_cadence_mode] =
    ifmd_locker_scheme(curr_cadence_mode, sp_ifmd_32_soft_status, total_phases + 1, sp_ifmd_32_soft_ctrl);

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug2332_en[1]) {
        HI_ERR_PQ("UC=%d,ML=%d,", sp_ifmd_32_soft_status->unlock_counter[curr_cadence_mode],
                  sp_ifmd_32_soft_status->mode_locked[curr_cadence_mode]);
    }
    return sp_ifmd_32_soft_status->mode_locked[curr_cadence_mode];
}

/*************************************************
Function:       ifmd_pulldown_detection_32322_cadence_mode
Description:    检测当前序列是否有可能是32322Pulldown（TF、BF、BTF或TBF），若是，给出其相位信息和PCC值。
Calls:          ifmd_check_sigma_vary_cadence(), ifmd_locker_scheme()
Input:          当前检测帧序号、场相位，场景切换信息，以及缓存序列特征的缓冲大小
Output:         最匹配相位信息，以及最匹配相位的PCC值
Return:         最匹配相位信息1~6: mode32322的TF1~6, 7~12: mode32322的BF1~6，
13~18:mode32322的BTF1~6，19~24: mode32322的TBF1~6，0: not mode32322 or not locked
Others:         该计算仅用于Pulldown
*************************************************/
hi_s32 ifmd_pulldown_detection_32322_cadence_mode(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                                                  ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_s32 total_phases = 24;
    hi_s32 curr_cadence_mode = MODE_32322;
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(pst_vir_soft_reg);
    sp_ifmd_32_soft_status->d_max_pcc_per_mode[curr_cadence_mode] =
    (hi_u16)ifmd_check_sigma_vary_cadence(curr_cadence_mode, sp_ifmd_32_soft_status, sp_ifmd_32_soft_ctrl);

    /* lock scheme */
    sp_ifmd_32_soft_status->mode_locked[curr_cadence_mode] = ifmd_locker_scheme(curr_cadence_mode,
        sp_ifmd_32_soft_status, total_phases + 1, sp_ifmd_32_soft_ctrl);

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug32322_en[1]) {
        HI_ERR_PQ("UC=%d,ML=%d,", sp_ifmd_32_soft_status->unlock_counter[curr_cadence_mode],
                  sp_ifmd_32_soft_status->mode_locked[curr_cadence_mode]);
    }
    return sp_ifmd_32_soft_status->mode_locked[curr_cadence_mode];
}

/*************************************************
Function:       PulldownDetection11_2_3CadenceMode
Description:    检测当前序列是否有可能是11_2_3Pulldown（TForBF），若是，给出其相位信息和PCC值。
Calls:          CheckSigmaVaryCadence(), LockerScheme()
Called By:      DetectPulldownModePhase()
Input:          当前检测帧序号、场相位，场景切换信息，以及缓存序列特征的缓冲大小
Output:         最匹配相位信息，以及最匹配相位的PCC值
Return:         最匹配相位信息1~25: mode11_2_3的TF1~25, 26~50: mode11_2_3的BF1~25，0: not mode11_2_3 or not locked
Others:         该计算仅用于Pulldown
*************************************************/
hi_s32 ifmd_pulldown_detection_11_2_3_cadence_mode(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                                                   ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_s32 total_phases = 50;
    hi_s32 curr_cadence_mode = MODE_11_2_3;
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(pst_vir_soft_reg);
    sp_ifmd_32_soft_status->d_max_pcc_per_mode[curr_cadence_mode] =
    (hi_u16)ifmd_check_sigma_vary_cadence(curr_cadence_mode, sp_ifmd_32_soft_status, sp_ifmd_32_soft_ctrl);

    /* lock scheme */
    sp_ifmd_32_soft_status->mode_locked[curr_cadence_mode] = ifmd_locker_scheme(curr_cadence_mode,
        sp_ifmd_32_soft_status, total_phases + 1, sp_ifmd_32_soft_ctrl);

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug11_2_3_en[0]) {
        HI_ERR_PQ("MPcc=%d,UC=%d,ML=%d,", sp_ifmd_32_soft_status->d_max_pcc_per_mode[curr_cadence_mode],
                  sp_ifmd_32_soft_status->unlock_counter[curr_cadence_mode],
                  sp_ifmd_32_soft_status->mode_locked[curr_cadence_mode]);
    }

    return sp_ifmd_32_soft_status->mode_locked[curr_cadence_mode];
}

hi_s8 ifmd_detect_om_mode_phase(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                                ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_s32 ret = 0;
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    if (sp_ifmd_32_soft_status->sp_frame_diff_characters.kf_cnt_btw_period == 2 || /* 2, threshold */
        sp_ifmd_32_soft_status->sp_frame_diff_characters.kf_cnt_btw_period == 1) {
        sp_ifmd_32_soft_status->om_lock_counter = ifmd_round(sp_ifmd_32_soft_status->om_lock_counter + 1, MODE_UP_BOUND,
                                                             MODE_DOWN_BOUND);
        sp_ifmd_32_soft_status->om_unlock_counter = ifmd_round(sp_ifmd_32_soft_status->om_unlock_counter - 1,
                                                               MODE_UP_BOUND, MODE_DOWN_BOUND);
    } else {
        sp_ifmd_32_soft_status->om_unlock_counter = ifmd_round(sp_ifmd_32_soft_status->om_unlock_counter + 1,
                                                               MODE_UP_BOUND, MODE_DOWN_BOUND);
        sp_ifmd_32_soft_status->om_lock_counter = ifmd_round(sp_ifmd_32_soft_status->om_lock_counter - 1, MODE_UP_BOUND,
                                                             MODE_DOWN_BOUND);
    }

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug55_64_87_en[0]) {
        HI_ERR_PQ("OMCnt=%2d,OMUCnt=%2d, ", sp_ifmd_32_soft_status->om_lock_counter,
                  sp_ifmd_32_soft_status->om_unlock_counter);
    }

    if (sp_ifmd_32_soft_status->sp_frame_diff_characters.other_mode_cv_cnt >= 4) { /* 4, threshold */
        sp_ifmd_32_soft_status->other_mode_lock_counter =
            ifmd_round(sp_ifmd_32_soft_status->other_mode_lock_counter + 1, 20, 0);
        sp_ifmd_32_soft_status->other_mode_unlock_counter =
            ifmd_round(sp_ifmd_32_soft_status->other_mode_unlock_counter - 1, 20, 0);
    } else {
        sp_ifmd_32_soft_status->other_mode_unlock_counter =
            ifmd_round(sp_ifmd_32_soft_status->other_mode_unlock_counter + 1, 20, 0);
        sp_ifmd_32_soft_status->other_mode_lock_counter =
            ifmd_round(sp_ifmd_32_soft_status->other_mode_lock_counter - 1, 20, 0);
    }

    if (sp_ifmd_32_soft_status->sp_frame_diff_characters.big_small_cnt_btw_period >= 4) { /* 4, threshold */
        sp_ifmd_32_soft_status->om_11_2_3_lock_counter =
        ifmd_round(sp_ifmd_32_soft_status->om_11_2_3_lock_counter + 1,
        MODE_UP_BOUND, MODE_DOWN_BOUND);
        sp_ifmd_32_soft_status->om_11_2_3_unlock_counter =
        ifmd_round(sp_ifmd_32_soft_status->om_11_2_3_unlock_counter - 1,
        MODE_UP_BOUND, MODE_DOWN_BOUND);
    } else {
        sp_ifmd_32_soft_status->om_11_2_3_unlock_counter =
        ifmd_round(sp_ifmd_32_soft_status->om_11_2_3_unlock_counter + 1,
        MODE_UP_BOUND, MODE_DOWN_BOUND);
        sp_ifmd_32_soft_status->om_11_2_3_lock_counter =
        ifmd_round(sp_ifmd_32_soft_status->om_11_2_3_lock_counter - 1,
        MODE_UP_BOUND, MODE_DOWN_BOUND);
    }

    if (sp_ifmd_32_soft_status->om_lock_counter >= 10 && /* 10, threshold */
        sp_ifmd_32_soft_status->om_unlock_counter == 0) {
        ret = 1;
    } else if (sp_ifmd_32_soft_status->om_unlock_counter >= 10) { /* 10, threshold */
        ret = 0;
    }

    if (sp_ifmd_32_soft_status->sp_frame_diff_characters.kf_cnt_btw_period == -1) {
        ret = 0;
    }

    // NTSC -> PAL
    if (ret == 1) {
        ret = 1;                                                        /* 1, return value */
    } else if (sp_ifmd_32_soft_status->other_mode_lock_counter >= 10) { /* 10, threshold */
        ret = 2;                                                        /* 2, return value */
    } else if (sp_ifmd_32_soft_status->other_mode_cnt > 0) {
        ret = 2;                                                          /* 2, return value */
    } else if (sp_ifmd_32_soft_status->other_mode_unlock_counter >= 10) { /* 10, threshold */
        ret = 0;
    }

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug55_64_87_en[1]) {
        HI_ERR_PQ("%2d,%2d,%2d ", sp_ifmd_32_soft_status->sp_frame_diff_characters.other_mode_cv_cnt,
                  sp_ifmd_32_soft_status->other_mode_cnt, sp_ifmd_32_soft_status->other_mode_lock_counter);
    }

    return ret;
}

hi_s32 ifmd_lock_mode_from_non(hi_s32 *p_final_mode, hi_s32 *p_final_phase, hi_s32 *mode_decision_field,
                               ifmd_32_soft_status_s *sp_ifmd_32_soft_status)
{
    hi_s32 i;
    hi_s32 temp_final_mode[TOTAL_MODES] = { 0 };
    hi_s32 lock_mode_count = 0;
    hi_s32 lock_pulldown_mode[TOTAL_MODES] = { 0 };
    hi_s32 pulldown = 0;
    hi_s32 final_mode = 0;
    hi_s32 final_phase = 0;

    // 对所有8种已知cadence的非零判断结果进行记录，然后再从非零结果中寻找可以输出的锁定值
    for (i = 0; i < TOTAL_MODES; i++) {
        pulldown = *(mode_decision_field + i);

        if (pulldown > 0) {
            temp_final_mode[i] = pulldown;
            lock_pulldown_mode[lock_mode_count] = i;
            lock_mode_count++;
        }
    }

    // if only one mode has been locked, it is the mode.
    if (lock_mode_count == 1) {
        // 只有一种锁定结果
        // final_mode: at the hundred is the 8 modes, at the most lowest octobers are the phases
        final_mode = lock_pulldown_mode[0] * 100 + temp_final_mode[lock_pulldown_mode[0]]; /* 0  index, 100 mask */
    } else if (lock_mode_count == 2) {                                                     /* 2, 两种锁定结果 */
        if (lock_pulldown_mode[0] == MODE_32 && lock_pulldown_mode[1] == MODE_32322) {
            final_mode = lock_pulldown_mode[1] * 100 + temp_final_mode[lock_pulldown_mode[1]]; /* 1  index, 100 mask */
        } else {
            final_mode = lock_pulldown_mode[0] * 100 + temp_final_mode[lock_pulldown_mode[0]]; /* 0  index, 100 mask */
        }
    }

    *p_final_mode = final_mode;
    *p_final_phase = final_phase;
    return 1;
}

/*************************************************
Functions:      检测序列是否为已知cadence模式，若是，给出相位。
Description:    首先对每种已知cadence模式检测最匹配相位，然后结合各种模式的结果给出最终匹配结果。
Called By:      main()
Input:          当前检测帧序号、场相位，场景切换信息，以及缓存序列特征的缓冲大小
Output:         最匹配模式相位信息
Return:         最匹配模式相位信息，三位表示，最高位表示属于哪种cadence模式，后两位表示该模式下的相位
Others:         该计算仅用于Pulldown
*************************************************/
hi_s32 ifmd_get_final_mode(hi_s32 mode_locked, hi_s32 *mode_decision_field, hi_u32 mode_decision_len,
                           ifmd_32_soft_status_s *sp_ifmd_32_soft_status)
{
    hi_s32 final_mode = 0;
    hi_s32 final_phase = 0;

    PQ_CHECK_NUM_LOW_RANGE_RE_FAIL(mode_decision_len, TOTAL_MODES);

    // 若仍未锁定某一模式的某一相位，进行锁定操作
    if (!mode_locked) {
        ifmd_lock_mode_from_non(&final_mode, &final_phase, mode_decision_field, sp_ifmd_32_soft_status);
    } else {
        // 若已经锁定某一模式的某一相位，进行失锁判断操作
        final_mode = mode_locked / 100;  /* 100, mask */
        final_phase = mode_locked % 100; /* 100, mask */

        if (final_phase != *(mode_decision_field + final_mode)) {
            if ((*(mode_decision_field + final_mode)) != 0) {
                final_phase = *(mode_decision_field + final_mode);
            } else {
                // unlock
                final_mode = 0;
                final_phase = 0;
            }
        }

        final_mode = final_mode * 100 + final_phase; /* 100, mask */
    }

    return final_mode;  // final_mode: at the hundred is the 8 modes, at the most lowest octobers are the phases
}

/*************************************************
Function:       DetectPulldownModePhase
Description:    检测当前序列是否有可能是32Pulldown（TForBF），若是，给出其相位信息和PCC值。
Calls:          PulldownDetection32CadenceMode(), PulldownDetection2332CadenceMode()等
Called By:      main()
Input:          当前检测帧序号、场相位，场景切换信息，以及缓存序列特征的缓冲大小
Output:         最匹配模式相位信息
Return:         最匹配模式相位信息，三位表示，最高位表示属于哪种cadence模式，后两位表示该模式下的相位
Others:         该计算仅用于Pulldown
*************************************************/
hi_s32 ifmd_detect_pulldown_mode_phase(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                                       ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_s32 mode_decision_field[TOTAL_MODES] = { 0 };

    sp_ifmd_32_soft_status->el_32_confirm = 0;
    sp_ifmd_32_soft_status->cv_2_confirm = 0;

    mode_decision_field[MODE_32] = ifmd_pulldown_detection_32_cadence_mode(sp_ifmd_32_soft_status,
                                                                           sp_ifmd_32_soft_ctrl);
    mode_decision_field[MODE_2332] = ifmd_pulldown_detection_2332_cadence_mode(sp_ifmd_32_soft_status,
                                                                               sp_ifmd_32_soft_ctrl);
    mode_decision_field[MODE_32322] = ifmd_pulldown_detection_32322_cadence_mode(sp_ifmd_32_soft_status,
                                                                                 sp_ifmd_32_soft_ctrl);
    mode_decision_field[MODE_11_2_3] = ifmd_pulldown_detection_11_2_3_cadence_mode(sp_ifmd_32_soft_status,
                                                                                   sp_ifmd_32_soft_ctrl);

    sp_ifmd_32_soft_status->final_lock_mode_dei = ifmd_get_final_mode(sp_ifmd_32_soft_status->final_lock_mode_dei,
        mode_decision_field, TOTAL_MODES, sp_ifmd_32_soft_status);

    if (sp_ifmd_32_soft_status->final_lock_mode_dei > 0 &&
        sp_ifmd_32_soft_status->final_lock_mode_dei < 11) { /* 32 mode  11, threshold */
        sp_ifmd_32_soft_status->mask_any_cnt = 10;          /* 10, threshold */
    } else {
        sp_ifmd_32_soft_status->mask_any_cnt =
        MAX2(0, sp_ifmd_32_soft_status->mask_any_cnt - 1);
    }

    if (sp_ifmd_32_soft_status->mask_any_cnt > 0 &&
        !(sp_ifmd_32_soft_status->final_lock_mode_dei > 0 &&
          sp_ifmd_32_soft_status->final_lock_mode_dei < 11)) { /* 11, threshold */
        sp_ifmd_32_soft_status->final_lock_mode_dei = 0;
    }

    return (sp_ifmd_32_soft_status->final_lock_mode_dei);
}

hi_void ifmd_get_comb_and_sigma_feature(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                                        ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    hi_s32 mode;   /* 模式，32, 2224 or 2332, etc. */
    /* 相位, 每个模式有（FO个数每个FO相位个数）个相位, 从1开始计数 */
    hi_s32 phase;
    hi_u16 final_mode = sp_ifmd_32_soft_status->final_lock_mode_dei;

    hi_s32 field_num = sp_ifmd_32_soft_status->field_num;
    hi_u8 buff_size = sp_ifmd_32_soft_ctrl->ifmd_det_buff_size;

    hi_s32 *p_sigma = HI_NULL;
    hi_s32 *p_field_comb = HI_NULL;
    hi_s8 *p_sigma_vary = HI_NULL;
    hi_s8 *p_m_comb03_minus_m_comb12 = HI_NULL;
    hi_s32 key_frame_range;

    hi_s32 fo;                    // 当前的FO
    hi_s32 phase_in_fo;           // 在当前FO中的实际相位，即帧偏移量
    hi_s32 phase_num_in_each_fo;  // 每个FO相位个数：每个模式的周期都取了相位的两倍个数
    ifmd_cad_mode_para_set_s *p_cmd_para_set = HI_NULL;
    hi_s32 max_comb, min_comb, mean_comb_thd, i, mean_comb_thd6;
    hi_s32 max_sigma, min_sigma;
    hi_s32 diff_larger_flag;
    hi_s32 diff_large_flag;
    hi_s32 max_comb2[2] = { 0 }; /* 2  index */
    hi_s32 large_comb0, large_comb1;
    hi_s32 s32_ret;

    PQ_CHECK_NULL_PTR_RE_NULL(pst_vir_soft_reg);
    p_cmd_para_set = &(sp_ifmd_32_soft_status->s_cmd_para_set);

    sp_ifmd_32_soft_status->b_comb_diff_large = 0;
    sp_ifmd_32_soft_status->b_comb_large = 0;
    sp_ifmd_32_soft_status->b_sigma_has_vary = 0;

    mode = final_mode / 100;   // 模式，32, 2224 or 2332, etc. 100, mask
    phase = final_mode % 100;  // 相位，每个模式有（FO个数*每个FO相位个数）个相位，从1开始计数, 100, mask

    // changhong: himedia box cvbs, wuxianpu
    key_frame_range = sp_ifmd_32_soft_status->music_flag ? 6 : g_ca_key_frame_range[mode]; /* 6, default value */
    /* 每个FO相位个数：每个模式的周期都取了相位的两倍个数,2 */
    phase_num_in_each_fo = g_ca_period[mode] / 2; /* 2: num */
    fo = (phase - 1) / phase_num_in_each_fo;   /* 当前的FO */
    phase_in_fo = (phase - 1) % phase_num_in_each_fo;

    p_sigma = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma;
    p_field_comb = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb;
    p_sigma_vary = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_sigma_vary;
    p_m_comb03_minus_m_comb12 = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_mcomb_03_minus_mcomb_12;

    max_comb = ifmd_get_max_value(p_field_comb, key_frame_range, field_num, buff_size);
    min_comb = ifmd_get_min_value(p_field_comb, key_frame_range, field_num, buff_size);
    max_sigma = ifmd_get_max_value(p_sigma, key_frame_range, field_num, buff_size);
    min_sigma = ifmd_get_min_value(p_sigma, g_ca_period[mode], field_num, buff_size);
    mean_comb_thd = (min_comb + (max_comb - min_comb) / 4); /* 4, ratio */

    sp_ifmd_32_soft_status->key_value_from_module =
        (1 == p_cmd_para_set->p_md_cadence[mode][fo][(field_num + phase_in_fo * 2) %  /* 2, ratio */
                                                     g_ca_period[mode]]);

    if (p_field_comb[(field_num + 1) % buff_size] >  /* 下一场发生了场景切换 */
        max_comb * 2 + 1500) {   /* 2, ratio, 1500 offset value */
        sp_ifmd_32_soft_status->b_comb_large_sharp =
            sp_ifmd_32_soft_status->el_32_confirm ? 10 : 6; /* 10, 6 default value */
    } else if (sp_ifmd_32_soft_status->b_comb_large_sharp) {
        sp_ifmd_32_soft_status->b_comb_large_sharp = MAX2(sp_ifmd_32_soft_status->b_comb_large_sharp - 1, 0);
    }

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug55_64_87_en[2]) { /* 2  index */
        HI_ERR_PQ("M:%7d, ", max_comb);
        HI_ERR_PQ("S:%d, ", sp_ifmd_32_soft_status->b_comb_large_sharp);
    }

    max_comb = ifmd_get_max_value(p_field_comb, key_frame_range, field_num, buff_size);
    min_comb = ifmd_get_min_value(p_field_comb, key_frame_range, field_num, buff_size);
    mean_comb_thd = (min_comb + (max_comb - min_comb) / 4);  /* 4  ratio */
    mean_comb_thd6 = (min_comb + (max_comb - min_comb) / 6); /* 6  ratio */

    s32_ret = ifmd_get_2max_value(p_field_comb, max_comb2, key_frame_range + 1, field_num, buff_size);
    if (s32_ret != HI_SUCCESS) {
        HI_ERR_PQ("\n   ifmd_get_2max_value failed!\n");
        return;
    }

    large_comb0 = max_comb2[0];

    if (max_comb2[0] > 2 * max_comb2[1]) { /* 2 comb ratio, 1 index */
        large_comb1 = max_comb2[0];
    } else {
        large_comb1 = max_comb2[1];
    }

    /*  comb 区分度归类 */
    for (i = 0; i < g_ca_period[mode]; i++) {
        if (p_sigma_vary[(field_num - i + buff_size) % buff_size]) {
            sp_ifmd_32_soft_status->b_sigma_has_vary = 1;
            break;
        }
    }

    diff_larger_flag = max_comb > 2 * min_comb || sp_ifmd_32_soft_status->b_sigma_has_vary; /* 2 comb ratio */
    diff_large_flag = (max_comb > min_comb * 3 / 2 && min_comb > 10000) || /* 3 / 2, 10000 ratio and threshold */
                      (max_comb > min_comb * 7 / 4 && min_comb > 1000); /* 7 / 4, 1000  ratio and threshold */

    if (diff_larger_flag) {
        sp_ifmd_32_soft_status->b_comb_diff_large = 2; /* 2 combDiff threshold */
    } else if (diff_large_flag) {
        sp_ifmd_32_soft_status->b_comb_diff_large = 1; /* comb区分度较大 */
    }

    if (p_field_comb[field_num % buff_size] <= mean_comb_thd && p_field_comb[field_num % buff_size] > mean_comb_thd6 &&
        sp_ifmd_32_soft_status->s_still_or_rolling_subtitle[mode].b_start) {
        sp_ifmd_32_soft_status->b_comb_diff_large = -1;
    }

    /* 找出comb序列中的大值 */
    if (p_field_comb[field_num % buff_size] > mean_comb_thd) {
        sp_ifmd_32_soft_status->b_comb_large = 1; /* 当前comb为大值 */
    }
}

hi_s32 ifmd_dei_result_lock(hi_drv_pq_ifmd_playback *sp_ifmd_result, ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
                            ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    // hi_s32 mode = sp_ifmd_32_soft_status->final_lock_mode_dei / 100; /* 100 mask value */
    hi_s32 result_key_value = sp_ifmd_32_soft_status->key_value_from_module;
    hi_s32 result_dir_mch = 0;
    hi_s32 b_change_md_flag = 0;

    hi_s32 field_num = sp_ifmd_32_soft_status->field_num;
    hi_u8 buff_size = sp_ifmd_32_soft_ctrl->ifmd_det_buff_size;
    hi_s32 *p_field_comb = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(pst_vir_soft_reg);
    p_field_comb = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb;

    if (sp_ifmd_32_soft_status->scene_change || sp_ifmd_32_soft_status->b_comb_large_sharp) {
        sp_ifmd_result->dir_mch = -1;
        sp_ifmd_result->key_frame = sp_ifmd_32_soft_status->key_value_from_module;

        return HI_SUCCESS;
    }

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug11_2_3_en[1]) {
        HI_ERR_PQ("%d,", sp_ifmd_32_soft_status->el_32_confirm +
                  2 * sp_ifmd_32_soft_status->cv_2_confirm); /* 2 cv ratio */
        // HI_ERR_PQ("X2:%d, ", sp_ifmd_32_soft_status->cv_2_confirm);
    }

    if (sp_ifmd_32_soft_status->el_32_confirm &&
        sp_ifmd_32_soft_status->final_dei_mode_include_22 <= 10) { /* 10 mode value threshold */
        sp_ifmd_result->dir_mch = sp_ifmd_32_soft_status->sp_frame_diff_characters.el_vary[(field_num) % buff_size];
        sp_ifmd_result->key_frame = clip0(sp_ifmd_result->dir_mch);
        return HI_SUCCESS;
    } else if (sp_ifmd_32_soft_status->cv_2_confirm &&
               sp_ifmd_32_soft_status->final_dei_mode_include_22 <= 10) { /* 10 mode value threshold */
        sp_ifmd_result->dir_mch = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary_32[(field_num) %
                                              buff_size];
        sp_ifmd_result->key_frame = clip0(sp_ifmd_result->dir_mch);
        return HI_SUCCESS;
    }

    /* 如果Comb值有区分度，或者Sigma有区分度，说明可以利用Comb值大小来确定当前匹配方向 */
    if (sp_ifmd_32_soft_status->b_comb_diff_large > 0) {
        b_change_md_flag = 0;

        if (b_change_md_flag) {
            result_dir_mch = -1;
        } else {
            result_dir_mch = sp_ifmd_32_soft_status->b_comb_large;
            result_key_value = sp_ifmd_32_soft_status->b_comb_large;
        }
    } else {
        result_dir_mch = -1;
        result_key_value = sp_ifmd_32_soft_status->b_comb_large;
    }

    if (p_field_comb[(field_num + 1) % buff_size] * 5 / 4 > /* 5 /4 comb ratio */
        p_field_comb[(field_num) % buff_size] &&
        result_dir_mch == 1) {
        sp_ifmd_32_soft_status->mask_32_cnt = 3; /* 3 param default value */
    } else {
        sp_ifmd_32_soft_status->mask_32_cnt = MAX2(0, sp_ifmd_32_soft_status->mask_32_cnt - 1);
    }

    if (sp_ifmd_32_soft_status->mask_32_cnt > 0) {
        result_dir_mch = -1;
    }

    sp_ifmd_result->dir_mch = result_dir_mch;
    sp_ifmd_result->key_frame = result_key_value;

    return HI_SUCCESS;
}

/*************************************************
Function:       GetNon22ModeKeyFrameState
Description:    获取当前处理场（缓冲的5场数据中第2场）对应的是否为“关键帧”。
本函数用了根据当前检测的结果来判断当前处理场是否对应为deinterlace后的复制帧。
关键帧的定义：
每一个帧内容第一次出现的帧，即所谓“非复制帧”为关键帧，后面与其像素内容一致的均称为复制帧。
Calls:          N/A
Called By:      main()
Input:          参数指针
Output:         无
Return:         无
Others:         DeDei比Frc优先级高，
*************************************************/
hi_s32 ifmd_get_non22_mode_key_frame_state(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
    ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl, hi_drv_pq_ifmd_playback *sp_ifmd_result)
{
    if (!sp_ifmd_32_soft_status->final_lock_mode_dei) {
        sp_ifmd_result->dir_mch = -1;
        sp_ifmd_result->key_frame = 1;
        return 2; /* 2, return value */
    } else {
        ifmd_get_comb_and_sigma_feature(sp_ifmd_32_soft_status, sp_ifmd_32_soft_ctrl);

        ifmd_dei_result_lock(sp_ifmd_result, sp_ifmd_32_soft_status, sp_ifmd_32_soft_ctrl);
    }

    return HI_SUCCESS;
}

hi_void ifmd_get_pcc_eigen_value(ifmd_22_contxt_s *p_cntxt22, hi_s32 *like22_in_period, pq_ifmd_fod s32_fo)
{
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    hi_s32 i;
    hi_s32 s32_l03, s32_l12, s32_l23;
    hi_s32 b_judge;
    hi_u8 hd_flag;

    PQ_CHECK_NULL_PTR_RE_NULL(pst_vir_soft_reg);
    hd_flag = p_cntxt22->hd_flag;

    for (i = 0; i < (EIGENVALUE_BUFFSIZE - 1); i++) {
        p_cntxt22->a_like22_in_period[i] = p_cntxt22->a_like22_in_period[i + 1];

        if (p_cntxt22->a_like22_in_period[i]) {
            (*like22_in_period)++;
        }
    }

    s32_l03 = (p_cntxt22->frame_pcc_crss < 1) ? 1 : p_cntxt22->frame_pcc_crss;
    s32_l12 = (p_cntxt22->frame_non_match_weave < 1) ? 1 : p_cntxt22->frame_non_match_weave;
    s32_l23 = (p_cntxt22->frame_match_weave < 1) ? 1 : p_cntxt22->frame_match_weave;

    b_judge = (s32_l03 >= 30 && s32_l03 < 100 &&                                      /* 100, 30, threshold */
               pq_abs(s32_l12 - s32_l03) < 20 && s32_l23 == 1) ||                     /* 20, 1 threshold */
              (s32_l03 >= 100 && s32_l03 < 500 &&                                     /* 100, 500, threshold */
               pq_abs(s32_l12 - s32_l03) < 80 && s32_l23 <= 5) ||                     /* 80, 5 threshold */
              (s32_l03 >= 500 && s32_l03 < 1000 &&                                    /* 500, 1000, threshold */
               pq_abs(s32_l12 - s32_l03) < 100 && s32_l23 <= 20) ||                   /* 100, 20 threshold */
              (s32_l03 >= 1000 && s32_l03 < 2000 &&                                   /* 2000, 1000, threshold */
               pq_abs(s32_l12 - s32_l03) < 500 && s32_l23 <= 50) ||                   /* 500, 50 threshold */
              (s32_l03 >= 2000 && s32_l03 < 10000 && pq_abs(s32_l12 - s32_l03) < 1000 && /* 2000, 10000, 1000 threshold */
               (s32_l23 <= 100 || (s32_l03 / s32_l23 >= 3 && hd_flag))) ||         /* 100, 3  threshold */
              (s32_l03 >= 10000 && pq_abs(s32_l12 - s32_l03) < 2000 && s32_l23 <= 600);  /* 10000, 2000, 600 threshold */

    if (b_judge) {
        p_cntxt22->a_like22_in_period[EIGENVALUE_BUFFSIZE - 1] = 1;
        (*like22_in_period)++;
    } else {
        p_cntxt22->a_like22_in_period[EIGENVALUE_BUFFSIZE - 1] = 0;
    }

    // patch for hisense,
    if (s32_l03 >= 5000 && s32_l03 < 15000 &&                          /* 5000, 15000, threshold */
        pq_abs(s32_l12 - s32_l03) < 1000 && s32_l23 >= 100 && !hd_flag) { /* 1000, 100 threshold */
        p_cntxt22->mask_22 = 1;
    } else {
        p_cntxt22->mask_22 = 0;
    }

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug22_en[1]) {
        if (s32_fo == TBF) {
            HI_ERR_PQ("L:%6d,%6d,%6d,", s32_l03, s32_l12, s32_l23);
        }
    }

    return;
}

hi_void ifmd_get_um_eigen_value(ifmd_22_contxt_s *p_cntxt22, hi_s32 *um_like22_in_period, pq_ifmd_fod s32_fo)
{
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    hi_s32 i;
    hi_s32 a, b, c, d;
    hi_s32 b_judge;
    hi_s32 non_um_div_um;
    hi_s32 non_um2_div_um2;
    hi_s32 um_div_um2;
    hi_s32 non_um_div_non_um2;

    PQ_CHECK_NULL_PTR_RE_NULL(pst_vir_soft_reg);
    for (i = 0; i < (EIGENVALUE_BUFFSIZE - 1); i++) {
        p_cntxt22->a_um_like22_in_period[i] = p_cntxt22->a_um_like22_in_period[i + 1];

        if (p_cntxt22->a_um_like22_in_period[i]) {
            (*um_like22_in_period)++;
        }
    }

    p_cntxt22->frame_non_match_um = MIN2(p_cntxt22->frame_non_match_um, MAX_S32_VALUE_DIV_1000);
    p_cntxt22->frame_non_match_um_2 = MIN2(p_cntxt22->frame_non_match_um_2, MAX_S32_VALUE_DIV_1000);
    p_cntxt22->frame_match_um = MIN2(p_cntxt22->frame_match_um, MAX_S32_VALUE_DIV_1000);

    non_um_div_um = (hi_u32)p_cntxt22->frame_non_match_um * 1000 / /* UM 1000, ratio, 1, ratio */
                    (p_cntxt22->frame_match_um == 0 ? 1 : p_cntxt22->frame_match_um);
    non_um2_div_um2 = p_cntxt22->frame_non_match_um_2 * 1000 / /* 1000, ratio, 1, ratio */
                      (p_cntxt22->frame_match_um_2 == 0 ? 1 : p_cntxt22->frame_match_um_2);
    um_div_um2 = p_cntxt22->frame_match_um * 1000 / /* 1000, ratio, 1, ratio */
                 (p_cntxt22->frame_match_um_2 == 0 ? 1 : p_cntxt22->frame_match_um_2);
    non_um_div_non_um2 = p_cntxt22->frame_non_match_um * 1000 / /* 1000, ratio */
                         (p_cntxt22->frame_non_match_um_2 == 0 ? 1 : p_cntxt22->frame_non_match_um_2);

    a = non_um_div_um;
    b = non_um2_div_um2;
    c = um_div_um2;
    d = non_um_div_non_um2;
    b_judge = ((a > 2000 && a < 10000 && pq_abs(a - b) < 3000) ||       /* 2000, 3000, 10000, threshold */
               (a > 10000 && a < 20000 && pq_abs(a - b) < 5000) ||   /* 20000, 5000, 10000, threshold */
               (a > 20000 && a < 500000 && pq_abs(a - b) < 10000) || /* 20000, 500000, 10000, threshold */
               (a > 20000 && b > 20000)) && /* 20000 threshold */
              (((c > 300) && (c < 1500))&& ((d > 300) && (d < 1500))); /* 300, 1500 threshold */
    if (b_judge) {
        p_cntxt22->a_um_like22_in_period[EIGENVALUE_BUFFSIZE - 1] = 1;
        (*um_like22_in_period)++;
    } else {
        p_cntxt22->a_um_like22_in_period[EIGENVALUE_BUFFSIZE - 1] = 0;
    }

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug22_en[2]) { /* 2, index */
        if (s32_fo == TBF) {
            HI_ERR_PQ("a:%6d,%6d,%6d,%6d,", a, b, c, d);
        }
    }

    return;
}

hi_void ifmd_get_it_diff_eigen_value(ifmd_hard2soft_api *sp_ifmd_hard_to_soft_api, pq_ifmd_fod s32_fo,
                                     ifmd_22_contxt_s *p_cntxt22)
{
    hi_s32 frame_it_diff = 0;

    if (s32_fo == TF) {
        frame_it_diff = sp_ifmd_hard_to_soft_api->frm_it_diff;
    } else if (s32_fo == TBF) {
        frame_it_diff = sp_ifmd_hard_to_soft_api->frm_it_diff_be;
    }

    /* 以下为采用frame ITDiff解锁，一般情况下，frame_it_diff的响应要比计数器快得多; */
    p_cntxt22->hist_frm_it_diff[4] = p_cntxt22->hist_frm_it_diff[3]; /* 4,3, index */
    p_cntxt22->hist_frm_it_diff[3] = p_cntxt22->hist_frm_it_diff[2]; /* 3,2, index */
    p_cntxt22->hist_frm_it_diff[2] = p_cntxt22->hist_frm_it_diff[1]; /* 2,1, index */
    p_cntxt22->hist_frm_it_diff[1] = p_cntxt22->hist_frm_it_diff[0];
    p_cntxt22->hist_frm_it_diff[0] = frame_it_diff;

    /* 计算均值; */
    p_cntxt22->cur_frm_it_diff_mean = (p_cntxt22->hist_frm_it_diff[4] + /* 4, index */
    p_cntxt22->hist_frm_it_diff[3] + p_cntxt22->hist_frm_it_diff[2] + /* 3, 2, index */
    p_cntxt22->hist_frm_it_diff[1] + p_cntxt22->hist_frm_it_diff[0]) / 5; /* 5, hist num */

    p_cntxt22->mean_frm_it_diff_acc = p_cntxt22->mean_frm_it_diff_acc +
    p_cntxt22->cur_frm_it_diff_mean - p_cntxt22->lst_frm_it_diff_mean;

    p_cntxt22->mean_frm_it_diff_acc = ifmd_clip3(-3000, 3000, p_cntxt22->mean_frm_it_diff_acc);
    p_cntxt22->lst_frm_it_diff_mean = p_cntxt22->cur_frm_it_diff_mean;

    return;
}

hi_void ifmd_j48_classifier(ifmd_22_contxt_s *p_cntxt22, hi_s32 like22_in_period, hi_s32 um_like22_in_period)
{
    /* 利用特征值进行分类; */
    if (um_like22_in_period <= 2 && like22_in_period <= 8) { /* 2,8, period threshold */
        p_cntxt22->phase_unlock_cnt += 4;                    /* 4, unlock step */
        p_cntxt22->phase_lock_cnt -= 4;                      /* 4, lock step */
    } else if (um_like22_in_period <= 5) {                   /* 5, period threshold */
        if (like22_in_period > 8) {                          /* 8, period threshold */
            p_cntxt22->phase_lock_cnt++;
            p_cntxt22->phase_unlock_cnt--;
        } else {
            p_cntxt22->phase_unlock_cnt += 2; /* 2, unlock step */
            p_cntxt22->phase_lock_cnt -= 2;   /* 2, lock step */
        }
    } else {
        if (um_like22_in_period > 9) { /* 9, period threshold */
            /* P */
            p_cntxt22->phase_lock_cnt += 2;   /* 2, lock step */
            p_cntxt22->phase_unlock_cnt -= 2; /* 2, unlock step */
        } else {
            if (um_like22_in_period <= 7) { /* 7, period threshold */
                if (like22_in_period > 4) { /* 4,  period threshold */
                    /* P */
                    p_cntxt22->phase_lock_cnt++;
                    p_cntxt22->phase_unlock_cnt--;
                } else {
                    /* I */
                    p_cntxt22->phase_unlock_cnt++;
                    p_cntxt22->phase_lock_cnt--;
                }
            } else {
                /* P */
                p_cntxt22->phase_lock_cnt++;
                p_cntxt22->phase_unlock_cnt--;
            }
        }
    }

    return;
}

/* Adjust 22 field Counter */
hi_void ifmd_adjust_22_counter(ifmd_22_contxt_s *p_cntxt22, ifmd_22_soft_ctrl_s *sp_ifmd_22_soft_ctrl)
{
    hi_s32 match_tkr;

    match_tkr = (p_cntxt22->frame_match_tkr < p_cntxt22->frame_non_match_tkr) ? p_cntxt22->frame_match_tkr :
                p_cntxt22->frame_non_match_tkr;

    if (0) {  // UmStillFlag)
        p_cntxt22->phase_lock_cnt = p_cntxt22->phase_last_lock_cnt;
        p_cntxt22->phase_unlock_cnt = p_cntxt22->phase_last_unlock_cnt;
    }

    if (match_tkr > sp_ifmd_22_soft_ctrl->tkr_ctrl.tkr_thr_level) {
        p_cntxt22->phase_lock_cnt -= sp_ifmd_22_soft_ctrl->locker_ctrl.hist_check_dec;
        p_cntxt22->phase_unlock_cnt += sp_ifmd_22_soft_ctrl->locker_ctrl.hist_check_dec;
    }
}

/* Calculate 22 field Lock Mode */
hi_void ifmd_get_22_lock_mode(ifmd_22_contxt_s *p_cntxt22, ifmd_22_soft_ctrl_s *sp_ifmd_22_soft_ctrl,
                              hi_s32 scene_change_counter, pq_ifmd_fod s32_fo)
{
    hi_s32 lst_lock = p_cntxt22->pld_22_lock;

    /* 锁定与解锁; */
    if (p_cntxt22->phase_lock_cnt >= sp_ifmd_22_soft_ctrl->locker_ctrl.enter_lock_level) {
        p_cntxt22->pld_22_lock = 1;

        if (p_cntxt22->phase_lock_cnt > p_cntxt22->phase_last_lock_cnt) {
            p_cntxt22->phase_unlock_cnt = 0;
        }
    }

    if (p_cntxt22->phase_unlock_cnt > sp_ifmd_22_soft_ctrl->locker_ctrl.exit_lock_level) {
        p_cntxt22->pld_22_lock = 0;

        if (p_cntxt22->phase_unlock_cnt > p_cntxt22->phase_last_unlock_cnt) {
            p_cntxt22->phase_lock_cnt = 0;
        }
    }

    p_cntxt22->phase_lock_cnt = ifmd_clip3(0, 100, p_cntxt22->phase_lock_cnt);     /* 100, limit threshold */
    p_cntxt22->phase_unlock_cnt = ifmd_clip3(0, 100, p_cntxt22->phase_unlock_cnt); /* 100, threshold */

    if ((p_cntxt22->mean_frm_it_diff_acc >= sp_ifmd_22_soft_ctrl->it_diff_mean_thd) && (!scene_change_counter)) {
        p_cntxt22->phase_lock_cnt = 0;
        p_cntxt22->phase_unlock_cnt = 20; /* 20, threshold */
        p_cntxt22->pld_22_lock = 0;
    }

    if ((lst_lock == 0) && (p_cntxt22->pld_22_lock == 1)) {
        p_cntxt22->mean_frm_it_diff_acc = 0;
    }

    p_cntxt22->phase_last_lock_cnt = p_cntxt22->phase_lock_cnt;
    p_cntxt22->phase_last_unlock_cnt = p_cntxt22->phase_unlock_cnt;

    return;
}

/* Calculate 22 field mode weight */
hi_void ifmd_calc_22_pld(ifmd_hard2soft_api *sp_ifmd_hard_to_soft_api, ifmd_22_soft_ctrl_s *sp_ifmd_22_soft_ctrl,
                         ifmd_22_contxt_s *p_cntxt22, pq_ifmd_fod s32_fo, hi_s32 scene_change_counter)
{
    hi_s32 like22_in_period = 0;
    hi_s32 um_like22_in_period = 0;
    hi_drv_pq_dbg_vpss_vir_soft *pst_vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    PQ_CHECK_NULL_PTR_RE_NULL(pst_vir_soft_reg);
    ifmd_get_pcc_eigen_value(p_cntxt22, &like22_in_period, s32_fo);
    ifmd_get_um_eigen_value(p_cntxt22, &um_like22_in_period, s32_fo);
    ifmd_get_it_diff_eigen_value(sp_ifmd_hard_to_soft_api, s32_fo, p_cntxt22);
    ifmd_j48_classifier(p_cntxt22, like22_in_period, um_like22_in_period);
    ifmd_adjust_22_counter(p_cntxt22, sp_ifmd_22_soft_ctrl);
    ifmd_get_22_lock_mode(p_cntxt22, sp_ifmd_22_soft_ctrl, scene_change_counter, s32_fo);

    if (pst_vir_soft_reg->ifmd_reg.ifmd_debug22_en[0]) {
        if (s32_fo == TBF) {
            HI_ERR_PQ("itDiff = %6d, Like22 = %2d, UMLike22 = %2d, LCnt22 = %2d, UnLCnt22 = %2d, ",
                      p_cntxt22->hist_frm_it_diff[0], like22_in_period, um_like22_in_period,
                      p_cntxt22->phase_lock_cnt, p_cntxt22->phase_unlock_cnt);
        }
    }

    return;
}

/* Detect the video is 22 field mode or not */
hi_s32 ifmd_detect_22_pld(ifmd_hard2soft_api *sp_ifmd_hard_to_soft_api,
                          ifmd_22_soft_status_s *sp_ifmd_22_softs_tatus, ifmd_22_soft_ctrl_s *sp_ifmd_22_soft_ctrl,
                          ifmd_32_soft_status_s *sp_ifmd_32_soft_status, ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl)
{
    ifmd_22_contxt_s *p_cntxt22;
    ifmd_22_contxt_s *p_cntxt22_be;
    hi_s32 cur_lock = 0;
    hi_s32 scene_change_counter = sp_ifmd_32_soft_status->scene_change_counter;

    p_cntxt22 = &(sp_ifmd_22_softs_tatus->pld_22_ctx);
    p_cntxt22_be = &(sp_ifmd_22_softs_tatus->pld_22_ctx_be);

    ifmd_calc_22_pld(sp_ifmd_hard_to_soft_api, sp_ifmd_22_soft_ctrl, p_cntxt22, TF, scene_change_counter);
    ifmd_calc_22_pld(sp_ifmd_hard_to_soft_api, sp_ifmd_22_soft_ctrl, p_cntxt22_be, TBF, scene_change_counter);

    if (p_cntxt22->pld_22_lock && !p_cntxt22_be->pld_22_lock) {
        cur_lock = p_cntxt22->pld_22_lock;
    } else if (!p_cntxt22->pld_22_lock && p_cntxt22_be->pld_22_lock) {
        cur_lock = p_cntxt22_be->pld_22_lock + 1;
    } else if (p_cntxt22->pld_22_lock && p_cntxt22_be->pld_22_lock) {
        cur_lock = p_cntxt22->pld_22_lock;
    } else {
        cur_lock = 0;
    }

    return cur_lock;
}

/* Check the Current Field is key Frame or not */
hi_bool ifmd_get_22_mode_key_frame_state(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
    ifmd_22_soft_status_s *sp_ifmd_22_softs_tatus, hi_drv_pq_ifmd_playback *sp_ifmd_result, hi_s32 field_mode)
{
    hi_bool b_key_frame = HI_FALSE;
    hi_s32 b_is_progressive = sp_ifmd_32_soft_status->b_is_progressive;
    hi_s32 backward_flag;
    hi_s32 forward_flag;
    hi_s32 *p_field_comb;

    hi_s32 field_num, buff_size, sigma_buff_idx, sigma_buff_idx_last, sigma_buff_idx_pro, sigma_buff_idx_next;
    field_num = sp_ifmd_32_soft_status->field_num;
    buff_size = BUFF_SIZE;
    sigma_buff_idx = field_num % buff_size;
    sigma_buff_idx_pro = (sigma_buff_idx + buff_size - 2) % buff_size;  /* -2, pro index */
    sigma_buff_idx_last = (sigma_buff_idx + buff_size - 1) % buff_size; /* -1, last index */
    sigma_buff_idx_next = (sigma_buff_idx + buff_size + 1) % buff_size; /* 1, next index */

    p_field_comb = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb;

    /* 软件层看来都是顶场优先，和实际的物理含义有差异 */
    backward_flag = (b_is_progressive == 1 && field_mode == TOP_FIELD_FMD) /* 1, 是否逐行 */
                    || (b_is_progressive == 2 && field_mode == BOTTOM_FIELD_FMD); /* 2, 是否逐行 */
    forward_flag = ((b_is_progressive == 1) && field_mode == BOTTOM_FIELD_FMD) /* 1, 是否逐行 */
                   || (b_is_progressive == 2 && field_mode == TOP_FIELD_FMD);     /* 2, 是否逐行 */

    if (backward_flag) {
        sp_ifmd_result->dir_mch = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[sigma_buff_idx];
        b_key_frame = HI_FALSE;

        /* solve PAl 306 chapter 34 35 lasi */
        if (2 * sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[sigma_buff_idx_next] < /* 2, ratio */
            sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[sigma_buff_idx_last] ||
            sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[sigma_buff_idx_next] >
            2 * sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[sigma_buff_idx_last]) { /* 2, ratio */
            if (sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[sigma_buff_idx_next] == 1 &&
                sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[sigma_buff_idx_last] == 1) {
                sp_ifmd_result->dir_mch = -1;
            }
        }

    } else if (forward_flag) {
        sp_ifmd_result->dir_mch = sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[sigma_buff_idx];
        b_key_frame = HI_TRUE;

        /* solve PAl 306 chapter 34 35 lasi */
        if ((2 * sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[sigma_buff_idx] < /* 2, ratio */
             sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[sigma_buff_idx_pro]) ||
            sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[sigma_buff_idx] >
            2 * sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb[sigma_buff_idx_pro]) { /* 2, ratio */
            if (sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[sigma_buff_idx] == 1 &&
                sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[sigma_buff_idx_pro] == 1) {
                sp_ifmd_result->dir_mch = -1;
            }
        }

    } else {
        sp_ifmd_result->dir_mch = -1;
    }

    if (sp_ifmd_result->dir_mch == 1 &&
        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[sigma_buff_idx_next] == 1) {
        sp_ifmd_result->dir_mch = -1;
    }

    if (MAX2(p_field_comb[sigma_buff_idx_next], p_field_comb[sigma_buff_idx]) <=
        4 * MIN2(p_field_comb[sigma_buff_idx_next], p_field_comb[sigma_buff_idx]) && /* 4, ratio */
        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[sigma_buff_idx_next] !=
        sp_ifmd_32_soft_status->sp_frame_diff_characters.a_field_comb_vary[sigma_buff_idx]) {
        sp_ifmd_result->dir_mch = -1;
    }

    if (sp_ifmd_32_soft_status->scene_change_counter || sp_ifmd_32_soft_status->mask_22_cnt ||
        sp_ifmd_22_softs_tatus->pld_22_ctx.mask_22 || sp_ifmd_22_softs_tatus->pld_22_ctx_be.mask_22) {
        sp_ifmd_result->dir_mch = -1;
    }

    return b_key_frame;
}

/* Init Ifmd api Param */
hi_s32 pq_mng_ifmd_sofeware_para_init(hi_u32 width, hi_u32 height)
{
    hi_s32 i, j;
    hi_u32 u32_ratio;

    hi_s32 phase[4] = { 2 * 5 + 1, 4 * 5 + 1, 4 * 6 + 1, 2 * 25 + 1}; /* 4/2/5/6/25:num */

    ifmd_32_soft_status_s *sp_ifmd_32_soft_status = &(g_ifmd_total_soft_para.s_ifmd_32_soft_status);
    ifmd_22_soft_status_s *sp_ifmd_22_softs_tatus = &(g_ifmd_total_soft_para.s_ifmd_22_soft_status);
    ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl = &(g_ifmd_total_soft_para.s_ifmd_32_soft_ctrl);
    ifmd_22_soft_ctrl_s *sp_ifmd_22_soft_ctrl = &(g_ifmd_total_soft_para.s_ifmd_22_soft_ctrl);

    u32_ratio = MAX2(1, width * height / (720 * 480)); /* (720 * 480), api para: default value */

    sp_ifmd_32_soft_status->mode_counters[MODE_32] = sp_ifmd_32_soft_status->mode_counter_32;
    sp_ifmd_32_soft_status->mode_counters[MODE_2332] = sp_ifmd_32_soft_status->mode_counter_2332;
    sp_ifmd_32_soft_status->mode_counters[MODE_32322] = sp_ifmd_32_soft_status->mode_counter_32322;
    sp_ifmd_32_soft_status->mode_counters[MODE_11_2_3] = sp_ifmd_32_soft_status->mode_counter11_2_3;

    for (i = 0; i < TOTAL_MODES; i++) {
        sp_ifmd_32_soft_status->last_trend[i] = 0; /* 0, api para: default value */
        sp_ifmd_32_soft_status->d_max_pcc_per_mode[i] = MIN_UINT;
        sp_ifmd_32_soft_status->max_pcc_phase[i] = 0;  /* 0, api para: default value */
        sp_ifmd_32_soft_status->mode_locked[i] = 0;    /* 0, api para: default value */
        sp_ifmd_32_soft_status->unlock_counter[i] = 0; /* 0, api para: default value */

        for (j = 0; j < phase[i]; j++) {
            sp_ifmd_32_soft_status->mode_counters[i][j] = 0; /* 0, api para: default value */
        }
    }

    sp_ifmd_32_soft_status->flag_adjust = 0;               /* 0, api para: default value */
    sp_ifmd_32_soft_status->scene_change = 0;              /* 0, api para: default value */
    sp_ifmd_32_soft_status->ov_title_det_method = 0;       /* 0, api para: default value */
    sp_ifmd_32_soft_status->final_lock_mode_dei = 0;       /* 0, api para: default value */
    sp_ifmd_32_soft_status->b_comb_large_sharp = 0;        /* 0, api para: default value */
    sp_ifmd_32_soft_status->mask_32_cnt = 0;               /* 0, api para: default value */
    sp_ifmd_32_soft_status->mask_22_cnt = 0;               /* 0, api para: default value */
    sp_ifmd_32_soft_status->mask_1123_cnt = 0;             /* 0, api para: default value */
    sp_ifmd_32_soft_status->mask_any_cnt = 0;              /* 0, api para: default value */
    sp_ifmd_32_soft_status->text_cnt = 0;                  /* 0, api para: default value */
    sp_ifmd_32_soft_status->y_cnt = 0;                     /* 0, api para: default value */
    sp_ifmd_32_soft_status->x_cnt = 0;                     /* 0, api para: default value */
    sp_ifmd_32_soft_status->other_mode_cnt = 0;            /* 0, api para: default value */
    sp_ifmd_32_soft_status->el_32_mask = 0;                /* 0, api para: default value */
    sp_ifmd_32_soft_status->field_num = 0;                 /* 0, api para: default value */
    sp_ifmd_32_soft_status->scene_change_counter = 0;      /* 0, api para: default value */
    sp_ifmd_32_soft_status->mode = 0;                      /* 0, api para: default value */
    sp_ifmd_32_soft_status->final_dei_mode_include_22 = 0; /* 0, api para: default value */
    sp_ifmd_32_soft_status->om_lock_counter = 0;           /* 0, api para: default value */
    sp_ifmd_32_soft_status->om_unlock_counter = 0;         /* 0, api para: default value */
    sp_ifmd_32_soft_status->other_mode_lock_counter = 0;   /* 0, api para: default value */
    sp_ifmd_32_soft_status->other_mode_unlock_counter = 0; /* 0, api para: default value */
    sp_ifmd_32_soft_status->final_om_dei = 0;              /* 0, api para: default value */

    sp_ifmd_22_softs_tatus->pld_22_ctx.hd_flag = u32_ratio > 5;    /* u32_ratio > 5, api para: default value */
    sp_ifmd_22_softs_tatus->pld_22_ctx_be.hd_flag = u32_ratio > 5; /* u32_ratio > 5, api para: default value */
    sp_ifmd_22_softs_tatus->pld_22_ctx.phase_lock_cnt = 0;         /* 0, api para: default value */
    sp_ifmd_22_softs_tatus->pld_22_ctx_be.phase_lock_cnt = 0;      /* 0, api para: default value */
    sp_ifmd_22_softs_tatus->pld_22_ctx.pld_22_lock = 0;            /* 0, api para: default value */
    sp_ifmd_22_softs_tatus->pld_22_ctx_be.pld_22_lock = 0;         /* 0, api para: default value */

    sp_ifmd_32_soft_ctrl->mode_32_en = 1;       /* 1, api para: default value */
    sp_ifmd_32_soft_ctrl->mode_2332_en = 1;     /* 1, api para: default value */
    sp_ifmd_32_soft_ctrl->mode_32322_en = 1;    /* 1, api para: default value */
    sp_ifmd_32_soft_ctrl->mode_22_en = 1;       /* 1, api para: default value */
    sp_ifmd_32_soft_ctrl->mode_55_64_87_en = 1; /* 1, api para: default value */
    sp_ifmd_32_soft_ctrl->mode_11_2_3_en = 1;   /* 1, api para: default value */

    sp_ifmd_32_soft_ctrl->ifmd_det_buff_size = BUFF_SIZE;
    sp_ifmd_32_soft_ctrl->field_order = TOP_FIRST;
    sp_ifmd_32_soft_ctrl->polarity_last = BOTTOM_FIELD_FMD;
    sp_ifmd_32_soft_ctrl->sv_buff_ctrl.low_times = 2; /* 2, api para: default value */
    sp_ifmd_32_soft_ctrl->sv_buff_ctrl.mid_times = 3; /* 3, api para: default value */
    sp_ifmd_32_soft_ctrl->sv_buff_ctrl.high_times = 4; /* 4, api para: default value */

    sp_ifmd_32_soft_ctrl->sv_buff_ctrl.sigma_thr_0 = 10 * u32_ratio;    /* 10 * u32_ratio, default value */
    sp_ifmd_32_soft_ctrl->sv_buff_ctrl.sigma_thr_1 = 100 * u32_ratio;    /* 100 * u32_ratio, default value */
    sp_ifmd_32_soft_ctrl->sv_buff_ctrl.sigma_thr_2 = 1000 * u32_ratio; /* 1000 * u32_ratio, default value */
    sp_ifmd_32_soft_ctrl->sv_buff_ctrl.sigma_thr_3 = 10000 * u32_ratio; /* 10000 * u32_ratio, default value */
    sp_ifmd_32_soft_ctrl->sv_buff_ctrl.sigma_thr_4 = 100000 * u32_ratio; /* 100000 * u32_ratio, def value */

    sp_ifmd_32_soft_ctrl->ss_buff_ctrl.still_divi_thr = 5; /* 5, api para: default value */
    sp_ifmd_32_soft_ctrl->ss_buff_ctrl.comb_chg_mult_thr = 2;  /* 2, api para: default value */
    sp_ifmd_32_soft_ctrl->ss_buff_ctrl.still_scen_thr = 50 * u32_ratio; /* 50 * u32_ratio, api para: default value */
    sp_ifmd_32_soft_ctrl->ss_buff_ctrl.ss_max_thr = 2000 * u32_ratio; /* 2000 * u32_ratio, api para: default value */
    sp_ifmd_32_soft_ctrl->ss_buff_ctrl.ss_thr_0 = 10 * u32_ratio; /* 10 * u32_ratio, api para: default value */
    sp_ifmd_32_soft_ctrl->ss_buff_ctrl.ss_thr_1 = 100 * u32_ratio; /* 100 * u32_ratio, api para: default value */
    sp_ifmd_32_soft_ctrl->ss_buff_ctrl.still_add_thr = 100 * u32_ratio; /* 100 * u32_ratio, api para: default value */
    sp_ifmd_32_soft_ctrl->ss_buff_ctrl.comb_chg_add_thr = 400 * u32_ratio; /* 400 * u32_ratio, api para: def value */

    sp_ifmd_32_soft_ctrl->mmm_buff_ctrl.mmm_divi_thr = 5; /* 5, api para: default value */
    sp_ifmd_32_soft_ctrl->mmm_buff_ctrl.mmm_add_thr = 50 * u32_ratio; /* 50 * u32_ratio, api para: default value */

    sp_ifmd_32_soft_ctrl->nm_buff_ctrl.non_monotony_hist_range_start = 1; /* 1, api para: default value */
    sp_ifmd_32_soft_ctrl->nm_buff_ctrl.non_monotony_hist_range_end = 63;  /* 63, api para: default value */
    sp_ifmd_32_soft_ctrl->nm_buff_ctrl.non_monotony_thr = 0;  /* 0, api para: default value */

    // actually 1080i: thr > 12
    sp_ifmd_32_soft_ctrl->err_line_thr = u32_ratio < 5 ? 7 : 15; /* u32_ratio < 5 ? 7 : 15, api para: default value */

    sp_ifmd_32_soft_ctrl->a_lock_counter_thds[MODE_32] = 12;     /* 12, api para: default value */
    sp_ifmd_32_soft_ctrl->a_lock_counter_thds[MODE_2332] = 20;   /* 20, api para: default value */
    sp_ifmd_32_soft_ctrl->a_lock_counter_thds[MODE_32322] = 26;  /* 26, api para: default value */
    sp_ifmd_32_soft_ctrl->a_lock_counter_thds[MODE_11_2_3] = 27; /* 27, api para: default value */

    sp_ifmd_32_soft_ctrl->a_unlock_counter_thds[MODE_32] = 10;     /* 10, api para: default value */
    sp_ifmd_32_soft_ctrl->a_unlock_counter_thds[MODE_2332] = 20;   /* 20, api para: default value */
    sp_ifmd_32_soft_ctrl->a_unlock_counter_thds[MODE_32322] = 20;  /* 20, api para: default value */
    sp_ifmd_32_soft_ctrl->a_unlock_counter_thds[MODE_11_2_3] = 20; /* 20, api para: default value */

    sp_ifmd_32_soft_ctrl->sigma_bin_start = 8; /* 8, api para: default value */
    sp_ifmd_32_soft_ctrl->comb_bin_start = 0;  /* 0, api para: default value */

    sp_ifmd_22_soft_ctrl->it_diff_mean_thd = 5000; /* 5000, api para: default value */

    sp_ifmd_22_soft_ctrl->locker_ctrl.hist_check_dec = 15;   /* 15, api para: default value */
    sp_ifmd_22_soft_ctrl->locker_ctrl.enter_lock_level = 25; /* 25, api para: default value */
    sp_ifmd_22_soft_ctrl->locker_ctrl.exit_lock_level = 15;  /* 15, api para: default value */

    sp_ifmd_22_soft_ctrl->tkr_ctrl.tkr_thr_level = u32_ratio * 100;  /* u32_ratio * 100, api para: default value */
    sp_ifmd_22_soft_ctrl->tkr_ctrl.ovlp_title_thr = u32_ratio * 100; /* u32_ratio * 100, api para: default value */

    sp_ifmd_22_soft_ctrl->pcc_ctrl.max_still_pcc = u32_ratio * 5;        /* u32_ratio * 5, api para: default value */
    sp_ifmd_22_soft_ctrl->pcc_ctrl.min_usable_pcc = u32_ratio * 120;     /* u32_ratio * 120, api para: default value */
    sp_ifmd_22_soft_ctrl->pcc_ctrl.non_match_crss_thr = u32_ratio * 800; /* u32_ratio * 800, api para: default value */
    sp_ifmd_22_soft_ctrl->pcc_ctrl.max_non_match_pcc = u32_ratio * 800;  /* u32_ratio * 800, api para: default value */

    sp_ifmd_22_soft_ctrl->um_ctrl.still_um_thr = u32_ratio * 500; /* u32_ratio * 500,  default value */
    sp_ifmd_22_soft_ctrl->um_ctrl.lower_non_match_thr_0 = u32_ratio * 1000; /* 1000, default value */
    sp_ifmd_22_soft_ctrl->um_ctrl.lower_non_match_thr_1 = u32_ratio * 80000; /* 80000, def value */
    sp_ifmd_22_soft_ctrl->um_ctrl.upper_match_thr = u32_ratio * 8000; /* 8000, default value */
    sp_ifmd_22_soft_ctrl->um_ctrl.non_match_ratio = 8; /* 8, default value */
    return HI_SUCCESS;
}

hi_s32 pq_mng_get_film_mode(hi_u32 *p_film_mode)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(p_film_mode);

    *p_film_mode = g_film_mode;

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
