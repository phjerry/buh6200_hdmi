/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_prj.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __DRV_VPSS_STT_PRJ_H__
#define __DRV_VPSS_STT_PRJ_H__

#include "vpss_comm.h"
#include "hal_vpss_stt_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_RGME_MAX_NODE 3

typedef struct {
    hi_u64 read_addr;
    hi_u8 *read_vir_addr;
    hi_u64 write_addr;
    hi_u8 *write_vir_addr;
    hi_u32 width;
    hi_u32 height;
    hi_u32 stride;  // 运动指数行间距
    hi_u32 size;
} vpss_stt_prj_cfg;

typedef struct {
    hi_bool init;
    hi_bool interlace;
    hi_u32 cnt;
    vpss_stt_channel_attr prjv_channel_attr;
    vpss_stt_channel_attr prjh_channel_attr;
    vpss_stt_data data_list[VPSS_RGME_MAX_NODE];
    drv_vpss_mem_info vpss_mem;
    list *first_ref;
} vpss_stt_prj;

// RGME运动信息队列初始化
hi_s32 vpss_stt_prj_init(vpss_stt_prj *prj, vpss_stt_comm_attr *comm_attr);
// RGME运动信息队列去初始化
hi_s32 vpss_stt_prj_deinit(vpss_stt_prj *prj);
// 获取RGME运动信息
hi_s32 vpss_stt_prj_get_cfg(vpss_stt_prj *prj, vpss_stt_prj_cfg *prjh_cfg, vpss_stt_prj_cfg *prjv_cfg);
// RGME运动信息队列轮转
hi_s32 vpss_stt_prj_complete(vpss_stt_prj *prj);
// RGME运动信息队列reset
hi_s32 vpss_stt_prj_reset(vpss_stt_prj *prj);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





