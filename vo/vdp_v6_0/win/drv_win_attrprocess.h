/*
 * * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * * Description: header file of win_attrprocess Component
 * * Author: image team
 * * Create: 2019-07-2
 * *
 */
#ifndef  __DRV_WIN_ATTRPROCESS__
#define  __DRV_WIN_ATTRPROCESS__

#include "hi_type.h"
#include "hi_drv_disp.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_u32              src_width;
    hi_u32              src_height;
    hi_rect             src_crop_rect;
    hi_drv_aspect_ratio src_asp_ratio;
    hi_drv_video_afd_type src_afd_type;
    hi_bool             src_active_format_flag;

    hi_rect             out_physics_screen;
    hi_rect             out_reference_screen;
    hi_drv_aspect_ratio out_device_asp_ratio;

    // 输出窗口的大小
    hi_rect             win_out_rect;
    hi_bool             is_phy_coordinate;
    hi_drv_asp_rat_mode win_out_asp_mode;
} drv_win_attr_cfg;


typedef struct {
    hi_rect                     src_crop_rect;
    hi_rect                     win_video_rect;
    hi_rect                     win_out_rect;
    hi_bool                     is_smaller_than_minimum;
} drv_win_attr_out_config;



hi_s32 drv_win_attrprocess_init(void);
hi_s32 drv_win_attrprocess_deinit(void);

hi_handle drv_win_attrprocess_create(void);
hi_s32 drv_win_attrprocess_destory(hi_handle h_attr);

hi_s32 drv_win_attrprocess_update(hi_handle hi_attr, drv_win_attr_cfg *attr_cfg,
    drv_win_attr_out_config *out_cfg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

