/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_diemad.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __DRV_VPSS_STT_DIEMAD_H__
#define __DRV_VPSS_STT_DIEMAD_H__

#include "vpss_comm.h"
#include "hal_vpss_stt_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_DEIMAD_MAX_NODE 2

typedef struct {
    hi_u64 read_addr;
    hi_u8 *read_vir_addr;
    hi_u64 write_addr;
    hi_u8 *write_vir_addr;
    hi_u32 width;
    hi_u32 height;
    hi_u32 stride;
    hi_u32 size;
} vpss_stt_deimad_cfg;

typedef struct {
    hi_bool init;
    hi_u32 cnt;
    vpss_stt_channel_attr dei_mad_chan_attr;
    drv_vpss_mem_info vpss_mem;
    vpss_stt_data data_list[VPSS_DEIMAD_MAX_NODE];
    list *first_ref;
} vpss_stt_deimad;

hi_s32 vpss_stt_deimad_init(vpss_stt_deimad *dei_mad, vpss_stt_comm_attr *comm_attr);
hi_s32 vpss_stt_deimad_deinit(vpss_stt_deimad *dei_mad);
hi_s32 vpss_stt_deimad_get_cfg(vpss_stt_deimad *dei_mad, vpss_stt_deimad_cfg *dei_mad_cfg);
hi_s32 vpss_stt_deimad_complete(vpss_stt_deimad *dei_mad);
hi_s32 vpss_stt_deimad_reset(vpss_stt_deimad *dei_mad);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





