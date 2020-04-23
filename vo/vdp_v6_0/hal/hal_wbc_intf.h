/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#ifndef __HAL_WBC_INTF_H__
#define __HAL_WBC_INTF_H__

#include "hi_drv_disp.h"
#include "vdp_base_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct tag_wbc_capacity {
    hi_bool support;
    vdp_rect max_read_rect;
    vdp_rect max_write_rect;

    /* format */
    hi_u32 support_format_num;
    hi_drv_pixel_format *support_format;
} wbc_capacity;

wbc_capacity *hal_wbc_get_capacity(hi_u32 wbc);
hi_s32 hal_wbc_get_policy(hi_void *para, hi_void *policy); /* modify arg */
hi_s32 hal_wbc_cfg_process(hi_void *para);                 /* modify arg */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_VO_HAL_H__ */



