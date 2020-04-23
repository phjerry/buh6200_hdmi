/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq zme api define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_HAL_ZME_SET_COEF_H__
#define __PQ_HAL_ZME_SET_COEF_H__

#include "hi_register_vdp.h"
#include "pq_hal_comm.h"
#include "pq_hal_zme.h"
#include "drv_pq_comm.h"

typedef enum {
    PQ_8KZME_TYPE_VID1 = 0,
    PQ_8KZME_TYPE_VID2,
    PQ_8KZME_TYPE_VID3,
    PQ_8KZME_TYPE_VID0,
    PQ_8KZME_TYPE_WBC,

    PQ_8KZME_TYPE_MAX
} pq_8kzme_type;

typedef enum {
    PQ_LAYER_VID1 = 0,
    PQ_LAYER_VID2 = 1,
    PQ_LAYER_VID3 = 2,
    PQ_LAYER_VID0 = 3,
    PQ_LAYER_DC = 4,

    PQ_LAYER_VID_MAX
} pq_zme_layer_vid;

typedef enum {
    PQ_ZME_ID_HZME_V0 = 0,     /* 0x32600 */
    PQ_ZME_ID_HZME_V1 = 1,     /* 0x3500 */
    PQ_ZME_ID_HZME_GP0_SR = 2, /* 0x9A000 */
    PQ_ZME_ID_HZME_WD = 3,     /* 0x9880 */
    PQ_ZME_ID_VZME_V0 = 4,     /* 0x32b00 */
    PQ_ZME_ID_VZME_V1 = 5,     /* 0x3600 */
    PQ_ZME_ID_VZME_GP0_SR = 6, /* 0x9A100 */
    PQ_ZME_ID_VZME_WD = 7,     /* 0x9c00 */
    PQ_ZME_ID_HCDS_WD = 8,     /* 0x9a00 */
    PQ_ZME_ID_CVFIR = 9,       /* 0x9b00 */

    PQ_ZME_ID_MAX
} pq_zme_coef_id;

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
} pq_coef_in_para;

typedef struct {
    hi_u32 phase;
    hi_u32 tap;
    hi_s16 (*typ_lut)[8];  /* 8 is number */
    hi_s16 (*gen_lut)[8];  /* 8 is number */
    hi_s16 *max_val;
    hi_s16 *min_val;
} pq_zme_coef;

hi_void pq_hal_set_4kzme_coef(hi_u32 offset, hi_u8 *coef_addr_h, hi_u8 *coef_addr_v);
hi_void pq_hal_set_8kzme_coef(pq_8kzme_type layer, hi_u8 *coef_addr_h, hi_u8 *coef_addr_v);
hi_void pq_hal_set_4ksr_coef(hi_u32 offset, hi_u8 *addr);
hi_void pq_hal_set_8ksr_coef(hi_u32 offset, hi_u8 *addr);

#endif
