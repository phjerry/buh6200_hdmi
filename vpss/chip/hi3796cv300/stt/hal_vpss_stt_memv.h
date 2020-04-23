/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_memv.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __HAL_VPSS_STT_MEMV_H__
#define __HAL_VPSS_STT_MEMV_H__

#include "vpss_comm.h"
#include "hal_vpss_stt_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_MEMV_MAX_NODE 4

typedef struct {
    hi_u64 cur_write_addr;  // Cur写地址
    hi_u8 *write_vir_addr;
    hi_u64 p1_read_addr;  // P1读地址
    hi_u8 *p1_read_vir_addr;
    hi_u64 p2_read_addr;  // P2读地址
    hi_u8 *p2_read_vir_addr;
    hi_u64 p3_read_addr;  // P3读地址
    hi_u8 *p3_read_vir_addr;
    hi_u32 width;
    hi_u32 height;
    hi_u32 stride;  // 运动指数行间距
    hi_u32 size;
} vpss_stt_memv_cfg;

typedef struct {
    hi_u64 write_addr;
    hi_u8 *write_vir_addr;
    hi_u64 read_addr;
    hi_u8 *read_vir_addr;
    hi_u32 width;
    hi_u32 height;
    hi_u32 stride;  // 运动指数行间距
    hi_u32 size;
} vpss_stt_memvfortnr_cfg;

typedef struct {
    hi_bool init;
    hi_u32 cnt;
    vpss_stt_channel_attr memv_channel_attr;
    vpss_stt_data data_list[VPSS_MEMV_MAX_NODE];
    drv_vpss_mem_info vpss_mem;
    drv_vpss_mem_info vpss_mem_for_tnr;
    list *first_ref;
} vpss_stt_memv;

hi_s32 vpss_stt_memv_init(vpss_stt_memv *memv, vpss_stt_comm_attr *comm_attr);
hi_s32 vpss_stt_memv_deinit(vpss_stt_memv *memv);
hi_s32 vpss_stt_memv_get_cfg(vpss_stt_memv *memv, vpss_stt_memv_cfg *memv_cfg);
hi_s32 vpss_stt_memv_for_tnr_get_cfg(vpss_stt_memv *memv, vpss_stt_memvfortnr_cfg *memv_for_tnr_cfg);
hi_s32 vpss_stt_memv_complete(vpss_stt_memv *memv);
hi_s32 vpss_stt_memv_reset(vpss_stt_memv *memv);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





