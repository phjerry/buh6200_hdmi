/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: pq hal dm api
 * Author: pq
 * Create: 2019-01-1
 */

#include <linux/string.h>
#include "pq_hal_dm.h"

hi_s32 pq_hal_enable_dm(hi_u32 handle, hi_bool on_off)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);
    vpss_reg->vpss_dbm_ctrl.bits.dm_en = on_off;

    return HI_SUCCESS;
}

hi_s32 pq_hal_enable_dm_demo(hi_u32 handle, hi_bool on_off)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);
    vpss_reg->vpss_dbm_ctrl.bits.dbm_demo_en = on_off;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_dm_strength(hi_u32 handle, hi_u32 strength)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);
    vpss_reg->vpss_dm_edge.bits.dm_global_str = strength;

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

static hi_s32 pq_hal_get_dm_parser_rgmv(hi_drv_pq_vpss_stt_info *dm_calc_info, hi_drv_pq_dm_input_info *dm_api_input)
{
    hi_u32 rg_num_v, rg_num_h, row, col, stride;
    hi_u32 *tmp = HI_NULL;
    hi_u32 *rgmv = HI_NULL;
    hi_s32 m_vx, m_vy, mag;

    PQ_CHECK_NULL_PTR_RE_FAIL(dm_calc_info);

    if (dm_calc_info->frame_fmt != HI_DRV_PQ_FRM_FRAME) {
        rg_num_v = (dm_calc_info->height / 2 + 9) / 16; /* 2/9/16: multi */
    } else {
        rg_num_v = (dm_calc_info->height + 9) / 16; /* 9/16: multi */
    }

    if (dm_calc_info->width > WIN_SD_W) {
        rg_num_h = (dm_calc_info->width / 2 + (RG_SIZE_H / 2 + 1)) / RG_SIZE_H; /* 2: multi */
    } else {
        rg_num_h = (dm_calc_info->width + (RG_SIZE_H / 2 + 1)) / RG_SIZE_H; /* 2: multi */
    }

    if ((rg_num_h < 2) || (rg_num_v < 2) ||  /* 2: multi */
        (rg_num_h > (DM_MAX_WIDTH / RGSIZEW_SD)) || (rg_num_v > (DM_MAX_HEIGHT / RGSIZEH))) {
        HI_ERR_PQ("============= err u32RGnUmh[%d] or u32RGnUmV[%d]============\n", rg_num_h, rg_num_v);
        return HI_FAILURE;
    }

    dm_api_input->rgmv_blknum_w = rg_num_h;
    dm_api_input->rgmv_blknum_h = rg_num_v;

    rgmv = (hi_u32 *)dm_calc_info->rgmv_reg_addr;
    stride = dm_calc_info->stride;

    for (row = 0; row < rg_num_v; row++) {
        for (col = 0; col < rg_num_h; col++) {
            tmp = (hi_u32*)rgmv + (row * stride / 4 + col * 4); /* 4: multi */
            if (tmp == HI_NULL || (tmp + 1) == HI_NULL) {
                HI_INFO_PQ("\n	rgmv = 0x%x , offset = 0x%x\n", rgmv, (row * stride / 4 + col * 4)); /* 4: multi */
            }
            m_vx = 0;
            m_vy = 0;
            mag = 0;

            pq_alg_copy_u32_by_bit_with_signal(*(hi_u32*)((hi_u32*)rgmv +
                (row * stride / 4 + col * 2)), 0, &m_vx, 0, 9); /* 4/2:multi; 9:bit len; x[8:0] */
            pq_alg_copy_u32_by_bit_with_signal(*(hi_u32*)((hi_u32*)rgmv +
                (row * stride / 4 + col * 2) + 1), 4, &m_vy, 0, 8); /* 4/2: multi; 8:bit len; mvy[7:0] */
            pq_alg_copy_u32_by_bit_with_signal(*(hi_u32*)((hi_u32*)rgmv +
                (row * stride / 4 + col * 2)), 19, &mag, 0, 10); /* 2/4: multi; 19: start bit; 10:bit len; mag[9:0] */

            if (row * rg_num_h + col < (DM_MAX_HEIGHT / RGSIZEH_SD) * (DM_MAX_WIDTH / RGSIZEW_SD)) {
                dm_api_input->rgmv_x_buf[row * rg_num_h + col] = (hi_s16)m_vx;
                dm_api_input->rgmv_y_buf[row * rg_num_h + col] = (hi_s16)m_vy;
                dm_api_input->rgmv_mag_buf[row * rg_num_h + col] = (hi_u16)mag;
            }
        }
    }

    return HI_SUCCESS;
}

static hi_s32 pq_hal_get_dm_trans_band(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_dm_input_info *dm_api_input)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_stt_info);
    PQ_CHECK_NULL_PTR_RE_FAIL(dm_api_input);

    vpss_reg = pq_hal_get_vpss_reg(vpss_stt_info->handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    dm_api_input->transband[0]  = vpss_reg->vpss_dm_lut5.bits.dm_trans_band_lut_p0  ;
    dm_api_input->transband[1]  = vpss_reg->vpss_dm_lut5.bits.dm_trans_band_lut_p1  ;
    dm_api_input->transband[2]  = vpss_reg->vpss_dm_lut5.bits.dm_trans_band_lut_p2  ; /* 2: index */
    dm_api_input->transband[3]  = vpss_reg->vpss_dm_lut5.bits.dm_trans_band_lut_p3  ; /* 3: index */
    dm_api_input->transband[4]  = vpss_reg->vpss_dm_lut5.bits.dm_trans_band_lut_p4  ; /* 4: index */
    dm_api_input->transband[5]  = vpss_reg->vpss_dm_lut6.bits.dm_trans_band_lut_p5  ; /* 5: index */
    dm_api_input->transband[6]  = vpss_reg->vpss_dm_lut6.bits.dm_trans_band_lut_p6  ; /* 6: index */
    dm_api_input->transband[7]  = vpss_reg->vpss_dm_lut6.bits.dm_trans_band_lut_p7  ; /* 7: index */
    dm_api_input->transband[8]  = vpss_reg->vpss_dm_lut6.bits.dm_trans_band_lut_p8  ; /* 8: index */
    dm_api_input->transband[9]  = vpss_reg->vpss_dm_lut6.bits.dm_trans_band_lut_p9  ; /* 9: index */
    dm_api_input->transband[10] = vpss_reg->vpss_dm_lut7.bits.dm_trans_band_lut_p10 ; /* 10: index */
    dm_api_input->transband[11] = vpss_reg->vpss_dm_lut7.bits.dm_trans_band_lut_p11 ; /* 11: index */
    dm_api_input->transband[12] = vpss_reg->vpss_dm_lut7.bits.dm_trans_band_lut_p12 ; /* 12: index */
    dm_api_input->transband[13] = vpss_reg->vpss_dm_lut7.bits.dm_trans_band_lut_p13 ; /* 13: index */
    dm_api_input->transband[14] = vpss_reg->vpss_dm_lut7.bits.dm_trans_band_lut_p14 ; /* 14: index */
    dm_api_input->transband[15] = vpss_reg->vpss_dm_lut8.bits.dm_trans_band_lut_p15 ; /* 15: index */
    dm_api_input->transband[16] = vpss_reg->vpss_dm_lut8.bits.dm_trans_band_lut_p16 ; /* 16: index */
    dm_api_input->transband[17] = vpss_reg->vpss_dm_lut8.bits.dm_trans_band_lut_p17 ; /* 17: index */
    dm_api_input->transband[18] = vpss_reg->vpss_dm_lut8.bits.dm_trans_band_lut_p18 ; /* 18: index */
    dm_api_input->transband[19] = vpss_reg->vpss_dm_lut8.bits.dm_trans_band_lut_p19 ; /* 19: index */
    dm_api_input->transband[20] = vpss_reg->vpss_dm_lut9.bits.dm_trans_band_lut_p20 ; /* 20: index */
    dm_api_input->transband[21] = vpss_reg->vpss_dm_lut9.bits.dm_trans_band_lut_p21 ; /* 21: index */
    dm_api_input->transband[22] = vpss_reg->vpss_dm_lut9.bits.dm_trans_band_lut_p22 ; /* 22: index */
    dm_api_input->transband[23] = vpss_reg->vpss_dm_lut9.bits.dm_trans_band_lut_p23 ; /* 23: index */
    dm_api_input->transband[24] = vpss_reg->vpss_dm_lut9.bits.dm_trans_band_lut_p24 ; /* 24: index */
    dm_api_input->transband[25] = vpss_reg->vpss_dm_lut10.bits.dm_trans_band_lut_p25; /* 25: index */
    dm_api_input->transband[26] = vpss_reg->vpss_dm_lut10.bits.dm_trans_band_lut_p26; /* 26: index */
    dm_api_input->transband[27] = vpss_reg->vpss_dm_lut10.bits.dm_trans_band_lut_p27; /* 27: index */
    dm_api_input->transband[28] = vpss_reg->vpss_dm_lut10.bits.dm_trans_band_lut_p28; /* 28: index */
    dm_api_input->transband[29] = vpss_reg->vpss_dm_lut10.bits.dm_trans_band_lut_p29; /* 29: index */
    dm_api_input->transband[30] = vpss_reg->vpss_dm_lut11.bits.dm_trans_band_lut_p30; /* 30: index */

    return HI_SUCCESS;
}

static hi_s32 pq_hal_read_dm_para(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_dm_input_info *dm_api_input)
{
    hi_s32 ret;
    vpss_reg_type *vpss_reg = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_stt_info);
    PQ_CHECK_NULL_PTR_RE_FAIL(dm_api_input);

    vpss_reg = pq_hal_get_vpss_reg(vpss_stt_info->handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    dm_api_input->opp_ang_ctrst_t      = vpss_reg->vpss_dm_dir.bits.dm_opp_ang_ctrst_t;
    dm_api_input->mn_dir_opp_ctrst_t   = vpss_reg->vpss_dm_dirc.bits.dm_mndir_opp_ctrst_t;
    dm_api_input->csw_trsnt_lt         = vpss_reg->vpss_dm_mmflr.bits.dm_csw_trsnt_lt;
    dm_api_input->csw_trsnt_lt_10bit   = vpss_reg->vpss_dm_mmfsr.bits.dm_csw_trsnt_lt_10;
    dm_api_input->lsw_ratio            = vpss_reg->vpss_dm_lsw.bits.dm_lsw_ratio;
    dm_api_input->limit_lsw_ratio      = vpss_reg->vpss_dm_thr0.bits.dm_limit_lsw_ratio;
    dm_api_input->mmf_lr               = vpss_reg->vpss_dm_mmflr.bits.dm_mmf_lr;
    dm_api_input->mmf_lr_10bit         = vpss_reg->vpss_dm_mmflr.bits.dm_mmf_lr_10;
    dm_api_input->mmf_sr               = vpss_reg->vpss_dm_mmfsr.bits.dm_mmf_sr;
    dm_api_input->mmf_sr_10bit         = vpss_reg->vpss_dm_mmfsr.bits.dm_mmf_sr_10;
    dm_api_input->mmf_limit_en         = vpss_reg->vpss_dm_dirc.bits.dm_mmf_limit_en;
    dm_api_input->mmf_set              = vpss_reg->vpss_dm_edge.bits.dm_mmf_set;
    dm_api_input->init_val_step        = vpss_reg->vpss_dm_edge.bits.dm_init_val_step;

    dm_api_input->limit_t             = vpss_reg->vpss_dm_lut1.bits.dm_limit_t;
    dm_api_input->limit_t10bit        = vpss_reg->vpss_dm_lut1.bits.dm_limit_t_10;
    dm_api_input->lim_res_blend_str1  = vpss_reg->vpss_dm_thr0.bits.dm_lim_res_blend_str1;
    dm_api_input->lim_res_blend_str2  = vpss_reg->vpss_dm_thr0.bits.dm_lim_res_blend_str2;
    dm_api_input->dir_blend_str       = vpss_reg->vpss_dm_thr0.bits.dm_dir_blend_str;
    dm_api_input->lw_ctrst_t          = vpss_reg->vpss_dm_lsw.bits.dm_lw_ctrst_t;
    dm_api_input->lw_ctrst_t_10bit    = vpss_reg->vpss_dm_lsw.bits.dm_lw_ctrst_t_10;
    dm_api_input->csw_trsnt_st        = vpss_reg->vpss_dm_lsw.bits.dm_csw_trsnt_st;
    dm_api_input->csw_trsnt_st_10bit  = vpss_reg->vpss_dm_diro.bits.dm_csw_trsnt_st_10;
    dm_api_input->global_motion          = vpss_reg->vpss_hipp_tnr_reg145.bits.global_motion;
    ret = pq_hal_get_dm_trans_band(vpss_stt_info, dm_api_input);

    return ret;
}

hi_s32 pq_hal_update_dm_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_dm_input_info *dm_api_input)
{
    hi_s32 ret;
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(vpss_stt_info->handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_dbd_img.bits.img_width_org = vpss_stt_info->width - 1;
    vpss_reg->vpss_dbd_img.bits.img_height_org = vpss_stt_info->height - 1;
    vpss_reg->vpss_dbd_area.bits.valid_area_width = vpss_stt_info->width - 1;

    ret = pq_hal_read_dm_para(vpss_stt_info, dm_api_input);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_get_dm_parser_rgmv(vpss_stt_info, dm_api_input);
    return ret;
}

hi_s32 pq_hal_set_dm_trans_band(hi_u32 handle_no, hi_drv_pq_dm_output_reg *dm_api_reg)
{
    vpss_reg_type *vpss_vir_reg = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(dm_api_reg);
    vpss_vir_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_vir_reg);

    vpss_vir_reg->vpss_dm_lut5.bits.dm_trans_band_lut_p4 = dm_api_reg->trans_band[4]; /* 4: index */
    vpss_vir_reg->vpss_dm_lut5.bits.dm_trans_band_lut_p3 = dm_api_reg->trans_band[3]; /* 3: index */
    vpss_vir_reg->vpss_dm_lut5.bits.dm_trans_band_lut_p2 = dm_api_reg->trans_band[2]; /* 2: index */
    vpss_vir_reg->vpss_dm_lut5.bits.dm_trans_band_lut_p1 = dm_api_reg->trans_band[1];
    vpss_vir_reg->vpss_dm_lut5.bits.dm_trans_band_lut_p0 = dm_api_reg->trans_band[0];

    vpss_vir_reg->vpss_dm_lut6.bits.dm_trans_band_lut_p9 = dm_api_reg->trans_band[9]; /* 9: index */
    vpss_vir_reg->vpss_dm_lut6.bits.dm_trans_band_lut_p8 = dm_api_reg->trans_band[8]; /* 8: index */
    vpss_vir_reg->vpss_dm_lut6.bits.dm_trans_band_lut_p7 = dm_api_reg->trans_band[7]; /* 7: index */
    vpss_vir_reg->vpss_dm_lut6.bits.dm_trans_band_lut_p6 = dm_api_reg->trans_band[6]; /* 6: index */
    vpss_vir_reg->vpss_dm_lut6.bits.dm_trans_band_lut_p5 = dm_api_reg->trans_band[5]; /* 5: index */

    vpss_vir_reg->vpss_dm_lut7.bits.dm_trans_band_lut_p14 = dm_api_reg->trans_band[14]; /* 14: index */
    vpss_vir_reg->vpss_dm_lut7.bits.dm_trans_band_lut_p13 = dm_api_reg->trans_band[13]; /* 13: index */
    vpss_vir_reg->vpss_dm_lut7.bits.dm_trans_band_lut_p12 = dm_api_reg->trans_band[12]; /* 12: index */
    vpss_vir_reg->vpss_dm_lut7.bits.dm_trans_band_lut_p11 = dm_api_reg->trans_band[11]; /* 11: index */
    vpss_vir_reg->vpss_dm_lut7.bits.dm_trans_band_lut_p10 = dm_api_reg->trans_band[10]; /* 10: index */

    vpss_vir_reg->vpss_dm_lut8.bits.dm_trans_band_lut_p19 = dm_api_reg->trans_band[19]; /* 19: index */
    vpss_vir_reg->vpss_dm_lut8.bits.dm_trans_band_lut_p18 = dm_api_reg->trans_band[18]; /* 18: index */
    vpss_vir_reg->vpss_dm_lut8.bits.dm_trans_band_lut_p17 = dm_api_reg->trans_band[17]; /* 17: index */
    vpss_vir_reg->vpss_dm_lut8.bits.dm_trans_band_lut_p16 = dm_api_reg->trans_band[16]; /* 16: index */
    vpss_vir_reg->vpss_dm_lut8.bits.dm_trans_band_lut_p15 = dm_api_reg->trans_band[15]; /* 15: index */

    vpss_vir_reg->vpss_dm_lut9.bits.dm_trans_band_lut_p24 = dm_api_reg->trans_band[24]; /* 24: index */
    vpss_vir_reg->vpss_dm_lut9.bits.dm_trans_band_lut_p23 = dm_api_reg->trans_band[23]; /* 23: index */
    vpss_vir_reg->vpss_dm_lut9.bits.dm_trans_band_lut_p22 = dm_api_reg->trans_band[22]; /* 22: index */
    vpss_vir_reg->vpss_dm_lut9.bits.dm_trans_band_lut_p21 = dm_api_reg->trans_band[21]; /* 21: index */
    vpss_vir_reg->vpss_dm_lut9.bits.dm_trans_band_lut_p20 = dm_api_reg->trans_band[20]; /* 20: index */

    vpss_vir_reg->vpss_dm_lut10.bits.dm_trans_band_lut_p29 = dm_api_reg->trans_band[29]; /* 29: index */
    vpss_vir_reg->vpss_dm_lut10.bits.dm_trans_band_lut_p28 = dm_api_reg->trans_band[28]; /* 28: index */
    vpss_vir_reg->vpss_dm_lut10.bits.dm_trans_band_lut_p27 = dm_api_reg->trans_band[27]; /* 27: index */
    vpss_vir_reg->vpss_dm_lut10.bits.dm_trans_band_lut_p26 = dm_api_reg->trans_band[26]; /* 26: index */
    vpss_vir_reg->vpss_dm_lut10.bits.dm_trans_band_lut_p25 = dm_api_reg->trans_band[25]; /* 25: index */

    vpss_vir_reg->vpss_dm_lut11.bits.dm_trans_band_lut_p30 = dm_api_reg->trans_band[30]; /* 30: index */

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_dm_px_top_lft(hi_u32 handle_no, hi_drv_pq_dm_output_reg *dm_api_reg)
{
    vpss_reg_type *vpss_vir_reg = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(dm_api_reg);
    vpss_vir_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_vir_reg);

    /* x_tl */
    vpss_vir_reg->vpss_dm_subpx_tl0.bits.dm_rosub_px_toplft0 = dm_api_reg->rosub_buf[0].px_top_lft;
    vpss_vir_reg->vpss_dm_subpx_tl0.bits.dm_rosub_px_toplft1 = dm_api_reg->rosub_buf[1].px_top_lft;
    vpss_vir_reg->vpss_dm_subpx_tl0.bits.dm_rosub_px_toplft2 = dm_api_reg->rosub_buf[2].px_top_lft; /* 2: index */

    vpss_vir_reg->vpss_dm_subpx_tl1.bits.dm_rosub_px_toplft3 = dm_api_reg->rosub_buf[3].px_top_lft; /* 3: index */
    vpss_vir_reg->vpss_dm_subpx_tl1.bits.dm_rosub_px_toplft4 = dm_api_reg->rosub_buf[4].px_top_lft; /* 4: index */
    vpss_vir_reg->vpss_dm_subpx_tl1.bits.dm_rosub_px_toplft5 = dm_api_reg->rosub_buf[5].px_top_lft; /* 5: index */

    vpss_vir_reg->vpss_dm_subpx_tl2.bits.dm_rosub_px_toplft6 = dm_api_reg->rosub_buf[6].px_top_lft; /* 6: index */
    vpss_vir_reg->vpss_dm_subpx_tl2.bits.dm_rosub_px_toplft7 = dm_api_reg->rosub_buf[7].px_top_lft; /* 7: index */
    vpss_vir_reg->vpss_dm_subpx_tl2.bits.dm_rosub_px_toplft8 = dm_api_reg->rosub_buf[8].px_top_lft; /* 8: index */

    vpss_vir_reg->vpss_dm_subpx_tl3.bits.dm_rosub_px_toplft9 = dm_api_reg->rosub_buf[9].px_top_lft;   /* 9: index */
    vpss_vir_reg->vpss_dm_subpx_tl3.bits.dm_rosub_px_toplft10 = dm_api_reg->rosub_buf[10].px_top_lft; /* 10: index */
    vpss_vir_reg->vpss_dm_subpx_tl3.bits.dm_rosub_px_toplft11 = dm_api_reg->rosub_buf[11].px_top_lft; /* 11: index */

    vpss_vir_reg->vpss_dm_subpx_tl4.bits.dm_rosub_px_toplft12 = dm_api_reg->rosub_buf[12].px_top_lft; /* 12: index */
    vpss_vir_reg->vpss_dm_subpx_tl4.bits.dm_rosub_px_toplft13 = dm_api_reg->rosub_buf[13].px_top_lft; /* 13: index */
    vpss_vir_reg->vpss_dm_subpx_tl4.bits.dm_rosub_px_toplft14 = dm_api_reg->rosub_buf[14].px_top_lft; /* 14: index */

    vpss_vir_reg->vpss_dm_subpx_tl5.bits.dm_rosub_px_toplft15 = dm_api_reg->rosub_buf[15].px_top_lft; /* 15: index */
    vpss_vir_reg->vpss_dm_subpx_tl5.bits.dm_rosub_px_toplft16 = dm_api_reg->rosub_buf[16].px_top_lft; /* 16: index */
    vpss_vir_reg->vpss_dm_subpx_tl5.bits.dm_rosub_px_toplft17 = dm_api_reg->rosub_buf[17].px_top_lft; /* 17: index */

    vpss_vir_reg->vpss_dm_subpx_tl6.bits.dm_rosub_px_toplft18 = dm_api_reg->rosub_buf[18].px_top_lft; /* 18: index */
    vpss_vir_reg->vpss_dm_subpx_tl6.bits.dm_rosub_px_toplft19 = dm_api_reg->rosub_buf[19].px_top_lft; /* 19: index */
    vpss_vir_reg->vpss_dm_subpx_tl6.bits.dm_rosub_px_toplft20 = dm_api_reg->rosub_buf[20].px_top_lft; /* 20: index */

    vpss_vir_reg->vpss_dm_subpx_tl7.bits.dm_rosub_px_toplft21 = dm_api_reg->rosub_buf[21].px_top_lft; /* 21: index */
    vpss_vir_reg->vpss_dm_subpx_tl7.bits.dm_rosub_px_toplft22 = dm_api_reg->rosub_buf[22].px_top_lft; /* 22: index */
    vpss_vir_reg->vpss_dm_subpx_tl7.bits.dm_rosub_px_toplft23 = dm_api_reg->rosub_buf[23].px_top_lft; /* 23: index */

    vpss_vir_reg->vpss_dm_subpx_tl8.bits.dm_rosub_px_toplft24 = dm_api_reg->rosub_buf[24].px_top_lft; /* 24: index */
    vpss_vir_reg->vpss_dm_subpx_tl8.bits.dm_rosub_px_toplft25 = dm_api_reg->rosub_buf[25].px_top_lft; /* 25: index */
    vpss_vir_reg->vpss_dm_subpx_tl8.bits.dm_rosub_px_toplft26 = dm_api_reg->rosub_buf[26].px_top_lft; /* 26: index */

    vpss_vir_reg->vpss_dm_subpx_tl9.bits.dm_rosub_px_toplft27 = dm_api_reg->rosub_buf[27].px_top_lft; /* 27: index */
    vpss_vir_reg->vpss_dm_subpx_tl9.bits.dm_rosub_px_toplft28 = dm_api_reg->rosub_buf[28].px_top_lft; /* 28: index */
    vpss_vir_reg->vpss_dm_subpx_tl9.bits.dm_rosub_px_toplft29 = dm_api_reg->rosub_buf[29].px_top_lft; /* 29: index */

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_dm_py_top_lft(hi_u32 handle_no, hi_drv_pq_dm_output_reg *dm_api_reg)
{
    vpss_reg_type *vpss_vir_reg = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(dm_api_reg);
    vpss_vir_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_vir_reg);

    /* y_tl */
    vpss_vir_reg->vpss_dm_subpy_tl0.bits.dm_rosub_py_toplft0 = dm_api_reg->rosub_buf[0].py_top_lft;
    vpss_vir_reg->vpss_dm_subpy_tl0.bits.dm_rosub_py_toplft1 = dm_api_reg->rosub_buf[1].py_top_lft;
    vpss_vir_reg->vpss_dm_subpy_tl0.bits.dm_rosub_py_toplft2 = dm_api_reg->rosub_buf[2].py_top_lft; /* 2: index */

    vpss_vir_reg->vpss_dm_subpy_tl1.bits.dm_rosub_py_toplft3 = dm_api_reg->rosub_buf[3].py_top_lft; /* 3: index */
    vpss_vir_reg->vpss_dm_subpy_tl1.bits.dm_rosub_py_toplft4 = dm_api_reg->rosub_buf[4].py_top_lft; /* 4: index */
    vpss_vir_reg->vpss_dm_subpy_tl1.bits.dm_rosub_py_toplft5 = dm_api_reg->rosub_buf[5].py_top_lft; /* 5: index */

    vpss_vir_reg->vpss_dm_subpy_tl2.bits.dm_rosub_py_toplft6 = dm_api_reg->rosub_buf[6].py_top_lft; /* 6: index */
    vpss_vir_reg->vpss_dm_subpy_tl2.bits.dm_rosub_py_toplft7 = dm_api_reg->rosub_buf[7].py_top_lft; /* 7: index */
    vpss_vir_reg->vpss_dm_subpy_tl2.bits.dm_rosub_py_toplft8 = dm_api_reg->rosub_buf[8].py_top_lft; /* 8: index */

    vpss_vir_reg->vpss_dm_subpy_tl3.bits.dm_rosub_py_toplft9 = dm_api_reg->rosub_buf[9].py_top_lft;   /* 9: index */
    vpss_vir_reg->vpss_dm_subpy_tl3.bits.dm_rosub_py_toplft10 = dm_api_reg->rosub_buf[10].py_top_lft; /* 10: index */
    vpss_vir_reg->vpss_dm_subpy_tl3.bits.dm_rosub_py_toplft11 = dm_api_reg->rosub_buf[11].py_top_lft; /* 11: index */

    vpss_vir_reg->vpss_dm_subpy_tl4.bits.dm_rosub_py_toplft12 = dm_api_reg->rosub_buf[12].py_top_lft; /* 12: index */
    vpss_vir_reg->vpss_dm_subpy_tl4.bits.dm_rosub_py_toplft13 = dm_api_reg->rosub_buf[13].py_top_lft; /* 13: index */
    vpss_vir_reg->vpss_dm_subpy_tl4.bits.dm_rosub_py_toplft14 = dm_api_reg->rosub_buf[14].py_top_lft; /* 14: index */

    vpss_vir_reg->vpss_dm_subpy_tl5.bits.dm_rosub_py_toplft15 = dm_api_reg->rosub_buf[15].py_top_lft; /* 15: index */
    vpss_vir_reg->vpss_dm_subpy_tl5.bits.dm_rosub_py_toplft16 = dm_api_reg->rosub_buf[16].py_top_lft; /* 16: index */
    vpss_vir_reg->vpss_dm_subpy_tl5.bits.dm_rosub_py_toplft17 = dm_api_reg->rosub_buf[17].py_top_lft; /* 17: index */

    vpss_vir_reg->vpss_dm_subpy_tl6.bits.dm_rosub_py_toplft18 = dm_api_reg->rosub_buf[18].py_top_lft; /* 18: index */
    vpss_vir_reg->vpss_dm_subpy_tl6.bits.dm_rosub_py_toplft19 = dm_api_reg->rosub_buf[19].py_top_lft; /* 19: index */
    vpss_vir_reg->vpss_dm_subpy_tl6.bits.dm_rosub_py_toplft20 = dm_api_reg->rosub_buf[20].py_top_lft; /* 20: index */

    vpss_vir_reg->vpss_dm_subpy_tl7.bits.dm_rosub_py_toplft21 = dm_api_reg->rosub_buf[21].py_top_lft; /* 21: index */
    vpss_vir_reg->vpss_dm_subpy_tl7.bits.dm_rosub_py_toplft22 = dm_api_reg->rosub_buf[22].py_top_lft; /* 22: index */
    vpss_vir_reg->vpss_dm_subpy_tl7.bits.dm_rosub_py_toplft23 = dm_api_reg->rosub_buf[23].py_top_lft; /* 23: index */

    vpss_vir_reg->vpss_dm_subpy_tl8.bits.dm_rosub_py_toplft24 = dm_api_reg->rosub_buf[24].py_top_lft; /* 24: index */
    vpss_vir_reg->vpss_dm_subpy_tl8.bits.dm_rosub_py_toplft25 = dm_api_reg->rosub_buf[25].py_top_lft; /* 25: index */
    vpss_vir_reg->vpss_dm_subpy_tl8.bits.dm_rosub_py_toplft26 = dm_api_reg->rosub_buf[26].py_top_lft; /* 26: index */

    vpss_vir_reg->vpss_dm_subpy_tl9.bits.dm_rosub_py_toplft27 = dm_api_reg->rosub_buf[27].py_top_lft; /* 27: index */
    vpss_vir_reg->vpss_dm_subpy_tl9.bits.dm_rosub_py_toplft28 = dm_api_reg->rosub_buf[28].py_top_lft; /* 28: index */
    vpss_vir_reg->vpss_dm_subpy_tl9.bits.dm_rosub_py_toplft29 = dm_api_reg->rosub_buf[29].py_top_lft; /* 29: index */

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_dm_px_bot_rgt(hi_u32 handle_no, hi_drv_pq_dm_output_reg *dm_api_reg)
{
    vpss_reg_type *vpss_vir_reg = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(dm_api_reg);
    vpss_vir_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_vir_reg);

    /* x_br */
    vpss_vir_reg->vpss_dm_subpx_br0.bits.dm_rosub_px_botrgt0 = dm_api_reg->rosub_buf[0].px_bot_rgt;
    vpss_vir_reg->vpss_dm_subpx_br0.bits.dm_rosub_px_botrgt1 = dm_api_reg->rosub_buf[1].px_bot_rgt;
    vpss_vir_reg->vpss_dm_subpx_br0.bits.dm_rosub_px_botrgt2 = dm_api_reg->rosub_buf[2].px_bot_rgt; /* 2: index */

    vpss_vir_reg->vpss_dm_subpx_br1.bits.dm_rosub_px_botrgt3 = dm_api_reg->rosub_buf[3].px_bot_rgt; /* 3: index */
    vpss_vir_reg->vpss_dm_subpx_br1.bits.dm_rosub_px_botrgt4 = dm_api_reg->rosub_buf[4].px_bot_rgt; /* 4: index */
    vpss_vir_reg->vpss_dm_subpx_br1.bits.dm_rosub_px_botrgt5 = dm_api_reg->rosub_buf[5].px_bot_rgt; /* 5: index */

    vpss_vir_reg->vpss_dm_subpx_br2.bits.dm_rosub_px_botrgt6 = dm_api_reg->rosub_buf[6].px_bot_rgt; /* 6: index */
    vpss_vir_reg->vpss_dm_subpx_br2.bits.dm_rosub_px_botrgt7 = dm_api_reg->rosub_buf[7].px_bot_rgt; /* 7: index */
    vpss_vir_reg->vpss_dm_subpx_br2.bits.dm_rosub_px_botrgt8 = dm_api_reg->rosub_buf[8].px_bot_rgt; /* 8: index */

    vpss_vir_reg->vpss_dm_subpx_br3.bits.dm_rosub_px_botrgt9 = dm_api_reg->rosub_buf[9].px_bot_rgt;   /* 9: index */
    vpss_vir_reg->vpss_dm_subpx_br3.bits.dm_rosub_px_botrgt10 = dm_api_reg->rosub_buf[10].px_bot_rgt; /* 10: index */
    vpss_vir_reg->vpss_dm_subpx_br3.bits.dm_rosub_px_botrgt11 = dm_api_reg->rosub_buf[11].px_bot_rgt; /* 11: index */

    vpss_vir_reg->vpss_dm_subpx_br4.bits.dm_rosub_px_botrgt12 = dm_api_reg->rosub_buf[12].px_bot_rgt; /* 12: index */
    vpss_vir_reg->vpss_dm_subpx_br4.bits.dm_rosub_px_botrgt13 = dm_api_reg->rosub_buf[13].px_bot_rgt; /* 13: index */
    vpss_vir_reg->vpss_dm_subpx_br4.bits.dm_rosub_px_botrgt14 = dm_api_reg->rosub_buf[14].px_bot_rgt; /* 14: index */

    vpss_vir_reg->vpss_dm_subpx_br5.bits.dm_rosub_px_botrgt15 = dm_api_reg->rosub_buf[15].px_bot_rgt; /* 15: index */
    vpss_vir_reg->vpss_dm_subpx_br5.bits.dm_rosub_px_botrgt16 = dm_api_reg->rosub_buf[16].px_bot_rgt; /* 16: index */
    vpss_vir_reg->vpss_dm_subpx_br5.bits.dm_rosub_px_botrgt17 = dm_api_reg->rosub_buf[17].px_bot_rgt; /* 17: index */

    vpss_vir_reg->vpss_dm_subpx_br6.bits.dm_rosub_px_botrgt18 = dm_api_reg->rosub_buf[18].px_bot_rgt; /* 18: index */
    vpss_vir_reg->vpss_dm_subpx_br6.bits.dm_rosub_px_botrgt19 = dm_api_reg->rosub_buf[19].px_bot_rgt; /* 19: index */
    vpss_vir_reg->vpss_dm_subpx_br6.bits.dm_rosub_px_botrgt20 = dm_api_reg->rosub_buf[20].px_bot_rgt; /* 20: index */

    vpss_vir_reg->vpss_dm_subpx_br7.bits.dm_rosub_px_botrgt21 = dm_api_reg->rosub_buf[21].px_bot_rgt; /* 21: index */
    vpss_vir_reg->vpss_dm_subpx_br7.bits.dm_rosub_px_botrgt22 = dm_api_reg->rosub_buf[22].px_bot_rgt; /* 22: index */
    vpss_vir_reg->vpss_dm_subpx_br7.bits.dm_rosub_px_botrgt23 = dm_api_reg->rosub_buf[23].px_bot_rgt; /* 23: index */

    vpss_vir_reg->vpss_dm_subpx_br8.bits.dm_rosub_px_botrgt24 = dm_api_reg->rosub_buf[24].px_bot_rgt; /* 24: index */
    vpss_vir_reg->vpss_dm_subpx_br8.bits.dm_rosub_px_botrgt25 = dm_api_reg->rosub_buf[25].px_bot_rgt; /* 25: index */
    vpss_vir_reg->vpss_dm_subpx_br8.bits.dm_rosub_px_botrgt26 = dm_api_reg->rosub_buf[26].px_bot_rgt; /* 26: index */

    vpss_vir_reg->vpss_dm_subpx_br9.bits.dm_rosub_px_botrgt27 = dm_api_reg->rosub_buf[27].px_bot_rgt; /* 27: index */
    vpss_vir_reg->vpss_dm_subpx_br9.bits.dm_rosub_px_botrgt28 = dm_api_reg->rosub_buf[28].px_bot_rgt; /* 28: index */
    vpss_vir_reg->vpss_dm_subpx_br9.bits.dm_rosub_px_botrgt29 = dm_api_reg->rosub_buf[29].px_bot_rgt; /* 29: index */

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_dm_py_bot_rgt(hi_u32 handle_no, hi_drv_pq_dm_output_reg *dm_api_reg)
{
    vpss_reg_type *vpss_vir_reg = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(dm_api_reg);
    vpss_vir_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_vir_reg);

    /* y_br */
    vpss_vir_reg->vpss_dm_subpy_br0.bits.dm_rosub_py_botrgt0 = dm_api_reg->rosub_buf[0].py_bot_rgt;
    vpss_vir_reg->vpss_dm_subpy_br0.bits.dm_rosub_py_botrgt1 = dm_api_reg->rosub_buf[1].py_bot_rgt;
    vpss_vir_reg->vpss_dm_subpy_br0.bits.dm_rosub_py_botrgt2 = dm_api_reg->rosub_buf[2].py_bot_rgt; /* 2: index */

    vpss_vir_reg->vpss_dm_subpy_br1.bits.dm_rosub_py_botrgt3 = dm_api_reg->rosub_buf[3].py_bot_rgt; /* 3: index */
    vpss_vir_reg->vpss_dm_subpy_br1.bits.dm_rosub_py_botrgt4 = dm_api_reg->rosub_buf[4].py_bot_rgt; /* 4: index */
    vpss_vir_reg->vpss_dm_subpy_br1.bits.dm_rosub_py_botrgt5 = dm_api_reg->rosub_buf[5].py_bot_rgt; /* 5: index */

    vpss_vir_reg->vpss_dm_subpy_br2.bits.dm_rosub_py_botrgt6 = dm_api_reg->rosub_buf[6].py_bot_rgt; /* 6: index */
    vpss_vir_reg->vpss_dm_subpy_br2.bits.dm_rosub_py_botrgt7 = dm_api_reg->rosub_buf[7].py_bot_rgt; /* 7: index */
    vpss_vir_reg->vpss_dm_subpy_br2.bits.dm_rosub_py_botrgt8 = dm_api_reg->rosub_buf[8].py_bot_rgt; /* 8: index */

    vpss_vir_reg->vpss_dm_subpy_br3.bits.dm_rosub_py_botrgt9 = dm_api_reg->rosub_buf[9].py_bot_rgt;   /* 9: index */
    vpss_vir_reg->vpss_dm_subpy_br3.bits.dm_rosub_py_botrgt10 = dm_api_reg->rosub_buf[10].py_bot_rgt; /* 10: index */
    vpss_vir_reg->vpss_dm_subpy_br3.bits.dm_rosub_py_botrgt11 = dm_api_reg->rosub_buf[11].py_bot_rgt; /* 11: index */

    vpss_vir_reg->vpss_dm_subpy_br4.bits.dm_rosub_py_botrgt12 = dm_api_reg->rosub_buf[12].py_bot_rgt; /* 12: index */
    vpss_vir_reg->vpss_dm_subpy_br4.bits.dm_rosub_py_botrgt13 = dm_api_reg->rosub_buf[13].py_bot_rgt; /* 13: index */
    vpss_vir_reg->vpss_dm_subpy_br4.bits.dm_rosub_py_botrgt14 = dm_api_reg->rosub_buf[14].py_bot_rgt; /* 14: index */

    vpss_vir_reg->vpss_dm_subpy_br5.bits.dm_rosub_py_botrgt15 = dm_api_reg->rosub_buf[15].py_bot_rgt; /* 15: index */
    vpss_vir_reg->vpss_dm_subpy_br5.bits.dm_rosub_py_botrgt16 = dm_api_reg->rosub_buf[16].py_bot_rgt; /* 16: index */
    vpss_vir_reg->vpss_dm_subpy_br5.bits.dm_rosub_py_botrgt17 = dm_api_reg->rosub_buf[17].py_bot_rgt; /* 17: index */

    vpss_vir_reg->vpss_dm_subpy_br6.bits.dm_rosub_py_botrgt18 = dm_api_reg->rosub_buf[18].py_bot_rgt; /* 18: index */
    vpss_vir_reg->vpss_dm_subpy_br6.bits.dm_rosub_py_botrgt19 = dm_api_reg->rosub_buf[19].py_bot_rgt; /* 19: index */
    vpss_vir_reg->vpss_dm_subpy_br6.bits.dm_rosub_py_botrgt20 = dm_api_reg->rosub_buf[20].py_bot_rgt; /* 20: index */

    vpss_vir_reg->vpss_dm_subpy_br7.bits.dm_rosub_py_botrgt21 = dm_api_reg->rosub_buf[21].py_bot_rgt; /* 21: index */
    vpss_vir_reg->vpss_dm_subpy_br7.bits.dm_rosub_py_botrgt22 = dm_api_reg->rosub_buf[22].py_bot_rgt; /* 22: index */
    vpss_vir_reg->vpss_dm_subpy_br7.bits.dm_rosub_py_botrgt23 = dm_api_reg->rosub_buf[23].py_bot_rgt; /* 23: index */

    vpss_vir_reg->vpss_dm_subpy_br8.bits.dm_rosub_py_botrgt24 = dm_api_reg->rosub_buf[24].py_bot_rgt; /* 24: index */
    vpss_vir_reg->vpss_dm_subpy_br8.bits.dm_rosub_py_botrgt25 = dm_api_reg->rosub_buf[25].py_bot_rgt; /* 25: index */
    vpss_vir_reg->vpss_dm_subpy_br8.bits.dm_rosub_py_botrgt26 = dm_api_reg->rosub_buf[26].py_bot_rgt; /* 26: index */

    vpss_vir_reg->vpss_dm_subpy_br9.bits.dm_rosub_py_botrgt27 = dm_api_reg->rosub_buf[27].py_bot_rgt; /* 27: index */
    vpss_vir_reg->vpss_dm_subpy_br9.bits.dm_rosub_py_botrgt28 = dm_api_reg->rosub_buf[28].py_bot_rgt; /* 28: index */
    vpss_vir_reg->vpss_dm_subpy_br9.bits.dm_rosub_py_botrgt29 = dm_api_reg->rosub_buf[29].py_bot_rgt; /* 29: index */

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_dm_api_reg(hi_u32 handle_no, hi_drv_pq_dm_output_reg *dm_api_reg)
{
    hi_s32 ret;
    vpss_reg_type *vpss_vir_reg = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(dm_api_reg);
    vpss_vir_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_vir_reg);

    vpss_vir_reg->vpss_dm_dir.bits.dm_opp_ang_ctrst_t = dm_api_reg->opp_ang_ctrst_t;
    vpss_vir_reg->vpss_dm_dirc.bits.dm_mndir_opp_ctrst_t = dm_api_reg->mn_dir_opp_ctrst_t;

    vpss_vir_reg->vpss_dm_edge.bits.dm_init_val_step = dm_api_reg->init_val_step;
    vpss_vir_reg->vpss_dm_edge.bits.dm_mmf_set = dm_api_reg->mmf_set;
    vpss_vir_reg->vpss_dm_edge.bits.dm_global_str = dm_api_reg->dm_global_str;

    vpss_vir_reg->vpss_dm_lut1.bits.dm_limit_t_10 = dm_api_reg->limit_t10bit;
    vpss_vir_reg->vpss_dm_lut1.bits.dm_limit_t = dm_api_reg->limit_t;

    vpss_vir_reg->vpss_dm_dirc.bits.dm_mmf_limit_en = dm_api_reg->mmf_limit_en;

    vpss_vir_reg->vpss_dm_lsw.bits.dm_lsw_ratio = dm_api_reg->lsw_ratio;
    vpss_vir_reg->vpss_dm_lsw.bits.dm_csw_trsnt_st = dm_api_reg->csw_trsnt_st;
    vpss_vir_reg->vpss_dm_lsw.bits.dm_lw_ctrst_t_10 = dm_api_reg->lwc_trst_t10bit;
    vpss_vir_reg->vpss_dm_lsw.bits.dm_lw_ctrst_t = dm_api_reg->lwc_trst_t;

    vpss_vir_reg->vpss_dm_mmflr.bits.dm_csw_trsnt_lt = dm_api_reg->csw_trsnt_lt;
    vpss_vir_reg->vpss_dm_mmflr.bits.dm_mmf_lr_10 = dm_api_reg->mmf_lr10bit;
    vpss_vir_reg->vpss_dm_mmflr.bits.dm_mmf_lr = dm_api_reg->mmf_lr;

    vpss_vir_reg->vpss_dm_mmfsr.bits.dm_csw_trsnt_lt_10 = dm_api_reg->csw_trsnt_lt10bit;
    vpss_vir_reg->vpss_dm_mmfsr.bits.dm_mmf_sr_10 = dm_api_reg->mmf_sr10bit;
    vpss_vir_reg->vpss_dm_mmfsr.bits.dm_mmf_sr = dm_api_reg->mmf_sr;

    vpss_vir_reg->vpss_dm_thr0.bits.dm_dir_blend_str = dm_api_reg->dir_blend_str;
    vpss_vir_reg->vpss_dm_thr0.bits.dm_lim_res_blend_str2 = dm_api_reg->lim_res_blend_str2;
    vpss_vir_reg->vpss_dm_thr0.bits.dm_lim_res_blend_str1 = dm_api_reg->lim_res_blend_str1;
    vpss_vir_reg->vpss_dm_thr0.bits.dm_limit_lsw_ratio = dm_api_reg->limit_lsw_ratio;

    vpss_vir_reg->vpss_dm_diro.bits.dm_sizeof_rosub = dm_api_reg->size_of_rosub;
    vpss_vir_reg->vpss_dm_diro.bits.dm_csw_trsnt_st_10 = dm_api_reg->csw_trsnt_st10bit;

    ret = pq_hal_set_dm_trans_band(handle_no, dm_api_reg);
    PQ_CHECK_RETURN_SUCCESS(ret);
    ret = pq_hal_set_dm_px_top_lft(handle_no, dm_api_reg);
    PQ_CHECK_RETURN_SUCCESS(ret);
    ret = pq_hal_set_dm_py_top_lft(handle_no, dm_api_reg);
    PQ_CHECK_RETURN_SUCCESS(ret);
    ret = pq_hal_set_dm_px_bot_rgt(handle_no, dm_api_reg);
    PQ_CHECK_RETURN_SUCCESS(ret);
    ret = pq_hal_set_dm_py_bot_rgt(handle_no, dm_api_reg);

    return ret;
}
