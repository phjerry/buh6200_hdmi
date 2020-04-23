/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
* Description :hpp rom keyladder.
* Author : Hisilicon security team
* Created : 2019-03-13
*/

#include "drv_rkp_dbg.h"
#include "hal_rkp.h"
#include "drv_rkp_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


int rkp_set_cpu_type(rkp_cpu cpu)
{
    unused(cpu);
    return 0;
}

void rkp_get_crc(rkp_calc_crc_0 *crc_0, rkp_calc_crc_1 *crc_1)
{
    crc_0->u32 = rkp_read_reg(RKP_CALC_CRC_0);
    crc_1->u32 = rkp_read_reg(RKP_CALC_CRC_1);

    HI_INFO_KLAD("vmask_crc    : 0x%02x\n", crc_0->bits.calc_vmask_crc);
    HI_INFO_KLAD("clear_rk_crc : 0x%02x\n", crc_0->bits.calc_clear_rk_crc);
    HI_INFO_KLAD("fixed_rk_crc : 0x%02x\n", crc_0->bits.calc_fixed_rk_crc);
    HI_INFO_KLAD("sck_crc      : 0x%02x\n", crc_0->bits.calc_sck_crc);

    HI_INFO_KLAD("sckv_crc     : 0x%02x\n", crc_1->bits.calc_sckv_crc);
    HI_INFO_KLAD("seedv_crc    : 0x%02x\n", crc_1->bits.calc_seedv_crc);
    HI_INFO_KLAD("modk_crc     : 0x%02x\n", crc_1->bits.calc_modk_crc);
    HI_INFO_KLAD("eff_rk_crc   : 0x%02x\n", crc_1->bits.calc_eff_rk_crc);
    return ;
}

hi_s32 rkp_get_checksum(hi_rkp_slot_choose slot)
{
    return rkp_read_reg(RKP_SLOT0_CHECKNUM + slot * 0x4);
}

#define print_rkp_rk_vld(reg, value) \
    do { \
        HI_ERR_KLAD("%s:%-16s=%s\n", #reg, #value, (reg.bits.value != 0) ? "valid" : "invalid");\
    } while (0)
#define print_rkp_crc_rst(reg, value) \
    do { \
        HI_ERR_KLAD("%s:%-16s=%s\n", #reg, #value, (reg.bits.value != 0) ? "yes" : "no");\
    } while (0)

void rkp_dump_rk_vld(void)
{
    rkp_otp_rk_vld otp_rk_vld;

    otp_rk_vld.u32 = rkp_read_reg(RKP_OTP_RK_VLD);
    otp_rk_vld.u32 = otp_rk_vld.u32;
    print_rkp_rk_vld(otp_rk_vld, slot0_vld);
    print_rkp_rk_vld(otp_rk_vld, slot1_vld);
    print_rkp_rk_vld(otp_rk_vld, slot2_vld);
    print_rkp_rk_vld(otp_rk_vld, slot3_vld);
    print_rkp_rk_vld(otp_rk_vld, slot4_vld);
    print_rkp_rk_vld(otp_rk_vld, slot5_vld);
    print_rkp_rk_vld(otp_rk_vld, slot6_vld);
    print_rkp_rk_vld(otp_rk_vld, slot7_vld);
    print_rkp_rk_vld(otp_rk_vld, boot_vld);
    print_rkp_rk_vld(otp_rk_vld, hisi_vld);
    print_rkp_rk_vld(otp_rk_vld, stbm_vld);
}

void rkp_dump_rk_crc(hi_rkp_slot_choose slot)
{
    rkp_slotx_crc_rst slotx_crc_rst;

    slotx_crc_rst.u32 = rkp_read_reg(RKP_SLOT0_CRC_RST + slot * 0x4);
    slotx_crc_rst.u32 = slotx_crc_rst.u32;
    print_rkp_crc_rst(slotx_crc_rst, slotx_crc_ok);
    print_rkp_crc_rst(slotx_crc_rst, slotx_rk_all_zero);
    print_rkp_crc_rst(slotx_crc_rst, slotx_rk_all_one);
    if (slot <= HI_RKP_SLOT_CHOOSE_BOOT) {
        print_rkp_crc_rst(slotx_crc_rst, slotx_owner_id_all_zero);
        print_rkp_crc_rst(slotx_crc_rst, slotx_owner_id_all_one);
        print_rkp_crc_rst(slotx_crc_rst, slotx_owner_id_correct);
    }
}

#define ERR_NUM 27
char *g_err_str[ERR_NUM] = {
    "rkp finished.",
    "command type illegal.",
    "slot choose illegal.",
    "keyladder select illegal.",
    "otp fuse chip_id_sel illegal(deob+inte / inte).",
    "No keyladder lock(kdf).",
    "keyladder is busy(kdf).",
    "modlule_id is not match(kdf).",
    "allower nonce bit in module id set 1, keyladder level < 3.",
    "otp fuse deactivation_flag set 1,flash_prot_en in module id set 0, ca_vendor_id set 1.",
    "oem slot selected, start deob or deob+inte process.",
    "otp key slot is invalid.",
    "inte multi times",
    "keyladder level check faild(>5 for hkl or >4 for skl).",
    "c2_checknum_en in module id set 1,but otp fuse ca_vendor_id is not 2(irdeto).",
    "flash_prot_en in module id set 1,but otp fuse ca_vendor_id is not 1(nagra).",
    "ts_cipher in module in set 1, but slot is not common slot.",
    "slot is not oem or boot slot when execute commmand crc.",
    "inte+inte multi times in one slot.",
    "crc multi times in one slot.",
    "use ta keyladder but otp fuse ca_vendor_id is not 2.",
    "klad_sm4 in module id set 1, but otp fuse sm4_disable set 1.",
    "reserved_0.",
    "reserved_1.",
    "reserved_2.",
    "reserved_3.",
    "otp fuse direct_skl set 1 when execute command deob for tpp.",
};

void rkp_dump_err_status(void)
{
    int i = 0;
    rkp_error_status_cpux err_status;

    err_status.u32 = rkp_read_reg(RKP_ERROR_STATUS);
    for (i = 0; i < ERR_NUM; i++) {
        if (((1 << i) & err_status.u32) != 0) {
            HI_ERR_KLAD("%s\n", g_err_str[i]);
        }
    }
}


int dbg_rkp_deob_crc(rkp_deob_rst *deob_rst)
{
    rkp_calc_crc_0 crc_0;

    crc_0.u32 = 0;
    if (deob_rst == NULL) {
        return HI_FAILURE;
    }
    crc_0.u32 = rkp_read_reg(RKP_CALC_CRC_0);
    HI_ERR_KLAD("vmask_crc    : 0x%02x\n", crc_0.bits.calc_vmask_crc);
    HI_ERR_KLAD("clear_rk_crc : 0x%02x\n", crc_0.bits.calc_clear_rk_crc);
    deob_rst->vmask_crc = crc_0.bits.calc_vmask_crc;
    deob_rst->clear_rk_crc = crc_0.bits.calc_clear_rk_crc;
    return HI_SUCCESS;
}

int dbg_rkp_eff_rk_crc(rkp_deob_kdf_rst *deob_kdf_rst)
{
    rkp_calc_crc_0 crc_0;
    rkp_calc_crc_1 crc_1;

    crc_0.u32 = 0;
    crc_1.u32 = 0;
    if (deob_kdf_rst == NULL) {
        return HI_FAILURE;
    }

    rkp_get_crc(&crc_0, &crc_1);
    deob_kdf_rst->deob_rst.vmask_crc = crc_0.bits.calc_vmask_crc;
    deob_kdf_rst->deob_rst.clear_rk_crc = crc_0.bits.calc_clear_rk_crc;
    deob_kdf_rst->kdf_rst.sck_crc = crc_0.bits.calc_sck_crc;
    deob_kdf_rst->kdf_rst.sckv_crc = crc_1.bits.calc_sckv_crc;
    deob_kdf_rst->kdf_rst.seedv_crc = crc_1.bits.calc_seedv_crc;
    deob_kdf_rst->kdf_rst.modv_crc = crc_1.bits.calc_modk_crc;
    deob_kdf_rst->kdf_rst.eff_rk_crc = crc_1.bits.calc_eff_rk_crc;

    return HI_SUCCESS;
}

int dbg_rkp_fixed_rk_crc(rkp_fix_rk_kdf_rst *fix_rk_kdf_rst)
{
    rkp_calc_crc_0 crc_0;
    rkp_calc_crc_1 crc_1;

    crc_0.u32 = 0;
    crc_1.u32 = 0;
    if (fix_rk_kdf_rst == NULL) {
        return -1;
    }

    rkp_get_crc(&crc_0, &crc_1);
    fix_rk_kdf_rst->fix_rk_rst.fix_rk_crc = crc_0.bits.calc_fixed_rk_crc;
    fix_rk_kdf_rst->kdf_rst.sck_crc = crc_0.bits.calc_sck_crc;
    fix_rk_kdf_rst->kdf_rst.sckv_crc = crc_1.bits.calc_sckv_crc;
    fix_rk_kdf_rst->kdf_rst.seedv_crc = crc_1.bits.calc_seedv_crc;
    fix_rk_kdf_rst->kdf_rst.modv_crc = crc_1.bits.calc_modk_crc;
    fix_rk_kdf_rst->kdf_rst.eff_rk_crc = crc_1.bits.calc_eff_rk_crc;
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
