/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: pq mng acm
 * Author: pq
 * Create: 2019-10-26
 */

#ifndef __PQ_MNG_LCACM_H__
#define __PQ_MNG_LCACM_H__

#include "drv_pq_table.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define acm_clip(a, b)                \
    do {                                 \
        if ((a) > 0) {                   \
            (a) = (a) > (b) ? (b) : (a);   \
        } else {                         \
            (a)  = (a) < (-(b)) ? (-(b)) : (a); \
        }                                \
    } while (0)

typedef struct
{
    hi_bool is_init;
    hi_bool enable;
    hi_u32  strength;
    hi_bool demo_en;
    pq_demo_mode demo_mode;
    hi_u32 demo_pos;
    hi_drv_hdr_type hdr_type_in;  /* SDR/HDR/.... */
    hi_drv_hdr_type hdr_type_out; /* SDR/HDR/.... */
} pq_acm_info;

/* 六基色映射表 */
typedef struct {
    hi_u32 hue_start;
    hi_u32 hue_end;
} hue_range;

/* 界面色度调节分段 */
typedef enum {
    RED = 0,      /* red */
    GREEN,    /* green */
    BLUE1,    /* blue */
    BLUE2,    /* blue */
    CYAN,     /* cyan */
    MAGENTA,  /* magenta */
    YELLOW,   /* yellow */
    FLESH,    /* flesh */
    COLOR_SEGMENTATION_MAX
} color_segmentation;

hi_s32 pq_mng_register_acm(pq_reg_type type);
hi_s32 pq_mng_unregister_acm(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

