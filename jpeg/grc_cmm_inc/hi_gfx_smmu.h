/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: err msg
 * Author: data structure
 * Create: 2016-01-01
 */

#ifndef _HI_GFX_SMMU_H_
#define _HI_GFX_SMMU_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* **************************** SDK Version Macro Definition *********************************** */

/* ********************************add include here********************************************* */
#include "hi_type.h"
#include <linux/ion.h>
#include "hi_osal.h"

/* **************************** Macro Definition *********************************************** */
#ifdef GRAPHIC_COMM_PRINT
#undef GRAPHIC_COMM_PRINT
#endif

#if defined(HI_LOG_SUPPORT) && (0 == HI_LOG_SUPPORT)
#define GRAPHIC_COMM_PRINT(fmt, args...)
#else
#define GRAPHIC_COMM_PRINT osal_printk
#endif
/* ************************** Enum Definition ************************************************** */

/* ************************** Structure Definition ********************************************* */
#ifdef CONFIG_GFX_MMU_SUPPORT
/* Define the union u_smmu_scr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int glb_bypass : 1;  /* [0] */
        unsigned int reserved_0 : 3;  /* [3..1] */
        unsigned int page_typ_s : 4;  /* [7..4] */
        unsigned int reserved_1 : 23; /* [30..8] */
        unsigned int src_lock : 1;    /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_scr;

/* Define the union u_smmu_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 3;  /* [2..0] */
        unsigned int int_en : 1;      /* [3] */
        unsigned int page_typ_ns : 4; /* [7..4] */
        unsigned int reserved_1 : 8;  /* [15..8] */
        unsigned int ptw_pf : 4;      /* [19..16] */
        unsigned int reserved_2 : 12; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_ctrl;

/* Define the union u_smmu_lp_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int auto_clk_gt_en : 1; /* [0] */
        unsigned int com_clk_gt_en : 1;  /* [1] */
        unsigned int mst_clk_gt_en : 1;  /* [2] */
        unsigned int reserved_0 : 29;    /* [31..3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_lp_ctrl;

/* Define the union u_smmu_mem_speedctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rfs_emaw : 2;   /* [1..0] */
        unsigned int rfs_ema : 3;    /* [4..2] */
        unsigned int rfs_ret1n : 1;  /* [5] */
        unsigned int reserved_0 : 2; /* [7..6] */
        unsigned int rashd_rme : 1;  /* [8] */
        unsigned int rfshd_rme : 1;  /* [9] */
        unsigned int rfshs_rme : 1;  /* [10] */
        unsigned int rfsehd_rme : 1; /* [11] */
        unsigned int rashd_rm : 4;   /* [15..12] */
        unsigned int rfshd_rm : 4;   /* [19..16] */
        unsigned int rfshs_rm : 4;   /* [23..20] */
        unsigned int rfsehd_rm : 4;  /* [27..24] */
        unsigned int reserved_1 : 4; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_mem_speedctrl;

/* Define the union u_smmu_intmask_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ints_tlbmiss_msk : 1;       /* [0] */
        unsigned int ints_ptw_trans_msk : 1;     /* [1] */
        unsigned int ints_tlbinvalid_rd_msk : 1; /* [2] */
        unsigned int ints_tlbinvalid_wr_msk : 1; /* [3] */
        unsigned int ints_tlbunmatch_rd_msk : 1; /* [4] */
        unsigned int ints_tlbunmatch_wr_msk : 1; /* [5] */
        unsigned int reserved_0 : 26;            /* [31..6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_intmask_s;

/* Define the union u_smmu_intraw_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ints_tlbmiss_raw : 1;       /* [0] */
        unsigned int ints_ptw_trans_raw : 1;     /* [1] */
        unsigned int ints_tlbinvalid_rd_raw : 1; /* [2] */
        unsigned int ints_tlbinvalid_wr_raw : 1; /* [3] */
        unsigned int ints_tlbunmatch_rd_raw : 1; /* [4] */
        unsigned int ints_tlbunmatch_wr_raw : 1; /* [5] */
        unsigned int reserved_0 : 26;            /* [31..6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_intraw_s;

/* Define the union u_smmu_intstat_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ints_tlbmiss_stat : 1;       /* [0] */
        unsigned int ints_ptw_trans_stat : 1;     /* [1] */
        unsigned int ints_tlbinvalid_rd_stat : 1; /* [2] */
        unsigned int ints_tlbinvalid_wr_stat : 1; /* [3] */
        unsigned int ints_tlbunmatch_rd_stat : 1; /* [4] */
        unsigned int ints_tlbunmatch_wr_stat : 1; /* [5] */
        unsigned int reserved_0 : 26;             /* [31..6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_intstat_s;

/* Define the union u_smmu_intclr_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ints_tlbmiss_clr : 1;       /* [0] */
        unsigned int ints_ptw_trans_clr : 1;     /* [1] */
        unsigned int ints_tlbinvalid_rd_clr : 1; /* [2] */
        unsigned int ints_tlbinvalid_wr_clr : 1; /* [3] */
        unsigned int ints_tlbunmatch_rd_clr : 1; /* [4] */
        unsigned int ints_tlbunmatch_wr_clr : 1; /* [5] */
        unsigned int reserved_0 : 26;            /* [31..6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_intclr_s;

/* Define the union u_smmu_intmask_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intns_tlbmiss_msk : 1;       /* [0] */
        unsigned int intns_ptw_trans_msk : 1;     /* [1] */
        unsigned int intns_tlbinvalid_rd_msk : 1; /* [2] */
        unsigned int intns_tlbinvalid_wr_msk : 1; /* [3] */
        unsigned int intns_tlbunmatch_rd_msk : 1; /* [4] */
        unsigned int intns_tlbunmatch_wr_msk : 1; /* [5] */
        unsigned int reserved_0 : 26;             /* [31..6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_intmask_ns;

/* Define the union u_smmu_intraw_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intns_tlbmiss_raw : 1;       /* [0] */
        unsigned int intns_ptw_trans_raw : 1;     /* [1] */
        unsigned int intns_tlbinvalid_rd_raw : 1; /* [2] */
        unsigned int intns_tlbinvalid_wr_raw : 1; /* [3] */
        unsigned int intns_tlbunmatch_rd_raw : 1; /* [4] */
        unsigned int intns_tlbunmatch_wr_raw : 1; /* [5] */
        unsigned int reserved_0 : 26;             /* [31..6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_intraw_ns;

/* Define the union u_smmu_intstat_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intns_tlbmiss_stat : 1;       /* [0] */
        unsigned int intns_ptw_trans_stat : 1;     /* [1] */
        unsigned int intns_tlbinvalid_rd_stat : 1; /* [2] */
        unsigned int intns_tlbinvalid_wr_stat : 1; /* [3] */
        unsigned int intns_tlbunmatch_rd_stat : 1; /* [4] */
        unsigned int intns_tlbunmatch_wr_stat : 1; /* [5] */
        unsigned int reserved_0 : 26;              /* [31..6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_intstat_ns;

/* Define the union u_smmu_intclr_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intns_tlbmiss_clr : 1;       /* [0] */
        unsigned int intns_ptw_trans_clr : 1;     /* [1] */
        unsigned int intns_tlbinvalid_rd_clr : 1; /* [2] */
        unsigned int intns_tlbinvalid_wr_clr : 1; /* [3] */
        unsigned int intns_tlbunmatch_rd_clr : 1; /* [4] */
        unsigned int intns_tlbunmatch_wr_clr : 1; /* [5] */
        unsigned int reserved_0 : 26;             /* [31..6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_intclr_ns;

/* Define the union u_smmu_scb_ttbr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int scb_ttbr : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_scb_ttbr;
/* Define the union u_smmu_scb_ttbr_h */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int scb_ttbr_h : 4;  /* [3..0] */
        unsigned int reserved_0 : 28; /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_scb_ttbr_h;

/* Define the union u_smmu_stag_rd_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int scmd_tag_rd_en : 1; /* [0] */
        unsigned int reserved_0 : 31;    /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_stag_rd_ctrl;

/* Define the union u_smmu_stag_rd_access0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int scmd_rd_access0 : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_stag_rd_access0;
/* Define the union u_smmu_stag_rd_access1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int scmd_rd_access1 : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_stag_rd_access1;
/* Define the union u_smmu_stag_wr_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int scmd_tag_wr_en : 1; /* [0] */
        unsigned int reserved_0 : 31;    /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_stag_wr_ctrl;

/* Define the union u_smmu_stag_wr_access0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int scmd_wr_access0 : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_stag_wr_access0;
/* Define the union u_smmu_stag_wr_access1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int scmd_wr_access1 : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_stag_wr_access1;
/* Define the union u_smmu_err_rdaddr_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int err_s_rd_addr : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_err_rdaddr_s;
/* Define the union u_smmu_err_rdaddr_h_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int err_s_rd_addr_h : 4; /* [3..0] */
        unsigned int reserved_0 : 28;     /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_err_rdaddr_h_s;

/* Define the union u_smmu_err_wraddr_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int err_s_wr_addr : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_err_wraddr_s;
/* Define the union u_smmu_err_wraddr_h_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int err_s_wr_addr_h : 4; /* [3..0] */
        unsigned int reserved_0 : 28;     /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_err_wraddr_h_s;

/* Define the union u_smmu_cb_ttbr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cb_ttbr : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_cb_ttbr;
/* Define the union u_smmu_cb_ttbr_h */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cb_ttbr_h : 4;   /* [3..0] */
        unsigned int reserved_0 : 28; /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_cb_ttbr_h;

/* Define the union u_smmu_tag_rd_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmd_tag_rd_en : 1; /* [0] */
        unsigned int reserved_0 : 31;   /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_tag_rd_ctrl;

/* Define the union u_smmu_tag_rd_access0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmd_rd_access0 : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_tag_rd_access0;
/* Define the union u_smmu_tag_rd_access1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmd_rd_access1 : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_tag_rd_access1;
/* Define the union u_smmu_tag_wr_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmd_tag_wr_en : 1; /* [0] */
        unsigned int reserved_0 : 31;   /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_tag_wr_ctrl;

/* Define the union u_smmu_tag_wr_access0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmd_wr_access0 : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_tag_wr_access0;
/* Define the union u_smmu_tag_wr_access1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmd_wr_access1 : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_tag_wr_access1;
/* Define the union u_smmu_err_rdaddr_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int err_ns_rd_addr : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_err_rdaddr_ns;
/* Define the union u_smmu_err_rdaddr_h_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int err_ns_rd_addr_h : 4; /* [3..0] */
        unsigned int reserved_0 : 28;      /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_err_rdaddr_h_ns;

/* Define the union u_smmu_err_wraddr_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int err_ns_wr_addr : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_err_wraddr_ns;
/* Define the union u_smmu_err_wraddr_h_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int err_ns_wr_addr_h : 4; /* [3..0] */
        unsigned int reserved_0 : 28;      /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_err_wraddr_h_ns;

/* Define the union u_smmu_fault_addr_ptw_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_addr_ptw_s : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_addr_ptw_s;
/* Define the union u_smmu_fault_id_ptw_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_iid_ptw_s : 16; /* [15..0] */
        unsigned int fault_sid_ptw_s : 2;  /* [17..16] */
        unsigned int reserved_0 : 14;      /* [31..18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_id_ptw_s;

/* Define the union u_smmu_fault_addr_ptw_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_addr_ptw_ns : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_addr_ptw_ns;
/* Define the union u_smmu_fault_id_ptw_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_iid_ptw_ns : 16; /* [15..0] */
        unsigned int fault_sid_ptw_ns : 2;  /* [17..16] */
        unsigned int reserved_0 : 14;       /* [31..18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_id_ptw_ns;

/* Define the union u_smmu_fault_addr_wr_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_addr_wr_s : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_addr_wr_s;
/* Define the union u_smmu_fault_tlb_wr_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_tlb_wr_s : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_tlb_wr_s;
/* Define the union u_smmu_fault_id_wr_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_iid_wr_s : 16; /* [15..0] */
        unsigned int fault_sid_wr_s : 2;  /* [17..16] */
        unsigned int reserved_0 : 14;     /* [31..18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_id_wr_s;

/* Define the union u_smmu_fault_addr_wr_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_addr_wr_ns : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_addr_wr_ns;
/* Define the union u_smmu_fault_tlb_wr_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_tlb_wr_ns : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_tlb_wr_ns;
/* Define the union u_smmu_fault_id_wr_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_iid_wr_ns : 16; /* [15..0] */
        unsigned int fault_sid_wr_ns : 2;  /* [17..16] */
        unsigned int reserved_0 : 14;      /* [31..18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_id_wr_ns;

/* Define the union u_smmu_fault_addr_rd_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_addr_rd_s : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_addr_rd_s;
/* Define the union u_smmu_fault_tlb_rd_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_tlb_rd_s : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_tlb_rd_s;
/* Define the union u_smmu_fault_id_rd_s */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_iid_rd_s : 16; /* [15..0] */
        unsigned int fault_sid_rd_s : 2;  /* [17..16] */
        unsigned int reserved_0 : 14;     /* [31..18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_id_rd_s;

/* Define the union u_smmu_fault_addr_rd_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_addr_rd_ns : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_addr_rd_ns;
/* Define the union u_smmu_fault_tlb_rd_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_tlb_rd_ns : 32; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_tlb_rd_ns;
/* Define the union u_smmu_fault_id_rd_ns */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fault_iid_rd_ns : 16; /* [15..0] */
        unsigned int fault_sid_rd_ns : 2;  /* [17..16] */
        unsigned int reserved_0 : 14;      /* [31..18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_smmu_fault_id_rd_ns;

typedef struct {
    u_smmu_scr smmu_scr;                             /* 0xf000 */
    u_smmu_ctrl smmu_ctrl;                           /* 0xf004 */
    u_smmu_lp_ctrl smmu_lp_ctrl;                     /* 0xf008 */
    u_smmu_mem_speedctrl smmu_mem_speedctrl;         /* 0xf00c */
    u_smmu_intmask_s smmu_intmask_s;                 /* 0xf010 */
    u_smmu_intraw_s smmu_intraw_s;                   /* 0xf014 */
    u_smmu_intstat_s smmu_intstat_s;                 /* 0xf018 */
    u_smmu_intclr_s smmu_intclr_s;                   /* 0xf01c */
    u_smmu_intmask_ns smmu_intmask_ns;               /* 0xf020 */
    u_smmu_intraw_ns smmu_intraw_ns;                 /* 0xf024 */
    u_smmu_intstat_ns smmu_intstat_ns;               /* 0xf028 */
    u_smmu_intclr_ns smmu_intclr_ns;                 /* 0xf02c */
    unsigned int reserved_95[52];                    /* 52: 0xf030~0xf0fc */
    u_smmu_scb_ttbr smmu_scb_ttbr;                   /* 0xf100 */
    u_smmu_scb_ttbr_h smmu_scb_ttbr_h;               /* 0xf104 */
    unsigned int reserved_96[2];                     /* 2: 0xf108~0xf10c */
    u_smmu_stag_rd_ctrl smmu_stag_rd_ctrl;           /* 0xf110 */
    u_smmu_stag_rd_access0 smmu_stag_rd_access0;     /* 0xf114 */
    u_smmu_stag_rd_access1 smmu_stag_rd_access1;     /* 0xf118 */
    unsigned int reserved_97;                        /* 0xf11c */
    u_smmu_stag_wr_ctrl smmu_stag_wr_ctrl;           /* 0xf120 */
    u_smmu_stag_wr_access0 smmu_stag_wr_access0;     /* 0xf124 */
    u_smmu_stag_wr_access1 smmu_stag_wr_access1;     /* 0xf128 */
    unsigned int reserved_98;                        /* 0xf12c */
    u_smmu_err_rdaddr_s smmu_err_rdaddr_s;           /* 0xf130 */
    u_smmu_err_rdaddr_h_s smmu_err_rdaddr_h_s;       /* 0xf134 */
    u_smmu_err_wraddr_s smmu_err_wraddr_s;           /* 0xf138 */
    u_smmu_err_wraddr_h_s smmu_err_wraddr_h_s;       /* 0xf13c */
    unsigned int reserved_99[48];                    /* 48: 0xf140~0xf1fc */
    u_smmu_cb_ttbr smmu_cb_ttbr;                     /* 0xf200 */
    u_smmu_cb_ttbr_h smmu_cb_ttbr_h;                 /* 0xf204 */
    unsigned int reserved_100[2];                    /* 2: 0xf208~0xf20c */
    u_smmu_tag_rd_ctrl smmu_tag_rd_ctrl;             /* 0xf210 */
    u_smmu_tag_rd_access0 smmu_tag_rd_access0;       /* 0xf214 */
    u_smmu_tag_rd_access1 smmu_tag_rd_access1;       /* 0xf218 */
    unsigned int reserved_101;                       /* 0xf21c */
    u_smmu_tag_wr_ctrl smmu_tag_wr_ctrl;             /* 0xf220 */
    u_smmu_tag_wr_access0 smmu_tag_wr_access0;       /* 0xf224 */
    u_smmu_tag_wr_access1 smmu_tag_wr_access1;       /* 0xf228 */
    unsigned int reserved_102;                       /* 0xf22c */
    u_smmu_err_rdaddr_ns smmu_err_rdaddr_ns;         /* 0xf230 */
    u_smmu_err_rdaddr_h_ns smmu_err_rdaddr_h_ns;     /* 0xf234 */
    u_smmu_err_wraddr_ns smmu_err_wraddr_ns;         /* 0xf238 */
    u_smmu_err_wraddr_h_ns smmu_err_wraddr_h_ns;     /* 0xf23c */
    unsigned int reserved_103[48];                   /* 48: 0xf240~0xf2fc */
    u_smmu_fault_addr_ptw_s smmu_fault_addr_ptw_s;   /* 0xf300 */
    u_smmu_fault_id_ptw_s smmu_fault_id_ptw_s;       /* 0xf304 */
    u_smmu_fault_addr_ptw_ns smmu_fault_addr_ptw_ns; /* 0xf308 */
    u_smmu_fault_id_ptw_ns smmu_fault_id_ptw_ns;     /* 0xf30c */
    unsigned int reserved_104[8];                    /* 8: 0xf310~0xf32c */
    u_smmu_fault_addr_wr_s smmu_fault_addr_wr_s;     /* 0xf330 */
    u_smmu_fault_tlb_wr_s smmu_fault_tlb_wr_s;       /* 0xf334 */
    u_smmu_fault_id_wr_s smmu_fault_id_wr_s;         /* 0xf338 */
    unsigned int reserved_105;                       /* 0xf33c */
    u_smmu_fault_addr_wr_ns smmu_fault_addr_wr_ns;   /* 0xf340 */
    u_smmu_fault_tlb_wr_ns smmu_fault_tlb_wr_ns;     /* 0xf344 */
    u_smmu_fault_id_wr_ns smmu_fault_id_wr_ns;       /* 0xf348 */
    unsigned int reserved_106;                       /* 0xf34c */
    u_smmu_fault_addr_rd_s smmu_fault_addr_rd_s;     /* 0xf350 */
    u_smmu_fault_tlb_rd_s smmu_fault_tlb_rd_s;       /* 0xf354 */
    u_smmu_fault_id_rd_s smmu_fault_id_rd_s;         /* 0xf358 */
    unsigned int reserved_107;                       /* 0xf35c */
    u_smmu_fault_addr_rd_ns smmu_fault_addr_rd_ns;   /* 0xf360 */
    u_smmu_fault_tlb_rd_ns smmu_fault_tlb_rd_ns;     /* 0xf364 */
    u_smmu_fault_id_rd_ns smmu_fault_id_rd_ns;       /* 0xf368 */

} drv_gfx_mmu_reg;

/* ********************* Global Variable declaration ******************************************* */
static volatile drv_gfx_mmu_reg *g_mmu_reg = HI_NULL;

#endif

/* ****************************** API declaration ********************************************** */

/***************************************************************************
* func          : HI_GFX_MapSmmu
* description   : map smmu register
                  CNcomment: 映射SMMU寄存器 CNend\n
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
****************************************************************************/
static inline hi_s32 HI_GFX_MapSmmuReg(hi_u32 u32SmmuAddr)
{
#ifdef CONFIG_GFX_MMU_SUPPORT
    if ((u32SmmuAddr != 0) && (g_mmu_reg == HI_NULL)) {
        g_mmu_reg = (volatile drv_gfx_mmu_reg *)osal_ioremap_nocache(u32SmmuAddr, sizeof(drv_gfx_mmu_reg));
    }

    if (g_mmu_reg == HI_NULL) {
        GRAPHIC_COMM_PRINT("[module-gfx][err] : %s %d failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
#else
#endif
    return HI_SUCCESS;
}

/***************************************************************************
* func          : HI_GFX_UnMapSmmu
* description   : un map smmu register
                  CNcomment: 去映射SMMU寄存器 CNend\n
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
****************************************************************************/
static inline hi_void HI_GFX_UnMapSmmuReg(hi_void)
{
#ifdef CONFIG_GFX_MMU_SUPPORT
    if (g_mmu_reg != HI_NULL) {
        osal_iounmap((hi_void *)g_mmu_reg);
    }
    g_mmu_reg = HI_NULL;
#endif
    return;
}

/***************************************************************************
* func          : HI_GFX_InitSmmu
* description   : init smmu, open smmu while work
                  CNcomment: 在工作的时候才打开smmu，低功耗策略 CNend\n
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
****************************************************************************/
static inline hi_s32 HI_GFX_InitSmmu(hi_u32 u32SmmuAddr)
{
#ifdef CONFIG_GFX_MMU_SUPPORT
    hi_ulong cb_ttbr = 0;
    hi_ulong err_rd_addr = 0;
    hi_ulong err_wr_addr = 0;

    if (g_mmu_reg == HI_NULL) {
        GRAPHIC_COMM_PRINT("[module-gfx][err] : %s %d failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    g_mmu_reg->smmu_scr.bits.glb_bypass = 0x0;
    g_mmu_reg->smmu_ctrl.bits.int_en = 0x1;

    osal_mem_get_nssmmu_pgtinfo(&cb_ttbr, &err_rd_addr, &err_wr_addr);

    g_mmu_reg->smmu_cb_ttbr.bits.cb_ttbr = (hi_u32)(cb_ttbr & 0xFFFFFFFF);
    /* 0xFFFFFFFF00000000 and right shift 32 bits to get high addr */
    g_mmu_reg->smmu_cb_ttbr_h.bits.cb_ttbr_h = (hi_u32)((cb_ttbr & 0xFFFFFFFF00000000) >> 32);
    g_mmu_reg->smmu_err_rdaddr_ns.bits.err_ns_rd_addr = (hi_u32)(err_rd_addr & 0xFFFFFFFF);
    /* 0xFFFFFFFF00000000 and right shift 32 bits to get high addr */
    g_mmu_reg->smmu_err_rdaddr_h_ns.bits.err_ns_rd_addr_h = (hi_u32)((err_rd_addr & 0xFFFFFFFF00000000) >> 32);
    g_mmu_reg->smmu_err_wraddr_ns.bits.err_ns_wr_addr = (hi_u32)(err_wr_addr & 0xFFFFFFFF);
    /* 0xFFFFFFFF00000000 and right shift 32 bits to get high addr */
    g_mmu_reg->smmu_err_wraddr_h_ns.bits.err_ns_wr_addr_h = (hi_u32)((err_wr_addr & 0xFFFFFFFF00000000) >> 32);

    if (g_mmu_reg->smmu_cb_ttbr.bits.cb_ttbr == 0) {
        GRAPHIC_COMM_PRINT("[module-gfx][err] : %s %d %lu failure\n", __FUNCTION__, __LINE__, cb_ttbr);
        return HI_FAILURE;
    }

    if (g_mmu_reg->smmu_err_rdaddr_ns.bits.err_ns_rd_addr == 0) {
        GRAPHIC_COMM_PRINT("[module-gfx][err] : %s %d %lu failure\n", __FUNCTION__, __LINE__, err_rd_addr);
        return HI_FAILURE;
    }

    if (g_mmu_reg->smmu_err_wraddr_ns.bits.err_ns_wr_addr == 0) {
        GRAPHIC_COMM_PRINT("[module-gfx][err] : %s %d %lu failure\n", __FUNCTION__, __LINE__, err_rd_addr);
        return HI_FAILURE;
    }
#else
#endif
    return HI_SUCCESS;
}

/***************************************************************************
* func          : HI_GFX_DeinitSmmu
* description   : dinit smmu, close smmu while not work
                  CNcomment: 不工作的时候关闭smmu，低功耗策略 CNend\n
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
****************************************************************************/
static inline hi_void HI_GFX_DeinitSmmu(hi_void)
{
#ifdef CONFIG_GFX_MMU_SUPPORT
    if (g_mmu_reg != HI_NULL) {
        /**< task maybe not finish, so should not close smmu **/
        /* * g_mmu_reg->uScr.bits.glb_bypass = 0x1; * */
        /* * g_mmu_reg->uScr.bits.int_en     = 0x0; * */
    }
#endif
    return;
}

/***************************************************************************
* func          : HI_GFX_SmmuIsr
* description   : irq of smmu
                  CNcomment: smmu中断处理
                  0x10 ~ 0x1c 安全中断处理，对应着
                  0x20 ~ 0x2c 非安全中断处理 CNend\n
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
****************************************************************************/
static inline hi_s32 HI_GFX_SmmuIsr(hi_char *pErrType)
{
#ifdef CONFIG_GFX_MMU_SUPPORT
    if (g_mmu_reg == HI_NULL) {
        GRAPHIC_COMM_PRINT("[module-gfx][err] : %s %d failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    if (g_mmu_reg->smmu_intstat_ns.u32 == 0) {
        return HI_SUCCESS;
    }

    GRAPHIC_COMM_PRINT("MODULE : %s unsafe int stat value : 0x%x\n", pErrType, g_mmu_reg->smmu_intstat_ns.u32);

    if (g_mmu_reg->smmu_intstat_ns.bits.intns_tlbinvalid_wr_stat != 0) {
        GRAPHIC_COMM_PRINT("SMMU_WRITE_ERR MODULE : %s  unsafe write_err_addr : 0x%x\n", pErrType,
                           g_mmu_reg->smmu_fault_addr_wr_ns.bits.fault_addr_wr_ns);
    }

    if (g_mmu_reg->smmu_intstat_ns.bits.intns_tlbinvalid_rd_stat != 0) {
        GRAPHIC_COMM_PRINT("SMMU_READ_ERR  MODULE : %s  unsafe read_err_addr : 0x%x\n", pErrType,
                           g_mmu_reg->smmu_fault_addr_rd_ns.bits.fault_addr_rd_ns);
    }

    g_mmu_reg->smmu_intclr_ns.u32 |= 0xf;

    return HI_FAILURE;
#else
    return HI_SUCCESS;
#endif
}
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* *_HI_GFX_SMMU_H_         * */
