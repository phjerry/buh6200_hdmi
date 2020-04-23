/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __VFMW_SYS_H__
#define __VFMW_SYS_H__
#include "vfmw_sys_mdc.h"
#define SCD_NUM      2
#define BPD_NUM      1
#define VDH_NUM      2
#define MDMA_NUM     2
#define VDH_REG_NUM  8
#define VDH_MSG_NUM  8
#define VDH_BSP_NUM  8
#define VDH_PXP_NUM  8
#define MDMA_REG_NUM 1
#define MDMA_MSG_NUM 1

#define FPGA_BSP_NUM 8
#define FPGA_PXP_NUM 8

#define VFMW_METDATA_MAX_SIZE 0x1000
#define VDH_BASE_PHY_ADDR     (0x01200000)
#define VDM_REG_PHY_ADDR      (VDH_BASE_PHY_ADDR)
#define SCD_REG_PHY_ADDR      (VDH_BASE_PHY_ADDR + 0x10000 + 0x3000)
#define SCD_REG_SEC_PHY_ADDR  (SCD_REG_PHY_ADDR + 0x60)     /* need check */

#define VDH_CRG_REG_PHY_ADDR (VDH_BASE_PHY_ADDR + 0x10000 + 0x3400)
#define BPD_REG_PHY_ADDR     (VDH_BASE_PHY_ADDR + 0x10000 + 0x4100)
/* MMU REG */
#define MMU_REG_PHY_ADDR (VDH_BASE_PHY_ADDR + 0x10000 + 0x7100)
#define MMU_REG_LENGTH   0x800
#define BSP_MMU_NUM      1
#define PXP_MMU_NUM      VDH_PXP_NUM
#define VDH_MMU_NUM      (BSP_MMU_NUM + PXP_MMU_NUM)

#define MDMA_REG_PHY_ADDR     (VDH_BASE_PHY_ADDR + 0x6100)
#define MDMA_REG_SEC_PHY_ADDR (VDH_BASE_PHY_ADDR + 0x6100)  /* need check */

#define VREG_OFS_MMU_SRC             (0x000)
#define VERG_MMU_SRC_GLB_BYPASS_BIT  (0)
#define VERG_MMU_SRC_SEC_PT_TYPE_BIT (4)
#define VERG_MMU_SRC_LOCK_BIT        (31)
#define VREG_OFS_MMU_CTRL            (0x004)
#define VERG_MMU_CTRL_INT_EN_BIT     (3)
#define VERG_MMU_CTRL_PT_TYPE_BIT    (4)
#define VERG_MMU_CTRL_PTW_PF_BIT     (16)

#define VREG_OFS_SEC_MMU_CB_TTBR         (0x100)
#define VREG_OFS_SEC_MMU_CB_TTBR_H       (0x104)
#define VREG_OFS_SEC_MMU_TAG_RD_CTRL     (0x110)
#define VREG_OFS_SEC_MMU_TAG_RD_ACCESS_0 (0x114)
#define VREG_OFS_SEC_MMU_TAG_RD_ACCESS_1 (0x118)
#define VREG_OFS_SEC_MMU_TAG_WR_CTRL     (0x120)
#define VREG_OFS_SEC_MMU_TAG_WR_ACCESS_0 (0x124)
#define VREG_OFS_SEC_MMU_TAG_WR_ACCESS_1 (0x128)
#define VREG_OFS_SEC_MMU_ERR_RD_ADDR     (0x130)
#define VREG_OFS_SEC_MMU_ERR_RD_ADDR_H   (0x134)
#define VREG_OFS_SEC_MMU_ERR_WR_ADDR     (0x138)
#define VREG_OFS_SEC_MMU_ERR_WR_ADDR_H   (0x13C)
#define VREG_OFS_SEC_MMU_INT_MASK        (0x010)

#define VREG_OFS_SEC_SMMU_INTSTAT       (0xd018)
#define VREG_OFS_SEC_SMMU_INTCLR        (0xd01c)
#define VREG_OFS_SEC_SMMU_FAULT_ADDR_RD (0xd350)
#define VREG_OFS_SEC_SMMU_FAULT_ADDR_WR (0xd330)

#define VREG_OFS_MMU_CB_TTBR       (0x200)
#define VREG_OFS_MMU_CB_TTBR_H     (0x204)
#define VREG_OFS_MMU_ERR_RD_ADDR   (0x230)
#define VREG_OFS_MMU_ERR_RD_ADDR_H (0x234)
#define VREG_OFS_MMU_ERR_WR_ADDR   (0x238)
#define VREG_OFS_MMU_ERR_WR_ADDR_H (0x23C)
#define VREG_OFS_MMU_INT_MASK      (0x020)

#define VREG_OFS_SMMU_INTSTAT       (0xd028)
#define VREG_OFS_SMMU_INTCLR        (0xd02c)
#define VREG_OFS_SMMU_FAULT_ADDR_RD (0xd360)
#define VREG_OFS_SMMU_FAULT_ADDR_WR (0xd340)

#define VDM_CLKSEL_PHY_ADDR  (VDH_CRG_REG_PHY_ADDR + 0x04)
#define VDM_RST_REG_PHY_ADDR (VDH_CRG_REG_PHY_ADDR + 0x0c)
#define VDM_RST_OK_PHY_ADDR  (VDH_CRG_REG_PHY_ADDR + 0x14)

typedef struct {
    hi_u32 vdh_all_clken : 2;
    hi_u32 vdh_mfde_clken : 2;
    hi_u32 vdh_pxp_clken : 2;
    hi_u32 vdh_mdma_clken : 2;
    hi_u32 vdh_scd_clken : 2;
    hi_u32 vdh_bpd_clken : 2;
    hi_u32 vdh_bsp_clken : 2;
    hi_u32 reserved : 18;
} vcrg_vdh_clksel;

typedef struct {
    hi_u32 vdh_all_srst_req : 1;
    hi_u32 vdh_mfde_srst_req : 1;
    hi_u32 vdh_mdma_srst_req : 1;
    hi_u32 vdh_scd_srst_req : 1;
    hi_u32 vdh_bpd_srst_req : 1;
    hi_u32 reserved : 27;
} vcrg_vdh_srst_req;

typedef struct {
    hi_u32 vdh_all_force_ack : 1;
    hi_u32 vdh_mfde_force_ack : 1;
    hi_u32 vdh_mdma_force_ack : 1;
    hi_u32 vdh_scd_force_ack : 1;
    hi_u32 vdh_bpd_force_ack : 1;
    hi_u32 reserved : 27;
} vcrg_vdh_force_req_ack;

typedef struct {
    hi_u32 vdh_all_srst_ok : 1;
    hi_u32 vdh_mfde_srst_ok : 1;
    hi_u32 vdh_mdma_srst_ok : 1;
    hi_u32 vdh_scd_srst_ok : 1;
    hi_u32 vdh_bpd_srst_ok : 1;
    hi_u32 reserved : 27;
} vcrg_vdh_srst_ok;

typedef struct {
    hi_u32 vdh_idle : 1;
    hi_u32 vdh_mfde_idle : 1;
    hi_u32 vdh_pxp_idle : 1;
    hi_u32 vdh_mdma_idle : 1;
    hi_u32 vdh_scd_idle : 1;
    hi_u32 vdh_bpd_idle : 1;
    hi_u32 vdh_bsp_idle : 1;
    hi_u32 reserved : 25;
} vcrg_vdh_ip_idle;

typedef struct {
    UADDR sys_crg_phy;
    hi_u8 *sys_crg_vir;

    UADDR vdh_crg_phy;
    hi_u8 *vdh_crg_vir;

    UADDR vdh_mmu_phy;
    hi_u8 *vdh_mmu_vir;
} vdh_glb_reg_info;

#endif
