/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include <linux/string.h>

#include "pq_hal_comm.h"
#include "drv_pq_ext.h"
#include "pq_hal_vpsshdr.h"
#include "pq_hal_vpsshdr_coef.h"
#include "pq_hal_vpsshdr_regset.h"
#include "pq_hal_hdrv2_regset_comm.h"

#define PQ_VPSS_HDR_OFFSET (0x87000 / 4)

#define INDEX_0 0
#define INDEX_1 1
#define INDEX_2 2
#define INDEX_3 3
#define INDEX_4 4
#define INDEX_5 5
#define INDEX_6 6
#define INDEX_7 7

static hi_pq_hdr_mode g_vpsshdr_mode[VPSS_HANDLE_NUM] = {0};

typedef struct {
    hi_u32 clut[PQ_VPSSHDR_DEGAMMA];
} pq_hal_vpsshdr_degamma;

static hi_u32 g_vpsshdr_cm_lut_linear[32] = {  /* 32 : hdrv1 cm lut length */
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
};

const hi_s32 g_pq_vpsshdr_sin_table[PQ_VPSSHDR_TABLE_MAX] = {
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

const hi_u32 g_pq_vpsshdr_cos_table[PQ_VPSSHDR_TABLE_MAX] = {
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

hi_u32 g_vpsshdr_bright = PQ_CSC_VIDEO_DEFAULT;
hi_u32 g_vpsshdr_contrast = PQ_CSC_VIDEO_DEFAULT;
hi_u32 g_vpsshdr_satu = PQ_CSC_VIDEO_DEFAULT;
hi_u32 g_vpsshdr_hue = PQ_CSC_VIDEO_DEFAULT;
hi_u32 g_vpsshdr_wcg_temperature = PQ_CSC_VIDEO_DEFAULT;

hi_u32 g_vpsshdr_offset_bright[HI_PQ_HDR_MODE_MAX] = {
    128,    128,    128,    128,    128,    128,
    128,    128,    128,    128,    128,    128,
};

hi_u32 g_vpsshdr_offset_contrast[HI_PQ_HDR_MODE_MAX] = {
    128,    128,    128,    128,    128,    128,
    128,    128,    128,    128,    128,    128,
};

hi_u32 g_vpsshdr_offset_satu[HI_PQ_HDR_MODE_MAX] = {
    128,    128,    128,    128,    128,    128,
    128,    128,    128,    128,    128,    128,
};

hi_u32 g_vpsshdr_offset_hue[HI_PQ_HDR_MODE_MAX] = {
    128,    128,    128,    128,    128,    128,
    128,    128,    128,    128,    128,    128,
};

hi_u32 g_vpsshdr_offset_red[HI_PQ_HDR_MODE_MAX] = {
    128,    128,    128,    128,    128,    128,
    128,    128,    128,    128,    128,    128,
};

hi_u32 g_vpsshdr_offset_green[HI_PQ_HDR_MODE_MAX] = {
    128,    128,    128,    128,    128,    128,
    128,    128,    128,    128,    128,    128,
};

hi_u32 g_vpsshdr_offset_blue[HI_PQ_HDR_MODE_MAX] = {
    128,    128,    128,    128,    128,    128,
    128,    128,    128,    128,    128,    128,
};

hi_u32 g_vpsshdr_dark_cv_str[HI_PQ_HDR_MODE_MAX] = {
    50,    50,    50,    50,    50,    50,
    50,    50,    50,    50,    50,    50,
};

hi_u32 g_vpsshdr_bright_cv_str[HI_PQ_HDR_MODE_MAX] = {
    50,    50,    50,    50,    50,    50,
    50,    50,    50,    50,    50,    50,
};

typedef struct {
    hi_u32 gammax[PQ_VPSSHDR_GAMMA];
    hi_u32 gammaa[PQ_VPSSHDR_GAMMA];
    hi_u32 gammab[PQ_VPSSHDR_GAMMA];
} pq_hal_vpsshdr_gamma;

typedef struct {
    hi_u32 clut[PQ_VPSSHDR_TM_CLUT];
} pq_hal_vpsshdr_tm;

typedef hi_s32 (*pf_pq_hal_vpss_gethdrpathcfg)(hi_u32 handle, drv_pq_internal_color_space en_in_cs,
                                               drv_pq_internal_color_space en_out_cs,
                                               pq_hal_vpsshdr_cfg *pst_pq_hdr_cfg);

typedef struct {
    hi_pq_hdr_mode hdr_mode;
    pf_pq_hal_vpss_gethdrpathcfg hdr_func;
} pq_vpss_hdr_fun;

#define PQ_VPSSHDR_COEF_SIZE   24576
#define PQ_VPSS_COEF_OVER_SIZE 128

#define PQ_COEF_SIZE_VPSS_HDR0 12288
#define PQ_COEF_SIZE_VPSS_HDR1 4096
#define PQ_COEF_SIZE_VPSS_HDR2 4096
#define PQ_COEF_SIZE_VPSS_HDR3 4096

typedef enum {
    PQ_VPSS_COEF_DATA_TYPE_U8 = 0,
    PQ_VPSS_COEF_DATA_TYPE_S8,
    PQ_VPSS_COEF_DATA_TYPE_U16,
    PQ_VPSS_COEF_DATA_TYPE_S16,
    PQ_VPSS_COEF_DATA_TYPE_U32,
    PQ_VPSS_COEF_DATA_TYPE_S32,

    PQ_VPSS_COEF_DATA_TYPE_MAX
} pq_vpss_coef_data_type;

typedef struct {
    void *coef;
    void *coef_new;
    pq_vpss_coef_data_type coef_data_type;
    hi_u32 length;
    hi_s32 coef_max;
    hi_s32 coef_min;
} pq_vpss_coef_gen_cfg;

typedef struct {
    hi_u8 *coef_addr;
    hi_u32 lut_num;
    hi_u32 burst_num;
    hi_u32 cycle_num;
    void **coef_array;
    hi_u32 *lut_length;
    hi_u32 *coef_bit_length;
    pq_vpss_coef_data_type data_type;
} pq_vpss_coef_send_cfg;

typedef struct {
    hi_u32 data3;
    hi_u32 data2;
    hi_u32 data1;
    hi_u32 data0;
    hi_u32 depth;
} pq_vpss_coef_data;

#define VPSS_BIT_32  32
#define VPSS_BIT_64  64
#define VPSS_BIT_96  96
#define VPSS_BIT_128 128

#define VPSS_LUT_SIZE 9

typedef struct {
    hi_u16 *lut00_r;
    hi_u16 *lut00_g;
    hi_u16 *lut00_b;
    hi_u16 *lut01_r;
    hi_u16 *lut01_g;
    hi_u16 *lut01_b;
    hi_u16 *lut1_r;
    hi_u16 *lut1_g;
    hi_u16 *lut1_b;
    hi_u16 *lut2_r;
    hi_u16 *lut2_g;
    hi_u16 *lut2_b;
    hi_u16 *lut3_r;
    hi_u16 *lut3_g;
    hi_u16 *lut3_b;
    hi_u16 *lut4_r;
    hi_u16 *lut4_g;
    hi_u16 *lut4_b;
    hi_u16 *lut5_r;
    hi_u16 *lut5_g;
    hi_u16 *lut5_b;
    hi_u16 *lut6_r;
    hi_u16 *lut6_g;
    hi_u16 *lut6_b;
    hi_u16 *lut7_r;
    hi_u16 *lut7_g;
    hi_u16 *lut7_b;
} vpss_cm_coef;

#define DEF_L2PQ_LUT_NODES 128

typedef struct {
    hi_void *vpsshdr_v1_cm_lut;
    hi_void *vpsshdr_imap_degamma;
    hi_void *vpsshdr_tmapv1_lut;

    hi_void *vpsshdr_imap_gamma_lut_x;
    hi_void *vpsshdr_imap_gamma_lut_a;
    hi_void *vpsshdr_imap_gamma_lut_b;

    hi_void *vpsshdr_tmap_tm_lut_i;
    hi_void *vpsshdr_tmap_tm_lut_s;
    hi_void *vpsshdr_tmap_sm_lut_i;
    hi_void *vpsshdr_tmap_sm_lut_s;
    hi_void *vpsshdr_tmap_smc_lut;

    hi_u16 vpsshdr_imap_cacm_lut_r[PQ_VPSSHDR_CM_3DCLUT][PQ_VPSSHDR_CM_3DCLUT][PQ_VPSSHDR_CM_3DCLUT];
    hi_u16 vpsshdr_imap_cacm_lut_g[PQ_VPSSHDR_CM_3DCLUT][PQ_VPSSHDR_CM_3DCLUT][PQ_VPSSHDR_CM_3DCLUT];
    hi_u16 vpsshdr_imap_cacm_lut_b[PQ_VPSSHDR_CM_3DCLUT][PQ_VPSSHDR_CM_3DCLUT][PQ_VPSSHDR_CM_3DCLUT];

    hi_u16 vpsshdr_coef_cm_lutr[VPSS_LUT_SIZE][VPSS_LUT_SIZE][VPSS_LUT_SIZE];
    hi_u16 vpsshdr_coef_cm_lutg[VPSS_LUT_SIZE][VPSS_LUT_SIZE][VPSS_LUT_SIZE];
    hi_u16 vpsshdr_coef_cm_lutb[VPSS_LUT_SIZE][VPSS_LUT_SIZE][VPSS_LUT_SIZE];
} pq_vpsshdr_param;

typedef struct {
    hi_u16 vpsshdr_coef_cm_lutr3d0[125]; /* 125 is length */
    hi_u16 vpsshdr_coef_cm_lutr3d1[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lutr3d2[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lutr3d3[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lutr3d4[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lutr3d5[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lutr3d6[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lutr3d7[64];  /* 64 is length */

    hi_u16 vpsshdr_coef_cm_lutg3d0[125]; /* 125 is length */
    hi_u16 vpsshdr_coef_cm_lutg3d1[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lutg3d2[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lutg3d3[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lutg3d4[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lutg3d5[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lutg3d6[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lutg3d7[64];  /* 64 is length */

    hi_u16 vpsshdr_coef_cm_lutb3d0[125]; /* 125 is length */
    hi_u16 vpsshdr_coef_cm_lutb3d1[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lutb3d2[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lutb3d3[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lutb3d4[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lutb3d5[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lutb3d6[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lutb3d7[64];  /* 64 is length */
} pq_vpsshdr_cacm_rgb3dlut;

#define EVE_SIZE 5
#define ODD_SIZE 4
#define DIV_2    2

typedef struct {
    hi_u16 vpsshdr_coef_cm_lut00_r[64]; /* 64 is length */
    hi_u16 vpsshdr_coef_cm_lut01_r[64]; /* 64 is length */
    hi_u16 vpsshdr_coef_cm_lut1_r[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lut2_r[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lut3_r[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lut4_r[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lut5_r[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lut6_r[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lut7_r[64];  /* 64 is length */

    hi_u16 vpsshdr_coef_cm_lut00_g[64]; /* 64 is length */
    hi_u16 vpsshdr_coef_cm_lut01_g[64]; /* 64 is length */
    hi_u16 vpsshdr_coef_cm_lut1_g[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lut2_g[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lut3_g[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lut4_g[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lut5_g[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lut6_g[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lut7_g[64];  /* 64 is length */

    hi_u16 vpsshdr_coef_cm_lut00_b[64]; /* 64 is length */
    hi_u16 vpsshdr_coef_cm_lut01_b[64]; /* 64 is length */
    hi_u16 vpsshdr_coef_cm_lut1_b[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lut2_b[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lut3_b[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lut4_b[100]; /* 100 is length */
    hi_u16 vpsshdr_coef_cm_lut5_b[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lut6_b[80];  /* 80 is length */
    hi_u16 vpsshdr_coef_cm_lut7_b[64];  /* 64 is length */
} pq_vpsshdr_cacm_rgblut;

typedef struct {
    hi_s32 vpsshdr_coef_tm_new[256]; /* 256 is length */
    hi_u32 vpsshdr_degamm_pq_new[1024]; /* 1024 is length */
    hi_u32 vpsshdr_gamm_l2pq_lut_x[DEF_L2PQ_LUT_NODES];
    hi_u32 vpsshdr_gamm_l2pq_lut_a[DEF_L2PQ_LUT_NODES];
    hi_u32 vpsshdr_gamm_l2pq_lut_b[DEF_L2PQ_LUT_NODES];
}pq_vpsshdr_param_new;

pq_vpsshdr_coef_addr g_pq_vdp_vpss_hdr_coef_buf[VPSS_HANDLE_NUM] = {0};
static pq_vpsshdr_param g_pq_vpsshdr_param[VPSS_HANDLE_NUM] = {0};
static pq_vpsshdr_param_new g_pq_vpsshdr_param_new[VPSS_HANDLE_NUM] = {0};
static pq_vpsshdr_cacm_rgb3dlut g_pq_vpsshdr_cacm_rgb3dlut[VPSS_HANDLE_NUM] = {0};
static pq_vpsshdr_cacm_rgblut g_pq_vpsshdr_cacm_rgblut[VPSS_HANDLE_NUM] = {0};

hi_u32 pq_vpss_drv_gencoef(pq_vpss_coef_gen_cfg *cfg)
{
    hi_u32 ii = 0;

    if (cfg->coef_data_type == PQ_VPSS_COEF_DATA_TYPE_U8) {
        for (ii = 0; ii < cfg->length; ii++) {
            ((hi_u8 *)cfg->coef_new)[ii] = ((hi_u8 *)cfg->coef)[ii];
        }
    } else if (cfg->coef_data_type == PQ_VPSS_COEF_DATA_TYPE_S8) {
        for (ii = 0; ii < cfg->length; ii++) {
            ((hi_s8 *)cfg->coef_new)[ii] = ((hi_s8 *)cfg->coef)[ii];
        }
    } else if (cfg->coef_data_type == PQ_VPSS_COEF_DATA_TYPE_U16) {
        for (ii = 0; ii < cfg->length; ii++) {
            ((hi_u16 *)cfg->coef_new)[ii] = ((hi_u16 *)cfg->coef)[ii];
        }
    } else if (cfg->coef_data_type == PQ_VPSS_COEF_DATA_TYPE_S16) {
        for (ii = 0; ii < cfg->length; ii++) {
            ((hi_s16 *)cfg->coef_new)[ii] = ((hi_s16 *)cfg->coef)[ii];
        }
    } else if (cfg->coef_data_type == PQ_VPSS_COEF_DATA_TYPE_U32) {
        for (ii = 0; ii < cfg->length; ii++) {
            ((hi_u32 *)cfg->coef_new)[ii] = ((hi_u32 *)cfg->coef)[ii];
        }
    } else if (cfg->coef_data_type == PQ_VPSS_COEF_DATA_TYPE_S32) {
        for (ii = 0; ii < cfg->length; ii++) {
            ((hi_s32 *)cfg->coef_new)[ii] = ((hi_s32 *)cfg->coef)[ii];
        }
    }

    return 0;
}

static hi_void pq_coef_push128(pq_vpss_coef_data *data, hi_u32 coef_data, hi_u32 bit_len)
{
    coef_data = coef_data & (0xFFFFFFFF >> (VPSS_BIT_32 - bit_len));

    if (data->depth < VPSS_BIT_32) {
        if ((data->depth + bit_len) <= VPSS_BIT_32) {
            data->data0 = (coef_data << data->depth) | data->data0;
        } else {
            data->data0 = (coef_data << data->depth) | data->data0;
            data->data1 = coef_data >> (VPSS_BIT_32 - data->depth % VPSS_BIT_32);
        }
    } else if (data->depth >= VPSS_BIT_32 && data->depth < VPSS_BIT_64) {
        if ((data->depth + bit_len) <= VPSS_BIT_64) {
            data->data1 = (coef_data << (data->depth % VPSS_BIT_32)) | data->data1;
        } else {
            data->data1 = (coef_data << (data->depth % VPSS_BIT_32)) | data->data1;
            data->data2 = coef_data >> (VPSS_BIT_32 - data->depth % VPSS_BIT_32);
        }
    } else if (data->depth >= VPSS_BIT_64 && data->depth < VPSS_BIT_96) {
        if ((data->depth + bit_len) <= VPSS_BIT_96) {
            data->data2 = (coef_data << (data->depth % VPSS_BIT_32)) | data->data2;
        } else {
            data->data2 = (coef_data << (data->depth % VPSS_BIT_32)) | data->data2;
            data->data3 = coef_data >> (VPSS_BIT_32 - data->depth % VPSS_BIT_32);
        }
    } else if (data->depth >= VPSS_BIT_96) {
        if ((data->depth + bit_len) <= VPSS_BIT_128) {
            data->data3 = (coef_data << (data->depth % VPSS_BIT_32)) | data->data3;
        }
    }

    data->depth = data->depth + bit_len;

    if (data->depth > VPSS_BIT_128) {
        HI_ERR_PQ("ip coef error.\n");
    }
}

static hi_u32 pq_coef_findmax(hi_u32 *array, hi_u32 num)
{
    hi_u32 ii;
    hi_u32 data = array[0];

    for (ii = 1; ii < num; ii++) {
        if (data < array[ii]) {
            data = array[ii];
        }
    }
    return data;
}

static hi_u32 pq_vpss_ip_coef_sendcoef_get_data(pq_vpss_coef_data_type type, hi_u32 n,
                                                hi_u32 coef_cnt, hi_u8 **coef_array)
{
    hi_u32 tmp_data = 0;

    if (type == PQ_VPSS_COEF_DATA_TYPE_S16) {
        tmp_data = ((hi_s16 **)coef_array)[n][coef_cnt];
    } else if (type == PQ_VPSS_COEF_DATA_TYPE_U16) {
        tmp_data = ((hi_u16 **)coef_array)[n][coef_cnt];
    } else if (type == PQ_VPSS_COEF_DATA_TYPE_U32) {
        tmp_data = ((hi_u32 **)coef_array)[n][coef_cnt];
    } else if (type == PQ_VPSS_COEF_DATA_TYPE_S32) {
        tmp_data = ((hi_s32 **)coef_array)[n][coef_cnt];
    } else if (type == PQ_VPSS_COEF_DATA_TYPE_S8) {
        tmp_data = ((hi_s8 **)coef_array)[n][coef_cnt];
    } else if (type == PQ_VPSS_COEF_DATA_TYPE_U8) {
        tmp_data = ((hi_u8 **)coef_array)[n][coef_cnt];
    }

    return tmp_data;
}

hi_void pq_vpss_ip_coef_sendcoef_cnt(hi_u32 coef_cnt, pq_vpss_coef_send_cfg *cfg, pq_vpss_coef_data *data)
{
    hi_u32 tmp_data;
    hi_u32 n;

    for (n = 0; n < cfg->lut_num; n++) {
        if (coef_cnt < cfg->lut_length[n]) {
            tmp_data = pq_vpss_ip_coef_sendcoef_get_data(cfg->data_type, n, coef_cnt, (hi_u8 **)(cfg->coef_array));
        } else {
            tmp_data = 0;
        }
        pq_coef_push128(data, tmp_data, cfg->coef_bit_length[n]);
    }
}

static hi_u32 pq_coef_writeddr(hi_u8 *addr, pq_vpss_coef_data *data)
{
    hi_u32 i = 0;
    hi_u32 data_arr[4] = { data->data0, data->data1, data->data2, data->data3 }; /* 4 is number */
    hi_u8 *tmp_addr = 0;
    hi_u32 tmp_data = 0;

    for (i = 0; i < 4; i++) {    /* 4 is number */
        tmp_addr = addr + i * 4; /* 4 is number */
        tmp_data = data_arr[i];
        *(hi_u32 *)tmp_addr = tmp_data;
    }

    return 0;
}

hi_u8 *pq_vpss_ip_coef_sendcoef(pq_vpss_coef_send_cfg *cfg)
{
    hi_u32 i, k, m;
    hi_u32 addr_offset = 0;
    hi_u8 *addr_base = cfg->coef_addr;
    hi_u8 *addr = addr_base;

    hi_u32 max_len;
    hi_u32 coef_cnt;

    hi_u32 cycle_num;
    hi_u32 total_bit_len;
    hi_u32 total_burst_num;

    pq_vpss_coef_data data;

    addr_base = cfg->coef_addr;
    addr = addr_base;
    cycle_num = cfg->cycle_num;

    for (i = 0; i < cfg->lut_num; i++) {
        total_bit_len = total_bit_len + cfg->coef_bit_length[i];
    }

    /* send data */
    max_len = pq_coef_findmax(cfg->lut_length, cfg->lut_num);
    total_burst_num = (max_len + cycle_num - 1) / cycle_num;

    for (k = 0; k < total_burst_num; k++) {
        memset((void *)&data, 0, sizeof(data));
        for (m = 0; m < cycle_num; m++) {
            coef_cnt = k * cycle_num + m;
            pq_vpss_ip_coef_sendcoef_cnt(coef_cnt, cfg, &data);
        }
        addr = addr_base + addr_offset;
        addr_offset = addr_offset + 16; /* 16 is number */
        pq_coef_writeddr(addr, &data);
    }

    return (addr_base + addr_offset);
}

static hi_void pq_vpsshdr_set_imap_tmpa_v1_coef(hi_u32 handle, hi_u8 *addr)
{
    pq_vpss_coef_gen_cfg coef_gen;
    pq_vpss_coef_send_cfg coef_send;

    hi_void *tone_map_clut;

    void *p_coef_array[1] = { g_pq_vpsshdr_param_new[handle].vpsshdr_coef_tm_new };
    hi_u32 lut_length[1] = { 256 };
    hi_u32 coef_bit_length[1] = { 16 };

    tone_map_clut = g_pq_vpsshdr_param[handle].vpsshdr_tmapv1_lut;
    coef_gen.coef = tone_map_clut;
    coef_gen.coef_new = g_pq_vpsshdr_param_new[handle].vpsshdr_coef_tm_new;
    coef_gen.length = 256; /* 256 is length */
    coef_gen.coef_data_type = PQ_VPSS_COEF_DATA_TYPE_U32;
    coef_gen.coef_max = ((1 << 14) - 1); /* 14 is coef */
    coef_gen.coef_min = 0;
    pq_vpss_drv_gencoef(&coef_gen);

    coef_send.coef_addr = addr;
    coef_send.lut_num = 1;
    coef_send.burst_num = 1;
    coef_send.cycle_num = 8; /* 8 is number */
    coef_send.coef_array = p_coef_array;
    coef_send.lut_length = lut_length;
    coef_send.coef_bit_length = coef_bit_length;
    coef_send.data_type = PQ_VPSS_COEF_DATA_TYPE_U32;
    pq_vpss_ip_coef_sendcoef(&coef_send);
}

static hi_void pq_vpsshdr_set_imap_cacm_coef(vpss_cm_coef *lut_cm_coef, hi_u8 *addr)
{
    pq_vpss_coef_send_cfg coef_send;

    hi_u16 lut_eq_0[100] = { 0 }; /* 100 is length */
    hi_u16 lut_eq_1[100] = { 0 }; /* 100 is length */
    void *coef_array[11] = { /* 11 is length */
        lut_cm_coef->lut1_b, lut_cm_coef->lut1_g, lut_cm_coef->lut1_r, lut_cm_coef->lut2_b, lut_cm_coef->lut2_g,
        lut_cm_coef->lut2_r, lut_cm_coef->lut4_b, lut_cm_coef->lut4_g, lut_cm_coef->lut4_r, lut_eq_1, lut_eq_0};
    hi_u32 lut_length[11] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 }; /* 11 is length */
    hi_u32 coef_bit_length[11] = { 12, 12, 12, 12, 12, 12, 12, 12, 12, 10, 10 }; /* 11 is length */

    hi_u16 lut_eq_2[80] = { 0 }; /* 80 is length */
    hi_u16 lut_eq_3[80] = { 0 }; /* 80 is length */
    void *coef_array1[11] = { /* 11 is length */
        lut_cm_coef->lut3_b, lut_cm_coef->lut3_g, lut_cm_coef->lut3_r, lut_cm_coef->lut5_b, lut_cm_coef->lut5_g,
        lut_cm_coef->lut5_r, lut_cm_coef->lut6_b, lut_cm_coef->lut6_g, lut_cm_coef->lut6_r, lut_eq_3, lut_eq_2};
    hi_u32 lut_length1[11] = { 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80 }; /* 11 is length */
    hi_u32 coef_bit_length1[11] = { 12, 12, 12, 12, 12, 12, 12, 12, 12, 10, 10 }; /* 11 is length */

    hi_u16 lut_eq_4[64] = { 0 }; /* 64 is length */
    hi_u16 lut_eq_5[64] = { 0 }; /* 64 is length */
    void *coef_array2[11] = { /* 11 is length */
        lut_cm_coef->lut00_b, lut_cm_coef->lut00_g, lut_cm_coef->lut00_r, lut_cm_coef->lut01_b, lut_cm_coef->lut01_g,
        lut_cm_coef->lut01_r, lut_cm_coef->lut7_b, lut_cm_coef->lut7_g, lut_cm_coef->lut7_r, lut_eq_5, lut_eq_4};
    hi_u32 lut_length2[11] = { 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64 }; /* 11 is length */
    hi_u32 coef_bit_length2[11] = { 12, 12, 12, 12, 12, 12, 12, 12, 12, 10, 10 }; /* 11 is length */

    coef_send.coef_addr = addr;
    coef_send.lut_num = 11; /* 11 is number */
    coef_send.burst_num = 1;
    coef_send.cycle_num = 1;
    coef_send.coef_array = coef_array;
    coef_send.lut_length = lut_length;
    coef_send.coef_bit_length = coef_bit_length;
    coef_send.data_type = PQ_VPSS_COEF_DATA_TYPE_U16;
    addr = pq_vpss_ip_coef_sendcoef(&coef_send);

    coef_send.coef_addr = addr;
    coef_send.cycle_num = 1;
    coef_send.coef_array = coef_array1;
    coef_send.lut_length = lut_length1;
    coef_send.coef_bit_length = coef_bit_length1;
    coef_send.data_type = PQ_VPSS_COEF_DATA_TYPE_U16;
    addr = pq_vpss_ip_coef_sendcoef(&coef_send);

    coef_send.coef_addr = addr;
    coef_send.cycle_num = 1;
    coef_send.coef_array = coef_array2;
    coef_send.lut_length = lut_length2;
    coef_send.coef_bit_length = coef_bit_length2;
    coef_send.data_type = PQ_VPSS_COEF_DATA_TYPE_U16;
    addr = pq_vpss_ip_coef_sendcoef(&coef_send);
}

static hi_void pq_vpss_hdr_get_coef_rgb_lut(hi_u32 handle, hi_u16 ii_r, hi_u16 ii_g, hi_u16 ii_b)
{
    hi_u16 index;
    if ((ii_r % DIV_2 == 0) && (ii_g % DIV_2 == 0) && (ii_b % DIV_2 == 0)) {
        index = EVE_SIZE * EVE_SIZE * ((ii_r + 1) / DIV_2) + EVE_SIZE * ((ii_g + 1) / DIV_2) + (ii_b + 1) / DIV_2;
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d0[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d0[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d0[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    } else if ((ii_r % DIV_2 == 0) && (ii_g % DIV_2 == 0) && (ii_b % DIV_2 == 1)) {
        index = EVE_SIZE * ODD_SIZE * ((ii_r + 1) / DIV_2) + ODD_SIZE * ((ii_g + 1) / DIV_2) + (ii_b + 0) / DIV_2;
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d1[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d1[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d1[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    } else if ((ii_r % DIV_2 == 0) && (ii_g % DIV_2 == 1) && (ii_b % DIV_2 == 0)) {
        index = EVE_SIZE * ODD_SIZE * ((ii_r + 1) / DIV_2) + EVE_SIZE * ((ii_g + 0) / DIV_2) + (ii_b + 1) / DIV_2;
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d4[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d4[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d4[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    } else if ((ii_r % DIV_2 == 0) && (ii_g % DIV_2 == 1) && (ii_b % DIV_2 == 1)) {
        index = ODD_SIZE * ODD_SIZE * ((ii_r + 1) / DIV_2) + ODD_SIZE * ((ii_g + 0) / DIV_2) + (ii_b + 0) / DIV_2;
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d5[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d5[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d5[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    } else if ((ii_r % DIV_2 == 1) && (ii_g % DIV_2 == 0) && (ii_b % DIV_2 == 0)) {
        index = EVE_SIZE * EVE_SIZE * ((ii_r + 0) / DIV_2) + EVE_SIZE * ((ii_g + 1) / DIV_2) + (ii_b + 1) / DIV_2;
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d2[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d2[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d2[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    } else if ((ii_r % DIV_2 == 1) && (ii_g % DIV_2 == 0) && (ii_b % DIV_2 == 1)) {
        index = EVE_SIZE * ODD_SIZE * ((ii_r + 0) / DIV_2) + ODD_SIZE * ((ii_g + 1) / DIV_2) + (ii_b + 0) / DIV_2;
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d3[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d3[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d3[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    } else if ((ii_r % DIV_2 == 1) && (ii_g % DIV_2 == 1) && (ii_b % DIV_2 == 0)) {
        index = EVE_SIZE * ODD_SIZE * ((ii_r + 0) / DIV_2) + EVE_SIZE * ((ii_g + 0) / DIV_2) + (ii_b + 1) / DIV_2;
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d6[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d6[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d6[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    } else if ((ii_r % DIV_2 == 1) && (ii_g % DIV_2 == 1) && (ii_b % DIV_2 == 1)) {
        index = ODD_SIZE * ODD_SIZE * ((ii_r + 0) / DIV_2) + ODD_SIZE * ((ii_g + 0) / DIV_2) + (ii_b + 0) / DIV_2;
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d7[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d7[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d7[index] =
            g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    }
}

static hi_void pq_vpss_hdr_get_coef_lut(hi_u32 handle)
{
    hi_u16 ii_r, ii_g, ii_b;
    for (ii_r = 0; ii_r < VPSS_LUT_SIZE; ii_r++) {
        for (ii_g = 0; ii_g < VPSS_LUT_SIZE; ii_g++) {
            for (ii_b = 0; ii_b < VPSS_LUT_SIZE; ii_b++) {
                pq_vpss_hdr_get_coef_rgb_lut(handle, ii_r, ii_g, ii_b);
            }
        }
    }
}

static hi_void pq_vpss_hdr_get_coef_r_lut(hi_u32 handle)
{
    hi_u16 j;
    hi_u16 *clut00 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut00_r;
    hi_u16 *clut01 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut01_r;
    hi_u16 *clut1 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut1_r;
    hi_u16 *clut2 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut2_r;
    hi_u16 *clut3 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut3_r;
    hi_u16 *clut4 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut4_r;
    hi_u16 *clut5 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut5_r;
    hi_u16 *clut6 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut6_r;
    hi_u16 *clut7 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut7_r;

    for (j = 0; j < 125; j++) { /* 125 is length */
        if (j % DIV_2 == 0) {
            *clut00 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d0[j];
            clut00++;
        } else if (j % DIV_2 == 1) {
            *clut01 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d0[j];
            clut01++;
        }
    }

    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut1 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d1[j];
        clut1++;
    }

    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut2 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d2[j];
        clut2++;
    }

    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut3 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d3[j];
        clut3++;
    }

    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut4 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d4[j];
        clut4++;
    }

    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut5 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d5[j];
        clut5++;
    }

    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut6 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d6[j];
        clut6++;
    }

    for (j = 0; j < 64; j++) { /* 64 is length */
        *clut7 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutr3d7[j];
        clut7++;
    }
}

static hi_void pq_vpss_hdr_get_coef_g_lut(hi_u32 handle)
{
    hi_u16 j;
    hi_u16 *clut00 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut00_g;
    hi_u16 *clut01 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut01_g;
    hi_u16 *clut1 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut1_g;
    hi_u16 *clut2 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut2_g;
    hi_u16 *clut3 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut3_g;
    hi_u16 *clut4 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut4_g;
    hi_u16 *clut5 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut5_g;
    hi_u16 *clut6 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut6_g;
    hi_u16 *clut7 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut7_g;

    for (j = 0; j < 125; j++) { /* 125 is length */
        if (j % DIV_2 == 0) {
            *clut00 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d0[j];
            clut00++;
        } else if (j % DIV_2 == 1) {
            *clut01 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d0[j];
            clut01++;
        }
    }

    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut1 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d1[j];
        clut1++;
    }

    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut2 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d2[j];
        clut2++;
    }

    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut3 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d3[j];
        clut3++;
    }

    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut4 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d4[j];
        clut4++;
    }

    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut5 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d5[j];
        clut5++;
    }

    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut6 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d6[j];
        clut6++;
    }

    for (j = 0; j < 64; j++) { /* 64 is length */
        *clut7 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutg3d7[j];
        clut7++;
    }
}

static hi_void pq_vpss_hdr_get_coef_b_lut(hi_u32 handle)
{
    hi_u16 j;
    hi_u16 *clut00 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut00_b;
    hi_u16 *clut01 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut01_b;
    hi_u16 *clut1 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut1_b;
    hi_u16 *clut2 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut2_b;
    hi_u16 *clut3 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut3_b;
    hi_u16 *clut4 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut4_b;
    hi_u16 *clut5 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut5_b;
    hi_u16 *clut6 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut6_b;
    hi_u16 *clut7 = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut7_b;

    for (j = 0; j < 125; j++) { /* 125 is length */
        if (j % DIV_2 == 0) {
            *clut00 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d0[j];
            clut00++;
        } else if (j % DIV_2 == 1) {
            *clut01 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d0[j];
            clut01++;
        }
    }
    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut1 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d1[j];
        clut1++;
    }

    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut2 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d2[j];
        clut2++;
    }
    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut3 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d3[j];
        clut3++;
    }
    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut4 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d4[j];
        clut4++;
    }
    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut5 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d5[j];
        clut5++;
    }

    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut6 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d6[j];
        clut6++;
    }
    for (j = 0; j < 64; j++) { /* 64 is length */
        *clut7 = g_pq_vpsshdr_cacm_rgb3dlut[handle].vpsshdr_coef_cm_lutb3d7[j];
        clut7++;
    }
}

static hi_void pq_vpsshdr_set_imap_cm_coef(hi_u32 handle, hi_u8 *addr)
{
    vpss_cm_coef lut_cm_coef;
    hi_u16 i, j, k;

    for (i = 0; i < VPSS_LUT_SIZE; i++) {
        for (j = 0; j < VPSS_LUT_SIZE; j++) {
            for (k = 0; k < VPSS_LUT_SIZE; k++) {
                g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutr[i][j][k] =
                    g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_r[i][j][k];
                g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutg[i][j][k] =
                    g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_g[i][j][k];
                g_pq_vpsshdr_param[handle].vpsshdr_coef_cm_lutb[i][j][k] =
                    g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_b[i][j][k];
            }
        }
    }

    pq_vpss_hdr_get_coef_lut(handle);
    pq_vpss_hdr_get_coef_r_lut(handle);
    pq_vpss_hdr_get_coef_g_lut(handle);
    pq_vpss_hdr_get_coef_b_lut(handle);

    lut_cm_coef.lut00_r = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut00_r;
    lut_cm_coef.lut00_g = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut00_g;
    lut_cm_coef.lut00_b = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut00_b;
    lut_cm_coef.lut01_r = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut01_r;
    lut_cm_coef.lut01_g = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut01_g;
    lut_cm_coef.lut01_b = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut01_b;
    lut_cm_coef.lut1_r = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut1_r;
    lut_cm_coef.lut1_g = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut1_g;
    lut_cm_coef.lut1_b = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut1_b;
    lut_cm_coef.lut2_r = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut2_r;
    lut_cm_coef.lut2_g = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut2_g;
    lut_cm_coef.lut2_b = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut2_b;
    lut_cm_coef.lut3_r = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut3_r;
    lut_cm_coef.lut3_g = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut3_g;
    lut_cm_coef.lut3_b = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut3_b;
    lut_cm_coef.lut4_r = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut4_r;
    lut_cm_coef.lut4_g = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut4_g;
    lut_cm_coef.lut4_b = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut4_b;
    lut_cm_coef.lut5_r = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut5_r;
    lut_cm_coef.lut5_g = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut5_g;
    lut_cm_coef.lut5_b = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut5_b;
    lut_cm_coef.lut6_r = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut6_r;
    lut_cm_coef.lut6_g = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut6_g;
    lut_cm_coef.lut6_b = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut6_b;
    lut_cm_coef.lut7_r = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut7_r;
    lut_cm_coef.lut7_g = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut7_g;
    lut_cm_coef.lut7_b = g_pq_vpsshdr_cacm_rgblut[handle].vpsshdr_coef_cm_lut7_b;

    pq_vpsshdr_set_imap_cacm_coef(&lut_cm_coef, addr);
}

static hi_void pq_vpsshdr_get_degamma_coef_cfg(hi_u32 handle, pq_vpss_coef_gen_cfg *coef_gen)
{
    coef_gen->coef = g_pq_vpsshdr_param[handle].vpsshdr_imap_degamma;
    coef_gen->coef_new = g_pq_vpsshdr_param_new[handle].vpsshdr_degamm_pq_new;
    coef_gen->coef_data_type = PQ_VPSS_COEF_DATA_TYPE_U32;
    coef_gen->length = 1024;                              /* 1024 is length */
    coef_gen->coef_max = (hi_u32)(((hi_u64)1 << 31) - 1); /* 31 is length */
    coef_gen->coef_min = 0;
}

static hi_void pq_vpsshdr_get_gamma_x_coef_cfg(hi_u32 handle, pq_vpss_coef_gen_cfg *coef_gen)
{
    coef_gen->coef = g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_x;
    coef_gen->coef_new = g_pq_vpsshdr_param_new[handle].vpsshdr_gamm_l2pq_lut_x;
    coef_gen->coef_data_type = PQ_VPSS_COEF_DATA_TYPE_U32;
    coef_gen->length = 128;                               /* 128 is length */
    coef_gen->coef_max = (hi_u32)(((hi_u64)1 << 31) - 1); /* 31 is length */
    coef_gen->coef_min = 0;
}

static hi_void pq_vpsshdr_get_gamma_a_coef_cfg(hi_u32 handle, pq_vpss_coef_gen_cfg *coef_gen)
{
    coef_gen->coef = g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_a;
    coef_gen->coef_new = g_pq_vpsshdr_param_new[handle].vpsshdr_gamm_l2pq_lut_a;
    coef_gen->coef_data_type = PQ_VPSS_COEF_DATA_TYPE_U32;
    coef_gen->length = 128;             /* 128 is length */
    coef_gen->coef_max = (1 << 28) - 1; /* 28 is length */
    coef_gen->coef_min = 0;
}

static hi_void pq_vpsshdr_get_gamma_b_coef_cfg(hi_u32 handle, pq_vpss_coef_gen_cfg *coef_gen)
{
    coef_gen->coef = g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_b;
    coef_gen->coef_new = g_pq_vpsshdr_param_new[handle].vpsshdr_gamm_l2pq_lut_b;
    coef_gen->coef_data_type = PQ_VPSS_COEF_DATA_TYPE_U32;
    coef_gen->length = 128;             /* 128 is length */
    coef_gen->coef_max = (1 << 16) - 1; /* 16 is length */
    coef_gen->coef_min = 0;
}

static hi_void pq_vpsshdr_set_imap_coef2ddr(hi_u32 handle, hi_u8 *addr, hi_u8 *addr2)
{
    pq_vpss_coef_gen_cfg coef_gen;
    pq_vpss_coef_send_cfg coef_send;
    void *coef_array[1];
    hi_u32 lut_length = 1024;    /* 1024 is length */
    hi_u32 coef_bit_length = 32; /* 32 is length */
    void *coef_array_l2pq[] = {
        g_pq_vpsshdr_param_new[handle].vpsshdr_gamm_l2pq_lut_x,
        g_pq_vpsshdr_param_new[handle].vpsshdr_gamm_l2pq_lut_a,
        g_pq_vpsshdr_param_new[handle].vpsshdr_gamm_l2pq_lut_b
    };
    hi_u32 lut_length2[] = { 128,                       128,                       128 };
    hi_u32 coef_bit_length2[] = { 32,                        28,                        16 };

    pq_vpsshdr_get_degamma_coef_cfg(handle, &coef_gen);
    pq_vpss_drv_gencoef(&coef_gen);

    coef_array[0] = g_pq_vpsshdr_param_new[handle].vpsshdr_degamm_pq_new;

    coef_send.coef_addr = addr;
    coef_send.lut_num = 1;
    coef_send.burst_num = 1;
    coef_send.cycle_num = 4; /* 4 is length */
    coef_send.coef_array = coef_array;
    coef_send.lut_length = &lut_length;
    coef_send.coef_bit_length = &coef_bit_length;
    coef_send.data_type = PQ_VPSS_COEF_DATA_TYPE_U32;
    pq_vpss_ip_coef_sendcoef(&coef_send);

    pq_vpsshdr_get_gamma_x_coef_cfg(handle, &coef_gen);
    pq_vpss_drv_gencoef(&coef_gen);

    pq_vpsshdr_get_gamma_a_coef_cfg(handle, &coef_gen);
    pq_vpss_drv_gencoef(&coef_gen);

    pq_vpsshdr_get_gamma_b_coef_cfg(handle, &coef_gen);
    pq_vpss_drv_gencoef(&coef_gen);

    coef_send.coef_addr = addr2;
    coef_send.lut_num = 3; /* 3 is length */
    coef_send.burst_num = 1;
    coef_send.cycle_num = 1;
    coef_send.coef_array = coef_array_l2pq;
    coef_send.lut_length = lut_length2;
    coef_send.coef_bit_length = coef_bit_length2;
    coef_send.data_type = PQ_VPSS_COEF_DATA_TYPE_U32;
    pq_vpss_ip_coef_sendcoef(&coef_send);

    return;
}

hi_void pq_hal_vpsshdr_get_hdr_coef(hi_u32 handle, pq_hal_vpsshdr_cfg *hdr_cfg)
{
    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_vpsshdr_set_imap_coef2ddr(handle, g_pq_vdp_vpss_hdr_coef_buf[handle].vir_addr[VPSS_HDR_COEF_BUF_HDR2],
                                     g_pq_vdp_vpss_hdr_coef_buf[handle].vir_addr[VPSS_HDR_COEF_BUF_HDR3]);
    }

    if (hdr_cfg->imap_cfg.tmap_v1.enable == HI_TRUE) {
        pq_vpsshdr_set_imap_tmpa_v1_coef(handle, g_pq_vdp_vpss_hdr_coef_buf[handle].vir_addr[VPSS_HDR_COEF_BUF_HDR4]);
    }

    if (hdr_cfg->imap_cfg.cacm.enable == HI_TRUE) {
        pq_vpsshdr_set_imap_cm_coef(handle, g_pq_vdp_vpss_hdr_coef_buf[handle].vir_addr[VPSS_HDR_COEF_BUF_HDR5]);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        /* reserve addr  */
    }

    hdr_cfg->coef_addr.imap_degamma_addr = g_pq_vdp_vpss_hdr_coef_buf[handle].phy_addr[VPSS_HDR_COEF_BUF_HDR2];
    hdr_cfg->coef_addr.imap_gamma_addr = g_pq_vdp_vpss_hdr_coef_buf[handle].phy_addr[VPSS_HDR_COEF_BUF_HDR3];
    hdr_cfg->coef_addr.imap_tmapv1_addr = g_pq_vdp_vpss_hdr_coef_buf[handle].phy_addr[VPSS_HDR_COEF_BUF_HDR4];
    hdr_cfg->coef_addr.imap_cacm_addr = g_pq_vdp_vpss_hdr_coef_buf[handle].phy_addr[VPSS_HDR_COEF_BUF_HDR5];
}

hi_s32 pq_hal_set_vpsshdr_csc_setting(hi_pq_image_param *pic_setting)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(pic_setting);

    /* gfx csc use 0 -100, convert to 0 - 255 to sdr2hdr */
    g_vpsshdr_bright = LEVEL2NUM(pic_setting->brightness);
    g_vpsshdr_contrast = LEVEL2NUM(pic_setting->contrast);
    g_vpsshdr_satu = LEVEL2NUM(pic_setting->hue);
    g_vpsshdr_hue = LEVEL2NUM(pic_setting->saturation);
    g_vpsshdr_wcg_temperature = LEVEL2NUM(pic_setting->wcg_temperature);

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_vpsshdr_offset(hi_pq_hdr_offset *hdr_offset_para)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_offset_para);

    g_vpsshdr_offset_bright[hdr_offset_para->hdr_process_scene] = hdr_offset_para->bright;
    g_vpsshdr_offset_contrast[hdr_offset_para->hdr_process_scene] = hdr_offset_para->contrast;
    g_vpsshdr_offset_satu[hdr_offset_para->hdr_process_scene] = hdr_offset_para->satu;
    g_vpsshdr_offset_hue[hdr_offset_para->hdr_process_scene] = hdr_offset_para->hue;
    g_vpsshdr_offset_red[hdr_offset_para->hdr_process_scene] = hdr_offset_para->r;
    g_vpsshdr_offset_green[hdr_offset_para->hdr_process_scene] = hdr_offset_para->g;
    g_vpsshdr_offset_blue[hdr_offset_para->hdr_process_scene] = hdr_offset_para->b;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_vpsshdr_tm_curve(hi_pq_hdr_offset *hdr_offset_para)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_offset_para);

    g_vpsshdr_dark_cv_str[hdr_offset_para->hdr_process_scene] = hdr_offset_para->dark_cv;
    g_vpsshdr_bright_cv_str[hdr_offset_para->hdr_process_scene] = hdr_offset_para->bright_cv;

    return HI_SUCCESS;
}

static hi_void pq_hal_vpsshdr_get_yuv2rgb_imap_matrix(drv_pq_internal_color_space in_cs,
                                                      hi_drv_pq_imap_cfg *imap_cfg)
{
    switch (in_cs) {
        case HI_DRV_CS_BT601_YUV_LIMITED:
            imap_cfg->yuv2rgb.scale2p = g_vpsshdr_m33_yuv2rgb_scale2p - 2; /* 11 : scale2p */
            memcpy(imap_cfg->yuv2rgb.coef, g_vpsshdr_m33_yuv2rgb_bt601_yl2bt709_rf, sizeof(imap_cfg->yuv2rgb.coef));
            memcpy(imap_cfg->yuv2rgb.offinrgb,
                   g_vpsshdr_v3_yuv2rgb_off_in_rgb_bt601_yl2bt709_rf, sizeof(imap_cfg->yuv2rgb.offinrgb));
            break;
        case HI_DRV_CS_BT601_YUV_FULL:
            imap_cfg->yuv2rgb.scale2p = g_vpsshdr_m33_yuv2rgb_scale2p - 2; /* 11 : scale2p */
            memcpy(imap_cfg->yuv2rgb.coef, g_vpsshdr_m33_yuv2rgb_bt601_yf2bt709_rf, sizeof(imap_cfg->yuv2rgb.coef));
            memcpy(imap_cfg->yuv2rgb.offinrgb,
                   g_vpsshdr_v3_yuv2rgb_off_in_rgb_bt601_yf2bt709_rf, sizeof(imap_cfg->yuv2rgb.offinrgb));
            break;
        case HI_DRV_CS_BT709_YUV_LIMITED:
            imap_cfg->yuv2rgb.scale2p = g_vpsshdr_m33_yuv2rgb_scale2p - 2; /* 11 : scale2p */
            memcpy(imap_cfg->yuv2rgb.coef, g_vpsshdr_m33_yuv2rgb_bt709_yl2rf_2, sizeof(imap_cfg->yuv2rgb.coef));
            memcpy(imap_cfg->yuv2rgb.offinrgb,
                   g_vpsshdr_v3_yuv2rgb_off_in_rgb_bt709_yl2rf, sizeof(imap_cfg->yuv2rgb.offinrgb));
            break;
        case HI_DRV_CS_BT709_YUV_FULL:
            imap_cfg->yuv2rgb.scale2p = g_vpsshdr_m33_yuv2rgb_scale2p - 2; /* 11 : scale2p */
            memcpy(imap_cfg->yuv2rgb.coef, g_vpsshdr_m33_yuv2rgb_bt709_yf2rf, sizeof(imap_cfg->yuv2rgb.coef));
            memcpy(imap_cfg->yuv2rgb.offinrgb,
                   g_vpsshdr_v3_yuv2rgb_off_in_rgb_bt709_yf2rf, sizeof(imap_cfg->yuv2rgb.offinrgb));
            break;
        case HI_DRV_CS_BT2020_YUV_LIMITED:
            imap_cfg->yuv2rgb.scale2p = g_vpsshdr_m33_yuv2rgb_scale2p - 2; /* 11 : scale2p */
            memcpy(imap_cfg->yuv2rgb.coef, g_vpsshdr_m33_yuv2rgb_bt2020_yl2rf_2, sizeof(imap_cfg->yuv2rgb.coef));
            memcpy(imap_cfg->yuv2rgb.offinrgb,
                   g_vpsshdr_v3_yuv2rgb_off_in_rgb_bt2020_yl2rf, sizeof(imap_cfg->yuv2rgb.offinrgb));
            break;
        case HI_DRV_CS_BT2020_YUV_FULL:
            imap_cfg->yuv2rgb.scale2p = g_vpsshdr_m33_yuv2rgb_scale2p - 2; /* 11 : scale2p */
            memcpy(imap_cfg->yuv2rgb.coef, g_vpsshdr_m33_yuv2rgb_bt2020_yf2rf, sizeof(imap_cfg->yuv2rgb.coef));
            memcpy(imap_cfg->yuv2rgb.offinrgb,
                   g_vpsshdr_v3_yuv2rgb_off_in_rgb_bt2020_yf2rf, sizeof(imap_cfg->yuv2rgb.offinrgb));
            break;
        case HI_DRV_CS_BT601_RGB_LIMITED:
        case HI_DRV_CS_BT601_RGB_FULL:
        case HI_DRV_CS_BT709_RGB_LIMITED:
        case HI_DRV_CS_BT709_RGB_FULL:
        case HI_DRV_CS_BT2020_RGB_LIMITED:
        case HI_DRV_CS_BT2020_RGB_FULL:
            break;
        default:
            break;
    }
}

hi_void pq_hal_vpsshdr_get_yuv2rgb_matrix(drv_pq_internal_color_space in_cs,
                                          drv_pq_internal_color_space out_cs,
                                          hi_drv_pq_imap_cfg *imap_cfg)
{
    imap_cfg->yuv2rgb.mode = 3;             /* 3 : mode,BT2020CL should set 2 */
    imap_cfg->yuv2rgb.clip_max = 134215680; /* 134215680 : clip_max */
    imap_cfg->yuv2rgb.clip_min = 0;
    imap_cfg->yuv2rgb.thr_r = 0;
    imap_cfg->yuv2rgb.thr_b = 0;

    memcpy(imap_cfg->yuv2rgb.dc_in, g_vpsshdr_y2r_dc_in, sizeof(imap_cfg->yuv2rgb.dc_in));
    memcpy(imap_cfg->yuv2rgb.dc_in2, g_vpsshdr_y2r_dc_in, sizeof(imap_cfg->yuv2rgb.dc_in2));
    memcpy(imap_cfg->yuv2rgb.offinrgb2, g_vpsshdr_y2r_offinrgb2, sizeof(imap_cfg->yuv2rgb.offinrgb2));
    memcpy(imap_cfg->yuv2rgb.coef2, g_vpsshdr_y2r_m33_2, sizeof(imap_cfg->yuv2rgb.coef2));

    pq_hal_vpsshdr_get_yuv2rgb_imap_matrix(in_cs, imap_cfg);
}

hi_void pq_hal_vpsshdr_get_imap_cacm_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
                                         drv_pq_internal_color_space out_cs, hi_drv_pq_imap_cfg *imap_cfg)
{
    /* module on/off according 709/2020, pos according HDR/SDR */
    imap_cfg->cacm.demo_pos = 0;
    imap_cfg->cacm.demo_mode = 0;
    imap_cfg->cacm.bitdepth_out_mode = 3; /* 3: bitdepth_out_mode */
    imap_cfg->cacm.bitdepth_in_mode = 3; /* 3: bitdepth_in_mode */

    memcpy(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_r,
           g_vpsshdr_cm_lut_bt709rf2yl_r, sizeof(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_r));
    memcpy(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_g,
           g_vpsshdr_cm_lut_bt709rf2yl_g, sizeof(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_g));
    memcpy(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_b,
           g_vpsshdr_cm_lut_bt709rf2yl_b, sizeof(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_b));

    if (out_cs == HI_DRV_CS_BT709_YUV_LIMITED || out_cs == HI_DRV_CS_BT709_YUV_FULL) {
        memcpy(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_r,
               g_vpsshdr_cm_lut_bt709rf2yl_r, sizeof(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_r));
        memcpy(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_g,
               g_vpsshdr_cm_lut_bt709rf2yl_g, sizeof(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_g));
        memcpy(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_b,
               g_vpsshdr_cm_lut_bt709rf2yl_b, sizeof(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_b));
    } else if (out_cs == HI_DRV_CS_BT601_YUV_LIMITED || out_cs == HI_DRV_CS_BT601_YUV_FULL) {
        memcpy(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_r,
               g_vpsshdr_cm_lut_bt709rf2yl_r, sizeof(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_r));
        memcpy(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_g,
               g_vpsshdr_cm_lut_bt709rf2yl_g, sizeof(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_g));
        memcpy(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_b,
               g_vpsshdr_cm_lut_bt709rf2yl_b, sizeof(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_b));
    } else if (out_cs == HI_DRV_CS_BT2020_YUV_LIMITED || out_cs == HI_DRV_CS_BT2020_YUV_FULL) {
        memcpy(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_r,
               g_vpsshdr_cm_lut_bt2020rf2yl_r, sizeof(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_r));
        memcpy(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_g,
               g_vpsshdr_cm_lut_bt2020rf2yl_g, sizeof(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_g));
        memcpy(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_b,
               g_vpsshdr_cm_lut_bt2020rf2yl_b, sizeof(g_pq_vpsshdr_param[handle].vpsshdr_imap_cacm_lut_b));
    }
}

hi_void pq_hal_vpsshdr_get_rgb2lms_matrix(drv_pq_internal_color_space in_cs, drv_pq_internal_color_space out_cs,
                                          hi_drv_pq_imap_cfg *imap_cfg)
{
    /* hdrv1_mode : RGB2RGB */
    imap_cfg->rgb2lms.scale2p = g_vpsshdr_map_scale; /* 14 : scale2p */
    imap_cfg->rgb2lms.clip_min = 0;
    imap_cfg->rgb2lms.clip_max = 2621440000; /* 2621440000 is max clip */
    memcpy(imap_cfg->rgb2lms.dc_in, g_vpsshdr_hdr2sdr_imap_r2l_dc_in, sizeof(imap_cfg->rgb2lms.dc_in));
    if ((in_cs == HI_DRV_CS_BT2020_YUV_LIMITED || in_cs == HI_DRV_CS_BT2020_YUV_FULL ||
         in_cs == HI_DRV_CS_BT2020_RGB_LIMITED || in_cs == HI_DRV_CS_BT2020_RGB_FULL) &&
        (out_cs == HI_DRV_CS_BT709_YUV_LIMITED || out_cs == HI_DRV_CS_BT709_YUV_FULL ||
         out_cs == HI_DRV_CS_BT709_RGB_LIMITED || out_cs == HI_DRV_CS_BT709_RGB_FULL)) {
        memcpy(imap_cfg->rgb2lms.coef, g_vpsshdr_map_m33_2020_ncl_709, sizeof(imap_cfg->rgb2lms.coef));
    } else if ((in_cs == HI_DRV_CS_BT709_YUV_LIMITED || in_cs == HI_DRV_CS_BT709_YUV_FULL ||
                in_cs == HI_DRV_CS_BT709_RGB_LIMITED || in_cs == HI_DRV_CS_BT709_RGB_FULL) &&
               (out_cs == HI_DRV_CS_BT2020_YUV_LIMITED || out_cs == HI_DRV_CS_BT2020_YUV_FULL ||
                out_cs == HI_DRV_CS_BT2020_RGB_LIMITED || out_cs == HI_DRV_CS_BT2020_RGB_FULL)) {
        memcpy(imap_cfg->rgb2lms.coef, g_vpsshdr_map_m33_709_2020_ncl, sizeof(imap_cfg->rgb2lms.coef));
    } else if ((in_cs == HI_DRV_CS_BT601_YUV_LIMITED || in_cs == HI_DRV_CS_BT601_YUV_FULL ||
                in_cs == HI_DRV_CS_BT601_RGB_LIMITED || in_cs == HI_DRV_CS_BT601_RGB_FULL) &&
               (out_cs == HI_DRV_CS_BT2020_YUV_LIMITED || out_cs == HI_DRV_CS_BT2020_YUV_FULL ||
                out_cs == HI_DRV_CS_BT2020_RGB_LIMITED || out_cs == HI_DRV_CS_BT2020_RGB_FULL)) {
        memcpy(imap_cfg->rgb2lms.coef, g_vpsshdr_map_m33_709_2020_ncl, sizeof(imap_cfg->rgb2lms.coef));
    } else {
        memcpy(imap_cfg->rgb2lms.coef, g_vpsshdr_map_m33_linear, sizeof(imap_cfg->rgb2lms.coef));
    }
    if (imap_cfg->cacm.enable == HI_TRUE) {
        memcpy(imap_cfg->rgb2lms.coef, g_vpsshdr_map_m33_linear, sizeof(imap_cfg->rgb2lms.coef));
    }

    return;
}

hi_void pq_hal_vpsshdr_get_tm_v1_lut(hi_u32 handle, hi_drv_pq_imap_cfg *imap_cfg)
{
    switch (g_vpsshdr_mode[handle]) {
        case HI_PQ_HDR_MODE_HDR10_TO_HDR10:
            imap_cfg->tmap_v1.scale_coef = g_vpsshdr_tm_v1_scale_coef_linear;
            g_pq_vpsshdr_param[handle].vpsshdr_tmapv1_lut = g_vpsshdr_tm_v1_lut_linear;
            break;
        case HI_PQ_HDR_MODE_HDR10_TO_HLG:
            imap_cfg->tmap_v1.scale_coef = g_vpsshdr_tm_v1_scale_coef_pq2hlg;
            g_pq_vpsshdr_param[handle].vpsshdr_tmapv1_lut = g_vpsshdr_tm_v1_lut_pq2hlg;
            break;
        case HI_PQ_HDR_MODE_HDR10_TO_SDR:
            imap_cfg->tmap_v1.scale_coef = g_vpsshdr_tm_v1_scale_coef_pq2sdr;
            g_pq_vpsshdr_param[handle].vpsshdr_tmapv1_lut = g_vpsshdr_tm_v1_lut_pq2sdr;
            break;
        case HI_PQ_HDR_MODE_HLG_TO_HDR10:
            imap_cfg->tmap_v1.scale_coef = g_vpsshdr_tm_v1_scale_coef_hlg2pq;
            g_pq_vpsshdr_param[handle].vpsshdr_tmapv1_lut = g_vpsshdr_tm_v1_lut_hlg2pq;
            break;
        case HI_PQ_HDR_MODE_HLG_TO_HLG:
            imap_cfg->tmap_v1.scale_coef = g_vpsshdr_tm_v1_scale_coef_linear;
            g_pq_vpsshdr_param[handle].vpsshdr_tmapv1_lut = g_vpsshdr_tm_v1_lut_linear;
            break;
        case HI_PQ_HDR_MODE_HLG_TO_SDR:
            imap_cfg->tmap_v1.scale_coef = g_vpsshdr_tm_v1_scale_coef_hlg2sdr;
            g_pq_vpsshdr_param[handle].vpsshdr_tmapv1_lut = g_vpsshdr_tm_v1_lut_hlg2sdr;
            break;
        case HI_PQ_HDR_MODE_SDR_TO_HDR10:
            imap_cfg->tmap_v1.scale_coef = g_vpsshdr_tm_v1_scale_coef_sdr2pq;
            g_pq_vpsshdr_param[handle].vpsshdr_tmapv1_lut = g_vpsshdr_tm_v1_lut_sdr2pq;
            break;
        case HI_PQ_HDR_MODE_SDR_TO_HLG:
            imap_cfg->tmap_v1.scale_coef = g_vpsshdr_tm_v1_scale_coef_sdr2hlg;
            g_pq_vpsshdr_param[handle].vpsshdr_tmapv1_lut = g_vpsshdr_tm_v1_lut_sdr2hlg;
            break;
        case HI_PQ_HDR_MODE_SDR_TO_SDR:
            imap_cfg->tmap_v1.scale_coef = g_vpsshdr_tm_v1_scale_coef_linear;
            g_pq_vpsshdr_param[handle].vpsshdr_tmapv1_lut = g_vpsshdr_tm_v1_lut_linear;
            break;
        default:
            imap_cfg->tmap_v1.scale_coef = g_vpsshdr_tm_v1_scale_coef_linear;
            g_pq_vpsshdr_param[handle].vpsshdr_tmapv1_lut = g_vpsshdr_tm_v1_lut_linear;
            break;
    }
}
hi_void pq_hal_vpsshdr_get_tmap_v1_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
                                       drv_pq_internal_color_space out_cs, hi_drv_pq_imap_cfg *imap_cfg)
{
    /* related with scene, para change in case */
    imap_cfg->tmap_v1.tm_pos = 0;
    imap_cfg->tmap_v1.scale_mix_alpha = 15; /* 15 is coef */
    imap_cfg->tmap_v1.mix_alpha = 0;

    imap_cfg->tmap_v1.clip_min = g_vpsshdr_tm_clip_min;
    imap_cfg->tmap_v1.clip_max = g_vpsshdr_tm_clip_max;

    imap_cfg->tmap_v1.scale_lum_cal = 15; /* 15 is coef */
    imap_cfg->tmap_v1.scale_coef = 15; /* 15 is coef */
    memcpy(imap_cfg->tmap_v1.step, g_vpsshdr_tm_xstep, sizeof(imap_cfg->tmap_v1.step));
    memcpy(imap_cfg->tmap_v1.pos, g_vpsshdr_tm_xpos, sizeof(imap_cfg->tmap_v1.pos));
    memcpy(imap_cfg->tmap_v1.num, g_vpsshdr_tm_xnum, sizeof(imap_cfg->tmap_v1.num));
    memcpy(imap_cfg->tmap_v1.lum_cal, g_vpsshdr_tm_lum_cal, sizeof(imap_cfg->tmap_v1.lum_cal));
    memcpy(imap_cfg->tmap_v1.dc_out, g_vpsshdr_tm_dc_out, sizeof(imap_cfg->tmap_v1.dc_out));

    pq_hal_vpsshdr_get_tm_v1_lut(handle, imap_cfg);
}

hi_void pq_hal_vpsshdr_get_lms2ipt_matrix(drv_pq_internal_color_space in_cs, drv_pq_internal_color_space out_cs,
                                          hi_drv_pq_imap_cfg *imap_cfg)
{
    /* RGB2YUV without DcOut */
    imap_cfg->lms2ipt.scale2p = g_vpsshdr_m33_rgb2yuv_scale2p; /* 12: lms2ipt scale */
    imap_cfg->lms2ipt.clip_min_y = 0;                          /* -32767: lms2ipt clip */
    imap_cfg->lms2ipt.clip_max_y = 32767;                      /* 32767: lms2ipt scale */
    imap_cfg->lms2ipt.clip_min_c = 0;                          /* -32767: lms2ipt clip */
    imap_cfg->lms2ipt.clip_max_c = 32767;                      /* 32767: lms2ipt scale */

    memcpy(imap_cfg->lms2ipt.coef, g_vpsshdr_m33_rgb2yuv_linear, sizeof(imap_cfg->lms2ipt.coef));
    memcpy(imap_cfg->lms2ipt.dc_in, g_vpsshdr_v3_rgb2yuv_off_dc_zero, sizeof(imap_cfg->lms2ipt.dc_in));
    memcpy(imap_cfg->lms2ipt.dc_out, g_vpsshdr_lms2ipt_dc_out, sizeof(imap_cfg->lms2ipt.dc_out));

    switch (out_cs) {
        case HI_DRV_CS_BT601_YUV_LIMITED:
            memcpy(imap_cfg->lms2ipt.coef, g_vpsshdr_m33_rgb2yuv_bt709rf_to_bt601yl, sizeof(imap_cfg->lms2ipt.coef));
            break;
        case HI_DRV_CS_BT601_YUV_FULL:
            memcpy(imap_cfg->lms2ipt.coef, g_vpsshdr_m33_rgb2yuv_bt709rf_to_bt601yf, sizeof(imap_cfg->lms2ipt.coef));
            break;
        case HI_DRV_CS_BT709_YUV_LIMITED:
            memcpy(imap_cfg->lms2ipt.coef, g_vpsshdr_m33_rgb2yuv_bt709_rf2yl_2, sizeof(imap_cfg->lms2ipt.coef));
            break;
        case HI_DRV_CS_BT709_YUV_FULL:
            memcpy(imap_cfg->lms2ipt.coef, g_vpsshdr_m33_rgb2yuv_bt709rf2yf, sizeof(imap_cfg->lms2ipt.coef));
            break;
        case HI_DRV_CS_BT2020_YUV_LIMITED:
            memcpy(imap_cfg->lms2ipt.coef, g_vpsshdr_m33_rgb2yuv_bt2020_rf2yl_2, sizeof(imap_cfg->lms2ipt.coef));
            break;
        case HI_DRV_CS_BT2020_YUV_FULL:
            memcpy(imap_cfg->lms2ipt.coef, g_vpsshdr_m33_rgb2yuv_bt2020rf2yf, sizeof(imap_cfg->lms2ipt.coef));
            break;
        case HI_DRV_CS_BT601_RGB_LIMITED:
        case HI_DRV_CS_BT601_RGB_FULL:
        case HI_DRV_CS_BT709_RGB_LIMITED:
        case HI_DRV_CS_BT709_RGB_FULL:
        case HI_DRV_CS_BT2020_RGB_LIMITED:
        case HI_DRV_CS_BT2020_RGB_FULL:
        default:
            memcpy(imap_cfg->lms2ipt.coef, g_vpsshdr_m33_rgb2yuv_linear, sizeof(imap_cfg->lms2ipt.coef));
            break;
    }
}

hi_void pq_hal_vpsshdr_get_imap_default_cfg(hi_drv_pq_imap_cfg *imap_cfg)
{
    /* default para, every scene not change it */
    imap_cfg->v0_hdr_enable = HI_TRUE;
    imap_cfg->v1_hdr_enable = HI_FALSE;

    imap_cfg->yuv2rgb.v0_enable = HI_TRUE;
    imap_cfg->yuv2rgb.v1_enable = HI_FALSE; /* PIP use, this version only use v0 */

    imap_cfg->eotfparam_eotf = 1;
    imap_cfg->eotfparam_range_min = 0;
    imap_cfg->eotfparam_range = 16383;     /* 16383: range */
    imap_cfg->eotfparam_range_inv = 65540; /* 65540: range */
    imap_cfg->v3ipt_off[0] = 0;
    imap_cfg->v3ipt_off[1] = 0;
    imap_cfg->v3ipt_off[2] = 0; /* 2: index */
    imap_cfg->ipt_scale = 0;
    imap_cfg->degamma_clip_max = 0; /* 2621440000UL: clip max, logic delete it */
    imap_cfg->degamma_clip_min = 0; /* logic delete it */

    imap_cfg->in_color = 0; /* reserve reg, ret 0 */
    imap_cfg->in_bits = 1;

    /* dolby used, HIHDR close */
    imap_cfg->demo_luma_en = HI_FALSE;
    imap_cfg->ladj_chroma_weight = 0; /* ladj_en close in HIHDR, not config it */
    imap_cfg->ladj_en = HI_FALSE;

    imap_cfg->ipt_in_sel = HI_FALSE;

    imap_cfg->norm_en = HI_FALSE; /* HIHDR close it */

    imap_cfg->rshift_round_en = HI_FALSE; /* logic delete it */
    imap_cfg->rshift_en = HI_FALSE;       /* logic delete it */

    return;
}

static hi_s32 pq_hal_vpsshdr_get_hdrv1cm_cfg(drv_pq_internal_color_space in_cs,
                                             drv_pq_internal_color_space out_cs,
                                             pq_vpsshdr_v1_cm_cfg *cm_cfg)
{
    cm_cfg->clip_c_max = 0; /* 对应stHiHDRCmCfg.u16ClipMaxUV */
    cm_cfg->clip_c_min = 0; /* 对应stHiHDRCmCfg.u16ClipMinUV */
    cm_cfg->scale2p = 0; /* 对应stHiHDRCmCfg.u16ScaleCoefCM */

    return HI_SUCCESS;
}

hi_s32 pq_hal_vpsshdr_get_sdr2sdr_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                                drv_pq_internal_color_space out_cs,
                                                hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_TRUE;
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_VPSSHDR_CM_POS1;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmapv1 */
    imap_cfg->tmap_v1.enable = HI_FALSE;
    imap_cfg->tmap_v1.tm_pos = PQ_VPSSHDR_TM_POS0; /* 这个的绑定位置如何决策？ */

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_FALSE;

    pq_hal_vpsshdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_vpsshdr_get_sdr2hdr_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                                drv_pq_internal_color_space out_cs,
                                                hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_TRUE;
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_VPSSHDR_CM_POS1;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_VPSSHDR_TM_POS1;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_FALSE;

    pq_hal_vpsshdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_vpsshdr_get_sdr2hlg_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                                drv_pq_internal_color_space out_cs,
                                                hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_TRUE;
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_VPSSHDR_CM_POS1;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_VPSSHDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_FALSE;

    pq_hal_vpsshdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_vpsshdr_get_hdr2sdr_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                                drv_pq_internal_color_space out_cs,
                                                hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_TRUE;
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_VPSSHDR_CM_POS1;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_VPSSHDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_FALSE;

    pq_hal_vpsshdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_vpsshdr_get_hdr2hdr_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                                drv_pq_internal_color_space out_cs,
                                                hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_TRUE;
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_VPSSHDR_CM_POS1;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_FALSE;
    imap_cfg->tmap_v1.tm_pos = PQ_VPSSHDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_FALSE;

    pq_hal_vpsshdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_vpsshdr_get_hdr2hlg_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                                drv_pq_internal_color_space out_cs,
                                                hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_TRUE;
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_VPSSHDR_CM_POS1;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_VPSSHDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_FALSE;

    pq_hal_vpsshdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_vpsshdr_get_hlg2sdr_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                                drv_pq_internal_color_space out_cs,
                                                hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_TRUE;
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_VPSSHDR_CM_POS1;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_VPSSHDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_FALSE;

    pq_hal_vpsshdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_vpsshdr_get_hlg2hdr_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                                drv_pq_internal_color_space out_cs,
                                                hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_TRUE;
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_VPSSHDR_CM_POS1;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_VPSSHDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_FALSE;

    pq_hal_vpsshdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_vpsshdr_get_hlg2hlg_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                                drv_pq_internal_color_space out_cs,
                                                hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_TRUE;

    /* cacm */
    imap_cfg->cacm.enable = HI_TRUE;
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_VPSSHDR_CM_POS1;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_FALSE;
    imap_cfg->tmap_v1.tm_pos = PQ_VPSSHDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_FALSE;

    pq_hal_vpsshdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

static hi_void pq_hal_vpsshdr_get_tmap_default_cfg(pq_vpsshdr_tmap_cfg *tmap_cfg)
{
    /* default para, every scene not change it */
    tmap_cfg->s2u_en = HI_TRUE; /* alway open it */

    tmap_cfg->sclut_rd_en = HI_FALSE; /* only debug use */
    tmap_cfg->sslut_rd_en = HI_FALSE; /* only debug use */
    tmap_cfg->tslut_rd_en = HI_FALSE; /* only debug use */
    tmap_cfg->silut_rd_en = HI_FALSE; /* only debug use */
    tmap_cfg->tilut_rd_en = HI_FALSE; /* only debug use */

    tmap_cfg->rshift_en = HI_FALSE;
    tmap_cfg->rshift_bit = 0;

    tmap_cfg->para_rdata = 0; /* read only reg, debug used it, not config */

    return;
}

static hi_s32 pq_hal_vpsshdr_get_tmap_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
    drv_pq_internal_color_space out_cs, pq_vpsshdr_tmap_cfg *tmap_cfg)
{
    switch (g_vpsshdr_mode[handle]) {
        case HI_PQ_HDR_MODE_HDR10_TO_SDR:
            tmap_cfg->rshift_round_en = HI_TRUE;

            tmap_cfg->smc_enable = HI_TRUE;
            tmap_cfg->c1_expan = 1382; /* 1382: c1_expan */

            break;
        case HI_PQ_HDR_MODE_SDR_TO_HDR10:
            tmap_cfg->rshift_round_en = HI_TRUE;

            tmap_cfg->smc_enable = HI_FALSE;
            tmap_cfg->c1_expan = 1 << 10; /* 10: c1_expan rshift len */
            break;
        default:
            tmap_cfg->rshift_round_en = HI_TRUE;
            tmap_cfg->smc_enable = HI_FALSE;
            tmap_cfg->c1_expan = 1 << 10; /* 10: c1_expan rshift len */
            break;
    }

    pq_hal_vpsshdr_get_tmap_default_cfg(tmap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpss_hdr2sdr_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
                                   drv_pq_internal_color_space out_cs,
                                   pq_hal_vpsshdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_v1_cm_lut = g_vpsshdr_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        /* 1. get imap base cfg */
        pq_hal_vpsshdr_get_hdr2sdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 2. yuv2rgb */
        pq_hal_vpsshdr_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 3. cacm */
        pq_hal_vpsshdr_get_imap_cacm_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 4. degamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_degamma = g_vpsshdr_degmm_lut_pq;

        /* 5. rgb2lms */
        pq_hal_vpsshdr_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 6. tone_mapping tmap v1 */
        pq_hal_vpsshdr_get_tmap_v1_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 7. gamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_x = g_vpsshdr_im_gmm_lutx;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_a = g_vpsshdr_im_gmm_luta_gmm22;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_b = g_vpsshdr_im_gmm_lutb_gmm22;

        /* 8. lms2ipt */
        pq_hal_vpsshdr_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_tmap_cfg(handle, in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_i = g_vpsshdr_tm_lut_i_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_i = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_smc_lut = g_vpsshdr_sm_lut_bypass;
    }

    pq_hal_vpsshdr_get_hdr_coef(handle, hdr_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpss_hdr2hdr_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
                                   drv_pq_internal_color_space out_cs,
                                   pq_hal_vpsshdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    if (in_cs == out_cs) {
        hdr_cfg->imap_cfg.enable = HI_FALSE;
        hdr_cfg->tmap_cfg.enable = HI_FALSE;
        return HI_SUCCESS;
    }

    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_v1_cm_lut = g_vpsshdr_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        /* 1. get imap base cfg */
        pq_hal_vpsshdr_get_hdr2hdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 2. yuv2rgb */
        pq_hal_vpsshdr_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 3. cacm */
        pq_hal_vpsshdr_get_imap_cacm_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 4. degamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_degamma = g_vpsshdr_degmm_lut_pq;

        /* 5. rgb2lms */
        pq_hal_vpsshdr_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 6. tone_mapping tmap v1 */
        pq_hal_vpsshdr_get_tmap_v1_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 7. gamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_x = g_vpsshdr_im_gmm_lutx;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_a = g_vpsshdr_im_gmm_luta_pq;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_b = g_vpsshdr_im_gmm_lutb_pq;

        /* 8. lms2ipt */
        pq_hal_vpsshdr_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_tmap_cfg(handle, in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_i = g_vpsshdr_tm_lut_i_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_i = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_smc_lut = g_vpsshdr_sm_lut_bypass;
    }

    pq_hal_vpsshdr_get_hdr_coef(handle, hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpss_hdr2hlg_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
                                   drv_pq_internal_color_space out_cs,
                                   pq_hal_vpsshdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_v1_cm_lut = g_vpsshdr_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        /* 1. get imap base cfg */
        pq_hal_vpsshdr_get_hdr2hlg_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 2. yuv2rgb */
        pq_hal_vpsshdr_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 3. cacm */
        pq_hal_vpsshdr_get_imap_cacm_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 4. degamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_degamma = g_vpsshdr_degmm_lut_pq;

        /* 5. rgb2lms */
        pq_hal_vpsshdr_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 6. tone_mapping tmap v1 */
        pq_hal_vpsshdr_get_tmap_v1_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 7. gamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_x = g_vpsshdr_im_gmm_lutx;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_a = g_vpsshdr_im_gmm_luta_hlg;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_b = g_vpsshdr_im_gmm_lutb_hlg;

        /* 8. lms2ipt */
        pq_hal_vpsshdr_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_tmap_cfg(handle, in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_i = g_vpsshdr_tm_lut_i_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_i = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_smc_lut = g_vpsshdr_sm_lut_bypass;
    }

    pq_hal_vpsshdr_get_hdr_coef(handle, hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpss_hlg2sdr_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
                                   drv_pq_internal_color_space out_cs,
                                   pq_hal_vpsshdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_v1_cm_lut = g_vpsshdr_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        /* 1. get imap base cfg */
        pq_hal_vpsshdr_get_hlg2sdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 2. yuv2rgb */
        pq_hal_vpsshdr_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 3. cacm */
        pq_hal_vpsshdr_get_imap_cacm_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 4. degamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_degamma = g_vpsshdr_degmm_lut_hlg_10000;

        /* 5. rgb2lms */
        pq_hal_vpsshdr_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 6. tone_mapping tmap v1 */
        pq_hal_vpsshdr_get_tmap_v1_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 7. gamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_x = g_vpsshdr_im_gmm_lutx;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_a = g_vpsshdr_im_gmm_luta_gmm22;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_b = g_vpsshdr_im_gmm_lutb_gmm22;

        /* 8. lms2ipt */
        pq_hal_vpsshdr_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_tmap_cfg(handle, in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_i = g_vpsshdr_tm_lut_i_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_i = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_smc_lut = g_vpsshdr_sm_lut_bypass;
    }

    pq_hal_vpsshdr_get_hdr_coef(handle, hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpss_hlg2hdr_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
                                   drv_pq_internal_color_space out_cs,
                                   pq_hal_vpsshdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_v1_cm_lut = g_vpsshdr_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        /* 1. get imap base cfg */
        pq_hal_vpsshdr_get_hlg2hdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 2. yuv2rgb */
        pq_hal_vpsshdr_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 3. cacm */
        pq_hal_vpsshdr_get_imap_cacm_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 4. degamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_degamma = g_vpsshdr_degmm_lut_hlg_10000;

        /* 5. rgb2lms */
        pq_hal_vpsshdr_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 6. tone_mapping tmap v1 */
        pq_hal_vpsshdr_get_tmap_v1_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 7. gamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_x = g_vpsshdr_im_gmm_lutx;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_a = g_vpsshdr_im_gmm_luta_pq;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_b = g_vpsshdr_im_gmm_lutb_pq;

        /* 8. lms2ipt */
        pq_hal_vpsshdr_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_tmap_cfg(handle, in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_i = g_vpsshdr_tm_lut_i_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_i = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_smc_lut = g_vpsshdr_sm_lut_bypass;
    }

    pq_hal_vpsshdr_get_hdr_coef(handle, hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpss_hlg2hlg_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
                                   drv_pq_internal_color_space out_cs,
                                   pq_hal_vpsshdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_v1_cm_lut = g_vpsshdr_cm_lut_linear;
    }

    if (in_cs == out_cs) {  // set Bypass Mode
        hdr_cfg->imap_cfg.enable = HI_FALSE;
        hdr_cfg->tmap_cfg.enable = HI_FALSE;
        return HI_SUCCESS;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        /* 1. get imap base cfg */
        pq_hal_vpsshdr_get_hlg2hlg_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 2. yuv2rgb */
        pq_hal_vpsshdr_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 3. cacm */
        pq_hal_vpsshdr_get_imap_cacm_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 4. degamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_degamma = g_vpsshdr_degmm_lut_hlg_10000;

        /* 5. rgb2lms */
        pq_hal_vpsshdr_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 6. tone_mapping tmap v1 */
        pq_hal_vpsshdr_get_tmap_v1_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 7. gamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_x = g_vpsshdr_im_gmm_lutx;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_a = g_vpsshdr_im_gmm_luta_hlg;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_b = g_vpsshdr_im_gmm_lutb_hlg;

        /* 8. lms2ipt */
        pq_hal_vpsshdr_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_tmap_cfg(handle, in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_i = g_vpsshdr_tm_lut_i_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_i = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_smc_lut = g_vpsshdr_sm_lut_bypass;
    }

    pq_hal_vpsshdr_get_hdr_coef(handle, hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpss_sdr2hdr_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
                                   drv_pq_internal_color_space out_cs,
                                   pq_hal_vpsshdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_v1_cm_lut = g_vpsshdr_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        /* 1. get imap base cfg */
        pq_hal_vpsshdr_get_sdr2hdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 2. yuv2rgb */
        pq_hal_vpsshdr_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 3. cacm */
        pq_hal_vpsshdr_get_imap_cacm_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 4. degamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_degamma = g_vpsshdr_degmm_lut_gmm24_10000;

        /* 5. rgb2lms */
        pq_hal_vpsshdr_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 6. tone_mapping tmap v1 */
        pq_hal_vpsshdr_get_tmap_v1_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 7. gamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_x = g_vpsshdr_im_gmm_lutx;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_a = g_vpsshdr_im_gmm_luta_pq;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_b = g_vpsshdr_im_gmm_lutb_pq;

        /* 8. lms2ipt */
        pq_hal_vpsshdr_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_tmap_cfg(handle, in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_i = g_vpsshdr_tm_lut_i_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_i = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_smc_lut = g_vpsshdr_sm_lut_bypass;
    }

    pq_hal_vpsshdr_get_hdr_coef(handle, hdr_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpss_sdr2hlg_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
                                   drv_pq_internal_color_space out_cs,
                                   pq_hal_vpsshdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_v1_cm_lut = g_vpsshdr_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        /* 1. get imap base cfg */
        pq_hal_vpsshdr_get_sdr2hlg_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 2. yuv2rgb */
        pq_hal_vpsshdr_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 3. cacm */
        pq_hal_vpsshdr_get_imap_cacm_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 4. degamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_degamma = g_vpsshdr_degmm_lut_gmm24_10000;

        /* 5. rgb2lms */
        pq_hal_vpsshdr_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 6. tone_mapping tmap v1 */
        pq_hal_vpsshdr_get_tmap_v1_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 7. gamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_x = g_vpsshdr_im_gmm_lutx;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_a = g_vpsshdr_im_gmm_luta_hlg;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_b = g_vpsshdr_im_gmm_lutb_hlg;

        /* 8. lms2ipt */
        pq_hal_vpsshdr_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_tmap_cfg(handle, in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_i = g_vpsshdr_tm_lut_i_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_i = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_smc_lut = g_vpsshdr_sm_lut_bypass;
    }

    pq_hal_vpsshdr_get_hdr_coef(handle, hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpss_sdr2sdr_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
                                   drv_pq_internal_color_space out_cs,
                                   pq_hal_vpsshdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    if (in_cs == out_cs) {
        hdr_cfg->imap_cfg.enable = HI_FALSE;
        hdr_cfg->tmap_cfg.enable = HI_FALSE;
        return HI_SUCCESS;
    } else if ((g_vpsshdr_mode[handle] == HI_PQ_HDR_MODE_SDR_TO_SDR)
               && (in_cs != HI_DRV_CS_BT2020_YUV_LIMITED) && (in_cs != HI_DRV_CS_BT2020_YUV_FULL)
               && (in_cs != HI_DRV_CS_BT2020_RGB_LIMITED) && (in_cs != HI_DRV_CS_BT2020_RGB_FULL)
               && (out_cs != HI_DRV_CS_BT2020_YUV_LIMITED) && (out_cs != HI_DRV_CS_BT2020_YUV_FULL)
               && (out_cs != HI_DRV_CS_BT2020_RGB_LIMITED) && (out_cs != HI_DRV_CS_BT2020_RGB_FULL)) {
        hdr_cfg->imap_cfg.enable = HI_FALSE;
        hdr_cfg->tmap_cfg.enable = HI_FALSE;
        return HI_SUCCESS;
    }

    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_v1_cm_lut = g_vpsshdr_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        /* 1. get imap base cfg */
        pq_hal_vpsshdr_get_sdr2sdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 2. yuv2rgb */
        pq_hal_vpsshdr_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 3. cacm */
        pq_hal_vpsshdr_get_imap_cacm_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 4. degamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_degamma = g_vpsshdr_degmm_lut_gmm24_10000;

        /* 5. rgb2lms */
        pq_hal_vpsshdr_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 6. tone_mapping tmap v1 */
        pq_hal_vpsshdr_get_tmap_v1_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 7. gamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_x = g_vpsshdr_im_gmm_lutx;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_a = g_vpsshdr_im_gmm_luta_gmm24;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_b = g_vpsshdr_im_gmm_lutb_gmm24;

        /* 8. lms2ipt */
        pq_hal_vpsshdr_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_tmap_cfg(handle, in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_i = g_vpsshdr_tm_lut_i_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_i = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_smc_lut = g_vpsshdr_sm_lut_bypass;
    }

    pq_hal_vpsshdr_get_hdr_coef(handle, hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpss_hdr10plus2sdr_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
                                         drv_pq_internal_color_space out_cs,
                                         pq_hal_vpsshdr_cfg *hdr_cfg)
{
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_v1_cm_lut = g_vpsshdr_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        /* 1. get imap base cfg */
        pq_hal_vpsshdr_get_sdr2hdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 2. yuv2rgb */
        pq_hal_vpsshdr_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 3. cacm */
        pq_hal_vpsshdr_get_imap_cacm_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 4. degamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_degamma = g_vpsshdr_degmm_lut_gmm24_10000;

        /* 5. rgb2lms */
        pq_hal_vpsshdr_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 6. tone_mapping tmap v1 */
        pq_hal_vpsshdr_get_tmap_v1_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 7. gamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_x = g_vpsshdr_im_gmm_lutx;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_a = g_vpsshdr_im_gmm_luta_gmm24;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_b = g_vpsshdr_im_gmm_lutb_gmm24;

        /* 8. lms2ipt */
        pq_hal_vpsshdr_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_tmap_cfg(handle, in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_i = g_vpsshdr_tm_lut_i_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_i = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_smc_lut = g_vpsshdr_sm_lut_bypass;
    }

    pq_hal_vpsshdr_get_hdr_coef(handle, hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpss_hdr10plus2hdr10_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
                                           drv_pq_internal_color_space out_cs,
                                           pq_hal_vpsshdr_cfg *hdr_cfg)
{
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_v1_cm_lut = g_vpsshdr_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        /* 1. get imap base cfg */
        pq_hal_vpsshdr_get_sdr2hdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 2. yuv2rgb */
        pq_hal_vpsshdr_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 3. cacm */
        pq_hal_vpsshdr_get_imap_cacm_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 4. degamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_degamma = g_vpsshdr_degmm_lut_gmm24_10000;

        /* 5. rgb2lms */
        pq_hal_vpsshdr_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 6. tone_mapping tmap v1 */
        pq_hal_vpsshdr_get_tmap_v1_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 7. gamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_x = g_vpsshdr_im_gmm_lutx;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_a = g_vpsshdr_im_gmm_luta_gmm24;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_b = g_vpsshdr_im_gmm_lutb_gmm24;

        /* 8. lms2ipt */
        pq_hal_vpsshdr_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_tmap_cfg(handle, in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_i = g_vpsshdr_tm_lut_i_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_i = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_smc_lut = g_vpsshdr_sm_lut_bypass;
    }

    pq_hal_vpsshdr_get_hdr_coef(handle, hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpss_hdr10plus2hlg_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
                                         drv_pq_internal_color_space out_cs,
                                         pq_hal_vpsshdr_cfg *hdr_cfg)
{
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_v1_cm_lut = g_vpsshdr_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        /* 1. get imap base cfg */
        pq_hal_vpsshdr_get_sdr2hdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 2. yuv2rgb */
        pq_hal_vpsshdr_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 3. cacm */
        pq_hal_vpsshdr_get_imap_cacm_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 4. degamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_degamma = g_vpsshdr_degmm_lut_gmm24_10000;

        /* 5. rgb2lms */
        pq_hal_vpsshdr_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 6. tone_mapping tmap v1 */
        pq_hal_vpsshdr_get_tmap_v1_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 7. gamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_x = g_vpsshdr_im_gmm_lutx;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_a = g_vpsshdr_im_gmm_luta_gmm24;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_b = g_vpsshdr_im_gmm_lutb_gmm24;

        /* 8. lms2ipt */
        pq_hal_vpsshdr_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_tmap_cfg(handle, in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_i = g_vpsshdr_tm_lut_i_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_i = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_smc_lut = g_vpsshdr_sm_lut_bypass;
    }

    pq_hal_vpsshdr_get_hdr_coef(handle, hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpss_hdr10plus2hdr10plus_cfg(hi_u32 handle, drv_pq_internal_color_space in_cs,
                                               drv_pq_internal_color_space out_cs,
                                               pq_hal_vpsshdr_cfg *hdr_cfg)
{
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    /* hdrv1_cm: resever */
    if (hdr_cfg->cm_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_hdrv1cm_cfg(in_cs, out_cs, &hdr_cfg->cm_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_v1_cm_lut = g_vpsshdr_cm_lut_linear;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        /* 1. get imap base cfg */
        pq_hal_vpsshdr_get_sdr2hdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 2. yuv2rgb */
        pq_hal_vpsshdr_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 3. cacm */
        pq_hal_vpsshdr_get_imap_cacm_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 4. degamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_degamma = g_vpsshdr_degmm_lut_gmm24_10000;

        /* 5. rgb2lms */
        pq_hal_vpsshdr_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 6. tone_mapping tmap v1 */
        pq_hal_vpsshdr_get_tmap_v1_cfg(handle, in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 7. gamma */
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_x = g_vpsshdr_im_gmm_lutx;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_a = g_vpsshdr_im_gmm_luta_gmm24;
        g_pq_vpsshdr_param[handle].vpsshdr_imap_gamma_lut_b = g_vpsshdr_im_gmm_lutb_gmm24;

        /* 8. lms2ipt */
        pq_hal_vpsshdr_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_vpsshdr_get_tmap_cfg(handle, in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_i = g_vpsshdr_tm_lut_i_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_tm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_i = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_sm_lut_s = g_vpsshdr_sm_lut_bypass;
        g_pq_vpsshdr_param[handle].vpsshdr_tmap_smc_lut = g_vpsshdr_sm_lut_bypass;
    }

    pq_hal_vpsshdr_get_hdr_coef(handle, hdr_cfg);

    return HI_SUCCESS;
}

pq_vpss_hdr_fun g_pf_pq_hal_get_vpss_hdr_path_cfg[HI_PQ_HDR_MODE_MAX] = {
    { HI_PQ_HDR_MODE_HDR10_TO_SDR,   pq_hal_get_vpss_hdr2sdr_cfg },
    { HI_PQ_HDR_MODE_HDR10_TO_HDR10, pq_hal_get_vpss_hdr2hdr_cfg },
    { HI_PQ_HDR_MODE_HDR10_TO_HLG,   pq_hal_get_vpss_hdr2hlg_cfg },

    { HI_PQ_HDR_MODE_HLG_TO_SDR,   pq_hal_get_vpss_hlg2sdr_cfg },
    { HI_PQ_HDR_MODE_HLG_TO_HDR10, pq_hal_get_vpss_hlg2hdr_cfg },
    { HI_PQ_HDR_MODE_HLG_TO_HLG,   pq_hal_get_vpss_hlg2hlg_cfg },

    { HI_PQ_HDR_MODE_SDR_TO_SDR,   pq_hal_get_vpss_sdr2sdr_cfg },
    { HI_PQ_HDR_MODE_SDR_TO_HDR10, pq_hal_get_vpss_sdr2hdr_cfg },
    { HI_PQ_HDR_MODE_SDR_TO_HLG,   pq_hal_get_vpss_sdr2hlg_cfg },

    { HI_PQ_HDR_MODE_HDR10PLUS_TO_HDR10PLUS, pq_hal_get_vpss_hdr10plus2hdr10plus_cfg },
    { HI_PQ_HDR_MODE_HDR10PLUS_TO_SDR,       pq_hal_get_vpss_hdr10plus2sdr_cfg },
    { HI_PQ_HDR_MODE_HDR10PLUS_TO_HDR10,     pq_hal_get_vpss_hdr10plus2hdr10_cfg },
    { HI_PQ_HDR_MODE_HDR10PLUS_TO_HLG,       pq_hal_get_vpss_hdr10plus2hlg_cfg },
};

hi_s32 pq_hal_get_vpss_sdr_trans_mode(hi_drv_hdr_type disp_hdr, hi_pq_hdr_mode *hdr_mode)
{
    if (disp_hdr == HI_DRV_HDR_TYPE_SDR) {
        *hdr_mode = HI_PQ_HDR_MODE_SDR_TO_SDR;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HDR10) {
        *hdr_mode = HI_PQ_HDR_MODE_SDR_TO_HDR10;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HLG) {
        *hdr_mode = HI_PQ_HDR_MODE_SDR_TO_HLG;
    } else {
        HI_WARN_PQ("Unkown support hdr type, src: %d, disp : %d\n", HI_DRV_HDR_TYPE_SDR, disp_hdr);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpss_hdr10_trans_mode(hi_drv_hdr_type disp_hdr, hi_pq_hdr_mode *hdr_mode)
{
    if (disp_hdr == HI_DRV_HDR_TYPE_SDR) {
        *hdr_mode = HI_PQ_HDR_MODE_HDR10_TO_SDR;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HDR10) {
        *hdr_mode = HI_PQ_HDR_MODE_HDR10_TO_HDR10;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HLG) {
        *hdr_mode = HI_PQ_HDR_MODE_HDR10_TO_HLG;
    } else {
        HI_WARN_PQ("Unkown support hdr type, src: %d, disp : %d\n", HI_DRV_HDR_TYPE_HDR10, disp_hdr);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpss_hlg_trans_mode(hi_drv_hdr_type disp_hdr, hi_pq_hdr_mode *hdr_mode)
{
    if (disp_hdr == HI_DRV_HDR_TYPE_SDR) {
        *hdr_mode = HI_PQ_HDR_MODE_HLG_TO_SDR;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HDR10) {
        *hdr_mode = HI_PQ_HDR_MODE_HLG_TO_HDR10;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HLG) {
        *hdr_mode = HI_PQ_HDR_MODE_HLG_TO_HLG;
    } else {
        HI_WARN_PQ("Unkown support hdr type, src: %d, disp : %d\n", HI_DRV_HDR_TYPE_HLG, disp_hdr);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#ifdef HI_HDR10PLUS_SUPPORT
hi_s32 pq_hal_get_vpss_hdr10plus_trans_mode(hi_drv_hdr_type disp_hdr, hi_pq_hdr_mode *hdr_mode)
{
    if (disp_hdr == HI_DRV_HDR_TYPE_SDR) {
        *hdr_mode = HI_PQ_HDR_MODE_HDR10PLUS_TO_SDR;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HDR10) {
        *hdr_mode = HI_PQ_HDR_MODE_HDR10PLUS_TO_HDR10;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HLG) {
        *hdr_mode = HI_PQ_HDR_MODE_HDR10PLUS_TO_HLG;
    } else if (disp_hdr == HI_DRV_HDR_TYPE_HDR10PLUS) {
        *hdr_mode = HI_PQ_HDR_MODE_HDR10PLUS_TO_HDR10PLUS;
    } else {
        HI_WARN_PQ("Unkown support hdr type, src: %d, disp : %d\n", HI_DRV_HDR_TYPE_HDR10PLUS, disp_hdr);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
#endif

hi_s32 pq_hal_get_vpsshdr_mode(hi_drv_hdr_type src_hdr, hi_drv_hdr_type disp_hdr, hi_pq_hdr_mode *hdr_mode)
{
    if (src_hdr == HI_DRV_HDR_TYPE_SDR) {
        pq_hal_get_vpss_sdr_trans_mode(disp_hdr, hdr_mode);
    } else if (src_hdr == HI_DRV_HDR_TYPE_HDR10) {
        pq_hal_get_vpss_hdr10_trans_mode(disp_hdr, hdr_mode);
    } else if (src_hdr == HI_DRV_HDR_TYPE_HLG) {
        pq_hal_get_vpss_hlg_trans_mode(disp_hdr, hdr_mode);
#ifdef HI_HDR10PLUS_SUPPORT
    } else if (src_hdr == HI_DRV_HDR_TYPE_HDR10PLUS) {
        pq_hal_get_vpss_hdr10plus_trans_mode(disp_hdr, hdr_mode);
#endif
    } else {
        HI_WARN_PQ("Unkown support hdr type, src: %d, disp : %d\n", src_hdr, disp_hdr);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vpsshdr_cfg(drv_pq_internal_xdr_frame_info *xdr_frame_info, pq_hal_vpsshdr_cfg *hdr_cfg)
{
    hi_s32 ret;
    hi_u32 index;
    hi_pq_hdr_mode hdr_mode = HI_PQ_HDR_MODE_SDR_TO_HDR10;

    ret = pq_hal_get_vpsshdr_mode(xdr_frame_info->src_hdr_type, xdr_frame_info->disp_hdr_type, &hdr_mode);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("gfx hdr cannot support !!!\n");
        return HI_FAILURE;
    }

    g_vpsshdr_mode[xdr_frame_info->handle] = hdr_mode;

    for (index = 0; index < HI_PQ_HDR_MODE_MAX; index++) {
        if (g_pf_pq_hal_get_vpss_hdr_path_cfg[index].hdr_mode == hdr_mode) {
            break;
        }
    }

    if (index < HI_PQ_HDR_MODE_MAX) {
        if (g_pf_pq_hal_get_vpss_hdr_path_cfg[index].hdr_func != HI_NULL) {
            ret = g_pf_pq_hal_get_vpss_hdr_path_cfg[index].hdr_func(xdr_frame_info->handle,
                xdr_frame_info->color_space_in, xdr_frame_info->color_space_out, hdr_cfg);
        } else {
            HI_WARN_PQ("hdr_func is NULL! \n");
            return HI_FAILURE;
        }
    } else {
        HI_WARN_PQ("no such hdr mode\n");
        return HI_FAILURE;
    }

    return ret;
}

hi_void pq_hal_set_im_degamma_coef_addr(vpss_reg_type *vdp_reg, hi_u32 para_addr_vhd)
{
    u_vpss_para_addr_vhd_chn05 vpss_para_addr_vhd_chn05;

    vpss_para_addr_vhd_chn05.bits.para_addr_vhd_chn05 = para_addr_vhd;
    pq_hal_vpss_reg_write(&(vdp_reg->vpss_para_addr_vhd_chn05.u32), vpss_para_addr_vhd_chn05.u32);

    return;
}

hi_void pq_hal_set_im_gamma_coef_addr(vpss_reg_type *vdp_reg, hi_u32 para_addr_vhd)
{
    u_vpss_para_addr_vhd_chn06 vpss_para_addr_vhd_chn06;

    vpss_para_addr_vhd_chn06.bits.para_addr_vhd_chn06 = para_addr_vhd;
    pq_hal_vpss_reg_write(&(vdp_reg->vpss_para_addr_vhd_chn06.u32), vpss_para_addr_vhd_chn06.u32);

    return;
}

hi_void pq_hal_set_im_cacm_coef_addr(vpss_reg_type *vdp_reg, hi_u32 para_addr_vhd)
{
    u_vpss_para_addr_vhd_chn08 vpss_para_addr_vhd_chn08;

    vpss_para_addr_vhd_chn08.bits.para_addr_vhd_chn08 = para_addr_vhd;
    pq_hal_vpss_reg_write(&(vdp_reg->vpss_para_addr_vhd_chn08.u32), vpss_para_addr_vhd_chn08.u32);

    return;
}

hi_void pq_hal_set_im_tmapv1_coef_addr(vpss_reg_type *vdp_reg, hi_u32 para_addr_vhd)
{
    u_vpss_para_addr_vhd_chn07 vpss_para_addr_vhd_chn07;

    vpss_para_addr_vhd_chn07.bits.para_addr_vhd_chn07 = para_addr_vhd;
    pq_hal_vpss_reg_write(&(vdp_reg->vpss_para_addr_vhd_chn07.u32), vpss_para_addr_vhd_chn07.u32);

    return;
}

hi_void pq_hal_set_tmapv2_coef_addr(vpss_reg_type *vdp_reg, hi_u32 para_addr_vhd)
{
    return;
}

hi_s32 pq_hal_set_vpsshdr_coef_addr(hi_drv_pq_vpss_layer layer_id, pq_hal_vpsshdr_cfg *hdr_cfg,
                                    vpss_reg_type *vpss_reg)
{
    if (hdr_cfg->imap_cfg.degamma_en == HI_TRUE) {
        pq_hal_set_im_degamma_coef_addr(vpss_reg, hdr_cfg->coef_addr.imap_degamma_addr);
    }

    if (hdr_cfg->imap_cfg.gamma_en == HI_TRUE) {
        pq_hal_set_im_gamma_coef_addr(vpss_reg, hdr_cfg->coef_addr.imap_gamma_addr);
    }

    if (hdr_cfg->imap_cfg.cacm.enable == HI_TRUE) {
        pq_hal_set_im_cacm_coef_addr(vpss_reg, hdr_cfg->coef_addr.imap_cacm_addr);
    }

    if (hdr_cfg->imap_cfg.tmap_v1.enable == HI_TRUE) {
        pq_hal_set_im_tmapv1_coef_addr(vpss_reg, hdr_cfg->coef_addr.imap_tmapv1_addr);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_set_tmapv2_coef_addr(vpss_reg, hdr_cfg->coef_addr.tmapv2_addr);
    }

    return HI_SUCCESS;
}

hi_void pq_hal_set_vpsshdr_cm_mode(hi_drv_pq_vpss_layer layer_id, hi_u32 addr_offset,
                                   pq_hal_vpsshdr_cfg *hdr_cfg, vpss_reg_type *vpss_reg)
{
    vpss_hihdr_v2_set_im_cm_pos(vpss_reg, addr_offset, hdr_cfg->imap_cfg.cacm.cm_pos);
    vpss_hihdr_v2_set_im_cm_demo_pos(vpss_reg, addr_offset, hdr_cfg->imap_cfg.cacm.demo_pos);
    vpss_hihdr_v2_set_im_cm_demo_mode(vpss_reg, addr_offset, hdr_cfg->imap_cfg.cacm.demo_mode);
    vpss_hihdr_v2_set_im_cm_demo_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.cacm.demo_en);
    vpss_hihdr_v2_set_im_cm_bitdepth_out_mode(vpss_reg, addr_offset, hdr_cfg->imap_cfg.cacm.bitdepth_out_mode);
    vpss_hihdr_v2_set_im_cm_bitdepth_in_mode(vpss_reg, addr_offset, hdr_cfg->imap_cfg.cacm.bitdepth_in_mode);
    vpss_hihdr_v2_set_im_cm_ck_gt_en(vpss_reg, addr_offset, HI_TRUE);
    vpss_hihdr_v2_set_im_cm_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.cacm.enable);
}

hi_void pq_hal_set_vpsshdr_imap_yuv2rgb_coef(hi_drv_pq_vpss_layer layer_id, hi_u32 addr_offset,
                                             pq_hal_vpsshdr_cfg *hdr_cfg, vpss_reg_type *vpss_reg)
{
    vpss_hihdr_v2_set_im_yuv2rgb_mode(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.mode);
    vpss_hihdr_v2_set_imap_m33yuv2rgb00(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef[INDEX_0][INDEX_0]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb01(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef[INDEX_0][INDEX_1]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb02(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef[INDEX_0][INDEX_2]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb10(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef[INDEX_1][INDEX_0]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb11(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef[INDEX_1][INDEX_1]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb12(vpss_reg, addr_offset,
                                        hdr_cfg->imap_cfg.yuv2rgb.coef[INDEX_1][INDEX_2]); /* 2 is index */
    vpss_hihdr_v2_set_imap_m33yuv2rgb20(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef[INDEX_2][INDEX_0]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb21(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef[INDEX_2][INDEX_1]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb22(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef[INDEX_2][INDEX_2]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb200(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef2[INDEX_0][INDEX_0]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb201(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef2[INDEX_0][INDEX_1]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb202(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef2[INDEX_0][INDEX_2]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb210(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef2[INDEX_1][INDEX_0]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb211(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef2[INDEX_1][INDEX_1]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb212(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef2[INDEX_1][INDEX_2]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb220(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef2[INDEX_2][INDEX_0]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb221(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef2[INDEX_2][INDEX_1]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb222(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.coef2[INDEX_2][INDEX_2]);
    vpss_hihdr_v2_set_imap_m33yuv2rgb_scale2p(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.scale2p);

    vpss_hihdr_v2_set_im_v3_yuv2rgb_dc_in0(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.dc_in[INDEX_0]);
    vpss_hihdr_v2_set_im_v3_yuv2rgb_dc_in1(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.dc_in[INDEX_1]);
    vpss_hihdr_v2_set_im_v3_yuv2rgb_dc_in2(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.dc_in[INDEX_2]);
    vpss_hihdr_v2_set_im_v3_yuv2rgb2_dc_in0(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.dc_in2[INDEX_0]);
    vpss_hihdr_v2_set_im_v3_yuv2rgb2_dc_in1(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.dc_in2[INDEX_1]);
    vpss_hihdr_v2_set_im_v3_yuv2rgb2_dc_in2(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.dc_in2[INDEX_2]);

    vpss_hihdr_v2_set_imap_v3yuv2rgb_offinrgb0(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.offinrgb[INDEX_0]);
    vpss_hihdr_v2_set_imap_v3yuv2rgb_offinrgb1(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.offinrgb[INDEX_1]);
    vpss_hihdr_v2_set_imap_v3yuv2rgb_offinrgb2(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.offinrgb[INDEX_2]);
    vpss_hihdr_v2_set_imap_v3yuv2rgb2_offinrgb0(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.offinrgb2[INDEX_0]);
    vpss_hihdr_v2_set_imap_v3yuv2rgb2_offinrgb1(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.offinrgb2[INDEX_1]);
    vpss_hihdr_v2_set_imap_v3yuv2rgb2_offinrgb2(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.offinrgb2[INDEX_2]);

    vpss_hihdr_v2_set_im_yuv2rgb_clip_min(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.clip_min);
    vpss_hihdr_v2_set_im_yuv2rgb_clip_max(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.clip_max);
    vpss_hihdr_v2_set_im_yuv2rgb_thr_r(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.thr_r);
    vpss_hihdr_v2_set_im_yuv2rgb_thr_b(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.thr_b);

    vpss_hihdr_v2_set_im_yuv2rgb_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.enable);
    vpss_hihdr_v2_set_im_v1_y2r_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.v1_enable);
    vpss_hihdr_v2_set_im_v0_y2r_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.yuv2rgb.v0_enable);
}

hi_void pq_hal_set_vpsshdr_imap_rgb2lms_coef(hi_drv_pq_vpss_layer layer_id, hi_u32 addr_offset,
                                             pq_hal_vpsshdr_cfg *hdr_cfg, vpss_reg_type *vpss_reg)
{
    vpss_hihdr_v2_set_im_rgb2lms_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.enable);
    vpss_hihdr_v2_set_imap_m33rgb2lms00(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.coef[INDEX_0][INDEX_0]);
    vpss_hihdr_v2_set_imap_m33rgb2lms01(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.coef[INDEX_0][INDEX_1]);
    vpss_hihdr_v2_set_imap_m33rgb2lms02(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.coef[INDEX_0][INDEX_2]);
    vpss_hihdr_v2_set_imap_m33rgb2lms10(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.coef[INDEX_1][INDEX_0]);
    vpss_hihdr_v2_set_imap_m33rgb2lms11(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.coef[INDEX_1][INDEX_1]);
    vpss_hihdr_v2_set_imap_m33rgb2lms12(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.coef[INDEX_1][INDEX_2]);
    vpss_hihdr_v2_set_imap_m33rgb2lms20(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.coef[INDEX_2][INDEX_0]);
    vpss_hihdr_v2_set_imap_m33rgb2lms21(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.coef[INDEX_2][INDEX_1]);
    vpss_hihdr_v2_set_imap_m33rgb2lms22(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.coef[INDEX_2][INDEX_2]);
    vpss_hihdr_v2_set_imap_m33rgb2lms_scale2p(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.scale2p);
    vpss_hihdr_v2_set_im_rgb2lms_clip_min(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.clip_min);
    vpss_hihdr_v2_set_im_rgb2lms_clip_max(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.clip_max);
    vpss_hihdr_v2_set_im_v3_rgb2lms_dc_in0(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.dc_in[INDEX_0]);
    vpss_hihdr_v2_set_im_v3_rgb2lms_dc_in1(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.dc_in[INDEX_1]);
    vpss_hihdr_v2_set_im_v3_rgb2lms_dc_in2(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rgb2lms.dc_in[INDEX_2]);
}

hi_void pq_hal_set_vpsshdr_imap_lms2ipt_coef(hi_drv_pq_vpss_layer layer_id, hi_u32 addr_offset,
                                             pq_hal_vpsshdr_cfg *hdr_cfg, vpss_reg_type *vpss_reg)
{
    vpss_hihdr_v2_set_im_lms2ipt_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.enable);
    vpss_hihdr_v2_set_imap_m33lms2ipt00(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.coef[INDEX_0][INDEX_0]);
    vpss_hihdr_v2_set_imap_m33lms2ipt01(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.coef[INDEX_0][INDEX_1]);
    vpss_hihdr_v2_set_imap_m33lms2ipt02(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.coef[INDEX_0][INDEX_2]);
    vpss_hihdr_v2_set_imap_m33lms2ipt10(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.coef[INDEX_1][INDEX_0]);
    vpss_hihdr_v2_set_imap_m33lms2ipt11(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.coef[INDEX_1][INDEX_1]);
    vpss_hihdr_v2_set_imap_m33lms2ipt12(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.coef[INDEX_1][INDEX_2]);
    vpss_hihdr_v2_set_imap_m33lms2ipt20(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.coef[INDEX_2][INDEX_0]);
    vpss_hihdr_v2_set_imap_m33lms2ipt21(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.coef[INDEX_2][INDEX_1]);
    vpss_hihdr_v2_set_imap_m33lms2ipt22(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.coef[INDEX_2][INDEX_2]);
    vpss_hihdr_v2_set_imap_m33lms2ipt_scale2p(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.scale2p);

    vpss_hihdr_v2_set_im_lms2ipt_clip_min_y(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.clip_min_y);
    vpss_hihdr_v2_set_im_lms2ipt_clip_max_y(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.clip_max_y);
    vpss_hihdr_v2_set_im_lms2ipt_clip_min_c(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.clip_min_c);
    vpss_hihdr_v2_set_im_lms2ipt_clip_max_c(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.clip_max_c);

    vpss_hihdr_v2_set_im_v3_lms2ipt_dc_out0(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.dc_out[INDEX_0]);
    vpss_hihdr_v2_set_im_v3_lms2ipt_dc_out1(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.dc_out[INDEX_1]);
    vpss_hihdr_v2_set_im_v3_lms2ipt_dc_out2(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.dc_out[INDEX_2]);
    vpss_hihdr_v2_set_im_v3_lms2ipt_dc_in0(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.dc_in[INDEX_0]);
    vpss_hihdr_v2_set_im_v3_lms2ipt_dc_in1(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.dc_in[INDEX_1]);
    vpss_hihdr_v2_set_im_v3_lms2ipt_dc_in2(vpss_reg, addr_offset, hdr_cfg->imap_cfg.lms2ipt.dc_in[INDEX_2]);
}

hi_void pq_hal_set_vpsshdr_imap_tm_coef(hi_drv_pq_vpss_layer layer_id, hi_u32 addr_offset,
                                        pq_hal_vpsshdr_cfg *hdr_cfg, vpss_reg_type *vpss_reg)
{
    vpss_hihdr_v2_set_im_tm_v1_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.enable);
    vpss_hihdr_v2_set_im_tm_pos(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.tm_pos);
    vpss_hihdr_v2_set_im_tm_scale_mix_alpha(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.scale_mix_alpha);
    vpss_hihdr_v2_set_im_tm_mix_alpha(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.mix_alpha);
    vpss_hihdr_v2_set_im_tm_xstep3(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.step[INDEX_3]);
    vpss_hihdr_v2_set_im_tm_xstep2(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.step[INDEX_2]);
    vpss_hihdr_v2_set_im_tm_xstep1(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.step[INDEX_1]);
    vpss_hihdr_v2_set_im_tm_xstep0(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.step[INDEX_0]);
    vpss_hihdr_v2_set_im_tm_xstep7(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.step[INDEX_7]);
    vpss_hihdr_v2_set_im_tm_xstep6(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.step[INDEX_6]);
    vpss_hihdr_v2_set_im_tm_xstep5(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.step[INDEX_5]);
    vpss_hihdr_v2_set_im_tm_xstep4(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.step[INDEX_4]);
    vpss_hihdr_v2_set_im_tm_xpos0(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.pos[INDEX_0]);
    vpss_hihdr_v2_set_im_tm_xpos1(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.pos[INDEX_1]);
    vpss_hihdr_v2_set_im_tm_xpos2(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.pos[INDEX_2]);
    vpss_hihdr_v2_set_im_tm_xpos3(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.pos[INDEX_3]);
    vpss_hihdr_v2_set_im_tm_xpos4(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.pos[INDEX_4]);
    vpss_hihdr_v2_set_im_tm_xpos5(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.pos[INDEX_5]);
    vpss_hihdr_v2_set_im_tm_xpos6(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.pos[INDEX_6]);
    vpss_hihdr_v2_set_im_tm_xpos7(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.pos[INDEX_7]);
    vpss_hihdr_v2_set_im_tm_xnum3(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.num[INDEX_3]);
    vpss_hihdr_v2_set_im_tm_xnum2(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.num[INDEX_2]);
    vpss_hihdr_v2_set_im_tm_xnum1(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.num[INDEX_1]);
    vpss_hihdr_v2_set_im_tm_xnum0(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.num[INDEX_0]);
    vpss_hihdr_v2_set_im_tm_xnum7(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.num[INDEX_7]);
    vpss_hihdr_v2_set_im_tm_xnum6(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.num[INDEX_6]);
    vpss_hihdr_v2_set_im_tm_xnum5(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.num[INDEX_5]);
    vpss_hihdr_v2_set_im_tm_xnum4(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.num[INDEX_4]);

    vpss_hihdr_v2_set_im_tm_m3_lum_cal0(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.lum_cal[INDEX_0]);
    vpss_hihdr_v2_set_im_tm_m3_lum_cal1(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.lum_cal[INDEX_1]);
    vpss_hihdr_v2_set_im_tm_m3_lum_cal2(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.lum_cal[INDEX_2]);
    vpss_hihdr_v2_set_im_tm_scale_lum_cal(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.scale_lum_cal);
    vpss_hihdr_v2_set_im_tm_scale_coef(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.scale_coef);
    vpss_hihdr_v2_set_im_tm_clip_min(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.clip_min);
    vpss_hihdr_v2_set_im_tm_clip_max(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.clip_max);

    vpss_hihdr_v2_set_im_tm_dc_out0(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.dc_out[INDEX_0]);
    vpss_hihdr_v2_set_im_tm_dc_out1(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.dc_out[INDEX_1]);
    vpss_hihdr_v2_set_im_tm_dc_out2(vpss_reg, addr_offset, hdr_cfg->imap_cfg.tmap_v1.dc_out[INDEX_2]);
}

hi_void pq_hal_set_vpsshdr_imap_coef(hi_drv_pq_vpss_layer layer_id, hi_u32 addr_offset,
                                     pq_hal_vpsshdr_cfg *hdr_cfg, vpss_reg_type *vpss_reg)
{
    vpss_hihdr_v2_set_im_gamma_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.gamma_en);
    vpss_hihdr_v2_set_im_degamma_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.degamma_en);
    vpss_hihdr_v2_set_im_norm_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.norm_en);
    vpss_hihdr_v2_set_im_ladj_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.ladj_en);
    vpss_hihdr_v2_set_im_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.enable);

    vpss_hihdr_v2_set_im_rshift_round_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rshift_round_en);
    vpss_hihdr_v2_set_im_rshift_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.rshift_en);
    vpss_hihdr_v2_set_im_ck_gt_en(vpss_reg, addr_offset, HI_TRUE);
    vpss_hihdr_v2_set_im_ipt_in_sel(vpss_reg, addr_offset, hdr_cfg->imap_cfg.ipt_in_sel);

    vpss_hihdr_v2_set_imap_eotfparam_eotf(vpss_reg, addr_offset, hdr_cfg->imap_cfg.eotfparam_eotf);
    vpss_hihdr_v2_set_imap_in_color(vpss_reg, addr_offset, hdr_cfg->imap_cfg.in_color);
    vpss_hihdr_v2_set_im_in_bits(vpss_reg, addr_offset, hdr_cfg->imap_cfg.in_bits);

    vpss_hihdr_v2_set_imap_eotfparam_range_min(vpss_reg, addr_offset, hdr_cfg->imap_cfg.eotfparam_range_min);
    vpss_hihdr_v2_set_imap_eotfparam_range(vpss_reg, addr_offset, hdr_cfg->imap_cfg.eotfparam_range);
    vpss_hihdr_v2_set_imap_eotfparam_range_inv(vpss_reg, addr_offset, hdr_cfg->imap_cfg.eotfparam_range_inv);
    vpss_hihdr_v2_set_imap_v3ipt_off0(vpss_reg, addr_offset, hdr_cfg->imap_cfg.v3ipt_off[INDEX_0]);
    vpss_hihdr_v2_set_imap_v3ipt_off1(vpss_reg, addr_offset, hdr_cfg->imap_cfg.v3ipt_off[INDEX_0]);
    vpss_hihdr_v2_set_imap_v3ipt_off2(vpss_reg, addr_offset, hdr_cfg->imap_cfg.v3ipt_off[INDEX_0]);
    vpss_hihdr_v2_set_imap_ipt_scale(vpss_reg, addr_offset, hdr_cfg->imap_cfg.ipt_scale);

    vpss_hihdr_v2_set_im_degamma_clip_max(vpss_reg, addr_offset, hdr_cfg->imap_cfg.degamma_clip_max);
    vpss_hihdr_v2_set_im_degamma_clip_min(vpss_reg, addr_offset, hdr_cfg->imap_cfg.degamma_clip_min);
    vpss_hihdr_v2_set_imap_ladj_chroma_weight(vpss_reg, addr_offset, hdr_cfg->imap_cfg.ladj_chroma_weight);
    vpss_hihdr_v2_set_im_demo_luma_mode(vpss_reg, addr_offset, hdr_cfg->imap_cfg.demo_luma_mode);
    vpss_hihdr_v2_set_im_demo_luma_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.demo_luma_en);
    vpss_hihdr_v2_set_im_v1_hdr_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.v1_hdr_enable);
    vpss_hihdr_v2_set_im_v0_hdr_en(vpss_reg, addr_offset, hdr_cfg->imap_cfg.v0_hdr_enable);
}

hi_void pq_hal_set_vpsshdr_imap_mode(hi_drv_pq_vpss_layer layer_id, hi_u32 addr_offset,
                                     pq_hal_vpsshdr_cfg *hdr_cfg, vpss_reg_type *vpss_reg)
{
    pq_hal_set_vpsshdr_imap_coef(layer_id, addr_offset, hdr_cfg, vpss_reg);
    pq_hal_set_vpsshdr_imap_yuv2rgb_coef(layer_id, addr_offset, hdr_cfg, vpss_reg);
    pq_hal_set_vpsshdr_imap_rgb2lms_coef(layer_id, addr_offset, hdr_cfg, vpss_reg);
    pq_hal_set_vpsshdr_imap_lms2ipt_coef(layer_id, addr_offset, hdr_cfg, vpss_reg);
    pq_hal_set_vpsshdr_imap_tm_coef(layer_id, addr_offset, hdr_cfg, vpss_reg);
}

hi_s32 pq_hal_set_vpsshdr_cfg(hi_drv_pq_vpss_layer layer_id, pq_hal_vpsshdr_cfg *hdr_cfg,
                              vpss_reg_type *vpss_reg)
{
    hi_u32 addr_offset = 0;
    pq_hal_set_vpsshdr_imap_mode(layer_id, addr_offset, hdr_cfg, vpss_reg);
    pq_hal_set_vpsshdr_cm_mode(layer_id, addr_offset, hdr_cfg, vpss_reg);

    return HI_SUCCESS;
}

/* b_default : 0 / bin success; 1 / bin fail */
hi_s32 pq_hal_init_vpsshdr(pq_bin_param *pq_param, hi_bool default_code)
{
    return HI_SUCCESS;
}

hi_s32 pq_hal_deinit_vpsshdr(hi_void)
{
    return HI_SUCCESS;
}


