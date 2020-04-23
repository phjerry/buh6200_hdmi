/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: hifb alg decompress
 * Author: sdk
 * Create: 2019-08-15
 */
#include "hifb_alg_decmp.h"

#define MB_WTH 32

#ifndef ABS
#define ABS(x) ((x) >= 0 ? (x) : (-(x)))
#endif

#ifndef SIGN
#define SIGN(x) ((x) >= 0 ? 1 : -1)
#endif

#ifndef CMP
#define CMP(x, y) (((x) == (y)) ? 0 : (((x) > (y)) ? 1 : -1))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define CLIP(a) (((a) >= 0) ? (a) : 0)

static hi_void ice_v3r2_line_osd_cfg_init_thr(ice_v3r2_line_osd_cfg *rm_cfg, hi_u32 bit_depth, hi_u32 cpt_num)
{
    rm_cfg->smth_thr = 6 * bit_depth / 8;                                       /* 6 8 is alg num */
    rm_cfg->still_thr = 1 * bit_depth / 8;                                      /* 1 8 is alg num */
    rm_cfg->big_grad_thr = 30 * bit_depth / 8;                                  /* 30 8 is alg num */
    rm_cfg->diff_thr = 20 * bit_depth / 8;                                      /* 20 8 is alg num */
    rm_cfg->smth_pix_num_thr = 6;                                               /* 6 is alg num */
    rm_cfg->still_pix_num_thr = 10;                                             /* 10 is alg num */
    rm_cfg->noise_pix_num_thr = 16;                                             /* 16 is alg num */
    rm_cfg->grph_group_num_thr = 6;                                             /* 6 is alg num */
    rm_cfg->qp_inc1_bits_thr = 10 * cpt_num;                                    /* 10 is alg num */
    rm_cfg->qp_inc2_bits_thr = 15 * cpt_num;                                    /* 15 is alg num */
    rm_cfg->qp_dec1_bits_thr = 10 * cpt_num;                                    /* 10 is alg num */
    rm_cfg->qp_dec2_bits_thr = 20 * cpt_num;                                    /* 20 is alg num */
    rm_cfg->raw_bits_penalty = 10;                                              /* 10 is alg num */
    rm_cfg->grph_loss_thr = 4;                                                  /* 4 is alg num */
    rm_cfg->buf_fullness_thr_reg0 = (71 << 24) | (56 << 16) | (35 << 8) | 18;   /* 71 24 56 16 35 8 18 is alg num */
    rm_cfg->buf_fullness_thr_reg1 = (109 << 24) | (108 << 16) | (98 << 8) | 85; /* 109 24 108 16 98 8 85 is alg num */
    rm_cfg->buf_fullness_thr_reg2 = (0 << 24) | (116 << 16) | (114 << 8) | 110; /* 0 24 116 16 114 8 110 is alg num */
    rm_cfg->bits_offset_reg0 = ((-2 & 0xff) << 24) | ((-1 & 0xff) << 16) |      /* -2 24 -1 16 is alg num */
                               ((0 & 0xff) << 8) | (2 & 0xff);                  /* 0 8 2 is alg num */
    rm_cfg->bits_offset_reg1 = ((-4 & 0xff) << 24) | ((-4 & 0xff) << 16) |      /* -4 24 -4 16 is alg num */
                               ((-4 & 0xff) << 8) | (-3 & 0xff);                /* -4 8 -3 is alg num */
    rm_cfg->bits_offset_reg2 = ((-4 & 0xff) << 24) | ((-4 & 0xff) << 16) |      /* -4 24 -4 16 is alg num */
                               ((-4 & 0xff) << 8) | (-4 & 0xff);                /* -4 8 -4 is alg num */
    rm_cfg->est_err_gain_map = (7 << 28) | (7 << 24) | (7 << 20) | (7 << 16) |  /* 7 28 7 24 7 20 7 16 is alg num */
                               (6 << 12) | (6 << 8) | (5 << 4) | 5;             /* 6 12 6 8 5 4 5 is alg num */
    rm_cfg->smooth_status_thr = 10;                                             /* 10 is alg num */
    rm_cfg->reserve_para0 = (((10 * cpt_num) & 0xff) << 24) |                   /* 10 24 is alg num */
                            ((0 & 0xff) << 16) |                                /* 16 is alg num */
                            (((63 * cpt_num) & 0xff) << 8) |                    /* 63 8 is alg num */
                            ((32 * cpt_num) & 0xff);                            /* 32 is alg num */
}

static hi_void ice_v3r2_line_osd_cfg_init_base(ice_v3r2_line_osd_cfg *rm_cfg, ice_v3r2_line_osd_input *input)
{
    hi_u32 mb_ori_bits;
    hi_u32 mb_safe_bit;
    hi_u32 bit_depth = input->bit_depth * 2 + 8;     /* 2 & 8 calc to 888 or 101010 */
    hi_u32 cpt_num = input->pix_format == 0 ? 3 : 4; /* 3 for rgb, 4 for argb */
    hi_u32 mb_num_x, delta, lastMbWth;

    rm_cfg->is_lossless = input->is_lossless;
    rm_cfg->cmp_mode = input->cmp_mode;
    rm_cfg->pix_format = input->pix_format;
    rm_cfg->frame_width = input->frame_width;
    rm_cfg->frame_height = input->frame_height;
    rm_cfg->rm_debug_en = input->rc_type_mode ? 0 : 1;
    rm_cfg->bit_depth = input->bit_depth;

    rm_cfg->conv_en = input->conv_en;
    rm_cfg->buffer_init_bits = 7000; /* alg num: 7000 */
    rm_cfg->buffer_size = 8192;      /* alg num: 8192 */

    lastMbWth = (rm_cfg->frame_width % MB_WTH) ? (rm_cfg->frame_width % MB_WTH) : MB_WTH;
    mb_ori_bits = bit_depth * MB_WTH * cpt_num;
    mb_safe_bit = 2 * MB_WTH * cpt_num; /* 2 for alg num */

    mb_num_x = (rm_cfg->frame_width + MB_WTH - 1) / MB_WTH;
    lastMbWth = (rm_cfg->frame_width % MB_WTH) ? (rm_cfg->frame_width % MB_WTH) : MB_WTH;
    /* 2 is alg num */
    delta = MAX(1, (rm_cfg->buffer_size - rm_cfg->buffer_init_bits + mb_num_x / 2) / (mb_num_x));

    rm_cfg->budget_mb_bits = MB_WTH * bit_depth * cpt_num * 1000 / input->comp_ratio_int; /* 1000 is alg num */
    rm_cfg->budget_mb_bits = input->is_lossless ? MB_WTH * bit_depth * cpt_num : rm_cfg->budget_mb_bits;
    rm_cfg->max_mb_qp = rm_cfg->is_lossless
                            ? 0
                            : bit_depth - (rm_cfg->budget_mb_bits - 76) /                   /* 76 is alg num */
                                              (MB_WTH * (rm_cfg->pix_format == 0 ? 3 : 4)); /* 3 4 is alg num */
    rm_cfg->budget_mb_bits = rm_cfg->is_lossless ? rm_cfg->budget_mb_bits : CLIP(rm_cfg->budget_mb_bits - delta);
    rm_cfg->budget_mb_bits = MIN(MAX(rm_cfg->budget_mb_bits, mb_safe_bit), mb_ori_bits);

    rm_cfg->min_mb_bits = rm_cfg->budget_mb_bits * 3 / 4; /* 3 4 is alg num */
    rm_cfg->budget_mb_bits_last = rm_cfg->budget_mb_bits * lastMbWth / MB_WTH;

    ice_v3r2_line_osd_cfg_init_thr(rm_cfg, bit_depth, cpt_num);
}

static hi_void ice_v3r2_line_osd_cfg_init_reg_over_5(ice_v3r2_line_osd_cfg *rm_cfg)
{
    rm_cfg->qp_rge_reg0 = ((rm_cfg->max_mb_qp - 3) << 28) | (3 << 24) | /* 3 28 3 24 is alg num */
                          ((rm_cfg->max_mb_qp - 3) << 20) | (2 << 16) | /* 3 20 2 16 is alg num */
                          ((rm_cfg->max_mb_qp - 3) << 12) | (2 << 8) |  /* 3 12 2 8 is alg num */
                          ((rm_cfg->max_mb_qp - 4) << 4) | 2;           /* 4 4 2 is alg num */
    rm_cfg->qp_rge_reg1 = ((rm_cfg->max_mb_qp - 2) << 28) | (3 << 24) | /* 2 28 3 24 is alg num */
                          ((rm_cfg->max_mb_qp - 2) << 20) | (3 << 16) | /* 2 20 3 16 is alg num */
                          ((rm_cfg->max_mb_qp - 2) << 12) | (3 << 8) |  /* 2 12 3 8 is alg num */
                          ((rm_cfg->max_mb_qp - 3) << 4) | 3;           /* 3 4 3 is alg num */
    rm_cfg->qp_rge_reg2 = (rm_cfg->max_mb_qp << 28) | (4 << 24) |       /* 28 4 24 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 20) | (4 << 16) | /* 1 20 4 16 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 12) | (4 << 8) |  /* 1 12 4 8 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 4) | 4;           /* 1 4 4 is alg num */
}

static hi_void ice_v3r2_line_osd_cfg_init_reg_over_2(ice_v3r2_line_osd_cfg *rm_cfg)
{
    rm_cfg->qp_rge_reg0 = ((rm_cfg->max_mb_qp - 3) << 28) | (0 << 24) | /* 3 28 0 24 is alg num */
                          ((rm_cfg->max_mb_qp - 3) << 20) | (0 << 16) | /* 3 20 0 16 is alg num */
                          ((rm_cfg->max_mb_qp - 3) << 12) | (0 << 8) |  /* 3 12 0 8 is alg num */
                          ((rm_cfg->max_mb_qp - 3) << 4) | 0;           /* 3 4 0 is alg num */
    rm_cfg->qp_rge_reg1 = ((rm_cfg->max_mb_qp - 2) << 28) | (0 << 24) | /* 2 28 0 24 is alg num */
                          ((rm_cfg->max_mb_qp - 2) << 20) | (0 << 16) | /* 2 20 0 16 is alg num */
                          ((rm_cfg->max_mb_qp - 2) << 12) | (0 << 8) |  /* 2 12 0 8 is alg num */
                          ((rm_cfg->max_mb_qp - 3) << 4) | 0;           /* 3 4 0 is alg num */
    rm_cfg->qp_rge_reg2 = (rm_cfg->max_mb_qp << 28) | (1 << 24) |       /* 28 1 24 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 20) | (1 << 16) | /* 1 20 1 16 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 12) | (1 << 8) |  /* 1 12 1 8 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 4) | 1;           /* 1 4 1 is alg num */
}

static hi_void ice_v3r2_line_osd_cfg_init_reg_over_1(ice_v3r2_line_osd_cfg *rm_cfg)
{
    rm_cfg->qp_rge_reg0 = ((rm_cfg->max_mb_qp - 2) << 28) | (0 << 24) | /* 2 28 0 24 is alg num */
                          ((rm_cfg->max_mb_qp - 2) << 20) | (0 << 16) | /* 0 24 2 20 0 16 is alg num */
                          ((rm_cfg->max_mb_qp - 2) << 12) | (0 << 8) |  /* 0 16 2 12 0 8 is alg num */
                          ((rm_cfg->max_mb_qp - 2) << 4) | 0;           /* 0 8 2 4 0 is alg num */
    rm_cfg->qp_rge_reg1 = ((rm_cfg->max_mb_qp - 2) << 28) | (0 << 24) | /* 2 28 0 24 is alg num */
                          ((rm_cfg->max_mb_qp - 2) << 20) | (0 << 16) | /* 2 20 0 16 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 12) | (0 << 8) |  /* 1 12 0 8 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 4) | 0;           /* 1 4 0 is alg num */
    rm_cfg->qp_rge_reg2 = (rm_cfg->max_mb_qp << 28) | (1 << 24) |       /* 28 1 24 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 20) | (1 << 16) | /* 1 20 1 16 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 12) | (1 << 8) |  /* 1 12 1 8 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 4) | 1;           /* 1 4 1 is alg num */
}

static hi_void ice_v3r2_line_osd_cfg_init_reg_over_0(ice_v3r2_line_osd_cfg *rm_cfg)
{
    rm_cfg->qp_rge_reg0 = ((rm_cfg->max_mb_qp - 1) << 28) | (0 << 24) | /* 1 28 0 24 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 20) | (0 << 16) | /* 1 20 0 16 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 12) | (0 << 8) |  /* 1 12 0 8 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 4) | 0;           /* 1 4 0 is alg num */
    rm_cfg->qp_rge_reg1 = ((rm_cfg->max_mb_qp - 1) << 28) | (0 << 24) | /* 1 28 0 24 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 20) | (0 << 16) | /* 1 20 0 16 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 12) | (0 << 8) |  /* 1 12 0 8 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 4) | 0;           /* 1 4 0 is alg num */
    rm_cfg->qp_rge_reg2 = (rm_cfg->max_mb_qp << 28) | (1 << 24) |       /* 28 1 24 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 20) | (1 << 16) | /* 1 20 1 16 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 12) | (1 << 8) |  /* 1 12 1 8 is alg num */
                          ((rm_cfg->max_mb_qp - 1) << 4) | 1;           /* 1 4 1 is alg num */
}

static hi_void ice_v3r2_line_osd_cfg_init_reg(ice_v3r2_line_osd_cfg *rm_cfg)
{
    if (rm_cfg->max_mb_qp > 5) { /* 5 is alg num */
        ice_v3r2_line_osd_cfg_init_reg_over_5(rm_cfg);
    } else if (rm_cfg->max_mb_qp > 2) { /* 2 is alg num */
        ice_v3r2_line_osd_cfg_init_reg_over_2(rm_cfg);
    } else if (rm_cfg->max_mb_qp > 1) { /* 1 is alg num */
        ice_v3r2_line_osd_cfg_init_reg_over_1(rm_cfg);
    } else if (rm_cfg->max_mb_qp > 0) {
        ice_v3r2_line_osd_cfg_init_reg_over_0(rm_cfg);
    } else {
        rm_cfg->qp_rge_reg0 = 0;
        rm_cfg->qp_rge_reg1 = 0;
        rm_cfg->qp_rge_reg2 = 0;
    }
}

hi_void ice_v3r2_line_osd_cfg_init(ice_v3r2_line_osd_cfg *rm_cfg, ice_v3r2_line_osd_input *input)
{
    ice_v3r2_line_osd_cfg_init_base(rm_cfg, input);
    ice_v3r2_line_osd_cfg_init_reg(rm_cfg);
}
