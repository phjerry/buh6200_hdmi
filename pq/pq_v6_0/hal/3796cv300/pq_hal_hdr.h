/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal hdr api
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef _PQ_HAL_HDR_H_
#define _PQ_HAL_HDR_H_

#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PQ_DEGMM_SEG_SIZE       8
#define PQ_TMAP_SEG_SIZE        8
#define PQ_SMAP_SEG_SIZE        8
#define PQ_GMM_SEG_SIZE         8
#define PQ_DEGMM_LUT_SIZE      64
#define PQ_TMAP_LUT_SIZE       64
#define PQ_SMAP_LUT_SIZE       64
#define PQ_GMM_LUT_SIZE        64

#define PQ_HDR_CM_8   8
#define PQ_HDR_CM_3   3
#define PQ_HDR_OMAP_8   8
#define PQ_HDR_OMAP_3   3
#define PQ_HDR_TMAP_8   8
#define PQ_HDR_TMAP_3   3
#define PQ_HDR_DC_3   3
#define PQ_HDR_IPT_3   3

#define HDRV1_CM_32 32
#define IMAP_DEGAMMA_1024 1024
#define IMAP_GAMMA_LUT_128 128
#define TMAPV1_LUT_256 256
#define IMAP_CM_LUT_9 9
#define TMAP_TM_LUT_512 512
#define TMAP_SM_LUT_512 512
#define TMAP_SMC_LUT_512 512
#define OMAP_DEGAMMA_1024 1024
#define OMAP_GAMMA_LUT_128 128
#define OMAP_CM_LUT_9 9

#define DEF_PQ2L_LUT_SIZE_1024      (1<<10)
#define DEF_L2PQ_LUT_NODES          128

#define LUT_SIZE                    9
#define LUT_EVE_SIZE                5
#define LUT_ODD_SIZE                4

typedef enum {
    PQ_COEF_BUF_OM_DEGMM = 7,
    PQ_COEF_BUF_OM_GMM = 8,
    PQ_COEF_BUF_OM_CM = 19,
    PQ_COEF_BUF_V0_HDR0 = 64 + 1,
    PQ_COEF_BUF_V0_HDR1 = 64 + 2,
    PQ_COEF_BUF_V0_HDR2 = 64 + 3,
    PQ_COEF_BUF_V0_HDR3 = 64 + 4,
    PQ_COEF_BUF_V0_HDR4 = 64 + 5,
    PQ_COEF_BUF_V0_HDR5 = 64 + 6,
    PQ_COEF_BUF_V0_HDR6 = 64 + 7,
    PQ_COEF_BUF_V0_HDR7 = 64 + 8,
    PQ_COEF_BUF_V0_HDR8 = 64 + 9,
    PQ_COEF_BUF_HDR_RESERVE = 64 + 15,

    PQ_COEF_BUF_8K_ZME = 64 + 18,
    PQ_COEF_BUF_ALL = 200,
    PQ_COEF_BUF_BUTT = 201
} vdp_coef_buf;

typedef struct {
    hi_u32 u32size;
    hi_u8 *vir_addr[PQ_COEF_BUF_BUTT + 1];
    hi_u64 phy_addr[PQ_COEF_BUF_BUTT + 1];
} pq_hdr_coef_addr;

typedef struct {
    hi_bool enable;
    hi_u32 clip_c_max;
    hi_u32 clip_c_min;
    hi_u32 scale2p;
    hi_u32 step[PQ_HDR_CM_8]; /* 8 is number */
    hi_u32 pos[PQ_HDR_CM_8];  /* 8 is number */
    hi_u32 num[PQ_HDR_CM_8];  /* 8 is number */
} pq_hdrv1_cm_cfg;

typedef struct {
    hi_bool enable;
    hi_bool rshift_round_en;
    hi_bool sclut_rd_en;
    hi_bool sslut_rd_en;
    hi_bool tslut_rd_en;
    hi_bool silut_rd_en;
    hi_bool tilut_rd_en;
    hi_bool smc_enable;
    hi_bool rshift_en;
    hi_bool s2u_en;

    hi_u32 c1_expan;
    hi_u32 para_rdata;
    hi_u32 rshift_bit;
} pq_tmap_cfg;

typedef struct {
    hi_bool enable;
    hi_u32 min;
    hi_u32 max;
    hi_u32 scale2p;
    hi_s16 coef[3][3]; /* 3 is number */
} pq_lms2rgb_cfg;

typedef struct {
    hi_bool enable;
    hi_u32 min;
    hi_u32 max;
    hi_u32 scale2p;
    hi_s32 out_dc[3];  /* 3 is number */
    hi_s32 in_dc[3];   /* 3 is number */
    hi_s16 coef[3][3]; /* 3 is number */
} pq_ipt2lms_cfg;

typedef struct {
    hi_bool enable;
    hi_s16 coef[3][3]; /* 3 is number */
    hi_u32 scale2p;
    hi_s32 out_dc[3]; /* 3 is number */
    hi_s32 in_dc[3];  /* 3 is number */
    hi_u32 min;
    hi_u32 max;
} pq_rgb2yuv_cfg;

typedef struct {
    hi_bool enable;
    hi_u32 cm_pos;
    hi_u32 demo_pos;
    hi_u32 demo_mode;
    hi_bool demo_en;
    hi_u32 bitdepth_out_mode;
    hi_u32 bitdepth_in_mode;
} pq_cacm_cfg;

typedef struct {
    hi_bool enable;

    hi_bool ipt_out_en; /* OmapIptOutEn */
    hi_bool u2s_enable;
    hi_bool lshift_en;
    hi_bool degamma_en;
    hi_bool gamma_en;
    hi_bool denorm_en;
    hi_bool cvm_en; /* ?a??¡§o?tmap |¨¬?cvm */

    hi_u32 out_bits;
    hi_u32 out_color;
    hi_u32 ipt_scale;
    hi_u32 oetf;
    hi_u32 range_min;
    hi_u32 range_over;
    hi_s32 ipt_off[3]; /* 3 is number */

    pq_ipt2lms_cfg ipt2lms;
    pq_lms2rgb_cfg lms2rgb;
    pq_cacm_cfg cm_cfg;
    pq_rgb2yuv_cfg rgb2yuv;
} pq_omap_cfg;

typedef struct {
    hi_bool hdr_enable;

    hi_u32 hdr_link;
    hi_u32 omap_link;

    pq_hdrv1_cm_cfg cm_cfg;
    hi_drv_pq_imap_cfg imap_cfg;
    pq_tmap_cfg tmap_cfg;
    pq_omap_cfg omap_cfg;

    hi_drv_pq_hdr_coef_addr coef_addr;
} pq_hal_hdr_cfg;

typedef struct {
    hi_u32 src_max_pq;
    hi_u32 src_min_pq;
} dm_info;

typedef enum {
    PQ_HDR_CM_POS0 = 0,
    PQ_HDR_CM_POS1,

    PQ_HDR_CM_POS_MAX
} pq_hdr_cm_pos;

typedef enum {
    PQ_HDR_TM_POS0 = 0,
    PQ_HDR_TM_POS1,

    PQ_HDR_TM_POS_MAX
} pq_hdr_tm_pos;

hi_s32 pq_hal_get_hdrcfg(hi_drv_pq_xdr_frame_info *xdr_frame_info, pq_hal_hdr_cfg *hdr_cfg);

hi_s32 pq_hal_init_hdr(pq_bin_param *param, hi_bool para_use_table_default);

hi_s32 pq_hal_tool_set_hdrpara_mode(hi_pq_hdr_para_mode *mode);

hi_s32 pq_hal_tool_get_hdrpara_mode(hi_pq_hdr_para_mode *mode);

hi_s32 pq_hal_tool_set_hdr_tmap(hi_pq_hdr_tmap *hdr_tmap);

hi_s32 pq_hal_tool_get_hdr_tmap(hi_pq_hdr_tmap *hdr_tmap);

hi_s32 pq_hal_tool_set_hdr_smap(hi_pq_hdr_smap *hdr_smap);

hi_s32 pq_hal_tool_get_hdr_smap(hi_pq_hdr_smap *hdr_smap);

hi_s32 pq_hal_set_hdrdci_histgram(hi_u32 dci_out_width, hi_u32 dci_out_height, hi_pq_dci_histgram *dci_histgram);

hi_s32 pq_hal_set_hdr_tm_curve(hi_pq_hdr_offset *hdr_offset_para);

hi_s32 pq_hal_set_hdr_offset(hi_pq_hdr_offset *hdr_offset_para);

hi_s32 pq_hal_set_hdr_csc_setting(hi_pq_image_param *pic_setting);
hi_s32 pq_hal_proc_get_hdr_patch_config(hi_pq_hdr_mode *hdr_mode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
