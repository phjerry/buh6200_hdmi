/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description:
 * Author: pq
 * Create: 2016-01-1
 */

#include "pq_mng_gfxcsc.h"
#include "pq_hal_comm.h"
#include <linux/kernel.h>

pq_gfx_csc_proc_info g_gfx_csc_info = {0};
static hi_bool g_is_bgr_in = HI_FALSE;
pq_gfx_csc_tuning g_gfx_csc_tuning = {
    PQ_CSC_PICTURE_DEFAULT, PQ_CSC_PICTURE_DEFAULT,
    PQ_CSC_PICTURE_DEFAULT, PQ_CSC_PICTURE_DEFAULT,
    PQ_CSC_PICTURE_DEFAULT, PQ_CSC_PICTURE_DEFAULT,
    PQ_CSC_PICTURE_DEFAULT, PQ_CSC_PICTURE_DEFAULT
};

static hi_pq_image_param g_gfxcsc_sd_pic_para = {
    PQ_CSC_PICTURE_DEFAULT, PQ_CSC_PICTURE_DEFAULT,
    PQ_CSC_PICTURE_DEFAULT, PQ_CSC_PICTURE_DEFAULT,
    PQ_CSC_PICTURE_DEFAULT,
};
static hi_pq_image_param g_gfxcsc_hd_pic_para = {
    PQ_CSC_PICTURE_DEFAULT, PQ_CSC_PICTURE_DEFAULT,
    PQ_CSC_PICTURE_DEFAULT, PQ_CSC_PICTURE_DEFAULT,
    PQ_CSC_PICTURE_DEFAULT
};

/*************************** structure definition ****************************/
typedef enum {
    PQ_GFX_CS_EUNKNOWN = 0,
    PQ_GFX_CS_EYCBCR_709 = 1,
    PQ_GFX_CS_EYCBCR_601 = 2,
    PQ_GFX_CS_EYCBCR_2020 = 3, /* new add */
    PQ_GFX_CS_ERGB_709 = 4,
    PQ_GFX_CS_ERGB_601 = 5,  /* new add */
    PQ_GFX_CS_ERGB_2020 = 6, /* new add */

    PQ_GFX_CS_TYPE_MAX
} pq_gfx_cs_type;

typedef enum {
    PQ_GFX_CS_RANGE_EUNKNOWN = 0,
    PQ_GFX_CS_RANGE_FULL = 1,
    PQ_GFX_CS_RANGE_LMTD = 2,
    PQ_GFX_CS_RANGE_MAX
} pq_gfx_cs_range;

typedef struct {
    pq_gfx_cs_type cs_type;
    pq_gfx_cs_range cs_range;
} pq_gfx_cs;

typedef struct {
    pq_gfx_cs gfx_csc_csc_in;
    pq_gfx_cs gfx_csc_csc_out;
} pq_gfx_csc_mode;

const hi_s32 g_pq_gfx_sin_table[PQ_GFX_TABLE_MAX] = {
    -500, -485, -469, -454, -438, -422,
    -407, -391, -374, -358, -342, -325,
    -309, -292, -276, -259, -242, -225,
    -208, -191, -174, -156, -139, -122,
    -104, -87, -70, -52, -35, -17,
    0, 17, 35, 52, 70, 87,
    104, 122, 139, 156, 174, 191,
    208, 225, 242, 259, 276, 292,
    309, 325, 342, 358, 374, 391,
    407, 422, 438, 454, 469, 485,
    500
};

const hi_u32 g_pq_gfx_cos_table[PQ_GFX_TABLE_MAX] = {
    866, 875, 883, 891, 899, 906,
    914, 921, 927, 934, 940, 946,
    951, 956, 961, 966, 970, 974,
    978, 982, 985, 988, 990, 993,
    995, 996, 998, 999, 999, 1000,
    1000, 1000, 999, 999, 998, 996,
    995, 993, 990, 988, 985, 982,
    978, 974, 970, 966, 961, 956,
    951, 946, 940, 934, 927, 921,
    914, 906, 899, 891, 883, 875,
    866
};

/**************************************************
         CSC matrix for [y_cb_cr]->[y_cb_cr]
**************************************************/
/* y_cb_cr_601_l (i.e. SD) -> y_cb_cr_709_l (i.e. HD) */
const hi_drv_pq_gfx_csc_ac_coef g_pq_gfx_csc_table_yuv601l_to_yuv709l = {
    1024, -118, -213,
    0, 1043, 117,
    0, 77, 1050
};

/* y_cb_cr_709_l (i.e. HD) -> y_cb_cr_601_l (i.e. SD) */
const hi_drv_pq_gfx_csc_ac_coef g_pq_gfx_csc_table_yuv709l_to_yuv601l = {
    1024, 102, 196,
    0, 1014, -113,
    0, -74, 1007
};

/**************************************************
         CSC matrix for [RGB]->[y_cb_cr]
**************************************************/
/* s_rgbf -> y_cb_cr_709_l (i.e. HD) */
const hi_drv_pq_gfx_csc_ac_coef g_pq_gfx_csc_table_srgb709f_to_yuv709l = {
    188, 629, 63,
    -103, -347, 450,
    450, -409, -41
};                 /* range[0,255]->range[16,235] */

/* s_rgbf -> y_cb_cr_601_l (i.e. SD) */
const hi_drv_pq_gfx_csc_ac_coef g_pq_gfx_csc_table_srgb601f_to_yuv601l = {
    264, 516, 100,
    -152, -298, 450,
    450, -377, -73
};                 /* range[0,255]->range[16,235] */

/**************************************************
          CSC matrix for [y_cb_cr]->[RGB]
**************************************************/
/* y_cb_cr_709_l (i.e. HD) -> s_rgbf */
const hi_drv_pq_gfx_csc_ac_coef g_pq_gfx_csc_table_yuv709l_to_srgb709f = {
    1192, 0, 1836,
    1192, -218, -547,
    1192, 2166, 0
};                 /* range[0,255]->range[16,235] */

/* y_cb_cr_601_l (i.e. SD) -> s_rgbf */
const hi_drv_pq_gfx_csc_ac_coef g_pq_gfx_csc_table_yuv601l_to_srgb601f = {
    1192, 0, 1634,
    1192, -400, -833,
    1192, 2066, 0
};                 /* range[0,255]->range[16,235] */

/**************************************************
       CSC matrix for no change of color space
**************************************************/
/* identity matrix */
const hi_drv_pq_gfx_csc_ac_coef g_pq_gfx_csc_table_identity = {
    1024, 0, 0,
    0, 1024, 0,
    0, 0, 1024
};

/* the compositor color matrices table with color primaries matching */
/* add 2020YUV、2020RGB、 601YUV */
const static hi_drv_pq_gfx_csc_ac_coef *g_pq_gfx_csc_matrix_tbl[PQ_GFX_CS_TYPE_MAX][PQ_GFX_CS_TYPE_MAX] = {
    {
        &g_pq_gfx_csc_table_identity,                    /* unknown -> unknown;   identity */
        &g_pq_gfx_csc_table_identity,                    /* unknown -> y_cb_cr709;  identity */
        &g_pq_gfx_csc_table_identity,                    /* unknown -> y_cb_cr601;  identity */
        &g_pq_gfx_csc_table_identity, /* unknown -> y_cb_cr2020; identity // impossible condition */
        &g_pq_gfx_csc_table_identity,                    /* unknown -> s_rgb709;   identity */
        &g_pq_gfx_csc_table_identity, /* unknown -> s_rgb601;   identity // impossible condition */
        &g_pq_gfx_csc_table_identity /* unknown -> s_rgb2020;  identity // impossible condition */
    },

    {
        &g_pq_gfx_csc_table_identity,                                /* y_cb_cr709 -> unknown */
        &g_pq_gfx_csc_table_identity,                                /* y_cb_cr709 -> y_cb_cr709 */
        &g_pq_gfx_csc_table_yuv709l_to_yuv601l,                /* y_cb_cr709 -> y_cb_cr601 */
        &g_pq_gfx_csc_table_identity, /* y_cb_cr709 -> y_cb_cr2020 */             /* impossible condition */
        &g_pq_gfx_csc_table_yuv709l_to_srgb709f,                    /* y_cb_cr709 -> s_rgb709 */
        &g_pq_gfx_csc_table_yuv709l_to_srgb709f, /* y_cb_cr709 -> s_rgb601 // impossible condition */
        &g_pq_gfx_csc_table_yuv709l_to_srgb709f /* y_cb_cr709 -> s_rgb2020 */  /* impossible condition */
    },

    {
        &g_pq_gfx_csc_table_identity,                    /* y_cb_cr601 -> unknown */
        &g_pq_gfx_csc_table_yuv601l_to_yuv709l,    /* y_cb_cr601 -> y_cb_cr709 */
        &g_pq_gfx_csc_table_identity,                    /* y_cb_cr601 -> y_cb_cr601 */
        &g_pq_gfx_csc_table_identity, /* y_cb_cr601 -> y_cb_cr2020 // impossible condition */
        &g_pq_gfx_csc_table_yuv601l_to_srgb601f,        /* y_cb_cr601 -> s_rgb709 */
        &g_pq_gfx_csc_table_identity, /* y_cb_cr601 -> s_rgb601 // impossible condition */
        &g_pq_gfx_csc_table_identity /* y_cb_cr601 -> s_rgb2020 // impossible condition */
    },

    {
        &g_pq_gfx_csc_table_identity, /* y_cb_cr2020 -> unknown;   identity */
        &g_pq_gfx_csc_table_identity, /* y_cb_cr2020 -> y_cb_cr709;  identity */
        &g_pq_gfx_csc_table_identity, /* y_cb_cr2020 -> y_cb_cr601;  identity */
        &g_pq_gfx_csc_table_identity, /* y_cb_cr2020 -> y_cb_cr2020; identity */
        &g_pq_gfx_csc_table_identity, /* y_cb_cr2020 -> s_rgb709;   identity */
        &g_pq_gfx_csc_table_identity, /* y_cb_cr2020 -> s_rgb601;   identity */
        &g_pq_gfx_csc_table_identity  /* y_cb_cr2020 -> s_rgb2020;  identity */
    },

    {
        &g_pq_gfx_csc_table_identity,                    /* s_rgb709 -> unknown */
        &g_pq_gfx_csc_table_srgb709f_to_yuv709l,        /* s_rgb709 -> y_cb_cr709 */
        &g_pq_gfx_csc_table_srgb601f_to_yuv601l,        /* s_rgb709 -> y_cb_cr601 */
        &g_pq_gfx_csc_table_identity, /* s_rgb709 -> y_cb_cr2020 // impossible condition */
        &g_pq_gfx_csc_table_identity,                    /* s_rgb709 -> s_rgb709 */
        &g_pq_gfx_csc_table_identity, /* s_rgb709 -> s_rgb601 // impossible condition */
        &g_pq_gfx_csc_table_identity /* s_rgb709 -> s_rgb2020 // impossible condition */
    },

    {
        &g_pq_gfx_csc_table_identity, /* s_rgb601 -> unknown */
        &g_pq_gfx_csc_table_identity, /* s_rgb601 -> y_cb_cr709 */
        &g_pq_gfx_csc_table_identity, /* s_rgb601 -> y_cb_cr601 */
        &g_pq_gfx_csc_table_identity, /* s_rgb601 -> y_cb_cr2020 */
        &g_pq_gfx_csc_table_identity, /* s_rgb601 -> s_rgb709 */
        &g_pq_gfx_csc_table_identity, /* s_rgb601 -> s_rgb601 */
        &g_pq_gfx_csc_table_identity  /* s_rgb601 -> s_rgb2020 */
    },

    {
        &g_pq_gfx_csc_table_identity, /* identity */
        &g_pq_gfx_csc_table_identity, /* identity */
        &g_pq_gfx_csc_table_identity, /* identity */
        &g_pq_gfx_csc_table_identity, /* identity */
        &g_pq_gfx_csc_table_identity, /* identity */
        &g_pq_gfx_csc_table_identity, /* identity */
        &g_pq_gfx_csc_table_identity  /* identity */
    },
};

/***************************************************************************************/

hi_void get_csc_type(drv_pq_internal_color_space cs_drv, pq_gfx_cs *cs_alg)
{
    switch (cs_drv) {
        case HI_DRV_CS_BT601_YUV_LIMITED:
            cs_alg->cs_type = PQ_GFX_CS_EYCBCR_601;
            cs_alg->cs_range = PQ_GFX_CS_RANGE_LMTD;
            break;
        case HI_DRV_CS_BT601_YUV_FULL:
            cs_alg->cs_type = PQ_GFX_CS_EYCBCR_601;
            cs_alg->cs_range = PQ_GFX_CS_RANGE_FULL;
            break;
        case HI_DRV_CS_BT601_RGB_FULL:
            cs_alg->cs_type = PQ_GFX_CS_ERGB_601;
            cs_alg->cs_range = PQ_GFX_CS_RANGE_FULL;
            break;
        case HI_DRV_CS_BT709_YUV_LIMITED:
            cs_alg->cs_type = PQ_GFX_CS_EYCBCR_709;
            cs_alg->cs_range = PQ_GFX_CS_RANGE_LMTD;
            break;
        case HI_DRV_CS_BT709_YUV_FULL:
            cs_alg->cs_type = PQ_GFX_CS_EYCBCR_709;
            cs_alg->cs_range = PQ_GFX_CS_RANGE_FULL;
            break;
        case HI_DRV_CS_BT709_RGB_FULL:
            cs_alg->cs_type = PQ_GFX_CS_ERGB_709;
            cs_alg->cs_range = PQ_GFX_CS_RANGE_FULL;
            break;
        case HI_DRV_CS_BT2020_YUV_LIMITED:
            cs_alg->cs_type = PQ_GFX_CS_EYCBCR_2020;
            cs_alg->cs_range = PQ_GFX_CS_RANGE_LMTD;
            break;
        case HI_DRV_CS_BT2020_RGB_FULL:
            cs_alg->cs_type = PQ_GFX_CS_ERGB_2020;
            cs_alg->cs_range = PQ_GFX_CS_RANGE_FULL;
            break;
        default:
            cs_alg->cs_type = PQ_GFX_CS_EUNKNOWN;
            cs_alg->cs_range = PQ_GFX_CS_RANGE_EUNKNOWN;
            break;
    }
    return;
}

static hi_void pq_mng_calc_csc_dc_yuv2_yuv(pq_gfx_csc_mode *gfx_csc_mode, hi_drv_pq_gfx_csc_dc_coef *csc_dc_coef)
{
    hi_s32 tmp_bright = (hi_s32)g_gfx_csc_tuning.bright - PQ_CSC_PICTURE_DEFAULT;
    switch (gfx_csc_mode->gfx_csc_csc_in.cs_range) { /* input DC */
        case PQ_GFX_CS_RANGE_FULL:                     /* YUV_F -> YUV_FL */
            csc_dc_coef->csc_in_dc0 = 0;
            csc_dc_coef->csc_in_dc1 = -128; /* -128 dc coef */
            csc_dc_coef->csc_in_dc2 = -128; /* -128 dc coef */
            break;
        case PQ_GFX_CS_RANGE_LMTD: /* YUV_L -> YUV_FL */
            csc_dc_coef->csc_in_dc0 = -16;  /* -16 dc coef */
            csc_dc_coef->csc_in_dc1 = -128; /* -128 dc coef */
            csc_dc_coef->csc_in_dc2 = -128; /* -128 dc coef */
            break;
        default:
            csc_dc_coef->csc_in_dc0 = -16;  /* -16 dc coef */
            csc_dc_coef->csc_in_dc1 = -128; /* -128 dc coef */
            csc_dc_coef->csc_in_dc2 = -128; /* -128 dc coef */
            break;
    }

    switch (gfx_csc_mode->gfx_csc_csc_out.cs_range) { /* output DC */
        case PQ_GFX_CS_RANGE_FULL:                      /* YUV_FL -> YUV_F */
            csc_dc_coef->csc_out_dc0 = tmp_bright;
            csc_dc_coef->csc_out_dc1 = 128; /* 128 dc coef */
            csc_dc_coef->csc_out_dc2 = 128; /* 128 dc coef */
            break;
        case PQ_GFX_CS_RANGE_LMTD: /* YUV_FL -> YUV_L */
            csc_dc_coef->csc_out_dc0 = tmp_bright + 16;  /* 16 dc coef */
            csc_dc_coef->csc_out_dc1 = 128; /* 128 dc coef */
            csc_dc_coef->csc_out_dc2 = 128; /* 128 dc coef */
            break;
        default:
            csc_dc_coef->csc_out_dc0 = tmp_bright + 16; /* 16 dc coef */
            csc_dc_coef->csc_out_dc1 = 128; /* 128 dc coef */
            csc_dc_coef->csc_out_dc2 = 128; /* 128 dc coef */
            break;
    }

    return;
}

static hi_void pq_mng_calc_csc_dc_yuv2_rgb(pq_gfx_csc_mode *gfx_csc_mode, hi_drv_pq_gfx_csc_dc_coef *csc_dc_coef)
{
    hi_s32 tmp_bright = (hi_s32)g_gfx_csc_tuning.bright - PQ_CSC_PICTURE_DEFAULT;
    hi_s32 tmp_contrst = (hi_s32)g_gfx_csc_tuning.contrst;

    if (tmp_contrst == 0) {
        tmp_contrst = 1;
    }

    switch (gfx_csc_mode->gfx_csc_csc_in.cs_range) { /* input DC */
        case PQ_GFX_CS_RANGE_FULL:                     /* YUV_F -> RGB_FL */
            csc_dc_coef->csc_in_dc0 = tmp_bright * PQ_CSC_PICTURE_DEFAULT / (hi_s32)tmp_contrst;
            csc_dc_coef->csc_in_dc1 = -128; /* -128 dc coef */
            csc_dc_coef->csc_in_dc2 = -128; /* -128 dc coef */
            break;
        case PQ_GFX_CS_RANGE_LMTD: /* YUV_L -> RGB_FL */
            csc_dc_coef->csc_in_dc0 = -16 +  /* -16 dc coef */
                                      (tmp_bright * PQ_CSC_PICTURE_DEFAULT / (hi_s32)tmp_contrst);
            csc_dc_coef->csc_in_dc1 = -128; /* -128 dc coef */
            csc_dc_coef->csc_in_dc2 = -128; /* -128 dc coef */
            break;
        default:
            csc_dc_coef->csc_in_dc0 = -16 + /* -16 dc coef */
                                      (tmp_bright * PQ_CSC_PICTURE_DEFAULT / (hi_s32)tmp_contrst);
            csc_dc_coef->csc_in_dc1 = -128; /* -128 dc coef */
            csc_dc_coef->csc_in_dc2 = -128; /* -128 dc coef */
            break;
    }

    switch (gfx_csc_mode->gfx_csc_csc_in.cs_range) { /* output DC */
        case PQ_GFX_CS_RANGE_FULL:                     /* YUV_FL -> RGB_F */
            csc_dc_coef->csc_out_dc0 = 0;
            csc_dc_coef->csc_out_dc1 = 0;
            csc_dc_coef->csc_out_dc2 = 0;
            break;
        case PQ_GFX_CS_RANGE_LMTD: /* YUV_FL -> RGB_L */
            csc_dc_coef->csc_out_dc0 = 16; /* 16 dc coef */
            csc_dc_coef->csc_out_dc1 = 16; /* 16 dc coef */
            csc_dc_coef->csc_out_dc2 = 16; /* 16 dc coef */
            break;
        default:
            csc_dc_coef->csc_out_dc0 = 0;
            csc_dc_coef->csc_out_dc1 = 0;
            csc_dc_coef->csc_out_dc2 = 0;
            break;
    }

    return;
}

static hi_void pq_mng_calc_csc_dc_rgb2_yuv(pq_gfx_csc_mode *gfx_csc_mode, hi_drv_pq_gfx_csc_dc_coef *csc_dc_coef)
{
    hi_s32 tmp_bright = (hi_s32)g_gfx_csc_tuning.bright - PQ_CSC_PICTURE_DEFAULT;
    switch (gfx_csc_mode->gfx_csc_csc_in.cs_range) { /* input DC */
        case PQ_GFX_CS_RANGE_FULL:                     /* RGB_F -> YUV_FL */
            csc_dc_coef->csc_in_dc0 = 0;
            csc_dc_coef->csc_in_dc1 = 0;
            csc_dc_coef->csc_in_dc2 = 0;
            break;
        case PQ_GFX_CS_RANGE_LMTD: /* RGB_L -> YUV_FL */
            csc_dc_coef->csc_in_dc0 = 16; /* 16 dc coef */
            csc_dc_coef->csc_in_dc1 = 16; /* 16 dc coef */
            csc_dc_coef->csc_in_dc2 = 16; /* 16 dc coef */
            break;
        default:
            csc_dc_coef->csc_in_dc0 = 0;
            csc_dc_coef->csc_in_dc1 = 0;
            csc_dc_coef->csc_in_dc2 = 0;
            break;
    }

    switch (gfx_csc_mode->gfx_csc_csc_out.cs_range) { /* output DC */
        case PQ_GFX_CS_RANGE_FULL:                      /* RGB_FL -> YUV_F */
            csc_dc_coef->csc_out_dc0 = tmp_bright;
            csc_dc_coef->csc_out_dc1 = 128; /* 128 dc coef */
            csc_dc_coef->csc_out_dc2 = 128; /* 128 dc coef */
            break;
        case PQ_GFX_CS_RANGE_LMTD: /* RGB_FL -> YUV_L */
            csc_dc_coef->csc_out_dc0 = tmp_bright + 16;  /* 16 dc coef */
            csc_dc_coef->csc_out_dc1 = 128; /* 128 dc coef */
            csc_dc_coef->csc_out_dc2 = 128; /* 128 dc coef */
            break;
        default:
            csc_dc_coef->csc_out_dc0 = tmp_bright + 16;  /* 16 dc coef */
            csc_dc_coef->csc_out_dc1 = 128; /* 128 dc coef */
            csc_dc_coef->csc_out_dc2 = 128; /* 128 dc coef */
            break;
    }

    return;
}

static hi_void pq_mng_calc_csc_dc_rgb2_rgb(pq_gfx_csc_mode *gfx_csc_mode, hi_drv_pq_gfx_csc_dc_coef *csc_dc_coef)
{
    hi_s32 tmp_bright = (hi_s32)g_gfx_csc_tuning.bright - PQ_CSC_PICTURE_DEFAULT;
    switch (gfx_csc_mode->gfx_csc_csc_in.cs_range) { /* input DC */
        case PQ_GFX_CS_RANGE_FULL:                     /* RGB_F -> RGB_FL */
            csc_dc_coef->csc_in_dc0 = 0;
            csc_dc_coef->csc_in_dc1 = 0;
            csc_dc_coef->csc_in_dc2 = 0;
            break;
        case PQ_GFX_CS_RANGE_LMTD: /* RGB_L -> RGB_FL */
            csc_dc_coef->csc_in_dc0 = 16;  /* 16 dc coef */
            csc_dc_coef->csc_in_dc1 = 16;  /* 16 dc coef */
            csc_dc_coef->csc_in_dc2 = 16;  /* 16 dc coef */
            break;
        default:
            csc_dc_coef->csc_in_dc0 = 0;
            csc_dc_coef->csc_in_dc1 = 0;
            csc_dc_coef->csc_in_dc2 = 0;
            break;
    }

    switch (gfx_csc_mode->gfx_csc_csc_out.cs_range) { /* output DC */
        case PQ_GFX_CS_RANGE_FULL:                      /* RGB_FL -> RGB_F */
            csc_dc_coef->csc_out_dc0 = (tmp_bright * 298 + 128) / 256; /* 298/128/256 dc coef */
            csc_dc_coef->csc_out_dc1 = (tmp_bright * 298 + 128) / 256; /* 298/128/256 dc coef */
            csc_dc_coef->csc_out_dc2 = (tmp_bright * 298 + 128) / 256; /* 298/128/256 dc coef */
            break;
        case PQ_GFX_CS_RANGE_LMTD:                                          /* RGB_FL -> RGB_L */
            csc_dc_coef->csc_out_dc0 = (tmp_bright * 298 + 128) / 256 + 16; /* 298/128/256/16 dc coef */
            csc_dc_coef->csc_out_dc1 = (tmp_bright * 298 + 128) / 256 + 16; /* 298/128/256/16 dc coef */
            csc_dc_coef->csc_out_dc2 = (tmp_bright * 298 + 128) / 256 + 16; /* 298/128/256/16 dc coef */
            break;
        default:
            csc_dc_coef->csc_out_dc0 = (tmp_bright * 298 + 128) / 256; /* 298/128/256 dc coef */
            csc_dc_coef->csc_out_dc1 = (tmp_bright * 298 + 128) / 256; /* 298/128/256 dc coef */
            csc_dc_coef->csc_out_dc2 = (tmp_bright * 298 + 128) / 256; /* 298/128/256 dc coef */
            break;
    }

    return;
}

/***************************************************************************************
* func          : pq_mng_calc_csc_dc
* description   : c_ncomment: 计算DC系数 c_nend\n
* param[in]     : hi_void
* retval        : NA
* others:       : NA
***************************************************************************************/
hi_void pq_mng_calc_csc_dc(pq_gfx_csc_mode *gfx_csc_mode, hi_drv_pq_gfx_csc_dc_coef *csc_dc_coef)
{
    if ((gfx_csc_mode->gfx_csc_csc_in.cs_type >= PQ_GFX_CS_EYCBCR_709)
        && (gfx_csc_mode->gfx_csc_csc_in.cs_type <= PQ_GFX_CS_EYCBCR_2020)) {      /* input YUV color space */
        if ((gfx_csc_mode->gfx_csc_csc_out.cs_type >= PQ_GFX_CS_EYCBCR_709)
            && (gfx_csc_mode->gfx_csc_csc_out.cs_type <= PQ_GFX_CS_EYCBCR_2020)) { /* YUV->YUV */
            pq_mng_calc_csc_dc_yuv2_yuv(gfx_csc_mode, csc_dc_coef);
        } else { /* YUV -> RGB */
            pq_mng_calc_csc_dc_yuv2_rgb(gfx_csc_mode, csc_dc_coef);
        }
    } else {                                                                               /* input RGB color space */
        if ((gfx_csc_mode->gfx_csc_csc_out.cs_type >= PQ_GFX_CS_EYCBCR_709)
            && (gfx_csc_mode->gfx_csc_csc_out.cs_type <= PQ_GFX_CS_EYCBCR_601)) { /* RGB->YUV */
            pq_mng_calc_csc_dc_rgb2_yuv(gfx_csc_mode, csc_dc_coef);
        } else { /* RGB -> RGB */
            pq_mng_calc_csc_dc_rgb2_rgb(gfx_csc_mode, csc_dc_coef);
        }
    }

    csc_dc_coef->csc_in_dc0 = (hi_s32)(csc_dc_coef->csc_in_dc0 * 4);  /* 4 : dc multi */
    csc_dc_coef->csc_in_dc1 = (hi_s32)(csc_dc_coef->csc_in_dc1 * 4);  /* 4 : dc multi */
    csc_dc_coef->csc_in_dc2 = (hi_s32)(csc_dc_coef->csc_in_dc2 * 4);  /* 4 : dc multi */
    csc_dc_coef->csc_out_dc0 = (hi_s32)(csc_dc_coef->csc_out_dc0 * 4);  /* 4 : dc multi */
    csc_dc_coef->csc_out_dc1 = (hi_s32)(csc_dc_coef->csc_out_dc1 * 4);  /* 4 : dc multi */
    csc_dc_coef->csc_out_dc2 = (hi_s32)(csc_dc_coef->csc_out_dc2 * 4);  /* 4 : dc multi */

    return;
}

/* for RGB -> RGB */
static hi_void pq_mng_calc_csc_coef_rgbto_rgb(pq_gfx_csc_tuning *gfx_csc_tuning,
    const hi_drv_pq_gfx_csc_ac_coef *matrix_ori, hi_drv_pq_gfx_csc_ac_coef *matrix_dst)
{
    hi_s32 contrst_adj, sat_adj, rgain, ggain, bgain;
    hi_u32 hue;

    contrst_adj = (hi_s32)(gfx_csc_tuning->contrst) - PQ_CSC_PICTURE_DEFAULT;
    contrst_adj = ((2 * contrst_adj) / 4) + PQ_CSC_PICTURE_MAX;   /* 2/4 : csc multi */
    sat_adj = (hi_s32)(gfx_csc_tuning->satur) - PQ_CSC_PICTURE_DEFAULT;
    sat_adj = ((2 * sat_adj) / 4) + PQ_CSC_PICTURE_MAX;   /* 2/4 : csc multi */
    hue = gfx_csc_tuning->hue * 60 / PQ_CSC_PICTURE_MAX;   /* 60 : csc multi */

    rgain = (hi_s32)(gfx_csc_tuning->kr) - PQ_CSC_PICTURE_DEFAULT;
    rgain = 2 * rgain + PQ_CSC_PICTURE_MAX;   /* 2 : rgb gain */
    ggain = (hi_s32)(gfx_csc_tuning->kg) - PQ_CSC_PICTURE_DEFAULT;
    ggain = 2 * ggain + PQ_CSC_PICTURE_MAX;   /* 2 : rgb gain */
    bgain = (hi_s32)(gfx_csc_tuning->kb) - PQ_CSC_PICTURE_DEFAULT;
    bgain = 2 * bgain + PQ_CSC_PICTURE_MAX;   /* 2 : rgb gain */

    if (hue >= PQ_GFX_TABLE_MAX) {
        return;
    }

    /* 306/242: coef mult */
    matrix_dst->csc_coef00 = (hi_s32)(306 + (hi_s32)(242 * g_pq_gfx_sin_table[hue] * sat_adj) / PQ_MULT_100000 +
    (hi_s32)(718 * g_pq_gfx_cos_table[hue] * sat_adj) / PQ_MULT_100000) * /* 718: coef mult */
    contrst_adj * rgain / PQ_MULT_10000;
    /* 601/475: coef mult */
    matrix_dst->csc_coef01 = (hi_s32)(601 + (hi_s32)(475 * g_pq_gfx_sin_table[hue] * sat_adj) / PQ_MULT_100000 -
    (hi_s32)(601 * g_pq_gfx_cos_table[hue] * sat_adj) / PQ_MULT_100000) *  /* 601: coef mult */
    contrst_adj * ggain / PQ_MULT_10000;
     /* 117/718: coef mult */
    matrix_dst->csc_coef02 = (hi_s32)(117 - (hi_s32)(718 * g_pq_gfx_sin_table[hue] * sat_adj) / PQ_MULT_100000 -
    (hi_s32)(117 * g_pq_gfx_cos_table[hue] * sat_adj) / PQ_MULT_100000) * /* 117: coef mult */
    contrst_adj * bgain / PQ_MULT_10000;
     /* 306/299: coef mult */
    matrix_dst->csc_coef10 = (hi_s32)(306 - (hi_s32)(299 * g_pq_gfx_sin_table[hue] * sat_adj) / PQ_MULT_100000 -
    (hi_s32)(306 * g_pq_gfx_cos_table[hue] * sat_adj) / PQ_MULT_100000) * /* 306: coef mult */
    contrst_adj * rgain / PQ_MULT_10000;
     /* 601/95: coef mult */
    matrix_dst->csc_coef11 = (hi_s32)(601 - (hi_s32)(95 * g_pq_gfx_sin_table[hue] * sat_adj) / PQ_MULT_100000 +
    (hi_s32)(423 * g_pq_gfx_cos_table[hue] * sat_adj) / PQ_MULT_100000) * /* 423: coef mult */
     contrst_adj * ggain / PQ_MULT_10000;
     /* 117/394: coef mult */
    matrix_dst->csc_coef12 = (hi_s32)(117 + (hi_s32)(394 * g_pq_gfx_sin_table[hue] * sat_adj) / PQ_MULT_100000 -
    (hi_s32)(117 * g_pq_gfx_cos_table[hue] * sat_adj) / PQ_MULT_100000) * /* 117: coef mult */
    contrst_adj * bgain / PQ_MULT_10000;
     /* 306/906: coef mult */
    matrix_dst->csc_coef20 = (hi_s32)(306 + (hi_s32)(906 * g_pq_gfx_sin_table[hue] * sat_adj) / PQ_MULT_100000 -
    (hi_s32)(306 * g_pq_gfx_cos_table[hue] * sat_adj) / PQ_MULT_100000) * /* 306: coef mult */ 
    contrst_adj * rgain / PQ_MULT_10000;
     /* 601/760: coef mult */
    matrix_dst->csc_coef21 = (hi_s32)(601 - (hi_s32)(760 * g_pq_gfx_sin_table[hue] * sat_adj) / PQ_MULT_100000 -
    (hi_s32)(601 * g_pq_gfx_cos_table[hue] * sat_adj) / PQ_MULT_100000) * /* 601: coef mult */
    contrst_adj * ggain / PQ_MULT_10000;
     /* 117/146: coef mult */
    matrix_dst->csc_coef22 = (hi_s32)(117 - (hi_s32)(146 * g_pq_gfx_sin_table[hue] * sat_adj) / PQ_MULT_100000 +
    (hi_s32)(907 * g_pq_gfx_cos_table[hue] * sat_adj) / PQ_MULT_100000) * /* 907: coef mult */
    contrst_adj * bgain / PQ_MULT_10000;

    return;
}

/* for RGB -> y_cb_cr */
static hi_void pq_mng_calc_csc_coef_rgbto_ycb_cr(pq_gfx_csc_tuning *gfx_csc_tuning,
    const hi_drv_pq_gfx_csc_ac_coef *matrix_ori, hi_drv_pq_gfx_csc_ac_coef *matrix_dst)
{
    hi_s32 contrst_adj, sat_adj, rgain, ggain, bgain;
    hi_u32 hue;

    /* 0-100 */
    contrst_adj = (hi_s32)(gfx_csc_tuning->contrst) - PQ_CSC_PICTURE_DEFAULT;
    contrst_adj = contrst_adj + PQ_CSC_PICTURE_MAX;
    sat_adj = (hi_s32)(gfx_csc_tuning->satur) - PQ_CSC_PICTURE_DEFAULT;
    sat_adj = ((2 * sat_adj * 3) / 4) + PQ_CSC_PICTURE_MAX; /* 2/3/4 multi */
    hue = gfx_csc_tuning->hue * 60 / PQ_CSC_PICTURE_MAX; /* 60 multi */

    rgain = (hi_s32)(gfx_csc_tuning->kr) - PQ_CSC_PICTURE_DEFAULT;
    rgain = 2 * rgain + PQ_CSC_PICTURE_MAX; /* 2: rgb gain */
    ggain = (hi_s32)(gfx_csc_tuning->kg) - PQ_CSC_PICTURE_DEFAULT;
    ggain = 2 * ggain + PQ_CSC_PICTURE_MAX; /* 2: rgb gain */
    bgain = (hi_s32)(gfx_csc_tuning->kb) - PQ_CSC_PICTURE_DEFAULT;
    bgain = 2 * bgain + PQ_CSC_PICTURE_MAX; /* 2: rgb gain */

    matrix_dst->csc_coef00 = matrix_ori->csc_coef00 * contrst_adj * rgain / PQ_MULT_10000;
    matrix_dst->csc_coef01 = matrix_ori->csc_coef01 * contrst_adj * ggain / PQ_MULT_10000;
    matrix_dst->csc_coef02 = matrix_ori->csc_coef02 * contrst_adj * bgain / PQ_MULT_10000;

    if (hue >= PQ_GFX_TABLE_MAX) {
        return;
    }

    matrix_dst->csc_coef10 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef10 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000 +
                                       (hi_s32)(matrix_ori->csc_coef20 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * sat_adj * rgain) / PQ_MULT_1000000;
    matrix_dst->csc_coef11 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef11 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000 +
                                       (hi_s32)(matrix_ori->csc_coef21 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * sat_adj * ggain) / PQ_MULT_1000000;
    matrix_dst->csc_coef12 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef12 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000 +
                                       (hi_s32)(matrix_ori->csc_coef22 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * sat_adj * bgain) / PQ_MULT_1000000;

    matrix_dst->csc_coef20 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef20 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000 -
                                       (hi_s32)(matrix_ori->csc_coef10 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * sat_adj * rgain) / PQ_MULT_1000000;
    matrix_dst->csc_coef21 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef21 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000 -
                                       (hi_s32)(matrix_ori->csc_coef11 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * sat_adj * ggain) / PQ_MULT_1000000;
    matrix_dst->csc_coef22 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef22 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000 -
                                       (hi_s32)(matrix_ori->csc_coef12 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * sat_adj * bgain) / PQ_MULT_1000000;

    return;
}

/* for y_cb_cr -> RGB */
static hi_void pq_mng_calc_csc_coef_ycb_crto_rgb(pq_gfx_csc_tuning *gfx_csc_tuning,
    const hi_drv_pq_gfx_csc_ac_coef *matrix_ori, hi_drv_pq_gfx_csc_ac_coef *matrix_dst)
{
    hi_s32 contrst_adj, sat_adj, rgain, ggain, bgain;
    hi_u32 hue;

    /* 0-100 */
    contrst_adj = (hi_s32)(gfx_csc_tuning->contrst) - PQ_CSC_PICTURE_DEFAULT;
    contrst_adj = 2 * contrst_adj + PQ_CSC_PICTURE_MAX;  /* 2: multi */
    sat_adj = (hi_s32)(gfx_csc_tuning->satur) - PQ_CSC_PICTURE_DEFAULT;
    sat_adj = 2 * sat_adj + PQ_CSC_PICTURE_MAX;   /* 2: multi */
    hue = gfx_csc_tuning->hue * 60 / PQ_CSC_PICTURE_MAX; /* 60: multi */

    rgain = (hi_s32)(gfx_csc_tuning->kr) - PQ_CSC_PICTURE_DEFAULT;
    rgain = 2 * rgain + PQ_CSC_PICTURE_MAX;  /* 2: rgb gain */
    ggain = (hi_s32)(gfx_csc_tuning->kg) - PQ_CSC_PICTURE_DEFAULT;
    ggain = 2 * ggain + PQ_CSC_PICTURE_MAX; /* 2: rgb gain */
    bgain = (hi_s32)(gfx_csc_tuning->kb) - PQ_CSC_PICTURE_DEFAULT;
    bgain = 2 * bgain + PQ_CSC_PICTURE_MAX; /* 2: rgb gain */

    if (hue >= PQ_GFX_TABLE_MAX) {
        return;
    }

    matrix_dst->csc_coef00 = matrix_ori->csc_coef00 * contrst_adj * rgain / PQ_MULT_10000;
    matrix_dst->csc_coef01 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef01 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000 -
    ((hi_s32)(matrix_ori->csc_coef02 * g_pq_gfx_sin_table[hue])) * sat_adj / PQ_MULT_100000) *
    contrst_adj * rgain) / PQ_MULT_10000;
    matrix_dst->csc_coef02 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef01 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000 +
    ((hi_s32)(matrix_ori->csc_coef02 * g_pq_gfx_cos_table[hue])) * sat_adj / PQ_MULT_100000) *
    contrst_adj * rgain) / PQ_MULT_10000;

    matrix_dst->csc_coef10 = matrix_ori->csc_coef10 * contrst_adj * ggain / PQ_MULT_10000;
    matrix_dst->csc_coef11 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef11 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000 -
                                       (hi_s32)(matrix_ori->csc_coef12 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * sat_adj * ggain) / PQ_MULT_1000000;
    matrix_dst->csc_coef12 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef11 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000 +
                                       (hi_s32)(matrix_ori->csc_coef12 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * sat_adj * ggain) / PQ_MULT_1000000;

    matrix_dst->csc_coef20 = matrix_ori->csc_coef20 * contrst_adj * bgain / PQ_MULT_10000;
    matrix_dst->csc_coef21 = ((((hi_s32)(matrix_ori->csc_coef21 * g_pq_gfx_cos_table[hue])) * sat_adj / PQ_MULT_100000 -
                                       (hi_s32)(matrix_ori->csc_coef22 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * bgain) / PQ_MULT_10000;
    matrix_dst->csc_coef22 = ((((hi_s32)(matrix_ori->csc_coef21 * g_pq_gfx_sin_table[hue])) * sat_adj / PQ_MULT_100000 -
                                       (hi_s32)(matrix_ori->csc_coef22 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * bgain) / PQ_MULT_10000;

    return;
}

hi_s32 y_cb_cr_colr_calc(pq_gfx_csc_tuning *gfx_csc_tuning, hi_drv_pq_gfx_csc_ac_coef *y_cb_cr_colr)
{
    hi_s32 rgain, ggain, bgain;
    hi_drv_pq_gfx_csc_ac_coef y_cb_cr_colr_temp;

    PQ_CHECK_NULL_PTR_RE_FAIL(gfx_csc_tuning);
    PQ_CHECK_NULL_PTR_RE_FAIL(y_cb_cr_colr);

    /* 0-100 */
    rgain = (hi_s32)(gfx_csc_tuning->kr) - PQ_CSC_PICTURE_DEFAULT;
    rgain = 2 * rgain + PQ_CSC_PICTURE_MAX; /* 2: rgb gain */
    ggain = (hi_s32)(gfx_csc_tuning->kg) - PQ_CSC_PICTURE_DEFAULT;
    ggain = 2 * ggain + PQ_CSC_PICTURE_MAX; /* 2: rgb gain */
    bgain = (hi_s32)(gfx_csc_tuning->kb) - PQ_CSC_PICTURE_DEFAULT;
    bgain = 2 * bgain + PQ_CSC_PICTURE_MAX; /* 2: rgb gain */

    /* kr系列精度100, 色温矩阵系数精度1000 */
    /* 114/587/299: rgb gain */
    y_cb_cr_colr_temp.csc_coef00 = (114 * bgain + 587 * ggain + 299 * rgain) / PQ_CSC_PICTURE_MAX;
    y_cb_cr_colr_temp.csc_coef01 = (232 * bgain - 232 * ggain) / PQ_CSC_PICTURE_MAX; /* 232 rgb gain */
    y_cb_cr_colr_temp.csc_coef02 = -(341 * ggain - 341 * rgain) / PQ_CSC_PICTURE_MAX; /* 341 rgb gain */

    /* 289/436/147 : rgb gain */
    y_cb_cr_colr_temp.csc_coef10 = -(289 * ggain - 436 * bgain + 147 * rgain) / PQ_CSC_PICTURE_MAX;
    y_cb_cr_colr_temp.csc_coef11 = (886 * bgain + 114 * ggain) / PQ_CSC_PICTURE_MAX; /* 886/114 rgb gain */
    y_cb_cr_colr_temp.csc_coef12 = (168 * ggain - 168 * rgain) / PQ_CSC_PICTURE_MAX; /* 168 rgb gain */

    /* 515/615 : rgb gain */
    y_cb_cr_colr_temp.csc_coef20 = -(PQ_CSC_PICTURE_MAX * bgain + 515 * ggain - 615 * rgain) / PQ_CSC_PICTURE_MAX;
    y_cb_cr_colr_temp.csc_coef21 = -(203 * bgain - 203 * ggain) / PQ_CSC_PICTURE_MAX; /* 203 rgb gain */
    y_cb_cr_colr_temp.csc_coef22 = (299 * ggain + 701 * rgain) / PQ_CSC_PICTURE_MAX; /* 299/701 rgb gain */

    *y_cb_cr_colr = y_cb_cr_colr_temp;

    return HI_SUCCESS;
}

/* 计算色温变换矩阵(for y_cb_cr->y_cb_cr 调节色温) */
static hi_void pq_mng_calc_csc_tmprt_mat(pq_gfx_csc_tuning *gfx_csc_tuning, hi_drv_pq_gfx_csc_ac_coef *matrix_ori,
    hi_drv_pq_gfx_csc_ac_coef *matrix_dst)
{
    hi_s32 ret;

    hi_drv_pq_gfx_csc_ac_coef y_cb_cr_colr_temp;

    ret = y_cb_cr_colr_calc(gfx_csc_tuning, &y_cb_cr_colr_temp);
    PQ_CHECK_SUCCESS_RETURN_NULL(ret);

    matrix_dst->csc_coef00 = (matrix_ori->csc_coef00 * y_cb_cr_colr_temp.csc_coef00 +
                              matrix_ori->csc_coef01 * y_cb_cr_colr_temp.csc_coef10 +
                              matrix_ori->csc_coef02 * y_cb_cr_colr_temp.csc_coef20) / PQ_MULT_1000;
    matrix_dst->csc_coef01 = (matrix_ori->csc_coef00 * y_cb_cr_colr_temp.csc_coef01 +
                              matrix_ori->csc_coef01 * y_cb_cr_colr_temp.csc_coef11 +
                              matrix_ori->csc_coef02 * y_cb_cr_colr_temp.csc_coef21) / PQ_MULT_1000;
    matrix_dst->csc_coef02 = (matrix_ori->csc_coef00 * y_cb_cr_colr_temp.csc_coef02 +
                              matrix_ori->csc_coef01 * y_cb_cr_colr_temp.csc_coef12 +
                              matrix_ori->csc_coef02 * y_cb_cr_colr_temp.csc_coef22) / PQ_MULT_1000;

    matrix_dst->csc_coef10 = (matrix_ori->csc_coef10 * y_cb_cr_colr_temp.csc_coef00 +
                              matrix_ori->csc_coef11 * y_cb_cr_colr_temp.csc_coef10 +
                              matrix_ori->csc_coef12 * y_cb_cr_colr_temp.csc_coef20) / PQ_MULT_1000;
    matrix_dst->csc_coef11 = (matrix_ori->csc_coef10 * y_cb_cr_colr_temp.csc_coef01 +
                              matrix_ori->csc_coef11 * y_cb_cr_colr_temp.csc_coef11 +
                              matrix_ori->csc_coef12 * y_cb_cr_colr_temp.csc_coef21) / PQ_MULT_1000;
    matrix_dst->csc_coef12 = (matrix_ori->csc_coef10 * y_cb_cr_colr_temp.csc_coef02 +
                              matrix_ori->csc_coef11 * y_cb_cr_colr_temp.csc_coef12 +
                              matrix_ori->csc_coef12 * y_cb_cr_colr_temp.csc_coef22) / PQ_MULT_1000;

    matrix_dst->csc_coef20 = (matrix_ori->csc_coef20 * y_cb_cr_colr_temp.csc_coef00 +
                              matrix_ori->csc_coef21 * y_cb_cr_colr_temp.csc_coef10 +
                              matrix_ori->csc_coef22 * y_cb_cr_colr_temp.csc_coef20) / PQ_MULT_1000;
    matrix_dst->csc_coef21 = (matrix_ori->csc_coef20 * y_cb_cr_colr_temp.csc_coef01 +
                              matrix_ori->csc_coef21 * y_cb_cr_colr_temp.csc_coef11 +
                              matrix_ori->csc_coef22 * y_cb_cr_colr_temp.csc_coef21) / PQ_MULT_1000;
    matrix_dst->csc_coef22 = (matrix_ori->csc_coef20 * y_cb_cr_colr_temp.csc_coef02 +
                              matrix_ori->csc_coef21 * y_cb_cr_colr_temp.csc_coef12 +
                              matrix_ori->csc_coef22 * y_cb_cr_colr_temp.csc_coef22) / PQ_MULT_1000;

    return;
}

/* 计算色彩空间转换系数 for y_cb_cr->y_cb_cr */
static hi_void pq_mng_calc_csc_coef_ycb_crto_ycb_cr(pq_gfx_csc_tuning *gfx_csc_tuning,
    const hi_drv_pq_gfx_csc_ac_coef *matrix_ori, hi_drv_pq_gfx_csc_ac_coef *matrix_dst)
{
    hi_s32 contrst_adj, sat_adj;
    hi_u32 hue;
    hi_drv_pq_gfx_csc_ac_coef dst_csc_table_tmp = {0};

    /* 0-100 */
    contrst_adj = (hi_s32)(gfx_csc_tuning->contrst) - PQ_CSC_PICTURE_DEFAULT;
    contrst_adj = 2 * contrst_adj + PQ_CSC_PICTURE_MAX; /* 2: multi */
    sat_adj = (hi_s32)(gfx_csc_tuning->satur) - PQ_CSC_PICTURE_DEFAULT;
    sat_adj = 2 * sat_adj + PQ_CSC_PICTURE_MAX; /* 2: multi */
    hue = gfx_csc_tuning->hue * 60 / PQ_CSC_PICTURE_MAX; /* 60: multi */

    matrix_dst->csc_coef00 = matrix_ori->csc_coef00 * contrst_adj / PQ_CSC_PICTURE_MAX;
    matrix_dst->csc_coef01 = matrix_ori->csc_coef01 * contrst_adj / PQ_CSC_PICTURE_MAX;
    matrix_dst->csc_coef02 = matrix_ori->csc_coef02 * contrst_adj / PQ_CSC_PICTURE_MAX;

    if (hue >= PQ_GFX_TABLE_MAX) {
        return;
    }

    matrix_dst->csc_coef10 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef10 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000 +
                                       (hi_s32)(matrix_ori->csc_coef20 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * sat_adj) / PQ_MULT_10000;
    matrix_dst->csc_coef11 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef11 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000 +
                                       (hi_s32)(matrix_ori->csc_coef21 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * sat_adj) / PQ_MULT_10000;
    matrix_dst->csc_coef12 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef12 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000 +
                                       (hi_s32)(matrix_ori->csc_coef22 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * sat_adj) / PQ_MULT_10000;

    matrix_dst->csc_coef20 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef20 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000 -
                                       (hi_s32)(matrix_ori->csc_coef10 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * sat_adj) / PQ_MULT_10000;
    matrix_dst->csc_coef21 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef21 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000 -
                                       (hi_s32)(matrix_ori->csc_coef11 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * sat_adj) / PQ_MULT_10000;
    matrix_dst->csc_coef22 = (hi_s32)(((hi_s32)(matrix_ori->csc_coef22 * g_pq_gfx_cos_table[hue]) / PQ_MULT_1000 -
                                       (hi_s32)(matrix_ori->csc_coef12 * g_pq_gfx_sin_table[hue]) / PQ_MULT_1000) *
                                      contrst_adj * sat_adj) / PQ_MULT_10000;

    pq_mng_calc_csc_tmprt_mat(gfx_csc_tuning, matrix_dst, &dst_csc_table_tmp);

    matrix_dst->csc_coef00 = dst_csc_table_tmp.csc_coef00;
    matrix_dst->csc_coef01 = dst_csc_table_tmp.csc_coef01;
    matrix_dst->csc_coef02 = dst_csc_table_tmp.csc_coef02;
    matrix_dst->csc_coef10 = dst_csc_table_tmp.csc_coef10;
    matrix_dst->csc_coef11 = dst_csc_table_tmp.csc_coef11;
    matrix_dst->csc_coef12 = dst_csc_table_tmp.csc_coef12;
    matrix_dst->csc_coef20 = dst_csc_table_tmp.csc_coef20;
    matrix_dst->csc_coef21 = dst_csc_table_tmp.csc_coef21;
    matrix_dst->csc_coef22 = dst_csc_table_tmp.csc_coef22;

    return;
}

/***************************************************************************************
* func          : pq_mng_calc_csc_matrix
* description   : c_ncomment: 计算CSC距阵 c_nend\n
* param[in]     : hi_void
* retval        : NA
* others:       : NA
***************************************************************************************/
/* note: 这部分的计算, 精度还是按照原来的10bits配置, 可以在运算的最后乘上精度 */
hi_void pq_mng_calc_csc_matrix(pq_gfx_csc_mode *gfx_csc_mode,
                               hi_drv_pq_gfx_csc_ac_coef *gfx_csc_coef, hi_bool is_bgr_in)
{
    const hi_drv_pq_gfx_csc_ac_coef *csc_matrix_ori = HI_NULL;
    hi_drv_pq_gfx_csc_ac_coef csc_tbl = { 0 };

    if ((gfx_csc_mode == HI_NULL) || (gfx_csc_coef == HI_NULL)) {
        return;
    }

    if ((gfx_csc_mode->gfx_csc_csc_in.cs_type >= PQ_GFX_CS_TYPE_MAX)
        || (gfx_csc_mode->gfx_csc_csc_out.cs_type >= PQ_GFX_CS_TYPE_MAX)) {
        return;
    }

    csc_matrix_ori =
        g_pq_gfx_csc_matrix_tbl[gfx_csc_mode->gfx_csc_csc_in.cs_type][gfx_csc_mode->gfx_csc_csc_out.cs_type];
    /* correct the 3*3 matrix dci_ording to the value range of input and output of st_csc_tbl */
    if (gfx_csc_mode->gfx_csc_csc_in.cs_type >= PQ_GFX_CS_ERGB_709) {      /* input color space RGB */
        if (gfx_csc_mode->gfx_csc_csc_out.cs_type >= PQ_GFX_CS_ERGB_709) { /* RGB -> RGB */
            pq_mng_calc_csc_coef_rgbto_rgb(&g_gfx_csc_tuning, csc_matrix_ori, &csc_tbl);
        } else { /* RGB -> y_cb_cr */
            pq_mng_calc_csc_coef_rgbto_ycb_cr(&g_gfx_csc_tuning, csc_matrix_ori, &csc_tbl);
        }
    } else {                                                                 /* input color space y_cb_cr */
        if (gfx_csc_mode->gfx_csc_csc_out.cs_type >= PQ_GFX_CS_ERGB_709) { /* y_cb_cr -> RGB */
            pq_mng_calc_csc_coef_ycb_crto_rgb(&g_gfx_csc_tuning, csc_matrix_ori, &csc_tbl);
        } else { /* y_cb_cr -> y_cb_cr */
            pq_mng_calc_csc_coef_ycb_crto_ycb_cr(&g_gfx_csc_tuning, csc_matrix_ori, &csc_tbl);
        }
    }
    if (is_bgr_in == HI_FALSE) {
        gfx_csc_coef->csc_coef00 = csc_tbl.csc_coef00;
        gfx_csc_coef->csc_coef01 = csc_tbl.csc_coef01;
        gfx_csc_coef->csc_coef02 = csc_tbl.csc_coef02;
        gfx_csc_coef->csc_coef10 = csc_tbl.csc_coef10;
        gfx_csc_coef->csc_coef11 = csc_tbl.csc_coef11;
        gfx_csc_coef->csc_coef12 = csc_tbl.csc_coef12;
        gfx_csc_coef->csc_coef20 = csc_tbl.csc_coef20;
        gfx_csc_coef->csc_coef21 = csc_tbl.csc_coef21;
        gfx_csc_coef->csc_coef22 = csc_tbl.csc_coef22;
    } else {
        gfx_csc_coef->csc_coef00 = csc_tbl.csc_coef02;
        gfx_csc_coef->csc_coef01 = csc_tbl.csc_coef01;
        gfx_csc_coef->csc_coef02 = csc_tbl.csc_coef00;
        gfx_csc_coef->csc_coef10 = csc_tbl.csc_coef12;
        gfx_csc_coef->csc_coef11 = csc_tbl.csc_coef11;
        gfx_csc_coef->csc_coef12 = csc_tbl.csc_coef10;
        gfx_csc_coef->csc_coef20 = csc_tbl.csc_coef22;
        gfx_csc_coef->csc_coef21 = csc_tbl.csc_coef21;
        gfx_csc_coef->csc_coef22 = csc_tbl.csc_coef20;
    }

    return;
}

hi_void pq_mng_calc_rgb2020_matrix(pq_gfx_cs_type gfx_output_type, hi_drv_pq_gfx_csc_ac_coef *gfx_csc_coef)
{
    PQ_CHECK_NULL_PTR_RE_NULL(gfx_csc_coef);

    if (gfx_output_type == PQ_GFX_CS_EYCBCR_2020) { /* RGB_2020 -> y_cb_cr_2020 */
        gfx_csc_coef->csc_coef00 = (hi_s32)0.2256; /* 0.2256 coef */
        gfx_csc_coef->csc_coef01 = (hi_s32)0.5823; /* 0.5823 coef */
        gfx_csc_coef->csc_coef02 = (hi_s32)0.0509; /* 0.0509 coef */
        gfx_csc_coef->csc_coef10 = (hi_s32)-0.1227; /* -0.1227 coef */
        gfx_csc_coef->csc_coef11 = (hi_s32)-0.3166; /* -0.3166 coef */
        gfx_csc_coef->csc_coef12 = (hi_s32)0.4392; /* 0.4392 coef */
        gfx_csc_coef->csc_coef20 = (hi_s32)0.4392; /* 0.4392 coef */
        gfx_csc_coef->csc_coef21 = (hi_s32)-0.4039; /* -0.4039 coef */
        gfx_csc_coef->csc_coef22 = (hi_s32)-0.0353; /* -0.0353 coef */
    }  else if ((gfx_output_type == PQ_GFX_CS_ERGB_709) ||
                (gfx_output_type == PQ_GFX_CS_ERGB_601)) { /* RGB_2020 -> RGB_709 */
        gfx_csc_coef->csc_coef00 = (hi_s32)1.6605; /* 1.6605 coef */
        gfx_csc_coef->csc_coef01 = (hi_s32)-0.5877; /* -0.5877 coef */
        gfx_csc_coef->csc_coef02 = (hi_s32)-0.0728; /* -0.0728 coef */
        gfx_csc_coef->csc_coef10 = (hi_s32)-0.1246; /* -0.1246 coef */
        gfx_csc_coef->csc_coef11 = (hi_s32)1.1329; /* 1.1329 coef */
        gfx_csc_coef->csc_coef12 = (hi_s32)-0.0083; /* -0.0083 coef */
        gfx_csc_coef->csc_coef20 = (hi_s32)-0.0182; /* -0.0182 coef */
        gfx_csc_coef->csc_coef21 = (hi_s32)-0.1006; /* -0.1006 coef */
        gfx_csc_coef->csc_coef22 = (hi_s32)1.1188; /* 1.1188 coef */
    } else { /* identity */
        gfx_csc_coef->csc_coef00 = 1;
        gfx_csc_coef->csc_coef01 = 0;
        gfx_csc_coef->csc_coef02 = 0;
        gfx_csc_coef->csc_coef10 = 0;
        gfx_csc_coef->csc_coef11 = 1;
        gfx_csc_coef->csc_coef12 = 0;
        gfx_csc_coef->csc_coef20 = 0;
        gfx_csc_coef->csc_coef21 = 0;
        gfx_csc_coef->csc_coef22 = 1;
    }

    return;
}

hi_void pq_mng_calc_rgb709_matrix(pq_gfx_cs_type gfx_output_type, hi_drv_pq_gfx_csc_ac_coef *gfx_csc_coef)
{
    PQ_CHECK_NULL_PTR_RE_NULL(gfx_csc_coef);

    if (gfx_output_type == PQ_GFX_CS_EYCBCR_709) { /* RGB_709 -> y_cb_cr_709 */
        gfx_csc_coef->csc_coef00 = (hi_s32)0.1826; /* 0.1826 coef */
        gfx_csc_coef->csc_coef01 = (hi_s32)0.6142; /* 0.6142 coef */
        gfx_csc_coef->csc_coef02 = (hi_s32)0.0620; /* 0.0620 coef */
        gfx_csc_coef->csc_coef10 = (hi_s32)-0.1006; /* -0.1006 coef */
        gfx_csc_coef->csc_coef11 = (hi_s32)-0.3386; /* -0.3386 coef */
        gfx_csc_coef->csc_coef12 = (hi_s32)0.4392; /* 0.4392 coef */
        gfx_csc_coef->csc_coef20 = (hi_s32)0.4392; /* 0.4392 coef */
        gfx_csc_coef->csc_coef21 = (hi_s32)-0.3989; /* -0.3989 coef */
        gfx_csc_coef->csc_coef22 = (hi_s32)-0.0403; /* -0.0403 coef */
    } else if (gfx_output_type == PQ_GFX_CS_ERGB_2020) { /* RGB_709 -> RGB_2020 */
        gfx_csc_coef->csc_coef00 = (hi_s32)0.6274; /* 0.6274 coef */
        gfx_csc_coef->csc_coef01 = (hi_s32)0.3293; /* 0.3293 coef */
        gfx_csc_coef->csc_coef02 = (hi_s32)0.0433; /* 0.0433 coef */
        gfx_csc_coef->csc_coef10 = (hi_s32)0.0692; /* 0.0692 coef */
        gfx_csc_coef->csc_coef11 = (hi_s32)0.9194; /* 0.9194 coef */
        gfx_csc_coef->csc_coef12 = (hi_s32)0.0114; /* 0.0114 coef */
        gfx_csc_coef->csc_coef20 = (hi_s32)0.0163; /* 0.0163 coef */
        gfx_csc_coef->csc_coef21 = (hi_s32)0.0880; /* 0.0880 coef */
        gfx_csc_coef->csc_coef22 = (hi_s32)0.8956; /* 0.8956 coef */
    } else { /* identity */
        gfx_csc_coef->csc_coef00 = 1;
        gfx_csc_coef->csc_coef01 = 0;
        gfx_csc_coef->csc_coef02 = 0;
        gfx_csc_coef->csc_coef10 = 0;
        gfx_csc_coef->csc_coef11 = 1;
        gfx_csc_coef->csc_coef12 = 0;
        gfx_csc_coef->csc_coef20 = 0;
        gfx_csc_coef->csc_coef21 = 0;
        gfx_csc_coef->csc_coef22 = 1;
    }

    return;
}

hi_void pq_mng_calc_rgb601_matrix(pq_gfx_cs_type gfx_output_type, hi_drv_pq_gfx_csc_ac_coef *gfx_csc_coef)
{
    PQ_CHECK_NULL_PTR_RE_NULL(gfx_csc_coef);

    if (gfx_output_type == PQ_GFX_CS_ERGB_2020) {  /* RGB_601 -> RGB_2020; same to RGB_709 -> RGB_2020 */
        gfx_csc_coef->csc_coef00 = (hi_s32)0.6274; /* 0.6274 coef */
        gfx_csc_coef->csc_coef01 = (hi_s32)0.3293; /* 0.3293 coef */
        gfx_csc_coef->csc_coef02 = (hi_s32)0.0433; /* 0.0433 coef */
        gfx_csc_coef->csc_coef10 = (hi_s32)0.0692; /* 0.0692 coef */
        gfx_csc_coef->csc_coef11 = (hi_s32)0.9194; /* 0.9194 coef */
        gfx_csc_coef->csc_coef12 = (hi_s32)0.0114; /* 0.0114 coef */
        gfx_csc_coef->csc_coef20 = (hi_s32)0.0163; /* 0.0163 coef */
        gfx_csc_coef->csc_coef21 = (hi_s32)0.0880; /* 0.0880 coef */
        gfx_csc_coef->csc_coef22 = (hi_s32)0.8956; /* 0.8956 coef */
    } else { /* null */
        return;
    }

    return;
}
/***************************************************************************************
* func          : pq_mng_calc_wcg_matrix
* description   : c_ncomment: 计算CSC距阵 c_nend\n
* param[in]     : hi_void
* retval        : NA
* others:       : NA
***************************************************************************************/
hi_void pq_mng_calc_wcg_matrix(pq_gfx_csc_mode *gfx_csc_mode, hi_drv_pq_gfx_csc_ac_coef *gfx_csc_coef)
{
    pq_gfx_cs_type gfx_input_type, gfx_output_type;
    gfx_input_type = gfx_csc_mode->gfx_csc_csc_in.cs_type;
    gfx_output_type = gfx_csc_mode->gfx_csc_csc_out.cs_type;

    /* list all possible condition; note: when RGB -> y_cb_cr, there in not tran like "709/601 <--> 2020" */
    if (gfx_input_type == PQ_GFX_CS_ERGB_2020) {        /* input RGB_2020 */
        pq_mng_calc_rgb2020_matrix(gfx_output_type, gfx_csc_coef);
    } else if (gfx_input_type == PQ_GFX_CS_ERGB_709) { /* input RGB_709 */
        pq_mng_calc_rgb709_matrix(gfx_output_type, gfx_csc_coef);
    } else if (gfx_input_type == PQ_GFX_CS_ERGB_601) { /* input RGB_601; do not insure effect */
        pq_mng_calc_rgb601_matrix(gfx_output_type, gfx_csc_coef);
    } else { /* identity; impossible condition: input y_cb_cr_709; input y_cb_cr_601; input y_cb_cr_2020 */
        gfx_csc_coef->csc_coef00 = 1;
        gfx_csc_coef->csc_coef01 = 0;
        gfx_csc_coef->csc_coef02 = 0;
        gfx_csc_coef->csc_coef10 = 0;
        gfx_csc_coef->csc_coef11 = 1;
        gfx_csc_coef->csc_coef12 = 0;
        gfx_csc_coef->csc_coef20 = 0;
        gfx_csc_coef->csc_coef21 = 0;
        gfx_csc_coef->csc_coef22 = 1;
    }

    gfx_csc_coef->csc_coef00 = (hi_s32)(gfx_csc_coef->csc_coef00 * CSC_PRE);
    gfx_csc_coef->csc_coef01 = (hi_s32)(gfx_csc_coef->csc_coef01 * CSC_PRE);
    gfx_csc_coef->csc_coef02 = (hi_s32)(gfx_csc_coef->csc_coef02 * CSC_PRE);
    gfx_csc_coef->csc_coef10 = (hi_s32)(gfx_csc_coef->csc_coef10 * CSC_PRE);
    gfx_csc_coef->csc_coef11 = (hi_s32)(gfx_csc_coef->csc_coef11 * CSC_PRE);
    gfx_csc_coef->csc_coef12 = (hi_s32)(gfx_csc_coef->csc_coef12 * CSC_PRE);
    gfx_csc_coef->csc_coef20 = (hi_s32)(gfx_csc_coef->csc_coef20 * CSC_PRE);
    gfx_csc_coef->csc_coef21 = (hi_s32)(gfx_csc_coef->csc_coef21 * CSC_PRE);
    gfx_csc_coef->csc_coef22 = (hi_s32)(gfx_csc_coef->csc_coef22 * CSC_PRE);

    return;
}

static hi_void pq_mng_calc_wcg_dc_yuv2_yuv(pq_gfx_csc_mode *csc_mode, hi_drv_pq_gfx_csc_dc_coef *gfx_csc_dc_coef)
{
    pq_gfx_cs_range gfx_input_range = csc_mode->gfx_csc_csc_in.cs_range;
    pq_gfx_cs_range gfx_output_range = csc_mode->gfx_csc_csc_out.cs_range;

    switch (gfx_input_range) {     /* input DC */
        case PQ_GFX_CS_RANGE_FULL: /* YUV_F -> YUV_FL */
            gfx_csc_dc_coef->csc_in_dc0 = 0;
            gfx_csc_dc_coef->csc_in_dc1 = -128; /* -128 coef */
            gfx_csc_dc_coef->csc_in_dc2 = -128; /* -128 coef */
            break;
        case PQ_GFX_CS_RANGE_LMTD: /* YUV_L -> YUV_FL */
            gfx_csc_dc_coef->csc_in_dc0 = -16; /* -16 coef */
            gfx_csc_dc_coef->csc_in_dc1 = -128; /* -128 coef */
            gfx_csc_dc_coef->csc_in_dc2 = -128; /* -128 coef */
            break;
        default:
            gfx_csc_dc_coef->csc_in_dc0 = -16; /* -16 coef */
            gfx_csc_dc_coef->csc_in_dc1 = -128; /* -128 coef */
            gfx_csc_dc_coef->csc_in_dc2 = -128; /* -128 coef */
            break;
    }

    switch (gfx_output_range) {    /* output DC */
        case PQ_GFX_CS_RANGE_FULL: /* YUV_FL -> YUV_F */
            gfx_csc_dc_coef->csc_out_dc0 = 0;
            gfx_csc_dc_coef->csc_out_dc1 = 128; /* 128 coef */
            gfx_csc_dc_coef->csc_out_dc2 = 128; /* 128 coef */
            break;
        case PQ_GFX_CS_RANGE_LMTD: /* YUV_FL -> YUV_L */
            gfx_csc_dc_coef->csc_out_dc0 = 16; /* 16 coef */
            gfx_csc_dc_coef->csc_out_dc1 = 128; /* 128 coef */
            gfx_csc_dc_coef->csc_out_dc2 = 128; /* 128 coef */
            break;
        default:
            gfx_csc_dc_coef->csc_out_dc0 = 16; /* 16 coef */
            gfx_csc_dc_coef->csc_out_dc1 = 128; /* 128 coef */
            gfx_csc_dc_coef->csc_out_dc2 = 128; /* 128 coef */
            break;
    }

    return;
}

static hi_void pq_mng_calc_wcg_dc_yuv2_rgb(pq_gfx_csc_mode *csc_mode, hi_drv_pq_gfx_csc_dc_coef *gfx_csc_dc_coef)
{
    pq_gfx_cs_range gfx_input_range = csc_mode->gfx_csc_csc_in.cs_range;
    pq_gfx_cs_range gfx_output_range = csc_mode->gfx_csc_csc_out.cs_range;

    switch (gfx_input_range) {     /* input DC */
        case PQ_GFX_CS_RANGE_FULL: /* YUV_F -> RGB_FL */
            gfx_csc_dc_coef->csc_in_dc0 = 0;
            gfx_csc_dc_coef->csc_in_dc1 = -128; /* -128 coef */
            gfx_csc_dc_coef->csc_in_dc2 = -128; /* -128 coef */
            break;
        case PQ_GFX_CS_RANGE_LMTD: /* YUV_L -> RGB_FL */
            gfx_csc_dc_coef->csc_in_dc0 = -16; /* -16 coef */
            gfx_csc_dc_coef->csc_in_dc1 = -128; /* -128 coef */
            gfx_csc_dc_coef->csc_in_dc2 = -128; /* -128 coef */
            break;
        default:
            gfx_csc_dc_coef->csc_in_dc0 = -16; /* -16 coef */
            gfx_csc_dc_coef->csc_in_dc1 = -128; /* -128 coef */
            gfx_csc_dc_coef->csc_in_dc2 = -128; /* -128 coef */
            break;
    }

    switch (gfx_output_range) {    /* output DC */
        case PQ_GFX_CS_RANGE_FULL: /* YUV_FL -> RGB_F */
            gfx_csc_dc_coef->csc_out_dc0 = 0;
            gfx_csc_dc_coef->csc_out_dc1 = 0;
            gfx_csc_dc_coef->csc_out_dc2 = 0;
            break;
        case PQ_GFX_CS_RANGE_LMTD: /* YUV_FL -> RGB_L */
            gfx_csc_dc_coef->csc_out_dc0 = 16; /* 16 coef */
            gfx_csc_dc_coef->csc_out_dc1 = 16; /* 16 coef */
            gfx_csc_dc_coef->csc_out_dc2 = 16; /* 16 coef */
            break;
        default:
            gfx_csc_dc_coef->csc_out_dc0 = 0;
            gfx_csc_dc_coef->csc_out_dc1 = 0;
            gfx_csc_dc_coef->csc_out_dc2 = 0;
            break;
    }

    return;
}

static hi_void pq_mng_calc_wcg_dc_rgb2_yuv(pq_gfx_csc_mode *csc_mode, hi_drv_pq_gfx_csc_dc_coef *gfx_csc_dc_coef)
{
    pq_gfx_cs_range gfx_input_range = csc_mode->gfx_csc_csc_in.cs_range;
    pq_gfx_cs_range gfx_output_range = csc_mode->gfx_csc_csc_out.cs_range;

    switch (gfx_input_range) {     /* input DC */
        case PQ_GFX_CS_RANGE_FULL: /* RGB_F -> YUV_FL */
            gfx_csc_dc_coef->csc_in_dc0 = 0;
            gfx_csc_dc_coef->csc_in_dc1 = 0;
            gfx_csc_dc_coef->csc_in_dc2 = 0;
            break;
        case PQ_GFX_CS_RANGE_LMTD: /* RGB_L -> YUV_FL */
            gfx_csc_dc_coef->csc_in_dc0 = 16; /* 16 coef */
            gfx_csc_dc_coef->csc_in_dc1 = 16; /* 16 coef */
            gfx_csc_dc_coef->csc_in_dc2 = 16; /* 16 coef */
            break;
        default:
            gfx_csc_dc_coef->csc_in_dc0 = 0;
            gfx_csc_dc_coef->csc_in_dc1 = 0;
            gfx_csc_dc_coef->csc_in_dc2 = 0;
            break;
    }

    switch (gfx_output_range) {    /* output DC */
        case PQ_GFX_CS_RANGE_FULL: /* RGB_FL -> YUV_F */
            gfx_csc_dc_coef->csc_out_dc0 = 0;
            gfx_csc_dc_coef->csc_out_dc1 = 128; /* 128 coef */
            gfx_csc_dc_coef->csc_out_dc2 = 128; /* 128 coef */
            break;
        case PQ_GFX_CS_RANGE_LMTD: /* RGB_FL -> YUV_L */
            gfx_csc_dc_coef->csc_out_dc0 = 16; /* 16 coef */
            gfx_csc_dc_coef->csc_out_dc1 = 128; /* 128 coef */
            gfx_csc_dc_coef->csc_out_dc2 = 128; /* 128 coef */
            break;
        default:
            gfx_csc_dc_coef->csc_out_dc0 = 16; /* 16 coef */
            gfx_csc_dc_coef->csc_out_dc1 = 128; /* 128 coef */
            gfx_csc_dc_coef->csc_out_dc2 = 128; /* 128 coef */
            break;
    }

    return;
}

static hi_void pq_mng_calc_wcg_dc_rgb2_rgb(pq_gfx_csc_mode *csc_mode, hi_drv_pq_gfx_csc_dc_coef *gfx_csc_dc_coef)
{
    pq_gfx_cs_range gfx_input_range = csc_mode->gfx_csc_csc_in.cs_range;
    pq_gfx_cs_range gfx_output_range = csc_mode->gfx_csc_csc_out.cs_range;

    switch (gfx_input_range) {     /* input DC */
        case PQ_GFX_CS_RANGE_FULL: /* RGB_F -> RGB_FL */
            gfx_csc_dc_coef->csc_in_dc0 = 0;
            gfx_csc_dc_coef->csc_in_dc1 = 0;
            gfx_csc_dc_coef->csc_in_dc2 = 0;
            break;
        case PQ_GFX_CS_RANGE_LMTD: /* RGB_L -> RGB_FL */
            gfx_csc_dc_coef->csc_in_dc0 = 16; /* 16 coef */
            gfx_csc_dc_coef->csc_in_dc1 = 16; /* 16 coef */
            gfx_csc_dc_coef->csc_in_dc2 = 16; /* 16 coef */
            break;
        default:
            gfx_csc_dc_coef->csc_in_dc0 = 0;
            gfx_csc_dc_coef->csc_in_dc1 = 0;
            gfx_csc_dc_coef->csc_in_dc2 = 0;
            break;
    }

    switch (gfx_output_range) {    /* output DC */
        case PQ_GFX_CS_RANGE_FULL: /* RGB_FL -> RGB_F */
            gfx_csc_dc_coef->csc_out_dc0 = 0;
            gfx_csc_dc_coef->csc_out_dc1 = 0;
            gfx_csc_dc_coef->csc_out_dc2 = 0;
            break;
        case PQ_GFX_CS_RANGE_LMTD: /* RGB_FL -> RGB_L */
            gfx_csc_dc_coef->csc_out_dc0 = 16; /* 16 coef */
            gfx_csc_dc_coef->csc_out_dc1 = 16; /* 16 coef */
            gfx_csc_dc_coef->csc_out_dc2 = 16; /* 16 coef */
            break;
        default:
            gfx_csc_dc_coef->csc_out_dc0 = 0;
            gfx_csc_dc_coef->csc_out_dc1 = 0;
            gfx_csc_dc_coef->csc_out_dc2 = 0;
            break;
    }

    return;
}

/***************************************************************************************
* func          : pq_mng_calc_wcg_dc
* description   : c_ncomment: 计算DC系数 c_nend\n
* param[in]     : hi_void
* retval        : NA
* others:       : NA
***************************************************************************************/
hi_void pq_mng_calc_wcg_dc(pq_gfx_csc_mode *csc_mode, hi_drv_pq_gfx_csc_dc_coef *gfx_csc_dc_coef)
{
    pq_gfx_cs_type gfx_input_type = csc_mode->gfx_csc_csc_in.cs_type;
    pq_gfx_cs_type gfx_output_type = csc_mode->gfx_csc_csc_out.cs_type;

    /* list all possible condition */
    if (gfx_input_type >= PQ_GFX_CS_EYCBCR_709 && gfx_input_type <= PQ_GFX_CS_EYCBCR_2020) { /* input YUV color space */
        if (gfx_output_type >= PQ_GFX_CS_EYCBCR_709 && gfx_output_type <= PQ_GFX_CS_EYCBCR_2020) { /* YUV -> YUV */
            pq_mng_calc_wcg_dc_yuv2_yuv(csc_mode, gfx_csc_dc_coef);
        } else { /* YUV -> RGB */
            pq_mng_calc_wcg_dc_yuv2_rgb(csc_mode, gfx_csc_dc_coef);
        }
    } else { /* input RGB color space */
        if (gfx_output_type >= PQ_GFX_CS_EYCBCR_709 && gfx_output_type <= PQ_GFX_CS_EYCBCR_601) { /* RGB -> YUV */
            pq_mng_calc_wcg_dc_rgb2_yuv(csc_mode, gfx_csc_dc_coef);
        } else { /* RGB -> RGB */
            pq_mng_calc_wcg_dc_rgb2_rgb(csc_mode, gfx_csc_dc_coef);
        }
    }

    gfx_csc_dc_coef->csc_in_dc0 = (hi_s32)(gfx_csc_dc_coef->csc_in_dc0 * 4); /* 4: coef multi */
    gfx_csc_dc_coef->csc_in_dc1 = (hi_s32)(gfx_csc_dc_coef->csc_in_dc1 * 4); /* 4: coef multi */
    gfx_csc_dc_coef->csc_in_dc2 = (hi_s32)(gfx_csc_dc_coef->csc_in_dc2 * 4); /* 4: coef multi */
    gfx_csc_dc_coef->csc_out_dc0 = (hi_s32)(gfx_csc_dc_coef->csc_out_dc0 * 4); /* 4: coef multi */
    gfx_csc_dc_coef->csc_out_dc1 = (hi_s32)(gfx_csc_dc_coef->csc_out_dc1 * 4); /* 4: coef multi */
    gfx_csc_dc_coef->csc_out_dc2 = (hi_s32)(gfx_csc_dc_coef->csc_out_dc2 * 4); /* 4: coef multi */

    return;
}

hi_s32 pq_mng_get_gfx_csc_coef(drv_pq_internal_csc_info gfx_csc, hi_drv_pq_gfx_csc_coef *csc_coef)
{
    pq_gfx_csc_mode gfx_csc_mode;
    pq_gfx_cs alg_cs_in, alg_cs_out;

    /* tran mode, cs_type and cs_range */
    get_csc_type(gfx_csc.csc_in, &alg_cs_in);
    get_csc_type(gfx_csc.csc_out, &alg_cs_out);
    /* CSC input_mode and output mode */
    gfx_csc_mode.gfx_csc_csc_in.cs_type = alg_cs_in.cs_type;
    gfx_csc_mode.gfx_csc_csc_in.cs_range = alg_cs_in.cs_range;
    gfx_csc_mode.gfx_csc_csc_out.cs_type = alg_cs_out.cs_type;
    gfx_csc_mode.gfx_csc_csc_out.cs_range = alg_cs_out.cs_range;

    if (gfx_csc.csc_type == HI_PQ_CSC_NORMAL_SETTING) { /* it does not be tuned */
        /* about 2020 */ /* don not care impossible */
        if ((PQ_GFX_CS_EYCBCR_2020 == alg_cs_in.cs_type) || (PQ_GFX_CS_ERGB_2020 == alg_cs_in.cs_type)
            || (PQ_GFX_CS_EYCBCR_2020 == alg_cs_out.cs_type) || (PQ_GFX_CS_ERGB_2020 == alg_cs_out.cs_type)) {
            pq_mng_calc_wcg_dc(&gfx_csc_mode, &(csc_coef->gfx_dc_coef));
            pq_mng_calc_wcg_matrix(&gfx_csc_mode, &(csc_coef->gfx_ac_coef));
        } else { /* not about 2020 */
            pq_mng_calc_csc_dc(&gfx_csc_mode, &(csc_coef->gfx_dc_coef));
            pq_mng_calc_csc_matrix(&gfx_csc_mode, &(csc_coef->gfx_ac_coef), g_is_bgr_in);
        }
    } else { /* may need tuning; calc */
        if (HI_PQ_CSC_TUNING_GP0 == gfx_csc.csc_type) {
            g_gfx_csc_tuning.bright = g_gfxcsc_hd_pic_para.brightness;
            g_gfx_csc_tuning.contrst = g_gfxcsc_hd_pic_para.contrast;
            g_gfx_csc_tuning.hue = g_gfxcsc_hd_pic_para.hue;
            g_gfx_csc_tuning.satur = g_gfxcsc_hd_pic_para.saturation;
            g_gfx_csc_tuning.wcg_temperature = g_gfxcsc_hd_pic_para.wcg_temperature;
        } else if (HI_PQ_CSC_TUNING_GP1 == gfx_csc.csc_type) {
            g_gfx_csc_tuning.bright = g_gfxcsc_sd_pic_para.brightness;
            g_gfx_csc_tuning.contrst = g_gfxcsc_sd_pic_para.contrast;
            g_gfx_csc_tuning.hue = g_gfxcsc_sd_pic_para.hue;
            g_gfx_csc_tuning.satur = g_gfxcsc_sd_pic_para.saturation;
            g_gfx_csc_tuning.wcg_temperature = g_gfxcsc_sd_pic_para.wcg_temperature;
        }

        if (gfx_csc.csc_en == HI_TRUE) { /* reload picture setting for DTV request */
            pq_mng_calc_csc_dc(&gfx_csc_mode, &(csc_coef->gfx_dc_coef));
            pq_mng_calc_csc_matrix(&gfx_csc_mode, &(csc_coef->gfx_ac_coef), g_is_bgr_in);
        }
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_gp0_gfx_csc_trans(drv_pq_internal_csc_info gfx_csc_src, drv_pq_internal_csc_info *gfx_csc_dst)
{
    if ((HI_DRV_CS_BT709_RGB_FULL == gfx_csc_src.csc_in)
        && (HI_DRV_CS_BT709_YUV_FULL == gfx_csc_src.csc_out)) {
        gfx_csc_dst->csc_in = HI_DRV_CS_BT709_RGB_FULL;
        gfx_csc_dst->csc_out = HI_DRV_CS_BT709_YUV_FULL;
    } else if ((HI_DRV_CS_BT709_RGB_FULL == gfx_csc_src.csc_in)
               && (HI_DRV_CS_BT709_YUV_LIMITED == gfx_csc_src.csc_out)) {
        gfx_csc_dst->csc_in = HI_DRV_CS_BT709_RGB_FULL;
        gfx_csc_dst->csc_out = HI_DRV_CS_BT709_YUV_LIMITED;
    } else if ((HI_DRV_CS_BT709_RGB_FULL == gfx_csc_src.csc_in)
               && (HI_DRV_CS_BT601_YUV_LIMITED == gfx_csc_src.csc_out)) {
        gfx_csc_dst->csc_in = HI_DRV_CS_BT709_RGB_FULL;
        gfx_csc_dst->csc_out = HI_DRV_CS_BT601_YUV_LIMITED;
    } else if ((HI_DRV_CS_BT709_RGB_FULL == gfx_csc_src.csc_in)
               && (HI_DRV_CS_BT709_RGB_FULL == gfx_csc_src.csc_out)) {
        gfx_csc_dst->csc_in = HI_DRV_CS_BT709_RGB_FULL;
        gfx_csc_dst->csc_out = HI_DRV_CS_BT709_RGB_FULL;
    } else if ((HI_DRV_CS_BT709_RGB_FULL == gfx_csc_src.csc_in)
               && (HI_DRV_CS_BT601_RGB_FULL == gfx_csc_src.csc_out)) {
        gfx_csc_dst->csc_in = HI_DRV_CS_BT709_RGB_FULL;
        gfx_csc_dst->csc_out = HI_DRV_CS_BT601_RGB_FULL;
    } else {
        gfx_csc_dst->csc_in = HI_DRV_CS_BT709_RGB_FULL;
        gfx_csc_dst->csc_out = HI_DRV_CS_BT709_YUV_LIMITED;
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_gp1_gfx_csc_trans(drv_pq_internal_csc_info gfx_csc_src, drv_pq_internal_csc_info *gfx_csc_dst)
{
    if ((HI_DRV_CS_BT709_RGB_FULL == gfx_csc_src.csc_in)
        && (HI_DRV_CS_BT601_YUV_LIMITED == gfx_csc_src.csc_out)) {
        gfx_csc_dst->csc_in = HI_DRV_CS_BT709_RGB_FULL;
        gfx_csc_dst->csc_out = HI_DRV_CS_BT601_YUV_LIMITED;
    } else if ((HI_DRV_CS_BT709_RGB_FULL == gfx_csc_src.csc_in)
               && (HI_DRV_CS_BT601_YUV_FULL == gfx_csc_src.csc_out)) {
        gfx_csc_dst->csc_in = HI_DRV_CS_BT709_RGB_FULL;
        gfx_csc_dst->csc_out = HI_DRV_CS_BT601_YUV_FULL;
    } else {
        gfx_csc_dst->csc_in = HI_DRV_CS_BT709_RGB_FULL;
        gfx_csc_dst->csc_out = HI_DRV_CS_BT601_YUV_LIMITED;
    }

    return HI_SUCCESS;
}

hi_void pq_mng_proc_get_gfx_csc_info(pq_gfx_csc_proc_info *gfx_csc_out)
{
    PQ_CHECK_NULL_PTR_RE_NULL(gfx_csc_out);

    gfx_csc_out->is_bgr_in = g_gfx_csc_info.is_bgr_in;
    gfx_csc_out->csc_en = g_gfx_csc_info.csc_en;
    gfx_csc_out->csc_in = g_gfx_csc_info.csc_in;
    gfx_csc_out->csc_out = g_gfx_csc_info.csc_out;
    gfx_csc_out->csc_pecision = g_gfx_csc_info.csc_pecision;
    gfx_csc_out->csc_type = g_gfx_csc_info.csc_type;

    return;
}

hi_s32 pq_mng_get_gfx_csc(hi_drv_pq_gfx_layer gfx_layer, drv_pq_internal_gfx_csc_info *gfx_csc_info,
                          hi_drv_pq_gfx_csc_coef *gfx_csc_coef)
{
    hi_s32 ret;
    drv_pq_internal_csc_info gfx_csc;

    PQ_CHECK_OVER_RANGE_RE_FAIL(gfx_layer, HI_DRV_PQ_GFX_LAYER_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(gfx_csc_info);
    PQ_CHECK_NULL_PTR_RE_FAIL(gfx_csc_coef);

    g_is_bgr_in = gfx_csc_info->is_bgr_in;

    /* dci_ording to the input and output csc mode of layer, get the strategy of layer */
    if (gfx_layer == HI_DRV_PQ_GFX_LAYER_GP0) {
        gfx_csc.csc_en = HI_TRUE;
        gfx_csc.csc_pecision = 12;  /* 12: GP0 pecision */
        gfx_csc.csc_type = HI_PQ_CSC_TUNING_GP0; /* may need tuning */

        ret = pq_mng_gp0_gfx_csc_trans(gfx_csc_info->gfx_csc_mode, &gfx_csc);
        PQ_CHECK_RETURN_SUCCESS(ret);
    } else if (gfx_layer == HI_DRV_PQ_GFX_LAYER_GP1) {
        gfx_csc.csc_en = HI_TRUE;
        gfx_csc.csc_pecision = 12;   /* 12: GP1 pecision */
        gfx_csc.csc_type = HI_PQ_CSC_TUNING_GP1; /* may need tuning */

        ret = pq_mng_gp1_gfx_csc_trans(gfx_csc_info->gfx_csc_mode, &gfx_csc);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    ret = pq_mng_get_gfx_csc_coef(gfx_csc, gfx_csc_coef);

    /* proc used */
    g_gfx_csc_info.gfx_layer = gfx_layer;
    g_gfx_csc_info.is_bgr_in = gfx_csc_info->is_bgr_in;
    g_gfx_csc_info.csc_en = gfx_csc_info->gfx_csc_mode.csc_en;
    g_gfx_csc_info.csc_in = gfx_csc_info->gfx_csc_mode.csc_in;
    g_gfx_csc_info.csc_out = gfx_csc_info->gfx_csc_mode.csc_out;
    g_gfx_csc_info.csc_pecision = gfx_csc_info->gfx_csc_mode.csc_pecision;
    g_gfx_csc_info.csc_type = gfx_csc_info->gfx_csc_mode.csc_type;

    return ret;
}

hi_s32 pq_mng_set_gfx_csc_picture_setting(hi_pq_display disp, hi_pq_image_param *pic_setting)
{
    PQ_CHECK_OVER_RANGE_RE_FAIL(disp, HI_PQ_DISPLAY_MAX);

    if (HI_PQ_DISPLAY_0 == disp) { /* SD */
        g_gfxcsc_sd_pic_para.brightness = pic_setting->brightness;
        g_gfxcsc_sd_pic_para.contrast = pic_setting->contrast;
        g_gfxcsc_sd_pic_para.saturation = pic_setting->saturation;
        g_gfxcsc_sd_pic_para.hue = pic_setting->hue;
        g_gfxcsc_sd_pic_para.wcg_temperature = pic_setting->wcg_temperature;
    } else { /* HD */
        g_gfxcsc_hd_pic_para.brightness = pic_setting->brightness;
        g_gfxcsc_hd_pic_para.contrast = pic_setting->contrast;
        g_gfxcsc_hd_pic_para.saturation = pic_setting->saturation;
        g_gfxcsc_hd_pic_para.hue = pic_setting->hue;
        g_gfxcsc_hd_pic_para.wcg_temperature = pic_setting->wcg_temperature;
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_gfx_csc_picture_setting(hi_pq_display disp, hi_pq_image_param *pic_setting)
{
    PQ_CHECK_OVER_RANGE_RE_FAIL(disp, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(pic_setting);

    if (HI_PQ_DISPLAY_0 == disp) { /* SD */
        pic_setting->brightness = g_gfxcsc_sd_pic_para.brightness;
        pic_setting->contrast = g_gfxcsc_sd_pic_para.contrast;
        pic_setting->hue = g_gfxcsc_sd_pic_para.hue;
        pic_setting->saturation = g_gfxcsc_sd_pic_para.saturation;
        pic_setting->wcg_temperature = g_gfxcsc_sd_pic_para.wcg_temperature;
    } else { /* HD */
        pic_setting->brightness = g_gfxcsc_hd_pic_para.brightness;
        pic_setting->contrast = g_gfxcsc_hd_pic_para.contrast;
        pic_setting->hue = g_gfxcsc_hd_pic_para.hue;
        pic_setting->saturation = g_gfxcsc_hd_pic_para.saturation;
        pic_setting->wcg_temperature = g_gfxcsc_hd_pic_para.wcg_temperature;
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_gfx_csc_color_temp(hi_pq_display disp, hi_pq_color_temperature *temperature)
{
    PQ_CHECK_OVER_RANGE_RE_FAIL(disp, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(temperature);

    temperature->red_gain = g_gfx_csc_tuning.kr;
    temperature->green_gain = g_gfx_csc_tuning.kg;
    temperature->blue_gain = g_gfx_csc_tuning.kb;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_gfx_csc_color_temp(hi_pq_display disp, hi_pq_color_temperature *temperature)
{
    PQ_CHECK_OVER_RANGE_RE_FAIL(disp, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(temperature->red_gain, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(temperature->green_gain, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(temperature->blue_gain, PQ_ALG_MAX_VALUE);

    g_gfx_csc_tuning.kr = temperature->red_gain;
    g_gfx_csc_tuning.kg = temperature->green_gain;
    g_gfx_csc_tuning.kb = temperature->blue_gain;

    return HI_SUCCESS;
}

static pq_alg_funcs g_gfx_csc_funcs = {
    .get_gfx_csc_coef = pq_mng_get_gfx_csc,
    .set_gfx_csc_picture_setting = pq_mng_set_gfx_csc_picture_setting,
    .get_gfx_csc_picture_setting = pq_mng_get_gfx_csc_picture_setting,
    .get_color_temp = pq_mng_get_gfx_csc_color_temp,
    .set_color_temp = pq_mng_set_gfx_csc_color_temp,
    .get_gfx_csc_info = pq_mng_proc_get_gfx_csc_info,
};

hi_s32 pq_mng_register_gfx_csc()
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_GFXCSC, REG_TYPE_ALL, PQ_BIN_ADAPT_SINGLE, "gfx csc",
                               &g_gfx_csc_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_gfx_csc()
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_GFXCSC);

    return ret;
}

