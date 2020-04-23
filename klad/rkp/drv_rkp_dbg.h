/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
* Description :hpp rom keyladder.
* Author : Hisilicon security team
* Created : 2019-03-13
*/
#ifndef __DRV_RKP_DBG_H__
#define __DRV_RKP_DBG_H__

#include "drv_rkp_define.h"
#include "hal_rkp.h"
#include "drv_rkp_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    SIMU_CPU_HPP          = 0x00,
    SIMU_CPU_TPP          = 0x01,
    SIMU_CPU_TEE          = 0x02,
    SIMU_CPU_REE          = 0x03,
    SIMU_CPU_MAX
} rkp_cpu;

#define RKP_ERR_26_DEOB_BUT_DIRECT_SKL_0              (1<<26) /* only PANDA */
#define RKP_ERR_25_DEOB_TYPE_ILLEGAL                  (1<<25) /* only PANDA */
#define RKP_ERR_24_KLAD_TYPE_PANDA_BUT_OTP_DIS        (1<<24) /* only PANDA */
#define RKP_ERR_21_KLAD_SM4_EN_BUT_OTP_SM4_DIS        (1<<21)
#define RKP_ERR_20_KLAD_SEL_HKL_TA_BUT_CAVD_NOT2      (1<<20)
#define RKP_ERR_19_CRC_MORE_THAN_ONCE                 (1<<19)
#define RKP_ERR_18_DEOB_INTE_MORE_THAN_ONCE           (1<<18)
#define RKP_ERR_17_CRC_BUT_NOT_STBM_BOOT              (1<<17)
#define RKP_ERR_16_FIXED_TO_TSC_OR_NONCAS_SLOT_TO_TSC (1<<16)
#define RKP_ERR_15_NAGRA_FLASH_EN_BUT_CAVD_NOT1       (1<<15)
#define RKP_ERR_14_C2_CN_EN_BUT_CACD_NOT2             (1<<14)
#define RKP_ERR_13_HKL_5STAGE_OR_SKL_4STAGE_MORE      (1<<13)
#define RKP_ERR_12_INTE_MORE_THAN_ONCE                (1<<12)
#define RKP_ERR_11_SLOT_INVALD_BUT_NOT_CRC            (1<<11)
#define RKP_ERR_10_STBM_BUT_DEOB_OR_DEOB_INTE         (1<<10)
#define RKP_ERR_9_NAGRA_FLASH_DIS_BUT_OTP_DEACT       (1<<9) /* no SMCU */
#define RKP_ERR_8_NONCE_BUT_3STAGE_LESS               (1<<8)
#define RKP_ERR_7_DST_NOT_MATCH_TAG                   (1<<7)
#define RKP_ERR_6_KLAD_BUSY                           (1<<6)
#define RKP_ERR_5_HKL_NOT_LOCK                        (1<<5)
#define RKP_ERR_4_CHIP_ID_SEL_ILLEGAL                 (1<<4) /* no SMCU */
#define RKP_ERR_3_KLAD_SEL_ILLEGAL                    (1<<3)
#define RKP_ERR_2_SLOT_CHOOSE_ILLEGAL                 (1<<2)
#define RKP_ERR_1_COMMAND_TYPE_ILLEGAL                (1<<1)
#define RKP_ERR_0_FINISH_OR_ERR                       (1<<0)

void rkp_get_crc(rkp_calc_crc_0 *crc_0, rkp_calc_crc_1 *crc_1);
hi_s32 rkp_get_checksum(hi_rkp_slot_choose slot);
void rkp_dump_rk_vld(void);
void rkp_dump_rk_crc(hi_rkp_slot_choose slot);
void rkp_dump_err_status(void);
int rkp_set_cpu_type(rkp_cpu cpu);


typedef struct {
    hi_u8 vmask_crc;
    hi_u8 clear_rk_crc;
} rkp_deob_rst;

typedef struct {
    hi_u8 sck_crc;
    hi_u8 sckv_crc;
    hi_u8 seedv_crc;
    hi_u8 modv_crc;
    hi_u8 eff_rk_crc;
} rkp_kdf_rst;

typedef struct {
    rkp_deob_rst     deob_rst;
    rkp_kdf_rst      kdf_rst;
} rkp_deob_kdf_rst;

typedef struct {
    unsigned short fix_rk_crc;
} rkp_fix_rk_rst;

typedef struct {
    rkp_fix_rk_rst fix_rk_rst;
    rkp_kdf_rst      kdf_rst;
} rkp_fix_rk_kdf_rst;

int dbg_rkp_deob_crc(rkp_deob_rst *deob_rst);

int dbg_rkp_eff_rk_crc(rkp_deob_kdf_rst *deob_kdf_rst);

int dbg_rkp_fixed_rk_crc(rkp_fix_rk_kdf_rst *fix_rk_kdf_rst);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_RKP_DBG_H__

