/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description:  definitions of buffer manager.
 * Author: sdk
 * Create: 2019-07-18
 */
#include <linux/ion.h>

#include "drv_venc_efl.h"
#include "drv_omxvenc_efl.h"
#include "hi_drv_mem.h"
#include "linux/hisilicon/securec.h"

#include "drv_venc.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern optm_venc_chn g_venc_chn[HI_VENC_MAX_CHN];
extern vedu_efl_ip_ctx g_vedu_ip_ctx;

#define D_VENC_GET_CHN(ve_chn, venc_chn) do { \
    if ((venc_chn) == NULL) { \
        (ve_chn) = HI_VENC_MAX_CHN; \
        break; \
    } \
    for ((ve_chn) = 0; (ve_chn) < HI_VENC_MAX_CHN; (ve_chn)++) { \
        if (g_venc_chn[ve_chn].venc_handle == (venc_chn)) { break; } \
    } \
} while (0)


/******************************************************************************
Function   :
Description:
Calls      :
Input      :
Output     :
Return     :
Others     :
******************************************************************************/
hi_s32 venc_drv_efl_put_msg_omx(queue_info *queue, hi_u32 msg_id, hi_u32 status, void *msg_data)
{
    int ret;

    if (queue == NULL) {
        HI_ERR_VENC("null point!!\n");
        return HI_FAILURE;
    }

    ret = venc_drv_mng_queue(queue, queue->omx, msg_data, msg_id, status, sizeof(hi_venc_user_buf));
    if (ret == HI_SUCCESS) {
        venc_drv_efl_wake_up_thread();
    }

    return ret;
}

/******************************************************************************
Function   :
Description:
Calls      :
Input      :
Output     :
Return     :
Others     :
******************************************************************************/
hi_s32 venc_drv_efl_get_msg_omx(queue_info *queue, hi_venc_msg_info *msg_info)
{
    if (msg_info == NULL || queue == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return HI_FAILURE;
    }

    if (venc_drv_mng_dequeue(queue, msg_info, sizeof(hi_venc_msg_info))) {
        return HI_FAILURE;
    }

    return 0;
}

/******************************************************************************
Function   :
Description:
Calls      :
Input      :
Output     :
Return     :
Others     :
******************************************************************************/
hi_s32 venc_drv_efl_mmap_to_kernel_omx(vedu_efl_enc_para *enc_para, venc_ioctl_mmz_map *mmb_info)
{
    hi_s32 ret;
    hi_s32 i = 0;
    hi_u32 chan_id = 0;
    venc_buffer buf_venc = {0};

    D_VENC_GET_CHN(chan_id, enc_para);
    D_VENC_CHECK_CHN(chan_id);

    if (mmb_info == NULL) {
        HI_ERR_VENC("bad input!!\n");
        return HI_FAILURE;
    }

    buf_venc.fd = mmb_info->buf_handle;
    buf_venc.size = mmb_info->buf_size;

    ret = hi_drv_venc_handle_map(&buf_venc);
    if (ret != HI_SUCCESS) {
        mmb_info->phys_addr = 0;
        mmb_info->virt_addr = HI_NULL;
        return ret;
    }

    mmb_info->phys_addr = buf_venc.start_phy_addr;
    mmb_info->virt_addr = buf_venc.start_vir_addr;

    if (mmb_info->dir == HI_VENC_PORT_DIROUTPUT) {
        for (i = 0; i < OMX_OUTPUT_PORT_BUFFER_MAX; i++) {
            if (enc_para->omx_output_buf[i].start_vir_addr == 0) {
                enc_para->omx_output_buf[i].dma_buf = buf_venc.dma_buf;
                enc_para->omx_output_buf[i].start_vir_addr = buf_venc.start_vir_addr;
                enc_para->omx_output_buf[i].start_phy_addr = buf_venc.start_phy_addr;
                enc_para->omx_output_buf[i].fd = buf_venc.fd; // use fd to find the buf when unmap
                enc_para->omx_output_buf[i].size = buf_venc.size;
                break;
            }
        }

        if (i == OMX_OUTPUT_PORT_BUFFER_MAX) {
            HI_ERR_VENC("i = %d, enc_para->omx_output_buf[] not idle node!\n", i);
        }
    } else {
        for (i = 0; i < OMX_INPUT_PORT_BUFFER_MAX; i++) {
            if (enc_para->omx_input_buf[i].fd == 0) {
                enc_para->omx_input_buf[i].fd = buf_venc.fd;
                enc_para->omx_input_buf[i].dma_buf = buf_venc.dma_buf;
                break;
            }
        }

        if (i == OMX_INPUT_PORT_BUFFER_MAX) {
            HI_ERR_VENC("i = %d, enc_para->omx_input_buf[] not idle node!\n", i);
        }
    }

    return ret;
}

hi_s32 venc_drv_efl_ummap_to_kernel_omx(vedu_efl_enc_para *enc_para, venc_ioctl_mmz_map *mmb_info)
{
    hi_s32 i = 0;
    hi_s32 ret;
    hi_u32 chan_id = 0;
    venc_buffer buf_venc = {0};

    D_VENC_GET_CHN(chan_id, enc_para);
    D_VENC_CHECK_CHN(chan_id);

    if (mmb_info == NULL) {
        HI_ERR_VENC("bad input!!\n");
        return HI_FAILURE;
    }

    buf_venc.fd = mmb_info->buf_handle;

    if (mmb_info->dir == HI_VENC_PORT_DIROUTPUT) {
        for (i = 0; i < OMX_OUTPUT_PORT_BUFFER_MAX; i++) {
            if (enc_para->omx_output_buf[i].fd == buf_venc.fd) {
                buf_venc.dma_buf = enc_para->omx_output_buf[i].dma_buf;
                VENC_CHECK_NEQ_RET(memset_s(&enc_para->omx_output_buf[i], sizeof(venc_buffer), 0, sizeof(venc_buffer)),
                    HI_SUCCESS, HI_FAILURE);
                break;
            }
        }

        if (i == OMX_OUTPUT_PORT_BUFFER_MAX) {
            HI_ERR_VENC("i = %d, enc_para->omx_output_buf[] can't find fd(%d)\n", i, buf_venc.fd);
            return HI_FAILURE;
        }
    } else {
        for (i = 0; i < OMX_INPUT_PORT_BUFFER_MAX; i++) {
            if (enc_para->omx_input_buf[i].fd == buf_venc.fd) {
                buf_venc.dma_buf = enc_para->omx_input_buf[i].dma_buf;
                VENC_CHECK_NEQ_RET(memset_s(&enc_para->omx_input_buf[i], sizeof(venc_buffer), 0, sizeof(venc_buffer)),
                    HI_SUCCESS, HI_FAILURE);
                break;
            }
        }

        if (i == OMX_INPUT_PORT_BUFFER_MAX) {
            HI_ERR_VENC("i = %d, enc_para->omx_input_buf[] can't find fd(%d)\n", i, buf_venc.fd);
            return HI_FAILURE;
        }
    }

    ret = hi_drv_venc_handle_unmap(&buf_venc);

    return ret;
}

static hi_s32 venc_drv_efl_flush_output_port_omx(vedu_efl_enc_para *enc_para, vedu_lock_flag *flag,
    hi_bool intra)
{
    hi_venc_msg_info queue_data;
    hi_s32 ret = 0;
    hi_u32 wait_cnt = 0;

    if (enc_para->stream_queue_omx->stm_lock) {
        venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, flag);

        /* mutex with the stop processing add by sdk */
        while ((enc_para->stream_queue_omx->stm_lock) && (wait_cnt < 500)) { /* 500:wait limit */
            osal_msleep_uninterruptible(2); /* sleep 2s */
            wait_cnt++;
        }

        if (wait_cnt >= 500) { /* 500:wait limit */
            HI_WARN_VENC("wait stm_lock timeout!\n");
        }
        venc_drv_osal_lock(g_vedu_ip_ctx.chn_lock, flag);
    }

    wait_cnt = 0;
    while ((!venc_drv_mng_queue_empty(enc_para->stream_queue_omx)) && (!ret) && (wait_cnt < 500)) { /* 500:wait limit */
        ret = (hi_s32)((hi_u32)ret | (hi_u32)venc_drv_mng_dequeue(enc_para->stream_queue_omx,
            &queue_data, sizeof(hi_venc_msg_info)));

        if (!ret) {
            enc_para->stat.stream_queue_num--;
            queue_data.buf.data_len = 0;
            if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx, HI_VENC_MSG_RESP_OUTPUT_DONE, ret,
                &queue_data.buf) != HI_SUCCESS) {
                HI_WARN_VENC("put msg omx err!\n");
            }
            enc_para->stat.fbd_cnt++;
            enc_para->stat.msg_queue_num++;
        }

        HI_INFO_VENC("############ flush output port!\n");
        wait_cnt++;
    }

    if (wait_cnt >= 500) { /* 500:wait limit */
        HI_WARN_VENC("Flush output port timeout!\n");
    }
    HI_INFO_VENC("############## flush output port ok!put message!\n");

    if (!intra) {
        if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx, HI_VENC_MSG_RESP_FLUSH_OUTPUT_DONE,
            ret, NULL) != HI_SUCCESS) {
            HI_WARN_VENC("put msg omx err!\n");
        }
        enc_para->stat.msg_queue_num++;
    }

    return ret;
}

static hi_s32 venc_drv_efl_flush_input_port_omx(vedu_efl_enc_para* enc_para, hi_u32 chan_id, hi_bool intra)
{
    hi_s32 ret = 0;
    hi_u32 wait_cnt = 0;
    hi_venc_msg_info queue_data;

    if (enc_para->image_valid) {
        if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx, HI_VENC_MSG_RESP_INPUT_DONE, HI_SUCCESS,
            &enc_para->image_omx) != HI_SUCCESS) {
            HI_ERR_VENC("put remain frame failed pts = 0x%x, 0x%x!\n", enc_para->image_omx.time_stamp0,
                enc_para->image_omx.time_stamp1);
        }
        enc_para->image_valid = HI_FALSE;
    }

    while ((!venc_drv_mng_queue_empty(enc_para->frame_queue_omx)) && (!ret) && (wait_cnt < 500)) { /* 500:wait limit */
        ret = (hi_s32)((hi_u32)ret | (hi_u32)venc_drv_mng_dequeue(enc_para->frame_queue_omx,
            &queue_data, sizeof(hi_venc_msg_info)));

        if (!ret) {
            enc_para->stat.queue_num--;
            queue_data.buf.data_len = 0;
            if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx, HI_VENC_MSG_RESP_INPUT_DONE,
                ret, &(queue_data.buf)) != HI_SUCCESS) {
                HI_WARN_VENC("put msg omx err!\n");
            }
            enc_para->stat.ebd_cnt++;
            enc_para->stat.msg_queue_num++;
        }

        HI_INFO_VENC("############ flush input port!\n");
        wait_cnt++;
    }
    if (wait_cnt >= 500) { /* 500:wait limit */
        HI_WARN_VENC("Flush input port timeout!\n");
    }
    HI_INFO_VENC("############## flush input port ok!\n");

    if (!intra) {
        if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx, HI_VENC_MSG_RESP_FLUSH_INPUT_DONE,
            HI_SUCCESS, NULL) != HI_SUCCESS) {
            HI_WARN_VENC("put msg omx err!\n");
        }
        enc_para->stat.msg_queue_num++;
    }

    return ret;
}

hi_s32 venc_drv_efl_flush_port_omx(vedu_efl_enc_para *enc_handle, hi_u32 port_index, hi_bool intra)
{
    vedu_efl_enc_para *enc_para = HI_NULL;
    vedu_lock_flag flag = 0;
    hi_u32 chan_id;
    hi_s32 ret1 = 0;
    hi_s32 ret2 = 0;

    D_VENC_GET_CHN(chan_id, enc_handle);
    D_VENC_CHECK_CHN(chan_id);

    enc_para = enc_handle;

    if (port_index != ALL_PORT && port_index > OUTPUT_PORT) {
        HI_ERR_VENC("bad PortIndex(%d)!!\n", port_index);
        return HI_FAILURE;
    }

    venc_drv_osal_lock(g_vedu_ip_ctx.chn_lock, &flag);

    if ((port_index == OUTPUT_PORT) || (port_index == ALL_PORT)) {
        ret1 = venc_drv_efl_flush_output_port_omx(enc_para, &flag, intra);
    }

    venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, &flag);

    if ((port_index == INPUT_PORT) || (port_index == ALL_PORT)) {
        ret2 = venc_drv_efl_flush_input_port_omx(enc_para, chan_id, intra);
    }

    return (ret1 || ret2);
}
/*************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
