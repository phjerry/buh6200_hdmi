/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_megmv.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __DRV_VPSS_STT_MEGMV_H__
#define __DRV_VPSS_STT_MEGMV_H__

#include "vpss_comm.h"
#include "hal_vpss_stt_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_MEGMV_MAX_NODE 2

typedef struct {
    hi_u64 write_addr;
    hi_u8 *write_vir_addr;
    hi_u64 read_addr;
    hi_u8 *read_vir_addr;
    hi_u32 stride;  // 运动指数行间距
    hi_u32 size;
} vpss_stt_megmv_cfg;

typedef struct {
    hi_bool init;
    hi_u32 cnt;
    hi_u32 stride;
    hi_u32 size;
    vpss_stt_data data_list[VPSS_MEGMV_MAX_NODE];
    drv_vpss_mem_info vpss_mem;
    list *first_ref;
} vpss_stt_megmv;

hi_s32 vpss_stt_megmv_init(vpss_stt_megmv *megmv);
hi_s32 vpss_stt_megmv_deinit(vpss_stt_megmv *megmv);
hi_s32 vpss_stt_megmv_get_cfg(vpss_stt_megmv *megmv, vpss_stt_megmv_cfg *memv_cfg);
hi_s32 vpss_stt_megmv_complete(vpss_stt_megmv *megmv);
hi_s32 vpss_stt_megmv_reset(vpss_stt_megmv *megmv);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





