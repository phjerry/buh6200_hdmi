/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_ip_para.h"

#include "hal_vdp_comm.h"
#include "hal_vdp_reg_para.h"
#ifndef CBB_OSAL_TYPE_SUPPORT
#include "drv_xdp_ion.h"
#endif

#define VDP_ALL_COEF_SIZE  8000000
#define VDP_COEF_OVER_SIZE 128

#define COEF_SIZE_DCI         (4096 * 128 / 8)
#define COEF_SIZE_ACM         (4096 * 128 / 8)
#define COEF_SIZE_V1_HZME     (4096 * 128 / 8)
#define COEF_SIZE_V1_VZME     (4096 * 128 / 8)
#define COEF_SIZE_8K_HZME     (4096 * 128 / 8)
#define COEF_SIZE_8K_VZME     (4096 * 128 / 8)
#define COEF_SIZE_GZME        (4096 * 128 / 8)
#define COEF_SIZE_V1_HDRD     (4096 * 128 / 8)
#define COEF_SIZE_V1_HDRS     (4096 * 128 / 8)
#define COEF_SIZE_V1_PS0_CLUT (4096 * 128 / 8)
#define COEF_SIZE_V1_PS1_PLUT (4096 * 128 / 8)
#define COEF_SIZE_V1_PS2_SLUT (4096 * 128 / 8)
#define COEF_SIZE_V1_PS3_ILUT (4096 * 128 / 8)
#define COEF_SIZE_V1_PS4_DLUT (4096 * 128 / 8)
#define COEF_SIZE_WD_HZME     (4096 * 128 / 8)
#define COEF_SIZE_WD_HCDS     (4096 * 128 / 8)
#define COEF_SIZE_WD_VZME     (4096 * 128 / 8)
#define COEF_SIZE_GCHN0_CLUT  0  // (4096*128/8)
#define COEF_SIZE_GCHN1_CLUT  (4096 * 128 / 8)

#define COEF_SIZE_DIM0      (4096 * 128 / 8)
#define COEF_SIZE_DIM1      (4096 * 128 / 8)
#define COEF_SIZE_DIM2      (4096 * 128 / 8)
#define COEF_SIZE_SELF_RGBW (4096 * 128 / 8)
#define COEF_SIZE_GAMMA     (4096 * 128 / 8)
#define COEF_SIZE_PCID      (4096 * 128 / 8)
#define COEF_SIZE_MPLUS     (4096 * 128 / 8)
#define COEF_SIZE_FCIC      (40960 * 128 / 8)
#define COEF_SIZE_DIM       (256 * 128 / 8)
#define COEF_SIZE_DIM_HZME  (257 * 128 / 8)
#define COEF_SIZE_DIM_VZME  (257 * 128 / 8)
#define COEF_SIZE_CGM       (205 * 128 / 8)

#define COEF_SIZE_DC (17 * 128 / 8)

#define COEF_SIZE_OD          (273 * 128 * 24 / 8 * 4) // *3 to support addr exchange
#define COEF_SIZE_SELF_DEMURA (261515 * 128 / 8 * 4)   //
#define COEF_SIZE_DEMURA      (261515 * 128 / 8 * 4)   //
#define COEF_SIZE_DITHER      (128 * 128 / 8)          //

#define COEF_SIZE_4K_SR0        (12000 * 128 / 8)
#define COEF_SIZE_4K_SR1        (4096 * 128 / 8)
#define COEF_SIZE_8K_SR0        (12000 * 128 / 8)
#define COEF_SIZE_8K_SR1        (100 * 128 / 8)
#define COEF_SIZE_8K_SR2        (100 * 128 / 8)
#define COEF_SIZE_8K_SR3        (100 * 128 / 8)
#define COEF_SIZE_V0_F_PS3_ILUT (4096 * 128 / 8)
#define COEF_SIZE_V0_F_PS4_DLUT (4096 * 128 / 8)
#define COEF_SIZE_V0_F_SLF      (4096 * 128 / 8)
#define COEF_SIZE_V0_F_HDRMAIN0 (4096 * 128 / 8)
#define COEF_SIZE_V0_F_HDRMAIN1 (4096 * 128 / 8)
#define COEF_SIZE_XDP_2DSCALE_H (4096 * 128 / 8)
#define COEF_SIZE_XDP_2DSCALE_V (4096 * 128 / 8)
#define COEF_SIZE_SHARPEN_FHD   (4096 * 128 / 8)
#define COEF_SIZE_V0_F_SR0      (4096 * 128 / 8)
#define COEF_SIZE_V0_F_SR1      (4096 * 128 / 8)
#define COEF_SIZE_SHARPEN_4K    (4096 * 128 / 8)
#define COEF_SIZE_V0_F_HZME     (4096 * 128 / 8)
#define COEF_SIZE_WF_HZME1      (4096 * 128 / 8)
#define COEF_SIZE_WF_VZME1      (4096 * 128 / 8)
#define COEF_SIZE_WF_HZME2      (4096 * 128 / 8)
#define COEF_SIZE_WF_VZME2      (4096 * 128 / 8)
#define COEF_SIZE_V0_F_DCI      (4096 * 128 / 8)
#define COEF_SIZE_HDR0          8192  // 4096
#define COEF_SIZE_HDR1          4096
#define COEF_SIZE_HDR2          10000  // 4096
#define COEF_SIZE_HDR3          4096
#define COEF_SIZE_HDR4          4096
#define COEF_SIZE_HDR5          4096
#define COEF_SIZE_HDR6          8192
#define COEF_SIZE_HDR7          4096
#define COEF_SIZE_HDR8          40000
#define COEF_SIZE_OM_DEGMM      (256 * 128 / 8)
#define COEF_SIZE_OM_GMM        (256 * 128 / 8)
#define COEF_SIZE_OM_CM         (256 * 128 / 8)
#define COEF_SIZE_REGION_V1     (192 * 128 / 8)

#define COEF_SIZE_4K_CLE_REG 500
#define COEF_SIZE_8K_CLE_REG 500

vdp_coef_addr g_vdp_coef_buf_addr = { 0 };

#ifndef CBB_OSAL_TYPE_SUPPORT
drv_xdp_mem_info g_buf_addr = { 0 };
#else
MMZ_BUFFER_S g_buf_addr = { 0 };
#endif

static hi_void vir_addr_distribute(vdp_coef_addr *addr)
{
    addr->vir_addr[VDP_COEF_BUF_V0_HDR0] = addr->vir_addr[VDP_COEF_BUF_8K_DCI0] + COEF_SIZE_DCI;
    addr->vir_addr[VDP_COEF_BUF_V0_HDR1] = addr->vir_addr[VDP_COEF_BUF_V0_HDR0] + COEF_SIZE_HDR0;
    addr->vir_addr[VDP_COEF_BUF_V0_HDR2] = addr->vir_addr[VDP_COEF_BUF_V0_HDR1] + COEF_SIZE_HDR1;
    addr->vir_addr[VDP_COEF_BUF_V0_HDR3] = addr->vir_addr[VDP_COEF_BUF_V0_HDR2] + COEF_SIZE_HDR2;
    addr->vir_addr[VDP_COEF_BUF_V0_HDR4] = addr->vir_addr[VDP_COEF_BUF_V0_HDR3] + COEF_SIZE_HDR3;
    addr->vir_addr[VDP_COEF_BUF_V0_HDR5] = addr->vir_addr[VDP_COEF_BUF_V0_HDR4] + COEF_SIZE_HDR4;
    addr->vir_addr[VDP_COEF_BUF_V0_HDR6] = addr->vir_addr[VDP_COEF_BUF_V0_HDR5] + COEF_SIZE_HDR5;
    addr->vir_addr[VDP_COEF_BUF_V0_HDR7] = addr->vir_addr[VDP_COEF_BUF_V0_HDR6] + COEF_SIZE_HDR6;
    addr->vir_addr[VDP_COEF_BUF_V0_HDR8] = addr->vir_addr[VDP_COEF_BUF_V0_HDR7] + COEF_SIZE_HDR7;
    addr->vir_addr[VDP_COEF_BUF_OM_DEGMM] = addr->vir_addr[VDP_COEF_BUF_V0_HDR8] + COEF_SIZE_HDR8;
    addr->vir_addr[VDP_COEF_BUF_OM_GMM] = addr->vir_addr[VDP_COEF_BUF_OM_DEGMM] + COEF_SIZE_OM_DEGMM;
    addr->vir_addr[VDP_COEF_BUF_OM_CM] = addr->vir_addr[VDP_COEF_BUF_OM_GMM] + COEF_SIZE_OM_GMM;
    addr->vir_addr[VDP_COEF_BUF_ACM] = addr->vir_addr[VDP_COEF_BUF_OM_CM] + COEF_SIZE_OM_CM;
    addr->vir_addr[VDP_COEF_BUF_V1_HZME] = addr->vir_addr[VDP_COEF_BUF_ACM] + COEF_SIZE_ACM;
    addr->vir_addr[VDP_COEF_BUF_V1_VZME] = addr->vir_addr[VDP_COEF_BUF_V1_HZME] + COEF_SIZE_V1_HZME;
    addr->vir_addr[VDP_COEF_BUF_V1_DBHDR0] = addr->vir_addr[VDP_COEF_BUF_V1_VZME] + COEF_SIZE_V1_VZME;
    addr->vir_addr[VDP_COEF_BUF_V1_DBHDR1] = addr->vir_addr[VDP_COEF_BUF_V1_DBHDR0] + COEF_SIZE_HDR2;
    addr->vir_addr[VDP_COEF_BUF_V1_DBHDR2] = addr->vir_addr[VDP_COEF_BUF_V1_DBHDR1] + COEF_SIZE_HDR3;
    addr->vir_addr[VDP_COEF_BUF_V1_DBHDR3] = addr->vir_addr[VDP_COEF_BUF_V1_DBHDR2] + COEF_SIZE_HDR4;
    addr->vir_addr[VDP_COEF_BUF_V1_DBHDR4] = addr->vir_addr[VDP_COEF_BUF_V1_DBHDR3] + COEF_SIZE_HDR5;
    addr->vir_addr[VDP_COEF_BUF_SHARPEN_FHD] = addr->vir_addr[VDP_COEF_BUF_V1_DBHDR4] + COEF_SIZE_HDR6;
    addr->vir_addr[VDP_COEF_BUF_8K_VZME] = addr->vir_addr[VDP_COEF_BUF_SHARPEN_FHD] + COEF_SIZE_SHARPEN_FHD;
    addr->vir_addr[VDP_COEF_BUF_8K_HZME] = addr->vir_addr[VDP_COEF_BUF_8K_VZME] + COEF_SIZE_8K_VZME;
    addr->vir_addr[VDP_COEF_BUF_XDP_2DSCALE_H] = addr->vir_addr[VDP_COEF_BUF_8K_HZME] + COEF_SIZE_8K_HZME;
    addr->vir_addr[VDP_COEF_BUF_XDP_2DSCALE_V] = addr->vir_addr[VDP_COEF_BUF_XDP_2DSCALE_H] + COEF_SIZE_XDP_2DSCALE_H;
    addr->vir_addr[VDP_COEF_BUF_CGM] = addr->vir_addr[VDP_COEF_BUF_XDP_2DSCALE_V] + COEF_SIZE_XDP_2DSCALE_V;
    addr->vir_addr[VDP_COEF_BUF_GAMMA] = addr->vir_addr[VDP_COEF_BUF_CGM] + COEF_SIZE_CGM;
    addr->vir_addr[VDP_COEF_BUF_4K_SR0] = addr->vir_addr[VDP_COEF_BUF_GAMMA] + COEF_SIZE_GAMMA;
    addr->vir_addr[VDP_COEF_BUF_4K_SR1] = addr->vir_addr[VDP_COEF_BUF_4K_SR0] + COEF_SIZE_4K_SR0;
    addr->vir_addr[VDP_COEF_BUF_8K_SR0] = addr->vir_addr[VDP_COEF_BUF_4K_SR1] + COEF_SIZE_4K_SR1;
    addr->vir_addr[VDP_COEF_BUF_8K_SR1] = addr->vir_addr[VDP_COEF_BUF_8K_SR0] + COEF_SIZE_8K_SR0;
    addr->vir_addr[VDP_COEF_BUF_8K_SR2] = addr->vir_addr[VDP_COEF_BUF_8K_SR1] + COEF_SIZE_8K_SR1;
    addr->vir_addr[VDP_COEF_BUF_8K_SR3] = addr->vir_addr[VDP_COEF_BUF_8K_SR2] + COEF_SIZE_8K_SR2;
    addr->vir_addr[VDP_COEF_BUF_DC] = addr->vir_addr[VDP_COEF_BUF_8K_SR3] + COEF_SIZE_8K_SR3;
    addr->vir_addr[VDP_COEF_BUF_REGION_V1] = addr->vir_addr[VDP_COEF_BUF_DC] + COEF_SIZE_DC;
    addr->vir_addr[VDP_COEF_BUF_DIM] = addr->vir_addr[VDP_COEF_BUF_REGION_V1] + COEF_SIZE_REGION_V1;
    addr->vir_addr[VDP_COEF_BUF_DIM_HZME] = addr->vir_addr[VDP_COEF_BUF_DIM] + COEF_SIZE_DIM;
    addr->vir_addr[VDP_COEF_BUF_DIM_VZME] = addr->vir_addr[VDP_COEF_BUF_DIM_HZME] + COEF_SIZE_DIM_HZME;
    addr->vir_addr[VDP_COEF_BUF_4K_CLE_REG] = addr->vir_addr[VDP_COEF_BUF_DIM_VZME] + COEF_SIZE_DIM_VZME;
    addr->vir_addr[VDP_COEF_BUF_8K_CLE_REG] = addr->vir_addr[VDP_COEF_BUF_4K_CLE_REG] + COEF_SIZE_4K_CLE_REG;
}

static hi_s32 phy_addr_distribute(vdp_coef_addr *addr)
{
    addr->phy_addr[VDP_COEF_BUF_V0_HDR0] = addr->phy_addr[VDP_COEF_BUF_8K_DCI0] + COEF_SIZE_DCI;
    addr->phy_addr[VDP_COEF_BUF_V0_HDR1] = addr->phy_addr[VDP_COEF_BUF_V0_HDR0] + COEF_SIZE_HDR0;
    addr->phy_addr[VDP_COEF_BUF_V0_HDR2] = addr->phy_addr[VDP_COEF_BUF_V0_HDR1] + COEF_SIZE_HDR1;
    addr->phy_addr[VDP_COEF_BUF_V0_HDR3] = addr->phy_addr[VDP_COEF_BUF_V0_HDR2] + COEF_SIZE_HDR2;
    addr->phy_addr[VDP_COEF_BUF_V0_HDR4] = addr->phy_addr[VDP_COEF_BUF_V0_HDR3] + COEF_SIZE_HDR3;
    addr->phy_addr[VDP_COEF_BUF_V0_HDR5] = addr->phy_addr[VDP_COEF_BUF_V0_HDR4] + COEF_SIZE_HDR4;
    addr->phy_addr[VDP_COEF_BUF_V0_HDR6] = addr->phy_addr[VDP_COEF_BUF_V0_HDR5] + COEF_SIZE_HDR5;
    addr->phy_addr[VDP_COEF_BUF_V0_HDR7] = addr->phy_addr[VDP_COEF_BUF_V0_HDR6] + COEF_SIZE_HDR6;
    addr->phy_addr[VDP_COEF_BUF_V0_HDR8] = addr->phy_addr[VDP_COEF_BUF_V0_HDR7] + COEF_SIZE_HDR7;
    addr->phy_addr[VDP_COEF_BUF_OM_DEGMM] = addr->phy_addr[VDP_COEF_BUF_V0_HDR8] + COEF_SIZE_HDR8;
    addr->phy_addr[VDP_COEF_BUF_OM_GMM] = addr->phy_addr[VDP_COEF_BUF_OM_DEGMM] + COEF_SIZE_OM_DEGMM;
    addr->phy_addr[VDP_COEF_BUF_OM_CM] = addr->phy_addr[VDP_COEF_BUF_OM_GMM] + COEF_SIZE_OM_GMM;
    addr->phy_addr[VDP_COEF_BUF_ACM] = addr->phy_addr[VDP_COEF_BUF_OM_CM] + COEF_SIZE_OM_CM;
    addr->phy_addr[VDP_COEF_BUF_V1_HZME] = addr->phy_addr[VDP_COEF_BUF_ACM] + COEF_SIZE_ACM;
    addr->phy_addr[VDP_COEF_BUF_V1_VZME] = addr->phy_addr[VDP_COEF_BUF_V1_HZME] + COEF_SIZE_V1_HZME;
    addr->phy_addr[VDP_COEF_BUF_V1_DBHDR0] = addr->phy_addr[VDP_COEF_BUF_V1_VZME] + COEF_SIZE_V1_VZME;
    addr->phy_addr[VDP_COEF_BUF_V1_DBHDR1] = addr->phy_addr[VDP_COEF_BUF_V1_DBHDR0] + COEF_SIZE_HDR2;
    addr->phy_addr[VDP_COEF_BUF_V1_DBHDR2] = addr->phy_addr[VDP_COEF_BUF_V1_DBHDR1] + COEF_SIZE_HDR3;
    addr->phy_addr[VDP_COEF_BUF_V1_DBHDR3] = addr->phy_addr[VDP_COEF_BUF_V1_DBHDR2] + COEF_SIZE_HDR4;
    addr->phy_addr[VDP_COEF_BUF_V1_DBHDR4] = addr->phy_addr[VDP_COEF_BUF_V1_DBHDR3] + COEF_SIZE_HDR5;
    addr->phy_addr[VDP_COEF_BUF_SHARPEN_FHD] = addr->phy_addr[VDP_COEF_BUF_V1_DBHDR4] + COEF_SIZE_HDR6;
    addr->phy_addr[VDP_COEF_BUF_8K_VZME] = addr->phy_addr[VDP_COEF_BUF_SHARPEN_FHD] + COEF_SIZE_SHARPEN_FHD;
    addr->phy_addr[VDP_COEF_BUF_8K_HZME] = addr->phy_addr[VDP_COEF_BUF_8K_VZME] + COEF_SIZE_8K_VZME;
    addr->phy_addr[VDP_COEF_BUF_XDP_2DSCALE_H] = addr->phy_addr[VDP_COEF_BUF_8K_HZME] + COEF_SIZE_8K_HZME;
    addr->phy_addr[VDP_COEF_BUF_XDP_2DSCALE_V] = addr->phy_addr[VDP_COEF_BUF_XDP_2DSCALE_H] + COEF_SIZE_XDP_2DSCALE_H;
    addr->phy_addr[VDP_COEF_BUF_CGM] = addr->phy_addr[VDP_COEF_BUF_XDP_2DSCALE_V] + COEF_SIZE_XDP_2DSCALE_V;
    addr->phy_addr[VDP_COEF_BUF_GAMMA] = addr->phy_addr[VDP_COEF_BUF_CGM] + COEF_SIZE_CGM;
    addr->phy_addr[VDP_COEF_BUF_4K_SR0] = addr->phy_addr[VDP_COEF_BUF_GAMMA] + COEF_SIZE_GAMMA;
    addr->phy_addr[VDP_COEF_BUF_4K_SR1] = addr->phy_addr[VDP_COEF_BUF_4K_SR0] + COEF_SIZE_4K_SR0;
    addr->phy_addr[VDP_COEF_BUF_8K_SR0] = addr->phy_addr[VDP_COEF_BUF_4K_SR1] + COEF_SIZE_4K_SR1;
    addr->phy_addr[VDP_COEF_BUF_8K_SR1] = addr->phy_addr[VDP_COEF_BUF_8K_SR0] + COEF_SIZE_8K_SR0;
    addr->phy_addr[VDP_COEF_BUF_8K_SR2] = addr->phy_addr[VDP_COEF_BUF_8K_SR1] + COEF_SIZE_8K_SR1;
    addr->phy_addr[VDP_COEF_BUF_8K_SR3] = addr->phy_addr[VDP_COEF_BUF_8K_SR2] + COEF_SIZE_8K_SR2;
    addr->phy_addr[VDP_COEF_BUF_DC] = addr->phy_addr[VDP_COEF_BUF_8K_SR3] + COEF_SIZE_8K_SR3;
    addr->phy_addr[VDP_COEF_BUF_REGION_V1] = addr->phy_addr[VDP_COEF_BUF_DC] + COEF_SIZE_DC;
    addr->phy_addr[VDP_COEF_BUF_DIM] = addr->phy_addr[VDP_COEF_BUF_REGION_V1] + COEF_SIZE_REGION_V1;
    addr->phy_addr[VDP_COEF_BUF_DIM_HZME] = addr->phy_addr[VDP_COEF_BUF_DIM] + COEF_SIZE_DIM;
    addr->phy_addr[VDP_COEF_BUF_DIM_VZME] = addr->phy_addr[VDP_COEF_BUF_DIM_HZME] + COEF_SIZE_DIM_HZME;
    addr->phy_addr[VDP_COEF_BUF_DC] = addr->phy_addr[VDP_COEF_BUF_8K_SR3] + COEF_SIZE_8K_SR3;
    addr->phy_addr[VDP_COEF_BUF_REGION_V1] = addr->phy_addr[VDP_COEF_BUF_DC] + COEF_SIZE_DC;
    addr->phy_addr[VDP_COEF_BUF_DIM] = addr->phy_addr[VDP_COEF_BUF_REGION_V1] + COEF_SIZE_REGION_V1;
    addr->phy_addr[VDP_COEF_BUF_DIM_HZME] = addr->phy_addr[VDP_COEF_BUF_DIM] + COEF_SIZE_DIM;
    addr->phy_addr[VDP_COEF_BUF_DIM_VZME] = addr->phy_addr[VDP_COEF_BUF_DIM_HZME] + COEF_SIZE_DIM_HZME;

    return HI_SUCCESS;
}

static hi_s32 cfg_distribute_addr(vdp_coef_addr *vdp_coef_buf_addr)
{
    vdp_para_setparaaddrvhdchn17(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_REGION_V1]);
#ifndef VDP_SDK_PQ_SUPPORT
    vdp_para_setparaaddrvhdchn00(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_V1_DBHDR0]);
    vdp_para_setparaaddrvhdchn01(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_V1_DBHDR1]);
    vdp_para_setparaaddrvhdchn02(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_V1_DBHDR2]);
    vdp_para_setparaaddrvhdchn03(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_V1_DBHDR3]);
    vdp_para_setparaaddrvhdchn04(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_V1_DBHDR4]);
    vdp_para_setparaaddrvhdchn07(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_OM_DEGMM]);
    vdp_para_setparaaddrvhdchn08(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_OM_GMM]);
    vdp_para_setparaaddrvhdchn19(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_OM_CM]);
    vdp_para_setparaaddrv0chn14(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_SHARPEN_FHD]);
    vdp_para_setparaaddrv0chn15(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_8K_DCI0]);
    vdp_para_setparaaddrv0chn01(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_V0_HDR0]);
    vdp_para_setparaaddrv0chn02(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_V0_HDR1]);
    vdp_para_setparaaddrv0chn03(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_V0_HDR2]);
    vdp_para_setparaaddrv0chn04(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_V0_HDR3]);
    vdp_para_setparaaddrv0chn05(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_V0_HDR4]);
    vdp_para_setparaaddrv0chn06(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_V0_HDR5]);
    vdp_para_setparaaddrv0chn07(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_V0_HDR6]);
    vdp_para_setparaaddrv0chn08(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_V0_HDR7]);
    vdp_para_setparaaddrv0chn09(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_V0_HDR8]);
    vdp_para_setparaaddrv0chn26(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_ACM]);
    vdp_para_setparaaddrv0chn00(g_vdp_reg, vdp_coef_buf_addr->phy_addr[VDP_COEF_BUF_DC]);
#endif

    return HI_SUCCESS;
}

hi_void vdp_ip_para_init(hi_void)
{
#ifndef CBB_OSAL_TYPE_SUPPORT
    if (g_buf_addr.vir_addr == HI_NULL) {
        drv_xdp_mem_attr memattr = {
            .mode = OSAL_MMZ_TYPE,
            .is_map_viraddr = HI_TRUE,
            .is_cache = HI_FALSE,
            .size = VDP_ALL_COEF_SIZE,
            .name = "VDP_COEF_DDR",
        };

        if (drv_xdp_mem_alloc(&memattr, &g_buf_addr) != HI_SUCCESS) {
            return;
        }
        g_vdp_coef_buf_addr.vir_addr[VDP_COEF_BUF_8K_DCI0] = g_buf_addr.vir_addr;
        g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_8K_DCI0] = g_buf_addr.phy_addr;
#else
    if (g_buf_addr.pu8StartVirAddr == HI_NULL) {
        if (HI_DRV_MMZ_AllocAndMap("VDP_COEF_DDR", HI_NULL,
                                   VDP_ALL_COEF_SIZE, 0, &g_buf_addr) != HI_SUCCESS) {
            return;
        }
        g_vdp_coef_buf_addr.vir_addr[VDP_COEF_BUF_8K_DCI0] = g_buf_addr.pu8StartVirAddr;
        g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_8K_DCI0] = g_buf_addr.u32StartPhyAddr;

#endif

        vir_addr_distribute(&g_vdp_coef_buf_addr);
        phy_addr_distribute(&g_vdp_coef_buf_addr);
        cfg_distribute_addr(&g_vdp_coef_buf_addr);
    }
}

hi_void vdp_ip_para_deinit(hi_void)
{
#ifndef CBB_OSAL_TYPE_SUPPORT
    if (g_buf_addr.vir_addr != HI_NULL) {
        drv_xdp_mem_free(&g_buf_addr);
        g_buf_addr.vir_addr = HI_NULL;
    }
#else
    if (g_buf_addr.pu8StartVirAddr != HI_NULL) {
        HI_DRV_MMZ_UnmapAndRelease(&g_buf_addr);
        g_buf_addr.pu8StartVirAddr = HI_NULL;
    }
#endif
}

