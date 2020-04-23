/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: sr hal header file
 * Author: pq
 * Create: 2019-09-29
 */

#ifndef _PQ_HAL_SR_H_
#define _PQ_HAL_SR_H_

#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/***************************** macro definition ******************************/
#define SR_CFG_BASE_ADDR      0x34000
#define SR_8K_START_ADDR      0x34000
#define SR_4K_START_ADDR      0x35000

#define PQ_8K_SR_SPLIT_MAX    2



typedef struct {
    hi_u32 demo_en[PQ_8K_SR_SPLIT_MAX];
    hi_u32 demo_mode[PQ_8K_SR_SPLIT_MAX];
    hi_u32 demo_pos[PQ_8K_SR_SPLIT_MAX];
    hi_u32 dbsr_demo_en[PQ_8K_SR_SPLIT_MAX];
    hi_u32 dbsr_demo_mode[PQ_8K_SR_SPLIT_MAX];
    hi_u32 dbsr_demo_pos[PQ_8K_SR_SPLIT_MAX];
    hi_u32 c0_graph_split[PQ_8K_SR_SPLIT_MAX];
    hi_u32 c1_graph_split[PQ_8K_SR_SPLIT_MAX];
} pq_8k_sr_split_out;


hi_s32 pq_hal_sr_init(hi_void);
hi_s32 pq_hal_sr_deinit(hi_void);
hi_s32 pq_hal_sr_set_scale_mode(pq_sr_id id, hi_u32 scale_mode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
