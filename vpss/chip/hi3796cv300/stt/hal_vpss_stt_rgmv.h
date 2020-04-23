/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_rgmv.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __DRV_VPSS_STT_RGMV_H__
#define __DRV_VPSS_STT_RGMV_H__

#include "vpss_comm.h"
#include "hal_vpss_stt_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_RGME_MAX_NODE 3

typedef struct {
    hi_u64 p2_read_addr;
    hi_u8 *p2_read_vir_addr;
    hi_u64 p1_read_addr;
    hi_u8 *p1_read_vir_addr;
    hi_u64 cur_write_addr;
    hi_u8 *cur_write_vir_addr;
    hi_u32 width;
    hi_u32 height;
    hi_u32 stride;
    hi_u32 size;
} vpss_stt_rgmv_cfg;

typedef struct {
    hi_bool init;
    hi_u32 cnt;
    vpss_stt_channel_attr rgmv_channel_attr;
    vpss_stt_data data_list[VPSS_RGME_MAX_NODE];
    drv_vpss_mem_info vpss_mem;
    list *first_ref;
} vpss_stt_rgmv;

// RGME运动信息队列初始化
hi_s32 vpss_stt_rgmv_init(vpss_stt_rgmv *rgmv, vpss_stt_comm_attr *comm_attr);
// RGME运动信息队列去初始化
hi_s32 vpss_stt_rgmv_deinit(vpss_stt_rgmv *rgmv);
// 获取RGME运动信息
hi_s32 vpss_stt_rgmv_get_cfg(vpss_stt_rgmv *rgmv, vpss_stt_rgmv_cfg *rgme_cfg);
// RGME运动信息队列轮转
hi_s32 vpss_stt_rgmv_complete(vpss_stt_rgmv *rgmv);
// RGME运动信息队列reset
hi_s32 vpss_stt_rgmv_reset(vpss_stt_rgmv *rgmv);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





