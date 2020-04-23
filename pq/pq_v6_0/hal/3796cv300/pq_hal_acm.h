/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: pq hal acm
 * Author: pq
 * Create: 2019-10-26
 */

#ifndef __PQ_HAL_LCACM_H__
#define __PQ_HAL_LCACM_H__

#include "hi_type.h"
#include "drv_pq_table.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define ACM_COEF_SIZE (8 * (264 + 256 + 264 + 256 + 168 + 160 + 132 + 128))

#define YHS_HS_A_LUT_LEN  264  /* 33 * 8 */
#define YHS_HS_B_LUT_LEN  256  /* 32 * 8 */
#define YHS_HS_C_LUT_LEN  264  /* 33 * 8 */
#define YHS_HS_D_LUT_LEN  256  /* 32 * 8 */
#define YHS_HY_A_LUT_LEN  168  /* 33 * 5 + 3 */
#define YHS_HY_B_LUT_LEN  160  /* 32 * 5 */
#define YHS_HY_C_LUT_LEN  132  /* 33 * 4 */
#define YHS_HY_D_LUT_LEN  128  /* 32 * 4 */

#define COLOR_H_NUM       65
#define COLOR_S_NUM       16
#define COLOR_Y_NUM        9

#define MAX_ACM_GAIN 1024

/** CNcomment:YHS */
typedef enum {
    PQ_COLOR_YHS_Y = 0,      /** Luma */ /** <CNcomment:Y 亮度 */
    PQ_COLOR_YHS_H,          /** Hue */ /** <CNcomment:H 色调 */
    PQ_COLOR_YHS_S,          /** Sat */ /** <CNcomment:S 饱和度 */
    PQ_COLOR_YHS_MAX        /** Invalid value */ /** <CNcomment:非法边界值 */
} pq_color_yhs;

/* 拼表系数类型 */
typedef struct {
    hi_u32 yhs_hs_lut_a[YHS_HS_A_LUT_LEN];
    hi_u32 yhs_hs_lut_b[YHS_HS_B_LUT_LEN];
    hi_u32 yhs_hs_lut_c[YHS_HS_C_LUT_LEN];
    hi_u32 yhs_hs_lut_d[YHS_HS_D_LUT_LEN];
    hi_u32 yhs_hy_lut_a[YHS_HY_A_LUT_LEN];
    hi_u32 yhs_hy_lut_b[YHS_HY_B_LUT_LEN];
    hi_u32 yhs_hy_lut_c[YHS_HY_C_LUT_LEN];
    hi_u32 yhs_hy_lut_d[YHS_HY_D_LUT_LEN];
} acm_convert_table;

/* ACM table结构 */
typedef struct {
    hi_u16 hue_gain;                                /* 表示对Hue的增益，范围0-1023 */
    hi_u16 sat_gain;                                /* 表示对Saturation的增益 */
    hi_u16 luma_gain;                               /* 表示对Luma的增益 */
    hi_s16 yhs_by_h[PQ_COLOR_YHS_MAX][COLOR_H_NUM]; /* YbyH[65],    HbyH[65],    SbyH[65]     delta  curve */
    /* YbyH[65][13],HbyH[65][13],SbyH[65][13] S gain curve */
    hi_s16 yhs_by_hs_gain[PQ_COLOR_YHS_MAX][COLOR_H_NUM][COLOR_S_NUM];
    /* YbyH[65][9], HbyH[65][9], SbyH[65][9]  Y gain curve */
    hi_s16 yhs_by_hy_gain[PQ_COLOR_YHS_MAX][COLOR_H_NUM][COLOR_Y_NUM];
} acm_effect_param;

typedef struct {
    hi_u32 size;
    hi_u8 *vir_addr;
    hi_u64 phy_addr;
} pq_acm_coef_addr;

/* acm mix table acm 转换表 */
typedef struct {
    /* yby_h[29][13], hby_h[29][13], sby_h[29][13] S gain curve */
    hi_s16 yhs_by_hs[PQ_COLOR_YHS_MAX][COLOR_H_NUM][COLOR_S_NUM];
    /* yby_h[29][9], hby_h[29][9], sby_h[29][9]  Y gain curve */
    hi_s16 yhs_by_hy[PQ_COLOR_YHS_MAX][COLOR_H_NUM][COLOR_Y_NUM];
} acm_split_lut;

hi_s32 pq_hal_acm_set_param(acm_effect_param *color_param_2d);

hi_s32 pq_hal_set_acm_enable(vdp_regs_type *vdp_reg, hi_bool on_off);

hi_s32 pq_hal_set_acm_link(vdp_regs_type *vdp_reg, pq_acm_link bind);

hi_s32 pq_hal_set_acm_cbcr_threshold(vdp_regs_type *vdp_reg, hi_u32 data);

hi_s32 pq_hal_set_acm_stretch(vdp_regs_type *vdp_reg, hi_u32 data);

hi_s32 pq_hal_set_acm_clip_range(vdp_regs_type *vdp_reg, hi_u32 data);

hi_s32 pq_hal_set_acm_gain(vdp_regs_type *vdp_reg, hi_u32 luma, hi_u32 hue, hi_u32 sat);

hi_s32 pq_hal_acm_distribute_addr(hi_u8 *vir_addr, dma_addr_t phy_addr);

hi_s32 pq_hal_acm_reset_addr(hi_void);

hi_s32 pq_hal_acm_set_demo_en(hi_bool on_off);

hi_s32 pq_hal_acm_set_demo_mode(pq_demo_mode mode);

hi_s32 pq_hal_acm_set_demo_pos(hi_u32 x_pos);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

