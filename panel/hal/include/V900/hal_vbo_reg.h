/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel module
* Author: sdk
* Create: 2019-11-20
*/

#ifndef __UNION_DEFINE_VBOTX_H_
#define __UNION_DEFINE_VBOTX_H_

/* define the union u_vbo_ctrl0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vbo_en                : 1   ; /* [0]  */
        unsigned int    msb_lsb_mode          : 1   ; /* [1]  */
        unsigned int    vbo_lane_map          : 2   ; /* [3..2]  */
        unsigned int    vbo_byte_num          : 4   ; /* [7..4]  */
        unsigned int    vbo_lane_num          : 8   ; /* [15..8]  */
        unsigned int    vbo_chan_num          : 4   ; /* [19..16]  */
        unsigned int    vbo_data_mode         : 3   ; /* [22..20]  */
        unsigned int    reserved_0            : 1   ; /* [23]  */
        unsigned int    vbo_lockn_sel         : 4   ; /* [27..24]  */
        unsigned int    vbo_htpdn_sel         : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_vbo_ctrl0;

/* define the union u_vbo_ctrl1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vbo_frm_mask          : 1   ; /* [0]  */
        unsigned int    reserved_0            : 1   ; /* [1]  */
        unsigned int    vbo_de_only_mode      : 1   ; /* [2]  */
        unsigned int    vbo_lock_cnt_clr      : 1   ; /* [3]  */
        unsigned int    vbo_htpn_sw           : 1   ; /* [4]  */
        unsigned int    vbo_htpn_mode         : 1   ; /* [5]  */
        unsigned int    vbo_lockn_cfg         : 1   ; /* [6]  */
        unsigned int    vbo_lockn_mode        : 1   ; /* [7]  */
        unsigned int    vbo_test_mode         : 4   ; /* [11..8]  */
        unsigned int    vbo_test_en           : 1   ; /* [12]  */
        unsigned int    vsync_inf             : 1   ; /* [13]  */
        unsigned int    hsync_inf             : 1   ; /* [14]  */
        unsigned int    de_inf                : 1   ; /* [15]  */
        unsigned int    de_rise_cnt_en        : 1   ; /* [16]  */
        unsigned int    htotal_err_clr        : 1   ; /* [17]  */
        unsigned int    curr_st_err_clr       : 1   ; /* [18]  */
        unsigned int    reserved_1            : 13  ; /* [31..19]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_vbo_ctrl1;

/* define the union u_vbo_deug0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    active_width          : 16  ; /* [15..0]  */
        unsigned int    active_height         : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_vbo_deug0;

/* define the union u_phystat0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vbo_lockn_0           : 1   ; /* [0]  */
        unsigned int    vbo_lockn_1           : 1   ; /* [1]  */
        unsigned int    vbo_lockn_2           : 1   ; /* [2]  */
        unsigned int    vbo_lockn_3           : 1   ; /* [3]  */
        unsigned int    norm_sta0             : 1   ; /* [4]  */
        unsigned int    norm_sta1             : 1   ; /* [5]  */
        unsigned int    norm_sta2             : 1   ; /* [6]  */
        unsigned int    norm_sta3             : 1   ; /* [7]  */
        unsigned int    norm_sta4             : 1   ; /* [8]  */
        unsigned int    norm_sta5             : 1   ; /* [9]  */
        unsigned int    norm_sta6             : 1   ; /* [10]  */
        unsigned int    norm_sta7             : 1   ; /* [11]  */
        unsigned int    htotal_err            : 1   ; /* [12]  */
        unsigned int    curr_st_err           : 1   ; /* [13]  */
        unsigned int    reserved_0            : 2   ; /* [15..14]  */
        unsigned int    htotal                : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_phystat0;

/* define the union u_phystat1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vbo_lockn0_cnt        : 8   ; /* [7..0]  */
        unsigned int    vbo_lockn1_cnt        : 8   ; /* [15..8]  */
        unsigned int    vbo_lockn2_cnt        : 8   ; /* [23..16]  */
        unsigned int    vbo_lockn3_cnt        : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_phystat1;

/* define the union u_phystat2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    curr_sta0             : 4   ; /* [3..0]  */
        unsigned int    curr_sta1             : 4   ; /* [7..4]  */
        unsigned int    curr_sta2             : 4   ; /* [11..8]  */
        unsigned int    curr_sta3             : 4   ; /* [15..12]  */
        unsigned int    curr_sta4             : 4   ; /* [19..16]  */
        unsigned int    curr_sta5             : 4   ; /* [23..20]  */
        unsigned int    curr_sta6             : 4   ; /* [27..24]  */
        unsigned int    curr_sta7             : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_phystat2;

/* define the union u_lane_swap1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane3_swap            : 8   ; /* [7..0]  */
        unsigned int    lane2_swap            : 8   ; /* [15..8]  */
        unsigned int    lane1_swap            : 8   ; /* [23..16]  */
        unsigned int    lane0_swap            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap1;

/* define the union u_lane_swap2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane7_swap            : 8   ; /* [7..0]  */
        unsigned int    lane6_swap            : 8   ; /* [15..8]  */
        unsigned int    lane5_swap            : 8   ; /* [23..16]  */
        unsigned int    lane4_swap            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap2;

/* define the union u_lane_swap3 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane11_swap           : 8   ; /* [7..0]  */
        unsigned int    lane10_swap           : 8   ; /* [15..8]  */
        unsigned int    lane9_swap            : 8   ; /* [23..16]  */
        unsigned int    lane8_swap            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap3;

/* define the union u_lane_swap4 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane15_swap           : 8   ; /* [7..0]  */
        unsigned int    lane14_swap           : 8   ; /* [15..8]  */
        unsigned int    lane13_swap           : 8   ; /* [23..16]  */
        unsigned int    lane12_swap           : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap4;

/* define the union u_lane_swap5 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane19_swap           : 8   ; /* [7..0]  */
        unsigned int    lane18_swap           : 8   ; /* [15..8]  */
        unsigned int    lane17_swap           : 8   ; /* [23..16]  */
        unsigned int    lane16_swap           : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap5;

/* define the union u_lane_swap6 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane23_swap           : 8   ; /* [7..0]  */
        unsigned int    lane22_swap           : 8   ; /* [15..8]  */
        unsigned int    lane21_swap           : 8   ; /* [23..16]  */
        unsigned int    lane20_swap           : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap6;

/* define the union u_lane_swap7 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane27_swap           : 8   ; /* [7..0]  */
        unsigned int    lane26_swap           : 8   ; /* [15..8]  */
        unsigned int    lane25_swap           : 8   ; /* [23..16]  */
        unsigned int    lane24_swap           : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap7;

/* define the union u_lane_swap8 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane31_swap           : 8   ; /* [7..0]  */
        unsigned int    lane30_swap           : 8   ; /* [15..8]  */
        unsigned int    lane29_swap           : 8   ; /* [23..16]  */
        unsigned int    lane28_swap           : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap8;

/* define the union u_lane_swap9 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane35_swap           : 8   ; /* [7..0]  */
        unsigned int    lane34_swap           : 8   ; /* [15..8]  */
        unsigned int    lane33_swap           : 8   ; /* [23..16]  */
        unsigned int    lane32_swap           : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap9;

/* define the union u_lane_swap10 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane39_swap           : 8   ; /* [7..0]  */
        unsigned int    lane38_swap           : 8   ; /* [15..8]  */
        unsigned int    lane37_swap           : 8   ; /* [23..16]  */
        unsigned int    lane36_swap           : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap10;

/* define the union u_lane_swap11 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane43_swap           : 8   ; /* [7..0]  */
        unsigned int    lane42_swap           : 8   ; /* [15..8]  */
        unsigned int    lane41_swap           : 8   ; /* [23..16]  */
        unsigned int    lane40_swap           : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap11;

/* define the union u_lane_swap12 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane47_swap           : 8   ; /* [7..0]  */
        unsigned int    lane46_swap           : 8   ; /* [15..8]  */
        unsigned int    lane45_swap           : 8   ; /* [23..16]  */
        unsigned int    lane44_swap           : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap12;

/* define the union u_lane_swap13 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane51_swap           : 8   ; /* [7..0]  */
        unsigned int    lane50_swap           : 8   ; /* [15..8]  */
        unsigned int    lane49_swap           : 8   ; /* [23..16]  */
        unsigned int    lane48_swap           : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap13;

/* define the union u_lane_swap14 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane55_swap           : 8   ; /* [7..0]  */
        unsigned int    lane54_swap           : 8   ; /* [15..8]  */
        unsigned int    lane53_swap           : 8   ; /* [23..16]  */
        unsigned int    lane52_swap           : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap14;

/* define the union u_lane_swap15 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane59_swap           : 8   ; /* [7..0]  */
        unsigned int    lane58_swap           : 8   ; /* [15..8]  */
        unsigned int    lane57_swap           : 8   ; /* [23..16]  */
        unsigned int    lane56_swap           : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap15;

/* define the union u_lane_swap16 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane63_swap           : 8   ; /* [7..0]  */
        unsigned int    lane62_swap           : 8   ; /* [15..8]  */
        unsigned int    lane61_swap           : 8   ; /* [23..16]  */
        unsigned int    lane60_swap           : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_lane_swap16;

/* define the union u_channel_sel */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    chan0_sel             : 4   ; /* [3..0]  */
        unsigned int    chan1_sel             : 4   ; /* [7..4]  */
        unsigned int    chan2_sel             : 4   ; /* [11..8]  */
        unsigned int    chan3_sel             : 4   ; /* [15..12]  */
        unsigned int    chan4_sel             : 4   ; /* [19..16]  */
        unsigned int    chan5_sel             : 4   ; /* [23..20]  */
        unsigned int    chan6_sel             : 4   ; /* [27..24]  */
        unsigned int    chan7_sel             : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_channel_sel;

/* define the union u_cbar_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cbar_data_en          : 1   ; /* [0]  */
        unsigned int    cbar_timing_en        : 1   ; /* [1]  */
        unsigned int    cbar_sel              : 1   ; /* [2]  */
        unsigned int    cbar_mode             : 1   ; /* [3]  */
        unsigned int    multichn_en           : 2   ; /* [5..4]  */
        unsigned int    manual_mode           : 1   ; /* [6]  */
        unsigned int    reserved_0            : 25  ; /* [31..7]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_cbar_ctrl;

/* define the union u_test_hb_num */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    test_hfb              : 16  ; /* [15..0]  */
        unsigned int    test_hbb              : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_test_hb_num;

/* define the union u_test_vb_num */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    test_vfb              : 16  ; /* [15..0]  */
        unsigned int    test_vbb              : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_test_vb_num;

/* define the union u_test_act_num */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    test_width            : 16  ; /* [15..0]  */
        unsigned int    test_height           : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_test_act_num;

/* define the union u_test_hv_width */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    test_hpw              : 16  ; /* [15..0]  */
        unsigned int    test_vpw              : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_test_hv_width;

/* define the union u_test_hmid */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    reserved_0            : 16  ; /* [15..0]  */
        unsigned int    test_hmid             : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_test_hmid;

/* define the union u_test_start_pos */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    start_pos             : 8   ; /* [7..0]  */
        unsigned int    timing_start_pos      : 8   ; /* [15..8]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_test_start_pos;

/* define the union u_test_cbar_upd */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    regup                 : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_test_cbar_upd;

/* define the global struct */
typedef struct {
    u_vbo_ctrl0                     vbo_ctrl0                        ; /* 0x0 */
    u_vbo_ctrl1                     vbo_ctrl1                        ; /* 0x4 */
    u_vbo_deug0                     vbo_deug0                        ; /* 0x8 */
    u_phystat0                      phystat0                         ; /* 0xc */
    u_phystat1                      phystat1                         ; /* 0x10 */
    u_phystat2                      phystat2                         ; /* 0x14 */
    u_lane_swap1                    lane_swap1                       ; /* 0x18 */
    u_lane_swap2                    lane_swap2                       ; /* 0x1c */
    u_lane_swap3                    lane_swap3                       ; /* 0x20 */
    u_lane_swap4                    lane_swap4                       ; /* 0x24 */
    u_lane_swap5                    lane_swap5                       ; /* 0x28 */
    u_lane_swap6                    lane_swap6                       ; /* 0x2c */
    u_lane_swap7                    lane_swap7                       ; /* 0x30 */
    u_lane_swap8                    lane_swap8                       ; /* 0x34 */
    u_lane_swap9                    lane_swap9                       ; /* 0x38 */
    u_lane_swap10                   lane_swap10                      ; /* 0x3c */
    u_lane_swap11                   lane_swap11                      ; /* 0x40 */
    u_lane_swap12                   lane_swap12                      ; /* 0x44 */
    u_lane_swap13                   lane_swap13                      ; /* 0x48 */
    u_lane_swap14                   lane_swap14                      ; /* 0x4c */
    u_lane_swap15                   lane_swap15                      ; /* 0x50 */
    u_lane_swap16                   lane_swap16                      ; /* 0x54 */
    u_channel_sel                   channel_sel                      ; /* 0x58 */
    unsigned int                    reserved_0[41]                   ; /* 0x5c~0xfc, 41 is byte num */
    u_cbar_ctrl                     cbar_ctrl                        ; /* 0x100 */
    u_test_hb_num                   test_hb_num                      ; /* 0x104 */
    u_test_vb_num                   test_vb_num                      ; /* 0x108 */
    u_test_act_num                  test_act_num                     ; /* 0x10c */
    u_test_hv_width                 test_hv_width                    ; /* 0x110 */
    u_test_hmid                     test_hmid                        ; /* 0x114 */
    u_test_start_pos                test_start_pos                   ; /* 0x118 */
    u_test_cbar_upd                 test_cbar_upd                    ; /* 0x11c */
} s_vbotx_regs_type;

/* declare the struct pointor of the module vbotx */
extern s_vbotx_regs_type *g_vbotx_all_reg;

#endif /* __c_union_define_vbotx_h__ */
