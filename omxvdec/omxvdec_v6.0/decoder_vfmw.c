/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */

#include "hi_drv_module.h"  // HI_ID_OMXVDEC
#include "hi_drv_stat.h"
#include "decoder.h"
#include "vfmw_ext.h"
#include "hi_drv_video.h"
#include "linux/hisilicon/securec.h"
#include <asm/uaccess.h>
#include "linux/ion.h"
#include <linux/dma-buf.h>


/* ================ EXTERN VALUE ================ */
extern OMXVDEC_ENTRY *g_omx_vdec;
extern OMXVDEC_FUNC g_st_omx_func;
extern hi_bool g_save_yuv_enable;
extern hi_u32 g_save_yuv_chan_num;
extern hi_char g_save_path[];
extern hi_char g_save_name[];
extern hi_u32 g_save_num;

/* ================ STATIC VALUE ================ */
vfmw_export_fun *g_vfmw_func = HI_NULL;

/* ================ GLOBAL VALUE ================ */
hi_u32 g_client_reserved_num = 10;
hi_u32 g_seg_size = 2;               // (M)
hi_bool g_raw_move_enable = HI_TRUE;  // 码流搬移使能标志，解决scd切割失败不释放码流的情况
hi_bool g_fast_output_mode = HI_FALSE;

/* ================== DATA TYPE ================= */
typedef enum {
    CFG_CAP_SD = 0,
    CFG_CAP_HD,
    CFG_CAP_UHD,
    CFG_CAP_BUTT,
} e_cfg_cap;

/* ============== INTERNAL FUNCTION DECLARE============= */
hi_s32 decoder_get_output_format(OMXVDEC_CHAN_CTX *pchan, hi_bool *is_tile);

/* ============== INTERNAL FUNCTION ============= */
static hi_void decoder_event_new_image_process(OMXVDEC_CHAN_CTX *pchan, omxvdec_buf *pbuf,
                                               hi_drv_video_frame *pst_frame)
{
    OMXVDEC_BUF_DESC user_buf;
    hi_u32 image_id;
    vfmw_private_frm_info *vfmw_private_info =
        (vfmw_private_frm_info *)UINT64_PTR(pst_frame->video_private.vdec_private.data);

    if (vfmw_private_info == HI_NULL) {
        return ;
    }

    image_id = vfmw_private_info->image_id;

    HI_CHECK_SEC_FUNC(memset_s(&user_buf, sizeof(user_buf), 0, sizeof(OMXVDEC_BUF_DESC)));

    user_buf.dir = PORT_DIR_OUTPUT;
    user_buf.bufferaddr = (uintptr_t)pbuf->user_vaddr;
    user_buf.buffer_len = pbuf->buf_len;
    user_buf.phyaddr = pbuf->phy_addr + pbuf->offset;
    user_buf.client_data = (uintptr_t)pbuf->client_data;
    user_buf.flags = VFMW_OMX_STM_FLAG_ENDOFFRAME;
    user_buf.release_fd = pbuf->release_fd;

    user_buf.out_frame.stride = pst_frame->buf_addr[0].stride_y;
    user_buf.out_frame.width = pst_frame->width;
    user_buf.out_frame.height = pst_frame->height;
    user_buf.frame_rate = pst_frame->frame_rate;
    user_buf.buffer_type = pbuf->buf_type;

    user_buf.dec_color_aspects.m_range = vfmw_private_info->color_desc.full_rangeFlag;
    user_buf.dec_color_aspects.m_primaries = vfmw_private_info->color_desc.colour_primaries;
    user_buf.dec_color_aspects.m_matrix_coeffs = vfmw_private_info->color_desc.matrix_coeffs;
    user_buf.dec_color_aspects.m_transfer = vfmw_private_info->color_desc.transfer_characteristics;

    user_buf.src_frame_type = pst_frame->hdr_type;
    HI_CHECK_SEC_FUNC(memcpy_s(&user_buf.hdr_info,  sizeof(hi_drv_hdr_metadata),
        &pst_frame->hdr_info,  sizeof(hi_drv_hdr_metadata)));

    pchan->last_frame.frame_rate = pst_frame->frame_rate;

    if (g_save_yuv_chan_num == pchan->channel_id) {
        user_buf.out_frame.save_yuv = g_save_yuv_enable;
    }

#if (1 == HI_PROC_SUPPORT)
    if (user_buf.out_frame.save_yuv == HI_TRUE) {
        hi_s32 ret;
        ret = snprintf_s(user_buf.out_frame.save_path, sizeof(user_buf.out_frame.save_path),
            sizeof(user_buf.out_frame.save_path) - 1, "%s/%s_%d_%dx%d.yuv",
            g_save_path, g_save_name, g_save_num, pst_frame->width, pst_frame->height);
        if (ret == -1) {
            omx_print(OMX_ALWS, "call sec funtion error");
        }
        user_buf.out_frame.save_path[PATH_LEN - 1] = '\0';
    }
#endif

    if (pchan->output_flush_pending) {
        pbuf->is_bind = HI_FALSE;
        user_buf.data_len = 0;
        user_buf.timestamp = 0;
    } else {
        user_buf.timestamp = pst_frame->pts;
        user_buf.data_len = DEFAULT_FRAME_SIZE(pchan->out_stride, pst_frame->height);
    }

    pbuf->act_len = user_buf.data_len;
    pbuf->ext_info.data = pst_frame->video_private.vdec_private.data;
    vfmw_private_info->image_id = image_id != INVALID_IMAGE_ID ? image_id : 0;
    pchan->last_frame.ext_info.data = pbuf->ext_info.data;

    pchan->last_frame.ext_info.last_frame = (pst_frame->video_private.last_flag == HI_DRV_LAST_FRAME_FLAG) ? 1 : 0;

    omx_print(OMX_PTS, "output time stamp: %lld\n", user_buf.timestamp);

    message_queue(pchan->msg_queue, VDEC_MSG_RESP_OUTPUT_DONE, VDEC_S_SUCCESS, &user_buf);
    pchan->omx_chan_statinfo.FBD++;

    if (pchan->output_flush_pending) {
        pchan->yuv_use_cnt = (pchan->yuv_use_cnt > 0) ? (pchan->yuv_use_cnt - 1) : 0;

        if (pchan->yuv_use_cnt == 0) {
            message_queue(pchan->msg_queue, VDEC_MSG_RESP_FLUSH_OUTPUT_DONE, VDEC_S_SUCCESS, HI_NULL);
            pchan->output_flush_pending = 0;
            omx_print(OMX_INFO, "flush done\n");
        }
    }

    return;
}

static hi_u32 decoder_get_frame_phyaddr(OMXVDEC_CHAN_CTX *chan, hi_s64 fd)
{
    hi_s32 i;
    hi_u32 phy_addr = 0;
    for (i = 0; i < OMXVDEC_MAX_EXT_BUF_NUM; i++) {
        if (chan->ext_buf[i].is_valid == 1 && fd == chan->ext_buf[i].fd) {
            phy_addr = chan->ext_buf[i].phy_addr;
            break;
        }
    }

    if (i >= OMXVDEC_MAX_EXT_BUF_NUM) {
        omx_print(OMX_WARN, "i: %d >= max: %d overflow\n", i, OMXVDEC_MAX_EXT_BUF_NUM);
    }

    return phy_addr;
}

static hi_s32 decoder_event_new_image(OMXVDEC_CHAN_CTX *pchan, hi_void *pargs, hi_s32 size)
{
    hi_u32 phyaddr;
    unsigned long flags;
    hi_s32 is_find = 0;
    hi_bool secure;

    omxvdec_buf *pbuf = HI_NULL;
    omxvdec_buf *ptmp = HI_NULL;
    hi_drv_video_frame *pst_frame = pargs;

    OMXVDEC_BUF_DESC user_buf = { 0 };

    if (pchan == HI_NULL || pargs == HI_NULL || size != sizeof(hi_drv_video_frame)) {
        omx_print(OMX_FATAL, "param is null!\n");
        return HI_FAILURE;
    }

    HI_CHECK_SEC_FUNC(memset_s(&user_buf, sizeof(user_buf), 0, sizeof(OMXVDEC_BUF_DESC)));

    VDEC_DOWN_INTERRUPTIBLE(&pchan->chan_sema);

    pchan->omx_chan_statinfo.report_image_try++;

    secure = pst_frame->secure;

    phyaddr = decoder_get_frame_phyaddr(pchan, pst_frame->buf_addr[0].start_addr);
    if (phyaddr == 0) {
        VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);
        return HI_FAILURE;
    }

    /* for we del list during, so use safe means */
    osal_spin_lock_irqsave(&pchan->yuv_lock, &flags);
    if (list_empty(&pchan->yuv_queue)) {
        osal_spin_unlock_irqrestore(&pchan->yuv_lock, &flags);
        omx_print(OMX_ERR, "list is empty\n");
        return HI_FAILURE;
    }

    list_for_each_entry_safe(pbuf, ptmp, &pchan->yuv_queue, list)
    {
        if (phyaddr == (pbuf->phy_addr + pbuf->offset)) {
            if (pbuf->status != BUF_STATE_USING) {
                omx_print(OMX_ERR, "buffer(0x%08x) flags confused!\n", phyaddr);
            }

            is_find = 1;
            pbuf->status = BUF_STATE_IDLE;
            list_del(&pbuf->list);
            break;
        }
    }

    if (!is_find) {
        osal_spin_unlock_irqrestore(&pchan->yuv_lock, &flags);
        omx_print(OMX_ERR, "buffer(0x%08x) not in queue!\n", phyaddr);
        return HI_FAILURE;
    }

    decoder_event_new_image_process(pchan, pbuf, pst_frame);

    osal_spin_unlock_irqrestore(&pchan->yuv_lock, &flags);

    omx_print(OMX_OUTBUF, "report frame: phy addr = 0x%x, data_len: %d\n", phyaddr, user_buf.data_len);

    pchan->omx_chan_statinfo.report_image_ok++;

    VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);

    return HI_SUCCESS;
}

hi_s32 decoder_report_eos(OMXVDEC_CHAN_CTX *pchan)
{
    unsigned long flags;
    unsigned long flags_yuv;
    hi_s32 is_find = 0;
    omxvdec_buf *pbuf = HI_NULL;
    omxvdec_buf *ptmp = HI_NULL;
    OMXVDEC_BUF_DESC user_buf = { 0 };
    HI_CHECK_SEC_FUNC(memset_s(&user_buf, sizeof(user_buf), 0, sizeof(OMXVDEC_BUF_DESC)));

    FUNC_IN();

    osal_spin_lock_irqsave(&pchan->chan_lock, &flags);
    if (pchan->state != CHAN_STATE_WORK) {
        osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags);
        omx_print(OMX_VPSS, "pchan->state != CHAN_STATE_WORK\n");
        return HI_FAILURE;
    }

    osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags);

    if (pchan->output_flush_pending) {
        omx_print(OMX_ERR, "output_flush_pending\n");
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&pchan->yuv_lock, &flags_yuv);
    if (list_empty(&pchan->yuv_queue)) {
        osal_spin_unlock_irqrestore(&pchan->yuv_lock, &flags_yuv);
        omx_print(OMX_ERR, "list is empty!\n");

        return HI_FAILURE;
    }

    list_for_each_entry_safe(pbuf, ptmp, &pchan->yuv_queue, list)
    {
        if (pbuf->status == BUF_STATE_IDLE) {
            continue;
        }

        is_find = 1;
        pbuf->status = BUF_STATE_IDLE;
        list_del(&pbuf->list);
        break;
    }

    osal_spin_unlock_irqrestore(&pchan->yuv_lock, &flags_yuv);

    if (!is_find) {
        omx_print(OMX_ERR, "no availible buffer in the queue!\n");
        return HI_FAILURE;
    }

    /* let out msg to inform application */
    user_buf.dir = PORT_DIR_OUTPUT;
    user_buf.bufferaddr = (uintptr_t)pbuf->user_vaddr;
    user_buf.buffer_len = pbuf->buf_len;
    user_buf.client_data = (uintptr_t)pbuf->client_data;
    user_buf.data_len = 0;
    user_buf.timestamp = 0;
    user_buf.flags |= VFMW_OMX_STM_FLAG_EOS;
    user_buf.release_fd = -1;

    pchan->eos_info.status = CHAN_REPORT_SUCCESS;
    pchan->eos_info.eos_send_num++;

    omx_print(OMX_INFO, "report eos success\n");

    message_queue(pchan->msg_queue, VDEC_MSG_RESP_OUTPUT_DONE, VDEC_S_SUCCESS, (hi_void *)&user_buf);
    FUNC_EXIT();

    return HI_SUCCESS;
}

static hi_s32 decoder_event_last_frame(OMXVDEC_CHAN_CTX *pchan, hi_void *pargs, hi_s32 size)
{
    hi_u32 *ptemp = (hi_u32 *)pargs;

    if (pargs == HI_NULL || size != sizeof(hi_u32)) {
        omx_print(OMX_ERR, "para err\n");
        return HI_FAILURE;
    }

    omx_print(OMX_INFO, "get last frame report!\n");

    /* pargs[0]-> 0: success, 1: fail,  2+: report last frame image id */
    if (ptemp[0] == DECODER_REPORT_LAST_FRAME_SUCCESS) {
        pchan->eos_info.status = DECODER_REPORT_SUCCESS;
        omx_print(OMX_INFO, "last frame report success!\n");
    } else if (ptemp[0] == DECODER_REPORT_LAST_FRAME_FAIL) {
        pchan->eos_info.status = DECODER_REPORT_FAILED;
        omx_print(OMX_ERR, "last frame report failed!\n");
    } else {
        pchan->eos_info.status = DECODER_REPORT_WITH_ID;
        pchan->eos_info.image_id = ptemp[0] - 2; /* 2 is gap value */
        omx_print(OMX_INFO, "last frame report image id %d!\n", ptemp[0] - 2); /* 2 is gap value */
    }

    return HI_SUCCESS;
}

static hi_s32 decoder_event_unsupport(OMXVDEC_CHAN_CTX *pchan, hi_void *pargs, hi_s32 size)
{
    omx_print(OMX_ERR, "report not supported\n");

    message_queue(pchan->msg_queue, VDEC_ERR_NOT_SUPPORTED, VDEC_S_SUCCESS, HI_NULL);

    return HI_SUCCESS;
}

static hi_void decoder_clear_ext_buffer_record(OMXVDEC_CHAN_CTX *chan)
{
    hi_s32 i;

    for (i = 0 ; i < OMXVDEC_MAX_EXT_BUF_NUM; i++) {
        if (chan->ext_buf[i].is_valid == 1) {
            chan->ext_buf[i].is_valid = -1;
        }
    }

    return;
}

static hi_s32 decoder_report_resolution(OMXVDEC_CHAN_CTX *pchan, hi_void *pargs, hi_s32 size)
{
    OMXVDEC_IMG_SIZE image_info;
    hi_u32 *presolution_info = (hi_u32 *)pargs;

    image_info.size = presolution_info[0];
    image_info.max_num = presolution_info[1];
    image_info.width = presolution_info[6]; /* 6 is array index */
    image_info.height = presolution_info[7]; /* 7 is array index */
    image_info.stride = presolution_info[4]; /* 4 is array index */
    image_info.bit_depth = presolution_info[5]; /* 5 is array index */

    image_info.min_num = image_info.max_num - 3; /* 3 :g_client_reserved_num; */

    pchan->out_width = image_info.width;
    pchan->out_height = image_info.height;
    pchan->out_stride = image_info.stride;
    pchan->recfg_flag = 1;
    pchan->port_enable_flag = 0;

    decoder_clear_ext_buffer_record(pchan);

    message_queue(pchan->msg_queue, VDEC_EVT_REPORT_IMG_SIZE_CHG, VDEC_S_SUCCESS, (hi_void *)&image_info);

    omx_print(OMX_INFO, "size change, w&h:%d&%d stride:%d size:%d min num:%d bit depth:%d\n",
              image_info.width, image_info.height, image_info.stride,
              image_info.size, image_info.min_num, image_info.bit_depth);

    return HI_SUCCESS;
}

static hi_s32 decoder_hdr_info_report(OMXVDEC_CHAN_CTX *pchan, hi_void *pargs)
{
    OMXVDEC_DEC_COLOR_ASPECTS color_aspects_info;
    hi_u32 *hdr_info = (hi_u32 *)pargs;

    color_aspects_info.m_range = hdr_info[0];
    color_aspects_info.m_primaries = hdr_info[1];
    color_aspects_info.m_matrix_coeffs = hdr_info[2]; /* 2 is array index */
    color_aspects_info.m_transfer = hdr_info[3]; /* 3 is array index */

    message_queue(pchan->msg_queue, VDEC_EVT_REPORT_HDR_INFO, VDEC_S_SUCCESS, (hi_void *)&color_aspects_info);

    omx_print(OMX_INFO, "report hdr info, m_range:%d m_primaries:%d m_matrix_coeffs:%d m_transfer:%d \n",
              hdr_info[0], hdr_info[1], hdr_info[2], hdr_info[3]); /* 2 3 is an array index */

    return HI_SUCCESS;
}

static hi_s32 decoder_event_receive_failed(OMXVDEC_CHAN_CTX *pchan)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 failed_num = 0;
    hi_u32 image_id = INVALID_IMAGE_ID;

    if (pchan->last_frame.ext_info.data != 0) {
        image_id = ((vfmw_private_frm_info *)UINT64_PTR(pchan->last_frame.ext_info.data))->image_id;
    }

    if (pchan->eos_info.status == CHAN_REPORT_SUCCESS) {
        return HI_SUCCESS;
    }

    if (pchan->eos_info.status != STATUS_INVALID) {
        pchan->eos_info.eos_try_num++;
        omx_print(OMX_INFO, "status:%d last_frame:%d try num:%d\n",
                  pchan->eos_info.status, pchan->last_frame.ext_info.last_frame, pchan->eos_info.eos_try_num);
    }

    if (((pchan->eos_info.image_id == image_id % 100) /* 100 is unit conversion */
         && (pchan->eos_info.status == DECODER_REPORT_WITH_ID))
        || (pchan->eos_info.status == DECODER_REPORT_FAILED)
        || (pchan->last_frame.ext_info.last_frame == 1)
        || ((pchan->eos_info.status == DECODER_REPORT_SUCCESS) && (pchan->eos_info.eos_try_num > 4))) { /* 4 :try num */
        do {
            ret = decoder_report_eos(pchan);
            if (ret != HI_SUCCESS) {
                failed_num++;
                msleep(5); /* 5 is time */
            }

            if (failed_num > 300) { /* 300 : decoder report eos failed time */
                omx_print(OMX_ERR, "decoder_report_eos is failed !!\n");
                return HI_FAILURE;
            }
            if (pchan->state == CHAN_STATE_INVALID || pchan->state == CHAN_STATE_IDLE) {
                omx_print(OMX_ERR, "pchan->state is INVALID or IDLE !!\n");
                return HI_FAILURE;
            }
        } while (ret != HI_SUCCESS);
    }

    return HI_SUCCESS;
}

static hi_s32 decoder_event_handler(hi_s32 chan_id, hi_s32 event_type, hi_void *pargs, hi_s32 size)
{
    hi_s32 ret = HI_FAILURE;
    OMXVDEC_CHAN_CTX *pchan = HI_NULL;

    pchan = channel_find_inst_by_decoder_id(g_omx_vdec, chan_id);
    if (pchan == HI_NULL) {
        omx_print(OMX_FATAL, "can't find chan(%d) type:%d\n", chan_id, event_type);
        return HI_FAILURE;
    }

    if (pchan->tunnel_mode_enable) {
        return HI_SUCCESS;
    }

    switch (event_type) {
        case EVNT_NEW_YUV:
            OMXVDEC_ASSERT_RETURN(pargs != HI_NULL, "pargs = HI_NULL");
            ret = decoder_event_new_image(pchan, pargs, size);
            break;

        case EVNT_RECEIVE_FAILED:
            ret = decoder_event_receive_failed(pchan);
            break;

        case EVNT_LAST_FRAME:
            OMXVDEC_ASSERT_RETURN(pargs != HI_NULL, "pargs = HI_NULL");

            ret = decoder_event_last_frame(pchan, pargs, size);
            break;

        case EVNT_UNSUPPORT:
            ret = decoder_event_unsupport(pchan, pargs, size);
            break;

        case EVNT_UNSUPPORT_SPEC:
            ret = decoder_event_unsupport(pchan, pargs, size);
            break;

        case EVNT_RESOLUTION_CHANGE:
            OMXVDEC_ASSERT_RETURN(pargs != HI_NULL, "pargs = HI_NULL");

            ret = decoder_report_resolution(pchan, pargs, size);
            break;

        case EVNT_HDR_INFO:
            OMXVDEC_ASSERT_RETURN(pargs != HI_NULL, "pargs = HI_NULL");

            ret = decoder_hdr_info_report(pchan, pargs);
            break;

        default:
            break;
    }

    return ret;
}

static hi_void decoder_init_capability(OMXVDEC_CHAN_CTX *pchan, vfmw_chan_cfg *pchan_cfg,
    vfmw_chan_option *pst_option)
{
    if (pchan_cfg->vid_std == VFMW_HEVC || pchan_cfg->vid_std == VFMW_H264 || pchan_cfg->vid_std == VFMW_MVC) {
        pst_option->max_sps_num = 32; /* 32 for H264,H265 max sps number */
        pst_option->max_pps_num = 256; /* 256 for H264,H265 max pps number */
    }

    return;
}

static hi_s32 decoder_init_option(OMXVDEC_CHAN_CTX *pchan, vfmw_chan_cfg *pchan_cfg,
                                  vfmw_chan_option *pst_option)
{
    if (pchan == HI_NULL || pchan_cfg == HI_NULL || pst_option == HI_NULL) {
        omx_print(OMX_FATAL, "param invalid!\n");

        return HI_FAILURE;
    }

    HI_CHECK_SEC_FUNC(memset_s(pst_option, sizeof(vfmw_chan_option), 0, sizeof(vfmw_chan_option)));

    pst_option->adpt_type = ADPT_TYPE_OMX;
    pst_option->is_sec_mode = pchan->is_tvp;
    pst_option->ssm_handle = pchan->ssm_handle;
    pst_option->vdec_handle = pchan->omx_handle;

    decoder_init_capability(pchan, pchan_cfg, pst_option);

    return HI_SUCCESS;
}

static hi_s32 decoder_get_stream(hi_s32 chan_id, vfmw_stream_data *stream_data)
{
    hi_s32 ret;
    unsigned long flags;
    unsigned long flags_chan;
    unsigned long flags_raw;
    omxvdec_buf *pbuf = HI_NULL;
    OMXVDEC_CHAN_CTX *pchan = HI_NULL;

    osal_spin_lock_irqsave(&g_omx_vdec->stream_lock, &flags);

    pchan = channel_find_inst_by_channel_id(g_omx_vdec, chan_id);
    if (pchan == HI_NULL) {
        osal_spin_unlock_irqrestore(&g_omx_vdec->stream_lock, &flags);

        omx_print(OMX_FATAL, "can't find chan(%d).\n", chan_id);
        return HI_FAILURE;
    }

    pchan->omx_chan_statinfo.get_stream_try++;

    osal_spin_lock_irqsave(&pchan->chan_lock, &flags_chan);
    if (pchan->state != CHAN_STATE_WORK) {
        osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags_chan);
        osal_spin_unlock_irqrestore(&g_omx_vdec->stream_lock, &flags);

        return HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags_chan);

    osal_spin_lock_irqsave(&pchan->raw_lock, &flags_raw);
    if (list_empty(&pchan->raw_queue)) {
        osal_spin_unlock_irqrestore(&pchan->raw_lock, &flags_raw);
        osal_spin_unlock_irqrestore(&g_omx_vdec->stream_lock, &flags);

        return HI_FAILURE;
    }

    if (pchan->input_flush_pending) {
        osal_spin_unlock_irqrestore(&pchan->raw_lock, &flags_raw);
        osal_spin_unlock_irqrestore(&g_omx_vdec->stream_lock, &flags);
        omx_print(OMX_INBUF, "invalid: input_flush_pending\n");

        return HI_FAILURE;
    }

    ret = HI_FAILURE;
    list_for_each_entry(pbuf, &pchan->raw_queue, list)
    {
        if (pbuf->status == BUF_STATE_USING) {
            continue;
        }

        HI_CHECK_SEC_FUNC(memset_s(stream_data, sizeof(vfmw_stream_data), 0, sizeof(vfmw_stream_data)));

        pbuf->status = BUF_STATE_USING;
        stream_data->phy_addr = pbuf->phy_addr + pbuf->offset;
        stream_data->vir_addr = (hi_u64)(uintptr_t)(pbuf->kern_vaddr + pbuf->offset);
        stream_data->length = pbuf->act_len;
        stream_data->pts = pbuf->time_stamp;
        stream_data->vfmw_raw_ext.flags = pbuf->flags;
        stream_data->vfmw_raw_ext.buf_len = pbuf->buf_len;
        stream_data->vfmw_raw_ext.cfg_width = pchan->out_width;
        stream_data->vfmw_raw_ext.cfg_height = pchan->out_height;
        stream_data->user_tag = pbuf->usr_tag;

        if (pchan->seek_pending) {
            stream_data->vfmw_raw_ext.is_seek_pending = 1;
            pchan->seek_pending = 0;
        }

        if (pbuf->flags & VFMW_OMX_STM_FLAG_ENDOFFRAME) {
            stream_data->is_not_last_packet_flag = 0;
        } else {
            stream_data->is_not_last_packet_flag = 1;
        }

        if (pbuf->buf_id == LAST_FRAME_BUF_ID) {
            omx_print(OMX_INFO, "vfmw read last frame.\n");
            stream_data->is_stream_end_flag = 1;
        }

        pchan->raw_use_cnt++;

        omx_print(OMX_PTS, "input time stamp: %lld\n", stream_data->pts);

        ret = HI_SUCCESS;

        if ((pchan->b_lowdelay || g_fast_output_mode) && (pchan->omx_vdec_lowdelay_proc_rec != HI_NULL)) {
            channel_add_lowdelay_tag_time(pchan, stream_data->user_tag, OMX_LOWDELAY_REC_VFMW_RCV_STRM_TIME,
                omx_get_time_in_ms());
        }

        omx_print(OMX_INBUF, "PhyAddr = 0x%08llx, len = %d\n", stream_data->phy_addr, stream_data->length);
        pchan->omx_chan_statinfo.get_stream_ok++;
        break;
    }

    osal_spin_unlock_irqrestore(&pchan->raw_lock, &flags_raw);
    osal_spin_unlock_irqrestore(&g_omx_vdec->stream_lock, &flags);

    return ret;
}

hi_s32 decoder_get_stream_ex(hi_s32 chan_id, hi_void *stream_data)
{
    hi_s32 ret;

    ret = decoder_get_stream(chan_id, (vfmw_stream_data *)stream_data);
    if (ret == HI_SUCCESS) {
        if (((vfmw_stream_data *)stream_data)->pts != HI_INVALID_PTS) {
            do_div(((vfmw_stream_data *)stream_data)->pts, 1000); /* 1000 is unit conversion */
        }
    }

    return ret;
}

static hi_void decoder_release_stream_process(OMXVDEC_CHAN_CTX *pchan, vfmw_stream_data *stream_data,
                                              omxvdec_buf *pbuf)
{
    OMXVDEC_BUF_DESC user_buf;

    HI_CHECK_SEC_FUNC(memset_s(&user_buf, sizeof(user_buf), 0, sizeof(user_buf)));

    if (pbuf->buf_id != LAST_FRAME_BUF_ID) { /* let msg to indicate buffer was given back */
        user_buf.dir = PORT_DIR_INPUT;
        user_buf.bufferaddr = (uintptr_t)pbuf->user_vaddr;
        user_buf.buffer_len = pbuf->buf_len;
        user_buf.client_data = (uintptr_t)pbuf->client_data;
        user_buf.data_len = 0;
        user_buf.timestamp = 0;
        user_buf.phyaddr = pbuf->phy_addr;

        pbuf->act_len = user_buf.data_len;

        message_queue(pchan->msg_queue, VDEC_MSG_RESP_INPUT_DONE, VDEC_S_SUCCESS, (hi_void *)&user_buf);
        pchan->omx_chan_statinfo.EBD++;
    } else {
        pchan->eos_info.eos_in_list = HI_FALSE;
        omx_print(OMX_INFO, "vfmw release last stream.\n");
    }

    pchan->raw_use_cnt = (pchan->raw_use_cnt > 0) ? (pchan->raw_use_cnt - 1) : 0;

    if (pchan->input_flush_pending && (pchan->raw_use_cnt == 0)) {
        omx_print(OMX_INBUF, "input flush done!\n");
        message_queue(pchan->msg_queue, VDEC_MSG_RESP_FLUSH_INPUT_DONE, VDEC_S_SUCCESS, HI_NULL);
        pchan->input_flush_pending = 0;
    }

    if ((pchan->b_lowdelay || g_fast_output_mode) && (pchan->omx_vdec_lowdelay_proc_rec != HI_NULL)) {
        channel_add_lowdelay_tag_time(pchan, stream_data->user_tag, OMX_LOWDELAY_REC_VFMW_RLS_STRM_TIME,
            omx_get_time_in_ms());
    }

    return;
}

static hi_s32 decoder_release_stream(hi_s32 chan_id, vfmw_stream_data *stream_data)
{
    unsigned long flags;
    unsigned long flags_raw;
    hi_s32 is_find = 0;
    omxvdec_buf *pbuf = HI_NULL;
    omxvdec_buf *ptmp = HI_NULL;
    OMXVDEC_CHAN_CTX *pchan = HI_NULL;

    if (stream_data == HI_NULL) {
        omx_print(OMX_FATAL, "stream_data = HI_NULL.\n");

        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&g_omx_vdec->stream_lock, &flags);

    pchan = channel_find_inst_by_channel_id(g_omx_vdec, chan_id);
    if (pchan == HI_NULL) {
        osal_spin_unlock_irqrestore(&g_omx_vdec->stream_lock, &flags);
        omx_print(OMX_FATAL, "can't find chan(%d).\n", chan_id);

        return HI_FAILURE;
    }

    pchan->omx_chan_statinfo.release_stream_try++;

    /* for we del element during, so use safe methods for list */
    osal_spin_lock_irqsave(&pchan->raw_lock, &flags_raw);
    if (list_empty(&pchan->raw_queue)) {
        osal_spin_unlock_irqrestore(&pchan->raw_lock, &flags_raw);
        osal_spin_unlock_irqrestore(&g_omx_vdec->stream_lock, &flags);
        omx_print(OMX_ERR, "list is empty\n");

        return HI_SUCCESS;
    }

    list_for_each_entry_safe(pbuf, ptmp, &pchan->raw_queue, list)
    {
        if (stream_data->phy_addr == (pbuf->phy_addr + pbuf->offset)) {
            if (pbuf->status != BUF_STATE_USING) {
                omx_print(OMX_ERR, "buf(0x%08llx) flag confused!\n", stream_data->phy_addr);
            }

            pbuf->status = BUF_STATE_IDLE;
            list_del(&pbuf->list);
            is_find = 1;
            break;
        }
    }

    if (!is_find) {
        osal_spin_unlock_irqrestore(&pchan->raw_lock, &flags_raw);
        osal_spin_unlock_irqrestore(&g_omx_vdec->stream_lock, &flags);

        omx_print(OMX_ERR, "buffer(0x%08llx) not in queue!\n", stream_data->phy_addr);

        return HI_FAILURE;
    }

    decoder_release_stream_process(pchan, stream_data, pbuf);
    pchan->omx_chan_statinfo.release_stream_ok++;

    osal_spin_unlock_irqrestore(&pchan->raw_lock, &flags_raw);
    osal_spin_unlock_irqrestore(&g_omx_vdec->stream_lock, &flags);

    omx_print(OMX_INBUF, "PhyAddr = 0x%08llx, len = %d\n", stream_data->phy_addr, stream_data->length);

    return HI_SUCCESS;
}

hi_s32 decoder_release_stream_ex(hi_s32 chan_id, hi_void *stream_data)
{
    return decoder_release_stream(chan_id, (vfmw_stream_data *)stream_data);
}

static hi_s32 decoder_clear_stream(OMXVDEC_CHAN_CTX *pchan)
{
    hi_s32 ret;

    FUNC_IN();

    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_RELEASE_STREAM, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "release stream failed\n");

        return HI_FAILURE;
    }

    FUNC_EXIT();

    return HI_SUCCESS;
}

static hi_s32 decoder_resume_process(OMXVDEC_CHAN_CTX *pchan)
{
#if 0 // todo
    hi_s32 ret;

    FUNC_IN();

    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_RESUME_PROCESS, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "failed\n");

        return HI_FAILURE;
    }

    FUNC_EXIT();
#endif
    return HI_SUCCESS;
}

static hi_s32 decoder_pause_process(OMXVDEC_CHAN_CTX *pchan)
{
#if 0 // todo
    hi_s32 ret;

    FUNC_IN();

    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_PAUSE_PROCESS, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "failed\n");

        return HI_FAILURE;
    }

    FUNC_EXIT();
#endif

    return HI_SUCCESS;
}

static hi_s32 decoder_bind_es_buf(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf)
{
    vfmw_stream_buf buf = {0};
    hi_s32 ret;

    OMXVDEC_ASSERT_RETURN(puser_buf != HI_NULL, "puser_buf = HI_NULL");
    HI_CHECK_SEC_FUNC(memset_s(&buf, sizeof(buf), 0, sizeof(vfmw_stream_buf)));

    buf.phy_addr = puser_buf->phyaddr;
    buf.length = puser_buf->buffer_len;

    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_BIND_ES_BUF, &buf, sizeof(vfmw_ext_buf));
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "vfmw chan %d bind es buffer fail!\n", pchan->decoder_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 decoder_bind_mem(OMXVDEC_CHAN_CTX *pchan, omxvdec_buf *pframe)
{
    hi_s32 ret;
    vfmw_ext_buf ext_buf;
    vfmw_mem_report *p_mem = HI_NULL;

    FUNC_IN();

    OMXVDEC_ASSERT_RETURN(pframe != HI_NULL, "pframe = HI_NULL");

    HI_CHECK_SEC_FUNC(memset_s(&ext_buf, sizeof(ext_buf), 0, sizeof(vfmw_ext_buf)));

    p_mem = &(ext_buf.ext_frm_buf);

    ret = snprintf_s(p_mem->buf_name, sizeof(p_mem->buf_name),  sizeof(p_mem->buf_name) - 1, "extern_buf");
    if (ret == -1) {
        omx_print(OMX_FATAL, "call sec funtion error");
    }

    p_mem->phy_addr = pframe->phy_addr;
    p_mem->dma_buf = pframe->dma_buf;
    p_mem->mem_fd = pframe->fd;
    p_mem->vir_addr = (hi_u64)(uintptr_t)(pframe->kern_vaddr);
    p_mem->size = pframe->buf_len;
    p_mem->is_map = HI_TRUE;
    p_mem->is_cache = HI_FALSE;

    omx_print(OMX_OUTBUF, "disp buffer fd:%lld PhyAddr:%llx kern_vaddr:%llx size:%d map:%d cache:%d\n",
              pframe->fd, p_mem->phy_addr, p_mem->vir_addr, p_mem->size, p_mem->is_map, p_mem->is_cache);

    p_mem = &(ext_buf.ext_info_buf);

    ret = snprintf_s(p_mem->buf_name, sizeof(p_mem->buf_name), sizeof(p_mem->buf_name) - 1, "attached_buf");
    if (ret == -1) {
        omx_print(OMX_FATAL, "call sec funtion error");
    }

    p_mem->phy_addr = pframe->private_phy_addr;
    p_mem->mem_fd = pframe->private_fd;
    p_mem->size = pframe->private_len;
    p_mem->vir_addr = (hi_u64)(uintptr_t)(pframe->private_vaddr);
    p_mem->is_map = (p_mem->vir_addr == HI_NULL) ? HI_TRUE : HI_FALSE;
    p_mem->is_cache = HI_FALSE;

    omx_print(OMX_OUTBUF, "attach buffer PhyAddr:%llx kern_vaddr:%llx size:%d map:%d cache:%d\n",
              p_mem->phy_addr, p_mem->vir_addr, p_mem->size, p_mem->is_map, p_mem->is_cache);

    p_mem = &(ext_buf.ext_hfbc_buf);
    ret = snprintf_s(p_mem->buf_name, sizeof(p_mem->buf_name), sizeof(p_mem->buf_name) - 1, "hfbc_buf");
    if (ret == -1) {
        omx_print(OMX_FATAL, "call sec funtion error");
    }

    p_mem->mem_fd = pframe->private_fd;
    p_mem->vir_addr = (hi_u64)(uintptr_t)(pframe->hfbc_vaddr);
    p_mem->is_map = (p_mem->vir_addr == HI_NULL) ? HI_TRUE : HI_FALSE;
    p_mem->is_cache = HI_FALSE;

    omx_print(OMX_OUTBUF, "hfbc buffer fd = %lld kern_vaddr:%llx map:%d cache:%d\n",
              p_mem->mem_fd, p_mem->vir_addr, p_mem->is_map, p_mem->is_cache);

    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_BIND_EXTERN_BUFFER,
                                          &ext_buf, sizeof(vfmw_ext_buf));
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "bind mem to channel failed\n");

        return HI_FAILURE;
    }

    FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 decoder_unbind_mem(OMXVDEC_CHAN_CTX *pchan, omxvdec_buf *pframe)
{
    hi_s32 ret;
    vfmw_ext_buf ext_buf;
    vfmw_mem_report *p_mem = HI_NULL;

    FUNC_IN();

    OMXVDEC_ASSERT_RETURN(pframe != HI_NULL, "pframe = HI_NULL");

    HI_CHECK_SEC_FUNC(memset_s(&ext_buf, sizeof(ext_buf), 0, sizeof(vfmw_ext_buf)));

    p_mem = &(ext_buf.ext_frm_buf);
    ret = snprintf_s(p_mem->buf_name, sizeof(p_mem->buf_name), sizeof(p_mem->buf_name) - 1, "extern_buf");
    if (ret == -1) {
        omx_print(OMX_FATAL, "call sec funtion error");
    }

    p_mem->phy_addr = pframe->phy_addr;
    p_mem->size = pframe->buf_len;

    if (pframe->kern_vaddr) {
        p_mem->vir_addr = (hi_u64)(uintptr_t)pframe->kern_vaddr;
    }

    omx_print(OMX_OUTBUF, "disp buffer PhyAddr:%llx kern_vaddr:%llx size:%d map:%d cache:%d\n",
              p_mem->phy_addr, p_mem->vir_addr, p_mem->size, p_mem->is_map, p_mem->is_cache);

    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_UNBIND_EXTERN_BUFFER,
                                          &ext_buf, sizeof(vfmw_ext_buf));
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "unbind mem to channel failed\n");
        return HI_FAILURE;
    } else if (ext_buf.status == EXT_BUF_STATUS_UNAVAILABLE) {
        omx_print(OMX_FATAL, "unbind mem to channel failed, status is unavailable\n");
        return HI_FAILURE;
    }

    FUNC_EXIT();

    return HI_SUCCESS;
}

static hi_s32 decoder_release_frame(OMXVDEC_CHAN_CTX *pchan, omxvdec_buf *p_buf)
{
    hi_s32 ret;
    vfmw_private_frm_info *vdec_private =
        (vfmw_private_frm_info *)UINT64_PTR(pchan->frame->video_private.vdec_private.data);

    FUNC_IN();

    pchan->omx_chan_statinfo.release_image_try++;

    if (pchan->frame == HI_NULL) {
        omx_print(OMX_FATAL, "pchan->frame is null\n");
        return HI_FAILURE;
    }

    HI_CHECK_SEC_FUNC(memset_s(pchan->frame, sizeof(hi_drv_video_frame), 0x0, sizeof(hi_drv_video_frame)));
    pchan->frame->video_private.vdec_private.data = PTR_UINT64(vdec_private);
    pchan->frame->buf_addr[0].start_addr = p_buf->fd;
    pchan->frame->video_private.vdec_private.data = p_buf->ext_info.data;
    pchan->frame->video_private.vdec_private.tpye = HI_DRV_MODULE_VDEC;
    pchan->frame->video_private.vdec_private.size = sizeof(vfmw_private_frm_info);
#if 0
    /* todo */
    if (p_buf->attach_buf.attached_vir_addr) {
        frame = (hi_drv_video_frame *)(uintptr_t)(p_buf->attach_buf.attached_vir_addr);
        if (frame != HI_NULL) {
            pchan->frame->hardware_buffer.src_id = frame->hardware_buffer.src_id;
            pchan->frame->hardware_buffer.dst_id = frame->hardware_buffer.dst_id;
            pchan->frame->hardware_buffer.buffer_index = frame->hardware_buffer.buffer_index;
            pchan->frame->hardware_buffer.buffer_addr = frame->hardware_buffer.buffer_addr;
            pchan->frame->hardware_buffer.backward_tunnel_enable = frame->hardware_buffer.backward_tunnel_enable;
            pchan->frame->hardware_buffer.forward_tunnel_enable = frame->hardware_buffer.forward_tunnel_enable;
        }
    }
#endif

    omx_print(OMX_OUTBUF, "PhyAddr:0x%x image_id:%d\n", p_buf->phy_addr,
        ((vfmw_private_frm_info *)UINT64_PTR(p_buf->ext_info.data))->image_id);

    ret = g_vfmw_func->fn_vfmw_rels_frame(pchan->decoder_id, pchan->frame);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "release image(0x%x) failed\n", p_buf->phy_addr);

        return HI_FAILURE;
    }

    pchan->omx_chan_statinfo.release_image_ok++;
    FUNC_EXIT();

    return HI_SUCCESS;
}

static hi_s32 decoder_set_frame_num(OMXVDEC_CHAN_CTX *pchan, hi_u32 *p_num)
{
    return HI_FAILURE;
}

static hi_void decoder_config_inst_sample_frd(OMXVDEC_CHAN_CTX *pchan, vfmw_chan_cfg *pchan_cfg)
{
    hi_u8 need_frd = 0;  // defualt close sample FRD

    /* 猜帧策略不适用场景: CTS/GTS */
    if (pchan->android_test) {
        need_frd = 0;
    }

    /* 猜帧策略不适用场景: lowdelay/fast output */
    if (pchan->b_lowdelay || g_fast_output_mode) {
        need_frd = 0;
    }

    /* 猜帧策略不适用场景: not IPB/not disp order */
    if (pchan_cfg->dec_mode != VFMW_DEC_MODE_NORMAL || pchan_cfg->dec_order_output != 0) {
        need_frd = 0;
    }

    if (need_frd == 1) {
        pchan_cfg->sample_frd_enable = HI_TRUE;
    } else {
        pchan_cfg->sample_frd_enable = HI_FALSE;
    }

    return;
}

/* ============ EXPORT INTERFACE ============= */
hi_s32 decoder_init(hi_void)
{
    hi_s32 ret;
    vfmw_init_param st_init_param;

    FUNC_IN();

    g_vfmw_func = HI_NULL;

    ret = osal_exportfunc_get(HI_ID_VFMW, (hi_void **)&g_st_omx_func.p_decoder_func);
    if (ret != HI_SUCCESS || g_st_omx_func.p_decoder_func == HI_NULL) {
        omx_print(OMX_FATAL, "get vfmw function failed!\n");

        return HI_FAILURE;
    }

    g_vfmw_func = (vfmw_export_fun *)(g_st_omx_func.p_decoder_func);

    HI_CHECK_SEC_FUNC(memset_s(&st_init_param, sizeof(st_init_param), 0, sizeof(vfmw_init_param)));

    st_init_param.is_secure = HI_FALSE;
    st_init_param.adpt_type = ADPT_TYPE_OMX;
    st_init_param.fn_callback = decoder_event_handler;

    ret = (g_vfmw_func->fn_vfmw_init)(&st_init_param);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "init vfmw failed!\n");

        return HI_FAILURE;
    }

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 decoder_exit(hi_void)
{
    hi_s32 ret;
    hi_bool secure = HI_FALSE;

    FUNC_IN();

    ret = (g_vfmw_func->fn_vfmw_exit)(&secure);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "exit vfmw failed!\n");

        return HI_FAILURE;
    }

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 decoder_suspend(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    FUNC_IN();

    if ((g_vfmw_func != HI_NULL) && (g_vfmw_func->fn_vfmw_suspend != HI_NULL)) {
        ret = (g_vfmw_func->fn_vfmw_suspend)(HI_NULL);
        if (ret != HI_SUCCESS) {
            omx_print(OMX_FATAL, "suspend decoder failed\n");

            return HI_FAILURE;
        }
    }

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 decoder_resume(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    FUNC_IN();

    if ((g_vfmw_func != HI_NULL) && (g_vfmw_func->fn_vfmw_resume != HI_NULL)) {
        ret = (g_vfmw_func->fn_vfmw_resume)(HI_NULL);
        if (ret != HI_SUCCESS) {
            omx_print(OMX_FATAL, "resume decoder failed\n");

            return HI_FAILURE;
        }
    }

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 decoder_init_trusted(hi_void)
{
    hi_s32 ret;
    vfmw_init_param st_init_param;

    FUNC_IN();

    HI_CHECK_SEC_FUNC(memset_s(&st_init_param, sizeof(st_init_param), 0, sizeof(vfmw_init_param)));

    st_init_param.is_secure = HI_TRUE;
    st_init_param.adpt_type = ADPT_TYPE_OMX;
    st_init_param.fn_callback = decoder_event_handler;

    ret = (g_vfmw_func->fn_vfmw_init)(&st_init_param);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "init trusted decoder failed!n");

        return HI_FAILURE;
    }

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 decoder_exit_trusted(hi_void)
{
    hi_s32 ret;
    hi_bool secure = HI_TRUE;

    FUNC_IN();

    ret = (g_vfmw_func->fn_vfmw_exit)(&secure);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "exit trusted decoder failed\n");

        return HI_FAILURE;
    }

    FUNC_EXIT();

    return HI_SUCCESS;
}

static hi_void decoder_config_inst_lowdelay(OMXVDEC_DRV_CFG *pdrv_cfg)
{
    vfmw_chan_cfg *pchan_cfg = &pdrv_cfg->chan_cfg;

    /* 在云游戏的场景，视频为H264，只有IP帧，如果使能快速输出模式，
       设置IP_MODE,SIMPLE_DPB,SCD lowdly */
    pchan_cfg->dec_mode = VFMW_DEC_MODE_IP;
    pchan_cfg->dec_order_output = 1;
    pchan_cfg->module_lowly_enable = HI_FALSE;  // HI_TRUE;
    pchan_cfg->chan_lowly_enable = HI_TRUE;

    return;
}

static hi_void decoder_config_inst_format(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_DRV_CFG *pdrv_cfg)
{
    vfmw_chan_cfg *pchan_cfg = &pdrv_cfg->chan_cfg;

    if (pchan->is_overlay == HI_TRUE || pchan->tunnel_mode_enable == 1) {
        pchan_cfg->v_cmp_mode = VFMW_CMP_ADJUST;
    } else {
        pchan_cfg->v_cmp_mode = VFMW_CMP_ON;
    }

    /* output to surface texture only in 8bit */
    pchan_cfg->en_bit_depth_cfg = pdrv_cfg->output_view == 0 ? VFMW_BIT_DEPTH_8BIT_ONLY : VFMW_BIT_DEPTH_NORMAL;
    omx_print(OMX_INFO, "cfg bit depth :%d (0:normal;1:8bit)\n", pchan_cfg->en_bit_depth_cfg);

    return;
}

static hi_void decoder_config_inst_android_test(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_DRV_CFG *pdrv_cfg)
{
    vfmw_chan_cfg *pchan_cfg = &pdrv_cfg->chan_cfg;

    pchan_cfg->android_test = pchan->android_test;
    pchan_cfg->extra_frame_store_num = g_client_reserved_num;

    return;
}

static hi_void decoder_config_inst_hwbuf_tunnel(OMXVDEC_DRV_CFG *drv_cfg)
{
#if 0 // todo
    vfmw_chan_cfg *cfg = &drv_cfg->chan_cfg;

    cfg->hwbuf_tunnel_en = HI_FALSE;
    if (drv_cfg->android_test == 0 && drv_cfg->output_view == 1 &&
        cfg->chan_err_thr == 100 && /* 100 :channel error threshold */
       (cfg->vid_std == VFMW_H264 || cfg->vid_std == VFMW_HEVC || cfg->vid_std == VFMW_VP9)) {
        cfg->hwbuf_tunnel_en = HI_TRUE;
    }
#endif
    return;
}

static hi_void decoder_config_inst_seg_buf(vfmw_chan_cfg *cfg)
{
#if 0 // todo
    hi_s32 ret;
    HI_SYS_MEM_CONFIG_S mem_config;

    ret = hi_drv_sys_get_mem_cfg(&mem_config.u32TotalSize, &mem_config.u32MMZSize);
    if (ret == HI_SUCCESS) {
        if (mem_config.u32TotalSize > 768) {  /* 768M */
            cfg->seg_buf_size = 8 * 1024 * 1024;  /* 8M:8*1024*1024 */
        } else {
            cfg->seg_buf_size = 2 * 1024 * 1024; /* 2M:2*1024*1024 */
        }
    } else {
        omx_print(OMX_FATAL, "call hi_drv_sys_get_mem_config failed\n");
    }
#endif
}

static hi_s32 decoder_config_inst(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_DRV_CFG *pdrv_cfg)
{
    hi_s32 ret;
    vfmw_chan_cfg *pchan_cfg = &pdrv_cfg->chan_cfg;

    FUNC_IN();

    // 如果不使能码流移动功能，将码流包个数设置最大即可
    if (g_raw_move_enable == HI_FALSE) {
        pchan_cfg->max_raw_packet_num = -1;
    }

    pchan_cfg->source = (pchan->is_net_work) ? VFMW_SOURCE_NETWORK : VFMW_SOURCE_MEDIA;

    if (pchan->b_lowdelay == HI_TRUE || g_fast_output_mode == HI_TRUE) {
        decoder_config_inst_lowdelay(pdrv_cfg);
    }

    decoder_config_inst_sample_frd(pchan, pchan_cfg);

    decoder_config_inst_android_test(pchan, pdrv_cfg);

    decoder_config_inst_format(pchan, pdrv_cfg);

    decoder_config_inst_hwbuf_tunnel(pdrv_cfg);

    decoder_config_inst_seg_buf(pchan_cfg);
#if 0
    pchan_cfg->cfg_width = pdrv_cfg->cfg_width;
    pchan_cfg->cfg_height = pdrv_cfg->cfg_height;
#endif
    pchan_cfg->alloc_type = VFMW_FRM_ALLOC_OUTER;
    omx_print(OMX_INFO, "%s alloc_type %d\n", __func__, pchan_cfg->alloc_type);

    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_CFG_CHAN, pchan_cfg, sizeof(vfmw_chan_cfg));
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "CFG_CHAN failed\n");
    }
    FUNC_EXIT();

    return ret;
}

static hi_u32 convert_event_map(hi_void)
{
    hi_s32 i;
    hi_u32 vfmw_event_map = 0;
    hi_u32 event_array[] = {EVNT_NEW_YUV, EVNT_RECEIVE_FAILED, EVNT_LAST_FRAME, EVNT_ERR_FRAME,
                            EVNT_UNSUPPORT, EVNT_UNSUPPORT_SPEC, EVNT_RESOLUTION_CHANGE, EVNT_HDR_INFO, EVNT_MAX};

    for (i = 0; event_array[i] != EVNT_MAX; i++) {
        vfmw_event_map |= event_array[i];
    }

    return vfmw_event_map;
}

hi_s32 decoder_set_event_map(OMXVDEC_CHAN_CTX *pchan)
{
    hi_s32 ret;
    hi_u32 vfmw_event_map;

    vfmw_event_map = convert_event_map();

    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_SET_EVENT_MAP, &vfmw_event_map, sizeof(hi_u32));
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "set event map failed\n");

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 decoder_create_inst(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_DRV_CFG *pdrv_cfg)
{
    hi_s32 ret;
    vfmw_chan_option st_option;
    vfmw_pts_frmrate st_frm_rate = { 0 };
    vfmw_chan_cfg *pchan_cfg = HI_NULL;
    vfmw_frm_alloc_type alloc_type;

    /* frame_buffer_alloc_type en_alloc_type */
    FUNC_IN();

    pchan->eos_info.image_id = INVALID_IMAGE_ID;

    pchan_cfg = &pdrv_cfg->chan_cfg;
    ret = decoder_init_option(pchan, pchan_cfg, &st_option);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "call decoder_init_option failed\n");
        goto error0;
    }

    ret = (g_vfmw_func->fn_vfmw_control)(-1, VFMW_CID_CREATE_CHAN, &st_option, sizeof(st_option));
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "CREATE_CHAN_WITH_OPTION failed:%#x\n", ret);
        goto error0;
    }

    pchan->decoder_id = st_option.chan_id;
    omx_print(OMX_INFO, "Create decoder %d success!\n", pchan->decoder_id);

    ret = decoder_config_inst(pchan, pdrv_cfg);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "CFG_CHAN failed\n");
        goto error1;
    }

    pchan->stream_ops.stream_provider_inst_id = pchan->channel_id;
    pchan->stream_ops.read_stream = decoder_get_stream;
    pchan->stream_ops.release_stream = decoder_release_stream;
    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_SET_STREAM_INTF, &pchan->stream_ops,
                                      sizeof(pchan->stream_ops));
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "SET_STREAM_INTF failed\n");
        goto error1;
    }

    /* set PTS module frame rate */
    st_frm_rate.en_frm_rate_type = PTS_FRMRATE_TYPE_USER_PTS;
    st_frm_rate.st_set_frm_rate.fps_integer = pdrv_cfg->cfg_framerate / 1000; /* 1000 is unit conversion */
    st_frm_rate.st_set_frm_rate.fps_decimal = pdrv_cfg->cfg_framerate % 1000; /* 1000 is unit conversion */

    if (st_frm_rate.st_set_frm_rate.fps_integer != 0) {
        ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_SET_FRAME_RATE_TYPE, &st_frm_rate,
            sizeof(st_frm_rate));
        if (ret != HI_SUCCESS) {
            omx_print(OMX_FATAL, "set frame rate failed!\n");
        }
    }

    alloc_type = VFMW_FRM_ALLOC_OUTER;
    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_SET_FRAME_BUFFER_MODE, &alloc_type,
                      sizeof(alloc_type));
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "set frame buffer mode failed\n");
    }

    ret = decoder_set_event_map(pchan);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "set event map failed\n");
        goto error1;
    }

    FUNC_EXIT();

    return HI_SUCCESS;

error1:
    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_DESTROY_CHAN, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "DESTROY_CHAN failed\n");
    }

error0:
    if (pchan->is_tvp == HI_TRUE) {
        ret = decoder_exit_trusted();
        if (ret != HI_SUCCESS) {
            omx_print(OMX_FATAL, "decoder_exit_trusted failed\n");
        }
    }

    return HI_FAILURE;
}

hi_s32 decoder_release_inst(OMXVDEC_CHAN_CTX *pchan)
{
    hi_s32 ret;

    FUNC_IN();

    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_DESTROY_CHAN, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "destroy vfmw failed\n");
    }

    FUNC_EXIT();

    return ret;
}

hi_s32 decoder_start_inst(OMXVDEC_CHAN_CTX *pchan)
{
    hi_s32 ret;

    FUNC_IN();

    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_START_CHAN, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "start vfmw failed\n");

        return HI_FAILURE;
    }

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 decoder_stop_inst(OMXVDEC_CHAN_CTX *pchan)
{
    hi_s32 ret;

    FUNC_IN();

    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_STOP_CHAN, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "stop vfmw failed\n");

        return HI_FAILURE;
    }

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 decoder_reset_inst_with_option(OMXVDEC_CHAN_CTX *pchan)
{
    hi_s32 ret;
    vfmw_flush_type option = VFMW_FLUSH_ALL;

    FUNC_IN();

    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_STOP_CHAN, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "stop vfmw failed\n");

        return HI_FAILURE;
    }

    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_FLUSH_CHAN, &option, sizeof(option));
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "reset vfmw with option failed\n");

        return HI_FAILURE;
    }

    if (pchan->state != CHAN_STATE_WORK) {
        FUNC_EXIT();
        return HI_SUCCESS;
    }

    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_START_CHAN, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "start vfmw failed\n");

        return HI_FAILURE;
    }

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 decoder_flush_inst(OMXVDEC_CHAN_CTX *pchan)
{
    hi_s32 ret;

    FUNC_IN();
    ret = (g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_FLUSH_POST_PROCESS, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "decoder flush extern buffer failed\n");
        return HI_FAILURE;
    }

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 decoder_command_handler(OMXVDEC_CHAN_CTX *pchan, decoder_cmd e_cmd, hi_void *p_args, hi_u32 param_length)
{
    hi_s32 ret = HI_FAILURE;

    switch (e_cmd) {
        case DEC_CMD_CLEAR_STREAM:
            ret = decoder_clear_stream(pchan);
            break;

        case DEC_CMD_PAUSE:
            ret = decoder_pause_process(pchan);
            break;

        case DEC_CMD_RESUME:
            ret = decoder_resume_process(pchan);
            break;

        case DEC_CMD_BIND_MEM:
            ret = decoder_bind_mem(pchan, (omxvdec_buf *)p_args);
            break;

        case DEC_CMD_UNBIND_MEM:
            ret = decoder_unbind_mem(pchan, (omxvdec_buf *)p_args);
            break;

        case DEC_CMD_RELEASE_IMAGE:
            ret = decoder_release_frame(pchan, (omxvdec_buf *)p_args);
            break;

        case DEC_CMD_SET_FRAME_NUM:
            ret = decoder_set_frame_num(pchan, p_args);
            break;

        case DEC_CMD_CFG_INST:
            ret = decoder_config_inst(pchan, (OMXVDEC_DRV_CFG *)p_args);
            break;

        case DEC_CMD_FLUSH:
            ret = decoder_flush_inst(pchan);
            break;

        case DEC_CMD_BIND_ES_BUF:
            ret = decoder_bind_es_buf(pchan, (OMXVDEC_BUF_DESC *)p_args);
            break;

        default:
            omx_print(OMX_FATAL, "unkown command %d\n", e_cmd);
            break;
    }

    return ret;
}

