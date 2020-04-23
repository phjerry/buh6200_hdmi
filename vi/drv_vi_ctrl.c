/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drv vi ctrl
 * Author: sdk
 * Create: 2019-12-14
 */

#include "linux/hisilicon/securec.h"
#include "hi_drv_sys.h"

#include "vi_hal.h"
#include "vi_buf.h"
#include "vi_uf.h"
#include "vi_dsc.h"

#include "drv_vi_proc.h"
#include "drv_vi_ctrl.h"

VI_STATIC hi_s32 drv_vi_check_create_color_descript(hi_vi_attr *vi_attr_p)
{
    if (vi_attr_p->color_descript.color_space >= HI_DRV_COLOR_CS_MAX) {
        vi_drv_log_err("color_space(%#x) error.\n", vi_attr_p->color_descript.color_space);
        return HI_FAILURE;
    }

    if (vi_attr_p->color_descript.quantify_range >= HI_DRV_COLOR_RANGE_MAX) {
        vi_drv_log_err("quantify_range(%#x) error.\n", vi_attr_p->color_descript.quantify_range);
        return HI_FAILURE;
    }

    if (vi_attr_p->color_descript.color_primary >= HI_DRV_COLOR_PRIMARY_COLOR_MAX) {
        vi_drv_log_err("color_primary(%#x) error.\n", vi_attr_p->color_descript.color_primary);
        return HI_FAILURE;
    }

    if (vi_attr_p->color_descript.transfer_type >= HI_DRV_COLOR_TRANSFER_TYPE_MAX) {
        vi_drv_log_err("transfer_type(%#x) error.\n", vi_attr_p->color_descript.transfer_type);
        return HI_FAILURE;
    }

    if (vi_attr_p->color_descript.matrix_coef >= HI_DRV_COLOR_MATRIX_COEFFS_MAX) {
        vi_drv_log_err("matrix_coef(%#x) error.\n", vi_attr_p->color_descript.matrix_coef);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_vi_check_create_config(hi_vi_attr *vi_attr_p, hi_vi_config *config_p)
{
    if (config_p->field_mode >= HI_DRV_FIELD_MAX) {
        vi_drv_log_err("display_field_mode(%#x) error.\n", config_p->field_mode);
        return HI_FAILURE;
    }

    if (config_p->color_sys >= HI_DRV_COLOR_SYS_MAX) {
        vi_drv_log_err("color_sys(%#x) error.\n", config_p->color_sys);
        return HI_FAILURE;
    }

    if (config_p->width < 2 || config_p->height < 2) { /* dsc need odd to even, so must >= 2 */
        vi_drv_log_err("width(%d) height(%d) error.\n", config_p->width, config_p->height);
        return HI_FAILURE;
    }

    if (config_p->frame_rate == 0) {
        vi_drv_log_err("frame_rate(%d) error.\n", config_p->frame_rate);
        return HI_FAILURE;
    }

    if ((config_p->interlace != HI_TRUE) && (config_p->interlace != HI_FALSE)) {
        vi_drv_log_err("interlace(%#x) error.\n", config_p->interlace);
        return HI_FAILURE;
    }

    if ((config_p->crop_rect.rect_x < 0) || (config_p->crop_rect.rect_y < 0) || (config_p->crop_rect.rect_w <= 0) ||
        (config_p->crop_rect.rect_h <= 0)) {
        vi_drv_log_err("invalid input_rect:(%d,%d,%d,%d).\n", config_p->crop_rect.rect_x, config_p->crop_rect.rect_y,
                       config_p->crop_rect.rect_w, config_p->crop_rect.rect_h);
        return HI_FAILURE;
    }

    if ((config_p->crop_rect.rect_w + config_p->crop_rect.rect_x > config_p->width) ||
        (config_p->crop_rect.rect_h + config_p->crop_rect.rect_y > config_p->height)) {
        vi_drv_log_err("crop rect must < src size, X:%d, Y:%d, W:%d, H:%d src_w:%d, src_h:%d\n",
                       config_p->crop_rect.rect_x, config_p->crop_rect.rect_y, config_p->crop_rect.rect_w,
                       config_p->crop_rect.rect_w, config_p->width, config_p->height);
        return HI_FAILURE;
    }

    if ((vi_attr_p->pixel_format == HI_DRV_PIXEL_FMT_NV61_2X1) &&
        (vi_drv_is_odd_num((hi_u32)config_p->crop_rect.rect_x) ||
         vi_drv_is_odd_num((hi_u32)config_p->crop_rect.rect_w))) {
        vi_drv_log_err("width must be even value, X:%d, CW:%d\n", config_p->crop_rect.rect_x,
                       config_p->crop_rect.rect_w);
        return HI_FAILURE;
    }

    if ((config_p->interlace == HI_TRUE) && (vi_drv_is_odd_num((hi_u32)config_p->crop_rect.rect_y) ||
                                             vi_drv_is_odd_num((hi_u32)config_p->crop_rect.rect_h))) {
        vi_drv_log_err("height item of each field must be even value Y:%d, H%d\n", config_p->crop_rect.rect_y,
                       config_p->crop_rect.rect_h);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_vi_check_create_other(hi_vi_attr *vi_attr_p)
{
    if (vi_attr_p->vi_type >= HI_VI_TYPE_MAX) {
        vi_drv_log_err("vi_type(%#x) error.\n", vi_attr_p->vi_type);
        return HI_FAILURE;
    }

    if (vi_attr_p->intf_mode >= HI_VI_INTF_MAX) {
        vi_drv_log_err("intf_mode(%#x) error.\n", vi_attr_p->intf_mode);
        return HI_FAILURE;
    }

    if (vi_attr_p->access_type >= HI_VI_ACCESS_MAX) {
        vi_drv_log_err("access(%#x) error.\n", vi_attr_p->access_type);
        return HI_FAILURE;
    }

    if (vi_attr_p->over_sample >= HI_VI_OVERSAMPLE_MAX) {
        vi_drv_log_err("over_sample(%#x) error.\n", vi_attr_p->over_sample);
        return HI_FAILURE;
    }

    if (vi_attr_p->picture_mode >= HI_VI_PICTURE_MODE_MAX) {
        vi_drv_log_err("picture_mode(%#x) error.\n", vi_attr_p->picture_mode);
        return HI_FAILURE;
    }

    if (vi_attr_p->source_type >= HI_DRV_SOURCE_MAX) {
        vi_drv_log_err("source_type(%#x) error.\n", vi_attr_p->source_type);
        return HI_FAILURE;
    }

    if (vi_attr_p->video_3d_type >= HI_DRV_3D_MAX) {
        vi_drv_log_err("en3_d_fmt(%#x) error.\n", vi_attr_p->video_3d_type);
        return HI_FAILURE;
    }

    /* 3D FP格式需要检测帧/场消隐区的宽度 */
    if ((vi_attr_p->video_3d_type == HI_DRV_3D_FPK) && (vi_attr_p->vblank == 0)) {
        vi_drv_log_err("vblank only for 3d fp!\n");
        return HI_FAILURE;
    }

    if ((vi_attr_p->pixel_format != HI_DRV_PIXEL_FMT_NV42_RGB) && (vi_attr_p->pixel_format != HI_DRV_PIXEL_FMT_NV42) &&
        (vi_attr_p->pixel_format != HI_DRV_PIXEL_FMT_NV61_2X1) && (vi_attr_p->pixel_format != HI_DRV_PIXEL_FMT_NV21)) {
        vi_drv_log_err("pix_fmt(%#x) error.\n", vi_attr_p->pixel_format);
        return HI_FAILURE;
    }

    if (vi_attr_p->bit_width >= HI_DRV_PIXEL_BITWIDTH_MAX) {
        vi_drv_log_err("bit_width(%#x) error.\n", vi_attr_p->bit_width);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_vi_check_create_attr(hi_vi_attr *vi_attr_p)
{
    hi_s32 ret;

    ret = drv_vi_check_create_color_descript(vi_attr_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = drv_vi_check_create_config(vi_attr_p, &vi_attr_p->config);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = drv_vi_check_create_other(vi_attr_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_vi_check_attr(vi_instance *instance_p)
{
    if (instance_p->vi_handle == HI_INVALID_HANDLE) {
        vi_drv_log_err("invalid handle(%#x)\n", instance_p->vi_handle);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_vi_init_instance(vi_instance *instance_p)
{
    if (instance_p->poll_wait_queue.wait == HI_NULL) {
        osal_wait_init(&instance_p->poll_wait_queue);
    }

    instance_p->vo_handle = HI_INVALID_HANDLE;

    memset_s(&instance_p->done_node_set, sizeof(instance_p->done_node_set), 0, sizeof(instance_p->done_node_set));
    memset_s(&instance_p->writing_node_set, sizeof(instance_p->writing_node_set), 0,
             sizeof(instance_p->writing_node_set));
    memset_s(&instance_p->cfg_node_set, sizeof(instance_p->cfg_node_set), 0, sizeof(instance_p->cfg_node_set));

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_vi_reset_instance(vi_instance *instance_p)
{
    if (instance_p->poll_wait_queue.wait != HI_NULL) {
        osal_wait_destroy(&instance_p->poll_wait_queue);
    }

    memset_s(instance_p, sizeof(vi_instance), 0, sizeof(vi_instance));

    instance_p->vi_handle = HI_INVALID_HANDLE;
    instance_p->vo_handle = HI_INVALID_HANDLE;
    instance_p->vi_status = VI_INSTANCE_STATUS_INIT;

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_vi_set_delay_time(vi_instance *instance_p)
{
    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_vi_release_alone_buf(vi_instance *instance_p)
{
#if 0
    hi_s32 ret;

    ret = vi_buf_put_node(instance_p->buf_handle_p, VI_NODE_STATUS_IDLE, &instance_p->done_node_set);
    HI_ASSERT(ret == HI_SUCCESS);

    ret = vi_buf_put_node(instance_p->buf_handle_p, VI_NODE_STATUS_IDLE, &instance_p->writing_node_set);
    HI_ASSERT(ret == HI_SUCCESS);

    ret = vi_buf_put_node(instance_p->buf_handle_p, VI_NODE_STATUS_IDLE, &instance_p->cfg_node_set);
    HI_ASSERT(ret == HI_SUCCESS);
#endif

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_vi_get_buf_alloc_size(hi_u32 y_stride, hi_u32 c_stride, hi_u32 width, hi_u32 height,
                                           hi_drv_pixel_format in_pix_fmt, hi_drv_pixel_format out_pix_fmt,
                                           hi_u32 *buf_size_p)
{
    hi_u32 ret;
    hi_u32 dolby_y_stride = 0;
    hi_u32 rgb_size = 0;
    hi_u32 buf_size = 0;
    hi_u32 dolby_buf_size = 0;

    /* 根据输出的video_fmt确认buf_size */
    if ((HI_DRV_PIXEL_FMT_NV42_RGB == out_pix_fmt) || (HI_DRV_PIXEL_FMT_NV42 == out_pix_fmt)) {
        buf_size = y_stride * height * 3; /* rbg444 is y*3 */
        if (in_pix_fmt == HI_DRV_PIXEL_FMT_NV42_RGB && out_pix_fmt == HI_DRV_PIXEL_FMT_NV42_RGB) {
            /* add dolby support */
            /* under PC mode, can not right curren input type, so the buffer must enough to support */
            ret = vi_comm_get_y_stride(width, HI_DRV_PIXEL_BITWIDTH_12BIT, &dolby_y_stride);
            if (ret != HI_SUCCESS) {
                vi_drv_log_err("vi_comm_get_y_stride ERR!\n");
                return HI_FAILURE;
            }

            /* dolby lowtency RGB 10bit alloc */
            ret = vi_comm_get_y_stride(width, HI_DRV_PIXEL_BITWIDTH_10BIT, &y_stride);
            if (ret != HI_SUCCESS) {
                vi_drv_log_err("vi_comm_get_y_stride ERR!\n");
                return HI_FAILURE;
            }

            dolby_buf_size = dolby_y_stride * height * 2; /* dolby strid need * 2 */
            rgb_size = y_stride * height * 3;             /* rbg is y*3 */

            buf_size = (dolby_buf_size > rgb_size) ? dolby_buf_size : rgb_size;
        }
    } else if (HI_DRV_PIXEL_FMT_NV61_2X1 == out_pix_fmt) {
        buf_size = y_stride * height * 2; /* rbg422 is y*2 */

        /* add dolby support */
        /* when rgb444 in with dolby, it must tunnel to yuv422 12bit in vicap, so the buffer must enough to support */
        if (in_pix_fmt == HI_DRV_PIXEL_FMT_NV42_RGB) {
            ret = vi_comm_get_y_stride(width, HI_DRV_PIXEL_BITWIDTH_12BIT, &dolby_y_stride);
            if (ret != HI_SUCCESS) {
                vi_drv_log_err("vi_comm_get_y_stride ERR!\n");
                return ret;
            }

            dolby_buf_size = dolby_y_stride * height * 2; /* rbg422 is y*2 */
            rgb_size = y_stride * height * 3;             /* rbg is y*3 */

            buf_size = (dolby_buf_size > rgb_size) ? dolby_buf_size : rgb_size;
        }
    } else if (HI_DRV_PIXEL_FMT_NV21 == out_pix_fmt) {
        buf_size = y_stride * height + c_stride * height;
    } else {
        vi_drv_log_err("invalid video format of VICAP!\n");
        return HI_FAILURE;
    }

    *buf_size_p = buf_size;

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_vi_get_buf_attr(vi_instance *instance_p, vi_buf_attr *buf_attr_p)
{
    hi_s32 ret;
    hi_u32 width;
    hi_u32 height;
    hi_u32 y_stride = 0;
    hi_u32 c_stride = 0;
    hi_u32 buf_size = 0;

    ret = vi_dsc_get_buf_num(instance_p, &buf_attr_p->min_buf_num, &buf_attr_p->max_buf_num);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    width = instance_p->attr_in.width;   /* 源图像一帧的宽度 */
    height = instance_p->attr_in.height; /* 源图像一帧的高度 */

    if (instance_p->attr_in.source_type == HI_DRV_SOURCE_ATV) {
        height = VI_MAX_ATV_HEIGHT;
    }

    /* 隔行信号，存储高度减半(端子送入的为一帧的高度) */
    if (instance_p->attr_in.interlace) {
        height >>= 1;
    }

    /* 计算stride */
    ret = vi_comm_get_y_stride(width, instance_p->attr_out.bit_width, &y_stride);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 根据输出的video_fmt确认cstride与buf_size */
    ret = vi_comm_get_c_stride(y_stride, instance_p->attr_out.pix_fmt, &c_stride);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 3D输入, 3D播放 */
    if (HI_DRV_3D_NONE != instance_p->attr_in.video_3d_type) {
        buf_size = buf_size * 2; /* 3d size need * 2 */
    }

    ret = drv_vi_get_buf_alloc_size(y_stride, c_stride, width, height, instance_p->attr_in.pix_fmt,
                                    instance_p->attr_out.pix_fmt, &buf_size);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    buf_attr_p->buf_size = buf_size;

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_vi_buf_uf_init(vi_instance *instance_p)
{
    hi_s32 ret;
    vi_buf_attr buf_attr = {0};
    vi_uf_attr uf_attr = {0};

    ret = drv_vi_get_buf_attr(instance_p, &buf_attr);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = vi_buf_init(&buf_attr, &instance_p->buf_handle_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 创建存帧 */
    uf_attr.buf_handle_p = instance_p->buf_handle_p;
    uf_attr.depth = 0;
    ret = vi_uf_init(&uf_attr, &instance_p->uf_handle_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_bool drv_vi_frame_rate_ctrl(vi_instance *vi_instance_p)
{
    hi_u32 src_rate = vi_instance_p->attr_in.rate;
    hi_u32 dst_rate = vi_instance_p->attr_out.frame_rate;
    hi_s32 *lost_frame_key_p = &vi_instance_p->lost_frame_key; /* 丢帧策略关键值 */

    /* 如果是满帧通道则每一帧都捕获 */
    if (dst_rate == src_rate) {
        return HI_TRUE;
    }

    /* 丢帧算法 */
    *lost_frame_key_p += dst_rate;

    /* 按照算法，本帧应该捕获 */
    if (*lost_frame_key_p >= src_rate) {
        *lost_frame_key_p -= src_rate;
        return HI_TRUE;
    }

    /* 丢帧 */
    return HI_FALSE;
}

VI_STATIC hi_void drv_vi_fill_frame_addr(vi_instance *instance_p, vi_buf_node *node_set_p)
{
    vi_out_attr *vi_out_attr_p = &instance_p->attr_out;
    hi_drv_video_frame *video_frm_p = video_frm_p = &node_set_p->frame_info;

    /* 检测视频是否为3D格式 */
    if (vi_out_attr_p->video_3d_type == HI_DRV_3D_NONE) {
        video_frm_p->buf_addr[HI_DRV_3D_EYE_LEFT].start_addr = node_set_p->frame_addr.video_buf_addr.buf_handle;
        video_frm_p->buf_addr[HI_DRV_3D_EYE_LEFT].y_offset = 0x0;
        video_frm_p->buf_addr[HI_DRV_3D_EYE_LEFT].cr_offset = 0x0;
        video_frm_p->buf_addr[HI_DRV_3D_EYE_LEFT].stride_y = vi_out_attr_p->y_stride;
        video_frm_p->buf_addr[HI_DRV_3D_EYE_LEFT].stride_c = vi_out_attr_p->c_stride;
        video_frm_p->buf_addr[HI_DRV_3D_EYE_LEFT].stride_cr = 0x0;
        if (vi_out_attr_p->interlace) {
            video_frm_p->buf_addr[HI_DRV_3D_EYE_LEFT].c_offset =
                video_frm_p->buf_addr[HI_DRV_3D_EYE_LEFT].y_offset +
                vi_out_attr_p->y_stride * vi_out_attr_p->height / P_I_MULTIPLE;
        } else {
            video_frm_p->buf_addr[HI_DRV_3D_EYE_LEFT].c_offset =
                video_frm_p->buf_addr[HI_DRV_3D_EYE_LEFT].y_offset + vi_out_attr_p->y_stride * vi_out_attr_p->height;
        }

        video_frm_p->buf_addr[HI_DRV_3D_EYE_RIGHT].start_addr = 0x0;
        video_frm_p->buf_addr[HI_DRV_3D_EYE_RIGHT].y_offset = 0x0;
        video_frm_p->buf_addr[HI_DRV_3D_EYE_RIGHT].c_offset = 0x0;
        video_frm_p->buf_addr[HI_DRV_3D_EYE_RIGHT].cr_offset = 0x0;
        video_frm_p->buf_addr[HI_DRV_3D_EYE_RIGHT].stride_y = 0x0;
        video_frm_p->buf_addr[HI_DRV_3D_EYE_RIGHT].stride_c = 0x0;
        video_frm_p->buf_addr[HI_DRV_3D_EYE_RIGHT].stride_cr = 0x0;
    }
}

VI_STATIC hi_void drv_vi_fill_frame_info(vi_instance *instance_p, vi_buf_node *node_set_p)
{
    vi_instance_dbg_data *vi_instance_dbg_data_p = &instance_p->dbg_data;
    hi_drv_video_frame *cur_frm_p = &node_set_p->frame_info;

    /* 清空帧信息 */
    memset_s(cur_frm_p, sizeof(hi_drv_video_frame), 0, sizeof(hi_drv_video_frame));

    /* 设置当前帧的信息 */
    cur_frm_p->frame_index = vi_instance_dbg_data_p->sequence++;

    /* 满足系统需求，在发送之前，3D格式统一更改为HI_DRV_FT_FPK */
    if (instance_p->attr_out.video_3d_type > HI_DRV_3D_NONE) {
        cur_frm_p->video_3d_type = HI_DRV_3D_FPK;
    } else {
        cur_frm_p->video_3d_type = instance_p->attr_out.video_3d_type;
    }

    cur_frm_p->width = instance_p->attr_out.width;
    if (instance_p->attr_out.interlace) {
        cur_frm_p->height = instance_p->attr_out.height / P_I_MULTIPLE;
        cur_frm_p->sample_type = HI_DRV_SAMPLE_TYPE_INTERLACE;
        if (instance_p->attr_ctrls.disp_field_mode == HI_DRV_FIELD_ALL) {
            cur_frm_p->frame_rate = instance_p->attr_out.frame_rate * FRAME_PRECISION;
            if (vi_hal_get_cur_filed_type(instance_p->host_handle_p) == 0) {
                cur_frm_p->field_mode = HI_DRV_FIELD_BOTTOM;
            } else {
                cur_frm_p->field_mode = HI_DRV_FIELD_TOP;
            }
        } else {
            cur_frm_p->frame_rate = instance_p->attr_out.frame_rate * FRAME_PRECISION / P_I_MULTIPLE;
            cur_frm_p->field_mode = HI_DRV_FIELD_ALL;
        }
    } else {
        cur_frm_p->height = instance_p->attr_out.height;
        cur_frm_p->sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
        cur_frm_p->field_mode = HI_DRV_FIELD_ALL;
        cur_frm_p->frame_rate = instance_p->attr_out.frame_rate * FRAME_PRECISION;
    }

    cur_frm_p->ori_frame_rate = instance_p->attr_in.rate * FRAME_PRECISION;

    cur_frm_p->bit_width = instance_p->attr_out.bit_width;
    cur_frm_p->pixel_format = instance_p->attr_out.pix_fmt;
    cur_frm_p->color_space = instance_p->attr_out.color_descript;

    cur_frm_p->disp_ctrl_width = cur_frm_p->width;
    cur_frm_p->disp_ctrl_height = cur_frm_p->height;
    cur_frm_p->buf_valid_mode = cur_frm_p->field_mode;
    cur_frm_p->disp_rect.rect_x = 0;
    cur_frm_p->disp_rect.rect_y = 0;
    cur_frm_p->disp_rect.rect_w = cur_frm_p->width;
    cur_frm_p->disp_rect.rect_h = cur_frm_p->height;

    cur_frm_p->video_private.source = instance_p->attr_in.source_type;
    cur_frm_p->video_private.color_sys = instance_p->attr_out.color_sys;
}

VI_STATIC hi_s32 drv_vi_update_node(vi_instance *instance_p)
{
#if 0
    hi_s32 ret;

    if (instance_p->done_node_set.valid) {
        vi_drv_log_err("done node is valid!\n");
        ret = vi_buf_put_node(instance_p->buf_handle_p, VI_NODE_STATUS_IDLE, &instance_p->done_node_set);
        if (ret != HI_SUCCESS) {
            vi_drv_log_err("put idle buf fail\n");
            return HI_SUCCESS;
        }
    }
#endif

    memcpy_s(&instance_p->done_node_set, sizeof(vi_buf_node), &instance_p->writing_node_set, sizeof(vi_buf_node));
    memcpy_s(&instance_p->writing_node_set, sizeof(vi_buf_node), &instance_p->cfg_node_set, sizeof(vi_buf_node));

    instance_p->cfg_node_set.valid = HI_FALSE;

    return HI_SUCCESS;
}

VI_STATIC hi_void drv_vi_update_frame_pts(vi_instance *instance_p)
{
    hi_u32 pts;
    vi_buf_node *node_set_p = &instance_p->writing_node_set;

    pts = hi_drv_sys_get_time_stamp_ms();

    node_set_p->frame_info.src_pts = pts;
    node_set_p->frame_info.pts = pts;
}

VI_STATIC hi_s32 drv_vi_send_frm(vi_instance *instance_p)
{
    hi_s32 ret;
    vi_buf_node *node_set_p = NULL;

    if (instance_p->attr_ctrls.low_delay) {
        node_set_p = &instance_p->done_node_set;
    } else {
        node_set_p = &instance_p->done_node_set;
    }

    if (!node_set_p->valid) {
        return HI_SUCCESS;
    }

    ret = vi_buf_put_node(instance_p->buf_handle_p, VI_NODE_STATUS_BUSY, node_set_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("put busy buf fail\n");
        return HI_SUCCESS;
    }

    vi_uf_sendfrm(instance_p->uf_handle_p, node_set_p);

    osal_wait_wakeup(&instance_p->poll_wait_queue);

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_vi_prepare_frm(vi_instance *instance_p)
{
    hi_s32 ret;
    vi_buf_node *cfg_node_set_p = &instance_p->cfg_node_set;
    vi_instance_dbg_data *vi_instance_dbg_data_p = &instance_p->dbg_data;

    if (drv_vi_frame_rate_ctrl(instance_p) == HI_FALSE) {
        vi_drv_log_dbg("drop this frame!\n");
        goto drop_this_frame;
    }

    ret = vi_buf_get_node(instance_p->buf_handle_p, VI_NODE_STATUS_IDLE, cfg_node_set_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("get idle buf fail\n");
        goto get_buf_faild;
    }

    drv_vi_fill_frame_info(instance_p, cfg_node_set_p);
    drv_vi_fill_frame_addr(instance_p, cfg_node_set_p);

    /* 置视频帧有效标志 */
    cfg_node_set_p->valid = HI_TRUE;

    return HI_SUCCESS;

get_buf_faild:
    vi_instance_dbg_data_p->get_fail_cnt++;
drop_this_frame:
    cfg_node_set_p->valid = HI_FALSE;
    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_vi_cfg_frm(vi_instance *instance_p)
{
    hi_s32 ret;
    vi_buf_node *node_set_p = &instance_p->cfg_node_set;

    if (!node_set_p->valid) {
        return HI_SUCCESS;
    }

    ret = vi_hal_cfg_node(instance_p->host_handle_p, node_set_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_vi_process_fun(hi_void *arg)
{
    hi_s32 ret;
    vi_instance *instance_p = (vi_instance *)arg;

    if (instance_p->vi_status < VI_INSTANCE_STATUS_START) {
        return HI_FAILURE;
    }

    ret = drv_vi_update_node(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    drv_vi_update_frame_pts(instance_p);

    if (instance_p->attr_ctrls.low_delay) {
        ret = drv_vi_prepare_frm(instance_p);
        if (ret != HI_SUCCESS) {
            vi_drv_log_err("err:0x%x\n", ret);
            return ret;
        }

        ret = drv_vi_cfg_frm(instance_p);
        if (ret != HI_SUCCESS) {
            vi_drv_log_err("err:0x%x\n", ret);
            return ret;
        }

        ret = drv_vi_send_frm(instance_p);
        if (ret != HI_SUCCESS) {
            vi_drv_log_err("err:0x%x\n", ret);
            return ret;
        }
    } else {
        ret = drv_vi_send_frm(instance_p);
        if (ret != HI_SUCCESS) {
            vi_drv_log_err("err:0x%x\n", ret);
            return ret;
        }

        ret = drv_vi_prepare_frm(instance_p);
        if (ret != HI_SUCCESS) {
            vi_drv_log_err("err:0x%x\n", ret);
            return ret;
        }

        ret = drv_vi_cfg_frm(instance_p);
        if (ret != HI_SUCCESS) {
            vi_drv_log_err("err:0x%x\n", ret);
            return ret;
        }
    }

    instance_p->last_field_mode = vi_hal_get_cur_filed_type(instance_p->host_handle_p);
    instance_p->last_src_hdr_type = instance_p->attr_in.hdr_type;

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_create(vi_instance *instance_p, hi_vi_attr *vi_attr_p)
{
    hi_s32 ret;

    vi_drv_func_enter();

    /* 参数检查 */
    ret = drv_vi_check_create_attr(vi_attr_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 状态机检查 */
    if (instance_p->vi_status < VI_INSTANCE_STATUS_INIT) {
        return HI_FAILURE;
    } else if (instance_p->vi_status > VI_INSTANCE_STATUS_INIT) {
        return HI_FAILURE;
    }

    /* 获取空闲vicap */
    ret = vi_hal_creat_host(vi_attr_p, &instance_p->host_handle_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 刷新属性 */
    memcpy_s(&instance_p->attr_orig, sizeof(hi_vi_attr), vi_attr_p, sizeof(hi_vi_attr));
    ret = vi_dsc_updata_vi_attr(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 创建buffer和存帧 */
    ret = drv_vi_buf_uf_init(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 生成句柄 */
    instance_p->vi_handle = vi_drv_generate_handle(vi_hal_get_vicap_id(instance_p->host_handle_p));

    /* 添加proc */
    ret = vi_drv_proc_add(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
    }

    /* 切状态 */
    instance_p->vi_status = VI_INSTANCE_STATUS_CREATE;

    /* 初始化上下文 */
    ret = drv_vi_init_instance(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_destroy(vi_instance *instance_p)
{
    hi_s32 ret;

    vi_drv_func_enter();

    /* 参数检查 */
    ret = drv_vi_check_attr(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 状态机检查 */
    if (instance_p->vi_status < VI_INSTANCE_STATUS_CREATE) {
        return HI_FAILURE;
    } else if (instance_p->vi_status == VI_INSTANCE_STATUS_ATTACH) {
        return HI_FAILURE;
    } else if (instance_p->vi_status >= VI_INSTANCE_STATUS_START) {
        return HI_FAILURE;
    }

    /* 清理轮转帧相关资源 */
    ret = drv_vi_release_alone_buf(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 销毁存帧 */
    ret = vi_uf_deinit(instance_p->uf_handle_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 销毁buffer */
    ret = vi_buf_deinit(instance_p->buf_handle_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 删除proc */
    vi_drv_proc_del(instance_p);

    /* 复位上下文 */
    ret = drv_vi_reset_instance(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 改状态机状态 */
    instance_p->vi_status = VI_INSTANCE_STATUS_INIT;

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_attach(vi_instance *instance_p, hi_handle vo_handle)
{
    hi_s32 ret;

    vi_drv_func_enter();

    /* 检查参数 */
    ret = drv_vi_check_attr(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 状态机状态检查 */
    if (instance_p->vi_status < VI_INSTANCE_STATUS_INIT) {
        return HI_FAILURE;
    } else if (instance_p->vi_status < VI_INSTANCE_STATUS_CREATE) {
        return HI_FAILURE;
    } else if (instance_p->vi_status >= VI_INSTANCE_STATUS_ATTACH) {
        if (instance_p->vo_handle == vo_handle) {
            vi_drv_log_info("vi_instance_status=%d!\n\n", instance_p->vi_status);
            return HI_SUCCESS;
        } else {
            vi_drv_log_err("attach (0x%x) fail, (0x%x) attached\n", vo_handle, instance_p->vo_handle);
            return HI_FAILURE;
        }
    }

    instance_p->vo_handle = vo_handle;

    /* 刷属性 */
    ret = vi_dsc_updata_vi_attr(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 注册回调 */
    ret = vi_hal_set_callback(instance_p->host_handle_p, drv_vi_process_fun, instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 初始化vicap */
    ret = vi_hal_prepare(instance_p->host_handle_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 切状态 */
    instance_p->vi_status = VI_INSTANCE_STATUS_ATTACH;

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_detach(vi_instance *instance_p, hi_handle vo_handle)
{
    hi_s32 ret;

    vi_drv_func_enter();

    /* 检查参数 */
    ret = drv_vi_check_attr(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 状态机状态检查 */
    if (instance_p->vi_status < VI_INSTANCE_STATUS_CREATE) {
        return HI_FAILURE;
    } else if (instance_p->vi_status == VI_INSTANCE_STATUS_CREATE) {
        vi_drv_log_info("vi_instance_status=%d!\n\n", instance_p->vi_status);
        return HI_SUCCESS;
    } else if (instance_p->vi_status >= VI_INSTANCE_STATUS_START) {
        return HI_FAILURE;
    }

    /* 反初始化vicap */
    ret = vi_hal_unprepare(instance_p->host_handle_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }
    instance_p->vo_handle = HI_INVALID_HANDLE;

    /* 反注册回调 */
    ret = vi_hal_set_callback(instance_p->host_handle_p, HI_NULL, HI_NULL);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 切状态 */
    instance_p->vi_status = VI_INSTANCE_STATUS_CREATE;

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_start(vi_instance *instance_p)
{
    hi_s32 ret;

    vi_drv_func_enter();

    /* 检查参数 */
    ret = drv_vi_check_attr(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 状态机状态检查 */
    if (instance_p->vi_status < VI_INSTANCE_STATUS_INIT) {
        return HI_FAILURE;
    } else if (instance_p->vi_status < VI_INSTANCE_STATUS_CREATE) {
        return HI_FAILURE;
    } else if (instance_p->vi_status < VI_INSTANCE_STATUS_ATTACH) {
        return HI_FAILURE;
    } else if (instance_p->vi_status >= VI_INSTANCE_STATUS_START) {
        vi_drv_log_info("vi_instance_status=%d!\n\n", instance_p->vi_status);
        return HI_SUCCESS;
    }

    /* 刷属性 */
    ret = vi_dsc_updata_vi_attr(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* set vi delay time to window */
    ret = drv_vi_set_delay_time(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 切状态 */
    instance_p->vi_status = VI_INSTANCE_STATUS_START;

    /* 启动采集 */
    ret = vi_hal_start(instance_p->host_handle_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_stop(vi_instance *instance_p)
{
    hi_s32 ret;

    vi_drv_func_enter();

    /* 检查参数 */
    ret = drv_vi_check_attr(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    if (instance_p->vi_status < VI_INSTANCE_STATUS_CREATE) {
        return HI_FAILURE;
    } else if (instance_p->vi_status < VI_INSTANCE_STATUS_START) {
        vi_drv_log_info("vi_instance_status=%d!\n", instance_p->vi_status);
        return HI_SUCCESS;
    }

    /* 停止采集 */
    ret = vi_hal_stop(instance_p->host_handle_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 切状态 */
    instance_p->vi_status = VI_INSTANCE_STATUS_ATTACH;

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_close_path(vi_instance *instance_p)
{
    hi_s32 ret;

    vi_drv_func_enter();

    ret = vi_drv_ctrl_stop(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_warn("err:0x%x\n", ret);
    }

    ret = vi_drv_ctrl_detach(instance_p, instance_p->vo_handle);
    if (ret != HI_SUCCESS) {
        vi_drv_log_warn("err:0x%x\n", ret);
    }

    ret = vi_drv_ctrl_destroy(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_warn("err:0x%x\n", ret);
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_updata_dbg_info(vi_instance *instance_p, hi_vi_dbg_info *dbg_info_p)
{
    if (instance_p->vi_status < VI_INSTANCE_STATUS_CREATE) {
        vi_drv_log_err("vi_instance_status=%d!\n", instance_p->vi_status);
        return HI_FAILURE;
    }

    memcpy_s(&instance_p->dbg_data.user_info, sizeof(hi_vi_dbg_info), dbg_info_p, sizeof(hi_vi_dbg_info));

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_acquire_frame(vi_instance *instance_p, hi_drv_video_frame *frame_p)
{
    if (instance_p->vi_status != VI_INSTANCE_STATUS_START) {
        vi_drv_log_err("vi_instance_status=%d!\n", instance_p->vi_status);
        return HI_FAILURE;
    }

    return vi_buf_get_frame(instance_p->buf_handle_p, VI_NODE_STATUS_BUSY, frame_p);
}

hi_s32 vi_drv_ctrl_release_frame(vi_instance *instance_p, hi_drv_video_frame *frame_p)
{
    if (instance_p->vi_status < VI_INSTANCE_STATUS_CREATE) {
        vi_drv_log_err("vi_instance_status=%d!\n", instance_p->vi_status);
        return HI_FAILURE;
    }

    return vi_buf_put_frame(instance_p->buf_handle_p, VI_NODE_STATUS_IDLE, frame_p);
}

hi_s32 vi_drv_ctrl_get_attr(vi_instance *instance_p, hi_vi_attr *vi_attr_p)
{
    vi_drv_func_enter();

    if (instance_p->vi_status < VI_INSTANCE_STATUS_CREATE) {
        vi_drv_log_err("vi_instance_status=%d!\n", instance_p->vi_status);
        return HI_FAILURE;
    }

    memcpy_s(vi_attr_p, sizeof(hi_vi_attr), &instance_p->attr_orig, sizeof(hi_vi_attr));

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_set_config(vi_instance *instance_p, hi_vi_config *vi_config_p)
{
    hi_s32 ret;

    vi_drv_func_enter();

    if ((instance_p->vi_status != VI_INSTANCE_STATUS_CREATE) && (instance_p->vi_status != VI_INSTANCE_STATUS_ATTACH) &&
        (instance_p->vi_status != VI_INSTANCE_STATUS_START)) {
        vi_drv_log_err("vi_instance_status=%d!\n", instance_p->vi_status);
        return HI_FAILURE;
    }

    memcpy_s(&instance_p->attr_orig.config, sizeof(hi_vi_config), vi_config_p, sizeof(hi_vi_config));

    ret = vi_dsc_updata_vi_attr(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_get_config(vi_instance *instance_p, hi_vi_config *vi_config_p)
{
    vi_drv_func_enter();

    if (instance_p->vi_status < VI_INSTANCE_STATUS_CREATE) {
        vi_drv_log_err("vi_instance_status=%d!\n", instance_p->vi_status);
        return HI_FAILURE;
    }

    memcpy_s(vi_config_p, sizeof(hi_vi_config), &instance_p->attr_orig.config, sizeof(hi_vi_config));

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_set_src_quantization_range(vi_instance *instance_p, hi_vi_quantization_range range)
{
    hi_s32 ret;

    vi_drv_func_enter();

    if (instance_p->vi_status < VI_INSTANCE_STATUS_CREATE) {
        vi_drv_log_err("vi_instance_status=%d!\n", instance_p->vi_status);
        return HI_FAILURE;
    }

    if (range >= HI_VI_QUANTIZATION_RANGE_MAX) {
        vi_drv_log_err("not support this type!\n");
        return HI_FAILURE;
    }

    instance_p->attr_ctrls.quantization_range = range;

    ret = vi_dsc_updata_vi_attr(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_update_nstd_info(vi_instance *instance_p, hi_vi_nonstd_info nstd_info)
{
    hi_s32 ret;

    vi_drv_func_enter();

    if (instance_p->vi_status < VI_INSTANCE_STATUS_CREATE) {
        vi_drv_log_err("vi_instance_status=%d!\n", instance_p->vi_status);
        return HI_FAILURE;
    }

    if ((nstd_info.height <= 1) || (nstd_info.height >= VI_MAX_ATV_HEIGHT)) {
        vi_drv_log_err("nstd_info(%d,%d,%d)%d\n", nstd_info.non_std, nstd_info.height, nstd_info.v_freq);
        return HI_FAILURE;
    }

    instance_p->attr_ctrls.nstd_info.non_std = nstd_info.non_std;
    instance_p->attr_ctrls.nstd_info.height = nstd_info.height;
    instance_p->attr_ctrls.nstd_info.v_freq = nstd_info.v_freq;

    ret = vi_dsc_updata_vi_attr(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_poll(vi_instance *instance_p)
{
    return vi_buf_check_busy(instance_p->buf_handle_p);
}

hi_s32 vi_drv_ctrl_suspend(hi_void)
{
    hi_s32 ret;

    vi_drv_func_enter();

    ret = vi_hal_suspend();
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_resume(hi_void)
{
    hi_s32 ret;

    vi_drv_func_enter();

    ret = vi_hal_resume();
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_init(hi_void)
{
    hi_s32 ret;

    vi_drv_func_enter();

    ret = vi_hal_init();
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_drv_ctrl_deinit(hi_void)
{
    hi_s32 ret;

    vi_drv_func_enter();

    ret = vi_hal_deinit();
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}
