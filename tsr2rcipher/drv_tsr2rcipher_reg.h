/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: tsr2rcipher register definition.
 */

#ifndef __DRV_TSR2RCIPHER_REG_H__
#define __DRV_TSR2RCIPHER_REG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define TSC_READ_REG(base, offset)         osal_readl(((base) + (offset)))
#define TSC_WRITE_REG(base, offset, value) osal_writel((value), ((base) + (offset)))

#define TSC_COM_EQUAL(exp, act) do {                                                    \
    if ((exp) != (act)) {                                                               \
        HI_ERR_TSR2RCIPHER("write register error, exp=0x%x, act=0x%x\n", (exp), (act)); \
    }                                                                                   \
} while (0)

#define TSC_READ_REG_SUB(base, subbase, offset)         osal_readl(((base) + (subbase) + (offset)))
#define TSC_WRITE_REG_SUB(base, subbase, offset, value) osal_writel((value), ((base) + (subbase) + (offset)))

#define INT_FLAG_REG_NUM (32)

/* TSR2RCIPHER RX register definition */
#define TSC_READ_REG_RX(base, offset)         osal_readl(((base) + TSC_REGS_RX_BASE + (offset)))
#define TSC_WRITE_REG_RX(base, offset, value) osal_writel((value), ((base) + TSC_REGS_RX_BASE + (offset)))

typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_dsptor_full : 1;  /* [0] */
        unsigned int reserved_0     : 31; /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_DSPTOR_CTRL;

#define RX_DSPTOR_CTRL(ch) (0x10 + (ch << 6))

typedef union {
    /* define the struct bits  */
    struct {
        unsigned int rx_dsptor_start_addr : 32; /* [31..0] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_DSPTOR_START_ADDR;

#define RX_DSPTOR_START_ADDR(ch) (0x14 + (ch << 6))

typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_dsptor_length : 16; /* [15..0] */
        unsigned int reserved_0       : 15; /* [30..16] */
        unsigned int rx_dsptor_cfg    : 1;  /* [31] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_DSPTOR_LENGTH;

#define RX_DSPTOR_LENGTH(ch) (0x18 + (ch << 6))

typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_buf_type      : 1; /* [0] */
        unsigned int reserved_0       : 7; /* [7..1] */
        unsigned int rx_pkt_int_level : 8; /* [15..8] */
        unsigned int reserved_1       : 4; /* [19..16] */
        unsigned int rx_pkt_int_cnt   : 8; /* [27..20] */
        unsigned int rx_session_id    : 4; /* [31..28] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_RX_CTRL;

#define TSC_RX_CTRL(ch) (0x1C + (ch << 6))

typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0          : 1; /* [0] */
        unsigned int iraw_rx_dsptor_done : 1; /* [1] */
        unsigned int iraw_rx_pkt_cnt     : 1; /* [2] */
        unsigned int reserved_1          : 29; /* [31..3] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_IRAW_RX;

#define IRAW_RX(ch) (0x20 + (ch << 6))

typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0          : 1; /* [0] */
        unsigned int iena_rx_dsptor_done : 1; /* [1] */
        unsigned int iena_rx_pkt_cnt     : 1; /* [2] */
        unsigned int reserved_1          : 29; /* [31..3] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_IENA_RX;

#define IENA_RX(ch) (0x24 + (ch << 6))

typedef union {
    /* define the struct bits */
    struct {
        unsigned int key_id          : 8; /* [7..0] */
        unsigned int reserved_0      : 8; /* [15..8] */
        unsigned int odd_even_sel    : 1; /* [16] */
        unsigned int dsc_type        : 1; /* [17] */
        unsigned int core_sel        : 1; /* [18] */
        unsigned int reserved_1      : 5; /* [23..19] */
        unsigned int tsc_crc_en      : 1; /* [24] */
        unsigned int pl_raw_sel      : 1; /* [25] */
        unsigned int tsc_47_replace  : 1; /* [26] */
        unsigned int reserved_2      : 2; /* [28..27] */
        unsigned int tsc_tx_sec_attr : 1; /* [29] */
        unsigned int tsc_rx_sec_attr : 1; /* [30] */
        unsigned int tsc_chn_en      : 1; /* [31] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_MODE_CTRL;

#define TSC_MODE_CTRL(ch) (0x30 + (ch << 6))

typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0         : 1; /* [0] */
        unsigned int rx_dsptor_done_int : 1; /* [1] */
        unsigned int rx_pkt_cnt_int     : 1; /* [2] */
        unsigned int reserved_1         : 29; /* [31..3] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_INT_TYPE;

#define RX_INT_TYPE           (0x4010)

#define  RX_INT_DSPTOR_BLOCK  (0x4018)
#define  RX_INT_PKT_CNT_BLOCK (0x401C)

#define ISTA_RX_DSPTOR0(ch)   (0x4150 + (ch << 2))
#define ISTA_RX_PKT_CNT0(ch)  (0x4200 + (ch << 2))


/* TSR2RCIPHER TX register definition */
#define TSC_READ_REG_TX(base, offset)         osal_readl(((base) + TSC_REGS_TX_BASE + (offset)))
#define TSC_WRITE_REG_TX(base, offset, value) osal_writel((value), ((base) + TSC_REGS_TX_BASE + (offset)))

typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_dsptor_full : 1; /* [0] */
        unsigned int reserved_0     : 31; /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_DSPTOR_CTRL;

#define TX_DSPTOR_CTRL(ch) (0x10 + (ch << 6))

typedef union {
    /* define the struct bits  */
    struct {
        unsigned int tx_dsptor_start_addr : 32; /* [31..0] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_DSPTOR_START_ADDR;

#define TX_DSPTOR_START_ADDR(ch) (0x14 + (ch << 6))

/* define the union U_TX_DSPTOR_LENGTH */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_dsptor_length : 16; /* [15..0] */
        unsigned int reserved_0       : 15; /* [30..16] */
        unsigned int tx_dsptor_cfg    : 1; /* [31] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_DSPTOR_LENGTH;

#define TX_DSPTOR_LENGTH(ch) (0x18 + (ch << 6))

typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_buf_type      : 1; /* [0] */
        unsigned int tx_press_dis     : 1; /* [1] */
        unsigned int reserved_0       : 6; /* [7..2] */
        unsigned int tx_pkt_int_level : 8; /* [15..8] */
        unsigned int reserved_1       : 4; /* [19..16] */
        unsigned int tx_pkt_int_cnt   : 8; /* [27..20] */
        unsigned int tx_session_id    : 4; /* [31..28] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_TX_CTRL;

#define TSC_TX_CTRL(ch) (0x1C + (ch << 6))

typedef union {
    /* define the struct bits */
    struct {
        unsigned int iraw_tx_buf_afull   : 1; /* [0] */
        unsigned int iraw_tx_dsptor_done : 1; /* [1] */
        unsigned int iraw_tx_pkt_cnt     : 1; /* [2] */
        unsigned int reserved_0          : 29; /* [31..3] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_IRAW_TX;

#define IRAW_TX(ch) (0x20 + (ch << 6))

typedef union {
    /* define the struct bits */
    struct {
        unsigned int iena_tx_buf_afull   : 1; /* [0] */
        unsigned int iena_tx_dsptor_done : 1; /* [1] */
        unsigned int iena_tx_pkt_cnt     : 1; /* [2] */
        unsigned int reserved_0          : 29; /* [31..3] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_IENA_TX;

#define IENA_TX(ch) (0x24 + (ch << 6))

typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_buf_afull_int   : 1; /* [0] */
        unsigned int tx_dsptor_done_int : 1; /* [1] */
        unsigned int tx_pkt_cnt_int     : 1; /* [2] */
        unsigned int reserved_0         : 29; /* [31..3] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_INT_TYPE;

#define TX_INT_TYPE           (0x4010)

#define  TX_INT_DSPTOR_BLOCK  (0x4018)
#define  TX_INT_PKT_CNT_BLOCK (0x401C)

#define ISTA_TX_DSPTOR0(ch)   (0x4150 + (ch << 2))
#define ISTA_TX_PKT_CNT0(ch)  (0x4200 + (ch << 2))

/* TSR2RCIPHER TOP register definition */
#define TSC_READ_REG_TOP(base, offset)         osal_readl(((base) + TSC_REGS_TOP_BASE + (offset)))
#define TSC_WRITE_REG_TOP(base, offset, value) osal_writel((value), ((base) + TSC_REGS_TOP_BASE + (offset)))

typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx2cpu_iraw     : 1; /* [0] */
        unsigned int reserved_0      : 7; /* [7..1] */
        unsigned int tx2cpu_iraw     : 1; /* [8] */
        unsigned int reserved_1      : 7; /* [15..9] */
        unsigned int cipher2cpu_iraw : 1; /* [16] */
        unsigned int reserved_2      : 15; /* [31..17] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_IRAW;

#define TSC_IRAW (0x0)

typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx2cpu_ista     : 1; /* [0] */
        unsigned int reserved_0      : 7; /* [7..1] */
        unsigned int tx2cpu_ista     : 1; /* [8] */
        unsigned int reserved_1      : 7; /* [15..9] */
        unsigned int cipher2cpu_ista : 1; /* [16] */
        unsigned int reserved_2      : 15; /* [31..17] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_ISTA;

#define TSC_ISTA (0x4)

typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx2cpu_iena     : 1; /* [0] */
        unsigned int reserved_0      : 7; /* [7..1] */
        unsigned int tx2cpu_iena     : 1; /* [8] */
        unsigned int reserved_1      : 7; /* [15..9] */
        unsigned int cipher2cpu_iena : 1; /* [16] */
        unsigned int reserved_2      : 15; /* [31..17] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_IENA;

#define TSC_IENA (0x8)

typedef union {
    /* define the struct bits */
    struct {
        unsigned int wr_reg_lock_en : 1; /* [0] */
        unsigned int reserved_0     : 31; /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_REG_WR_LOCK_EN;

typedef union {
    /* define the struct bits */
    struct {
        unsigned int sec_wr_lock : 1; /* [0] */
        unsigned int reserved_0  : 31; /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_SEC_WR_LOCK;

typedef union {
    /* define the struct bits */
    struct {
        unsigned int nonsec_wr_lock : 1; /* [0] */
        unsigned int reserved_0     : 31; /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_NONSEC_WR_LOCK;

typedef union {
    /* define the struct bits */
    struct {
        unsigned int sec_wr_fail : 1; /* [0] */
        unsigned int reserved_0  : 31; /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_SEC_WR_FAIL_STATE;

typedef union {
    /* define the struct bits */
    struct {
        unsigned int nonsec_wr_fail : 1; /* [0] */
        unsigned int reserved_0     : 31; /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_NONSEC_WR_FAIL_STATE;

typedef union {
    /* define the struct bits */
    struct {
        unsigned int rfs_ema      : 3; /* [2..0] */
        unsigned int reserved_0   : 1; /* [3] */
        unsigned int rfs_emaw     : 2; /* [5..4] */
        unsigned int reserved_1   : 2; /* [7..6] */
        unsigned int ras_ema      : 3; /* [10..8] */
        unsigned int reserved_2   : 1; /* [11] */
        unsigned int ras_emaw     : 2; /* [13..12] */
        unsigned int reserved_3   : 2; /* [15..14] */
        unsigned int rft_emaa     : 3; /* [18..16] */
        unsigned int reserved_4   : 1; /* [19] */
        unsigned int rft_emab     : 3; /* [22..20] */
        unsigned int reserved_5   : 1; /* [23] */
        unsigned int rft_emasa    : 1; /* [24] */
        unsigned int rft_colldisn : 1; /* [25] */
        unsigned int reserved_6   : 2; /* [27..26] */
        unsigned int ros_ema      : 3; /* [30..28] */
        unsigned int reserved_7   : 1; /* [31] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_MEM_CFG;

typedef union {
    /* define the struct bits */
    struct {
        unsigned int ram_ck_gt_en : 1; /* [0] */
        unsigned int reserved_0   : 31; /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_MEM_CK_GT_EN;

#define TSC_MMU_NOSEC_TLB (0x114)

typedef union {
    /* define the struct bits */
    struct {
        unsigned int sw_mmu_rx_nosec_eaddr_h : 4; /* [3..0] */
        unsigned int reserved_0              : 28; /* [31..4] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_MMU_RX_NOSEC_EADDR_H;
#define TSC_MMU_RX_NOSEC_EADDR_H (0x00A8)

#define TSC_MMU_RX_NOSEC_EADDR_L (0x00AC)

typedef union {
    /* define the struct bits */
    struct {
        unsigned int sw_mmu_tx_nosec_eaddr_h : 4; /* [3..0] */
        unsigned int reserved_0              : 28; /* [31..4] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_MMU_TX_NOSEC_EADDR_H;
#define TSC_MMU_TX_NOSEC_EADDR_H (0x00D8)

#define TSC_MMU_TX_NOSEC_EADDR_L (0x00DC)

typedef union {
    /* define the struct bits */
    struct {
        unsigned int sw_nosec_mmu_en      : 1; /* [0] */
        unsigned int reserved_0           : 7; /* [7..1] */
        unsigned int sw_nosec_mmu_type    : 2; /* [9..8] */
        unsigned int reserved_1           : 21; /* [30..10] */
        unsigned int sw_nosec_mmu_en_lock : 1; /* [31] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TSC_MMU_NOSEC_EN;
#define TSC_MMU_NOSEC_EN (0x0110)

#define TSC_MMU_RX_CLR(index) (0x0120 + (index << 2))

#define TSC_MMU_TX_CLR(index) (0x0150 + (index << 2))

/* TSR2RCIPHER CIPHER register definition */
#define TSC_READ_REG_CIPHER(base, offset)         osal_readl(((base) + TSC_REGS_CIPHER_BASE + (offset)))
#define TSC_WRITE_REG_CIPHER(base, offset, value) osal_writel((value), ((base) + TSC_REGS_CIPHER_BASE + (offset)))


/* hi_demux_v200 MDSC register definition  begin */
#define TSC_READ_REG_MDSC(base, offset)         osal_readl(((base) + DMX_TSC_REGS_MDSC_BASE + (offset)))
#define TSC_WRITE_REG_MDSC(base, offset, value) osal_writel((value), ((base) + DMX_TSC_REGS_MDSC_BASE + (offset)))

#define BITS_PER_REG             (32)
#define KEY_ENCRPTY_SEL(reg_idx) (0x0000 + ((reg_idx) << 2)) /* IV or CW KEY even/odd bits select register */
#define CSA2_ENTROPY_CLOSE       (0x0040) /* CSA2 entropy decrease register */

typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0            : 8; /* [7..0] */
        unsigned int ca_en                 : 1; /* [8] */
        unsigned int reserved_1            : 3; /* [11..9] */
        unsigned int ts_ctrl_dsc_change_en : 1; /* [12] */
        unsigned int reserved_2            : 3; /* [23..13] */
        unsigned int cw_iv_en              : 1; /* [24] */
        unsigned int reserved_3            : 7; /* [31..25] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_MDSC_EN;

#define MDSC_EN (0x0050) /* MDSC enable register */

#ifdef __cplusplus
}
#endif

#endif  /* __DRV_TSR2RCIPHER_REG_H__ */
