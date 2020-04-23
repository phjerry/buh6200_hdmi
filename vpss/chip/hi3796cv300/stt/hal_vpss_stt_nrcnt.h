/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_nrcnt.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __DRV_VPSS_STT_NRCNT_H__
#define __DRV_VPSS_STT_NRCNT_H__

#include "vpss_comm.h"
#include "hal_vpss_stt_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_bool init;  // 初始化标识
    hi_u32 cnt;
    vpss_stt_channel_attr nr_cnt_channel_attr;
    drv_vpss_mem_info vpss_mem;
} vpss_nrcntinfo;

typedef struct {
    hi_u64 read_addr;
    hi_u8 *read_vir_addr;
    hi_u64 write_addr;
    hi_u8 *write_vir_addr;
    hi_u32 width;
    hi_u32 height;
    hi_u32 u32stride;
    hi_u32 size;
} vpss_nrcntcfg;

hi_s32 vpss_stt_nrcnt_init(vpss_nrcntinfo *nr_cnt_info, vpss_stt_comm_attr *comm_attr);
hi_s32 vpss_stt_nrcnt_deinit(vpss_nrcntinfo *nr_cnt_info);
hi_s32 vpss_stt_nrcnt_complete(vpss_nrcntinfo *nr_cnt_info);
hi_s32 vpss_stt_nrcnt_reset(vpss_nrcntinfo *nr_cnt_info);
hi_s32 vpss_stt_nrcnt_get_info(vpss_nrcntinfo *nr_cnt_info, vpss_nrcntcfg *nr_cnt_cfg);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





