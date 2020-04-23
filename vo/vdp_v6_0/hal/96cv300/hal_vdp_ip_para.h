/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_IP_PARA_H__
#define __HAL_VDP_IP_PARA_H__

#include "vdp_chip_define.h"

typedef enum {
    VDP_COEF_BUF_V1_DBHDR0 = 0,
    VDP_COEF_BUF_V1_DBHDR1 = 1,
    VDP_COEF_BUF_V1_DBHDR2 = 2,
    VDP_COEF_BUF_V1_DBHDR3 = 3,
    VDP_COEF_BUF_V1_DBHDR4 = 4,
    VDP_COEF_BUF_V1_HZME = 5,
    VDP_COEF_BUF_V1_VZME = 6,
    VDP_COEF_BUF_OM_DEGMM = 7,
    VDP_COEF_BUF_OM_GMM = 8,
    VDP_COEF_BUF_WD_HZME = 10,
    VDP_COEF_BUF_WD_VZME0 = 11,
    VDP_COEF_BUF_WD_VZME1 = 12,
    VDP_COEF_BUF_GCHN0_CLUT = 13,
    VDP_COEF_BUF_GCHN1_CLUT = 14,
    VDP_COEF_BUF_GCHN2_CLUT = 15,
    VDP_COEF_BUF_REGION_V1 = 17,
    VDP_COEF_BUF_V2_LBOX = 18,
    VDP_COEF_BUF_OM_CM = 19,
    VDP_COEF_BUF_G3_ZME = 32 + 0,
    VDP_COEF_BUF_GCHN3_CLUT = 32 + 1,
    VDP_COEF_BUF_V3_LBOX = 32 + 2,
    VDP_COEF_BUF_DC = 64 + 0,
    VDP_COEF_BUF_V0_HDR0 = 64 + 1,
    VDP_COEF_BUF_V0_HDR1 = 64 + 2,
    VDP_COEF_BUF_V0_HDR2 = 64 + 3,
    VDP_COEF_BUF_V0_HDR3 = 64 + 4,
    VDP_COEF_BUF_V0_HDR4 = 64 + 5,
    VDP_COEF_BUF_V0_HDR5 = 64 + 6,
    VDP_COEF_BUF_V0_HDR6 = 64 + 7,
    VDP_COEF_BUF_V0_HDR7 = 64 + 8,
    VDP_COEF_BUF_V0_HDR8 = 64 + 9,
    VDP_COEF_BUF_8K_VZME = 64 + 10,
    VDP_COEF_BUF_8K_HZME = 64 + 11,
    VDP_COEF_BUF_XDP_2DSCALE_V = 64 + 12,
    VDP_COEF_BUF_XDP_2DSCALE_H = 64 + 13,
    VDP_COEF_BUF_SHARPEN_FHD = 64 + 14,
    VDP_COEF_BUF_8K_DCI0 = 64 + 15,
    VDP_COEF_BUF_4K_SR0 = 64 + 19,
    VDP_COEF_BUF_4K_SR1 = 64 + 20,
    VDP_COEF_BUF_8K_SR0 = 64 + 21,
    VDP_COEF_BUF_8K_SR1 = 64 + 22,
    VDP_COEF_BUF_8K_SR2 = 64 + 23,
    VDP_COEF_BUF_8K_SR3 = 64 + 24,
    VDP_COEF_BUF_ACM = 64 + 26,
    VDP_COEF_BUF_DFIR0 = 64 + 27,
    VDP_COEF_BUF_DFIR1 = 64 + 28,

    VDP_COEF_BUF_4K_CLE_REG = 29,
    VDP_COEF_BUF_8K_CLE_REG = 30,

    /* tcon para chn distribute, please do not modify */
    VDP_COEF_BUF_DIM = 96 + 0,
    VDP_COEF_BUF_DIM_HZME = 96 + 1,
    VDP_COEF_BUF_DIM_VZME = 96 + 2,
    VDP_COEF_BUF_CGM = 96 + 3,
    VDP_COEF_BUF_GAMMA = 96 + 4,
    VDP_COEF_BUF_PCID = 96 + 5,
    VDP_COEF_BUF_GP0_SR_HZME = 96 + 6,
    VDP_COEF_BUF_GP0_SR_VZME = 96 + 7,
    VDP_COEF_BUF_GP0_ZME = 96 + 8,
    VDP_COEF_BUF_GP0_HDR0 = 96 + 9,
    VDP_COEF_BUF_GP0_HDR1 = 96 + 10,
    VDP_COEF_BUF_GP0_HDR2 = 96 + 11,
    VDP_COEF_BUF_GP0_HDR3 = 96 + 12,
    VDP_COEF_BUF_GP0_HDR4 = 96 + 13,
    VDP_COEF_BUF_MPLUS = 96 + 14,
    VDP_COEF_BUF_OD = 96 + 29,
    VDP_COEF_BUF_SELF_DEMURA = 96 + 30,
    VDP_COEF_BUF_DITHER = 96 + 31,

    VDP_COEF_BUF_ALL = 200,
    VDP_COEF_BUF_BUTT = 201
} vdp_coef_buf;

typedef struct {
    hi_u32 u32size;
    hi_u8 *vir_addr[VDP_COEF_BUF_BUTT + 1];
    hi_u64 phy_addr[VDP_COEF_BUF_BUTT + 1];
} vdp_coef_addr;

extern vdp_coef_addr g_vdp_coef_buf_addr;

hi_void vdp_ip_para_init(hi_void);
hi_void vdp_ip_para_deinit(hi_void);

#endif

