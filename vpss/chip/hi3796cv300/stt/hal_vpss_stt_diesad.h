/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_diesad.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __DRV_VPSS_STT_DIESAD_H__
#define __DRV_VPSS_STT_DIESAD_H__

#include "vpss_comm.h"
#include "hal_vpss_stt_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SD_WIDTH 960

typedef struct {
    hi_u64 read_addr;
    hi_u8 *read_vir_addr;
    hi_u64 write_addr;
    hi_u8 *write_vir_addr;
    hi_u32 width;
    hi_u32 height;
    hi_u32 stride;
    hi_u32 size;
} vpss_stt_deisad_cfg;

typedef struct {
    hi_bool init;
    hi_u32 cnt;
    vpss_stt_channel_attr sad_y_channel_attr;
    vpss_stt_channel_attr sad_c_channel_attr;
    drv_vpss_mem_info vpss_mem;
} vpss_stt_deisad;

hi_s32 vpss_stt_deisad_init(vpss_stt_deisad *dei_sad, vpss_stt_comm_attr *comm_attr);
hi_s32 vpss_stt_deisad_deinit(vpss_stt_deisad *dei_sad);
hi_s32 vpss_stt_deisad_get_cfg(vpss_stt_deisad *dei_sad,
                               vpss_stt_deisad_cfg *dei_sad_y_cfg,
                               vpss_stt_deisad_cfg *dei_sad_c_cfg);
hi_s32 vpss_stt_deisad_complete(vpss_stt_deisad *dei_sad);
hi_s32 vpss_stt_deisad_reset(vpss_stt_deisad *dei_sad);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





