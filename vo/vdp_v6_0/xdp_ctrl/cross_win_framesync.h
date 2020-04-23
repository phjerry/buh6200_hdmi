/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: ion  adapter code
* Author: image
* Create: 2019-03-28
 */

#ifndef CROSS_WIN_FRAMESYNC_H
#define CROSS_WIN_FRAMESYNC_H

typedef enum {
    FRAME_SYNC_PLAY = 0,
    FRAME_SYNC_DISCARD,
    FRAME_SYNC_STOP,
} xdp_ctrl_framesync_opt;

typedef struct hiwin_attr_status {
    hi_handle win_handle;
    HI_RECT_S win_coordinate;
    hi_u16 win_zorder;

    /* this maybe input param, also can be output param. */
    hi_u32 layer_id;
    hi_u32 region_id;
} win_attr_status;

typedef struct hiwin_hdr_status {
    hi_drv_disp_out_type disp_output_hdr_type;
    HI_DRV_COLOR_SPACE_E disp_output_csc_type;
} win_hdr_status;

typedef struct hiwin_zorder_info_s {
    hi_handle win_handle;
    xdp_ctrl_framesync_opt framesync_opt;
    hi_u32 frame_index;
} win_sync_opt;

/*************************extern dependable interface.****************************************/
extern hi_s32 win_buf_get_cur_frameinfo(HI_DRV_VIDEO_FRAME_S *current_frame_info);
extern hi_s32 win_get_total_winnum(hi_u32 *win_num);

/* expected infor should not be  get from xdp_ctrl. since the policy is triggered by  queue-frame/setattr,
  framesync in isr will get the wrong infor. */
extern hi_s32 win_get_expected_attr(win_attr_status *expected_win_map_result,
                                    win_attr_status *using_win_map_result,
                                    hi_u32 win_num);

/* get the display information from display module. */
extern hi_s32 disp_get_expected_output_hdrtype(win_hdr_status *expect_output_hdr_type,
                                               win_hdr_status *using_output_hdr_type);

extern hi_s32 hifb_get_gfx_win_layoutInfor(win_layer_mappingresult *win_map_result);

/*************************end***********************************************************/
hi_bool xdpctrl_is_attr_need_match(win_attr_status *expected_win_attr,
                                   win_attr_status *using_win_attr,
                                   win_attr_status *gfx_current_win_attr,
                                   hi_u32 win_num,
                                   hi_u32 gfx_win_counts);

hi_bool xdpctrl_is_hdr_type_need_match(hi_drv_disp_out_type *expect_output_hdr_type,
                                       hi_drv_disp_out_type *using_output_hdr_type,
                                       hi_drv_hdr_type *frame_hdr_type,
                                       hi_u32 win_num);

/* FIXME: do sync, and return the sync result, include gfx and video. now we do not distinguish the gfx and video. */
hi_bool xdpctrl_do_winbuf_sync(win_attr_status *expected_win_attr,
                               win_attr_status *using_win_attr,
                               win_attr_status *gfx_current_win_attr,
                               hi_u32 win_num,
                               hi_u32 gfx_win_counts,
                               hi_drv_disp_out_type *expect_output_hdr_type,
                               hi_drv_disp_out_type *using_output_hdr_type,
                               hi_drv_hdr_type *frame_hdr_type);
#endif
