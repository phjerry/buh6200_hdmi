/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef _PQ_HAL_GFXHDR_H_
#define _PQ_HAL_GFXHDR_H_

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define GFXHDR_MIN(x, y)           (((x) > (y)) ? (y) : (x))
#define GFXHDR_MAX(x, y)           (((x) > (y)) ? (x) : (y))
#define GFXHDR_CLIP3(low, high, x) (GFXHDR_MAX(GFXHDR_MIN((high), (x)), (low)))

typedef enum {
    PQ_COEF_BUF_GP0_HDR0 = 0,
    PQ_COEF_BUF_GP0_HDR1 = 1,
    PQ_COEF_BUF_GP0_HDR2 = 2,
    PQ_COEF_BUF_GP0_HDR3 = 3,

    PQ_COEF_BUF_MAX,
} pq_gfx_coef_buf;

typedef struct {
    hi_bool enable;
    hi_bool rshift_round_en;
    hi_bool sclut_rd_en;
    hi_bool sslut_rd_en;
    hi_bool tslut_rd_en;
    hi_bool silut_rd_en;
    hi_bool tilut_rd_en;
    hi_bool smc_enable;
    hi_bool rshift_en;
    hi_bool s2u_en;

    hi_u32 c1_expan;
    hi_u32 para_rdata;
    hi_u32 rshift_bit;
} pq_gfxhdr_tmap_cfg;

typedef enum {
    PQ_GFXHDR_CM_POS0 = 0,
    PQ_GFXHDR_CM_POS1,

    PQ_GFXHDR_CM_POS_MAX
} pq_gfxhdr_cm_pos;

typedef enum {
    PQ_GFXHDR_TM_POS0 = 0,
    PQ_GFXHDR_TM_POS1,

    PQ_GFXHDR_TM_POS_MAX
} pq_gfxhdr_tm_pos;

typedef struct {
    hi_u32 u32size;
    hi_u8 *vir_addr[PQ_COEF_BUF_MAX + 1];
    hi_u64 phy_addr[PQ_COEF_BUF_MAX + 1];
} pq_gfx_coef_addr;

hi_s32 pq_hal_get_gfxhdr_cfg(hi_drv_pq_gfx_layer gfx_layer, drv_pq_internal_gfx_hdr_info *hdr_info);
hi_s32 pq_hal_set_gfxhdr_cfg(hi_drv_pq_gfx_layer gfx_layer, drv_pq_internal_gfx_hdr_info *hdr_info);

hi_s32 pq_hal_set_gfxhdr_offset(hi_pq_hdr_offset *hdr_offset_para);

hi_s32 pq_hal_set_gfxhdr_csc_setting(hi_pq_image_param *pic_setting);

hi_s32 pq_hal_set_gfxhdr_tm_curve(hi_pq_hdr_offset *hdr_offset_para);

hi_s32 pq_hal_init_gfxhdr(pq_bin_param *pq_param, hi_bool default_code);

hi_s32 pq_hal_deinit_gfxhdr(hi_void);
hi_s32 pq_hal_get_gfxhdr_cfg_proc(hi_bool *cm_enable, hi_bool *imap_enable, hi_bool *tmap_enable);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
