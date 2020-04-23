/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description:
 * Author: pq
 * Create: 2016-01-1
 */

#include <linux/string.h>
#include "pq_mng_csc.h"
#include "pq_hal_comm.h"

static hi_bool g_isogeny_mode = HI_FALSE;
static hi_pq_image_param g_sd_picture_setting = {
    PQ_CSC_VIDEO_DEFAULT, PQ_CSC_VIDEO_DEFAULT, PQ_CSC_VIDEO_DEFAULT, PQ_CSC_VIDEO_DEFAULT, PQ_CSC_VIDEO_DEFAULT
};
static hi_pq_image_param g_hd_picture_setting = {
    PQ_CSC_VIDEO_DEFAULT, PQ_CSC_VIDEO_DEFAULT, PQ_CSC_VIDEO_DEFAULT, PQ_CSC_VIDEO_DEFAULT, PQ_CSC_VIDEO_DEFAULT
};
static csc_color_temp g_color_temp[HI_PQ_DISPLAY_MAX] = {
    { PQ_CSC_PICTURE_DEFAULT, PQ_CSC_PICTURE_DEFAULT, PQ_CSC_PICTURE_DEFAULT },
    { PQ_CSC_PICTURE_DEFAULT, PQ_CSC_PICTURE_DEFAULT, PQ_CSC_PICTURE_DEFAULT },
};

static color_space g_csc_mode[HI_PQ_CSC_TYPE_MAX] = {
    { OPTM_CS_EXVYCC_601, OPTM_CS_EITU_R_BT_709, HI_FALSE },  // V0
    { OPTM_CS_EXVYCC_601, OPTM_CS_EITU_R_BT_709, HI_FALSE },  // V1
    { OPTM_CS_EXVYCC_601, OPTM_CS_EITU_R_BT_709, HI_FALSE },  // V2
    { OPTM_CS_EXVYCC_601, OPTM_CS_EITU_R_BT_709, HI_FALSE },  // V3
    { OPTM_CS_EXVYCC_601, OPTM_CS_EITU_R_BT_709, HI_FALSE },  // V4
    { OPTM_CS_MAX,        OPTM_CS_MAX,           HI_FALSE }                    // default
};

static const hi_s32 g_sin_table[61] = { /* 61 :a number */
    -500, -485, -469, -454, -438, -422, -407, -391,
    -374, -358, -342, -325, -309, -292, -276, -259,
    -242, -225, -208, -191, -174, -156, -139, -122,
    -104, -87, -70, -52, -35, -17, 0, 17,
    35, 52, 70, 87, 104, 122, 139, 156,
    174, 191, 208, 225, 242, 259, 276, 292,
    309, 325, 342, 358, 374, 391, 407, 422,
    438, 454, 469, 485, 500
};

static const hi_u32 g_cos_table[61] = { /* 61 :a number */
    866, 875, 883, 891, 899, 906, 914, 921,
    927, 934, 940, 946, 951, 956, 961, 966,
    970, 974, 978, 982, 985, 988, 990, 993,
    995, 996, 998, 999, 999, 1000, 1000, 1000,
    999, 999, 998, 996, 995, 993, 990, 988,
    985, 982, 978, 974, 970, 966, 961, 956,
    951, 946, 940, 934, 927, 921, 914, 906,
    899, 891, 883, 875, 866
};

/**************************************************
CSC matrix for [y_cb_cr]->[y_cb_cr]
**************************************************/
/* SMPTE 170M (i.e. modern SD NTSC) -> BT.709 (i.e. HD) */
hi_drv_pq_csc_ac_coef g_csc_ntsc_sdyuv_hdyuv = {
    1024, -88, -204,
    0, 1032, 114,
    0, 60, 964
    /*
    16384, -1408, -3264,
    0,     16512,  1824,
    0,       960, 15424,
    */
};

/* BT.470-2 system B, G (i.e. SD pal) -> BT.709 (i.e. HD) */
hi_drv_pq_csc_ac_coef g_csc_pal_sdyuv_hdyuv = {
    1024, -116, -192,
    0, 1028, 104,
    0, 88, 1100
    /*
    16384, -1856, -3072,
    0,     16448,  1664,
    0,      1408, 17600
    */
};

/* SMPTE 240M (i.e. 1987 ATSC HD) -> BT.709 (i.e. HD) */
hi_drv_pq_csc_ac_coef g_table_240_mhdyuv_hdyuv = {
    1024, 0, 0,
    0, 1016, 0,
    0, 1, 948
    /*
    16384,  0,    0,
    0,  16256,    0,
    0,     16, 15168
    */
};

/* FCC (i.e. 1953 NTSC) -> BT.709 (i.e. HD) */
hi_drv_pq_csc_ac_coef g_table_fcc_sdyuv_hdyuv = {
    1024, 0, 0,
    56, 1148, -4,
    36, -12, 1148
    /*
    16384,   0,     0,
    896, 18368,   -64,
    576,  -192, 18368
    */
};

/* xv_ycc BT.601 (i.e. SD) -> to BT.709 (i.e. HD) */
hi_drv_pq_csc_ac_coef g_table_xv_ycc_sdyuv_hdyuv = {
    1024, -118, -213,
    0, 1043, 117,
    0, 77, 1050
    /*
    16384, -1892, -3406,
    0,     16689,  1878,
    0,      1229, 16799
    */
};

/* BT.709 (i.e. HD) -> to xv_ycc BT.601 (i.e. SD) */
hi_drv_pq_csc_ac_coef g_table_hdyuv_xvycc_sdyuv = {
    1024, 102, 196,
    0, 1014, -113,
    0, -74, 1007
    /*
    16384, 1627,  3141,
    0,    16217, -1814,
    0,    -1188, 16112
    */
};

/* FCC (i.e. 1953 NTSC) -> to xv_ycc BT.601 (i.e. SD) */
hi_drv_pq_csc_ac_coef g_table_fcc_sdyuv_xvycc_sdyuv = {
    1036, 112, 292,
    52, 1140, -176,
    28, -96, 1444
    /*
    16576, 1792,  4672,
    832,  18240, -2816,
    448,  -1536, 23104
    */
};

/* BT.470-2 system B, G (i.e. SD pal) -> xv_ycc BT.601 (i.e. SD) */
hi_drv_pq_csc_ac_coef g_table_pal_sdyuv_xvycc_sdyuv = {
    1024, 0, 28,
    0, 1004, -20,
    0, 4, 1072
    /*
    16384,  0,   448,
    0,  16064,  -320,
    0,     64, 17152
    */
};

/* SMPTE 170M (i.e. modern SD NTSC)) -> xv_ycc BT.601 (i.e. SD) */
hi_drv_pq_csc_ac_coef g_table_ntsc_sdyuv_xvycc_sdyuv = {
    1024, 24, -8,
    0, 1016, 4,
    0, -20, 940
    /*
    16384, 384, -128,
    0,   16256,   64,
    0,    -320, 15040
    */
};

/* SMPTE 240M (i.e. 1987 ATSC HD) -> xv_ycc BT.601 (i.e. SD) */
hi_drv_pq_csc_ac_coef g_table_240_mhdyuv_xvycc_sdyuv = {
    1024, 104, 180,
    0, 1004, -104,
    0, -68, 932
    /*
    16384, 1664,  2880,
    0,    16064, -1664,
    0,    -1088, 14912
    */
};

/* identity matrix */
hi_drv_pq_csc_ac_coef g_table_identity_yuv_to_yuv = {
    1024, 0, 0,
    0, 1024, 0,
    0, 0, 1024
    /*
    16384,  0,    0,
    0,  16384,    0,
    0,      0, 16384
    */
};

/**************************************************
CSC matrix for [RGB]->[y_cb_cr]
**************************************************/
/* RGB -> YUV709 (i.e. SD) */
hi_drv_pq_csc_ac_coef g_table_rgb_to_hdyuv = {
    188, 629, 63,
    -103, -347, 450,
    450, -409, -41
    /*
    2992,  10063, 1016,
    -1648, -5548, 7196,
    7196,  -6536, -660,
    */
}; /* range[0,255]->range[16,235] */

/* RGB -> YUV709 full (i.e. SD) */
hi_drv_pq_csc_ac_coef g_table_rgb_to_hdyuv_full = {
    218, 732, 74,
    -118, -394, 512,
    512, -465, -47
    /*
    3483,  11718, 1183,
    -1877, -6315, 8192,
    8192,  -7441, -751
    */
}; /* range[0,255]->range[0,255] */

/* RGB -> YUV601 (i.e. SD) */
hi_drv_pq_csc_ac_coef g_table_rgb_to_xvycc_sdyuv = {
    264, 516, 100,
    -152, -298, 450,
    450, -377, -73
    /*
    4211,  8258,  1606,
    -2425, -4768, 7193,
    7193,  -6029, -1163
    */
}; /* range[0,255]->range[16,235] */

/* RGB -> YUV601 full(i.e. SD) */
hi_drv_pq_csc_ac_coef g_table_rgb_to_xvycc_sdyuv_full = {
    306, 601, 117,
    -173, -339, 512,
    512, -429, -83
    /*
    4899,   9617,  1868,
    -2769, -5423,  8192,
    8192,  -6865, -1327
    */
}; /* range[0,255]->range[0,255] */

/**************************************************
CSC matrix for [y_cb_cr]->[RGB]
**************************************************/
/* BT.709 (i.e. HD) -> RGB */
hi_drv_pq_csc_ac_coef g_table_hdyuv_to_rgb = {
    1192, 0, 1836,
    1192, -218, -547,
    1192, 2166, 0
    /*
    19078,     0, 29372,
    19078, -3495, -8729,
    19078, 34610,     0
    */
}; /* range[16,235] -> range[0,255] */

/* BT.709 full(i.e. HD) -> RGB */
hi_drv_pq_csc_ac_coef g_table_hdyuv_to_rgb_full = {
    1024, 0, 1613,
    1024, -191, -479,
    1024, 1901, 0
    /*
    16384,     0, 25805,
    16384, -3064, -7668,
    16384, 30409,     0
    */
}; /* range[0,255]->range[0,255] */

/* xv_ycc BT.601 (i.e. SD) -> RGB */
hi_drv_pq_csc_ac_coef g_table_xvycc_sdyuv_rgb = {
    1192, 0, 1634,
    1192, -400, -833,
    1192, 2066, 0
    /*
    19072,     0,  26144,
    19072, -6400, -13328
    19072, 33056,      0
    */
}; /* range[16,235] -> range[0,255] */

/* xv_ycc BT.601 full(i.e. SD) -> RGB */
hi_drv_pq_csc_ac_coef g_table_xvycc_sdyuv_to_rgb_full = {
    1024, 0, 1436,
    1024, -352, -731,
    1024, 1815, 0
    /*
    16384,     0,  22976,
    16384, -5632, -11696,
    16384, 29040,      0
    */
}; /* range[0,255]->range[0,255] */

/**************************************************
CSC matrix for [RGB]->[RGB]
**************************************************/
/* identity matrix */
hi_drv_pq_csc_ac_coef g_table_identity_rgb_to_rgb = {
    1024, 0, 0,
    0, 1024, 0,
    0, 0, 1024
    /*
    16384,  0,    0,
    0,  16384,    0,
    0,      0, 16384
    */
};

/* the compositor color matrices table WITH color primaries matching */
static hi_drv_pq_csc_ac_coef *g_csc_yuv_matrix_tbl[OPTM_CS_MAX][2] = { /* 2 :a number */
    /* optm_cs_e_unknown */
    {
        &g_table_identity_yuv_to_yuv, /* identity */
        &g_table_identity_yuv_to_yuv  /* identity */
    },
    /* optm_cs_e_itu_r_bt_709 */
    {
        &g_table_identity_yuv_to_yuv, /* HD -> HD, xv_ycc_hd */
        &g_table_hdyuv_xvycc_sdyuv  /* HD -> xv_ycc_sd */
    },
    /* optm_cs_e_unknown */
    {
        &g_table_identity_yuv_to_yuv, /* identity */
        &g_table_identity_yuv_to_yuv  /* identity */
    },
    /* forbidden 3 */
    {
        NULL, NULL
    },
    /* optm_cs_e_fcc  */
    {
        &g_table_fcc_sdyuv_hdyuv,     /* FCC -> HD, xv_ycc_hd */
        &g_table_fcc_sdyuv_xvycc_sdyuv /* FCC -> xv_ycc_sd */
    },
    /* optm_cs_e_itu_r_bt_470_2_bg  */
    {
        &g_csc_pal_sdyuv_hdyuv,       /* PAL -> HD, xv_ycc_hd */
        &g_table_pal_sdyuv_xvycc_sdyuv /* PAL -> xv_ycc_sd */
    },
    /* optm_cs_e_smpte_170_m  */
    {
        &g_csc_ntsc_sdyuv_hdyuv,       /* NTSC -> HD, xv_ycc_hd */
        &g_table_ntsc_sdyuv_xvycc_sdyuv /* NTSC -> xv_ycc_sd */
    },
    /* optm_cs_e_smpte_240_m  */
    {
        &g_table_240_mhdyuv_hdyuv,     /* 240M -> HD, xv_ycc_hd */
        &g_table_240_mhdyuv_xvycc_sdyuv /* 240M -> xv_ycc_sd */
    },
    /* optm_cs_e_xv_ycc_601 */
    {
        &g_table_identity_yuv_to_yuv,
        &g_table_identity_yuv_to_yuv /* xv_ycc_sd -> xv_ycc_sd */
    },
    /* optm_cs_e_rgb  */
    {
        &g_table_identity_rgb_to_rgb, /* RGB->RGB */
        &g_table_identity_rgb_to_rgb  /* RGB->RGB */
    }
};

/* the compositor y_cb_cr to RGB color matrices table */
static hi_drv_pq_csc_ac_coef *g_csc_yuv2rgb_prim_matrix_tbl[][9] = { /* 9 :a number */
    /* LIMIT */
    {
        NULL,                    /* bvdc_p_matrix_coefficients_e_hdmi_rgb  */
        &g_table_hdyuv_to_rgb,      /* bvdc_p_matrix_coefficients_e_itu_r_bt_709 */
        NULL,                    /* bvdc_p_matrix_coefficients_e_unknown  2 */
        NULL,                    /* forbidden 3 */
        NULL,                    /* bvdc_p_matrix_coefficients_e_fcc  */
        NULL,                    /* bvdc_p_matrix_coefficients_e_itu_r_bt_470_2_bg  5 */
        NULL,                    /* bvdc_p_matrix_coefficients_e_smpte_170_m  6 */
        NULL,                    /* bvdc_p_matrix_coefficients_e_smpte_240_m 7 */
        &g_table_xvycc_sdyuv_rgb, /* bvdc_p_matrix_coefficients_e_xv_ycc_601  8 */
    },
    /* FULL */
    {
        NULL,                                       /* bvdc_p_matrix_coefficients_e_hdmi_rgb  0 */
        &g_table_hdyuv_to_rgb_full, /* bvdc_p_matrix_coefficients_e_itu_r_bt_709 */
        NULL,                                       /* bvdc_p_matrix_coefficients_e_unknown  2 */
        NULL,                                       /* forbidden 3 */
        NULL,                                       /* bvdc_p_matrix_coefficients_e_fcc  4 */
        NULL,                                       /* bvdc_p_matrix_coefficients_e_itu_r_bt_470_2_bg  5 */
        NULL,                                       /* bvdc_p_matrix_coefficients_e_smpte_170_m  6 */
        NULL,                                       /* bvdc_p_matrix_coefficients_e_smpte_240_m  7 */
        &g_table_xvycc_sdyuv_to_rgb_full,               /* bvdc_p_matrix_coefficients_e_xv_ycc_601  8 */
    },
};

/* the compositor RGB to y_cb_cr color matrices table */
static hi_drv_pq_csc_ac_coef *g_csc_rgb_prim_to_yuv_matrix_tbl[][2] = { /* 2 :a number */
    /* LIMIT */
    {
        &g_table_rgb_to_hdyuv,      /* HD,limit */
        &g_table_rgb_to_xvycc_sdyuv, /* SD,limit */
    },
    /* FULL */
    {
        &g_table_rgb_to_hdyuv_full,      /* HD, full */
        &g_table_rgb_to_xvycc_sdyuv_full, /* SD, full */
    }
};

/* 计算色彩空间转换系数 for RGB->RGB */
static hi_void pq_mng_cal_csccoef_tprt_rgb2rgb(
    hi_pq_image_param *image_para, csc_color_temp *color_temp,
    hi_drv_pq_csc_ac_coef *csc_table, hi_drv_pq_csc_ac_coef *dst_csc)
{
    hi_s32 contrast_adjust;
    hi_s32 saturation_adjust;
    hi_s32 r_gain, g_gain, gain;
    hi_u32 hue;
    /* 0-255 */
    contrast_adjust = image_para->contrast * 40 / 51; /* 40 51 :a number */
    saturation_adjust = image_para->saturation * 40 / 51; /* 40 51 :a number */
    hue = image_para->hue * 60 / 255; /* 60 255 :a number */
    r_gain = color_temp->red_gain * 40 / 51; /* 40/51 : gain ratio */
    g_gain = color_temp->green_gain * 40 / 51; /* 40/51 : gain ratio */
    gain = color_temp->blue_gain * 40 / 51; /* 40/51 : gain ratio */
    dst_csc->csc_coef00 = (hi_s32)(77 + (hi_s32)(60 * g_sin_table[hue] * saturation_adjust) / 100000 /* 77 :a number 60 :a number 100000 :a number */
                                   + (hi_s32)(179 * g_cos_table[hue] * saturation_adjust) / 100000) /* 179 :a number 100000 :a number */
                          * 4 * contrast_adjust * r_gain / 10000; /* 4 :a number 10000 :a number */
    dst_csc->csc_coef01 = (hi_s32)(150 + (hi_s32)(119 * g_sin_table[hue] * saturation_adjust) / 100000 * 4 /* 150 :a number 119 :a number 100000 :a number 4 :a number */
                                   - (hi_s32)(150 * g_cos_table[hue] * saturation_adjust) / 100000) /* 150 :a number 100000 :a number */
                          * 4 * contrast_adjust * g_gain / 10000; /* 4 :a number 10000 :a number */
    dst_csc->csc_coef02 = (hi_s32)(29 - (hi_s32)(179 * g_sin_table[hue] * saturation_adjust) / 100000 * 4 /* 29 :a number 179 :a number 100000 :a number 4 :a number */
                                   - (hi_s32)(29 * g_cos_table[hue] * saturation_adjust) / 100000) /* 29 :a number 100000 :a number */
                          * 4 * contrast_adjust * gain / 10000; /* 4 :a number 10000 :a number */
    dst_csc->csc_coef10 = (hi_s32)(77 - (hi_s32)(75 * g_sin_table[hue] * saturation_adjust) / 100000 * 4 /* 77 :a number 75 :a number 100000 :a number 4 :a number */
                                   - (hi_s32)(77 * g_cos_table[hue] * saturation_adjust) / 100000) /* 77 :a number 100000 :a number */
                          * 4 * contrast_adjust * r_gain / 10000; /* 4 :a number 10000 :a number */
    dst_csc->csc_coef11 = (hi_s32)(150 - (hi_s32)(24 * g_sin_table[hue] * saturation_adjust) / 100000 * 4 /* 150 :a number 24 :a number 100000 :a number 4 :a number */
                                   + (hi_s32)(106 * g_cos_table[hue] * saturation_adjust) / 100000) /* 106 :a number 100000 :a number */
                          * 4 * contrast_adjust * g_gain / 10000; /* 4 :a number 10000 :a number */
    dst_csc->csc_coef12 = (hi_s32)(29 + (hi_s32)(99 * g_sin_table[hue] * saturation_adjust) / 100000 * 4 /* 29 :a number 99 :a number 100000 :a number 4 :a number */
                                   - (hi_s32)(29 * g_cos_table[hue] * saturation_adjust) / 100000) /* 29 :a number 100000 :a number */
                          * 4 * contrast_adjust * gain / 10000; /* 4 :a number 10000 :a number */
    dst_csc->csc_coef20 = (hi_s32)(77 + (hi_s32)(227 * g_sin_table[hue] * saturation_adjust) / 100000 * 4 /* 77 :a number 227 :a number 100000 :a number 4 :a number */
                                   - (hi_s32)(77 * g_cos_table[hue] * saturation_adjust) / 100000) /* 77 :a number 100000 :a number */
                          * 4 * contrast_adjust * r_gain / 10000; /* 4 :a number 10000 :a number */
    dst_csc->csc_coef21 = (hi_s32)(150 - (hi_s32)(190 * g_sin_table[hue] * saturation_adjust) / 100000 * 4 /* 150 :a number 190 :a number 100000 :a number 4 :a number */
                                   - (hi_s32)(150 * g_cos_table[hue] * saturation_adjust) / 100000) /* 150 :a number 100000 :a number */
                          * 4 * contrast_adjust * g_gain / 10000; /* 4 :a number 10000 :a number */
    dst_csc->csc_coef22 = (hi_s32)(29 - (hi_s32)(37 * g_sin_table[hue] * saturation_adjust) / 100000 * 4 /* 29 :a number 37 :a number 100000 :a number 4 :a number */
                                   + (hi_s32)(227 * g_cos_table[hue] * saturation_adjust) / 100000) /* 227 :a number 100000 :a number */
                          * 4 * contrast_adjust * gain / 10000; /* 4 :a number 10000 :a number */
    dst_csc->csc_coef00 = PQ_CLIP3(dst_csc->csc_coef00, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef01 = PQ_CLIP3(dst_csc->csc_coef01, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef02 = PQ_CLIP3(dst_csc->csc_coef02, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef10 = PQ_CLIP3(dst_csc->csc_coef10, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef11 = PQ_CLIP3(dst_csc->csc_coef11, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef12 = PQ_CLIP3(dst_csc->csc_coef12, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef20 = PQ_CLIP3(dst_csc->csc_coef20, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef21 = PQ_CLIP3(dst_csc->csc_coef21, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef22 = PQ_CLIP3(dst_csc->csc_coef22, -4095, 4095); /* -4095 :a number 4095 :a number */
    return;
}

static hi_void pq_mng_cal_csccoef_tprt_yuv2rgb(
    hi_pq_image_param *image_para, csc_color_temp *color_temp,
    hi_drv_pq_csc_ac_coef *csc_table, hi_drv_pq_csc_ac_coef *dst_csc)
{
    hi_s32 contrast_adjust;
    hi_s32 saturation_adjust;
    hi_s32 r_gain, g_gain, gain;
    hi_u32 hue;
    /* 0-255 */
    contrast_adjust = image_para->contrast * 40 / 51; /* 40 :a number 51 :a number */
    saturation_adjust = image_para->saturation * 40 / 51; /* 40 :a number 51 :a number */
    hue = image_para->hue * 60 / 255; /* 60 :a number 255 :a number */
    r_gain = color_temp->red_gain * 40 / 51; /* 40/51 : gain ratio */
    g_gain = color_temp->green_gain * 40 / 51; /* 40/51 : gain ratio */
    gain = color_temp->blue_gain * 40 / 51; /* 40/51 : gain ratio */
    dst_csc->csc_coef00 = csc_table->csc_coef00 * contrast_adjust * r_gain / 10000; /* 10000 :a number */
    dst_csc->csc_coef01 = (hi_s32)(((hi_s32)(csc_table->csc_coef01 * g_cos_table[hue]) / 1000 /* 1000 :a number */
                                    - ((hi_s32)(csc_table->csc_coef02 * g_sin_table[hue])) * saturation_adjust / 100000) /* 100000 :a number */
                                   * contrast_adjust * r_gain) / 10000; /* 10000 :a number */
    dst_csc->csc_coef02 = (hi_s32)(((hi_s32)(csc_table->csc_coef01 * g_sin_table[hue]) / 1000 /* 1000 :a number */
                                    + ((hi_s32)(csc_table->csc_coef02 * g_cos_table[hue])) * saturation_adjust / 100000) /* 100000 :a number */
                                   * contrast_adjust * r_gain) / 10000; /* 10000 :a number */
    dst_csc->csc_coef10 = csc_table->csc_coef10 * contrast_adjust * g_gain / 10000; /* 10000 :a number */
    dst_csc->csc_coef11 = (hi_s32)(((hi_s32)(csc_table->csc_coef11 * g_cos_table[hue]) / 1000 /* 1000 :a number */
                                    - (hi_s32)(csc_table->csc_coef12 * g_sin_table[hue]) / 1000) /* 1000 :a number */
                                   * contrast_adjust * saturation_adjust * g_gain) / 1000000; /* 1000000 :a number */
    dst_csc->csc_coef12 = (hi_s32)(((hi_s32)(csc_table->csc_coef11 * g_sin_table[hue]) / 1000 /* 1000 :a number */
                                    + (hi_s32)(csc_table->csc_coef12 * g_cos_table[hue]) / 1000) /* 1000 :a number */
                                   * contrast_adjust * saturation_adjust * g_gain) / 1000000; /* 1000000 :a number */
    dst_csc->csc_coef20 = csc_table->csc_coef20 * contrast_adjust * gain / 10000; /* 10000 :a number */
    dst_csc->csc_coef21 = (hi_s32)((((hi_s32)(csc_table->csc_coef21 * g_cos_table[hue])) * saturation_adjust /
                                    100000 /* 100000 :a number */
                                    - (hi_s32)(csc_table->csc_coef22 * g_sin_table[hue]) / 1000) * /* 1000 :a number */
                                   contrast_adjust *
                                   gain) /
                          10000; /* 10000 :a number */
    dst_csc->csc_coef22 = (hi_s32)((((hi_s32)(csc_table->csc_coef21 * g_sin_table[hue])) * saturation_adjust /
                                    100000 /* 100000 :a number */
                                    - (hi_s32)(csc_table->csc_coef22 * g_cos_table[hue]) / 1000) * /* 1000 :a number */
                                   contrast_adjust *
                                   gain) /
                          10000; /* 10000 :a number */
    dst_csc->csc_coef00 = PQ_CLIP3(dst_csc->csc_coef00, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef01 = PQ_CLIP3(dst_csc->csc_coef01, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef02 = PQ_CLIP3(dst_csc->csc_coef02, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef10 = PQ_CLIP3(dst_csc->csc_coef10, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef11 = PQ_CLIP3(dst_csc->csc_coef11, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef12 = PQ_CLIP3(dst_csc->csc_coef12, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef20 = PQ_CLIP3(dst_csc->csc_coef20, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef21 = PQ_CLIP3(dst_csc->csc_coef21, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef22 = PQ_CLIP3(dst_csc->csc_coef22, -4095, 4095); /* -4095 :a number 4095 :a number */
    return;
}

/* 计算色彩空间转换系数
   计算色温变换矩阵(for y_cb_cr->y_cb_cr 调节色温)
 */
static hi_void pq_mng_cal_ctemperature_mat(
    csc_color_temp *color_temp, hi_drv_pq_csc_ac_coef *csc_table, hi_drv_pq_csc_ac_coef *dst_csc)
{
    hi_s32 r_gain;
    hi_s32 g_gain;
    hi_s32 gain;
    hi_drv_pq_csc_ac_coef yuv_color_temp;
    /* 0-255 */
    r_gain = color_temp->red_gain * 40 / 51; /* 40/51 : gain ratio */
    g_gain = color_temp->green_gain * 40 / 51; /* 40/51 : gain ratio */
    gain = color_temp->blue_gain * 40 / 51; /* 40/51 : gain ratio */
    /* kr系列精度100, 色温矩阵系数精度1000 */
    yuv_color_temp.csc_coef00 = (114 * gain + 587 * g_gain + 299 * r_gain) / 100; /* 114 :a number 587 :a number 299 :a number 100 :a number */
    yuv_color_temp.csc_coef01 = (232 * gain - 232 * g_gain) / 100; /* 232 :a number 232 :a number 100 :a number */
    yuv_color_temp.csc_coef02 = -(341 * g_gain - 341 * r_gain) / 100; /* 341 :a number 341 :a number 100 :a number */
    yuv_color_temp.csc_coef10 = -(289 * g_gain - 436 * gain + 147 * r_gain) / 100; /* 289 :a number 436 :a number 147 :a number 100 :a number */
    yuv_color_temp.csc_coef11 = (886 * gain + 114 * g_gain) / 100; /* 886 :a number 114 :a number 100 :a number */
    yuv_color_temp.csc_coef12 = (168 * g_gain - 168 * r_gain) / 100; /* 168 :a number 168 :a number 100 :a number */
    yuv_color_temp.csc_coef20 = -(100 * gain + 515 * g_gain - 615 * r_gain) / 100; /* 100 :a number 515 :a number 615 :a number 100 :a number */
    yuv_color_temp.csc_coef21 = -(203 * gain - 203 * g_gain) / 100; /* 203 :a number 203 :a number 100 :a number */
    yuv_color_temp.csc_coef22 = (299 * g_gain + 701 * r_gain) / 100; /* 299 :a number 701 :a number 100 :a number */
    dst_csc->csc_coef00 = (csc_table->csc_coef00 * yuv_color_temp.csc_coef00 +
                           csc_table->csc_coef01 * yuv_color_temp.csc_coef10 +
                           csc_table->csc_coef02 * yuv_color_temp.csc_coef20) /
                          1000; /* 1000 :a number */
    dst_csc->csc_coef01 = (csc_table->csc_coef00 * yuv_color_temp.csc_coef01 +
                           csc_table->csc_coef01 * yuv_color_temp.csc_coef11 +
                           csc_table->csc_coef02 * yuv_color_temp.csc_coef21) /
                          1000; /* 1000 :a number */
    dst_csc->csc_coef02 = (csc_table->csc_coef00 * yuv_color_temp.csc_coef02 +
                           csc_table->csc_coef01 * yuv_color_temp.csc_coef12 +
                           csc_table->csc_coef02 * yuv_color_temp.csc_coef22) /
                          1000; /* 1000 :a number */
    dst_csc->csc_coef10 = (csc_table->csc_coef10 * yuv_color_temp.csc_coef00 +
                           csc_table->csc_coef11 * yuv_color_temp.csc_coef10 +
                           csc_table->csc_coef12 * yuv_color_temp.csc_coef20) /
                          1000; /* 1000 :a number */
    dst_csc->csc_coef11 = (csc_table->csc_coef10 * yuv_color_temp.csc_coef01 +
                           csc_table->csc_coef11 * yuv_color_temp.csc_coef11 +
                           csc_table->csc_coef12 * yuv_color_temp.csc_coef21) /
                          1000; /* 1000 :a number */
    dst_csc->csc_coef12 = (csc_table->csc_coef10 * yuv_color_temp.csc_coef02 +
                           csc_table->csc_coef11 * yuv_color_temp.csc_coef12 +
                           csc_table->csc_coef12 * yuv_color_temp.csc_coef22) /
                          1000; /* 1000 :a number */
    dst_csc->csc_coef20 = (csc_table->csc_coef20 * yuv_color_temp.csc_coef00 +
                           csc_table->csc_coef21 * yuv_color_temp.csc_coef10 +
                           csc_table->csc_coef22 * yuv_color_temp.csc_coef20) /
                          1000; /* 1000 :a number */
    dst_csc->csc_coef21 = (csc_table->csc_coef20 * yuv_color_temp.csc_coef01 +
                           csc_table->csc_coef21 * yuv_color_temp.csc_coef11 +
                           csc_table->csc_coef22 * yuv_color_temp.csc_coef21) /
                          1000; /* 1000 :a number */
    dst_csc->csc_coef22 = (csc_table->csc_coef20 * yuv_color_temp.csc_coef02 +
                           csc_table->csc_coef21 * yuv_color_temp.csc_coef12 +
                           csc_table->csc_coef22 * yuv_color_temp.csc_coef22) /
                          1000; /* 1000 :a number */
    return;
}

/* 计算色彩空间转换系数 for y_cb_cr->y_cb_cr */
static hi_void pq_mng_cal_csccoef_tprt_yuv2yuv(
    hi_pq_image_param *image_para, csc_color_temp *color_temp,
    hi_drv_pq_csc_ac_coef *csc_table, hi_drv_pq_csc_ac_coef *dst_csc)
{
    hi_s32 chroma_adjust;
    hi_s32 saturation_adjust;
    hi_drv_pq_csc_ac_coef dst_csc_table2;
    hi_u32 hue;
    /* 0-255 */
    chroma_adjust = image_para->contrast * 40 / 51; /* 40 :a number 51 :a number */
    saturation_adjust = image_para->saturation * 40 / 51; /* 40 :a number 51 :a number */
    hue = image_para->hue * 60 / 255; /* 60 :a number 255 :a number */
    dst_csc->csc_coef00 = csc_table->csc_coef00 * chroma_adjust / 100; /* 100 :a number */
    dst_csc->csc_coef01 = csc_table->csc_coef01 * chroma_adjust / 100; /* 100 :a number */
    dst_csc->csc_coef02 = csc_table->csc_coef02 * chroma_adjust / 100; /* 100 :a number */
    dst_csc->csc_coef10 = (hi_s32)(((hi_s32)(csc_table->csc_coef10 * g_cos_table[hue]) / 1000 /* 1000 :a number */
                                    + (hi_s32)(csc_table->csc_coef20 * g_sin_table[hue]) / 1000) /* 1000 :a number */
                                   * chroma_adjust * saturation_adjust) / 10000; /* 10000 :a number */
    dst_csc->csc_coef11 = (hi_s32)(((hi_s32)(csc_table->csc_coef11 * g_cos_table[hue]) / 1000 /* 1000 :a number */
                                    + (hi_s32)(csc_table->csc_coef21 * g_sin_table[hue]) / 1000) /* 1000 :a number */
                                   * chroma_adjust * saturation_adjust) / 10000; /* 10000 :a number */
    dst_csc->csc_coef12 = (hi_s32)(((hi_s32)(csc_table->csc_coef12 * g_cos_table[hue]) / 1000 /* 1000 :a number */
                                    + (hi_s32)(csc_table->csc_coef22 * g_sin_table[hue]) / 1000) /* 1000 :a number */
                                   * chroma_adjust * saturation_adjust) / 10000; /* 10000 :a number */
    dst_csc->csc_coef20 = (hi_s32)(((hi_s32)(csc_table->csc_coef20 * g_cos_table[hue]) / 1000 /* 1000 :a number */
                                    - (hi_s32)(csc_table->csc_coef10 * g_sin_table[hue]) / 1000) /* 1000 :a number */
                                   * chroma_adjust * saturation_adjust) / 10000; /* 10000 :a number */
    dst_csc->csc_coef21 = (hi_s32)(((hi_s32)(csc_table->csc_coef21 * g_cos_table[hue]) / 1000 /* 1000 :a number */
                                    - (hi_s32)(csc_table->csc_coef11 * g_sin_table[hue]) / 1000) /* 1000 :a number */
                                   * chroma_adjust * saturation_adjust) / 10000; /* 10000 :a number */
    dst_csc->csc_coef22 = (hi_s32)(((hi_s32)(csc_table->csc_coef22 * g_cos_table[hue]) / 1000 /* 1000 :a number */
                                    - (hi_s32)(csc_table->csc_coef12 * g_sin_table[hue]) / 1000) /* 1000 :a number */
                                   * chroma_adjust * saturation_adjust) / 10000; /* 10000 :a number */
    pq_mng_cal_ctemperature_mat(color_temp, dst_csc, &dst_csc_table2);
    dst_csc->csc_coef00 = PQ_CLIP3(dst_csc_table2.csc_coef00, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef01 = PQ_CLIP3(dst_csc_table2.csc_coef01, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef02 = PQ_CLIP3(dst_csc_table2.csc_coef02, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef10 = PQ_CLIP3(dst_csc_table2.csc_coef10, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef11 = PQ_CLIP3(dst_csc_table2.csc_coef11, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef12 = PQ_CLIP3(dst_csc_table2.csc_coef12, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef20 = PQ_CLIP3(dst_csc_table2.csc_coef20, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef21 = PQ_CLIP3(dst_csc_table2.csc_coef21, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef22 = PQ_CLIP3(dst_csc_table2.csc_coef22, -4095, 4095); /* -4095 :a number 4095 :a number */
    return;
}

/* 计算色彩空间转换系数 for RGB->y_cb_cr */
static hi_void pq_mng_cal_csccoef_tprt_rgb2yuv(
    hi_pq_image_param *image_para, csc_color_temp *color_temp,
    hi_drv_pq_csc_ac_coef *csc_table, hi_drv_pq_csc_ac_coef *dst_csc)
{
    hi_s32 contrast_adjust;
    hi_s32 saturation_adjust;
    hi_s32 r_gain, g_gain, gain;
    hi_u32 hue;
    /* 0-255 */
    contrast_adjust = image_para->contrast * 40 / 51; /* 40 :a number 51 :a number */
    saturation_adjust = image_para->saturation * 40 / 51; /* 40 :a number 51 :a number */
    hue = image_para->hue * 60 / 255; /* 60 :a number 255 :a number */
    r_gain = color_temp->red_gain * 40 / 51; /* 40/51 : gain ratio */
    g_gain = color_temp->green_gain * 40 / 51; /* 40/51 : gain ratio */
    gain = color_temp->blue_gain * 40 / 51; /* 40/51 : gain ratio */
    dst_csc->csc_coef00 = csc_table->csc_coef00 * contrast_adjust * r_gain / 10000; /* 10000 :a number */
    dst_csc->csc_coef01 = csc_table->csc_coef01 * contrast_adjust * g_gain / 10000; /* 10000 :a number */
    dst_csc->csc_coef02 = csc_table->csc_coef02 * contrast_adjust * gain / 10000; /* 10000 :a number */
    dst_csc->csc_coef10 = (hi_s32)(((hi_s32)(csc_table->csc_coef10 * g_cos_table[hue]) / 1000 /* 1000 :a number */
                                    + (hi_s32)(csc_table->csc_coef20 * g_sin_table[hue]) / 1000) /* 1000 :a number */
                                   * contrast_adjust * saturation_adjust * r_gain) / 1000000; /* 1000000 :a number */
    dst_csc->csc_coef11 = (hi_s32)(((hi_s32)(csc_table->csc_coef11 * g_cos_table[hue]) / 1000 /* 1000 :a number */
                                    + (hi_s32)(csc_table->csc_coef21 * g_sin_table[hue]) / 1000) /* 1000 :a number */
                                   * contrast_adjust * saturation_adjust * g_gain) / 1000000; /* 1000000 :a number */
    dst_csc->csc_coef12 = (hi_s32)(((hi_s32)(csc_table->csc_coef12 * g_cos_table[hue]) / 1000 /* 1000 :a number */
                                    + (hi_s32)(csc_table->csc_coef22 * g_sin_table[hue]) / 1000) /* 1000 :a number */
                                   * contrast_adjust * saturation_adjust * gain) / 1000000; /* 1000000 :a number */
    dst_csc->csc_coef20 = (hi_s32)(((hi_s32)(csc_table->csc_coef20 * g_cos_table[hue]) / 1000 /* 1000 :a number */
                                    - (hi_s32)(csc_table->csc_coef10 * g_sin_table[hue]) / 1000) /* 1000 :a number */
                                   * contrast_adjust * saturation_adjust * r_gain) / 1000000; /* 1000000 :a number */
    dst_csc->csc_coef21 = (hi_s32)(((hi_s32)(csc_table->csc_coef21 * g_cos_table[hue]) / 1000 /* 1000 :a number */
                                    - (hi_s32)(csc_table->csc_coef11 * g_sin_table[hue]) / 1000) /* 1000 :a number */
                                   * contrast_adjust * saturation_adjust * g_gain) / 1000000; /* 1000000 :a number */
    dst_csc->csc_coef22 = (hi_s32)(((hi_s32)(csc_table->csc_coef22 * g_cos_table[hue]) / 1000 /* 1000 :a number */
                                    - (hi_s32)(csc_table->csc_coef12 * g_sin_table[hue]) / 1000) /* 1000 :a number */
                                   * contrast_adjust * saturation_adjust * gain) / 1000000; /* 1000000 :a number */
    dst_csc->csc_coef00 = PQ_CLIP3(dst_csc->csc_coef00, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef01 = PQ_CLIP3(dst_csc->csc_coef01, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef02 = PQ_CLIP3(dst_csc->csc_coef02, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef10 = PQ_CLIP3(dst_csc->csc_coef10, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef11 = PQ_CLIP3(dst_csc->csc_coef11, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef12 = PQ_CLIP3(dst_csc->csc_coef12, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef20 = PQ_CLIP3(dst_csc->csc_coef20, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef21 = PQ_CLIP3(dst_csc->csc_coef21, -4095, 4095); /* -4095 :a number 4095 :a number */
    dst_csc->csc_coef22 = PQ_CLIP3(dst_csc->csc_coef22, -4095, 4095); /* -4095 :a number 4095 :a number */
    return;
}

/* 计算直流分量 */
/* note: 这部分的计算, 精度还是按照原来的10bits配置, 可以在运算的最后乘上精度 */
static hi_void pq_mng_calc_csc_dc_coef(csc_para *csc_parameter, hi_drv_pq_csc_dc_coef *csc_dc_coef)
{
    hi_s32 brightness = (hi_s32)csc_parameter->image_para.brightness * 100 / 255 - 50; /* 100 :a number 255 :a number 50 :a number */

    if (csc_parameter->in_color_type == OPTM_CS_ERGB) {      /* input RGB color space */
        if (csc_parameter->out_color_type == OPTM_CS_ERGB) { /* RGB -> RGB */
            csc_dc_coef->csc_in_dc0 = 0;
            csc_dc_coef->csc_in_dc1 = 0;
            csc_dc_coef->csc_in_dc2 = 0;
            csc_dc_coef->csc_out_dc0 = (brightness * 4 * 298 + 128) * 4 / 256 /* 4 :a number 298 :a number 128 :a number 4 :a number 256 :a number */
                                       + (csc_parameter->color_lut_temp_offset[COLORTEMP_R]) - 128; /* 128 :a number */
            csc_dc_coef->csc_out_dc1 = (brightness * 4 * 298 + 128) * 4 / 256 /* 4 :a number 298 :a number 128 :a number 4 :a number 256 :a number */
                                       + (csc_parameter->color_lut_temp_offset[COLORTEMP_G]) - 128; /* 128 :a number */
            csc_dc_coef->csc_out_dc2 = (brightness * 4 * 298 + 128) * 4 / 256 /* 4 :a number 298 :a number 128 :a number 4 :a number 256 :a number */
                                       + (csc_parameter->color_lut_temp_offset[COLORTEMP_B]) - 128; /* 128 :a number */
        } else { /* RGB -> y_cb_cr */
            csc_dc_coef->csc_in_dc0 = 0;
            csc_dc_coef->csc_in_dc1 = 0;
            csc_dc_coef->csc_in_dc2 = 0;
            csc_dc_coef->csc_out_dc0 = brightness * 4 + 64; /* 4 :a number 64 :a number */
            csc_dc_coef->csc_out_dc1 = 512; /* 512 :a number */
            csc_dc_coef->csc_out_dc2 = 512; /* 512 :a number */
        }
    } else {   /* input y_cb_cr color space */
        if (csc_parameter->out_color_type == OPTM_CS_ERGB) { /* y_cb_cr -> RGB */
            csc_dc_coef->csc_in_dc0 = -64 + brightness * 4; /* -64 :a number 4 :a number */
            csc_dc_coef->csc_in_dc1 = -512; /* -512 :a number */
            csc_dc_coef->csc_in_dc2 = -512; /* -512 :a number */
            csc_dc_coef->csc_out_dc0 = -128 + (csc_parameter->color_lut_temp_offset[COLORTEMP_R]);  // 0 /* -128 :a number */
            csc_dc_coef->csc_out_dc1 = -128 + (csc_parameter->color_lut_temp_offset[COLORTEMP_G]);  // 0 /* -128 :a number */
            csc_dc_coef->csc_out_dc2 = -128 + (csc_parameter->color_lut_temp_offset[COLORTEMP_B]);  // 0 /* -128 :a number */
        } else {   /* y_cb_cr -> y_cb_cr */
            csc_dc_coef->csc_in_dc0 = -64; /* -64 :a number */
            csc_dc_coef->csc_in_dc1 = -512; /* -512 :a number */
            csc_dc_coef->csc_in_dc2 = -512; /* -512 :a number */
            csc_dc_coef->csc_out_dc0 = brightness * 4 + 64; /* 4 :a number 64 :a number */
            csc_dc_coef->csc_out_dc1 = 512; /* 512 :a number */
            csc_dc_coef->csc_out_dc2 = 512; /* 512 :a number */
        }
    }

    return;
}

/* 10 bit precision */
static hi_s32 pq_mng_calc_csccoef(csc_para *csc_parameter, hi_drv_pq_csc_ac_coef *csc_coef)
{
    hi_drv_pq_csc_ac_coef *csc_matrix = HI_NULL;
    hi_drv_pq_csc_ac_coef csc_matrix_tmp;
    PQ_CHECK_NULL_PTR_RE_FAIL(csc_parameter);
    PQ_CHECK_NULL_PTR_RE_FAIL(csc_coef);

    if (csc_parameter->in_color_type >= OPTM_CS_MAX) {
        return HI_FAILURE;
    }

    if (csc_parameter->in_color_type == OPTM_CS_ERGB) {      /* input RGB color space */
        if (csc_parameter->out_color_type == OPTM_CS_ERGB) { /* RGB -> RGB */
            csc_matrix = &g_table_identity_rgb_to_rgb;       /* identity */
            PQ_CHECK_NULL_PTR_RE_FAIL(csc_matrix);
            pq_mng_cal_csccoef_tprt_rgb2rgb(&csc_parameter->image_para,
                                            &csc_parameter->color_temp,
                                            csc_matrix, &csc_matrix_tmp);
        } else { /* RGB -> y_cb_cr */
            if (OPTM_CS_EXVYCC_709 == csc_parameter->out_color_type) {
                /* R2Y_709; full or limited */
                csc_matrix = g_csc_rgb_prim_to_yuv_matrix_tbl[csc_parameter->full_range][0];
            } else {
                /* R2Y_601; full or limited */
                csc_matrix = g_csc_rgb_prim_to_yuv_matrix_tbl[csc_parameter->full_range][1];
            }

            PQ_CHECK_NULL_PTR_RE_FAIL(csc_matrix);
            pq_mng_cal_csccoef_tprt_rgb2yuv(&csc_parameter->image_para,
                                            &csc_parameter->color_temp,
                                            csc_matrix, &csc_matrix_tmp);
        }
    } else {   /* input y_cb_cr color space */
        if (csc_parameter->out_color_type == OPTM_CS_ERGB) { /* y_cb_cr -> RGB */
            if (OPTM_CS_EXVYCC_709 == csc_parameter->in_color_type) {
                /* Y709 ->RGB; full or limited */
                csc_matrix = g_csc_yuv2rgb_prim_matrix_tbl[csc_parameter->full_range][1];
            } else {
                /* Y_601 ->RGB; full or limited */
                csc_matrix = g_csc_yuv2rgb_prim_matrix_tbl[csc_parameter->full_range][8]; /* 8 :a number */
            }

            PQ_CHECK_NULL_PTR_RE_FAIL(csc_matrix);
            pq_mng_cal_csccoef_tprt_yuv2rgb(&csc_parameter->image_para,
                                            &csc_parameter->color_temp,
                                            csc_matrix, &csc_matrix_tmp);
        } else { /* y_cb_cr -> y_cb_cr */
            if (OPTM_CS_EXVYCC_709 == csc_parameter->out_color_type) {
                csc_matrix = g_csc_yuv_matrix_tbl[csc_parameter->in_color_type][0];
            } else {
                csc_matrix = g_csc_yuv_matrix_tbl[csc_parameter->in_color_type][1];
            }

            PQ_CHECK_NULL_PTR_RE_FAIL(csc_matrix);
            pq_mng_cal_csccoef_tprt_yuv2yuv(&csc_parameter->image_para,
                                            &csc_parameter->color_temp,
                                            csc_matrix, &csc_matrix_tmp);
        }
    }

    csc_coef->csc_coef00 = csc_matrix_tmp.csc_coef00;
    csc_coef->csc_coef01 = csc_matrix_tmp.csc_coef01;
    csc_coef->csc_coef02 = csc_matrix_tmp.csc_coef02;
    csc_coef->csc_coef10 = csc_matrix_tmp.csc_coef10;
    csc_coef->csc_coef11 = csc_matrix_tmp.csc_coef11;
    csc_coef->csc_coef12 = csc_matrix_tmp.csc_coef12;
    csc_coef->csc_coef20 = csc_matrix_tmp.csc_coef20;
    csc_coef->csc_coef21 = csc_matrix_tmp.csc_coef21;
    csc_coef->csc_coef22 = csc_matrix_tmp.csc_coef22;
    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_in_csc_trans(hi_drv_pq_csc_type csc_type,
                                      drv_pq_internal_color_space csc_src, drv_pq_internal_color_space *csc_dst)
{
    switch (csc_src) {
        case HI_DRV_CS_BT709_YUV_LIMITED:
        case HI_DRV_CS_BT709_YUV_FULL:
            g_csc_mode[csc_type].input_color_space = OPTM_CS_EITU_R_BT_709;
            *csc_dst = HI_DRV_CS_BT709_YUV_LIMITED;
            break;

        case HI_DRV_CS_BT601_YUV_LIMITED:
        case HI_DRV_CS_BT601_YUV_FULL:
            g_csc_mode[csc_type].input_color_space = OPTM_CS_EXVYCC_601;
            *csc_dst = HI_DRV_CS_BT601_YUV_LIMITED;
            break;

        case HI_DRV_CS_BT601_RGB_FULL:
        case HI_DRV_CS_BT601_RGB_LIMITED:
            g_csc_mode[csc_type].input_color_space = OPTM_CS_ERGB;
            *csc_dst = HI_DRV_CS_BT601_RGB_FULL;
            break;

        case HI_DRV_CS_BT709_RGB_LIMITED:
        case HI_DRV_CS_BT709_RGB_FULL:
            g_csc_mode[csc_type].input_color_space = OPTM_CS_ERGB;
            *csc_dst = HI_DRV_CS_BT709_RGB_LIMITED;
            break;

        default:
            g_csc_mode[csc_type].input_color_space = OPTM_CS_EUNKNOWN;
            *csc_dst = HI_DRV_CS_MAX;
            HI_ERR_PQ("csc_src = %d is invalid.\n", csc_src);
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_mng_set_out_csc_trans(hi_drv_pq_csc_type csc_type,
                                       drv_pq_internal_color_space csc_src, drv_pq_internal_color_space *csc_dst)
{
    switch (csc_src) {
        case HI_DRV_CS_BT709_YUV_LIMITED:
        case HI_DRV_CS_BT709_YUV_FULL:
            g_csc_mode[csc_type].output_color_space = OPTM_CS_EITU_R_BT_709;
            *csc_dst = HI_DRV_CS_BT709_YUV_LIMITED;
            break;

        case HI_DRV_CS_BT601_YUV_LIMITED:
        case HI_DRV_CS_BT601_YUV_FULL:
            g_csc_mode[csc_type].output_color_space = OPTM_CS_EXVYCC_601;
            *csc_dst = HI_DRV_CS_BT601_YUV_LIMITED;
            break;

        case HI_DRV_CS_BT601_RGB_FULL:
        case HI_DRV_CS_BT601_RGB_LIMITED:
            g_csc_mode[csc_type].output_color_space = OPTM_CS_ERGB;
            *csc_dst = HI_DRV_CS_BT601_RGB_FULL;
            break;

        case HI_DRV_CS_BT709_RGB_LIMITED:
        case HI_DRV_CS_BT709_RGB_FULL:
            g_csc_mode[csc_type].output_color_space = OPTM_CS_ERGB;
            *csc_dst = HI_DRV_CS_BT709_RGB_LIMITED;
            break;

        default:
            g_csc_mode[csc_type].output_color_space = OPTM_CS_EUNKNOWN;
            *csc_dst = HI_DRV_CS_MAX;
            HI_ERR_PQ("csc_src = %d is invalid.\n", csc_src);
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void get_vid_csc_type(drv_pq_internal_color_space cs_drv, pq_csc *cs_alg)
{
    switch (cs_drv) {
        case HI_DRV_CS_BT709_YUV_LIMITED:
            cs_alg->cs_type = OPTM_CS_EITU_R_BT_709;
            cs_alg->cs_range = PQ_CSC_RANGE_LMTD;
            break;

        case HI_DRV_CS_BT709_YUV_FULL:
            cs_alg->cs_type = OPTM_CS_EITU_R_BT_709;
            cs_alg->cs_range = PQ_CSC_RANGE_FULL;
            break;

        case HI_DRV_CS_BT601_YUV_LIMITED:
            cs_alg->cs_type = OPTM_CS_EXVYCC_601;
            cs_alg->cs_range = PQ_CSC_RANGE_LMTD;
            break;

        case HI_DRV_CS_BT601_YUV_FULL:
            cs_alg->cs_type = OPTM_CS_EXVYCC_601;
            cs_alg->cs_range = PQ_CSC_RANGE_FULL;
            break;

        case HI_DRV_CS_BT709_RGB_LIMITED:
            cs_alg->cs_type = OPTM_CS_ERGB;
            cs_alg->cs_range = PQ_CSC_RANGE_LMTD;
            break;

        case HI_DRV_CS_BT709_RGB_FULL:
            cs_alg->cs_type = OPTM_CS_ERGB;
            cs_alg->cs_range = PQ_CSC_RANGE_FULL;
            break;

        case HI_DRV_CS_BT601_RGB_LIMITED:
            cs_alg->cs_type = OPTM_CS_ERGB;
            cs_alg->cs_range = PQ_CSC_RANGE_LMTD;
            break;

        case HI_DRV_CS_BT601_RGB_FULL:
            cs_alg->cs_type = OPTM_CS_ERGB;
            cs_alg->cs_range = PQ_CSC_RANGE_FULL;
            break;

        default:
            cs_alg->cs_type = OPTM_CS_EUNKNOWN;
            cs_alg->cs_range = PQ_CSC_RANGE_EUNKNOWN;
            break;
    }

    return;
}

/* 设置CSC参数 */
static hi_s32 pq_mng_set_csc_param(drv_pq_internal_csc_info csc_info, hi_drv_pq_csc_coef *csc_coef)
{
    pq_csc_mode csc_mode;
    pq_csc alg_cs_in, alg_cs_out;
    csc_para v0_csc_para;
    csc_para v1_csc_para;
    csc_para v3_csc_para;
    /* tran mode, cs_type and cs_range */
    get_vid_csc_type(csc_info.csc_in, &alg_cs_in);
    get_vid_csc_type(csc_info.csc_out, &alg_cs_out);
    /* CSC input_mode and output mode */
    csc_mode.csc_in.cs_type = alg_cs_in.cs_type;
    csc_mode.csc_in.cs_range = alg_cs_in.cs_range;
    csc_mode.csc_out.cs_type = alg_cs_out.cs_type;
    csc_mode.csc_out.cs_range = alg_cs_out.cs_range;

    /* HD: adjust brightness/contrast in V0/V1 CSC */
    if (HI_PQ_CSC_TUNING_V0 == csc_info.csc_type) {
        v0_csc_para.image_para.brightness = g_hd_picture_setting.brightness;
        v0_csc_para.image_para.contrast = g_hd_picture_setting.contrast;
        v0_csc_para.image_para.saturation = g_hd_picture_setting.saturation;
        v0_csc_para.image_para.hue = g_hd_picture_setting.hue;
        v0_csc_para.image_para.wcg_temperature = g_hd_picture_setting.wcg_temperature;

        if ((csc_mode.csc_in.cs_range == PQ_CSC_RANGE_FULL) || (csc_mode.csc_out.cs_range == PQ_CSC_RANGE_FULL)) {
            v0_csc_para.full_range = HI_TRUE;
        } else {
            v0_csc_para.full_range = HI_FALSE;
        }

        v0_csc_para.in_color_type = csc_mode.csc_in.cs_type;
        v0_csc_para.out_color_type = csc_mode.csc_out.cs_type;
        v0_csc_para.color_temp.red_gain = LEVEL2NUM(g_color_temp[HI_PQ_DISPLAY_1].red_gain);
        v0_csc_para.color_temp.green_gain = LEVEL2NUM(g_color_temp[HI_PQ_DISPLAY_1].green_gain);
        v0_csc_para.color_temp.blue_gain = LEVEL2NUM(g_color_temp[HI_PQ_DISPLAY_1].blue_gain);
        v0_csc_para.color_lut_temp_offset[COLORTEMP_R] = 128; /* 128 :a number */
        v0_csc_para.color_lut_temp_offset[COLORTEMP_G] = 128; /* 128 :a number */
        v0_csc_para.color_lut_temp_offset[COLORTEMP_B] = 128; /* 128 :a number */
        pq_mng_calc_csccoef(&v0_csc_para, &csc_coef->csc_ac_coef);
        pq_mng_calc_csc_dc_coef(&v0_csc_para, &csc_coef->csc_dc_coef);
    } else if (HI_PQ_CSC_TUNING_V1 == csc_info.csc_type) {
        v1_csc_para.image_para.brightness = g_hd_picture_setting.brightness;
        v1_csc_para.image_para.contrast = g_hd_picture_setting.contrast;
        v1_csc_para.image_para.saturation = g_hd_picture_setting.saturation;
        v1_csc_para.image_para.hue = g_hd_picture_setting.hue;
        v1_csc_para.image_para.wcg_temperature = g_hd_picture_setting.wcg_temperature;

        if ((csc_mode.csc_in.cs_range == PQ_CSC_RANGE_FULL) || (csc_mode.csc_out.cs_range == PQ_CSC_RANGE_FULL)) {
            v1_csc_para.full_range = HI_TRUE;
        } else {
            v1_csc_para.full_range = HI_FALSE;
        }

        v1_csc_para.in_color_type = csc_mode.csc_in.cs_type;
        v1_csc_para.out_color_type = csc_mode.csc_out.cs_type;
        v1_csc_para.color_temp.red_gain = LEVEL2NUM(g_color_temp[HI_PQ_DISPLAY_1].red_gain);
        v1_csc_para.color_temp.green_gain = LEVEL2NUM(g_color_temp[HI_PQ_DISPLAY_1].green_gain);
        v1_csc_para.color_temp.blue_gain = LEVEL2NUM(g_color_temp[HI_PQ_DISPLAY_1].blue_gain);
        v1_csc_para.color_lut_temp_offset[COLORTEMP_R] = 128; /* 128 :a number */
        v1_csc_para.color_lut_temp_offset[COLORTEMP_G] = 128; /* 128 :a number */
        v1_csc_para.color_lut_temp_offset[COLORTEMP_B] = 128; /* 128 :a number */
        pq_mng_calc_csccoef(&v1_csc_para, &csc_coef->csc_ac_coef);
        pq_mng_calc_csc_dc_coef(&v1_csc_para, &csc_coef->csc_dc_coef);
    } else if (HI_PQ_CSC_TUNING_V3 == csc_info.csc_type) {
        v3_csc_para.image_para.brightness = g_sd_picture_setting.brightness;
        v3_csc_para.image_para.contrast = g_sd_picture_setting.contrast;
        v3_csc_para.image_para.saturation = g_sd_picture_setting.saturation;
        v3_csc_para.image_para.hue = g_sd_picture_setting.hue;
        v3_csc_para.image_para.wcg_temperature = g_sd_picture_setting.wcg_temperature;

        if ((csc_mode.csc_in.cs_range == PQ_CSC_RANGE_FULL) || (csc_mode.csc_out.cs_range == PQ_CSC_RANGE_FULL)) {
            v3_csc_para.full_range = HI_TRUE;
        } else {
            v3_csc_para.full_range = HI_FALSE;
        }

        v3_csc_para.in_color_type = csc_mode.csc_in.cs_type;
        v3_csc_para.out_color_type = csc_mode.csc_out.cs_type;
        v3_csc_para.color_temp.red_gain = LEVEL2NUM(g_color_temp[HI_PQ_DISPLAY_0].red_gain);
        v3_csc_para.color_temp.green_gain = LEVEL2NUM(g_color_temp[HI_PQ_DISPLAY_0].green_gain);
        v3_csc_para.color_temp.blue_gain = LEVEL2NUM(g_color_temp[HI_PQ_DISPLAY_0].blue_gain);
        v3_csc_para.color_lut_temp_offset[COLORTEMP_R] = 128; /* 128 :a number */
        v3_csc_para.color_lut_temp_offset[COLORTEMP_G] = 128; /* 128 :a number */
        v3_csc_para.color_lut_temp_offset[COLORTEMP_B] = 128; /* 128 :a number */

        if ((WBC_POINT_AFTER_CSC == (wbc_point_sel)pq_hal_get_wbc_point_sel()) && (g_isogeny_mode == HI_TRUE)) {
            /* when video display mode is isogeny and WBC_VP point after CSC,
                  no set brightness/contrast/saturation/hue of V3 */
            PQPRINT(PQ_PRN_CSC, "wbc_point_sel:%d, isogeny_mode: %d\n", pq_hal_get_wbc_point_sel(), g_isogeny_mode);
            v3_csc_para.image_para.brightness = 128; /* 128 :a number */
            v3_csc_para.image_para.contrast = 128; /* 128 :a number */
            v3_csc_para.image_para.saturation = 128; /* 128 :a number */
            v3_csc_para.image_para.hue = 128; /* 128 :a number */
            v3_csc_para.image_para.wcg_temperature = 128; /* 128 :a number */
        }

        if (HI_PQ_CSC_TUNING_V3 == csc_info.csc_type) {
            pq_mng_calc_csccoef(&v3_csc_para, &csc_coef->csc_ac_coef);
            pq_mng_calc_csc_dc_coef(&v3_csc_para, &csc_coef->csc_dc_coef);
        }
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_csc_video_setting(hi_pq_display disp, hi_pq_image_param *pic_setting)
{
    if (pic_setting == HI_NULL) {
        HI_ERR_PQ("pst_pic_setting is null!\n");
        return HI_FAILURE;
    }

    if (HI_PQ_DISPLAY_0 == disp) { /* SD */
        g_sd_picture_setting.brightness = pic_setting->brightness;
        g_sd_picture_setting.contrast = pic_setting->contrast;
        g_sd_picture_setting.saturation = pic_setting->saturation;
        g_sd_picture_setting.hue = pic_setting->hue;
        g_sd_picture_setting.wcg_temperature = pic_setting->wcg_temperature;
        PQPRINT(PQ_PRN_CSC, "set MNG SD brightness=%d,contrast=%d,saturation=%d,hue=%d\n",
                g_sd_picture_setting.brightness, g_sd_picture_setting.contrast,
                g_sd_picture_setting.saturation, g_sd_picture_setting.hue);
    } else { /* HD */
        g_hd_picture_setting.brightness = pic_setting->brightness;
        g_hd_picture_setting.contrast = pic_setting->contrast;
        g_hd_picture_setting.saturation = pic_setting->saturation;
        g_hd_picture_setting.hue = pic_setting->hue;
        g_hd_picture_setting.wcg_temperature = pic_setting->wcg_temperature;
        PQPRINT(PQ_PRN_CSC, "set MNG HD brightness=%d,contrast=%d,saturation=%d,hue=%d\n",
                g_hd_picture_setting.brightness, g_hd_picture_setting.contrast,
                g_hd_picture_setting.saturation, g_hd_picture_setting.hue);
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_csc_video_setting(hi_pq_display disp, hi_pq_image_param *pic_setting)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(pic_setting);

    if (HI_PQ_DISPLAY_0 == disp) { /* SD */
        pic_setting->brightness = g_sd_picture_setting.brightness;
        pic_setting->contrast = g_sd_picture_setting.contrast;
        pic_setting->hue = g_sd_picture_setting.hue;
        pic_setting->saturation = g_sd_picture_setting.saturation;
        pic_setting->wcg_temperature = g_sd_picture_setting.wcg_temperature;
    } else { /* HD */
        pic_setting->brightness = g_hd_picture_setting.brightness;
        pic_setting->contrast = g_hd_picture_setting.contrast;
        pic_setting->hue = g_hd_picture_setting.hue;
        pic_setting->saturation = g_hd_picture_setting.saturation;
        pic_setting->wcg_temperature = g_hd_picture_setting.wcg_temperature;
    }

    return HI_SUCCESS;
}
static hi_bool pq_csc_check_type_valid(drv_pq_internal_csc_info csc_info)
{
    if ((csc_info.csc_in != HI_DRV_CS_BT601_YUV_LIMITED) &&
        (csc_info.csc_in != HI_DRV_CS_BT601_YUV_FULL) &&
        (csc_info.csc_in != HI_DRV_CS_BT601_RGB_LIMITED) &&
        (csc_info.csc_in != HI_DRV_CS_BT601_RGB_FULL) &&
        (csc_info.csc_in != HI_DRV_CS_BT709_YUV_LIMITED) &&
        (csc_info.csc_in != HI_DRV_CS_BT709_YUV_FULL) &&
        (csc_info.csc_in != HI_DRV_CS_BT709_RGB_LIMITED) &&
        (csc_info.csc_in != HI_DRV_CS_BT709_RGB_FULL)) {
        HI_ERR_PQ("csc_info.csc_in is not match type; %d !\n", csc_info.csc_in);
        return HI_FALSE;
    }

    if ((csc_info.csc_out != HI_DRV_CS_BT601_YUV_LIMITED) &&
        (csc_info.csc_out != HI_DRV_CS_BT601_YUV_FULL) &&
        (csc_info.csc_out != HI_DRV_CS_BT601_RGB_LIMITED) &&
        (csc_info.csc_out != HI_DRV_CS_BT601_RGB_FULL) &&
        (csc_info.csc_out != HI_DRV_CS_BT709_YUV_LIMITED) &&
        (csc_info.csc_out != HI_DRV_CS_BT709_YUV_FULL) &&
        (csc_info.csc_out != HI_DRV_CS_BT709_RGB_LIMITED) &&
        (csc_info.csc_out != HI_DRV_CS_BT709_RGB_FULL)) {
        HI_ERR_PQ("csc_info.csc_out is not match type; %d !\n", csc_info.csc_out);
        return HI_FALSE;
    }

    return HI_TRUE;
}

hi_s32 pq_mng_get_csc_coef(hi_drv_pq_csc_layer layer_id, drv_pq_internal_csc_info csc_info,
                           hi_drv_pq_csc_coef *csc_coef)
{
    hi_s32 ret;
    hi_bool is_valid_type;
    drv_pq_internal_csc_info csc_info_temp = { 0 };

    if (csc_coef == HI_NULL) {
        HI_ERR_PQ("csc_coef is NULL!\n");
        return HI_FAILURE;
    }

    PQ_CHECK_OVER_RANGE_RE_FAIL(layer_id, HI_DRV_PQ_CSC_LAYER_MAX);
    PQ_CHECK_OVER_RANGE_RE_FAIL(csc_info.csc_type, HI_PQ_CSC_TYPE_MAX);
    is_valid_type = pq_csc_check_type_valid(csc_info);
    if (is_valid_type == HI_FALSE) {
        return HI_FAILURE;
    }

    if (csc_info.csc_en == HI_TRUE) {
        ret = pq_mng_set_in_csc_trans(csc_info_temp.csc_type, csc_info.csc_in, &csc_info_temp.csc_in);
        PQ_CHECK_RETURN_SUCCESS(ret);
        ret = pq_mng_set_out_csc_trans(csc_info_temp.csc_type, csc_info.csc_out, &csc_info_temp.csc_out);
        PQ_CHECK_RETURN_SUCCESS(ret);
        pq_mng_set_csc_param(csc_info_temp, csc_coef);
    } else {
        HI_ERR_PQ("csc en is off !!! \n");
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_isogeny_mode(hi_bool isogeny_mode)
{
    g_isogeny_mode = isogeny_mode;
    return HI_SUCCESS;
}

hi_s32 pq_mng_get_isogeny_mode(hi_bool *isogeny_mode)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(isogeny_mode);
    *isogeny_mode = g_isogeny_mode;
    return HI_SUCCESS;
}

hi_s32 pq_mng_get_color_temp(hi_pq_display disp, hi_pq_color_temperature *temperature)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(temperature);
    temperature->red_gain = g_color_temp[disp].red_gain;
    temperature->green_gain = g_color_temp[disp].green_gain;
    temperature->blue_gain = g_color_temp[disp].blue_gain;
    return HI_SUCCESS;
}

hi_s32 pq_mng_set_color_temp(hi_pq_display disp, hi_pq_color_temperature *temperature)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(temperature);
    g_color_temp[disp].red_gain = temperature->red_gain;
    g_color_temp[disp].green_gain = temperature->green_gain;
    g_color_temp[disp].blue_gain = temperature->blue_gain;
    return HI_SUCCESS;
}

static pq_alg_funcs g_csc_funcs = {
    .set_isogeny_mode = pq_mng_set_isogeny_mode,
    .get_isogeny_mode = pq_mng_get_isogeny_mode,
    .get_csc_video_setting = pq_mng_get_csc_video_setting,
    .set_csc_video_setting = pq_mng_set_csc_video_setting,
    .get_csc_coef = pq_mng_get_csc_coef,
    .get_color_temp = pq_mng_get_color_temp,
    .set_color_temp = pq_mng_set_color_temp,
};

hi_s32 pq_mng_register_csc(pq_reg_type type)
{
    hi_s32 ret;
    ret = pq_comm_alg_register(HI_PQ_MODULE_CSC, type, PQ_BIN_ADAPT_SINGLE, "csc", &g_csc_funcs);
    return ret;
}

hi_s32 pq_mng_unregister_csc()
{
    hi_s32 ret;
    ret = pq_comm_alg_unregister(HI_PQ_MODULE_CSC);
    return ret;
}

