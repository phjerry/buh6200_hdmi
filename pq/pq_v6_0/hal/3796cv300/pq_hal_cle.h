/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: cle hal header file
 * Author: pq
 * Create: 2019-09-21
 */

#ifndef _PQ_HAL_CLE_H_
#define _PQ_HAL_CLE_H_

#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* **************************** macro definition ***************************** */
#define CLE_VP_MAX_NUM 2
#define CLE_PARAUP_COEF_OFFSET 64

#define COEF_SIZE_4K_SR1       (4096 * 128 / 8)
#define COEF_SIZE_8K_SR1       (100 * 128 / 8)
#define COEF_SIZE_4K_CLE_REG   500           /* DDR for loading para to 4k cle reg. */
#define COEF_SIZE_8K_CLE_REG   (500 * 3 * 4) /* DDR for loading para to 8k + 8k-left + 8k-right cle reg. */
#define CLE_COEF_SIZE          (COEF_SIZE_4K_SR1 + COEF_SIZE_8K_SR1 + COEF_SIZE_4K_CLE_REG + COEF_SIZE_8K_CLE_REG)

#define CLE_CFG_BASE_ADDR      0x34200
#define CLE_8K_START_ADDR      0x34200
#define CLE_4K_START_ADDR      0x35200

#define CLE_REG_4K_OFFSET_ADDR  ((CLE_4K_START_ADDR - CLE_CFG_BASE_ADDR) / 4)
#define CLE_REG_8K_OFFSET_ADDR  ((CLE_8K_START_ADDR - CLE_CFG_BASE_ADDR) / 4)
#define CLE_REG_8K_LEFT_OFFSET  (0x500 / 4)
#define CLE_REG_8K_RIGHT_OFFSET (0xA00 / 4)

#define CLE_4K_RAISR_ADDR      0x352d0
#define CLE_4K_RAISR_LSB       0
#define CLE_4K_RAISR_MSB       8
#define CLE_4K_NLSR_ADDR       0x352d4
#define CLE_4K_NLSR_LSB        0
#define CLE_4K_NLSR_MSB        8
#define CLE_4K_PEAK_ADDR       0x35404
#define CLE_4K_PEAK_LSB        22
#define CLE_4K_PEAK_MSB        31

#define CLE_8K_RAISR_ADDR      0x342d0
#define CLE_8K_RAISR_LSB       0
#define CLE_8K_RAISR_MSB       8
#define CLE_8K_NLSR_ADDR       0x342d4
#define CLE_8K_NLSR_LSB        0
#define CLE_8K_NLSR_MSB        8
#define CLE_8K_PEAK_ADDR       0x34404
#define CLE_8K_PEAK_LSB        22
#define CLE_8K_PEAK_MSB        31

#define CLE_COLOR_LUT_ROW_NUM  16
#define CLE_COLOR_LUT_COL_NUM  16

/* ************************** structure definition *************************** */
typedef struct {
    hi_bool       is_init;
    pq_cle_reso   cle_reso[CLE_ID_MAX];
    hi_u32        strength;
    hi_bool       demo_en;
    pq_demo_mode  demo_mode;
    hi_u32        demo_pos;
} alg_cle;

typedef enum {
    CLE_MODE_TYPICAL = 0,
    CLE_MODE_RAND_COEF,
    CLE_MODE_PARA_MAX,
    CLE_MODE_PARA_MIN,
    CLE_MODE_SCENE,
    CLE_MODE_MAX
} cle_mode;

typedef enum {
    CLE_COEF_BUF_4K_SR1 = CLE_PARAUP_COEF_OFFSET + 20,
    CLE_COEF_BUF_8K_SR1 = CLE_PARAUP_COEF_OFFSET + 22,

    CLE_COEF_BUF_4K_REG = 29,
    CLE_COEF_BUF_8K_REG = 30,

    CLE_COEF_BUF_MAX = 201
} cle_coef_buf_type;

typedef struct {
    hi_u32 u32size;
    hi_u8 *vir_addr[CLE_COEF_BUF_MAX];
    hi_u64 phy_addr[CLE_COEF_BUF_MAX];
} pq_cle_coef_addr;

typedef struct {
    hi_u32 cle_ck_gt_en;
    hi_u32 cle_en;
    hi_u32 demo_en;
    hi_u32 demo_pos;
    hi_u32 demo_mode;
    hi_u32 lti_en;
    hi_u32 lti_limit_clip_en;
    hi_u32 peak_limit_clip_en;
    hi_u32 img_width;
    hi_u32 img_height;
} cle_attr_cfg;

typedef struct {
    hi_u32 demo_en;
    hi_u32 demo_pos;
    hi_u32 demo_mode;
    hi_u32 iw;
    hi_u32 facepos0[20]; /* 20 :a number */
    hi_u32 facepos2[20]; /* 20 :a number */
} cle_split_cfg;

typedef struct {
    hi_u32 demo_en[2];      /* 2 :a number */
    hi_u32 demo_pos[2];     /* 2 :a number */
    hi_u32 demo_mode[2];    /* 2 :a number */
    hi_u32 facepos0[20][2]; /* 20 :a number 2 :a number */
    hi_u32 facepos2[20][2]; /* 20 :a number 2 :a number */
} cle_split_out;

typedef struct {
    hi_u32 lti_limit_clip_en;
    hi_u32 cle_en;
    hi_u32 demo_pos;
    hi_u32 demo_mode;
    hi_u32 demo_en;
    hi_u32 shp_clut_rd_en;
    hi_u32 clut_addr;
    hi_u32 clut_rdata;
    hi_u32 face_num;
    hi_u32 face_confidence_en;
    hi_u32 cle_ck_gt_en;
    hi_u32 peak_limit_clip_en;
    hi_u32 contrastbit;
    hi_u32 contrastthr;
    hi_u32 flatbit;
    hi_u32 flatthrl;
    hi_u32 a1slp;
    hi_u32 kneg;
    hi_u32 a1thrl;
    hi_u32 gradmonoslp;
    hi_u32 gradmonomin;
    hi_u32 gradmonomax;
    hi_u32 pixmonothr;
    hi_u32 ltiratio;
    hi_u32 ltigain;
    hi_u32 stcv_sclwgtratio;
    hi_u32 skin_yvalue_mixratio;
    hi_u32 facepos0_1;
    hi_u32 facepos0_0;
    hi_u32 facepos0_4;
    hi_u32 facepos0_3;
    hi_u32 facepos0_2;
    hi_u32 facepos1_1;
    hi_u32 facepos1_0;
    hi_u32 facepos1_4;
    hi_u32 facepos1_3;
    hi_u32 facepos1_2;
    hi_u32 facepos2_1;
    hi_u32 facepos2_0;
    hi_u32 facepos2_4;
    hi_u32 facepos2_3;
    hi_u32 facepos2_2;
    hi_u32 facepos3_1;
    hi_u32 facepos3_0;
    hi_u32 facepos3_4;
    hi_u32 facepos3_3;
    hi_u32 facepos3_2;
    hi_u32 facepos4_1;
    hi_u32 facepos4_0;
    hi_u32 facepos4_4;
    hi_u32 facepos4_3;
    hi_u32 facepos4_2;
    hi_u32 facepos5_1;
    hi_u32 facepos5_0;
    hi_u32 facepos5_4;
    hi_u32 facepos5_3;
    hi_u32 facepos5_2;
    hi_u32 facepos6_1;
    hi_u32 facepos6_0;
    hi_u32 facepos6_4;
    hi_u32 facepos6_3;
    hi_u32 facepos6_2;
    hi_u32 facepos7_1;
    hi_u32 facepos7_0;
    hi_u32 facepos7_4;
    hi_u32 facepos7_3;
    hi_u32 facepos7_2;
    hi_u32 facepos8_1;
    hi_u32 facepos8_0;
    hi_u32 facepos8_4;
    hi_u32 facepos8_3;
    hi_u32 facepos8_2;
    hi_u32 facepos9_1;
    hi_u32 facepos9_0;
    hi_u32 facepos9_4;
    hi_u32 facepos9_3;
    hi_u32 facepos9_2;
    hi_u32 facepos10_1;
    hi_u32 facepos10_0;
    hi_u32 facepos10_4;
    hi_u32 facepos10_3;
    hi_u32 facepos10_2;
    hi_u32 facepos11_1;
    hi_u32 facepos11_0;
    hi_u32 facepos11_4;
    hi_u32 facepos11_3;
    hi_u32 facepos11_2;
    hi_u32 facepos12_1;
    hi_u32 facepos12_0;
    hi_u32 facepos12_4;
    hi_u32 facepos12_3;
    hi_u32 facepos12_2;
    hi_u32 facepos13_1;
    hi_u32 facepos13_0;
    hi_u32 facepos13_4;
    hi_u32 facepos13_3;
    hi_u32 facepos13_2;
    hi_u32 facepos14_1;
    hi_u32 facepos14_0;
    hi_u32 facepos14_4;
    hi_u32 facepos14_3;
    hi_u32 facepos14_2;
    hi_u32 facepos15_1;
    hi_u32 facepos15_0;
    hi_u32 facepos15_4;
    hi_u32 facepos15_3;
    hi_u32 facepos15_2;
    hi_u32 facepos16_1;
    hi_u32 facepos16_0;
    hi_u32 facepos16_4;
    hi_u32 facepos16_3;
    hi_u32 facepos16_2;
    hi_u32 facepos17_1;
    hi_u32 facepos17_0;
    hi_u32 facepos17_4;
    hi_u32 facepos17_3;
    hi_u32 facepos17_2;
    hi_u32 facepos18_1;
    hi_u32 facepos18_0;
    hi_u32 facepos18_4;
    hi_u32 facepos18_3;
    hi_u32 facepos18_2;
    hi_u32 facepos19_1;
    hi_u32 facepos19_0;
    hi_u32 facepos19_4;
    hi_u32 facepos19_3;
    hi_u32 facepos19_2;
} cle_ip_cfg;

hi_u32 pq_hal_cle_ddr_regread(uintptr_t reg_addr);
hi_s32 pq_hal_cle_ddr_regwrite(uintptr_t reg_addr, hi_u32 value);

hi_void pq_hal_set_4k_cle_reg_vir_addr(uintptr_t vir_addr);
hi_u64 pq_hal_get_4k_cle_reg_vir_addr(hi_void);

hi_void pq_hal_set_8k_cle_reg_vir_addr(uintptr_t vir_addr);
hi_u64 pq_hal_get_8k_cle_reg_vir_addr(hi_void);

hi_s32 pq_hal_cle_reset_addr(hi_void);
hi_s32 pq_hal_cle_distribute_addr(hi_u8 *vir_addr, dma_addr_t phy_addr);
hi_s32 pq_hal_cle_update_cfg(const alg_cle *cle_cfg);

hi_s32 pq_hal_cle_set_strength(pq_cle_id id, hi_u32 strength, hi_u32 table_raisr, hi_u32 table_nlsr, hi_u32 table_peak);

hi_s32 pq_hal_cle_set_demo_en(hi_bool enable);
hi_s32 pq_hal_cle_set_demo_mode(pq_demo_mode demo_mode);
hi_s32 pq_hal_set_cle_demo_pos(hi_u32 x_pos);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
