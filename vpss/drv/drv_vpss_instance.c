/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: drv_vpss_instance.c source file vpss instance
 * Author: zhangjunyu
 * Create: 2016/06/13
 */
#include "hi_drv_proc.h"
#include "hi_osal.h"
#include "drv_vpss_instance.h"
#include "vpss_policy.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_FENCE_TIMEOUT_VALUE 300 /* ms */
#define VPSS_FENCE_TIME_MS  16000000 /* 16ms */

/* only for proc : need to be same as video.h */
static const hi_u8 *g_true_string[] = { "false", "true", "unknown" };
static const hi_u8 *g_hdr_type[] = {
    "SDR", "HDR10", "HLG", "CUVA", "SL_HDR", "HDR10PLUS", "DOLBY", "MAX"
};

static const hi_u8 *g_csc_primary[] = {
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
    "COLOR_MAX"
};
static const hi_u8 *g_csc_space[] = {
    "YUV",
    "RGB",
    "MAX"
};
static const hi_u8 *g_csc_quantify[] = {
    "LIMITED",
    "FULL",
    "MAX"
};
static const hi_u8 *g_csc_transfer[] = {
    "TYPE_GAMMA_SDR",
    "TYPE_GAMMA_HDR",
    "TYPE_PQ",
    "TYPE_HLG",
    "TYPE_XVYCC",
    "TYPE_MAX",
};
static const hi_u8 *g_csc_matrix[] = {
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
    "MAX",
};
static const hi_u8 *g_data_fmt[] = {
    "LINER", "TILE", "PKG", "AFBC", "MAX"
};
static const hi_u8 *g_cmp_mode[] = {
    "OFF", "SEG_LOSS", "SEGLOSSLESS", "LINE_LOSS", "LINELOSSLESS", "FRM_LOSS", "FRM_LOSSLESS", "MAX"
};
static const hi_u8 *g_sample_type[] = { "PROGRESSIVE", "INTERLACE", "UNKNOWN", "MAX" };
static const hi_u8 *g_bitwidth[] = { "8BIT", "10BIT", "12BIT", "MAX" };
static const hi_u8 *g_string_3d[] = { "2D", "SBS", "TAB", "FS",  "FPK", "MAX" };
static const hi_u8 *g_field_string[] = { "TOP", "BOTTOM", "ALL", "MAX" };
static const hi_u8 *g_buff_mode_string[] = { "TOP", "BOTTOM", "ALL", "MAX" };
static const hi_u8 *g_source_mode[] = {
    "DTV", "USB", "ATV", "SCART", "SVIDEO", "CVBS", "VGA", "YPBPR", "HDMI", "MEDIA", "NETWORK", "MAX"
};
static const hi_u8 *g_alg_mode_string[] = { "OFF", "ON", "AUTO", "MAX" };
static const hi_u8 *g_rotation_string[] = { "00", "90", "180", "270", "MAX" };
static const hi_u8 *g_port_type_string[] = { "norm", "npu", "3d_detect" };

hi_u8 *g_vpss_task_string[] = {
    "IDLE",
    "PREPARE",
    "CONFIG",
    "START",
    "WAIT",
    "SUCCESS",
    "CLEAR",
    "BUTT",
};

hi_u8 *g_vpss_work_mode[] = {
    "auto",
    "bypass ",
    "force bypass",
};

hi_u8 *g_vpss_prog_detect[] = {
    "force_prog",
    "force_inter",
    "auto",
    "off",
};

hi_u8 *g_vpss_3drs_string[] = {
    "V3",
    "V4",
    "V5",
    "MAX",
};

hi_u8 *g_vpss_node_type_string[] = {
    "2D_FRAME",
    "2D_4FIELD",
    "2D_5FIELD",
    "2D_3FIELD",
    "3D_FRAME_R",
    "PZME",
    "UHD",
    "UHD_HIGH_SPEED",
    "3DDET",
    "ZME_2L",
    "ROTATION_Y",
    "ROTATION_C",
    "8K",
    "ONLY_MANR_DBG",
    "MAX",
};

static hi_void vpss_state_proc(hi_void *p, vpss_instance *instance)
{
    osal_proc_print(p, "-------------------------------VPSS%04x STATE---------------------------------\n",
                    instance->id);
    /* todo more proc need add */
    osal_proc_print(p, "%-40s:%-12s \n", "state ",
                    g_vpss_task_string[instance->state]);
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12s/%-12s\n", "handle/ctrl_ip/enable/quick",
                    instance->id, instance->ctrl_id,
                    g_true_string[instance->enable],
                    g_true_string[instance->quick_out]);
    osal_proc_print(p, "%-40s:%-12s/%-12s\n", "suspend/pause",
                    g_true_string[instance->suspend], g_true_string[instance->pause]);
    osal_proc_print(p, "%-40s:%-12s/%-12s/%-12s\n", "work mode/use_pq_module/prog_detect",
                    g_vpss_work_mode[instance->work_mode], g_true_string[instance->use_pq_module],
                    g_vpss_prog_detect[instance->prog_detect]);
}

static hi_void vpss_algorithm_proc(hi_void *p, vpss_instance *instance)
{
    osal_proc_print(p, "-------------------------------VPSS%04x algorithm-----------------------------\n",
                    instance->id);
    osal_proc_print(p, "%-40s:%-12s/%-12d\n", "3drs_type/scan_num",
                    g_vpss_3drs_string[instance->alg_info.alg_state.en3_drs],
                    instance->alg_info.alg_state.me_scan_num);
    osal_proc_print(p, "%-40s:%-12s/%-12s/%-12s/%-12s\n", "dei_en/dei_ma_mode/tnr_en/tnr_ma_mode",
                    g_true_string[instance->vpss_alg_ctrl.dei_en],
                    g_true_string[instance->vpss_alg_ctrl.dei_ma_mode],
                    g_true_string[instance->vpss_alg_ctrl.tnr_en],
                    g_true_string[instance->vpss_alg_ctrl.tnr_ma_mode]);
    osal_proc_print(p, "%-40s:%-12s/%-12s/%-12s\n", "dm430_en/hdr_en/node_type",
                    g_true_string[instance->vpss_alg_ctrl.dm430_en],
                    g_true_string[instance->vpss_alg_ctrl.hdr_en],
                    g_vpss_node_type_string[instance->hal_info.node_type]);
}

static hi_void vpss_frame_ori_info_proc(hi_void *p, vpss_instance *instance)
{
    vpss_stream_info *stream_info = &instance->stream_info;
    hi_drv_vpss_video_frame *ori_frame = &stream_info->orignal_frame;

    osal_proc_print(p, "--------------------------------orignal frame_info-------------------------------\n");
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12s\n", "frame(W/H)/bit_width", ori_frame->comm_frame.width,
                    ori_frame->comm_frame.height, g_bitwidth[ori_frame->comm_frame.bit_width]);

    if ((ori_frame->comm_frame.hdr_type >= HI_DRV_HDR_TYPE_MAX) ||
        (ori_frame->comm_frame.color_space.color_primary >= HI_DRV_COLOR_PRIMARY_COLOR_MAX) ||
        (ori_frame->comm_frame.color_space.color_space >= HI_DRV_COLOR_CS_MAX)) {
        vpss_error("wrong ! hdr_type:%d, color_primary:%d, color_space:%d \n",
                   ori_frame->comm_frame.hdr_type,
                   ori_frame->comm_frame.color_space.color_primary,
                   ori_frame->comm_frame.color_space.color_space);
        return;
    }
    osal_proc_print(p, "%-40s:%-12s/%-20s/%-20s\n", "hdr_type/primary/space",
                    g_hdr_type[ori_frame->comm_frame.hdr_type],
                    g_csc_primary[ori_frame->comm_frame.color_space.color_primary],
                    g_csc_space[ori_frame->comm_frame.color_space.color_space]);

    if ((ori_frame->comm_frame.color_space.quantify_range >= HI_DRV_COLOR_RANGE_MAX) ||
        (ori_frame->comm_frame.color_space.transfer_type >= HI_DRV_COLOR_TRANSFER_TYPE_MAX) ||
        (ori_frame->comm_frame.color_space.matrix_coef >= HI_DRV_COLOR_MATRIX_COEFFS_MAX)) {
        vpss_error("wrong ! quantify_range:%d, transfer_type:%d, matrix_coef:%d \n",
                   ori_frame->comm_frame.color_space.quantify_range,
                   ori_frame->comm_frame.color_space.transfer_type,
                   ori_frame->comm_frame.color_space.matrix_coef);
        return;
    }
    osal_proc_print(p, "%-40s:%-12s/%-20s/%-20s\n", "quantify/transfer/matrix",
                    g_csc_quantify[ori_frame->comm_frame.color_space.quantify_range],
                    g_csc_transfer[ori_frame->comm_frame.color_space.transfer_type],
                    g_csc_matrix[ori_frame->comm_frame.color_space.matrix_coef]);

    osal_proc_print(p, "%-40s:%-12s/%-12s/%-12s/%-12s\n", "secure/data_fmt/cmp_mode/topfirst",
                    g_true_string[ori_frame->comm_frame.secure], g_data_fmt[ori_frame->comm_frame.cmp_info.data_fmt],
                    g_cmp_mode[ori_frame->comm_frame.cmp_info.cmp_mode],
                    g_true_string[ori_frame->comm_frame.top_field_first]);
    osal_proc_print(p, "%-40s:%-12s/%-12d/%-12s/%-12d\n", "sample_type/rate/3d_type/fmt",
                    g_sample_type[ori_frame->comm_frame.sample_type], ori_frame->comm_frame.frame_rate,
                    g_string_3d[ori_frame->comm_frame.video_3d_type], ori_frame->comm_frame.pixel_format);
    osal_proc_print(p, "%-40s:%-12s/%-12s/%-12s/%-12d\n", "source/field_mode/buff_mode/frame_index",
                    g_source_mode[ori_frame->comm_frame.video_private.source],
                    g_field_string[ori_frame->comm_frame.field_mode],
                    g_buff_mode_string[ori_frame->comm_frame.buf_valid_mode], ori_frame->comm_frame.frame_index);

    return;
}

static hi_void vpss_frame_revise_info_proc(hi_void *p, vpss_instance *instance)
{
    vpss_stream_info *stream_info = &instance->stream_info;
    hi_drv_vpss_video_frame *ori_frame = &stream_info->orignal_frame;
    hi_drv_vpss_video_frame *image = &instance->rev_new_frame;

    osal_proc_print(p, "--------------------------------revise frame_info---------------------------------\n");
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12s/%-12s\n", "frame(W/H)/bit_width/special_field", stream_info->width,
                    stream_info->height, g_bitwidth[instance->stream_info.bit_width],
                    g_true_string[instance->stream_info.special_field]);
    osal_proc_print(p, "%-40s:%-12s/%-12s/%-12s/%-12s\n", "secure/bypass/topfirst/real_top_first",
                    g_true_string[stream_info->secure], g_true_string[!stream_info->vpss_process],
                    g_true_string[stream_info->top_first], g_true_string[stream_info->real_topfirst]);
    osal_proc_print(p, "%-40s:%-12s/%-12d/%-12s/%-12d\n", "sample_type/rate/3d_type/fmt",
                    g_sample_type[stream_info->interlace], stream_info->in_rate,
                    g_string_3d[stream_info->video_3d_type], stream_info->pixel_format);
    osal_proc_print(p, "%-40s:%-12s/%-12s/%-12d/%-12d\n", "cmp_mode/data_fmt/ycmp_rate/ccmp_rate",
                    g_cmp_mode[ori_frame->comm_frame.cmp_info.cmp_mode],
                    g_data_fmt[ori_frame->comm_frame.cmp_info.data_fmt],
                    stream_info->cmp_info.ycmp_rate, stream_info->cmp_info.ccmp_rate);
    osal_proc_print(p, "%-40s:%-12s/%-12s/%-12s\n", "source/field_mode/pre_field_mode",
                    g_source_mode[stream_info->source], g_field_string[stream_info->field_mode],
                    g_field_string[stream_info->pre_fieldmode]);

    osal_proc_print(p, "--------------------------------prog_info_condition -------------------------------\n");
    osal_proc_print(p, "%-40s:%-12s/%-12d/%-12d/%-12d\n", "(P/I)/pixel_format/width/height",
                    g_sample_type[ori_frame->comm_frame.sample_type], image->comm_frame.pixel_format,
                    image->comm_frame.width, image->comm_frame.height);
    osal_proc_print(p, "%-40s:%-12s/%-12d/%-12s\n", "3d_type/framerate/fieldmode",
                    g_string_3d[image->comm_frame.video_3d_type],
                    image->comm_frame.frame_rate, g_field_string[image->comm_frame.field_mode]);

    return;
}


static hi_void vpss_proc_in_buff_list_info(hi_void *p, list *frame_list)
{
    hi_u32 i = 0;
    list *pos = HI_NULL;
    list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_s32 ret = HI_SUCCESS;
    vpss_in_node *temp_node = HI_NULL;
    vpss_in_node *frame_node[DEF_IN_NODE_MAX_NUM] = { 0 };
    hi_drv_win_vpss_meta_info *metadata_info = HI_NULL;

    osal_list_for_each_safe(pos, n, frame_list) {
        temp_node = osal_list_entry(pos, vpss_in_node, node);
        list_entry_times++;
        if (list_entry_times > DEF_IN_NODE_MAX_NUM) {
            vpss_error("list error\n");
            ret = HI_FAILURE;
            break;
        }

        if (temp_node == HI_NULL) {
            vpss_error("node is NULL\n");
            ret = HI_FAILURE;
            break;
        }
        frame_node[list_entry_times - 1] = temp_node;
    }
    if (ret != HI_SUCCESS) {
        return;
    }

    for (i = 0; i < list_entry_times; i++) {
        osal_proc_print(p, "[%06d,0x%08llx]",
                        frame_node[i]->in_data.frame_index, frame_node[i]->in_data.buf_addr[0].dma_handle);

        metadata_info = (hi_drv_win_vpss_meta_info *)frame_node[i]->in_data.video_private.win_private.data;
        if (metadata_info == HI_NULL) {
            if ((i % 2) == 1) { /* 2 change print line */
                osal_proc_print(p, "\n");
                osal_proc_print(p, "%-40s:", " ");
            }
            continue;
        }
        osal_proc_print(p, "[0x%llx %d %d %d %d] ",
            (hi_u64)metadata_info, metadata_info->vpss_process,
            metadata_info->force_bypass, metadata_info->force_pq_close, metadata_info->play_index);

        if ((i % 2) == 1) { /* 2 change print line */
            osal_proc_print(p, "\n");
            osal_proc_print(p, "%-40s:", " ");
        }
    }
    osal_proc_print(p, "\n");
}

static hi_void vpss_proc_out_buff_list_info(hi_void *p, list *frame_list)
{
    hi_u32 i = 0;
    list *pos = HI_NULL;
    list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_s32 ret = HI_SUCCESS;
    vpss_out_node *temp_node = HI_NULL;
    vpss_out_node *frame_node[DEF_OUT_NODE_MAX_NUM] = { 0 };
    hi_drv_win_vpss_meta_info *metadata_info = HI_NULL;

    osal_list_for_each_safe(pos, n, frame_list) {
        temp_node = osal_list_entry(pos, vpss_out_node, node);
        list_entry_times++;
        if (list_entry_times > DEF_OUT_NODE_MAX_NUM) {
            vpss_error("list error\n");
            ret = HI_FAILURE;
            break;
        }

        if (temp_node == HI_NULL) {
            vpss_error("node is NULL\n");
            ret = HI_FAILURE;
            break;
        }
        frame_node[list_entry_times - 1] = temp_node;
    }
    if (ret != HI_SUCCESS) {
        return;
    }

    for (i = 0; i < list_entry_times; i++) {
        osal_proc_print(p, "[%06d,0x%08llx]",
                        frame_node[i]->out_priv_frame.comm_frame.frame_index,
                        frame_node[i]->out_priv_frame.comm_frame.buf_addr[0].dma_handle);

        metadata_info = (hi_drv_win_vpss_meta_info *)frame_node[i]->out_frame.video_private.vpss_private.data;
        if (metadata_info == HI_NULL) {
            if ((i % 2) == 1) { /* 2 change print line */
                osal_proc_print(p, "\n");
                osal_proc_print(p, "%-40s:", " ");
            }
            continue;
        }

        osal_proc_print(p, "[0x%llx %d %d %d %d] ",
            (hi_u64)metadata_info, metadata_info->vpss_process,
            metadata_info->force_bypass, metadata_info->force_pq_close, metadata_info->play_index);

        if ((i % 2) == 1) { /* 2 change print line */
            osal_proc_print(p, "\n");
            osal_proc_print(p, "%-40s:", " ");
        }
    }
    osal_proc_print(p, "\n");
}

hi_void vpss_proc_in_buffer_node_info(hi_void *p, vpss_instance *instance)
{
    unsigned long flag = 0;
    vpss_in *in_list = HI_NULL;
    in_list = &instance->in_list;

    osal_proc_print(p, "------------------------------frame in node info---------------------------------\n");

    vpss_comm_down_spin(&in_list->src_in_lock, &flag);

    osal_proc_print(p, "%-40s:", "free_list frame [index, addr, meta]");
    vpss_proc_in_buff_list_info(p, &instance->in_list.free_list);

    osal_proc_print(p, "%-40s:", "busy_list frame [index, addr, meta]");
    vpss_proc_in_buff_list_info(p, &instance->in_list.busy_list);

    osal_proc_print(p, "%-40s:", "wait_list frame [index, addr, meta]");
    vpss_proc_in_buff_list_info(p, &instance->in_list.wait_rel_list);

    osal_proc_print(p, "%-40s:", "rel_list frame  [index, addr, meta]");
    vpss_proc_in_buff_list_info(p, &instance->in_list.release_list);

    vpss_comm_up_spin(&in_list->src_in_lock, &flag);
}

static hi_void vpss_source_frame_list_proc(hi_void *p, vpss_instance *instance)
{
    osal_proc_print(p, "-------------------------------source_frame_list info------------------------------\n");
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "get_src_hz(try/ok)/get_out_hz(try/ok)",
                    instance->perfor.img_dfx.get_try_rate, instance->perfor.img_dfx.get_suc_rate,
                    instance->perfor.buf_dfx.try_rate, instance->perfor.buf_dfx.try_suc_rate);
    osal_proc_print(p, "%-40s:%-12d/%-12d\n", "hz:check_process(try/ok)",
                    instance->perfor.check_dfx.try_rate, instance->perfor.check_dfx.try_suc_rate);
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "cnt:in_queue(try/ok)/deque(try/ok)",
                    instance->perfor.in_dfx.queue_cnt, instance->perfor.in_dfx.queue_suc_cnt,
                    instance->perfor.in_dfx.dequeue_cnt, instance->perfor.in_dfx.dequeue_suc_cnt);

    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "cnt:out_queue(try/ok)/out_deque(try/ok)",
                    instance->perfor.out_dfx.queue_cnt, instance->perfor.out_dfx.queue_suc_cnt,
                    instance->perfor.out_dfx.dequeue_cnt, instance->perfor.out_dfx.dequeue_suc_cnt);

    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "cnt:out_acq(try/ok)/out_rel(try/ok)",
                    instance->perfor.out_dfx.acquire_cnt, instance->perfor.out_dfx.acquire_suc_cnt,
                    instance->perfor.out_dfx.release_cnt, instance->perfor.out_dfx.release_suc_cnt);

    osal_proc_print(p, "%-40s:%-12d/%-12d\n", "cnt:out_que_trans(try/ok)",
                    instance->perfor.out_dfx.queue_trans_cnt, instance->perfor.out_dfx.queue_trans_suc_cnt);
    osal_proc_print(p, "%-40s:%-12d/%-12d\n", "cnt:rel_complete(try/ok)",
                    instance->perfor.src_dfx.rel_complete_cnt, instance->perfor.src_dfx.rel_complete_suc_cnt);
    osal_proc_print(p, "%-40s:%-12d/%-12d\n", "cnt:src_rel/out_rel_trans",
                    instance->perfor.src_dfx.rel_cnt, instance->perfor.out_dfx.release_trans_cnt);
    osal_proc_print(p, "%-40s:%-12d\n", "clear_task_cnt", instance->perfor.cnt_dfx.error_count);
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "cnt:src_list(put/complete/rel/full)",
                    instance->src_list.put_src_count, instance->src_list.complete_src_count,
                    instance->src_list.release_src_count, instance->src_list.ful_src_list_num);
    osal_proc_print(p, "%-40s:%-12d/%-12d\n", "cnt:src_list_total(put/rel)",
                    instance->src_list.put_src_total, instance->src_list.release_src_total);
    osal_proc_print(p, "%-40s:%-12d/%-12d\n", "cnt:trans_fb_list(put/rel)",
                    instance->perfor.out_dfx.queue_trans_suc_cnt,
                    instance->perfor.out_dfx.release_trans_cnt);
    osal_proc_print(p, "-----------------------------map umap count info-----------------------------------\n");
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "cnt:map(try/ok)/umap(try/ok)",
                    instance->perfor.buf_dfx.try_map_cnt, instance->perfor.buf_dfx.map_suc_cnt,
                    instance->perfor.buf_dfx.try_umap_cnt, instance->perfor.buf_dfx.umap_suc_cnt);

    osal_proc_print(p, "-----------------------------reference count info-----------------------------------\n");
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "cnt:inc_ref(try/ok)/dec_ref(try/pk)",
                    instance->perfor.refcnt_dfx.try_inc_cnt, instance->perfor.refcnt_dfx.try_inc_suc_cnt,
                    instance->perfor.refcnt_dfx.try_dec_cnt + instance->src_list.try_decrease_ref +
                    instance->in_list.try_decrease_ref,
                    instance->perfor.refcnt_dfx.try_dec_suc_cnt + instance->src_list.suc_decrease_ref +
                    instance->in_list.suc_decrease_ref);

#if !((defined HI_VPSS_DRV_USE_GOLDEN) || (defined HI_VPSS_DRV_USE_GOLDEN_COEF))
    vpss_proc_in_buffer_node_info(p, instance);
#endif
}

static hi_void vpss_port_info_proc_tunl(hi_void *p, vpss_instance *instance, vpss_port *port)
{
    osal_proc_print(p, "--------------------------------tunl info-------------------------------------\n");
    osal_proc_print(p, "%-40s:%-12d/%-12d\n", "in_wtunl/in_rtunl",
                    instance->hal_info.in_wtunl_en, instance->hal_info.in_rtunl_en);
    osal_proc_print(p, "%-40s:%-12d/%-12d\n", "out_wtunl/out_rtunl",
                    instance->hal_info.out_wtunl_en, instance->hal_info.out_rtunl_en);

    return;
}

static hi_void vpss_port_info_part1_proc(hi_void *p, vpss_instance *instance, vpss_port *port)
{
    osal_proc_print(p, "------------------------------port[0x%x] info--------------------------------------\n",
                    port->port_id);
    osal_proc_print(p, "%-40s:%-12s/%-12s/%-12s/%-12d\n", "state/bit_width/3d_support/out_count",
                    g_alg_mode_string[port->enble], g_bitwidth[port->bit_width],
                    g_alg_mode_string[port->enable_3d], port->out_count);
    osal_proc_print(p, "%-40s:%-12d/%-12s/%-12s\n", "data_fmt/rotation/user_crop",
                    port->format, g_rotation_string[port->rotation],
                    g_alg_mode_string[port->user_crop_en]);
    osal_proc_print(p, "%-40s:%-12s/%-12s/%-12d/%-12d\n", "cmp_mode/data_fmt/ycmp_rate/ycmp_rate",
                    g_cmp_mode[port->cmp_info.cmp_mode], g_data_fmt[port->cmp_info.data_fmt],
                    port->cmp_info.ycmp_rate, port->cmp_info.ccmp_rate);

    if ((port->hdr_follow_src >= 2) || /* 2:bool max value */
        (port->out_hdr_type >= HI_DRV_HDR_TYPE_MAX) ||
        (port->out_color_desp.color_primary >= HI_DRV_COLOR_PRIMARY_COLOR_MAX) ||
        (port->out_color_desp.color_space >= HI_DRV_COLOR_CS_MAX)) {
        vpss_error("wrong ! hdr_follow_src:%d, out_hdr_type:%d, color_primary:%d, color_space:%d \n",
                   port->hdr_follow_src, port->out_hdr_type,
                   port->out_color_desp.color_primary, port->out_color_desp.color_space);
        return;
    }
    osal_proc_print(p, "%-40s:%-12s/%-12s/%-20s/%-20s\n", "hdr_follow_src/hdr_type/primary/space",
                    g_true_string[port->hdr_follow_src],
                    g_hdr_type[port->out_hdr_type],
                    g_csc_primary[port->out_color_desp.color_primary],
                    g_csc_space[port->out_color_desp.color_space]);

    if ((port->out_color_desp.quantify_range >= HI_DRV_COLOR_RANGE_MAX) ||
        (port->out_color_desp.transfer_type >= HI_DRV_COLOR_TRANSFER_TYPE_MAX) ||
        (port->out_color_desp.matrix_coef >= HI_DRV_COLOR_MATRIX_COEFFS_MAX)) {
        vpss_error("wrong ! quantify_range:%d, transfer_type:%d, matrix_coef:%d \n",
                   port->out_color_desp.quantify_range,
                   port->out_color_desp.transfer_type,
                   port->out_color_desp.matrix_coef);
        return;
    }
    osal_proc_print(p, "%-40s:%-20s/%-20s/%-20s\n", "quantify/transfer/matrix",
                    g_csc_quantify[port->out_color_desp.quantify_range],
                    g_csc_transfer[port->out_color_desp.transfer_type],
                    g_csc_matrix[port->out_color_desp.matrix_coef]);

    return;
}

static hi_void vpss_port_info_part2_proc(hi_void *p, vpss_instance *instance, vpss_port *port)
{
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12s\n", "out_width/out_height/port_type",
                    (port->out_rect.rect_w == 0) ? instance->stream_info.width : port->out_rect.rect_w,
                    (port->out_rect.rect_h == 0) ? instance->stream_info.height : port->out_rect.rect_h,
                    g_port_type_string[port->port_type]);
    osal_proc_print(p, "%-40s:%-12d/%-12s/%-12s/%-12s\n", "max_framerate/lowdelay/h_flip/v_flip",
                    port->max_frame_rate, g_alg_mode_string[port->tunnel_enable],
                    g_alg_mode_string[port->hori_flip], g_alg_mode_string[port->vert_flip]);
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "out_rect(X/Y/W/H)",
                    port->out_rect.rect_x, port->out_rect.rect_y,
                    port->out_rect.rect_w, port->out_rect.rect_h);
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "video_rect(X/Y/W/H)",
                    port->video_rect.rect_x, port->video_rect.rect_y,
                    port->video_rect.rect_w, port->video_rect.rect_h);
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "crop_rect(X/Y/W/H)",
                    port->crop_rect.rect_x, port->crop_rect.rect_y,
                    port->crop_rect.rect_w, port->crop_rect.rect_h);
    osal_proc_print(p, "%-40s:%-2d/%-12d/%-12d/%-12d/%-12d\n", "user_crop_en, offset_rect(L/R/T/B)",
                    port->user_crop_en, port->offset_rect.left_offset, port->offset_rect.right_offset,
                    port->offset_rect.top_offset, port->offset_rect.bottom_offset);
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "revise_crop_rect(X/Y/W/H)",
                    port->revise_crop_rect.rect_x, port->revise_crop_rect.rect_y,
                    port->revise_crop_rect.rect_w, port->revise_crop_rect.rect_h);
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "revise_video_rect(X/Y/W/H)",
                    port->revise_video_rect.rect_x, port->revise_video_rect.rect_y,
                    port->revise_video_rect.rect_w, port->revise_video_rect.rect_h);
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "revise_out_rect(L/R/T/B)",
                    port->revise_out_rect.rect_x, port->revise_out_rect.rect_y,
                    port->revise_out_rect.rect_w, port->revise_out_rect.rect_h);
    osal_proc_print(p, "%-40s:%-12s/%-12d/0x%llx/%-12d\n", "dst_id/hw_idx/hw_addr/stride",
                    "VDP", instance->hal_info.port_info[0].out_info.hw_index,
                    instance->hal_info.port_info[0].out_info.hal_addr.phy_addr_y,
                    instance->hal_info.port_info[0].out_info.hal_addr.stride_y);

    return;
}

hi_void vpss_proc_out_buffer_node_info(hi_void *p, vpss_port *port)
{
    unsigned long flag = 0;
    vpss_out *out_list = HI_NULL;
    out_list = &port->out_list;

    osal_proc_print(p, "-----------------------------frame out node info---------------------------------\n");
    osal_proc_print(p, "%-40s:", "empty_buf frame [index, addr, meta]");
    vpss_comm_down_spin(&port->out_list.empty_buf_spin, &flag);
    vpss_proc_out_buff_list_info(p, &port->out_list.empty_frm_list);
    vpss_comm_up_spin(&port->out_list.empty_buf_spin, &flag);

    osal_proc_print(p, "%-40s:", "ful_frm frame   [index, addr, meta]");
    vpss_comm_down_spin(&port->out_list.ful_buf_spin, &flag);
    vpss_proc_out_buff_list_info(p, &port->out_list.ful_frm_list);
    vpss_comm_up_spin(&port->out_list.ful_buf_spin, &flag);
}

static hi_void vpss_out_frame_list_proc(hi_void *p, vpss_port *port, vpss_fb_state *port_state)
{
    hi_u8 *g_buf_type_string[] = { "vpss", "usr", "unknow" };

    osal_proc_print(p, "-----------------------------out_frame_list info---------------------------------\n");
    osal_proc_print(p, "%-40s:%-12s/%-12d/%-12d/%-12d\n", "buf_manager/buf_num/buf_ful/buf_empty",
                    g_buf_type_string[port->buf_type],
                    port->out_list.attr.buf_num,
                    port_state->ful_list_numb, port_state->empty_list_numb);
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "acquire_hz/releasing/waiting/working",
                    port_state->get_hz, port_state->wait_sink_rls_numb,
                    port_state->ful_list_numb - port_state->wait_sink_rls_numb,
                    port->out_list.attr.buf_num - port_state->ful_list_numb
                    - port_state->empty_list_numb);
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "acquire(try/OK)/release(try/OK)",
                    port_state->get_total, port_state->get_success,
                    port_state->rel_total, port_state->rel_success);
    osal_proc_print(p, "%-40s:%-12d/%-12d\n", "acquire_success/release_success",
                    port_state->get_success_total, port_state->rel_success_total);
    osal_proc_print(p, "%-40s:%-12d\n", "out_rate", port_state->out_rate);
#if !((defined HI_VPSS_DRV_USE_GOLDEN) || (defined HI_VPSS_DRV_USE_GOLDEN_COEF))
    vpss_proc_out_buffer_node_info(p, port);
#endif
}

static hi_void vpss_port_proc(hi_void *p, vpss_instance *instance)
{
    hi_u32 i = 0;
    vpss_port *port = HI_NULL;
    vpss_fb_state port_state = { 0 };

    for (i = 0; i < VPSS_PORT_MAX_NUM; i++) {
        port = &(instance->port[i]);

        if ((port->port_id == HI_INVALID_HANDLE) || (port->enble == HI_FALSE)) {
            continue;
        }

        vpss_port_info_part1_proc(p, instance, port);
        vpss_port_info_part2_proc(p, instance, port);
        vpss_port_info_proc_tunl(p, instance, port);
        vpss_out_get_state(&(port->out_list), &port_state);
        vpss_out_frame_list_proc(p, port, &port_state);
    }
}

static hi_void vpss_performance_statistics_complete_proc(hi_void *p, vpss_inst_perfor *performance,
    vpss_inst_proc_perf *proc_performance)
{
    hi_u32 i;

    osal_proc_print(p, "%-40s:%-12lld/%-12lld/%-12lld\n", "complete time:send/rel/complete",
                    proc_performance->proc_complete_send_time,
                    proc_performance->proc_complete_rel_time,
                    proc_performance->proc_complete_complete_time);

    for (i = 0; i < VPSS_PORT_MAX_NUM; i++) {
        osal_proc_print(p, "%-40s:%-12lld/%-12lld/%-12lld/%-12lld\n", "complete time:copy1/copy2/copy3/q",
                        proc_performance->proc_complete_send1_time[i],
                        proc_performance->proc_complete_send2_time[i],
                        proc_performance->proc_complete_send3_time[i],
                        proc_performance->proc_complete_send_q_time[i]);
    }

    osal_proc_print(p, "%-40s:%-12lld/%-12lld\n", "time:complete_task/pq_stt",
                    proc_performance->proc_complete_time, proc_performance->proc_pq_stt_time);

    return;
}

static hi_void vpss_performance_statistics_proc(hi_void *p, vpss_instance *instance)
{
    vpss_inst_perfor *performance = HI_NULL;
    vpss_inst_proc_perf *proc_performance = HI_NULL;

    performance = &(instance->perfor);
    proc_performance = &(instance->proc_perfor);
    osal_proc_print(p, "-------------------------performance statistics -----------------------------------\n");
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d/%-12d\n", "cnt:src_chg/src_drop/out_drop/reset",
                    performance->cnt_dfx.scence_chg_cnt, performance->cnt_dfx.source_frame_drop_cnt,
                    performance->cnt_dfx.frame_drop_count, performance->cnt_dfx.reset_cnt);
    osal_proc_print(p, "%-40s:%-12lld\n", "fence_time", proc_performance->proc_fence_time);
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d\n", "cnt:fence_err/time_err/time_warn",
                    performance->fence_dfx.err_cnt,
                    performance->fence_dfx.time_err_cnt,
                    performance->fence_dfx.time_warn_cnt);
    osal_proc_print(p, "%-40s:%-12d/%-12d/%-12d\n", "task_cfg_err_cnt/2d/3d/rote",
                    performance->config_dfx.task_2d_err_cnt,
                    performance->config_dfx.task_3d_err_cnt,
                    performance->config_dfx.task_rota_err_cnt);
    osal_proc_print(p, "%-40s:%-12lld/%-12lld/%-12lld\n", "time:acq/create_buffer/destroy_buffer",
                    performance->buf_dfx.buf_acq_time_end - performance->buf_dfx.buf_acq_time_start,
                    performance->buf_dfx.buf_create_time_end - performance->buf_dfx.buf_create_time_start,
                    performance->buf_dfx.buf_destroy_time_end - performance->buf_dfx.buf_destroy_time_start);
    osal_proc_print(p, "%-40s:%-12lld/%-12lld/%-12lld\n", "time:check/dq/prepare",
                    proc_performance->proc_check_time,
                    performance->buf_dfx.dq_time_end - performance->buf_dfx.dq_time_start,
                    proc_performance->proc_prepare_time);
    osal_proc_print(p, "%-40s:%-12lld/%-12lld\n", "time:config_task/start_task",
                    proc_performance->proc_config_time, proc_performance->proc_start_time);

    osal_proc_print(p, "%-40s:%-12lld/%-12lld/%-12lld/%-12lld\n", "time:pq_timing/2d_task/rot_task/3d_task",
                    performance->pq_dfx.init_time_end - performance->pq_dfx.init_time_start,
                    performance->config_dfx.task_2d_time_end - performance->config_dfx.task_2d_time_start,
                    performance->config_dfx.task_rota_time_end - performance->config_dfx.task_rota_time_start,
                    performance->config_dfx.task_3d_time_end - performance->config_dfx.task_3d_time_start);

    osal_proc_print(p, "%-40s:%-12lld/%-12lld/%-12lld\n", "time:fill_hal/set_node/vir_task",
                    performance->config_dfx.fill_hal_time_end - performance->config_dfx.fill_hal_time_start,
                    performance->config_dfx.set_node_time_end - performance->config_dfx.set_node_time_start,
                    performance->config_dfx.vir_node_time_end - performance->config_dfx.vir_node_time_start);
    osal_proc_print(p, "%-40s:%-12lld\n", "logic_time", proc_performance->proc_logic_time);

    vpss_performance_statistics_complete_proc(p, performance, proc_performance);

    osal_proc_print(p, "%-40s:%-12lld/%-12d\n", "total_time/time_out_cnt",
                    proc_performance->proc_total_time, instance->perfor.cnt_dfx.time_out_count);
}

vpss_src_mode get_src_list_mode(hi_drv_vpss_video_frame *src_frame)
{
    if (src_frame->comm_frame.sample_type != HI_DRV_SAMPLE_TYPE_INTERLACE) {
        return SRC_MODE_FRAME;
    }

    if (src_frame->comm_frame.field_mode != HI_DRV_FIELD_ALL) {
        return SRC_MODE_SPEC_FIELD;
    }

    return SRC_MODE_FIELD;
}

hi_bool check_inst_need_drop_by_rate(vpss_instance *instance, hi_u32 out_rate, hi_u32 out_count)
{
    hi_u32 multiple;
    hi_u32 quote;
    hi_bool need_drop = HI_FALSE;

    if (instance->stream_info.in_rate < out_rate || out_rate == 0) {
        need_drop = HI_FALSE;
    } else {
        /* 10 is used to cal */
        multiple = instance->stream_info.in_rate * 10 / out_rate; /* 10 calc para */
        /* 5 and 10 are used to cal */
        quote = (multiple + 5) / 10; /* 5/10 calc para */
        if (quote == 0) {
            vpss_error("need_drop quote == 0, please check\n");
            return HI_FALSE;
        }

        if (out_count % quote == 1) {
            need_drop = HI_TRUE;
        } else {
            need_drop = HI_FALSE;
        }
    }

    /* todo:we will modify */
    return need_drop;
}

hi_s32 vpss_inst_update_vpss_pq_reg(vpss_instance *instance)
{
    hi_drv_pq_vpss_stt_info *stt_info = HI_NULL;
    hi_drv_pq_vpss_cfg_info *cfg_info = HI_NULL;
    /* todo : check this intf with pq */
    return vpss_comm_pq_update_pq_reg(stt_info, cfg_info, &instance->hal_info.pq_reg_data);
}

hi_void vpss_inst_print_pq_info(hi_drv_pq_vpss_stt_info *info_in, hi_drv_pq_vpss_cfg_info *info_out)
{
    vpss_info("run_alg_in frame_index:%d handle_id:%d bit_width:%d w:%d h:%d frame_fmt:%d"
              "nr_en:%d dei_en:%d ifmd_en:%d db_en:%d dm_en:%d scd:%d stride:%d frame_rate:%d\n",
              info_in->frame_index, info_in->handle_id, info_in->bit_width, info_in->width,
              info_in->height, info_in->frame_fmt,
              info_in->alg_en.nr_en, info_in->alg_en.dei_en, info_in->alg_en.ifmd_en,
              info_in->alg_en.db_en, info_in->alg_en.dm_en,
              info_in->scd, info_in->stride, info_in->frame_rate);
    vpss_info("run_alg_out dir_mch:%d die_out_sel:%d field_order:%d film_mode:%d key_frame:%d\n",
              info_out->ifmd_playback.dir_mch, info_out->ifmd_playback.die_out_sel,
              info_out->ifmd_playback.field_order, info_out->ifmd_playback.film_mode,
              info_out->ifmd_playback.key_frame);
}

hi_s32 vpss_inst_update_pq_timing_info(vpss_instance *instance, hi_drv_vpss_video_frame *src_image)
{
    hi_drv_pq_vpss_info timing_info;
    hi_s32 ret;

    if (instance->use_pq_module != HI_TRUE) {
        vpss_dbg("vpss not use pq alg now !!\n");
        return HI_SUCCESS;
    }

    timing_info.handle_id = instance->id;
    timing_info.frame_width = src_image->comm_frame.width;
    timing_info.frame_height = src_image->comm_frame.height;
    timing_info.frame_rate = src_image->comm_frame.frame_rate;
    timing_info.frame_fmt = HI_DRV_PQ_FRM_FRAME; /* do no need */
    timing_info.hdr_type_in = src_image->comm_frame.hdr_type;
    /* one instance support 2/3 port : just use port0 */
    timing_info.hdr_type_out = instance->port[0].out_hdr_type;

    instance->alg_info.pq_timing = timing_info;

    if (instance->global_dbg_ctrl->print_pq_info == HI_TRUE) {
        vpss_info("pq timing handle_id %d frame_width %d frame_height %d frame_rate %d"
                  "frame_fmt %d hdr_type_in %d hdr_type_out %d\n",
                  timing_info.handle_id, timing_info.frame_width,
                  timing_info.frame_height, timing_info.frame_rate,
                  timing_info.frame_fmt, timing_info.hdr_type_in, timing_info.hdr_type_out);
    }

    ret = vpss_comm_pq_update_timing_info(instance->id, &timing_info, &instance->hal_info.pq_reg_data);

    return ret;
}

static hi_void vpss_inst_fill_rgmv_addr(vpss_instance *instance, vpss_hal_stt_rgme_cfg *rgme_cfg,
                                        vpss_hal_stt_soft_cfg *global_cfg, hi_drv_pq_vpss_stt_info *info_in)
{
    drv_vpss_mem_info smmu_buf = { 0 };
    drv_vpss_mem_info mmz_buf = { 0 };

    info_in->rgmv_reg_addr = rgme_cfg->w_rgmv_chn.vir_addr;
    info_in->stride = rgme_cfg->w_rgmv_chn.stride;

    smmu_buf.vir_addr = rgme_cfg->w_rgmv_chn.vir_addr;
    smmu_buf.phy_addr = rgme_cfg->w_rgmv_chn.phy_addr;
    smmu_buf.size = rgme_cfg->w_rgmv_chn.size;
    vpss_comm_mem_flush(&smmu_buf);

    mmz_buf.vir_addr = global_cfg->w_stt_chn.vir_addr;
    mmz_buf.phy_addr = global_cfg->w_stt_chn.phy_addr;
    mmz_buf.size = global_cfg->w_stt_chn.size;
    vpss_comm_mem_flush(&mmz_buf);

    return;
}

hi_void invoke_pq_function(vpss_instance *instance)
{
    hi_s32 ret;
    hi_drv_pq_vpss_stt_info *stt_in = HI_NULL;
    hi_drv_pq_vpss_cfg_info *info_out = HI_NULL;
    vpss_hal_stt_soft_cfg *global_cfg = HI_NULL;
    vpss_hal_stt_rgme_cfg *rgme_cfg = HI_NULL;
    drv_vpss_mem_info mem_info = { 0 };

    if ((instance->use_pq_module != HI_TRUE) || (instance->pause == HI_TRUE)) {
        return;
    }

    info_out = &instance->alg_info.alg_out_para;
    global_cfg = &instance->hal_info.stt_cfg.global_cfg;
    if ((global_cfg->w_stt_chn.vir_addr == HI_NULL) || (global_cfg->w_me1_stt_chn.vir_addr == HI_NULL) ||
        (global_cfg->w_me1_stt_chn.vir_addr == HI_NULL)) {
        return;
    }
    rgme_cfg = &instance->hal_info.stt_cfg.rgme_cfg;
    instance->perfor.pq_dfx.stt_time_start = vpss_comm_get_sched_time();

    stt_in = &instance->alg_info.alg_in_para;
    stt_in->frame_index = instance->cur_process_frame_info.comm_frame.frame_index;
    stt_in->handle_id = instance->id;
    stt_in->bit_width = instance->stream_info.bit_width;
    stt_in->width = instance->alg_info.pq_timing.frame_width;
    stt_in->height = instance->alg_info.pq_timing.frame_height;
    stt_in->alg_en.nr_en = instance->alg_info.alg_state.alg_ctrl.nr_en;
    stt_in->alg_en.dei_en = instance->alg_info.alg_state.alg_ctrl.dei_en;
    stt_in->alg_en.ifmd_en = stt_in->alg_en.dei_en;
    stt_in->alg_en.db_en = instance->alg_info.alg_state.alg_ctrl.db_en;
    stt_in->alg_en.dm_en = instance->alg_info.alg_state.alg_ctrl.dm_en;
    if (stt_in->alg_en.dei_en == HI_TRUE) {
        stt_in->frame_fmt = HI_DRV_PQ_FRM_TOP_FIELD; /* no frame means field */
    } else {
        stt_in->frame_fmt = HI_DRV_PQ_FRM_FRAME;
    }

    stt_in->stream_id = 0;
    stt_in->frame_rate = instance->stream_info.in_rate;
    if (instance->stream_info.top_first == HI_TRUE) {
        stt_in->vdec_info.field_order = HI_DRV_PQ_FIELD_TOP_FIRST;
    } else {
        stt_in->vdec_info.field_order = HI_DRV_PQ_FIELD_BOTTOM_FIRST;
    }

    stt_in->vpss_stt_reg = global_cfg->w_stt_chn.vir_addr;
    stt_in->scan_stt_reg = global_cfg->w_me2_stt_chn.vir_addr;
    stt_in->nr_stt_reg = global_cfg->w_me1_stt_chn.vir_addr;

    vpss_inst_fill_rgmv_addr(instance, rgme_cfg, global_cfg, stt_in);

    stt_in->scd = info_out->ifmd_playback.die_out_sel;
    stt_in->ref_field = instance->hal_info.in_info.field_mode;
    /* ifmd result */
    vpss_comm_pq_run_alg(stt_in, info_out);

    /* attention : stream topfirst, pq topfirst */
    if (info_out->ifmd_playback.field_order == HI_DRV_PQ_FIELD_TOP_FIRST) {
        instance->stream_info.real_topfirst = HI_TRUE;
    } else  if (info_out->ifmd_playback.field_order == HI_DRV_PQ_FIELD_BOTTOM_FIRST) {
        instance->stream_info.real_topfirst = HI_FALSE;
    } else {
        instance->stream_info.real_topfirst = instance->stream_info.top_first;
    }

    if (instance->global_dbg_ctrl->print_pq_info) {
        vpss_inst_print_pq_info(stt_in, info_out);
    }
    ret = vpss_comm_pq_run_stt_alg(stt_in);

    mem_info.vir_addr = global_cfg->w_stt_chn.vir_addr;
    mem_info.phy_addr = global_cfg->w_stt_chn.phy_addr;
    mem_info.size = global_cfg->w_stt_chn.size;
    vpss_comm_mem_flush(&mem_info);

    instance->perfor.pq_dfx.stt_time_end = vpss_comm_get_sched_time();
}

hi_void get_out_frame_info(vpss_instance *instance, hi_u32 port_id, vpss_out_frame_info *out_frame_attr)
{
    vpss_port *port = HI_NULL;

    port = &instance->port[port_id];
    out_frame_attr->width = port->revise_out_rect.rect_w;
    out_frame_attr->height = port->revise_out_rect.rect_h;
    out_frame_attr->secure = instance->stream_info.secure;
    out_frame_attr->bit_width = port->bit_width;
    out_frame_attr->pixel_format = port->format;
    out_frame_attr->cmp_info = port->cmp_info;
    out_frame_attr->video_3d_type = instance->stream_info.video_3d_type;
    out_frame_attr->vpss_process = HI_TRUE;

    return;
}

hi_bool check_frame_need_drop(vpss_instance *instance, vpss_port *port, hi_drv_vpss_video_frame *out_frame)
{
    hi_bool need_drop = HI_FALSE;
    port->out_count++;
    need_drop = check_inst_need_drop_by_rate(instance, port->max_frame_rate, port->out_count);

    if (instance->stream_info.interlace == HI_TRUE) {
        if ((instance->wbc_list[0].complete_count == VPSS_OUTPUT_FRAME_FIRST) ||
            (instance->wbc_list[0].complete_count == VPSS_OUTPUT_FRAME_THIRD)) {
            need_drop = HI_TRUE;
        }
    }

    /* if the last frame  ,it can't be dropped */
    if (out_frame->comm_frame.video_private.last_flag == HI_DRV_LAST_ERROR_FLAG) {
        need_drop = HI_FALSE;
    }

    if (need_drop == HI_TRUE) {
        instance->perfor.cnt_dfx.frame_drop_count++;
    }

#ifdef HI_VPSS_DRV_USE_GOLDEN
    return HI_FALSE;
#endif
    return need_drop;
}

hi_void fill_rotate_frame_addr_info(vpss_buffer *buf, hi_drv_vpss_video_frame *frm, hi_drv_3d_eye_type en3_dtype)
{
    hi_u64 phy_addr = buf->buff_info.phy_addr;
    hi_u32 stride_y = buf->stride_y;
    hi_u32 stride_c = buf->stride_c;

    vpss_check_zero(stride_y);

    frm->vpss_buf_addr[en3_dtype].stride_y = stride_y;
    frm->vpss_buf_addr[en3_dtype].stride_c = stride_c;

    if (en3_dtype == HI_DRV_3D_EYE_LEFT) {
        frm->vpss_buf_addr[en3_dtype].phy_addr_y = phy_addr;
        frm->vpss_buf_addr[en3_dtype].phy_addr_y_offset = 0;
        frm->vpss_buf_addr[en3_dtype].phy_addr_c = phy_addr + stride_y * frm->comm_frame.height;
        frm->vpss_buf_addr[en3_dtype].phy_addr_c_offset = stride_y * frm->comm_frame.height;
    } else {
        frm->vpss_buf_addr[en3_dtype].phy_addr_y = phy_addr + (buf->buff_info.size / 2);  /* 1/2 used to cal */
        frm->vpss_buf_addr[en3_dtype].phy_addr_y_offset = (buf->buff_info.size / 2);  /* 1/2 used to cal */
        frm->vpss_buf_addr[en3_dtype].phy_addr_c = frm->vpss_buf_addr[en3_dtype].phy_addr_y +
            stride_y * frm->comm_frame.height;
        frm->vpss_buf_addr[en3_dtype].phy_addr_c_offset = stride_y * frm->comm_frame.height;
    }

    return;
}

hi_void fill_out_frame_addr_info(vpss_buffer *buf, hi_drv_vpss_video_frame *frm, hi_drv_3d_eye_type en3_dtype)
{
    hi_u64 phy_addr = buf->buff_info.phy_addr;
    hi_u32 stride_y = buf->stride_y;
    hi_u32 stride_c = buf->stride_c;

    frm->vpss_buf_addr[en3_dtype].stride_y = stride_y;
    frm->vpss_buf_addr[en3_dtype].stride_c = stride_c;

    if (en3_dtype == HI_DRV_3D_EYE_LEFT) {
        frm->vpss_buf_addr[en3_dtype].phy_addr_y = phy_addr;
        frm->vpss_buf_addr[en3_dtype].phy_addr_y_offset = 0;
        /* todo : add cmp adjust */
        frm->vpss_buf_addr[en3_dtype].phy_addr_c = phy_addr + stride_y * frm->comm_frame.height;
        frm->vpss_buf_addr[en3_dtype].phy_addr_c_offset = stride_y * frm->comm_frame.height;
    } else {
        frm->vpss_buf_addr[en3_dtype].phy_addr_y = phy_addr + (buf->buff_info.size / 2);  /* 1/2 used to cal */
        frm->vpss_buf_addr[en3_dtype].phy_addr_y_offset = (buf->buff_info.size / 2);  /* 1/2 used to cal */
        /* todo : add cmp adjust */
        frm->vpss_buf_addr[en3_dtype].phy_addr_c = frm->vpss_buf_addr[en3_dtype].phy_addr_y +
            stride_y * frm->comm_frame.height;
        frm->vpss_buf_addr[en3_dtype].phy_addr_c_offset = stride_y * frm->comm_frame.height;
    }

#ifdef DPT
    if (en3_dtype == HI_DRV_3D_EYE_LEFT) {
        frm->vpss_buf_addr[en3_dtype].phy_addr_y = phy_addr;
        frm->vpss_buf_addr[en3_dtype].phy_addr_c = phy_addr + stride_y * VPSS_MAX_HEIGHT_TRANS;
    } else {
        frm->vpss_buf_addr[en3_dtype].phy_addr_y = phy_addr + (buf->buff_info.size / 2);  /* 1/2 used to cal */
        frm->vpss_buf_addr[en3_dtype].phy_addr_c = frm->vpss_buf_addr[en3_dtype].phy_addr_y +
		    stride_y * VPSS_MAX_HEIGHT_TRANS;
    }
#endif
    return;
}

hi_bool vpss_inst_check_3d_task(vpss_instance *instance)
{
    if (instance->stream_info.video_3d_type != HI_DRV_3D_NONE) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

vpss_hal_node_type vpss_inst_check_node_type(vpss_instance *inst, hi_drv_3d_eye_type en3_dtype)
{
    vpss_hal_node_type node_type;

    if (en3_dtype == HI_DRV_3D_EYE_LEFT) {
        if (inst->stream_info.interlace == HI_FALSE) {
            if ((inst->stream_info.width <= VPSS_WIDTH_FHD) && (inst->stream_info.height <= VPSS_HEIGHT_FHD_1088)) {
                node_type = VPSS_HAL_NODE_2D_FRAME;
            } else if ((inst->stream_info.width <= VPSS_WIDTH_UHD) && (inst->stream_info.height <= VPSS_HEIGHT_UHD)) {
                node_type = VPSS_HAL_NODE_UHD;
            } else {
                node_type = VPSS_HAL_NODE_8K;
            }
        } else {
            node_type = VPSS_HAL_NODE_2D_4FIELD;
        }
    } else if (en3_dtype == HI_DRV_3D_EYE_RIGHT) {
        node_type = VPSS_HAL_NODE_3D_FRAME_R;
    } else {
        vpss_error("can't happen en3_dtype %d\n", en3_dtype);
        node_type = VPSS_HAL_NODE_8K;
    }

    /* add tile cmp no pq */
    if (((node_type == VPSS_HAL_NODE_2D_4FIELD) || (node_type == VPSS_HAL_NODE_2D_FRAME)) &&
        (inst->stream_info.cmp_info.data_fmt == HI_DRV_DATA_FMT_TILE) &&
        ((inst->stream_info.cmp_info.cmp_mode == HI_DRV_COMPRESS_MODE_SEG_LOSS) ||
         (inst->stream_info.cmp_info.cmp_mode == HI_DRV_COMPRESS_MODE_SEG_LOSSLESS))) {
        node_type = VPSS_HAL_NODE_UHD;
    }

    if ((node_type != VPSS_HAL_NODE_2D_4FIELD) && (inst->stream_info.force_pq_close == HI_TRUE)) {
        node_type = VPSS_HAL_NODE_8K;
    }

    return node_type;
}

hi_bool vpss_inst_check_check_rotation(vpss_instance *instance, hi_u32 port_id)
{
    vpss_port *port;

    port = &instance->port[port_id];
    if (port->enble != HI_TRUE) {
        return HI_FALSE;
    }

    if ((port->rotation == HI_DRV_VPSS_ROTATION_90) ||
        (port->rotation == HI_DRV_VPSS_ROTATION_180) ||
        (port->rotation == HI_DRV_VPSS_ROTATION_270)) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_bool vpss_inst_check_virtual_start(vpss_instance *instance)
{
    hi_u32 port_id;
    vpss_hal_info *hal_info = HI_NULL;
    vpss_hal_port_info *port_info = HI_NULL;

    hal_info = &instance->hal_info;

    for (port_id = 0; port_id < VPSS_PORT_MAX_NUM; port_id++) {
        port_info = &(hal_info->port_info[port_id]);
        if ((port_info->enable == HI_TRUE) && (port_info->config == HI_FALSE)) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

hi_void vpss_inst_release_in_buffer(vpss_instance *instance)
{
    vpss_src_data *image = HI_NULL;
    unsigned long flags;
    hi_s32 ret;

    vpss_comm_down_spin(&(instance->src_spin), &flags);
    ret = vpss_src_get_process_image(&instance->src_list, &image);
    if (ret != HI_SUCCESS) {
        vpss_comm_up_spin(&(instance->src_spin), &flags);
        vpss_error("clear_task get_process_image failed!\n");
        instance->state = INSTANCE_STATE_IDLE;
        return;
    }

    vpss_src_complete_priv_image(&instance->src_list);
    vpss_comm_up_spin(&(instance->src_spin), &flags);

    return;
}

hi_void vpss_inst_set_hal_frame_info(hi_drv_vpss_video_frame *frame, vpss_hal_frame *hal_frm, hi_drv_3d_eye_type buf_lr)
{
    if (frame == HI_NULL) {
        vpss_error("can't frame is null \n");
        return;
    }

    hal_frm->frm_type = frame->comm_frame.video_3d_type;
#ifdef HI_VPSS_DRV_USE_GOLDEN
    hal_frm->vir_addr_y = (hi_u8 *)frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y;
    hal_frm->vir_addr_c = (hi_u8 *)frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c;
#endif
    hal_frm->rect_w = frame->comm_frame.width;
    hal_frm->rect_h = frame->comm_frame.height;
    hal_frm->pixel_format = frame->comm_frame.pixel_format;
    hal_frm->cmp_info = frame->comm_frame.cmp_info;
    hal_frm->field_mode = frame->comm_frame.field_mode;

    if (frame->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_INTERLACE) {
        hal_frm->progressive = HI_FALSE;
    } else if (frame->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_PROGRESSIVE) {
        hal_frm->progressive = HI_TRUE;
    } else {
        hal_frm->progressive = HI_TRUE;
        vpss_error("sample_type:%d UNKOWN \n", frame->comm_frame.sample_type);
    }

    memcpy(&(hal_frm->hal_addr), &(frame->vpss_buf_addr[buf_lr]), sizeof(hi_drv_vpss_vid_frame_addr));
    memcpy(&(hal_frm->hal_addr_lb), &(frame->vpss_buf_addr_lb[buf_lr]), sizeof(hi_drv_vpss_vid_frame_addr));

    hal_frm->bit_width = frame->comm_frame.bit_width;
    hal_frm->secure = frame->comm_frame.secure;
#ifdef DPT
    hal_frm->hw_index = frame->hardware_buffer.buffer_index;
#endif
    /* vc1 setting */
    if (frame->comm_frame.codec_type == HI_DRV_VCODEC_VC1) {
        hal_frm->vc1_en = HI_TRUE;
        hal_frm->vc1_info = frame->comm_frame.video_private.vc1_range;
    } else {
        hal_frm->vc1_en = HI_FALSE;
    }
}

hi_void get_hal_info_rd_mode(vpss_hal_rmode *pen_rd_mode, hi_drv_field_mode field_mode,
                             vpss_stream_info *stream_info)
{
    if (stream_info->interlace == HI_FALSE) {
        *pen_rd_mode = VPSS_HAL_RMODE_PROGRESSIVE;
    } else {
        if (field_mode == HI_DRV_FIELD_TOP) {
            if (stream_info->special_field) {
                *pen_rd_mode = VPSS_HAL_RMODE_PROG_TOP;
            } else {
                *pen_rd_mode = VPSS_HAL_RMODE_INTER_TOP;
            }
        } else {
            if (stream_info->special_field) {
                *pen_rd_mode = VPSS_HAL_RMODE_PROG_BOTTOM;
            } else {
                *pen_rd_mode = VPSS_HAL_RMODE_INTER_BOTTOM;
            }
        }
    }

    return;
}

hi_void vpss_inst_fill_tunl_info(vpss_instance *instance, hi_drv_vpss_video_frame *cur,
                                 vpss_hal_info *hal_info)
{
    /* todo */
#ifdef TOBEMODIFY
    hal_info->in_info.src_id = cur->hardware_buffer.src_id;
    hal_info->in_info.hw_index = cur->hardware_buffer.buffer_index;
    hal_info->out_rtunl_en = HI_TRUE;
    hal_info->out_wtunl_en = HI_TRUE;
    cur->hardware_buffer.backward_tunnel_enable = cur->hardware_buffer.forward_tunnel_enable;
    hal_info->in_rtunl_en = cur->hardware_buffer.forward_tunnel_enable;
    hal_info->in_wtunl_en = cur->hardware_buffer.backward_tunnel_enable;

    if (hal_info->in_info.src_id == HI_DRV_HARDWARE_BUFFER_ID_VI) {
        hal_info->in_wtunl_en = HI_FALSE;
    }

    if (instance->global_dbg_ctrl->in_tunl == HI_TRUE) {
        hal_info->in_wtunl_en = instance->global_dbg_ctrl->in_wtunl_enable;
    }

    if (instance->global_dbg_ctrl->out_tunl == HI_TRUE) {
        hal_info->out_wtunl_en = instance->global_dbg_ctrl->out_wtunl_enable;
    }

    if ((hal_info->in_rtunl_en == HI_TRUE) || (hal_info->in_wtunl_en == HI_TRUE) ||
        (hal_info->out_rtunl_en == HI_TRUE) || (hal_info->out_wtunl_en == HI_TRUE)) {
        hal_info->tunl_en = HI_TRUE;
    }

    if ((cur->interlace == HI_TRUE) && (instance->stream_info.field_mode != HI_DRV_FIELD_ALL)) {
        hal_info->in_info.single_filed = HI_TRUE;
    }

#endif
}

hi_void vpss_inst_fill_10bit_addr_info(hi_drv_vpss_video_frame *cur, hi_drv_3d_eye_type en3_dtype,
                                       vpss_hal_info *hal_info)
{
    hal_info->in_info.hal_addr_lb.phy_addr_y = cur->vpss_buf_addr_lb[en3_dtype].phy_addr_y;
    hal_info->in_info.hal_addr_lb.phy_addr_c = cur->vpss_buf_addr_lb[en3_dtype].phy_addr_c;
    hal_info->in_info.hal_addr_lb.phy_addr_cr = cur->vpss_buf_addr_lb[en3_dtype].phy_addr_cr;
    hal_info->in_info.hal_addr_lb.stride_y = cur->vpss_buf_addr_lb[en3_dtype].stride_y;
    hal_info->in_info.hal_addr_lb.stride_c = cur->vpss_buf_addr_lb[en3_dtype].stride_c;
}

hi_void vpss_inst_fill_alg_info(vpss_instance *instance, vpss_hal_info *hal_info)
{
    vpss_policy_alg_input_info policy_alg;

    instance->alg_info.alg_state.en3_drs = VPSS_HAL_3DRS_TYPE_V3;

    if (instance->global_dbg_ctrl->set_me_version) {
        instance->alg_info.alg_state.en3_drs = instance->global_dbg_ctrl->me_version;
    }

    if (instance->alg_info.alg_state.en3_drs == VPSS_HAL_3DRS_TYPE_V3) {
        instance->alg_info.alg_state.me_scan_num = VPSS_ME_SCAN_NUM;
    } else {
        instance->alg_info.alg_state.me_scan_num = VPSS_ME_SCAN_NUM - 1;
    }

    if (instance->global_dbg_ctrl->set_me_scan_num) {
        instance->alg_info.alg_state.me_scan_num = instance->global_dbg_ctrl->me_scan_num;
    }

    hal_info->use_pq_module = instance->use_pq_module;

    policy_alg.width = instance->stream_info.width;
    policy_alg.height = instance->stream_info.height;
    policy_alg.sample_type = (instance->stream_info.interlace == HI_TRUE) ?
                             HI_DRV_SAMPLE_TYPE_INTERLACE : HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
    policy_alg.frame_rate = instance->stream_info.in_rate;

    instance->alg_info.alg_state.alg_ctrl.nr_en = vpss_policy_support_tnr(hal_info->vpss_alg_ctrl, &policy_alg);
    instance->alg_info.alg_state.alg_ctrl.dei_en = vpss_policy_support_dei(hal_info->vpss_alg_ctrl, &policy_alg);
    instance->alg_info.alg_state.alg_ctrl.ifmd_en = vpss_policy_support_dei(hal_info->vpss_alg_ctrl, &policy_alg);
    instance->alg_info.alg_state.alg_ctrl.db_en = vpss_policy_support_db(hal_info->vpss_alg_ctrl, &policy_alg);
    instance->alg_info.alg_state.alg_ctrl.dm_en = vpss_policy_support_dm430(hal_info->vpss_alg_ctrl, &policy_alg);
    hal_info->alg_ctrl = instance->alg_info.alg_state.alg_ctrl;

    instance->vpss_alg_ctrl.dm430_en = vpss_policy_support_dm430(hal_info->vpss_alg_ctrl, &policy_alg);
    hal_info->vpss_alg_ctrl = instance->vpss_alg_ctrl;

    hal_info->in_frame_type = VPSS_HAL_FRAME_TYPE_NORMAL;
    hal_info->en3_drs = instance->alg_info.alg_state.en3_drs;
    hal_info->me_scan_num = instance->alg_info.alg_state.me_scan_num;
}

hi_void vpss_inst_fill_rect_info(vpss_instance *instance, hi_drv_3d_eye_type en3_dtype,
                                 hi_drv_vpss_video_frame *cur, vpss_hal_info *hal_info)
{
    hal_info->in_info.rect_x = instance->src_crop_rect.rect_x;
    hal_info->in_info.rect_y = instance->src_crop_rect.rect_y;
    hal_info->in_info.rect_w = instance->src_crop_rect.rect_w;
    hal_info->in_info.rect_h = instance->src_crop_rect.rect_h;
    hal_info->in_info.top_first = instance->stream_info.real_topfirst;

    if (cur->comm_frame.video_3d_type == HI_DRV_3D_SBS_HALF) {
        if (en3_dtype == HI_DRV_3D_EYE_RIGHT) {
            hal_info->in_info.rect_x = instance->src_crop_rect.rect_w;
        }
    }

    if (cur->comm_frame.video_3d_type == HI_DRV_3D_TAB) {
        if (en3_dtype == HI_DRV_3D_EYE_RIGHT) {
            hal_info->in_info.rect_y = instance->src_crop_rect.rect_h;
        }
    }
}

hi_void vpss_inst_fill_frame_type(vpss_instance *instance, hi_drv_3d_eye_type en3_dtype, vpss_hal_info *hal_info)
{
    hal_info->in_frame_type = VPSS_HAL_FRAME_TYPE_NORMAL;

    if (instance->wbc_list[en3_dtype].complete_count == VPSS_OUTPUT_FRAME_FIRST) {
        hal_info->in_frame_type = VPSS_HAL_FRAME_TYPE_FIRST;
    }
}

hi_void vpss_inst_fill_src_hdr_type(hi_drv_vpss_video_frame *cur, vpss_hal_info *hal_info)
{
    hal_info->in_info.src_hdr_type = cur->comm_frame.hdr_type;
    hal_info->in_info.src_color_desp = cur->comm_frame.color_space;

    return;
}

hi_bool vpss_inst_fence_wait(vpss_instance *instance, hi_drv_vpss_video_frame *frame_info)
{
    hi_s32 ret;
    hi_s32 source_fence = frame_info->comm_frame.source_fence;
    hi_drv_fence_info *fence_info = HI_NULL;
    hi_bool fence_ret = HI_FALSE;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;
#if (defined HI_VPSS_DRV_USE_GOLDEN)
    return HI_TRUE;
#endif

    meta_info = (hi_drv_win_vpss_meta_info *)frame_info->comm_frame.video_private.win_private.data;
    /* return true : no fence or fence ok */
    if (frame_info->comm_frame.source_fence < 0) {
        return HI_TRUE;
    }

    /* return true : fence is destroyed:when interlace, 3d, 1 in 2 out */
    if (meta_info->fence_is_destroyed == HI_TRUE) {
        frame_info->comm_frame.source_fence = -1;
        return HI_TRUE;
    }

    instance->perfor.fence_dfx.time_warn_start = vpss_comm_get_sched_time();
    ret = osal_fence_wait(source_fence, VPSS_FENCE_TIMEOUT_VALUE);
    instance->perfor.fence_dfx.time_warn_end = vpss_comm_get_sched_time();
    if (instance->perfor.fence_dfx.time_warn_end - instance->perfor.fence_dfx.time_warn_start > VPSS_FENCE_TIME_MS) {
        instance->perfor.fence_dfx.time_warn_cnt++;
    }

    if (ret != OSAL_FENCE_ERR_SUCCESS) {
        vpss_warn("wait fence failed, index:%d, id:%d.\n", frame_info->comm_frame.frame_index, source_fence);
        instance->perfor.fence_dfx.time_err_cnt++;
        instance->perfor.fence_dfx.err_cnt++;
        goto __FENCE_DESTROY;
    }

    ret = osal_fence_acquire_private(HI_ID_VPSS, source_fence, (void *)&fence_info);
    if (ret != OSAL_FENCE_ERR_SUCCESS) {
        vpss_error("get private failed,id:%d.\n", source_fence);
        instance->perfor.fence_dfx.err_cnt++;
        goto __FENCE_DESTROY;
    }

    if (fence_info->need_display != HI_TRUE) { /* 不需要处理的废弃帧，需要归还给解码器 */
        vpss_warn("decoded failed, id:%d.\n", source_fence);
        fence_ret = HI_FALSE;
        goto __FENCE_RLS;
    }

    fence_ret = HI_TRUE;

__FENCE_RLS:
    ret = osal_fence_release_private(HI_ID_VPSS, source_fence, (void *)fence_info);
    if (ret != OSAL_FENCE_ERR_SUCCESS) {
        instance->perfor.fence_dfx.err_cnt++;
        fence_ret = HI_FALSE;
        vpss_error("rls private err.\n", source_fence);
    }
__FENCE_DESTROY:
    osal_fence_destroy(HI_ID_VPSS, frame_info->comm_frame.source_fence);
    frame_info->comm_frame.source_fence = -1;
    meta_info->fence_is_destroyed = HI_TRUE;

    return fence_ret;
}

hi_s32 vpss_inst_fill_input_frame_info(vpss_instance *instance, hi_drv_3d_eye_type en3_dtype)
{
    unsigned long flags;
    vpss_hal_info *hal_info = HI_NULL;
    hi_drv_vpss_video_frame *cur = HI_NULL;
    hi_drv_vpss_video_frame *cur_next = HI_NULL;
    hi_bool fence_valid = HI_FALSE;

    hal_info = &instance->hal_info;
    vpss_inst_fill_alg_info(instance, hal_info);
    hal_info->cf_pattern = instance->global_dbg_ctrl->cf_pattern;
    hal_info->out_pattern = instance->global_dbg_ctrl->out_pattern;

    vpss_comm_down_spin(&(instance->src_spin), &flags);
    if (vpss_src_get_process_image(&instance->src_list, &cur) != HI_SUCCESS) {
        vpss_comm_up_spin(&(instance->src_spin), &flags);
        vpss_error("can't get src frame happen,error!\n");
        return HI_FAILURE;
    }

    if ((instance->stream_info.interlace == HI_TRUE) &&
        (instance->pause == HI_FALSE) &&
        (instance->stream_info.video_3d_type == HI_DRV_3D_NONE) &&
        (instance->stream_info.field_mode != HI_DRV_FIELD_ALL)) {
        /* the stream order changed,we drop one field */
        if (cur->comm_frame.field_mode == instance->stream_info.pre_fieldmode) {
            if (vpss_src_move_next(&instance->src_list, 1) != HI_SUCCESS) {
                vpss_dbg("has only one frame\n");
            }

            if (vpss_src_get_process_image(&instance->src_list, &cur_next) == HI_SUCCESS) {
                cur = cur_next;
            } else {
                vpss_error("current field order is same with the last one and no more frame to process\n");
            }
        }
    }
    vpss_comm_up_spin(&(instance->src_spin), &flags);

    /* add fence here */
    instance->perfor.fence_dfx.time_start = vpss_comm_get_sched_time();
    fence_valid = vpss_inst_fence_wait(instance, cur);
    instance->perfor.fence_dfx.time_end = vpss_comm_get_sched_time();
    /* fence flag changed in frame, need to copy cur_frame to cur_process_frame , to use when clear */
    memcpy(&(instance->cur_process_frame_info), cur, sizeof(hi_drv_vpss_video_frame));
    if (fence_valid == HI_FALSE) {
        vpss_warn("can't get src frame fence, error!\n");
        return HI_FAILURE;
    }

    instance->stream_info.pre_fieldmode = cur->comm_frame.field_mode;
    vpss_inst_set_hal_frame_info(cur, &hal_info->in_info, en3_dtype);

    if (instance->quick_out == HI_TRUE) {
        hal_info->in_info.tunnel_addr = cur->comm_frame.tunnel_phy_addr;
    } else {
        hal_info->in_info.tunnel_addr = 0;
    }

    get_hal_info_rd_mode(&hal_info->in_info.rd_mode, cur->comm_frame.field_mode, &instance->stream_info);
    vpss_inst_fill_rect_info(instance, en3_dtype, cur, hal_info);
    hal_info->need_src_flip = cur->comm_frame.flip_v;
#ifndef HI_VPSS_DRV_USE_GOLDEN
    vpss_inst_fill_tunl_info(instance, cur, hal_info);
#endif
    vpss_inst_fill_frame_type(instance, en3_dtype, hal_info);
    vpss_inst_fill_src_hdr_type(cur, hal_info);

    vpss_inst_fill_10bit_addr_info(cur, en3_dtype, hal_info);
    hal_info->complete_cnt = instance->wbc_list[en3_dtype].complete_count;

    return vpss_inst_update_vpss_pq_reg(instance);
}

hi_void vpss_inst_set_3d_detect_out_frame_info(hi_drv_3d_type video_3d_type, vpss_buffer *buf,
                                               hi_drv_vpss_video_frame *frm, hi_drv_3d_eye_type en3_d_eye)
{
    hi_u32 stride_y;
    hi_drv_3d_eye_type buf_lr = en3_d_eye;

    if (en3_d_eye >= HI_DRV_3D_EYE_MAX) {
        vpss_error("can't here ,3dtype %d, eye %d", video_3d_type, en3_d_eye);
        return;
    }

    stride_y = frm->vpss_buf_addr[buf_lr].stride_y;

    if (video_3d_type == HI_DRV_3D_SBS_HALF) {
        frm->comm_frame.width >>= 1;

        if ((frm->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV12) ||
            (frm->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV21) ||
            (frm->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV61_2X1) ||
            (frm->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV16_2X1)) {
            if (buf_lr == HI_DRV_3D_EYE_RIGHT) {
                frm->vpss_buf_addr[buf_lr].phy_addr_y =
                    frm->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].phy_addr_y + frm->comm_frame.width;
                frm->vpss_buf_addr[buf_lr].phy_addr_c =
                    frm->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].phy_addr_c + frm->comm_frame.width;
            } else {
                frm->vpss_buf_addr[buf_lr].phy_addr_y += buf_lr * frm->comm_frame.width;
                frm->vpss_buf_addr[buf_lr].phy_addr_c += buf_lr * frm->comm_frame.width;
            }
        } else {
            vpss_error("invalid out pixel_format %d,can't get addr\n", frm->comm_frame.pixel_format);
            return;
        }
    } else if (video_3d_type == HI_DRV_3D_TAB) {
        frm->comm_frame.height >>= 1;

        if ((frm->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV12) ||
            (frm->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV21)) {
            if (buf_lr == HI_DRV_3D_EYE_RIGHT) {
                frm->vpss_buf_addr[HI_DRV_3D_EYE_RIGHT].phy_addr_y =
                    frm->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].phy_addr_y + (frm->comm_frame.height * stride_y);
                frm->vpss_buf_addr[HI_DRV_3D_EYE_RIGHT].phy_addr_c =
                    frm->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].phy_addr_c + (frm->comm_frame.height * stride_y);
            }
        } else if ((frm->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV61_2X1) ||
                   (frm->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV16_2X1)) {
            frm->vpss_buf_addr[buf_lr].phy_addr_y += buf_lr * stride_y * frm->comm_frame.height;
            frm->vpss_buf_addr[buf_lr].phy_addr_c += buf_lr * stride_y * frm->comm_frame.height;
        } else {
            vpss_error("invalid out pixel_format %d,can't get addr\n", frm->comm_frame.pixel_format);
            return;
        }
    } else {
        vpss_dbg("invalid 3D type %d!!!!!\n", video_3d_type);
    }
}

hi_void vpss_inst_rotate_outframe_info(vpss_instance *inst, hi_u32 port_id, vpss_buffer *buf,
                                       hi_drv_vpss_video_frame *frm, hi_drv_3d_eye_type buf_lr)
{
    hi_s32 ret;
    hi_drv_vpss_video_frame *cur = HI_NULL;
    vpss_port *port = HI_NULL;
    unsigned long flags;

    vpss_comm_down_spin(&(inst->src_spin), &flags);
    ret = vpss_src_get_process_image(&inst->src_list, &cur);
    vpss_comm_up_spin(&(inst->src_spin), &flags);

    if (ret != HI_SUCCESS) {
        vpss_error("vpss_src_get_process_image failed!\n");
        return;
    }

    memcpy(frm, cur, sizeof(hi_drv_vpss_video_frame));

    port = &inst->port[port_id];
    frm->comm_frame.width = port->revise_out_rect.rect_w;
    frm->comm_frame.height = port->revise_out_rect.rect_h;
    frm->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
    frm->comm_frame.field_mode = HI_DRV_FIELD_ALL;
    frm->comm_frame.bit_width = port->bit_width;
    frm->comm_frame.pixel_format = port->format;
    /* todo : if out cmp & rota, pay attention */
    frm->comm_frame.cmp_info = port->cmp_info;
    frm->comm_frame.video_3d_type = HI_DRV_3D_NONE;
    frm->comm_frame.flip_h = 0;
    frm->comm_frame.flip_v = 0;
    /* todo : addr hdr process here */
    frm->comm_frame.hdr_type = port->out_hdr_type;
    frm->comm_frame.color_space = port->out_color_desp;
    /* todo */
#ifdef DPT
    frm->hardware_buffer.buffer_index = inst->src_list.complete_src_count % (0x20);
    frm->hardware_buffer.buffer_addr = buf->buff_info.phy_addr;
    frm->hardware_buffer.src_id = HI_DRV_HARDWARE_BUFFER_ID_VPSS_OUT;
    frm->hardware_buffer.backward_tunnel_enable = HI_TRUE;
    frm->hardware_buffer.forward_tunnel_enable = HI_TRUE;

    if (inst->hal_info.out_wtunl_en != HI_TRUE) {
        frm->hardware_buffer.forward_tunnel_enable = HI_FALSE;
    }
#endif

#ifndef DPT
#ifndef HI_VPSS_DRV_USE_GOLDEN
    if (port->rotation != HI_DRV_VPSS_ROTATION_180) {
        if (port->bit_width == HI_DRV_PIXEL_BITWIDTH_8BIT) {
            buf->stride_y = vpss_align_8bit_ystride(frm->comm_frame.width);
            buf->stride_c = buf->stride_y;
        } else {
            buf->stride_y = vpss_align_10bit_comp_ystride(frm->comm_frame.width);
            buf->stride_c = buf->stride_y;
        }
    }
#endif
#else
#ifndef HI_VPSS_DRV_USE_GOLDEN
    if (port->rotation != HI_DRV_VPSS_ROTATION_180) {
        if (port->bit_width == HI_DRV_PIXEL_BITWIDTH_8BIT) {
            buf->stride_y = vpss_align_8bit_ystride(VPSS_MAX_HEIGHT_TRANS);
            buf->stride_c = buf->stride_y;
        } else {
            buf->stride_y = vpss_align_10bit_comp_ystride(VPSS_MAX_HEIGHT_TRANS);
            buf->stride_c = buf->stride_y;
        }
    }
#endif
#endif
    fill_rotate_frame_addr_info(buf, frm, buf_lr);
}

hi_void vpss_inst_fill_out_frame_hardware(vpss_instance *inst, vpss_buffer *buf, hi_drv_vpss_video_frame *frm)
{
#ifdef DPT
    frm->hardware_buffer.buffer_index = inst->perfor.out_dfx.queue_cnt % (0x20); /* 0x20: frame cnt */
    frm->hardware_buffer.buffer_addr = buf->buff_info.phy_addr;
    frm->hardware_buffer.src_id = HI_DRV_HARDWARE_BUFFER_ID_VPSS_OUT;
    frm->hardware_buffer.backward_tunnel_enable = HI_TRUE;
    frm->hardware_buffer.forward_tunnel_enable = HI_TRUE;

    if (inst->global_dbg_ctrl->out_tunl == HI_TRUE) {
        frm->hardware_buffer.forward_tunnel_enable = inst->global_dbg_ctrl->out_wtunl_enable;
    }
#endif
}

hi_void vpss_inst_fill_out_frame_info(vpss_instance *inst, vpss_buffer *buf, hi_drv_3d_eye_type en3_dtype,
                                      vpss_port *port, hi_drv_vpss_video_frame *frm)
{
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;

    frm->comm_frame.pixel_format = port->format;
    frm->comm_frame.cmp_info = port->cmp_info;
    frm->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
    frm->comm_frame.field_mode = HI_DRV_FIELD_ALL;
    frm->comm_frame.bit_width = port->bit_width;

    frm->comm_frame.hdr_type = port->out_hdr_type;
    frm->comm_frame.color_space = port->out_color_desp;

    meta_info = (hi_drv_win_vpss_meta_info *)frm->comm_frame.video_private.vpss_private.data;
    meta_info->src_interlace = inst->stream_info.interlace;

    vpss_inst_fill_out_frame_hardware(inst, buf, frm);

    frm->comm_frame.flip_h = 0;
    frm->comm_frame.flip_v = 0;

    if ((port->rotation == HI_DRV_VPSS_ROTATION_90) || (port->rotation == HI_DRV_VPSS_ROTATION_270)) {
        fill_rotate_frame_addr_info(buf, frm, en3_dtype);
    } else {
        fill_out_frame_addr_info(buf, frm, en3_dtype);
    }

#ifdef HI_VPSS_DRV_USE_GOLDEN
    fill_rotate_frame_addr_info(buf, frm, en3_dtype);
#endif
}

hi_void vpss_inst_adjust_out_frame_info(vpss_instance *inst, hi_drv_vpss_video_frame *frm,
                                        vpss_pts_node *out_pts_node)
{
    list *ref_node = HI_NULL;
    hi_u32 i = 0;

    if (inst->hal_info.in_info.progressive != HI_TRUE) {
        if (inst->hal_info.complete_cnt == 0) {
            for (i = 0; i < VPSS_PTS_NODE_NUM; i++) {
                inst->pts_node[i].pts_data = frm->comm_frame.pts;
                inst->pts_node[i].field_mode = frm->comm_frame.field_mode;
                inst->pts_node[i].frame_index = frm->comm_frame.frame_index;
                inst->pts_node[i].repeat_first_field = frm->comm_frame.repeat_first_field;
                inst->pts_node[i].top_field_first = frm->comm_frame.top_field_first;
            }
        } else {
            ref_node = inst->first_ref->next;
            inst->first_ref = inst->first_ref->next;
            out_pts_node = (vpss_pts_node *)osal_list_entry(ref_node, vpss_pts_node, node);
            out_pts_node->pts_data = frm->comm_frame.pts;
            out_pts_node->field_mode = frm->comm_frame.field_mode;
            out_pts_node->frame_index = frm->comm_frame.frame_index;
            out_pts_node->repeat_first_field = frm->comm_frame.repeat_first_field;
            out_pts_node->top_field_first = frm->comm_frame.top_field_first;
        }
    }
}

hi_void vpss_inst_fill_out_frame_3d_type(vpss_port *port, hi_drv_vpss_video_frame *cur,
                                         vpss_buffer *buf, hi_drv_3d_eye_type en3_dtype, hi_drv_vpss_video_frame *frm)
{
    if ((port->enable_3d == HI_TRUE) && (cur->comm_frame.video_3d_type != HI_DRV_3D_NONE)) {
        frm->comm_frame.video_3d_type = HI_DRV_3D_FS;
    } else {
        frm->comm_frame.video_3d_type = HI_DRV_3D_NONE;
    }

    if (port->port_type == HI_DRV_VPSS_PORT_TYPE_3D_DETECT) {
        vpss_inst_set_3d_detect_out_frame_info(cur->comm_frame.video_3d_type, buf, frm, en3_dtype);
    }
}

hi_s32 vpss_inst_alloc_rotate_buff(vpss_buffer *buf, hi_drv_vpss_video_frame *frm, hi_u32 buf_size,
                                   hi_u32 stride_y, drv_vpss_mem_attr attr)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_ssm_buf_attach_info attach_info;
    hi_u64 sec_info_addr;

    if (((buf->buff_info.size != buf_size) || (buf->stride_y != stride_y)) &&
        (buf->buff_info.size != 0)) {
        vpss_comm_mem_free(&(buf->buff_info));
    }

    if ((buf->buff_info.size != buf_size) || (buf->stride_y != stride_y)) {
        attr.name = "vpss_rot_buf";
        attr.size = buf_size;
        attr.mode = frm->comm_frame.secure ? OSAL_SECSMMU_TYPE : OSAL_NSSMMU_TYPE; /* just smmu support */
        attr.is_map_viraddr = HI_FALSE;
        attr.is_cache = HI_FALSE;
        ret = vpss_comm_mem_alloc(&attr, &(buf->buff_info));
    }

    if (ret != HI_SUCCESS) {
        vpss_error("alloc ro_buf failed\n");
        return HI_FAILURE;
    }

    attach_info.session_handle = 0x0FFFFFFF; /* 0x0FFFFFFF: unvalid */
    attach_info.dma_buf_addr = buf->buff_info.dma_handle;
    attach_info.buf_id = HI_SSM_BUFFER_ID_INTERNAL_BUF_VPSS;
    attach_info.module_handle = 0x5CFFFFFF; /* 0x5CFFFFFF: vpss handle, 0x5C valid */
    vpss_comm_attach_buf(frm->comm_frame.secure, attach_info, &sec_info_addr);

    return HI_SUCCESS;
}

hi_s32 vpss_inst_set_out_frame_info(vpss_instance *inst, hi_u32 port_id, vpss_buffer *buf,
                                    vpss_out_node *frm_node, hi_drv_3d_eye_type en3_dtype)
{
    hi_s32 ret;
    unsigned long flags;
    hi_drv_vpss_video_frame *cur = HI_NULL;
    vpss_port *port = &inst->port[port_id];
    drv_vpss_mem_attr attr = { 0 };
    vpss_wbc *wbc = &inst->wbc_list[en3_dtype];
    vpss_wbc_cfg wbc_cfg = { 0 };
    vpss_pts_node *out_pts_node = HI_NULL;
    vpss_buffer_attr buffer_attr = { 0 };
    hi_bool rota_alg_en = HI_FALSE;

    vpss_comm_down_spin(&(inst->src_spin), &flags);
    ret = vpss_src_get_process_image(&inst->src_list, &cur);
    vpss_comm_up_spin(&(inst->src_spin), &flags);
    if (ret != HI_SUCCESS) {
        vpss_error("can't happen,error!\n");
        return HI_FAILURE;
    }

    if (en3_dtype == HI_DRV_3D_EYE_LEFT) {
        /* step 1 : out metadata copy */
        cur->comm_frame.video_private.vpss_private.data = (hi_u64)frm_node->metedata.vir_addr;
        cur->comm_frame.video_private.vpss_private.size = cur->comm_frame.video_private.win_private.size;

        memcpy((hi_drv_win_vpss_meta_info *)cur->comm_frame.video_private.vpss_private.data,
               (hi_drv_win_vpss_meta_info *)cur->comm_frame.video_private.win_private.data,
               cur->comm_frame.video_private.win_private.size);

        memcpy(&frm_node->out_priv_frame, cur, sizeof(hi_drv_vpss_video_frame));
        frm_node->out_priv_frame.vpss_buf_addr[0].vir_addr_y = (hi_u64)frm_node->buffer.buff_info.vir_addr;
    }

    if (vpss_wbc_get_cfg(wbc, &wbc_cfg) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if ((port->rotation == HI_DRV_VPSS_ROTATION_90) || (port->rotation == HI_DRV_VPSS_ROTATION_270)) {
        frm_node->out_priv_frame.comm_frame.width = port->revise_out_rect.rect_h;
        frm_node->out_priv_frame.comm_frame.height = port->revise_out_rect.rect_w;
        buffer_attr.width = frm_node->out_priv_frame.comm_frame.width;
        buffer_attr.height = frm_node->out_priv_frame.comm_frame.height;

        rota_alg_en = vpss_policy_support_rota_alg(&inst->dbg_ctrl,
            frm_node->out_priv_frame.comm_frame.width, frm_node->out_priv_frame.comm_frame.height);
        if (rota_alg_en == HI_TRUE) {
            vpss_comm_cal_buf_size(&buffer_attr, port->format, port->cmp_info, port->bit_width,
                                   HI_TRUE, VPSS_SUPPORT_CMP_RATE);

            ret = vpss_inst_alloc_rotate_buff(buf, &frm_node->out_priv_frame, buffer_attr.buf_size,
                                              buffer_attr.stride_y, attr);

            buf->stride_y = buffer_attr.stride_y;
            buf->stride_c = buffer_attr.stride_c;
        }
    } else {
        frm_node->out_priv_frame.comm_frame.width = port->revise_out_rect.rect_w;
        frm_node->out_priv_frame.comm_frame.height = port->revise_out_rect.rect_h;
    }

    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    vpss_inst_fill_out_frame_info(inst, buf, en3_dtype, port, &frm_node->out_priv_frame);
    vpss_inst_fill_out_frame_3d_type(port, cur, buf, en3_dtype, &frm_node->out_priv_frame);
    vpss_inst_adjust_out_frame_info(inst, &frm_node->out_priv_frame, out_pts_node);

    return HI_SUCCESS;
}

hi_void vpss_inst_set_hal_rotate(hi_bool need_src_flip, vpss_port *port, vpss_hal_port_info *hal_port_info)
{
    hal_port_info->rotation = port->rotation;

    if (hal_port_info->rotation == HI_DRV_VPSS_ROTATION_180) {
        hal_port_info->need_flip = HI_TRUE;
        hal_port_info->need_mirror = HI_TRUE;
    }

    if (port->hori_flip == HI_TRUE) {
        hal_port_info->need_mirror = !hal_port_info->need_mirror;
    }

    if (port->vert_flip == HI_TRUE) {
        hal_port_info->need_flip = !hal_port_info->need_flip;
    }

    if (need_src_flip == HI_TRUE) {
        hal_port_info->need_flip = !hal_port_info->need_flip;
    }

#ifdef VPSS_LBX_DBG
    vpss_error("rota:%d; mirror:%d; flip:%d; src_flip:%d \n",
               hal_port_info->rotation, hal_port_info->need_mirror, hal_port_info->need_flip, need_src_flip);
#endif

    return;
}

static hi_bool vpss_inst_update_hdr_flag(vpss_hal_info *hal_info, vpss_hal_frame *out_info)
{
    hi_bool support_hdr = vpss_policy_support_hdr();
    hi_bool need_csc = HI_FALSE;

    if (support_hdr != HI_TRUE) {
        return HI_FALSE;
    }

    if (hal_info->in_info.src_hdr_type != out_info->out_hdr_type) {
        return HI_TRUE;
    }

    need_csc = vpss_policy_check_need_csc(&hal_info->in_info.src_color_desp, &out_info->out_color_desp);
    if (need_csc == HI_TRUE) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_bool vpss_inst_check_need_rotation_buff(vpss_instance *instance, hi_u32 port_id)
{
    vpss_port *port;
    port = &instance->port[port_id];

    if (port->enble != HI_TRUE) {
        return HI_FALSE;
    }

    if ((port->rotation == HI_DRV_VPSS_ROTATION_90) ||
        (port->rotation == HI_DRV_VPSS_ROTATION_270)) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_s32 vpss_inst_fill_output_frame_info(vpss_instance *instance, hi_drv_3d_eye_type en3_dtype)
{
    hi_u32 port_id;
    vpss_out_node *frm_node = HI_NULL;
    vpss_hal_info *hal_info = HI_NULL;
    vpss_stream_info *stream_info = HI_NULL;
    vpss_hal_port_info *port_info = HI_NULL;
    hi_s32 ret;

    hal_info = &instance->hal_info;
    stream_info = &instance->stream_info;

    for (port_id = 0; port_id < VPSS_PORT_MAX_NUM; port_id++) {
        port_info = &hal_info->port_info[port_id];
        frm_node = instance->out_frm_node[port_id];

        if (frm_node == HI_NULL) {
            port_info->enable = HI_FALSE;
            continue;
        }

        port_info->enable = HI_TRUE;
        port_info->config = HI_FALSE;
        port_info->port_type = instance->port[port_id].port_type;

        // rotation buffer
        if (vpss_inst_check_need_rotation_buff(instance, port_id) == HI_TRUE) {
            ret = vpss_inst_set_out_frame_info(instance, port_id, &instance->ro_buf[port_id], frm_node, en3_dtype);
        } else {
            ret = vpss_inst_set_out_frame_info(instance, port_id, &frm_node->buffer, frm_node, en3_dtype);
        }

        if (ret != HI_SUCCESS) {
            vpss_error("vpss_inst_set_out_frame_info failed!\n");
            return HI_FAILURE;
        }

        vpss_inst_set_hal_frame_info(&frm_node->out_priv_frame, &port_info->out_info, en3_dtype);
        port_info->in_crop_rect = instance->port[port_id].revise_crop_rect;
        port_info->video_rect = instance->port[port_id].revise_video_rect;
        port_info->out_info.out_hdr_type = instance->port[port_id].out_hdr_type;
        port_info->out_info.out_color_desp = instance->port[port_id].out_color_desp;
        port_info->need_hdr = vpss_inst_update_hdr_flag(hal_info, &port_info->out_info);

        if (instance->port[port_id].port_type == HI_DRV_VPSS_PORT_TYPE_3D_DETECT) {
            if (stream_info->video_3d_type == HI_DRV_3D_SBS_HALF) {
                port_info->video_rect.rect_w /= 2;  // 1/2 used to cal
            } else if (stream_info->video_3d_type == HI_DRV_3D_TAB) {
                port_info->video_rect.rect_h /= 2;  // 1/2 used to cal
            } else {
            }
        }

        vpss_inst_set_hal_rotate(hal_info->need_src_flip, &instance->port[port_id], &hal_info->port_info[port_id]);
    }

    return HI_SUCCESS;
}

hi_s32 vpss_inst_fill_hal_info(vpss_instance *instance, hi_drv_3d_eye_type en3_dtype)
{
    hi_s32 ret;
    vpss_hal_info *hal_info = HI_NULL;

    /* init hal info cfg */
    hal_info = &instance->hal_info;
    /* reset hal info */
    vpss_hal_init_hal_info(hal_info);
    hal_info->inst_id = instance->id;
    hal_info->node_type = vpss_inst_check_node_type(instance, en3_dtype);
    /* input frame info */
    ret = vpss_inst_fill_input_frame_info(instance, en3_dtype);
    if (ret != HI_SUCCESS) {
        vpss_warn("fill input frame info error,en_lr = %d!\n", en3_dtype);
        return HI_FAILURE;
    }

    if (hal_info->node_type != VPSS_HAL_NODE_8K) {
        ret = vpss_wbc_fill_wbc_frame_info(instance, en3_dtype);
        if (ret != HI_SUCCESS) {
            vpss_error("error,en_lr = %d!\n", en3_dtype);
            return HI_FAILURE;
        }

        ret = vpss_stt_fill_stt_info(instance, en3_dtype);
        if (ret != HI_SUCCESS) {
            vpss_error("error,en_lr = %d!\n", en3_dtype);
            return HI_FAILURE;
        }
    }

    ret = vpss_inst_fill_output_frame_info(instance, en3_dtype);
    if (ret != HI_SUCCESS) {
        vpss_error("error,en_lr = %d!\n", en3_dtype);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
hi_void vpss_inst_release_frame(vpss_instance *instance)
{
    vpss_src *src_list = HI_NULL;
    unsigned long flags;

    if (instance->pause == HI_TRUE) {
        vpss_info("vpss instance%d state:pause", instance->id);
        return;
    }

    src_list = &instance->src_list;
    vpss_comm_down_spin(&(instance->src_spin), &flags);
    vpss_src_complete_priv_image(src_list);
    vpss_comm_up_spin(&(instance->src_spin), &flags);

    return;
}
hi_void vpss_inst_complete_frame(vpss_instance *instance)
{
    vpss_wbc *wbc_list = HI_NULL;
    vpss_stt *stt_list = HI_NULL;
    hi_drv_3d_type frame_type;
    wbc_list = &instance->wbc_list[0];
    stt_list = &instance->stt_list[0];
    frame_type = instance->hal_info.in_info.frm_type;
    vpss_wbc_complete(wbc_list);
    vpss_stt_complete(stt_list);

    if (frame_type != HI_DRV_3D_NONE) {
        vpss_wbc_complete(wbc_list + 1);
        vpss_stt_complete(stt_list + 1);
    }
}
hi_void vpss_inst_3d_detect_outframe_info(hi_drv_3d_type src_frm_type, hi_drv_vpss_video_frame *frm)
{
    if (src_frm_type == HI_DRV_3D_SBS_HALF) {
        frm->comm_frame.width <<= 1;
    } else if (src_frm_type == HI_DRV_3D_TAB) {
        frm->comm_frame.height <<= 1;
    }

    return;
}

hi_s32 vpss_inst_check_flag_info(vpss_instance *instance, hi_drv_video_frame *image)
{
    if ((image->secure != HI_TRUE) && (image->secure != HI_FALSE)) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed secure %d\n", image->secure);
        return HI_FAILURE;
    }

#ifdef TOBEMODIFY /* todo to change top_field_first to u8 or u32 */
    if ((image->top_field_first != HI_TRUE) && (image->top_field_first != HI_FALSE)) {
    }
#endif

    if (image->top_field_first > 2) { /* 2 : unknow */
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed top_field_first %d\n", image->top_field_first);
        return HI_FAILURE;
    }

    if (image->sample_type >= HI_DRV_SAMPLE_TYPE_MAX) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed interlace %d\n", image->sample_type);
        return HI_FAILURE;
    }

    if ((image->single_i_frame != HI_TRUE) && (image->single_i_frame != HI_FALSE)) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed single_i_frame %d\n", image->single_i_frame);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vpss_inst_check_image(vpss_instance *instance, hi_drv_video_frame *image)
{
    hi_bool support_in_fmt = HI_FALSE;
    hi_bool support_reso = HI_FALSE;

    support_in_fmt = vpss_policy_support_in_fmt(image->pixel_format);
    if (support_in_fmt != HI_TRUE) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed fmt %d\n", image->pixel_format);
        return HI_FAILURE;
    }

    if (image->video_3d_type >= HI_DRV_3D_MAX) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed frm_type %d\n", image->video_3d_type);
        return HI_FAILURE;
    }

    if (image->video_private.source >= HI_DRV_SOURCE_MAX) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed source %d\n", image->video_private.source);
        return HI_FAILURE;
    }

    if (image->field_mode >= HI_DRV_FIELD_MAX) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed field_mode %d\n", image->field_mode);
        return HI_FAILURE;
    }

    if (image->buf_valid_mode >= HI_DRV_FIELD_MAX) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed buf_valid_mode %d\n", image->buf_valid_mode);
        return HI_FAILURE;
    }

    support_reso = vpss_policy_support_height_and_width(image->width, image->height);
    if (support_reso != HI_TRUE) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed w %d, h %d\n", image->width, image->height);
        return HI_FAILURE;
    }

    if ((image->frame_rate == 0) || (image->frame_rate >= VPSS_SUPPORT_RATE_MAX)) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed frame_rate %d\n", image->frame_rate);
        return HI_FAILURE;
    }

    if (image->bit_width > HI_DRV_PIXEL_BITWIDTH_12BIT) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed bit_width %d\n", image->bit_width);
        return HI_FAILURE;
    }

    if (image->hdr_type >= HI_DRV_HDR_TYPE_MAX) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed hdr_type %d\n", image->hdr_type);
        return HI_FAILURE;
    }

    if (image->color_space.color_primary >= HI_DRV_COLOR_PRIMARY_COLOR_MAX) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed color_primary %d\n", image->color_space.color_primary);
        return HI_FAILURE;
    }

    if (image->color_space.color_space >= HI_DRV_COLOR_CS_MAX) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed color_space %d\n", image->color_space.color_space);
        return HI_FAILURE;
    }

    if (image->color_space.quantify_range >= HI_DRV_COLOR_RANGE_MAX) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed quantify_range %d\n", image->color_space.quantify_range);
        return HI_FAILURE;
    }

    if (image->color_space.transfer_type >= HI_DRV_COLOR_TRANSFER_TYPE_MAX) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed transfer_type %d\n", image->color_space.transfer_type);
        return HI_FAILURE;
    }

    if (image->color_space.matrix_coef >= HI_DRV_COLOR_MATRIX_COEFFS_MAX) {
        instance->perfor.cnt_dfx.source_frame_drop_cnt++;
        vpss_error("in image can't be processed matrix_coef %d\n", image->color_space.matrix_coef);
        return HI_FAILURE;
    }

    return vpss_inst_check_flag_info(instance, image);
}

hi_s32 vpss_inst_check_port_flag(vpss_instance *instance, hi_drv_vpss_port_cfg *vpss_port_cfg)
{
    if ((vpss_port_cfg->user_crop_en != HI_TRUE) && (vpss_port_cfg->user_crop_en != HI_FALSE)) {
        vpss_error("user_crop_en %d is invalid.\n", vpss_port_cfg->user_crop_en);
        return HI_FAILURE;
    }

    if ((vpss_port_cfg->vertical_flip != HI_TRUE) && (vpss_port_cfg->vertical_flip != HI_FALSE)) {
        vpss_error("vertical_flip %d is invalid.\n", vpss_port_cfg->vertical_flip);
        return HI_FAILURE;
    }

    if ((vpss_port_cfg->horizontal_flip != HI_TRUE) && (vpss_port_cfg->horizontal_flip != HI_FALSE)) {
        vpss_error("horizontal_flip %d is invalid.\n", vpss_port_cfg->horizontal_flip);
        return HI_FAILURE;
    }

    if (vpss_port_cfg->port_type >= HI_DRV_VPSS_PORT_TYPE_MAX) {
        vpss_error("port_type %d is invalid.\n", vpss_port_cfg->port_type);
        return HI_FAILURE;
    }

    if ((vpss_port_cfg->hdr_follow_src != HI_TRUE) && (vpss_port_cfg->hdr_follow_src != HI_FALSE)) {
        vpss_error("hdr_follow_src %d is invalid.\n", vpss_port_cfg->hdr_follow_src);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
hi_s32 vpss_inst_check_port_cfg(vpss_instance *instance, hi_drv_vpss_port_cfg *vpss_port_cfg)
{
    hi_s32 ret;
    hi_bool support_out_fmt = HI_FALSE;
    hi_bool support_cmp_fmt = HI_FALSE;
    hi_bool support_cmp_rota = HI_FALSE;
    ret = vpss_inst_check_port_flag(instance, vpss_port_cfg);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    vpss_check_enum(vpss_port_cfg->rotation, HI_DRV_VPSS_ROTATION_MAX);
    vpss_check_enum(vpss_port_cfg->bit_width, HI_DRV_PIXEL_BITWIDTH_MAX);
    vpss_check_enum(vpss_port_cfg->bypass_mode, HI_DRV_VPSS_WORKMODE_MAX);
    vpss_check_enum(vpss_port_cfg->buf_cfg.buf_type, HI_DRV_VPSS_BUF_TYPE_MAX);

    support_out_fmt = vpss_policy_support_out_fmt(vpss_port_cfg->format);
    if (support_out_fmt != HI_TRUE) {
        vpss_error("invalid port out format %d\n", vpss_port_cfg->format);
        return HI_FAILURE;
    }

    support_cmp_fmt = vpss_policy_check_out_frame_cmp_fmt(vpss_port_cfg->format, vpss_port_cfg->cmp_info);
    if (support_cmp_fmt != HI_TRUE) {
        vpss_error("invalid port out cmp format %d\n", vpss_port_cfg->format);
        return HI_FAILURE;
    }

    support_cmp_rota = vpss_policy_check_out_frame_cmp_rote(vpss_port_cfg->rotation, vpss_port_cfg->cmp_info);
    if (support_cmp_rota != HI_TRUE) {
        vpss_error("invalid port out cmp:%d; rotation:%d\n", vpss_port_cfg->cmp_info.cmp_mode, vpss_port_cfg->rotation);
        return HI_FAILURE;
    }

    if (vpss_port_cfg->port_type == HI_DRV_VPSS_PORT_TYPE_3D_DETECT) {
        if (vpss_port_cfg->rotation != HI_DRV_VPSS_ROTATION_DISABLE) {
            vpss_error("3D detect port can't support rotation %d", vpss_port_cfg->rotation);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 inst_check_frame_port_flag(hi_drv_vpss_port_frame_cfg *port_cfg)
{
    if ((port_cfg->user_crop_en != HI_TRUE) && (port_cfg->user_crop_en != HI_FALSE)) {
        vpss_error("user_crop_en %d is invalid.\n", port_cfg->user_crop_en);
        return HI_FAILURE;
    }

    if ((port_cfg->vertical_flip != HI_TRUE) && (port_cfg->vertical_flip != HI_FALSE)) {
        vpss_error("vertical_flip %d is invalid.\n", port_cfg->vertical_flip);
        return HI_FAILURE;
    }

    if ((port_cfg->horizontal_flip != HI_TRUE) && (port_cfg->horizontal_flip != HI_FALSE)) {
        vpss_error("horizontal_flip %d is invalid.\n", port_cfg->horizontal_flip);
        return HI_FAILURE;
    }

    if ((port_cfg->hdr_follow_src != HI_TRUE) && (port_cfg->hdr_follow_src != HI_FALSE)) {
        vpss_error("hdr_follow_src %d is invalid.\n", port_cfg->hdr_follow_src);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vpss_inst_check_frame_port_cfg(hi_drv_vpss_video_frame *src_frame, hi_drv_vpss_port_frame_cfg *port_cfg)
{
    hi_s32 ret;
    hi_bool support_out_fmt = HI_FALSE;
    hi_bool support_cmp_fmt = HI_FALSE;
    hi_bool support_cmp_rota = HI_FALSE;
    hi_bool check_port_reso = HI_FALSE;

    ret = inst_check_frame_port_flag(port_cfg);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    support_out_fmt = vpss_policy_support_out_fmt(port_cfg->format);
    if (support_out_fmt != HI_TRUE) {
        vpss_error("invalid port out format %d\n", port_cfg->format);
        return HI_FAILURE;
    }

    support_cmp_fmt = vpss_policy_check_out_frame_cmp_fmt(port_cfg->format, port_cfg->cmp_info);
    if (support_cmp_fmt != HI_TRUE) {
        vpss_error("invalid port out cmp format %d\n", port_cfg->format);
        return HI_FAILURE;
    }

    support_cmp_rota = vpss_policy_check_out_frame_cmp_rote(port_cfg->rotation, port_cfg->cmp_info);
    if (support_cmp_rota != HI_TRUE) {
        vpss_error("invalid port out cmp:%d; rotation:%d\n", port_cfg->cmp_info.cmp_mode, port_cfg->rotation);
        return HI_FAILURE;
    }

    check_port_reso = vpss_policy_check_port_reso_limit(src_frame, port_cfg);
    if (check_port_reso != HI_TRUE) {
        vpss_error("invalid port out reso is not aligned \n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


vpss_port *vpss_inst_get_port(vpss_instance *instance, hi_handle h_port)
{
    hi_u32 port_id;
    vpss_port *port = HI_NULL;
    port_id = porthandle_to_portid(h_port);
    if (port_id >= VPSS_PORT_MAX_NUM) {
        vpss_error("invalid port_id %#x.", port_id);
        return HI_NULL;
    }

    port = &(instance->port[port_id]);

    if (port->port_id == HI_INVALID_HANDLE) {
        vpss_error("port doesn't exit, handle %d\n", h_port);
        port = HI_NULL;
    }

    return port;
}

static hi_void vpss_inst_tran_reference_count(vpss_instance *instance, hi_drv_vpss_video_frame *frame)
{
    hi_s32 ret;
    hi_u64 dma_handle = instance->tran_release_frame_info.comm_frame.buf_addr[0].dma_handle;

    if (dma_handle == HI_NULL) {
        memcpy(&(instance->tran_release_frame_info), frame, sizeof(hi_drv_vpss_video_frame));
    }

    if ((dma_handle == frame->comm_frame.buf_addr[0].dma_handle) &&
        (instance->tran_release_frame_info.comm_frame.frame_index == frame->comm_frame.frame_index)) {
        return;
    } else {
        instance->perfor.refcnt_dfx.try_dec_cnt++;
        ret = vpss_comm_decrease_frame_refcount_privframe(&(instance->tran_release_frame_info));
        if (ret == HI_SUCCESS) {
            instance->perfor.refcnt_dfx.try_dec_suc_cnt++;
            vpss_dbg_print_ref_count_info(instance->global_dbg_ctrl, &(instance->tran_release_frame_info),
                                          VPSS_REFCNT_DECREASE_TRAN_TYPE);
        } else {
            vpss_error("decrease ref fail! dma_handle=0x%llx\n",
                       instance->tran_release_frame_info.comm_frame.buf_addr[0].dma_handle);
        }

        memcpy(&(instance->tran_release_frame_info), frame, sizeof(hi_drv_vpss_video_frame));
    }

    return;
}

static hi_void inst_rest_port_dequeue_all_sink_buf(vpss_instance *instance, vpss_port *port,
    hi_drv_video_frame *comm_image, hi_drv_vpss_video_frame *priv_image)
{
    hi_s32 ret;
    hi_bool trans_frame = HI_FALSE;
    hi_u32 deque_num = 0;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;

    /* dequeue all sink buffer */
    if (port->call_back_func.sink_dequeue_frame == HI_NULL) {
        return;
    }

    instance->perfor.out_dfx.dequeue_cnt++;
    ret = port->call_back_func.sink_dequeue_frame(port->h_dst_module, comm_image);
    while ((ret == HI_SUCCESS) && (deque_num <= DEF_OUT_NODE_MAX_NUM)) {
        deque_num++;
        instance->perfor.out_dfx.dequeue_suc_cnt++;

        vpss_comm_cvt_comm_to_privite_nomap(comm_image, priv_image);

        vpss_out_rel_ful_frm(&port->out_list, comm_image);

        meta_info = (hi_drv_win_vpss_meta_info *)priv_image->comm_frame.video_private.vpss_private.data;
        trans_frame = !meta_info->vpss_process;
        if (trans_frame == HI_TRUE) {
            if (meta_info->vdp_advance_frame == HI_TRUE) {
                vpss_inst_tran_reference_count(instance, priv_image);
            }

            instance->perfor.out_dfx.release_trans_cnt++;
            vpss_comm_copy_priv_vpss_meta_to_win_meta(priv_image);
            instance->src_funcs.vpss_rel_in_priv_image(instance->id, priv_image);
        }
        vpss_dbg_print_frame_addr(instance->global_dbg_ctrl, comm_image, VPSS_OUT_DEQUEUE_FRAME_TYPE);

        ret = port->call_back_func.sink_dequeue_frame(port->h_dst_module, comm_image);
    }

    return;
}

hi_s32 vpss_inst_reset_port(vpss_instance *instance, hi_handle h_port)
{
    vpss_port *port = HI_NULL;
    hi_drv_video_frame *comm_image = HI_NULL;
    hi_drv_vpss_video_frame *priv_image = HI_NULL;

    port = vpss_inst_get_port(instance, h_port);
    if (port == HI_NULL) {
        return HI_FAILURE;
    }

    port->out_count = 0;

    comm_image = (hi_drv_video_frame *)vpss_kmalloc(sizeof(hi_drv_video_frame), OSAL_GFP_KERNEL);
    priv_image = (hi_drv_vpss_video_frame *)vpss_kmalloc(sizeof(hi_drv_vpss_video_frame), OSAL_GFP_KERNEL);
    if ((priv_image == HI_NULL) || (comm_image == HI_NULL)) {
        vpss_error("priv_frame malloc failed! \n");
        return HI_FAILURE;
    }
    memset(priv_image, 0, sizeof(hi_drv_vpss_video_frame));
    memset(comm_image, 0, sizeof(hi_drv_video_frame));

    /* dequeue all sink buffer */
    inst_rest_port_dequeue_all_sink_buf(instance, port, comm_image, priv_image);

    vpss_out_reset(&port->out_list);

    vpss_kfree(comm_image);
    vpss_kfree(priv_image);

    return HI_SUCCESS;
}

hi_s32 vpss_inst_set_enable(vpss_instance *instance, hi_bool enable)
{
    unsigned long flags;
    vpss_comm_down_spin(&(instance->usr_set_spin), &flags);
    instance->enable = enable;
    vpss_comm_up_spin(&(instance->usr_set_spin), &flags);
    return HI_SUCCESS;
}

hi_s32 vpss_inst_get_def_inst_cfg(hi_drv_vpss_instance_cfg *instance_cfg)
{
    instance_cfg->quick_enable = HI_FALSE;
    instance_cfg->prog_detect = HI_DRV_VPSS_PROG_DETECT_AUTO;
    instance_cfg->work_mode = HI_DRV_VPSS_WORKMODE_AUTO;
    instance_cfg->pq_mode = HI_DRV_VPSS_PQMODE_ALLPQ;
    return HI_SUCCESS;
}

hi_s32 vpss_inst_set_inst_cfg(vpss_instance *instance, hi_drv_vpss_instance_cfg *instance_cfg)
{
    unsigned long flags;
    hi_drv_vpss_instance_cfg *inst_usrc_cfg;
    inst_usrc_cfg = &(instance->usr_inst_cfg);
    vpss_comm_down_spin(&(instance->usr_set_spin), &flags);
    instance->cfg_new = HI_TRUE;
    memcpy(inst_usrc_cfg, instance_cfg, sizeof(hi_drv_vpss_instance_cfg));
    vpss_comm_up_spin(&(instance->usr_set_spin), &flags);
    return HI_SUCCESS;
}

hi_u32 vpss_inst_get_inst_cfg(vpss_instance *instance, hi_drv_vpss_instance_cfg *instance_cfg)
{
    unsigned long flags;
    hi_drv_vpss_instance_cfg *inst_usrc_cfg;
    inst_usrc_cfg = &(instance->usr_inst_cfg);
    vpss_comm_down_spin(&(instance->usr_set_spin), &flags);
    memcpy(instance_cfg, inst_usrc_cfg, sizeof(hi_drv_vpss_instance_cfg));
    vpss_comm_up_spin(&(instance->usr_set_spin), &flags);
    return HI_SUCCESS;
}
hi_u32 vpss_inst_get_def_port_cfg(hi_drv_vpss_port_cfg *port_cfg)
{
    memset(port_cfg, 0, sizeof(hi_drv_vpss_port_cfg));
    port_cfg->port_type = HI_DRV_VPSS_PORT_TYPE_NORMAL;
    port_cfg->bypass_mode = HI_DRV_VPSS_WORKMODE_AUTO;
    /* asp Info */
    port_cfg->user_crop_en = HI_FALSE;
    memset(&(port_cfg->in_offset_rect), 0, sizeof(hi_drv_crop_rect));
    memset(&(port_cfg->crop_rect), 0, sizeof(hi_drv_rect));
    memset(&(port_cfg->video_rect), 0, sizeof(hi_drv_rect));
    memset(&(port_cfg->out_rect), 0, sizeof(hi_drv_rect));
    port_cfg->buf_cfg.buf_type = HI_DRV_VPSS_BUF_VPSS_ALLOC_MANAGE;
    port_cfg->buf_cfg.buf_num = HI_VPSS_MAX_BUFFER_NUMB;
    port_cfg->format = HI_DRV_PIXEL_FMT_NV21;
    port_cfg->cmp_info.cmp_mode = HI_DRV_COMPRESS_MODE_OFF;
    port_cfg->cmp_info.data_fmt = HI_DRV_DATA_FMT_LINER;
    port_cfg->cmp_info.ycmp_rate = 0;
    port_cfg->cmp_info.ccmp_rate = 0;
    port_cfg->bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
    port_cfg->ori_frame_rate = HI_FALSE;
    port_cfg->max_frame_rate = 120; /* default max frame rate is 120 */
    port_cfg->horizontal_flip = HI_FALSE;
    port_cfg->vertical_flip = HI_FALSE;
    port_cfg->enable_3d = HI_FALSE;
    port_cfg->rotation = HI_DRV_VPSS_ROTATION_DISABLE;
    port_cfg->vertical_flip = HI_FALSE;
    port_cfg->horizontal_flip = HI_FALSE;

    port_cfg->hdr_follow_src = HI_TRUE;
    port_cfg->out_hdr_type = HI_DRV_HDR_TYPE_SDR;

    port_cfg->out_color_desp.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
    port_cfg->out_color_desp.color_space = HI_DRV_COLOR_CS_YUV;
    port_cfg->out_color_desp.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
    port_cfg->out_color_desp.transfer_type = HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR; /* no use */
    port_cfg->out_color_desp.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709; /* no use */

    return HI_SUCCESS;
}

hi_s32 vpss_inst_get_port_cfg(vpss_instance *instance, hi_handle h_port, hi_drv_vpss_port_cfg *port_cfg)
{
    unsigned long flags;
    hi_u32 port_id;
    hi_drv_vpss_port_cfg *usr_port_cfg = HI_NULL;
    port_id = porthandle_to_portid(h_port);
    if (port_id >= VPSS_PORT_MAX_NUM) {
        vpss_error("invalid port_id %d, handle %d", port_id, h_port);
        return HI_FAILURE;
    }

    vpss_comm_down_spin(&(instance->usr_set_spin), &flags);
    usr_port_cfg = &(instance->usr_port_cfg[port_id]);
    memcpy(port_cfg, usr_port_cfg, sizeof(hi_drv_vpss_port_cfg));
    vpss_comm_up_spin(&(instance->usr_set_spin), &flags);
    return HI_SUCCESS;
}

hi_s32 vpss_inst_set_port_cfg(vpss_instance *instance, hi_handle h_port, hi_drv_vpss_port_cfg *port_cfg)
{
    unsigned long flags;
    hi_u32 port_id;
    hi_drv_vpss_port_cfg *usr_port_cfg = HI_NULL;

    port_id = porthandle_to_portid(h_port);
    if (port_id >= VPSS_PORT_MAX_NUM) {
        vpss_error("invalid port_id %d, handle %d", port_id, h_port);
        return HI_FAILURE;
    }

    vpss_comm_down_spin(&(instance->usr_set_spin), &flags);
    instance->cfg_new = HI_TRUE;
    usr_port_cfg = &(instance->usr_port_cfg[port_id]);
    memcpy(usr_port_cfg, port_cfg, sizeof(hi_drv_vpss_port_cfg));
    vpss_comm_up_spin(&(instance->usr_set_spin), &flags);

    return HI_SUCCESS;
}

vpss_instance *vpss_ctrl_get_instance(hi_handle h_vpss);

hi_s32 vpss_inst_proc_read(hi_void *p, hi_void *v)
{
#ifndef __VPSS_UT__
    hi_handle h_handle;
    vpss_instance *instance = HI_NULL;

    if (v == HI_NULL) {
        vpss_error("v is null pointer!\n");
        return -EFAULT;
    }

    h_handle = *(hi_handle *)v;
    instance = vpss_ctrl_get_instance(h_handle);
    if (instance == HI_NULL) {
        vpss_error("can't get instance %d proc!\n", h_handle);
        return HI_FAILURE;
    }

    vpss_state_proc(p, instance);
    vpss_algorithm_proc(p, instance);
    vpss_frame_ori_info_proc(p, instance);
    vpss_frame_revise_info_proc(p, instance);
    vpss_source_frame_list_proc(p, instance);
    vpss_port_proc(p, instance);
    vpss_performance_statistics_proc(p, instance);
#endif
    return HI_SUCCESS;
}

static hi_s32 vpss_inst_echo_help(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    if (vpss_comm_cmd_check(argc, 1, private, private) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    osal_printk("echo help > /proc/msp/vpss_ctrl00\n");
    return HI_SUCCESS;
}

static osal_proc_cmd g_vpss_instance_proc[] = {
    { "help",   vpss_inst_echo_help },
};

hi_s32 vpss_inst_create_proc(vpss_instance *instance)
{
#ifndef __VPSS_UT__
    hi_char proc_name[VPSS_FILE_NAMELENGTH];
    osal_proc_entry *proc_item = HI_NULL;

    snprintf(proc_name, VPSS_FILE_NAMELENGTH, "vpss%02x", instance->id);
    proc_item = osal_proc_add(proc_name, strlen(proc_name));
    if (proc_item == HI_NULL) {
        vpss_error("vpss add proc failed!\n");
        return HI_FAILURE;
    }
    proc_item->private = (hi_void *)&(instance->id);
    proc_item->read = vpss_inst_proc_read;
    proc_item->cmd_list = g_vpss_instance_proc;
    proc_item->cmd_cnt = sizeof(g_vpss_instance_proc) / sizeof(osal_proc_cmd);
#endif
    return HI_SUCCESS;
}

hi_void vpss_inst_destory_proc(vpss_instance *instance)
{
    hi_char proc_name[VPSS_FILE_NAMELENGTH];
    snprintf(proc_name, VPSS_FILE_NAMELENGTH, "vpss%02x", instance->id);
    osal_proc_remove(proc_name, strlen(proc_name));
}

hi_void vpss_inst_flush(vpss_instance *instance)
{
    unsigned long flags;
    vpss_in *in_list = HI_NULL;
    vpss_src *src_list = HI_NULL;

    in_list = &instance->in_list;
    src_list = &instance->src_list;

    vpss_comm_down_spin(&(instance->src_spin), &flags);
    vpss_src_flush(src_list);
    vpss_src_reset(src_list);
    vpss_in_reset(in_list);
    vpss_comm_up_spin(&(instance->src_spin), &flags);
}

hi_s32 vpss_inst_reset(vpss_instance *instance)
{
    hi_u32 count;
    vpss_port *port = HI_NULL;
    vpss_wbc *wbc_list = HI_NULL;
    vpss_stt *pstt_list = HI_NULL;
    wbc_list = &instance->wbc_list[0];
    pstt_list = &instance->stt_list[0];

    /* reset port */
    for (count = 0; count < VPSS_PORT_MAX_NUM; count++) {
        port = &(instance->port[count]);

        if (port->port_id != HI_INVALID_HANDLE) {
            vpss_inst_reset_port(instance, port->port_id);
        }
    }

    /* reset image list */
    vpss_inst_flush(instance);
    /* reset wbc list */
    vpss_wbc_reset(wbc_list);
    vpss_wbc_reset(wbc_list + 1);
    /* reset stt list */
    vpss_stt_reset(pstt_list);
    vpss_stt_reset(pstt_list + 1);
    instance->perfor.cnt_dfx.reset_cnt++;
    return HI_SUCCESS;
}

hi_s32 vpss_inst_acq_in_priv_frame(hi_handle h_vpss, hi_drv_vpss_video_frame *image)
{
    vpss_instance *instance = HI_NULL;
    hi_drv_video_frame *comm_image = HI_NULL;
    hi_s32 ret, ret_umap;

    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("get instance failed , h_vpss = %d\n", h_vpss);
        return HI_FAILURE;
    }

    if (image == HI_NULL) {
        vpss_error("null pointer\n");
        return HI_FAILURE;
    }

    comm_image = (hi_drv_video_frame *)vpss_kmalloc(sizeof(hi_drv_video_frame), OSAL_GFP_KERNEL);
    if (comm_image == HI_NULL) {
        vpss_error("comm_frame malloc failed! \n");
        return HI_FAILURE;
    }
    memset(comm_image, 0, sizeof(hi_drv_video_frame));

    ret = vpss_in_acquire_frame(&instance->in_list, comm_image);
    /* get src frame, must use vpss_priv_frame, only get usefull frame can be mapped */
    if (ret == HI_SUCCESS) {
        vpss_comm_cvt_comm_to_privite_nomap(comm_image, image);

        instance->perfor.buf_dfx.try_map_cnt++;
        ret_umap = vpss_comm_map_priv_video(comm_image, image);
        if (ret_umap != HI_SUCCESS) {
            vpss_error("umap failed \n");
        } else {
            instance->perfor.buf_dfx.map_suc_cnt++;
        }
    }

    vpss_kfree(comm_image);

    return ret;
}

hi_s32 vpss_inst_rel_in_priv_frame(hi_handle h_vpss, hi_drv_vpss_video_frame *image)
{
    hi_s32 ret, ret_umap, ret_in_release;
    vpss_instance *instance = HI_NULL;
    hi_drv_video_frame *comm_image = HI_NULL;

    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("get instance failed , h_vpss = %d\n", h_vpss);
        return HI_FAILURE;
    }

    if (image == HI_NULL) {
        vpss_error("null pointer\n");
        return HI_FAILURE;
    }

    comm_image = (hi_drv_video_frame *)vpss_kmalloc(sizeof(hi_drv_video_frame), OSAL_GFP_KERNEL);
    if (comm_image == HI_NULL) {
        vpss_error("priv_frame malloc failed! \n");
        return HI_FAILURE;
    }
    memset(comm_image, 0, sizeof(hi_drv_video_frame));

    vpss_comm_cvt_privite_to_comm_nomap(image, comm_image);

    ret_in_release = vpss_in_release_frame(&instance->in_list, comm_image);
    /* release  fail means no frame */
    if (ret_in_release == HI_SUCCESS) {
        instance->perfor.buf_dfx.try_umap_cnt++;
        ret_umap = vpss_comm_umap_priv_video(image);
        if (ret_umap != HI_SUCCESS) {
            vpss_error("umap failed \n");
        } else {
            instance->perfor.buf_dfx.umap_suc_cnt++;
        }

        if (instance->src_funcs.vpss_complete_in_comm_image != HI_NULL) {
            instance->perfor.src_dfx.rel_complete_cnt++;

            ret = instance->src_funcs.vpss_complete_in_comm_image(instance->src_funcs.h_src, comm_image);
            if (ret != HI_SUCCESS) {
                vpss_error("vpss complete fence buffer failed");
            } else {
                instance->perfor.src_dfx.rel_complete_suc_cnt++;
            }
        }
    }

    instance->perfor.src_dfx.rel_cnt++;
    memcpy(&(instance->in_list.global_dbg_ctrl), instance->global_dbg_ctrl, sizeof(vpss_debug_info));

    vpss_kfree(comm_image);

    return ret_in_release;
}

hi_s32 vpss_inst_rel_in_undo_image(hi_handle h_vpss, hi_drv_vpss_video_frame *image)
{
    hi_s32 ret;
    vpss_instance *instance = HI_NULL;
    hi_drv_video_frame *comm_image = HI_NULL;

    instance = vpss_ctrl_get_instance(h_vpss);
    if (!instance) {
        return HI_FAILURE;
    }

    comm_image = (hi_drv_video_frame *)vpss_kmalloc(sizeof(hi_drv_video_frame), OSAL_GFP_KERNEL);
    if (comm_image == HI_NULL) {
        vpss_error("priv_frame malloc failed! \n");
        return HI_FAILURE;
    }
    memset(comm_image, 0, sizeof(hi_drv_video_frame));

    if (instance->src_funcs.vpss_complete_in_comm_image != HI_NULL) {
        instance->perfor.src_dfx.rel_complete_cnt++;

        vpss_comm_cvt_privite_to_comm_nomap(image, comm_image);

        ret = instance->src_funcs.vpss_complete_in_comm_image(instance->src_funcs.h_src, comm_image);
        if (ret != HI_SUCCESS) {
            vpss_error("vpss complete fence buffer failed");
        } else {
            instance->perfor.src_dfx.rel_complete_suc_cnt++;
        }
    } else {
        vpss_error("vpss complete src image function is not register\n");
        ret = HI_FAILURE;
    }

    instance->perfor.src_dfx.rel_cnt++;

    vpss_kfree(comm_image);

    return ret;
}

hi_s32 vpss_inst_init(vpss_instance *instance, hi_drv_vpss_instance_cfg *instance_cfg)
{
    hi_drv_vpss_instance_cfg tmp_cfg = { 0 };
    vpss_in_attr in_attr = { 0 };
    hi_u32 count;
    list *list_head = HI_NULL;
    hi_u32 i = 0;

    memset(instance, 0, sizeof(vpss_instance));

    instance->dbg_ctrl.rot_alg_width = VPSS_DEFAULT_WIDTH_FOR_ROTATE_ALG;
    instance->dbg_ctrl.rot_alg_height = VPSS_DEFAULT_WIDTH_FOR_ROTATE_ALG;
    instance->hal_info.pre_frame_addr = 0;

    if (vpss_comm_init_spin(&(instance->usr_set_spin)) != HI_SUCCESS) {
        vpss_error("Init usr_set_spin failed.\n");
        return HI_FAILURE;
    }
    if (vpss_comm_init_spin(&(instance->src_spin)) != HI_SUCCESS) {
        vpss_error("Init src_spin failed.\n");
        vpss_comm_destory_spin(&(instance->usr_set_spin));
        return HI_FAILURE;
    }
    count = 0;

    while (count < VPSS_PORT_MAX_NUM) {
        instance->port[count].port_id = HI_INVALID_HANDLE;
        instance->port[count].enble = HI_FALSE;
        count++;
    }

    if (instance_cfg == HI_NULL) {
        vpss_inst_get_def_inst_cfg(&tmp_cfg);
        vpss_inst_set_inst_cfg(instance, &tmp_cfg);
    } else {
        vpss_inst_set_inst_cfg(instance, instance_cfg);
    }

    in_attr.rls_in_undo_priv_image = vpss_inst_rel_in_undo_image;
    in_attr.h_src_module = instance->id;
    vpss_in_init(&in_attr, &instance->in_list);

    instance->src_funcs.vpss_acq_in_priv_image = vpss_inst_acq_in_priv_frame;
    instance->src_funcs.vpss_rel_in_priv_image = vpss_inst_rel_in_priv_frame;
    instance->src_funcs.vpss_complete_in_comm_image = HI_NULL;
    instance->src_funcs.h_src = HI_INVALID_HANDLE;

    list_head = &(instance->pts_node[0].node);
    OSAL_INIT_LIST_HEAD(list_head);
    instance->first_ref = list_head;

    for (i = 1; i < VPSS_PTS_NODE_NUM; i++) {
        osal_list_add_tail(&(instance->pts_node[i].node), list_head);
    }

    return HI_SUCCESS;
}

hi_void vpss_inst_desroy_alg_buffer(vpss_instance *instance)
{
    vpss_ip vpss_ip;
    vpss_src *src_list;
    vpss_wbc *wbc_list;
    vpss_stt *stt_list;
    src_list = &instance->src_list;
    wbc_list = &instance->wbc_list[0];
    stt_list = &instance->stt_list[0];
    // whenever buffer create,failed or success,we need to call deinit
    instance->perfor.buf_dfx.buf_destroy_time_start = vpss_comm_get_sched_time();
    vpss_ip = instance->ctrl_id;
#ifdef DPT
    free_all_split_node_buffer(vpss_ip);
#endif
    vpss_src_deinit(src_list);
    vpss_wbc_deinit(wbc_list);
    vpss_wbc_deinit(wbc_list + 1);
    vpss_stt_deinit(stt_list);
    vpss_stt_deinit(stt_list + 1);
    instance->perfor.buf_dfx.buf_destroy_time_end = vpss_comm_get_sched_time();
    return;
}

hi_s32 vpss_inst_create_all_buffer(vpss_instance *instance, hi_drv_vpss_video_frame *src_frame)
{
    hi_s32 ret;
    unsigned long flags;
    vpss_src *src_list = &instance->src_list;
    vpss_wbc *wbc_list = &instance->wbc_list[0];
    vpss_stt *stt_list = &instance->stt_list[0];
    vpss_inst_perfor *performance = HI_NULL;
    vpss_src_attr src_attr = { 0 };
    vpss_wbc_attr wbc_attr = { 0 };
    vpss_stt_attr stt_attr = { 0 };
    hi_bool interlace = HI_FALSE;

    src_attr.h_src_module = instance->id;
    src_attr.rls_src_priv_image = instance->src_funcs.vpss_rel_in_priv_image;
    src_attr.mode = get_src_list_mode(src_frame);

    if (src_frame->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_INTERLACE) {
        interlace = HI_TRUE;
    } else if (src_frame->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_PROGRESSIVE) {
        interlace = HI_FALSE;
    } else {
        interlace = HI_FALSE;
        vpss_error("UNKOWN sample_type:%d;\n", src_frame->comm_frame.sample_type);
    }

    wbc_attr.interlace = interlace;
    wbc_attr.secure = src_frame->comm_frame.secure;
    wbc_attr.pixel_format = src_frame->comm_frame.pixel_format;
    wbc_attr.source = src_frame->comm_frame.video_private.source;
    wbc_attr.width = src_frame->comm_frame.width;
    wbc_attr.height = src_frame->comm_frame.height;
    stt_attr.interlace = interlace;
    stt_attr.secure = src_frame->comm_frame.secure;
    stt_attr.pixel_format = src_frame->comm_frame.pixel_format;
    stt_attr.source = src_frame->comm_frame.video_private.source;
    stt_attr.width = src_frame->comm_frame.width;
    stt_attr.height = src_frame->comm_frame.height;
    performance = &instance->perfor;

    performance->buf_dfx.buf_create_time_start = vpss_comm_get_sched_time();
    vpss_inst_desroy_alg_buffer(instance);
    instance->stream_info.real_topfirst = src_frame->comm_frame.top_field_first;
    instance->stream_info.pre_fieldmode = (instance->stream_info.real_topfirst == HI_TRUE) ? HI_DRV_FIELD_BOTTOM :
        HI_DRV_FIELD_TOP;
    vpss_comm_down_spin(&(instance->src_spin), &flags);
    ret = vpss_src_init(src_list, src_attr);
    vpss_comm_up_spin(&(instance->src_spin), &flags);
    ret += vpss_wbc_init(wbc_list, &wbc_attr);
    ret += vpss_stt_init(stt_list, &stt_attr);

    if (src_frame->comm_frame.video_3d_type != HI_DRV_3D_NONE) {
        ret += vpss_wbc_init(wbc_list + 1, &wbc_attr);
        ret += vpss_stt_init(stt_list + 1, &stt_attr);
    }

    if (ret != HI_SUCCESS) {
        vpss_error("vpss create buffer failed,can't work");
        return ret;
    }

    performance->buf_dfx.buf_create_time_end = vpss_comm_get_sched_time();
    return HI_SUCCESS;
}

static hi_void vpss_inst_last_tran_refrence_cnt(vpss_instance *instance)
{
    hi_s32 ret;
    hi_bool vpss_process = HI_FALSE;
    hi_drv_win_vpss_meta_info *meta = HI_NULL;

    meta = (hi_drv_win_vpss_meta_info *)instance->tran_release_frame_info.comm_frame.video_private.vpss_private.data;
    if (meta == HI_NULL) {
        vpss_process = HI_FALSE;
    } else {
        vpss_process = meta->vpss_process;
    }
    if ((vpss_process != HI_TRUE) &&
        (instance->tran_release_frame_info.vpss_buf_addr[0].phy_addr_y != 0)) {
        instance->perfor.refcnt_dfx.try_dec_cnt++;
        ret = vpss_comm_decrease_frame_refcount_privframe(&(instance->tran_release_frame_info));
        if (ret == HI_SUCCESS) {
            instance->perfor.refcnt_dfx.try_dec_suc_cnt++;
            vpss_dbg_print_ref_count_info(instance->global_dbg_ctrl, &(instance->tran_release_frame_info),
                                          VPSS_REFCNT_DECREASE_TRAN_TYPE);
        } else {
            vpss_error("decrease ref fail! addr=%08x\n",
                       instance->tran_release_frame_info.vpss_buf_addr[0].phy_addr_y);
        }

        instance->tran_release_frame_info.vpss_buf_addr[0].phy_addr_y = 0;
    }

    return;
}

hi_void vpss_inst_deinit(vpss_instance *instance)
{
    hi_u32 count;
    unsigned long flags;
    vpss_port *port = HI_NULL;
    vpss_in *in_list = HI_NULL;
    hi_drv_pq_vpss_info timing_info = {0};

    if (instance->use_pq_module == HI_TRUE) {
        timing_info.handle_id = instance->id;
        vpss_comm_pq_update_timing_info(instance->id, &timing_info, HI_NULL);
    }

    in_list = &instance->in_list;
    vpss_comm_down_spin(&(instance->src_spin), &flags);
    vpss_in_deinit(in_list);
    vpss_comm_up_spin(&(instance->src_spin), &flags);

    vpss_inst_desroy_alg_buffer(instance);

    for (count = 0; count < VPSS_PORT_MAX_NUM; count++) {
        if (instance->ro_buf[count].buff_info.size != 0) {
            vpss_comm_mem_free(&instance->ro_buf[count].buff_info);
            instance->ro_buf[count].buff_info.size = 0;
        }
    }

    for (count = 0; count < VPSS_PORT_MAX_NUM; count++) {
        port = &(instance->port[count]);
        if (port->port_id != HI_INVALID_HANDLE) {
            vpss_inst_destory_port(instance, port->port_id);
            port->port_id = HI_INVALID_HANDLE;
        }
    }

    vpss_inst_last_tran_refrence_cnt(instance);
    vpss_inst_destory_proc(instance);

    if (instance->usr_set_spin.lock != HI_NULL) {
        vpss_comm_destory_spin(&(instance->usr_set_spin));
    }

    if (instance->src_spin.lock != HI_NULL) {
        vpss_comm_destory_spin(&(instance->src_spin));
    }
}

hi_s32 vpss_inst_create_port(vpss_instance *instance, hi_drv_vpss_port_cfg *port_cfg, hi_handle *ph_port)
{
    hi_u32 count = 0;
    vpss_port *port = HI_NULL;
    hi_s32 ret;
    hi_drv_vpss_port_cfg port_def_cfg;
    hi_drv_vpss_port_cfg *port_set_cfg = HI_NULL;
    vpss_out_attr out_attr;

    for (count = 0; count < VPSS_PORT_MAX_NUM; count++) {
        port = &(instance->port[count]);
        if (port->port_id == HI_INVALID_HANDLE) {
            break;
        }
    }

    if (count == VPSS_PORT_MAX_NUM) {
        vpss_error("port number is MAX %d.\n", count);
        *ph_port = 0;
        return HI_FAILURE;
    }

    memset(port, 0, sizeof(vpss_port));

    if (port_cfg == HI_NULL) {
        vpss_inst_get_def_port_cfg(&(port_def_cfg));
        port_set_cfg = &(port_def_cfg);
    } else {
        port_set_cfg = port_cfg;
    }

    ret = vpss_inst_check_port_cfg(instance, port_set_cfg);
    if (ret != HI_SUCCESS) {
        vpss_error("set_port_cfg error.\n");
        return HI_FAILURE;
    }

    out_attr.buf_num = HI_VPSS_MAX_BUFFER_NUMB;

    if (instance->global_dbg_ctrl->set_buff_num == HI_TRUE) {
        out_attr.buf_num = instance->global_dbg_ctrl->buff_num;
    }

    ret = vpss_out_init(&(port->out_list), &out_attr);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    port->enble = HI_FALSE;
    port->port_id = (instance->id << 8) + count;  // 8 is used to cal port id
    port->out_count = 0;
    port->call_back_func.sink_dequeue_frame = HI_NULL;
    port->call_back_func.sink_queue_frame = HI_NULL;
    port->call_back_func.get_port_cfg_by_frame = HI_NULL;
    port->call_back_func.get_ai_cfg_by_frame = HI_NULL;

    if (vpss_inst_set_port_cfg(instance, port->port_id, port_set_cfg) != HI_SUCCESS) {
        vpss_error("set_port_cfg error.\n");
    }

    *ph_port = port->port_id;
    return HI_SUCCESS;
}

hi_s32 vpss_inst_destory_port(vpss_instance *instance, hi_handle h_port)
{
    vpss_port *port = HI_NULL;

    port = vpss_inst_get_port(instance, h_port);
    if (port == HI_NULL) {
        vpss_error("invalid port_id %d", h_port);
        return HI_FAILURE;
    }

    vpss_out_deinit(&(port->out_list));

    memset(port, 0, sizeof(vpss_port));
    port->port_id = HI_INVALID_HANDLE;

    return HI_SUCCESS;
}

hi_s32 vpss_inst_enable_port(vpss_instance *instance, hi_handle h_port, hi_bool port_en)
{
    vpss_port *port = HI_NULL;
    port = vpss_inst_get_port(instance, h_port);
    if (port == HI_NULL) {
        vpss_error("invalid h_port ID %d\n", h_port);
        return HI_FAILURE;
    }

    port->enble = port_en;
    return HI_SUCCESS;
}

hi_s32 vpss_inst_set_pause(vpss_instance *instance, hi_bool pause)
{
    instance->pause = pause;
    instance->stream_info.pre_fieldmode = HI_DRV_FIELD_ALL;  // pause can't jump field
    return HI_SUCCESS;
}

hi_s32 vpss_inst_set_hold(vpss_instance *instance, hi_bool hold)
{
    instance->hold = hold;
    return HI_SUCCESS;
}

hi_s32 vpss_inst_reply_user_command(vpss_instance *instance,
                                    hi_drv_vpss_user_command command,
                                    hi_void *args)
{
    hi_s32 ret = HI_FAILURE;
    hi_bool *pause = HI_NULL;
    hi_bool *hold = HI_NULL;

    switch (command) {
        case HI_DRV_VPSS_USER_COMMAND_START:
            ret = vpss_inst_set_enable(instance, HI_TRUE);
            break;
        case HI_DRV_VPSS_USER_COMMAND_STOP:
            ret = vpss_inst_set_enable(instance, HI_FALSE);
            break;
        case HI_DRV_VPSS_USER_COMMAND_RESET:
            ret = vpss_inst_reset(instance);
            break;
        case HI_DRV_VPSS_USER_COMMAND_PAUSE:
            if (args == HI_NULL) {
                vpss_error("p_args is null \n");
                return HI_FAILURE;
            }

            pause = (hi_bool *)args;
            ret = vpss_inst_set_pause(instance, *pause);
            break;
        case HI_DRV_VPSS_USER_COMMAND_HOLD:
            if (args == HI_NULL) {
                vpss_error("p_args is null \n");
                return HI_FAILURE;
            }

            hold = (hi_bool *)args;
            ret = vpss_inst_set_hold(instance, *hold);
            break;
        default:
            vpss_error("unknown command = %d\n", command);
            break;
    }

    return ret;
}

hi_s32 vpss_inst_queue_frame(vpss_instance *instance, hi_drv_video_frame *frame)
{
    hi_s32 ret;
    vpss_in *in_list = HI_NULL;
    hi_drv_vpss_video_frame *priv_image = HI_NULL;

    instance->perfor.in_dfx.queue_cnt++;
    in_list = &instance->in_list;
    instance->perfor.refcnt_dfx.try_inc_cnt++;

    priv_image = (hi_drv_vpss_video_frame *)vpss_kmalloc(sizeof(hi_drv_vpss_video_frame), OSAL_GFP_KERNEL);
    if (priv_image == HI_NULL) {
        vpss_error("priv_frame malloc failed! \n");
        return HI_FAILURE;
    }
    memset(priv_image, 0, sizeof(hi_drv_vpss_video_frame));

    ret = vpss_in_queue_frame(in_list, frame);

    vpss_comm_cvt_comm_to_privite_nomap(frame, priv_image);

    if (ret != HI_SUCCESS) {
        instance->perfor.refcnt_dfx.try_dec_cnt++;

        if (vpss_comm_decrease_frame_refcount(frame) == HI_SUCCESS) {
            instance->perfor.refcnt_dfx.try_dec_suc_cnt++;
            vpss_dbg_print_ref_count_info(instance->global_dbg_ctrl, &(instance->tran_release_frame_info),
                                          VPSS_REFCNT_DECREASE_UNPRO_TYPE);
        } else {
            vpss_error("decrease ref fail! addr=%08x\n", priv_image->vpss_buf_addr[0].phy_addr_y);
        }

        vpss_kfree(priv_image);

        return ret;
    }

    ret = vpss_comm_increase_frame_refcount(frame);
    if (ret == HI_SUCCESS) {
        instance->perfor.refcnt_dfx.try_inc_suc_cnt++;
        vpss_dbg_print_ref_count_info_comm_frame(instance->global_dbg_ctrl, frame, VPSS_REFCNT_INCREASE_TYPE);
    } else {
        vpss_kfree(priv_image);
        return ret;
    }

    /* overlay need vpss_process status */
    vpss_dbg_print_frame_addr(instance->global_dbg_ctrl, frame, VPSS_IN_QUEUE_FRAME_TYPE);
    instance->perfor.in_dfx.queue_suc_cnt++;

    vpss_kfree(priv_image);

    return ret;
}

hi_s32 vpss_inst_dequeue_frame(vpss_instance *instance, hi_drv_video_frame *frame)
{
    hi_s32 ret;
    vpss_in *in_list = HI_NULL;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;
    hi_drv_vpss_video_frame *priv_image = HI_NULL;

    instance->perfor.in_dfx.dequeue_cnt++;
    in_list = &instance->in_list;
    ret = vpss_in_dequeue_frame(in_list, frame);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    priv_image = (hi_drv_vpss_video_frame *)vpss_kmalloc(sizeof(hi_drv_vpss_video_frame), OSAL_GFP_KERNEL);
    if (priv_image == HI_NULL) {
        vpss_error("priv_frame malloc failed! \n");
        return HI_FAILURE;
    }
    memset(priv_image, 0, sizeof(hi_drv_vpss_video_frame));

    vpss_comm_cvt_comm_to_privite_nomap(frame, priv_image);

    meta_info = (hi_drv_win_vpss_meta_info *)frame->video_private.win_private.data;
    if (meta_info->vpss_reset_frame == HI_TRUE) {
        instance->perfor.refcnt_dfx.try_dec_cnt++;
        ret = vpss_comm_decrease_frame_refcount(frame);
        if (ret == HI_SUCCESS) {
            instance->perfor.refcnt_dfx.try_dec_suc_cnt++;
            vpss_dbg_print_ref_count_info_comm_frame(instance->global_dbg_ctrl, frame, VPSS_REFCNT_DECREASE_UNPRO_TYPE);
        } else {
            vpss_error("decrease ref fail! addr=0x%llx \n", frame->buf_addr[0].dma_handle);
        }
    }

    vpss_dbg_print_frame_addr(instance->global_dbg_ctrl, frame, VPSS_IN_DEQUEUE_FRAME_TYPE);
    instance->perfor.in_dfx.dequeue_suc_cnt++;

    vpss_kfree(priv_image);

    return ret;
}

hi_s32 vpss_inst_rel_port_frame(vpss_instance *instance, hi_handle h_port, hi_drv_video_frame *frame)
{
    vpss_port *port = HI_NULL;
    hi_u32 port_id;
    vpss_out *out_list = HI_NULL;
    hi_s32 ret, ret_umap;
    hi_bool trans_frame = HI_FALSE;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;
    hi_drv_vpss_video_frame *priv_image = HI_NULL;

    port = vpss_inst_get_port(instance, h_port);
    if (port == HI_NULL) {
        vpss_error("invalid h_port ID %d, handle %d\n", h_port, h_port);
        return HI_FAILURE;
    }

    instance->perfor.out_dfx.release_cnt++;

    meta_info = (hi_drv_win_vpss_meta_info *)frame->video_private.vpss_private.data;
    trans_frame = !meta_info->vpss_process;
    out_list = &(port->out_list);

    ret = vpss_out_rel_ful_frm(out_list, frame);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    priv_image = (hi_drv_vpss_video_frame *)vpss_kmalloc(sizeof(hi_drv_vpss_video_frame), OSAL_GFP_KERNEL);
    if (priv_image == HI_NULL) {
        vpss_error("priv_frame malloc failed! \n");
        return HI_FAILURE;
    }
    memset(priv_image, 0, sizeof(hi_drv_vpss_video_frame));

    vpss_comm_cvt_comm_to_privite_nomap(frame, priv_image);

    if (trans_frame == HI_TRUE) {
        instance->perfor.out_dfx.release_trans_cnt++;
        vpss_error("rel_port_frame index:%d\n", frame->frame_index);
        instance->src_funcs.vpss_rel_in_priv_image(instance->id, priv_image);
    }

    port_id = porthandle_to_portid(h_port);
    vpss_dbg_print_frame_addr(instance->global_dbg_ctrl, frame, VPSS_OUT_RELEASE_FRAME_TYPE);
    vpss_dbg_print_frame(instance->global_dbg_ctrl, priv_image, port_id);

    instance->perfor.out_dfx.release_suc_cnt++;

    instance->perfor.buf_dfx.try_umap_cnt++;
    ret_umap = vpss_comm_umap_priv_video(priv_image);
    if (ret_umap != HI_SUCCESS) {
        vpss_error("umap failed \n");
    } else {
        instance->perfor.buf_dfx.umap_suc_cnt++;
    }

    vpss_kfree(priv_image);

    return ret;
}

hi_s32 vpss_inst_get_port_frame(vpss_instance *instance, hi_handle h_port, hi_drv_video_frame *frame)
{
    vpss_port *port = HI_NULL;
    vpss_out *out_list = HI_NULL;
    hi_u32 port_id;
    hi_s32 ret, ret_map;
    hi_drv_vpss_video_frame *priv_image = HI_NULL;

    port_id = porthandle_to_portid(h_port);
    if (port_id >= VPSS_PORT_MAX_NUM) {
        vpss_error("invalid port_id %d, handle %d.", port_id, h_port);
        return HI_FAILURE;
    }

    instance->perfor.out_dfx.acquire_cnt++;
    port = vpss_inst_get_port(instance, h_port);
    if (port == HI_NULL) {
        vpss_error("invalid port_id %d, handle %d.", port_id, h_port);
        return HI_FAILURE;
    }

    out_list = &(port->out_list);
    ret = vpss_out_get_ful_frm(out_list, frame);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    priv_image = (hi_drv_vpss_video_frame *)vpss_kmalloc(sizeof(hi_drv_vpss_video_frame), OSAL_GFP_KERNEL);
    if (priv_image == HI_NULL) {
        vpss_error("priv_frame malloc failed! \n");
        return HI_FAILURE;
    }
    memset(priv_image, 0, sizeof(hi_drv_vpss_video_frame));

    vpss_comm_cvt_comm_to_privite_nomap(frame, priv_image);

    instance->perfor.buf_dfx.try_map_cnt++;
    ret_map = vpss_comm_map_priv_video(frame, priv_image);
    if (ret_map != HI_SUCCESS) {
        vpss_error("umap failed \n");
    } else {
        instance->perfor.buf_dfx.map_suc_cnt++;
    }

    vpss_dbg_print_frame_addr(instance->global_dbg_ctrl, frame, VPSS_OUT_ACQUIRE_FRAME_TYPE);
    vpss_dbg_print_frame(instance->global_dbg_ctrl, priv_image, port_id);
    vpss_dbg_save_frame(instance->global_dbg_ctrl, priv_image, port_id);
    instance->perfor.out_dfx.acquire_suc_cnt++;

    vpss_kfree(priv_image);

    return HI_SUCCESS;
}

hi_s32 vpss_inst_regist_hook_event(vpss_instance *instance, hi_handle dst_id, fn_vpss_event_callback event_callback)
{
    instance->h_dst = dst_id;

    if (event_callback != HI_NULL) {
        instance->event_callback = event_callback;
        return HI_SUCCESS;
    } else {
        return HI_FAILURE;
    }
}

hi_s32 vpss_inst_regist_callback(vpss_instance *instance, hi_handle h_port, hi_handle h_sink,
                                 hi_handle h_src, hi_drv_vpss_callback_func *fn_vpss_callback)
{
    vpss_port *port = HI_NULL;

    port = vpss_inst_get_port(instance, h_port);
    if (port == HI_NULL) {
        vpss_error("invalid h_port ID %d\n", h_port);
        return HI_FAILURE;
    }

    if ((fn_vpss_callback->sink_dequeue_frame == HI_NULL) ||
        (fn_vpss_callback->sink_queue_frame == HI_NULL) ||
        (fn_vpss_callback->src_complete == HI_NULL)) {
        vpss_error("regist hook fail, call back func is null");
        return HI_FAILURE;
    }

    if ((fn_vpss_callback->get_port_cfg_by_frame == HI_NULL) ||
        (fn_vpss_callback->get_ai_cfg_by_frame == HI_NULL)) {
        vpss_warn("not register all the callback function\n");
    }

    port->sink_func.h_sink = h_sink;
    port->sink_func.dst_func = *fn_vpss_callback;

    instance->src_funcs.h_src = h_src;
    instance->src_funcs.vpss_complete_in_comm_image = fn_vpss_callback->src_complete;
    instance->src_funcs.get_ai_cfg_by_frame = fn_vpss_callback->get_ai_cfg_by_frame;

    return HI_SUCCESS;
}

hi_void vpss_inst_debug_ctr(vpss_instance *instance, hi_drv_vpss_video_frame *frame)
{
    if (instance->global_dbg_ctrl->set_prog_inter_en) {
        if (instance->global_dbg_ctrl->set_interlaced == HI_TRUE) {
            frame->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_INTERLACE;
        } else {
            frame->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
        }
    }

    if (instance->global_dbg_ctrl->set_pause) {
        instance->pause = HI_TRUE;
    }
}

hi_void vpss_inst_update_stream_info(hi_drv_vpss_video_frame *src_image, vpss_stream_info *stream_info)
{
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;

    meta_info = (hi_drv_win_vpss_meta_info *)src_image->comm_frame.video_private.win_private.data;

    stream_info->vpss_process = meta_info->vpss_process;
    stream_info->force_pq_close = meta_info->force_pq_close;

    stream_info->top_first = src_image->comm_frame.top_field_first;

    if (src_image->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_INTERLACE) {
        stream_info->interlace = HI_TRUE;
    } else if (src_image->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_PROGRESSIVE) {
        stream_info->interlace = HI_FALSE;
    } else {
        stream_info->interlace = HI_FALSE;
        vpss_error("UNKOWN sample_type:%d;\n", src_image->comm_frame.sample_type);
    }

    stream_info->height = src_image->comm_frame.height;
    stream_info->width = src_image->comm_frame.width;
    stream_info->special_field = vpss_policy_check_special_field(src_image);
    stream_info->video_3d_type = src_image->comm_frame.video_3d_type;
    stream_info->field_mode = src_image->comm_frame.field_mode;
    stream_info->secure = src_image->comm_frame.secure;
    stream_info->color_sys = src_image->comm_frame.video_private.color_sys;
    stream_info->source = src_image->comm_frame.video_private.source;
    stream_info->pixel_format = src_image->comm_frame.pixel_format;
    stream_info->cmp_info = src_image->comm_frame.cmp_info;
    stream_info->bit_width = src_image->comm_frame.bit_width;

    stream_info->src_hdr_type = src_image->comm_frame.hdr_type;
    stream_info->src_color_desp = src_image->comm_frame.color_space;
}

hi_void vpss_inst_fill_attr_info(hi_drv_vpss_video_frame *frame, vpss_out_frame_info *attr)
{
    attr->secure = frame->comm_frame.secure;
    attr->vpss_process = HI_FALSE;
    attr->video_3d_type = frame->comm_frame.video_3d_type;
    attr->bit_width = frame->comm_frame.bit_width;
    attr->pixel_format = frame->comm_frame.pixel_format;
    attr->cmp_info = frame->comm_frame.cmp_info;
    attr->height = frame->comm_frame.height;
    attr->width = frame->comm_frame.width;
}
hi_s32 vpss_inst_check_port_num(hi_u32 count)
{
    if (count == 0) {
        return HI_FAILURE;
    }

    if (count > 1) {
        vpss_error("trans fb warning,use multi port, port num %d.\n", count);
    }

    return HI_SUCCESS;
}

hi_s32 vpss_inst_process_trans_frame(vpss_instance *instance, hi_drv_vpss_video_frame *frame)
{
    hi_s32 ret;
    hi_u32 port_id;
    hi_u32 count = 0;
    vpss_port *port = HI_NULL;
    vpss_out_node *fb_node = HI_NULL;
    vpss_out_frame_info attr;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;

    vpss_inst_fill_attr_info(frame, &attr);

    for (port_id = 0; port_id < VPSS_PORT_MAX_NUM; port_id++) {
        port = &(instance->port[port_id]);
        if ((port->port_id == HI_INVALID_HANDLE) || (port->enble == HI_FALSE) ||
            (port->port_type == HI_DRV_VPSS_PORT_TYPE_3D_DETECT)) {
            continue;
        }

        fb_node = vpss_out_get_empty_buf(&port->out_list, &attr);
        if (fb_node == HI_NULL) {
            vpss_error("port%d buffer is full.\n", port_id);
            continue;
        }

        /* step 1 : copy metadata */
        meta_info = (hi_drv_win_vpss_meta_info *)frame->comm_frame.video_private.win_private.data;
        meta_info->pause_frame = instance->pause;

        frame->comm_frame.video_private.vpss_private.data = (hi_u64)fb_node->metedata.vir_addr;
        vpss_comm_copy_priv_win_meta_to_vpss_meta(frame);

        /* step 2 : out frame info copy */
        memcpy(&(fb_node->out_priv_frame), frame, sizeof(hi_drv_vpss_video_frame));

        /* step 3 : trans only copy comm priv frame */
        memcpy(&(fb_node->out_frame), &(fb_node->out_priv_frame), sizeof(hi_drv_video_frame));

        vpss_out_add_ful_frm_buf(&port->out_list, fb_node);

        if (port->call_back_func.sink_queue_frame != HI_NULL) {
            instance->perfor.out_dfx.queue_trans_cnt++;
            vpss_dbg_print_frame(instance->global_dbg_ctrl, frame, port_id);
            vpss_dbg_save_frame(instance->global_dbg_ctrl, frame, port_id);
            vpss_dbg_print_frame_addr(instance->global_dbg_ctrl, &(fb_node->out_frame), VPSS_OUT_QUEUE_FRAME_TYPE);

            ret = port->call_back_func.sink_queue_frame(port->h_dst_module, &(fb_node->out_frame));
            if (ret != HI_SUCCESS) {
                vpss_error("VPSS queue_frame to vdp failed! id:%d laddr:0x%llx raddr:0x%llx\n",
                           frame->comm_frame.frame_index,
                           frame->comm_frame.buf_addr[0].dma_handle,
                           frame->comm_frame.buf_addr[1].dma_handle);
                vpss_out_move_target(&port->out_list);
                vpss_out_rel_ful_frm(&port->out_list, &(fb_node->out_frame));

                vpss_comm_copy_priv_vpss_meta_to_win_meta(&(fb_node->out_priv_frame));
                instance->src_funcs.vpss_rel_in_priv_image(instance->id, &(fb_node->out_priv_frame));
            } else {
                vpss_comm_notify_event(frame, HI_STAT_LD_EVENT_VPSS_FRM_OUT);
                vpss_out_move_target(&port->out_list);
                instance->perfor.out_dfx.queue_trans_suc_cnt++;
            }
        }

        count++;
    }

    ret = vpss_inst_check_port_num(count);

    return ret;
}

hi_bool check_atv_source(hi_drv_vpss_video_frame *image)
{
    if ((image->comm_frame.video_private.source != HI_DRV_SOURCE_MEDIA) &&
        (image->comm_frame.video_private.source != HI_DRV_SOURCE_DTV) &&
        (image->comm_frame.video_private.source != HI_DRV_SOURCE_NETWORK)) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_bool check_hdr_frame(hi_drv_vpss_video_frame *image)
{
    /* all streams except SDR fix to prog */
    if (image->comm_frame.hdr_type != HI_DRV_HDR_TYPE_SDR) {
        image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_bool check_inter_min_frame_rate(hi_drv_vpss_video_frame *image)
{
    if ((image->comm_frame.video_private.source == HI_DRV_SOURCE_DTV) &&
        (image->comm_frame.frame_rate >= 35000) && /* 35000 is used to judge */
        (image->comm_frame.field_mode == HI_DRV_FIELD_ALL)) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_bool check_tile_frame(hi_drv_vpss_video_frame *image)
{
    if ((image->comm_frame.cmp_info.data_fmt == HI_DRV_DATA_FMT_TILE) &&
        ((image->comm_frame.cmp_info.cmp_mode == HI_DRV_COMPRESS_MODE_SEG_LOSS) ||
         (image->comm_frame.cmp_info.cmp_mode == HI_DRV_COMPRESS_MODE_SEG_LOSSLESS))) {
        return HI_TRUE;
    }

    if (((image->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV12) ||
         (image->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV21) ||
         (image->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV12) ||
         (image->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV21)) &&
        ((image->comm_frame.cmp_info.data_fmt == HI_DRV_DATA_FMT_TILE) ||
         (image->comm_frame.cmp_info.cmp_mode != HI_DRV_COMPRESS_MODE_OFF))) {
        if (image->comm_frame.bit_width == HI_DRV_PIXEL_BITWIDTH_10BIT) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

hi_bool check_yuyv_frame(hi_drv_vpss_video_frame *image)
{
    if ((image->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_YUYV) ||
        (image->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_YVYU) ||
        (image->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_UYVY)) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_void vpss_inst_correct_prog_with_cfg(vpss_instance *instance, hi_drv_vpss_video_frame *image)
{
    if (instance->prog_detect == HI_DRV_VPSS_PROG_DETECT_AUTO) {
        if ((instance->rev_new_frame.comm_frame.codec_type == HI_DRV_VCODEC_REAL8) ||
            (instance->rev_new_frame.comm_frame.codec_type == HI_DRV_VCODEC_REAL9) ||
            (instance->rev_new_frame.comm_frame.codec_type == HI_DRV_VCODEC_MPEG4)) {
            return;
        }

        if (image->comm_frame.height == 720) { /* 720 is used to judge due to the experiences */
            image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
        } else if (image->comm_frame.height <= 576) {     /* 576 is used to judge due to the experiences */
            if ((image->comm_frame.height == 480) && (image->comm_frame.frame_rate > 30000)) { /* 480,30000:judge */
                image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
            } else if ((image->comm_frame.height <= 240) && (image->comm_frame.width <= 320)) { /* 240,320:judge */
                image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
            } else if (image->comm_frame.height <= (image->comm_frame.width * 9 / 14)) { /* 9/14:experiences */
                /* rule: wide aspect ratio stream is normal progressive. we think that progressive info is correct. */
            } else {
                vpss_info("sample_type is revised to inter \n");
                image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_INTERLACE;
            }
        } else {
        }
    } else if (instance->prog_detect == HI_DRV_VPSS_PROG_DETECT_FORCE_INTERLACE) {
        image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_INTERLACE;
    } else if (instance->prog_detect == HI_DRV_VPSS_PROG_DETECT_FORCE_PROGRESSIVE) {
        image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
    }
}

hi_bool vpss_inst_correct_prog_by_resolution(hi_drv_vpss_video_frame *image)
{
    if ((image->comm_frame.height > VPSS_MAX_HEIGHT_INTER) || (image->comm_frame.width > VPSS_MAX_WIDTH_INTER)) {
        image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
        return HI_TRUE;
    }

    if (((image->comm_frame.height == VPSS_HEIGHT_INTER) || (image->comm_frame.height == VPSS_MAX_HEIGHT_INTER))
        && (image->comm_frame.width == VPSS_MAX_WIDTH_INTER)) {
        if (image->comm_frame.frame_rate > VPSS_MAX_FRAMERATE_FOR_INTER) {
            image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
            return HI_TRUE;
        }

        /* Decoder cannot give frame  right progressive info for AVS+ */
        if (image->comm_frame.codec_type == HI_DRV_VCODEC_AVS) {
            image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_INTERLACE;
            return HI_TRUE;
        }
    }

    if ((image->comm_frame.height < VPSS_MIN_HEIGHT_INTER) || (image->comm_frame.width < VPSS_MIN_WIDTH_INTER)) {
        image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_s32 vpss_inst_correct_prog_by_frame_info(hi_drv_vpss_video_frame *image)
{
    hi_bool is_special_field = HI_FALSE;

    if (vpss_inst_correct_prog_by_resolution(image) == HI_TRUE) {
        return HI_SUCCESS;
    }

    is_special_field = vpss_policy_check_special_field(image);
    if (is_special_field == HI_TRUE) {
        if ((image->comm_frame.height * 2) > VPSS_MAX_HEIGHT_INTER) { /* 2 : half reso */
            image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
            return HI_SUCCESS;
        }
    }

    /* belive VI, VI will give the right info */
#ifdef DPT
    if (((image->tunnel_src >> 16) & 0xff) == HI_ID_VI) { /* 16 : vi tunnel limit */
        return HI_SUCCESS;
    }
#endif

    /* progressive is true when overlay */
    if (image->comm_frame.secure == HI_TRUE) {
        image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
        return HI_SUCCESS;
    }

    if (check_atv_source(image) == HI_TRUE) {
        return HI_SUCCESS;
    }

    if (check_hdr_frame(image) == HI_TRUE) {
        image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
        return HI_SUCCESS;
    }

    if (check_inter_min_frame_rate(image) == HI_TRUE) {
        image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
        return HI_SUCCESS;
    }

    /* i frame progressive only */
    if (image->comm_frame.single_i_frame == HI_TRUE) {
        image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
        return HI_SUCCESS;
    }

    if (check_tile_frame(image) == HI_TRUE) {
        image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
        return HI_SUCCESS;
    }

    /* if not top/bottom interleaved,force split */
    if (image->comm_frame.field_mode != HI_DRV_FIELD_ALL) {
        image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_INTERLACE;
        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

hi_void vpss_inst_correct_prog_info(vpss_instance *instance, hi_drv_vpss_video_frame *image)
{
    hi_bool rota_alg_en = HI_FALSE;
    hi_u32 count;
    vpss_port *port = HI_NULL;

    rota_alg_en = vpss_policy_support_rota_alg(&instance->dbg_ctrl, image->comm_frame.width, image->comm_frame.height);

    for (count = 0; count < VPSS_PORT_MAX_NUM; count++) {
        port = &instance->port[count];
        if ((port->port_id == HI_INVALID_HANDLE) || (instance->port[count].enble != HI_TRUE)) {
            continue;
        }

        if ((port->rotation != HI_DRV_VPSS_ROTATION_DISABLE) && (rota_alg_en != HI_TRUE)) {
            image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
            return;
        }
    }

    if (instance->quick_out == HI_TRUE) {
        image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
        return;
    }

    if (image->comm_frame.video_private.last_flag == HI_DRV_LAST_ERROR_FLAG) {
        return;
    }

    if (vpss_inst_correct_prog_by_frame_info(image) == HI_SUCCESS) {
        return;
    }

    if (image->comm_frame.video_3d_type != HI_DRV_3D_NONE) {
        image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
        return;
    }

    if (check_yuyv_frame(image) == HI_TRUE) {
        image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_PROGRESSIVE;
        return;
    }

    /* H264 source is default to interlaced */
    if (image->comm_frame.codec_type == HI_DRV_VCODEC_H264) {
        image->comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_INTERLACE;
        return;
    }

#if (defined HI_VPSS_DRV_USE_GOLDEN) || (defined HI_VPSS_DRV_USE_GOLDEN_COEF)
    return;
#endif
    vpss_inst_correct_prog_with_cfg(instance, image);
}

hi_void vpss_inst_change_in_rate(vpss_stream_info *stream_info, hi_u32 in_rate)
{
    hi_u32 hz_rate;            /* 0 -- 100 */
    hz_rate = in_rate / 1000;  // 1000 is used to cal

    if (hz_rate < 10) {         // 10 is used to judge
        hz_rate = 1;            // 1 is used to assign
    } else if (hz_rate < 20) {  // 20 is used to judge
        hz_rate = 10;           // 10 is used to assign
    } else if (hz_rate < 30) {  // 30 is used to judge
        hz_rate = 25;           // 25 is used to assign
    } else if (hz_rate < 40) {  // 40 is used to judge
        hz_rate = 30;           // 30 is used to assign
    } else if (hz_rate < 60) {  // 60 is used to judge
        hz_rate = 50;           // 50 is used to assign
    } else {
        hz_rate = hz_rate / 10 * 10;  // 10 is used to cal
    }

    stream_info->in_rate = hz_rate;
}

static hi_void inst_check_hdr_scenes_change(vpss_stream_info *stream_info,
    hi_drv_vpss_video_frame *src_image, hi_bool *new_scene)
{
    if (stream_info->color_sys != src_image->comm_frame.video_private.color_sys) {
        *new_scene = HI_TRUE;
    } else if (stream_info->src_hdr_type != src_image->comm_frame.hdr_type) {
        *new_scene = HI_TRUE;
    } else if ((osal_memncmp(&stream_info->src_color_desp, sizeof(hi_drv_color_descript),
                             &src_image->comm_frame.color_space, sizeof(hi_drv_color_descript))) != 0) {
        *new_scene = HI_TRUE;
    }

    return;
}

hi_void vpss_inst_check_scenes_change(vpss_stream_info *stream_info, hi_drv_vpss_video_frame *src_image)
{
    hi_bool special_field = HI_FALSE;
    hi_bool interlace = HI_FALSE;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;
    hi_bool field_mode_same = HI_FALSE;

    field_mode_same = (((stream_info->field_mode == HI_DRV_FIELD_ALL) &&
                        (src_image->comm_frame.field_mode != HI_DRV_FIELD_ALL)) ||
                       ((stream_info->field_mode != HI_DRV_FIELD_ALL) &&
                        (src_image->comm_frame.field_mode == HI_DRV_FIELD_ALL)));

    special_field = vpss_policy_check_special_field(src_image);

    if (src_image->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_INTERLACE) {
        interlace = HI_TRUE;
    } else if (src_image->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_PROGRESSIVE) {
        interlace = HI_FALSE;
    } else {
        interlace = HI_FALSE;
        vpss_error("UNKOWN sample_type:%d;\n", src_image->comm_frame.sample_type);
    }

    if (stream_info->secure != src_image->comm_frame.secure) {
        stream_info->new_scene = HI_TRUE;
    } else if (stream_info->interlace != interlace) {
        stream_info->new_scene = HI_TRUE;
    } else if (stream_info->height != src_image->comm_frame.height) {
        stream_info->new_scene = HI_TRUE;
    } else if (stream_info->width != src_image->comm_frame.width) {
        stream_info->new_scene = HI_TRUE;
    } else if (stream_info->pixel_format != src_image->comm_frame.pixel_format) {
        stream_info->new_scene = HI_TRUE;
    } else if (stream_info->cmp_info.cmp_mode != src_image->comm_frame.cmp_info.cmp_mode) {
        stream_info->new_scene = HI_TRUE;
    } else if (stream_info->cmp_info.data_fmt != src_image->comm_frame.cmp_info.data_fmt) {
        stream_info->new_scene = HI_TRUE;
    } else if (stream_info->video_3d_type != src_image->comm_frame.video_3d_type) {
        stream_info->new_scene = HI_TRUE;
    } else if (field_mode_same == HI_TRUE) {
        stream_info->new_scene = HI_TRUE;
    } else if (stream_info->special_field != special_field) {
        stream_info->new_scene = HI_TRUE;
    } else {
        stream_info->new_scene = HI_FALSE;
    }

    inst_check_hdr_scenes_change(stream_info, src_image, &stream_info->new_scene);

    meta_info = (hi_drv_win_vpss_meta_info *)src_image->comm_frame.video_private.win_private.data;
    if (stream_info->vpss_process != meta_info->vpss_process) {
        stream_info->new_scene = HI_TRUE;
    }

    return;
}

hi_s32 vpss_inst_get_frame_from_in_buffer(vpss_instance *instance, hi_drv_vpss_video_frame *src_image)
{
    hi_s32 ret;
    fn_vpss_src_priv_func fn_acq_in_frame;

    fn_acq_in_frame = instance->src_funcs.vpss_acq_in_priv_image;
    if (fn_acq_in_frame == HI_NULL) {
        vpss_error("VPSS_GET_SRCIMAGE doesn't exit.\n");
        return HI_FAILURE;
    }

    instance->perfor.buf_dfx.buf_acq_time_start = vpss_comm_get_sched_time();

    ret = fn_acq_in_frame(instance->id, src_image);
    if (ret == HI_SUCCESS) {
        if ((instance->global_dbg_ctrl->set_src_width != 0) || (instance->global_dbg_ctrl->set_src_height != 0)) {
            if ((src_image->comm_frame.width < instance->global_dbg_ctrl->set_src_width) ||
                (src_image->comm_frame.height < instance->global_dbg_ctrl->set_src_height)) {
                vpss_error("set src reso is not valid! src:(w:%d h:%d) set:(w:%d h:%d) \n",
                           src_image->comm_frame.width,
                           src_image->comm_frame.height,
                           instance->global_dbg_ctrl->set_src_width,
                           instance->global_dbg_ctrl->set_src_height);
            }
            src_image->comm_frame.width = instance->global_dbg_ctrl->set_src_width;
            src_image->comm_frame.height = instance->global_dbg_ctrl->set_src_height;
        }
    }

    instance->perfor.buf_dfx.buf_acq_time_end = vpss_comm_get_sched_time();

    return ret;
}

hi_void vpss_inst_fill_instance_info_from_port(vpss_instance *instance)
{
    if (instance->port[0].port_id != HI_INVALID_HANDLE) {
        instance->user_crop_en = instance->port[0].user_crop_en;
        instance->src_crop_rect = instance->port[0].crop_rect;
        instance->need_src_flip = instance->port[0].vert_flip;
        instance->offset_rect = instance->port[0].offset_rect;
    }
}

static hi_void inst_correct_8k_port_rota(hi_drv_vpss_video_frame *src_frame, vpss_port *port)
{
    if (((src_frame->comm_frame.width > VPSS_WIDTH_UHD) || (src_frame->comm_frame.height > VPSS_HEIGHT_UHD)) &&
        (port->rotation != HI_DRV_VPSS_ROTATION_DISABLE)) {
        port->rotation = HI_DRV_VPSS_ROTATION_DISABLE;
    }

    return;
}

hi_s32 vpss_inst_sync_port_cfg_by_frame(hi_drv_vpss_video_frame *src_frame, vpss_instance *instance)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 count;
    vpss_port *port = HI_NULL;
    hi_drv_vpss_port_frame_cfg port_frame_cfg;
    hi_drv_video_frame *comm_image = HI_NULL;
    hi_bool support_hdr = vpss_policy_support_hdr();

    comm_image = (hi_drv_video_frame *)vpss_kmalloc(sizeof(hi_drv_video_frame), OSAL_GFP_KERNEL);
    if (comm_image == HI_NULL) {
        vpss_error("priv_frame malloc failed! \n");
        return HI_FAILURE;
    }
    memset(comm_image, 0, sizeof(hi_drv_video_frame));

    for (count = 0; count < VPSS_PORT_MAX_NUM; count++) {
        port = &(instance->port[count]);
        if ((port->port_id == HI_INVALID_HANDLE) || (port->call_back_func.get_port_cfg_by_frame == HI_NULL)) {
            continue;
        }

        vpss_comm_cvt_privite_to_comm_nomap(src_frame, comm_image);

        ret = port->call_back_func.get_port_cfg_by_frame(port->h_dst_module, comm_image, &port_frame_cfg);
        if (ret != HI_SUCCESS) {
            vpss_error("get port cfg by frame failed ret %d\n", ret);
            break;
        }

        /* x, y sets for  multi-region location */
        src_frame->comm_frame.disp_rect.rect_x = comm_image->disp_rect.rect_x;
        src_frame->comm_frame.disp_rect.rect_y = comm_image->disp_rect.rect_y;

        ret = vpss_inst_check_frame_port_cfg(src_frame, &port_frame_cfg);
        if (ret != HI_SUCCESS) {
            vpss_error("get port cfg by frame failed ret %d\n", ret);
            break;
        }

        port->user_crop_en = port_frame_cfg.user_crop_en;
        port->offset_rect = port_frame_cfg.in_offset_rect;
        port->crop_rect = port_frame_cfg.crop_rect;
        port->video_rect = port_frame_cfg.video_rect;
        port->out_rect = port_frame_cfg.out_rect;
        port->format = port_frame_cfg.format;
        port->cmp_info = port_frame_cfg.cmp_info;
        port->bit_width = port_frame_cfg.bit_width;
        if (instance->global_dbg_ctrl->set_rota_en[count] == HI_TRUE) {
            port->rotation = instance->global_dbg_ctrl->rotation[count];
        } else {
            port->rotation = port_frame_cfg.rotation;
        }
        inst_correct_8k_port_rota(src_frame, port);

        port->hori_flip = port_frame_cfg.horizontal_flip;
        port->vert_flip = port_frame_cfg.vertical_flip;
        port->hdr_follow_src = port_frame_cfg.hdr_follow_src;
        if ((port->hdr_follow_src == HI_TRUE) || (support_hdr != HI_TRUE)) {
            port->out_hdr_type = comm_image->hdr_type;
            port->out_color_desp = comm_image->color_space;
        } else {
            port->out_hdr_type = port_frame_cfg.out_hdr_type;
            port->out_color_desp = port_frame_cfg.out_color_desp;
        }
    }

    vpss_kfree(comm_image);

    return ret;
}

hi_s32 vpss_inst_put_frame(vpss_instance *instance, hi_drv_vpss_video_frame *src_frame,
                           vpss_stream_info *stream_info, vpss_src *src_list)
{
    hi_s32 ret;

    if (stream_info->new_scene == HI_TRUE) {
        instance->perfor.cnt_dfx.scence_chg_cnt++;
        hi_drv_stat_event(HI_STAT_EVENT_VPSS_GET_FRM, 0);

        if ((instance->quick_out == HI_TRUE) ||
            ((src_frame->comm_frame.video_private.source != HI_DRV_SOURCE_DTV) &&
             (src_frame->comm_frame.video_private.source != HI_DRV_SOURCE_MEDIA) &&
             (src_frame->comm_frame.video_private.source != HI_DRV_SOURCE_NETWORK))) {
            /* because vpss process first frame,will create all buffer,avoid vicap lost frame */
            if (instance->perfor.cnt_dfx.scence_chg_cnt == 1) {
                hi_drv_vpss_video_frame frame = { 0 };
                hi_u32 vi_buffer_count = 0;
                hi_u32 max_vi_buffer_number = 20;
                ret = vpss_inst_get_frame_from_in_buffer(instance, &frame);

                while (ret == HI_SUCCESS) {
                    vi_buffer_count++;

                    instance->src_funcs.vpss_rel_in_priv_image(instance->id, src_frame);

                    memcpy(src_frame, &frame, sizeof(hi_drv_vpss_video_frame));

                    /* 这里是否需要将metadata copy出去 */
                    vpss_inst_fill_instance_info_from_port(instance);
                    vpss_policy_revise_image(&instance->src_crop_rect, instance->user_crop_en,
                                             &instance->offset_rect, src_frame);
                    ret = vpss_inst_get_frame_from_in_buffer(instance, &frame);

                    if (vi_buffer_count > max_vi_buffer_number) {
                        break;
                    }
                }
            }
        }

        vpss_inst_update_stream_info(src_frame, stream_info);
        ret = vpss_inst_create_all_buffer(instance, src_frame);
        if (ret != HI_SUCCESS) {
            vpss_inst_desroy_alg_buffer(instance);
            instance->src_funcs.vpss_rel_in_priv_image(instance->id, src_frame);
            return HI_FAILURE;
        }

        return vpss_src_put_image(src_list, src_frame);
    } else {
        if (stream_info->real_topfirst != src_frame->comm_frame.top_field_first) {
            src_frame->comm_frame.top_field_first = stream_info->real_topfirst;
        }

        return vpss_src_put_image(src_list, src_frame);
    }
}

hi_s32 vpss_inst_check_add_process_frame(vpss_instance *instance)
{
    hi_s32 ret;
    hi_drv_vpss_video_frame *src_frame = &instance->rev_new_frame;
    vpss_stream_info *stream_info = &instance->stream_info;
    vpss_src *src_list = &instance->src_list;
    vpss_debug_info *dbg_info = instance->global_dbg_ctrl;
    hi_bool need_trans = HI_FALSE;
    hi_bool rota_alg_en = HI_FALSE;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;

    instance->perfor.img_dfx.get_try_cnt++;
    ret = vpss_inst_get_frame_from_in_buffer(instance, src_frame);
    if (ret != HI_SUCCESS) {
        vpss_dbg("----VPSS %d get process frame fail \n", instance->id);
        return HI_FAILURE;
    }

    meta_info = (hi_drv_win_vpss_meta_info *)src_frame->comm_frame.video_private.win_private.data;

    instance->perfor.img_dfx.get_suc_cnt++;
    stream_info->orignal_frame = instance->rev_new_frame;
    vpss_dbg_print_frame(dbg_info, src_frame, DEF_DBG_SRC_ID);
    vpss_dbg_save_frame(dbg_info, src_frame, DEF_DBG_SRC_ID);

    if (src_frame->comm_frame.video_private.last_flag == HI_DRV_LAST_ERROR_FLAG) {
        instance->src_funcs.vpss_rel_in_priv_image(instance->id, src_frame);
        vpss_inst_flush(instance);
        return HI_FAILURE;
    }

    vpss_comm_notify_event(src_frame, HI_STAT_LD_EVENT_VPSS_FRM_IN);

    ret = vpss_inst_sync_port_cfg_by_frame(src_frame, instance);
    if (ret != HI_SUCCESS) {
        vpss_error("sync frame port faile, ret = %d \n", ret);
        vpss_inst_desroy_alg_buffer(instance);
        vpss_inst_update_stream_info(src_frame, stream_info);
        instance->src_funcs.vpss_rel_in_priv_image(instance->id, src_frame);
        return HI_FAILURE;
    }

    need_trans = vpss_policy_check_need_trans_withport(src_frame, instance);
    if (need_trans == HI_TRUE) {
        if (meta_info->vpss_process != HI_TRUE && stream_info->vpss_process == HI_TRUE) {
            vpss_inst_desroy_alg_buffer(instance);
            stream_info->vpss_process = HI_FALSE;
        }

        vpss_inst_update_stream_info(src_frame, stream_info);
        vpss_inst_process_trans_frame(instance, src_frame);
        // return failuer,because vpss logic can't work
        return HI_FAILURE;
    }

    vpss_inst_fill_instance_info_from_port(instance);
    vpss_inst_correct_prog_info(instance, src_frame);
    vpss_policy_revise_image(&instance->src_crop_rect, instance->user_crop_en, &instance->offset_rect, src_frame);

    rota_alg_en = vpss_policy_support_rota_alg(dbg_info, src_frame->comm_frame.width, src_frame->comm_frame.height);
    if ((instance->port[0].rotation != HI_DRV_VPSS_ROTATION_DISABLE) && (rota_alg_en != HI_TRUE)) {
        vpss_policy_revise_tran_frame_bitwidth(src_frame);
    }

    vpss_inst_change_in_rate(stream_info, src_frame->comm_frame.frame_rate);
    vpss_inst_debug_ctr(instance, src_frame);
    vpss_inst_check_scenes_change(stream_info, src_frame);
    vpss_inst_update_stream_info(src_frame, stream_info);

    instance->use_pq_module = vpss_policy_check_use_pq(src_frame, instance->global_dbg_ctrl);
    ret = vpss_inst_put_frame(instance, src_frame, stream_info, src_list);

    return ret;
}

hi_s32 vpss_inst_check_process_frame(vpss_instance *instance)
{
    hi_s32 ret;
    vpss_src_data *image = HI_NULL;
    unsigned long flags;

    vpss_comm_down_spin(&(instance->src_spin), &flags);
    ret = vpss_src_get_process_image(&instance->src_list, &image);
    vpss_comm_up_spin(&(instance->src_spin), &flags);
    if (ret == HI_SUCCESS) {
        return HI_SUCCESS;
    }

    return vpss_inst_check_add_process_frame(instance);
}

hi_void vpss_inst_sync_usr_cfg(vpss_instance *instance)
{
    unsigned long flags;
    hi_drv_vpss_instance_cfg *inst_usrc_cfg = HI_NULL;
    hi_u32 count;
    vpss_port *port = HI_NULL;
    hi_drv_vpss_port_cfg *port_cfg = HI_NULL;

    vpss_comm_down_spin(&(instance->usr_set_spin), &flags);
    if (instance->cfg_new) {
        inst_usrc_cfg = &(instance->usr_inst_cfg);
        instance->quick_out = inst_usrc_cfg->quick_enable;
        instance->prog_detect = inst_usrc_cfg->prog_detect;
        instance->work_mode = inst_usrc_cfg->work_mode;
        instance->pq_mode = inst_usrc_cfg->pq_mode;

        for (count = 0; count < VPSS_PORT_MAX_NUM; count++) {
            port = &(instance->port[count]);

            if (port->port_id != HI_INVALID_HANDLE) {
                port_cfg = &(instance->usr_port_cfg[count]);
                port->port_type = port_cfg->port_type;
                port->vpss_work_mode = port_cfg->bypass_mode;
                port->user_crop_en = port_cfg->user_crop_en;
                port->offset_rect = port_cfg->in_offset_rect;
                port->crop_rect = port_cfg->crop_rect;
                port->video_rect = port_cfg->video_rect;
                port->out_rect = port_cfg->out_rect;
                port->tunnel_enable = instance->quick_out;
                port->enable_3d = port_cfg->enable_3d;
                port->format = port_cfg->format;
                port->cmp_info = port_cfg->cmp_info;
                port->bit_width = port_cfg->bit_width;
                port->buf_type = port_cfg->buf_cfg.buf_type;
                port->rotation = port_cfg->rotation;
                port->hori_flip = port_cfg->horizontal_flip;
                port->vert_flip = port_cfg->vertical_flip;

                if (port->rotation == HI_DRV_VPSS_ROTATION_180) {
                    port->hori_flip = !port->hori_flip;
                    port->vert_flip = !port->vert_flip;
                }

                port->ori_frame_rate = port_cfg->ori_frame_rate;
                port->max_frame_rate = port_cfg->max_frame_rate;
                port->hdr_follow_src = port_cfg->hdr_follow_src;
                port->out_hdr_type = port_cfg->out_hdr_type;
                port->out_color_desp = port_cfg->out_color_desp;

                port->h_dst_module = port->sink_func.h_sink;
                port->call_back_func = port->sink_func.dst_func;
            }
        }

        instance->cfg_new = HI_FALSE;
    }
    vpss_comm_up_spin(&(instance->usr_set_spin), &flags);
}

hi_void get_rotation_rect(vpss_port *port, hi_drv_rect *video_rect_before_rota, hi_drv_rect *out_rect_before_rota)
{
    if ((port->rotation != HI_DRV_VPSS_ROTATION_90) && (port->rotation != HI_DRV_VPSS_ROTATION_270)) {
        memcpy(video_rect_before_rota, &port->revise_video_rect, sizeof(hi_drv_rect));
        memcpy(out_rect_before_rota, &port->revise_out_rect, sizeof(hi_drv_rect));
        return;
    }

    if (port->rotation == HI_DRV_VPSS_ROTATION_90) {
        video_rect_before_rota->rect_y = port->revise_out_rect.rect_w - port->revise_video_rect.rect_w
            - port->revise_video_rect.rect_x;
        video_rect_before_rota->rect_x = port->revise_video_rect.rect_y;
    }

    if (port->rotation == HI_DRV_VPSS_ROTATION_270) {
        video_rect_before_rota->rect_x = port->revise_out_rect.rect_h - port->revise_video_rect.rect_h
            - port->revise_video_rect.rect_y;
        video_rect_before_rota->rect_y = port->revise_video_rect.rect_x;
    }

    video_rect_before_rota->rect_h = port->revise_video_rect.rect_w;
    video_rect_before_rota->rect_w = port->revise_video_rect.rect_h;
    out_rect_before_rota->rect_h = port->revise_out_rect.rect_w;
    out_rect_before_rota->rect_w = port->revise_out_rect.rect_h;
}

hi_void get_flip_rect(vpss_port *port, hi_drv_rect *video_rect_before_rota,
    hi_drv_rect *out_rect_before_rota, hi_drv_rect *video_rect_before_flip)
{
    if ((port->hori_flip != HI_TRUE) && (port->vert_flip != HI_TRUE)) {
        return;
    }

    if (port->hori_flip == HI_TRUE && port->vert_flip == HI_TRUE) {
        video_rect_before_flip->rect_x = out_rect_before_rota->rect_w - video_rect_before_rota->rect_x
            - video_rect_before_rota->rect_w;
        video_rect_before_flip->rect_y = out_rect_before_rota->rect_h - video_rect_before_rota->rect_y
            - video_rect_before_rota->rect_h;
    } else if (port->hori_flip == HI_TRUE) {
        video_rect_before_flip->rect_x = out_rect_before_rota->rect_w - video_rect_before_rota->rect_x
            - video_rect_before_rota->rect_w;
        video_rect_before_flip->rect_y = video_rect_before_rota->rect_y;
    } else if (port->vert_flip == HI_TRUE) {
        video_rect_before_flip->rect_x = video_rect_before_rota->rect_x;
        video_rect_before_flip->rect_y = out_rect_before_rota->rect_h - video_rect_before_rota->rect_y
            - video_rect_before_rota->rect_h;
    }
}

hi_void correct_port_rect(vpss_port *port, vpss_stream_info *stream_info)
{
    hi_drv_rect real_crop_rect;
    hi_drv_rect stream_reso;
    hi_drv_rect video_rect_before_rota;
    hi_drv_rect out_rect_before_rota;
    hi_drv_rect video_rect_before_flip;

    hi_bool support_lbx = vpss_policy_support_lbx();
    hi_bool support_zme = vpss_policy_support_out0_zme();

    stream_reso.rect_w = stream_info->width;
    stream_reso.rect_h = stream_info->height;

    vpss_policy_correct_port_rect(&stream_reso, port->user_crop_en,
        &port->crop_rect, &port->offset_rect, &real_crop_rect);

    memcpy(&port->revise_crop_rect, &real_crop_rect, sizeof(hi_drv_rect));
    memcpy(&port->revise_video_rect, &port->video_rect, sizeof(hi_drv_rect));
    memcpy(&port->revise_out_rect, &port->out_rect, sizeof(hi_drv_rect));

    if ((port->revise_out_rect.rect_w == 0) || (port->revise_out_rect.rect_h == 0)) {
        port->revise_out_rect.rect_w = real_crop_rect.rect_w;
        port->revise_out_rect.rect_h = real_crop_rect.rect_h;
    }

    if (support_zme != HI_TRUE && support_lbx != HI_TRUE) {
        port->revise_video_rect.rect_x = 0;
        port->revise_video_rect.rect_y = 0;
        port->revise_video_rect.rect_w = real_crop_rect.rect_w;
        port->revise_video_rect.rect_h = real_crop_rect.rect_h;
        port->revise_out_rect.rect_w = real_crop_rect.rect_w;
        port->revise_out_rect.rect_h = real_crop_rect.rect_h;
    }

    if ((port->revise_video_rect.rect_w == 0) || (port->revise_video_rect.rect_h == 0)) {
        port->revise_video_rect.rect_x = 0;
        port->revise_video_rect.rect_y = 0;
        port->revise_video_rect.rect_w = port->revise_out_rect.rect_w;
        port->revise_video_rect.rect_h = port->revise_out_rect.rect_h;
    }

    if (port->port_type == HI_DRV_VPSS_PORT_TYPE_3D_DETECT) {
        if (stream_info->orignal_frame.comm_frame.width < VPSS_DETECTPORT_WIDTH) {
            port->revise_out_rect.rect_w = stream_info->orignal_frame.comm_frame.width;
        }

        if (stream_info->orignal_frame.comm_frame.height < VPSS_DETECTPORT_HEIGHT) {
            port->revise_out_rect.rect_h = stream_info->orignal_frame.comm_frame.height;
        }
    }

    if ((port->revise_video_rect.rect_x + port->revise_video_rect.rect_w > port->revise_out_rect.rect_w) ||
        (port->revise_video_rect.rect_y + port->revise_video_rect.rect_h > port->revise_out_rect.rect_h)) {
        port->revise_video_rect.rect_x = 0;
        port->revise_video_rect.rect_y = 0;
        port->revise_video_rect.rect_w = port->revise_out_rect.rect_w;
        port->revise_video_rect.rect_h = port->revise_out_rect.rect_h;
    }

    if (port->port_type == HI_DRV_VPSS_PORT_TYPE_3D_DETECT) {
        return;
    }

    get_rotation_rect(port, &video_rect_before_rota, &out_rect_before_rota);
    memcpy(&video_rect_before_flip, &video_rect_before_rota, sizeof(hi_drv_rect));

    get_flip_rect(port, &video_rect_before_rota, &out_rect_before_rota, &video_rect_before_flip);
    memcpy(&port->revise_video_rect, &video_rect_before_flip, sizeof(hi_drv_rect));

#ifdef VPSS_LBX_DBG
    vpss_error("set crop:%d %d %d %d; video:%d %d %d %d; out:%d %d %d %d \n",
               port->crop_rect.rect_x, port->crop_rect.rect_y,
               port->crop_rect.rect_w, port->crop_rect.rect_h,
               port->video_rect.rect_x, port->video_rect.rect_y,
               port->video_rect.rect_w, port->video_rect.rect_h,
               port->out_rect.rect_x, port->out_rect.rect_y,
               port->out_rect.rect_w, port->out_rect.rect_h);

    vpss_error("revise crop:%d %d %d %d; video:%d %d %d %d; out:%d %d %d %d \n",
               port->revise_crop_rect.rect_x, port->revise_crop_rect.rect_y,
               port->revise_crop_rect.rect_w, port->revise_crop_rect.rect_h,
               port->revise_video_rect.rect_x, port->revise_video_rect.rect_y,
               port->revise_video_rect.rect_w, port->revise_video_rect.rect_h,
               port->revise_out_rect.rect_x, port->revise_out_rect.rect_y,
               port->revise_out_rect.rect_w, port->revise_out_rect.rect_h);
#endif

    return;
}

hi_void correct_port_rotation(vpss_port *port, hi_drv_3d_type stream_frm_type)
{
    if (stream_frm_type != HI_DRV_3D_NONE) {
        port->rotation = HI_DRV_VPSS_ROTATION_DISABLE;
    }

    return;
}

hi_void correct_port_format(vpss_port *port, hi_drv_pixel_format pixel_format)
{
    hi_bool support_zme_upsamp = vpss_policy_support_zme_upsamp();
    if (support_zme_upsamp == HI_FALSE) {
        port->format = vpss_policy_get_port_out_fmt(pixel_format, port->format);
    }

    if ((port->rotation == HI_DRV_VPSS_ROTATION_90) ||
        (port->rotation == HI_DRV_VPSS_ROTATION_270) ||
        (port->rotation == HI_DRV_VPSS_ROTATION_180)) {
        if ((port->format == HI_DRV_PIXEL_FMT_NV16) || (port->format == HI_DRV_PIXEL_FMT_NV16_2X1)) {
            port->format = HI_DRV_PIXEL_FMT_NV12;
        }

        if ((port->format == HI_DRV_PIXEL_FMT_NV61) || (port->format == HI_DRV_PIXEL_FMT_NV61_2X1)) {
            port->format = HI_DRV_PIXEL_FMT_NV21;
        }
    }

    vpss_policy_revise_out_frame_info(&port->format, &port->cmp_info);
    return;
}

hi_void correct_3d_detect_port(vpss_port *port)
{
    hi_bool support_3d_detect = vpss_policy_support_logic_3d_detect_port();

    if (port->port_type == HI_DRV_VPSS_PORT_TYPE_3D_DETECT) {
        if (support_3d_detect != HI_TRUE) {
            vpss_error("3D detect port can't support tunnel; support_3d_detect:%d \n", support_3d_detect);
            port->enble = HI_FALSE;
        }

        if (port->tunnel_enable == HI_TRUE) {
            vpss_error("3D detect port can't support tunnel; port_id:%d \n", port->port_id);
            port->tunnel_enable = HI_FALSE;
        }
    }

    return;
}

hi_void correct_port_bit_width(vpss_port *port)
{
#ifndef HI_VPSS_DRV_USE_GOLDEN
    hi_bool fix_out_width10 = vpss_policy_support_fix_out_bitwidth10();
    /* dpt default 10bit out, stb donot reviese, use setting value */
    if (fix_out_width10 == HI_TRUE) {
        if (port->port_type != HI_DRV_VPSS_PORT_TYPE_3D_DETECT) {
            port->bit_width = HI_DRV_PIXEL_BITWIDTH_10BIT;
        } else {
            port->bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
        }
    }
#endif
    return;
}

hi_void vpss_inst_correct_port_config(vpss_instance *instance)
{
    hi_u32 count;
    vpss_port *port = HI_NULL;
    vpss_stream_info *stream_info = HI_NULL;
    stream_info = &instance->stream_info;

    for (count = 0; count < VPSS_PORT_MAX_NUM; count++) {
        port = &(instance->port[count]);

        if (port->port_id == HI_INVALID_HANDLE) {
            continue;
        }

        correct_port_rotation(port, stream_info->video_3d_type);
        correct_port_rect(port, stream_info);
        correct_port_format(port, stream_info->pixel_format);
        correct_3d_detect_port(port);
        correct_port_bit_width(port);
    }
}

hi_bool vpss_inst_check_vaild_port(vpss_instance *instance)
{
    hi_u32 count;
    hi_bool has_enable_port = HI_FALSE;

    for (count = 0; count < VPSS_PORT_MAX_NUM; count++) {
        if ((instance->port[count].port_id != HI_INVALID_HANDLE) && (instance->port[count].enble == HI_TRUE)) {
            has_enable_port = HI_TRUE;
            break;
        }
    }

    if (has_enable_port != HI_TRUE) {
        return HI_FALSE;
    }

    return HI_TRUE;
}

hi_bool vpss_inst_check_in_out_buff(vpss_instance *instance,
    hi_s32 has_buffer_port_count, vpss_frame_status *frame_status)
{
    if ((instance->port[0].port_id != HI_INVALID_HANDLE) && (instance->port[0].enble == HI_TRUE)) {
        if (vpss_out_check_empty_buf(&instance->port[0].out_list) == HI_TRUE) {
            has_buffer_port_count++;
        }
    }

    if (has_buffer_port_count == 0) {
        vpss_dbg("inst %d OUT buf is FULL.\n", instance->id);
        frame_status->vpss_frame_back = HI_FALSE;
        return HI_FALSE;
    } else {
        instance->perfor.buf_dfx.try_suc_cnt++;
    }

    if (vpss_inst_check_process_frame(instance) != HI_SUCCESS) { /* check in buffer */
        frame_status->vpss_frame_arrival = HI_FALSE;
        return HI_FALSE;
    }

    return HI_TRUE;
}

hi_void vpss_inst_process_deque_frame_from_vdp(vpss_instance *instance, hi_drv_video_frame *frame, vpss_out *out_list)
{
    hi_bool trans_frame = HI_FALSE;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;
    hi_drv_vpss_video_frame *priv_image = HI_NULL;

    priv_image = (hi_drv_vpss_video_frame *)vpss_kmalloc(sizeof(hi_drv_vpss_video_frame), OSAL_GFP_KERNEL);
    if (priv_image == HI_NULL) {
        vpss_error("priv_frame malloc failed! \n");
        return;
    }
    memset(priv_image, 0, sizeof(hi_drv_vpss_video_frame));

    vpss_comm_cvt_comm_to_privite_nomap(frame, priv_image);

    vpss_dbg_print_frame_addr(instance->global_dbg_ctrl, frame, VPSS_OUT_DEQUEUE_FRAME_TYPE);
    instance->perfor.out_dfx.dequeue_suc_cnt++;

    vpss_out_rel_ful_frm(out_list, frame);

    meta_info = (hi_drv_win_vpss_meta_info *)frame->video_private.vpss_private.data;
    trans_frame = !meta_info->vpss_process;
    if (meta_info->vpss_process != HI_TRUE) {
        if (trans_frame == HI_TRUE) {
            if (meta_info->vdp_advance_frame == HI_TRUE) {
                vpss_inst_tran_reference_count(instance, priv_image);
            }
        }

        instance->perfor.out_dfx.release_trans_cnt++;
        vpss_comm_copy_priv_vpss_meta_to_win_meta(priv_image);
        instance->src_funcs.vpss_rel_in_priv_image(instance->id, priv_image);
    } else {
        vpss_inst_last_tran_refrence_cnt(instance);
    }

    vpss_kfree(priv_image);
}

static hi_void vpss_inst_get_allport_frame_from_vdp(vpss_instance *instance)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 count;
    vpss_out *out_list = HI_NULL;
    vpss_port *port = HI_NULL;
    hi_drv_video_frame *frame = HI_NULL;

    frame = (hi_drv_video_frame *)vpss_kmalloc(sizeof(hi_drv_video_frame), OSAL_GFP_KERNEL);
    if (frame == HI_NULL) {
        vpss_error("comm_frame malloc failed! \n");
        return;
    }
    memset(frame, 0, sizeof(hi_drv_video_frame));

    instance->perfor.buf_dfx.dq_time_start = vpss_comm_get_sched_time();
    for (count = 0; count < VPSS_PORT_MAX_NUM; count++) { /* check if port has write space, only one port */
        port = &instance->port[count];
        if ((port->port_id == HI_INVALID_HANDLE) || (instance->port[count].enble != HI_TRUE) ||
            (port->call_back_func.sink_dequeue_frame == HI_NULL)) {
            continue;
        }

        out_list = &port->out_list;

        while (ret == HI_SUCCESS) {
            instance->perfor.out_dfx.dequeue_cnt++;
            ret = port->call_back_func.sink_dequeue_frame(port->h_dst_module, frame);
            if (ret == HI_SUCCESS) {
                vpss_inst_process_deque_frame_from_vdp(instance, frame, out_list);
            }
        }

        ret = HI_SUCCESS;
    }
    instance->perfor.buf_dfx.dq_time_end = vpss_comm_get_sched_time();

    vpss_kfree(frame);

    return;
}

hi_bool vpss_inst_check_inst_available(vpss_instance *instance, vpss_frame_status *frame_status)
{
    hi_s32 has_buffer_port_count = 0;
    hi_bool has_enable_port = HI_FALSE;
    hi_bool has_in_out_buff = HI_FALSE;

    /* check enable; check idle;check out buffer;check in buffer */
    frame_status->vpss_frame_arrival = HI_TRUE;
    frame_status->vpss_frame_back = HI_TRUE;

    instance->perfor.check_dfx.try_cnt++;
    if ((instance->suspend == HI_TRUE) || (instance->enable == HI_FALSE) || (instance->state != INSTANCE_STATE_IDLE)) {
        return HI_FALSE;
    }

    vpss_inst_sync_usr_cfg(instance);
    has_enable_port = vpss_inst_check_vaild_port(instance);
    if (has_enable_port != HI_TRUE) {
        return HI_FALSE;
    }

    instance->perfor.buf_dfx.try_cnt++;

    vpss_inst_get_allport_frame_from_vdp(instance);

    has_in_out_buff = vpss_inst_check_in_out_buff(instance, has_buffer_port_count, frame_status);
    if (has_in_out_buff != HI_TRUE) {
        return HI_FALSE;
    }

    vpss_inst_correct_port_config(instance);
    instance->perfor.check_dfx.try_suc_cnt++;

    return HI_TRUE;
}

hi_s32 vpss_inst_prepare_task(vpss_instance *instance)
{
    hi_u32 port_id;
    hi_u32 valid_port_number;
    vpss_port *port = HI_NULL;
    vpss_out *out_list = HI_NULL;
    vpss_out_frame_info out_frame_attr = { 0 };

    valid_port_number = 0;
    instance->state = INSTANCE_STATE_PREPARE;

    /***********get in/out buffer************************/
    for (port_id = 0; port_id < VPSS_PORT_MAX_NUM; port_id++) {
        instance->out_frm_node[port_id] = HI_NULL;
        port = &instance->port[port_id];
        if (port->port_id == HI_INVALID_HANDLE) {
            continue;
        }

        if (port->enble != HI_TRUE) {
            continue;
        }

        out_list = &port->out_list;
        get_out_frame_info(instance, port_id, &out_frame_attr);
        instance->out_frm_node[port_id] = vpss_out_get_empty_buf(out_list, &out_frame_attr);
        if (instance->out_frm_node[port_id] == HI_NULL) {
            vpss_dbg("vpss port %d has no empty buffer", port->port_id);
            continue;
        }

        valid_port_number++;
    }

    if (valid_port_number == 0) {
        vpss_error("vpss port has no empty buffer");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vpss_inst_config_2d_task(vpss_instance *instance)
{
    hi_s32 ret;
    vpss_ip ip;
    vpss_hal_info *hal_info = HI_NULL;
    hi_bool rota_alg_en = HI_FALSE;
    hi_bool support_virtual_port = HI_FALSE;
    hi_bool virt_port_start = HI_FALSE;

    ip = instance->ctrl_id;
    hal_info = &instance->hal_info;
    instance->perfor.config_dfx.fill_hal_time_start = vpss_comm_get_sched_time();
    ret = vpss_inst_fill_hal_info(instance, HI_DRV_3D_EYE_LEFT);
    if (ret != HI_SUCCESS) {
        vpss_warn("call vpss_ctrl_fix_task error!!!\n");
        return HI_FAILURE;
    }
    instance->perfor.config_dfx.fill_hal_time_end = vpss_comm_get_sched_time();

    if (instance->global_dbg_ctrl->rot_alg_width != 0 || instance->global_dbg_ctrl->rot_alg_height != 0) {
        instance->dbg_ctrl.rot_alg_width = instance->global_dbg_ctrl->rot_alg_width;
        instance->dbg_ctrl.rot_alg_height = instance->global_dbg_ctrl->rot_alg_height;
    }

    instance->perfor.config_dfx.set_node_time_start = vpss_comm_get_sched_time();
    rota_alg_en = vpss_policy_support_rota_alg(&instance->dbg_ctrl, hal_info->in_info.rect_w, hal_info->in_info.rect_h);
    if ((rota_alg_en == HI_TRUE) || (instance->port[0].rotation == HI_DRV_VPSS_ROTATION_DISABLE)) {
        ret = vpss_hal_set_node_info(ip, hal_info);
        if (ret != HI_SUCCESS) {
            vpss_error("call vpss_hal_set_node_info error!!!\n");
            return HI_FAILURE;
        }
    }
    instance->perfor.config_dfx.set_node_time_end = vpss_comm_get_sched_time();

    instance->perfor.config_dfx.vir_node_time_start = vpss_comm_get_sched_time();
    support_virtual_port = vpss_policy_check_support_virtual_port();
    virt_port_start = vpss_inst_check_virtual_start(instance);
    if ((support_virtual_port == HI_TRUE) && (virt_port_start == HI_TRUE)) {
        ret = vpss_hal_set_node_info(ip, hal_info);
        if (ret != HI_SUCCESS) {
            vpss_error("call vpss_hal_set_node_info error!!!\n");
            return HI_FAILURE;
        }
    }
    instance->perfor.config_dfx.vir_node_time_end = vpss_comm_get_sched_time();

    return HI_SUCCESS;
}
hi_s32 vpss_inst_config_3d_task(vpss_instance *instance)
{
    hi_s32 ret;
    vpss_ip ip;
    vpss_hal_info *hal_info;

    ip = instance->ctrl_id;
    hal_info = &instance->hal_info;
    ret = vpss_inst_fill_hal_info(instance, HI_DRV_3D_EYE_RIGHT);
    if (ret != HI_SUCCESS) {
        vpss_warn("call vpss_ctrl_fix_task error!!!\n");
        return HI_FAILURE;
    }

    ret = vpss_hal_set_node_info(ip, hal_info);
    if (ret != HI_SUCCESS) {
        vpss_error("call vpss_hal_set_node_info error!!!\n");
        return HI_FAILURE;
    }

    if (vpss_inst_check_virtual_start(instance) == HI_TRUE) {
        ret = vpss_hal_set_node_info(ip, hal_info);
        if (ret != HI_SUCCESS) {
            vpss_error("call vpss_hal_set_node_info error!!!\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_void vpss_inst_set_rotate_tunl(vpss_hal_info *hal_info)
{
    hal_info->out_wtunl_en = HI_FALSE;

    if ((hal_info->in_rtunl_en == HI_TRUE) || (hal_info->in_wtunl_en == HI_TRUE) ||
        (hal_info->out_rtunl_en == HI_TRUE) || (hal_info->out_wtunl_en == HI_TRUE)) {
        hal_info->tunl_en = HI_TRUE;
    } else {
        hal_info->tunl_en = HI_FALSE;
    }
}

hi_s32 vpss_inst_config_rotation_noalg(vpss_instance *instance)
{
    hi_s32 ret;

    ret = vpss_inst_fill_input_frame_info(instance, HI_DRV_3D_EYE_LEFT);
    if (ret != HI_SUCCESS) {
        vpss_warn("fill input frame info error,en_lr = %d!\n", HI_DRV_3D_EYE_LEFT);
        return HI_FAILURE;
    }

    if (((instance->stream_info.pixel_format == HI_DRV_PIXEL_FMT_NV12) ||
         (instance->stream_info.pixel_format == HI_DRV_PIXEL_FMT_NV21)) &&
        (instance->stream_info.cmp_info.data_fmt == HI_DRV_DATA_FMT_TILE)) {
        instance->hal_info.in_info.bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
    }

    vpss_policy_revise_out_frame_info(&(instance->stream_info.pixel_format), &(instance->stream_info.cmp_info));
    instance->hal_info.in_info.pixel_format = instance->stream_info.pixel_format;
    instance->hal_info.in_info.cmp_info = instance->stream_info.cmp_info;

    return HI_SUCCESS;
}

hi_s32 vpss_inst_config_rotation_task(vpss_instance *instance, hi_u32 port_id)
{
    hi_s32 ret;
    hi_drv_vpss_video_frame *input_frame = HI_NULL;
    vpss_hal_info *hal_info = &instance->hal_info;
    vpss_ip ip = instance->ctrl_id;
    vpss_out_node *out_info = HI_NULL;
    hi_bool rota_alg_en = vpss_policy_support_rota_alg(&instance->dbg_ctrl,
        hal_info->in_info.rect_w, hal_info->in_info.rect_h);
    if (rota_alg_en == HI_TRUE) {
        hal_info->in_rtunl_en = HI_FALSE;
        hal_info->in_wtunl_en = HI_FALSE;
        input_frame = &(instance->out_frm_node[port_id]->out_priv_frame);
        vpss_inst_set_hal_frame_info(input_frame, &hal_info->in_info, HI_DRV_3D_EYE_LEFT);
    } else {
        ret = vpss_inst_config_rotation_noalg(instance);
        if (ret != HI_SUCCESS) {
            vpss_warn("vpss_inst_config_rotation_noalg error !\n");
            return HI_FAILURE;
        }
    }

    vpss_inst_set_rotate_tunl(hal_info);
    out_info = instance->out_frm_node[port_id];

    vpss_inst_rotate_outframe_info(instance, port_id, &out_info->buffer, &out_info->out_priv_frame, HI_DRV_3D_EYE_LEFT);
    vpss_inst_set_hal_frame_info(&out_info->out_priv_frame, &hal_info->port_info[port_id].out_info, HI_DRV_3D_EYE_LEFT);

    hal_info->port_id = port_id;
    hal_info->node_type = VPSS_HAL_NODE_ROTATION_Y;
    ret = vpss_hal_set_node_info(ip, hal_info);
    if (ret != HI_SUCCESS) {
        vpss_error("call vpss_hal_set_node_info ROTATION_Y error!!!\n");
        return HI_FAILURE;
    }

    if (hal_info->port_info[port_id].rotation != HI_DRV_VPSS_ROTATION_180) {
        hal_info->node_type = VPSS_HAL_NODE_ROTATION_C;
        ret = vpss_hal_set_node_info(ip, hal_info);
        if (ret != HI_SUCCESS) {
            vpss_error("call vpss_hal_set_node_info ROTATION_C error!!!\n");
        }
    }

    return ret;
}

hi_void vpss_inst_process_tunnel(vpss_instance *instance)
{
}

hi_s32 vpss_inst_config_task(vpss_instance *instance)
{
    hi_u32 port_id;
    hi_bool start_3d = HI_FALSE;
    hi_s32 ret;
    hi_bool rota_alg_en = HI_FALSE;
#ifdef DPT
    hi_u32 valid_port = 0;
#endif

    /* step 1 : init pq para */
    instance->perfor.pq_dfx.init_time_start = vpss_comm_get_sched_time();
    if (instance->stream_info.new_scene == HI_TRUE) {
        ret = vpss_inst_update_pq_timing_info(instance, &(instance->rev_new_frame));
        if (ret != HI_SUCCESS) {
            vpss_error("Update pq timing info failed.\n");
        }
    }
    instance->perfor.pq_dfx.init_time_end = vpss_comm_get_sched_time();

    instance->state = INSTANCE_STATE_CONFIG;

    /* step 2 : always config one 2d task */
    instance->perfor.config_dfx.task_2d_time_start = vpss_comm_get_sched_time();
    ret = vpss_inst_config_2d_task(instance);
    instance->perfor.config_dfx.task_2d_time_end = vpss_comm_get_sched_time();
    if (ret != HI_SUCCESS) {
        instance->perfor.config_dfx.task_2d_err_cnt++;
        vpss_warn("call vpss_ctrl_start2d_task error!!!\n");
        return HI_FAILURE;
    }

    /* step 3 : config rotate */
    instance->perfor.config_dfx.task_rota_time_start = vpss_comm_get_sched_time();
    for (port_id = 0; port_id < VPSS_PORT_MAX_NUM; port_id++) {
        if (vpss_inst_check_check_rotation(instance, port_id) == HI_TRUE) {
#ifdef DPT
            valid_port = port_id;
#endif
            rota_alg_en = vpss_policy_support_rota_alg(&instance->dbg_ctrl,
                instance->hal_info.in_info.rect_w, instance->hal_info.in_info.rect_h);
            if ((rota_alg_en == HI_TRUE)
                && (instance->hal_info.port_info[port_id].rotation == HI_DRV_VPSS_ROTATION_180)) {
                continue;
            }

            ret = vpss_inst_config_rotation_task(instance, port_id);
            if (ret != HI_SUCCESS) {
                instance->perfor.config_dfx.task_rota_err_cnt++;
                vpss_warn("call vpss_ctrl_start_rotate_task error!!!\n");
                return HI_FAILURE;
            }
        } else {
            if (instance->ro_buf[port_id].buff_info.size != 0) {
                vpss_comm_mem_free(&instance->ro_buf[port_id].buff_info);
                instance->ro_buf[port_id].buff_info.size = 0;
            }
        }
    }
    instance->perfor.config_dfx.task_rota_time_end = vpss_comm_get_sched_time();

#ifdef DPT
    vpss_hal_hwbuff_cfg(valid_port, &instance->hal_info);
#endif
    /* step 4 : when 3d, config one more task */
    instance->perfor.config_dfx.task_3d_time_start = vpss_comm_get_sched_time();
    start_3d = vpss_inst_check_3d_task(instance);
    if (start_3d == HI_TRUE) {
        ret = vpss_inst_config_3d_task(instance);
        if (ret != HI_SUCCESS) {
            instance->perfor.config_dfx.task_3d_err_cnt++;
            vpss_warn("call vpss_ctrl_start3_d_task error!!!\n");
            return HI_FAILURE;
        }
    }
    instance->perfor.config_dfx.task_3d_time_end = vpss_comm_get_sched_time();

    /* step 5 : when tunnel, config tunnel */
    vpss_inst_process_tunnel(instance);
    instance->hal_info.pre_frame_addr = instance->hal_info.in_info.hal_addr.phy_addr_y;
    return HI_SUCCESS;
}

hi_void vpss_inst_modify_buff_status(vpss_instance *instance)
{
    /* only used by logic low delay */
#ifdef DPT
    hi_bool empty_list = HI_FALSE;
    unsigned long flags;
    hi_s32 ret;

    vpss_hal_modify_complete(instance->ctrl_id, HWBUFF_CHAN_VPSS_SEND, HWBUFF_ERROR_ACCOMPLISH);

    vpss_comm_down_spin(&(instance->src_spin), &flags);
    empty_list = vpss_src_is_empty(&instance->src_list);
    vpss_comm_up_spin(&(instance->src_spin), &flags);

    if (empty_list != HI_TRUE) {
        return;
    }

    vpss_hal_modify_complete(instance->ctrl_id, HWBUFF_CHAN_VPSS_GET, HWBUFF_SOFT_ACCOMPLISH);
    instance->perfor.refcnt_dfx.try_dec_cnt++;
    ret = vpss_comm_decrease_frame_refcount_privframe(&instance->cur_process_frame_info);
    if (ret == HI_SUCCESS) {
        instance->perfor.refcnt_dfx.try_dec_suc_cnt++;
        vpss_dbg_print_ref_count_info(instance->global_dbg_ctrl, &(instance->cur_process_frame_info),
            VPSS_REFCNT_DECREASE_PRO_TYPE);
    } else {
        vpss_error("decrease ref fail! addr=%08x\n", instance->cur_process_frame_info.vpss_buf_addr[0].phy_addr_y);
    }

#endif

    return;
}

hi_void vpss_inst_clear_task(vpss_instance *instance)
{
    hi_u32 count;
    vpss_port *port = HI_NULL;
    vpss_out_node *node = HI_NULL;

    instance->state = INSTANCE_STATE_CLEAR;

    /* step1:release in buffer */
    if ((instance->hal_info.in_wtunl_en != HI_TRUE) || (instance->logic_work != HI_TRUE)) {
        vpss_inst_release_in_buffer(instance);
    }

    vpss_inst_modify_buff_status(instance);
    instance->perfor.cnt_dfx.error_count++;

    /* step2:release out buffer */
    if (instance->hal_info.out_wtunl_en != HI_TRUE || (instance->logic_work != HI_TRUE)) {
        for (count = 0; count < VPSS_PORT_MAX_NUM; count++) {
            port = &instance->port[count];
            node = instance->out_frm_node[count];

            if (instance->ro_buf[count].buff_info.size != 0) {
                vpss_comm_mem_free(&(instance->ro_buf[count].buff_info));
                instance->ro_buf[count].buff_info.size = 0;
            }

            if (port->tunnel_enable != HI_TRUE) {
                if (node != HI_NULL) {
                    vpss_out_add_empty_frm_buf(&port->out_list, node);
                }
            } else {
                vpss_out_rel_ful_frm(&port->out_list, &node->out_frame);
            }
        }
    }

    /* step3:reset vpss logic */
    (hi_void) vpss_hal_clear_task_reset(instance->ctrl_id);

    instance->state = INSTANCE_STATE_IDLE;
}

hi_void get_lbd_info(hi_u8 *vir_addr, hi_bool interlace, hi_bool print_memc_info,
                     hi_drv_vpss_video_frame *out_frame)
{
#ifdef DPT /* donot support */
    hi_drv_crop_rect lbx_info = { 0 };
    vpss_hal_get_lbd_info(vir_addr, &lbx_info, &out_frame->lbx_delta_info);

    if (interlace == HI_TRUE) {
        lbx_info.top_offset *= 2;                      // 2 is used to cal when interlace
        lbx_info.bottom_offset *= 2;                   // 2 is used to cal when interlace
        out_frame->lbx_delta_info.top_offset *= 2;     // 2 is used to cal when interlace
        out_frame->lbx_delta_info.bottom_offset *= 2;  // 2 is used to cal when interlace
    }

    if ((lbx_info.bottom_offset > out_frame->comm_frame.height) ||
        (lbx_info.right_offset > out_frame->comm_frame.width)) {
        vpss_info("get_lbd_info error frame w %d h %d,lbd top %d bot %d left %d right %d!\n",
                  out_frame->comm_frame.width, out_frame->comm_frame.height, lbx_info.top_offset,
                  lbx_info.bottom_offset, lbx_info.left_offset, lbx_info.right_offset);
        memset(&(out_frame->lbx_info), 0, sizeof(hi_drv_crop_rect));
        return;
    }

    out_frame->lbx_info.top_offset = lbx_info.top_offset;
    out_frame->lbx_info.bottom_offset = out_frame->comm_frame.height - lbx_info.bottom_offset;
    out_frame->lbx_info.left_offset = lbx_info.left_offset;
    out_frame->lbx_info.right_offset = out_frame->comm_frame.width - lbx_info.right_offset;

    if (print_memc_info) {
        vpss_info("get_lbd_info t %d b %d l %d r %d shift %d %d %d %d!\n", out_frame->lbx_info.top_offset,
                  out_frame->lbx_info.bottom_offset, out_frame->lbx_info.left_offset,
                  out_frame->lbx_info.right_offset, out_frame->lbx_delta_info.top_offset,
                  out_frame->lbx_delta_info.bottom_offset, out_frame->lbx_delta_info.left_offset,
                  out_frame->lbx_delta_info.right_offset);
    }

#endif
    return;
}

hi_void get_cmp_size(hi_u32 ctrl_id, hi_u32 *cmp_size)
{
    hi_s32 ret;
    ret = vpss_hal_get_cmp_size(ctrl_id, cmp_size);
    if (ret != HI_SUCCESS) {
        vpss_error("vpss_hal_get_cmp_size failed!\n");
    }

    return;
}

hi_void vpss_inst_adjust_frame_info(vpss_instance *instance, vpss_out_node *node)
{
    vpss_pts_node *out_pts_node = HI_NULL;
    list *ref_node = HI_NULL;

    if (instance->hal_info.in_info.progressive != HI_TRUE) {
        ref_node = instance->first_ref->prev->prev;
        out_pts_node = (vpss_pts_node *)osal_list_entry(ref_node, vpss_pts_node, node);
        node->out_priv_frame.comm_frame.pts = out_pts_node->pts_data;
        node->out_priv_frame.comm_frame.field_mode = out_pts_node->field_mode;
        node->out_priv_frame.comm_frame.frame_index = out_pts_node->frame_index;
        node->out_priv_frame.comm_frame.repeat_first_field = out_pts_node->repeat_first_field;

        if (node->out_priv_frame.comm_frame.repeat_first_field == 1) {
            if (node->out_priv_frame.comm_frame.top_field_first) {
                node->out_priv_frame.comm_frame.repeat_first_field =
                    (node->out_priv_frame.comm_frame.field_mode == HI_DRV_FIELD_TOP) ? 1 : 0;
            } else {
                node->out_priv_frame.comm_frame.repeat_first_field =
                    (node->out_priv_frame.comm_frame.field_mode == HI_DRV_FIELD_BOTTOM) ? 1 : 0;
            }
        }
    }

    return;
}

static hi_void vpss_inst_update_frame_info(vpss_instance *instance, vpss_out_node *node)
{
    hi_bool support_lbd = vpss_policy_support_lbd();
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;
    hi_drv_win_vpss_meta_info *meta = HI_NULL;

    if (support_lbd == HI_TRUE) {
        get_lbd_info(instance->hal_info.stt_cfg.global_cfg.w_stt_chn.vir_addr,
                     instance->stream_info.interlace, instance->global_dbg_ctrl->print_memc_info,
                     &node->out_priv_frame);
    }

    meta_info = (hi_drv_win_vpss_meta_info *)node->out_priv_frame.comm_frame.video_private.win_private.data;

#ifdef DPT
    get_cmp_size(instance->ctrl_id, &meta_info->frame_cmp_size);
    meta_info->film_type = instance->alg_info.alg_out_para.ifmd_reault.film_type;
    meta_info->key_value = instance->alg_info.alg_out_para.ifmd_reault.key_value;
#else
    meta_info->film_type = instance->alg_info.alg_out_para.ifmd_playback.film_mode;
    meta_info->key_value = instance->alg_info.alg_out_para.ifmd_playback.key_frame;
#endif
    vpss_policy_revise_out_frame_info(&node->out_priv_frame.comm_frame.pixel_format,
        &node->out_priv_frame.comm_frame.cmp_info);

    meta = (hi_drv_win_vpss_meta_info *)instance->cur_process_frame_info.comm_frame.video_private.win_private.data;
    meta_info->vpss_crop = meta->vpss_crop;
}

hi_void vpss_inst_notify_out_event(vpss_instance *instance)
{
    if (instance->wbc_list[0].complete_count == VPSS_OUTPUT_FRAME_FIRST) {
        hi_drv_stat_event(HI_STAT_EVENT_VPSS_OUT_FRM, 0);
    }
}

hi_void vpss_inst_cvt_privite_to_comm(vpss_out_node *node)
{
    hi_u64 node_dma_buf = (hi_u64)node->buffer.buff_info.dma_handle;

    /* convert priv frame add to dma_handle of comm frame */
    node->out_frame.buf_addr[0].start_addr = 0;
    node->out_frame.buf_addr[0].dma_handle = node_dma_buf;
    node->out_frame.buf_addr[0].y_head_offset = 0;
    node->out_frame.buf_addr[0].c_head_offset = 0;
    node->out_frame.buf_addr[0].cr_head_offset = 0;
    node->out_frame.buf_addr[0].y_offset = node->out_priv_frame.vpss_buf_addr[0].phy_addr_y_offset;
    node->out_frame.buf_addr[0].c_offset = node->out_priv_frame.vpss_buf_addr[0].phy_addr_c_offset;
    node->out_frame.buf_addr[0].cr_offset = node->out_priv_frame.vpss_buf_addr[0].phy_addr_cr_offset;

    /* for save yuv */
    node->out_priv_frame.comm_frame.buf_addr[0].dma_handle = node_dma_buf;
    node->out_priv_frame.comm_frame.buf_addr[0].y_head_offset = 0;
    node->out_priv_frame.comm_frame.buf_addr[0].c_head_offset = 0;
    node->out_priv_frame.comm_frame.buf_addr[0].cr_head_offset = 0;
    node->out_priv_frame.comm_frame.buf_addr[0].y_offset = node->out_frame.buf_addr[0].y_offset;
    node->out_priv_frame.comm_frame.buf_addr[0].c_offset = node->out_frame.buf_addr[0].c_offset;
    node->out_priv_frame.comm_frame.buf_addr[0].cr_offset = node->out_frame.buf_addr[0].cr_offset;

    /* fill stride */
    node->out_frame.buf_addr[0].stride_y = node->buffer.stride_y;
    node->out_frame.buf_addr[0].stride_c = node->buffer.stride_c;

    memset(&(node->out_frame.buf_addr[1]), 0, sizeof(hi_drv_vid_frame_addr));
    memset(&(node->out_frame.buf_addr_lb[0]), 0, sizeof(hi_drv_vid_frame_addr));
    memset(&(node->out_frame.buf_addr_lb[1]), 0, sizeof(hi_drv_vid_frame_addr));

    return;
}

hi_void vpss_inst_send_frame(vpss_instance *instance)
{
    hi_s32 ret;
    hi_u32 count = 0;
    vpss_port *port = HI_NULL;
    vpss_out_node *node = HI_NULL;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;
    hi_bool repeat_idx = HI_FALSE;

    instance->state = INSTANCE_STATE_SUCCESS;

    for (count = 0; count < VPSS_PORT_MAX_NUM; count++) {
        port = &(instance->port[count]);
        if ((port->port_id == HI_INVALID_HANDLE) || (port->enble == HI_FALSE) || (port->tunnel_enable == HI_TRUE)) {
            continue;
        }

        node = instance->out_frm_node[count];
        if (node == HI_NULL) {
            vpss_dbg("error,can't happen portid %d\n", count);
            continue;
        }

        if (check_frame_need_drop(instance, port, &node->out_priv_frame) == HI_FALSE) {
            vpss_inst_adjust_frame_info(instance, node);

            if (port->port_type == HI_DRV_VPSS_PORT_TYPE_3D_DETECT) {
                vpss_inst_3d_detect_outframe_info(instance->cur_process_frame_info.comm_frame.video_3d_type,
                    &node->out_priv_frame);
            } else {
                vpss_inst_update_frame_info(instance, node);
            }

            instance->perfor.complete_dfx.send_out_start[count] = vpss_comm_get_sched_time();

            /* step 1 : copy metadata */
            meta_info = (hi_drv_win_vpss_meta_info *)node->out_priv_frame.comm_frame.video_private.win_private.data;
            meta_info->pause_frame = instance->pause;
            instance->perfor.complete_dfx.send_out_end1[count] = vpss_comm_get_sched_time();

            node->out_priv_frame.comm_frame.video_private.vpss_private.data = (hi_u64)node->metedata.vir_addr;
            vpss_comm_copy_priv_win_meta_to_vpss_meta(&(node->out_priv_frame));
            /* interlace playcnt  */
            if ((instance->stream_info.interlace == HI_TRUE) && (instance->stream_info.special_field != HI_TRUE)) {
                repeat_idx = vpss_out_check_frame_index_repeat(&port->out_list,
                    node->out_priv_frame.comm_frame.frame_index);
                vpss_policy_distribute_out_meta_playcnt(repeat_idx, &(node->out_priv_frame));
            }

            /* step 2 : out priv frame to out frame */
            vpss_comm_cvt_privite_to_comm_nomap(&node->out_priv_frame, &node->out_frame);
            instance->perfor.complete_dfx.send_out_end2[count] = vpss_comm_get_sched_time();

            /* step 3 : dma_handle to adappt */
            vpss_inst_cvt_privite_to_comm(node);
            instance->perfor.complete_dfx.send_out_end3[count] = vpss_comm_get_sched_time();

            vpss_out_add_ful_frm_buf(&port->out_list, node);

            if (port->call_back_func.sink_queue_frame != HI_NULL) {
                instance->perfor.out_dfx.queue_cnt++;
                vpss_dbg_print_frame(instance->global_dbg_ctrl, &node->out_priv_frame, count);
                vpss_dbg_save_frame(instance->global_dbg_ctrl, &node->out_priv_frame, count);
                vpss_dbg_print_frame_addr(instance->global_dbg_ctrl, &node->out_frame, VPSS_OUT_QUEUE_FRAME_TYPE);

                ret = port->call_back_func.sink_queue_frame(port->h_dst_module, &node->out_frame);
                if (ret != HI_SUCCESS) {
                    vpss_error("VPSS sink_queue_frame to vdp failed id %d laddr %x \n",
                               node->out_priv_frame.comm_frame.frame_index,
                               node->out_priv_frame.vpss_buf_addr[0].phy_addr_y);
                    vpss_out_move_target(&port->out_list);
                    vpss_out_rel_ful_frm(&port->out_list, &node->out_frame);
                } else {
                    vpss_comm_notify_event(&node->out_priv_frame, HI_STAT_LD_EVENT_VPSS_FRM_OUT);
                    vpss_out_move_target(&port->out_list);
                    instance->perfor.out_dfx.queue_suc_cnt++;
                }
            }
            instance->perfor.complete_dfx.send_out_q_end4[count] = vpss_comm_get_sched_time();
        } else {
            vpss_out_add_empty_frm_buf(&port->out_list, node);
        }

        vpss_inst_notify_out_event(instance);
    }

    return;
}

hi_u32 vpss_inst_need_modify_complete_state(vpss_instance *instance)
{
    hi_u32 rect_w = instance->hal_info.in_info.rect_w;
    hi_u32 rect_h = instance->hal_info.in_info.rect_h;
    hi_bool rota_alg_en = vpss_policy_support_rota_alg(&instance->dbg_ctrl, rect_w, rect_h);
    if (((rect_w <= VPSS_SD_WIDTH) ||
         ((rota_alg_en == HI_TRUE) &&
          (instance->hal_info.port_info[0].rotation == HI_DRV_VPSS_ROTATION_DISABLE))) &&
        (instance->hal_info.in_info.progressive != HI_TRUE)) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

hi_s32 vpss_inst_complete_task(vpss_instance *instance)
{
    hi_s32 ret;
    hi_bool empty_list = HI_FALSE;
    unsigned long flags;
    hi_bool rota_alg_en = HI_FALSE;

    /* step 1 :run alg */
    vpss_hal_get_rgmv_addr(instance->ctrl_id, &(instance->hal_info.stt_cfg.cf_rgmv_phy_addr));
    invoke_pq_function(instance);

    /* step 2:add outframe to out,queue frame to sink */
    instance->perfor.complete_dfx.send_out_time_start = vpss_comm_get_sched_time();
    if (instance->hal_info.out_wtunl_en != HI_TRUE) {
        vpss_inst_send_frame(instance);
    }
    instance->perfor.complete_dfx.send_out_time_end = vpss_comm_get_sched_time();

    /* step 3 :release processed frame */
    instance->perfor.complete_dfx.rel_src_time_start = vpss_comm_get_sched_time();
    if (instance->hal_info.in_wtunl_en != HI_TRUE) {
        vpss_inst_release_frame(instance);
    }
    instance->perfor.complete_dfx.rel_src_time_end = vpss_comm_get_sched_time();

    rota_alg_en = vpss_policy_support_rota_alg(&instance->dbg_ctrl,
        instance->hal_info.in_info.rect_w, instance->hal_info.in_info.rect_h);

    instance->perfor.complete_dfx.complete_time_start = vpss_comm_get_sched_time();
    if ((instance->hal_info.port_info[0].rotation == HI_DRV_VPSS_ROTATION_DISABLE) ||
        (rota_alg_en == HI_TRUE)) {
        vpss_inst_complete_frame(instance);
    } else {
        instance->wbc_list->complete_count = VPSS_OUTPUT_FRAME_FIRST;
    }
    instance->perfor.complete_dfx.complete_time_end = vpss_comm_get_sched_time();

    vpss_comm_down_spin(&(instance->src_spin), &flags);
    empty_list = vpss_src_is_empty(&instance->src_list);
    vpss_comm_up_spin(&(instance->src_spin), &flags);

    if (empty_list == HI_TRUE) {
#ifdef DPT
        if (vpss_inst_need_modify_complete_state(instance) == HI_TRUE) {
            vpss_hal_modify_complete(instance->ctrl_id, HWBUFF_CHAN_VPSS_GET, HWBUFF_SOFT_ACCOMPLISH);
        }
#endif

        instance->perfor.refcnt_dfx.try_dec_cnt++;
        ret = vpss_comm_decrease_frame_refcount_privframe(&(instance->cur_process_frame_info));
        if (ret == HI_SUCCESS) {
            instance->perfor.refcnt_dfx.try_dec_suc_cnt++;
            vpss_dbg_print_ref_count_info(instance->global_dbg_ctrl, &(instance->cur_process_frame_info),
                                          VPSS_REFCNT_DECREASE_PRO_TYPE);
        } else {
            vpss_error("decrease ref fail! addr=%08x\n",
                       instance->cur_process_frame_info.vpss_buf_addr[0].phy_addr_y);
        }
    }

    instance->state = INSTANCE_STATE_IDLE;
    return HI_SUCCESS;
}

hi_bool vpss_inst_update_state(vpss_instance *instance, vpss_instance_state state)
{
    unsigned long flags;
    vpss_comm_down_spin(&(instance->usr_set_spin), &flags);

    if (state != INSTANCE_STATE_IDLE) {
        instance->prepareing = HI_FALSE;
    }

    instance->state = state;
    vpss_comm_up_spin(&(instance->usr_set_spin), &flags);
    return HI_TRUE;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
