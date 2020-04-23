/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_nrmad.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __HAL_VPSS_STT_NRMAD_H__
#define __HAL_VPSS_STT_NRMAD_H__

#include "vpss_comm.h"
#include "hal_vpss_stt_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_NR_MAX_NODE 3

typedef struct {
    hi_bool init;  // 初始化标识
    hi_u32 cnt;
    hi_bool interlace;
    vpss_stt_channel_attr nr_mad_channel_attr;
    vpss_stt_data data_list[VPSS_NR_MAX_NODE];
    drv_vpss_mem_info vpss_mem;
    list *first_ref;
} vpss_nrmadinfo;

typedef struct {
    hi_u64 tnr_read_addr;
    hi_u8 *tnr_read_vir_addr;
    hi_u64 tnr_write_addr;
    hi_u8 *write_vir_addr;
    hi_u64 snr_read_addr;
    hi_u8 *snr_read_vir_addr;
    hi_u32 width;
    hi_u32 height;
    hi_u32 u32stride;
    hi_u32 size;
} vpss_nrmadcfg;

hi_s32 vpss_stt_nrmad_init(vpss_nrmadinfo *nr_mad_info, vpss_stt_comm_attr *comm_attr);
hi_s32 vpss_stt_nrmad_deinit(vpss_nrmadinfo *nr_mad_info);
hi_s32 vpss_stt_nrmad_get_info(vpss_nrmadinfo *nr_mad_info, vpss_nrmadcfg *nr_mad_cfg);
hi_s32 vpss_stt_nrmad_reset(vpss_nrmadinfo *nr_mad_info);
hi_s32 vpss_stt_nrmad_complete(vpss_nrmadinfo *nr_mad_info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





