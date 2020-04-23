/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:KLAD register and structure definition.
 * Author: Linux SDK team
 * Create: 2019/06/22
 */
#ifndef __HAL_KLAD_REG_H__
#define __HAL_KLAD_REG_H__

#define PERM_REG_RANGE             0x1000
#define PERM_REG_BASE              0x00b04000
/*
 * only used for multi2 algorithm, once this register is configuared,
 * the next times configuaration on this register can not change the register any more.
 */
#define PERM_MULTI2_CFG            (PERM_REG_BASE + 0x45c)

#define HKL_DATA_IN_0              (KLAD_REG_BASE + 0x000)
#define HKL_DATA_IN_1              (KLAD_REG_BASE + 0x004)
#define HKL_DATA_IN_2              (KLAD_REG_BASE + 0x008)
#define HKL_DATA_IN_3              (KLAD_REG_BASE + 0x00C)
#define HKL_KEY_ADDR               (KLAD_REG_BASE + 0x010)
#define HKL_KEY_CFG                (KLAD_REG_BASE + 0x014)
#define HKL_KEY_SEC_CFG            (KLAD_REG_BASE + 0x018)
#define HKL_STATE                  (KLAD_REG_BASE + 0x030)
#define HKL_CRC                    (KLAD_REG_BASE + 0x034)
#define HKL_ERROR                  (KLAD_REG_BASE + 0x038)
#define HKL_KC_ERROR               (KLAD_REG_BASE + 0x03C)
#define HKL_INT_EN                 (KLAD_REG_BASE + 0x040)
#define HKL_INT_RAW                (KLAD_REG_BASE + 0x044)
#define HKL_INT                    (KLAD_REG_BASE + 0x048)
#define HKL_POWER_EN               (KLAD_REG_BASE + 0x0050)
#define HKL_POWER_EN_LOCK          (KLAD_REG_BASE + 0x0054)
#define HKL_CMD_STAT               (KLAD_REG_BASE + 0x0058)
#define HKL_LOCK_CTRL              (KLAD_REG_BASE + 0x074)
#define HKL_UNLOCK_CTRL            (KLAD_REG_BASE + 0x078)
#define HKL_COM_LOCK_INFO          (KLAD_REG_BASE + 0x07C)
#define HKL_COM_LOCK_STAT(n)       (KLAD_REG_BASE + (n)*0x30+0x80)
#define HKL_COM_CTRL(n)            (KLAD_REG_BASE + (n)*0x30+0x84)
#define HKL_COM_STAT(n)            (KLAD_REG_BASE + (n)*0x30+0x88)
#define HKL_COM_RK_INFO(n)         (KLAD_REG_BASE + (n)*0x30+0x8C)
#define HKL_COM_RK_TAG_0(n)        (KLAD_REG_BASE + (n)*0x30+0x90)
#define HKL_COM_RK_TAG_1(n)        (KLAD_REG_BASE + (n)*0x30+0x94)
#define HKL_COM_RK_TAG_2(n)        (KLAD_REG_BASE + (n)*0x30+0x98)
#define HKL_COM_RK_TAG_3(n)        (KLAD_REG_BASE + (n)*0x30+0x9C)
#define HKL_C2_CCC_RSLT_0          (KLAD_REG_BASE + 0x3B0)
#define HKL_C2_CCC_RSLT_1          (KLAD_REG_BASE + 0x3B4)
#define HKL_C2_CCC_RSLT_2          (KLAD_REG_BASE + 0x3B8)
#define HKL_C2_CCC_RSLT_3          (KLAD_REG_BASE + 0x3BC)
#define HKL_HIS_CCC_RSLT_0         (KLAD_REG_BASE + 0x3C0)
#define HKL_HIS_CCC_RSLT_1         (KLAD_REG_BASE + 0x3C4)
#define HKL_HIS_CCC_RSLT_2         (KLAD_REG_BASE + 0x3C8)
#define HKL_HIS_CCC_RSLT_3         (KLAD_REG_BASE + 0x3CC)
#define HKL_NONCE_LOCK_INFO        (KLAD_REG_BASE + 0x400)
#define HKL_NONCE_LOCK_STAT        (KLAD_REG_BASE + 0x404)
#define HKL_NONCE_RK_SEL           (KLAD_REG_BASE + 0x408)
#define HKL_NONCE_CTRL             (KLAD_REG_BASE + 0x40C)
#define HKL_NONCE_STAT             (KLAD_REG_BASE + 0x410)
#define HKL_DA_NONCE_0             (KLAD_REG_BASE + 0x420)
#define HKL_DA_NONCE_1             (KLAD_REG_BASE + 0x424)
#define HKL_DA_NONCE_2             (KLAD_REG_BASE + 0x428)
#define HKL_DA_NONCE_3             (KLAD_REG_BASE + 0x42C)
#define HKL_CLR_LOCK_INFO          (KLAD_REG_BASE + 0x430)
#define HKL_CLR_LOCK_STAT          (KLAD_REG_BASE + 0x434)
#define HKL_CLR_CTRL               (KLAD_REG_BASE + 0x438)
#define HKL_FP_LOCK_INFO           (KLAD_REG_BASE + 0x450)
#define HKL_FP_LOCK_STAT           (KLAD_REG_BASE + 0x454)
#define HKL_FP_RK_SEL              (KLAD_REG_BASE + 0x458)
#define HKL_FP_CTRL                (KLAD_REG_BASE + 0x45C)
#define HKL_FP_STAT                (KLAD_REG_BASE + 0x460)
#define HKL_FP_DEC_CTRL            (KLAD_REG_BASE + 0x464)
#define HKL_FP_ENC_RSLT_0          (KLAD_REG_BASE + 0x470)
#define HKL_FP_ENC_RSLT_1          (KLAD_REG_BASE + 0x474)
#define HKL_FP_ENC_RSLT_2          (KLAD_REG_BASE + 0x478)
#define HKL_FP_ENC_RSLT_3          (KLAD_REG_BASE + 0x47C)
#define HKL_FP_DEC_RSLT_0          (KLAD_REG_BASE + 0x480)
#define HKL_FP_DEC_RSLT_1          (KLAD_REG_BASE + 0x484)
#define HKL_FP_DEC_RSLT_2          (KLAD_REG_BASE + 0x488)
#define HKL_FP_DEC_RSLT_3          (KLAD_REG_BASE + 0x48C)
#define HKL_TA_LOCK_INFO           (KLAD_REG_BASE + 0x4A0)
#define HKL_TA_LOCK_STAT           (KLAD_REG_BASE + 0x4A4)
#define HKL_TA_CTRL                (KLAD_REG_BASE + 0x4A8)
#define HKL_TA_STAT                (KLAD_REG_BASE + 0x4AC)
#define HKL_CSGK2_LOCK_INFO        (KLAD_REG_BASE + 0x4B0)
#define HKL_CSGK2_LOCK_STATUS      (KLAD_REG_BASE + 0x4B4)
#define HKL_CSGK2_CTRL             (KLAD_REG_BASE + 0x4B8)
#define HKL_CSGK2_DISABLE          (KLAD_REG_BASE + 0x4BC)
#define HKL_CSGK2_DISABLE_LOCK     (KLAD_REG_BASE + 0x4C0)
#define HKL_ALARM_INFO             (KLAD_REG_BASE + 0x600)

/* define the union perm_multi2_cfg */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    ca_multi2_round_num   : 8  ;  /* [7..0]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} perm_multi2_cfg;

/* define the union u_kl_key_addr */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    key_addr              : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_key_addr;

/* define the union hkl_key_cfg */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    port_sel              : 2   ; /* [1..0]  */
        unsigned int    reserved_0            : 2   ; /* [3..2]  */
        unsigned int    dsc_code              : 8   ; /* [11..4]  */
        unsigned int    reserved_1            : 4   ; /* [15..12]  */
        unsigned int    key_enc               : 1   ; /* [16]  */
        unsigned int    key_dec               : 1   ; /* [17]  */
        unsigned int    reserved_2            : 14  ; /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_key_cfg;

/* define the union hkl_key_sec_cfg */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    key_sec               : 1   ; /* [0]  */
        unsigned int    src_nsec              : 1   ; /* [1]  */
        unsigned int    src_sec               : 1   ; /* [2]  */
        unsigned int    dest_nsec             : 1   ; /* [3]  */
        unsigned int    dest_sec              : 1   ; /* [4]  */
        unsigned int    hpp_only              : 1   ; /* [5]  */
        unsigned int    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_key_sec_cfg;

/* define the union hkl_state */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_busy               : 2   ; /* [1..0]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_state;

/* define the union hkl_crc */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_crc                : 8   ; /* [7..0]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_crc;

/* define the union hkl_error */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    level_sel_err         : 1   ; /* [0]  */
        unsigned int    algo_sel_err          : 1   ; /* [1]  */
        unsigned int    port_sel_err          : 1   ; /* [2]  */
        unsigned int    dsc_code_err          : 1   ; /* [3]  */
        unsigned int    dec_enc_err           : 1   ; /* [4]  */
        unsigned int    kl_disable_err        : 1   ; /* [5]  */
        unsigned int    key_size_err          : 1   ; /* [6]  */
        unsigned int    rk_busy_err           : 1   ; /* [7]  */
        unsigned int    level_seq_err         : 1   ; /* [8]  */
        unsigned int    reserved_0            : 3   ; /* [11..9]  */
        unsigned int    rk_rdy_err            : 1   ; /* [12]  */
        unsigned int    lvl_rdy_err           : 1   ; /* [13]  */
        unsigned int    lv2_rdy_err           : 1   ; /* [14]  */
        unsigned int    lv3_rdy_err           : 1   ; /* [15]  */
        unsigned int    lv4_rdy_err           : 1   ; /* [16]  */
        unsigned int    reserved_1            : 3   ; /* [19..17]  */
        unsigned int    tdes_key_err          : 1   ; /* [20]  */
        unsigned int    cw_check_err          : 1   ; /* [21]  */
        unsigned int    reserved_2            : 1   ; /* [22]  */
        unsigned int    stage_cfg_err         : 1   ; /* [23]  */
        unsigned int    rk_access_err         : 1   ; /* [24]  */
        unsigned int    ta_hash_err           : 1   ; /* [25]  */
        unsigned int    ta_rslt_rdy_err       : 1   ; /* [26]  */
        unsigned int    ta_access_err         : 1   ; /* [27]  */
        unsigned int    reserved_3            : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_error;

/* define the union u_kc_error */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    tpp_access_err        : 1   ; /* [0]  */
        unsigned int    hpp_access_err        : 1   ; /* [1]  */
        unsigned int    tee_access_err        : 1   ; /* [2]  */
        unsigned int    ree_access_err        : 1   ; /* [3]  */
        unsigned int    csa2_hardonly_err     : 1   ; /* [4]  */
        unsigned int    csa3_hardonly_err     : 1   ; /* [5]  */
        unsigned int    aes_hardonly_err      : 1   ; /* [6]  */
        unsigned int    sm4_hardonly_err      : 1   ; /* [7]  */
        unsigned int    tdes_hardonly_err     : 1   ; /* [8]  */
        unsigned int    multi2_hardonly_err   : 1   ; /* [9]  */
        unsigned int    csa2_dis_err          : 1   ; /* [10]  */
        unsigned int    csa3_dis_err          : 1   ; /* [11]  */
        unsigned int    aes_dis_err           : 1   ; /* [12]  */
        unsigned int    des_dis_err           : 1   ; /* [13]  */
        unsigned int    sm4_dis_err           : 1   ; /* [14]  */
        unsigned int    tdes_dis_err          : 1   ; /* [15]  */
        unsigned int    multi2_dis_err        : 1   ; /* [16]  */
        unsigned int    asa_dis_err           : 1   ; /* [17]  */
        unsigned int    buffer_security_err   : 1   ; /* [18]  */
        unsigned int    reserved_0            : 1   ; /* [19]  */
        unsigned int    enc_dec_err           : 1   ; /* [20]  */
        unsigned int    send_time_out         : 1   ; /* [21]  */
        unsigned int    reserved_1            : 10  ; /* [31..22]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_kc_error;

/* define the union hkl_int_en */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_int_en             : 1   ; /* [0]  */
        unsigned int    kl_lock_int_en        : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_int_en;

/* define the union hkl_int_raw */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_int_raw            : 1   ; /* [0]  */
        unsigned int    reserved_0            : 3   ; /* [3..1]  */
        unsigned int    kl_int_num            : 5   ; /* [8..4]  */
        unsigned int    reserved_1            : 3   ; /* [11..9]  */
        unsigned int    kl_com_lock_int_raw   : 1   ; /* [12]  */
        unsigned int    kl_nonce_lock_int_raw : 1   ; /* [13]  */
        unsigned int    kl_clr_lock_int_raw   : 1   ; /* [14]  */
        unsigned int    kl_fp_lock_int_raw    : 1   ; /* [15]  */
        unsigned int    kl_ta_lock_int_raw    : 1   ; /* [16]  */
        unsigned int    kl_csgk2_lock_int_raw : 1   ; /* [17]  */
        unsigned int    reserved_2            : 14  ; /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_int_raw;

/* define the union hkl_int */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_int                : 1   ; /* [0]  */
        unsigned int    kl_lock_int           : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_int;

/* define the union hkl_power_en */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_power_en           : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_power_en;

/* define the union hkl_power_en_lock */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_power_en_lock      : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_power_en_lock;

/* define the union hkl_rk_gen_status */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_rk_gen_busy        : 9   ; /* [8..0]  */
        unsigned int    reserved_0            : 23  ; /* [31..9]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_rk_gen_status;

/* define the union hkl_lock_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_lock               : 1   ; /* [0]  */
        unsigned int    reserved_0            : 3   ; /* [3..1]  */
        unsigned int    kl_lock_num           : 3   ; /* [6..4]  */
        unsigned int    reserved_1            : 25  ; /* [31..7]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_lock_ctrl;

/* define the union hkl_unlock_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_unlock             : 1   ; /* [0]  */
        unsigned int    reserved_0            : 3   ; /* [3..1]  */
        unsigned int    kl_unlock_num         : 3   ; /* [6..4]  */
        unsigned int    reserved_1            : 1   ; /* [7]  */
        unsigned int    kl_com_unlock_num     : 3   ; /* [10..8]  */
        unsigned int    reserved_2            : 21  ; /* [31..11]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_unlock_ctrl;

/* define the union hkl_com_lock_info */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_com_lock_busy      : 2   ; /* [1..0]  */
        unsigned int    kl_com_lock_fail      : 2   ; /* [3..2]  */
        unsigned int    kl_com_unlock_fail    : 2   ; /* [5..4]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    kl_com_lock_num       : 3   ; /* [10..8]  */
        unsigned int    reserved_1            : 21  ; /* [31..11]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_com_lock_info;

/* define the union hkl_com_lock_status */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_com_lock_stat      : 8   ; /* [7..0]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_com_lock_status;

/* define the union hkl_com_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_com_start          : 1   ; /* [0]  */
        unsigned int    kl_com_level_sel      : 3   ; /* [3..1]  */
        unsigned int    kl_com_alg_sel        : 2   ; /* [5..4]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    kl_com_ta_sel         : 1   ; /* [8]  */
        unsigned int    kl_com_key_abort         : 1   ; /* [9]  */
        unsigned int    reserved_1            : 22  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_com_ctrl;

/* define the union hkl_com_status */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_com_rk_rdy         : 1   ; /* [0]  */
        unsigned int    kl_com_lv1_rdy        : 1   ; /* [1]  */
        unsigned int    kl_com_lv2_rdy        : 1   ; /* [2]  */
        unsigned int    kl_com_lv3_rdy        : 1   ; /* [3]  */
        unsigned int    kl_com_lv4_rdy        : 1   ; /* [4]  */
        unsigned int    kl_com_lv5_rdy        : 1   ; /* [5]  */
        unsigned int    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_com_status;

/* define the union hkl_com_rk_info */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    ca_owner_id           : 8   ; /* [7..0]  */
        unsigned int    csa2_spec_check_en    : 1   ; /* [8]  */
        unsigned int    reserved_0            : 23  ; /* [31..9]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_com_rk_info;

/* define the union hkl_com_rk_tag0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    reserved_0             : 2   ; /* [1..0]  */
        unsigned int    key_decrypt_enable     : 1   ; /* [2]  */
        unsigned int    key_encrypt_enable     : 1   ; /* [3]  */
        unsigned int    scipher_port_enable    : 1   ; /* [4]  */
        unsigned int    mcipher_port_enable    : 1   ; /* [5]  */
        unsigned int    reserved_1             : 1   ; /* [6]  */
        unsigned int    tscipher_port_enable   : 1   ; /* [7]  */
        unsigned int    sm4_engine_enable      : 1   ; /* [8]  */
        unsigned int    tdes_engine_enable     : 1   ; /* [9]  */
        unsigned int    aes_engine_enable      : 1   ; /* [10]  */
        unsigned int    csa3_engine_enable     : 1   ; /* [11]  */
        unsigned int    csa2_engine_enable     : 1   ; /* [12]  */
        unsigned int    multi2_engine_enable   : 1   ; /* [13]  */
        unsigned int    sm3_engine_enable      : 1   ; /* [14]  */
        unsigned int    sha_hmac_engine_enable : 1   ; /* [15]  */
        unsigned int    reserved_2             : 8   ; /* [23..16]  */
        unsigned int    kl_stage               : 4   ; /* [27..24]  */
        unsigned int    reserved_3             : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_com_rk_tag0;

/* define the union hkl_com_rk_tag1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_aes_algo_enable     : 1   ; /* [0]  */
        unsigned int    kl_tdes_algo_enable    : 1   ; /* [1]  */
        unsigned int    kl_sm4_algo_enable     : 1   ; /* [2]  */
        unsigned int    reserved_0             : 29  ; /* [31..3]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_com_rk_tag1;

/* define the union hkl_com_rk_tag2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    reserved_0            : 1   ; /* [25]  */
        unsigned int    flash_prot_en         : 1   ; /* [26]  */
        unsigned int    nonce_en              : 1   ; /* [27]  */
        unsigned int    c2_checksum_en        : 1   ; /* [28]  */
        unsigned int    cm_checksum_en        : 1   ; /* [29]  */
        unsigned int    hdcp_rk               : 1   ; /* [30]  */
        unsigned int    reserved_1            : 2   ; /* [32..31]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_com_rk_tag2;

/* define the union hkl_nonce_lock_info */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_nonce_lock_busy    : 2   ; /* [1..0]  */
        unsigned int    kl_nonce_lock_fail    : 2   ; /* [3..2]  */
        unsigned int    kl_nonce_unlock_fail  : 2   ; /* [5..4]  */
        unsigned int    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_nonce_lock_info;

/* define the union hkl_nonce_lock_status */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_nonce_lock_stat    : 8   ; /* [7..0]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_nonce_lock_status;

/* define the union hkl_nonce_rk_sel */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_nonce_rk_sel       : 3   ; /* [2..0]  */
        unsigned int    reserved_0            : 29  ; /* [31..3]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_nonce_rk_sel;

/* define the union hkl_nonce_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_nonce_start        : 1   ; /* [0]  */
        unsigned int    reserved_0            : 3   ; /* [3..1]  */
        unsigned int    kl_nonce_alg_sel      : 2   ; /* [5..4]  */
        unsigned int    reserved_1            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_nonce_ctrl;

/* define the union hkl_nonce_status */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_nonce_rk_rdy       : 1   ; /* [0]  */
        unsigned int    kl_nonce_lvl1_rdy     : 1   ; /* [1]  */
        unsigned int    kl_nonce_lvl2_rdy     : 1   ; /* [2]  */
        unsigned int    reserved_0            : 29  ; /* [31..3]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_nonce_status;

/* define the union hkl_clr_lock_info */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_clr_lock_busy      : 2   ; /* [1..0]  */
        unsigned int    kl_clr_lock_fail      : 2   ; /* [3..2]  */
        unsigned int    kl_clr_unlock_fail    : 2   ; /* [5..4]  */
        unsigned int    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_clr_lock_info;

/* define the union hkl_clr_lock_status */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_clr_lock_stat      : 8   ; /* [7..0]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_clr_lock_status;

/* define the union hkl_clr_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_clr_start          : 1   ; /* [0]  */
        unsigned int    kl_clr_iv_sel         : 1   ; /* [1]  */
        unsigned int    kl_clr_key_size       : 2   ; /* [3..2]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_clr_ctrl;

/* define the union hkl_fp_lock_info */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_fp_lock_busy       : 2   ; /* [1..0]  */
        unsigned int    kl_fp_lock_fail       : 2   ; /* [3..2]  */
        unsigned int    kl_fp_unlock_fail     : 2   ; /* [5..4]  */
        unsigned int    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_fp_lock_info;

/* define the union hkl_fp_lock_status */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_fp_lock_stat       : 8   ; /* [7..0]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_fp_lock_status;

/* define the union hkl_fp_rk_sel */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_fp_rk_sel          : 3   ; /* [2..0]  */
        unsigned int    reserved_0            : 29  ; /* [31..3]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_fp_rk_sel;

/* define the union hkl_fp_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_fp_start           : 1   ; /* [0]  */
        unsigned int    kl_fp_level_sel       : 1   ; /* [1]  */
        unsigned int    reserved_0            : 4   ; /* [5..2]  */
        unsigned int    kl_fp_dec_sel         : 1   ; /* [6]  */
        unsigned int    reserved_1            : 25  ; /* [31..7]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_fp_ctrl;

/* define the union hkl_fp_status */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_fp_rk_rdy          : 1   ; /* [0]  */
        unsigned int    kl_fp_lv1_enc_rdy     : 1   ; /* [1]  */
        unsigned int    kl_fp_lv1_dec_rdy     : 1   ; /* [2]  */
        unsigned int    reserved_0            : 29  ; /* [31..3]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_fp_status;

/* define the union hkl_fp_dec_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_fp_dec_rd_dis      : 4   ; /* [3..0]  */
        unsigned int    kl_fp_dec_route_dis   : 4   ; /* [7..4]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_fp_dec_ctrl;

/* define the union hkl_ta_lock_info */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_ta_lock_busy       : 2   ; /* [1..0]  */
        unsigned int    kl_ta_lock_fail       : 2   ; /* [3..2]  */
        unsigned int    kl_ta_unlock_fail     : 2   ; /* [5..4]  */
        unsigned int    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_ta_lock_info;

/* define the union hkl_ta_lock_status */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_ta_lock_stat       : 8   ; /* [7..0]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_ta_lock_status;

/* define the union hkl_ta_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_ta_start           : 1   ; /* [0]  */
        unsigned int    kl_ta_level_sel       : 2   ; /* [2..1]  */
        unsigned int    reserved_0            : 2   ; /* [4..3]  */
        unsigned int    kl_ta_cur_128bit_cnt  : 6   ; /* [10..5]  */
        unsigned int    kl_ta_last_time       : 1   ; /* [11]  */
        unsigned int    kl_ta_lut_alg_sel     : 1   ; /* [12]  */
        unsigned int    reserved_1            : 19  ; /* [31..13]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_ta_ctrl;

/* define the union hkl_ta_status */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_ta_rk_rdy          : 1   ; /* [0]  */
        unsigned int    kl_ta_lvl1_rdy        : 1   ; /* [1]  */
        unsigned int    kl_ta_lvl2_rdy        : 1   ; /* [2]  */
        unsigned int    kl_ta_lvl3_rdy        : 1   ; /* [3]  */
        unsigned int    kl_ta_f_lut_rdy       : 1   ; /* [4]  */
        unsigned int    kl_ta_f_m_rdy         : 1   ; /* [5]  */
        unsigned int    kl_ta_f_bc_rdy        : 1   ; /* [6]  */
        unsigned int    reserved_0            : 25  ; /* [31..7]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_ta_status;

/* define the union hkl_csgk2_lock_info */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_csgk2_lock_busy    : 2   ; /* [1..0]  */
        unsigned int    kl_csgk2_lock_fail    : 2   ; /* [3..2]  */
        unsigned int    kl_csgk2_unlock_fail  : 2   ; /* [5..4]  */
        unsigned int    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_csgk2_lock_info;

/* define the union hkl_csgk2_lock_status */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_csgk2_lock_stat    : 8   ; /* [7..0]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_csgk2_lock_status;

/* define the union hkl_csgk2_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_csgk2_start        : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_csgk2_ctrl;

/* define the union hkl_csgk2_disable */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_csgk2_dis          : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_csgk2_disable;

/* define the union hkl_csgk2_disable_lock */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    kl_csgk2_dis_lock     : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_csgk2_disable_lock;

/* define the union hkl_alarm_info */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    rng_crc4_alarm        : 1   ; /* [0]  */
        unsigned int    kl_cfg_sig_alarm      : 1   ; /* [1]  */
        unsigned int    kl_rk_tag_sig_alarm   : 1   ; /* [2]  */
        unsigned int    kl_rk_tag_crc16_alarm : 1   ; /* [3]  */
        unsigned int    kl_rk_info_crc4_alarm : 1   ; /* [4]  */
        unsigned int    kl_sel_sig_alarm      : 1   ; /* [5]  */
        unsigned int    kl_com_crc16_alarm    : 1   ; /* [6]  */
        unsigned int    kl_nonce_crc16_alarm  : 1   ; /* [7]  */
        unsigned int    kl_fp_crc16_alarm     : 1   ; /* [8]  */
        unsigned int    kl_ta_crc16_alarm     : 1   ; /* [9]  */
        unsigned int    kl_ta_fsm_alarm       : 1   ; /* [10]  */
        unsigned int    reserved_0            : 13  ; /* [23..11]  */
        unsigned int    cm_core_alarm         : 1   ; /* [24]  */
        unsigned int    reserved_1            : 7   ; /* [31..25]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} hkl_alarm_info;

#endif /* __HAL_KLAD_REG_H__ */
