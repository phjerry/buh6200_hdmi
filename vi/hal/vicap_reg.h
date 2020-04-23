/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vicap register define
 * Author: sdk
 * Create: 2019-12-14
 */

#ifndef __VICAP_REG_H__
#define __VICAP_REG_H__

/* define the union U_WK_MODE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int power_mode : 1;  /* [0]  */
        unsigned int reserved_0 : 14; /* [14..1]  */
        unsigned int cb_first : 1;    /* [15]  */
        unsigned int oe_mode : 1;     /* [16]  */
        unsigned int reserved_1 : 13; /* [29..17]  */
        unsigned int mix_mode : 1;    /* [30]  */
        unsigned int mix_en : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_wk_mode;

/* define the union U_MEM_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int s14_rfshd_rm : 4;   /* [3..0]  */
        unsigned int s14_rfshs_rm : 4;   /* [7..4]  */
        unsigned int s14_rasehd_rm : 4;  /* [11..8]  */
        unsigned int s14_rashd_rm : 4;   /* [15..12]  */
        unsigned int s14_rfshd_rme : 1;  /* [16]  */
        unsigned int s14_rfshs_rme : 1;  /* [17]  */
        unsigned int s14_rasehd_rme : 1; /* [18]  */
        unsigned int s14_rashd_rme : 1;  /* [19]  */
        unsigned int s14_rfthd_rma : 4;  /* [23..20]  */
        unsigned int s14_rfthd_rmb : 4;  /* [27..24]  */
        unsigned int s14_rfthd_rmea : 1; /* [28]  */
        unsigned int s14_rfthd_rmeb : 1; /* [29]  */
        unsigned int reserved_0 : 2;     /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_mem_ctrl;

/* define the union U_MEM_CTRL1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int s14_rom_rm : 4;  /* [3..0]  */
        unsigned int s14_rom_rme : 1; /* [4]  */
        unsigned int reserved_0 : 27; /* [31..5]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_mem_ctrl1;

/* define the union U_HDMI2VI_ADAPTER */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hdmi2vi_adapter_stat : 16; /* [15..0]  */
        unsigned int hdmi2vi_adapter : 16;      /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_hdmi2vi_adapter;

/* define the union U_AXI_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 4;  /* [3..0]  */
        unsigned int outstanding : 4; /* [7..4]  */
        unsigned int reserved_1 : 24; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_axi_cfg;

/* define the union U_MAC_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int req_prio : 8;    /* [7..0]  */
        unsigned int reserved_0 : 8;  /* [15..8]  */
        unsigned int ch0y_sel : 1;    /* [16]  */
        unsigned int ch0c_sel : 1;    /* [17]  */
        unsigned int ch1y_sel : 1;    /* [18]  */
        unsigned int ch1c_sel : 1;    /* [19]  */
        unsigned int reserved_1 : 12; /* [31..20]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_mac_cfg;

/* define the union U_PROT_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int prot_ctrl : 1;   /* [0]  */
        unsigned int reserved_0 : 31; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_prot_ctrl;

/* define the union U_PROT_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int prot : 2;        /* [1..0]  */
        unsigned int reserved_0 : 30; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_prot_cfg;

/* define the union U_PT_SEL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int pt0_sel : 1;     /* [0]  */
        unsigned int pt1_sel : 1;     /* [1]  */
        unsigned int pt2_sel : 1;     /* [2]  */
        unsigned int pt3_sel : 1;     /* [3]  */
        unsigned int pt4_sel : 1;     /* [4]  */
        unsigned int pt5_sel : 1;     /* [5]  */
        unsigned int pt6_sel : 1;     /* [6]  */
        unsigned int pt7_sel : 1;     /* [7]  */
        unsigned int reserved_0 : 24; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_sel;

/* define the union U_MIX_ADAPTER_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hsync_mode : 1;  /* [0]  */
        unsigned int vsync_mode : 1;  /* [1]  */
        unsigned int reserved_0 : 30; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_mix_adapter_cfg;

/* define the union U_MIX_420_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hblank : 16;     /* [15..0]  */
        unsigned int reserved_0 : 14; /* [29..16]  */
        unsigned int cb_first : 1;    /* [30]  */
        unsigned int enable : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_mix_420_cfg;

/* define the union U_MIX_420_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int width : 16;  /* [15..0]  */
        unsigned int height : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_mix_420_size;

/* define the union U_ABM_SEL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int vicap_abm_sel : 1; /* [0]  */
        unsigned int hdmi_abm_sel : 1;  /* [1]  */
        unsigned int reserved_0 : 30;   /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_abm_sel;

/* define the union U_VICAP_INT_TEE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int int_ch0_tee : 1; /* [0]  */
        unsigned int reserved_0 : 15; /* [15..1]  */
        unsigned int int_pt0_tee : 1; /* [16]  */
        unsigned int int_pt1_tee : 1; /* [17]  */
        unsigned int reserved_1 : 14; /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_vicap_int_tee;

/* define the union U_VICAP_INT_MASK_TEE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int int_ch0_tee : 1; /* [0]  */
        unsigned int reserved_0 : 15; /* [15..1]  */
        unsigned int int_pt0_tee : 1; /* [16]  */
        unsigned int int_pt1_tee : 1; /* [17]  */
        unsigned int reserved_1 : 14; /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_vicap_int_mask_tee;

/* define the union U_APB_TIMEOUT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int timeout : 16;    /* [15..0]  */
        unsigned int reserved_0 : 15; /* [30..16]  */
        unsigned int enable : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_apb_timeout;

/* define the union U_VICAP_INT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int int_ch0 : 1;     /* [0]  */
        unsigned int reserved_0 : 15; /* [15..1]  */
        unsigned int int_pt0 : 1;     /* [16]  */
        unsigned int int_pt1 : 1;     /* [17]  */
        unsigned int int_pt2 : 1;     /* [18]  */
        unsigned int int_pt3 : 1;     /* [19]  */
        unsigned int int_pt4 : 1;     /* [20]  */
        unsigned int int_pt5 : 1;     /* [21]  */
        unsigned int int_pt6 : 1;     /* [22]  */
        unsigned int int_pt7 : 1;     /* [23]  */
        unsigned int reserved_1 : 8;  /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_vicap_int;

/* define the union U_VICAP_INT_MASK */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int int_ch0 : 1;     /* [0]  */
        unsigned int reserved_0 : 15; /* [15..1]  */
        unsigned int int_pt0 : 1;     /* [16]  */
        unsigned int int_pt1 : 1;     /* [17]  */
        unsigned int int_pt2 : 1;     /* [18]  */
        unsigned int int_pt3 : 1;     /* [19]  */
        unsigned int int_pt4 : 1;     /* [20]  */
        unsigned int int_pt5 : 1;     /* [21]  */
        unsigned int int_pt6 : 1;     /* [22]  */
        unsigned int int_pt7 : 1;     /* [23]  */
        unsigned int reserved_1 : 8;  /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_vicap_int_mask;

/* define the union U_PT_INTF_MOD */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int mode : 1;        /* [0]  */
        unsigned int reserved_0 : 30; /* [30..1]  */
        unsigned int enable : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_intf_mod;

/* define the union U_PT_OFFSET0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int offset : 6;     /* [5..0]  */
        unsigned int reserved_0 : 9; /* [14..6]  */
        unsigned int rev : 1;        /* [15]  */
        unsigned int mask : 16;      /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_offset0;

/* define the union U_PT_OFFSET1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int offset : 6;     /* [5..0]  */
        unsigned int reserved_0 : 9; /* [14..6]  */
        unsigned int rev : 1;        /* [15]  */
        unsigned int mask : 16;      /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_offset1;

/* define the union U_PT_OFFSET2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int offset : 6;     /* [5..0]  */
        unsigned int reserved_0 : 9; /* [14..6]  */
        unsigned int rev : 1;        /* [15]  */
        unsigned int mask : 16;      /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_offset2;

/* define the union U_PT_UNIFY_TIMING_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int de_sel : 2;     /* [1..0]  */
        unsigned int de_inv : 1;     /* [2]  */
        unsigned int reserved_0 : 5; /* [7..3]  */
        unsigned int hsync_sel : 2;  /* [9..8]  */
        unsigned int hsync_inv : 1;  /* [10]  */
        unsigned int hsync_and : 2;  /* [12..11]  */
        unsigned int hsync_mode : 2; /* [14..13]  */
        unsigned int reserved_1 : 1; /* [15]  */
        unsigned int vsync_sel : 2;  /* [17..16]  */
        unsigned int vsync_inv : 1;  /* [18]  */
        unsigned int vsync_mode : 2; /* [20..19]  */
        unsigned int reserved_2 : 3; /* [23..21]  */
        unsigned int field_sel : 2;  /* [25..24]  */
        unsigned int field_inv : 1;  /* [26]  */
        unsigned int reserved_3 : 5; /* [31..27]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_unify_timing_cfg;

/* define the union U_PT_GEN_TIMING_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 1;  /* [0]  */
        unsigned int hsync_mode : 1;  /* [1]  */
        unsigned int vsync_mode : 1;  /* [2]  */
        unsigned int reserved_1 : 27; /* [29..3]  */
        unsigned int gen_mode : 1;    /* [30]  */
        unsigned int enable : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_gen_timing_cfg;

/* define the union U_PT_UNIFY_DATA_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int comp_num : 2;    /* [1..0]  */
        unsigned int yc_seq : 1;      /* [2]  */
        unsigned int uv_seq : 1;      /* [3]  */
        unsigned int reserved_0 : 27; /* [30..4]  */
        unsigned int enable : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_unify_data_cfg;

/* define the union U_PT_GEN_DATA_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int de_move : 1;     /* [0]  */
        unsigned int hsync_move : 1;  /* [1]  */
        unsigned int vsync_move : 1;  /* [2]  */
        unsigned int hsync_reset : 1; /* [3]  */
        unsigned int vsync_reset : 1; /* [4]  */
        unsigned int data2_move : 1;  /* [5]  */
        unsigned int data1_move : 1;  /* [6]  */
        unsigned int data0_move : 1;  /* [7]  */
        unsigned int reserved_0 : 20; /* [27..8]  */
        unsigned int gen_mode : 2;    /* [29..28]  */
        unsigned int color_space : 1; /* [30]  */
        unsigned int enable : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_gen_data_cfg;

/* define the union U_PT_GEN_DATA_COEF */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int step_space : 8; /* [7..0]  */
        unsigned int inc_space : 8;  /* [15..8]  */
        unsigned int step_frame : 8; /* [23..16]  */
        unsigned int inc_frame : 8;  /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_gen_data_coef;

/* define the union U_PT_GEN_DATA_INIT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int data0 : 8;      /* [7..0]  */
        unsigned int data1 : 8;      /* [15..8]  */
        unsigned int data2 : 8;      /* [23..16]  */
        unsigned int reserved_0 : 8; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_gen_data_init;

/* define the union U_PT_YUV444_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 31; /* [30..0]  */
        unsigned int enable : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_yuv444_cfg;

/* define the union U_PT_HOR_SKIP */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int skip_cfg : 8;    /* [7..0]  */
        unsigned int reserved_0 : 24; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_hor_skip;

/* define the union U_PT_INTF_HFB */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hfb : 16;        /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_intf_hfb;

/* define the union u_pt_intf_hact */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int hact : 32; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_intf_hact;

/* define the union U_PT_INTF_HBB */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hbb : 16;        /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_intf_hbb;

/* define the union U_PT_INTF_VFB */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int vfb : 16;        /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_intf_vfb;

/* define the union U_PT_INTF_VACT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int vact : 16;       /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_intf_vact;

/* define the union U_PT_INTF_VBB */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int vbb : 16;        /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_intf_vbb;

/* define the union U_PT_INTF_VBFB */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int vbfb : 16;       /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_intf_vbfb;

/* define the union U_PT_INTF_VBACT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int vbact : 16;      /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_intf_vbact;

/* define the union U_PT_INTF_VBBB */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int vbbb : 16;       /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_intf_vbbb;

/* define the union U_PT_CLR_ERR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int clr_err : 1;     /* [0]  */
        unsigned int reserved_0 : 31; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_clr_err;

/* define the union U_PT_ERR_CNT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int width_err_cnt : 16;  /* [15..0]  */
        unsigned int height_err_cnt : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_err_cnt;

/* define the union U_PT_ERR_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int err_width : 16;  /* [15..0]  */
        unsigned int err_height : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_err_size;

/* define the union U_PT_STATUS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int de : 1;          /* [0]  */
        unsigned int hsync : 1;       /* [1]  */
        unsigned int vysnc : 1;       /* [2]  */
        unsigned int field : 1;       /* [3]  */
        unsigned int reserved_0 : 28; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_status;

/* define the union U_PT_BT656_STATUS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 8;  /* [7..0]  */
        unsigned int seav : 8;        /* [15..8]  */
        unsigned int reserved_1 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_bt656_status;

/* define the union U_PT_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int width : 16;  /* [15..0]  */
        unsigned int height : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_size;

/* define the union U_PT_INT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int fstart : 1;      /* [0]  */
        unsigned int width_err : 1;   /* [1]  */
        unsigned int height_err : 1;  /* [2]  */
        unsigned int reserved_0 : 29; /* [31..3]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_int;

/* define the union U_PT_INT_MASK */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int fstart : 1;      /* [0]  */
        unsigned int width_err : 1;   /* [1]  */
        unsigned int height_err : 1;  /* [2]  */
        unsigned int reserved_0 : 29; /* [31..3]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_pt_int_mask;

/* define the union U_CH_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 31; /* [30..0]  */
        unsigned int enable : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_ctrl;

/* define the union U_CH_REG_NEWER */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reg_newer : 1;   /* [0]  */
        unsigned int reserved_0 : 31; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_reg_newer;

/* define the union U_CH_ADAPTER_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hsync_mode : 1;  /* [0]  */
        unsigned int vsync_mode : 1;  /* [1]  */
        unsigned int reserved_0 : 30; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_adapter_cfg;

/* define the union U_CH_LINE_NUM */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int line_num : 16;   /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_line_num;

/* define the union U_CH_LINE_CNT_Y */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int line_cnt_y : 16; /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_line_cnt_y;

/* define the union U_CH_LINE_CNT_C */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int line_cnt_c : 16; /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_line_cnt_c;

/* define the union U_CH_COUNT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int h_count : 16; /* [15..0]  */
        unsigned int v_count : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_count;

/* define the union U_CH_DLY0_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 16; /* [15..0]  */
        unsigned int v_dly_cfg : 16;  /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_dly0_cfg;

/* define the union U_CH_DLY1_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 16; /* [15..0]  */
        unsigned int v_dly_cfg : 16;  /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_dly1_cfg;

/* define the union U_CH_DLY2_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 16; /* [15..0]  */
        unsigned int v_dly_cfg : 16;  /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_dly2_cfg;

/* define the union U_CH_DOLBY_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int data_mode : 1;       /* [0]  */
        unsigned int dolby_tunnel_en : 1; /* [1]  */
        unsigned int dolby_wr_en : 1;     /* [2]  */
        unsigned int dolby_chk_en : 1;    /* [3]  */
        unsigned int reserved_0 : 28;     /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_dolby_cfg;

/* define the union U_CH_DOLBY_PACK_NUM */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int ch_dolby_pack_num : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;        /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_dolby_pack_num;

/* define the union U_CH_DOLBY_PACK_NUM_T */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int ch_dolby_pack_num : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;        /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_dolby_pack_num_t;

/* define the union U_CH_420_PROC_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 30;    /* [29..0]  */
        unsigned int uv_swap : 1;        /* [30]  */
        unsigned int yuv420_proc_en : 1; /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_420_proc_cfg;

/* define the union U_CH_WCH_MMU */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int wch_y_mmu_bypass : 1; /* [0]  */
        unsigned int wch_c_mmu_bypass : 1; /* [1]  */
        unsigned int reserved_0 : 30;      /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_wch_mmu;

/* define the union U_CH_WCH_Y_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 16; /* [15..0]  */
        unsigned int bit_width : 2;   /* [17..16]  */
        unsigned int bfield : 1;      /* [18]  */
        unsigned int interleave : 1;  /* [19]  */
        unsigned int fpki_mode : 1;   /* [20]  */
        unsigned int reserved_1 : 10; /* [30..21]  */
        unsigned int enable : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_wch_y_cfg;

/* define the union U_CH_WCH_Y_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int width : 16;  /* [15..0]  */
        unsigned int height : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_wch_y_size;

/* define the union U_CH_WCH_Y_STRIDE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int stride : 16;     /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_wch_y_stride;

/* define the union U_CH_WCH_C_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 16; /* [15..0]  */
        unsigned int bit_width : 2;   /* [17..16]  */
        unsigned int bfield : 1;      /* [18]  */
        unsigned int interleave : 1;  /* [19]  */
        unsigned int fpki_mode : 1;   /* [20]  */
        unsigned int reserved_1 : 10; /* [30..21]  */
        unsigned int enable : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_wch_c_cfg;

/* define the union U_CH_WCH_C_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int width : 16;  /* [15..0]  */
        unsigned int height : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_wch_c_size;

/* define the union U_CH_WCH_C_STRIDE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int stride : 16;     /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_wch_c_stride;

/* define the union U_CH_INT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int fstart : 1;      /* [0]  */
        unsigned int cc_int : 1;      /* [1]  */
        unsigned int buf_ovf : 1;     /* [2]  */
        unsigned int field_throw : 1; /* [3]  */
        unsigned int update_cfg : 1;  /* [4]  */
        unsigned int bus_err_c : 1;   /* [5]  */
        unsigned int bus_err_y : 1;   /* [6]  */
        unsigned int crc_err_md : 1;  /* [7]  */
        unsigned int cc_int_md : 1;   /* [8]  */
        unsigned int buf_ovf_md : 1;  /* [9]  */
        unsigned int eos_md : 1;      /* [10]  */
        unsigned int reserved_0 : 4;  /* [14..11]  */
        unsigned int fstart_dly0 : 1; /* [15]  */
        unsigned int fstart_dly1 : 1; /* [16]  */
        unsigned int fstart_dly2 : 1; /* [17]  */
        unsigned int hdmi_err0 : 1;   /* [18]  */
        unsigned int hdmi_err1 : 1;   /* [19]  */
        unsigned int hdmi_err2 : 1;   /* [20]  */
        unsigned int hdmi_err3 : 1;   /* [21]  */
        unsigned int hdmi_err4 : 1;   /* [22]  */
        unsigned int hdmi_err5 : 1;   /* [23]  */
        unsigned int hdmi_err6 : 1;   /* [24]  */
        unsigned int hdmi_err7 : 1;   /* [25]  */
        unsigned int cc_int_hdr : 1;  /* [26]  */
        unsigned int buf_ovf_hdr : 1; /* [27]  */
        unsigned int reserved_1 : 4;  /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_int;

/* define the union U_CH_INT_MASK */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int fstart : 1;      /* [0]  */
        unsigned int cc_int : 1;      /* [1]  */
        unsigned int buf_ovf : 1;     /* [2]  */
        unsigned int field_throw : 1; /* [3]  */
        unsigned int update_cfg : 1;  /* [4]  */
        unsigned int bus_err_c : 1;   /* [5]  */
        unsigned int bus_err_y : 1;   /* [6]  */
        unsigned int crc_err_md : 1;  /* [7]  */
        unsigned int cc_int_md : 1;   /* [8]  */
        unsigned int buf_ovf_md : 1;  /* [9]  */
        unsigned int eos_md : 1;      /* [10]  */
        unsigned int reserved_0 : 4;  /* [14..11]  */
        unsigned int fstart_dly0 : 1; /* [15]  */
        unsigned int fstart_dly1 : 1; /* [16]  */
        unsigned int fstart_dly2 : 1; /* [17]  */
        unsigned int hdmi_err0 : 1;   /* [18]  */
        unsigned int hdmi_err1 : 1;   /* [19]  */
        unsigned int hdmi_err2 : 1;   /* [20]  */
        unsigned int hdmi_err3 : 1;   /* [21]  */
        unsigned int hdmi_err4 : 1;   /* [22]  */
        unsigned int hdmi_err5 : 1;   /* [23]  */
        unsigned int hdmi_err6 : 1;   /* [24]  */
        unsigned int hdmi_err7 : 1;   /* [25]  */
        unsigned int cc_int_hdr : 1;  /* [26]  */
        unsigned int buf_ovf_hdr : 1; /* [27]  */
        unsigned int reserved_1 : 4;  /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_int_mask;

/* define the union U_CH_CROP_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int n0_en : 1;       /* [0]  */
        unsigned int n1_en : 1;       /* [1]  */
        unsigned int reserved_0 : 30; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_crop_cfg;

/* define the union U_CH_CROP_WIN */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int width : 13;     /* [12..0]  */
        unsigned int reserved_0 : 3; /* [15..13]  */
        unsigned int height : 13;    /* [28..16]  */
        unsigned int reserved_1 : 3; /* [31..29]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_crop_win;

/* define the union U_CH_CROP0_START */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int x_start : 13;   /* [12..0]  */
        unsigned int reserved_0 : 3; /* [15..13]  */
        unsigned int y_start : 13;   /* [28..16]  */
        unsigned int reserved_1 : 3; /* [31..29]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_crop0_start;

/* define the union U_CH_CROP0_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int width : 13;     /* [12..0]  */
        unsigned int reserved_0 : 3; /* [15..13]  */
        unsigned int height : 13;    /* [28..16]  */
        unsigned int reserved_1 : 3; /* [31..29]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_crop0_size;

/* define the union U_CH_CROP1_START */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int x_start : 13;   /* [12..0]  */
        unsigned int reserved_0 : 3; /* [15..13]  */
        unsigned int y_start : 13;   /* [28..16]  */
        unsigned int reserved_1 : 3; /* [31..29]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_crop1_start;

/* define the union U_CH_CROP1_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int width : 13;     /* [12..0]  */
        unsigned int reserved_0 : 3; /* [15..13]  */
        unsigned int height : 13;    /* [28..16]  */
        unsigned int reserved_1 : 3; /* [31..29]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_crop1_size;

/* define the union U_CH_CSC_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 31; /* [30..0]  */
        unsigned int enable : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_csc_cfg;

/* define the union U_CH_CSC_COEF0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 1; /* [0]  */
        unsigned int coef00 : 15;    /* [15..1]  */
        unsigned int reserved_1 : 1; /* [16]  */
        unsigned int coef01 : 15;    /* [31..17]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_csc_coef0;

/* define the union U_CH_CSC_COEF1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 1; /* [0]  */
        unsigned int coef02 : 15;    /* [15..1]  */
        unsigned int reserved_1 : 1; /* [16]  */
        unsigned int coef10 : 15;    /* [31..17]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_csc_coef1;

/* define the union U_CH_CSC_COEF2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 1; /* [0]  */
        unsigned int coef11 : 15;    /* [15..1]  */
        unsigned int reserved_1 : 1; /* [16]  */
        unsigned int coef12 : 15;    /* [31..17]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_csc_coef2;

/* define the union U_CH_CSC_COEF3 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 1; /* [0]  */
        unsigned int coef20 : 15;    /* [15..1]  */
        unsigned int reserved_1 : 1; /* [16]  */
        unsigned int coef21 : 15;    /* [31..17]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_csc_coef3;

/* define the union U_CH_CSC_COEF4 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 1;  /* [0]  */
        unsigned int coef22 : 15;     /* [15..1]  */
        unsigned int reserved_1 : 1;  /* [16]  */
        unsigned int reserved_2 : 15; /* [31..17]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_csc_coef4;

/* define the union U_CH_CSC_IN_DC0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 5; /* [4..0]  */
        unsigned int in_dc0 : 11;    /* [15..5]  */
        unsigned int reserved_1 : 5; /* [20..16]  */
        unsigned int in_dc1 : 11;    /* [31..21]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_csc_in_dc0;

/* define the union U_CH_CSC_IN_DC1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 5;  /* [4..0]  */
        unsigned int in_dc2 : 11;     /* [15..5]  */
        unsigned int reserved_1 : 5;  /* [20..16]  */
        unsigned int reserved_2 : 11; /* [31..21]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_csc_in_dc1;

/* define the union U_CH_CSC_OUT_DC0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 5; /* [4..0]  */
        unsigned int out_dc0 : 11;   /* [15..5]  */
        unsigned int reserved_1 : 5; /* [20..16]  */
        unsigned int out_dc1 : 11;   /* [31..21]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_csc_out_dc0;

/* define the union U_CH_CSC_OUT_DC1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 5;  /* [4..0]  */
        unsigned int out_dc2 : 11;    /* [15..5]  */
        unsigned int reserved_1 : 5;  /* [20..16]  */
        unsigned int reserved_2 : 11; /* [31..21]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_csc_out_dc1;

/* define the union U_CH_CSC_INK_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int x_pos : 13;       /* [12..0]  */
        unsigned int y_pos : 13;       /* [25..13]  */
        unsigned int color_mode : 2;   /* [27..26]  */
        unsigned int data_fmt : 1;     /* [28]  */
        unsigned int cross_enable : 1; /* [29]  */
        unsigned int ink_sel : 1;      /* [30]  */
        unsigned int ink_en : 1;       /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_csc_ink_ctrl;

/* define the union U_CH_CSC_INK_DATA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int ink_d2 : 10;    /* [9..0]  */
        unsigned int ink_d1 : 10;    /* [19..10]  */
        unsigned int ink_d0 : 10;    /* [29..20]  */
        unsigned int reserved_0 : 2; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_csc_ink_data;

/* define the union U_CH_SKIP_Y_WIN */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int width : 3;       /* [2..0]  */
        unsigned int reserved_0 : 13; /* [15..3]  */
        unsigned int height : 2;      /* [17..16]  */
        unsigned int reserved_1 : 14; /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_skip_y_win;

/* define the union U_CH_SKIP_C_WIN */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int width : 3;       /* [2..0]  */
        unsigned int reserved_0 : 13; /* [15..3]  */
        unsigned int height : 2;      /* [17..16]  */
        unsigned int reserved_1 : 14; /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_skip_c_win;

/* define the union U_CH_SKIP_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int width : 16;  /* [15..0]  */
        unsigned int height : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_skip_size;

/* define the union U_CH_COEF_UPDATE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int lhcoef_update : 1; /* [0]  */
        unsigned int chcoef_update : 1; /* [1]  */
        unsigned int reserved_0 : 30;   /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_coef_update;

/* define the union U_CH_COEF_RSEL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int lhcoef_read_sel : 1; /* [0]  */
        unsigned int chcoef_read_sel : 1; /* [1]  */
        unsigned int reserved_0 : 30;     /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_coef_rsel;

/* define the union U_CH_LHFIR_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int ratio : 24;     /* [23..0]  */
        unsigned int reserved_0 : 5; /* [28..24]  */
        unsigned int fir_en : 1;     /* [29]  */
        unsigned int mid_en : 1;     /* [30]  */
        unsigned int enable : 1;     /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_lhfir_cfg;

/* define the union U_CH_CHFIR_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int ratio : 24;     /* [23..0]  */
        unsigned int reserved_0 : 5; /* [28..24]  */
        unsigned int fir_en : 1;     /* [29]  */
        unsigned int mid_en : 1;     /* [30]  */
        unsigned int enable : 1;     /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_chfir_cfg;

/* define the union U_CH_LHFIR_OFFSET */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int offset : 28;    /* [27..0]  */
        unsigned int reserved_0 : 4; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_lhfir_offset;

/* define the union U_CH_CHFIR_OFFSET */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int offset : 28;    /* [27..0]  */
        unsigned int reserved_0 : 4; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_chfir_offset;

/* define the union U_CH_LVFIR_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int ratio : 5;       /* [4..0]  */
        unsigned int reserved_0 : 26; /* [30..5]  */
        unsigned int enable : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_lvfir_cfg;

/* define the union U_CH_CVFIR_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int ratio : 5;       /* [4..0]  */
        unsigned int reserved_0 : 26; /* [30..5]  */
        unsigned int enable : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_cvfir_cfg;

/* define the union U_CH_LFIR_IN_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int width : 13;     /* [12..0]  */
        unsigned int reserved_0 : 3; /* [15..13]  */
        unsigned int height : 13;    /* [28..16]  */
        unsigned int reserved_1 : 3; /* [31..29]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_lfir_in_size;

/* define the union U_CH_CFIR_IN_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int width : 13;     /* [12..0]  */
        unsigned int reserved_0 : 3; /* [15..13]  */
        unsigned int height : 13;    /* [28..16]  */
        unsigned int reserved_1 : 3; /* [31..29]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_cfir_in_size;

/* define the union U_CH_LFIR_OUT_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int width : 13;     /* [12..0]  */
        unsigned int reserved_0 : 3; /* [15..13]  */
        unsigned int height : 13;    /* [28..16]  */
        unsigned int reserved_1 : 3; /* [31..29]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_lfir_out_size;

/* define the union U_CH_CFIR_OUT_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int width : 13;     /* [12..0]  */
        unsigned int reserved_0 : 3; /* [15..13]  */
        unsigned int height : 13;    /* [28..16]  */
        unsigned int reserved_1 : 3; /* [31..29]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_cfir_out_size;

/* define the union U_CH_IN_FORMAT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int in_fmt : 2;      /* [1..0]  */
        unsigned int reserved_0 : 30; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_in_format;

/* define the union U_CH_3D_SPLIT_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int mode : 1;        /* [0]  */
        unsigned int reserved_0 : 30; /* [30..1]  */
        unsigned int enable : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_3d_split_cfg;

/* define the union U_CH_DITHER_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int mode : 1;        /* [0]  */
        unsigned int round : 1;       /* [1]  */
        unsigned int reserved_0 : 29; /* [30..2]  */
        unsigned int enable : 1;      /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_dither_cfg;

/* define the union U_CH_CLIP_Y_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int min : 16; /* [15..0]  */
        unsigned int max : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_clip_y_cfg;

/* define the union U_CH_CLIP_C_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int min : 16; /* [15..0]  */
        unsigned int max : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_clip_c_cfg;

/* define the union U_CH_LHFIR_COEF0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int coef0 : 10;     /* [9..0]  */
        unsigned int coef1 : 10;     /* [19..10]  */
        unsigned int coef2 : 10;     /* [29..20]  */
        unsigned int reserved_0 : 2; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_lhfir_coef0;

/* define the union U_CH_LHFIR_COEF1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int coef3 : 10;     /* [9..0]  */
        unsigned int coef4 : 10;     /* [19..10]  */
        unsigned int coef5 : 10;     /* [29..20]  */
        unsigned int reserved_0 : 2; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_lhfir_coef1;

/* define the union U_CH_LHFIR_COEF2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int coef6 : 10;      /* [9..0]  */
        unsigned int coef7 : 10;      /* [19..10]  */
        unsigned int reserved_0 : 12; /* [31..20]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_lhfir_coef2;

/* define the union U_CH_CHFIR_COEF0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int coef0 : 10;     /* [9..0]  */
        unsigned int coef1 : 10;     /* [19..10]  */
        unsigned int coef2 : 10;     /* [29..20]  */
        unsigned int reserved_0 : 2; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_chfir_coef0;

/* define the union U_CH_CHFIR_COEF1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int coef3 : 10;     /* [9..0]  */
        unsigned int coef4 : 10;     /* [19..10]  */
        unsigned int coef5 : 10;     /* [29..20]  */
        unsigned int reserved_0 : 2; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_chfir_coef1;

/* define the union U_CH_CHFIR_COEF2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int coef6 : 10;      /* [9..0]  */
        unsigned int coef7 : 10;      /* [19..10]  */
        unsigned int reserved_0 : 12; /* [31..20]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_chfir_coef2;

/* define the union U_CH_CHECK_SUM_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int check_sum_yc_en : 1;       /* [0]  */
        unsigned int check_sum_metadata_en : 1; /* [1]  */
        unsigned int reserved_0 : 30;           /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_check_sum_cfg;

/* define the union U_CH_HDR_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int max_metadata_length : 16; /* [15..0]  */
        unsigned int reserved_0 : 13;          /* [28..16]  */
        unsigned int hdr_det_en : 1;           /* [29]  */
        unsigned int hdr_wr_en : 1;            /* [30]  */
        unsigned int hdr_en : 1;               /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_hdr_ctrl;

/* define the union U_CH_DET_Y_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int wch_y_width : 16;  /* [15..0]  */
        unsigned int wch_y_height : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_det_y_size;

/* define the union U_CH_DET_C_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int wch_c_width : 16;  /* [15..0]  */
        unsigned int wch_c_height : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_ch_det_c_size;

/* define the union U_ZME_8K_0_HINFO */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int out_width : 16;  /* [15..0]  */
        unsigned int ck_gt_en : 1;    /* [16]  */
        unsigned int out_fmt : 3;     /* [19..17]  */
        unsigned int reserved_0 : 12; /* [31..20]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_0_hinfo;

/* define the union U_ZME_8K_0_HSP */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hratio : 24;    /* [23..0]  */
        unsigned int hfir_order : 1; /* [24]  */
        unsigned int chfir_mode : 1; /* [25]  */
        unsigned int lhfir_mode : 1; /* [26]  */
        unsigned int non_lnr_en : 1; /* [27]  */
        unsigned int chmid_en : 1;   /* [28]  */
        unsigned int lhmid_en : 1;   /* [29]  */
        unsigned int chfir_en : 1;   /* [30]  */
        unsigned int lhfir_en : 1;   /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_0_hsp;

/* define the union U_ZME_8K_0_HLOFFSET */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int lhfir_offset : 28; /* [27..0]  */
        unsigned int reserved_0 : 4;    /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_0_hloffset;

/* define the union U_ZME_8K_0_HCOFFSET */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int chfir_offset : 28; /* [27..0]  */
        unsigned int reserved_0 : 4;    /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_0_hcoffset;

/* define the union U_ZME_8K_0_HZONE0DELTA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int zone0_delta : 22; /* [21..0]  */
        unsigned int reserved_0 : 10;  /* [31..22]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_0_hzone0delta;

/* define the union U_ZME_8K_0_HZONE2DELTA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int zone2_delta : 22; /* [21..0]  */
        unsigned int reserved_0 : 10;  /* [31..22]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_0_hzone2delta;

/* define the union U_ZME_8K_0_HZONEEND */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int zone0_end : 12; /* [11..0]  */
        unsigned int zone1_end : 12; /* [23..12]  */
        unsigned int reserved_0 : 8; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_0_hzoneend;

/* define the union U_ZME_8K_0_HL_SHOOTCTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hl_coring : 8;         /* [7..0]  */
        unsigned int hl_gain : 6;           /* [13..8]  */
        unsigned int hl_coringadj_en : 1;   /* [14]  */
        unsigned int hl_flatdect_mode : 1;  /* [15]  */
        unsigned int hl_shootctrl_mode : 2; /* [17..16]  */
        unsigned int hl_shootctrl_en : 1;   /* [18]  */
        unsigned int reserved_0 : 13;       /* [31..19]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_0_hl_shootctrl;

/* define the union U_ZME_8K_0_HC_SHOOTCTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hc_coring : 8;         /* [7..0]  */
        unsigned int hc_gain : 6;           /* [13..8]  */
        unsigned int hc_coringadj_en : 1;   /* [14]  */
        unsigned int hc_flatdect_mode : 1;  /* [15]  */
        unsigned int hc_shootctrl_mode : 2; /* [17..16]  */
        unsigned int hc_shootctrl_en : 1;   /* [18]  */
        unsigned int reserved_0 : 13;       /* [31..19]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_0_hc_shootctrl;

/* define the union U_ZME_8K_0_HRCOEF */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int apb_vhd_hf_cren : 1; /* [0]  */
        unsigned int apb_vhd_hf_lren : 1; /* [1]  */
        unsigned int reserved_0 : 30;     /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_0_hrcoef;

/* define the union U_ZME_8K_0_HYCOEFAD */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int apb_vhd_hycoef_raddr : 8; /* [7..0]  */
        unsigned int reserved_0 : 24;          /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_0_hycoefad;

/* define the union U_ZME_8K_0_HCCOEFAD */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int apb_vhd_hccoef_raddr : 8; /* [7..0]  */
        unsigned int reserved_0 : 24;          /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_0_hccoefad;

/* define the union U_ZME_8K_0_IRESO */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int in_width : 16;   /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_0_ireso;

/* define the union U_ZME_8K_1_HINFO */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int out_width : 16;  /* [15..0]  */
        unsigned int ck_gt_en : 1;    /* [16]  */
        unsigned int out_fmt : 3;     /* [19..17]  */
        unsigned int reserved_0 : 12; /* [31..20]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_1_hinfo;

/* define the union U_ZME_8K_1_HSP */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hratio : 24;    /* [23..0]  */
        unsigned int hfir_order : 1; /* [24]  */
        unsigned int chfir_mode : 1; /* [25]  */
        unsigned int lhfir_mode : 1; /* [26]  */
        unsigned int non_lnr_en : 1; /* [27]  */
        unsigned int chmid_en : 1;   /* [28]  */
        unsigned int lhmid_en : 1;   /* [29]  */
        unsigned int chfir_en : 1;   /* [30]  */
        unsigned int lhfir_en : 1;   /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_1_hsp;

/* define the union U_ZME_8K_1_HLOFFSET */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int lhfir_offset : 28; /* [27..0]  */
        unsigned int reserved_0 : 4;    /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_1_hloffset;

/* define the union U_ZME_8K_1_HCOFFSET */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int chfir_offset : 28; /* [27..0]  */
        unsigned int reserved_0 : 4;    /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_1_hcoffset;

/* define the union U_ZME_8K_1_HZONE0DELTA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int zone0_delta : 22; /* [21..0]  */
        unsigned int reserved_0 : 10;  /* [31..22]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_1_hzone0delta;

/* define the union U_ZME_8K_1_HZONE2DELTA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int zone2_delta : 22; /* [21..0]  */
        unsigned int reserved_0 : 10;  /* [31..22]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_1_hzone2delta;

/* define the union U_ZME_8K_1_HZONEEND */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int zone0_end : 12; /* [11..0]  */
        unsigned int zone1_end : 12; /* [23..12]  */
        unsigned int reserved_0 : 8; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_1_hzoneend;

/* define the union U_ZME_8K_1_HL_SHOOTCTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hl_coring : 8;         /* [7..0]  */
        unsigned int hl_gain : 6;           /* [13..8]  */
        unsigned int hl_coringadj_en : 1;   /* [14]  */
        unsigned int hl_flatdect_mode : 1;  /* [15]  */
        unsigned int hl_shootctrl_mode : 2; /* [17..16]  */
        unsigned int hl_shootctrl_en : 1;   /* [18]  */
        unsigned int reserved_0 : 13;       /* [31..19]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_1_hl_shootctrl;

/* define the union U_ZME_8K_1_HC_SHOOTCTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hc_coring : 8;         /* [7..0]  */
        unsigned int hc_gain : 6;           /* [13..8]  */
        unsigned int hc_coringadj_en : 1;   /* [14]  */
        unsigned int hc_flatdect_mode : 1;  /* [15]  */
        unsigned int hc_shootctrl_mode : 2; /* [17..16]  */
        unsigned int hc_shootctrl_en : 1;   /* [18]  */
        unsigned int reserved_0 : 13;       /* [31..19]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_1_hc_shootctrl;

/* define the union U_ZME_8K_1_HRCOEF */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int apb_vhd_hf_cren : 1; /* [0]  */
        unsigned int apb_vhd_hf_lren : 1; /* [1]  */
        unsigned int reserved_0 : 30;     /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_1_hrcoef;

/* define the union U_ZME_8K_1_HYCOEFAD */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int apb_vhd_hycoef_raddr : 8; /* [7..0]  */
        unsigned int reserved_0 : 24;          /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_1_hycoefad;

/* define the union U_ZME_8K_1_HCCOEFAD */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int apb_vhd_hccoef_raddr : 8; /* [7..0]  */
        unsigned int reserved_0 : 24;          /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_1_hccoefad;

/* define the union U_ZME_8K_1_IRESO */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int in_width : 16;   /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_1_ireso;

/* define the union U_ZME_8K_2_HINFO */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int out_width : 16;  /* [15..0]  */
        unsigned int ck_gt_en : 1;    /* [16]  */
        unsigned int out_fmt : 3;     /* [19..17]  */
        unsigned int reserved_0 : 12; /* [31..20]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_2_hinfo;

/* define the union U_ZME_8K_2_HSP */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hratio : 24;    /* [23..0]  */
        unsigned int hfir_order : 1; /* [24]  */
        unsigned int chfir_mode : 1; /* [25]  */
        unsigned int lhfir_mode : 1; /* [26]  */
        unsigned int non_lnr_en : 1; /* [27]  */
        unsigned int chmid_en : 1;   /* [28]  */
        unsigned int lhmid_en : 1;   /* [29]  */
        unsigned int chfir_en : 1;   /* [30]  */
        unsigned int lhfir_en : 1;   /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_2_hsp;

/* define the union U_ZME_8K_2_HLOFFSET */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int lhfir_offset : 28; /* [27..0]  */
        unsigned int reserved_0 : 4;    /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_2_hloffset;

/* define the union U_ZME_8K_2_HCOFFSET */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int chfir_offset : 28; /* [27..0]  */
        unsigned int reserved_0 : 4;    /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_2_hcoffset;

/* define the union U_ZME_8K_2_HZONE0DELTA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int zone0_delta : 22; /* [21..0]  */
        unsigned int reserved_0 : 10;  /* [31..22]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_2_hzone0delta;

/* define the union U_ZME_8K_2_HZONE2DELTA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int zone2_delta : 22; /* [21..0]  */
        unsigned int reserved_0 : 10;  /* [31..22]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_2_hzone2delta;

/* define the union U_ZME_8K_2_HZONEEND */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int zone0_end : 12; /* [11..0]  */
        unsigned int zone1_end : 12; /* [23..12]  */
        unsigned int reserved_0 : 8; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_2_hzoneend;

/* define the union U_ZME_8K_2_HL_SHOOTCTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hl_coring : 8;         /* [7..0]  */
        unsigned int hl_gain : 6;           /* [13..8]  */
        unsigned int hl_coringadj_en : 1;   /* [14]  */
        unsigned int hl_flatdect_mode : 1;  /* [15]  */
        unsigned int hl_shootctrl_mode : 2; /* [17..16]  */
        unsigned int hl_shootctrl_en : 1;   /* [18]  */
        unsigned int reserved_0 : 13;       /* [31..19]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_2_hl_shootctrl;

/* define the union U_ZME_8K_2_HC_SHOOTCTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hc_coring : 8;         /* [7..0]  */
        unsigned int hc_gain : 6;           /* [13..8]  */
        unsigned int hc_coringadj_en : 1;   /* [14]  */
        unsigned int hc_flatdect_mode : 1;  /* [15]  */
        unsigned int hc_shootctrl_mode : 2; /* [17..16]  */
        unsigned int hc_shootctrl_en : 1;   /* [18]  */
        unsigned int reserved_0 : 13;       /* [31..19]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_2_hc_shootctrl;

/* define the union U_ZME_8K_2_HRCOEF */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int apb_vhd_hf_cren : 1; /* [0]  */
        unsigned int apb_vhd_hf_lren : 1; /* [1]  */
        unsigned int reserved_0 : 30;     /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_2_hrcoef;

/* define the union U_ZME_8K_2_HYCOEFAD */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int apb_vhd_hycoef_raddr : 8; /* [7..0]  */
        unsigned int reserved_0 : 24;          /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_2_hycoefad;

/* define the union U_ZME_8K_2_HCCOEFAD */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int apb_vhd_hccoef_raddr : 8; /* [7..0]  */
        unsigned int reserved_0 : 24;          /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_2_hccoefad;

/* define the union U_ZME_8K_2_IRESO */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int in_width : 16;   /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_2_ireso;

/* define the union U_ZME_8K_3_HINFO */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int out_width : 16;  /* [15..0]  */
        unsigned int ck_gt_en : 1;    /* [16]  */
        unsigned int out_fmt : 3;     /* [19..17]  */
        unsigned int reserved_0 : 12; /* [31..20]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_3_hinfo;

/* define the union U_ZME_8K_3_HSP */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hratio : 24;    /* [23..0]  */
        unsigned int hfir_order : 1; /* [24]  */
        unsigned int chfir_mode : 1; /* [25]  */
        unsigned int lhfir_mode : 1; /* [26]  */
        unsigned int non_lnr_en : 1; /* [27]  */
        unsigned int chmid_en : 1;   /* [28]  */
        unsigned int lhmid_en : 1;   /* [29]  */
        unsigned int chfir_en : 1;   /* [30]  */
        unsigned int lhfir_en : 1;   /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_3_hsp;

/* define the union U_ZME_8K_3_HLOFFSET */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int lhfir_offset : 28; /* [27..0]  */
        unsigned int reserved_0 : 4;    /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_3_hloffset;

/* define the union U_ZME_8K_3_HCOFFSET */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int chfir_offset : 28; /* [27..0]  */
        unsigned int reserved_0 : 4;    /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_3_hcoffset;

/* define the union U_ZME_8K_3_HZONE0DELTA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int zone0_delta : 22; /* [21..0]  */
        unsigned int reserved_0 : 10;  /* [31..22]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_3_hzone0delta;

/* define the union U_ZME_8K_3_HZONE2DELTA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int zone2_delta : 22; /* [21..0]  */
        unsigned int reserved_0 : 10;  /* [31..22]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_3_hzone2delta;

/* define the union U_ZME_8K_3_HZONEEND */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int zone0_end : 12; /* [11..0]  */
        unsigned int zone1_end : 12; /* [23..12]  */
        unsigned int reserved_0 : 8; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_3_hzoneend;

/* define the union U_ZME_8K_3_HL_SHOOTCTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hl_coring : 8;         /* [7..0]  */
        unsigned int hl_gain : 6;           /* [13..8]  */
        unsigned int hl_coringadj_en : 1;   /* [14]  */
        unsigned int hl_flatdect_mode : 1;  /* [15]  */
        unsigned int hl_shootctrl_mode : 2; /* [17..16]  */
        unsigned int hl_shootctrl_en : 1;   /* [18]  */
        unsigned int reserved_0 : 13;       /* [31..19]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_3_hl_shootctrl;

/* define the union U_ZME_8K_3_HC_SHOOTCTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hc_coring : 8;         /* [7..0]  */
        unsigned int hc_gain : 6;           /* [13..8]  */
        unsigned int hc_coringadj_en : 1;   /* [14]  */
        unsigned int hc_flatdect_mode : 1;  /* [15]  */
        unsigned int hc_shootctrl_mode : 2; /* [17..16]  */
        unsigned int hc_shootctrl_en : 1;   /* [18]  */
        unsigned int reserved_0 : 13;       /* [31..19]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_3_hc_shootctrl;

/* define the union U_ZME_8K_3_HRCOEF */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int apb_vhd_hf_cren : 1; /* [0]  */
        unsigned int apb_vhd_hf_lren : 1; /* [1]  */
        unsigned int reserved_0 : 30;     /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_3_hrcoef;

/* define the union U_ZME_8K_3_HYCOEFAD */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int apb_vhd_hycoef_raddr : 8; /* [7..0]  */
        unsigned int reserved_0 : 24;          /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_3_hycoefad;

/* define the union U_ZME_8K_3_HCCOEFAD */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int apb_vhd_hccoef_raddr : 8; /* [7..0]  */
        unsigned int reserved_0 : 24;          /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_3_hccoefad;

/* define the union U_ZME_8K_3_IRESO */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int in_width : 16;   /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_3_ireso;

/* define the union U_ZME_8K_4_HINFO */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int out_width : 16;  /* [15..0]  */
        unsigned int ck_gt_en : 1;    /* [16]  */
        unsigned int out_fmt : 3;     /* [19..17]  */
        unsigned int reserved_0 : 12; /* [31..20]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_4_hinfo;

/* define the union U_ZME_8K_4_HSP */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hratio : 24;    /* [23..0]  */
        unsigned int hfir_order : 1; /* [24]  */
        unsigned int chfir_mode : 1; /* [25]  */
        unsigned int lhfir_mode : 1; /* [26]  */
        unsigned int non_lnr_en : 1; /* [27]  */
        unsigned int chmid_en : 1;   /* [28]  */
        unsigned int lhmid_en : 1;   /* [29]  */
        unsigned int chfir_en : 1;   /* [30]  */
        unsigned int lhfir_en : 1;   /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_4_hsp;

/* define the union U_ZME_8K_4_HLOFFSET */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int lhfir_offset : 28; /* [27..0]  */
        unsigned int reserved_0 : 4;    /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_4_hloffset;

/* define the union U_ZME_8K_4_HCOFFSET */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int chfir_offset : 28; /* [27..0]  */
        unsigned int reserved_0 : 4;    /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_4_hcoffset;

/* define the union U_ZME_8K_4_HZONE0DELTA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int zone0_delta : 22; /* [21..0]  */
        unsigned int reserved_0 : 10;  /* [31..22]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_4_hzone0delta;

/* define the union U_ZME_8K_4_HZONE2DELTA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int zone2_delta : 22; /* [21..0]  */
        unsigned int reserved_0 : 10;  /* [31..22]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_4_hzone2delta;

/* define the union U_ZME_8K_4_HZONEEND */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int zone0_end : 12; /* [11..0]  */
        unsigned int zone1_end : 12; /* [23..12]  */
        unsigned int reserved_0 : 8; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_4_hzoneend;

/* define the union U_ZME_8K_4_HL_SHOOTCTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hl_coring : 8;         /* [7..0]  */
        unsigned int hl_gain : 6;           /* [13..8]  */
        unsigned int hl_coringadj_en : 1;   /* [14]  */
        unsigned int hl_flatdect_mode : 1;  /* [15]  */
        unsigned int hl_shootctrl_mode : 2; /* [17..16]  */
        unsigned int hl_shootctrl_en : 1;   /* [18]  */
        unsigned int reserved_0 : 13;       /* [31..19]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_4_hl_shootctrl;

/* define the union U_ZME_8K_4_HC_SHOOTCTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int hc_coring : 8;         /* [7..0]  */
        unsigned int hc_gain : 6;           /* [13..8]  */
        unsigned int hc_coringadj_en : 1;   /* [14]  */
        unsigned int hc_flatdect_mode : 1;  /* [15]  */
        unsigned int hc_shootctrl_mode : 2; /* [17..16]  */
        unsigned int hc_shootctrl_en : 1;   /* [18]  */
        unsigned int reserved_0 : 13;       /* [31..19]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_4_hc_shootctrl;

/* define the union U_ZME_8K_4_HRCOEF */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int apb_vhd_hf_cren : 1; /* [0]  */
        unsigned int apb_vhd_hf_lren : 1; /* [1]  */
        unsigned int reserved_0 : 30;     /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_4_hrcoef;

/* define the union U_ZME_8K_4_HYCOEFAD */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int apb_vhd_hycoef_raddr : 8; /* [7..0]  */
        unsigned int reserved_0 : 24;          /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_4_hycoefad;

/* define the union U_ZME_8K_4_HCCOEFAD */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int apb_vhd_hccoef_raddr : 8; /* [7..0]  */
        unsigned int reserved_0 : 24;          /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_4_hccoefad;

/* define the union U_ZME_8K_4_IRESO */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int in_width : 16;   /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_zme_8k_4_ireso;

/* define the union U_SMMU_SCR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int glb_bypass : 1;  /* [0]  */
        unsigned int reserved_0 : 3;  /* [3..1]  */
        unsigned int page_typ_s : 4;  /* [7..4]  */
        unsigned int reserved_1 : 23; /* [30..8]  */
        unsigned int src_lock : 1;    /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_scr;

/* define the union U_SMMU_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 3;  /* [2..0]  */
        unsigned int int_en : 1;      /* [3]  */
        unsigned int page_typ_ns : 4; /* [7..4]  */
        unsigned int reserved_1 : 8;  /* [15..8]  */
        unsigned int ptw_pf : 4;      /* [19..16]  */
        unsigned int reserved_2 : 12; /* [31..20]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_ctrl;

/* define the union U_SMMU_LP_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int auto_clk_gt_en : 1; /* [0]  */
        unsigned int com_clk_gt_en : 1;  /* [1]  */
        unsigned int mst_clk_gt_en : 1;  /* [2]  */
        unsigned int reserved_0 : 29;    /* [31..3]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_lp_ctrl;

/* define the union U_SMMU_MEM_SPEEDCTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rfs_emaw : 2;   /* [1..0]  */
        unsigned int rfs_ema : 3;    /* [4..2]  */
        unsigned int rfs_ret1n : 1;  /* [5]  */
        unsigned int reserved_0 : 2; /* [7..6]  */
        unsigned int rashd_rme : 1;  /* [8]  */
        unsigned int rfshd_rme : 1;  /* [9]  */
        unsigned int rfshs_rme : 1;  /* [10]  */
        unsigned int rfsehd_rme : 1; /* [11]  */
        unsigned int rashd_rm : 4;   /* [15..12]  */
        unsigned int rfshd_rm : 4;   /* [19..16]  */
        unsigned int rfshs_rm : 4;   /* [23..20]  */
        unsigned int rfsehd_rm : 4;  /* [27..24]  */
        unsigned int reserved_1 : 4; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_mem_speedctrl;

/* define the union u_smmu_intmask_s */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int ints_tlbmiss_msk : 1;       /* [0]  */
        unsigned int ints_ptw_trans_msk : 1;     /* [1]  */
        unsigned int ints_tlbinvalid_rd_msk : 1; /* [2]  */
        unsigned int ints_tlbinvalid_wr_msk : 1; /* [3]  */
        unsigned int ints_tlbunmatch_rd_msk : 1; /* [4]  */
        unsigned int ints_tlbunmatch_wr_msk : 1; /* [5]  */
        unsigned int reserved_0 : 26;            /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_intmask_s;

/* define the union u_smmu_intraw_s */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int ints_tlbmiss_raw : 1;       /* [0]  */
        unsigned int ints_ptw_trans_raw : 1;     /* [1]  */
        unsigned int ints_tlbinvalid_rd_raw : 1; /* [2]  */
        unsigned int ints_tlbinvalid_wr_raw : 1; /* [3]  */
        unsigned int ints_tlbunmatch_rd_raw : 1; /* [4]  */
        unsigned int ints_tlbunmatch_wr_raw : 1; /* [5]  */
        unsigned int reserved_0 : 26;            /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_intraw_s;

/* define the union u_smmu_intstat_s */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int ints_tlbmiss_stat : 1;       /* [0]  */
        unsigned int ints_ptw_trans_stat : 1;     /* [1]  */
        unsigned int ints_tlbinvalid_rd_stat : 1; /* [2]  */
        unsigned int ints_tlbinvalid_wr_stat : 1; /* [3]  */
        unsigned int ints_tlbunmatch_rd_stat : 1; /* [4]  */
        unsigned int ints_tlbunmatch_wr_stat : 1; /* [5]  */
        unsigned int reserved_0 : 26;             /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_intstat_s;

/* define the union u_smmu_intclr_s */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int ints_tlbmiss_clr : 1;       /* [0]  */
        unsigned int ints_ptw_trans_clr : 1;     /* [1]  */
        unsigned int ints_tlbinvalid_rd_clr : 1; /* [2]  */
        unsigned int ints_tlbinvalid_wr_clr : 1; /* [3]  */
        unsigned int ints_tlbunmatch_rd_clr : 1; /* [4]  */
        unsigned int ints_tlbunmatch_wr_clr : 1; /* [5]  */
        unsigned int reserved_0 : 26;            /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_intclr_s;

/* define the union U_SMMU_INTMASK_NS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int intns_tlbmiss_msk : 1;       /* [0]  */
        unsigned int intns_ptw_trans_msk : 1;     /* [1]  */
        unsigned int intns_tlbinvalid_rd_msk : 1; /* [2]  */
        unsigned int intns_tlbinvalid_wr_msk : 1; /* [3]  */
        unsigned int intns_tlbunmatch_rd_msk : 1; /* [4]  */
        unsigned int intns_tlbunmatch_wr_msk : 1; /* [5]  */
        unsigned int reserved_0 : 26;             /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_intmask_ns;

/* define the union U_SMMU_INTRAW_NS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int intns_tlbmiss_raw : 1;       /* [0]  */
        unsigned int intns_ptw_trans_raw : 1;     /* [1]  */
        unsigned int intns_tlbinvalid_rd_raw : 1; /* [2]  */
        unsigned int intns_tlbinvalid_wr_raw : 1; /* [3]  */
        unsigned int intns_tlbunmatch_rd_raw : 1; /* [4]  */
        unsigned int intns_tlbunmatch_wr_raw : 1; /* [5]  */
        unsigned int reserved_0 : 26;             /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_intraw_ns;

/* define the union U_SMMU_INTSTAT_NS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int intns_tlbmiss_stat : 1;       /* [0]  */
        unsigned int intns_ptw_trans_stat : 1;     /* [1]  */
        unsigned int intns_tlbinvalid_rd_stat : 1; /* [2]  */
        unsigned int intns_tlbinvalid_wr_stat : 1; /* [3]  */
        unsigned int intns_tlbunmatch_rd_stat : 1; /* [4]  */
        unsigned int intns_tlbunmatch_wr_stat : 1; /* [5]  */
        unsigned int reserved_0 : 26;              /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_intstat_ns;

/* define the union U_SMMU_INTCLR_NS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int intns_tlbmiss_clr : 1;       /* [0]  */
        unsigned int intns_ptw_trans_clr : 1;     /* [1]  */
        unsigned int intns_tlbinvalid_rd_clr : 1; /* [2]  */
        unsigned int intns_tlbinvalid_wr_clr : 1; /* [3]  */
        unsigned int intns_tlbunmatch_rd_clr : 1; /* [4]  */
        unsigned int intns_tlbunmatch_wr_clr : 1; /* [5]  */
        unsigned int reserved_0 : 26;             /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_intclr_ns;

/* define the union U_SMMU_SCB_TTBR_H */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int scb_ttbr_h : 4;  /* [3..0]  */
        unsigned int reserved_0 : 28; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_scb_ttbr_h;

/* define the union U_SMMU_STAG_RD_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int scmd_tag_rd_en : 1; /* [0]  */
        unsigned int reserved_0 : 31;    /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_stag_rd_ctrl;

/* define the union U_SMMU_STAG_WR_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int scmd_tag_wr_en : 1; /* [0]  */
        unsigned int reserved_0 : 31;    /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_stag_wr_ctrl;

/* define the union u_smmu_err_rdaddr_h_s */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int err_s_rd_addr_h : 4; /* [3..0]  */
        unsigned int reserved_0 : 28;     /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_err_rdaddr_h_s;

/* define the union u_smmu_err_wraddr_h_s */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int err_s_wr_addr_h : 4; /* [3..0]  */
        unsigned int reserved_0 : 28;     /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_err_wraddr_h_s;

/* define the union U_SMMU_CB_TTBR_H */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int cb_ttbr_h : 4;   /* [3..0]  */
        unsigned int reserved_0 : 28; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_cb_ttbr_h;

/* define the union U_SMMU_TAG_RD_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int cmd_tag_rd_en : 1; /* [0]  */
        unsigned int reserved_0 : 31;   /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_tag_rd_ctrl;

/* define the union U_SMMU_TAG_WR_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int cmd_tag_wr_en : 1; /* [0]  */
        unsigned int reserved_0 : 31;   /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_tag_wr_ctrl;

/* define the union U_SMMU_ERR_RDADDR_H_NS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int err_ns_rd_addr_h : 4; /* [3..0]  */
        unsigned int reserved_0 : 28;      /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_err_rdaddr_h_ns;

/* define the union U_SMMU_ERR_WRADDR_H_NS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int err_ns_wr_addr_h : 4; /* [3..0]  */
        unsigned int reserved_0 : 28;      /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_err_wraddr_h_ns;

/* define the union u_smmu_fault_id_ptw_s */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int fault_iid_ptw_s : 16; /* [15..0]  */
        unsigned int fault_sid_ptw_s : 2;  /* [17..16]  */
        unsigned int reserved_0 : 14;      /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_fault_id_ptw_s;

/* define the union U_SMMU_FAULT_ID_PTW_NS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int fault_iid_ptw_ns : 16; /* [15..0]  */
        unsigned int fault_sid_ptw_ns : 2;  /* [17..16]  */
        unsigned int reserved_0 : 14;       /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_fault_id_ptw_ns;

/* define the union u_smmu_fault_id_wr_s */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int fault_iid_wr_s : 16; /* [15..0]  */
        unsigned int fault_sid_wr_s : 2;  /* [17..16]  */
        unsigned int reserved_0 : 14;     /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_fault_id_wr_s;

/* define the union U_SMMU_FAULT_ID_WR_NS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int fault_iid_wr_ns : 16; /* [15..0]  */
        unsigned int fault_sid_wr_ns : 2;  /* [17..16]  */
        unsigned int reserved_0 : 14;      /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_fault_id_wr_ns;

/* define the union u_smmu_fault_id_rd_s */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int fault_iid_rd_s : 16; /* [15..0]  */
        unsigned int fault_sid_rd_s : 2;  /* [17..16]  */
        unsigned int reserved_0 : 14;     /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_fault_id_rd_s;

/* define the union U_SMMU_FAULT_ID_RD_NS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int fault_iid_rd_ns : 16; /* [15..0]  */
        unsigned int fault_sid_rd_ns : 2;  /* [17..16]  */
        unsigned int reserved_0 : 14;      /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_fault_id_rd_ns;

/* define the union u_smmu_match_id_wr_s */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int match_iid_wr_s : 16; /* [15..0]  */
        unsigned int match_sid_wr_s : 2;  /* [17..16]  */
        unsigned int reserved_0 : 14;     /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_match_id_wr_s;

/* define the union U_SMMU_MATCH__ID_WR_NS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int match_iid_wr_ns : 16; /* [15..0]  */
        unsigned int match_sid_wr_ns : 2;  /* [17..16]  */
        unsigned int reserved_0 : 14;      /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_match__id_wr_ns;

/* define the union u_smmu_match_id_rd_s */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int match_iid_rd_s : 16; /* [15..0]  */
        unsigned int match_sid_rd_s : 2;  /* [17..16]  */
        unsigned int reserved_0 : 14;     /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_match_id_rd_s;

/* define the union U_SMMU_MATCH_ID_RD_NS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int match_iid_rd_ns : 16; /* [15..0]  */
        unsigned int match_sid_rd_ns : 2;  /* [17..16]  */
        unsigned int reserved_0 : 14;      /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_match_id_rd_ns;

/* define the union U_SMMU_PREF_DBG0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int dbg_pref_osd : 8;  /* [7..0]  */
        unsigned int dbg_pref_idle : 1; /* [8]  */
        unsigned int pref_dbg0 : 23;    /* [31..9]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_pref_dbg0;

/* define the union U_SMMU_TBU_DBG0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int r_out_cnt : 8;   /* [7..0]  */
        unsigned int r_in_cnt : 8;    /* [15..8]  */
        unsigned int r_dummy_cnt : 8; /* [23..16]  */
        unsigned int tbu_dbg0 : 7;    /* [30..24]  */
        unsigned int tbu_r_idle : 1;  /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_tbu_dbg0;

/* define the union U_SMMU_TBU_DBG1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int w_out_cnt : 8;   /* [7..0]  */
        unsigned int w_in_cnt : 8;    /* [15..8]  */
        unsigned int w_dummy_cnt : 8; /* [23..16]  */
        unsigned int tbu_dbg1 : 7;    /* [30..24]  */
        unsigned int tbu_w_idle : 1;  /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_tbu_dbg1;

/* define the union U_SMMU_MASTER_DBG0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int last_miss_cnt_rd : 10; /* [9..0]  */
        unsigned int cur_miss_cnt_rd : 10;  /* [19..10]  */
        unsigned int vld_debug_rd : 4;      /* [23..20]  */
        unsigned int rdy_debug_rd : 4;      /* [27..24]  */
        unsigned int in_out_cmd_cnt_rd : 4; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_master_dbg0;

/* define the union U_SMMU_MASTER_DBG1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int last_miss_cnt_wr : 10; /* [9..0]  */
        unsigned int cur_miss_cnt_wr : 10;  /* [19..10]  */
        unsigned int vld_debug_wr : 4;      /* [23..20]  */
        unsigned int rdy_debug_wr : 4;      /* [27..24]  */
        unsigned int in_out_cmd_cnt_wr : 4; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_master_dbg1;

/* define the union U_SMMU_MASTER_DBG2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int last_double_miss_cnt_rd : 8; /* [7..0]  */
        unsigned int cur_double_miss_cnt_rd : 8;  /* [15..8]  */
        unsigned int last_double_upd_cnt_rd : 8;  /* [23..16]  */
        unsigned int cur_double_upd_cnt_rd : 8;   /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_master_dbg2;

/* define the union U_SMMU_MASTER_DBG3 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int last_double_miss_cnt_wr : 8; /* [7..0]  */
        unsigned int cur_double_miss_cnt_wr : 8;  /* [15..8]  */
        unsigned int last_double_upd_cnt_wr : 7;  /* [22..16]  */
        unsigned int cur_double_upd_cnt_wr : 7;   /* [29..23]  */
        unsigned int mst_fsm_cur : 2;             /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_master_dbg3;

/* define the union U_SMMU_MASTER_DBG4 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int last_sel1_chn_miss_cnt_rd : 16; /* [15..0]  */
        unsigned int cur_sel1_chn_miss_cnt_rd : 16;  /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_master_dbg4;

/* define the union U_SMMU_MASTER_DBG5 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int last_sel2_chn_miss_cnt_rd : 16; /* [15..0]  */
        unsigned int cur_sel2_chn_miss_cnt_rd : 16;  /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_master_dbg5;

/* define the union U_SMMU_MASTER_DBG6 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int last_sel1_chn_miss_cnt_wr : 16; /* [15..0]  */
        unsigned int cur_sel1_chn_miss_cnt_wr : 16;  /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_master_dbg6;

/* define the union U_SMMU_MASTER_DBG7 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int last_sel2_chn_miss_cnt_wr : 16; /* [15..0]  */
        unsigned int cur_sel2_chn_miss_cnt_wr : 16;  /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_master_dbg7;

/* define the union U_SMMU_MASTER_DBG8 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int sel2_chn_wr : 8; /* [7..0]  */
        unsigned int sel1_chn_wr : 8; /* [15..8]  */
        unsigned int sel2_chn_rd : 8; /* [23..16]  */
        unsigned int sel1_chn_rd : 8; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} u_smmu_master_dbg8;

/* Define the struct CH_LHFIR_COEF_S */
typedef struct {
    u_ch_lhfir_coef0 u_ch_lhfir_coef0;
    u_ch_lhfir_coef1 u_ch_lhfir_coef1;
    u_ch_lhfir_coef2 u_ch_lhfir_coef2;
    unsigned int reserved_0;
} vicap_reg_ch_lhfir_coef;

/* Define the struct CH_CHFIR_COEF_S */
typedef struct {
    u_ch_chfir_coef0 u_ch_chfir_coef0;
    u_ch_chfir_coef1 u_ch_chfir_coef1;
    u_ch_chfir_coef2 u_ch_chfir_coef2;
    unsigned int reserved_0;
} vicap_reg_ch_chfir_coef;

/* clang-format off */
#define vicap_reg_offset_wk_mode(vicap_id)                (vicap_id * 0x4000 + 0x0)
#define vicap_reg_offset_mem_ctrl(vicap_id)               (vicap_id * 0x4000 + 0x4)
#define vicap_reg_offset_mem_ctrl1(vicap_id)              (vicap_id * 0x4000 + 0x8)
#define vicap_reg_offset_hdmi2vi_adapter(vicap_id)        (vicap_id * 0x4000 + 0xC)
#define vicap_reg_offset_axi_cfg(vicap_id)                (vicap_id * 0x4000 + 0x10)
#define vicap_reg_offset_mac_cfg(vicap_id)                (vicap_id * 0x4000 + 0x14)
#define vicap_reg_offset_prot_ctrl(vicap_id)              (vicap_id * 0x4000 + 0x18)
#define vicap_reg_offset_prot_cfg(vicap_id)               (vicap_id * 0x4000 + 0x1C)
#define vicap_reg_offset_pt_sel(vicap_id)                 (vicap_id * 0x4000 + 0x20)
#define vicap_reg_offset_mix_adapter_cfg(vicap_id)        (vicap_id * 0x4000 + 0x4C)
#define vicap_reg_offset_mix_420_cfg(vicap_id)            (vicap_id * 0x4000 + 0x60)
#define vicap_reg_offset_mix_420_size(vicap_id)           (vicap_id * 0x4000 + 0x64)
#define vicap_reg_offset_abm_sel(vicap_id)                (vicap_id * 0x4000 + 0x80)
#define vicap_reg_offset_vicap_int_tee(vicap_id)          (vicap_id * 0x4000 + 0xD0)
#define vicap_reg_offset_vicap_int_mask_tee(vicap_id)     (vicap_id * 0x4000 + 0xD8)
#define vicap_reg_offset_apb_timeout(vicap_id)            (vicap_id * 0x4000 + 0xE0)
#define vicap_reg_offset_vicap_int(vicap_id)              (vicap_id * 0x4000 + 0xF0)
#define vicap_reg_offset_vicap_int_mask(vicap_id)         (vicap_id * 0x4000 + 0xF8)
#define vicap_reg_offset_pt_intf_mod(vicap_id)            (vicap_id * 0x4000 + 0x100)
#define vicap_reg_offset_pt_offset0(vicap_id)             (vicap_id * 0x4000 + 0x110)
#define vicap_reg_offset_pt_offset1(vicap_id)             (vicap_id * 0x4000 + 0x114)
#define vicap_reg_offset_pt_offset2(vicap_id)             (vicap_id * 0x4000 + 0x118)
#define vicap_reg_offset_pt_unify_timing_cfg(vicap_id)    (vicap_id * 0x4000 + 0x130)
#define vicap_reg_offset_pt_gen_timing_cfg(vicap_id)      (vicap_id * 0x4000 + 0x134)
#define vicap_reg_offset_pt_unify_data_cfg(vicap_id)      (vicap_id * 0x4000 + 0x140)
#define vicap_reg_offset_pt_gen_data_cfg(vicap_id)        (vicap_id * 0x4000 + 0x144)
#define vicap_reg_offset_pt_gen_data_coef(vicap_id)       (vicap_id * 0x4000 + 0x148)
#define vicap_reg_offset_pt_gen_data_init(vicap_id)       (vicap_id * 0x4000 + 0x14C)
#define vicap_reg_offset_pt_yuv444_cfg(vicap_id)          (vicap_id * 0x4000 + 0x150)
#define vicap_reg_offset_pt_fstart_dly(vicap_id)          (vicap_id * 0x4000 + 0x160)
#define vicap_reg_offset_pt_hor_skip(vicap_id)            (vicap_id * 0x4000 + 0x170)
#define vicap_reg_offset_pt_intf_hfb(vicap_id)            (vicap_id * 0x4000 + 0x180)
#define vicap_reg_offset_pt_intf_hact(vicap_id)           (vicap_id * 0x4000 + 0x184)
#define vicap_reg_offset_pt_intf_hbb(vicap_id)            (vicap_id * 0x4000 + 0x188)
#define vicap_reg_offset_pt_intf_vfb(vicap_id)            (vicap_id * 0x4000 + 0x18C)
#define vicap_reg_offset_pt_intf_vact(vicap_id)           (vicap_id * 0x4000 + 0x190)
#define vicap_reg_offset_pt_intf_vbb(vicap_id)            (vicap_id * 0x4000 + 0x194)
#define vicap_reg_offset_pt_intf_vbfb(vicap_id)           (vicap_id * 0x4000 + 0x198)
#define vicap_reg_offset_pt_intf_vbact(vicap_id)          (vicap_id * 0x4000 + 0x19C)
#define vicap_reg_offset_pt_intf_vbbb(vicap_id)           (vicap_id * 0x4000 + 0x1A0)
#define vicap_reg_offset_pt_clr_err(vicap_id)             (vicap_id * 0x4000 + 0x1D0)
#define vicap_reg_offset_pt_err_cnt(vicap_id)             (vicap_id * 0x4000 + 0x1D4)
#define vicap_reg_offset_pt_err_size(vicap_id)            (vicap_id * 0x4000 + 0x1D8)
#define vicap_reg_offset_pt_status(vicap_id)              (vicap_id * 0x4000 + 0x1E0)
#define vicap_reg_offset_pt_bt656_status(vicap_id)        (vicap_id * 0x4000 + 0x1E4)
#define vicap_reg_offset_pt_size(vicap_id)                (vicap_id * 0x4000 + 0x1EC)
#define vicap_reg_offset_pt_int(vicap_id)                 (vicap_id * 0x4000 + 0x1F0)
#define vicap_reg_offset_pt_int_mask(vicap_id)            (vicap_id * 0x4000 + 0x1F8)
#define vicap_reg_offset_ch_ctrl(vicap_id)                (vicap_id * 0x4000 + 0x1000)
#define vicap_reg_offset_ch_reg_newer(vicap_id)           (vicap_id * 0x4000 + 0x1004)
#define vicap_reg_offset_ch_adapter_cfg(vicap_id)         (vicap_id * 0x4000 + 0x1010)
#define vicap_reg_offset_ch_line_num(vicap_id)            (vicap_id * 0x4000 + 0x1014)
#define vicap_reg_offset_ch_line_cnt_y(vicap_id)          (vicap_id * 0x4000 + 0x1018)
#define vicap_reg_offset_ch_line_cnt_c(vicap_id)          (vicap_id * 0x4000 + 0x101C)
#define vicap_reg_offset_ch_fstart_dly_cfg(vicap_id)      (vicap_id * 0x4000 + 0x1020)
#define vicap_reg_offset_ch_count(vicap_id)               (vicap_id * 0x4000 + 0x1030)
#define vicap_reg_offset_ch_dly0_cfg(vicap_id)            (vicap_id * 0x4000 + 0x1034)
#define vicap_reg_offset_ch_dly1_cfg(vicap_id)            (vicap_id * 0x4000 + 0x1038)
#define vicap_reg_offset_ch_dly2_cfg(vicap_id)            (vicap_id * 0x4000 + 0x103C)
#define vicap_reg_offset_ch_dolby_cfg(vicap_id)           (vicap_id * 0x4000 + 0x1040)
#define vicap_reg_offset_ch_dolby_pack_num(vicap_id)      (vicap_id * 0x4000 + 0x1044)
#define vicap_reg_offset_ch_dolby_pack_num_t(vicap_id)    (vicap_id * 0x4000 + 0x1048)
#define vicap_reg_offset_ch_dolby_faddr_h(vicap_id)       (vicap_id * 0x4000 + 0x1050)
#define vicap_reg_offset_ch_dolby_faddr_l(vicap_id)       (vicap_id * 0x4000 + 0x1054)
#define vicap_reg_offset_ch_420_proc_cfg(vicap_id)        (vicap_id * 0x4000 + 0x1060)
#define vicap_reg_offset_ch_wch_mmu(vicap_id)             (vicap_id * 0x4000 + 0x1070)
#define vicap_reg_offset_ch_wch_y_cfg(vicap_id)           (vicap_id * 0x4000 + 0x1080)
#define vicap_reg_offset_ch_wch_y_size(vicap_id)          (vicap_id * 0x4000 + 0x1084)
#define vicap_reg_offset_ch_wch_y_faddr_h(vicap_id)       (vicap_id * 0x4000 + 0x1090)
#define vicap_reg_offset_ch_wch_y_faddr_l(vicap_id)       (vicap_id * 0x4000 + 0x1094)
#define vicap_reg_offset_ch_wch_y_stride(vicap_id)        (vicap_id * 0x4000 + 0x1098)
#define vicap_reg_offset_ch_wch_c_cfg(vicap_id)           (vicap_id * 0x4000 + 0x10A0)
#define vicap_reg_offset_ch_wch_c_size(vicap_id)          (vicap_id * 0x4000 + 0x10A4)
#define vicap_reg_offset_ch_wch_c_faddr_h(vicap_id)       (vicap_id * 0x4000 + 0x10B0)
#define vicap_reg_offset_ch_wch_c_faddr_l(vicap_id)       (vicap_id * 0x4000 + 0x10B4)
#define vicap_reg_offset_ch_wch_c_stride(vicap_id)        (vicap_id * 0x4000 + 0x10B8)
#define vicap_reg_offset_ch_int(vicap_id)                 (vicap_id * 0x4000 + 0x10F0)
#define vicap_reg_offset_ch_int_mask(vicap_id)            (vicap_id * 0x4000 + 0x10F8)
#define vicap_reg_offset_ch_crop_cfg(vicap_id)            (vicap_id * 0x4000 + 0x1100)
#define vicap_reg_offset_ch_crop_win(vicap_id)            (vicap_id * 0x4000 + 0x1104)
#define vicap_reg_offset_ch_crop0_start(vicap_id)         (vicap_id * 0x4000 + 0x1110)
#define vicap_reg_offset_ch_crop0_size(vicap_id)          (vicap_id * 0x4000 + 0x1114)
#define vicap_reg_offset_ch_crop1_start(vicap_id)         (vicap_id * 0x4000 + 0x1120)
#define vicap_reg_offset_ch_crop1_size(vicap_id)          (vicap_id * 0x4000 + 0x1124)
#define vicap_reg_offset_ch_csc_cfg(vicap_id)             (vicap_id * 0x4000 + 0x1200)
#define vicap_reg_offset_ch_csc_coef0(vicap_id)           (vicap_id * 0x4000 + 0x1210)
#define vicap_reg_offset_ch_csc_coef1(vicap_id)           (vicap_id * 0x4000 + 0x1214)
#define vicap_reg_offset_ch_csc_coef2(vicap_id)           (vicap_id * 0x4000 + 0x1218)
#define vicap_reg_offset_ch_csc_coef3(vicap_id)           (vicap_id * 0x4000 + 0x121C)
#define vicap_reg_offset_ch_csc_coef4(vicap_id)           (vicap_id * 0x4000 + 0x1220)
#define vicap_reg_offset_ch_csc_in_dc0(vicap_id)          (vicap_id * 0x4000 + 0x1230)
#define vicap_reg_offset_ch_csc_in_dc1(vicap_id)          (vicap_id * 0x4000 + 0x1234)
#define vicap_reg_offset_ch_csc_out_dc0(vicap_id)         (vicap_id * 0x4000 + 0x1238)
#define vicap_reg_offset_ch_csc_out_dc1(vicap_id)         (vicap_id * 0x4000 + 0x123C)
#define vicap_reg_offset_ch_csc_ink_ctrl(vicap_id)        (vicap_id * 0x4000 + 0x1240)
#define vicap_reg_offset_ch_csc_ink_data(vicap_id)        (vicap_id * 0x4000 + 0x1250)
#define vicap_reg_offset_ch_skip_y_cfg(vicap_id)          (vicap_id * 0x4000 + 0x1300)
#define vicap_reg_offset_ch_skip_y_win(vicap_id)          (vicap_id * 0x4000 + 0x1304)
#define vicap_reg_offset_ch_skip_c_cfg(vicap_id)          (vicap_id * 0x4000 + 0x1310)
#define vicap_reg_offset_ch_skip_c_win(vicap_id)          (vicap_id * 0x4000 + 0x1314)
#define vicap_reg_offset_ch_skip_size(vicap_id)           (vicap_id * 0x4000 + 0x1320)
#define vicap_reg_offset_ch_coef_update(vicap_id)         (vicap_id * 0x4000 + 0x1400)
#define vicap_reg_offset_ch_coef_rsel(vicap_id)           (vicap_id * 0x4000 + 0x1404)
#define vicap_reg_offset_ch_lhfir_cfg(vicap_id)           (vicap_id * 0x4000 + 0x1410)
#define vicap_reg_offset_ch_chfir_cfg(vicap_id)           (vicap_id * 0x4000 + 0x1414)
#define vicap_reg_offset_ch_lhfir_offset(vicap_id)        (vicap_id * 0x4000 + 0x1418)
#define vicap_reg_offset_ch_chfir_offset(vicap_id)        (vicap_id * 0x4000 + 0x141C)
#define vicap_reg_offset_ch_lvfir_cfg(vicap_id)           (vicap_id * 0x4000 + 0x1420)
#define vicap_reg_offset_ch_cvfir_cfg(vicap_id)           (vicap_id * 0x4000 + 0x1424)
#define vicap_reg_offset_ch_lfir_in_size(vicap_id)        (vicap_id * 0x4000 + 0x1430)
#define vicap_reg_offset_ch_cfir_in_size(vicap_id)        (vicap_id * 0x4000 + 0x1434)
#define vicap_reg_offset_ch_lfir_out_size(vicap_id)       (vicap_id * 0x4000 + 0x1438)
#define vicap_reg_offset_ch_cfir_out_size(vicap_id)       (vicap_id * 0x4000 + 0x143C)
#define vicap_reg_offset_ch_in_format(vicap_id)           (vicap_id * 0x4000 + 0x1440)
#define vicap_reg_offset_ch_3d_split_cfg(vicap_id)        (vicap_id * 0x4000 + 0x1500)
#define vicap_reg_offset_ch_dither_cfg(vicap_id)          (vicap_id * 0x4000 + 0x1600)
#define vicap_reg_offset_ch_clip_y_cfg(vicap_id)          (vicap_id * 0x4000 + 0x1700)
#define vicap_reg_offset_ch_clip_c_cfg(vicap_id)          (vicap_id * 0x4000 + 0x1704)
#define vicap_reg_offset_ch_lhfir_coef0(vicap_id)         (vicap_id * 0x4000 + 0x1800)
#define vicap_reg_offset_ch_lhfir_coef1(vicap_id)         (vicap_id * 0x4000 + 0x1804)
#define vicap_reg_offset_ch_lhfir_coef2(vicap_id)         (vicap_id * 0x4000 + 0x1808)
#define vicap_reg_offset_ch_chfir_coef0(vicap_id)         (vicap_id * 0x4000 + 0x1A00)
#define vicap_reg_offset_ch_chfir_coef1(vicap_id)         (vicap_id * 0x4000 + 0x1A04)
#define vicap_reg_offset_ch_chfir_coef2(vicap_id)         (vicap_id * 0x4000 + 0x1A08)
#define vicap_reg_offset_ch_check_sum_cfg(vicap_id)       (vicap_id * 0x4000 + 0x1C00)
#define vicap_reg_offset_ch_check_sum_y_h(vicap_id)       (vicap_id * 0x4000 + 0x1C10)
#define vicap_reg_offset_ch_check_sum_y_l(vicap_id)       (vicap_id * 0x4000 + 0x1C14)
#define vicap_reg_offset_ch_check_sum_c_h(vicap_id)       (vicap_id * 0x4000 + 0x1C20)
#define vicap_reg_offset_ch_check_sum_c_l(vicap_id)       (vicap_id * 0x4000 + 0x1C24)
#define vicap_reg_offset_ch_check_sum_metadata(vicap_id)  (vicap_id * 0x4000 + 0x1C30)
#define vicap_reg_offset_ch_hdr_ctrl(vicap_id)            (vicap_id * 0x4000 + 0x1C40)
#define vicap_reg_offset_ch_hdr_status0(vicap_id)         (vicap_id * 0x4000 + 0x1C48)
#define vicap_reg_offset_ch_hdr_wch_addr_h(vicap_id)      (vicap_id * 0x4000 + 0x1C50)
#define vicap_reg_offset_ch_hdr_wch_addr_l(vicap_id)      (vicap_id * 0x4000 + 0x1C54)
#define vicap_reg_offset_ch_det_y_size(vicap_id)          (vicap_id * 0x4000 + 0x1E40)
#define vicap_reg_offset_ch_det_c_size(vicap_id)          (vicap_id * 0x4000 + 0x1E44)
#define vicap_reg_offset_zme_8k_0_hinfo(vicap_id)         (vicap_id * 0x4000 + 0x2000)

#define vicap_reg_offset_zme_8k_0_hsp(vicap_id)           (vicap_id * 0x4000 + 0x2004)
#define vicap_reg_offset_zme_8k_0_hloffset(vicap_id)      (vicap_id * 0x4000 + 0x2008)
#define vicap_reg_offset_zme_8k_0_hcoffset(vicap_id)      (vicap_id * 0x4000 + 0x200C)
#define vicap_reg_offset_zme_8k_0_hzone0delta(vicap_id)   (vicap_id * 0x4000 + 0x2010)
#define vicap_reg_offset_zme_8k_0_hzone2delta(vicap_id)   (vicap_id * 0x4000 + 0x2014)
#define vicap_reg_offset_zme_8k_0_hzoneend(vicap_id)      (vicap_id * 0x4000 + 0x2018)
#define vicap_reg_offset_zme_8k_0_hl_shootctrl(vicap_id)  (vicap_id * 0x4000 + 0x201C)
#define vicap_reg_offset_zme_8k_0_hc_shootctrl(vicap_id)  (vicap_id * 0x4000 + 0x2020)
#define vicap_reg_offset_zme_8k_0_hrcoef(vicap_id)        (vicap_id * 0x4000 + 0x2024)
#define vicap_reg_offset_zme_8k_0_hycoefad(vicap_id)      (vicap_id * 0x4000 + 0x2028)
#define vicap_reg_offset_zme_8k_0_hccoefad(vicap_id)      (vicap_id * 0x4000 + 0x202C)
#define vicap_reg_offset_zme_8k_0_ireso(vicap_id)         (vicap_id * 0x4000 + 0x2070)
#define vicap_reg_offset_zme_8k_1_hinfo(vicap_id)         (vicap_id * 0x4000 + 0x2100)

#define vicap_reg_offset_zme_8k_1_hsp(vicap_id)           (vicap_id * 0x4000 + 0x2104)
#define vicap_reg_offset_zme_8k_1_hloffset(vicap_id)      (vicap_id * 0x4000 + 0x2108)
#define vicap_reg_offset_zme_8k_1_hcoffset(vicap_id)      (vicap_id * 0x4000 + 0x210C)
#define vicap_reg_offset_zme_8k_1_hzone0delta(vicap_id)   (vicap_id * 0x4000 + 0x2110)
#define vicap_reg_offset_zme_8k_1_hzone2delta(vicap_id)   (vicap_id * 0x4000 + 0x2114)
#define vicap_reg_offset_zme_8k_1_hzoneend(vicap_id)      (vicap_id * 0x4000 + 0x2118)
#define vicap_reg_offset_zme_8k_1_hl_shootctrl(vicap_id)  (vicap_id * 0x4000 + 0x211C)
#define vicap_reg_offset_zme_8k_1_hc_shootctrl(vicap_id)  (vicap_id * 0x4000 + 0x2120)
#define vicap_reg_offset_zme_8k_1_hrcoef(vicap_id)        (vicap_id * 0x4000 + 0x2124)
#define vicap_reg_offset_zme_8k_1_hycoefad(vicap_id)      (vicap_id * 0x4000 + 0x2128)
#define vicap_reg_offset_zme_8k_1_hccoefad(vicap_id)      (vicap_id * 0x4000 + 0x212C)
#define vicap_reg_offset_zme_8k_1_ireso(vicap_id)         (vicap_id * 0x4000 + 0x2170)
#define vicap_reg_offset_zme_8k_2_hinfo(vicap_id)         (vicap_id * 0x4000 + 0x2200)

#define vicap_reg_offset_zme_8k_2_hsp(vicap_id)           (vicap_id * 0x4000 + 0x2204)
#define vicap_reg_offset_zme_8k_2_hloffset(vicap_id)      (vicap_id * 0x4000 + 0x2208)
#define vicap_reg_offset_zme_8k_2_hcoffset(vicap_id)      (vicap_id * 0x4000 + 0x220C)
#define vicap_reg_offset_zme_8k_2_hzone0delta(vicap_id)   (vicap_id * 0x4000 + 0x2210)
#define vicap_reg_offset_zme_8k_2_hzone2delta(vicap_id)   (vicap_id * 0x4000 + 0x2214)
#define vicap_reg_offset_zme_8k_2_hzoneend(vicap_id)      (vicap_id * 0x4000 + 0x2218)
#define vicap_reg_offset_zme_8k_2_hl_shootctrl(vicap_id)  (vicap_id * 0x4000 + 0x221C)
#define vicap_reg_offset_zme_8k_2_hc_shootctrl(vicap_id)  (vicap_id * 0x4000 + 0x2220)
#define vicap_reg_offset_zme_8k_2_hrcoef(vicap_id)        (vicap_id * 0x4000 + 0x2224)
#define vicap_reg_offset_zme_8k_2_hycoefad(vicap_id)      (vicap_id * 0x4000 + 0x2228)
#define vicap_reg_offset_zme_8k_2_hccoefad(vicap_id)      (vicap_id * 0x4000 + 0x222C)
#define vicap_reg_offset_zme_8k_2_ireso(vicap_id)         (vicap_id * 0x4000 + 0x2270)
#define vicap_reg_offset_zme_8k_3_hinfo(vicap_id)         (vicap_id * 0x4000 + 0x2300)

#define vicap_reg_offset_zme_8k_3_hsp(vicap_id)           (vicap_id * 0x4000 + 0x2304)
#define vicap_reg_offset_zme_8k_3_hloffset(vicap_id)      (vicap_id * 0x4000 + 0x2308)
#define vicap_reg_offset_zme_8k_3_hcoffset(vicap_id)      (vicap_id * 0x4000 + 0x230C)
#define vicap_reg_offset_zme_8k_3_hzone0delta(vicap_id)   (vicap_id * 0x4000 + 0x2310)
#define vicap_reg_offset_zme_8k_3_hzone2delta(vicap_id)   (vicap_id * 0x4000 + 0x2314)
#define vicap_reg_offset_zme_8k_3_hzoneend(vicap_id)      (vicap_id * 0x4000 + 0x2318)
#define vicap_reg_offset_zme_8k_3_hl_shootctrl(vicap_id)  (vicap_id * 0x4000 + 0x231C)
#define vicap_reg_offset_zme_8k_3_hc_shootctrl(vicap_id)  (vicap_id * 0x4000 + 0x2320)
#define vicap_reg_offset_zme_8k_3_hrcoef(vicap_id)        (vicap_id * 0x4000 + 0x2324)
#define vicap_reg_offset_zme_8k_3_hycoefad(vicap_id)      (vicap_id * 0x4000 + 0x2328)
#define vicap_reg_offset_zme_8k_3_hccoefad(vicap_id)      (vicap_id * 0x4000 + 0x232C)
#define vicap_reg_offset_zme_8k_3_ireso(vicap_id)         (vicap_id * 0x4000 + 0x2370)
#define vicap_reg_offset_zme_8k_4_hinfo(vicap_id)         (vicap_id * 0x4000 + 0x2400)

#define vicap_reg_offset_zme_8k_4_hsp(vicap_id)           (vicap_id * 0x4000 + 0x2404)
#define vicap_reg_offset_zme_8k_4_hloffset(vicap_id)      (vicap_id * 0x4000 + 0x2408)
#define vicap_reg_offset_zme_8k_4_hcoffset(vicap_id)      (vicap_id * 0x4000 + 0x240C)
#define vicap_reg_offset_zme_8k_4_hzone0delta(vicap_id)   (vicap_id * 0x4000 + 0x2410)
#define vicap_reg_offset_zme_8k_4_hzone2delta(vicap_id)   (vicap_id * 0x4000 + 0x2414)
#define vicap_reg_offset_zme_8k_4_hzoneend(vicap_id)      (vicap_id * 0x4000 + 0x2418)
#define vicap_reg_offset_zme_8k_4_hl_shootctrl(vicap_id)  (vicap_id * 0x4000 + 0x241C)
#define vicap_reg_offset_zme_8k_4_hc_shootctrl(vicap_id)  (vicap_id * 0x4000 + 0x2420)
#define vicap_reg_offset_zme_8k_4_hrcoef(vicap_id)        (vicap_id * 0x4000 + 0x2424)
#define vicap_reg_offset_zme_8k_4_hycoefad(vicap_id)      (vicap_id * 0x4000 + 0x2428)
#define vicap_reg_offset_zme_8k_4_hccoefad(vicap_id)      (vicap_id * 0x4000 + 0x242C)
#define vicap_reg_offset_zme_8k_4_ireso(vicap_id)         (vicap_id * 0x4000 + 0x2470)

#define vicap_reg_offset_smmu_scr(vicap_id)               (vicap_id * 0x800 + 0xF000)
#define vicap_reg_offset_smmu_ctrl(vicap_id)              (vicap_id * 0x800 + 0xF004)
#define vicap_reg_offset_smmu_lp_ctrl(vicap_id)           (vicap_id * 0x800 + 0xF008)
#define vicap_reg_offset_smmu_mem_speedctrl(vicap_id)     (vicap_id * 0x800 + 0xF00C)
#define vicap_reg_offset_smmu_intmask_s(vicap_id)         (vicap_id * 0x800 + 0xF010)
#define vicap_reg_offset_smmu_intraw_s(vicap_id)          (vicap_id * 0x800 + 0xF014)
#define vicap_reg_offset_smmu_intstat_s(vicap_id)         (vicap_id * 0x800 + 0xF018)
#define vicap_reg_offset_smmu_intclr_s(vicap_id)          (vicap_id * 0x800 + 0xF01C)
#define vicap_reg_offset_smmu_intmask_ns(vicap_id)        (vicap_id * 0x800 + 0xF020)
#define vicap_reg_offset_smmu_intraw_ns(vicap_id)         (vicap_id * 0x800 + 0xF024)
#define vicap_reg_offset_smmu_intstat_ns(vicap_id)        (vicap_id * 0x800 + 0xF028)
#define vicap_reg_offset_smmu_intclr_ns(vicap_id)         (vicap_id * 0x800 + 0xF02C)
#define vicap_reg_offset_smmu_scb_ttbr(vicap_id)          (vicap_id * 0x800 + 0xF100)
#define vicap_reg_offset_smmu_scb_ttbr_h(vicap_id)        (vicap_id * 0x800 + 0xF104)
#define vicap_reg_offset_smmu_stag_rd_ctrl(vicap_id)      (vicap_id * 0x800 + 0xF110)
#define vicap_reg_offset_smmu_stag_rd_access0(vicap_id)   (vicap_id * 0x800 + 0xF114)
#define vicap_reg_offset_smmu_stag_rd_access1(vicap_id)   (vicap_id * 0x800 + 0xF118)
#define vicap_reg_offset_smmu_stag_wr_ctrl(vicap_id)      (vicap_id * 0x800 + 0xF120)
#define vicap_reg_offset_smmu_stag_wr_access0(vicap_id)   (vicap_id * 0x800 + 0xF124)
#define vicap_reg_offset_smmu_stag_wr_access1(vicap_id)   (vicap_id * 0x800 + 0xF128)
#define vicap_reg_offset_smmu_err_rdaddr_s(vicap_id)      (vicap_id * 0x800 + 0xF130)
#define vicap_reg_offset_smmu_err_rdaddr_h_s(vicap_id)    (vicap_id * 0x800 + 0xF134)
#define vicap_reg_offset_smmu_err_wraddr_s(vicap_id)      (vicap_id * 0x800 + 0xF138)
#define vicap_reg_offset_smmu_err_wraddr_h_s(vicap_id)    (vicap_id * 0x800 + 0xF13C)
#define vicap_reg_offset_smmu_cb_ttbr(vicap_id)           (vicap_id * 0x800 + 0xF200)
#define vicap_reg_offset_smmu_cb_ttbr_h(vicap_id)         (vicap_id * 0x800 + 0xF204)
#define vicap_reg_offset_smmu_tag_rd_ctrl(vicap_id)       (vicap_id * 0x800 + 0xF210)
#define vicap_reg_offset_smmu_tag_rd_access0(vicap_id)    (vicap_id * 0x800 + 0xF214)
#define vicap_reg_offset_smmu_tag_rd_access1(vicap_id)    (vicap_id * 0x800 + 0xF218)
#define vicap_reg_offset_smmu_tag_wr_ctrl(vicap_id)       (vicap_id * 0x800 + 0xF220)
#define vicap_reg_offset_smmu_tag_wr_access0(vicap_id)    (vicap_id * 0x800 + 0xF224)
#define vicap_reg_offset_smmu_tag_wr_access1(vicap_id)    (vicap_id * 0x800 + 0xF228)
#define vicap_reg_offset_smmu_err_rdaddr_ns(vicap_id)     (vicap_id * 0x800 + 0xF230)
#define vicap_reg_offset_smmu_err_rdaddr_h_ns(vicap_id)   (vicap_id * 0x800 + 0xF234)
#define vicap_reg_offset_smmu_err_wraddr_ns(vicap_id)     (vicap_id * 0x800 + 0xF238)
#define vicap_reg_offset_smmu_err_wraddr_h_ns(vicap_id)   (vicap_id * 0x800 + 0xF23C)
#define vicap_reg_offset_smmu_fault_addr_ptw_s(vicap_id)  (vicap_id * 0x800 + 0xF300)
#define vicap_reg_offset_smmu_fault_id_ptw_s(vicap_id)    (vicap_id * 0x800 + 0xF304)
#define vicap_reg_offset_smmu_fault_addr_ptw_ns(vicap_id) (vicap_id * 0x800 + 0xF308)
#define vicap_reg_offset_smmu_fault_id_ptw_ns(vicap_id)   (vicap_id * 0x800 + 0xF30C)
#define vicap_reg_offset_smmu_fault_addr_wr_s(vicap_id)   (vicap_id * 0x800 + 0xF330)
#define vicap_reg_offset_smmu_fault_tlb_wr_s(vicap_id)    (vicap_id * 0x800 + 0xF334)
#define vicap_reg_offset_smmu_fault_id_wr_s(vicap_id)     (vicap_id * 0x800 + 0xF338)
#define vicap_reg_offset_smmu_fault_addr_wr_ns(vicap_id)  (vicap_id * 0x800 + 0xF340)
#define vicap_reg_offset_smmu_fault_tlb_wr_ns(vicap_id)   (vicap_id * 0x800 + 0xF344)
#define vicap_reg_offset_smmu_fault_id_wr_ns(vicap_id)    (vicap_id * 0x800 + 0xF348)
#define vicap_reg_offset_smmu_fault_addr_rd_s(vicap_id)   (vicap_id * 0x800 + 0xF350)
#define vicap_reg_offset_smmu_fault_tlb_rd_s(vicap_id)    (vicap_id * 0x800 + 0xF354)
#define vicap_reg_offset_smmu_fault_id_rd_s(vicap_id)     (vicap_id * 0x800 + 0xF358)
#define vicap_reg_offset_smmu_fault_addr_rd_ns(vicap_id)  (vicap_id * 0x800 + 0xF360)
#define vicap_reg_offset_smmu_fault_tlb_rd_ns(vicap_id)   (vicap_id * 0x800 + 0xF364)
#define vicap_reg_offset_smmu_fault_id_rd_ns(vicap_id)    (vicap_id * 0x800 + 0xF368)
#define vicap_reg_offset_smmu_match_addr_wr_s(vicap_id)   (vicap_id * 0x800 + 0xF370)
#define vicap_reg_offset_smmu_match_tlb_wr_s(vicap_id)    (vicap_id * 0x800 + 0xF374)
#define vicap_reg_offset_smmu_match_id_wr_s(vicap_id)     (vicap_id * 0x800 + 0xF378)
#define vicap_reg_offset_smmu_match_addr_wr_ns(vicap_id)  (vicap_id * 0x800 + 0xF380)
#define vicap_reg_offset_smmu_match__tlb_wr_ns(vicap_id)  (vicap_id * 0x800 + 0xF384)
#define vicap_reg_offset_smmu_match__id_wr_ns(vicap_id)   (vicap_id * 0x800 + 0xF388)
#define vicap_reg_offset_smmu_match_addr_rd_s(vicap_id)   (vicap_id * 0x800 + 0xF390)
#define vicap_reg_offset_smmu_match_tlb_rd_s(vicap_id)    (vicap_id * 0x800 + 0xF394)
#define vicap_reg_offset_smmu_match_id_rd_s(vicap_id)     (vicap_id * 0x800 + 0xF398)
#define vicap_reg_offset_smmu_match_addr_rd_ns(vicap_id)  (vicap_id * 0x800 + 0xF3A0)
#define vicap_reg_offset_smmu_match_tlb_rd_ns(vicap_id)   (vicap_id * 0x800 + 0xF3A4)
#define vicap_reg_offset_smmu_match_id_rd_ns(vicap_id)    (vicap_id * 0x800 + 0xF3A8)
#define vicap_reg_offset_smmu_pref_dbg0(vicap_id)         (vicap_id * 0x800 + 0xF3E0)
#define vicap_reg_offset_smmu_pref_dbg1(vicap_id)         (vicap_id * 0x800 + 0xF3E4)
#define vicap_reg_offset_smmu_pref_dbg2(vicap_id)         (vicap_id * 0x800 + 0xF3E8)
#define vicap_reg_offset_smmu_pref_dbg3(vicap_id)         (vicap_id * 0x800 + 0xF3EC)
#define vicap_reg_offset_smmu_tbu_dbg0(vicap_id)          (vicap_id * 0x800 + 0xF3F0)
#define vicap_reg_offset_smmu_tbu_dbg1(vicap_id)          (vicap_id * 0x800 + 0xF3F4)
#define vicap_reg_offset_smmu_tbu_dbg2(vicap_id)          (vicap_id * 0x800 + 0xF3F8)
#define vicap_reg_offset_smmu_tbu_dbg3(vicap_id)          (vicap_id * 0x800 + 0xF3FC)
#define vicap_reg_offset_smmu_master_dbg0(vicap_id)       (vicap_id * 0x800 + 0xF400)
#define vicap_reg_offset_smmu_master_dbg1(vicap_id)       (vicap_id * 0x800 + 0xF404)
#define vicap_reg_offset_smmu_master_dbg2(vicap_id)       (vicap_id * 0x800 + 0xF408)
#define vicap_reg_offset_smmu_master_dbg3(vicap_id)       (vicap_id * 0x800 + 0xF40C)
#define vicap_reg_offset_smmu_master_dbg4(vicap_id)       (vicap_id * 0x800 + 0xF410)
#define vicap_reg_offset_smmu_master_dbg5(vicap_id)       (vicap_id * 0x800 + 0xF414)
#define vicap_reg_offset_smmu_master_dbg6(vicap_id)       (vicap_id * 0x800 + 0xF418)
#define vicap_reg_offset_smmu_master_dbg7(vicap_id)       (vicap_id * 0x800 + 0xF41C)
#define vicap_reg_offset_smmu_master_dbg8(vicap_id)       (vicap_id * 0x800 + 0xF420)
/* clang-format on */
#endif /* __VICAP_REG_H__ */
