/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: vpss policy
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#include "vpss_policy.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_MAX_HEIGHT       4608
#define VPSS_MIN_HEIGHT       64
#define VPSS_MAX_WIDTH        8192
#define VPSS_MIN_WIDTH        64
#define vpss_align_down(x, a) ((x) - (x) % (a))
#define vpss_align_up(x, a)   ((((x) + (a)-1) / (a)) * (a))

static hi_drv_pixel_format g_input_pix_fmt_capacity[] = {
    HI_DRV_PIXEL_FMT_NV12,
    HI_DRV_PIXEL_FMT_NV21,
    HI_DRV_PIXEL_FMT_NV16,
    HI_DRV_PIXEL_FMT_NV61,
    HI_DRV_PIXEL_FMT_NV16_2X1,
    HI_DRV_PIXEL_FMT_NV61_2X1,
    HI_DRV_PIXEL_FMT_NV42,     /* vpss loggic can't support, trans to vdp */
    HI_DRV_PIXEL_FMT_NV24,     /* vpss loggic can't support, trans to vdp */
    HI_DRV_PIXEL_FMT_NV42_RGB, /* vpss loggic can't support, trans to vdp */
    HI_DRV_PIXEL_FMT_ARGB8888, /* vpss loggic can't support, trans to vdp */
    HI_DRV_PIXEL_FMT_RGB24     /* vpss loggic can't support, trans to vdp */
};
static hi_drv_pixel_format g_output_pix_fmt_capacity[] = {
    HI_DRV_PIXEL_FMT_NV12,
    HI_DRV_PIXEL_FMT_NV21,
    HI_DRV_PIXEL_FMT_NV16,
    HI_DRV_PIXEL_FMT_NV61,
    HI_DRV_PIXEL_FMT_NV16_2X1,
    HI_DRV_PIXEL_FMT_NV61_2X1,
    HI_DRV_PIXEL_FMT_NV42,     /* vpss loggic can't support, trans to vdp */
    HI_DRV_PIXEL_FMT_NV24,     /* vpss loggic can't support, trans to vdp */
    HI_DRV_PIXEL_FMT_NV42_RGB, /* vpss loggic can't support, trans to vdp */
    HI_DRV_PIXEL_FMT_ARGB8888, /* vpss loggic can't support, trans to vdp */
    HI_DRV_PIXEL_FMT_RGB24,    /* vpss loggic can't support, trans to vdp */
};

hi_bool vpss_policy_support_height_and_width(hi_u32 width, hi_u32 height)
{
    if ((width > VPSS_MAX_WIDTH) || (width < VPSS_MIN_HEIGHT)) {
        vpss_error("VPSS can't be processed w: %d, [%d,%d] \n",
                   width, VPSS_MIN_HEIGHT, VPSS_MAX_WIDTH);
        return HI_FALSE;
    }

    if ((height > VPSS_MAX_HEIGHT) || (height < VPSS_MIN_HEIGHT)) {
        vpss_error("VPSS can't be processed h: %d, [%d,%d] \n",
                   height, VPSS_MIN_HEIGHT, VPSS_MAX_HEIGHT);
        return HI_FALSE;
    }

    return HI_TRUE;
}

hi_s32 vpss_policy_support_cccl_height_and_width(hi_u32 *width, hi_u32 *height)
{
    *width = 960;  /* reso 960:para */
    *height = 576; /* reso 576:para */
    return HI_SUCCESS;
}

hi_bool vpss_policy_check_special_field(hi_drv_vpss_video_frame *image)
{
    if ((image->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_INTERLACE) &&
        (image->comm_frame.field_mode != HI_DRV_FIELD_ALL) &&
        (image->comm_frame.buf_valid_mode == HI_DRV_FIELD_ALL)) {
        return HI_TRUE;
    }

    return HI_FALSE;
}


hi_bool vpss_policy_support_out0_zme(hi_void)
{
    return HI_TRUE;
}

hi_bool vpss_policy_support_out1_zme(hi_void)
{
    return HI_TRUE;
}
/* todo : npu : out2 */
hi_bool vpss_policy_support_out2_zme(hi_void)
{
    return HI_FALSE;
}
/* logic is DZME, mean draw zme */
hi_bool vpss_policy_support_pzme(hi_void)
{
    return HI_FALSE;
}

hi_bool vpss_policy_support_zme_upsamp(hi_void)
{
    return HI_TRUE; /* zme support 420 <-->422 */
}

hi_bool vpss_policy_support_input_crop(hi_void)
{
    return HI_FALSE;
}

hi_bool vpss_policy_support_output_crop(hi_void)
{
    return HI_TRUE;
}


hi_bool vpss_policy_support_lbx(hi_void)
{
    return HI_TRUE;
}

hi_bool vpss_policy_check_need_trans(hi_drv_vpss_video_frame *frame, vpss_instance *instance)
{
    hi_drv_pixel_format pixel_format = frame->comm_frame.pixel_format;
    hi_drv_hdr_type src_hdr_type = frame->comm_frame.hdr_type;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;
    hi_bool support_zme = HI_FALSE;

    meta_info = (hi_drv_win_vpss_meta_info *)frame->comm_frame.video_private.win_private.data;
    if (instance->work_mode == HI_DRV_VPSS_WORKMODE_BYPASS) {
        meta_info->vpss_process = HI_FALSE;
        return !meta_info->vpss_process;
    }

    meta_info->vpss_process = HI_TRUE;

    if (src_hdr_type == HI_DRV_HDR_TYPE_DOLBYVISION) {
        meta_info->vpss_process = HI_FALSE;
    }

    if ((pixel_format == HI_DRV_PIXEL_FMT_NV42) || (pixel_format == HI_DRV_PIXEL_FMT_NV24)
        || (pixel_format == HI_DRV_PIXEL_FMT_NV42_RGB) || (pixel_format == HI_DRV_PIXEL_FMT_ARGB8888)
        || (pixel_format == HI_DRV_PIXEL_FMT_RGB24)) {
        meta_info->vpss_process = HI_FALSE;
    }

    support_zme = vpss_policy_support_out0_zme();
    if (support_zme == HI_FALSE) {
        if ((frame->comm_frame.width > VPSS_MAX_WIDTH_TRANS) || (frame->comm_frame.height > VPSS_MAX_HEIGHT_TRANS) ||
            (frame->comm_frame.width < VPSS_MIN_WIDTH_TRANS) || (frame->comm_frame.height < VPSS_MIN_HEIGHT_TRANS)) {
            meta_info->vpss_process = HI_FALSE;
        }
    }

    if (vpss_policy_check_special_field(frame) == HI_TRUE) {
        if ((frame->comm_frame.height * 2) > VPSS_MAX_HEIGHT_TRANS) { /* 2:times */
            meta_info->vpss_process = HI_FALSE;
        }
    }

    if (instance->global_dbg_ctrl->vpss_bypass == HI_TRUE) {
        meta_info->vpss_process = HI_FALSE;
    }

    return !meta_info->vpss_process;
}

hi_bool vpss_policy_check_need_csc(hi_drv_color_descript *in_color, hi_drv_color_descript *out_color)
{
    if (((in_color->color_primary == HI_DRV_COLOR_PRIMARY_BT601_525) ||
         (in_color->color_primary == HI_DRV_COLOR_PRIMARY_BT601_625) ||
         (in_color->color_primary == HI_DRV_COLOR_PRIMARY_BT709)) &&
        (out_color->color_primary == HI_DRV_COLOR_PRIMARY_BT2020)) {
        return HI_TRUE;
    }

    if (((out_color->color_primary == HI_DRV_COLOR_PRIMARY_BT601_525) ||
         (out_color->color_primary == HI_DRV_COLOR_PRIMARY_BT601_625) ||
         (out_color->color_primary == HI_DRV_COLOR_PRIMARY_BT709)) &&
        (in_color->color_primary == HI_DRV_COLOR_PRIMARY_BT2020)) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_bool vpss_policy_check_need_trans_port(hi_drv_vpss_video_frame *frame, vpss_port *port)
{
    hi_bool bypass0 = HI_FALSE;
    hi_bool bypass1 = HI_FALSE;
    hi_bool bypass2 = HI_FALSE;
    hi_bool bypass3 = HI_FALSE;

    if ((frame->comm_frame.width > VPSS_MAX_WIDTH_TRANS) || (frame->comm_frame.height > VPSS_MAX_HEIGHT_TRANS) ||
        (frame->comm_frame.width < VPSS_MIN_WIDTH_TRANS) || (frame->comm_frame.height < VPSS_MIN_HEIGHT_TRANS)) {
        /* out */
        if (((port->out_rect.rect_x == 0) && (port->out_rect.rect_y == 0)) &&
            (((port->out_rect.rect_w == 0) && (port->out_rect.rect_h == 0)) ||
             ((port->out_rect.rect_w == frame->comm_frame.width) &&
             (port->out_rect.rect_h == frame->comm_frame.height)))) {
            bypass0 = HI_TRUE;
        }

        /* video */
        if (((port->video_rect.rect_x == 0) && (port->video_rect.rect_y == 0)) &&
            (((port->video_rect.rect_w == 0) && (port->video_rect.rect_h == 0)) ||
             ((port->video_rect.rect_w == frame->comm_frame.width) &&
             (port->video_rect.rect_h == frame->comm_frame.height)))) {
            bypass1 = HI_TRUE;
        }

        /* crop */
        if (port->user_crop_en == HI_TRUE) {
            if ((port->offset_rect.left_offset == 0) && (port->offset_rect.right_offset == 0) &&
                (port->offset_rect.top_offset == 0) && (port->offset_rect.bottom_offset == 0)) {
                bypass2 = HI_TRUE;
            }
        } else {
            if (((port->crop_rect.rect_x == 0) && (port->crop_rect.rect_y == 0)) &&
                (((port->crop_rect.rect_w == 0) && (port->crop_rect.rect_h == 0)) ||
                 ((port->crop_rect.rect_w == frame->comm_frame.width) &&
                  (port->crop_rect.rect_h == frame->comm_frame.height)))) {
                bypass2 = HI_TRUE;
            }
        }

        /* hdr */
        if (port->hdr_follow_src == HI_TRUE) {
            bypass3 = HI_TRUE;
        } else {
            if ((port->out_hdr_type == frame->comm_frame.hdr_type) &&
                (vpss_policy_check_need_csc(&frame->comm_frame.color_space, &port->out_color_desp) != HI_TRUE)) {
                bypass3 = HI_TRUE;
            }
        }
    }

    return (bypass0 & bypass1 & bypass2 & bypass3);
}

hi_bool vpss_policy_check_need_trans_withport(hi_drv_vpss_video_frame *frame, vpss_instance *instance)
{
    hi_bool bypass = HI_FALSE;
    hi_bool bypass_port = HI_TRUE;
    hi_bool bypass_port_ret[VPSS_PORT_MAX_NUM] = {HI_TRUE, HI_TRUE, HI_TRUE};
    hi_bool bypass_ret = HI_FALSE;
    hi_u32 count;
    hi_u32 valid_port_cnt = 0;
    hi_u32 normal_port_cnt = 0;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;
    vpss_port *port = HI_NULL;

    meta_info = (hi_drv_win_vpss_meta_info *)frame->comm_frame.video_private.win_private.data;

    bypass = vpss_policy_check_need_trans(frame, instance);

    for (count = 0; count < VPSS_PORT_MAX_NUM; count++) {
        port = &instance->port[count];
        if ((port->port_id == HI_INVALID_HANDLE) || (port->enble != HI_TRUE)) {
            continue;
        }
        valid_port_cnt++;
        bypass_port_ret[count] = vpss_policy_check_need_trans_port(frame, port);

        /* when more than one normal port, tran process is not allowed */
        if (port->port_type == HI_DRV_VPSS_PORT_TYPE_NORMAL) {
            normal_port_cnt++;
        }
    }

    for (count = 0; count < valid_port_cnt; count++) {
        if (bypass_port_ret[count] != HI_TRUE) {
            bypass_port = HI_FALSE;
            break;
        }
    }

    /* prio : 1 instance set bypas; 2 multy port; 3 singal port bypass; 4 force bypass */
    if (bypass == HI_TRUE) {
        meta_info->vpss_process = HI_FALSE;
    } else if (normal_port_cnt >= 2) { /* 2:multy normal port */
        meta_info->vpss_process = HI_TRUE;
    } else if (bypass_port == HI_TRUE) {
        meta_info->vpss_process = HI_FALSE;
    } else if (meta_info->force_bypass == HI_TRUE) {
        /* add vdp force bypass */
        meta_info->vpss_process = HI_FALSE;
    } else {
        meta_info->vpss_process = HI_TRUE;
    }

    bypass_ret = !meta_info->vpss_process;

    return bypass_ret;
}

hi_bool vpss_policy_get_port_buf_strategy(hi_void)
{
    return HI_TRUE;
}

hi_void vpss_policy_correct_port_rect(hi_drv_rect *input_reso, hi_bool use_crop_rect, hi_drv_rect *crop_rect,
                                      hi_drv_crop_rect *offset_rect, hi_drv_rect *real_crop_rect)
{
    /* to do: add interlace conditon judge */
    hi_u32 input_width = input_reso->rect_w;
    hi_u32 input_height = input_reso->rect_h;
    hi_bool support_crop = vpss_policy_support_output_crop();
    if (support_crop == HI_FALSE) {
        real_crop_rect->rect_x = 0;
        real_crop_rect->rect_y = 0;
        real_crop_rect->rect_w = input_width;
        real_crop_rect->rect_h = input_height;
        return;
    }

    if (use_crop_rect == HI_TRUE) {
        if ((input_width <= (offset_rect->left_offset + offset_rect->right_offset)) ||
            (input_height <= (offset_rect->top_offset + offset_rect->bottom_offset))) {
            vpss_info("crop rect is large than input reso, croprect [t %d,b %d,l %d,r %d], input reso [w %d,h %d]\n",
                      offset_rect->top_offset, offset_rect->bottom_offset,
                      offset_rect->left_offset, offset_rect->right_offset,
                      input_width, input_height);
        } else {
            /* to do: add crop limit */
            real_crop_rect->rect_x = offset_rect->left_offset;
            real_crop_rect->rect_y = offset_rect->top_offset;
            real_crop_rect->rect_w = input_width - offset_rect->left_offset - offset_rect->right_offset;
            real_crop_rect->rect_h = input_height - offset_rect->top_offset - offset_rect->bottom_offset;
        }
    } else {
        if ((input_width < (crop_rect->rect_x + crop_rect->rect_w)) ||
            (input_height < (crop_rect->rect_y + crop_rect->rect_h))) {
            vpss_info("in rect is large than input reso, inrect [x %d,y %d,w %d,h %d], input reso [w %d,h %d]\n",
                      crop_rect->rect_x, crop_rect->rect_y,
                      crop_rect->rect_w, crop_rect->rect_h,
                      input_width, input_height);
        } else {
            /* to do: add crop limit */
            if (crop_rect->rect_w == 0 || crop_rect->rect_h == 0) {
                real_crop_rect->rect_x = 0;
                real_crop_rect->rect_y = 0;
                real_crop_rect->rect_w = input_width;
                real_crop_rect->rect_h = input_height;
            } else {
                real_crop_rect->rect_x = crop_rect->rect_x;
                real_crop_rect->rect_y = crop_rect->rect_y;
                real_crop_rect->rect_w = crop_rect->rect_w;
                real_crop_rect->rect_h = crop_rect->rect_h;
            }
        }
    }
}

hi_void vpss_policy_check_crop_rect(hi_drv_vpss_video_frame *frame, hi_drv_sample_type sample_type,
                                    hi_drv_rect *crop_rect)
{
    hi_bool forbid_crop = HI_FALSE;

    if (HI_FALSE == vpss_policy_support_input_crop()) {
        forbid_crop = HI_TRUE;
    }

    if ((crop_rect->rect_x < 0)
        || (crop_rect->rect_y < 0)
        || (crop_rect->rect_w < 0)
        || (crop_rect->rect_h < 0)) {
        vpss_error("s32X(%d)  s32Y(%d) s32Width(%d) s32Height(%d) is invaild\n",
                   crop_rect->rect_x, crop_rect->rect_y, crop_rect->rect_w, crop_rect->rect_h);
        forbid_crop = HI_TRUE;
    }

    if (((crop_rect->rect_x + crop_rect->rect_w) > frame->comm_frame.width)
        || ((crop_rect->rect_y + crop_rect->rect_h) > frame->comm_frame.height)) {
        vpss_error("s32X(%d) add s32Width(%d) is too large  u32SrcW(%d)n"
                   "Or s32Y(%d) add s32Height(%d) is too large  u32SrcH(%d)n",
                   crop_rect->rect_x, crop_rect->rect_w, frame->comm_frame.width,
                   crop_rect->rect_y, crop_rect->rect_h, frame->comm_frame.height);
        forbid_crop = HI_TRUE;
    }

    if ((forbid_crop == HI_TRUE) || (crop_rect->rect_h == 0) || (crop_rect->rect_w == 0)) {
        crop_rect->rect_x = 0;
        crop_rect->rect_y = 0;
        crop_rect->rect_w = frame->comm_frame.width;
        crop_rect->rect_h = frame->comm_frame.height;
    }

    crop_rect->rect_x = vpss_align_down(crop_rect->rect_x, 2);
    crop_rect->rect_w = vpss_align_down(crop_rect->rect_w, 2);

    if (sample_type == HI_DRV_SAMPLE_TYPE_PROGRESSIVE) {
        /* Revise image height to 2X,Die need */
        crop_rect->rect_y = vpss_align_down(crop_rect->rect_y, 2);
        crop_rect->rect_h = vpss_align_down(crop_rect->rect_h, 2);
    } else if (sample_type == HI_DRV_SAMPLE_TYPE_INTERLACE) {
        /* Revise image height to 4X,Die need */
        if (HI_TRUE == vpss_policy_check_special_field(frame)) {
            crop_rect->rect_y = vpss_align_down(crop_rect->rect_y, 2);
            crop_rect->rect_h = vpss_align_down(crop_rect->rect_h, 2);
        } else {
            crop_rect->rect_y = vpss_align_down(crop_rect->rect_y, 4);
            crop_rect->rect_h = vpss_align_down(crop_rect->rect_h, 4);
        }
    } else {
        vpss_error("UNKOWN sample_type:%d;\n", sample_type);
    }

    return;
}

static hi_void vpss_policy_set_meta_crop_by_offset(hi_drv_crop_rect *offset_rect, hi_drv_win_vpss_meta_info *meta_info)
{
    if ((offset_rect->left_offset == 0) && (offset_rect->right_offset == 0) &&
        (offset_rect->top_offset == 0) && (offset_rect->bottom_offset == 0)) {
        meta_info->vpss_crop = HI_FALSE;
    } else {
        meta_info->vpss_crop = HI_TRUE;
    }

    return;
}

static hi_void vpss_policy_set_meta_crop_by_src_crop(hi_drv_rect *src_crop_rect, hi_drv_win_vpss_meta_info *meta_info)
{
    if ((src_crop_rect->rect_x == 0) && (src_crop_rect->rect_w == 0)
        && (src_crop_rect->rect_y == 0) && (src_crop_rect->rect_h == 0)) {
        meta_info->vpss_crop = HI_FALSE;
    } else {
        meta_info->vpss_crop = HI_TRUE;
    }

    return;
}

static hi_void vpss_policy_calc_user_crop_rect(hi_drv_vpss_video_frame *frame,
    hi_drv_crop_rect *offset_rect, hi_drv_rect *real_crop_rect)
{
    hi_u32 crop_bottom = 0;
    hi_u32 crop_top = 0;
    hi_bool special_field = HI_FALSE;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;
    hi_u32 width = frame->comm_frame.width;
    hi_u32 height = frame->comm_frame.height;

    special_field = vpss_policy_check_special_field(frame);
    meta_info = (hi_drv_win_vpss_meta_info *)frame->comm_frame.video_private.win_private.data;

    vpss_policy_set_meta_crop_by_offset(offset_rect, meta_info);

    if ((width <= (offset_rect->left_offset + offset_rect->right_offset)) ||
        (height <= (offset_rect->top_offset + offset_rect->bottom_offset))) {
        vpss_info("crop rect is large than frame reso, croprect [t %d,b %d,l %d,r %d], frame [w %d,h %d]\n",
                  offset_rect->top_offset, offset_rect->bottom_offset, offset_rect->left_offset,
                  offset_rect->right_offset, width, height);
        return;
    }

    real_crop_rect->rect_x = offset_rect->left_offset;
    real_crop_rect->rect_w = width - offset_rect->left_offset - offset_rect->right_offset;

    if (special_field == HI_TRUE) {
        crop_bottom = vpss_align_up(offset_rect->bottom_offset, VPSS_RESO_ALIGN);
        crop_top = vpss_align_up(offset_rect->top_offset, VPSS_RESO_ALIGN);
        real_crop_rect->rect_y = crop_top;

        if (height < (crop_top + crop_bottom + VPSS_MIN_HEIGHT)) {
            vpss_warn("crop rect is larger than frame after correction.[top %d, bottom %d, frame h %d]\n",
                      crop_top, crop_bottom, height);
            real_crop_rect->rect_x = 0;
            real_crop_rect->rect_y = 0;
            real_crop_rect->rect_w = width;
            real_crop_rect->rect_h = height;
        } else {
            real_crop_rect->rect_h = height - crop_top - crop_bottom;
        }
    } else {
        if (frame->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_INTERLACE) {
            offset_rect->top_offset = vpss_align_up(offset_rect->top_offset, VPSS_RESO_ALIGN_4);
            offset_rect->bottom_offset = vpss_align_up(offset_rect->bottom_offset, VPSS_RESO_ALIGN_4);
        } else if (frame->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_PROGRESSIVE) {
            offset_rect->top_offset = vpss_align_up(offset_rect->top_offset, VPSS_RESO_ALIGN);
            offset_rect->bottom_offset = vpss_align_up(offset_rect->bottom_offset, VPSS_RESO_ALIGN);
        } else {
            vpss_error("UNKOWN sample_type:%d;\n", frame->comm_frame.sample_type);
        }

        real_crop_rect->rect_y = offset_rect->top_offset;
        real_crop_rect->rect_h = height - offset_rect->top_offset - offset_rect->bottom_offset;
    }

    return;
}

static hi_void vpss_policy_calc_norm_crop_rect(hi_drv_vpss_video_frame *frame, hi_drv_rect *src_crop_rect,
    hi_drv_crop_rect *offset_rect, hi_drv_rect *real_crop_rect)
{
    hi_bool special_field = HI_FALSE;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;
    hi_u32 width = frame->comm_frame.width;
    hi_u32 height = frame->comm_frame.height;

    special_field = vpss_policy_check_special_field(frame);
    meta_info = (hi_drv_win_vpss_meta_info *)frame->comm_frame.video_private.win_private.data;

    vpss_policy_set_meta_crop_by_src_crop(src_crop_rect, meta_info);

    if ((width < (src_crop_rect->rect_x + src_crop_rect->rect_w)) ||
        (height < (src_crop_rect->rect_y + src_crop_rect->rect_h))) {
        vpss_info("in rect is large than frame reso, inrect [x %d,y %d,w %d,h %d], frame [w %d,h %d]\n",
                  src_crop_rect->rect_x, src_crop_rect->rect_y,
                  src_crop_rect->rect_w, src_crop_rect->rect_h,
                  width, height);
        return;
    }

    real_crop_rect->rect_x = src_crop_rect->rect_x;
    real_crop_rect->rect_w = src_crop_rect->rect_w;

    if (special_field == HI_TRUE) {
        real_crop_rect->rect_y = vpss_align_down(src_crop_rect->rect_y, VPSS_RESO_ALIGN);
        real_crop_rect->rect_h = vpss_align_down(src_crop_rect->rect_h, VPSS_RESO_ALIGN);
    } else {
        if (frame->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_INTERLACE) {
            src_crop_rect->rect_y = vpss_align_down(src_crop_rect->rect_y, VPSS_RESO_ALIGN_4);
        } else {
            src_crop_rect->rect_y = vpss_align_down(src_crop_rect->rect_y, VPSS_RESO_ALIGN);
        }

        real_crop_rect->rect_y = src_crop_rect->rect_y;
        real_crop_rect->rect_h = src_crop_rect->rect_h;
    }

    return;
}


hi_void vpss_policy_calc_crop_rect(hi_drv_vpss_video_frame *frame, hi_drv_rect *src_crop_rect,
                                   hi_bool user_crop_en, hi_drv_crop_rect *offset_rect, hi_drv_rect *real_crop_rect)
{
    hi_bool special_field = HI_FALSE;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;

    special_field = vpss_policy_check_special_field(frame);
    meta_info = (hi_drv_win_vpss_meta_info *)frame->comm_frame.video_private.win_private.data;

    if (user_crop_en == HI_TRUE) {
        vpss_policy_calc_user_crop_rect(frame, offset_rect, real_crop_rect);
    } else {
        vpss_policy_calc_norm_crop_rect(frame, src_crop_rect, offset_rect, real_crop_rect);
    }

    real_crop_rect->rect_x = vpss_align_down(real_crop_rect->rect_x, 2);
    real_crop_rect->rect_w = vpss_align_down(real_crop_rect->rect_w, 2);

    if (frame->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_PROGRESSIVE) {
        /* Revise image height to 2X,Die need */
        real_crop_rect->rect_y = vpss_align_down(real_crop_rect->rect_y, 2);
        real_crop_rect->rect_h = vpss_align_down(real_crop_rect->rect_h, 2);
    } else {
        /* Revise image height to 4X,Die need */
        if (special_field == HI_TRUE) {
            real_crop_rect->rect_y = vpss_align_down(real_crop_rect->rect_y, 2);
            real_crop_rect->rect_h = vpss_align_down(real_crop_rect->rect_h, 2);
        } else {
            real_crop_rect->rect_y = vpss_align_down(real_crop_rect->rect_y, 4);
            real_crop_rect->rect_h = vpss_align_down(real_crop_rect->rect_h, 4);
        }
    }

    return;
}

static hi_void vpss_policy_revise_3d_image(hi_drv_vpss_video_frame *frame, hi_drv_rect *src_crop_rect)
{
    hi_bool is_support_cmp = HI_FALSE;

    memcpy(&(frame->vpss_buf_addr[HI_DRV_3D_EYE_RIGHT]), &(frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT]),
           sizeof(hi_drv_vpss_vid_frame_addr));

    is_support_cmp = vpss_policy_check_in_frame_cmp(frame->comm_frame.pixel_format, frame->comm_frame.cmp_info);
    if (is_support_cmp == HI_TRUE) {
        vpss_error("3D tile image can't be processed ePixFormat %d\n", frame->comm_frame.pixel_format);
    } else {
        /* 对于非压缩的格式，将宽高直接除根据格式进行除2 */
        if (frame->comm_frame.video_3d_type == HI_DRV_3D_SBS_HALF) {
            src_crop_rect->rect_w /= 2; /* 2:3d divise reso */
            frame->comm_frame.width = frame->comm_frame.width / 2; /* 2:3d divise reso */
            if (frame->comm_frame.width % VPSS_RESO_ALIGN != 0) {
                vpss_error("3D image can't be processed W %d\n", frame->comm_frame.width);
            }
        }

        if (frame->comm_frame.video_3d_type == HI_DRV_3D_TAB) {
            src_crop_rect->rect_h /= 2; /* 2:3d divise reso */
            frame->comm_frame.height = frame->comm_frame.height / 2; /* 2:3d divise reso */
            if (frame->comm_frame.height % VPSS_RESO_ALIGN != 0) {
                vpss_error("3D image can't be processed H %d\n", frame->comm_frame.height);
            }
        }
    }
}

hi_bool vpss_policy_revise_image(hi_drv_rect *in_src_crop_rect, hi_bool user_crop_en,
                                 hi_drv_crop_rect *offset_rect, hi_drv_vpss_video_frame *frame)
{
    hi_bool is_special_field = HI_FALSE;
    hi_drv_rect src_crop_rect = { 0 };

    vpss_policy_calc_crop_rect(frame, in_src_crop_rect, user_crop_en, offset_rect, &src_crop_rect);
    vpss_policy_check_crop_rect(frame, frame->comm_frame.sample_type, &src_crop_rect);
    frame->comm_frame.width = src_crop_rect.rect_w;
    frame->comm_frame.height = src_crop_rect.rect_h;
    /* 1.Revise image width to 2X */
    frame->comm_frame.width = frame->comm_frame.width & 0xfffffffe;

    if (frame->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_PROGRESSIVE) {
        /* Revise image height to 2X,Die need */
        frame->comm_frame.height = frame->comm_frame.height & 0xfffffffe;
    } else {
        /* Revise image height to 4X,Die need */
        frame->comm_frame.height = frame->comm_frame.height & 0xfffffffc;
    }

    /* 2.Revise singlefield mode ,change heigth and stride */
    is_special_field = vpss_policy_check_special_field(frame);
    if (is_special_field == HI_TRUE) {
        src_crop_rect.rect_h *= 2; /* 2:change h */
        src_crop_rect.rect_y *= 2; /* 2:change w */
        frame->comm_frame.height = src_crop_rect.rect_h;
    }

    /* 2. 3d addr revise   * SBS TAB read half image   * MVC read two addr  */
    if ((frame->comm_frame.video_3d_type == HI_DRV_3D_SBS_HALF) || (frame->comm_frame.video_3d_type == HI_DRV_3D_TAB)) {
        vpss_policy_revise_3d_image(frame, &src_crop_rect);
    }

    /* protect frame rate,if interlace stream framerate > 30 ,we revise it to 30 */
    if ((frame->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_INTERLACE) &&
        (frame->comm_frame.frame_rate > 30000) /* 30000:framerate */
        && (frame->comm_frame.field_mode == HI_DRV_FIELD_ALL)) {
        frame->comm_frame.frame_rate = 30000; /* 30000:framerate */
    }

    memcpy(in_src_crop_rect, &src_crop_rect, sizeof(hi_drv_rect));

    return HI_SUCCESS;
}

hi_bool vpss_policy_check_use_pq(hi_drv_vpss_video_frame *frame, vpss_debug_info *global_dbg_ctrl)
{
#ifdef HI_VPSS_DRV_USE_PQ
    if (global_dbg_ctrl->pq_bypass == HI_TRUE) {
        return HI_FALSE;
    }

    return HI_TRUE;
#else
    return HI_FALSE;
#endif
}

hi_bool vpss_policy_support_in_fmt(hi_drv_pixel_format pixel_format)
{
    hi_u32 pix_fmt_id = 0;
    hi_u32 support_pix_fmt_num = sizeof(g_input_pix_fmt_capacity) / sizeof(hi_drv_pixel_format);

    for (pix_fmt_id = 0; pix_fmt_id < support_pix_fmt_num; pix_fmt_id++) {
        if (pixel_format == g_input_pix_fmt_capacity[pix_fmt_id]) {
            return HI_TRUE;
        }
    }

    vpss_error("vpss can't be processed inpute PixFormat %d\n", pixel_format);
    return HI_FALSE;
}

hi_bool vpss_policy_support_out_fmt(hi_drv_pixel_format pixel_format)
{
    hi_u32 pix_fmt_id;
    hi_u32 support_pix_fmt_num = sizeof(g_output_pix_fmt_capacity) / sizeof(hi_drv_pixel_format);

    for (pix_fmt_id = 0; pix_fmt_id < support_pix_fmt_num; pix_fmt_id++) {
        if (pixel_format == g_output_pix_fmt_capacity[pix_fmt_id]) {
            return HI_TRUE;
        }
    }

    vpss_error("vpss can't be processed output PixFormat %d\n", pixel_format);
    return HI_FALSE;
}

vpss_policy_datafmt vpss_policy_get_data_fmt(hi_drv_pixel_format src_format)
{
    if ((src_format == HI_DRV_PIXEL_FMT_NV12)
        || (src_format == HI_DRV_PIXEL_FMT_NV21)) {
        return VPSS_POLICY_DATAFMT_420;
    }

    if ((src_format == HI_DRV_PIXEL_FMT_NV16)
        || (src_format == HI_DRV_PIXEL_FMT_NV61)
        || (src_format == HI_DRV_PIXEL_FMT_NV16_2X1)
        || (src_format == HI_DRV_PIXEL_FMT_NV61_2X1)) {
        return VPSS_POLICY_DATAFMT_422;
    }

    vpss_error("cannot 400 src_format %d\n", src_format);
    return VPSS_POLICY_DATAFMT_400;
}

hi_drv_pixel_format vpss_policy_get_nr_frf_fmt(hi_u32 width, hi_drv_pixel_format src_pixel_format)
{
#ifdef HI_VPSS_DRV_USE_GOLDEN
    return src_pixel_format;
#endif

    if (width <= VPSS_WIDTH_FHD) {
        return src_pixel_format;
    }

    if (VPSS_POLICY_DATAFMT_420 == vpss_policy_get_data_fmt(src_pixel_format)) {
        return HI_DRV_PIXEL_FMT_NV21;
    }

    if (VPSS_POLICY_DATAFMT_422 == vpss_policy_get_data_fmt(src_pixel_format)) {
        return HI_DRV_PIXEL_FMT_NV16_2X1;
    }

    return src_pixel_format;
}

void vpss_policy_get_nr_frf_cmp_info(hi_u32 width, hi_u32 height, hi_drv_compress_info *cmp_info)
{
    if (width <= VPSS_WIDTH_FHD && height <= VPSS_HEIGHT_FHD_1088) {
        cmp_info->cmp_mode = HI_DRV_COMPRESS_MODE_OFF;
    } else {
        cmp_info->cmp_mode = HI_DRV_COMPRESS_MODE_FRM_LOSS;
    }

#ifndef VPSS_96CV300_CS_SUPPORT
    cmp_info->cmp_mode = HI_DRV_COMPRESS_MODE_OFF;
#endif

#ifdef HI_VPSS_DRV_USE_GOLDEN
    cmp_info->cmp_mode = HI_DRV_COMPRESS_MODE_OFF;
#endif

    cmp_info->data_fmt = HI_DRV_DATA_FMT_LINER;
    cmp_info->ycmp_rate = VPSS_SUPPORT_CMP_NR_RATE;
    cmp_info->ccmp_rate = VPSS_SUPPORT_CMP_NR_RATE;

    return;
}

hi_drv_pixel_format vpss_policy_get_port_out_fmt(hi_drv_pixel_format src_fmt, hi_drv_pixel_format port_fmt)
{
    vpss_policy_datafmt data_fmt = vpss_policy_get_data_fmt(src_fmt);
    if (data_fmt == VPSS_POLICY_DATAFMT_420) {
        return HI_DRV_PIXEL_FMT_NV21;
    }

    if (data_fmt == VPSS_POLICY_DATAFMT_422) {
        return HI_DRV_PIXEL_FMT_NV61_2X1;
    }

    return src_fmt;
}

hi_bool vpss_policy_check_out_frame_cmp_fmt(hi_drv_pixel_format pixel_format, hi_drv_compress_info cmp_info)
{
    if (((pixel_format == HI_DRV_PIXEL_FMT_NV12)
         || (pixel_format == HI_DRV_PIXEL_FMT_NV21)
         || (pixel_format == HI_DRV_PIXEL_FMT_NV16)
         || (pixel_format == HI_DRV_PIXEL_FMT_NV61)
         || (pixel_format == HI_DRV_PIXEL_FMT_NV16_2X1)
         || (pixel_format == HI_DRV_PIXEL_FMT_NV61_2X1)) &&
        ((cmp_info.cmp_mode == HI_DRV_COMPRESS_MODE_OFF) ||
         (cmp_info.cmp_mode == HI_DRV_COMPRESS_MODE_LINE_LOSS) ||
         (cmp_info.cmp_mode == HI_DRV_COMPRESS_MODE_LINE_LOSSLESS))) {
        return HI_TRUE;
    }

    vpss_error("not support cmp:%d; pixel_format:%d\n", cmp_info.cmp_mode, pixel_format);
    return HI_FALSE;
}

hi_bool vpss_policy_check_out_frame_cmp_rote(hi_drv_vpss_rotation rotation, hi_drv_compress_info cmp_info)
{
    if ((cmp_info.cmp_mode == HI_DRV_COMPRESS_MODE_LINE_LOSS) ||
        (cmp_info.cmp_mode == HI_DRV_COMPRESS_MODE_LINE_LOSSLESS)) {
        if ((rotation == HI_DRV_VPSS_ROTATION_90) || (rotation == HI_DRV_VPSS_ROTATION_270)) {
            vpss_error("not support cmp:%d; rotation:%d\n", cmp_info.cmp_mode, rotation);
            return HI_FALSE;
        }
    }

    return HI_TRUE;
}

hi_bool vpss_policy_check_port_crop_limit(hi_drv_vpss_video_frame *src_frame, hi_drv_vpss_port_frame_cfg *port_cfg)
{
    /* crop reso limit */
    if (port_cfg->user_crop_en == HI_TRUE) {
        if (((port_cfg->in_offset_rect.left_offset % VPSS_RESO_ALIGN) != 0) ||
            ((port_cfg->in_offset_rect.right_offset % VPSS_RESO_ALIGN) != 0) ||
            ((port_cfg->in_offset_rect.top_offset % VPSS_RESO_ALIGN) != 0) ||
            ((port_cfg->in_offset_rect.bottom_offset % VPSS_RESO_ALIGN) != 0)) {
            vpss_error("invalid port crop offset l:%d; r:%d; t:%d; b:%d \n",
                       port_cfg->in_offset_rect.left_offset, port_cfg->in_offset_rect.right_offset,
                       port_cfg->in_offset_rect.top_offset, port_cfg->in_offset_rect.bottom_offset);
            return HI_FALSE;
        }

        if (((port_cfg->in_offset_rect.left_offset + port_cfg->in_offset_rect.right_offset) >=
                 src_frame->comm_frame.width) ||
            ((port_cfg->in_offset_rect.top_offset + port_cfg->in_offset_rect.bottom_offset) >=
                 src_frame->comm_frame.height)) {
            vpss_error("invalid port crop offset l:%d; r:%d; t:%d; b:%d; out w:%d; h:%d \n",
                       port_cfg->in_offset_rect.left_offset, port_cfg->in_offset_rect.right_offset,
                       port_cfg->in_offset_rect.top_offset, port_cfg->in_offset_rect.bottom_offset,
                       src_frame->comm_frame.width, src_frame->comm_frame.height);
            return HI_FALSE;
        }
    } else {
        if (((port_cfg->crop_rect.rect_x % VPSS_RESO_ALIGN) != 0) ||
            ((port_cfg->crop_rect.rect_y % VPSS_RESO_ALIGN) != 0) ||
            ((port_cfg->crop_rect.rect_w % VPSS_RESO_ALIGN) != 0) ||
            ((port_cfg->crop_rect.rect_h % VPSS_RESO_ALIGN) != 0)) {
            vpss_error("invalid port crop x:%d; y:%d; w:%d; h:%d \n",
                       port_cfg->crop_rect.rect_x, port_cfg->crop_rect.rect_y,
                       port_cfg->crop_rect.rect_w, port_cfg->crop_rect.rect_h);
            return HI_FALSE;
        }

        if (((port_cfg->crop_rect.rect_x + port_cfg->crop_rect.rect_w) > src_frame->comm_frame.width) ||
            ((port_cfg->crop_rect.rect_y + port_cfg->crop_rect.rect_h) > src_frame->comm_frame.height)) {
            vpss_error("invalid port crop x:%d; y:%d; w:%d; h:%d; out w:%d; h:%d\n",
                       port_cfg->crop_rect.rect_x, port_cfg->crop_rect.rect_y,
                       port_cfg->crop_rect.rect_w, port_cfg->crop_rect.rect_h,
                       src_frame->comm_frame.width, src_frame->comm_frame.height);
            return HI_FALSE;
        }
    }

    return HI_TRUE;
}

hi_bool vpss_policy_check_port_video_limit(hi_drv_vpss_video_frame *src_frame, hi_drv_vpss_port_frame_cfg *port_cfg)
{
    /* video reso limit */
    if ((port_cfg->video_rect.rect_x < 0) ||
        (port_cfg->video_rect.rect_y < 0) ||
        ((port_cfg->video_rect.rect_x % VPSS_RESO_ALIGN) != 0) ||
        ((port_cfg->video_rect.rect_y % VPSS_RESO_ALIGN) != 0) ||
        ((port_cfg->video_rect.rect_w % VPSS_RESO_ALIGN) != 0) ||
        ((port_cfg->video_rect.rect_h % VPSS_RESO_ALIGN) != 0)) {
        vpss_error("invalid port video x:%d; y:%d; w:%d; h:%d; \n",
                   port_cfg->video_rect.rect_x, port_cfg->video_rect.rect_y,
                   port_cfg->video_rect.rect_w, port_cfg->video_rect.rect_h);
        return HI_FALSE;
    }

    if ((port_cfg->video_rect.rect_w != 0) && (port_cfg->video_rect.rect_h != 0)) {
        if ((src_frame->comm_frame.width >= port_cfg->video_rect.rect_w * VPSS_MAX_ZME_MULTIPLY) ||
            (src_frame->comm_frame.height >= port_cfg->video_rect.rect_h * VPSS_MAX_ZME_MULTIPLY)) {
            vpss_error("invalid para : src(%d %d); video(%d %d) \n",
                       src_frame->comm_frame.width, src_frame->comm_frame.height,
                       port_cfg->video_rect.rect_w, port_cfg->video_rect.rect_h);
            return HI_FALSE;
        }
    }

    return HI_TRUE;
}

static hi_bool vpss_policy_check_port_out_limit(hi_drv_vpss_port_frame_cfg *port_cfg)
{
    /* out reso limit */
    if ((port_cfg->out_rect.rect_x != 0) ||
        (port_cfg->out_rect.rect_y != 0)) {
        vpss_error("invalid port out x:%d; y:%d\n", port_cfg->out_rect.rect_x, port_cfg->out_rect.rect_y);
        return HI_FALSE;
    }

    if (((port_cfg->video_rect.rect_x + port_cfg->video_rect.rect_w) > port_cfg->out_rect.rect_w) ||
        ((port_cfg->video_rect.rect_y + port_cfg->video_rect.rect_h) > port_cfg->out_rect.rect_h)) {
        vpss_error("invalid port video is larger than out | video x:%d; y:%d; w:%d; h:%d; out w:%d; h:%d\n",
                   port_cfg->video_rect.rect_x, port_cfg->video_rect.rect_y,
                   port_cfg->video_rect.rect_w, port_cfg->video_rect.rect_h,
                   port_cfg->out_rect.rect_w, port_cfg->out_rect.rect_h);
        return HI_FALSE;
    }

    if (((port_cfg->out_rect.rect_w % VPSS_RESO_ALIGN) != 0) ||
        ((port_cfg->out_rect.rect_h % VPSS_RESO_ALIGN) != 0)) {
        vpss_error("invalid port out w:%d; h:%d\n",
                   port_cfg->video_rect.rect_w, port_cfg->video_rect.rect_h);
        return HI_FALSE;
    }

    return HI_TRUE;
}

hi_bool vpss_policy_check_port_reso_limit(hi_drv_vpss_video_frame *src_frame, hi_drv_vpss_port_frame_cfg *port_cfg)
{
    hi_bool support = HI_FALSE;

    support = vpss_policy_check_port_crop_limit(src_frame, port_cfg);
    if (support != HI_TRUE) {
        vpss_error("not support port crop \n");
        return HI_FALSE;
    }

    support = vpss_policy_check_port_video_limit(src_frame, port_cfg);
    if (support != HI_TRUE) {
        vpss_error("not support port video \n");
        return HI_FALSE;
    }

    support = vpss_policy_check_port_out_limit(port_cfg);
    if (support != HI_TRUE) {
        vpss_error("not support port out \n");
        return HI_FALSE;
    }

    return HI_TRUE;
}

hi_bool vpss_policy_check_in_frame_cmp(hi_drv_pixel_format pixel_format, hi_drv_compress_info cmp_info)
{
    if ((cmp_info.cmp_mode != HI_DRV_COMPRESS_MODE_OFF) &&
        (cmp_info.cmp_mode != HI_DRV_COMPRESS_MODE_SEG_LOSSLESS)) {
        vpss_error("not support in cmp_info.cmp_mode %d\n", cmp_info.cmp_mode);
        return HI_FALSE;
    }

    return HI_TRUE;
}

hi_bool vpss_policy_check_nr_cmp(hi_drv_pixel_format pixel_format, hi_drv_compress_info cmp_info)
{
    /* todo : add nr dcmp support */
    return HI_FALSE;
}

hi_bool vpss_policy_check_nr_dcmp(hi_drv_pixel_format pixel_format, hi_drv_compress_info cmp_info)
{
    /* todo : add nr dcmp support */
    return HI_FALSE;
}

hi_void vpss_policy_revise_out_frame_info(hi_drv_pixel_format *pixel_format, hi_drv_compress_info *cmp_info)
{
    if (HI_TRUE != vpss_policy_check_out_frame_cmp_fmt(*pixel_format, *cmp_info)) {
        cmp_info->cmp_mode = HI_DRV_COMPRESS_MODE_OFF;
        cmp_info->ycmp_rate = VPSS_SUPPORT_CMP_LOSSLESS_RATE;
        cmp_info->ccmp_rate = VPSS_SUPPORT_CMP_LOSSLESS_RATE;
        return;
    }

    if (cmp_info->cmp_mode == HI_DRV_COMPRESS_MODE_LINE_LOSSLESS) {
        cmp_info->ycmp_rate = VPSS_SUPPORT_CMP_LOSSLESS_RATE;
        cmp_info->ccmp_rate = VPSS_SUPPORT_CMP_LOSSLESS_RATE;
    }

    if ((*pixel_format != HI_DRV_PIXEL_FMT_NV12) &&
        (*pixel_format != HI_DRV_PIXEL_FMT_NV21) &&
        (*pixel_format != HI_DRV_PIXEL_FMT_NV16) &&
        (*pixel_format != HI_DRV_PIXEL_FMT_NV16_2X1) &&
        (*pixel_format != HI_DRV_PIXEL_FMT_NV61) &&
        (*pixel_format != HI_DRV_PIXEL_FMT_NV61_2X1)) {
        vpss_error("Dont support %d pixformat\n", *pixel_format);
    }

    return;
}

/* whether support pq process when rotation */
hi_bool vpss_policy_support_rota_alg(vpss_debug_info *global_dbg_ctrl, hi_u32 width, hi_u32 height)
{
    if ((width <= global_dbg_ctrl->rot_alg_width) && (height <= global_dbg_ctrl->rot_alg_height)) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

hi_void vpss_policy_revise_tran_frame_bitwidth(hi_drv_vpss_video_frame *src_frame)
{
    if (((src_frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV12) ||
         (src_frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV21)) &&
        (src_frame->comm_frame.cmp_info.data_fmt == HI_DRV_DATA_FMT_TILE)) {
        src_frame->comm_frame.bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
    }
}

/* 3d_detect port function  */
hi_bool vpss_policy_support_logic_3d_detect_port(hi_void)
{
    return HI_FALSE;
}

hi_bool vpss_policy_check_support_virtual_port(hi_void)
{
    return HI_TRUE;
}

hi_bool vpss_policy_support_rwzb(hi_void)
{
    return HI_TRUE;
}

hi_bool vpss_policy_support_hdr(hi_void)
{
    return HI_TRUE;
}

hi_bool vpss_policy_support_decountor(hi_void)
{
    return HI_FALSE;
}

hi_bool vpss_policy_support_npu_wbc(hi_void)
{
    return HI_FALSE;
}

hi_bool vpss_policy_support_cccl(hi_void)
{
    return HI_FALSE;
}

hi_bool vpss_policy_support_lbd(hi_void)
{
    return HI_FALSE;
}

hi_bool vpss_policy_support_dei(vpss_perfor_alg_ctrl vpss_alg_ctrl, vpss_policy_alg_input_info *policy_alg)
{
    if ((policy_alg->sample_type == HI_DRV_SAMPLE_TYPE_INTERLACE) &&
        (policy_alg->width <= VPSS_WIDTH_FHD) &&
        (policy_alg->height <= VPSS_MAX_HEIGHT_INTER)) {
        return HI_TRUE;
    } else  {
        return HI_FALSE;
    }
}

hi_bool vpss_policy_support_tnr(vpss_perfor_alg_ctrl vpss_alg_ctrl, vpss_policy_alg_input_info *policy_alg)
{
    if ((policy_alg->width <= VPSS_MAX_WIDTH_TRANS) &&
        (policy_alg->height <= VPSS_MAX_HEIGHT_TRANS)) {
        return HI_TRUE;
    } else  {
        return HI_FALSE;
    }
}

hi_bool vpss_policy_support_snr(vpss_perfor_alg_ctrl vpss_alg_ctrl, vpss_policy_alg_input_info *policy_alg)
{
    if ((policy_alg->width <= VPSS_MAX_WIDTH_TRANS) &&
        (policy_alg->height <= VPSS_MAX_HEIGHT_TRANS)) {
        return HI_TRUE;
    } else  {
        return HI_FALSE;
    }
}

hi_bool vpss_policy_support_db(vpss_perfor_alg_ctrl vpss_alg_ctrl, vpss_policy_alg_input_info *policy_alg)
{
    if ((policy_alg->width <= VPSS_MAX_WIDTH_TRANS) &&
        (policy_alg->height <= VPSS_MAX_HEIGHT_TRANS)) {
        return HI_TRUE;
    } else  {
        return HI_FALSE;
    }
}

hi_bool vpss_policy_support_dm(vpss_perfor_alg_ctrl vpss_alg_ctrl, vpss_policy_alg_input_info *policy_alg)
{
    if ((policy_alg->width <= VPSS_MAX_WIDTH_TRANS) &&
        (policy_alg->height <= VPSS_MAX_HEIGHT_TRANS)) {
        return HI_TRUE;
    } else  {
        return HI_FALSE;
    }
}

hi_bool vpss_policy_support_dm430(vpss_perfor_alg_ctrl vpss_alg_ctrl, vpss_policy_alg_input_info *policy_alg)
{
    if ((policy_alg->width <= VPSS_DM430_WIDTH) && (policy_alg->height <= VPSS_DM430_HEIGHT) &&
        (policy_alg->frame_rate <= VPSS_DM430_MAX_RATE)) {
        return HI_TRUE;
    } else  {
        return HI_FALSE;
    }
}

hi_void vpss_policy_distribute_out_meta_playcnt(hi_bool frame_index_repeat, hi_drv_vpss_video_frame *vpss_image)
{
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;
    hi_bool playcnt_more = HI_FALSE;

    meta_info = (hi_drv_win_vpss_meta_info *)vpss_image->comm_frame.video_private.vpss_private.data;

    if (meta_info->play_cnt % 2 == 0) { /* 2:even */
        meta_info->play_cnt = meta_info->play_cnt / 2;  /* 2:distribute */
        return;
    }

    playcnt_more = (frame_index_repeat == HI_TRUE) ^ (vpss_image->comm_frame.top_field_first == HI_TRUE);

    if (playcnt_more == HI_TRUE) {
        if (meta_info->play_cnt == 1) {
            meta_info->play_cnt = 1;
        } else {
            meta_info->play_cnt = meta_info->play_cnt % 2 + meta_info->play_cnt / 2; /* 2:distribute */
        }

        return;
    }

    if (meta_info->play_cnt == 1) {
        meta_info->play_cnt = 0;
    } else {
        meta_info->play_cnt = meta_info->play_cnt / 2; /* 2:distribute */
    }

    return;
}

/* whether fix out put pixel bitwidht : 10bit */
hi_bool vpss_policy_support_fix_out_bitwidth10(hi_void)
{
    return HI_FALSE;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

