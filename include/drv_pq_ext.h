/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: export api for other module
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __DRV_PQ_EXT_H__
#define __DRV_PQ_EXT_H__

#include "hi_type.h"
#include "hi_drv_color.h"
#include "hi_drv_video.h"
#include "drv_pq_ext_hdr.h"
#include "drv_pq_ext_zme.h"
#include "drv_pq_ext_csc.h"
#include "hi_drv_dev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define PQ_AI_AREA_NUM_MAX 100

/***************************************Common********************************************/
/* AI result from NPU */
/* AI AREA TYPE */
typedef enum {
    HI_DRV_PQ_AI_CHECK_FACE = 0, /* face area */
    HI_DRV_PQ_AI_CHECK_WORD,     /* word area */
    HI_DRV_PQ_AI_CHECK_SCENE,    /* scene */

    HI_DRV_PQ_AI_CHECK_MAX
} hi_drv_pq_ai_check_type;

/* AI SCENSE TYPE */
typedef enum {
    HI_DRV_PQ_AI_SCENE_RAINSNOW = 0, /* rain and snow scene */
    HI_DRV_PQ_AI_SCENE_LANDSCAPE,    /* landscape scene */
    HI_DRV_PQ_AI_SCENE_FOOD,         /* food scene */

    HI_DRV_PQ_AI_SCENE_MAX
} hi_drv_pq_ai_scense_type;

typedef struct {
    hi_u16 x0;
    hi_u16 y0;
    hi_u16 x1;
    hi_u16 y1;
    hi_u16 alpha;
} hi_drv_pq_ai_area_info;

typedef struct {
    hi_u16 alpha[HI_DRV_PQ_AI_SCENE_MAX];
} hi_drv_pq_ai_scene_result;

typedef struct {
    hi_u32 area_num;                                         /* ai check result area number */
    hi_drv_pq_ai_area_info ai_area_info[PQ_AI_AREA_NUM_MAX]; /* ai every area check result */
} hi_drv_pq_ai_area_result;

typedef struct {
    hi_bool ai_check_en[HI_DRV_PQ_AI_CHECK_MAX]; /* check enable or disable */

    hi_drv_pq_ai_area_result ai_face_result; /* ai face area check result */

    hi_drv_pq_ai_area_result ai_word_result; /* ai word area check result */

    hi_drv_pq_ai_scene_result ai_scene_result; /* ai scene check result */
} hi_drv_pq_ai_result;
/***************************************VDP********************************************/
/* display ID */
typedef enum {
    HI_DRV_PQ_DISPLAY_0 = 0, /* < DISPLAY0, SD */
    HI_DRV_PQ_DISPLAY_1,     /* < DISPLAY1, HD */

    HI_DRV_PQ_DISPLAY_MAX
} hi_drv_pq_display_id;

/* Layer ID */
typedef enum {
    HI_DRV_PQ_VDP_LAYER_VID0 = 0, /* VDP_LAYER_V0 */
    HI_DRV_PQ_VDP_LAYER_VID1,     /* VDP_LAYER_V1 */
    HI_DRV_PQ_VDP_LAYER_VID2,     /* VDP_LAYER_V2 */
    HI_DRV_PQ_VDP_LAYER_VID3,     /* VDP_LAYER_V3 */
    HI_DRV_PQ_VDP_LAYER_VID4,     /* VDP_LAYER_V4 */

    HI_DRV_PQ_VDP_LAYER_VID_MAX
} hi_drv_pq_vdp_layer_id;

typedef struct {
    hi_u32 frame_width;
    hi_u32 frame_height;
    hi_u32 frame_rate;
    hi_u32 bit_width;

    hi_drv_pq_frame_fmt frame_fmt_in; /* frame or field */
    hi_bool is_ogeny_mode;            /* support SD writer back from HD */
    hi_bool is_part_update;           /* update partly PQ para */

    hi_drv_hdr_type hdr_type_in;  /* SDR/HDR/.... */
    hi_drv_hdr_type hdr_type_out; /* SDR/HDR/.... */
} hi_drv_pq_vdp_frame_info;

/* fmt information */
typedef struct {
    hi_drv_rect fmt_rect;
    hi_drv_rect video_rect;
    hi_u32 refresh_rate;
    hi_drv_pq_frame_fmt frame_fmt;
    hi_drv_color_descript color_space_out;
    hi_drv_hdr_type disp_hdr_type;
} hi_drv_pq_vdp_channel_timing;

/* VDP input info */
typedef struct {
    hi_drv_pq_vdp_layer_id layer_id; /* V0/V1/V2/.... */

    hi_drv_pq_vdp_frame_info frame_info;
    hi_drv_pq_vdp_channel_timing channel_timing[HI_DRV_PQ_DISPLAY_MAX];
} hi_drv_pq_vdp_info;

/***************************************VPSS********************************************/
typedef enum {
    HI_DRV_PQ_SCENE_MODE_NORMAL = 0,
    HI_DRV_PQ_SCENE_MODE_PREVIEW,
    HI_DRV_PQ_SCENE_MODE_REMOTE,

    HI_DRV_PQ_SCENE_MODE_MAX
} hi_drv_pq_scene_mode;

typedef struct {
    hi_drv_pq_scene_mode scene_mode;
} hi_drv_pq_vpss_scene_mode;

/* VPSS input info */
typedef struct {
    hi_u32 handle_id; /* vpss handle numble */
    hi_u32 frame_width;
    hi_u32 frame_height;
    hi_u32 frame_rate;
    hi_drv_pq_frame_fmt frame_fmt;

    hi_drv_hdr_type hdr_type_in;  /* SDR/HDR/.... */
    hi_drv_hdr_type hdr_type_out; /* SDR/HDR/.... */
} hi_drv_pq_vpss_info;

typedef enum {
    HI_DRV_PQ_FIELD_TOP_FIRST = 0,
    HI_DRV_PQ_FIELD_BOTTOM_FIRST,
    HI_DRV_PQ_FIELD_UNKOWN,

    HI_DRV_PQ_FIELD_MAX
} hi_drv_pq_vpss_field_order;

typedef struct {
    hi_s32 is_progressive_seq;
    hi_s32 frame_rate;
    hi_drv_pq_frame_fmt frame_fmt;
    hi_drv_pq_vpss_field_order field_order;
} hi_drv_pq_vdec_info;

/* vpss module enble info */
typedef struct {
    hi_bool     nr_en;
    hi_bool     dei_en;
    hi_bool     ifmd_en;
    hi_bool     db_en;
    hi_bool     dm_en;
} hi_drv_pq_vpss_alg_enable;

/* VPSS input info */
typedef struct {
    /* Common */
    hi_u32 frame_index;
    hi_u32 handle_id;
    hi_u32 bit_width; /* 8bit / 10bit */

    hi_u32 width;  /* input width */
    hi_u32 height;  /* input height */

    hi_drv_pq_frame_fmt frame_fmt;

    hi_drv_pq_vpss_alg_enable alg_en;

    hi_u32 stream_id; /* special stream id */

    hi_void *vpss_stt_reg; /* ifmd , mcnr and dbm stt info, pq transfer vpss_stt_reg_type process */

    /* GlobalMotion */
    hi_u32 scd;
    hi_u32 stride;
    hi_void *rgmv_reg_addr;

    hi_void *nr_stt_reg; /* 3DRS_NR */

    /* vpss ddr addr for dei and fmd */
    hi_void *scan_stt_reg;

    /* IFMD */
    hi_u32 frame_rate;
    hi_bool pre_info;              /* if DEI logic timeout,  process last field */
    hi_drv_pq_vdec_info vdec_info; /* passive or interlace info from VDEC */

    /* FOD */
    hi_s32 ref_field; /* refference field, used by fod */
} hi_drv_pq_vpss_stt_info;

typedef struct {
    hi_s32 dir_mch;
    hi_s32 die_out_sel;

    hi_drv_pq_vpss_field_order field_order; /* top field first or bottom field first */

    hi_s32 film_mode;              /* film mode : 2:2 or 2:3 */
    hi_u32 key_frame;
} hi_drv_pq_ifmd_playback;

/* PQ output and pass to vpss info */
typedef struct {
    hi_drv_pq_ifmd_playback ifmd_playback;
} hi_drv_pq_vpss_cfg_info;

typedef enum {
    HI_DRV_PQ_VPSS_LAYER_PORT0 = 0,
    HI_DRV_PQ_VPSS_LAYER_PORT1,
    HI_DRV_PQ_VPSS_LAYER_PORT2,

    HI_DRV_PQ_VPSS_LAYER_MAX
} hi_drv_pq_vpss_layer;

/***************************************GFX********************************************/
typedef enum {
    HI_DRV_PQ_GFX_LAYER_GP0 = 0,
    HI_DRV_PQ_GFX_LAYER_GP1,
    HI_DRV_PQ_GFX_LAYER_WBC_GP,

    HI_DRV_PQ_GFX_LAYER_MAX
} hi_drv_pq_gfx_layer;

typedef enum {
    HI_DRV_PQ_HWC_LAYER_0 = 0,
    HI_DRV_PQ_HWC_LAYER_1,
    HI_DRV_PQ_HWC_LAYER_2,
    HI_DRV_PQ_HWC_LAYER_3,
    HI_DRV_PQ_HWC_LAYER_4,
    HI_DRV_PQ_HWC_LAYER_5,
    HI_DRV_PQ_HWC_LAYER_6,
    HI_DRV_PQ_HWC_LAYER_7,
    HI_DRV_PQ_HWC_LAYER_8,
    HI_DRV_PQ_HWC_LAYER_9,

    HI_DRV_PQ_HWC_LAYER_MAX
} hi_drv_pq_hwc_layer;

/****************************************EXPORT FUNC*******************************************/
hi_s32 pq_drv_mod_init(hi_void);
hi_void pq_drv_module_exit(hi_void);
typedef hi_s32 (*fn_pq_init)(hi_void);
typedef hi_s32 (*fn_pq_deinit)(hi_void);
typedef hi_s32 (*fn_pq_suspend)(hi_void *dev);
typedef hi_s32 (*fn_pq_resume)(hi_void *dev);

typedef hi_s32 (*fn_pq_update_vdp_stt_info)(hi_void);
typedef hi_s32 (*fn_pq_update_vdp_alg)(hi_drv_pq_display_id display_id, hi_drv_pq_vdp_info *vdp_info);
typedef hi_s32 (*fn_pq_get_vdp_csc_coef)(hi_drv_pq_csc_layer layer_id, hi_drv_pq_csc_info csc_info,
                                         hi_drv_pq_csc_coef *csc_coef);
typedef hi_s32 (*fn_pq_get_vdp_zme_strategy)(hi_drv_pq_zme_layer layer_id, hi_drv_pq_zme_strategy_in *zme_in,
                                             hi_drv_pq_zme_strategy_out *zme_out);
typedef hi_s32 (*fn_pq_get_vdp_zme_coef)(hi_drv_pq_zme_coef_in zme_coef_in,
                                         hi_drv_pq_zme_coef_out *zme_coef_out);
typedef hi_s32 (*fn_pq_set_vdp_hdr_cfg)(hi_drv_pq_xdr_layer_id layer_id,
                                        hi_drv_pq_xdr_frame_info *xdr_frame_info);
typedef hi_s32 (*fn_pq_set_default_param)(hi_bool is_default);
typedef hi_s32 (*fn_pq_update_vdp_ai_result)(hi_drv_pq_display_id display_id, hi_drv_pq_ai_result *ai_result);

typedef hi_s32 (*fn_pq_init_vpss_alg)(hi_drv_pq_vpss_scene_mode scence_mode, hi_drv_pq_vpss_info *vpss_src_info,
                                      hi_void *vpss_reg);
typedef hi_s32 (*fn_pq_update_vpss_stt_info)(hi_drv_pq_vpss_stt_info *vpss_stt_info);
typedef hi_s32 (*fn_pq_update_vpss_alg)(hi_drv_pq_vpss_stt_info *vpss_stt_info,
                                        hi_drv_pq_vpss_cfg_info *vpss_cfg_info);
typedef hi_s32 (*fn_pq_get_vpss_zme_coef)(hi_drv_pq_vpss_layer layer_id, hi_drv_pq_vpsszme_in *zme_in,
                                          hi_drv_pq_vpsszme_out *zme_out);
typedef hi_s32 (*fn_pq_set_vpss_hdr_cfg)(hi_drv_pq_vpss_layer layer_id, hi_drv_pq_xdr_frame_info *xdr_frame_info,
                                         hi_void *vpss_reg);
typedef hi_s32 (*fn_pq_update_vpss_ai_result)(hi_drv_pq_vpss_layer layer_id, hi_drv_pq_ai_result *ai_result);

typedef hi_s32 (*fn_pq_get_gfx_csc_coef)(hi_drv_pq_gfx_layer gfx_layer, hi_drv_pq_gfx_csc_info *gfx_csc_info,
                                         hi_drv_pq_gfx_csc_coef *gfx_csc_coef);
typedef hi_s32 (*fn_pq_get_gfx_zme_strategy)(hi_drv_pq_gfx_layer gfx_layer, hi_drv_pq_gfx_zme_in *gfx_zme_in,
                                             hi_drv_pq_gfx_zme_out *gfx_zme_out);
typedef hi_s32 (*fn_pq_set_gfx_hdr_cfg)(hi_drv_pq_gfx_layer gfx_layer, hi_drv_pq_gfx_hdr_info *gfx_hdr_info);

typedef hi_s32 (*fn_pq_get_hwc_hdr_cfg)(hi_drv_pq_hwc_layer hwc_layer, hi_drv_pq_hwc_hdr_info* hwc_hdr_info,
                                        hi_drv_pq_hwc_hdr_cfg* hwc_hdr_cfg);

typedef struct {
    fn_pq_init pq_init;
    fn_pq_deinit pq_deinit;
    fn_pq_suspend pq_suspend;
    fn_pq_resume pq_resume;

    fn_pq_update_vdp_stt_info pq_update_vdp_stt_info;
    fn_pq_update_vdp_alg pq_update_vdp_alg;
    fn_pq_get_vdp_csc_coef pq_get_vdp_csc_coef;
    fn_pq_get_vdp_zme_strategy pq_get_vdp_zme_strategy;
    fn_pq_get_vdp_zme_coef pq_get_vdp_zme_coef;
    fn_pq_set_vdp_hdr_cfg pq_set_vdp_hdr_cfg;
    fn_pq_set_default_param pq_set_default_param;
    fn_pq_update_vdp_ai_result pq_update_vdp_ai_result;

    fn_pq_init_vpss_alg pq_init_vpss_alg;
    fn_pq_update_vpss_stt_info pq_update_vpss_stt_info;
    fn_pq_update_vpss_alg pq_update_vpss_alg;
    fn_pq_get_vpss_zme_coef pq_get_vpss_zme_coef;
    fn_pq_set_vpss_hdr_cfg pq_set_vpss_hdr_cfg;
    fn_pq_update_vpss_ai_result pq_update_vpss_ai_result;

    fn_pq_get_gfx_csc_coef pq_get_gfx_csc_coef;
    fn_pq_get_gfx_zme_strategy pq_get_gfx_zme_strategy;
    fn_pq_set_gfx_hdr_cfg pq_set_gfx_hdr_cfg;

    fn_pq_get_hwc_hdr_cfg pq_get_hwc_hdr_cfg;
} pq_export_func;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* End of #ifndef __DRV_PQ_EXT_H__ */
