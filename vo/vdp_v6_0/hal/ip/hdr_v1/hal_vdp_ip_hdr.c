/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_comm.h"
#include "hal_vdp_ip_hdr.h"
#include "hal_vdp_ip_para.h"

#include "hal_vdp_reg_para.h"
#include "hal_vdp_reg_composer.h"
#include "hal_vdp_reg_tchdr.h"
#include "hal_vdp_reg_tmap_v2.h"
#include "hal_vdp_reg_vhdr_cm.h"
#include "hal_vdp_reg_hihdr_v2.h"
#include "hal_vdp_reg_vid.h"

#ifdef VDP_CBB_TEST_SUPPORT
#include "hal_vdp_test_hdr.h"
#endif

static hi_u32 g_hdr_offset_addr[VDP_LAYER_VID_BUTT] = {
    VID1_HDR_OFFSET,
    0,
    0,
    VID0_HDR_OFFSET,
    0,
};

static hi_void vdp_hdr_set_composer_mode(vdp_regs_type *vdp_reg, hi_u32 offset, hal_composer_info *comp_cfg)
{
    if (comp_cfg == HI_NULL) {
        return;
    }

    /* composer ctrl */
    vdp_vid_setdmcomposeren       (vdp_reg, offset, comp_cfg->comp_enable);
    vdp_vid_setdmstben            (vdp_reg, offset, comp_cfg->stb_enable);
    vdp_vid_setcomposerstbmode    (vdp_reg, offset, comp_cfg->mode);
    vdp_vid_setcomposervdrbitdepth(vdp_reg, offset, comp_cfg->bit_depth);

    /* min/max */
    vdp_vid_setcomposerupsamplemax(vdp_reg, offset, comp_cfg->max);
    vdp_vid_setcomposerupsamplemin(vdp_reg, offset, comp_cfg->min);

    /* poly ctrl */
    vdp_vid_setcomposerblen(vdp_reg, offset, comp_cfg->bl_enable);
    vdp_vid_setcomposerblbitdepth(vdp_reg, offset, comp_cfg->bl_bit_depth);
    vdp_vid_setcomposerblnumpivotsy(vdp_reg, offset, comp_cfg->bl_num_pivots);

    /* nldq ctrl */
    vdp_vid_setcomposerelen(vdp_reg, offset, comp_cfg->el_enable);
    vdp_vid_setcomposerelbitdepth(vdp_reg, offset, comp_cfg->el_zme_en);
    vdp_vid_setcomposerelzmeen(vdp_reg, offset, comp_cfg->el_bit_depth);

    /* mmr ctrl */
    vdp_vid_setcomposerblnumpivotsu(vdp_reg, offset, comp_cfg->bl_num_pivots_u);
    vdp_vid_setcomposerblnumpivotsv(vdp_reg, offset, comp_cfg->bl_num_pivots_v);
    vdp_vid_setcomposermappingidcu(vdp_reg, offset, comp_cfg->mapping_idc_u);
    vdp_vid_setcomposermappingidcv(vdp_reg, offset, comp_cfg->mapping_idc_v);

    /* poly coef */
    vdp_vid_setcomposerblpolypivotvaluey(vdp_reg, offset, comp_cfg->bl_pivot_value_y);
    vdp_vid_setcomposerblpolypivotvalueu(vdp_reg, offset, comp_cfg->bl_pivot_value_u);
    vdp_vid_setcomposerblpolypivotvaluev(vdp_reg, offset, comp_cfg->bl_pivot_value_v);
    vdp_vid_setcomposerblpolyordery(vdp_reg, offset, comp_cfg->bl_poly_order_y);
    vdp_vid_setcomposerblpolyorderu(vdp_reg, offset, comp_cfg->bl_poly_order_u);
    vdp_vid_setcomposerblpolyorderv(vdp_reg, offset, comp_cfg->bl_poly_order_v);
    vdp_vid_setcomposerblpolycoefy(vdp_reg, offset, comp_cfg->bl_coef_y);
    vdp_vid_setcomposerblpolycoefu(vdp_reg, offset, comp_cfg->bl_coef_u);
    vdp_vid_setcomposerblpolycoefv(vdp_reg, offset, comp_cfg->bl_coef_v);

    /* mmr coef */
    vdp_vid_setcomposerblmmrorderu(vdp_reg, offset, comp_cfg->bl_mmr_order_u);
    vdp_vid_setcomposerblmmrorderv(vdp_reg, offset, comp_cfg->bl_mmr_order_v);
    vdp_vid_setcomposerblmmrcoefu(vdp_reg, offset, comp_cfg->bl_mmr_coef_u);
    vdp_vid_setcomposerblmmrcoefv(vdp_reg, offset, comp_cfg->bl_mmr_coef_v);

    /* nldq coef */
    vdp_vid_setcomposerelnldqoffsety(vdp_reg, offset, comp_cfg->el_nldq_offset_y);
    vdp_vid_setcomposerelnldqoffsetu(vdp_reg, offset, comp_cfg->el_nldq_offset_u);
    vdp_vid_setcomposerelnldqoffsetv(vdp_reg, offset, comp_cfg->el_nldq_offset_v);
    vdp_vid_setcomposerelnldqcoefy(vdp_reg, offset, comp_cfg->el_nldq_coef_y);
    vdp_vid_setcomposerelnldqcoefu(vdp_reg, offset, comp_cfg->el_nldq_coef_u);
    vdp_vid_setcomposerelnldqcoefv(vdp_reg, offset, comp_cfg->el_nldq_coef_v);

    return ;
}

static hi_void vdp_hdr_set_tecnicolor_mode(vdp_regs_type *vdp_reg, hi_u32 offset, hal_tc_cfg *tc_cfg)
{
    if (tc_cfg == HI_NULL) {
        return;
    }

    vdp_tchdr_settchdren(vdp_reg, offset, tc_cfg->enable);
    vdp_tchdr_settchdrckgten(vdp_reg, offset, HI_TRUE);

    vdp_tchdr_settchdryuvrange(vdp_reg, offset, tc_cfg->yuv_range);
    vdp_tchdr_settchdralphab(vdp_reg, offset, tc_cfg->alpha_b);
    vdp_tchdr_settchdralphaa(vdp_reg, offset, tc_cfg->alpha_a);
    vdp_tchdr_settchdroct2(vdp_reg, offset, tc_cfg->oct[2]); // 2 is reg index
    vdp_tchdr_settchdroct1(vdp_reg, offset, tc_cfg->oct[1]); // 1 is reg index
    vdp_tchdr_settchdroct0(vdp_reg, offset, tc_cfg->oct[0]); // 0 is reg index
    vdp_tchdr_settchdroct5(vdp_reg, offset, tc_cfg->oct[5]); // 5 is reg index
    vdp_tchdr_settchdroct4(vdp_reg, offset, tc_cfg->oct[4]); // 4 is reg index
    vdp_tchdr_settchdroct3(vdp_reg, offset, tc_cfg->oct[3]); // 3 is reg index
    vdp_tchdr_settchdroct6(vdp_reg, offset, tc_cfg->oct[6]); // 6 is reg index
    vdp_tchdr_settchdrdlutstep3(vdp_reg, offset, tc_cfg->d_lut_step[3]); // 3 is reg index
    vdp_tchdr_settchdrdlutstep2(vdp_reg, offset, tc_cfg->d_lut_step[2]); // 2 is reg index
    vdp_tchdr_settchdrdlutstep1(vdp_reg, offset, tc_cfg->d_lut_step[1]);
    vdp_tchdr_settchdrdlutstep0(vdp_reg, offset, tc_cfg->d_lut_step[0]);
    vdp_tchdr_settchdrdlutthr1(vdp_reg, offset, tc_cfg->d_lut_thr[1]);
    vdp_tchdr_settchdrdlutthr0(vdp_reg, offset, tc_cfg->d_lut_thr[0]);
    vdp_tchdr_settchdrdlutthr2(vdp_reg, offset, tc_cfg->d_lut_thr[2]); // 2 is reg index
    vdp_tchdr_settchdrr2ycoef1(vdp_reg, offset, tc_cfg->r2y_coef[1]);
    vdp_tchdr_settchdrr2ycoef0(vdp_reg, offset, tc_cfg->r2y_coef[0]);
    vdp_tchdr_settchdrr2ycoef3(vdp_reg, offset, tc_cfg->r2y_coef[3]); // 3 is reg index
    vdp_tchdr_settchdrr2ycoef2(vdp_reg, offset, tc_cfg->r2y_coef[2]); // 2 is reg index
    vdp_tchdr_settchdrr2ycoef5(vdp_reg, offset, tc_cfg->r2y_coef[5]); // 5 is reg index
    vdp_tchdr_settchdrr2ycoef4(vdp_reg, offset, tc_cfg->r2y_coef[4]); // 4 is reg index
    vdp_tchdr_settchdrr2ycoef7(vdp_reg, offset, tc_cfg->r2y_coef[7]); // 7 is reg index
    vdp_tchdr_settchdrr2ycoef6(vdp_reg, offset, tc_cfg->r2y_coef[6]); // 6 is reg index
    vdp_tchdr_settchdrr2ycoef8(vdp_reg, offset, tc_cfg->r2y_coef[8]); // 8 is reg index
    vdp_tchdr_settchdrr2yscale2p(vdp_reg, offset, tc_cfg->r2y_scale2p);
    vdp_tchdr_settchdrr2youtdc0(vdp_reg, offset, tc_cfg->r2y_out_dc[0]); // 0 is reg index
    vdp_tchdr_settchdrr2youtdc1(vdp_reg, offset, tc_cfg->r2y_out_dc[1]);
    vdp_tchdr_settchdrr2youtdc2(vdp_reg, offset, tc_cfg->r2y_out_dc[2]); // 2 is reg index
    vdp_tchdr_settchdrr2yclipmin(vdp_reg, offset, tc_cfg->r2y_clip_min);
    vdp_tchdr_settchdrr2yclipmax(vdp_reg, offset, tc_cfg->r2y_clip_max);
    vdp_tchdr_settchdrr2yclipen(vdp_reg, offset, tc_cfg->r2y_clip_en);

    vdp_tchdr_settchdrdemopos(vdp_reg, offset, tc_cfg->demo_pos);
    vdp_tchdr_settchdrdemomode(vdp_reg, offset, tc_cfg->demo_mode);
    vdp_tchdr_settchdrdemoen(vdp_reg, offset, tc_cfg->demo_en);

    return ;
}

static hi_void vdp_hdr_set_cm_mode(vdp_regs_type *vdp_reg, hi_u32 offset, vdp_cm_cfg *cm_cfg)
{
    vdp_vhdr_cm_setvhdrcmckgten(vdp_reg, offset, HI_TRUE);
    vdp_vhdr_cm_setvhdrcmen(vdp_reg, offset, cm_cfg->enable);
    vdp_vhdr_cm_setvhdrcmclipcmax(vdp_reg, offset, cm_cfg->clip_c_max);
    vdp_vhdr_cm_setvhdrcmclipcmin(vdp_reg, offset, cm_cfg->clip_c_min);
    vdp_vhdr_cm_setvhdrcmscale2p(vdp_reg, offset, cm_cfg->scale2p);
    vdp_vhdr_cm_setvcmx4step(vdp_reg, offset, cm_cfg->step[3]); // 3 is reg index
    vdp_vhdr_cm_setvcmx3step(vdp_reg, offset, cm_cfg->step[2]); // 2 is reg index
    vdp_vhdr_cm_setvcmx2step(vdp_reg, offset, cm_cfg->step[1]);
    vdp_vhdr_cm_setvcmx1step(vdp_reg, offset, cm_cfg->step[0]);
    vdp_vhdr_cm_setvcmx8step(vdp_reg, offset, cm_cfg->step[7]); // 7 is reg index
    vdp_vhdr_cm_setvcmx7step(vdp_reg, offset, cm_cfg->step[6]); // 6 is reg index
    vdp_vhdr_cm_setvcmx6step(vdp_reg, offset, cm_cfg->step[5]); // 5 is reg index
    vdp_vhdr_cm_setvcmx5step(vdp_reg, offset, cm_cfg->step[4]); // 4 is reg index
    vdp_vhdr_cm_setvcmx1pos(vdp_reg, offset, cm_cfg->pos[1]);
    vdp_vhdr_cm_setvcmx2pos(vdp_reg, offset, cm_cfg->pos[1]);
    vdp_vhdr_cm_setvcmx3pos(vdp_reg, offset, cm_cfg->pos[2]); // 2 is reg index
    vdp_vhdr_cm_setvcmx4pos(vdp_reg, offset, cm_cfg->pos[3]); // 3 is reg index
    vdp_vhdr_cm_setvcmx5pos(vdp_reg, offset, cm_cfg->pos[4]); // 4 is reg index
    vdp_vhdr_cm_setvcmx6pos(vdp_reg, offset, cm_cfg->pos[5]); // 5 is reg index
    vdp_vhdr_cm_setvcmx7pos(vdp_reg, offset, cm_cfg->pos[6]); // 6 is reg index
    vdp_vhdr_cm_setvcmx8pos(vdp_reg, offset, cm_cfg->pos[7]); // 7 is reg index
    vdp_vhdr_cm_setvcmx4num(vdp_reg, offset, cm_cfg->num[3]); // 3 is reg index
    vdp_vhdr_cm_setvcmx3num(vdp_reg, offset, cm_cfg->num[2]); // 2 is reg index
    vdp_vhdr_cm_setvcmx2num(vdp_reg, offset, cm_cfg->num[1]);
    vdp_vhdr_cm_setvcmx1num(vdp_reg, offset, cm_cfg->num[0]);
    vdp_vhdr_cm_setvcmx8num(vdp_reg, offset, cm_cfg->num[7]); // 7 is reg index
    vdp_vhdr_cm_setvcmx7num(vdp_reg, offset, cm_cfg->num[6]); // 6 is reg index
    vdp_vhdr_cm_setvcmx6num(vdp_reg, offset, cm_cfg->num[5]); // 5 is reg index
    vdp_vhdr_cm_setvcmx5num(vdp_reg, offset, cm_cfg->num[4]); // 4 is reg index

    return ;
}

static hi_void vdp_hdr_set_imap_yuv2rgb(vdp_regs_type *vdp_reg, hi_u32 offset, vdp_yuv2rgb_cfg *yuv2rgb)
{
    /* yuv2rgb */
    vdp_hihdr_v2_setimyuv2rgbmode(vdp_reg, offset, yuv2rgb->mode);
    vdp_hihdr_v2_setimv1y2ren(vdp_reg, offset, yuv2rgb->v1_enable);
    vdp_hihdr_v2_setimv0y2ren(vdp_reg, offset, yuv2rgb->v0_enable);
    vdp_hihdr_v2_setimyuv2rgben(vdp_reg, offset, yuv2rgb->enable);
    vdp_hihdr_v2_setimapm33yuv2rgb00(vdp_reg, offset, yuv2rgb->coef[0][0]);
    vdp_hihdr_v2_setimapm33yuv2rgb01(vdp_reg, offset, yuv2rgb->coef[0][1]);
    vdp_hihdr_v2_setimapm33yuv2rgb02(vdp_reg, offset, yuv2rgb->coef[0][2]); // 2 is reg index
    vdp_hihdr_v2_setimapm33yuv2rgb10(vdp_reg, offset, yuv2rgb->coef[1][0]);
    vdp_hihdr_v2_setimapm33yuv2rgb11(vdp_reg, offset, yuv2rgb->coef[1][1]);
    vdp_hihdr_v2_setimapm33yuv2rgb12(vdp_reg, offset, yuv2rgb->coef[1][2]); // 2 is reg index
    vdp_hihdr_v2_setimapm33yuv2rgb20(vdp_reg, offset, yuv2rgb->coef[2][0]); // 2 is reg index
    vdp_hihdr_v2_setimapm33yuv2rgb21(vdp_reg, offset, yuv2rgb->coef[2][1]); // 2 is reg index
    vdp_hihdr_v2_setimapm33yuv2rgb22(vdp_reg, offset, yuv2rgb->coef[2][2]); // 2 is reg index
    vdp_hihdr_v2_setimapm33yuv2rgb200(vdp_reg, offset, yuv2rgb->coef2[0][0]);
    vdp_hihdr_v2_setimapm33yuv2rgb201(vdp_reg, offset, yuv2rgb->coef2[0][1]);
    vdp_hihdr_v2_setimapm33yuv2rgb202(vdp_reg, offset, yuv2rgb->coef2[0][2]); // 2 is reg index
    vdp_hihdr_v2_setimapm33yuv2rgb210(vdp_reg, offset, yuv2rgb->coef2[1][0]);
    vdp_hihdr_v2_setimapm33yuv2rgb211(vdp_reg, offset, yuv2rgb->coef2[1][1]);
    vdp_hihdr_v2_setimapm33yuv2rgb212(vdp_reg, offset, yuv2rgb->coef2[1][2]); // 2 is reg index
    vdp_hihdr_v2_setimapm33yuv2rgb220(vdp_reg, offset, yuv2rgb->coef2[2][0]); // 2 is reg index
    vdp_hihdr_v2_setimapm33yuv2rgb221(vdp_reg, offset, yuv2rgb->coef2[2][1]); // 2 is reg index
    vdp_hihdr_v2_setimapm33yuv2rgb222(vdp_reg, offset, yuv2rgb->coef2[2][2]); // 2 is reg index
    vdp_hihdr_v2_setimapm33yuv2rgbscale2p(vdp_reg, offset, yuv2rgb->scale2p);
    vdp_hihdr_v2_setimv3yuv2rgbdcin0(vdp_reg, offset, yuv2rgb->dc_in[0]);
    vdp_hihdr_v2_setimv3yuv2rgbdcin1(vdp_reg, offset, yuv2rgb->dc_in[1]);
    vdp_hihdr_v2_setimv3yuv2rgbdcin2(vdp_reg, offset, yuv2rgb->dc_in[2]); // 2 is reg index
    vdp_hihdr_v2_setimapv3yuv2rgboffinrgb0(vdp_reg, offset, yuv2rgb->offinrgb[0]);
    vdp_hihdr_v2_setimapv3yuv2rgboffinrgb1(vdp_reg, offset, yuv2rgb->offinrgb[1]);
    vdp_hihdr_v2_setimapv3yuv2rgboffinrgb2(vdp_reg, offset, yuv2rgb->offinrgb[2]); // 2 is reg index
    vdp_hihdr_v2_setimv3yuv2rgb2dcin0(vdp_reg, offset, yuv2rgb->dc_in2[0]);
    vdp_hihdr_v2_setimv3yuv2rgb2dcin1(vdp_reg, offset, yuv2rgb->dc_in2[1]);
    vdp_hihdr_v2_setimv3yuv2rgb2dcin2(vdp_reg, offset, yuv2rgb->dc_in2[2]); // 2 is reg index
    vdp_hihdr_v2_setimapv3yuv2rgb2offinrgb0(vdp_reg, offset, yuv2rgb->offinrgb2[0]);
    vdp_hihdr_v2_setimapv3yuv2rgb2offinrgb1(vdp_reg, offset, yuv2rgb->offinrgb2[0]);
    vdp_hihdr_v2_setimapv3yuv2rgb2offinrgb2(vdp_reg, offset, yuv2rgb->offinrgb2[0]);
    vdp_hihdr_v2_setimyuv2rgbclipmin(vdp_reg, offset, yuv2rgb->clip_min);
    vdp_hihdr_v2_setimyuv2rgbclipmax(vdp_reg, offset, yuv2rgb->clip_max);
    vdp_hihdr_v2_setimyuv2rgbthrr(vdp_reg, offset, yuv2rgb->thr_r);
    vdp_hihdr_v2_setimyuv2rgbthrb(vdp_reg, offset, yuv2rgb->thr_b);
}

static hi_void vdp_hdr_set_imap_rgb2lms(vdp_regs_type *vdp_reg, hi_u32 offset, vdp_rgb2lms_cfg *rgb2lms)
{
    /* rgb2lms */
    vdp_hihdr_v2_setimrgb2lmsen(vdp_reg, offset, rgb2lms->enable);
    vdp_hihdr_v2_setimapm33rgb2lms00(vdp_reg, offset, rgb2lms->coef[0][0]);
    vdp_hihdr_v2_setimapm33rgb2lms01(vdp_reg, offset, rgb2lms->coef[0][1]);
    vdp_hihdr_v2_setimapm33rgb2lms02(vdp_reg, offset, rgb2lms->coef[0][2]); // 2 is reg index
    vdp_hihdr_v2_setimapm33rgb2lms10(vdp_reg, offset, rgb2lms->coef[1][0]);
    vdp_hihdr_v2_setimapm33rgb2lms11(vdp_reg, offset, rgb2lms->coef[1][1]);
    vdp_hihdr_v2_setimapm33rgb2lms12(vdp_reg, offset, rgb2lms->coef[1][2]); // 2 is reg index
    vdp_hihdr_v2_setimapm33rgb2lms20(vdp_reg, offset, rgb2lms->coef[2][0]); // 2 is reg index
    vdp_hihdr_v2_setimapm33rgb2lms21(vdp_reg, offset, rgb2lms->coef[2][1]); // 2 is reg index
    vdp_hihdr_v2_setimapm33rgb2lms22(vdp_reg, offset, rgb2lms->coef[2][2]); // 2 is reg index
    vdp_hihdr_v2_setimapm33rgb2lmsscale2p(vdp_reg, offset, rgb2lms->scale2p);
    vdp_hihdr_v2_setimrgb2lmsclipmin(vdp_reg, offset, rgb2lms->clip_min);
    vdp_hihdr_v2_setimrgb2lmsclipmax(vdp_reg, offset, rgb2lms->clip_max);
    vdp_hihdr_v2_setimv3rgb2lmsdcin0(vdp_reg, offset, rgb2lms->dc_in[0]);
    vdp_hihdr_v2_setimv3rgb2lmsdcin1(vdp_reg, offset, rgb2lms->dc_in[1]);
    vdp_hihdr_v2_setimv3rgb2lmsdcin2(vdp_reg, offset, rgb2lms->dc_in[2]); // 2 is reg index
}

static hi_void vdp_hdr_set_imap_lms2ipt(vdp_regs_type *vdp_reg, hi_u32 offset, vdp_lms2ipt_cfg *lms2ipt)
{
    /* lms2ipt */
    vdp_hihdr_v2_setimlms2ipten(vdp_reg, offset, lms2ipt->enable);
    vdp_hihdr_v2_setimapm33lms2ipt00(vdp_reg, offset, lms2ipt->coef[0][0]);
    vdp_hihdr_v2_setimapm33lms2ipt01(vdp_reg, offset, lms2ipt->coef[0][1]);
    vdp_hihdr_v2_setimapm33lms2ipt02(vdp_reg, offset, lms2ipt->coef[0][2]); // 2 is reg index
    vdp_hihdr_v2_setimapm33lms2ipt10(vdp_reg, offset, lms2ipt->coef[1][0]);
    vdp_hihdr_v2_setimapm33lms2ipt11(vdp_reg, offset, lms2ipt->coef[1][1]);
    vdp_hihdr_v2_setimapm33lms2ipt12(vdp_reg, offset, lms2ipt->coef[1][2]); // 2 is reg index
    vdp_hihdr_v2_setimapm33lms2ipt20(vdp_reg, offset, lms2ipt->coef[2][0]); // 2 is reg index
    vdp_hihdr_v2_setimapm33lms2ipt21(vdp_reg, offset, lms2ipt->coef[2][1]); // 2 is reg index
    vdp_hihdr_v2_setimapm33lms2ipt22(vdp_reg, offset, lms2ipt->coef[2][2]); // 2 is reg index
    vdp_hihdr_v2_setimapm33lms2iptscale2p(vdp_reg, offset, lms2ipt->scale2p);
    vdp_hihdr_v2_setimlms2iptclipminy(vdp_reg, offset, lms2ipt->clip_min_y);
    vdp_hihdr_v2_setimlms2iptclipmaxy(vdp_reg, offset, lms2ipt->clip_max_y);
    vdp_hihdr_v2_setimlms2iptclipminc(vdp_reg, offset, lms2ipt->clip_min_c);
    vdp_hihdr_v2_setimlms2iptclipmaxc(vdp_reg, offset, lms2ipt->clip_max_c);
    vdp_hihdr_v2_setimv3lms2iptdcout0(vdp_reg, offset, lms2ipt->dc_out[0]);
    vdp_hihdr_v2_setimv3lms2iptdcout1(vdp_reg, offset, lms2ipt->dc_out[1]);
    vdp_hihdr_v2_setimv3lms2iptdcout2(vdp_reg, offset, lms2ipt->dc_out[2]); // 2 is reg index
    vdp_hihdr_v2_setimv3lms2iptdcin0(vdp_reg, offset, lms2ipt->dc_in[0]);
    vdp_hihdr_v2_setimv3lms2iptdcin1(vdp_reg, offset, lms2ipt->dc_in[1]);
    vdp_hihdr_v2_setimv3lms2iptdcin2(vdp_reg, offset, lms2ipt->dc_in[2]); // 2 is reg index
}

static hi_void vdp_hdr_set_imap_tmap_v1(vdp_regs_type *vdp_reg, hi_u32 offset, vdp_tmap_v1_cfg *tmap_v1)
{
    /* tmap_v1 */
    vdp_hihdr_v2_setimtmpos(vdp_reg, offset, tmap_v1->tm_pos);
    vdp_hihdr_v2_setimtmv1en(vdp_reg, offset, tmap_v1->enable);
    vdp_hihdr_v2_setimtmscalemixalpha(vdp_reg, offset, tmap_v1->scale_mix_alpha);
    vdp_hihdr_v2_setimtmmixalpha(vdp_reg, offset, tmap_v1->mix_alpha);
    vdp_hihdr_v2_setimtmxstep3(vdp_reg, offset, tmap_v1->step[3]); // 3 is reg index
    vdp_hihdr_v2_setimtmxstep2(vdp_reg, offset, tmap_v1->step[2]); // 2 is reg index
    vdp_hihdr_v2_setimtmxstep1(vdp_reg, offset, tmap_v1->step[1]);
    vdp_hihdr_v2_setimtmxstep0(vdp_reg, offset, tmap_v1->step[0]);
    vdp_hihdr_v2_setimtmxstep7(vdp_reg, offset, tmap_v1->step[7]); // 7 is reg index
    vdp_hihdr_v2_setimtmxstep6(vdp_reg, offset, tmap_v1->step[6]); // 6 is reg index
    vdp_hihdr_v2_setimtmxstep5(vdp_reg, offset, tmap_v1->step[5]); // 5 is reg index
    vdp_hihdr_v2_setimtmxstep4(vdp_reg, offset, tmap_v1->step[4]); // 4 is reg index
    vdp_hihdr_v2_setimtmxpos0(vdp_reg, offset, tmap_v1->pos[0]);
    vdp_hihdr_v2_setimtmxpos1(vdp_reg, offset, tmap_v1->pos[1]);
    vdp_hihdr_v2_setimtmxpos2(vdp_reg, offset, tmap_v1->pos[2]); // 2 is reg index
    vdp_hihdr_v2_setimtmxpos3(vdp_reg, offset, tmap_v1->pos[3]); // 3 is reg index
    vdp_hihdr_v2_setimtmxpos4(vdp_reg, offset, tmap_v1->pos[4]); // 4 is reg index
    vdp_hihdr_v2_setimtmxpos5(vdp_reg, offset, tmap_v1->pos[5]); // 5 is reg index
    vdp_hihdr_v2_setimtmxpos6(vdp_reg, offset, tmap_v1->pos[6]); // 6 is reg index
    vdp_hihdr_v2_setimtmxpos7(vdp_reg, offset, tmap_v1->pos[7]); // 7 is reg index
    vdp_hihdr_v2_setimtmxnum3(vdp_reg, offset, tmap_v1->num[3]); // 3 is reg index
    vdp_hihdr_v2_setimtmxnum2(vdp_reg, offset, tmap_v1->num[2]); // 2 is reg index
    vdp_hihdr_v2_setimtmxnum1(vdp_reg, offset, tmap_v1->num[1]);
    vdp_hihdr_v2_setimtmxnum0(vdp_reg, offset, tmap_v1->num[0]);
    vdp_hihdr_v2_setimtmxnum7(vdp_reg, offset, tmap_v1->num[7]); // 7 is reg index
    vdp_hihdr_v2_setimtmxnum6(vdp_reg, offset, tmap_v1->num[6]); // 6 is reg index
    vdp_hihdr_v2_setimtmxnum5(vdp_reg, offset, tmap_v1->num[5]); // 5 is reg index
    vdp_hihdr_v2_setimtmxnum4(vdp_reg, offset, tmap_v1->num[4]); // 4 is reg index
    vdp_hihdr_v2_setimtmm3lumcal0(vdp_reg, offset, tmap_v1->lum_cal[0]);
    vdp_hihdr_v2_setimtmm3lumcal1(vdp_reg, offset, tmap_v1->lum_cal[1]);
    vdp_hihdr_v2_setimtmm3lumcal2(vdp_reg, offset, tmap_v1->lum_cal[2]); // 2 is reg index
    vdp_hihdr_v2_setimtmscalelumcal(vdp_reg, offset, tmap_v1->scale_lum_cal);
    vdp_hihdr_v2_setimtmscalecoef(vdp_reg, offset, tmap_v1->scale_coef);
    vdp_hihdr_v2_setimtmclipmin(vdp_reg, offset, tmap_v1->clip_min);
    vdp_hihdr_v2_setimtmclipmax(vdp_reg, offset, tmap_v1->clip_max);
    vdp_hihdr_v2_setimtmdcout0(vdp_reg, offset, tmap_v1->dc_out[0]);
    vdp_hihdr_v2_setimtmdcout1(vdp_reg, offset, tmap_v1->dc_out[1]);
    vdp_hihdr_v2_setimtmdcout2(vdp_reg, offset, tmap_v1->dc_out[2]); // 2 is reg index
}

static hi_void vdp_hdr_set_imap_cacm(vdp_regs_type *vdp_reg, hi_u32 offset, vdp_cacm_cfg *cacm)
{
    /* CACM */
    vdp_hihdr_v2_setimcmpos(vdp_reg, offset, cacm->cm_pos);
    vdp_hihdr_v2_setimcmdemopos(vdp_reg, offset, cacm->demo_pos);
    vdp_hihdr_v2_setimcmdemomode(vdp_reg, offset, cacm->demo_mode);
    vdp_hihdr_v2_setimcmdemoen(vdp_reg, offset, cacm->demo_en);
    vdp_hihdr_v2_setimcmbitdepthoutmode(vdp_reg, offset, cacm->bitdepth_out_mode);
    vdp_hihdr_v2_setimcmbitdepthinmode(vdp_reg, offset, cacm->bitdepth_in_mode);
    vdp_hihdr_v2_setimcmckgten(vdp_reg, offset, HI_TRUE);
    vdp_hihdr_v2_setimcmen(vdp_reg, offset, cacm->enable);
}

static hi_void vdp_hdr_set_imap_mode(vdp_regs_type *vdp_reg, hi_u32 offset, vdp_imap_cfg* imap_cfg)
{
    vdp_hihdr_v2_setimrshiftrounden(vdp_reg, offset, imap_cfg->rshift_round_en);
    vdp_hihdr_v2_setimrshiften(vdp_reg, offset, imap_cfg->rshift_en);
    vdp_hihdr_v2_setimckgten(vdp_reg, offset, HI_TRUE);
    vdp_hihdr_v2_setimiptinsel(vdp_reg, offset, imap_cfg->ipt_in_sel);
    vdp_hihdr_v2_setimladjen(vdp_reg, offset, imap_cfg->ladj_en);
    vdp_hihdr_v2_setimapincolor(vdp_reg, offset, imap_cfg->in_color);
    vdp_hihdr_v2_setiminbits(vdp_reg, offset, imap_cfg->in_bits);
    vdp_hihdr_v2_setimgammaen(vdp_reg, offset, imap_cfg->gamma_en);
    vdp_hihdr_v2_setimdegammaen(vdp_reg, offset, imap_cfg->degamma_en);
    vdp_hihdr_v2_setimnormen(vdp_reg, offset, imap_cfg->norm_en);
    vdp_hihdr_v2_setimen(vdp_reg, offset, imap_cfg->enable);
    vdp_hihdr_v2_setimapeotfparameotf(vdp_reg, offset, imap_cfg->eotf);
    vdp_hihdr_v2_setimapeotfparamrangemin(vdp_reg, offset, imap_cfg->range_min);
    vdp_hihdr_v2_setimapeotfparamrange(vdp_reg, offset, imap_cfg->range);
    vdp_hihdr_v2_setimapeotfparamrangeinv(vdp_reg, offset, imap_cfg->range_inv);
    vdp_hihdr_v2_setimapv3iptoff0(vdp_reg, offset, imap_cfg->ipt_off[0]);
    vdp_hihdr_v2_setimapv3iptoff1(vdp_reg, offset, imap_cfg->ipt_off[1]);
    vdp_hihdr_v2_setimapv3iptoff2(vdp_reg, offset, imap_cfg->ipt_off[2]); // 2 is reg index
    vdp_hihdr_v2_setimapiptscale(vdp_reg, offset, imap_cfg->ipt_scale);
    vdp_hihdr_v2_setimdegammaclipmax(vdp_reg, offset, imap_cfg->degamma_clip_max);
    vdp_hihdr_v2_setimdegammaclipmin(vdp_reg, offset, imap_cfg->degamma_clip_min);
    vdp_hihdr_v2_setimapladjchromaweight(vdp_reg, offset, imap_cfg->ladj_chroma_weight);
    vdp_hihdr_v2_setimdemolumamode(vdp_reg, offset, imap_cfg->demo_luma_mode);
    vdp_hihdr_v2_setimdemolumaen(vdp_reg, offset, imap_cfg->demo_luma_en);
    vdp_hihdr_v2_setimv1hdren(vdp_reg, offset, imap_cfg->v1_hdr_enable);
    vdp_hihdr_v2_setimv0hdren(vdp_reg, offset, imap_cfg->v0_hdr_enable);

    vdp_hdr_set_imap_yuv2rgb(vdp_reg, offset, &imap_cfg->yuv2rgb);
    vdp_hdr_set_imap_rgb2lms(vdp_reg, offset, &imap_cfg->rgb2lms);
    vdp_hdr_set_imap_lms2ipt(vdp_reg, offset, &imap_cfg->lms2ipt);
    vdp_hdr_set_imap_tmap_v1(vdp_reg, offset, &imap_cfg->tmap_v1);
    vdp_hdr_set_imap_cacm(vdp_reg, offset, &imap_cfg->cacm);

    return ;
}

static hi_void vdp_hdr_set_tmap_mode(vdp_regs_type *vdp_reg, hi_u32 offset, vdp_tmap_cfg* tmap_cfg)
{
    vdp_tmap_v2_settmrshiftrounden(vdp_reg, offset, tmap_cfg->rshift_round_en);
    vdp_tmap_v2_setstbtonemapen(vdp_reg, offset, tmap_cfg->enable);
    vdp_tmap_v2_settmapsclutrden(vdp_reg, offset, tmap_cfg->sclut_rd_en);
    vdp_tmap_v2_settmapsslutrden(vdp_reg, offset, tmap_cfg->sslut_rd_en);
    vdp_tmap_v2_settmaptslutrden(vdp_reg, offset, tmap_cfg->tslut_rd_en);
    vdp_tmap_v2_settmapsilutrden(vdp_reg, offset, tmap_cfg->silut_rd_en);
    vdp_tmap_v2_settmaptilutrden(vdp_reg, offset, tmap_cfg->tilut_rd_en);
    vdp_tmap_v2_settmapparardata(vdp_reg, offset, tmap_cfg->para_rdata);
    vdp_tmap_v2_settmc1expan(vdp_reg, offset, tmap_cfg->c1_expan);
    vdp_tmap_v2_settmsmcenable(vdp_reg, offset, tmap_cfg->smc_enable);

    vdp_tmap_v2_setimtmrshiftbit(vdp_reg, offset, tmap_cfg->rshift_bit);
    vdp_tmap_v2_setimtmrshiften(vdp_reg, offset, tmap_cfg->rshift_en);
    vdp_tmap_v2_setimtms2uen(vdp_reg, offset, tmap_cfg->s2u_en);

    return ;
}

static hi_void vdp_hdr_set_omap_mode(vdp_regs_type *vdp_reg, hi_u32 offset, vdp_omap_cfg* omap_cfg)
{
    vdp_hipp_dbhdr_setomcmckgten (vdp_reg, HI_TRUE);

    /* cvm */
    vdp_hipp_dbhdr_setomen       (vdp_reg, omap_cfg->enable);
    vdp_hipp_dbhdr_setomu2sen    (vdp_reg, omap_cfg->u2s_enable);
    vdp_hipp_dbhdr_setomlshiften (vdp_reg, omap_cfg->lshift_en);
    vdp_hipp_dbhdr_setomdegammaen(vdp_reg, omap_cfg->degamma_en);
    vdp_hipp_dbhdr_setomgammaen  (vdp_reg, omap_cfg->gamma_en);
    vdp_hipp_dbhdr_setvdenormen  (vdp_reg, omap_cfg->denorm_en);
    vdp_hipp_dbhdr_setomvcmen    (vdp_reg, omap_cfg->cvm_en);

    vdp_hdr_setvcvmomaprangemin       (vdp_reg, omap_cfg->range_min);
    vdp_hdr_setvcvmomaprangeover      (vdp_reg, omap_cfg->range_over);
    vdp_hdr_setcvmoutbits             (vdp_reg, omap_cfg->out_bits);
    vdp_hdr_setcvmoutcolor            (vdp_reg, omap_cfg->out_color);
    vdp_hdr_setvcvmomapiptoff         (vdp_reg, omap_cfg->ipt_off);
    vdp_hdr_setvcvmomapiptscale       (vdp_reg, omap_cfg->scale);
    vdp_hdr_setvcvmoutoetf            (vdp_reg, omap_cfg->oetf);

    /* ipt2lms */
    vdp_hdr_setvcvmipt2lmsen          (vdp_reg, omap_cfg->ipt2lms.enable);
    vdp_hdr_setvdmomapipt2lmsmin      (vdp_reg, omap_cfg->ipt2lms.min);
    vdp_hdr_setvdmomapipt2lmsmax      (vdp_reg, omap_cfg->ipt2lms.max);
    vdp_hdr_setvcvmomapipt2lms        (vdp_reg, omap_cfg->ipt2lms.coef);
    vdp_hdr_setvcvmomapipt2lmsscale2p (vdp_reg, omap_cfg->ipt2lms.scale2p);
    vdp_hdr_setvcvmomapipt2lmsoutdc   (vdp_reg, omap_cfg->ipt2lms.out_dc);
    vdp_hdr_setvcvmomapipt2lmsindc    (vdp_reg, omap_cfg->ipt2lms.in_dc);

    /* lms2rgb */
    vdp_hdr_setvcvmlms2rgben          (vdp_reg, omap_cfg->lms2rgb.enable);
    vdp_hdr_setvdmomaplms2rgbmin      (vdp_reg, omap_cfg->lms2rgb.min);
    vdp_hdr_setvdmomaplms2rgbmax      (vdp_reg, omap_cfg->lms2rgb.max);
    vdp_hdr_setvcvmomaplms2rgb        (vdp_reg, omap_cfg->lms2rgb.coef);
    vdp_hdr_setvcvmomaplms2rgbscale2p (vdp_reg, omap_cfg->lms2rgb.scale2p);

    /* rgb2yuv */
    vdp_hdr_setvcvmrgb2yuven          (vdp_reg, omap_cfg->rgb2yuv.enable);
    vdp_hdr_setvcvmomaprgb2yuv        (vdp_reg, omap_cfg->rgb2yuv.coef);
    vdp_hdr_setvcvmomaprgb2yuvscale2p (vdp_reg, omap_cfg->rgb2yuv.scale2p);
    vdp_hdr_setvcvmomaprgb2yuvoutdc   (vdp_reg, omap_cfg->rgb2yuv.out_dc);
    vdp_hdr_setvcvmomaprgb2yuvindc    (vdp_reg, omap_cfg->rgb2yuv.in_dc);
    vdp_hdr_setvcvmomaprgb2yuvmin     (vdp_reg, omap_cfg->rgb2yuv.min);
    vdp_hdr_setvcvmomaprgb2yuvmax     (vdp_reg, omap_cfg->rgb2yuv.max);

    /* cm */
    vdp_hipp_dbhdr_setomcmen              (vdp_reg, omap_cfg->cm_cfg.enable);
    vdp_hipp_dbhdr_setomcmbitdepthinmode  (vdp_reg, omap_cfg->cm_cfg.bitdepth_in_mode);
    vdp_hipp_dbhdr_setomcmbitdepthoutmode (vdp_reg, omap_cfg->cm_cfg.bitdepth_out_mode);
    vdp_hipp_dbhdr_setomcmdemoen          (vdp_reg, omap_cfg->cm_cfg.demo_en);
    vdp_hipp_dbhdr_setomcmdemomode        (vdp_reg, omap_cfg->cm_cfg.demo_mode);
    vdp_hipp_dbhdr_setomcmdemopos         (vdp_reg, omap_cfg->cm_cfg.demo_pos);

    return ;
}

static hi_void vdp_ip_hdr_set_coef_addr(vdp_hdr_coef_addr *coef_addr)
{
    vdp_para_setparaaddrvhdchn07(g_vdp_reg, coef_addr->degamm_addr);
    vdp_para_setparahaddrvhdchn07(g_vdp_reg, coef_addr->degamm_addr >> 32); /* 32 is number */

    vdp_para_setparaaddrvhdchn08(g_vdp_reg, coef_addr->gamm_addr);
    vdp_para_setparahaddrvhdchn08(g_vdp_reg, coef_addr->gamm_addr >> 32); /* 32 is number */

    vdp_para_setparaaddrvhdchn19(g_vdp_reg,coef_addr->cm_addr);
    vdp_para_setparahaddrvhdchn19(g_vdp_reg, coef_addr->cm_addr >> 32); /* 32 is number */

    vdp_para_setparaaddrv0chn02(g_vdp_reg, coef_addr->hdr1_addr);
    vdp_para_setparahaddrv0chn02(g_vdp_reg, coef_addr->hdr1_addr >> 32); /* 32 is number */

    vdp_para_setparaaddrv0chn03(g_vdp_reg, coef_addr->hdr2_addr);
    vdp_para_setparahaddrv0chn03(g_vdp_reg, coef_addr->hdr2_addr >> 32); /* 32 is number */

    vdp_para_setparaaddrv0chn04(g_vdp_reg, coef_addr->hdr3_addr);
    vdp_para_setparahaddrv0chn04(g_vdp_reg, coef_addr->hdr3_addr >> 32); /* 32 is number */

    vdp_para_setparaaddrv0chn07(g_vdp_reg, coef_addr->hdr6_addr);
    vdp_para_setparahaddrv0chn07(g_vdp_reg, coef_addr->hdr6_addr >> 32); /* 32 is number */
}

hi_void vdp_ip_hdr_set(hi_u32 layer, hal_hdr_info* hdr_info)
{
    vdp_vid_sethdrlinkctrl(g_vdp_reg, VDP_HDR_LINK_STB_HDR);
    vdp_vid_sethdroutmaplinkctrl(g_vdp_reg, VDP_HDR_OUTMAP_LINK_V0_STB);

    return ;
}

hi_void vdp_ip_dolby_set(hi_u32 layer, hal_hdr_info *hdr_info)
{
    hi_u32 offset = 0;

    vdp_ip_hdr_set_coef_addr(&hdr_info->coef_addr);

    vdp_hdr_set_cm_mode(g_vdp_reg, offset, &hdr_info->cm_cfg);
    vdp_hdr_set_imap_mode(g_vdp_reg, offset, &hdr_info->imap_cfg);
    vdp_hdr_set_tmap_mode(g_vdp_reg, offset, &hdr_info->tmap_cfg);
    vdp_hdr_set_omap_mode(g_vdp_reg, offset, &hdr_info->omap_cfg);
}

hi_void vdp_ip_composer_set(hi_u32 layer, hal_composer_info* comp_info)
{
    hi_u32 offset = g_hdr_offset_addr[layer];

    vdp_hdr_set_composer_mode(g_vdp_reg, offset, HI_NULL);

#ifdef VDP_CBB_TEST_SUPPORT
    vdp_composer_test(offset, comp_info);
#endif
}

hi_void vdp_ip_tecnicolor_set(hi_u32 layer, hal_tecnicolor_info* tc_info)
{
    hi_u32 offset = g_hdr_offset_addr[layer];

#ifdef VDP_CBB_TEST_SUPPORT
    tc_info.hdr0_addr = (hi_u64)g_vdp_coef_buf_addr.vir_addr[VDP_COEF_BUF_V0_HDR0];

    vdp_tecnicolor_test(offset, tc_info);

    tc_info.hdr0_addr = g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_V0_HDR0];
#endif
    vdp_para_setparaaddrv0chn01(g_vdp_reg, tc_info->hdr0_addr);
    vdp_para_setparahaddrv0chn01(g_vdp_reg, tc_info->hdr0_addr >> 32); /* 32 is number */

    vdp_hdr_set_tecnicolor_mode(g_vdp_reg, offset, HI_NULL);

    vdp_para_setparaupv0chn(g_vdp_reg, VDP_COEF_BUF_V0_HDR0);
}

