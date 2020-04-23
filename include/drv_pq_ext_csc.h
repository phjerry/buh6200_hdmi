/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: csc api for other module
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __DRV_PQ_EXT_CSC_H__
#define __DRV_PQ_EXT_CSC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/***************************************VDP********************************************/
typedef enum {
    HI_DRV_PQ_CSC_LAYER_V0 = 0,
    HI_DRV_PQ_CSC_LAYER_V1,
    HI_DRV_PQ_CSC_LAYER_V2,
    HI_DRV_PQ_CSC_LAYER_V3,
    HI_DRV_PQ_CSC_LAYER_V4,
    HI_DRV_PQ_CSC_LAYER_VPSS,

    HI_DRV_PQ_CSC_LAYER_MAX
} hi_drv_pq_csc_layer;

/* CSC tuning or no tuning; limit: there is only one csc needing tuning at every layer */
typedef enum {
    HI_PQ_CSC_TUNING_V0 = 0, /* tuning csc; need tune bright etc. */
    HI_PQ_CSC_TUNING_V1,
    HI_PQ_CSC_TUNING_V2,
    HI_PQ_CSC_TUNING_V3,
    HI_PQ_CSC_TUNING_V4,
    HI_PQ_CSC_TUNING_GP0,
    HI_PQ_CSC_TUNING_GP1,
    HI_PQ_CSC_NORMAL_SETTING, /* normal csc; only need coef */

    HI_PQ_CSC_TYPE_MAX
} hi_drv_pq_csc_type;

typedef struct {
    hi_bool csc_en;      /* Current Csc Enable: 1:Open; 0:Close */
    hi_u32 csc_pecision; /* Current Csc Pecision: 10~15 */
    hi_drv_pq_csc_type csc_type;

    hi_drv_color_descript csc_in;
    hi_drv_color_descript csc_out;
} hi_drv_pq_csc_info;

/* CSC matrix coefficient */
typedef struct {
    hi_s32 csc_coef00;
    hi_s32 csc_coef01;
    hi_s32 csc_coef02;

    hi_s32 csc_coef10;
    hi_s32 csc_coef11;
    hi_s32 csc_coef12;

    hi_s32 csc_coef20;
    hi_s32 csc_coef21;
    hi_s32 csc_coef22;
} hi_drv_pq_csc_ac_coef;

/* CSC DC component */
typedef struct {
    hi_s32 csc_in_dc0;
    hi_s32 csc_in_dc1;
    hi_s32 csc_in_dc2;

    hi_s32 csc_out_dc0;
    hi_s32 csc_out_dc1;
    hi_s32 csc_out_dc2;
} hi_drv_pq_csc_dc_coef;

/* csc coef */
typedef struct {
    hi_drv_pq_csc_ac_coef csc_ac_coef;
    hi_drv_pq_csc_dc_coef csc_dc_coef;

    hi_bool is_update; /* csc coef update flag, 0: not update, 1: update */
} hi_drv_pq_csc_coef;

/***************************************GFX********************************************/
typedef struct {
    hi_bool is_bgr_in;               /* Is BGR in */
    hi_drv_pq_csc_info gfx_csc_mode; /* input and output gfxcsc mode */
} hi_drv_pq_gfx_csc_info;

/* CSC matrix coefficient */
typedef struct {
    hi_s32 csc_coef00; /* member 00 of 3*3 matrix */
    hi_s32 csc_coef01; /* member 01 of 3*3 matrix */
    hi_s32 csc_coef02; /* member 02 of 3*3 matrix */

    hi_s32 csc_coef10; /* member 10 of 3*3 matrix */
    hi_s32 csc_coef11; /* member 11 of 3*3 matrix */
    hi_s32 csc_coef12; /* member 12 of 3*3 matrix */

    hi_s32 csc_coef20; /* member 20 of 3*3 matrix */
    hi_s32 csc_coef21; /* member 21 of 3*3 matrix */
    hi_s32 csc_coef22; /* member 22 of 3*3 matrix */
} hi_drv_pq_gfx_csc_ac_coef;

/* CSC DC component */
typedef struct {
    hi_s32 csc_in_dc0; /* input color space DC value of component 0; */
    hi_s32 csc_in_dc1; /* input color space DC value of component 1; */
    hi_s32 csc_in_dc2; /* input color space DC value of component 2; */

    hi_s32 csc_out_dc0; /* output color space DC value of component 0; */
    hi_s32 csc_out_dc1; /* output color space DC value of component 1; */
    hi_s32 csc_out_dc2; /* output color space DC value of component 2; */
} hi_drv_pq_gfx_csc_dc_coef;

typedef struct {
    hi_drv_pq_gfx_csc_ac_coef gfx_ac_coef;
    hi_drv_pq_gfx_csc_dc_coef gfx_dc_coef;

    hi_bool is_update; /* gfx csc coef update flag, 0: not update, 1: update */
} hi_drv_pq_gfx_csc_coef;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
