/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: hal_vpss_stt_dmcnt.h hander file
 * Author: zhangjunyu
 * Create: 2019/11/21
 */

#ifndef __HAL_VPSS_STT_DMCNT_H__
#define __HAL_VPSS_STT_DMCNT_H__

#include "vpss_comm.h"
#include "hal_vpss_stt_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_DMCNT_MAX_NODE 2

typedef struct {
    hi_bool init;
    hi_u32 cnt;
    vpss_stt_channel_attr dmcnt_channel_attr;
    vpss_stt_data data_list[VPSS_DMCNT_MAX_NODE];
    drv_vpss_mem_info vpss_mem;
    list *first_ref;
} vpss_dmcnt_info;

typedef struct {
    hi_u64 read_phy_addr;
    hi_u8 *read_vir_addr;

    hi_u64 write_phy_addr;
    hi_u8 *write_vir_addr;

    hi_u32 width;
    hi_u32 height;
    hi_u32 stride;
    hi_u32 size;
} vpss_dmcnt_cfg;

hi_s32 vpss_stt_dmcnt_init(vpss_dmcnt_info *dmcnt_info, vpss_stt_comm_attr *comm_attr);
hi_s32 vpss_stt_dmcnt_deinit(vpss_dmcnt_info *dmcnt_info);
hi_s32 vpss_stt_dmcnt_get_info(vpss_dmcnt_info *dmcnt_info, vpss_dmcnt_cfg *dmcnt_cfg);
hi_s32 vpss_stt_dmcnt_reset(vpss_dmcnt_info *dmcnt_info);
hi_s32 vpss_stt_dmcnt_complete(vpss_dmcnt_info *dmcnt_info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





