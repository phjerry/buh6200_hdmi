/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */

#include "dbg.h"
#include "vpp.h"
#include "hi_osal.h"
#include "hi_drv_sys.h"
#ifdef VFMW_FENCE_SUPPORT
#include "fence.h"
#endif

/* -------------------------------LOCAL MACRO---------------------------------- */
#define VPP_TASK_NAME            "HI_VFMW_VPP"
#define PP_MAX_FRAME_RATE        (120 * 1000)
#define PP_DEFAULT_FRAME_RATE    (25 * 1000)
#define PP_MAX_FAILED_TIME       5
#define USERDATA_IDENTIFIER_DVB1 0x34394147
#define USERDATA_IDENTIFIER_AFD  0x31475444
#define USERDATA_TYPE_DVB1_CC    0x03
#define USERDATA_TYPE_DVB1_BAR   0x06
#define USERDATA_TYPE_INDEX      4

#define VPP_REPORT_NEW_FRAME(chan, chan_id, frame, size)                      do {                                                                   \
    if ((chan)->event_report != HI_NULL && ((((chan)->evt_map) & EVNT_NEW_YUV) != 0)) { \
        (chan)->event_report ((chan_id), EVNT_NEW_YUV, (hi_void *)(frame), size); \
    }                                                                  \
} while (0)

#define VPP_REPORT_RECEIVE_FAILED(chan, chan_id)                          do {                                                                  \
    if ((chan)->event_report != HI_NULL && ((((chan)->evt_map) & EVNT_RECEIVE_FAILED) != 0)) { \
        (chan)->event_report((chan_id), EVNT_RECEIVE_FAILED, HI_NULL, 0); \
    }                                                                 \
} while (0)

#define VPP_REPORT_NORM_CHG(chan, chan_id, norm, size) do { \
    if ((chan)->event_report != HI_NULL && ((((chan)->evt_map) & EVNT_NORM_CHG) != 0)) { \
        (chan)->event_report ((chan_id), EVNT_NORM_CHG, (hi_void *)(norm), size); \
    } \
} while (0)

#define VPP_REPORT_FRM_PCK_TYPE_CHG(chan, chan_id, frm_pck_type, size) do { \
    if ((chan)->event_report != HI_NULL && ((((chan)->evt_map) & EVNT_FRM_PCK_CHG) != 0)) { \
        (chan)->event_report ((chan_id), EVNT_FRM_PCK_CHG, (hi_void *)(frm_pck_type), size); \
    } \
} while (0)


/* -----------------------------LOCAL DEFINITION------------------------------- */
static vpp_entry g_vpp_entry = {0};
static const hi_u32 g_type_user_table[][2] = { /* 2 is array size */
    { SPYCbCr420,      HI_DRV_PIXEL_FMT_NV21 },
    { SPYCbCr400,      HI_DRV_PIXEL_FMT_NV08},
    { SPYCbCr411,      HI_DRV_PIXEL_FMT_NV12_411},
    { SPYCbCr422_1X2,  HI_DRV_PIXEL_FMT_NV16},
    { SPYCbCr422_2X1,  HI_DRV_PIXEL_FMT_NV16_2X1},
    { SPYCbCr444,      HI_DRV_PIXEL_FMT_NV24},
    { PLNYCbCr400,     HI_DRV_PIXEL_FMT_YUV400 },
    { PLNYCbCr411,     HI_DRV_PIXEL_FMT_YUV411 },
    { PLNYCbCr420,     HI_DRV_PIXEL_FMT_YUV420P },
    { PLNYCbCr422_1X2, HI_DRV_PIXEL_FMT_YUV422_1X2 },
    { PLNYCbCr422_2X1, HI_DRV_PIXEL_FMT_YUV422_2X1 },
    { PLNYCbCr444,     HI_DRV_PIXEL_FMT_YUV_444 },
    { PLNYCbCr410,     HI_DRV_PIXEL_FMT_YUV410P },

    { VFMW_YUV_FMT_MAX, HI_DRV_PIXEL_MAX },  /* terminal element n */
};

/* ----------------------------INTERNAL FUNCTION------------------------------- */
vpp_entry *vpp_get_entry(hi_void)
{
    return &g_vpp_entry;
}

vpp_chan *vpp_get_chan(hi_u32 chan_id)
{
    vpp_entry *entry = HI_NULL;
    vpp_chan *chan = HI_NULL;

    entry = vpp_get_entry();
    chan = entry->chan[chan_id];

    return chan;
}

static hi_void vpp_sema_down(hi_void)
{
    vpp_entry *entry = vpp_get_entry();
    OS_SEMA_DOWN(entry->sema);
}

static hi_void vpp_sema_up(hi_void)
{
    vpp_entry *entry = vpp_get_entry();
    OS_SEMA_UP(entry->sema);
}

static hi_void vpp_print_frame_info(hi_drv_video_frame *frame)
{
    hi_s32 i = 0;;

    dprint(PRN_ALWS, "\n------------------------- print video info --------------------------\n\n");

    dprint(PRN_ALWS, " index       :%-10d\n", frame->frame_index);
    for (i = 0; i < HI_DRV_3D_EYE_MAX; i++) {
        dprint(PRN_ALWS, " PhyAddr[%d]:\n"
               " phy Y head    :0x%-10x     phy Y     :0x%-10x    stride Y :%-10d    phy C head   :0x%-10x    "
               "phy C       :0x%-10x \n",
               i, frame->buf_addr[i].start_addr, frame->buf_addr[i].y_offset,
               frame->buf_addr[i].stride_y,
               frame->buf_addr[i].cr_offset, frame->buf_addr[i].c_offset);
    }

    for (i = 0; i < HI_DRV_3D_EYE_MAX; i++) {
        dprint(PRN_ALWS, " phy_addr_lb[%d]:\n"
               " phy Y head   :%-10d     phy Y        :0x%-10x    stride Y   :%-10d    phy C head   :0x%-10x    "
               "phy C        :0x%-10x \n",
               i, frame->buf_addr_lb[i].start_addr, frame->buf_addr_lb[i].y_offset,
               frame->buf_addr_lb[i].stride_y,
               frame->buf_addr_lb[i].c_head_offset, frame->buf_addr_lb[i].c_offset);
    }

    dprint(PRN_ALWS, "\n");
    dprint(PRN_ALWS, " tunnel addr  :0x%-10x   back tunnel addr  :0x%-10x    tunnel handle      :0x%-10x   \n",
           frame->tunnel_phy_addr, frame->back_tunnel_phy_addr, frame->tunnel_handle);

    dprint(PRN_ALWS,
           " width       :%-10d      height       :%-10d     disp ctrl width    :%-10d    disp ctrl height    :%-10d\n",
           frame->width, frame->height, frame->disp_ctrl_width, frame->disp_ctrl_height);

    dprint(PRN_ALWS,
           " dv_dual_layer   :%-10d     single_i_frame  :%-10d    src_pts   :%-10d    pts:%-10d\n",
           frame->dv_dual_layer, frame->single_i_frame, frame->src_pts, frame->pts);

    dprint(PRN_ALWS,
           " discard     :%-10d     aspect width       :%-10d    aspect height  :%-10d\n",
           frame->discard, frame->aspect_width, frame->aspect_height);

    dprint(PRN_ALWS,
           " frame rate       :%-10d     ori frame rate  :%-10d\n",
           frame->frame_rate, frame->ori_frame_rate);

    dprint(PRN_ALWS,
           " pixel format:%-10d     sample type  :%-10d    field mode:%-10d    buf valid mode   :%-10d      "
           "top field first         :%-10d    repeat first field:%-10d\n",
           frame->pixel_format, frame->sample_type, frame->field_mode, frame->buf_valid_mode, frame->top_field_first,
           frame->repeat_first_field);

    dprint(PRN_ALWS,
           " compressed       :%-10d     bit width :%-10d\n", frame->compressd, frame->bit_width);

    dprint(PRN_ALWS,
           " rect x:%-10d     rect y  :%-10d    rect w:%-10d    rect h   :%-10d\n",
           frame->disp_rect.rect_x, frame->disp_rect.rect_y, frame->disp_rect.rect_w, frame->disp_rect.rect_h);

    dprint(PRN_ALWS,
        " 3d type :%-10d     error level :%-10d  secure:%-10d  codec type: 0x%-10x color space:%-10d afd type:%-10d\n",
        frame->video_3d_type, frame->error_level,
        frame->secure, frame->codec_type, frame->color_space, frame->afd_format);

    dprint(PRN_ALWS, " source :%-10d     last flag :0x%-10x  color sys:%-10d\n",
           frame->video_private.source, frame->video_private.last_flag, frame->video_private.color_sys);

    dprint(PRN_ALWS, " cmp type :%-10d     ycmp rate :%-10d  ccmp rate:%-10d data fmt:%-10d\n",
           frame->cmp_info.cmp_mode, frame->cmp_info.ycmp_rate, frame->cmp_info.ccmp_rate, frame->cmp_info.data_fmt);

    dprint(PRN_ALWS, "---------------------------------------------------------------\n\n");
    OS_MSLEEP(15); /* sleep 15 ms */

    return;
}

static hi_u32 vpp_format_convert(hi_u32 code)
{
    hi_u32 index = 0;
    hi_u32 converted_code = HI_DRV_PIXEL_FMT_NV21;

    while (1) {
        if (g_type_user_table[index][0] == VFMW_YUV_FMT_MAX || g_type_user_table[index][1] == HI_DRV_PIXEL_MAX) {
            break;
        }

        if (code == g_type_user_table[index][0]) {
            converted_code = g_type_user_table[index][1];
            break;
        }

        index++;
    }

    return converted_code;
}

static hi_void vpp_get_pix_format(vfmw_image *image, hi_drv_video_frame *frame)
{
    frame->pixel_format = vpp_format_convert(image->yuv_format);
}

static hi_void vpp_get_cmp_info(vfmw_image *image, hi_drv_video_frame *frame)
{
    frame->compressd = image->disp_info.compress_en;

    if (image->disp_info.compress_en == 0) {
        frame->cmp_info.cmp_mode = HI_DRV_COMPRESS_MODE_OFF;
    } else {
        frame->cmp_info.cmp_mode = HI_DRV_COMPRESS_MODE_SEG_LOSSLESS;
    }

    if (image->disp_info.linear_en == 1) {
        frame->cmp_info.data_fmt = HI_DRV_DATA_FMT_LINER;
    } else {
        frame->cmp_info.data_fmt = HI_DRV_DATA_FMT_TILE;
    }

    return;
}

static hi_void vpp_get_sample_type(vfmw_image *image, hi_drv_video_frame *frame)
{
    switch (image->format.source_format) {
        case 0: /* 0 is PROGRESSIVE in vfmw */
            frame->sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
            break;
        case 1: /* 1 is INTERLACE in vfmw */
            frame->sample_type = HI_DRV_SAMPLE_TYPE_INTERLACE;
            break;
        default:
            frame->sample_type = HI_DRV_SAMPLE_TYPE_UNKNOWN;
            break;
    }

    return;
}

static hi_void vpp_get_field_mode(vfmw_image *image, hi_drv_video_frame *frame)
{
    switch (image->format.field_valid_flag) {
        case 1: /* 1 :a number */
            frame->field_mode = HI_DRV_FIELD_TOP;
            break;
        case 2: /* 2 :a number */
            frame->field_mode = HI_DRV_FIELD_BOTTOM;
            break;
        case 3: /* 3 :a number */
            frame->field_mode = HI_DRV_FIELD_ALL;
            break;
        default:
            frame->field_mode = HI_DRV_FIELD_MAX;
            break;
    }

    return;
}


static hi_void vpp_get_buffer_mode(vfmw_image *image, hi_drv_video_frame *frame)
{
    /* c_ncomment: VPSSЭ̴ : h265  tt/bbtbt/btb ָ buff mode ͳһALL */
    if (image->vid_std == VFMW_HEVC || image->vid_std == VFMW_AVS2) {
        frame->buf_valid_mode = HI_DRV_FIELD_ALL;
    } else if (image->vid_std == VFMW_H264) {
        switch (frame->field_mode) {
            case HI_DRV_FIELD_TOP:
                frame->buf_valid_mode = HI_DRV_FIELD_TOP;
                break;

            case HI_DRV_FIELD_BOTTOM:
                frame->buf_valid_mode = HI_DRV_FIELD_BOTTOM;
                break;

            case HI_DRV_FIELD_ALL:
                frame->buf_valid_mode = HI_DRV_FIELD_ALL;
                break;

            default:
                frame->buf_valid_mode = HI_DRV_FIELD_ALL;
                break;
        }
    }

    return;
}

static hi_void vpp_get_bit_width(vfmw_image *image, hi_drv_video_frame *frame)
{
    if (image->bit_depth == 10) { /* 10 :a number */
        frame->bit_width = HI_DRV_PIXEL_BITWIDTH_10BIT;
    } else if (image->bit_depth == 12) { /* 12 :a number */
        frame->bit_width = HI_DRV_PIXEL_BITWIDTH_12BIT;
    } else {
        frame->bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
    }

    return;
}

static hi_u64 vpp_get_buf_handle(vpp_chan *chan, image_disp *disp_info, hi_bool right)
{
    hi_u64 fd;
    hi_u64 dma_buf;

    if (chan->alloc_type == VFMW_FRM_ALLOC_OUTER) {
        fd = (right == HI_TRUE) ?  disp_info->frm_mem_fd_1 : disp_info->frm_mem_fd;
    } else {
        dma_buf = (right == HI_TRUE) ? disp_info->frm_dma_buf_1 : disp_info->frm_dma_buf;
        fd = OS_GET_MEM_FD(UINT64_PTR(dma_buf));
    }

    return fd;
}

static hi_void vpp_get_hdr_fd(vfmw_image *image, hi_drv_video_frame *frame, hi_mem_handle_t *hdr_mtdt_fd)
{
    (*hdr_mtdt_fd) = HI_INVALID_HANDLE;

    switch (frame->codec_type) {
        case HI_DRV_VCODEC_H264:
            if (frame->hdr_type == HI_DRV_HDR_TYPE_DOLBYVISION && image->hdr_input.hdr_dma_buf != 0) {
                frame->hdr_info.dolby_info.metadata.mem_addr.mem_handle =
                    OS_GET_MEM_FD(UINT64_PTR(image->hdr_input.hdr_dma_buf));
                frame->hdr_info.dolby_info.metadata.mem_addr.addr_offset = 0;
                (*hdr_mtdt_fd) = frame->hdr_info.dolby_info.metadata.mem_addr.mem_handle;
            }
            break;
        case HI_DRV_VCODEC_HEVC:
            if (frame->hdr_type == HI_DRV_HDR_TYPE_DOLBYVISION && image->hdr_input.hdr_dma_buf != 0) {
                frame->hdr_info.dolby_info.metadata.mem_addr.mem_handle =
                    OS_GET_MEM_FD(UINT64_PTR(image->hdr_input.hdr_dma_buf));
                frame->hdr_info.dolby_info.metadata.mem_addr.addr_offset = 0;
                (*hdr_mtdt_fd) = frame->hdr_info.dolby_info.metadata.mem_addr.mem_handle;
            }

            if (frame->hdr_type == HI_DRV_HDR_TYPE_HDR10PLUS &&
                frame->hdr_info.hdr10_plus_info.dynamic_metadata_available == HI_TRUE &&
                image->hdr_input.hdr_dma_buf != 0) {
                frame->hdr_info.hdr10_plus_info.dynamic_metadata.mem_addr.mem_handle =
                    OS_GET_MEM_FD(UINT64_PTR(image->hdr_input.hdr_dma_buf));
                frame->hdr_info.hdr10_plus_info.dynamic_metadata.mem_addr.addr_offset = 0;
                (*hdr_mtdt_fd) = frame->hdr_info.hdr10_plus_info.dynamic_metadata.mem_addr.mem_handle;
            }
            break;
        case HI_DRV_VCODEC_AVS2:
        case HI_DRV_VCODEC_AVS3:
            if (frame->hdr_type == HI_DRV_HDR_TYPE_JTP_SL_HDR &&
                frame->hdr_info.sl_hdr_info.dynamic_metadata_available == HI_TRUE &&
                image->hdr_input.hdr_dma_buf != 0) {
                frame->hdr_info.sl_hdr_info.dynamic_metadata.mem_addr.mem_handle =
                    OS_GET_MEM_FD(UINT64_PTR(image->hdr_input.hdr_dma_buf));
                frame->hdr_info.sl_hdr_info.dynamic_metadata.mem_addr.addr_offset = 0;
                (*hdr_mtdt_fd) = frame->hdr_info.sl_hdr_info.dynamic_metadata.mem_addr.mem_handle;
            }
            break;
        default:
            break;
    }

    return ;
}

#if 0
static hi_void vpp_put_hdr_fd(hi_drv_video_frame *frame)
{
    switch (frame->codec_type) {
        case HI_DRV_VCODEC_H264:
            if (frame->hdr_type == HI_DRV_HDR_TYPE_DOLBYVISION) {
                OS_PUT_MEM_FD(frame->hdr_info.dolby_info.metadata.mem_addr.mem_handle);
            }
            break;
        case HI_DRV_VCODEC_HEVC:
            if (frame->hdr_type == HI_DRV_HDR_TYPE_DOLBYVISION) {
                OS_PUT_MEM_FD(frame->hdr_info.dolby_info.metadata.mem_addr.mem_handle);
            }

            if (frame->hdr_type == HI_DRV_HDR_TYPE_HDR10PLUS &&
                frame->hdr_info.hdr10_plus_info.dynamic_metadata_available == HI_TRUE) {
                OS_PUT_MEM_FD(frame->hdr_info.hdr10_plus_info.dynamic_metadata.mem_addr.mem_handle);
            }
            break;
        case HI_DRV_VCODEC_AVS2:
        case HI_DRV_VCODEC_AVS3:
            if (frame->hdr_type == HI_DRV_HDR_TYPE_JTP_SL_HDR &&
                frame->hdr_info.sl_hdr_info.dynamic_metadata_available == HI_TRUE) {
                OS_PUT_MEM_FD(frame->hdr_info.sl_hdr_info.dynamic_metadata.mem_addr.mem_handle);
            }
            break;
        default:
            break;
    }

    return ;
}
#endif

static hi_void vpp_get_buf_addr(vfmw_image *image, hi_drv_video_frame *frame, vpp_chan *chan,
                                hi_mem_handle_t *mtdt_fd)
{
    frame->buf_addr[0].start_addr = vpp_get_buf_handle(chan, &image->disp_info, HI_FALSE);
    frame->buf_addr[0].y_head_offset = 0;
    frame->buf_addr[0].y_offset = frame->buf_addr[0].y_head_offset;
    frame->buf_addr[0].stride_y = image->disp_info.y_stride;
    frame->buf_addr[0].c_head_offset = image->disp_info.chrom_phy_addr - image->disp_info.luma_phy_addr;
    frame->buf_addr[0].c_offset = frame->buf_addr[0].c_head_offset;
    frame->buf_addr[0].stride_c = frame->buf_addr[0].stride_y;

    if (frame->bit_width == HI_DRV_PIXEL_BITWIDTH_10BIT) {
        frame->buf_addr_lb[0].start_addr = frame->buf_addr[0].start_addr;
        frame->buf_addr_lb[0].y_offset = image->disp_info.luma_phy_addr_2bit -
                                         image->disp_info.luma_phy_addr;
        frame->buf_addr_lb[0].stride_y  = image->disp_info.stride_2bit;
        frame->buf_addr_lb[0].c_offset = image->disp_info.chrom_phy_addr_2bit -
                                         image->disp_info.luma_phy_addr;
        frame->buf_addr_lb[0].stride_c = image->disp_info.stride_2bit;
    }

    if (image->disp_info.compress_en != 0) {
        frame->buf_addr[0].y_offset = image->disp_info.head_info_size;
        frame->buf_addr[0].c_offset += image->disp_info.head_info_size / 2; /* divided by 2 */
        frame->buf_addr[0].head_stride = image->disp_info.head_stride;
        frame->buf_addr[0].head_size = image->disp_info.head_info_size;
    }

    if (image->is_3d) {
        frame->buf_addr[1].start_addr = vpp_get_buf_handle(chan, &image->disp_info, HI_TRUE);
        frame->buf_addr[1].y_head_offset = 0;
        frame->buf_addr[1].y_offset = frame->buf_addr[1].y_head_offset;
        frame->buf_addr[1].stride_y = image->disp_info.y_stride;

        frame->buf_addr[1].c_head_offset =
            image->disp_info.chrom_phy_addr_1 - image->disp_info.luma_phy_addr_1;
        frame->buf_addr[1].c_offset = frame->buf_addr[1].c_head_offset;
        frame->buf_addr[1].stride_c = frame->buf_addr[1].stride_y;

        if (image->disp_info.compress_en != 0) {
            frame->buf_addr[1].y_offset = image->disp_info.head_info_size;
            frame->buf_addr[1].c_offset += image->disp_info.head_info_size / 2; /* divided by 2 */
        }
    }

    vpp_get_hdr_fd(image, frame, mtdt_fd);

    return;
}

static hi_void vpp_get_3d_type(vpp_chan *chan, hi_drv_video_frame *frame, vfmw_image *image)
{
    vfmw_vid_std vid_std = image->vid_std;

    if (vid_std == VFMW_MVC) {
        switch (image->frm_pack_type) {
            case FRAME_PACKING_TYPE_NONE:
                frame->video_3d_type = HI_DRV_3D_NONE;
                break;

            case FRAME_PACKING_TYPE_SIDE_BY_SIDE:
                frame->video_3d_type = HI_DRV_3D_SBS_HALF;
                break;

            case FRAME_PACKING_TYPE_TOP_BOTTOM:
                frame->video_3d_type = HI_DRV_3D_TAB;
                break;

            case FRAME_PACKING_TYPE_TIME_INTERLACED:
                frame->video_3d_type = HI_DRV_3D_FPK;
                break;

            default:
                frame->video_3d_type = HI_DRV_3D_MAX;
                break;
        }

        if (chan->ext_3d_type != HI_DRV_3D_MAX) {
            frame->video_3d_type = chan->ext_3d_type;
        }
    }

    return;
}

static hi_void vpp_convert_codec_type(vfmw_vid_std vid_std, hi_drv_video_frame *frame)
{
    switch (vid_std) {
        case VFMW_H264:
            frame->codec_type = HI_DRV_VCODEC_H264;
            break;
        case VFMW_HEVC:
            frame->codec_type = HI_DRV_VCODEC_HEVC;
            break;
        case VFMW_H263:
            frame->codec_type = HI_DRV_VCODEC_H263;
            break;
        case VFMW_VC1:
            frame->codec_type = HI_DRV_VCODEC_VC1;
            break;
        case VFMW_MPEG4:
            frame->codec_type = HI_DRV_VCODEC_MPEG4;
            break;
        case VFMW_MPEG2:
            frame->codec_type = HI_DRV_VCODEC_MPEG2;
            break;
        case VFMW_DIVX3:
            frame->codec_type = HI_DRV_VCODEC_DIVX3;
            break;
        case VFMW_AVS:
            frame->codec_type = HI_DRV_VCODEC_AVS;
            break;
        case VFMW_JPEG:
            frame->codec_type = HI_DRV_VCODEC_JPEG;
            break;
        case VFMW_REAL8:
            frame->codec_type = HI_DRV_VCODEC_REAL8;
            break;
        case VFMW_REAL9:
            frame->codec_type = HI_DRV_VCODEC_REAL9;
            break;
        case VFMW_VP6:
            frame->codec_type = HI_DRV_VCODEC_VP6;
            break;
        case VFMW_VP6F:
            frame->codec_type = HI_DRV_VCODEC_VP6F;
            break;
        case VFMW_VP6A:
            frame->codec_type = HI_DRV_VCODEC_VP6A;
            break;
        case VFMW_VP8:
            frame->codec_type = HI_DRV_VCODEC_VP8;
            break;
        case VFMW_SORENSON:
            frame->codec_type = HI_DRV_VCODEC_SORENSON;
            break;
        case VFMW_MVC:
            frame->codec_type = HI_DRV_VCODEC_MVC;
            break;
        case VFMW_VP9:
            frame->codec_type = HI_DRV_VCODEC_VP9;
            break;
        case VFMW_AVS2:
            frame->codec_type = HI_DRV_VCODEC_AVS2;
            break;
        case VFMW_AV1:
            frame->codec_type = HI_DRV_VCODEC_AV1;
            break;
        case VFMW_AVS3:
            frame->codec_type = HI_DRV_VCODEC_AVS3;
            break;
        case VFMW_MJPEG:
            frame->codec_type = HI_DRV_VCODEC_MJPEG;
            break;
        default:
            frame->codec_type = HI_DRV_VCODEC_H264;
            dprint(PRN_ERROR, "%s: vid_std:%d unkonw!\n", __func__, vid_std);

            break;
    }

    return;
}

static hi_void vpp_get_uvmos(vfmw_image *image, hi_vdec_ext_frm_info *ext_info)
{
    ext_info->uvmos_info.avg_mv = image->uvmos_info.avg_mv;
    ext_info->uvmos_info.avg_qp = image->uvmos_info.avg_qp;
    ext_info->uvmos_info.frame_stream_size = image->uvmos_info.frame_stream_size;
    ext_info->uvmos_info.max_mv = image->uvmos_info.max_mv;
    ext_info->uvmos_info.min_mv = image->uvmos_info.min_mv;
    ext_info->uvmos_info.skip_ratio = image->uvmos_info.skip_ratio;

    switch (image->format.frame_type) {
        case 0: /* 0 is I FRM */
            ext_info->uvmos_info.frm_type = HI_VDEC_I_FRM;
            break;
        case 1: /* 1 is P FRM */
            ext_info->uvmos_info.frm_type = HI_VDEC_P_FRM;
            break;
        case 2: /* 2 is B FRM */
            ext_info->uvmos_info.frm_type = HI_VDEC_B_FRM;
            break;
        default:
            ext_info->uvmos_info.frm_type = HI_VDEC_FRM_TYPE_MAX;
            break;
    }
}

static hi_void vpp_get_usd(vfmw_image *image, hi_vdec_ext_frm_info *ext_info)
{
    hi_s32 i = 0;

    ext_info->usd_num = 0;

    for (i = 0; i < VFMW_MAX_USD_NUM; i++) {
        if (image->usrdat[i] == 0) {
            continue;
        }
        ext_info->usd_num++;
    }
}

static hi_void vpp_get_color_space_default(vfmw_image *image, hi_drv_video_frame *frame)
{
    if (frame->color_space.color_primary == HI_DRV_COLOR_PRIMARY_UNSPECIFY ||
        frame->color_space.matrix_coef == HI_DRV_COLOR_MATRIX_COEFFS_IDENTITY) {
        if (image->image_width >= 1280 || image->image_height >= 720) { /* 1280 720 is width and height threshold */
            frame->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
            frame->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
        } else {
            frame->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT601_525;
            frame->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT601_525;
        }
    }

    return;
}

static hi_s32 vpp_check_frame(hi_drv_video_frame *frame)
{
    if (frame->width == 0 || frame->height == 0) {
        dprint(PRN_ERROR, "%s err, width:%d height:%d\n", __func__, frame->width, frame->height);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void vpp_get_private_frame_info(vfmw_image *image, vfmw_private_frm_info *private_info,
                                          hi_mem_handle_t mtdt_fd)
{
    private_info->image_id = image->image_id;
    private_info->image_id_1 = image->image_id_1;
    private_info->gop_num = image->gop_num;
    private_info->frm_type = image->format.frame_type;
    private_info->top_field_frame_format = image->top_fld_type & 0x3;
    private_info->bot_field_frame_format = image->bottom_fld_type & 0x3;
    private_info->field_flag = image->is_fld_flag;
    private_info->mtdt_mem_handle = mtdt_fd;
    VFMW_CHECK_SEC_FUNC(memcpy_s(&private_info->color_desc, sizeof(vfmw_color_desc),
                                 &image->hdr_input.colour_info, sizeof(vfmw_color_desc)));
}

static hi_void vpp_get_vc1_range_info(vfmw_image *image, hi_drv_video_frame *frame)
{
    vfmw_vc1_range *src_range = HI_NULL;
    hi_drv_vc1_range_info *dst_range = HI_NULL;

    if (image->vid_std == VFMW_VC1) {
        src_range = &image->vc1_range;
        dst_range = &frame->video_private.vc1_range;

        dst_range->pic_structure = src_range->pic_structure;
        dst_range->pic_qp_enable = src_range->pic_qp_enable;
        dst_range->chroma_format_idc = src_range->chroma_format_idc;
        dst_range->vc1_profile = src_range->vc1_profile;

        dst_range->qp_y = src_range->qp_y;
        dst_range->qp_u = src_range->qp_u;
        dst_range->qp_v = src_range->qp_v;
        dst_range->ranged_frm = src_range->ranged_frm;

        dst_range->range_mapy_flag = src_range->range_mapy_flag;
        dst_range->range_mapy = src_range->range_mapy;
        dst_range->range_mapuv_flag = src_range->range_mapuv_flag;
        dst_range->range_mapuv = src_range->range_mapuv;

        dst_range->bottom_range_mapy_flag = src_range->bottom_range_mapy_flag;
        dst_range->bottom_range_mapy = src_range->bottom_range_mapy;
        dst_range->bottom_range_mapuv_flag = src_range->bottom_range_mapuv_flag;
        dst_range->bottom_range_mapuv = src_range->bottom_range_mapuv;
    }
}

static hi_vdec_norm_type vpp_get_disp_norm(hi_u32 video_fmt)
{
    switch (video_fmt) {
        case 1: /* 1 is PAL in vfmw */
            return HI_VDEC_NORM_PAL;
        case 2: /* 2 is NTSC in vfmw */
            return HI_VDEC_NORM_NTSC;
        default:
            return HI_VDEC_NORM_MAX;
    }
}

static hi_void vpp_report_norm_info(vpp_chan *chan, vfmw_image *image)
{
    hi_vdec_norm_param norm_param;
    norm_param.norm_type = vpp_get_disp_norm(image->format.video_format);
    norm_param.width = image->disp_info.disp_width;
    norm_param.height = image->disp_info.disp_height;
    norm_param.progressive =
        (image->format.source_format == 0 || image->format.source_format == 2) ? /* 0 2 is progressive */
        HI_TRUE : HI_FALSE;
    if (norm_param.height <= 288) { /* 288 is image height */
        norm_param.progressive = HI_TRUE;
    }
    norm_param.frm_rate = 25000; /* 25000 is default frame rate */
    VPP_REPORT_NORM_CHG(chan, chan->chan_id, &norm_param, sizeof(hi_vdec_norm_param));
}

static vfmw_private_frm_info *vpp_get_private_node(vpp_chan *chan)
{
    hi_s32 i;
    vpp_private_block *block = HI_NULL;

    for (i = 0; i < VPP_MAX_PRIVATE_NODE_NUM; i++) {
        block = chan->vpp_private_node[i];
        if (block == HI_NULL) {
            break;
        }

        if (block->is_use == HI_FALSE) {
            block->is_use = HI_TRUE;
            return &block->vdec_private;
        }
    }

    if (i < VPP_MAX_PRIVATE_NODE_NUM) {
        block = OS_ALLOC_VIR("vpp_private_info", sizeof(vpp_private_block));
        if (block == HI_NULL) {
            dprint(PRN_ERROR, "%s:%d alloc vir mem fail!\n", __func__, __LINE__);
            return HI_NULL;
        }

        chan->vpp_private_node[i] = block;
        block->is_use = HI_TRUE;
        return &block->vdec_private;
    }

    return HI_NULL;
}

static hi_void vpp_put_private_node(vpp_chan *chan, vfmw_private_frm_info *vdec_private)
{
    hi_s32 i;
    vpp_private_block *block = HI_NULL;

    for (i = 0; i < VPP_MAX_PRIVATE_NODE_NUM; i++) {
        block = chan->vpp_private_node[i];
        if (block != HI_NULL && (&block->vdec_private) == vdec_private) {
            block->is_use = HI_FALSE;
            break;
        }
    }
}

static hi_void vpp_free_all_private_node(vpp_chan *chan)
{
    hi_s32 i;
    vpp_private_block *block = HI_NULL;

    for (i = 0; i < VPP_MAX_PRIVATE_NODE_NUM; i++) {
        block = chan->vpp_private_node[i];
        if (block != HI_NULL) {
            OS_FREE_VIR(block);
            chan->vpp_private_node[i] = HI_NULL;
        }
    }
}

static hi_void vpp_get_frame_common_info(vpp_chan *chan, vfmw_image *image, hi_drv_video_frame *frame)
{
    frame->width = image->disp_info.disp_width;
    frame->height = image->disp_info.disp_height;
    frame->disp_ctrl_width = image->disp_ctrl_width;
    frame->disp_ctrl_height = image->disp_ctrl_height;
    frame->disp_rect.rect_w = image->disp_info.disp_width;
    frame->disp_rect.rect_h = image->disp_info.disp_height;
    frame->afd_format = (image->afd.active_format_flag != 0) ?
                        (hi_drv_video_afd_type)(image->afd.active_format) : HI_DRV_AFD_TYPE_UNKNOW;
    frame->disp_rect.rect_x = 0;
    frame->disp_rect.rect_y = 0;
    frame->error_level = image->error_level;
    frame->src_pts = image->src_pts;
    frame->pts = image->pts;
    frame->dv_dual_layer = image->dv_dual_layer;
    frame->frame_index = image->seq_img_cnt;
    frame->discard = (image->error_level > chan->err_thr) ? HI_TRUE : HI_FALSE;
    frame->aspect_width = image->aspect_width;
    frame->aspect_height = image->aspect_height;
    frame->frame_rate = image->frame_rate <= PP_MAX_FRAME_RATE ? image->frame_rate : PP_MAX_FRAME_RATE;
    frame->tunnel_phy_addr = image->line_num_phy_addr;
    frame->back_tunnel_phy_addr = image->dec_info.back_tunnel_line_num_phy_addr;
    frame->secure = image->is_sec_frm ? HI_TRUE : HI_FALSE;
    frame->top_field_first = image->format.top_field_first;
    frame->repeat_first_field = image->format.repeat_first_filed;
    frame->single_i_frame = (chan->dec_mode == VFMW_DEC_MODE_I) ? 1 : 0;
}

static hi_s32 vpp_image_2_frame(vpp_chan *chan, vfmw_image *image, hi_drv_video_frame *frame,
                                hi_vdec_ext_frm_info *ext_info)
{
    hi_drv_video_private *private_video_frame_info = HI_NULL;
    vfmw_private_frm_info *vpp_private = HI_NULL;
    hi_mem_handle_t mtdt_fd;

    VFMW_ASSERT_RET((frame != HI_NULL) && (image != HI_NULL), HI_FAILURE);
    private_video_frame_info = &(frame->video_private);
    vpp_private = vpp_get_private_node(chan);
    if (vpp_private == HI_NULL) {
        return HI_FAILURE;
    }

    vpp_get_bit_width(image, frame);
    vpp_get_pix_format(image, frame);
    vpp_get_field_mode(image, frame);
    vpp_get_buffer_mode(image, frame);
    vpp_get_buf_addr(image, frame, chan, &mtdt_fd);
    vpp_get_sample_type(image, frame);
    vpp_get_cmp_info(image, frame);
    vpp_get_3d_type(chan, frame, image);
    vpp_get_color_space_default(image, frame);
    vpp_convert_codec_type(image->vid_std, frame);
    vpp_get_uvmos(image, ext_info);
    vpp_get_usd(image, ext_info);
    vpp_get_frame_common_info(chan, image, frame);
    vpp_get_vc1_range_info(image, frame);

    private_video_frame_info->last_flag = image->last_frame == 1 ? HI_DRV_LAST_FRAME_FLAG : 0;
    private_video_frame_info->priv_disp_time = image->disp_time;
    private_video_frame_info->color_sys = HI_DRV_COLOR_SYS_AUTO;
    private_video_frame_info->source = chan->source;
    vpp_get_private_frame_info(image, vpp_private, mtdt_fd);
    private_video_frame_info->vdec_private.tpye = HI_DRV_MODULE_VDEC;
    private_video_frame_info->vdec_private.data = PTR_UINT64(vpp_private);
    private_video_frame_info->vdec_private.size = sizeof(vfmw_private_frm_info);

    if (frame->video_3d_type != chan->cur_3d_type) {
        VPP_REPORT_FRM_PCK_TYPE_CHG(chan, chan->chan_id, &frame->video_3d_type, sizeof(hi_drv_3d_type));
        chan->cur_3d_type = frame->video_3d_type;
    }

    if (image->disp_info.disp_width != chan->last_image.disp_info.disp_width ||
        image->disp_info.disp_height != chan->last_image.disp_info.disp_height) {
        vpp_report_norm_info(chan, image);
    }

    return HI_SUCCESS;
}

static hi_s32 vpp_frame_2_image(vpp_chan *chan, hi_drv_video_frame *frame, vfmw_image *image)
{
    hi_drv_module_private *module_private = &(frame->video_private.vdec_private);
    vfmw_private_frm_info *vpp_private = (vfmw_private_frm_info *)UINT64_PTR(module_private->data);

    dprint(PRN_DBG, "rls frame addr:%llx\n", frame->buf_addr[0].start_addr);
    if (module_private->tpye != HI_DRV_MODULE_VDEC || module_private->data == HI_NULL ||
        module_private->size != sizeof(vfmw_private_frm_info)) {
        return HI_FAILURE;
    }

    image->image_id = vpp_private->image_id;
    image->image_id_1 = vpp_private->image_id_1;
    image->disp_info.disp_width = frame->width;
    image->disp_info.disp_height = frame->height;

    if (chan->alloc_type != VFMW_FRM_ALLOC_OUTER) {
        OS_PUT_MEM_FD(frame->buf_addr[0].start_addr);
    }

    if (vpp_private->mtdt_mem_handle != HI_INVALID_HANDLE) {
        OS_PUT_MEM_FD(vpp_private->mtdt_mem_handle);
    }

    if (image->image_id_1 != -1) {
        image->is_3d = 1;
        if (chan->alloc_type != VFMW_FRM_ALLOC_OUTER) {
            OS_PUT_MEM_FD(frame->buf_addr[1].start_addr);
        }
    }

    vpp_put_private_node(chan, vpp_private);
    module_private->data = HI_NULL;
    module_private->size = 0;

    return HI_SUCCESS;
}

static hi_void vpp_image_2_hfbc(vfmw_image *tmp_image, hi_drv_video_frame *tmp_frame)
{
    vfmw_attr_hfbc tmp_hfbc = {0};

    if ((tmp_image->attach_buf.attached_handle != -1) &&
        (tmp_image->attach_buf.attached_phy_addr != 0) &&
        (tmp_image->attach_buf.attached_vir_addr != 0) &&
        (tmp_image->attach_buf.attached_size >= sizeof(hi_drv_video_frame))) {
        VFMW_CHECK_SEC_FUNC(memcpy_s(UINT64_PTR(tmp_image->attach_buf.attached_vir_addr),
                                     tmp_image->attach_buf.attached_size, tmp_frame, sizeof(hi_drv_video_frame)));
    }

    dprint(PRN_DBG, "fd = %lld ext_att_phy_addr = 0x%x ext_att_vir_addr = 0x%llx ext_att_buf_size = %d\n",
           tmp_image->attach_buf.attached_handle, tmp_image->attach_buf.attached_phy_addr,
           tmp_image->attach_buf.attached_vir_addr, tmp_image->attach_buf.attached_size);

    if ((tmp_image->attach_buf.hfbc_handle != -1) &&
        (tmp_image->attach_buf.hfbc_vir_addr != 0)) {
        tmp_hfbc.disp_width = tmp_image->dec_info.dec_width;  /* gpu hfbc need dec width/height */
        tmp_hfbc.disp_height = tmp_image->dec_info.dec_height;
        tmp_hfbc.bit_depth = tmp_image->bit_depth;
        if (tmp_image->bit_depth == 8) { /* 8 is bit depth */
            tmp_hfbc.luma_stride = tmp_frame->buf_addr[0].stride_y;
        } else {
            tmp_hfbc.luma_stride = tmp_frame->buf_addr[0].stride_y * 2; /* muti 2 */
        }

        tmp_hfbc.luma_header_offset = tmp_frame->buf_addr[0].y_head_offset;
        tmp_hfbc.luma_data_offset = tmp_frame->buf_addr[0].y_offset;
        tmp_hfbc.luma_data2_offset = tmp_image->disp_info.luma_phy_addr_2bit - tmp_image->disp_info.luma_phy_addr;;
        tmp_hfbc.chroma_header_offset = tmp_frame->buf_addr[0].c_head_offset;
        tmp_hfbc.chroma_data_offset = tmp_frame->buf_addr[0].c_offset;
        tmp_hfbc.chroma_data2_offset = tmp_image->disp_info.chrom_phy_addr_2bit - tmp_image->disp_info.luma_phy_addr;

        VFMW_CHECK_SEC_FUNC(memcpy_s(UINT64_PTR(tmp_image->attach_buf.hfbc_vir_addr),
                                     sizeof(vfmw_attr_hfbc), &tmp_hfbc, sizeof(vfmw_attr_hfbc)));
    }

    return;
}

static hi_s32 vpp_recieve_normal_mode(hi_u32 chan_id, hi_void *frame, hi_void *ext_frm_info)
{
    hi_s32 ret;
    vpp_chan *chan = HI_NULL;
    vfmw_image tmp_image = {0};
    vpp_entry *entry = vpp_get_entry();
    hi_drv_video_frame *tmp_frame = (hi_drv_video_frame *)frame;
    hi_vdec_ext_frm_info *ext_info = (hi_vdec_ext_frm_info *)ext_frm_info;

    chan = vpp_get_chan(chan_id);
    if (chan == HI_NULL || chan->created != HI_TRUE) {
        dprint(PRN_ERROR, "%s chan:%d not Create\n", __func__, chan_id);
        return HI_FAILURE;
    }

    chan->receive_try++;

    tmp_frame->source_fence = -1;
    tmp_frame->sink_fence = -1;
    tmp_image.frame_ptr = (hi_u64)(uintptr_t)frame;

    if (entry->get_image == HI_NULL) {
        return HI_FAILURE;
    }

#ifdef VFMW_FENCE_SUPPORT
    if (chan->is_omx_path == HI_FALSE) {
        fence_set_frm_flag(chan_id, &tmp_image);
    }
#endif

    ret = entry->get_image(chan_id, &tmp_image);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = vpp_image_2_frame(chan, &tmp_image, tmp_frame, ext_info);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "call vpp_image_2_frame failed\n");
        goto EXIT;
    }

    ret = vpp_check_frame(tmp_frame);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "call vir_pp_check_frame failed\n");
        goto EXIT;
    }

    if (entry->print_frm) {
        vpp_print_frame_info(tmp_frame);
    }

    vpp_image_2_hfbc(&tmp_image, tmp_frame);

    if (tmp_frame->frame_rate == 0 || entry->bypass) {
        goto EXIT;
    }

#ifdef VFMW_FENCE_SUPPORT
    if (chan->is_omx_path == HI_FALSE) {
        ret = fence_add_rcv_frm(chan_id, &tmp_frame->source_fence, &tmp_image);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "call vpp_add_rcv_frm failed\n");
            goto EXIT;
        }
        OS_EVENT_GIVE(entry->event);
    }
#endif

    VFMW_CHECK_SEC_FUNC(memcpy_s(&chan->last_image, sizeof(vfmw_image), &tmp_image, sizeof(vfmw_image)));

    chan->receive_ok++;

    return HI_SUCCESS;

EXIT:
    ret = entry->rls_image(chan_id, &tmp_image);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "%s %d release image fail.\n", __func__, __LINE__);
    }

    return HI_FAILURE;
}

static hi_void vpp_thread_process(vpp_entry *entry)
{
    hi_s32 ret;
    hi_s32 i = 0;
    hi_drv_video_frame frame = {0};
    hi_vdec_ext_frm_info ext_info = {{0}, 0};
    vpp_chan *chan = HI_NULL;

    for (i = 0; i < VFMW_CHAN_NUM; i++) {
        vpp_sema_down();

        chan = vpp_get_chan(i);
        if ((chan != HI_NULL) &&
            (chan->created == HI_TRUE) &&
            (chan->state == START_STATE) &&
            (chan->work_mode == REPORT_MODE)) {
            do {
                VFMW_CHECK_SEC_FUNC(memset_s(&frame, sizeof(hi_drv_video_frame), 0, sizeof(hi_drv_video_frame)));

                ret = vpp_receive_frame(i, &frame, &ext_info);
                if (ret == HI_SUCCESS) {
                    chan->failed_time = 0;
                    VPP_REPORT_NEW_FRAME(chan, i, (hi_void *)&frame, sizeof(frame));
                }
            } while (ret == HI_SUCCESS);

            chan->failed_time++;
            if (chan->failed_time >= PP_MAX_FAILED_TIME) {
                chan->failed_time = 0;
                VPP_REPORT_RECEIVE_FAILED(chan, i);
            }
        }

#ifdef VFMW_FENCE_SUPPORT
        if (chan != HI_NULL && chan->is_omx_path == HI_FALSE) {
            entry->wait_time = entry->init_time;
            if (fence_proc_frm_state(i) != 0) {
                entry->wait_time = 1;
            }
        }
#endif

        vpp_sema_up();
    }
}

static hi_s32 vpp_ctrl_thread(hi_void *data)
{
    vpp_entry *entry = vpp_get_entry();

    entry->wait_time = entry->init_time;
    entry->thread_state = THREA_IDLE;
    while (!OS_THREAD_SHOULD_STOP()) {
        entry->thread_state = THREA_RUNNING;

        vpp_thread_process(entry);

        entry->thread_state = THREA_SLEEP;
        OS_EVENT_WAIT(entry->event, entry->wait_time);
    }

    return HI_SUCCESS;
}

static hi_void vpp_config_souce(vpp_chan *chan, hi_u32 source)
{
    chan->source = source;
}

static hi_bool vpp_i_frame_check(vfmw_image *img)
{
    if (img->format.frame_type == 0) {
        return HI_TRUE;
    }

    if ((img->format.frame_type == 1) && (img->is_fld_flag)) {
        if ((img->top_fld_type == 0) || (img->bottom_fld_type == 0)) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

hi_bool vpp_fast_backward_check_for_new_i_frame(vfmw_private_frm_info *priv_info, vpp_chan *chan)
{
    if ((chan->pvr_info.get_first_frame == HI_TRUE) && (((priv_info->frm_type == 0) && (priv_info->field_flag == 0))
        ||  (((priv_info->bot_field_frame_format == 0) || (priv_info->top_field_frame_format == 0))
        &&  (priv_info->field_flag == 1)))) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

hi_bool vpp_fast_backward_check_for_new_gop(vfmw_private_frm_info *priv_info, vpp_chan *chan)
{
    if ((chan->pvr_info.get_first_frame == HI_TRUE) &&
        (chan->pvr_info.last_frame_gop_num != -1) &&
        (chan->pvr_info.last_frame_gop_num != priv_info->gop_num) &&
        (chan->pvr_info.tmp_list_pos > 0)) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

static hi_s32 vpp_insert_frm_to_tmp_list(vpp_chan *chan, hi_drv_video_frame *frame,
                                         hi_vdec_ext_frm_info *ext_info, vfmw_image *img)
{
    hi_s32 index;
    OS_SEMA *sema = HI_NULL;
    hi_drv_video_frame *first_frame = HI_NULL;

    sema = &(chan->pvr_info.pvr_sema);
    OS_SEMA_DOWN(*sema);
    index = chan->pvr_info.tmp_list_pos;

    if (index >= VPP_PVR_MAX_LIST_LEN) {
        dprint(PRN_ERROR, "Invalid List Index: %d\n", index);
        OS_SEMA_UP(*sema);
        return HI_FAILURE;
    }

    VFMW_CHECK_SEC_FUNC(memcpy_s(&(chan->pvr_info.tmp_list[index].frame),
                                 sizeof(hi_drv_video_frame), frame, sizeof(hi_drv_video_frame)));
    VFMW_CHECK_SEC_FUNC(memcpy_s(&(chan->pvr_info.tmp_list[index].ext_frm_info),
                                 sizeof(hi_drv_video_frame), ext_info, sizeof(hi_vdec_ext_frm_info)));
    chan->pvr_info.tmp_list[index].image_info.image_id = img->image_id;
    chan->pvr_info.tmp_list[index].image_info.image_id_1 = img->image_id_1;
    chan->pvr_info.tmp_list[index].image_info.is_3d = img->is_3d;
    chan->pvr_info.tmp_list[index].image_info.std = img->vid_std;
    chan->pvr_info.tmp_list[index].image_info.phy_addr = img->disp_info.luma_phy_addr;
    chan->pvr_info.tmp_list[index].image_info.width = img->disp_info.disp_width;
    chan->pvr_info.tmp_list[index].image_info.height = img->disp_info.disp_height;
    chan->pvr_info.tmp_list[index].image_info.phy_addr_1 = img->disp_info.luma_phy_addr_1;
    chan->pvr_info.tmp_list[index].image_info.meta_data_addr = img->hdr_input.hdr_metadata_phy_addr;

    /* due to the last frame decoded by decoder is not the last frame output to post module */
    /* exchange the flag with the first frame of the gop */
    if (frame->video_private.last_flag != 0) {
        first_frame = &chan->pvr_info.tmp_list[0].frame;
        first_frame->video_private.last_flag = frame->video_private.last_flag;
        frame->video_private.last_flag = 0;
    }

    chan->pvr_info.tmp_list_pos++;

    OS_SEMA_UP(*sema);

    return HI_SUCCESS;
}

static hi_s32 vpp_insert_tmp_list_to_final_list(vpp_chan *chan)
{
    hi_s32 i;
    struct OS_LIST_HEAD *frm_list = HI_NULL;
    struct OS_LIST_HEAD *node = HI_NULL;
    OS_SEMA *list_sema = HI_NULL;
    vpp_pvr_list_node *list_node = HI_NULL;
    vpp_pvr_list_node *inserted_frame[VPP_PVR_MAX_LIST_LEN] = {0};
    hi_u32 inserted_index = 0;

    list_sema = &chan->pvr_info.pvr_sema;

    OS_SEMA_DOWN(*list_sema);
    i = chan->pvr_info.tmp_list_pos - 1;

    if (i < 0) {
        OS_SEMA_UP(*list_sema);
        return HI_FAILURE;
    }

    frm_list = &(chan->pvr_info.frame_list);

    for (; i >= 0; i--) {
        list_node = OS_ALLOC_VIR("vpp_pvr_list_node", sizeof(vpp_pvr_list_node));
        if (list_node != HI_NULL) {
            VFMW_CHECK_SEC_FUNC(memset_s(list_node, sizeof(vpp_pvr_list_node), 0, sizeof(vpp_pvr_list_node)));
            if (inserted_index < VPP_PVR_MAX_LIST_LEN) {
                inserted_frame[inserted_index] = list_node;
                inserted_index++;
            }
        } else {
            dprint(PRN_ERROR, "alloc vpp_pvr_list_node fail.\n");
            for (i = 0; i < inserted_index; i++) {
                if (inserted_frame[i]) {
                    OS_FREE_VIR(inserted_frame[i]);
                }
            }
            OS_SEMA_UP(*list_sema);
            return HI_FAILURE;
        }

        VFMW_CHECK_SEC_FUNC(memcpy_s(&(list_node->frame), sizeof(hi_drv_video_frame),
                                     &(chan->pvr_info.tmp_list[i].frame), sizeof(hi_drv_video_frame)));
        VFMW_CHECK_SEC_FUNC(memcpy_s(&(list_node->ext_frm_info), sizeof(hi_vdec_ext_frm_info),
                                     &(chan->pvr_info.tmp_list[i].ext_frm_info), sizeof(hi_vdec_ext_frm_info)));
        VFMW_CHECK_SEC_FUNC(memcpy_s(&(list_node->image_info), sizeof(vpp_pvr_image_info),
                                     &(chan->pvr_info.tmp_list[i].image_info), sizeof(vpp_pvr_image_info)));
        node = &(list_node->node);
        OS_LIST_ADD_TAIL(node, frm_list);
        chan->pvr_info.final_frame_num++;
    }

    chan->pvr_info.tmp_list_pos = 0;
    OS_SEMA_UP(*list_sema);

    return HI_SUCCESS;
}

static hi_s32 vpp_recieve_frame_from_final_list(vpp_chan *chan, hi_void *frame, hi_void *ext_frm_info,
                                                vfmw_image *img)
{
    OS_SEMA *sema = &chan->pvr_info.pvr_sema;
    vpp_pvr_list_node *list_node = HI_NULL;

    OS_SEMA_DOWN(*sema);

    if (OS_LIST_EMPTY(&chan->pvr_info.frame_list)) {
        OS_SEMA_UP(*sema);
        return HI_FAILURE;
    }

    list_node = OS_LIST_ENTRY(chan->pvr_info.frame_list.next, vpp_pvr_list_node, node);
    VFMW_CHECK_SEC_FUNC(memcpy_s(frame, sizeof(hi_drv_video_frame),
                                 &(list_node->frame), sizeof(hi_drv_video_frame)));
    VFMW_CHECK_SEC_FUNC(memcpy_s(ext_frm_info, sizeof(hi_vdec_ext_frm_info),
                                 &(list_node->ext_frm_info), sizeof(hi_vdec_ext_frm_info)));
    img->image_id = list_node->image_info.image_id;
    img->image_id_1 = list_node->image_info.image_id_1;
    img->vid_std = list_node->image_info.std;
    img->is_3d = list_node->image_info.is_3d;
    img->disp_info.luma_phy_addr = list_node->image_info.phy_addr;
    img->disp_info.disp_width = list_node->image_info.width;
    img->disp_info.disp_height = list_node->image_info.height;
    img->disp_info.luma_phy_addr_1 = list_node->image_info.phy_addr_1;
    img->hdr_input.hdr_metadata_phy_addr = list_node->image_info.meta_data_addr;
    OS_LIST_DEL(&(list_node->node));
    chan->pvr_info.final_frame_num--;
    OS_FREE_VIR(list_node);

    OS_SEMA_UP(*sema);

    return HI_SUCCESS;
}

static hi_void vpp_clear_fast_backward_list(vpp_chan *chan)
{
    hi_s32 ret;
    hi_s32 i = 0;
    hi_drv_video_frame *frame = HI_NULL;
    vpp_pvr_list_node *list_node = HI_NULL;
    OS_SEMA *sema = HI_NULL;

    sema = &(chan->pvr_info.pvr_sema);

    OS_SEMA_DOWN(*sema);

    /* clear tmp list */
    for (i = 0; i < chan->pvr_info.tmp_list_pos; i++) {
        frame = &(chan->pvr_info.tmp_list[i].frame);
        ret = vpp_release_frame(chan->chan_id, frame);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "clear tmp list failed\n");
        }
    }

    chan->pvr_info.tmp_list_pos = 0;

    /* clear final list */
    while (!OS_LIST_EMPTY(&chan->pvr_info.frame_list)) {
        list_node = OS_LIST_ENTRY(chan->pvr_info.frame_list.next, vpp_pvr_list_node, node);
        if (!list_node) {
            dprint(PRN_ERROR, "list node is null\n");
            continue;
        }

        frame = &(list_node->frame);
        ret = vpp_release_frame(chan->chan_id, frame);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "Clear final list failed\n");
        }

        OS_LIST_DEL(&(list_node->node));
        OS_FREE_VIR(list_node);
        list_node = HI_NULL;
    }

    OS_SEMA_UP(*sema);

    return;
}

static hi_void vpp_free_fast_backward_list(vpp_chan *chan)
{
    vpp_pvr_list_node *list_node = HI_NULL;
    OS_SEMA *sema = HI_NULL;

    sema = &(chan->pvr_info.pvr_sema);

    OS_SEMA_DOWN(*sema);

    chan->pvr_info.tmp_list_pos = 0;

    /* clear final list */
    while (!OS_LIST_EMPTY(&chan->pvr_info.frame_list)) {
        list_node = OS_LIST_ENTRY(chan->pvr_info.frame_list.next, vpp_pvr_list_node, node);
        if (!list_node) {
            dprint(PRN_ERROR, "list node is null\n");
            continue;
        }

        OS_LIST_DEL(&(list_node->node));
        OS_FREE_VIR(list_node);
        list_node = HI_NULL;
    }

    OS_SEMA_UP(*sema);

    return;
}

static hi_s32 vpp_fast_backward_process_distance(vpp_chan *chan, vfmw_image *img)
{
    hi_s32 ret = HI_FAILURE;
    vpp_entry *entry = vpp_get_entry();

    if (img->last_frame == 1) {
        return HI_SUCCESS;
    }

    if (chan->pvr_info.ctrl_info.backward_optimize_flag) {
        if (!img->disp_enable_flag) {
            ret = entry->rls_image(chan->chan_id, img);
            if (ret != HI_SUCCESS) {
                dprint(PRN_ERROR, "release vfmw image fail!\n");
            }
            return HI_FAILURE;
        }

        if (chan->pvr_info.get_first_i_frame) {
            if (chan->pvr_info.img_distnace == 0) {
                chan->pvr_info.img_distnace = img->disp_frame_distance;
                return HI_SUCCESS;
            } else {
                chan->pvr_info.img_distnace--;
                ret = entry->rls_image(chan->chan_id, img);
                if (ret != HI_SUCCESS) {
                    dprint(PRN_ERROR, "release vfmw image fail!\n");
                }
                return HI_FAILURE;
            }
        }

        chan->pvr_info.get_first_i_frame = HI_TRUE;
        chan->pvr_info.img_distnace = img->disp_frame_distance;

        if (chan->pvr_info.img_distnace == 0) {
            return HI_SUCCESS;
        }

        chan->pvr_info.img_distnace--;
        ret = entry->rls_image(chan->chan_id, img);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "release vfmw image fail!\n");
        }
        return HI_FAILURE;
    }

    if (chan->pvr_info.get_first_i_frame) {
        return HI_SUCCESS;
    }

    if (vpp_i_frame_check(img)) {
        chan->pvr_info.get_first_i_frame = HI_TRUE;
        return HI_SUCCESS;
    }

    ret = entry->rls_image(chan->chan_id, img);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "release vfmw image fail!\n");
    }

    return HI_FAILURE;
}

static hi_s32 vpp_recieve_frame_fast_backward(hi_u32 chan_id, hi_void *frame, hi_void *ext_frm_info)
{
    hi_s32 ret;
    vpp_entry *entry = vpp_get_entry();
    vfmw_image tmp_img = {0};
    hi_u8 end_frame;
    vpp_chan *chan = vpp_get_chan(chan_id);
    hi_drv_video_frame *drv_frm = (hi_drv_video_frame *)frame;
    vfmw_private_frm_info *private_info = HI_NULL;

    tmp_img.frame_ptr = (hi_u64)(uintptr_t)frame;

    ret = entry->get_image(chan_id, &tmp_img);
    if (ret == HI_SUCCESS) {
        ret = vpp_fast_backward_process_distance(chan, &tmp_img);
        if (ret != HI_SUCCESS) {
            goto RECV_BACKWARDFRAME;
        }

        ret = vpp_image_2_frame(chan, &tmp_img, frame, ext_frm_info);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "convert image to frame error!\n");
            ret = entry->rls_image(chan_id, &tmp_img);
            if (ret != HI_SUCCESS) {
                dprint(PRN_ERROR, "release image fail!\n");
            }
            return HI_FAILURE;
        }

        /* based on current solution,we need reverse field order when pass the frame to post module
         in PVR fast backward scenes. */
        drv_frm->top_field_first = !drv_frm->top_field_first;
        private_info = (vfmw_private_frm_info *)UINT64_PTR(drv_frm->video_private.vdec_private.data);
        if (chan->pvr_info.ctrl_info.backward_optimize_flag) {
            if (vpp_fast_backward_check_for_new_gop(private_info, chan)) {
                ret = vpp_insert_tmp_list_to_final_list(chan);
                if (ret != HI_SUCCESS) {
                    dprint(PRN_ERROR, "insert tmp_list to final list fail!\n");
                }
            }
            chan->pvr_info.get_first_frame = HI_TRUE;
            chan->pvr_info.last_frame_gop_num = private_info->gop_num;
        } else {
            if (vpp_fast_backward_check_for_new_i_frame(private_info, chan)) {
                ret = vpp_insert_tmp_list_to_final_list(chan);
                if (ret != HI_SUCCESS) {
                    dprint(PRN_ERROR, "insert tmp_list to final list fail!\n");
                }
            }
            chan->pvr_info.get_first_frame = HI_TRUE;
        }

        end_frame = tmp_img.last_frame;

        ret = vpp_insert_frm_to_tmp_list(chan, frame, ext_frm_info, &tmp_img);
        if (ret != HI_SUCCESS) {
            ret = entry->rls_image(chan_id, &tmp_img);
            if (ret != HI_SUCCESS) {
                dprint(PRN_ERROR, "release image fail!\n");
            }
            dprint(PRN_ERROR, "Insert frame to tmp list failed\n");
        }

        /* if last frame has been decoded and insert into tmplist, we need to output tmplist frames to finallist */
        if (end_frame == 1) {
            ret = vpp_insert_tmp_list_to_final_list(chan);
            if (ret != HI_SUCCESS) {
                dprint(PRN_ERROR, "insert tmp_list to final list fail!\n");
            }

            if (chan->pvr_info.ctrl_info.backward_optimize_flag) {
                chan->pvr_info.last_frame_gop_num = private_info->gop_num;
            }
        }
    }

RECV_BACKWARDFRAME:
    ret = vpp_recieve_frame_from_final_list(chan, frame, ext_frm_info, &tmp_img);

#ifdef VFMW_FENCE_SUPPORT
    if (ret == HI_SUCCESS) {
        ret = fence_add_rcv_frm(chan_id, &drv_frm->source_fence, &tmp_img);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "call vpp_add_rcv_frm failed\n");
            ret = entry->rls_image(chan_id, &tmp_img);
            return HI_FAILURE;
        }
    }
#endif

    return ret;
}

static hi_void vpp_clear_manger_block(vpp_chan *chan)
{
    hi_s32 i = 0;
    vpp_private_block *block = HI_NULL;

    for (i = 0; i < VPP_MAX_PRIVATE_NODE_NUM; i++) {
        block = chan->vpp_private_node[i];
        if (block == HI_NULL) {
            continue;
        }

        if (block->is_use == HI_TRUE) {
            block->is_use = HI_FALSE;
        }
    }
}

hi_s32 vpp_init(vpp_init_param *param)
{
    hi_s32 ret;
    vpp_entry *entry = vpp_get_entry();

    OS_SEMA_INIT(&entry->sema);

    ret = OS_EVENT_INIT(&entry->event, 1);
    if (ret != OSAL_OK) {
        dprint(PRN_ERROR, "init event failed!\n");
        return HI_FAILURE;
    }

    entry->init_time = 10; /* 10 ms */
    entry->thread = OS_CREATE_THREAD(vpp_ctrl_thread, HI_NULL, VPP_TASK_NAME);
    if (entry->thread == HI_NULL) {
        dprint(PRN_ERROR, "%s create failed!\n", VPP_TASK_NAME);
        return HI_FAILURE;
    }

    entry->get_image = param->get_image;
    entry->rls_image = param->rls_image;

    entry->inited = HI_TRUE;

    return HI_SUCCESS;
}

hi_s32 vpp_deinit(hi_void)
{
    vpp_entry *entry = vpp_get_entry();

    VFMW_ASSERT_RET(entry->inited == HI_TRUE, HI_FAILURE);

    if (entry->thread != HI_NULL) {
        OS_EVENT_GIVE(entry->event);
        OS_STOP_THREAD(entry->thread);
        entry->thread = HI_NULL;
        entry->get_image = HI_NULL;
        entry->rls_image = HI_NULL;
    }
    entry->inited = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 vpp_create(hi_u32 chan_id, vpp_create_param *param)
{
    vpp_chan *chan = HI_NULL;
    vpp_entry *entry = vpp_get_entry();

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(param->event_report != HI_NULL, HI_FAILURE);
    VFMW_ASSERT_RET(entry->inited == HI_TRUE, HI_FAILURE);

    vpp_sema_down();
    chan = vpp_get_chan(chan_id);
    if (chan != HI_NULL) {
        vpp_sema_up();
        return HI_FAILURE;
    }
    vpp_sema_up();
    chan = OS_ALLOC_VIR("vpp_inst", sizeof(vpp_chan));
    if (chan == HI_NULL) {
        return HI_FAILURE;
    }
    VFMW_CHECK_SEC_FUNC(memset_s(chan, sizeof(vpp_chan), 0, sizeof(vpp_chan)));
    chan->created = HI_TRUE;
    chan->chan_id = chan_id;
    chan->state = STOP_STATE;
    chan->work_mode = param->work_mode;
    chan->event_report = param->event_report;
    chan->ext_3d_type = HI_DRV_3D_MAX;

    /* pvr info init */
    chan->pvr_info.last_frame_gop_num = -1;
    OS_SEMA_INIT(&chan->pvr_info.pvr_sema);
    OS_LIST_INIT_HEAD(&chan->pvr_info.frame_list);

    vpp_sema_down();
    entry->chan[chan_id] = chan;
    vpp_sema_up();

    return HI_SUCCESS;
}

hi_s32 vpp_destroy(hi_u32 chan_id)
{
    vpp_chan *chan = HI_NULL;
    vpp_entry *entry = vpp_get_entry();

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(entry->inited == HI_TRUE, HI_FAILURE);

    vpp_sema_down();

    chan = vpp_get_chan(chan_id);
    if (chan == HI_NULL || chan->created == HI_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        vpp_sema_up();
        return HI_FAILURE;
    }

    if (chan->state == START_STATE) {
        dprint(PRN_ERROR, "%s chan:%d not stop before destroy\n", __func__, chan_id);
        vpp_sema_up();
        return HI_FAILURE;
    }

    vpp_free_fast_backward_list(chan);
    vpp_free_all_private_node(chan);

    OS_SEMA_EXIT(chan->pvr_info.pvr_sema);

    entry->chan[chan_id] = HI_NULL;
    OS_FREE_VIR(chan);

    vpp_sema_up();

    return HI_SUCCESS;
}

hi_s32 vpp_start(hi_u32 chan_id)
{
    vpp_chan *chan = HI_NULL;
    vpp_entry *entry = vpp_get_entry();

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(entry->inited == HI_TRUE, HI_FAILURE);

    vpp_sema_down();
    chan = vpp_get_chan(chan_id);
    if (chan == HI_NULL || chan->created == HI_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        vpp_sema_up();
        return HI_FAILURE;
    }

    chan->state = START_STATE;
    vpp_sema_up();

    return HI_SUCCESS;
}

hi_s32 vpp_stop(hi_u32 chan_id)
{
    vpp_chan *chan = HI_NULL;
    vpp_entry *entry = vpp_get_entry();

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(entry->inited == HI_TRUE, HI_FAILURE);

    vpp_sema_down();

    chan = vpp_get_chan(chan_id);
    if (chan == HI_NULL || chan->created == HI_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        vpp_sema_up();

        return HI_FAILURE;
    }

    chan->state = STOP_STATE;
    vpp_sema_up();

    return HI_SUCCESS;
}

hi_s32 vpp_reset(hi_u32 chan_id)
{
    vpp_chan *chan = HI_NULL;
    vpp_entry *entry = vpp_get_entry();

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(entry->inited == HI_TRUE, HI_FAILURE);

    vpp_sema_down();

    chan = vpp_get_chan(chan_id);
    if (chan == HI_NULL || chan->created == HI_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        vpp_sema_up();
        return HI_FAILURE;
    }

    if (chan->state == START_STATE) {
        dprint(PRN_ERROR, "%s chan:%d not stop before reset\n", __func__, chan_id);
        vpp_sema_up();
        return HI_FAILURE;
    }

    vpp_clear_fast_backward_list(chan);

    if (chan->is_omx_path == HI_TRUE) {
        vpp_clear_manger_block(chan);
    }

    vpp_sema_up();

    return HI_SUCCESS;
}

hi_s32 vpp_config(hi_u32 chan_id, vfmw_chan_cfg *cfg)
{
    vpp_chan *chan = HI_NULL;
    vpp_entry *entry = vpp_get_entry();

    VFMW_ASSERT_RET(entry->inited == HI_TRUE, HI_FAILURE);
    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(cfg != HI_NULL, HI_FAILURE);

    vpp_sema_down();

    chan = vpp_get_chan(chan_id);
    if (chan == HI_NULL || chan->created == HI_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        vpp_sema_up();
        return HI_FAILURE;
    }

    vpp_config_souce(chan, cfg->source);

    chan->err_thr = cfg->chan_err_thr;
    chan->dec_mode = cfg->dec_mode;
    chan->is_omx_path = cfg->is_omx_path;
    chan->alloc_type = cfg->alloc_type;

    vpp_sema_up();

    return HI_SUCCESS;
}

hi_s32 vpp_get_chan_status(hi_u32 chan_id, vfmw_chan_info *status)
{
    vpp_chan *chan = HI_NULL;
    vpp_entry *entry = vpp_get_entry();

    VFMW_ASSERT_RET(entry->inited == HI_TRUE, HI_FAILURE);
    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(status != HI_NULL, HI_FAILURE);

    vpp_sema_down();

    chan = vpp_get_chan(chan_id);
    if (chan == HI_NULL || chan->created != HI_TRUE) {
        vpp_sema_up();
        return HI_FAILURE;
    }

    status->std = chan->last_image.vid_std;
    status->video_format = chan->last_image.format.video_format;
    status->scan_type = chan->last_image.format.source_format;
    status->aspect_width = chan->last_image.aspect_width;
    status->aspect_height = chan->last_image.aspect_height;
    status->bit_depth = chan->last_image.bit_depth;
    status->frame_rate = chan->last_image.frame_rate;
    status->disp_height = chan->last_image.disp_info.disp_height;
    status->disp_width = chan->last_image.disp_info.disp_width;
    status->disp_center_x = status->disp_width / 2 ; /* 2 caculate center by divide 2 */
    status->disp_center_y = status->disp_height / 2; /* 2 caculate center by divide 2 */
    status->image_width = chan->last_image.image_width;
    status->image_height = chan->last_image.image_height;
    status->is_field_flg = chan->last_image.is_fld_flag;

    VFMW_CHECK_SEC_FUNC(memcpy_s(&status->color_info, sizeof(vfmw_color_desc),
                                 &chan->last_image.hdr_input.colour_info, sizeof(vfmw_color_desc)));

    vpp_sema_up();

    return HI_SUCCESS;
}

hi_s32 vpp_set_pvr_speed(hi_u32 chan_id, hi_s32 speed)
{
    vpp_chan *chan = HI_NULL;
    vpp_entry *entry = vpp_get_entry();

    VFMW_ASSERT_RET(entry->inited == HI_TRUE, HI_FAILURE);
    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);

    vpp_sema_down();

    chan = vpp_get_chan(chan_id);
    if (chan == HI_NULL || chan->created != HI_TRUE) {
        vpp_sema_up();
        return HI_FAILURE;
    }

    chan->pvr_info.speed = speed;

    vpp_sema_up();

    return HI_SUCCESS;
}

hi_s32 vpp_set_pvr_ctrl_info(hi_u32 chan_id, vfmw_control_info *ctrl)
{
    vpp_chan *chan = HI_NULL;
    vpp_entry *entry = vpp_get_entry();

    VFMW_ASSERT_RET(entry->inited == HI_TRUE, HI_FAILURE);
    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(ctrl != HI_NULL, HI_FAILURE);

    vpp_sema_down();

    chan = vpp_get_chan(chan_id);
    if (chan == HI_NULL || chan->created != HI_TRUE) {
        vpp_sema_up();
        return HI_FAILURE;
    }

    VFMW_CHECK_SEC_FUNC(memcpy_s(&chan->pvr_info.ctrl_info,
                                 sizeof(vfmw_control_info), ctrl, sizeof(vfmw_control_info)));

    vpp_sema_up();

    return HI_SUCCESS;
}

hi_s32 vpp_receive_frame(hi_u32 chan_id, hi_void *frame, hi_void *ext_frm_info)
{
    hi_s32 ret;
    vpp_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(frame != HI_NULL, HI_FAILURE);
    VFMW_ASSERT_RET(ext_frm_info != HI_NULL, HI_FAILURE);

    chan = vpp_get_chan(chan_id);
    if (chan->pvr_info.speed < 0) {
        ret = vpp_recieve_frame_fast_backward(chan_id, frame, ext_frm_info);
    } else {
        ret = vpp_recieve_normal_mode(chan_id, frame, ext_frm_info);
    }

    return ret;
}

hi_s32 vpp_release_frame(hi_u32 chan_id, const hi_void *frame)
{
    hi_s32 ret;
    vpp_chan *chan = HI_NULL;
    vfmw_image tmp_image = {0};
    hi_drv_video_frame *tmp_frame = HI_NULL;
    vpp_entry *entry = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(frame != HI_NULL, HI_FAILURE);

    chan = vpp_get_chan(chan_id);
    if (chan == HI_NULL || chan->created != HI_TRUE) {
        dprint(PRN_ERROR, "%s chan:%d not Create\n", __func__, chan_id);
        return HI_FAILURE;
    }

    chan->release_try++;

    entry = vpp_get_entry();
    tmp_frame = (hi_drv_video_frame *)frame;
    ret = vpp_frame_2_image(chan, tmp_frame, &tmp_image);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "call vir_pp_convert_image failed\n");
        return HI_FAILURE;
    }

    if (entry->rls_image == HI_NULL) {
        return HI_FAILURE;
    }

#ifdef VFMW_FENCE_SUPPORT
    if (chan->is_omx_path == HI_FALSE) {
        ret = fence_add_rls_frm(chan_id, tmp_frame->source_fence, tmp_frame->sink_fence, &tmp_image);
        if (ret == HI_SUCCESS) {
            chan->release_ok++;
            return HI_SUCCESS;
        }
    }
#endif

    ret = entry->rls_image(chan_id, &tmp_image);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "call vir_pp_release_image failed\n");
        return HI_FAILURE;
    }

    chan->release_ok++;

    return HI_SUCCESS;
}

hi_s32 vpp_set_frm_3d_type(hi_u32 chan_id, hi_void *frm_3d_type)
{
    vpp_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(frm_3d_type != HI_NULL, HI_FAILURE);

    chan = vpp_get_chan(chan_id);
    if (chan == HI_NULL || chan->created != HI_TRUE) {
        dprint(PRN_ERROR, "%s chan:%d not Create\n", __func__, chan_id);
        return HI_FAILURE;
    }

    chan->ext_3d_type = *((hi_drv_3d_type *)frm_3d_type);

    return HI_SUCCESS;
}

hi_s32 vpp_get_frm_3d_type(hi_u32 chan_id, hi_void *frm_3d_type)
{
    vpp_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(frm_3d_type != HI_NULL, HI_FAILURE);

    chan = vpp_get_chan(chan_id);
    if (chan == HI_NULL || chan->created != HI_TRUE) {
        dprint(PRN_ERROR, "%s chan:%d not Create\n", __func__, chan_id);
        return HI_FAILURE;
    }

    *((hi_drv_3d_type *)frm_3d_type) = chan->cur_3d_type;

    return HI_SUCCESS;
}

hi_s32 vpp_set_evt_map(hi_u32 chan_id, hi_u32 evt_map)
{
    vpp_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);

    chan = vpp_get_chan(chan_id);
    if (chan == HI_NULL || chan->created != HI_TRUE) {
        dprint(PRN_ERROR, "%s chan:%d not Create\n", __func__, chan_id);
        return HI_FAILURE;
    }

    chan->evt_map = evt_map;

    return HI_SUCCESS;
}
