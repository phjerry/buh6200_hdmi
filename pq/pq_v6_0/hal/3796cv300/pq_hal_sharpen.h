/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: sharpen hal layer header file
 * Author: pq
 * Create: 2019-11-11
 */

#ifndef __PQ_HAL_SHARPEN_H__
#define __PQ_HAL_SHARPEN_H__

#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SHARPEN_PARAUP_COEF_OFFSET  64
#define SHARPEN_REG_DDR_NUM         70

#define SHARPEN_COEF_SIZE     256 /* 256 Bytes. */
#define SHARPEN_DDR_REG_SIZE  (SHARPEN_REG_DDR_NUM * 4) /* 70*4=280 Bytes, 4:sizeof(hi_u32). */
#define SHARPEN_BUF_SIZE      (SHARPEN_COEF_SIZE + SHARPEN_DDR_REG_SIZE)

typedef enum {
    SHARPEN_COEF_BUF_FHD = SHARPEN_PARAUP_COEF_OFFSET + 14, /* 14: chn offset. */
    SHARPEN_COEF_BUF_DDR_REG,
    SHARPEN_COEF_BUF_MAX = 201
} sharp_coef_buf_type;

typedef struct {
    hi_u32 u32size;
    hi_u8 *vir_addr[SHARPEN_COEF_BUF_MAX];
    hi_u64 phy_addr[SHARPEN_COEF_BUF_MAX];
} pq_sharp_coef_addr;


hi_s32 pq_hal_sharp_init_hal(hi_u8 *vir_addr, dma_addr_t phy_addr);
hi_s32 pq_hal_sharp_reset_addr(hi_void);
hi_s32 pq_hal_sharp_update_cfg(hi_void);

hi_s32 pq_hal_sharp_set_en(hi_bool on_or_off);
hi_s32 pq_hal_sharp_set_demo_en(hi_bool on_or_off);
hi_s32 pq_hal_sharp_set_demo_mode(pq_demo_mode demo_mode);
hi_s32 pq_hal_sharp_set_demo_pos(hi_u32 x_pos);
hi_s32 pq_hal_sharp_set_str(hi_u32 peak_ratio, hi_u32 lti_ratio);

hi_s32 pq_hal_sharp_ddr_regread(uintptr_t reg_addr, hi_u32 *reg_value);
hi_s32 pq_hal_sharp_ddr_regwrite(uintptr_t reg_addr, hi_u32 value);

hi_s32 pq_hal_enable_sharp(hi_bool on_off);
hi_s32 pq_hal_set_sharpen_str_reg(hi_u32 data, hi_u32 peak_gain, hi_u32 edge_gain);
hi_s32 pq_hal_enable_sharpen_demo(hi_u32 data, hi_bool on_off);
hi_s32 pq_hal_set_sharpen_demo_mode(hi_u32 data, hi_u32 sharpen_demo_mode);
hi_s32 pq_hal_set_sharpen_demo_pos(hi_u32 data, hi_u32 pos);
hi_s32 pq_hal_get_sharpen_demo_pos(hi_u32 data, hi_u32 *x_pos);
hi_s32 pq_hal_set_sharpen_detec_en(hi_u32 data, hi_bool on_off);
hi_s32 pq_hal_set_sharpen_peak_gain(hi_u32 data, hi_u32 peak_gain);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

