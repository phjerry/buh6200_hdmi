/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: ion  adapter code
* Author: image
* Create: 2019-03-28
 */

#include "hi_type.h"
#include "hi_errno.h"

#include "hi_drv_video.h"
#include "hi_drv_disp.h"
#include "hi_drv_win.h"
#include "drv_display.h"
#include "drv_xdp_osal.h"
#include "osal_list.h"
#include "drv_window_priv.h"
#include "drv_window.h"
#include "drv_disp_isr.h"
#include "linux/hisilicon/securec.h"
#include "drv_win_mng.h"
#include "hal_layer_intf.h"
#include "hi_drv_proc.h"

#include "drv_hdmi_module_ext.h"
typedef struct hihdr_policy_result_info {
    hi_drv_disp_out_type disp_output_hdr_type;
    hi_drv_disp_output_color_space disp_output_csc_type;
    hi_drv_hdr_metadata     hdr_info;
    hi_bool is_dynamic_mentata;

} hdr_policy_result_info;

typedef struct  {
    win_runtime_attr_s attr;
    win_layermapping_result layermapping;

    hi_drv_disp_out_type disp_output_hdr_type;
    hi_drv_disp_output_color_space disp_output_csc_type;

    hi_drv_video_frame *frame_info;
} win_policy_result_info;

typedef struct {
    hi_u32 activate_win_num;
    hi_u32 master_win_index;
    win_policy_result_info current_info[WINDOW_MAX_NUMBER];
    win_policy_result_info expected_info[WINDOW_MAX_NUMBER];
} win_config_info;

typedef struct {
    vdp_layer_info basic_config_info[LAYER_ID_BUTT];
    vdp_hdr_info hdr_config_info[LAYER_ID_BUTT];
} layer_hal_config_infor;

typedef struct {
    hi_s32  init_cnt;
    hdr_policy_result_info last_out_put_info;
    hdr_policy_result_info out_put_info;
    win_config_info win_cfg;
    hi_drv_disp_sink_capability sink_cap;
} xdp_ctrl_info;

#define INVALID_INDEX   (0xffffffff)
#define DEFAULT_FRAME_RATE 60000
layer_hal_config_infor g_layer_hal_conf_info;
xdp_ctrl_info g_xdp_ctrl_info[HI_DRV_DISPLAY_BUTT] = { 0 };

static struct hi_hdmi_ext_intf *g_hdmi_intf = HI_NULL;
xdp_ctrl_info *xdpctrl_get_ctrl_info(hi_drv_display disp_chn)
{
    return &g_xdp_ctrl_info[disp_chn];
}

#ifdef OPEN_XDP_CTRL_POLICY_
/****************************begin to frame sync*********************************/
static hi_bool xdpctrl_is_attr_need_match(win_attr_status *expected_win_attr,
                                          win_attr_status *using_win_attr,
                                          win_attr_status *gfx_current_win_attr,
                                          hi_u32 win_num,
                                          hi_u32 gfx_win_counts)
{
    return HI_FALSE;
}

static hi_bool xdpctrl_is_hdr_type_need_match(hi_drv_disp_out_type *expect_output_hdr_type,
                                              hi_drv_disp_out_type *using_output_hdr_type,
                                              hi_drv_hdr_type *frame_hdr_type,
                                              hi_u32 win_num)
{
    return HI_FALSE;
}

/* FIXME: do sync, and return the sync result,
  * include gfx and video. now we do not distinguish the gfx and video.
 */
static xdp_ctrl_framesync_result xdpctrl_do_winbuf_sync(win_attr_status *expected_win_attr,
                                                        win_attr_status *using_win_attr,
                                                        win_attr_status *gfx_current_win_attr,
                                                        hi_u32 win_num,
                                                        hi_u32 gfx_win_counts,
                                                        hi_drv_disp_out_type *expect_output_hdr_type,
                                                        hi_drv_disp_out_type *using_output_hdr_type,
                                                        hi_drv_hdr_type *frame_hdr_type)
{
    return 0;
}

#endif

hi_void xdp_ctrl_set_multi_region_cfg_frame(hi_u32 layer_id,
                                            hi_u32 region_num,
                                            hi_drv_video_frame *cfg_frame,
                                            win_runtime_attr_s *attr,
                                            vdp_layer_info *hal_cfg_info)
{
    hi_drv_win_vpss_meta_info *metadata_info;
    if (region_num >= FDR_REGION_NUM_MAX) {
        WIN_ERROR("region_num id err! %d\n", region_num);
        return;
    }

    hal_cfg_info->region[region_num].enable = attr->win_enable;

    if (cfg_frame != HI_NULL) {

        if ((attr->disp_rect.height != cfg_frame->height)
            || (attr->disp_rect.width != cfg_frame->width)) {
            hal_cfg_info->region[region_num].enable = HI_FALSE;
            return;
        }
        hal_cfg_info->region[region_num].smmu_en = HI_TRUE;
        hal_cfg_info->region[region_num].mute_en = HI_FALSE;
        hal_cfg_info->region[region_num].in_rect.x = 0;
        hal_cfg_info->region[region_num].in_rect.y = 0;
        hal_cfg_info->region[region_num].in_rect.w = cfg_frame->width;
        hal_cfg_info->region[region_num].in_rect.h = cfg_frame->height;
        hal_cfg_info->region[region_num].disp_rect.x = cfg_frame->disp_rect.rect_x;
        hal_cfg_info->region[region_num].disp_rect.y = cfg_frame->disp_rect.rect_y;
        hal_cfg_info->region[region_num].disp_rect.w = cfg_frame->width;
        hal_cfg_info->region[region_num].disp_rect.h = cfg_frame->height;
        metadata_info = (hi_drv_win_vpss_meta_info *)cfg_frame->video_private.vpss_private.data;
        hal_cfg_info->region[region_num].addr[VDP_FDR_MRG_ADDR_DATA].lum_addr =
            metadata_info->buf_addr[0].phy_addr_y;
        hal_cfg_info->region[region_num].addr[VDP_FDR_MRG_ADDR_DATA].chm_addr =
            metadata_info->buf_addr[0].phy_addr_c;
        hal_cfg_info->region[region_num].addr[VDP_FDR_MRG_ADDR_DATA].lum_str =
            cfg_frame->buf_addr[0].stride_y;
        hal_cfg_info->region[region_num].addr[VDP_FDR_MRG_ADDR_DATA].chm_str =
            cfg_frame->buf_addr[0].stride_c;
    } else {
        hal_cfg_info->region[region_num].smmu_en = HI_TRUE;
        hal_cfg_info->region[region_num].mute_en = HI_TRUE;
        hal_cfg_info->region[region_num].in_rect.x = 0;
        hal_cfg_info->region[region_num].in_rect.y = 0;
        hal_cfg_info->region[region_num].in_rect.w = attr->video_rect.width;
        hal_cfg_info->region[region_num].in_rect.h = attr->video_rect.height;
        hal_cfg_info->region[region_num].disp_rect.x = attr->video_rect.x;
        hal_cfg_info->region[region_num].disp_rect.y = attr->video_rect.y;
        hal_cfg_info->region[region_num].disp_rect.w = attr->video_rect.width;
        hal_cfg_info->region[region_num].disp_rect.h = attr->video_rect.height;
    }
    return;
}

hi_void xdp_ctrl_set_single_region_frame_dafult_cfg(hi_u32 layer_id,
                                                    win_runtime_attr_s *attr,
                                                    layer_hal_config_infor *hal_cfg_info)

{
    hal_cfg_info->basic_config_info[layer_id].frame_smmu_en = HI_TRUE;
    hal_cfg_info->basic_config_info[layer_id].frame_rate = DEFAULT_FRAME_RATE; /* 60000 is default rate */
    hal_cfg_info->basic_config_info[layer_id].frame_inerlace = HI_FALSE;
    hal_cfg_info->basic_config_info[layer_id].frame_field_mode = HI_DRV_FIELD_ALL;
    hal_cfg_info->basic_config_info[layer_id].frame_secure_en = HI_FALSE;
    hal_cfg_info->basic_config_info[layer_id].mute_en = HI_TRUE;
    hal_cfg_info->basic_config_info[layer_id].frame_rect.x = 0;
    hal_cfg_info->basic_config_info[layer_id].frame_rect.y = 0;
    hal_cfg_info->basic_config_info[layer_id].frame_rect.w = attr->video_rect.width;
    hal_cfg_info->basic_config_info[layer_id].frame_rect.h = attr->video_rect.height;
    hal_cfg_info->basic_config_info[layer_id].crop_rect = hal_cfg_info->basic_config_info[layer_id].frame_rect;

    hal_cfg_info->basic_config_info[layer_id].frame_color_space.color_space = HI_DRV_COLOR_CS_YUV;
    hal_cfg_info->basic_config_info[layer_id].frame_color_space.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
    hal_cfg_info->basic_config_info[layer_id].frame_color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
    hal_cfg_info->basic_config_info[layer_id].frame_color_space.transfer_type = HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR;;
    hal_cfg_info->basic_config_info[layer_id].frame_color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;

    hal_cfg_info->basic_config_info[layer_id].frame_pix_format = HI_DRV_PIXEL_FMT_NV21;
    hal_cfg_info->basic_config_info[layer_id].data_fmt = HI_DRV_DATA_FMT_LINER;
    hal_cfg_info->basic_config_info[layer_id].cmp_mode = HI_DRV_COMPRESS_MODE_OFF;

    hal_cfg_info->basic_config_info[layer_id].ycmp_rate = 0;
    hal_cfg_info->basic_config_info[layer_id].ccmp_rate = 0;
    hal_cfg_info->basic_config_info[layer_id].frame_bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
    return;
}

hi_void xdp_ctrl_set_single_region_frame_addr(hi_drv_video_frame *frame_info, vdp_layer_info *layer_config)
{
    hi_drv_win_vpss_meta_info *metadata_info;
    metadata_info = (hi_drv_win_vpss_meta_info *)frame_info->video_private.vpss_private.data;

    layer_config->frame_addr[VDP_FDR_VID_ADDR_DATA].lum_addr = metadata_info->buf_addr[0].phy_addr_y;
    layer_config->frame_addr[VDP_FDR_VID_ADDR_DATA].chm_addr = metadata_info->buf_addr[0].phy_addr_c;
    layer_config->frame_addr[VDP_FDR_VID_ADDR_DATA].lum_str = frame_info->buf_addr[0].stride_y;
    layer_config->frame_addr[VDP_FDR_VID_ADDR_DATA].chm_str = frame_info->buf_addr[0].stride_c;
    layer_config->frame_addr[VDP_FDR_VID_ADDR_HEAD].lum_addr = metadata_info->buf_addr[0].phy_addr_y_head;
    layer_config->frame_addr[VDP_FDR_VID_ADDR_HEAD].chm_addr = metadata_info->buf_addr[0].phy_addr_c_head;
    layer_config->frame_addr[VDP_FDR_VID_ADDR_HEAD].lum_str = frame_info->buf_addr[0].head_stride;
    layer_config->frame_addr[VDP_FDR_VID_ADDR_HEAD].chm_str = frame_info->buf_addr[0].head_stride;
    layer_config->frame_addr[VDP_FDR_VID_ADDR_2B].lum_addr = metadata_info->buf_addr_lb[0].phy_addr_y;
    layer_config->frame_addr[VDP_FDR_VID_ADDR_2B].chm_addr = metadata_info->buf_addr_lb[0].phy_addr_c;
    layer_config->frame_addr[VDP_FDR_VID_ADDR_2B].lum_str = frame_info->buf_addr_lb[0].stride_y;
    layer_config->frame_addr[VDP_FDR_VID_ADDR_2B].chm_str = frame_info->buf_addr_lb[0].stride_c;
    return;
}

hi_void xdp_ctrl_set_single_region_frame_cfg(hi_u32 layer_id,
                                             win_runtime_attr_s *attr,
                                             hi_drv_video_frame *frame_info,
                                             layer_hal_config_infor *hal_cfg_info)
{
    if (frame_info == HI_NULL) {
        xdp_ctrl_set_single_region_frame_dafult_cfg(layer_id, attr, hal_cfg_info);
        return;
    }

    if ((layer_id == LAYER_ID_VID3) &&
        ((hal_cfg_info->basic_config_info[layer_id].vid_video_rect.w != frame_info->width) ||
        (hal_cfg_info->basic_config_info[layer_id].vid_video_rect.h != frame_info->height))) {
        xdp_ctrl_set_single_region_frame_dafult_cfg(layer_id, attr, hal_cfg_info);
        return;
    }

    hal_cfg_info->basic_config_info[layer_id].frame_smmu_en = HI_TRUE;
    hal_cfg_info->basic_config_info[layer_id].frame_rate = DEFAULT_FRAME_RATE; /* 60000 is default rate */
    hal_cfg_info->basic_config_info[layer_id].frame_inerlace = HI_FALSE;
    hal_cfg_info->basic_config_info[layer_id].frame_field_mode = HI_DRV_FIELD_ALL;
    hal_cfg_info->basic_config_info[layer_id].frame_secure_en = frame_info->secure;
    hal_cfg_info->basic_config_info[layer_id].mute_en = HI_FALSE;
    hal_cfg_info->basic_config_info[layer_id].frame_rect.x = 0;
    hal_cfg_info->basic_config_info[layer_id].frame_rect.y = 0;
    hal_cfg_info->basic_config_info[layer_id].frame_rect.w = frame_info->width;
    hal_cfg_info->basic_config_info[layer_id].frame_rect.h = frame_info->height;
    hal_cfg_info->basic_config_info[layer_id].frame_bit_width = frame_info->bit_width;
    hal_cfg_info->basic_config_info[layer_id].frame_pix_format = frame_info->pixel_format;
    hal_cfg_info->basic_config_info[layer_id].frame_hdr_type = frame_info->hdr_type;
    hal_cfg_info->basic_config_info[layer_id].frame_color_space = frame_info->color_space;
    hal_cfg_info->basic_config_info[layer_id].frame_3d_type = frame_info->video_3d_type;
    hal_cfg_info->basic_config_info[layer_id].cmp_mode = frame_info->cmp_info.cmp_mode;
    hal_cfg_info->basic_config_info[layer_id].data_fmt = frame_info->cmp_info.data_fmt;
    hal_cfg_info->basic_config_info[layer_id].ycmp_rate = frame_info->cmp_info.ycmp_rate;
    hal_cfg_info->basic_config_info[layer_id].ccmp_rate = frame_info->cmp_info.ccmp_rate;
    if (layer_id == LAYER_ID_VID0) {
        hal_cfg_info->basic_config_info[layer_id].crop_rect.x = attr->crop_rect.x;
        hal_cfg_info->basic_config_info[layer_id].crop_rect.y = attr->crop_rect.y;
        hal_cfg_info->basic_config_info[layer_id].crop_rect.w = attr->crop_rect.width;
        hal_cfg_info->basic_config_info[layer_id].crop_rect.h = attr->crop_rect.height;
    } else if (layer_id == LAYER_ID_VID3) {
        hal_cfg_info->basic_config_info[layer_id].crop_rect.x = 0;
        hal_cfg_info->basic_config_info[layer_id].crop_rect.y = 0;
        hal_cfg_info->basic_config_info[layer_id].crop_rect.w = frame_info->width;
        hal_cfg_info->basic_config_info[layer_id].crop_rect.h = frame_info->height;
    }

    xdp_ctrl_set_single_region_frame_addr(frame_info, &hal_cfg_info->basic_config_info[layer_id]);

    return;
}

hi_void xdp_ctrl_set_layer_single_region_attr_cfg(hi_u32 layer_id,
                                                  win_policy_result_info *win_tmp_cfg,
                                                  layer_hal_config_infor *hal_cfg_info)

{
    hal_cfg_info->basic_config_info[layer_id].region_num = win_tmp_cfg->layermapping.layer_region_id;
    hal_cfg_info->basic_config_info[layer_id].enable = win_tmp_cfg->attr.win_enable;


    if (layer_id == LAYER_ID_VID3) {
        /* v3 letterbox in vpss */
        hal_cfg_info->basic_config_info[layer_id].vid_video_rect.x = win_tmp_cfg->attr.disp_rect.x;
        hal_cfg_info->basic_config_info[layer_id].vid_video_rect.y = win_tmp_cfg->attr.disp_rect.y;
        hal_cfg_info->basic_config_info[layer_id].vid_video_rect.w = win_tmp_cfg->attr.disp_rect.width;
        hal_cfg_info->basic_config_info[layer_id].vid_video_rect.h = win_tmp_cfg->attr.disp_rect.height;
    } else {
        hal_cfg_info->basic_config_info[layer_id].vid_video_rect.x = win_tmp_cfg->attr.video_rect.x;
        hal_cfg_info->basic_config_info[layer_id].vid_video_rect.y = win_tmp_cfg->attr.video_rect.y;
        hal_cfg_info->basic_config_info[layer_id].vid_video_rect.w = win_tmp_cfg->attr.video_rect.width;
        hal_cfg_info->basic_config_info[layer_id].vid_video_rect.h = win_tmp_cfg->attr.video_rect.height;
    }

    hal_cfg_info->basic_config_info[layer_id].vid_disp_rect.x = win_tmp_cfg->attr.disp_rect.x;
    hal_cfg_info->basic_config_info[layer_id].vid_disp_rect.y = win_tmp_cfg->attr.disp_rect.y;
    hal_cfg_info->basic_config_info[layer_id].vid_disp_rect.w = win_tmp_cfg->attr.disp_rect.width;
    hal_cfg_info->basic_config_info[layer_id].vid_disp_rect.h = win_tmp_cfg->attr.disp_rect.height;
    xdp_ctrl_set_single_region_frame_cfg(layer_id, &win_tmp_cfg->attr, win_tmp_cfg->frame_info, hal_cfg_info);
    return;
}
hi_void xdp_ctrl_set_layer_single_region_attr_default_cfg(hi_u32 layer_id,
                                                          hi_disp_display_info *disp_info,
                                                          layer_hal_config_infor *hal_cfg_info)
{
    hal_cfg_info->basic_config_info[layer_id].vp_rect.x = disp_info->fmt_resolution.x;
    hal_cfg_info->basic_config_info[layer_id].vp_rect.y = disp_info->fmt_resolution.y;
    hal_cfg_info->basic_config_info[layer_id].vp_rect.w = disp_info->fmt_resolution.width;
    hal_cfg_info->basic_config_info[layer_id].vp_rect.h = disp_info->fmt_resolution.height;
    hal_cfg_info->basic_config_info[layer_id].crop_rect = hal_cfg_info->basic_config_info[layer_id].vp_rect;
    hal_cfg_info->basic_config_info[layer_id].vid_video_rect = hal_cfg_info->basic_config_info[layer_id].vp_rect;
    hal_cfg_info->basic_config_info[layer_id].vid_disp_rect = hal_cfg_info->basic_config_info[layer_id].vp_rect;
    hal_cfg_info->basic_config_info[layer_id].enable = HI_FALSE;
    return;
}

hi_void xdp_ctrl_set_layer_single_region_cfg(hi_drv_display disp_chn,
                                             hi_u32 layer_id,
                                             xdp_ctrl_info *xdp_ctrl,
                                             hi_disp_display_info *disp_info,
                                             layer_hal_config_infor *hal_cfg_info)
{
    hi_u32 i = 0 ;
    win_policy_result_info *win_tmp_cfg = HI_NULL;
    if ((layer_id != LAYER_ID_VID0) && (layer_id != LAYER_ID_VID3)) {
        WIN_ERROR("layer id err! %d\n", layer_id);
        return;
    }
    memset_s(&hal_cfg_info->basic_config_info[layer_id], sizeof(hal_cfg_info->basic_config_info[layer_id]),
             0, sizeof(hal_cfg_info->basic_config_info[layer_id]));

    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        if (xdp_ctrl->win_cfg.expected_info[i].layermapping.layer_id == layer_id) {
            win_tmp_cfg = &xdp_ctrl->win_cfg.expected_info[i];
            break;
        }
    }
    hal_cfg_info->basic_config_info[layer_id].multi_region_enable = HI_FALSE;
    hal_cfg_info->basic_config_info[layer_id].disp_chn = disp_chn;
    hal_cfg_info->basic_config_info[layer_id].vp_rect.x = disp_info->fmt_resolution.x;
    hal_cfg_info->basic_config_info[layer_id].vp_rect.y = disp_info->fmt_resolution.y;
    hal_cfg_info->basic_config_info[layer_id].vp_rect.w = disp_info->fmt_resolution.width;
    hal_cfg_info->basic_config_info[layer_id].vp_rect.h = disp_info->fmt_resolution.height;
    hal_cfg_info->basic_config_info[layer_id].out_color_space = disp_info->color_space;
    hal_cfg_info->basic_config_info[layer_id].out_3d_mode = disp_info->stereo;
    hal_cfg_info->basic_config_info[layer_id].out_hdr_type = disp_info->disp_type;
    hal_cfg_info->basic_config_info[layer_id].vid_alpha = disp_info->alpha;
    hal_cfg_info->basic_config_info[layer_id].vp_alpha = disp_info->alpha;
    hal_cfg_info->basic_config_info[layer_id].out_disp_rate = disp_info->refresh_rate;

    if (win_tmp_cfg != HI_NULL) {
        xdp_ctrl_set_layer_single_region_attr_cfg(layer_id, win_tmp_cfg, hal_cfg_info);
    } else {
        xdp_ctrl_set_layer_single_region_attr_default_cfg(layer_id, disp_info, hal_cfg_info);
    }
    return;
}

hi_void xdp_ctrl_set_layer_multi_region_common_cfg(hi_u32 layer_id,
                                                   xdp_ctrl_info *xdp_ctrl,
                                                   hi_disp_display_info *disp_info,
                                                   layer_hal_config_infor *hal_cfg_info)
{
    /* others */
    hal_cfg_info->basic_config_info[layer_id].frame_smmu_en = HI_TRUE;
    hal_cfg_info->basic_config_info[layer_id].frame_inerlace = HI_FALSE;
    hal_cfg_info->basic_config_info[layer_id].frame_field_mode = HI_DRV_FIELD_ALL;
    hal_cfg_info->basic_config_info[layer_id].frame_hdr_type = HI_DRV_HDR_TYPE_SDR;
    hal_cfg_info->basic_config_info[layer_id].frame_3d_type = HI_DRV_3D_NONE;
    hal_cfg_info->basic_config_info[layer_id].multi_region_enable = HI_TRUE;
    hal_cfg_info->basic_config_info[layer_id].region_num = 0;
    hal_cfg_info->basic_config_info[layer_id].out_inerlace = HI_FALSE;

    hal_cfg_info->basic_config_info[layer_id].frame_smmu_en = HI_TRUE;
    hal_cfg_info->basic_config_info[layer_id].frame_rate = DEFAULT_FRAME_RATE; /* 60000 is default rate */
    hal_cfg_info->basic_config_info[layer_id].frame_inerlace = HI_FALSE;
    hal_cfg_info->basic_config_info[layer_id].frame_field_mode = HI_DRV_FIELD_ALL;

    hal_cfg_info->basic_config_info[layer_id].frame_rect.h = disp_info->fmt_resolution.height;
    hal_cfg_info->basic_config_info[layer_id].frame_rect.w = disp_info->fmt_resolution.width;

    hal_cfg_info->basic_config_info[layer_id].frame_bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
    hal_cfg_info->basic_config_info[layer_id].frame_pix_format = HI_DRV_PIXEL_FMT_NV21;
    hal_cfg_info->basic_config_info[layer_id].data_fmt = HI_DRV_DATA_FMT_LINER;
    hal_cfg_info->basic_config_info[layer_id].cmp_mode = HI_DRV_COMPRESS_MODE_OFF;
    hal_cfg_info->basic_config_info[layer_id].ycmp_rate = 0;
    hal_cfg_info->basic_config_info[layer_id].ccmp_rate = 0;
    hal_cfg_info->basic_config_info[layer_id].out_disp_rate = disp_info->refresh_rate;

    hal_cfg_info->basic_config_info[layer_id].vp_rect.x = disp_info->fmt_resolution.x;
    hal_cfg_info->basic_config_info[layer_id].vp_rect.y = disp_info->fmt_resolution.y;
    hal_cfg_info->basic_config_info[layer_id].vp_rect.w = disp_info->fmt_resolution.width;
    hal_cfg_info->basic_config_info[layer_id].vp_rect.h = disp_info->fmt_resolution.height;
    hal_cfg_info->basic_config_info[layer_id].crop_rect = hal_cfg_info->basic_config_info[layer_id].vp_rect;
    hal_cfg_info->basic_config_info[layer_id].vid_video_rect = hal_cfg_info->basic_config_info[layer_id].vp_rect;
    hal_cfg_info->basic_config_info[layer_id].vid_disp_rect = hal_cfg_info->basic_config_info[layer_id].vp_rect;

    hal_cfg_info->basic_config_info[layer_id].out_color_space = disp_info->color_space;
    hal_cfg_info->basic_config_info[layer_id].out_3d_mode = disp_info->stereo;
    hal_cfg_info->basic_config_info[layer_id].out_hdr_type = disp_info->disp_type;

    hal_cfg_info->basic_config_info[layer_id].vid_alpha = disp_info->alpha;
    hal_cfg_info->basic_config_info[layer_id].vp_alpha =  disp_info->alpha;
    return;
}

hi_void xdp_ctrl_set_layer_multi_region_cfg(hi_drv_display disp_chn,
                                            hi_u32 layer_id,
                                            xdp_ctrl_info *xdp_ctrl,
                                            hi_disp_display_info *disp_info,
                                            layer_hal_config_infor *hal_cfg_info)
{

    hi_u32 i = 0 ;
    hi_bool layer_enable = HI_FALSE;
    hi_bool layer_secure = HI_FALSE;
    win_policy_result_info *win_tmp_cfg = HI_NULL;

    if ((layer_id != LAYER_ID_VID0)
        &&  (layer_id != LAYER_ID_VID1)
        &&  (layer_id != LAYER_ID_VID3)) {
        WIN_ERROR("layer id err! %d\n", layer_id);
        return;
    }

    memset_s(&hal_cfg_info->basic_config_info[layer_id],
             sizeof(hal_cfg_info->basic_config_info[layer_id]),
             0,
             sizeof(hal_cfg_info->basic_config_info[layer_id]));

    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        if (xdp_ctrl->win_cfg.expected_info[i].layermapping.layer_id == layer_id) {
            win_tmp_cfg = &xdp_ctrl->win_cfg.expected_info[i];
            xdp_ctrl_set_multi_region_cfg_frame(layer_id,
                                                win_tmp_cfg->layermapping.layer_region_id,
                                                win_tmp_cfg->frame_info,
                                                &win_tmp_cfg->attr,
                                                &hal_cfg_info->basic_config_info[layer_id]);

            if (win_tmp_cfg->frame_info != HI_NULL) {
                layer_secure |= win_tmp_cfg->frame_info->secure;
            }
        }
    }

    for (i = 0; i < FDR_REGION_NUM_MAX; i++) {
        layer_enable |= hal_cfg_info->basic_config_info[layer_id].region[i].enable;
    }

    xdp_ctrl_set_layer_multi_region_common_cfg(layer_id, xdp_ctrl, disp_info, hal_cfg_info);

    hal_cfg_info->basic_config_info[layer_id].frame_secure_en= layer_secure;
    hal_cfg_info->basic_config_info[layer_id].enable = layer_enable;
    hal_cfg_info->basic_config_info[layer_id].disp_chn = disp_chn;

    return;
}


static hi_void xdp_ctrl_generate_hal_cfg(hi_drv_display disp_chn,
                                         xdp_ctrl_info *xdp_ctrl,
                                         hi_disp_display_info *disp_info,
                                         layer_hal_config_infor *hal_cfg_info)

{
    if (disp_chn == HI_DRV_DISPLAY_1) {
        xdp_ctrl_set_layer_single_region_cfg(disp_chn, LAYER_ID_VID3, xdp_ctrl, disp_info, hal_cfg_info);
        return;
    } else if (disp_chn == HI_DRV_DISPLAY_0) {
        xdp_ctrl_set_layer_single_region_cfg(disp_chn, LAYER_ID_VID0, xdp_ctrl, disp_info, hal_cfg_info);
        xdp_ctrl_set_layer_multi_region_cfg(disp_chn, LAYER_ID_VID1, xdp_ctrl, disp_info, hal_cfg_info);
    }

    return;
}

static hi_void xdp_ctrl_config_hal(hi_drv_display disp_chn, hi_u32 layer_order)
{
    if (disp_chn == HI_DRV_DISPLAY_1) {
        hal_layer_cfg_hisi_process(LAYER_ID_VID3, &g_layer_hal_conf_info.basic_config_info[LAYER_ID_VID3],
                                   &g_layer_hal_conf_info.hdr_config_info[LAYER_ID_VID3]);
        return;
    }

    hal_layer_cfg_hisi_process(LAYER_ID_VID0,
                               &g_layer_hal_conf_info.basic_config_info[LAYER_ID_VID0],
                               &g_layer_hal_conf_info.hdr_config_info[LAYER_ID_VID0]);

    hal_layer_cfg_hisi_process(LAYER_ID_VID1,
                               &g_layer_hal_conf_info.basic_config_info[LAYER_ID_VID1],
                               &g_layer_hal_conf_info.hdr_config_info[LAYER_ID_VID1]);

    hal_layer_set_zorder(LAYER_ID_VID0, layer_order);

    return;
}
static hi_void xdpctrl_get_default_hdr10_info_frame(hi_drv_hdr_metadata *hdr_info)
{
    memset_s(hdr_info, sizeof(hdr_info), 0, sizeof(hdr_info));
    return;
}
static hi_void xdpctrl_get_default_hlg_info_frame(hi_drv_hdr_metadata *hdr_info)
{
    memset_s(hdr_info, sizeof(hdr_info), 0, sizeof(hdr_info));
    return;
}

#define PACKAGE_LENGTH 32
#define PACKAGE_NUM 4
#define HDMI_AVI_DRM_INFO  (PACKAGE_LENGTH * PACKAGE_NUM)
#define CHANGE_DELAY_TIMES  8
#define DEFAULT_ASPECT_RATIO_W_16  16
#define DEFAULT_ASPECT_RATIO_H_9  9
#define DEFAULT_ASPECT_RATIO_W_4  4
#define DEFAULT_ASPECT_RATIO_H_3  3
#define CHECK_SUM_PARAM  256

hi_u32 g_delaytime = 0;

static hi_void xdp_ctrl_get_avi_infoframe(hi_disp_display_info *disp_info,
                                          hi_drv_disp_intf_info *intf_info,
                                          hi_drv_disp_avi_infoframe *frame)
{
    frame->vic = intf_info->disp_timing.static_timing.vic_num;
    frame->pixel_repeat = 1;
    frame->pix_format = intf_info->out_info.pixel_format;
    frame->scan_mode = HI_DRV_DISP_SCAN_MODE_NONE;
    frame->color_space = intf_info->out_info.color_space;
    frame->nups = HI_DRV_DISP_NUPS_UNKNOWN;
    frame->content_type = HI_DRV_DISP_CONTENT_TYPE_NONE;

    frame->picture_aspect.aspect_ratio_w = intf_info->disp_timing.static_timing.aspect_ratio.aspect_ratio_w;
    frame->picture_aspect.aspect_ratio_h = intf_info->disp_timing.static_timing.aspect_ratio.aspect_ratio_h;

    if ((disp_info->aspect_ratio.aspect_ratio_w == DEFAULT_ASPECT_RATIO_W_16) &&
        (disp_info->aspect_ratio.aspect_ratio_h == DEFAULT_ASPECT_RATIO_H_9)) {
        frame->active_aspect = HI_DRV_AFD_TYPE_16_9;
    } else if ((disp_info->aspect_ratio.aspect_ratio_w == DEFAULT_ASPECT_RATIO_W_4) &&
               (disp_info->aspect_ratio.aspect_ratio_h == DEFAULT_ASPECT_RATIO_H_3)) {
        frame->active_aspect = HI_DRV_AFD_TYPE_4_3;
    } else {
        frame->active_aspect = HI_DRV_AFD_TYPE_UNKNOW;
    }

    frame->top_bar = 0;
    frame->bottom_bar = 0;
    frame->left_bar = 0;
    frame->right_bar = 0;
    frame->disp_3d_mode = disp_info->stereo;
    return;
}

static hi_bool xdp_ctrl_get_avi_pack(disp_intf_func *hdmi_ext_func,
                                     hi_drv_disp_avi_infoframe *avi_frame,
                                     hi_u8 *buffer,
                                     hi_u32 size)
{
    hi_u8 i;
    hi_u8  check_sum = 0;
    hi_u8 *data_ptr = HI_NULL;
    data_ptr = &buffer[1];

    if (hdmi_ext_func == HI_NULL) {
        hi_err_disp("get hdmi func is err(%x)\n");
        return HI_FALSE;
    }

    if (hdmi_ext_func->intf_hdr10_infoframe_pack == HI_NULL) {
        hi_err_disp("get hdmi func is err(%x)\n");
        return HI_FALSE;
    }

    hdmi_ext_func->intf_avi_infoframe_pack(avi_frame, data_ptr, PACKAGE_LENGTH);
    for (i = 0; i < PACKAGE_LENGTH; i++) {
        check_sum += data_ptr[i];
    }
    buffer[0] = CHECK_SUM_PARAM - check_sum;

    return HI_TRUE;
}

static hi_bool xdp_ctrl_get_hdr10_pack(disp_intf_func *hdmi_ext_func,
                                       hi_drv_hdr_type hdr_type,
                                       hi_drv_hdr_metadata *hdr_info,
                                       hi_u8 *buffer,
                                       hi_u32 size)
{
    hi_u8 i;
    hi_drv_hdr_static_metadata hdr10_frame = {0};
    hi_u8  check_sum = 0;
    hi_u8 *data_ptr = HI_NULL;
    data_ptr = &buffer[1];
    if (hdmi_ext_func == HI_NULL) {
        hi_err_disp("get hdmi func is err(%x)\n");
        return HI_FALSE;
    }
    if (hdmi_ext_func->intf_hdr10_infoframe_pack == HI_NULL) {
        hi_err_disp("get hdmi func is err(%x)\n");
        return HI_FALSE;
    }

    if ((hdr_info != HI_NULL) || (g_delaytime > 0)) {

        if (hdr_info != HI_NULL) {
            g_delaytime = CHANGE_DELAY_TIMES;
            if (hdr_type == HI_DRV_HDR_TYPE_HLG) {
                hdmi_ext_func->intf_hlg_infoframe_pack(&hdr_info->hlg_info, data_ptr, PACKAGE_LENGTH);
            } else if (hdr_type == HI_DRV_HDR_TYPE_HDR10) {
            hdmi_ext_func->intf_hdr10_infoframe_pack(&hdr_info->hdr10_info, data_ptr, PACKAGE_LENGTH);
            }
        } else {
            memset(&hdr10_frame, 0, sizeof(hi_drv_hdr_static_metadata));
            hdmi_ext_func->intf_hdr10_infoframe_pack(&hdr10_frame, data_ptr, PACKAGE_LENGTH);
            g_delaytime--;
        }

        for (i = 0; i < PACKAGE_LENGTH; i++) {
            check_sum += data_ptr[i];
        }

        buffer[0] = CHECK_SUM_PARAM - check_sum;
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_void xdp_ctrl_update_hdmi_hdr_info(disp_intf_func *hdmi_ext_func,
                                             hi_drv_disp_intf_info *intf_info)
{
    hi_drv_disp_intf intf;
    hi_u32 mode;
    hi_s32 ret;

    if (hdmi_ext_func == HI_NULL) {
        hi_err_disp("get hdmi func is err(%x)\n");
        return ;
    }

    if ((hdmi_ext_func->intf_prepare == HI_NULL) ||
        (hdmi_ext_func->intf_config == HI_NULL)) {
        hi_err_disp("get hdmi func is err(%x)\n");
        return ;
    }

    intf.intf_type = HI_DRV_DISP_INTF_TYPE_HDMI;
    intf.un_intf.hdmi = HI_DRV_HDMI_ID_0;
    mode = MODE_STATIC_HDR;

    ret = hdmi_ext_func->intf_prepare(intf, mode, intf_info);
    if (ret != HI_SUCCESS) {
        hi_err_disp("intf_prepare hdmi(%d) mode (%d)err(%x).\n", intf.un_intf.hdmi, mode);
        return ;
    }
    ret = hdmi_ext_func->intf_config(intf, mode, intf_info);
    if (ret != HI_SUCCESS) {
        hi_err_disp("intf_prepare hdmi(%d) mode (%d)err(%x).\n", intf.un_intf.hdmi, mode);
        return ;
    }

    return ;
}

hi_void xdp_ctrl_send_infoframe(hi_drv_hdr_type hdr_type,
                                hi_drv_hdr_metadata *hdr_info,
                                hi_drv_disp_intf_info *intf_info,
                                hi_disp_display_info *disp_info)
{
    hal_hdmi_infoframe cfg;
    hi_u8 matadata[HDMI_AVI_DRM_INFO] = {0};
    disp_intf_func *hdmi_ext_func = HI_NULL;
    hi_drv_disp_avi_infoframe avi_frame = {0};

    if (g_hdmi_intf == HI_NULL) {
        hi_err_disp("no hdmi export func!\n");
        return;
    }

    hdmi_ext_func = (disp_intf_func *)(g_hdmi_intf->vo_data);
    cfg.data_len = 0;


    xdp_ctrl_get_avi_infoframe(disp_info, intf_info, &avi_frame);
    cfg.avi_enable  = xdp_ctrl_get_avi_pack(hdmi_ext_func, &avi_frame, &matadata[0], PACKAGE_LENGTH);
    if (cfg.avi_enable == HI_TRUE) {
        cfg.data_len = cfg.data_len + PACKAGE_LENGTH;
    }

    cfg.smd_enable = xdp_ctrl_get_hdr10_pack(hdmi_ext_func, hdr_type, hdr_info,
                                             &matadata[PACKAGE_LENGTH], PACKAGE_LENGTH);
    if (cfg.smd_enable == HI_TRUE) {
        cfg.data_len = cfg.data_len + PACKAGE_LENGTH;
    }

    if ((cfg.smd_enable == HI_TRUE) || (cfg.avi_enable == HI_TRUE)) {
        cfg.enable = HI_TRUE;
    } else {
        cfg.enable = HI_FALSE;
    }

    cfg.addr = (hi_u64)&matadata[0];
    hal_disp_send_infoframe(&cfg);

    xdp_ctrl_update_hdmi_hdr_info(hdmi_ext_func, intf_info);
    return;
}

static hi_void xdp_ctrl_config_hdmi_info_frame(xdp_ctrl_info *xdp_ctrl,
                                               hi_disp_display_info *disp_info)
{
    hi_drv_disp_intf_info intf_info;
    hi_drv_video_frame *master_frame_info = HI_NULL;

    drv_disp_get_hdmi_intf_info(HI_DRV_DISPLAY_0, &intf_info);
    xdp_ctrl->out_put_info.disp_output_hdr_type = disp_info->disp_type;
    xdp_ctrl->out_put_info.disp_output_csc_type = disp_info->color_space_mode;

    if (xdp_ctrl->win_cfg.master_win_index != INVALID_INDEX) {
        master_frame_info = xdp_ctrl->win_cfg.expected_info[xdp_ctrl->win_cfg.master_win_index].frame_info;
    }

    /* only support hdr10 mode */
    if (xdp_ctrl->out_put_info.disp_output_hdr_type == HI_DRV_DISP_TYPE_HDR10) {
        if (master_frame_info == HI_NULL) {
            xdpctrl_get_default_hdr10_info_frame(&xdp_ctrl->out_put_info.hdr_info);
        } else {
            if (master_frame_info->hdr_type == HI_DRV_HDR_TYPE_HDR10) {
                memcpy_s(&xdp_ctrl->out_put_info.hdr_info, sizeof(hi_drv_hdr_metadata),
                         &master_frame_info->hdr_info,
                         sizeof(hi_drv_hdr_metadata));
            } else {
                xdpctrl_get_default_hdr10_info_frame(&xdp_ctrl->out_put_info.hdr_info);
            }
        }

        xdp_ctrl_send_infoframe(HI_DRV_HDR_TYPE_HDR10, &xdp_ctrl->out_put_info.hdr_info, &intf_info, disp_info);
    } else if (xdp_ctrl->out_put_info.disp_output_hdr_type == HI_DRV_DISP_TYPE_HLG) {
        if (master_frame_info == HI_NULL) {
            xdpctrl_get_default_hlg_info_frame(&xdp_ctrl->out_put_info.hdr_info);
        } else {
            if (master_frame_info->hdr_type == HI_DRV_HDR_TYPE_HLG) {
                memcpy_s(&xdp_ctrl->out_put_info.hdr_info, sizeof(hi_drv_hdr_metadata), &master_frame_info->hdr_info,
                         sizeof(hi_drv_hdr_metadata));
            } else {
                xdpctrl_get_default_hlg_info_frame(&xdp_ctrl->out_put_info.hdr_info);
            }
        }
        xdp_ctrl_send_infoframe(HI_DRV_HDR_TYPE_HLG, &xdp_ctrl->out_put_info.hdr_info, &intf_info, disp_info);
    } else {
        xdp_ctrl_send_infoframe(HI_DRV_HDR_TYPE_SDR, HI_NULL, &intf_info, disp_info);
    }
    return;

}

#define HDR_POLICY_LEVEL_MAX  4
static hi_drv_disp_out_type g_hdr_policy_list[HI_DRV_HDR_TYPE_MAX + 1][HDR_POLICY_LEVEL_MAX] = {
    /* frame type : HI_DRV_HDR_TYPE_SDR */
    {HI_DRV_DISP_TYPE_NORMAL, HI_DRV_DISP_TYPE_HDR10, HI_DRV_DISP_TYPE_HLG, HI_DRV_DISP_TYPE_BUTT},
    /* frame type : HI_DRV_HDR_TYPE_HDR10 */
    {HI_DRV_DISP_TYPE_HDR10, HI_DRV_DISP_TYPE_HLG, HI_DRV_DISP_TYPE_NORMAL, HI_DRV_DISP_TYPE_BUTT},
    /* frame type : HI_DRV_HDR_TYPE_HLG */
    {HI_DRV_DISP_TYPE_HLG, HI_DRV_DISP_TYPE_HDR10, HI_DRV_DISP_TYPE_NORMAL, HI_DRV_DISP_TYPE_BUTT},
    /* frame type : HI_DRV_HDR_TYPE_CUVA */
    {HI_DRV_DISP_TYPE_HDR10, HI_DRV_DISP_TYPE_HLG, HI_DRV_DISP_TYPE_NORMAL, HI_DRV_DISP_TYPE_BUTT},
    /* frame type : HI_DRV_HDR_TYPE_JTP_SL_HDR */
    {HI_DRV_DISP_TYPE_TECHNICOLOR, HI_DRV_DISP_TYPE_HDR10, HI_DRV_DISP_TYPE_HLG, HI_DRV_DISP_TYPE_BUTT},
    /* frame type : HI_DRV_HDR_TYPE_HDR10PLUS */
    {HI_DRV_DISP_TYPE_HDR10PLUS, HI_DRV_DISP_TYPE_HDR10, HI_DRV_DISP_TYPE_HLG, HI_DRV_DISP_TYPE_BUTT},
    /* frame type : HI_DRV_HDR_TYPE_DOLBYVISION */
    {HI_DRV_DISP_TYPE_DOLBY, HI_DRV_DISP_TYPE_DOLBY_LL, HI_DRV_DISP_TYPE_HDR10, HI_DRV_DISP_TYPE_BUTT},
    /* frame type : HI_DRV_HDR_TYPE_MAX */
    {HI_DRV_DISP_TYPE_BUTT, HI_DRV_DISP_TYPE_BUTT, HI_DRV_DISP_TYPE_BUTT, HI_DRV_DISP_TYPE_BUTT}
};

static hi_bool xdp_ctrl_sink_cap_match(hi_drv_disp_out_type disp_out_type,
                                       hi_drv_disp_sink_capability  *sink_cap)
{
    hi_bool match = HI_FALSE;

    switch (disp_out_type) {
        case HI_DRV_DISP_TYPE_NORMAL:
            match = (sink_cap->support_sdr == HI_TRUE) ? HI_TRUE : HI_FALSE;
            break;
        case HI_DRV_DISP_TYPE_HDR10:
            match = (sink_cap->support_hdr10 == HI_TRUE) ? HI_TRUE : HI_FALSE;
            break;
        case HI_DRV_DISP_TYPE_HDR10PLUS:
            match = (sink_cap->support_hdr10plus == HI_TRUE) ? HI_TRUE : HI_FALSE;
            break;
        case HI_DRV_DISP_TYPE_HLG:
            match = (sink_cap->support_hlg == HI_TRUE) ? HI_TRUE : HI_FALSE;
            break;
        case HI_DRV_DISP_TYPE_DOLBY:
            match = (sink_cap->support_dolby_vision == HI_TRUE) ? HI_TRUE : HI_FALSE;
            break;
        case HI_DRV_DISP_TYPE_DOLBY_LL:
            match = (sink_cap->support_dolby_low_latency == HI_TRUE) ? HI_TRUE : HI_FALSE;
            break;

        default:
            WIN_ERROR("Unsupport output type under Auto mode!outType->[%d]\n", disp_out_type);
            break;
    }

    return match;
}

static hi_drv_disp_output_color_space disp_transform_color_descript_to_mode(hi_drv_color_descript *color_descript)
{
    hi_drv_disp_output_color_space  color_space_mode;
    switch (color_descript->color_primary) {
        case HI_DRV_COLOR_PRIMARY_BT2020:
            color_space_mode = HI_DRV_DISP_COLOR_SPACE_BT2020;
            break;
        case HI_DRV_COLOR_PRIMARY_BT601_525:
        case HI_DRV_COLOR_PRIMARY_BT601_625:
            color_space_mode = HI_DRV_DISP_COLOR_SPACE_BT601;
            break;

        default:
            color_space_mode = HI_DRV_DISP_COLOR_SPACE_BT709;
            break;
    }
    return color_space_mode;
}

static hi_void xdp_ctrl_match_content_process(hi_drv_display disp,
                                              hi_drv_video_frame *frame_info,
                                              hi_drv_disp_sink_capability *sink_cap,
                                              hi_drv_disp_out_type *drv_disp_type,
                                              hi_drv_disp_output_color_space *color_space_mode)
{
    hi_u32 i;
    hi_drv_disp_out_type tmp_drv_disp_type;

    *drv_disp_type = HI_DRV_DISP_TYPE_NORMAL;
    *color_space_mode = HI_DRV_DISP_COLOR_SPACE_BT709;

    if ((disp != HI_DRV_DISPLAY_0) || (frame_info == HI_NULL)) {
        return;
    }

    for (i = 0; i < HDR_POLICY_LEVEL_MAX ; i++) {
        tmp_drv_disp_type = g_hdr_policy_list[frame_info->hdr_type][i];
        COMMON_DEBUG(HDR_INFOR, "match disp%d: src (%d)\n", disp, tmp_drv_disp_type);
        if (xdp_ctrl_sink_cap_match(tmp_drv_disp_type, sink_cap) == HI_TRUE) {
            *drv_disp_type = tmp_drv_disp_type;
            if (tmp_drv_disp_type == HI_DRV_DISP_TYPE_NORMAL) {
                *color_space_mode = disp_transform_color_descript_to_mode(&frame_info->color_space);
            } else {
                *color_space_mode = HI_DRV_DISP_COLOR_SPACE_BT2020;
            }
            break;
        }
    }

    return;
}

static hi_void xdp_ctrl_update_disp_info(hi_drv_display disp,
                                         xdp_ctrl_info *xdp_ctrl)
{
    hi_drv_disp_out_type drv_disp_type = HI_DRV_DISP_TYPE_NORMAL;
    hi_drv_disp_output_color_space color_space_mode = HI_DRV_DISP_COLOR_SPACE_BT709;
    disp_setting setting = {0};
    hi_s32 ret;
    hi_bool master_win_full = HI_FALSE;
    hi_drv_video_frame *master_frame_info = HI_NULL;

    ret = drv_disp_get_display_setting(disp, &setting);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("get display setting failed.\n");
        return;
    }
    xdp_ctrl->sink_cap = setting.sink_capability;

    if (xdp_ctrl->win_cfg.master_win_index != INVALID_INDEX) {
        master_win_full = xdp_ctrl->win_cfg.expected_info[xdp_ctrl->win_cfg.master_win_index].attr.is_full_screen;
        master_frame_info = xdp_ctrl->win_cfg.expected_info[xdp_ctrl->win_cfg.master_win_index].frame_info;
    }

    if ((xdp_ctrl->win_cfg.activate_win_num == 1) &&
        (setting.match_mode == HI_DRV_DISP_MATCH_CONTENT_MODE_DEFAULT) &&
        (master_win_full == HI_TRUE)) {
        xdp_ctrl_match_content_process(disp, master_frame_info, &xdp_ctrl->sink_cap, &drv_disp_type, &color_space_mode);
    } else {
        drv_disp_type = setting.disp_type;
        color_space_mode = setting.color_space_mode;
    }

    ret = drv_disp_update_display_info(disp, drv_disp_type, color_space_mode);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("get display setting failed.\n");
    }
    return;
}



hi_s32 drv_win_get_expected_info(hi_handle win_handle,
                                 win_runtime_attr_s *expected_win_attr,
                                 win_layermapping_result *expect_win_layermapping);

static hi_void xdp_ctrl_update_expect_cfg(hi_handle win_handle,
                                          hi_u32 index,
                                          xdp_ctrl_info *xdp_ctrl,
                                          hi_drv_video_frame *frame_info)
{
    hi_s32 ret;

    ret = drv_win_get_expected_info(win_handle,
                                    &xdp_ctrl->win_cfg.expected_info[index].attr,
                                    &xdp_ctrl->win_cfg.expected_info[index].layermapping);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("update expect cfg failed.\n");
    }

    xdp_ctrl->win_cfg.expected_info[index].frame_info = frame_info;

    return;
}

static hi_void xdp_ctrl_get_zorder_result(xdp_ctrl_info *xdp_ctrl, hi_u32 *layer_order)
{
    hi_u32 i = 0;

    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        if (xdp_ctrl->win_cfg.expected_info[i].layermapping.layer_id == LAYER_ID_VID0) {
            *layer_order = xdp_ctrl->win_cfg.expected_info[i].layermapping.layer_order;
            break;
        }
    }

    return;
}

static hi_u32 xdp_ctrl_get_master_win_index(xdp_ctrl_info *xdp_ctrl)
{
    hi_u32 i = 0;

    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        if (xdp_ctrl->win_cfg.expected_info[i].layermapping.layer_id == LAYER_ID_VID0) {
            return i;
        }
    }

    return INVALID_INDEX;
}

hi_void isr_xdp_ctrl_post_display_config(hi_void *hDst,
                                         const hi_drv_disp_callback_info *pstInfo)
{
    hi_drv_video_frame *frame_info = HI_NULL ;
    hi_handle *win_handle = HI_NULL;
    hi_u32 activate_win_num ;
    hi_u32 i;
    win_framesync_result sync_result[WINDOW_MAX_NUMBER] = {0};
    hi_ulong lock_flags = 0;
    hi_bool win_enable = HI_TRUE;
    hi_drv_display disp;
    xdp_ctrl_info *xdp_ctrl = HI_NULL;
    hi_u32 layer_order = 0;

    disp = (hi_drv_display)hDst;
    winmanage_get_lock(&lock_flags);
    win_handle = winmanage_get_win_handle(disp);
    xdp_ctrl =  xdpctrl_get_ctrl_info(disp);

    /* according to framesync result,adjust the window's buf ptr. */
    activate_win_num = 0;
    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        /* according to framesync result,adjust the window's buf ptr. */
        if (win_handle[i] == 0) {
            continue;
        }
        (hi_void) drv_win_get_status(win_handle[i], &win_enable);
        if (win_enable == HI_TRUE) {
            activate_win_num++;
            frame_info = drv_win_update_cfg_frame(win_handle[i], sync_result[i]);
            drv_win_update_expected_attr(win_handle[i], frame_info);
        }
        xdp_ctrl_update_expect_cfg(win_handle[i], i, xdp_ctrl, frame_info);
    }
    xdp_ctrl->win_cfg.activate_win_num = activate_win_num;
    xdp_ctrl->win_cfg.master_win_index = xdp_ctrl_get_master_win_index(xdp_ctrl);

    xdp_ctrl_update_disp_info(disp, xdp_ctrl);
    xdp_ctrl_generate_hal_cfg(disp, xdp_ctrl, (hi_disp_display_info *)&pstInfo->disp_info, &g_layer_hal_conf_info);
    winmanage_rls_lock(&lock_flags);

    xdp_ctrl_get_zorder_result(xdp_ctrl, &layer_order);
    xdp_ctrl_config_hal(disp, layer_order);
    if (disp == HI_DRV_DISPLAY_0) {
        xdp_ctrl_config_hdmi_info_frame(xdp_ctrl, (hi_disp_display_info *)&pstInfo->disp_info);
    }
    return;
}

static hi_void isr_xdp_ctrl_post_release_frame(hi_void *hDst,
                                               const hi_drv_disp_callback_info *pstInfo)
{
    hi_handle *win_handle = hDst;
    hi_u32 i, win_num;
    hi_ulong lock_flags = 0;
    hi_bool win_enable = HI_TRUE;
    winmanage_get_lock(&lock_flags);

    /* win_framesync_result sync_result[]; */
    win_num = winmanage_get_win_num((hi_drv_display)hDst);
    win_handle = winmanage_get_win_handle((hi_drv_display)hDst);

    /* according to framesync result, adjust the window's buf ptr. */
    for (i = 0; i < win_num; i++) {
        /* according to framesync result,adjust the window's buf ptr. */
        if (win_handle[i] == 0) {
            continue;
        }

        (hi_void) drv_win_get_status(win_handle[i], &win_enable);
        if (win_enable == HI_TRUE) {
            drv_win_update_disp_frame(win_handle[i]);
        }
    }

    winmanage_rls_lock(&lock_flags);
    hal_layer_disp_complete_process((hi_drv_display)hDst);
    return;
}

hi_s32 drv_xdp_ctrl_init(hi_drv_display disp)
{
    hi_drv_disp_callback isr_callback_descp = { 0 };
    hi_s32 ret;

    if (disp >= HI_DRV_DISPLAY_2) {
        WIN_ERROR("unsupport channel callback disp%d.\n",disp);
        return HI_FAILURE;
    }

    if (g_xdp_ctrl_info[disp].init_cnt > 0) {
        return HI_SUCCESS;
    }

    ret = hi_drv_module_get_func(HI_ID_HDMITX, (hi_void **)&g_hdmi_intf);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get hdmi func is err(%x)\n", ret);
        return ret;
    }

    isr_callback_descp.hdst = (hi_void *)disp;
    isr_callback_descp.pf_disp_callback = isr_xdp_ctrl_post_display_config;
    isr_callback_descp.callback_prior = HI_DRV_DISP_CALLBACK_PRIORTY_HIGH;

    ret = disp_isr_reg_callback(disp, HI_DRV_DISP_C_INTPOS_0_PERCENT, &isr_callback_descp);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("register callback failed.\n");
        return ret;
    }

    isr_callback_descp.hdst = (hi_void *)disp;
    isr_callback_descp.pf_disp_callback = isr_xdp_ctrl_post_release_frame;
    isr_callback_descp.callback_prior = HI_DRV_DISP_CALLBACK_PRIORTY_HIGH;

    ret = disp_isr_reg_callback(disp, HI_DRV_DISP_C_INTPOS_100_PERCENT, &isr_callback_descp);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("register callback failed.\n");

        isr_callback_descp.hdst = (hi_void *)disp;
        isr_callback_descp.pf_disp_callback = isr_xdp_ctrl_post_display_config;
        isr_callback_descp.callback_prior = HI_DRV_DISP_CALLBACK_PRIORTY_HIGH;
        disp_isr_unreg_callback(disp, HI_DRV_DISP_C_INTPOS_0_PERCENT, &isr_callback_descp);
        return ret;
    }

    g_xdp_ctrl_info[disp].init_cnt++;
    return HI_SUCCESS;
}

hi_void drv_xdp_ctrl_deinit(hi_drv_display disp)
{
    hi_drv_disp_callback isr_callback_descp = { 0 };
    hi_s32 ret;

    if (g_xdp_ctrl_info[disp].init_cnt > 0) {
        g_xdp_ctrl_info[disp].init_cnt--;
        if (g_xdp_ctrl_info[disp].init_cnt > 0) {
            WIN_ERROR("refcnt not equals to zero.\n");
            return;
        }
    }

    isr_callback_descp.hdst = (hi_void *)disp;
    isr_callback_descp.pf_disp_callback = isr_xdp_ctrl_post_display_config;
    isr_callback_descp.callback_prior = HI_DRV_DISP_CALLBACK_PRIORTY_HIGH;
    ret = disp_isr_unreg_callback(disp, HI_DRV_DISP_C_INTPOS_0_PERCENT, &isr_callback_descp);
    if (ret) {
        WIN_ERROR("unreg callback failed.\n");
    }

    isr_callback_descp.hdst = (hi_void *)disp;
    isr_callback_descp.pf_disp_callback = isr_xdp_ctrl_post_release_frame;
    isr_callback_descp.callback_prior = HI_DRV_DISP_CALLBACK_PRIORTY_HIGH;
    ret = disp_isr_unreg_callback(disp, HI_DRV_DISP_C_INTPOS_100_PERCENT, &isr_callback_descp);
    if (ret) {
        WIN_ERROR("unreg callback failed.\n");
    }

    return;
}

hi_char *g_xdp_ctrl_output_hdr_type[HI_DRV_DISP_TYPE_BUTT + 1] = {
    "SDR",
    "HDR10",
    "HDR10PLUS",
    "HLG",
    "DOLBY",
    "DOLBY_LL",
    "SDR CERT",
    "HDR10 CERT",
    "TECHNICOLOR",
    "BUTT",
};
hi_char *g_xdp_ctrl_output_color_space[HI_DRV_DISP_COLOR_SPACE_BUTT + 1] = {
    "BT709", "BT601", "BT2020", "BUTT"
};

hi_void drv_xdp_ctrl_proc_info(struct seq_file *p, hi_drv_display disp)
{
    xdp_ctrl_info *xdp_ctrl = HI_NULL;
    hi_drv_hdr_static_metadata        *hdr10_info = HI_NULL;

    xdp_ctrl = xdpctrl_get_ctrl_info(disp);
    HI_PROC_PRINT(p, "%-40s:%-10d\n", "support_bt601", xdp_ctrl->sink_cap.support_bt601);
    HI_PROC_PRINT(p, "%-40s:%-10d\n", "support_bt709", xdp_ctrl->sink_cap.support_bt709);
    HI_PROC_PRINT(p, "%-40s:%-10d\n", "support_bt2020", xdp_ctrl->sink_cap.support_bt2020);
    HI_PROC_PRINT(p, "%-40s:%-10d\n", "support_sdr", xdp_ctrl->sink_cap.support_sdr);
    HI_PROC_PRINT(p, "%-40s:%-10d\n", "support_hdr10", xdp_ctrl->sink_cap.support_hdr10);
    HI_PROC_PRINT(p, "%-40s:%-10d\n", "support_hdr10plus", xdp_ctrl->sink_cap.support_hdr10plus);
    HI_PROC_PRINT(p, "%-40s:%-10d\n", "support_hlg", xdp_ctrl->sink_cap.support_hlg);
    HI_PROC_PRINT(p, "%-40s:%-10d\n", "support_dolby_vision", xdp_ctrl->sink_cap.support_dolby_vision);
    HI_PROC_PRINT(p, "%-40s:%-10d\n", "support_dolby_low_latency", xdp_ctrl->sink_cap.support_dolby_low_latency);

    hdr10_info = &xdp_ctrl->out_put_info.hdr_info.hdr10_info;

    HI_PROC_PRINT(p, "%-40s:%-40s\n", "output_hdr_type",
                  g_xdp_ctrl_output_hdr_type[xdp_ctrl->out_put_info.disp_output_hdr_type]);
    HI_PROC_PRINT(p, "%-40s:%-40s\n", "disp_output_csc_type",
                  g_xdp_ctrl_output_color_space[xdp_ctrl->out_put_info.disp_output_csc_type]);

    if (xdp_ctrl->out_put_info.disp_output_hdr_type != HI_DRV_DISP_TYPE_NORMAL) {
        HI_PROC_PRINT(p, "%-40s:%-10d\n", "primaries_x0", hdr10_info->mastering_info.display_primaries_x[0]);
        HI_PROC_PRINT(p, "%-40s:%-10d\n", "primaries_x1", hdr10_info->mastering_info.display_primaries_x[1]);
        HI_PROC_PRINT(p, "%-40s:%-10d\n", "primaries_x2",
                      hdr10_info->mastering_info.display_primaries_x[2]); /* 2 : param index */
        HI_PROC_PRINT(p, "%-40s:%-10d\n", "primaries_y0", hdr10_info->mastering_info.display_primaries_y[0]);
        HI_PROC_PRINT(p, "%-40s:%-10d\n", "primaries_y1", hdr10_info->mastering_info.display_primaries_y[1]);
        HI_PROC_PRINT(p, "%-40s:%-10d\n", "primaries_y2",
                      hdr10_info->mastering_info.display_primaries_y[2]); /* 2 : param index */
        HI_PROC_PRINT(p, "%-40s:%-10d\n", "white_point_x", hdr10_info->mastering_info.white_point_x);
        HI_PROC_PRINT(p, "%-40s:%-10d\n", "white_point_y", hdr10_info->mastering_info.white_point_y);
        HI_PROC_PRINT(p, "%-40s:%-10d\n", "max_display_mastering_luminance",
                      hdr10_info->mastering_info.max_display_mastering_luminance);
        HI_PROC_PRINT(p, "%-40s:%-10d\n", "min_display_mastering_luminance",
                      hdr10_info->mastering_info.min_display_mastering_luminance);
        HI_PROC_PRINT(p, "%-40s:%-10d\n", "max_pic_average_light_level",
                      hdr10_info->content_info.max_pic_average_light_level);
        HI_PROC_PRINT(p, "%-40s:%-10d\n", "max_content_light_level", hdr10_info->content_info.max_content_light_level);
    }

    return;
}
