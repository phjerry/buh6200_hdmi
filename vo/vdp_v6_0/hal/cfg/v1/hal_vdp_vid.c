/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_vid.h"
#include "hal_vdp_comm.h"
#include "hal_vdp_debug.h"
#include "hal_vdp_ip_para.h"

#ifdef VDP_SDK_PQ_SUPPORT
#include "vdp_ext_func.h"
#endif

#define VDP_DEFAULT_FDR_CMP_RADIO 2000
#define VDP_DRAW_LINE_DIV2        2

typedef struct hi_layer_format {
    hi_drv_pixel_format pix_format;
    hi_drv_compress_mode cmp_mode;
    hi_drv_data_fmt data_fmt;

    vdp_fdr_vid_data_fmt vid_data_fmt;
    vdp_fdr_vid_in_fmt in_fmt;
    vdp_fdr_vid_data_type data_type;
    vdp_fdr_dcmp_type dcmp_type;
    hi_bool lossless;
    hi_bool uv_order;
} layer_format;

static layer_format g_layer_format[] = {
    /* line 422 uvorder */
    {   HI_DRV_PIXEL_FMT_NV16_2X1, HI_DRV_COMPRESS_MODE_OFF, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_422,
        VDP_FDR_VID_DATA_TYPE_YUV422, VDP_FDR_DCMP_TYPE_OFF, HI_TRUE, HI_TRUE
    },
    /* line 422 */
    {   HI_DRV_PIXEL_FMT_NV61_2X1, HI_DRV_COMPRESS_MODE_OFF, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_422,
        VDP_FDR_VID_DATA_TYPE_YUV422, VDP_FDR_DCMP_TYPE_OFF, HI_TRUE, HI_FALSE
    },
    /* line 420 */
    {   HI_DRV_PIXEL_FMT_NV21, HI_DRV_COMPRESS_MODE_OFF, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_420,
        VDP_FDR_VID_DATA_TYPE_YUV420, VDP_FDR_DCMP_TYPE_OFF, HI_TRUE, HI_FALSE
    },
    /* line 420 uvorder */
    {   HI_DRV_PIXEL_FMT_NV12, HI_DRV_COMPRESS_MODE_OFF, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_420,
        VDP_FDR_VID_DATA_TYPE_YUV420, VDP_FDR_DCMP_TYPE_OFF, HI_TRUE, HI_TRUE
    },
    /* line 444 */
    {   HI_DRV_PIXEL_FMT_NV42, HI_DRV_COMPRESS_MODE_OFF, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_444,
        VDP_FDR_VID_DATA_TYPE_YUV444, VDP_FDR_DCMP_TYPE_OFF, HI_TRUE, HI_FALSE
    },
    /* line 422 uvorder */
    {   HI_DRV_PIXEL_FMT_NV16, HI_DRV_COMPRESS_MODE_OFF, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_422,
        VDP_FDR_VID_DATA_TYPE_YUV422, VDP_FDR_DCMP_TYPE_OFF, HI_TRUE, HI_TRUE
    },
    /* line 422 */
    {   HI_DRV_PIXEL_FMT_NV61, HI_DRV_COMPRESS_MODE_OFF, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_422,
        VDP_FDR_VID_DATA_TYPE_YUV422, VDP_FDR_DCMP_TYPE_OFF, HI_TRUE, HI_FALSE
    },
    /* rgb8888 */
    {   HI_DRV_PIXEL_FMT_RGB24, HI_DRV_COMPRESS_MODE_OFF, HI_DRV_DATA_FMT_PKG,
        VDP_FDR_VID_DATA_FMT_PKG, VDP_FDR_VID_IFMT_RGB_888,
        VDP_FDR_VID_DATA_TYPE_RGB888, VDP_FDR_DCMP_TYPE_OFF, HI_TRUE, HI_FALSE
    },
    /* argb8888 */
    {   HI_DRV_PIXEL_FMT_ARGB8888, HI_DRV_COMPRESS_MODE_OFF, HI_DRV_DATA_FMT_PKG,
        VDP_FDR_VID_DATA_FMT_PKG, VDP_FDR_VID_IFMT_ARGB_8888,
        VDP_FDR_VID_DATA_TYPE_ARGB8888, VDP_FDR_DCMP_TYPE_OFF, HI_TRUE, HI_FALSE
    },
    /* line cmp 420 loss */
    {   HI_DRV_PIXEL_FMT_NV21, HI_DRV_COMPRESS_MODE_LINE_LOSS, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_420,
        VDP_FDR_VID_DATA_TYPE_YUV420, VDP_FDR_DCMP_TYPE_LINE, HI_FALSE, HI_FALSE
    },
    /* line cmp 420 lossless */
    {   HI_DRV_PIXEL_FMT_NV21, HI_DRV_COMPRESS_MODE_LINE_LOSSLESS, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_420,
        VDP_FDR_VID_DATA_TYPE_YUV420, VDP_FDR_DCMP_TYPE_LINE, HI_TRUE, HI_FALSE
    },
    /* line cmp 422 loss */
    {   HI_DRV_PIXEL_FMT_NV61, HI_DRV_COMPRESS_MODE_LINE_LOSS, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_422,
        VDP_FDR_VID_DATA_TYPE_YUV422, VDP_FDR_DCMP_TYPE_LINE, HI_FALSE, HI_FALSE
    },
    /* line cmp 422 lossless */
    {   HI_DRV_PIXEL_FMT_NV61, HI_DRV_COMPRESS_MODE_LINE_LOSSLESS, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_422,
        VDP_FDR_VID_DATA_TYPE_YUV422, VDP_FDR_DCMP_TYPE_LINE, HI_TRUE, HI_FALSE
    },
    /* line cmp 422 loss uvorder */
    {   HI_DRV_PIXEL_FMT_NV16, HI_DRV_COMPRESS_MODE_LINE_LOSS, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_422,
        VDP_FDR_VID_DATA_TYPE_YUV422, VDP_FDR_DCMP_TYPE_LINE, HI_FALSE, HI_TRUE
    },
    /* line cmp 422 lossless uvorder */
    {   HI_DRV_PIXEL_FMT_NV16, HI_DRV_COMPRESS_MODE_LINE_LOSSLESS, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_422,
        VDP_FDR_VID_DATA_TYPE_YUV422, VDP_FDR_DCMP_TYPE_LINE, HI_TRUE, HI_TRUE
    },
    /* line cmp 422 loss */
    {   HI_DRV_PIXEL_FMT_NV61_2X1, HI_DRV_COMPRESS_MODE_LINE_LOSS, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_422,
        VDP_FDR_VID_DATA_TYPE_YUV422, VDP_FDR_DCMP_TYPE_LINE, HI_FALSE, HI_FALSE
    },
    /* line cmp 422 lossless */
    {   HI_DRV_PIXEL_FMT_NV61_2X1, HI_DRV_COMPRESS_MODE_LINE_LOSSLESS, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_422,
        VDP_FDR_VID_DATA_TYPE_YUV422, VDP_FDR_DCMP_TYPE_LINE, HI_TRUE, HI_FALSE
    },
    /* line cmp 422 loss uvorder */
    {   HI_DRV_PIXEL_FMT_NV16_2X1, HI_DRV_COMPRESS_MODE_LINE_LOSS, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_422,
        VDP_FDR_VID_DATA_TYPE_YUV422, VDP_FDR_DCMP_TYPE_LINE, HI_FALSE, HI_TRUE
    },
    /* line cmp 422 lossless uvorder */
    {   HI_DRV_PIXEL_FMT_NV16_2X1, HI_DRV_COMPRESS_MODE_LINE_LOSSLESS, HI_DRV_DATA_FMT_LINER,
        VDP_FDR_VID_DATA_FMT_LINEAR, VDP_FDR_VID_IFMT_SP_422,
        VDP_FDR_VID_DATA_TYPE_YUV422, VDP_FDR_DCMP_TYPE_LINE, HI_TRUE, HI_TRUE
    },
    /* 420 tile */
    {   HI_DRV_PIXEL_FMT_NV21, HI_DRV_COMPRESS_MODE_OFF, HI_DRV_DATA_FMT_TILE,
        VDP_FDR_VID_DATA_FMT_TILE, VDP_FDR_VID_IFMT_SP_TILE,
        VDP_FDR_VID_DATA_TYPE_YUV420, VDP_FDR_DCMP_TYPE_OFF, HI_TRUE, HI_FALSE
    },
    /* 420 tile cmp loss */
    {   HI_DRV_PIXEL_FMT_NV21, HI_DRV_COMPRESS_MODE_SEG_LOSS, HI_DRV_DATA_FMT_TILE,
        VDP_FDR_VID_DATA_FMT_TILE, VDP_FDR_VID_IFMT_SP_TILE,
        VDP_FDR_VID_DATA_TYPE_YUV420, VDP_FDR_DCMP_TYPE_SEG, HI_FALSE, HI_FALSE
    },
    /* 420 tile cmp lossless */
    {   HI_DRV_PIXEL_FMT_NV21, HI_DRV_COMPRESS_MODE_SEG_LOSSLESS, HI_DRV_DATA_FMT_TILE,
        VDP_FDR_VID_DATA_FMT_TILE, VDP_FDR_VID_IFMT_SP_TILE,
        VDP_FDR_VID_DATA_TYPE_YUV420, VDP_FDR_DCMP_TYPE_SEG, HI_FALSE, HI_FALSE
    },

};

static vdp_data_wth transfer_bit_width(hi_drv_pixel_bitwidth frame_bit_width)
{
    switch (frame_bit_width) {
        case HI_DRV_PIXEL_BITWIDTH_8BIT:
            return VDP_DATA_WTH_8;
        case HI_DRV_PIXEL_BITWIDTH_10BIT:
            return VDP_DATA_WTH_10;
        case HI_DRV_PIXEL_BITWIDTH_12BIT:
            return VDP_DATA_WTH_12;
        default:
            VDP_PRINT("Invalid enBitWidth %d.\n", frame_bit_width);
            return VDP_DATA_WTH_8;
    }
}

static vdp_proc_fmt hal_layer_transfer_pix_fmt(vdp_fdr_vid_data_type data_type)
{
    vdp_proc_fmt in_pix_fmt;
    switch (data_type) {
        case VDP_FDR_VID_DATA_TYPE_YUV422:
            in_pix_fmt = VDP_PROC_FMT_SP_422;
            break;
        case VDP_FDR_VID_DATA_TYPE_YUV420:
            in_pix_fmt = VDP_PROC_FMT_SP_420;
            break;
        case VDP_FDR_VID_DATA_TYPE_YUV444:
            in_pix_fmt = VDP_PROC_FMT_SP_444;
            break;
        case VDP_FDR_VID_DATA_TYPE_RGB888:
        case VDP_FDR_VID_DATA_TYPE_ARGB8888:
            in_pix_fmt = VDP_PROC_FMT_RGB_888;
            break;
        default:
            in_pix_fmt = VDP_PROC_FMT_SP_422;
            break;
    }

    return in_pix_fmt;
}

static hi_void hal_layer_zme_strategy_init(vdp_zme_node id, vdp_layer_info *layer_info, vdp_vid_info *vid_info)
{
    vid_info->zme_info.image_info[id].v_enable = HI_TRUE;
    vid_info->zme_info.image_info[id].h_enable = HI_TRUE;

#ifdef VDP_CBB_TEST_SUPPORT
    // no cbb test use cpy mode
    vid_info->zme_info.image_info[id].h_lfir_en = HI_TRUE;
    vid_info->zme_info.image_info[id].h_cfir_en = HI_TRUE;
    vid_info->zme_info.image_info[id].v_lfir_en = HI_TRUE;
    vid_info->zme_info.image_info[id].v_cfir_en = HI_TRUE;
#endif

    vid_info->zme_info.image_info[id].h_med_en = HI_FALSE;
    vid_info->zme_info.image_info[id].v_med_en = HI_FALSE;
    vid_info->zme_info.image_info[id].in_mode = !layer_info->frame_inerlace;
    vid_info->zme_info.image_info[id].out_mode = !layer_info->frame_inerlace;
    vid_info->zme_info.image_info[id].offset.h_loffset = 0;
    vid_info->zme_info.image_info[id].offset.h_coffset = 0;
    vid_info->zme_info.image_info[id].offset.v_loffset = 0;
    vid_info->zme_info.image_info[id].offset.v_coffset = 0;
    vid_info->zme_info.image_info[id].offset.v_bloffset = 0;
    vid_info->zme_info.image_info[id].offset.v_bcoffset = 0;
}

static hi_void get_vid_fdr_src_info(vdp_layer_vid layer_id, vdp_layer_info *layer_info, vdp_vid_info *vid_info)
{
    hi_u32 i;
    vid_info->fdr_info.flip_en = layer_info->flip;
    vid_info->fdr_info.chm_copy_en = HI_FALSE;
    vid_info->fdr_info.rmode_3d = VDP_FDR_RMODE_3D_INTF;
    vid_info->fdr_info.rmode = (layer_info->frame_inerlace == HI_FALSE)
                               ? VDP_FDR_RMODE_PROGRESSIVE
                               : VDP_FDR_RMODE_INTERFACE;
    vid_info->fdr_info.smmu_en = layer_info->frame_smmu_en;
    vid_info->fdr_info.secure_en = layer_info->frame_secure_en;
    /* payattention : draw line need update in zme */
    vid_info->fdr_info.draw_pixel_mode = VDP_FDR_DRAW_MODE_1;
    vid_info->fdr_info.draw_mode = VDP_FDR_DRAW_MODE_1;

    /* src info */
    vid_info->fdr_info.data_width = transfer_bit_width(layer_info->frame_bit_width);
    for (i = 0; i < sizeof(g_layer_format) / sizeof(layer_format); i++) {
        if ((layer_info->frame_pix_format == g_layer_format[i].pix_format) &&
            (layer_info->data_fmt == g_layer_format[i].data_fmt) &&
            (layer_info->cmp_mode == g_layer_format[i].cmp_mode)) {
            vid_info->fdr_info.in_fmt = g_layer_format[i].in_fmt;
            vid_info->fdr_info.data_fmt = g_layer_format[i].vid_data_fmt;
            vid_info->fdr_info.data_type = g_layer_format[i].data_type;
            vid_info->fdr_info.uv_order = g_layer_format[i].uv_order;
            vid_info->fdr_info.dcmp_type = g_layer_format[i].dcmp_type;
            vid_info->fdr_info.dcmp_line_cfg.lossless_y = g_layer_format[i].lossless;
            vid_info->fdr_info.dcmp_line_cfg.lossless_c = g_layer_format[i].lossless;
            break;
        }
    }

    /* select 2d mode, this is a special mode only for 430 */
    vid_info->fdr_info.src_disp_mode = VDP_FDR_DISP_MODE_2D;
}

static hi_void get_vid_fdr_mute_info(vdp_layer_vid layer_id, vdp_layer_info *layer_info, vdp_vid_info *vid_info)
{
    vdp_ip_csc_data_transfer(&layer_info->mute_color,
                             layer_info->frame_color_space, &vid_info->fdr_info.mute_bkg);

    vid_info->fdr_info.mute_en = layer_info->mute_en;
    vid_info->fdr_info.mute_req_en = HI_FALSE;
}

static hi_void get_vid_fdr_dcmp_info(vdp_layer_vid layer_id, vdp_layer_info *layer_info, vdp_vid_info *vid_info)
{
    /* dcmp */
    vid_info->fdr_info.dcmp_seg_cfg.is_raw_en = HI_FALSE;
    vid_info->fdr_info.dcmp_seg_cfg.csc_en = HI_FALSE;

    if (vid_info->fdr_info.dcmp_type == VDP_FDR_DCMP_TYPE_LINE) {
        vid_info->fdr_info.dcmp_line_cfg.enable = HI_TRUE;
    } else {
        vid_info->fdr_info.dcmp_line_cfg.enable = HI_FALSE;
    }

    vid_info->fdr_info.dcmp_line_cfg.cmp_ratio_y = layer_info->ycmp_rate;
    vid_info->fdr_info.dcmp_line_cfg.cmp_ratio_c = layer_info->ccmp_rate;

    vid_info->fdr_info.dcmp_frm_cfg.dcmp_en = HI_FALSE;
}

static hi_bool get_hdr_tran_process_type(vdp_layer_vid layer_id, vdp_layer_info *layer_info)
{
    if ((layer_info->frame_hdr_type != HI_DRV_HDR_TYPE_SDR) ||
        (layer_info->out_hdr_type != HI_DRV_DISP_TYPE_NORMAL)) {
        return HI_TRUE;
    }

    if ((layer_info->frame_color_space.color_primary == HI_DRV_COLOR_PRIMARY_BT2020) ||
        (layer_info->out_color_space.color_primary == HI_DRV_COLOR_PRIMARY_BT2020)) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_void get_vid_fdr_multiregion_info(vdp_layer_vid layer_id, vdp_layer_info *layer_info,
                                            vdp_vid_info *vid_info)
{
    hi_u32 i;

    vid_info->fdr_info.mrg_mode_en = (layer_id == VDP_LAYER_VID1) ? layer_info->multi_region_enable : HI_FALSE;
    vid_info->fdr_info.mrg_total_num = (layer_id == VDP_LAYER_VID1) ? FDR_REGION_NUM_MAX : 0;

    if (vid_info->fdr_info.mrg_mode_en) {
        for (i = 0; i < FDR_REGION_NUM_MAX; i++) {
            vid_info->fdr_info.mrg_info[i].enable = layer_info->region[i].enable;
            vid_info->fdr_info.mrg_info[i].mute_en = layer_info->region[i].mute_en;
            vid_info->fdr_info.mrg_info[i].crop_en = HI_FALSE;
            vid_info->fdr_info.mrg_info[i].mmu_bypass = layer_info->frame_smmu_en;
            vid_info->fdr_info.mrg_info[i].dcmp_en =
                (vid_info->fdr_info.dcmp_type == VDP_FDR_DCMP_TYPE_OFF) ? HI_FALSE : HI_TRUE;
            vid_info->fdr_info.mrg_info[i].in_rect = layer_info->region[i].in_rect;
            vid_info->fdr_info.mrg_info[i].disp_rect = layer_info->region[i].disp_rect;
            memcpy(vid_info->fdr_info.mrg_info[i].addr, layer_info->region[i].addr,
                   sizeof(layer_info->region[i].addr));
        }
    }
}

static hi_void hal_layer_get_fdr_info(vdp_layer_vid layer_id, vdp_layer_info *layer_info, vdp_vid_info *vid_info)
{
    /* crop */
    vid_info->fdr_info.in_rect = layer_info->crop_rect;
    vid_info->fdr_info.src_rect = layer_info->frame_rect;

    get_vid_fdr_src_info(layer_id, layer_info, vid_info);
    get_vid_fdr_mute_info(layer_id, layer_info, vid_info);
    get_vid_fdr_dcmp_info(layer_id, layer_info, vid_info);
    get_vid_fdr_multiregion_info(layer_id, layer_info, vid_info);

    /* addr */
    memcpy(vid_info->fdr_addr.addr, layer_info->frame_addr, sizeof(layer_info->frame_addr));
}

#ifdef VDP_SDK_PQ_SUPPORT

static hi_drv_pq_zme_fmt hal_layer_transfer_pq_pix_fmt(vdp_fdr_vid_data_type data_type)
{
    hi_drv_pq_zme_fmt in_pix_fmt;
    switch (data_type) {
        case VDP_FDR_VID_DATA_TYPE_YUV420:
            in_pix_fmt = HI_DRV_PQ_ZME_FMT_420;
            break;
        case VDP_FDR_VID_DATA_TYPE_YUV422:
            in_pix_fmt = HI_DRV_PQ_ZME_FMT_422;
            break;
        case VDP_FDR_VID_DATA_TYPE_YUV444:
        case VDP_FDR_VID_DATA_TYPE_RGB888:
        case VDP_FDR_VID_DATA_TYPE_ARGB8888:
            in_pix_fmt = HI_DRV_PQ_ZME_FMT_444;
            break;
        default:
            in_pix_fmt = HI_DRV_PQ_ZME_FMT_420;
            break;
    }
    return in_pix_fmt;
}

static vdp_fdr_draw_mode hal_layer_transfer_pq_drawline(hi_drv_pq_ver_draw_mul ver_draw_mul)
{
    vdp_fdr_draw_mode draw_mode;
    switch (ver_draw_mul) {
        case HI_DRV_PQ_PREZME_VER_DISABLE:
            draw_mode = VDP_FDR_DRAW_MODE_1;
            break;
        case HI_DRV_PQ_PREZME_VER_2:
            draw_mode = VDP_FDR_DRAW_MODE_2;
            break;
        case HI_DRV_PQ_PREZME_VER_4:
            draw_mode = VDP_FDR_DRAW_MODE_4;
            break;
        case HI_DRV_PQ_PREZME_VER_8:
            draw_mode = VDP_FDR_DRAW_MODE_8;
            break;
        case HI_DRV_PQ_PREZME_VER_16:
            draw_mode = VDP_FDR_DRAW_MODE_16;
            break;
        default:
            draw_mode = VDP_FDR_DRAW_MODE_1;
            break;
    }
    return draw_mode;
}

static vdp_fdr_draw_mode hal_layer_transfer_pq_drawpixel(hi_drv_pq_hor_draw_mul hor_draw_mul)
{
    vdp_fdr_draw_mode draw_mode;
    switch (hor_draw_mul) {
        case HI_DRV_PQ_PREZME_HOR_DISABLE:
            draw_mode = VDP_FDR_DRAW_MODE_1;
            break;
        case HI_DRV_PQ_PREZME_HOR_2:
            draw_mode = VDP_FDR_DRAW_MODE_2;
            break;
        case HI_DRV_PQ_PREZME_HOR_4:
            draw_mode = VDP_FDR_DRAW_MODE_4;
            break;
        case HI_DRV_PQ_PREZME_HOR_8:
            draw_mode = VDP_FDR_DRAW_MODE_8;
            break;
        case HI_DRV_PQ_PREZME_HOR_16:
            draw_mode = VDP_FDR_DRAW_MODE_16;
            break;
        default:
            draw_mode = VDP_FDR_DRAW_MODE_1;
            break;
    }
    return draw_mode;
}

static hi_void hal_layer_transfer_pq_zme_node_info(hi_drv_pq_zme_common_out *zme_fmt,
                                                   vdp_zme_image_info *image_info, vdp_zme_node zme_node)
{
    image_info->link = zme_fmt->zme_point;
    image_info->in_width = zme_fmt->zme_w_in;
    image_info->in_height = zme_fmt->zme_h_in;
    image_info->out_width = zme_fmt->zme_w_out;
    image_info->out_height = zme_fmt->zme_h_out;
    image_info->in_pix_fmt = zme_fmt->zme_fmt_in;
    image_info->out_pix_fmt = zme_fmt->zme_fmt_out;
    image_info->v_out_pix_fmt = zme_fmt->zme_fmt_out; /* no use now */
    image_info->v_enable = zme_fmt->zme_ver_enable;
    image_info->h_enable = zme_fmt->zme_hor_enable;
    image_info->h_lfir_en = zme_fmt->zme_fir_mode.zme_fir_hl == HI_DRV_PQ_ZME_FIR ? 1 : 0;
    image_info->h_cfir_en = zme_fmt->zme_fir_mode.zme_fir_hc == HI_DRV_PQ_ZME_FIR ? 1 : 0;
    image_info->v_lfir_en = zme_fmt->zme_fir_mode.zme_fir_vl == HI_DRV_PQ_ZME_FIR ? 1 : 0;
    image_info->v_cfir_en = zme_fmt->zme_fir_mode.zme_fir_vc == HI_DRV_PQ_ZME_FIR ? 1 : 0;
    image_info->h_med_en = zme_fmt->zme_med_fir_en.zme_med_h_en;
    image_info->v_med_en = zme_fmt->zme_med_fir_en.zme_med_h_en;
    image_info->in_mode = zme_fmt->frame_fmt_in == HI_DRV_PQ_FRM_FRAME ? 1 : 0;
    image_info->out_mode = zme_fmt->frame_fmt_out == HI_DRV_PQ_FRM_FRAME ? 1 : 0;
    image_info->offset.h_loffset = zme_fmt->zme_offset.zme_offset_hl;
    image_info->offset.h_coffset = zme_fmt->zme_offset.zme_offset_hc;
    image_info->offset.v_loffset = zme_fmt->zme_offset.zme_offset_vl;
    image_info->offset.v_coffset = zme_fmt->zme_offset.zme_offset_vc;
    image_info->offset.v_bloffset = zme_fmt->zme_offset.zme_offset_vl_btm;
    image_info->offset.v_bcoffset = zme_fmt->zme_offset.zme_offset_vc_btm;
}

hi_void hal_layer_zme_get_strategy_from_pq(vdp_layer_vid layer_id, vdp_layer_info *layer_info,
                                           vdp_vid_info *vid_info)
{
    hi_drv_pq_zme_strategy_in zme_in = { 0 };
    hi_drv_pq_zme_strategy_out zme_out = { 0 };

    zme_in.hd_zme_strategy.zme_common_info.zme_w_in = layer_info->crop_rect.w;
    zme_in.hd_zme_strategy.zme_common_info.zme_h_in = layer_info->crop_rect.h;
    zme_in.hd_zme_strategy.zme_common_info.zme_w_out = vid_info->vid_cfg.video_rect.w;
    zme_in.hd_zme_strategy.zme_common_info.zme_h_out = vid_info->vid_cfg.video_rect.h;
    zme_in.hd_zme_strategy.zme_common_info.zme_fmt_in =
        hal_layer_transfer_pq_pix_fmt(vid_info->fdr_info.data_type);
    zme_in.hd_zme_strategy.zme_common_info.zme_fmt_out = HI_DRV_PQ_ZME_FMT_444;
    zme_in.hd_zme_strategy.zme_common_info.zme_frame_in = HI_DRV_PQ_FRM_FRAME;  /* need update by cs */
    zme_in.hd_zme_strategy.zme_common_info.zme_frame_out = HI_DRV_PQ_FRM_FRAME; /* need update by cs */

    zme_in.hd_zme_strategy.is_dolby_path = HI_FALSE;

    zme_in.hd_zme_strategy.hdcp_factor.is_hdcp_scence = HI_FALSE;  // layer_info->hdcp_en;
    zme_in.hd_zme_strategy.hdcp_factor.sr_en = HI_TRUE;
    zme_in.hd_zme_strategy.hdcp_factor.hdcp_width = 0x0;  /* need update by hdcp */
    zme_in.hd_zme_strategy.hdcp_factor.hdcp_height = 0x0; /* need update by hdcp */

    if (vdp_ext_pq_get_vdp_zme_strategy(HI_DRV_PQ_DISP_V0_LAYER_ZME,
                                        &zme_in, &zme_out) != HI_SUCCESS) {
        return;
    }

    vid_info->fdr_info.draw_mode = hal_layer_transfer_pq_drawline(zme_out.hd_zme_strategy.ver_draw_mul);
    vid_info->fdr_info.draw_pixel_mode = hal_layer_transfer_pq_drawpixel(zme_out.hd_zme_strategy.hor_draw_mul);

    hal_layer_transfer_pq_zme_node_info(&zme_out.hd_zme_strategy.zme_fmt[HI_DRV_PQ_4KZME],
                                        &vid_info->zme_info.image_info[VDP_ZME_4K_ZME], VDP_ZME_4K_ZME);
    hal_layer_transfer_pq_zme_node_info(&zme_out.hd_zme_strategy.zme_fmt[HI_DRV_PQ_4KSR],
                                        &vid_info->zme_info.image_info[VDP_ZME_4K_SR], VDP_ZME_4K_SR);
    hal_layer_transfer_pq_zme_node_info(&zme_out.hd_zme_strategy.zme_fmt[HI_DRV_PQ_8KSR],
                                        &vid_info->zme_info.image_info[VDP_ZME_8K_SR], VDP_ZME_8K_SR);
    hal_layer_transfer_pq_zme_node_info(&zme_out.hd_zme_strategy.zme_fmt[HI_DRV_PQ_8KZME],
                                        &vid_info->zme_info.image_info[VDP_ZME_8K_ZME], VDP_ZME_8K_ZME);

    vid_info->zme_info.cle_8k_enable = zme_out.hd_zme_strategy.cle_8k_en;
    vid_info->zme_info.cle_4k_enable = zme_out.hd_zme_strategy.cle_4k_en;

    g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_XDP_2DSCALE_V] =
        zme_out.hd_zme_strategy.zme_coef_addr.pq_4kzme_coef_addr_v;
    g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_XDP_2DSCALE_H] =
        zme_out.hd_zme_strategy.zme_coef_addr.pq_4kzme_coef_addr_h;
    g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_8K_VZME] = zme_out.hd_zme_strategy.zme_coef_addr.pq_8kzme_coef_addr_v;
    g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_8K_HZME] = zme_out.hd_zme_strategy.zme_coef_addr.pq_8kzme_coef_addr_h;
    g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_4K_SR0] = zme_out.hd_zme_strategy.zme_coef_addr.pq_4ksr0_coef_addr;
    g_vdp_coef_buf_addr.phy_addr[VDP_COEF_BUF_8K_SR0] = zme_out.hd_zme_strategy.zme_coef_addr.pq_8ksr0_coef_addr;
}
#else
static hi_void hal_layer_zme_strategy_src64(vdp_layer_vid layer_id, vdp_layer_info *layer_info,
                                            vdp_vid_info *vid_info)
{
    vdp_zme_info *zme = &(vid_info->zme_info);

    if (vid_info->vid_cfg.video_rect.w / 4 < layer_info->crop_rect.w) { /* 4 is number */
        hal_layer_zme_strategy_init(VDP_ZME_4K_ZME, layer_info, vid_info);
        zme->image_info[VDP_ZME_4K_ZME].link = 0;
        zme->image_info[VDP_ZME_4K_ZME].in_pix_fmt = hal_layer_transfer_pix_fmt(vid_info->fdr_info.data_type);
        zme->image_info[VDP_ZME_4K_ZME].out_pix_fmt = VDP_PROC_FMT_SP_444;
        zme->image_info[VDP_ZME_4K_ZME].in_width = layer_info->crop_rect.w;
        zme->image_info[VDP_ZME_4K_ZME].in_height = layer_info->crop_rect.h;
        zme->image_info[VDP_ZME_4K_ZME].out_width = vid_info->vid_cfg.video_rect.w;
        zme->image_info[VDP_ZME_4K_ZME].out_height = vid_info->vid_cfg.video_rect.h;
    } else {
        hal_layer_zme_strategy_init(VDP_ZME_4K_ZME, layer_info, vid_info);
        zme->image_info[VDP_ZME_4K_ZME].link = 0;
        zme->image_info[VDP_ZME_4K_ZME].in_pix_fmt = hal_layer_transfer_pix_fmt(vid_info->fdr_info.data_type);
        zme->image_info[VDP_ZME_4K_ZME].out_pix_fmt = VDP_PROC_FMT_SP_444;
        zme->image_info[VDP_ZME_4K_ZME].in_width = layer_info->crop_rect.w;
        zme->image_info[VDP_ZME_4K_ZME].in_height = layer_info->crop_rect.h;
        zme->image_info[VDP_ZME_4K_ZME].out_width = layer_info->crop_rect.w * 2;  /* 2 is number */
        zme->image_info[VDP_ZME_4K_ZME].out_height = layer_info->crop_rect.h * 2; /* 2 is number */

        hal_layer_zme_strategy_init(VDP_ZME_4K_SR, layer_info, vid_info);
        zme->image_info[VDP_ZME_4K_SR].link = 0;
        zme->image_info[VDP_ZME_4K_SR].in_pix_fmt = VDP_PROC_FMT_SP_444;
        zme->image_info[VDP_ZME_4K_SR].out_pix_fmt = VDP_PROC_FMT_SP_444;
        zme->image_info[VDP_ZME_4K_SR].in_width = zme->image_info[VDP_ZME_4K_ZME].out_width;
        zme->image_info[VDP_ZME_4K_SR].in_height = zme->image_info[VDP_ZME_4K_ZME].out_height;
        zme->image_info[VDP_ZME_4K_SR].out_width = zme->image_info[VDP_ZME_4K_ZME].out_width * 2;   /* 2 is number */
        zme->image_info[VDP_ZME_4K_SR].out_height = zme->image_info[VDP_ZME_4K_ZME].out_height * 2; /* 2 is number */

        hal_layer_zme_strategy_init(VDP_ZME_8K_SR, layer_info, vid_info);
        zme->image_info[VDP_ZME_8K_SR].link = 0;
        zme->image_info[VDP_ZME_8K_SR].in_pix_fmt = VDP_PROC_FMT_SP_444;
        zme->image_info[VDP_ZME_8K_SR].out_pix_fmt = VDP_PROC_FMT_SP_444;
        zme->image_info[VDP_ZME_8K_SR].in_width = zme->image_info[VDP_ZME_4K_SR].out_width;
        zme->image_info[VDP_ZME_8K_SR].in_height = zme->image_info[VDP_ZME_4K_SR].out_height;
        zme->image_info[VDP_ZME_8K_SR].out_width = zme->image_info[VDP_ZME_4K_SR].out_width * 2;   /* 2 is number */
        zme->image_info[VDP_ZME_8K_SR].out_height = zme->image_info[VDP_ZME_4K_SR].out_height * 2; /* 2 is number */

        hal_layer_zme_strategy_init(VDP_ZME_8K_ZME, layer_info, vid_info);
        zme->image_info[VDP_ZME_8K_ZME].link = 1;
        zme->image_info[VDP_ZME_8K_ZME].in_pix_fmt = VDP_PROC_FMT_SP_444;
        zme->image_info[VDP_ZME_8K_ZME].out_pix_fmt = VDP_PROC_FMT_SP_444;
        zme->image_info[VDP_ZME_8K_ZME].in_width = zme->image_info[VDP_ZME_8K_SR].out_width;
        zme->image_info[VDP_ZME_8K_ZME].in_height = zme->image_info[VDP_ZME_8K_SR].out_height;
        zme->image_info[VDP_ZME_8K_ZME].out_width = vid_info->vid_cfg.video_rect.w;
        zme->image_info[VDP_ZME_8K_ZME].out_height = vid_info->vid_cfg.video_rect.h;
    }
}

static hi_void hal_layer_zme_strategy_drawmode(hi_u32 zoom, vdp_layer_info *layer_info, vdp_vid_info *vid_info)
{
    if (layer_info->crop_rect.w > vid_info->vid_cfg.video_rect.w) {
        if (layer_info->crop_rect.w / zoom <= vid_info->vid_cfg.video_rect.w) {
            vid_info->fdr_info.draw_pixel_mode = VDP_DRAW_MODE_1;
        } else if (layer_info->crop_rect.w / (zoom * 2) < vid_info->vid_cfg.video_rect.w) { /* 2 is number */
            vid_info->fdr_info.draw_pixel_mode = VDP_DRAW_MODE_2;
        } else if (layer_info->crop_rect.w / (zoom * 4) < vid_info->vid_cfg.video_rect.w) { /* 4 is number */
            vid_info->fdr_info.draw_pixel_mode = VDP_DRAW_MODE_4;
        } else if (layer_info->crop_rect.w / (zoom * 8) < vid_info->vid_cfg.video_rect.w) { /* 8 is number */
            vid_info->fdr_info.draw_pixel_mode = VDP_DRAW_MODE_8;
        } else if (layer_info->crop_rect.w / (zoom * 16) < vid_info->vid_cfg.video_rect.w) { /* 16 is number */
            vid_info->fdr_info.draw_pixel_mode = VDP_DRAW_MODE_16;
        } else {
            vid_info->fdr_info.draw_pixel_mode = VDP_DRAW_MODE_32;
        }
        layer_info->crop_rect.w /= (1 << vid_info->fdr_info.draw_pixel_mode);
    }

    if (layer_info->crop_rect.h > vid_info->vid_cfg.video_rect.h) {
        if (layer_info->crop_rect.h / zoom <= vid_info->vid_cfg.video_rect.h) {
            vid_info->fdr_info.draw_mode = VDP_DRAW_MODE_1;
        } else if (layer_info->crop_rect.h / (zoom * 2) < vid_info->vid_cfg.video_rect.h) { /* 2 is number */
            vid_info->fdr_info.draw_mode = VDP_DRAW_MODE_2;
        } else if (layer_info->crop_rect.h / (zoom * 4) < vid_info->vid_cfg.video_rect.h) { /* 4 is number */
            vid_info->fdr_info.draw_mode = VDP_DRAW_MODE_4;
        } else if (layer_info->crop_rect.h / (zoom * 8) < vid_info->vid_cfg.video_rect.h) { /* 8 is number */
            vid_info->fdr_info.draw_mode = VDP_DRAW_MODE_8;
        } else if (layer_info->crop_rect.h / (zoom * 16) < vid_info->vid_cfg.video_rect.h) { /* 16 is number */
            vid_info->fdr_info.draw_mode = VDP_DRAW_MODE_16;
        } else {
            vid_info->fdr_info.draw_mode = VDP_DRAW_MODE_32;
        }
        layer_info->crop_rect.h /= (1 << vid_info->fdr_info.draw_mode);
    }
}

static hi_void hal_layer_zme_strategy_other(vdp_layer_vid layer_id, vdp_layer_info *layer_info,
                                            vdp_vid_info *vid_info)
{
    if (layer_info->crop_rect.h / 64 < vid_info->vid_cfg.video_rect.h) { /* 64 is number */
        hal_layer_zme_strategy_drawmode(2, layer_info, vid_info);        /* 2 is number */

        hal_layer_zme_strategy_init(VDP_ZME_8K_ZME, layer_info, vid_info);
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].link = 0;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_pix_fmt =
            hal_layer_transfer_pix_fmt(vid_info->fdr_info.data_type);
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_pix_fmt = VDP_PROC_FMT_SP_444;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_width = layer_info->crop_rect.w;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_height = layer_info->crop_rect.h;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_width = vid_info->vid_cfg.video_rect.w;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_height = vid_info->vid_cfg.video_rect.h;
    } else if (layer_info->crop_rect.h / 128 < vid_info->vid_cfg.video_rect.h) { /* 128 is number */
        hal_layer_zme_strategy_drawmode(4, layer_info, vid_info);                /* 4 is number */

        hal_layer_zme_strategy_init(VDP_ZME_4K_ZME, layer_info, vid_info);
        vid_info->zme_info.image_info[VDP_ZME_4K_ZME].link = 0;
        vid_info->zme_info.image_info[VDP_ZME_4K_ZME].in_pix_fmt =
            hal_layer_transfer_pix_fmt(vid_info->fdr_info.data_type);
        vid_info->zme_info.image_info[VDP_ZME_4K_ZME].out_pix_fmt = VDP_PROC_FMT_SP_444;
        vid_info->zme_info.image_info[VDP_ZME_4K_ZME].in_width = layer_info->crop_rect.w;
        vid_info->zme_info.image_info[VDP_ZME_4K_ZME].in_height = layer_info->crop_rect.h;
        vid_info->zme_info.image_info[VDP_ZME_4K_ZME].out_width = vid_info->vid_cfg.video_rect.w / 2;  /* 2 is number */
        vid_info->zme_info.image_info[VDP_ZME_4K_ZME].out_height = vid_info->vid_cfg.video_rect.h / 2; /* 2 is number */

        hal_layer_zme_strategy_init(VDP_ZME_8K_ZME, layer_info, vid_info);
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].link = 1;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_pix_fmt = VDP_PROC_FMT_SP_444;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_pix_fmt = VDP_PROC_FMT_SP_444;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_width =
            vid_info->zme_info.image_info[VDP_ZME_4K_ZME].out_width;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_height =
            vid_info->zme_info.image_info[VDP_ZME_4K_ZME].out_height;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_width = vid_info->vid_cfg.video_rect.w;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_height = vid_info->vid_cfg.video_rect.h;
    } else {
        hal_layer_zme_strategy_init(VDP_ZME_8K_ZME, layer_info, vid_info);
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].link = 0;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_pix_fmt =
            hal_layer_transfer_pix_fmt(vid_info->fdr_info.data_type);
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_pix_fmt = VDP_PROC_FMT_SP_444;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_width = layer_info->crop_rect.w;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_height = layer_info->crop_rect.h;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_width = vid_info->vid_cfg.video_rect.w;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_height = vid_info->vid_cfg.video_rect.h;
    }
}
#endif

static hi_void hal_layer_get_zme_info(vdp_layer_vid layer_id, vdp_layer_info *layer_info, vdp_vid_info *vid_info)
{
    vid_info->fdr_info.draw_mode = VDP_FDR_DRAW_MODE_1;
    vid_info->fdr_info.draw_pixel_mode = VDP_FDR_DRAW_MODE_1;

    if (layer_id == VDP_LAYER_VID0) {
#ifdef VDP_SDK_PQ_SUPPORT
        hal_layer_zme_get_strategy_from_pq(layer_id, layer_info, vid_info);
#else
        if (layer_info->crop_rect.w < 256 || layer_info->crop_rect.h < 256) { /* 256 is number */
            hal_layer_zme_strategy_src64(layer_id, layer_info, vid_info);
        } else {
            hal_layer_zme_strategy_other(layer_id, layer_info, vid_info);
        }

        /* enable v0 8k zme fir */
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].h_lfir_en = HI_TRUE;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].h_cfir_en = HI_TRUE;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].v_lfir_en = HI_TRUE;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].v_cfir_en = HI_TRUE;
#endif
    } else if (layer_id == VDP_LAYER_VID1) {
        hal_layer_zme_strategy_init(VDP_ZME_8K_ZME, layer_info, vid_info);
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].link = 0;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_pix_fmt =
            hal_layer_transfer_pix_fmt(vid_info->fdr_info.data_type);
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_pix_fmt = VDP_PROC_FMT_SP_444;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_width = layer_info->crop_rect.w;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_height = layer_info->crop_rect.h;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_width = layer_info->crop_rect.w;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_height = layer_info->crop_rect.h;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_mode = 0;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_mode = 0;
    } else if (layer_id == VDP_LAYER_VID3) {
        hal_layer_zme_strategy_init(VDP_ZME_8K_ZME, layer_info, vid_info);
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].link = 0;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_pix_fmt =
            hal_layer_transfer_pix_fmt(vid_info->fdr_info.data_type);
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_pix_fmt = VDP_PROC_FMT_SP_444;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_width = layer_info->crop_rect.w;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_height = layer_info->crop_rect.h;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_width = layer_info->crop_rect.w;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_height = layer_info->crop_rect.h;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].in_mode = 0;
        vid_info->zme_info.image_info[VDP_ZME_8K_ZME].out_mode = 0;
    }
}

hi_void vdp_vid_update(vdp_vid_info *vid_info, vdp_vid_info *save_info, vdp_vid_update_info *update_info)
{
    if (osal_memncmp(&vid_info->fdr_info, sizeof(vdp_fdr_info), &save_info->fdr_info, sizeof(vdp_fdr_info)) != 0) {
        memcpy(&save_info->fdr_info, &vid_info->fdr_info, sizeof(vdp_fdr_info));
        update_info->fdr_info_update = HI_TRUE;
    }

    if (osal_memncmp(&vid_info->fdr_addr, sizeof(vdp_fdr_addr), &save_info->fdr_addr, sizeof(vdp_fdr_addr)) != 0) {
        memcpy(&save_info->fdr_addr, &vid_info->fdr_addr, sizeof(vdp_fdr_addr));
        update_info->fdr_addr_update = HI_TRUE;
    }

    if (osal_memncmp(&vid_info->vid_cfg, sizeof(vdp_vid_cfg), &save_info->vid_cfg, sizeof(vdp_vid_cfg)) != 0) {
        memcpy(&save_info->vid_cfg, &vid_info->vid_cfg, sizeof(vdp_vid_cfg));
        update_info->vid_cfg_update = HI_TRUE;
    }

    if (osal_memncmp(&vid_info->vp_cfg, sizeof(vdp_vp_cfg), &save_info->vp_cfg, sizeof(vdp_vp_cfg)) != 0) {
        memcpy(&save_info->vp_cfg, &vid_info->vp_cfg, sizeof(vdp_vp_cfg));
        update_info->vp_cfg_update = HI_TRUE;
    }

    if (osal_memncmp(&vid_info->zme_info, sizeof(vdp_zme_info), &save_info->zme_info, sizeof(vdp_zme_info)) != 0) {
        memcpy(&save_info->zme_info, &vid_info->zme_info, sizeof(vdp_zme_info));
        update_info->zme_info_update = HI_TRUE;
    }

    if (osal_memncmp(&vid_info->mac_info, sizeof(vdp_fdr_init_info), &save_info->mac_info,
                     sizeof(vdp_fdr_init_info)) != 0) {
        memcpy(&save_info->mac_info, &vid_info->mac_info, sizeof(vdp_fdr_init_info));
        update_info->mac_info_update = HI_TRUE;
    }

    if (osal_memncmp(&vid_info->csc_info, sizeof(vdp_csc_info), &save_info->csc_info, sizeof(vdp_csc_info)) != 0) {
        memcpy(&save_info->csc_info, &vid_info->csc_info, sizeof(vdp_csc_info));
        update_info->csc_info_update = HI_TRUE;
    }
}

hi_void vdp_vid_assert_fdr(vdp_layer_vid layer, vdp_layer_info *layer_info, vdp_vid_info *info)
{
    /* fdr */
    VDP_ASSERT(info->fdr_info.in_rect.w % 2 == 0);  /* 2 is number */
    VDP_ASSERT(info->fdr_info.in_rect.h % 2 == 0);  /* 2 is number */
    VDP_ASSERT(info->fdr_info.src_rect.w % 2 == 0); /* 2 is number */
    VDP_ASSERT(info->fdr_info.src_rect.h % 2 == 0); /* 2 is number */

    if (layer == VDP_LAYER_VID0) {
        VDP_ASSERT(info->fdr_info.src_rect.w >= info->fdr_info.in_rect.w + info->fdr_info.in_rect.x);
        VDP_ASSERT(info->fdr_info.src_rect.h >= info->fdr_info.in_rect.h + info->fdr_info.in_rect.y);
    }

    if ((info->fdr_info.in_fmt == VDP_FDR_VID_IFMT_SP_420) &&
        (info->fdr_info.rmode == VDP_FDR_RMODE_TOP || info->fdr_info.rmode == VDP_FDR_RMODE_BOTTOM)) {
        VDP_ASSERT(info->fdr_info.in_rect.y % 4 == 0); /* 4 is number */
        VDP_ASSERT(info->fdr_info.in_rect.h % 4 == 0); /* 4 is number */
    }

    if (info->fdr_info.dcmp_type == VDP_FDR_DCMP_TYPE_LINE) {
        VDP_ASSERT(info->fdr_info.src_rect.w <= 4096 && info->fdr_info.src_rect.h <= 2304); /* 4096, 2304 is number */
    }

    VDP_ASSERT(info->fdr_addr.addr[VDP_FDR_VID_ADDR_DATA].lum_str % 16 == 0); /* 16 is number */
    VDP_ASSERT(info->fdr_addr.addr[VDP_FDR_VID_ADDR_DATA].chm_str % 16 == 0); /* 16 is number */
    VDP_ASSERT(info->fdr_addr.addr[VDP_FDR_VID_ADDR_DATA].lum_str < 0x10000);
    VDP_ASSERT(info->fdr_addr.addr[VDP_FDR_VID_ADDR_DATA].chm_str < 0x10000);
    VDP_ASSERT(info->fdr_addr.addr[VDP_FDR_VID_ADDR_DATA_3D].lum_str < 0x10000);
    VDP_ASSERT(info->fdr_addr.addr[VDP_FDR_VID_ADDR_DATA_3D].chm_str < 0x10000);
    VDP_ASSERT(info->fdr_addr.addr[VDP_FDR_VID_ADDR_HEAD].lum_str < 0x10000);
    VDP_ASSERT(info->fdr_addr.addr[VDP_FDR_VID_ADDR_HEAD].chm_str < 0x10000);
}

hi_void vdp_vid_assert(vdp_layer_vid layer, vdp_layer_info *layer_info, vdp_vid_info *info)
{
    if (layer == VDP_LAYER_DC || info->vid_cfg.enable == HI_FALSE) {
        return;
    }

    vdp_vid_assert_fdr(layer, layer_info, info);

    /* vp */
    VDP_ASSERT(info->vp_cfg.vp0_rect.w <= 7680); /* 7680 is number */
    VDP_ASSERT(info->vp_cfg.vp0_rect.h <= 7680); /* 7680 is number */

    /* vid */
    VDP_ASSERT(info->vid_cfg.video_rect.x + info->vid_cfg.video_rect.w <=
               info->vid_cfg.disp_rect.x + info->vid_cfg.disp_rect.w);
    VDP_ASSERT(info->vid_cfg.video_rect.y + info->vid_cfg.video_rect.h <=
               info->vid_cfg.disp_rect.y + info->vid_cfg.disp_rect.h);
    VDP_ASSERT(info->vid_cfg.video_rect.w <= 7680); /* 7680 is number */
    VDP_ASSERT(info->vid_cfg.video_rect.h <= 7680); /* 7680 is number */
    VDP_ASSERT(info->vid_cfg.disp_rect.w <= 7680);  /* 7680 is number */
    VDP_ASSERT(info->vid_cfg.disp_rect.h <= 7680);  /* 7680 is number */

    /* zme */
    if (layer == VDP_LAYER_VID0 &&
        ((info->zme_info.image_info[VDP_ZME_8K_ZME].v_enable == HI_TRUE) ||
         (info->zme_info.image_info[VDP_ZME_8K_ZME].h_enable == HI_TRUE))) {
        VDP_ASSERT(info->zme_info.image_info[VDP_ZME_8K_ZME].in_width >= 256);  /* 256 is number */
        VDP_ASSERT(info->zme_info.image_info[VDP_ZME_8K_ZME].in_height >= 256); /* 256 is number */
    }

    /* v1 crop video disp must == vp */
    if ((layer == VDP_LAYER_VID1) && (info->vid_cfg.enable == HI_TRUE)) {
        VDP_ASSERT(info->vid_cfg.disp_rect.w == info->vp_cfg.vp0_rect.w);
        VDP_ASSERT(info->vid_cfg.disp_rect.h == info->vp_cfg.vp0_rect.h);
    }
}

hi_void vdp_vid_transfer(vdp_layer_vid layer_id, vdp_layer_info *layer_info, vdp_vid_info *vid_info)
{
    hi_drv_disp_color vp_letterbox = { 0 };

    if (layer_info->crop_rect.w == 0 && layer_info->crop_rect.h == 0) {
        layer_info->crop_rect = layer_info->frame_rect;
    }

    /* video disp rect */
    vid_info->vp_cfg.alpha = layer_info->vp_alpha;
    vid_info->vid_cfg.alpha = layer_info->vid_alpha;
    vid_info->vid_cfg.secure_en = layer_info->frame_secure_en;
    vid_info->vid_cfg.disp_rect = layer_info->vid_disp_rect;
    vid_info->vid_cfg.video_rect = layer_info->vid_video_rect;
    vid_info->vp_cfg.vp0_rect = layer_info->vp_rect;
    vid_info->vid_cfg.enable = layer_info->enable;

    if (vid_info->vid_cfg.enable == HI_FALSE) {
        return;
    }

    vdp_ip_csc_data_transfer(&vp_letterbox, layer_info->out_color_space, &vid_info->vp_cfg.bkg);
    vdp_ip_csc_data_transfer(&layer_info->letterbox_color, layer_info->out_color_space, &vid_info->vid_cfg.bkg);

    /* mac */
    vid_info->mac_info.pre_rd_en = HI_TRUE;
    vid_info->mac_info.ofl_master = 0;
    vid_info->mac_info.req_len = VDP_FDR_REQ_LENGTH_16;
    vid_info->mac_info.req_ctrl = VDP_FDR_REQ_CTRL_16BURST_1;

    /* csc */
    vid_info->csc_info.csc_disable_for_hdr = get_hdr_tran_process_type(layer_id, layer_info);
    vid_info->csc_info.i_mode = layer_info->frame_color_space;
    vid_info->csc_info.o_mode = layer_info->out_color_space;

    hal_layer_get_fdr_info(layer_id, layer_info, vid_info);
    hal_layer_get_zme_info(layer_id, layer_info, vid_info);
}

hi_void vdp_vid_set_layer(hi_u32 layer, vdp_vid_info *vid_info, vdp_vid_update_info *update_info)
{
    if (update_info->vid_cfg_update == HI_TRUE) {
        vdp_ip_vid_set_cfg(layer, &vid_info->vid_cfg);
        if (vid_info->vid_cfg.enable == HI_FALSE) {
            vdp_vid_regup();
            return;
        }
    }

    if (update_info->fdr_info_update == HI_TRUE) {
        vdp_ip_fdr_set(layer, &vid_info->fdr_info);
    }

    if (update_info->fdr_addr_update == HI_TRUE) {
        vdp_ip_fdr_addr(layer, &vid_info->fdr_addr, &vid_info->fdr_info);
    }

    if (update_info->mac_info_update == HI_TRUE) {
        vdp_ip_fdr_mac_init(layer, &vid_info->mac_info);
    }

    if (update_info->zme_info_update == HI_TRUE) {
        vdp_ip_zme_set(layer, &vid_info->zme_info);
    }

    vdp_ip_hdr_set(layer, HI_NULL);

    if (update_info->vp_cfg_update == HI_TRUE) {
        if ((layer == VDP_LAYER_VID0) ||
            (layer == VDP_LAYER_VID1)) {
            vdp_ip_vp_set_cfg(VDP_LAYER_VP0, &vid_info->vp_cfg);
        }
    }

    if (vid_info->vid_cfg.enable == HI_TRUE) {
        if (layer == VDP_LAYER_VID0) {
            if (vid_info->csc_info.csc_disable_for_hdr == HI_TRUE) {
                vdp_ip_csc_disable(VDP_CSC_ID_VID0);
            } else {
                vdp_ip_csc_set(VDP_CSC_ID_VID0, vid_info->csc_info.i_mode, vid_info->csc_info.o_mode);
            }
        } else if (layer == VDP_LAYER_VID3) {
            vdp_ip_csc_set(VDP_CSC_ID_VID3, vid_info->csc_info.i_mode, vid_info->csc_info.o_mode);
        }
    }

    vdp_vid_regup();

    return;
}

hi_void vdp_vid_regup(hi_void)
{
    vdp_ip_vid_regup();
}

hi_bool vdp_vid_get_mark_status(hi_u32 layer)
{
    return vdp_ip_vid_get_mark_status(layer);
}
