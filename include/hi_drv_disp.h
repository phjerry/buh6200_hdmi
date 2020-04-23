/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: define for external use
* Author: vdp
* Create: 2019-06-28
 */

#ifndef __HI_DRV_DISP_H__
#define __HI_DRV_DISP_H__

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_drv_video.h"
#include "hi_drv_module.h"
#include "hi_disp_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 hi_drv_disp_mod_init(hi_void);
hi_void hi_drv_disp_mod_exit(hi_void);

hi_s32 hi_drv_disp_init(hi_void);
hi_s32 hi_drv_disp_deinit(hi_void);
hi_s32 hi_drv_disp_attach(hi_drv_display disp, hi_drv_display slave);
hi_s32 hi_drv_disp_detach(hi_drv_display disp, hi_drv_display slave);
hi_s32 hi_drv_disp_setformat(hi_drv_display disp, hi_drv_disp_fmt encoding_format);
hi_s32 hi_drv_disp_getformat(hi_drv_display disp, hi_drv_disp_fmt *format);
hi_s32 hi_drv_disp_setcustomtiming(hi_drv_display disp, hi_drv_disp_timing *timing);
hi_s32 hi_drv_disp_getcustomtiming(hi_drv_display disp, hi_drv_disp_timing *timing);
hi_s32 hi_drv_disp_addintf(hi_drv_display disp, hi_drv_disp_intf *intf);
hi_s32 hi_drv_disp_delintf(hi_drv_display disp, hi_drv_disp_intf *intf);

hi_s32 hi_drv_disp_open(hi_drv_display disp);

hi_s32 hi_drv_disp_close(hi_drv_display disp);
hi_s32 hi_drv_disp_setenable(hi_drv_display disp, hi_bool enable);
hi_s32 hi_drv_disp_getenable(hi_drv_display disp, hi_bool *enable);
hi_s32 hi_drv_disp_setrighteyefirst(hi_drv_display disp, hi_bool enable);

hi_s32 hi_drv_disp_setbgcolor(hi_drv_display disp, hi_drv_disp_color *bg_color);
hi_s32 hi_drv_disp_getbgcolor(hi_drv_display disp, hi_drv_disp_color *bg_color);

hi_s32 hi_drv_disp_setaspectratio(hi_drv_display disp, hi_u32 u32ratio_h, hi_u32 u32ratio_v);
hi_s32 hi_drv_disp_getaspectratio(hi_drv_display disp, hi_u32 *pu32ratio_h, hi_u32 *pu32ratio_v);

hi_s32 hi_drv_disp_setlayerzorder(hi_drv_display disp, hi_drv_disp_layer layer, hi_drv_disp_zorder zorder_flag);
hi_s32 hi_drv_disp_getlayerzorder(hi_drv_display disp, hi_drv_disp_layer layer, hi_u32 *zorder);

hi_s32 hi_drv_disp_createcast(hi_drv_display disp, hi_drv_disp_cast_cfg *cfg, hi_handle *cast_handle);
hi_s32 hi_drv_disp_destroycast(hi_handle cast_handle);

hi_s32 hi_drv_disp_setcastenable(hi_handle cast_handle, hi_bool enable);
hi_s32 hi_drv_disp_getcastenable(hi_handle cast_handle, hi_bool *enable);

hi_s32 hi_drv_disp_acquirecastframe(hi_handle cast_handle, hi_drv_video_frame *cast_frame);
hi_s32 hi_drv_disp_releasecastframe(hi_handle cast_handle, hi_drv_video_frame *cast_frame);
hi_s32 hi_drv_disp_externlattach(hi_handle cast_handle, hi_handle sink_handle);
hi_s32 hi_drv_disp_externldetach(hi_handle cast_handle, hi_handle sink_handle);

hi_s32 hi_drv_disp_getmaster(hi_drv_display disp, hi_drv_display *master);
hi_s32 hi_drv_disp_get_display_info(hi_drv_display disp, hi_disp_display_info *disp_info);
hi_s32 hi_drv_disp_get_current_timing_status(hi_drv_display disp, hi_drv_disp_timing_status *current_timing_status);

hi_s32 hi_drv_disp_process(hi_u32 cmd, hi_void *arg);
hi_s32 hi_drv_disp_regcallback(hi_drv_display disp, hi_drv_disp_callback_type type,
                               hi_drv_disp_callback *callback);
hi_s32 hi_drv_disp_unregcallback(hi_drv_display disp, hi_drv_disp_callback_type type,
                                         hi_drv_disp_callback *callback);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif
