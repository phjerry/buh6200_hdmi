/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: vpss reg scan stt head file
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#ifndef __VPSS_REG_SCAN_STT_H__
#define __VPSS_REG_SCAN_STT_H__

/* Define the union U_VPSS_ME_GLBMV_HIST_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int me_glbmv0_mvx : 8;  /* [7..0] */
        unsigned int me_glbmv0_mvy : 7;  /* [14..8] */
        unsigned int me_glbmv0_num : 17; /* [31..15] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vpss_me_glbmv_hist_0;

/* Define the union U_VPSS_ME_GLBMV_HIST_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int me_glbmv1_mvx : 8;  /* [7..0] */
        unsigned int me_glbmv1_mvy : 7;  /* [14..8] */
        unsigned int me_glbmv1_num : 17; /* [31..15] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vpss_me_glbmv_hist_1;

/* Define the union U_VPSS_ME_GLBMV_HIST_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int me_glbmv_goodmv_cnt : 16; /* [15..0] */
        unsigned int blk_mv_hist_num_0 : 16;   /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vpss_me_glbmv_hist_2;

/* Define the union U_VPSS_ME_GLBMV_HIST_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int blk_mv_hist_num_1 : 16; /* [15..0] */
        unsigned int blk_mv_hist_num_2 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vpss_me_glbmv_hist_3;

/* Define the union U_VPSS_ME_GLBMV_HIST_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int blk_mv_hist_num_3 : 16; /* [15..0] */
        unsigned int blk_mv_hist_num_4 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vpss_me_glbmv_hist_4;

/* Define the union U_VPSS_ME_GLBMV_HIST_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vpss_me_glbmv_hist_5;
/* Define the union U_VPSS_ME_GLBMV_HIST_6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vpss_me_glbmv_hist_6;
/* Define the union U_VPSS_ME_GLBMV_HIST_7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vpss_me_glbmv_hist_7;
// ==============================================================================
/* Define the global struct */
typedef struct {
    u_vpss_me_glbmv_hist_0 vpss_me_glbmv_hist_0; /* 0x0 */
    u_vpss_me_glbmv_hist_1 vpss_me_glbmv_hist_1; /* 0x4 */
    u_vpss_me_glbmv_hist_2 vpss_me_glbmv_hist_2; /* 0x8 */
    u_vpss_me_glbmv_hist_3 vpss_me_glbmv_hist_3; /* 0xc */
    u_vpss_me_glbmv_hist_4 vpss_me_glbmv_hist_4; /* 0x10 */
    u_vpss_me_glbmv_hist_5 vpss_me_glbmv_hist_5; /* 0x14 */
    u_vpss_me_glbmv_hist_6 vpss_me_glbmv_hist_6; /* 0x18 */
    u_vpss_me_glbmv_hist_7 vpss_me_glbmv_hist_7; /* 0x1c */
} vpss_scan_stt_reg_type;

#endif /* __VPSS_REG_SCAN_STT_H__ */
