/******************************************************************************

  copyright (C), 2012-2018, hisilicon tech. co., ltd.

******************************************************************************
  file name     : pq_mng_csc.h
  version       : initial draft
  author        : sdk
  created       : 2013/10/15
  description   :

******************************************************************************/

#ifndef __PQ_MNG_CSC_H__
#define __PQ_MNG_CSC_H__

#include "pq_hal_csc.h"
#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* 窗口类型 */
typedef enum {
    CSC_WINDOW_MAIN = 0,
    CSC_WINDOW_SUB,

    CSC_WINDOW_MAX
} csc_window;

/* 色温设定 */
typedef struct {
    hi_s16 red_gain;
    hi_s16 green_gain;
    hi_s16 blue_gain;
    hi_s16 red_offset;
    hi_s16 green_offset;
    hi_s16 blue_offset;
} color_temperature;

/* 色彩空间设定 */
typedef struct {
    color_space_type input_color_space;  /* 输入色彩空间 */
    color_space_type output_color_space; /* 输出色彩空间 */
    hi_bool full_range;                  /* 0:limit,1:full range */
} color_space;

hi_s32 pq_mng_register_csc(pq_reg_type type);

hi_s32 pq_mng_unregister_csc(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
