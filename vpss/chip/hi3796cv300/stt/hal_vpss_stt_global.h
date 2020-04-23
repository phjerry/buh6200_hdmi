/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_global.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __DRV_VPSS_STT_GLOBAL_H__
#define __DRV_VPSS_STT_GLOBAL_H__

#include "vpss_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_STTGLOBAL_SIZE     (20 * 1024)
#define VPSS_STTME_SIZE         (4 * 1024)
#define VPSS_STTGLOBAL_MAX_NODE 2

typedef struct {
    hi_u64 phy_addr;
    hi_u8 *vir_addr;
    list node;
} vpss_sttglobal_data;

typedef enum {
    STT_MODE_GLOBAL = 0,
    STT_MODE_ME,
    NR_MODE_MAX
} vpss_stt_mode;

typedef struct {
    hi_u64 write_phy_addr;
    hi_u8 *write_vir_addr;
    hi_u64 read_phy_addr;
    hi_u8 *read_vir_addr;
    hi_u32 size;
} vpss_stt_global_cfg;

typedef struct {
    hi_bool init;  // 初始化标识
    vpss_stt_mode stt_mode;
    hi_u32 cnt;
    drv_vpss_mem_info vpss_mem;
    vpss_sttglobal_data data_list[VPSS_STTGLOBAL_MAX_NODE];
    list *first_ref;
} vpss_stt_global;

hi_s32 vpss_stt_global_init(vpss_stt_global *wbc, vpss_stt_mode stt_mode);

hi_s32 vpss_stt_global_deinit(vpss_stt_global *wbc);
hi_s32 vpss_stt_global_reset(vpss_stt_global *wbc);
hi_s32 vpss_stt_global_complete(vpss_stt_global *wbc);
hi_s32 vpss_stt_global_get_info(vpss_stt_global *stt, vpss_stt_global_cfg *stt_global_cfg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





