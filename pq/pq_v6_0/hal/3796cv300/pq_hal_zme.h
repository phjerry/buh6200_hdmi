/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq hal zme api define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_HAL_ZME_H__
#define __PQ_HAL_ZME_H__

#include "pq_hal_comm.h"
#include "drv_pq_comm.h"

/* 水平和垂直方向的亮度色度长度各8*17，16bit，7种缩放比例 */
#define ZME_PIXEL_ALIGN      8

extern hi_bool g_pq_sr_enable;

/* -------------------zme convert start--------------- */
typedef enum {
    PQ_HAL_ZME_COEF_1 = 0,
    PQ_HAL_ZME_COEF_E1 = 1,
    PQ_HAL_ZME_COEF_075 = 2,
    PQ_HAL_ZME_COEF_05 = 3,
    PQ_HAL_ZME_COEF_033 = 4,
    PQ_HAL_ZME_COEF_025 = 5,
    PQ_HAL_ZME_COEF_0 = 6,

    PQ_HAL_ZME_COEF_RATIO_MAX
} pq_hal_zme_coef_ratio;

typedef enum {
    HIPP_CLM_ID_V0_4K = 0,  /* 0x35000 */
    HIPP_CLM_ID_V0_8K = 1,  /* 0x34000 */
    HIPP_CLM_ID_BUTT
} pq_zme_coef_hipp_clm_id;

/* 64: addr offset */
typedef enum {
    PQ_ZME_COEF_BUF_RESERVER    = 64 + 14,
    PQ_ZME_COEF_BUF_8K_VZME        = 64 + 10,
    PQ_ZME_COEF_BUF_8K_HZME        = 64 + 11,
    PQ_ZME_COEF_BUF_XDP_2DSCALE_V  = 64 + 12,
    PQ_ZME_COEF_BUF_XDP_2DSCALE_H  = 64 + 13,
    PQ_ZME_COEF_BUF_4K_SR0         = 64 + 19,
    PQ_ZME_COEF_BUF_8K_SR0         = 64 + 21,

    PQ_ZME_COEF_BUF_MAX
} pq_zme_coef_buf;

typedef struct {
    hi_u32 u32size;
    hi_u8 *vir_addr[PQ_ZME_COEF_BUF_MAX + 1];
    hi_u64 phy_addr[PQ_ZME_COEF_BUF_MAX + 1];
} pq_zme_coef_addr;

hi_s32 pq_hal_get_vdp_zme_strategy(hi_drv_pq_zme_layer layer_id, hi_drv_pq_zme_strategy_in *zme_in,
                                   hi_drv_pq_zme_strategy_out *zme_out);

hi_s32 pq_hal_get_zme_coef_addr(hi_drv_pq_zme_layer layer_id, hi_drv_pq_zme_strategy_in *zme_in,
    hi_drv_pq_zme_strategy_out *zme_out);

hi_s32 pq_hal_set_vdp_zme_default(hi_bool on_off);

hi_s32 pq_hal_set_vdp_hdcpen(hi_bool on_off);

hi_void pq_hal_set_vdp_zme_fir_en(hi_bool zme_fir_en);

hi_void pq_hal_set_vdp_zme_med_en(hi_bool zme_mod_en);

hi_s32 pq_hal_set_proc_vdp_zme_strategy(hi_pq_proc_vdpzme_strategy proc_vdp_zme_strategy);

hi_s32 pq_hal_get_proc_vdp_zme_strategy(hi_pq_proc_get_vdpzme_strategy *proc_vdp_zme_strategy);

hi_s32 pq_hal_get_proc_vdp_zme_reso(hi_pq_proc_vdpzme_reso *proc_vdp_zme_reso);

hi_s32 pq_hal_set_zme_stream_id(hi_u32 stream_id);

#endif


