/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
* Description :hpp rom keyladder.
* Author : Hisilicon security team
* Created : 2019-03-13
*/

#ifndef _DRV_RKP_REG_H__
#define _DRV_RKP_REG_H__

#include "drv_rkp_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OTP_SHADOW_REG_RANGE       0x1000
#define OTP_SHADOW_REG_BASE        0x00b00000

#define OTP_SHW_OWNER_UPPER_BIT    (OTP_SHADOW_REG_BASE + 0x00C)
#define OTP_SHW_CAS_SLOT0_FLAG     (OTP_SHADOW_REG_BASE + 0x010)
#define OTP_SHW_CAS_SLOT1_FLAG     (OTP_SHADOW_REG_BASE + 0x014)
#define OTP_SHW_CAS_SLOT2_FLAG     (OTP_SHADOW_REG_BASE + 0x018)
#define OTP_SHW_CAS_SLOT3_FLAG     (OTP_SHADOW_REG_BASE + 0x01C)
#define OTP_SHW_CAS_SLOT4_FLAG     (OTP_SHADOW_REG_BASE + 0x020)
#define OTP_SHW_CAS_SLOT5_FLAG     (OTP_SHADOW_REG_BASE + 0x024)
#define OTP_SHW_CAS_SLOT6_FLAG     (OTP_SHADOW_REG_BASE + 0x028)
#define OTP_SHW_CAS_SLOT7_FLAG     (OTP_SHADOW_REG_BASE + 0x02C)
#define OTP_SHW_BOOT_SLOT_FLAG     (OTP_SHADOW_REG_BASE + 0x4E0)
#define OTP_SHW_STBM_SLOT_FLAG     (OTP_SHADOW_REG_BASE + 0x4E4)

#define RKP_REG_RANGE               0x1000
#define RKP_REG_BASE                0x00b05000

#define RKP_DEBUG_INFO_0            (RKP_REG_BASE + 0x0000)
#define RKP_DEBUG_INFO_1            (RKP_REG_BASE + 0x0004)
#define RKP_DEBUG_INFO_3            (RKP_REG_BASE + 0x0008)
#define RKP_INTE_INFO               (RKP_REG_BASE + 0x000c)
#define RKP_RESERVED0               (RKP_REG_BASE + 0x0010)
#define RKP_CRC_INFO                (RKP_REG_BASE + 0x0018)
#define RKP_OTP_RK_VLD              (RKP_REG_BASE + 0x001c)
#define RKP_RESERVED1               (RKP_REG_BASE + 0x0020)
#define RKP_ROBUST_ALARM            (RKP_REG_BASE + 0x0040)
#define RKP_RESERVED2               (RKP_REG_BASE + 0x0044)
#define RKP_SLOT0_CRC_RST           (RKP_REG_BASE + 0x0050)
#define RKP_SLOT1_CRC_RST           (RKP_REG_BASE + 0x0054)
#define RKP_SLOT2_CRC_RST           (RKP_REG_BASE + 0x0058)
#define RKP_SLOT3_CRC_RST           (RKP_REG_BASE + 0x005c)
#define RKP_SLOT4_CRC_RST           (RKP_REG_BASE + 0x0060)
#define RKP_SLOT5_CRC_RST           (RKP_REG_BASE + 0x0064)
#define RKP_SLOT6_CRC_RST           (RKP_REG_BASE + 0x0068)
#define RKP_SLOT7_CRC_RST           (RKP_REG_BASE + 0x006c)
#define RKP_BOOT_CRC_RST            (RKP_REG_BASE + 0x0070)
#define RKP_HISI_CRC_RST            (RKP_REG_BASE + 0x0074)
#define RKP_STBM_CRC_RST            (RKP_REG_BASE + 0x0078)
#define RKP_RESERVED3               (RKP_REG_BASE + 0x007c)
#define RKP_VERSION                 (RKP_REG_BASE + 0x00fc)
#define RKP_SLOT0_CHECKNUM          (RKP_REG_BASE + 0x0100)
#define RKP_SLOT1_CHECKNUM          (RKP_REG_BASE + 0x0104)
#define RKP_SLOT2_CHECKNUM          (RKP_REG_BASE + 0x0108)
#define RKP_SLOT3_CHECKNUM          (RKP_REG_BASE + 0x010c)
#define RKP_SLOT4_CHECKNUM          (RKP_REG_BASE + 0x0110)
#define RKP_SLOT5_CHECKNUM          (RKP_REG_BASE + 0x0114)
#define RKP_SLOT6_CHECKNUM          (RKP_REG_BASE + 0x0118)
#define RKP_SLOT7_CHECKNUM          (RKP_REG_BASE + 0x011c)
#define RKP_BOOT_CHECKNUM           (RKP_REG_BASE + 0x0120)
#define RKP_HISI_CHECKNUM           (RKP_REG_BASE + 0x0124)
#define RKP_STBM_CHECKNUM           (RKP_REG_BASE + 0x0128)
#define RKP_RESERVED4               (RKP_REG_BASE + 0x012c)
#define RKP_CALC_CRC_0              (RKP_REG_BASE + 0x0150)
#define RKP_CALC_CRC_1              (RKP_REG_BASE + 0x0154)
#define RKP_RESERVED5               (RKP_REG_BASE + 0x0158)
#define RKP_LOW_POWER               (RKP_REG_BASE + 0x0160)
#define RKP_USE_STATUS              (RKP_REG_BASE + 0x0164)
#define RKP_RESERVED6               (RKP_REG_BASE + 0x0168)

#define RKP_TPP_OFFSET              0x0000
#define RKP_TCPU_OFFSET             0x0100
#define RKP_HPP_OFFSET              0x0200
#define RKP_ACPU_OFFSET             0x0300

#define RKP_CALC_START              (RKP_REG_BASE + 0x0200)
#define RKP_SLOT_CHOOSE             (RKP_REG_BASE + 0x0204)
#define RKP_KLAD_CHOOSE             (RKP_REG_BASE + 0x0208)
#define RKP_VENDOR_ID               (RKP_REG_BASE + 0x020c)
#define RKP_MODULE_ID_0             (RKP_REG_BASE + 0x0210)
#define RKP_MODULE_ID_1             (RKP_REG_BASE + 0x0214)
#define RKP_MODULE_ID_2             (RKP_REG_BASE + 0x0218)
#define RKP_MODULE_ID_3             (RKP_REG_BASE + 0x021c)
#define RKP_UNIQUE_TYPE             (RKP_REG_BASE + 0x0220)
#define RKP_FIXED_RK_DATA           (RKP_REG_BASE + 0x0224)
#define RKP_FIXED_RK_KEY            (RKP_REG_BASE + 0x0228)
#define RKP_FIXED_RK_KEY_SEL        (RKP_REG_BASE + 0x022c)
#define RKP_INT_STATUS              (RKP_REG_BASE + 0x0230)
#define RKP_RAW_INT                 (RKP_REG_BASE + 0x0234)
#define RKP_INT_ENABLE              (RKP_REG_BASE + 0x0238)
#define RKP_ERROR_STATUS            (RKP_REG_BASE + 0x023c)
#define RKP_MODULE_ID_DISABLE       (RKP_REG_BASE + 0x0240)
#define RKP_CAS_KDF_STATIC_DISABLE  (RKP_REG_BASE + 0x0244)
#define RKP_EFF_RK_0                (RKP_REG_BASE + 0x0250)
#define RKP_EFF_RK_1                (RKP_REG_BASE + 0x0254)
#define RKP_EFF_RK_2                (RKP_REG_BASE + 0x0258)
#define RKP_EFF_RK_3                (RKP_REG_BASE + 0x025c)

/* define the union u_rkp_debug_info_0 */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    crc_finish            : 1   ; /* [0]  */
        hi_u32    cas_jtag_deob_finish  : 1   ; /* [1]  */
        hi_u32    chip_jtag_kdf_finish  : 1   ; /* [2]  */
        hi_u32    soc_jtag_kdf_finish   : 1   ; /* [3]  */
        hi_u32    cas_tpp_jtag_kdf_finish : 1   ; /* [4]  */
        hi_u32    cas_uart_jtag_kdf_finish : 1   ; /* [5]  */
        hi_u32    reserved_0            : 10  ; /* [15..6]  */
        hi_u32    hisi_rk_deob_finish   : 1   ; /* [16]  */
        hi_u32    hisi_tpp_jtag_kdf_finish : 1   ; /* [17]  */
        hi_u32    hpp_jtag_kdf_finish  : 1   ; /* [18]  */
        hi_u32    hisi_tee_priv_jtag_finish : 1   ; /* [19]  */
        hi_u32    reserved_1            : 12  ; /* [31..20]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_debug_info_0;

/* define the union u_rkp_debug_info_1 */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    vmask_busy_flag       : 1   ; /* [0]  */
        hi_u32    deob_busy_flag        : 1   ; /* [1]  */
        hi_u32    inte_busy_flag        : 1   ; /* [2]  */
        hi_u32    crc_busy_flag         : 1   ; /* [3]  */
        hi_u32    sck_busy_flag         : 1   ; /* [4]  */
        hi_u32    sckv_busy_flag        : 1   ; /* [5]  */
        hi_u32    seedv_busy_flag       : 1   ; /* [6]  */
        hi_u32    modkv_busy_flag       : 1   ; /* [7]  */
        hi_u32    eff_rk_busy_flag      : 1   ; /* [8]  */
        hi_u32    fixed_rk_busy_flag    : 1   ; /* [9]  */
        hi_u32    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_debug_info_1;

/* define the union u_rkp_debug_info_3 */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    rkp_init_finish       : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_debug_info_3;

/* define the union u_rkp_inte_info */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    reserved_0            : 8   ; /* [7..0]  */
        hi_u32    slot0_inte_once_vld   : 1   ; /* [8]  */
        hi_u32    slot1_inte_once_vld   : 1   ; /* [9]  */
        hi_u32    slot2_inte_once_vld   : 1   ; /* [10]  */
        hi_u32    slot3_inte_once_vld   : 1   ; /* [11]  */
        hi_u32    slot4_inte_once_vld   : 1   ; /* [12]  */
        hi_u32    slot5_inte_once_vld   : 1   ; /* [13]  */
        hi_u32    slot6_inte_once_vld   : 1   ; /* [14]  */
        hi_u32    slot7_inte_once_vld   : 1   ; /* [15]  */
        hi_u32    boot_inte_once_vld    : 1   ; /* [16]  */
        hi_u32    hisi_inte_once_vld    : 1   ; /* [17]  */
        hi_u32    stbm_inte_once_vld    : 1   ; /* [18]  */
        hi_u32    slot0_deob_inte_once_vld : 1   ; /* [19]  */
        hi_u32    slot1_deob_inte_once_vld : 1   ; /* [20]  */
        hi_u32    slot2_deob_inte_once_vld : 1   ; /* [21]  */
        hi_u32    slot3_deob_inte_once_vld : 1   ; /* [22]  */
        hi_u32    slot4_deob_inte_once_vld : 1   ; /* [23]  */
        hi_u32    slot5_deob_inte_once_vld : 1   ; /* [24]  */
        hi_u32    slot6_deob_inte_once_vld : 1   ; /* [25]  */
        hi_u32    slot7_deob_inte_once_vld : 1   ; /* [26]  */
        hi_u32    boot_deob_inte_once_vld : 1   ; /* [27]  */
        hi_u32    hisi_deob_inte_once_vld : 1   ; /* [28]  */
        hi_u32    reserved_1            : 3   ; /* [31..29]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_inte_info;

/* define the union u_rkp_crc_info */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    reserved_0            : 16  ; /* [15..0]  */
        hi_u32    boot_crc_once         : 1   ; /* [16]  */
        hi_u32    stbm_crc_once         : 1   ; /* [17]  */
        hi_u32    reserved_1            : 14  ; /* [31..18]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_crc_info;

/* define the union u_rkp_otp_rk_vld */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    slot0_vld             : 1   ; /* [0]  */
        hi_u32    slot1_vld             : 1   ; /* [1]  */
        hi_u32    slot2_vld             : 1   ; /* [2]  */
        hi_u32    slot3_vld             : 1   ; /* [3]  */
        hi_u32    slot4_vld             : 1   ; /* [4]  */
        hi_u32    slot5_vld             : 1   ; /* [5]  */
        hi_u32    slot6_vld             : 1   ; /* [6]  */
        hi_u32    slot7_vld             : 1   ; /* [7]  */
        hi_u32    boot_vld              : 1   ; /* [8]  */
        hi_u32    hisi_vld              : 1   ; /* [9]  */
        hi_u32    stbm_vld              : 1   ; /* [10]  */
        hi_u32    reserved_0            : 21  ; /* [31..11]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_otp_rk_vld;

/* define the union u_rkp_robust_alarm */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    rkp_import_signal_alarm : 1   ; /* [0]  */
        hi_u32    rkp_crc_alarm         : 1   ; /* [1]  */
        hi_u32    rkp_fsm_alarm         : 1   ; /* [2]  */
        hi_u32    rkp_algo_alarm        : 1   ; /* [3]  */
        hi_u32    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_robust_alarm;

/* define the union u_rkp_slotx_crc_rst */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    slotx_crc_ok          : 1   ; /* [0]  */
        hi_u32    slotx_rk_all_zero     : 1   ; /* [1]  */
        hi_u32    slotx_rk_all_one      : 1   ; /* [2]  */
        hi_u32    slotx_owner_id_all_zero : 1   ; /* [3]  */
        hi_u32    slotx_owner_id_all_one : 1   ; /* [4]  */
        hi_u32    slotx_owner_id_correct : 1   ; /* [5]  */
        hi_u32    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_slotx_crc_rst;

/* define the union u_rkp_slot0_crc_rst */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    slot0_crc_ok          : 1   ; /* [0]  */
        hi_u32    slot0_rk_all_zero     : 1   ; /* [1]  */
        hi_u32    slot0_rk_all_one      : 1   ; /* [2]  */
        hi_u32    slot0_owner_id_all_zero : 1   ; /* [3]  */
        hi_u32    slot0_owner_id_all_one : 1   ; /* [4]  */
        hi_u32    slot0_owner_id_correct : 1   ; /* [5]  */
        hi_u32    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_slot0_crc_rst;

/* define the union u_rkp_slot1_crc_rst */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    slot1_crc_ok          : 1   ; /* [0]  */
        hi_u32    slot1_rk_all_zero     : 1   ; /* [1]  */
        hi_u32    slot1_rk_all_one      : 1   ; /* [2]  */
        hi_u32    slot1_owner_id_all_zero : 1   ; /* [3]  */
        hi_u32    slot1_owner_id_all_one : 1   ; /* [4]  */
        hi_u32    slot1_owner_id_correct : 1   ; /* [5]  */
        hi_u32    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_slot1_crc_rst;

/* define the union u_rkp_slot2_crc_rst */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    slot2_crc_ok          : 1   ; /* [0]  */
        hi_u32    slot2_rk_all_zero     : 1   ; /* [1]  */
        hi_u32    slot2_rk_all_one      : 1   ; /* [2]  */
        hi_u32    slot2_owner_id_all_zero : 1   ; /* [3]  */
        hi_u32    slot2_owner_id_all_one : 1   ; /* [4]  */
        hi_u32    slot2_owner_id_correct : 1   ; /* [5]  */
        hi_u32    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_slot2_crc_rst;

/* define the union u_rkp_slot3_crc_rst */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    slot3_crc_ok          : 1   ; /* [0]  */
        hi_u32    slot3_rk_all_zero     : 1   ; /* [1]  */
        hi_u32    slot3_rk_all_one      : 1   ; /* [2]  */
        hi_u32    slot3_owner_id_all_zero : 1   ; /* [3]  */
        hi_u32    slot3_owner_id_all_one : 1   ; /* [4]  */
        hi_u32    slot3_owner_id_correct : 1   ; /* [5]  */
        hi_u32    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_slot3_crc_rst;

/* define the union u_rkp_slot4_crc_rst */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    slot4_crc_ok          : 1   ; /* [0]  */
        hi_u32    slot4_rk_all_zero     : 1   ; /* [1]  */
        hi_u32    slot4_rk_all_one      : 1   ; /* [2]  */
        hi_u32    slot4_owner_id_all_zero : 1   ; /* [3]  */
        hi_u32    slot4_owner_id_all_one : 1   ; /* [4]  */
        hi_u32    slot4_owner_id_correct : 1   ; /* [5]  */
        hi_u32    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_slot4_crc_rst;

/* define the union u_rkp_slot5_crc_rst */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    slot5_crc_ok          : 1   ; /* [0]  */
        hi_u32    slot5_rk_all_zero     : 1   ; /* [1]  */
        hi_u32    slot5_rk_all_one      : 1   ; /* [2]  */
        hi_u32    slot5_owner_id_all_zero : 1   ; /* [3]  */
        hi_u32    slot5_owner_id_all_one : 1   ; /* [4]  */
        hi_u32    slot5_owner_id_correct : 1   ; /* [5]  */
        hi_u32    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_slot5_crc_rst;

/* define the union u_rkp_slot6_crc_rst */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    slot6_crc_ok          : 1   ; /* [0]  */
        hi_u32    slot6_rk_all_zero     : 1   ; /* [1]  */
        hi_u32    slot6_rk_all_one      : 1   ; /* [2]  */
        hi_u32    slot6_owner_id_all_zero : 1   ; /* [3]  */
        hi_u32    slot6_owner_id_all_one : 1   ; /* [4]  */
        hi_u32    slot6_owner_id_correct : 1   ; /* [5]  */
        hi_u32    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_slot6_crc_rst;

/* define the union u_rkp_slot7_crc_rst */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    slot7_crc_ok          : 1   ; /* [0]  */
        hi_u32    slot7_rk_all_zero     : 1   ; /* [1]  */
        hi_u32    slot7_rk_all_one      : 1   ; /* [2]  */
        hi_u32    slot7_owner_id_all_zero : 1   ; /* [3]  */
        hi_u32    slot7_owner_id_all_one : 1   ; /* [4]  */
        hi_u32    slot7_owner_id_correct : 1   ; /* [5]  */
        hi_u32    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_slot7_crc_rst;

/* define the union u_rkp_boot_crc_rst */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    boot_crc_ok           : 1   ; /* [0]  */
        hi_u32    boot_rk_all_zero      : 1   ; /* [1]  */
        hi_u32    boot_rk_all_one       : 1   ; /* [2]  */
        hi_u32    boot_owner_id_all_zero : 1   ; /* [3]  */
        hi_u32    boot_owner_id_all_one : 1   ; /* [4]  */
        hi_u32    boot_owner_id_correct : 1   ; /* [5]  */
        hi_u32    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_boot_crc_rst;

/* define the union u_rkp_hisi_crc_rst */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    hisi_crc_ok           : 1   ; /* [0]  */
        hi_u32    hisi_rk_all_zero      : 1   ; /* [1]  */
        hi_u32    hisi_rk_all_one       : 1   ; /* [2]  */
        hi_u32    reserved_0            : 29  ; /* [31..3]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_hisi_crc_rst;

/* define the union u_rkp_oem_crc_rst */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    stbm_crc_ok           : 1   ; /* [0]  */
        hi_u32    stbm_rk_all_zero      : 1   ; /* [1]  */
        hi_u32    stbm_rk_all_one       : 1   ; /* [2]  */
        hi_u32    reserved_0            : 29  ; /* [31..3]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_stbm_crc_rst;

/* define the union u_rkp_calc_crc_0 */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    calc_vmask_crc        : 8   ; /* [7..0]  */
        hi_u32    calc_clear_rk_crc     : 8   ; /* [15..8]  */
        hi_u32    calc_fixed_rk_crc     : 8   ; /* [23..16]  */
        hi_u32    calc_sck_crc          : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_calc_crc_0;

/* define the union u_rkp_calc_crc_1 */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    calc_sckv_crc         : 8   ; /* [7..0]  */
        hi_u32    calc_seedv_crc        : 8   ; /* [15..8]  */
        hi_u32    calc_modk_crc         : 8   ; /* [23..16]  */
        hi_u32    calc_eff_rk_crc       : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_calc_crc_1;

/* define the union u_rkp_low_power */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    rkp_low_power_enable  : 4   ; /* [3..0]  */
        hi_u32    rkp_low_power_enable_lock : 1   ; /* [4]  */
        hi_u32    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_low_power;

/* define the union u_rkp_use_status */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    rkp_cur_right         : 8   ; /* [7..0]  */
        hi_u32    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_use_status;

/* define the union u_rkp_calc_start_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tpp_calc_start      : 1   ; /* [0]  */
        hi_u32    reserved_0            : 3   ; /* [3..1]  */
        hi_u32    tpp_command_type    : 4   ; /* [7..4]  */
        hi_u32    reserved_1            : 8   ; /* [15..8]  */
        hi_u32    deob_type             : 2   ; /* [17..16]  */
        hi_u32    reserved_2            : 14  ; /* [31..18]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_calc_start_tpp;

/* define the union u_rkp_slot_choose_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tpp_slot_choose     : 4   ; /* [3..0]  */
        hi_u32    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_slot_choose_tpp;

/* define the union u_rkp_klad_choose_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tpp_klad_type_sel   : 1   ; /* [0]  */
        hi_u32    reserved_0            : 3   ; /* [3..1]  */
        hi_u32    tpp_klad_sel        : 4   ; /* [7..4]  */
        hi_u32    reserved_1            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_klad_choose_tpp;

/* define the union u_rkp_module_id_0_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    reserved_0            : 1   ; /* [0]  */
        hi_u32    reserved_1            : 1   ; /* [1]  */
        hi_u32    tpp_decrypt         : 1   ; /* [2]  */
        hi_u32    tpp_encrypt         : 1   ; /* [3]  */
        hi_u32    tpp_content_key_scipher : 1   ; /* [4]  */
        hi_u32    tpp_content_key_mcipher : 1   ; /* [5]  */
        hi_u32    reserved_2            : 1   ; /* [6]  */
        hi_u32    tpp_content_key_tscipher : 1   ; /* [7]  */
        hi_u32    tpp_destination_sm4 : 1   ; /* [8]  */
        hi_u32    tpp_destination_tdes : 1   ; /* [9]  */
        hi_u32    tpp_destination_aes : 1   ; /* [10]  */
        hi_u32    tpp_destination_csa3 : 1   ; /* [11]  */
        hi_u32    tpp_destination_csa2 : 1   ; /* [12]  */
        hi_u32    tpp_destination_multi2 : 1   ; /* [13]  */
        hi_u32    tpp_destination_sm3_hmac : 1   ; /* [14]  */
        hi_u32    tpp_destination_sha2_hmac : 1   ; /* [15]  */
        hi_u32    reserved_3            : 8   ; /* [23..16]  */
        hi_u32    tpp_level_up        : 1   ; /* [24]  */
        hi_u32    tpp_stage           : 3   ; /* [27..25]  */
        hi_u32    reserved_4            : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_module_id_0_tpp;

/* define the union u_rkp_module_id_1_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tpp_klad_aes        : 1   ; /* [0]  */
        hi_u32    tpp_klad_tdes       : 1   ; /* [1]  */
        hi_u32    tpp_klad_sm4        : 1   ; /* [2]  */
        hi_u32    reserved_0            : 5   ; /* [7..3]  */
        hi_u32    tpp_klad_hkl        : 1   ; /* [8]  */
        hi_u32    tpp_klad_skl        : 1   ; /* [9]  */
        hi_u32    reserved_1            : 6   ; /* [15..10]  */
        hi_u32    tpp_no_restriction  : 8   ; /* [23..16]  */
        hi_u32    tpp_tpp_hpp_access  : 4   ; /* [27..24]  */
        hi_u32    tpp_tpp_access      : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_module_id_1_tpp;

/* define the union u_rkp_module_id_2_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    reserved_0            : 16  ; /* [15..0]  */
        hi_u32    tpp_tee_tpp_hpp_access : 8   ; /* [23..16]  */
        hi_u32    tpp_remap           : 1   ; /* [24]  */
        hi_u32    tpp_flash_prot_en   : 1   ; /* [25]  */
        hi_u32    tpp_allowed_nonce   : 1   ; /* [26]  */
        hi_u32    tpp_c2_checksum_en  : 1   ; /* [27]  */
        hi_u32    tpp_cm_checksum_en  : 1   ; /* [28]  */
        hi_u32    tpp_hdcp_rk         : 1   ; /* [29]  */
        hi_u32    reserved_1            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_module_id_2_tpp;

/* define the union u_rkp_unique_type_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tpp_unique_type     : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_unique_type_tpp;

/* define the union u_rkp_fixed_rk_data_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tpp_fixed_rk_data   : 16  ; /* [15..0]  */
        hi_u32    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_fixed_rk_data_tpp;

/* define the union u_rkp_fixed_rk_key_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tpp_fixed_rk_key    : 16  ; /* [15..0]  */
        hi_u32    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_fixed_rk_key_tpp;

/* define the union u_rkp_fixed_rk_key_sel_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tpp_fixed_rk_key_sel : 2   ; /* [1..0]  */
        hi_u32    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_fixed_rk_key_sel_tpp;

/* define the union u_rkp_int_status_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tpp_rkp_int         : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_int_status_tpp;

/* define the union u_rkp_raw_int_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tpp_rkp_raw_int     : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_raw_int_tpp;

/* define the union u_rkp_int_enable_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tpp_int_enable      : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_int_enable_tpp;

typedef union {
    /* define the struct bits */
    struct {
        hi_u32    cpux_error_type_0    : 1   ; /* [0]  */
        hi_u32    cpux_error_type_1    : 1   ; /* [1]  */
        hi_u32    cpux_error_type_2    : 1   ; /* [2]  */
        hi_u32    cpux_error_type_3    : 1   ; /* [3]  */
        hi_u32    cpux_error_type_4    : 1   ; /* [4]  */
        hi_u32    cpux_error_type_5    : 1   ; /* [5]  */
        hi_u32    cpux_error_type_6    : 1   ; /* [6]  */
        hi_u32    cpux_error_type_7    : 1   ; /* [7]  */
        hi_u32    cpux_error_type_8    : 1   ; /* [8]  */
        hi_u32    cpux_error_type_9    : 1   ; /* [9]  */
        hi_u32    cpux_error_type_10   : 1   ; /* [10]  */
        hi_u32    cpux_error_type_11   : 1   ; /* [11]  */
        hi_u32    cpux_error_type_12   : 1   ; /* [12]  */
        hi_u32    cpux_error_type_13   : 1   ; /* [13]  */
        hi_u32    cpux_error_type_14   : 1   ; /* [14]  */
        hi_u32    cpux_error_type_15   : 1   ; /* [15]  */
        hi_u32    cpux_error_type_16   : 1   ; /* [16]  */
        hi_u32    cpux_error_type_17   : 1   ; /* [17]  */
        hi_u32    cpux_error_type_18   : 1   ; /* [18]  */
        hi_u32    cpux_error_type_19   : 1   ; /* [19]  */
        hi_u32    cpux_error_type_20   : 1   ; /* [20]  */
        hi_u32    cpux_error_type_21   : 1   ; /* [21]  */
        hi_u32    reserved_0           : 2   ; /* [23..22]  */
        hi_u32    cpux_error_type_24   : 1   ; /* [24]  */
        hi_u32    cpux_error_type_25   : 1   ; /* [25]  */
        hi_u32    cpux_error_type_26   : 1   ; /* [26]  */
        hi_u32    reserved_1           : 5   ; /* [31..27]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_error_status_cpux;

/* define the union u_rkp_error_status_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tpp_error_type_0    : 1   ; /* [0]  */
        hi_u32    tpp_error_type_1    : 1   ; /* [1]  */
        hi_u32    tpp_error_type_2    : 1   ; /* [2]  */
        hi_u32    tpp_error_type_3    : 1   ; /* [3]  */
        hi_u32    tpp_error_type_4    : 1   ; /* [4]  */
        hi_u32    tpp_error_type_5    : 1   ; /* [5]  */
        hi_u32    tpp_error_type_6    : 1   ; /* [6]  */
        hi_u32    tpp_error_type_7    : 1   ; /* [7]  */
        hi_u32    tpp_error_type_8    : 1   ; /* [8]  */
        hi_u32    tpp_error_type_9    : 1   ; /* [9]  */
        hi_u32    tpp_error_type_10   : 1   ; /* [10]  */
        hi_u32    tpp_error_type_11   : 1   ; /* [11]  */
        hi_u32    tpp_error_type_12   : 1   ; /* [12]  */
        hi_u32    tpp_error_type_13   : 1   ; /* [13]  */
        hi_u32    tpp_error_type_14   : 1   ; /* [14]  */
        hi_u32    tpp_error_type_15   : 1   ; /* [15]  */
        hi_u32    tpp_error_type_16   : 1   ; /* [16]  */
        hi_u32    tpp_error_type_17   : 1   ; /* [17]  */
        hi_u32    tpp_error_type_18   : 1   ; /* [18]  */
        hi_u32    tpp_error_type_19   : 1   ; /* [19]  */
        hi_u32    tpp_error_type_20   : 1   ; /* [20]  */
        hi_u32    tpp_error_type_21   : 1   ; /* [21]  */
        hi_u32    tpp_error_type_23_22 : 2   ; /* [23..22]  */
        hi_u32    tpp_error_type_24   : 1   ; /* [24]  */
        hi_u32    tpp_error_type_25   : 1   ; /* [25]  */
        hi_u32    tpp_error_type_26   : 1   ; /* [26]  */
        hi_u32    tpp_error_type_31_27 : 5   ; /* [31..27]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_error_status_tpp;

/* define the union u_rkp_module_id_disable_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tpp_tpp_hpp_disable : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_module_id_disable_tpp;

/* define the union rkp_cas_kdf_static_disable_tpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tpp_cas_kdf_static_choose : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_cas_kdf_static_disable_tpp;

/* define the union u_rkp_calc_start_tee */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tee_calc_start        : 1   ; /* [0]  */
        hi_u32    reserved_0            : 3   ; /* [3..1]  */
        hi_u32    tee_command_type      : 4   ; /* [7..4]  */
        hi_u32    reserved_1            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_calc_start_tee;

/* define the union u_rkp_slot_choose_tee */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tee_slot_choose       : 4   ; /* [3..0]  */
        hi_u32    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_slot_choose_tee;

/* define the union u_rkp_klad_choose_tee */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    reserved_0            : 4   ; /* [3..0]  */
        hi_u32    tee_klad_sel          : 4   ; /* [7..4]  */
        hi_u32    reserved_1            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_klad_choose_tee;

/* define the union u_rkp_module_id_0_tee */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    reserved_0            : 1   ; /* [0]  */
        hi_u32    reserved_1            : 1   ; /* [1]  */
        hi_u32    tee_decrypt           : 1   ; /* [2]  */
        hi_u32    tee_encrypt           : 1   ; /* [3]  */
        hi_u32    reserved_2            : 1   ; /* [4]  */
        hi_u32    tee_content_key_mcipher : 1   ; /* [5]  */
        hi_u32    reserved_3            : 1   ; /* [6]  */
        hi_u32    tee_content_key_tscipher : 1   ; /* [7]  */
        hi_u32    tee_destination_sm4   : 1   ; /* [8]  */
        hi_u32    tee_destination_tdes  : 1   ; /* [9]  */
        hi_u32    tee_destination_aes   : 1   ; /* [10]  */
        hi_u32    tee_destination_csa3  : 1   ; /* [11]  */
        hi_u32    tee_destination_csa2  : 1   ; /* [12]  */
        hi_u32    tee_destination_multi2 : 1   ; /* [13]  */
        hi_u32    tee_destination_sm3_hmac : 1   ; /* [14]  */
        hi_u32    tee_destination_sha2_hmac : 1   ; /* [15]  */
        hi_u32    reserved_4            : 8   ; /* [23..16]  */
        hi_u32    tee_level_up          : 1   ; /* [24]  */
        hi_u32    tee_stage             : 3   ; /* [27..25]  */
        hi_u32    reserved_5            : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_module_id_0_tee;

/* define the union u_rkp_module_id_1_tee */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tee_klad_aes          : 1   ; /* [0]  */
        hi_u32    tee_klad_tdes         : 1   ; /* [1]  */
        hi_u32    tee_klad_sm4          : 1   ; /* [2]  */
        hi_u32    reserved_0            : 5   ; /* [7..3]  */
        hi_u32    tee_klad_hkl          : 1   ; /* [8]  */
        hi_u32    reserved_1            : 7   ; /* [15..9]  */
        hi_u32    tee_no_restriction    : 8   ; /* [23..16]  */
        hi_u32    reserved_2            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_module_id_1_tee;

/* define the union u_rkp_module_id_2_tee */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tee_tee_only          : 8   ; /* [7..0]  */
        hi_u32    reserved_0            : 8   ; /* [15..8]  */
        hi_u32    tee_tee_tpp_hpp_access : 8   ; /* [23..16]  */
        hi_u32    tee_remap             : 1   ; /* [24]  */
        hi_u32    tee_flash_prot_en     : 1   ; /* [25]  */
        hi_u32    tee_allowed_nonce     : 1   ; /* [26]  */
        hi_u32    tee_c2_checksum_en    : 1   ; /* [27]  */
        hi_u32    tee_cm_checksum_en    : 1   ; /* [28]  */
        hi_u32    tee_hdcp_rk           : 1   ; /* [29]  */
        hi_u32    reserved_1            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_module_id_2_tee;

/* define the union u_rkp_unique_type_tee */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tee_unique_type       : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_unique_type_tee;

/* define the union u_rkp_fixed_rk_data_tee */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tee_fixed_rk_data     : 16  ; /* [15..0]  */
        hi_u32    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_fixed_rk_data_tee;

/* define the union u_rkp_fixed_rk_key_tee */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tee_fixed_rk_key      : 12  ; /* [11..0]  */
        hi_u32    reserved_0            : 20  ; /* [31..12]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_fixed_rk_key_tee;

/* define the union u_rkp_fixed_rk_key_sel_tee */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tee_fixed_rk_key_sel  : 2   ; /* [1..0]  */
        hi_u32    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_fixed_rk_key_sel_tee;

/* define the union u_rkp_int_status_tee */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tee_rkp_int           : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_int_status_tee;

/* define the union u_rkp_raw_int_tee */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tee_rkp_raw_int       : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_raw_int_tee;

/* define the union u_rkp_int_enable_tee */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tee_int_enable        : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_int_enable_tee;

/* define the union u_rkp_error_status_tee */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tee_error_type_0      : 1   ; /* [0]  */
        hi_u32    tee_error_type_1      : 1   ; /* [1]  */
        hi_u32    tee_error_type_2      : 1   ; /* [2]  */
        hi_u32    tee_error_type_3      : 1   ; /* [3]  */
        hi_u32    tee_error_type_4      : 1   ; /* [4]  */
        hi_u32    tee_error_type_5      : 1   ; /* [5]  */
        hi_u32    tee_error_type_6      : 1   ; /* [6]  */
        hi_u32    tee_error_type_7      : 1   ; /* [7]  */
        hi_u32    tee_error_type_8      : 1   ; /* [8]  */
        hi_u32    tee_error_type_9      : 1   ; /* [9]  */
        hi_u32    tee_error_type_10     : 1   ; /* [10]  */
        hi_u32    tee_error_type_11     : 1   ; /* [11]  */
        hi_u32    tee_error_type_12     : 1   ; /* [12]  */
        hi_u32    tee_error_type_13     : 1   ; /* [13]  */
        hi_u32    tee_error_type_14     : 1   ; /* [14]  */
        hi_u32    tee_error_type_15     : 1   ; /* [15]  */
        hi_u32    tee_error_type_16     : 1   ; /* [16]  */
        hi_u32    tee_error_type_17     : 1   ; /* [17]  */
        hi_u32    tee_error_type_18     : 1   ; /* [18]  */
        hi_u32    tee_error_type_19     : 1   ; /* [19]  */
        hi_u32    tee_error_type_20     : 1   ; /* [20]  */
        hi_u32    tee_error_type_21     : 1   ; /* [21]  */
        hi_u32    tee_error_type_31_22  : 10  ; /* [31..22]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_error_status_tee;


/* define the union rkp_cas_kdf_static_disable_tee */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    tee_cas_kdf_static_choose : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_cas_kdf_static_disable_tee;

/* define the union u_rkp_calc_start_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    hpp_calc_start       : 1   ; /* [0]  */
        hi_u32    reserved_0            : 3   ; /* [3..1]  */
        hi_u32    hpp_command_type     : 4   ; /* [7..4]  */
        hi_u32    reserved_1            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_calc_start_hpp;

/* define the union u_rkp_slot_choose_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    hpp_slot_choose      : 4   ; /* [3..0]  */
        hi_u32    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_slot_choose_hpp;

/* define the union u_rkp_klad_choose_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    reserved_0            : 4   ; /* [3..0]  */
        hi_u32    hpp_klad_sel         : 4   ; /* [7..4]  */
        hi_u32    reserved_1            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_klad_choose_hpp;

/* define the union u_rkp_module_id_0_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    reserved_0            : 1   ; /* [0]  */
        hi_u32    reserved_1            : 1   ; /* [1]  */
        hi_u32    hpp_decrypt          : 1   ; /* [2]  */
        hi_u32    hpp_encrypt          : 1   ; /* [3]  */
        hi_u32    reserved_2            : 1   ; /* [4]  */
        hi_u32    hpp_content_key_mcipher : 1   ; /* [5]  */
        hi_u32    reserved_3            : 1   ; /* [6]  */
        hi_u32    hpp_content_key_tscipher : 1   ; /* [7]  */
        hi_u32    hpp_destination_sm4  : 1   ; /* [8]  */
        hi_u32    hpp_destination_tdes : 1   ; /* [9]  */
        hi_u32    hpp_destination_aes  : 1   ; /* [10]  */
        hi_u32    hpp_destination_csa3 : 1   ; /* [11]  */
        hi_u32    hpp_destination_csa2 : 1   ; /* [12]  */
        hi_u32    hpp_destination_multi2 : 1   ; /* [13]  */
        hi_u32    hpp_destination_sm3_hmac : 1   ; /* [14]  */
        hi_u32    hpp_destination_sha2_hmac : 1   ; /* [15]  */
        hi_u32    reserved_4            : 8   ; /* [23..16]  */
        hi_u32    hpp_level_up         : 1   ; /* [24]  */
        hi_u32    hpp_stage            : 3   ; /* [27..25]  */
        hi_u32    reserved_5            : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_module_id_0_hpp;

/* define the union u_rkp_module_id_1_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    hpp_klad_aes         : 1   ; /* [0]  */
        hi_u32    hpp_klad_tdes        : 1   ; /* [1]  */
        hi_u32    hpp_klad_sm4         : 1   ; /* [2]  */
        hi_u32    reserved_0            : 5   ; /* [7..3]  */
        hi_u32    hpp_klad_hkl         : 1   ; /* [8]  */
        hi_u32    reserved_1            : 7   ; /* [15..9]  */
        hi_u32    hpp_no_restriction   : 8   ; /* [23..16]  */
        hi_u32    hpp_tpp_hpp_access   : 4   ; /* [27..24]  */
        hi_u32    reserved_2            : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_module_id_1_hpp;

/* define the union u_rkp_module_id_2_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    reserved_0            : 8   ; /* [7..0]  */
        hi_u32    hpp_hpp_access       : 8   ; /* [15..8]  */
        hi_u32    hpp_tee_hpp_tpp_access : 8   ; /* [23..16]  */
        hi_u32    hpp_remap            : 1   ; /* [24]  */
        hi_u32    hpp_flash_prot_en    : 1   ; /* [25]  */
        hi_u32    hpp_allowed_nonce    : 1   ; /* [26]  */
        hi_u32    hpp_c2_checksum_en   : 1   ; /* [27]  */
        hi_u32    hpp_cm_checksum_en   : 1   ; /* [28]  */
        hi_u32    hpp_hdcp_rk          : 1   ; /* [29]  */
        hi_u32    reserved_1            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_module_id_2_hpp;

/* define the union u_rkp_unique_type_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    hpp_unique_type      : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_unique_type_hpp;

/* define the union u_rkp_fixed_rk_data_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    hpp_fixed_rk_data    : 16  ; /* [15..0]  */
        hi_u32    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_fixed_rk_data_hpp;

/* define the union u_rkp_fixed_rk_key_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    hpp_fixed_rk_key     : 12  ; /* [11..0]  */
        hi_u32    reserved_0            : 20  ; /* [31..12]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_fixed_rk_key_hpp;

/* define the union u_rkp_fixed_rk_key_sel_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    hpp_fixed_rk_key_sel : 2   ; /* [1..0]  */
        hi_u32    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_fixed_rk_key_sel_hpp;

/* define the union u_rkp_int_status_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    hpp_rkp_int          : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_int_status_hpp;

/* define the union u_rkp_raw_int_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    hpp_rkp_raw_int      : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_raw_int_hpp;

/* define the union u_rkp_int_enable_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    hpp_int_enable       : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_int_enable_hpp;

/* define the union u_rkp_error_status_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    hpp_error_type_0     : 1   ; /* [0]  */
        hi_u32    hpp_error_type_1     : 1   ; /* [1]  */
        hi_u32    hpp_error_type_2     : 1   ; /* [2]  */
        hi_u32    hpp_error_type_3     : 1   ; /* [3]  */
        hi_u32    reserved_0            : 1   ; /* [4]  */
        hi_u32    hpp_error_type_5     : 1   ; /* [5]  */
        hi_u32    hpp_error_type_6     : 1   ; /* [6]  */
        hi_u32    hpp_error_type_7     : 1   ; /* [7]  */
        hi_u32    hpp_error_type_8     : 1   ; /* [8]  */
        hi_u32    reserved_1            : 1   ; /* [9]  */
        hi_u32    hpp_error_type_10    : 1   ; /* [10]  */
        hi_u32    hpp_error_type_11    : 1   ; /* [11]  */
        hi_u32    hpp_error_type_12    : 1   ; /* [12]  */
        hi_u32    hpp_error_type_13    : 1   ; /* [13]  */
        hi_u32    hpp_error_type_14    : 1   ; /* [14]  */
        hi_u32    hpp_error_type_15    : 1   ; /* [15]  */
        hi_u32    hpp_error_type_16    : 1   ; /* [16]  */
        hi_u32    hpp_error_type_17    : 1   ; /* [17]  */
        hi_u32    hpp_error_type_18    : 1   ; /* [18]  */
        hi_u32    hpp_error_type_19    : 1   ; /* [19]  */
        hi_u32    hpp_error_type_20    : 1   ; /* [20]  */
        hi_u32    hpp_error_type_21    : 1   ; /* [21]  */
        hi_u32    hpp_error_type_25_22 : 4   ; /* [25..22]  */
        hi_u32    hpp_error_type_26    : 1   ; /* [26]  */
        hi_u32    hpp_error_type_31_22 : 5   ; /* [31..27]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_error_status_hpp;

/* define the union u_rkp_module_id_disable_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    hpp_tpp_hpp_disable  : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_module_id_disable_hpp;

/* define the union rkp_cas_kdf_static_disable_hpp */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    hpp_cas_kdf_static_choose : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_cas_kdf_static_disable_hpp;

/* define the union u_rkp_calc_start_ree */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    ree_calc_start        : 1   ; /* [0]  */
        hi_u32    reserved_0            : 3   ; /* [3..1]  */
        hi_u32    ree_command_type      : 4   ; /* [7..4]  */
        hi_u32    reserved_1            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_calc_start_ree;

/* define the union u_rkp_slot_choose_ree */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    ree_slot_choose       : 4   ; /* [3..0]  */
        hi_u32    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_slot_choose_ree;

/* define the union u_rkp_klad_choose_ree */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    reserved_0            : 4   ; /* [3..0]  */
        hi_u32    ree_klad_sel          : 4   ; /* [7..4]  */
        hi_u32    reserved_1            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_klad_choose_ree;

/* define the union u_rkp_module_id_0_ree */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    reserved_0            : 2   ; /* [1..0]  */
        hi_u32    ree_decrypt           : 1   ; /* [2]  */
        hi_u32    ree_encrypt           : 1   ; /* [3]  */
        hi_u32    reserved_1            : 1   ; /* [4]  */
        hi_u32    ree_content_key_mcipher : 1   ; /* [5]  */
        hi_u32    reserved_2            : 1   ; /* [6]  */
        hi_u32    ree_content_key_tscipher : 1   ; /* [7]  */
        hi_u32    ree_destination_sm4   : 1   ; /* [8]  */
        hi_u32    ree_destination_tdes  : 1   ; /* [9]  */
        hi_u32    ree_destination_aes   : 1   ; /* [10]  */
        hi_u32    ree_destination_csa3  : 1   ; /* [11]  */
        hi_u32    ree_destination_csa2  : 1   ; /* [12]  */
        hi_u32    ree_destination_multi2 : 1   ; /* [13]  */
        hi_u32    ree_destination_sm3_hmac : 1   ; /* [14]  */
        hi_u32    ree_destination_sha2_hmac : 1   ; /* [15]  */
        hi_u32    reserved_3            : 8   ; /* [23..16]  */
        hi_u32    ree_level_up          : 1   ; /* [24]  */
        hi_u32    ree_stage             : 3   ; /* [27..25]  */
        hi_u32    reserved_4            : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_module_id_0_ree;

/* define the union u_rkp_module_id_1_ree */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    ree_klad_aes          : 1   ; /* [0]  */
        hi_u32    ree_klad_tdes         : 1   ; /* [1]  */
        hi_u32    ree_klad_sm4          : 1   ; /* [2]  */
        hi_u32    reserved_0            : 5   ; /* [7..3]  */
        hi_u32    ree_klad_hkl          : 1   ; /* [8]  */
        hi_u32    reserved_1            : 7   ; /* [15..9]  */
        hi_u32    ree_no_restriction    : 8   ; /* [23..16]  */
        hi_u32    reserved_2            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_module_id_1_ree;

/* define the union u_rkp_module_id_2_ree */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    reserved_0            : 24  ; /* [23..0]  */
        hi_u32    ree_remap             : 1   ; /* [24]  */
        hi_u32    ree_flash_prot_en     : 1   ; /* [25]  */
        hi_u32    ree_allowed_nonce     : 1   ; /* [26]  */
        hi_u32    ree_c2_checksum_en    : 1   ; /* [27]  */
        hi_u32    ree_cm_checksum_en    : 1   ; /* [28]  */
        hi_u32    ree_hdcp_rk           : 1   ; /* [29]  */
        hi_u32    reserved_1            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_module_id_2_ree;

/* define the union u_rkp_unique_type_ree */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    ree_unique_type       : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_unique_type_ree;

/* define the union u_rkp_fixed_rk_data_ree */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    ree_fixed_rk_data     : 16  ; /* [15..0]  */
        hi_u32    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_fixed_rk_data_ree;

/* define the union u_rkp_fixed_rk_key_ree */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    ree_fixed_rk_key      : 8   ; /* [7..0]  */
        hi_u32    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_fixed_rk_key_ree;

/* define the union u_rkp_fixed_rk_key_sel_ree */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    ree_fixed_rk_key_sel  : 2   ; /* [1..0]  */
        hi_u32    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_fixed_rk_key_sel_ree;

/* define the union u_rkp_int_status_ree */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    ree_rkp_int           : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_int_status_ree;

/* define the union u_rkp_raw_int_ree */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    ree_rkp_raw_int       : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_raw_int_ree;

/* define the union u_rkp_int_enable_ree */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    ree_int_enable        : 1   ; /* [0]  */
        hi_u32    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_int_enable_ree;

/* define the union u_rkp_error_status_ree */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    ree_error_type_0      : 1   ; /* [0]  */
        hi_u32    ree_error_type_1      : 1   ; /* [1]  */
        hi_u32    ree_error_type_2      : 1   ; /* [2]  */
        hi_u32    ree_error_type_3      : 1   ; /* [3]  */
        hi_u32    ree_error_type_4      : 1   ; /* [4]  */
        hi_u32    ree_error_type_5      : 1   ; /* [5]  */
        hi_u32    ree_error_type_6      : 1   ; /* [6]  */
        hi_u32    ree_error_type_7      : 1   ; /* [7]  */
        hi_u32    ree_error_type_8      : 1   ; /* [8]  */
        hi_u32    ree_error_type_9      : 1   ; /* [9]  */
        hi_u32    ree_error_type_10     : 1   ; /* [10]  */
        hi_u32    ree_error_type_11     : 1   ; /* [11]  */
        hi_u32    ree_error_type_12     : 1   ; /* [12]  */
        hi_u32    ree_error_type_13     : 1   ; /* [13]  */
        hi_u32    ree_error_type_14     : 1   ; /* [14]  */
        hi_u32    ree_error_type_15     : 1   ; /* [15]  */
        hi_u32    ree_error_type_16     : 1   ; /* [16]  */
        hi_u32    ree_error_type_17     : 1   ; /* [17]  */
        hi_u32    ree_error_type_18     : 1   ; /* [18]  */
        hi_u32    ree_error_type_19     : 1   ; /* [19]  */
        hi_u32    ree_error_type_20     : 1   ; /* [20]  */
        hi_u32    ree_error_type_21     : 1   ; /* [21]  */
        hi_u32    ree_error_type_31_22  : 10  ; /* [31..22]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_error_status_ree;

/* define the union u_rkp_module_id_0 */
typedef union {
    /* define an unsigned member */
    rkp_module_id_0_tpp tpp;
    rkp_module_id_0_tee tee;
    rkp_module_id_0_ree ree;
    rkp_module_id_0_hpp hpp;
    unsigned int u32;
} rkp_module_id_0;

/* define the union rkp_module_id_1 */
typedef union {
    /* define an unsigned member */
    rkp_module_id_1_tpp tpp;
    rkp_module_id_1_tee tee;
    rkp_module_id_1_ree ree;
    rkp_module_id_1_hpp hpp;
    unsigned int u32;
} rkp_module_id_1;


/* define the union rkp_module_id_2 */
typedef union {
    /* define an unsigned member */
    rkp_module_id_2_tpp tpp;
    rkp_module_id_2_tee tee;
    rkp_module_id_2_ree ree;
    rkp_module_id_2_hpp hpp;
    unsigned int u32;
} rkp_module_id_2;

typedef union {
    unsigned int u32;
} rkp_module_id_3;

/* define the union rkp_rk_info_s */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    ca_vendor_id             : 8  ; /* [7..0]  */
        unsigned int    csa2_check               : 1  ; /* [8]  */
        unsigned int    reserved_1               : 23 ; /* [31..9]  */
    } bits;
    /* define an unsigned member */
    unsigned int    u32;
} rkp_rk_info;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // _DRV_RKP_REG_H__

