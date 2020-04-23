/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "drv_win_ext.h"
#include "drv_venc_ext.h"
#include "drv_disp_ext.h"
#include "hi_drv_module.h"
#include "hi_drv_file.h"
#include "drv_venc_efl.h"
#include "drv_omxvenc_efl.h"
#include "drv_venc_buf_mng.h"
#include "drv_omxvenc.h"
#include "hi_drv_video.h"
#include "linux/hisilicon/securec.h"

#include "hi_drv_sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

extern optm_venc_chn g_venc_chn[HI_VENC_MAX_CHN];

#ifdef __VENC_DRV_DBG__

extern hi_u32 g_tmp_time_dbg[100][7]; /* 7,100:max count */
extern hi_u32 g_isr_id;
extern hi_u32 g_putmsg_ebd_id;
extern hi_u32 g_putmsg_fbd_id;
extern hi_u32 g_getmsg_ebd_id;
extern hi_u32 g_getmsg_fbd_id;
extern hi_bool g_flag_dbg;

#endif

#ifndef HI_ADVCA_FUNCTION_RELEASE
#define smooth_printk printk
#else
#define smooth_printk(format, arg...)
#endif


#define D_VENC_GET_CHN(ve_chn, venc_chn) do { \
    if ((venc_chn) == NULL) { \
        (ve_chn) = HI_VENC_MAX_CHN; \
        break; \
    } \
    for ((ve_chn) = 0; (ve_chn) < HI_VENC_MAX_CHN; (ve_chn)++) { \
        if (g_venc_chn[ve_chn].venc_handle == (venc_chn)) { break; } \
    } \
} while (0)


#define D_VENC_GET_PRIORITY_ID(ve_chn, priority_id) \
    do {\
        (priority_id) = 0; \
        while ((priority_id) < HI_VENC_MAX_CHN) {   \
        if (g_priority_tab[0][(priority_id)] == (ve_chn)) { \
            break; \
        } \
        (priority_id)++; \
        } \
    } while (0)

#define D_VENC_CHECK_CHN(chan) do { \
    if ((chan) >= HI_VENC_MAX_CHN) {    \
        return HI_ERR_VENC_CHN_NOT_EXIST;           \
    }                                          \
} while (0)


enum {
    OMX_VENC_YUV_420    = 0,
    OMX_VENC_YUV_422    = 1,
    OMX_VENC_YUV_444    = 2,
    OMX_VENC_YUV_NONE   = 3
};

enum {
    OMX_VENC_V_U        = 0,
    OMX_VENC_U_V        = 1
};

enum {
    OMX_VENC_STORE_SEMIPLANNAR    = 0,
    OMX_VENC_STORE_PACKAGE       = 1,
    OMX_VENC_STORE_PLANNAR      = 2
};

enum {
    OMX_VENC_PACKAGE_UY0VY1      = 141 /* 0b10001101 */,
    OMX_VENC_PACKAGE_Y0UY1V        = 216 /* 0b11011000 */,
    OMX_VENC_PACKAGE_Y0VY1U     = 120 /* 0b01111000 */
};

static UADDR get_frame_phy_addr_base(hi_u32 fd)
{
    hi_s32 ret;
    venc_buffer venc_buf;

    venc_buf.fd = fd;
    ret = hi_drv_venc_handle_map(&venc_buf);
    if (ret != HI_SUCCESS) {
        HI_INFO_VENC("Map frame physical address failed.\n");
        return ret;
    }

    return venc_buf.start_phy_addr;
}

hi_s32 venc_drv_queue_frame_omx(vedu_efl_enc_para *venc_chn, hi_venc_user_buf *frame_info)
{
    hi_s32 ret;
    hi_u32 ve_chn = 0;
    vedu_efl_enc_para *enc_chn_para = HI_NULL;
    hi_drv_video_frame frame;
    VENC_CHECK_NEQ_RET(memset_s(&frame, sizeof(hi_drv_video_frame), 0, sizeof(hi_drv_video_frame)),
        HI_SUCCESS, HI_FAILURE);
    D_VENC_GET_CHN(ve_chn, venc_chn);

    if (HI_VENC_MAX_CHN == ve_chn) {
        HI_ERR_VENC("the handle(%p) does not create either!\n", venc_chn);
        return HI_FAILURE;
    }

    enc_chn_para = venc_chn;

    if (enc_chn_para->never_enc == HI_TRUE) {
        enc_chn_para->src_info.handle         = g_venc_chn[ve_chn].user_handle;
        enc_chn_para->src_info.get_image_omx = venc_drv_efl_get_image_omx;
        enc_chn_para->src_info.put_image     = HI_NULL;
        enc_chn_para->never_enc = HI_FALSE;
    }

    if (frame_info->meta_data_flag) {
        frame_info->meta_data.buffer_addr_phy = get_frame_phy_addr_base(frame_info->buffer_handle);
        if (frame_info->meta_data.buffer_addr_phy == 0 || frame_info->meta_data.buffer_addr_phy == 0xffffffff) {
            HI_INFO_VENC("get_frame_phy_addr_base return 0x%x, invalid!\n", frame_info->meta_data.buffer_addr_phy);
            return HI_FAILURE;
        }
    } else {
        frame_info->buffer_addr_phy = get_frame_phy_addr_base(frame_info->buffer_handle);
        if (frame_info->buffer_addr_phy == 0 || frame_info->buffer_addr_phy == 0xffffffff) {
            HI_INFO_VENC("get_frame_phy_addr_base return 0x%x, invalid!\n", frame_info->buffer_addr_phy);
            return HI_FAILURE;
        }
    }

    ret = venc_drv_efl_put_msg_omx(enc_chn_para->frame_queue_omx, HI_VENC_MSG_RESP_INPUT_DONE, 0, frame_info);

    if (!ret) {
        enc_chn_para->stat.etb_cnt++;
        enc_chn_para->stat.queue_num++;
    }

    return ret;
}

hi_s32 venc_drv_queue_stream_omx(vedu_efl_enc_para *venc_chn, hi_venc_user_buf *frame_info)
{
    hi_s32 ret;
    hi_s32 ve_chn = 0;
    vedu_efl_enc_para *pstEncChnPara = HI_NULL;

    D_VENC_GET_CHN(ve_chn, venc_chn);
    if (HI_VENC_MAX_CHN == ve_chn) {
        HI_ERR_VENC("the handle(%p) does not start or even not be create either!\n", venc_chn);
        return HI_FAILURE;
    }
    pstEncChnPara = venc_chn;
    ret = venc_drv_efl_put_msg_omx(pstEncChnPara->stream_queue_omx, HI_VENC_MSG_RESP_OUTPUT_DONE, 0, frame_info);
    if (!ret) {
        pstEncChnPara->stat.ftb_cnt++;
        pstEncChnPara->stat.stream_queue_num++;
    }
    return ret;
}

#if 0
static void venc_drv_get_time_omx()
{
    int i = 0;
    unsigned int diff[7] = {0}; /* 7:total length */
    unsigned int total = 0;
    smooth_printk("\n/******************************* BEGIN ****************************************/\n");
    smooth_printk(" FrameID        queue_stream   isr   put_msg     get_msg  diff1 diff2 diff3   total_diff\n");
    for (i = 0; i < 100; i++) { /* 100:max count */
        diff[0] = g_tmp_time_dbg[i][1] - g_tmp_time_dbg[i][0]; /* 0,1 are num index */
        diff[1] = g_tmp_time_dbg[i][2] - g_tmp_time_dbg[i][1]; /* 2,1 are num index */
        diff[2] = g_tmp_time_dbg[i][3] - g_tmp_time_dbg[i][1]; /* 3,2,1 are num index */
        diff[3] = g_tmp_time_dbg[i][4] - g_tmp_time_dbg[i][2]; /* 4,3,2 are num index */
        diff[4] = g_tmp_time_dbg[i][5] - g_tmp_time_dbg[i][3]; /* 4,5,3 are num index */
        diff[5] = g_tmp_time_dbg[i][6] - g_tmp_time_dbg[i][4]; /* 5,6,4 are num index */
        total = g_tmp_time_dbg[i][6] - g_tmp_time_dbg[i][0]; /* 0,6 are num index */
        smooth_printk(" %03d    %8d   %8d  |  %8d   %8d  | %8d  %8d  ||%8d||  \n",
        i, diff[0], diff[1], diff[2], diff[3], diff[4], diff[5], total);
    }

    smooth_printk("\n/*******************************  END *******************************************/\n");

}
#endif

hi_s32 venc_drv_get_message_omx(vedu_efl_enc_para *venc_chn, hi_venc_msg_info *pmsg_info)
{
    hi_s32 ret;
    hi_s32 ve_chn = 0;
    hi_venc_msg_info msg_info;
    vedu_efl_enc_para *enc_chn_para = HI_NULL;

    D_VENC_GET_CHN(ve_chn, venc_chn);
    if (HI_VENC_MAX_CHN == ve_chn) {
        HI_ERR_VENC("the handle(%p) does not start or even not be create either!\n", venc_chn);
        return HI_FAILURE;
    }
    if (pmsg_info == NULL) {
        HI_ERR_VENC("invalid param\n");
        return HI_FAILURE;
    }
    enc_chn_para = venc_chn;

    ret = venc_drv_efl_get_msg_omx(enc_chn_para->msg_queue_omx, &msg_info);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }


#ifdef __VENC_DRV_DBG__

    if (msg_info.msg_code == HI_VENC_MSG_RESP_INPUT_DONE) { /* FBD */
        if (g_getmsg_ebd_id < 100) { /* 100:max count */
            g_tmp_time_dbg[g_getmsg_ebd_id][5] = hi_drv_sys_get_time_stamp_ms(); /* 5 is num index */
            g_getmsg_ebd_id++;
        }
    }

    if (msg_info.msg_code == HI_VENC_MSG_RESP_OUTPUT_DONE) {
        if (g_getmsg_fbd_id < 100) { /* 100:max count */
            g_tmp_time_dbg[g_getmsg_fbd_id][6] = hi_drv_sys_get_time_stamp_ms(); /* 6 is num index */
            g_getmsg_fbd_id++;
        } else if(g_flag_dbg) {
#if 0
            venc_drv_get_time_omx();
#endif
            g_flag_dbg = 0;
        }
    }
#endif

    enc_chn_para->stat.msg_queue_num--;
    VENC_CHECK_NEQ_RET(memcpy_s(pmsg_info, sizeof(hi_venc_msg_info), &msg_info, sizeof(hi_venc_msg_info)),
        HI_SUCCESS, HI_FAILURE);
    return HI_SUCCESS;
}

hi_s32 venc_drv_flush_port_omx(vedu_efl_enc_para *venc_chn, hi_u32 port_index, hi_u32 inter_flag)
{
    hi_s32 ret;
    hi_s32 ve_chn = 0;

    D_VENC_GET_CHN(ve_chn, venc_chn);
    if (ve_chn == HI_VENC_MAX_CHN) {
        HI_ERR_VENC("the handle(%p) does not start or even not be create either!\n", venc_chn);
        return HI_FAILURE;
    }

    ret = venc_drv_efl_flush_port_omx(venc_chn, port_index, inter_flag);
    return ret;
}

hi_s32 venc_drv_mmz_map_omx(vedu_efl_enc_para *venc_chn, venc_ioctl_mmz_map *mmb_info)
{
    hi_s32 ret;

    if (mmb_info == NULL) {
        HI_ERR_VENC("bad input!!\n");
        return HI_FAILURE;
    }

    ret = venc_drv_efl_mmap_to_kernel_omx(venc_chn, mmb_info);
    if ((ret != HI_SUCCESS) || (mmb_info->virt_addr == NULL)) {
        HI_ERR_VENC("VENC_DRV_EflMMapToKernel failed!\n");
        return HI_FAILURE;
    }
    HI_INFO_VENC("map the Phyaddr = 0x%x==>VirAddr = %p\n", mmb_info->phys_addr, mmb_info->virt_addr);

    return HI_SUCCESS;
}

hi_s32 venc_drv_mmz_ummap_omx(vedu_efl_enc_para *venc_chn, venc_ioctl_mmz_map *mmb_info)
{
    hi_s32 ret;

    if (mmb_info == NULL) {
        HI_ERR_VENC("bad input!!\n");
        return HI_FAILURE;
    }

    ret = venc_drv_efl_ummap_to_kernel_omx(venc_chn, mmb_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("VENC_DRV_EflUMMapToKernel failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

