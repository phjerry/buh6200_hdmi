/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description:
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_MNG_GFXZME_H__
#define __PQ_MNG_GFXZME_H__

#include "drv_pq_comm.h"
#include "pq_hal_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* **************************** macro definition ***************************** */
/*
 * ZME COEF
 * 处理水平精度查询
 * 98MV100/HIFONE-STB: 水平精度查询是按照20bit
 * 其它: 水平精度查询是按照12bit
 * 本来是按照12bit计算的，按照这个再进行偏移
 */
#define PQ_PRECISION_GZMERATIONH_12BITOFFSET 8

#define PQ_G_HZME_PRECISION (1<<20)
#define PQ_G_VZME_PRECISION (1<<12)

#define GZME_WIDTH_MAX     4096
#define GZME_WIDTH_RATIO_80    (GZME_WIDTH_MAX * 80 / 100)
#define GZME_WIDTH_RATIO_85    (GZME_WIDTH_MAX * 85 / 100)
#define GZME_WIDTH_RATIO_75    (GZME_WIDTH_MAX * 75 / 100)
#define GZME_WIDTH_RATIO_50     (GZME_WIDTH_MAX * 50 / 100)
#define GZME_WIDTH_RATIO_33     (GZME_WIDTH_MAX * 33 / 100)
#define GZME_WIDTH_RATIO_25     (GZME_WIDTH_MAX * 25 / 100)
#define GZME_WIDTH_RATIO_0       0

#define GZME_PIXEL_ALIGN      2
#define GZME_HOR_TABLE_SIZE      (8 * 8)
#define GZME_VER_TABLE_SIZE      (8 * 16)

#define GZME_COEF_BASE_SIZE      256
#define GZME_HOR_COEF_SIZE     (64)
#define GZME_VER_COEF_SIZE      (128)

#define GZME_COEF_BIT_SIZE 12
/* ************************** structure definition *************************** */
typedef struct {
    hi_u64 zme_coef_addr_hl_1;
    hi_u64 zme_coef_addr_vl_1;
    hi_u64 zme_coef_addr_hl_e1;
    hi_u64 zme_coef_addr_vl_e1;
    hi_u64 zme_coef_addr_hl_075;
    hi_u64 zme_coef_addr_vl_075;
    hi_u64 zme_coef_addr_hl_05;
    hi_u64 zme_coef_addr_vl_05;
    hi_u64 zme_coef_addr_hl_033;
    hi_u64 zme_coef_addr_vl_033;
    hi_u64 zme_coef_addr_hl_025;
    hi_u64 zme_coef_addr_vl_025;
    hi_u64 zme_coef_addr_hl_0;
    hi_u64 zme_coef_addr_vl_0;

    hi_u64 zme_coef_addr_hc_1;
    hi_u64 zme_coef_addr_vc_1;
    hi_u64 zme_coef_addr_hc_e1;
    hi_u64 zme_coef_addr_vc_e1;
    hi_u64 zme_coef_addr_hc_075;
    hi_u64 zme_coef_addr_vc_075;
    hi_u64 zme_coef_addr_hc_05;
    hi_u64 zme_coef_addr_vc_05;
    hi_u64 zme_coef_addr_hc_033;
    hi_u64 zme_coef_addr_vc_033;
    hi_u64 zme_coef_addr_hc_025;
    hi_u64 zme_coef_addr_vc_025;
    hi_u64 zme_coef_addr_hc_0;
    hi_u64 zme_coef_addr_vc_0;

    hi_u64 zme_sd_coef_addr_hl_1;
    hi_u64 zme_sd_coef_addr_vl_1;
    hi_u64 zme_sd_coef_addr_hl_e1;
    hi_u64 zme_sd_coef_addr_vl_e1;
    hi_u64 zme_sd_coef_addr_hl_075;
    hi_u64 zme_sd_coef_addr_vl_075;
    hi_u64 zme_sd_coef_addr_hl_05;
    hi_u64 zme_sd_coef_addr_vl_05;
    hi_u64 zme_sd_coef_addr_hl_033;
    hi_u64 zme_sd_coef_addr_vl_033;
    hi_u64 zme_sd_coef_addr_hl_025;
    hi_u64 zme_sd_coef_addr_vl_025;
    hi_u64 zme_sd_coef_addr_hl_0;
    hi_u64 zme_sd_coef_addr_vl_0;

    hi_u64 zme_sd_coef_addr_hc_1;
    hi_u64 zme_sd_coef_addr_vc_1;
    hi_u64 zme_sd_coef_addr_hc_e1;
    hi_u64 zme_sd_coef_addr_vc_e1;
    hi_u64 zme_sd_coef_addr_hc_075;
    hi_u64 zme_sd_coef_addr_vc_075;
    hi_u64 zme_sd_coef_addr_hc_05;
    hi_u64 zme_sd_coef_addr_vc_05;
    hi_u64 zme_sd_coef_addr_hc_033;
    hi_u64 zme_sd_coef_addr_vc_033;
    hi_u64 zme_sd_coef_addr_hc_025;
    hi_u64 zme_sd_coef_addr_vc_025;
    hi_u64 zme_sd_coef_addr_hc_0;
    hi_u64 zme_sd_coef_addr_vc_0;
} pq_gzme_coef_addr;

typedef struct {
    drv_pq_mem_info m_buf; /* common drv */
    pq_gzme_coef_addr zme_coef_addr;
} pq_gzme_mem;

typedef enum {
    PQ_GP_ORDER_NULL = 0x0,
    PQ_GP_ORDER_CSC = 0x1,
    PQ_GP_ORDER_ZME = 0x2,
    PQ_GP_ORDER_CSC_ZME = 0x3,
    PQ_GP_ORDER_ZME_CSC = 0x4,

    PQ_GP_ORDER_MAX
} pq_gp_order;

typedef struct {
    hi_s32 bits_0 : 10;
    hi_s32 bits_1 : 10;
    hi_s32 bits_2 : 10;
    hi_s32 bits_32 : 2;
    hi_s32 bits_38 : 8;
    hi_s32 bits_4 : 10;
    hi_s32 bits_5 : 10;
    hi_s32 bits_64 : 4;
    hi_s32 bits_66 : 6;
    hi_s32 bits_7 : 10;
    hi_s32 bits_8 : 10;
    hi_s32 bits_96 : 6;
    hi_s32 bits_94 : 4;
    hi_s32 bits_10 : 10;
    hi_s32 bits_11 : 10;
    hi_s32 bits_12 : 8;
} pq_gzme_coef_bit;

typedef struct {
    hi_u32 size;
    pq_gzme_coef_bit bit[GZME_COEF_BIT_SIZE];
} pq_gzme_coef_bitarray;

typedef struct {
    hi_bool gfx_zme_init_flag;
    hi_bool gfx_zme_fir_mode;
    hi_bool gfx_zme_med_en;
} gfx_zme_mode_proc;

hi_s32 pq_mng_register_gfx_zme(hi_void);
hi_s32 pq_mng_unregister_gfx_zme(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


