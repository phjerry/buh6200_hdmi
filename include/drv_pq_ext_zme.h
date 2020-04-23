/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: zme api for other module
 * Author: shijiaoling
 * Create: 2019-7-1
 */

#ifndef __DRV_PQ_EXT_ZME_H__
#define __DRV_PQ_EXT_ZME_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/***************************************Common********************************************/
typedef enum {
    HI_DRV_PQ_ZME_COPY = 0, /* zme use copy filter */
    HI_DRV_PQ_ZME_FIR,    /* zme use fir filter */

    HI_DRV_PQ_ZME_MAX,
} hi_drv_pq_fir_mode;

typedef enum {
    HI_DRV_PQ_ZME_FMT_420 = 0,
    HI_DRV_PQ_ZME_FMT_422,
    HI_DRV_PQ_ZME_FMT_444,

    HI_DRV_PQ_ZME_FMT_MAX,
} hi_drv_pq_zme_fmt;

typedef enum {
    HI_DRV_PQ_FRM_FRAME = 0,    /* frame */
    HI_DRV_PQ_FRM_TOP_FIELD,    /* top filed */
    HI_DRV_PQ_FRM_BOTTOM_FIELD, /* bottom filed */

    HI_DRV_PQ_FRM_MAX,
} hi_drv_pq_frame_fmt;

typedef struct {
    hi_u32 zme_w_in;   /* zme  input frame width */
    hi_u32 zme_h_in;  /* zme  input frame height */
    hi_u32 zme_w_out;  /* zme output frame width */
    hi_u32 zme_h_out; /* zme output frame height */

    hi_drv_pq_zme_fmt zme_fmt_in;
    hi_drv_pq_zme_fmt zme_fmt_out;

    hi_drv_pq_frame_fmt zme_frame_in;
    hi_drv_pq_frame_fmt zme_frame_out;
} hi_drv_pq_zme_common_in;

typedef struct {
    hi_bool zme_h_en;
    hi_bool zme_v_en;
    hi_bool zme_hl_en;
    hi_bool zme_hc_en;
    hi_bool zme_vl_en;
    hi_bool zme_vc_en;
} hi_drv_pq_zme_fir_en;

typedef struct {
    hi_drv_pq_fir_mode zme_fir_hl;
    hi_drv_pq_fir_mode zme_fir_hc;
    hi_drv_pq_fir_mode zme_fir_vl;
    hi_drv_pq_fir_mode zme_fir_vc;
} hi_drv_pq_zme_fir_mode;

typedef struct {
    hi_bool zme_med_h_en;
    hi_bool zme_med_hl_en;
    hi_bool zme_med_hc_en;
    hi_bool zme_med_v_en;
    hi_bool zme_med_vl_en;
    hi_bool zme_med_vc_en;
} hi_drv_pq_zme_med_fir_en;

typedef struct {
    hi_s32 zme_offset_h;
    hi_s32 zme_offset_hl;
    hi_s32 zme_offset_hc;
    hi_s32 zme_offset_v;
    hi_s32 zme_offset_vl;
    hi_s32 zme_offset_vc;
    hi_s32 zme_offset_v_tp;
    hi_s32 zme_offset_v_btm;
    hi_s32 zme_offset_vl_btm;
    hi_s32 zme_offset_vc_btm;
} hi_drv_pq_zme_offset;

/* hor draw pixel */
typedef enum {
    HI_DRV_PQ_ZME_VER_HOR = 0,
    HI_DRV_PQ_ZME_HOR_VER,

    HI_DRV_PQ_ZME_ORDER_MAX
} hi_drv_pq_zme_order;

/* hor draw pixel */
typedef enum {
    HI_DRV_PQ_PREZME_HOR_DISABLE = 1,
    HI_DRV_PQ_PREZME_HOR_2 = 2,
    HI_DRV_PQ_PREZME_HOR_4 = 4,
    HI_DRV_PQ_PREZME_HOR_8 = 8,
    HI_DRV_PQ_PREZME_HOR_16 = 16,
    HI_DRV_PQ_PREZME_HOR_32 = 32,

    HI_DRV_PQ_PREZME_HOR_MAX
} hi_drv_pq_hor_draw_mul;

/* ver draw line */
typedef enum {
    HI_DRV_PQ_PREZME_VER_DISABLE = 1,
    HI_DRV_PQ_PREZME_VER_2 = 2,
    HI_DRV_PQ_PREZME_VER_4 = 4,
    HI_DRV_PQ_PREZME_VER_8 = 8,
    HI_DRV_PQ_PREZME_VER_16 = 16,
    HI_DRV_PQ_PREZME_VER_32 = 32,

    HI_DRV_PQ_PREZME_VER_MAX
} hi_drv_pq_ver_draw_mul;

typedef enum {
    HI_DRV_PQ_8KZME_POS0 = 0,
    HI_DRV_PQ_8KZME_POS1,

    HI_DRV_PQ_ZME_LINK_MAX
} hi_drv_pq_zme_link;

typedef struct {
    hi_u32 zme_w_in;
    hi_u32 zme_h_in;
    hi_u32 zme_w_out;
    hi_u32 zme_h_out;

    hi_bool zme_enable;
    hi_bool zme_hor_enable;
    hi_bool zme_ver_enable;

    hi_drv_pq_zme_fir_en zme_fir_en;         /* filter  enable or diable */
    hi_drv_pq_zme_fir_mode zme_fir_mode;     /* use copy or fir mode */
    hi_drv_pq_zme_med_fir_en zme_med_fir_en; /* med filter enable or diable */

    hi_drv_pq_zme_offset zme_offset;

    hi_drv_pq_zme_fmt zme_fmt_in;  /* video format for zme input : 422/420/444 */
    hi_drv_pq_zme_fmt zme_fmt_out; /* video format for zme input : 422/420/444 */

    hi_drv_pq_frame_fmt frame_fmt_in;  /* Frame format for zme input: 0-field; 1-frame */
    hi_drv_pq_frame_fmt frame_fmt_out;  /* Frame format for zme input: 0-field; 1-frame */

    hi_drv_pq_zme_link zme_point; /* 8KZME:  0-表达绑定在SR前，1-表示绑定到SR后*/
} hi_drv_pq_zme_common_out;

typedef struct {
    hi_u64 zme_coef_hl_addr;
    hi_u64 zme_coef_hc_addr;
    hi_u64 zme_coef_vl_addr;
    hi_u64 zme_coef_vc_addr;
} hi_drv_pq_zme_coef_addr;

/***************************************VDP********************************************/
typedef enum {
    HI_DRV_PQ_DISP_V0_LAYER_ZME = 0,
    HI_DRV_PQ_DISP_V1_LAYER_ZME,
    HI_DRV_PQ_DISP_V2_LAYER_ZME,
    HI_DRV_PQ_DISP_V3_LAYER_ZME,
    HI_DRV_PQ_DISP_V4_LAYER_ZME,
    HI_DRV_PQ_DISP_WBC_VP_LAYER_ZME,
    HI_DRV_PQ_DISP_WBC_DHD0_LAYER_ZME,

    HI_DRV_PQ_DISP_LAYER_ZME_MAX
} hi_drv_pq_zme_layer;

typedef struct {
    hi_bool is_hdcp_scence; /* is HDCP Scence? 1:yes; 0:no */
    hi_bool sr_en;          /* sr need close or not */
    hi_u32 hdcp_width;
    hi_u32 hdcp_height;
} hi_drv_pq_hdcp_factor;

typedef struct {
    hi_drv_pq_zme_common_in zme_common_info;
    /* special scence */
    hi_bool is_dolby_path;
    hi_drv_pq_hdcp_factor hdcp_factor; /* HDCP scence reduce resolution */
} hi_drv_pq_hd_zme_strategy_in;

typedef struct {
    /* when lowdelay mode, field mode is needed, give a mandatory setting. 0-frame; 1-field */
    hi_bool is_force_wbc_field;

    /* pip/mosaic and watermark scene, wbc point should be forced to set to VP. */
    hi_bool is_force_wbc_point_vp;

    hi_drv_pq_zme_common_in zme_common_info;
} hi_drv_pq_wbc_zme_strategy_in;

typedef union {
    hi_drv_pq_hd_zme_strategy_in hd_zme_strategy;
    hi_drv_pq_wbc_zme_strategy_in wbc_vp_zme_strategy;
    hi_drv_pq_wbc_zme_strategy_in wbc_dhd0_zme_strategy;
} hi_drv_pq_zme_strategy_in;

typedef enum {
    HI_DRV_PQ_4KZME = 0, /**< V0 4K ZME */
    HI_DRV_PQ_4KSR,     /**< V0 4K SR */
    HI_DRV_PQ_8KSR,     /**< V0 8K SR */
    HI_DRV_PQ_8KZME,     /**< V0 8K ZME */

    HI_DRV_PQ_NODE_MAX
} hi_drv_pq_v0_zme_node;

typedef enum {
    HI_DRV_PQ_WBC_VP_POINT_VP = 0,
    HI_DRV_PQ_WBC_VP_POINT_V0 = 1,

    HI_DRV_PQ_WBC_VP_POINT_MAX
} hi_drv_pq_wbc_vp_band_point;

typedef enum {
    HI_DRV_PQ_WBC_DHD0_POINT_V0 = 0,
    HI_DRV_PQ_WBC_DHD0_POINT_CBM0,

    HI_DRV_PQ_WBC_DHD0_POINT_MAX
} hi_drv_pq_wbc_dhd0_band_point;

typedef struct {
    hi_u64 pq_8kzme_coef_addr_h;
    hi_u64 pq_8kzme_coef_addr_v;

    hi_u64 pq_4kzme_coef_addr_h;
    hi_u64 pq_4kzme_coef_addr_v;

    hi_u64 pq_4ksr0_coef_addr;
    hi_u64 pq_8ksr0_coef_addr;
} hi_drv_pq_hd_zme_coef_addr;
/* V0 */
typedef struct {
    hi_drv_pq_zme_common_out zme_fmt[HI_DRV_PQ_NODE_MAX];
    hi_u32 zme_num;
    hi_drv_pq_frame_fmt read_fmt_in;

    hi_drv_pq_hor_draw_mul hor_draw_mul; /* Horizontal Extract Pixel */
    hi_drv_pq_ver_draw_mul ver_draw_mul; /* Vertical   Extract Line */

    hi_bool cle_4k_en;
    hi_bool cle_8k_en;

    hi_drv_pq_hd_zme_coef_addr zme_coef_addr;
    hi_bool is_update; /* zme strategy update flag, 0: not update, 1: update */
} hi_drv_pq_hd_zme_strategy_out;

/* WBC VP */
typedef struct {
    hi_drv_pq_zme_common_out zme_common_out;
    hi_drv_pq_wbc_vp_band_point wbc_vp_point; /* wbc_vp point v0 or vp */
    hi_drv_pq_hor_draw_mul hor_draw_mul; /* Horizontal Extract Pixel */
    hi_drv_pq_ver_draw_mul ver_draw_mul; /* Vertical   Extract Line */
    hi_bool is_update; /* zme strategy update flag, 0: not update, 1: update */
} hi_drv_pq_wbc_vp_zme_strategy_out;

/* WBC DHD */
typedef struct {
    hi_drv_pq_zme_common_out zme_fmt;
    hi_drv_pq_wbc_dhd0_band_point dhd0_point;
    hi_drv_pq_hor_draw_mul hor_draw_mul; /* Horizontal Extract Pixel */
    hi_drv_pq_ver_draw_mul ver_draw_mul; /* Vertical   Extract Line */
    hi_bool is_update; /* zme strategy update flag, 0: not update, 1: update */
} hi_drv_pq_wbc_dhd0_zme_strategy_out;

typedef union {
    hi_drv_pq_hd_zme_strategy_out hd_zme_strategy;
    hi_drv_pq_wbc_vp_zme_strategy_out wbc_vp_zme_strategy;
    hi_drv_pq_wbc_dhd0_zme_strategy_out wbc_dhd0_zme_strategy;
} hi_drv_pq_zme_strategy_out;

/***************************************VPSS********************************************/
typedef struct {
    hi_u32 handle_id; /* vpss handle numble */

    hi_drv_pq_zme_common_in comm_info; /* zme common info, include W/H/FMT/... */

    hi_u32 frame_rate_in;  /* vpss in frame_rate *1000 */
    hi_u32 frame_rate_out; /* vpss out frame_rate *1000 */
} hi_drv_pq_vpsszme_in;

typedef struct {
    hi_drv_pq_zme_coef_addr zme_coef_addr;

    hi_bool is_update; /* vpss zme update flag, 0: not update, 1: update */
} hi_drv_pq_vpsszme_out;

typedef struct {
    hi_u32 zme_width_in;   /* zme  input frame width */
    hi_u32 zme_height_in;  /* zme  input frame height */
    hi_u32 zme_width_out;  /* zme output frame width */
    hi_u32 zme_height_out; /* zme output frame height */

    /* reduce tap for performance */
    hi_bool is_reduce_tap;
} hi_drv_pq_zme_coef_in;

typedef struct {
    hi_drv_pq_zme_coef_addr zme_coef_addr;

    hi_bool is_update; /* zme coef update flag, 0: not update, 1: update */
} hi_drv_pq_zme_coef_out;

/***************************************GFX********************************************/
typedef struct {
    hi_u32 zme_width_in;   /* zme input  frame width */
    hi_u32 zme_height_in;  /* zme input  frame height */
    hi_u32 zme_width_out;  /* zme output frame width */
    hi_u32 zme_height_out; /* zme output frame height */

    hi_drv_pq_frame_fmt zme_frame_in;
    hi_drv_pq_frame_fmt zme_frame_out;

    hi_bool is_deflicker; /* gfx deflicker flag, 8bit offset to 12bit */
    hi_bool is_ogeny;     /* 0-no isogeny; 1-isogeny */
} hi_drv_pq_gfx_zme_in;

typedef struct {
    hi_drv_pq_fir_mode zme_fir_h;
    hi_drv_pq_fir_mode zme_fir_hl;
    hi_drv_pq_fir_mode zme_fir_hc;
    hi_drv_pq_fir_mode zme_fir_ha;
    hi_drv_pq_fir_mode zme_fir_v;
    hi_drv_pq_fir_mode zme_fir_vl;
    hi_drv_pq_fir_mode zme_fir_vc;
    hi_drv_pq_fir_mode zme_fir_va;
} hi_drv_pq_gfx_zme_fir_mode;

typedef struct {
    hi_bool zme_med_h_en;
    hi_bool zme_med_hl_en;
    hi_bool zme_med_hc_en;
    hi_bool zme_med_ha_en;
    hi_bool zme_med_v_en;
    hi_bool zme_med_vl_en;
    hi_bool zme_med_vc_en;
    hi_bool zme_med_va_en;
} hi_drv_pq_gfx_zme_med_fir_en;

typedef struct {
    hi_u32 zme_ratio_h;
    hi_u32 zme_ratio_hl;
    hi_u32 zme_ratio_hc;
    hi_bool zme_ratio_ha;
    hi_u32 zme_ratio_v;
    hi_u32 zme_ratio_vl;
    hi_u32 zme_ratio_vc;
    hi_bool zme_ratio_va;
} hi_drv_pq_gfx_zme_ratio;

typedef struct {
    hi_u32 zme_width_in;
    hi_u32 zme_height_in;
    hi_u32 zme_width_out;
    hi_u32 zme_height_out;

    hi_drv_pq_zme_fir_en zme_fir_en;             /* filter  enable or diable */
    hi_drv_pq_gfx_zme_fir_mode zme_fir_mode;     /* use copy or fir mode */
    hi_drv_pq_gfx_zme_med_fir_en zme_med_fir_en; /* med filter enable or diable */
    hi_drv_pq_zme_offset zme_offset;
    hi_drv_pq_zme_order zme_order;
    hi_drv_pq_gfx_zme_ratio zme_ratio;

    hi_bool is_frame_out;  /* Frame format for zme input: 0-field; 1-frame */
    hi_bool is_bottom_out; /* Input field polar when input is field format: 0-top field; 1-bottom field */
} hi_drv_pq_gfx_zme_common_out;

typedef struct {
    hi_drv_pq_gfx_zme_common_out gfx_zme_common_out;
    hi_drv_pq_zme_coef_addr zme_coef_addr;

    hi_bool is_update; /* gfx zme coef update flag, 0: not update, 1: update */
} hi_drv_pq_gfx_zme_out;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* End of #ifndef __DRV_PQ_EXT_ZME_H__ */
