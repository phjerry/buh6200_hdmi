/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: ion  adapter code
* Author: image
* Create: 2019-03-28
 */

#ifndef __DRV_XDP_CTRL_H__
#define __DRV_XDP_CTRL_H__
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 drv_xdp_ctrl_init(hi_drv_display disp);
hi_void drv_xdp_ctrl_deinit(hi_drv_display disp);
hi_void drv_xdp_ctrl_proc_info( struct seq_file *p,hi_drv_display disp);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
