/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
* Description :hpp rom keyladder.
* Author : Hisilicon security team
* Created : 2019-03-13
*/
#ifndef __HAL_RKP_H__
#define __HAL_RKP_H__

#include "drv_rkp_define.h"
#include "drv_rkp_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



typedef enum {
    HI_ALG_AES          = 0x00,
    HI_ALG_SM4          = 0x01,
    HI_ALG_TWEAKED_AES  = 0x02,
    HI_ALG_TDES         = 0x03,
    HI_ALG_MAX
} hi_alg_sel;

typedef enum {
    HI_RKP_COMMAND_TYPE_INTE                 = 0x0,
    HI_RKP_COMMAND_TYPE_DEOB_INTE            = 0x1,
    HI_RKP_COMMAND_TYPE_CRC                  = 0x2,
    HI_RKP_COMMAND_TYPE_EFF_RK_GEN           = 0x3,
    HI_RKP_COMMAND_TYPE_FIXED_RK_KDF         = 0x4,
    HI_RKP_COMMAND_TYPE_DEOB                 = 0x5, /* only for TPP/HPP */
    HI_RKP_COMMAND_TYPE_TEE_USR_JTAG         = 0x6, /* only for TEE */
    HI_RKP_COMMAND_TYPE_MAX
} hi_rkp_command_type;

/* Note: unchangerable order */
typedef enum {
    HI_RKP_SLOT_CHOOSE_SLOT0                 = 0x0,
    HI_RKP_SLOT_CHOOSE_SLOT1                 = 0x1,
    HI_RKP_SLOT_CHOOSE_SLOT2                 = 0x2,
    HI_RKP_SLOT_CHOOSE_SLOT3                 = 0x3,
    HI_RKP_SLOT_CHOOSE_SLOT4                 = 0x4,
    HI_RKP_SLOT_CHOOSE_SLOT5                 = 0x5,
    HI_RKP_SLOT_CHOOSE_SLOT6                 = 0x6,
    HI_RKP_SLOT_CHOOSE_SLOT7                 = 0x7,
    HI_RKP_SLOT_CHOOSE_BOOT                  = 0x8,
    HI_RKP_SLOT_CHOOSE_HISI                  = 0x9,
    HI_RKP_SLOT_CHOOSE_OEM                   = 0xa,
    HI_RKP_SLOT_CHOOSE_MAX
} hi_rkp_slot_choose;

typedef enum {
    HI_RKP_HISI_TEE_USR                      = 0x0,
    HI_RKP_HISI_TEE_RPIV                     = 0x1,
    HI_RKP_HISI_PANDA                        = 0x2,
    HI_RKP_HISI_SMCU                         = 0x3,
    HI_RKP_CAS_SOC_JTAG                      = 0x4,
    HI_RKP_CAS_CHIP_JTAG                     = 0x5,
    HI_RKP_CAS_PANDA                         = 0x6,
    HI_RKP_CAS_UART                          = 0x7,
} hi_rkp_sck_din_choose_e;

typedef enum {
    HI_RKP_KLAD_SEL_SKL_KLAD0                 = 0x0,
    HI_RKP_KLAD_SEL_SKL_KLAD1                 = 0x1,
    HI_RKP_KLAD_SEL_HKL_KLAD0                 = 0x0,
    HI_RKP_KLAD_SEL_HKL_KLAD1                 = 0x1,
    HI_RKP_KLAD_SEL_HKL_KLAD2                 = 0x2,
    HI_RKP_KLAD_SEL_HKL_KLAD3                 = 0x3,
    HI_RKP_KLAD_SEL_HKL_KLAD4                 = 0x4,
    HI_RKP_KLAD_SEL_HKL_KLAD5                 = 0x5,
    HI_RKP_KLAD_SEL_HKL_KLAD6                 = 0x6,
    HI_RKP_KLAD_SEL_HKL_KLAD7                 = 0x7,
    HI_RKP_KLAD_SEL_HKL_TA                    = 0x8,
    HI_RKP_KLAD_SEL_MAX
} hi_rkp_klad_sel;

typedef enum {
    HI_RKP_KLAD_TYPE_SEL_HKL                  = 0x0,
    HI_RKP_KLAD_TYPE_SEL_SKL                  = 0x1,
    HI_RKP_KLAD_TYPE_SEL_MAX
} hi_rkp_klad_type_sel;

typedef enum {
    HI_RKP_CAS_KDF_STATIC_SECURE              = 0x0,
    HI_RKP_CAS_KDF_STATIC_REE                 = 0x1,
    HI_RKP_CAS_KDF_STATIC_MAX
} hi_rkp_cas_kdf_static_sel;

hi_u32  rkp_read_reg(hi_u32 addr);
hi_u32 rkp_read_int_enable(void);
hi_u32 rkp_is_idle(void);
hi_s32 rkp_wait_idle(void);
hi_s32 rkp_check_error(void);
void rkp_write_reg(hi_u32 addr, hi_u32 value);
void rkp_slot_choose(hi_rkp_slot_choose slot);
void rkp_klad_sel(hi_rkp_klad_sel klad_sel, hi_rkp_klad_type_sel klad_type_sel);
void rkp_vendor_id(hi_u32 vendor_id);
hi_s32 rkp_module_id(hi_u32 module_id3, hi_u32 module_id2, hi_u32 module_id1, hi_u32 module_id0);
void rkp_unique_type(hi_u32 unique_type);
void rkp_fixed_rk_data(hi_u16 fixed_rk_data);
void rkp_fixed_rk_key(hi_u16 fixed_rk_key);
void rkp_fixed_rk_key_sel(hi_u16 fixed_rk_key_sel);
void rkp_calc_start(hi_rkp_command_type command_type);
void rkp_deob_calc_start(void);
void rkp_cas_kdf_static_disable(hi_rkp_cas_kdf_static_sel cas_kdf_static);
hi_s32 rkp_get_klad_level(hi_u32 module_id_0);

#ifdef HI_INT_SUPPORT
hi_void rkp_int_en(hi_void);
hi_void rkp_int_dis(hi_void);
hi_void rkp_int_clr(hi_void);
hi_u32 rkp_get_int_stat(hi_void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __HAL_RKP_H__

