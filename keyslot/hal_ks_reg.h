/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:KS register and structure definition.
 * Author: Linux SDK team
 * Create: 2019/06/22
 */
#ifndef __HAL_KS_REG_H__
#define __HAL_KS_REG_H__

#define KC_REG_RANGE            0x100
#define KC_REG_BASE             0x00b0ab00

#define KC_TEE_LOCK_CMD         (KC_REG_BASE + 0x00)
#define KC_REE_LOCK_CMD         (KC_REG_BASE + 0x04)
#define KC_TEE_FLUSH_BUSY       (KC_REG_BASE + 0x10)
#define KC_REE_FLUSH_BUSY       (KC_REG_BASE + 0x14)

#define KC_SEND_DBG             (KC_REG_BASE + 0x20)
#define KC_ROB_ALARM            (KC_REG_BASE + 0x24)
#define KC_RD_SLOT_NUM          (KC_REG_BASE + 0x30)
#define KC_RD_LOCK_STATUS       (KC_REG_BASE + 0x34)
#define KC_SS_DS_CHK_FAIL       (KC_REG_BASE + 0x40)

/* define the union u_kc_teecpu_lock_cmd */
typedef union {
    struct {
        unsigned int    tee_key_slot_num      : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 6   ; /* [15..10]  */
        unsigned int    tee_tscipher_ind      : 1   ; /* [16]  */
        unsigned int    reserved_1            : 3   ; /* [19..17]  */
        unsigned int    tee_lock_cmd          : 1   ; /* [20]  */
        unsigned int    reserved_2            : 11  ; /* [31..21]  */
    } bits;
    unsigned int    u32;
} kc_tee_lock_cmd;

/* define the union kc_ree_lock_cmd */
typedef union {
    struct {
        unsigned int    ree_key_slot_num      : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 6   ; /* [15..10]  */
        unsigned int    ree_tscipher_ind      : 1   ; /* [16]  */
        unsigned int    reserved_1            : 3   ; /* [19..17]  */
        unsigned int    ree_lock_cmd          : 1   ; /* [20]  */
        unsigned int    reserved_2            : 11  ; /* [31..21]  */
    } bits;
    unsigned int    u32;
} kc_ree_lock_cmd;

/* define the union kc_hpp_lock_cmd */
typedef union {
    struct {
        unsigned int    hpp_key_slot_num      : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 6   ; /* [15..10]  */
        unsigned int    hpp_tscipher_ind      : 1   ; /* [16]  */
        unsigned int    reserved_1            : 3   ; /* [19..17]  */
        unsigned int    hpp_lock_cmd          : 1   ; /* [20]  */
        unsigned int    reserved_2            : 11  ; /* [31..21]  */
    } bits;
    unsigned int    u32;
} kc_hpp_lock_cmd;

/* define the union kc_tee_flush_busy */
typedef union {
    struct {
        unsigned int    tee_flush_busy        : 1   ; /* [0]  */
        unsigned int    tee_flush_fail        : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;
    unsigned int    u32;
} kc_tee_flush_busy;

/* define the union kc_ree_flush_busy */
typedef union {
    struct {
        unsigned int    ree_flush_busy        : 1   ; /* [0]  */
        unsigned int    ree_flush_fail        : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;
    unsigned int    u32;
} kc_ree_flush_busy;

/* define the union kc_hpp_flush_busy */
typedef union {
    struct {
        unsigned int    hpp_flush_busy        : 1   ; /* [0]  */
        unsigned int    hpp_flush_fail        : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;
    unsigned int    u32;
} kc_hpp_flush_busy;

/* define the union kc_send_dbg */
typedef union {
    struct {
        unsigned int    reserved_0            : 4   ; /* [3..0]  */
        unsigned int    kc_cur_st             : 4   ; /* [7..4]  */
        unsigned int    reserved_1            : 24  ; /* [31..8]  */
    } bits;
    unsigned int    u32;
} kc_send_dbg;

/* define the union kc_rob_alarm */
typedef union {
    struct {
        unsigned int    kc_rob_alarm          : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;
    unsigned int    u32;
} kc_rob_alarm;

/* define the union kc_rd_slot_num */
typedef union {
    struct {
        unsigned int    slot_num_cfg          : 10 ; /* [9..0]  */
        unsigned int    reserved_0            : 6  ; /* [15..10]  */
        unsigned int    tscipher_slot_ind     : 1  ; /* [16]  */
        unsigned int    reserved_1            : 15 ; /* [31..17]  */
    } bits;
    unsigned int    u32;
} kc_rd_slot_num;

/* define the union kc_rd_lock_status */
typedef union {
    struct {
        unsigned int    rd_lock_status        : 3;  /* [2..0]  */
        unsigned int    rd_tpp_key_enable_bit : 1;  /* [3] */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;
    unsigned int    u32;
} kc_rd_lock_status;

#endif /* __HAL_KS_REG_H__ */
