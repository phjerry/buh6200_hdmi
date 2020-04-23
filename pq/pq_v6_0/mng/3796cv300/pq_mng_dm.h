/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: pq mng dm api
 * Author: pq
 * Create: 2019-01-1
 */

#ifndef __PQ_MNG_DM_H__
#define __PQ_MNG_DM_H__

#include "pq_hal_dm.h"
#include "pq_hal_comm.h"
#include "drv_pq_table.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_s32 depth;
    hi_s32 y_width;
    hi_s32 y_height;
    hi_s32 y_stride;
    hi_u16 *y_in_buf;
    hi_u16 *y_out_buf;
    hi_u16 *u_in_buf;
    hi_u16 *u_out_buf;
    hi_u16 *in_buf;
    hi_u16 *out_buf;

    hi_s32 dm_en;              /* u1,[0,1];   default value:1 */

    hi_u8 grad_sub_ratio;      /* u5,[0,31]; default value:24 */
    hi_u8 ctrst_thresh;        /* u2,[0,3]; default value:2 */
    hi_u8 const * dir_str_gain_lut;
    hi_u8 const * dir_str_lut;
    /* original codes */
    hi_u8 const * sw_wht_lut;   /* u7,[0,127]; default value @ hi_s32 DM_InitReg(DM_INF_S *pInf) function */

    hi_u8 dm_global_str;        /* u4,[0,15]; default value:8 */
    /* original codes */
    hi_u16 limit_t;             /* u8,[0,255]; default value:18 */
    hi_u16 limit_t10bit;        /* u10,[0,1023]; default value:72 */

    hi_u8 opp_ang_ctrst_div;    /* u2,[1,3]; default value:2 */
    hi_u16 opp_ang_ctrst_t;     /* u8,[0,255]; default value:20 */

    /* original codes */
    hi_u16 mn_dir_opp_ctrst_t;  /* u8,[0,255]; default value:18 */
    hi_u8 dir_blend_str;        /* u4,[0,8]; default value:7; */

    hi_u16 lw_ctrst_t;          /* u8,[0,255]; default value:30 */
    hi_u16 lw_ctrst_t10bit;     /* u10,[0,1023]; default value:120 */
    hi_u16 sw_trsnt_lt;         /* u8,[0,255]; default value:40 */
    hi_u16 sw_trsnt_lt10bit;    /* u10,[0,1023]; default value:160 */
    hi_u16 sw_trsnt_st;         /* u8,[0,255]; default value:0 */
    hi_u16 sw_trsnt_st10bit;    /* u10,[0,1023]; default value:0 */

    /* original codes */
    /* SD big window process mode: 0: interlace mode; 1: progressive mode */
    /* for MMF limit */
    hi_s16 mmflr;       /* s9,[-256,255]; default value:50; MMFLR > MMFSR */
    hi_s16 mmflr10bit;  /* s11,[-1024,1023]; default value:200; MMFLR10bit > MMFSR10bit */
    hi_s16 mmfsr;       /* s9,[-256,255]; default value:0 */
    hi_s16 mmfsr10bit;  /* s11,[-1024,1023]; default value:0 */

    hi_u8 mm_flimit_en; /* u1,[0,1]; default value:1; 1: enable the MMF limit condition; 0: disable */
    /* test */
    hi_u8 mmf_set;      /* u1,[0,1]; default value:0; 0:MMF[0,3], 1:MMF[1,2] */

    hi_u8 lsw_ratio;           /* u3,[2,6]; default value:4 */
    hi_u8 limit_lsw_ratio;     /* u3,[2,6]; default value:4 */

    /* for MMF filtering blending */
    hi_u8 lim_res_blend_str1;  /* u4,[0,8]; default value:7; */
    hi_u8 lim_res_blend_str2;  /* u4,[0,8]; default value:7; */

    /* for test */
    hi_u8 init_val_step;       /* u2,[0,3]; default value:0 */

    /* test */
    hi_u16 const * trans_band; /* u6,[0,63]; default value @ hi_s32 DM_InitReg(DM_INF_S *pInf) function */
} dm_param;

typedef struct {
    hi_bool init;
    hi_bool enable;
    hi_u32 strength;
    hi_bool demo_enable;
} alg_dm;

hi_s32 pq_mng_register_dm(pq_reg_type type);
hi_s32 pq_mng_unregister_dm(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
