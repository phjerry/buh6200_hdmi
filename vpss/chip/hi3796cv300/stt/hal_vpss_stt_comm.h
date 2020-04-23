/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_comm.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __DRV_VPSS_STT_COMM_H__
#define __DRV_VPSS_STT_COMM_H__

#include "vpss_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_u8 *vir_addr;
    hi_u64 phy_addr;
    list node;
} vpss_stt_data;

typedef struct {
    hi_u32 width;
    hi_u32 height;
    hi_u32 stride;
    hi_u32 size;
} vpss_stt_channel_attr;

typedef struct {
    hi_u64 phy_addr;
    hi_u8 *vir_addr;
    hi_u32 width;
    hi_u32 height;
    hi_u32 stride;
    hi_u32 size;
} vpss_stt_channel_cfg;

typedef struct {
    hi_bool interlace;
    hi_u32 width;
    hi_u32 height;
    hi_bool secure;
    hi_drv_pixel_format pixel_format;
} vpss_stt_comm_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





