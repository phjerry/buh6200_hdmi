/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hdr api for other module
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __DRV_PQ_EXT_HDR_H__
#define __DRV_PQ_EXT_HDR_H__

#include "hi_drv_color.h"
#include "hi_drv_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PQ_HWC_HDR_TMAP_8   8
#define PQ_HWC_HDR_TMAP_3   3
#define PQ_HWC_HDR_DC_3   3
#define PQ_HWC_HDR_IPT_3   3
#define PQ_CSC_M33_SIZE 3
#define PQ_CSC_DC_SIZE 3

/***************************************Common********************************************/
/* Hdr Layer ID */
typedef enum {
    HI_DRV_PQ_XDR_LAYER_ID_0 = 0, /* HDR LAYER ID 0 */
    HI_DRV_PQ_XDR_LAYER_ID_1 = 1, /* HDR LAYER ID 1 */

    HI_DRV_PQ_XDR_LAYER_ID_MAX
} hi_drv_pq_xdr_layer_id;

typedef struct {
    hi_u32 window_num; /* window num : 1,vo, >1:multi, 0:invaild */

    hi_u32 handle;
    hi_drv_hdr_type src_hdr_type;  /* src type: SDR/HDR10/... */
    hi_drv_hdr_type disp_hdr_type; /* out type: SDR/HDR10/... */

    union {
        hi_drv_hdr_static_metadata hdr10_info; /* HDR10 frame info. */
#ifdef HI_HDR10PLUS_SUPPORT
        hi_drv_hdr_hdr10plus_metadata hdr10plus_info; /* HDR10+ frame info. */
#endif
        hi_drv_hdr_hlg_metadata hlg_info; /* HLG frame info. */
    } hdr_info;

    hi_drv_color_descript color_space_in;  /* input color */
    hi_drv_color_descript color_space_out; /* output color */
} hi_drv_pq_xdr_frame_info;

/***************************************GFX********************************************/
typedef struct {
    hi_drv_hdr_type src_hdr_type;       /* input : SDR/HDR10/... */
    hi_drv_hdr_type disp_hdr_type;      /* output : SDR/HDR10/... */
    hi_drv_color_descript color_space_in;  /* input color */
    hi_drv_color_descript color_space_out; /* output color */
} hi_drv_pq_gfx_hdr_info;

/***************************************HWC********************************************/
typedef enum {
    HI_DRV_PQ_FMT_ARGB2101010 = 0,
    HI_DRV_PQ_FMT_ARGB10101010,
    HI_DRV_PQ_FMT_ARGB8888,
    HI_DRV_PQ_FMT_YUV420,

    HI_DRV_PQ_FMT_MAX,
} hi_drv_pq_hwc_color_fmt;

typedef enum {
    HI_DRV_PQ_CG_601 = 0,
    HI_DRV_PQ_CG_709,
    HI_DRV_PQ_CG_2020,
    HI_DRV_PQ_CG_SRGBA,
    HI_DRV_PQ_CG_SCRGB,

    HI_DRV_PQ_CG_MAX,
} hi_drv_pq_color_gamut;

typedef enum {
    HI_DRV_PQ_CR_LIMIT = 0,
    HI_DRV_PQ_CR_FULL,

    HI_DRV_PQ_CR_MAX,
}hi_drv_pq_color_range;

typedef struct {
    hi_drv_pq_color_gamut cg;
    hi_drv_pq_color_range cr;
} hi_drv_pq_hwc_color_space;

typedef enum {
    HI_DRV_HWCHDR_TYPE_SDR = 0,      /* normal SDR frame */
    HI_DRV_HWCHDR_TYPE_HDR10,        /* HDR10 frame */
    HI_DRV_HWCHDR_TYPE_HLG,          /* HLG frame */

    HI_DRV_HWCHDR_TYPE_MAX
} hi_drv_pq_hwc_hdr_type;

typedef struct {
    hi_drv_pq_hwc_color_fmt color_fmt_in;
    hi_drv_pq_hwc_color_fmt color_fmt_out;

    hi_drv_pq_hwc_hdr_type hdr_type_in; /* input : SDR/HDR10/... */
    hi_drv_pq_hwc_hdr_type hdr_type_out; /* output : SDR/HDR10/... */

    hi_drv_pq_hwc_color_space color_space_in;  /* input color */
    hi_drv_pq_hwc_color_space color_space_out; /* output color */
} hi_drv_pq_hwc_hdr_info;

typedef struct {
    hi_bool enable;
    hi_u32 cm_pos;
    hi_u32 demo_pos;
    hi_u32 demo_mode;
    hi_u32 demo_en;
    hi_u32 bitdepth_out_mode;
    hi_u32 bitdepth_in_mode;
} hi_drv_pq_cacm_cfg;

typedef struct {
    hi_bool enable;
    hi_u32 tm_pos;
    hi_u32 scale_mix_alpha;
    hi_u32 mix_alpha;
    hi_u32 clip_min;
    hi_u32 clip_max;
    hi_u32 step[PQ_HWC_HDR_TMAP_8];
    hi_u32 pos[PQ_HWC_HDR_TMAP_8];
    hi_u32 num[PQ_HWC_HDR_TMAP_8];
    hi_u32 lum_cal[PQ_HWC_HDR_TMAP_3];
    hi_u32 scale_lum_cal;
    hi_u32 scale_coef;
    hi_s32 dc_out[PQ_HWC_HDR_DC_3];
} hi_drv_pq_tmap_v1_cfg;

typedef struct {
    hi_bool enable;
    hi_bool v1_enable;
    hi_bool v0_enable;
    hi_u32 mode;

    hi_u32 scale2p;
    hi_s32 dc_in[PQ_HWC_HDR_DC_3];
    hi_s32 dc_in2[PQ_HWC_HDR_DC_3];
    hi_s32 offinrgb[PQ_HWC_HDR_DC_3];
    hi_s32 offinrgb2[PQ_HWC_HDR_DC_3];
    hi_u32 clip_max;
    hi_u32 clip_min;
    hi_u32 thr_r;
    hi_u32 thr_b;
    hi_s32 coef[PQ_HWC_HDR_DC_3][PQ_HWC_HDR_DC_3];
    hi_s32 coef2[PQ_HWC_HDR_DC_3][PQ_HWC_HDR_DC_3];
} hi_drv_pq_yuv2rgb_cfg;

typedef struct {
    hi_bool enable;
    hi_s32 coef[PQ_HWC_HDR_DC_3][PQ_HWC_HDR_DC_3];

    hi_u32 scale2p;
    hi_u32 clip_max;
    hi_u32 clip_min;
    hi_s32 dc_in[PQ_HWC_HDR_DC_3];
} hi_drv_pq_rgb2lms_cfg;

typedef struct {
    hi_bool enable;
    hi_s32 coef[PQ_HWC_HDR_DC_3][PQ_HWC_HDR_DC_3];
    hi_u32 scale2p;

    hi_s32 clip_min_y;
    hi_s32 clip_max_y;
    hi_s32 clip_min_c;
    hi_s32 clip_max_c;

    hi_s32 dc_out[PQ_HWC_HDR_DC_3];
    hi_s32 dc_in[PQ_HWC_HDR_DC_3];
} hi_drv_pq_lms2ipt_cfg;

typedef struct {
    hi_bool enable;
    hi_bool v1_hdr_enable;
    hi_bool v0_hdr_enable;

    hi_bool rshift_round_en;
    hi_bool rshift_en;
    hi_u32 ipt_in_sel;
    hi_bool ladj_en;

    hi_u32 in_color;
    hi_u32 in_bits;
    hi_bool gamma_en;
    hi_bool degamma_en;
    hi_bool norm_en;

    hi_u32 eotfparam_eotf;
    hi_u32 eotfparam_range_min;
    hi_u32 eotfparam_range;
    hi_u32 eotfparam_range_inv;

    hi_s32 v3ipt_off[PQ_HWC_HDR_IPT_3];
    hi_s32 ipt_scale;
    hi_u32 degamma_clip_max;
    hi_u32 degamma_clip_min;
    hi_u32 ladj_chroma_weight;

    hi_u32 demo_luma_mode;
    hi_bool demo_luma_en;

    hi_drv_pq_cacm_cfg cacm;
    hi_drv_pq_tmap_v1_cfg tmap_v1;
    hi_drv_pq_yuv2rgb_cfg yuv2rgb;
    hi_drv_pq_rgb2lms_cfg rgb2lms;
    hi_drv_pq_lms2ipt_cfg lms2ipt;
} hi_drv_pq_imap_cfg;

typedef struct {
    hi_u64 hdrv1_cm;
    hi_u64 imap_degamma_addr;
    hi_u64 imap_gamma_addr;
    hi_u64 imap_cacm_addr;
    hi_u64 imap_tmapv1_addr;
    hi_u64 tmapv2_addr;

    hi_u64 omap_degamm_addr;
    hi_u64 omap_gamm_addr;
    hi_u64 omap_cm_addr;
} hi_drv_pq_hdr_coef_addr;

typedef struct {
    hi_drv_pq_imap_cfg  imap_cfg;
    hi_drv_pq_hdr_coef_addr coef_addr;
} hi_drv_pq_hwc_hdr_cfg;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


