/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: vdp
* Create: 2019-04-12
 */

#include "hi_drv_pdm.h"

/* common headers */
#include "hi_drv_module.h"
#include "hi_drv_proc.h"
#include "hi_drv_sys.h"
#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "hi_drv_mem.h"
#include "hi_osal.h"

#include "hi_drv_disp.h"
#include "drv_disp.h"
#include "drv_disp_ioctl.h"
#include "drv_display.h"

#include "drv_disp_ext.h"
#include "drv_win_ext.h"
#include "drv_disp_isr.h"
#include "hal_disp_intf.h"
#include "drv_disp_timing.h"
#include "drv_hdmi_module_ext.h"
#include "drv_disp_interface.h"
#include "xdp_ctrl.h"
#include "linux/hisilicon/securec.h"
#include <linux/hisilicon/hi_license.h>
#include "hi_drv_panel.h"

#include "drv_window.h"
#ifdef HI_TEE_SUPPORT
#include "hi_drv_ssm.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    disp_channel disp[HI_DRV_DISPLAY_BUTT];
    hi_bool is_device_init;
} drv_disp_device;
hi_bool g_disp_suspend = HI_FALSE;
drv_disp_device g_disp_dev;

#define DISP_DEFAULT_COLOR_RED   0
#define DISP_DEFAULT_COLOR_GREEN 0
#define DISP_DEFAULT_COLOR_BLUE  0

#define DISP_DEFAULT_TIME_1000_MS 1000
#define DISP_REFRESH_UINT_100_HZ  100
#define DISP_MAX_VACT_TIME        50
#define DISP_MIN_VACT_TIME        50
#define DISP_VACT_CNT             2
#define DISP_ASPECT_RATION_W      16
#define DISP_ASPECT_RATION_H      9
#define DISP_DELAY_TIME_100_MS    100
#define DISP_INTERLACE_HEIGHT_COEFFICIENT  2
#define DISP_SCREEN_OFFSET_MAX    100
#define DISP_VIRTUAL_SCREEN_WIDTH_MIN   1280
#define DISP_VIRTUAL_SCREEN_WIDTH_MAX   3840
#define DISP_VIRTUAL_SCREEN_HEIGHT_MIN   720
#define DISP_VIRTUAL_SCREEN_HEIGHT_MAX   2160

#define GET_CHANNEL_BY_ID(id, ptr)                                             \
    do {                                                                       \
        if (id >= HI_DRV_DISPLAY_BUTT) {                                       \
            hi_err_disp("DISP ERROR! Invalid display in %s!\n", __FUNCTION__); \
            return HI_ERR_DISP_INVALID_PARA;                                   \
        }                                                                      \
        (ptr) = &g_disp_dev.disp[(id) - HI_DRV_DISPLAY_0];                     \
    } while(0)

#define GET_CHANNEL_DIRECTLY_BY_ID(id, ptr)                                    \
    do {                                                                       \
        (ptr) = &g_disp_dev.disp[(id) - HI_DRV_DISPLAY_0];                     \
    } while(0)

hi_bool drv_disp_is_opened(hi_drv_display disp)
{
    if ((g_disp_dev.is_device_init != HI_TRUE) ||
        (disp >= HI_DRV_DISPLAY_2)) {
        hi_err_disp("device is not init, disp(%d)!\n", disp);
        return HI_FALSE;
    }

    return g_disp_dev.disp[disp - HI_DRV_DISPLAY_0].open;
}

hi_drv_display *drv_disp_get_channel_id(hi_drv_display disp)
{
    return &g_disp_dev.disp[disp - HI_DRV_DISPLAY_0].disp;
}

hi_bool drv_disp_is_licensed(hi_drv_display disp)
{
    disp_channel *disp_chan = HI_NULL;
    if ((g_disp_dev.is_device_init != HI_TRUE) ||
        (disp >= HI_DRV_DISPLAY_2)) {
        hi_err_disp("device is not init, disp(%d)!\n", disp);
        return HI_FALSE;
    }

    GET_CHANNEL_DIRECTLY_BY_ID(disp, disp_chan);
    return disp_chan->setting.licensed;
}

hi_s32 drv_disp_attach_src(hi_drv_display master, hi_drv_display slave)
{
    return HI_SUCCESS;
}
hi_s32 drv_disp_detach_src(hi_drv_display master, hi_drv_display slave)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_update_intf_info(hi_drv_display disp,
                                 hi_drv_disp_fmt format,
                                 hi_drv_disp_stereo_mode stereo,
                                 hi_drv_disp_timing *custom_timing,
                                 hi_drv_disp_intf_info *intf_info)
{
    hi_s32 ret;
    disp_timing_info timing_info;
    disp_channel *disp_chan;
    GET_CHANNEL_DIRECTLY_BY_ID(disp, disp_chan);


    intf_info->out_type = disp_chan->disp_info.disp_type;

    intf_info->disp_timing.dynamic_timing.allm_en = disp_chan->setting.allm_en;
    intf_info->disp_timing.dynamic_timing.vrr_cfg = disp_chan->setting.vrr_cfg;
    intf_info->disp_timing.dynamic_timing.qms_enable = disp_chan->setting.qms_enable;
    intf_info->disp_timing.dynamic_timing.qft_factor = disp_chan->setting.factor;

    if (format == HI_DRV_DISP_FMT_CUSTOM) {
        intf_info->disp_timing.static_timing.timing = *custom_timing;
        intf_info->disp_timing.static_timing.vic_num = 0;
        intf_info->disp_timing.static_timing.interlace = HI_FALSE;
        intf_info->disp_timing.static_timing.pix_repeat = 1;
        intf_info->disp_timing.static_timing.disp_fmt = HI_DRV_DISP_FMT_CUSTOM;
        intf_info->disp_timing.static_timing.disp_3d_mode = HI_DRV_DISP_STEREO_NONE;

        intf_info->disp_timing.static_timing.aspect_ratio.aspect_ratio_w = DISP_ASPECT_RATION_W;
        intf_info->disp_timing.static_timing.aspect_ratio.aspect_ratio_h = DISP_ASPECT_RATION_H;
    } else {
        ret = drv_disp_timing_get_timing_info(format, &(intf_info->disp_timing.static_timing), &timing_info);
        if (ret != HI_SUCCESS) {
            hi_err_disp("get timing cfg is error, fmt is %d\n", format);
            return ret;
        }
    }

    if (((format >= HI_DRV_DISP_FMT_4096X2160_50) && (format <= HI_DRV_DISP_FMT_7680X4320_120)) ||
        ((format >= HI_DRV_DISP_FMT_3840X2160_50) && (format <= HI_DRV_DISP_FMT_3840X2160_120))) {
        intf_info->out_info.pixel_format = HI_DRV_DISP_PIXEL_YUV420;
    } else {
        intf_info->out_info.pixel_format = HI_DRV_DISP_PIXEL_YUV444;
    }

    intf_info->in_info.color_space = disp_chan->disp_info.color_space;
    intf_info->in_info.pixel_format = HI_DRV_DISP_PIXEL_YUV444;
    intf_info->in_info.data_width = HI_DRV_PIXEL_BITWIDTH_10BIT;

    intf_info->out_info.color_space = intf_info->in_info.color_space;
    intf_info->out_info.data_width = intf_info->in_info.data_width;

    return HI_SUCCESS;
}

hi_s32 disp_get_intf_attr(hi_drv_display disp,
                          hi_drv_disp_format_param *format_param,
                          hi_drv_disp_intf_info *intf_info)
{
    hi_s32 ret;
    disp_channel *disp_chan;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    format_param->format = disp_chan->setting.format;
    format_param->disp_3d_mode = disp_chan->setting.stereo;
    format_param->custom_timing = disp_chan->setting.custom_timing;

    ret = drv_disp_intf_get_attr(disp, format_param);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get intf attr is error\n");
        return ret;
    }

    ret = drv_disp_update_intf_info(disp,
                                    format_param->format,
                                    format_param->disp_3d_mode,
                                    &disp_chan->setting.custom_timing,
                                    intf_info);

    return ret;
}

hi_s32 disp_get_attached_intf(hi_drv_display disp,
                              hi_drv_disp_format_param *format_param)
{
    hi_s32 ret;
    hi_u32 number;
    disp_channel *disp_chan;
    disp_get_intf intf_state;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    ret = drv_disp_intf_get_status(disp, &intf_state);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get intf status is error\n");
        return ret;
    }

    for (number = 0; number < intf_state.intf_num; number++) {
        memcpy_s(&(format_param->intf_output[number].intf),
                 sizeof(hi_drv_disp_intf),
                 &(intf_state.intf[number]),
                 sizeof(hi_drv_disp_intf));
    }

    format_param->number = intf_state.intf_num;
    return HI_SUCCESS;
}

hi_s32 disp_get_channel_attr(hi_drv_display disp,
                             hi_drv_disp_format_param *format_param,
                             hi_drv_disp_intf_info *intf_info)
{
    hi_s32 ret;

    ret = disp_get_attached_intf(disp, format_param);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get attached intf is error\n");
        return ret;
    }

    ret = disp_get_intf_attr(disp, format_param, intf_info);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get intf attr is error\n");
    }

    return ret;
}

hi_s32 drv_disp_check_intf_validate(hi_drv_display disp, hi_drv_disp_format_param *format_param)
{
    hi_s32 ret;
    hi_drv_disp_intf_info intf_info;
    disp_channel *disp_chan;

    GET_CHANNEL_DIRECTLY_BY_ID(disp, disp_chan);

    ret = drv_disp_update_intf_info(disp, format_param->format, format_param->disp_3d_mode,
                                    &(disp_chan->setting.custom_timing), &intf_info);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get intf info error(%x)\n", ret);
        return ret;
    }

    ret = disp_get_attached_intf(disp, format_param);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get attached intf info error(%x)\n", ret);
        return ret;
    }

    if (format_param->number > 1) {
        hi_err_disp("disp%d attached intf num(%d) more than 1\n", format_param->number);
        return HI_ERR_DISP_INVALID_PARA;
    }

    ret = drv_disp_intf_check_output_validate(disp, &intf_info, format_param);
    if (ret != HI_SUCCESS) {
        hi_err_disp("check intf fmt error(%x)\n", ret);
    }

    return ret;
}

hi_s32 disp_get_cfg_timing(hi_drv_display disp,
                           hi_drv_disp_fmt format,
                           disp_timing_info *cfg_timing,
                           hi_drv_disp_timing *custom_timing)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_disp_static_timing static_timing;
    disp_channel *disp_chan;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    if (format == HI_DRV_DISP_FMT_CUSTOM) {
        cfg_timing->disp_fmt = HI_DRV_DISP_FMT_CUSTOM;
        cfg_timing->disp_3d_mode = HI_DRV_DISP_STEREO_NONE;
        cfg_timing->color_space = disp_chan->disp_info.color_space;
        cfg_timing->interlace = HI_FALSE;
        cfg_timing->refresh_rate = custom_timing->refresh_rate;
        cfg_timing->pixel_clk = custom_timing->pix_freq;
        cfg_timing->vic_num = 1;
        cfg_timing->aspect_ratio.aspect_ratio_w = DISP_ASPECT_RATION_W;
        cfg_timing->aspect_ratio.aspect_ratio_h = DISP_ASPECT_RATION_H;
        cfg_timing->sync_info.iop = 1;
        cfg_timing->sync_info.vact = custom_timing->vact;
        cfg_timing->sync_info.vbb = custom_timing->vbb;
        cfg_timing->sync_info.vfb = custom_timing->vfb;
        cfg_timing->sync_info.hact = custom_timing->hact;
        cfg_timing->sync_info.hbb = custom_timing->hbb;
        cfg_timing->sync_info.hfb = custom_timing->hfb;
        cfg_timing->sync_info.bvact = 1;
        cfg_timing->sync_info.bvbb = 1;
        cfg_timing->sync_info.bvfb = 1;
        cfg_timing->sync_info.hpw = custom_timing->hpw;
        cfg_timing->sync_info.vpw = custom_timing->vpw;
        cfg_timing->sync_info.hmid = 1;
        cfg_timing->sync_info.idv = custom_timing->idv;
        cfg_timing->sync_info.ihs = custom_timing->ihs;
        cfg_timing->sync_info.ivs = custom_timing->ivs;
    } else {
        ret = drv_disp_timing_get_timing_info(format, &static_timing, cfg_timing);
        if (ret != HI_SUCCESS) {
            hi_err_disp("get timing cfg is error, fmt is %d\n", format);
        }
    }

    return ret;
}

hi_s32 disp_set_output_timing(hi_drv_display disp,
                              hi_drv_disp_stereo_mode stereo,
                              hi_drv_disp_format_param *format_param,
                              hi_drv_disp_timing *custom_timing)
{
    hi_s32 ret;
    disp_timing_info static_timing;
    hi_drv_disp_intf_info intf_info;

    ret = drv_disp_update_intf_info(disp, format_param->format, format_param->disp_3d_mode,
                                    custom_timing, &intf_info);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get intf info error(%x)\n", ret);
        return ret;
    }

    ret = drv_disp_intf_check_output_validate(disp, &intf_info, format_param);
    if (ret != HI_SUCCESS) {
        hi_err_disp("check intf fmt error(%x)\n", ret);
        return ret;
    }

    ret = drv_disp_intf_output_prepare(disp, &intf_info, format_param);
    if (ret != HI_SUCCESS) {
        hi_err_disp("disp%d prepare intf is error\n", disp);
    }

    ret = drv_disp_set_enable(disp, HI_FALSE);
    if (ret != HI_SUCCESS) {
        hi_err_disp("set disp%d disable is error\n", disp);
    }

    ret = disp_get_cfg_timing(disp, format_param->format, &static_timing, custom_timing);
    if (ret != HI_SUCCESS) {
        hi_err_disp("disp_get_cfg_timing  error(%x),format (%d)\n", ret, format_param->format);
        return ret;
    }

    ret = drv_disp_timing_config(disp, &static_timing);
    if (ret != HI_SUCCESS) {
        hi_err_disp("set timing cfg is error\n");
        return ret;
    }

    drv_disp_set_enable(disp, HI_TRUE);
    if (ret != HI_SUCCESS) {
        hi_err_disp("set disp%d enable is error\n", disp);
    }

    ret = drv_disp_intf_output_config(disp, &intf_info, format_param);
    if (ret != HI_SUCCESS) {
        hi_err_disp("set intf config error(%x)\n", ret);
        return ret;
    }

    return ret;
}

hi_void disp_debug_bit_width_and_pixel_fmt(hi_drv_display disp, hi_drv_disp_format_param *format_param)
{
    hi_u32 number;
    disp_channel *disp_chan;
    GET_CHANNEL_DIRECTLY_BY_ID(disp, disp_chan);
    for (number = 0; number < format_param->number; number++) {
        if ((disp_chan->disp_debug.debug_pixel_format >= HI_DRV_DISP_PIXEL_RGB) &&
            (disp_chan->disp_debug.debug_pixel_format <= HI_DRV_DISP_PIXEL_YUV420)) {
            format_param->intf_output[number].pixel_fmt = disp_chan->disp_debug.debug_pixel_format;
        }

        if ((disp_chan->disp_debug.debug_data_width >= HI_DRV_PIXEL_BITWIDTH_8BIT) &&
            (disp_chan->disp_debug.debug_data_width <= HI_DRV_PIXEL_BITWIDTH_12BIT)) {
            format_param->intf_output[number].bit_width = disp_chan->disp_debug.debug_data_width;
        }
    }
}

hi_s32 disp_set_format(hi_drv_display disp, hi_drv_disp_stereo_mode stereo, hi_drv_disp_fmt format)
{
    hi_s32 ret;

    disp_channel *disp_chan;
    hi_drv_disp_format_param format_param;
    hi_drv_disp_intf_info intf_info;

    GET_CHANNEL_BY_ID(disp, disp_chan);
    if (disp_chan->out_connect_type == DISP_OUTPUT_PANEL_TYPE) {
        hi_err_disp("not support set format in panel type\n");
        return HI_SUCCESS;
    }

    ret = disp_get_channel_attr(disp, &format_param, &intf_info);
    if (ret != HI_SUCCESS) {
        hi_err_disp("update intf is error(%x)\n", ret);
        return ret;
    }

    format_param.disp_3d_mode = stereo;
    format_param.format = format;
    disp_debug_bit_width_and_pixel_fmt(disp, &format_param);
    ret = disp_set_output_timing(disp,
                                 stereo,
                                 &format_param,
                                 &(format_param.custom_timing));
    if (ret != HI_SUCCESS) {
        hi_err_disp(" set output timing is error!\n", ret);
        return ret;
    }

    disp_chan->setting.format = format;
    disp_chan->setting.stereo = stereo;

    if ((disp_chan->disp_debug.debug_pixel_format == HI_DRV_DISP_PIXEL_MAX) &&
        (disp_chan->disp_debug.debug_data_width == HI_DRV_PIXEL_BITWIDTH_MAX)) {
        ret = drv_disp_intf_set_attr(disp, &format_param);
        if (ret != HI_SUCCESS) {
            hi_err_disp("set intf attr is error\n");
        }
    }

    return ret;
}

hi_s32 drv_disp_set_format(hi_drv_display disp, hi_drv_disp_stereo_mode stereo, hi_drv_disp_fmt format)
{
    hi_s32 ret;
    disp_channel *disp_chan;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    if (format >= HI_DRV_DISP_FMT_BUTT) {
        hi_err_disp("Fmt is error (%d)!\n", format);
        return HI_ERR_DISP_NOT_SUPPORT_FMT;
    }

    if ((format == disp_chan->setting.format) && (stereo == disp_chan->setting.stereo)) {
        hi_warn_disp(" set the same format %d,stereo %d !\n", format, stereo);
        return HI_SUCCESS;
    }

    ret = disp_set_format(disp, stereo, format);

    return ret;
}

/* this is a dfx func use for set format in testcase */
hi_void drv_disp_dfx_force_format(hi_drv_display disp, hi_drv_disp_stereo_mode stereo, hi_drv_disp_fmt format)
{
    hi_s32 ret;
    disp_channel *disp_chan = HI_NULL;
    hi_drv_disp_format_param format_param;
    hi_drv_disp_intf_info intf_info;
    disp_timing_info static_timing;
    hi_drv_disp_timing custom_timing;

    GET_CHANNEL_DIRECTLY_BY_ID(disp, disp_chan);

    (hi_void)disp_get_channel_attr(disp, &format_param, &intf_info);
    format_param.disp_3d_mode = stereo;
    format_param.format = format;

    disp_debug_bit_width_and_pixel_fmt(disp, &format_param);
    ret = drv_disp_set_enable(disp, HI_FALSE);
    if (ret != HI_SUCCESS) {
        hi_warn_disp("dfx format warning set disable\n");
    }
    ret = disp_get_cfg_timing(disp, format_param.format, &static_timing, &custom_timing);
    if (ret != HI_SUCCESS) {
        hi_warn_disp("dfx format warning get timing\n");
    }
    ret = drv_disp_timing_config(disp, &static_timing);
    if (ret != HI_SUCCESS) {
        hi_warn_disp("dfx format warning set timing\n");
    }
    ret = drv_disp_set_enable(disp, HI_TRUE);
    if (ret != HI_SUCCESS) {
        hi_warn_disp("dfx format warning set enable\n");
    }

    disp_chan->setting.format = format;
    disp_chan->setting.stereo = stereo;
    if ((disp_chan->disp_debug.debug_pixel_format == HI_DRV_DISP_PIXEL_MAX) &&
        (disp_chan->disp_debug.debug_data_width == HI_DRV_PIXEL_BITWIDTH_MAX)) {
        ret = drv_disp_intf_set_attr(disp, &format_param);
        if (ret != HI_SUCCESS) {
            hi_warn_disp("dfx format warning set intf attr\n");
        }
    }
    return;
}

hi_s32 drv_disp_set_bit_width(hi_drv_display disp, hi_drv_pixel_bitwidth data_width)
{
    hi_s32 ret;
    disp_channel *disp_chan;
    hi_drv_disp_format_param format_param;
    hi_drv_disp_intf_info intf_info;

    GET_CHANNEL_BY_ID(disp, disp_chan);
    ret = disp_get_channel_attr(disp, &format_param, &intf_info);
    if (ret != HI_SUCCESS) {
        hi_err_disp("update intf is error(%x)\n", ret);
        return ret;
    }

    disp_chan->disp_debug.debug_data_width = data_width;
    disp_debug_bit_width_and_pixel_fmt(disp, &format_param);

    ret = disp_set_output_timing(disp,
                                 disp_chan->setting.stereo,
                                 &format_param,
                                 &(disp_chan->setting.custom_timing));
    if (ret != HI_SUCCESS) {
        hi_err_disp(" set output timing is error!\n", ret);
        return ret;
    }

    return ret;
}

hi_s32 drv_disp_set_pixel_format(hi_drv_display disp, hi_drv_disp_pix_format pixel_format)
{
    hi_s32 ret;
    disp_channel *disp_chan;
    hi_drv_disp_format_param format_param;
    hi_drv_disp_intf_info intf_info;

    GET_CHANNEL_BY_ID(disp, disp_chan);
    ret = disp_get_channel_attr(disp, &format_param, &intf_info);
    if (ret != HI_SUCCESS) {
        hi_err_disp("update intf is error(%x)\n", ret);
        return ret;
    }

    disp_chan->disp_debug.debug_pixel_format = pixel_format;
    disp_debug_bit_width_and_pixel_fmt(disp, &format_param);

    ret = disp_set_output_timing(disp,
                                 disp_chan->setting.stereo,
                                 &format_param,
                                 &(disp_chan->setting.custom_timing));
    if (ret != HI_SUCCESS) {
        hi_err_disp(" set output timing is error!\n", ret);
        return ret;
    }

    return ret;
}

hi_s32 drv_disp_get_format(hi_drv_display disp, hi_drv_disp_stereo_mode *stereo, hi_drv_disp_fmt *fmt)
{
    hi_s32 ret = HI_SUCCESS;
    disp_channel *disp_chan;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    *fmt = disp_chan->setting.format;
    *stereo = disp_chan->setting.stereo;

    return ret;
}

hi_bool disp_is_intf_param_valid(hi_drv_display disp, hi_drv_disp_format_param *format_param)
{
    hi_bool intf_existed_status;

    if (format_param->number == 0) {
        hi_err_disp("input intf number is zero\n");
        return HI_FALSE;
    }

    intf_existed_status = drv_disp_intf_check_existed_status(disp, format_param);
    if (intf_existed_status != HI_TRUE) {
        return HI_FALSE;
    }

    return HI_TRUE;
}

hi_bool disp_is_intf_same_param(hi_drv_display disp,
                                hi_drv_disp_stereo_mode disp_stereo_mode,
                                hi_drv_disp_format_param *format_param)
{
    hi_bool is_same_param;
    disp_channel *disp_chan;

    GET_CHANNEL_DIRECTLY_BY_ID(disp, disp_chan);

    if ((disp_chan->setting.stereo == disp_stereo_mode) &&
        (disp_chan->setting.format == format_param->format)) {
        is_same_param = drv_disp_intf_check_same_param(disp, format_param);
        if (is_same_param == HI_TRUE) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

hi_s32 drv_disp_set_custom_timing(hi_drv_display disp, hi_drv_disp_format_param *format_param)
{
    disp_channel *disp_chan;

    GET_CHANNEL_BY_ID(disp, disp_chan);
    DISP_CHECK_NULL_POINTER(format_param);

    disp_chan->setting.custom_timing = format_param->custom_timing;

    return HI_SUCCESS;
}

hi_s32 drv_disp_set_timing(hi_drv_display disp, hi_drv_disp_format_param *format_param)
{
    hi_s32 ret;
    hi_bool check_status;
    disp_channel *disp_chan;

    GET_CHANNEL_BY_ID(disp, disp_chan);
    DISP_CHECK_NULL_POINTER(format_param);

    if (disp_chan->out_connect_type == DISP_OUTPUT_PANEL_TYPE) {
        hi_err_disp("not support set timing in panel type\n");
        return HI_SUCCESS;
    }

    check_status = disp_is_intf_param_valid(disp, format_param);
    if (check_status != HI_TRUE) {
        hi_err_disp(" intf param is invalid!\n");
        return HI_ERR_DISP_INVALID_PARA;
    }
    check_status = disp_is_intf_same_param(disp, format_param->disp_3d_mode, format_param);
    if (check_status == HI_TRUE) {
        hi_warn_disp(" intf param is same!\n");
        return HI_SUCCESS;
    }

    disp_debug_bit_width_and_pixel_fmt(disp, format_param);
    ret = disp_set_output_timing(disp,
                                 format_param->disp_3d_mode,
                                 format_param,
                                 &(disp_chan->setting.custom_timing));
    if (ret != HI_SUCCESS) {
        hi_err_disp(" set output timing is error!\n", ret);
        return ret;
    }

    disp_chan->setting.format = format_param->format;
    disp_chan->setting.stereo = format_param->disp_3d_mode;

    if ((disp_chan->disp_debug.debug_pixel_format == HI_DRV_DISP_PIXEL_MAX) &&
        (disp_chan->disp_debug.debug_data_width == HI_DRV_PIXEL_BITWIDTH_MAX)) {
        ret = drv_disp_intf_set_attr(disp, format_param);
        if (ret != HI_SUCCESS) {
            hi_err_disp("set intf attr is error\n");
        }
    }

    return ret;
}

hi_s32 drv_disp_get_timing(hi_drv_display disp, hi_drv_disp_format_param *format_param)
{
    hi_s32 ret;
    hi_u32 number;
    disp_get_intf intf_state;
    disp_channel *disp_chan;

    GET_CHANNEL_BY_ID(disp, disp_chan);
    DISP_CHECK_NULL_POINTER(format_param);
    if (disp_chan->out_connect_type == DISP_OUTPUT_PANEL_TYPE) {
        hi_err_disp("not support get timing in panel type\n");
        return HI_SUCCESS;
    }

    ret = drv_disp_intf_get_status(disp, &intf_state);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    for (number = 0; number < intf_state.intf_num; number++) {
        memcpy_s(&(format_param->intf_output[number].intf),
                 sizeof(hi_drv_disp_intf),
                 &(intf_state.intf[number]),
                 sizeof(hi_drv_disp_intf));
    }

    format_param->number = intf_state.intf_num;
    format_param->disp_3d_mode = disp_chan->setting.stereo;
    format_param->format = disp_chan->setting.format;

    ret = drv_disp_intf_get_attr(disp, format_param);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get intf attr is error\n");
        return ret;
    }

    format_param->custom_timing = disp_chan->setting.custom_timing;

    return HI_SUCCESS;
}

hi_s32 drv_disp_get_fmt_timing(hi_drv_disp_fmt fmt, hi_drv_disp_timing *timing)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_add_intf(hi_drv_display disp, hi_drv_disp_intf intf)
{
    hi_s32 ret;
    hi_bool is_support;

    DISP_CHECK_ID(disp);
    ret = drv_disp_intf_get_capability(disp, intf, &is_support);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get intf capability is error\n");
        return ret;
    }

    if (is_support != HI_TRUE) {
        hi_err_disp("no right to bind the intf(%d)\n", intf.intf_type);
        return HI_FAILURE;
    }

    ret = drv_disp_intf_attach(disp, intf);
    if (ret != HI_SUCCESS) {
        hi_err_disp("disp%d add intf(%d) is error\n", disp, intf.intf_type);
    }

    return ret;
}

hi_s32 drv_disp_del_intf(hi_drv_display disp, hi_drv_disp_intf intf)
{
    hi_s32 ret = 0;

    ret = drv_disp_intf_detach(disp, intf);
    if (ret != HI_SUCCESS) {
        hi_err_disp("disp%d del intf(%d) is error\n", disp, intf.intf_type);
    }

    return ret;
}

hi_s32 drv_disp_get_intf_status(hi_drv_display disp, disp_get_intf *intf_state)
{
    hi_s32 ret = HI_SUCCESS;

    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(intf_state);

    ret = drv_disp_intf_get_status(disp, intf_state);
    if (ret != HI_SUCCESS) {
        hi_err_disp("disp%d get intf status is error\n", disp);
        return ret;
    }

    return HI_SUCCESS;
}

hi_void disp_get_intf_param_info(hi_drv_display disp,
                                 hi_drv_disp_format_param *format_param,
                                 hi_drv_disp_intf_info *intf_info,
                                 hi_drv_disp_intf *drv_intf,
                                 hi_bool enable)
{
    hi_s32 ret;

    format_param->intf_output[0].intf = *drv_intf;
    format_param->intf_output[0].enable = enable;
    format_param->number = 1;

    ret = disp_get_intf_attr(disp, format_param, intf_info);
    if (ret != HI_SUCCESS) {
        hi_err_disp("update intf is error(%x)\n", ret);
    }

    return;
}

hi_s32 drv_disp_set_intf_enable(hi_drv_display disp, hi_drv_disp_intf drv_intf, hi_bool enable)
{
    hi_s32 ret;
    disp_channel *disp_chan;
    hi_bool is_enable;
    hi_drv_disp_intf_info intf_info;
    hi_drv_disp_format_param format_param;

    DISP_CHECK_ID(disp);
    GET_CHANNEL_BY_ID(disp, disp_chan);

    if (disp_chan->setting.enable == HI_FALSE && enable == HI_TRUE) {
        hi_err_disp("disp%d channel is off, intf(%d) cannot set the enable\n", disp, drv_intf.intf_type);
        return HI_ERR_DISP_INVALID_PARA;
    }

    ret = drv_disp_get_intf_enable(disp, drv_intf, &is_enable);
    if (enable == is_enable) {
        hi_warn_disp("set same intf enable status(%d)\n", is_enable);
        return HI_SUCCESS;
    }

    disp_get_intf_param_info(disp, &format_param, &intf_info, &drv_intf, enable);

    if (enable == HI_TRUE) {
        ret = drv_disp_intf_check_output_validate(disp, &intf_info, &format_param);
        if (ret != HI_SUCCESS) {
            hi_err_disp("intf check is error, ret = %x\n", ret);
            return ret;
        }

        ret = drv_disp_intf_set_enable_status(disp, &format_param);
        if (ret != HI_SUCCESS) {
            hi_err_disp("disp%d set enable status is error\n", disp);
        }

        ret = drv_disp_intf_output_prepare(disp, &intf_info, &format_param);
        if (ret != HI_SUCCESS) {
            hi_err_disp("disp%d prepare intf(%d) is error\n", disp, drv_intf.intf_type);
        }

        ret = drv_disp_intf_output_config(disp, &intf_info, &format_param);
        if (ret != HI_SUCCESS) {
            hi_err_disp("disp%d config intf(%d) is error\n", disp, drv_intf.intf_type);
        }

        ret = drv_disp_intf_output_enable(disp, &format_param, enable);
        if (ret != HI_SUCCESS) {
            hi_err_disp("disp%d prepare intf(%d) is error\n", disp, drv_intf.intf_type);
        }

    } else {
        ret = drv_disp_intf_output_enable(disp, &format_param, enable);
        if (ret != HI_SUCCESS) {
            hi_err_disp("disp%d prepare intf(%d) is error\n", disp, drv_intf.intf_type);
        }
    }

    return ret;
}

hi_s32 drv_disp_get_intf_enable(hi_drv_display disp, hi_drv_disp_intf drv_intf, hi_bool *enable)
{
    hi_s32 ret;
    DISP_CHECK_ID(disp);
    ret = drv_disp_intf_get_enable(disp, drv_intf, enable);
    if (ret != HI_SUCCESS) {
        hi_err_disp("disp%d get intf status is error\n", disp);
    }

    return ret;
}

hi_s32 drv_disp_set_right_eye_first(hi_drv_display disp, hi_bool enable)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_set_virt_screen(hi_drv_display disp, hi_rect *virt_screen)
{
    hi_s32 ret = HI_SUCCESS;
    disp_channel *disp_chan;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    if ((virt_screen->x != 0) ||
        (virt_screen->y != 0) ||
        (virt_screen->height < DISP_VIRTUAL_SCREEN_HEIGHT_MIN) ||
        (virt_screen->height > DISP_VIRTUAL_SCREEN_HEIGHT_MAX) ||
        (virt_screen->width < DISP_VIRTUAL_SCREEN_WIDTH_MIN) ||
        (virt_screen->width > DISP_VIRTUAL_SCREEN_WIDTH_MAX)) {
        hi_err_disp("virtual height(%d) width(%d) x(%d) y(%d) param is invalid.\n",
                    virt_screen->height,
                    virt_screen->width,
                    virt_screen->x,
                    virt_screen->y);
        return HI_ERR_DISP_INVALID_PARA;
    }

    disp_chan->setting.virtaul_screen = *virt_screen;

    return ret;
}

hi_s32 drv_disp_get_virt_screen(hi_drv_display disp, hi_rect *virt_screen)
{
    hi_s32 ret = HI_SUCCESS;
    disp_channel *disp_chan;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    *virt_screen = disp_chan->setting.virtaul_screen;

    return ret;
}

hi_s32 drv_disp_set_screen_offset(hi_drv_display disp, hi_drv_disp_offset *screen_offset)
{
    hi_s32 ret = HI_SUCCESS;
    disp_channel *disp_chan;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    if ((screen_offset->bottom > DISP_SCREEN_OFFSET_MAX) ||
        (screen_offset->top > DISP_SCREEN_OFFSET_MAX) ||
        (screen_offset->left > DISP_SCREEN_OFFSET_MAX) ||
        (screen_offset->right > DISP_SCREEN_OFFSET_MAX)) {
        hi_err_disp("offset bottom(%d) top(%d) left(%d) right(%d) param is invalid.\n",
                    screen_offset->bottom,
                    screen_offset->top,
                    screen_offset->left,
                    screen_offset->right);
        return HI_ERR_DISP_INVALID_PARA;
    }

    disp_chan->setting.offset_info = *screen_offset;

    return ret;
}

hi_s32 drv_disp_get_screen_offset(hi_drv_display disp, hi_drv_disp_offset *screen_offset)
{
    hi_s32 ret = HI_SUCCESS;
    disp_channel *disp_chan;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    *screen_offset = disp_chan->setting.offset_info;

    return ret;
}

hi_s32 drv_disp_get_hdmi_intf_info(hi_drv_display disp, hi_drv_disp_intf_info *intf_info)
{
    hi_s32 ret;
    hi_drv_disp_format_param format_param;

    format_param.number = 1;
    format_param.intf_output[0].intf.intf_type = HI_DRV_DISP_INTF_TYPE_HDMI;
    format_param.intf_output[0].intf.un_intf.hdmi = HI_DRV_HDMI_ID_0;

    ret = disp_get_intf_attr(disp, &format_param, intf_info);
    if (ret != HI_SUCCESS) {
        hi_err_disp("disp%d get intf attr is error\n", disp);
        return ret;
    }

    intf_info->out_info.data_width = format_param.intf_output[0].bit_width;
    intf_info->out_info.pixel_format = format_param.intf_output[0].pixel_fmt;

    return HI_SUCCESS;
}

hi_s32 drv_disp_set_bgcolor(hi_drv_display disp, hi_drv_disp_color *bg_color)
{
    hi_s32 ret = HI_SUCCESS;
    disp_channel *disp_chan;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    disp_chan->setting.bg_color = *bg_color;

    ret = hal_disp_set_bgcolor(disp, disp_chan->disp_info.color_space, &disp_chan->setting.bg_color);
    if (ret != HI_SUCCESS) {
        hi_err_disp("set timing cfg is error\n");
    }

    return ret;
}

hi_s32 drv_disp_get_bgcolor(hi_drv_display disp, hi_drv_disp_color *bg_color)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);

    *bg_color = disp_chan->setting.bg_color;

    return HI_SUCCESS;
}

hi_s32 drv_disp_set_color_bar(hi_drv_display disp, hi_bool enable)
{
    hal_disp_debug_colorbar(disp, enable);
    return HI_SUCCESS;
}

hi_s32 disp_proc_get_intf_type(disp_intf *intf_attr,
                               hi_drv_disp_intf *intf)
{
    hi_s32 ret = HI_SUCCESS;
    switch(intf_attr->id) {
        case HI_DRV_DISP_INTF_CVBS0:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_CVBS;
            intf->un_intf.cvbs.dac_cvbs = 0;
            break;
        case HI_DRV_DISP_INTF_HDMI0:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_HDMI;
            intf->un_intf.hdmi = HI_DRV_HDMI_ID_0;
            break;
        case HI_DRV_DISP_INTF_HDMI1:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_HDMI;
            intf->un_intf.hdmi = HI_DRV_HDMI_ID_1;
            break;
        case HI_DRV_DISP_INTF_MIPI0:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_MIPI;
            intf->un_intf.mipi.mipi_id = HI_DRV_MIPI_ID_0;
            intf->un_intf.mipi.mipi_mode = intf_attr->mipi_mode;
            break;
        case HI_DRV_DISP_INTF_MIPI1:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_MIPI;
            intf->un_intf.mipi.mipi_id = HI_DRV_MIPI_ID_1;
            intf->un_intf.mipi.mipi_mode = intf_attr->mipi_mode;
            break;
        default:
            ret = HI_FAILURE;
            hi_err_disp("intf(%d) type is error.\n", intf_attr->id);
            break;
    }

    return ret;
}

hi_s32 drv_disp_proc_set_intf_enable(hi_drv_display disp,
                                     disp_intf *intf_attr,
                                     hi_bool enable)
{
    hi_s32 ret;
    hi_drv_disp_intf intf;

    ret = disp_proc_get_intf_type(intf_attr, &intf);
    if (ret != HI_SUCCESS) {
        hi_err_disp("disp_proc_get_intf_type is error.\n");
        return ret;
    }

    if (enable == HI_TRUE) {
        ret = drv_disp_add_intf(disp, intf);
        if (ret != HI_SUCCESS) {
            hi_err_disp("attach mipi1 is error\n");
            return ret;
        }

        ret = drv_disp_set_intf_enable(disp, intf, HI_TRUE);
        if (ret != HI_SUCCESS) {
            hi_err_disp("set mipi1 enable is error\n");
        }
    } else {
        ret = drv_disp_set_intf_enable(disp, intf, HI_FALSE);
        if (ret != HI_SUCCESS) {
            hi_err_disp("set mipi1 enable is error\n");
            return ret;
        }

        ret = drv_disp_del_intf(disp, intf);
        if (ret != HI_SUCCESS) {
            hi_err_disp("detach mipi0 is error\n");
        }
    }

    return ret;
}

// set aspect ratio: 0 and 0 means auto
hi_s32 drv_disp_set_aspect_ratio(hi_drv_display disp,
                                 hi_drv_disp_aspect_mode drv_aspect_mode,
                                 hi_u32 ratio_h,
                                 hi_u32 ratio_v)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);

    if (drv_aspect_mode >= HI_DRV_DISP_ASPECT_RATIO_MODE_MAX) {
        hi_err_disp("para aspect ratio mode is invalid, (%d).\n", drv_aspect_mode);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (drv_aspect_mode == HI_DRV_DISP_ASPECT_RATIO_CUSTOM) {
        if ((ratio_h == 0) || (ratio_v == 0)) {
            hi_err_disp("para aspect ratio is invalid, h,v (%d,%d).\n", ratio_h, ratio_v);
            return HI_ERR_DISP_INVALID_PARA;
        }
        if ((ratio_h > (ratio_v * HI_DRV_DISP_ASPECT_RATIO_MAX))
            || (ratio_v > (ratio_h * HI_DRV_DISP_ASPECT_RATIO_MAX))) {
            hi_err_disp("para aspect ratio is invalid,h/v or v/h  out of 16 (%d,%d).\n", ratio_h, ratio_v);
            return HI_ERR_DISP_INVALID_PARA;
        }
    }

    if (drv_aspect_mode == HI_DRV_DISP_ASPECT_RATIO_4TO3) {
        disp_chan->setting.aspect_ratio.aspect_ratio_w = 4; /* 4 is asp value */
        disp_chan->setting.aspect_ratio.aspect_ratio_h = 3; /* 3 is asp value */
    } else if (drv_aspect_mode == HI_DRV_DISP_ASPECT_RATIO_16TO9) {
        disp_chan->setting.aspect_ratio.aspect_ratio_w = 16; /* 16 is asp value */
        disp_chan->setting.aspect_ratio.aspect_ratio_h = 9; /* 9 is asp value */
    } else if (drv_aspect_mode == HI_DRV_DISP_ASPECT_RATIO_221TO100) {
        disp_chan->setting.aspect_ratio.aspect_ratio_w = 221; /* 221 is asp value */
        disp_chan->setting.aspect_ratio.aspect_ratio_h = 100; /* 100 is asp value */
    } else if (drv_aspect_mode == HI_DRV_DISP_ASPECT_RATIO_CUSTOM) {
        disp_chan->setting.aspect_ratio.aspect_ratio_w = ratio_h;
        disp_chan->setting.aspect_ratio.aspect_ratio_h = ratio_v;
    } else {
        disp_chan->setting.aspect_ratio.aspect_ratio_w = 0;
        disp_chan->setting.aspect_ratio.aspect_ratio_h = 0;
    }

    disp_chan->setting.aspect_ratio.aspect_mode = drv_aspect_mode;
    return HI_SUCCESS;
}

hi_s32 drv_disp_get_aspect_ratio(hi_drv_display disp,
                                 hi_drv_disp_aspect_mode *drv_aspect_mode,
                                 hi_u32 *ratio_h,
                                 hi_u32 *ratio_v)
{
    hi_s32 ret = HI_SUCCESS;
    disp_channel *disp_chan;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    *ratio_h = disp_chan->setting.aspect_ratio.aspect_ratio_w;
    *ratio_v = disp_chan->setting.aspect_ratio.aspect_ratio_h;
    *drv_aspect_mode = disp_chan->setting.aspect_ratio.aspect_mode;

    return ret;
}

hi_s32 drv_disp_set_layer_zorder(hi_drv_display disp, hi_drv_disp_zorder layer, hi_drv_disp_zorder z_flag)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_get_layer_zorder(hi_drv_display disp, hi_drv_disp_zorder layer, hi_u32 *zorder);

// miracast
hi_s32 drv_disp_create_cast(hi_drv_display disp, hi_drv_disp_cast_cfg *cfg, hi_handle *hcast)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_destroy_cast(hi_handle hcast)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_set_cast_enable(hi_handle hcast, hi_bool enable)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_get_cast_enable(hi_handle hcast, hi_bool *enable)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_acquire_cast_frame(hi_handle hcast, hi_drv_video_frame *cast_frame)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_release_cast_frame(hi_handle hcast, hi_drv_video_frame *cast_frame)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_external_attach(hi_handle hcast, hi_handle hsink)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_external_de_attach(hi_handle hcast, hi_handle hsink)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_set_cast_attr(hi_handle hcast, hi_drv_disp_cast_attr *cast_attr)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_get_cast_attr(hi_handle hcast, hi_drv_disp_cast_attr *cast_attr)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_get_cast_handle(hi_drv_display disp, hi_handle *phcast, hi_void **phcast_ptr)
{
    return HI_SUCCESS;
}

// snapshot
hi_s32 drv_disp_acquire_snapshot(hi_drv_display disp, hi_drv_video_frame *snap_shot_frame,
                                 hi_void **snapshot_handle_out)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_release_snapshot(hi_drv_display disp, hi_drv_video_frame *snap_shot_frame,
                                 hi_void *snapshot_handle)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_destroy_snapshot(hi_void *h_snapshot)
{
    return HI_SUCCESS;
}

// Macrovision
hi_s32 drv_disp_set_macrovision(hi_drv_display disp, hi_drv_disp_macrovision mode)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_get_macrovision(hi_drv_display disp, hi_drv_disp_macrovision *mode)
{
    return HI_SUCCESS;
}

// cgms-a
hi_s32 drv_disp_set_cgms(hi_drv_display disp, hi_drv_disp_cgmsa_cfg *cfg)
{
    return HI_SUCCESS;
}

// vbi
hi_s32 drv_disp_create_vbichannel(hi_drv_display disp, hi_drv_disp_vbi_cfg *cfg, hi_handle *hvbi)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_destroy_vbichannel(hi_handle hvbi)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_send_vbi_data(hi_handle hvbi, hi_drv_disp_vbi_data *vbi_data)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_set_wss(hi_handle hvbi, hi_drv_disp_wss_data *wss_data)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_set_techni_bright(hi_drv_display disp, hi_s32 techni_drv_disp_bright)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_get_techni_bright(hi_drv_display disp, hi_s32 *techni_drv_disp_bright)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_set_output_color_space(hi_drv_display disp, hi_drv_disp_output_color_space color_space)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);

    if ((disp != HI_DRV_DISPLAY_0) && (color_space == HI_DRV_DISP_COLOR_SPACE_BT2020)) {
        hi_err_disp("color space bt2020 is only supported in disp0\n");
        return HI_FAILURE;
    }

    disp_chan->setting.color_space_mode = color_space;

    return HI_SUCCESS;
}

hi_s32 drv_disp_get_output_color_space(hi_drv_display disp, hi_drv_disp_output_color_space *color_space)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);

    *color_space = disp_chan->setting.color_space_mode;
    return HI_SUCCESS;
}

hi_s32 drv_disp_set_xdr_engine_type(hi_drv_display disp, hi_drv_disp_xdr_engine xdr_engine)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_get_xdr_engine_type(hi_drv_display disp, hi_drv_disp_xdr_engine *xdr_engine)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_get_alpha(hi_drv_display disp, hi_u32 *alpha)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);
    *alpha = disp_chan->setting.alpha;

    return HI_SUCCESS;
}

hi_s32 drv_disp_set_alpha(hi_drv_display disp, hi_u32 alpha)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);

    if ((alpha > HI_DRV_DISP_ALPHA_MAX) || (alpha < HI_DRV_DISP_ALPHA_MIN)) {
        hi_err_disp("disp%d set alpha %d is out of range(0~100)", disp, alpha);
        return HI_ERR_DISP_INVALID_PARA;
    }

    disp_chan->setting.alpha = alpha;

    return HI_SUCCESS;
}

hi_s32 drv_disp_set_sink_capability(hi_drv_display disp, hi_drv_disp_sink_capability *sink_cap)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);

    disp_chan->setting.sink_capability = *sink_cap;
    return HI_SUCCESS;
}

hi_s32 drv_disp_set_match_content_mode(hi_drv_display disp, hi_drv_disp_match_content_mode match_content_mode)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);
    disp_chan->setting.match_mode = match_content_mode;

    return HI_SUCCESS;
}

hi_s32 drv_disp_get_match_content_mode(hi_drv_display disp, hi_drv_disp_match_content_mode *match_content_mode)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);
    *match_content_mode = disp_chan->setting.match_mode;

    return HI_SUCCESS;
}

hi_s32 drv_disp_get_sink_capability(hi_drv_display disp, hi_drv_disp_sink_capability *sink_cap)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);

    *sink_cap = disp_chan->setting.sink_capability ;

    return HI_SUCCESS;
}

hi_s32 drv_disp_get_capability(hi_drv_display disp, hi_drv_disp_capability *disp_cap)
{
    disp_channel *disp_chan = HI_NULL;
    GET_CHANNEL_BY_ID(disp, disp_chan);

    disp_cap->hdr_cap.sdr = HI_TRUE;
    disp_cap->hdr_cap.hdr10_plus = HI_FALSE;
    disp_cap->hdr_cap.hlg = HI_FALSE;
    disp_cap->hdr_cap.dolby_vision = HI_FALSE;
    disp_cap->hdr_cap.dolby_low_latency = HI_FALSE;

    if (disp == HI_DRV_DISPLAY_1) {
        disp_cap->timing_cap = HI_DRV_DISP_CAP_TIMING_4K_60HZ;
        disp_cap->hdr_cap.hdr10 = HI_FALSE;
    } else if (disp == HI_DRV_DISPLAY_0) {
        disp_cap->timing_cap = HI_DRV_DISP_CAP_TIMING_8K_120HZ;
        disp_cap->hdr_cap.hdr10 = HI_TRUE;
    }

    disp_cap->support =  disp_chan->setting.licensed;

    return HI_SUCCESS;
}

hi_s32 drv_disp_get_output_status(hi_drv_display disp, hi_drv_disp_output_status *drv_disp_output_status)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);

    drv_disp_output_status->actual_output_colorspace = disp_chan->disp_info.color_space_mode;
    drv_disp_output_status->actual_output_type = disp_chan->disp_info.disp_type;
    return HI_SUCCESS;
}

hi_s32 drv_disp_get_fmt_from_vic(hi_u32 vic, hi_drv_disp_fmt *fmt)
{
    hi_s32 ret;
    ret = drv_disp_timing_get_fmt_from_vic(vic, fmt);
    if (ret != HI_SUCCESS) {
        hi_warn_disp("get fmt(%d) from vic(%d) is error", *fmt, vic);
    }

    return ret;
}

hi_s32 drv_disp_set_vrr(hi_drv_display disp, hi_drv_disp_vrr *disp_vrr)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);
    disp_chan->setting.vrr_cfg = *disp_vrr;

    return HI_SUCCESS;
}

hi_s32 drv_disp_get_vrr(hi_drv_display disp, hi_drv_disp_vrr *disp_vrr)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);
    *disp_vrr = disp_chan->setting.vrr_cfg;

    return HI_SUCCESS;
}

hi_s32 drv_disp_set_qms(hi_drv_display disp, hi_bool enable)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);
    disp_chan->setting.qms_enable = enable;

    return HI_SUCCESS;
}

hi_s32 drv_disp_get_qms(hi_drv_display disp, hi_bool *enable)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);
    *enable = disp_chan->setting.qms_enable;

    return HI_SUCCESS;
}

hi_s32 drv_disp_set_allm(hi_drv_display disp, hi_bool enable)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);
    disp_chan->setting.allm_en = enable;

    return HI_SUCCESS;
}

hi_s32 drv_disp_get_allm(hi_drv_display disp, hi_bool *enable)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);
    *enable = disp_chan->setting.allm_en;

    return HI_SUCCESS;
}

hi_s32 drv_disp_set_qft(hi_drv_display disp, hi_u32 factor)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);
    disp_chan->setting.factor = factor;

    return HI_SUCCESS;
}

hi_s32 drv_disp_get_qft(hi_drv_display disp, hi_u32 *factor)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);
    *factor = disp_chan->setting.factor;

    return HI_SUCCESS;
}

hi_s32 drv_disp_set_stop_hdmiinfo(hi_drv_display disp, hi_bool stop_send_hdmi_info)
{
    return HI_SUCCESS;
}
/*****************************************************/
// internal state
hi_s32 drv_disp_get_init_flag(hi_bool *inited)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_get_display_info(hi_drv_display disp, hi_disp_display_info *info)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);

    *info = disp_chan->disp_info;

    return HI_SUCCESS;
}

hi_s32 drv_disp_update_display_info(hi_drv_display disp,
                                    hi_drv_disp_out_type drv_disp_type,
                                    hi_drv_disp_output_color_space color_space_mode)
{
    disp_channel *disp_chan = HI_NULL;
    hi_s32 ret;

    GET_CHANNEL_BY_ID(disp, disp_chan);
    drv_disp_produce_display_info(&disp_chan->setting, &disp_chan->disp_info,
                                  drv_disp_type, color_space_mode);

    ret = disp_isr_set_disp_info(disp_chan->disp, &disp_chan->disp_info);

    return ret;
}

hi_s32 drv_disp_get_display_setting(hi_drv_display disp, disp_setting *setting)
{
    disp_channel *disp_chan = HI_NULL;
    GET_CHANNEL_BY_ID(disp, disp_chan);

    *setting = disp_chan->setting;

    return HI_SUCCESS;
}

#define DISP_CIRCLE_TIME_MAX_MS 50
#define DISP_LEFT_TIME_MAX_MS   50

hi_s32 drv_disp_get_current_timing_status(hi_drv_display disp, hi_drv_disp_timing_status *current_timing_status)
{
    hi_u32 total_line = 0;
    hi_drv_disp_static_timing timing_cfg;
    disp_channel *disp_chan;
    hi_u32 fmt_refresh_rate = 0;
    disp_timing_info timing_info;

    DISP_CHECK_NULL_POINTER(current_timing_status);
    GET_CHANNEL_BY_ID(disp, disp_chan);

    hal_disp_get_state(disp, &current_timing_status->btm, &current_timing_status->vline,
                       &current_timing_status->int_cnt);

    if (disp_chan->setting.format != HI_DRV_DISP_FMT_CUSTOM) {
        (hi_void) drv_disp_timing_get_timing_info(disp_chan->setting.format, &timing_cfg, &timing_info);
        total_line = timing_cfg.timing.vact + timing_cfg.timing.vfb + timing_cfg.timing.vbb;
        fmt_refresh_rate = timing_cfg.timing.refresh_rate;
    } else {
        total_line = disp_chan->setting.custom_timing.vact + disp_chan->setting.custom_timing.vfb +
                     disp_chan->setting.custom_timing.vbb;
        fmt_refresh_rate = disp_chan->setting.custom_timing.refresh_rate;
    }

    if (fmt_refresh_rate == 0 || total_line == 0) {
        hi_warn_disp("Denominator may be zero !\n");
        return HI_FAILURE;
    }

    /* for 60hz,  the circle_time will be 16.67ms, not 16 */
    current_timing_status->circle_time_us = (100000 * 1000)/ fmt_refresh_rate;
    current_timing_status->left_time = ((100000 / fmt_refresh_rate) * (total_line - current_timing_status->vline)) /
                                       total_line;

    if (current_timing_status->circle_time_us > (DISP_CIRCLE_TIME_MAX_MS * 1000)) {
        current_timing_status->circle_time_us = (DISP_CIRCLE_TIME_MAX_MS * 1000);
    }

    if (current_timing_status->left_time > DISP_LEFT_TIME_MAX_MS) {
        current_timing_status->left_time = DISP_LEFT_TIME_MAX_MS;
    }

    return HI_SUCCESS;
}

static hi_void disp_offset_calculation(hi_drv_disp_offset *offset, hi_rect *fmt_resolution,
                                      hi_rect *virtaul_screen, hi_drv_disp_offset *calc_reult,hi_rect *disp_rect)
{
    if ((virtaul_screen->width == 0)
        ||  (virtaul_screen->height == 0)) {
        virtaul_screen->width = DISPLAY_DEFAULT_VIRT_SCREEN_WIDTH;
        virtaul_screen->height = DISPLAY_DEFAULT_VIRT_SCREEN_HEIGHT;
    }

    calc_reult->left = (offset->left * fmt_resolution->width) / (virtaul_screen->width);
    calc_reult->right = (offset->right * fmt_resolution->width) / (virtaul_screen->width);
    calc_reult->top = (offset->top * fmt_resolution->height) / (virtaul_screen->height);
    calc_reult->bottom = (offset->bottom * fmt_resolution->height) / (virtaul_screen->height);

    calc_reult->left &= HI_DRV_DISP_OFFSET_HORIZONTAL_ALIGN;
    calc_reult->right &= HI_DRV_DISP_OFFSET_HORIZONTAL_ALIGN;
    calc_reult->top &= HI_DRV_DISP_OFFSET_VERTICAL_ALIGN;
    calc_reult->bottom &= HI_DRV_DISP_OFFSET_VERTICAL_ALIGN;

    disp_rect->x = calc_reult->left;
    disp_rect->y = calc_reult->top;

    if (calc_reult->left + calc_reult->right + SCREEN_DISP_WIDTH_MIN <= fmt_resolution->width) {
        disp_rect->width = fmt_resolution->width - calc_reult->left - calc_reult->right;
    } else {
        disp_rect->width = SCREEN_DISP_WIDTH_MIN;
    }

    if (calc_reult->top + calc_reult->bottom + SCREEN_DISP_HEIGHT_MIN <= fmt_resolution->height) {
        disp_rect->height = fmt_resolution->height - calc_reult->top - calc_reult->bottom;
    } else {
        disp_rect->height = SCREEN_DISP_HEIGHT_MIN;
    }

    return;
}

static hi_void disp_transform_color_descript(hi_drv_disp_output_color_space color_space_mode,
                                             hi_drv_color_descript *color_descript)
{
    if (color_space_mode == HI_DRV_DISP_COLOR_SPACE_BT2020) {
        color_descript->color_space = HI_DRV_COLOR_CS_YUV;
        color_descript->quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
        color_descript->color_primary = HI_DRV_COLOR_PRIMARY_BT2020;
        color_descript->transfer_type = HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR;
        color_descript->matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT;
    } else if (color_space_mode == HI_DRV_DISP_COLOR_SPACE_BT601) {
        color_descript->color_space = HI_DRV_COLOR_CS_YUV;
        color_descript->quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
        color_descript->color_primary = HI_DRV_COLOR_PRIMARY_BT601_525;
        color_descript->transfer_type = HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR;
        color_descript->matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT601_525;

    } else {
        color_descript->color_space = HI_DRV_COLOR_CS_YUV;
        color_descript->quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
        color_descript->color_primary = HI_DRV_COLOR_PRIMARY_BT709;
        color_descript->transfer_type = HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR;
        color_descript->matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
    }
    return;
}

hi_void drv_disp_produce_display_info(disp_setting *disp_setting, hi_disp_display_info *info,
                                      hi_drv_disp_out_type drv_disp_type,
                                      hi_drv_disp_output_color_space color_space_mode)
{
    disp_timing_info timing_info = {0};

    info->fmt = disp_setting->format;
    info->alpha = (disp_setting->alpha * HI_DRV_DISP_LOGIC_ALPHA_MAX) / HI_DRV_DISP_ALPHA_MAX;
    info->match_mode = disp_setting->match_mode;

    (hi_void)disp_get_cfg_timing(HI_DRV_DISPLAY_0, disp_setting->format,
                                 &timing_info, &(disp_setting->custom_timing));

    info->interlace = timing_info.interlace;
    info->refresh_rate = timing_info.refresh_rate;
    info->fmt_resolution.x = 0;
    info->fmt_resolution.y = 0;
    info->fmt_resolution.width = timing_info.sync_info.hact;
    info->fmt_resolution.height = timing_info.sync_info.vact;

    if (timing_info.interlace == HI_TRUE) {
        info->fmt_resolution.height *= DISP_INTERLACE_HEIGHT_COEFFICIENT;
    }
    info->master = disp_setting->master;
    info->slave = disp_setting->slave;
    info->enable = disp_setting->enable;
    info->stereo = disp_setting->stereo;
    info->virtaul_screen = disp_setting->virtaul_screen;
    disp_offset_calculation(&disp_setting->offset_info, &info->fmt_resolution,
                            &disp_setting->virtaul_screen, &info->offset_info,
                            &info->pixel_fmt_resolution);

    /*not support offset */
    info->pixel_fmt_resolution = info->fmt_resolution;
    info->color_space_mode = color_space_mode;
    if (((disp_setting->format >= HI_DRV_DISP_FMT_NTSC) &&
         (disp_setting->format <= HI_DRV_DISP_FMT_1440x576I_50))) {
        info->color_space_mode = HI_DRV_DISP_COLOR_SPACE_BT601;
    }

    disp_transform_color_descript(info->color_space_mode, &info->color_space);
    if ((disp_setting->format == HI_DRV_DISP_FMT_CUSTOM) ||
        ((disp_setting->format >= HI_DRV_DISP_FMT_861D_640X480_60) &&
         (disp_setting->format <= HI_DRV_DISP_FMT_VESA_2560X1600_60_RB))) {
        info->color_space.color_space = HI_DRV_COLOR_CS_RGB;
        info->color_space.quantify_range = HI_DRV_COLOR_FULL_RANGE;
        info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
        info->color_space.transfer_type = HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR;
        info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
    }
    info->disp_type = drv_disp_type;
    info->aspect_ratio = disp_setting->aspect_ratio;

    return;
}

hi_s32 drv_disp_set_output(hi_drv_display disp, const hi_drv_disp_out_type drv_disp_type)
{
    disp_channel *disp_chan = HI_NULL;

    if ((drv_disp_type != HI_DRV_DISP_TYPE_NORMAL) &&
        (drv_disp_type != HI_DRV_DISP_TYPE_HDR10) &&
        (drv_disp_type != HI_DRV_DISP_TYPE_HLG)) {
        hi_err_disp("disp(%d) cannot support type(%d)\n", disp, drv_disp_type);
        return HI_FAILURE;
    }

    if ((disp != HI_DRV_DISPLAY_0) && (drv_disp_type != HI_DRV_DISP_TYPE_NORMAL)) {
        hi_err_disp("disp(%d) cannot support type(%d)\n", disp, drv_disp_type);
        return HI_FAILURE;
    }

    GET_CHANNEL_BY_ID(disp, disp_chan);
    disp_chan->setting.disp_type = drv_disp_type;
    return HI_SUCCESS;
}

hi_s32 drv_disp_get_output(hi_drv_display disp, hi_drv_disp_out_type *drv_disp_type)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);

    *drv_disp_type = disp_chan->setting.disp_type;

    return HI_SUCCESS;
}

hi_void disp_device_init(drv_disp_device *disp_dev)
{
    hi_u32 i = 0;
    hi_s32 ret;
    hi_u32 value = 0;

    for (i = 0; i < HI_DRV_DISPLAY_BUTT; i++) {
        disp_dev->disp[i].disp = i;
        disp_dev->disp[i].open = HI_FALSE;
        disp_dev->disp[i].disp_setting_change = HI_FALSE;
        disp_dev->disp[i].setting.alpha = 100;
        disp_dev->disp[i].setting.enable = HI_FALSE;

        if (i == HI_DRV_DISPLAY_1) {
            ret = hi_drv_get_license_support(HI_LICENSE_DISPLAY2_EN, &value);
            if (ret != 0) {
                hi_err_disp("vdp get license err(0x%x)\n", ret);
            }
            disp_dev->disp[i].setting.licensed = (value == 1) ? HI_TRUE : HI_FALSE;
        } else {
            disp_dev->disp[i].setting.licensed = HI_TRUE;
        }

        disp_dev->disp[i].setting.master = HI_FALSE;
        disp_dev->disp[i].setting.slave = HI_FALSE;
        disp_dev->disp[i].setting.attached_disp = HI_DRV_DISPLAY_BUTT;
        disp_dev->disp[i].setting.stereo = HI_DRV_DISP_STEREO_NONE;

        disp_dev->disp[i].setting.format = HI_DRV_DISP_FMT_720P_50;
        disp_dev->disp[i].setting.right_eye_first = HI_FALSE;

        disp_dev->disp[i].setting.bg_color.red = DISP_DEFAULT_COLOR_RED;
        disp_dev->disp[i].setting.bg_color.green = DISP_DEFAULT_COLOR_GREEN;
        disp_dev->disp[i].setting.bg_color.blue = DISP_DEFAULT_COLOR_BLUE;

        disp_dev->disp[i].setting.aspect_ratio.aspect_ratio_h = DISP_ASPECT_RATION_H;
        disp_dev->disp[i].setting.aspect_ratio.aspect_ratio_w = DISP_ASPECT_RATION_W;

        disp_dev->disp[i].setting.virtaul_screen.x = 0;
        disp_dev->disp[i].setting.virtaul_screen.y = 0;
        disp_dev->disp[i].setting.virtaul_screen.width = 1280;
        disp_dev->disp[i].setting.virtaul_screen.height = 720;

        disp_dev->disp[i].setting.offset_info.left = 0;
        disp_dev->disp[i].setting.offset_info.right = 0;
        disp_dev->disp[i].setting.offset_info.top = 0;
        disp_dev->disp[i].setting.offset_info.bottom = 0;

        disp_dev->disp[i].setting.sink_capability.support_bt601 = HI_TRUE;
        disp_dev->disp[i].setting.sink_capability.support_bt709 = HI_TRUE;
        disp_dev->disp[i].setting.sink_capability.support_sdr = HI_TRUE;

        disp_dev->disp[i].disp_debug.debug_data_width = HI_DRV_PIXEL_BITWIDTH_MAX;
        disp_dev->disp[i].disp_debug.debug_pixel_format = HI_DRV_DISP_PIXEL_MAX;
    }
}

hi_void disp_device_deinit(drv_disp_device *disp_dev)
{
    memset(&g_disp_dev, 0, sizeof(drv_disp_device));
}

hi_s32 drv_disp_init(hi_void)
{
    hi_s32 ret;
    hi_drv_display disp;
    disp_channel *disp_chan;
    hi_bool enable;

    memset(&g_disp_dev, 0, sizeof(drv_disp_device));
    disp_device_init(&g_disp_dev);

    ret = hal_vdp_init();
    if (ret != HI_SUCCESS) {
        return ret;
    }

    for (disp = HI_DRV_DISPLAY_0; disp < HI_DRV_DISPLAY_2; disp++) {
        GET_CHANNEL_BY_ID(disp, disp_chan);
        ret = hal_disp_get_enable(disp, &enable);
        disp_chan->setting.enable = enable;
    }

#ifdef HI_TEE_SUPPORT
    ret = hi_drv_ssm_iommu_config(LOGIC_MOD_ID_VDP);
    if (ret != HI_SUCCESS) {
        hi_err_disp("init tee config err 0x%x\n", ret);
        return ret;
    }
#endif

    ret = disp_isr_init();
    if (ret != HI_SUCCESS) {
        return ret;
    }

    g_disp_dev.is_device_init = HI_TRUE;
    return HI_SUCCESS;
}
hi_s32 drv_disp_de_init(hi_void)
{
    hi_s32 ret;

    ret = disp_isr_deinit();
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = hal_vdp_deinit();
    if (ret != HI_SUCCESS) {
        return ret;
    }

    disp_device_deinit(&g_disp_dev);

    g_disp_dev.is_device_init = HI_FALSE;
    return HI_SUCCESS;
}

hi_s32 disp_get_pdm_param(hi_drv_display disp)
{
    hi_s32 ret;
    hi_u32 i;
    hi_disp_param disp_pdm_param = {0};
    disp_channel *disp_chan;
    hi_drv_disp_format_param format_param;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    ret = hi_drv_pdm_get_disp_param(disp, &disp_pdm_param);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get pdm param is error\n");
        return ret;
    }

#ifdef VDP_SUPPORT_PANEL
    disp_chan->out_connect_type = DISP_OUTPUT_PANEL_TYPE;
#endif

    for (i = 0; i < HI_DRV_DISP_INTF_TYPE_MAX; i++) {
        if (disp_pdm_param.intf[i].intf_type < HI_DRV_DISP_INTF_TYPE_ALL) {
            ret = drv_disp_add_intf(disp, disp_pdm_param.intf[i]);
            if (ret != HI_SUCCESS) {
                hi_err_disp("disp%d add intf(%d) is error\n",
                            disp,
                            disp_pdm_param.intf[i].intf_type);
                continue;
            }

            format_param.intf_output[0].intf = disp_pdm_param.intf[i];
            format_param.intf_output[0].bit_width = disp_pdm_param.deep_color_mode;
            format_param.intf_output[0].pixel_fmt = disp_pdm_param.vid_out_mode;
            format_param.intf_output[0].enable = HI_TRUE;
            format_param.number = 1; /* 1 is a intf number */
            ret = drv_disp_intf_set_attr(disp, &format_param);
            if (ret != HI_SUCCESS) {
                hi_err_disp("set attr is error\n");
            }

            ret = drv_disp_intf_set_enable_status(disp, &format_param);
            if (ret != HI_SUCCESS) {
                hi_err_disp("set enable attr is error\n");
            }
        }
    }

    disp_chan->setting.format = disp_pdm_param.format;
    disp_chan->setting.virtaul_screen.width = disp_pdm_param.virt_screen_width;
    disp_chan->setting.virtaul_screen.height = disp_pdm_param.virt_screen_height;
    disp_chan->setting.offset_info = disp_pdm_param.offset_info;
    disp_chan->setting.bg_color = disp_pdm_param.bg_color;
    disp_chan->setting.aspect_ratio = disp_pdm_param.aspect_ratio;
    disp_chan->setting.custom_timing = disp_pdm_param.disp_timing;

    return ret;
}

static hi_drv_panel_export_func *g_panel_func = HI_NULL;
hi_s32 disp_get_panel_timming_process(hi_drv_panel_disp_info *panel_src_info,
    disp_timing_info *timing_info)
{
    hi_drv_panel_cfg panel_config;
    hi_s32 ret = hi_drv_module_get_func(HI_ID_PANEL, (hi_void **)&g_panel_func);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get panel func faild\n");
        return ret;
    }

    ret = g_panel_func->panel_isr_main(panel_src_info, &panel_config);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get panel_isr_main faild\n");
        return ret;
    }

    timing_info->disp_fmt = HI_DRV_DISP_FMT_7680X4320_120;
    timing_info->disp_3d_mode = HI_DRV_DISP_STEREO_NONE; /* update by dispinfo */
    timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
    timing_info->color_space.color_space = HI_DRV_COLOR_CS_YUV;
    timing_info->color_space.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
    timing_info->color_space.transfer_type = HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR;
    timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;

    timing_info->interlace = HI_FALSE;
    timing_info->refresh_rate = panel_config.frame_rate;
    timing_info->pixel_clk = panel_config.timing.pixel_clk / 100; /* 100 is a ratio */
    timing_info->vic_num = 0;
    timing_info->aspect_ratio.aspect_ratio_h = 9; /* 9 is a disp ratio */
    timing_info->aspect_ratio.aspect_ratio_w = 16; /* 16 is a disp ratio */

    timing_info->sync_info.iop = 1;
    timing_info->sync_info.vact = panel_config.height;
    timing_info->sync_info.vfb = panel_config.timing.vsync_fp;
    timing_info->sync_info.vbb = panel_config.timing.vtotal - timing_info->sync_info.vfb - timing_info->sync_info.vact;

    timing_info->sync_info.hact = panel_config.width;
    timing_info->sync_info.hfb = panel_config.timing.hsync_fp;
    timing_info->sync_info.hbb = panel_config.timing.htotal - timing_info->sync_info.hfb - timing_info->sync_info.hact;

    timing_info->sync_info.hpw = panel_config.timing.hsync_width;
    timing_info->sync_info.vpw = panel_config.timing.vsync_width;

    /* panel not have inferface ouput, no use */
    timing_info->sync_info.bvact = 1;
    timing_info->sync_info.bvbb = 1;
    timing_info->sync_info.bvfb = 1;
    timing_info->sync_info.hmid = 1;

    timing_info->sync_info.idv = 0; /* panel do not need change, fix 0 */
    timing_info->sync_info.ihs = panel_config.timing_sync_info.hsync_negative;
    timing_info->sync_info.ivs = panel_config.timing_sync_info.vsync_negative;

    return HI_SUCCESS;
}

#define VDP_DEFAULT_PANEL_SRC_FRAMERATE 60000
#define VDP_DEFAULT_PANEL_SRC_WIDTH 3840
#define VDP_DEFAULT_PANEL_SRC_HEIGHT 2160
hi_s32 disp_set_channel_open(hi_drv_display disp, hi_drv_disp_stereo_mode stereo, hi_drv_disp_fmt format)
{
    hi_s32 ret;
    hi_u32 number;

    disp_channel *disp_chan;
    hi_drv_disp_format_param format_param;
    hi_drv_disp_intf_info intf_info;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    if (disp_chan->out_connect_type == DISP_OUTPUT_PANEL_TYPE) {
        hi_drv_panel_disp_info panel_src_info;
        hi_drv_disp_intf intf_attr;
        disp_timing_info timing_info;

        panel_src_info.frame_rate = VDP_DEFAULT_PANEL_SRC_FRAMERATE;
        panel_src_info.width = VDP_DEFAULT_PANEL_SRC_WIDTH;
        panel_src_info.height = VDP_DEFAULT_PANEL_SRC_HEIGHT;
        if (disp_get_panel_timming_process(&panel_src_info, &timing_info) == HI_SUCCESS) {
            hal_disp_set_timing(disp, &intf_attr, &timing_info);
        }

        ret = drv_disp_set_enable(disp, HI_TRUE);
        return ret;
    }

    ret = disp_get_channel_attr(disp, &format_param, &intf_info);
    if (ret != HI_SUCCESS) {
        hi_err_disp("update intf is error(%x)\n", ret);
        return ret;
    }

    format_param.disp_3d_mode = stereo;
    format_param.format = format;
    (hi_void)disp_set_output_timing(disp, stereo, &format_param, &(disp_chan->setting.custom_timing));

    ret = drv_disp_set_enable(disp, HI_TRUE);
    if (ret != HI_SUCCESS) {
        hi_err_disp("set disp%d enable is error\n", disp);
        return ret;
    }

    for (number = 0; number < format_param.number; number++) {
        ret = drv_disp_set_intf_enable(disp, format_param.intf_output[number].intf, HI_TRUE);
        if (ret != HI_SUCCESS) {
            hi_err_disp("disp%d set intf%d enable is error\n",
                        disp,
                        format_param.intf_output[number].intf.intf_type);
        }
    }

    return ret;
}


hi_s32 drv_disp_open(hi_drv_display disp)
{
    hi_s32 ret;
    hi_bool is_disp_opened;
    disp_channel *disp_chan = HI_NULL;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    is_disp_opened = drv_disp_is_opened(disp);
    if (is_disp_opened == HI_TRUE) {
        return HI_SUCCESS;
    }

    drv_disp_set_bgcolor(disp, &disp_chan->setting.bg_color);

    ret = disp_isr_open_chn(disp);
    if (ret != HI_SUCCESS) {
        hi_err_disp("disp%d open isr chn error\n", disp);
        return ret;
    }

    disp_isr_set_disp_info(disp, &disp_chan->disp_info);

    ret = disp_get_pdm_param(disp);
    if (ret != HI_SUCCESS) {
        hi_err_disp("config pdm param is error\n");
        return ret;
    }

    drv_xdp_ctrl_init(disp);

    ret = disp_set_channel_open(disp, disp_chan->setting.stereo, disp_chan->setting.format);
    HI_ASSERT(ret == HI_SUCCESS);

    disp_chan->open = HI_TRUE;

    return ret;
}

hi_s32 drv_disp_close(hi_drv_display disp)
{
    hi_s32 ret = HI_SUCCESS;
    hi_bool is_disp_opened;
    disp_channel *disp_chan = HI_NULL;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    is_disp_opened = drv_disp_is_opened(disp);
    if (is_disp_opened != HI_TRUE) {
        return HI_SUCCESS;
    }

    drv_disp_set_enable(disp, HI_FALSE);
    disp_isr_close_chn(disp);

    drv_xdp_ctrl_deinit(disp);
    disp_chan->open = HI_FALSE;
    return ret;
}

hi_s32 drv_disp_get_vact_time(disp_channel *disp_chan)
{
    hi_s32 vtime;

    if (disp_chan->disp_info.refresh_rate != 0) {
        vtime = (DISP_DEFAULT_TIME_1000_MS * DISP_REFRESH_UINT_100_HZ) /
                disp_chan->disp_info.refresh_rate;
    } else {
        vtime = DISP_MAX_VACT_TIME;
    }

    if (vtime > DISP_MAX_VACT_TIME) {
        vtime = DISP_MAX_VACT_TIME;
    } else if (vtime < DISP_MIN_VACT_TIME) {
        vtime = DISP_MIN_VACT_TIME;
    }

    return vtime;
}

hi_s32 drv_disp_set_enable(hi_drv_display disp, hi_bool enable)
{
    hi_s32 ret;
    hi_s32 vtime;
    disp_channel *disp_chan;

    GET_CHANNEL_BY_ID(disp, disp_chan);

    if (disp_chan->setting.enable == enable) {
        hi_warn_disp("set same disp status %d!\n", enable);
        return HI_SUCCESS;
    }

    ret = hal_disp_set_enable(disp, enable);
    if (ret != HI_SUCCESS) {
        hi_err_disp("set disp%d enable is error(%x)\n", disp, ret);
        return ret;
    }

    disp_chan->setting.enable = enable;

    if (enable == HI_FALSE) {
        vtime = drv_disp_get_vact_time(disp_chan);
        osal_msleep_uninterruptible(DISP_VACT_CNT * vtime);
    }

    return HI_SUCCESS;
}

hi_s32 drv_disp_get_enable(hi_drv_display disp, hi_bool *enable)
{
    disp_channel *disp_chan;
    GET_CHANNEL_BY_ID(disp, disp_chan);

    *enable = disp_chan->setting.enable;
    return HI_SUCCESS;
}

hi_s32 drv_display_suspend(hi_void)
{
    disp_channel *disp_chan = HI_NULL;
    hi_drv_display disp;
    hi_s32 ret;

    if (g_disp_suspend == HI_TRUE) {
        HI_PRINT("disp is already suspend\n");
        return HI_SUCCESS;
    }
    g_disp_suspend = HI_TRUE;

    win_suspend();

    for (disp = HI_DRV_DISPLAY_0; disp < HI_DRV_DISPLAY_2; disp++) {
        GET_CHANNEL_BY_ID(disp, disp_chan);
        if (disp_chan->open == HI_TRUE) {
            ret = drv_disp_intf_suspend(disp);
            if (ret != HI_SUCCESS) {
                hi_err_disp("drv_disp_intf_resume error(%x)\n", ret);
            }
            drv_disp_set_enable(disp, HI_FALSE);
        }
    }

    hal_vdp_suspend();

    HI_PRINT("disp suspend ok.\n");
    return HI_SUCCESS;
}

hi_s32 drv_display_resume(hi_void)
{
    disp_channel *disp_chan = HI_NULL;
    hi_drv_display disp;
    hi_s32 ret;

    if (g_disp_suspend == HI_FALSE) {
        HI_PRINT("disp is not suspend\n");
        return HI_SUCCESS;
    }

    hal_vdp_resume();
    for (disp = HI_DRV_DISPLAY_0; disp < HI_DRV_DISPLAY_2; disp++) {
        GET_CHANNEL_BY_ID(disp, disp_chan);
        if (disp_chan->open == HI_TRUE) {
            ret = drv_disp_intf_resume(disp);
            if (ret != HI_SUCCESS) {
                hi_err_disp("drv_disp_intf_suspend error(%x)\n", ret);
            }
            disp_set_channel_open(disp, disp_chan->setting.stereo, disp_chan->setting.format);
        }
    }

    win_resume();
    g_disp_suspend = HI_FALSE;
    HI_PRINT("disp resume ok.\n");
    return HI_SUCCESS;
}

hi_s32 drv_disp_get_proc_into(hi_drv_display disp, disp_proc_info *info)
{
    disp_channel *disp_chan;
    hi_drv_disp_static_timing timing_cfg;
    disp_timing_info timing_info;

    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(info);
    GET_CHANNEL_BY_ID(disp, disp_chan);

    info->enable = disp_chan->setting.enable;
    info->fmt = disp_chan->setting.format;
    info->master = disp_chan->setting.master;
    info->slave = disp_chan->setting.slave;
    info->licensed = disp_chan->setting.licensed;

    info->vrr_cfg = disp_chan->setting.vrr_cfg;
    info->qms_enable= disp_chan->setting.qms_enable;
    info->qft_factor = disp_chan->setting.factor;
    info->allm_en = disp_chan->setting.allm_en;
    if (info->fmt == HI_DRV_DISP_FMT_CUSTOM) {
        info->timing = disp_chan->setting.custom_timing;
    } else {
        (hi_void) drv_disp_timing_get_timing_info(info->fmt, &timing_cfg, &timing_info);
        info->timing = timing_cfg.timing;
    }

    info->stereo = disp_chan->setting.stereo;
    info->right_eye_first = disp_chan->setting.right_eye_first;
    info->virtaul_screen = disp_chan->setting.virtaul_screen;
    info->offset_info = disp_chan->setting.offset_info;
    info->actual_offset_info = disp_chan->disp_info.offset_info;

    info->ar_w = disp_chan->disp_info.aspect_ratio.aspect_ratio_w;
    info->ar_h = disp_chan->disp_info.aspect_ratio.aspect_ratio_h;

    info->match_mode = disp_chan->disp_info.match_mode;
    info->disp_type = disp_chan->setting.disp_type;
    info->color_space_mode = disp_chan->setting.color_space_mode;
    info->actual_output_type = disp_chan->disp_info.disp_type;
    info->actual_output_colorspace = disp_chan->disp_info.color_space_mode;
    info->color_space = disp_chan->disp_info.color_space;

    info->alpha = disp_chan->disp_info.alpha;
    info->bg_color = disp_chan->setting.bg_color;
    info->data_width = disp_chan->disp_debug.debug_data_width;
    info->pixel_format = disp_chan->disp_debug.debug_pixel_format;
    info->aspect_mode = disp_chan->setting.aspect_ratio.aspect_mode;
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */








