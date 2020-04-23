/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __VFMW_SYS_MDC_H__
#define __VFMW_SYS_MDC_H__

#define MDC_FREQ_BASE     0xA00324
#define MDC_FREQ_MAP_LEN  4
#define MDC_FREQ_MASK     0x000E0000
#define MDC_FREQ_BIT      17
#define MDC_FREQ_24M_CFG  0
#define MDC_FREQ_710M_CFG 1
#define MDC_FREQ_784M_CFG 2
#define MDC_FREQ_750M_CFG 3
#define MDC_FREQ_600M_CFG 4
#define MDC_FREQ_500M_CFG 5
#define MDC_FREQ_392M_CFG 6

#define MDC0_BASE_ADDR        0x01280000

#define MDC_INT_C2S_NUM       (26 + 8)
#define MDC_TEST_S2S_REG_NUM  (26 + 11)
#define MDC_INT_S2C_NUM       (132 + 32)
#define IPC_INT_BASE          (MDC0_BASE_ADDR + 0x1C000)
#define IPC_INT_SET           (0x0)
#define IPC_INT_CLEAR         (0x04)
#define IPC_INT_STATE         (0x08)
#define IPC_INT_MASK          (0x0C)
#define IPCM_REG_BASE         (MDC0_BASE_ADDR + 0x1C014)
#define IPCM_REG_MAP_LEN      0x10
#define IPCM_REG_CLT_STA_OFS  0x0
#define IPCM_REG_CLT_ADDR_OFS 0x4
#define IPCM_REG_SVR_STA_OFS  0x8
#define IPCM_REG_SVR_ADDR_OFS 0xC
#define IPCM_REG_ALIVE_BIT    31
#define MDC_STA_REG_BASE      (IPCM_REG_BASE + IPCM_REG_MAP_LEN)
#define MDC_STA_REG_BASE_LEN  (0x10)
#define IPC_INT1_C2S_NUM      (0x1)
#define IPC_INT1_S2C_NUM      (0x8)
#define IPC_INT1_S2C_NAME     "int_vmcu0_ipc1"
#define MDC_MBX_INT_C2S_NUM   (26 + 13)
#define MDC_TICK_REG          0x0129A008

#define SYS_CNT_REG_BASE    (MDC0_BASE_ADDR + 0x1A000)
#define SYS_CNT_TO_US       (24)
#define SYS_CNT_REG_CTRL    0x0
#define SYS_CNT_REG_MTIME_L 0x08
#define SYS_CNT_REG_MTIME_H 0x0C
#define SYS_CNT_REG_LEN     0x20
#define SYS_CNT_LOW_OFFSET  0x8
#define SYS_CNT_HIGH_OFFSET 0xC

/* BSA CFG */
#define BS_BASE_ADDR   (MDC0_BASE_ADDR + 0x14000)
#define BS_RST_REQ_REG (MDC0_BASE_ADDR + 0x1203C)
#define BS_RST_OK_REG  (MDC0_BASE_ADDR + 0x12028)
#define BS_RST_REQ_BIT 1
#define BS_RST_OK_BIT  4

/* MDC BOOT UPCFG */
#define MCU_CFG_CRG_OFS        0x12000
#define MCU_CFG_CRG_BASE       (MDC0_BASE_ADDR + MCU_CFG_CRG_OFS)
#define MCU_CFG_CRG_MAP_LEN    0x40
#define MCU_EXEC_ADDR_SRG_OFS  0x0
#define MCU_HART_ID_OFS        0x4
#define MCU_ITCM_START_REG_OSF 0x08
#define MCU_ITCM_END_REG_OSF   0x0C
#define MCU_DTCM_START_REG_OSF 0x10
#define MCU_DTCM_END_REG_OSF   0x14
#define MCU_AXI_START_REG_OSF  0x18
#define MCU_AXI_END_REG_OSF    0x1C
#define MCU_AHB_START_REG_OSF  0x20
#define MCU_AHB_END_REG_OSF    0x24
#define MCU_WFI_REG_OFS        0x28
#define MCU_WFI_BIT            0
#define MCU_WAIT_REG_OFS       0x2C
#define MCU_CLK_CFG            0x38
#define MCU_RST_REG_OFS        0x3C
#define ITCM_BASE_ADDR         MDC0_BASE_ADDR
#define ITCM_BASE_ADDR_IN_MDC  0x00000000
#define ITCM_CODE_LEN          0x8000 /* 32K */
#define DTCM_BASE_ADDR         (ITCM_BASE_ADDR + ITCM_CODE_LEN)
#define DTCM_BASE_ADDR_IN_MDC  0x00008000
#define DTCM_DATA_LEN          0x8000 /* 32K */
/* Relative path of vmdc_image.dat to mdc hal files */
#define MDC_BIN_PATH "../../img/vmdc_image.dat"

#define MDC_NO_SEC_RESERVED_ADDR 0x22000000
#define MDC_NO_SEC_RESERVED_SIZE 0x4000000
#define MCU_DAT_BASE_ADDR        (MDC_NO_SEC_RESERVED_ADDR + 0x400000)
#define MDC_PT_BASEADDR_NS       (MDC_NO_SEC_RESERVED_ADDR + 0x0)
#define MDC_LOG_OUTPUT_ADDR      (MDC_NO_SEC_RESERVED_ADDR + 0x800000)
#define MDC_LOG_OUTPUT_SIZE      0x100000
#define MDC_LOG_OUTPUT_ADDEND    (MDC_LOG_OUTPUT_ADDR + MDC_LOG_OUTPUT_SIZE)
#define MDC_PT_ERRADDR_RD_NS     (MDC_PT_BASEADDR_NS)         /* for temp */
#define MDC_PT_ERRADDR_WR_NS     (MDC_PT_BASEADDR_NS + 0x100) /* for temp */
#define MDC_SEC_RESERVED_ADDR    0x1C800000
#define MDC_SEC_RESERVED_SIZE    0xC00000
#define MDC_SEC_DAT_BASE_ADDR    (MDC_SEC_RESERVED_ADDR + 0x400000)
#define MDC_PT_BASEADDR_S        (MDC_SEC_RESERVED_ADDR + 0x0)
#define MDC_PT_ERRADDR_RD_S      (MDC_PT_BASEADDR_S)         /* for temp */
#define MDC_PT_ERRADDR_WR_S      (MDC_PT_BASEADDR_S + 0x100) /* for temp */
#define MDC_IOMMU_MAX_ADDR       0xFF000000

/* mdc iommu mem manage cfg */
#ifdef VFMW_SEC_SUPPORT
#define MDC_ADDR_LEN_NS_SCALE 50 /* 1~100 % */
#define MDC_ADDR_LEN_NC_SCALE 94 /* 1~100 % */

#define MDC_BIG_BLK_NUM        5
#define MDC_MIDDLE_BLK_NUM_CA  16
#define MDC_MIDDLE_BLK_NUM_NCA 32
#define MDC_SMALL_BLK_NUM      256
#define MDC_BIG_BLK_SIZE       0x4800000 /* dev hal */
#define MDC_SMALL_BLK_SIZE     0x38000
#else
#define MDC_ADDR_LEN_NS_SCALE 100 /* 1~100 % */
#define MDC_ADDR_LEN_NC_SCALE 93  /* 1~100 % */

#define MDC_BIG_BLK_NUM        5
#define MDC_MIDDLE_BLK_NUM_CA  64
#define MDC_MIDDLE_BLK_NUM_NCA 128
#define MDC_SMALL_BLK_NUM      1024
#define MDC_BIG_BLK_SIZE       0x4800000 /* dev hal */
#define MDC_SMALL_BLK_SIZE     0x38000
#endif

/* mdc iommu reg cfg */
#define IOMMU_COMMON_BASE_REG (MDC0_BASE_ADDR + 0x11000)
#define IOMMU_MASTER_BASE_REG (MDC0_BASE_ADDR + 0x10000)
#define IOMMU_SEC_IRQ_NUM     (26 + 4)
#define IOMMU_NO_SEC_IRQ_NUM  (26 + 5)

typedef enum {
    MPU_REGION_MMZ_START            = 0,
    /* LOS REGION CFG */
    MPU_REGION_LOS_CODE_START       = MPU_REGION_MMZ_START, /* 0 */
    MPU_REGION_LOS_CODE_END,        /* 1 */
    MPU_REGION_LOS_DATA_END,        /* 2 */

    /* PAGE_TABLE CONFIG */
    MPU_REGION_IOMMU_PT_NS,         /* 3 */
    MPU_REGION_IOMMU_PT_S,          /* 4 */

    /* ITCM CONFIG */
    MPU_REGION_ITCM,                /* 5 */
    MPU_REGION_LOG,                 /* 6 */
    MPU_REGION_MMZ_END,
    /*
    VFMW MMU REGION CFG:
    NC -- NON-CACHABLE
    C  -- CACHABLE
    NS -- NON-SECURE
    S  -- SECURE
    */
    MPU_REGION_MMU_START            = MPU_REGION_MMZ_END,
    MPU_REGION_IOMMU_NS_NC_START    = MPU_REGION_MMU_START, /* 7 */
    MPU_REGION_IOMMU_NS_NC_END,     /* 8 */
    MPU_REGION_IOMMU_NS_C_END,      /* 9 */
    MPU_REGION_IOMMU_S_NC_END,      /* 10 */
    MPU_REGION_IOMMU_S_C_END,       /* 11 */
    MPU_REGION_MMU_END,
} mdc_mpu_region_num;

#endif

#ifdef VDH_INT_TO_MCU
#define VDM_PXP_INT_SEC_NUM (26 + 22)
#define VDM_BSP_INT_SEC_NUM (26 + 24)
#define SCD_INT_SEC_NUM     (26 + 20)
#define MDMA_INT_SEC_NUM    (26 + 28)
#define MMU_VDH_SEC_NUM     (26 + 16)

#define VDM_PXP_INT_NUM (26 + 23)
#define VDM_BSP_INT_NUM (26 + 25)
#define SCD_INT_NUM     (26 + 21)
#define MDMA_INT_NUM    (26 + 29) /* not use yet */
#define MMU_VDH_NUM     (26 + 17)
#else
#define VDM_BSP_INT_SEC_NUM (138 + 32)
#define VDM_PXP_INT_SEC_NUM (136 + 32)
#define SCD_INT_SEC_NUM     (106 + 32)
#define MDMA_INT_SEC_NUM    (105 + 32)
#define MMU_VDH_SEC_NUM     (148 + 32)

#define VDM_PXP_INT_NUM (120 + 32)
#define VDM_BSP_INT_NUM (119 + 32)
#define SCD_INT_NUM     (121 + 32)
#define MMU_VDH_NUM     (123 + 32)

#endif
