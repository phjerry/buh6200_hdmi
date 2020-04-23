/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: drivers of hal_hici_define
 * Author: ÕÔ¹ðºé z00268517
 * Create: 2014-08-02
 */

#ifndef __HAL_HICI_DEFINE_H__
#define __HAL_HICI_DEFINE_H__

/* Define the union U_SLAVE_MODE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int slave_mode : 1;  /* [0]  */
        unsigned int reserved_0 : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} slave_mode;

/* Define the union U_CI_INF_CMD_MODE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ci_oen_pol : 1;     /* [0]  */
        unsigned int reserved_0 : 3;     /* [3..1]  */
        unsigned int cd2_dis : 1;        /* [4]  */
        unsigned int reserved_1 : 3;     /* [7..5]  */
        unsigned int irq_en : 1;         /* [8]  */
        unsigned int reserved_2 : 3;     /* [11..9]  */
        unsigned int card2_irq_en : 1;   /* [12]  */
        unsigned int reserved_3 : 3;     /* [15..13]  */
        unsigned int plug_allow_col : 1; /* [16]  */
        unsigned int reserved_4 : 3;     /* [19..17]  */
        unsigned int reserved_5 : 12;    /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
}ci_inf_cmd_mode;

/* Define the union U_CI_INF_SET */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int set_ce1_n : 1;        /* [0]  */
        unsigned int set_ce2_n : 1;        /* [1]  */
        unsigned int set_card2_ce1_n : 1;  /* [2]  */
        unsigned int set_card2_ce2_n : 1;  /* [3]  */
        unsigned int set_reg_n : 1;        /* [4]  */
        unsigned int rsv_0 : 3;            /* [7..5]  */
        unsigned int set_ci_reset : 1;     /* [8]  */
        unsigned int set_card2_reset : 1;  /* [9]  */
        unsigned int rsv_1 : 2;            /* [11..10]  */
        unsigned int sw_com_oen : 1;       /* [12]  */
        unsigned int sw_card2_com_oen : 1; /* [13]  */
        unsigned int rsv_2 : 2;            /* [15..14]  */
        unsigned int sw_rst_oen : 1;       /* [16]  */
        unsigned int sw_card2_rst_oen : 1; /* [17]  */
        unsigned int rsv_3 : 2;            /* [19..18]  */
        unsigned int sw_force_ce1 : 1;     /* [20]  */
        unsigned int rsv_4 : 7;            /* [27..21]  */
        unsigned int card2_cd2_n : 1;      /* [28]  */
        unsigned int card2_cd1_n : 1;      /* [29]  */
        unsigned int rsv_5 : 1;            /* [30]  */
        unsigned int card2_sel : 1;        /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} ci_inf_set;

/* Define the union U_CMD_CFG */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int inpackn_check : 1;   /* [0]  */
        unsigned int rdy_check : 1;       /* [1]  */
        unsigned int io_addr_check : 1;   /* [2]  */
        unsigned int rsv_0 : 5;           /* [7..3]  */
        unsigned int data_msb_rev : 1;    /* [8]  */
        unsigned int rsv_1 : 1;           /* [9]  */
        unsigned int rsv_2 : 6;           /* [15..10]  */
        unsigned int t_waitn_det_num : 4; /* [19..16]  */
        unsigned int waitn_chg_limit : 4; /* [23..20]  */
        unsigned int data_oen_mode : 1;   /* [24]  */
        unsigned int rsv_3 : 6;           /* [30..25]  */
        unsigned int cmd_doen_pol : 1;    /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} cmd_cfg;

/* Define the union U_EN_INT */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ena_ci_int : 1;  /* [0]  */
        unsigned int reserved_0 : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} en_int;

/* Define the union U_DBG_IN_SIG */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cd2 : 1;          /* [0]  */
        unsigned int cd1 : 1;          /* [1]  */
        unsigned int rdy : 1;          /* [2]  */
        unsigned int vs1 : 1;          /* [3]  */
        unsigned int inpack_n : 1;     /* [4]  */
        unsigned int wait_n : 1;       /* [5]  */
        unsigned int card2_wait_n : 1; /* [6]  */
        unsigned int card2_ready : 1;  /* [7]  */
        unsigned int data_in : 8;      /* [15..8]  */
        unsigned int reserved_0 : 16;  /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} dbg_in_sig;

/* Define the union U_CMD0_SET */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int wdata : 8;    /* [7..0]  */
        unsigned int cmd_type : 2; /* [9..8]  */
        unsigned int rvs_0 : 6;    /* [15..10]  */
        unsigned int addr : 16;    /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} cmd0_set;

/* Define the union U_CMD0_RDATA */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rdata : 8;       /* [7..0]  */
        unsigned int done : 1;        /* [8]  */
        unsigned int reserved_0 : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} cmd0_rdata;

/* Define the union U_PERI_CRG98 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0           : 1;   /* [0]  */
        unsigned int ci_srst_req         : 1;   /* [1]  */
        unsigned int reserved1           : 2;   /* [3..2]  */
        unsigned int ci_cken             : 1;   /* [4]  */
        unsigned int reserved2           : 28;   /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} peri_crg98;

/* Define the union U_PERI_IO_OEN */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int peri_tso0_oen     : 1;   /* [0]  */
        unsigned int reserved0         : 2;   /* [2..1]  */
        unsigned int peri_tso_loop_sel : 1;   /* [3]  */
        unsigned int reserved1         : 1;   /* [4]  */
        unsigned int peri_dvb0_tsi_sel : 1;   /* [5]  */
        unsigned int reserved2         : 26;  /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} peri_io_oen;

#define PERI_CRG_BASE        (0xF8A22000L)
#define PERI_CTRL_BASE       (0xF8A20000L)
#define CI_PHY_BASE_ADDR     (0xF9850000L)
#define PERI_CRG98           0x0188
#define PERI_IO_OEN          0x08AC

#define SLAVE_MODE      0x8
#define CI_INF_CMD_MODE 0xc
#define CI_INF_SET      0x10
#define CMD_CFG         0x114
#define EN_INT          0x214
#define DBG_IN_SIG      0x308
#define CMD0_SET        0x500
#define CMD0_RDATA      0x504

#endif /* __C_UNION_DEFINE_H__ */
