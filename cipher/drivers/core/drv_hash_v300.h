/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_hash_v300
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#ifndef _DRV_HASH_V3_H_
#define _DRV_HASH_V3_H_

#include "drv_osal_lib.h"

/*************************** Internal Structure Definition ****************************/
/** \addtogroup      hash drivers */
/** @{ */ /** <!-- [hash] */

/* ! \Define the offset of reg */

#define SPACC_IS                          (0x0000)
#define SPACC_IE                          (0x0004)
#define SPACC_INT_RAW_HASH_CLEAR_FINISH   (0x0014)
#define SPACC_HASH_CHN_LOCK               (0x0040)
#define SPACC_HASH_CHN_CLEAR_REQ          (0x0068)
#define SPACC_MMU_SEC_TYPE                (0x2000)
#define SPACC_MMU_SEC_TLB                 (0x2004)
#define SPACC_MMU_NOSEC_TYPE              (0x2010)
#define SPACC_MMU_NOSEC_TLB               (0x2014)
#define SPACC_MMU_IN_HASH_SEC_EADDR_H     (0x2200)
#define SPACC_MMU_IN_HASH_SEC_EADDR_L     (0x2204)
#define SPACC_MMU_IN_HASH_NOSEC_EADDR_H   (0x2210)
#define SPACC_MMU_IN_HASH_NOSEC_EADDR_L   (0x2214)
#define SPACC_MMU_IN_HASH_TAG_ID_H        (0x2220)
#define SPACC_MMU_IN_HASH_TAG_ID_L        (0x2224)
#define SPACC_MMU_IN_HASH_CLEAR           (0x2230)
#define SPACC_MMU_IN_HASH_SEC_UNVLD_VA_TAB   (0x2240)
#define SPACC_MMU_IN_HASH_SEC_UNVLD_PA_TAB   (0x2244)
#define SPACC_MMU_IN_HASH_NOSEC_UNVLD_VA_TAB (0x2248)
#define SPACC_MMU_IN_HASH_NOSEC_UNVLD_PA_TAB (0x224C)
#define SPACC_AXI_IRAW                    (0x3010)
#define IN_HASH_GLOBAL_CTRL               (0x6004)
#define ISTA_IN_SYM_NODE_CNT              (0x6518)
#define IRAW_IN_HASH_NODE_CNT             (0x6A10)
#define IENA_IN_HASH_NODE_CNT             (0x6A14)
#define ISTA_IN_HASH_NODE_CNT             (0x6A18)
#define IN_HASH_CHN_CTRL(id)              (0x5000 + (id)*0x100)
#define IN_HASH_CHN_KEY_CTRL(id)          (0x5010 + (id)*0x100)
#define IN_HASH_CHN_NODE_START_ADDR_H(id) (0x5020 + (id)*0x100)
#define IN_HASH_CHN_NODE_START_ADDR_L(id) (0x5024 + (id)*0x100)
#define IN_HASH_CHN_NODE_LENGTH(id)       (0x502c + (id)*0x100)
#define IN_HASH_CHN_NODE_WR_POINT(id)     (0x5030 + (id)*0x100)
#define IN_HASH_CHN_NODE_RD_POINT(id)     (0x5034 + (id)*0x100)
#define IN_HASH_CHN_NODE_CTRL(id)         (0x5040 + (id)*0x100)
#define DBG_IN_HASH_CHN_RD_ADDR_L(id)     (0x50a4 + (id)*0x100)
#define DBG_IN_HASH_CHN_DATA_LEN(id)      (0x50c0 + (id)*0x100)
#define SPACC_CALC_CRG_CFG                (0x8030)
#define HASH_CHANN_RAW_INT                (0x8600)
#define HASH_CHANN_RAW_INT_EN             (0x8604)
#define HASH_CHANN_INT                    (0x8608)
#define OUT_SYM_CHAN_LAST_NODE_INT        (0xc008)

#define HPP_HASH_CALC_CTRL_CHECK_ERR        (0x8058)
#define HPP_HASH_CALC_CTRL_CHECK_ERR_STATUS (0x805c)
#define TEE_HASH_CALC_CTRL_CHECK_ERR        (0x8068)
#define TEE_HASH_CALC_CTRL_CHECK_ERR_STATUS (0x806c)
#define REE_HASH_CALC_CTRL_CHECK_ERR        (0x8078)
#define REE_HASH_CALC_CTRL_CHECK_ERR_STATUS (0x807c)

#define CHANn_HASH_STATE_VAL(id)      (0xa000 + (id)*0x80)
#define CHANn_HASH_STATE_VAL_ADDR(id) (0xa004 + (id)*0x80)

#define HASH_CALC_CTRL_CHECK_ERR(cpu)        (0x8000 + 0x88 - (cpu)*0x10)
#define HASH_CALC_CTRL_CHECK_ERR_STATUS(cpu) (0x8000 + 0x8c - (cpu)*0x10)

#define HASH_LOW_POWER_ENABLE                 0x01

#define SPACC_CPU_HPP  (0x03)
#define SPACC_CPU_TEE  (0x02)
#define SPACC_CPU_REE  (0x01)
#define SPACC_CPU_IDLE (0x00)

#define CHN_WHO_USED_CLR(used, chn)      (used) &= ~(0x03U << ((chn)*2U))
#define CHN_WHO_USED_SET(used, chn, who) (used) |= (who) << ((chn)*2U)
#define CHN_WHO_USED_GET(used, chn)      ((((used) >> ((chn)*2U))) & 0x03U)

/* Define the union spacc_is : hpp | tee | ree */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int int_in_ctrl_ree : 1;      /* [0] */
        unsigned int int_in_ctrl_tee : 1;      /* [1] */
        unsigned int int_in_ctrl_hpp : 1;      /* [2] */
        unsigned int int_cal_ctrl_ree : 1;     /* [3] */
        unsigned int int_cal_ctrl_tee : 1;     /* [4] */
        unsigned int int_cal_ctrl_hpp : 1;     /* [5] */
        unsigned int int_out_ctrl_ree : 1;     /* [6] */
        unsigned int int_out_ctrl_tee : 1;     /* [7] */
        unsigned int int_out_ctrl_hpp : 1;     /* [8] */
        unsigned int int_mmu_ree : 1;          /* [9] */
        unsigned int int_mmu_tee : 1;          /* [10] */
        unsigned int int_mmu_hpp : 1;          /* [11] */
        unsigned int int_clear_finish_ree : 1; /* [12] */
        unsigned int int_clear_finish_tee : 1; /* [13] */
        unsigned int int_clear_finish_hpp : 1; /* [14] */
        unsigned int reserved : 17;            /* [31..15] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} spacc_is;

/* Define the union ista_in_sym_node_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        u32 ista_in_sym_node_cnt : 16; /* [15..1] */
        u32 reserved_1 : 16;           /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} ista_in_sym_node_cnt;

/* Define the union out_sym_chan_int */
typedef union {
    /* Define the struct bits */
    struct {
        u32 out_sym_chan_int : 16; /* [15..1] */
        u32 reserved_1 : 16;       /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} out_sym_chan_int;

/* Define the union iraw_in_hash_node_null */
typedef union {
    /* Define the struct bits */
    struct {
        u32 iraw_in_hash_node_null : 16; /* [15..1] */
        u32 reserved_1 : 16;             /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} iraw_in_hash_node_null;

/* Define the union iena_in_hash_node_null */
typedef union {
    /* Define the struct bits */
    struct {
        u32 iena_in_hash_node_null : 16; /* [15..1] */
        u32 reserved_1 : 16;             /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} iena_in_hash_node_null;

/* Define the union ista_in_hash_node_null */
typedef union {
    /* Define the struct bits */
    struct {
        u32 ista_in_hash_node_null : 16; /* [15..1] */
        u32 reserved_1 : 16;             /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} ista_in_hash_node_null;

/* Define the union iraw_in_hash_node_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        u32 iraw_in_hash_node_cnt : 16; /* [15..1] */
        u32 reserved_1 : 16;            /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} iraw_in_hash_node_cnt;

/* Define the union iena_in_hash_node_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        u32 iena_in_hash_node_cnt : 16; /* [15..1] */
        u32 reserved_1 : 16;            /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} iena_in_hash_node_cnt;

/* Define the union ista_in_hash_node_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        u32 ista_in_hash_node_cnt : 16; /* [15..1] */
        u32 reserved_1 : 16;            /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} ista_in_hash_node_cnt;

/* Define the union hash_chann_raw_int */
typedef union {
    /* Define the struct bits */
    struct {
        u32 hash_chann_raw_int : 16; /* [15..0] */
        u32 reserved_0 : 16;         /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} hash_chann_raw_int;

/* Define the union hash_chann_raw_int_en */
typedef union {
    /* Define the struct bits */
    struct {
        u32 hash_chann_int_en : 16; /* [15..0] */
        u32 reserved_0 : 16;        /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} hash_chann_raw_int_en;

/* Define the union hash_chann_int */
typedef union {
    /* Define the struct bits */
    struct {
        u32 hash_chann_int : 16; /* [15..0] */
        u32 reserved_0 : 16;     /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} hash_chann_int;

/* Define the union in_hash_chn0_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 31;   /* [30..0] */
        unsigned int hash_chn0_req : 1; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} in_hash_chn0_ctrl;

/* Define the union in_hash_chn0_special_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hash_chn0_hmac_key_addr : 8;        /* [7..0] */
        unsigned int reserved_0 : 8;                     /* [15..8] */
        unsigned int hash_chn0_alg_key_sel : 2;          /* [17..16] */
        unsigned int reserved_1 : 6;                     /* [23..18] */
        unsigned int hash_chn0_hmac_calc_step : 1;       /* [24] */
        unsigned int reserved_2 : 2;                     /* [26..25] */
        unsigned int hash_chn0_key_beyond_block_vld : 1; /* [27] */
        unsigned int hash_chn0_half_bank_vld : 1;        /* [28] */
        unsigned int reserved_3 : 3;                     /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} in_hash_chn0_special_ctrl;

/* Define the union in_hash_chn0_key_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 16;        /* [15..0] */
        unsigned int hash_chn0_alg_sel : 4;  /* [19..16] */
        unsigned int hash_chn0_alg_mode : 4; /* [23..20] */
        unsigned int reserved_1 : 8;         /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} in_hash_chn0_key_ctrl;

/* Define the union in_hash_chn0_block_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hash_chn0_first_block : 1;     /* [0] */
        unsigned int hash_chn0_last_block : 1;      /* [1] */
        unsigned int reserved_0 : 14;               /* [15..2] */
        unsigned int hash_chn0_hmac_last_block : 1; /* [16] */
        unsigned int reserved_1 : 15;               /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} in_hash_chn0_block_ctrl;

/* Define the union in_hash_chn_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hash_chn_mmu_en : 1; /* [0] */
        unsigned int reserved_0 : 7;      /* [7..1] */
        unsigned int hash_chn_ss : 1;     /* [8] */
        unsigned int reserved_1 : 22;     /* [30..9] */
        unsigned int hash_chn_en : 1;     /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} in_hash_chn_ctrl;

/* Define the union in_hash_chn_key_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 16;       /* [15..0] */
        unsigned int hash_chn_alg_sel : 4;  /* [19..16] */
        unsigned int hash_chn_alg_mode : 4; /* [23..20] */
        unsigned int reserved_1 : 8;        /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} in_hash_chn_key_ctrl;

/* Define the union in_hash_chn_node_start_addr_h */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hash_chn_node_start_addr_h : 4; /* [3..0] */
        unsigned int reserved_0 : 28;                /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} in_hash_chn_node_start_addr_h;

/* Define the union in_hash_chn_node_length */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hash_chn_node_length : 8; /* [7..0] */
        unsigned int reserved_0 : 24;          /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} in_hash_chn_node_length;

/* Define the union in_hash_chn_node_wr_point */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hash_chn_node_wr_point : 8; /* [7..0] */
        unsigned int reserved_0 : 24;            /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} in_hash_chn_node_wr_point;

/* Define the union in_hash_chn_node_rd_point */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hash_chn_node_rd_point : 8; /* [7..0] */
        unsigned int reserved_0 : 24;            /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} in_hash_chn_node_rd_point;

/* Define the union in_hash_chn_node_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hash_chn_node_level : 8; /* [7..0] */
        unsigned int reserved_0 : 8;          /* [15..8] */
        unsigned int hash_chn_node_cnt : 8;   /* [23..16] */
        unsigned int reserved_1 : 8;          /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} in_hash_chn_node_ctrl;

/* Define the union tee_hash_calc_ctrl_check_err */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 2;                        /* [1..0] */
        unsigned int tee_hash_check_alg_error : 1;          /* [2] */
        unsigned int tee_hash_check_alg_invld_error : 1;    /* [3] */
        unsigned int reserved_1 : 1;                        /* [4] */
        unsigned int tee_hash_check_tee_error : 1;          /* [5] */
        unsigned int tee_hash_check_sc_error : 1;           /* [6] */
        unsigned int reserved_2 : 5;                        /* [11..7] */
        unsigned int tee_hash_check_hmac_lock_error : 1;    /* [12] */
        unsigned int tee_hash_check_sm3_disable_error : 1;  /* [13] */
        unsigned int reserved_3 : 3;                        /* [16..14] */
        unsigned int tee_hash_check_sha1_disable_error : 1; /* [17] */
        unsigned int reserved_4 : 14;                       /* [31..18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} tee_hash_calc_ctrl_check_err;

/* Define the union tee_hash_calc_ctrl_check_err_status */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tee_hash_error_code_clr : 1; /* [0] */
        unsigned int reserved_0 : 15;             /* [15..1] */
        unsigned int tee_error_hash_chan_id : 4;  /* [19..16] */
        unsigned int reserved_1 : 12;             /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} tee_hash_calc_ctrl_check_err_status;

/* Define the union ree_hash_calc_ctrl_check_err */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 2;                        /* [1..0] */
        unsigned int ree_hash_check_alg_error : 1;          /* [2] */
        unsigned int ree_hash_check_alg_invld_error : 1;    /* [3] */
        unsigned int reserved_1 : 1;                        /* [4] */
        unsigned int ree_hash_check_ree_error : 1;          /* [5] */
        unsigned int ree_hash_check_sc_error : 1;           /* [6] */
        unsigned int reserved_2 : 5;                        /* [11..7] */
        unsigned int ree_hash_check_hmac_lock_error : 1;    /* [12] */
        unsigned int ree_hash_check_sm3_disable_error : 1;  /* [13] */
        unsigned int reserved_3 : 3;                        /* [16..14] */
        unsigned int ree_hash_check_sha1_disable_error : 1; /* [17] */
        unsigned int reserved_4 : 14;                       /* [31..18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} ree_hash_calc_ctrl_check_err;

/* Define the union ree_hash_calc_ctrl_check_err_status */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ree_hash_error_code_clr : 1; /* [0] */
        unsigned int reserved_0 : 15;             /* [15..1] */
        unsigned int ree_error_hash_chan_id : 4;  /* [19..16] */
        unsigned int reserved_1 : 12;             /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} ree_hash_calc_ctrl_check_err_status;

/* Define the union in_hash_global_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hash_rr_weight : 4;         /* [3..0] */
        unsigned int reserved_0 : 4;             /* [7..4] */
        unsigned int sw_hash_rr_weight_lock : 1; /* [8] */
        unsigned int reserved_1 : 23;            /* [31..9] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} in_hash_global_ctrl;

/* Define the union U_SPACC_MMU_IN_HASH_CLEAR */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sw_mmu_in_hash_clr    : 16  ; /* [15..0]  */
        unsigned int    reserved_1            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_mmu_in_hash_clear;

/* Define the union U_SPACC_MMU_IN_HASH_SEC_UNVLD_VA_TAB */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mmu_in_hash_sec_unvld_va_tab : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 3   ; /* [26..24]  */
        unsigned int    mmu_in_hash_sec_unvld_chn_id : 4   ; /* [30..27]  */
        unsigned int    mmu_in_hash_sec_unvld_va_vld : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_mmu_in_hash_sec_unvld_va_tab;

/* Define the union U_SPACC_MMU_IN_HASH_SEC_UNVLD_PA_TAB */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mmu_in_hash_sec_unvld_pa_tab : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 5   ; /* [28..24]  */
        unsigned int    mmu_in_hash_sec_unvld_tag_err : 1   ; /* [29]  */
        unsigned int    mmu_in_hash_sec_unvld_vld_err : 1   ; /* [30]  */
        unsigned int    mmu_in_hash_sec_unvld_pa_vld : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_mmu_in_hash_sec_unvld_pa_tab;

/* Define the union U_SPACC_MMU_IN_HASH_NOSEC_UNVLD_VA_TAB */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mmu_in_hash_nosec_unvld_va_tab : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 3   ; /* [26..24]  */
        unsigned int    mmu_in_hash_nosec_unvld_chn_id : 4   ; /* [30..27]  */
        unsigned int    mmu_in_hash_nosec_unvld_va_vld : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_mmu_in_hash_nosec_unvld_va_tab;

/* Define the union U_SPACC_MMU_IN_HASH_NOSEC_UNVLD_PA_TAB */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mmu_in_hash_nosec_unvld_pa_tab : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 6   ; /* [29..24]  */
        unsigned int    mmu_in_hash_nosec_unvld_vld_err : 1   ; /* [30]  */
        unsigned int    mmu_in_hash_nosec_unvld_pa_vld : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_mmu_in_hash_nosec_unvld_pa_tab;

/* Define the union U_SPACC_CALC_CRG_CFG */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    spacc_aes_low_power_enable  : 1  ; /* [0]  */
        unsigned int    spacc_tdes_low_power_enable : 1  ; /* [1]  */
        unsigned int    spacc_sm4_low_power_enable  : 1  ; /* [2]  */
        unsigned int    reserved0                   : 1  ; /* [3]  */
        unsigned int    spacc_sha1_low_power_enable : 1  ; /* [4]  */
        unsigned int    spacc_sha2_low_power_enable : 1  ; /* [5]  */
        unsigned int    spacc_sm3_low_power_enable  : 1  ; /* [6]  */
        unsigned int    reserved1                   : 25 ; /* [31..7]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_calc_crg_cfg;

/** @} */ /** <!-- ==== Structure Definition end ==== */
#endif
