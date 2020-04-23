/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq hal tnr api
 * Author: pq
 * Create: 2016-01-1
 */

#include "pq_hal_tnr.h"
#include "drv_pq.h"

pq_tnr_dci_status g_tnr_dci_info[VPSS_HANDLE_NUM];

static hi_u8 g_pq_rgmv_data[256] = { 0 }; /* 256: rgmv data len */
static hi_u32 g_nr_ori_global_motion = 0;

/* ********************************tnr proc set api start****************************************** */
hi_s32 pq_hal_enable_tnr(hi_u32 handle_no, hi_bool tnr_on_off)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_tnr_reg153.bits.y_tnr_en = tnr_on_off;
    vpss_reg->vpss_hipp_tnr_reg153.bits.tnr_en = tnr_on_off;

    return HI_SUCCESS;
}

hi_s32 pq_hal_enable_tnr_demo(hi_u32 handle_no, hi_bool tnr_demo_en)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_tnr_reg36.bits.market_mode_en = tnr_demo_en;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_tnr_demo_mode(hi_u32 handle_no, tnr_demo_mode mode)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_tnr_reg36.bits.market_mode = mode;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_tnr_str(hi_u32 handle_no, hi_u32 tnr_str)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_tnr_reg153.bits.tnr_str = tnr_str;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_tnr_demo_mode_coor(hi_u32 handle_no, hi_u32 x_pos)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_tnr_reg36.bits.market_coor = x_pos;

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_nr_ori_global_motion(hi_u32 *nr_ori_global_motion)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(0);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    *nr_ori_global_motion = g_nr_ori_global_motion;

    return HI_SUCCESS;
}

hi_void pq_hal_set_nr_other_vir_reg(hi_drv_pq_dbg_vpss_vir_soft *vir_soft_reg)
{
    vir_soft_reg->nr_reg.ar_gm_pnl_nl_offset = 9;     /* 9 : ar_gm_pnl_nl_offset */
    vir_soft_reg->nr_reg.ar_nl_pnl_gm_offset = 9;     /* 9 : ar_nl_pnl_gm_offset */
    vir_soft_reg->nr_reg.ar_min_nl_ratio = 5;            /* 5 : ar_min_nl_ratio */
    vir_soft_reg->nr_reg.ar_nl_pnl_str = 6;               /* 6 : ar_nl_pnl_str */
    vir_soft_reg->nr_reg.ar_dark_level_thd = 20;       /* 20 : ar_dark_level_thd */
    vir_soft_reg->nr_reg.ar_dark_nl_max = 8;            /* 8 : ar_dark_nl_max */
    vir_soft_reg->nr_reg.ar_nd_reli_max = 48;           /* 48 : ar_nd_reli_max */
    vir_soft_reg->nr_reg.ar_set_nl_min = 0;
    vir_soft_reg->nr_reg.ar_set_nl_max = 31;            /* 31 : ar_set_nl_max */
    vir_soft_reg->nr_reg.ar_set_gm_min = 0;
    vir_soft_reg->nr_reg.ar_set_gm_max = 31;          /* 31 : ar_set_gm_max */
    vir_soft_reg->nr_reg.ar_md_alpha1_offset = 3;    /* 3 : ar_md_alpha1_offset */
    vir_soft_reg->nr_reg.ar_md_alpha2_offset = 8;    /* 8 : ar_md_alpha2_offset */
    vir_soft_reg->nr_reg.ar_flat_info_x_min = 0;
    vir_soft_reg->nr_reg.ar_flat_info_x_max = 56;     /* 56 : ar_flat_info_x_max */
    vir_soft_reg->nr_reg.ar_flat_info_y_min = 0;
    vir_soft_reg->nr_reg.ar_flat_info_y_max = 63;     /* 63 : ar_flat_info_y_max */
    vir_soft_reg->nr_reg.ar_md_y_gain = 24;            /* 24 : ar_md_y_gain */
    vir_soft_reg->nr_reg.ar_md_y_core = -5;            /* -5 : ar_md_y_core */
    vir_soft_reg->nr_reg.ar_md_c_gain = 24;            /* 24 : ar_md_c_gain */
    vir_soft_reg->nr_reg.ar_md_c_core = -5;            /* -5 : ar_md_c_core */
    vir_soft_reg->nr_reg.ar_mc_y_gain = 15;            /* 15 : ar_mc_y_gain */
    vir_soft_reg->nr_reg.ar_mc_y_core = 3;              /* 3 : ar_mc_y_core */
    vir_soft_reg->nr_reg.ar_mc_c_gain = 15;            /* 15 : ar_mc_c_gain */
    vir_soft_reg->nr_reg.ar_mc_c_core = 3;              /* 3 : ar_mc_c_core */
    vir_soft_reg->nr_reg.ar_ref_mv_mode = 0;
    vir_soft_reg->nr_reg.ar_mag_pnl_gain_0mv = 127;  /* 127 : ar_mag_pnl_gain_0mv */
    vir_soft_reg->nr_reg.ar_mag_pnl_core_0mv = 63;    /* 63 : ar_mag_pnl_core_0mv */
    vir_soft_reg->nr_reg.ar_mag_pnl_gain_xmv = 127;  /* 127 : ar_mag_pnl_gain_xmv */
    vir_soft_reg->nr_reg.ar_mag_pnl_core_xmv = 63;    /* 63 : ar_mag_pnl_core_xmv */
    vir_soft_reg->nr_reg.ar_std_pnl_gain = 31;            /* 31: ar_std_pnl_gain */
    vir_soft_reg->nr_reg.ar_std_pnl_core = 0;
    vir_soft_reg->nr_reg.ar_adj_0mv_min = 4;             /* 4 : ar_adj_0mv_min */
    vir_soft_reg->nr_reg.ar_adj_xmv_min = 4;             /* 4 : ar_adj_xmv_min */
    vir_soft_reg->nr_reg.ar_adj_0mv_max = 16;           /* 16 : ar_adj_0mv_max */
    vir_soft_reg->nr_reg.ar_adj_xmv_max = 16;           /* 16 : ar_adj_xmv_max */
    vir_soft_reg->nr_reg.ar_adj_mv_max = 24;             /* 24: ar_adj_mv_max */
    vir_soft_reg->nr_reg.ar_adj_mv_min = 10;              /* 10 : ar_adj_mv_min */
    vir_soft_reg->nr_reg.ar_std_core_0mv = 28;            /* 28 : ar_std_core_0mv */
    vir_soft_reg->nr_reg.ar_std_core_xmv = 0;
    vir_soft_reg->nr_reg.ar_std_pnl_gain_0mv = 16;      /* 16 : ar_std_pnl_gain_0mv */
    vir_soft_reg->nr_reg.ar_std_pnl_core_0mv = 16;      /* 16 : ar_std_pnl_core_0mv */
    vir_soft_reg->nr_reg.ar_std_pnl_gain_xmv = 16;      /* 16 : ar_std_pnl_gain_xmv */
    vir_soft_reg->nr_reg.ar_std_pnl_core_xmv = 16;      /* 16 : ar_std_pnl_core_xmv */
    vir_soft_reg->nr_reg.ar_mc1d_alpha = 20;               /* 20 : ar_mc1d_alpha */
    vir_soft_reg->nr_reg.ar_tnr_str = 16;                      /* 16 : ar_tnr_str */
    vir_soft_reg->nr_reg.ar_cnr_str = 16;                      /* 16 : ar_cnr_str */

    return;
}

hi_void pq_hal_set_nr_weakest_lut_vir_reg(hi_drv_pq_dbg_vpss_vir_soft *vir_soft_reg)
{
    vir_soft_reg->nr_reg.ar_weakest_lut[0] = 21; /* 21 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[1] = 21; /* 21 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[2] = 21; /* 2:index, 21 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[3] = 22; /* 3:index, 22 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[4] = 22; /* 4:index, 22 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[5] = 22; /* 5:index, 22 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[6] = 23; /* 6:index, 23 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[7] = 23; /* 7:index, 23 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[8] = 24; /* 8:index, 24 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[9] = 25; /* 9:index, 25 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[10] = 26; /* 10:index, 26 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[11] = 27; /* 11:index, 27 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[12] = 28; /* 12:index, 28 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[13] = 29; /* 13:index, 29 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[14] = 30; /* 14:index, 30 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[15] = 30; /* 15:index, 30 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[16] = 31; /* 16:index, 31 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[17] = 31; /* 17:index, 31 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[18] = 31; /* 18:index, 31 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[19] = 31; /* 19:index, 31 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[20] = 31; /* 20:index, 31 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[21] = 31; /* 21:index, 31 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[22] = 31; /* 22:index, 31 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[23] = 31; /* 23:index, 31 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[24] = 31; /* 24:index, 31 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[25] = 31; /* 25:index, 31 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[26] = 31; /* 26:index, 31 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[27] = 31; /* 27:index, 31 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[28] = 31; /* 28:index, 31 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[29] = 31; /* 29:index, 31 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[30] = 31; /* 30:index, 31 : weakest lut */
    vir_soft_reg->nr_reg.ar_weakest_lut[31] = 31; /* 31:index, 31 : weakest lut */

    return;
}

hi_void pq_hal_set_nr_strongest_lut_vir_reg(hi_drv_pq_dbg_vpss_vir_soft *vir_soft_reg)
{
    vir_soft_reg->nr_reg.ar_strongest_lut[0] = 1;
    vir_soft_reg->nr_reg.ar_strongest_lut[1] = 1;
    vir_soft_reg->nr_reg.ar_strongest_lut[2] = 1; /* 2:index */
    vir_soft_reg->nr_reg.ar_strongest_lut[3] = 2; /* 3:index, 2 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[4] = 2; /* 4:index, 2 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[5] = 2; /* 5:index, 2 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[6] = 3; /* 6:index, 3 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[7] = 3; /* 7:index, 3 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[8] = 4; /* 8:index, 4 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[9] = 5; /* 9:index, 5 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[10] = 6; /* 10:index, 6 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[11] = 7; /* 11:index, 7 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[12] = 8; /* 12:index, 8 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[13] = 10; /* 13:index, 10 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[14] = 12; /* 14:index, 12 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[15] = 15; /* 15:index, 15 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[16] = 18; /* 16:index, 18 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[17] = 20; /* 17:index, 20 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[18] = 22; /* 18:index, 22 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[19] = 24; /* 19:index, 24 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[20] = 25; /* 20:index, 25 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[21] = 26; /* 21:index, 26 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[22] = 27; /* 22:index, 27 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[23] = 28; /* 23:index, 28 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[24] = 29; /* 24:index, 29 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[25] = 30; /* 25:index, 30 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[26] = 30; /* 26:index, 30 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[27] = 31; /* 27:index, 31 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[28] = 31; /* 28:index, 31 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[29] = 31; /* 29:index, 31 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[30] = 31; /* 30:index, 31 : strongest lut */
    vir_soft_reg->nr_reg.ar_strongest_lut[31] = 31; /* 31:index, 31 : strongest lut */

    return;
}

hi_void pq_hal_set_nr_level_gain_vir_reg(hi_drv_pq_dbg_vpss_vir_soft *vir_soft_reg)
{
    vir_soft_reg->nr_reg.tnr_level_gain[0] = 0;
    vir_soft_reg->nr_reg.tnr_level_gain[1] = 6; /* 6 : tnr level gain */
    vir_soft_reg->nr_reg.tnr_level_gain[2] = 16; /* 2:index, 16 : tnr level gain */
    vir_soft_reg->nr_reg.tnr_level_gain[3] = 28; /* 3:index, 28 : tnr level gain */

    vir_soft_reg->nr_reg.mcnr_level_gain[0] = 63; /* 63 : mcnr level gain */
    vir_soft_reg->nr_reg.mcnr_level_gain[1] = 48; /* 48 : mcnr level gain */
    vir_soft_reg->nr_reg.mcnr_level_gain[2] = 16; /* 2:index, 16 : mcnr level gain */
    vir_soft_reg->nr_reg.mcnr_level_gain[3] = 0; /* 3:index */

    vir_soft_reg->nr_reg.snr_level_gain[0] = 0;
    vir_soft_reg->nr_reg.snr_level_gain[1] = 8; /* 8 : snr level gain */
    vir_soft_reg->nr_reg.snr_level_gain[2] = 24; /* 2:index, 24 : snr level gain */
    vir_soft_reg->nr_reg.snr_level_gain[3] = 63; /* 3:index, 63 : snr level gain */

    return;
}

hi_s32 pq_hal_set_nr_vir_reg(hi_void)
{
    hi_drv_pq_dbg_vpss_vir_soft *vir_soft_reg = HI_NULL;

    vir_soft_reg = (hi_drv_pq_dbg_vpss_vir_soft *)pq_hal_get_vpss_vir_soft_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(vir_soft_reg);

    pq_hal_set_nr_other_vir_reg(vir_soft_reg);
    pq_hal_set_nr_weakest_lut_vir_reg(vir_soft_reg);
    pq_hal_set_nr_strongest_lut_vir_reg(vir_soft_reg);
    pq_hal_set_nr_level_gain_vir_reg(vir_soft_reg);

    return HI_SUCCESS;
}
/* ********************************tnr proc set api end ****************************************** */
/* ********************************tnr get input info api start ****************************************** */
#ifdef PQ_DPT_V900_SUPPORT
#ifdef PQ_ALG_DCI
static hi_void pq_hal_get_dci_info(hi_u32 handle, pq_tnr_dci_status *dci_hist)
{
    PQ_CHECK_NULL_PTR_RE_NULL(dci_hist);

    dci_hist->dci_en = g_tnr_dci_info[handle].dci_en;
    dci_hist->dci_histgram.dci_histgram_32.normalize = g_tnr_dci_info[handle].dci_histgram.dci_histgram_32.normalize;
    dci_hist->dci_histgram.dci_histgram_32.histgram_site =
        g_tnr_dci_info[handle].dci_histgram.dci_histgram_32.histgram_site;
    memcpy(dci_hist->dci_histgram.dci_histgram_32.histgram,
        g_tnr_dci_info[handle].dci_histgram.dci_histgram_32.histgram,
        sizeof(dci_hist->dci_histgram.dci_histgram_32.histgram));

    return;
}
#endif
#endif

static hi_void pq_hal_get_tnr_stt_reg(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_tnr_stt_reg *tnr_stt_reg)
{
    vpss_stt_reg_type *stt_reg = (vpss_stt_reg_type *)vpss_stt_info->vpss_stt_reg;
    vpss_scan_stt_reg_type *nr_stt_reg = (vpss_scan_stt_reg_type *)vpss_stt_info->nr_stt_reg;
    pq_tnr_dci_status dci_info = { 0 };

    PQ_CHECK_NULL_PTR_RE_NULL(vpss_stt_info);
    PQ_CHECK_NULL_PTR_RE_NULL(tnr_stt_reg);
    PQ_CHECK_NULL_PTR_RE_NULL(stt_reg);
    PQ_CHECK_NULL_PTR_RE_NULL(nr_stt_reg);

    tnr_stt_reg->nd_noise_point_cnt_max = stt_reg->vpss_wstt_tnr_noisepointcnt_max.bits.noisepointcntmax;
    tnr_stt_reg->nd_noise_point_cnt_med = stt_reg->vpss_wstt_tnr_noisepointcnt_med.bits.noisepointcntmed;
    tnr_stt_reg->nd_noise_point_cnt_min = stt_reg->vpss_wstt_tnr_noisepointcnt_min.bits.noisepointcntmin;
    tnr_stt_reg->nd_noise_point_num = stt_reg->vpss_wstt_tnr_noisepointnum.bits.noisepointnum;
    tnr_stt_reg->nd_sum_noise_info_max = stt_reg->vpss_wstt_tnr_sumnoiseinfo_max.bits.sumnoiseinfomax;
    tnr_stt_reg->nd_sum_noise_info_med = stt_reg->vpss_wstt_tnr_sumnoiseinfo_med.bits.sumnoiseinfomed;
    tnr_stt_reg->nd_sum_noise_info_min = stt_reg->vpss_wstt_tnr_sumnoiseinfo_min.bits.sumnoiseinfomin;

    tnr_stt_reg->me_blk_mv_hist[0] = nr_stt_reg->vpss_me_glbmv_hist_2.bits.blk_mv_hist_num_0;
    tnr_stt_reg->me_blk_mv_hist[1] = nr_stt_reg->vpss_me_glbmv_hist_3.bits.blk_mv_hist_num_1;
    tnr_stt_reg->me_blk_mv_hist[2] = nr_stt_reg->vpss_me_glbmv_hist_3.bits.blk_mv_hist_num_2; /* 2:index */
    tnr_stt_reg->me_blk_mv_hist[3] = nr_stt_reg->vpss_me_glbmv_hist_4.bits.blk_mv_hist_num_3; /* 3:index */
    tnr_stt_reg->me_blk_mv_hist[4] = nr_stt_reg->vpss_me_glbmv_hist_4.bits.blk_mv_hist_num_4; /* 4:index */

    dci_info.dci_histgram.dci_histgram_32.normalize = HI_TRUE;
    dci_info.dci_histgram.dci_histgram_32.histgram_site = DCI_HISTGRAM_SITE_VO;

    /* 目前还不支持DCI，先直方图设置为0,DCI不使能，后续要获取DCI直方图信息 */
#ifdef PQ_DPT_V900_SUPPORT
#ifdef PQ_ALG_DCI
    pq_hal_get_dci_info(vpss_stt_info->handle_id, &dci_info);
    tnr_stt_reg->dci_enable = dci_info.dci_en;
#else
    memset(dci_info.dci_histgram.dci_histgram_32.histgram, 0, sizeof(dci_info.dci_histgram.dci_histgram_32.histgram));
    tnr_stt_reg->dci_enable = HI_FALSE;
#endif
#else
    memset(dci_info.dci_histgram.dci_histgram_32.histgram, 0, sizeof(dci_info.dci_histgram.dci_histgram_32.histgram));
    tnr_stt_reg->dci_enable = HI_FALSE;
#endif

    memcpy(tnr_stt_reg->dci_histgram, dci_info.dci_histgram.dci_histgram_32.histgram,
        sizeof(tnr_stt_reg->dci_histgram));

    return;
}

static hi_void pq_hal_get_me_input_reg(hi_u32 handle, drv_pq_me_input_reg *me_input_reg)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle);
    PQ_CHECK_NULL_PTR_RE_NULL(vpss_reg);
    PQ_CHECK_NULL_PTR_RE_NULL(me_input_reg);

    me_input_reg->me_rgt_bndy = vpss_reg->vpss_hipp_me_bndy.bits.rgt_bndy;
    me_input_reg->me_lft_bndy = vpss_reg->vpss_hipp_me_bndy.bits.lft_bndy;
    me_input_reg->me_top_bndy = vpss_reg->vpss_hipp_me_bndy.bits.top_bndy;
    me_input_reg->me_bot_bndy = vpss_reg->vpss_hipp_me_bndy.bits.bot_bndy;

    me_input_reg->me_ds_en = vpss_reg->vpss_ctrl.bits.meds_en;

    return;
}

static hi_s32 pq_hal_get_tnr_vir_param(hi_drv_pq_tnr_input_reg *tnr_input_reg)
{
    hi_drv_pq_dbg_vpss_vir_soft *vir_reg = HI_NULL;
    hi_drv_pq_nr_vir_reg *nr_vir_reg = HI_NULL;

    vir_reg = (hi_drv_pq_dbg_vpss_vir_soft *)pq_hal_get_vpss_vir_soft_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(vir_reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(tnr_input_reg);

    nr_vir_reg = &vir_reg->nr_reg;
    memcpy(tnr_input_reg->ar_strongest_lut, nr_vir_reg->ar_strongest_lut, sizeof(tnr_input_reg->ar_strongest_lut));
    memcpy(tnr_input_reg->ar_weakest_lut, nr_vir_reg->ar_weakest_lut, sizeof(tnr_input_reg->ar_weakest_lut));

    tnr_input_reg->ar_gm_pnl_nl_offset = nr_vir_reg->ar_gm_pnl_nl_offset;
    tnr_input_reg->ar_min_nl_ratio = nr_vir_reg->ar_min_nl_ratio;
    tnr_input_reg->ar_nl_pnl_str = nr_vir_reg->ar_nl_pnl_str;
    tnr_input_reg->ar_nl_pnl_gm_offset = nr_vir_reg->ar_nl_pnl_gm_offset;
    tnr_input_reg->ar_dark_level_thd = nr_vir_reg->ar_dark_level_thd;
    tnr_input_reg->ar_dark_nl_max = nr_vir_reg->ar_dark_nl_max;
    tnr_input_reg->ar_nd_reli_max = nr_vir_reg->ar_nd_reli_max;
    tnr_input_reg->ar_set_nl_min = nr_vir_reg->ar_set_nl_min;
    tnr_input_reg->ar_set_nl_max = nr_vir_reg->ar_set_nl_max;
    tnr_input_reg->ar_set_gm_min = nr_vir_reg->ar_set_gm_min;
    tnr_input_reg->ar_set_gm_max = nr_vir_reg->ar_set_gm_max;
    tnr_input_reg->ar_md_alpha1_offset = nr_vir_reg->ar_md_alpha1_offset;
    tnr_input_reg->ar_md_alpha2_offset = nr_vir_reg->ar_md_alpha2_offset;
    tnr_input_reg->ar_flat_info_x_min = nr_vir_reg->ar_flat_info_x_min;
    tnr_input_reg->ar_flat_info_x_max = nr_vir_reg->ar_flat_info_x_max;
    tnr_input_reg->ar_flat_info_y_min = nr_vir_reg->ar_flat_info_y_min;
    tnr_input_reg->ar_flat_info_y_max = nr_vir_reg->ar_flat_info_y_max;
    tnr_input_reg->ar_md_y_gain = nr_vir_reg->ar_md_y_gain;
    tnr_input_reg->ar_md_y_core = nr_vir_reg->ar_md_y_core;
    tnr_input_reg->ar_md_c_gain = nr_vir_reg->ar_md_c_gain;
    tnr_input_reg->ar_md_c_core = nr_vir_reg->ar_md_c_core;
    tnr_input_reg->ar_mc_y_gain = nr_vir_reg->ar_mc_y_gain;
    tnr_input_reg->ar_mc_y_core = nr_vir_reg->ar_mc_y_core;
    tnr_input_reg->ar_mc_c_gain = nr_vir_reg->ar_mc_c_gain;
    tnr_input_reg->ar_mc_c_core = nr_vir_reg->ar_mc_c_core;
    tnr_input_reg->ar_ref_mv_mode = nr_vir_reg->ar_ref_mv_mode;
    tnr_input_reg->ar_mag_pnl_gain_0mv = nr_vir_reg->ar_mag_pnl_gain_0mv;
    tnr_input_reg->ar_mag_pnl_core_0mv = nr_vir_reg->ar_mag_pnl_core_0mv;
    tnr_input_reg->ar_mag_pnl_gain_xmv = nr_vir_reg->ar_mag_pnl_gain_xmv;
    tnr_input_reg->ar_mag_pnl_core_xmv = nr_vir_reg->ar_mag_pnl_core_xmv;
    tnr_input_reg->ar_std_pnl_gain_0mv = nr_vir_reg->ar_std_pnl_gain_0mv;
    tnr_input_reg->ar_std_pnl_core_0mv = nr_vir_reg->ar_std_pnl_core_0mv;
    tnr_input_reg->ar_std_pnl_gain_xmv = nr_vir_reg->ar_std_pnl_gain_xmv;
    tnr_input_reg->ar_std_pnl_core_xmv = nr_vir_reg->ar_std_pnl_core_xmv;
    tnr_input_reg->ar_std_core_0mv = nr_vir_reg->ar_std_core_0mv;
    tnr_input_reg->ar_std_core_xmv = nr_vir_reg->ar_std_core_xmv;
    tnr_input_reg->ar_std_pnl_gain = nr_vir_reg->ar_std_pnl_gain;
    tnr_input_reg->ar_std_pnl_core = nr_vir_reg->ar_std_pnl_core;
    tnr_input_reg->ar_adj_0mv_min = nr_vir_reg->ar_adj_0mv_min;
    tnr_input_reg->ar_adj_0mv_max = nr_vir_reg->ar_adj_0mv_max;
    tnr_input_reg->ar_adj_xmv_min = nr_vir_reg->ar_adj_xmv_min;
    tnr_input_reg->ar_adj_xmv_max = nr_vir_reg->ar_adj_xmv_max;
    tnr_input_reg->ar_adj_mv_max = nr_vir_reg->ar_adj_mv_max;
    tnr_input_reg->ar_adj_mv_min = nr_vir_reg->ar_adj_mv_min;
    tnr_input_reg->ar_mc1d_alpha = nr_vir_reg->ar_mc1d_alpha;
    tnr_input_reg->ar_tnr_str = nr_vir_reg->ar_tnr_str;
    tnr_input_reg->ar_cnr_str = nr_vir_reg->ar_cnr_str;

    tnr_input_reg->ar_mcnr_mc_mot_en = nr_vir_reg->mcnr_mc_mot_en ? HI_TRUE : HI_FALSE;
    tnr_input_reg->ar_mc_mt2dfs_adj_en = nr_vir_reg->mc_mt2dfs_adj_en ? HI_TRUE : HI_FALSE;

    tnr_input_reg->y_no_nr_range = nr_vir_reg->y_no_nr_range;
    tnr_input_reg->c_no_nr_range = nr_vir_reg->c_no_nr_range;

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_tnr_y_motion_lut(hi_u32 handle, hi_drv_pq_tnr_input_reg *tnr_input_reg)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(tnr_input_reg);

    tnr_input_reg->y_motion_str_lut[0] = vpss_reg->vpss_hipp_tnr_reg81.bits.y_motion_str_lut_0;
    tnr_input_reg->y_motion_str_lut[1] = vpss_reg->vpss_hipp_tnr_reg81.bits.y_motion_str_lut_1;
    tnr_input_reg->y_motion_str_lut[2] = vpss_reg->vpss_hipp_tnr_reg81.bits.y_motion_str_lut_2; /* 2:index */
    tnr_input_reg->y_motion_str_lut[3] = vpss_reg->vpss_hipp_tnr_reg81.bits.y_motion_str_lut_3; /* 3:index */

    tnr_input_reg->y_motion_str_lut[4] = vpss_reg->vpss_hipp_tnr_reg82.bits.y_motion_str_lut_4; /* 4:index */
    tnr_input_reg->y_motion_str_lut[5] = vpss_reg->vpss_hipp_tnr_reg82.bits.y_motion_str_lut_5; /* 5:index */
    tnr_input_reg->y_motion_str_lut[6] = vpss_reg->vpss_hipp_tnr_reg82.bits.y_motion_str_lut_6; /* 6:index */
    tnr_input_reg->y_motion_str_lut[7] = vpss_reg->vpss_hipp_tnr_reg82.bits.y_motion_str_lut_7; /* 7:index */

    tnr_input_reg->y_motion_str_lut[8] = vpss_reg->vpss_hipp_tnr_reg83.bits.y_motion_str_lut_8; /* 8:index */
    tnr_input_reg->y_motion_str_lut[9] = vpss_reg->vpss_hipp_tnr_reg83.bits.y_motion_str_lut_9; /* 9:index */
    tnr_input_reg->y_motion_str_lut[10] = vpss_reg->vpss_hipp_tnr_reg83.bits.y_motion_str_lut_10; /* 10:index */
    tnr_input_reg->y_motion_str_lut[11] = vpss_reg->vpss_hipp_tnr_reg83.bits.y_motion_str_lut_11; /* 11:index */

    tnr_input_reg->y_motion_str_lut[12] = vpss_reg->vpss_hipp_tnr_reg84.bits.y_motion_str_lut_12; /* 12:index */
    tnr_input_reg->y_motion_str_lut[13] = vpss_reg->vpss_hipp_tnr_reg84.bits.y_motion_str_lut_13; /* 13:index */
    tnr_input_reg->y_motion_str_lut[14] = vpss_reg->vpss_hipp_tnr_reg84.bits.y_motion_str_lut_14; /* 14:index */
    tnr_input_reg->y_motion_str_lut[15] = vpss_reg->vpss_hipp_tnr_reg84.bits.y_motion_str_lut_15; /* 15:index */

    tnr_input_reg->y_motion_str_lut[16] = vpss_reg->vpss_hipp_tnr_reg85.bits.y_motion_str_lut_16; /* 16:index */
    tnr_input_reg->y_motion_str_lut[17] = vpss_reg->vpss_hipp_tnr_reg85.bits.y_motion_str_lut_17; /* 17:index */
    tnr_input_reg->y_motion_str_lut[18] = vpss_reg->vpss_hipp_tnr_reg85.bits.y_motion_str_lut_18; /* 18:index */
    tnr_input_reg->y_motion_str_lut[19] = vpss_reg->vpss_hipp_tnr_reg85.bits.y_motion_str_lut_19; /* 19:index */

    tnr_input_reg->y_motion_str_lut[20] = vpss_reg->vpss_hipp_tnr_reg86.bits.y_motion_str_lut_20; /* 20:index */
    tnr_input_reg->y_motion_str_lut[21] = vpss_reg->vpss_hipp_tnr_reg86.bits.y_motion_str_lut_21; /* 21:index */
    tnr_input_reg->y_motion_str_lut[22] = vpss_reg->vpss_hipp_tnr_reg86.bits.y_motion_str_lut_22; /* 22:index */
    tnr_input_reg->y_motion_str_lut[23] = vpss_reg->vpss_hipp_tnr_reg86.bits.y_motion_str_lut_23; /* 23:index */

    tnr_input_reg->y_motion_str_lut[24] = vpss_reg->vpss_hipp_tnr_reg87.bits.y_motion_str_lut_24; /* 24:index */
    tnr_input_reg->y_motion_str_lut[25] = vpss_reg->vpss_hipp_tnr_reg87.bits.y_motion_str_lut_25; /* 25:index */
    tnr_input_reg->y_motion_str_lut[26] = vpss_reg->vpss_hipp_tnr_reg87.bits.y_motion_str_lut_26; /* 26:index */
    tnr_input_reg->y_motion_str_lut[27] = vpss_reg->vpss_hipp_tnr_reg87.bits.y_motion_str_lut_27; /* 27:index */

    tnr_input_reg->y_motion_str_lut[28] = vpss_reg->vpss_hipp_tnr_reg88.bits.y_motion_str_lut_28; /* 28:index */
    tnr_input_reg->y_motion_str_lut[29] = vpss_reg->vpss_hipp_tnr_reg88.bits.y_motion_str_lut_29; /* 29:index */
    tnr_input_reg->y_motion_str_lut[30] = vpss_reg->vpss_hipp_tnr_reg88.bits.y_motion_str_lut_30; /* 30:index */
    tnr_input_reg->y_motion_str_lut[31] = vpss_reg->vpss_hipp_tnr_reg88.bits.y_motion_str_lut_31; /* 31:index */

    tnr_input_reg->y_motion_str_lut[32] = vpss_reg->vpss_hipp_tnr_reg89.bits.y_motion_str_lut_32; /* 32:index */
    tnr_input_reg->y_motion_str_lut[33] = vpss_reg->vpss_hipp_tnr_reg89.bits.y_motion_str_lut_33; /* 33:index */
    tnr_input_reg->y_motion_str_lut[34] = vpss_reg->vpss_hipp_tnr_reg89.bits.y_motion_str_lut_34; /* 34:index */
    tnr_input_reg->y_motion_str_lut[35] = vpss_reg->vpss_hipp_tnr_reg89.bits.y_motion_str_lut_35; /* 35:index */

    tnr_input_reg->y_motion_str_lut[36] = vpss_reg->vpss_hipp_tnr_reg90.bits.y_motion_str_lut_36; /* 36:index */
    tnr_input_reg->y_motion_str_lut[37] = vpss_reg->vpss_hipp_tnr_reg90.bits.y_motion_str_lut_37; /* 37:index */
    tnr_input_reg->y_motion_str_lut[38] = vpss_reg->vpss_hipp_tnr_reg90.bits.y_motion_str_lut_38; /* 38:index */
    tnr_input_reg->y_motion_str_lut[39] = vpss_reg->vpss_hipp_tnr_reg90.bits.y_motion_str_lut_39; /* 39:index */

    tnr_input_reg->y_motion_str_lut[40] = vpss_reg->vpss_hipp_tnr_reg91.bits.y_motion_str_lut_40; /* 40:index */
    tnr_input_reg->y_motion_str_lut[41] = vpss_reg->vpss_hipp_tnr_reg91.bits.y_motion_str_lut_41; /* 41:index */
    tnr_input_reg->y_motion_str_lut[42] = vpss_reg->vpss_hipp_tnr_reg91.bits.y_motion_str_lut_42; /* 42:index */
    tnr_input_reg->y_motion_str_lut[43] = vpss_reg->vpss_hipp_tnr_reg91.bits.y_motion_str_lut_43; /* 43:index */

    tnr_input_reg->y_motion_str_lut[44] = vpss_reg->vpss_hipp_tnr_reg92.bits.y_motion_str_lut_44; /* 44:index */
    tnr_input_reg->y_motion_str_lut[45] = vpss_reg->vpss_hipp_tnr_reg92.bits.y_motion_str_lut_45; /* 45:index */
    tnr_input_reg->y_motion_str_lut[46] = vpss_reg->vpss_hipp_tnr_reg92.bits.y_motion_str_lut_46; /* 46:index */
    tnr_input_reg->y_motion_str_lut[47] = vpss_reg->vpss_hipp_tnr_reg92.bits.y_motion_str_lut_47; /* 47:index */

    tnr_input_reg->y_motion_str_lut[48] = vpss_reg->vpss_hipp_tnr_reg93.bits.y_motion_str_lut_48; /* 48:index */
    tnr_input_reg->y_motion_str_lut[49] = vpss_reg->vpss_hipp_tnr_reg93.bits.y_motion_str_lut_49; /* 49:index */
    tnr_input_reg->y_motion_str_lut[50] = vpss_reg->vpss_hipp_tnr_reg93.bits.y_motion_str_lut_50; /* 50:index */
    tnr_input_reg->y_motion_str_lut[51] = vpss_reg->vpss_hipp_tnr_reg93.bits.y_motion_str_lut_51; /* 51:index */

    tnr_input_reg->y_motion_str_lut[52] = vpss_reg->vpss_hipp_tnr_reg94.bits.y_motion_str_lut_52; /* 52:index */
    tnr_input_reg->y_motion_str_lut[53] = vpss_reg->vpss_hipp_tnr_reg94.bits.y_motion_str_lut_53; /* 53:index */
    tnr_input_reg->y_motion_str_lut[54] = vpss_reg->vpss_hipp_tnr_reg94.bits.y_motion_str_lut_54; /* 54:index */
    tnr_input_reg->y_motion_str_lut[55] = vpss_reg->vpss_hipp_tnr_reg94.bits.y_motion_str_lut_55; /* 55:index */

    tnr_input_reg->y_motion_str_lut[56] = vpss_reg->vpss_hipp_tnr_reg95.bits.y_motion_str_lut_56; /* 56:index */
    tnr_input_reg->y_motion_str_lut[57] = vpss_reg->vpss_hipp_tnr_reg95.bits.y_motion_str_lut_57; /* 57:index */
    tnr_input_reg->y_motion_str_lut[58] = vpss_reg->vpss_hipp_tnr_reg95.bits.y_motion_str_lut_58; /* 58:index */
    tnr_input_reg->y_motion_str_lut[59] = vpss_reg->vpss_hipp_tnr_reg95.bits.y_motion_str_lut_59; /* 59:index */

    tnr_input_reg->y_motion_str_lut[60] = vpss_reg->vpss_hipp_tnr_reg96.bits.y_motion_str_lut_60; /* 60:index */
    tnr_input_reg->y_motion_str_lut[61] = vpss_reg->vpss_hipp_tnr_reg96.bits.y_motion_str_lut_61; /* 61:index */
    tnr_input_reg->y_motion_str_lut[62] = vpss_reg->vpss_hipp_tnr_reg96.bits.y_motion_str_lut_62; /* 62:index */
    tnr_input_reg->y_motion_str_lut[63] = vpss_reg->vpss_hipp_tnr_reg96.bits.y_motion_str_lut_63; /* 63:index */

    tnr_input_reg->y_motion_str_lut[64] = vpss_reg->vpss_hipp_tnr_reg97.bits.y_motion_str_lut_64;     /* 64:index */
    tnr_input_reg->y_motion_str_lut[65] = vpss_reg->vpss_hipp_tnr_reg97.bits.y_motion_str_lut_65;     /* 65:index */
    tnr_input_reg->y_motion_str_lut[66] = vpss_reg->vpss_hipp_tnr_reg97.bits.y_motion_str_lut_66;     /* 66:index */
    tnr_input_reg->y_motion_str_lut[67] = vpss_reg->vpss_hipp_tnr_reg97.bits.y_motion_str_lut_67;     /* 67:index */

    tnr_input_reg->y_motion_str_lut[68] = vpss_reg->vpss_hipp_tnr_reg98.bits.y_motion_str_lut_68;     /* 68:index */
    tnr_input_reg->y_motion_str_lut[69] = vpss_reg->vpss_hipp_tnr_reg98.bits.y_motion_str_lut_69;     /* 69:index */
    tnr_input_reg->y_motion_str_lut[70] = vpss_reg->vpss_hipp_tnr_reg98.bits.y_motion_str_lut_70;     /* 70:index */
    tnr_input_reg->y_motion_str_lut[71] = vpss_reg->vpss_hipp_tnr_reg98.bits.y_motion_str_lut_71;     /* 71:index */

    tnr_input_reg->y_motion_str_lut[72] = vpss_reg->vpss_hipp_tnr_reg99.bits.y_motion_str_lut_72;     /* 72:index */
    tnr_input_reg->y_motion_str_lut[73] = vpss_reg->vpss_hipp_tnr_reg99.bits.y_motion_str_lut_73;     /* 73:index */
    tnr_input_reg->y_motion_str_lut[74] = vpss_reg->vpss_hipp_tnr_reg99.bits.y_motion_str_lut_74;     /* 74:index */
    tnr_input_reg->y_motion_str_lut[75] = vpss_reg->vpss_hipp_tnr_reg99.bits.y_motion_str_lut_75;     /* 75:index */

    tnr_input_reg->y_motion_str_lut[76] = vpss_reg->vpss_hipp_tnr_reg100.bits.y_motion_str_lut_76;    /* 76:index */
    tnr_input_reg->y_motion_str_lut[77] = vpss_reg->vpss_hipp_tnr_reg100.bits.y_motion_str_lut_77;    /* 77:index */
    tnr_input_reg->y_motion_str_lut[78] = vpss_reg->vpss_hipp_tnr_reg100.bits.y_motion_str_lut_78;    /* 78:index */
    tnr_input_reg->y_motion_str_lut[79] = vpss_reg->vpss_hipp_tnr_reg100.bits.y_motion_str_lut_79;    /* 79:index */

    tnr_input_reg->y_motion_str_lut[80] = vpss_reg->vpss_hipp_tnr_reg101.bits.y_motion_str_lut_80;    /* 80:index */
    tnr_input_reg->y_motion_str_lut[81] = vpss_reg->vpss_hipp_tnr_reg101.bits.y_motion_str_lut_81;    /* 81:index */
    tnr_input_reg->y_motion_str_lut[82] = vpss_reg->vpss_hipp_tnr_reg101.bits.y_motion_str_lut_82;    /* 82:index */
    tnr_input_reg->y_motion_str_lut[83] = vpss_reg->vpss_hipp_tnr_reg101.bits.y_motion_str_lut_83;    /* 83:index */

    tnr_input_reg->y_motion_str_lut[84] = vpss_reg->vpss_hipp_tnr_reg102.bits.y_motion_str_lut_84;    /* 84:index */
    tnr_input_reg->y_motion_str_lut[85] = vpss_reg->vpss_hipp_tnr_reg102.bits.y_motion_str_lut_85;    /* 85:index */
    tnr_input_reg->y_motion_str_lut[86] = vpss_reg->vpss_hipp_tnr_reg102.bits.y_motion_str_lut_86;    /* 86:index */
    tnr_input_reg->y_motion_str_lut[87] = vpss_reg->vpss_hipp_tnr_reg102.bits.y_motion_str_lut_87;    /* 87:index */

    tnr_input_reg->y_motion_str_lut[88] = vpss_reg->vpss_hipp_tnr_reg103.bits.y_motion_str_lut_88;    /* 88:index */
    tnr_input_reg->y_motion_str_lut[89] = vpss_reg->vpss_hipp_tnr_reg103.bits.y_motion_str_lut_89;    /* 89:index */
    tnr_input_reg->y_motion_str_lut[90] = vpss_reg->vpss_hipp_tnr_reg103.bits.y_motion_str_lut_90;    /* 90:index */
    tnr_input_reg->y_motion_str_lut[91] = vpss_reg->vpss_hipp_tnr_reg103.bits.y_motion_str_lut_91;    /* 91:index */

    tnr_input_reg->y_motion_str_lut[92] = vpss_reg->vpss_hipp_tnr_reg104.bits.y_motion_str_lut_92;    /* 92:index */
    tnr_input_reg->y_motion_str_lut[93] = vpss_reg->vpss_hipp_tnr_reg104.bits.y_motion_str_lut_93;    /* 93:index */
    tnr_input_reg->y_motion_str_lut[94] = vpss_reg->vpss_hipp_tnr_reg104.bits.y_motion_str_lut_94;    /* 94:index */
    tnr_input_reg->y_motion_str_lut[95] = vpss_reg->vpss_hipp_tnr_reg104.bits.y_motion_str_lut_95;    /* 95:index */

    tnr_input_reg->y_motion_str_lut[96] = vpss_reg->vpss_hipp_tnr_reg105.bits.y_motion_str_lut_96;    /* 96:index */
    tnr_input_reg->y_motion_str_lut[97] = vpss_reg->vpss_hipp_tnr_reg105.bits.y_motion_str_lut_97;    /* 97:index */
    tnr_input_reg->y_motion_str_lut[98] = vpss_reg->vpss_hipp_tnr_reg105.bits.y_motion_str_lut_98;    /* 98:index */
    tnr_input_reg->y_motion_str_lut[99] = vpss_reg->vpss_hipp_tnr_reg105.bits.y_motion_str_lut_99;    /* 99:index */

    tnr_input_reg->y_motion_str_lut[100] = vpss_reg->vpss_hipp_tnr_reg106.bits.y_motion_str_lut_100;  /* 100:index */
    tnr_input_reg->y_motion_str_lut[101] = vpss_reg->vpss_hipp_tnr_reg106.bits.y_motion_str_lut_101;  /* 101:index */
    tnr_input_reg->y_motion_str_lut[102] = vpss_reg->vpss_hipp_tnr_reg106.bits.y_motion_str_lut_102;  /* 102:index */
    tnr_input_reg->y_motion_str_lut[103] = vpss_reg->vpss_hipp_tnr_reg106.bits.y_motion_str_lut_103;  /* 103:index */

    tnr_input_reg->y_motion_str_lut[104] = vpss_reg->vpss_hipp_tnr_reg107.bits.y_motion_str_lut_104;  /* 104:index */
    tnr_input_reg->y_motion_str_lut[105] = vpss_reg->vpss_hipp_tnr_reg107.bits.y_motion_str_lut_105;  /* 105:index */
    tnr_input_reg->y_motion_str_lut[106] = vpss_reg->vpss_hipp_tnr_reg107.bits.y_motion_str_lut_106;  /* 106:index */
    tnr_input_reg->y_motion_str_lut[107] = vpss_reg->vpss_hipp_tnr_reg107.bits.y_motion_str_lut_107;  /* 107:index */

    tnr_input_reg->y_motion_str_lut[108] = vpss_reg->vpss_hipp_tnr_reg108.bits.y_motion_str_lut_108;  /* 108:index */
    tnr_input_reg->y_motion_str_lut[109] = vpss_reg->vpss_hipp_tnr_reg108.bits.y_motion_str_lut_109;  /* 109:index */
    tnr_input_reg->y_motion_str_lut[110] = vpss_reg->vpss_hipp_tnr_reg108.bits.y_motion_str_lut_110;  /* 110:index */
    tnr_input_reg->y_motion_str_lut[111] = vpss_reg->vpss_hipp_tnr_reg108.bits.y_motion_str_lut_111;  /* 111:index */

    tnr_input_reg->y_motion_str_lut[112] = vpss_reg->vpss_hipp_tnr_reg109.bits.y_motion_str_lut_112;  /* 112:index */
    tnr_input_reg->y_motion_str_lut[113] = vpss_reg->vpss_hipp_tnr_reg109.bits.y_motion_str_lut_113;  /* 113:index */
    tnr_input_reg->y_motion_str_lut[114] = vpss_reg->vpss_hipp_tnr_reg109.bits.y_motion_str_lut_114;  /* 114:index */
    tnr_input_reg->y_motion_str_lut[115] = vpss_reg->vpss_hipp_tnr_reg109.bits.y_motion_str_lut_115;  /* 115:index */

    tnr_input_reg->y_motion_str_lut[116] = vpss_reg->vpss_hipp_tnr_reg110.bits.y_motion_str_lut_116;  /* 116:index */
    tnr_input_reg->y_motion_str_lut[117] = vpss_reg->vpss_hipp_tnr_reg110.bits.y_motion_str_lut_117;  /* 117:index */
    tnr_input_reg->y_motion_str_lut[118] = vpss_reg->vpss_hipp_tnr_reg110.bits.y_motion_str_lut_118;  /* 118:index */
    tnr_input_reg->y_motion_str_lut[119] = vpss_reg->vpss_hipp_tnr_reg110.bits.y_motion_str_lut_119;  /* 119:index */

    tnr_input_reg->y_motion_str_lut[120] = vpss_reg->vpss_hipp_tnr_reg111.bits.y_motion_str_lut_120;  /* 120:index */
    tnr_input_reg->y_motion_str_lut[121] = vpss_reg->vpss_hipp_tnr_reg111.bits.y_motion_str_lut_121;  /* 121:index */
    tnr_input_reg->y_motion_str_lut[122] = vpss_reg->vpss_hipp_tnr_reg111.bits.y_motion_str_lut_122;  /* 122:index */
    tnr_input_reg->y_motion_str_lut[123] = vpss_reg->vpss_hipp_tnr_reg111.bits.y_motion_str_lut_123;  /* 123:index */

    tnr_input_reg->y_motion_str_lut[124] = vpss_reg->vpss_hipp_tnr_reg112.bits.y_motion_str_lut_124;  /* 124:index */
    tnr_input_reg->y_motion_str_lut[125] = vpss_reg->vpss_hipp_tnr_reg112.bits.y_motion_str_lut_125;  /* 125:index */
    tnr_input_reg->y_motion_str_lut[126] = vpss_reg->vpss_hipp_tnr_reg112.bits.y_motion_str_lut_126;  /* 126:index */
    tnr_input_reg->y_motion_str_lut[127] = vpss_reg->vpss_hipp_tnr_reg112.bits.y_motion_str_lut_127;  /* 127:index */

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_tnr_c_motion_lut(hi_u32 handle, hi_drv_pq_tnr_input_reg *tnr_input_reg)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(tnr_input_reg);

    tnr_input_reg->c_motion_str_lut[0] = vpss_reg->vpss_hipp_tnr_reg113.bits.motion_str_lut_0;
    tnr_input_reg->c_motion_str_lut[1] = vpss_reg->vpss_hipp_tnr_reg113.bits.motion_str_lut_1;
    tnr_input_reg->c_motion_str_lut[2] = vpss_reg->vpss_hipp_tnr_reg113.bits.motion_str_lut_2; /* 2:index */
    tnr_input_reg->c_motion_str_lut[3] = vpss_reg->vpss_hipp_tnr_reg113.bits.motion_str_lut_3; /* 3:index */

    tnr_input_reg->c_motion_str_lut[4] = vpss_reg->vpss_hipp_tnr_reg114.bits.motion_str_lut_4; /* 4:index */
    tnr_input_reg->c_motion_str_lut[5] = vpss_reg->vpss_hipp_tnr_reg114.bits.motion_str_lut_5; /* 5:index */
    tnr_input_reg->c_motion_str_lut[6] = vpss_reg->vpss_hipp_tnr_reg114.bits.motion_str_lut_6; /* 6:index */
    tnr_input_reg->c_motion_str_lut[7] = vpss_reg->vpss_hipp_tnr_reg114.bits.motion_str_lut_7; /* 7:index */

    tnr_input_reg->c_motion_str_lut[8] = vpss_reg->vpss_hipp_tnr_reg115.bits.motion_str_lut_8; /* 8:index */
    tnr_input_reg->c_motion_str_lut[9] = vpss_reg->vpss_hipp_tnr_reg115.bits.motion_str_lut_9; /* 9:index */
    tnr_input_reg->c_motion_str_lut[10] = vpss_reg->vpss_hipp_tnr_reg115.bits.motion_str_lut_10;   /* 10:index */
    tnr_input_reg->c_motion_str_lut[11] = vpss_reg->vpss_hipp_tnr_reg115.bits.motion_str_lut_11;   /* 11:index */

    tnr_input_reg->c_motion_str_lut[12] = vpss_reg->vpss_hipp_tnr_reg116.bits.motion_str_lut_12;   /* 12:index */
    tnr_input_reg->c_motion_str_lut[13] = vpss_reg->vpss_hipp_tnr_reg116.bits.motion_str_lut_13;   /* 13:index */
    tnr_input_reg->c_motion_str_lut[14] = vpss_reg->vpss_hipp_tnr_reg116.bits.motion_str_lut_14;   /* 14:index */
    tnr_input_reg->c_motion_str_lut[15] = vpss_reg->vpss_hipp_tnr_reg116.bits.motion_str_lut_15;   /* 15:index */

    tnr_input_reg->c_motion_str_lut[16] = vpss_reg->vpss_hipp_tnr_reg117.bits.motion_str_lut_16;   /* 16:index */
    tnr_input_reg->c_motion_str_lut[17] = vpss_reg->vpss_hipp_tnr_reg117.bits.motion_str_lut_17;   /* 17:index */
    tnr_input_reg->c_motion_str_lut[18] = vpss_reg->vpss_hipp_tnr_reg117.bits.motion_str_lut_18;   /* 18:index */
    tnr_input_reg->c_motion_str_lut[19] = vpss_reg->vpss_hipp_tnr_reg117.bits.motion_str_lut_19;   /* 19:index */

    tnr_input_reg->c_motion_str_lut[20] = vpss_reg->vpss_hipp_tnr_reg118.bits.motion_str_lut_20;   /* 20:index */
    tnr_input_reg->c_motion_str_lut[21] = vpss_reg->vpss_hipp_tnr_reg118.bits.motion_str_lut_21;   /* 21:index */
    tnr_input_reg->c_motion_str_lut[22] = vpss_reg->vpss_hipp_tnr_reg118.bits.motion_str_lut_22;   /* 22:index */
    tnr_input_reg->c_motion_str_lut[23] = vpss_reg->vpss_hipp_tnr_reg118.bits.motion_str_lut_23;   /* 23:index */

    tnr_input_reg->c_motion_str_lut[24] = vpss_reg->vpss_hipp_tnr_reg119.bits.motion_str_lut_24;   /* 24:index */
    tnr_input_reg->c_motion_str_lut[25] = vpss_reg->vpss_hipp_tnr_reg119.bits.motion_str_lut_25;   /* 25:index */
    tnr_input_reg->c_motion_str_lut[26] = vpss_reg->vpss_hipp_tnr_reg119.bits.motion_str_lut_26;   /* 26:index */
    tnr_input_reg->c_motion_str_lut[27] = vpss_reg->vpss_hipp_tnr_reg119.bits.motion_str_lut_27;   /* 27:index */

    tnr_input_reg->c_motion_str_lut[28] = vpss_reg->vpss_hipp_tnr_reg120.bits.motion_str_lut_28;   /* 28:index */
    tnr_input_reg->c_motion_str_lut[29] = vpss_reg->vpss_hipp_tnr_reg120.bits.motion_str_lut_29;   /* 29:index */
    tnr_input_reg->c_motion_str_lut[30] = vpss_reg->vpss_hipp_tnr_reg120.bits.motion_str_lut_30;   /* 30:index */
    tnr_input_reg->c_motion_str_lut[31] = vpss_reg->vpss_hipp_tnr_reg120.bits.motion_str_lut_31;   /* 31:index */

    tnr_input_reg->c_motion_str_lut[32] = vpss_reg->vpss_hipp_tnr_reg121.bits.motion_str_lut_32;   /* 32:index */
    tnr_input_reg->c_motion_str_lut[33] = vpss_reg->vpss_hipp_tnr_reg121.bits.motion_str_lut_33;   /* 33:index */
    tnr_input_reg->c_motion_str_lut[34] = vpss_reg->vpss_hipp_tnr_reg121.bits.motion_str_lut_34;   /* 34:index */
    tnr_input_reg->c_motion_str_lut[35] = vpss_reg->vpss_hipp_tnr_reg121.bits.motion_str_lut_35;   /* 35:index */

    tnr_input_reg->c_motion_str_lut[36] = vpss_reg->vpss_hipp_tnr_reg122.bits.motion_str_lut_36;   /* 36:index */
    tnr_input_reg->c_motion_str_lut[37] = vpss_reg->vpss_hipp_tnr_reg122.bits.motion_str_lut_37;   /* 37:index */
    tnr_input_reg->c_motion_str_lut[38] = vpss_reg->vpss_hipp_tnr_reg122.bits.motion_str_lut_38;   /* 38:index */
    tnr_input_reg->c_motion_str_lut[39] = vpss_reg->vpss_hipp_tnr_reg122.bits.motion_str_lut_39;   /* 39:index */

    tnr_input_reg->c_motion_str_lut[40] = vpss_reg->vpss_hipp_tnr_reg123.bits.motion_str_lut_40;   /* 40:index */
    tnr_input_reg->c_motion_str_lut[41] = vpss_reg->vpss_hipp_tnr_reg123.bits.motion_str_lut_41;   /* 41:index */
    tnr_input_reg->c_motion_str_lut[42] = vpss_reg->vpss_hipp_tnr_reg123.bits.motion_str_lut_42;   /* 42:index */
    tnr_input_reg->c_motion_str_lut[43] = vpss_reg->vpss_hipp_tnr_reg123.bits.motion_str_lut_43;   /* 43:index */

    tnr_input_reg->c_motion_str_lut[44] = vpss_reg->vpss_hipp_tnr_reg124.bits.motion_str_lut_44;   /* 44:index */
    tnr_input_reg->c_motion_str_lut[45] = vpss_reg->vpss_hipp_tnr_reg124.bits.motion_str_lut_45;   /* 45:index */
    tnr_input_reg->c_motion_str_lut[46] = vpss_reg->vpss_hipp_tnr_reg124.bits.motion_str_lut_46;   /* 46:index */
    tnr_input_reg->c_motion_str_lut[47] = vpss_reg->vpss_hipp_tnr_reg124.bits.motion_str_lut_47;   /* 47:index */

    tnr_input_reg->c_motion_str_lut[48] = vpss_reg->vpss_hipp_tnr_reg125.bits.motion_str_lut_48;   /* 48:index */
    tnr_input_reg->c_motion_str_lut[49] = vpss_reg->vpss_hipp_tnr_reg125.bits.motion_str_lut_49;   /* 49:index */
    tnr_input_reg->c_motion_str_lut[50] = vpss_reg->vpss_hipp_tnr_reg125.bits.motion_str_lut_50;   /* 50:index */
    tnr_input_reg->c_motion_str_lut[51] = vpss_reg->vpss_hipp_tnr_reg125.bits.motion_str_lut_51;   /* 51:index */

    tnr_input_reg->c_motion_str_lut[52] = vpss_reg->vpss_hipp_tnr_reg126.bits.motion_str_lut_52;   /* 52:index */
    tnr_input_reg->c_motion_str_lut[53] = vpss_reg->vpss_hipp_tnr_reg126.bits.motion_str_lut_53;   /* 53:index */
    tnr_input_reg->c_motion_str_lut[54] = vpss_reg->vpss_hipp_tnr_reg126.bits.motion_str_lut_54;   /* 54:index */
    tnr_input_reg->c_motion_str_lut[55] = vpss_reg->vpss_hipp_tnr_reg126.bits.motion_str_lut_55;   /* 55:index */

    tnr_input_reg->c_motion_str_lut[56] = vpss_reg->vpss_hipp_tnr_reg127.bits.motion_str_lut_56;   /* 56:index */
    tnr_input_reg->c_motion_str_lut[57] = vpss_reg->vpss_hipp_tnr_reg127.bits.motion_str_lut_57;   /* 57:index */
    tnr_input_reg->c_motion_str_lut[58] = vpss_reg->vpss_hipp_tnr_reg127.bits.motion_str_lut_58;   /* 58:index */
    tnr_input_reg->c_motion_str_lut[59] = vpss_reg->vpss_hipp_tnr_reg127.bits.motion_str_lut_59;   /* 59:index */

    tnr_input_reg->c_motion_str_lut[60] = vpss_reg->vpss_hipp_tnr_reg128.bits.motion_str_lut_60;   /* 60:index */
    tnr_input_reg->c_motion_str_lut[61] = vpss_reg->vpss_hipp_tnr_reg128.bits.motion_str_lut_61;   /* 61:index */
    tnr_input_reg->c_motion_str_lut[62] = vpss_reg->vpss_hipp_tnr_reg128.bits.motion_str_lut_62;   /* 62:index */
    tnr_input_reg->c_motion_str_lut[63] = vpss_reg->vpss_hipp_tnr_reg128.bits.motion_str_lut_63;   /* 63:index */

    tnr_input_reg->c_motion_str_lut[64] = vpss_reg->vpss_hipp_tnr_reg129.bits.motion_str_lut_64;   /* 64:index */
    tnr_input_reg->c_motion_str_lut[65] = vpss_reg->vpss_hipp_tnr_reg129.bits.motion_str_lut_65;   /* 65:index */
    tnr_input_reg->c_motion_str_lut[66] = vpss_reg->vpss_hipp_tnr_reg129.bits.motion_str_lut_66;   /* 66:index */
    tnr_input_reg->c_motion_str_lut[67] = vpss_reg->vpss_hipp_tnr_reg129.bits.motion_str_lut_67;   /* 67:index */

    tnr_input_reg->c_motion_str_lut[68] = vpss_reg->vpss_hipp_tnr_reg130.bits.motion_str_lut_68;   /* 68:index */
    tnr_input_reg->c_motion_str_lut[69] = vpss_reg->vpss_hipp_tnr_reg130.bits.motion_str_lut_69;   /* 69:index */
    tnr_input_reg->c_motion_str_lut[70] = vpss_reg->vpss_hipp_tnr_reg130.bits.motion_str_lut_70;   /* 70:index */
    tnr_input_reg->c_motion_str_lut[71] = vpss_reg->vpss_hipp_tnr_reg130.bits.motion_str_lut_71;   /* 71:index */

    tnr_input_reg->c_motion_str_lut[72] = vpss_reg->vpss_hipp_tnr_reg131.bits.motion_str_lut_72;   /* 72:index */
    tnr_input_reg->c_motion_str_lut[73] = vpss_reg->vpss_hipp_tnr_reg131.bits.motion_str_lut_73;   /* 73:index */
    tnr_input_reg->c_motion_str_lut[74] = vpss_reg->vpss_hipp_tnr_reg131.bits.motion_str_lut_74;   /* 74:index */
    tnr_input_reg->c_motion_str_lut[75] = vpss_reg->vpss_hipp_tnr_reg131.bits.motion_str_lut_75;   /* 75:index */

    tnr_input_reg->c_motion_str_lut[76] = vpss_reg->vpss_hipp_tnr_reg132.bits.motion_str_lut_76;   /* 76:index */
    tnr_input_reg->c_motion_str_lut[77] = vpss_reg->vpss_hipp_tnr_reg132.bits.motion_str_lut_77;   /* 77:index */
    tnr_input_reg->c_motion_str_lut[78] = vpss_reg->vpss_hipp_tnr_reg132.bits.motion_str_lut_78;   /* 78:index */
    tnr_input_reg->c_motion_str_lut[79] = vpss_reg->vpss_hipp_tnr_reg132.bits.motion_str_lut_79;   /* 79:index */

    tnr_input_reg->c_motion_str_lut[80] = vpss_reg->vpss_hipp_tnr_reg133.bits.motion_str_lut_80;   /* 80:index */
    tnr_input_reg->c_motion_str_lut[81] = vpss_reg->vpss_hipp_tnr_reg133.bits.motion_str_lut_81;   /* 81:index */
    tnr_input_reg->c_motion_str_lut[82] = vpss_reg->vpss_hipp_tnr_reg133.bits.motion_str_lut_82;   /* 82:index */
    tnr_input_reg->c_motion_str_lut[83] = vpss_reg->vpss_hipp_tnr_reg133.bits.motion_str_lut_83;   /* 83:index */

    tnr_input_reg->c_motion_str_lut[84] = vpss_reg->vpss_hipp_tnr_reg134.bits.motion_str_lut_84;   /* 84:index */
    tnr_input_reg->c_motion_str_lut[85] = vpss_reg->vpss_hipp_tnr_reg134.bits.motion_str_lut_85;   /* 85:index */
    tnr_input_reg->c_motion_str_lut[86] = vpss_reg->vpss_hipp_tnr_reg134.bits.motion_str_lut_86;   /* 86:index */
    tnr_input_reg->c_motion_str_lut[87] = vpss_reg->vpss_hipp_tnr_reg134.bits.motion_str_lut_87;   /* 87:index */

    tnr_input_reg->c_motion_str_lut[88] = vpss_reg->vpss_hipp_tnr_reg135.bits.motion_str_lut_88;   /* 88:index */
    tnr_input_reg->c_motion_str_lut[89] = vpss_reg->vpss_hipp_tnr_reg135.bits.motion_str_lut_89;   /* 89:index */
    tnr_input_reg->c_motion_str_lut[90] = vpss_reg->vpss_hipp_tnr_reg135.bits.motion_str_lut_90;   /* 90:index */
    tnr_input_reg->c_motion_str_lut[91] = vpss_reg->vpss_hipp_tnr_reg135.bits.motion_str_lut_91;   /* 91:index */

    tnr_input_reg->c_motion_str_lut[92] = vpss_reg->vpss_hipp_tnr_reg136.bits.motion_str_lut_92;   /* 92:index */
    tnr_input_reg->c_motion_str_lut[93] = vpss_reg->vpss_hipp_tnr_reg136.bits.motion_str_lut_93;   /* 93:index */
    tnr_input_reg->c_motion_str_lut[94] = vpss_reg->vpss_hipp_tnr_reg136.bits.motion_str_lut_94;   /* 94:index */
    tnr_input_reg->c_motion_str_lut[95] = vpss_reg->vpss_hipp_tnr_reg136.bits.motion_str_lut_95;   /* 95:index */

    tnr_input_reg->c_motion_str_lut[96] = vpss_reg->vpss_hipp_tnr_reg137.bits.motion_str_lut_96;   /* 96:index */
    tnr_input_reg->c_motion_str_lut[97] = vpss_reg->vpss_hipp_tnr_reg137.bits.motion_str_lut_97;   /* 97:index */
    tnr_input_reg->c_motion_str_lut[98] = vpss_reg->vpss_hipp_tnr_reg137.bits.motion_str_lut_98;   /* 98:index */
    tnr_input_reg->c_motion_str_lut[99] = vpss_reg->vpss_hipp_tnr_reg137.bits.motion_str_lut_99;   /* 99:index */

    tnr_input_reg->c_motion_str_lut[100] = vpss_reg->vpss_hipp_tnr_reg138.bits.motion_str_lut_100; /* 100:index */
    tnr_input_reg->c_motion_str_lut[101] = vpss_reg->vpss_hipp_tnr_reg138.bits.motion_str_lut_101; /* 101:index */
    tnr_input_reg->c_motion_str_lut[102] = vpss_reg->vpss_hipp_tnr_reg138.bits.motion_str_lut_102; /* 102:index */
    tnr_input_reg->c_motion_str_lut[103] = vpss_reg->vpss_hipp_tnr_reg138.bits.motion_str_lut_103; /* 103:index */

    tnr_input_reg->c_motion_str_lut[104] = vpss_reg->vpss_hipp_tnr_reg139.bits.motion_str_lut_104; /* 104:index */
    tnr_input_reg->c_motion_str_lut[105] = vpss_reg->vpss_hipp_tnr_reg139.bits.motion_str_lut_105; /* 105:index */
    tnr_input_reg->c_motion_str_lut[106] = vpss_reg->vpss_hipp_tnr_reg139.bits.motion_str_lut_106; /* 106:index */
    tnr_input_reg->c_motion_str_lut[107] = vpss_reg->vpss_hipp_tnr_reg139.bits.motion_str_lut_107; /* 107:index */

    tnr_input_reg->c_motion_str_lut[108] = vpss_reg->vpss_hipp_tnr_reg140.bits.motion_str_lut_108; /* 108:index */
    tnr_input_reg->c_motion_str_lut[109] = vpss_reg->vpss_hipp_tnr_reg140.bits.motion_str_lut_109; /* 109:index */
    tnr_input_reg->c_motion_str_lut[110] = vpss_reg->vpss_hipp_tnr_reg140.bits.motion_str_lut_110; /* 110:index */
    tnr_input_reg->c_motion_str_lut[111] = vpss_reg->vpss_hipp_tnr_reg140.bits.motion_str_lut_111; /* 111:index */

    tnr_input_reg->c_motion_str_lut[112] = vpss_reg->vpss_hipp_tnr_reg141.bits.motion_str_lut_112; /* 112:index */
    tnr_input_reg->c_motion_str_lut[113] = vpss_reg->vpss_hipp_tnr_reg141.bits.motion_str_lut_113; /* 113:index */
    tnr_input_reg->c_motion_str_lut[114] = vpss_reg->vpss_hipp_tnr_reg141.bits.motion_str_lut_114; /* 114:index */
    tnr_input_reg->c_motion_str_lut[115] = vpss_reg->vpss_hipp_tnr_reg141.bits.motion_str_lut_115; /* 115:index */

    tnr_input_reg->c_motion_str_lut[116] = vpss_reg->vpss_hipp_tnr_reg142.bits.motion_str_lut_116; /* 116:index */
    tnr_input_reg->c_motion_str_lut[117] = vpss_reg->vpss_hipp_tnr_reg142.bits.motion_str_lut_117; /* 117:index */
    tnr_input_reg->c_motion_str_lut[118] = vpss_reg->vpss_hipp_tnr_reg142.bits.motion_str_lut_118; /* 118:index */
    tnr_input_reg->c_motion_str_lut[119] = vpss_reg->vpss_hipp_tnr_reg142.bits.motion_str_lut_119; /* 119:index */

    tnr_input_reg->c_motion_str_lut[120] = vpss_reg->vpss_hipp_tnr_reg143.bits.motion_str_lut_120; /* 120:index */
    tnr_input_reg->c_motion_str_lut[121] = vpss_reg->vpss_hipp_tnr_reg143.bits.motion_str_lut_121; /* 121:index */
    tnr_input_reg->c_motion_str_lut[122] = vpss_reg->vpss_hipp_tnr_reg143.bits.motion_str_lut_122; /* 122:index */
    tnr_input_reg->c_motion_str_lut[123] = vpss_reg->vpss_hipp_tnr_reg143.bits.motion_str_lut_123; /* 123:index */

    tnr_input_reg->c_motion_str_lut[124] = vpss_reg->vpss_hipp_tnr_reg144.bits.motion_str_lut_124; /* 124:index */
    tnr_input_reg->c_motion_str_lut[125] = vpss_reg->vpss_hipp_tnr_reg144.bits.motion_str_lut_125; /* 125:index */
    tnr_input_reg->c_motion_str_lut[126] = vpss_reg->vpss_hipp_tnr_reg144.bits.motion_str_lut_126; /* 126:index */
    tnr_input_reg->c_motion_str_lut[127] = vpss_reg->vpss_hipp_tnr_reg144.bits.motion_str_lut_127; /* 127:index */

    tnr_input_reg->motion_estimation_en = vpss_reg->vpss_hipp_tnr_reg62.bits.motion_estimate_en;
    tnr_input_reg->noise_detect_en = vpss_reg->vpss_hipp_tnr_reg37.bits.noise_detect_en;
    tnr_input_reg->mcnr_en = vpss_reg->vpss_ctrl.bits.mcnr_en;

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_tnr_func_enable(hi_u32 handle, hi_drv_pq_tnr_input_reg *tnr_input_reg)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(tnr_input_reg);

    tnr_input_reg->motion_estimation_en = vpss_reg->vpss_hipp_tnr_reg62.bits.motion_estimate_en;
    tnr_input_reg->noise_detect_en = vpss_reg->vpss_hipp_tnr_reg37.bits.noise_detect_en;
    tnr_input_reg->mcnr_en = vpss_reg->vpss_ctrl.bits.mcnr_en;

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_tnr_input_reg(hi_u32 handle, hi_drv_pq_tnr_input_reg *tnr_input_reg)
{
    hi_s32 ret;

    ret = pq_hal_get_tnr_vir_param(tnr_input_reg);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("get tnr virtual register failed!");
        return HI_FAILURE;
    }

    ret = pq_hal_get_tnr_y_motion_lut(handle, tnr_input_reg);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("get tnr y motion failed!");
        return HI_FAILURE;
    }

    ret = pq_hal_get_tnr_c_motion_lut(handle, tnr_input_reg);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("get tnr c motion failed!");
        return HI_FAILURE;
    }

    ret = pq_hal_get_tnr_func_enable(handle, tnr_input_reg);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("get tnr api input reg failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static __inline__ hi_void pq_alg_copy_u32_by_bit_with_signal(
    hi_u32 src, hi_u32 src_start_bit, hi_s32 *dst, hi_u32 dst_start_bit, hi_u32 num)
{
    hi_u32 src_tmp;
    hi_u32 tmp = 0;

    if (((src_start_bit + num) > 32) || ((dst_start_bit + num) > 32)) { /* 32: bit len max */
        HI_ERR_PrintU32(src_start_bit);
        HI_ERR_PrintU32(dst_start_bit);
        HI_ERR_PrintU32(num);
        return;
    }

    src_tmp = src << (32 - src_start_bit - num); /* 32: bit len max */
    src_tmp = src_tmp >> (32 - num); /* 32: bit len max */

    if (((src_tmp >> (num - 1)) & 0x1) == 1) { /* is a signed int */
        if (dst_start_bit != 0) {
            tmp = (src_tmp << dst_start_bit) | (*dst);
        } else {
            tmp = src_tmp;
        }

        *dst = 0 - ((tmp ^ ((1 << (num + dst_start_bit)) - 1)) + 1);
    } else {
        if (dst_start_bit != 0) {
            tmp = (src_tmp << dst_start_bit) | (*dst);
        } else {
            tmp = src_tmp;
        }

        *dst = tmp & ((1 << (num + dst_start_bit)) - 1);
    }

    return;
}

static __inline__ hi_void tnr_calc_hist_mv1(hi_u16 tmp_mv, hi_u16 me_hist_mv1[5]) /* 5: index */
{
    if (tmp_mv <= 0) {
        me_hist_mv1[0]++;
    } else if (tmp_mv <= 1) {
        me_hist_mv1[1]++;
    } else if (tmp_mv <= 8) { /* 8: num */
        me_hist_mv1[2]++; /* 2: index */
    } else if (tmp_mv <= 16) { /* 16: num */
        me_hist_mv1[3]++; /* 3: index */
    } else {
        me_hist_mv1[4]++; /* 4: index */
    }
}

static __inline__ hi_void tnr_calc_hist_mag(hi_s32 mag, hi_u16 me_hist_mag[5]) /* 5: index */
{
    /* MV_mag */
    if (mag <= 0) {
        me_hist_mag[0]++;
    } else if (mag <= 2) { /* 2: num */
        me_hist_mag[1]++;
    } else if (mag <= 4) { /* 4: num */
        me_hist_mag[2]++; /* 2: index */
    } else if (mag <= 8) { /* 8: num */
        me_hist_mag[3]++; /* 3: index */
    } else {
        me_hist_mag[4]++; /* 4: index */
    }
}

static hi_s32 pq_hal_get_rgmv_reg(hi_drv_pq_vpss_stt_info *vpss_stt_info, drv_pq_tnr_rgmv_info *rgmv_info)
{
    hi_u16 tmp_mv;
    pq_nr_rgmv_info rgmv = { 0 };

    hi_u16 n_col, n_row;
    hi_u32 rgnumh, rgnumv;
    hi_u8 *rgmv_tmp = vpss_stt_info->rgmv_reg_addr;
    hi_u8 *rgmv_tmp2 = HI_NULL;

    memset(rgmv_info->me_hist_mv, 0, sizeof(rgmv_info->me_hist_mv));
    memset(rgmv_info->me_hist_mag, 0, sizeof(rgmv_info->me_hist_mag));

    if (vpss_stt_info->frame_fmt != HI_DRV_PQ_FRM_FRAME) { /* Field Mode - 场模式 */
        rgnumv = (vpss_stt_info->height / 2 + 9) / 16;  /* 2/9/16: num */
    } else { /* Frame Mode - 帧模式 */
        rgnumv = (vpss_stt_info->height + 9) / 16;  /* 9/16: num */
    }

    if (vpss_stt_info->width > WIN_SD_W) {
        rgnumh = (vpss_stt_info->width / 2 + 33) / 64;  /* 2/33/64: num */
    } else {
        rgnumh = (vpss_stt_info->width + 33) / 64;  /* 33/64: num */
    }

    for (n_row = 0; n_row < rgnumv; n_row++) {
        memcpy(g_pq_rgmv_data, rgmv_tmp, vpss_stt_info->stride);
        rgmv_tmp = rgmv_tmp + vpss_stt_info->stride;
        rgmv_tmp2 = g_pq_rgmv_data;

        for (n_col = 0; n_col < rgnumh; n_col++) {
            rgmv.mv_x = 0;
            rgmv.mag = 0;
            rgmv.mv_y = 0;

            /* parse the result data of RGMV */
            pq_alg_copy_u32_by_bit_with_signal(*(hi_u32 *)rgmv_tmp2, 0, &(rgmv.mv_x), 0, 8); /* x[7:0], 8: bit len */
            rgmv.mag = (((*(hi_u32 *)rgmv_tmp2 << (32 - 18 - 10)) >> (32 - 10)) & ((1 << 10) - 1));  /* 10/18/32: num */
            rgmv.mv_y = (((*((hi_u32 *)rgmv_tmp2 + 1) << (32 - 0 - 3)) >> (32 - 3)) & ((1 << 3) - 1));  /* 3/32: num */
            pq_alg_copy_u32_by_bit_with_signal(*((hi_u32 *)rgmv_tmp2 + 1), 0, &(rgmv.mv_y), 4, 3); /* 3/4: bit */

            rgmv_tmp2 += 8; /* 8:num */
            tmp_mv = pq_abs(rgmv.mv_x) + pq_abs(rgmv.mv_y);

            if ((n_row == 0) || (n_row == rgnumv - 1) || (n_col == 0) || (n_col == rgnumh - 1)) {
                continue;
            }

            tnr_calc_hist_mv1(tmp_mv, rgmv_info->me_hist_mv);
            tnr_calc_hist_mag(rgmv.mag, rgmv_info->me_hist_mag);
        }
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_update_tnr_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_nr_input_info *nr_api_input)
{
    hi_s32 i, ret;
    vpss_stt_reg_type *nr_stt_info = (vpss_stt_reg_type *)vpss_stt_info->vpss_stt_reg;

    nr_api_input->width = vpss_stt_info->width;
    nr_api_input->height = vpss_stt_info->height;
    nr_api_input->frame_fmt = vpss_stt_info->frame_fmt;

    /* for scene change */
    for (i = 0; i < 32; i++) { /* 32:index len */
        nr_api_input->scd_stt_reg.sc_hist_cf[i] =
            *(hi_s32 *)((hi_u32 *)(&nr_stt_info->vpss_wstt_scd_hist_bin_1.u32) + i);
    }

    /* for tnr  write back register */
    pq_hal_get_tnr_stt_reg(vpss_stt_info, &(nr_api_input->tnr_stt_reg));
    pq_hal_get_me_input_reg(vpss_stt_info->handle_id, &(nr_api_input->me_input_reg));
    ret = pq_hal_get_tnr_input_reg(vpss_stt_info->handle_id, &(nr_api_input->tnr_input_reg));
    PQ_CHECK_RETURN_SUCCESS(ret);
    ret = pq_hal_get_rgmv_reg(vpss_stt_info, &(nr_api_input->rgmv_info));

    return ret;
}

/* ********************************tnr get input info api end ****************************************** */
/* ****************************tnr update software alg cfg to vpss start ***************************** */
hi_void pq_hal_update_tnr_c_blending_alpha_lut(hi_u32 handle, hi_drv_pq_nr_api_output_reg *nr_out_reg)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle);
    PQ_CHECK_NULL_PTR_RE_NULL(vpss_reg);
    PQ_CHECK_NULL_PTR_RE_NULL(nr_out_reg);

    vpss_reg->vpss_hipp_tnr_reg160.bits.blending_alpha_lut_0 = nr_out_reg->tnr_out_reg.c_tf_lut[0];
    vpss_reg->vpss_hipp_tnr_reg160.bits.blending_alpha_lut_1 = nr_out_reg->tnr_out_reg.c_tf_lut[1];
    vpss_reg->vpss_hipp_tnr_reg160.bits.blending_alpha_lut_2 = nr_out_reg->tnr_out_reg.c_tf_lut[2]; /* 2:index */
    vpss_reg->vpss_hipp_tnr_reg160.bits.blending_alpha_lut_3 = nr_out_reg->tnr_out_reg.c_tf_lut[3]; /* 3:index */
    vpss_reg->vpss_hipp_tnr_reg160.bits.blending_alpha_lut_4 = nr_out_reg->tnr_out_reg.c_tf_lut[4]; /* 4:index */
    vpss_reg->vpss_hipp_tnr_reg160.bits.blending_alpha_lut_5 = nr_out_reg->tnr_out_reg.c_tf_lut[5]; /* 5:index */
    vpss_reg->vpss_hipp_tnr_reg161.bits.blending_alpha_lut_6 = nr_out_reg->tnr_out_reg.c_tf_lut[6]; /* 6:index */
    vpss_reg->vpss_hipp_tnr_reg161.bits.blending_alpha_lut_7 = nr_out_reg->tnr_out_reg.c_tf_lut[7]; /* 7:index */
    vpss_reg->vpss_hipp_tnr_reg161.bits.blending_alpha_lut_8 = nr_out_reg->tnr_out_reg.c_tf_lut[8]; /* 8:index */
    vpss_reg->vpss_hipp_tnr_reg161.bits.blending_alpha_lut_9 = nr_out_reg->tnr_out_reg.c_tf_lut[9]; /* 9:index */
    vpss_reg->vpss_hipp_tnr_reg161.bits.blending_alpha_lut_10 = nr_out_reg->tnr_out_reg.c_tf_lut[10]; /* 10:index */
    vpss_reg->vpss_hipp_tnr_reg161.bits.blending_alpha_lut_11 = nr_out_reg->tnr_out_reg.c_tf_lut[11]; /* 11:index */
    vpss_reg->vpss_hipp_tnr_reg162.bits.blending_alpha_lut_12 = nr_out_reg->tnr_out_reg.c_tf_lut[12]; /* 12:index */
    vpss_reg->vpss_hipp_tnr_reg162.bits.blending_alpha_lut_13 = nr_out_reg->tnr_out_reg.c_tf_lut[13]; /* 13:index */
    vpss_reg->vpss_hipp_tnr_reg162.bits.blending_alpha_lut_14 = nr_out_reg->tnr_out_reg.c_tf_lut[14]; /* 14:index */
    vpss_reg->vpss_hipp_tnr_reg162.bits.blending_alpha_lut_15 = nr_out_reg->tnr_out_reg.c_tf_lut[15]; /* 15:index */
    vpss_reg->vpss_hipp_tnr_reg162.bits.blending_alpha_lut_16 = nr_out_reg->tnr_out_reg.c_tf_lut[16]; /* 16:index */
    vpss_reg->vpss_hipp_tnr_reg162.bits.blending_alpha_lut_17 = nr_out_reg->tnr_out_reg.c_tf_lut[17]; /* 17:index */
    vpss_reg->vpss_hipp_tnr_reg163.bits.blending_alpha_lut_18 = nr_out_reg->tnr_out_reg.c_tf_lut[18]; /* 18:index */
    vpss_reg->vpss_hipp_tnr_reg163.bits.blending_alpha_lut_19 = nr_out_reg->tnr_out_reg.c_tf_lut[19]; /* 19:index */
    vpss_reg->vpss_hipp_tnr_reg163.bits.blending_alpha_lut_20 = nr_out_reg->tnr_out_reg.c_tf_lut[20]; /* 20:index */
    vpss_reg->vpss_hipp_tnr_reg163.bits.blending_alpha_lut_21 = nr_out_reg->tnr_out_reg.c_tf_lut[21]; /* 21:index */
    vpss_reg->vpss_hipp_tnr_reg163.bits.blending_alpha_lut_22 = nr_out_reg->tnr_out_reg.c_tf_lut[22]; /* 22:index */
    vpss_reg->vpss_hipp_tnr_reg163.bits.blending_alpha_lut_23 = nr_out_reg->tnr_out_reg.c_tf_lut[23]; /* 23:index */
    vpss_reg->vpss_hipp_tnr_reg164.bits.blending_alpha_lut_24 = nr_out_reg->tnr_out_reg.c_tf_lut[24]; /* 24:index */
    vpss_reg->vpss_hipp_tnr_reg164.bits.blending_alpha_lut_25 = nr_out_reg->tnr_out_reg.c_tf_lut[25]; /* 25:index */
    vpss_reg->vpss_hipp_tnr_reg164.bits.blending_alpha_lut_26 = nr_out_reg->tnr_out_reg.c_tf_lut[26]; /* 26:index */
    vpss_reg->vpss_hipp_tnr_reg164.bits.blending_alpha_lut_27 = nr_out_reg->tnr_out_reg.c_tf_lut[27]; /* 27:index */
    vpss_reg->vpss_hipp_tnr_reg164.bits.blending_alpha_lut_28 = nr_out_reg->tnr_out_reg.c_tf_lut[28]; /* 28:index */
    vpss_reg->vpss_hipp_tnr_reg164.bits.blending_alpha_lut_29 = nr_out_reg->tnr_out_reg.c_tf_lut[29]; /* 29:index */
    vpss_reg->vpss_hipp_tnr_reg165.bits.blending_alpha_lut_30 = nr_out_reg->tnr_out_reg.c_tf_lut[30]; /* 30:index */
    vpss_reg->vpss_hipp_tnr_reg165.bits.blending_alpha_lut_31 = nr_out_reg->tnr_out_reg.c_tf_lut[31]; /* 31:index */

    return;
}

hi_void pq_hal_update_tnr_y_blending_alpha_lut(hi_u32 handle, hi_drv_pq_nr_api_output_reg *nr_out_reg)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle);
    PQ_CHECK_NULL_PTR_RE_NULL(vpss_reg);
    PQ_CHECK_NULL_PTR_RE_NULL(nr_out_reg);

    vpss_reg->vpss_hipp_tnr_reg154.bits.y_blending_alpha_lut_0 = nr_out_reg->tnr_out_reg.y_tf_lut[0];
    vpss_reg->vpss_hipp_tnr_reg154.bits.y_blending_alpha_lut_1 = nr_out_reg->tnr_out_reg.y_tf_lut[1];
    vpss_reg->vpss_hipp_tnr_reg154.bits.y_blending_alpha_lut_2 = nr_out_reg->tnr_out_reg.y_tf_lut[2]; /* 2:index */
    vpss_reg->vpss_hipp_tnr_reg154.bits.y_blending_alpha_lut_3 = nr_out_reg->tnr_out_reg.y_tf_lut[3]; /* 3:index */
    vpss_reg->vpss_hipp_tnr_reg154.bits.y_blending_alpha_lut_4 = nr_out_reg->tnr_out_reg.y_tf_lut[4]; /* 4:index */
    vpss_reg->vpss_hipp_tnr_reg154.bits.y_blending_alpha_lut_5 = nr_out_reg->tnr_out_reg.y_tf_lut[5]; /* 5:index */

    vpss_reg->vpss_hipp_tnr_reg155.bits.y_blending_alpha_lut_6 = nr_out_reg->tnr_out_reg.y_tf_lut[6]; /* 6:index */
    vpss_reg->vpss_hipp_tnr_reg155.bits.y_blending_alpha_lut_7 = nr_out_reg->tnr_out_reg.y_tf_lut[7]; /* 7:index */
    vpss_reg->vpss_hipp_tnr_reg155.bits.y_blending_alpha_lut_8 = nr_out_reg->tnr_out_reg.y_tf_lut[8]; /* 8:index */
    vpss_reg->vpss_hipp_tnr_reg155.bits.y_blending_alpha_lut_9 = nr_out_reg->tnr_out_reg.y_tf_lut[9]; /* 9:index */
    vpss_reg->vpss_hipp_tnr_reg155.bits.y_blending_alpha_lut_10 = nr_out_reg->tnr_out_reg.y_tf_lut[10]; /* 10:index */
    vpss_reg->vpss_hipp_tnr_reg155.bits.y_blending_alpha_lut_11 = nr_out_reg->tnr_out_reg.y_tf_lut[11]; /* 11:index */

    vpss_reg->vpss_hipp_tnr_reg156.bits.y_blending_alpha_lut_12 = nr_out_reg->tnr_out_reg.y_tf_lut[12]; /* 12:index */
    vpss_reg->vpss_hipp_tnr_reg156.bits.y_blending_alpha_lut_13 = nr_out_reg->tnr_out_reg.y_tf_lut[13]; /* 13:index */
    vpss_reg->vpss_hipp_tnr_reg156.bits.y_blending_alpha_lut_14 = nr_out_reg->tnr_out_reg.y_tf_lut[14]; /* 14:index */
    vpss_reg->vpss_hipp_tnr_reg156.bits.y_blending_alpha_lut_15 = nr_out_reg->tnr_out_reg.y_tf_lut[15]; /* 15:index */
    vpss_reg->vpss_hipp_tnr_reg156.bits.y_blending_alpha_lut_16 = nr_out_reg->tnr_out_reg.y_tf_lut[16]; /* 16:index */
    vpss_reg->vpss_hipp_tnr_reg156.bits.y_blending_alpha_lut_17 = nr_out_reg->tnr_out_reg.y_tf_lut[17]; /* 17:index */

    vpss_reg->vpss_hipp_tnr_reg157.bits.y_blending_alpha_lut_18 = nr_out_reg->tnr_out_reg.y_tf_lut[18]; /* 18:index */
    vpss_reg->vpss_hipp_tnr_reg157.bits.y_blending_alpha_lut_19 = nr_out_reg->tnr_out_reg.y_tf_lut[19]; /* 19:index */
    vpss_reg->vpss_hipp_tnr_reg157.bits.y_blending_alpha_lut_20 = nr_out_reg->tnr_out_reg.y_tf_lut[20]; /* 20:index */
    vpss_reg->vpss_hipp_tnr_reg157.bits.y_blending_alpha_lut_21 = nr_out_reg->tnr_out_reg.y_tf_lut[21]; /* 21:index */
    vpss_reg->vpss_hipp_tnr_reg157.bits.y_blending_alpha_lut_22 = nr_out_reg->tnr_out_reg.y_tf_lut[22]; /* 22:index */
    vpss_reg->vpss_hipp_tnr_reg157.bits.y_blending_alpha_lut_23 = nr_out_reg->tnr_out_reg.y_tf_lut[23]; /* 23:index */

    vpss_reg->vpss_hipp_tnr_reg158.bits.y_blending_alpha_lut_24 = nr_out_reg->tnr_out_reg.y_tf_lut[24]; /* 24:index */
    vpss_reg->vpss_hipp_tnr_reg158.bits.y_blending_alpha_lut_25 = nr_out_reg->tnr_out_reg.y_tf_lut[25]; /* 25:index */
    vpss_reg->vpss_hipp_tnr_reg158.bits.y_blending_alpha_lut_26 = nr_out_reg->tnr_out_reg.y_tf_lut[26]; /* 26:index */
    vpss_reg->vpss_hipp_tnr_reg158.bits.y_blending_alpha_lut_27 = nr_out_reg->tnr_out_reg.y_tf_lut[27]; /* 27:index */
    vpss_reg->vpss_hipp_tnr_reg158.bits.y_blending_alpha_lut_28 = nr_out_reg->tnr_out_reg.y_tf_lut[28]; /* 28:index */
    vpss_reg->vpss_hipp_tnr_reg158.bits.y_blending_alpha_lut_29 = nr_out_reg->tnr_out_reg.y_tf_lut[29]; /* 29:index */

    vpss_reg->vpss_hipp_tnr_reg159.bits.y_blending_alpha_lut_30 = nr_out_reg->tnr_out_reg.y_tf_lut[30]; /* 30:index */
    vpss_reg->vpss_hipp_tnr_reg159.bits.y_blending_alpha_lut_31 = nr_out_reg->tnr_out_reg.y_tf_lut[31]; /* 31:index */

    return;
}

hi_void pq_hal_update_tnr_other_cfg(hi_u32 handle, hi_drv_pq_nr_api_output_reg *nr_out_reg)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle);
    PQ_CHECK_NULL_PTR_RE_NULL(vpss_reg);
    PQ_CHECK_NULL_PTR_RE_NULL(nr_out_reg);

    vpss_reg->vpss_hipp_tnr_reg38.bits.alpha1 = nr_out_reg->tnr_out_reg.md_alpha1;
    vpss_reg->vpss_hipp_tnr_reg39.bits.alpha2 = nr_out_reg->tnr_out_reg.md_alpha2;

    vpss_reg->vpss_hipp_tnr_reg50.bits.y_md_gain = nr_out_reg->tnr_out_reg.md_y_gain;
    vpss_reg->vpss_hipp_tnr_reg50.bits.y_md_core = nr_out_reg->tnr_out_reg.md_y_core;
    vpss_reg->vpss_hipp_tnr_reg50.bits.md_gain = nr_out_reg->tnr_out_reg.md_c_gain;
    vpss_reg->vpss_hipp_tnr_reg50.bits.md_core = nr_out_reg->tnr_out_reg.md_c_core;

    vpss_reg->vpss_hipp_tnr_reg69.bits.y_mc_gain = nr_out_reg->tnr_out_reg.mc_y_gain;
    vpss_reg->vpss_hipp_tnr_reg70.bits.y_mc_core = nr_out_reg->tnr_out_reg.mc_y_core;
    vpss_reg->vpss_hipp_tnr_reg70.bits.mc_gain = nr_out_reg->tnr_out_reg.mc_c_gain;
    vpss_reg->vpss_hipp_tnr_reg70.bits.mc_core = nr_out_reg->tnr_out_reg.mc_c_core;

    vpss_reg->vpss_hipp_tnr_reg67.bits.gm_adj = nr_out_reg->tnr_out_reg.me_gm_adj;

    vpss_reg->vpss_hipp_tnr_reg62.bits.discard_smv_x_en = nr_out_reg->tnr_out_reg.me_discard_smv_x_en;
    vpss_reg->vpss_hipp_tnr_reg62.bits.discard_smv_y_en = nr_out_reg->tnr_out_reg.me_discard_smv_y_en;
    vpss_reg->vpss_hipp_tnr_reg62.bits.smv_x_thd = nr_out_reg->tnr_out_reg.me_smv_x_thd;
    vpss_reg->vpss_hipp_tnr_reg62.bits.smv_y_thd = nr_out_reg->tnr_out_reg.me_smv_y_thd;

    vpss_reg->vpss_hipp_tnr_reg66.bits.std_pnl_gain_0mv = nr_out_reg->tnr_out_reg.me_std_pnl_gain_0mv;
    vpss_reg->vpss_hipp_tnr_reg66.bits.std_pnl_core_0mv = nr_out_reg->tnr_out_reg.me_std_pnl_core_0mv;
    vpss_reg->vpss_hipp_tnr_reg66.bits.std_pnl_gain_xmv = nr_out_reg->tnr_out_reg.me_std_pnl_gain_xmv;
    vpss_reg->vpss_hipp_tnr_reg66.bits.std_pnl_core_xmv = nr_out_reg->tnr_out_reg.me_std_pnl_core_xmv;

    vpss_reg->vpss_hipp_tnr_reg66.bits.std_core_0mv = nr_out_reg->tnr_out_reg.me_std_core_0mv;
    vpss_reg->vpss_hipp_tnr_reg66.bits.std_core_xmv = nr_out_reg->tnr_out_reg.me_std_core_xmv;

    vpss_reg->vpss_hipp_tnr_reg63.bits.mag_pnl_gain_0mv = nr_out_reg->tnr_out_reg.me_mag_pnl_gain_0mv;
    vpss_reg->vpss_hipp_tnr_reg63.bits.mag_pnl_core_0mv = nr_out_reg->tnr_out_reg.me_mag_pnl_core_0mv;
    vpss_reg->vpss_hipp_tnr_reg63.bits.mag_pnl_gain_xmv = nr_out_reg->tnr_out_reg.me_mag_pnl_gain_xmv;
    vpss_reg->vpss_hipp_tnr_reg63.bits.mag_pnl_core_xmv = nr_out_reg->tnr_out_reg.me_mag_pnl_core_xmv;

    vpss_reg->vpss_hipp_tnr_reg64.bits.std_pnl_gain = nr_out_reg->tnr_out_reg.me_std_pnl_gain;
    vpss_reg->vpss_hipp_tnr_reg64.bits.std_pnl_core = nr_out_reg->tnr_out_reg.me_std_pnl_core;

    vpss_reg->vpss_hipp_tnr_reg65.bits.adj_mv_max = nr_out_reg->tnr_out_reg.me_adj_mv_max;
    vpss_reg->vpss_hipp_tnr_reg65.bits.adj_mv_min = nr_out_reg->tnr_out_reg.me_adj_mv_min;
    vpss_reg->vpss_hipp_tnr_reg64.bits.adj_0mv_min = nr_out_reg->tnr_out_reg.me_adj_0mv_min;
    vpss_reg->vpss_hipp_tnr_reg64.bits.adj_0mv_max = nr_out_reg->tnr_out_reg.me_adj_0mv_max;
    vpss_reg->vpss_hipp_tnr_reg64.bits.adj_xmv_min = nr_out_reg->tnr_out_reg.me_adj_xmv_min;
    vpss_reg->vpss_hipp_tnr_reg64.bits.adj_xmv_max = nr_out_reg->tnr_out_reg.me_adj_xmv_max;

    vpss_reg->vpss_hipp_tnr_reg36.bits.scene_change_info = nr_out_reg->tnr_out_reg.scd_val;
    vpss_reg->vpss_hipp_tnr_reg145.bits.noise_level = nr_out_reg->tnr_out_reg.noise_level;
    vpss_reg->vpss_hipp_tnr_reg145.bits.global_motion = nr_out_reg->tnr_out_reg.global_motion;
    vpss_reg->vpss_hipp_tnr_reg70.bits.saltus_level = nr_out_reg->tnr_out_reg.saltus_level;
    vpss_reg->vpss_hipp_tnr_reg146.bits.mc1d_alpha = nr_out_reg->tnr_out_reg.mc1d_alpha;

    vpss_reg->vpss_hipp_tnr_reg153.bits.y_no_nr_range = nr_out_reg->tnr_out_reg.ar_y_no_nr_range;
    vpss_reg->vpss_hipp_tnr_reg153.bits.no_nr_range = nr_out_reg->tnr_out_reg.ar_c_no_nr_range;

    vpss_reg->vpss_hipp_tnr_reg5.bits.mcnr_mc_mot_en = nr_out_reg->tnr_out_reg.mcnr_mc_mot_en;
    vpss_reg->vpss_hipp_tnr_reg222.bits.mc_mt_2dfs_adj_en = nr_out_reg->tnr_out_reg.mc_mt_2dfs_adj_en;

    return;
}

hi_s32 pq_hal_update_tnr_cfg(hi_u32 handle, hi_drv_pq_nr_api_output_reg *nr_out_reg)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(nr_out_reg);

    pq_hal_update_tnr_c_blending_alpha_lut(handle, nr_out_reg);
    pq_hal_update_tnr_y_blending_alpha_lut(handle, nr_out_reg);
    pq_hal_update_tnr_other_cfg(handle, nr_out_reg);

    vpss_reg->vpss_hipp_tnr_reg214.bits.lut_yh_noise = nr_out_reg->ccs_out_reg.yh_noise;

    g_nr_ori_global_motion = nr_out_reg->tnr_out_reg.ori_global_motion;

    return HI_SUCCESS;
}
