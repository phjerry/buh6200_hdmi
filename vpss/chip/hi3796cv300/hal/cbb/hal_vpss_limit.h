/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_limit.h hander file vpss limit
 * Author: zhangjunyu
 * Create: 2016/07/03
 */

#ifndef __HAL_VPSS_LIMIT_H__
#define __HAL_VPSS_LIMIT_H__

#include "vpss_comm.h"
#include "hal_vpss_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_u32 format;  // 0:420 1: 422 2:444 3:400
    hi_u32 prio;
    hi_u32 img2d3d_mode;
    hi_u32 pro;     // 0:interlace 1:progressive
    hi_u32 even;    // 0:odd 1:even
    hi_u32 bfield;  // 0:top 1:bottom
    hi_u32 pre_mult_en;
    hi_u32 comp_valid;  // A/W Y/R U/G V/B

    hi_u32 wth;
    hi_u32 hgt;
} vpss_cfg_info;

typedef enum {
    VPSS_STT_R_ADDR = 0,
    VPSS_STT_W_ADDR,
    VPSS_CHK_SUM_ADDR,
    VPSS_CF_RTUNL_ADDR,
    VPSS_OUT0_WTUNL_ADDR,
    VPSS_CFY_ADDR,
    VPSS_CFC_ADDR,
    VPSS_CFCR_ADDR,
    VPSS_DI_CFY_ADDR,
    VPSS_DI_CFC_ADDR,
    VPSS_DI_CFCR_ADDR,
    VPSS_DI_P1Y_ADDR,
    VPSS_DI_P1C_ADDR,
    VPSS_DI_P2Y_ADDR,
    VPSS_DI_P2C_ADDR,
    VPSS_DI_P3Y_ADDR,
    VPSS_DI_P3C_ADDR,
    VPSS_DI_P3IY_ADDR,
    VPSS_DI_P3IC_ADDR,
    VPSS_CC_PR0Y_ADDR,
    VPSS_CC_PR0C_ADDR,
    VPSS_CC_PR4Y_ADDR,
    VPSS_CC_PR4C_ADDR,
    VPSS_CC_PR8Y_ADDR,
    VPSS_CC_PR8C_ADDR,
    VPSS_NR_REFY_ADDR,
    VPSS_NR_REFC_ADDR,
    VPSS_DI_RSADY_ADDR,
    VPSS_DI_RSADC_ADDR,
    VPSS_DI_RHISM_ADDR,
    VPSS_DI_P1MV_ADDR,
    VPSS_DI_P2MV_ADDR,
    VPSS_DI_P3MV_ADDR,
    VPSS_CC_RCCNT_ADDR,
    VPSS_CC_RYCNT_ADDR,
    VPSS_NR_RMAD_ADDR,
    VPSS_SNR_RMAD_ADDR,
    VPSS_NR_CFMV_ADDR,
    VPSS_NR_P1RGMV_ADDR,
    VPSS_NR_P2RGMV_ADDR,
    VPSS_NR_RCNT_ADDR,
    VPSS_RG_RPRJH_ADDR,
    VPSS_RG_RPRJV_ADDR,
    VPSS_RG_P1RGMV_ADDR,
    VPSS_RG_P2RGMV_ADDR,
    VPSS_OUT0Y_ADDR,
    VPSS_OUT0C_ADDR,
    VPSS_OUT3Y_ADDR,
    VPSS_NR_RFRY_ADDR,
    VPSS_NR_RFRC_ADDR,
    VPSS_NR_RFRHY_ADDR,
    VPSS_NR_RFRHVY_ADDR,
    VPSS_CC_RFRY_ADDR,
    VPSS_CC_RFRC_ADDR,
    VPSS_DI_RFRY_ADDR,
    VPSS_DI_RFRC_ADDR,
    VPSS_NR_WMAD_ADDR,
    VPSS_NR_WCNT_ADDR,
    VPSS_CC_WCCNT_ADDR,
    VPSS_CC_WYCNT_ADDR,
    VPSS_DI_WSADY_ADDR,
    VPSS_DI_WSADC_ADDR,
    VPSS_DI_WHISM_ADDR,
    VPSS_RG_WPRJH_ADDR,
    VPSS_RG_WPRJV_ADDR,
    VPSS_RG_CFRGMV_ADDR,
    VPSS_ME_CFY_ADDR,
    VPSS_ME_REFY_ADDR,
    VPSS_ME_P1MV_ADDR,
    VPSS_ME_PRMV_ADDR,
    VPSS_ME_P1GMV_ADDR,
    VPSS_ME_PRGMV_ADDR,
    VPSS_ME_P1RGMV_ADDR,
    VPSS_ME_P2RGMV_ADDR,
    VPSS_ME_CFMV_ADDR,
    VPSS_ME_CFGMV_ADDR,
    VPSS_ME_STT_W_ADDR,
    LAST_CHN_ADDR
} chn_addr;

typedef enum {
    VPSS_CF_SIZE = 0,
    VPSS_DI_RSADY_SIZE,
    VPSS_DI_RSADC_SIZE,
    VPSS_DI_RHISM_SIZE,
    VPSS_DI_P1MV_SIZE,
    VPSS_NR_RMAD_SIZE,
    VPSS_NR_CFMV_SIZE,
    VPSS_NR_P1RGMV_SIZE,
    VPSS_NR_RCNT_SIZE,
    VPSS_RG_RPRJH_SIZE,
    VPSS_RG_RPRJV_SIZE,
    VPSS_RG_P1RGMV_SIZE,
    VPSS_ME_CF_SIZE,
    VPSS_ME_REF_SIZE,
    VPSS_ME_P1MV_SIZE,
    VPSS_ME_PRMV_SIZE,
    VPSS_ME_P1GMV_SIZE,
    VPSS_ME_PRGMV_SIZE,
    VPSS_ME_P1RGMV_SIZE,
    VPSS_ME_P2RGMV_SIZE,
    LAST_CHN_SIZE
} chn_size;

hi_u32 vpss_hal_logic_cfg_check(vpss_reg_type *reg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





