/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_symc_v300
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#ifndef _DRV_SYMC_V300_H_
#define _DRV_SYMC_V300_H_

#include "drv_osal_lib.h"

/*************************** Internal Structure Definition ****************************/
/** \addtogroup      cipher drivers */
/** @{ */ /** <!-- [cipher] */

/* ! SYMC in entry list size */
#define SYMC_IN_NODE_SIZE (64 * SYMC_MAX_LIST_NUM) /* max: 64 * 255 ~ 16K */

#define SYMC_OUT_NODE_SIZE (16 * SYMC_MAX_LIST_NUM) /* max: 16 * 255 ~ 4k */

#define SYMC_NODE_SIZE (SYMC_IN_NODE_SIZE + SYMC_OUT_NODE_SIZE) /* max: 16k + 4k = 20K */

/* ! SYMC in entry list size */
#define SYMC_NODE_LIST_SIZE (SYMC_NODE_SIZE * CIPHER_HARD_CHANNEL_CNT)

/* ! \Define the offset of reg */
#define SPACC_IS                              0x0000
#define SPACC_IE                              0x0004
#define SPACC_SYM_CHN_LOCK                    0x0020
#define SPACC_INT_RAW_SYM_CLEAR_FINISH        0x0008
#define SPACC_EN_INT_RAW_SYM_CLEAR_FINISH     0x000c
#define SPACC_SYM_CHN_CLEAR_REQ               0x0060
#define SPACC_MMU_SEC_TYPE                    0x2000
#define SPACC_MMU_SEC_TLB                     0x2004
#define SPACC_MMU_NOSEC_TYPE                  0x2010
#define SPACC_MMU_NOSEC_TLB                   0x2014
#define SPACC_MMU_IN_SYM_SEC_EADDR_H          0x2100
#define SPACC_MMU_IN_SYM_SEC_EADDR_L          0x2104
#define SPACC_MMU_IN_SYM_NOSEC_EADDR_H        0x2110
#define SPACC_MMU_IN_SYM_NOSEC_EADDR_L        0x2114
#define SPACC_MMU_IN_SYM_TAG_ID_H             0x2120
#define SPACC_MMU_IN_SYM_TAG_ID_L             0x2124
#define SPACC_MMU_IN_SYM_CLEAR                0x2130
#define SPACC_MMU_IN_SYM_SEC_UNVLD_VA_TAB     0x2140
#define SPACC_MMU_IN_SYM_SEC_UNVLD_PA_TAB     0x2144
#define SPACC_MMU_IN_SYM_NOSEC_UNVLD_VA_TAB   0x2148
#define SPACC_MMU_IN_SYM_NOSEC_UNVLD_PA_TAB   0x214c
#define SPACC_MMU_OUT_SYM_SEC_EADDR_H         0x2500
#define SPACC_MMU_OUT_SYM_SEC_EADDR_L         0x2504
#define SPACC_MMU_OUT_SYM_NOSEC_EADDR_H       0x2510
#define SPACC_MMU_OUT_SYM_NOSEC_EADDR_L       0x2514
#define SPACC_MMU_OUT_SYM_TAG_ID_H            0x2520
#define SPACC_MMU_OUT_SYM_TAG_ID_L            0x2524
#define SPACC_MMU_OUT_SYM_CLEAR               0x2530
#define SPACC_MMU_OUT_SYM_SEC_UNVLD_VA_TAB    0x2540
#define SPACC_MMU_OUT_SYM_SEC_UNVLD_PA_TAB    0x2544
#define SPACC_MMU_OUT_SYM_NOSEC_UNVLD_VA_TAB  0x2548
#define SPACC_MMU_OUT_SYM_NOSEC_UNVLD_PA_TAB  0x254c
#define SPACC_AXI_IRAW                        0x3010
#define IN_SYM_CHN_CTRL(id)                   (0x4000 + (id)*0x100)
#define IN_SYM_OUT_CTRL(id)                   (0x4004 + (id)*0x100)
#define IN_SYM_CHN_KEY_CTRL(id)               (0x4010 + (id)*0x100)
#define IN_SYM_CHN_NODE_START_ADDR_H(id)      (0x4020 + (id)*0x100)
#define IN_SYM_CHN_NODE_START_ADDR_L(id)      (0x4024 + (id)*0x100)
#define IN_SYM_CHN_NODE_LENGTH(id)            (0x402c + (id)*0x100)
#define IN_SYM_CHN_NODE_WR_POINT(id)          (0x4030 + (id)*0x100)
#define IN_SYM_CHN_NODE_RD_POINT(id)          (0x4034 + (id)*0x100)
#define IN_SYM_CHN_NODE_CTRL(id)              (0x4040 + (id)*0x100)
#define DBG_IN_SYM_CHN_RD_ADDR_H(id)          (0x40a0 + (id)*0x100)
#define DBG_IN_SYM_CHN_RD_ADDR_L(id)          (0x40a4 + (id)*0x100)
#define DBG_IN_SYM_CHN_DATA_SA_H(id)          (0x40b0 + (id)*0x100)
#define DBG_IN_SYM_CHN_DATA_SA_L(id)          (0x40a4 + (id)*0x100)
#define DBG_IN_SYM_CHN_DATA_LEN(id)           (0x40c0 + (id)*0x100)
#define IN_SYM_GLOBAL_CTRL                    0x6000
#define IRAW_IN_SYM_NODE_CNT                  0x6510
#define IENA_IN_SYM_NODE_CNT                  0x6514
#define ISTA_IN_SYM_NODE_CNT                  0x6518
#define ISTA_IN_HASH_NODE_CNT                 0x6A18
#define SPACC_CALC_INC_CFG                    0x8008
#define SPACC_CALC_CRG_CFG                    0x8030
#define HPP_SYM_CALC_CTRL_CHECK_ERR           0x8058
#define HPP_SYM_CALC_CTRL_CHECK_ERR_STATUS    0x805c
#define TEE_SYM_CALC_CTRL_CHECK_ERR           0x8060
#define TEE_SYM_CALC_CTRL_CHECK_ERR_STATUS    0x8064
#define REE_SYM_CALC_CTRL_CHECK_ERR           0x8070
#define REE_SYM_CALC_CTRL_CHECK_ERR_STATUS    0x8074

#define SYM_CALC_CTRL_CHECK_ERR(cpu)          (0x8000 + 0x80 - (cpu)*0x10)
#define SYM_CALC_CTRL_CHECK_ERR_STATUS(cpu)   (0x8000 + 0x84 - (cpu)*0x10)

#define HDCP_DEBUG                            0x8114
#define CIPHER_ROBUST_ALARM_DEBUG             0x8200

#define SYM_CHANN_RAW_INT                     0x8500
#define SYM_CHANN_RAW_INT_EN                  0x8504
#define SYM_CHANN_INT                         0x8508

#define SYM_CHANN_ERROR_RAW_INT               0x8520
#define SYM_CHANN_ERROR_RAW_INT_EN            0x8524
#define SYM_CHANN_ERROR_INT                   0x8528

#define HASH_CHANN_INT                        0x8608

#define CHANn_CIPHER_IVOUT(id)             (0x9000 + (id)*0x80)
#define CHANn_CIPHER_CCM_GCM_TAG(id, word) (0x9010 + (word)*0x4 + (id)*0x80)
#define CHANN_CIPHER_DFA_EN(id)            (0x9020 + (id)*0x80)

#define OUT_SYM_CHAN_RAW_LAST_NODE_INT     0xc000
#define OUT_SYM_CHAN_RAW_LAST_NODE_INT_EN  0xc004
#define OUT_SYM_CHAN_LAST_NODE_INT         0xc008
#define OUT_SYM_CHAN_RAW_LEVEL_INT         0xc010
#define OUT_SYM_CHAN_RAW_LEVEL_INT_EN      0xc014
#define OUT_SYM_CHAN_LEVEL_INT             0xc018

#define OUT_SYM_CHN_CTRL(id)                  (0xcf00 + (id)*0x100)
#define OUT_SYM_CHN_STATUS(id)                (0xcf04 + (id)*0x100)
#define OUT_SYM_CHN_NODE_START_ADDR_H(id)     (0xcf20 + (id)*0x100)
#define OUT_SYM_CHN_NODE_START_ADDR_L(id)     (0xcf24 + (id)*0x100)
#define OUT_SYM_CHN_NODE_LENGTH(id)           (0xcf28 + (id)*0x100)
#define OUT_SYM_CHN_NODE_WR_POINT(id)         (0xcf30 + (id)*0x100)
#define OUT_SYM_CHN_NODE_RD_POINT(id)         (0xcf34 + (id)*0x100)
#define OUT_SYM_CHN_NODE_CTRL(id)             (0xcf40 + (id)*0x100)
#define DBG_OUT_SYM_CHN_RD_ADDR_H(id)         (0xcfa0 + (id)*0x100)
#define DBG_OUT_SYM_CHN_RD_ADDR_L(id)         (0xcfa4 + (id)*0x100)
#define DBG_OUT_SYM_CHN_NODE_LEFT_BUF_LEN(id) (0xcfa8 + (id)*0x100)

#define SYMC_LOW_POWER_ENABLE                  0x01

#define SPACC_CPU_HPP   0x03
#define SPACC_CPU_TEE   0x02
#define SPACC_CPU_REE   0x01
#define SPACC_CPU_IDLE  0x00

#define CHN_WHO_USED_CLR(used, chn)      (used) &= ~(0x03U << ((chn)*2U))
#define CHN_WHO_USED_SET(used, chn, who) (used) |= (who) << ((chn)*2U)
#define CHN_WHO_USED_GET(used, chn)      (((used) >> ((chn)*2U)) & 0x03U)

typedef enum {
    ECB = 0x01,
    CBC = 0x03,
    CTR = 0x06,
    OFB = 0x07,
    CFB = 0x08,
    CCM = 0x09,
    GCM = 0x0A,
} sym_alg_mode;

typedef enum {
    AES = 0x02,
    TDES = 0x07,
    SM4 = 0x05,
} sym_alg_sel;

typedef enum {
    KEY_64BIT = 0,
    KEY_128BIT = 1,
    KEY_192BIT = 2,
    KEY_256BIT = 3,
    KEY_CNT = 4
} sym_alg_key_len;

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

/* Define the union spacc_ie */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int spacc_ie_ree : 1; /* [0] */
        unsigned int reserved_0 : 3;   /* [3..1] */
        unsigned int spacc_ie_tee : 1; /* [4] */
        unsigned int reserved_1 : 3;   /* [7..5] */
        unsigned int spacc_ie_hpp : 1; /* [8] */
        unsigned int reserved_2 : 23;  /* [31..9] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} spacc_ie;

/* Define the union in_sym_out_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        u32 sym_dma_copy : 1; /* [0] */
        u32 reserved : 31;    /* [31..1] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} in_sym_out_ctrl;

/* Define the union in_sym_chn_key_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        u32 sym_key_chn_id : 9;     /* [8..0] */
        u32 reserved : 7;           /* [16..9] */
        u32 sym_alg_sel : 4;        /* [19..16] */
        u32 sym_alg_mode : 4;       /* [23..20] */
        u32 sym_alg_key_len : 2;    /* [25..24] */
        u32 sym_alg_data_width : 2; /* [27..26] */
        u32 sym_alg_decrypt : 1;    /* [28] */
        u32 reserved_2 : 3;         /* [31..29] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} in_sym_chn_key_ctrl;

/* Define the union in_sym_chn_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        u32 sym_chn_mmu_en : 1; /* [0] */
        u32 reserved : 7;       /* [7..1] */
        u32 sym_chn_ss : 4;     /* [8..11] */
        u32 sym_chn_ds : 4;     /* [12..15] */
        u32 reserved_3 : 15;    /* [30..16] */
        u32 sym_chn_en : 1;     /* [31] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} in_sym_chn_ctrl;

/* Define the union out_sym_chn_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        u32 reserved : 8;           /* [7..0] */
        u32 out_sym_chn_mmu_en : 1; /* [8] */
        u32 reserved_2 : 23;        /* [31..9] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} out_sym_chn_ctrl;

/* Define the union out_sym_chan_int */
typedef union {
    /* Define the struct bits */
    struct {
        u32 out_sym_chan_int : 16; /* [15..0] */
        u32 reserved_1 : 16;       /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} out_sym_chan_int;

/* Define the union out_sym_chan_raw_int_en */
typedef union {
    /* Define the struct bits */
    struct {
        u32 out_sym_chan_int_en : 16; /* [15..1] */
        u32 reserved_1 : 16;          /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} out_sym_chan_raw_int_en;

/* Define the union out_sym_chan_raw_int */
typedef union {
    /* Define the struct bits */
    struct {
        u32 out_sym_chan_raw_int : 16; /* [15..0] */
        u32 reserved_1 : 16;           /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} out_sym_chan_raw_int;

/* Define the union out_sym_chan_raw_level_int */
typedef union {
    /* Define the struct bits */
    struct {
        u32 out_sym_level_chan_raw_int : 16; /* [15..0] */
        u32 reserved_1 : 16;                 /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} out_sym_chan_raw_level_int;

/* Define the union out_sym_chan_raw_level_int_en */
typedef union {
    /* Define the struct bits */
    struct {
        u32 out_sym_level_chan_int_en : 16; /* [15..0] */
        u32 reserved_1 : 16;                /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} out_sym_chan_raw_level_int_en;

/* Define the union out_sym_chan_level_int */
typedef union {
    /* Define the struct bits */
    struct {
        u32 out_sym_level_chan_int : 16; /* [15..1] */
        u32 reserved_1 : 16;             /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} out_sym_chan_level_int;

/* Define the union out_sym_int_enable */
typedef union {
    /* Define the struct bits */
    struct {
        u32 out_sym_last_node_int_enable : 1;  /* [0] */
        u32 out_sym_node_empty_int_enable : 1; /* [1] */
        u32 out_sym_node_level_int_enable : 1; /* [2] */
        u32 reserved : 29;                     /* [31..3] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} out_sym_int_enable;

/* Define the union out_sym_chn_status */
typedef union {
    /* Define the struct bits */
    struct {
        u32 out_cur_sym_chn_node_level : 8; /* [7..0] */
        u32 reserved_0 : 8;                 /* [15..8] */
        u32 out_sym_chn_int_status : 2;     /* [16..17] */
        u32 reserved_1 : 14;                /* [31..18] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} out_sym_chn_status;

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

/* Define the union iena_in_sym_node_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        u32 iena_in_sym_node_cnt : 16; /* [15..1] */
        u32 reserved_1 : 16;           /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} iena_in_sym_node_cnt;

/* Define the union out_sym_chan_raw_int */
typedef union {
    /* Define the struct bits */
    struct {
        u32 iraw_in_sym_node_cnt : 16; /* [15..1] */
        u32 reserved_1 : 16;           /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} iraw_in_sym_node_cnt;

/* Define the union out_sym_chn_node_wr_point */
typedef union {
    /* Define the struct bits */
    struct {
        u32 out_sym_chn_node_wr_point : 8; /* [7..0] */
        u32 reserved_1 : 24;               /* [31..8] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} out_sym_chn_node_wr_point;

/* Define the union out_sym_chn_node_wr_point */
typedef union {
    /* Define the struct bits */
    struct {
        u32 sym_chn_node_wr_point : 8; /* [7..0] */
        u32 reserved_1 : 24;           /* [31..8] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} in_sym_chn_node_wr_point;

/* Define the union out_sym_chn_node_rd_point */
typedef union {
    /* Define the struct bits */
    struct {
        u32 out_sym_chn_node_rd_point : 8; /* [7..0] */
        u32 reserved_1 : 24;               /* [31..8] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} out_sym_chn_node_rd_point;

/* Define the union out_sym_chn_node_rd_point */
typedef union {
    /* Define the struct bits */
    struct {
        u32 sym_chn_node_rd_point : 8; /* [7..0] */
        u32 reserved_1 : 24;           /* [31..8] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} in_sym_chn_node_rd_point;

/* Define the union in_sym_chn_node_length */
typedef union {
    /* Define the struct bits */
    struct {
        u32 sym_chn_node_length : 8; /* [7..0] */
        u32 reserved_1 : 24;         /* [31..8] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} in_sym_chn_node_length;

/* Define the union out_sym_chn_node_length */
typedef union {
    /* Define the struct bits */
    struct {
        u32 out_sym_chn_node_length : 8; /* [7..0] */
        u32 reserved_1 : 24;             /* [31..8] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} out_sym_chn_node_length;

/* Define the union dbg_in_sym_chn_rd_addr_h */
typedef union {
    /* Define the struct bits */
    struct {
        u32 dbg_sym_chn_rd_addr_h : 4; /* [3..0] */
        u32 reserved : 28;             /* [31..4] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} dbg_in_sym_chn_rd_addr_h;

/* Define the union dbg_out_sym_chn_rd_addr_h */
typedef union {
    /* Define the struct bits */
    struct {
        u32 dbg_sym_chn_rd_addr_h : 4; /* [3..0] */
        u32 reserved : 28;             /* [31..4] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} dbg_out_sym_chn_rd_addr_h;

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

/* Define the union sym_calc_ctrl_check_err */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    calc_ctrl_check_ds_error : 1   ; /* [0]  */
        unsigned int    calc_ctrl_check_ss_error : 1   ; /* [1]  */
        unsigned int    calc_ctrl_check_alg_error : 1   ; /* [2]  */
        unsigned int    calc_ctrl_check_alg_invld_error : 1   ; /* [3]  */
        unsigned int    calc_ctrl_check_dec_error : 1   ; /* [4]  */
        unsigned int    calc_ctrl_check_error : 1   ; /* [5]  */
        unsigned int    calc_ctrl_check_sc_error : 1   ; /* [6]  */
        unsigned int    calc_ctrl_check_key_size_error : 1   ; /* [7]  */
        unsigned int    calc_ctrl_check_hdcp_error : 1   ; /* [8]  */
        unsigned int    calc_ctrl_check_cenc_dec_error : 1   ; /* [9]  */
        unsigned int    calc_ctrl_check_cenc_alg_error : 1   ; /* [10]  */
        unsigned int    calc_ctrl_check_gcm_ccm_error : 1   ; /* [11]  */
        unsigned int    reserved_0            : 2   ; /* [13..12]  */
        unsigned int    calc_ctrl_check_iv_length_error : 1   ; /* [14]  */
        unsigned int    calc_ctrl_check_data_length_error : 1   ; /* [15]  */
        unsigned int    calc_ctrl_big_key_info_diff_error : 1   ; /* [16]  */
        unsigned int    reserved_1            : 1   ; /* [17]  */
        unsigned int    calc_ctrl_hdcp_key_size_error : 1   ; /* [18]  */
        unsigned int    calc_ctrl_hdcp_sm4_error : 1   ; /* [19]  */
        unsigned int    reserved_2            : 11  ; /* [30..20]  */
        unsigned int    calc_ctrl_des_keysame_error : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} sym_calc_ctrl_check_err;

/* Define the union sym_calc_ctrl_check_err_status */
typedef union {
    /* Define the struct bits */
    struct {
        u32 hpp_sym_error_code_clr : 1; /* [0] */
        u32 reserved : 15;              /* [15..1] */
        u32 hpp_error_sym_chan_id : 4;  /* [19..16] */
        u32 reserved1 : 12;             /* [31..20] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} sym_calc_ctrl_check_err_status;

/* Define the union spacc_axi_iraw */
typedef union {
    /* Define the struct bits */
    struct {
        u32 spacc_mmu_in_sym_sec_err_iraw : 1;    /* [0] */
        u32 spacc_mmu_in_hash_sec_err_iraw : 1;   /* [1] */
        u32 spacc_mmu_out_sym_sec_err_iraw : 1;   /* [2] */
        u32 reserved : 5;                         /* [7..3] */
        u32 spacc_mmu_in_sym_nosec_err_iraw : 1;  /* [8] */
        u32 spacc_mmu_in_hash_nosec_err_iraw : 1; /* [9] */
        u32 spacc_mmu_out_sym_nosec_err_iraw : 1; /* [10] */
        u32 reserved2 : 21;                       /* [31..11] */
    } bits;

    /* Define an unsigned member */
    u32 u32;

} spacc_axi_iraw;

/* Define the union in_sym_chn_node_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sym_chn_node_level : 8;     /* [7..0] */
        unsigned int reserved_0 : 8;             /* [15..8] */
        unsigned int sym_chn_node_cnt : 8;       /* [23..16] */
        unsigned int reserved_1 : 7;             /* [30..24] */
        unsigned int sym_chn_node_cnt_clear : 1; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} in_sym_chn_node_ctrl;

/* Define the union out_sym_chn_node_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int out_sym_chn_node_level : 8; /* [7..0] */
        unsigned int reserved_0 : 24;            /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} out_sym_chn_node_ctrl;

/* Define the union spacc_sym_chn_clear_req */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sym_chn_clear_req : 16; /* [15..0] */
        unsigned int reserved_0 : 16;        /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} spacc_sym_chn_clear_req;

/* Define the union spacc_int_raw_sym_clear_finish */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int int_raw_sym_clear_finish : 16; /* [15..0] */
        unsigned int reserved_0 : 16;               /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} spacc_int_raw_sym_clear_finish;

/* Define the union sym_chann_raw_int */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sym_chann_raw_int : 16; /* [0] */
        unsigned int reserved_0 : 16;        /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} sym_chann_raw_int;

/* Define the union sym_chann_raw_int_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sym_chann_int_en : 16; /* [0] */
        unsigned int reserved_0 : 16;       /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} sym_chann_raw_int_en;

/* Define the union sym_chann_int */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sym_chann_int : 16; /* [0] */
        unsigned int reserved_0 : 16;    /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} sym_chann_int;

/* Define the union sym_chan0_finish_raw_int */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sym_chan0_finish_raw_int : 1; /* [0] */
        unsigned int reserved_0 : 31;              /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} sym_chan0_finish_raw_int;

/* Define the union sym_chan0_finish_raw_int_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sym_chan0_finish_int_en : 1; /* [0] */
        unsigned int reserved_0 : 31;             /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} sym_chan0_finish_raw_int_en;

/* Define the union sym_chan0_finish_int */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sym_chan0_finish_int : 1; /* [0] */
        unsigned int reserved_0 : 31;          /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} sym_chan0_finish_int;

/* Define the union sym_chann_error_raw_int */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sym_chan_raw_err_int : 16; /* [0] */
        unsigned int reserved_0 : 16;           /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} sym_chann_error_raw_int;

/* Define the union sym_chann_error_raw_int_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sym_chan_err_int_en : 16; /* [0] */
        unsigned int reserved_0 : 16;          /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} sym_chann_error_raw_int_en;

/* Define the union sym_chann_error_int */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sym_chan_err_int : 16; /* [0] */
        unsigned int reserved_0 : 16;       /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} sym_chann_error_int;

/* Define the union in_sym_global_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sym_rr_weight : 4;         /* [3..0] */
        unsigned int reserved_0 : 4;            /* [7..4] */
        unsigned int sw_sym_rr_weight_lock : 1; /* [8] */
        unsigned int reserved_1 : 23;           /* [31..9] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} in_sym_global_ctrl;

/* Define the union spacc_calc_inc_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int spacc_gcm_incr_disable : 1; /* [0] */
        unsigned int spacc_ccm_incr_disable : 1; /* [1] */
        unsigned int reserved : 30;              /* [31..2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} spacc_calc_inc_cfg;

/* Define the union U_SPACC_MMU_IN_SYM_CLEAR */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sw_mmu_in_sym_clr     : 16  ; /* [15..0]  */
        unsigned int    reserved_1            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_mmu_in_sym_clear;

/* Define the union U_SPACC_MMU_IN_SYM_SEC_UNVLD_VA_TAB */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mmu_in_sym_sec_unvld_va_tab : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 3   ; /* [26..24]  */
        unsigned int    mmu_in_sym_sec_unvld_chn_id : 4   ; /* [30..27]  */
        unsigned int    mmu_in_sym_sec_unvld_va_vld : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_mmu_in_sym_sec_unvld_va_tab;

/* Define the union U_SPACC_MMU_IN_SYM_SEC_UNVLD_PA_TAB */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mmu_in_sym_sec_unvld_pa_tab : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 5   ; /* [28..24]  */
        unsigned int    mmu_in_sym_sec_unvld_tag_err : 1   ; /* [29]  */
        unsigned int    mmu_in_sym_sec_unvld_vld_err : 1   ; /* [30]  */
        unsigned int    mmu_in_sym_sec_unvld_pa_vld : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_mmu_in_sym_sec_unvld_pa_tab;

/* Define the union U_SPACC_MMU_IN_SYM_NOSEC_UNVLD_VA_TAB */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mmu_in_sym_nosec_unvld_va_tab : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 3   ; /* [26..24]  */
        unsigned int    mmu_in_sym_nosec_unvld_chn_id : 4   ; /* [30..27]  */
        unsigned int    mmu_in_sym_nosec_unvld_va_vld : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_mmu_in_sym_nosec_unvld_va_tab;

/* Define the union U_SPACC_MMU_IN_SYM_NOSEC_UNVLD_PA_TAB */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mmu_in_sym_nosec_unvld_pa_tab : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 6   ; /* [29..24]  */
        unsigned int    mmu_in_sym_nosec_unvld_vld_err : 1   ; /* [30]  */
        unsigned int    mmu_in_sym_nosec_unvld_pa_vld : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_mmu_in_sym_nosec_unvld_pa_tab;

/* Define the union U_SPACC_MMU_OUT_SYM_CLEAR */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sw_mmu_out_sym_clr    : 16  ; /* [15..0]  */
        unsigned int    reserved_1            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_mmu_out_sym_clear;

/* Define the union U_SPACC_MMU_OUT_SYM_SEC_UNVLD_VA_TAB */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mmu_out_sym_sec_unvld_va_tab : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 3   ; /* [26..24]  */
        unsigned int    mmu_out_sym_sec_unvld_chn_id : 4   ; /* [30..27]  */
        unsigned int    mmu_out_sym_sec_unvld_va_vld : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_mmu_out_sym_sec_unvld_va_tab;

/* Define the union U_SPACC_MMU_OUT_SYM_SEC_UNVLD_PA_TAB */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mmu_out_sym_sec_unvld_pa_tab : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 5   ; /* [28..24]  */
        unsigned int    mmu_out_sym_sec_unvld_tag_err : 1   ; /* [29]  */
        unsigned int    mmu_out_sym_sec_unvld_vld_err : 1   ; /* [30]  */
        unsigned int    mmu_out_sym_sec_unvld_pa_vld : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_mmu_out_sym_sec_unvld_pa_tab;

/* Define the union U_SPACC_MMU_OUT_SYM_NOSEC_UNVLD_VA_TAB */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mmu_out_sym_nosec_unvld_va_tab : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 3   ; /* [26..24]  */
        unsigned int    mmu_out_sym_nosec_unvld_chn_id : 4   ; /* [30..27]  */
        unsigned int    mmu_out_sym_nosec_unvld_va_vld : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_mmu_out_sym_nosec_unvld_va_tab;

/* Define the union U_SPACC_MMU_OUT_SYM_NOSEC_UNVLD_PA_TAB */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mmu_out_sym_nosec_unvld_pa_tab : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 6   ; /* [29..24]  */
        unsigned int    mmu_out_sym_nosec_unvld_vld_err : 1   ; /* [30]  */
        unsigned int    mmu_out_sym_nosec_unvld_pa_vld : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} spacc_mmu_out_sym_nosec_unvld_pa_tab;

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
