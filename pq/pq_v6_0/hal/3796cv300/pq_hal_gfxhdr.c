/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include <linux/string.h>

#include "pq_hal_comm.h"
#include "drv_pq_ext.h"
#include "pq_hal_gfxhdr.h"
#include "pq_hal_gfxhdr_coef.h"
#include "pq_hal_gfxhdr_regset.h"
#include "pq_hal_hdrv2_regset_comm.h"

#define PQ_GP0_HDR_OFFSET   (0x87000 / 4)

static hi_pq_hdr_mode g_gfxhdr_mode = HI_PQ_HDR_MODE_SDR_TO_HDR10;

typedef struct {
    hi_u32 clut[PQ_GFXHDR_DEGAMMA];
} pq_hal_gfxhdr_degamma;

static hi_void *g_gfxhdr_imap_degamma = HI_NULL;
static hi_void *g_gfxhdr_tmapv1_lut = HI_NULL;

static hi_void *g_gfxhdr_imap_gamma_lut_x = HI_NULL;
static hi_void *g_gfxhdr_imap_gamma_lut_a = HI_NULL;
static hi_void *g_gfxhdr_imap_gamma_lut_b = HI_NULL;

static hi_void *g_gfxhdr_tmap_tm_lut_i = HI_NULL;
static hi_void *g_gfxhdr_tmap_tm_lut_s = HI_NULL;
static hi_void *g_gfxhdr_tmap_sm_lut_i = HI_NULL;
static hi_void *g_gfxhdr_tmap_sm_lut_s = HI_NULL;
static hi_void *g_gfxhdr_tmap_smc_lut = HI_NULL;

typedef struct {
    hi_u32 gammax[PQ_GFXHDR_GAMMA];
    hi_u32 gammaa[PQ_GFXHDR_GAMMA];
    hi_u32 gammab[PQ_GFXHDR_GAMMA];
} pq_hal_gfxhdr_gamma;

typedef struct {
    hi_u32 clut[PQ_GFXHDR_TM_CLUT];
} pq_hal_gfxhdr_tm;

hi_u16 g_gfxhdr_imap_cacm_lut_r[PQ_GFXHDR_CM_3DCLUT][PQ_GFXHDR_CM_3DCLUT][PQ_GFXHDR_CM_3DCLUT];
hi_u16 g_gfxhdr_imap_cacm_lut_g[PQ_GFXHDR_CM_3DCLUT][PQ_GFXHDR_CM_3DCLUT][PQ_GFXHDR_CM_3DCLUT];
hi_u16 g_gfxhdr_imap_cacm_lut_b[PQ_GFXHDR_CM_3DCLUT][PQ_GFXHDR_CM_3DCLUT][PQ_GFXHDR_CM_3DCLUT];

typedef struct {
    hi_drv_pq_imap_cfg imap_cfg;
    pq_gfxhdr_tmap_cfg tmap_cfg;
    hi_drv_pq_hdr_coef_addr coef_addr;
} pq_hal_gfxhdr_cfg;

pq_hal_gfxhdr_cfg g_gfxhdr_imap_cfg;

typedef hi_s32 (*pf_pq_hal_gfx_gethdrpathcfg)(drv_pq_internal_color_space en_in_cs,
                                              drv_pq_internal_color_space en_out_cs,
                                              pq_hal_gfxhdr_cfg *pst_pq_hdr_cfg);

typedef struct {
    hi_pq_hdr_mode hdr_mode;
    pf_pq_hal_gfx_gethdrpathcfg hdr_func;
} pq_gfx_hdr_fun;

#if 1
#define PQ_GFXHDR_COEF_SIZE  24576 /* 8000000 */
#define PQ_COEF_OVER_SIZE 128

#define PQ_COEF_SIZE_GP0_HDR0   12288 /* 10000 */
#define PQ_COEF_SIZE_GP0_HDR1   4096
#define PQ_COEF_SIZE_GP0_HDR2   4096
#define PQ_COEF_SIZE_GP0_HDR3   4096

typedef struct {
    hi_u8 *hdr0_addr; /* cbb is hdr1_addr */
    hi_u8 *hdr1_addr; /* cbb is hdr2_addr */
    hi_u8 *hdr2_addr; /* cbb is hdr3_addr */
    hi_u8 *hdr3_addr; /* cbb is hdr6_addr */
} pq_hdr_coef_addr;


typedef enum {
    PQ_COEF_DATA_TYPE_U8 = 0,
    PQ_COEF_DATA_TYPE_S8,
    PQ_COEF_DATA_TYPE_U16,
    PQ_COEF_DATA_TYPE_S16,
    PQ_COEF_DATA_TYPE_U32,
    PQ_COEF_DATA_TYPE_S32,

    PQ_COEF_DATA_TYPE_MAX
} pq_coef_data_type;

typedef struct {
    void *coef;
    void *coef_new;
    pq_coef_data_type coef_data_type;
    hi_u32 length;
    hi_s32 coef_max;
    hi_s32 coef_min;
} pq_coef_gen_cfg;

typedef struct {
    hi_u8 *coef_addr;
    hi_u32 lut_num;
    hi_u32 burst_num;
    hi_u32 cycle_num;
    void **coef_array;
    hi_u32 *lut_length;
    hi_u32 *coef_bit_length;
    pq_coef_data_type data_type;
} pq_coef_send_cfg;

typedef struct {
    hi_u32 data3;
    hi_u32 data2;
    hi_u32 data1;
    hi_u32 data0;
    hi_u32 depth;
} pq_coef_data;

#define COEF_BIT_32  32
#define COEF_BIT_64  64
#define COEF_BIT_96  96
#define COEF_BIT_128 128

#define LUT_SIZE 9
hi_u16 g_gfxhdr_coef_cm_lutr[LUT_SIZE][LUT_SIZE][LUT_SIZE];
hi_u16 g_gfxhdr_coef_cm_lutg[LUT_SIZE][LUT_SIZE][LUT_SIZE];
hi_u16 g_gfxhdr_coef_cm_lutb[LUT_SIZE][LUT_SIZE][LUT_SIZE];

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
} cm_coef;

#define DEF_L2PQ_LUT_NODES 128

hi_u16 g_gfxhdr_coef_cm_lutr3d0[125]; /* 125 is length */
hi_u16 g_gfxhdr_coef_cm_lutr3d1[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lutr3d2[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lutr3d3[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lutr3d4[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lutr3d5[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lutr3d6[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lutr3d7[64];  /* 64 is length */

hi_u16 g_gfxhdr_coef_cm_lutg3d0[125]; /* 125 is length */
hi_u16 g_gfxhdr_coef_cm_lutg3d1[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lutg3d2[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lutg3d3[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lutg3d4[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lutg3d5[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lutg3d6[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lutg3d7[64];  /* 64 is length */

hi_u16 g_gfxhdr_coef_cm_lutb3d0[125]; /* 125 is length */
hi_u16 g_gfxhdr_coef_cm_lutb3d1[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lutb3d2[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lutb3d3[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lutb3d4[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lutb3d5[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lutb3d6[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lutb3d7[64];  /* 64 is length */

#define EVE_SIZE 5
#define ODD_SIZE 4
#define DIV_2 2

hi_u16 g_gfxhdr_coef_cm_lut00_r[64]; /* 64 is length */
hi_u16 g_gfxhdr_coef_cm_lut01_r[64]; /* 64 is length */
hi_u16 g_gfxhdr_coef_cm_lut1_r[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lut2_r[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lut3_r[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lut4_r[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lut5_r[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lut6_r[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lut7_r[64];  /* 64 is length */

hi_u16 g_gfxhdr_coef_cm_lut00_g[64]; /* 64 is length */
hi_u16 g_gfxhdr_coef_cm_lut01_g[64]; /* 64 is length */
hi_u16 g_gfxhdr_coef_cm_lut1_g[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lut2_g[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lut3_g[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lut4_g[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lut5_g[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lut6_g[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lut7_g[64];  /* 64 is length */

hi_u16 g_gfxhdr_coef_cm_lut00_b[64]; /* 64 is length */
hi_u16 g_gfxhdr_coef_cm_lut01_b[64]; /* 64 is length */
hi_u16 g_gfxhdr_coef_cm_lut1_b[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lut2_b[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lut3_b[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lut4_b[100]; /* 100 is length */
hi_u16 g_gfxhdr_coef_cm_lut5_b[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lut6_b[80];  /* 80 is length */
hi_u16 g_gfxhdr_coef_cm_lut7_b[64];  /* 64 is length */

hi_s32 g_gfxhdr_coef_tm_new[256]; /* 256 is length */

static hi_u32 g_db_hdr_degamm_pq_new[1024]; /* 1024 is length */
static hi_u32 g_db_hdr_degamm_pq_new[1024]; /* 1024 is length */
static hi_u32 g_dbhdr_gamm_l2pq_lut_x[DEF_L2PQ_LUT_NODES];
static hi_u32 g_dbhdr_gamm_l2pq_lut_a[DEF_L2PQ_LUT_NODES];
static hi_u32 g_dbhdr_gamm_l2pq_lut_b[DEF_L2PQ_LUT_NODES];

pq_gfx_coef_addr g_pq_coef_buf_addr = { 0 };

drv_pq_mem_info g_gfxhdr_buf_addr = { 0 };

static hi_void vir_addr_distribute(pq_gfx_coef_addr *addr)
{
    addr->vir_addr[PQ_COEF_BUF_GP0_HDR0] = addr->vir_addr[PQ_COEF_BUF_GP0_HDR0];
    addr->vir_addr[PQ_COEF_BUF_GP0_HDR1] = addr->vir_addr[PQ_COEF_BUF_GP0_HDR0] + PQ_COEF_SIZE_GP0_HDR0;
    addr->vir_addr[PQ_COEF_BUF_GP0_HDR2] = addr->vir_addr[PQ_COEF_BUF_GP0_HDR1] + PQ_COEF_SIZE_GP0_HDR1;
    addr->vir_addr[PQ_COEF_BUF_GP0_HDR3] = addr->vir_addr[PQ_COEF_BUF_GP0_HDR2] + PQ_COEF_SIZE_GP0_HDR2;
}

static hi_s32 phy_addr_distribute(pq_gfx_coef_addr *addr)
{
    addr->phy_addr[PQ_COEF_BUF_GP0_HDR0] = addr->phy_addr[PQ_COEF_BUF_GP0_HDR0];
    addr->phy_addr[PQ_COEF_BUF_GP0_HDR1] = addr->phy_addr[PQ_COEF_BUF_GP0_HDR0] + PQ_COEF_SIZE_GP0_HDR0;
    addr->phy_addr[PQ_COEF_BUF_GP0_HDR2] = addr->phy_addr[PQ_COEF_BUF_GP0_HDR1] + PQ_COEF_SIZE_GP0_HDR1;
    addr->phy_addr[PQ_COEF_BUF_GP0_HDR3] = addr->phy_addr[PQ_COEF_BUF_GP0_HDR2] + PQ_COEF_SIZE_GP0_HDR2;

    return HI_SUCCESS;
}

hi_void pq_ip_para_init(hi_void)
{
    if (g_gfxhdr_buf_addr.vir_addr == HI_NULL) {
        drv_pq_mem_attr memattr = {
            .mode = OSAL_MMZ_TYPE,
            .is_map_viraddr = HI_TRUE,
            .is_cache = HI_FALSE,
            .size = PQ_GFXHDR_COEF_SIZE,
            .name = "pq_gfxhdr_coef",
        };

        if (drv_pq_mem_alloc(&memattr, &g_gfxhdr_buf_addr) != HI_SUCCESS) {
            return;
        }

        pq_comm_mem_flush(&(g_gfxhdr_buf_addr));

        g_pq_coef_buf_addr.vir_addr[PQ_COEF_BUF_GP0_HDR0] = g_gfxhdr_buf_addr.vir_addr;
        g_pq_coef_buf_addr.phy_addr[PQ_COEF_BUF_GP0_HDR0] = g_gfxhdr_buf_addr.phy_addr;

        vir_addr_distribute(&g_pq_coef_buf_addr);
        phy_addr_distribute(&g_pq_coef_buf_addr);
    }
}

hi_void pq_ip_para_deinit(hi_void)
{
    if (g_gfxhdr_buf_addr.vir_addr != HI_NULL) {
        drv_pq_mem_free(&g_gfxhdr_buf_addr);
        g_gfxhdr_buf_addr.vir_addr = HI_NULL;
    }
}

hi_u32 pq_drv_gencoef(pq_coef_gen_cfg *cfg)
{
    hi_u32 ii = 0;

    if (cfg->coef_data_type == PQ_COEF_DATA_TYPE_U8) {
        for (ii = 0; ii < cfg->length; ii++) {
            ((hi_u8 *)cfg->coef_new)[ii] = ((hi_u8 *)cfg->coef)[ii];
        }
    } else if (cfg->coef_data_type == PQ_COEF_DATA_TYPE_S8) {
        for (ii = 0; ii < cfg->length; ii++) {
            ((hi_s8 *)cfg->coef_new)[ii] = ((hi_s8 *)cfg->coef)[ii];
        }
    } else if (cfg->coef_data_type == PQ_COEF_DATA_TYPE_U16) {
        for (ii = 0; ii < cfg->length; ii++) {
            ((hi_u16 *)cfg->coef_new)[ii] = ((hi_u16 *)cfg->coef)[ii];
        }
    } else if (cfg->coef_data_type == PQ_COEF_DATA_TYPE_S16) {
        for (ii = 0; ii < cfg->length; ii++) {
            ((hi_s16 *)cfg->coef_new)[ii] = ((hi_s16 *)cfg->coef)[ii];
        }
    } else if (cfg->coef_data_type == PQ_COEF_DATA_TYPE_U32) {
        for (ii = 0; ii < cfg->length; ii++) {
            ((hi_u32 *)cfg->coef_new)[ii] = ((hi_u32 *)cfg->coef)[ii];
        }
    } else if (cfg->coef_data_type == PQ_COEF_DATA_TYPE_S32) {
        for (ii = 0; ii < cfg->length; ii++) {
            ((hi_s32 *)cfg->coef_new)[ii] = ((hi_s32 *)cfg->coef)[ii];
        }
    }

    return 0;
}

static hi_void pq_coef_push128(pq_coef_data *data, hi_u32 coef_data, hi_u32 bit_len)
{
    coef_data = coef_data & (0xFFFFFFFF >> (COEF_BIT_32 - bit_len));

    if (data->depth < COEF_BIT_32) {
        if ((data->depth + bit_len) <= COEF_BIT_32) {
            data->data0 = (coef_data << data->depth) | data->data0;
        } else {
            data->data0 = (coef_data << data->depth) | data->data0;
            data->data1 = coef_data >> (COEF_BIT_32 - data->depth % COEF_BIT_32);
        }
    } else if (data->depth >= COEF_BIT_32 && data->depth < COEF_BIT_64) {
        if ((data->depth + bit_len) <= COEF_BIT_64) {
            data->data1 = (coef_data << (data->depth % COEF_BIT_32)) | data->data1;
        } else {
            data->data1 = (coef_data << (data->depth % COEF_BIT_32)) | data->data1;
            data->data2 = coef_data >> (COEF_BIT_32 - data->depth % COEF_BIT_32);
        }
    } else if (data->depth >= COEF_BIT_64 && data->depth < COEF_BIT_96) {
        if ((data->depth + bit_len) <= COEF_BIT_96) {
            data->data2 = (coef_data << (data->depth % COEF_BIT_32)) | data->data2;
        } else {
            data->data2 = (coef_data << (data->depth % COEF_BIT_32)) | data->data2;
            data->data3 = coef_data >> (COEF_BIT_32 - data->depth % COEF_BIT_32);
        }
    } else if (data->depth >= COEF_BIT_96) {
        if ((data->depth + bit_len) <= COEF_BIT_128) {
            data->data3 = (coef_data << (data->depth % COEF_BIT_32)) | data->data3;
        }
    }

    data->depth = data->depth + bit_len;

    if (data->depth > COEF_BIT_128) {
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

static hi_u32 pq_ip_coef_sendcoef_get_data(pq_coef_data_type type, hi_u32 n,
    hi_u32 coef_cnt, hi_u8 **coef_array)
{
    hi_u32 tmp_data = 0;

    if (type == PQ_COEF_DATA_TYPE_S16) {
        tmp_data = ((hi_s16 **)coef_array)[n][coef_cnt];
    } else if (type == PQ_COEF_DATA_TYPE_U16) {
        tmp_data = ((hi_u16 **)coef_array)[n][coef_cnt];
    } else if (type == PQ_COEF_DATA_TYPE_U32) {
        tmp_data = ((hi_u32 **)coef_array)[n][coef_cnt];
    } else if (type == PQ_COEF_DATA_TYPE_S32) {
        tmp_data = ((hi_s32 **)coef_array)[n][coef_cnt];
    } else if (type == PQ_COEF_DATA_TYPE_S8) {
        tmp_data = ((hi_s8 **)coef_array)[n][coef_cnt];
    } else if (type == PQ_COEF_DATA_TYPE_U8) {
        tmp_data = ((hi_u8 **)coef_array)[n][coef_cnt];
    }

    return tmp_data;
}

hi_void pq_ip_coef_sendcoef_cnt(hi_u32 coef_cnt, pq_coef_send_cfg *cfg, pq_coef_data *data)
{
    hi_u32 tmp_data;
    hi_u32 n;

    for (n = 0; n < cfg->lut_num; n++) {
        if (coef_cnt < cfg->lut_length[n]) {
            tmp_data = pq_ip_coef_sendcoef_get_data(cfg->data_type, n, coef_cnt, (hi_u8 **)(cfg->coef_array));
        } else {
            tmp_data = 0;
        }
        pq_coef_push128(data, tmp_data, cfg->coef_bit_length[n]);
    }
}

static hi_u32 pq_coef_writeddr(hi_u8 *addr, pq_coef_data *data)
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

hi_u8 *pq_ip_coef_sendcoef(pq_coef_send_cfg *cfg)
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

    pq_coef_data data;

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
            pq_ip_coef_sendcoef_cnt(coef_cnt, cfg, &data);
        }
        addr = addr_base + addr_offset;
        addr_offset = addr_offset + 16; /* 16 is number */
        pq_coef_writeddr(addr, &data);
    }

    return (addr_base + addr_offset);
}

static hi_void pq_gfxhdr_set_imap_tmpa_v1_coef(hi_u8 *addr)
{
    pq_coef_gen_cfg coef_gen;
    pq_coef_send_cfg coef_send;

    hi_void *tone_map_clut;

    void *p_coef_array[1] = {g_gfxhdr_coef_tm_new};
    hi_u32 lut_length[1] = {256};
    hi_u32 coef_bit_length[1] = {16};

    tone_map_clut = g_gfxhdr_tmapv1_lut;
    coef_gen.coef = tone_map_clut;
    coef_gen.coef_new = g_gfxhdr_coef_tm_new;
    coef_gen.length = 256; /* 256 :a number */
    coef_gen.coef_data_type = PQ_COEF_DATA_TYPE_U32;
    coef_gen.coef_max = ((1 << 14) - 1); /* 14 :a number */
    coef_gen.coef_min = 0;
    pq_drv_gencoef(&coef_gen);

    coef_send.coef_addr = addr;
    coef_send.lut_num = 1;
    coef_send.burst_num = 1;
    coef_send.cycle_num = 8; /* 8 :a number */
    coef_send.coef_array = p_coef_array;
    coef_send.lut_length = lut_length;
    coef_send.coef_bit_length = coef_bit_length;
    coef_send.data_type = PQ_COEF_DATA_TYPE_U32;
    pq_ip_coef_sendcoef(&coef_send);
}

static hi_void pq_hdr_set_imap_cacm_coef(cm_coef *plutCMCoef, hi_u8 *addr)
{
    pq_coef_send_cfg coef_send;

    hi_u16 lut_eq_0[100] = { 0 }; /* 100 :a number */
    hi_u16 lut_eq_1[100] = { 0 }; /* 100 :a number */
    void *coef_array[11] = { /* 11 :a number */
        plutCMCoef->lut1_b, plutCMCoef->lut1_g,
        plutCMCoef->lut1_r, plutCMCoef->lut2_b,
        plutCMCoef->lut2_g, plutCMCoef->lut2_r,
        plutCMCoef->lut4_b, plutCMCoef->lut4_g,
        plutCMCoef->lut4_r, lut_eq_1, lut_eq_0
    };
    hi_u32 lut_length[11] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 }; /* 11 :a number 100 :a number 100 :a number 100 :a number 100 :a number 100 :a number 100 :a number 100 :a number 100 :a number 100 :a number 100 :a number 100 :a number */
    hi_u32 coef_bit_length[11] = { 12, 12, 12, 12, 12, 12, 12, 12, 12, 10, 10 }; /* 11 :a number 12 :a number 12 :a number 12 :a number 12 :a number 12 :a number 12 :a number 12 :a number 12 :a number 12 :a number 10 :a number 10 :a number */

    hi_u16 lut_eq_2[80] = { 0 }; /* 80 :a number */
    hi_u16 lut_eq_3[80] = { 0 }; /* 80 :a number */
    void *coef_array1[11] = { /* 11 :a number */
        plutCMCoef->lut3_b, plutCMCoef->lut3_g,
        plutCMCoef->lut3_r, plutCMCoef->lut5_b,
        plutCMCoef->lut5_g, plutCMCoef->lut5_r,
        plutCMCoef->lut6_b, plutCMCoef->lut6_g,
        plutCMCoef->lut6_r, lut_eq_3, lut_eq_2
    };
    hi_u32 lut_length1[11] = { 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80 }; /* 11 :a number 80 :a number 80 :a number 80 :a number 80 :a number 80 :a number 80 :a number 80 :a number 80 :a number 80 :a number 80 :a number 80 :a number */
    hi_u32 coef_bit_length1[11] = { 12, 12, 12, 12, 12, 12, 12, 12, 12, 10, 10 }; /* 11 :a number 12 :a number 12 :a number 12 :a number 12 :a number 12 :a number 12 :a number 12 :a number 12 :a number 12 :a number 10 :a number 10 :a number */

    hi_u16 lut_eq_4[64] = { 0 }; /* 64 :a number */
    hi_u16 lut_eq_5[64] = { 0 }; /* 64 :a number */
    void *coef_array2[11] = { /* 11 :a number */
        plutCMCoef->lut00_b, plutCMCoef->lut00_g,
        plutCMCoef->lut00_r, plutCMCoef->lut01_b,
        plutCMCoef->lut01_g, plutCMCoef->lut01_r,
        plutCMCoef->lut7_b, plutCMCoef->lut7_g,
        plutCMCoef->lut7_r, lut_eq_5, lut_eq_4
    };
    hi_u32 lut_length2[11] = { 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64 }; /* 11 :a number 64 :a number 64 :a number 64 :a number 64 :a number 64 :a number 64 :a number 64 :a number 64 :a number 64 :a number 64 :a number 64 :a number */
    hi_u32 coef_bit_length2[11] = { 12, 12, 12, 12, 12, 12, 12, 12, 12, 10, 10 }; /* 11 :a number 12 :a number 12 :a number 12 :a number 12 :a number 12 :a number 12 :a number 12 :a number 12 :a number 12 :a number 10 :a number 10 :a number */

    coef_send.coef_addr = addr;
    coef_send.lut_num = 11; /* 11 :a number */
    coef_send.burst_num = 1;
    coef_send.cycle_num = 1;
    coef_send.coef_array = coef_array;
    coef_send.lut_length = lut_length;
    coef_send.coef_bit_length = coef_bit_length;
    coef_send.data_type = PQ_COEF_DATA_TYPE_U16;
    addr = pq_ip_coef_sendcoef(&coef_send);

    coef_send.coef_addr = addr;
    coef_send.cycle_num = 1;
    coef_send.coef_array = coef_array1;
    coef_send.lut_length = lut_length1;
    coef_send.coef_bit_length = coef_bit_length1;
    coef_send.data_type = PQ_COEF_DATA_TYPE_U16;
    addr = pq_ip_coef_sendcoef(&coef_send);

    coef_send.coef_addr = addr;
    coef_send.cycle_num = 1;
    coef_send.coef_array = coef_array2;
    coef_send.lut_length = lut_length2;
    coef_send.coef_bit_length = coef_bit_length2;
    coef_send.data_type = PQ_COEF_DATA_TYPE_U16;
    addr = pq_ip_coef_sendcoef(&coef_send);
}

static hi_void pq_gfx_hdr_get_coef_rgb_lut(hi_u16 ii_r, hi_u16 ii_g, hi_u16 ii_b)
{
    hi_u16 index;
    if ((ii_r % DIV_2 == 0) && (ii_g % DIV_2 == 0) && (ii_b % DIV_2 == 0)) {
        index = EVE_SIZE * EVE_SIZE * ((ii_r + 1) / DIV_2) + EVE_SIZE * ((ii_g + 1) / DIV_2) + (ii_b + 1) / DIV_2;
        g_gfxhdr_coef_cm_lutr3d0[index] = g_gfxhdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutg3d0[index] = g_gfxhdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutb3d0[index] = g_gfxhdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    } else if ((ii_r % DIV_2 == 0) && (ii_g % DIV_2 == 0) && (ii_b % DIV_2 == 1)) {
        index = EVE_SIZE * ODD_SIZE * ((ii_r + 1) / DIV_2) + ODD_SIZE * ((ii_g + 1) / DIV_2) + (ii_b + 0) / DIV_2;
        g_gfxhdr_coef_cm_lutr3d1[index] = g_gfxhdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutg3d1[index] = g_gfxhdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutb3d1[index] = g_gfxhdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    } else if ((ii_r % DIV_2 == 0) && (ii_g % DIV_2 == 1) && (ii_b % DIV_2 == 0)) {
        index = EVE_SIZE * ODD_SIZE * ((ii_r + 1) / DIV_2) + EVE_SIZE * ((ii_g + 0) / DIV_2) + (ii_b + 1) / DIV_2;
        g_gfxhdr_coef_cm_lutr3d4[index] = g_gfxhdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutg3d4[index] = g_gfxhdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutb3d4[index] = g_gfxhdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    } else if ((ii_r % DIV_2 == 0) && (ii_g % DIV_2 == 1) && (ii_b % DIV_2 == 1)) {
        index = ODD_SIZE * ODD_SIZE * ((ii_r + 1) / DIV_2) + ODD_SIZE * ((ii_g + 0) / DIV_2) + (ii_b + 0) / DIV_2;
        g_gfxhdr_coef_cm_lutr3d5[index] = g_gfxhdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutg3d5[index] = g_gfxhdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutb3d5[index] = g_gfxhdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    } else if ((ii_r % DIV_2 == 1) && (ii_g % DIV_2 == 0) && (ii_b % DIV_2 == 0)) {
        index = EVE_SIZE * EVE_SIZE * ((ii_r + 0) / DIV_2) + EVE_SIZE * ((ii_g + 1) / DIV_2) + (ii_b + 1) / DIV_2;
        g_gfxhdr_coef_cm_lutr3d2[index] = g_gfxhdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutg3d2[index] = g_gfxhdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutb3d2[index] = g_gfxhdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    } else if ((ii_r % DIV_2 == 1) && (ii_g % DIV_2 == 0) && (ii_b % DIV_2 == 1)) {
        index = EVE_SIZE * ODD_SIZE * ((ii_r + 0) / DIV_2) + ODD_SIZE * ((ii_g + 1) / DIV_2) + (ii_b + 0) / DIV_2;
        g_gfxhdr_coef_cm_lutr3d3[index] = g_gfxhdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutg3d3[index] = g_gfxhdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutb3d3[index] = g_gfxhdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    } else if ((ii_r % DIV_2 == 1) && (ii_g % DIV_2 == 1) && (ii_b % DIV_2 == 0)) {
        index = EVE_SIZE * ODD_SIZE * ((ii_r + 0) / DIV_2) + EVE_SIZE * ((ii_g + 0) / DIV_2) + (ii_b + 1) / DIV_2;
        g_gfxhdr_coef_cm_lutr3d6[index] = g_gfxhdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutg3d6[index] = g_gfxhdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutb3d6[index] = g_gfxhdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    } else if ((ii_r % DIV_2 == 1) && (ii_g % DIV_2 == 1) && (ii_b % DIV_2 == 1)) {
        index = ODD_SIZE * ODD_SIZE * ((ii_r + 0) / DIV_2) + ODD_SIZE * ((ii_g + 0) / DIV_2) + (ii_b + 0) / DIV_2;
        g_gfxhdr_coef_cm_lutr3d7[index] = g_gfxhdr_coef_cm_lutr[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutg3d7[index] = g_gfxhdr_coef_cm_lutg[ii_r][ii_g][ii_b];
        g_gfxhdr_coef_cm_lutb3d7[index] = g_gfxhdr_coef_cm_lutb[ii_r][ii_g][ii_b];
    }
}

static hi_void pq_gfx_hdr_get_coef_lut(hi_void)
{
    hi_u16 ii_r, ii_g, ii_b;
    for (ii_r = 0; ii_r < LUT_SIZE; ii_r++) {
        for (ii_g = 0; ii_g < LUT_SIZE; ii_g++) {
            for (ii_b = 0; ii_b < LUT_SIZE; ii_b++) {
                pq_gfx_hdr_get_coef_rgb_lut(ii_r, ii_g, ii_b);
            }
        }
    }
}

static hi_void pq_gfx_hdr_get_coef_r_lut(hi_void)
{
    hi_u16 j;
    hi_u16 *clut00 = g_gfxhdr_coef_cm_lut00_r;
    hi_u16 *clut01 = g_gfxhdr_coef_cm_lut01_r;
    hi_u16 *clut1 = g_gfxhdr_coef_cm_lut1_r;
    hi_u16 *clut2 = g_gfxhdr_coef_cm_lut2_r;
    hi_u16 *clut3 = g_gfxhdr_coef_cm_lut3_r;
    hi_u16 *clut4 = g_gfxhdr_coef_cm_lut4_r;
    hi_u16 *clut5 = g_gfxhdr_coef_cm_lut5_r;
    hi_u16 *clut6 = g_gfxhdr_coef_cm_lut6_r;
    hi_u16 *clut7 = g_gfxhdr_coef_cm_lut7_r;

    for (j = 0; j < 125; j++) { /* 125 is length */
        if (j % DIV_2 == 0) {
            *clut00 = g_gfxhdr_coef_cm_lutr3d0[j];
            clut00++;
        } else if (j % DIV_2 == 1) {
            *clut01 = g_gfxhdr_coef_cm_lutr3d0[j];
            clut01++;
        }
    }

    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut1 = g_gfxhdr_coef_cm_lutr3d1[j];
        clut1++;
    }

    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut2 = g_gfxhdr_coef_cm_lutr3d2[j];
        clut2++;
    }

    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut3 = g_gfxhdr_coef_cm_lutr3d3[j];
        clut3++;
    }

    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut4 = g_gfxhdr_coef_cm_lutr3d4[j];
        clut4++;
    }

    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut5 = g_gfxhdr_coef_cm_lutr3d5[j];
        clut5++;
    }

    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut6 = g_gfxhdr_coef_cm_lutr3d6[j];
        clut6++;
    }

    for (j = 0; j < 64; j++) { /* 64 is length */
        *clut7 = g_gfxhdr_coef_cm_lutr3d7[j];
        clut7++;
    }
}

static hi_void pq_gfx_hdr_get_coef_g_lut(hi_void)
{
    hi_u16 j;
    hi_u16 *clut00 = g_gfxhdr_coef_cm_lut00_g;
    hi_u16 *clut01 = g_gfxhdr_coef_cm_lut01_g;
    hi_u16 *clut1 = g_gfxhdr_coef_cm_lut1_g;
    hi_u16 *clut2 = g_gfxhdr_coef_cm_lut2_g;
    hi_u16 *clut3 = g_gfxhdr_coef_cm_lut3_g;
    hi_u16 *clut4 = g_gfxhdr_coef_cm_lut4_g;
    hi_u16 *clut5 = g_gfxhdr_coef_cm_lut5_g;
    hi_u16 *clut6 = g_gfxhdr_coef_cm_lut6_g;
    hi_u16 *clut7 = g_gfxhdr_coef_cm_lut7_g;

    for (j = 0; j < 125; j++) { /* 125 is length */
        if (j % DIV_2 == 0) {
            *clut00 = g_gfxhdr_coef_cm_lutg3d0[j];
            clut00++;
        } else if (j % DIV_2 == 1) {
            *clut01 = g_gfxhdr_coef_cm_lutg3d0[j];
            clut01++;
        }
    }

    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut1 = g_gfxhdr_coef_cm_lutg3d1[j];
        clut1++;
    }

    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut2 = g_gfxhdr_coef_cm_lutg3d2[j];
        clut2++;
    }

    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut3 = g_gfxhdr_coef_cm_lutg3d3[j];
        clut3++;
    }

    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut4 = g_gfxhdr_coef_cm_lutg3d4[j];
        clut4++;
    }

    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut5 = g_gfxhdr_coef_cm_lutg3d5[j];
        clut5++;
    }

    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut6 = g_gfxhdr_coef_cm_lutg3d6[j];
        clut6++;
    }

    for (j = 0; j < 64; j++) { /* 64 is length */
        *clut7 = g_gfxhdr_coef_cm_lutg3d7[j];
        clut7++;
    }
}

static hi_void pq_gfx_hdr_get_coef_b_lut(hi_void)
{
    hi_u16 j;
    hi_u16 *clut00 = g_gfxhdr_coef_cm_lut00_b;
    hi_u16 *clut01 = g_gfxhdr_coef_cm_lut01_b;
    hi_u16 *clut1 = g_gfxhdr_coef_cm_lut1_b;
    hi_u16 *clut2 = g_gfxhdr_coef_cm_lut2_b;
    hi_u16 *clut3 = g_gfxhdr_coef_cm_lut3_b;
    hi_u16 *clut4 = g_gfxhdr_coef_cm_lut4_b;
    hi_u16 *clut5 = g_gfxhdr_coef_cm_lut5_b;
    hi_u16 *clut6 = g_gfxhdr_coef_cm_lut6_b;
    hi_u16 *clut7 = g_gfxhdr_coef_cm_lut7_b;

    for (j = 0; j < 125; j++) { /* 125 is length */
        if (j % DIV_2 == 0) {
            *clut00 = g_gfxhdr_coef_cm_lutb3d0[j];
            clut00++;
        } else if (j % DIV_2 == 1) {
            *clut01 = g_gfxhdr_coef_cm_lutb3d0[j];
            clut01++;
        }
    }
    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut1 = g_gfxhdr_coef_cm_lutb3d1[j];
        clut1++;
    }

    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut2 = g_gfxhdr_coef_cm_lutb3d2[j];
        clut2++;
    }
    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut3 = g_gfxhdr_coef_cm_lutb3d3[j];
        clut3++;
    }
    for (j = 0; j < 100; j++) { /* 100 is length */
        *clut4 = g_gfxhdr_coef_cm_lutb3d4[j];
        clut4++;
    }
    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut5 = g_gfxhdr_coef_cm_lutb3d5[j];
        clut5++;
    }

    for (j = 0; j < 80; j++) { /* 80 is length */
        *clut6 = g_gfxhdr_coef_cm_lutb3d6[j];
        clut6++;
    }
    for (j = 0; j < 64; j++) { /* 64 is length */
        *clut7 = g_gfxhdr_coef_cm_lutb3d7[j];
        clut7++;
    }
}

static hi_void pq_hdr_set_imap_cm_coef(hi_u8 *addr)
{
    cm_coef lut_cm_coef;
    hi_u16 i, j, k;

    for (i = 0; i < LUT_SIZE; i++) {
        for (j = 0; j < LUT_SIZE; j++) {
            for (k = 0; k < LUT_SIZE; k++) {
                g_gfxhdr_coef_cm_lutr[i][j][k] = g_gfxhdr_imap_cacm_lut_r[i][j][k];
                g_gfxhdr_coef_cm_lutg[i][j][k] = g_gfxhdr_imap_cacm_lut_g[i][j][k];
                g_gfxhdr_coef_cm_lutb[i][j][k] = g_gfxhdr_imap_cacm_lut_b[i][j][k];
            }
        }
    }

    pq_gfx_hdr_get_coef_lut();
    pq_gfx_hdr_get_coef_r_lut();
    pq_gfx_hdr_get_coef_g_lut();
    pq_gfx_hdr_get_coef_b_lut();

    lut_cm_coef.lut00_r = g_gfxhdr_coef_cm_lut00_r;
    lut_cm_coef.lut00_g = g_gfxhdr_coef_cm_lut00_g;
    lut_cm_coef.lut00_b = g_gfxhdr_coef_cm_lut00_b;
    lut_cm_coef.lut01_r = g_gfxhdr_coef_cm_lut01_r;
    lut_cm_coef.lut01_g = g_gfxhdr_coef_cm_lut01_g;
    lut_cm_coef.lut01_b = g_gfxhdr_coef_cm_lut01_b;
    lut_cm_coef.lut1_r = g_gfxhdr_coef_cm_lut1_r;
    lut_cm_coef.lut1_g = g_gfxhdr_coef_cm_lut1_g;
    lut_cm_coef.lut1_b = g_gfxhdr_coef_cm_lut1_b;
    lut_cm_coef.lut2_r = g_gfxhdr_coef_cm_lut2_r;
    lut_cm_coef.lut2_g = g_gfxhdr_coef_cm_lut2_g;
    lut_cm_coef.lut2_b = g_gfxhdr_coef_cm_lut2_b;
    lut_cm_coef.lut3_r = g_gfxhdr_coef_cm_lut3_r;
    lut_cm_coef.lut3_g = g_gfxhdr_coef_cm_lut3_g;
    lut_cm_coef.lut3_b = g_gfxhdr_coef_cm_lut3_b;
    lut_cm_coef.lut4_r = g_gfxhdr_coef_cm_lut4_r;
    lut_cm_coef.lut4_g = g_gfxhdr_coef_cm_lut4_g;
    lut_cm_coef.lut4_b = g_gfxhdr_coef_cm_lut4_b;
    lut_cm_coef.lut5_r = g_gfxhdr_coef_cm_lut5_r;
    lut_cm_coef.lut5_g = g_gfxhdr_coef_cm_lut5_g;
    lut_cm_coef.lut5_b = g_gfxhdr_coef_cm_lut5_b;
    lut_cm_coef.lut6_r = g_gfxhdr_coef_cm_lut6_r;
    lut_cm_coef.lut6_g = g_gfxhdr_coef_cm_lut6_g;
    lut_cm_coef.lut6_b = g_gfxhdr_coef_cm_lut6_b;
    lut_cm_coef.lut7_r = g_gfxhdr_coef_cm_lut7_r;
    lut_cm_coef.lut7_g = g_gfxhdr_coef_cm_lut7_g;
    lut_cm_coef.lut7_b = g_gfxhdr_coef_cm_lut7_b;

    pq_hdr_set_imap_cacm_coef(&lut_cm_coef, addr);
}

static hi_void pq_gfxhdr_get_degamma_coef_cfg(pq_coef_gen_cfg *coef_gen)
{
    coef_gen->coef = g_gfxhdr_imap_degamma;
    coef_gen->coef_new = g_db_hdr_degamm_pq_new;
    coef_gen->coef_data_type = PQ_COEF_DATA_TYPE_U32;
    coef_gen->length = 1024; /* 1024 is length */
    coef_gen->coef_max = (hi_u32)(((hi_u64)1 << 31) - 1); /* 31 is length */
    coef_gen->coef_min = 0;
}

static hi_void pq_gfxhdr_get_gamma_x_coef_cfg(pq_coef_gen_cfg *coef_gen)
{
    coef_gen->coef = g_gfxhdr_imap_gamma_lut_x;
    coef_gen->coef_new = g_dbhdr_gamm_l2pq_lut_x;
    coef_gen->coef_data_type = PQ_COEF_DATA_TYPE_U32;
    coef_gen->length = 128; /* 128 is length */
    coef_gen->coef_max = (hi_u32)(((hi_u64)1 << 31) - 1); /* 31 is length */
    coef_gen->coef_min = 0;
}

static hi_void pq_gfxhdr_get_gamma_a_coef_cfg(pq_coef_gen_cfg *coef_gen)
{
    coef_gen->coef = g_gfxhdr_imap_gamma_lut_a;
    coef_gen->coef_new = g_dbhdr_gamm_l2pq_lut_a;
    coef_gen->coef_data_type = PQ_COEF_DATA_TYPE_U32;
    coef_gen->length = 128; /* 128 is length */
    coef_gen->coef_max = (1 << 28) - 1; /* 28 is length */
    coef_gen->coef_min = 0;
}

static hi_void pq_gfxhdr_get_gamma_b_coef_cfg(pq_coef_gen_cfg *coef_gen)
{
    coef_gen->coef = g_gfxhdr_imap_gamma_lut_b;
    coef_gen->coef_new = g_dbhdr_gamm_l2pq_lut_b;
    coef_gen->coef_data_type = PQ_COEF_DATA_TYPE_U32;
    coef_gen->length = 128; /* 128 is length */
    coef_gen->coef_max = (1 << 16) - 1; /* 16 is length */
    coef_gen->coef_min = 0;
}

static hi_void pq_gfxhdr_set_imap_coef2ddr(hi_u8 *addr, hi_u8 *addr2)
{
    pq_coef_gen_cfg coef_gen;
    pq_coef_send_cfg coef_send;
    void *coef_array[1];
    hi_u32 lut_length = 1024; /* 1024 is length */
    hi_u32 coef_bit_length = 32; /* 32 is length */
    void *coef_array_l2pq[] = { g_dbhdr_gamm_l2pq_lut_x, g_dbhdr_gamm_l2pq_lut_a, g_dbhdr_gamm_l2pq_lut_b };
    hi_u32 lut_length2[] = { 128, 128, 128 };
    hi_u32 coef_bit_length2[] = { 32, 28, 16 };

    pq_gfxhdr_get_degamma_coef_cfg(&coef_gen);
    pq_drv_gencoef(&coef_gen);

    coef_array[0] = g_db_hdr_degamm_pq_new;

    coef_send.coef_addr = addr;
    coef_send.lut_num = 1;
    coef_send.burst_num = 1;
    coef_send.cycle_num = 4; /* 4 is length */
    coef_send.coef_array = coef_array;
    coef_send.lut_length = &lut_length;
    coef_send.coef_bit_length = &coef_bit_length;
    coef_send.data_type = PQ_COEF_DATA_TYPE_U32;
    pq_ip_coef_sendcoef(&coef_send);

    pq_gfxhdr_get_gamma_x_coef_cfg(&coef_gen);
    pq_drv_gencoef(&coef_gen);

    pq_gfxhdr_get_gamma_a_coef_cfg(&coef_gen);
    pq_drv_gencoef(&coef_gen);

    pq_gfxhdr_get_gamma_b_coef_cfg(&coef_gen);
    pq_drv_gencoef(&coef_gen);

    coef_send.coef_addr = addr2;
    coef_send.lut_num = 3; /* 3 is length */
    coef_send.burst_num = 1;
    coef_send.cycle_num = 1;
    coef_send.coef_array = coef_array_l2pq;
    coef_send.lut_length = lut_length2;
    coef_send.coef_bit_length = coef_bit_length2;
    coef_send.data_type = PQ_COEF_DATA_TYPE_U32;
    pq_ip_coef_sendcoef(&coef_send);

    return;
}


hi_void pq_hal_gfxhdr_get_hdr_coef(pq_hal_gfxhdr_cfg *hdr_cfg)
{
    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        pq_gfxhdr_set_imap_coef2ddr(g_pq_coef_buf_addr.vir_addr[PQ_COEF_BUF_GP0_HDR0],
                                    g_pq_coef_buf_addr.vir_addr[PQ_COEF_BUF_GP0_HDR1]);
    }

    if (hdr_cfg->imap_cfg.tmap_v1.enable == HI_TRUE) {
        pq_gfxhdr_set_imap_tmpa_v1_coef(g_pq_coef_buf_addr.vir_addr[PQ_COEF_BUF_GP0_HDR2]);
    }

    if (hdr_cfg->imap_cfg.cacm.enable == HI_TRUE) {
        pq_hdr_set_imap_cm_coef(g_pq_coef_buf_addr.vir_addr[PQ_COEF_BUF_GP0_HDR3]);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        /* Reserved address */
    }

    hdr_cfg->coef_addr.imap_degamma_addr = g_pq_coef_buf_addr.phy_addr[PQ_COEF_BUF_GP0_HDR0];
    hdr_cfg->coef_addr.imap_gamma_addr = g_pq_coef_buf_addr.phy_addr[PQ_COEF_BUF_GP0_HDR1];
    hdr_cfg->coef_addr.imap_tmapv1_addr = g_pq_coef_buf_addr.phy_addr[PQ_COEF_BUF_GP0_HDR2];
    hdr_cfg->coef_addr.imap_cacm_addr = g_pq_coef_buf_addr.phy_addr[PQ_COEF_BUF_GP0_HDR3];
}

#endif

hi_s32 pq_hal_set_gfxhdr_csc_setting(hi_pq_image_param *pic_setting)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(pic_setting);

    /* gfx csc use 0 -100, convert to 0 - 255 to sdr2hdr */
    g_gfxhdr_bright = LEVEL2NUM(pic_setting->brightness);
    g_gfxhdr_contrast = LEVEL2NUM(pic_setting->contrast);
    g_gfxhdr_satu = LEVEL2NUM(pic_setting->hue);
    g_gfxhdr_hue = LEVEL2NUM(pic_setting->saturation);
    g_gfxhdr_wcg_temperature = LEVEL2NUM(pic_setting->wcg_temperature);

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_gfxhdr_offset(hi_pq_hdr_offset *hdr_offset_para)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_offset_para);

    g_gfxhdr_offset_bright[hdr_offset_para->hdr_process_scene] = hdr_offset_para->bright;
    g_gfxhdr_offset_contrast[hdr_offset_para->hdr_process_scene] = hdr_offset_para->contrast;
    g_gfxhdr_offset_satu[hdr_offset_para->hdr_process_scene] = hdr_offset_para->satu;
    g_gfxhdr_offset_hue[hdr_offset_para->hdr_process_scene] = hdr_offset_para->hue;
    g_gfxhdr_offset_red[hdr_offset_para->hdr_process_scene] = hdr_offset_para->r;
    g_gfxhdr_offset_green[hdr_offset_para->hdr_process_scene] = hdr_offset_para->g;
    g_gfxhdr_offset_blue[hdr_offset_para->hdr_process_scene] = hdr_offset_para->b;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_gfxhdr_tm_curve(hi_pq_hdr_offset *hdr_offset_para)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_offset_para);

    g_gfxhdr_dark_cv_str[hdr_offset_para->hdr_process_scene] = hdr_offset_para->dark_cv;
    g_gfxhdr_bright_cv_str[hdr_offset_para->hdr_process_scene] = hdr_offset_para->bright_cv;

    return HI_SUCCESS;
}

static hi_void pq_hal_gfxhdr_get_yuv2rgb_imap_matrix(drv_pq_internal_color_space in_cs, hi_drv_pq_imap_cfg *imap_cfg)
{
    switch (in_cs) {
        case HI_DRV_CS_BT601_YUV_LIMITED:
        case HI_DRV_CS_BT601_YUV_FULL:
        case HI_DRV_CS_BT709_YUV_LIMITED:
        case HI_DRV_CS_BT709_YUV_FULL:
        case HI_DRV_CS_BT2020_YUV_LIMITED:
        case HI_DRV_CS_BT2020_YUV_FULL:
            imap_cfg->yuv2rgb.scale2p = 11; /* 11 : scale2p */
            memcpy(imap_cfg->yuv2rgb.coef, g_gfxhdr_y2r_m33, sizeof(imap_cfg->yuv2rgb.coef));
            memcpy(imap_cfg->yuv2rgb.offinrgb, g_gfxhdr_y2r_offinrgb, sizeof(imap_cfg->yuv2rgb.offinrgb));
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


hi_void pq_hal_gfxhdr_get_yuv2rgb_matrix(drv_pq_internal_color_space in_cs,
                                         drv_pq_internal_color_space out_cs,
                                         hi_drv_pq_imap_cfg *imap_cfg)
{
    imap_cfg->yuv2rgb.mode = 3; /* 3 : mode,BT2020CL should set 2 */
    imap_cfg->yuv2rgb.clip_max = 134215680; /* 134215680 : clip_max */
    imap_cfg->yuv2rgb.clip_min = 0;
    imap_cfg->yuv2rgb.thr_r = 0;
    imap_cfg->yuv2rgb.thr_b = 0;

    memcpy(imap_cfg->yuv2rgb.dc_in, g_gfxhdr_y2r_dc_in, sizeof(imap_cfg->yuv2rgb.dc_in));
    memcpy(imap_cfg->yuv2rgb.dc_in2, g_gfxhdr_y2r_dc_in, sizeof(imap_cfg->yuv2rgb.dc_in2));
    memcpy(imap_cfg->yuv2rgb.offinrgb2, g_gfxhdr_y2r_offinrgb2, sizeof(imap_cfg->yuv2rgb.offinrgb2));
    memcpy(imap_cfg->yuv2rgb.coef2, g_gfxhdr_y2r_m33_2, sizeof(imap_cfg->yuv2rgb.coef2));

    pq_hal_gfxhdr_get_yuv2rgb_imap_matrix(in_cs, imap_cfg);
}

hi_void pq_hal_gfxhdr_get_imap_cacm_cfg(drv_pq_internal_color_space in_cs,
                                        drv_pq_internal_color_space out_cs,
                                        hi_drv_pq_imap_cfg *imap_cfg)
{
    /* module on/off according 709/2020, pos according HDR/SDR */
    imap_cfg->cacm.demo_pos = 0;
    imap_cfg->cacm.demo_mode = 0;
    imap_cfg->cacm.bitdepth_out_mode = 3; /* 3: bitdepth_out_mode */
    imap_cfg->cacm.bitdepth_in_mode = 3;  /* 3: bitdepth_in_mode */

    switch (g_gfxhdr_mode) {
        case HI_PQ_HDR_MODE_SDR_TO_HDR10:
            imap_cfg->cacm.cm_pos = 0;
            memcpy(g_gfxhdr_imap_cacm_lut_r, g_gfxhdr_cmlut_expansion_r, sizeof(g_gfxhdr_imap_cacm_lut_r));
            memcpy(g_gfxhdr_imap_cacm_lut_g, g_gfxhdr_cmlut_expansion_g, sizeof(g_gfxhdr_imap_cacm_lut_g));
            memcpy(g_gfxhdr_imap_cacm_lut_b, g_gfxhdr_cmlut_expansion_b, sizeof(g_gfxhdr_imap_cacm_lut_b));
            break;
        case HI_PQ_HDR_MODE_SDR_TO_HLG:
            imap_cfg->cacm.cm_pos = 0;
            memcpy(g_gfxhdr_imap_cacm_lut_r, g_gfxhdr_cmlut_expansion_r, sizeof(g_gfxhdr_imap_cacm_lut_r));
            memcpy(g_gfxhdr_imap_cacm_lut_g, g_gfxhdr_cmlut_expansion_g, sizeof(g_gfxhdr_imap_cacm_lut_g));
            memcpy(g_gfxhdr_imap_cacm_lut_b, g_gfxhdr_cmlut_expansion_b, sizeof(g_gfxhdr_imap_cacm_lut_b));
            break;
        case HI_PQ_HDR_MODE_HDR10_TO_SDR:
            imap_cfg->cacm.cm_pos = 1;
            memcpy(g_gfxhdr_imap_cacm_lut_r, g_gfxhdr_cmlut_compression_r, sizeof(g_gfxhdr_imap_cacm_lut_r));
            memcpy(g_gfxhdr_imap_cacm_lut_g, g_gfxhdr_cmlut_compression_g, sizeof(g_gfxhdr_imap_cacm_lut_g));
            memcpy(g_gfxhdr_imap_cacm_lut_b, g_gfxhdr_cmlut_compression_b, sizeof(g_gfxhdr_imap_cacm_lut_b));
            break;
        case HI_PQ_HDR_MODE_HLG_TO_SDR:
            imap_cfg->cacm.cm_pos = 1;
            memcpy(g_gfxhdr_imap_cacm_lut_r, g_gfxhdr_cmlut_compression_r, sizeof(g_gfxhdr_imap_cacm_lut_r));
            memcpy(g_gfxhdr_imap_cacm_lut_g, g_gfxhdr_cmlut_compression_g, sizeof(g_gfxhdr_imap_cacm_lut_g));
            memcpy(g_gfxhdr_imap_cacm_lut_b, g_gfxhdr_cmlut_compression_b, sizeof(g_gfxhdr_imap_cacm_lut_b));
            break;
        case HI_PQ_HDR_MODE_SDR_TO_SDR:
        case HI_PQ_HDR_MODE_HDR10_TO_HDR10:
        case HI_PQ_HDR_MODE_HDR10_TO_HLG:
        case HI_PQ_HDR_MODE_HLG_TO_HDR10:
        case HI_PQ_HDR_MODE_HLG_TO_HLG:
            imap_cfg->cacm.enable = HI_FALSE;
            break;
        default:
            imap_cfg->cacm.enable = HI_FALSE;
            break;
    }
}

hi_void pq_hal_gfxhdr_get_rgb2lms_matrix(drv_pq_internal_color_space in_cs, drv_pq_internal_color_space out_cs,
                                         hi_drv_pq_imap_cfg *imap_cfg)
{
    /* hdrv1_mode : RGB2RGB */
    imap_cfg->rgb2lms.scale2p = g_gfxhdr_map_scale; /* 14 : scale2p */
    imap_cfg->rgb2lms.clip_min = 0;
    imap_cfg->rgb2lms.clip_max = 2621440000; /* 2621440000 is max clip */
    memcpy(imap_cfg->rgb2lms.dc_in, g_gfxhdr_hdr2sdr_imap_r2l_dc_in, sizeof(imap_cfg->rgb2lms.dc_in));
    if ((in_cs == HI_DRV_CS_BT2020_YUV_LIMITED || in_cs == HI_DRV_CS_BT2020_YUV_FULL ||
         in_cs == HI_DRV_CS_BT2020_RGB_LIMITED || in_cs == HI_DRV_CS_BT2020_RGB_FULL) &&
        (out_cs == HI_DRV_CS_BT709_YUV_LIMITED || out_cs == HI_DRV_CS_BT709_YUV_FULL ||
         out_cs == HI_DRV_CS_BT709_RGB_LIMITED || out_cs == HI_DRV_CS_BT709_RGB_FULL)) {
        memcpy(imap_cfg->rgb2lms.coef, g_gfxhdr_map_m33_2020_ncl_709, sizeof(imap_cfg->rgb2lms.coef));
    } else if ((in_cs == HI_DRV_CS_BT709_YUV_LIMITED || in_cs == HI_DRV_CS_BT709_YUV_FULL ||
                in_cs == HI_DRV_CS_BT709_RGB_LIMITED || in_cs == HI_DRV_CS_BT709_RGB_FULL) &&
               (out_cs == HI_DRV_CS_BT2020_YUV_LIMITED || out_cs == HI_DRV_CS_BT2020_YUV_FULL ||
                out_cs == HI_DRV_CS_BT2020_RGB_LIMITED || out_cs == HI_DRV_CS_BT2020_RGB_FULL)) {
        memcpy(imap_cfg->rgb2lms.coef, g_gfxhdr_map_m33_709_2020_ncl, sizeof(imap_cfg->rgb2lms.coef));
    } else if ((in_cs == HI_DRV_CS_BT601_YUV_LIMITED || in_cs == HI_DRV_CS_BT601_YUV_FULL ||
                in_cs == HI_DRV_CS_BT601_RGB_LIMITED || in_cs == HI_DRV_CS_BT601_RGB_FULL) &&
               (out_cs == HI_DRV_CS_BT2020_YUV_LIMITED || out_cs == HI_DRV_CS_BT2020_YUV_FULL ||
                out_cs == HI_DRV_CS_BT2020_RGB_LIMITED || out_cs == HI_DRV_CS_BT2020_RGB_FULL)) {
        memcpy(imap_cfg->rgb2lms.coef, g_gfxhdr_map_m33_709_2020_ncl, sizeof(imap_cfg->rgb2lms.coef));
    } else {
        memcpy(imap_cfg->rgb2lms.coef, g_gfxhdr_map_m33_linear, sizeof(imap_cfg->rgb2lms.coef));
    }

    if (imap_cfg->cacm.enable == HI_TRUE) {
        memcpy(imap_cfg->rgb2lms.coef, g_gfxhdr_map_m33_linear, sizeof(imap_cfg->rgb2lms.coef));
    }

    return;
}

hi_void pq_hal_gfxhdr_get_tm_v1_lut(hi_drv_pq_imap_cfg *imap_cfg)
{
    switch (g_gfxhdr_mode) {
        case HI_PQ_HDR_MODE_HDR10_TO_HDR10:
            imap_cfg->tmap_v1.scale_coef = g_gfxhdr_tm_v1_scale_coef_linear;
            g_gfxhdr_tmapv1_lut = g_gfxhdr_tm_v1_lut_linear;
            break;
        case HI_PQ_HDR_MODE_HDR10_TO_HLG:
            imap_cfg->tmap_v1.scale_coef = g_gfxhdr_tm_v1_scale_coef_pq2hlg;
            g_gfxhdr_tmapv1_lut = g_gfxhdr_tm_v1_lut_pq2hlg;
            break;
        case HI_PQ_HDR_MODE_HDR10_TO_SDR:
            imap_cfg->tmap_v1.scale_coef = g_gfxhdr_tm_v1_scale_coef_pq2sdr;
            g_gfxhdr_tmapv1_lut = g_gfxhdr_tm_v1_lut_pq2sdr;
            break;
        case HI_PQ_HDR_MODE_HLG_TO_HDR10:
            imap_cfg->tmap_v1.scale_coef = g_gfxhdr_tm_v1_scale_coef_hlg2pq;
            g_gfxhdr_tmapv1_lut = g_gfxhdr_tm_v1_lut_hlg2pq;
            break;
        case HI_PQ_HDR_MODE_HLG_TO_HLG:
            imap_cfg->tmap_v1.scale_coef = g_gfxhdr_tm_v1_scale_coef_linear;
            g_gfxhdr_tmapv1_lut = g_gfxhdr_tm_v1_lut_linear;
            break;
        case HI_PQ_HDR_MODE_HLG_TO_SDR:
            imap_cfg->tmap_v1.scale_coef = g_gfxhdr_tm_v1_scale_coef_hlg2sdr;
            g_gfxhdr_tmapv1_lut = g_gfxhdr_tm_v1_lut_hlg2sdr;
            break;
        case HI_PQ_HDR_MODE_SDR_TO_HDR10:
            imap_cfg->tmap_v1.scale_coef = g_gfxhdr_tm_v1_scale_coef_sdr2pq;
            g_gfxhdr_tmapv1_lut = g_gfxhdr_tm_v1_lut_sdr2pq;
            break;
        case HI_PQ_HDR_MODE_SDR_TO_HLG:
            imap_cfg->tmap_v1.scale_coef = g_gfxhdr_tm_v1_scale_coef_sdr2hlg;
            g_gfxhdr_tmapv1_lut = g_gfxhdr_tm_v1_lut_sdr2hlg;
            break;
        case HI_PQ_HDR_MODE_SDR_TO_SDR:
            imap_cfg->tmap_v1.scale_coef = g_gfxhdr_tm_v1_scale_coef_linear;
            g_gfxhdr_tmapv1_lut = g_gfxhdr_tm_v1_lut_linear;
            break;
        default:
            imap_cfg->tmap_v1.scale_coef = g_gfxhdr_tm_v1_scale_coef_linear;
            g_gfxhdr_tmapv1_lut = g_gfxhdr_tm_v1_lut_linear;
            break;
    }
}

hi_void pq_hal_gfxhdr_get_tmap_v1_cfg(drv_pq_internal_color_space in_cs, drv_pq_internal_color_space out_cs,
                                      hi_drv_pq_imap_cfg *imap_cfg)
{
    /* related with scene, para change in case */
    imap_cfg->tmap_v1.tm_pos = 0;
    imap_cfg->tmap_v1.scale_mix_alpha = 15; /* 15 :a number */
    imap_cfg->tmap_v1.mix_alpha = 0;

    imap_cfg->tmap_v1.clip_min = g_gfxhdr_tm_clip_min;
    imap_cfg->tmap_v1.clip_max = g_gfxhdr_tm_clip_max;

    imap_cfg->tmap_v1.scale_lum_cal = 15; /* 15 :a number */
    imap_cfg->tmap_v1.scale_coef = 15; /* 15 :a number */
    memcpy(imap_cfg->tmap_v1.step, g_gfxhdr_tm_xstep, sizeof(imap_cfg->tmap_v1.step));
    memcpy(imap_cfg->tmap_v1.pos, g_gfxhdr_tm_xpos, sizeof(imap_cfg->tmap_v1.pos));
    memcpy(imap_cfg->tmap_v1.num, g_gfxhdr_tm_xnum, sizeof(imap_cfg->tmap_v1.num));
    memcpy(imap_cfg->tmap_v1.lum_cal, g_gfxhdr_tm_lum_cal, sizeof(imap_cfg->tmap_v1.lum_cal));
    memcpy(imap_cfg->tmap_v1.dc_out, g_gfxhdr_tm_dc_out, sizeof(imap_cfg->tmap_v1.dc_out));

    pq_hal_gfxhdr_get_tm_v1_lut(imap_cfg);
}

hi_void pq_hal_gfxhdr_get_lms2ipt_matrix(drv_pq_internal_color_space in_cs, drv_pq_internal_color_space out_cs,
                                         hi_drv_pq_imap_cfg *imap_cfg)
{
    /* RGB2YUV without DcOut */
    imap_cfg->lms2ipt.scale2p = g_gfxhdr_m33_rgb2yuv_scale2p; /* 12: lms2ipt scale */
    imap_cfg->lms2ipt.clip_min_y = 0;   /* -32767: lms2ipt clip */
    imap_cfg->lms2ipt.clip_max_y = 32767;   /* 32767: lms2ipt scale */
    imap_cfg->lms2ipt.clip_min_c = 0;   /* -32767: lms2ipt clip */
    imap_cfg->lms2ipt.clip_max_c = 32767;   /* 32767: lms2ipt scale */

    memcpy(imap_cfg->lms2ipt.coef, g_gfxhdr_m33_rgb2yuv_linear, sizeof(imap_cfg->lms2ipt.coef));
    memcpy(imap_cfg->lms2ipt.dc_in, g_gfxhdr_v3_rgb2yuv_off_dc_zero, sizeof(imap_cfg->lms2ipt.dc_in));
    memcpy(imap_cfg->lms2ipt.dc_out, g_gfxhdr_lms2ipt_dc_out, sizeof(imap_cfg->lms2ipt.dc_out));

    switch (out_cs) {
        case HI_DRV_CS_BT601_YUV_LIMITED:
            memcpy(imap_cfg->lms2ipt.coef, g_gfxhdr_m33_rgb2yuv_bt709rf_to_bt601yl, sizeof(imap_cfg->lms2ipt.coef));
            break;
        case HI_DRV_CS_BT601_YUV_FULL:
            memcpy(imap_cfg->lms2ipt.coef, g_gfxhdr_m33_rgb2yuv_bt709rf_to_bt601yf, sizeof(imap_cfg->lms2ipt.coef));
            break;
        case HI_DRV_CS_BT709_YUV_LIMITED:
            memcpy(imap_cfg->lms2ipt.coef, g_gfxhdr_m33_rgb2yuv_bt709_rf2yl_2, sizeof(imap_cfg->lms2ipt.coef));
            break;
        case HI_DRV_CS_BT709_YUV_FULL:
            memcpy(imap_cfg->lms2ipt.coef, g_gfxhdr_m33_rgb2yuv_bt709rf2yf, sizeof(imap_cfg->lms2ipt.coef));
            break;
        case HI_DRV_CS_BT2020_YUV_LIMITED:
            memcpy(imap_cfg->lms2ipt.coef, g_gfxhdr_m33_rgb2yuv_bt2020_rf2yl_2, sizeof(imap_cfg->lms2ipt.coef));
            break;
        case HI_DRV_CS_BT2020_YUV_FULL:
            memcpy(imap_cfg->lms2ipt.coef, g_gfxhdr_m33_rgb2yuv_bt2020rf2yf, sizeof(imap_cfg->lms2ipt.coef));
            break;
        case HI_DRV_CS_BT601_RGB_LIMITED:
        case HI_DRV_CS_BT601_RGB_FULL:
        case HI_DRV_CS_BT709_RGB_LIMITED:
        case HI_DRV_CS_BT709_RGB_FULL:
        case HI_DRV_CS_BT2020_RGB_LIMITED:
        case HI_DRV_CS_BT2020_RGB_FULL:
        default:
            memcpy(imap_cfg->lms2ipt.coef, g_gfxhdr_m33_rgb2yuv_linear, sizeof(imap_cfg->lms2ipt.coef));
            break;
    }
}

hi_void pq_hal_gfxhdr_get_imap_default_cfg(hi_drv_pq_imap_cfg *imap_cfg)
{
    /* default para, every scene not change it */
    imap_cfg->v0_hdr_enable = HI_TRUE;
    imap_cfg->v1_hdr_enable = HI_FALSE;

    imap_cfg->yuv2rgb.v0_enable = HI_TRUE;
    imap_cfg->yuv2rgb.v1_enable = HI_FALSE; /* PIP use, this version only use v0 */

    imap_cfg->eotfparam_eotf = 1;
    imap_cfg->eotfparam_range_min = 0;
    imap_cfg->eotfparam_range = 16383; /* 16383: range */
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
    imap_cfg->rshift_en = HI_FALSE; /* logic delete it */

    return;
}

hi_s32 pq_hal_gfxhdr_get_sdr2sdr_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                               drv_pq_internal_color_space out_cs,
                                               hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_FALSE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE; /* SDR_WCG 关闭 */
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_GFXHDR_CM_POS0;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmapv1 */
    imap_cfg->tmap_v1.enable = HI_FALSE;
    imap_cfg->tmap_v1.tm_pos = PQ_GFXHDR_TM_POS0; /* 这个的绑定位置如何决策？ */

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_gfxhdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_gfxhdr_get_sdr2hdr_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                               drv_pq_internal_color_space out_cs,
                                               hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_FALSE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE;
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_GFXHDR_CM_POS0;
    if ((in_cs == HI_DRV_CS_BT709_YUV_LIMITED || in_cs == HI_DRV_CS_BT709_YUV_FULL ||
         in_cs == HI_DRV_CS_BT709_RGB_LIMITED || in_cs == HI_DRV_CS_BT709_RGB_FULL) &&
        (out_cs == HI_DRV_CS_BT2020_YUV_LIMITED || out_cs == HI_DRV_CS_BT2020_YUV_FULL ||
         out_cs == HI_DRV_CS_BT2020_RGB_LIMITED || out_cs == HI_DRV_CS_BT2020_RGB_FULL)) {
        imap_cfg->cacm.enable = HI_TRUE;
    }
    if ((in_cs == HI_DRV_CS_BT601_YUV_LIMITED || in_cs == HI_DRV_CS_BT601_YUV_FULL ||
         in_cs == HI_DRV_CS_BT601_RGB_LIMITED || in_cs == HI_DRV_CS_BT601_RGB_FULL) &&
        (out_cs == HI_DRV_CS_BT2020_YUV_LIMITED || out_cs == HI_DRV_CS_BT2020_YUV_FULL ||
         out_cs == HI_DRV_CS_BT2020_RGB_LIMITED || out_cs == HI_DRV_CS_BT2020_RGB_FULL)) {
        imap_cfg->cacm.enable = HI_TRUE;
    }

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_GFXHDR_TM_POS1;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_gfxhdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_gfxhdr_get_sdr2hlg_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                               drv_pq_internal_color_space out_cs,
                                               hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_FALSE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE;
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_GFXHDR_CM_POS0;
    if ((in_cs == HI_DRV_CS_BT709_YUV_LIMITED || in_cs == HI_DRV_CS_BT709_YUV_FULL ||
         in_cs == HI_DRV_CS_BT709_RGB_LIMITED || in_cs == HI_DRV_CS_BT709_RGB_FULL) &&
        (out_cs == HI_DRV_CS_BT2020_YUV_LIMITED || out_cs == HI_DRV_CS_BT2020_YUV_FULL ||
         out_cs == HI_DRV_CS_BT2020_RGB_LIMITED || out_cs == HI_DRV_CS_BT2020_RGB_FULL)) {
        imap_cfg->cacm.enable = HI_TRUE;
    }
    if ((in_cs == HI_DRV_CS_BT601_YUV_LIMITED || in_cs == HI_DRV_CS_BT601_YUV_FULL ||
         in_cs == HI_DRV_CS_BT601_RGB_LIMITED || in_cs == HI_DRV_CS_BT601_RGB_FULL) &&
        (out_cs == HI_DRV_CS_BT2020_YUV_LIMITED || out_cs == HI_DRV_CS_BT2020_YUV_FULL ||
         out_cs == HI_DRV_CS_BT2020_RGB_LIMITED || out_cs == HI_DRV_CS_BT2020_RGB_FULL)) {
        imap_cfg->cacm.enable = HI_TRUE;
    }

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_GFXHDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_gfxhdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_gfxhdr_get_hdr2sdr_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                               drv_pq_internal_color_space out_cs,
                                               hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_FALSE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE; /* HDR2SDR场景下要关闭 */
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_GFXHDR_CM_POS1;    /* HDR2SDR场景下要关闭 */
    if ((in_cs == HI_DRV_CS_BT2020_YUV_LIMITED || in_cs == HI_DRV_CS_BT2020_YUV_FULL ||
         in_cs == HI_DRV_CS_BT2020_RGB_LIMITED || in_cs == HI_DRV_CS_BT2020_RGB_FULL) &&
        (out_cs == HI_DRV_CS_BT709_YUV_LIMITED || out_cs == HI_DRV_CS_BT709_YUV_FULL ||
         out_cs == HI_DRV_CS_BT709_RGB_LIMITED || out_cs == HI_DRV_CS_BT709_RGB_FULL)) {
        imap_cfg->cacm.enable = HI_TRUE;
    }
    if ((in_cs == HI_DRV_CS_BT2020_YUV_LIMITED || in_cs == HI_DRV_CS_BT2020_YUV_FULL ||
         in_cs == HI_DRV_CS_BT2020_RGB_LIMITED || in_cs == HI_DRV_CS_BT2020_RGB_FULL) &&
        (out_cs == HI_DRV_CS_BT601_YUV_LIMITED || out_cs == HI_DRV_CS_BT601_YUV_FULL ||
         out_cs == HI_DRV_CS_BT601_RGB_LIMITED || out_cs == HI_DRV_CS_BT601_RGB_FULL)) {
        imap_cfg->cacm.enable = HI_TRUE;
    }

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_GFXHDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_gfxhdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_gfxhdr_get_hdr2hdr_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                               drv_pq_internal_color_space out_cs,
                                               hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_FALSE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE; /* SDR_WCG 关闭 */
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_GFXHDR_CM_POS0;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_FALSE;
    imap_cfg->tmap_v1.tm_pos = PQ_GFXHDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_gfxhdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_gfxhdr_get_hdr2hlg_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                               drv_pq_internal_color_space out_cs,
                                               hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_FALSE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE; /* 默认关闭 */
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_GFXHDR_CM_POS0;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_GFXHDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_gfxhdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_gfxhdr_get_hlg2sdr_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                               drv_pq_internal_color_space out_cs,
                                               hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_FALSE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE;
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_GFXHDR_CM_POS1;
    if ((in_cs == HI_DRV_CS_BT2020_YUV_LIMITED || in_cs == HI_DRV_CS_BT2020_YUV_FULL ||
         in_cs == HI_DRV_CS_BT2020_RGB_LIMITED || in_cs == HI_DRV_CS_BT2020_RGB_FULL) &&
        (out_cs == HI_DRV_CS_BT709_YUV_LIMITED || out_cs == HI_DRV_CS_BT709_YUV_FULL ||
         out_cs == HI_DRV_CS_BT709_RGB_LIMITED || out_cs == HI_DRV_CS_BT709_RGB_FULL)) {
        imap_cfg->cacm.enable = HI_TRUE;
    }
    if ((in_cs == HI_DRV_CS_BT2020_YUV_LIMITED || in_cs == HI_DRV_CS_BT2020_YUV_FULL ||
         in_cs == HI_DRV_CS_BT2020_RGB_LIMITED || in_cs == HI_DRV_CS_BT2020_RGB_FULL) &&
        (out_cs == HI_DRV_CS_BT601_YUV_LIMITED || out_cs == HI_DRV_CS_BT601_YUV_FULL ||
         out_cs == HI_DRV_CS_BT601_RGB_LIMITED || out_cs == HI_DRV_CS_BT601_RGB_FULL)) {
        imap_cfg->cacm.enable = HI_TRUE;
    }

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_GFXHDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_gfxhdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_gfxhdr_get_hlg2hdr_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                               drv_pq_internal_color_space out_cs,
                                               hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_FALSE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE; /* 默认关闭 */
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_GFXHDR_CM_POS0;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_TRUE;
    imap_cfg->tmap_v1.tm_pos = PQ_GFXHDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_gfxhdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_gfxhdr_get_hlg2hlg_imap_base_cfg(drv_pq_internal_color_space in_cs,
                                               drv_pq_internal_color_space out_cs,
                                               hi_drv_pq_imap_cfg *imap_cfg)
{
    /* yuv2rgb */
    imap_cfg->yuv2rgb.enable = HI_FALSE;

    /* cacm */
    imap_cfg->cacm.enable = HI_FALSE; /* SDR_WCG 关闭 */
    imap_cfg->cacm.demo_en = HI_FALSE;
    imap_cfg->cacm.cm_pos = PQ_GFXHDR_CM_POS0;

    /* degamma */
    imap_cfg->degamma_en = HI_TRUE;

    /* rgb2lms */
    imap_cfg->rgb2lms.enable = HI_TRUE;

    /* tmap v1 */
    imap_cfg->tmap_v1.enable = HI_FALSE;
    imap_cfg->tmap_v1.tm_pos = PQ_GFXHDR_TM_POS0;

    /* gamma */
    imap_cfg->gamma_en = HI_TRUE;

    /* lms2ipt */
    imap_cfg->lms2ipt.enable = HI_TRUE;

    pq_hal_gfxhdr_get_imap_default_cfg(imap_cfg);
    return HI_SUCCESS;
}

static hi_void pq_hal_gfxhdr_get_tmap_default_cfg(pq_gfxhdr_tmap_cfg *tmap_cfg)
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

static hi_s32 pq_hal_gfxhdr_get_tmap_cfg(drv_pq_internal_color_space in_cs, drv_pq_internal_color_space out_cs,
                                         pq_gfxhdr_tmap_cfg *tmap_cfg)
{
    switch (g_gfxhdr_mode) {
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

    pq_hal_gfxhdr_get_tmap_default_cfg(tmap_cfg);
    return HI_SUCCESS;
}


hi_s32 pq_hal_get_gfx_hdr2sdr_cfg(drv_pq_internal_color_space in_cs,
                                  drv_pq_internal_color_space out_cs,
                                  pq_hal_gfxhdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
         /* 1. get imap base cfg */
        pq_hal_gfxhdr_get_hdr2sdr_imap_base_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 2. yuv2rgb */
        pq_hal_gfxhdr_get_yuv2rgb_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 3. cacm */
        pq_hal_gfxhdr_get_imap_cacm_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 4. degamma */
        g_gfxhdr_imap_degamma = g_gfxhdr_degmm_lut_pq;

        /* 5. rgb2lms */
        pq_hal_gfxhdr_get_rgb2lms_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 6. tone_mapping tmap v1 */
        pq_hal_gfxhdr_get_tmap_v1_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 7. gamma */
        g_gfxhdr_imap_gamma_lut_x = g_gfxhdr_im_gmm_lutx;
        g_gfxhdr_imap_gamma_lut_a = g_gfxhdr_im_gmm_luta_gmm22;
        g_gfxhdr_imap_gamma_lut_b = g_gfxhdr_im_gmm_lutb_gmm22;

        /* 8. lms2ipt */
        pq_hal_gfxhdr_get_lms2ipt_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_gfxhdr_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_gfxhdr_tmap_tm_lut_i = g_gfxhdr_tm_lut_i_bypass;
        g_gfxhdr_tmap_tm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_i = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_smc_lut = g_gfxhdr_sm_lut_bypass;
    }

    pq_hal_gfxhdr_get_hdr_coef(hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_gfx_hdr2hdr_cfg(drv_pq_internal_color_space in_cs,
                                  drv_pq_internal_color_space out_cs,
                                  pq_hal_gfxhdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    if (in_cs == out_cs) {
        hdr_cfg->imap_cfg.enable = HI_FALSE;
        hdr_cfg->tmap_cfg.enable = HI_FALSE;
        return HI_SUCCESS;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
         /* 1. get imap base cfg */
        pq_hal_gfxhdr_get_hdr2hdr_imap_base_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 2. yuv2rgb */
        pq_hal_gfxhdr_get_yuv2rgb_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 3. cacm */
        pq_hal_gfxhdr_get_imap_cacm_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 4. degamma */
        g_gfxhdr_imap_degamma = g_gfxhdr_degmm_lut_pq;

        /* 5. rgb2lms */
        pq_hal_gfxhdr_get_rgb2lms_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 6. tone_mapping tmap v1 */
        pq_hal_gfxhdr_get_tmap_v1_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 7. gamma */
        g_gfxhdr_imap_gamma_lut_x = g_gfxhdr_im_gmm_lutx;
        g_gfxhdr_imap_gamma_lut_a = g_gfxhdr_im_gmm_luta_pq;
        g_gfxhdr_imap_gamma_lut_b = g_gfxhdr_im_gmm_lutb_pq;

        /* 8. lms2ipt */
        pq_hal_gfxhdr_get_lms2ipt_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_gfxhdr_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_gfxhdr_tmap_tm_lut_i = g_gfxhdr_tm_lut_i_bypass;
        g_gfxhdr_tmap_tm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_i = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_smc_lut = g_gfxhdr_sm_lut_bypass;
    }

    pq_hal_gfxhdr_get_hdr_coef(hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_gfx_hdr2hlg_cfg(drv_pq_internal_color_space in_cs,
                                  drv_pq_internal_color_space out_cs,
                                  pq_hal_gfxhdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
         /* 1. get imap base cfg */
        pq_hal_gfxhdr_get_hdr2hlg_imap_base_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 2. yuv2rgb */
        pq_hal_gfxhdr_get_yuv2rgb_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 3. cacm */
        pq_hal_gfxhdr_get_imap_cacm_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 4. degamma */
        g_gfxhdr_imap_degamma = g_gfxhdr_degmm_lut_pq;

        /* 5. rgb2lms */
        pq_hal_gfxhdr_get_rgb2lms_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 6. tone_mapping tmap v1 */
        pq_hal_gfxhdr_get_tmap_v1_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 7. gamma */
        g_gfxhdr_imap_gamma_lut_x = g_gfxhdr_im_gmm_lutx;
        g_gfxhdr_imap_gamma_lut_a = g_gfxhdr_im_gmm_luta_hlg;
        g_gfxhdr_imap_gamma_lut_b = g_gfxhdr_im_gmm_lutb_hlg;

        /* 8. lms2ipt */
        pq_hal_gfxhdr_get_lms2ipt_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_gfxhdr_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_gfxhdr_tmap_tm_lut_i = g_gfxhdr_tm_lut_i_bypass;
        g_gfxhdr_tmap_tm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_i = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_smc_lut = g_gfxhdr_sm_lut_bypass;
    }

    pq_hal_gfxhdr_get_hdr_coef(hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_gfx_hlg2sdr_cfg(drv_pq_internal_color_space in_cs,
                                  drv_pq_internal_color_space out_cs,
                                  pq_hal_gfxhdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
         /* 1. get imap base cfg */
        pq_hal_gfxhdr_get_hlg2sdr_imap_base_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 2. yuv2rgb */
        pq_hal_gfxhdr_get_yuv2rgb_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 3. cacm */
        pq_hal_gfxhdr_get_imap_cacm_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 4. degamma */
        g_gfxhdr_imap_degamma = g_gfxhdr_degmm_lut_hlg_10000;

        /* 5. rgb2lms */
        pq_hal_gfxhdr_get_rgb2lms_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 6. tone_mapping tmap v1 */
        pq_hal_gfxhdr_get_tmap_v1_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 7. gamma */
        g_gfxhdr_imap_gamma_lut_x = g_gfxhdr_im_gmm_lutx;
        g_gfxhdr_imap_gamma_lut_a = g_gfxhdr_im_gmm_luta_gmm22;
        g_gfxhdr_imap_gamma_lut_b = g_gfxhdr_im_gmm_lutb_gmm22;

        /* 8. lms2ipt */
        pq_hal_gfxhdr_get_lms2ipt_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_gfxhdr_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_gfxhdr_tmap_tm_lut_i = g_gfxhdr_tm_lut_i_bypass;
        g_gfxhdr_tmap_tm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_i = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_smc_lut = g_gfxhdr_sm_lut_bypass;
    }

    pq_hal_gfxhdr_get_hdr_coef(hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_gfx_hlg2hdr_cfg(drv_pq_internal_color_space in_cs,
                                  drv_pq_internal_color_space out_cs,
                                  pq_hal_gfxhdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
         /* 1. get imap base cfg */
        pq_hal_gfxhdr_get_hlg2hdr_imap_base_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 2. yuv2rgb */
        pq_hal_gfxhdr_get_yuv2rgb_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 3. cacm */
        pq_hal_gfxhdr_get_imap_cacm_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 4. degamma */
        g_gfxhdr_imap_degamma = g_gfxhdr_degmm_lut_hlg_10000;

        /* 5. rgb2lms */
        pq_hal_gfxhdr_get_rgb2lms_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 6. tone_mapping tmap v1 */
        pq_hal_gfxhdr_get_tmap_v1_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 7. gamma */
        g_gfxhdr_imap_gamma_lut_x = g_gfxhdr_im_gmm_lutx;
        g_gfxhdr_imap_gamma_lut_a = g_gfxhdr_im_gmm_luta_pq;
        g_gfxhdr_imap_gamma_lut_b = g_gfxhdr_im_gmm_lutb_pq;

        /* 8. lms2ipt */
        pq_hal_gfxhdr_get_lms2ipt_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_gfxhdr_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_gfxhdr_tmap_tm_lut_i = g_gfxhdr_tm_lut_i_bypass;
        g_gfxhdr_tmap_tm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_i = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_smc_lut = g_gfxhdr_sm_lut_bypass;
    }

    pq_hal_gfxhdr_get_hdr_coef(hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_gfx_hlg2hlg_cfg(drv_pq_internal_color_space in_cs,
                                  drv_pq_internal_color_space out_cs,
                                  pq_hal_gfxhdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    if (in_cs == out_cs) {
        hdr_cfg->imap_cfg.enable = HI_FALSE;
        hdr_cfg->tmap_cfg.enable = HI_FALSE;
        return HI_SUCCESS;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
         /* 1. get imap base cfg */
        pq_hal_gfxhdr_get_hlg2hlg_imap_base_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 2. yuv2rgb */
        pq_hal_gfxhdr_get_yuv2rgb_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 3. cacm */
        pq_hal_gfxhdr_get_imap_cacm_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 4. degamma */
        g_gfxhdr_imap_degamma = g_gfxhdr_degmm_lut_hlg_10000;

        /* 5. rgb2lms */
        pq_hal_gfxhdr_get_rgb2lms_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 6. tone_mapping tmap v1 */
        pq_hal_gfxhdr_get_tmap_v1_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 7. gamma */
        g_gfxhdr_imap_gamma_lut_x = g_gfxhdr_im_gmm_lutx;
        g_gfxhdr_imap_gamma_lut_a = g_gfxhdr_im_gmm_luta_hlg;
        g_gfxhdr_imap_gamma_lut_b = g_gfxhdr_im_gmm_lutb_hlg;

        /* 8. lms2ipt */
        pq_hal_gfxhdr_get_lms2ipt_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_gfxhdr_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_gfxhdr_tmap_tm_lut_i = g_gfxhdr_tm_lut_i_bypass;
        g_gfxhdr_tmap_tm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_i = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_smc_lut = g_gfxhdr_sm_lut_bypass;
    }

    pq_hal_gfxhdr_get_hdr_coef(hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_gfx_sdr2hdr_cfg(drv_pq_internal_color_space in_cs,
                                  drv_pq_internal_color_space out_cs,
                                  pq_hal_gfxhdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        /* 1. get imap base cfg */
        pq_hal_gfxhdr_get_sdr2hdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 2. yuv2rgb */
        pq_hal_gfxhdr_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 3. cacm  */
        pq_hal_gfxhdr_get_imap_cacm_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 4. degamma */
        g_gfxhdr_imap_degamma = g_gfxhdr_degmm_lut_gmm24_10000;

        /* 5. rgb2lms */
        pq_hal_gfxhdr_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 6. tone_mapping tmap v1 */
        pq_hal_gfxhdr_get_tmap_v1_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 7. gamma */
        g_gfxhdr_imap_gamma_lut_x = g_gfxhdr_im_gmm_lutx;
        g_gfxhdr_imap_gamma_lut_a = g_gfxhdr_im_gmm_luta_pq;
        g_gfxhdr_imap_gamma_lut_b = g_gfxhdr_im_gmm_lutb_pq;

        /* 8. lms2ipt */
        pq_hal_gfxhdr_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_gfxhdr_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_gfxhdr_tmap_tm_lut_i = g_gfxhdr_tm_lut_i_bypass;
        g_gfxhdr_tmap_tm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_i = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_smc_lut = g_gfxhdr_sm_lut_bypass;
    }

    pq_hal_gfxhdr_get_hdr_coef(hdr_cfg);
    return HI_SUCCESS;
}

hi_s32 pq_hal_get_gfx_sdr2hlg_cfg(drv_pq_internal_color_space in_cs,
                                  drv_pq_internal_color_space out_cs,
                                  pq_hal_gfxhdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
         /* 1. get imap base cfg */
        pq_hal_gfxhdr_get_sdr2hlg_imap_base_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 2. yuv2rgb */
        pq_hal_gfxhdr_get_yuv2rgb_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 3. cacm */
        pq_hal_gfxhdr_get_imap_cacm_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 4. degamma */
        g_gfxhdr_imap_degamma = g_gfxhdr_degmm_lut_gmm24_10000;

        /* 5. rgb2lms */
        pq_hal_gfxhdr_get_rgb2lms_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 6. tone_mapping tmap v1 */
        pq_hal_gfxhdr_get_tmap_v1_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 7. gamma */
        g_gfxhdr_imap_gamma_lut_x = g_gfxhdr_im_gmm_lutx;
        g_gfxhdr_imap_gamma_lut_a = g_gfxhdr_im_gmm_luta_hlg;
        g_gfxhdr_imap_gamma_lut_b = g_gfxhdr_im_gmm_lutb_hlg;

        /* 8. lms2ipt */
        pq_hal_gfxhdr_get_lms2ipt_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_gfxhdr_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_gfxhdr_tmap_tm_lut_i = g_gfxhdr_tm_lut_i_bypass;
        g_gfxhdr_tmap_tm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_i = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_smc_lut = g_gfxhdr_sm_lut_bypass;
    }

    pq_hal_gfxhdr_get_hdr_coef(hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_gfx_sdr2sdr_cfg(drv_pq_internal_color_space in_cs,
                                  drv_pq_internal_color_space out_cs,
                                  pq_hal_gfxhdr_cfg *hdr_cfg)
{
    hdr_cfg->imap_cfg.enable = HI_TRUE;
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    if (in_cs == out_cs) {
        hdr_cfg->imap_cfg.enable = HI_FALSE;
        hdr_cfg->tmap_cfg.enable = HI_FALSE;
        return HI_SUCCESS;
    } else if ((g_gfxhdr_mode == HI_PQ_HDR_MODE_SDR_TO_SDR)
               && (in_cs != HI_DRV_CS_BT2020_YUV_LIMITED) && (in_cs != HI_DRV_CS_BT2020_YUV_FULL)
               && (in_cs != HI_DRV_CS_BT2020_RGB_LIMITED) && (in_cs != HI_DRV_CS_BT2020_RGB_FULL)
               && (out_cs != HI_DRV_CS_BT2020_YUV_LIMITED) && (out_cs != HI_DRV_CS_BT2020_YUV_FULL)
               && (out_cs != HI_DRV_CS_BT2020_RGB_LIMITED) && (out_cs != HI_DRV_CS_BT2020_RGB_FULL)) {
        hdr_cfg->imap_cfg.enable = HI_FALSE;
        hdr_cfg->tmap_cfg.enable = HI_FALSE;
        return HI_SUCCESS;
    }

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
        /* 1. get imap base cfg */
        pq_hal_gfxhdr_get_sdr2sdr_imap_base_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 2. yuv2rgb */
        pq_hal_gfxhdr_get_yuv2rgb_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 3. cacm  */
        pq_hal_gfxhdr_get_imap_cacm_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 4. degamma */
        g_gfxhdr_imap_degamma = g_gfxhdr_degmm_lut_gmm24_10000;

        /* 5. rgb2lms */
        pq_hal_gfxhdr_get_rgb2lms_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 6. tone_mapping tmap v1 */
        pq_hal_gfxhdr_get_tmap_v1_cfg(in_cs, out_cs, &hdr_cfg->imap_cfg);

        /* 7. gamma */
        g_gfxhdr_imap_gamma_lut_x = g_gfxhdr_im_gmm_lutx;
        g_gfxhdr_imap_gamma_lut_a = g_gfxhdr_im_gmm_luta_gmm24;
        g_gfxhdr_imap_gamma_lut_b = g_gfxhdr_im_gmm_lutb_gmm24;

        /* 8. lms2ipt */
        pq_hal_gfxhdr_get_lms2ipt_matrix(in_cs, out_cs, &hdr_cfg->imap_cfg);
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_gfxhdr_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_gfxhdr_tmap_tm_lut_i = g_gfxhdr_tm_lut_i_bypass;
        g_gfxhdr_tmap_tm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_i = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_smc_lut = g_gfxhdr_sm_lut_bypass;
    }

    pq_hal_gfxhdr_get_hdr_coef(hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_gfx_hdr10plus2sdr_cfg(drv_pq_internal_color_space in_cs,
                                        drv_pq_internal_color_space out_cs,
                                        pq_hal_gfxhdr_cfg *hdr_cfg)
{
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
         /* 1. get imap base cfg */
        pq_hal_gfxhdr_get_sdr2sdr_imap_base_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 2. yuv2rgb */
        pq_hal_gfxhdr_get_yuv2rgb_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 3. cacm */
        pq_hal_gfxhdr_get_imap_cacm_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 4. degamma */
        g_gfxhdr_imap_degamma = g_gfxhdr_degmm_lut_gmm24_10000;

        /* 5. rgb2lms */
        pq_hal_gfxhdr_get_rgb2lms_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 6. tone_mapping tmap v1 */
        pq_hal_gfxhdr_get_tmap_v1_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 7. gamma */
        g_gfxhdr_imap_gamma_lut_x = g_gfxhdr_im_gmm_lutx;
        g_gfxhdr_imap_gamma_lut_a = g_gfxhdr_im_gmm_luta_gmm24;
        g_gfxhdr_imap_gamma_lut_b = g_gfxhdr_im_gmm_lutb_gmm24;

        /* 8. lms2ipt */
        pq_hal_gfxhdr_get_lms2ipt_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_gfxhdr_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_gfxhdr_tmap_tm_lut_i = g_gfxhdr_tm_lut_i_bypass;
        g_gfxhdr_tmap_tm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_i = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_smc_lut = g_gfxhdr_sm_lut_bypass;
    }

    pq_hal_gfxhdr_get_hdr_coef(hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_gfx_hdr10plus2hdr10_cfg(drv_pq_internal_color_space in_cs,
                                          drv_pq_internal_color_space out_cs,
                                          pq_hal_gfxhdr_cfg *hdr_cfg)
{
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
         /* 1. get imap base cfg */
        pq_hal_gfxhdr_get_sdr2sdr_imap_base_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 2. yuv2rgb */
        pq_hal_gfxhdr_get_yuv2rgb_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 3. cacm */
        pq_hal_gfxhdr_get_imap_cacm_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 4. degamma */
        g_gfxhdr_imap_degamma = g_gfxhdr_degmm_lut_gmm24_10000;

        /* 5. rgb2lms */
        pq_hal_gfxhdr_get_rgb2lms_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 6. tone_mapping tmap v1 */
        pq_hal_gfxhdr_get_tmap_v1_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 7. gamma */
        g_gfxhdr_imap_gamma_lut_x = g_gfxhdr_im_gmm_lutx;
        g_gfxhdr_imap_gamma_lut_a = g_gfxhdr_im_gmm_luta_gmm24;
        g_gfxhdr_imap_gamma_lut_b = g_gfxhdr_im_gmm_lutb_gmm24;

        /* 8. lms2ipt */
        pq_hal_gfxhdr_get_lms2ipt_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_gfxhdr_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_gfxhdr_tmap_tm_lut_i = g_gfxhdr_tm_lut_i_bypass;
        g_gfxhdr_tmap_tm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_i = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_smc_lut = g_gfxhdr_sm_lut_bypass;
    }

    pq_hal_gfxhdr_get_hdr_coef(hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_gfx_hdr10plus2hlg_cfg(drv_pq_internal_color_space in_cs,
                                        drv_pq_internal_color_space out_cs,
                                        pq_hal_gfxhdr_cfg *hdr_cfg)
{
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
         /* 1. get imap base cfg */
        pq_hal_gfxhdr_get_sdr2sdr_imap_base_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 2. yuv2rgb */
        pq_hal_gfxhdr_get_yuv2rgb_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 3. cacm */
        pq_hal_gfxhdr_get_imap_cacm_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 4. degamma */
        g_gfxhdr_imap_degamma = g_gfxhdr_degmm_lut_gmm24_10000;

        /* 5. rgb2lms */
        pq_hal_gfxhdr_get_rgb2lms_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 6. tone_mapping tmap v1 */
        pq_hal_gfxhdr_get_tmap_v1_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 7. gamma */
        g_gfxhdr_imap_gamma_lut_x = g_gfxhdr_im_gmm_lutx;
        g_gfxhdr_imap_gamma_lut_a = g_gfxhdr_im_gmm_luta_gmm24;
        g_gfxhdr_imap_gamma_lut_b = g_gfxhdr_im_gmm_lutb_gmm24;

        /* 8. lms2ipt */
        pq_hal_gfxhdr_get_lms2ipt_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_gfxhdr_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_gfxhdr_tmap_tm_lut_i = g_gfxhdr_tm_lut_i_bypass;
        g_gfxhdr_tmap_tm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_i = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_smc_lut = g_gfxhdr_sm_lut_bypass;
    }

    pq_hal_gfxhdr_get_hdr_coef(hdr_cfg);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_gfx_hdr10plus2hdr10plus_cfg(drv_pq_internal_color_space in_cs,
                                              drv_pq_internal_color_space out_cs,
                                              pq_hal_gfxhdr_cfg *hdr_cfg)
{
    hdr_cfg->tmap_cfg.enable = HI_FALSE;
    hdr_cfg->imap_cfg.enable = HI_TRUE;

    if (hdr_cfg->imap_cfg.enable == HI_TRUE) {
         /* 1. get imap base cfg */
        pq_hal_gfxhdr_get_sdr2sdr_imap_base_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 2. yuv2rgb */
        pq_hal_gfxhdr_get_yuv2rgb_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 3. cacm */
        pq_hal_gfxhdr_get_imap_cacm_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 4. degamma */
        g_gfxhdr_imap_degamma = g_gfxhdr_degmm_lut_gmm24_10000;

        /* 5. rgb2lms */
        pq_hal_gfxhdr_get_rgb2lms_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 6. tone_mapping tmap v1 */
        pq_hal_gfxhdr_get_tmap_v1_cfg(in_cs, out_cs, &(hdr_cfg->imap_cfg));

        /* 7. gamma */
        g_gfxhdr_imap_gamma_lut_x = g_gfxhdr_im_gmm_lutx;
        g_gfxhdr_imap_gamma_lut_a = g_gfxhdr_im_gmm_luta_gmm24;
        g_gfxhdr_imap_gamma_lut_b = g_gfxhdr_im_gmm_lutb_gmm24;

        /* 8. lms2ipt */
        pq_hal_gfxhdr_get_lms2ipt_matrix(in_cs, out_cs, &(hdr_cfg->imap_cfg));
    }

    if (hdr_cfg->tmap_cfg.enable == HI_TRUE) {
        pq_hal_gfxhdr_get_tmap_cfg(in_cs, out_cs, &hdr_cfg->tmap_cfg);
        g_gfxhdr_tmap_tm_lut_i = g_gfxhdr_tm_lut_i_bypass;
        g_gfxhdr_tmap_tm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_i = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_sm_lut_s = g_gfxhdr_sm_lut_bypass;
        g_gfxhdr_tmap_smc_lut = g_gfxhdr_sm_lut_bypass;
    }

    pq_hal_gfxhdr_get_hdr_coef(hdr_cfg);

    return HI_SUCCESS;
}

hi_void pq_gfxhdr_set_imap_yuv2rgb(vdp_regs_type *vdp_reg, hi_u32 offset, hi_drv_pq_yuv2rgb_cfg *yuv2rgb)
{
    pq_hihdr_v2_setimyuv2rgben(vdp_reg, offset, yuv2rgb->enable);
    if (yuv2rgb->enable == HI_FALSE) {
        return;
    }

    /* yuv2rgb */
    pq_hihdr_v2_setimyuv2rgbmode(vdp_reg, offset, yuv2rgb->mode);
    pq_hihdr_v2_setimv1y2ren(vdp_reg, offset, yuv2rgb->v1_enable);
    pq_hihdr_v2_setimv0y2ren(vdp_reg, offset, yuv2rgb->v0_enable);
    pq_hihdr_v2_setimapm33yuv2rgb00(vdp_reg, offset, yuv2rgb->coef[0][0]);
    pq_hihdr_v2_setimapm33yuv2rgb01(vdp_reg, offset, yuv2rgb->coef[0][1]);
    pq_hihdr_v2_setimapm33yuv2rgb02(vdp_reg, offset, yuv2rgb->coef[0][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb10(vdp_reg, offset, yuv2rgb->coef[1][0]);
    pq_hihdr_v2_setimapm33yuv2rgb11(vdp_reg, offset, yuv2rgb->coef[1][1]);
    pq_hihdr_v2_setimapm33yuv2rgb12(vdp_reg, offset, yuv2rgb->coef[1][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb20(vdp_reg, offset, yuv2rgb->coef[2][0]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb21(vdp_reg, offset, yuv2rgb->coef[2][1]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb22(vdp_reg, offset, yuv2rgb->coef[2][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb200(vdp_reg, offset, yuv2rgb->coef2[0][0]);
    pq_hihdr_v2_setimapm33yuv2rgb201(vdp_reg, offset, yuv2rgb->coef2[0][1]);
    pq_hihdr_v2_setimapm33yuv2rgb202(vdp_reg, offset, yuv2rgb->coef2[0][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb210(vdp_reg, offset, yuv2rgb->coef2[1][0]);
    pq_hihdr_v2_setimapm33yuv2rgb211(vdp_reg, offset, yuv2rgb->coef2[1][1]);
    pq_hihdr_v2_setimapm33yuv2rgb212(vdp_reg, offset, yuv2rgb->coef2[1][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb220(vdp_reg, offset, yuv2rgb->coef2[2][0]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb221(vdp_reg, offset, yuv2rgb->coef2[2][1]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb222(vdp_reg, offset, yuv2rgb->coef2[2][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgbscale2p(vdp_reg, offset, yuv2rgb->scale2p);
    pq_hihdr_v2_setimv3yuv2rgbdcin0(vdp_reg, offset, yuv2rgb->dc_in[0]);
    pq_hihdr_v2_setimv3yuv2rgbdcin1(vdp_reg, offset, yuv2rgb->dc_in[1]);
    pq_hihdr_v2_setimv3yuv2rgbdcin2(vdp_reg, offset, yuv2rgb->dc_in[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapv3yuv2rgboffinrgb0(vdp_reg, offset, yuv2rgb->offinrgb[0]);
    pq_hihdr_v2_setimapv3yuv2rgboffinrgb1(vdp_reg, offset, yuv2rgb->offinrgb[1]);
    pq_hihdr_v2_setimapv3yuv2rgboffinrgb2(vdp_reg, offset, yuv2rgb->offinrgb[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimv3yuv2rgb2dcin0(vdp_reg, offset, yuv2rgb->dc_in2[0]);
    pq_hihdr_v2_setimv3yuv2rgb2dcin1(vdp_reg, offset, yuv2rgb->dc_in2[1]);
    pq_hihdr_v2_setimv3yuv2rgb2dcin2(vdp_reg, offset, yuv2rgb->dc_in2[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapv3yuv2rgb2offinrgb0(vdp_reg, offset, yuv2rgb->offinrgb2[0]);
    pq_hihdr_v2_setimapv3yuv2rgb2offinrgb1(vdp_reg, offset, yuv2rgb->offinrgb2[1]);
    pq_hihdr_v2_setimapv3yuv2rgb2offinrgb2(vdp_reg, offset, yuv2rgb->offinrgb2[2]); /* 2 :a number */
    pq_hihdr_v2_setimyuv2rgbclipmin(vdp_reg, offset, yuv2rgb->clip_min);
    pq_hihdr_v2_setimyuv2rgbclipmax(vdp_reg, offset, yuv2rgb->clip_max);
    pq_hihdr_v2_setimyuv2rgbthrr(vdp_reg, offset, yuv2rgb->thr_r);
    pq_hihdr_v2_setimyuv2rgbthrb(vdp_reg, offset, yuv2rgb->thr_b);
}

hi_void pq_gfxhdr_set_imap_rgb2lms(vdp_regs_type *vdp_reg, hi_u32 offset, hi_drv_pq_rgb2lms_cfg *rgb2lms)
{
    /* rgb2lms */
    pq_hihdr_v2_setimrgb2lmsen(vdp_reg, offset, rgb2lms->enable);
    pq_hihdr_v2_setimapm33rgb2lms00(vdp_reg, offset, rgb2lms->coef[0][0]);
    pq_hihdr_v2_setimapm33rgb2lms01(vdp_reg, offset, rgb2lms->coef[0][1]);
    pq_hihdr_v2_setimapm33rgb2lms02(vdp_reg, offset, rgb2lms->coef[0][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33rgb2lms10(vdp_reg, offset, rgb2lms->coef[1][0]);
    pq_hihdr_v2_setimapm33rgb2lms11(vdp_reg, offset, rgb2lms->coef[1][1]);
    pq_hihdr_v2_setimapm33rgb2lms12(vdp_reg, offset, rgb2lms->coef[1][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33rgb2lms20(vdp_reg, offset, rgb2lms->coef[2][0]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33rgb2lms21(vdp_reg, offset, rgb2lms->coef[2][1]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33rgb2lms22(vdp_reg, offset, rgb2lms->coef[2][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33rgb2lmsscale2p(vdp_reg, offset, rgb2lms->scale2p);
    pq_hihdr_v2_setimrgb2lmsclipmin(vdp_reg, offset, rgb2lms->clip_min);
    pq_hihdr_v2_setimrgb2lmsclipmax(vdp_reg, offset, rgb2lms->clip_max);
    pq_hihdr_v2_setimv3rgb2lmsdcin0(vdp_reg, offset, rgb2lms->dc_in[0]);
    pq_hihdr_v2_setimv3rgb2lmsdcin1(vdp_reg, offset, rgb2lms->dc_in[1]);
    pq_hihdr_v2_setimv3rgb2lmsdcin2(vdp_reg, offset, rgb2lms->dc_in[2]);  /* 2 is reg index */
}

hi_void pq_gfxhdr_set_imap_lms2ipt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_drv_pq_lms2ipt_cfg *lms2ipt)
{
    /* lms2ipt */
    pq_hihdr_v2_setimlms2ipten(vdp_reg, offset, lms2ipt->enable);
    pq_hihdr_v2_setimapm33lms2ipt00(vdp_reg, offset, lms2ipt->coef[0][0]);
    pq_hihdr_v2_setimapm33lms2ipt01(vdp_reg, offset, lms2ipt->coef[0][1]);
    pq_hihdr_v2_setimapm33lms2ipt02(vdp_reg, offset, lms2ipt->coef[0][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33lms2ipt10(vdp_reg, offset, lms2ipt->coef[1][0]);
    pq_hihdr_v2_setimapm33lms2ipt11(vdp_reg, offset, lms2ipt->coef[1][1]);
    pq_hihdr_v2_setimapm33lms2ipt12(vdp_reg, offset, lms2ipt->coef[1][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33lms2ipt20(vdp_reg, offset, lms2ipt->coef[2][0]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33lms2ipt21(vdp_reg, offset, lms2ipt->coef[2][1]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33lms2ipt22(vdp_reg, offset, lms2ipt->coef[2][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33lms2iptscale2p(vdp_reg, offset, lms2ipt->scale2p);
    pq_hihdr_v2_setimlms2iptclipminy(vdp_reg, offset, lms2ipt->clip_min_y);
    pq_hihdr_v2_setimlms2iptclipmaxy(vdp_reg, offset, lms2ipt->clip_max_y);
    pq_hihdr_v2_setimlms2iptclipminc(vdp_reg, offset, lms2ipt->clip_min_c);
    pq_hihdr_v2_setimlms2iptclipmaxc(vdp_reg, offset, lms2ipt->clip_max_c);
    pq_hihdr_v2_setimv3lms2iptdcout0(vdp_reg, offset, lms2ipt->dc_out[0]);
    pq_hihdr_v2_setimv3lms2iptdcout1(vdp_reg, offset, lms2ipt->dc_out[1]);
    pq_hihdr_v2_setimv3lms2iptdcout2(vdp_reg, offset, lms2ipt->dc_out[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimv3lms2iptdcin0(vdp_reg, offset, lms2ipt->dc_in[0]);
    pq_hihdr_v2_setimv3lms2iptdcin1(vdp_reg, offset, lms2ipt->dc_in[1]);
    pq_hihdr_v2_setimv3lms2iptdcin2(vdp_reg, offset, lms2ipt->dc_in[2]);  /* 2 is reg index */
}

hi_void pq_gfxhdr_set_imap_tmap_v1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_drv_pq_tmap_v1_cfg *tmap_v1)
{
    pq_hihdr_v2_setimtmv1en(vdp_reg, offset, tmap_v1->enable);
    if (tmap_v1->enable == HI_FALSE) {
        return;
    }

    /* tmap_v1 */
    pq_hihdr_v2_setimtmpos(vdp_reg, offset, tmap_v1->tm_pos);
    pq_hihdr_v2_setimtmscalemixalpha(vdp_reg, offset, tmap_v1->scale_mix_alpha);
    pq_hihdr_v2_setimtmmixalpha(vdp_reg, offset, tmap_v1->mix_alpha);
    pq_hihdr_v2_setimtmxstep3(vdp_reg, offset, tmap_v1->step[3]);  /* 3 is reg index */
    pq_hihdr_v2_setimtmxstep2(vdp_reg, offset, tmap_v1->step[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimtmxstep1(vdp_reg, offset, tmap_v1->step[1]);
    pq_hihdr_v2_setimtmxstep0(vdp_reg, offset, tmap_v1->step[0]);
    pq_hihdr_v2_setimtmxstep7(vdp_reg, offset, tmap_v1->step[7]);  /* 7 is reg index */
    pq_hihdr_v2_setimtmxstep6(vdp_reg, offset, tmap_v1->step[6]);  /* 6 is reg index */
    pq_hihdr_v2_setimtmxstep5(vdp_reg, offset, tmap_v1->step[5]);  /* 5 is reg index */
    pq_hihdr_v2_setimtmxstep4(vdp_reg, offset, tmap_v1->step[4]);  /* 4 is reg index */
    pq_hihdr_v2_setimtmxpos0(vdp_reg, offset, tmap_v1->pos[0]);
    pq_hihdr_v2_setimtmxpos1(vdp_reg, offset, tmap_v1->pos[1]);
    pq_hihdr_v2_setimtmxpos2(vdp_reg, offset, tmap_v1->pos[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimtmxpos3(vdp_reg, offset, tmap_v1->pos[3]);  /* 3 is reg index */
    pq_hihdr_v2_setimtmxpos4(vdp_reg, offset, tmap_v1->pos[4]);  /*  4 is reg index */
    pq_hihdr_v2_setimtmxpos5(vdp_reg, offset, tmap_v1->pos[5]);  /*  5 is reg index */
    pq_hihdr_v2_setimtmxpos6(vdp_reg, offset, tmap_v1->pos[6]);  /*  6 is reg index */
    pq_hihdr_v2_setimtmxpos7(vdp_reg, offset, tmap_v1->pos[7]);  /*  7 is reg index */
    pq_hihdr_v2_setimtmxnum3(vdp_reg, offset, tmap_v1->num[3]);  /* 3 is reg index */
    pq_hihdr_v2_setimtmxnum2(vdp_reg, offset, tmap_v1->num[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimtmxnum1(vdp_reg, offset, tmap_v1->num[1]);
    pq_hihdr_v2_setimtmxnum0(vdp_reg, offset, tmap_v1->num[0]);
    pq_hihdr_v2_setimtmxnum7(vdp_reg, offset, tmap_v1->num[7]);  /*  7 is reg index */
    pq_hihdr_v2_setimtmxnum6(vdp_reg, offset, tmap_v1->num[6]);  /*  6 is reg index */
    pq_hihdr_v2_setimtmxnum5(vdp_reg, offset, tmap_v1->num[5]);  /*  5 is reg index */
    pq_hihdr_v2_setimtmxnum4(vdp_reg, offset, tmap_v1->num[4]);  /*  4 is reg index */
    pq_hihdr_v2_setimtmm3lumcal0(vdp_reg, offset, tmap_v1->lum_cal[0]);
    pq_hihdr_v2_setimtmm3lumcal1(vdp_reg, offset, tmap_v1->lum_cal[1]);
    pq_hihdr_v2_setimtmm3lumcal2(vdp_reg, offset, tmap_v1->lum_cal[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimtmscalelumcal(vdp_reg, offset, tmap_v1->scale_lum_cal);
    pq_hihdr_v2_setimtmscalecoef(vdp_reg, offset, tmap_v1->scale_coef);
    pq_hihdr_v2_setimtmclipmin(vdp_reg, offset, tmap_v1->clip_min);
    pq_hihdr_v2_setimtmclipmax(vdp_reg, offset, tmap_v1->clip_max);
    pq_hihdr_v2_setimtmdcout0(vdp_reg, offset, tmap_v1->dc_out[0]);
    pq_hihdr_v2_setimtmdcout1(vdp_reg, offset, tmap_v1->dc_out[1]);
    pq_hihdr_v2_setimtmdcout2(vdp_reg, offset, tmap_v1->dc_out[2]);  /* 2 is reg index */
}

hi_void pq_gfxhdr_set_imap_cacm(vdp_regs_type *vdp_reg, hi_u32 offset, hi_drv_pq_cacm_cfg *cacm)
{
    pq_hihdr_v2_setimcmen(vdp_reg, offset, cacm->enable);
    if (cacm->enable == HI_FALSE) {
        return;
    }

    /* CACM */
    pq_hihdr_v2_setimcmpos(vdp_reg, offset, cacm->cm_pos);
    pq_hihdr_v2_setimcmdemopos(vdp_reg, offset, cacm->demo_pos);
    pq_hihdr_v2_setimcmdemomode(vdp_reg, offset, cacm->demo_mode);
    pq_hihdr_v2_setimcmdemoen(vdp_reg, offset, cacm->demo_en);
    pq_hihdr_v2_setimcmbitdepthoutmode(vdp_reg, offset, cacm->bitdepth_out_mode);
    pq_hihdr_v2_setimcmbitdepthinmode(vdp_reg, offset, cacm->bitdepth_in_mode);
    pq_hihdr_v2_setimcmckgten(vdp_reg, offset, HI_TRUE);
}

hi_void pq_hdr_set_imap_mode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_drv_pq_imap_cfg *imap_cfg)
{
    pq_hihdr_v2_setimen(vdp_reg, offset, imap_cfg->enable);
    if (imap_cfg->enable == HI_FALSE) {
        return;
    }
    pq_hihdr_v2_setimladjen(vdp_reg, offset, imap_cfg->ladj_en);
    pq_hihdr_v2_setimgammaen(vdp_reg, offset, imap_cfg->gamma_en);
    pq_hihdr_v2_setimdegammaen(vdp_reg, offset, imap_cfg->degamma_en);
    pq_hihdr_v2_setimnormen(vdp_reg, offset, imap_cfg->norm_en);
    pq_hihdr_v2_setimdemolumaen(vdp_reg, offset, imap_cfg->demo_luma_en);
    pq_hihdr_v2_setimv1hdren(vdp_reg, offset, imap_cfg->v1_hdr_enable);
    pq_hihdr_v2_setimv0hdren(vdp_reg, offset, imap_cfg->v0_hdr_enable);

    pq_hihdr_v2_setimrshiftrounden(vdp_reg, offset, imap_cfg->rshift_round_en);
    pq_hihdr_v2_setimrshiften(vdp_reg, offset, imap_cfg->rshift_en);
    pq_hihdr_v2_setimckgten(vdp_reg, offset, HI_TRUE);
    pq_hihdr_v2_setimiptinsel(vdp_reg, offset, imap_cfg->ipt_in_sel);
    pq_hihdr_v2_setimapincolor(vdp_reg, offset, imap_cfg->in_color);
    pq_hihdr_v2_setiminbits(vdp_reg, offset, imap_cfg->in_bits);
    pq_hihdr_v2_setimapeotfparameotf(vdp_reg, offset, imap_cfg->eotfparam_eotf);
    pq_hihdr_v2_setimapeotfparamrangemin(vdp_reg, offset, imap_cfg->eotfparam_range_min);
    pq_hihdr_v2_setimapeotfparamrange(vdp_reg, offset, imap_cfg->eotfparam_range);
    pq_hihdr_v2_setimapeotfparamrangeinv(vdp_reg, offset, imap_cfg->eotfparam_range_inv);
    pq_hihdr_v2_setimapv3iptoff0(vdp_reg, offset, imap_cfg->v3ipt_off[0]);
    pq_hihdr_v2_setimapv3iptoff1(vdp_reg, offset, imap_cfg->v3ipt_off[1]);
    pq_hihdr_v2_setimapv3iptoff2(vdp_reg, offset, imap_cfg->v3ipt_off[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapiptscale(vdp_reg, offset, imap_cfg->ipt_scale);
    pq_hihdr_v2_setimdegammaclipmax(vdp_reg, offset, imap_cfg->degamma_clip_max);
    pq_hihdr_v2_setimdegammaclipmin(vdp_reg, offset, imap_cfg->degamma_clip_min);
    pq_hihdr_v2_setimapladjchromaweight(vdp_reg, offset, imap_cfg->ladj_chroma_weight);
    pq_hihdr_v2_setimdemolumamode(vdp_reg, offset, imap_cfg->demo_luma_mode);

    pq_gfxhdr_set_imap_yuv2rgb(vdp_reg, offset, &imap_cfg->yuv2rgb);
    pq_gfxhdr_set_imap_rgb2lms(vdp_reg, offset, &imap_cfg->rgb2lms);
    pq_gfxhdr_set_imap_lms2ipt(vdp_reg, offset, &imap_cfg->lms2ipt);
    pq_gfxhdr_set_imap_tmap_v1(vdp_reg, offset, &imap_cfg->tmap_v1);
    pq_gfxhdr_set_imap_cacm(vdp_reg, offset, &imap_cfg->cacm);

    return;
}

hi_void pq_gfx_ip_hdr_set(vdp_regs_type *vdp_reg, hi_drv_pq_imap_cfg *imap_cfg)
{
    hi_u32 offset = PQ_GP0_HDR_OFFSET;

    pq_hdr_set_imap_mode(vdp_reg, offset, imap_cfg);

    return;
}

hi_void pq_gfx_ip_hdr_coef_set(vdp_regs_type *vdp_reg, hi_drv_pq_hdr_coef_addr *coef_addr)
{
    hi_u32 shift = 32;
#ifdef PQ_CS_TYPE /* para chan & para up : es & cs are different !!!!!!!!!!!!!!!!!! */
    hi_u32 gp_hdr0_para_chn = 104; /* 104 : 96 + 8 */
    hi_u32 gp_hdr1_para_chn = 105; /* 105 : 96 + 9 */
    hi_u32 gp_hdr2_para_chn = 106; /* 106 : 96 + 10 */
    hi_u32 gp_hdr3_para_chn = 107; /* 107 : 96 + 11 */
#else
    hi_u32 gp_hdr0_para_chn = 105; /* 105 : 96 + 9 */
    hi_u32 gp_hdr1_para_chn = 106; /* 106 : 96 + 10 */
    hi_u32 gp_hdr2_para_chn = 107; /* 107 : 96 + 11 */
    hi_u32 gp_hdr3_para_chn = 108; /* 108 : 96 + 12 */
#endif

    pq_para_setparaaddrdispchn09(vdp_reg, g_pq_coef_buf_addr.phy_addr[PQ_COEF_BUF_GP0_HDR0]);
    pq_para_setparahaddrdispchn09(vdp_reg, g_pq_coef_buf_addr.phy_addr[PQ_COEF_BUF_GP0_HDR0] >> shift);

    pq_para_setparaaddrdispchn10(vdp_reg, g_pq_coef_buf_addr.phy_addr[PQ_COEF_BUF_GP0_HDR1]);
    pq_para_setparahaddrdispchn10(vdp_reg, g_pq_coef_buf_addr.phy_addr[PQ_COEF_BUF_GP0_HDR1] >> shift);

    pq_para_setparaaddrdispchn11(vdp_reg, g_pq_coef_buf_addr.phy_addr[PQ_COEF_BUF_GP0_HDR2]);
    pq_para_setparahaddrdispchn11(vdp_reg, g_pq_coef_buf_addr.phy_addr[PQ_COEF_BUF_GP0_HDR2] >> shift);

    pq_para_setparaaddrdispchn12(vdp_reg, g_pq_coef_buf_addr.phy_addr[PQ_COEF_BUF_GP0_HDR3]);
    pq_para_setparahaddrdispchn12(vdp_reg, g_pq_coef_buf_addr.phy_addr[PQ_COEF_BUF_GP0_HDR3] >> shift);

    pq_para_setparaupdispchn(vdp_reg, gp_hdr0_para_chn);
    pq_para_setparaupdispchn(vdp_reg, gp_hdr1_para_chn);
    pq_para_setparaupdispchn(vdp_reg, gp_hdr2_para_chn);
    pq_para_setparaupdispchn(vdp_reg, gp_hdr3_para_chn);

    return;
}

pq_gfx_hdr_fun g_pf_pq_hal_get_gfx_hdr_path_cfg[HI_PQ_HDR_MODE_MAX] = {
    { HI_PQ_HDR_MODE_HDR10_TO_SDR,   pq_hal_get_gfx_hdr2sdr_cfg },
    { HI_PQ_HDR_MODE_HDR10_TO_HDR10, pq_hal_get_gfx_hdr2hdr_cfg },
    { HI_PQ_HDR_MODE_HDR10_TO_HLG,   pq_hal_get_gfx_hdr2hlg_cfg },

    { HI_PQ_HDR_MODE_HLG_TO_SDR,   pq_hal_get_gfx_hlg2sdr_cfg },
    { HI_PQ_HDR_MODE_HLG_TO_HDR10, pq_hal_get_gfx_hlg2hdr_cfg },
    { HI_PQ_HDR_MODE_HLG_TO_HLG,   pq_hal_get_gfx_hlg2hlg_cfg },

    { HI_PQ_HDR_MODE_SDR_TO_SDR,   pq_hal_get_gfx_sdr2sdr_cfg },
    { HI_PQ_HDR_MODE_SDR_TO_HDR10, pq_hal_get_gfx_sdr2hdr_cfg },
    { HI_PQ_HDR_MODE_SDR_TO_HLG,   pq_hal_get_gfx_sdr2hlg_cfg },

    { HI_PQ_HDR_MODE_HDR10PLUS_TO_HDR10PLUS, pq_hal_get_gfx_hdr10plus2hdr10plus_cfg },
    { HI_PQ_HDR_MODE_HDR10PLUS_TO_SDR,       pq_hal_get_gfx_hdr10plus2sdr_cfg },
    { HI_PQ_HDR_MODE_HDR10PLUS_TO_HDR10,     pq_hal_get_gfx_hdr10plus2hdr10_cfg },
    { HI_PQ_HDR_MODE_HDR10PLUS_TO_HLG,       pq_hal_get_gfx_hdr10plus2hlg_cfg },
};

hi_s32 pq_hal_get_gfx_sdr_trans_mode(hi_drv_hdr_type disp_hdr, hi_pq_hdr_mode *hdr_mode)
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

hi_s32 pq_hal_get_gfx_hdr10_trans_mode(hi_drv_hdr_type disp_hdr, hi_pq_hdr_mode *hdr_mode)
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

hi_s32 pq_hal_get_gfx_hlg_trans_mode(hi_drv_hdr_type disp_hdr, hi_pq_hdr_mode *hdr_mode)
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
hi_s32 pq_hal_get_gfx_hdr10plus_trans_mode(hi_drv_hdr_type disp_hdr, hi_pq_hdr_mode *hdr_mode)
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

hi_s32 pq_hal_get_gfxhdr_mode(hi_drv_hdr_type src_hdr, hi_drv_hdr_type disp_hdr, hi_pq_hdr_mode *hdr_mode)
{
    if (src_hdr == HI_DRV_HDR_TYPE_SDR) {
        pq_hal_get_gfx_sdr_trans_mode(disp_hdr, hdr_mode);
    } else if (src_hdr == HI_DRV_HDR_TYPE_HDR10) {
        pq_hal_get_gfx_hdr10_trans_mode(disp_hdr, hdr_mode);
    } else if (src_hdr == HI_DRV_HDR_TYPE_HLG) {
        pq_hal_get_gfx_hlg_trans_mode(disp_hdr, hdr_mode);
#ifdef HI_HDR10PLUS_SUPPORT
    } else if (src_hdr == HI_DRV_HDR_TYPE_HDR10PLUS) {
        pq_hal_get_gfx_hdr10plus_trans_mode(disp_hdr, hdr_mode);
#endif
    } else {
        HI_WARN_PQ("Unkown support hdr type, src: %d, disp : %d\n", src_hdr, disp_hdr);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_gfxhdr_cfg(hi_drv_pq_gfx_layer gfx_layer, drv_pq_internal_gfx_hdr_info *hdr_info)
{
    hi_s32 ret;
    hi_u32 index;
    hi_pq_hdr_mode hdr_mode = HI_PQ_HDR_MODE_SDR_TO_HDR10;

    if (gfx_layer != HI_DRV_PQ_GFX_LAYER_GP0) {
        HI_ERR_PQ("do not support err layer : %d \n", gfx_layer);
        return HI_FAILURE;
    }

    ret = pq_hal_get_gfxhdr_mode(hdr_info->src_hdr_type, hdr_info->disp_hdr_type, &hdr_mode);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("gfx hdr cannot support !!!\n");
        return HI_FAILURE;
    }

    g_gfxhdr_mode = hdr_mode;

    for (index = 0; index < HI_PQ_HDR_MODE_MAX; index++) {
        if (g_pf_pq_hal_get_gfx_hdr_path_cfg[index].hdr_mode == hdr_mode) {
            break;
        }
    }

    if (index < HI_PQ_HDR_MODE_MAX) {
        if (g_pf_pq_hal_get_gfx_hdr_path_cfg[index].hdr_func != HI_NULL) {
            ret = g_pf_pq_hal_get_gfx_hdr_path_cfg[index].hdr_func(hdr_info->color_space_in,
                                                                   hdr_info->color_space_out,
                                                                   &g_gfxhdr_imap_cfg);
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

hi_s32 pq_hal_set_gfxhdr_cfg(hi_drv_pq_gfx_layer gfx_layer, drv_pq_internal_gfx_hdr_info *hdr_info)
{
    vdp_regs_type *vdp_reg = HI_NULL;
    vdp_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(vdp_reg);

    if (g_gfxhdr_mode < HI_PQ_HDR_MODE_MAX) {
        pq_gfx_ip_hdr_set(vdp_reg, &(g_gfxhdr_imap_cfg.imap_cfg));
        pq_gfx_ip_hdr_coef_set(vdp_reg, &(g_gfxhdr_imap_cfg.coef_addr));
    }

    return HI_SUCCESS;
}

/* b_default : 0 / bin success; 1 / bin fail */
hi_s32 pq_hal_init_gfxhdr(pq_bin_param *pq_param, hi_bool default_code)
{
    pq_ip_para_init();

    return HI_SUCCESS;
}

hi_s32 pq_hal_deinit_gfxhdr(hi_void)
{
    pq_ip_para_deinit();

    return HI_SUCCESS;
}

