/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drv vi ctrl
 * Author: sdk
 * Create: 2019-12-14
 */

#ifndef __DRV_VI_H__
#define __DRV_VI_H__

#include "hi_osal.h"

#include "vi_comm.h"
#include "vi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 vi_drv_ctrl_create(vi_instance *instance_p, hi_vi_attr *vi_attr_p);
hi_s32 vi_drv_ctrl_destroy(vi_instance *instance_p);
hi_s32 vi_drv_ctrl_attach(vi_instance *instance_p, hi_handle vo_handle);
hi_s32 vi_drv_ctrl_detach(vi_instance *instance_p, hi_handle vo_handle);
hi_s32 vi_drv_ctrl_start(vi_instance *instance_p);
hi_s32 vi_drv_ctrl_stop(vi_instance *instance_p);
hi_s32 vi_drv_ctrl_acquire_frame(vi_instance *instance_p, hi_drv_video_frame *frame_p);
hi_s32 vi_drv_ctrl_release_frame(vi_instance *instance_p, hi_drv_video_frame *frame_p);
hi_s32 vi_drv_ctrl_get_attr(vi_instance *instance_p, hi_vi_attr *vi_attr_p);
hi_s32 vi_drv_ctrl_set_config(vi_instance *instance_p, hi_vi_config *vi_config_p);
hi_s32 vi_drv_ctrl_get_config(vi_instance *instance_p, hi_vi_config *vi_config_p);
hi_s32 vi_drv_ctrl_set_src_quantization_range(vi_instance *instance_p, hi_vi_quantization_range range);
hi_s32 vi_drv_ctrl_update_nstd_info(vi_instance *instance_p, hi_vi_nonstd_info nstd_info);

hi_s32 vi_drv_ctrl_close_path(vi_instance *instance_p);
hi_s32 vi_drv_ctrl_updata_dbg_info(vi_instance *instance_p, hi_vi_dbg_info *dbg_info_p);

hi_s32 vi_drv_ctrl_poll(vi_instance *instance_p);
hi_s32 vi_drv_ctrl_suspend(hi_void);
hi_s32 vi_drv_ctrl_resume(hi_void);
hi_s32 vi_drv_ctrl_init(hi_void);
hi_s32 vi_drv_ctrl_deinit(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_VI_H__ */
