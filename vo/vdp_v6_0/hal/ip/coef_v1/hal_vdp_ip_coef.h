/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_IP_COEF_H__
#define __HAL_VDP_IP_COEF_H__

#include "vdp_chip_define.h"

typedef enum {
    VDP_ZME_ID_HZME_V0 = 0,      /* 0x32600 */
    VDP_ZME_ID_HZME_V1 = 1,      /* 0x3500 */
    VDP_ZME_ID_HZME_GP0_SR = 2,  /* 0x9A000 */
    VDP_ZME_ID_HZME_WD = 3,      /* 0x9880 */
    VDP_ZME_ID_VZME_V0 = 4,      /* 0x32b00 */
    VDP_ZME_ID_VZME_V1 = 5,      /* 0x3600 */
    VDP_ZME_ID_VZME_GP0_SR = 6,  /* 0x9A100 */
    VDP_ZME_ID_VZME_WD = 7,      /* 0x9b00 */
    VDP_ZME_ID_CVFIR = 8,        /* 0x9b00 */
    VDP_ZME_ID_BUTT
} vdo_coef_id;

typedef enum {
    DRV_COEF_DATA_TYPE_U8 = 0,
    DRV_COEF_DATA_TYPE_S8,
    DRV_COEF_DATA_TYPE_U16,
    DRV_COEF_DATA_TYPE_S16,
    DRV_COEF_DATA_TYPE_U32,
    DRV_COEF_DATA_TYPE_S32,
    DRV_COEF_DATA_TYPE_BUTT
} vdp_coef_data_type;

typedef enum {
    VDP_STI_FILE_DATA_VID0 = 0,
    VDP_STI_FILE_DATA_VID1,
    VDP_STI_FILE_DATA_VID2,
    VDP_STI_FILE_DATA_VID3,
    VDP_STI_FILE_DATA_GFX0,
    VDP_STI_FILE_DATA_GFX1,
    VDP_STI_FILE_DATA_GFX2,
    VDP_STI_FILE_DATA_GFX3,

    VDP_STI_FILE_COEF_SR,
    VDP_STI_FILE_COEF_ZME_V0,
    VDP_STI_FILE_COEF_SHARPEN,
    VDP_STI_FILE_COEF_ACM,
    VDP_STI_FILE_COEF_DCI,
    VDP_STI_FILE_COEF_DIM,
    VDP_STI_FILE_COEF_GMM,
    VDP_STI_FILE_COEF_CLUT,
    VDP_STI_FILE_COEF_HZME,
    VDP_STI_FILE_COEF_VZME,
    VDP_STI_FILE_COEF_GP0ZME,
    VDP_STI_FILE_COEF_CLE,
    VDP_STI_FILE_COEF_OD,
    VDP_STI_FILE_COEF_PCID,
    VDP_STI_FILE_COEF_RGBW,
    VDP_STI_FILE_COEF_DEMURA,
    VDP_STI_FILE_COEF_GAMMA,
    VDP_STI_FILE_COEF_SELF_RGBW,
    VDP_STI_FILE_COEF_SELF_DEMURA,
    VDP_STI_FILE_TYPE_NODEF,
    VDP_STI_FILE_V0_Y,
    VDP_STI_FILE_V0_C,
    VDP_STI_FILE_TILE_64_CMP_Y,
    VDP_STI_FILE_TILE_64_CMP_C,
    VDP_STI_FILE_TILE_64_CMP_Y_2BIT,
    VDP_STI_FILE_TILE_64_CMP_C_2BIT,
    VDP_STI_FILE_TILE_64_CMP_Y_HEAD,
    VDP_STI_FILE_TILE_64_CMP_C_HEAD,
    VDP_STI_FILE_OSD_CMP,
    VDP_STI_FILE_DATA_TNR_REF,
    VDP_STI_FILE_DATA_TNR_MAD,
    VDP_STI_FILE_DATA_TNR_RGMV,
    VDP_STI_FILE_TNR_REF_CMP_Y,

    VDP_STI_FILE_DATA_VMX,
    VDP_STI_FILE_COEF_HIHDR_V_D,
    VDP_STI_FILE_COEF_HIHDR_V_S,
    VDP_STI_FILE_COEF_HIHDR_G_TMAP,
    VDP_STI_FILE_COEF_HIHDR_G_GMM,
    VDP_STI_FILE_COEF_G3ZME,
    VDP_STI_FILE_COEF_WD_HZME,
    VDP_STI_FILE_COEF_WD_VZME,
    VDP_STI_FILE_COEF_HIHDR_WD_D,
    VDP_STI_FILE_COEF_HIHDR_WD_S,
    VDP_STI_FILE_COEF_TMAP_V1,
    VDP_STI_FILE_COEF_VHDR_CM,
    VDP_STI_FILE_COEF_CACM,
    VDP_STI_FILE_COEF_TCHDR,
    VDP_STI_FILE_COEF_HIPP_SR,
    VDP_STI_FILE_COEF_FCIC,
    VDP_STI_FILE_COEF_MPLUS,
    VDP_STI_FILE_COEF_DC,
    VDP_STI_FILE_COEF_HDR0,
    VDP_STI_FILE_COEF_HDR1,
    VDP_STI_FILE_COEF_HDR2,
    VDP_STI_FILE_COEF_HDR3,
    VDP_STI_FILE_COEF_HDR4,
    VDP_STI_FILE_COEF_HDR5,
    VDP_STI_FILE_COEF_HDR6,
    VDP_STI_FILE_COEF_HDR7,
    VDP_STI_FILE_COEF_HDR8,

    VDP_STI_FILE_CMP_V0_Y_HEAD,
    VDP_STI_FILE_CMP_V0_Y_DATA,
    VDP_STI_FILE_CMP_V0_Y_EXT,
    VDP_STI_FILE_CMP_V0_C_HEAD,
    VDP_STI_FILE_CMP_V0_C_DATA,
    VDP_STI_FILE_CMP_V0_C_EXT,
    VDP_STI_FILE_CMP_V1_Y_HEAD,
    VDP_STI_FILE_CMP_V1_Y_DATA,
    VDP_STI_FILE_CMP_V1_Y_EXT,
    VDP_STI_FILE_CMP_V1_C_HEAD,
    VDP_STI_FILE_CMP_V1_C_DATA,
    VDP_STI_FILE_CMP_V1_C_EXT,
    VDP_STI_FILE_CMP_G0_AR,
    VDP_STI_FILE_CMP_G0_GB,
    VDP_STI_FILE_CMP_G1_AR,
    VDP_STI_FILE_CMP_G1_GB,
    VDP_STI_FILE_COEF_SLF_HDR,
    VDP_STI_FILE_COEF_CGM,
    VDP_STI_FILE_COEF_DM,
    VDP_STI_FILE_COEF_GDM,
    VDP_STI_FILE_COEF_TONEMAP,
    VDP_STI_FILE_COEF_DM_DEGAMM,
    VDP_STI_FILE_COEF_DM_GAMM,
    VDP_STI_FILE_COEF_METADATA,
    VDP_STI_FILE_COEF_MAC_MDATA,

    VDP_STI_FILE_ESL,
    VDP_STI_FILE_MMU_MAP,
    VDP_STI_FILE_MMU_PAGE,

    VDP_STI_FILE_V0_REGION,
    VDP_STI_FILE_V1_REGION,

    VDP_STI_FILE_TYPE_BUTT
} vdp_coef_file_type;

typedef enum {
    HIPP_CLM_ID_V0_4K = 0,  /* 0x35000 */
    HIPP_CLM_ID_V0_8K = 1,  /* 0x34000 */
    HIPP_CLM_ID_BUTT
} vdp_coef_hipp_clm_id;

typedef struct {
    hi_u32 data3;
    hi_u32 data2;
    hi_u32 data1;
    hi_u32 data0;
    hi_u32 depth;
} vdp_coef_data;

typedef struct {
    hi_u32 phase;
    hi_u32 tap;
    hi_s16 (*typ_lut)[8]; /* 8 is number */
    hi_s16 (*gen_lut)[8]; /* 8 is number */
    hi_s16 *max_val;
    hi_s16 *min_val;
} vdp_cofe_pq;

typedef struct {
    hi_u32 offset;
    hi_u8 *coef_addr;
    hi_u32 zme_y_en;
    hi_u32 zme_c_en;
    hi_u32 zme_dw;
    hi_u32 zme_cw;
    hi_u32 zme_tap_hl;
    hi_u32 zme_tap_hc;
    hi_u32 zme_tap_vl;
    hi_u32 zme_tap_vc;
    hi_u32 zme_coeff_norm;
    hi_u32 zme_phase;
    hi_u32 zme_hl_rat;
    hi_u32 zme_hc_rat;
    hi_u32 zme_vl_rat;
    hi_u32 zme_vc_rat;

    hi_u32 zme_y_coef_fix;
    hi_u32 zme_y_coef_set;
    hi_u32 zme_c_coef_fix;
    hi_u32 zme_c_coef_set;

    hi_u32 max_iw;
    hi_u32 max_ih;
    hi_u32 max_ow;
    hi_u32 max_oh;
} vdp_coef_in_para;

typedef struct {
    hi_u8 *coef_addr;
    vdp_coef_file_type sti_type;
    struct file *fp_coef;
    hi_u32 lut_num;
    hi_u32 burst_num;
    hi_u32 cycle_num;
    void **p_coef_array;
    hi_u32 *lut_length;
    hi_u32 *coef_bit_length;
    vdp_coef_data_type data_type;
} vdp_coef_send_cfg;

hi_u8 *vdp_ip_coef_sendcoef(vdp_coef_send_cfg *cfg);

#endif

