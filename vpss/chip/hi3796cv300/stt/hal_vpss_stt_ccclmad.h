/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_ccclmad.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __DRV_VPSS_STT_CCCLMAD_H__
#define __DRV_VPSS_STT_CCCLMAD_H__

#include "vpss_comm.h"
#include "hal_vpss_stt_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_u64 read_addr;
    hi_u8 *read_vir_addr;
    hi_u64 write_addr;
    hi_u8 *write_vir_addr;
    hi_u32 width;
    hi_u32 height;
    hi_u32 stride;  // 运动指数行间距
    hi_u32 size;
} vpss_stt_ccclcnt_cfg;

typedef struct {
    hi_bool init;
    hi_u32 cnt;
    vpss_stt_channel_attr cnt_y_channel_attr;
    vpss_stt_channel_attr cnt_c_channel_attr;
    drv_vpss_mem_info vpss_mem;
    vpss_stt_comm_attr attr;
    hi_u64 buffer_addr;
    hi_u8 *buffer_vir_addr;
} vpss_stt_ccclcnt;

hi_s32 vpss_stt_cccl_cnt_init(vpss_stt_ccclcnt *cccl_cnt, vpss_stt_comm_attr *comm_attr);
hi_s32 vpss_stt_cccl_cnt_deinit(vpss_stt_ccclcnt *cccl_cnt);
hi_s32 vpss_stt_cccl_cnt_get_cfg(vpss_stt_ccclcnt *cccl_cnt,
                                 vpss_stt_ccclcnt_cfg *cccl_cnt_y_cfg, vpss_stt_ccclcnt_cfg *cccl_cnt_c_cfg);
hi_s32 vpss_stt_cccl_cnt_complete(vpss_stt_ccclcnt *cccl_cnt);
hi_s32 vpss_stt_cccl_cnt_reset(vpss_stt_ccclcnt *cccl_cnt);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





