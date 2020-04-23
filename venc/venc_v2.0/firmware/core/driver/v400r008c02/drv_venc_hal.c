/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-04-22
 */
#include "drv_venc_osal_ext.h"
#include "venc_hal_ext.h"
#include "drv_venc_reg_96cv300.h"
#include "drv_venc_hal.h"
#include "public.h"
#include "hi_type.h"
#include "sysconfig.h"
#include <linux/kernel.h>
#include <linux/delay.h>

#define TMP_MIN(a, b)            (((a) > (b)) ? (b) : (a))
#define TMP_MAX(a, b)            (((a) < (b)) ? (b) : (a))
#define TMP_CLIP3(low, high, x)  (TMP_MAX(TMP_MIN((x), high), low))
#define DDR 1

static venc_hal_reg_info *g_hal_reg_info = HI_NULL;

extern venc_osal_func_ptr g_venc_osal_func_ptr;

static hi_u32 *g_reg_base = HI_NULL;

static hi_void set_other_reg_info(venc_hal_static_cfg* reg_default_cfg)
{
    reg_default_cfg->w_out_std_num = 0x7;
    reg_default_cfg->r_out_std_num = 0x3f;

#ifdef HI_SMMU_SUPPORT
    reg_default_cfg->glb_bypass = 0;
    reg_default_cfg->int_en = 1;
#else
    reg_default_cfg->glb_bypass = 1;
    reg_default_cfg->int_en = 0;
    reg_default_cfg->vcpi_srcy_bypass = 1;
    reg_default_cfg->vcpi_srcv_bypass = 1;
    reg_default_cfg->vcpi_refy_bypass = 1;
    reg_default_cfg->vcpi_refc_bypass = 1;
    reg_default_cfg->vcpi_strm_bypass = 1;
#endif

    reg_default_cfg->clk_gate_en = 0x2;  /* open the frame level and MB level clk_gate */
    reg_default_cfg->mem_clk_gate_en = 1;
    reg_default_cfg->time_out = 40000000; /* 40000000: time out period */
    reg_default_cfg->pt_bits = 500000 * 8; /* 500000: stream bits; 8 : 1 byte */
}

static hi_void set_other_reg_info_dynamic(venc_hal_dynamic_cfg* reg_default_cfg, hi_u32 protocol)
{
    hi_u32 i;
    if (protocol == VEDU_H264) {
        for (i =  0; i < 40; i++) {  /* 40 : size of g_h264_lambda_ssd */
            reg_default_cfg->qpg_cfg_dynamic.lambda[i] = g_h264_lambda_ssd[i];
        }
        for (i = 40; i < 80; i++) { /* 40, 80 : size of g_h264_lambda_ssd */
            reg_default_cfg->qpg_cfg_dynamic.lambda[i] = g_h264_lambda_sad[i - 40]; /* 40 : size of g_h265_lambda_ssd */
        }
    } else {
        for (i =  0; i < 40; i++) { /* 40 : size of g_h265_lambda_ssd */
            reg_default_cfg->qpg_cfg_dynamic.lambda[i] = g_h265_lambda_ssd[i   ];
        }
        for (i = 40; i < 80; i++) { /* 40, 80 : size of g_h265_lambda_ssd */
            reg_default_cfg->qpg_cfg_dynamic.lambda[i] = g_h265_lambda_sad[i - 40]; /* 40 : size of g_h265_lambda_ssd */
        }
    }
    reg_default_cfg->vcpi_entropy_mode = 0;
    reg_default_cfg->vcpi_trans_mode = 0;
    reg_default_cfg->vcpi_intra_cu_en = 0xf;
}

#ifdef VENC_SUPPORT_ROI
static hi_void set_roi_info(roi_attr *roi_attr)
{
    hi_u32 i = 0;
    for (i = 0; i < 8; i++) { /* 8: array size */
        roi_attr->enable [i] = 0;
        roi_attr->keep   [i] = 0;
        roi_attr->abs_qp_en[i] = 1;
        roi_attr->qp     [i] = 26; /* 26 : init qp val */
        roi_attr->width  [i] = 0x7;
        roi_attr->height [i] = 0x7;
        roi_attr->start_x [i] = i * 5; /* 5 */
        roi_attr->start_y [i] = i * 5; /* 5 */
    }
}
#endif

static hi_void set_img_improve_info(img_improve_attr_static *img_improve_attr)
{
    img_improve_attr->norm32_tr1_denois_max_num = 0x4;
    img_improve_attr->norm32_coeff_protect_num = 0x100;
    img_improve_attr->norm16_coeff_protect_num = 0x80;

    img_improve_attr->skin32_tr1_denois_max_num = 0x4;
    img_improve_attr->skin32_coeff_protect_num = 0x80;
    img_improve_attr->skin16_tr1_denois_max_num = 0x3;

    img_improve_attr->static32_tr1_denois_max_num = 0x8;
    img_improve_attr->static32_coeff_protect_num = 0x20;

    img_improve_attr->hedge32_tr1_denois_max_num = 0x8;
    img_improve_attr->hedge32_coeff_protect_num = 0x20;

    img_improve_attr->edgemov32_tr1_denois_max_num = 0x6;
    img_improve_attr->edgemov32_coeff_protect_num = 0x20;

    img_improve_attr->rounding_skin_mechanism = 1;
    img_improve_attr->rounding_edge_mechanism = 1;
    img_improve_attr->rounding_edge_mov_mechanism = 1;
    img_improve_attr->skin_en = 0;
    img_improve_attr->img_improve_en = 1;
}

static hi_void set_img_improve_info_dynamic(img_improve_attr_dynamic *img_improve_attr)
{
    img_improve_attr->highedge_en = 0;
    img_improve_attr->static_en = 0;
    img_improve_attr->norm16_tr1_denois_max_num = 0x3;
    img_improve_attr->skin16_coeff_protect_num = 0x40;

    img_improve_attr->static16_tr1_denois_max_num = 0x4;
    img_improve_attr->static16_coeff_protect_num = 0x10;

    img_improve_attr->hedge16_tr1_denois_max_num = 0x4;
    img_improve_attr->hedge16_coeff_protect_num = 0x10;

    img_improve_attr->edgemov16_tr1_denois_max_num = 0x4;
    img_improve_attr->edgemov16_coeff_protect_num = 0x10;

    img_improve_attr->rounding_still_mechanism = 1;

    img_improve_attr->skin_inter_cu_rdcost_offset = 0x100;
}

static hi_void set_qpg_info_01(qpg_attr_static *qpg_attr)
{
    hi_u32 i;

    for (i = 0; i < 16; i++) { /* 16: array size */
        qpg_attr->qp_delta_level[i] = 1;
    }

    qpg_attr->qp_madi_switch_thr = 0x8;

    /* low luma */
    qpg_attr->lowluma_qp_delta = 0x4;
    qpg_attr->lowluma_max_qp = 0x33;
    qpg_attr->lowluma_min_qp = 0;

    /* strong_edge */
    qpg_attr->strong_edge_qp_delta = 0;
    qpg_attr->strong_edge_max_qp = 51; /* 51: max qp */
    qpg_attr->strong_edge_min_qp = 0;

    /* strong_edge_move */
    qpg_attr->strong_edge_move_qp_delta = 0;
    qpg_attr->strong_edge_move_max_qp = 0;
    qpg_attr->strong_edge_move_min_qp = 0;

    /* large_move */
    qpg_attr->large_move_qp_delta = 0;
    qpg_attr->large_move_max_qp = 51; /* 51: max qp */
    qpg_attr->large_move_min_qp = 0;

    /* intra_det */
    qpg_attr->intra_det_qp_delta = 0x2; /* 2: qp_delta */
    qpg_attr->intra_det_max_qp = 51; /* 51: max qp */
    qpg_attr->intra_det_min_qp = 0;

    return;
}

static hi_void set_qpg_info_02(qpg_attr_static *qpg_attr)
{
    /* base cfg */
    qpg_attr->ave_lcu_bits = 0x6;
    qpg_attr->cu32_delta_low = 0xc;
    qpg_attr->cu32_delta_high = 0xc;
    qpg_attr->rc_cu_qp_en = 1;
    qpg_attr->rc_row_qp_en = 1;
    qpg_attr->rc_cu_madi_en = 1;

    qpg_attr->strong_edge_move_en = 0;
    qpg_attr->intra_det_qp_en = 0;
    qpg_attr->rd_min_sad_flag_en = 0;
    qpg_attr->wr_min_sad_flag_en = 0;
    qpg_attr->prev_min_sad_en = 1;
    qpg_attr->low_min_sad_en = 0;
    qpg_attr->qpgld_en = 0;
    qpg_attr->map_roikeep_en = 0;
    qpg_attr->flat_region_en = 0;

    qpg_attr->qp_restrain_large_sad = 0;
    qpg_attr->lambda_inter_stredge_en = 0;
    qpg_attr->rdo_lambda_choose_mode = 0;
    qpg_attr->lambda_qp_offset = 0x1a;
    qpg_attr->qp_restrain_delta_blk32 = 0x5;
    qpg_attr->qp_restrain_delta_blk16 = 0x5;
    qpg_attr->qp_restrain_mode = 1;
    qpg_attr->qp_restrain_en = 1;
    qpg_attr->qp_restrain_madi_thr = 0x8;
    qpg_attr->min_sad_thresh_low = 0x0c080401;
    qpg_attr->min_sad_thresh_high = 0xB4825A3C;
    qpg_attr->min_sad_qp_restrain_en = 0;
    qpg_attr->min_sad_madi_en = 0;
    qpg_attr->low_min_sad_mode = 0;
    qpg_attr->min_sad_level = 170; /* 170: min_sad_level */
    qpg_attr->strong_edge_move_min_qp = 0;

    qpg_attr->bfg_roi_min_qp = 0;
    qpg_attr->skin_min_qp = 0;

    return;
}

static hi_void set_qpg_info_03(qpg_attr_static *qpg_attr)
{
    hi_u32 i;

    qpg_attr->qp_detlta_size_cu64 = 0;
    qpg_attr->smart_get_cu64_qp_mode = 0;
    qpg_attr->smart_get_cu32_qp_mode = 0;
    qpg_attr->smart_abs_qp_mode = 0;
    qpg_attr->smart_cu_level_qp_mode = 0;
    qpg_attr->stredge_min_qp = 0;
    qpg_attr->large_move_min_qp = 0;
    qpg_attr->intra_det_min_qp = 0;
    qpg_attr->row_target_bits = 0;
    qpg_attr->flat_region_min_qp = 0xa; /* 10: flat_region_min_qp */
    qpg_attr->flat_region_max_qp = 51; /* 51: flat_region_max_qp */
    qpg_attr->flat_region_qp_delta = 0x4;
    qpg_attr->vcpi_res_coef_en = 0;
    qpg_attr->vcpi_large_res_coef = 0x8;
    qpg_attr->vcpi_small_res_coef = 0x8;

    /* curr_sad */
    qpg_attr->vcpi_rc_cu_sad_en = 0;
    qpg_attr->vcpi_sad_switch_thr = 0x8;
    qpg_attr->vcpi_rc_cu_sad_mod = 0x2;
    qpg_attr->vcpi_rc_cu_sad_offset = 0x8;
    qpg_attr->vcpi_rc_cu_sad_gain = 0x8;

    for (i = 0;i < 16; i++) { /* 16: array size */
        qpg_attr->vcpi_curr_sad_level[i] = 1;
    }
    qpg_attr->vcpi_curr_sad_thresh[0] = 0;
    qpg_attr->vcpi_curr_sad_thresh[1] = 0;
    qpg_attr->vcpi_curr_sad_thresh[2] = 0; /* 2: array index */
    qpg_attr->vcpi_curr_sad_thresh[3] = 0; /* 3: array index */
    qpg_attr->vcpi_curr_sad_thresh[4] = 0x3; /* 4: array index */
    qpg_attr->vcpi_curr_sad_thresh[5] = 0x3; /* 5: array index */
    qpg_attr->vcpi_curr_sad_thresh[6] = 0x5; /* 6: array index */
    qpg_attr->vcpi_curr_sad_thresh[7] = 0x5; /* 7: array index */
    qpg_attr->vcpi_curr_sad_thresh[8] = 0x8; /* 8: array index */
    qpg_attr->vcpi_curr_sad_thresh[9] = 0x8; /* 9: array index */
    qpg_attr->vcpi_curr_sad_thresh[10] = 0x8; /* 10: array index */
    qpg_attr->vcpi_curr_sad_thresh[11] = 0xf; /* 11: array index */
    qpg_attr->vcpi_curr_sad_thresh[12] = 0x14; /* 12: array index */
    qpg_attr->vcpi_curr_sad_thresh[13] = 0x14; /* 13: array index */
    qpg_attr->vcpi_curr_sad_thresh[14] = 0x19; /* 14: array index */
    qpg_attr->vcpi_curr_sad_thresh[15] = 0x19; /* 15: array index */

    return;
}

static hi_void set_qpg_info_04(qpg_attr_static *qpg_attr)
{
    hi_u32 i;

    /* rc_luma */
    qpg_attr->vcpi_rc_luma_en = 0;
    qpg_attr->vcpi_rc_luma_mode          = 0;
    qpg_attr->vcpi_rc_luma_switch_thr    = 0x8;
    qpg_attr->vcpi_rc_luma_low_madi_thr  = 0;
    qpg_attr->vcpi_rc_luma_high_madi_thr = 40; /* 40: vcpi_rc_luma_high_madi_thr */
    for (i = 0; i < 16; i++) { /* 16: level num */
        if (i < 2) { /* 2: set 0 */
            qpg_attr->vcpi_rc_luma_level[i] = 0;
        }
        qpg_attr->vcpi_rc_luma_level[i] = 1;
    }
    qpg_attr->vcpi_rc_luma_thresh[0] =  1;
    qpg_attr->vcpi_rc_luma_thresh[1] =  1;
    qpg_attr->vcpi_rc_luma_thresh[2] = 0x14; /* 2: array index */
    qpg_attr->vcpi_rc_luma_thresh[3] = 0x14; /* 3: array index */
    qpg_attr->vcpi_rc_luma_thresh[4] = 0x28; /* 4: array index */
    qpg_attr->vcpi_rc_luma_thresh[5] = 0x28; /* 5: array index */
    qpg_attr->vcpi_rc_luma_thresh[6] = 0x32; /* 6: array index */
    qpg_attr->vcpi_rc_luma_thresh[7] = 0x32; /* 7: array index */
    qpg_attr->vcpi_rc_luma_thresh[8] = 0xc8; /* 8: array index */
    qpg_attr->vcpi_rc_luma_thresh[9] = 0xc8; /* 9: array index */
    qpg_attr->vcpi_rc_luma_thresh[10] = 0xe6; /* 10: array index */
    qpg_attr->vcpi_rc_luma_thresh[11] = 0xe6; /* 11: array index */
    qpg_attr->vcpi_rc_luma_thresh[12] = 0xff; /* 12: array index */
    qpg_attr->vcpi_rc_luma_thresh[13] = 0xff; /* 13: array index */
    qpg_attr->vcpi_rc_luma_thresh[14] = 0xff; /* 14: array index */
    qpg_attr->vcpi_rc_luma_thresh[15] = 0xff; /* 15: array index */

    /* chroma prot */
    qpg_attr->vcpi_chroma_prot_en = 0;
    qpg_attr->vcpi_chroma_max_qp = 51; /* 51: vcpi_chroma_max_qp */
    qpg_attr->vcpi_chroma_min_qp = 0xa; /* 10: vcpi_chroma_min_qp */
    qpg_attr->vcpi_chroma_qp_delta = 0x3;

    qpg_attr->qpg_rc_max_qp = 51;  /* 51: qpg_rc_max_qp */
    qpg_attr->qpg_rc_min_qp = 0;

    return;
}

static hi_void set_qpg_info(qpg_attr_static *qpg_attr)
{
    set_qpg_info_01(qpg_attr);
    set_qpg_info_02(qpg_attr);
    set_qpg_info_03(qpg_attr);
    set_qpg_info_04(qpg_attr);

    return;
}

static hi_void set_qpg_info_dynamic(qpg_attr_dynamic *qpg_attr, hi_u32 protocol)
{
    if (protocol == VEDU_H264) {
        qpg_attr->qp_delta = 0x2 ;
        qpg_attr->cu_qp_delta_thresh[3] = 0x0 ; /* 3: array index */
        qpg_attr->cu_qp_delta_thresh[2] = 0x0 ; /* 2: array index */
        qpg_attr->cu_qp_delta_thresh[1] = 0x0 ;
        qpg_attr->cu_qp_delta_thresh[0] = 0x0 ;
        qpg_attr->cu_qp_delta_thresh[7] = 0x5 ; /* 7: array index */
        qpg_attr->cu_qp_delta_thresh[6] = 0x5 ; /* 6: array index */
        qpg_attr->cu_qp_delta_thresh[5] = 0x3 ; /* 5: array index */
        qpg_attr->cu_qp_delta_thresh[4] = 0x3 ; /* 4: array index */
        qpg_attr->cu_qp_delta_thresh[11] = 0xf ; /* 11: array index */
        qpg_attr->cu_qp_delta_thresh[10] = 0x8 ; /* 10: array index */
        qpg_attr->cu_qp_delta_thresh[9] = 0x8 ; /* 9: array index */
        qpg_attr->cu_qp_delta_thresh[8] = 0x8 ; /* 8: array index */
        qpg_attr->cu_qp_delta_thresh[15] = 0x19 ; /* 15: array index */
        qpg_attr->cu_qp_delta_thresh[14] = 0x19 ; /* 14: array index */
        qpg_attr->cu_qp_delta_thresh[13] = 0x14 ; /* 13: array index */
        qpg_attr->cu_qp_delta_thresh[12] = 0x14 ; /* 12: array index */

        qpg_attr->skin_qp_delta  = 0x3;
        qpg_attr->skin_max_qp  = 51; /* 51: skin_max_qp */
        qpg_attr->move_scene_en = 1;
        qpg_attr->move_sad_en = 1;
    } else {
        qpg_attr->qp_delta = 0x2;
        qpg_attr->cu_qp_delta_thresh[2] = 0; /* 2: array index */
        qpg_attr->cu_qp_delta_thresh[3] = 0; /* 3: array index */
        qpg_attr->cu_qp_delta_thresh[4] = 0; /* 4: array index */
        qpg_attr->cu_qp_delta_thresh[5] = 0x3; /* 5: array index */
        qpg_attr->cu_qp_delta_thresh[6] = 0x5; /* 6: array index */
        qpg_attr->cu_qp_delta_thresh[7] = 0x5; /* 7: array index */
        qpg_attr->cu_qp_delta_thresh[8] = 0xff; /* 8: array index */
        qpg_attr->cu_qp_delta_thresh[9] = 0xff; /* 9: array index */
        qpg_attr->cu_qp_delta_thresh[10] = 0xff; /* 10: array index */
        qpg_attr->cu_qp_delta_thresh[11] = 0xff; /* 11: array index */
        qpg_attr->cu_qp_delta_thresh[12] = 0xff; /* 12: array index */
        qpg_attr->cu_qp_delta_thresh[13] = 0xff; /* 13: array index */
        qpg_attr->cu_qp_delta_thresh[14] = 0xff; /* 14: array index */
        qpg_attr->cu_qp_delta_thresh[15] = 0xff; /* 15: array index */
    }

    qpg_attr->skin_qp_delta = 0x3;
    qpg_attr->skin_max_qp = 51; /* 51: skin_max_qp */

    qpg_attr->move_scene_en = 0;
}

static hi_void set_pme_info_01(pme_attr_static *pme_attr)
{
    pme_attr->l0_psw_adapt_en = 0;
    pme_attr->l1_psw_adapt_en = 0;

    pme_attr->l0_psw_thr0 = 0x14;
    pme_attr->l0_psw_thr1 = 0x96;
    pme_attr->l0_psw_thr2 = 0x12c;
    pme_attr->l1_psw_thr0 = 0x14;
    pme_attr->l1_psw_thr1 = 0x96;
    pme_attr->l1_psw_thr2 = 0x12c;

    pme_attr->skipblk_pre_en = 0;
    pme_attr->skipblk_pre_cost_thr = 0x96;
    pme_attr->tr_weightx_2 = 0x30;
    pme_attr->tr_weightx_1 = 0x20;
    pme_attr->tr_weightx_0 = 0x10;
    pme_attr->tr_weighty_2 = 0x30;
    pme_attr->tr_weighty_1 = 0x20;
    pme_attr->tr_weighty_0 = 0x10;
    pme_attr->sr_weight_2 = 1;
    pme_attr->sr_weight_1 = 0x2;
    pme_attr->sr_weight_0 = 0x4;

    pme_attr->skin_v_min_thr = 0x87;
    pme_attr->skin_v_max_thr = 0xa0;
    pme_attr->skin_u_min_thr = 0x64;
    pme_attr->skin_u_max_thr = 0x7f;
    pme_attr->pme_intra16_madi_thr = 0x10;
    pme_attr->pme_intra32_madi_thr = 0x14;
    pme_attr->move_scene_thr = 0x3;
    pme_attr->cost_lamda_en = 0;
    pme_attr->new_cost_en = 1;
    pme_attr->cost_lamda2 = 0;
    pme_attr->cost_lamda1 = 0x2;
    pme_attr->cost_lamda0 = 0x6;
    pme_attr->l0_win0_width  = 0x8;
    pme_attr->l0_win0_height = 0x8;
    pme_attr->l1_win0_width  = 0x8;
    pme_attr->l1_win0_height = 0x8;
    pme_attr->l1_cost_offset = 0;
    pme_attr->l0_cost_offset = 0;
    pme_attr->pme_iblk_pre_cost_thr_h264 = 0x400;
    pme_attr->pme_intrablk_det_cost_thr0 = 0x1ff;
    pme_attr->pme_intrablk_det_cost_thr1 = 0;
    pme_attr->pme_intrablk_det_mv_dif_thr0 = 0xf;
    pme_attr->pme_intrablk_det_mv_dif_thr1 = 0xf;
    pme_attr->pme_intrablk_det_mvx_thr = 0x5;
    pme_attr->pme_intrablk_det_mvy_thr = 0x5;

    return;
}

static hi_void set_pme_info_02(pme_attr_static *pme_attr)
{
    pme_attr->pme_safe_line = 0;
    pme_attr->pme_safe_line_val = 0;
    pme_attr->pme_safe_line_mode = 0;
    pme_attr->pme_iblk_refresh_en = 0;
    pme_attr->pme_iblk_refresh_mode = 0;
    pme_attr->pme_iblk_refresh_start_num = 0;
    pme_attr->pme_iblk_refresh_num = 0;

    pme_attr->pme_high_luma_thr = 0x3c;
    pme_attr->low_luma_thr = 0;
    pme_attr->low_luma_madi_thr = 0x4;
    pme_attr->madi_dif_thr = 0x5;
    pme_attr->cur_madi_dif_thr = 0x5;
    pme_attr->min_sad_thr_gain = 0;
    pme_attr->min_sad_thr_offset = 0;

    pme_attr->pme_min_sad_thr_gain_cur = 0;
    pme_attr->pme_min_sad_thr_offset_cur = 0;

    pme_attr->pskip_mvy_consistency_thr	= 0;
    pme_attr->pskip_mvx_consistency_thr	= 0;
    pme_attr->vcpi_strong_edge_thr_u = 1;
    pme_attr->vcpi_strong_edge_cnt_u = 1;
    pme_attr->vcpi_strong_edge_thr_v = 0x3;
    pme_attr->vcpi_strong_edge_cnt_v = 0x2;
    pme_attr->vcpi_chroma_sad_thr_offset = 0x4;
    pme_attr->vcpi_chroma_sad_thr_gain = 0x4;
    pme_attr->vcpi_chroma_u0_thr_min = 0x64;
    pme_attr->vcpi_chroma_u0_thr_max = 0x87;
    pme_attr->vcpi_chroma_v0_thr_min = 0x8c;
    pme_attr->vcpi_chroma_v0_thr_max = 0xe6;
    pme_attr->vcpi_chroma_u1_thr_min = 0x74;
    pme_attr->vcpi_chroma_u1_thr_max = 0x8c;
    pme_attr->vcpi_chroma_v1_thr_min = 0x78;
    pme_attr->vcpi_chroma_v1_thr_max = 0x88;
    pme_attr->vcpi_chroma_uv0_thr_min = 0x10c;
    pme_attr->vcpi_chroma_uv0_thr_max = 0x14a;
    pme_attr->vcpi_chroma_uv1_thr_min = 0x64;
    pme_attr->vcpi_chroma_uv1_thr_max = 0x104;
    pme_attr->vcpi_chroma_count0_thr_min = 0x10;
    pme_attr->vcpi_chroma_count0_thr_max = 0xf0;
    pme_attr->vcpi_chroma_count1_thr_min = 0x10;
    pme_attr->vcpi_chroma_count1_thr_max = 0xf0;

    return;
}

static hi_void set_pme_info_03(pme_attr_static *pme_attr, hi_u32 protocol)
{
    pme_attr->vcpi_move_scene_mv_thr = 0x4;
    pme_attr->vcpi_move_scene_mv_en = 1;
    pme_attr->pme_skip_sad_thr_offset = 0x8;
    pme_attr->pme_skip_sad_thr_gain = 0x8;
    pme_attr->pme_skip_large_res_det = 0;
    pme_attr->vcpi_skin_sad_thr_offset = 0x4;
    pme_attr->vcpi_skin_sad_thr_gain = 0;
    pme_attr->pme_adjust_pmemv_dist_times = 0x4;
    pme_attr->pme_adjust_pmemv_en = 0;
    pme_attr->vcpi_new_madi_th0 = 0x5;
    pme_attr->vcpi_new_madi_th1 = 0xa;
    pme_attr->vcpi_new_madi_th2 = 0xf;
    pme_attr->vcpi_new_madi_th3 = 0x14;
    pme_attr->vcpi_new_lambda = 0x5a;
    pme_attr->vcpi_pskip_strongedge_madi_thr = 0x14;
    pme_attr->vcpi_pskip_strongedge_madi_times = 0x3;
    pme_attr->vcpi_pskip_flatregion_madi_thr = 0x5;
    pme_attr->pme_psw_lp_diff_thx = 0x5;
    pme_attr->pme_psw_lp_diff_thy = 0x5;
    pme_attr->pme_pblk_pre_madi_times = 0x5;
    pme_attr->pme_pblk_pre_offset = 0x5;
    pme_attr->pme_pblk_pre_gain = 0x5;
    pme_attr->pme_pblk_pre_mv_dif_cost_thr = 0x3e8;
    pme_attr->pme_pblk_pre_mv_dif_thr0 = 1;
    pme_attr->pme_pblk_pre_mv_dif_thr1 = 1;

    if (protocol == VEDU_H264) {
        pme_attr->tr_weightx_2 = 0x30;
        pme_attr->tr_weightx_1 = 0x20;
        pme_attr->tr_weightx_0 = 0x10;
        pme_attr->tr_weighty_2 = 0x30;
        pme_attr->tr_weighty_1 = 0x20;
        pme_attr->tr_weighty_0 = 0x10;
        pme_attr->sr_weight_2  = 0x4;
        pme_attr->sr_weight_1  = 0x2;
        pme_attr->sr_weight_0  = 1;
        pme_attr->pme_skip_sad_thr_offset = 0x6;
        pme_attr->pme_skip_sad_thr_gain	= 0x4;
        pme_attr->pme_skip_large_res_det = 1;
        pme_attr->vcpi_chroma_sad_thr_gain = 0x2;
        pme_attr->vcpi_chroma_sad_thr_offset = 0x2;
    }
}

static hi_void set_pme_info(pme_attr_static *pme_attr, hi_u32 protocol)
{
    set_pme_info_01(pme_attr);
    set_pme_info_02(pme_attr);
    set_pme_info_03(pme_attr, protocol);

    return;
}

static hi_void set_pme_info_dynamic(pme_attr_dynamic *pme_attr)
{
    pme_attr->skin_num = 0xc8;
    pme_attr->still_scene_thr = 0x2;

    pme_attr->high_edge_cnt = 0xd;
    pme_attr->high_edge_thr = 0xa;
    pme_attr->move_sad_thr = 0x100;
    pme_attr->interstrongedge_madi_thr = 0x40;
    pme_attr->interdiff_max_min_madi_times = 0x3 ;
    pme_attr->interdiff_max_min_madi_abs   = 0x8 ;
}

static hi_void set_base_info_01(base_attr *base_attr)
{
    base_attr->vcpi_time_en = 1;
    base_attr->vedu_timeout = 1;

    base_attr->intra_period = 1;
    base_attr->vcpi_bp_lcu_y = 1;
    base_attr->vcpi_bp_lcu_x = 1;

    base_attr->vcpi_mrg_cu_en = 0xf;
    base_attr->vcpi_fme_cu_en = 0xf;

    base_attr->vcpi_inter8x8_en = 1;
    base_attr->vcpi_pskp_en = 1;
    base_attr->vcpi_idr_pic = 1;

    base_attr->vcpi_tmv_wr_rd_avail = 0x3;
    base_attr->vcpi_cross_slice = 1;

    base_attr->intra_cu4_mode0_31_en = 0xFFFFFFFF;
    base_attr->intra_cu4_mode32_34_en = 0x7 ;
    base_attr->intra_cu8_mode0_31_en = 0xFFFFFFFF;
    base_attr->intra_cu8_mode32_34_en = 0x7;
    base_attr->intra_cu16_mode0_31_en = 0xFFFFFFFF;
    base_attr->intra_cu16_mode32_34_en = 0x7;
    base_attr->intra_cu32_mode0_31_en = 0xFFFFFFFF;
    base_attr->intra_cu32_mode32_34_en = 0x7;
    base_attr->h264chroma_mode0_en = 1;
    base_attr->h264chroma_mode1_en = 1;
    base_attr->h264chroma_mode2_en = 1;
    base_attr->h264chroma_mode3_en = 1;

    return;
}

static hi_void set_base_info_02(base_attr *base_attr)
{
    base_attr->vcpi_ext_edge_en = 0;
    base_attr->vcpi_iblk_pre_en = 1;
    base_attr->vcpi_pblk_pre_en = 0;

    base_attr->vcpi_pblk_pre_cost_thr = 0x14;
    base_attr->vcpi_iblk_pre_cost_thr = 0x100;
    base_attr->vcpi_iblk_pre_mvy_thr = 0xa0;
    base_attr->vcpi_iblk_pre_mvx_thr = 0xc0;
    base_attr->vcpi_iblk_pre_mv_dif_thr1 = 1;
    base_attr->vcpi_iblk_pre_mv_dif_thr0 = 0xff;

    base_attr->pblk_pre_mvx_thr = 0x10;
    base_attr->pblk_pre_mvy_thr = 0x10;

    base_attr->vcpi_skin_close_angle = 0;
    base_attr->num_short_term_ref_pic_sets = 1 ;

    base_attr->vcpi_lambdaoff16 = 0;
    base_attr->vcpi_lambdaoff8 = 0;

    base_attr->vcpi2cu_qp_min_cu_size = 0x2;
    base_attr->vcpi2pu_log2_max_ipcm_cbsizey = 0x3;
    base_attr->vcpi2pu_log2_min_ipcm_cbsizey = 0x3;

    base_attr->narrow_tile_width = 0x3;

    base_attr->clip_luma_min = 0;
    base_attr->clip_luma_max = 0xff;
    base_attr->clip_chrm_min = 0;
    base_attr->clip_chrm_max = 0xff;

    base_attr->cu_qp_delta_enable_flag = 1;

    base_attr->vcpi_ipcm_en = 0;
    base_attr->vcpi_sao_chroma = 1;
    base_attr->vcpi_sao_luma = 1;
    base_attr->max_num_mergecand = 0x2;
    base_attr->tmv_en = 1;
    base_attr->vcpi_sw_l1_height = 0xf;
    base_attr->vcpi_sw_l1_width = 0x47;
    base_attr->vcpi_selfrst_en  = 1;
    base_attr->intra_bit_weight = 0;

    return;
}

static hi_void set_base_info(base_attr *base_attr)
{
    set_base_info_01(base_attr);
    set_base_info_02(base_attr);

    return;
}

static hi_void set_low_power_info(low_power_attr *low_power_attr)
{
    low_power_attr->vcpi_lowpow_fme_thr1 = 0x14;
    low_power_attr->vcpi_lowpow_fme_thr0 = 0xa;
    low_power_attr->vcpi_fme_lowpow = 1 ;
    low_power_attr->vcpi_ime_lowpow = 1 ;
}

static hi_void set_q_scaling_info(q_scaling_attr *qscaling_attr)
{
    qscaling_attr->deadzone_intra_slice = 0xab;
    qscaling_attr->deadzone_inter_slice = 0x55;
}

#if PARA_ADAPT_ENCODER
static hi_void set_para_normal_region(attr_table *enc_para_table, hi_u32 i, hi_u32 j)
{
    hi_u32 i, j;
    for (i = 0; i < 2; i++) { /* 2: array size */
        g_venc_osal_func_ptr.fun_osal_mem_set(&enc_para_table[i], sizeof(attr_table), 0, sizeof(attr_table));

        for (j = 0; j < 5; j++) { /* 5: array size */
            /* norm region */
            enc_para_table[i].img_improve_cfg_dynamic[j].norm16_tr1_denois_max_num = 0x3;
            enc_para_table[i].pme_cfg_dynamic[j].skin_num = 0x60;
            enc_para_table[i].qpg_cfg_dynamic[j].skin_qp_delta = 0x4;
            enc_para_table[i].img_improve_cfg_dynamic[j].skin_intra_cu16_rdcost_offset = 0x0;
            enc_para_table[i].img_improve_cfg_dynamic[j].skin_inter_cu_rdcost_offset = 0x000;
            enc_para_table[i].img_improve_cfg_dynamic[j].skin16_coeff_protect_num = 0x40 ;

            enc_para_table[i].img_improve_cfg_dynamic[j].highedge_en = 1;
            enc_para_table[i].pme_cfg_dynamic[j].high_edge_cnt = 0x3;
            enc_para_table[i].pme_cfg_dynamic[j].high_edge_thr = 0x14;
            enc_para_table[i].pme_cfg_dynamic[j].interstrongedge_madi_thr = 0x40;
            enc_para_table[i].pme_cfg_dynamic[j].interdiff_max_min_madi_times = 0x4;
            enc_para_table[i].pme_cfg_dynamic[j].interdiff_max_min_madi_abs = 0x8 ;

            enc_para_table[i].img_improve_cfg_dynamic[j].hedge_intra_cu16_rdcost_offset = 0x0;
            enc_para_table[i].img_improve_cfg_dynamic[j].hedge_inter_cu_rdcost_offset = 0x000;
            enc_para_table[i].img_improve_cfg_dynamic[j].hedge16_tr1_denois_max_num = 0x4 ;
            enc_para_table[i].img_improve_cfg_dynamic[j].hedge16_coeff_protect_num = 0x20  ;

            enc_para_table[i].qpg_cfg_dynamic[j].move_scene_en = 1;
            enc_para_table[i].qpg_cfg_dynamic[j].move_sad_en = 0;
            enc_para_table[i].pme_cfg_dynamic[j].move_sad_thr = 0x180;

            enc_para_table[i].img_improve_cfg_dynamic[j].strmov_intra_cu16_rdcost_offset = 0x0;
            enc_para_table[i].img_improve_cfg_dynamic[j].strmov_inter_cu_rdcost_offset = 0x000;
            enc_para_table[i].img_improve_cfg_dynamic[j].edgemov16_tr1_denois_max_num = 0x4 ;
            enc_para_table[i].img_improve_cfg_dynamic[j].edgemov16_coeff_protect_num = 0x20;

            enc_para_table[i].img_improve_cfg_dynamic[j].static_en = 1;
            enc_para_table[i].pme_cfg_dynamic[j].still_scene_thr = 0x2;
            enc_para_table[i].img_improve_cfg_dynamic[j].rounding_still_mechanism = 1 ;
            enc_para_table[i].img_improve_cfg_dynamic[j].static16_tr1_denois_max_num = 0x4;
            enc_para_table[i].img_improve_cfg_dynamic[j].static16_coeff_protect_num = 0x10;

            enc_para_table[i].qpg_cfg_dynamic[j].rc_low_luma_en = 1;
            enc_para_table[i].qpg_cfg_dynamic[j].skin_max_qp = 0x33;
        }
    }

    return;
}

static hi_void set_para_for_qp_01(attr_table *enc_para_table, hi_u32 i, hi_u32 j)
{
    enc_para_table[i].img_improve_cfg_dynamic[j].norm16_tr1_denois_max_num = 1;
    enc_para_table[i].img_improve_cfg_dynamic[j].skin16_coeff_protect_num = 0x80;
    enc_para_table[i].img_improve_cfg_dynamic[j].hedge16_coeff_protect_num = 0x80;
    enc_para_table[i].img_improve_cfg_dynamic[j].edgemov16_coeff_protect_num = 0x80;
    enc_para_table[i].qpg_cfg_dynamic[j].rc_low_luma_en = 0;
    enc_para_table[i].img_improve_cfg_dynamic[j].static_en = 0;
    enc_para_table[i].qpg_cfg_dynamic[j].move_scene_en = 0;
    enc_para_table[i].img_improve_cfg_dynamic[j].highedge_en = 0;

    return;
}

static hi_void set_para_for_qp_02(attr_table *enc_para_table, hi_u32 i, hi_u32 j)
{
    enc_para_table[i].qpg_cfg_dynamic[j].move_scene_en = 0;
    enc_para_table[i].img_improve_cfg_dynamic[j].skin16_coeff_protect_num = 0x40;
    enc_para_table[i].img_improve_cfg_dynamic[j].hedge16_coeff_protect_num = 0x40;
    enc_para_table[i].img_improve_cfg_dynamic[j].edgemov16_coeff_protect_num = 0x40;
    enc_para_table[i].qpg_cfg_dynamic[j].skin_qp_delta = 0x6;

    return;
}

static hi_void set_para_for_qp_03(attr_table *enc_para_table, hi_u32 i, hi_u32 j)
{
    enc_para_table[i].img_improve_cfg_dynamic[j].skin16_coeff_protect_num = 0x40;
    enc_para_table[i].img_improve_cfg_dynamic[j].hedge16_coeff_protect_num = 0x40;
    enc_para_table[i].img_improve_cfg_dynamic[j].edgemov16_coeff_protect_num = 0x40;
    enc_para_table[i].qpg_cfg_dynamic[j].skin_qp_delta = 0x5;

    return;
}

static hi_void set_para_for_qp_04(attr_table *enc_para_table, hi_u32 i, hi_u32 j)
{
    enc_para_table[i].img_improve_cfg_dynamic[j].skin_intra_cu16_rdcost_offset = 0x1;
    enc_para_table[i].img_improve_cfg_dynamic[j].skin_inter_cu_rdcost_offset = 0x020;
    enc_para_table[i].img_improve_cfg_dynamic[j].skin_fme_cu16_rdcost_offset = 0x0;
    enc_para_table[i].img_improve_cfg_dynamic[j].skin_mrg_cu16_rdcost_offset = 0x0;

    enc_para_table[i].pme_cfg_dynamic[j].high_edge_cnt = 0x2;
    enc_para_table[i].pme_cfg_dynamic[j].high_edge_thr = 0xf;
    enc_para_table[i].pme_cfg_dynamic[j].interstrongedge_madi_thr = 0x40;
    enc_para_table[i].pme_cfg_dynamic[j].interdiff_max_min_madi_times = 0x2;
    enc_para_table[i].pme_cfg_dynamic[j].interdiff_max_min_madi_abs = 0x4 ;
    enc_para_table[i].qpg_cfg_dynamic[j].skin_max_qp = 0x33;

    return;
}

static hi_void set_para_for_pic_quality(attr_table *enc_para_table)
{
    hi_u32 i, j;

    set_para_normal_region(enc_para_table);

    for (i = 0; i < 2; i++) { /* 2: array size */
        /* start_qp in [0,25] */
        j = 0;
        set_para_for_qp_01(enc_para_table, i, j);

        /* start_qp in (25,30] */
        j = 1;
        set_para_for_qp_02(enc_para_table, i, j);

        /* start_qp in (30,35] */
        j = 0x2;
        set_para_for_qp_03(enc_para_table, i, j);

        /* start_qp in (42,51] */
        j = 0x4;
        set_para_for_qp_04(enc_para_table, i, j);

        if (i == 0) {
            enc_para_table[i].img_improve_cfg_dynamic[j].hedge_intra_cu16_rdcost_offset = 0x4;
        } else {
            enc_para_table[i].img_improve_cfg_dynamic[j].hedge_intra_cu16_rdcost_offset = 0x3;
        }

        enc_para_table[i].img_improve_cfg_dynamic[j].hedge_inter_cu_rdcost_offset = 0x000;
        enc_para_table[i].img_improve_cfg_dynamic[j].hedge16_tr1_denois_max_num = 0x3;
        enc_para_table[i].img_improve_cfg_dynamic[j].hedge16_coeff_protect_num = 0x20;
        enc_para_table[i].qpg_cfg_dynamic[j].move_sad_en = 1;
        enc_para_table[i].pme_cfg_dynamic[j].move_sad_thr = 0x100;

        if (i == 0) {
            enc_para_table[i].img_improve_cfg_dynamic[j].strmov_intra_cu16_rdcost_offset = 0x4;
        }  else {
            enc_para_table[i].img_improve_cfg_dynamic[j].strmov_intra_cu16_rdcost_offset = 0x3;
        }

        enc_para_table[i].img_improve_cfg_dynamic[j].strmov_inter_cu_rdcost_offset = 0x022;
        enc_para_table[i].img_improve_cfg_dynamic[j].strmov_fme_cu16_rdcost_offset = 0x3;
        enc_para_table[i].img_improve_cfg_dynamic[j].strmov_mrg_cu16_rdcost_offset = 0x3;

        enc_para_table[i].img_improve_cfg_dynamic[j].edgemov16_tr1_denois_max_num = 0x3 ;
        enc_para_table[i].img_improve_cfg_dynamic[j].edgemov16_coeff_protect_num = 0x20;

        enc_para_table[i].img_improve_cfg_dynamic[j].static_en = 1;
        enc_para_table[i].pme_cfg_dynamic[j].still_scene_thr = 0x2;
        enc_para_table[i].img_improve_cfg_dynamic[j].rounding_still_mechanism = 1;
        enc_para_table[i].img_improve_cfg_dynamic[j].static16_tr1_denois_max_num = 0x4;
        enc_para_table[i].img_improve_cfg_dynamic[j].static16_coeff_protect_num = 0x10;
    }
}

static hi_void drv_venc_hal_set_default_attr_by_start_qp(venc_hal_dynamic_cfg *dynamic_cfg, hi_u32 start_qp,
                                                         hi_u32 is_intra_pic)
{
    hi_u32 default_attr_by_qp_idx = 0;
    hi_u32 default_attr_frm_type_idx;

    if (start_qp <= 25) { /* 25: qp */
        default_attr_by_qp_idx = 0;
    } else if ((start_qp > 25) && (start_qp < 30)) { /* 25, 30: qp */
        default_attr_by_qp_idx = 1;
    } else if ((start_qp >= 30) && (start_qp < 35)) { /* 35, 30: qp */
        default_attr_by_qp_idx = 0x2;
    } else if ((start_qp >= 35) && (start_qp < 42)) { /* 35, 42: qp */
        default_attr_by_qp_idx = 0x3;
    } else {
        default_attr_by_qp_idx = 0x4;
    }

    default_attr_frm_type_idx = is_intra_pic ? 0 : 1;

    dynamic_cfg->img_improve_cfg_dynamic = \
        dynamic_cfg->enc_para_table[default_attr_frm_type_idx].img_improve_cfg_dynamic[default_attr_by_qp_idx];
    dynamic_cfg->qpg_cfg_dynamic = \
        dynamic_cfg->enc_para_table[default_attr_frm_type_idx].qpg_cfg_dynamic[default_attr_by_qp_idx];
    dynamic_cfg->pme_cfg_dynamic = \
        dynamic_cfg->enc_para_table[default_attr_frm_type_idx].pme_cfg_dynamic[default_attr_by_qp_idx];
}
#endif

static hi_void set_default_dynamic_attr_for_h264(venc_hal_dynamic_cfg *dynamic_cfg)
{
    hi_u32 i;
    for (i = 0; i < 40; i++) { /* 40: h264_lambda_ssd */
        dynamic_cfg->qpg_cfg_dynamic.lambda[i] = g_h264_lambda_ssd[i];
    }

    for (i = 40; i < 80; i++) { /* 40, 80: h264_lambda_ssd */
        dynamic_cfg->qpg_cfg_dynamic.lambda[i] = g_h264_lambda_sad[i - 40]; /* 40: h264_lambda_ssd */
    }

    dynamic_cfg->qpg_cfg_dynamic.qp_delta = 1;
    dynamic_cfg->pme_cfg_dynamic.skin_num = 0x20;
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[0] = 0;
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[1]  = 0;
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[2]  = 0; /* 2: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[3]  = 0; /* 3: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[4]  = 0x3; /* 4: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[5]  = 0x3; /* 5: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[6]  = 0x5; /* 6: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[7]  = 0x5; /* 7: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[8]  = 0x8; /* 8: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[9]  = 0x8; /* 9: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[10] = 0x8; /* 10: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[11] = 0xf; /* 11: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[12] = 0xf; /* 12: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[13] = 0x14; /* 13: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[14] = 0x19; /* 14: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[15] = 0x19; /* 15: array index */

    return;
}

static hi_void set_default_dynamic_attr_for_h265(venc_hal_dynamic_cfg *dynamic_cfg)
{
    hi_u32 i;
    for (i =  0; i < 40; i++) { /* 40: h265_lambda_ssd */
        dynamic_cfg->qpg_cfg_dynamic.lambda[i] = g_h265_lambda_ssd[i];
    }

    for (i = 40; i < 80; i++) { /* 40, 80: h265_lambda_ssd */
        dynamic_cfg->qpg_cfg_dynamic.lambda[i] = g_h265_lambda_sad[i - 40]; /* 40: h265_lambda_ssd */
    }

    dynamic_cfg->qpg_cfg_dynamic.qp_delta = 0x2;
    dynamic_cfg->pme_cfg_dynamic.skin_num = 0x60;
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[0]  = 0;
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[1]  = 0;
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[2]  = 0; /* 2: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[3]  = 0; /* 3: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[4]  = 0; /* 4: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[5]  = 0x3; /* 5: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[6]  = 0x4; /* 6: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[7]  = 0x5; /* 7: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[8]  = 0x2d; /* 8: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[9]  = 0xff; /* 9: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[10] = 0xff; /* 10: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[11] = 0xff; /* 11: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[12] = 0xff; /* 12: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[13] = 0xff; /* 13: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[14] = 0xff; /* 14: array index */
    dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[15] = 0xff; /* 15: array index */

    return;
}

static hi_void drv_venc_hal_set_default_attr_by_protocol(venc_hal_dynamic_cfg *dynamic_cfg, hi_u32 protocol)
{
    if (protocol == VEDU_H264) {
        set_default_dynamic_attr_for_h264(dynamic_cfg);
    } else {
        set_default_dynamic_attr_for_h265(dynamic_cfg);
    }
}

static hi_void drv_venc_hal_set_tunl_read_intvl(venc_hal_cfg *reg_cfg, hi_u32 *tunl_read_intvl)
{
    if (reg_cfg->ctrl_cfg.low_dly_mode) {
        if (reg_cfg->ctrl_cfg.enc_width >= 1920) { /* 1920: enc_width */
            *tunl_read_intvl = 0x3;
        } else if (reg_cfg->ctrl_cfg.enc_width >= 720) { /* 720: enc_width */
            *tunl_read_intvl = 0x2;
        } else {
            *tunl_read_intvl = 1;
        }
    }
}
static hi_void drv_venc_hal_set_mode_by_profile(hi_u32 profile, hi_u32 protocol, venc_hal_dynamic_cfg* dynamic_cfg)
{
    if (protocol == VEDU_H264) {
        dynamic_cfg->vcpi_entropy_mode = (profile == VEDU_H264_BASELINE_PROFILE) ? 1 : 0;
        dynamic_cfg->vcpi_trans_mode = (profile == VEDU_H264_HIGH_PROFILE ? 0 : 1);
        dynamic_cfg->vcpi_intra_cu_en = (profile == VEDU_H264_HIGH_PROFILE ? 15 : 5); /* 15, 5: profile */
    } else {
        dynamic_cfg->vcpi_entropy_mode = 0;
        dynamic_cfg->vcpi_trans_mode = 0;
        dynamic_cfg->vcpi_intra_cu_en = 0xf;
    }
}

static SINT32 drv_venc_hal_map_base_reg(hi_void)
{
    hi_s32 ret;

    if (g_reg_base == HI_NULL) {
        g_reg_base = (hi_u32*)g_venc_osal_func_ptr.fun_osal_ioremap(VEDU_REG_BASE_ADDR, \
            D_VENC_ALIGN_UP(sizeof(vedu_regs_type), 256)); /* 256: map size */
    }

    ret = (g_reg_base == HI_NULL) ? HI_FAILURE : HI_SUCCESS;

    return ret;
}

static hi_void drv_venc_hal_unmap_base_reg(hi_void)
{
    if (g_reg_base) {
        g_venc_osal_func_ptr.fun_osal_iounmap(g_reg_base);
        g_reg_base = HI_NULL;
    }
}

hi_void drv_venc_hal_v400_r008_c02_clr_int(interrupt_type type)
{
    vedu_regs_type *vedu_reg = (vedu_regs_type*)g_reg_base;

    switch (type) {
        case INTERRUPT_TYPE_PIC_END:
            vedu_reg->vedu_regs.vedu_vcpi_intclr.bits.vcpi_clr_ve_eop = 1;
            break;
        case INTERRUPT_TYPE_SLICE_END:
            vedu_reg->vedu_regs.vedu_vcpi_intclr.bits.vcpi_clr_vedu_slice_end = 1;
            break;
        case INTERRUPT_TYPE_TIMEOUT:
            vedu_reg->vedu_regs.vedu_vcpi_intclr.bits.vcpi_clr_vedu_timeout = 1;
            break;
        default:   /* clear all */
            vedu_reg->vedu_regs.vedu_vcpi_intclr.data = 0xFFFFFFFF;
            break;
    }

    return;
}

hi_void drv_venc_hal_v400_r008_c02_disable_int(interrupt_type type)
{
    vedu_regs_type *vedu_reg = (vedu_regs_type *)g_reg_base;

    switch (type) {
        case INTERRUPT_TYPE_PIC_END:
            vedu_reg->vedu_regs.vedu_vcpi_intmask.bits.vcpi_enable_ve_eop = 0;
            break;
        case INTERRUPT_TYPE_SLICE_END:
            vedu_reg->vedu_regs.vedu_vcpi_intmask.bits.vcpi_enable_vedu_slice_end = 0;
            break;
        case INTERRUPT_TYPE_TIMEOUT:
            vedu_reg->vedu_regs.vedu_vcpi_intmask.bits.vcpi_enable_vedu_timeout = 0;
            break;
        default:   /* disable all */
            vedu_reg->vedu_regs.vedu_vcpi_intmask.data = 0;
            break;
    }

    return;
}

#ifdef HI_SMMU_SUPPORT
/******************************************************************************
function   :
description:
calls      :
input      :
output     : read back int_stat & rate control register  for smmu
return     :
others     :
******************************************************************************/
hi_void drv_venc_hal_v400_r008_c02_read_reg_smmu(venc_hal_read_smmu* read_back)
{
    vedu_regs_type* all_reg = (vedu_regs_type *)g_reg_base;

    if (read_back == NULL) {
        return;
    }

    read_back->tlb_miss_stat_ns = all_reg->vedu_mmu_regs.smmu_intstat_ns.bits.intns_tlbmiss_stat;
    read_back->ptw_trans_stat_ns = all_reg->vedu_mmu_regs.smmu_intstat_ns.bits.intns_ptw_trans_stat;
    read_back->tlb_invalid_stat_ns_rd = all_reg->vedu_mmu_regs.smmu_intstat_ns.bits.intns_tlbinvalid_rd_stat;
    read_back->tlb_invalid_stat_ns_wr = all_reg->vedu_mmu_regs.smmu_intstat_ns.bits.intns_tlbinvalid_wr_stat;

    read_back->tlb_miss_stat_s = all_reg->vedu_mmu_regs.smmu_intstat_s.bits.ints_tlbmiss_stat;
    read_back->ptw_trans_stat_s = all_reg->vedu_mmu_regs.smmu_intstat_s.bits.ints_ptw_trans_stat;
    read_back->tlb_invalid_stat_s_rd = all_reg->vedu_mmu_regs.smmu_intstat_s.bits.ints_tlbinvalid_rd_stat;
    read_back->tlb_invalid_stat_s_wr = all_reg->vedu_mmu_regs.smmu_intstat_s.bits.ints_tlbinvalid_wr_stat;

    read_back->fault_addr_rd_ns = all_reg->vedu_mmu_regs.smmu_fault_addr_rd_ns.data;
    read_back->fault_addr_rd_s = all_reg->vedu_mmu_regs.smmu_fault_addr_rd_s.data;
    read_back->fault_addr_wr_ns = all_reg->vedu_mmu_regs.smmu_fault_addr_wr_ns.data;
    read_back->fault_addr_wr_s = all_reg->vedu_mmu_regs.smmu_fault_addr_wr_s.data;
    all_reg->vedu_mmu_regs.smmu_intclr_s.data = 0xFFFFFFFF;

    return;
}

static hi_void venc_hal_cfg_smmu_reg(venc_hal_smmu *smmu_cfg)
{
    vedu_regs_type  *all_reg = (vedu_regs_type *)g_reg_base;
    all_reg->vedu_mmu_regs.smmu_scb_ttbr.data = smmu_cfg->smmu_ns_page_base_addr;
    all_reg->vedu_mmu_regs.smmu_cb_ttbr.data = smmu_cfg->smmu_ns_page_base_addr;
    all_reg->vedu_mmu_regs.smmu_err_rdaddr_s.data = smmu_cfg->smmu_ns_err_read_addr;
    all_reg->vedu_mmu_regs.smmu_err_wraddr_s.data = smmu_cfg->smmu_ns_err_write_addr;
    all_reg->vedu_mmu_regs.smmu_err_rdaddr_ns.data = smmu_cfg->smmu_ns_err_read_addr;
    all_reg->vedu_mmu_regs.smmu_err_wraddr_ns.data = smmu_cfg->smmu_ns_err_write_addr;
}
#endif

static hi_void read_back_slice_length_reg(venc_hal_read *read_back, vedu_regs_type* all_reg)
{
    read_back->slice_length[0] = all_reg->vedu_func_regs.func_vlcst_dsrptr00.bits.slc_len0 - \
        all_reg->vedu_func_regs.func_vlcst_dsrptr01.bits.invalidnum0;
    read_back->slice_length[1] = all_reg->vedu_func_regs.func_vlcst_dsrptr10.bits.slc_len1 - \
        all_reg->vedu_func_regs.func_vlcst_dsrptr11.bits.invalidnum1;
    read_back->slice_length[2] = all_reg->vedu_func_regs.func_vlcst_dsrptr20.bits.slc_len2 - /* 2: array index */ \
        all_reg->vedu_func_regs.func_vlcst_dsrptr21.bits.invalidnum2;
    read_back->slice_length[3] = all_reg->vedu_func_regs.func_vlcst_dsrptr30.bits.slc_len3 - /* 3: array index */ \
        all_reg->vedu_func_regs.func_vlcst_dsrptr31.bits.invalidnum3;
    read_back->slice_length[4] = all_reg->vedu_func_regs.func_vlcst_dsrptr40.bits.slc_len4 - /* 4: array index */ \
        all_reg->vedu_func_regs.func_vlcst_dsrptr41.bits.invalidnum4;
    read_back->slice_length[5] = all_reg->vedu_func_regs.func_vlcst_dsrptr50.bits.slc_len5 - /* 5: array index */ \
        all_reg->vedu_func_regs.func_vlcst_dsrptr51.bits.invalidnum5;
    read_back->slice_length[6] = all_reg->vedu_func_regs.func_vlcst_dsrptr60.bits.slc_len6 - /* 6: array index */ \
        all_reg->vedu_func_regs.func_vlcst_dsrptr61.bits.invalidnum6;
    read_back->slice_length[7] = all_reg->vedu_func_regs.func_vlcst_dsrptr70.bits.slc_len7 - /* 7: array index */ \
        all_reg->vedu_func_regs.func_vlcst_dsrptr71.bits.invalidnum7;
    read_back->slice_length[8] = all_reg->vedu_func_regs.func_vlcst_dsrptr80.bits.slc_len8 - /* 8: array index */ \
        all_reg->vedu_func_regs.func_vlcst_dsrptr81.bits.invalidnum8;
    read_back->slice_length[9] = all_reg->vedu_func_regs.func_vlcst_dsrptr90.bits.slc_len9 - /* 9: array index */ \
        all_reg->vedu_func_regs.func_vlcst_dsrptr91.bits.invalidnum9;
    read_back->slice_length[10] = all_reg->vedu_func_regs.func_vlcst_dsrptr100.bits.slc_len10 - /* 10: array index */ \
        all_reg->vedu_func_regs.func_vlcst_dsrptr101.bits.invalidnum10;
    read_back->slice_length[11] = all_reg->vedu_func_regs.func_vlcst_dsrptr110.bits.slc_len11 - /* 11: array index */ \
        all_reg->vedu_func_regs.func_vlcst_dsrptr111.bits.invalidnum11;
    read_back->slice_length[12] = all_reg->vedu_func_regs.func_vlcst_dsrptr120.bits.slc_len12 - /* 12: array index */ \
        all_reg->vedu_func_regs.func_vlcst_dsrptr121.bits.invalidnum12;
    read_back->slice_length[13] = all_reg->vedu_func_regs.func_vlcst_dsrptr130.bits.slc_len13 - /* 13: array index */ \
        all_reg->vedu_func_regs.func_vlcst_dsrptr131.bits.invalidnum13;
    read_back->slice_length[14] = all_reg->vedu_func_regs.func_vlcst_dsrptr140.bits.slc_len14 - /* 14: array index */ \
        all_reg->vedu_func_regs.func_vlcst_dsrptr141.bits.invalidnum14;
    read_back->slice_length[15] = all_reg->vedu_func_regs.func_vlcst_dsrptr150.bits.slc_len15 - /* 15: array index */ \
        all_reg->vedu_func_regs.func_vlcst_dsrptr151.bits.invalidnum15;
}

static hi_void read_back_slice_end_reg(venc_hal_read *read_back, vedu_regs_type* all_reg)
{
    read_back->slice_is_end[0] = all_reg->vedu_func_regs.func_vlcst_dsrptr01.bits.islastslc0;
    read_back->slice_is_end[1] = all_reg->vedu_func_regs.func_vlcst_dsrptr11.bits.islastslc1;
    read_back->slice_is_end[2] = all_reg->vedu_func_regs.func_vlcst_dsrptr21.bits.islastslc2; /* 2: array index */
    read_back->slice_is_end[3] = all_reg->vedu_func_regs.func_vlcst_dsrptr31.bits.islastslc3; /* 3: array index */
    read_back->slice_is_end[4] = all_reg->vedu_func_regs.func_vlcst_dsrptr41.bits.islastslc4; /* 4: array index */
    read_back->slice_is_end[5] = all_reg->vedu_func_regs.func_vlcst_dsrptr51.bits.islastslc5; /* 5: array index */
    read_back->slice_is_end[6] = all_reg->vedu_func_regs.func_vlcst_dsrptr61.bits.islastslc6; /* 6: array index */
    read_back->slice_is_end[7] = all_reg->vedu_func_regs.func_vlcst_dsrptr71.bits.islastslc7; /* 7: array index */
    read_back->slice_is_end[8] = all_reg->vedu_func_regs.func_vlcst_dsrptr81.bits.islastslc8; /* 8: array index */
    read_back->slice_is_end[9] = all_reg->vedu_func_regs.func_vlcst_dsrptr91.bits.islastslc9; /* 9: array index */
    read_back->slice_is_end[10] = all_reg->vedu_func_regs.func_vlcst_dsrptr101.bits.islastslc10; /* 10: array index */
    read_back->slice_is_end[11] = all_reg->vedu_func_regs.func_vlcst_dsrptr111.bits.islastslc11; /* 11: array index */
    read_back->slice_is_end[12] = all_reg->vedu_func_regs.func_vlcst_dsrptr121.bits.islastslc12; /* 12: array index */
    read_back->slice_is_end[13] = all_reg->vedu_func_regs.func_vlcst_dsrptr131.bits.islastslc13; /* 13: array index */
    read_back->slice_is_end[14] = all_reg->vedu_func_regs.func_vlcst_dsrptr141.bits.islastslc14; /* 14: array index */
    read_back->slice_is_end[15] = all_reg->vedu_func_regs.func_vlcst_dsrptr151.bits.islastslc15; /* 15: array index */
}

static hi_void read_back_slice_reg(venc_hal_read *read_back, vedu_regs_type* all_reg)
{
    read_back_slice_length_reg(read_back, all_reg);
    read_back_slice_end_reg(read_back, all_reg);

    return;
}

/******************************************************************************
function   :
description:
calls      :
input      :
output     : read back int_stat & rate control register
return     :
others     :
******************************************************************************/
hi_void drv_venc_hal_v400_r008_c02_read_reg(venc_hal_read *read_back)
{
    vedu_regs_type* all_reg = (vedu_regs_type*)g_reg_base;
    venc_hal_ctrl* ctrl_cfg = &g_hal_reg_info->reg_ext_cfg.ctrl_cfg;

    if (read_back == NULL) {
        return;
    }

    /* read int_stat & RC reg */
    read_back->venc_end_of_pic = all_reg->vedu_func_regs.func_vcpi_intstat.bits.vcpi_int_ve_eop;
    read_back->venc_buf_full = all_reg->vedu_func_regs.func_vcpi_rawint.bits.vcpi_rint_ve_buffull;
    read_back->venc_time_out = all_reg->vedu_func_regs.func_vcpi_intstat.bits.vcpi_int_vedu_timeout;
    read_back->venc_slice_int = all_reg->vedu_func_regs.func_vcpi_intstat.bits.vcpi_int_vedu_slice_end;
    read_back->venc_pbit_overflow = all_reg->vedu_func_regs.func_vcpi_rawint.bits.vcpi_rint_ve_pbitsover;

    if (!read_back->venc_buf_full && !read_back->venc_pbit_overflow) {
        if (ctrl_cfg->protocol == VEDU_H264) {
            read_back->pic_bits = all_reg->vedu_func_regs.func_vlcst_slc_len_cnt * 8; /* 8: byte to bit */
            read_back->num_i_mb_cur_frm =  all_reg->vedu_func_regs.func_sel_intra_opt_8_x8_cnt.bits.intra_opt_8x8_cnt +
                all_reg->vedu_func_regs.func_sel_intra_pcm_opt_8_x8_cnt.bits.pcm_opt_8x8_cnt +
                all_reg->vedu_func_regs.func_sel_opt_4_x4_cnt.bits.opt_4x4_cnt;
        } else {
            read_back->pic_bits = all_reg->vedu_func_regs.func_cabac_pic_strmsize * 8; /* 8: byte to bit */
            read_back->num_i_mb_cur_frm =  all_reg->vedu_func_regs.func_sel_intra_opt_8_x8_cnt.bits.intra_opt_8x8_cnt +
                all_reg->vedu_func_regs.func_sel_intra_opt_16_x16_cnt.bits.intra_opt_16x16_cnt;
        }

        read_back->num_i_mb_cur_frm = read_back->num_i_mb_cur_frm / 4; /* 4: mb num */
        read_back->mhb_bits = all_reg->vedu_func_regs.func_cabac_hdr_bin_num;
        read_back->txt_bits = all_reg->vedu_func_regs.func_cabac_res_bin_num;
        read_back->madi_val = all_reg->vedu_func_regs.func_pme_madi_sum.bits.pme_madi_sum;
        read_back->madp_val = all_reg->vedu_func_regs.func_pme_madp_sum.bits.pme_madp_sum;
    }
    read_back_slice_reg(read_back, all_reg);

    return;
}
#if (VENC_REG_CONFIG_MODE == 1)
static hi_void ddr_cfg_cabac_slcher_part2_seg(venc_hal_cfg *reg_cfg) /* need to add h264 */
{
    s_vedu_regs_type* all_ddr = (s_vedu_regs_type*)(ULONG)reg_cfg->ext_cfg.ddr_reg_cfg_vir_addr;

    if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {

        if (reg_cfg->stream_cfg.slc_hdr_bits[1] < 32) { /* 32: slc_hdr_bits */
            all_ddr->vedu_cabac_slchdr_part2_seg1 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[0] << \
                (32 - reg_cfg->stream_cfg.slc_hdr_bits[1]); /* 32: slc_hdr_bits */
        } else {
            all_ddr->vedu_cabac_slchdr_part2_seg1 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[0] ;
        }


        if (reg_cfg->stream_cfg.slc_hdr_bits[1] < 64) { /* 64: slc_hdr_bits */
            all_ddr->vedu_cabac_slchdr_part2_seg2 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[1] << \
                (32 - (reg_cfg->stream_cfg.slc_hdr_bits[1] - 32)); /* 32: slc_hdr_bits */
        } else {
            all_ddr->vedu_cabac_slchdr_part2_seg2 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[1];
        }

        if (reg_cfg->stream_cfg.slc_hdr_bits[1] > 64) { /* 64: slc_hdr_bits */
            all_ddr->vedu_cabac_slchdr_part2_seg3 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[2] << /* 2:arry idx */ \
                (32 - (reg_cfg->stream_cfg.slc_hdr_bits[1] - 64)); /* 32,64: slc_hdr_bits */
        }

        all_ddr->vedu_cabac_slchdr_part2_seg4 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[3]; /* 3: array idx */
        all_ddr->vedu_cabac_slchdr_part2_seg5 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[4]; /* 4: array idx */
        all_ddr->vedu_cabac_slchdr_part2_seg6 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[5]; /* 5: array idx */
        all_ddr->vedu_cabac_slchdr_part2_seg7 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[6]; /* 6: array idx */
        all_ddr->vedu_cabac_slchdr_part2_seg8 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[7]; /* 7: array idx */
    }
}

static hi_void ddr_cfg_cabac_slcher_part2_seg_i(venc_hal_dynamic_cfg *dynamic_cfg, venc_hal_cfg *reg_cfg)
{
    s_vedu_regs_type* all_ddr = (s_vedu_regs_type*)(ULONG)reg_cfg->ext_cfg.ddr_reg_cfg_vir_addr;

    if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
        if (dynamic_cfg->slchdr_size_part2_i  < 32) { /* 32: slchdr_size_part2_i */
            all_ddr->vedu_cabac_slchdr_part2_seg1_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[0] << \
                (32 - dynamic_cfg->slchdr_size_part2_i); /* 32: slchdr_size_part2_i */
        } else {
            all_ddr->vedu_cabac_slchdr_part2_seg1_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[0] ;
        }
        if (dynamic_cfg->slchdr_size_part2_i  < 64) { /* 64: slchdr_size_part2_i */
            all_ddr->vedu_cabac_slchdr_part2_seg2_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[1] << \
                (32 - (dynamic_cfg->slchdr_size_part2_i  - 32)); /* 32: slchdr_size_part2_i */
        } else {
            all_ddr->vedu_cabac_slchdr_part2_seg2_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[1];
        }

        if (dynamic_cfg->slchdr_size_part2_i  > 64) { /* 64: slchdr_size_part2_i */
            all_ddr->vedu_cabac_slchdr_part2_seg3_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[2] << /* 2:array size */ \
                (32 - (dynamic_cfg->slchdr_size_part2_i  - 64)); /* 32, 64: slchdr_size_part2_i */
        }

        all_ddr->vedu_cabac_slchdr_part2_seg4_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[3]; /* 3: part2_seg_i */
        all_ddr->vedu_cabac_slchdr_part2_seg5_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[4]; /* 4: part2_seg_i */
        all_ddr->vedu_cabac_slchdr_part2_seg6_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[5]; /* 5: part2_seg_i */
        all_ddr->vedu_cabac_slchdr_part2_seg7_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[6]; /* 6: part2_seg_i */
        all_ddr->vedu_cabac_slchdr_part2_seg8_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[7]; /* 7: part2_seg_i */
    }
}
#endif

#if (VENC_REG_CONFIG_MODE == 0)
static hi_void cfg_cabac_slcher_part2_seg(venc_hal_cfg *reg_cfg)
{
    vedu_regs_type  *all_reg = (vedu_regs_type  *)g_reg_base;

    if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {

        if (reg_cfg->stream_cfg.slc_hdr_bits[1] < 32) { /* 32: slc_hdr_bits */
            all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg1 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[0] << \
                (32 - reg_cfg->stream_cfg.slc_hdr_bits[1]); /* 32: slc_hdr_bits */
        } else {
            all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg1 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[0] ;
        }


        if (reg_cfg->stream_cfg.slc_hdr_bits[1] < 64) { /* 64: slc_hdr_bits */
            all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg2 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[1] << \
                (32 - (reg_cfg->stream_cfg.slc_hdr_bits[1] - 32)); /* 32: slc_hdr_bits */
        } else {
            all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg2 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[1];
        }

        if (reg_cfg->stream_cfg.slc_hdr_bits[1] > 64) { /* 64: slc_hdr_bits */
            all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg3 = \
                reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[2] << /* 2: array idx */ \
                (32 - (reg_cfg->stream_cfg.slc_hdr_bits[1] - 64)); /* 32, 64: slc_hdr_bits */
        }

        all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg4 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[3]; /* 3:[] idx */
        all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg5 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[4]; /* 4:[] idx */
        all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg6 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[5]; /* 5:[] idx */
        all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg7 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[6]; /* 6:[] idx */
        all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg8 = reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[7]; /* 7:[] idx */
    }
}

static hi_void cfg_cabac_slcher_part2_seg_i(venc_hal_dynamic_cfg *dynamic_cfg, venc_hal_cfg *reg_cfg)
{
    vedu_regs_type  *all_reg = (vedu_regs_type  *)g_reg_base;

    if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
        if (dynamic_cfg->slchdr_size_part2_i  < 32) { /* 32: slchdr_size_part2_i */
            all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg1_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[0] << \
                (32 - dynamic_cfg->slchdr_size_part2_i); /* 32: slchdr_size_part2_i */
        } else {
            all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg1_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[0] ;
        }

        if (dynamic_cfg->slchdr_size_part2_i  < 64) { /* 64: slchdr_size_part2_i */
            all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg2_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[1] << \
                (32 - (dynamic_cfg->slchdr_size_part2_i - 32)); /* 32: slchdr_size_part2_i */
        } else {
            all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg2_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[1];
        }

        if (dynamic_cfg->slchdr_size_part2_i  > 64) { /* 64: slchdr_size_part2_i */
            all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg3_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[2] << /* 2:i */\
                (32 - (dynamic_cfg->slchdr_size_part2_i - 64)); /* 32, 64: slc_hdr_bits */
        }

        all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg4_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[3]; /* 3:array idx */
        all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg5_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[4]; /* 4:array idx */
        all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg6_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[5]; /* 5:array idx */
        all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg7_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[6]; /* 6:array idx */
        all_reg->vedu_regs.vedu_cabac_slchdr_part2_seg8_i = dynamic_cfg->cabac_slc_hdr_part2_seg_i[7]; /* 7:array idx */
    }
}

#endif
static hi_void cfg_sw_l0_size(venc_hal_cfg *reg_cfg)
{
    s_vedu_regs_type* all_ddr = (s_vedu_regs_type *)(ULONG)reg_cfg->ext_cfg.ddr_reg_cfg_vir_addr;
    vedu_regs_type   *all_reg = (vedu_regs_type *)g_reg_base;

    u_vedu_vcpi_sw_l0_size reg_data;

    if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
        hi_u32 w = 0x48;
        hi_u32 h = 0x20;

        h = TMP_MIN((reg_cfg->ctrl_cfg.enc_height / 4 - 16) / 2, h); /* 4, 16, 2: calculate heigh */
        w = TMP_MIN((reg_cfg->ctrl_cfg.enc_width / 4 - 16) / 2, w); /* 4, 16, 2: calculate width */

        h = h / 4 * 4; /* 4: align height */
        w = w / 4 * 4; /* 4: align width */

        h = TMP_CLIP3(8, 32, h); /* 8, 32: CLIP height */
        w = TMP_CLIP3(8, 72, w); /* 8, 72: CLIP width */

        reg_data.bits.vcpi_sw_l0_height  = h - 1;
        reg_data.bits.vcpi_sw_l0_width   = w - 1;
    } else if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {
        reg_data.bits.vcpi_sw_l0_height  = 0x10 - 1;
        reg_data.bits.vcpi_sw_l0_width   = 0x20 - 1;
    }

    if (reg_cfg->ctrl_cfg.reg_config_mode) {
        all_ddr->vedu_vcpi_sw_l0_size.data = reg_data.data;
    } else {
        all_reg->vedu_regs.vedu_vcpi_sw_l0_size.data = reg_data.data;
    }
}

static hi_void cfg_sw_l1_size(venc_hal_cfg *reg_cfg)
{
    s_vedu_regs_type* all_ddr = (s_vedu_regs_type*)(ULONG)reg_cfg->ext_cfg.ddr_reg_cfg_vir_addr;
    vedu_regs_type   *all_reg = (vedu_regs_type  *)g_reg_base;

    u_vedu_vcpi_sw_l1_size  reg_data;

    if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
        hi_u32 w = 0x48;
        hi_u32 h = 0x20;

        h = TMP_MIN((reg_cfg->ctrl_cfg.enc_height / 4 - 16) / 2, h); /* 4, 16, 2: calculate heigh */
        w = TMP_MIN((reg_cfg->ctrl_cfg.enc_width / 4 - 16) / 2, w); /* 4, 16, 2: calculate width */

        h = h / 4 * 4; /* 4: align height */
        w = w / 4 * 4; /* 4: align height */

        h = TMP_CLIP3(8, 32, h); /* 8, 32: CLIP height */
        w = TMP_CLIP3(8, 72, w); /* 8, 72: CLIP width */

        reg_data.bits.vcpi_sw_l1_height  = h - 1;
        reg_data.bits.vcpi_sw_l1_width   = w - 1;
    } else if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {
        reg_data.bits.vcpi_sw_l1_height  = 0x10 - 1;
        reg_data.bits.vcpi_sw_l1_width   = 0x20 - 1;
    }

    if (reg_cfg->ctrl_cfg.reg_config_mode) {
        all_ddr->vedu_vcpi_sw_l1_size.data = reg_data.data;
    } else {
        all_reg->vedu_regs.vedu_vcpi_sw_l1_size.data = reg_data.data;
    }
}

static hi_void cfg_str_fmt(venc_hal_cfg *reg_cfg, base_attr *base_cfg)
{
    vedu_regs_type *all_reg = (vedu_regs_type  *)g_reg_base;
    s_vedu_regs_type* all_ddr = (s_vedu_regs_type*)(ULONG)reg_cfg->ext_cfg.ddr_reg_cfg_vir_addr;
    u_vedu_vcpi_strfmt reg_data;

    reg_data.bits.vcpi_recst_disable = base_cfg->recst_disable;
    reg_data.bits.vcpi_package_sel = reg_cfg->buf_cfg.store_fmt == 0 ? !reg_cfg->buf_cfg.package_sel : \
                                        reg_cfg->buf_cfg.package_sel;
    reg_data.bits.vcpi_str_fmt = reg_cfg->buf_cfg.store_fmt == 0x2 ? 0x3  : (reg_cfg->buf_cfg.store_fmt == 1 ? 0x6 : 0);
    reg_data.bits.vcpi_blk_type = 0;
    reg_data.bits.vcpi_store_mode = 0;

    if (reg_cfg->ctrl_cfg.reg_config_mode) {
        all_ddr->vedu_vcpi_strfmt.data = reg_data.data;
    } else {
        all_reg->vedu_regs.vedu_vcpi_strfmt.data = reg_data.data;
    }
}

static hi_void cfg_vcpi_mode(venc_hal_cfg *reg_cfg, venc_hal_dynamic_cfg *dynamic_cfg, venc_hal_static_cfg *static_cfg)
{
    vedu_regs_type  *all_reg = (vedu_regs_type  *)g_reg_base;

    u_vedu_vcpi_mode  reg_data;
    reg_data.bits.vcpi_refc_nload = static_cfg->base_cfg.refld_col2gray_en;
    reg_data.bits.vcpi_time_en = 1; /* open timeout clock */

    reg_data.bits.vcpi_iscaling_type = 0x0;
    reg_data.bits.vcpi_ref_cmp_en = static_cfg->base_cfg.vcpi_ref0_cmp_en;
    reg_data.bits.vcpi_trans_mode = dynamic_cfg->vcpi_trans_mode;

    reg_data.bits.vcpi_sobel_weight_en = static_cfg->base_cfg.vcpi_sobel_weight_en;
    reg_data.bits.vcpi_pr_inter_en = static_cfg->base_cfg.vcpi_pr_inter_en;
    reg_data.bits.vcpi_pskip_en = static_cfg->base_cfg.vcpi_pskp_en;
    reg_data.bits.vcpi_idr_pic = reg_cfg->rc_cfg.intra_pic;

    reg_data.bits.vcpi_ref_num = reg_cfg->buf_cfg.ref_num >= 1 ? reg_cfg->buf_cfg.ref_num - 1 : 0; /* 0->1 1->2 */
    reg_data.bits.vcpi_long_term_refpic = reg_cfg->buf_cfg.long_term_refpic;

    reg_data.bits.vcpi_entropy_mode = dynamic_cfg->vcpi_entropy_mode;
    reg_data.bits.vcpi_frame_type = (reg_cfg->rc_cfg.intra_pic ? 0 : \
                                    (reg_cfg->rc_cfg.pic_type == PIC_BIINTER ? 2 : 1)); /* 2: B frame */

    reg_data.bits.vcpi_img_improve_en = static_cfg->img_improve_cfg_static.img_improve_en;
    reg_data.bits.vcpi_rec_cmp_en = static_cfg->base_cfg.vcpi_rec_cmp_en;
    reg_data.bits.vcpi_sao_chroma = static_cfg->base_cfg.vcpi_sao_chroma;
    reg_data.bits.vcpi_sao_luma = static_cfg->base_cfg.vcpi_sao_luma;

    reg_data.bits.vcpi_slice_int_en = 1; /* reg_cfg->ctrl_cfg.low_dly_mode */
    reg_data.bits.vcpi_protocol = (reg_cfg->ctrl_cfg.protocol == VEDU_H265 ? 0 : 1);
    reg_data.bits.vcpi_cfg_mode = (reg_cfg->ctrl_cfg.reg_config_mode) ? 1 : 0;

    reg_data.bits.vedu_selfrst_en = static_cfg->base_cfg.vcpi_selfrst_en;
    reg_data.bits.vcpi_vedsel = static_cfg->base_cfg.vcpi_vedsel;

    all_reg->vedu_regs.vedu_vcpi_mode.data = reg_data.data;
}

static hi_void reg_constraint_for176x144(venc_hal_ctrl* ctrl_cfg, venc_hal_rc* rc_cfg, hi_u32* qp_delta,
                                         venc_hal_static_cfg *static_cfg)
{
    if ((ctrl_cfg->enc_width <= 0x100) && (ctrl_cfg->enc_width >= 0x80) && (ctrl_cfg->protocol == VEDU_H264)) {
        rc_cfg->max_qp = 0x1c;
        rc_cfg->min_qp = 0x1c;
        rc_cfg->start_qp = 0x1c;
        *qp_delta = 0;
        ctrl_cfg->slc_split_en = 0;
        static_cfg->low_power_cfg.vcpi_ime_lowpow = 0;
        static_cfg->base_cfg.vcpi_iblk_pre_en = 0;
        static_cfg->base_cfg.vcpi_pblk_pre_en = 0;
        static_cfg->base_cfg.vcpi_force_inter = 0;
        static_cfg->pme_cfg_static.pme_iblk_refresh_num = 0;
        static_cfg->qpg_cfg_static.intra_det_qp_en = 0;
        static_cfg->base_cfg.vcpi_insert_i_slc_en = 0;
    }

    return;
}

static hi_void direct_cfg_reg_01(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vcpi_intmask reg_data;
        reg_data.bits.vcpi_enable_ve_eop = 0x1 ;
        reg_data.bits.vcpi_enable_vedu_slice_end = 0x1 ;
        reg_data.bits.vcpi_enable_ve_buffull = 0 ;
        reg_data.bits.vcpi_enable_ve_pbitsover = 0 ;
        reg_data.bits.vcpi_enable_axi_rdbus_err = 0 ;
        reg_data.bits.vcpi_enable_axi_wrbus_err = 0 ;
        reg_data.bits.vcpi_enable_vedu_timeout = 0x1 ;
        reg_data.bits.vcpi_enable_decoder_linenum_end = 0 ;
        reg_data.bits.vcpi_enable_decoder_sed_err_core = 0 ;
        reg_data.bits.vcpi_enable_decoder_sed_err_ext = 0 ;
        all_reg->vedu_regs.vedu_vcpi_intmask.data = reg_data.data;
    }
    all_reg->vedu_regs.vedu_vcpi_frameno = static_cfg->base_cfg.vcpi_frame_no; /* only for test */

    cfg_vcpi_mode(reg_cfg, dynamic_cfg, static_cfg);
}

static hi_void direct_cfg_reg_02(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    all_reg->vedu_regs.vedu_vcpi_basic_cfg0.data = 0x0;
    all_reg->vedu_regs.vedu_vcpi_basic_cfg1.data = 0x0;
    all_reg->vedu_regs.vedu_vcpi_decoder_d0.data = 0x0;
    all_reg->vedu_regs.vedu_vcpi_decoder_d1.data = 0x0;
    all_reg->vedu_regs.vedu_vcpi_decoder_d2.data = 0x0;
    all_reg->vedu_regs.vedu_vcpi_decoder_d3.data = 0x0;
    {
        u_vedu_vcpi_picsize_pix reg_data;
        reg_data.bits.vcpi_imgheight_pix = reg_cfg->ctrl_cfg.enc_height - 1;
        reg_data.bits.vcpi_imgwidth_pix  = reg_cfg->ctrl_cfg.enc_width  - 1;
        all_reg->vedu_regs.vedu_vcpi_picsize_pix.data = reg_data.data;
    }
    {
        u_vedu_vcpi_qpcfg reg_data;
        reg_data.bits.vcpi_frm_qp = reg_cfg->rc_cfg.start_qp;
        reg_data.bits.vcpi_cb_qp_offset = static_cfg->base_cfg.vcpi_cb_qp_offset;
        reg_data.bits.vcpi_cr_qp_offset = static_cfg->base_cfg.vcpi_cr_qp_offset;
        reg_data.bits.vcpi_intra_det_qp_en = static_cfg->qpg_cfg_static.intra_det_qp_en;
        reg_data.bits.vcpi_rc_cu_madi_en = static_cfg->qpg_cfg_static.rc_cu_madi_en;
        all_reg->vedu_regs.vedu_vcpi_qpcfg.data = reg_data.data;
    }
    {
        u_vedu_vcpi_outstd reg_data;
        reg_data.bits.vcpi_w_outstanding = static_cfg->w_out_std_num;
        reg_data.bits.vcpi_r_outstanding = static_cfg->r_out_std_num;
        reg_data.bits.vcpi_latency_statis_en = 0;
        reg_data.bits.vcpi_axi_lock_en       = 1;
        reg_data.bits.vcpi_axi_unlock_en     = 1;
        all_reg->vedu_regs.vedu_vcpi_outstd.data = reg_data.data;
    }
    all_reg->vedu_regs.vedu_vcpi_timeout = static_cfg->time_out;
}

static hi_void direct_cfg_reg_03(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vcpi_multislc reg_data;
        reg_data.bits.vcpi_multislc_en  = reg_cfg->ctrl_cfg.slc_split_en;
        reg_data.bits.vcpi_slcspilt_mod = 1; /* line mode */
        reg_data.bits.vcpi_slice_size   = reg_cfg->ctrl_cfg.split_size;
        all_reg->vedu_regs.vedu_vcpi_multislc.data = reg_data.data;
    }
    all_reg->vedu_regs.vedu_vcpi_llild_addr_l = reg_cfg->ext_cfg.ddr_reg_cfg_phy_addr;
    all_reg->vedu_regs.vedu_vcpi_llild_addr_h = 0x0 ;
    {
        u_vedu_vcpi_rc_enable reg_data;
        reg_data.bits.vcpi_rc_cu_qp_en = static_cfg->qpg_cfg_static.rc_cu_qp_en;
        reg_data.bits.vcpi_rc_row_qp_en = static_cfg->qpg_cfg_static.rc_row_qp_en;
        reg_data.bits.vcpi_move_scene_en = dynamic_cfg->qpg_cfg_dynamic.move_scene_en;
        reg_data.bits.vcpi_strong_edge_move_en = static_cfg->qpg_cfg_static.strong_edge_move_en;
        reg_data.bits.vcpi_rc_low_luma_en = dynamic_cfg->qpg_cfg_dynamic.rc_low_luma_en;
        reg_data.bits.vcpi_qp_restrain_large_sad = static_cfg->qpg_cfg_static.qp_restrain_large_sad;

        if (static_cfg->qpg_cfg_static.prev_min_sad_en == 0 || reg_cfg->rc_cfg.pic_type == PIC_INTRA) {
            reg_data.bits.vcpi_rd_min_sad_flag_en = 0 ;
        } else {
            reg_data.bits.vcpi_rd_min_sad_flag_en = 1 ;
        }
        reg_data.bits.vcpi_prev_min_sad_en = static_cfg->qpg_cfg_static.prev_min_sad_en;
        reg_data.bits.vcpi_low_min_sad_en  = static_cfg->qpg_cfg_static.low_min_sad_en ;
        reg_data.bits.vcpi_qpgld_en = static_cfg->qpg_cfg_static.qpgld_en;
        reg_data.bits.vcpi_map_roikeep_en = static_cfg->qpg_cfg_static.map_roikeep_en;
        reg_data.bits.vcpi_flat_region_en = static_cfg->qpg_cfg_static.flat_region_en;
        all_reg->vedu_regs.vedu_vcpi_rc_enable.data = reg_data.data;
    }
    {
        u_vedu_vlcst_ptbits_en reg_data;
        reg_data.bits.vlcst_ptbits_en = reg_cfg->ctrl_cfg.pt_bits_en;
        all_reg->vedu_regs.vedu_vlcst_ptbits_en.data = reg_data.data;
    }

    all_reg->vedu_regs.vedu_vlcst_ptbits = static_cfg->pt_bits;
    {
        u_vedu_qpg_max_min_qp reg_data;
        reg_data.bits.qpg_max_qp = reg_cfg->rc_cfg.max_qp;;
        reg_data.bits.qpg_cu_qp_delta_enable_flag = static_cfg->base_cfg.cu_qp_delta_enable_flag;
        reg_data.bits.qpg_min_qp = reg_cfg->rc_cfg.min_qp;
        all_reg->vedu_regs.vedu_qpg_max_min_qp.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_04(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_qpg_rc_max_min_qp reg_data;
        reg_data.bits.qpg_rc_min_qp = 0; /* para->sps_para_cfg.qpg_rc_min_qp */
        reg_data.bits.qpg_rc_max_qp = 0x33; /* para->sps_para_cfg.qpg_rc_max_qp */
        all_reg->vedu_regs.vedu_qpg_rc_max_min_qp.data = reg_data.data;
    }
    {
        u_vedu_qpg_smart_reg reg_data;
        reg_data.bits.qpg_smart_get_cu32_qp_mode = 0; /* enc_para->qpg_cfg.smart_get_cu32_qp_mode */
        reg_data.bits.qpg_smart_get_cu64_qp_mode = 0; /* enc_para->qpg_cfg.smart_get_cu64_qp_mode */
        reg_data.bits.qpg_qp_detlta_size_cu64    = 0x0;  /* enc_para->qpg_cfg.qp_detlta_size_cu64 */
        all_reg->vedu_regs.vedu_qpg_smart_reg.data  =  reg_data.data;
    }
    {
        u_vedu_qpg_row_target_bits reg_data;
        if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {

            reg_data.bits.qpg_row_target_bits = reg_cfg->rc_cfg.target_bits / \
                (reg_cfg->ctrl_cfg.enc_height / 16); /* 16: qpg_row_target_bits */
        } else {
            reg_data.bits.qpg_row_target_bits = reg_cfg->rc_cfg.target_bits / \
                (reg_cfg->ctrl_cfg.enc_height / 32); /* 32: qpg_row_target_bits */
        }

        reg_data.bits.qpg_qp_delta = dynamic_cfg->qpg_cfg_dynamic.qp_delta;
        all_reg->vedu_regs.vedu_qpg_row_target_bits.data = reg_data.data;
    }
    {
        u_vedu_qpg_average_lcu_bits reg_data;
        reg_data.bits.qpg_ave_lcu_bits = static_cfg->qpg_cfg_static.ave_lcu_bits;
        all_reg->vedu_regs.vedu_qpg_average_lcu_bits.data = reg_data.data;
    }
    {
        u_vedu_qpg_cu_qp_delta_thresh_reg0 reg_data;
        reg_data.bits.qpg_cu_qp_delta_thresh3 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[3]; /* 3: array index */
        reg_data.bits.qpg_cu_qp_delta_thresh2 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[2]; /* 2: array index */
        reg_data.bits.qpg_cu_qp_delta_thresh1 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[1];
        reg_data.bits.qpg_cu_qp_delta_thresh0 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[0];
        all_reg->vedu_regs.vedu_qpg_cu_qp_delta_thresh_reg0.data = reg_data.data;
    }
    {
        u_vedu_qpg_cu_qp_delta_thresh_reg1 reg_data;
        reg_data.bits.qpg_cu_qp_delta_thresh7 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[7]; /* 7: array index */
        reg_data.bits.qpg_cu_qp_delta_thresh6 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[6]; /* 6: array index */
        reg_data.bits.qpg_cu_qp_delta_thresh5 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[5]; /* 5: array index */
        reg_data.bits.qpg_cu_qp_delta_thresh4 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[4]; /* 4: array index */
        all_reg->vedu_regs.vedu_qpg_cu_qp_delta_thresh_reg1.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_05(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_qpg_cu_qp_delta_thresh_reg2 reg_data;
        reg_data.bits.qpg_cu_qp_delta_thresh11 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[11]; /* 11:array idx */
        reg_data.bits.qpg_cu_qp_delta_thresh10 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[10]; /* 10:array idx */
        reg_data.bits.qpg_cu_qp_delta_thresh9 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[9]; /* 9: array index */
        reg_data.bits.qpg_cu_qp_delta_thresh8 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[8]; /* 8: array index */
        all_reg->vedu_regs.vedu_qpg_cu_qp_delta_thresh_reg2.data = reg_data.data;
    }
    {
        u_vedu_qpg_cu_qp_delta_thresh_reg3 reg_data;
        reg_data.bits.qpg_cu_qp_delta_thresh15 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[15]; /* 15:array idx */
        reg_data.bits.qpg_cu_qp_delta_thresh14 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[14]; /* 14:array idx */
        reg_data.bits.qpg_cu_qp_delta_thresh13 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[13]; /* 13:array idx */
        reg_data.bits.qpg_cu_qp_delta_thresh12 = dynamic_cfg->qpg_cfg_dynamic.cu_qp_delta_thresh[12]; /* 12:array idx */
        all_reg->vedu_regs.vedu_qpg_cu_qp_delta_thresh_reg3.data = reg_data.data; ;
    }
    {
        u_vedu_qpg_delta_level reg_data;
        reg_data.bits.qpg_qp_delta_level_0 = static_cfg->qpg_cfg_static.qp_delta_level[0];
        reg_data.bits.qpg_qp_delta_level_1 = static_cfg->qpg_cfg_static.qp_delta_level[1];
        reg_data.bits.qpg_qp_delta_level_2 = static_cfg->qpg_cfg_static.qp_delta_level[2]; /* 2: array index */
        reg_data.bits.qpg_qp_delta_level_3 = static_cfg->qpg_cfg_static.qp_delta_level[3]; /* 3: array index */
        reg_data.bits.qpg_qp_delta_level_4 = static_cfg->qpg_cfg_static.qp_delta_level[4]; /* 4: array index */
        reg_data.bits.qpg_qp_delta_level_5 = static_cfg->qpg_cfg_static.qp_delta_level[5]; /* 5: array index */
        reg_data.bits.qpg_qp_delta_level_6 = static_cfg->qpg_cfg_static.qp_delta_level[6]; /* 6: array index */
        reg_data.bits.qpg_qp_delta_level_7 = static_cfg->qpg_cfg_static.qp_delta_level[7]; /* 7: array index */
        reg_data.bits.qpg_qp_delta_level_8 = static_cfg->qpg_cfg_static.qp_delta_level[8]; /* 8: array index */
        reg_data.bits.qpg_qp_delta_level_9 = static_cfg->qpg_cfg_static.qp_delta_level[9]; /* 9: array index */
        reg_data.bits.qpg_qp_delta_level_10 = static_cfg->qpg_cfg_static.qp_delta_level[10]; /* 10: array index */
        reg_data.bits.qpg_qp_delta_level_11 = static_cfg->qpg_cfg_static.qp_delta_level[11]; /* 11: array index */
        reg_data.bits.qpg_qp_delta_level_12 = static_cfg->qpg_cfg_static.qp_delta_level[12]; /* 12: array index */
        reg_data.bits.qpg_qp_delta_level_13 = static_cfg->qpg_cfg_static.qp_delta_level[13]; /* 13: array index */
        reg_data.bits.qpg_qp_delta_level_14 = static_cfg->qpg_cfg_static.qp_delta_level[14]; /* 14: array index */
        reg_data.bits.qpg_qp_delta_level_15 = static_cfg->qpg_cfg_static.qp_delta_level[15]; /* 15: array index */
        all_reg->vedu_regs.vedu_qpg_delta_level.data = reg_data.data;
    }
    {
        u_vedu_qpg_madi_switch_thr reg_data;
        reg_data.bits.qpg_qp_madi_switch_thr = static_cfg->qpg_cfg_static.qp_madi_switch_thr;
        all_reg->vedu_regs.vedu_qpg_madi_switch_thr.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_06(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_qpg_curr_sad_en reg_data;
        reg_data.bits.vcpi_rc_cu_sad_en = static_cfg->qpg_cfg_static.vcpi_rc_cu_sad_en;
        reg_data.bits.vcpi_sad_switch_thr = static_cfg->qpg_cfg_static.vcpi_sad_switch_thr;
        reg_data.bits.vcpi_rc_cu_sad_mod = static_cfg->qpg_cfg_static.vcpi_rc_cu_sad_mod;
        reg_data.bits.vcpi_rc_cu_sad_offset = static_cfg->qpg_cfg_static.vcpi_rc_cu_sad_offset;
        reg_data.bits.vcpi_rc_cu_sad_gain = static_cfg->qpg_cfg_static.vcpi_rc_cu_sad_gain;
        all_reg->vedu_regs.vedu_qpg_curr_sad_en.data = reg_data.data;
    }
    {
        u_vedu_qpg_curr_sad_level reg_data;
        reg_data.bits.vcpi_curr_sad_level_15 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[15]; /* 15:array idx */
        reg_data.bits.vcpi_curr_sad_level_14 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[14]; /* 14:array idx */
        reg_data.bits.vcpi_curr_sad_level_13 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[13]; /* 13:array idx */
        reg_data.bits.vcpi_curr_sad_level_12 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[12]; /* 12: array index */
        reg_data.bits.vcpi_curr_sad_level_11 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[11]; /* 11: array index */
        reg_data.bits.vcpi_curr_sad_level_10 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[10]; /* 10: array index */
        reg_data.bits.vcpi_curr_sad_level_9 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[9]; /* 9: array index */
        reg_data.bits.vcpi_curr_sad_level_8 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[8]; /* 8: array index */
        reg_data.bits.vcpi_curr_sad_level_7 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[7]; /* 7: array index */
        reg_data.bits.vcpi_curr_sad_level_6 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[6]; /* 6: array index */
        reg_data.bits.vcpi_curr_sad_level_5 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[5]; /* 5: array index */
        reg_data.bits.vcpi_curr_sad_level_4 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[4]; /* 4: array index */
        reg_data.bits.vcpi_curr_sad_level_3 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[3]; /* 3: array index */
        reg_data.bits.vcpi_curr_sad_level_2 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[2]; /* 2: array index */
        reg_data.bits.vcpi_curr_sad_level_1 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[1];
        reg_data.bits.vcpi_curr_sad_level_0 = static_cfg->qpg_cfg_static.vcpi_curr_sad_level[0];
        all_reg->vedu_regs.vedu_qpg_curr_sad_level.data = reg_data.data;
    }
    {
        u_vedu_qpg_curr_sad_thresh0 reg_data;
        reg_data.bits.vcpi_curr_sad_thresh_0 = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[0];
        reg_data.bits.vcpi_curr_sad_thresh_1 = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[1];
        reg_data.bits.vcpi_curr_sad_thresh_2 = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[2]; /* 2: array index */
        reg_data.bits.vcpi_curr_sad_thresh_3 = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[3]; /* 3: array index */
        all_reg->vedu_regs.vedu_qpg_curr_sad_thresh0.data = reg_data.data;
    }
    {
        u_vedu_qpg_curr_sad_thresh1 reg_data;
        reg_data.bits.vcpi_curr_sad_thresh_4 = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[4]; /* 4: array index */
        reg_data.bits.vcpi_curr_sad_thresh_5 = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[5]; /* 5: array index */
        reg_data.bits.vcpi_curr_sad_thresh_6 = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[6]; /* 6: array index */
        reg_data.bits.vcpi_curr_sad_thresh_7 = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[7]; /* 7: array index */
        all_reg->vedu_regs.vedu_qpg_curr_sad_thresh1.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_07(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_qpg_curr_sad_thresh2 reg_data;
        reg_data.bits.vcpi_curr_sad_thresh_8  = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[8]; /* 8:array idx */
        reg_data.bits.vcpi_curr_sad_thresh_9  = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[9]; /* 9:array idx */
        reg_data.bits.vcpi_curr_sad_thresh_10 = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[10]; /* 10:array idx */
        reg_data.bits.vcpi_curr_sad_thresh_11 = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[11]; /* 11:array idx */
        all_reg->vedu_regs.vedu_qpg_curr_sad_thresh2.data = reg_data.data;
    }
    {
        u_vedu_qpg_curr_sad_thresh3 reg_data;
        reg_data.bits.vcpi_curr_sad_thresh_12 = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[12]; /* 12:array idx */
        reg_data.bits.vcpi_curr_sad_thresh_13 = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[13]; /* 13:array idx */
        reg_data.bits.vcpi_curr_sad_thresh_14 = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[14]; /* 14:array idx */
        reg_data.bits.vcpi_curr_sad_thresh_15 = static_cfg->qpg_cfg_static.vcpi_curr_sad_thresh[15]; /* 15:array idx */
        all_reg->vedu_regs.vedu_qpg_curr_sad_thresh3.data = reg_data.data;
    }
    {
        u_vedu_luma_rc reg_data;
        reg_data.bits.vcpi_rc_luma_en = static_cfg->qpg_cfg_static.vcpi_rc_luma_en;
        reg_data.bits.vcpi_rc_luma_mode = static_cfg->qpg_cfg_static.vcpi_rc_luma_mode;
        reg_data.bits.vcpi_rc_luma_switch_thr = static_cfg->qpg_cfg_static.vcpi_rc_luma_switch_thr;
        reg_data.bits.vcpi_rc_luma_low_madi_thr = static_cfg->qpg_cfg_static.vcpi_rc_luma_low_madi_thr;
        reg_data.bits.vcpi_rc_luma_high_madi_thr = static_cfg->qpg_cfg_static.vcpi_rc_luma_high_madi_thr;
        all_reg->vedu_regs.vedu_luma_rc.data = reg_data.data;
    }
    {
        u_vedu_luma_level reg_data;
        reg_data.bits.vcpi_rc_luma_level_15 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[15]; /* 15: array index */
        reg_data.bits.vcpi_rc_luma_level_14 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[14]; /* 14: array index */
        reg_data.bits.vcpi_rc_luma_level_13 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[13]; /* 13: array index */
        reg_data.bits.vcpi_rc_luma_level_12 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[12]; /* 12: array index */
        reg_data.bits.vcpi_rc_luma_level_11 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[11]; /* 11: array index */
        reg_data.bits.vcpi_rc_luma_level_10 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[10]; /* 10: array index */
        reg_data.bits.vcpi_rc_luma_level_9 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[9]; /* 9: array index */
        reg_data.bits.vcpi_rc_luma_level_8 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[8]; /* 8: array index */
        reg_data.bits.vcpi_rc_luma_level_7 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[7]; /* 7: array index */
        reg_data.bits.vcpi_rc_luma_level_6 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[6]; /* 6: array index */
        reg_data.bits.vcpi_rc_luma_level_5 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[5]; /* 5: array index */
        reg_data.bits.vcpi_rc_luma_level_4 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[4]; /* 4: array index */
        reg_data.bits.vcpi_rc_luma_level_3 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[3]; /* 3: array index */
        reg_data.bits.vcpi_rc_luma_level_2 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[2]; /* 2: array index */
        reg_data.bits.vcpi_rc_luma_level_1 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[1];
        reg_data.bits.vcpi_rc_luma_level_0 = static_cfg->qpg_cfg_static.vcpi_rc_luma_level[0];
        all_reg->vedu_regs.vedu_luma_level.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_08(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_luma_thresh0 reg_data;
        reg_data.bits.vcpi_rc_luma_thresh_0 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[0]; /* 0: array index */
        reg_data.bits.vcpi_rc_luma_thresh_1 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[1]; /* 1: array index */
        reg_data.bits.vcpi_rc_luma_thresh_2 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[2]; /* 2: array index */
        reg_data.bits.vcpi_rc_luma_thresh_3 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[3]; /* 3: array index */
        all_reg->vedu_regs.vedu_luma_thresh0.data  =  reg_data.data;
    }
    {
        u_vedu_luma_thresh1 reg_data;
        reg_data.bits.vcpi_rc_luma_thresh_4 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[4]; /* 4: array index */
        reg_data.bits.vcpi_rc_luma_thresh_5 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[5]; /* 5: array index */
        reg_data.bits.vcpi_rc_luma_thresh_6 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[6]; /* 6: array index */
        reg_data.bits.vcpi_rc_luma_thresh_7 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[7]; /* 7: array index */
        all_reg->vedu_regs.vedu_luma_thresh1.data = reg_data.data;
    }
    {
        u_vedu_luma_thresh2 reg_data;
        reg_data.bits.vcpi_rc_luma_thresh_8 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[8]; /* 8: array index */
        reg_data.bits.vcpi_rc_luma_thresh_9 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[9]; /* 9: array index */
        reg_data.bits.vcpi_rc_luma_thresh_10 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[10]; /* 10: array index */
        reg_data.bits.vcpi_rc_luma_thresh_11 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[11]; /* 11: array index */
        all_reg->vedu_regs.vedu_luma_thresh2.data = reg_data.data;
    }
    {
        u_vedu_luma_thresh3 reg_data;
        reg_data.bits.vcpi_rc_luma_thresh_12 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[12]; /* 12: array index */
        reg_data.bits.vcpi_rc_luma_thresh_13 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[13]; /* 13: array index */
        reg_data.bits.vcpi_rc_luma_thresh_14 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[14]; /* 14: array index */
        reg_data.bits.vcpi_rc_luma_thresh_15 = static_cfg->qpg_cfg_static.vcpi_rc_luma_thresh[15]; /* 15: array index */
        all_reg->vedu_regs.vedu_luma_thresh3.data  =  reg_data.data;
    }
    {
        u_vedu_vctrl_lcu_baseline reg_data;
        if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
            reg_data.bits.vctrl_lcu_performance_baseline = 5300; /* 5300: for h265 */
        } else {
            reg_data.bits.vctrl_lcu_performance_baseline = 6250; /* 6250: for h264 */
        }
        reg_data.bits.vctrl_lcu_performance_en = reg_cfg->ctrl_cfg.lcu_performance_en;
        all_reg->vedu_regs.vedu_vctrl_lcu_baseline.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_09(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_mem_ctrl reg_data;
        reg_data.bits.rashd_rme = 0x0;
        reg_data.bits.rashd_rm = 0x2;
        reg_data.bits.rfshd_rme = 0x0;
        reg_data.bits.rfshd_rm = 0x2;
        reg_data.bits.rfshs_rme = 0x0 ;
        reg_data.bits.rfshs_rm = 0x2 ;
        reg_data.bits.rfthd_rma_high = 0x0;
        reg_data.bits.rfthd_rmea = 0x0;
        reg_data.bits.rfthd_rma = 0x2;
        reg_data.bits.rfthd_rmb_high = 0x0;
        reg_data.bits.rfthd_rmeb = 0x0;
        reg_data.bits.rfthd_rmb = 0x2;
        all_reg->vedu_regs.vedu_mem_ctrl.data = reg_data.data;
    }
    {
        u_vedu_safe_mode_flag reg_data;
        reg_data.bits.safe_flag_vcpi_llild = 0;
        reg_data.bits.safe_flag_curld_osd = 0;
        reg_data.bits.safe_flag_curld_lowdly = 0;
        reg_data.bits.safe_flag_curld_src = 0;
        reg_data.bits.safe_flag_refld_ref = 0;
        reg_data.bits.safe_flag_pmeld_ds = 0;
        reg_data.bits.safe_flag_pmest_ds = 0;
        reg_data.bits.safe_flag_recst_rec = 0;
        reg_data.bits.safe_flag_nbi_tmv = 0;
        reg_data.bits.safe_flag_pmeinfoldst_info = 0;
        reg_data.bits.safe_flag_qpgld_info = 0;
        reg_data.bits.safe_flag_pmeinfoldst_skipweight = 0;
        reg_data.bits.safe_flag_tqitq_qm = 0;
        reg_data.bits.safe_flag_vlcst_strm = 0;
        reg_data.bits.safe_flag_vlcst_swptr = 0;
        reg_data.bits.safe_flag_vlcst_srptr = 0;
        all_reg->vedu_regs.vedu_safe_mode_flag.data = reg_data.data;
    }
    {
        u_vedu_vcpi_ddr_cross reg_data;
        reg_data.bits.vcpi_ddr_cross_idx    = 0; /* [10..0] */
        reg_data.bits.vcpi_ddr_cross_en     = 1; /* [16] */
        all_reg->vedu_regs.vedu_vcpi_ddr_cross.data = reg_data.data;
    }
        all_reg->vedu_regs.vedu_debug_md5.data = 0x0;
}

static hi_void direct_cfg_reg_10(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_chn_bypass reg_data;
        reg_data.bits.vcpi_llild_bypass = 0;
        reg_data.bits.curld_lowdly_bypass = 0;
        reg_data.bits.curld_srcy_bypass = 0;
        reg_data.bits.curld_srcu_bypass = 0;
        reg_data.bits.curld_srcv_bypass = 0;
        reg_data.bits.pmeldst_bypass = 0;
        reg_data.bits.pmeinfoldst_bypass = 0;
        reg_data.bits.pmeinfold_qpgld_bypass = 0;
        reg_data.bits.pmeinfold_skipweight_bypass = 0;
        reg_data.bits.refld_bypass = 0;
        reg_data.bits.nbildst_tmv_bypass = 0;
        reg_data.bits.nbildst_up_bypass = 0;
        reg_data.bits.recst_bypass = 0;
        reg_data.bits.vlcst_strm_rptr_bypass = 1;
        reg_data.bits.vlcst_strm_wptr_bypass = 1;
        reg_data.bits.vlcst_para_bypass = 1;
        reg_data.bits.vlcst_strm_bypass = 0;
        reg_data.bits.refld_head_bypass = 0;
        reg_data.bits.recst_head_bypass = 0;
        reg_data.bits.curld_head_y_bypass = 0;
        reg_data.bits.curld_head_c_bypass = 0;
        reg_data.bits.curld_osd_bypass = 0;
#ifdef HI_SMMU_SUPPORT
        all_reg->vedu_regs.vedu_chn_bypass.data = reg_data.data;
#else
        all_reg->vedu_regs.vedu_chn_bypass.data = 0xffffffff;
#endif
    }
}

static hi_void direct_cfg_reg_72(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
#ifndef HI_TEE_SUPPORT
    {
        /* mmu addr cfg */
        u_vedu_smmu_scr reg_data;
        reg_data.bits.glb_bypass = static_cfg->glb_bypass;
        all_reg->vedu_mmu_regs.smmu_scr.data = reg_data.data;
    }
#endif
    {
        u_vedu_smmu_ctrl reg_data;
        reg_data.bits.int_en = static_cfg->int_en;
        reg_data.bits.ptw_pf = 0x3;
        all_reg->vedu_mmu_regs.smmu_ctrl.data = reg_data.data;
    }
    all_reg->vedu_mmu_regs.smmu_lp_ctrl.data = 0;
    all_reg->vedu_mmu_regs.smmu_intmask_s.data   = 0;
    all_reg->vedu_mmu_regs.smmu_intclr_s.data   = 0;
    all_reg->vedu_mmu_regs.smmu_intmask_ns.data = 0;
    all_reg->vedu_mmu_regs.smmu_intclr_ns.data  = 0;
    {
        u_vedu_smmu_master_dbg8 reg_data;
        reg_data.bits.sel1_chn_rd = 0x09;
        reg_data.bits.sel2_chn_rd = 0x07;
        reg_data.bits.sel1_chn_wr = 0x00;
        reg_data.bits.sel2_chn_wr = 0x01;
        all_reg->vedu_mmu_regs.smmu_master_dbg8.data = reg_data.data;
    }
    all_reg->vedu_regs.vedu_vcpi_intclr.data = 0;
#ifdef HI_SMMU_SUPPORT
    venc_hal_cfg_smmu_reg(&reg_cfg->smmu_cfg);
#endif

    all_reg->vedu_regs.vedu_vcpi_start.data = 1;
}

#if (VENC_REG_CONFIG_MODE == 0)
static hi_void direct_cfg_reg_11(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vcpi_ref_flag reg_data;
        if (reg_cfg->rc_cfg.pic_type != PIC_INTRA) {
            reg_data.bits.vcpi_curr_ref_long_flag = reg_cfg->buf_cfg.vcpi_curr_ref_long_flag;
            reg_data.bits.vcpi_col_from_l0_flag = 1;
        }

        if (reg_cfg->rc_cfg.pic_type == PIC_BIINTER) {
            reg_data.bits.vcpi_predflag_sel = 0x2;
        } else {
            reg_data.bits.vcpi_predflag_sel = 0;
        }
        all_reg->vedu_regs.vedu_vcpi_ref_flag.data = reg_data.data;
    }
    {
        u_vedu_pmv_tmv_en reg_data;
        if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
            reg_data.bits.pmv_tmv_en = static_cfg->base_cfg.tmv_en;
        } else if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {
            reg_data.bits.pmv_tmv_en = 1;
        }
        all_reg->vedu_regs.vedu_pmv_tmv_en.data = reg_data.data;
    }
    all_reg->vedu_regs.vedu_pmv_poc_0 = reg_cfg->buf_cfg.pmv_poc[0];
    all_reg->vedu_regs.vedu_pmv_poc_1 = reg_cfg->buf_cfg.pmv_poc[1];
    all_reg->vedu_regs.vedu_pmv_poc_2 = reg_cfg->buf_cfg.pmv_poc[2]; /* 2: ;pmv poc */
    all_reg->vedu_regs.vedu_pmv_poc_3 = reg_cfg->buf_cfg.pmv_poc[3]; /* 3: ;pmv poc */
    all_reg->vedu_regs.vedu_pmv_poc_4 = reg_cfg->buf_cfg.pmv_poc[4]; /* 4: ;pmv poc */
    all_reg->vedu_regs.vedu_pmv_poc_5 = reg_cfg->buf_cfg.pmv_poc[5]; /* 5: ;pmv poc */
    {
        u_vedu_cabac_glb_cfg reg_data;
        reg_data.data = 0;
        reg_data.bits.cabac_max_num_mergecand = static_cfg->base_cfg.max_num_mergecand;
        if (reg_cfg->rc_cfg.intra_pic) {
            reg_data.bits.cabac_nal_unit_head = 0x2601;
        } else {
            reg_data.bits.cabac_nal_unit_head = 0x0201;
        }
        all_reg->vedu_regs.vedu_cabac_glb_cfg.data = reg_data.data;
    }
    all_reg->vedu_regs.vedu_ice_cmc_mode_cfg0.data = 0x0005FB14;

    all_reg->vedu_regs.vedu_ice_cmc_mode_cfg1.data = 0x0005FB14;
}

static hi_void direct_cfg_reg_12(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vcpi_cross_tile_slc reg_data;
        reg_data.bits.vcpi_cross_slice = static_cfg->base_cfg.vcpi_cross_slice;
        reg_data.bits.vcpi_cross_tile = 0;
        all_reg->vedu_regs.vedu_vcpi_cross_tile_slc.data = reg_data.data;
    }
    {
        u_vedu_vcpi_dblkcfg  reg_data;
        reg_data.bits.vcpi_dblk_filter_flag = static_cfg->base_cfg.vcpi_dblk_filter_flag;
        reg_data.bits.vcpi_dblk_alpha = static_cfg->base_cfg.vcpi_dblk_alpha;
        reg_data.bits.vcpi_dblk_beta = static_cfg->base_cfg.vcpi_dblk_beta;
        all_reg->vedu_regs.vedu_vcpi_dblkcfg.data = reg_data.data;
    }
    {
        u_vedu_vcpi_refld_dw reg_data;
        reg_data.bits.vcpi_refld_hdw = 0x10;
        reg_data.bits.vcpi_refld_vdw = 0x10;
        all_reg->vedu_regs.vedu_vcpi_refld_dw.data = reg_data.data;
    }
    {
        u_vedu_intra_cfg reg_data;
        reg_data.bits.constrained_intra_pred_flag = 0;
        reg_data.bits.intra_smooth = 1;
        all_reg->vedu_regs.vedu_intra_cfg.data = reg_data.data;
    }
    {
        u_vedu_curld_gcfg reg_data;
        reg_data.bits.curld_osd0_global_en = 0;
        reg_data.bits.curld_osd1_global_en = 0;
        reg_data.bits.curld_osd2_global_en = 0;
        reg_data.bits.curld_osd3_global_en = 0;
        reg_data.bits.curld_osd4_global_en = 0;
        reg_data.bits.curld_osd5_global_en = 0;
        reg_data.bits.curld_osd6_global_en = 0;
        reg_data.bits.curld_osd7_global_en = 0;

        reg_data.bits.curld_col2gray_en = static_cfg->base_cfg.curld_col2gray_en;
        reg_data.bits.curld_clip_en = static_cfg->base_cfg.curld_clip_en;
        reg_data.bits.curld_lowdly_en = reg_cfg->ctrl_cfg.low_dly_mode;
        reg_data.bits.curld_read_interval = dynamic_cfg->tunl_read_intvl;
        reg_data.bits.curld_osd_rgbfmt =  0;

        all_reg->vedu_regs.vedu_curld_gcfg.data = reg_data.data;
    }
    cfg_str_fmt(reg_cfg, &static_cfg->base_cfg);
}

static hi_void direct_cfg_reg_13(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_lcu_target_bit reg_data;
        hi_u32 tmp;

        if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {
            tmp = 0x10;
        } else {
            tmp = 0x20;
        }
        tmp = ((reg_cfg->ctrl_cfg.enc_width + tmp - 1) / tmp) * ((reg_cfg->ctrl_cfg.enc_height + tmp - 1) / tmp);
        reg_data.bits.vctrl_lcu_target_bit = reg_cfg->rc_cfg.target_bits / tmp;
        all_reg->vedu_regs.vedu_vctrl_lcu_target_bit.data = reg_data.data;
    }
    {
        u_vedu_pme_safe_cfg reg_data;
        reg_data.bits.pme_safe_line = static_cfg->pme_cfg_static.pme_safe_line;
        reg_data.bits.pme_safe_line_val = static_cfg->pme_cfg_static.pme_safe_line_val;
        reg_data.bits.pme_safe_line_mode = static_cfg->pme_cfg_static.pme_safe_line_mode;
        all_reg->vedu_regs.vedu_pme_safe_cfg.data = reg_data.data;
    }
    {
        u_vedu_pme_iblk_refresh reg_data;
        reg_data.bits.pme_iblk_refresh_en = static_cfg->pme_cfg_static.pme_iblk_refresh_en;
        reg_data.bits.pme_iblk_refresh_mode = static_cfg->pme_cfg_static.pme_iblk_refresh_mode;
        all_reg->vedu_regs.vedu_pme_iblk_refresh.data = reg_data.data;
    }
    {
        u_vedu_pme_iblk_refresh_para reg_data;
        reg_data.bits.pme_iblk_refresh_start = static_cfg->pme_cfg_static.pme_iblk_refresh_start_num;
        reg_data.bits.pme_iblk_refresh_size = static_cfg->pme_cfg_static.pme_iblk_refresh_num;
        all_reg->vedu_regs.vedu_pme_iblk_refresh_para.data = reg_data.data;
    }

    if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
        all_reg->vedu_regs.vedu_intra_chnl4_ang_0_en.data = 0xffffffff;
    } else if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {
        all_reg->vedu_regs.vedu_intra_chnl4_ang_0_en.data = 0xffffffff;
    }
    all_reg->vedu_regs.vedu_intra_chnl4_ang_1_en.data = 0x7;

    if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
        all_reg->vedu_regs.vedu_intra_chnl8_ang_0_en.data = 0xffffffff;
    } else if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {
        all_reg->vedu_regs.vedu_intra_chnl8_ang_0_en.data = 0xffffffff;
    }
    all_reg->vedu_regs.vedu_intra_chnl8_ang_1_en.data = 0x7;
}

static hi_void direct_cfg_reg_14(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
        all_reg->vedu_regs.vedu_intra_chnl16_ang_0_en.data = 0xffffffff;
    } else if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {
        all_reg->vedu_regs.vedu_intra_chnl16_ang_0_en.data = 0xffffffff;
    }
    all_reg->vedu_regs.vedu_intra_chnl16_ang_1_en.data = 0x7;

    all_reg->vedu_regs.vedu_intra_chnl32_ang_0_en.data = 0xffffffff;

    all_reg->vedu_regs.vedu_intra_chnl32_ang_1_en.data = 0x7;
    {
        u_vedu_pack_cu_parameter reg_data;
        reg_data.bits.pack_vcpi2cu_qp_min_cu_size = 0x2;
        all_reg->vedu_regs.vedu_pack_cu_parameter.data = reg_data.data;
    }
    {
        u_vedu_rgb2yuv_coef_p0 reg_data;
        reg_data.bits.rgb2yuv_coef_00 = 0;
        reg_data.bits.rgb2yuv_coef_01 = 0;
        reg_data.bits.rgb2yuv_coef_02 = 0;
        all_reg->vedu_regs.vedu_rgb2_yuv_coef_p0.data = reg_data.data;
    }
    {
        u_vedu_rgb2yuv_coef_p1  reg_data;
        reg_data.bits.rgb2yuv_coef_10 = 0;
        reg_data.bits.rgb2yuv_coef_11 = 0;
        reg_data.bits.rgb2yuv_coef_12 = 0;
        all_reg->vedu_regs.vedu_rgb2_yuv_coef_p1.data = reg_data.data;
    }
    {
        u_vedu_rgb2yuv_coef_p2 reg_data;
        reg_data.bits.rgb2yuv_coef_20 = 0;
        reg_data.bits.rgb2yuv_coef_21 = 0;
        reg_data.bits.rgb2yuv_coef_22 = 0;

        all_reg->vedu_regs.vedu_rgb2_yuv_coef_p2.data = reg_data.data;
    }
    {
        u_vedu_rgb2yuv_offset reg_data;
        reg_data.bits.rgb2yuv_offset_y = 0;
        reg_data.bits.rgb2yuv_offset_u = 0;
        reg_data.bits.rgb2yuv_offset_v = 0;
        all_reg->vedu_regs.vedu_rgb2_yuv_offset.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_15(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_rgb2yuv_clip_thr_y  reg_data;
        reg_data.bits.rgb2yuv_clip_min_y = 0;
        reg_data.bits.rgb2yuv_clip_max_y = 0;
        all_reg->vedu_regs.vedu_rgb2_yuv_clip_thr_y.data = reg_data.data;
    }
    {
        u_vedu_rgb2yuv_clip_thr_c  reg_data;
        reg_data.bits.rgb2yuv_clip_min_c = 0;
        reg_data.bits.rgb2yuv_clip_max_c = 0;
        all_reg->vedu_regs.vedu_rgb2_yuv_clip_thr_c.data = reg_data.data;
    }
    {
        u_vedu_rgb2yuv_shift_width  reg_data;
        reg_data.bits.rgb2yuv_shift_width = 0;
        all_reg->vedu_regs.vedu_rgb2_yuv_shift_width.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_enable  reg_data;
        reg_data.bits.vcpi_osd_en = 0;
        reg_data.bits.vcpi_osd7_absqp = 0;
        reg_data.bits.vcpi_osd6_absqp = 0;
        reg_data.bits.vcpi_osd5_absqp = 0;
        reg_data.bits.vcpi_osd4_absqp = 0;
        reg_data.bits.vcpi_osd3_absqp = 0;
        reg_data.bits.vcpi_osd2_absqp = 0;
        reg_data.bits.vcpi_osd1_absqp = 0;
        reg_data.bits.vcpi_osd0_absqp = 0;
        reg_data.bits.vcpi_osd7_en = 0;
        reg_data.bits.vcpi_osd6_en = 0;
        reg_data.bits.vcpi_osd5_en = 0;
        reg_data.bits.vcpi_osd4_en = 0;
        reg_data.bits.vcpi_osd3_en = 0;
        reg_data.bits.vcpi_osd2_en = 0;
        reg_data.bits.vcpi_osd1_en = 0;
        reg_data.bits.vcpi_osd0_en = 0;
        reg_data.bits.vcpi_roi_osd_sel_0 = 0;
        reg_data.bits.vcpi_roi_osd_sel_1 = 0;
        reg_data.bits.vcpi_roi_osd_sel_2 = 0;
        reg_data.bits.vcpi_roi_osd_sel_3 = 0;
        reg_data.bits.vcpi_roi_osd_sel_4 = 0;
        reg_data.bits.vcpi_roi_osd_sel_5 = 0;
        reg_data.bits.vcpi_roi_osd_sel_6 = 0;
        reg_data.bits.vcpi_roi_osd_sel_7 = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_enable.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_16(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_vcpi_osd_pos_0 reg_data;
        reg_data.bits.vcpi_osd0_y = 0;
        reg_data.bits.vcpi_osd0_x = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_pos_0.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_pos_1 reg_data;
        reg_data.bits.vcpi_osd1_y = 0;
        reg_data.bits.vcpi_osd1_x = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_pos_1.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_pos_2 reg_data;
        reg_data.bits.vcpi_osd2_y = 0;
        reg_data.bits.vcpi_osd2_x = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_pos_2.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_pos_3 reg_data;
        reg_data.bits.vcpi_osd3_y = 0;
        reg_data.bits.vcpi_osd3_x = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_pos_3.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_pos_4 reg_data;
        reg_data.bits.vcpi_osd4_y = 0;
        reg_data.bits.vcpi_osd4_x = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_pos_4.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_pos_5 reg_data;
        reg_data.bits.vcpi_osd5_y = 0;
        reg_data.bits.vcpi_osd5_x = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_pos_5.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_pos_6 reg_data;
        reg_data.bits.vcpi_osd6_y = 0;
        reg_data.bits.vcpi_osd6_x = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_pos_6.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_17(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_vcpi_osd_pos_7 reg_data;
        reg_data.bits.vcpi_osd7_y = 0;
        reg_data.bits.vcpi_osd7_x = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_pos_7.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_size_0 reg_data;
        reg_data.bits.vcpi_osd0_h = 0;
        reg_data.bits.vcpi_osd0_w = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_size_0.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_size_1 reg_data;
        reg_data.bits.vcpi_osd1_h = 0;
        reg_data.bits.vcpi_osd1_w = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_size_1.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_size_2 reg_data;
        reg_data.bits.vcpi_osd2_h = 0;
        reg_data.bits.vcpi_osd2_w = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_size_2.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_size_3 reg_data;
        reg_data.bits.vcpi_osd3_h = 0;
        reg_data.bits.vcpi_osd3_w = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_size_3.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_size_4 reg_data;
        reg_data.bits.vcpi_osd4_h = 0;
        reg_data.bits.vcpi_osd4_w = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_size_4.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_size_5 reg_data;
        reg_data.bits.vcpi_osd5_h = 0;
        reg_data.bits.vcpi_osd5_w = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_size_5.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_18(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_vcpi_osd_size_6 reg_data;
        reg_data.bits.vcpi_osd6_h = 0;
        reg_data.bits.vcpi_osd6_w = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_size_6.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_size_7 reg_data;
        reg_data.bits.vcpi_osd7_h = 0;
        reg_data.bits.vcpi_osd7_w = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_size_7.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_layerid  reg_data;
        reg_data.bits.vcpi_osd7_layer_id = 0;
        reg_data.bits.vcpi_osd6_layer_id = 0;
        reg_data.bits.vcpi_osd5_layer_id = 0;
        reg_data.bits.vcpi_osd4_layer_id = 0;
        reg_data.bits.vcpi_osd3_layer_id = 0;
        reg_data.bits.vcpi_osd2_layer_id = 0;
        reg_data.bits.vcpi_osd1_layer_id = 0;
        reg_data.bits.vcpi_osd0_layer_id = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_layerid.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_qp0 reg_data;
        reg_data.bits.vcpi_osd3_qp = 0;
        reg_data.bits.vcpi_osd2_qp = 0;
        reg_data.bits.vcpi_osd1_qp = 0;
        reg_data.bits.vcpi_osd0_qp = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_qp0.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_qp1 reg_data;
        reg_data.bits.vcpi_osd7_qp = 0;
        reg_data.bits.vcpi_osd6_qp = 0;
        reg_data.bits.vcpi_osd5_qp = 0;
        reg_data.bits.vcpi_osd4_qp = 0;
        all_reg->vedu_regs.vedu_vcpi_osd_qp1.data = reg_data.data;
    }

}

static hi_void direct_cfg_reg_19(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_curld_osd01_alpha reg_data;
        reg_data.bits.curld_osd0_alpha0 = 0;
        reg_data.bits.curld_osd0_alpha1 = 0;
        reg_data.bits.curld_osd1_alpha0 = 0;
        reg_data.bits.curld_osd1_alpha1 = 0;
        all_reg->vedu_regs.vedu_curld_osd01_alpha.data = reg_data.data;
    }
    {
        u_vedu_curld_osd23_alpha  reg_data;
        reg_data.bits.curld_osd2_alpha0 = 0;
        reg_data.bits.curld_osd2_alpha1 = 0;
        reg_data.bits.curld_osd3_alpha0 = 0;
        reg_data.bits.curld_osd3_alpha1 = 0;
        all_reg->vedu_regs.vedu_curld_osd23_alpha.data = reg_data.data;
    }
    {
        u_vedu_curld_osd45_alpha reg_data;
        reg_data.bits.curld_osd4_alpha0 = 0;
        reg_data.bits.curld_osd4_alpha1 = 0;
        reg_data.bits.curld_osd5_alpha0 = 0;
        reg_data.bits.curld_osd5_alpha1 = 0;
        all_reg->vedu_regs.vedu_curld_osd45_alpha.data = reg_data.data;
    }
    {
        u_vedu_curld_osd67_alpha reg_data;
        reg_data.bits.curld_osd6_alpha0 = 0;
        reg_data.bits.curld_osd6_alpha1 = 0;
        reg_data.bits.curld_osd7_alpha0 = 0;
        reg_data.bits.curld_osd7_alpha1 = 0;
        all_reg->vedu_regs.vedu_curld_osd67_alpha.data = reg_data.data;
    }
    {
        u_vedu_curld_osd_galpha0 reg_data;
        reg_data.bits.curld_osd0_global_alpha = 0;
        reg_data.bits.curld_osd1_global_alpha = 0;
        reg_data.bits.curld_osd2_global_alpha = 0;
        reg_data.bits.curld_osd3_global_alpha = 0;
        all_reg->vedu_regs.vedu_curld_osd_galpha0.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_20(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_curld_osd_galpha1 reg_data;
        reg_data.bits.curld_osd4_global_alpha = 0;
        reg_data.bits.curld_osd5_global_alpha = 0;
        reg_data.bits.curld_osd6_global_alpha = 0;
        reg_data.bits.curld_osd7_global_alpha = 0;
        all_reg->vedu_regs.vedu_curld_osd_galpha1.data = reg_data.data;
    }

    all_reg->vedu_regs.vedu_curld_osd0_addr_l = 0;
    all_reg->vedu_regs.vedu_curld_osd1_addr_l = 0;
    all_reg->vedu_regs.vedu_curld_osd2_addr_l = 0;
    all_reg->vedu_regs.vedu_curld_osd3_addr_l = 0;
    all_reg->vedu_regs.vedu_curld_osd4_addr_l = 0;
    all_reg->vedu_regs.vedu_curld_osd5_addr_l = 0;
    all_reg->vedu_regs.vedu_curld_osd6_addr_l = 0;
    all_reg->vedu_regs.vedu_curld_osd7_addr_l = 0;
    all_reg->vedu_regs.vedu_curld_osd0_addr_h = 0x0;
    all_reg->vedu_regs.vedu_curld_osd1_addr_h = 0x0;
    all_reg->vedu_regs.vedu_curld_osd2_addr_h = 0x0;
    all_reg->vedu_regs.vedu_curld_osd3_addr_h = 0x0;
    all_reg->vedu_regs.vedu_curld_osd4_addr_h = 0x0;
    all_reg->vedu_regs.vedu_curld_osd5_addr_h = 0x0;
    all_reg->vedu_regs.vedu_curld_osd6_addr_h = 0x0;
    all_reg->vedu_regs.vedu_curld_osd7_addr_h = 0x0;
    {
        u_vedu_curld_osd01_stride reg_data;
        reg_data.bits.curld_osd0_stride = 0;
        reg_data.bits.curld_osd1_stride = 0;
        all_reg->vedu_regs.vedu_curld_osd01_stride.data = reg_data.data;
    }
    {
        u_vedu_curld_osd23_stride reg_data;
        reg_data.bits.curld_osd2_stride = 0;
        reg_data.bits.curld_osd3_stride = 0;
        all_reg->vedu_regs.vedu_curld_osd23_stride.data = reg_data.data;
    }
    {
        u_vedu_curld_osd45_stride reg_data;
        reg_data.bits.curld_osd4_stride = 0;
        reg_data.bits.curld_osd5_stride = 0;
        all_reg->vedu_regs.vedu_curld_osd45_stride.data =  reg_data.data;
    }
}

static hi_void direct_cfg_reg_21(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_curld_osd67_stride reg_data;
        reg_data.bits.curld_osd6_stride = 0;
        reg_data.bits.curld_osd7_stride = 0;
        all_reg->vedu_regs.vedu_curld_osd67_stride.data = reg_data.data;
    }
    {
        u_vedu_vcpi_vlc_config reg_data;
        reg_data.bits.vcpi_byte_stuffing  = static_cfg->base_cfg.vcpi_byte_stuffing;
        reg_data.bits.vcpi_cabac_init_idc = static_cfg->base_cfg.vcpi_cabac_init_idc;

        if (reg_cfg->rc_cfg.pic_type == PIC_BIINTER) { /* for h264 */
            reg_data.bits.vcpi_ref_idc = 0;
        } else {
            reg_data.bits.vcpi_ref_idc = 0x3;
        }
        all_reg->vedu_regs.vedu_vcpi_vlc_config.data = reg_data.data;
    }
    {
        u_vedu_cabac_slchdr_size reg_data;
        reg_data.data = 0;
        reg_data.bits.cabac_slchdr_size_part1 = reg_cfg->stream_cfg.slc_hdr_bits[0];
        reg_data.bits.cabac_slchdr_size_part2 = reg_cfg->stream_cfg.slc_hdr_bits[1];
        all_reg->vedu_regs.vedu_cabac_slchdr_size.data = reg_data.data;
    }
    {
        u_vedu_cabac_slchdr_part1 reg_data;
        reg_data.data = 0;
        reg_data.bits.cabac_slchdr_part1 = reg_cfg->stream_cfg.slc_hdr_part1  << \
            (16 - reg_cfg->stream_cfg.slc_hdr_bits[0]); /* 16: slc hdr bit */
        all_reg->vedu_regs.vedu_cabac_slchdr_part1.data = reg_data.data;
    }
        cfg_cabac_slcher_part2_seg(reg_cfg);
    {
        u_vedu_cabac_slchdr_size_i reg_data;
        reg_data.data = 0;
        reg_data.bits.cabac_slchdr_size_part1_i = dynamic_cfg->slchdr_size_part1_i;
        reg_data.bits.cabac_slchdr_size_part2_i = dynamic_cfg->slchdr_size_part2_i;
        all_reg->vedu_regs.vedu_cabac_slchdr_size_i.data = reg_data.data;
    }
    {
        u_vedu_cabac_slchdr_part1_i reg_data;
        reg_data.data = 0;
        reg_data.bits.cabac_slchdr_part1_i = dynamic_cfg->slchdr_part1_i << \
            (16 - dynamic_cfg->slchdr_size_part1_i); /* 16: slc hdr bit */
        all_reg->vedu_regs.vedu_cabac_slchdr_part1_i.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_22(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    cfg_cabac_slcher_part2_seg_i(dynamic_cfg, reg_cfg);
    {
        u_vedu_vlc_slchdrpara reg_data;
        reg_data.bits.vlc_parabit = ((reg_cfg->stream_cfg.slc_hdr_bits[0] >>  0) & 0xFF) - 1;
        reg_data.bits.vlc_reorderbit = ((reg_cfg->stream_cfg.slc_hdr_bits[0] >>  8) & 0xFF) - 1; /* 8: slc hdr bit */
        reg_data.bits.vlc_markingbit = ((reg_cfg->stream_cfg.slc_hdr_bits[0] >> 16) & 0xFF) - 1; /* 16: slc hdr bit */
        all_reg->vedu_regs.vedu_vlc_slchdrpara.data = reg_data.data;
    }
    {
        u_vedu_vlc_svc reg_data;
        reg_data.bits.vlc_svc_en = 0;
        reg_data.bits.vlc_svc_strm = static_cfg->base_cfg.vlc_svc_strm;
        all_reg->vedu_regs.vedu_vlc_svc.data = reg_data.data;
    }
    all_reg->vedu_regs.vedu_vlc_slchdrstrm0 = reg_cfg->stream_cfg.slc_hdr_stream [0];
    all_reg->vedu_regs.vedu_vlc_slchdrstrm1 = reg_cfg->stream_cfg.slc_hdr_stream [1];
    all_reg->vedu_regs.vedu_vlc_slchdrstrm2 = reg_cfg->stream_cfg.slc_hdr_stream [2]; /* 2: array index */
    all_reg->vedu_regs.vedu_vlc_slchdrstrm3 = reg_cfg->stream_cfg.slc_hdr_stream [3]; /* 3: array index */
    all_reg->vedu_regs.vedu_vlc_reorderstrm0 = reg_cfg->stream_cfg.reorder_stream[0];
    all_reg->vedu_regs.vedu_vlc_reorderstrm1 = reg_cfg->stream_cfg.reorder_stream[1];
    all_reg->vedu_regs.vedu_vlc_markingstrm0 = reg_cfg->stream_cfg.marking_stream[0];
    all_reg->vedu_regs.vedu_vlc_markingstrm1 = reg_cfg->stream_cfg.marking_stream[1];

    all_reg->vedu_regs.vedu_vlc_slchdrstrm0_i = dynamic_cfg->slc_hdr_stream_i [0];
    all_reg->vedu_regs.vedu_vlc_slchdrstrm1_i = dynamic_cfg->slc_hdr_stream_i [1];
    all_reg->vedu_regs.vedu_vlc_slchdrstrm2_i = dynamic_cfg->slc_hdr_stream_i [2]; /* 2: array index */
    all_reg->vedu_regs.vedu_vlc_slchdrstrm3_i = dynamic_cfg->slc_hdr_stream_i [3]; /* 3: array index */
    all_reg->vedu_regs.vedu_vlc_reorderstrm0_i = dynamic_cfg->reorder_stream_i[0];
    all_reg->vedu_regs.vedu_vlc_reorderstrm1_i = dynamic_cfg->reorder_stream_i[1];
    all_reg->vedu_regs.vedu_vlc_markingstrm0_i = dynamic_cfg->marking_stream_i[0];
    all_reg->vedu_regs.vedu_vlc_markingstrm1_i = dynamic_cfg->marking_stream_i[1];

    all_reg->vedu_regs.vedu_vlc_slchdrpara_i.bits.vlc_parabit_i = ((dynamic_cfg->slc_hdr_bits_i >>  0) & 0xFF) - 1;
    all_reg->vedu_regs.vedu_vlc_slchdrpara_i.bits.vlc_reorderbit_i = \
        ((dynamic_cfg->slc_hdr_bits_i >>  8) & 0xFF) - 1; /* 8: slc hdr bit */
    all_reg->vedu_regs.vedu_vlc_slchdrpara_i.bits.vlc_markingbit_i = \
        ((dynamic_cfg->slc_hdr_bits_i >> 16) & 0xFF) - 1; /* 16: slc hdr bit */

    all_reg->vedu_regs.vedu_vlcst_slc_cfg0 = 0;
    all_reg->vedu_regs.vedu_vlcst_slc_cfg1 = 0;
    all_reg->vedu_regs.vedu_vlcst_slc_cfg2 = 0;
    all_reg->vedu_regs.vedu_vlcst_slc_cfg3 = 0;
}

static hi_void direct_cfg_reg_23(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_ice_v3r2_seg_256x1_dcmp_ctrl reg_data;
        reg_data.bits.is_lossless = 0;
        reg_data.bits.head_tword = 0;

        all_reg->vedu_regs.vedu_ice_v3_r2_seg_256_x1_dcmp_ctrl.data = reg_data.data;
    }
    {
        u_vedu_vcpi_low_power reg_data;
        reg_data.bits.vcpi_osd_clkgete_en = 1;
        reg_data.bits.vcpi_curld_dcmp_clkgate_en = 1;
        reg_data.bits.vcpi_refld_dcmp_clkgate_en = 1;
        reg_data.bits.vcpi_cpi_clkgate_en = 1;
        reg_data.bits.vcpi_vlc_clkgate_en = 1;
        reg_data.bits.vcpi_ref1_clkgate_en = 1;
        reg_data.bits.vcpi_rec_cmp_clkgate_en = 1;

        reg_data.bits.vcpi_mem_clkgate_en = static_cfg->mem_clk_gate_en;
        reg_data.bits.vcpi_clkgate_en = static_cfg->clk_gate_en;
        reg_data.bits.vcpi_mrg_gtck_en = 1;
        reg_data.bits.vcpi_tqitq_gtck_en = 1;
        reg_data.bits.vcpi_ime_lowpow_en = static_cfg->low_power_cfg.vcpi_ime_lowpow;
        reg_data.bits.vcpi_fme_lowpow_en = static_cfg->low_power_cfg.vcpi_fme_lowpow;
        reg_data.bits.vcpi_intra_lowpow_en = static_cfg->low_power_cfg.vcpi_intra_lowpow;
        all_reg->vedu_regs.vedu_vcpi_low_power.data = reg_data.data;
    }
    {
        u_vedu_vcpi_pre_judge_ext_en reg_data;
        reg_data.bits.vcpi_ext_edge_en = static_cfg->base_cfg.vcpi_ext_edge_en;
        reg_data.bits.vcpi_pintra_inter_flag_disable = static_cfg->base_cfg.vcpi_pintra_inter_flag_disable;
        reg_data.bits.vcpi_force_inter = static_cfg->base_cfg.vcpi_force_inter;
        reg_data.bits.vcpi_pblk_pre_en = static_cfg->base_cfg.vcpi_pblk_pre_en;
        all_reg->vedu_regs.vedu_vcpi_pre_judge_ext_en.data = reg_data.data;
    }
    {
        u_vedu_vcpi_pre_judge_cost_thr reg_data;
        reg_data.bits.vcpi_pblk_pre_cost_thr = static_cfg->base_cfg.vcpi_pblk_pre_cost_thr;
        reg_data.bits.vcpi_iblk_pre_cost_thr = static_cfg->base_cfg.vcpi_iblk_pre_cost_thr;
        all_reg->vedu_regs.vedu_vcpi_pre_judge_cost_thr.data = reg_data.data;
    }
    {
        u_vedu_vcpi_pme_param reg_data;
        reg_data.bits.vcpi_move_sad_en = dynamic_cfg->qpg_cfg_dynamic.move_sad_en;
        reg_data.bits.vcpi_pblk_pre_mvx_thr = static_cfg->base_cfg.pblk_pre_mvx_thr;
        reg_data.bits.vcpi_pblk_pre_mvy_thr = static_cfg->base_cfg.pblk_pre_mvy_thr;
        all_reg->vedu_regs.vedu_vcpi_pme_param.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_24(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_pme_pblk_pre1 reg_data;
        reg_data.bits.pme_pblk_pre_mv_dif_thr1 = static_cfg->pme_cfg_static.pme_pblk_pre_mv_dif_thr1;
        reg_data.bits.pme_pblk_pre_mv_dif_thr0 = static_cfg->pme_cfg_static.pme_pblk_pre_mv_dif_thr0;
        reg_data.bits.pme_pblk_pre_mv_dif_cost_thr = static_cfg->pme_cfg_static.pme_pblk_pre_mv_dif_cost_thr;
        all_reg->vedu_regs.vedu_pme_pblk_pre1.data = reg_data.data;
    }
    {
        u_vedu_pme_pblk_pre2 reg_data;
        reg_data.bits.pme_pblk_pre_madi_times = static_cfg->pme_cfg_static.pme_pblk_pre_madi_times;
        reg_data.bits.pme_pblk_pre_offset = static_cfg->pme_cfg_static.pme_pblk_pre_offset;
        reg_data.bits.pme_pblk_pre_gain = static_cfg->pme_cfg_static.pme_pblk_pre_gain;
        all_reg->vedu_regs.vedu_pme_pblk_pre2.data = reg_data.data;
    }
    {
        u_vedu_pme_sw_adapt_en reg_data;
        reg_data.bits.pme_l0_psw_adapt_en = static_cfg->pme_cfg_static.l0_psw_adapt_en;
        reg_data.bits.pme_l1_psw_adapt_en = 1;
        all_reg->vedu_regs.vedu_pme_sw_adapt_en.data = reg_data.data;
    }
    {
        u_vedu_pme_window_size0_l0 reg_data;
        reg_data.bits.pme_l0_win0_width = static_cfg->pme_cfg_static.l0_win0_width;
        reg_data.bits.pme_l0_win0_height = static_cfg->pme_cfg_static.l0_win0_height;
        all_reg->vedu_regs.vedu_pme_window_size0_l0.data = reg_data.data;
    }
    {
        u_vedu_pme_window_size0_l1 reg_data;
        reg_data.bits.pme_l1_win0_width = static_cfg->pme_cfg_static.l1_win0_width;
        reg_data.bits.pme_l1_win0_height = static_cfg->pme_cfg_static.l1_win0_height;
        all_reg->vedu_regs.vedu_pme_window_size0_l1.data = reg_data.data;
    }
    {
        u_vedu_pme_psw_lpw reg_data;
        reg_data.bits.pme_psw_lp_diff_thx = static_cfg->pme_cfg_static.pme_psw_lp_diff_thx;
        reg_data.bits.pme_psw_lp_diff_thy = static_cfg->pme_cfg_static.pme_psw_lp_diff_thy;
        all_reg->vedu_regs.vedu_pme_psw_lpw.data = reg_data.data  ;
    }
    {
        u_vedu_pme_skip_pre reg_data;
        reg_data.bits.pme_skipblk_pre_en = static_cfg->pme_cfg_static.skipblk_pre_en;
        reg_data.bits.pme_skipblk_pre_cost_thr = static_cfg->pme_cfg_static.skipblk_pre_cost_thr;
        all_reg->vedu_regs.vedu_pme_skip_pre.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_25(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_pme_skip_flag reg_data;
        reg_data.bits.vcpi_pskip_strongedge_madi_thr = static_cfg->pme_cfg_static.vcpi_pskip_strongedge_madi_thr;
        reg_data.bits.vcpi_pskip_strongedge_madi_times = static_cfg->pme_cfg_static.vcpi_pskip_strongedge_madi_times;
        reg_data.bits.vcpi_pskip_flatregion_madi_thr = static_cfg->pme_cfg_static.vcpi_pskip_flatregion_madi_thr;
        all_reg->vedu_regs.vedu_pme_skip_flag.data = reg_data.data;
    }
    {
        u_vedu_pme_tr_weightx reg_data;
        reg_data.bits.pme_tr_weightx_2 = static_cfg->pme_cfg_static.tr_weightx_2;
        reg_data.bits.pme_tr_weightx_1 = static_cfg->pme_cfg_static.tr_weightx_1;
        reg_data.bits.pme_tr_weightx_0 = static_cfg->pme_cfg_static.tr_weightx_0;
        all_reg->vedu_regs.vedu_pme_tr_weightx.data = reg_data.data;
    }
    {
        u_vedu_pme_tr_weighty reg_data;
        reg_data.bits.pme_tr_weighty_2 = static_cfg->pme_cfg_static.tr_weighty_2;
        reg_data.bits.pme_tr_weighty_1 = static_cfg->pme_cfg_static.tr_weighty_1;
        reg_data.bits.pme_tr_weighty_0 = static_cfg->pme_cfg_static.tr_weighty_0;
        all_reg->vedu_regs.vedu_pme_tr_weighty.data = reg_data.data;
    }
    {
        u_vedu_pme_sr_weight reg_data;
        reg_data.bits.pme_sr_weight_2 = static_cfg->pme_cfg_static.sr_weight_2;
        reg_data.bits.pme_sr_weight_1 = static_cfg->pme_cfg_static.sr_weight_1;
        reg_data.bits.pme_sr_weight_0 = static_cfg->pme_cfg_static.sr_weight_0;
        all_reg->vedu_regs.vedu_pme_sr_weight.data = reg_data.data;
    }
    {
        u_vedu_ime_fme_lpow_thr reg_data;
        reg_data.bits.ime_lowpow_fme_thr1 = static_cfg->low_power_cfg.vcpi_lowpow_fme_thr1;
        reg_data.bits.ime_lowpow_fme_thr0 = static_cfg->low_power_cfg.vcpi_lowpow_fme_thr0;
        all_reg->vedu_regs.vedu_ime_fme_lpow_thr.data = reg_data.data;
    }
    {
        u_vedu_fme_pu64_lwp reg_data;
        reg_data.bits.fme_pu64_lwp_flag = 0x0 ; /* enc_para->low_power_cfg.fme_pu64_lwp_flag */
        reg_data.bits.vcpi_detect_low3_en = 0x0 ; /* enc_para->low_power_cfg.vcpi_detect_low3_en */
        reg_data.bits.vcpi_strong_edge_with_space_en = 0;
        all_reg->vedu_regs.vedu_fme_pu64_lwp.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_26(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_mrg_force_zero_en reg_data;
        if (reg_cfg->rc_cfg.avbr && !reg_cfg->rc_cfg.intra_pic) {
            reg_data.bits.mrg_force_v_zero_en = 1;
            reg_data.bits.mrg_force_u_zero_en = 1;
            reg_data.bits.mrg_force_y_zero_en = 1;
            reg_data.bits.mrg_force_zero_en   = 1;
        } else {
            reg_data.bits.mrg_force_v_zero_en = static_cfg->base_cfg.force_v_zero_en;
            reg_data.bits.mrg_force_u_zero_en = static_cfg->base_cfg.force_u_zero_en;
            reg_data.bits.mrg_force_y_zero_en = static_cfg->base_cfg.force_y_zero_en;
            reg_data.bits.mrg_force_zero_en   = static_cfg->base_cfg.force_zero_en;
        }
        reg_data.bits.force_adapt_en = 0;
        reg_data.bits.fme_rdo_lpw_en = 0;
        reg_data.bits.dct4_en = 1;
        reg_data.bits.fme_rdo_lpw_th = 0;
        reg_data.bits.mrg_skip_weight_en = 0;
        all_reg->vedu_regs.vedu_mrg_force_zero_en.data = reg_data.data;
    }
    {
        u_vedu_mrg_force_skip_en reg_data;
        reg_data.bits.inter32_use_tu16_en = 1; /* enc_para->base_cfg.inter32_use_tu16_en */
        reg_data.bits.mrg_detect_intercu32_use_tu16 = 0x0; /* enc_para->low_power_cfg.mrg_detect_intercu32_use_tu16 */
        reg_data.bits.mrg_not_use_sad_en = 0x0; /* enc_para->low_power_cfg.mrg_not_use_sad_en */
        reg_data.bits.mrg_not_use_sad_th = 0x4;
        all_reg->vedu_regs.vedu_mrg_force_skip_en.data = reg_data.data;
    }
    {
        u_vedu_ime_inter_mode reg_data;
        reg_data.bits.ime_intra4_lowpow_en = 0x0 ;
        reg_data.bits.ime_high3pre_en = static_cfg->low_power_cfg.vcpi_high3pre_en;
        reg_data.bits.ime_inter8x8_en = static_cfg->base_cfg.vcpi_inter8x8_en; /* for h264 */
        reg_data.bits.ime_layer3to2_en  = 0;
        all_reg->vedu_regs.vedu_ime_inter_mode.data = reg_data.data;
    }
    {
        u_vedu_ime_layer3to2_thr reg_data;
        reg_data.bits.ime_layer3to2_thr0 = 0x70;
        reg_data.bits.ime_layer3to2_thr1 = 0x10e;
        all_reg->vedu_regs.vedu_ime_layer3_to2_thr.data = reg_data.data;
    }
    {
        u_vedu_ime_layer3to2_thr1 reg_data;
        reg_data.bits.ime_layer3to2_cost_diff_thr = 0x100;
        all_reg->vedu_regs.vedu_ime_layer3_to2_thr1.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_27(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_ime_layer3to1_thr reg_data;
        reg_data.bits.ime_layer3to1_en = 0x0; /* enc_para->low_power_cfg.ime_layer3to1_en */
        reg_data.bits.ime_layer3to1_pu64_madi_thr = 0x5;
        all_reg->vedu_regs.vedu_ime_layer3_to1_thr.data = reg_data.data;
    }
    {
        u_vedu_ime_layer3to1_thr1 reg_data;
        reg_data.bits.ime_layer3to1_pu32_cost_thr = 0xbb8;
        reg_data.bits.ime_layer3to1_pu64_cost_thr = 0x1000;
        all_reg->vedu_regs.vedu_ime_layer3_to1_thr1.data = reg_data.data;
    }
    {
        u_vedu_vcpi_intra32_low_power reg_data;
        reg_data.bits.vcpi_intra32_low_power_thr = 0x400;
        reg_data.bits.vcpi_intra32_low_power_en = 1;
        reg_data.bits.vcpi_intra32_low_power_gain = 0x8;
        reg_data.bits.vcpi_intra32_low_power_offset = 0x2;
        all_reg->vedu_regs.vedu_vcpi_intra32_low_power.data = reg_data.data;
    }
    {
        u_vedu_vcpi_intra16_low_power reg_data;
        reg_data.bits.vcpi_intra16_low_power_thr = 0x100;
        reg_data.bits.vcpi_intra16_low_power_en = 0;
        reg_data.bits.vcpi_intra16_low_power_gain = 0;
        reg_data.bits.vcpi_intra16_low_power_offset = 0;
        all_reg->vedu_regs.vedu_vcpi_intra16_low_power.data = reg_data.data;
    }
    {
        u_vedu_pme_intra_lowpow reg_data;
        reg_data.bits.pme_intra16_madi_thr = static_cfg->pme_cfg_static.pme_intra16_madi_thr;
        reg_data.bits.pme_intra32_madi_thr = static_cfg->pme_cfg_static.pme_intra32_madi_thr;
        reg_data.bits.pme_intra_lowpow_en  = 0;
        all_reg->vedu_regs.vedu_pme_intra_lowpow.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_28(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_intra_low_pow0 reg_data;
        reg_data.bits.cu16_weak_ang_thr = 0x3;
        reg_data.bits.cu16_medium_ang_thr = 0x5;
        reg_data.bits.cu16_strong_ang_thr = 0xb;
        reg_data.bits.cu16_rdo_num = 0x3;
        reg_data.bits.cu16_adaptive_reduce_rdo_en = 0;
        reg_data.bits.cu32_weak_ang_thr = 0x3;
        reg_data.bits.cu32_medium_ang_thr = 0x6;
        reg_data.bits.cu32_strong_ang_thr = 0xd;
        reg_data.bits.cu32_rdo_num = 0x2;
        reg_data.bits.cu32_adaptive_reduce_rdo_en = 0;
        all_reg->vedu_regs.vedu_intra_low_pow0.data = reg_data.data;
    }
    {
        u_vedu_intra_low_pow1 reg_data;
        reg_data.bits.vcpi_force_cu16_low_pow = 0;
        reg_data.bits.vcpi_force_cu32_low_pow = 0;
        reg_data.bits.vcpi_detect_close_intra32_en = 0x0 ; /* enc_para->low_power_cfg.vcpi_detect_close_intra32_en */
        all_reg->vedu_regs.vedu_intra_low_pow1.data = reg_data.data;
    }
    {
        u_vedu_vcpi_intra_inter_cu_en reg_data;
        reg_data.bits.vcpi_mrg_cu_en = static_cfg->base_cfg.vcpi_mrg_cu_en;
        reg_data.bits.vcpi_fme_cu_en = static_cfg->base_cfg.vcpi_fme_cu_en;
        reg_data.bits.vcpi_ipcm_en = static_cfg->base_cfg.vcpi_ipcm_en;
        reg_data.bits.vcpi_intra_cu_en = dynamic_cfg->vcpi_intra_cu_en;
        reg_data.bits.vcpi_intra_h264_cutdiag = static_cfg->base_cfg.vcpi_intra_h264_cutdiag;
        all_reg->vedu_regs.vedu_vcpi_intra_inter_cu_en.data = reg_data.data;
    }
    {
        u_vedu_pack_pcm_parameter reg_data;
        reg_data.bits.pack_vcpi2pu_log2_max_ipcm_cbsizey = 0x3 ;
        reg_data.bits.pack_vcpi2pu_log2_min_ipcm_cbsizey = 0x3 ;
        all_reg->vedu_regs.vedu_pack_pcm_parameter.data = reg_data.data;
    }
    {
        u_vedu_qpg_cu32_delta reg_data;
        reg_data.bits.qpg_cu32_delta_low = static_cfg->qpg_cfg_static.cu32_delta_low;
        reg_data.bits.qpg_cu32_delta_high = static_cfg->qpg_cfg_static.cu32_delta_high;
        all_reg->vedu_regs.vedu_qpg_cu32_delta.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_29(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_qpg_qp_restrain reg_data;
        reg_data.bits.qpg_qp_restrain_delta_blk32 = static_cfg->qpg_cfg_static.qp_restrain_delta_blk32 ;
        reg_data.bits.qpg_qp_restrain_delta_blk16 = static_cfg->qpg_cfg_static.qp_restrain_delta_blk16;
        reg_data.bits.qpg_qp_restrain_en = static_cfg->qpg_cfg_static.qp_restrain_en;
        reg_data.bits.qpg_qp_restrain_madi_thr = static_cfg->qpg_cfg_static.qp_restrain_madi_thr;
        all_reg->vedu_regs.vedu_qpg_qp_restrain.data = reg_data.data;
    }
    {
        u_vedu_pme_qpg_rc_thr1 reg_data;
        reg_data.bits.pme_min_sad_thr_gain = static_cfg->pme_cfg_static.min_sad_thr_gain;
        reg_data.bits.pme_min_sad_thr_offset = static_cfg->pme_cfg_static.min_sad_thr_offset;
        reg_data.bits.pme_min_sad_thr_offset_cur = static_cfg->pme_cfg_static.pme_min_sad_thr_offset_cur;
        reg_data.bits.pme_min_sad_thr_gain_cur = static_cfg->pme_cfg_static.pme_min_sad_thr_gain_cur;
        all_reg->vedu_regs.vedu_pme_qpg_rc_thr1.data = reg_data.data;
    }
    {
        u_vedu_qpg_cu_min_sad_reg reg_data;
        reg_data.bits.qpg_min_sad_level = static_cfg->qpg_cfg_static.min_sad_level;
        reg_data.bits.qpg_min_sad_madi_en = static_cfg->qpg_cfg_static.min_sad_madi_en;
        reg_data.bits.qpg_min_sad_qp_restrain_en = static_cfg->qpg_cfg_static.min_sad_qp_restrain_en ;
        reg_data.bits.qpg_low_min_sad_mode = static_cfg->qpg_cfg_static.low_min_sad_mode;
        all_reg->vedu_regs.vedu_qpg_cu_min_sad_reg.data  =  reg_data.data;
    }
    {
        u_vedu_qpg_lambda_mode reg_data;
        reg_data.bits.qpg_rdo_lambda_choose_mode = static_cfg->qpg_cfg_static.rdo_lambda_choose_mode;
        reg_data.bits.qpg_lambda_inter_stredge_en = static_cfg->qpg_cfg_static.lambda_inter_stredge_en;
        reg_data.bits.qpg_lambda_qp_offset = static_cfg->qpg_cfg_static.lambda_qp_offset;
        all_reg->vedu_regs.vedu_qpg_lambda_mode.data = reg_data.data;
    }

    cfg_sw_l0_size(reg_cfg);
    cfg_sw_l1_size(reg_cfg);
    {
        u_vedu_pme_new_cost reg_data;
        reg_data.bits.pme_mvp3median_en = static_cfg->pme_cfg_static.mvp3median_en;
        reg_data.bits.pme_new_cost_en = static_cfg->pme_cfg_static.new_cost_en;
        reg_data.bits.pme_cost_lamda2 = static_cfg->pme_cfg_static.cost_lamda2;
        reg_data.bits.pme_cost_lamda1 = static_cfg->pme_cfg_static.cost_lamda1;
        reg_data.bits.pme_cost_lamda0 = static_cfg->pme_cfg_static.cost_lamda0;
        all_reg->vedu_regs.vedu_pme_new_cost.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_30(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_pme_new_lambda reg_data;
        reg_data.bits.vcpi_new_lambda    = static_cfg->pme_cfg_static.vcpi_new_lambda;
        all_reg->vedu_regs.vedu_pme_new_lambda.data = reg_data.data;
    }
    {
        u_vedu_pme_new_madi_th reg_data;
        reg_data.bits.vcpi_new_madi_th0 = static_cfg->pme_cfg_static.vcpi_new_madi_th0;
        reg_data.bits.vcpi_new_madi_th1 = static_cfg->pme_cfg_static.vcpi_new_madi_th1;
        reg_data.bits.vcpi_new_madi_th2 = static_cfg->pme_cfg_static.vcpi_new_madi_th2;
        reg_data.bits.vcpi_new_madi_th3 = static_cfg->pme_cfg_static.vcpi_new_madi_th3;
        all_reg->vedu_regs.vedu_pme_new_madi_th.data = reg_data.data;
    }
    {
        u_vedu_pme_cost_offset reg_data;
        reg_data.bits.pme_l0_cost_offset = static_cfg->pme_cfg_static.l0_cost_offset;
        reg_data.bits.pme_l1_cost_offset = static_cfg->pme_cfg_static.l1_cost_offset;
        all_reg->vedu_regs.vedu_pme_cost_offset.data = reg_data.data;
    }
    {
        u_vedu_pme_adjust_pmemv_h264 reg_data;
        reg_data.bits.pme_adjust_pmemv_dist_times = static_cfg->pme_cfg_static.pme_adjust_pmemv_dist_times;
        reg_data.bits.pme_adjust_pmemv_en = static_cfg->pme_cfg_static.pme_adjust_pmemv_en;
        all_reg->vedu_regs.vedu_pme_adjust_pmemv_h264.data = reg_data.data;
    }
    {
        u_vedu_pme_qpg_rc_th0 reg_data;
        reg_data.bits.pme_madi_dif_thr = static_cfg->pme_cfg_static.madi_dif_thr;
        reg_data.bits.pme_cur_madi_dif_thr = static_cfg->pme_cfg_static.cur_madi_dif_thr;
        all_reg->vedu_regs.vedu_pme_qpg_rc_thr0.data = reg_data.data;
    }
    {
        u_vedu_vlcst_descriptor reg_data;
        reg_data.bits.vlcst_chnid = 0;
        if (reg_cfg->rc_cfg.intra_pic) {
            reg_data.bits.vlcst_idrind = 1;
        } else {
            reg_data.bits.vlcst_idrind = 0;
        }
        all_reg->vedu_regs.vedu_vlcst_descriptor.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_31(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_curld_clip_thr reg_data;
        reg_data.bits.curld_clip_luma_min = static_cfg->base_cfg.clip_luma_min;
        reg_data.bits.curld_clip_luma_max = static_cfg->base_cfg.clip_luma_max;
        reg_data.bits.curld_clip_chrm_min = static_cfg->base_cfg.clip_chrm_min;
        reg_data.bits.curld_clip_chrm_max = static_cfg->base_cfg.clip_chrm_max;
        all_reg->vedu_regs.vedu_curld_clip_thr.data = reg_data.data;
    }
    {
        u_vedu_tqitq_deadzone reg_data;
        reg_data.bits.tqitq_deadzone_intra_slice = static_cfg->q_scaling_cfg.deadzone_intra_slice;
        reg_data.bits.tqitq_deadzone_inter_slice = static_cfg->q_scaling_cfg.deadzone_inter_slice;
        reg_data.bits.tqitq_bitest_magth = 0;
        all_reg->vedu_regs.vedu_tqitq_deadzone.data = reg_data.data;
    }
    {
        u_vedu_vcpi_pintra_texture_strength reg_data;
        reg_data.bits.vcpi_pintra_intra8_texture_strength_multi = 0;
        reg_data.bits.vcpi_pintra_intra8_texture_strength = 0;
        all_reg->vedu_regs.vedu_vcpi_pintra_texture_strength.data = reg_data.data;
    }
    {
        u_vedu_vcpi_noforcezero reg_data;
        reg_data.bits.vcpi_bislayer0flag = 0;
        reg_data.bits.vcpi_bnoforcezero_flag = 0;
        reg_data.bits.vcpi_bnoforcezero_posx = 0;
        reg_data.bits.vcpi_bnoforcezero_posy = 0;
        all_reg->vedu_regs.vedu_vcpi_noforcezero.data = reg_data.data;
    }
}

#ifdef VENC_SUPPORT_ROI
static hi_void direct_cfg_reg_32(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vcpi_roi_cfg0 reg_data;
        reg_data.bits.vctrl_roi_en = static_cfg->roi_cfg.enable[7] | \  /* 7:array idx */
            static_cfg->roi_cfg.enable[6] | /* 6:array idx */
            static_cfg->roi_cfg.enable[5] | static_cfg->roi_cfg.enable[4] | \  /* 4, 5: array index */
            static_cfg->roi_cfg.enable[3] |  /* 3: array index */
            static_cfg->roi_cfg.enable[2] | static_cfg->roi_cfg.enable[1] | \  /* 2: array index */
            static_cfg->roi_cfg.enable[0];
        reg_data.bits.vctrl_region7keep = static_cfg->roi_cfg.keep[7]; /* 7: array index */
        reg_data.bits.vctrl_region6keep = static_cfg->roi_cfg.keep[6]; /* 6: array index */
        reg_data.bits.vctrl_region5keep = static_cfg->roi_cfg.keep[5]; /* 5: array index */
        reg_data.bits.vctrl_region4keep = static_cfg->roi_cfg.keep[4]; /* 4: array index */
        reg_data.bits.vctrl_region3keep = static_cfg->roi_cfg.keep[3]; /* 3: array index */
        reg_data.bits.vctrl_region2keep = static_cfg->roi_cfg.keep[2]; /* 2: array index */
        reg_data.bits.vctrl_region1keep = static_cfg->roi_cfg.keep[1];
        reg_data.bits.vctrl_region0keep = static_cfg->roi_cfg.keep[0];
        reg_data.bits.vctrl_absqp7 = static_cfg->roi_cfg.abs_qp_en[7]; /* 7: array index */
        reg_data.bits.vctrl_absqp6 = static_cfg->roi_cfg.abs_qp_en[6]; /* 6: array index */
        reg_data.bits.vctrl_absqp5 = static_cfg->roi_cfg.abs_qp_en[5]; /* 5: array index */
        reg_data.bits.vctrl_absqp4 = static_cfg->roi_cfg.abs_qp_en[4]; /* 4: array index */
        reg_data.bits.vctrl_absqp3 = static_cfg->roi_cfg.abs_qp_en[3]; /* 3: array index */
        reg_data.bits.vctrl_absqp2 = static_cfg->roi_cfg.abs_qp_en[2]; /* 2: array index */
        reg_data.bits.vctrl_absqp1 = static_cfg->roi_cfg.abs_qp_en[1];
        reg_data.bits.vctrl_absqp0 = static_cfg->roi_cfg.abs_qp_en[0];
        reg_data.bits.vctrl_region7en = static_cfg->roi_cfg.enable[7]; /* 7: array index */
        reg_data.bits.vctrl_region6en = static_cfg->roi_cfg.enable[6]; /* 6: array index */
        reg_data.bits.vctrl_region5en = static_cfg->roi_cfg.enable[5]; /* 5: array index */
        reg_data.bits.vctrl_region4en = static_cfg->roi_cfg.enable[4]; /* 4: array index */
        reg_data.bits.vctrl_region3en = static_cfg->roi_cfg.enable[3]; /* 3: array index */
        reg_data.bits.vctrl_region2en = static_cfg->roi_cfg.enable[2]; /* 2: array index */
        reg_data.bits.vctrl_region1en = static_cfg->roi_cfg.enable[1];
        reg_data.bits.vctrl_region0en = static_cfg->roi_cfg.enable[0];
        all_reg->vedu_regs.vedu_vctrl_roi_cfg0.data = reg_data.data;
    }
    {
        u_vedu_vcpi_roi_cfg1 reg_data;
        reg_data.bits.vctrl_roiqp3 = static_cfg->roi_cfg.qp[3]; /* 3: array index */
        reg_data.bits.vctrl_roiqp2 = static_cfg->roi_cfg.qp[2]; /* 2: array index */
        reg_data.bits.vctrl_roiqp1 = static_cfg->roi_cfg.qp[1];
        reg_data.bits.vctrl_roiqp0 = static_cfg->roi_cfg.qp[0];
        all_reg->vedu_regs.vedu_vctrl_roi_cfg1.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_33(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_roi_cfg2 reg_data;
        reg_data.bits.vctrl_roiqp7 = static_cfg->roi_cfg.qp[7]; /* 7: array index */
        reg_data.bits.vctrl_roiqp6 = static_cfg->roi_cfg.qp[6]; /* 6: array index */
        reg_data.bits.vctrl_roiqp5 = static_cfg->roi_cfg.qp[5]; /* 5: array index */
        reg_data.bits.vctrl_roiqp4 = static_cfg->roi_cfg.qp[4]; /* 4: array index */
        all_reg->vedu_regs.vedu_vctrl_roi_cfg2.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_size_0 reg_data;
        reg_data.bits.vctrl_size0_roiheight = static_cfg->roi_cfg.height[0];
        reg_data.bits.vctrl_size0_roiwidth = static_cfg->roi_cfg.width[0];
        all_reg->vedu_regs.vedu_vctrl_roi_size_0.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_size_1 reg_data;
        reg_data.bits.vctrl_size1_roiheight = static_cfg->roi_cfg.height[1];
        reg_data.bits.vctrl_size1_roiwidth = static_cfg->roi_cfg.width[1];
        all_reg->vedu_regs.vedu_vctrl_roi_size_1.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_size_2 reg_data;
        reg_data.bits.vctrl_size2_roiheight = static_cfg->roi_cfg.height[2]; /* 2: array index */
        reg_data.bits.vctrl_size2_roiwidth = static_cfg->roi_cfg.width[2]; /* 2: array index */
        all_reg->vedu_regs.vedu_vctrl_roi_size_2.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_size_3 reg_data;
        reg_data.bits.vctrl_size3_roiheight = static_cfg->roi_cfg.height[3]; /* 3: array index */
        reg_data.bits.vctrl_size3_roiwidth = static_cfg->roi_cfg.width[3]; /* 3: array index */
        all_reg->vedu_regs.vedu_vctrl_roi_size_3.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_size_4 reg_data;
        reg_data.bits.vctrl_size4_roiheight = static_cfg->roi_cfg.height[4]; /* 4: array index */
        reg_data.bits.vctrl_size4_roiwidth = static_cfg->roi_cfg.width[4]; /* 4: array index */
        all_reg->vedu_regs.vedu_vctrl_roi_size_4.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_size_5 reg_data;
        reg_data.bits.vctrl_size5_roiheight = static_cfg->roi_cfg.height[5]; /* 5: array index */
        reg_data.bits.vctrl_size5_roiwidth = static_cfg->roi_cfg.width[5]; /* 5: array index */
        all_reg->vedu_regs.vedu_vctrl_roi_size_5.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_34(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_roi_size_6 reg_data;
        reg_data.bits.vctrl_size6_roiheight = static_cfg->roi_cfg.height[6]; /* 6: array index */
        reg_data.bits.vctrl_size6_roiwidth = static_cfg->roi_cfg.width[6]; /* 6: array index */
        all_reg->vedu_regs.vedu_vctrl_roi_size_6.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_size_7 reg_data;
        reg_data.bits.vctrl_size7_roiheight = static_cfg->roi_cfg.height[7]; /* 7: array index */
        reg_data.bits.vctrl_size7_roiwidth = static_cfg->roi_cfg.width[7]; /* 7: array index */
        all_reg->vedu_regs.vedu_vctrl_roi_size_7.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_start_0 reg_data;
        reg_data.bits.vctrl_start0_roistarty = static_cfg->roi_cfg.start_y[0];
        reg_data.bits.vctrl_start0_roistartx = static_cfg->roi_cfg.start_x[0];
        all_reg->vedu_regs.vedu_vctrl_roi_start_0.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_start_1 reg_data;
        reg_data.bits.vctrl_start1_roistarty = static_cfg->roi_cfg.start_y[1];
        reg_data.bits.vctrl_start1_roistartx = static_cfg->roi_cfg.start_x[1];
        all_reg->vedu_regs.vedu_vctrl_roi_start_1.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_35(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_roi_start_2 reg_data;
        reg_data.bits.vctrl_start2_roistarty = static_cfg->roi_cfg.start_y[2]; /* 2: array index */
        reg_data.bits.vctrl_start2_roistartx = static_cfg->roi_cfg.start_x[2]; /* 2: array index */
        all_reg->vedu_regs.vedu_vctrl_roi_start_2.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_start_3 reg_data;
        reg_data.bits.vctrl_start3_roistarty = static_cfg->roi_cfg.start_y[3]; /* 3: array index */
        reg_data.bits.vctrl_start3_roistartx = static_cfg->roi_cfg.start_x[3]; /* 3: array index */
        all_reg->vedu_regs.vedu_vctrl_roi_start_3.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_start_4 reg_data;
        reg_data.bits.vctrl_start4_roistarty = static_cfg->roi_cfg.start_y[4]; /* 4: array index */
        reg_data.bits.vctrl_start4_roistartx = static_cfg->roi_cfg.start_x[4]; /* 4: array index */
        all_reg->vedu_regs.vedu_vctrl_roi_start_4.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_start_5 reg_data;
        reg_data.bits.vctrl_start5_roistarty = static_cfg->roi_cfg.start_y[5]; /* 5: array index */
        reg_data.bits.vctrl_start5_roistartx = static_cfg->roi_cfg.start_x[5]; /* 5: array index */
        all_reg->vedu_regs.vedu_vctrl_roi_start_5.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_start_6 reg_data;
        reg_data.bits.vctrl_start6_roistarty = static_cfg->roi_cfg.start_y[6]; /* 6: array index */
        reg_data.bits.vctrl_start6_roistartx = static_cfg->roi_cfg.start_x[6]; /* 6: array index */
        all_reg->vedu_regs.vedu_vctrl_roi_start_6.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_start_7 reg_data;
        reg_data.bits.vctrl_start7_roistarty = static_cfg->roi_cfg.start_y[7]; /* 7: array index */
        reg_data.bits.vctrl_start7_roistartx = static_cfg->roi_cfg.start_x[7]; /* 7: array index */
        all_reg->vedu_regs.vedu_vctrl_roi_start_7.data = reg_data.data;
    }
}
#endif

static hi_void direct_cfg_reg_36(vedu_regs_type *all_reg)
{
    all_reg->vedu_regs.vedu_vctrl_roi_cfg0.data = 0;
}

static hi_void direct_cfg_reg_37(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_intra_bit_weight reg_data;
        reg_data.bits.intra_bit_weight = static_cfg->base_cfg.intra_bit_weight;
        reg_data.bits.mpm8_th = 0;
        all_reg->vedu_regs.vedu_intra_bit_weight.data = reg_data.data;
    }
    {
        u_vedu_intra_rdo_cost_offset_3 reg_data;
        reg_data.bits.intra_h264_rdo_cost_offset     = 0;
        all_reg->vedu_regs.vedu_intra_rdo_cost_offset_3.data = reg_data.data;
    }
    {
        u_vedu_vctrl_intra_rdo_factor_0 reg_data;
        reg_data.bits.vctrl_norm_intra_cu4_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_intra_cu4_rdcost_offset;
        reg_data.bits.vctrl_norm_intra_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_intra_cu8_rdcost_offset;
        reg_data.bits.vctrl_norm_intra_cu16_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_intra_cu16_rdcost_offset;
        reg_data.bits.vctrl_strmov_intra_cu4_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_intra_cu4_rdcost_offset;
        reg_data.bits.vctrl_strmov_intra_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_intra_cu8_rdcost_offset;
        reg_data.bits.vctrl_strmov_intra_cu16_rdcost_offset = \
            dynamic_cfg->img_improve_cfg_dynamic.strmov_intra_cu16_rdcost_offset;
        all_reg->vedu_regs.vedu_vctrl_intra_rdo_factor_0.data = reg_data.data;
    }
    {
        u_vedu_vctrl_intra_rdo_factor_1 reg_data;
        reg_data.bits.vctrl_skin_intra_cu32_rdcost_offset = 0;
        reg_data.bits.vctrl_skin_intra_cu16_rdcost_offset = 0;
        reg_data.bits.vctrl_skin_intra_cu8_rdcost_offset = 0;
        reg_data.bits.vctrl_skin_intra_cu4_rdcost_offset = 0;
        reg_data.bits.vctrl_sobel_str_intra_cu32_rdcost_offset = 0xa;
        reg_data.bits.vctrl_sobel_str_intra_cu16_rdcost_offset = 0xa;
        reg_data.bits.vctrl_sobel_str_intra_cu8_rdcost_offset = 0;
        reg_data.bits.vctrl_sobel_str_intra_cu4_rdcost_offset = 0;
        all_reg->vedu_regs.vedu_vctrl_intra_rdo_factor_1.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_38(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_intra_rdo_factor_2 reg_data;
        reg_data.bits.vctrl_hedge_intra_cu4_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_intra_cu4_rdcost_offset;
        reg_data.bits.vctrl_hedge_intra_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_intra_cu8_rdcost_offset;
        reg_data.bits.vctrl_hedge_intra_cu16_rdcost_offset = \
            dynamic_cfg->img_improve_cfg_dynamic.hedge_intra_cu16_rdcost_offset;
        reg_data.bits.vctrl_sobel_tex_intra_cu4_rdcost_offset = 0; /* sobel_tex_intra_cu4_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_intra_cu8_rdcost_offset = 0; /* sobel_tex_intra_cu8_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_intra_cu16_rdcost_offset = 1; /* sobel_tex_intra_cu16_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_intra_cu32_rdcost_offset = 1;
        all_reg->vedu_regs.vedu_vctrl_intra_rdo_factor_2.data = reg_data.data;
    }
    {
        u_vedu_intra_rdo_cost_offset_0 reg_data;
        reg_data.bits.intra_cu16_rdo_cost_offset = 0;
        reg_data.bits.intra_cu32_rdo_cost_offset = 0;
        all_reg->vedu_regs.vedu_intra_rdo_cost_offset_0.data = reg_data.data;
    }
    {
        u_vedu_intra_rdo_cost_offset_1 reg_data;
        reg_data.bits.intra_cu4_rdo_cost_offset = 0;
        reg_data.bits.intra_cu8_rdo_cost_offset = 0;
        all_reg->vedu_regs.vedu_intra_rdo_cost_offset_1.data = reg_data.data;
    }
    {
        u_vedu_intra_no_dc_cost_offset_0 reg_data;
        reg_data.bits.intra_cu16_non_dc_mode_offset = 0;
        reg_data.bits.intra_cu32_non_dc_mode_offset = 0;
        all_reg->vedu_regs.vedu_intra_no_dc_cost_offset_0.data = reg_data.data;
    }
    {
        u_vedu_intra_no_dc_cost_offset_1 reg_data;
        reg_data.bits.intra_cu8_non_dc_mode_offset = 0;
        reg_data.bits.intra_cu4_non_dc_mode_offset = 0;
        all_reg->vedu_regs.vedu_intra_no_dc_cost_offset_1.data = reg_data.data;
    }
    {
        u_vedu_pme_skip_large_res reg_data;
        reg_data.bits.pme_skip_sad_thr_offset = static_cfg->pme_cfg_static.pme_skip_sad_thr_offset;
        reg_data.bits.pme_skip_sad_thr_gain  = static_cfg->pme_cfg_static.pme_skip_sad_thr_gain;
        reg_data.bits.pme_skip_large_res_det = static_cfg->pme_cfg_static.pme_skip_large_res_det;
        all_reg->vedu_regs.vedu_pme_skip_large_res.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_39(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_mrg_adj_weight reg_data;
        reg_data.bits.cu8_fz_weight = 0;
        reg_data.bits.cu16_fz_weight = 0;
        reg_data.bits.cu32_fz_weight = 0;
        reg_data.bits.cu64_fz_weight = 0;
        reg_data.bits.cu8_fz_adapt_weight  = 0x3;
        reg_data.bits.cu16_fz_adapt_weight = 0x3;
        reg_data.bits.cu32_fz_adapt_weight = 0x3;
        reg_data.bits.cu64_fz_adapt_weight = 0x3;
        all_reg->vedu_regs.vedu_mrg_adj_weight.data = reg_data.data;
    }
    {
        u_vedu_qpg_res_coef reg_data;
        reg_data.bits.vcpi_small_res_coef = static_cfg->qpg_cfg_static.vcpi_small_res_coef;
        reg_data.bits.vcpi_large_res_coef = static_cfg->qpg_cfg_static.vcpi_large_res_coef;
        reg_data.bits.vcpi_res_coef_en = static_cfg->qpg_cfg_static.vcpi_res_coef_en;
        all_reg->vedu_regs.vedu_qpg_res_coef.data  = reg_data.data;
    }
    {
        u_vedu_pme_bias_cost0 reg_data;
        reg_data.bits.fme_pu8_bias_cost  = 0;
        reg_data.bits.fme_pu16_bias_cost = 0;
        all_reg->vedu_regs.vedu_fme_bias_cost0.data = reg_data.data;
    }
    {
        u_vedu_pme_bias_cost1 reg_data;
        reg_data.bits.fme_pu32_bias_cost = 0;
        reg_data.bits.fme_pu64_bias_cost = 0;
        all_reg->vedu_regs.vedu_fme_bias_cost1.data = reg_data.data;
    }
    {
        u_vedu_mrg_bias_cost0 reg_data;
        reg_data.bits.mrg_pu8_bias_cost  = 0;
        reg_data.bits.mrg_pu16_bias_cost = 0;
        all_reg->vedu_regs.vedu_mrg_bias_cost0.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_40(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_mrg_bias_cost1 reg_data;
        reg_data.bits.mrg_pu32_bias_cost = 0;
        reg_data.bits.mrg_pu64_bias_cost = 0;
        all_reg->vedu_regs.vedu_mrg_bias_cost1.data = reg_data.data;
    }
    {
        u_vedu_mrg_abs_offset0 reg_data;
        reg_data.bits.mrg_pu8_abs_offset  = 0;
        reg_data.bits.mrg_pu16_abs_offset = 0;
        all_reg->vedu_regs.vedu_mrg_abs_offset0.data = reg_data.data;
    }
    {
        u_vedu_mrg_abs_offset1 reg_data;
        reg_data.bits.mrg_pu32_abs_offset = 0;
        reg_data.bits.mrg_pu64_abs_offset = 0;
        all_reg->vedu_regs.vedu_mrg_abs_offset1.data = reg_data.data;
    }
    {
        u_vedu_vctrl_mrg_rdo_factor_0 reg_data;
        reg_data.bits.vctrl_norm_mrg_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_mrg_cu64_rdcost_offset;
        reg_data.bits.vctrl_norm_mrg_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_mrg_cu32_rdcost_offset;
        reg_data.bits.vctrl_norm_mrg_cu16_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_mrg_cu16_rdcost_offset;
        reg_data.bits.vctrl_norm_mrg_cu8_rdcost_offset = static_cfg->img_improve_cfg_static.norm_mrg_cu8_rdcost_offset;
        reg_data.bits.vctrl_strmov_mrg_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_mrg_cu64_rdcost_offset;
        reg_data.bits.vctrl_strmov_mrg_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_mrg_cu32_rdcost_offset;
        reg_data.bits.vctrl_strmov_mrg_cu16_rdcost_offset = \
            dynamic_cfg->img_improve_cfg_dynamic.strmov_mrg_cu16_rdcost_offset;
        reg_data.bits.vctrl_strmov_mrg_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_mrg_cu8_rdcost_offset;
        all_reg->vedu_regs.vedu_vctrl_mrg_rdo_factor_0.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_41(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_mrg_rdo_factor_1 reg_data;
        reg_data.bits.vctrl_skin_mrg_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.skin_mrg_cu64_rdcost_offset;
        reg_data.bits.vctrl_skin_mrg_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.skin_mrg_cu32_rdcost_offset;
        reg_data.bits.vctrl_skin_mrg_cu16_rdcost_offset = \
            dynamic_cfg->img_improve_cfg_dynamic.skin_mrg_cu16_rdcost_offset;
        reg_data.bits.vctrl_skin_mrg_cu8_rdcost_offset = static_cfg->img_improve_cfg_static.skin_mrg_cu8_rdcost_offset;
        reg_data.bits.vctrl_sobel_str_mrg_cu64_rdcost_offset = 0xa; /* sobel_str_mrg_cu64_rdcost_offset */
        reg_data.bits.vctrl_sobel_str_mrg_cu32_rdcost_offset = 0xa; /* sobel_str_mrg_cu32_rdcost_offset */
        reg_data.bits.vctrl_sobel_str_mrg_cu16_rdcost_offset = 0xa; /* sobel_str_mrg_cu16_rdcost_offset */
        reg_data.bits.vctrl_sobel_str_mrg_cu8_rdcost_offset = 0; /* sobel_str_mrg_cu8_rdcost_offset */
        all_reg->vedu_regs.vedu_vctrl_mrg_rdo_factor_1.data = reg_data.data;
    }
    {
        u_vedu_vctrl_mrg_rdo_factor_2 reg_data;
        reg_data.bits.vctrl_hedge_mrg_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_mrg_cu64_rdcost_offset;
        reg_data.bits.vctrl_hedge_mrg_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_mrg_cu32_rdcost_offset;
        reg_data.bits.vctrl_hedge_mrg_cu16_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_mrg_cu16_rdcost_offset;
        reg_data.bits.vctrl_hedge_mrg_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_mrg_cu8_rdcost_offset;
        reg_data.bits.vctrl_sobel_tex_mrg_cu64_rdcost_offset = 1; /* sobel_tex_mrg_cu64_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_mrg_cu32_rdcost_offset = 1; /* sobel_tex_mrg_cu32_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_mrg_cu16_rdcost_offset = 1; /* sobel_tex_mrg_cu16_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_mrg_cu8_rdcost_offset = 0; /* sobel_tex_mrg_cu8_rdcost_offset */
        all_reg->vedu_regs.vedu_vctrl_mrg_rdo_factor_2.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_42(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_fme_rdo_factor_0 reg_data;
        reg_data.bits.vctrl_norm_fme_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_fme_cu64_rdcost_offset;
        reg_data.bits.vctrl_norm_fme_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_fme_cu32_rdcost_offset;
        reg_data.bits.vctrl_norm_fme_cu16_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_fme_cu16_rdcost_offset;
        reg_data.bits.vctrl_norm_fme_cu8_rdcost_offset = static_cfg->img_improve_cfg_static.norm_fme_cu8_rdcost_offset;
        reg_data.bits.vctrl_strmov_fme_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_fme_cu64_rdcost_offset;
        reg_data.bits.vctrl_strmov_fme_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_fme_cu32_rdcost_offset;
        reg_data.bits.vctrl_strmov_fme_cu16_rdcost_offset = \
            dynamic_cfg->img_improve_cfg_dynamic.strmov_fme_cu16_rdcost_offset;
        reg_data.bits.vctrl_strmov_fme_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_fme_cu8_rdcost_offset;
        all_reg->vedu_regs.vedu_vctrl_fme_rdo_factor_0.data = reg_data.data;
    }
    {
        u_vedu_vctrl_fme_rdo_factor_1 reg_data;
        reg_data.bits.vctrl_skin_fme_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.skin_fme_cu64_rdcost_offset;
        reg_data.bits.vctrl_skin_fme_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.skin_fme_cu32_rdcost_offset;
        reg_data.bits.vctrl_skin_fme_cu16_rdcost_offset = \
            dynamic_cfg->img_improve_cfg_dynamic.skin_fme_cu16_rdcost_offset;
        reg_data.bits.vctrl_skin_fme_cu8_rdcost_offset = static_cfg->img_improve_cfg_static.skin_fme_cu8_rdcost_offset;
        reg_data.bits.vctrl_sobel_str_fme_cu64_rdcost_offset = 0xa; /* sobel_str_fme_cu64_rdcost_offset */
        reg_data.bits.vctrl_sobel_str_fme_cu32_rdcost_offset = 0xa; /* sobel_str_fme_cu32_rdcost_offset */
        reg_data.bits.vctrl_sobel_str_fme_cu16_rdcost_offset = 0xa; /* sobel_str_fme_cu16_rdcost_offset */
        reg_data.bits.vctrl_sobel_str_fme_cu8_rdcost_offset  = 0; /* sobel_str_fme_cu8_rdcost_offset */
        all_reg->vedu_regs.vedu_vctrl_fme_rdo_factor_1.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_43(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_fme_rdo_factor_2 reg_data;
        reg_data.bits.vctrl_hedge_fme_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_fme_cu64_rdcost_offset;
        reg_data.bits.vctrl_hedge_fme_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_fme_cu32_rdcost_offset;
        reg_data.bits.vctrl_hedge_fme_cu16_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_fme_cu16_rdcost_offset;
        reg_data.bits.vctrl_hedge_fme_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_fme_cu8_rdcost_offset;
        reg_data.bits.vctrl_sobel_tex_fme_cu64_rdcost_offset = 1; /* sobel_tex_fme_cu64_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_fme_cu32_rdcost_offset = 1; /* sobel_tex_fme_cu32_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_fme_cu16_rdcost_offset = 1; /* sobel_tex_fme_cu16_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_fme_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.sobel_tex_fme_cu8_rdcost_offset;
        all_reg->vedu_regs.vedu_vctrl_fme_rdo_factor_2.data = reg_data.data;
    }
    {
        u_vedu_ime_rdocfg reg_data;
        reg_data.bits.ime_lambdaoff16 = static_cfg->base_cfg.vcpi_lambdaoff16;
        reg_data.bits.ime_lambdaoff8 = static_cfg->base_cfg.vcpi_lambdaoff8;
        all_reg->vedu_regs.vedu_ime_rdocfg.data = reg_data.data;
    }
    {
        u_vedu_vcpi_pintra_thresh0 reg_data;
        reg_data.bits.vcpi_pintra_pu16_amp_th = 0x80;
        reg_data.bits.vcpi_pintra_pu32_amp_th = 0x80;

        all_reg->vedu_regs.vedu_vcpi_pintra_thresh0.data = reg_data.data;
    }
    {
        u_vedu_vcpi_pintra_thresh1 reg_data;
        reg_data.bits.vcpi_pintra_pu32_std_th = 0x80;

        all_reg->vedu_regs.vedu_vcpi_pintra_thresh1.data = reg_data.data;
    }
    {
        u_vedu_vcpi_pintra_thresh2 reg_data;
        reg_data.bits.vcpi_rpintra_pu4_strong_edge_th = 0;
        reg_data.bits.vcpi_rpintra_pu4_mode_distance_th = 0;
        reg_data.bits.vcpi_rpintra_bypass = 0;
        all_reg->vedu_regs.vedu_vcpi_pintra_thresh2.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_44(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_pme_skin_thr reg_data;
        reg_data.bits.pme_skin_v_min_thr = static_cfg->pme_cfg_static.skin_v_min_thr;
        reg_data.bits.pme_skin_v_max_thr = static_cfg->pme_cfg_static.skin_v_max_thr;
        reg_data.bits.pme_skin_u_min_thr = static_cfg->pme_cfg_static.skin_u_min_thr;
        reg_data.bits.pme_skin_u_max_thr = static_cfg->pme_cfg_static.skin_u_max_thr;
        all_reg->vedu_regs.vedu_pme_skin_thr.data = reg_data.data;
    }
    {
        u_vedu_pme_strong_edge reg_data;
        reg_data.bits.pme_still_scene_thr = dynamic_cfg->pme_cfg_dynamic.still_scene_thr;
        reg_data.bits.pme_strong_edge_cnt = dynamic_cfg->pme_cfg_dynamic.high_edge_cnt;
        reg_data.bits.pme_strong_edge_thr = dynamic_cfg->pme_cfg_dynamic.high_edge_thr;
        reg_data.bits.pme_skin_num = dynamic_cfg->pme_cfg_dynamic.skin_num;
        all_reg->vedu_regs.vedu_pme_strong_edge.data = reg_data.data;
    }
    {
        u_vedu_pme_skin_sad_thr reg_data;
        reg_data.bits.vcpi_skin_sad_thr_offset = static_cfg->pme_cfg_static.vcpi_skin_sad_thr_offset;
        reg_data.bits.vcpi_skin_sad_thr_gain   = static_cfg->pme_cfg_static.vcpi_skin_sad_thr_gain;
        all_reg->vedu_regs.vedu_pme_skin_sad_thr.data = reg_data.data;
    }
    {
        u_vedu_qpg_skin reg_data;
        reg_data.bits.qpg_skin_max_qp = dynamic_cfg->qpg_cfg_dynamic.skin_max_qp;
        reg_data.bits.qpg_skin_qp_delta = dynamic_cfg->qpg_cfg_dynamic.skin_qp_delta;
        reg_data.bits.qpg_skin_min_qp = static_cfg->qpg_cfg_static.skin_min_qp;
        all_reg->vedu_regs.vedu_qpg_skin.data = reg_data.data;
    }
    {
        u_vedu_pme_inter_strong_edge reg_data;
        reg_data.bits.pme_interstrongedge_madi_thr = dynamic_cfg->pme_cfg_dynamic.interstrongedge_madi_thr;
        reg_data.bits.pme_interdiff_max_min_madi_times = dynamic_cfg->pme_cfg_dynamic.interdiff_max_min_madi_times;
        reg_data.bits.pme_interdiff_max_min_madi_abs = dynamic_cfg->pme_cfg_dynamic.interdiff_max_min_madi_abs;
        all_reg->vedu_regs.vedu_pme_inter_strong_edge.data = reg_data.data;
    }
    {
        u_vedu_qpg_hedge reg_data;
        reg_data.bits.qpg_hedge_max_qp = static_cfg->qpg_cfg_static.strong_edge_max_qp;
        reg_data.bits.qpg_hedge_qp_delta = static_cfg->qpg_cfg_static.strong_edge_qp_delta;
        reg_data.bits.qpg_hedge_min_qp = static_cfg->qpg_cfg_static.stredge_min_qp;
        all_reg->vedu_regs.vedu_qpg_hedge.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_45(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_pme_large_move_thr reg_data;
        reg_data.bits.pme_move_sad_thr = dynamic_cfg->pme_cfg_dynamic.move_sad_thr;
        reg_data.bits.pme_move_scene_thr = static_cfg->pme_cfg_static.move_scene_thr;
        all_reg->vedu_regs.vedu_pme_large_move_thr.data = reg_data.data;
    }
    {
        u_vedu_qpg_hedge_move reg_data;
        reg_data.bits.qpg_hedge_move_max_qp = static_cfg->qpg_cfg_static.strong_edge_move_max_qp;
        reg_data.bits.qpg_hedge_move_qp_delta = static_cfg->qpg_cfg_static.strong_edge_move_qp_delta;
        reg_data.bits.qpg_hedge_move_min_qp = static_cfg->qpg_cfg_static.stredge_move_min_qp;
        all_reg->vedu_regs.vedu_qpg_hedge_move.data = reg_data.data;
    }
    {
        u_vedu_pme_move_scene_thr reg_data;
        reg_data.bits.vcpi_move_scene_mv_thr = static_cfg->pme_cfg_static.vcpi_move_scene_mv_thr;
        reg_data.bits.vcpi_move_scene_mv_en  = static_cfg->pme_cfg_static.vcpi_move_scene_mv_en;
        all_reg->vedu_regs.vedu_pme_move_scene_thr.data = reg_data.data;
    }
    {
        u_vedu_pme_low_luma_thr reg_data;
        reg_data.bits.pme_low_luma_thr = static_cfg->pme_cfg_static.low_luma_thr;
        reg_data.bits.pme_low_luma_madi_thr = static_cfg->pme_cfg_static.low_luma_madi_thr;
        reg_data.bits.pme_high_luma_thr = static_cfg->pme_cfg_static.pme_high_luma_thr;
        all_reg->vedu_regs.vedu_pme_low_luma_thr.data = reg_data.data;
    }
    {
        u_vedu_qpg_lowluma reg_data;
        reg_data.bits.qpg_lowluma_max_qp = static_cfg->qpg_cfg_static.lowluma_max_qp;
        reg_data.bits.qpg_lowluma_qp_delta = static_cfg->qpg_cfg_static.lowluma_qp_delta;
        reg_data.bits.qpg_lowluma_min_qp = static_cfg->qpg_cfg_static.lowluma_min_qp;
        all_reg->vedu_regs.vedu_qpg_lowluma.data = reg_data.data;
    }
    {
        u_vedu_pme_chroma_flat reg_data;
        reg_data.bits.pme_flat_u_thr_low = 125; /* 125 static_cfg->pme_cfg_static.pme_flat_u_thr_low */
        reg_data.bits.pme_flat_u_thr_high = 133; /* 133 static_cfg->pme_cfg_static.pme_flat_u_thr_high */
        reg_data.bits.pme_flat_v_thr_low = 123; /* 123 static_cfg->pme_cfg_static.pme_flat_v_thr_low */
        reg_data.bits.pme_flat_v_thr_high = 133; /*  133 static_cfg->pme_cfg_static.pme_flat_v_thr_high */
        all_reg->vedu_regs.vedu_pme_chroma_flat.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_46(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_pme_luma_flat reg_data;
        reg_data.bits.pme_flat_high_luma_thr = 0x64 ; /* static_cfg->pme_cfg_static.pme_flat_high_luma_thr */
        reg_data.bits.pme_flat_low_luma_thr = 40; /* 40 static_cfg->pme_cfg_static.pme_flat_low_luma_thr */
        reg_data.bits.pme_flat_luma_madi_thr = 0x2; /* static_cfg->pme_cfg_static.pme_flat_luma_madi_thr */
        reg_data.bits.pme_flat_pmemv_thr = 1; /* static_cfg->pme_cfg_static.pme_flat_pmemv_thr */
        all_reg->vedu_regs.vedu_pme_luma_flat.data = reg_data.data;
    }
    {
        u_vedu_pme_madi_flat reg_data;
        reg_data.bits.pme_flat_madi_times = 1; /* static_cfg->pme_cfg_static.pme_flat_madi_times */
        reg_data.bits.pme_flat_region_cnt = 0xd; /* static_cfg->pme_cfg_static.pme_flat_region_cnt */
        reg_data.bits.pme_flat_icount_thr = 200; /* 200 static_cfg->pme_cfg_static.pme_flat_icount_thr */
        reg_data.bits.pme_flat_pmesad_thr = 0x40; /* static_cfg->pme_cfg_static.pme_flat_pmesad_thr */
        all_reg->vedu_regs.vedu_pme_madi_flat.data = reg_data.data;
    }
    {
        u_vedu_qpg_flat_region reg_data;
        reg_data.bits.qpg_flat_region_qp_delta = static_cfg->qpg_cfg_static.flat_region_qp_delta;
        reg_data.bits.qpg_flat_region_max_qp = static_cfg->qpg_cfg_static.flat_region_max_qp;
        reg_data.bits.qpg_flat_region_min_qp = static_cfg->qpg_cfg_static.flat_region_min_qp;
        reg_data.bits.vcpi_cu32_use_cu16_mean_en = 0;
        all_reg->vedu_regs.vedu_qpg_flat_region.data = reg_data.data;
    }
    {
        u_vedu_chroma_protect reg_data;
        reg_data.bits.vcpi_chroma_qp_delta = static_cfg->qpg_cfg_static.vcpi_chroma_qp_delta;
        reg_data.bits.vcpi_chroma_min_qp = static_cfg->qpg_cfg_static.vcpi_chroma_min_qp;
        reg_data.bits.vcpi_chroma_max_qp = static_cfg->qpg_cfg_static.vcpi_chroma_max_qp;
        reg_data.bits.vcpi_chroma_prot_en = static_cfg->qpg_cfg_static.vcpi_chroma_prot_en;
        all_reg->vedu_regs.vedu_chroma_protect.data = reg_data.data;
    }
    {
        u_vedu_pme_chroma_strong_edge reg_data;
        reg_data.bits.vcpi_strong_edge_thr_u = static_cfg->pme_cfg_static.vcpi_strong_edge_thr_u;
        reg_data.bits.vcpi_strong_edge_cnt_u = static_cfg->pme_cfg_static.vcpi_strong_edge_cnt_u;
        reg_data.bits.vcpi_strong_edge_thr_v = static_cfg->pme_cfg_static.vcpi_strong_edge_thr_v;
        reg_data.bits.vcpi_strong_edge_cnt_v = static_cfg->pme_cfg_static.vcpi_strong_edge_cnt_v;
        all_reg->vedu_regs.vedu_pme_chroma_strong_edge.data = reg_data.data;
    }
    {
        u_vedu_chroma_sad_thr reg_data;
        reg_data.bits.vcpi_chroma_sad_thr_offset = static_cfg->pme_cfg_static.vcpi_chroma_sad_thr_offset;
        reg_data.bits.vcpi_chroma_sad_thr_gain = static_cfg->pme_cfg_static.vcpi_chroma_sad_thr_gain;
        all_reg->vedu_regs.vedu_chroma_sad_thr.data = reg_data.data;
    }

}

static hi_void direct_cfg_reg_47(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_chroma_fg_thr  reg_data;
        reg_data.bits.vcpi_chroma_u0_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_u0_thr_min;
        reg_data.bits.vcpi_chroma_u0_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_u0_thr_max;
        reg_data.bits.vcpi_chroma_v0_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_v0_thr_min;
        reg_data.bits.vcpi_chroma_v0_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_v0_thr_max;
        all_reg->vedu_regs.vedu_chroma_fg_thr.data = reg_data.data;
    }
    {
        u_vedu_chroma_bg_thr reg_data;
        reg_data.bits.vcpi_chroma_u1_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_u1_thr_min;
        reg_data.bits.vcpi_chroma_u1_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_u1_thr_max;
        reg_data.bits.vcpi_chroma_v1_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_v1_thr_min;
        reg_data.bits.vcpi_chroma_v1_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_v1_thr_max;
        all_reg->vedu_regs.vedu_chroma_bg_thr.data = reg_data.data;
    }
    {
        u_vedu_chroma_sum_fg_thr reg_data;
        reg_data.bits.vcpi_chroma_uv0_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_uv0_thr_min;
        reg_data.bits.vcpi_chroma_uv0_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_uv0_thr_max;
        all_reg->vedu_regs.vedu_chroma_sum_fg_thr.data = reg_data.data;
    }
    {
        u_vedu_chroma_sum_bg_thr reg_data;
        reg_data.bits.vcpi_chroma_uv1_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_uv1_thr_min;
        reg_data.bits.vcpi_chroma_uv1_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_uv1_thr_max;
        all_reg->vedu_regs.vedu_chroma_sum_bg_thr.data = reg_data.data;
    }
    {
        u_vedu_chroma_fg_count_thr reg_data;
        reg_data.bits.vcpi_chroma_count0_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_count0_thr_min;
        reg_data.bits.vcpi_chroma_count0_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_count0_thr_max;
        all_reg->vedu_regs.vedu_chroma_fg_count_thr.data = reg_data.data;
    }
    {
        u_vedu_chroma_bg_count_thr reg_data;
        reg_data.bits.vcpi_chroma_count1_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_count1_thr_min;
        reg_data.bits.vcpi_chroma_count1_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_count1_thr_max;
        all_reg->vedu_regs.vedu_chroma_bg_count_thr.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_48(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_pme_intrablk_det reg_data;
        reg_data.bits.pme_intrablk_det_cost_thr0 = static_cfg->pme_cfg_static.pme_intrablk_det_cost_thr0;
        reg_data.bits.pme_pskip_mvy_consistency_thr = static_cfg->pme_cfg_static.pskip_mvy_consistency_thr;
        reg_data.bits.pme_pskip_mvx_consistency_thr = static_cfg->pme_cfg_static.pskip_mvx_consistency_thr;
        all_reg->vedu_regs.vedu_pme_intrablk_det.data = reg_data.data;
    }
    {
        u_vedu_pme_intrablk_det_thr reg_data;
        reg_data.bits.pme_intrablk_det_mv_dif_thr1  = static_cfg->pme_cfg_static.pme_intrablk_det_mv_dif_thr1;
        reg_data.bits.pme_intrablk_det_mv_dif_thr0  = static_cfg->pme_cfg_static.pme_intrablk_det_mv_dif_thr0;
        reg_data.bits.pme_intrablk_det_mvy_thr = static_cfg->pme_cfg_static.pme_intrablk_det_mvy_thr;
        reg_data.bits.pme_intrablk_det_mvx_thr = static_cfg->pme_cfg_static.pme_intrablk_det_mvx_thr;
        all_reg->vedu_regs.vedu_pme_intrablk_det_thr.data = reg_data.data;
    }
    {
        u_vedu_qpg_intra_det reg_data;
        reg_data.bits.qpg_intra_det_qp_delta = static_cfg->qpg_cfg_static.intra_det_qp_delta;
        reg_data.bits.qpg_intra_det_max_qp = static_cfg->qpg_cfg_static.intra_det_max_qp;
        reg_data.bits.qpg_intra_det_min_qp = static_cfg->qpg_cfg_static.intra_det_min_qp;
        all_reg->vedu_regs.vedu_qpg_intra_det.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_49(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg)
{
    hi_s32 i;

    if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
        if (reg_cfg->rc_cfg.avbr && !reg_cfg->rc_cfg.intra_pic) {
            /* avbr p increase lamda */
            for (i = 0; i < 40; i++) {  /* 40: array size */
                u_vedu_qpg_qp_lambda_ctrl_reg00 reg_data;
                reg_data.bits.qpg_lambda00 = g_h265_lambda_ssd[i + 3]; /* 3: array offset */
                all_reg->vedu_regs.vedu_qpg_qp_lambda_ctrl_reg00[i].data = reg_data.data;
            }
            for (i = 0; i < 40; i++) { /* 40: array size */
                u_vedu_qpg_qp_lambda_ctrl_reg00 reg_data;
                reg_data.bits.qpg_lambda00 = g_h265_lambda_sad[i + 3]; /* 3: array offset */
                all_reg->vedu_regs.vedu_qpg_qp_lambda_ctrl_reg00[i + 40].data = reg_data.data; /* 40: array offset */
            }
        } else {
            for (i = 0; i < 80; i++) { /* 80: array size */
                u_vedu_qpg_qp_lambda_ctrl_reg00 reg_data;
                reg_data.bits.qpg_lambda00 = dynamic_cfg->qpg_cfg_dynamic.lambda[i];
                all_reg->vedu_regs.vedu_qpg_qp_lambda_ctrl_reg00[i].data = reg_data.data;
            }
        }
    } else {
        for (i = 0; i < 80; i++) { /* 80: array size */
            u_vedu_qpg_qp_lambda_ctrl_reg00 reg_data;
            reg_data.bits.qpg_lambda00 = dynamic_cfg->qpg_cfg_dynamic.lambda[i];
            all_reg->vedu_regs.vedu_qpg_qp_lambda_ctrl_reg00[i].data = reg_data.data;
        }
    }

    {
        u_vedu_sao_ssd_area0_start reg_data;
        reg_data.bits.sao_area0_start_lcux = 0;
        reg_data.bits.sao_area0_start_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area0_start.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area0_end reg_data;
        reg_data.bits.sao_area0_end_lcux = 0;
        reg_data.bits.sao_area0_end_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area0_end.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area1_start reg_data;
        reg_data.bits.sao_area1_start_lcux = 0;
        reg_data.bits.sao_area1_start_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area1_start.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_50(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_sao_ssd_area1_end reg_data;
        reg_data.bits.sao_area1_end_lcux = 0;
        reg_data.bits.sao_area1_end_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area1_end.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area2_start reg_data;
        reg_data.bits.sao_area2_start_lcux = 0;
        reg_data.bits.sao_area2_start_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area2_start.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area2_end reg_data;
        reg_data.bits.sao_area2_end_lcux = 0;
        reg_data.bits.sao_area2_end_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area2_end.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area3_start reg_data;
        reg_data.bits.sao_area3_start_lcux = 0;
        reg_data.bits.sao_area3_start_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area3_start.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area3_end reg_data;
        reg_data.bits.sao_area3_end_lcux = 0;
        reg_data.bits.sao_area3_end_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area3_end.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area4_start reg_data;
        reg_data.bits.sao_area4_start_lcux = 0;
        reg_data.bits.sao_area4_start_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area4_start.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_51(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_sao_ssd_area4_end reg_data;
        reg_data.bits.sao_area4_end_lcux = 0;
        reg_data.bits.sao_area4_end_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area4_end.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area5_start reg_data;
        reg_data.bits.sao_area5_start_lcux = 0;
        reg_data.bits.sao_area5_start_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area5_start.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area5_end reg_data;
        reg_data.bits.sao_area5_end_lcux = 0;
        reg_data.bits.sao_area5_end_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area5_end.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area6_start reg_data;
        reg_data.bits.sao_area6_start_lcux = 0;
        reg_data.bits.sao_area6_start_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area6_start.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area6_end reg_data;
        reg_data.bits.sao_area6_end_lcux = 0;
        reg_data.bits.sao_area6_end_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area6_end.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area7_start reg_data;
        reg_data.bits.sao_area7_start_lcux = 0;
        reg_data.bits.sao_area7_start_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area7_start.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area7_end reg_data;
        reg_data.bits.sao_area7_end_lcux = 0;
        reg_data.bits.sao_area7_end_lcuy = 0;
        all_reg->vedu_regs.vedu_sao_ssd_area7_end.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_52(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_dynamic_cfg *dynamic_cfg,
                                 venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vcpi_pic_strong_en reg_data;
        reg_data.bits.vcpi_still_en = dynamic_cfg->img_improve_cfg_dynamic.static_en;
        reg_data.bits.vcpi_strong_edge_en = dynamic_cfg->img_improve_cfg_dynamic.highedge_en;
        reg_data.bits.vcpi_skin_en = static_cfg->img_improve_cfg_static.skin_en;
        reg_data.bits.vcpi_skin_close_angle = static_cfg->base_cfg.vcpi_skin_close_angle;
        reg_data.bits.vcpi_rounding_sobel_en = static_cfg->base_cfg.vcpi_rounding_sobel_en;
        all_reg->vedu_regs.vedu_vcpi_pic_strong_en.data = reg_data.data;
    }
    {
        u_vedu_vctrl_nm_acoffset_denoise reg_data;
        reg_data.bits.vctrl_chos_oth_blk_spec_offset = 0;
        reg_data.bits.vctrl_chos_oth_blk_offset16 = 0;
        reg_data.bits.vctrl_i_chos_cur_blk_offset16 = 0;
        reg_data.bits.vctrl_low_freq_ac_blk16 = 0;
        reg_data.bits.vctrl_chos_oth_blk_offset32 = 0;
        reg_data.bits.vctrl_i_chos_cur_blk_offset32 = 0;
        reg_data.bits.vctrl_low_freq_ac_blk32 = 0;
        all_reg->vedu_regs.vedu_vctrl_nm_acoffset_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_nm_engthr_denoise reg_data;
        reg_data.bits.vctrl_chos_oth_blk_spec_thr = 0;
        reg_data.bits.vctrl_chos_oth_blk_eng_thr = 0;
        reg_data.bits.vctrl_chos_oth_blk_thr3 = 0;
        reg_data.bits.vctrl_chos_oth_blk_thr2 = 0;
        reg_data.bits.vctrl_chos_oth_blk_thr = 0;
        reg_data.bits.vctrl_rm_all_high_ac_thr = 0;
        all_reg->vedu_regs.vedu_vctrl_nm_engthr_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_nm_tu8_denoise reg_data;
        reg_data.bits.vctrl_ring_eng_thr = 0;
        reg_data.bits.vctrl_ring_ac_thr = 0;
        reg_data.bits.vctrl_p_chos_oth_blk_offset = 0;
        reg_data.bits.vctrl_p_chos_oth_blk_offset8 = 0;
        reg_data.bits.vctrl_i_chos_cur_blk_offset8 = 0;
        reg_data.bits.vctrl_i_low_freq_ac_blk8 = 0;
        reg_data.bits.vctrl_blk8_enable_flag = 0;
        reg_data.bits.vctrl_chos_oth_blk_offset_sec16 = 0;
        reg_data.bits.vctrl_chos_oth_blk_offset_sec32 = 0;
        all_reg->vedu_regs.vedu_vctrl_nm_tu8_denoise.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_53(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_vctrl_sk_acoffset_denoise reg_data;
        reg_data.bits.vctrl_skin_chos_oth_blk_spec_offset = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_offset16 = 0;
        reg_data.bits.vctrl_skin_chos_cur_blk_offset16 = 0;
        reg_data.bits.vctrl_skin_low_freq_ac_blk16 = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_offset32 = 0;
        reg_data.bits.vctrl_skin_i_chos_cur_blk_offset32 = 0;
        reg_data.bits.vctrl_skin_low_freq_ac_blk32 = 0;
        all_reg->vedu_regs.vedu_vctrl_sk_acoffset_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_sk_engthr_denoise reg_data;
        reg_data.bits.vctrl_skin_chos_oth_blk_spec_thr = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_eng_thr = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_thr3 = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_thr2 = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_thr = 0;
        reg_data.bits.vctrl_skin_rm_all_high_ac_thr = 0;
        all_reg->vedu_regs.vedu_vctrl_sk_engthr_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_sk_tu8_denoise reg_data;
        reg_data.bits.vctrl_skin_ring_eng_thr = 0;
        reg_data.bits.vctrl_skin_ring_ac_thr = 0;
        reg_data.bits.vctrl_skin_p_chos_oth_blk_offset = 0;
        reg_data.bits.vctrl_skin_p_chos_oth_blk_offset8 = 0;
        reg_data.bits.vctrl_skin_i_chos_cur_blk_offset8 = 0;
        reg_data.bits.vctrl_skin_low_freq_ac_blk8 = 0;
        reg_data.bits.vctrl_skin_blk8_enable_flag = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_offset_sec16 = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_offset_sec32 = 0;
        all_reg->vedu_regs.vedu_vctrl_sk_tu8_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_st_acoffset_denoise reg_data;
        reg_data.bits.vctrl_still_chos_oth_blk_spec_offset = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_offset16 = 0;
        reg_data.bits.vctrl_still_i_chos_cur_blk_offset16 = 0;
        reg_data.bits.vctrl_still_low_freq_ac_blk16 = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_offset32 = 0;
        reg_data.bits.vctrl_still_i_chos_cur_blk_offset32 = 0;
        reg_data.bits.vctrl_still_low_freq_ac_blk32 = 0;
        all_reg->vedu_regs.vedu_vctrl_st_acoffset_denoise.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_54(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_vctrl_st_engthr_denoise reg_data;
        reg_data.bits.vctrl_still_chos_oth_blk_spec_thr = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_eng_thr = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_thr3 = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_thr2 = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_thr = 0;
        reg_data.bits.vctrl_still_rm_all_high_ac_thr = 0;
        all_reg->vedu_regs.vedu_vctrl_st_engthr_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_st_tu8_denoise reg_data;
        reg_data.bits.vctrl_still_ring_eng_thr = 0;
        reg_data.bits.vctrl_still_ring_ac_thr = 0;
        reg_data.bits.vctrl_still_p_chos_oth_blk_offset = 0;
        reg_data.bits.vctrl_still_p_chos_oth_blk_offset8 = 0;
        reg_data.bits.vctrl_still_i_chos_cur_blk_offset8 = 0;
        reg_data.bits.vctrl_still_low_freq_ac_blk8 = 0;
        reg_data.bits.vctrl_still_blk8_enable_flag = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_offset_sec16 = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_offset_sec32 = 0;
        all_reg->vedu_regs.vedu_vctrl_st_tu8_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_se_acoffset_denoise reg_data;
        reg_data.bits.vctrl_edge_chos_oth_blk_spec_offset = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_offset16 = 0;
        reg_data.bits.vctrl_edge_i_chos_cur_blk_offset16 = 0;
        reg_data.bits.vctrl_edge_low_freq_ac_blk16 = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_offset32 = 0;
        reg_data.bits.vctrl_edge_i_chos_cur_blk_offset32 = 0;
        reg_data.bits.vctrl_edge_low_freq_ac_blk32 = 0;
        all_reg->vedu_regs.vedu_vctrl_se_acoffset_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_se_engthr_denoise reg_data;
        reg_data.bits.vctrl_edge_chos_oth_blk_spec_thr = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_eng_thr = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_thr3 = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_thr2 = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_thr = 0;
        reg_data.bits.vctrl_edge_rm_all_high_ac_thr = 0;
        all_reg->vedu_regs.vedu_vctrl_se_engthr_denoise.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_55(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_vctrl_se_tu8_denoise reg_data;
        reg_data.bits.vctrl_edge_ring_eng_thr = 0;
        reg_data.bits.vctrl_edge_ring_ac_thr = 0;
        reg_data.bits.vctrl_edge_p_chos_oth_blk_offset = 0;
        reg_data.bits.vctrl_edge_p_chos_oth_blk_offset8 = 0;
        reg_data.bits.vctrl_edge_i_chos_cur_blk_offset8 = 0;
        reg_data.bits.vctrl_edge_low_freq_ac_blk8 = 0;
        reg_data.bits.vctrl_edge_blk8_enable_flag = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_offset_sec16 = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_offset_sec32 = 0;
        all_reg->vedu_regs.vedu_vctrl_se_tu8_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_selm_acoffset_denoise reg_data;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_spec_offset = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_offset16 = 0;
        reg_data.bits.vctrl_edge_and_move_i_chos_cur_blk_offset16 = 0;
        reg_data.bits.vctrl_edge_and_move_low_freq_ac_blk16 = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_offset32 = 0;
        reg_data.bits.vctrl_edge_and_move_i_chos_cur_blk_offset32 = 0;
        reg_data.bits.vctrl_edge_and_move_low_freq_ac_blk32 = 0;
        all_reg->vedu_regs.vedu_vctrl_selm_acoffset_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_selm_engthr_denoise reg_data;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_spec_thr = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_eng_thr = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_thr3 = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_thr2 = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_thr = 0;
        reg_data.bits.vctrl_edge_and_move_rm_all_high_ac_thr = 0;
        all_reg->vedu_regs.vedu_vctrl_selm_engthr_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_selm_tu8_denoise reg_data;
        reg_data.bits.vctrl_edge_and_move_ring_eng_thr = 0;
        reg_data.bits.vctrl_edge_and_move_ring_ac_thr = 0;
        reg_data.bits.vctrl_edge_and_move_p_chos_oth_blk_offset = 0;
        reg_data.bits.vctrl_edge_and_move_p_chos_oth_blk_offset8 = 0;
        reg_data.bits.vctrl_edge_and_move_i_chos_cur_blk_offset8 = 0;
        reg_data.bits.vctrl_edge_and_move_low_freq_ac_blk8 = 0;
        reg_data.bits.vctrl_edge_and_move_blk8_enable_flag = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_offset_sec16 = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_offset_sec32 = 0;
        all_reg->vedu_regs.vedu_vctrl_selm_tu8_denoise.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_56(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_vctrl_ws_acoffset_denoise reg_data;
        reg_data.bits.vctrl_weak_chos_oth_blk_spec_offset = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_offset16 = 0;
        reg_data.bits.vctrl_weak_i_chos_cur_blk_offset16 = 0;
        reg_data.bits.vctrl_weak_low_freq_ac_blk16 = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_offset32 = 0;
        reg_data.bits.vctrl_weak_i_chos_cur_blk_offset32 = 0;
        reg_data.bits.vctrl_weak_low_freq_ac_blk32 = 0;
        all_reg->vedu_regs.vedu_vctrl_ws_acoffset_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_ws_engthr_denoise reg_data;
        reg_data.bits.vctrl_weak_chos_oth_blk_spec_thr = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_eng_thr = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_thr3 = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_thr2 = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_thr = 0;
        reg_data.bits.vctrl_weak_rm_all_high_ac_thr = 0;
        all_reg->vedu_regs.vedu_vctrl_ws_engthr_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_ws_tu8_denoise reg_data;
        reg_data.bits.vctrl_weak_ring_eng_thr = 0;
        reg_data.bits.vctrl_weak_ring_ac_thr = 0;
        reg_data.bits.vctrl_weak_p_chos_oth_blk_offset = 0;
        reg_data.bits.vctrl_weak_p_chos_oth_blk_offset8 = 0;
        reg_data.bits.vctrl_weak_i_chos_cur_blk_offset8 = 0;
        reg_data.bits.vctrl_weak_low_freq_ac_blk8 = 0;
        reg_data.bits.vctrl_weak_blk8_enable_flag = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_offset_sec16 = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_offset_sec32 = 0;
        all_reg->vedu_regs.vedu_vctrl_ws_tu8_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_ssse_acoffset_denoise reg_data;
        reg_data.bits.vctrl_strong_chos_oth_blk_spec_offset = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_offset16 = 0;
        reg_data.bits.vctrl_strong_i_chos_cur_blk_offset16 = 0;
        reg_data.bits.vctrl_strong_low_freq_ac_blk16 = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_offset32 = 0;
        reg_data.bits.vctrl_strong_i_chos_cur_blk_offset32 = 0;
        reg_data.bits.vctrl_strong_low_freq_ac_blk32 = 0;
        all_reg->vedu_regs.vedu_vctrl_ssse_acoffset_denoise.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_57(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_vctrl_ssse_engthr_denoise reg_data;
        reg_data.bits.vctrl_strong_chos_oth_blk_spec_thr = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_eng_thr = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_thr3 = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_thr2 = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_thr = 0;
        reg_data.bits.vctrl_strong_rm_all_high_ac_thr = 0;
        all_reg->vedu_regs.vedu_vctrl_ssse_engthr_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_ssse_tu8_denoise reg_data;
        reg_data.bits.vctrl_strong_ring_eng_thr = 0;
        reg_data.bits.vctrl_strong_ring_ac_thr = 0;
        reg_data.bits.vctrl_strong_p_chos_oth_blk_offset = 0;
        reg_data.bits.vctrl_strong_p_chos_oth_blk_offset8 = 0;
        reg_data.bits.vctrl_strong_i_chos_cur_blk_offset8 = 0;
        reg_data.bits.vctrl_strong_low_freq_ac_blk8 = 0;
        reg_data.bits.vctrl_strong_blk8_enable_flag = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_offset_sec16 = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_offset_sec32 = 0;
        all_reg->vedu_regs.vedu_vctrl_ssse_tu8_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_pr_inter_offset0 reg_data;
        reg_data.bits.vctrl_normal_inter_thr_offset = 0;
        reg_data.bits.vctrl_skin_inter_thr_offset = 0;
        reg_data.bits.vctrl_still_inter_thr_offset = 0;
        reg_data.bits.vctrl_s_edge_inter_thr_offset = 0;
        reg_data.bits.vctrl_e_move_inter_thr_offset = 0;
        all_reg->vedu_regs.vedu_vctrl_pr_inter_offset0.data = reg_data.data;
    }
    {
        u_vedu_vctrl_pr_inter_offset1 reg_data;
        reg_data.bits.vctrl_week_s_inter_thr_offset = 0;
        reg_data.bits.vctrl_sobel_sse_inter_thr_offset = 0;
        all_reg->vedu_regs.vedu_vctrl_pr_inter_offset1.data = reg_data.data;
    }
    all_reg->vedu_regs.vedu_vcpi_tunlcell_addr_l = reg_cfg->buf_cfg.tunl_cell_addr;
    all_reg->vedu_regs.vedu_vcpi_src_yaddr_l = reg_cfg->buf_cfg.src_y_addr;
    all_reg->vedu_regs.vedu_vcpi_src_caddr_l = reg_cfg->buf_cfg.src_c_addr;
    all_reg->vedu_regs.vedu_vcpi_src_vaddr_l = reg_cfg->buf_cfg.src_v_addr;
    all_reg->vedu_regs.vedu_vcpi_tunlcell_addr_h  = 0x0;
    all_reg->vedu_regs.vedu_vcpi_src_yaddr_h = 0x0;
    all_reg->vedu_regs.vedu_vcpi_src_caddr_h = 0x0;
    all_reg->vedu_regs.vedu_vcpi_src_vaddr_h = 0x0;
}

static hi_void direct_cfg_reg_58(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    all_reg->vedu_regs.vedu_secure_info_l = 0x08000800;
    all_reg->vedu_regs.vedu_secure_info_h = 0;

    all_reg->vedu_regs.vedu_vlcst_para_addr_l = 0x08000800;
    all_reg->vedu_regs.vedu_vlcst_para_addr_h = 0;
    {
        u_vedu_vcpi_stride reg_data;
        reg_data.bits.vcpi_curld_y_stride = reg_cfg->buf_cfg.s_stride_y;
        reg_data.bits.vcpi_curld_c_stride = reg_cfg->buf_cfg.s_stride_c;
        all_reg->vedu_regs.vedu_vcpi_stride.data = reg_data.data;
    }
    {
        u_vedu_sao_mode reg_data;
        reg_data.bits.sao_bo_mode_off_en = 0;
        reg_data.bits.sao_eo_mode_off_en = 0;
        reg_data.bits.sao_merge_mode_off = 0;
        reg_data.bits.sao_merge_mode_qp = 0;
        all_reg->vedu_regs.vedu_sao_mode.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_59(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    all_reg->vedu_regs.vedu_vcpi_rec_yh_addr_l = 0;
    all_reg->vedu_regs.vedu_vcpi_rec_ch_addr_l = 0;

    all_reg->vedu_regs.vedu_vcpi_refyh_l0_addr_l = 0;
    all_reg->vedu_regs.vedu_vcpi_refch_l0_addr_l = 0;
    all_reg->vedu_regs.vedu_vcpi_refyh_l1_addr_l = 0;
    all_reg->vedu_regs.vedu_vcpi_refch_l1_addr_l = 0;

    all_reg->vedu_regs.vedu_vcpi_rec_yh_addr_h = 0x0;
    all_reg->vedu_regs.vedu_vcpi_rec_ch_addr_h = 0x0;
    all_reg->vedu_regs.vedu_vcpi_refyh_l0_addr_h = 0x0;
    all_reg->vedu_regs.vedu_vcpi_refch_l0_addr_h = 0x0;
    all_reg->vedu_regs.vedu_vcpi_refyh_l1_addr_h = 0x0;
    all_reg->vedu_regs.vedu_vcpi_refch_l1_addr_h = 0x0;

    all_reg->vedu_regs.vedu_vcpi_refy_l0_addr0_h  = 0x0;
    all_reg->vedu_regs.vedu_vcpi_refc_l0_addr0_h  = 0x0;
    all_reg->vedu_regs.vedu_vcpi_refy_l1_addr0_h  = 0x0;
    all_reg->vedu_regs.vedu_vcpi_refc_l1_addr0_h  = 0x0;
    all_reg->vedu_regs.vedu_vcpi_refy_l0_addr1_h  = 0x0;
    all_reg->vedu_regs.vedu_vcpi_refc_l0_addr1_h  = 0x0;
    all_reg->vedu_regs.vedu_vcpi_refy_l1_addr1_h  = 0x0;
    all_reg->vedu_regs.vedu_vcpi_refc_l1_addr1_h  = 0x0;

    all_reg->vedu_regs.vedu_vcpi_rec_yaddr0_h = 0x0;
    all_reg->vedu_regs.vedu_vcpi_rec_caddr0_h = 0x0;
    all_reg->vedu_regs.vedu_vcpi_rec_yaddr1_h = 0x0;
    all_reg->vedu_regs.vedu_vcpi_rec_caddr1_h = 0x0;
}

static hi_void direct_cfg_reg_60(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    all_reg->vedu_regs.vedu_vcpi_refy_l0_addr0_l = reg_cfg->buf_cfg.vedu_vcpi_refy_l0_addr0_l;
    all_reg->vedu_regs.vedu_vcpi_refc_l0_addr0_l = reg_cfg->buf_cfg.vedu_vcpi_refc_l0_addr0_l;

    all_reg->vedu_regs.vedu_vcpi_refy_l1_addr0_l = reg_cfg->buf_cfg.vedu_vcpi_refy_l1_addr0_l;
    all_reg->vedu_regs.vedu_vcpi_refc_l1_addr0_l = reg_cfg->buf_cfg.vedu_vcpi_refc_l1_addr0_l;

    all_reg->vedu_regs.vedu_vcpi_rec_yaddr0_l = reg_cfg->buf_cfg.vedu_vcpi_rec_yaddr0_l;
    all_reg->vedu_regs.vedu_vcpi_rec_caddr0_l = reg_cfg->buf_cfg.vedu_vcpi_rec_caddr0_l;

#ifdef CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT
    all_reg->vedu_regs.vedu_vcpi_refy_l0_addr1_l = reg_cfg->buf_cfg.vedu_vcpi_refy_l0_addr1_l;
    all_reg->vedu_regs.vedu_vcpi_refc_l0_addr1_l = reg_cfg->buf_cfg.vedu_vcpi_refc_l0_addr1_l;

    all_reg->vedu_regs.vedu_vcpi_refy_l1_addr1_l = reg_cfg->buf_cfg.vedu_vcpi_refy_l1_addr1_l;
    all_reg->vedu_regs.vedu_vcpi_refc_l1_addr1_l = reg_cfg->buf_cfg.vedu_vcpi_refc_l1_addr1_l;

    all_reg->vedu_regs.vedu_vcpi_rec_yaddr1_l = reg_cfg->buf_cfg.vedu_vcpi_rec_yaddr1_l;
    all_reg->vedu_regs.vedu_vcpi_rec_caddr1_l = reg_cfg->buf_cfg.vedu_vcpi_rec_caddr1_l;
#endif


    {
        u_vedu_vcpi_ref_l0_stride reg_data;
        reg_data.bits.vcpi_refy_l0_stride = reg_cfg->buf_cfg.vcpi_refy_l0_stride;
        reg_data.bits.vcpi_refc_l0_stride = reg_cfg->buf_cfg.vcpi_refc_l0_stride;
        all_reg->vedu_regs.vedu_vcpi_ref_l0_stride.data = reg_data.data;
    }
    {
        u_vedu_vcpi_ref_l1_stride  reg_data;
        reg_data.bits.vcpi_refy_l1_stride = reg_cfg->buf_cfg.vcpi_refy_l1_stride;
        reg_data.bits.vcpi_refc_l1_stride = reg_cfg->buf_cfg.vcpi_refc_l1_stride;
        all_reg->vedu_regs.vedu_vcpi_ref_l1_stride.data = reg_data.data;
    }
    {
        u_vedu_vcpi_rec_stride  reg_data;
        reg_data.bits.vcpi_recst_ystride = reg_cfg->buf_cfg.vcpi_recst_ystride;
        reg_data.bits.vcpi_recst_cstride = reg_cfg->buf_cfg.vcpi_recst_cstride;
        all_reg->vedu_regs.vedu_vcpi_rec_stride.data = reg_data.data;
    }
    all_reg->vedu_regs.vedu_vcpi_pmest_stride = reg_cfg->buf_cfg.pme_stride;
    all_reg->vedu_regs.vedu_vcpi_pmeld_stride = reg_cfg->buf_cfg.pme_stride;
}

static hi_void direct_cfg_reg_61(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    if (reg_cfg->ctrl_cfg.enable_rcn_ref_share_buf == HI_TRUE) {
        {
            u_vedu_vcpi_rec_length reg_data;
            reg_data.bits.vcpi_rec_luma_length = reg_cfg->buf_cfg.vcpi_rec_luma_length;
            reg_data.bits.vcpi_rec_chroma_length = reg_cfg->buf_cfg.vcpi_rec_chroma_length;
            all_reg->vedu_regs.vedu_vcpi_rec_length.data = reg_data.data;
        }
        {
            u_vedu_vcpi_ref_l0_length reg_data;
            reg_data.bits.vcpi_ref0_luma_length  = reg_cfg->buf_cfg.vcpi_ref0_luma_length;
            reg_data.bits.vcpi_ref0_chroma_length = reg_cfg->buf_cfg.vcpi_ref0_chroma_length;
            all_reg->vedu_regs.vedu_vcpi_ref_l0_length.data = reg_data.data;
        }
        {
            u_vedu_vcpi_ref_l1_length reg_data;
            reg_data.bits.vcpi_ref1_luma_length = reg_cfg->buf_cfg.vcpi_ref1_luma_length;
            reg_data.bits.vcpi_ref1_chroma_length = reg_cfg->buf_cfg.vcpi_ref1_chroma_length;
            all_reg->vedu_regs.vedu_vcpi_ref_l1_length.data = reg_data.data;
        }
    } else {
        {
            u_vedu_vcpi_rec_length reg_data;
            reg_data.bits.vcpi_rec_luma_length = reg_cfg->ctrl_cfg.enc_height;
            reg_data.bits.vcpi_rec_chroma_length = reg_cfg->ctrl_cfg.enc_height;
            all_reg->vedu_regs.vedu_vcpi_rec_length.data = reg_data.data;
        }
        {
            u_vedu_vcpi_ref_l0_length reg_data;
            reg_data.bits.vcpi_ref0_luma_length = reg_cfg->ctrl_cfg.enc_height;
            reg_data.bits.vcpi_ref0_chroma_length = reg_cfg->ctrl_cfg.enc_height;
            all_reg->vedu_regs.vedu_vcpi_ref_l0_length.data = reg_data.data;
        }
        {
            u_vedu_vcpi_ref_l1_length reg_data;
            reg_data.bits.vcpi_ref1_luma_length = reg_cfg->ctrl_cfg.enc_height;
            reg_data.bits.vcpi_ref1_chroma_length = reg_cfg->ctrl_cfg.enc_height;
            all_reg->vedu_regs.vedu_vcpi_ref_l1_length.data = reg_data.data;
        }
    }
}

static hi_void direct_cfg_reg_62(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    all_reg->vedu_regs.vedu_vcpi_pmeld_l0_addr_l = reg_cfg->buf_cfg.vedu_vcpi_pmeld_l0_addr_l;
    all_reg->vedu_regs.vedu_vcpi_pmeld_l0_addr_h = 0x0;
    all_reg->vedu_regs.vedu_vcpi_pmeld_l1_addr_l = reg_cfg->buf_cfg.vedu_vcpi_pmeld_l1_addr_l;
    all_reg->vedu_regs.vedu_vcpi_pmeld_l1_addr_h = 0x0;

    all_reg->vedu_regs.vedu_vcpi_pmest_addr_l = reg_cfg->buf_cfg.vedu_vcpi_pmest_addr_l;
    all_reg->vedu_regs.vedu_vcpi_pmest_addr_h = 0x0 ;
    all_reg->vedu_regs.vedu_vcpi_swptraddr_l = reg_cfg->buf_cfg.strm_buf_wp_addr;
    all_reg->vedu_regs.vedu_vcpi_srptraddr_l = reg_cfg->buf_cfg.strm_buf_rp_addr;
    all_reg->vedu_regs.vedu_vcpi_swptraddr_h = 0x0;
    all_reg->vedu_regs.vedu_vcpi_srptraddr_h = 0x0;

    all_reg->vedu_regs.vedu_vcpi_pmeinfo_st_addr_l = reg_cfg->buf_cfg.vedu_vcpi_pmeinfo_st_addr_l;
    all_reg->vedu_regs.vedu_vcpi_pmeinfo_ld0_addr_l = reg_cfg->buf_cfg.vedu_vcpi_pmeinfo_ld0_addr_l;
    all_reg->vedu_regs.vedu_vcpi_pmeinfo_st_addr_h = 0x0;
    all_reg->vedu_regs.vedu_vcpi_pmeinfo_ld0_addr_h = 0x0;


    all_reg->vedu_regs.vedu_vcpi_qpgld_inf_addr_l = 0;
    all_reg->vedu_regs.vedu_skipweight_ld_addr_l = 0;
    all_reg->vedu_regs.vedu_vcpi_nbi_mvst_addr_l = reg_cfg->buf_cfg.vedu_vcpi_nbi_mvst_addr_l ;
    all_reg->vedu_regs.vedu_vcpi_nbi_mvld_addr_l = reg_cfg->buf_cfg.vedu_vcpi_nbi_mvld_addr_l;
    all_reg->vedu_regs.vedu_vcpi_qpgld_inf_addr_h = 0x0;
    all_reg->vedu_regs.vedu_skipweight_ld_addr_h = 0x0;
    all_reg->vedu_regs.vedu_vcpi_nbi_mvst_addr_h = 0x0;
    all_reg->vedu_regs.vedu_vcpi_nbi_mvld_addr_h = 0x0;
}

static hi_void direct_cfg_reg_63(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    all_reg->vedu_regs.vedu_emar_wch_press_en.data = 0x0;
    all_reg->vedu_regs.vedu_emar_rch_press_en.data = 0x0;
    {
        u_vedu_emar_awpress_num_00 reg_data;
        reg_data.bits.vcpi_awpress_ch00_num = 0;
        reg_data.bits.vcpi_awpress_ch01_num = 0;
        all_reg->vedu_regs.vedu_emar_awpress_num_00.data = reg_data.data;
    }
    {
        u_vedu_emar_awpress_num_01 reg_data;
        reg_data.bits.vcpi_awpress_ch02_num = 0;
        reg_data.bits.vcpi_awpress_ch03_num = 0;
        all_reg->vedu_regs.vedu_emar_awpress_num_01.data = reg_data.data;
    }
    {
        u_vedu_emar_awpress_num_02 reg_data;
        reg_data.bits.vcpi_awpress_ch04_num = 0;
        reg_data.bits.vcpi_awpress_ch05_num = 0;
        all_reg->vedu_regs.vedu_emar_awpress_num_02.data = reg_data.data;
    }
    {
        u_vedu_emar_awpress_num_03 reg_data;
        reg_data.bits.vcpi_awpress_ch06_num = 0;
        reg_data.bits.vcpi_awpress_ch07_num = 0;
        all_reg->vedu_regs.vedu_emar_awpress_num_03.data = reg_data.data;
    }
    {
        u_vedu_emar_awpress_num_04 reg_data;
        reg_data.bits.vcpi_awpress_ch08_num = 0;
        reg_data.bits.vcpi_awpress_ch09_num = 0;
        all_reg->vedu_regs.vedu_emar_awpress_num_04.data = reg_data.data;
    }
    {
        u_vedu_emar_awpress_num_05 reg_data;
        reg_data.bits.vcpi_awpress_ch10_num = 0;
        reg_data.bits.vcpi_awpress_ch11_num = 0;
        all_reg->vedu_regs.vedu_emar_awpress_num_05.data = reg_data.data;
    }
    {
        u_vedu_emar_awpress_num_06 reg_data;
        reg_data.bits.vcpi_awpress_ch12_num = 0;
        reg_data.bits.vcpi_awpress_ch13_num = 0;
        all_reg->vedu_regs.vedu_emar_awpress_num_06.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_64(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_emar_awpress_num_07 reg_data;
        reg_data.bits.vcpi_awpress_ch14_num = 0;
        all_reg->vedu_regs.vedu_emar_awpress_num_07.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_00 reg_data;
        reg_data.bits.vcpi_arpress_ch00_num = 0;
        reg_data.bits.vcpi_arpress_ch01_num = 0;
        all_reg->vedu_regs.vedu_emar_arpress_num_00.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_01 reg_data;
        reg_data.bits.vcpi_arpress_ch02_num = 0;
        reg_data.bits.vcpi_arpress_ch03_num = 0;
        all_reg->vedu_regs.vedu_emar_arpress_num_01.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_02 reg_data;
        reg_data.bits.vcpi_arpress_ch04_num = 0;
        reg_data.bits.vcpi_arpress_ch05_num = 0;
        all_reg->vedu_regs.vedu_emar_arpress_num_02.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_03 reg_data;
        reg_data.bits.vcpi_arpress_ch06_num = 0;
        reg_data.bits.vcpi_arpress_ch07_num = 0;
        all_reg->vedu_regs.vedu_emar_arpress_num_03.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_04 reg_data;
        reg_data.bits.vcpi_arpress_ch08_num = 0;
        reg_data.bits.vcpi_arpress_ch09_num = 0;
        all_reg->vedu_regs.vedu_emar_arpress_num_04.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_05 reg_data;
        reg_data.bits.vcpi_arpress_ch10_num = 0;
        reg_data.bits.vcpi_arpress_ch11_num = 0;
        all_reg->vedu_regs.vedu_emar_arpress_num_05.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_06 reg_data;
        reg_data.bits.vcpi_arpress_ch12_num = 0;
        reg_data.bits.vcpi_arpress_ch13_num = 0;
        all_reg->vedu_regs.vedu_emar_arpress_num_06.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_65(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    {
        u_vedu_emar_arpress_num_07 reg_data;
        reg_data.bits.vcpi_arpress_ch14_num = 0;
        reg_data.bits.vcpi_arpress_ch15_num = 0;
        all_reg->vedu_regs.vedu_emar_arpress_num_07.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_08 reg_data;
        reg_data.bits.vcpi_arpress_ch16_num = 0;
        reg_data.bits.vcpi_arpress_ch17_num = 0;
        all_reg->vedu_regs.vedu_emar_arpress_num_08.data = reg_data.data;
    }
    {
        u_vedu_emar_wpress_num_00 reg_data;
        reg_data.bits.vcpi_wpress_ch00_num = 0;
        reg_data.bits.vcpi_wpress_ch01_num = 0;
        all_reg->vedu_regs.vedu_emar_wpress_num_00.data = reg_data.data;
    }
    {
        u_vedu_emar_wpress_num_01 reg_data;
        reg_data.bits.vcpi_wpress_ch02_num = 0;
        reg_data.bits.vcpi_wpress_ch03_num = 0;
        all_reg->vedu_regs.vedu_emar_wpress_num_01.data = reg_data.data;
    }
    {
        u_vedu_emar_wpress_num_02 reg_data;
        reg_data.bits.vcpi_wpress_ch04_num = 0 ;
        reg_data.bits.vcpi_wpress_ch05_num = 0;
        all_reg->vedu_regs.vedu_emar_wpress_num_02.data = reg_data.data;
    }
    {
        u_vedu_emar_wpress_num_03 reg_data;
        reg_data.bits.vcpi_wpress_ch06_num = 0;
        reg_data.bits.vcpi_wpress_ch07_num = 0;
        all_reg->vedu_regs.vedu_emar_wpress_num_03.data = reg_data.data;
    }
    {
        u_vedu_emar_wpress_num_04 reg_data;
        reg_data.bits.vcpi_wpress_ch08_num = 0;
        reg_data.bits.vcpi_wpress_ch09_num = 0;
        all_reg->vedu_regs.vedu_emar_wpress_num_04.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_66(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_emar_wpress_num_05 reg_data;
        reg_data.bits.vcpi_wpress_ch10_num = 0;
        reg_data.bits.vcpi_wpress_ch11_num = 0;
        all_reg->vedu_regs.vedu_emar_wpress_num_05.data = reg_data.data;
    }
    {
        u_vedu_emar_wpress_num_06 reg_data;
        reg_data.bits.vcpi_wpress_ch12_num = 0;
        reg_data.bits.vcpi_wpress_ch13_num = 0;
        all_reg->vedu_regs.vedu_emar_wpress_num_06.data = reg_data.data;
    }
    {
        u_vedu_emar_wpress_num_07 reg_data;
        reg_data.bits.vcpi_wpress_ch14_num = 0;
        all_reg->vedu_regs.vedu_emar_wpress_num_07.data = reg_data.data;
    }
    {
        u_vedu_vcpi_tmv_load reg_data;
        if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
            if (static_cfg->base_cfg.tmv_en == 1) {
                reg_data.bits.vcpi_tmv_wr_rd_avail = 0x3;
            } else {
                reg_data.bits.vcpi_tmv_wr_rd_avail = static_cfg->base_cfg.vcpi_tmv_wr_rd_avail;
            }
        } else if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {
            reg_data.bits.vcpi_tmv_wr_rd_avail = 0x3;
        }
        all_reg->vedu_regs.vedu_vcpi_tmv_load.data = reg_data.data;
    }
    {
        u_vedu_vcpi_iblk_pre_mv_thr reg_data;
        reg_data.bits.vcpi_iblk_pre_mvy_thr = static_cfg->base_cfg.vcpi_iblk_pre_mvy_thr;
        reg_data.bits.vcpi_iblk_pre_mvx_thr = static_cfg->base_cfg.vcpi_iblk_pre_mvx_thr;
        reg_data.bits.vcpi_iblk_pre_mv_dif_thr1 = static_cfg->base_cfg.vcpi_iblk_pre_mv_dif_thr1;
        reg_data.bits.vcpi_iblk_pre_mv_dif_thr0 = static_cfg->base_cfg.vcpi_iblk_pre_mv_dif_thr0;
        all_reg->vedu_regs.vedu_vcpi_iblk_pre_mv_thr.data = reg_data.data;
    }
    {
        u_vedu_vcpi_i_slc_insert reg_data;
        reg_data.bits.vcpi_insert_i_slc_en  = static_cfg->base_cfg.vcpi_insert_i_slc_en;
        reg_data.bits.vcpi_insert_i_slc_idx = static_cfg->base_cfg.vcpi_insert_i_slc_idx;
        all_reg->vedu_regs.vedu_vcpi_i_slc_insert.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_67(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vcpi_iblk_pre_cost_thr reg_data;
        reg_data.bits.pme_iblk_pre_cost_thr_h264 = static_cfg->pme_cfg_static.pme_iblk_pre_cost_thr_h264;
        reg_data.bits.pme_intrablk_det_cost_thr1 = static_cfg->pme_cfg_static.pme_intrablk_det_cost_thr1;
        all_reg->vedu_regs.vedu_pme_iblk_cost_thr.data = reg_data.data;
    }
        all_reg->vedu_regs.vedu_qpg_cu_min_sad_thresh_0 = static_cfg->qpg_cfg_static.min_sad_thresh_low;
    {
        u_vedu_sel_offset_strength reg_data;
        reg_data.bits.sel_offset_strength = 1; /* static_cfg->img_improve_cfg_static.sel_offset_strength */
        all_reg->vedu_regs.vedu_sel_offset_strength.data = reg_data.data;
    }
    {
        u_vedu_sel_cu32_dc_ac_th_offset reg_data;
        reg_data.bits.sel_cu32_dc_ac_th_offset = 1;
        all_reg->vedu_regs.vedu_sel_cu32_dc_ac_th_offset.data = reg_data.data;
    }
    {
        u_vedu_sel_cu32_qp_th reg_data;
        reg_data.bits.sel_cu32_qp0_th = 0x26;
        reg_data.bits.sel_cu32_qp1_th = 0x21;
        all_reg->vedu_regs.vedu_sel_cu32_qp_th.data = reg_data.data;
    }
    {
        u_vedu_sel_res_dc_ac_th reg_data;
        reg_data.bits.sel_res16_luma_dc_th = 0x3;
        reg_data.bits.sel_res16_chroma_dc_th = 0x2;
        reg_data.bits.sel_res16_luma_ac_th = 0x3;
        reg_data.bits.sel_res16_chroma_ac_th = 0x2;
        reg_data.bits.sel_res32_luma_dc_th = 0x3;
        reg_data.bits.sel_res32_chroma_dc_th = 0x4;
        reg_data.bits.sel_res32_luma_ac_th = 0x3;
        reg_data.bits.sel_res32_chroma_ac_th = 0x4;
        all_reg->vedu_regs.vedu_sel_res_dc_ac_th.data = reg_data.data;
    }
}

static hi_void direct_cfg_reg_68(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    hi_s32 i;
    /* the stream of current frame wether includes para sets, I:yes, P: no */
    if (reg_cfg->buf_cfg.stream_flag == STREAM_LIST) {
        all_reg->vedu_regs.vedu_vlcst_parameter.bits.vlcst_store_type = 1;
        if (reg_cfg->rc_cfg.intra_pic) {
            all_reg->vedu_regs.vedu_vlcst_parameter.bits.vlcst_para_set_en = 1;
        } else {
            all_reg->vedu_regs.vedu_vlcst_parameter.bits.vlcst_para_set_en = 0;
        }
        /* para sets wether has indepandent base addr, now must be no, because para set only can use mmz buffer */
        all_reg->vedu_regs.vedu_vlcst_parameter.bits.vlcst_para_sprat_en = 1;
        if (all_reg->vedu_regs.vedu_vlcst_parameter.bits.vlcst_para_set_en == 1) {
            /* Length of the parameter set, which is aligned with 64 bytes.
             * The value is greater than 0 and less than or equal to 192 bytes.
             */
            all_reg->vedu_regs.vedu_vlcst_parameter.bits.vlcst_para_set_len = reg_cfg->buf_cfg.para_set_buf_size;
            for (i = 0; i < 48; i++) { /* 48: array size */
                all_reg->vedu_regs.vedu_vlcst_para_data[i] = reg_cfg->buf_cfg.para_set_reg[i];
            }
        }
    }
}

static hi_void direct_cfg_reg_69(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    all_reg->vedu_regs.vedu_vlcst_strmaddr0_l = reg_cfg->buf_cfg.strm_buf_addr[0];
    all_reg->vedu_regs.vedu_vlcst_strmaddr1_l = reg_cfg->buf_cfg.strm_buf_addr[1];
    all_reg->vedu_regs.vedu_vlcst_strmaddr2_l = reg_cfg->buf_cfg.strm_buf_addr[2]; /* 2: array index */
    all_reg->vedu_regs.vedu_vlcst_strmaddr3_l = reg_cfg->buf_cfg.strm_buf_addr[3]; /* 3: array index */
    all_reg->vedu_regs.vedu_vlcst_strmaddr4_l = reg_cfg->buf_cfg.strm_buf_addr[4]; /* 4: array index */
    all_reg->vedu_regs.vedu_vlcst_strmaddr5_l = reg_cfg->buf_cfg.strm_buf_addr[5]; /* 5: array index */
    all_reg->vedu_regs.vedu_vlcst_strmaddr6_l = reg_cfg->buf_cfg.strm_buf_addr[6]; /* 6: array index */
    all_reg->vedu_regs.vedu_vlcst_strmaddr7_l = reg_cfg->buf_cfg.strm_buf_addr[7]; /* 7: array index */
    all_reg->vedu_regs.vedu_vlcst_strmaddr8_l = reg_cfg->buf_cfg.strm_buf_addr[8]; /* 8: array index */
    all_reg->vedu_regs.vedu_vlcst_strmaddr9_l = reg_cfg->buf_cfg.strm_buf_addr[9]; /* 9: array index */
    all_reg->vedu_regs.vedu_vlcst_strmaddr10_l = reg_cfg->buf_cfg.strm_buf_addr[10]; /* 10: array index */
    all_reg->vedu_regs.vedu_vlcst_strmaddr11_l = reg_cfg->buf_cfg.strm_buf_addr[11]; /* 11: array index */
    all_reg->vedu_regs.vedu_vlcst_strmaddr12_l = reg_cfg->buf_cfg.strm_buf_addr[12]; /* 12: array index */
    all_reg->vedu_regs.vedu_vlcst_strmaddr13_l = reg_cfg->buf_cfg.strm_buf_addr[13]; /* 13: array index */
    all_reg->vedu_regs.vedu_vlcst_strmaddr14_l = reg_cfg->buf_cfg.strm_buf_addr[14]; /* 14: array index */
    all_reg->vedu_regs.vedu_vlcst_strmaddr15_l = reg_cfg->buf_cfg.strm_buf_addr[15]; /* 15: array index */
    all_reg->vedu_regs.vedu_vlcst_strmaddr16_l = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr17_l = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr18_l = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr19_l = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr20_l = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr21_l = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr22_l = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr23_l = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr24_l = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr25_l = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr26_l = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr27_l = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr28_l = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr29_l = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr30_l = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr31_l = 0x0;
}

static hi_void direct_cfg_reg_70(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    all_reg->vedu_regs.vedu_vlcst_strmaddr0_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr1_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr2_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr3_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr4_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr5_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr6_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr7_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr8_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr9_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr10_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr11_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr12_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr13_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr14_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr15_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr16_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr17_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr18_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr19_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr20_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr21_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr22_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr23_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr24_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr25_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr26_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr27_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr28_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr29_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr30_h = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmaddr31_h = 0x0;
}

static hi_void direct_cfg_reg_71(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg)
{
    all_reg->vedu_regs.vedu_vlcst_strmbuflen0 = reg_cfg->buf_cfg.strm_buf_size[0];
    all_reg->vedu_regs.vedu_vlcst_strmbuflen1 = reg_cfg->buf_cfg.strm_buf_size[1];
    all_reg->vedu_regs.vedu_vlcst_strmbuflen2 = reg_cfg->buf_cfg.strm_buf_size[2]; /* 2: array index */
    all_reg->vedu_regs.vedu_vlcst_strmbuflen3 = reg_cfg->buf_cfg.strm_buf_size[3]; /* 3: array index */
    all_reg->vedu_regs.vedu_vlcst_strmbuflen4 = reg_cfg->buf_cfg.strm_buf_size[4]; /* 4: array index */
    all_reg->vedu_regs.vedu_vlcst_strmbuflen5 = reg_cfg->buf_cfg.strm_buf_size[5]; /* 5: array index */
    all_reg->vedu_regs.vedu_vlcst_strmbuflen6 = reg_cfg->buf_cfg.strm_buf_size[6]; /* 6: array index */
    all_reg->vedu_regs.vedu_vlcst_strmbuflen7 = reg_cfg->buf_cfg.strm_buf_size[7]; /* 7: array index */
    all_reg->vedu_regs.vedu_vlcst_strmbuflen8 = reg_cfg->buf_cfg.strm_buf_size[8]; /* 8: array index */
    all_reg->vedu_regs.vedu_vlcst_strmbuflen9 = reg_cfg->buf_cfg.strm_buf_size[9]; /* 9: array index */
    all_reg->vedu_regs.vedu_vlcst_strmbuflen10 = reg_cfg->buf_cfg.strm_buf_size[10]; /* 10: array index */
    all_reg->vedu_regs.vedu_vlcst_strmbuflen11 = reg_cfg->buf_cfg.strm_buf_size[11]; /* 11: array index */
    all_reg->vedu_regs.vedu_vlcst_strmbuflen12 = reg_cfg->buf_cfg.strm_buf_size[12]; /* 12: array index */
    all_reg->vedu_regs.vedu_vlcst_strmbuflen13 = reg_cfg->buf_cfg.strm_buf_size[13]; /* 13: array index */
    all_reg->vedu_regs.vedu_vlcst_strmbuflen14 = reg_cfg->buf_cfg.strm_buf_size[14]; /* 14: array index */
    all_reg->vedu_regs.vedu_vlcst_strmbuflen15 = reg_cfg->buf_cfg.strm_buf_size[15]; /* 15: array index */
    all_reg->vedu_regs.vedu_vlcst_strmbuflen16 = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmbuflen17 = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmbuflen18 = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmbuflen19 = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmbuflen20 = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmbuflen21 = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmbuflen22 = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmbuflen23 = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmbuflen24 = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmbuflen25 = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmbuflen26 = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmbuflen27 = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmbuflen28 = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmbuflen29 = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmbuflen30 = 0x0;
    all_reg->vedu_regs.vedu_vlcst_strmbuflen31 = 0x0;

    all_reg->vedu_regs.vedu_vlcst_para_addr_l  = reg_cfg->buf_cfg.para_set_addr;
    all_reg->vedu_regs.vedu_vlcst_para_addr_h  = 0;
}

static hi_void direct_cfg_reg_upper_half(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg,
                                         venc_hal_dynamic_cfg *dynamic_cfg, venc_hal_static_cfg *static_cfg)
{
    direct_cfg_reg_01(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_02(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_03(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_04(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_05(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_06(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_07(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_08(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_09(reg_cfg, all_reg);
    direct_cfg_reg_10(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_11(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_12(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_13(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_14(reg_cfg, all_reg);
    direct_cfg_reg_15(reg_cfg, all_reg);
    direct_cfg_reg_16(reg_cfg, all_reg);
    direct_cfg_reg_17(reg_cfg, all_reg);
    direct_cfg_reg_18(reg_cfg, all_reg);
    direct_cfg_reg_19(reg_cfg, all_reg);
    direct_cfg_reg_20(reg_cfg, all_reg);
    direct_cfg_reg_21(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_22(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_23(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_24(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_25(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_26(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_27(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_28(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_29(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_30(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_31(reg_cfg, all_reg, static_cfg);
#ifdef VENC_SUPPORT_ROI
    direct_cfg_reg_32(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_33(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_34(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_35(reg_cfg, all_reg, static_cfg);
#else
    direct_cfg_reg_36(all_reg);
#endif
}

static hi_void direct_cfg_reg_lower_half(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg,
                                         venc_hal_dynamic_cfg *dynamic_cfg, venc_hal_static_cfg *static_cfg)
{
    direct_cfg_reg_37(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_38(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_39(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_40(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_41(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_42(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_43(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_44(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_45(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_46(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_47(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_48(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_49(reg_cfg, all_reg, dynamic_cfg);
    direct_cfg_reg_50(reg_cfg, all_reg);
    direct_cfg_reg_51(reg_cfg, all_reg);
    direct_cfg_reg_52(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_53(reg_cfg, all_reg);
    direct_cfg_reg_54(reg_cfg, all_reg);
    direct_cfg_reg_55(reg_cfg, all_reg);
    direct_cfg_reg_56(reg_cfg, all_reg);
    direct_cfg_reg_57(reg_cfg, all_reg);
    direct_cfg_reg_58(reg_cfg, all_reg);
    direct_cfg_reg_59(reg_cfg, all_reg);
    direct_cfg_reg_60(reg_cfg, all_reg);
    direct_cfg_reg_61(reg_cfg, all_reg);
    direct_cfg_reg_62(reg_cfg, all_reg);
    direct_cfg_reg_63(reg_cfg, all_reg);
    direct_cfg_reg_64(reg_cfg, all_reg);
    direct_cfg_reg_65(reg_cfg, all_reg);
    direct_cfg_reg_66(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_67(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_68(reg_cfg, all_reg);
    direct_cfg_reg_69(reg_cfg, all_reg);
    direct_cfg_reg_70(reg_cfg, all_reg);
    direct_cfg_reg_71(reg_cfg, all_reg);
    direct_cfg_reg_72(reg_cfg, all_reg, static_cfg);
}

static hi_void drv_venc_hal_direct_cfg_reg(venc_hal_cfg *reg_cfg)
{
    vedu_regs_type  *all_reg = (vedu_regs_type*)g_reg_base;
    venc_hal_dynamic_cfg *dynamic_cfg = &g_hal_reg_info->reg_dynamic_cfg;
    venc_hal_static_cfg *static_cfg = &g_hal_reg_info->reg_static_cfg;

#if PARA_ADAPT_ENCODER
    drv_venc_hal_set_default_attr_by_start_qp(dynamic_cfg, reg_cfg->rc_cfg.start_qp, reg_cfg->rc_cfg.intra_pic);
#endif
    drv_venc_hal_set_default_attr_by_protocol(dynamic_cfg, reg_cfg->ctrl_cfg.protocol);
    drv_venc_hal_set_tunl_read_intvl(reg_cfg, &dynamic_cfg->tunl_read_intvl);
    drv_venc_hal_set_mode_by_profile(reg_cfg->ctrl_cfg.profile, reg_cfg->ctrl_cfg.protocol, dynamic_cfg);
    reg_constraint_for176x144(&reg_cfg->ctrl_cfg, &reg_cfg->rc_cfg, &dynamic_cfg->qpg_cfg_dynamic.qp_delta, static_cfg);

    direct_cfg_reg_upper_half(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_lower_half(reg_cfg, all_reg, dynamic_cfg, static_cfg);
}
#else

static hi_void ddr_cfg_reg_11(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vcpi_ref_flag reg_data;
        if (reg_cfg->rc_cfg.pic_type != PIC_INTRA) {
            reg_data.bits.vcpi_curr_ref_long_flag = reg_cfg->buf_cfg.vcpi_curr_ref_long_flag;
            reg_data.bits.vcpi_col_from_l0_flag = 1;
        }

        if (reg_cfg->rc_cfg.pic_type == PIC_BIINTER) {
            reg_data.bits.vcpi_predflag_sel = 0x2;
        } else {
            reg_data.bits.vcpi_predflag_sel = 0;
        }
        all_ddr->vedu_vcpi_ref_flag.data = reg_data.data;
    }
    {
        u_vedu_pmv_tmv_en reg_data;
        if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
            reg_data.bits.pmv_tmv_en = static_cfg->base_cfg.tmv_en;
        } else if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {
            reg_data.bits.pmv_tmv_en = 1;
        }
        all_ddr->vedu_pmv_tmv_en.data = reg_data.data;
    }
    all_ddr->vedu_pmv_poc_0 = reg_cfg->buf_cfg.pmv_poc[0];
    all_ddr->vedu_pmv_poc_1 = reg_cfg->buf_cfg.pmv_poc[1];
    all_ddr->vedu_pmv_poc_2 = reg_cfg->buf_cfg.pmv_poc[2]; /* 2: ;pmv poc */
    all_ddr->vedu_pmv_poc_3 = reg_cfg->buf_cfg.pmv_poc[3]; /* 3: ;pmv poc */
    all_ddr->vedu_pmv_poc_4 = reg_cfg->buf_cfg.pmv_poc[4]; /* 4: ;pmv poc */
    all_ddr->vedu_pmv_poc_5 = reg_cfg->buf_cfg.pmv_poc[5]; /* 5: ;pmv poc */
    {
        u_vedu_cabac_glb_cfg reg_data;
        reg_data.data = 0;
        reg_data.bits.cabac_max_num_mergecand = static_cfg->base_cfg.max_num_mergecand;
        if (reg_cfg->rc_cfg.intra_pic) {
            reg_data.bits.cabac_nal_unit_head = 0x2601;
        } else {
            reg_data.bits.cabac_nal_unit_head = 0x0201;
        }
        all_ddr->vedu_cabac_glb_cfg.data = reg_data.data;
    }
    all_ddr->vedu_ice_cmc_mode_cfg0.data = 0x0005FB14;

    all_ddr->vedu_ice_cmc_mode_cfg1.data = 0x0005FB14;
}

static hi_void ddr_cfg_reg_12(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_dynamic_cfg *dynamic_cfg,
                              venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vcpi_cross_tile_slc reg_data;
        reg_data.bits.vcpi_cross_slice = static_cfg->base_cfg.vcpi_cross_slice;
        reg_data.bits.vcpi_cross_tile = 0;
        all_ddr->vedu_vcpi_cross_tile_slc.data = reg_data.data;
    }
    {
        u_vedu_vcpi_dblkcfg  reg_data;
        reg_data.bits.vcpi_dblk_filter_flag = static_cfg->base_cfg.vcpi_dblk_filter_flag;
        reg_data.bits.vcpi_dblk_alpha = static_cfg->base_cfg.vcpi_dblk_alpha;
        reg_data.bits.vcpi_dblk_beta = static_cfg->base_cfg.vcpi_dblk_beta;
        all_ddr->vedu_vcpi_dblkcfg.data = reg_data.data;
    }
    {
        u_vedu_vcpi_refld_dw reg_data;
        reg_data.bits.vcpi_refld_hdw = 0x10;
        reg_data.bits.vcpi_refld_vdw = 0x10;
        all_ddr->vedu_vcpi_refld_dw.data = reg_data.data;
    }
    {
        u_vedu_intra_cfg reg_data;
        reg_data.bits.constrained_intra_pred_flag = 0;
        reg_data.bits.intra_smooth = 1;
        all_ddr->vedu_intra_cfg.data = reg_data.data;
    }
    {
        u_vedu_curld_gcfg reg_data;
        reg_data.bits.curld_osd0_global_en = 0;
        reg_data.bits.curld_osd1_global_en = 0;
        reg_data.bits.curld_osd2_global_en = 0;
        reg_data.bits.curld_osd3_global_en = 0;
        reg_data.bits.curld_osd4_global_en = 0;
        reg_data.bits.curld_osd5_global_en = 0;
        reg_data.bits.curld_osd6_global_en = 0;
        reg_data.bits.curld_osd7_global_en = 0;

        reg_data.bits.curld_col2gray_en = static_cfg->base_cfg.curld_col2gray_en;
        reg_data.bits.curld_clip_en = static_cfg->base_cfg.curld_clip_en;
        reg_data.bits.curld_lowdly_en = reg_cfg->ctrl_cfg.low_dly_mode;
        reg_data.bits.curld_read_interval = dynamic_cfg->tunl_read_intvl;
        reg_data.bits.curld_osd_rgbfmt =  0;

        all_ddr->vedu_curld_gcfg.data = reg_data.data;
    }
    cfg_str_fmt(reg_cfg, &static_cfg->base_cfg);
}

static hi_void ddr_cfg_reg_13(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_lcu_target_bit reg_data;
        hi_u32 tmp;

        if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {
            tmp = 0x10;
        } else {
            tmp = 0x20;
        }
        tmp = ((reg_cfg->ctrl_cfg.enc_width + tmp - 1) / tmp) * ((reg_cfg->ctrl_cfg.enc_height + tmp - 1) / tmp);
        reg_data.bits.vctrl_lcu_target_bit = reg_cfg->rc_cfg.target_bits / tmp;
        all_ddr->vedu_vctrl_lcu_target_bit.data = reg_data.data;
    }
    {
        u_vedu_pme_safe_cfg reg_data;
        reg_data.bits.pme_safe_line = static_cfg->pme_cfg_static.pme_safe_line;
        reg_data.bits.pme_safe_line_val = static_cfg->pme_cfg_static.pme_safe_line_val;
        reg_data.bits.pme_safe_line_mode = static_cfg->pme_cfg_static.pme_safe_line_mode;
        all_ddr->vedu_pme_safe_cfg.data = reg_data.data;
    }
    {
        u_vedu_pme_iblk_refresh reg_data;
        reg_data.bits.pme_iblk_refresh_en = static_cfg->pme_cfg_static.pme_iblk_refresh_en;
        reg_data.bits.pme_iblk_refresh_mode = static_cfg->pme_cfg_static.pme_iblk_refresh_mode;
        all_ddr->vedu_pme_iblk_refresh.data = reg_data.data;
    }
    {
        u_vedu_pme_iblk_refresh_para reg_data;
        reg_data.bits.pme_iblk_refresh_start = static_cfg->pme_cfg_static.pme_iblk_refresh_start_num;
        reg_data.bits.pme_iblk_refresh_size = static_cfg->pme_cfg_static.pme_iblk_refresh_num;
        all_ddr->vedu_pme_iblk_refresh_para.data = reg_data.data;
    }

    if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
        all_ddr->vedu_intra_chnl4_ang_0_en.data = 0xffffffff;
    } else if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {
        all_ddr->vedu_intra_chnl4_ang_0_en.data = 0xffffffff;
    }
    all_ddr->vedu_intra_chnl4_ang_1_en.data = 0x7;

    if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
        all_ddr->vedu_intra_chnl8_ang_0_en.data = 0xffffffff;
    } else if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {
        all_ddr->vedu_intra_chnl8_ang_0_en.data = 0xffffffff;
    }
    all_ddr->vedu_intra_chnl8_ang_1_en.data = 0x7;
}

static hi_void ddr_cfg_reg_14(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
        all_ddr->vedu_intra_chnl16_ang_0_en.data = 0xffffffff;
    } else if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {
        all_ddr->vedu_intra_chnl16_ang_0_en.data = 0xffffffff;
    }
    all_ddr->vedu_intra_chnl16_ang_1_en.data = 0x7;

    all_ddr->vedu_intra_chnl32_ang_0_en.data = 0xffffffff;

    all_ddr->vedu_intra_chnl32_ang_1_en.data = 0x7;
    {
        u_vedu_pack_cu_parameter reg_data;
        reg_data.bits.pack_vcpi2cu_qp_min_cu_size = 0x2;
        all_ddr->vedu_pack_cu_parameter.data = reg_data.data;
    }
    {
        u_vedu_rgb2yuv_coef_p0 reg_data;
        reg_data.bits.rgb2yuv_coef_00 = 0;
        reg_data.bits.rgb2yuv_coef_01 = 0;
        reg_data.bits.rgb2yuv_coef_02 = 0;
        all_ddr->vedu_rgb2_yuv_coef_p0.data = reg_data.data;
    }
    {
        u_vedu_rgb2yuv_coef_p1  reg_data;
        reg_data.bits.rgb2yuv_coef_10 = 0;
        reg_data.bits.rgb2yuv_coef_11 = 0;
        reg_data.bits.rgb2yuv_coef_12 = 0;
        all_ddr->vedu_rgb2_yuv_coef_p1.data = reg_data.data;
    }
    {
        u_vedu_rgb2yuv_coef_p2 reg_data;
        reg_data.bits.rgb2yuv_coef_20 = 0;
        reg_data.bits.rgb2yuv_coef_21 = 0;
        reg_data.bits.rgb2yuv_coef_22 = 0;

        all_ddr->vedu_rgb2_yuv_coef_p2.data = reg_data.data;
    }
    {
        u_vedu_rgb2yuv_offset reg_data;
        reg_data.bits.rgb2yuv_offset_y = 0;
        reg_data.bits.rgb2yuv_offset_u = 0;
        reg_data.bits.rgb2yuv_offset_v = 0;
        all_ddr->vedu_rgb2_yuv_offset.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_15(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    {
        u_vedu_rgb2yuv_clip_thr_y  reg_data;
        reg_data.bits.rgb2yuv_clip_min_y = 0;
        reg_data.bits.rgb2yuv_clip_max_y = 0;
        all_ddr->vedu_rgb2_yuv_clip_thr_y.data = reg_data.data;
    }
    {
        u_vedu_rgb2yuv_clip_thr_c  reg_data;
        reg_data.bits.rgb2yuv_clip_min_c = 0;
        reg_data.bits.rgb2yuv_clip_max_c = 0;
        all_ddr->vedu_rgb2_yuv_clip_thr_c.data = reg_data.data;
    }
    {
        u_vedu_rgb2yuv_shift_width  reg_data;
        reg_data.bits.rgb2yuv_shift_width = 0;
        all_ddr->vedu_rgb2_yuv_shift_width.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_enable  reg_data;
        reg_data.bits.vcpi_osd_en = 0;
        reg_data.bits.vcpi_osd7_absqp = 0;
        reg_data.bits.vcpi_osd6_absqp = 0;
        reg_data.bits.vcpi_osd5_absqp = 0;
        reg_data.bits.vcpi_osd4_absqp = 0;
        reg_data.bits.vcpi_osd3_absqp = 0;
        reg_data.bits.vcpi_osd2_absqp = 0;
        reg_data.bits.vcpi_osd1_absqp = 0;
        reg_data.bits.vcpi_osd0_absqp = 0;
        reg_data.bits.vcpi_osd7_en = 0;
        reg_data.bits.vcpi_osd6_en = 0;
        reg_data.bits.vcpi_osd5_en = 0;
        reg_data.bits.vcpi_osd4_en = 0;
        reg_data.bits.vcpi_osd3_en = 0;
        reg_data.bits.vcpi_osd2_en = 0;
        reg_data.bits.vcpi_osd1_en = 0;
        reg_data.bits.vcpi_osd0_en = 0;
        reg_data.bits.vcpi_roi_osd_sel_0 = 0;
        reg_data.bits.vcpi_roi_osd_sel_1 = 0;
        reg_data.bits.vcpi_roi_osd_sel_2 = 0;
        reg_data.bits.vcpi_roi_osd_sel_3 = 0;
        reg_data.bits.vcpi_roi_osd_sel_4 = 0;
        reg_data.bits.vcpi_roi_osd_sel_5 = 0;
        reg_data.bits.vcpi_roi_osd_sel_6 = 0;
        reg_data.bits.vcpi_roi_osd_sel_7 = 0;
        all_ddr->vedu_vcpi_osd_enable.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_16(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    {
        u_vedu_vcpi_osd_pos_0 reg_data;
        reg_data.bits.vcpi_osd0_y = 0;
        reg_data.bits.vcpi_osd0_x = 0;
        all_ddr->vedu_vcpi_osd_pos_0.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_pos_1 reg_data;
        reg_data.bits.vcpi_osd1_y = 0;
        reg_data.bits.vcpi_osd1_x = 0;
        all_ddr->vedu_vcpi_osd_pos_1.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_pos_2 reg_data;
        reg_data.bits.vcpi_osd2_y = 0;
        reg_data.bits.vcpi_osd2_x = 0;
        all_ddr->vedu_vcpi_osd_pos_2.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_pos_3 reg_data;
        reg_data.bits.vcpi_osd3_y = 0;
        reg_data.bits.vcpi_osd3_x = 0;
        all_ddr->vedu_vcpi_osd_pos_3.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_pos_4 reg_data;
        reg_data.bits.vcpi_osd4_y = 0;
        reg_data.bits.vcpi_osd4_x = 0;
        all_ddr->vedu_vcpi_osd_pos_4.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_pos_5 reg_data;
        reg_data.bits.vcpi_osd5_y = 0;
        reg_data.bits.vcpi_osd5_x = 0;
        all_ddr->vedu_vcpi_osd_pos_5.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_pos_6 reg_data;
        reg_data.bits.vcpi_osd6_y = 0;
        reg_data.bits.vcpi_osd6_x = 0;
        all_ddr->vedu_vcpi_osd_pos_6.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_17(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    {
        u_vedu_vcpi_osd_pos_7 reg_data;
        reg_data.bits.vcpi_osd7_y = 0;
        reg_data.bits.vcpi_osd7_x = 0;
        all_ddr->vedu_vcpi_osd_pos_7.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_size_0 reg_data;
        reg_data.bits.vcpi_osd0_h = 0;
        reg_data.bits.vcpi_osd0_w = 0;
        all_ddr->vedu_vcpi_osd_size_0.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_size_1 reg_data;
        reg_data.bits.vcpi_osd1_h = 0;
        reg_data.bits.vcpi_osd1_w = 0;
        all_ddr->vedu_vcpi_osd_size_1.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_size_2 reg_data;
        reg_data.bits.vcpi_osd2_h = 0;
        reg_data.bits.vcpi_osd2_w = 0;
        all_ddr->vedu_vcpi_osd_size_2.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_size_3 reg_data;
        reg_data.bits.vcpi_osd3_h = 0;
        reg_data.bits.vcpi_osd3_w = 0;
        all_ddr->vedu_vcpi_osd_size_3.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_size_4 reg_data;
        reg_data.bits.vcpi_osd4_h = 0;
        reg_data.bits.vcpi_osd4_w = 0;
        all_ddr->vedu_vcpi_osd_size_4.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_size_5 reg_data;
        reg_data.bits.vcpi_osd5_h = 0;
        reg_data.bits.vcpi_osd5_w = 0;
        all_ddr->vedu_vcpi_osd_size_5.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_18(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    {
        u_vedu_vcpi_osd_size_6 reg_data;
        reg_data.bits.vcpi_osd6_h = 0;
        reg_data.bits.vcpi_osd6_w = 0;
        all_ddr->vedu_vcpi_osd_size_6.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_size_7 reg_data;
        reg_data.bits.vcpi_osd7_h = 0;
        reg_data.bits.vcpi_osd7_w = 0;
        all_ddr->vedu_vcpi_osd_size_7.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_layerid  reg_data;
        reg_data.bits.vcpi_osd7_layer_id = 0;
        reg_data.bits.vcpi_osd6_layer_id = 0;
        reg_data.bits.vcpi_osd5_layer_id = 0;
        reg_data.bits.vcpi_osd4_layer_id = 0;
        reg_data.bits.vcpi_osd3_layer_id = 0;
        reg_data.bits.vcpi_osd2_layer_id = 0;
        reg_data.bits.vcpi_osd1_layer_id = 0;
        reg_data.bits.vcpi_osd0_layer_id = 0;
        all_ddr->vedu_vcpi_osd_layerid.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_qp0 reg_data;
        reg_data.bits.vcpi_osd3_qp = 0;
        reg_data.bits.vcpi_osd2_qp = 0;
        reg_data.bits.vcpi_osd1_qp = 0;
        reg_data.bits.vcpi_osd0_qp = 0;
        all_ddr->vedu_vcpi_osd_qp0.data = reg_data.data;
    }
    {
        u_vedu_vcpi_osd_qp1 reg_data;
        reg_data.bits.vcpi_osd7_qp = 0;
        reg_data.bits.vcpi_osd6_qp = 0;
        reg_data.bits.vcpi_osd5_qp = 0;
        reg_data.bits.vcpi_osd4_qp = 0;
        all_ddr->vedu_vcpi_osd_qp1.data = reg_data.data;
    }

}

static hi_void ddr_cfg_reg_19(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    {
        u_vedu_curld_osd01_alpha reg_data;
        reg_data.bits.curld_osd0_alpha0 = 0;
        reg_data.bits.curld_osd0_alpha1 = 0;
        reg_data.bits.curld_osd1_alpha0 = 0;
        reg_data.bits.curld_osd1_alpha1 = 0;
        all_ddr->vedu_curld_osd01_alpha.data = reg_data.data;
    }
    {
        u_vedu_curld_osd23_alpha  reg_data;
        reg_data.bits.curld_osd2_alpha0 = 0;
        reg_data.bits.curld_osd2_alpha1 = 0;
        reg_data.bits.curld_osd3_alpha0 = 0;
        reg_data.bits.curld_osd3_alpha1 = 0;
        all_ddr->vedu_curld_osd23_alpha.data = reg_data.data;
    }
    {
        u_vedu_curld_osd45_alpha reg_data;
        reg_data.bits.curld_osd4_alpha0 = 0;
        reg_data.bits.curld_osd4_alpha1 = 0;
        reg_data.bits.curld_osd5_alpha0 = 0;
        reg_data.bits.curld_osd5_alpha1 = 0;
        all_ddr->vedu_curld_osd45_alpha.data = reg_data.data;
    }
    {
        u_vedu_curld_osd67_alpha reg_data;
        reg_data.bits.curld_osd6_alpha0 = 0;
        reg_data.bits.curld_osd6_alpha1 = 0;
        reg_data.bits.curld_osd7_alpha0 = 0;
        reg_data.bits.curld_osd7_alpha1 = 0;
        all_ddr->vedu_curld_osd67_alpha.data = reg_data.data;
    }
    {
        u_vedu_curld_osd_galpha0 reg_data;
        reg_data.bits.curld_osd0_global_alpha = 0;
        reg_data.bits.curld_osd1_global_alpha = 0;
        reg_data.bits.curld_osd2_global_alpha = 0;
        reg_data.bits.curld_osd3_global_alpha = 0;
        all_ddr->vedu_curld_osd_galpha0.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_20(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    {
        u_vedu_curld_osd_galpha1 reg_data;
        reg_data.bits.curld_osd4_global_alpha = 0;
        reg_data.bits.curld_osd5_global_alpha = 0;
        reg_data.bits.curld_osd6_global_alpha = 0;
        reg_data.bits.curld_osd7_global_alpha = 0;
        all_ddr->vedu_curld_osd_galpha1.data = reg_data.data;
    }

    all_ddr->vedu_curld_osd0_addr_l = 0;
    all_ddr->vedu_curld_osd1_addr_l = 0;
    all_ddr->vedu_curld_osd2_addr_l = 0;
    all_ddr->vedu_curld_osd3_addr_l = 0;
    all_ddr->vedu_curld_osd4_addr_l = 0;
    all_ddr->vedu_curld_osd5_addr_l = 0;
    all_ddr->vedu_curld_osd6_addr_l = 0;
    all_ddr->vedu_curld_osd7_addr_l = 0;
    all_ddr->vedu_curld_osd0_addr_h = 0x0;
    all_ddr->vedu_curld_osd1_addr_h = 0x0;
    all_ddr->vedu_curld_osd2_addr_h = 0x0;
    all_ddr->vedu_curld_osd3_addr_h = 0x0;
    all_ddr->vedu_curld_osd4_addr_h = 0x0;
    all_ddr->vedu_curld_osd5_addr_h = 0x0;
    all_ddr->vedu_curld_osd6_addr_h = 0x0;
    all_ddr->vedu_curld_osd7_addr_h = 0x0;
    {
        u_vedu_curld_osd01_stride reg_data;
        reg_data.bits.curld_osd0_stride = 0;
        reg_data.bits.curld_osd1_stride = 0;
        all_ddr->vedu_curld_osd01_stride.data = reg_data.data;
    }
    {
        u_vedu_curld_osd23_stride reg_data;
        reg_data.bits.curld_osd2_stride = 0;
        reg_data.bits.curld_osd3_stride = 0;
        all_ddr->vedu_curld_osd23_stride.data = reg_data.data;
    }
    {
        u_vedu_curld_osd45_stride reg_data;
        reg_data.bits.curld_osd4_stride = 0;
        reg_data.bits.curld_osd5_stride = 0;
        all_ddr->vedu_curld_osd45_stride.data =  reg_data.data;
    }
}

static hi_void ddr_cfg_reg_21(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_dynamic_cfg *dynamic_cfg,
                              venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_curld_osd67_stride reg_data;
        reg_data.bits.curld_osd6_stride = 0;
        reg_data.bits.curld_osd7_stride = 0;
        all_ddr->vedu_curld_osd67_stride.data = reg_data.data;
    }
    {
        u_vedu_vcpi_vlc_config reg_data;
        reg_data.bits.vcpi_byte_stuffing  = static_cfg->base_cfg.vcpi_byte_stuffing;
        reg_data.bits.vcpi_cabac_init_idc = static_cfg->base_cfg.vcpi_cabac_init_idc;

        if (reg_cfg->rc_cfg.pic_type == PIC_BIINTER) { /* for h264 */
            reg_data.bits.vcpi_ref_idc = 0;
        } else {
            reg_data.bits.vcpi_ref_idc = 0x3;
        }
        all_ddr->vedu_vcpi_vlc_config.data = reg_data.data;
    }
    {
        u_vedu_cabac_slchdr_size reg_data;
        reg_data.data = 0;
        reg_data.bits.cabac_slchdr_size_part1 = reg_cfg->stream_cfg.slc_hdr_bits[0];
        reg_data.bits.cabac_slchdr_size_part2 = reg_cfg->stream_cfg.slc_hdr_bits[1];
        all_ddr->vedu_cabac_slchdr_size.data = reg_data.data;
    }
    {
        u_vedu_cabac_slchdr_part1 reg_data;
        reg_data.data = 0;
        reg_data.bits.cabac_slchdr_part1 = reg_cfg->stream_cfg.slc_hdr_part1  << \
            (16 - reg_cfg->stream_cfg.slc_hdr_bits[0]); /* 16: slc hdr bit */
        all_ddr->vedu_cabac_slchdr_part1.data = reg_data.data;
    }
        ddr_cfg_cabac_slcher_part2_seg(reg_cfg);
    {
        u_vedu_cabac_slchdr_size_i reg_data;
        reg_data.data = 0;
        reg_data.bits.cabac_slchdr_size_part1_i = dynamic_cfg->slchdr_size_part1_i;
        reg_data.bits.cabac_slchdr_size_part2_i = dynamic_cfg->slchdr_size_part2_i;
        all_ddr->vedu_cabac_slchdr_size_i.data = reg_data.data;
    }
    {
        u_vedu_cabac_slchdr_part1_i reg_data;
        reg_data.data = 0;
        reg_data.bits.cabac_slchdr_part1_i = dynamic_cfg->slchdr_part1_i << \
            (16 - dynamic_cfg->slchdr_size_part1_i); /* 16: slc hdr bit */
        all_ddr->vedu_cabac_slchdr_part1_i.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_22(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_dynamic_cfg *dynamic_cfg,
                              venc_hal_static_cfg *static_cfg)
{
    ddr_cfg_cabac_slcher_part2_seg_i(dynamic_cfg, reg_cfg);
    {
        u_vedu_vlc_slchdrpara reg_data;
        reg_data.bits.vlc_parabit = ((reg_cfg->stream_cfg.slc_hdr_bits[0] >>  0) & 0xFF) - 1;
        reg_data.bits.vlc_reorderbit = ((reg_cfg->stream_cfg.slc_hdr_bits[0] >>  8) & 0xFF) - 1; /* 8: slc hdr bit */
        reg_data.bits.vlc_markingbit = ((reg_cfg->stream_cfg.slc_hdr_bits[0] >> 16) & 0xFF) - 1; /* 16: slc hdr bit */
        all_ddr->vedu_vlc_slchdrpara.data = reg_data.data;
    }
    {
        u_vedu_vlc_svc reg_data;
        reg_data.bits.vlc_svc_en = 0;
        reg_data.bits.vlc_svc_strm = static_cfg->base_cfg.vlc_svc_strm;
        all_ddr->vedu_vlc_svc.data = reg_data.data;
    }
    all_ddr->vedu_vlc_slchdrstrm0 = reg_cfg->stream_cfg.slc_hdr_stream [0];
    all_ddr->vedu_vlc_slchdrstrm1 = reg_cfg->stream_cfg.slc_hdr_stream [1];
    all_ddr->vedu_vlc_slchdrstrm2 = reg_cfg->stream_cfg.slc_hdr_stream [2]; /* 2: array index */
    all_ddr->vedu_vlc_slchdrstrm3 = reg_cfg->stream_cfg.slc_hdr_stream [3]; /* 3: array index */
    all_ddr->vedu_vlc_reorderstrm0 = reg_cfg->stream_cfg.reorder_stream[0];
    all_ddr->vedu_vlc_reorderstrm1 = reg_cfg->stream_cfg.reorder_stream[1];
    all_ddr->vedu_vlc_markingstrm0 = reg_cfg->stream_cfg.marking_stream[0];
    all_ddr->vedu_vlc_markingstrm1 = reg_cfg->stream_cfg.marking_stream[1];

    all_ddr->vedu_vlc_slchdrstrm0_i = dynamic_cfg->slc_hdr_stream_i [0];
    all_ddr->vedu_vlc_slchdrstrm1_i = dynamic_cfg->slc_hdr_stream_i [1];
    all_ddr->vedu_vlc_slchdrstrm2_i = dynamic_cfg->slc_hdr_stream_i [2]; /* 2: array index */
    all_ddr->vedu_vlc_slchdrstrm3_i = dynamic_cfg->slc_hdr_stream_i [3]; /* 3: array index */
    all_ddr->vedu_vlc_reorderstrm0_i = dynamic_cfg->reorder_stream_i[0];
    all_ddr->vedu_vlc_reorderstrm1_i = dynamic_cfg->reorder_stream_i[1];
    all_ddr->vedu_vlc_markingstrm0_i = dynamic_cfg->marking_stream_i[0];
    all_ddr->vedu_vlc_markingstrm1_i = dynamic_cfg->marking_stream_i[1];

    all_ddr->vedu_vlc_slchdrpara_i.bits.vlc_parabit_i = ((dynamic_cfg->slc_hdr_bits_i >>  0) & 0xFF) - 1;
    all_ddr->vedu_vlc_slchdrpara_i.bits.vlc_reorderbit_i = \
        ((dynamic_cfg->slc_hdr_bits_i >>  8) & 0xFF) - 1; /* 8: slc hdr bit */
    all_ddr->vedu_vlc_slchdrpara_i.bits.vlc_markingbit_i = \
        ((dynamic_cfg->slc_hdr_bits_i >> 16) & 0xFF) - 1; /* 16: slc hdr bit */

    all_ddr->vedu_vlcst_slc_cfg0 = 0;
    all_ddr->vedu_vlcst_slc_cfg1 = 0;
    all_ddr->vedu_vlcst_slc_cfg2 = 0;
    all_ddr->vedu_vlcst_slc_cfg3 = 0;
}

static hi_void ddr_cfg_reg_23(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_dynamic_cfg *dynamic_cfg,
                              venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_ice_v3r2_seg_256x1_dcmp_ctrl reg_data;
        reg_data.bits.is_lossless = 0;
        reg_data.bits.head_tword = 0;

        all_ddr->vedu_ice_v3_r2_seg_256_x1_dcmp_ctrl.data = reg_data.data;
    }
    {
        u_vedu_vcpi_low_power reg_data;
        reg_data.bits.vcpi_osd_clkgete_en = 0;
        reg_data.bits.vcpi_curld_dcmp_clkgate_en = 0;
        reg_data.bits.vcpi_refld_dcmp_clkgate_en = 0;
        reg_data.bits.vcpi_cpi_clkgate_en = 0;
        reg_data.bits.vcpi_vlc_clkgate_en = 0;
        reg_data.bits.vcpi_ref1_clkgate_en = 0;
        reg_data.bits.vcpi_rec_cmp_clkgate_en = 0;

        reg_data.bits.vcpi_mem_clkgate_en = static_cfg->mem_clk_gate_en;
        reg_data.bits.vcpi_clkgate_en = static_cfg->clk_gate_en;
        reg_data.bits.vcpi_mrg_gtck_en = 1;
        reg_data.bits.vcpi_tqitq_gtck_en = 1;
        reg_data.bits.vcpi_ime_lowpow_en = static_cfg->low_power_cfg.vcpi_ime_lowpow;
        reg_data.bits.vcpi_fme_lowpow_en = static_cfg->low_power_cfg.vcpi_fme_lowpow;
        reg_data.bits.vcpi_intra_lowpow_en = static_cfg->low_power_cfg.vcpi_intra_lowpow;
        all_ddr->vedu_vcpi_low_power.data = reg_data.data;
    }
    {
        u_vedu_vcpi_pre_judge_ext_en reg_data;
        reg_data.bits.vcpi_ext_edge_en = static_cfg->base_cfg.vcpi_ext_edge_en;
        reg_data.bits.vcpi_pintra_inter_flag_disable = static_cfg->base_cfg.vcpi_pintra_inter_flag_disable;
        reg_data.bits.vcpi_force_inter = static_cfg->base_cfg.vcpi_force_inter;
        reg_data.bits.vcpi_pblk_pre_en = static_cfg->base_cfg.vcpi_pblk_pre_en;
        all_ddr->vedu_vcpi_pre_judge_ext_en.data = reg_data.data;
    }
    {
        u_vedu_vcpi_pre_judge_cost_thr reg_data;
        reg_data.bits.vcpi_pblk_pre_cost_thr = static_cfg->base_cfg.vcpi_pblk_pre_cost_thr;
        reg_data.bits.vcpi_iblk_pre_cost_thr = static_cfg->base_cfg.vcpi_iblk_pre_cost_thr;
        all_ddr->vedu_vcpi_pre_judge_cost_thr.data = reg_data.data;
    }
    {
        u_vedu_vcpi_pme_param reg_data;
        reg_data.bits.vcpi_move_sad_en = dynamic_cfg->qpg_cfg_dynamic.move_sad_en;
        reg_data.bits.vcpi_pblk_pre_mvx_thr = static_cfg->base_cfg.pblk_pre_mvx_thr;
        reg_data.bits.vcpi_pblk_pre_mvy_thr = static_cfg->base_cfg.pblk_pre_mvy_thr;
        all_ddr->vedu_vcpi_pme_param.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_24(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_pme_pblk_pre1 reg_data;
        reg_data.bits.pme_pblk_pre_mv_dif_thr1 = static_cfg->pme_cfg_static.pme_pblk_pre_mv_dif_thr1;
        reg_data.bits.pme_pblk_pre_mv_dif_thr0 = static_cfg->pme_cfg_static.pme_pblk_pre_mv_dif_thr0;
        reg_data.bits.pme_pblk_pre_mv_dif_cost_thr = static_cfg->pme_cfg_static.pme_pblk_pre_mv_dif_cost_thr;
        all_ddr->vedu_pme_pblk_pre1.data = reg_data.data;
    }
    {
        u_vedu_pme_pblk_pre2 reg_data;
        reg_data.bits.pme_pblk_pre_madi_times = static_cfg->pme_cfg_static.pme_pblk_pre_madi_times;
        reg_data.bits.pme_pblk_pre_offset = static_cfg->pme_cfg_static.pme_pblk_pre_offset;
        reg_data.bits.pme_pblk_pre_gain = static_cfg->pme_cfg_static.pme_pblk_pre_gain;
        all_ddr->vedu_pme_pblk_pre2.data = reg_data.data;
    }
    {
        u_vedu_pme_sw_adapt_en reg_data;
        reg_data.bits.pme_l0_psw_adapt_en = static_cfg->pme_cfg_static.l0_psw_adapt_en;
        reg_data.bits.pme_l1_psw_adapt_en = 1;
        all_ddr->vedu_pme_sw_adapt_en.data = reg_data.data;
    }
    {
        u_vedu_pme_window_size0_l0 reg_data;
        reg_data.bits.pme_l0_win0_width = static_cfg->pme_cfg_static.l0_win0_width;
        reg_data.bits.pme_l0_win0_height = static_cfg->pme_cfg_static.l0_win0_height;
        all_ddr->vedu_pme_window_size0_l0.data = reg_data.data;
    }
    {
        u_vedu_pme_window_size0_l1 reg_data;
        reg_data.bits.pme_l1_win0_width = static_cfg->pme_cfg_static.l1_win0_width;
        reg_data.bits.pme_l1_win0_height = static_cfg->pme_cfg_static.l1_win0_height;
        all_ddr->vedu_pme_window_size0_l1.data = reg_data.data;
    }
    {
        u_vedu_pme_psw_lpw reg_data;
        reg_data.bits.pme_psw_lp_diff_thx = static_cfg->pme_cfg_static.pme_psw_lp_diff_thx;
        reg_data.bits.pme_psw_lp_diff_thy = static_cfg->pme_cfg_static.pme_psw_lp_diff_thy;
        all_ddr->vedu_pme_psw_lpw.data = reg_data.data  ;
    }
    {
        u_vedu_pme_skip_pre reg_data;
        reg_data.bits.pme_skipblk_pre_en = static_cfg->pme_cfg_static.skipblk_pre_en;
        reg_data.bits.pme_skipblk_pre_cost_thr = static_cfg->pme_cfg_static.skipblk_pre_cost_thr;
        all_ddr->vedu_pme_skip_pre.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_25(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_pme_skip_flag reg_data;
        reg_data.bits.vcpi_pskip_strongedge_madi_thr = static_cfg->pme_cfg_static.vcpi_pskip_strongedge_madi_thr;
        reg_data.bits.vcpi_pskip_strongedge_madi_times = static_cfg->pme_cfg_static.vcpi_pskip_strongedge_madi_times;
        reg_data.bits.vcpi_pskip_flatregion_madi_thr = static_cfg->pme_cfg_static.vcpi_pskip_flatregion_madi_thr;
        all_ddr->vedu_pme_skip_flag.data = reg_data.data;
    }
    {
        u_vedu_pme_tr_weightx reg_data;
        reg_data.bits.pme_tr_weightx_2 = static_cfg->pme_cfg_static.tr_weightx_2;
        reg_data.bits.pme_tr_weightx_1 = static_cfg->pme_cfg_static.tr_weightx_1;
        reg_data.bits.pme_tr_weightx_0 = static_cfg->pme_cfg_static.tr_weightx_0;
        all_ddr->vedu_pme_tr_weightx.data = reg_data.data;
    }
    {
        u_vedu_pme_tr_weighty reg_data;
        reg_data.bits.pme_tr_weighty_2 = static_cfg->pme_cfg_static.tr_weighty_2;
        reg_data.bits.pme_tr_weighty_1 = static_cfg->pme_cfg_static.tr_weighty_1;
        reg_data.bits.pme_tr_weighty_0 = static_cfg->pme_cfg_static.tr_weighty_0;
        all_ddr->vedu_pme_tr_weighty.data = reg_data.data;
    }
    {
        u_vedu_pme_sr_weight reg_data;
        reg_data.bits.pme_sr_weight_2 = static_cfg->pme_cfg_static.sr_weight_2;
        reg_data.bits.pme_sr_weight_1 = static_cfg->pme_cfg_static.sr_weight_1;
        reg_data.bits.pme_sr_weight_0 = static_cfg->pme_cfg_static.sr_weight_0;
        all_ddr->vedu_pme_sr_weight.data = reg_data.data;
    }
    {
        u_vedu_ime_fme_lpow_thr reg_data;
        reg_data.bits.ime_lowpow_fme_thr1 = static_cfg->low_power_cfg.vcpi_lowpow_fme_thr1;
        reg_data.bits.ime_lowpow_fme_thr0 = static_cfg->low_power_cfg.vcpi_lowpow_fme_thr0;
        all_ddr->vedu_ime_fme_lpow_thr.data = reg_data.data;
    }
    {
        u_vedu_fme_pu64_lwp reg_data;
        reg_data.bits.fme_pu64_lwp_flag = 0x0 ; /* enc_para->low_power_cfg.fme_pu64_lwp_flag */
        reg_data.bits.vcpi_detect_low3_en = 0x0 ; /* enc_para->low_power_cfg.vcpi_detect_low3_en */
        reg_data.bits.vcpi_strong_edge_with_space_en = 0;
        all_ddr->vedu_fme_pu64_lwp.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_26(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_mrg_force_zero_en reg_data;
        if (reg_cfg->rc_cfg.avbr && !reg_cfg->rc_cfg.intra_pic) {
            reg_data.bits.mrg_force_v_zero_en = 1;
            reg_data.bits.mrg_force_u_zero_en = 1;
            reg_data.bits.mrg_force_y_zero_en = 1;
            reg_data.bits.mrg_force_zero_en   = 1;
        } else {
            reg_data.bits.mrg_force_v_zero_en = static_cfg->base_cfg.force_v_zero_en;
            reg_data.bits.mrg_force_u_zero_en = static_cfg->base_cfg.force_u_zero_en;
            reg_data.bits.mrg_force_y_zero_en = static_cfg->base_cfg.force_y_zero_en;
            reg_data.bits.mrg_force_zero_en   = static_cfg->base_cfg.force_zero_en;
        }
        reg_data.bits.force_adapt_en = 0;
        reg_data.bits.fme_rdo_lpw_en = 0;
        reg_data.bits.dct4_en = 1;
        reg_data.bits.fme_rdo_lpw_th = 0;
        reg_data.bits.mrg_skip_weight_en = 0;
        all_ddr->vedu_mrg_force_zero_en.data = reg_data.data;
    }
    {
        u_vedu_mrg_force_skip_en reg_data;
        reg_data.bits.inter32_use_tu16_en = 1; /* enc_para->base_cfg.inter32_use_tu16_en */
        reg_data.bits.mrg_detect_intercu32_use_tu16 = 0x0; /* enc_para->low_power_cfg.mrg_detect_intercu32_use_tu16 */
        reg_data.bits.mrg_not_use_sad_en = 0x0; /* enc_para->low_power_cfg.mrg_not_use_sad_en */
        reg_data.bits.mrg_not_use_sad_th = 0x4;
        all_ddr->vedu_mrg_force_skip_en.data = reg_data.data;
    }
    {
        u_vedu_ime_inter_mode reg_data;
        reg_data.bits.ime_intra4_lowpow_en = 0x0 ;
        reg_data.bits.ime_high3pre_en = static_cfg->low_power_cfg.vcpi_high3pre_en;
        reg_data.bits.ime_inter8x8_en = static_cfg->base_cfg.vcpi_inter8x8_en; /* for h264 */
        reg_data.bits.ime_layer3to2_en  = 0;
        all_ddr->vedu_ime_inter_mode.data = reg_data.data;
    }
    {
        u_vedu_ime_layer3to2_thr reg_data;
        reg_data.bits.ime_layer3to2_thr0 = 0x70;
        reg_data.bits.ime_layer3to2_thr1 = 0x10e;
        all_ddr->vedu_ime_layer3_to2_thr.data = reg_data.data;
    }
    {
        u_vedu_ime_layer3to2_thr1 reg_data;
        reg_data.bits.ime_layer3to2_cost_diff_thr = 0x100;
        all_ddr->vedu_ime_layer3_to2_thr1.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_27(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_ime_layer3to1_thr reg_data;
        reg_data.bits.ime_layer3to1_en = 0x0; /* enc_para->low_power_cfg.ime_layer3to1_en */
        reg_data.bits.ime_layer3to1_pu64_madi_thr = 0x5;
        all_ddr->vedu_ime_layer3_to1_thr.data = reg_data.data;
    }
    {
        u_vedu_ime_layer3to1_thr1 reg_data;
        reg_data.bits.ime_layer3to1_pu32_cost_thr = 0xbb8;
        reg_data.bits.ime_layer3to1_pu64_cost_thr = 0x1000;
        all_ddr->vedu_ime_layer3_to1_thr1.data = reg_data.data;
    }
    {
        u_vedu_vcpi_intra32_low_power reg_data;
        reg_data.bits.vcpi_intra32_low_power_thr = 0x400;
        reg_data.bits.vcpi_intra32_low_power_en = 1;
        reg_data.bits.vcpi_intra32_low_power_gain = 0x8;
        reg_data.bits.vcpi_intra32_low_power_offset = 0x2;
        all_ddr->vedu_vcpi_intra32_low_power.data = reg_data.data;
    }
    {
        u_vedu_vcpi_intra16_low_power reg_data;
        reg_data.bits.vcpi_intra16_low_power_thr = 0x100;
        reg_data.bits.vcpi_intra16_low_power_en = 0;
        reg_data.bits.vcpi_intra16_low_power_gain = 0;
        reg_data.bits.vcpi_intra16_low_power_offset = 0;
        all_ddr->vedu_vcpi_intra16_low_power.data = reg_data.data;
    }
    {
        u_vedu_pme_intra_lowpow reg_data;
        reg_data.bits.pme_intra16_madi_thr = static_cfg->pme_cfg_static.pme_intra16_madi_thr;
        reg_data.bits.pme_intra32_madi_thr = static_cfg->pme_cfg_static.pme_intra32_madi_thr;
        reg_data.bits.pme_intra_lowpow_en  = 0;
        all_ddr->vedu_pme_intra_lowpow.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_28(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_dynamic_cfg *dynamic_cfg,
                              venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_intra_low_pow0 reg_data;
        reg_data.bits.cu16_weak_ang_thr = 0x3;
        reg_data.bits.cu16_medium_ang_thr = 0x5;
        reg_data.bits.cu16_strong_ang_thr = 0xb;
        reg_data.bits.cu16_rdo_num = 0x3;
        reg_data.bits.cu16_adaptive_reduce_rdo_en = 0;
        reg_data.bits.cu32_weak_ang_thr = 0x3;
        reg_data.bits.cu32_medium_ang_thr = 0x6;
        reg_data.bits.cu32_strong_ang_thr = 0xd;
        reg_data.bits.cu32_rdo_num = 0x2;
        reg_data.bits.cu32_adaptive_reduce_rdo_en = 0;
        all_ddr->vedu_intra_low_pow0.data = reg_data.data;
    }
    {
        u_vedu_intra_low_pow1 reg_data;
        reg_data.bits.vcpi_force_cu16_low_pow = 0;
        reg_data.bits.vcpi_force_cu32_low_pow = 0;
        reg_data.bits.vcpi_detect_close_intra32_en = 0x0 ; /* enc_para->low_power_cfg.vcpi_detect_close_intra32_en */
        all_ddr->vedu_intra_low_pow1.data = reg_data.data;
    }
    {
        u_vedu_vcpi_intra_inter_cu_en reg_data;
        reg_data.bits.vcpi_mrg_cu_en = static_cfg->base_cfg.vcpi_mrg_cu_en;
        reg_data.bits.vcpi_fme_cu_en = static_cfg->base_cfg.vcpi_fme_cu_en;
        reg_data.bits.vcpi_ipcm_en = static_cfg->base_cfg.vcpi_ipcm_en;
        reg_data.bits.vcpi_intra_cu_en = dynamic_cfg->vcpi_intra_cu_en;
        reg_data.bits.vcpi_intra_h264_cutdiag = static_cfg->base_cfg.vcpi_intra_h264_cutdiag;
        all_ddr->vedu_vcpi_intra_inter_cu_en.data = reg_data.data;
    }
    {
        u_vedu_pack_pcm_parameter reg_data;
        reg_data.bits.pack_vcpi2pu_log2_max_ipcm_cbsizey = 0x3 ;
        reg_data.bits.pack_vcpi2pu_log2_min_ipcm_cbsizey = 0x3 ;
        all_ddr->vedu_pack_pcm_parameter.data = reg_data.data;
    }
    {
        u_vedu_qpg_cu32_delta reg_data;
        reg_data.bits.qpg_cu32_delta_low = static_cfg->qpg_cfg_static.cu32_delta_low;
        reg_data.bits.qpg_cu32_delta_high = static_cfg->qpg_cfg_static.cu32_delta_high;
        all_ddr->vedu_qpg_cu32_delta.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_29(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_qpg_qp_restrain reg_data;
        reg_data.bits.qpg_qp_restrain_delta_blk32 = static_cfg->qpg_cfg_static.qp_restrain_delta_blk32 ;
        reg_data.bits.qpg_qp_restrain_delta_blk16 = static_cfg->qpg_cfg_static.qp_restrain_delta_blk16;
        reg_data.bits.qpg_qp_restrain_en = static_cfg->qpg_cfg_static.qp_restrain_en;
        reg_data.bits.qpg_qp_restrain_madi_thr = static_cfg->qpg_cfg_static.qp_restrain_madi_thr;
        all_ddr->vedu_qpg_qp_restrain.data = reg_data.data;
    }
    {
        u_vedu_pme_qpg_rc_thr1 reg_data;
        reg_data.bits.pme_min_sad_thr_gain = static_cfg->pme_cfg_static.min_sad_thr_gain;
        reg_data.bits.pme_min_sad_thr_offset = static_cfg->pme_cfg_static.min_sad_thr_offset;
        reg_data.bits.pme_min_sad_thr_offset_cur = static_cfg->pme_cfg_static.pme_min_sad_thr_offset_cur;
        reg_data.bits.pme_min_sad_thr_gain_cur = static_cfg->pme_cfg_static.pme_min_sad_thr_gain_cur;
        all_ddr->vedu_pme_qpg_rc_thr1.data = reg_data.data;
    }
    {
        u_vedu_qpg_cu_min_sad_reg reg_data;
        reg_data.bits.qpg_min_sad_level = static_cfg->qpg_cfg_static.min_sad_level;
        reg_data.bits.qpg_min_sad_madi_en = static_cfg->qpg_cfg_static.min_sad_madi_en;
        reg_data.bits.qpg_min_sad_qp_restrain_en = static_cfg->qpg_cfg_static.min_sad_qp_restrain_en ;
        reg_data.bits.qpg_low_min_sad_mode = static_cfg->qpg_cfg_static.low_min_sad_mode;
        all_ddr->vedu_qpg_cu_min_sad_reg.data  =  reg_data.data;
    }
    {
        u_vedu_qpg_lambda_mode reg_data;
        reg_data.bits.qpg_rdo_lambda_choose_mode = static_cfg->qpg_cfg_static.rdo_lambda_choose_mode;
        reg_data.bits.qpg_lambda_inter_stredge_en = static_cfg->qpg_cfg_static.lambda_inter_stredge_en;
        reg_data.bits.qpg_lambda_qp_offset = static_cfg->qpg_cfg_static.lambda_qp_offset;
        all_ddr->vedu_qpg_lambda_mode.data = reg_data.data;
    }

    cfg_sw_l0_size(reg_cfg);
    cfg_sw_l1_size(reg_cfg);
    {
        u_vedu_pme_new_cost reg_data;
        reg_data.bits.pme_mvp3median_en = static_cfg->pme_cfg_static.mvp3median_en;
        reg_data.bits.pme_new_cost_en = static_cfg->pme_cfg_static.new_cost_en;
        reg_data.bits.pme_cost_lamda2 = static_cfg->pme_cfg_static.cost_lamda2;
        reg_data.bits.pme_cost_lamda1 = static_cfg->pme_cfg_static.cost_lamda1;
        reg_data.bits.pme_cost_lamda0 = static_cfg->pme_cfg_static.cost_lamda0;
        all_ddr->vedu_pme_new_cost.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_30(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_pme_new_lambda reg_data;
        reg_data.bits.vcpi_new_lambda    = static_cfg->pme_cfg_static.vcpi_new_lambda;
        all_ddr->vedu_pme_new_lambda.data = reg_data.data;
    }
    {
        u_vedu_pme_new_madi_th reg_data;
        reg_data.bits.vcpi_new_madi_th0 = static_cfg->pme_cfg_static.vcpi_new_madi_th0;
        reg_data.bits.vcpi_new_madi_th1 = static_cfg->pme_cfg_static.vcpi_new_madi_th1;
        reg_data.bits.vcpi_new_madi_th2 = static_cfg->pme_cfg_static.vcpi_new_madi_th2;
        reg_data.bits.vcpi_new_madi_th3 = static_cfg->pme_cfg_static.vcpi_new_madi_th3;
        all_ddr->vedu_pme_new_madi_th.data = reg_data.data;
    }
    {
        u_vedu_pme_cost_offset reg_data;
        reg_data.bits.pme_l0_cost_offset = static_cfg->pme_cfg_static.l0_cost_offset;
        reg_data.bits.pme_l1_cost_offset = static_cfg->pme_cfg_static.l1_cost_offset;
        all_ddr->vedu_pme_cost_offset.data = reg_data.data;
    }
    {
        u_vedu_pme_adjust_pmemv_h264 reg_data;
        reg_data.bits.pme_adjust_pmemv_dist_times = static_cfg->pme_cfg_static.pme_adjust_pmemv_dist_times;
        reg_data.bits.pme_adjust_pmemv_en = static_cfg->pme_cfg_static.pme_adjust_pmemv_en;
        all_ddr->vedu_pme_adjust_pmemv_h264.data = reg_data.data;
    }
    {
        u_vedu_pme_qpg_rc_th0 reg_data;
        reg_data.bits.pme_madi_dif_thr = static_cfg->pme_cfg_static.madi_dif_thr;
        reg_data.bits.pme_cur_madi_dif_thr = static_cfg->pme_cfg_static.cur_madi_dif_thr;
        all_ddr->vedu_pme_qpg_rc_thr0.data = reg_data.data;
    }
    {
        u_vedu_vlcst_descriptor reg_data;
        reg_data.bits.vlcst_chnid = 0;
        if (reg_cfg->rc_cfg.intra_pic) {
            reg_data.bits.vlcst_idrind = 1;
        } else {
            reg_data.bits.vlcst_idrind = 0;
        }
        all_ddr->vedu_vlcst_descriptor.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_31(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_curld_clip_thr reg_data;
        reg_data.bits.curld_clip_luma_min = static_cfg->base_cfg.clip_luma_min;
        reg_data.bits.curld_clip_luma_max = static_cfg->base_cfg.clip_luma_max;
        reg_data.bits.curld_clip_chrm_min = static_cfg->base_cfg.clip_chrm_min;
        reg_data.bits.curld_clip_chrm_max = static_cfg->base_cfg.clip_chrm_max;
        all_ddr->vedu_curld_clip_thr.data = reg_data.data;
    }
    {
        u_vedu_tqitq_deadzone reg_data;
        reg_data.bits.tqitq_deadzone_intra_slice = static_cfg->q_scaling_cfg.deadzone_intra_slice;
        reg_data.bits.tqitq_deadzone_inter_slice = static_cfg->q_scaling_cfg.deadzone_inter_slice;
        reg_data.bits.tqitq_bitest_magth = 0;
        all_ddr->vedu_tqitq_deadzone.data = reg_data.data;
    }
    {
        u_vedu_vcpi_pintra_texture_strength reg_data;
        reg_data.bits.vcpi_pintra_intra8_texture_strength_multi = 0;
        reg_data.bits.vcpi_pintra_intra8_texture_strength = 0;
        all_ddr->vedu_vcpi_pintra_texture_strength.data = reg_data.data;
    }
    {
        u_vedu_vcpi_noforcezero reg_data;
        reg_data.bits.vcpi_bislayer0flag = 0;
        reg_data.bits.vcpi_bnoforcezero_flag = 0;
        reg_data.bits.vcpi_bnoforcezero_posx = 0;
        reg_data.bits.vcpi_bnoforcezero_posy = 0;
        all_ddr->vedu_vcpi_noforcezero.data = reg_data.data;
    }
}

#ifdef VENC_SUPPORT_ROI
static hi_void ddr_cfg_reg_32(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vcpi_roi_cfg0 reg_data;
        reg_data.bits.vctrl_roi_en = static_cfg->roi_cfg.enable[7] | \  /* 7:array idx */
            static_cfg->roi_cfg.enable[6] | /* 6:array idx */
            static_cfg->roi_cfg.enable[5] | static_cfg->roi_cfg.enable[4] | \  /* 4, 5: array index */
            static_cfg->roi_cfg.enable[3] |  /* 3: array index */
            static_cfg->roi_cfg.enable[2] | static_cfg->roi_cfg.enable[1] | \  /* 2: array index */
            static_cfg->roi_cfg.enable[0];
        reg_data.bits.vctrl_region7keep = static_cfg->roi_cfg.keep[7]; /* 7: array index */
        reg_data.bits.vctrl_region6keep = static_cfg->roi_cfg.keep[6]; /* 6: array index */
        reg_data.bits.vctrl_region5keep = static_cfg->roi_cfg.keep[5]; /* 5: array index */
        reg_data.bits.vctrl_region4keep = static_cfg->roi_cfg.keep[4]; /* 4: array index */
        reg_data.bits.vctrl_region3keep = static_cfg->roi_cfg.keep[3]; /* 3: array index */
        reg_data.bits.vctrl_region2keep = static_cfg->roi_cfg.keep[2]; /* 2: array index */
        reg_data.bits.vctrl_region1keep = static_cfg->roi_cfg.keep[1];
        reg_data.bits.vctrl_region0keep = static_cfg->roi_cfg.keep[0];
        reg_data.bits.vctrl_absqp7 = static_cfg->roi_cfg.abs_qp_en[7]; /* 7: array index */
        reg_data.bits.vctrl_absqp6 = static_cfg->roi_cfg.abs_qp_en[6]; /* 6: array index */
        reg_data.bits.vctrl_absqp5 = static_cfg->roi_cfg.abs_qp_en[5]; /* 5: array index */
        reg_data.bits.vctrl_absqp4 = static_cfg->roi_cfg.abs_qp_en[4]; /* 4: array index */
        reg_data.bits.vctrl_absqp3 = static_cfg->roi_cfg.abs_qp_en[3]; /* 3: array index */
        reg_data.bits.vctrl_absqp2 = static_cfg->roi_cfg.abs_qp_en[2]; /* 2: array index */
        reg_data.bits.vctrl_absqp1 = static_cfg->roi_cfg.abs_qp_en[1];
        reg_data.bits.vctrl_absqp0 = static_cfg->roi_cfg.abs_qp_en[0];
        reg_data.bits.vctrl_region7en = static_cfg->roi_cfg.enable[7]; /* 7: array index */
        reg_data.bits.vctrl_region6en = static_cfg->roi_cfg.enable[6]; /* 6: array index */
        reg_data.bits.vctrl_region5en = static_cfg->roi_cfg.enable[5]; /* 5: array index */
        reg_data.bits.vctrl_region4en = static_cfg->roi_cfg.enable[4]; /* 4: array index */
        reg_data.bits.vctrl_region3en = static_cfg->roi_cfg.enable[3]; /* 3: array index */
        reg_data.bits.vctrl_region2en = static_cfg->roi_cfg.enable[2]; /* 2: array index */
        reg_data.bits.vctrl_region1en = static_cfg->roi_cfg.enable[1];
        reg_data.bits.vctrl_region0en = static_cfg->roi_cfg.enable[0];
        all_ddr->vedu_vctrl_roi_cfg0.data = reg_data.data;
    }
    {
        u_vedu_vcpi_roi_cfg1 reg_data;
        reg_data.bits.vctrl_roiqp3 = static_cfg->roi_cfg.qp[3]; /* 3: array index */
        reg_data.bits.vctrl_roiqp2 = static_cfg->roi_cfg.qp[2]; /* 2: array index */
        reg_data.bits.vctrl_roiqp1 = static_cfg->roi_cfg.qp[1];
        reg_data.bits.vctrl_roiqp0 = static_cfg->roi_cfg.qp[0];
        all_ddr->vedu_vctrl_roi_cfg1.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_33(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_roi_cfg2 reg_data;
        reg_data.bits.vctrl_roiqp7 = static_cfg->roi_cfg.qp[7]; /* 7: array index */
        reg_data.bits.vctrl_roiqp6 = static_cfg->roi_cfg.qp[6]; /* 6: array index */
        reg_data.bits.vctrl_roiqp5 = static_cfg->roi_cfg.qp[5]; /* 5: array index */
        reg_data.bits.vctrl_roiqp4 = static_cfg->roi_cfg.qp[4]; /* 4: array index */
        all_ddr->vedu_vctrl_roi_cfg2.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_size_0 reg_data;
        reg_data.bits.vctrl_size0_roiheight = static_cfg->roi_cfg.height[0];
        reg_data.bits.vctrl_size0_roiwidth = static_cfg->roi_cfg.width[0];
        all_ddr->vedu_vctrl_roi_size_0.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_size_1 reg_data;
        reg_data.bits.vctrl_size1_roiheight = static_cfg->roi_cfg.height[1];
        reg_data.bits.vctrl_size1_roiwidth = static_cfg->roi_cfg.width[1];
        all_ddr->vedu_vctrl_roi_size_1.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_size_2 reg_data;
        reg_data.bits.vctrl_size2_roiheight = static_cfg->roi_cfg.height[2]; /* 2: array index */
        reg_data.bits.vctrl_size2_roiwidth = static_cfg->roi_cfg.width[2]; /* 2: array index */
        all_ddr->vedu_vctrl_roi_size_2.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_size_3 reg_data;
        reg_data.bits.vctrl_size3_roiheight = static_cfg->roi_cfg.height[3]; /* 3: array index */
        reg_data.bits.vctrl_size3_roiwidth = static_cfg->roi_cfg.width[3]; /* 3: array index */
        all_ddr->vedu_vctrl_roi_size_3.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_size_4 reg_data;
        reg_data.bits.vctrl_size4_roiheight = static_cfg->roi_cfg.height[4]; /* 4: array index */
        reg_data.bits.vctrl_size4_roiwidth = static_cfg->roi_cfg.width[4]; /* 4: array index */
        all_ddr->vedu_vctrl_roi_size_4.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_size_5 reg_data;
        reg_data.bits.vctrl_size5_roiheight = static_cfg->roi_cfg.height[5]; /* 5: array index */
        reg_data.bits.vctrl_size5_roiwidth = static_cfg->roi_cfg.width[5]; /* 5: array index */
        all_ddr->vedu_vctrl_roi_size_5.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_34(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_roi_size_6 reg_data;
        reg_data.bits.vctrl_size6_roiheight = static_cfg->roi_cfg.height[6]; /* 6: array index */
        reg_data.bits.vctrl_size6_roiwidth = static_cfg->roi_cfg.width[6]; /* 6: array index */
        all_ddr->vedu_vctrl_roi_size_6.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_size_7 reg_data;
        reg_data.bits.vctrl_size7_roiheight = static_cfg->roi_cfg.height[7]; /* 7: array index */
        reg_data.bits.vctrl_size7_roiwidth = static_cfg->roi_cfg.width[7]; /* 7: array index */
        all_ddr->vedu_vctrl_roi_size_7.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_start_0 reg_data;
        reg_data.bits.vctrl_start0_roistarty = static_cfg->roi_cfg.start_y[0];
        reg_data.bits.vctrl_start0_roistartx = static_cfg->roi_cfg.start_x[0];
        all_ddr->vedu_vctrl_roi_start_0.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_start_1 reg_data;
        reg_data.bits.vctrl_start1_roistarty = static_cfg->roi_cfg.start_y[1];
        reg_data.bits.vctrl_start1_roistartx = static_cfg->roi_cfg.start_x[1];
        all_ddr->vedu_vctrl_roi_start_1.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_35(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_roi_start_2 reg_data;
        reg_data.bits.vctrl_start2_roistarty = static_cfg->roi_cfg.start_y[2]; /* 2: array index */
        reg_data.bits.vctrl_start2_roistartx = static_cfg->roi_cfg.start_x[2]; /* 2: array index */
        all_ddr->vedu_vctrl_roi_start_2.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_start_3 reg_data;
        reg_data.bits.vctrl_start3_roistarty = static_cfg->roi_cfg.start_y[3]; /* 3: array index */
        reg_data.bits.vctrl_start3_roistartx = static_cfg->roi_cfg.start_x[3]; /* 3: array index */
        all_ddr->vedu_vctrl_roi_start_3.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_start_4 reg_data;
        reg_data.bits.vctrl_start4_roistarty = static_cfg->roi_cfg.start_y[4]; /* 4: array index */
        reg_data.bits.vctrl_start4_roistartx = static_cfg->roi_cfg.start_x[4]; /* 4: array index */
        all_ddr->vedu_vctrl_roi_start_4.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_start_5 reg_data;
        reg_data.bits.vctrl_start5_roistarty = static_cfg->roi_cfg.start_y[5]; /* 5: array index */
        reg_data.bits.vctrl_start5_roistartx = static_cfg->roi_cfg.start_x[5]; /* 5: array index */
        all_ddr->vedu_vctrl_roi_start_5.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_start_6 reg_data;
        reg_data.bits.vctrl_start6_roistarty = static_cfg->roi_cfg.start_y[6]; /* 6: array index */
        reg_data.bits.vctrl_start6_roistartx = static_cfg->roi_cfg.start_x[6]; /* 6: array index */
        all_ddr->vedu_vctrl_roi_start_6.data = reg_data.data;
    }
    {
        u_vedu_vctrl_roi_start_7 reg_data;
        reg_data.bits.vctrl_start7_roistarty = static_cfg->roi_cfg.start_y[7]; /* 7: array index */
        reg_data.bits.vctrl_start7_roistartx = static_cfg->roi_cfg.start_x[7]; /* 7: array index */
        all_ddr->vedu_vctrl_roi_start_7.data = reg_data.data;
    }
}
#endif

static hi_void ddr_cfg_reg_36(s_vedu_regs_type *all_ddr)
{
    all_ddr->vedu_vctrl_roi_cfg0.data = 0;
}

static hi_void ddr_cfg_reg_37(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_dynamic_cfg *dynamic_cfg,
                              venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_intra_bit_weight reg_data;
        reg_data.bits.intra_bit_weight = static_cfg->base_cfg.intra_bit_weight;
        reg_data.bits.mpm8_th = 0;
        all_ddr->vedu_intra_bit_weight.data = reg_data.data;
    }
    {
        u_vedu_intra_rdo_cost_offset_3 reg_data;
        reg_data.bits.intra_h264_rdo_cost_offset     = 0;
        all_ddr->vedu_intra_rdo_cost_offset_3.data = reg_data.data;
    }
    {
        u_vedu_vctrl_intra_rdo_factor_0 reg_data;
        reg_data.bits.vctrl_norm_intra_cu4_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_intra_cu4_rdcost_offset;
        reg_data.bits.vctrl_norm_intra_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_intra_cu8_rdcost_offset;
        reg_data.bits.vctrl_norm_intra_cu16_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_intra_cu16_rdcost_offset;
        reg_data.bits.vctrl_strmov_intra_cu4_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_intra_cu4_rdcost_offset;
        reg_data.bits.vctrl_strmov_intra_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_intra_cu8_rdcost_offset;
        reg_data.bits.vctrl_strmov_intra_cu16_rdcost_offset = \
            dynamic_cfg->img_improve_cfg_dynamic.strmov_intra_cu16_rdcost_offset;
        all_ddr->vedu_vctrl_intra_rdo_factor_0.data = reg_data.data;
    }
    {
        u_vedu_vctrl_intra_rdo_factor_1 reg_data;
        reg_data.bits.vctrl_skin_intra_cu32_rdcost_offset = 0;
        reg_data.bits.vctrl_skin_intra_cu16_rdcost_offset = 0;
        reg_data.bits.vctrl_skin_intra_cu8_rdcost_offset = 0;
        reg_data.bits.vctrl_skin_intra_cu4_rdcost_offset = 0;
        reg_data.bits.vctrl_sobel_str_intra_cu32_rdcost_offset = 0xa;
        reg_data.bits.vctrl_sobel_str_intra_cu16_rdcost_offset = 0xa;
        reg_data.bits.vctrl_sobel_str_intra_cu8_rdcost_offset = 0;
        reg_data.bits.vctrl_sobel_str_intra_cu4_rdcost_offset = 0;
        all_ddr->vedu_vctrl_intra_rdo_factor_1.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_38(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_dynamic_cfg *dynamic_cfg,
                              venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_intra_rdo_factor_2 reg_data;
        reg_data.bits.vctrl_hedge_intra_cu4_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_intra_cu4_rdcost_offset;
        reg_data.bits.vctrl_hedge_intra_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_intra_cu8_rdcost_offset;
        reg_data.bits.vctrl_hedge_intra_cu16_rdcost_offset = \
            dynamic_cfg->img_improve_cfg_dynamic.hedge_intra_cu16_rdcost_offset;
        reg_data.bits.vctrl_sobel_tex_intra_cu4_rdcost_offset = 0; /* sobel_tex_intra_cu4_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_intra_cu8_rdcost_offset = 0; /* sobel_tex_intra_cu8_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_intra_cu16_rdcost_offset = 1; /* sobel_tex_intra_cu16_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_intra_cu32_rdcost_offset = 1;
        all_ddr->vedu_vctrl_intra_rdo_factor_2.data = reg_data.data;
    }
    {
        u_vedu_intra_rdo_cost_offset_0 reg_data;
        reg_data.bits.intra_cu16_rdo_cost_offset = 0;
        reg_data.bits.intra_cu32_rdo_cost_offset = 0;
        all_ddr->vedu_intra_rdo_cost_offset_0.data = reg_data.data;
    }
    {
        u_vedu_intra_rdo_cost_offset_1 reg_data;
        reg_data.bits.intra_cu4_rdo_cost_offset = 0;
        reg_data.bits.intra_cu8_rdo_cost_offset = 0;
        all_ddr->vedu_intra_rdo_cost_offset_1.data = reg_data.data;
    }
    {
        u_vedu_intra_no_dc_cost_offset_0 reg_data;
        reg_data.bits.intra_cu16_non_dc_mode_offset = 0;
        reg_data.bits.intra_cu32_non_dc_mode_offset = 0;
        all_ddr->vedu_intra_no_dc_cost_offset_0.data = reg_data.data;
    }
    {
        u_vedu_intra_no_dc_cost_offset_1 reg_data;
        reg_data.bits.intra_cu8_non_dc_mode_offset = 0;
        reg_data.bits.intra_cu4_non_dc_mode_offset = 0;
        all_ddr->vedu_intra_no_dc_cost_offset_1.data = reg_data.data;
    }
    {
        u_vedu_pme_skip_large_res reg_data;
        reg_data.bits.pme_skip_sad_thr_offset = static_cfg->pme_cfg_static.pme_skip_sad_thr_offset;
        reg_data.bits.pme_skip_sad_thr_gain  = static_cfg->pme_cfg_static.pme_skip_sad_thr_gain;
        reg_data.bits.pme_skip_large_res_det = static_cfg->pme_cfg_static.pme_skip_large_res_det;
        all_ddr->vedu_pme_skip_large_res.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_39(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_mrg_adj_weight reg_data;
        reg_data.bits.cu8_fz_weight = 0;
        reg_data.bits.cu16_fz_weight = 0;
        reg_data.bits.cu32_fz_weight = 0;
        reg_data.bits.cu64_fz_weight = 0;
        reg_data.bits.cu8_fz_adapt_weight  = 0x3;
        reg_data.bits.cu16_fz_adapt_weight = 0x3;
        reg_data.bits.cu32_fz_adapt_weight = 0x3;
        reg_data.bits.cu64_fz_adapt_weight = 0x3;
        all_ddr->vedu_mrg_adj_weight.data = reg_data.data;
    }
    {
        u_vedu_qpg_res_coef reg_data;
        reg_data.bits.vcpi_small_res_coef = static_cfg->qpg_cfg_static.vcpi_small_res_coef;
        reg_data.bits.vcpi_large_res_coef = static_cfg->qpg_cfg_static.vcpi_large_res_coef;
        reg_data.bits.vcpi_res_coef_en = static_cfg->qpg_cfg_static.vcpi_res_coef_en;
        all_ddr->vedu_qpg_res_coef.data  = reg_data.data;
    }
    {
        u_vedu_pme_bias_cost0 reg_data;
        reg_data.bits.fme_pu8_bias_cost  = 0;
        reg_data.bits.fme_pu16_bias_cost = 0;
        all_ddr->vedu_fme_bias_cost0.data = reg_data.data;
    }
    {
        u_vedu_pme_bias_cost1 reg_data;
        reg_data.bits.fme_pu32_bias_cost = 0;
        reg_data.bits.fme_pu64_bias_cost = 0;
        all_ddr->vedu_fme_bias_cost1.data = reg_data.data;
    }
    {
        u_vedu_mrg_bias_cost0 reg_data;
        reg_data.bits.mrg_pu8_bias_cost  = 0;
        reg_data.bits.mrg_pu16_bias_cost = 0;
        all_ddr->vedu_mrg_bias_cost0.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_40(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_dynamic_cfg *dynamic_cfg,
                              venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_mrg_bias_cost1 reg_data;
        reg_data.bits.mrg_pu32_bias_cost = 0;
        reg_data.bits.mrg_pu64_bias_cost = 0;
        all_ddr->vedu_mrg_bias_cost1.data = reg_data.data;
    }
    {
        u_vedu_mrg_abs_offset0 reg_data;
        reg_data.bits.mrg_pu8_abs_offset  = 0;
        reg_data.bits.mrg_pu16_abs_offset = 0;
        all_ddr->vedu_mrg_abs_offset0.data = reg_data.data;
    }
    {
        u_vedu_mrg_abs_offset1 reg_data;
        reg_data.bits.mrg_pu32_abs_offset = 0;
        reg_data.bits.mrg_pu64_abs_offset = 0;
        all_ddr->vedu_mrg_abs_offset1.data = reg_data.data;
    }
    {
        u_vedu_vctrl_mrg_rdo_factor_0 reg_data;
        reg_data.bits.vctrl_norm_mrg_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_mrg_cu64_rdcost_offset;
        reg_data.bits.vctrl_norm_mrg_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_mrg_cu32_rdcost_offset;
        reg_data.bits.vctrl_norm_mrg_cu16_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_mrg_cu16_rdcost_offset;
        reg_data.bits.vctrl_norm_mrg_cu8_rdcost_offset = static_cfg->img_improve_cfg_static.norm_mrg_cu8_rdcost_offset;
        reg_data.bits.vctrl_strmov_mrg_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_mrg_cu64_rdcost_offset;
        reg_data.bits.vctrl_strmov_mrg_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_mrg_cu32_rdcost_offset;
        reg_data.bits.vctrl_strmov_mrg_cu16_rdcost_offset = \
            dynamic_cfg->img_improve_cfg_dynamic.strmov_mrg_cu16_rdcost_offset;
        reg_data.bits.vctrl_strmov_mrg_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_mrg_cu8_rdcost_offset;
        all_ddr->vedu_vctrl_mrg_rdo_factor_0.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_41(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_dynamic_cfg *dynamic_cfg,
                              venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_mrg_rdo_factor_1 reg_data;
        reg_data.bits.vctrl_skin_mrg_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.skin_mrg_cu64_rdcost_offset;
        reg_data.bits.vctrl_skin_mrg_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.skin_mrg_cu32_rdcost_offset;
        reg_data.bits.vctrl_skin_mrg_cu16_rdcost_offset = \
            dynamic_cfg->img_improve_cfg_dynamic.skin_mrg_cu16_rdcost_offset;
        reg_data.bits.vctrl_skin_mrg_cu8_rdcost_offset = static_cfg->img_improve_cfg_static.skin_mrg_cu8_rdcost_offset;
        reg_data.bits.vctrl_sobel_str_mrg_cu64_rdcost_offset = 0xa; /* sobel_str_mrg_cu64_rdcost_offset */
        reg_data.bits.vctrl_sobel_str_mrg_cu32_rdcost_offset = 0xa; /* sobel_str_mrg_cu32_rdcost_offset */
        reg_data.bits.vctrl_sobel_str_mrg_cu16_rdcost_offset = 0xa; /* sobel_str_mrg_cu16_rdcost_offset */
        reg_data.bits.vctrl_sobel_str_mrg_cu8_rdcost_offset = 0; /* sobel_str_mrg_cu8_rdcost_offset */
        all_ddr->vedu_vctrl_mrg_rdo_factor_1.data = reg_data.data;
    }
    {
        u_vedu_vctrl_mrg_rdo_factor_2 reg_data;
        reg_data.bits.vctrl_hedge_mrg_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_mrg_cu64_rdcost_offset;
        reg_data.bits.vctrl_hedge_mrg_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_mrg_cu32_rdcost_offset;
        reg_data.bits.vctrl_hedge_mrg_cu16_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_mrg_cu16_rdcost_offset;
        reg_data.bits.vctrl_hedge_mrg_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_mrg_cu8_rdcost_offset;
        reg_data.bits.vctrl_sobel_tex_mrg_cu64_rdcost_offset = 1; /* sobel_tex_mrg_cu64_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_mrg_cu32_rdcost_offset = 1; /* sobel_tex_mrg_cu32_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_mrg_cu16_rdcost_offset = 1; /* sobel_tex_mrg_cu16_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_mrg_cu8_rdcost_offset = 0; /* sobel_tex_mrg_cu8_rdcost_offset */
        all_ddr->vedu_vctrl_mrg_rdo_factor_2.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_42(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_dynamic_cfg *dynamic_cfg,
                              venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_fme_rdo_factor_0 reg_data;
        reg_data.bits.vctrl_norm_fme_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_fme_cu64_rdcost_offset;
        reg_data.bits.vctrl_norm_fme_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_fme_cu32_rdcost_offset;
        reg_data.bits.vctrl_norm_fme_cu16_rdcost_offset = \
            static_cfg->img_improve_cfg_static.norm_fme_cu16_rdcost_offset;
        reg_data.bits.vctrl_norm_fme_cu8_rdcost_offset = static_cfg->img_improve_cfg_static.norm_fme_cu8_rdcost_offset;
        reg_data.bits.vctrl_strmov_fme_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_fme_cu64_rdcost_offset;
        reg_data.bits.vctrl_strmov_fme_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_fme_cu32_rdcost_offset;
        reg_data.bits.vctrl_strmov_fme_cu16_rdcost_offset = \
            dynamic_cfg->img_improve_cfg_dynamic.strmov_fme_cu16_rdcost_offset;
        reg_data.bits.vctrl_strmov_fme_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.strmov_fme_cu8_rdcost_offset;
        all_ddr->vedu_vctrl_fme_rdo_factor_0.data = reg_data.data;
    }
    {
        u_vedu_vctrl_fme_rdo_factor_1 reg_data;
        reg_data.bits.vctrl_skin_fme_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.skin_fme_cu64_rdcost_offset;
        reg_data.bits.vctrl_skin_fme_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.skin_fme_cu32_rdcost_offset;
        reg_data.bits.vctrl_skin_fme_cu16_rdcost_offset = \
            dynamic_cfg->img_improve_cfg_dynamic.skin_fme_cu16_rdcost_offset;
        reg_data.bits.vctrl_skin_fme_cu8_rdcost_offset = static_cfg->img_improve_cfg_static.skin_fme_cu8_rdcost_offset;
        reg_data.bits.vctrl_sobel_str_fme_cu64_rdcost_offset = 0xa; /* sobel_str_fme_cu64_rdcost_offset */
        reg_data.bits.vctrl_sobel_str_fme_cu32_rdcost_offset = 0xa; /* sobel_str_fme_cu32_rdcost_offset */
        reg_data.bits.vctrl_sobel_str_fme_cu16_rdcost_offset = 0xa; /* sobel_str_fme_cu16_rdcost_offset */
        reg_data.bits.vctrl_sobel_str_fme_cu8_rdcost_offset  = 0; /* sobel_str_fme_cu8_rdcost_offset */
        all_ddr->vedu_vctrl_fme_rdo_factor_1.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_43(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vctrl_fme_rdo_factor_2 reg_data;
        reg_data.bits.vctrl_hedge_fme_cu64_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_fme_cu64_rdcost_offset;
        reg_data.bits.vctrl_hedge_fme_cu32_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_fme_cu32_rdcost_offset;
        reg_data.bits.vctrl_hedge_fme_cu16_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_fme_cu16_rdcost_offset;
        reg_data.bits.vctrl_hedge_fme_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.hedge_fme_cu8_rdcost_offset;
        reg_data.bits.vctrl_sobel_tex_fme_cu64_rdcost_offset = 1; /* sobel_tex_fme_cu64_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_fme_cu32_rdcost_offset = 1; /* sobel_tex_fme_cu32_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_fme_cu16_rdcost_offset = 1; /* sobel_tex_fme_cu16_rdcost_offset */
        reg_data.bits.vctrl_sobel_tex_fme_cu8_rdcost_offset = \
            static_cfg->img_improve_cfg_static.sobel_tex_fme_cu8_rdcost_offset;
        all_ddr->vedu_vctrl_fme_rdo_factor_2.data = reg_data.data;
    }
    {
        u_vedu_ime_rdocfg reg_data;
        reg_data.bits.ime_lambdaoff16 = static_cfg->base_cfg.vcpi_lambdaoff16;
        reg_data.bits.ime_lambdaoff8 = static_cfg->base_cfg.vcpi_lambdaoff8;
        all_ddr->vedu_ime_rdocfg.data = reg_data.data;
    }
    {
        u_vedu_vcpi_pintra_thresh0 reg_data;
        reg_data.bits.vcpi_pintra_pu16_amp_th = 0x80;
        reg_data.bits.vcpi_pintra_pu32_amp_th = 0x80;

        all_ddr->vedu_vcpi_pintra_thresh0.data = reg_data.data;
    }
    {
        u_vedu_vcpi_pintra_thresh1 reg_data;
        reg_data.bits.vcpi_pintra_pu32_std_th = 0x80;

        all_ddr->vedu_vcpi_pintra_thresh1.data = reg_data.data;
    }
    {
        u_vedu_vcpi_pintra_thresh2 reg_data;
        reg_data.bits.vcpi_rpintra_pu4_strong_edge_th = 0;
        reg_data.bits.vcpi_rpintra_pu4_mode_distance_th = 0;
        reg_data.bits.vcpi_rpintra_bypass = 0;
        all_ddr->vedu_vcpi_pintra_thresh2.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_44(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_dynamic_cfg *dynamic_cfg,
                              venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_pme_skin_thr reg_data;
        reg_data.bits.pme_skin_v_min_thr = static_cfg->pme_cfg_static.skin_v_min_thr;
        reg_data.bits.pme_skin_v_max_thr = static_cfg->pme_cfg_static.skin_v_max_thr;
        reg_data.bits.pme_skin_u_min_thr = static_cfg->pme_cfg_static.skin_u_min_thr;
        reg_data.bits.pme_skin_u_max_thr = static_cfg->pme_cfg_static.skin_u_max_thr;
        all_ddr->vedu_pme_skin_thr.data = reg_data.data;
    }
    {
        u_vedu_pme_strong_edge reg_data;
        reg_data.bits.pme_still_scene_thr = dynamic_cfg->pme_cfg_dynamic.still_scene_thr;
        reg_data.bits.pme_strong_edge_cnt = dynamic_cfg->pme_cfg_dynamic.high_edge_cnt;
        reg_data.bits.pme_strong_edge_thr = dynamic_cfg->pme_cfg_dynamic.high_edge_thr;
        reg_data.bits.pme_skin_num = dynamic_cfg->pme_cfg_dynamic.skin_num;
        all_ddr->vedu_pme_strong_edge.data = reg_data.data;
    }
    {
        u_vedu_pme_skin_sad_thr reg_data;
        reg_data.bits.vcpi_skin_sad_thr_offset = static_cfg->pme_cfg_static.vcpi_skin_sad_thr_offset;
        reg_data.bits.vcpi_skin_sad_thr_gain   = static_cfg->pme_cfg_static.vcpi_skin_sad_thr_gain;
        all_ddr->vedu_pme_skin_sad_thr.data = reg_data.data;
    }
    {
        u_vedu_qpg_skin reg_data;
        reg_data.bits.qpg_skin_max_qp = dynamic_cfg->qpg_cfg_dynamic.skin_max_qp;
        reg_data.bits.qpg_skin_qp_delta = dynamic_cfg->qpg_cfg_dynamic.skin_qp_delta;
        reg_data.bits.qpg_skin_min_qp = static_cfg->qpg_cfg_static.skin_min_qp;
        all_ddr->vedu_qpg_skin.data = reg_data.data;
    }
    {
        u_vedu_pme_inter_strong_edge reg_data;
        reg_data.bits.pme_interstrongedge_madi_thr = dynamic_cfg->pme_cfg_dynamic.interstrongedge_madi_thr;
        reg_data.bits.pme_interdiff_max_min_madi_times = dynamic_cfg->pme_cfg_dynamic.interdiff_max_min_madi_times;
        reg_data.bits.pme_interdiff_max_min_madi_abs = dynamic_cfg->pme_cfg_dynamic.interdiff_max_min_madi_abs;
        all_ddr->vedu_pme_inter_strong_edge.data = reg_data.data;
    }
    {
        u_vedu_qpg_hedge reg_data;
        reg_data.bits.qpg_hedge_max_qp = static_cfg->qpg_cfg_static.strong_edge_max_qp;
        reg_data.bits.qpg_hedge_qp_delta = static_cfg->qpg_cfg_static.strong_edge_qp_delta;
        reg_data.bits.qpg_hedge_min_qp = static_cfg->qpg_cfg_static.stredge_min_qp;
        all_ddr->vedu_qpg_hedge.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_45(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_dynamic_cfg *dynamic_cfg,
                              venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_pme_large_move_thr reg_data;
        reg_data.bits.pme_move_sad_thr = dynamic_cfg->pme_cfg_dynamic.move_sad_thr;
        reg_data.bits.pme_move_scene_thr = static_cfg->pme_cfg_static.move_scene_thr;
        all_ddr->vedu_pme_large_move_thr.data = reg_data.data;
    }
    {
        u_vedu_qpg_hedge_move reg_data;
        reg_data.bits.qpg_hedge_move_max_qp = static_cfg->qpg_cfg_static.strong_edge_move_max_qp;
        reg_data.bits.qpg_hedge_move_qp_delta = static_cfg->qpg_cfg_static.strong_edge_move_qp_delta;
        reg_data.bits.qpg_hedge_move_min_qp = static_cfg->qpg_cfg_static.stredge_move_min_qp;
        all_ddr->vedu_qpg_hedge_move.data = reg_data.data;
    }
    {
        u_vedu_pme_move_scene_thr reg_data;
        reg_data.bits.vcpi_move_scene_mv_thr = static_cfg->pme_cfg_static.vcpi_move_scene_mv_thr;
        reg_data.bits.vcpi_move_scene_mv_en  = static_cfg->pme_cfg_static.vcpi_move_scene_mv_en;
        all_ddr->vedu_pme_move_scene_thr.data = reg_data.data;
    }
    {
        u_vedu_pme_low_luma_thr reg_data;
        reg_data.bits.pme_low_luma_thr = static_cfg->pme_cfg_static.low_luma_thr;
        reg_data.bits.pme_low_luma_madi_thr = static_cfg->pme_cfg_static.low_luma_madi_thr;
        reg_data.bits.pme_high_luma_thr = static_cfg->pme_cfg_static.pme_high_luma_thr;
        all_ddr->vedu_pme_low_luma_thr.data = reg_data.data;
    }
    {
        u_vedu_qpg_lowluma reg_data;
        reg_data.bits.qpg_lowluma_max_qp = static_cfg->qpg_cfg_static.lowluma_max_qp;
        reg_data.bits.qpg_lowluma_qp_delta = static_cfg->qpg_cfg_static.lowluma_qp_delta;
        reg_data.bits.qpg_lowluma_min_qp = static_cfg->qpg_cfg_static.lowluma_min_qp;
        all_ddr->vedu_qpg_lowluma.data = reg_data.data;
    }
    {
        u_vedu_pme_chroma_flat reg_data;
        reg_data.bits.pme_flat_u_thr_low = 125; /* 125 static_cfg->pme_cfg_static.pme_flat_u_thr_low */
        reg_data.bits.pme_flat_u_thr_high = 133; /* 133 static_cfg->pme_cfg_static.pme_flat_u_thr_high */
        reg_data.bits.pme_flat_v_thr_low = 123; /* 123 static_cfg->pme_cfg_static.pme_flat_v_thr_low */
        reg_data.bits.pme_flat_v_thr_high = 133; /*  133 static_cfg->pme_cfg_static.pme_flat_v_thr_high */
        all_ddr->vedu_pme_chroma_flat.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_46(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_pme_luma_flat reg_data;
        reg_data.bits.pme_flat_high_luma_thr = 0x64 ; /* static_cfg->pme_cfg_static.pme_flat_high_luma_thr */
        reg_data.bits.pme_flat_low_luma_thr = 40; /* 40 static_cfg->pme_cfg_static.pme_flat_low_luma_thr */
        reg_data.bits.pme_flat_luma_madi_thr = 0x2; /* static_cfg->pme_cfg_static.pme_flat_luma_madi_thr */
        reg_data.bits.pme_flat_pmemv_thr = 1; /* static_cfg->pme_cfg_static.pme_flat_pmemv_thr */
        all_ddr->vedu_pme_luma_flat.data = reg_data.data;
    }
    {
        u_vedu_pme_madi_flat reg_data;
        reg_data.bits.pme_flat_madi_times = 1; /* static_cfg->pme_cfg_static.pme_flat_madi_times */
        reg_data.bits.pme_flat_region_cnt = 0xd; /* static_cfg->pme_cfg_static.pme_flat_region_cnt */
        reg_data.bits.pme_flat_icount_thr = 200; /* 200 static_cfg->pme_cfg_static.pme_flat_icount_thr */
        reg_data.bits.pme_flat_pmesad_thr = 0x40; /* static_cfg->pme_cfg_static.pme_flat_pmesad_thr */
        all_ddr->vedu_pme_madi_flat.data = reg_data.data;
    }
    {
        u_vedu_qpg_flat_region reg_data;
        reg_data.bits.qpg_flat_region_qp_delta = static_cfg->qpg_cfg_static.flat_region_qp_delta;
        reg_data.bits.qpg_flat_region_max_qp = static_cfg->qpg_cfg_static.flat_region_max_qp;
        reg_data.bits.qpg_flat_region_min_qp = static_cfg->qpg_cfg_static.flat_region_min_qp;
        reg_data.bits.vcpi_cu32_use_cu16_mean_en = 0;
        all_ddr->vedu_qpg_flat_region.data = reg_data.data;
    }
    {
        u_vedu_chroma_protect reg_data;
        reg_data.bits.vcpi_chroma_qp_delta = static_cfg->qpg_cfg_static.vcpi_chroma_qp_delta;
        reg_data.bits.vcpi_chroma_min_qp = static_cfg->qpg_cfg_static.vcpi_chroma_min_qp;
        reg_data.bits.vcpi_chroma_max_qp = static_cfg->qpg_cfg_static.vcpi_chroma_max_qp;
        reg_data.bits.vcpi_chroma_prot_en = static_cfg->qpg_cfg_static.vcpi_chroma_prot_en;
        all_ddr->vedu_chroma_protect.data = reg_data.data;
    }
    {
        u_vedu_pme_chroma_strong_edge reg_data;
        reg_data.bits.vcpi_strong_edge_thr_u = static_cfg->pme_cfg_static.vcpi_strong_edge_thr_u;
        reg_data.bits.vcpi_strong_edge_cnt_u = static_cfg->pme_cfg_static.vcpi_strong_edge_cnt_u;
        reg_data.bits.vcpi_strong_edge_thr_v = static_cfg->pme_cfg_static.vcpi_strong_edge_thr_v;
        reg_data.bits.vcpi_strong_edge_cnt_v = static_cfg->pme_cfg_static.vcpi_strong_edge_cnt_v;
        all_ddr->vedu_pme_chroma_strong_edge.data = reg_data.data;
    }
    {
        u_vedu_chroma_sad_thr reg_data;
        reg_data.bits.vcpi_chroma_sad_thr_offset = static_cfg->pme_cfg_static.vcpi_chroma_sad_thr_offset;
        reg_data.bits.vcpi_chroma_sad_thr_gain = static_cfg->pme_cfg_static.vcpi_chroma_sad_thr_gain;
        all_ddr->vedu_chroma_sad_thr.data = reg_data.data;
    }

}

static hi_void ddr_cfg_reg_47(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_chroma_fg_thr  reg_data;
        reg_data.bits.vcpi_chroma_u0_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_u0_thr_min;
        reg_data.bits.vcpi_chroma_u0_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_u0_thr_max;
        reg_data.bits.vcpi_chroma_v0_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_v0_thr_min;
        reg_data.bits.vcpi_chroma_v0_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_v0_thr_max;
        all_ddr->vedu_chroma_fg_thr.data = reg_data.data;
    }
    {
        u_vedu_chroma_bg_thr reg_data;
        reg_data.bits.vcpi_chroma_u1_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_u1_thr_min;
        reg_data.bits.vcpi_chroma_u1_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_u1_thr_max;
        reg_data.bits.vcpi_chroma_v1_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_v1_thr_min;
        reg_data.bits.vcpi_chroma_v1_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_v1_thr_max;
        all_ddr->vedu_chroma_bg_thr.data = reg_data.data;
    }
    {
        u_vedu_chroma_sum_fg_thr reg_data;
        reg_data.bits.vcpi_chroma_uv0_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_uv0_thr_min;
        reg_data.bits.vcpi_chroma_uv0_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_uv0_thr_max;
        all_ddr->vedu_chroma_sum_fg_thr.data = reg_data.data;
    }
    {
        u_vedu_chroma_sum_bg_thr reg_data;
        reg_data.bits.vcpi_chroma_uv1_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_uv1_thr_min;
        reg_data.bits.vcpi_chroma_uv1_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_uv1_thr_max;
        all_ddr->vedu_chroma_sum_bg_thr.data = reg_data.data;
    }
    {
        u_vedu_chroma_fg_count_thr reg_data;
        reg_data.bits.vcpi_chroma_count0_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_count0_thr_min;
        reg_data.bits.vcpi_chroma_count0_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_count0_thr_max;
        all_ddr->vedu_chroma_fg_count_thr.data = reg_data.data;
    }
    {
        u_vedu_chroma_bg_count_thr reg_data;
        reg_data.bits.vcpi_chroma_count1_thr_min = static_cfg->pme_cfg_static.vcpi_chroma_count1_thr_min;
        reg_data.bits.vcpi_chroma_count1_thr_max = static_cfg->pme_cfg_static.vcpi_chroma_count1_thr_max;
        all_ddr->vedu_chroma_bg_count_thr.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_48(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_pme_intrablk_det reg_data;
        reg_data.bits.pme_intrablk_det_cost_thr0 = static_cfg->pme_cfg_static.pme_intrablk_det_cost_thr0;
        reg_data.bits.pme_pskip_mvy_consistency_thr = static_cfg->pme_cfg_static.pskip_mvy_consistency_thr;
        reg_data.bits.pme_pskip_mvx_consistency_thr = static_cfg->pme_cfg_static.pskip_mvx_consistency_thr;
        all_ddr->vedu_pme_intrablk_det.data = reg_data.data;
    }
    {
        u_vedu_pme_intrablk_det_thr reg_data;
        reg_data.bits.pme_intrablk_det_mv_dif_thr1  = static_cfg->pme_cfg_static.pme_intrablk_det_mv_dif_thr1;
        reg_data.bits.pme_intrablk_det_mv_dif_thr0  = static_cfg->pme_cfg_static.pme_intrablk_det_mv_dif_thr0;
        reg_data.bits.pme_intrablk_det_mvy_thr = static_cfg->pme_cfg_static.pme_intrablk_det_mvy_thr;
        reg_data.bits.pme_intrablk_det_mvx_thr = static_cfg->pme_cfg_static.pme_intrablk_det_mvx_thr;
        all_ddr->vedu_pme_intrablk_det_thr.data = reg_data.data;
    }
    {
        u_vedu_qpg_intra_det reg_data;
        reg_data.bits.qpg_intra_det_qp_delta = static_cfg->qpg_cfg_static.intra_det_qp_delta;
        reg_data.bits.qpg_intra_det_max_qp = static_cfg->qpg_cfg_static.intra_det_max_qp;
        reg_data.bits.qpg_intra_det_min_qp = static_cfg->qpg_cfg_static.intra_det_min_qp;
        all_ddr->vedu_qpg_intra_det.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_49(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_dynamic_cfg *dynamic_cfg)
{
    hi_s32 i;

    if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
        if (reg_cfg->rc_cfg.avbr && !reg_cfg->rc_cfg.intra_pic) {
            /* avbr p increase lamda */
            for (i = 0; i < 40; i++) {  /* 40: array size */
                u_vedu_qpg_qp_lambda_ctrl_reg00 reg_data;
                reg_data.bits.qpg_lambda00 = g_h265_lambda_ssd[i + 3]; /* 3: array offset */
                all_ddr->vedu_qpg_qp_lambda_ctrl_reg00[i].data = reg_data.data;
            }
            for (i = 0; i < 40; i++) { /* 40: array size */
                u_vedu_qpg_qp_lambda_ctrl_reg00 reg_data;
                reg_data.bits.qpg_lambda00 = g_h265_lambda_sad[i + 3]; /* 3: array offset */
                all_ddr->vedu_qpg_qp_lambda_ctrl_reg00[i + 40].data = reg_data.data; /* 40: array offset */
            }
        } else {
            for (i = 0; i < 80; i++) { /* 80: array size */
                u_vedu_qpg_qp_lambda_ctrl_reg00 reg_data;
                reg_data.bits.qpg_lambda00 = dynamic_cfg->qpg_cfg_dynamic.lambda[i];
                all_ddr->vedu_qpg_qp_lambda_ctrl_reg00[i].data = reg_data.data;
            }
        }
    } else {
        for (i = 0; i < 80; i++) { /* 80: array size */
            u_vedu_qpg_qp_lambda_ctrl_reg00 reg_data;
            reg_data.bits.qpg_lambda00 = dynamic_cfg->qpg_cfg_dynamic.lambda[i];
            all_ddr->vedu_qpg_qp_lambda_ctrl_reg00[i].data = reg_data.data;
        }
    }

    {
        u_vedu_sao_ssd_area0_start reg_data;
        reg_data.bits.sao_area0_start_lcux = 0;
        reg_data.bits.sao_area0_start_lcuy = 0;
        all_ddr->vedu_sao_ssd_area0_start.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area0_end reg_data;
        reg_data.bits.sao_area0_end_lcux = 0;
        reg_data.bits.sao_area0_end_lcuy = 0;
        all_ddr->vedu_sao_ssd_area0_end.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area1_start reg_data;
        reg_data.bits.sao_area1_start_lcux = 0;
        reg_data.bits.sao_area1_start_lcuy = 0;
        all_ddr->vedu_sao_ssd_area1_start.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_50(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    {
        u_vedu_sao_ssd_area1_end reg_data;
        reg_data.bits.sao_area1_end_lcux = 0;
        reg_data.bits.sao_area1_end_lcuy = 0;
        all_ddr->vedu_sao_ssd_area1_end.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area2_start reg_data;
        reg_data.bits.sao_area2_start_lcux = 0;
        reg_data.bits.sao_area2_start_lcuy = 0;
        all_ddr->vedu_sao_ssd_area2_start.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area2_end reg_data;
        reg_data.bits.sao_area2_end_lcux = 0;
        reg_data.bits.sao_area2_end_lcuy = 0;
        all_ddr->vedu_sao_ssd_area2_end.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area3_start reg_data;
        reg_data.bits.sao_area3_start_lcux = 0;
        reg_data.bits.sao_area3_start_lcuy = 0;
        all_ddr->vedu_sao_ssd_area3_start.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area3_end reg_data;
        reg_data.bits.sao_area3_end_lcux = 0;
        reg_data.bits.sao_area3_end_lcuy = 0;
        all_ddr->vedu_sao_ssd_area3_end.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area4_start reg_data;
        reg_data.bits.sao_area4_start_lcux = 0;
        reg_data.bits.sao_area4_start_lcuy = 0;
        all_ddr->vedu_sao_ssd_area4_start.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_51(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    {
        u_vedu_sao_ssd_area4_end reg_data;
        reg_data.bits.sao_area4_end_lcux = 0;
        reg_data.bits.sao_area4_end_lcuy = 0;
        all_ddr->vedu_sao_ssd_area4_end.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area5_start reg_data;
        reg_data.bits.sao_area5_start_lcux = 0;
        reg_data.bits.sao_area5_start_lcuy = 0;
        all_ddr->vedu_sao_ssd_area5_start.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area5_end reg_data;
        reg_data.bits.sao_area5_end_lcux = 0;
        reg_data.bits.sao_area5_end_lcuy = 0;
        all_ddr->vedu_sao_ssd_area5_end.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area6_start reg_data;
        reg_data.bits.sao_area6_start_lcux = 0;
        reg_data.bits.sao_area6_start_lcuy = 0;
        all_ddr->vedu_sao_ssd_area6_start.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area6_end reg_data;
        reg_data.bits.sao_area6_end_lcux = 0;
        reg_data.bits.sao_area6_end_lcuy = 0;
        all_ddr->vedu_sao_ssd_area6_end.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area7_start reg_data;
        reg_data.bits.sao_area7_start_lcux = 0;
        reg_data.bits.sao_area7_start_lcuy = 0;
        all_ddr->vedu_sao_ssd_area7_start.data = reg_data.data;
    }
    {
        u_vedu_sao_ssd_area7_end reg_data;
        reg_data.bits.sao_area7_end_lcux = 0;
        reg_data.bits.sao_area7_end_lcuy = 0;
        all_ddr->vedu_sao_ssd_area7_end.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_52(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_dynamic_cfg *dynamic_cfg,
                              venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vcpi_pic_strong_en reg_data;
        reg_data.bits.vcpi_still_en = dynamic_cfg->img_improve_cfg_dynamic.static_en;
        reg_data.bits.vcpi_strong_edge_en = dynamic_cfg->img_improve_cfg_dynamic.highedge_en;
        reg_data.bits.vcpi_skin_en = static_cfg->img_improve_cfg_static.skin_en;
        reg_data.bits.vcpi_skin_close_angle = static_cfg->base_cfg.vcpi_skin_close_angle;
        reg_data.bits.vcpi_rounding_sobel_en = static_cfg->base_cfg.vcpi_rounding_sobel_en;
        all_ddr->vedu_vcpi_pic_strong_en.data = reg_data.data;
    }
    {
        u_vedu_vctrl_nm_acoffset_denoise reg_data;
        reg_data.bits.vctrl_chos_oth_blk_spec_offset = 0;
        reg_data.bits.vctrl_chos_oth_blk_offset16 = 0;
        reg_data.bits.vctrl_i_chos_cur_blk_offset16 = 0;
        reg_data.bits.vctrl_low_freq_ac_blk16 = 0;
        reg_data.bits.vctrl_chos_oth_blk_offset32 = 0;
        reg_data.bits.vctrl_i_chos_cur_blk_offset32 = 0;
        reg_data.bits.vctrl_low_freq_ac_blk32 = 0;
        all_ddr->vedu_vctrl_nm_acoffset_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_nm_engthr_denoise reg_data;
        reg_data.bits.vctrl_chos_oth_blk_spec_thr = 0;
        reg_data.bits.vctrl_chos_oth_blk_eng_thr = 0;
        reg_data.bits.vctrl_chos_oth_blk_thr3 = 0;
        reg_data.bits.vctrl_chos_oth_blk_thr2 = 0;
        reg_data.bits.vctrl_chos_oth_blk_thr = 0;
        reg_data.bits.vctrl_rm_all_high_ac_thr = 0;
        all_ddr->vedu_vctrl_nm_engthr_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_nm_tu8_denoise reg_data;
        reg_data.bits.vctrl_ring_eng_thr = 0;
        reg_data.bits.vctrl_ring_ac_thr = 0;
        reg_data.bits.vctrl_p_chos_oth_blk_offset = 0;
        reg_data.bits.vctrl_p_chos_oth_blk_offset8 = 0;
        reg_data.bits.vctrl_i_chos_cur_blk_offset8 = 0;
        reg_data.bits.vctrl_i_low_freq_ac_blk8 = 0;
        reg_data.bits.vctrl_blk8_enable_flag = 0;
        reg_data.bits.vctrl_chos_oth_blk_offset_sec16 = 0;
        reg_data.bits.vctrl_chos_oth_blk_offset_sec32 = 0;
        all_ddr->vedu_vctrl_nm_tu8_denoise.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_53(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    {
        u_vedu_vctrl_sk_acoffset_denoise reg_data;
        reg_data.bits.vctrl_skin_chos_oth_blk_spec_offset = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_offset16 = 0;
        reg_data.bits.vctrl_skin_chos_cur_blk_offset16 = 0;
        reg_data.bits.vctrl_skin_low_freq_ac_blk16 = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_offset32 = 0;
        reg_data.bits.vctrl_skin_i_chos_cur_blk_offset32 = 0;
        reg_data.bits.vctrl_skin_low_freq_ac_blk32 = 0;
        all_ddr->vedu_vctrl_sk_acoffset_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_sk_engthr_denoise reg_data;
        reg_data.bits.vctrl_skin_chos_oth_blk_spec_thr = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_eng_thr = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_thr3 = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_thr2 = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_thr = 0;
        reg_data.bits.vctrl_skin_rm_all_high_ac_thr = 0;
        all_ddr->vedu_vctrl_sk_engthr_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_sk_tu8_denoise reg_data;
        reg_data.bits.vctrl_skin_ring_eng_thr = 0;
        reg_data.bits.vctrl_skin_ring_ac_thr = 0;
        reg_data.bits.vctrl_skin_p_chos_oth_blk_offset = 0;
        reg_data.bits.vctrl_skin_p_chos_oth_blk_offset8 = 0;
        reg_data.bits.vctrl_skin_i_chos_cur_blk_offset8 = 0;
        reg_data.bits.vctrl_skin_low_freq_ac_blk8 = 0;
        reg_data.bits.vctrl_skin_blk8_enable_flag = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_offset_sec16 = 0;
        reg_data.bits.vctrl_skin_chos_oth_blk_offset_sec32 = 0;
        all_ddr->vedu_vctrl_sk_tu8_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_st_acoffset_denoise reg_data;
        reg_data.bits.vctrl_still_chos_oth_blk_spec_offset = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_offset16 = 0;
        reg_data.bits.vctrl_still_i_chos_cur_blk_offset16 = 0;
        reg_data.bits.vctrl_still_low_freq_ac_blk16 = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_offset32 = 0;
        reg_data.bits.vctrl_still_i_chos_cur_blk_offset32 = 0;
        reg_data.bits.vctrl_still_low_freq_ac_blk32 = 0;
        all_ddr->vedu_vctrl_st_acoffset_denoise.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_54(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    {
        u_vedu_vctrl_st_engthr_denoise reg_data;
        reg_data.bits.vctrl_still_chos_oth_blk_spec_thr = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_eng_thr = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_thr3 = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_thr2 = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_thr = 0;
        reg_data.bits.vctrl_still_rm_all_high_ac_thr = 0;
        all_ddr->vedu_vctrl_st_engthr_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_st_tu8_denoise reg_data;
        reg_data.bits.vctrl_still_ring_eng_thr = 0;
        reg_data.bits.vctrl_still_ring_ac_thr = 0;
        reg_data.bits.vctrl_still_p_chos_oth_blk_offset = 0;
        reg_data.bits.vctrl_still_p_chos_oth_blk_offset8 = 0;
        reg_data.bits.vctrl_still_i_chos_cur_blk_offset8 = 0;
        reg_data.bits.vctrl_still_low_freq_ac_blk8 = 0;
        reg_data.bits.vctrl_still_blk8_enable_flag = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_offset_sec16 = 0;
        reg_data.bits.vctrl_still_chos_oth_blk_offset_sec32 = 0;
        all_ddr->vedu_vctrl_st_tu8_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_se_acoffset_denoise reg_data;
        reg_data.bits.vctrl_edge_chos_oth_blk_spec_offset = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_offset16 = 0;
        reg_data.bits.vctrl_edge_i_chos_cur_blk_offset16 = 0;
        reg_data.bits.vctrl_edge_low_freq_ac_blk16 = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_offset32 = 0;
        reg_data.bits.vctrl_edge_i_chos_cur_blk_offset32 = 0;
        reg_data.bits.vctrl_edge_low_freq_ac_blk32 = 0;
        all_ddr->vedu_vctrl_se_acoffset_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_se_engthr_denoise reg_data;
        reg_data.bits.vctrl_edge_chos_oth_blk_spec_thr = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_eng_thr = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_thr3 = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_thr2 = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_thr = 0;
        reg_data.bits.vctrl_edge_rm_all_high_ac_thr = 0;
        all_ddr->vedu_vctrl_se_engthr_denoise.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_55(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    {
        u_vedu_vctrl_se_tu8_denoise reg_data;
        reg_data.bits.vctrl_edge_ring_eng_thr = 0;
        reg_data.bits.vctrl_edge_ring_ac_thr = 0;
        reg_data.bits.vctrl_edge_p_chos_oth_blk_offset = 0;
        reg_data.bits.vctrl_edge_p_chos_oth_blk_offset8 = 0;
        reg_data.bits.vctrl_edge_i_chos_cur_blk_offset8 = 0;
        reg_data.bits.vctrl_edge_low_freq_ac_blk8 = 0;
        reg_data.bits.vctrl_edge_blk8_enable_flag = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_offset_sec16 = 0;
        reg_data.bits.vctrl_edge_chos_oth_blk_offset_sec32 = 0;
        all_ddr->vedu_vctrl_se_tu8_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_selm_acoffset_denoise reg_data;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_spec_offset = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_offset16 = 0;
        reg_data.bits.vctrl_edge_and_move_i_chos_cur_blk_offset16 = 0;
        reg_data.bits.vctrl_edge_and_move_low_freq_ac_blk16 = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_offset32 = 0;
        reg_data.bits.vctrl_edge_and_move_i_chos_cur_blk_offset32 = 0;
        reg_data.bits.vctrl_edge_and_move_low_freq_ac_blk32 = 0;
        all_ddr->vedu_vctrl_selm_acoffset_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_selm_engthr_denoise reg_data;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_spec_thr = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_eng_thr = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_thr3 = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_thr2 = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_thr = 0;
        reg_data.bits.vctrl_edge_and_move_rm_all_high_ac_thr = 0;
        all_ddr->vedu_vctrl_selm_engthr_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_selm_tu8_denoise reg_data;
        reg_data.bits.vctrl_edge_and_move_ring_eng_thr = 0;
        reg_data.bits.vctrl_edge_and_move_ring_ac_thr = 0;
        reg_data.bits.vctrl_edge_and_move_p_chos_oth_blk_offset = 0;
        reg_data.bits.vctrl_edge_and_move_p_chos_oth_blk_offset8 = 0;
        reg_data.bits.vctrl_edge_and_move_i_chos_cur_blk_offset8 = 0;
        reg_data.bits.vctrl_edge_and_move_low_freq_ac_blk8 = 0;
        reg_data.bits.vctrl_edge_and_move_blk8_enable_flag = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_offset_sec16 = 0;
        reg_data.bits.vctrl_edge_and_move_chos_oth_blk_offset_sec32 = 0;
        all_ddr->vedu_vctrl_selm_tu8_denoise.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_56(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    {
        u_vedu_vctrl_ws_acoffset_denoise reg_data;
        reg_data.bits.vctrl_weak_chos_oth_blk_spec_offset = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_offset16 = 0;
        reg_data.bits.vctrl_weak_i_chos_cur_blk_offset16 = 0;
        reg_data.bits.vctrl_weak_low_freq_ac_blk16 = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_offset32 = 0;
        reg_data.bits.vctrl_weak_i_chos_cur_blk_offset32 = 0;
        reg_data.bits.vctrl_weak_low_freq_ac_blk32 = 0;
        all_ddr->vedu_vctrl_ws_acoffset_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_ws_engthr_denoise reg_data;
        reg_data.bits.vctrl_weak_chos_oth_blk_spec_thr = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_eng_thr = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_thr3 = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_thr2 = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_thr = 0;
        reg_data.bits.vctrl_weak_rm_all_high_ac_thr = 0;
        all_ddr->vedu_vctrl_ws_engthr_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_ws_tu8_denoise reg_data;
        reg_data.bits.vctrl_weak_ring_eng_thr = 0;
        reg_data.bits.vctrl_weak_ring_ac_thr = 0;
        reg_data.bits.vctrl_weak_p_chos_oth_blk_offset = 0;
        reg_data.bits.vctrl_weak_p_chos_oth_blk_offset8 = 0;
        reg_data.bits.vctrl_weak_i_chos_cur_blk_offset8 = 0;
        reg_data.bits.vctrl_weak_low_freq_ac_blk8 = 0;
        reg_data.bits.vctrl_weak_blk8_enable_flag = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_offset_sec16 = 0;
        reg_data.bits.vctrl_weak_chos_oth_blk_offset_sec32 = 0;
        all_ddr->vedu_vctrl_ws_tu8_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_ssse_acoffset_denoise reg_data;
        reg_data.bits.vctrl_strong_chos_oth_blk_spec_offset = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_offset16 = 0;
        reg_data.bits.vctrl_strong_i_chos_cur_blk_offset16 = 0;
        reg_data.bits.vctrl_strong_low_freq_ac_blk16 = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_offset32 = 0;
        reg_data.bits.vctrl_strong_i_chos_cur_blk_offset32 = 0;
        reg_data.bits.vctrl_strong_low_freq_ac_blk32 = 0;
        all_ddr->vedu_vctrl_ssse_acoffset_denoise.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_57(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    {
        u_vedu_vctrl_ssse_engthr_denoise reg_data;
        reg_data.bits.vctrl_strong_chos_oth_blk_spec_thr = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_eng_thr = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_thr3 = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_thr2 = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_thr = 0;
        reg_data.bits.vctrl_strong_rm_all_high_ac_thr = 0;
        all_ddr->vedu_vctrl_ssse_engthr_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_ssse_tu8_denoise reg_data;
        reg_data.bits.vctrl_strong_ring_eng_thr = 0;
        reg_data.bits.vctrl_strong_ring_ac_thr = 0;
        reg_data.bits.vctrl_strong_p_chos_oth_blk_offset = 0;
        reg_data.bits.vctrl_strong_p_chos_oth_blk_offset8 = 0;
        reg_data.bits.vctrl_strong_i_chos_cur_blk_offset8 = 0;
        reg_data.bits.vctrl_strong_low_freq_ac_blk8 = 0;
        reg_data.bits.vctrl_strong_blk8_enable_flag = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_offset_sec16 = 0;
        reg_data.bits.vctrl_strong_chos_oth_blk_offset_sec32 = 0;
        all_ddr->vedu_vctrl_ssse_tu8_denoise.data = reg_data.data;
    }
    {
        u_vedu_vctrl_pr_inter_offset0 reg_data;
        reg_data.bits.vctrl_normal_inter_thr_offset = 0;
        reg_data.bits.vctrl_skin_inter_thr_offset = 0;
        reg_data.bits.vctrl_still_inter_thr_offset = 0;
        reg_data.bits.vctrl_s_edge_inter_thr_offset = 0;
        reg_data.bits.vctrl_e_move_inter_thr_offset = 0;
        all_ddr->vedu_vctrl_pr_inter_offset0.data = reg_data.data;
    }
    {
        u_vedu_vctrl_pr_inter_offset1 reg_data;
        reg_data.bits.vctrl_week_s_inter_thr_offset = 0;
        reg_data.bits.vctrl_sobel_sse_inter_thr_offset = 0;
        all_ddr->vedu_vctrl_pr_inter_offset1.data = reg_data.data;
    }
    all_ddr->vedu_vcpi_tunlcell_addr_l = reg_cfg->buf_cfg.tunl_cell_addr;
    all_ddr->vedu_vcpi_src_yaddr_l = reg_cfg->buf_cfg.src_y_addr;
    all_ddr->vedu_vcpi_src_caddr_l = reg_cfg->buf_cfg.src_c_addr;
    all_ddr->vedu_vcpi_src_vaddr_l = reg_cfg->buf_cfg.src_v_addr;
    all_ddr->vedu_vcpi_tunlcell_addr_h  = 0x0;
    all_ddr->vedu_vcpi_src_yaddr_h = 0x0;
    all_ddr->vedu_vcpi_src_caddr_h = 0x0;
    all_ddr->vedu_vcpi_src_vaddr_h = 0x0;
}

static hi_void ddr_cfg_reg_58(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    all_ddr->vedu_secure_info_l = 0x08000800;
    all_ddr->vedu_secure_info_h = 0;

    all_ddr->vedu_vlcst_para_addr_l = 0x08000800;
    all_ddr->vedu_vlcst_para_addr_h = 0;
    {
        u_vedu_vcpi_stride reg_data;
        reg_data.bits.vcpi_curld_y_stride = reg_cfg->buf_cfg.s_stride_y;
        reg_data.bits.vcpi_curld_c_stride = reg_cfg->buf_cfg.s_stride_c;
        all_ddr->vedu_vcpi_stride.data = reg_data.data;
    }
    {
        u_vedu_sao_mode reg_data;
        reg_data.bits.sao_bo_mode_off_en = 0;
        reg_data.bits.sao_eo_mode_off_en = 0;
        reg_data.bits.sao_merge_mode_off = 0;
        reg_data.bits.sao_merge_mode_qp = 0;
        all_ddr->vedu_sao_mode.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_59(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    all_ddr->vedu_vcpi_rec_yh_addr_l = 0;
    all_ddr->vedu_vcpi_rec_ch_addr_l = 0;

    all_ddr->vedu_vcpi_refyh_l0_addr_l = 0;
    all_ddr->vedu_vcpi_refch_l0_addr_l = 0;
    all_ddr->vedu_vcpi_refyh_l1_addr_l = 0;
    all_ddr->vedu_vcpi_refch_l1_addr_l = 0;

    all_ddr->vedu_vcpi_rec_yh_addr_h = 0x0;
    all_ddr->vedu_vcpi_rec_ch_addr_h = 0x0;
    all_ddr->vedu_vcpi_refyh_l0_addr_h = 0x0;
    all_ddr->vedu_vcpi_refch_l0_addr_h = 0x0;
    all_ddr->vedu_vcpi_refyh_l1_addr_h = 0x0;
    all_ddr->vedu_vcpi_refch_l1_addr_h = 0x0;

    all_ddr->vedu_vcpi_refy_l0_addr0_h  = 0x0;
    all_ddr->vedu_vcpi_refc_l0_addr0_h  = 0x0;
    all_ddr->vedu_vcpi_refy_l1_addr0_h  = 0x0;
    all_ddr->vedu_vcpi_refc_l1_addr0_h  = 0x0;
    all_ddr->vedu_vcpi_refy_l0_addr1_h  = 0x0;
    all_ddr->vedu_vcpi_refc_l0_addr1_h  = 0x0;
    all_ddr->vedu_vcpi_refy_l1_addr1_h  = 0x0;
    all_ddr->vedu_vcpi_refc_l1_addr1_h  = 0x0;

    all_ddr->vedu_vcpi_rec_yaddr0_h = 0x0;
    all_ddr->vedu_vcpi_rec_caddr0_h = 0x0;
    all_ddr->vedu_vcpi_rec_yaddr1_h = 0x0;
    all_ddr->vedu_vcpi_rec_caddr1_h = 0x0;
}

static hi_void ddr_cfg_reg_60(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    all_ddr->vedu_vcpi_refy_l0_addr0_l = reg_cfg->buf_cfg.vedu_vcpi_refy_l0_addr0_l;
    all_ddr->vedu_vcpi_refc_l0_addr0_l = reg_cfg->buf_cfg.vedu_vcpi_refc_l0_addr0_l;

    all_ddr->vedu_vcpi_refy_l1_addr0_l = reg_cfg->buf_cfg.vedu_vcpi_refy_l1_addr0_l;
    all_ddr->vedu_vcpi_refc_l1_addr0_l = reg_cfg->buf_cfg.vedu_vcpi_refc_l1_addr0_l;

    all_ddr->vedu_vcpi_rec_yaddr0_l = reg_cfg->buf_cfg.vedu_vcpi_rec_yaddr0_l;
    all_ddr->vedu_vcpi_rec_caddr0_l = reg_cfg->buf_cfg.vedu_vcpi_rec_caddr0_l;

#ifdef CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT
    all_ddr->vedu_vcpi_refy_l0_addr1_l = reg_cfg->buf_cfg.vedu_vcpi_refy_l0_addr1_l;
    all_ddr->vedu_vcpi_refc_l0_addr1_l = reg_cfg->buf_cfg.vedu_vcpi_refc_l0_addr1_l;

    all_ddr->vedu_vcpi_refy_l1_addr1_l = reg_cfg->buf_cfg.vedu_vcpi_refy_l1_addr1_l;
    all_ddr->vedu_vcpi_refc_l1_addr1_l = reg_cfg->buf_cfg.vedu_vcpi_refc_l1_addr1_l;

    all_ddr->vedu_vcpi_rec_yaddr1_l = reg_cfg->buf_cfg.vedu_vcpi_rec_yaddr1_l;
    all_ddr->vedu_vcpi_rec_caddr1_l = reg_cfg->buf_cfg.vedu_vcpi_rec_caddr1_l;
#endif


    {
        u_vedu_vcpi_ref_l0_stride reg_data;
        reg_data.bits.vcpi_refy_l0_stride = reg_cfg->buf_cfg.vcpi_refy_l0_stride;
        reg_data.bits.vcpi_refc_l0_stride = reg_cfg->buf_cfg.vcpi_refc_l0_stride;
        all_ddr->vedu_vcpi_ref_l0_stride.data = reg_data.data;
    }
    {
        u_vedu_vcpi_ref_l1_stride  reg_data;

        reg_data.bits.vcpi_refy_l1_stride = reg_cfg->buf_cfg.vcpi_refy_l1_stride;
        reg_data.bits.vcpi_refc_l1_stride = reg_cfg->buf_cfg.vcpi_refc_l1_stride;
        all_ddr->vedu_vcpi_ref_l1_stride.data = reg_data.data;
    }
    {
        u_vedu_vcpi_rec_stride  reg_data;
        reg_data.bits.vcpi_recst_ystride = reg_cfg->buf_cfg.vcpi_recst_ystride;
        reg_data.bits.vcpi_recst_cstride = reg_cfg->buf_cfg.vcpi_recst_cstride;
        all_ddr->vedu_vcpi_rec_stride.data = reg_data.data;
    }
    all_ddr->vedu_vcpi_pmest_stride = reg_cfg->buf_cfg.pme_stride;
    all_ddr->vedu_vcpi_pmeld_stride = reg_cfg->buf_cfg.pme_stride;
}

static hi_void ddr_cfg_reg_61(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    if (reg_cfg->ctrl_cfg.enable_rcn_ref_share_buf == HI_TRUE) {
        {
            u_vedu_vcpi_rec_length reg_data;
            reg_data.bits.vcpi_rec_luma_length = reg_cfg->buf_cfg.vcpi_rec_luma_length;
            reg_data.bits.vcpi_rec_chroma_length = reg_cfg->buf_cfg.vcpi_rec_chroma_length;
            all_ddr->vedu_vcpi_rec_length.data = reg_data.data;
        }
        {
            u_vedu_vcpi_ref_l0_length reg_data;
            reg_data.bits.vcpi_ref0_luma_length  = reg_cfg->buf_cfg.vcpi_ref0_luma_length;
            reg_data.bits.vcpi_ref0_chroma_length = reg_cfg->buf_cfg.vcpi_ref0_chroma_length;
            all_ddr->vedu_vcpi_ref_l0_length.data = reg_data.data;
        }
        {
            u_vedu_vcpi_ref_l1_length reg_data;
            reg_data.bits.vcpi_ref1_luma_length = reg_cfg->buf_cfg.vcpi_ref1_luma_length;
            reg_data.bits.vcpi_ref1_chroma_length = reg_cfg->buf_cfg.vcpi_ref1_chroma_length;
            all_ddr->vedu_vcpi_ref_l1_length.data = reg_data.data;
        }
    } else {
        {
            u_vedu_vcpi_rec_length reg_data;
            reg_data.bits.vcpi_rec_luma_length = reg_cfg->ctrl_cfg.enc_height;
            reg_data.bits.vcpi_rec_chroma_length = reg_cfg->ctrl_cfg.enc_height;
            all_ddr->vedu_vcpi_rec_length.data = reg_data.data;
        }
        {
            u_vedu_vcpi_ref_l0_length reg_data;
            reg_data.bits.vcpi_ref0_luma_length = reg_cfg->ctrl_cfg.enc_height;
            reg_data.bits.vcpi_ref0_chroma_length = reg_cfg->ctrl_cfg.enc_height;
            all_ddr->vedu_vcpi_ref_l0_length.data = reg_data.data;
        }
        {
            u_vedu_vcpi_ref_l1_length reg_data;
            reg_data.bits.vcpi_ref1_luma_length = reg_cfg->ctrl_cfg.enc_height;
            reg_data.bits.vcpi_ref1_chroma_length = reg_cfg->ctrl_cfg.enc_height;
            all_ddr->vedu_vcpi_ref_l1_length.data = reg_data.data;
        }
    }
}

static hi_void ddr_cfg_reg_62(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    all_ddr->vedu_vcpi_pmeld_l0_addr_l = reg_cfg->buf_cfg.vedu_vcpi_pmeld_l0_addr_l;
    all_ddr->vedu_vcpi_pmeld_l0_addr_h = 0x0;
    all_ddr->vedu_vcpi_pmeld_l1_addr_l = reg_cfg->buf_cfg.vedu_vcpi_pmeld_l1_addr_l;
    all_ddr->vedu_vcpi_pmeld_l1_addr_h = 0x0;

    all_ddr->vedu_vcpi_pmest_addr_l = reg_cfg->buf_cfg.vedu_vcpi_pmest_addr_l;
    all_ddr->vedu_vcpi_pmest_addr_h = 0x0 ;
    all_ddr->vedu_vcpi_swptraddr_l = reg_cfg->buf_cfg.strm_buf_wp_addr;
    all_ddr->vedu_vcpi_srptraddr_l = reg_cfg->buf_cfg.strm_buf_rp_addr;
    all_ddr->vedu_vcpi_swptraddr_h = 0x0;
    all_ddr->vedu_vcpi_srptraddr_h = 0x0;

    all_ddr->vedu_vcpi_pmeinfo_st_addr_l = reg_cfg->buf_cfg.vedu_vcpi_pmeinfo_st_addr_l;
    all_ddr->vedu_vcpi_pmeinfo_ld0_addr_l = reg_cfg->buf_cfg.vedu_vcpi_pmeinfo_ld0_addr_l;
    all_ddr->vedu_vcpi_pmeinfo_st_addr_h = 0x0;
    all_ddr->vedu_vcpi_pmeinfo_ld0_addr_h = 0x0;


    all_ddr->vedu_vcpi_qpgld_inf_addr_l = 0;
    all_ddr->vedu_skipweight_ld_addr_l = 0;
    all_ddr->vedu_vcpi_nbi_mvst_addr_l = reg_cfg->buf_cfg.vedu_vcpi_nbi_mvst_addr_l ;
    all_ddr->vedu_vcpi_nbi_mvld_addr_l = reg_cfg->buf_cfg.vedu_vcpi_nbi_mvld_addr_l;
    all_ddr->vedu_vcpi_qpgld_inf_addr_h = 0x0;
    all_ddr->vedu_skipweight_ld_addr_h = 0x0;
    all_ddr->vedu_vcpi_nbi_mvst_addr_h = 0x0;
    all_ddr->vedu_vcpi_nbi_mvld_addr_h = 0x0;
}

static hi_void ddr_cfg_reg_63(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    all_ddr->vedu_emar_wch_press_en.data = 0x0;
    all_ddr->vedu_emar_rch_press_en.data = 0x0;
    {
        u_vedu_emar_awpress_num_00 reg_data;
        reg_data.bits.vcpi_awpress_ch00_num = 0;
        reg_data.bits.vcpi_awpress_ch01_num = 0;
        all_ddr->vedu_emar_awpress_num_00.data = reg_data.data;
    }
    {
        u_vedu_emar_awpress_num_01 reg_data;
        reg_data.bits.vcpi_awpress_ch02_num = 0;
        reg_data.bits.vcpi_awpress_ch03_num = 0;
        all_ddr->vedu_emar_awpress_num_01.data = reg_data.data;
    }
    {
        u_vedu_emar_awpress_num_02 reg_data;
        reg_data.bits.vcpi_awpress_ch04_num = 0;
        reg_data.bits.vcpi_awpress_ch05_num = 0;
        all_ddr->vedu_emar_awpress_num_02.data = reg_data.data;
    }
    {
        u_vedu_emar_awpress_num_03 reg_data;
        reg_data.bits.vcpi_awpress_ch06_num = 0;
        reg_data.bits.vcpi_awpress_ch07_num = 0;
        all_ddr->vedu_emar_awpress_num_03.data = reg_data.data;
    }
    {
        u_vedu_emar_awpress_num_04 reg_data;
        reg_data.bits.vcpi_awpress_ch08_num = 0;
        reg_data.bits.vcpi_awpress_ch09_num = 0;
        all_ddr->vedu_emar_awpress_num_04.data = reg_data.data;
    }
    {
        u_vedu_emar_awpress_num_05 reg_data;
        reg_data.bits.vcpi_awpress_ch10_num = 0;
        reg_data.bits.vcpi_awpress_ch11_num = 0;
        all_ddr->vedu_emar_awpress_num_05.data = reg_data.data;
    }
    {
        u_vedu_emar_awpress_num_06 reg_data;
        reg_data.bits.vcpi_awpress_ch12_num = 0;
        reg_data.bits.vcpi_awpress_ch13_num = 0;
        all_ddr->vedu_emar_awpress_num_06.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_64(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    {
        u_vedu_emar_awpress_num_07 reg_data;
        reg_data.bits.vcpi_awpress_ch14_num = 0;
        all_ddr->vedu_emar_awpress_num_07.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_00 reg_data;
        reg_data.bits.vcpi_arpress_ch00_num = 0;
        reg_data.bits.vcpi_arpress_ch01_num = 0;
        all_ddr->vedu_emar_arpress_num_00.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_01 reg_data;
        reg_data.bits.vcpi_arpress_ch02_num = 0;
        reg_data.bits.vcpi_arpress_ch03_num = 0;
        all_ddr->vedu_emar_arpress_num_01.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_02 reg_data;
        reg_data.bits.vcpi_arpress_ch04_num = 0;
        reg_data.bits.vcpi_arpress_ch05_num = 0;
        all_ddr->vedu_emar_arpress_num_02.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_03 reg_data;
        reg_data.bits.vcpi_arpress_ch06_num = 0;
        reg_data.bits.vcpi_arpress_ch07_num = 0;
        all_ddr->vedu_emar_arpress_num_03.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_04 reg_data;
        reg_data.bits.vcpi_arpress_ch08_num = 0;
        reg_data.bits.vcpi_arpress_ch09_num = 0;
        all_ddr->vedu_emar_arpress_num_04.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_05 reg_data;
        reg_data.bits.vcpi_arpress_ch10_num = 0;
        reg_data.bits.vcpi_arpress_ch11_num = 0;
        all_ddr->vedu_emar_arpress_num_05.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_06 reg_data;
        reg_data.bits.vcpi_arpress_ch12_num = 0;
        reg_data.bits.vcpi_arpress_ch13_num = 0;
        all_ddr->vedu_emar_arpress_num_06.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_65(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    {
        u_vedu_emar_arpress_num_07 reg_data;
        reg_data.bits.vcpi_arpress_ch14_num = 0;
        reg_data.bits.vcpi_arpress_ch15_num = 0;
        all_ddr->vedu_emar_arpress_num_07.data = reg_data.data;
    }
    {
        u_vedu_emar_arpress_num_08 reg_data;
        reg_data.bits.vcpi_arpress_ch16_num = 0;
        reg_data.bits.vcpi_arpress_ch17_num = 0;
        all_ddr->vedu_emar_arpress_num_08.data = reg_data.data;
    }
    {
        u_vedu_emar_wpress_num_00 reg_data;
        reg_data.bits.vcpi_wpress_ch00_num = 0;
        reg_data.bits.vcpi_wpress_ch01_num = 0;
        all_ddr->vedu_emar_wpress_num_00.data = reg_data.data;
    }
    {
        u_vedu_emar_wpress_num_01 reg_data;
        reg_data.bits.vcpi_wpress_ch02_num = 0;
        reg_data.bits.vcpi_wpress_ch03_num = 0;
        all_ddr->vedu_emar_wpress_num_01.data = reg_data.data;
    }
    {
        u_vedu_emar_wpress_num_02 reg_data;
        reg_data.bits.vcpi_wpress_ch04_num = 0 ;
        reg_data.bits.vcpi_wpress_ch05_num = 0;
        all_ddr->vedu_emar_wpress_num_02.data = reg_data.data;
    }
    {
        u_vedu_emar_wpress_num_03 reg_data;
        reg_data.bits.vcpi_wpress_ch06_num = 0;
        reg_data.bits.vcpi_wpress_ch07_num = 0;
        all_ddr->vedu_emar_wpress_num_03.data = reg_data.data;
    }
    {
        u_vedu_emar_wpress_num_04 reg_data;
        reg_data.bits.vcpi_wpress_ch08_num = 0;
        reg_data.bits.vcpi_wpress_ch09_num = 0;
        all_ddr->vedu_emar_wpress_num_04.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_66(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_emar_wpress_num_05 reg_data;
        reg_data.bits.vcpi_wpress_ch10_num = 0;
        reg_data.bits.vcpi_wpress_ch11_num = 0;
        all_ddr->vedu_emar_wpress_num_05.data = reg_data.data;
    }
    {
        u_vedu_emar_wpress_num_06 reg_data;
        reg_data.bits.vcpi_wpress_ch12_num = 0;
        reg_data.bits.vcpi_wpress_ch13_num = 0;
        all_ddr->vedu_emar_wpress_num_06.data = reg_data.data;
    }
    {
        u_vedu_emar_wpress_num_07 reg_data;
        reg_data.bits.vcpi_wpress_ch14_num = 0;
        all_ddr->vedu_emar_wpress_num_07.data = reg_data.data;
    }
    {
        u_vedu_vcpi_tmv_load reg_data;
        if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
            if (static_cfg->base_cfg.tmv_en == 1) {
                reg_data.bits.vcpi_tmv_wr_rd_avail = 0x3;
            } else {
                reg_data.bits.vcpi_tmv_wr_rd_avail = static_cfg->base_cfg.vcpi_tmv_wr_rd_avail;
            }
        } else if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {
            reg_data.bits.vcpi_tmv_wr_rd_avail = 0x3;
        }
        all_ddr->vedu_vcpi_tmv_load.data = reg_data.data;
    }
    {
        u_vedu_vcpi_iblk_pre_mv_thr reg_data;
        reg_data.bits.vcpi_iblk_pre_mvy_thr = static_cfg->base_cfg.vcpi_iblk_pre_mvy_thr;
        reg_data.bits.vcpi_iblk_pre_mvx_thr = static_cfg->base_cfg.vcpi_iblk_pre_mvx_thr;
        reg_data.bits.vcpi_iblk_pre_mv_dif_thr1 = static_cfg->base_cfg.vcpi_iblk_pre_mv_dif_thr1;
        reg_data.bits.vcpi_iblk_pre_mv_dif_thr0 = static_cfg->base_cfg.vcpi_iblk_pre_mv_dif_thr0;
        all_ddr->vedu_vcpi_iblk_pre_mv_thr.data = reg_data.data;
    }
    {
        u_vedu_vcpi_i_slc_insert reg_data;
        reg_data.bits.vcpi_insert_i_slc_en  = static_cfg->base_cfg.vcpi_insert_i_slc_en;
        reg_data.bits.vcpi_insert_i_slc_idx = static_cfg->base_cfg.vcpi_insert_i_slc_idx;
        all_ddr->vedu_vcpi_i_slc_insert.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_67(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr, venc_hal_static_cfg *static_cfg)
{
    {
        u_vedu_vcpi_iblk_pre_cost_thr reg_data;
        reg_data.bits.pme_iblk_pre_cost_thr_h264 = static_cfg->pme_cfg_static.pme_iblk_pre_cost_thr_h264;
        reg_data.bits.pme_intrablk_det_cost_thr1 = static_cfg->pme_cfg_static.pme_intrablk_det_cost_thr1;
        all_ddr->vedu_pme_iblk_cost_thr.data = reg_data.data;
    }
        all_ddr->vedu_qpg_cu_min_sad_thresh_0 = static_cfg->qpg_cfg_static.min_sad_thresh_low;
    {
        u_vedu_sel_offset_strength reg_data;
        reg_data.bits.sel_offset_strength = 1; /* static_cfg->img_improve_cfg_static.sel_offset_strength */
        all_ddr->vedu_sel_offset_strength.data = reg_data.data;
    }
    {
        u_vedu_sel_cu32_dc_ac_th_offset reg_data;
        reg_data.bits.sel_cu32_dc_ac_th_offset = 1;
        all_ddr->vedu_sel_cu32_dc_ac_th_offset.data = reg_data.data;
    }
    {
        u_vedu_sel_cu32_qp_th reg_data;
        reg_data.bits.sel_cu32_qp0_th = 0x26;
        reg_data.bits.sel_cu32_qp1_th = 0x21;
        all_ddr->vedu_sel_cu32_qp_th.data = reg_data.data;
    }
    {
        u_vedu_sel_res_dc_ac_th reg_data;
        reg_data.bits.sel_res16_luma_dc_th = 0x3;
        reg_data.bits.sel_res16_chroma_dc_th = 0x2;
        reg_data.bits.sel_res16_luma_ac_th = 0x3;
        reg_data.bits.sel_res16_chroma_ac_th = 0x2;
        reg_data.bits.sel_res32_luma_dc_th = 0x3;
        reg_data.bits.sel_res32_chroma_dc_th = 0x4;
        reg_data.bits.sel_res32_luma_ac_th = 0x3;
        reg_data.bits.sel_res32_chroma_ac_th = 0x4;
        all_ddr->vedu_sel_res_dc_ac_th.data = reg_data.data;
    }
}

static hi_void ddr_cfg_reg_68(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    hi_s32 i;
    /* the stream of current frame wether includes para sets, I:yes, P: no */
    if (reg_cfg->buf_cfg.stream_flag == STREAM_LIST) {
        all_ddr->vedu_vlcst_parameter.bits.vlcst_store_type = 1;
        if (reg_cfg->rc_cfg.intra_pic) {
            all_ddr->vedu_vlcst_parameter.bits.vlcst_para_set_en = 1;
        } else {
            all_ddr->vedu_vlcst_parameter.bits.vlcst_para_set_en = 0;
        }
        /* para sets wether has indepandent base addr, now must be no, because para set only can use mmz buffer */
        all_ddr->vedu_vlcst_parameter.bits.vlcst_para_sprat_en = 1;
        if (all_ddr->vedu_vlcst_parameter.bits.vlcst_para_set_en == 1) {
            /* Length of the parameter set, which is aligned with 64 bytes.
             * The value is greater than 0 and less than or equal to 192 bytes.
             */
            all_ddr->vedu_vlcst_parameter.bits.vlcst_para_set_len = reg_cfg->buf_cfg.para_set_buf_size;
            for (i = 0; i < 48; i++) { /* 48: array size */
                all_ddr->vedu_vlcst_para_data[i] = reg_cfg->buf_cfg.para_set_reg[i];
            }
        }
    }
}

static hi_void ddr_cfg_reg_69(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    all_ddr->vedu_vlcst_strmaddr0_l = reg_cfg->buf_cfg.strm_buf_addr[0];
    all_ddr->vedu_vlcst_strmaddr1_l = reg_cfg->buf_cfg.strm_buf_addr[1];
    all_ddr->vedu_vlcst_strmaddr2_l = reg_cfg->buf_cfg.strm_buf_addr[2]; /* 2: array index */
    all_ddr->vedu_vlcst_strmaddr3_l = reg_cfg->buf_cfg.strm_buf_addr[3]; /* 3: array index */
    all_ddr->vedu_vlcst_strmaddr4_l = reg_cfg->buf_cfg.strm_buf_addr[4]; /* 4: array index */
    all_ddr->vedu_vlcst_strmaddr5_l = reg_cfg->buf_cfg.strm_buf_addr[5]; /* 5: array index */
    all_ddr->vedu_vlcst_strmaddr6_l = reg_cfg->buf_cfg.strm_buf_addr[6]; /* 6: array index */
    all_ddr->vedu_vlcst_strmaddr7_l = reg_cfg->buf_cfg.strm_buf_addr[7]; /* 7: array index */
    all_ddr->vedu_vlcst_strmaddr8_l = reg_cfg->buf_cfg.strm_buf_addr[8]; /* 8: array index */
    all_ddr->vedu_vlcst_strmaddr9_l = reg_cfg->buf_cfg.strm_buf_addr[9]; /* 9: array index */
    all_ddr->vedu_vlcst_strmaddr10_l = reg_cfg->buf_cfg.strm_buf_addr[10]; /* 10: array index */
    all_ddr->vedu_vlcst_strmaddr11_l = reg_cfg->buf_cfg.strm_buf_addr[11]; /* 11: array index */
    all_ddr->vedu_vlcst_strmaddr12_l = reg_cfg->buf_cfg.strm_buf_addr[12]; /* 12: array index */
    all_ddr->vedu_vlcst_strmaddr13_l = reg_cfg->buf_cfg.strm_buf_addr[13]; /* 13: array index */
    all_ddr->vedu_vlcst_strmaddr14_l = reg_cfg->buf_cfg.strm_buf_addr[14]; /* 14: array index */
    all_ddr->vedu_vlcst_strmaddr15_l = reg_cfg->buf_cfg.strm_buf_addr[15]; /* 15: array index */
    all_ddr->vedu_vlcst_strmaddr16_l = 0x0;
    all_ddr->vedu_vlcst_strmaddr17_l = 0x0;
    all_ddr->vedu_vlcst_strmaddr18_l = 0x0;
    all_ddr->vedu_vlcst_strmaddr19_l = 0x0;
    all_ddr->vedu_vlcst_strmaddr20_l = 0x0;
    all_ddr->vedu_vlcst_strmaddr21_l = 0x0;
    all_ddr->vedu_vlcst_strmaddr22_l = 0x0;
    all_ddr->vedu_vlcst_strmaddr23_l = 0x0;
    all_ddr->vedu_vlcst_strmaddr24_l = 0x0;
    all_ddr->vedu_vlcst_strmaddr25_l = 0x0;
    all_ddr->vedu_vlcst_strmaddr26_l = 0x0;
    all_ddr->vedu_vlcst_strmaddr27_l = 0x0;
    all_ddr->vedu_vlcst_strmaddr28_l = 0x0;
    all_ddr->vedu_vlcst_strmaddr29_l = 0x0;
    all_ddr->vedu_vlcst_strmaddr30_l = 0x0;
    all_ddr->vedu_vlcst_strmaddr31_l = 0x0;
}

static hi_void ddr_cfg_reg_70(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    all_ddr->vedu_vlcst_strmaddr0_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr1_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr2_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr3_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr4_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr5_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr6_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr7_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr8_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr9_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr10_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr11_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr12_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr13_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr14_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr15_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr16_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr17_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr18_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr19_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr20_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr21_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr22_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr23_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr24_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr25_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr26_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr27_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr28_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr29_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr30_h = 0x0;
    all_ddr->vedu_vlcst_strmaddr31_h = 0x0;
}

static hi_void ddr_cfg_reg_71(venc_hal_cfg *reg_cfg, s_vedu_regs_type  *all_ddr)
{
    all_ddr->vedu_vlcst_strmbuflen0 = reg_cfg->buf_cfg.strm_buf_size[0];
    all_ddr->vedu_vlcst_strmbuflen1 = reg_cfg->buf_cfg.strm_buf_size[1];
    all_ddr->vedu_vlcst_strmbuflen2 = reg_cfg->buf_cfg.strm_buf_size[2]; /* 2: array index */
    all_ddr->vedu_vlcst_strmbuflen3 = reg_cfg->buf_cfg.strm_buf_size[3]; /* 3: array index */
    all_ddr->vedu_vlcst_strmbuflen4 = reg_cfg->buf_cfg.strm_buf_size[4]; /* 4: array index */
    all_ddr->vedu_vlcst_strmbuflen5 = reg_cfg->buf_cfg.strm_buf_size[5]; /* 5: array index */
    all_ddr->vedu_vlcst_strmbuflen6 = reg_cfg->buf_cfg.strm_buf_size[6]; /* 6: array index */
    all_ddr->vedu_vlcst_strmbuflen7 = reg_cfg->buf_cfg.strm_buf_size[7]; /* 7: array index */
    all_ddr->vedu_vlcst_strmbuflen8 = reg_cfg->buf_cfg.strm_buf_size[8]; /* 8: array index */
    all_ddr->vedu_vlcst_strmbuflen9 = reg_cfg->buf_cfg.strm_buf_size[9]; /* 9: array index */
    all_ddr->vedu_vlcst_strmbuflen10 = reg_cfg->buf_cfg.strm_buf_size[10]; /* 10: array index */
    all_ddr->vedu_vlcst_strmbuflen11 = reg_cfg->buf_cfg.strm_buf_size[11]; /* 11: array index */
    all_ddr->vedu_vlcst_strmbuflen12 = reg_cfg->buf_cfg.strm_buf_size[12]; /* 12: array index */
    all_ddr->vedu_vlcst_strmbuflen13 = reg_cfg->buf_cfg.strm_buf_size[13]; /* 13: array index */
    all_ddr->vedu_vlcst_strmbuflen14 = reg_cfg->buf_cfg.strm_buf_size[14]; /* 14: array index */
    all_ddr->vedu_vlcst_strmbuflen15 = reg_cfg->buf_cfg.strm_buf_size[15]; /* 15: array index */
    all_ddr->vedu_vlcst_strmbuflen16 = 0x0;
    all_ddr->vedu_vlcst_strmbuflen17 = 0x0;
    all_ddr->vedu_vlcst_strmbuflen18 = 0x0;
    all_ddr->vedu_vlcst_strmbuflen19 = 0x0;
    all_ddr->vedu_vlcst_strmbuflen20 = 0x0;
    all_ddr->vedu_vlcst_strmbuflen21 = 0x0;
    all_ddr->vedu_vlcst_strmbuflen22 = 0x0;
    all_ddr->vedu_vlcst_strmbuflen23 = 0x0;
    all_ddr->vedu_vlcst_strmbuflen24 = 0x0;
    all_ddr->vedu_vlcst_strmbuflen25 = 0x0;
    all_ddr->vedu_vlcst_strmbuflen26 = 0x0;
    all_ddr->vedu_vlcst_strmbuflen27 = 0x0;
    all_ddr->vedu_vlcst_strmbuflen28 = 0x0;
    all_ddr->vedu_vlcst_strmbuflen29 = 0x0;
    all_ddr->vedu_vlcst_strmbuflen30 = 0x0;
    all_ddr->vedu_vlcst_strmbuflen31 = 0x0;

    all_ddr->vedu_vlcst_para_addr_l  = reg_cfg->buf_cfg.para_set_addr;
    all_ddr->vedu_vlcst_para_addr_h  = 0;
}

static hi_void ddr_cfg_reg_upper_half(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, s_vedu_regs_type  *all_ddr,
                                      venc_hal_dynamic_cfg *dynamic_cfg, venc_hal_static_cfg *static_cfg)
{
    /* reg cfg */
    direct_cfg_reg_01(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_02(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_03(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_04(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_05(reg_cfg, all_reg, dynamic_cfg, static_cfg);
    direct_cfg_reg_06(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_07(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_08(reg_cfg, all_reg, static_cfg);
    direct_cfg_reg_09(reg_cfg, all_reg);
    direct_cfg_reg_10(reg_cfg, all_reg, static_cfg);
    /* ddr cfg */
    ddr_cfg_reg_11(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_12(reg_cfg, all_ddr, dynamic_cfg, static_cfg);
    ddr_cfg_reg_13(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_14(reg_cfg, all_ddr);
    ddr_cfg_reg_15(reg_cfg, all_ddr);
    ddr_cfg_reg_16(reg_cfg, all_ddr);
    ddr_cfg_reg_17(reg_cfg, all_ddr);
    ddr_cfg_reg_18(reg_cfg, all_ddr);
    ddr_cfg_reg_19(reg_cfg, all_ddr);
    ddr_cfg_reg_20(reg_cfg, all_ddr);
    ddr_cfg_reg_21(reg_cfg, all_ddr, dynamic_cfg, static_cfg);
    ddr_cfg_reg_22(reg_cfg, all_ddr, dynamic_cfg, static_cfg);
    ddr_cfg_reg_23(reg_cfg, all_ddr, dynamic_cfg, static_cfg);
    ddr_cfg_reg_24(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_25(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_26(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_27(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_28(reg_cfg, all_ddr, dynamic_cfg, static_cfg);
    ddr_cfg_reg_29(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_30(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_31(reg_cfg, all_ddr, static_cfg);
#ifdef VENC_SUPPORT_ROI
    ddr_cfg_reg_32(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_33(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_34(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_35(reg_cfg, all_ddr, static_cfg);
#else
    ddr_cfg_reg_36(all_ddr);
#endif
}

static hi_void ddr_cfg_reg_lower_half(venc_hal_cfg *reg_cfg, vedu_regs_type  *all_reg, s_vedu_regs_type  *all_ddr,
                                      venc_hal_dynamic_cfg *dynamic_cfg, venc_hal_static_cfg *static_cfg)
{
    ddr_cfg_reg_37(reg_cfg, all_ddr, dynamic_cfg, static_cfg);
    ddr_cfg_reg_38(reg_cfg, all_ddr, dynamic_cfg, static_cfg);
    ddr_cfg_reg_39(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_40(reg_cfg, all_ddr, dynamic_cfg, static_cfg);
    ddr_cfg_reg_41(reg_cfg, all_ddr, dynamic_cfg, static_cfg);
    ddr_cfg_reg_42(reg_cfg, all_ddr, dynamic_cfg, static_cfg);
    ddr_cfg_reg_43(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_44(reg_cfg, all_ddr, dynamic_cfg, static_cfg);
    ddr_cfg_reg_45(reg_cfg, all_ddr, dynamic_cfg, static_cfg);
    ddr_cfg_reg_46(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_47(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_48(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_49(reg_cfg, all_ddr, dynamic_cfg);
    ddr_cfg_reg_50(reg_cfg, all_ddr);
    ddr_cfg_reg_51(reg_cfg, all_ddr);
    ddr_cfg_reg_52(reg_cfg, all_ddr, dynamic_cfg, static_cfg);
    ddr_cfg_reg_53(reg_cfg, all_ddr);
    ddr_cfg_reg_54(reg_cfg, all_ddr);
    ddr_cfg_reg_55(reg_cfg, all_ddr);
    ddr_cfg_reg_56(reg_cfg, all_ddr);
    ddr_cfg_reg_57(reg_cfg, all_ddr);
    ddr_cfg_reg_58(reg_cfg, all_ddr);
    ddr_cfg_reg_59(reg_cfg, all_ddr);
    ddr_cfg_reg_60(reg_cfg, all_ddr);
    ddr_cfg_reg_61(reg_cfg, all_ddr);
    ddr_cfg_reg_62(reg_cfg, all_ddr);
    ddr_cfg_reg_63(reg_cfg, all_ddr);
    ddr_cfg_reg_64(reg_cfg, all_ddr);
    ddr_cfg_reg_65(reg_cfg, all_ddr);
    ddr_cfg_reg_66(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_67(reg_cfg, all_ddr, static_cfg);
    ddr_cfg_reg_68(reg_cfg, all_ddr);
    ddr_cfg_reg_69(reg_cfg, all_ddr);
    ddr_cfg_reg_70(reg_cfg, all_ddr);
    ddr_cfg_reg_71(reg_cfg, all_ddr);
    direct_cfg_reg_72(reg_cfg, all_reg, static_cfg);
}
/******************************************************************************
function   :
description: config vedu reg & start one frame encode by DDR method.
calls      :
input      :
output     :
return     :
others     :
******************************************************************************/
static void drv_venc_hal_ddr_cfg_reg(venc_hal_cfg *reg_cfg)
{
    vedu_regs_type *all_reg = (vedu_regs_type*)g_reg_base;
    s_vedu_regs_type* all_ddr = (s_vedu_regs_type *)(ULONG)reg_cfg->ext_cfg.ddr_reg_cfg_vir_addr;
    venc_hal_dynamic_cfg *dynamic_cfg = &g_hal_reg_info->reg_dynamic_cfg;
    venc_hal_static_cfg *static_cfg = &g_hal_reg_info->reg_static_cfg;

    g_venc_osal_func_ptr.fun_osal_mem_cpy(&g_hal_reg_info->reg_ext_cfg, sizeof(venc_hal_cfg),
        reg_cfg, sizeof(venc_hal_cfg));
#if PARA_ADAPT_ENCODER
    drv_venc_hal_set_default_attr_by_start_qp(dynamic_cfg, reg_cfg->rc_cfg.start_qp, reg_cfg->rc_cfg.intra_pic);
#endif
    drv_venc_hal_set_default_attr_by_protocol(dynamic_cfg, reg_cfg->ctrl_cfg.protocol);
    drv_venc_hal_set_tunl_read_intvl(reg_cfg, &dynamic_cfg->tunl_read_intvl);
    drv_venc_hal_set_mode_by_profile(reg_cfg->ctrl_cfg.profile, reg_cfg->ctrl_cfg.protocol, dynamic_cfg);
    reg_constraint_for176x144(&reg_cfg->ctrl_cfg, &reg_cfg->rc_cfg, &dynamic_cfg->qpg_cfg_dynamic.qp_delta, static_cfg);

    ddr_cfg_reg_upper_half(reg_cfg, all_reg, all_ddr, dynamic_cfg, static_cfg);
    ddr_cfg_reg_lower_half(reg_cfg, all_reg, all_ddr, dynamic_cfg, static_cfg);
}
#endif


static hi_void set_default_info_ext_config(venc_hal_cfg *reg_cfg)
{
    reg_cfg->reg_base = g_reg_base;

    /* get ctrl_cfg default info */
    reg_cfg->ctrl_cfg.protocol = VEDU_H264;
    reg_cfg->ctrl_cfg.profile = 0x3; /* 3: h264 high profile,h265 1¨¬?¡§¦Ì?¨°a¡Á¡é¨°a!!!!!!! */
    reg_cfg->ctrl_cfg.enc_width = 1280; /* 1280: enc_width */
    reg_cfg->ctrl_cfg.enc_height = 720; /* 720: enc_height */
    reg_cfg->ctrl_cfg.split_size = 0xa * 1024; /* 1024: byte to kb */
    reg_cfg->ctrl_cfg.low_dly_mode = 1;
    reg_cfg->ctrl_cfg.time_out_en = 0x3;
    reg_cfg->ctrl_cfg.reg_config_mode = VENC_REG_CONFIG_MODE; /* 0: cfg mode 1: ddr mode */

    /* get rc_cfg default info */
    reg_cfg->rc_cfg.start_qp = 0x1c;
    reg_cfg->rc_cfg.min_qp = 0x10;
    reg_cfg->rc_cfg.max_qp = 0x30;

   /* get smmu_cfg default info */
#ifndef HI_SMMU_SUPPORT
    reg_cfg->smmu_cfg.src_y_bypass = 1;
    reg_cfg->smmu_cfg.src_u_bypass = 1;
    reg_cfg->smmu_cfg.src_v_bypass = 1;
#endif
}

static hi_void set_default_info_static_config(venc_hal_static_cfg *static_cfg, hi_u32 protocol)
{
   /* get other reg default info */
#ifdef VENC_SUPPORT_ROI
    set_roi_info(&static_cfg->roi_cfg);
#endif
    set_img_improve_info(&static_cfg->img_improve_cfg_static);

    set_qpg_info(&static_cfg->qpg_cfg_static);

    set_pme_info(&static_cfg->pme_cfg_static, protocol);

    set_base_info(&static_cfg->base_cfg);
    set_low_power_info(&static_cfg->low_power_cfg);
    set_q_scaling_info(&static_cfg->q_scaling_cfg);

    set_other_reg_info(static_cfg);
}

static hi_void set_default_info_dynamic_config(venc_hal_dynamic_cfg *dynamic_cfg, hi_u32 protocol)
{
    set_img_improve_info_dynamic(&dynamic_cfg->img_improve_cfg_dynamic);
    set_qpg_info_dynamic(&dynamic_cfg->qpg_cfg_dynamic, protocol);
    set_pme_info_dynamic(&dynamic_cfg->pme_cfg_dynamic);
    set_other_reg_info_dynamic(dynamic_cfg, protocol);

#if PARA_ADAPT_ENCODER
    set_para_for_pic_quality(dynamic_cfg->enc_para_table);
#endif
    return;
}

static hi_void drv_venc_hal_set_default_info(venc_hal_reg_info *info)
{
    if (!info) {
        return;
    }

    set_default_info_ext_config(&info->reg_ext_cfg);
    set_default_info_static_config(&info->reg_static_cfg, info->reg_ext_cfg.ctrl_cfg.protocol);
    set_default_info_dynamic_config(&info->reg_dynamic_cfg, info->reg_ext_cfg.ctrl_cfg.protocol);

    return;
}

hi_void drv_venc_hal_v400_r008_c02_cfg_reg(venc_hal_cfg *reg_cfg)
{
    if (reg_cfg == NULL) {
        return;
    }
#if (VENC_REG_CONFIG_MODE == 1)
#if DDR
    drv_venc_hal_ddr_cfg_reg(reg_cfg);
#endif
#else
    drv_venc_hal_direct_cfg_reg(reg_cfg);
#endif

    return;
}

SINT32 drv_venc_hal_v400_r008_c02_init(hi_void)
{
    if (drv_venc_hal_map_base_reg() != HI_SUCCESS) {
        return HI_FAILURE;
    }
    g_hal_reg_info = g_venc_osal_func_ptr.fun_osal_alloc_vir_mem(sizeof(venc_hal_reg_info));
    if (g_hal_reg_info == NULL) {
        drv_venc_hal_unmap_base_reg();
        return HI_FAILURE;
    }
    g_venc_osal_func_ptr.fun_osal_mem_set(g_hal_reg_info, sizeof(venc_hal_reg_info), 0, sizeof(venc_hal_reg_info));
    drv_venc_hal_set_default_info(g_hal_reg_info);

    return HI_SUCCESS;
}

hi_void drv_venc_hal_v400_r008_c02_deinit(hi_void)
{
    drv_venc_hal_unmap_base_reg();

    if (g_hal_reg_info) {
        g_venc_osal_func_ptr.fun_osal_free_vir_mem(g_hal_reg_info);
        g_hal_reg_info = HI_NULL;
    }
}

