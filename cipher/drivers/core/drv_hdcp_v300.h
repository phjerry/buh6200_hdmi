/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_hdcp_v300
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#ifndef _DRV_HDCP_V300_H_
#define _DRV_HDCP_V300_H_

#include "drv_osal_lib.h"

/*************************** Internal Structure Definition ****************************/
/** \addtogroup      cipher drivers */
/** @{ */ /** <!-- [cipher] */

/* ! \Define the offset of reg */
#define IN_SYM_CHN0_CTRL                         0x4000
#define IN_SYM_CHN0_SPECIAL_CTRL                 0x4004
#define IN_SYM_CHN0_KEY_CTRL                     0x4010
#define IN_SYM_CHN0_CCM_GCM                      0x4020
#define IN_SYM_CHN0_IV_DATA_CTRL                 0x4030
#define SYM_CHAN0_FINISH_RAW_INT                 0x8510
#define HDCP_MODE_CTRL                           0x8110
#define HDCP_DEBUG                               0x8114

#define SYM_DEBUG_TRIGGER_POINT                  0x8090


#define IN_SYM_CHN0_IV(offset)                   (0x4040 + (offset)*0x4)
#define IN_SYM_CHN0_DATA(offset)                 (0x4050 + (offset)*0x4)
#define CHAN0_CIPHER_DOUT(offset)                (0x8100 + (offset)*0x4)


/* Define the union U_IN_SYM_CHN0_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        u32    reserved                   : 31    ; /* [30..0]  */
        u32    sym_chn0_req               : 1     ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    u32    u32;

} in_sym_chn0_ctrl;

/* Define the union U_IN_SYM_CHN0_SPECIAL_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        u32    sym_chn0_odd_even          : 1     ; /* [0]  */
        u32    reserved                   : 31    ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    u32    u32;

} in_sym_chn0_special_ctrl;

/* Define the union U_IN_SYM_CHN0_KEY_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        u32    sym_chn0_key_chn_id        : 9    ; /* [8..0]   */
        u32    reserved                   : 7    ; /* [15..9]  */
        u32    sym_chn0_alg_sel           : 4    ; /* [19..16]  */
        u32    sym_chn0_alg_mode          : 4    ; /* [23..20]  */
        u32    sym_chn0_alg_key_len       : 2    ; /* [25..24]  */
        u32    sym_chn0_alg_data_width    : 2    ; /* [27..26]  */
        u32    sym_chn0_alg_decrypt       : 1    ; /* [28]  */
        u32    reserved_1                 : 3    ; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    u32    u32;

} in_sym_chn0_key_ctrl;

/* Define the union U_IN_SYM_CHN0_CCM_GCM */
typedef union {
    /* Define the struct bits */
    struct {
        u32    sym_chn0_ccm_gcm_in_flag   : 2    ; /* [1..0]   */
        u32    reserved                   : 1    ; /* [2]  */
        u32    sym_chn0_gcm_ctr_iv_len    : 5    ; /* [7..3]  */
        u32    sym_chn0_gcm_first         : 1    ; /* [8]  */
        u32    reserved_1                 : 3    ; /* [11..9]  */
        u32    sym_chn0_ccm_last          : 1    ; /* [12]  */
        u32    reserved_2                 : 19    ; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    u32    u32;

} in_sym_chn0_ccm_gcm;

/* Define the union U_IN_SYM_CHN0_IV_DATA_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        u32    sym_chn0_first_block   : 1    ; /* [0]   */
        u32    sym_chn0_last_block    : 1    ; /* [1]  */
        u32    reserved_0             : 14   ; /* [15..2]  */
        u32    sym_chn0_block_len     : 5    ; /* [20..16]  */
        u32    reserved_1             : 11   ; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    u32    u32;

} in_sym_chn0_iv_data_ctrl;

/* Define the union U_HDCP_MODE_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    hdcp_mode_en          : 1   ; /* [0]  */
        unsigned int    hdcp_rootkey_sel      : 1   ; /* [1]  */
        unsigned int    reserved_0            : 6   ; /* [7..2]  */
        unsigned int    hdcp_mode_sel         : 1   ; /* [8]  */
        unsigned int    reserved_1            : 7   ; /* [15..9]  */
        unsigned int    hdcp_rx_mode          : 4   ; /* [19..16]  */
        unsigned int    reserved_2            : 5   ; /* [24..20]  */
        unsigned int    hdcp_tx_mode          : 3   ; /* [27..25]  */
        unsigned int    reserved_3            : 4   ; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} hdcp_mode_ctrl;

/* Define the union U_HDCP_DEBUG */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    hdcp_crc4             : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} hdcp_debug;

/** @} */ /** <!-- ==== Structure Definition end ==== */
#endif
