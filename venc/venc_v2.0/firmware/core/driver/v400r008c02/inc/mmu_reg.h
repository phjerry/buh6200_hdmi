/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */

#ifndef __C_UNION_DEFINE_MMU_H__
#define __C_UNION_DEFINE_MMU_H__

/* Define the union U_SMMU_SCR */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int glb_bypass            : 1   ; /* [0]  */
        unsigned int reserved_0            : 3   ; /* [3..1]  */
        unsigned int page_typ_s            : 4   ; /* [7..4]  */
        unsigned int reserved_1            : 23  ; /* [30..8]  */
        unsigned int src_lock              : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_scr;

/* Define the union u_vedu_smmu_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0            : 3   ; /* [2..0]  */
        unsigned int int_en                : 1   ; /* [3]  */
        unsigned int page_typ_ns           : 4   ; /* [7..4]  */
        unsigned int reserved_1            : 8   ; /* [15..8]  */
        unsigned int ptw_pf                : 4   ; /* [19..16]  */
        unsigned int reserved_2            : 12  ; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_ctrl;

/* Define the union u_vedu_smmu_lp_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int auto_clk_gt_en        : 1   ; /* [0]  */
        unsigned int com_clk_gt_en         : 1   ; /* [1]  */
        unsigned int mst_clk_gt_en         : 1   ; /* [2]  */
        unsigned int reserved_0            : 29  ; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_vedu_smmu_lp_ctrl;

/* Define the union u_vedu_smmu_mem_speedctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rfs_emaw              : 2   ; /* [1..0]  */
        unsigned int rfs_ema               : 3   ; /* [4..2]  */
        unsigned int rfs_ret1n             : 1   ; /* [5]  */
        unsigned int reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_vedu_smmu_mem_speedctrl;

/* Define the union u_vedu_smmu_intmask_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ints_tlbmiss_msk      : 1   ; /* [0]  */
        unsigned int ints_ptw_trans_msk    : 1   ; /* [1]  */
        unsigned int ints_tlbinvalid_rd_msk : 1   ; /* [2]  */
        unsigned int ints_tlbinvalid_wr_msk : 1   ; /* [3]  */
        unsigned int ints_tlbunmatch_rd_msk : 1   ; /* [4]  */
        unsigned int ints_tlbunmatch_wr_msk : 1   ; /* [5]  */
        unsigned int reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_vedu_smmu_intmask_s;

/* Define the union u_vedu_smmu_intraw_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ints_tlbmiss_raw      : 1   ; /* [0]  */
        unsigned int ints_ptw_trans_raw    : 1   ; /* [1]  */
        unsigned int ints_tlbinvalid_rd_raw : 1   ; /* [2]  */
        unsigned int ints_tlbinvalid_wr_raw : 1   ; /* [3]  */
        unsigned int ints_tlbunmatch_rd_raw : 1   ; /* [4]  */
        unsigned int ints_tlbunmatch_wr_raw : 1   ; /* [5]  */
        unsigned int reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_vedu_smmu_intraw_s;

/* Define the union u_vedu_smmu_intstat_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ints_tlbmiss_stat     : 1   ; /* [0]  */
        unsigned int ints_ptw_trans_stat   : 1   ; /* [1]  */
        unsigned int ints_tlbinvalid_rd_stat : 1   ; /* [2]  */
        unsigned int ints_tlbinvalid_wr_stat : 1   ; /* [3]  */
        unsigned int ints_tlbunmatch_rd_stat : 1   ; /* [4]  */
        unsigned int ints_tlbunmatch_wr_stat : 1   ; /* [5]  */
        unsigned int reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_vedu_smmu_intstat_s;

/* Define the union u_vedu_smmu_intclr_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ints_tlbmiss_clr      : 1   ; /* [0]  */
        unsigned int ints_ptw_trans_clr    : 1   ; /* [1]  */
        unsigned int ints_tlbinvalid_rd_clr : 1   ; /* [2]  */
        unsigned int ints_tlbinvalid_wr_clr : 1   ; /* [3]  */
        unsigned int ints_tlbunmatch_rd_clr : 1   ; /* [4]  */
        unsigned int ints_tlbunmatch_wr_clr : 1   ; /* [5]  */
        unsigned int reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_vedu_smmu_intclr_s;

/* Define the union u_vedu_smmu_intmask_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intns_tlbmiss_msk     : 1   ; /* [0]  */
        unsigned int intns_ptw_trans_msk   : 1   ; /* [1]  */
        unsigned int intns_tlbinvalid_rd_msk : 1   ; /* [2]  */
        unsigned int intns_tlbinvalid_wr_msk : 1   ; /* [3]  */
        unsigned int intns_tlbunmatch_rd_msk : 1   ; /* [4]  */
        unsigned int intns_tlbunmatch_wr_msk : 1   ; /* [5]  */
        unsigned int reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_vedu_smmu_intmask_ns;

/* Define the union u_vedu_smmu_intraw_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intns_tlbmiss_raw     : 1   ; /* [0]  */
        unsigned int intns_ptw_trans_raw   : 1   ; /* [1]  */
        unsigned int intns_tlbinvalid_rd_raw : 1   ; /* [2]  */
        unsigned int intns_tlbinvalid_wr_raw : 1   ; /* [3]  */
        unsigned int intns_tlbunmatch_rd_raw : 1   ; /* [4]  */
        unsigned int intns_tlbunmatch_wr_raw : 1   ; /* [5]  */
        unsigned int reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_vedu_smmu_intraw_ns;

/* Define the union u_vedu_smmu_intstat_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intns_tlbmiss_stat    : 1   ; /* [0]  */
        unsigned int intns_ptw_trans_stat  : 1   ; /* [1]  */
        unsigned int intns_tlbinvalid_rd_stat : 1   ; /* [2]  */
        unsigned int intns_tlbinvalid_wr_stat : 1   ; /* [3]  */
        unsigned int intns_tlbunmatch_rd_stat : 1   ; /* [4]  */
        unsigned int intns_tlbunmatch_wr_stat : 1   ; /* [5]  */
        unsigned int reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_vedu_smmu_intstat_ns;

/* Define the union u_vedu_smmu_intclr_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intns_tlbmiss_clr     : 1   ; /* [0]  */
        unsigned int intns_ptw_trans_clr   : 1   ; /* [1]  */
        unsigned int intns_tlbinvalid_rd_clr : 1   ; /* [2]  */
        unsigned int intns_tlbinvalid_wr_clr : 1   ; /* [3]  */
        unsigned int intns_tlbunmatch_rd_clr : 1   ; /* [4]  */
        unsigned int intns_tlbunmatch_wr_clr : 1   ; /* [5]  */
        unsigned int reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_vedu_smmu_intclr_ns;

/* Define the union u_vedu_smmu_reserved_11 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_11;

/* Define the union u_vedu_smmu_scb_ttbr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int scb_ttbr               : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_scb_ttbr;
/* Define the union u_vedu_smmu_scb_ttbr_h */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int scb_ttbr_h            : 4   ; /* [3..0]  */
        unsigned int reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_vedu_smmu_scb_ttbr_h;

/* Define the union u_vedu_smmu_reserved_12 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_12;
/* Define the union u_vedu_smmu_stag_rd_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int scmd_tag_rd_en        : 1   ; /* [0]  */
        unsigned int reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_stag_rd_ctrl;

/* Define the union u_vedu_smmu_stag_rd_access0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int scmd_rd_access0        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_stag_rd_access0;

/* Define the union u_vedu_smmu_stag_rd_access1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int scmd_rd_access1        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_stag_rd_access1;

/* Define the union u_vedu_smmu_reserved_13 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_13;

/* Define the union u_vedu_smmu_stag_wr_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int scmd_tag_wr_en        : 1   ; /* [0]  */
        unsigned int reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_stag_wr_ctrl;

/* Define the union u_vedu_smmu_stag_wr_access0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int scmd_wr_access0        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_stag_wr_access0;

/* Define the union u_vedu_smmu_stag_wr_access1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int scmd_wr_access1        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_stag_wr_access1;

/* Define the union u_vedu_smmu_reserved_14 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_14;

/* Define the union u_vedu_smmu_err_rdaddr_s */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int err_s_rd_addr          : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_err_rdaddr_s;

/* Define the union u_vedu_smmu_err_rdaddr_h_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int err_s_rd_addr_h       : 4   ; /* [3..0]  */
        unsigned int reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_err_rdaddr_h_s;

/* Define the union u_vedu_smmu_err_wraddr_s */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int err_s_wr_addr          : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_err_wraddr_s;

/* Define the union u_vedu_smmu_err_wraddr_h_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int err_s_wr_addr_h       : 4   ; /* [3..0]  */
        unsigned int reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_err_wraddr_h_s;

/* Define the union u_vedu_smmu_reserved_15 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_15;

/* Define the union u_vedu_smmu_cb_ttbr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int cb_ttbr                : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_cb_ttbr;

/* Define the union u_vedu_smmu_cb_ttbr_h */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cb_ttbr_h             : 4   ; /* [3..0]  */
        unsigned int reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_cb_ttbr_h;

/* Define the union u_vedu_smmu_reserved_16 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_16;

/* Define the union u_vedu_smmu_tag_rd_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmd_tag_rd_en         : 1   ; /* [0]  */
        unsigned int reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_tag_rd_ctrl;

/* Define the union u_vedu_smmu_tag_rd_access0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int cmd_rd_access0         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_tag_rd_access0;

/* Define the union u_vedu_smmu_tag_rd_access1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int cmd_rd_access1         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_tag_rd_access1;

/* Define the union u_vedu_smmu_reserved_17 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_17;

/* Define the union u_vedu_smmu_tag_wr_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmd_tag_wr_en         : 1   ; /* [0]  */
        unsigned int reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_vedu_smmu_tag_wr_ctrl;

/* Define the union u_vedu_smmu_tag_wr_access0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int cmd_wr_access0         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_tag_wr_access0;

/* Define the union u_vedu_smmu_tag_wr_access1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int cmd_wr_access1         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_tag_wr_access1;

/* Define the union u_vedu_smmu_reserved_18 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_18;

/* Define the union u_vedu_smmu_err_rdaddr_ns */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int err_ns_rd_addr         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_err_rdaddr_ns;

/* Define the union u_vedu_smmu_err_rdaddr_h_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int err_ns_rd_addr_h      : 4   ; /* [3..0]  */
        unsigned int reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_vedu_smmu_err_rdaddr_h_ns;

/* Define the union u_vedu_smmu_err_wraddr_ns */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int err_ns_wr_addr         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_err_wraddr_ns;

/* Define the union u_vedu_smmu_err_wraddr_h_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int err_ns_wr_addr_h      : 4   ; /* [3..0]  */
        unsigned int reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_err_wraddr_h_ns;

/* Define the union u_vedu_smmu_reserved_19 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_19;

/* Define the union u_vedu_smmu_fault_addr_ptw_s */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int fault_addr_ptw_s       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_addr_ptw_s;

/* Define the union u_vedu_smmu_fault_id_ptw_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_iid_ptw_s       : 16  ; /* [15..0]  */
        unsigned int fault_sid_ptw_s       : 2   ; /* [17..16]  */
        unsigned int reserved_0            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_id_ptw_s;

/* Define the union u_vedu_smmu_fault_addr_ptw_ns */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int fault_addr_ptw_ns      : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_addr_ptw_ns;

/* Define the union u_vedu_smmu_fault_id_ptw_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_iid_ptw_ns      : 16  ; /* [15..0]  */
        unsigned int fault_sid_ptw_ns      : 2   ; /* [17..16]  */
        unsigned int reserved_0            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_id_ptw_ns;

/* Define the union u_vedu_smmu_reserved_20 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_20;

/* Define the union u_vedu_smmu_fault_addr_wr_s */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int fault_addr_wr_s        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_addr_wr_s;

/* Define the union u_vedu_smmu_fault_tlb_wr_s */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int fault_tlb_wr_s         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_tlb_wr_s;

/* Define the union u_vedu_smmu_fault_id_wr_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_iid_wr_s        : 16  ; /* [15..0]  */
        unsigned int fault_sid_wr_s        : 2   ; /* [17..16]  */
        unsigned int reserved_0            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_id_wr_s;

/* Define the union u_vedu_smmu_reserved_21 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_21;

/* Define the union u_vedu_smmu_fault_addr_wr_ns */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int fault_addr_wr_ns       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_addr_wr_ns;

/* Define the union u_vedu_smmu_fault_tlb_wr_ns */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int fault_tlb_wr_ns        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_tlb_wr_ns;

/* Define the union u_vedu_smmu_fault_id_wr_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_iid_wr_ns       : 16  ; /* [15..0]  */
        unsigned int fault_sid_wr_ns       : 2   ; /* [17..16]  */
        unsigned int reserved_0            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_id_wr_ns;

/* Define the union u_vedu_smmu_reserved_22 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_22;

/* Define the union u_vedu_smmu_fault_addr_rd_s */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int fault_addr_rd_s        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_addr_rd_s;

/* Define the union u_vedu_smmu_fault_tlb_rd_s */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int fault_tlb_rd_s         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_tlb_rd_s;

/* Define the union u_vedu_smmu_fault_id_rd_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_iid_rd_s        : 16  ; /* [15..0]  */
        unsigned int fault_sid_rd_s        : 2   ; /* [17..16]  */
        unsigned int reserved_0            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_id_rd_s;

/* Define the union u_vedu_smmu_reserved_23 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_23;

/* Define the union u_vedu_smmu_fault_addr_rd_ns */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int fault_addr_rd_ns       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_addr_rd_ns;

/* Define the union u_vedu_smmu_fault_tlb_rd_ns */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int fault_tlb_rd_ns        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_tlb_rd_ns;

/* Define the union u_vedu_smmu_fault_id_rd_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_iid_rd_ns       : 16  ; /* [15..0]  */
        unsigned int fault_sid_rd_ns       : 2   ; /* [17..16]  */
        unsigned int reserved_0            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_fault_id_rd_ns;

/* Define the union u_vedu_smmu_reserved_24 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_24;

/* Define the union u_vedu_smmu_match_addr_wr_s */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int match_addr_wr_s        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_match_addr_wr_s;

/* Define the union u_vedu_smmu_match_tlb_wr_s */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int match_tlb_wr_s         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_match_tlb_wr_s;

/* Define the union u_vedu_smmu_match_id_wr_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int match_iid_wr_s        : 16  ; /* [15..0]  */
        unsigned int match_sid_wr_s        : 2   ; /* [17..16]  */
        unsigned int reserved_0            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_match_id_wr_s;

/* Define the union u_vedu_smmu_reserved_25 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_25;

/* Define the union u_vedu_smmu_match_addr_wr_ns */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int match_addr_wr_ns       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_match_addr_wr_ns;

/* Define the union u_vedu_smmu_match__tlb_wr_ns */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int match_tlb_wr_ns        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_match__tlb_wr_ns;

/* Define the union u_vedu_smmu_match__id_wr_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int match_iid_wr_ns       : 16  ; /* [15..0]  */
        unsigned int match_sid_wr_ns       : 2   ; /* [17..16]  */
        unsigned int reserved_0            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_match__id_wr_ns;

/* Define the union u_vedu_smmu_reserved_26 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_26;

/* Define the union u_vedu_smmu_match_addr_rd_s */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int match_addr_rd_s        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_match_addr_rd_s;

/* Define the union u_vedu_smmu_match_tlb_rd_s */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int match_tlb_rd_s         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_match_tlb_rd_s;

/* Define the union u_vedu_smmu_match_id_rd_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int match_iid_rd_s        : 16  ; /* [15..0] */
        unsigned int match_sid_rd_s        : 2   ; /* [17..16] */
        unsigned int reserved_0            : 14  ; /* [31..18] */
    } bits;

    /* Define an unsigned member */
    unsigned int    data;
} u_vedu_smmu_match_id_rd_s;

/* Define the union u_vedu_smmu_reserved_27 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_27;

/* Define the union u_vedu_smmu_match_addr_rd_ns */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int match_addr_rd_ns       : 32  ; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_match_addr_rd_ns;

/* Define the union u_vedu_smmu_match_tlb_rd_ns */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int match_tlb_rd_ns        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_match_tlb_rd_ns;

/* Define the union u_vedu_smmu_match_id_rd_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int match_iid_rd_ns       : 16  ; /* [15..0]  */
        unsigned int match_sid_rd_ns       : 2   ; /* [17..16]  */
        unsigned int reserved_0            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_match_id_rd_ns;

/* Define the union u_vedu_smmu_reserved_28 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_reserved_28;

/* Define the union u_vedu_smmu_pref_dbg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dbg_pref_osd          : 8   ; /* [7..0]  */
        unsigned int dbg_pref_idle         : 1   ; /* [8]  */
        unsigned int pref_dbg0             : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_pref_dbg0;

/* Define the union u_vedu_smmu_pref_dbg1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int pref_dbg1              : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_pref_dbg1;

/* Define the union u_vedu_smmu_pref_dbg2 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int pref_dbg2              : 32  ; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_pref_dbg2;

/* Define the union u_vedu_smmu_pref_dbg3 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int pref_dbg3              : 32  ; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_pref_dbg3;

/* Define the union u_vedu_smmu_tbu_dbg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int r_out_cnt             : 8   ; /* [7..0] */
        unsigned int r_in_cnt              : 8   ; /* [15..8] */
        unsigned int r_dummy_cnt           : 8   ; /* [23..16] */
        unsigned int tbu_dbg0              : 7   ; /* [30..24] */
        unsigned int tbu_r_idle            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_tbu_dbg0;

/* Define the union u_vedu_smmu_tbu_dbg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int w_out_cnt             : 8   ; /* [7..0]  */
        unsigned int w_in_cnt              : 8   ; /* [15..8]  */
        unsigned int w_dummy_cnt           : 8   ; /* [23..16]  */
        unsigned int tbu_dbg1              : 7   ; /* [30..24]  */
        unsigned int tbu_w_idle            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_tbu_dbg1;

/* Define the union u_vedu_smmu_tbu_dbg2 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int tbu_dbg2               : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_tbu_dbg2;

/* Define the union u_vedu_smmu_tbu_dbg3 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int tbu_dbg3               : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_tbu_dbg3;

/* Define the union u_vedu_smmu_master_dbg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int last_miss_cnt_rd      : 10  ; /* [9..0]  */
        unsigned int cur_miss_cnt_rd       : 10  ; /* [19..10]  */
        unsigned int vld_debug_rd          : 4   ; /* [23..20]  */
        unsigned int rdy_debug_rd          : 4   ; /* [27..24]  */
        unsigned int in_out_cmd_cnt_rd     : 4   ; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_master_dbg0;

/* Define the union u_vedu_smmu_master_dbg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int last_miss_cnt_wr      : 10  ; /* [9..0] */
        unsigned int cur_miss_cnt_wr       : 10  ; /* [19..10] */
        unsigned int vld_debug_wr          : 4   ; /* [23..20] */
        unsigned int rdy_debug_wr          : 4   ; /* [27..24] */
        unsigned int in_out_cmd_cnt_wr     : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_master_dbg1;

/* Define the union u_vedu_smmu_master_dbg2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int last_double_miss_cnt_rd : 8   ; /* [7..0] */
        unsigned int cur_double_miss_cnt_rd : 8   ; /* [15..8] */
        unsigned int last_double_upd_cnt_rd : 8   ; /* [23..16] */
        unsigned int cur_double_upd_cnt_rd : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_master_dbg2;

/* Define the union u_vedu_smmu_master_dbg3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int last_double_miss_cnt_wr : 8   ; /* [7..0] */
        unsigned int cur_double_miss_cnt_wr : 8   ; /* [15..8] */
        unsigned int last_double_upd_cnt_wr : 7   ; /* [22..16] */
        unsigned int cur_double_upd_cnt_wr : 7   ; /* [29..23] */
        unsigned int mst_fsm_cur           : 2   ; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_master_dbg3;

/* Define the union u_vedu_smmu_master_dbg4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int last_sel1_chn_miss_cnt_rd : 16  ; /* [15..0] */
        unsigned int cur_sel1_chn_miss_cnt_rd : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_master_dbg4;

/* Define the union u_vedu_smmu_master_dbg5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int last_sel2_chn_miss_cnt_rd : 16  ; /* [15..0] */
        unsigned int cur_sel2_chn_miss_cnt_rd : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_master_dbg5;

/* Define the union u_vedu_smmu_master_dbg6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int last_sel1_chn_miss_cnt_wr : 16  ; /* [15..0] */
        unsigned int cur_sel1_chn_miss_cnt_wr : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_master_dbg6;

/* Define the union u_vedu_smmu_master_dbg7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int last_sel2_chn_miss_cnt_wr : 16  ; /* [15..0] */
        unsigned int cur_sel2_chn_miss_cnt_wr : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_master_dbg7;

/* Define the union u_vedu_smmu_master_dbg8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sel2_chn_wr           : 8   ; /* [7..0]  */
        unsigned int sel1_chn_wr           : 8   ; /* [15..8]  */
        unsigned int sel2_chn_rd           : 8   ; /* [23..16]  */
        unsigned int sel1_chn_rd           : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_smmu_master_dbg8;

/* Define the union u_reserved11 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_reserved11;

/* ============================================================================== */
/* Define the global struct */
typedef struct {
    u_vedu_smmu_scr smmu_scr;                       /* 0x0 */
    u_vedu_smmu_ctrl smmu_ctrl;                     /* 0x4 */
    u_vedu_smmu_lp_ctrl smmu_lp_ctrl;               /* 0x8 */
    u_vedu_smmu_mem_speedctrl smmu_mem_speedctrl;   /* 0xc */
    u_vedu_smmu_intmask_s smmu_intmask_s;           /* 0x10 */
    u_vedu_smmu_intraw_s smmu_intraw_s;             /* 0x14 */
    u_vedu_smmu_intstat_s smmu_intstat_s;           /* 0x18 */
    u_vedu_smmu_intclr_s smmu_intclr_s;             /* 0x1c */
    u_vedu_smmu_intmask_ns smmu_intmask_ns;         /* 0x20 */
    u_vedu_smmu_intraw_ns smmu_intraw_ns;           /* 0x24 */
    u_vedu_smmu_intstat_ns smmu_intstat_ns;         /* 0x28 */
    u_vedu_smmu_intclr_ns smmu_intclr_ns;           /* 0x2c */
    u_vedu_smmu_reserved_11 smmu_reserved_11;       /* 0x30 */
    unsigned int reserved_0[51];                    /* 0x34~0xfc, 51: size of reserved_0 */
    u_vedu_smmu_scb_ttbr smmu_scb_ttbr;             /* 0x100 */
    u_vedu_smmu_scb_ttbr_h smmu_scb_ttbr_h;         /* 0x104 */
    u_vedu_smmu_reserved_12 smmu_reserved_12;       /* 0x108 */
    unsigned int reserved_1;                        /* 0x10c */
    u_vedu_smmu_stag_rd_ctrl smmu_stag_rd_ctrl;     /* 0x110 */
    u_vedu_smmu_stag_rd_access0 smmu_stag_rd_access0;   /* 0x114 */
    u_vedu_smmu_stag_rd_access1 smmu_stag_rd_access1;   /* 0x118 */
    u_vedu_smmu_reserved_13 smmu_reserved_13;           /* 0x11c */
    u_vedu_smmu_stag_wr_ctrl smmu_stag_wr_ctrl;         /* 0x120 */
    u_vedu_smmu_stag_wr_access0 smmu_stag_wr_access0;   /* 0x124 */
    u_vedu_smmu_stag_wr_access1 smmu_stag_wr_access1;   /* 0x128 */
    u_vedu_smmu_reserved_14 smmu_reserved_14;           /* 0x12c */
    u_vedu_smmu_err_rdaddr_s smmu_err_rdaddr_s;         /* 0x130 */
    u_vedu_smmu_err_rdaddr_h_s smmu_err_rdaddr_h_s;     /* 0x134 */
    u_vedu_smmu_err_wraddr_s smmu_err_wraddr_s;         /* 0x138 */
    u_vedu_smmu_err_wraddr_h_s smmu_err_wraddr_h_s;     /* 0x13c */
    u_vedu_smmu_reserved_15 smmu_reserved_15;           /* 0x140 */
    unsigned int reserved_2[47];                        /* 0x144~0x1fc, 47:size of reserved_2 */
    u_vedu_smmu_cb_ttbr smmu_cb_ttbr;                   /* 0x200 */
    u_vedu_smmu_cb_ttbr_h smmu_cb_ttbr_h;               /* 0x204 */
    u_vedu_smmu_reserved_16 smmu_reserved_16;           /* 0x208 */
    unsigned int reserved_3;                            /* 0x20c */
    u_vedu_smmu_tag_rd_ctrl smmu_tag_rd_ctrl;           /* 0x210 */
    u_vedu_smmu_tag_rd_access0 smmu_tag_rd_access0;     /* 0x214 */
    u_vedu_smmu_tag_rd_access1 smmu_tag_rd_access1;     /* 0x218 */
    u_vedu_smmu_reserved_17 smmu_reserved_17;           /* 0x21c */
    u_vedu_smmu_tag_wr_ctrl smmu_tag_wr_ctrl;           /* 0x220 */
    u_vedu_smmu_tag_wr_access0 smmu_tag_wr_access0;     /* 0x224 */
    u_vedu_smmu_tag_wr_access1 smmu_tag_wr_access1;     /* 0x228 */
    u_vedu_smmu_reserved_18 smmu_reserved_18;           /* 0x22c */
    u_vedu_smmu_err_rdaddr_ns smmu_err_rdaddr_ns;       /* 0x230 */
    u_vedu_smmu_err_rdaddr_h_ns smmu_err_rdaddr_h_ns;   /* 0x234 */
    u_vedu_smmu_err_wraddr_ns smmu_err_wraddr_ns;       /* 0x238 */
    u_vedu_smmu_err_wraddr_h_ns smmu_err_wraddr_h_ns;   /* 0x23c */
    u_vedu_smmu_reserved_19 smmu_reserved_19;           /* 0x240 */
    unsigned int reserved_4[47];                        /* 0x244~0x2fc, 47:size of reserved_4 */
    u_vedu_smmu_fault_addr_ptw_s smmu_fault_addr_ptw_s; /* 0x300 */
    u_vedu_smmu_fault_id_ptw_s smmu_fault_id_ptw_s;     /* 0x304 */
    u_vedu_smmu_fault_addr_ptw_ns smmu_fault_addr_ptw_ns;   /* 0x308 */
    u_vedu_smmu_fault_id_ptw_ns smmu_fault_id_ptw_ns;   /* 0x30c */
    u_vedu_smmu_reserved_20 smmu_reserved_20;           /* 0x310 */
    unsigned int reserved_5[7];                         /* 0x314~0x32c, 7: size of reserved_5 */
    u_vedu_smmu_fault_addr_wr_s smmu_fault_addr_wr_s;   /* 0x330 */
    u_vedu_smmu_fault_tlb_wr_s smmu_fault_tlb_wr_s;     /* 0x334 */
    u_vedu_smmu_fault_id_wr_s smmu_fault_id_wr_s;       /* 0x338 */
    u_vedu_smmu_reserved_21 smmu_reserved_21;           /* 0x33c */
    u_vedu_smmu_fault_addr_wr_ns smmu_fault_addr_wr_ns; /* 0x340 */
    u_vedu_smmu_fault_tlb_wr_ns smmu_fault_tlb_wr_ns;   /* 0x344 */
    u_vedu_smmu_fault_id_wr_ns smmu_fault_id_wr_ns;     /* 0x348 */
    u_vedu_smmu_reserved_22 smmu_reserved_22;           /* 0x34c */
    u_vedu_smmu_fault_addr_rd_s smmu_fault_addr_rd_s;   /* 0x350 */
    u_vedu_smmu_fault_tlb_rd_s smmu_fault_tlb_rd_s;     /* 0x354 */
    u_vedu_smmu_fault_id_rd_s smmu_fault_id_rd_s;       /* 0x358 */
    u_vedu_smmu_reserved_23 smmu_reserved_23;           /* 0x35c */
    u_vedu_smmu_fault_addr_rd_ns smmu_fault_addr_rd_ns; /* 0x360 */
    u_vedu_smmu_fault_tlb_rd_ns smmu_fault_tlb_rd_ns;   /* 0x364 */
    u_vedu_smmu_fault_id_rd_ns smmu_fault_id_rd_ns;     /* 0x368 */
    u_vedu_smmu_reserved_24 smmu_reserved_24;           /* 0x36c */
    u_vedu_smmu_match_addr_wr_s smmu_match_addr_wr_s;   /* 0x370 */
    u_vedu_smmu_match_tlb_wr_s smmu_match_tlb_wr_s;     /* 0x374 */
    u_vedu_smmu_match_id_wr_s smmu_match_id_wr_s;       /* 0x378 */
    u_vedu_smmu_reserved_25 smmu_reserved_25;           /* 0x37c */
    u_vedu_smmu_match_addr_wr_ns smmu_match_addr_wr_ns; /* 0x380 */
    u_vedu_smmu_match__tlb_wr_ns smmu_match__tlb_wr_ns; /* 0x384 */
    u_vedu_smmu_match__id_wr_ns smmu_match__id_wr_ns;   /* 0x388 */
    u_vedu_smmu_reserved_26 smmu_reserved_26;           /* 0x38c */
    u_vedu_smmu_match_addr_rd_s smmu_match_addr_rd_s;   /* 0x390 */
    u_vedu_smmu_match_tlb_rd_s smmu_match_tlb_rd_s;     /* 0x394 */
    u_vedu_smmu_match_id_rd_s smmu_match_id_rd_s;       /* 0x398 */
    u_vedu_smmu_reserved_27 smmu_reserved_27;           /* 0x39c */
    u_vedu_smmu_match_addr_rd_ns smmu_match_addr_rd_ns; /* 0x3a0 */
    u_vedu_smmu_match_tlb_rd_ns smmu_match_tlb_rd_ns;   /* 0x3a4 */
    u_vedu_smmu_match_id_rd_ns smmu_match_id_rd_ns;     /* 0x3a8 */
    u_vedu_smmu_reserved_28 smmu_reserved_28;           /* 0x3ac */
    unsigned int reserved_6[12];                /* 0x3b0~0x3dc, 12:size of reserved_6 */
    u_vedu_smmu_pref_dbg0 smmu_pref_dbg0;       /* 0x3e0 */
    u_vedu_smmu_pref_dbg1 smmu_pref_dbg1;       /* 0x3e4 */
    u_vedu_smmu_pref_dbg2 smmu_pref_dbg2;       /* 0x3e8 */
    u_vedu_smmu_pref_dbg3 smmu_pref_dbg3;       /* 0x3ec */
    u_vedu_smmu_tbu_dbg0 smmu_tbu_dbg0;         /* 0x3f0 */
    u_vedu_smmu_tbu_dbg1 smmu_tbu_dbg1;         /* 0x3f4 */
    u_vedu_smmu_tbu_dbg2 smmu_tbu_dbg2;         /* 0x3f8 */
    u_vedu_smmu_tbu_dbg3 smmu_tbu_dbg3;         /* 0x3fc */
    u_vedu_smmu_master_dbg0 smmu_master_dbg0;   /* 0x400 */
    u_vedu_smmu_master_dbg1 smmu_master_dbg1;   /* 0x404 */
    u_vedu_smmu_master_dbg2 smmu_master_dbg2;   /* 0x408 */
    u_vedu_smmu_master_dbg3 smmu_master_dbg3;   /* 0x40c */
    u_vedu_smmu_master_dbg4 smmu_master_dbg4;   /* 0x410 */
    u_vedu_smmu_master_dbg5 smmu_master_dbg5;   /* 0x414 */
    u_vedu_smmu_master_dbg6 smmu_master_dbg6;   /* 0x418 */
    u_vedu_smmu_master_dbg7 smmu_master_dbg7;   /* 0x41c */
    u_vedu_smmu_master_dbg8  smmu_master_dbg8;  /* 0x420 */
    u_reserved11 reserved11[759];               /* 0x424, 759: size of reserved11 */
} s_mmu_regs_type;

#endif /* __C_UNION_DEFINE_MMU_H__ */
