/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#include "hi_drv_pq.h"
#include "vdp_chip_define.h"

#include "hal_layer_intf.h"
#include "hal_vdp_vid.h"
#include "hal_vdp_chip_config.h"

#ifdef VDP_SDK_PQ_SUPPORT
#include "vdp_ext_func.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static hi_u32 g_layer_pro[LAYER_ID_BUTT] = { 0, 1, 0, 0 };
static vdp_vid_info g_info[LAYER_ID_BUTT] = { 0 };
static vdp_vid_info g_save_info[LAYER_ID_BUTT] = { 0 };
static hi_bool g_update_pq_for_resume = HI_FALSE;

#ifdef VDP_SDK_PQ_SUPPORT

hi_void hal_type_hdr_to_pq(hi_drv_disp_out_type hdr_type, hi_drv_hdr_type *pq_hdr_type)
{
    switch (hdr_type) {
        case HI_DRV_DISP_TYPE_NORMAL:
            *pq_hdr_type = HI_DRV_HDR_TYPE_SDR;
            break;
        case HI_DRV_DISP_TYPE_HDR10:
            *pq_hdr_type = HI_DRV_HDR_TYPE_HDR10;
            break;
        case HI_DRV_DISP_TYPE_HLG:
            *pq_hdr_type = HI_DRV_HDR_TYPE_HLG;
            break;
        case HI_DRV_DISP_TYPE_DOLBY:
            *pq_hdr_type = HI_DRV_HDR_TYPE_DOLBYVISION;
            break;
        default:
            *pq_hdr_type = HI_DRV_HDR_TYPE_MAX;
            break;
    }
    return ;
}

static hi_void hal_layer_cfg_pq_process(hi_u32 layer, vdp_layer_info *layer_info, vdp_hdr_info *hdr_info)
{
    hi_drv_pq_vdp_info pq_vdp_info = { 0 };
    if ((layer != LAYER_ID_VID0) || (layer_info->enable == HI_FALSE)) {
        return; /* only layer 0 need pq process */
    }

    pq_vdp_info.layer_id = HI_DRV_PQ_VDP_LAYER_VID0;

    pq_vdp_info.frame_info.frame_width = layer_info->frame_rect.w;
    pq_vdp_info.frame_info.frame_height = layer_info->frame_rect.h;
    pq_vdp_info.frame_info.frame_rate = layer_info->frame_rate;
    pq_vdp_info.frame_info.bit_width = layer_info->frame_bit_width;

    pq_vdp_info.frame_info.frame_fmt_in = (layer_info->frame_inerlace == HI_FALSE) ?
        HI_DRV_PQ_FRM_FRAME : ((layer_info->frame_field_mode == HI_DRV_FIELD_TOP) ?
        HI_DRV_PQ_FRM_TOP_FIELD : HI_DRV_PQ_FRM_BOTTOM_FIELD);

    pq_vdp_info.frame_info.is_ogeny_mode = HI_FALSE;
    pq_vdp_info.frame_info.is_part_update = HI_TRUE;
    if (g_update_pq_for_resume == HI_TRUE) {
        pq_vdp_info.frame_info.is_part_update = HI_FALSE;
        g_update_pq_for_resume = HI_FALSE;
    }

    pq_vdp_info.frame_info.hdr_type_in = layer_info->frame_hdr_type;
    hal_type_hdr_to_pq(layer_info->out_hdr_type, &pq_vdp_info.frame_info.hdr_type_out);

    /* get fmt resolution */
    pq_vdp_info.channel_timing[HI_DRV_PQ_DISPLAY_1].fmt_rect.rect_x = layer_info->vp_rect.x;
    pq_vdp_info.channel_timing[HI_DRV_PQ_DISPLAY_1].fmt_rect.rect_y = layer_info->vp_rect.y;
    pq_vdp_info.channel_timing[HI_DRV_PQ_DISPLAY_1].fmt_rect.rect_w = layer_info->vp_rect.w;
    pq_vdp_info.channel_timing[HI_DRV_PQ_DISPLAY_1].fmt_rect.rect_h = layer_info->vp_rect.h;

    pq_vdp_info.channel_timing[HI_DRV_PQ_DISPLAY_1].video_rect.rect_x = layer_info->vid_video_rect.x;
    pq_vdp_info.channel_timing[HI_DRV_PQ_DISPLAY_1].video_rect.rect_y = layer_info->vid_video_rect.y;
    pq_vdp_info.channel_timing[HI_DRV_PQ_DISPLAY_1].video_rect.rect_w = layer_info->vid_video_rect.w;
    pq_vdp_info.channel_timing[HI_DRV_PQ_DISPLAY_1].video_rect.rect_h = layer_info->vid_video_rect.h;

    pq_vdp_info.channel_timing[HI_DRV_PQ_DISPLAY_1].refresh_rate = layer_info->out_disp_rate;
    pq_vdp_info.channel_timing[HI_DRV_PQ_DISPLAY_1].frame_fmt = (layer_info->out_inerlace == HI_FALSE) ?
                                                                HI_DRV_PQ_FRM_FRAME : HI_DRV_PQ_FRM_TOP_FIELD;
    pq_vdp_info.channel_timing[HI_DRV_PQ_DISPLAY_1].color_space_out = layer_info->out_color_space;
    hal_type_hdr_to_pq(layer_info->out_hdr_type, &pq_vdp_info.channel_timing[HI_DRV_PQ_DISPLAY_1].disp_hdr_type);

    vdp_ext_pq_update_vdp_alg(HI_DRV_PQ_DISPLAY_1, &pq_vdp_info);
}
#endif

vdp_layer_vid layer_get_id(hi_u32 layer)
{
    switch (layer) {
        case LAYER_ID_VID0:
            return VDP_LAYER_VID0;
        case LAYER_ID_VID1:
            return VDP_LAYER_VID1;
        case LAYER_ID_VID3:
            return VDP_LAYER_VID3;
        default:
            VDP_ERROR("set wrong layer id %d\n", layer);
            return VDP_LAYER_VID0;
    }
}

/* capacity */
layer_capacity *hal_layer_get_capacity(hi_u32 layer)
{
    return hal_layer_get_chip_capacity(layer);
}

/* disable layer close layer or region from window for kpi */
hi_void hal_layer_cfg_disable(hi_u32 layer, hi_u32 region_index)
{
    vdp_layer_vid layer_id = layer_get_id(layer);
    vdp_ip_fdr_disable_layer(layer_get_id(layer), region_index);
    vdp_vid_regup();
    /* when close layer , need clean the save info , for update flag cmp in isr */
    memset(&g_save_info[layer_id].vid_cfg, 0x0, sizeof(vdp_vid_cfg));
    memset(&g_save_info[layer_id].fdr_info, 0x0, sizeof(vdp_fdr_info));
    return;
}

hi_void hal_layer_clean_flag_for_suspend(hi_void)
{
    /* clear all vid cfg , make sure resume first isr cfg all vid info */
    memset(g_save_info, 0x0, sizeof(g_save_info));
    g_update_pq_for_resume = HI_TRUE;
}

hi_void hal_layer_disp_complete_process(hi_drv_display disp_chn)
{
    if (disp_chn == HI_DRV_DISPLAY_0) {
        vdp_ext_pq_update_vdp_stt_info();
    }
    return;
}

/* cfg layer */
hi_void hal_layer_cfg_hisi_process(hi_u32 layer, vdp_layer_info *layer_info, vdp_hdr_info *hdr_info)
{
    vdp_vid_update_info update_info;
    vdp_layer_vid layer_id = layer_get_id(layer);
    hi_drv_pq_xdr_frame_info xdr_frame_info = { 0 };

    memset(&g_info[layer_id], 0x0, sizeof(vdp_vid_info));
    memset(&update_info, 0x0, sizeof(vdp_vid_update_info));

#ifdef VDP_SDK_PQ_SUPPORT
    hal_layer_cfg_pq_process(layer, layer_info, hdr_info);
#endif

    vdp_vid_transfer(layer_id, layer_info, &g_info[layer_id]);
    vdp_vid_assert(layer_id, layer_info, &g_info[layer_id]);
    vdp_vid_update(&g_info[layer_id], &g_save_info[layer_id], &update_info);

    vdp_vid_set_layer(layer_id, &g_info[layer_id], &update_info);

    if ((layer != LAYER_ID_VID0) || (layer_info->enable == HI_FALSE)) {
        vdp_vid_regup();
        return; /* only layer 0 need hdr process */
    }

    /* pq hdr */
    xdr_frame_info.window_num = 1;
    xdr_frame_info.src_hdr_type = layer_info->frame_hdr_type;

    if (layer_info->out_hdr_type == HI_DRV_DISP_TYPE_HDR10) {
        xdr_frame_info.disp_hdr_type = HI_DRV_HDR_TYPE_HDR10;
    } else if (layer_info->out_hdr_type == HI_DRV_DISP_TYPE_HLG) {
        xdr_frame_info.disp_hdr_type = HI_DRV_HDR_TYPE_HLG;
    } else {
        xdr_frame_info.disp_hdr_type = HI_DRV_HDR_TYPE_SDR;
    }

    if ((xdr_frame_info.src_hdr_type ==  HI_DRV_HDR_TYPE_SDR)
        && (xdr_frame_info.disp_hdr_type ==  HI_DRV_HDR_TYPE_SDR)) {
        xdr_frame_info.color_space_in = g_info[layer_id].csc_info.i_mode;
        xdr_frame_info.color_space_out = g_info[layer_id].csc_info.o_mode;
    } else {
        xdr_frame_info.color_space_in = g_info[layer_id].csc_info.i_mode;
        xdr_frame_info.color_space_out = g_info[layer_id].csc_info.o_mode;
    }

    if (layer == LAYER_ID_VID0) {
        hi_drv_pq_set_vdp_hdr_cfg(HI_DRV_PQ_XDR_LAYER_ID_0, &xdr_frame_info);
    }

    vdp_vid_regup();

    return;
}

hi_void hal_layer_cfg_dolby_process(hi_u32 layer, vdp_layer_info *layer_info, vdp_dolby_info *hdr_info)
{
    vdp_vid_update_info update_info;
    vdp_layer_vid layer_id = layer_get_id(layer);

    memset(&g_info[layer_id], 0x0, sizeof(vdp_vid_info));
    memset(&update_info, 0x0, sizeof(vdp_vid_update_info));

    vdp_vid_transfer(layer_id, layer_info, &g_info[layer_id]);
    vdp_vid_assert(layer_id, layer_info, &g_info[layer_id]);
    vdp_vid_update(&g_info[layer_id], &g_save_info[layer_id], &update_info);

    vdp_vid_set_layer(layer_id, &g_info[layer_id], &update_info);

    vdp_vid_regup();

    return;
}

hi_s32 hal_layer_set_zorder(hi_u32 layer, hi_u32 zorder)
{
    /* v3 always in pro 0 */
    if (layer == LAYER_ID_VID3) {
        return HI_SUCCESS;
    }

    /* v0 v1 must be in pro 0 or 1 */
    if (zorder > 1) {
        VDP_ERROR("set wrong layer id = %d zoder %d\n", layer, zorder);
        return HI_FAILURE;
    }

    if ((layer == LAYER_ID_VID0) || (layer == LAYER_ID_VID1)) {
        layer_vid need_update_layer = (layer == LAYER_ID_VID0) ? LAYER_ID_VID1 : LAYER_ID_VID0;
        if (g_layer_pro[layer] != zorder) {
            g_layer_pro[layer] = zorder;
            g_layer_pro[need_update_layer] = 1 - zorder;
            vdp_ip_dispchn_set_cbm_mix(VDP_DISPCHN_CBM_MIXV0, VDP_DISPCHN_MIX0_VID0, g_layer_pro[LAYER_ID_VID0]);
            vdp_ip_dispchn_set_cbm_mix(VDP_DISPCHN_CBM_MIXV0, VDP_DISPCHN_MIX0_VID1, g_layer_pro[LAYER_ID_VID1]);
        }
        vdp_vid_regup();
        return HI_SUCCESS;
    }

    VDP_ERROR("set wrong layer id = %d\n", layer);
    return HI_FAILURE;
}

hi_s32 hal_layer_get_zorder(hi_u32 layer, hi_u32 *zorder)
{
    if ((layer == LAYER_ID_VID0) || (layer == LAYER_ID_VID1)
        || (layer == LAYER_ID_VID3)) {
        *zorder = g_layer_pro[layer];
        return HI_SUCCESS;
    }

    VDP_ERROR("set wrong layer id = %d\n", layer);
    return HI_FAILURE;
}

/* state */
hi_void hal_layer_get_water_mark_state(hi_u32 layer, hi_bool *water_mark)
{
    *water_mark = vdp_vid_get_mark_status(layer_get_id(layer));
}

/* debug */
hi_void hal_layer_debug_get_logic_info(hi_u32 layer, hi_u32 region, char *logic_buf)
{
    return;
}

hi_void hal_layer_debug_pattern(hi_u32 layer, vdp_fdr_testpattern *debug)
{
    vdp_ip_fdr_testpattern(layer_get_id(layer), debug);
    vdp_vid_regup();
    return;
}

hi_void hal_layer_debug_get_checksum(hi_u32 layer, hi_u32 *checksum)
{
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

