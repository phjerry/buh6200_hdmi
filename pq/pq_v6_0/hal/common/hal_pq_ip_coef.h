/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hdr ip coef api
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __HAL_PQ_IP_COEF_H__
#define __HAL_PQ_IP_COEF_H__

#include <linux/fs.h>
#include "hi_type.h"

typedef enum {
    DRV_COEF_DATA_TYPE_U8 = 0,
    DRV_COEF_DATA_TYPE_S8,
    DRV_COEF_DATA_TYPE_U16,
    DRV_COEF_DATA_TYPE_S16,
    DRV_COEF_DATA_TYPE_U32,
    DRV_COEF_DATA_TYPE_S32,
    DRV_COEF_DATA_TYPE_BUTT
} pq_coef_data_type;

typedef enum {
    PQ_STI_FILE_DATA_VID0 = 0,
    PQ_STI_FILE_DATA_VID1,
    PQ_STI_FILE_DATA_VID2,
    PQ_STI_FILE_DATA_VID3,
    PQ_STI_FILE_DATA_GFX0,
    PQ_STI_FILE_DATA_GFX1,
    PQ_STI_FILE_DATA_GFX2,
    PQ_STI_FILE_DATA_GFX3,

    PQ_STI_FILE_COEF_SR,
    PQ_STI_FILE_COEF_ZME_V0,
    PQ_STI_FILE_COEF_SHARPEN,
    PQ_STI_FILE_COEF_ACM,
    PQ_STI_FILE_COEF_DCI,
    PQ_STI_FILE_COEF_DIM,
    PQ_STI_FILE_COEF_GMM,
    PQ_STI_FILE_COEF_CLUT,
    PQ_STI_FILE_COEF_HZME,
    PQ_STI_FILE_COEF_VZME,
    PQ_STI_FILE_COEF_GP0ZME,
    PQ_STI_FILE_COEF_CLE,
    PQ_STI_FILE_COEF_OD,
    PQ_STI_FILE_COEF_PCID,
    PQ_STI_FILE_COEF_RGBW,
    PQ_STI_FILE_COEF_DEMURA,
    PQ_STI_FILE_COEF_GAMMA,
    PQ_STI_FILE_COEF_SELF_RGBW,
    PQ_STI_FILE_COEF_SELF_DEMURA,
    PQ_STI_FILE_TYPE_NODEF,
    PQ_STI_FILE_V0_Y,
    PQ_STI_FILE_V0_C,
    PQ_STI_FILE_TILE_64_CMP_Y,
    PQ_STI_FILE_TILE_64_CMP_C,
    PQ_STI_FILE_TILE_64_CMP_Y_2BIT,
    PQ_STI_FILE_TILE_64_CMP_C_2BIT,
    PQ_STI_FILE_TILE_64_CMP_Y_HEAD,
    PQ_STI_FILE_TILE_64_CMP_C_HEAD,
    PQ_STI_FILE_OSD_CMP,
    PQ_STI_FILE_DATA_TNR_REF,
    PQ_STI_FILE_DATA_TNR_MAD,
    PQ_STI_FILE_DATA_TNR_RGMV,
    PQ_STI_FILE_TNR_REF_CMP_Y,

    PQ_STI_FILE_DATA_VMX,
    PQ_STI_FILE_COEF_HIHDR_V_D,
    PQ_STI_FILE_COEF_HIHDR_V_S,
    PQ_STI_FILE_COEF_HIHDR_G_TMAP,
    PQ_STI_FILE_COEF_HIHDR_G_GMM,
    PQ_STI_FILE_COEF_G3ZME,
    PQ_STI_FILE_COEF_WD_HZME,
    PQ_STI_FILE_COEF_WD_VZME,
    PQ_STI_FILE_COEF_HIHDR_WD_D,
    PQ_STI_FILE_COEF_HIHDR_WD_S,
    PQ_STI_FILE_COEF_TMAP_V1,
    PQ_STI_FILE_COEF_VHDR_CM,
    PQ_STI_FILE_COEF_CACM,
    PQ_STI_FILE_COEF_TCHDR,
    PQ_STI_FILE_COEF_HIPP_SR,
    PQ_STI_FILE_COEF_FCIC,
    PQ_STI_FILE_COEF_MPLUS,
    PQ_STI_FILE_COEF_DC,
    PQ_STI_FILE_COEF_HDR0,
    PQ_STI_FILE_COEF_HDR1,
    PQ_STI_FILE_COEF_HDR2,
    PQ_STI_FILE_COEF_HDR3,
    PQ_STI_FILE_COEF_HDR4,
    PQ_STI_FILE_COEF_HDR5,
    PQ_STI_FILE_COEF_HDR6,
    PQ_STI_FILE_COEF_HDR7,
    PQ_STI_FILE_COEF_HDR8,

    PQ_STI_FILE_CMP_V0_Y_HEAD,
    PQ_STI_FILE_CMP_V0_Y_DATA,
    PQ_STI_FILE_CMP_V0_Y_EXT,
    PQ_STI_FILE_CMP_V0_C_HEAD,
    PQ_STI_FILE_CMP_V0_C_DATA,
    PQ_STI_FILE_CMP_V0_C_EXT,
    PQ_STI_FILE_CMP_V1_Y_HEAD,
    PQ_STI_FILE_CMP_V1_Y_DATA,
    PQ_STI_FILE_CMP_V1_Y_EXT,
    PQ_STI_FILE_CMP_V1_C_HEAD,
    PQ_STI_FILE_CMP_V1_C_DATA,
    PQ_STI_FILE_CMP_V1_C_EXT,
    PQ_STI_FILE_CMP_G0_AR,
    PQ_STI_FILE_CMP_G0_GB,
    PQ_STI_FILE_CMP_G1_AR,
    PQ_STI_FILE_CMP_G1_GB,
    PQ_STI_FILE_COEF_SLF_HDR,
    PQ_STI_FILE_COEF_CGM,
    PQ_STI_FILE_COEF_DM,
    PQ_STI_FILE_COEF_GDM,
    PQ_STI_FILE_COEF_TONEMAP,
    PQ_STI_FILE_COEF_DM_DEGAMM,
    PQ_STI_FILE_COEF_DM_GAMM,
    PQ_STI_FILE_COEF_METADATA,
    PQ_STI_FILE_COEF_MAC_MDATA,

    PQ_STI_FILE_ESL,
    PQ_STI_FILE_MMU_MAP,
    PQ_STI_FILE_MMU_PAGE,

    PQ_STI_FILE_V0_REGION,
    PQ_STI_FILE_V1_REGION,

    PQ_STI_FILE_TYPE_BUTT
} pq_coef_file_type;

typedef struct {
    hi_u32 data3;
    hi_u32 data2;
    hi_u32 data1;
    hi_u32 data0;
    hi_u32 depth;
} pq_coef_data;

typedef struct {
    hi_u8 *coef_addr;
    pq_coef_file_type sti_type;
    struct file *fp_coef;
    hi_u32 lut_num;
    hi_u32 burst_num;
    hi_u32 cycle_num;
    void **p_coef_array;
    hi_u32 *lut_length;
    hi_u32 *coef_bit_length;
    pq_coef_data_type data_type;
} pq_coef_send_cfg;

/* **test cbb code***** */
typedef enum {
    VDP_RM_COEF_MODE_TYP = 0x0,
    VDP_RM_COEF_MODE_RAN = 0x1,
    VDP_RM_COEF_MODE_MIN = 0x2,
    VDP_RM_COEF_MODE_MAX = 0x3,
    VDP_RM_COEF_MODE_ZRO = 0x4,
    VDP_RM_COEF_MODE_CUS = 0x5,
    VDP_RM_COEF_MODE_UP = 0x6,
    VDP_RM_COEF_MODE_LBL_TYP = 0x7,
    VDP_RM_COEF_MODE_LBL_RAN = 0x8,
    VDP_RM_COEF_MODE_LBL_MIN = 0x9,
    VDP_RM_COEF_MODE_LBL_MAX = 0x10,
    VDP_RM_COEF_MODE_BUTT
} pq_rm_coef_mode;

typedef struct {
    void *p_coef;
    void *p_coef_new;
    pq_coef_data_type coef_data_type;
    hi_u32 length;
    pq_rm_coef_mode coef_data_mode;
    hi_s32 coef_max;
    hi_s32 coef_min;
} pq_coef_gen_cfg;

hi_u8 *pq_send_coef2ddr(pq_coef_send_cfg *cfg);
hi_u32 pq_drv_gen_coef(pq_coef_gen_cfg *cfg);

#endif

