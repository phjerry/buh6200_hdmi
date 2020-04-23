/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_ip_zme.h"
#include "hal_vdp_ip_para.h"
#include "hal_vdp_comm.h"
#include "hal_vdp_reg_vid.h"
#include "hal_vdp_reg_hzme.h"
#include "hal_vdp_reg_vzme.h"
#include "hal_vdp_reg_2dscale.h"
#include "hal_vdp_reg_hipp_sr.h"
#include "hal_vdp_reg_para.h"
#include "hal_vdp_reg_hfir.h"
#include "hal_vdp_reg_cvfir.h"
#include "hal_vdp_test_zme.h"

typedef struct {
} vdp_8kzme_pq_cofe;

typedef struct {
} vdp_4ksr_pq_cofe;

typedef struct {
} vdp_8ksr_pq_cofe;

static hi_u32 g_zme8k_reg[VDP_LAYER_VID_BUTT] = {
    0,
    0,
    0,
    0x2f100 / 4, /* 4 is a number */
};
static hi_u32 g_zme8k_offset_reg[VDP_8K_ZME_SPLIT_MAX] = {
    (0x100 / 4), /* 4 is a number */
    (0x200 / 4), /* 4 is a number */
    (0x300 / 4), /* 4 is a number */
    (0x400 / 4), /* 4 is a number */
};

static hi_u32 g_zme4k_reg = 0;
static hi_u32 g_sr4k_reg[VDP_LAYER_VID_BUTT] = {
    0,
    0,
    0,
    0x1000 / 4, /* 4 is a number */
};
static hi_u32 g_sr8k_reg[VDP_LAYER_VID_BUTT] = {
    0,
    0,
    0,
    0,
};
static hi_u32 g_sr8k_split_offset_reg[VDP_8K_SR_SPLIT_MAX] = {
    (0x500 / 4), /* 4 is a number */
    (0xA00 / 4), /* 4 is a number */
};

static hi_void vdp_zme_set_pq_4kzme_lv_dirdect2(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 lv_weightnon_gain = 16;
    hi_u32 lv_weight_neiblmt_gain = 16;
    hi_u32 lv_weight_non_b = 1;
    hi_u32 lv_weight_bld_cor = 0;
    hi_u32 lv_weight_bld_k = 13;
    hi_u32 lv_weight_bld_gmax = 32;
    hi_u32 lv_weight_bld_gmin = 0;
    hi_u32 lv_denseline_det_en = 1;
    hi_u32 lv_denseline_gain = 8;
    hi_u32 lv_denseline_cor = 8;
    hi_u32 lv_denseline_cork = 4;
    hi_u32 lv_denseline_wintype = 1;
    hi_u32 lv_denseline_flattype = 0;
    hi_u32 lv_denseline_type = 3;
    hi_u32 lv_dir_weightlpf_en = 1;
    hi_u32 lv_dir_weightlpf_neibmode = 1;
    hi_u32 lv_dir_weightlpf_gain = 0;

    vdp_xdp_2dscale_setlvweightneiblmtgain(g_vdp_reg, lv_weight_neiblmt_gain);
    vdp_xdp_2dscale_setlvweightnongain(g_vdp_reg, lv_weightnon_gain);
    vdp_xdp_2dscale_setlvweightnonb(g_vdp_reg, lv_weight_non_b);

    vdp_xdp_2dscale_setlvweightbldgmin(g_vdp_reg, lv_weight_bld_gmin);
    vdp_xdp_2dscale_setlvweightbldgmax(g_vdp_reg, lv_weight_bld_gmax);
    vdp_xdp_2dscale_setlvweightbldk(g_vdp_reg, lv_weight_bld_k);
    vdp_xdp_2dscale_setlvweightbldcor(g_vdp_reg, lv_weight_bld_cor);

    vdp_xdp_2dscale_setlvdenselinetype(g_vdp_reg, lv_denseline_type);
    vdp_xdp_2dscale_setlvdenselinecor(g_vdp_reg, lv_denseline_cor);
    vdp_xdp_2dscale_setlvdenselinecork(g_vdp_reg, lv_denseline_cork);
    vdp_xdp_2dscale_setlvdenselinegain(g_vdp_reg, lv_denseline_gain);
    vdp_xdp_2dscale_setlvdenselineflattype(g_vdp_reg, lv_denseline_flattype);
    vdp_xdp_2dscale_setlvdenselinewintype(g_vdp_reg, lv_denseline_wintype);
    vdp_xdp_2dscale_setlvdenselinedeten(g_vdp_reg, lv_denseline_det_en);

    vdp_xdp_2dscale_setlvdirweightlpfgain(g_vdp_reg, lv_dir_weightlpf_gain);
    vdp_xdp_2dscale_setlvdirweightlpfneibmode(g_vdp_reg, lv_dir_weightlpf_neibmode);
    vdp_xdp_2dscale_setlvdirweightlpfen(g_vdp_reg, lv_dir_weightlpf_en);

}

static hi_void vdp_zme_set_pq_4kzme_lv_dirdect(hi_u32 offset, vdp_zme_image_info *info)
{
    /* lv dirdect */
    hi_u32 lv_sad_neibbld_gain = 4;
    hi_u32 lv_sadmono_neibgain = 8;
    hi_u32 lv_sad_avg_thr = 10;
    hi_u32 lv_sad_dif_thr = 6;
    hi_u32 lv_sadmono_add_k = 7;
    hi_u32 lv_sad_difhv_k = 20;
    hi_u32 lv_sad_amendlmt_b = 3;
    hi_u32 lv_sad_amendlmt_k = 32;
    hi_u32 lv_sad_monopunish_en = 1;
    hi_u32 lv_minsad_cor = 8;
    hi_u32 lv_minsad_gain = 0;
    hi_u32 lv_dir_weightmax_k = 0;
    hi_u32 lv_dir_tblmt_en = 1;
    hi_u32 lv_dir_hvlmt_en = 0;
    hi_u32 lv_dir_hvdiaglmt_en = 1;
    hi_u32 lv_dir_weight_mixmode = 0;
    hi_u32 lv_dir_stop_num = 0;

    vdp_xdp_2dscale_setlvsadmononeibgain(g_vdp_reg, lv_sadmono_neibgain);
    vdp_xdp_2dscale_setlvsadneibbldgain(g_vdp_reg, lv_sad_neibbld_gain);
    vdp_xdp_2dscale_setlvsadmonoaddk(g_vdp_reg, lv_sadmono_add_k);
    vdp_xdp_2dscale_setlvsaddifthr(g_vdp_reg, lv_sad_dif_thr);
    vdp_xdp_2dscale_setlvsadavgthr(g_vdp_reg, lv_sad_avg_thr);
    vdp_xdp_2dscale_setlvminsadcor(g_vdp_reg, lv_minsad_cor);
    vdp_xdp_2dscale_setlvsadamendlmtk(g_vdp_reg, lv_sad_amendlmt_k);
    vdp_xdp_2dscale_setlvsadamendlmtb(g_vdp_reg, lv_sad_amendlmt_b);
    vdp_xdp_2dscale_setlvsaddifhvk(g_vdp_reg, lv_sad_difhv_k);
    vdp_xdp_2dscale_setlvdirstopnum(g_vdp_reg, lv_dir_stop_num);
    vdp_xdp_2dscale_setlvsadmonopunishen(g_vdp_reg, lv_sad_monopunish_en);

    vdp_xdp_2dscale_setlvdirweightmixmode(g_vdp_reg, lv_dir_weight_mixmode);
    vdp_xdp_2dscale_setlvdirhvdiaglmten(g_vdp_reg, lv_dir_hvdiaglmt_en);
    vdp_xdp_2dscale_setlvdirhvlmten(g_vdp_reg, lv_dir_hvlmt_en);
    vdp_xdp_2dscale_setlvdirtblmten(g_vdp_reg, lv_dir_tblmt_en);
    vdp_xdp_2dscale_setlvdirweightmaxk(g_vdp_reg, lv_dir_weightmax_k);

    vdp_xdp_2dscale_setlvminsadgain(g_vdp_reg, lv_minsad_gain);
}

static hi_void vdp_zme_set_pq_4kzme_lv_lut(hi_u32 offset, vdp_zme_image_info *info)
{
    /* lv dering talbe */
    hi_u32 lv_dr_lut1[16] = { /* 16 is a number */
        2, 4, 6, 8, 10, 12, 14, 16, /* 2, 4, 6, 8, 10, 12, 14, 16, 18 is a number */
        18, 20, 22, 24, 26, 28, 30, 32 /* 20, 22, 24, 26, 28, 30, 32 is a number */
    };

    hi_u32 lv_dr_lut2[16] = { /* 16 is a number */
        4, 8, 12, 16, 20, 24, 28, 32, /* 4, 8, 12, 16, 20, 24, 28, 32 is a number */
        36, 40, 44, 48, 52, 56, 60, 64 /* 36, 40, 44, 48, 52, 56, 60, 64 is a number */
    };

    vdp_xdp_2dscale_setlvdrlut103(g_vdp_reg, lv_dr_lut1[3]); /* 3 is a number */
    vdp_xdp_2dscale_setlvdrlut102(g_vdp_reg, lv_dr_lut1[2]); /* 2 is a number */
    vdp_xdp_2dscale_setlvdrlut101(g_vdp_reg, lv_dr_lut1[1]);
    vdp_xdp_2dscale_setlvdrlut100(g_vdp_reg, lv_dr_lut1[0]);
    vdp_xdp_2dscale_setlvdrlut107(g_vdp_reg, lv_dr_lut1[7]); /* 7 is a number */
    vdp_xdp_2dscale_setlvdrlut106(g_vdp_reg, lv_dr_lut1[6]); /* 6 is a number */
    vdp_xdp_2dscale_setlvdrlut105(g_vdp_reg, lv_dr_lut1[5]); /* 5 is a number */
    vdp_xdp_2dscale_setlvdrlut104(g_vdp_reg, lv_dr_lut1[4]); /* 4 is a number */
    vdp_xdp_2dscale_setlvdrlut111(g_vdp_reg, lv_dr_lut1[11]); /* 11 is a number */
    vdp_xdp_2dscale_setlvdrlut110(g_vdp_reg, lv_dr_lut1[10]); /* 10 is a number */
    vdp_xdp_2dscale_setlvdrlut109(g_vdp_reg, lv_dr_lut1[9]); /* 9 is a number */
    vdp_xdp_2dscale_setlvdrlut108(g_vdp_reg, lv_dr_lut1[8]); /* 8 is a number */
    vdp_xdp_2dscale_setlvdrlut115(g_vdp_reg, lv_dr_lut1[15]); /* 15 is a number */
    vdp_xdp_2dscale_setlvdrlut114(g_vdp_reg, lv_dr_lut1[14]); /* 14 is a number */
    vdp_xdp_2dscale_setlvdrlut113(g_vdp_reg, lv_dr_lut1[13]); /* 13 is a number */
    vdp_xdp_2dscale_setlvdrlut112(g_vdp_reg, lv_dr_lut1[12]); /* 12 is a number */
    vdp_xdp_2dscale_setlvdrlut203(g_vdp_reg, lv_dr_lut2[3]); /* 3 is a number */
    vdp_xdp_2dscale_setlvdrlut202(g_vdp_reg, lv_dr_lut2[2]); /* 2 is a number */
    vdp_xdp_2dscale_setlvdrlut201(g_vdp_reg, lv_dr_lut2[1]);
    vdp_xdp_2dscale_setlvdrlut200(g_vdp_reg, lv_dr_lut2[0]);
    vdp_xdp_2dscale_setlvdrlut207(g_vdp_reg, lv_dr_lut2[7]); /* 7 is a number */
    vdp_xdp_2dscale_setlvdrlut206(g_vdp_reg, lv_dr_lut2[6]); /* 6 is a number */
    vdp_xdp_2dscale_setlvdrlut205(g_vdp_reg, lv_dr_lut2[5]); /* 5 is a number */
    vdp_xdp_2dscale_setlvdrlut204(g_vdp_reg, lv_dr_lut2[4]); /* 4 is a number */
    vdp_xdp_2dscale_setlvdrlut211(g_vdp_reg, lv_dr_lut2[11]); /* 11 is a number */
    vdp_xdp_2dscale_setlvdrlut210(g_vdp_reg, lv_dr_lut2[10]); /* 10 is a number */
    vdp_xdp_2dscale_setlvdrlut209(g_vdp_reg, lv_dr_lut2[9]); /* 9 is a number */
    vdp_xdp_2dscale_setlvdrlut208(g_vdp_reg, lv_dr_lut2[8]); /* 8 is a number */
    vdp_xdp_2dscale_setlvdrlut215(g_vdp_reg, lv_dr_lut2[15]); /* 15 is a number */
    vdp_xdp_2dscale_setlvdrlut214(g_vdp_reg, lv_dr_lut2[14]); /* 14 is a number */
    vdp_xdp_2dscale_setlvdrlut213(g_vdp_reg, lv_dr_lut2[13]); /* 13 is a number */
    vdp_xdp_2dscale_setlvdrlut212(g_vdp_reg, lv_dr_lut2[12]); /* 2 is a number 12 is a number */
}

static hi_void vdp_zme_set_pq_4kzme_lv(hi_u32 offset, vdp_zme_image_info *info)
{
    /* lv shoot2nd */
    hi_u32 lv_stc2ndus_en = 1;
    hi_u32 lv_stc2ndos_en = 1;
    hi_u32 lv_stc2nd_flatmode = 1;
    hi_u32 lv_stc2nd_coradjen = 1;
    hi_u32 lv_stc2nd_gain = 32;
    hi_u32 lv_stc2nd_cor = 32;

    /* lv dirinterp and dering */
    hi_u32 lv_dir_gain = 16;
    hi_u32 lv_reslpf_gain = 16;
    hi_u32 lv_reslpf_en = 1;
    hi_u32 lv_prelpf_thr = 255;
    hi_u32 lv_dr_gradk = 4;
    hi_u32 lv_dr_gradcor = 96;
    hi_u32 lv_dr_lpfgain = 4;


    hi_u32 lv_stc2nd_en = 1;
    hi_u32 lv_dr_en = 1;
    hi_u32 lv_dir_en = 1;

    vdp_xdp_2dscale_setlvstc2nden(g_vdp_reg, lv_stc2nd_en);
    vdp_xdp_2dscale_setlvstc2ndusen(g_vdp_reg, lv_stc2ndus_en);
    vdp_xdp_2dscale_setlvstc2ndosen(g_vdp_reg, lv_stc2ndos_en);
    vdp_xdp_2dscale_setlvstc2ndflatmode(g_vdp_reg, lv_stc2nd_flatmode);
    vdp_xdp_2dscale_setlvstc2ndcoradjen(g_vdp_reg, lv_stc2nd_coradjen);
    vdp_xdp_2dscale_setlvstc2ndgain(g_vdp_reg, lv_stc2nd_gain);
    vdp_xdp_2dscale_setlvstc2ndcor(g_vdp_reg, lv_stc2nd_cor);

    vdp_xdp_2dscale_setlvdren(g_vdp_reg, lv_dr_en);
    vdp_xdp_2dscale_setlvdiren(g_vdp_reg, lv_dir_en);

    vdp_xdp_2dscale_setlvreslpfgain(g_vdp_reg, lv_reslpf_gain);
    vdp_xdp_2dscale_setlvreslpfen(g_vdp_reg, lv_reslpf_en);
    vdp_xdp_2dscale_setlvprelpfthr(g_vdp_reg, lv_prelpf_thr);
    vdp_xdp_2dscale_setlvdrgradk(g_vdp_reg, lv_dr_gradk);
    vdp_xdp_2dscale_setlvdrgradcor(g_vdp_reg, lv_dr_gradcor);
    vdp_xdp_2dscale_setlvdrlpfgain(g_vdp_reg, lv_dr_lpfgain);

    vdp_xdp_2dscale_setlvdirgain(g_vdp_reg, lv_dir_gain);

    vdp_zme_set_pq_4kzme_lv_dirdect(offset, info);
    vdp_zme_set_pq_4kzme_lv_dirdect2(offset, info);
    vdp_zme_set_pq_4kzme_lv_lut(offset, info);
}

static hi_void vdp_zme_set_pq_4kzme_lh_dirdect2(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 lh_weightnon_gain = 16;
    hi_u32 lh_weight_neiblmt_gain = 16;
    hi_u32 lh_weight_non_b = 1;
    hi_u32 lh_weight_bld_cor = 0;
    hi_u32 lh_weight_bld_k = 13;
    hi_u32 lh_weight_bld_gmax = 32;
    hi_u32 lh_weight_bld_gmin = 0;
    hi_u32 lh_denseline_det_en = 1;
    hi_u32 lh_denseline_gain = 8;
    hi_u32 lh_denseline_cor = 8;
    hi_u32 lh_denseline_cork = 4;
    hi_u32 lh_denseline_wintype = 1;
    hi_u32 lh_denseline_flattype = 0;
    hi_u32 lh_denseline_type = 3;
    hi_u32 lh_dir_weightlpf_en = 1;
    hi_u32 lh_dir_weightlpf_neibmode = 1;
    hi_u32 lh_dir_weightlpf_gain = 0;

    vdp_xdp_2dscale_setlhweightneiblmtgain(g_vdp_reg, lh_weight_neiblmt_gain);
    vdp_xdp_2dscale_setlhweightnongain(g_vdp_reg, lh_weightnon_gain);
    vdp_xdp_2dscale_setlhweightnonb(g_vdp_reg, lh_weight_non_b);

    vdp_xdp_2dscale_setlhweightbldgmin(g_vdp_reg, lh_weight_bld_gmin);
    vdp_xdp_2dscale_setlhweightbldgmax(g_vdp_reg, lh_weight_bld_gmax);
    vdp_xdp_2dscale_setlhweightbldk(g_vdp_reg, lh_weight_bld_k);
    vdp_xdp_2dscale_setlhweightbldcor(g_vdp_reg, lh_weight_bld_cor);

    vdp_xdp_2dscale_setlhdenselinetype(g_vdp_reg, lh_denseline_type);
    vdp_xdp_2dscale_setlhdenselinecor(g_vdp_reg, lh_denseline_cor);
    vdp_xdp_2dscale_setlhdenselinecork(g_vdp_reg, lh_denseline_cork);
    vdp_xdp_2dscale_setlhdenselinegain(g_vdp_reg, lh_denseline_gain);
    vdp_xdp_2dscale_setlhdenselineflattype(g_vdp_reg, lh_denseline_flattype);
    vdp_xdp_2dscale_setlhdenselinewintype(g_vdp_reg, lh_denseline_wintype);
    vdp_xdp_2dscale_setlhdenselinedeten(g_vdp_reg, lh_denseline_det_en);

    vdp_xdp_2dscale_setlhdirweightlpfgain(g_vdp_reg, lh_dir_weightlpf_gain);
    vdp_xdp_2dscale_setlhdirweightlpfneibmode(g_vdp_reg, lh_dir_weightlpf_neibmode);
    vdp_xdp_2dscale_setlhdirweightlpfen(g_vdp_reg, lh_dir_weightlpf_en);
}

static hi_void vdp_zme_set_pq_4kzme_lh_dirdect(hi_u32 offset, vdp_zme_image_info *info)
{
    /* lh dirdect */
    hi_u32 lh_sad_neibbld_gain = 4;
    hi_u32 lh_sadmono_neibgain = 8;
    hi_u32 lh_sad_avg_thr = 10;
    hi_u32 lh_sad_dif_thr = 6;
    hi_u32 lh_sadmono_add_k = 7;
    hi_u32 lh_sad_difhv_k = 20;
    hi_u32 lh_sad_amendlmt_b = 3;
    hi_u32 lh_sad_amendlmt_k = 32;
    hi_u32 lh_sad_monopunish_en = 1;
    hi_u32 lh_minsad_cor = 8;
    hi_u32 lh_minsad_gain = 0;
    hi_u32 lh_dir_weightmax_k = 0;
    hi_u32 lh_dir_tblmt_en = 1;
    hi_u32 lh_dir_hvlmt_en = 0;
    hi_u32 lh_dir_hvdiaglmt_en = 1;
    hi_u32 lh_dir_weight_mixmode = 0;
    hi_u32 lh_dir_stop_num = 0;

    vdp_xdp_2dscale_setlhsadmononeibgain(g_vdp_reg, lh_sadmono_neibgain);
    vdp_xdp_2dscale_setlhsadneibbldgain(g_vdp_reg, lh_sad_neibbld_gain);
    vdp_xdp_2dscale_setlhsadmonoaddk(g_vdp_reg, lh_sadmono_add_k);
    vdp_xdp_2dscale_setlhsaddifthr(g_vdp_reg, lh_sad_dif_thr);
    vdp_xdp_2dscale_setlhsadavgthr(g_vdp_reg, lh_sad_avg_thr);
    vdp_xdp_2dscale_setlhminsadcor(g_vdp_reg, lh_minsad_cor);
    vdp_xdp_2dscale_setlhsadamendlmtk(g_vdp_reg, lh_sad_amendlmt_k);
    vdp_xdp_2dscale_setlhsadamendlmtb(g_vdp_reg, lh_sad_amendlmt_b);
    vdp_xdp_2dscale_setlhsaddifhvk(g_vdp_reg, lh_sad_difhv_k);
    vdp_xdp_2dscale_setlhdirstopnum(g_vdp_reg, lh_dir_stop_num);
    vdp_xdp_2dscale_setlhsadmonopunishen(g_vdp_reg, lh_sad_monopunish_en);

    vdp_xdp_2dscale_setlhdirweightmixmode(g_vdp_reg, lh_dir_weight_mixmode);
    vdp_xdp_2dscale_setlhdirhvdiaglmten(g_vdp_reg, lh_dir_hvdiaglmt_en);
    vdp_xdp_2dscale_setlhdirhvlmten(g_vdp_reg, lh_dir_hvlmt_en);
    vdp_xdp_2dscale_setlhdirtblmten(g_vdp_reg, lh_dir_tblmt_en);
    vdp_xdp_2dscale_setlhdirweightmaxk(g_vdp_reg, lh_dir_weightmax_k);

    vdp_xdp_2dscale_setlhminsadgain(g_vdp_reg, lh_minsad_gain);
}

static hi_void vdp_zme_set_pq_4kzme_lh_lut(hi_u32 offset, vdp_zme_image_info *info)
{
    /* lh dering table */
    hi_u32 lh_dr_lut1[16] = { /* 16 is a number */
        2, 4, 6, 8, 10, 12, 14, 16, /* 2, 4, 6, 8, 10, 12, 14 is a number 16 */
        18, 20, 22, 24, 26, 28, 30, 32 /* 18, 20, 22, 24, 26, 28, 30, 32 is a number */
    };
    hi_u32 lh_dr_lut2[16] = { /* 16 is a number */
        4, 8, 12, 16, 20, 24, 28, 32, /* 4, 8, 12, 16, 20, 24, 28, 32 is a number */
        36, 40, 44, 48, 52, 56, 60, 64 /* 36, 40, 44, 48, 52, 56, 60, 64 is a number */
    };

    vdp_xdp_2dscale_setlhdrlut103(g_vdp_reg, lh_dr_lut1[3]); /* 3 is a number */
    vdp_xdp_2dscale_setlhdrlut102(g_vdp_reg, lh_dr_lut1[2]); /* 2 is a number */
    vdp_xdp_2dscale_setlhdrlut101(g_vdp_reg, lh_dr_lut1[1]);
    vdp_xdp_2dscale_setlhdrlut100(g_vdp_reg, lh_dr_lut1[0]);
    vdp_xdp_2dscale_setlhdrlut107(g_vdp_reg, lh_dr_lut1[7]); /* 7 is a number */
    vdp_xdp_2dscale_setlhdrlut106(g_vdp_reg, lh_dr_lut1[6]); /* 6 is a number */
    vdp_xdp_2dscale_setlhdrlut105(g_vdp_reg, lh_dr_lut1[5]); /* 5 is a number */
    vdp_xdp_2dscale_setlhdrlut104(g_vdp_reg, lh_dr_lut1[4]); /* 4 is a number */
    vdp_xdp_2dscale_setlhdrlut111(g_vdp_reg, lh_dr_lut1[11]); /* 11 is a number */
    vdp_xdp_2dscale_setlhdrlut110(g_vdp_reg, lh_dr_lut1[10]); /* 10 is a number */
    vdp_xdp_2dscale_setlhdrlut109(g_vdp_reg, lh_dr_lut1[9]); /* 9 is a number */
    vdp_xdp_2dscale_setlhdrlut108(g_vdp_reg, lh_dr_lut1[8]); /* 8 is a number */
    vdp_xdp_2dscale_setlhdrlut115(g_vdp_reg, lh_dr_lut1[15]); /* 15 is a number */
    vdp_xdp_2dscale_setlhdrlut114(g_vdp_reg, lh_dr_lut1[14]); /* 14 is a number */
    vdp_xdp_2dscale_setlhdrlut113(g_vdp_reg, lh_dr_lut1[13]); /* 13 is a number */
    vdp_xdp_2dscale_setlhdrlut112(g_vdp_reg, lh_dr_lut1[12]); /* 12 is a number */
    vdp_xdp_2dscale_setlhdrlut203(g_vdp_reg, lh_dr_lut2[3]); /* 3 is a number */
    vdp_xdp_2dscale_setlhdrlut202(g_vdp_reg, lh_dr_lut2[2]); /* 2 is a number */
    vdp_xdp_2dscale_setlhdrlut201(g_vdp_reg, lh_dr_lut2[1]);
    vdp_xdp_2dscale_setlhdrlut200(g_vdp_reg, lh_dr_lut2[0]);
    vdp_xdp_2dscale_setlhdrlut207(g_vdp_reg, lh_dr_lut2[7]); /* 7 is a number */
    vdp_xdp_2dscale_setlhdrlut206(g_vdp_reg, lh_dr_lut2[6]); /* 6 is a number */
    vdp_xdp_2dscale_setlhdrlut205(g_vdp_reg, lh_dr_lut2[5]); /* 5 is a number */
    vdp_xdp_2dscale_setlhdrlut204(g_vdp_reg, lh_dr_lut2[4]); /* 4 is a number */
    vdp_xdp_2dscale_setlhdrlut211(g_vdp_reg, lh_dr_lut2[11]); /* 11 is a number */
    vdp_xdp_2dscale_setlhdrlut210(g_vdp_reg, lh_dr_lut2[10]); /* 10 is a number */
    vdp_xdp_2dscale_setlhdrlut209(g_vdp_reg, lh_dr_lut2[9]); /* 9 is a number */
    vdp_xdp_2dscale_setlhdrlut208(g_vdp_reg, lh_dr_lut2[8]); /* 8 is a number */
    vdp_xdp_2dscale_setlhdrlut215(g_vdp_reg, lh_dr_lut2[15]); /* 15 is a number */
    vdp_xdp_2dscale_setlhdrlut214(g_vdp_reg, lh_dr_lut2[14]); /* 14 is a number */
    vdp_xdp_2dscale_setlhdrlut213(g_vdp_reg, lh_dr_lut2[13]); /* 13 is a number */
    vdp_xdp_2dscale_setlhdrlut212(g_vdp_reg, lh_dr_lut2[12]); /* 2 is a number 12 is a number */
}

static hi_void vdp_zme_set_pq_4kzme_lh(hi_u32 offset, vdp_zme_image_info *info)
{
    /* lh shoot2nd */
    hi_u32 lh_stc2ndus_en = 1;
    hi_u32 lh_stc2ndos_en = 1;
    hi_u32 lh_stc2nd_flatmode = 1;
    hi_u32 lh_stc2nd_coradjen = 1;
    hi_u32 lh_stc2nd_gain = 32;
    hi_u32 lh_stc2nd_cor = 32;

    /* lh dirinterp and dering */
    hi_u32 lh_dir_gain = 16;
    hi_u32 lh_reslpf_gain = 16;
    hi_u32 lh_reslpf_en = 1;
    hi_u32 lh_prelpf_thr = 255;
    hi_u32 lh_dr_gradlpfgain = 4;
    hi_u32 lh_dr_gradk = 4;
    hi_u32 lh_dr_gradcor = 96;
    hi_u32 lh_dr_lpfgain = 4;

    hi_u32 lh_stc2nd_en = 1;
    hi_u32 lh_dr_en = 1;
    hi_u32 lh_dir_en = 1;
    hi_u32 ch_med_en = 1;

    vdp_xdp_2dscale_setlhstc2nden(g_vdp_reg, lh_stc2nd_en);

    vdp_xdp_2dscale_setlhstc2ndusen(g_vdp_reg, lh_stc2ndus_en);
    vdp_xdp_2dscale_setlhstc2ndosen(g_vdp_reg, lh_stc2ndos_en);
    vdp_xdp_2dscale_setlhstc2ndflatmode(g_vdp_reg, lh_stc2nd_flatmode);
    vdp_xdp_2dscale_setlhstc2ndcoradjen(g_vdp_reg, lh_stc2nd_coradjen);
    vdp_xdp_2dscale_setlhstc2ndgain(g_vdp_reg, lh_stc2nd_gain);
    vdp_xdp_2dscale_setlhstc2ndcor(g_vdp_reg, lh_stc2nd_cor);

    vdp_xdp_2dscale_setlhdren(g_vdp_reg, lh_dr_en);
    vdp_xdp_2dscale_setlhdiren(g_vdp_reg, lh_dir_en);

    vdp_xdp_2dscale_setlhreslpfgain(g_vdp_reg, lh_reslpf_gain);
    vdp_xdp_2dscale_setlhreslpfen(g_vdp_reg, lh_reslpf_en);
    vdp_xdp_2dscale_setlhprelpfthr(g_vdp_reg, lh_prelpf_thr);
    vdp_xdp_2dscale_setlhdrgradlpfgain(g_vdp_reg, lh_dr_gradlpfgain);
    vdp_xdp_2dscale_setlhdrgradk(g_vdp_reg, lh_dr_gradk);
    vdp_xdp_2dscale_setlhdrgradcor(g_vdp_reg, lh_dr_gradcor);
    vdp_xdp_2dscale_setlhdrlpfgain(g_vdp_reg, lh_dr_lpfgain);
    vdp_xdp_2dscale_setchmeden(g_vdp_reg, ch_med_en);
    vdp_xdp_2dscale_setlhdirgain(g_vdp_reg, lh_dir_gain);

    vdp_zme_set_pq_4kzme_lh_dirdect(offset, info);
    vdp_zme_set_pq_4kzme_lh_dirdect2(offset, info);
    vdp_zme_set_pq_4kzme_lh_lut(offset, info);
}

static hi_void vdp_zme_set_pq_4kzme_graph(hi_u32 offset, vdp_zme_image_info *info)
{
    /* graphdet */
    hi_u32 difth0_graph = 8;
    hi_u32 difth1_graph = 24;
    hi_u32 difth2_graph = 64;
    hi_u32 difth3_graph = 128;
    hi_u32 corek1_graph = 32;
    hi_u32 corek2_graph = 16;
    hi_u32 corek3_graph = 6;
    hi_u32 x0_kk_graph = 16;
    hi_u32 x1_kk_graph = 64;
    hi_u32 x2_kk_graph = 192;
    hi_u32 g0_kk_graph = 312;
    hi_u32 g1_kk_graph = 256;
    hi_u32 g2_kk_graph = 128;
    hi_u32 g3_kk_graph = 64;
    hi_u32 k1_kk_graph = 43;
    hi_u32 k2_kk_graph = 16;
    hi_u32 k3_kk_graph = 24;
    hi_u32 k1_graph = 24;
    hi_u32 k2_graph = 80;
    hi_u32 k3_graph = 128;

    hi_u32 x0_gs_graph = 64;
    hi_u32 x1_gs_graph = 128;
    hi_u32 shift_gs_graph = 6;

    vdp_xdp_2dscale_setdifth3graph(g_vdp_reg, difth3_graph);
    vdp_xdp_2dscale_setdifth2graph(g_vdp_reg, difth2_graph);
    vdp_xdp_2dscale_setdifth1graph(g_vdp_reg, difth1_graph);
    vdp_xdp_2dscale_setdifth0graph(g_vdp_reg, difth0_graph);
    vdp_xdp_2dscale_setcorek3graph(g_vdp_reg, corek3_graph);
    vdp_xdp_2dscale_setcorek2graph(g_vdp_reg, corek2_graph);
    vdp_xdp_2dscale_setcorek1graph(g_vdp_reg, corek1_graph);
    vdp_xdp_2dscale_setx1kkgraph(g_vdp_reg, x1_kk_graph);
    vdp_xdp_2dscale_setx0kkgraph(g_vdp_reg, x0_kk_graph);
    vdp_xdp_2dscale_setg0kkgraph(g_vdp_reg, g0_kk_graph);
    vdp_xdp_2dscale_setx2kkgraph(g_vdp_reg, x2_kk_graph);
    vdp_xdp_2dscale_setg2kkgraph(g_vdp_reg, g2_kk_graph);
    vdp_xdp_2dscale_setg1kkgraph(g_vdp_reg, g1_kk_graph);
    vdp_xdp_2dscale_setk1kkgraph(g_vdp_reg, k1_kk_graph);
    vdp_xdp_2dscale_setg3kkgraph(g_vdp_reg, g3_kk_graph);
    vdp_xdp_2dscale_setk3kkgraph(g_vdp_reg, k3_kk_graph);
    vdp_xdp_2dscale_setk2kkgraph(g_vdp_reg, k2_kk_graph);
    vdp_xdp_2dscale_setk3graph(g_vdp_reg, k3_graph);
    vdp_xdp_2dscale_setk2graph(g_vdp_reg, k2_graph);
    vdp_xdp_2dscale_setk1graph(g_vdp_reg, k1_graph);
    vdp_xdp_2dscale_setshiftgsgraph(g_vdp_reg, shift_gs_graph);
    vdp_xdp_2dscale_setx1gsgraph(g_vdp_reg, x1_gs_graph);
    vdp_xdp_2dscale_setx0gsgraph(g_vdp_reg, x0_gs_graph);
}

static hi_void vdp_zme_set_pq_4kzme_demo(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 demo_en = 0;
    hi_u32 demo_mode = 0;
    hi_u32 demo_in_col = 0;
    hi_u32 demo_out_col = 0;

    vdp_xdp_2dscale_setdemoen(g_vdp_reg, demo_en);
    vdp_xdp_2dscale_setdemomode(g_vdp_reg, demo_mode);
    vdp_xdp_2dscale_setdemoincol(g_vdp_reg, demo_in_col);
    vdp_xdp_2dscale_setdemooutcol(g_vdp_reg, demo_out_col);
}

static hi_void vdp_zme_set_pq_4kzme(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 clk_2dscale_fix_en = 0;
    hi_u32 clk_2dscale_div2_en = 0;
    hi_u32 nonlinear_scl_en = 0;

    hi_u32 graph_det_en = 0;
    hi_u32 graphdet_w = (info->in_width <= 2048) ? info->in_width : 2048; /* 2048, 2048 is a number */
    hi_u32 graphdet_h = info->in_mode ?
        (info->in_height <= 1080 ? info->in_height - 1 : 1080 - 1) /* 1080 is a number */
        : (info->in_height / 2 <= 540) ? info->in_height / 2 - 1 : 540 - 1; /* 2 ,540, 2, 540 is a number */
    hi_u32 col_end = graphdet_w - 1;
    hi_u32 col_start = graphdet_w / 2; /* 2 is a number */
    hi_u32 row_end = graphdet_h - 1;
    hi_u32 row_start = graphdet_h / 2; /* 2 is a number */
    hi_u32 div_height = (((1 << 20) + (row_end + 1 - row_start) / 2) /* 20, 2 is a number */
         / (row_end + 1 - row_start) > 65535) ?  /* 65535 is a number */
        65535 : ((1 << 20) + (row_end + 1 - row_start) / 2) / (row_end + 1 - row_start); /* 65535, 20, 2 is a number */
    hi_u32 div_width = (((1 << 20) + (col_end + 1 - col_start) / 2) /* 20, 2 is a number */
        / (col_end + 1 - col_start) > 65535) ? /* 65535 is a number */
        65535 : ((1 << 20) + (col_end + 1 - col_start) / 2) / (col_end + 1 - col_start); /* 65535, 20, 2 is a number */

    /* zone */
    hi_s32 zone0_delta = 0;
    hi_s32 zone2_delta = 0;
    hi_u32 zone1_end = 0;
    hi_u32 zone0_end = 0;

    hi_u32 gscore_mode = 0;
    hi_u32 gscore_cfg = 0;

    vdp_xdp_2dscale_setclk2dscalefixen(g_vdp_reg, clk_2dscale_fix_en);
    vdp_xdp_2dscale_setclk2dscalediv2en(g_vdp_reg, clk_2dscale_div2_en);

    vdp_xdp_2dscale_setnonlinearsclen(g_vdp_reg, nonlinear_scl_en);

    vdp_xdp_2dscale_setzone0delta(g_vdp_reg, zone0_delta);
    vdp_xdp_2dscale_setzone2delta(g_vdp_reg, zone2_delta);
    vdp_xdp_2dscale_setzone1end(g_vdp_reg, zone1_end);
    vdp_xdp_2dscale_setzone0end(g_vdp_reg, zone0_end);

    vdp_xdp_2dscale_setcolend(g_vdp_reg, col_end);
    vdp_xdp_2dscale_setcolstart(g_vdp_reg, col_start);
    vdp_xdp_2dscale_setgraphdeten(g_vdp_reg, graph_det_en);
    vdp_xdp_2dscale_setrowend(g_vdp_reg, row_end);
    vdp_xdp_2dscale_setrowstart(g_vdp_reg, row_start);
    vdp_xdp_2dscale_setdivheight(g_vdp_reg, div_height);
    vdp_xdp_2dscale_setdivwidth(g_vdp_reg, div_width);
    vdp_xdp_2dscale_setgscoremode(g_vdp_reg, gscore_mode);
    vdp_xdp_2dscale_setgscorecfg(g_vdp_reg, gscore_cfg);

    vdp_zme_set_pq_4kzme_lh(offset, info);
    vdp_zme_set_pq_4kzme_lv(offset, info);
    vdp_zme_set_pq_4kzme_graph(offset, info);
    vdp_zme_set_pq_4kzme_demo(offset, info);

    return;
}

static hi_void vdp_zme_set_coef_4kzme(hi_u32 offset, vdp_zme_image_info *info)
{

}

static hi_void vdp_zme_set_4kzme(hi_u32 layer, vdp_zme_image_info *info)
{
    hi_u32 offset = g_zme4k_reg;
    hi_u32 tap_reduce_en = 0;

    hi_u32 zme_hprec = 1 << 20; /* 20 is a number */
    hi_u32 zme_vprec = 1 << 12; /* 12 is a number */
    hi_u32 hratio = (info->in_width * (zme_hprec / 4)) / info->out_width * 4; /* 4 is a number 4 is a number */
    hi_u32 vratio = (info->in_height * zme_vprec) / info->out_height;

    vdp_xdp_2dscale_setlvsclen(g_vdp_reg, info->v_enable);
    vdp_xdp_2dscale_setcvsclen(g_vdp_reg, info->v_enable);
    vdp_xdp_2dscale_setlhsclen(g_vdp_reg, info->h_enable);
    vdp_xdp_2dscale_setchsclen(g_vdp_reg, info->h_enable);

    if (info->v_enable == HI_FALSE && info->h_enable == HI_FALSE) {
        return;
    }

    vdp_xdp_2dscale_setscl2den(g_vdp_reg, info->link);

    vdp_xdp_2dscale_setoutwidth(g_vdp_reg, info->out_width - 1);
    vdp_xdp_2dscale_setoutheight(g_vdp_reg, info->out_height - 1);
    vdp_xdp_2dscale_setoutfmt(g_vdp_reg, info->out_pix_fmt);
    vdp_xdp_2dscale_setoutpro(g_vdp_reg, info->out_mode);
    vdp_xdp_2dscale_setckgten(g_vdp_reg, HI_TRUE);
    vdp_xdp_2dscale_settapreduceen(g_vdp_reg, tap_reduce_en);

    /* v */
    vdp_xdp_2dscale_setlvflten(g_vdp_reg, info->v_lfir_en);
    vdp_xdp_2dscale_setcvflten(g_vdp_reg, info->v_cfir_en);
    vdp_xdp_2dscale_setvlumaoffset(g_vdp_reg, info->offset.v_loffset);
    vdp_xdp_2dscale_setvchromaoffset(g_vdp_reg, info->offset.v_coffset);
    vdp_xdp_2dscale_setvblumaoffset(g_vdp_reg, info->offset.v_bloffset);
    vdp_xdp_2dscale_setvbchromaoffset(g_vdp_reg, info->offset.v_bcoffset);
    vdp_xdp_2dscale_setcvmeden(g_vdp_reg, info->v_med_en);
    vdp_xdp_2dscale_setvratio(g_vdp_reg, vratio);

    /* h */
    vdp_xdp_2dscale_setlhflten(g_vdp_reg, info->h_lfir_en);
    vdp_xdp_2dscale_setchflten(g_vdp_reg, info->h_cfir_en);
    vdp_xdp_2dscale_setlhfiroffset(g_vdp_reg, info->offset.h_loffset);
    vdp_xdp_2dscale_setchfiroffset(g_vdp_reg, info->offset.h_coffset);
    vdp_xdp_2dscale_setchmeden(g_vdp_reg, info->h_med_en);
    vdp_xdp_2dscale_sethratio(g_vdp_reg, hratio);

    vdp_zme_set_pq_4kzme(offset, info);

    vdp_zme_set_coef_4kzme(offset, info);

#ifdef VDP_CBB_TEST_SUPPORT
    vdp_zme_test_4kzme(offset, info);
#endif

    return;
}

static hi_void vdp_zme_splite_alg_8kzme(vdp_zme_image_info *info, vdp_8k_zme_split_out *split_out, hi_u64 hratio)
{
    hi_u32 i = 0;

    hi_u32 in_w[VDP_8K_ZME_SPLIT_MAX] = { 0 };
    hi_u32 out_w[VDP_8K_ZME_SPLIT_MAX] = { 0 };
    hi_s64 h_loffset[VDP_8K_ZME_SPLIT_MAX] = { 0 };
    hi_s64 h_coffset[VDP_8K_ZME_SPLIT_MAX] = { 0 };

    hi_u32 iw = info->in_width;
    hi_u32 ow = info->out_width;
    hi_s32 hor_loffset = info->offset.h_loffset;
    hi_s32 hor_coffset = info->offset.h_coffset;

    hi_u64 overlap = 16;

    hi_u64 ow_per = ow / 4; /* 4 is a number */
    hi_u64 ow_align = ow_per / 8 * 8; /* 8, 8 is a number */

    hi_u64 xst_pos_cord_in;
    hi_u32 xst_pos_cord_in_overlap, xst_pos_cord_int_align2[4]; /* 4 is a number */
    hi_u64 hcratio = (info->in_pix_fmt == 2) ? hratio : (hratio / 2); /* 2 is a number 2 is a number */;

    for (i = 0; i < VDP_8K_ZME_SPLIT_MAX; i++) {
        out_w[i] = (((i == 3) ? (ow - 1) : ((i + 1) * ow_align - 1)) - (i * ow_align) + 1); /* 3 is a number */
        xst_pos_cord_in = hor_loffset + ((hi_u64)(i * ow_align) * hratio);
        xst_pos_cord_int_align2[i] = (xst_pos_cord_in >> 20) / 2 * 2; /* 2, 20 is a number */
        xst_pos_cord_in_overlap = (i == 0) ? 0 : (xst_pos_cord_int_align2[i] - ((i == 0) ? 0 : overlap));
        h_loffset[i] = xst_pos_cord_in - (xst_pos_cord_in_overlap << 20); /* 20 is a number */

        h_coffset[i] = (hor_coffset + ((hi_u64)(i * ow_align) * hcratio)) -
            (((info->in_pix_fmt == 2) ? xst_pos_cord_in_overlap : /* 2 is a number */
            (xst_pos_cord_in_overlap / 2)) << 20); /* 2, 20 is a number */
    }

    for (i = 0; i < VDP_8K_ZME_SPLIT_MAX; i++) {
        if (i == 3) { /* 3 is a number */
            in_w[i] = iw - xst_pos_cord_int_align2[3] + overlap; /* 3 is a number */
        } else if (i != 0) {
            in_w[i] = xst_pos_cord_int_align2[i + 1] - xst_pos_cord_int_align2[i] + overlap * 2; /* 2 is a number */
        } else {
            in_w[i] = xst_pos_cord_int_align2[i + 1] - 0 + overlap;
        }
    }

    for (i = 0; i < VDP_8K_ZME_SPLIT_MAX; i++) {
        split_out->in_w[i] = in_w[i] - 1;
        split_out->out_w[i] = out_w[i] - 1;
        split_out->h_loffset[i] = h_loffset[i];
        split_out->h_coffset[i] = h_coffset[i];
    }

    return;
}

static hi_void vdp_zme_set_pq_8kzme_h(hi_u32 offset, vdp_zme_image_info *info)
{
    /* H */
    hi_u32 zone0_delta = 0;
    hi_u32 zone2_delta = 0;
    hi_u32 zone1_end = 0;
    hi_u32 zone0_end = 0;
    hi_u32 hl_shootctrl_en = 0;
    hi_u32 hl_shootctrl_mode = 0;
    hi_u32 hl_flatdect_mode = 0;
    hi_u32 hl_coringadj_en = 0;
    hi_u32 hl_gain = 0;
    hi_u32 hl_coring = 0;
    hi_u32 hc_shootctrl_en = 0;
    hi_u32 hc_shootctrl_mode = 0;
    hi_u32 hc_flatdect_mode = 0;
    hi_u32 hc_coringadj_en = 0;
    hi_u32 hc_gain = 0;
    hi_u32 hc_coring = 0;

    vdp_hzme_setzone0delta(g_vdp_reg, offset, zone0_delta);
    vdp_hzme_setzone2delta(g_vdp_reg, offset, zone2_delta);
    vdp_hzme_setzone1end(g_vdp_reg, offset, zone1_end);
    vdp_hzme_setzone0end(g_vdp_reg, offset, zone0_end);

    vdp_hzme_sethlshootctrlen(g_vdp_reg, offset, hl_shootctrl_en);
    vdp_hzme_sethlshootctrlmode(g_vdp_reg, offset, hl_shootctrl_mode);
    vdp_hzme_sethlflatdectmode(g_vdp_reg, offset, hl_flatdect_mode);
    vdp_hzme_sethlcoringadjen(g_vdp_reg, offset, hl_coringadj_en);
    vdp_hzme_sethlgain(g_vdp_reg, offset, hl_gain);
    vdp_hzme_sethlcoring(g_vdp_reg, offset, hl_coring);
    vdp_hzme_sethcshootctrlen(g_vdp_reg, offset, hc_shootctrl_en);
    vdp_hzme_sethcshootctrlmode(g_vdp_reg, offset, hc_shootctrl_mode);
    vdp_hzme_sethcflatdectmode(g_vdp_reg, offset, hc_flatdect_mode);
    vdp_hzme_sethccoringadjen(g_vdp_reg, offset, hc_coringadj_en);
    vdp_hzme_sethcgain(g_vdp_reg, offset, hc_gain);
    vdp_hzme_sethccoring(g_vdp_reg, offset, hc_coring);
}

static hi_void vdp_zme_set_pq_8kzme_v(hi_u32 offset, vdp_zme_image_info *info)
{
    /* V */
    hi_u32 vl_shootctrl_en = 0;
    hi_u32 vl_shootctrl_mode = 0;
    hi_u32 vl_flatdect_mode = 0;
    hi_u32 vl_coringadj_en = 0;
    hi_u32 vl_gain = 0;
    hi_u32 vl_coring = 0;
    hi_u32 vc_shootctrl_en = 0;
    hi_u32 vc_shootctrl_mode = 0;
    hi_u32 vc_flatdect_mode = 0;
    hi_u32 vc_coringadj_en = 0;
    hi_u32 vc_gain = 0;
    hi_u32 vc_coring = 0;

    vdp_vzme_setvlshootctrlen(g_vdp_reg, offset, vl_shootctrl_en);
    vdp_vzme_setvlshootctrlmode(g_vdp_reg, offset, vl_shootctrl_mode);
    vdp_vzme_setvlflatdectmode(g_vdp_reg, offset, vl_flatdect_mode);
    vdp_vzme_setvlcoringadjen(g_vdp_reg, offset, vl_coringadj_en);
    vdp_vzme_setvlgain(g_vdp_reg, offset, vl_gain);
    vdp_vzme_setvlcoring(g_vdp_reg, offset, vl_coring);
    vdp_vzme_setvcshootctrlen(g_vdp_reg, offset, vc_shootctrl_en);
    vdp_vzme_setvcshootctrlmode(g_vdp_reg, offset, vc_shootctrl_mode);
    vdp_vzme_setvcflatdectmode(g_vdp_reg, offset, vc_flatdect_mode);
    vdp_vzme_setvccoringadjen(g_vdp_reg, offset, vc_coringadj_en);
    vdp_vzme_setvcgain(g_vdp_reg, offset, vc_gain);
    vdp_vzme_setvccoring(g_vdp_reg, offset, vc_coring);
}

static hi_void vdp_zme_set_pq_8kzme(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 non_lnr_en = 0;

    vdp_hzme_setnonlnren(g_vdp_reg, offset, non_lnr_en);

    vdp_zme_set_pq_8kzme_h(offset, info);
    vdp_zme_set_pq_8kzme_v(offset, info);

    return;
}

static hi_void vdp_zme_set_coef_8kzme(hi_u32 offset, vdp_zme_image_info *info)
{

}

static hi_void vdp_zme_set_8kzme_hzme(hi_u32 layer, vdp_zme_image_info *info)
{
    hi_u32 offset = g_zme8k_reg[layer];
    hi_u32 h_out_pix_fmt = 2;
    hi_u32 zme_hprec = 1 << 20; /* 20 is a number */
    hi_u32 hratio = (info->in_width * (zme_hprec / 4)) / info->out_width * 4; /* 4 is a number 4 is a number */

    /* hzme */
    vdp_hzme_setckgten(g_vdp_reg, offset, HI_TRUE);
    vdp_hzme_sethfirorder(g_vdp_reg, offset, HI_TRUE);
    vdp_hzme_sethratio(g_vdp_reg, offset, hratio);

    vdp_hzme_setoutfmt(g_vdp_reg, offset, h_out_pix_fmt);
    vdp_hzme_setinwidth(g_vdp_reg, offset, info->in_width - 1);
    vdp_hzme_setoutwidth(g_vdp_reg, offset, info->out_width - 1);

    vdp_hzme_setlhfirmode(g_vdp_reg, offset, info->h_lfir_en);
    vdp_hzme_setchfirmode(g_vdp_reg, offset, info->h_cfir_en);
    vdp_hzme_setlhmiden(g_vdp_reg, offset, info->h_med_en);
    vdp_hzme_setchmiden(g_vdp_reg, offset, info->h_med_en);

    vdp_hzme_setlhfiroffset(g_vdp_reg, offset, info->offset.h_loffset);
    vdp_hzme_setchfiroffset(g_vdp_reg, offset, info->offset.h_coffset);
}

static hi_void vdp_zme_set_8kzme_vzme(hi_u32 layer, vdp_zme_image_info *info)
{
    hi_u32 i;
    hi_u32 offset = g_zme8k_reg[layer];
    hi_u32 v_out_pix_fmt = (info->in_pix_fmt <= 1) ? 1 : 2; /* 2 is a number */
    vdp_8k_zme_split_out split_out = { 0 };

    hi_u32 zme_vprec = 1 << 12; /* 12 is a number */
    hi_u32 zme_hprec = 1 << 20; /* 20 is a number */
    hi_u32 hratio = (info->in_width * (zme_hprec / 4)) / info->out_width * 4; /* 4 is a number 4 is a number */
    hi_u32 vratio = (info->in_height * zme_vprec) / info->out_height;

    if (layer == VDP_LAYER_VID0) {
        offset = g_zme8k_reg[layer] + g_zme8k_offset_reg[3]; /* 3 is a number */
    }

    /* vzme */
    vdp_vzme_setckgten(g_vdp_reg, offset, 0);
    vdp_vzme_setvratio(g_vdp_reg, offset, vratio);

    vdp_vzme_setoutfmt(g_vdp_reg, offset, v_out_pix_fmt);
    vdp_vzme_setoutheight(g_vdp_reg, offset, info->out_height - 1);

    vdp_vzme_setlvfirmode(g_vdp_reg, offset, info->v_lfir_en);
    vdp_vzme_setcvfirmode(g_vdp_reg, offset, info->v_cfir_en);
    vdp_vzme_setlvmiden(g_vdp_reg, offset, info->v_med_en);
    vdp_vzme_setcvmiden(g_vdp_reg, offset, info->v_med_en);

    vdp_vzme_setvlumaoffset(g_vdp_reg, offset, info->offset.v_loffset);
    vdp_vzme_setvchromaoffset(g_vdp_reg, offset, info->offset.v_coffset);
    vdp_vzme_setvblumaoffset(g_vdp_reg, offset, info->offset.v_bloffset);
    vdp_vzme_setvbchromaoffset(g_vdp_reg, offset, info->offset.v_bcoffset);

    vdp_zme_splite_alg_8kzme(info, &split_out, hratio);

    if (layer == VDP_LAYER_VID0) {
        /* splite */
        for (i = 0; i < VDP_8K_ZME_SPLIT_MAX; i++) {
            offset = g_zme8k_reg[layer] + g_zme8k_offset_reg[i];
            vdp_hzme_setoutfmt(g_vdp_reg, offset, info->out_pix_fmt);
            vdp_hzme_setckgten(g_vdp_reg, offset, HI_TRUE);
            vdp_hzme_setinwidth(g_vdp_reg, offset, split_out.in_w[i]);
            vdp_hzme_setoutwidth(g_vdp_reg, offset, split_out.out_w[i]);
            vdp_hzme_setlhfiroffset(g_vdp_reg, offset, split_out.h_loffset[i]);
            vdp_hzme_setchfiroffset(g_vdp_reg, offset, split_out.h_coffset[i]);
        }
    }
}

static hi_void vdp_zme_set_8kzme(hi_u32 layer, vdp_zme_image_info *info)
{
    hi_u32 offset = g_zme8k_reg[layer];
    if (layer == VDP_LAYER_VID0) {
        vdp_hzme_setlhfiren(g_vdp_reg, g_zme8k_reg[layer], info->h_enable);
        vdp_vzme_setlvfiren(g_vdp_reg, g_zme8k_reg[layer] +
            g_zme8k_offset_reg[VDP_8K_ZME_SPLIT_MAX - 1], info->v_enable);
        vdp_hzme_setchfiren(g_vdp_reg, g_zme8k_reg[layer], info->h_enable);
        vdp_vzme_setcvfiren(g_vdp_reg, g_zme8k_reg[layer] +
            g_zme8k_offset_reg[VDP_8K_ZME_SPLIT_MAX - 1], info->v_enable);

        vdp_vid_set8kzmelinkctrl(g_vdp_reg, info->link);
    } else if (layer == VDP_LAYER_VID1) {
         /* v1 8kzme only have 4k line buffer, need close brightness zme,
            otherwise will zme err in 8k format */
        vdp_hzme_setlhfiren(g_vdp_reg, g_zme8k_reg[layer], HI_FALSE);
        vdp_vzme_setlvfiren(g_vdp_reg, g_zme8k_reg[layer], HI_FALSE);
        vdp_hzme_setchfiren(g_vdp_reg, g_zme8k_reg[layer], info->h_enable);
        vdp_vzme_setcvfiren(g_vdp_reg, g_zme8k_reg[layer], info->v_enable);
    } else {
        vdp_hzme_setlhfiren(g_vdp_reg, g_zme8k_reg[layer], info->h_enable);
        vdp_vzme_setlvfiren(g_vdp_reg, g_zme8k_reg[layer], info->v_enable);
        vdp_hzme_setchfiren(g_vdp_reg, g_zme8k_reg[layer], info->h_enable);
        vdp_vzme_setcvfiren(g_vdp_reg, g_zme8k_reg[layer], info->v_enable);
    }

    if (info->v_enable == HI_FALSE && info->h_enable == HI_FALSE) {
        return ;
    }

    vdp_zme_set_8kzme_hzme(layer, info);
    vdp_zme_set_8kzme_vzme(layer, info);

    vdp_zme_set_pq_8kzme(offset, info);
    vdp_zme_set_coef_8kzme(offset, info);

    if (layer == VDP_LAYER_VID0) {
        vdp_zme_test_8kzme(layer, info);
    }

    return;
}

static hi_void vdp_zme_set_pq_4ksr_dbsr_bicubic(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 dbsr_bicubic_phase03 = -4;
    hi_u32 dbsr_bicubic_phase02 = 54;
    hi_u32 dbsr_bicubic_phase01 = 16;
    hi_u32 dbsr_bicubic_phase00 = -2;
    hi_u32 dbsr_bicubic_phase13 = -1;
    hi_u32 dbsr_bicubic_phase12 = 13;
    hi_u32 dbsr_bicubic_phase11 = 57;
    hi_u32 dbsr_bicubic_phase10 = -5;

    vdp_hipp_sr_setdbsrbicubicphase03(g_vdp_reg, offset, dbsr_bicubic_phase03);
    vdp_hipp_sr_setdbsrbicubicphase02(g_vdp_reg, offset, dbsr_bicubic_phase02);
    vdp_hipp_sr_setdbsrbicubicphase01(g_vdp_reg, offset, dbsr_bicubic_phase01);
    vdp_hipp_sr_setdbsrbicubicphase00(g_vdp_reg, offset, dbsr_bicubic_phase00);
    vdp_hipp_sr_setdbsrbicubicphase13(g_vdp_reg, offset, dbsr_bicubic_phase13);
    vdp_hipp_sr_setdbsrbicubicphase12(g_vdp_reg, offset, dbsr_bicubic_phase12);
    vdp_hipp_sr_setdbsrbicubicphase11(g_vdp_reg, offset, dbsr_bicubic_phase11);
    vdp_hipp_sr_setdbsrbicubicphase10(g_vdp_reg, offset, dbsr_bicubic_phase10);
}

static hi_void vdp_zme_set_pq_4ksr_dbsr_graph(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 difth_graph[4] = { 8, 24, 64, 128 }; /* 4, 8, 24, 64, 128 is a number */
    hi_u32 corek_graph[3] = { 32, 16, 6 }; /* 3, 32, 16, 6 is a number */
    hi_u32 x_kk_graph[3] = { 16, 64, 192 }; /* 3, 16, 64, 192 is a number */
    hi_u32 g_kk_graph[4] = { 312, 256, 128, 64 }; /* 4, 312, 256, 128, 64 is a number */
    hi_u32 k_kk_graph[3] = { 43, 16, 24 }; /* 3, 43, 16, 24 is a number */
    hi_u32 k_graph[3] = { 24, 80, 128 }; /* 3, 24, 80, 128 is a number */
    hi_u32 w_graph = 0;

    hi_u32 c0_graph = 0;
    hi_u32 c1_graph = info->in_width;
    hi_u32 r0_graph = 0;
    hi_u32 r1_graph = info->in_height;
    hi_u32 c0_graph_split = 0;
    hi_u32 c1_graph_split = info->in_width;

    vdp_hipp_sr_setdifth0graph(g_vdp_reg, offset, difth_graph[0]);
    vdp_hipp_sr_setdifth1graph(g_vdp_reg, offset, difth_graph[1]);
    vdp_hipp_sr_setdifth2graph(g_vdp_reg, offset, difth_graph[2]); /* 2 is a number */
    vdp_hipp_sr_setdifth3graph(g_vdp_reg, offset, difth_graph[3]); /* 3 is a number */
    vdp_hipp_sr_setcorek1graph(g_vdp_reg, offset, corek_graph[0]);
    vdp_hipp_sr_setcorek2graph(g_vdp_reg, offset, corek_graph[1]);
    vdp_hipp_sr_setcorek3graph(g_vdp_reg, offset, corek_graph[2]); /* 2 is a number */
    vdp_hipp_sr_setx0kkgraph(g_vdp_reg, offset, x_kk_graph[0]);
    vdp_hipp_sr_setx1kkgraph(g_vdp_reg, offset, x_kk_graph[1]);
    vdp_hipp_sr_setx2kkgraph(g_vdp_reg, offset, x_kk_graph[2]); /* 2 is a number */
    vdp_hipp_sr_setg0kkgraph(g_vdp_reg, offset, g_kk_graph[0]);
    vdp_hipp_sr_setg1kkgraph(g_vdp_reg, offset, g_kk_graph[1]);
    vdp_hipp_sr_setg2kkgraph(g_vdp_reg, offset, g_kk_graph[2]); /* 2 is a number */
    vdp_hipp_sr_setg3kkgraph(g_vdp_reg, offset, g_kk_graph[3]); /* 3 is a number */
    vdp_hipp_sr_setk3kkgraph(g_vdp_reg, offset, k_kk_graph[2]); /* 2 is a number */
    vdp_hipp_sr_setk1kkgraph(g_vdp_reg, offset, k_kk_graph[0]);
    vdp_hipp_sr_setk2kkgraph(g_vdp_reg, offset, k_kk_graph[1]);
    vdp_hipp_sr_setk1graph(g_vdp_reg, offset, k_graph[0]);
    vdp_hipp_sr_setk2graph(g_vdp_reg, offset, k_graph[1]);
    vdp_hipp_sr_setk3graph(g_vdp_reg, offset, k_graph[2]); /* 2 is a number */
    vdp_hipp_sr_setwgraph(g_vdp_reg, offset, w_graph);

    vdp_hipp_sr_setc0graph(g_vdp_reg, offset, c0_graph);
    vdp_hipp_sr_setc1graph(g_vdp_reg, offset, c1_graph);
    vdp_hipp_sr_setc0graphsplit(g_vdp_reg, offset, c0_graph_split);
    vdp_hipp_sr_setc1graphsplit(g_vdp_reg, offset, c1_graph_split);
    vdp_hipp_sr_setr0graph(g_vdp_reg, offset, r0_graph);
    vdp_hipp_sr_setr1graph(g_vdp_reg, offset, r1_graph);
}

static hi_void vdp_zme_set_pq_4ksr_dbsr_mag_dbw(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 x1_mag_dbw_bld = 96;
    hi_u32 x0_mag_dbw_bld = 16;
    hi_u32 k1_mag_dbw_bld = 64;
    hi_u32 k0_mag_dbw_bld = 192;
    hi_u32 k2_mag_dbw_bld = 32;
    hi_u32 g2_mag_dbw_bld = 16;
    hi_u32 g1_mag_dbw_bld = 256;
    hi_u32 g0_mag_dbw_bld = 64;

    hi_u32 x1_mag_dbw = 80;
    hi_u32 x0_mag_dbw = 16;
    hi_u32 k1_mag_dbw = 64;
    hi_u32 k0_mag_dbw = 192;
    hi_u32 k2_mag_dbw = 64;
    hi_u32 g2_mag_dbw = 16;
    hi_u32 g1_mag_dbw = 256;
    hi_u32 g0_mag_dbw = 64;

    vdp_hipp_sr_setx1magdbwbld(g_vdp_reg, offset, x1_mag_dbw_bld);
    vdp_hipp_sr_setx0magdbwbld(g_vdp_reg, offset, x0_mag_dbw_bld);
    vdp_hipp_sr_setk1magdbwbld(g_vdp_reg, offset, k1_mag_dbw_bld);
    vdp_hipp_sr_setk0magdbwbld(g_vdp_reg, offset, k0_mag_dbw_bld);
    vdp_hipp_sr_setk2magdbwbld(g_vdp_reg, offset, k2_mag_dbw_bld);
    vdp_hipp_sr_setg2magdbwbld(g_vdp_reg, offset, g2_mag_dbw_bld);
    vdp_hipp_sr_setg1magdbwbld(g_vdp_reg, offset, g1_mag_dbw_bld);
    vdp_hipp_sr_setg0magdbwbld(g_vdp_reg, offset, g0_mag_dbw_bld);

    vdp_hipp_sr_setx1magdbw(g_vdp_reg, offset, x1_mag_dbw);
    vdp_hipp_sr_setx0magdbw(g_vdp_reg, offset, x0_mag_dbw);
    vdp_hipp_sr_setk1magdbw(g_vdp_reg, offset, k1_mag_dbw);
    vdp_hipp_sr_setk0magdbw(g_vdp_reg, offset, k0_mag_dbw);
    vdp_hipp_sr_setk2magdbw(g_vdp_reg, offset, k2_mag_dbw);
    vdp_hipp_sr_setg2magdbw(g_vdp_reg, offset, g2_mag_dbw);
    vdp_hipp_sr_setg1magdbw(g_vdp_reg, offset, g1_mag_dbw);
    vdp_hipp_sr_setg0magdbw(g_vdp_reg, offset, g0_mag_dbw);
}

static hi_void vdp_zme_set_pq_4ksr_dbsr_mag(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 g1_mag_dirw_adj = 20;
    hi_u32 k1_mag_dirw_adj = 4;
    hi_u32 g0_mag_dirw_adj = 64;
    hi_u32 x0_mag_dirw_adj = 16;
    hi_u32 g1_mag_ssw_adj = 16;
    hi_u32 k1_mag_ssw_adj = 4;
    hi_u32 g0_mag_ssw_adj = 32;
    hi_u32 x0_mag_ssw_adj = 16;
    hi_u32 g1_mag_ssw_kadj = 0;
    hi_u32 k1_mag_ssw_kadj = 8;
    hi_u32 g0_mag_ssw_kadj = 64;
    hi_u32 x0_mag_ssw_kadj = 96;

    hi_u32 k1_magk_conf = 16;
    hi_u32 g0_magk_conf = 16;
    hi_u32 x0_magk_conf = 8;
    hi_u32 k1_magw_conf = 16;
    hi_u32 g0_magw_conf = 0;
    hi_u32 x0_magw_conf = 4;

    vdp_hipp_sr_setg1magdirwadj(g_vdp_reg, offset, g1_mag_dirw_adj);
    vdp_hipp_sr_setk1magdirwadj(g_vdp_reg, offset, k1_mag_dirw_adj);
    vdp_hipp_sr_setg0magdirwadj(g_vdp_reg, offset, g0_mag_dirw_adj);
    vdp_hipp_sr_setx0magdirwadj(g_vdp_reg, offset, x0_mag_dirw_adj);

    vdp_hipp_sr_setg1magsswadj(g_vdp_reg, offset, g1_mag_ssw_adj);
    vdp_hipp_sr_setk1magsswadj(g_vdp_reg, offset, k1_mag_ssw_adj);
    vdp_hipp_sr_setg0magsswadj(g_vdp_reg, offset, g0_mag_ssw_adj);
    vdp_hipp_sr_setx0magsswadj(g_vdp_reg, offset, x0_mag_ssw_adj);
    vdp_hipp_sr_setg1magsswkadj(g_vdp_reg, offset, g1_mag_ssw_kadj);
    vdp_hipp_sr_setk1magsswkadj(g_vdp_reg, offset, k1_mag_ssw_kadj);
    vdp_hipp_sr_setg0magsswkadj(g_vdp_reg, offset, g0_mag_ssw_kadj);
    vdp_hipp_sr_setx0magsswkadj(g_vdp_reg, offset, x0_mag_ssw_kadj);

    vdp_hipp_sr_setk1magkconf(g_vdp_reg, offset, k1_magk_conf);
    vdp_hipp_sr_setg0magkconf(g_vdp_reg, offset, g0_magk_conf);
    vdp_hipp_sr_setx0magkconf(g_vdp_reg, offset, x0_magk_conf);
    vdp_hipp_sr_setk1magwconf(g_vdp_reg, offset, k1_magw_conf);
    vdp_hipp_sr_setg0magwconf(g_vdp_reg, offset, g0_magw_conf);
    vdp_hipp_sr_setx0magwconf(g_vdp_reg, offset, x0_magw_conf);

    vdp_zme_set_pq_4ksr_dbsr_mag_dbw(offset, info);
}

static hi_void vdp_zme_set_pq_4ksr_dbsr_cur(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 k_dif_of = 4;
    hi_u32 x_confw = 16;
    hi_u32 k_confw = 48;
    hi_u32 x_ofw = 32;
    hi_u32 k_ofw = 16;
    hi_u32 x_pdifw = 24;
    hi_u32 k_pdifw = 64;

    vdp_hipp_sr_setkdifof(g_vdp_reg, offset, k_dif_of);
    vdp_hipp_sr_setxconfw(g_vdp_reg, offset, x_confw);
    vdp_hipp_sr_setkconfw(g_vdp_reg, offset, k_confw);
    vdp_hipp_sr_setxofw(g_vdp_reg, offset, x_ofw);
    vdp_hipp_sr_setkofw(g_vdp_reg, offset, k_ofw);
    vdp_hipp_sr_setxpdifw(g_vdp_reg, offset, x_pdifw);
    vdp_hipp_sr_setkpdifw(g_vdp_reg, offset, k_pdifw);
}

static hi_void vdp_zme_set_pq_4ksr_dbsr_wgt(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 k_ssw_dirw_adj = 16;
    hi_u32 k_ssw_ssw_adj = 32;
    hi_u32 x_dbw_ssw_adj = 31;

    hi_u32 k_bp_dbw_bld = 68;
    hi_u32 x_bp_dbw_bld = 8;

    hi_u32 k_minw_dbw_bld = 6;
    hi_u32 k_idbw_ssw_bld = 18;
    hi_u32 x_idbw_ssw_bld = 3;

    vdp_hipp_sr_setksswdirwadj(g_vdp_reg, offset, k_ssw_dirw_adj);
    vdp_hipp_sr_setksswsswadj(g_vdp_reg, offset, k_ssw_ssw_adj);
    vdp_hipp_sr_setxdbwsswadj(g_vdp_reg, offset, x_dbw_ssw_adj);

    vdp_hipp_sr_setkbpdbwbld(g_vdp_reg, offset, k_bp_dbw_bld);
    vdp_hipp_sr_setxbpdbwbld(g_vdp_reg, offset, x_bp_dbw_bld);

    vdp_hipp_sr_setkminwdbwbld(g_vdp_reg, offset, k_minw_dbw_bld);
    vdp_hipp_sr_setkidbwsswbld(g_vdp_reg, offset, k_idbw_ssw_bld);
    vdp_hipp_sr_setxidbwsswbld(g_vdp_reg, offset, x_idbw_ssw_bld);
}

static hi_void vdp_zme_set_pq_4ksr_dbsr_bld(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 k_ssw_bld = 16;
    hi_u32 b_ssw_bld = 0;
    hi_u32 k_dbw_bld = 16;
    hi_u32 b_dbw_bld = 0;

    vdp_hipp_sr_setksswbld(g_vdp_reg, offset, k_ssw_bld);
    vdp_hipp_sr_setbsswbld(g_vdp_reg, offset, b_ssw_bld);
    vdp_hipp_sr_setkdbwbld(g_vdp_reg, offset, k_dbw_bld);
    vdp_hipp_sr_setbdbwbld(g_vdp_reg, offset, b_dbw_bld);
}

static hi_void vdp_zme_set_pq_4ksr_dbsr(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 graph_soft_en = 0;
    hi_u32 para_mode = 0;

    hi_u32 fix_iw_en_dbsr = 0;
    hi_u32 fix_iw_dbsr = 256;

    hi_u32 wsum_lmt = 2048;
    hi_u32 core_kmag_pdif = 8;
    hi_u32 core_mad_pdif = 1;

    hi_u32 lmt_conf_lst = 512;
    hi_u32 k_lmt_conf0 = 8;

    hi_u32 k_lpf_bp_db = 8;
    hi_u32 coring_bp_db = 1;

    vdp_hipp_sr_setgraphsoften(g_vdp_reg, offset, graph_soft_en);
    vdp_hipp_sr_setparamode(g_vdp_reg, offset, para_mode);

    vdp_hipp_sr_setfixiwendbsr(g_vdp_reg, offset, fix_iw_en_dbsr);
    vdp_hipp_sr_setfixiwdbsr(g_vdp_reg, offset, fix_iw_dbsr);

    vdp_hipp_sr_setwsumlmt(g_vdp_reg, offset, wsum_lmt);

    vdp_hipp_sr_setcorekmagpdif(g_vdp_reg, offset, core_kmag_pdif);
    vdp_hipp_sr_setcoremadpdif(g_vdp_reg, offset, core_mad_pdif);

    vdp_hipp_sr_setlmtconflst(g_vdp_reg, offset, lmt_conf_lst);
    vdp_hipp_sr_setklmtconf0(g_vdp_reg, offset, k_lmt_conf0);

    vdp_hipp_sr_setklpfbpdb(g_vdp_reg, offset, k_lpf_bp_db);
    vdp_hipp_sr_setcoringbpdb(g_vdp_reg, offset, coring_bp_db);

    vdp_zme_set_pq_4ksr_dbsr_bicubic(offset, info);
    vdp_zme_set_pq_4ksr_dbsr_graph(offset, info);
    vdp_zme_set_pq_4ksr_dbsr_mag(offset, info);
    vdp_zme_set_pq_4ksr_dbsr_cur(offset, info);
    vdp_zme_set_pq_4ksr_dbsr_wgt(offset, info);
    vdp_zme_set_pq_4ksr_dbsr_bld(offset, info);
}

static hi_void vdp_zme_set_pq_4ksr_self_lamda(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 lamda1_thr = 16;
    hi_u32 lamda1_k2 = 26;
    hi_u32 lamda1_k1 = 24;
    hi_u32 lamda1_g1 = 96;
    hi_u32 lamda2_thr = 16;
    hi_u32 lamda2_k2 = 16;
    hi_u32 lamda2_k1 = 12;
    hi_u32 lamda2_g1 = 48;

    vdp_hipp_sr_setlamda1thr(g_vdp_reg, offset, lamda1_thr);
    vdp_hipp_sr_setlamda1k2(g_vdp_reg, offset, lamda1_k2);
    vdp_hipp_sr_setlamda1k1(g_vdp_reg, offset, lamda1_k1);
    vdp_hipp_sr_setlamda1g1(g_vdp_reg, offset, lamda1_g1);
    vdp_hipp_sr_setlamda2thr(g_vdp_reg, offset, lamda2_thr);
    vdp_hipp_sr_setlamda2k2(g_vdp_reg, offset, lamda2_k2);
    vdp_hipp_sr_setlamda2k1(g_vdp_reg, offset, lamda2_k1);
    vdp_hipp_sr_setlamda2g1(g_vdp_reg, offset, lamda2_g1);
}

static hi_void vdp_zme_set_pq_4ksr_self_cfilter(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 lamda1_coring = 1;
    hi_u32 cfilter_coef_11 = 42;
    hi_u32 cfilter_coef_10 = -10;
    hi_u32 cfilter_coef_03 = 0;
    hi_u32 cfilter_coef_02 = 0;

    vdp_hipp_sr_setlamda1coring(g_vdp_reg, offset, lamda1_coring);
    vdp_hipp_sr_setcfiltercoef11(g_vdp_reg, offset, cfilter_coef_11);
    vdp_hipp_sr_setcfiltercoef10(g_vdp_reg, offset, cfilter_coef_10);
    vdp_hipp_sr_setcfiltercoef03(g_vdp_reg, offset, cfilter_coef_03);
    vdp_hipp_sr_setcfiltercoef02(g_vdp_reg, offset, cfilter_coef_02);
}

static hi_void vdp_zme_set_pq_4ksr_self_yfilter(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 lamda2_coring = 3;
    hi_u32 yfilter_coef_02 = 0;
    hi_u32 yfilter_coef_01 = 64;
    hi_u32 yfilter_coef_00 = 0;
    hi_u32 yfilter_coef_11 = 42;
    hi_u32 yfilter_coef_10 = -10;
    hi_u32 yfilter_coef_03 = 0;
    hi_u32 bisection_hsum_thr1 = 1;
    hi_u32 bisection_hsum_thr0 = 1;
    hi_u32 bisection_vsum_thr2 = 1;
    hi_u32 bisection_vsum_thr1 = 1;
    hi_u32 bisection_vsum_thr0 = 1;
    hi_u32 bisection_hsum_thr2 = 1;

    vdp_hipp_sr_setbisectionhsumthr1(g_vdp_reg, offset, bisection_hsum_thr1);
    vdp_hipp_sr_setbisectionhsumthr0(g_vdp_reg, offset, bisection_hsum_thr0);
    vdp_hipp_sr_setbisectionvsumthr2(g_vdp_reg, offset, bisection_vsum_thr2);
    vdp_hipp_sr_setbisectionvsumthr1(g_vdp_reg, offset, bisection_vsum_thr1);
    vdp_hipp_sr_setbisectionvsumthr0(g_vdp_reg, offset, bisection_vsum_thr0);
    vdp_hipp_sr_setbisectionhsumthr2(g_vdp_reg, offset, bisection_hsum_thr2);

    vdp_hipp_sr_setyfiltercoef02(g_vdp_reg, offset, yfilter_coef_02);
    vdp_hipp_sr_setyfiltercoef01(g_vdp_reg, offset, yfilter_coef_01);
    vdp_hipp_sr_setyfiltercoef00(g_vdp_reg, offset, yfilter_coef_00);
    vdp_hipp_sr_setyfiltercoef11(g_vdp_reg, offset, yfilter_coef_11);
    vdp_hipp_sr_setyfiltercoef10(g_vdp_reg, offset, yfilter_coef_10);
    vdp_hipp_sr_setyfiltercoef03(g_vdp_reg, offset, yfilter_coef_03);

    vdp_hipp_sr_setlamda2coring(g_vdp_reg, offset, lamda2_coring);
}

static hi_void vdp_zme_set_pq_4ksr_self_ang_amend(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 ang_rel_amend_weight1 = 64;
    hi_u32 ang_rel_h_amend_weight1 = 0;
    hi_u32 ang_rel_amend_thr2 = 62;
    hi_u32 ang_rel_amend_thr1 = 7;
    hi_u32 ang_rel_amend_weight2 = 0;
    hi_u32 ang_rel_h_amend_thr1 = 64;
    hi_u32 ang_rel_h_amend_weight2 = 64;
    hi_u32 ang_rel_amend_slop = 0;
    hi_u32 ang_rel_v_amend_weight1 = 0;
    hi_u32 ang_rel_h_amend_slop = 0;
    hi_u32 ang_rel_h_amend_thr2 = 128;
    hi_u32 ang_rel_v_amend_thr2 = 128;
    hi_u32 ang_rel_v_amend_thr1 = 64;
    hi_u32 ang_rel_v_amend_weight2 = 64;
    hi_u32 ang_diff_thr = 8;

    hi_u32 angle_v_amend_weight2 = 0;
    hi_u32 angle_v_amend_weight1 = 64;
    hi_u32 angle_h_amend_slop = 0;
    hi_u32 angle_h_amend_thr2 = 24;
    hi_u32 angle_v_amend_slop = 0;
    hi_u32 angle_v_amend_thr2 = 24;
    hi_u32 angle_v_amend_thr1 = 3;

    vdp_hipp_sr_setangrelamendweight1(g_vdp_reg, offset, ang_rel_amend_weight1);
    vdp_hipp_sr_setangrelhamendweight1(g_vdp_reg, offset, ang_rel_h_amend_weight1);
    vdp_hipp_sr_setangrelamendthr2(g_vdp_reg, offset, ang_rel_amend_thr2);
    vdp_hipp_sr_setangrelamendthr1(g_vdp_reg, offset, ang_rel_amend_thr1);
    vdp_hipp_sr_setangrelamendweight2(g_vdp_reg, offset, ang_rel_amend_weight2);
    vdp_hipp_sr_setangrelhamendthr1(g_vdp_reg, offset, ang_rel_h_amend_thr1);
    vdp_hipp_sr_setangrelhamendweight2(g_vdp_reg, offset, ang_rel_h_amend_weight2);
    vdp_hipp_sr_setangrelamendslop(g_vdp_reg, offset, ang_rel_amend_slop);
    vdp_hipp_sr_setangrelvamendweight1(g_vdp_reg, offset, ang_rel_v_amend_weight1);
    vdp_hipp_sr_setangrelhamendslop(g_vdp_reg, offset, ang_rel_h_amend_slop);
    vdp_hipp_sr_setangrelhamendthr2(g_vdp_reg, offset, ang_rel_h_amend_thr2);
    vdp_hipp_sr_setangrelvamendthr2(g_vdp_reg, offset, ang_rel_v_amend_thr2);
    vdp_hipp_sr_setangrelvamendthr1(g_vdp_reg, offset, ang_rel_v_amend_thr1);
    vdp_hipp_sr_setangrelvamendweight2(g_vdp_reg, offset, ang_rel_v_amend_weight2);

    vdp_hipp_sr_setangdiffthr(g_vdp_reg, offset, ang_diff_thr);

    vdp_hipp_sr_setanglevamendweight2(g_vdp_reg, offset, angle_v_amend_weight2);
    vdp_hipp_sr_setanglevamendweight1(g_vdp_reg, offset, angle_v_amend_weight1);
    vdp_hipp_sr_setanglehamendslop(g_vdp_reg, offset, angle_h_amend_slop);
    vdp_hipp_sr_setanglehamendthr2(g_vdp_reg, offset, angle_h_amend_thr2);
    vdp_hipp_sr_setanglevamendslop(g_vdp_reg, offset, angle_v_amend_slop);
    vdp_hipp_sr_setanglevamendthr2(g_vdp_reg, offset, angle_v_amend_thr2);
    vdp_hipp_sr_setanglevamendthr1(g_vdp_reg, offset, angle_v_amend_thr1);
}

static hi_void vdp_zme_set_pq_4ksr_self_ang_adj(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 ang_diff_gain = 4;
    hi_u32 ang_adj_thr0 = 40;
    hi_u32 ang_adj_thr1 = 40;
    hi_u32 weight_adj_clip = 10;

    hi_u32 angle_h_amend_thr1 = 3;
    hi_u32 angle_h_amend_weight2 = 0;
    hi_u32 angle_h_amend_weight1 = 64;

    hi_u32 sad_max3_thr2 = 36;
    hi_u32 sad_max3_thr1 = 24;
    hi_u32 sad_max3_thr0 = 12;
    hi_u32 sad_max3_adj0 = 6;
    hi_u32 sad_max3_k3 = 40;
    hi_u32 sad_max3_k2 = 32;
    hi_u32 sad_max3_k1 = 24;
    hi_u32 sad_max3_k0 = 16;
    hi_u32 sad_max3_adj2 = 3;
    hi_u32 sad_max3_adj1 = 3;

    vdp_hipp_sr_setangdiffgain(g_vdp_reg, offset, ang_diff_gain);
    vdp_hipp_sr_setangadjthr0(g_vdp_reg, offset, ang_adj_thr0);
    vdp_hipp_sr_setweightadjclip(g_vdp_reg, offset, weight_adj_clip);
    vdp_hipp_sr_setangadjthr1(g_vdp_reg, offset, ang_adj_thr1);

    vdp_hipp_sr_setanglehamendthr1(g_vdp_reg, offset, angle_h_amend_thr1);
    vdp_hipp_sr_setanglehamendweight2(g_vdp_reg, offset, angle_h_amend_weight2);
    vdp_hipp_sr_setanglehamendweight1(g_vdp_reg, offset, angle_h_amend_weight1);

    vdp_hipp_sr_setsadmax3thr2(g_vdp_reg, offset, sad_max3_thr2);
    vdp_hipp_sr_setsadmax3thr1(g_vdp_reg, offset, sad_max3_thr1);
    vdp_hipp_sr_setsadmax3thr0(g_vdp_reg, offset, sad_max3_thr0);
    vdp_hipp_sr_setsadmax3adj0(g_vdp_reg, offset, sad_max3_adj0);
    vdp_hipp_sr_setsadmax3k3(g_vdp_reg, offset, sad_max3_k3);
    vdp_hipp_sr_setsadmax3k2(g_vdp_reg, offset, sad_max3_k2);
    vdp_hipp_sr_setsadmax3k1(g_vdp_reg, offset, sad_max3_k1);
    vdp_hipp_sr_setsadmax3k0(g_vdp_reg, offset, sad_max3_k0);
    vdp_hipp_sr_setsadmax3adj2(g_vdp_reg, offset, sad_max3_adj2);
    vdp_hipp_sr_setsadmax3adj1(g_vdp_reg, offset, sad_max3_adj1);
}

static hi_void vdp_zme_set_pq_4ksr_self_ang(hi_u32 offset, vdp_zme_image_info *info)
{
    vdp_zme_set_pq_4ksr_self_ang_adj(offset, info);
    vdp_zme_set_pq_4ksr_self_ang_amend(offset, info);
}

static hi_void vdp_zme_set_pq_4ksr_self_resbld(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 angle_weight1 = 1024;
    hi_u32 angle_weight4 = 1024;
    hi_u32 angle_weight3 = 256;
    hi_u32 angle_weight2 = 256;
    hi_u32 angle_thr3 = 32;
    hi_u32 angle_thr2 = 16;
    hi_u32 angle_thr1 = 4;
    hi_u32 angle_slop1 = 0;
    hi_u32 angle_thr4 = 64;

    hi_u32 cfilter_coef_00 = 0;
    hi_u32 cfilter_coef_01 = 64;
    hi_u32 ang_rel_v_amend_slop = 0;

    hi_u32 grad_weight1 = 0;
    hi_u32 angle_slop2 = 0;

    hi_u32 bp_diff_weight1 = 1024;
    hi_u32 bp_diff_thr2 = 240;
    hi_u32 bp_diff_thr1 = 80;
    hi_u32 bp_diff_weight2 = 0;
    hi_u32 bp_diff_slop = 0;

    hi_u32 grad_thr2 = 200;
    hi_u32 grad_thr1 = 60;
    hi_u32 grad_weight2 = 1024;
    hi_u32 grad_slop = 0;

    vdp_hipp_sr_setbpdiffweight1(g_vdp_reg, offset, bp_diff_weight1);
    vdp_hipp_sr_setangleslop2(g_vdp_reg, offset, angle_slop2);
    vdp_hipp_sr_setbpdiffthr2(g_vdp_reg, offset, bp_diff_thr2);
    vdp_hipp_sr_setbpdiffthr1(g_vdp_reg, offset, bp_diff_thr1);
    vdp_hipp_sr_setbpdiffweight2(g_vdp_reg, offset, bp_diff_weight2);
    vdp_hipp_sr_setgradweight1(g_vdp_reg, offset, grad_weight1);
    vdp_hipp_sr_setbpdiffslop(g_vdp_reg, offset, bp_diff_slop);
    vdp_hipp_sr_setgradthr2(g_vdp_reg, offset, grad_thr2);
    vdp_hipp_sr_setgradthr1(g_vdp_reg, offset, grad_thr1);
    vdp_hipp_sr_setgradweight2(g_vdp_reg, offset, grad_weight2);
    vdp_hipp_sr_setgradslop(g_vdp_reg, offset, grad_slop);

    vdp_hipp_sr_setangrelvamendslop(g_vdp_reg, offset, ang_rel_v_amend_slop);

    vdp_hipp_sr_setcfiltercoef00(g_vdp_reg, offset, cfilter_coef_00);
    vdp_hipp_sr_setcfiltercoef01(g_vdp_reg, offset, cfilter_coef_01);

    vdp_hipp_sr_setangleweight1(g_vdp_reg, offset, angle_weight1);
    vdp_hipp_sr_setangleweight4(g_vdp_reg, offset, angle_weight4);
    vdp_hipp_sr_setangleweight3(g_vdp_reg, offset, angle_weight3);
    vdp_hipp_sr_setangleweight2(g_vdp_reg, offset, angle_weight2);
    vdp_hipp_sr_setanglethr3(g_vdp_reg, offset, angle_thr3);
    vdp_hipp_sr_setanglethr2(g_vdp_reg, offset, angle_thr2);
    vdp_hipp_sr_setanglethr1(g_vdp_reg, offset, angle_thr1);
    vdp_hipp_sr_setangleslop1(g_vdp_reg, offset, angle_slop1);
    vdp_hipp_sr_setanglethr4(g_vdp_reg, offset, angle_thr4);
}

static hi_void vdp_zme_set_pq_4ksr_self_sr2d_inter(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 sad_max3_coring = 12;
    hi_u32 sad_max1_clip = 120;
    hi_u32 angle_adj_en = 0;
    hi_u32 low_ang_dir_weight = 12;
    hi_u32 high_ang_dir_weight = 12;

    vdp_hipp_sr_setsadmax3coring(g_vdp_reg, offset, sad_max3_coring);
    vdp_hipp_sr_setsadmax1clip(g_vdp_reg, offset, sad_max1_clip);
    vdp_hipp_sr_setangleadjen(g_vdp_reg, offset, angle_adj_en);
    vdp_hipp_sr_setlowangdirweight(g_vdp_reg, offset, low_ang_dir_weight);
    vdp_hipp_sr_sethighangdirweight(g_vdp_reg, offset, high_ang_dir_weight);
}

static hi_void vdp_zme_set_pq_4ksr_self_sr2d(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 min_sad_gain = 4;

    hi_u32 horv_sad_offset = 3;
    hi_u32 diagsad_limitgain = 20;
    hi_u32 sadmono_neib_side0_gain = 16;
    hi_u32 weight_non_gain = 20;
    hi_u32 sad_correct_gain = 6;
    hi_u32 neib_sad_gain = 4;
    hi_u32 weight_lpf_gain = 15;
    hi_u32 horv_sad_gain = 32;
    hi_u32 weight_non_offset = 1;
    hi_u32 sadmono_neib_side0_max_gain = 0;
    hi_u32 diff_sad_weight = 6;
    hi_u32 avg_sad_weight = 10;
    hi_u32 neib_dir_weight = 16;
    hi_u32 sadmono_neib_side1_gain = 4;
    hi_u32 hvdirdiffthr = 1023;
    hi_u32 maxweight_gain = 0;
    hi_u32 flat_detmode = 3;
    hi_u32 flat_coring_gain = 0;
    hi_u32 min_sad_coring = 256;

    vdp_hipp_sr_setminsadgain(g_vdp_reg, offset, min_sad_gain);
    vdp_hipp_sr_sethorvsadoffset(g_vdp_reg, offset, horv_sad_offset);
    vdp_hipp_sr_setdiagsadlimitgain(g_vdp_reg, offset, diagsad_limitgain);
    vdp_hipp_sr_setsadmononeibside0gain(g_vdp_reg, offset, sadmono_neib_side0_gain);
    vdp_hipp_sr_setweightnongain(g_vdp_reg, offset, weight_non_gain);
    vdp_hipp_sr_setsadcorrectgain(g_vdp_reg, offset, sad_correct_gain);
    vdp_hipp_sr_setneibsadgain(g_vdp_reg, offset, neib_sad_gain);
    vdp_hipp_sr_setweightlpfgain(g_vdp_reg, offset, weight_lpf_gain);
    vdp_hipp_sr_sethorvsadgain(g_vdp_reg, offset, horv_sad_gain);
    vdp_hipp_sr_setweightnonoffset(g_vdp_reg, offset, weight_non_offset);
    vdp_hipp_sr_setsadmononeibside0maxgain(g_vdp_reg, offset, sadmono_neib_side0_max_gain);
    vdp_hipp_sr_setdiffsadweight(g_vdp_reg, offset, diff_sad_weight);
    vdp_hipp_sr_setavgsadweight(g_vdp_reg, offset, avg_sad_weight);
    vdp_hipp_sr_setneibdirweight(g_vdp_reg, offset, neib_dir_weight);
    vdp_hipp_sr_setsadmononeibside1gain(g_vdp_reg, offset, sadmono_neib_side1_gain);
    vdp_hipp_sr_sethvdirdiffthr(g_vdp_reg, offset, hvdirdiffthr);
    vdp_hipp_sr_setmaxweightgain(g_vdp_reg, offset, maxweight_gain);
    vdp_hipp_sr_setflatdetmode(g_vdp_reg, offset, flat_detmode);
    vdp_hipp_sr_setflatcoringgain(g_vdp_reg, offset, flat_coring_gain);
    vdp_hipp_sr_setminsadcoring(g_vdp_reg, offset, min_sad_coring);

    vdp_zme_set_pq_4ksr_self_sr2d_inter(offset, info);
}

static hi_void vdp_zme_set_pq_4ksr_self_minsad(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 snd_sad_diff_thr2 = 127;
    hi_u32 snd_sad_diff_thr1 = 15;
    hi_u32 snd_sad_diff_weight2 = 0;
    hi_u32 snd_sad_diff_weight1 = 64;
    hi_u32 snd_sad_diff_slop = 0;
    hi_u32 match_weight_clip = 512;
    hi_u32 block6x6_amend_mode = 0;
    hi_u32 min_sad_adjust_thr1 = 1023;
    hi_u32 min_sad_adjust_thr0 = 511;

    hi_u32 min_sad_thr1 = 51;
    hi_u32 min_sad_weight2 = 0;
    hi_u32 min_sad_weight1 = 64;
    hi_u32 min_sad_slop = 0;
    hi_u32 min_sad_thr2 = 205;

    vdp_hipp_sr_setminsadthr1(g_vdp_reg, offset, min_sad_thr1);
    vdp_hipp_sr_setminsadweight2(g_vdp_reg, offset, min_sad_weight2);
    vdp_hipp_sr_setminsadweight1(g_vdp_reg, offset, min_sad_weight1);
    vdp_hipp_sr_setminsadslop(g_vdp_reg, offset, min_sad_slop);
    vdp_hipp_sr_setminsadthr2(g_vdp_reg, offset, min_sad_thr2);

    vdp_hipp_sr_setmatchweightclip(g_vdp_reg, offset, match_weight_clip);
    vdp_hipp_sr_setblock6x6amendmode(g_vdp_reg, offset, block6x6_amend_mode);

    vdp_hipp_sr_setminsadadjustthr1(g_vdp_reg, offset, min_sad_adjust_thr1);
    vdp_hipp_sr_setminsadadjustthr0(g_vdp_reg, offset, min_sad_adjust_thr0);

    vdp_hipp_sr_setsndsaddiffthr2(g_vdp_reg, offset, snd_sad_diff_thr2);
    vdp_hipp_sr_setsndsaddiffthr1(g_vdp_reg, offset, snd_sad_diff_thr1);
    vdp_hipp_sr_setsndsaddiffweight2(g_vdp_reg, offset, snd_sad_diff_weight2);
    vdp_hipp_sr_setsndsaddiffweight1(g_vdp_reg, offset, snd_sad_diff_weight1);
    vdp_hipp_sr_setsndsaddiffslop(g_vdp_reg, offset, snd_sad_diff_slop);
}

static hi_void vdp_zme_set_pq_4ksr_self_sad(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 sad_diff_thr2 = 71;
    hi_u32 sad_diff_thr1 = 3;
    hi_u32 sad_diff_weight2 = 0;
    hi_u32 sad_diff_weight1 = 48;
    hi_u32 sad_diff_slop = 0;
    hi_u32 min_sad_ratio = 12;

    vdp_hipp_sr_setminsadratio(g_vdp_reg, offset, min_sad_ratio);

    vdp_hipp_sr_setsaddiffthr2(g_vdp_reg, offset, sad_diff_thr2);
    vdp_hipp_sr_setsaddiffthr1(g_vdp_reg, offset, sad_diff_thr1);
    vdp_hipp_sr_setsaddiffweight2(g_vdp_reg, offset, sad_diff_weight2);
    vdp_hipp_sr_setsaddiffweight1(g_vdp_reg, offset, sad_diff_weight1);
    vdp_hipp_sr_setsaddiffslop(g_vdp_reg, offset, sad_diff_slop);
}

static hi_void vdp_zme_set_pq_4ksr_self_bicubic(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 bicubic_phase03 = -4;
    hi_u32 bicubic_phase02 = 59;
    hi_u32 bicubic_phase01 = 10;
    hi_u32 bicubic_phase00 = -1;
    hi_u32 bicubic_phase11 = 36;
    hi_u32 bicubic_phase10 = -4;

    vdp_hipp_sr_setbicubicphase03(g_vdp_reg, offset, bicubic_phase03);
    vdp_hipp_sr_setbicubicphase02(g_vdp_reg, offset, bicubic_phase02);
    vdp_hipp_sr_setbicubicphase01(g_vdp_reg, offset, bicubic_phase01);
    vdp_hipp_sr_setbicubicphase00(g_vdp_reg, offset, bicubic_phase00);
    vdp_hipp_sr_setbicubicphase11(g_vdp_reg, offset, bicubic_phase11);
    vdp_hipp_sr_setbicubicphase10(g_vdp_reg, offset, bicubic_phase10);
}

static hi_void vdp_zme_set_pq_4ksr_self_sumsad(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 sum_sad_thr1 = 51;
    hi_u32 sum_sad_weight2 = 0;
    hi_u32 sum_sad_weight1 = 12;
    hi_u32 sum_sad_slop = 0;
    hi_u32 sum_sad_thr2 = 205;

    vdp_hipp_sr_setsumsadthr1(g_vdp_reg, offset, sum_sad_thr1);
    vdp_hipp_sr_setsumsadweight2(g_vdp_reg, offset, sum_sad_weight2);
    vdp_hipp_sr_setsumsadweight1(g_vdp_reg, offset, sum_sad_weight1);
    vdp_hipp_sr_setsumsadslop(g_vdp_reg, offset, sum_sad_slop);
    vdp_hipp_sr_setsumsadthr2(g_vdp_reg, offset, sum_sad_thr2);
}

static hi_void vdp_zme_set_pq_4ksr_self(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 bisection_vcount_thr = 2;
    hi_u32 bisection_hcount_thr = 2;

    vdp_hipp_sr_setbisectionvcountthr(g_vdp_reg, offset, bisection_vcount_thr);
    vdp_hipp_sr_setbisectionhcountthr(g_vdp_reg, offset, bisection_hcount_thr);

    vdp_zme_set_pq_4ksr_self_lamda(offset, info);
    vdp_zme_set_pq_4ksr_self_cfilter(offset, info);
    vdp_zme_set_pq_4ksr_self_yfilter(offset, info);
    vdp_zme_set_pq_4ksr_self_ang(offset, info);
    vdp_zme_set_pq_4ksr_self_resbld(offset, info);
    vdp_zme_set_pq_4ksr_self_sr2d(offset, info);
    vdp_zme_set_pq_4ksr_self_minsad(offset, info);
    vdp_zme_set_pq_4ksr_self_sad(offset, info);
    vdp_zme_set_pq_4ksr_self_bicubic(offset, info);
    vdp_zme_set_pq_4ksr_self_sumsad(offset, info);
}

static hi_void vdp_zme_set_pq_4ksr_demo(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 demo_en = 0;
    hi_u32 demo_mode = 0;
    hi_u32 demo_pos = 1;
    hi_u32 dbsr_demo_en = 0;
    hi_u32 dbsr_demo_mode = 0;

    vdp_hipp_sr_setdemoen(g_vdp_reg, offset, demo_en);
    vdp_hipp_sr_setdemomode(g_vdp_reg, offset, demo_mode);
    vdp_hipp_sr_setdemopos(g_vdp_reg, offset, demo_pos);

    vdp_hipp_sr_setdbsrdemoen(g_vdp_reg, offset, dbsr_demo_en);
    vdp_hipp_sr_setdbsrdemomode(g_vdp_reg, offset, dbsr_demo_mode);
}

static hi_void vdp_zme_set_pq_4ksr(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 outresult = 0;
#ifdef VDP_SDK_PQ_SUPPORT
    /* if pq exist, cfg by pq */
    return;
#endif

    vdp_hipp_sr_setoutresult(g_vdp_reg, offset, outresult);

    vdp_zme_set_pq_4ksr_dbsr(offset, info);
    vdp_zme_set_pq_4ksr_self(offset, info);
    vdp_zme_set_pq_4ksr_demo(offset, info);

    return;
}

static hi_void vdp_zme_set_4ksr(hi_u32 layer, vdp_zme_image_info *info)
{
    hi_u32 offset = g_sr4k_reg[layer];
#ifndef VDP_SDK_PQ_SUPPORT
    hi_u32 scale_mode = 0x8;
#endif

    vdp_hipp_sr_setsren(g_vdp_reg, offset, info->v_enable);
    if (info->v_enable == HI_FALSE) {
        return;
    }

    vdp_hipp_sr_sethippsrckgten(g_vdp_reg, offset, HI_TRUE);
#ifndef VDP_SDK_PQ_SUPPORT
    vdp_hipp_sr_setscalemode(g_vdp_reg, offset, scale_mode);
#endif

    vdp_zme_set_pq_4ksr(offset, info);

#ifdef VDP_CBB_TEST_SUPPORT
    vdp_zme_test_4ksr(offset, info);
#endif

    return;
}

static hi_void vdp_zme_splite_alg_8ksr_demo(vdp_zme_image_info *info, vdp_8k_sr_split_out *split_out)
{
    hi_u32 over_lap = 30;
    hi_u32 iw = info->in_width / 2; /* 2 is a number */

    hi_u32 left_width = (info->in_width / 2 + 11) / 12 * 12; /* 2, 11, 12, 12 is a number */
    hi_u32 right_width = info->in_width - left_width;

    hi_u32 demo_en = 0;
    hi_u32 demo_mode = 0;
    hi_u32 demo_pos = 0;
    hi_u32 i;

    if (demo_en == 0) {
        for (i = 0; i < VDP_8K_SR_SPLIT_MAX; i++) {
            split_out->demo_en[i] = demo_en;
            split_out->demo_pos[i] = demo_pos;
            split_out->demo_mode[i] = demo_mode;
        }
    } else {
        if (demo_mode == 0) {
            if (demo_pos <= (left_width * 2)) { /* 2 is a number */
                split_out->demo_pos[0] = demo_pos;
                split_out->demo_mode[0] = demo_mode;

                split_out->demo_en[1] = 0;
                split_out->demo_pos[1] = demo_pos;
            } else {
                split_out->demo_pos[0] = 0;
                split_out->demo_mode[0] = 1;

                split_out->demo_en[1] = demo_en;
                split_out->demo_pos[1] = right_width * 2 + over_lap * 2 - iw * 2 + demo_pos; /* 2 is a number */
            }
            split_out->demo_en[0] = demo_en;
            split_out->demo_mode[1] = demo_mode;
        } else {
            if (demo_pos < (left_width * 2)) { /* 2 is a number */
                split_out->demo_en[0] = demo_en;
                split_out->demo_pos[1] = 0;
            } else {
                split_out->demo_en[0] = 0;
                split_out->demo_pos[1] = right_width * 2 + over_lap * 2 - iw * 2 + demo_pos; /* 2 is a number */
            }
            split_out->demo_pos[0] = demo_pos;
            split_out->demo_mode[0] = demo_mode;
            split_out->demo_en[1] = demo_en;
            split_out->demo_mode[1] = demo_mode;
        }
    }
}

static hi_void vdp_zme_splite_alg_8ksr_demo_dbsr(vdp_zme_image_info *info, vdp_8k_sr_split_out *split_out)
{
    hi_u32 over_lap = 30;
    hi_u32 iw = info->in_width / 2; /* 2 is a number */

    hi_u32 dbsr_demo_en = 0;
    hi_u32 dbsr_demo_mode = 0;
    hi_u32 dbsr_demo_pos = 0;

    hi_u32 left_w = (info->in_width / 2 + 11) / 12 * 12; /* 2, 11, 12, 12 is a number */
    hi_u32 right_w = info->in_width - left_w;
    hi_u32 i;

    if (dbsr_demo_en == 0) {
        for (i = 0; i < VDP_8K_SR_SPLIT_MAX; i++) {
            split_out->dbsr_demo_en[i] = dbsr_demo_en;
            split_out->dbsr_demo_pos[i] = dbsr_demo_pos;
            split_out->dbsr_demo_mode[i] = dbsr_demo_mode;
        }
    } else {
        if (dbsr_demo_mode == 0) {
            if (dbsr_demo_pos <= (left_w * 2)) { /* 2 is a number */
                split_out->dbsr_demo_pos[0] = dbsr_demo_pos;
                split_out->dbsr_demo_mode[0] = dbsr_demo_mode;
                split_out->dbsr_demo_en[1] = 0;
                split_out->dbsr_demo_pos[1] = dbsr_demo_pos;
            } else {
                split_out->dbsr_demo_pos[0] = 0;
                split_out->dbsr_demo_mode[0] = 1;
                split_out->dbsr_demo_en[1] = dbsr_demo_en;
                split_out->dbsr_demo_pos[1] = right_w * 2 + over_lap * 2 - iw * 2 + dbsr_demo_pos; /* 2 is a number */
            }
            split_out->dbsr_demo_en[0] = dbsr_demo_en;
            split_out->dbsr_demo_mode[1] = dbsr_demo_mode;
        } else {
            if (dbsr_demo_pos < (left_w * 2)) { /* 2 is a number */
                split_out->dbsr_demo_en[0] = dbsr_demo_en;
                split_out->dbsr_demo_pos[1] = 0;
            } else {
                split_out->dbsr_demo_en[0] = 0;
                split_out->dbsr_demo_pos[1] = right_w * 2 + over_lap * 2 - iw * 2 + dbsr_demo_pos; /* 2 is a number */
            }
            split_out->dbsr_demo_pos[0] = dbsr_demo_pos;
            split_out->dbsr_demo_mode[0] = dbsr_demo_mode;
            split_out->dbsr_demo_en[1] = dbsr_demo_en;
            split_out->dbsr_demo_mode[1] = dbsr_demo_mode;
        }
    }
}

static hi_void vdp_zme_splite_alg_8ksr(vdp_zme_image_info *info, vdp_8k_sr_split_out *split_out)
{
    hi_u32 over_lap = 30;

    hi_u32 c0_graph = 0;
    hi_u32 c1_graph = info->in_width;
    hi_u32 left_width = (info->in_width / 2 + 11) / 12 * 12; /* 2, 11, 12, 12 is a number */

    vdp_zme_splite_alg_8ksr_demo(info, split_out);
    vdp_zme_splite_alg_8ksr_demo_dbsr(info, split_out);

    if ((c0_graph <= left_width) && (c1_graph <= left_width)) {
        split_out->c0_graph_split[0] = c0_graph;
        split_out->c1_graph_split[0] = c1_graph;
        split_out->c0_graph_split[1] = 0;
        split_out->c1_graph_split[1] = 0;
    } else if ((c0_graph >= left_width) && (c1_graph >= left_width)) {
        split_out->c0_graph_split[0] = 0;
        split_out->c1_graph_split[0] = 0;
        split_out->c0_graph_split[1] = c0_graph - left_width + over_lap;
        split_out->c1_graph_split[1] = c1_graph - left_width + over_lap;
    } else if ((c0_graph <= left_width) && (c1_graph >= left_width)) {
        split_out->c0_graph_split[0] = c0_graph;
        split_out->c1_graph_split[0] = left_width;
        split_out->c0_graph_split[1] = over_lap;
        split_out->c1_graph_split[1] = c1_graph - left_width + over_lap;
    }

    return;
}

static hi_void vdp_zme_set_pq_8ksr(hi_u32 offset, vdp_zme_image_info *info)
{
    hi_u32 split_offset;
    hi_u32 outresult = 0;
    hi_u32 para_mode = 0;
    hi_u32 graph_soft_en = 0;
    hi_u32 i;

    vdp_8k_sr_split_out split_out = {0};
#ifdef VDP_SDK_PQ_SUPPORT
    /* if pq exist, cfg by pq */
    return;
#endif

    vdp_zme_splite_alg_8ksr(info, &split_out);

    vdp_zme_set_pq_4ksr(offset, info);

    for (i = 0; i < VDP_8K_SR_SPLIT_MAX; i++) {
        split_offset = offset + g_sr8k_split_offset_reg[i];

        vdp_hipp_sr_setsren(g_vdp_reg, split_offset, HI_TRUE);
        vdp_hipp_sr_setoutresult(g_vdp_reg, split_offset, outresult);
        vdp_hipp_sr_sethippsrckgten(g_vdp_reg, split_offset, HI_TRUE);
        vdp_hipp_sr_setscalemode(g_vdp_reg, split_offset, 0x8);
        vdp_hipp_sr_setparamode(g_vdp_reg, split_offset, para_mode);
        vdp_hipp_sr_setgraphsoften(g_vdp_reg, split_offset, graph_soft_en);
        vdp_hipp_sr_setdemoen(g_vdp_reg, split_offset, split_out.demo_en[i]);
        vdp_hipp_sr_setdbsrdemoen(g_vdp_reg, split_offset, split_out.dbsr_demo_en[i]);
        vdp_hipp_sr_setdemomode(g_vdp_reg, split_offset, split_out.demo_mode[i]);
        vdp_hipp_sr_setdbsrdemomode(g_vdp_reg, split_offset, split_out.dbsr_demo_mode[i]);

        if ((split_out.demo_en[i] == 0) && (split_out.dbsr_demo_en[i] == 1)) {
            vdp_hipp_sr_setdemopos(g_vdp_reg, split_offset, split_out.dbsr_demo_pos[i]);
        } else {
            vdp_hipp_sr_setdemopos(g_vdp_reg, split_offset, split_out.demo_pos[i]);
        }

        vdp_hipp_sr_setc0graphsplit(g_vdp_reg, split_offset, split_out.c0_graph_split[i]);
        vdp_hipp_sr_setc1graphsplit(g_vdp_reg, split_offset, split_out.c1_graph_split[i]);
    }

    return;
}

static hi_void vdp_zme_set_8ksr(hi_u32 layer, vdp_zme_image_info *info)
{
    hi_u32 offset = g_sr8k_reg[layer];
#ifndef VDP_SDK_PQ_SUPPORT
    hi_u32 scale_mode = 0x8;
#endif

    vdp_hipp_sr_setsren(g_vdp_reg, offset, info->v_enable);
    if (info->v_enable == HI_FALSE) {
        return;
    }
    vdp_vid_set8kclmparasrc4kenable(g_vdp_reg, VDP_LAYER_VID0, HI_TRUE);
    vdp_hipp_sr_sethippsrckgten(g_vdp_reg, offset, HI_TRUE);

#ifndef VDP_SDK_PQ_SUPPORT
    vdp_hipp_sr_setscalemode(g_vdp_reg, offset, scale_mode);
#endif

    vdp_zme_set_pq_8ksr(offset, info);

#ifdef VDP_CBB_TEST_SUPPORT
    vdp_zme_test_8ksr(offset, info);
#endif

    return;
}

#define MUX_4K_ZME_MAX_WIDTH 4096
#define MUX_4K_ZME_MAX_HEIGHT 2160
hi_void vdp_ip_zme_update_mux(hi_u32 layer, vdp_zme_info *zme_info)
{
    if ((zme_info->image_info[VDP_ZME_8K_SR].v_enable == HI_FALSE)
        && (zme_info->cle_8k_enable == HI_FALSE)) {
        vdp_vid_set8ksrmuxdisable(g_vdp_reg, VDP_LAYER_VID0, HI_TRUE);
        vdp_vid_set8ksrpwrupingenable(g_vdp_reg, VDP_LAYER_VID0, HI_TRUE);
    } else {
        vdp_vid_set8ksrmuxdisable(g_vdp_reg, VDP_LAYER_VID0, HI_FALSE);
        vdp_vid_set8ksrpwrupingenable(g_vdp_reg, VDP_LAYER_VID0, HI_FALSE);
        vdp_vid_set8ksrpwrdownenable(g_vdp_reg, VDP_LAYER_VID0, HI_FALSE);
    }

    if ((zme_info->image_info[VDP_ZME_4K_SR].v_enable == HI_FALSE)
        && (zme_info->cle_4k_enable == HI_FALSE)) {
        vdp_vid_set4ksrmuxdisable(g_vdp_reg, VDP_LAYER_VID0, HI_TRUE);
        vdp_vid_set4ksrpwrdownenable(g_vdp_reg, VDP_LAYER_VID0, HI_TRUE);
    } else {
        vdp_vid_set4ksrmuxdisable(g_vdp_reg, VDP_LAYER_VID0, HI_FALSE);
        vdp_vid_set4ksrpwrdownenable(g_vdp_reg, VDP_LAYER_VID0, HI_FALSE);
        vdp_vid_set4ksrpwrupingenable(g_vdp_reg, VDP_LAYER_VID0, HI_FALSE);
    }

    if ((zme_info->image_info[VDP_ZME_4K_ZME].in_width <= MUX_4K_ZME_MAX_WIDTH) &&
        (zme_info->image_info[VDP_ZME_4K_ZME].in_height <= MUX_4K_ZME_MAX_HEIGHT)) {
        vdp_vid_set4kzmemuxdisable(g_vdp_reg, VDP_LAYER_VID0, HI_FALSE);
    } else {
        vdp_vid_set4kzmemuxdisable(g_vdp_reg, VDP_LAYER_VID0, HI_TRUE);
    }
}

hi_void vdp_ip_zme_update_split(hi_u32 layer, vdp_zme_info *zme_info)
{
    hi_u32 left_w;
    hi_u32 right_w;

    if (zme_info->image_info[VDP_ZME_8K_SR].v_enable == HI_TRUE) {
        left_w = (zme_info->image_info[VDP_ZME_8K_SR].in_width / 2 + 11) / 12 * 12; /* 2,11,12 is align strategy.*/
        right_w = zme_info->image_info[VDP_ZME_8K_SR].in_width - left_w;
    } else if (zme_info->cle_8k_enable == HI_TRUE) {
        left_w = (zme_info->image_info[VDP_ZME_8K_SR].in_width / 2 + 22) / 12 * 12; /* 2,22,12 is align strategy.*/
        right_w = zme_info->image_info[VDP_ZME_8K_SR].in_width - left_w;
    } else if (zme_info->image_info[VDP_ZME_4K_SR].v_enable == HI_TRUE) {
        left_w = (zme_info->image_info[VDP_ZME_4K_SR].out_width / 2 + 11) / 12 * 12; /* 2,11,12 is align strategy.*/
        right_w = zme_info->image_info[VDP_ZME_4K_SR].out_width - left_w;
    } else {
        left_w = (zme_info->image_info[VDP_ZME_8K_SR].in_width / 2 + 11) / 12 * 12; /* 2,11,12 is align strategy.*/
        right_w = zme_info->image_info[VDP_ZME_8K_SR].in_width - left_w;
    }

    vdp_vid_set8ksrtwochnwth(g_vdp_reg, 0, left_w, right_w);
}

hi_void vdp_ip_zme_set_v0(hi_u32 layer, vdp_zme_info *zme_info)
{
    vdp_para_setparaaddrv0chn12(g_vdp_reg, g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_XDP_2DSCALE_V]);
    vdp_para_setparaaddrv0chn13(g_vdp_reg, g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_XDP_2DSCALE_H]);
    vdp_para_setparaaddrv0chn10(g_vdp_reg, g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_8K_VZME]);
    vdp_para_setparaaddrv0chn11(g_vdp_reg, g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_8K_HZME]);
    vdp_para_setparaaddrv0chn19(g_vdp_reg, g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_4K_SR0]);
    vdp_para_setparaaddrv0chn21(g_vdp_reg, g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_8K_SR0]);

    /* 4kzme */
    vdp_zme_set_4kzme(layer, &(zme_info->image_info[VDP_ZME_4K_ZME]));

    /* 8kzme */
    vdp_zme_set_8kzme(layer, &(zme_info->image_info[VDP_ZME_8K_ZME]));

    /* 4ksr */
    vdp_zme_set_4ksr(layer, &(zme_info->image_info[VDP_ZME_4K_SR]));

    /* 8ksr */
    vdp_zme_set_8ksr(layer, &(zme_info->image_info[VDP_ZME_8K_SR]));

    vdp_para_setparaupv0chn(g_vdp_reg, VDP_COEF_BUF_XDP_2DSCALE_H);
    vdp_para_setparaupv0chn(g_vdp_reg, VDP_COEF_BUF_XDP_2DSCALE_V);
    vdp_para_setparaupv0chn(g_vdp_reg, VDP_COEF_BUF_8K_HZME);
    vdp_para_setparaupv0chn(g_vdp_reg, VDP_COEF_BUF_8K_VZME);
    vdp_para_setparaupv0chn(g_vdp_reg, VDP_COEF_BUF_4K_SR0);
    vdp_para_setparaupv0chn(g_vdp_reg, VDP_COEF_BUF_8K_SR0);

    vdp_ip_zme_update_mux(layer, zme_info);
    vdp_ip_zme_update_split(layer, zme_info);

    return;
}

hi_void vdp_ip_zme_set_v1(hi_u32 layer, vdp_zme_info *zme_info)
{
    vdp_para_setparaaddrvhdchn05(g_vdp_reg, g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_V1_VZME]);
    vdp_para_setparaaddrvhdchn06(g_vdp_reg, g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_V1_HZME]);

    vdp_zme_set_8kzme(layer, &(zme_info->image_info[VDP_ZME_8K_ZME]));

    vdp_para_setparaupvhdchn(g_vdp_reg, VDP_COEF_BUF_V1_VZME);
    vdp_para_setparaupvhdchn(g_vdp_reg, VDP_COEF_BUF_V1_HZME);
}

hi_void vdp_ip_zme_set_v3(hi_u32 layer, vdp_zme_image_info *info)
{
    hi_u32 vratio = 1 << 12; /* 12 is a number */
    hi_s32 ifir_v3_coef[8] = {0}; /* 8 is a number */

    vratio = (info->in_height * vratio) / info->in_height;

    vdp_cvfir_setvzmeckgten(g_vdp_reg, HI_TRUE);
    vdp_cvfir_setvratio(g_vdp_reg, vratio);

    if (info->in_pix_fmt == 0) {
        vdp_cvfir_setoutpro(g_vdp_reg, info->out_mode);
        vdp_cvfir_setoutfmt(g_vdp_reg, info->out_pix_fmt);
        vdp_cvfir_setoutheight(g_vdp_reg, info->out_height);
        vdp_cvfir_setcvfiren(g_vdp_reg, info->v_enable);
        vdp_cvfir_setcvmiden(g_vdp_reg, info->v_med_en);
        vdp_cvfir_setcvfirmode(g_vdp_reg, info->v_cfir_en);
        vdp_cvfir_setvchromaoffset(g_vdp_reg, info->offset.v_coffset);
        vdp_cvfir_setvbchromaoffset(g_vdp_reg, info->offset.v_bcoffset);
#ifdef VDP_CBB_TEST_SUPPORT
        vdp_zme_test_ifir(layer, info);
#endif
    }

    if (info->in_pix_fmt != 2) { /* 2 is a number */
        vdp_vid_setifirmode(g_vdp_reg, VDP_LAYER_VID3, VDP_IFIRMODE_FILT);
        ifir_v3_coef[0] = -14; /* -14 is a number */
        ifir_v3_coef[1] = 18; /* 18 is a number */
        ifir_v3_coef[2] = -24; /* 2 is a number -24 is a number */
        ifir_v3_coef[3] = 32; /* 3 is a number 32 is a number */
        ifir_v3_coef[4] = -43; /* 4 is a number -43 is a number */
        ifir_v3_coef[5] = 64; /* 5 is a number 64 is a number */
        ifir_v3_coef[6] = -109; /* 6 is a number -109 is a number */
        ifir_v3_coef[7] = 332; /* 7 is a number 332 is a number */
        vdp_vid_setifircoef(g_vdp_reg, VDP_LAYER_VID3, ifir_v3_coef);
    }
}

hi_void vdp_ip_zme_set(hi_u32 layer, vdp_zme_info *zme_info)
{
    if (layer == VDP_LAYER_VID0) {
        vdp_ip_zme_set_v0(layer, zme_info);
    } else if (layer == VDP_LAYER_VID1) {
        vdp_ip_zme_set_v1(layer, zme_info);
    } else if (layer == VDP_LAYER_VID3) {
        vdp_ip_zme_set_v3(layer, &(zme_info->image_info[VDP_ZME_8K_ZME]));
    }
}

