/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */

#ifndef __C_UNION_DEFINE_FUNC_H__
#define __C_UNION_DEFINE_FUNC_H__

/* Define the union func_vcpi_intstat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_int_ve_eop       : 1;     /* [0] */
        unsigned int vcpi_int_vedu_slice_end : 1;   /* [1] */
        unsigned int vcpi_int_ve_buffull   : 1;     /* [2] */
        unsigned int vcpi_int_ve_pbitsover : 1;     /* [3] */
        unsigned int vcpi_int_axi_rdbus_err : 1;    /* [4] */
        unsigned int vcpi_int_axi_wrbus_err : 1;    /* [5] */
        unsigned int vcpi_int_vedu_timeout : 1;     /* [6] */
        unsigned int vcpi_int_linenum_end  : 1;     /* [7] */
        unsigned int vcpi_int_sed_err_core : 1;     /* [8] */
        unsigned int vcpi_int_sed_err_ext  : 1;     /* [9] */
        unsigned int reserved_0            : 22;    /* [31..10] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vcpi_intstat;

/* Define the union func_vcpi_rawint */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_rint_ve_eop      : 1;     /* [0] */
        unsigned int vcpi_rint_vedu_slice_end : 1;  /* [1] */
        unsigned int vcpi_rint_ve_buffull  : 1;     /* [2] */
        unsigned int vcpi_rint_ve_pbitsover : 1;    /* [3] */
        unsigned int vcpi_rint_axi_rdbus_err : 1;   /* [4] */
        unsigned int vcpi_rint_axi_wrbus_err : 1;   /* [5] */
        unsigned int vcpi_rint_vedu_timeout : 1;    /* [6] */
        unsigned int vcpi_rint_linenum_end : 1;     /* [7] */
        unsigned int vcpi_rint_sed_err_core : 1;    /* [8] */
        unsigned int vcpi_rint_sed_err_ext : 1 ;    /* [9] */
        unsigned int reserved_0            : 22;    /* [31..10] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vcpi_rawint;

/* Define the union func_vcpi_bus_idle_flag */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bus_idle_flag         : 1;     /* [0] */
        unsigned int reserved_0            : 31;    /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vcpi_bus_idle_flag;

/* Define the union func_pme_madi_sum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_madi_sum          : 27;    /* [26..0] */
        unsigned int reserved_0            : 5;     /* [31..27] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_pme_madi_sum;

/* Define the union func_pme_madp_sum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_madp_sum          : 25;    /* [24..0] */
        unsigned int reserved_0            : 7;     /* [31..25] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_pme_madp_sum;

/* Define the union func_pme_madi_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_madi_num          : 19;    /* [18..0] */
        unsigned int reserved_0            : 13;    /* [31..19] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_pme_madi_num;

/* Define the union func_pme_madp_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_madp_num          : 17;    /* [16..0] */
        unsigned int reserved_0            : 15;    /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_func_pme_madp_num;

/* Define the union func_pme_large_sad_sum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int large_sad_sum         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_pme_large_sad_sum;

/* Define the union func_pme_low_luma_sum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    low_luma_sum          : 21; /* [20..0] */
        unsigned int    reserved_0            : 11; /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_func_pme_low_luma_sum;

/* Define the union func_pme_chroma_scene_sum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int chroma_prot_sum       : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_pme_chroma_scene_sum;

/* Define the union func_pme_move_scene_sum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int move_scene_sum        : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_pme_move_scene_sum;

/* Define the union func_pme_skin_region_sum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int skin_region_sum       : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_pme_skin_region_sum;

/* Define the union func_bgstr_block_count */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_bggen_block_count : 18;   /* [17..0] */
        unsigned int reserved_0            : 14;    /* [31..18] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_bgstr_block_count;

/* Define the union func_bgstr_frame_bgm_dist */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_frame_bgm_dist   : 31;    /* [30..0] */
        unsigned int reserved_0            : 1;     /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_bgstr_frame_bgm_dist;

/* Define the union func_vlcst_dsrptr00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len0              : 29;    /* [28..0] */
        unsigned int reserved_0            : 3;     /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr00;

/* Define the union func_vlcst_dsrptr01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum0           : 7;     /* [6..0] */
        unsigned int reserved_0            : 9;     /* [15..7] */
        unsigned int slice_type0           : 2;     /* [17..16] */
        unsigned int reserved_1            : 13;    /* [30..18] */
        unsigned int islastslc0            : 1;     /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr01;

/* Define the union func_vlcst_dsrptr10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len1              : 29;    /* [28..0] */
        unsigned int reserved_0            : 3;     /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr10;

/* Define the union func_vlcst_dsrptr11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum1           : 7;     /* [6..0] */
        unsigned int reserved_0            : 9;     /* [15..7] */
        unsigned int slice_type1           : 2;     /* [17..16] */
        unsigned int reserved_1            : 13;    /* [30..18] */
        unsigned int islastslc1            : 1;     /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr11;

/* Define the union func_vlcst_dsrptr20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len2              : 29;    /* [28..0] */
        unsigned int reserved_0            : 3;     /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr20;

/* Define the union func_vlcst_dsrptr21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum2           : 7;     /* [6..0] */
        unsigned int reserved_0            : 9;     /* [15..7] */
        unsigned int slice_type2           : 2;     /* [17..16] */
        unsigned int reserved_1            : 13;    /* [30..18] */
        unsigned int islastslc2            : 1;     /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr21;

/* Define the union func_vlcst_dsrptr30 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len3              : 29;    /* [28..0] */
        unsigned int reserved_0            : 3;     /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr30;

/* Define the union func_vlcst_dsrptr31 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum3           : 7;     /* [6..0] */
        unsigned int reserved_0            : 9;     /* [15..7] */
        unsigned int slice_type3           : 2;     /* [17..16] */
        unsigned int reserved_1            : 13;    /* [30..18] */
        unsigned int islastslc3            : 1;     /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr31;

/* Define the union func_vlcst_dsrptr40 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len4              : 29;    /* [28..0] */
        unsigned int reserved_0            : 3;     /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr40;

/* Define the union func_vlcst_dsrptr41 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum4           : 7;     /* [6..0] */
        unsigned int reserved_0            : 9;     /* [15..7] */
        unsigned int slice_type4           : 2;     /* [17..16] */
        unsigned int reserved_1            : 13;    /* [30..18] */
        unsigned int islastslc4            : 1;     /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr41;

/* Define the union func_vlcst_dsrptr50 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len5              : 29;    /* [28..0] */
        unsigned int reserved_0            : 3;     /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr50;

/* Define the union func_vlcst_dsrptr51 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum5           : 7;     /* [6..0] */
        unsigned int reserved_0            : 9;     /* [15..7] */
        unsigned int slice_type5           : 2;     /* [17..16] */
        unsigned int reserved_1            : 13;    /* [30..18] */
        unsigned int islastslc5            : 1;     /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr51;

/* Define the union func_vlcst_dsrptr60 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len6              : 29;    /* [28..0] */
        unsigned int reserved_0            : 3;     /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr60;

/* Define the union func_vlcst_dsrptr61 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum6           : 7;     /* [6..0] */
        unsigned int reserved_0            : 9;     /* [15..7] */
        unsigned int slice_type6           : 2;     /* [17..16] */
        unsigned int reserved_1            : 13;    /* [30..18] */
        unsigned int islastslc6            : 1;     /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr61;

/* Define the union func_vlcst_dsrptr70 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len7              : 29;    /* [28..0] */
        unsigned int reserved_0            : 3;     /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr70;

/* Define the union func_vlcst_dsrptr71 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum7           : 7;     /* [6..0] */
        unsigned int reserved_0            : 9;     /* [15..7] */
        unsigned int slice_type7           : 2;     /* [17..16] */
        unsigned int reserved_1            : 13;    /* [30..18] */
        unsigned int islastslc7            : 1;     /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr71;

/* Define the union func_vlcst_dsrptr80 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len8              : 29;    /* [28..0] */
        unsigned int reserved_0            : 3;     /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr80;

/* Define the union func_vlcst_dsrptr81 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum8           : 7;     /* [6..0] */
        unsigned int reserved_0            : 9;     /* [15..7] */
        unsigned int slice_type8           : 2;     /* [17..16] */
        unsigned int reserved_1            : 13;    /* [30..18] */
        unsigned int islastslc8            : 1;     /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr81;

/* Define the union func_vlcst_dsrptr90 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len9              : 29;    /* [28..0] */
        unsigned int reserved_0            : 3;     /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr90;

/* Define the union func_vlcst_dsrptr91 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum9           : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type9           : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc9            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr91;

/* Define the union func_vlcst_dsrptr100 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len10             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr100;

/* Define the union func_vlcst_dsrptr101 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum10          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type10          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc10           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr101;

/* Define the union func_vlcst_dsrptr110 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len11             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr110;

/* Define the union func_vlcst_dsrptr111 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum11          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type11          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc11           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr111;

/* Define the union func_vlcst_dsrptr120 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len12             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr120;

/* Define the union func_vlcst_dsrptr121 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum12          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type12          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc12           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_func_vlcst_dsrptr121;

/* Define the union func_vlcst_dsrptr130 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len13             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr130;

/* Define the union func_vlcst_dsrptr131 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum13          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type13          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc13           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr131;

/* Define the union func_vlcst_dsrptr140 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len14             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr140;

/* Define the union func_vlcst_dsrptr141 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum14          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type14          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc14           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr141;

/* Define the union func_vlcst_dsrptr150 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len15             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr150;

/* Define the union func_vlcst_dsrptr151 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum15          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type15          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc15           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr151;

/* Define the union func_vlcst_dsrptr160 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len16             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr160;

/* Define the union func_vlcst_dsrptr161 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum16          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type16          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc16           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr161;

/* Define the union func_vlcst_dsrptr170 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len17             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr170;

/* Define the union func_vlcst_dsrptr171 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum17          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type17          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc17           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr171;

/* Define the union func_vlcst_dsrptr180 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len18             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr180;

/* Define the union func_vlcst_dsrptr181 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum18          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type18          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc18           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr181;

/* Define the union func_vlcst_dsrptr190 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len19             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr190;

/* Define the union func_vlcst_dsrptr191 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum19          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type19          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc19           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr191;

/* Define the union func_vlcst_dsrptr200 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len20             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr200;

/* Define the union func_vlcst_dsrptr201 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum20          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type20          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc20           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr201;

/* Define the union func_vlcst_dsrptr210 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len21             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr210;

/* Define the union func_vlcst_dsrptr211 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum21          : 7   ; /* [6..0]  */
        unsigned int reserved_0            : 9   ; /* [15..7]  */
        unsigned int slice_type21          : 2   ; /* [17..16]  */
        unsigned int reserved_1            : 13  ; /* [30..18]  */
        unsigned int islastslc21           : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr211;

/* Define the union func_vlcst_dsrptr220 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len22             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr220;

/* Define the union func_vlcst_dsrptr221 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum22          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type22          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc22           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr221;

/* Define the union func_vlcst_dsrptr230 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len23             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr230;

/* Define the union func_vlcst_dsrptr231 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum23          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type23          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc23           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr231;

/* Define the union func_vlcst_dsrptr240 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len24             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr240;

/* Define the union func_vlcst_dsrptr241 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum24          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type24          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc24           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr241;

/* Define the union func_vlcst_dsrptr250 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len25             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr250;

/* Define the union func_vlcst_dsrptr251 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum25          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type25          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc25           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr251;

/* Define the union func_vlcst_dsrptr260 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len26             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr260;

/* Define the union func_vlcst_dsrptr261 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum26          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type26          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc26           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr261;

/* Define the union func_vlcst_dsrptr270 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len27             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr270;

/* Define the union func_vlcst_dsrptr271 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum27          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type27          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc27           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr271;

/* Define the union func_vlcst_dsrptr280 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len28             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr280;

/* Define the union func_vlcst_dsrptr281 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum28          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int slice_type28          : 2   ; /* [17..16] */
        unsigned int reserved_1            : 13  ; /* [30..18] */
        unsigned int islastslc28           : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr281;

/* Define the union func_vlcst_dsrptr290 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len29             : 29  ; /* [28..0] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr290;

/* Define the union func_vlcst_dsrptr291 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum29          : 7;     /* [6..0] */
        unsigned int reserved_0            : 9;     /* [15..7] */
        unsigned int slice_type29          : 2;     /* [17..16] */
        unsigned int reserved_1            : 13;    /* [30..18] */
        unsigned int islastslc29           : 1;     /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr291;

/* Define the union func_vlcst_dsrptr300 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len30             : 29;    /* [28..0] */
        unsigned int reserved_0            : 3;     /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr300;

/* Define the union func_vlcst_dsrptr301 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum30          : 7;     /* [6..0] */
        unsigned int reserved_0            : 9;     /* [15..7] */
        unsigned int slice_type30          : 2;     /* [17..16] */
        unsigned int reserved_1            : 13;    /* [30..18] */
        unsigned int islastslc30           : 1;     /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr301;

/* Define the union func_vlcst_dsrptr310 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slc_len31             : 29;    /* [28..0] */
        unsigned int reserved_0            : 3;     /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr310;

/* Define the union func_vlcst_dsrptr311 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int invalidnum31          : 7;     /* [6..0] */
        unsigned int reserved_0            : 9;     /* [15..7] */
        unsigned int slice_type31          : 2;     /* [17..16] */
        unsigned int reserved_1            : 13;    /* [30..18] */
        unsigned int islastslc31           : 1;     /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_vlcst_dsrptr311;

/* Define the union func_sel_opt_8x8_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int opt_8x8_cnt           : 19  ; /* [18..0] */
        unsigned int reserved_0            : 13  ; /* [31..19] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_opt_8x8_cnt;

/* Define the union func_sel_intra_opt_8x8_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_opt_8x8_cnt     : 19;    /* [18..0] */
        unsigned int reserved_0            : 13;    /* [31..19] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_intra_opt_8x8_cnt;

/* Define the union func_sel_intra_pcm_opt_8x8_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pcm_opt_8x8_cnt       : 19;    /* [18..0] */
        unsigned int reserved_0            : 13;    /* [31..19] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_intra_pcm_opt_8x8_cnt;

/* Define the union func_sel_inter_opt_8x8_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int inter_opt_8x8_cnt     : 19;    /* [18..0] */
        unsigned int reserved_0            : 13;    /* [31..19] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_inter_opt_8x8_cnt;

/* Define the union func_sel_inter_fme_opt_8x8_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fme_opt_8x8_cnt       : 19;    /* [18..0] */
        unsigned int reserved_0            : 13;    /* [31..19] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_inter_fme_opt_8x8_cnt;

/* Define the union func_sel_inter_merge_opt_8x8_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mrg_opt_8x8_cnt       : 19;    /* [18..0] */
        unsigned int reserved_0            : 13;    /* [31..19] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_inter_merge_opt_8x8_cnt;

/* Define the union func_sel_inter_skip_opt_8x8_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mrg_skip_opt_8x8_cnt  : 19;    /* [18..0] */
        unsigned int reserved_0            : 13;    /* [31..19] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_inter_skip_opt_8x8_cnt;

/* Define the union func_sel_opt_16x16_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int opt_16x16_cnt         : 17;    /* [16..0] */
        unsigned int reserved_0            : 15;    /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_opt_16x16_cnt;

/* Define the union func_sel_intra_opt_16x16_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_opt_16x16_cnt   : 17;    /* [16..0] */
        unsigned int reserved_0            : 15;    /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_intra_opt_16x16_cnt;

/* Define the union func_sel_opt_4x4_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int opt_4x4_cnt           : 19;    /* [18..0] */
        unsigned int reserved_0            : 13;    /* [31..19] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_opt_4x4_cnt;

/* Define the union func_sel_inter_opt_16x16_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int inter_opt_16x16_cnt   : 17;    /* [16..0] */
        unsigned int reserved_0            : 15;    /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_inter_opt_16x16_cnt;

/* Define the union func_sel_inter_fme_opt_16x16_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fme_opt_16x16_cnt     : 17;    /* [16..0] */
        unsigned int reserved_0            : 15;    /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_inter_fme_opt_16x16_cnt;

/* Define the union func_sel_inter_merge_opt_16x16_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mrg_opt_16x16_cnt     : 17;    /* [16..0] */
        unsigned int reserved_0            : 15;    /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_inter_merge_opt_16x16_cnt;

/* Define the union func_sel_inter_skip_opt_16x16_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mrg_skip_opt_16x16_cnt : 17;   /* [16..0] */
        unsigned int reserved_0            : 15;    /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_inter_skip_opt_16x16_cnt;

/* Define the union func_sel_opt_32x32_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int opt_32x32_cnt         : 16;    /* [15..0] */
        unsigned int reserved_0            : 16;    /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_opt_32x32_cnt;

/* Define the union func_sel_intra_opt_32x32_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_opt_32x32_cnt   : 16;    /* [15..0] */
        unsigned int reserved_0            : 16;    /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_intra_opt_32x32_cnt;

/* Define the union func_sel_inter_opt_32x32_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int inter_opt_32x32_cnt   : 16;    /* [15..0] */
        unsigned int reserved_0            : 16;    /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_inter_opt_32x32_cnt;

/* Define the union func_sel_inter_fme_opt_32x32_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fme_opt_32x32_cnt     : 16;    /* [15..0] */
        unsigned int reserved_0            : 16;    /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_inter_fme_opt_32x32_cnt;

/* Define the union func_sel_inter_merge_opt_32x32_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mrg_opt_32x32_cnt     : 16;    /* [15..0] */
        unsigned int reserved_0            : 16;    /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_inter_merge_opt_32x32_cnt;

/* Define the union func_sel_inter_skip_opt_32x32_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mrg_skip_opt_32x32_cnt : 16;   /* [15..0] */
        unsigned int reserved_0            : 16;    /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_inter_skip_opt_32x32_cnt;

/* Define the union func_sel_opt_64x64_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int opt_64x64_cnt         : 16;    /* [15..0] */
        unsigned int reserved_0            : 16;    /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_opt_64x64_cnt;

/* Define the union func_sel_inter_fme_opt_64x64_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fme_opt_64x64_cnt     : 16;    /* [15..0] */
        unsigned int reserved_0            : 16;    /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_inter_fme_opt_64x64_cnt;

/* Define the union u_func_sel_inter_merge_opt_64x64_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mrg_opt_64x64_cnt     : 16;    /* [15..0] */
        unsigned int reserved_0            : 16;    /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_inter_merge_opt_64x64_cnt;

/* Define the union func_sel_inter_skip_opt_64x64_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mrg_skip_opt_64x64_cnt : 16;   /* [15..0] */
        unsigned int reserved_0            : 16;    /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_inter_skip_opt_64x64_cnt;

/* Define the union func_sel_total_luma_qp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int total_luma_qp         : 26;    /* [25..0] */
        unsigned int reserved_0            : 6;     /* [31..26] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_total_luma_qp;

/* Define the union func_sel_max_min_luma_qp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int min_luma_qp           : 6;     /* [5..0] */
        unsigned int reserved_0            : 2;     /* [7..6] */
        unsigned int max_luma_qp           : 6;     /* [13..8] */
        unsigned int reserved_1            : 18;    /* [31..14] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_max_min_luma_qp;

/* Define the union func_sel_luma_qp0_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp0_cnt          : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp0_cnt;

/* Define the union func_sel_luma_qp1_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    luma_qp1_cnt          : 21;     /* [20..0] */
        unsigned int    reserved_0            : 11;     /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_func_sel_luma_qp1_cnt;

/* Define the union func_sel_luma_qp2_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp2_cnt          : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp2_cnt;

/* Define the union func_sel_luma_qp3_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp3_cnt          : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp3_cnt;

/* Define the union func_sel_luma_qp4_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp4_cnt          : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp4_cnt;

/* Define the union func_sel_luma_qp5_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp5_cnt          : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp5_cnt;

/* Define the union func_sel_luma_qp6_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp6_cnt          : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp6_cnt;

/* Define the union func_sel_luma_qp7_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp7_cnt          : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp7_cnt;

/* Define the union func_sel_luma_qp8_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp8_cnt          : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp8_cnt;

/* Define the union func_sel_luma_qp9_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp9_cnt          : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp9_cnt;

/* Define the union func_sel_luma_qp10_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp10_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp10_cnt;

/* Define the union func_sel_luma_qp11_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp11_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp11_cnt;

/* Define the union func_sel_luma_qp12_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp12_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp12_cnt;

/* Define the union func_sel_luma_qp13_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp13_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp13_cnt;

/* Define the union func_sel_luma_qp14_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp14_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp14_cnt;

/* Define the union func_sel_luma_qp15_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp15_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp15_cnt;

/* Define the union func_sel_luma_qp16_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp16_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp16_cnt;

/* Define the union func_sel_luma_qp17_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp17_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp17_cnt;

/* Define the union func_sel_luma_qp18_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp18_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp18_cnt;

/* Define the union func_sel_luma_qp19_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp19_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp19_cnt;

/* Define the union func_sel_luma_qp20_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp20_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp20_cnt;

/* Define the union func_sel_luma_qp21_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp21_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp21_cnt;

/* Define the union func_sel_luma_qp22_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp22_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp22_cnt;

/* Define the union func_sel_luma_qp23_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp23_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp23_cnt;

/* Define the union func_sel_luma_qp24_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp24_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp24_cnt;

/* Define the union func_sel_luma_qp25_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp25_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp25_cnt;

/* Define the union func_sel_luma_qp26_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp26_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp26_cnt;

/* Define the union func_sel_luma_qp27_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp27_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp27_cnt;

/* Define the union func_sel_luma_qp28_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp28_cnt         : 21  ; /* [20..0] */
        unsigned int reserved_0            : 11  ; /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp28_cnt;

/* Define the union func_sel_luma_qp29_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp29_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp29_cnt;

/* Define the union func_sel_luma_qp30_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp30_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp30_cnt;

/* Define the union func_sel_luma_qp31_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp31_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp31_cnt;

/* Define the union func_sel_luma_qp32_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp32_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp32_cnt;

/* Define the union func_sel_luma_qp33_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp33_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp33_cnt;

/* Define the union func_sel_luma_qp34_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp34_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp34_cnt;

/* Define the union func_sel_luma_qp35_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp35_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp35_cnt;

/* Define the union func_sel_luma_qp36_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp36_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp36_cnt;

/* Define the union func_sel_luma_qp37_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp37_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp37_cnt;

/* Define the union func_sel_luma_qp38_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp38_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp38_cnt;

/* Define the union func_sel_luma_qp39_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp39_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp39_cnt;

/* Define the union func_sel_luma_qp40_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp40_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp40_cnt;

/* Define the union func_sel_luma_qp41_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp41_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp41_cnt;

/* Define the union func_sel_luma_qp42_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp42_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp42_cnt;

/* Define the union func_sel_luma_qp43_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp43_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp43_cnt;

/* Define the union func_sel_luma_qp44_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp44_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp44_cnt;

/* Define the union func_sel_luma_qp45_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp45_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp45_cnt;

/* Define the union func_sel_luma_qp46_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp46_cnt         : 21;    /* [20..0]  */
        unsigned int reserved_0            : 11;    /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp46_cnt;

/* Define the union func_sel_luma_qp47_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp47_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp47_cnt;

/* Define the union func_sel_luma_qp48_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp48_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp48_cnt;

/* Define the union func_sel_luma_qp49_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp49_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp49_cnt;

/* Define the union func_sel_luma_qp50_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp50_cnt         : 21;    /* [20..0] */
        unsigned int reserved_0            : 11;    /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp50_cnt;

/* Define the union func_sel_luma_qp51_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_qp51_cnt         : 21  ; /* [20..0] */
        unsigned int reserved_0            : 11  ; /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sel_luma_qp51_cnt;

/* Define the union func_sao_off_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao2vcpi_saooff_num_luma : 14;     /* [13..0] */
        unsigned int reserved_0            : 2;         /* [15..14] */
        unsigned int sao2vcpi_saooff_num_chroma : 14;   /* [29..16] */
        unsigned int reserved_1            : 2 ;        /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sao_off_num;

/* Define the union func_sao_mse_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao2vcpi_mse_cnt      : 15  ; /* [14..0] */
        unsigned int reserved_0            : 17  ; /* [31..15] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sao_mse_cnt;

/* Define the union func_sao_mse_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao2vcpi_mse_max      : 28  ; /* [27..0] */
        unsigned int reserved_0            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_func_sao_mse_max;

/* ============================================================================== */
/* Define the global struct */
typedef struct {
    volatile u_func_vcpi_intstat func_vcpi_intstat;
    volatile u_func_vcpi_rawint func_vcpi_rawint;
    volatile unsigned int func_vcpi_vedu_timer;
    volatile unsigned int func_vcpi_idle_timer;
    volatile u_func_vcpi_bus_idle_flag func_vcpi_bus_idle_flag;
    volatile unsigned int reserved_func_vcpi[11];   /* 11: size of reserved_func_vcpi */
    volatile unsigned int reserved_func_vctrl[16];  /* 16: size of reserved_func_vctrl */
    volatile unsigned int reserved_func_qpgld[16];  /* 16: size of reserved_func_qpgld */
    volatile unsigned int reserved_func_curld[16];  /* 16: size of reserved_func_curld */
    volatile unsigned int reserved_func_nbi[16];    /* 16: size of reserved_func_nbi */
    volatile unsigned int reserved_func_pmeld[16];  /* 16: size of reserved_func_pmeld */
    volatile unsigned int reserved_func_pme_info_ld[16]; /* 16: size of reserved_func_pme_info_ld */
    volatile u_func_pme_madi_sum func_pme_madi_sum;
    volatile u_func_pme_madp_sum func_pme_madp_sum;
    volatile u_func_pme_madi_num func_pme_madi_num;
    volatile u_func_pme_madp_num func_pme_madp_num;
    volatile u_func_pme_large_sad_sum func_pme_large_sad_sum;
    volatile u_func_pme_low_luma_sum func_pme_low_luma_sum;
    volatile u_func_pme_chroma_scene_sum func_pme_chroma_scene_sum;
    volatile u_func_pme_move_scene_sum func_pme_move_scene_sum;
    volatile u_func_pme_skin_region_sum func_pme_skin_region_sum;
    volatile unsigned int reserved_func_pme[7];     /* 7: size of register array */
    volatile unsigned int reserved_func_pmest[16];  /* 16: size of register array */
    volatile unsigned int reserved_func_pmeinfo_st[16]; /* 16: size of register array */
    volatile u_func_bgstr_block_count func_bgstr_block_count;
    volatile u_func_bgstr_frame_bgm_dist func_bgstr_frame_bgm_dist;
    volatile unsigned int reserved_func_bgstr[14];  /* 14: size of reserved_func_bgstr */
    volatile unsigned int reserved_func_qpg[16];    /* 16: size of reserved_func_qpg */
    volatile unsigned int reserved_func_refld[16];  /* 16: size of reserved_func_refld */
    volatile unsigned int reserved_func_pintra[16]; /* 16: size of reserved_func_pintra */
    volatile unsigned int reserved_func_ime[16];    /* 16: size of reserved_func_ime */
    volatile unsigned int reserved_func_fme[16];    /* 16: size of reserved_func_fme */
    volatile unsigned int reserved_func_mrg[16];    /* 16: size of reserved_func_mrg */
    volatile unsigned int reserved_func_bggen[16];  /* 16: size of reserved_func_bggen */
    volatile unsigned int reserved_func_intra[16];  /* 16: size of reserved_func_intra */
    volatile unsigned int reserved_func_nu0[16];    /* 16: size of reserved_func_nu0 */
    volatile unsigned int reserved_func_pmv[16];    /* 16: size of reserved_func_pmv */
    volatile unsigned int reserved_func_nu1[16];    /* 16: size of reserved_func_nu1 */
    volatile unsigned int reserved_func_tqitq0[16]; /* 16: size of reserved_func_tqitq0 */
    volatile unsigned int reserved_func_nu2[16];    /* 16: size of reserved_func_nu2 */
    volatile unsigned int reserved_func_bgld[16];   /* 16: size of reserved_func_bgld */
    volatile unsigned int reserved_func_lfldst[16]; /* 16: size of reserved_func_lfldst */
    volatile unsigned int reserved_func_dblk[16];   /* 16: size of reserved_func_dblk */
    volatile unsigned int reserved_func_nu4[16];    /* 16: size of reserved_func_nu4 */
    volatile unsigned int reserved_func_recst[16];  /* 16: size of reserved_func_recst */
    volatile unsigned int reserved_func_pack0[16];  /* 16: size of reserved_func_pack0 */
    volatile unsigned int reserved_func_pack1[16];  /* 16: size of reserved_func_pack1 */
    volatile unsigned int func_cabac_pic_strmsize;
    volatile unsigned int func_cabac_bin_num;
    volatile unsigned int func_cabac_res_bin_num;
    volatile unsigned int func_cabac_hdr_bin_num;
    volatile unsigned int func_cabac_move_scene_bits;
    volatile unsigned int func_cabac_stredge_move_bits;
    volatile unsigned int func_cabac_skin_bits;
    volatile unsigned int func_cabac_lowluma_bits;
    volatile unsigned int func_cabac_chromaport_bits;
    volatile unsigned int reserved_func_cabac[7];   /* 7: size of reserved_func_cabac */
    volatile unsigned int func_vlc_cabac_hrd_bits;
    volatile unsigned int func_vlc_cabac_res_bits;
    volatile unsigned int func_vlc_cavlc_hrd_bits;
    volatile unsigned int func_vlc_cavlc_res_bits;
    volatile unsigned int func_vlc_pic_strmsize;
    volatile unsigned int func_vlc_move_scene_bits;
    volatile unsigned int func_vlc_stredge_move_bits;
    volatile unsigned int func_vlc_skin_bits;
    volatile unsigned int func_vlc_lowluma_bits;
    volatile unsigned int func_vlc_chromaport_bits;
    volatile unsigned int reserved_func_vlc[6];     /* 6: size of reserved_func_vlc */
    volatile unsigned int func_vlcst_slc_len_cnt;
    volatile u_func_vlcst_dsrptr00 func_vlcst_dsrptr00;
    volatile u_func_vlcst_dsrptr01 func_vlcst_dsrptr01;
    volatile u_func_vlcst_dsrptr10 func_vlcst_dsrptr10;
    volatile u_func_vlcst_dsrptr11 func_vlcst_dsrptr11;
    volatile u_func_vlcst_dsrptr20 func_vlcst_dsrptr20;
    volatile u_func_vlcst_dsrptr21 func_vlcst_dsrptr21;
    volatile u_func_vlcst_dsrptr30 func_vlcst_dsrptr30;
    volatile u_func_vlcst_dsrptr31 func_vlcst_dsrptr31;
    volatile u_func_vlcst_dsrptr40 func_vlcst_dsrptr40;
    volatile u_func_vlcst_dsrptr41 func_vlcst_dsrptr41;
    volatile u_func_vlcst_dsrptr50 func_vlcst_dsrptr50;
    volatile u_func_vlcst_dsrptr51 func_vlcst_dsrptr51;
    volatile u_func_vlcst_dsrptr60 func_vlcst_dsrptr60;
    volatile u_func_vlcst_dsrptr61 func_vlcst_dsrptr61;
    volatile u_func_vlcst_dsrptr70 func_vlcst_dsrptr70;
    volatile u_func_vlcst_dsrptr71 func_vlcst_dsrptr71;
    volatile u_func_vlcst_dsrptr80 func_vlcst_dsrptr80;
    volatile u_func_vlcst_dsrptr81 func_vlcst_dsrptr81;
    volatile u_func_vlcst_dsrptr90 func_vlcst_dsrptr90;
    volatile u_func_vlcst_dsrptr91 func_vlcst_dsrptr91;
    volatile u_func_vlcst_dsrptr100 func_vlcst_dsrptr100;
    volatile u_func_vlcst_dsrptr101 func_vlcst_dsrptr101;
    volatile u_func_vlcst_dsrptr110 func_vlcst_dsrptr110;
    volatile u_func_vlcst_dsrptr111 func_vlcst_dsrptr111;
    volatile u_func_vlcst_dsrptr120 func_vlcst_dsrptr120;
    volatile u_func_vlcst_dsrptr121 func_vlcst_dsrptr121;
    volatile u_func_vlcst_dsrptr130 func_vlcst_dsrptr130;
    volatile u_func_vlcst_dsrptr131 func_vlcst_dsrptr131;
    volatile u_func_vlcst_dsrptr140 func_vlcst_dsrptr140;
    volatile u_func_vlcst_dsrptr141 func_vlcst_dsrptr141;
    volatile u_func_vlcst_dsrptr150 func_vlcst_dsrptr150;
    volatile u_func_vlcst_dsrptr151 func_vlcst_dsrptr151;
    volatile u_func_vlcst_dsrptr160 func_vlcst_dsrptr160;
    volatile u_func_vlcst_dsrptr161 func_vlcst_dsrptr161;
    volatile u_func_vlcst_dsrptr170 func_vlcst_dsrptr170;
    volatile u_func_vlcst_dsrptr171 func_vlcst_dsrptr171;
    volatile u_func_vlcst_dsrptr180 func_vlcst_dsrptr180;
    volatile u_func_vlcst_dsrptr181 func_vlcst_dsrptr181;
    volatile u_func_vlcst_dsrptr190 func_vlcst_dsrptr190;
    volatile u_func_vlcst_dsrptr191 func_vlcst_dsrptr191;
    volatile u_func_vlcst_dsrptr200 func_vlcst_dsrptr200;
    volatile u_func_vlcst_dsrptr201 func_vlcst_dsrptr201;
    volatile u_func_vlcst_dsrptr210 func_vlcst_dsrptr210;
    volatile u_func_vlcst_dsrptr211 func_vlcst_dsrptr211;
    volatile u_func_vlcst_dsrptr220 func_vlcst_dsrptr220;
    volatile u_func_vlcst_dsrptr221 func_vlcst_dsrptr221;
    volatile u_func_vlcst_dsrptr230 func_vlcst_dsrptr230;
    volatile u_func_vlcst_dsrptr231 func_vlcst_dsrptr231;
    volatile u_func_vlcst_dsrptr240 func_vlcst_dsrptr240;
    volatile u_func_vlcst_dsrptr241 func_vlcst_dsrptr241;
    volatile u_func_vlcst_dsrptr250 func_vlcst_dsrptr250;
    volatile u_func_vlcst_dsrptr251 func_vlcst_dsrptr251;
    volatile u_func_vlcst_dsrptr260 func_vlcst_dsrptr260;
    volatile u_func_vlcst_dsrptr261 func_vlcst_dsrptr261;
    volatile u_func_vlcst_dsrptr270 func_vlcst_dsrptr270;
    volatile u_func_vlcst_dsrptr271 func_vlcst_dsrptr271;
    volatile u_func_vlcst_dsrptr280 func_vlcst_dsrptr280;
    volatile u_func_vlcst_dsrptr281 func_vlcst_dsrptr281;
    volatile u_func_vlcst_dsrptr290 func_vlcst_dsrptr290;
    volatile u_func_vlcst_dsrptr291 func_vlcst_dsrptr291;
    volatile u_func_vlcst_dsrptr300 func_vlcst_dsrptr300;
    volatile u_func_vlcst_dsrptr301 func_vlcst_dsrptr301;
    volatile u_func_vlcst_dsrptr310 func_vlcst_dsrptr310;
    volatile u_func_vlcst_dsrptr311 func_vlcst_dsrptr311;
    volatile unsigned int reserved_func_vlcst[63];  /* 63: size of reserved_func_vlcst */
    volatile unsigned int reserved_func_emar[16];   /* 16: size of reserved_func_emar */
    volatile unsigned int reserved_func_ppfd[16];   /* 16: size of reserved_func_ppfd */
    volatile u_func_sel_opt_8x8_cnt func_sel_opt_8_x8_cnt;
    volatile u_func_sel_intra_opt_8x8_cnt func_sel_intra_opt_8_x8_cnt;
    volatile unsigned int func_sel_intra_normal_opt_8_x8_cnt;
    volatile u_func_sel_intra_pcm_opt_8x8_cnt func_sel_intra_pcm_opt_8_x8_cnt;
    volatile u_func_sel_inter_opt_8x8_cnt func_sel_inter_opt_8_x8_cnt;
    volatile u_func_sel_inter_fme_opt_8x8_cnt func_sel_inter_fme_opt_8_x8_cnt;
    volatile u_func_sel_inter_merge_opt_8x8_cnt func_selinter_merge_opt_8_x8_cnt;
    volatile u_func_sel_inter_skip_opt_8x8_cnt func_sel_inter_skip_opt_8_x8_cnt;
    volatile u_func_sel_opt_16x16_cnt func_sel_opt_16_x16_cnt;
    volatile u_func_sel_intra_opt_16x16_cnt func_sel_intra_opt_16_x16_cnt;
    volatile u_func_sel_opt_4x4_cnt func_sel_opt_4_x4_cnt;
    volatile unsigned int reserved0_func_sel;
    volatile u_func_sel_inter_opt_16x16_cnt func_sel_inter_opt_16_x16_cnt;
    volatile u_func_sel_inter_fme_opt_16x16_cnt func_sel_inter_fme_opt_16_x16_cnt;
    volatile u_func_sel_inter_merge_opt_16x16_cnt func_sel_inter_merge_opt_16_x16_cnt;
    volatile u_func_sel_inter_skip_opt_16x16_cnt func_sel_inter_skip_opt_16_x16_cnt;
    volatile u_func_sel_opt_32x32_cnt func_sel_opt_32_x32_cnt;
    volatile u_func_sel_intra_opt_32x32_cnt func_sel_intra_opt_32_x32_cnt;
    volatile unsigned int reserved1_func_sel;
    volatile u_func_sel_inter_opt_32x32_cnt func_sel_inter_opt_32_x32_cnt;
    volatile u_func_sel_inter_fme_opt_32x32_cnt func_sel_inter_fme_opt_32_x32_cnt;
    volatile u_func_sel_inter_merge_opt_32x32_cnt func_sel_inter_merge_opt_32_x32_cnt;
    volatile u_func_sel_inter_skip_opt_32x32_cnt func_sel_inter_skip_opt_32_x32_cnt;
    volatile u_func_sel_opt_64x64_cnt func_sel_opt_64_x64_cnt;
    volatile u_func_sel_inter_fme_opt_64x64_cnt func_sel_inter_fme_opt_64_x64_cnt;
    volatile u_func_sel_inter_merge_opt_64x64_cnt func_sel_inter_merge_opt_64_x64_cnt;
    volatile u_func_sel_inter_skip_opt_64x64_cnt func_sel_inter_skip_opt_64_x64_cnt;
    volatile u_func_sel_total_luma_qp func_sel_total_luma_qp;
    volatile u_func_sel_max_min_luma_qp func_sel_max_min_luma_qp;
    volatile unsigned int reserved2_func_sel[35];   /* 35: size of reserved2_func_sel */
    volatile u_func_sel_luma_qp0_cnt func_sel_luma_qp0_cnt;
    volatile u_func_sel_luma_qp1_cnt func_sel_luma_qp1_cnt;
    volatile u_func_sel_luma_qp2_cnt func_sel_luma_qp2_cnt;
    volatile u_func_sel_luma_qp3_cnt func_sel_luma_qp3_cnt;
    volatile u_func_sel_luma_qp4_cnt func_sel_luma_qp4_cnt;
    volatile u_func_sel_luma_qp5_cnt func_sel_luma_qp5_cnt;
    volatile u_func_sel_luma_qp6_cnt func_sel_luma_qp6_cnt;
    volatile u_func_sel_luma_qp7_cnt func_sel_luma_qp7_cnt;
    volatile u_func_sel_luma_qp8_cnt func_sel_luma_qp8_cnt;
    volatile u_func_sel_luma_qp9_cnt func_sel_luma_qp9_cnt;
    volatile u_func_sel_luma_qp10_cnt func_sel_luma_qp10_cnt;
    volatile u_func_sel_luma_qp11_cnt func_sel_luma_qp11_cnt;
    volatile u_func_sel_luma_qp12_cnt func_sel_luma_qp12_cnt;
    volatile u_func_sel_luma_qp13_cnt func_sel_luma_qp13_cnt;
    volatile u_func_sel_luma_qp14_cnt func_sel_luma_qp14_cnt;
    volatile u_func_sel_luma_qp15_cnt func_sel_luma_qp15_cnt;
    volatile u_func_sel_luma_qp16_cnt func_sel_luma_qp16_cnt;
    volatile u_func_sel_luma_qp17_cnt func_sel_luma_qp17_cnt;
    volatile u_func_sel_luma_qp18_cnt func_sel_luma_qp18_cnt;
    volatile u_func_sel_luma_qp19_cnt func_sel_luma_qp19_cnt;
    volatile u_func_sel_luma_qp20_cnt func_sel_luma_qp20_cnt;
    volatile u_func_sel_luma_qp21_cnt func_sel_luma_qp21_cnt;
    volatile u_func_sel_luma_qp22_cnt func_sel_luma_qp22_cnt;
    volatile u_func_sel_luma_qp23_cnt func_sel_luma_qp23_cnt;
    volatile u_func_sel_luma_qp24_cnt func_sel_luma_qp24_cnt;
    volatile u_func_sel_luma_qp25_cnt func_sel_luma_qp25_cnt;
    volatile u_func_sel_luma_qp26_cnt func_sel_luma_qp26_cnt;
    volatile u_func_sel_luma_qp27_cnt func_sel_luma_qp27_cnt;
    volatile u_func_sel_luma_qp28_cnt func_sel_luma_qp28_cnt;
    volatile u_func_sel_luma_qp29_cnt func_sel_luma_qp29_cnt;
    volatile u_func_sel_luma_qp30_cnt func_sel_luma_qp30_cnt;
    volatile u_func_sel_luma_qp31_cnt func_sel_luma_qp31_cnt;
    volatile u_func_sel_luma_qp32_cnt func_sel_luma_qp32_cnt;
    volatile u_func_sel_luma_qp33_cnt func_sel_luma_qp33_cnt;
    volatile u_func_sel_luma_qp34_cnt func_sel_luma_qp34_cnt;
    volatile u_func_sel_luma_qp35_cnt func_sel_luma_qp35_cnt;
    volatile u_func_sel_luma_qp36_cnt func_sel_luma_qp36_cnt;
    volatile u_func_sel_luma_qp37_cnt func_sel_luma_qp37_cnt;
    volatile u_func_sel_luma_qp38_cnt func_sel_luma_qp38_cnt;
    volatile u_func_sel_luma_qp39_cnt func_sel_luma_qp39_cnt;
    volatile u_func_sel_luma_qp40_cnt func_sel_luma_qp40_cnt;
    volatile u_func_sel_luma_qp41_cnt func_sel_luma_qp41_cnt;
    volatile u_func_sel_luma_qp42_cnt func_sel_luma_qp42_cnt;
    volatile u_func_sel_luma_qp43_cnt func_sel_luma_qp43_cnt;
    volatile u_func_sel_luma_qp44_cnt func_sel_luma_qp44_cnt;
    volatile u_func_sel_luma_qp45_cnt func_sel_luma_qp45_cnt;
    volatile u_func_sel_luma_qp46_cnt func_sel_luma_qp46_cnt;
    volatile u_func_sel_luma_qp47_cnt func_sel_luma_qp47_cnt;
    volatile u_func_sel_luma_qp48_cnt func_sel_luma_qp48_cnt;
    volatile u_func_sel_luma_qp49_cnt func_sel_luma_qp49_cnt;
    volatile u_func_sel_luma_qp50_cnt func_sel_luma_qp50_cnt;
    volatile u_func_sel_luma_qp51_cnt func_sel_luma_qp51_cnt;
    volatile unsigned int reserved3_func_sel[12];       /* 12: size of reserved3_func_sel */
    volatile unsigned int reserved_func_tbldst[128];    /* 128: size of reserved_func_tbldst */
    volatile u_func_sao_off_num func_sao_off_num;
    volatile unsigned int func_sao_mse_sum;
    volatile u_func_sao_mse_cnt func_sao_mse_cnt;
    volatile u_func_sao_mse_max func_sao_mse_max;
    volatile unsigned int func_sao_ssd_area0_sum;
    volatile unsigned int func_sao_ssd_area1_sum;
    volatile unsigned int func_sao_ssd_area2_sum;
    volatile unsigned int func_sao_ssd_area3_sum;
    volatile unsigned int func_sao_ssd_area4_sum;
    volatile unsigned int func_sao_ssd_area5_sum;
    volatile unsigned int func_sao_ssd_area6_sum;
    volatile unsigned int func_sao_ssd_area7_sum;
    volatile unsigned int reserved_func_sao[20];    /* 20: size of reserved_func_sao */
    volatile unsigned int reserved_func_nu3[48];    /* 48: size of reserved_func_nu3 */
    volatile unsigned int reserved4[11264];         /* 11264: size of reserved4 */
} func_regs_type;

#endif /* __C_UNION_DEFINE_FUNC_H__ */
