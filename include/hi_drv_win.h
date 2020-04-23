/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */
#ifndef __HI_DRV_WIN_H__
#define __HI_DRV_WIN_H__

#include "hi_disp_type.h"
#include "hi_win_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


typedef hi_s32 (*pfn_get_frame_callback)(hi_handle handle, hi_drv_video_frame *frame);
typedef hi_s32 (*pfn_put_frame_callback)(hi_handle handle, hi_drv_video_frame *frame);
typedef hi_s32 (*pfn_get_win_info_callback)(hi_handle handle, hi_drv_win_priv_info *win_priv_info);

hi_s32 hi_drv_win_init(hi_void);
hi_s32 hi_drv_win_deinit(hi_void);

hi_s32 hi_drv_win_create(hi_drv_win_attr *pwinattr, hi_handle *win_handle);
hi_s32 hi_drv_win_destroy(hi_handle win_handle);

hi_s32 hi_drv_win_setattr(hi_handle win_handle, hi_drv_win_attr *pwinattr);
hi_s32 hi_drv_win_getattr(hi_handle win_handle, hi_drv_win_attr *pwinattr);

hi_s32 hi_drv_win_setsource(hi_handle win_handle, hi_drv_win_src_handle *src);
hi_s32 hi_drv_win_getsource(hi_handle win_handle, hi_drv_win_src_handle *src);

hi_s32 hi_drv_win_setenable(hi_handle win_handle, hi_bool enable);
hi_s32 hi_drv_win_getenable(hi_handle win_handle, hi_bool *enable);

hi_s32 hi_drv_win_queueframe(hi_handle win_handle, hi_drv_video_frame *frame);
hi_s32 hi_drv_win_dequeueqframe(hi_handle win_handle, hi_drv_video_frame *frame);

hi_s32 hi_drv_win_getplayinfo(hi_handle win_handle, hi_drv_win_play_info *info);

hi_s32 hi_drv_win_setzorder(hi_handle win_handle, hi_drv_disp_zorder zflag);
hi_s32 hi_drv_win_getzorder(hi_handle win_handle, hi_u32 *pu32zorder);

hi_s32 hi_drv_win_reset(hi_handle win_handle, hi_drv_win_reset_mode reset_mode);
hi_s32 hi_drv_win_freeze(hi_handle win_handle, hi_bool enable, hi_drv_win_freeze_mode freeze_mode);
hi_s32 hi_drv_win_pause(hi_handle win_handle, hi_bool enable);
hi_s32 hi_drv_win_setquick(hi_handle win_handle, hi_bool enable);
hi_s32 hi_drv_win_acquire_captureframe(hi_handle win_handle, hi_drv_video_frame *pstpic);
hi_s32 hi_drv_win_release_captureframe(hi_handle win_handle, hi_drv_video_frame *pic);
hi_s32 hi_drv_win_getlatestframeinfo(hi_handle win_handle, hi_drv_video_frame *frame_info);

hi_s32 hi_drv_win_setextbuffer(hi_handle win_handle, hi_drv_video_buffer_pool *buf);

hi_s32 hi_drv_win_acquireframe(hi_handle win_handle, hi_drv_video_frame *frame_info);
hi_s32 hi_drv_win_releaseframe(hi_handle win_handle, hi_drv_video_frame *frame_info);

hi_s32 hi_drv_win_setrotation(hi_handle win_handle, hi_drv_rot_angle rotation);
hi_s32 hi_drv_win_getrotation(hi_handle win_handle, hi_drv_rot_angle *rotation);

hi_s32 hi_drv_win_setflip(hi_handle win_handle, hi_bool bhoriflip, hi_bool vertflip);
hi_s32 hi_drv_win_getflip(hi_handle win_handle, hi_bool *pbhoriflip, hi_bool *vert_flip);

hi_s32 hi_drv_win_get_ai_result(hi_handle win_handle, hi_drv_win_ai_config *ai_cfg);
hi_s32 hi_drv_win_set_ai_result(hi_handle win_handle, hi_drv_win_ai_config *ai_cfg);

/* for vpss hook function */
hi_s32 hi_drv_win_vpss_complete_frame(hi_handle win_handle, hi_drv_video_frame *frame_info);
hi_s32 hi_drv_win_vpss_queue_frame(hi_handle win_handle, hi_drv_video_frame *image_info);
hi_s32 hi_drv_win_vpss_dequeue_frame(hi_handle win_handle, hi_drv_video_frame *image_info);

hi_s32 hi_drv_win_get_config(hi_handle win_handle, hi_drv_video_frame *videoframe, hi_drv_win_out_config out_cfg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __hi_drv_win_H__ */
