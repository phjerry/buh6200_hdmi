/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: dci hal layer header file
 * Author: pq
 * Create: 2019-01-01
 */

#ifndef __PQ_HAL_DCI_H__
#define __PQ_HAL_DCI_H__

#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DCI_PARAUP_COEF_OFFSET  64
#define DCI_COEF_SIZE  (4096 * 128 / 8)
#define LUT_BIN_WEIGHT_NUM   32
#define LUT_ADJUST_NUM       33
#define LUT_ADJUST_COEF_NUM  (LUT_ADJUST_NUM * 3)
#define LUT_BS_DELTA_NUM     320
#define LUT_VOFFSET_NUM      160
#define LUT_UV_DIV_NUM       64
#define LUT_SKIN_PRO_NUM     256
#define DCI_HIST_BIN32_NUM   32
#define DCI_SAD_MAX_CNT      7


typedef struct {
    hi_s16 lut0[LUT_BIN_WEIGHT_NUM];
    hi_s16 lut1[LUT_BIN_WEIGHT_NUM];
    hi_s16 lut2[LUT_BIN_WEIGHT_NUM];
} dci_bin_weight_lut;

typedef struct {
    hi_s16 lut0[LUT_ADJUST_NUM];
    hi_s16 lut1[LUT_ADJUST_NUM];
    hi_s16 lut2[LUT_ADJUST_NUM];
} dci_adjust_lut;

typedef struct {
    dci_bin_weight_lut *bin_weight_lut[2]; /* 2: glb_bw/lcl_bw */
    dci_adjust_lut *adjust_lut[2];         /* 2: glb_max/lcl_max */
    hi_s32 *bs_delta_lut;
    hi_s32 *v_offset_lut;
    hi_s16 *uv_div_lut;
    hi_s32 *skin_pro_lut;
} dci_lut;

typedef struct {
    hi_u8 lut0[LUT_BIN_WEIGHT_NUM];
    hi_u8 lut1[LUT_BIN_WEIGHT_NUM];
    hi_u8 lut2[LUT_BIN_WEIGHT_NUM];
} dci_gen_coef_bin_weight;

typedef struct {
    hi_s16 lut[LUT_ADJUST_COEF_NUM];
} dci_gen_coef_adjust;

typedef struct {
    hi_u16 lut[LUT_BS_DELTA_NUM];
} dci_gen_coef_bs_delta;

typedef struct {
    hi_u16 lut[LUT_VOFFSET_NUM];
} dci_gen_coef_voffset;

typedef struct {
    hi_u16 lut[LUT_UV_DIV_NUM];
} dci_gen_coef_uv_div;

typedef struct {
    hi_u8 lut[LUT_SKIN_PRO_NUM];
} dci_gen_coef_skin_pro;

typedef struct {
    dci_gen_coef_bin_weight glb_bw[2]; /* 2: store ori_coef_array and coef_array_new. */
    dci_gen_coef_bin_weight lcl_bw[2]; /* 2: store ori_coef_array and coef_array_new. */
    dci_gen_coef_adjust     lutglb[2]; /* 2: store ori_coef_array and coef_array_new. */
    dci_gen_coef_adjust     lutlcl[2]; /* 2: store ori_coef_array and coef_array_new. */
    dci_gen_coef_bs_delta   lutbsd[2]; /* 2: store ori_coef_array and coef_array_new. */
    dci_gen_coef_voffset    lutvof[2]; /* 2: store ori_coef_array and coef_array_new. */
    dci_gen_coef_uv_div     lutuvd[2]; /* 2: store ori_coef_array and coef_array_new. */
    dci_gen_coef_skin_pro   lutskp[2]; /* 2: store ori_coef_array and coef_array_new. */
} dci_gen_coef_lut;

typedef enum {
    DCI_COEF_BUF_DCI0 = DCI_PARAUP_COEF_OFFSET + 15, /* 15: chn offset. */
    DCI_COEF_BUF_MAX = 201
} dci_coef_buf_type;

typedef struct {
    hi_u32 u32size;
    hi_u8 *vir_addr[DCI_COEF_BUF_MAX];
    hi_u64 phy_addr[DCI_COEF_BUF_MAX];
} pq_dci_coef_addr;

typedef struct {
    hi_u32 width;
    hi_u32 height;
    pq_dci_link_type link_type;

    hi_u32 demo_pos;
} pq_dci_cfg;

typedef struct {
    hi_u32 global_in_hor_end;
    hi_u32 global_in_hor_start;
    hi_u32 global_in_ver_end;
    hi_u32 global_in_ver_start;
    hi_u32 global_out_hor_end;
    hi_u32 global_out_hor_start;
    hi_u32 global_out_ver_end;
    hi_u32 global_out_ver_start;
    hi_u32 local_hor_end;
    hi_u32 local_hor_start;
    hi_u32 local_ver_end;
    hi_u32 local_ver_start;
} pq_dci_coordinate;

hi_s32 pq_hal_dci_distribute_addr(hi_u8 *vir_addr, dma_addr_t phy_addr);
hi_s32 pq_hal_dci_reset_addr(hi_void);

hi_s32 pq_hal_dci_set_en(hi_bool enable);
hi_s32 pq_hal_dci_set_demo_en(hi_bool enable);
hi_s32 pq_hal_dci_set_demo_mode(pq_demo_mode demo_mode);
hi_s32 pq_hal_dci_set_demo_pos(hi_u32 width, hi_u32 height, hi_u32 demo_pos);
hi_s32 pq_hal_dci_set_strength(hi_u32 global_gain0, hi_u32 global_gain1, hi_u32 global_gain2);
hi_s32 pq_hal_dci_update_cfg(pq_dci_cfg *dci_cfg);

hi_s32 pq_hal_dci_get_histgram(hi_pq_dci_histgram *dci_hist, hi_u32 win_size);
hi_s32 pq_hal_dci_get_mean_value(hi_u32 *mean_value);

hi_s32 pq_hal_dci_set_scd_en(hi_bool enable);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

