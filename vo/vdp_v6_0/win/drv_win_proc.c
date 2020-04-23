/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: functions define
* Author: VDP
* Create: 2019-8-13
*/

#include "hi_type.h"
#include "hi_errno.h"
#include "drv_xdp_osal.h"

#include "hi_drv_video.h"
#include "hi_drv_disp.h"
#include "hi_drv_win.h"
#include "hi_drv_sys.h"
#include "hi_drv_proc.h"
#include "hi_osal.h"

#include "drv_win_common.h"
#include "drv_win_vpss.h"
#include "drv_win_buffer.h"
#include "drv_win_frc.h"
#include "drv_win_mng.h"

#include "drv_window_priv.h"
#include "drv_win_common.h"
#include "drv_display.h"
#include "drv_window.h"
#include "hal_layer_intf.h"
#include "drv_xdp_ion.h"

#define ARG_LENGTH_MAX 256
#define UNKONE_FMT_LENGTH 20
#define BOOL_INDEX_MAX 3
#define FRAME_RATE_FACTOR 1000
#define PARA_NUM_MAX 4
#define OUT_RECT_PARA_NUM 4
#define CROP_PARA_NUM 4
#define TPLAY_PARA_NUM 2
#define FLIP_PARA_NUM 2
#define PARA_LENGTH 10

#define VDP_FILE_OPERATE_RIGHTS 0644
#define PATH_NAME_MAX_LEN 256
#define FILE_NAME_MAX_LEN 64

#define WIN_PROC_NAME_LENGTH 12
#define WINDOW_INDEX_MASK 0x00000FFFl

typedef struct {
    const hi_char *command_name;
    hi_s32 (*debug_fun)(win_descriptor *win_descp, hi_char *arg2);
} win_proc_command;

hi_u8 g_pix_fmt_unknown[UNKONE_FMT_LENGTH] = { 0 };
hi_char g_win_proc_buffer[ARG_LENGTH_MAX];

hi_u8 *g_win_true_string[BOOL_INDEX_MAX] = {
    "False",
    "True ",
};

hi_u8 *g_win_tpye_string[HI_DRV_WIN_MAX + 1] = {
    "Display",
    "Virtual",
    "AI_WIN  ",
    "MAX",
};

hi_u8 *g_win_aspect_cvrs_string[HI_DRV_ASP_RAT_MODE_MAX + 1] = {
    "Full      ",
    "LetterBox ",
    "PanAndScan",
    "Combined  ",
    "FullHori  ",
    "FullVert  ",
    "Customer  ",
    "TV        ",
    "MAX",
};

hi_u8 *g_win_field_mode_string[HI_DRV_FIELD_MAX + 1] = {
    "Top",
    "Bottom",
    "All",
    "MAX",
};

hi_u8 *g_win_frm_bit_width_string[HI_DRV_PIXEL_BITWIDTH_MAX + 1] = {
    "8-bit",
    "10-bit",
    "12-bit",
    "MAX",
};

hi_u8 *g_xdr_src_frm_type[HI_DRV_HDR_TYPE_MAX + 1] = {
    "SDR",
    "HDR10",
    "HLG",
    "CUVA",
    "JTP_SL_HDR",
    "HDR10PLUS",
    "DOLBYVISION",
    "MAX",
};

hi_u8 *g_win_cmp_type[HI_DRV_COMPRESS_MODE_MAX + 1] = {
    "OFF",
    "SEG_LOSS",
    "SEG_LOSSLESS",
    "LINE_LOSS",
    "LINE_LOSSLESS",
    "FRM_LOSS",
    "FRM_LOSSLESS",
    "MAX",
};

hi_u8 *g_win_rotation_angle[HI_DRV_ROT_ANGLE_MAX + 1] = {
    "0",
    "90",
    "180",
    "270",
    "MAX",
};

hi_u8 *g_win_field_type_string[HI_DRV_FIELD_MAX + 1] = {
    "Top",
    "Bottom",
    "All",
    "MAX",
};

hi_u8 *g_color_transfer_curve_string[HI_DRV_COLOR_TRANSFER_TYPE_MAX + 1] = {
    "GAMMA_SDR",
    "GAMMA_HDR",
    "PQ",
    "HLG",
    "XVYCC",
    "MAX",
};

hi_u8 *g_color_primary_string[HI_DRV_COLOR_PRIMARY_COLOR_MAX + 1] = {
    "UNSPECIFY",
    "BT601_525",
    "BT601_625",
    "BT709",
    "BT2020",
    "CIE1931_XYZ",
    "BT470_SYSM",
    "SMPTE_240M",
    "GENERIC_FILM",
    "SMPTE_RP431",
    "SMPTE_EG432_1",
    "EBU_TECH_3213E",
    "COLOR_MAX",
};

hi_u8 *g_color_matrix_coeffs_string[HI_DRV_COLOR_MATRIX_COEFFS_MAX + 1] = {
    "IDENTITY",
    "UNSPECIFY",
    "BT601_525",
    "BT601_625",
    "BT709",
    "BT2020_NON_CONSTANT",
    "BT2020_CONSTANT",
    "BT2100_ICTCP",
    "USFCC",
    "SMPTE_240M",
    "YCGCO",
    "ST2085",
    "CHROMAT_NON_CONSTANT",
    "CHROMAT_CONSTANT",
    "COEFFS_MAX",
};

hi_u8 *g_color_space_string[HI_DRV_COLOR_CS_MAX + 1] = {
    "YUV",
    "RGB",
    "MAX",
};

hi_u8 *g_color_range_string[HI_DRV_COLOR_RANGE_MAX + 1] = {
    "LIMITED",
    "FULL",
    "RANGE_MAX",
};

hi_u8 *g_win_buf_status[HI_DRV_WIN_FREEZE_MAX + 1] = {
    "DISABLE",
    "LAST",
    "BLACK",
    "MAX",
};

hi_u8 *g_win_frm_mode[WIN_FRAME_FORCE_MAX + 1] = {
    "CLOSE",
    "PROG",
    "INTERLEAVE",
    "MAX",
};

static hi_s32 osal_cmd_check(hi_u32 in_argc, hi_u32 aspect_argc, hi_void *arg, hi_void *private)
{
    if (in_argc < aspect_argc) {
        osal_printk("in_argc is %d, aspect_argc is %d\n", in_argc, aspect_argc);
        return HI_FAILURE;
    }

    if (private == HI_NULL) {
        osal_printk("private is null\n");
        return HI_FAILURE;
    }

    if (arg == HI_NULL) {
        osal_printk("arg is null\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 win_proc_help(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    osal_printk("------------------Win Debug Option----------------\n");
    osal_printk("echo help                    > /proc/msp/winXXXX  \n");
    osal_printk("echo step on/off             > /proc/msp/winXXXX  \n");
    osal_printk("echo next                    > /proc/msp/winXXXX  \n");
    osal_printk("echo vpss_bypass on/off      > /proc/msp/winXXXX  \n");
    osal_printk("echo test_pattern static_on/dynamic_on/off       > /proc/msp/winXXXX  \n");
    osal_printk("echo out_rect x y w h        > /proc/msp/winXXXX  \n");
    osal_printk("echo asp (0/1/2/3)full/let/pan/cmb   > /proc/msp/winXXXX  \n");
    osal_printk("echo pause   on/off          > /proc/msp/winXXXX  \n");
    osal_printk("echo reset   black/still     > /proc/msp/winXXXX  \n");
    osal_printk("echo freeze last/black/off   > /proc/msp/winXXXX  \n");
    osal_printk("echo order   up/down         > /proc/msp/winXXXX  \n");
    osal_printk("echo quick   on/off          > /proc/msp/winXXXX  \n");
    osal_printk("echo rota    0/90/180/270    > /proc/msp/winXXXX  \n");
    osal_printk("echo flip    hori,vert       > /proc/msp/winXXXX  \n");
    osal_printk("echo crop l r t b            > /proc/msp/winXXXX  \n");
    osal_printk("echo capture    path         > /proc/msp/winXXXX  \n");
    osal_printk("echo frm_mode   prog/interleave/close   > /proc/msp/winXXXX  \n");
    return HI_SUCCESS;
}

#ifdef CAPTURE_SUPPORT
/* ********************************************************************** */
/* win proc file operation */
/* ********************************************************************** */
static struct file *win_proc_fopen(const hi_char *filename, hi_s32 flags, hi_s32 mode)
{
    struct file *fp = HI_NULL;

    fp = osal_klib_fopen(filename, flags, mode);
    return (IS_ERR(fp)) ? HI_NULL : fp;
}

static hi_void win_proc_fclose(struct file *fp)
{
    if (fp != HI_NULL) {
        osal_klib_fclose(fp);
    }
}

static hi_s32 win_proc_fwrite(hi_char *buf, hi_s32 len, struct file *fp)
{
    hi_s32 writelen;

    if (fp == HI_NULL) {
        return -ENOENT;
    }

    if ((((fp->f_flags) & OSAL_O_ACCMODE) & (OSAL_O_WRONLY | OSAL_O_RDWR)) == 0) {
        return -EACCES;
    }

    writelen = osal_klib_user_fwrite(buf, len, fp);

    return writelen;
}
#endif

static hi_u8 *win_get_pix_fmt_string(hi_drv_pixel_format pixfmt)
{
    switch (pixfmt) {
        case HI_DRV_PIXEL_FMT_NV12:
            return "NV12";
        case HI_DRV_PIXEL_FMT_NV21:
            return "NV21";
        case HI_DRV_PIXEL_FMT_YUYV:
            return "YUYV";
        case HI_DRV_PIXEL_FMT_YYUV:
            return "YYUV";
        case HI_DRV_PIXEL_FMT_YVYU:
            return "YVYU";
        case HI_DRV_PIXEL_FMT_UYVY:
            return "UYVY";
        case HI_DRV_PIXEL_FMT_VYUY:
            return "VYUY";
        default:
            snprintf(g_pix_fmt_unknown, UNKONE_FMT_LENGTH, "%d", pixfmt);
            return g_pix_fmt_unknown;
    }
}

hi_void win_proc_buf_play_ctrl_info(hi_void *p, win_play_ctrl_s *win_play_ctrl)
{
    osal_proc_print(p, "%-19s:%-20s", "Quickout", g_win_true_string[win_play_ctrl->quickout_enalbe]);
    osal_proc_print(p, "%-19s:%-20s\n", "|Pause", g_win_true_string[win_play_ctrl->pause_enalbe]);
    osal_proc_print(p, "%-19s:%-20s\n", "Freeze_ctrl", g_win_buf_status[win_play_ctrl->freeze_ctrl]);
    return;
}

hi_void win_proc_hdr_info(hi_void *p, hi_drv_hdr_static_metadata *hdr10_info)
{
    osal_proc_print(p, "%-40s:%-10d/\n", "primaries_x0", hdr10_info->mastering_info.display_primaries_x[0]);
    osal_proc_print(p, "%-40s:%-10d/\n", "primaries_x1", hdr10_info->mastering_info.display_primaries_x[1]);
    osal_proc_print(p, "%-40s:%-10d/\n", "primaries_x2",
        hdr10_info->mastering_info.display_primaries_x[2]); /* 2 : param index */
    osal_proc_print(p, "%-40s:%-10d/\n", "primaries_y0", hdr10_info->mastering_info.display_primaries_y[0]);
    osal_proc_print(p, "%-40s:%-10d/\n", "primaries_y1", hdr10_info->mastering_info.display_primaries_y[1]);
    osal_proc_print(p, "%-40s:%-10d/\n", "primaries_y2",
        hdr10_info->mastering_info.display_primaries_y[2]); /* 2 : param index */
    osal_proc_print(p, "%-40s:%-10d/\n", "white_point_x", hdr10_info->mastering_info.white_point_x);
    osal_proc_print(p, "%-40s:%-10d/\n", "white_point_y", hdr10_info->mastering_info.white_point_y);
    osal_proc_print(p, "%-40s:%-10d/\n", "max_display_mastering_luminance",
        hdr10_info->mastering_info.max_display_mastering_luminance);
    osal_proc_print(p, "%-40s:%-10d/\n", "min_display_mastering_luminance",
        hdr10_info->mastering_info.min_display_mastering_luminance);
    osal_proc_print(p, "%-40s:%-10d/\n", "max_pic_average_light_level",
        hdr10_info->content_info.max_pic_average_light_level);
    osal_proc_print(p, "%-40s:%-10d/\n", "max_content_light_level", hdr10_info->content_info.max_content_light_level);
    return;
}

hi_void win_proc_basic_attr_frm_info(win_descriptor *win_descp, hi_void *p, hi_drv_video_frame *new_frame)
{
    osal_proc_print(p, "%-19s:%-20s", "Enable", g_win_true_string[win_descp->win_play_ctrl.win_enable]);
    osal_proc_print(p, "%-19s:%-10s/%-9s\n", "|Type/PixFmt", g_win_cmp_type[new_frame->cmp_info.cmp_mode],
                    win_get_pix_fmt_string(new_frame->pixel_format));
    osal_proc_print(p, "%-19s:%-20s", "Rotation", g_win_rotation_angle[win_descp->win_user_attr.rotation]);
    osal_proc_print(p, "%-19s:%-10s/%-9s\n", "|VerFlip/HoriFlip",
                    g_win_true_string[win_descp->win_user_attr.vert_flip_enable],
                    g_win_true_string[win_descp->win_user_attr.horz_flip_enable]);
    osal_proc_print(p, "%-19s:%-20s", "Type", g_win_tpye_string[win_descp->win_type]);
    osal_proc_print(p, "%-19s:%4d/%4d(%4d:%4d)\n", "|W/H(Aspect W:H)", new_frame->width, new_frame->height,
                    (hi_u32)new_frame->aspect_width, (hi_u32)new_frame->aspect_height);
    osal_proc_print(p, "%-19s:%-10d/%-9d", "LayerID/LayerOrder", win_descp->expect_win_layermapping.layer_id,
                    win_descp->expect_win_layermapping.layer_order);
    osal_proc_print(p, "%-19s:%4d/%4d/%4d/%4d \n", "|Disp(X/Y/W/H)", new_frame->disp_rect.rect_x,
                    new_frame->disp_rect.rect_y, new_frame->disp_rect.rect_w, new_frame->disp_rect.rect_h);
    osal_proc_print(p, "%-19s:%-20s", "AspectRatioConvert",
                    g_win_aspect_cvrs_string[win_descp->win_user_attr.aspect_ratio_mode]);
    osal_proc_print(p, "%-19s:%d.%.3d\n", "|FrameRate", new_frame->frame_rate / FRAME_RATE_FACTOR,
                    new_frame->frame_rate - (new_frame->frame_rate / FRAME_RATE_FACTOR * FRAME_RATE_FACTOR));
    osal_proc_print(p, "%-19s:%-4d:%-15d \n", "CustAspectRatio",
                    (hi_u32)win_descp->win_user_attr.user_def_aspect_ratio.aspect_ratio_w,
                    (hi_u32)win_descp->win_user_attr.user_def_aspect_ratio.aspect_ratio_h);
    osal_proc_print(p, "%-19s:%-20s", "Crop", g_win_true_string[win_descp->win_user_attr.user_crop_enable]);
    osal_proc_print(p, "%-19s:%s(%s)\n", "|Fieldmode(Origin)", g_win_field_type_string[new_frame->field_mode], "NA");
    osal_proc_print(p, "%-19s:%4d/%4d/%4d/%4d ", "Crop(L/T/R/B) ", win_descp->win_user_attr.crop_rect.left_offset,
                    win_descp->win_user_attr.crop_rect.top_offset, win_descp->win_user_attr.crop_rect.right_offset,
                    win_descp->win_user_attr.crop_rect.bottom_offset);
    osal_proc_print(p, "%-19s:%d/%d/%s/%s\n", "|OriRect(X/Y/W/H)", 0, 0, "NA", "NA");
    osal_proc_print(p, "%-19s:%4d/%4d/%4d/%4d ", "In  (X/Y/W/H)", win_descp->win_user_attr.in_rect.x,
                    win_descp->win_user_attr.in_rect.y, win_descp->win_user_attr.in_rect.width,
                    win_descp->win_user_attr.in_rect.height);
    osal_proc_print(p, "%-19s:0x%x\n", "|FrameIndex", new_frame->frame_index);
    osal_proc_print(p, "%-19s:%4d/%4d/%4d/%4d ", "Out(X/Y/W/H)", win_descp->win_user_attr.out_rect.x,
                    win_descp->win_user_attr.out_rect.y, win_descp->win_user_attr.out_rect.width,
                    win_descp->win_user_attr.out_rect.height);
    osal_proc_print(p, "%-19s:0x%llx/0x%llx\n", "|SrcPTS/PTS", new_frame->src_pts, new_frame->pts);
    osal_proc_print(p, "%-19s:%-20s\n", "ColorSpace",
                    g_color_space_string[new_frame->color_space.color_space]);
    osal_proc_print(p, "%-19s:%-20s\n", "range",
                    g_color_range_string[new_frame->color_space.quantify_range]);
    osal_proc_print(p, "%-19s:%-20s\n", "matrix_coeffs",
                    g_color_matrix_coeffs_string[new_frame->color_space.matrix_coef]);
    osal_proc_print(p, "%-19s:%-20s\n", "color_primary",
                    g_color_primary_string[new_frame->color_space.color_primary]);
    osal_proc_print(p, "%-19s:%-20s\n", "transfer_curve",
                    g_color_transfer_curve_string[new_frame->color_space.transfer_type]);
    win_proc_buf_play_ctrl_info(p, &win_descp->win_play_ctrl);
    win_proc_hdr_info(p, &new_frame->hdr_info.hdr10_info);
    return;
}

hi_void win_proc_more_attr_frm_info(win_descriptor *win_descp, hi_void *p, hi_drv_video_frame *new_frame)
{
    osal_proc_print(p, "%-19s:%-10s/%-9s", "DispMode/RightFirst", "NA",
                    g_win_true_string[win_descp->win_user_attr.right_eye_first_enable]);
    osal_proc_print(p, "%-19s:%d\n", "|PlayTime", new_frame->video_private.priv_disp_time);
    osal_proc_print(p, "%-19s:%-20s\n", "|FieldMode", g_win_field_mode_string[new_frame->field_mode]);
    osal_proc_print(p, "%-19s:%-20s", "AttachSource", g_win_true_string[win_descp->src_handle != HI_INVALID_HANDLE]);
    osal_proc_print(p, "%-19s:%s\n", "|Fidelity", "NA");

    osal_proc_print(p, "%-19s:0x%llx/0x%x\n", "|H8:YAddr/YStride", new_frame->buf_addr[0].dma_handle,
                    new_frame->buf_addr[0].stride_y);
    osal_proc_print(p, "%-19s:0x%llx/0x%x\n", "|L2:YAddr/YStride", new_frame->buf_addr_lb[0].dma_handle,
                    new_frame->buf_addr_lb[0].stride_y);

    osal_proc_print(p, "%-19s:%-20s", "Frame Mode", g_win_frm_mode[win_descp->win_play_ctrl.frame_mode]);
    osal_proc_print(p, "%-19s:%-20s\n", "|Frame BitWidth", g_win_frm_bit_width_string[new_frame->bit_width]);
    osal_proc_print(p, "%-19s:%-20s", "OpenHdr", "NA");
    osal_proc_print(p, "%-19s:%-20s\n", "|Frame Type", g_xdr_src_frm_type[new_frame->hdr_type]);
    osal_proc_print(p, "%-19s:%-20s|\n", "HisiHdr", g_win_true_string[win_descp->win_user_attr.hisi_hdr]);

    if (win_descp->slave_win) {
        osal_proc_print(p, "%-19s:%04X                |\n", "SlaveWinID", (hi_u32)(win_descp->slave_win & 0xFFFFul));
    } else {
        osal_proc_print(p, "%-19s:%04X                |\n", "SlaveWinID", (hi_u32)0xFFFFul);
    }

    osal_proc_print(p, "%-19s:%-20X|\n", "UsingLayerRegionNum", win_descp->expect_win_layermapping.layer_region_id);
    osal_proc_print(p, "%-19s:%-20X|\n", "UsingZorder", win_descp->expected_win_attr.win_zorder);
    osal_proc_print(p, "%-19s:%4d/%4d/%4d/%4d \n", "ExpectedCropRect(X/Y/W/H)",
                    win_descp->expected_win_attr.crop_rect.x,
                    win_descp->expected_win_attr.crop_rect.y, win_descp->expected_win_attr.crop_rect.width,
                    win_descp->expected_win_attr.crop_rect.height);
    osal_proc_print(p, "%-19s:%4d/%4d/%4d/%4d \n", "ExpectedVideoRect(X/Y/W/H)",
                    win_descp->expected_win_attr.video_rect.x, win_descp->expected_win_attr.video_rect.y,
                    win_descp->expected_win_attr.video_rect.width, win_descp->expected_win_attr.video_rect.height);
    osal_proc_print(p, "%-19s:%4d/%4d/%4d/%4d \n", "ExpectedDispRect(X/Y/W/H)",
                    win_descp->expected_win_attr.disp_rect.x,
                    win_descp->expected_win_attr.disp_rect.y, win_descp->expected_win_attr.disp_rect.width,
                    win_descp->expected_win_attr.disp_rect.height);
    return;
}

hi_void win_proc_output_playctrl_info(hi_handle win_handle, hi_void *p)
{
    win_proc_play_status win_status;

    drv_win_proc_get_playstatus(win_handle, &win_status);

    osal_proc_print(p, "------------------------------Win play ctrl stastics--------------------------\n");
    osal_proc_print(p, "%-30s:%d/%d\n", "Src queue/dequeue cnts", win_status.aquire_frame_cnt,
                    win_status.rls_frame_cnt);
    osal_proc_print(p, "%-30s:%4d/%4d \n", "Que interval time(current/max)", win_status.queue_interval,
                    win_status.max_queue_interval);
    osal_proc_print(p, "%-30s:%4d/%4d/%4d/%4d\n", "Fence src (destroy/access/rls/rls_atonce)",
                    win_status.src_fence_destroy_cnts, win_status.src_fence_access_cnts,
                    win_status.src_fence_rls_cnts,
                    win_status.src_rls_atonce_cnts);
    osal_proc_print(p, "%-30s:%4d/%4d/\n", "Fence src (time_outcnt/fence_id/)", win_status.src_fence_timeout_cnts,
                    win_status.src_fence_timeout_fd);

    osal_proc_print(p, "%-30s:%4d/%4d \n", "Fence sink (creat/signal/)", win_status.sink_fence_create_cnts,
                    win_status.sink_fence_signal_cnts);

    osal_proc_print(p, "%-30s:%4d/%4d/%4d\n", "Frame stay time(min/max/current)", win_status.min_frame_stay_time,
                    win_status.max_frame_stay_time, win_status.newest_frame_stay_time);

    osal_proc_print(p, "%-30s:%4d/%4d \n", "tplay speed(int/decimal)", win_status.tplay_speed_integer,
                    win_status.tplay_speed_decimal);

    return;
}

hi_void win_proc_basic_info(win_descriptor *win_descp, hi_void *p)
{
    hi_drv_video_frame new_frame;

    if (win_descp->win_type != HI_DRV_AI_WIN) {
        if (win_descp->win_play_status.displaying_frame == HI_NULL) {
            WIN_ERROR("frame is null\n");
            win_proc_output_playctrl_info(win_descp->win_index, p);
            return;
        }

        new_frame = *win_descp->win_play_status.displaying_frame;
        win_proc_basic_attr_frm_info(win_descp, p, &new_frame);
        win_proc_more_attr_frm_info(win_descp, p, &new_frame);
        win_proc_output_playctrl_info(win_descp->win_index, p);
    }
    return;
}

static hi_s32 win_proc_set_step_mode(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    win_descriptor *win_descp = HI_NULL;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        return HI_FAILURE;
    }

    win_descp = winmanage_get_window(*(hi_handle *)private);
    if (win_descp == HI_NULL) {
        WIN_WARN("WIN is not exist! %#x\n", *(hi_handle *)private);
        return HI_FAILURE;
    }

    if (argv[1] != HI_NULL) {
        osal_printk("set window step_mode %s \n", argv[1]);

        if (osal_strncmp(argv[1], strlen(argv[1]), "on", strlen("on")) == 0) {
            win_descp->win_play_status.is_stepmode = HI_TRUE;
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "off", strlen("off")) == 0) {
            win_descp->win_play_status.is_stepmode = HI_FALSE;
        } else {
            return HI_FAILURE;
        }
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 win_proc_set_pause_mode(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        return HI_FAILURE;
    }

    if (argv[1] != HI_NULL) {
        osal_printk("set window pause %s \n", argv[1]);

        if (osal_strncmp(argv[1], strlen(argv[1]), "on", strlen("on")) == 0) {
            drv_win_pause(*(hi_handle *)private, HI_TRUE);
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "off", strlen("off")) == 0) {
            drv_win_pause(*(hi_handle *)private, HI_FALSE);
        } else {
            return HI_FAILURE;
        }
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


static hi_s32 win_proc_set_reset_mode(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        return HI_FAILURE;
    }

    if (argv[1] != HI_NULL) {
        osal_printk("set window reset %s \n", argv[1]);

        if (osal_strncmp(argv[1], strlen(argv[1]), "black", strlen("black")) == 0) {
            drv_win_reset(*(hi_handle *)private, HI_DRV_WIN_RESET_BLACK);
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "still", strlen("still")) == 0) {
            drv_win_reset(*(hi_handle *)private, HI_DRV_WIN_RESET_LAST);
        } else {
            return HI_FAILURE;
        }
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 win_proc_set_quickout_mode(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        return HI_FAILURE;
    }

    if (argv[1] != HI_NULL) {
        osal_printk("set window quickout %s \n", argv[1]);

        if (osal_strncmp(argv[1], strlen(argv[1]), "on", strlen("on")) == 0) {
            drv_win_set_quickout(*(hi_handle *)private, HI_TRUE);
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "off", strlen("off")) == 0) {
            drv_win_set_quickout(*(hi_handle *)private, HI_FALSE);
        } else {
            return HI_FAILURE;
        }
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 win_proc_set_order(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        return HI_FAILURE;
    }

    if (argv[1] != HI_NULL) {
        if (osal_strncmp(argv[1], strlen(argv[1]), "up", strlen("up")) == 0) {
            drv_win_set_zorder(*(hi_handle *)private, HI_DRV_DISP_ZORDER_MOVEUP);
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "down", strlen("down")) == 0) {
            drv_win_set_zorder(*(hi_handle *)private, HI_DRV_DISP_ZORDER_MOVEDOWN);
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "top", strlen("top")) == 0) {
            drv_win_set_zorder(*(hi_handle *)private, HI_DRV_DISP_ZORDER_MOVETOP);
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "bottom", strlen("bottom")) == 0) {
            drv_win_set_zorder(*(hi_handle *)private, HI_DRV_DISP_ZORDER_MOVEBOTTOM);
        } else {
            return HI_FAILURE;
        }
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 win_proc_set_rotation(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        return HI_FAILURE;
    }

    if (argv[1] != HI_NULL) {
        if (osal_strncmp(argv[1], strlen(argv[1]), "0", strlen("0")) == 0) {
            drv_win_set_rotation(*(hi_handle *)private, HI_DRV_ROT_ANGLE_0);
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "90", strlen("90")) == 0) {
            drv_win_set_rotation(*(hi_handle *)private, HI_DRV_ROT_ANGLE_90);
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "180", strlen("180")) == 0) {
            drv_win_set_rotation(*(hi_handle *)private, HI_DRV_ROT_ANGLE_180);
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "270", strlen("270")) == 0) {
            drv_win_set_rotation(*(hi_handle *)private, HI_DRV_ROT_ANGLE_270);
        } else {
            return HI_FAILURE;
        }
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 win_proc_set_flip(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 para[FLIP_PARA_NUM] = {0};

    if (osal_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is para index */
        return HI_FAILURE;
    }

    if ((argv[1] != HI_NULL)
        && (argv[2] != HI_NULL)) { /* 2 is para index */
        para[0] = (int)osal_strtol(argv[1], NULL, PARA_LENGTH);
        para[1] = (int)osal_strtol(argv[2], NULL, PARA_LENGTH); /* 2 is para index */
        if (((para[0] != HI_FALSE) && (para[0] != HI_TRUE)) ||
            ((para[1] != HI_FALSE) && ((para[1] != HI_TRUE)))) {
            return HI_FAILURE;
        }

        ret = drv_win_set_flip(*(hi_handle *)private, para[0], para[1]);
    } else {
        return HI_FAILURE;
    }

    return ret;
}

static hi_s32 win_proc_next_frame_queue(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    win_descriptor *win_descp = HI_NULL;

    if (osal_cmd_check(argc, 1, private, private) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    win_descp = winmanage_get_window(*(hi_handle *)private);
    if (win_descp == HI_NULL) {
        WIN_WARN("WIN is not exist! %#x\n", *(hi_handle *)private);
        return HI_FAILURE;
    }

    win_descp->win_play_status.is_next_frm = HI_TRUE;
    return HI_SUCCESS;
}

static hi_s32 win_proc_set_vpss_bypss(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        return HI_FAILURE;
    }

    if (argv[1] != HI_NULL) {
        osal_printk("set win vpss set bypass %s \n", argv[1]);
        if (osal_strncmp(argv[1], strlen(argv[1]), "on", strlen("on")) == 0) {
            win_vpss_proc_set_bypass(*(hi_handle *)private, HI_TRUE);
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "off", strlen("off")) == 0) {
            win_vpss_proc_set_bypass(*(hi_handle *)private, HI_FALSE);
        } else {
            return HI_FAILURE;
        }
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#define TEST_PATTERN_SPEED 5
static hi_s32 win_proc_set_test_pattern(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    vdp_fdr_testpattern debug;
    win_descriptor *win_descp = HI_NULL;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        return HI_FAILURE;
    }

    win_descp = winmanage_get_window(*(hi_handle *)private);
    if (win_descp == HI_NULL) {
        WIN_WARN("WIN is not exist! %#x\n", *(hi_handle *)private);
        return HI_FAILURE;
    }

    memset(&debug, 0, sizeof(debug));

    if (argv[1] != HI_NULL) {
        osal_printk("set win testpattern %s \n", argv[1]);

        if (osal_strncmp(argv[1], strlen(argv[1]), "static_on", strlen("static_on")) == 0) {
            debug.pattern_en = HI_TRUE;
            debug.demo_mode = VDP_FDR_TESTPTTN_MODE_STATIC;
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "dynamic_on", strlen("dynamic_on")) == 0) {
            debug.pattern_en = HI_TRUE;
            debug.demo_mode = VDP_FDR_TESTPTTN_MODE_DYNAMIC;
            debug.color_mode = VDP_FDR_TESTPTTN_CLR_MODE_NORM;
            debug.line_width = VDP_FDR_TESTPTTN_LINEW_2P;
            debug.speed = TEST_PATTERN_SPEED;
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "off", strlen("off")) == 0) {
            debug.pattern_en = HI_FALSE;
        } else {
            return HI_FAILURE;
        }
        hal_layer_debug_pattern(win_descp->expect_win_layermapping.layer_id, &debug);
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 win_proc_set_freeze(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_win_freeze_mode freeze_mode;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        return HI_FAILURE;
    }

    if (argv[1] != HI_NULL) {
        osal_printk("set win freeze %s \n", argv[1]);

        if (osal_strncmp(argv[1], strlen(argv[1]), "last", strlen("last")) == 0) {
            freeze_mode = HI_DRV_WIN_FREEZE_LAST;
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "black", strlen("black")) == 0) {
            freeze_mode = HI_DRV_WIN_FREEZE_BLACK;
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "off", strlen("off")) == 0) {
            freeze_mode = HI_DRV_WIN_FREEZE_DISABLE;
        } else {
            return HI_FAILURE;
        }
        drv_win_freeze(*(hi_handle *)private, freeze_mode);
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#ifdef CAPTURE_SUPPORT
static hi_s32 win_proc_check_and_set_save_path(hi_char *path, hi_u32 path_len, hi_u32 max_len, hi_char *vdp_save_path,
    hi_drv_video_frame *cur_frame)
{
    hi_u32 i = 0;
    hi_u32 j = 0;
    hi_char tmp_path[PATH_NAME_MAX_LEN] = "";
    struct tm now;

    time_to_tm(get_seconds(), 0, &now);

    for (; (i < path_len) && (j < max_len - 1); i++) {
        if ((j == 0) && (path[i] == ' ')) {
            continue;
        }
        if (path[i] > ' ') {
            vdp_save_path[j++] = path[i];
        }
        if ((j > 0) && (path[i] <= ' ')) {
            break;
        }
    }

    vdp_save_path[j] = 0;

    if ((vdp_save_path[0] == '/') && (j > 0)) {
        if (vdp_save_path[j - 1] == '/') {
            vdp_save_path[j - 1] = 0;
        }
        if (j >= max_len) {
            WIN_ERROR("save path name is too long, please reset it\n");
            return HI_FAILURE;
        }
        strncpy(tmp_path, vdp_save_path, j);
    }

    snprintf(vdp_save_path, max_len, "%s/vdp_0x%llx_%d_%d_%02u_%02u_%02u.yuv", tmp_path,
        cur_frame->buf_addr[HI_DRV_3D_EYE_LEFT].start_addr, cur_frame->width, cur_frame->height, now.tm_hour,
        now.tm_min, now.tm_sec);
    vdp_save_path[max_len - 1] = '\0';
    WIN_WARN("******* VDP save path: %s ********\n", vdp_save_path);
    return HI_SUCCESS;
}

static hi_s32 win_proc_save_yuv_img(hi_drv_video_frame *cur_frame, hi_char *path, hi_u32 path_len)
{
    struct file *fp = HI_NULL;
    hi_u64 dma_buf_handle;
    hi_s32 ret;
    drv_xdp_mem_info fd_info = { 0 };
    hi_s32 len;
    hi_char vdp_save_path[PATH_NAME_MAX_LEN];
    hi_s32 size;

    dma_buf_handle = cur_frame->buf_addr[HI_DRV_3D_EYE_LEFT].dma_handle;
    if (dma_buf_handle == HI_NULL) {
        WIN_WARN("dma handle is null.\n");
        return HI_FAILURE;
    }

    ret = drv_xdp_mem_map_dma_handle((struct dma_buf *)dma_buf_handle, &fd_info);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("get y phy addr failed!\n");
        return ret;
    }

    ret = win_proc_check_and_set_save_path(path, path_len, PATH_NAME_MAX_LEN, vdp_save_path, cur_frame);
    if (ret != HI_SUCCESS) {
        goto ERR_UNMAP_DMA;
    }

    fp = win_proc_fopen(vdp_save_path, O_RDWR | O_CREAT, VDP_FILE_OPERATE_RIGHTS);
    if (fp == HI_NULL) {
        WIN_ERROR("failed to open file");
        goto ERR_UNMAP_DMA;
    }

    /* 3 / 2 is the storage ratio between y data and uv data */
    size = cur_frame->height * cur_frame->buf_addr[HI_DRV_3D_EYE_LEFT].stride_y * 3 / 2;
    len = win_proc_fwrite(fd_info.vir_addr, size, fp);
    if (len != size) {
        WIN_ERROR("capture pic failed");
        goto ERR_FILE_CLOSE;
    }

    drv_xdp_mem_unmap_dma_handle((struct dma_buf *)dma_buf_handle, &fd_info);
    win_proc_fclose(fp);

    return HI_SUCCESS;

ERR_FILE_CLOSE:
    win_proc_fclose(fp);
ERR_UNMAP_DMA:
    drv_xdp_mem_unmap_dma_handle((struct dma_buf *)dma_buf_handle, &fd_info);
    return HI_FAILURE;
}

hi_s32 win_proc_capture(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_drv_video_frame cur_frame = { 0 };
    hi_char *path = arg2;
    hi_u32 path_len = 0;

    /* get currently displaying frame */
    ret = drv_win_acquire_captured_frame(win_descp->win_index, &cur_frame);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("call drv_win_acquire_captured_frame failed\n");
        return ret;
    }

    /* calculate char nubmer of path string */
    while ((*path != ' ') && (*path != '\0')) {
        path++;
        path_len++;
    }
    path_len++;

    /* save yuv frame */
    ret = win_proc_save_yuv_img(&cur_frame, arg2, path_len);

    drv_win_release_captured_frame(win_descp->win_index, &cur_frame);

    return ret;
}
#endif

static hi_s32 win_proc_set_out_rect(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_win_attr win_attr = { 0 };
    hi_s32 ret;

    if (osal_cmd_check(argc, 5, private, private) != HI_SUCCESS) { /* 5 is para index */
        return HI_FAILURE;
    }

    if ((argv[1] != HI_NULL)
        && (argv[2] != HI_NULL) /* 2 is para index */
        && (argv[3] != HI_NULL) /* 3 is para index */
        && (argv[4] != HI_NULL)) { /* 4 is para index */
        ret = drv_win_get_attr(*(hi_handle *)private, &win_attr);
        if (ret != HI_SUCCESS) {
            return ret;
        }
        win_attr.out_rect.x =
            (int)osal_strtol(argv[1], NULL, PARA_LENGTH);
        win_attr.out_rect.y =
            (int)osal_strtol(argv[2], NULL, PARA_LENGTH); /* 2 is para index */
        win_attr.out_rect.width =
            (int)osal_strtol(argv[3], NULL, PARA_LENGTH); /* 3 is para index */
        win_attr.out_rect.height =
            (int)osal_strtol(argv[4], NULL, PARA_LENGTH); /* 4 is para index */
        osal_printk("set outrect (x = %d, y = %d, w = %d, h = %d) \n",
                    win_attr.out_rect.x, win_attr.out_rect.y,
                    win_attr.out_rect.width, win_attr.out_rect.height);
        ret = drv_win_set_attr(*(hi_handle *)private, &win_attr);
    } else {
        return HI_FAILURE;
    }

    return ret;
}

static hi_s32 win_proc_set_crop_rect(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_win_attr win_attr = { 0 };
    hi_s32 ret;

    if (osal_cmd_check(argc, 5, private, private) != HI_SUCCESS) { /* 5 is para index */
        return HI_FAILURE;
    }

    if ((argv[1] != HI_NULL)
        && (argv[2] != HI_NULL) /* 2 is para index */
        && (argv[3] != HI_NULL) /* 3 is para index */
        && (argv[4] != HI_NULL)) { /* 4 is para index */
        ret = drv_win_get_attr(*(hi_handle *)private, &win_attr);
        if (ret != HI_SUCCESS) {
            return ret;
        }
        win_attr.use_crop_rect = HI_TRUE;
        win_attr.crop_rect.left_offset =
            (int)osal_strtol(argv[1], NULL, PARA_LENGTH);
        win_attr.crop_rect.right_offset =
            (int)osal_strtol(argv[2], NULL, PARA_LENGTH); /* 2 is para index */
        win_attr.crop_rect.top_offset =
            (int)osal_strtol(argv[3], NULL, PARA_LENGTH); /* 3 is para index */
        win_attr.crop_rect.bottom_offset =
            (int)osal_strtol(argv[4], NULL, PARA_LENGTH); /* 4 is para index */
        osal_printk("set crop_rect (x = %d, y = %d, w = %d, h = %d) \n",
                    win_attr.use_crop_rect,
                    win_attr.crop_rect.left_offset,
                    win_attr.crop_rect.right_offset,
                    win_attr.crop_rect.top_offset,
                    win_attr.crop_rect.bottom_offset);
        ret = drv_win_set_attr(*(hi_handle *)private, &win_attr);
    } else {
        return HI_FAILURE;
    }

    return ret;
}

static hi_s32 win_proc_set_frame_mode(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    win_descriptor *win_descp = HI_NULL;
    hi_drv_win_forceframeprog_flag frame_mode;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        return HI_FAILURE;
    }

    win_descp = winmanage_get_window(*(hi_handle *)private);
    if (win_descp == HI_NULL) {
        WIN_WARN("WIN is not exist! %#x\n", *(hi_handle *)private);
        return HI_FAILURE;
    }

    if (argv[1] != HI_NULL) {
        osal_printk("set frame mode %s \n", argv[1]);
        if (osal_strncmp(argv[1], strlen(argv[1]), "prog", strlen("prog")) == 0) {
            frame_mode = WIN_FRAME_FORCE_PROG;
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "interleave", strlen("interleave")) == 0) {
            frame_mode = WIN_FRAME_FORCE_INTERLEAVE;
        } else if (osal_strncmp(argv[1], strlen(argv[1]), "close", strlen("close")) == 0) {
            frame_mode = WIN_FRAME_FORCE_CLOSE;
        } else {
            return HI_FAILURE;
        }
    } else {
        return HI_FAILURE;
    }
    ret = drv_win_set_frame_proginterleave_mode(win_descp->win_index, frame_mode);
    if (ret == HI_SUCCESS) {
        win_descp->win_play_ctrl.frame_mode = frame_mode;
    }
    return ret;
}

static hi_s32 win_proc_set_asp(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_drv_win_attr win_attr = { 0 };

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        return HI_FAILURE;
    }

    ret = drv_win_get_attr(*(hi_handle *)private, &win_attr);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (argv[1] != HI_NULL) {
        win_attr.aspect_ratio_mode = (hi_drv_asp_rat_mode)osal_strtol(argv[1], NULL, PARA_LENGTH);

        if ((win_attr.aspect_ratio_mode <= HI_DRV_ASP_RAT_MODE_COMBINED)) {
            osal_printk("set asp mode: %s", g_win_aspect_cvrs_string[win_attr.aspect_ratio_mode]);
            ret = drv_win_set_attr(*(hi_handle *)private, &win_attr);
        }
    }

    return ret;
}

hi_void win_sync_proc_cnt_info(win_avsync_info *avsync_info, hi_void *p)
{
    osal_proc_print(p, "----------------------------------sync count info(mode-%d)-------------------------------\n",
                    avsync_info->sync_mode);
    osal_proc_print(p, "%-40s:%-10d/%-10d\n", "discard_cnt/repeat_cnt", avsync_info->debug_discard_cnt,
                    avsync_info->debug_repeat_cnt);
    osal_proc_print(p, "%-40s:%-10d/%-10d\n", "all_play_cnt/adjust", avsync_info->all_need_play_cnt,
                    avsync_info->need_sync_adjust);
    osal_proc_print(p, "%-40s:%-10d/%-10d\n", "disp_frame_index/disp_frame_cnt",
                    avsync_info->display_frame_play_index,
                    avsync_info->display_frame_need_play_cnt);
    return;
}

hi_s32 win_proc_set_sync(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_drv_win_sync sync_mode;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        return HI_FAILURE;
    }

    if (argv[1] != HI_NULL) {
        sync_mode = (int)osal_strtol(argv[1], NULL, PARA_LENGTH);
        ret = drv_win_set_sync(*(hi_handle *)private, sync_mode);
    } else {
        return HI_FAILURE;
    }

    return ret;
}

hi_s32 win_proc_get_play_info(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_drv_win_play_info play_info;

    ret = drv_win_get_play_info(*(hi_handle *)private, &play_info);
    osal_printk("remain_frame_num :%d\n", play_info.remain_frame_num);
    osal_printk("delay time:%d\n", play_info.delay_time);
    osal_printk("out put frame rate:%d\n", play_info.out_put_frame_rate);
    osal_printk("underload times:%d\n", play_info.underload_times);
    return ret;
}

hi_s32 win_proc_set_tplay(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_drv_win_play_ctrl playctrl_info = { 0 };

    if (osal_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is para index */
        return HI_FAILURE;
    }

    if ((argv[1] != HI_NULL)
        && (argv[2] != HI_NULL)) { /* 2 is para index */
        playctrl_info.speed_integer = (int)osal_strtol(argv[1], NULL, PARA_LENGTH);
        playctrl_info.speed_decimal = (int)osal_strtol(argv[2], NULL, PARA_LENGTH); /* 2 is para index */

        ret = drv_win_set_playctl_info(*(hi_handle *)private, &playctrl_info);
    } else {
        return HI_FAILURE;
    }

    return ret;
}

static hi_s32 win_proc_info(hi_handle win_handle, hi_void *p)
{
    win_descriptor *win_descp = HI_NULL;

    win_descp = winmanage_get_window(win_handle);
    if (win_descp == HI_NULL) {
        WIN_WARN("WIN is not exist! %#x\n", win_handle);
        return HI_ERR_VO_WIN_NOT_EXIST;
    }

    osal_proc_print(p,
        "------------------------------Win%04x[type=%s]------------------------------------\n"
        "------------Win Info-------------------|-------------Frame Info-------------------\n",
        (hi_u32)(win_handle & WINDOW_INDEX_MASK), g_win_tpye_string[win_descp->win_type]);

    /* frame info proc */
    win_proc_basic_info(win_descp, p);
    /* win_buffer proc */
    win_buffer_proc_cnt_info(win_descp->win_component.h_buf, p);
    win_buffer_proc_node_info(win_descp->win_component.h_buf, p);
    win_sync_proc_cnt_info(&win_descp->avsync_info, p);
    win_vpss_proc_info(win_descp->win_component.h_vpss, p);
    win_frc_proc_info(win_descp->win_component.h_frc, p);

    return HI_SUCCESS;
}


static hi_s32 drv_win_proc_read(hi_void *p, hi_void *data)
{
    hi_handle hWin;
    hi_s32 ret;

    if (p == HI_NULL) {
        WIN_ERROR("p is null pointer!\n");
        return -EFAULT;
    }
    if (data == HI_NULL) {
        WIN_ERROR("data is null pointer!\n");
        return -EFAULT;
    }

    hWin = *(hi_handle *)data;

    ret = win_proc_info(hWin, p);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("Window read proc failed!\n");
    }

    return ret;
}

osal_proc_cmd g_win_proc_cmd_list[] = {
    { "help", win_proc_help },
    { "pause", win_proc_set_pause_mode},
    { "reset", win_proc_set_reset_mode},
    { "step", win_proc_set_step_mode },
    { "next", win_proc_next_frame_queue },
    { "vpss_bypass", win_proc_set_vpss_bypss },
    { "test_pattern", win_proc_set_test_pattern },
    { "out_rect", win_proc_set_out_rect },
    { "asp", win_proc_set_asp },
    { "sync", win_proc_set_sync },
    { "info", win_proc_get_play_info },
    { "quick", win_proc_set_quickout_mode},
    { "rota", win_proc_set_rotation},
    { "freeze", win_proc_set_freeze },
#ifdef CAPTURE_SUPPORT
    { "capture", win_proc_capture },
#endif
    { "order", win_proc_set_order},
    { "flip", win_proc_set_flip},
    { "tplay", win_proc_set_tplay},
    { "crop", win_proc_set_crop_rect },
    { "frm_mode", win_proc_set_frame_mode }

};

hi_s32 win_add_proc(hi_handle win_handle)
{
    hi_char proc_name[WIN_PROC_NAME_LENGTH];
    osal_proc_entry *proc_item = HI_NULL;
    win_descriptor *win_descp = HI_NULL;

    win_descp = winmanage_get_window(win_handle);
    if (win_descp == HI_NULL) {
        WIN_WARN("WIN is not exist! %#x\n", win_handle);
        return HI_FAILURE;
    }

    snprintf(proc_name, WIN_PROC_NAME_LENGTH, "win%04x", (hi_u32)(win_descp->win_index & WINDOW_INDEX_MASK));
    proc_name[WIN_PROC_NAME_LENGTH - 1] = '\0';

    proc_item = osal_proc_add(proc_name, strlen(proc_name));
    if (proc_item == HI_NULL) {
        WIN_ERROR("handle %x create proc %s failed\r\n", win_descp->win_index, proc_name);
        return HI_FAILURE;
    }

    proc_item->private = (hi_void *)&(win_descp->win_index);
    proc_item->read = drv_win_proc_read;
    proc_item->cmd_list = g_win_proc_cmd_list;
    proc_item->cmd_cnt = sizeof(g_win_proc_cmd_list) / sizeof(osal_proc_cmd);

    return HI_SUCCESS;
}

hi_void win_remove_proc(hi_handle win_handle)
{
    hi_char proc_name[WIN_PROC_NAME_LENGTH];
    win_descriptor *win_descp = HI_NULL;

    win_descp = winmanage_get_window(win_handle);
    if (win_descp == HI_NULL) {
        WIN_WARN("WIN is not exist! %#x\n", win_handle);
        return;
    }

    snprintf(proc_name, WIN_PROC_NAME_LENGTH, "win%04x", (hi_u32)(win_handle & WINDOW_INDEX_MASK));
    proc_name[WIN_PROC_NAME_LENGTH - 1] = '\0';

    osal_proc_remove(proc_name, strlen(proc_name));

    return;
}

