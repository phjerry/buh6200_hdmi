/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: drv_vpss_dbg.c source file vpss debug
 * Author: zhangjunyu
 * Create: 2016/06/13
 */
#include "drv_vpss_dbg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_MAXWEITH_SAVEYUV 4096
static hi_void print_frame_info(hi_bool is_src, hi_drv_vpss_video_frame *frm)
{
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;

    if (is_src == HI_TRUE) {
        meta_info = (hi_drv_win_vpss_meta_info *)frm->comm_frame.video_private.win_private.data;
    } else {
        meta_info = (hi_drv_win_vpss_meta_info *)frm->comm_frame.video_private.vpss_private.data;
    }

    osal_printk("image info:index:%d, tun_addr:%llx, format:%d, W:%d, H:%d; hdr_type:%d",
                frm->comm_frame.frame_index, frm->comm_frame.tunnel_phy_addr, frm->comm_frame.pixel_format,
                frm->comm_frame.width, frm->comm_frame.height, frm->comm_frame.hdr_type);

    osal_printk("--- color_primary:%d, color_space:%d, quantify_range:%d, transfer_type:%d, matrix_coef:%d",
                frm->comm_frame.color_space.color_primary,
                frm->comm_frame.color_space.color_space, frm->comm_frame.color_space.quantify_range,
                frm->comm_frame.color_space.transfer_type, frm->comm_frame.color_space.matrix_coef);

    if (meta_info != HI_NULL) {
        osal_printk("--- playcnt:%d, play_index:%d, parent_frame_index:%d",
                    meta_info->play_cnt, meta_info->play_index, meta_info->parent_frame_index);
    }
    osal_printk("--- sample_type:%d, field_mode:%d, buf_valid_mode:%d, src_pts:%lld, frame_rate:%d, last_flag:%d;",
                frm->comm_frame.sample_type, frm->comm_frame.field_mode, frm->comm_frame.buf_valid_mode,
                frm->comm_frame.src_pts, frm->comm_frame.frame_rate, frm->comm_frame.video_private.last_flag);
    osal_printk("--- adjust pts:%lld, data_fmt:%d, cmp_mode:%d, ycmp_rate:%d, ccmp_rate:%d;",
                frm->comm_frame.pts, frm->comm_frame.cmp_info.cmp_mode, frm->comm_frame.cmp_info.data_fmt,
                frm->comm_frame.cmp_info.ycmp_rate, frm->comm_frame.cmp_info.ccmp_rate);
    osal_printk("--- source_type:%d, bit_width:%d, flip_h:%d, flip_v:%d\n",
                frm->comm_frame.video_private.source, frm->comm_frame.bit_width,
                frm->comm_frame.flip_h, frm->comm_frame.flip_v);
    osal_printk("--- L:dma:0x%llx, yoff:0x%x, coff:0x%x, ystr:0x%x, cstr:0x%x \n",
                frm->comm_frame.buf_addr[HI_DRV_3D_EYE_LEFT].dma_handle,
                frm->comm_frame.buf_addr[HI_DRV_3D_EYE_LEFT].y_offset,
                frm->comm_frame.buf_addr[HI_DRV_3D_EYE_LEFT].c_offset,
                frm->comm_frame.buf_addr[HI_DRV_3D_EYE_LEFT].stride_y,
                frm->comm_frame.buf_addr[HI_DRV_3D_EYE_LEFT].stride_c);

    osal_printk("--- R:dma:0x%llx, yoff:0x%x, coff:0x%x, ystr:0x%x, cstr:0x%x \n",
                frm->comm_frame.buf_addr[HI_DRV_3D_EYE_RIGHT].dma_handle,
                frm->comm_frame.buf_addr[HI_DRV_3D_EYE_RIGHT].y_offset,
                frm->comm_frame.buf_addr[HI_DRV_3D_EYE_RIGHT].c_offset,
                frm->comm_frame.buf_addr[HI_DRV_3D_EYE_RIGHT].stride_y,
                frm->comm_frame.buf_addr[HI_DRV_3D_EYE_RIGHT].stride_c);

    return;
}

hi_s32 vpss_dbg_print_frame(vpss_debug_info *dbg_info, hi_drv_vpss_video_frame *frame, hi_u32 dbg_part)
{
    hi_bool is_src = HI_FALSE;

    switch (dbg_part) {
        case DEF_DBG_SRC_ID:
            if ((dbg_info->print_src_info) && (dbg_info->src_dbg_times >= 1)) {
                is_src = HI_TRUE;
                print_frame_info(is_src, frame);
                dbg_info->src_dbg_times--;

                if (dbg_info->src_dbg_times == 0) {
                    dbg_info->print_src_info = HI_FALSE;
                }
            }
            break;
        case DEF_DBG_PORT0_ID:
        case DEF_DBG_PORT1_ID:
        case DEF_DBG_PORT2_ID:
            if ((dbg_info->print_out_info[dbg_part]) && (dbg_info->out_dbg_times[dbg_part] >= 1)) {
                is_src = HI_FALSE;
                print_frame_info(is_src, frame);
                dbg_info->out_dbg_times[dbg_part]--;

                if (dbg_info->out_dbg_times[dbg_part] == 0) {
                    dbg_info->print_out_info[dbg_part] = HI_FALSE;
                }
            }
            break;
        default:
            vpss_error("invalid dbg_part %#x\n", dbg_part);
            break;
    }

    return HI_SUCCESS;
}

hi_void vpss_dbg_save_src_frame(vpss_debug_info *dbg_info, hi_drv_vpss_video_frame *frame, hi_u32 dbg_part)
{
    hi_s8 ch_file[DEF_FILE_NAMELENGTH] = { 0 };
    hi_drv_vpss_video_frame tmp_frm = { 0 };

    if ((dbg_info->save_in_frame) && (dbg_info->src_dbg_times >= 1)) {
        snprintf(ch_file, DEF_FILE_NAMELENGTH, "vpss_src_%d_x%d_%d_hz_.yuv",
                 frame->comm_frame.width, frame->comm_frame.height,
                 frame->comm_frame.frame_rate / 1000); /* 1000 is times */
        vpss_comm_save_frame_to_file(frame, ch_file, DEF_FILE_NAMELENGTH);

        if (frame->comm_frame.video_3d_type == HI_DRV_3D_FPK) {
            snprintf(ch_file, DEF_FILE_NAMELENGTH, "vpss_src_right_%d_x%d_%d_hz_.yuv",
                     frame->comm_frame.width, frame->comm_frame.height,
                     frame->comm_frame.frame_rate / 1000); /* 1000 is times */
            memcpy(&tmp_frm, frame, sizeof(hi_drv_vpss_video_frame));
            memcpy(&(tmp_frm.vpss_buf_addr[0]), &(frame->vpss_buf_addr[1]), sizeof(hi_drv_vpss_vid_frame_addr));
            vpss_comm_save_frame_to_file(&tmp_frm, ch_file, DEF_FILE_NAMELENGTH);
        }

        dbg_info->src_dbg_times--;

        if (dbg_info->src_dbg_times == 0) {
            dbg_info->save_in_frame = HI_FALSE;
        }
    }
}

hi_void vpss_dbg_save_output_frame(vpss_debug_info *dbg_info, hi_drv_vpss_video_frame *frame, hi_u32 dbg_part)
{
    hi_s8 ch_file[DEF_FILE_NAMELENGTH] = { 0 };
    hi_drv_vpss_video_frame tmp_frm = { 0 };

    if ((dbg_info->save_out_frame[dbg_part]) && (dbg_info->out_dbg_times[dbg_part] >= 1)
        && (frame->comm_frame.cmp_info.cmp_mode == HI_DRV_COMPRESS_MODE_OFF)) {
        snprintf(ch_file, DEF_FILE_NAMELENGTH, "vpss_p%d_%d_x%d_%d_hz_.yuv",
                 dbg_part, frame->comm_frame.width, frame->comm_frame.height,
                 frame->comm_frame.frame_rate / 1000); /* 1000 is times */
        vpss_comm_save_out_frame_to_file(frame, ch_file, DEF_FILE_NAMELENGTH);

        if (frame->comm_frame.video_3d_type == HI_DRV_3D_FPK) {
            snprintf(ch_file, DEF_FILE_NAMELENGTH, "vpss_p%d_right_%d_x%d_%d_hz_.yuv", dbg_part,
                     frame->comm_frame.width, frame->comm_frame.height,
                     frame->comm_frame.frame_rate / 1000); /* 1000 is times */
            memcpy(&tmp_frm, frame, sizeof(hi_drv_vpss_video_frame));
            memcpy(&(tmp_frm.vpss_buf_addr[0]), &(frame->vpss_buf_addr[1]), sizeof(hi_drv_vpss_vid_frame_addr));
            vpss_comm_save_out_frame_to_file(&tmp_frm, ch_file, DEF_FILE_NAMELENGTH);
        }

        dbg_info->out_dbg_times[dbg_part]--;

        if (dbg_info->out_dbg_times[dbg_part] == 0) {
            dbg_info->save_out_frame[dbg_part] = HI_FALSE;
        }
    }
}

hi_s32 vpss_dbg_save_frame(vpss_debug_info *dbg_info, hi_drv_vpss_video_frame *frame, hi_u32 dbg_part)
{
    if ((frame->comm_frame.video_private.last_flag != HI_DRV_LAST_ERROR_FLAG) &&
        ((frame->comm_frame.width == 0) || (frame->comm_frame.height == 0) ||
         (frame->vpss_buf_addr[0].phy_addr_y == 0) || (frame->vpss_buf_addr[0].phy_addr_c == 0) ||
         (frame->vpss_buf_addr[0].stride_y == 0) || (frame->vpss_buf_addr[0].stride_c == 0))) {
        vpss_info("invalid frm info w=%d,h=%d,yaddr=0x%08x,caddr=0x%08x,ystride=%d,cstride=%d\n",
                  frame->comm_frame.width, frame->comm_frame.height, frame->vpss_buf_addr[0].phy_addr_y,
                  frame->vpss_buf_addr[0].phy_addr_c,
                  frame->vpss_buf_addr[0].stride_y, frame->vpss_buf_addr[0].stride_c);
        return HI_FAILURE;
    }

    if (frame->comm_frame.width > VPSS_MAXWEITH_SAVEYUV) {
        return HI_FAILURE;
    }

    switch (dbg_part) {
        case DEF_DBG_SRC_ID:
            vpss_dbg_save_src_frame(dbg_info, frame, dbg_part);
            break;
        case DEF_DBG_PORT0_ID:
        case DEF_DBG_PORT1_ID:
        case DEF_DBG_PORT2_ID:
            vpss_dbg_save_output_frame(dbg_info, frame, dbg_part);
            break;
        default:
            vpss_error("invalid dbg_part %#x\n", dbg_part);
            break;
    }

    return HI_SUCCESS;
}

#ifndef DPT
hi_s32 vpss_dbg_print_frame_addr(vpss_debug_info *dbg_info, hi_drv_video_frame *frame, vpss_debug_type type)
{
    if (dbg_info->print_frame_addr) {
        switch (type) {
            case VPSS_IN_QUEUE_FRAME_TYPE: {
                osal_printk("vpss in que    idx=%d addr=0x%llx \n", frame->frame_index, frame->buf_addr[0].dma_handle);
            }
            break;
            case VPSS_IN_DEQUEUE_FRAME_TYPE: {
                osal_printk("vpss in deque  idx=%d addr=0x%llx \n", frame->frame_index, frame->buf_addr[0].dma_handle);
            }
            break;
            case VPSS_OUT_ACQUIRE_FRAME_TYPE: {
                osal_printk("vpss out acq   idx=%d addr=0x%llx \n", frame->frame_index, frame->buf_addr[0].dma_handle);
            }
            break;
            case VPSS_OUT_RELEASE_FRAME_TYPE: {
                osal_printk("vpss out rel   idx=%d addr=0x%llx \n", frame->frame_index, frame->buf_addr[0].dma_handle);
            }
            break;
            case VPSS_OUT_QUEUE_FRAME_TYPE: {
                osal_printk("vpss out que   idx=%d addr=0x%llx \n", frame->frame_index, frame->buf_addr[0].dma_handle);
            }
            break;
            case VPSS_OUT_DEQUEUE_FRAME_TYPE: {
                osal_printk("vpss out deque idx=%d addr=0x%llx \n", frame->frame_index, frame->buf_addr[0].dma_handle);
            }
            break;
            case VPSS_FRAME_TYPE_MAX:
                vpss_error("invalid frame_type %d\n", type);
                break;
        }
    }

    return HI_SUCCESS;
}
#else
hi_s32 vpss_dbg_print_frame_addr(vpss_debug_info *dbg_info, hi_drv_video_frame *frame, vpss_debug_type type)
{
    /* todo */
    hi_drv_hardware_buffer *hw_buf_attr = &frame->hardware_buffer;

    if (dbg_info->print_frame_addr) {
        switch (type) {
            case VPSS_IN_QUEUE_FRAME_TYPE: {
                osal_printk("\nvpss que idx=%d addr=0x%x srcid=%d tunl=%d hw_idx=%d hwaddr=0x%llx\n",
                            frame->frame_index, frame->vpss_buf_addr[0].phy_dma_handle, hw_buf_attr->src_id,
                            hw_buf_attr->forward_tunnel_enable, hw_buf_attr->buffer_index, hw_buf_attr->buffer_addr);
            }
            break;
            case VPSS_IN_DEQUEUE_FRAME_TYPE: {
                osal_printk("\nvpss deque idx=%d addr=0x%x dstid=%d tunl=%d hw_idx=%d hwaddr=0x%llx\n",
                            frame->frame_index, frame->vpss_buf_addr[0].phy_dma_handle, hw_buf_attr->dst_id,
                            hw_buf_attr->backward_tunnel_enable, hw_buf_attr->buffer_index, hw_buf_attr->buffer_addr);
            }
            break;
            case VPSS_OUT_ACQUIRE_FRAME_TYPE: {
                osal_printk("\nvpss get idx=%d addr=0x%x srcid=%d tunl=%d hw_idx=%d hwaddr=0x%llx\n",
                            frame->frame_index, frame->vpss_buf_addr[0].phy_dma_handle, hw_buf_attr->src_id,
                            hw_buf_attr->forward_tunnel_enable, hw_buf_attr->buffer_index, hw_buf_attr->buffer_addr);
            }
            break;
            case VPSS_OUT_RELEASE_FRAME_TYPE: {
                osal_printk("\nvpss release idx=%d addr=0x%x dstid=%d tunl=%d hw_idx=%d hwaddr=0x%llx\n",
                            frame->frame_index, frame->vpss_buf_addr[0].phy_dma_handle, hw_buf_attr->dst_id,
                            hw_buf_attr->backward_tunnel_enable, hw_buf_attr->buffer_index, hw_buf_attr->buffer_addr);
            }
            break;
            case VPSS_OUT_QUEUE_FRAME_TYPE: {
                osal_printk("\nvpss out_put idx=%d addr=0x%x srcid=%d tunl=%d hw_idx=%d hwaddr=0x%llx\n",
                            frame->frame_index, frame->vpss_buf_addr[0].phy_dma_handle, hw_buf_attr->src_id,
                            hw_buf_attr->forward_tunnel_enable, hw_buf_attr->buffer_index, hw_buf_attr->buffer_addr);
            }
            break;
            case VPSS_OUT_DEQUEUE_FRAME_TYPE: {
                osal_printk("\nvpss out_get idx=%d addr=0x%x dstid=%d tunl=%d hw_idx=%d hwaddr=0x%llx\n",
                            frame->frame_index, frame->vpss_buf_addr[0].phy_dma_handle, hw_buf_attr->dst_id,
                            hw_buf_attr->backward_tunnel_enable, hw_buf_attr->buffer_index, hw_buf_attr->buffer_addr);
            }
            break;
            case VPSS_FRAME_TYPE_MAX:
                vpss_error("invalid frame_type %d\n", type);
                break;
        }
    }

    return HI_SUCCESS;
}
#endif

hi_void vpss_dbg_print_tunl_info(vpss_debug_info *dbg_info, hi_drv_video_frame *frame, vpss_debug_tunl_type type)
{
    /* todo */
#ifdef DPT
    if (dbg_info->print_tunl_info) {
        switch (type) {
            case VPSS_INPUT_TUNL_TYPE: {
                osal_printk("\nvpss intunl  src_id=%d idx=%d  addr=0x%llx\n",
                            frame->hardware_buffer.src_id, frame->hardware_buffer.buffer_index,
                            frame->buf_addr[0].dma_handle);
                break;
            }
            case VPSS_OUTPUT_TUNL_TYPE: {
                osal_printk("\nvpss outtunl dst_id=%d idx=%d  addr=0x%llx\n",
                            frame->hardware_buffer.src_id, frame->hardware_buffer.buffer_index,
                            frame->buf_addr[0].dma_handle);
                break;
            }
            case VPSS_TUNL_TYPE_MAX:
                vpss_error("invalid tunl type %d\n", type);
                break;
        }
    }
#endif
    return;
}

hi_void vpss_dbg_print_ref_count_info(vpss_debug_info *dbg_info, hi_drv_vpss_video_frame *frame,
                                      vpss_debug_ref_count_type type)
{
    if (dbg_info->print_ref_cnt_info) {
        switch (type) {
            case VPSS_REFCNT_INCREASE_TYPE: {
                osal_printk("vpss queue  refcnt++    idx=%d dma=0x%llx\n",
                            frame->comm_frame.frame_index, frame->comm_frame.buf_addr[0].dma_handle);
                break;
            }
            case VPSS_REFCNT_DECREASE_PRO_TYPE: {
                osal_printk("vpss process refcnt--   idx=%d dma=0x%llx\n",
                            frame->comm_frame.frame_index, frame->comm_frame.buf_addr[0].dma_handle);
                break;
            }
            case VPSS_REFCNT_DECREASE_TRAN_TYPE: {
                osal_printk("vpss tran   refcnt--    idx=%d dma=0x%llx\n",
                            frame->comm_frame.frame_index, frame->comm_frame.buf_addr[0].dma_handle);
                break;
            }
            case VPSS_REFCNT_DECREASE_UNPRO_TYPE: {
                osal_printk("vpss unprocess refcnt-- idx=%d dma=0x%llx\n",
                            frame->comm_frame.frame_index, frame->comm_frame.buf_addr[0].dma_handle);
                break;
            }
            case VPSS_REFCNT_TYPE_MAX:
                vpss_error("invalid refcnt type %d\n", type);
                break;
        }
    }
    return;
}

hi_void vpss_dbg_print_ref_count_info_comm_frame(vpss_debug_info *dbg_info, hi_drv_video_frame *frame,
    vpss_debug_ref_count_type type)
{
    if (dbg_info->print_ref_cnt_info) {
        switch (type) {
            case VPSS_REFCNT_INCREASE_TYPE: {
                osal_printk("vpss queue  refcnt++    idx=%d dma=0x%llx\n",
                            frame->frame_index, frame->buf_addr[0].dma_handle);
                break;
            }
            case VPSS_REFCNT_DECREASE_PRO_TYPE: {
                osal_printk("vpss process refcnt--   idx=%d dma=0x%llx\n",
                            frame->frame_index, frame->buf_addr[0].dma_handle);
                break;
            }
            case VPSS_REFCNT_DECREASE_TRAN_TYPE: {
                osal_printk("vpss tran   refcnt--    idx=%d dma=0x%llx\n",
                            frame->frame_index, frame->buf_addr[0].dma_handle);
                break;
            }
            case VPSS_REFCNT_DECREASE_UNPRO_TYPE: {
                osal_printk("vpss unprocess refcnt-- idx=%d dma=0x%llx\n",
                            frame->frame_index, frame->buf_addr[0].dma_handle);
                break;
            }
            case VPSS_REFCNT_TYPE_MAX:
                vpss_error("invalid refcnt type %d\n", type);
                break;
        }
    }
    return;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


