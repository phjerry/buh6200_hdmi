/******************************************************************************
*
* copyright (C) 2014-2015 hisilicon technologies co., ltd.  all rights reserved.
*
* this program is confidential and proprietary to hisilicon  technologies co., ltd. (hisilicon),
*  and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of hisilicon.
*
*****************************************************************************

  file name     : pq_hal_csc.h
  version       : initial draft
  author        : sdk
  created       : 2013/09/18
  description   :

******************************************************************************/

#ifndef __PQ_HAL_CSC_H__
#define __PQ_HAL_CSC_H__

#include "hi_debug.h"
#include "hi_type.h"
#include "hi_drv_pq.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 色彩空间标准 */
typedef enum {
    OPTM_CS_EUNKNOWN = 0,
    OPTM_CS_EITU_R_BT_709 = 1,
    OPTM_CS_EFCC = 4,
    OPTM_CS_EITU_R_BT_470_2_BG = 5,
    OPTM_CS_ESMPTE_170M = 6,
    OPTM_CS_ESMPTE_240M = 7,
    OPTM_CS_EXVYCC_709 = OPTM_CS_EITU_R_BT_709,
    OPTM_CS_EXVYCC_601 = 8,
    OPTM_CS_ERGB = 9,

    OPTM_CS_MAX
} color_space_type;

typedef enum {
    PQ_CSC_RANGE_EUNKNOWN = 0,
    PQ_CSC_RANGE_FULL = 1,
    PQ_CSC_RANGE_LMTD = 2,
    PQ_CSC_RANGE_MAX
} pq_csc_range;

typedef struct {
    color_space_type cs_type;
    pq_csc_range cs_range;
} pq_csc;

typedef struct {
    pq_csc csc_in;
    pq_csc csc_out;
} pq_csc_mode;

/* 色温通道 */
typedef enum {
    COLORTEMP_R = 0,
    COLORTEMP_G = 1,
    COLORTEMP_B = 2,

    COLORTEMP_ALL = 3
} colortemp_channel;

typedef struct {
    hi_u32 red_gain;
    hi_u32 green_gain;
    hi_u32 blue_gain;
} csc_color_temp;

/* CSC 参数结构 */
typedef struct {
    hi_pq_image_param image_para;
    csc_color_temp color_temp;
    hi_u32 color_lut_temp_offset[COLORTEMP_ALL];
    color_space_type in_color_type;
    color_space_type out_color_type;
    hi_bool full_range;
} csc_para;

typedef enum {
    WBC_POINT_AFTER_CSC = 0, /* 00：回写点在VP CSC后，数据格式为YUV444 */
    WBC_POINT_AFTER_ZME,     /* 01：回写点在V0 ZME后，数据格式为YUV422 */
    WBC_POINT_BEFORE_ZME,    /* 10：回写点在V0 ZME前，数据格式为YUV420或YUV422 */

    WBC_POINT_MAX
} wbc_point_sel;

hi_s32 pq_hal_get_wbc_point_sel(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
