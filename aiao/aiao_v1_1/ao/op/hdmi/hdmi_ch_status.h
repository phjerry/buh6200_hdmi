/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao hdmi header file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __HDMI_CH_STATUS_H__
#define __HDMI_CH_STATUS_H__

#include "audio_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* define the union hdmi_ch_status_0 */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32 use_a    : 1;   /* [0] */
        hi_u32 use_b    : 1;   /* [1] */
        hi_u32 use_c    : 1;   /* [2] */
        hi_u32 use_d    : 3;   /* [5..3] */
        hi_u32 use_mode : 2;   /* [7..6] */
        hi_u32 reserved : 24;  /* [31..8] */
    } bits;

    /* define an hi_u32 member */
    hi_u32 u32;
} hdmi_ch_status_0;

/* define the union hdmi_ch_status_1 */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32 category_code : 8;  /* [7..0] */
        hi_u32 reserved      : 24; /* [31..8] */
    } bits;

    /* define an hi_u32 member */
    hi_u32 u32;
} hdmi_ch_status_1;

/* define the union hdmi_ch_status_2 */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32 src_nb   : 4;   /* [3..0] */
        hi_u32 ch_nb    : 4;   /* [7..4] */
        hi_u32 reserved : 24;  /* [31..8] */
    } bits;

    /* define an hi_u32 member */
    hi_u32 u32;
} hdmi_ch_status_2;

/* define the union U_HDMI_CFG_3 */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32 freq         : 4;  /* [3..0] */
        hi_u32 clk_accuracy : 2;  /* [5..4] */
        hi_u32 reserved     : 26; /* [31..6] */
    } bits;

    /* define an hi_u32 member */
    hi_u32 u32;
} hdmi_ch_status_3;

/* define the union hdmi_ch_status_4 */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32 bit_max   : 1;  /* [0] */
        hi_u32 bit_width : 3;  /* [3..1] */
        hi_u32 org_freq  : 4;  /* [7..4] */
        hi_u32 reserved  : 24; /* [31..8] */
    } bits;

    /* define an hi_u32 member */
    hi_u32 u32;
} hdmi_ch_status_4;

/* define the union hdmi_ch_status_5 */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32 cgms_a   : 2;   /* [1..0] */
        hi_u32 reserved : 30;  /* [31..2] */
    } bits;

    /* define an hi_u32 member */
    hi_u32 u32;
} hdmi_ch_status_5;

typedef struct {
    hdmi_ch_status_0 ch_status_0;
    hdmi_ch_status_1 ch_status_1;
    hdmi_ch_status_2 ch_status_2;
    hdmi_ch_status_3 ch_status_3;
    hdmi_ch_status_4 ch_status_4;
    hdmi_ch_status_5 ch_status_5;
} hdmi_ch_status;

hi_void hdmi_ch_status_init(hdmi_ao_attr *ao_attr, hdmi_ch_status *ch_status);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

