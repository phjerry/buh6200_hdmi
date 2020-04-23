#ifndef __DRV_OSAL_HI3796CV300_H_
#define __DRV_OSAL_HI3796CV300_H_

#if defined(HI_TEE_SUPPORT) || defined(HI_LOADER_APPLOADER) || defined(HI_DRV_WORK_IN_NON_SECURE_MODE)

/* the total cipher hard channel which we can used*/
#define CIPHER_HARD_CHANNEL_CNT         (0x0F)

/* mask which cipher channel we can used, bit0 means channel 0*/
#define CIPHER_HARD_CHANNEL_MASK        (0xFFFE)

/* the total hash hard channel which we can used*/
#define HASH_HARD_CHANNEL_CNT           (0x01)

/* mask which cipher channel we can used, bit0 means channel 0*/
#define HASH_HARD_CHANNEL_MASK          (0x04)
#define HASH_HARD_CHANNEL               (0x02)

/* the total cipher hard key channel which we can used*/
#define CIPHER_HARD_KEY_CHANNEL_CNT     (0x02)

/* mask which cipher hard key channel we can used, bit0 means channel 0*/
#define CIPHER_HARD_KEY_CHANNEL_MASK    (0x0C)

/* support smmu*/
#ifdef HI_SMMU_SUPPORT
#define CRYPTO_SMMU_SUPPORT
#endif

/* support interrupt*/
#define CRYPTO_OS_INT_SUPPORT

/* MSR 2.2 remove CAUK within hi3796mv200, so needn't support hash_cmac any more after hi3796mv200 */
//#define HASH_CMAC_SUPPORT

/* the hardware version */
#define CHIP_SYMC_VER_V300
#define CHIP_HDCP_VER_V300
#define CHIP_HASH_VER_V300
//#define CHIP_IFEP_RSA_VER_V100
#define CHIP_PKE_VER_V200
#define TEE_CIPHER_TA_NONSUPPORT

/* supoort odd key */
#define CHIP_SYMC_ODD_KEY_SUPPORT

/* supoort SM3 */
#define CHIP_SYMC_SM3_SUPPORT

/* supoort SM4 */
#define CHIP_SYMC_SM4_SUPPORT

/* supoort spacc channel contest */
#define SPACC_CHN_WHO_USED_SUPPORT

/* the hardware capacity */
#define CHIP_AES_CCM_GCM_SUPPORT

/* symc cpu key from klad */
#define CHIP_SYMC_CPU_KEY_FROM_KLAD

/* the software capacity */
//#define SOFT_PKE_SUPPORT
//#define SOFT_ECC_SUPPORT
#else
/* when no TEE , default cpu is sec cpu*/
//#define CRYPTO_SEC_CPU

/* the total cipher hard channel which we can used*/
#define CIPHER_HARD_CHANNEL_CNT         (0x0F)

/* mask which cipher channel we can used, bit0 means channel 0*/
#define CIPHER_HARD_CHANNEL_MASK        (0xFFFE)

/* the total hash hard channel which we can used*/
#define HASH_HARD_CHANNEL_CNT           (0x01)

/* mask which cipher channel we can used, bit0 means channel 0*/
#define HASH_HARD_CHANNEL_MASK          (0x04)
#define HASH_HARD_CHANNEL               (0x02)

/* the total cipher hard key channel which we can used*/
#define CIPHER_HARD_KEY_CHANNEL_CNT     (0x06)

/* mask which cipher hard key channel we can used, bit0 means channel 0*/
#define CIPHER_HARD_KEY_CHANNEL_MASK    (0xFC)

/* support smmu*/
#define CRYPTO_SMMU_SUPPORT

/* support interrupt*/
#define CRYPTO_OS_INT_SUPPORT

/* MSR 2.2 remove CAUK within hi3796mv200, so needn't support hash_cmac any more after hi3796mv200 */
//#define HASH_CMAC_SUPPORT

/* the hardware version */
#define CHIP_SYMC_VER_V300
#define CHIP_HDCP_VER_V300
#define CHIP_HASH_VER_V300
#define CHIP_TRNG_VER_V200
//#define CHIP_IFEP_RSA_VER_V100
#define CHIP_PKE_VER_V200

#define SOFT_SM3_SUPPORT
#define SOFT_SHA1_SUPPORT
#define SOFT_SHA256_SUPPORT
#define SOFT_SHA512_SUPPORT

/* supoort odd key */
#define CHIP_SYMC_ODD_KEY_SUPPORT

/* supoort SM3 */
#define CHIP_SYMC_SM3_SUPPORT

/* supoort SM4 */
#define CHIP_SYMC_SM4_SUPPORT

/* the hardware capacity */
#define CHIP_AES_CCM_GCM_SUPPORT

/* the software capacity */
//#define SOFT_PKE_SUPPORT
//#define SOFT_ECC_SUPPORT
#endif

/* moudle unsupport, we need set the table*/
#define BASE_TABLE_NULL    {\
    .reset_valid = 0,  \
    .clk_valid = 0, \
    .phy_valid = 0, \
    .crg_valid = 0, \
    .ver_valid = 0, \
    .int_valid = 0, \
}

/* define initial value of struct sys_arch_boot_dts for cipher*/
#define HARD_INFO_CIPHER {\
    .name = "int_spacc_tee",  \
    .reset_valid = 0,  \
    .clk_valid = 0, \
    .phy_valid = 1, \
    .crg_valid = 1, \
    .ver_valid = 1, \
    .int_valid = 1, \
    .int_num = 172, \
    .version_reg = 0x308, \
    .version_val = 0x0, \
    .reg_addr_phy = 0x00bc0000, \
    .reg_addr_size = 0x10000,    \
    .crg_addr_phy = 0xb60104, \
    .reset_bit = 4, \
    .clk_bit = 5, \
}

/* define initial value of struct sys_arch_boot_dts for cipher*/
#define HARD_INFO_HASH {\
    .name = "int_spacc_tee",  \
    .reset_valid = 0,  \
    .clk_valid = 0, \
    .phy_valid = 1, \
    .crg_valid = 0, \
    .ver_valid = 0, \
    .int_valid = 1, \
    .int_num = 172, \
    .reg_addr_phy = 0x00bc0000, \
    .reg_addr_size = 0x10000, \
}

/* define initial value of struct sys_arch_boot_dts for HASH*/
#define HARD_INFO_TRNG {\
    .name = "trng",  \
    .reset_valid = 0,  \
    .clk_valid = 0, \
    .phy_valid = 1, \
    .crg_valid = 0, \
    .ver_valid = 0, \
    .int_valid = 0, \
    .reg_addr_phy = 0x00B0C200,  \
    .reg_addr_size = 0x100,   \
}

/* define initial value of struct sys_arch_boot_dts for SM2 */
#define HARD_INFO_PKE {\
    .name = "int_pke_tee",  \
    .reset_valid = 1,  \
    .clk_valid = 1, \
    .phy_valid = 1, \
    .crg_valid = 1, \
    .ver_valid = 0, \
    .int_valid = 1, \
    .reg_addr_phy = 0x00B90000,  \
    .reg_addr_size = 0x2000,\
    .crg_addr_phy = 0x00B60104, \
    .reset_bit = 2, \
    .clk_bit = 3, \
    .int_num = 168, \
    .version_reg = 0x88, \
    .version_val = 0x00000009, \
}

#define HARD_INFO_SMMU             BASE_TABLE_NULL
#define HARD_INFO_SIC_RSA          BASE_TABLE_NULL
#define HARD_INFO_CIPHER_KEY       BASE_TABLE_NULL
#define HARD_INFO_SM4              BASE_TABLE_NULL
#define HARD_INFO_IFEP_RSA         BASE_TABLE_NULL

/* the loader cpu may be running under non-secure CPU or secure CPU,
 * need to switch the hard info when ree running in non-secure CPU
 */
#define CRYPTO_SWITCH_CPU

#define NSEC_HARD_INFO_CIPHER {\
    .name = "int_spacc_ree",  \
    .reset_valid = 0,  \
    .clk_valid = 0, \
    .phy_valid = 1, \
    .crg_valid = 0, \
    .ver_valid = 1, \
    .int_valid = 1, \
    .int_num = 173, \
    .version_reg = 0x308, \
    .version_val = 0x0, \
    .reg_addr_phy = 0x00bc0000, \
    .reg_addr_size = 0x10000,    \
    .crg_addr_phy = 0xb60104, \
    .reset_bit = 4, \
    .clk_bit = 5, \
}

/* define initial value of struct sys_arch_boot_dts for cipher*/
#define NSEC_HARD_INFO_HASH {\
    .name = "int_spacc_ree",  \
    .reset_valid = 0,  \
    .clk_valid = 0, \
    .phy_valid = 1, \
    .crg_valid = 0, \
    .ver_valid = 1, \
    .int_valid = 1, \
    .int_num = 173, \
    .version_reg = 0x308, \
    .version_val = 0x0, \
    .reg_addr_phy = 0x00bc0000, \
    .reg_addr_size = 0x10000,    \
    .crg_addr_phy = 0xb60104, \
    .reset_bit = 4, \
    .clk_bit = 5, \
}

/* define initial value of struct sys_arch_boot_dts for SM2 */
#define NSEC_HARD_INFO_PKE {\
    .name = "int_pke_ree",  \
    .reset_valid = 1,  \
    .clk_valid = 1, \
    .phy_valid = 1, \
    .crg_valid = 1, \
    .ver_valid = 0, \
    .int_valid = 1, \
    .reg_addr_phy = 0x00B90000,  \
    .reg_addr_size = 0x2000,\
    .crg_addr_phy = 0x00B60104, \
    .reset_bit = 2, \
    .clk_bit = 3, \
    .int_num = 169, \
    .version_reg = 0x88, \
    .version_val = 0x00000009, \
}

#define NSEC_HARD_INFO_CIPHER_KEY       BASE_TABLE_NULL
#define NSEC_HARD_INFO_SIC_RSA          BASE_TABLE_NULL
#define NSEC_HARD_INFO_TRNG             BASE_TABLE_NULL
#define NSEC_HARD_INFO_SMMU             BASE_TABLE_NULL
#define NSEC_HARD_INFO_SM4              BASE_TABLE_NULL
#define NSEC_HARD_INFO_IFEP_RSA         BASE_TABLE_NULL

#endif

