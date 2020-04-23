/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: drv_vpss_out.h hander file vpss out define
 * Author: zhangjunyu
 * Create: 2016/07/01
 */
#ifndef __DRV_VPSS_OUT_H__
#define __DRV_VPSS_OUT_H__

#include "vpss_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_bool vpss_process;
    hi_bool secure;
    hi_drv_pixel_format pixel_format;
    hi_drv_compress_info cmp_info;
    hi_drv_pixel_bitwidth bit_width;
    hi_drv_3d_type video_3d_type;
    hi_u32 height;
    hi_u32 width;
} vpss_out_frame_info;

typedef struct {
    drv_vpss_mem_info buff_info;
    hi_u32 stride_y;
    hi_u32 stride_c;
} vpss_buffer;

typedef struct {
    hi_drv_video_frame out_frame;

    hi_drv_vpss_video_frame out_priv_frame;

    vpss_buffer buffer;
    drv_vpss_mem_info metedata;
    list node;
} vpss_out_node;

typedef struct {
    hi_u32 buf_num;
} vpss_out_attr;

typedef struct {
    hi_bool init;
    vpss_out_attr attr;
    vpss_comm_spin ful_buf_spin;
    vpss_comm_spin empty_buf_spin;
    vpss_out_node out_node[DEF_OUT_NODE_MAX_NUM];

    list empty_frm_list;
    list ful_frm_list;
    list *out_target_1;

    hi_u32 get_total;
    hi_u32 get_success;
    hi_u32 get_success_total;
    hi_u32 rel_total;
    hi_u32 rel_success;
    hi_u32 rel_success_total;
    hi_u32 get_hz;
    hi_u32 get_last;
    hi_u64 start;
} vpss_out;

typedef struct {
    hi_u32 total_numb;
    hi_u32 empty_list_numb;
    hi_u32 ful_list_numb;
    hi_u32 ext_list_numb;
    hi_u32 wait_sink_rls_numb;
    hi_u32 get_total;
    hi_u32 get_success;
    hi_u32 get_success_total;
    hi_u32 rel_total;
    hi_u32 rel_success;
    hi_u32 rel_success_total;
    hi_u32 get_hz;
    hi_u32 out_rate;
} vpss_fb_state;

hi_s32 vpss_out_init(vpss_out *frame_list, vpss_out_attr *attr);
hi_s32 vpss_out_deinit(vpss_out *frame_list);
hi_s32 vpss_out_get_ful_frm(vpss_out *frame_list, hi_drv_video_frame *frame);
hi_s32 vpss_out_rel_ful_frm(vpss_out *frame_list, hi_drv_video_frame *frame);
hi_s32 vpss_out_add_ful_frm_buf(vpss_out *frame_list, vpss_out_node *node);
hi_bool vpss_out_move_target(vpss_out *frame_list);
vpss_out_node *vpss_out_get_empty_buf(vpss_out *frame_list, vpss_out_frame_info *attr);
hi_s32 vpss_out_add_empty_frm_buf(vpss_out *frame_list, vpss_out_node *node);
hi_bool vpss_out_check_empty_buf(vpss_out *frame_list);
hi_s32 vpss_out_reset(vpss_out *frame_list);
hi_s32 vpss_out_get_state(vpss_out *frame_list, vpss_fb_state *state);
hi_s32 vpss_out_show_status(vpss_out *frame_list);
hi_bool vpss_out_check_frame_index_repeat(vpss_out *frame_list, hi_u32 frame_index);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

