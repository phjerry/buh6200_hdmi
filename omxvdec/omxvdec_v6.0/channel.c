/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */

#include <linux/dma-buf.h>  // for dma buf, ion map

#include "omxvdec.h"
#include "channel.h"
#include "decoder.h"
#include "vfmw_ext.h"
#include "hi_module.h"
#include "hi_drv_stat.h"
#include "linux/hisilicon/securec.h"


/* ================ EXTERN VALUE ================ */
extern hi_bool g_save_raw_enable;
extern hi_char g_save_path[];
extern hi_char g_save_name[];
extern hi_u32 g_save_num;
extern struct file *g_save_raw_file;
extern hi_u32 g_save_raw_chan_num;
extern hi_bool g_fast_output_mode;
extern hi_u32 g_low_delay_count_frame;
extern vfmw_export_fun *g_vfmw_func;
extern OMXVDEC_ENTRY *g_omx_vdec;
extern hi_bool g_map_frm_enable;
extern hi_u32 g_client_reserved_num;

/* =============== INTERNAL VALUE =============== */
typedef struct {
    hi_u32 std_id;
    hi_char* p_name_string;
} OMXVDEC_CODEC_INFO_ST;

static const OMXVDEC_CODEC_INFO_ST g_codec_table[] = {
    { VFMW_H264,         "H264" },
    { VFMW_HEVC,         "H265" },
    { VFMW_MVC,          "MVC" },
    { VFMW_MPEG4,        "MPEG4" },
    { VFMW_H263,         "H263" },
    { VFMW_MPEG2,        "MPEG2" },
    { VFMW_DIVX3,        "DIVX3" },
    { VFMW_VP6,          "VP6" },
    { VFMW_VP6A,         "VP6A" },
    { VFMW_VP6F,         "VP6F" },
    { VFMW_VP8,          "VP8" },
    { VFMW_VP9,          "VP9" },
    { VFMW_REAL8,        "REAL8" },
    { VFMW_REAL9,        "REAL9" },
    { VFMW_AVS,          "AVS" },
    { VFMW_AVS2,         "AVS2" },
    { VFMW_VC1,          "VC1" },
    { VFMW_SORENSON,     "SORENSON" },
    { VFMW_MJPEG,        "MJPEG" },
    { VFMW_AVS3,         "AVS3" },
    { VFMW_AV1,          "AV1" },
    { VFMW_STD_MAX,      HI_NULL },
};

/* ============== INTERNAL FUNCTION ============= */
static hi_s32 channel_get_codec_table_index(hi_u32 std_id)
{
    hi_s32 index = 0;
    hi_s32 target_index = -1;

    while (1) {
        if (g_codec_table[index].std_id == VFMW_STD_MAX) {
            break;
        }

        if (std_id == g_codec_table[index].std_id) {
            target_index = index;
            break;
        }

        index++;
    }

    return target_index;
}

static inline hi_char* channel_show_protocol(hi_u32 protocol)
{
    hi_s32 codec_index;

    codec_index = channel_get_codec_table_index(protocol);
    if (codec_index < 0) {
        return "unkown";
    } else {
        return g_codec_table[codec_index].p_name_string;
    }
}

static hi_char* channel_show_chan_state(e_chan_state state)
{
    switch (state) {
        case CHAN_STATE_IDLE:
            return "IDLE";
            break;

        case CHAN_STATE_WORK:
            return "WORK";
            break;

        case CHAN_STATE_PAUSE:
            return "PAUSE";
            break;

        case CHAN_STATE_PAUSE_PENDING:
            return "PAUSE_PENDING";
            break;

        default:
            return "INVALID";
            break;
    }
}

static hi_char* channel_show_color_format(omx_pix_format format)
{
    switch (format) {
        case OMX_PIX_FMT_NV12:
            return "yuv420_semi_planar";

        case OMX_PIX_FMT_NV21:
            return "yvu420_semi_planar";

        case OMX_PIX_FMT_YUVAFBC:
            return "YUVAFBC";

        default:
            omx_print(OMX_ERR, "unkown format %d\n", format);
            return "unkown format";
    }
}

static hi_char* channel_show_buffer_state(e_buffer_state state)
{
    switch (state) {
        case BUF_STATE_IDLE:
            return "idle";
            break;

        case BUF_STATE_QUEUED:
            return "queued";
            break;

        case BUF_STATE_USING:
            return "using";
            break;

        default:
            return "invalid";
            break;
    }
}

static hi_void channel_show_chan_config(OMXVDEC_DRV_CFG *pcfg)
{
    omx_print(OMX_INFO, "\n");
    omx_print(OMX_INFO, " is_omx_path        = %d\n", pcfg->chan_cfg.is_omx_path);
    omx_print(OMX_INFO, " protocol         = %s\n", channel_show_protocol(pcfg->chan_cfg.vid_std));
    omx_print(OMX_INFO, " resolution       = %dx%d\n", pcfg->cfg_width, pcfg->cfg_height);
    omx_print(OMX_INFO, " stride           = %d\n", pcfg->cfg_stride);
    omx_print(OMX_INFO, " use_native_buf     = %d\n", pcfg->m_use_native_buf);
    omx_print(OMX_INFO, " color_format      = %s\n", channel_show_color_format(pcfg->cfg_color_format));
    omx_print(OMX_INFO, " IsAdvProfile     = %d\n", pcfg->chan_cfg.std_ext.vc1_ext.is_adv_profile);
    omx_print(OMX_INFO, " CodecVersion     = %d\n", pcfg->chan_cfg.std_ext.vc1_ext.codec_version);
    omx_print(OMX_INFO, " bReversed        = %d\n", pcfg->chan_cfg.std_ext.vp6_ext.reversed);
    omx_print(OMX_INFO, " chan_priority     = %d\n", pcfg->chan_cfg.chan_priority);
    omx_print(OMX_INFO, " chan_err_thr       = %d\n", pcfg->chan_cfg.chan_err_thr);
    omx_print(OMX_INFO, " dec_mode          = %d\n", pcfg->chan_cfg.dec_mode);
    omx_print(OMX_INFO, " dec_order_output   = %d\n", pcfg->chan_cfg.dec_order_output);
    omx_print(OMX_INFO, " support_all_p      = %d\n", pcfg->chan_cfg.support_all_p);
    /* omx_print(OMX_INFO, " lowdly_enable     = %d\n", pcfg->chan_cfg.lowdly_enable); */
    omx_print(OMX_INFO, " max_raw_packet_num  = %d\n", pcfg->chan_cfg.max_raw_packet_num);
    omx_print(OMX_INFO, " max_raw_packet_size = %d\n", pcfg->chan_cfg.max_raw_packet_size);
    omx_print(OMX_INFO, "\n");

    return;
}

static hi_u32 channel_get_phy_from_fd(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf)
{
    hi_void *buf = HI_NULL;

    buf = osal_mem_handle_get(puser_buf->fd, HI_ID_VDEC);
    if (IS_ERR(buf)) {
        omx_print(OMX_FATAL, "dma_buf_get failed!\n");
        return HI_FAILURE;
    }
    osal_mem_ref_put(buf, HI_ID_VDEC);
    puser_buf->phyaddr = osal_mem_nssmmu_map(buf, 0);

    return HI_SUCCESS;
}

static hi_s32 channel_add_chan_record(OMXVDEC_ENTRY *omxvdec, OMXVDEC_CHAN_CTX *pchan)
{
    hi_u32 record_id = 0;
    hi_u32 channel_id = 0;
    unsigned long flags;
    OMXVDEC_CHAN_CTX *recorded_chan = HI_NULL;

    if (omxvdec == HI_NULL || pchan == HI_NULL) {
        omx_print(OMX_FATAL, "param invalid!\n");
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&omxvdec->channel_lock, &flags);
    if (!list_empty(&omxvdec->chan_list)) {
        do {
            record_id = channel_id;
            list_for_each_entry(recorded_chan, &omxvdec->chan_list, chan_list)
            {
                if (recorded_chan->channel_id == channel_id) {
                    channel_id++;
                    break;
                }
            }
        } while (record_id != channel_id);
    }

    pchan->channel_id = channel_id;

    list_add_tail(&pchan->chan_list, &omxvdec->chan_list);
    osal_spin_unlock_irqrestore(&omxvdec->channel_lock, &flags);

    omxvdec->total_chan_num++;
    omx_print(OMX_INFO, " channel %d create successfully!\n", channel_id);

    return HI_SUCCESS;
}

static hi_s32 channel_delete_chan_record(OMXVDEC_ENTRY *omxvdec, OMXVDEC_CHAN_CTX *pchan)
{
    unsigned long flags;

    if (omxvdec == HI_NULL || pchan == HI_NULL) {
        omx_print(OMX_FATAL, "param invalid!\n");
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&omxvdec->channel_lock, &flags);
    list_del(&pchan->chan_list);
    osal_spin_unlock_irqrestore(&omxvdec->channel_lock, &flags);

    omxvdec->total_chan_num--;

    omx_print(OMX_INFO, "channel %d release successfully!\n", pchan->channel_id);

    return HI_SUCCESS;
}

static hi_s32 channel_ion_map_viraddr(omxvdec_ion_buf *p_ion_buf)
{
    hi_u8 *pu8_vir_addr = HI_NULL;
    hi_void *buf_hhd = HI_NULL;
    if (p_ion_buf == HI_NULL) {
        omx_print(OMX_FATAL, "input null param\n");
        return HI_FAILURE;
    }

    buf_hhd = osal_mem_handle_get(p_ion_buf->buf_fd, HI_ID_VDEC);
    if (IS_ERR(buf_hhd)) {
        omx_print(OMX_FATAL, "dma_buf_get failed!\n");
        return HI_FAILURE;
    }

    osal_mem_flush(buf_hhd);

    pu8_vir_addr = osal_mem_kmap(buf_hhd, 0, 0); /* non-cacheable */
    if (pu8_vir_addr == HI_NULL) {
        omx_print(OMX_FATAL, "dma_buf_get return HI_NULL!! fd = 0x%x\n", p_ion_buf->buf_fd);
        goto err1;
    }

    p_ion_buf->pu8_dma_buf = (hi_u8 *)buf_hhd;
    p_ion_buf->pu8_start_vir_addr = pu8_vir_addr;

    omx_print(OMX_INFO, "map success! dmabuf = %p, vir = %p\n", buf_hhd, pu8_vir_addr);

    return HI_SUCCESS;

err1:
    dma_buf_end_cpu_access(buf_hhd, 0);
    osal_mem_ref_put(buf_hhd, HI_ID_VDEC);

    return HI_FAILURE;
}

static hi_void channel_ion_unmap_viraddr(omxvdec_ion_buf *p_ion_buf)
{
    hi_void *buf_hhd = HI_NULL;

    if (p_ion_buf == HI_NULL) {
        omx_print(OMX_FATAL, "input null parm\n");
        return;
    }

    buf_hhd = (hi_void *)p_ion_buf->pu8_dma_buf;
    if (buf_hhd == HI_NULL) {
        omx_print(OMX_FATAL, "buf_hhd is null\n");
        return;
    }

    osal_mem_kunmap(buf_hhd, (hi_void *)p_ion_buf->pu8_start_vir_addr, 0);

    dma_buf_end_cpu_access(buf_hhd, 0);

    osal_mem_ref_put(buf_hhd, HI_ID_VDEC);

    return;
}

static hi_s32 channel_map_kernel_viraddr(OMXVDEC_CHAN_CTX *pchan, omxvdec_buf *pbuf, OMXVDEC_BUF_DESC *puser_buf,
                                         hi_void **kern_vaddr)
{
    hi_s32 ret;
    omxvdec_buffer st_buffer = { 0 };
    omxvdec_ion_buf st_omx_vdec_meta_priv;
    HI_CHECK_SEC_FUNC(memset_s(&st_buffer, sizeof(st_buffer), 0, sizeof(omxvdec_buffer)));

    if (puser_buf->buffer_type == OMX_USE_NATIVE) {
        HI_CHECK_SEC_FUNC(memset_s(&st_omx_vdec_meta_priv,  sizeof(st_omx_vdec_meta_priv),  0,
                                   sizeof(omxvdec_ion_buf)));

        st_omx_vdec_meta_priv.buf_fd = puser_buf->fd;
        st_omx_vdec_meta_priv.offset = 0;

        ret = channel_ion_map_viraddr(&st_omx_vdec_meta_priv);
        if (ret != HI_SUCCESS) {
            omx_print(OMX_ERR, "failed! fd = 0x%x, phyaddr = 0x%x\n", puser_buf->fd, puser_buf->phyaddr);
        }

        *kern_vaddr = (hi_void *)st_omx_vdec_meta_priv.pu8_start_vir_addr;
        HI_CHECK_SEC_FUNC(memcpy_s(&pbuf->frame_ion_buffer,  sizeof(pbuf->frame_ion_buffer),  &st_omx_vdec_meta_priv,
                                   sizeof(omxvdec_ion_buf)));
    } else if ((puser_buf->buffer_type == OMX_ALLOCATE_SECURE) || (puser_buf->buffer_type == OMX_USE_SECURE)) {
        // do nothing
    } else if (puser_buf->buffer_type == OMX_USER_BIGBUF) {
        if (puser_buf->is_first_buf != HI_TRUE) {
            *kern_vaddr = (hi_void *)(pchan->in_buf_start_vir + (puser_buf->phyaddr - pchan->in_buf_start_phy));
            return HI_SUCCESS;
        }
        st_buffer.phys_addr = puser_buf->phyaddr;

        ret = hi_drv_omxvdec_map(&st_buffer, 0);
        if (ret != HI_SUCCESS) {
            omx_print(OMX_FATAL, "%s call mem_map failed!\n", __func__);

            return HI_FAILURE;
        }

        pchan->in_buf_start_vir = *kern_vaddr = (hi_void *)(st_buffer.virt_addr);
        pchan->in_buf_start_phy = puser_buf->phyaddr;
    } else {
        st_buffer.phys_addr = puser_buf->phyaddr;
        st_buffer.dma_buf = puser_buf->dma_buf;

        ret = hi_drv_omxvdec_map(&st_buffer, 0);
        if (ret != HI_SUCCESS) {
            omx_print(OMX_FATAL, "call mem_map failed!\n");
            return HI_FAILURE;
        }
        *kern_vaddr = (hi_void *)st_buffer.virt_addr;
    }

    return HI_SUCCESS;
}

static hi_s32 channel_unmap_kernel_viraddr(omxvdec_buf *puser_buf)
{
    omxvdec_buffer st_buffer = { 0 };
    HI_CHECK_SEC_FUNC(memset_s(&st_buffer, sizeof(st_buffer), 0, sizeof(omxvdec_buffer)));

    if ((puser_buf->buf_type != OMX_USE_NATIVE)
        && (puser_buf->buf_type != OMX_ALLOCATE_SECURE)
        && (puser_buf->buf_type != OMX_USE_SECURE)
        && ((puser_buf->buf_type != OMX_USER_BIGBUF) || (puser_buf->is_first_buf == HI_TRUE))) {
        st_buffer.virt_addr = (hi_u8 *)puser_buf->kern_vaddr;
        st_buffer.phys_addr = puser_buf->phy_addr;
        st_buffer.dma_buf = puser_buf->dma_buf;
        hi_drv_omxvdec_unmap(&st_buffer);
    } else if (puser_buf->buf_type == OMX_USE_NATIVE) {
        channel_ion_unmap_viraddr(&puser_buf->frame_ion_buffer);
    }

    return HI_SUCCESS;
}

static omxvdec_buf *channel_lookup_addr_table(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf)
{
    hi_s32 i = 0;
    hi_s32 *num_of_buffers = HI_NULL;
    hi_u32 tmp_addr = 0;
    omxvdec_buf *buf_addr_table = HI_NULL;

    if (pchan == HI_NULL || puser_buf == HI_NULL) {
        omx_print(OMX_FATAL, "param invalid!\n");
        return HI_NULL;
    }

    if (puser_buf->dir == PORT_DIR_INPUT) {
        buf_addr_table = (omxvdec_buf *)pchan->in_buf_table;
        num_of_buffers = &pchan->input_buf_num;
    } else if (puser_buf->dir == PORT_DIR_OUTPUT) {
        buf_addr_table = (omxvdec_buf *)pchan->out_buf_table;
        num_of_buffers = &pchan->output_buf_num;
    } else {
        omx_print(OMX_ERR, "buf dir invalid!\n");
        return HI_NULL;
    }

    if (puser_buf->from_work_queue == HI_FALSE) {
        if (channel_get_phy_from_fd(pchan, puser_buf) != HI_SUCCESS) {
            return HI_NULL;
        }
    }

    if (pchan->anw_store_meta == HI_TRUE && puser_buf->dir == PORT_DIR_OUTPUT) {
        for (i = 0; i < *num_of_buffers; i++) {
            if (puser_buf->client_data == (uintptr_t)(buf_addr_table[i].client_data)) {
                break;
            }
        }
    } else {
        for (i = 0; i < *num_of_buffers; i++) {
            tmp_addr = buf_addr_table[i].phy_addr;
            if (puser_buf->phyaddr == tmp_addr) {
                break;
            }
        }
    }

    if (i < *num_of_buffers) {
        // in alloc usr path, insert in table before map user addr, so update user addr here
        buf_addr_table[i].user_vaddr = (hi_void *)(uintptr_t)puser_buf->bufferaddr;
        return &buf_addr_table[i];
    }

    omx_print(OMX_ERR, "buffer(0x%08x) not found!\n", puser_buf->phyaddr);

    return HI_NULL;
}

static hi_void channel_insert_addr_table_process(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf,
                                                 omxvdec_buf *pbuf)
{
    hi_s32 ret = HI_FAILURE;
    omxvdec_buffer st_buffer;
    HI_CHECK_SEC_FUNC(memset_s(&st_buffer, sizeof(st_buffer), 0, sizeof(omxvdec_buffer)));

    if (puser_buf->dir == PORT_DIR_INPUT) {
        omx_print(OMX_INBUF, "insert input buffer, PhyAddr = 0x%08x, success!\n", puser_buf->phyaddr);
    } else {
        pchan->is_overlay = puser_buf->is_overlay;
        omx_print(OMX_OUTBUF, "insert output buffer, PhyAddr = 0x%08x, priv_phy:0x%x success!\n", puser_buf->phyaddr,
                  pbuf->private_phy_addr);
    }

    if (!(pchan->is_overlay == HI_TRUE && puser_buf->dir == PORT_DIR_OUTPUT)) {
        pbuf->private_vaddr = HI_NULL;
        goto EXIT;
    }
    if (puser_buf->meta_type == OMX_ALLOCATE_WITH_META) {  // gstpath
        st_buffer.phys_addr = puser_buf->private_phyaddr;
        ret = hi_drv_omxvdec_map(&st_buffer, 0);
        if (ret != HI_SUCCESS) {
            omx_print(OMX_FATAL, "%s call mem_map failed!\n", __func__);
        }
        pbuf->private_vaddr = st_buffer.virt_addr;
        omx_print(OMX_OUTBUF,
                  "insert addr: framevir(0x%p),framephy(0x%x),priphy(0x%x),privir(0x%p), kernelvir(0x%p)\n",
                  pbuf->user_vaddr, pbuf->phy_addr, pbuf->private_phy_addr, pbuf->private_vaddr, pbuf->kern_vaddr);

        return;
    } else {
        hi_s32 ret = HI_FAILURE;
        omxvdec_ion_buf st_omx_vdec_meta_priv;
        HI_CHECK_SEC_FUNC(memset_s(&st_omx_vdec_meta_priv,  sizeof(st_omx_vdec_meta_priv),  0,
                                   sizeof(omxvdec_ion_buf)));

        st_omx_vdec_meta_priv.buf_fd = puser_buf->private_fd;
        st_omx_vdec_meta_priv.offset = puser_buf->private_offset;

        ret = channel_ion_map_viraddr(&st_omx_vdec_meta_priv);
        if (ret != HI_SUCCESS) {
            omx_print(OMX_ERR, "channel_ion_map_viraddr fd = 0x%x, phyaddr = 0x%x,failed!\n", puser_buf->private_fd,
                      puser_buf->phyaddr);
        }

        pbuf->private_vaddr = (hi_void *)st_omx_vdec_meta_priv.pu8_start_vir_addr + st_omx_vdec_meta_priv.offset;

        HI_CHECK_SEC_FUNC(memcpy_s(&pbuf->ion_buffer,  sizeof(pbuf->ion_buffer),  &st_omx_vdec_meta_priv,
                                   sizeof(omxvdec_ion_buf)));
        return;
    }

EXIT:
    if (puser_buf->meta_type == OMX_USE_WITH_HFBC) {
        hi_s32 ret = HI_FAILURE;
        omxvdec_ion_buf omx_vdec_hfbc_priv;
        HI_CHECK_SEC_FUNC(memset_s(&omx_vdec_hfbc_priv,  sizeof(omx_vdec_hfbc_priv),  0,
                                   sizeof(omxvdec_ion_buf)));

        omx_vdec_hfbc_priv.buf_fd = puser_buf->private_fd;
        omx_vdec_hfbc_priv.offset = puser_buf->hfbc_offset;

        ret = channel_ion_map_viraddr(&omx_vdec_hfbc_priv);
        if (ret != HI_SUCCESS) {
            omx_print(OMX_ERR, "channel_ion_map_viraddr fd = 0x%x failed!\n", puser_buf->private_fd);
        }

        pbuf->hfbc_vaddr = (hi_void *)omx_vdec_hfbc_priv.pu8_start_vir_addr + omx_vdec_hfbc_priv.offset;

        omx_print(OMX_ERR, "hfbc_fd = %d  hfbc_vaddr = %p\n", puser_buf->private_fd, pbuf->hfbc_vaddr);

        HI_CHECK_SEC_FUNC(memcpy_s(&pbuf->hfbc_ion_buffer,  sizeof(pbuf->hfbc_ion_buffer),  &omx_vdec_hfbc_priv,
                                   sizeof(omxvdec_ion_buf)));
    }

    return;
}

static hi_u32 channel_insert_addr_table(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf)
{
    hi_s32 i;
    hi_void *kern_vaddr = HI_NULL;
    hi_u32 phyaddr;
    hi_s32 *buffer_num = HI_NULL;

    omxvdec_buf *buf_addr_table = HI_NULL;
    omxvdec_buf *pbuf = HI_NULL;

    VDEC_DOWN_INTERRUPTIBLE(&pchan->chan_sema);

    if ((pchan->anw_store_meta == HI_FALSE) ||
        (pchan->anw_store_meta == HI_TRUE && puser_buf->dir == PORT_DIR_INPUT)) {
        if (channel_get_phy_from_fd(pchan, puser_buf) != HI_SUCCESS) {
            return HI_FAILURE;
        }
    }

    if (puser_buf->dir == PORT_DIR_INPUT) {
        omx_print(OMX_INBUF, "insert input buffer, phy addr = 0x%08x\n", puser_buf->phyaddr);
        pchan->max_in_buf_num = puser_buf->max_num;
        buf_addr_table = (omxvdec_buf *)pchan->in_buf_table;
        buffer_num = &pchan->input_buf_num;

        if (*buffer_num >= pchan->max_in_buf_num) {
            VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);
            omx_print(OMX_ERR, "number of buffers reached max in value(%d)\n", pchan->max_in_buf_num);

            return HI_FAILURE;
        }
        if (decoder_command_handler(pchan, DEC_CMD_BIND_ES_BUF, puser_buf, 0) != HI_SUCCESS) {
            omx_print(OMX_ERR, "BIND_ES_BUF fail \n");
        }

        puser_buf->is_first_buf = (*buffer_num == 0) ? HI_TRUE : HI_FALSE;
    } else if (puser_buf->dir == PORT_DIR_OUTPUT) {
        omx_print(OMX_OUTBUF, "insert output buffer, phy addr = 0x%08x\n", puser_buf->phyaddr);
        pchan->max_out_buf_num = puser_buf->max_num;
        buf_addr_table = (omxvdec_buf *)pchan->out_buf_table;
        buffer_num = &pchan->output_buf_num;

        if (*buffer_num >= pchan->max_out_buf_num) {
            VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);
            omx_print(OMX_ERR, "buffer num(%d) reached max out value(%d)\n", *buffer_num, pchan->max_out_buf_num);

            return HI_FAILURE;
        }
    } else {
        VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);
        omx_print(OMX_ERR, "buffer dir(%d) invalid!\n", puser_buf->dir);

        return HI_FAILURE;
    }

    if (pchan->anw_store_meta == HI_FALSE) {
        phyaddr = puser_buf->phyaddr;
        i = 0;
        while ((i < *buffer_num) && (phyaddr != buf_addr_table[i].phy_addr)) {
            i++;
        }
    } else {
        i = 0;
        while ((i < *buffer_num) &&
               ((hi_void *)(uintptr_t)puser_buf->client_data != buf_addr_table[i].client_data)) {
            i++;
        }
    }

    if (i < *buffer_num) {
        VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);
        omx_print(OMX_ERR, "phyaddr = 0x%x already insert\n", phyaddr);

        return HI_FAILURE;
    }

    pbuf = buf_addr_table + *buffer_num;

    // vir addr is not necessary for output buffer
    if (puser_buf->dir == PORT_DIR_INPUT || g_map_frm_enable) {
        /* get kernel virtual address */
        if (channel_map_kernel_viraddr(pchan, pbuf, puser_buf, &kern_vaddr) != HI_SUCCESS) {
            VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);
            omx_print(OMX_FATAL, "get_addr failed (phyaddr: 0x%x)\n", phyaddr);

            return HI_FAILURE;
        }
    }

    pbuf->user_vaddr = (hi_void *)(uintptr_t)puser_buf->bufferaddr;
    pbuf->phy_addr = puser_buf->phyaddr;
    pbuf->dma_buf = puser_buf->dma_buf;
    pbuf->fd = puser_buf->fd;
    if (puser_buf->buffer_type == OMX_USE_NATIVE && puser_buf->fd != 0) {
        pbuf->fd = puser_buf->fd;
        pbuf->dma_buf = PTR_UINT64(osal_mem_handle_get(pbuf->fd, HI_ID_VDEC));
        osal_mem_ref_put(UINT64_PTR(pbuf->dma_buf), HI_ID_VDEC);
    }

    pbuf->private_fd = puser_buf->private_fd;
    pbuf->private_phy_addr = puser_buf->private_phyaddr;
    pbuf->kern_vaddr = kern_vaddr;
    pbuf->client_data = (hi_void *)(uintptr_t)puser_buf->client_data;
    pbuf->buf_len = puser_buf->buffer_len;
    pbuf->act_len = 0;
    pbuf->private_len = puser_buf->private_len;
    pbuf->offset = 0;
    pbuf->buf_type = puser_buf->buffer_type;
    pbuf->status = BUF_STATE_IDLE;
    pbuf->buf_id = *buffer_num;
    pbuf->is_first_buf = puser_buf->is_first_buf;

    *buffer_num += 1;

    if ((pchan->anw_store_meta == HI_FALSE) ||
        (pchan->anw_store_meta == HI_TRUE && puser_buf->dir == PORT_DIR_INPUT)) {
        channel_insert_addr_table_process(pchan, puser_buf, pbuf);
    }

    VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);

    return HI_SUCCESS;
}

static hi_void channel_delete_addr_table_list(OMXVDEC_CHAN_CTX *pchan, omxvdec_buf *pbuf,
                                              OMXVDEC_BUF_DESC *puser_buf, hi_u32 buf_id)
{
    hi_s32 is_find = 0;
    hi_u32 *buffer_num = HI_NULL;
    osal_spinlock *p_lock = HI_NULL;
    struct list_head *p_queue = HI_NULL;
    unsigned long flags = 0;
    omxvdec_buf *p_qbuf = HI_NULL;
    omxvdec_buf *ptmp = HI_NULL;
    omxvdec_buf *buf_addr_table = HI_NULL;

    if (puser_buf->dir == PORT_DIR_INPUT) {
        omx_print(OMX_INBUF, "delete input buffer, phy addr = 0x%08x\n", puser_buf->phyaddr);
        buf_addr_table = pchan->in_buf_table;
        buffer_num = &pchan->input_buf_num;
        p_lock = &pchan->raw_lock;
        p_queue = &pchan->raw_queue;
    } else if (puser_buf->dir == PORT_DIR_OUTPUT) {
        omx_print(OMX_OUTBUF, "delete output buffer, phy addr = 0x%08x\n", puser_buf->phyaddr);
        buf_addr_table = pchan->out_buf_table;
        buffer_num = &pchan->output_buf_num;
        p_lock = &pchan->yuv_lock;
        p_queue = &pchan->yuv_queue;
    }
    if (buffer_num == HI_NULL) {
        omx_print(OMX_FATAL, "%s buffer_num null error!\n", __func__);
        return;
    }

    if (buf_id < (*buffer_num - 1)) {
        osal_spin_lock_irqsave(p_lock, &flags);
        if (!list_empty(p_queue)) {
            list_for_each_entry_safe(p_qbuf, ptmp, p_queue, list)
            {
                if (pbuf->phy_addr == p_qbuf->phy_addr) {
                    /* delete old node in list */
                    list_del(&p_qbuf->list);
                    break;
                }
            }
        }

        HI_CHECK_SEC_FUNC(memcpy_s(pbuf,  sizeof(omxvdec_buf),  &buf_addr_table[*buffer_num - 1],
                                   sizeof(omxvdec_buf)));
        pbuf->buf_id = buf_id;

        if (!list_empty(p_queue)) {
            list_for_each_entry_safe(p_qbuf, ptmp, p_queue, list)
            {
                if (buf_addr_table[*buffer_num - 1].phy_addr == (p_qbuf->phy_addr)) {
                    is_find = 1;
                    /* delete old node in list */
                    list_del(&p_qbuf->list);
                    break;
                }
            }

            /* insert new node into list */
            if (is_find) {
                list_add_tail(&pbuf->list, p_queue);
            }
        }
        osal_spin_unlock_irqrestore(p_lock, &flags);
    }

    HI_CHECK_SEC_FUNC(memset_s(&buf_addr_table[*buffer_num - 1],  sizeof(buf_addr_table[*buffer_num - 1]),  0,
                               sizeof(omxvdec_buf)));
    *buffer_num = *buffer_num - 1;

    return;
}

static hi_u32 channel_delete_addr_table(OMXVDEC_CHAN_CTX *pchan,
    OMXVDEC_BUF_DESC *puser_buf, omx_mem_free_type free_type)
{
    hi_u32 i = 0;
    hi_u32 phyaddr;
    osal_spinlock *p_lock = HI_NULL;
    hi_u32 *buffer_num = HI_NULL;
    omxvdec_buf *pbuf = HI_NULL;
    struct list_head *p_queue = HI_NULL;
    omxvdec_buf *buf_addr_table = HI_NULL;
    omxvdec_buffer st_buffer;

    HI_CHECK_SEC_FUNC(memset_s(&st_buffer, sizeof(st_buffer), 0, sizeof(omxvdec_buffer)));

    if ((pchan->anw_store_meta == HI_FALSE) ||
        (pchan->anw_store_meta == HI_TRUE && puser_buf->dir == PORT_DIR_INPUT)) {
        if (free_type == OMX_MEM_FREE_TYPE_NORMAL && channel_get_phy_from_fd(pchan, puser_buf) != HI_SUCCESS) {
            return HI_FAILURE;
        }
    }

    if (puser_buf->dir == PORT_DIR_INPUT) {
        omx_print(OMX_INBUF, "delete input buffer, phy addr = 0x%08x\n", puser_buf->phyaddr);
        buf_addr_table = pchan->in_buf_table;
        buffer_num = &pchan->input_buf_num;
        p_lock = &pchan->raw_lock;
        p_queue = &pchan->raw_queue;
    } else if (puser_buf->dir == PORT_DIR_OUTPUT) {
        omx_print(OMX_OUTBUF, "delete output buffer, phy addr = 0x%08x\n", puser_buf->phyaddr);
        buf_addr_table = pchan->out_buf_table;
        buffer_num = &pchan->output_buf_num;
        p_lock = &pchan->yuv_lock;
        p_queue = &pchan->yuv_queue;
    }

    if (buffer_num == HI_NULL || buf_addr_table == HI_NULL || p_lock == HI_NULL || p_queue == HI_NULL) {
        omx_print(OMX_ERR, "table is empty!!\n");
        return HI_FAILURE;
    }

    phyaddr = puser_buf->phyaddr;
    if (pchan->anw_store_meta == HI_FALSE) {
        i = 0;
        while ((i < *buffer_num) && (phyaddr != buf_addr_table[i].phy_addr)) {
            i++;
        }
    } else {
        i = 0;
        while ((i < *buffer_num) &&
               ((hi_void *)(uintptr_t)puser_buf->client_data != buf_addr_table[i].client_data)) {
            i++;
        }
    }

    if (i == *buffer_num) {
        omx_print(OMX_ERR, "phyaddr = 0x%x not found", phyaddr);
        return HI_FAILURE;
    }

    pbuf = &buf_addr_table[i];

    if (pbuf->status == BUF_STATE_USING) {
        omx_print(OMX_ERR, "WARN: buffer 0x%x still in use?!\n", phyaddr);
        // not exit, force continue
    }

    if (puser_buf->dir == PORT_DIR_INPUT || g_map_frm_enable) {
        /* unmap kernel virtual address */
        channel_unmap_kernel_viraddr(pbuf);
    }

    if ((pbuf->private_vaddr != HI_NULL) && (pchan->is_overlay == HI_TRUE)) {
        if ((puser_buf->meta_type == OMX_ALLOCATE_WITH_META) && (puser_buf->dir == PORT_DIR_OUTPUT)) {  // gstpath
            st_buffer.virt_addr = pbuf->private_vaddr;
            st_buffer.phys_addr = pbuf->private_phy_addr;
            hi_drv_omxvdec_unmap(&st_buffer);
        } else {
            channel_ion_unmap_viraddr(&pbuf->ion_buffer);
        }
        pbuf->private_vaddr = HI_NULL;
    }

    if ((pbuf->hfbc_vaddr != HI_NULL) && (pchan->is_overlay == HI_FALSE)) {
        if ((puser_buf->meta_type == OMX_USE_WITH_HFBC) && (puser_buf->dir == PORT_DIR_OUTPUT)) {
            channel_ion_unmap_viraddr(&pbuf->hfbc_ion_buffer);
        } else {
            ;
        }
        pbuf->hfbc_vaddr = HI_NULL;
    }

    pbuf->kern_vaddr = HI_NULL;

    channel_delete_addr_table_list(pchan, pbuf, puser_buf, i);

    omx_print(OMX_INBUF, "delete buffer, phy addr = 0x%08x, done!\n", puser_buf->phyaddr);

    return HI_SUCCESS;
}

hi_s32 channel_insert_in_raw_list(OMXVDEC_CHAN_CTX *pchan, omxvdec_buf *pstream)
{
    hi_s32 is_find = 0;
    unsigned long flags;
    omxvdec_buf *pbuf = HI_NULL;

    osal_spin_lock_irqsave(&pchan->raw_lock, &flags);
    if (!(list_empty(&pchan->raw_queue))) {
        list_for_each_entry(pbuf, &pchan->raw_queue, list)
        {
            if (pstream->phy_addr == pbuf->phy_addr) {
                is_find = 1;
                break;
            }
        }

        if (is_find) {
            omx_print(OMX_ERR, "0x%x already in raw list!\n", pstream->phy_addr);
            osal_spin_unlock_irqrestore(&pchan->raw_lock, &flags);
            return HI_FAILURE;
        }
    }

    omx_print(OMX_INBUF, "empty this buffer, phyaddr: 0x%08x, data_len: %d\n", pstream->phy_addr, pstream->act_len);

    /* insert the stream packet to raw queue */
    pstream->status = BUF_STATE_QUEUED;
    list_add_tail(&pstream->list, &pchan->raw_queue);

    osal_spin_unlock_irqrestore(&pchan->raw_lock, &flags);

    return HI_SUCCESS;
}

hi_s32 channel_insert_in_yuv_list(OMXVDEC_CHAN_CTX *pchan, omxvdec_buf *pframe)
{
    hi_s32 is_find = 0;
    unsigned long flags;
    omxvdec_buf *pbuf = HI_NULL;

    osal_spin_lock_irqsave(&pchan->yuv_lock, &flags);
    if (!(list_empty(&pchan->yuv_queue))) {
        list_for_each_entry(pbuf, &pchan->yuv_queue, list)
        {
            if (pchan->anw_store_meta == HI_TRUE) {
                if (pframe->client_data == pbuf->client_data) {
                    is_find = 1;
                    break;
                }
            } else {
                if (pframe->phy_addr == pbuf->phy_addr) {
                    is_find = 1;
                    break;
                }
            }
        }

        if (is_find) {
            omx_print(OMX_ERR, "0x%x already in yuv list!\n", pframe->phy_addr);
            osal_spin_unlock_irqrestore(&pchan->yuv_lock, &flags);
            return HI_FAILURE;
        }
    }

    omx_print(OMX_OUTBUF, "fill this buffer, phyaddr:0x%08x\n", pframe->phy_addr);

    pframe->status = BUF_STATE_QUEUED;
    list_add_tail(&pframe->list, &pchan->yuv_queue);

    osal_spin_unlock_irqrestore(&pchan->yuv_lock, &flags);

    return HI_SUCCESS;
}

static hi_s32 channel_add_last_frame(OMXVDEC_CHAN_CTX *pchan, omxvdec_buf *pstream)
{
    if (pchan == HI_NULL || pstream == HI_NULL) {
        omx_print(OMX_ERR, "param invalid!\n");
        return HI_FAILURE;
    }

    pstream->act_len = pstream->buf_len;
    pstream->offset = 0;
    pstream->flags = VFMW_OMX_STM_FLAG_ENDOFFRAME;
    pstream->time_stamp = HI_INVALID_PTS;
    pstream->buf_id = LAST_FRAME_BUF_ID;

    return HI_SUCCESS;
}

static hi_void channel_save_stream(OMXVDEC_CHAN_CTX *pchan, omxvdec_buf *pstream)
{
    hi_s32 len;
    mm_segment_t oldfs;

    if (g_save_raw_file == HI_NULL || g_save_raw_chan_num != pchan->channel_id) {
        return;
    }

    omx_print(OMX_ALWS, "saving raw data vir:0x%p len:0x%x\n", pstream->kern_vaddr, pstream->act_len);

    oldfs = get_fs();
    set_fs(KERNEL_DS);
    len = vfs_write(g_save_raw_file, pstream->kern_vaddr, pstream->act_len, &g_save_raw_file->f_pos);
    if (len != pstream->act_len) {
        omx_print(OMX_ALWS, "saving raw data of inst_%d, save length(%d)!= act len(%d)\n",
                  pchan->channel_id, len, pstream->act_len);
    }

    vfs_fsync(g_save_raw_file, 0);
    set_fs(oldfs);

    return;
}

/* ============== EXPORT INTERFACE ============= */
hi_s32 channel_init(hi_void)
{
    hi_s32 ret;

    FUNC_IN();

    ret = decoder_init();
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "init decoder failed!\n");
        return HI_FAILURE;
    }

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_void channel_exit(hi_void)
{
    hi_s32 ret;

    FUNC_IN();

    ret = decoder_exit();
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "exit decoder failed!\n");
    }

    FUNC_EXIT();

    return;
}

static hi_void channel_vdec_report_process_start_done(OMXVDEC_CHAN_CTX *pchan, hi_u32 *pstatus,
                                                      const hi_void *priv_data)
{
    unsigned long flags;

    osal_spin_lock_irqsave(&pchan->chan_lock, &flags);
    if (pchan->state == CHAN_STATE_WORK) {
        omx_print(OMX_ERR, "already in work state!\n");
    } else {
        *pstatus = VDEC_S_SUCCESS;
        pchan->state = CHAN_STATE_WORK;
        pchan->decoder_id = *((hi_s32 *)priv_data);
    }
    osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags);

    return;
}

static hi_s32 channel_vdec_report_process(OMXVDEC_CHAN_CTX *pchan, hi_u32 event_id, hi_u32 *pstatus,
                                          hi_void *priv_data)
{
    unsigned long flags;
    OMXVDEC_IMG_SIZE *pimage_info = HI_NULL;

    switch (event_id) {
        case VDEC_MSG_RESP_START_DONE: {
            if (priv_data == HI_NULL) {
                omx_print(OMX_FATAL, "param invalid\n");

                return HI_FAILURE;
            }

            channel_vdec_report_process_start_done(pchan, pstatus, priv_data);

            break;
        }
        case VDEC_MSG_RESP_STOP_DONE: {
            osal_spin_lock_irqsave(&pchan->chan_lock, &flags);
            if (pchan->state == CHAN_STATE_IDLE) {
                *pstatus = VDEC_ERR_BAD_STATE;
                omx_print(OMX_FATAL, "already stop!\n");
            } else {
                *pstatus = VDEC_S_SUCCESS;
                pchan->state = CHAN_STATE_IDLE;
                pchan->decoder_id = -1;
            }
            osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags);
            break;
        }

        case VDEC_EVT_REPORT_IMG_SIZE_CHG: {
            if (priv_data == HI_NULL) {
                omx_print(OMX_FATAL, "param invalid\n");

                return HI_FAILURE;
            }

            osal_spin_lock_irqsave(&pchan->chan_lock, &flags);

            pimage_info = (OMXVDEC_IMG_SIZE *)priv_data;

            pchan->out_width = pimage_info->width;
            pchan->out_height = pimage_info->height;
            pchan->out_stride = pimage_info->stride;
            pimage_info->min_num = pimage_info->max_num - 3; /* 3 : g_client_reserved_num; */

            channel_report_message(pchan, VDEC_EVT_REPORT_IMG_SIZE_CHG, (hi_void *)pimage_info);
            osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags);
            return HI_SUCCESS;
        }

        default:
            *pstatus = VDEC_S_SUCCESS;
            break;
    }

    return HI_SUCCESS;
}

hi_s32 channel_vdec_report(hi_s32 chan_id, hi_u32 event_id, hi_s32 result, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;
    OMXVDEC_CHAN_CTX *pchan = HI_NULL;
    hi_u32 status = VDEC_S_FAILED;

    pchan = channel_find_inst_by_channel_id(g_omx_vdec, chan_id);
    if (pchan == HI_NULL) {
        omx_print(OMX_FATAL, "can't find chan(%d).\n", chan_id);
        return HI_FAILURE;
    }

    VDEC_DOWN_INTERRUPTIBLE(&pchan->chan_sema);

    if (!pchan->tunnel_mode_enable) {
        VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);
        omx_print(OMX_FATAL, "chan(%d) is NOT tunnel mode\n", chan_id);
        return HI_FAILURE;
    }

    if (result == HI_SUCCESS) {
        ret = channel_vdec_report_process(pchan, event_id, &status, private_data);
        if (ret != HI_SUCCESS) {
            VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);
            omx_print(OMX_FATAL, "chan:%d report process failed\n", chan_id);

            return HI_FAILURE;
        }
    } else {
        status = VDEC_S_FAILED;
    }

    message_queue(pchan->msg_queue, event_id, status, HI_NULL);

    VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);

    return HI_SUCCESS;
}

hi_void channel_init_inst_config(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_DRV_CFG *pcfg)
{
    channel_show_chan_config(pcfg);

#ifdef HI_OMX_TEE_SUPPORT
    pchan->is_tvp = pcfg->is_tvp;
    pchan->ssm_handle = pcfg->ssm_handle;
    pchan->omx_handle = OMX_MAKE_HANDLE(pchan->channel_id);
#endif
    pchan->out_width = pcfg->cfg_width;
    pchan->out_height = pcfg->cfg_height;
    pchan->out_stride = pcfg->cfg_stride;
    pchan->rotation = pcfg->cfg_rotation;
    pchan->b_lowdelay = 0; /* pcfg->chan_cfg.lowdly_enable; todo */
    pchan->m_use_native_buf = pcfg->m_use_native_buf;
    pchan->tunnel_mode_enable = pcfg->tunnel_mode_enable;
    pchan->is_net_work = pcfg->is_net_work;
    pchan->android_test = pcfg->android_test;
    pchan->b_is_on_live = pcfg->b_is_on_live;
    pchan->b_is_gst = pcfg->b_is_gst;
    pchan->b_meta_data_alloc = pcfg->b_meta_data_alloc;
    pchan->color_format = pcfg->cfg_color_format;
    pchan->anw_store_meta = pcfg->anw_store_meta;

    if (pcfg->chan_cfg.vid_std == VFMW_VP6 || pcfg->chan_cfg.vid_std == VFMW_VP6F ||
        pcfg->chan_cfg.vid_std == VFMW_VP6A) {
        pchan->bReversed = pcfg->chan_cfg.std_ext.vp6_ext.reversed;
    } else {
        pchan->bReversed = 0;
    }

    if (pcfg->chan_cfg.vid_std == VFMW_VC1 && pcfg->chan_cfg.std_ext.vc1_ext.is_adv_profile == 0) {
        pchan->protocol = VFMW_VC1;
    } else {
        pchan->protocol = pcfg->chan_cfg.vid_std;
    }

    pchan->src_frame_type = pcfg->src_frame_type;

    pchan->set_corlour_info = pcfg->set_corlour_info;
    if (pchan->set_corlour_info == HI_TRUE) {
        HI_CHECK_SEC_FUNC(memcpy_s(&pchan->st_colour_info,  sizeof(pchan->st_colour_info),  &pcfg->colour_info,
                                   sizeof(hi_drv_color_descript)));
    }

    pchan->set_hdr_static_info = pcfg->set_hdr_static_info;

    if (pchan->set_hdr_static_info == HI_TRUE) {
        HI_CHECK_SEC_FUNC(memcpy_s(&pchan->st_hdr10_info,  sizeof(pchan->st_hdr10_info),  &pcfg->hdr10_info,
                                   sizeof(hi_drv_hdr_static_metadata)));
    }

    return;
}

static hi_s32 channel_alloc_inst_memory(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_DRV_CFG *pcfg)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 table_size;
    hi_u32 total_size;

    if (pcfg->cfg_inbuf_num > MAX_IN_BUF_SLOT_NUM || pcfg->cfg_outbuf_num > MAX_OUT_BUF_SLOT_NUM) {
        omx_print(OMX_FATAL, "%s cfg num is too big! input:%d output:%d\n", __func__, pcfg->cfg_inbuf_num,
                  pcfg->cfg_outbuf_num);
        return HI_FAILURE;
    }

#ifdef HI_OMX_TEE_SUPPORT
    if (pchan->is_tvp == HI_TRUE) {
        table_size = (MAX_IN_BUF_SLOT_NUM + MAX_OUT_BUF_SLOT_NUM) * sizeof(omxvdec_buf);
        total_size = table_size;

#ifdef HI_SMMU_SUPPORT
        pchan->mem_recorder.channel_extra_buf.mode = OMX_MEM_MMU_SEC;
#else
        pchan->mem_recorder.channel_extra_buf.mode = OMX_MEM_CMA_SEC;
#endif

        ret = hi_drv_omxvdec_alloc_and_map("OMXVDEC_EXTRA", 0, total_size, 0,
            &pchan->mem_recorder.channel_extra_buf);
        if (ret != HI_SUCCESS) {
            omx_print(OMX_FATAL, "alloc extra buffer mem failed\n");

            return HI_FAILURE;
        }

        HI_CHECK_SEC_FUNC(memset_s(pchan->mem_recorder.channel_extra_buf.virt_addr,
                                   pchan->mem_recorder.channel_extra_buf.size,
                                   0,  pchan->mem_recorder.channel_extra_buf.size));

        pchan->in_buf_table = (hi_void *)(pchan->mem_recorder.channel_extra_buf.virt_addr);
        pchan->out_buf_table = pchan->in_buf_table + MAX_IN_BUF_SLOT_NUM * sizeof(omxvdec_buf);
        pchan->max_in_buf_num = pcfg->cfg_inbuf_num;
        pchan->max_out_buf_num = pcfg->cfg_outbuf_num;
        pchan->mem_recorder.e_extra_mem_alloc = ALLOC_BY_MMZ;

#ifdef HI_SMMU_SUPPORT
        pchan->mem_recorder.decoder_eos_buf.mode = OMX_MEM_MMU_SEC;
#else
        pchan->mem_recorder.decoder_eos_buf.mode = OMX_MEM_CMA_SEC;
#endif

        ret = hi_drv_omxvdec_alloc("SEC_OMXVDEC_LAST_FRAME", LAST_FRAME_BUF_SIZE,
                                   4, &pchan->mem_recorder.decoder_eos_buf); /* alloc buffer size is 4 */
        if (ret != HI_SUCCESS) {
            omx_print(OMX_FATAL, "alloc sec last frame mem failed\n");
            omxvdec_release_mem(&pchan->mem_recorder.channel_extra_buf, pchan->mem_recorder.e_extra_mem_alloc,
                OMX_MEM_FREE_TYPE_NORMAL);

            return HI_FAILURE;
        }

        pchan->last_frame.kern_vaddr = (hi_void *)(uintptr_t)pchan->mem_recorder.decoder_eos_buf.virt_addr;
        pchan->last_frame.phy_addr = pchan->mem_recorder.decoder_eos_buf.phys_addr;
        pchan->last_frame.buf_len = pchan->mem_recorder.decoder_eos_buf.size;
        pchan->last_frame.frame_rate = pcfg->cfg_framerate;
        pchan->mem_recorder.e_eos_mem_alloc = ALLOC_BY_SEC;
    }
#endif

#ifdef HI_OMX_TEE_SUPPORT
    if (pchan->is_tvp != HI_TRUE) {
#endif
        table_size = (MAX_IN_BUF_SLOT_NUM + MAX_OUT_BUF_SLOT_NUM) * sizeof(omxvdec_buf);
        total_size = LAST_FRAME_BUF_SIZE + 16 + table_size;  // +16 for gap

#ifdef HI_SMMU_SUPPORT
        pchan->mem_recorder.channel_extra_buf.mode = OMX_MEM_MMU_MMU;
#else
        pchan->mem_recorder.channel_extra_buf.mode = OMX_MEM_CMA_CMA;
#endif

        ret = hi_drv_omxvdec_alloc_and_map("OMXVDEC_EXTRA", 0, total_size, 0,
                                           &pchan->mem_recorder.channel_extra_buf);
        if (ret != HI_SUCCESS) {
            omx_print(OMX_FATAL, "alloc extra buffer mem failed\n");

            return HI_FAILURE;
        }

        HI_CHECK_SEC_FUNC(memset_s(pchan->mem_recorder.channel_extra_buf.virt_addr,
                                   pchan->mem_recorder.channel_extra_buf.size,
                                   0,  pchan->mem_recorder.channel_extra_buf.size));

        pchan->last_frame.phy_addr = pchan->mem_recorder.channel_extra_buf.phys_addr;
        pchan->last_frame.kern_vaddr = (hi_void *)pchan->mem_recorder.channel_extra_buf.virt_addr;
        pchan->last_frame.buf_len = LAST_FRAME_BUF_SIZE;
        pchan->last_frame.frame_rate = pcfg->cfg_framerate;

        pchan->in_buf_table = (hi_void *)(pchan->mem_recorder.channel_extra_buf.virt_addr +
            LAST_FRAME_BUF_SIZE + 16); /* 16  is offset size */
        pchan->out_buf_table = pchan->in_buf_table + MAX_IN_BUF_SLOT_NUM * sizeof(omxvdec_buf);
        pchan->max_in_buf_num = pcfg->cfg_inbuf_num;
        pchan->max_out_buf_num = pcfg->cfg_outbuf_num;
        pchan->mem_recorder.e_extra_mem_alloc = ALLOC_BY_MMZ;
        pchan->omx_vdec_lowdelay_proc_rec = HI_NULL;

        if (pchan->b_lowdelay || g_fast_output_mode) {
            pchan->omx_vdec_lowdelay_proc_rec = hi_kmalloc_omxvdec(sizeof(OMXVDEC_LOWDELAY_PROC_RECORD));
            if (pchan->omx_vdec_lowdelay_proc_rec == HI_NULL) {
                omx_print(OMX_FATAL, "alloc lowdelay_proc_rec failed!\n");
            } else {
                HI_CHECK_SEC_FUNC(memset_s(pchan->omx_vdec_lowdelay_proc_rec,  sizeof(OMXVDEC_LOWDELAY_PROC_RECORD),  0,
                                           sizeof(OMXVDEC_LOWDELAY_PROC_RECORD)));
                pchan->omx_vdec_lowdelay_proc_rec->usrtag_start_time = (omx_get_time_in_ms()) & 0xFF;
                pchan->omx_vdec_lowdelay_proc_rec->interval = ((omx_get_time_in_ms()) & 0xF) + 1;
            }
        }
#ifdef HI_OMX_TEE_SUPPORT
    }
#endif

    return HI_SUCCESS;
}

static hi_void channel_free_inst_memory(OMXVDEC_CHAN_CTX *pchan, omx_mem_free_type free_type)
{
    omxvdec_release_mem(&pchan->mem_recorder.channel_extra_buf, pchan->mem_recorder.e_extra_mem_alloc, free_type);

    // sec:will release  N-sec:the structure is empty,will NOT release actually
    omxvdec_release_mem(&pchan->mem_recorder.decoder_eos_buf, pchan->mem_recorder.e_eos_mem_alloc, free_type);

    if ((pchan->b_lowdelay || g_fast_output_mode) && (pchan->omx_vdec_lowdelay_proc_rec != HI_NULL)) {
        hi_kfree_omxvdec(pchan->omx_vdec_lowdelay_proc_rec);
    }

    return;
}

hi_s32 channel_config_inst(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_DRV_CFG *pcfg)
{
    hi_s32 ret;

    ret = decoder_command_handler(pchan, DEC_CMD_CFG_INST, (hi_void *)pcfg, sizeof(OMXVDEC_DRV_CFG));
    if (ret != HI_SUCCESS) {
        omx_print(OMX_ERR, "config install failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 channel_create_inst(struct file *fd, OMXVDEC_DRV_CFG *pcfg)
{
    hi_s32 ret;
    unsigned long flags;
    OMXVDEC_CHAN_CTX *pchan = HI_NULL;
    OMXVDEC_ENTRY *omxvdec = HI_NULL;
    vfmw_private_frm_info *vdec_private_info = HI_NULL;
    hi_s32 i;

    FUNC_IN();

    /* get omxvdec */
    omxvdec = fd->private_data;
    OMXVDEC_ASSERT_RETURN(omxvdec != HI_NULL, "omxvde = null");

    /* alloc context */
    pchan = hi_kmalloc_omxvdec(sizeof(OMXVDEC_CHAN_CTX));
    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "pchan = null");

    HI_CHECK_SEC_FUNC(memset_s(pchan, sizeof(OMXVDEC_CHAN_CTX), 0, sizeof(OMXVDEC_CHAN_CTX)));

    /* initialize ctx resource */
    VDEC_INIT_MUTEX(&pchan->chan_sema);
    osal_spin_lock_init(&pchan->chan_lock);
    osal_spin_lock_init(&pchan->raw_lock);
    osal_spin_lock_init(&pchan->yuv_lock);

    INIT_LIST_HEAD(&pchan->chan_list);
    INIT_LIST_HEAD(&pchan->raw_queue);
    INIT_LIST_HEAD(&pchan->yuv_queue);

    if (osal_sem_down_interruptible(&pchan->chan_sema)) {
        omx_print(OMX_FATAL, "down_interruptible failed!\n");
        goto cleanup;
    }

    pchan->vdec = omxvdec;
    pchan->file_dec = (hi_u32 *)fd;
    for (i = 0; i < OMXVDEC_MAX_EXT_BUF_NUM; i++) {
        pchan->ext_buf[i].is_valid = -1;
    }

    /* record this channel */
    ret = channel_add_chan_record(omxvdec, pchan);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "add chan record failed\n");
        goto cleanup0;
    }

    /* initialize msg queue */
    pchan->msg_queue = message_queue_init(QUEUE_DEFAULT_DEPTH);
    if (pchan->msg_queue == HI_NULL) {
        omx_print(OMX_FATAL, "call msg_queue_init failed!\n");
        goto cleanup1;
    }

    /* init channel configuration */
    channel_init_inst_config(pchan, pcfg);

    if (pchan->is_tvp) {
        ret = decoder_init_trusted();
        if (ret != HI_SUCCESS) {
            omx_print(OMX_FATAL, "%s call decoder_init_vfmw failed\n", __func__);
            goto cleanup2;
        }
    }

    /* Create sub module */
    if (pcfg->tunnel_mode_enable != 1) {
        ret = decoder_create_inst(pchan, pcfg);
        if (ret != HI_SUCCESS) {
            omx_print(OMX_FATAL, "call channel_create_with_vfmw failed!\n");
            goto cleanup3;
        }
    } else {
        pchan->decoder_id = HI_INVALID_HANDLE;
    }

    pchan->frame = hi_vmalloc_omxvdec(sizeof(hi_drv_video_frame));
    if (pchan->frame == HI_NULL) {
        omx_print(OMX_FATAL, "alloc pchan->frame is null\n");
        goto cleanup4;
    }

    vdec_private_info = hi_vmalloc_omxvdec(sizeof(vfmw_private_frm_info));
    if (vdec_private_info == HI_NULL) {
        omx_print(OMX_FATAL, "alloc vdec_private_info is null\n");
        goto cleanup5;
    }

    pchan->frame->video_private.vdec_private.data = PTR_UINT64(vdec_private_info);

    /* get memory resource */
    ret = channel_alloc_inst_memory(pchan, pcfg);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "call channel_alloc_inst_memory failed!\n");
        goto cleanup5;
    }

    osal_spin_lock_irqsave(&pchan->chan_lock, &flags);
    pchan->state = CHAN_STATE_IDLE;
    osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags);

    VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);

#ifdef HI_DEC_SYNC_SUPPORT
    ret = drv_dec_fence_init(pchan->channel_id, pchan);

    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "omxvdec dec_fence_init failed!\n");
        goto cleanup5;
    }
#endif

    FUNC_EXIT();

    return pchan->channel_id;

cleanup5:
    if (pchan->frame != HI_NULL) {
        hi_vfree_omxvdec(pchan->frame);
    }

cleanup4:
    if (pchan->decoder_id != HI_INVALID_HANDLE) {
        decoder_release_inst(pchan);
    }
cleanup3:
    if (pchan->is_tvp == HI_TRUE) {
        ret = decoder_exit_trusted();
        if (ret != HI_SUCCESS) {
            omx_print(OMX_FATAL, "decoder_exit_trusted failed\n");
        }
    }
cleanup2:
    message_queue_deinit(pchan->msg_queue);
cleanup1:
    ret = channel_delete_chan_record(omxvdec, pchan);
    if (ret == HI_FAILURE) {
        omx_print(OMX_FATAL, "channel_delete_chan_record param invalid!\n");
    }
cleanup0:
    VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);
cleanup:
    hi_kfree_omxvdec(pchan);

    return HI_FAILURE;
}

hi_s32 channel_release_inst(OMXVDEC_CHAN_CTX *pchan, omx_mem_free_type free_type)
{
    hi_u32 i;
    hi_s32 ret = HI_FAILURE;
    unsigned long flags;
    unsigned long flags_chan;
    e_chan_state state;
    OMXVDEC_ENTRY *omxvdec = HI_NULL;
    omxvdec_buffer st_buffer;

    HI_CHECK_SEC_FUNC(memset_s(&st_buffer, sizeof(st_buffer), 0, sizeof(omxvdec_buffer)));

    FUNC_IN();

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "pchan = null");

    VDEC_DOWN_INTERRUPTIBLE(&pchan->chan_sema);

    omxvdec = pchan->vdec;
    if (omxvdec == HI_NULL) {
        omx_print(OMX_FATAL, "omxvdec = HI_NULL\n");
        VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);

        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&omxvdec->stream_lock, &flags);
    osal_spin_lock_irqsave(&pchan->chan_lock, &flags_chan);
    state = pchan->state;
    osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags_chan);
    osal_spin_unlock_irqrestore(&omxvdec->stream_lock, &flags);

    if (pchan->tunnel_mode_enable != 1) {
        if ((state == CHAN_STATE_WORK || state == CHAN_STATE_PAUSE) && decoder_stop_inst(pchan) < 0) {
                omx_print(OMX_FATAL, "call channel_stop_with_vfmw failed!\n");
        }
        if (state == CHAN_STATE_WORK || state == CHAN_STATE_PAUSE) {
            /* check if the msg queue read out */
            osal_spin_lock_irqsave(&omxvdec->stream_lock, &flags);
            osal_spin_lock_irqsave(&pchan->chan_lock, &flags_chan);
            pchan->state = CHAN_STATE_IDLE;
            osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags_chan);
            osal_spin_unlock_irqrestore(&omxvdec->stream_lock, &flags);
        }

        if (decoder_release_inst(pchan) < 0) {
            omx_print(OMX_FATAL, "call channel_release_with_vfmw failed!\n");
        }
    }

    // 容错处理: 崩库时，用户态不会调用free buffer 接口，导致内核态分配的内存无法释放
    for (i = 0; i < pchan->mem_recorder.alloc_num && i < MAX_MEM_RECORDE_NUM; i++) {
        if (pchan->mem_recorder.chan_buf[i].phyaddr != 0) {
            channel_unbind_user_buffer(pchan, &(pchan->mem_recorder.chan_buf[i]), free_type);
            channel_release_buf(pchan, &(pchan->mem_recorder.chan_buf[i]), free_type);
        }
    }

    pchan->mem_recorder.alloc_num = 0;

    channel_free_inst_memory(pchan, free_type);

    if (pchan->is_tvp == HI_TRUE) {
        ret = decoder_exit_trusted();
        if (ret != HI_SUCCESS) {
            omx_print(OMX_FATAL, "decoder_exit_trusted failed\n");
        }
    }

    message_queue_deinit(pchan->msg_queue);

    osal_spin_lock_irqsave(&omxvdec->stream_lock, &flags);
    osal_spin_lock_irqsave(&pchan->chan_lock, &flags_chan);
    pchan->state = CHAN_STATE_INVALID;
    osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags_chan);

    ret = channel_delete_chan_record(omxvdec, pchan);
    if (ret == HI_FAILURE) {
        omx_print(OMX_FATAL, "channel_delete_chan_record param invalid!\n");
    }
    osal_spin_unlock_irqrestore(&omxvdec->stream_lock, &flags);

    if (pchan->frame != HI_NULL) {
        hi_vfree_omxvdec(pchan->frame);
    }

    VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);

#ifdef HI_DEC_SYNC_SUPPORT
    drv_dec_fence_deinit(pchan->channel_id, pchan);
#endif

    hi_kfree_omxvdec(pchan);
    pchan = HI_NULL;

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 channel_start_inst(OMXVDEC_CHAN_CTX *pchan)
{
    unsigned long flags;
    hi_s32 ret;
    hi_u32 status;

    FUNC_IN();

    status = VDEC_S_FAILED;
    ret = HI_FAILURE;
    osal_spin_lock_irqsave(&pchan->chan_lock, &flags);
    if (pchan->state == CHAN_STATE_WORK) {
        osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags);
        omx_print(OMX_ERR, "already in work state!\n");
        status = VDEC_S_FAILED;
        goto error;
    }

    if ((pchan->b_lowdelay || g_fast_output_mode) && (pchan->omx_vdec_lowdelay_proc_rec != HI_NULL)) {
        pchan->omx_vdec_lowdelay_proc_rec->current_tag = pchan->omx_vdec_lowdelay_proc_rec->usrtag_start_time +
                                                         pchan->omx_vdec_lowdelay_proc_rec->interval;
        pchan->omx_vdec_lowdelay_proc_rec->time_cost_sum = 0;
        pchan->omx_vdec_lowdelay_proc_rec->average_time_cost = 0;
        pchan->omx_vdec_lowdelay_proc_rec->rec_index = 0;
        HI_CHECK_SEC_FUNC(memset_s(pchan->omx_vdec_lowdelay_proc_rec->time_record,
            sizeof(pchan->omx_vdec_lowdelay_proc_rec->time_record), 0,
            sizeof(pchan->omx_vdec_lowdelay_proc_rec->time_record)));
    }
    osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags);

    ret = decoder_start_inst(pchan);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "call decoder_start_inst failed!\n");
        goto error;
    }

    osal_spin_lock_irqsave(&pchan->chan_lock, &flags);
    pchan->state = CHAN_STATE_WORK;
    osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags);

    status = VDEC_S_SUCCESS;

    FUNC_EXIT();

error:
    message_queue(pchan->msg_queue, VDEC_MSG_RESP_START_DONE, status, HI_NULL);
    omx_print(OMX_INFO, "post msg ret=%d,status=%d!\n", ret, status);

    return ret;
}

hi_s32 channel_stop_inst(OMXVDEC_CHAN_CTX *pchan)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 status = VDEC_S_FAILED;
    unsigned long flags;

    FUNC_IN();

    osal_spin_lock_irqsave(&pchan->chan_lock, &flags);
    if (pchan->state == CHAN_STATE_IDLE) {
        status = VDEC_ERR_BAD_STATE;
        omx_print(OMX_FATAL, "already stop!\n");
    } else {
        status = VDEC_S_SUCCESS;
        pchan->state = CHAN_STATE_IDLE;
    }
    osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags);

    // stop order is important!!
    if (status == VDEC_S_SUCCESS) {
        ret = decoder_stop_inst(pchan);
        if (ret != HI_SUCCESS) {
            status = VDEC_ERR_HW_FATAL;
            omx_print(OMX_FATAL, "call channel_stop_with_vfmw failed!\n");
        }

        FUNC_EXIT();
        ret = HI_SUCCESS;
    } else {
        omx_print(OMX_INFO, "post msg ret=%d,status=%d!\n", ret, status);
        ret = HI_FAILURE;
    }

    message_queue(pchan->msg_queue, VDEC_MSG_RESP_STOP_DONE, status, HI_NULL);

    return ret;
}

hi_s32 channel_reset_inst(OMXVDEC_CHAN_CTX *pchan)
{
    hi_s32 ret;

    pchan->reset_pending = 1;

    ret = decoder_reset_inst_with_option(pchan);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "reset decoder failed!\n");
    }

    pchan->reset_pending = 0;

    return ret;
}

hi_s32 channel_pause_inst(OMXVDEC_CHAN_CTX *pchan)
{
    hi_s32 ret;
    unsigned long flags;

    FUNC_IN();

    osal_spin_lock_irqsave(&pchan->chan_lock, &flags);
    if (pchan->state != CHAN_STATE_WORK) {
        osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags);
        omx_print(OMX_ERR, "state != CHAN_STATE_WORK!\n");
        message_queue(pchan->msg_queue, VDEC_MSG_RESP_PAUSE_DONE, VDEC_ERR_BAD_STATE, HI_NULL);
        return HI_FAILURE;
    }

    pchan->state = CHAN_STATE_PAUSE;
    osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags);

    ret = decoder_command_handler(pchan, DEC_CMD_PAUSE, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_ERR, "pause decoder failed!\n");
    }

    message_queue(pchan->msg_queue, VDEC_MSG_RESP_PAUSE_DONE, VDEC_S_SUCCESS, HI_NULL);

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 channel_resume_inst(OMXVDEC_CHAN_CTX *pchan)
{
    hi_s32 ret;
    unsigned long flags;

    FUNC_IN();

    osal_spin_lock_irqsave(&pchan->chan_lock, &flags);
    if (pchan->state != CHAN_STATE_PAUSE) {
        osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags);
        omx_print(OMX_FATAL, "state != CHAN_STATE_PAUSE!\n");
        message_queue(pchan->msg_queue, VDEC_MSG_RESP_RESUME_DONE, VDEC_ERR_BAD_STATE, HI_NULL);
        return HI_FAILURE;
    }

    pchan->state = CHAN_STATE_WORK;
    osal_spin_unlock_irqrestore(&pchan->chan_lock, &flags);

    ret = decoder_command_handler(pchan, DEC_CMD_RESUME, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_ERR, "resume decoder failed!\n");
    }

    message_queue(pchan->msg_queue, VDEC_MSG_RESP_RESUME_DONE, VDEC_S_SUCCESS, HI_NULL);

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 channel_bind_user_buffer(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf)
{
    hi_s32 ret ;

    FUNC_IN();
    if (pchan == HI_NULL || puser_buf == HI_NULL) {
        omx_print(OMX_FATAL, "param invalid!\n");
        return HI_FAILURE;
    }

    ret = channel_insert_addr_table(pchan, puser_buf);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_ERR, "call channel_insert_addr_table failed!\n");
        return HI_FAILURE;
    }

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 channel_unbind_user_buffer(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf, omx_mem_free_type free_type)
{
    hi_s32 ret;

    FUNC_IN();

    if (pchan == HI_NULL || puser_buf == HI_NULL) {
        omx_print(OMX_FATAL, "param invalid!\n");
        return HI_FAILURE;
    }

    ret = channel_delete_addr_table(pchan, puser_buf, free_type);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "call channel_delete_addr_table failed!\n");
        return HI_FAILURE;
    }

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 channel_alloc_buf(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf)
{
    hi_s32 ret;
    hi_u32 buf_size;
    hi_u32 align;
    omxvdec_buffer tmp_buf;
    omxvdec_buffer meta_buf;
    hi_char bufname[20] = { 0 }; /* char size is 20 */

    FUNC_IN();

    if (pchan == HI_NULL || puser_buf == HI_NULL || pchan->mem_recorder.alloc_num >= MAX_MEM_RECORDE_NUM) {
        omx_print(OMX_FATAL, "param invalid!\n");
        return HI_FAILURE;
    }

    HI_CHECK_SEC_FUNC(memset_s(&tmp_buf, sizeof(tmp_buf), 0, sizeof(tmp_buf)));

    align = puser_buf->align;

    buf_size = (puser_buf->buffer_len + align - 1) & ~(align - 1);

    if (puser_buf->dir == 0) {
        HI_CHECK_SEC_FUNC(memcpy_s(bufname, sizeof(bufname), "OMXVDEC_IN", sizeof("OMXVDEC_IN")));
    } else {
        HI_CHECK_SEC_FUNC(memcpy_s(bufname, sizeof(bufname), "OMXVDEC_OUT", sizeof("OMXVDEC_OUT")));
    }

#ifdef HI_SMMU_SUPPORT
    if (puser_buf->is_sec) {
        tmp_buf.mode = OMX_MEM_MMU_SEC;
    } else {
        tmp_buf.mode = OMX_MEM_MMU_MMU;
    }
#else
    if (puser_buf->is_sec) {
        tmp_buf.mode = OMX_MEM_CMA_SEC;
    } else {
        tmp_buf.mode = OMX_MEM_CMA_CMA;
    }
#endif

    ret = hi_drv_omxvdec_alloc(bufname, buf_size, puser_buf->align, &tmp_buf);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "alloc  failed\n");

        return HI_FAILURE;
    }

    puser_buf->buffer_len = tmp_buf.size;
    puser_buf->phyaddr = tmp_buf.phys_addr;
    puser_buf->fd = tmp_buf.fd;
    puser_buf->dma_buf = tmp_buf.dma_buf;
    puser_buf->data_len = 0;
    puser_buf->data_offset = 0;
    puser_buf->timestamp = 0;

    if (puser_buf->dir == 0) {
        decoder_command_handler(pchan, DEC_CMD_BIND_ES_BUF, puser_buf, 0);
    }

    if (puser_buf->meta_type == OMX_ALLOCATE_WITH_META && puser_buf->dir == 1 && puser_buf->private_len != 0) {
        HI_CHECK_SEC_FUNC(memset_s(&meta_buf, sizeof(meta_buf), 0, sizeof(meta_buf)));
        HI_CHECK_SEC_FUNC(memcpy_s(bufname, sizeof(bufname), "OMXVDEC_METADATA", sizeof("OMXVDEC_METADATA")));
        buf_size = (puser_buf->private_len + align - 1) & ~(align - 1);
        ret = hi_drv_omxvdec_alloc(bufname, buf_size, puser_buf->align, &meta_buf);
        if (ret != HI_SUCCESS) {
            hi_drv_omxvdec_free(&tmp_buf, OMX_MEM_FREE_TYPE_NORMAL);
            omx_print(OMX_FATAL, "alloc  failed\n");

            return HI_FAILURE;
        }
        puser_buf->private_phyaddr = meta_buf.phys_addr;
        puser_buf->private_len = meta_buf.size;
        puser_buf->metadata_fd = meta_buf.fd;
        puser_buf->private_dma_buf = meta_buf.dma_buf;
    }

    HI_CHECK_SEC_FUNC(memcpy_s(&(pchan->mem_recorder.chan_buf[pchan->mem_recorder.alloc_num++]),
                               sizeof(OMXVDEC_BUF_DESC),  puser_buf,
                               sizeof(OMXVDEC_BUF_DESC)));

    omx_print(OMX_INFO, "alloc buf phy:0x%x len:%d dir:%d sec:%d\n", puser_buf->phyaddr, puser_buf->buffer_len,
              puser_buf->dir, puser_buf->is_sec);

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 channel_release_meta_buf(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf, omx_mem_free_type free_type)
{
    omxvdec_buffer meta_buf = { 0 };
    hi_u32 i = 0;

    if (puser_buf->meta_type != OMX_ALLOCATE_WITH_META || puser_buf->dir == 0) {
        return HI_SUCCESS;
    }

    for (i = 0; i < pchan->mem_recorder.alloc_num; i++) {
        if (puser_buf->private_phyaddr == pchan->mem_recorder.chan_buf[i].phyaddr) {
            HI_CHECK_SEC_FUNC(memset_s(&(pchan->mem_recorder.chan_buf[i]),  sizeof(pchan->mem_recorder.chan_buf[i]),
                0,  sizeof(OMXVDEC_BUF_DESC)));
            break;
        }
    }

    if (i >= pchan->mem_recorder.alloc_num) {
        omx_print(OMX_FATAL, "phy:0x%x size:0x%x not alloced\n", puser_buf->private_phyaddr, puser_buf->private_len);
        return HI_FAILURE;
    }

    if (puser_buf->private_phyaddr != HI_NULL) {
        meta_buf.phys_addr = (hi_u32)puser_buf->private_phyaddr;
        meta_buf.size = puser_buf->private_len;
        hi_drv_omxvdec_free(&meta_buf, free_type);
    }

    return HI_SUCCESS;
}

hi_s32 channel_release_buf(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf, omx_mem_free_type free_type)
{
    omxvdec_buffer tmp_buf = { 0 };
    omxvdec_buffer meta_buf = { 0 };
    hi_u32 i = 0;
    hi_s32 ret;

    FUNC_IN();

    if (pchan == HI_NULL || puser_buf == HI_NULL) {
        omx_print(OMX_FATAL, "param invalid!\n");
        return HI_FAILURE;
    }

    HI_CHECK_SEC_FUNC(memset_s(&tmp_buf, sizeof(tmp_buf), 0, sizeof(tmp_buf)));

    if (free_type == OMX_MEM_FREE_TYPE_NORMAL && channel_get_phy_from_fd(pchan, puser_buf) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    tmp_buf.phys_addr = (hi_u32)puser_buf->phyaddr;
    tmp_buf.size = puser_buf->buffer_len;
    tmp_buf.fd = puser_buf->fd;
    tmp_buf.dma_buf = puser_buf->dma_buf;

    for (i = 0; i < MAX_MEM_RECORDE_NUM; i++) {
        if (puser_buf->phyaddr == pchan->mem_recorder.chan_buf[i].phyaddr) {
            HI_CHECK_SEC_FUNC(memset_s(&(pchan->mem_recorder.chan_buf[i]),  sizeof(pchan->mem_recorder.chan_buf[i]),
                                       0,  sizeof(OMXVDEC_BUF_DESC)));
            break;
        }
    }

    if (i >= MAX_MEM_RECORDE_NUM) {
        omx_print(OMX_FATAL, "phy:0x%x size:0x%x not alloc before!\n", puser_buf->phyaddr, puser_buf->buffer_len);
        return HI_FAILURE;
    }

    hi_drv_omxvdec_free(&tmp_buf, free_type);

    if (puser_buf->meta_type == OMX_ALLOCATE_WITH_META && puser_buf->dir == 1) {
        if (puser_buf->private_phyaddr != 0) {
            meta_buf.phys_addr = (hi_u32)puser_buf->private_phyaddr;
            meta_buf.size = puser_buf->private_len;
            hi_drv_omxvdec_free(&meta_buf, free_type);
        }
    }

    ret = channel_release_meta_buf(pchan, puser_buf, free_type);
    pchan->mem_recorder.alloc_num--;

    omx_print(OMX_INFO, "free buf phy:0x%llx len:%d\n", tmp_buf.phys_addr, tmp_buf.size);
    FUNC_EXIT();

    return HI_SUCCESS;
}

static hi_void channel_receive_eos_flag(OMXVDEC_CHAN_CTX *pchan, omxvdec_buf *pstream)
{
    unsigned long flags;
    hi_s32 ret = HI_FAILURE;

    if (pchan->eos_info.eos_in_list == HI_FALSE) {
        pstream = &pchan->last_frame;
        ret = channel_add_last_frame(pchan, pstream);
        if (ret != HI_SUCCESS) {
            omx_print(OMX_ERR, "chan(%d) send last frame failed!\n", pchan->channel_id);
        } else {
            pstream->status = BUF_STATE_QUEUED;
            osal_spin_lock_irqsave(&pchan->raw_lock, &flags);
            list_add_tail(&pstream->list, &pchan->raw_queue);
            osal_spin_unlock_irqrestore(&pchan->raw_lock, &flags);
            pchan->eos_info.eos_in_list = HI_TRUE;
        }
    } else {
        omx_print(OMX_ERR, "chan(%d) eos already in list!\n", pchan->channel_id);
    }
}
static hi_void channel_empty_this_stream_process(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf,
                                                 omxvdec_buf *pstream)
{
    if (puser_buf->flags & VFMW_OMX_STM_FLAG_EOS) {
        omx_print(OMX_INFO, "receive EOS flag!\n");
        channel_receive_eos_flag(pchan, pstream);

        pchan->eos_info.eos_receive_num++;
    }

    if (puser_buf->data_len == 0) {
        omx_print(OMX_ERR, "receive an empty buffer, return immediately.\n");
        message_queue(pchan->msg_queue, VDEC_MSG_RESP_INPUT_DONE, VDEC_S_SUCCESS, (hi_void *)puser_buf);
        pchan->omx_chan_statinfo.EBD++;
    }

    pchan->omx_chan_statinfo.ETB++;

    if (pchan->b_lowdelay == HI_TRUE || g_fast_output_mode == HI_TRUE) {
        (void)(g_vfmw_func->fn_vfmw_control)(pchan->decoder_id, VFMW_CID_EXTRA_WAKEUP_THREAD, HI_NULL, 0);
    }

    return;
}

hi_s32 channel_empty_this_stream(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf)
{
    hi_s32 ret = HI_FAILURE;
    omxvdec_buf *pstream = HI_NULL;

    FUNC_IN();

    if (pchan == HI_NULL || puser_buf == HI_NULL) {
        omx_print(OMX_FATAL, "param invalid!\n");
        return HI_FAILURE;
    }

    // only process buffer with valid data len
    if (puser_buf->data_len != 0) {
        pstream = channel_lookup_addr_table(pchan, puser_buf);
        if (pstream == HI_NULL) {
            omx_print(OMX_ERR, "call channel_lookup_addr_table failed!\n");
            message_queue(pchan->msg_queue, VDEC_MSG_RESP_INPUT_DONE, VDEC_ERR_ILLEGAL_OP, (hi_void *)puser_buf);
            return HI_FAILURE;
        }

        pstream->act_len = puser_buf->data_len;
        pstream->offset = puser_buf->data_offset;
        pstream->time_stamp = puser_buf->timestamp;
        pstream->flags = puser_buf->flags;
        if ((pchan->b_lowdelay || g_fast_output_mode) && (pchan->omx_vdec_lowdelay_proc_rec != HI_NULL)) {
            pstream->usr_tag = pchan->omx_vdec_lowdelay_proc_rec->current_tag;
            pchan->omx_vdec_lowdelay_proc_rec->current_tag += pchan->omx_vdec_lowdelay_proc_rec->interval;
            channel_add_lowdelay_tag_time(pchan, pstream->usr_tag, OMX_LOWDELAY_REC_ETB_TIME, omx_get_time_in_ms());
        }

        /* save raw process */
        if (pchan->is_tvp != HI_TRUE) {
            channel_save_stream(pchan, pstream);
        }

        ret = channel_insert_in_raw_list(pchan, pstream);
        if (ret != HI_SUCCESS) {
            message_queue(pchan->msg_queue, VDEC_MSG_RESP_INPUT_DONE, VDEC_ERR_ILLEGAL_OP, (hi_void *)puser_buf);
            return HI_FAILURE;
        }
    }

    channel_empty_this_stream_process(pchan, puser_buf, pstream);

    FUNC_EXIT();

    return HI_SUCCESS;
}

static hi_void channel_bind_ext_buffer(OMXVDEC_CHAN_CTX *chan, omxvdec_buf *frame)
{
    hi_s32 i;

    for (i = 0 ; i < OMXVDEC_MAX_EXT_BUF_NUM; i++) {
        if (chan->ext_buf[i].is_valid == -1) {
            chan->ext_buf[i].dma_buf = frame->dma_buf;
            chan->ext_buf[i].fd = frame->fd;
            chan->ext_buf[i].phy_addr = frame->phy_addr;
            chan->ext_buf[i].is_valid = 1;
            break;
        }
    }

    if (i >= OMXVDEC_MAX_EXT_BUF_NUM) {
        omx_print(OMX_ERR, "can't find valid ext buf slot\n");
    }

    return;
}

static hi_void channel_unbind_ext_buffer(OMXVDEC_CHAN_CTX *chan)
{
    hi_s32 i;

    for (i = 0 ; i < OMXVDEC_MAX_EXT_BUF_NUM; i++) {
        chan->ext_buf[i].is_valid = -1;
    }

}

static hi_void channel_work_que_release_image(struct work_struct *work)
{
    hi_s32 ret;
    omx_vdec_work_queue *rls_work_queue = HI_NULL;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
    struct dma_fence *fence;
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
    struct fence *fence;
#else
    struct hi_sync_fence *fence;
#endif
    hi_u32 start_time;
    OMXVDEC_CHAN_CTX *chan = HI_NULL;

    rls_work_queue = container_of(work, omx_vdec_work_queue, sync_work);
    if (rls_work_queue == HI_NULL) {
        return;
    }

    fence = rls_work_queue->fence;
    if (fence == HI_NULL) {
        hi_vfree_omxvdec(rls_work_queue);
        return;
    }

    chan = rls_work_queue->chan_info;
    if (chan == HI_NULL) {
        hi_sync_fence_put(fence);
        hi_vfree_omxvdec(rls_work_queue);
        return;
    }

    start_time = omx_get_time_in_ms();
    ret = hi_sync_fence_wait(fence, 1000);
    if (ret < 0 || omx_get_time_in_ms() - start_time > 100) {
        omx_print(OMX_FATAL, "wait time %d, 0x%x, ret %d\n", omx_get_time_in_ms() - start_time,
            rls_work_queue->buf_info.phyaddr, ret);
    }

    hi_sync_fence_put(fence);

    rls_work_queue->buf_info.from_work_queue = HI_TRUE;
    ret = channel_fill_this_frame(rls_work_queue->chan_info, &(rls_work_queue->buf_info));
    if (ret != HI_SUCCESS) {
        omx_print(OMX_ERR, "%s fill this frame (0x%x) failed!\n", __func__, rls_work_queue->buf_info.phyaddr);
    }

    chan->work_number--;
    hi_vfree_omxvdec(rls_work_queue);

    return;
}

hi_s32 channel_fill_anw_store_meta(OMXVDEC_CHAN_CTX *chan, OMXVDEC_BUF_DESC *user_buf)
{
    hi_s32 ret;
    unsigned long flags;
    omxvdec_buf *frame = HI_NULL;
    hi_u32 image_id = INVALID_IMAGE_ID;

    if ((chan == HI_NULL) || (user_buf == HI_NULL)) {
        omx_print(OMX_FATAL, "param invalid!\n");
        return HI_FAILURE;
    }

    frame = channel_lookup_addr_table(chan, user_buf);
    if (frame == HI_NULL) {
        omx_print(OMX_ERR, "call channel_lookup_addr_table failed!\n");
        message_queue(chan->msg_queue, VDEC_MSG_RESP_OUTPUT_DONE, VDEC_ERR_ILLEGAL_OP, (hi_void *)user_buf);
        return HI_FAILURE;
    }

    if ((frame->phy_addr == 0) && user_buf->release_fd >= 0) {
        omx_print(OMX_ALWS, "fd %d not correct\n", user_buf->release_fd);
        drv_dec_fence_destroy(user_buf->release_fd);
        user_buf->release_fd = -1;
    }

    if (user_buf->release_fd >= 0) {
        omx_vdec_work_queue *rls_work_queue = HI_NULL;

        if (chan->release_queue == HI_NULL) {
            omx_print(OMX_ALWS, "%s, work queue is null\n", __func__);
            drv_dec_fence_destroy(user_buf->release_fd);
            return HI_FAILURE;
        }
#if 1
        rls_work_queue = hi_vmalloc_omxvdec(sizeof(omx_vdec_work_queue));
        if (rls_work_queue == HI_NULL) {
            omx_print(OMX_FATAL, "%s malloc work queue fail\n", __func__);
            return HI_FAILURE;
        }

        rls_work_queue->fence = hi_sync_fence_fdget(user_buf->release_fd);
        if (rls_work_queue != HI_NULL) {
            rls_work_queue->chan_info = chan;
            HI_CHECK_SEC_FUNC(memcpy_s(&(rls_work_queue->buf_info), sizeof(OMXVDEC_BUF_DESC),
                                         user_buf, sizeof(OMXVDEC_BUF_DESC)));
            INIT_WORK(&(rls_work_queue->sync_work), channel_work_que_release_image);
            queue_work(chan->release_queue, &(rls_work_queue->sync_work));
            chan->work_number++;
        }
#else
        {
            HI_U32 start_time;
            struct fence *fence = hi_sync_fence_fdget(user_buf->release_fd);
            start_time = omx_get_time_in_ms();

            omx_print(OMX_ALWS, "111 %d\n", fence_get_status(fence));
            //drv_dec_fence_destroy(user_buf->release_fd);

            omx_print(OMX_ALWS, "222 %d\n", fence_get_status(fence));
            ret = hi_sync_fence_wait(fence, 1000);
                if (ret <= 0 || omx_get_time_in_ms() - start_time > 100) {
                    omx_print(OMX_ALWS, "wait time %d, 0x%x, ret %d\n", omx_get_time_in_ms() - start_time,
                    user_buf->phyaddr, ret);
            }
            hi_sync_fence_put(fence);

            ret = channel_fill_this_frame(chan, user_buf);
            if (ret != HI_SUCCESS) {
                omx_print(OMX_ERR, "%s fill this frame (0x%x) failed!\n", __func__, user_buf->phyaddr);
            }
        }
#endif
        return HI_SUCCESS;
    } else {
        if (frame->phy_addr == 0) {
            hi_void *kern_vaddr = HI_NULL;
            /*
            if (channel_map_kernel_viraddr(chan, frame, user_buf, &kern_vaddr) != HI_SUCCESS) {
                omx_print(OMX_FATAL, "%s: get_addr failed (phyaddr: 0x%x)\n", __func__, user_buf->phyaddr);
                return HI_FAILURE;
            }

            omx_print(OMX_FATAL, "0x%x, %d, fd: %d, 0x%x\n", frame->phy_addr,
                user_buf->from_work_queue, user_buf->release_fd, user_buf->phyaddr);
            */

            frame->user_vaddr  = (hi_void *)(uintptr_t)user_buf->bufferaddr;
            frame->phy_addr = user_buf->phyaddr;
            frame->dma_buf = user_buf->dma_buf;
            frame->fd = user_buf->fd;
            if (user_buf->buffer_type == OMX_USE_NATIVE) {
                frame->fd = user_buf->fd;
                frame->dma_buf = PTR_UINT64(osal_mem_handle_get(frame->fd, HI_ID_VDEC));
                osal_mem_ref_put(UINT64_PTR(frame->dma_buf), HI_ID_VDEC);
            }
            frame->private_fd = user_buf->private_fd;
            frame->private_phy_addr = user_buf->private_phyaddr;
            frame->kern_vaddr = kern_vaddr;
            frame->client_data = (hi_void *)(uintptr_t)user_buf->client_data;
            frame->buf_len = user_buf->buffer_len;
            frame->act_len = 0;
            frame->private_len = user_buf->private_len;
            frame->offset = 0;
            frame->buf_type = user_buf->buffer_type;
            frame->is_first_buf = user_buf->is_first_buf;
            frame->status = BUF_STATE_IDLE;

            channel_insert_addr_table_process(chan, user_buf, frame);
        }

        frame->offset = user_buf->data_offset;
        frame->act_len = 0;
        frame->release_fd = user_buf->release_fd;

        ret = channel_insert_in_yuv_list(chan, frame);
        if (ret != HI_SUCCESS) {
            message_queue(chan->msg_queue, VDEC_MSG_RESP_OUTPUT_DONE, VDEC_ERR_ILLEGAL_OP, (hi_void *)user_buf);
            return HI_FAILURE;
        }

        if (chan->port_enable_flag != 1) {
            omx_print(OMX_WARN, "port_enable_flag != 1\n");
            return HI_SUCCESS;
        }

        if (frame->is_bind == HI_FALSE) {
            ret = decoder_command_handler(chan, DEC_CMD_BIND_MEM, (hi_void *)frame, 0);
            if (ret == HI_SUCCESS) {
                osal_spin_lock_irqsave(&chan->yuv_lock, &flags);
                frame->status = BUF_STATE_USING;
                osal_spin_unlock_irqrestore(&chan->yuv_lock, &flags);
                frame->is_bind = HI_TRUE;
                channel_bind_ext_buffer(chan, frame);
            }
            chan->omx_chan_statinfo.FTB++;
            return HI_SUCCESS;
        }

        if (frame->ext_info.data != 0 ) {
            image_id = ((vfmw_private_frm_info *)UINT64_PTR(frame->ext_info.data))->image_id;
        }

        if (image_id != INVALID_IMAGE_ID) {
            ret = decoder_command_handler(chan, DEC_CMD_RELEASE_IMAGE, (hi_void *)frame, 0);
            if (ret == HI_SUCCESS) {
                osal_spin_lock_irqsave(&chan->yuv_lock, &flags);
                frame->status = BUF_STATE_USING;
                osal_spin_unlock_irqrestore(&chan->yuv_lock, &flags);
            } else {
                osal_spin_lock_irqsave(&chan->yuv_lock, &flags);
                frame->status = BUF_STATE_QUEUED;
                osal_spin_unlock_irqrestore(&chan->yuv_lock, &flags);
                omx_print(OMX_FATAL, "release frame to decoder failed!\n");
            }
        } else {
            omx_print(OMX_FATAL, "release frame image_id err\n");
        }
        chan->omx_chan_statinfo.FTB++;
    }

    return HI_SUCCESS;
}

hi_s32 channel_fill_this_frame(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf)
{
    hi_s32 ret;
    unsigned long flags;
    hi_u32 image_id = INVALID_IMAGE_ID;

    omxvdec_buf *pframe = HI_NULL;

    FUNC_IN();

    if (pchan == HI_NULL || puser_buf == HI_NULL) {
        omx_print(OMX_FATAL, "param invalid!\n");
        return HI_FAILURE;
    }

    pframe = channel_lookup_addr_table(pchan, puser_buf);
    if (pframe == HI_NULL) {
        omx_print(OMX_ERR, "call channel_lookup_addr_table failed!\n");
        message_queue(pchan->msg_queue, VDEC_MSG_RESP_OUTPUT_DONE, VDEC_ERR_ILLEGAL_OP, (hi_void *)puser_buf);
        return HI_FAILURE;
    }

    pframe->offset = puser_buf->data_offset;
    pframe->act_len = 0;
    pframe->release_fd = puser_buf->release_fd;

    ret = channel_insert_in_yuv_list(pchan, pframe);
    if (ret != HI_SUCCESS) {
        message_queue(pchan->msg_queue, VDEC_MSG_RESP_OUTPUT_DONE, VDEC_ERR_ILLEGAL_OP, (hi_void *)puser_buf);
        return HI_FAILURE;
    }

    if (pchan->port_enable_flag != 1) {
        omx_print(OMX_WARN, "port_enable_flag != 1\n");
        return HI_SUCCESS;
    }

    if (pframe->is_bind == HI_FALSE) {
        ret = decoder_command_handler(pchan, DEC_CMD_BIND_MEM, (hi_void *)pframe, 0);
        if (ret == HI_SUCCESS) {
            osal_spin_lock_irqsave(&pchan->yuv_lock, &flags);
            pframe->status = BUF_STATE_USING;
            osal_spin_unlock_irqrestore(&pchan->yuv_lock, &flags);
            pframe->is_bind = HI_TRUE;
            channel_bind_ext_buffer(pchan, pframe);
        }
        pchan->omx_chan_statinfo.FTB++;

        FUNC_EXIT();

        return HI_SUCCESS;
    }

    if (pframe->ext_info.data != 0) {
        image_id = ((vfmw_private_frm_info *)UINT64_PTR(pframe->ext_info.data))->image_id;
    }

    if (image_id != INVALID_IMAGE_ID) {
        ret = decoder_command_handler(pchan, DEC_CMD_RELEASE_IMAGE, (hi_void *)pframe, 0);
        if (ret == HI_SUCCESS) {
            osal_spin_lock_irqsave(&pchan->yuv_lock, &flags);
            pframe->status = BUF_STATE_USING;
            osal_spin_unlock_irqrestore(&pchan->yuv_lock, &flags);
        } else {
            osal_spin_lock_irqsave(&pchan->yuv_lock, &flags);
            pframe->status = BUF_STATE_QUEUED;
            osal_spin_unlock_irqrestore(&pchan->yuv_lock, &flags);
            omx_print(OMX_FATAL, "release frame to decoder failed!\n");
        }
    } else {
        omx_print(OMX_FATAL, "release frame image_id err\n");
    }
    pchan->omx_chan_statinfo.FTB++;

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 channel_get_message(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_MSG_INFO *pmsg)
{
    if (pchan == HI_NULL || pmsg == HI_NULL) {
        omx_print(OMX_FATAL, "param invalid!\n");
        return HI_FAILURE;
    }

    return message_dequeue(pchan->msg_queue, pmsg);
}

hi_s32 channel_flush_in_port(OMXVDEC_CHAN_CTX *pchan)
{
    unsigned long flags;
    omxvdec_buf *pbuf = HI_NULL;
    omxvdec_buf *ptmp = HI_NULL;
    OMXVDEC_BUF_DESC user_buf;
    HI_CHECK_SEC_FUNC(memset_s(&user_buf, sizeof(user_buf), 0, sizeof(OMXVDEC_BUF_DESC)));

    pbuf = ptmp = HI_NULL;

    osal_spin_lock_irqsave(&pchan->raw_lock, &flags);
    list_for_each_entry_safe(pbuf, ptmp, &pchan->raw_queue, list)
    {
        if (pbuf->status == BUF_STATE_USING) {
            continue;
        }

        pbuf->status = BUF_STATE_IDLE;
        omx_print(OMX_ERR, "phy addr = 0x%x status = %d\n", pbuf->phy_addr, pbuf->status);
        list_del(&pbuf->list);

        if (pbuf->buf_id != LAST_FRAME_BUF_ID) {
            user_buf.dir = PORT_DIR_INPUT;
            user_buf.bufferaddr = (uintptr_t)pbuf->user_vaddr;
            user_buf.buffer_len = pbuf->buf_len;
            user_buf.client_data = (uintptr_t)pbuf->client_data;

            user_buf.data_len = 0;
            user_buf.timestamp = 0;

            pbuf->act_len = user_buf.data_len;
            message_queue(pchan->msg_queue, VDEC_MSG_RESP_INPUT_DONE, VDEC_S_SUCCESS, (void *)&user_buf);
            omx_print(OMX_OUTBUF, "release idle in buffer: phy addr = 0x%08x\n", pbuf->phy_addr);
        } else {
            pchan->eos_info.eos_in_list = HI_FALSE;
        }
    }

    if (pchan->raw_use_cnt > 0) {
        pchan->input_flush_pending = 1;
    } else {
        message_queue(pchan->msg_queue, VDEC_MSG_RESP_FLUSH_INPUT_DONE, VDEC_S_SUCCESS, HI_NULL);
    }

    osal_spin_unlock_irqrestore(&pchan->raw_lock, &flags);

    if (pchan->input_flush_pending) {
        omx_print(OMX_INBUF, "call vfmw to release input buffers.\n");
        decoder_command_handler(pchan, DEC_CMD_CLEAR_STREAM, HI_NULL, 0);  // call decoder to return all es buffer
    }

    return HI_SUCCESS;
}

hi_void channel_flush_output_buffer(OMXVDEC_CHAN_CTX *pchan)
{
    unsigned long flags;
    omxvdec_buf *pbuf = HI_NULL;
    omxvdec_buf *ptmp = HI_NULL;
    OMXVDEC_BUF_DESC user_buf;
    hi_u32 times = 0;

    FUNC_IN();

    while(pchan->work_number > 0 && times < 100) {
        msleep(5);
        times++;
    }

    osal_spin_lock_irqsave(&pchan->yuv_lock, &flags);

    HI_CHECK_SEC_FUNC(memset_s(&user_buf, sizeof(user_buf), 0, sizeof(OMXVDEC_BUF_DESC)));
    pbuf = ptmp = HI_NULL;

    list_for_each_entry_safe(pbuf, ptmp, &pchan->yuv_queue, list)
    {
        user_buf.dir = PORT_DIR_OUTPUT;
        user_buf.bufferaddr = (uintptr_t)pbuf->user_vaddr;
        user_buf.buffer_len = pbuf->buf_len;
        user_buf.client_data = (uintptr_t)pbuf->client_data;
        user_buf.flags = 0;
        user_buf.data_len = 0;
        user_buf.timestamp = 0;
        user_buf.release_fd = pbuf->release_fd;

        pbuf->status = BUF_STATE_IDLE;
        list_del(&pbuf->list);

        message_queue(pchan->msg_queue, VDEC_MSG_RESP_OUTPUT_DONE, VDEC_S_SUCCESS, (hi_void *)&user_buf);
        pchan->omx_chan_statinfo.FBD++;

        omx_print(OMX_OUTBUF, "release idle out buffer: phy addr = 0x%08x\n", pbuf->phy_addr);
    }

    if (pchan->yuv_use_cnt > 0) {
        pchan->output_flush_pending = 1;
        omx_print(OMX_INFO, "flush output pending yuv use cnt:%d\n", pchan->yuv_use_cnt);
    } else {
        message_queue(pchan->msg_queue, VDEC_MSG_RESP_FLUSH_OUTPUT_DONE, VDEC_S_SUCCESS, HI_NULL);
        omx_print(OMX_INFO, "flush done\n");
    }

    osal_spin_unlock_irqrestore(&pchan->yuv_lock, &flags);

    FUNC_EXIT();
    return;
}

hi_s32 channel_flush_out_port(OMXVDEC_CHAN_CTX *pchan)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long flags;
    hi_u32 i = 0;
    hi_u32 output_buf_num = 0;
    omxvdec_buf *out_buf_table = HI_NULL;
    omxvdec_buf *pbuf = HI_NULL;

    FUNC_IN();

    if (pchan->tunnel_mode_enable == 1) {
        pchan->seek_pending = 1;
        omx_print(OMX_FATAL, "tunneled-mode do not need flush output, return success!\n");
        osal_spin_lock_irqsave(&pchan->yuv_lock, &flags);
        message_queue(pchan->msg_queue, VDEC_MSG_RESP_FLUSH_OUTPUT_DONE, VDEC_S_SUCCESS, HI_NULL);
        osal_spin_unlock_irqrestore(&pchan->yuv_lock, &flags);
        FUNC_EXIT();
        return HI_SUCCESS;
    }

    if (pchan->recfg_flag == 1) {  // flush command called by resolution change
        omx_print(OMX_INFO, "wait for output with no clear.\n");
        pchan->recfg_flag = 0;

        ret = decoder_command_handler(pchan, DEC_CMD_FLUSH, (hi_void *)HI_NULL, 0);
        if (ret != HI_SUCCESS) {
            omx_print(OMX_ERR, "flush decoder failed!\n");
        }
    } else {  // flush command called by seek
        omx_print(OMX_INFO, "call to clear remain pictures.\n");
        decoder_command_handler(pchan, DEC_CMD_FLUSH, (hi_void *)HI_NULL, 0);
        channel_reset_inst(pchan);  // reset channel to clear old image

        osal_spin_lock_irqsave(&pchan->yuv_lock, &flags);

        out_buf_table = pchan->out_buf_table;
        output_buf_num = pchan->output_buf_num;

        for (i = 0; i < output_buf_num; i++) {
            pbuf = &out_buf_table[i];

            pbuf->is_bind = HI_FALSE;
        }

        channel_unbind_ext_buffer(pchan);
        osal_spin_unlock_irqrestore(&pchan->yuv_lock, &flags);

        pchan->seek_pending = 1;
    }

    channel_flush_output_buffer(pchan);

    FUNC_EXIT();

    return HI_SUCCESS;
}

hi_s32 channel_flush_inst(OMXVDEC_CHAN_CTX *pchan, e_port_dir dir)
{
    hi_s32 ret_in = HI_SUCCESS;
    hi_s32 ret_out = HI_SUCCESS;
    hi_u32 ret_in_u32;
    hi_u32 ret_out_u32;

    FUNC_IN();

    if (pchan == HI_NULL) {
        omx_print(OMX_FATAL, "param invalid!\n");
        return HI_FAILURE;
    }

    if ((dir == PORT_DIR_INPUT) || (dir == PORT_DIR_BOTH)) {
        ret_in = channel_flush_in_port(pchan);
        if (ret_in != HI_SUCCESS) {
            omx_print(OMX_ERR, "channel_flush_in_port failed!\n");
        }
    }

    if ((dir == PORT_DIR_OUTPUT) || (dir == PORT_DIR_BOTH)) {
        ret_out = channel_flush_out_port(pchan);
        if (ret_out != HI_SUCCESS) {
            omx_print(OMX_ERR, "channel_flush_out_port failed!\n");
        }
    }

    FUNC_EXIT();

    ret_in_u32 = (hi_u32)ret_in;
    ret_out_u32 = (hi_u32)ret_out;

    return (ret_in_u32 | ret_out_u32);
}

hi_s32 channel_report_message(OMXVDEC_CHAN_CTX *pchan, hi_u32 msgcode, const hi_void *priv)
{
    FUNC_IN();

    message_queue(pchan->msg_queue, msgcode, VDEC_S_SUCCESS, priv);

    FUNC_EXIT();

    return HI_SUCCESS;
}

OMXVDEC_CHAN_CTX *channel_find_inst_by_channel_id(OMXVDEC_ENTRY *omxvdec, hi_s32 handle)
{
    hi_s8 find = 0;
    OMXVDEC_CHAN_CTX *pchan = HI_NULL;
    unsigned long flags;

    if (omxvdec == HI_NULL || handle < 0) {
        omx_print(OMX_FATAL, "omxvdec = HI_NULL / handle(%d) invalid\n", handle);
        return HI_NULL;
    }

    osal_spin_lock_irqsave(&omxvdec->channel_lock, &flags);
    if (!list_empty(&omxvdec->chan_list)) {
        list_for_each_entry(pchan, &omxvdec->chan_list, chan_list)
        {
            if (pchan->channel_id == handle) {
                find = 1;
                break;
            }
        }
    }
    osal_spin_unlock_irqrestore(&omxvdec->channel_lock, &flags);

    if (find == 0) {
        pchan = HI_NULL;
    }

    return pchan;
}

OMXVDEC_CHAN_CTX *channel_find_inst_by_decoder_id(OMXVDEC_ENTRY *omxvdec, hi_s32 handle)
{
    hi_s8 find = 0;
    OMXVDEC_CHAN_CTX *pchan = HI_NULL;
    unsigned long flags;

    if (omxvdec == HI_NULL || handle < 0) {
        omx_print(OMX_FATAL, "omxvdec = HI_NULL / handle(%d) invalid\n", handle);
        return HI_NULL;
    }

    osal_spin_lock_irqsave(&omxvdec->channel_lock, &flags);
    if (!list_empty(&omxvdec->chan_list)) {
        list_for_each_entry(pchan, &omxvdec->chan_list, chan_list)
        {
            if (pchan->decoder_id == handle) {
                find = 1;
                break;
            }
        }
    }
    osal_spin_unlock_irqrestore(&omxvdec->channel_lock, &flags);

    if (find == 0) {
        pchan = HI_NULL;
    }

    return pchan;
}

static hi_void channel_lowdelay_rec_etb_time(OMXVDEC_CHAN_CTX *pchan, hi_u32 tag, hi_u32 type, hi_u32 time)
{
    hi_u32 i;

    for (i = 0; i < OMX_LOWDELAY_REC_NODE_NUM; i++) {
        if (pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_NODE_STATE] ==
            OMX_LOWDELAY_REC_NODE_FREE) {
                HI_CHECK_SEC_FUNC(memset_s(pchan->omx_vdec_lowdelay_proc_rec->time_record[i],
                    sizeof(pchan->omx_vdec_lowdelay_proc_rec->time_record[i]),
                    0, sizeof(pchan->omx_vdec_lowdelay_proc_rec->time_record[i])));
            pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_USERTAG] = tag;
            pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_ETB_TIME] = time;
            pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_NODE_STATE] =
                OMX_LOWDELAY_REC_NODE_WRITED;
            break;
        }
    }

    return;
}

static hi_void channel_lowdelay_rec_vfmw_rcv_strm_time(OMXVDEC_CHAN_CTX *pchan, hi_u32 tag, hi_u32 type,
                                                       hi_u32 time)
{
    hi_u32 i;

    for (i = 0; i < OMX_LOWDELAY_REC_NODE_NUM; i++) {
        if ((pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_USERTAG] == tag) &&
            (pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_NODE_STATE] ==
            OMX_LOWDELAY_REC_NODE_WRITED)) {
            pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_VFMW_RCV_STRM_TIME] = time;
            break;
        }
    }

    return;
}

static hi_void channel_lowdelay_rec_vfmw_rls_strm_time(OMXVDEC_CHAN_CTX *pchan, hi_u32 tag, hi_u32 type,
                                                       hi_u32 time)
{
    hi_u32 i;

    for (i = 0; i < OMX_LOWDELAY_REC_NODE_NUM; i++) {
        if ((pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_USERTAG] == tag) &&
            (pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_NODE_STATE] ==
                OMX_LOWDELAY_REC_NODE_WRITED)) {
            pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_VFMW_RLS_STRM_TIME] = time;
            break;
        }
    }

    return;
}

static hi_void channel_lowdelay_rec_vfmw_rpo_img_time(OMXVDEC_CHAN_CTX *pchan, hi_u32 tag, hi_u32 type,
                                                      hi_u32 time)
{
    hi_u32 i = 0;

    for (i = 0; i < OMX_LOWDELAY_REC_NODE_NUM; i++) {
        if ((pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_USERTAG] == tag) &&
            (pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_NODE_STATE] ==
            OMX_LOWDELAY_REC_NODE_WRITED)) {
            pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_VFMW_RPO_IMG_TIME] = time;
            break;
        }
    }

    return;
}

static hi_void channel_lowdelay_rec_vpss_rcv_img_time(OMXVDEC_CHAN_CTX *pchan, hi_u32 tag, hi_u32 type,
                                                      hi_u32 time)
{
    hi_u32 i;

    for (i = 0; i < OMX_LOWDELAY_REC_NODE_NUM; i++) {
        if ((pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_USERTAG] == tag) &&
            (pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_NODE_STATE] ==
            OMX_LOWDELAY_REC_NODE_WRITED)) {
            pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_VPSS_RCV_IMG_TIME] = time;
            break;
        }
    }

    return;
}

static hi_void channel_lowdelay_rec_vpss_rpo_img_time(OMXVDEC_CHAN_CTX *pchan, hi_u32 tag, hi_u32 type,
                                                      hi_u32 time)
{
    hi_u32 i;

    for (i = 0; i < OMX_LOWDELAY_REC_NODE_NUM; i++) {
        if ((pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_USERTAG] == tag) &&
            (pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_NODE_STATE] ==
            OMX_LOWDELAY_REC_NODE_WRITED)) {
            pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_VPSS_RPO_IMG_TIME] = time;
            pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_TOTAL_USED_TIME] =
                pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_VPSS_RPO_IMG_TIME] -
                pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_ETB_TIME];
            pchan->omx_vdec_lowdelay_proc_rec->rec_index++;
            if (pchan->omx_vdec_lowdelay_proc_rec->rec_index < g_low_delay_count_frame) {
                pchan->omx_vdec_lowdelay_proc_rec->time_cost_sum +=
                    pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_TOTAL_USED_TIME];
            } else {
                pchan->omx_vdec_lowdelay_proc_rec->average_time_cost =
                (pchan->omx_vdec_lowdelay_proc_rec->time_cost_sum) / (g_low_delay_count_frame);
                pchan->omx_vdec_lowdelay_proc_rec->rec_index = 0;
                pchan->omx_vdec_lowdelay_proc_rec->time_cost_sum = 0;
            }
            HI_CHECK_SEC_FUNC(memcpy_s(pchan->omx_vdec_lowdelay_proc_rec->time_record[OMX_LOWDELAY_REC_NODE_NUM],
                sizeof(pchan->omx_vdec_lowdelay_proc_rec->time_record[i]),
                pchan->omx_vdec_lowdelay_proc_rec->time_record[i],
                sizeof(pchan->omx_vdec_lowdelay_proc_rec->time_record[i])));
            break;
        }
    }

    for (i = 0; i < OMX_LOWDELAY_REC_NODE_NUM; i++) {
        if (pchan->omx_vdec_lowdelay_proc_rec->time_record[i][OMX_LOWDELAY_REC_USERTAG] <= tag) {
            HI_CHECK_SEC_FUNC(memset_s(pchan->omx_vdec_lowdelay_proc_rec->time_record[i],
                sizeof(pchan->omx_vdec_lowdelay_proc_rec->time_record[i]),  0,
                sizeof(pchan->omx_vdec_lowdelay_proc_rec->time_record[i])));
        }
    }

    return;
}

hi_void channel_add_lowdelay_tag_time(OMXVDEC_CHAN_CTX *pchan, hi_u32 tag, hi_u32 type, hi_u32 time)
{
    if (pchan == HI_NULL) {
        omx_print(OMX_FATAL, "param invalid!\n");

        return;
    }

    switch (type) {
        case OMX_LOWDELAY_REC_ETB_TIME:
            channel_lowdelay_rec_etb_time(pchan, tag, type, time);
            break;

        case OMX_LOWDELAY_REC_VFMW_RCV_STRM_TIME:
            channel_lowdelay_rec_vfmw_rcv_strm_time(pchan, tag, type, time);
            break;

        case OMX_LOWDELAY_REC_VFMW_RLS_STRM_TIME:
            channel_lowdelay_rec_vfmw_rls_strm_time(pchan, tag, type, time);
            break;

        case OMX_LOWDELAY_REC_VFMW_RPO_IMG_TIME:
            channel_lowdelay_rec_vfmw_rpo_img_time(pchan, tag, type, time);
            break;

        case OMX_LOWDELAY_REC_VPSS_RCV_IMG_TIME:
            channel_lowdelay_rec_vpss_rcv_img_time(pchan, tag, type, time);
            break;

        case OMX_LOWDELAY_REC_VPSS_RPO_IMG_TIME:
            channel_lowdelay_rec_vpss_rpo_img_time(pchan, tag, type, time);
            break;

        default:
            break;
    }

    return;
}

hi_void channel_proc_entry(struct seq_file *p, OMXVDEC_CHAN_CTX *pchan)
{
    hi_s32 i;
    hi_u32 image_id = INVALID_IMAGE_ID;
    omxvdec_buf *p_vdec_buf_table = HI_NULL;
    if (pchan == HI_NULL) {
        omx_print(OMX_FATAL, "%s: pchan = HI_NULL, invalid\n", __func__);
        return;
    }

    if (pchan->last_frame.ext_info.data != 0) {
        image_id = ((vfmw_private_frm_info *)UINT64_PTR(pchan->last_frame.ext_info.data))->image_id;
    }

    proc_print(p, "--------------- INST%2d --------------\n", pchan->channel_id);
    proc_print(p, "%-25s :%d\n", "chan_id", pchan->decoder_id);
    proc_print(p, "%-25s :%d\n", "TVP", pchan->is_tvp);
    proc_print(p, "%-25s :%d\n", "tunneled", pchan->tunnel_mode_enable);
    proc_print(p, "%-25s :%d\n", "overlay", pchan->is_overlay);
    proc_print(p, "%-25s :%d\n", "test", pchan->android_test);
    proc_print(p, "%-25s :%d\n", "AFBC", pchan->output_afbc);
    proc_print(p, "%-25s :%d\n", "10BIT", pchan->is_10bit);
    proc_print(p, "%-25s :%s\n", "state", channel_show_chan_state(pchan->state));
    proc_print(p, "%-25s :%s\n", "protocol", channel_show_protocol(pchan->protocol));
    proc_print(p, "%-25s :%dx%d\n", "resolution", pchan->out_width, pchan->out_height);
    proc_print(p, "%-25s :%d\n", "stride", pchan->out_stride);
    proc_print(p, "%-25s :%s\n", "color_format", channel_show_color_format(pchan->color_format));
    proc_print(p, "%-25s :%d\n", "eos recv num", pchan->eos_info.eos_receive_num);
    proc_print(p, "%-25s :%d\n", "eos_in_list", pchan->eos_info.eos_in_list);
    proc_print(p, "%-25s :%d\n", "eos send num", pchan->eos_info.eos_send_num);
    proc_print(p, "%-25s :%d\n", "eos status", pchan->eos_info.status);
    proc_print(p, "%-25s :%d(%d)\n", "report/last ID", pchan->eos_info.image_id, image_id);
    proc_print(p, "%-25s :%d\n", "lowdelay", pchan->b_lowdelay);
    proc_print(p, "%-25s :%d\n", "frame_rate", pchan->last_frame.frame_rate);
    proc_print(p, "%-25s :%d\n", "anw_meta", pchan->anw_store_meta);
    proc_print(p, "%-25s :%d\n", "work_number", pchan->work_number);

    if (VFMW_VP6 == pchan->protocol || VFMW_VP6A == pchan->protocol || VFMW_VP6F == pchan->protocol) {
        proc_print(p, "%-25s :%d\n", "VP6 reversed", pchan->bReversed);
    }

    proc_print(p, "\n");
    proc_print(p, "%-25s :%d/%d\n", "ETB/EBD", pchan->omx_chan_statinfo.ETB, pchan->omx_chan_statinfo.EBD);
    proc_print(p, "%-25s :%d/%d\n", "FTB/FBD", pchan->omx_chan_statinfo.FTB, pchan->omx_chan_statinfo.FBD);

    proc_print(p, "%-25s :%d/%d\n", "acquire_stream(try/OK)", pchan->omx_chan_statinfo.get_stream_try,
               pchan->omx_chan_statinfo.get_stream_ok);
    proc_print(p, "%-25s :%d/%d\n", "release_stream(try/OK)", pchan->omx_chan_statinfo.release_stream_try,
               pchan->omx_chan_statinfo.release_stream_ok);
    proc_print(p, "%-25s :%d/%d\n", "report_image(try/OK)", pchan->omx_chan_statinfo.report_image_try,
               pchan->omx_chan_statinfo.report_image_ok);
    proc_print(p, "%-25s :%d/%d\n", "release_image(try/OK)", pchan->omx_chan_statinfo.release_image_try,
               pchan->omx_chan_statinfo.release_image_ok);

    proc_print(p, "\n");

    proc_print(p, "%-10s :%d/%d\n", "in buffer", pchan->input_buf_num, pchan->max_in_buf_num);
    proc_print(p, " %-10s%-10s%-10s%-10s\n", "phyaddr", "size", "fill", "status");

    p_vdec_buf_table = (omxvdec_buf *)pchan->in_buf_table;
    for (i = 0; i < pchan->input_buf_num; i++) {
        proc_print(p, " %-10x%-10d%-10d%-10s\n",
                   p_vdec_buf_table[i].phy_addr,
                   p_vdec_buf_table[i].buf_len,
                   p_vdec_buf_table[i].act_len,
                   channel_show_buffer_state(p_vdec_buf_table[i].status));
    }
    proc_print(p, "\n");

    proc_print(p, "%-10s :%d/%d\n", "out buffer", pchan->output_buf_num, pchan->max_out_buf_num);
    proc_print(p, " %-10s%-10s%-10s%-10s\n", "phyaddr", "size", "fill", "status");
    p_vdec_buf_table = (omxvdec_buf *)pchan->out_buf_table;
    for (i = 0; i < pchan->output_buf_num; i++) {
        proc_print(p, " %-10x%-10d%-10d%-10s\n",
                   p_vdec_buf_table[i].phy_addr,
                   p_vdec_buf_table[i].buf_len,
                   p_vdec_buf_table[i].act_len,
                   channel_show_buffer_state(p_vdec_buf_table[i].status));
    }
    proc_print(p, "\n");

    if ((pchan->b_lowdelay || g_fast_output_mode) && (pchan->omx_vdec_lowdelay_proc_rec != HI_NULL)) {
        proc_print(p, "--------------- LOWDELAY TIME--------------\n");
        proc_print(p, "%-50s(ms) :%d\n", "empty this buffer       -> decoder receive stream",
                   pchan->omx_vdec_lowdelay_proc_rec->time_record[OMX_LOWDELAY_REC_NODE_NUM]
                   [OMX_LOWDELAY_REC_VFMW_RCV_STRM_TIME] -
                   pchan->omx_vdec_lowdelay_proc_rec->time_record[OMX_LOWDELAY_REC_NODE_NUM]
                   [OMX_LOWDELAY_REC_ETB_TIME]);
        proc_print(p, "%-50s(ms) :%d\n", "decoder receive stream  -> decoder report image",
                   pchan->omx_vdec_lowdelay_proc_rec->time_record[OMX_LOWDELAY_REC_NODE_NUM]
                   [OMX_LOWDELAY_REC_VFMW_RPO_IMG_TIME] -
                   pchan->omx_vdec_lowdelay_proc_rec->time_record[OMX_LOWDELAY_REC_NODE_NUM]
                   [OMX_LOWDELAY_REC_VFMW_RCV_STRM_TIME]);
        proc_print(p, "%-50s(ms) :%d\n", "decoder report image    -> processor receive image",
                   pchan->omx_vdec_lowdelay_proc_rec->time_record[OMX_LOWDELAY_REC_NODE_NUM]
                   [OMX_LOWDELAY_REC_VPSS_RCV_IMG_TIME] -
                   pchan->omx_vdec_lowdelay_proc_rec->time_record[OMX_LOWDELAY_REC_NODE_NUM]
                   [OMX_LOWDELAY_REC_VFMW_RPO_IMG_TIME]);
        proc_print(p, "%-50s(ms) :%d\n", "processor receive image -> processor report image",
                   pchan->omx_vdec_lowdelay_proc_rec->time_record[OMX_LOWDELAY_REC_NODE_NUM]
                   [OMX_LOWDELAY_REC_VPSS_RPO_IMG_TIME] -
                   pchan->omx_vdec_lowdelay_proc_rec->time_record[OMX_LOWDELAY_REC_NODE_NUM]
                   [OMX_LOWDELAY_REC_VPSS_RCV_IMG_TIME]);
        proc_print(p, "%-50s(ms) :%d\n", "total used time",
                   pchan->omx_vdec_lowdelay_proc_rec->time_record[OMX_LOWDELAY_REC_NODE_NUM]
                   [OMX_LOWDELAY_REC_TOTAL_USED_TIME]);
        proc_print(p, "%s(%d %s) %26s(ms) :%d\n", "average time", g_low_delay_count_frame, "frame", " ",
                   pchan->omx_vdec_lowdelay_proc_rec->average_time_cost);
    }

    proc_print(p, "-------------------------------------\n");
}

