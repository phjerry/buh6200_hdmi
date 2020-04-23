/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vi dsc
 * Author: sdk
 * Create: 2019-12-14
 */

#include "vi_comm.h"
#include "vi_dsc.h"

VI_STATIC hi_void drv_dsc_get_proc_dbg_flag(vi_instance *vi_instance_p, vi_proc_dbg_command dbg_cmd, hi_void *attr_p)
{
}

VI_STATIC hi_s32 drv_dsc_get_wh(vi_instance *vi_instance_p, hi_u32 *width_p, hi_u32 *height_p)
{
    hi_u32 width = *width_p;
    hi_u32 height = *height_p;

    switch (vi_instance_p->attr_orig.video_3d_type) {
        case HI_DRV_3D_NONE:
            /*
             * support odd width timing. fix odd to even.
             */
            *width_p &= 0xfffffffe;
            break;

        case HI_DRV_3D_SBS_HALF:
            *width_p = width >> 1;
            break;

        case HI_DRV_3D_TAB:
            *height_p = height >> 1;
            break;

        case HI_DRV_3D_FPK:
            /* 3D的FP格式，一次传输两张图像，高度需要除2 */
            if (vi_instance_p->attr_orig.config.interlace) {
                /* 3D的FP隔行有三个无效区，大小分别为:vblank+1、vblank、vblank+1 */
                *height_p = (height - (vi_instance_p->attr_orig.vblank * 3 + 2)) >> 1; /* 3 invalid zone,2=1+1 */
            } else {
                /* 3D的FP逐行有一个无效区，大小为:vblank */
                *height_p = (height - vi_instance_p->attr_orig.vblank) >> 1;
            }
            break;

        default:
            vi_drv_log_err("invalid 3D formt!\n");
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_dsc_get_lowdelay(vi_instance *vi_instance_p, hi_bool *low_delay_p)
{
    *low_delay_p = HI_FALSE;

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_dsc_get_hdr_check_mode(vi_instance *vi_instance_p)
{
    vi_ctrls_attr *vi_ctrls_p = HI_NULL;

    vi_ctrls_p = &vi_instance_p->attr_ctrls;

    vi_ctrls_p->check_dolby = HI_FALSE;
    vi_ctrls_p->check_hdr = HI_FALSE;

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_dsc_get_in_color_space(hi_vi_attr *vi_attr_p, hi_vi_quantization_range csc_type,
                                            hi_drv_color_descript *color_descript_p)
{
    *color_descript_p = vi_attr_p->color_descript;

    if (csc_type == HI_VI_QUANTIZATION_RANGE_LIMIT) {
        color_descript_p->quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
    } else if (csc_type == HI_VI_QUANTIZATION_RANGE_FULL) {
        color_descript_p->quantify_range = HI_DRV_COLOR_FULL_RANGE;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_dsc_get_out_color_spcae(vi_instance *vi_instance_p, hi_drv_color_descript *color_descript_p)
{
    vi_in_attr *vi_in_attr_p = &vi_instance_p->attr_in;
    vi_out_attr *vi_out_attr_p = &vi_instance_p->attr_out;

    vi_out_attr_p->color_descript = vi_in_attr_p->color_descript;
    vi_out_attr_p->color_descript.color_space = HI_DRV_COLOR_CS_YUV;

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_dsc_get_out_pix_fmt(vi_instance *vi_instance_p, hi_drv_pixel_format *pix_fmt_p)
{
    vi_in_attr *vi_in_attr_p = HI_NULL;
    vi_out_attr *vi_out_attr_p = HI_NULL;
    vi_ctrls_attr *vi_ctrls_p = HI_NULL;

    vi_in_attr_p = &vi_instance_p->attr_in;
    vi_out_attr_p = &vi_instance_p->attr_out;
    vi_ctrls_p = &vi_instance_p->attr_ctrls;

    /*
     * graphic_mode(UI display title is pc_mode)is superior to by_pass_vpss(UI display title is game_mode)
     * graphic_mode | by_pass_vpss
     * pc_mode      | game_mode
     * on           | on => bypassvpss, rgb not pass csc
     * on           | off => bypassvpss, rgb not pass csc
     * off          | on => bypassvpss, rgb pass csc
     * off          | off => normal scence
     * when full_screen is false, must passvpss, so give yuv422.
     */
    if (vi_instance_p->attr_orig.picture_mode == HI_VI_PICTURE_MODE_GAME) {
        *pix_fmt_p = vi_in_attr_p->pix_fmt;
    } else {
        if (vi_ctrls_p->vpss_bypass) {
            /* YUV420 in */
            if (HI_DRV_PIXEL_FMT_NV21 == vi_in_attr_p->pix_fmt) {
                *pix_fmt_p = vi_in_attr_p->pix_fmt;
            } else {
                *pix_fmt_p = HI_DRV_PIXEL_FMT_NV61_2X1; /* YUV422 out */
            }
        } else {
            /* YUV420 in */
            if (HI_DRV_PIXEL_FMT_NV21 == vi_in_attr_p->pix_fmt) {
                *pix_fmt_p = vi_in_attr_p->pix_fmt;
            } else {
                *pix_fmt_p = HI_DRV_PIXEL_FMT_NV61_2X1; /* YUV422 out */
            }
        }
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 drv_dsc_get_out_bitwidth(vi_instance *vi_instance_p, hi_drv_pixel_bitwidth *bit_width_p)
{
    *bit_width_p = HI_DRV_PIXEL_BITWIDTH_10BIT;

    return HI_SUCCESS;
}

hi_s32 vi_dsc_get_buf_num(vi_instance *vi_instance_p, hi_u32 *min_buf_num_p, hi_u32 *max_buf_num_p)
{
    if (vi_instance_p->attr_orig.config.interlace) {
        *min_buf_num_p = VI_I_MIN_BUF_NUM;
        *max_buf_num_p = VI_I_MAX_BUF_NUM;
    } else {
        *min_buf_num_p = VI_P_MIN_BUF_NUM;
        *max_buf_num_p = VI_P_MAX_BUF_NUM;
    }

    drv_dsc_get_proc_dbg_flag(vi_instance_p, VI_PROC_DBG_COMMAND_BUFNUM, (hi_void *)max_buf_num_p);

    return HI_SUCCESS;
}

hi_s32 vi_dsc_get_in_attr(vi_instance *vi_instance_p)
{
    hi_s32 ret;
    hi_vi_attr *vi_attr_p = HI_NULL;
    vi_in_attr *vi_in_attr_p = HI_NULL;
    hi_vi_nonstd_info *nstd_info_p = HI_NULL;

    vi_attr_p = &vi_instance_p->attr_orig;
    vi_in_attr_p = &vi_instance_p->attr_in;
    nstd_info_p = &vi_instance_p->attr_ctrls.nstd_info;

    vi_in_attr_p->access = vi_attr_p->access_type;
    vi_in_attr_p->over_sample = vi_attr_p->over_sample;
    vi_in_attr_p->intf_mode = vi_attr_p->intf_mode;
    vi_in_attr_p->source_type = vi_attr_p->source_type;
    vi_in_attr_p->hdr_type = HI_DRV_HDR_TYPE_SDR;
    vi_in_attr_p->video_3d_type = vi_attr_p->video_3d_type;
    vi_in_attr_p->pix_fmt = vi_attr_p->pixel_format;
    vi_in_attr_p->color_descript = vi_attr_p->color_descript;
    vi_in_attr_p->color_sys = vi_attr_p->config.color_sys;
    vi_in_attr_p->bit_width = vi_attr_p->bit_width;
    vi_in_attr_p->width = vi_attr_p->config.width;
    vi_in_attr_p->height = vi_attr_p->config.height;
    vi_in_attr_p->interlace = vi_attr_p->config.interlace;
    vi_in_attr_p->rate = vi_attr_p->config.frame_rate;
    vi_in_attr_p->vblank = vi_attr_p->vblank;

    ret = drv_dsc_get_in_color_space(vi_attr_p, vi_instance_p->attr_ctrls.quantization_range,
                                     &vi_in_attr_p->color_descript);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    if (nstd_info_p->non_std) {
        vi_in_attr_p->height = nstd_info_p->height;
    }

    ret = drv_dsc_get_wh(vi_instance_p, &vi_in_attr_p->width, &vi_in_attr_p->height);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    drv_dsc_get_proc_dbg_flag(vi_instance_p, VI_PROC_DBG_COMMAND_FRMRATE, (hi_void *)(&vi_in_attr_p->rate));

    return HI_SUCCESS;
}

hi_s32 vi_dsc_get_ctrl_attr(vi_instance *vi_instance_p)
{
    hi_s32 ret;
    hi_vi_attr *vi_attr_p = HI_NULL;
    vi_ctrls_attr *vi_ctrls_p = HI_NULL;

    vi_attr_p = &vi_instance_p->attr_orig;
    vi_ctrls_p = &vi_instance_p->attr_ctrls;

    drv_dsc_get_proc_dbg_flag(vi_instance_p, VI_PROC_DBG_COMMAND_BYPASSVPSS, HI_NULL);

    vi_ctrls_p->disp_field_mode = vi_attr_p->config.field_mode;
    vi_ctrls_p->vpss_bypass = (vi_attr_p->picture_mode != HI_VI_PICTURE_MODE_VIDEO);

    ret = drv_dsc_get_lowdelay(vi_instance_p, &vi_ctrls_p->low_delay);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = drv_dsc_get_hdr_check_mode(vi_instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    memcpy_s(&vi_ctrls_p->crop_rect, sizeof(hi_drv_rect), &vi_attr_p->config.crop_rect, sizeof(hi_drv_rect));
    ret = drv_dsc_get_wh(vi_instance_p, &vi_ctrls_p->crop_rect.rect_w, &vi_ctrls_p->crop_rect.rect_h);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    if ((vi_ctrls_p->crop_rect.rect_y + vi_ctrls_p->crop_rect.rect_h) > vi_instance_p->attr_in.height) {
        vi_ctrls_p->crop_rect.rect_h = vi_instance_p->attr_in.height - vi_ctrls_p->crop_rect.rect_y;
    }

    drv_dsc_get_proc_dbg_flag(vi_instance_p, VI_PROC_DBG_COMMAND_CROP, HI_NULL);

    return HI_SUCCESS;
}

hi_s32 vi_dsc_get_out_attr(vi_instance *vi_instance_p)
{
    hi_s32 ret;
    vi_in_attr *vi_in_attr_p = HI_NULL;
    vi_out_attr *vi_out_attr_p = HI_NULL;
    vi_ctrls_attr *vi_ctrls_p = HI_NULL;

    vi_in_attr_p = &vi_instance_p->attr_in;
    vi_out_attr_p = &vi_instance_p->attr_out;
    vi_ctrls_p = &vi_instance_p->attr_ctrls;

    vi_out_attr_p->hdr_type = vi_in_attr_p->hdr_type;
    vi_out_attr_p->video_3d_type = vi_in_attr_p->video_3d_type;
    vi_out_attr_p->width = vi_ctrls_p->crop_rect.rect_w;
    vi_out_attr_p->height = vi_ctrls_p->crop_rect.rect_h;
    vi_out_attr_p->interlace = vi_in_attr_p->interlace;
    vi_out_attr_p->color_sys = vi_in_attr_p->color_sys;

    if (vi_in_attr_p->rate > VI_MAX_OUTPUT_FRAMERATE) {
        vi_out_attr_p->frame_rate = VI_MAX_OUTPUT_FRAMERATE;
    } else {
        vi_out_attr_p->frame_rate = vi_in_attr_p->rate;
    }

    ret = drv_dsc_get_out_pix_fmt(vi_instance_p, &vi_out_attr_p->pix_fmt);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = drv_dsc_get_out_color_spcae(vi_instance_p, &vi_out_attr_p->color_descript);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* 默认输出视频位宽与输入相同，大于10bit按10bit输出 */
    ret = drv_dsc_get_out_bitwidth(vi_instance_p, &vi_out_attr_p->bit_width);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    /* set output Y/C addr and stride */
    ret = vi_comm_get_y_stride(vi_out_attr_p->width, vi_out_attr_p->bit_width, &vi_out_attr_p->y_stride);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = vi_comm_get_c_stride(vi_out_attr_p->y_stride, vi_out_attr_p->pix_fmt, &vi_out_attr_p->c_stride);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 vi_dsc_updata_vi_attr(vi_instance *vi_instance_p)
{
    hi_s32 ret;

    ret = vi_dsc_get_in_attr(vi_instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = vi_dsc_get_ctrl_attr(vi_instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = vi_dsc_get_out_attr(vi_instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}
