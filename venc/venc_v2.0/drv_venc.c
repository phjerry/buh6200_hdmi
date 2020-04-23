/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-04-22
 */
#include "public.h"
#include "hi_osal.h"

#include "drv_win_ext.h"
#include "drv_venc_ext.h"
#include "drv_disp_ext.h"
#include "hi_drv_module.h"
#include "hi_drv_file.h"
#include "drv_venc_efl.h"
#include "drv_venc_buf_mng.h"
#include "drv_venc.h"
#include "hi_drv_video.h"
#include "hi_reg_common.h"
#include "venc_hal_ext.h"
#include "linux/hisilicon/securec.h"
#include "linux/hisilicon/hi_license.h"

#ifdef VENC_SUPPORT_ATTACH_VI
#include "drv_vi_ext.h"
#endif
#ifdef VENC_SUPPORT_JPGE
#include "drv_jpge_ext.h"
#endif

#include <linux/delay.h>
#include <linux/kernel.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define VENC_MB_WIDTH      16
#define VENC_MB_POWER      4
#define FILE_NAME_LEN      64
#define BIT_NUM_BYTE       8

#define VENC_MB_NUM_2Kx1K  10800 /* 2160x1280 */
#define VENC_MB_NUM_2Kx2K  18225
#define VENC_MB_NUM_4Kx2K  34560
#define VENC_MB_NUM_4Kx4K  65536

#define BUF_MIN_SIZE_QCIF     (176 * 144 * 2)
#define BUF_MIN_SIZE_CIF      (352 * 288 * 2)
#define BUF_MIN_SIZE_D1       (720 * 576 * 2)
#define BUF_MIN_SIZE_720P     (1280 * 720 * 2)
#define BUF_MIN_SIZE_FULLHD   (1920 * 1080 * 2)
#define BUF_MIN_SIZE_2Kx1K    (2160 * 1280 * 3 / 2)
#define BUF_MIN_SIZE_2Kx2K    (2160 * 2160 * 3 / 2)
#define BUF_MIN_SIZE_4Kx2K    (4096 * 2160 * 3 / 2)
#define BUF_MIN_SIZE_4Kx4K    (4096 * 4096 * 3 / 2)

optm_venc_chn g_venc_chn[HI_VENC_MAX_CHN];
vedu_osal_event g_venc_wait_stream[HI_VENC_MAX_CHN];
extern vedu_osal_event g_venc_event_stream;

osal_spinlock g_send_frame_lock[HI_VENC_MAX_CHN];     /* lock the destroy and send frame */
hi_u32 g_vedu_chip_id = 0;

#ifdef VENC_SUPPORT_JPGE
extern hi_jpge_export_func *g_jpge_func;
#endif

#define D_VENC_CHECK_PTR(ptr) \
    do {\
        if ((ptr) == HI_NULL)\
        { \
            HI_ERR_VENC("PTR '%s' is NULL.\n", # ptr); \
            return HI_ERR_VENC_NULL_PTR;           \
        }  \
    } while (0)

#define D_VENC_GET_CHN(ve_chn, venc_chn) do { \
    if ((venc_chn) == NULL) { \
        (ve_chn) = HI_VENC_MAX_CHN; \
        break; \
    } \
    for ((ve_chn) = 0; (ve_chn) < HI_VENC_MAX_CHN; (ve_chn)++) { \
        if (g_venc_chn[ve_chn].venc_handle == (venc_chn)) { break; } \
    } \
} while (0)

#define D_VENC_GET_CHN_BY_UHND(ve_chn, venc_usr_chn) \
do { \
    if ((venc_usr_chn) == HI_INVALID_HANDLE) { \
        (ve_chn) = HI_VENC_MAX_CHN; \
        break; \
    } \
    for ((ve_chn) = 0; (ve_chn) < HI_VENC_MAX_CHN; (ve_chn)++) { \
        if (g_venc_chn[ve_chn].user_handle == (venc_usr_chn)) { break; } \
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

#define D_VENC_BOOL_INVALID(bool_val) ((bool_val) != HI_TRUE && (bool_val) != HI_FALSE)

hi_s8 g_priority_tab[2][16] =  /* 2,16:subscript */
    {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {}};

extern drv_venc_export_func g_venc_export_funcs;

static hi_s32 check_pic_size(hi_u32 width, hi_u32 height, hi_venc_std standard, hi_bool omx_chn, hi_bool max_flag)
{
    hi_u32 min_height;
    hi_u32 max_height;
    hi_u32 min_width;
    hi_u32 max_width;

    if (standard == HI_VENC_STD_JPEG) {
        min_height = HI_JPEG_MIN_HEIGTH;
        min_width  = HI_JPEG_MIN_WIDTH;
        max_height = HI_JPEG_MAX_HEIGTH;
        max_width  = HI_JPEG_MAX_WIDTH;
    } else {
        if (omx_chn) {
            min_height = HI_OMXVENC_MIN_HEIGTH;
            min_width  = HI_OMXVENC_MIN_WIDTH;
            max_height = HI_VENC_MAX_HEIGTH;
            max_width  = HI_VENC_MAX_WIDTH;
        } else {
            min_height = HI_VENC_MIN_HEIGTH;
            min_width  = HI_VENC_MIN_WIDTH;
            max_height = HI_VENC_MAX_HEIGTH;
            max_width  = HI_VENC_MAX_WIDTH;
        }
    }

    if ((height < min_height) || (width < min_width)) {
        HI_ERR_VENC("picture %s resolutioin: width(%u) or heigth(%u) too small.\n", (max_flag) ? "max" : "encoder",
            width, height);
        return HI_ERR_VENC_INVALID_PARA;
    }

    if ((height > max_height) || (width > max_width)) {
        HI_ERR_VENC("picture %s resolution: width(%u) or heigth(%u) too large.\n", (max_flag) ? "max" : "encoder",
            width, height);
        return HI_ERR_VENC_INVALID_PARA;
    }

    if ((height % HI_VENC_PIC_SZIE_ALIGN) || (width % HI_VENC_PIC_SZIE_ALIGN)) {
        HI_ERR_VENC("picture %s resolution: width(%u) or heigth(%u) invalid, should N*%d.\n",
            (max_flag) ? "max" : "encoder", width, height, HI_VENC_PIC_SZIE_ALIGN);
        return HI_ERR_VENC_INVALID_PARA;
    }

    return HI_SUCCESS;
}
static hi_s32 check_venc_type(hi_venc_std venc_type)
{
    hi_bool flag = HI_FALSE;

    if (venc_type == HI_VENC_STD_H264) {
        flag = HI_TRUE;
    }
#ifdef VENC_SUPPORT_JPGE
    if (venc_type == HI_VENC_STD_JPEG) {
        flag = HI_TRUE;
    }
#endif
#ifdef VENC_SUPPORT_H265
    if (venc_type == HI_VENC_STD_H265) {
        flag = HI_TRUE;
    }
#endif

    return (flag == HI_TRUE) ? HI_SUCCESS : HI_FAILURE;
}

static hi_s32 check_chan_created_attr(hi_venc_chan_info *attr, hi_bool omx_chn)
{
    hi_s32 ret;

    if (check_venc_type(attr->standard) != HI_SUCCESS) {
        HI_ERR_VENC("NOT support type(%d) VENC channel.\n", attr->standard);
        return HI_ERR_VENC_NOT_SUPPORT;
    }

    ret = check_pic_size(attr->max_width, attr->max_height, attr->standard, omx_chn, HI_TRUE);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if ((attr->strm_buf_size < HI_VENC_MIN_BUF_SIZE) || (attr->strm_buf_size > HI_VENC_MAX_BUF_SIZE)) {
        HI_ERR_VENC("strm_buf_size(%u) invalid.strm_buf_size should larger than %d and smaller than %d\n",
            attr->strm_buf_size, HI_VENC_MIN_BUF_SIZE, HI_VENC_MAX_BUF_SIZE);
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (D_VENC_BOOL_INVALID(attr->secure)) {
        HI_ERR_VENC("secure(%u) is invalid.\n", attr->secure);
        return HI_ERR_VENC_INVALID_PARA;
    }

    /* JPEG need not to check following attributes */
    if (attr->standard == HI_VENC_STD_JPEG) {
        return HI_SUCCESS;
    }

    if (attr->standard == HI_VENC_STD_H264) {
        if (attr->profile >= HI_VENC_H264_PROFILE_MAX) {
            HI_ERR_VENC("profile(%d) invalid.\n", attr->profile);
            return HI_ERR_VENC_INVALID_PARA;
        }

        if (attr->profile == HI_VENC_H264_PROFILE_EXTENDED) {
            HI_ERR_VENC("profile(%d) not support.\n", attr->profile);
            return HI_ERR_VENC_NOT_SUPPORT;
        }
    }

    if (attr->gop_mode >= HI_VENC_GOP_MODE_MAX) {
        HI_ERR_VENC("gop_type(%u) is invalid, should < %u.\n", attr->gop_mode, HI_VENC_GOP_MODE_BIPREDB);
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (D_VENC_BOOL_INVALID(attr->slc_split_enable)) {
        HI_ERR_VENC("slc_split_enable(%u) is invalid.\n", attr->slc_split_enable);
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (attr->slc_split_enable) {
        if ((attr->split_size > HI_VENC_MAX_SPLIT_BYTE_SIZE) || (attr->split_size < HI_VENC_MIN_SPLIT_BYTE_SIZE)) {
            HI_ERR_VENC("split_size(%u) is invalid.should < %u && > %u .\n", attr->split_size,
                        HI_VENC_MAX_SPLIT_BYTE_SIZE, HI_VENC_MIN_SPLIT_BYTE_SIZE);
            return HI_ERR_VENC_INVALID_PARA;
        }
    }

    return ret;
}

static hi_s32 check_chan_common_config(hi_venc_chan_config *config)
{
    if ((config->input_frame_rate < HI_VENC_MIN_fps) || (config->input_frame_rate > HI_VENC_MAX_fps)) {
        HI_ERR_VENC("input_frame_rate(%u) is invalid.\n", config->input_frame_rate);
        return HI_ERR_VENC_INVALID_PARA;
    }

    if ((config->target_frame_rate < HI_VENC_MIN_fps) || (config->target_frame_rate > config->input_frame_rate)) {
        HI_ERR_VENC("target_frame_rate(%u) is invalid, should 1~input_frame_rate.\n", config->target_frame_rate);
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (config->priority >= HI_VENC_MAX_PRIORITY) {
        HI_ERR_VENC("priority(%u) is invalid, should < max_priority(%u).\n", config->priority, HI_VENC_MAX_PRIORITY);
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (D_VENC_BOOL_INVALID(config->quick_encode)) {
        HI_ERR_VENC("quick_encode(%u) is invalid.\n", config->quick_encode);
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (config->frm_rate_type < HI_VENC_FRMRATE_AUTO || config->frm_rate_type > HI_VENC_FRMRATE_USER) {
        HI_ERR_VENC("frm_rate_type(%u) is invalid.\n", config->frm_rate_type);
        return HI_ERR_VENC_INVALID_PARA;
    }

    return HI_SUCCESS;
}

static hi_s32 check_chan_config(hi_venc_chan_info *attr, hi_venc_chan_config *config, hi_bool omx_chn)
{
    hi_s32 ret;

    ret = check_pic_size(config->width, config->height, attr->standard, omx_chn, HI_FALSE);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (config->width > attr->max_width || config->height > attr->max_height) {
        HI_ERR_VENC("current encode size %dx%d should smaller than max size %dx%d\n",
            config->width, config->height, attr->max_width, attr->max_height);
        return HI_ERR_VENC_INVALID_PARA;
    }

    ret = check_chan_common_config(config);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* JPEG need not to check following attributes */
    if (attr->standard == HI_VENC_STD_JPEG) {
#ifdef VENC_SUPPORT_JPGE
        if ((config->qfactor < HI_VENC_MIN_Q_VALUE) || (config->qfactor > HI_VENC_MAX_Q_VALUE)) {
            HI_ERR_VENC("quality_level(%u) is invalid.\n", config->qfactor);
            return HI_ERR_VENC_INVALID_PARA;
        }
#endif
        return HI_SUCCESS;
    }

    if ((config->target_bitrate < HI_VENC_MIN_bps) || (config->target_bitrate > HI_VENC_MAX_bps)) {
        HI_ERR_VENC("target_bit_rate(%u) is invalid.\n", config->target_bitrate);
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (config->gop < HI_VENC_MIN_GOP) {
        HI_ERR_VENC("gop(%u) is invalid, should > %u.\n", config->gop, HI_VENC_MIN_GOP);
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (attr->gop_mode == HI_VENC_GOP_MODE_DUALP) {
        if (config->sp_interval >= config->gop || config->sp_interval == 1) {
            HI_ERR_VENC("sp_interval(%u) is invalid, should < %u and != 1.\n", config->sp_interval, config->gop);
            return HI_ERR_VENC_INVALID_PARA;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 check_chan_attr(hi_venc_chan_info *attr, hi_bool omx_chn)
{
    hi_s32 ret;

    ret = check_chan_created_attr(attr, omx_chn);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = check_chan_config(attr, &attr->config, omx_chn);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* rate control parameter check */
    if (attr->rc_param.min_qp > HI_VENC_MAX_QP) {
        HI_ERR_VENC("min_qp(%u) is invalid, should < %u.\n", attr->rc_param.min_qp, HI_VENC_MAX_QP);
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (attr->rc_param.max_qp > HI_VENC_MAX_QP) {
        HI_ERR_VENC("max_qp(%u) is invalid, should < %u.\n", attr->rc_param.max_qp, HI_VENC_MAX_QP);
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (attr->rc_param.min_qp >= attr->rc_param.max_qp) {
        HI_ERR_VENC("min_qp(%u) is invalid, should < max_qp(%u).\n", attr->rc_param.min_qp, attr->rc_param.max_qp);
        return HI_ERR_VENC_INVALID_PARA;
    }

    /* frame drop strategy check */
    if (attr->frm_drop_strategy.bit_rate_threshold != (-1)) {
        if (attr->frm_drop_strategy.bit_rate_threshold > HI_VENC_MAX_RcThr) {
            HI_ERR_VENC("drift_rate_threshold(%u) is invalid,should smaller than %d\n",
                attr->frm_drop_strategy.bit_rate_threshold, HI_VENC_MAX_RcThr);
            return HI_ERR_VENC_INVALID_PARA;
        }
    }

    return HI_SUCCESS;
}



static hi_s32 check_attach_module(hi_mod_id mod_id)
{
    hi_bool flag = HI_FALSE;

#ifdef VENC_SUPPORT_ATTACH_VI
    if (mod_id == HI_ID_VI) {
        flag = HI_TRUE;
    }
#endif
#ifdef VENC_SUPPORT_ATTACH_VO
    if (mod_id == HI_ID_WIN) {
        flag = HI_TRUE;
    }
#endif
#ifdef VENC_SUPPORT_ATTACH_CAST
    if (mod_id == HI_ID_DISP) {
        flag = HI_TRUE;
    }
#endif

    return (flag == HI_TRUE) ? HI_SUCCESS : HI_FAILURE;
}

hi_void venc_drv_init_event(hi_void)
{
    hi_u32 i;

    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        venc_drv_osal_init_event(&g_venc_wait_stream[i], 0);
        osal_spin_lock_init(&g_send_frame_lock[i]);
    }

    venc_drv_osal_init_event(&g_venc_event_stream, 0);

    g_vedu_chip_id = 2; /* set chip id 2 */
}

hi_void venc_drv_deinit_event(hi_void)
{
    hi_u32 i;

    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        venc_drv_osal_deinit_event(&g_venc_wait_stream[i]);
        osal_spin_lock_destory(&g_send_frame_lock[i]);
    }

    venc_drv_osal_deinit_event(&g_venc_event_stream);
}


hi_void venc_drv_board_init(hi_void)
{
    /* open vedu clock  && set clock freq */
    VENC_PDT_OPEN_HARDWARE();
    /* cancel reset */
    VENC_PDT_RESET_HARDWARE();
}

hi_void venc_drv_board_deinit(hi_void)
{
    /* close vedu clock */
    VENC_PDT_CLOSE_HARDWARE();
    /* reset */
    VENC_PDT_RESET_HARDWARE();
}

static hi_void venc_drv_set_cfg_info(hi_venc_chan_info *attr, vedu_efl_enc_cfg *enc_cfg)
{
    switch (attr->standard) {
#ifdef VENC_SUPPORT_H265
        case HI_VENC_STD_H265:
            enc_cfg->protocol = VEDU_H265;
            enc_cfg->profile  = (hi_u32)attr->profile;
            enc_cfg->gop      = attr->config.gop;
            enc_cfg->q_level   = 0;
            break;
#endif
        case HI_VENC_STD_H264:
            enc_cfg->protocol = VEDU_H264;
            enc_cfg->profile  = (hi_u32)attr->profile;
            enc_cfg->gop      = attr->config.gop;
            enc_cfg->q_level   = 0;
            break;
#ifdef VENC_SUPPORT_JPGE
        case HI_VENC_STD_JPEG:
            enc_cfg->protocol = VEDU_JPGE;
            enc_cfg->profile  = VEDU_H264_PROFILE_BUTT;
            enc_cfg->q_level   = attr->config.qfactor;
            enc_cfg->gop      = 0;
            break;
#endif
        default:
            enc_cfg->protocol = VEDU_H264;
            break;
    }

}

static hi_void venc_drv_get_rc_skip_frm_en(vedu_efl_enc_cfg *enc_cfg)
{
    /* if drift_rc_thr is larger than 100, enable */
    enc_cfg->rc_skip_frm_en = (enc_cfg->drift_rc_thr <= 100) ? HI_TRUE : HI_FALSE; /* 100:threshold */
}

static hi_void venc_drv_get_split_size(vedu_efl_enc_cfg *enc_cfg, hi_u32 split_size)
{
    enc_cfg->split_size = (enc_cfg->slc_split_en) ? split_size : 0;
}

hi_bool venc_drv_check_chn_state_safe(hi_u32 chan_id)
{
    if (chan_id >= HI_VENC_MAX_CHN) {
        return HI_FALSE;
    }

    if ((g_venc_chn[chan_id].chan_state == VEDU_STATE_DESTORYING)
        || (g_venc_chn[chan_id].chan_state == VEDU_STATE_BUTT)) {
        return HI_FALSE;
    } else {
        return HI_TRUE;
    }
}

static void free_jpeg_buf(vedu_efl_enc_cfg *enc_cfg, vedu_efl_enc_para *enc_chn_para)
{
#ifdef VENC_SUPPORT_JPGE
    if (enc_cfg->protocol == VEDU_JPGE) {
        venc_drv_efl_free_buf_2_jpge(enc_chn_para);
    }
#endif
}

void jpeg_destroy_module(vedu_efl_enc_cfg *enc_cfg, hi_handle jpeg_chn)
{
#ifdef VENC_SUPPORT_JPGE
    if (enc_cfg->protocol == VEDU_JPGE) {
        if (g_jpge_func != HI_NULL) {
            g_jpge_func->drv_jpge_module_destory(jpeg_chn);
        }
    }
#endif
}

static hi_s32 get_enc_cfg(vedu_efl_enc_para** venc_chan, hi_venc_chan_info *attr, vedu_efl_enc_cfg *enc_cfg,
    hi_bool omx_chn, hi_s32 *i)
{
    hi_s32 ret;
    ret = check_chan_attr(attr, omx_chn);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    for (*i = 0; *i < HI_VENC_MAX_CHN; (*i)++) {
        if (g_venc_chn[*i].venc_handle == NULL) {
            break;
        }
    }

    if (*i == HI_VENC_MAX_CHN) {
        HI_ERR_VENC("channal create err! \n");
        return HI_ERR_VENC_CREATE_ERR;
    }

    venc_drv_set_cfg_info(attr, enc_cfg);

    enc_cfg->auto_request_ifrm = attr->omx_private.auto_request_i_frame;
    enc_cfg->prepend_sps_pps = attr->omx_private.prepend_sps_pps;
    enc_cfg->is_avbr        = (attr->omx_private.control_rate_type != HI_VENC_CONTROL_RATE_CONSTANT) ? 1 : 0;
    enc_cfg->gop_type      = attr->gop_mode;

    if (attr->gop_mode == HI_VENC_GOP_MODE_BIPREDB) {
        enc_cfg->b_frm_num = 0;
    }

    if (attr->gop_mode == HI_VENC_GOP_MODE_DUALP) {
        enc_cfg->sp_interval = attr->config.sp_interval;
    }
    enc_cfg->rcn_ref_share_buf = 1;
    enc_cfg->max_width = attr->max_width;
    enc_cfg->max_height = attr->max_height;
    enc_cfg->frame_width    = attr->config.width;
    enc_cfg->frame_height   = attr->config.height;
    enc_cfg->rotation_angle = 0;
    enc_cfg->priority       = attr->config.priority;
    enc_cfg->stream_buf_size = attr->strm_buf_size;
    enc_cfg->quick_encode   = attr->config.quick_encode;
    enc_cfg->slc_split_en   = (HI_TRUE == attr->slc_split_enable) ? 1 : 0;
    enc_cfg->drift_rc_thr   = attr->frm_drop_strategy.bit_rate_threshold;
    enc_cfg->priv_handle    = *i;
    venc_drv_get_rc_skip_frm_en(enc_cfg);

    enc_cfg->omx_chn  = omx_chn;
    ret = venc_drv_efl_create_chan(venc_chan, enc_cfg);

    return ret;
}

static void init_chan_para_part1(vedu_efl_enc_para *enc_chn_para, hi_ioctl_strm_buf_info *buf_info,
    hi_s32 i, struct file *file)
{
    buf_info->handle            = g_venc_chn[i].user_handle; /* venc_chan; */
    buf_info->strm_buf_fd       = enc_chn_para->hal.strm_buf_fd;
    buf_info->strm_buf_phy_addr = enc_chn_para->hal.strm_buf_addr;
    buf_info->buf_size        = enc_chn_para->hal.strm_buf_size;
    buf_info->slice_buf_phy_addr = enc_chn_para->hal.slice_buf.start_phy_addr;
    buf_info->slice_buf_fd = enc_chn_para->hal.slice_buf.fd;
    buf_info->buf_size_slice = enc_chn_para->hal.slice_buf.size;
    buf_info->strm_buf_phy_addr_jpeg = enc_chn_para->jpg_mmz_buf.start_phy_addr;

    buf_info->buf_size_jpeg    = enc_chn_para->jpg_mmz_buf.size;
    buf_info->protocol       = enc_chn_para->protocol;
    g_venc_chn[i].venc_handle   = enc_chn_para;
    g_venc_chn[i].source       = HI_INVALID_HANDLE;
    g_venc_chn[i].which_file    = file;
    g_venc_chn[i].frame_num_last_input    = 0;
    g_venc_chn[i].frame_num_last_encoded  = 0;
    g_venc_chn[i].total_byte_last_encoded = 0;
    g_venc_chn[i].last_sec_input_fps      = 0;
    g_venc_chn[i].last_sec_encoded_fps    = 0;
    g_venc_chn[i].last_sec_kbps           = 0;
    g_venc_chn[i].last_sec_try_num        = 0;
    g_venc_chn[i].last_try_num_total      = 0;
    g_venc_chn[i].last_sec_ok_num         = 0;
    g_venc_chn[i].last_ok_num_total       = 0;
    g_venc_chn[i].last_sec_put_num        = 0;
    g_venc_chn[i].last_put_num_total      = 0;
    g_venc_chn[i].enable                 = HI_FALSE;
    g_venc_chn[i].chan_state             = VEDU_STATE_CREATE;
    g_venc_chn[i].src_mod_id              = HI_ID_MAX;
}

static void init_chan_para_part2(hi_venc_chan_info *attr, hi_s32 i, hi_bool omx_chn, hi_handle jpeg_chn,
    vedu_efl_enc_cfg *enc_cfg)
{
    g_venc_chn[i].omx_chn                 = omx_chn;
    g_venc_chn[i].proc_write.frame_mode_run     = HI_FALSE;
    g_venc_chn[i].proc_write.time_mode_run      = HI_FALSE;
    g_venc_chn[i].proc_write.save_yuv_file_run   = HI_FALSE;
    g_venc_chn[i].proc_write.frame_mode_count = 0;
    g_venc_chn[i].proc_write.fp_save_file        = HI_NULL;
    g_venc_chn[i].jpge                         = jpeg_chn;

    g_venc_chn[i].chn_user_cfg  = *attr;

    if (enc_cfg->protocol == VEDU_JPGE) {
#ifdef VENC_SUPPORT_JPGE
        g_venc_chn[i].chn_user_cfg.config.gop = 0;
        g_venc_chn[i].chn_user_cfg.config.target_bitrate = 0;
        g_venc_chn[i].chn_user_cfg.rc_param.max_qp = 0;
        g_venc_chn[i].chn_user_cfg.rc_param.min_qp = 0;
        g_venc_chn[i].chn_user_cfg.profile = HI_VENC_H264_PROFILE_MAX;
#endif
    } else {
        g_venc_chn[i].chn_user_cfg.config.qfactor = 0;
    }

}

static hi_s32 get_name(vedu_efl_enc_para *enc_chn_para, hi_s32 i)
{
    hi_s32 ret;
    hi_char yuv_file_name[FILE_NAME_LEN] = {""};
    hi_char stream_file_name[FILE_NAME_LEN] = {""};

    ret = snprintf_s(yuv_file_name, FILE_NAME_LEN, FILE_NAME_LEN, "venc_proc_chn%02d.yuv", i);
    if (ret < 0) {
        HI_ERR_VENC("snprintf failed!\n");
        return HI_FAILURE;
    }
    VENC_CHECK_NEQ_RET(memcpy_s(g_venc_chn[i].proc_write.yuv_file_name, FILE_NAME_LEN, yuv_file_name, FILE_NAME_LEN),
        HI_SUCCESS, HI_FAILURE);

    ret = snprintf_s(stream_file_name, FILE_NAME_LEN, FILE_NAME_LEN, "venc_proc_chn%02d.h264", i);
    if (ret < 0) {
        HI_ERR_VENC("snprintf failed!\n");
        return HI_FAILURE;
    }
    VENC_CHECK_NEQ_RET(memcpy_s(g_venc_chn[i].proc_write.stream_file_name, FILE_NAME_LEN, stream_file_name,
        FILE_NAME_LEN), HI_SUCCESS, HI_FAILURE);
    HI_INFO_VENC("create OK, chn:%d/%#x.\n", i, g_venc_chn[i].venc_handle);

    return HI_SUCCESS;
}

#ifdef VENC_SUPPORT_JPGE
static hi_s32 jpeg_creat_chn(hi_venc_chan_info *attr, hi_handle *jpeg_chn, vedu_efl_enc_cfg *enc_cfg,
    hi_jpge_cfg *jpeg_enc_cfg, vedu_efl_enc_para *enc_chn_para)
{
    hi_s32 ret = HI_FAILURE;
    if (enc_cfg->protocol == VEDU_JPGE) {
        if (g_jpge_func == HI_NULL) {
            HI_ERR_VENC("g_jpge_func is NULL!\n");
            venc_drv_efl_destroy_venc(enc_chn_para);
            return ret;
        }
        jpeg_enc_cfg->frame_width  = attr->width;
        jpeg_enc_cfg->frame_height = attr->height;
        jpeg_enc_cfg->slice_split_enable  = 0; /* enc_cfg.slc_split_en; */
        jpeg_enc_cfg->slice_split_size   = enc_cfg->split_size;
        jpeg_enc_cfg->quality_level      = attr->quality_level;

        ret = g_jpge_func->drv_jpge_module_create(jpeg_chn, jpeg_enc_cfg);
        if (ret != HI_SUCCESS) {
            HI_ERR_VENC("pfn_jpge_create_chn failed!\n");
            ret = HI_ERR_VENC_CREATE_ERR;
            venc_drv_efl_destroy_venc(enc_chn_para);
            return ret;
        }

        ret = venc_drv_efl_alloc_buf_2_jpge(enc_chn_para, attr->max_width, attr->max_height);
        if (ret != HI_SUCCESS) {
            HI_ERR_VENC("pfn_jpge_create_chn failed!\n");
            ret = HI_ERR_VENC_CREATE_ERR;
            jpeg_destroy_module(enc_cfg, *jpeg_chn);
            venc_drv_efl_destroy_venc(enc_chn_para);
            return ret;
        }
    }

    return HI_SUCCESS;
}
#endif

static hi_s32 config_rc_attr(hi_venc_chan_info *attr, hi_handle jpeg_chn, vedu_efl_enc_cfg *enc_cfg, hi_s32 i,
    vedu_efl_enc_para *enc_chn_para)
{
    hi_s32 ret;
    const hi_u32 no_use_buf_size = 64;
    vedu_efl_rc_attr rc_attr_cfg;

    rc_attr_cfg.bit_rate    = attr->config.target_bitrate;
    rc_attr_cfg.in_frm_rate  = attr->config.input_frame_rate;
    rc_attr_cfg.out_frm_rate = attr->config.target_frame_rate;
    rc_attr_cfg.max_qp      = attr->rc_param.max_qp;
    rc_attr_cfg.min_qp      = attr->rc_param.min_qp;

    enc_chn_para->cur_strm_buf_size = rc_attr_cfg.bit_rate / rc_attr_cfg.out_frm_rate * BUF_SIZE_MULT_CHECK_INIT;
    if (enc_chn_para->cur_strm_buf_size > (attr->strm_buf_size - no_use_buf_size) * BIT_NUM_BYTE) {
        enc_chn_para->cur_strm_buf_size = (attr->strm_buf_size - no_use_buf_size) * BIT_NUM_BYTE;
    }
    enc_chn_para->cur_strm_buf_size = D_VENC_ALIGN_UP(enc_chn_para->cur_strm_buf_size / BIT_NUM_BYTE, VEDU_MMZ_ALIGN);
    enc_chn_para->hal.split_size  = enc_cfg->split_size;

    ret = venc_drv_efl_rc_attr_init(enc_chn_para, &rc_attr_cfg);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("config venc err:%#x.\n", ret);
        ret = HI_ERR_VENC_INVALID_PARA;
        free_jpeg_buf(enc_cfg, enc_chn_para);
        jpeg_destroy_module(enc_cfg, jpeg_chn);
        venc_drv_efl_destroy_venc(enc_chn_para);
        return ret;
    }

    /* creat one proc file */
    ret = venc_drv_proc_add(enc_chn_para, i);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("venc_proc_add failed, ret = 0x%08x\n", ret);
        ret = HI_ERR_VENC_CREATE_ERR;
        free_jpeg_buf(enc_cfg, enc_chn_para);
        jpeg_destroy_module(enc_cfg, jpeg_chn);
        venc_drv_efl_destroy_venc(enc_chn_para);
        return ret;
    }

    return HI_SUCCESS;
}
hi_s32 venc_drv_create_chn(vedu_efl_enc_para** venc_chan, hi_venc_chan_info *attr,
    hi_ioctl_strm_buf_info *buf_info, hi_bool omx_chn, struct file *file)
{
    hi_s32 ret;
    hi_s32 i = 0;
    hi_u32 slc_size;
    vedu_efl_enc_cfg enc_cfg;
    hi_handle jpeg_chn = HI_INVALID_HANDLE;
    vedu_efl_enc_para *enc_chn_para = HI_NULL;

#ifdef VENC_SUPPORT_JPGE
    hi_jpge_cfg    jpeg_enc_cfg;
    VENC_CHECK_NEQ_RET(memset_s(&jpeg_enc_cfg, sizeof(hi_jpge_cfg), 0, sizeof(hi_jpge_cfg)), HI_SUCCESS, HI_FAILURE);
#endif

    D_VENC_CHECK_PTR(venc_chan);
    D_VENC_CHECK_PTR(attr);
    D_VENC_CHECK_PTR(buf_info);

    ret = get_enc_cfg(venc_chan, attr, &enc_cfg, omx_chn, &i);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    enc_chn_para = (vedu_efl_enc_para*)(*venc_chan);

    slc_size = strm_cal_slc_size(enc_chn_para->strm_mng, enc_cfg.protocol, enc_cfg.frame_height, attr->split_size);

    if ((enc_cfg.slc_split_en) && (slc_size == 0xffffffff)) {
        HI_ERR_VENC("get slice size fail!");
        venc_drv_efl_destroy_venc(enc_chn_para);
        return ret;
    }

    venc_drv_get_split_size(&enc_cfg, slc_size);

#ifdef VENC_SUPPORT_JPGE
    ret = jpeg_creat_chn(attr, &jpeg_chn, &enc_cfg, &jpeg_enc_cfg, enc_chn_para);
    if (ret != HI_SUCCESS) {
        return ret;
    }
#endif

    ret = config_rc_attr(attr, jpeg_chn, &enc_cfg, i, enc_chn_para);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    init_chan_para_part1(enc_chn_para, buf_info, i, file);
    init_chan_para_part2(attr, i, omx_chn, jpeg_chn, &enc_cfg);

    ret = get_name(enc_chn_para, i);

    if (ret != HI_SUCCESS) {
        return ret;
    }

    return HI_SUCCESS;
}

static void clear_chan_para(hi_u32 ve_chn, unsigned long flags)
{
    osal_spin_lock_irqsave(&g_send_frame_lock[ve_chn], &flags);
    g_venc_chn[ve_chn].source     = HI_INVALID_HANDLE;
    g_venc_chn[ve_chn].venc_handle = NULL;
    g_venc_chn[ve_chn].omx_chn     = HI_FALSE;
    g_venc_chn[ve_chn].enable     = HI_FALSE;
    g_venc_chn[ve_chn].chan_state = VEDU_STATE_BUTT;
    osal_spin_unlock_irqrestore(&g_send_frame_lock[ve_chn], &flags);
    HI_INFO_VENC("venc_destroy_chn %d OK\n", ve_chn);
}

hi_s32 venc_drv_destroy_chn(vedu_efl_enc_para *venc_chn)
{
    hi_s32 ret   = 0;
    hi_u32 ve_chn = 0;
    optm_venc_chn *_venc = HI_NULL;
    unsigned long flags;
    vedu_efl_enc_para *enc_chn_para = HI_NULL;
    D_VENC_GET_CHN(ve_chn, venc_chn);
    D_VENC_CHECK_CHN(ve_chn);

    osal_spin_lock_irqsave(&g_send_frame_lock[ve_chn], &flags);
    _venc = &g_venc_chn[ve_chn];
    enc_chn_para = g_venc_chn[ve_chn].venc_handle;
    g_venc_chn[ve_chn].chan_state = VEDU_STATE_DESTORYING;
    osal_spin_unlock_irqrestore(&g_send_frame_lock[ve_chn], &flags);

    if (_venc->enable) {
        HI_WARN_VENC("error:destroy channel when VENC is run.\n");
        venc_drv_efl_stop_venc(enc_chn_para);
        venc_drv_osal_give_event(&g_venc_wait_stream[ve_chn]);
    }

    switch (_venc->src_mod_id) {
        case HI_ID_DISP:
            enc_chn_para->src_info.detach_func(_venc->source, _venc->user_handle);
            break;
        default:
            break;
    }

    venc_drv_proc_del(enc_chn_para, ve_chn);

#ifdef VENC_SUPPORT_JPGE
    if ((g_venc_chn[ve_chn].jpge != HI_INVALID_HANDLE) && (g_jpge_func != HI_NULL)) {
        ret = g_jpge_func->drv_jpge_module_destory(g_venc_chn[ve_chn].jpge);
        if (ret != HI_SUCCESS) {
            HI_WARN_VENC("error:destroy JGPE ERR.\n");
        }

        venc_drv_efl_free_buf_2_jpge(enc_chn_para);
    }
#endif

    ret = venc_drv_efl_destroy_venc(enc_chn_para);
    if (ret != HI_SUCCESS) {
        osal_spin_lock_irqsave(&g_send_frame_lock[ve_chn], &flags);
        g_venc_chn[ve_chn].chan_state = VEDU_STATE_STOP;
        osal_spin_unlock_irqrestore(&g_send_frame_lock[ve_chn], &flags);
        return ret;
    }

    clear_chan_para(ve_chn, flags);

    return HI_SUCCESS;
}

static hi_s32 venc_drv_attach_input_check_para(vedu_efl_enc_para *venc_chn, hi_u32 *ve_chn,
    optm_venc_chn *venc_chan, hi_mod_id mod_id)
{
    D_VENC_GET_CHN(*ve_chn, venc_chn);
    D_VENC_CHECK_CHN(*ve_chn);

    venc_chan = &g_venc_chn[*ve_chn];

    if (venc_chan->enable == HI_TRUE) {
        HI_ERR_VENC("VENC has already start! please attach before start!\n");
        return HI_ERR_VENC_CHN_INVALID_STAT;
    }

    if (check_attach_module(mod_id) != HI_SUCCESS) {
        HI_ERR_VENC("mod_id not surpport now, mod_id=%x!\n", mod_id);
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (venc_chan->source != HI_INVALID_HANDLE) {
        HI_ERR_VENC("venc%d already attached to %#x!\n", *ve_chn, venc_chan->source);
        return HI_ERR_VENC_CHN_INVALID_STAT;
    }

    return HI_SUCCESS;
}

#ifdef VENC_SUPPORT_ATTACH_CAST
static hi_s32 venc_drv_attach_input_disp(optm_venc_chn *venc_chan, hi_handle src, vedu_efl_enc_para *enc_chn_para)
{
    hi_s32 ret;
    vedu_efl_src_info src_info = {0};
    disp_export_func* disp_func = HI_NULL; /* to be continued with vdp */

    VENC_CHECK_NEQ_RET(memset_s(&src_info, sizeof(vedu_efl_src_info), 0, sizeof(vedu_efl_src_info)),
        HI_SUCCESS, HI_FAILURE);
    ret = hi_drv_module_get_func(HI_ID_DISP, (hi_void**)&disp_func);
    if (ret) {
        HI_ERR_VENC("get_function for module(%d),failed!\n", HI_ID_DISP);
        return HI_FAILURE;
    }
    if (disp_func != HI_NULL) {
        src_info.handle     = src;
        src_info.get_image = venc_drv_efl_get_image;
        src_info.put_image = (ve_image_func)(disp_func->disp_cast_release_frame);
        src_info.detach_func = (ve_detach_func)(disp_func->disp_cast_detach);
        src_info.change_info = (ve_change_info_func)(disp_func->disp_cast_set_attr);
    }
    D_VENC_CHECK_PTR(src_info.change_info);
    ret = (src_info.change_info)(src,
                        venc_chan->chn_user_cfg.config.width,
                        venc_chan->chn_user_cfg.config.height,
                        HI_VENC_MAX_fps);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("src_info.change_info failed!\n");
        return HI_FAILURE;
    }

    ret = venc_drv_efl_attach_input(enc_chn_para, &src_info);

    return HI_SUCCESS;
}
#endif

#ifdef VENC_SUPPORT_ATTACH_VO
static hi_s32 venc_drv_attach_input_vo(optm_venc_chn *venc_chan, hi_handle src, vedu_efl_enc_para *enc_chn_para)
{
    hi_s32 ret;
    vedu_efl_src_info src_info = {0};
    win_export_func_s *vo_func = HI_NULL;

    VENC_CHECK_NEQ_RET(memset_s(&src_info, sizeof(vedu_efl_src_info), 0, sizeof(vedu_efl_src_info)),
        HI_SUCCESS, HI_FAILURE);
    ret = hi_drv_module_get_func(HI_ID_WIN, (hi_void **)&vo_func);
    if (ret) {
        HI_ERR_VENC("get_function for module(%d),failed!\n", HI_ID_WIN);
        return HI_FAILURE;
    }

    if (vo_func != HI_NULL) {
        src_info.handle     = src;
        src_info.get_image = venc_drv_efl_get_image;
        src_info.put_image = (ve_image_func)(vo_func->pfnwin_release_frame);
        src_info.detach_func = NULL;
        src_info.change_info = (ve_change_info_func)(vo_func->pfnwin_set_vir_attr);

    }
    D_VENC_CHECK_PTR(src_info.change_info);
    ret = (src_info.change_info)(src,
                        venc_chan->chn_user_cfg.config.width,
                        venc_chan->chn_user_cfg.config.height,
                        (HI_VENC_MAX_fps * 100)); /* 100:use for caculation */
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("src_info.change_info failed!\n");
        return HI_FAILURE;
    }
    ret = venc_drv_efl_attach_input(enc_chn_para, &src_info);

    return HI_SUCCESS;
}
#endif
hi_s32 venc_drv_attach_input(vedu_efl_enc_para *venc_chn, hi_handle src, hi_mod_id mod_id)
{
    hi_s32 ret;
    hi_u32 ve_chn = 0;
    optm_venc_chn *venc_chan = HI_NULL;
    vedu_efl_enc_para *enc_chn_para = venc_chn;

#ifdef VENC_SUPPORT_ATTACH_VI

#endif

    ret = venc_drv_attach_input_check_para(venc_chn, &ve_chn, venc_chan, mod_id);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (mod_id == HI_ID_DISP) {
#ifdef VENC_SUPPORT_ATTACH_CAST
        ret = venc_drv_attach_input_disp(venc_chan, src, enc_chn_para);
        if (ret != HI_SUCCESS) {
            return ret;
        }
#endif
    } else if (mod_id == HI_ID_WIN) {
#ifdef VENC_SUPPORT_ATTACH_VO
        ret = venc_drv_attach_input_vo(venc_chan, src, enc_chn_para);
        if (ret != HI_SUCCESS) {
            return ret;
        }
#endif
    }

    if (ret == HI_SUCCESS) {
        venc_chan->src_mod_id   = mod_id;
        venc_chan->source      = src;
        HI_INFO_VENC("VENC%d attch_input_ok, src_hdl:%#x.\n", ve_chn, venc_chan->source);
    }

    return ret;
}

#ifdef VENC_SUPPORT_ATTACH_VI
static hi_s32 set_src_info(drv_venc_source_handle *src, vedu_efl_src_info src_info, optm_venc_chn *venc_chan,
    vi_export_func *vi_func)
{
    hi_s32 ret = HI_SUCCESS;

    switch (src->source_mode) {
        case HI_ID_VI: {
            ret = hi_drv_module_get_func(HI_ID_VI, (hi_void**)&vi_func);
            if ((ret != HI_SUCCESS) || (vi_func == HI_NULL)) {
                HI_ERR_VENC("get_function for module(%d),failed!\n", HI_ID_VI);
                return HI_FAILURE;
            }

            src_info->handle     = src->source_handle;
            src_info->get_image = venc_drv_efl_get_image;
            src_info->put_image = (ve_image_func)(vi_func->pfn_venc_rls_frame);
            src_info->detach_func = NULL;
            src_info->change_info = (ve_change_info_func)(vi_func->pfn_disp_set_cast_attr);

            D_VENC_CHECK_PTR(src_info->change_info);

            ret = (src_info->change_info)(src->source_handle,
                                    venc_chan->chn_user_cfg.width,
                                    venc_chan->chn_user_cfg.height,
                                    HI_VENC_MAX_fps);
            if (ret != HI_SUCCESS) {
                HI_ERR_VENC("the first time set_info to VI failed!ret = %x\n", ret);
                venc_chan->source      = HI_INVALID_HANDLE;
                venc_chan->src_mod_id   = HI_ID_MAX;
                return HI_FAILURE;
            }
            break;
        }
        default:
            HI_ERR_VENC("VENC not support set src to mod(%d)\n", venc_chan->src_mod_id);
            return HI_ERR_VENC_INVALID_PARA;
    }

    return ret;
}
#endif

hi_s32 venc_drv_set_src_info(vedu_efl_enc_para *venc_chn, drv_venc_source_handle *src)
{
    hi_s32 ret;
#ifdef VENC_SUPPORT_ATTACH_VI
    hi_u32 ve_chn = 0;
    vedu_efl_src_info src_info;
    optm_venc_chn *venc_chan = HI_NULL;
    vi_export_func *vi_func = HI_NULL;
    vedu_efl_enc_para *enc_chn_para = HI_NULL;

    D_VENC_GET_CHN(ve_chn, venc_chn);
    D_VENC_CHECK_CHN(ve_chn);
    D_VENC_CHECK_PTR(src);

    venc_chan = &g_venc_chn[ve_chn];
    enc_chn_para = (vedu_efl_enc_para*)g_venc_chn[ve_chn].venc_handle;

    VENC_CHECK_NEQ_RET(memset_s(&src_info, sizeof(vedu_efl_src_info), 0, sizeof(vedu_efl_src_info)),
        HI_SUCCESS, HI_FAILURE);

    if (venc_chan->source == HI_INVALID_HANDLE) {
        HI_ERR_VENC("venc%d haven't attach any src!\n", ve_chn);
        return HI_ERR_VENC_CHN_INVALID_STAT;
    }

    ret = set_src_info(src, &src_info, venc_chan, vi_func);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    ret = venc_drv_efl_attach_input(enc_chn_para, &src_info);
#else
    HI_ERR_VENC("venc not support this API\n");
    ret = HI_ERR_VENC_NOT_SUPPORT;
#endif
    return ret;
}

hi_s32 venc_drv_detach_input(vedu_efl_enc_para* venc_chn, hi_handle src, hi_mod_id mod_id)
{
    hi_u32 ve_chn = 0;
    optm_venc_chn *venc_chan = HI_NULL;
    vedu_efl_src_info src_info = {0};
    vedu_efl_enc_para *enc_chn_para = HI_NULL;

    if (mod_id >= HI_ID_MAX) {
        return HI_ERR_VENC_CHN_NO_ATTACH;
    }

    D_VENC_GET_CHN(ve_chn, venc_chn);
    D_VENC_CHECK_CHN(ve_chn);

    venc_chan = &g_venc_chn[ve_chn];
    enc_chn_para = (vedu_efl_enc_para *)g_venc_chn[ve_chn].venc_handle;

    if (venc_chan->source == HI_INVALID_HANDLE) {
        HI_WARN_VENC("venc%d NOT attached.\n", ve_chn);
        return HI_ERR_VENC_CHN_NO_ATTACH;
    }

    if (venc_chan->source != src) {
        HI_ERR_VENC("venc%d NOT attached to %#x, but attached to %#x.\n", ve_chn, src, venc_chan->source);
        return HI_ERR_VENC_CHN_INVALID_STAT;
    }

    if (check_attach_module(mod_id) != HI_SUCCESS) {
        HI_ERR_VENC("mod_id not surpport now, mod_id=%x!\n", mod_id);
        return HI_ERR_VENC_INVALID_PARA;
    }

    /* VENC must be stop working */
    if (venc_chan->enable) {
        if (venc_drv_stop_receive_pic(enc_chn_para) != HI_SUCCESS) {
            HI_ERR_VENC("stop receive picture failure!\n");
        }
    }

    src_info.handle      = HI_INVALID_HANDLE;
    src_info.get_image  = HI_NULL;
    src_info.put_image  = HI_NULL;
    src_info.change_info = HI_NULL;
    src_info.detach_func = HI_NULL;

    venc_drv_efl_detach_input(enc_chn_para, &src_info);
    HI_INFO_VENC("VENC%d dettch_input_ok, src_hdl:%#x.\n", ve_chn, venc_chan->source);

    g_venc_chn[ve_chn].src_mod_id = HI_ID_MAX;
    g_venc_chn[ve_chn].source = HI_INVALID_HANDLE;

    return HI_SUCCESS;
}

static hi_s32 wait_for_stream(vedu_efl_enc_para *venc_chn, hi_u32 timeout_ms)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 ve_chn = 0;
    vedu_efl_enc_para *enc_chn_para = HI_NULL;

    D_VENC_GET_CHN(ve_chn, venc_chn);
    D_VENC_CHECK_CHN(ve_chn);
    enc_chn_para = (vedu_efl_enc_para*)g_venc_chn[ve_chn].venc_handle;

    if (strm_get_written_length(enc_chn_para->strm_mng) <= 0) {
        if (timeout_ms == 0) {
            return HI_ERR_VENC_BUF_EMPTY;
        } else {
            ret = venc_drv_osal_wait_event(&g_venc_wait_stream[ve_chn], timeout_ms);
            if (ret == HI_FAILURE) {
                return HI_ERR_VENC_BUF_EMPTY;
            }

            ret = strm_get_written_length(enc_chn_para->strm_mng);
            if (ret <= 0) {
                return HI_ERR_VENC_BUF_EMPTY;
            }
        }
    }

    return HI_SUCCESS;
}

static hi_s32 acquire_venc_stream(vedu_efl_enc_para *venc_chn, hi_u32 timeout_ms, hi_venc_stream *stream,
    venc_ioctl_buf_offset *buf_offset)
{
    hi_s32 ret;
    vedu_efl_nalu vedu_packet;
    vedu_efl_enc_para *enc_chn_para = venc_chn;
    handle_protocol h_protocol = {0};

    ret = wait_for_stream(enc_chn_para, timeout_ms);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = venc_drv_efl_acq_read_buffer(enc_chn_para, &vedu_packet);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    h_protocol.handle = enc_chn_para->strm_mng;
    h_protocol.protocol = enc_chn_para->protocol;

    strm_get_acquire_stream_info(&h_protocol, enc_chn_para->stream_mmz_buf.start_vir_addr, &vedu_packet, stream,
        buf_offset);

    return HI_SUCCESS;
}

#ifdef VENC_SUPPORT_JPGE
static hi_s32 get_jpge_stream(vedu_efl_nalu *vedu_packet, hi_venc_stream *stream, vedu_efl_enc_para *venc_chn,
    venc_ioctl_buf_offset *buf_offset)
{
    hi_s32 ret;
    vedu_efl_enc_para *enc_chn_para = venc_chn;
    strm_mng_stream_info stream_info;

    set_stream_info(enc_chn_para, &stream_info);
    ret = strm_get_jpge_stream(enc_chn_para->strm_mng, &stream_info, vedu_packet, stream, buf_offset);

    enc_chn_para->stat.stream_total_byte = stream_info.stream_total_byte;

    return ret;
}

static hi_s32 acquire_jpge_stream(vedu_efl_enc_para *venc_chn, hi_u32 timeout_ms, hi_venc_stream *stream,
    venc_ioctl_buf_offset *buf_offset)
{
    hi_s32 ret;
    vedu_efl_nalu vedu_packet;
    vedu_efl_enc_para *enc_chn_para = venc_chn;

    ret = wait_for_stream(enc_chn_para, timeout_ms);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (enc_chn_para->stat.get_stream_num_ok != enc_chn_para->stat.put_stream_num_ok) {
        return HI_ERR_VENC_CHN_INVALID_STAT;
    }

    ret = venc_drv_efl_acq_read_buffer(enc_chn_para, &vedu_packet);
    if (ret != HI_SUCCESS) {
        return HI_ERR_VENC_BUF_EMPTY;
    }

    ret = get_jpge_stream(&vedu_packet, stream, enc_chn_para, buf_offset);

    return ret;
}
#endif

hi_s32 venc_drv_quire_stream(vedu_efl_enc_para *venc_chn, hi_venc_stream *stream, hi_u32 timeout_ms,
                              venc_ioctl_buf_offset *buf_offset)
{
    hi_s32 ret   = -1;
    hi_s32 ve_chn = 0;
    unsigned long flags;
    vedu_efl_enc_para *enc_chn_para = HI_NULL;
    D_VENC_CHECK_PTR(stream);
    D_VENC_CHECK_PTR(buf_offset);

    D_VENC_GET_CHN(ve_chn, venc_chn);
    D_VENC_CHECK_CHN(ve_chn);

    osal_spin_lock_irqsave(&g_send_frame_lock[ve_chn], &flags);

    enc_chn_para = g_venc_chn[ve_chn].venc_handle;
    if (!enc_chn_para || (g_venc_chn[ve_chn].venc_handle == NULL)) {
        osal_spin_unlock_irqrestore(&g_send_frame_lock[ve_chn], &flags);
        return HI_ERR_VENC_CHN_NOT_EXIST;
    } else {
        enc_chn_para->stat.get_stream_num_try++;
    }
    osal_spin_unlock_irqrestore(&g_send_frame_lock[ve_chn], &flags);

    if (g_venc_chn[ve_chn].jpge != HI_INVALID_HANDLE) { /* JPEG */
#ifdef VENC_SUPPORT_JPGE
        ret = acquire_jpge_stream(enc_chn_para, timeout_ms, stream, buf_offset);
#endif
    } else {
        ret = acquire_venc_stream(enc_chn_para, timeout_ms, stream, buf_offset);
    }

    if (ret != HI_SUCCESS) {
        return ret;
    }

    enc_chn_para->stat.get_stream_num_ok++;

    HI_TRACE(HI_LOG_LEVEL_INFO, HI_ID_VSYNC, "PTS=%u.\n", stream->pts_ms);

    return ret;
}

#ifdef VENC_SUPPORT_JPGE
static hi_s32 release_jpge_stream(vedu_efl_enc_para *venc_chn, venc_ioctl_acquire_stream *acq_strm)
{
    vedu_efl_enc_para *enc_chn_para = venc_chn;

    if ((acq_strm->jpeg_phy_addr != enc_chn_para->jpg_mmz_buf.start_phy_addr)
        || (enc_chn_para->stat.get_stream_num_ok <= enc_chn_para->stat.put_stream_num_ok)) {
        HI_ERR_VENC("release stream failed, get_stream_num_ok=%d,put_stream_num_ok=%d,_stream->vir_addr = %x,\
            jpeg_out_buf_phy_addr= %x.\n", enc_chn_para->stat.get_stream_num_ok, enc_chn_para->stat.put_stream_num_ok,
            acq_strm->jpeg_phy_addr, enc_chn_para->jpg_mmz_buf.start_phy_addr);
        return HI_ERR_VENC_CHN_RELEASE_ERR;
    }

    return HI_SUCCESS;
}
#endif

static hi_void get_rlease_stream_info(vedu_efl_enc_para *enc_chn_para, venc_ioctl_acquire_stream *acq_strm,
    vedu_efl_nalu *vedu_packet)
{
    strm_mng_stream_info stream_info;

    set_stream_info(enc_chn_para, &stream_info);
    strm_get_release_stream_info(enc_chn_para->strm_mng, enc_chn_para->protocol, &stream_info, acq_strm, vedu_packet);
}

static hi_s32 release_venc_stream(vedu_efl_enc_para *venc_chn, venc_ioctl_acquire_stream *acq_strm)
{
    hi_s32 ret;
    vedu_efl_nalu vedu_packet;
    vedu_efl_enc_para  *enc_chn_para = venc_chn;

    VENC_CHECK_NEQ_RET(memset_s(&vedu_packet, sizeof(vedu_efl_nalu), 0, sizeof(vedu_efl_nalu)), HI_SUCCESS, HI_FAILURE);

    get_rlease_stream_info(enc_chn_para, acq_strm, &vedu_packet);
    ret = venc_drv_efl_rls_read_buffer(enc_chn_para, &vedu_packet);

    return ret;
}

hi_s32 venc_drv_release_stream(vedu_efl_enc_para *venc_chn, venc_ioctl_acquire_stream *acq_strm)
{
    hi_u32 ve_chn = 0;
    hi_s32 ret = 0;
    vedu_efl_enc_para *enc_chn_para = NULL;

    D_VENC_GET_CHN(ve_chn, venc_chn);
    D_VENC_CHECK_CHN(ve_chn);

    if (acq_strm == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return HI_FAILURE;
    }

    enc_chn_para = (vedu_efl_enc_para *)venc_chn;

    enc_chn_para->stat.put_stream_num_try++;

    if (enc_chn_para->protocol == VEDU_JPGE) {
#ifdef VENC_SUPPORT_JPGE
        ret = release_jpge_stream(enc_chn_para, acq_strm);
#endif
    } else {
        ret = release_venc_stream(enc_chn_para, acq_strm);
    }

    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("release stream failed, ret= %#x.\n", ret);

        return HI_ERR_VENC_CHN_RELEASE_ERR;
    }
    enc_chn_para->stat.put_stream_num_ok++;

    return HI_SUCCESS;
}

hi_s32 venc_drv_start_receive_pic(vedu_efl_enc_para *enc_handle)
{
    hi_s32 ret;
    hi_u32 ve_chn = 0;
    vedu_efl_enc_para *enc_chn_para = HI_NULL;
    D_VENC_GET_CHN(ve_chn, enc_handle);
    D_VENC_CHECK_CHN(ve_chn);

    enc_chn_para = (vedu_efl_enc_para *)enc_handle;

    if (g_venc_chn[ve_chn].enable == HI_TRUE) {
        return HI_SUCCESS;
    }

    ret = venc_drv_efl_start_venc(enc_handle);
    if (ret == HI_SUCCESS) {
        g_venc_chn[ve_chn].enable = HI_TRUE;
        g_venc_chn[ve_chn].chan_state = VEDU_STATE_START;
        if (g_venc_chn[ve_chn].src_mod_id >= HI_ID_MAX) { /* non attach mode */
            enc_chn_para->src_info.get_image = venc_drv_efl_get_image;
            enc_chn_para->src_info.get_image_omx = venc_drv_efl_get_image_omx;
            enc_chn_para->src_info.put_image = venc_drv_efl_put_image;
        }
        HI_INFO_VENC("start chn %d/%#x. OK\n", ve_chn, enc_handle);
        enc_chn_para->inter_frm_cnt = enc_chn_para->rc.gop - 1;
        enc_chn_para->rc.rc_start = 1;
    } else {
        ret = HI_ERR_VENC_INVALID_CHNID;
    }
    return ret;
}

hi_s32 venc_drv_stop_receive_pic(vedu_efl_enc_para *enc_handle)
{
    hi_s32 ret;
    hi_u32 ve_chn = 0;
    vedu_efl_enc_para *enc_chn_para = HI_NULL;

    D_VENC_GET_CHN(ve_chn, enc_handle);
    D_VENC_CHECK_CHN(ve_chn);
    enc_chn_para = enc_handle;

    if (g_venc_chn[ve_chn].enable == HI_FALSE) {
        return HI_SUCCESS;
    }

    ret = venc_drv_efl_stop_venc(enc_chn_para);
    if (ret == HI_SUCCESS) {
        g_venc_chn[ve_chn].enable = HI_FALSE;
        g_venc_chn[ve_chn].chan_state = VEDU_STATE_STOP;
        HI_INFO_VENC("stop chn %d/%#x. OK\n", ve_chn, enc_chn_para);
    }

    return ret;
}

static hi_s32 venc_drv_set_config_rc_and_frm_rate(hi_venc_chan_config *config, vedu_efl_rc_attr *rc_attr_cfg,
    vedu_efl_enc_para *enc_handle, optm_venc_chn *venc_chan)
{
    hi_s32 ret = HI_FAILURE;
    hi_u8 rc_flag = HI_FALSE;
    hi_venc_chan_info *cur_attr = &venc_chan->chn_user_cfg;

    if ((cur_attr->standard == HI_VENC_STD_H264) || (cur_attr->standard == HI_VENC_STD_H265)) {
        rc_flag |= (hi_u8)(cur_attr->config.target_bitrate != config->target_bitrate);
        rc_flag |= (hi_u8)(cur_attr->config.input_frame_rate  != config->input_frame_rate);
        rc_flag |= (hi_u8)(cur_attr->config.target_frame_rate != config->target_frame_rate);
        rc_flag |= (hi_u8)(cur_attr->config.gop != config->gop);

        if (rc_flag == HI_TRUE) {
            rc_attr_cfg->bit_rate = config->target_bitrate;
            rc_attr_cfg->in_frm_rate = config->input_frame_rate;
            rc_attr_cfg->out_frm_rate = config->target_frame_rate;
            rc_attr_cfg->gop = config->gop;

            ret = venc_drv_efl_rc_set_attr(venc_chan->venc_handle, rc_attr_cfg);

            if (ret != HI_SUCCESS) {
                HI_ERR_VENC("config venc rate control attribute err:%#x.\n", ret);
                return HI_FAILURE;
            }
        }
    }  else {  /* jpeg */
#ifdef VENC_SUPPORT_JPGE
        vedu_efl_enc_para* enc_chn_para = enc_handle;

        if ((config->input_frame_rate > HI_VENC_MAX_fps) ||
            (config->input_frame_rate < config->target_frame_rate)) {
            HI_ERR_VENC("input_frame_rate invalid\n");
            return HI_ERR_VENC_INVALID_PARA;
        }

       /* initialize  parameter */
        enc_chn_para->rc.vo_frm_rate = config->target_frame_rate;
        enc_chn_para->rc.vi_frm_rate = config->input_frame_rate;
#endif
    }

    return HI_SUCCESS;
}


hi_s32 set_attr_gop_mode_dualp(vedu_efl_enc_para *enc_para, hi_u32 gop_type)
{
    hi_s32 extra_rcn_num, ret;
    hi_u32 alloc_buf_size, extra_rcn_size, extra_mvme_size, i, old_rcn_num;
    venc_buffer *extra_rcn_buf = &enc_para->extra_rcn_buf;
    pic_pool_buffer *pic = enc_para->pic_pool;
    pic_info_pool_buffer *pic_info = enc_para->pic_info_pool;
    hi_u32 pic_info_addr;

    if (gop_type == HI_VENC_GOP_MODE_DUALP) {
        extra_rcn_num = 1;
        extra_rcn_size = (enc_para->luma_size * 3 / 2 + enc_para->rcn_block_size) * extra_rcn_num; /* 2,3:use for cal */
        extra_mvme_size = (enc_para->tmv_size + enc_para->pme_size + enc_para->pme_info_size) * extra_rcn_num;
        alloc_buf_size = extra_rcn_size + extra_mvme_size;

        ret = hi_drv_venc_alloc_and_map("venc_extra_rcn_buf", 0, alloc_buf_size, VEDU_MMZ_ALIGN, extra_rcn_buf);
        if (ret != HI_SUCCESS) {
            HI_ERR_VENC("hi_drv_venc_alloc_and_map failed\n");
            return HI_FAILURE;
        }

        if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
            old_rcn_num = 1;
        } else {
            old_rcn_num = 2; /* 2:set start position */
        }

        for (i = old_rcn_num; i < enc_para->pic_num; i++) {
            pic[i].pic_addr = (hi_u32)(hi_size_t)extra_rcn_buf->start_vir_addr;
            pic[i].used = HI_FALSE;
            pic[i].pic_id = i;
        }

        pic_info_addr = (hi_u32)(hi_size_t)extra_rcn_buf->start_vir_addr +
            enc_para->luma_size * 3 / 2 + enc_para->rcn_block_size; /* 2,3:used for calculation. */

        for (i = 2; i < enc_para->pic_info_num; i++) { /* 2: start position */
            pic_info[i].pic_info_addr = pic_info_addr + enc_para->tmv_size +
                enc_para->pme_info_size + enc_para->pme_size;
            pic_info[i].used = HI_FALSE;
            pic_info[i].pic_info_id = i;
        }

        enc_para->extra_rcn_flag = HI_TRUE;
    }

    return HI_SUCCESS;
}
hi_s32 venc_drv_set_attr_gop_change(vedu_efl_enc_para *enc_handle, hi_u32 gop_type)
{
    hi_u32 ve_chn = 0;
    vedu_efl_enc_para *enc_para = (vedu_efl_enc_para *)enc_handle;
    hi_s32 ret;
    venc_buffer *extra_rcn_buf = &enc_para->extra_rcn_buf;

    D_VENC_GET_CHN(ve_chn, enc_handle);
    D_VENC_CHECK_CHN(ve_chn);

    if (gop_type == enc_para->rc.gop_type ||
        gop_type != HI_VENC_GOP_MODE_NORMALP ||
        gop_type != HI_VENC_GOP_MODE_DUALP) {
        HI_ERR_VENC("invalid gop mode\n");
        return HI_FAILURE;
    }

    ret = set_attr_gop_mode_dualp(enc_para, gop_type);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("request I frame fail\n");
        if (enc_para->extra_rcn_flag == HI_TRUE) {
            hi_drv_venc_unmap_and_release(extra_rcn_buf);
        }
    }

    ret = venc_drv_request_i_frame(enc_para);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("request I frame fail\n");
        if (enc_para->extra_rcn_flag == HI_TRUE) {
            hi_drv_venc_unmap_and_release(extra_rcn_buf);
        }
    }

    enc_para->rc.gop_type = gop_type;

    return ret;
}

static hi_s32 venc_drv_set_config_resolution(vedu_efl_enc_para *enc_handle, optm_venc_chn *venc_optm,
    hi_venc_chan_config *config)
{
    vedu_efl_enc_para *enc_chn_para = enc_handle;
    hi_u32 frame_rate = HI_VENC_MAX_fps;

     /* should be change */
    if ((venc_optm->chn_user_cfg.config.height  != config->height) ||
        (venc_optm->chn_user_cfg.config.width != config->width)) {
        if (venc_optm->src_mod_id != HI_ID_MAX) {    /* attach mode */
#ifdef VENC_SUPPORT_ATTACH_VO
            if (venc_optm->src_mod_id == HI_ID_WIN) {
                frame_rate = HI_VENC_MAX_fps * 1000;     /* 1000: VO frame rate times */
            }
#endif
            if ((enc_chn_para->src_info.change_info)(enc_chn_para->src_info.handle, config->width, config->height,
                                                     frame_rate) != HI_SUCCESS) {
                HI_ERR_VENC("src handle(%x) change info failed!.\n", enc_chn_para->src_info.handle);
                return HI_FAILURE;
            }

            if (venc_optm->enable == HI_FALSE) {  /* if not start encode release all frame */
                venc_drv_efl_rls_all_frame(enc_chn_para);
            }
        }
    }

    return HI_SUCCESS;
}

void update_user_config(hi_venc_chan_config *config, hi_venc_chan_info *cur_attr, vedu_efl_enc_para *enc_chn_para)
{
    if ((cur_attr->standard == HI_VENC_STD_H264) || (cur_attr->standard == HI_VENC_STD_H265)) {
        cur_attr->config.gop = config->gop;
        cur_attr->config.priority = config->priority;
        cur_attr->config.target_bitrate = config->target_bitrate;
        cur_attr->config.input_frame_rate = config->input_frame_rate;
        cur_attr->config.target_frame_rate = config->target_frame_rate;
        cur_attr->config.height = config->height;
        cur_attr->config.width = config->width;
        cur_attr->config.sp_interval = config->sp_interval;
        cur_attr->config.frm_rate_type = config->frm_rate_type;
        cur_attr->config.quick_encode = config->quick_encode;

        if (cur_attr->gop_mode == HI_VENC_GOP_MODE_BIPREDB) {
            /* B frame */
        }
    } else {   /* JPEG */
#ifdef VENC_SUPPORT_JPGE
        cur_attr->config.priority = config->priority;
        cur_attr->config.input_frame_rate = config->input_frame_rate;
        cur_attr->config.target_frame_rate = config->target_frame_rate;
        cur_attr->config.quick_encode = config->quick_encode;
        cur_attr->config.height = config->height;
        cur_attr->config.width = config->width;
        cur_attr->config.qfactor = config->qfactor;

        enc_chn_para->q_level = config->qfactor;
#endif
    }
    enc_chn_para->quick_encode = config->quick_encode;
}

hi_s32 venc_drv_set_attr(vedu_efl_enc_para *enc_handle, hi_venc_chan_info *attr, venc_ioctl_chan_info *ve_info)
{
    HI_ERR_VENC("Set attr isn't be supported.\n");
    return HI_FAILURE;
}


hi_s32 venc_drv_get_attr(vedu_efl_enc_para *enc_handle, hi_venc_chan_info *attr)
{
    hi_u32 ve_chn = 0;
    optm_venc_chn *venc_chan = NULL;

    D_VENC_GET_CHN(ve_chn, enc_handle);
    D_VENC_CHECK_CHN(ve_chn);
    D_VENC_CHECK_PTR(attr);
    venc_chan = &g_venc_chn[ve_chn];

    *attr = venc_chan->chn_user_cfg;

    return HI_SUCCESS;
}

hi_s32 venc_drv_set_rate_control_type(vedu_efl_enc_para *enc_handle, hi_venc_control_rate_type rc_type)
{
    hi_u32 ve_chn = 0;
    vedu_efl_rc_attr rc_attr_cfg;
    vedu_efl_enc_para *enc_chn_para = (vedu_efl_enc_para *)enc_handle;

    D_VENC_GET_CHN(ve_chn, enc_handle);
    D_VENC_CHECK_CHN(ve_chn);

    if ((rc_type != HI_VENC_CONTROL_RATE_VARIABLE) && (rc_type != HI_VENC_CONTROL_RATE_CONSTANT)) {
        HI_ERR_VENC("chan(%p) set rate_control type(%d) not support!\n", enc_handle, rc_type);
        return HI_ERR_VENC_INVALID_PARA;
    }

    venc_drv_efl_request_i_frame(enc_chn_para);

    VENC_CHECK_NEQ_RET(memset_s(&rc_attr_cfg, sizeof(vedu_efl_rc_attr), 0, sizeof(vedu_efl_rc_attr)),
        HI_SUCCESS, HI_FAILURE);
    rc_attr_cfg.bit_rate = enc_chn_para->rc.bit_rate;
    rc_attr_cfg.out_frm_rate = enc_chn_para->rc.vo_frm_rate;
    rc_attr_cfg.in_frm_rate = enc_chn_para->rc.vi_frm_rate;
    rc_attr_cfg.max_qp  = enc_chn_para->rc.max_qp;
    rc_attr_cfg.min_qp  = enc_chn_para->rc.min_qp;

    /* set rate_control mode */
    enc_chn_para->rc.is_avbr = (rc_type != HI_VENC_CONTROL_RATE_CONSTANT) ? 1 : 0;
    enc_chn_para->rc.max_time_of_p = (enc_chn_para->rc.is_avbr) ? 200 : 6; /* if avbr, take 200, or 6 */

    if (venc_drv_efl_rc_attr_init(enc_chn_para, &rc_attr_cfg) != HI_SUCCESS) {
        HI_ERR_VENC("config venc rate control attribute err!.\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 venc_drv_get_rate_control_type(vedu_efl_enc_para *enc_handle, hi_venc_control_rate_type *rc_type)
{
    hi_u32 ve_chn = 0;
    vedu_efl_enc_para *enc_chn_para = (vedu_efl_enc_para *)enc_handle;

    D_VENC_GET_CHN(ve_chn, enc_handle);
    D_VENC_CHECK_CHN(ve_chn);
    D_VENC_CHECK_PTR(rc_type);

    *rc_type = (enc_chn_para->rc.is_avbr == 1) ? HI_VENC_CONTROL_RATE_VARIABLE : HI_VENC_CONTROL_RATE_CONSTANT;

    return HI_SUCCESS;
}

hi_s32 venc_drv_set_in_frm_rate_type(vedu_efl_enc_para *enc_handle, hi_venc_frm_rate_type in_frame_rate_type)
{
    hi_u32 ve_chn = 0;
    vedu_efl_enc_para *enc_chn_para = (vedu_efl_enc_para *)enc_handle;

    D_VENC_GET_CHN(ve_chn, enc_handle);
    D_VENC_CHECK_CHN(ve_chn);

    if (in_frame_rate_type >= HI_VENC_FRMRATE_BUTT) {
        HI_ERR_VENC("chan(%p) set rate_control type(%d) not support!\n", enc_handle, in_frame_rate_type);
        return HI_ERR_VENC_INVALID_PARA;
    }

    enc_chn_para->input_frm_rate_type_config = in_frame_rate_type;

    return HI_SUCCESS;
}

hi_s32 venc_drv_get_in_frm_rate_type(vedu_efl_enc_para *enc_handle, hi_venc_frm_rate_type *in_frame_rate_type)
{
    hi_u32 ve_chn = 0;
    vedu_efl_enc_para *enc_chn_para = (vedu_efl_enc_para *)enc_handle;

    D_VENC_GET_CHN(ve_chn, enc_handle);
    D_VENC_CHECK_CHN(ve_chn);
    D_VENC_CHECK_PTR(in_frame_rate_type);

    if (enc_chn_para->input_frm_rate_type_config != HI_VENC_FRMRATE_BUTT) {
        *in_frame_rate_type = enc_chn_para->input_frm_rate_type_config;
    } else if (enc_chn_para->input_frm_rate_type_default != HI_VENC_FRMRATE_BUTT) {
        *in_frame_rate_type = enc_chn_para->input_frm_rate_type_default;
    } else {
        /* both input_frm_rate_type_default and input_frm_rate_type_config is invalid, return use_mode */
        *in_frame_rate_type = HI_VENC_FRMRATE_USER;
    }

    return HI_SUCCESS;
}

hi_s32 venc_drv_request_i_frame(vedu_efl_enc_para *enc_handle)
{
    hi_s32 ve_chn = 0;
    optm_venc_chn *venc_chan = HI_NULL;

    D_VENC_GET_CHN(ve_chn, enc_handle);
    D_VENC_CHECK_CHN(ve_chn);
    venc_chan = &g_venc_chn[ve_chn];

    venc_drv_efl_request_i_frame(venc_chan->venc_handle);

    return HI_SUCCESS;
}

hi_s32 venc_drv_queue_frame(vedu_efl_enc_para *venc_chn, hi_drv_video_frame* frame_info)
{
    hi_s32 ret;
    hi_u32 ve_chn = 0;
    vedu_efl_enc_para *enc_chn_para = HI_NULL;

    D_VENC_GET_CHN(ve_chn, venc_chn);
    D_VENC_CHECK_CHN(ve_chn);
    D_VENC_CHECK_PTR(frame_info);

    enc_chn_para = venc_chn;
    if (g_venc_chn[ve_chn].enable != HI_TRUE) {
        HI_ERR_VENC("please start the VENC before queue_frame!\n");
        return HI_ERR_VENC_CHN_INVALID_STAT;
    }

    if (enc_chn_para->never_enc == HI_TRUE) {
        if (g_venc_chn[ve_chn].src_mod_id == HI_ID_MAX) {  /* 如果未绑定则设置 */
            enc_chn_para->src_info.get_image = venc_drv_efl_get_image;
            enc_chn_para->src_info.put_image = venc_drv_efl_put_image;
        } else {
            HI_ERR_VENC("the venc had already attach another sourse!queue_frame is  invalid!! \n");
            return HI_ERR_VENC_CHN_INVALID_STAT;
        }
    }

    if (enc_chn_para->never_enc == HI_TRUE) {
        enc_chn_para->never_enc = HI_FALSE;
    }

    frame_info->tunnel_phy_addr = 0;
    ret = venc_drv_efl_queue_frame(enc_chn_para, frame_info);

    if (ret != HI_SUCCESS)  {
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 venc_drv_dequeue_frame(vedu_efl_enc_para *venc_chn, hi_drv_video_frame *frame_info)
{
    hi_s32 ret;
    hi_s32 ve_chn = 0;
    vedu_efl_enc_para *enc_chn_para = HI_NULL;
    D_VENC_GET_CHN(ve_chn, venc_chn);
    D_VENC_CHECK_CHN(ve_chn);
    D_VENC_CHECK_PTR(frame_info);

    VENC_CHECK_NEQ_RET(memset_s(frame_info, sizeof(hi_drv_video_frame), 0, sizeof(hi_drv_video_frame)),
        HI_SUCCESS, HI_FAILURE);
    enc_chn_para = venc_chn;
    if (g_venc_chn[ve_chn].src_mod_id < HI_ID_MAX)  {
        HI_ERR_VENC("the venc had already attach another sourse!dequeue_frame is  invalid!! \n");
        return HI_ERR_VENC_CHN_INVALID_STAT;
    }

    ret = venc_drv_efl_dequeue_frame(enc_chn_para, frame_info);

    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return ret;
}

#ifdef VENC_SUPPORT_H265
static hi_s32 get_cap_info_h265(hi_venc_cap_info *cap_info)
{
    D_VENC_CHECK_PTR(cap_info);

    cap_info->standard = HI_VENC_STD_H265;
    cap_info->max_chn_num = HI_VENC_MAX_CHN;

    cap_info->max_resolution.height  = HI_VENC_MAX_HEIGTH;
    cap_info->max_resolution.width   = HI_VENC_MAX_WIDTH;
    cap_info->max_resolution.max_bit_rate = HI_VENC_MAX_bps;
    cap_info->max_resolution.min_bit_rate = HI_VENC_MIN_bps;
    cap_info->max_resolution.max_frame_rate = HI_VENC_MAX_fps;
    cap_info->max_resolution.min_buf_size = HI_VENC_MIN_BUF_SIZE;

    cap_info->mix_resolution.height  = HI_VENC_MIN_HEIGTH;
    cap_info->mix_resolution.width   = HI_VENC_MIN_WIDTH;
    cap_info->mix_resolution.max_bit_rate = HI_VENC_MAX_bps;
    cap_info->mix_resolution.min_bit_rate = HI_VENC_MIN_bps;
    cap_info->mix_resolution.max_frame_rate = HI_VENC_MAX_fps;
    cap_info->mix_resolution.min_buf_size = HI_VENC_MIN_BUF_SIZE;

    return HI_SUCCESS;
}
#endif

#ifdef VENC_SUPPORT_H264
static hi_s32 get_cap_info_h264(hi_venc_cap_info *cap_info)
{
    D_VENC_CHECK_PTR(cap_info);

    cap_info->standard  = HI_VENC_STD_H264;
    cap_info->max_chn_num = HI_VENC_MAX_CHN;

    cap_info->max_resolution.height  = HI_VENC_MAX_HEIGTH;
    cap_info->max_resolution.width   = HI_VENC_MAX_WIDTH;
    cap_info->max_resolution.max_bit_rate = HI_VENC_MAX_bps;
    cap_info->max_resolution.min_bit_rate = HI_VENC_MIN_bps;
    cap_info->max_resolution.max_frame_rate = HI_VENC_MAX_fps;
    cap_info->max_resolution.min_buf_size = HI_VENC_MIN_BUF_SIZE;

    cap_info->mix_resolution.height  = HI_VENC_MIN_HEIGTH;
    cap_info->mix_resolution.width   = HI_VENC_MIN_WIDTH;
    cap_info->mix_resolution.max_bit_rate = HI_VENC_MAX_bps;
    cap_info->mix_resolution.min_bit_rate = HI_VENC_MIN_bps;
    cap_info->mix_resolution.max_frame_rate = HI_VENC_MAX_fps;
    cap_info->mix_resolution.min_buf_size = HI_VENC_MIN_BUF_SIZE;

    return HI_SUCCESS;
}
#endif

#ifdef VENC_SUPPORT_JPGE
static hi_s32 get_cap_info_jpge(hi_venc_cap_info *cap_info)
{
    D_VENC_CHECK_PTR(cap_info);

    cap_info->standard  = HI_VENC_STD_JPEG;
    cap_info->max_chn_num = HI_JPGE_MAX_CHN;

    cap_info->max_resolution.height  = HI_JPEG_MAX_HEIGTH;
    cap_info->max_resolution.width   = HI_JPEG_MAX_WIDTH;
    cap_info->max_resolution.max_bit_rate = HI_VENC_MAX_bps;
    cap_info->max_resolution.min_bit_rate = HI_VENC_MIN_bps;
    cap_info->max_resolution.max_frame_rate = HI_VENC_MAX_fps;
    cap_info->max_resolution.min_buf_size = HI_VENC_MIN_BUF_SIZE;

    cap_info->mix_resolution.height  = HI_JPEG_MIN_HEIGTH;
    cap_info->mix_resolution.width   = HI_JPEG_MIN_WIDTH;
    cap_info->mix_resolution.max_bit_rate = HI_VENC_MAX_bps;
    cap_info->mix_resolution.min_bit_rate = HI_VENC_MIN_bps;
    cap_info->mix_resolution.max_frame_rate = HI_VENC_MAX_fps;
    cap_info->mix_resolution.min_buf_size = HI_VENC_MIN_BUF_SIZE;

    return HI_SUCCESS;
}
#endif

hi_s32 venc_drv_get_capability(hi_venc_cap *capability)
{
    hi_u32 type_num = 0;

    D_VENC_CHECK_PTR(capability);
    VENC_CHECK_NEQ_RET(memset_s(capability, sizeof(hi_venc_cap), 0, sizeof(hi_venc_cap)), HI_SUCCESS, HI_FAILURE);

#ifdef VENC_SUPPORT_H265
    if (get_cap_info_h265(&capability->cap_info[type_num]) != HI_SUCCESS) {
        return HI_FAILURE;
    }
    type_num++;
#endif

#ifdef VENC_SUPPORT_H264
    if (get_cap_info_h264(&capability->cap_info[type_num]) != HI_SUCCESS) {
        return HI_FAILURE;
    }
    type_num++;
#endif

#ifdef VENC_SUPPORT_JPGE
    if (get_cap_info_jpge(&capability->cap_info[type_num]) != HI_SUCCESS) {
        return HI_FAILURE;
    }
    type_num++;
#endif
    capability->support_type_num = type_num;

    if (type_num == 0) {
        HI_ERR_VENC("get encode type of capability failure!");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 venc_drv_set_config(vedu_efl_enc_para *enc_para, hi_venc_chan_config *config)
{
    hi_s32 ret;
    hi_u32 ve_chn = 0;

    optm_venc_chn *venc_optm = NULL;
    hi_venc_chan_info *cur_attr = NULL;
    vedu_efl_rc_attr rc_attr_cfg;

    D_VENC_GET_CHN(ve_chn, enc_para);
    D_VENC_CHECK_CHN(ve_chn);
    D_VENC_CHECK_PTR(config);
    venc_optm = &g_venc_chn[ve_chn];
    cur_attr = &venc_optm->chn_user_cfg;

    ret = check_chan_config(cur_attr, config, venc_optm->omx_chn);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* this case to change the resolution */
    ret = venc_drv_set_config_resolution(enc_para, venc_optm, config);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* this case to change the RC & frm_rate control parameter */
    ret = venc_drv_set_config_rc_and_frm_rate(config, &rc_attr_cfg, enc_para, venc_optm);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* this case to change the priority */
    if (config->priority != cur_attr->config.priority) {
        hi_u32 priority_id;
        D_VENC_GET_PRIORITY_ID(ve_chn, priority_id);
        D_VENC_CHECK_CHN(priority_id);
        g_priority_tab[1][priority_id] = config->priority;
        venc_drv_efl_sort_priority();
    }

    /* update the new user attr */
    update_user_config(config, cur_attr, enc_para);

    return HI_SUCCESS;
}

hi_s32 venc_drv_get_config(vedu_efl_enc_para *enc_handle, hi_venc_chan_config *config)
{
    hi_u32 ve_chn = 0;
    optm_venc_chn *venc_chan = NULL;

    D_VENC_GET_CHN(ve_chn, enc_handle);
    D_VENC_CHECK_CHN(ve_chn);
    D_VENC_CHECK_PTR(config);
    venc_chan = &g_venc_chn[ve_chn];

    *config = venc_chan->chn_user_cfg.config;

    return HI_SUCCESS;
}

/******************* new function interface for open_tv 5 ***********************/
extern osal_semaphore g_venc_drv_mutex;
extern osal_atomic g_venc_count;
extern osal_timer g_venc_timer;
extern hi_u32 g_venc_open_flag;
extern hi_void venc_timer_func(hi_length_t value);

hi_s32 venc_drv_init(hi_void)
{
    hi_s32 ret;

    ret = osal_exportfunc_register(HI_ID_VENC, "HI_VENC", (hi_void *)&g_venc_export_funcs);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("hi_drv_module_register failed, mode ID = 0x%08X\n", HI_ID_VENC);
        return HI_FAILURE;
    }

    init_venc_interface();

    return HI_SUCCESS;
}

hi_void venc_drv_exit(hi_void)
{
    osal_exportfunc_unregister(HI_ID_VENC);
    return;
}

#ifndef VENC_DPT_ONLY
static hi_s32 venc_drv_release_stream_intar(vedu_efl_enc_para *venc_chn, hi_venc_stream *stream)
{
    hi_u32 ve_chn = 0;
    hi_s32 ret ;
    vedu_efl_nalu vedu_packet;
    vedu_efl_enc_para *enc_chn_para = HI_NULL;

    D_VENC_CHECK_PTR(stream);
    D_VENC_GET_CHN(ve_chn, venc_chn);
    D_VENC_CHECK_CHN(ve_chn);

    VENC_CHECK_NEQ_RET(memset_s(&vedu_packet, sizeof(vedu_packet), 0, sizeof(vedu_packet)), HI_SUCCESS, HI_FAILURE);
    enc_chn_para = (vedu_efl_enc_para*)venc_chn;

    if (g_venc_chn[ve_chn].jpge != HI_INVALID_HANDLE) {
        enc_chn_para->stat.put_stream_num_try++;

        if ((stream->virt_addr != (hi_size_t)(uintptr_t)enc_chn_para->jpg_mmz_buf.start_vir_addr)
            || (enc_chn_para->stat.get_stream_num_ok <= enc_chn_para->stat.put_stream_num_ok)) {
            HI_ERR_VENC("release stream failed, get_stream_num_ok=%d,put_stream_num_ok=%d,\
                stream->pu8_addr = %x, jpeg_out_buf_phy_addr= %x.\n", enc_chn_para->stat.get_stream_num_ok,
                enc_chn_para->stat.put_stream_num_ok, stream->virt_addr, enc_chn_para->jpg_mmz_buf.start_phy_addr);

            return HI_ERR_VENC_CHN_RELEASE_ERR;
        }
        enc_chn_para->stat.put_stream_num_ok++;

        return HI_SUCCESS;
    }


    if (strm_get_release_stream_info_intar(enc_chn_para->strm_mng, stream, &vedu_packet) != HI_SUCCESS) {
        return HI_ERR_VENC_CHN_RELEASE_ERR;
    }

    enc_chn_para->stat.put_stream_num_try++;

    ret = venc_drv_efl_rls_read_buffer(enc_chn_para, &vedu_packet);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("release stream failed, ret= %#x.\n", ret);
        return HI_ERR_VENC_CHN_RELEASE_ERR;
    }

    enc_chn_para->stat.put_stream_num_ok++;

    return HI_SUCCESS;
}

hi_s32 hi_drv_venc_get_user_hnd_by_ker_hnd(vedu_efl_enc_para *venc_handle, hi_handle *venc_usr_handle)
{
    hi_u32 i = 0;

    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        if (g_venc_chn[i].venc_handle == venc_handle)
        { break; }
    }

    if (i >= HI_VENC_MAX_CHN) {
        HI_ERR_VENC("unknown venchandle!\n");
        return HI_FAILURE;
    }

    *venc_usr_handle = GET_VENC_CHHANDLE(i);
    return HI_SUCCESS;
}


hi_s32 hi_drv_venc_init(hi_void)
{
    hi_s32 ret;

    /* init the VENC device */
    ret = venc_drv_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("init VENC drv fail!\n");
        return HI_FAILURE;
    }

    /* just init the wait_event */
    /* venc_drv_board_init();  ==> after create the channel */
    venc_drv_init_event();

    /* creat thread to manage channel */
    ret = venc_drv_efl_open_vedu();
    if (ret != HI_SUCCESS) {
        HI_FATAL_VENC("vedu_efl_open_vedu failed, ret=%d\n", ret);
        return HI_FAILURE;
    }
    return ret;
}

hi_s32 hi_drv_venc_de_init(hi_void)
{

    hi_s32 ret = 0;
    hi_u32 i = 0;

    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        if (g_venc_chn[i].venc_handle == NULL) {
            break;
        }
    }
    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        if ((g_venc_chn[i].venc_handle != NULL)) {
            HI_INFO_VENC("try venc_destroy_chn %d/%#p.\n", i, g_venc_chn[i].venc_handle);
            ret = venc_drv_destroy_chn(g_venc_chn[i].venc_handle);
            if (ret != HI_SUCCESS) {
                HI_WARN_VENC("force destroy_chn %d failed, ret=%#x.\n", i, ret);
            }
            g_venc_chn[i].which_file = HI_NULL;
        }
    }

    venc_drv_efl_close_vedu();

    /* close the venc lock */
    venc_drv_board_deinit();

    venc_drv_exit();

    return HI_SUCCESS;
}

hi_s32 hi_drv_venc_get_default_attr(hi_venc_chan_info *attr)
{
    if (attr == NULL) {
        return HI_ERR_VENC_NULL_PTR;
    }

    attr->standard = HI_VENC_STD_H264;
    attr->profile = HI_VENC_H264_PROFILE_HIGH;
    attr->slc_split_enable = HI_FALSE;
    attr->strm_buf_size = 1024 * 1024; /* set default strm buf size 1014*1024 */
    attr->max_width = 1280;             /* set default width 1280 */
    attr->max_height = 720;             /* set default height 720 */
    attr->gop_mode = HI_VENC_GOP_MODE_NORMALP;

    attr->config.width = 1280;             /* set default width 1280 */
    attr->config.height = 720;             /* set default height 720 */
    attr->config.target_bitrate = 4 * 1024 * 1024; /* set default target bit rate 4*1014*1024 */
    attr->config.input_frame_rate = 25;    /* set default input frame rate 25 */
    attr->config.target_frame_rate = 25;   /* set default target frame rate 25 */
    attr->config.gop = 100;                /* set default gop 100 */
    attr->config.quick_encode  = HI_FALSE;
    attr->config.priority = 0;
    attr->config.qfactor = 0;

    return HI_SUCCESS;
}

hi_s32 hi_drv_venc_create(hi_handle *venc_chn, hi_venc_chan_info *attr, hi_bool omx_chn, struct file *file)
{
    hi_s32 ret;
    hi_u32 index = 0;
    vedu_efl_enc_para *kernel_venc_chn = NULL;
    hi_venc_chan_info venc_cfg ;
    hi_ioctl_strm_buf_info buf_info;   /* no use output in the mode */

    if (venc_chn == NULL || attr == NULL) {
        HI_ERR_VENC("NULL point\n");
        return HI_FAILURE;
    }
    venc_cfg.omx_private.auto_request_i_frame = HI_FALSE;
    venc_cfg.omx_private.prepend_sps_pps   = HI_TRUE;
    venc_cfg.omx_private.omx_chan          = HI_FALSE;
    venc_cfg.omx_private.control_rate_type  = HI_VENC_CONTROL_RATE_CONSTANT;
    venc_cfg.omx_private.h264_level        = 0;
    VENC_CHECK_NEQ_RET(memcpy_s(&venc_cfg, sizeof(hi_venc_chan_info), attr, sizeof(hi_venc_chan_info)),
        HI_SUCCESS, HI_FAILURE);

    ret = osal_sem_down_interruptible(&g_venc_drv_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    ret = venc_drv_create_chn(&kernel_venc_chn, &venc_cfg, &buf_info, omx_chn, file);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("create channel err!\n");
        osal_sem_up(&g_venc_drv_mutex);
        return HI_ERR_VENC_CREATE_ERR;
    }

    D_VENC_GET_CHN(index, kernel_venc_chn);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_drv_mutex);
        return HI_ERR_VENC_CREATE_ERR;
    }

    g_venc_chn[index].user_handle = GET_VENC_CHHANDLE(index);
    *venc_chn = g_venc_chn[index].user_handle;
    osal_sem_up(&g_venc_drv_mutex);

    return ret;
}

hi_s32 hi_drv_venc_destroy(hi_handle venc_chan)
{
    hi_s32 ret;
    hi_u32 index;

    // VENC
    ret = osal_sem_down_interruptible(&g_venc_drv_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, venc_chan);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_drv_mutex);
        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_destroy_chn(g_venc_chn[index].venc_handle);

    osal_sem_up(&g_venc_drv_mutex);

    return ret;
}

hi_s32 hi_drv_venc_attach_input(hi_handle venc_chan, hi_handle src)
{
    hi_s32 ret;
    hi_u32 chan_id;
    vedu_efl_enc_para *venc_chn = HI_NULL;
    D_VENC_GET_CHN_BY_UHND(chan_id, venc_chan);
    D_VENC_CHECK_CHN(chan_id);
    venc_chn = g_venc_chn[chan_id].venc_handle;
    ret = osal_sem_down_interruptible(&g_venc_drv_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    ret = venc_drv_attach_input(venc_chn, src, ((src & 0xff0000) >> 16)); /* use 16 to get valid data */
    osal_sem_up(&g_venc_drv_mutex);
    return ret;

}

hi_s32 hi_drv_venc_detach_input(hi_handle venc_chan)
{
    hi_s32 ret;
    hi_handle src;
    hi_u32 chan_id;
    vedu_efl_enc_para *venc_chn = HI_NULL;

    ret = osal_sem_down_interruptible(&g_venc_drv_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(chan_id, venc_chan);
    D_VENC_CHECK_CHN(chan_id);
    venc_chn = g_venc_chn[chan_id].venc_handle;
    src = venc_chn->src_info.handle;
    ret = venc_drv_detach_input(venc_chn, src, ((src & 0xff0000) >> 16)); /* use 16 to get valid data */
    osal_sem_up(&g_venc_drv_mutex);

    return ret;
}

hi_s32 hi_drv_venc_start(hi_handle venc_chan)
{
    hi_s32 ret;
    hi_u32 chan_id;
    vedu_efl_enc_para* venc_chn = HI_NULL;
    ret = osal_sem_down_interruptible(&g_venc_drv_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(chan_id, venc_chan);
    D_VENC_CHECK_CHN(chan_id);
    venc_chn = g_venc_chn[chan_id].venc_handle;
    ret = venc_drv_start_receive_pic(venc_chn);
    osal_sem_up(&g_venc_drv_mutex);
    return ret;
}

hi_s32 hi_drv_venc_stop(hi_handle venc_chan)
{
    hi_s32 ret;
    hi_u32 chan_id;
    vedu_efl_enc_para *venc_chn = HI_NULL;
    ret = osal_sem_down_interruptible(&g_venc_drv_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(chan_id, venc_chan);
    D_VENC_CHECK_CHN(chan_id);
    venc_chn = g_venc_chn[chan_id].venc_handle;
    ret = venc_drv_stop_receive_pic(venc_chn);
    osal_sem_up(&g_venc_drv_mutex);
    return ret;
}

hi_s32 hi_drv_venc_acquire_stream(hi_handle venc_chan, hi_venc_stream *stream, hi_u32 timeout_ms)
{
    venc_ioctl_buf_offset buf_offset;
    hi_u32 chan_id;
    vedu_efl_enc_para *venc_chn = HI_NULL;
    D_VENC_GET_CHN_BY_UHND(chan_id, venc_chan);
    D_VENC_CHECK_CHN(chan_id);
    venc_chn = g_venc_chn[chan_id].venc_handle;
    return venc_drv_quire_stream(venc_chn, stream, timeout_ms, &buf_offset);
}

hi_s32 hi_drv_venc_release_stream(hi_handle venc_chan, hi_venc_stream *stream)
{
    hi_s32 ret;
    hi_u32 chan_id;
    vedu_efl_enc_para *venc_chn = HI_NULL;
    ret = osal_sem_down_interruptible(&g_venc_drv_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(chan_id, venc_chan);
    D_VENC_CHECK_CHN(chan_id);
    venc_chn = g_venc_chn[chan_id].venc_handle;
    ret = venc_drv_release_stream_intar(venc_chn, stream);
    osal_sem_up(&g_venc_drv_mutex);
    return ret;
}

hi_s32 hi_drv_venc_set_attr(hi_handle venc_chan, hi_venc_chan_info *attr)
{
    hi_s32 ret;
    hi_u32 chan_id;
    vedu_efl_enc_para *venc_chn = HI_NULL;
    ret = osal_sem_down_interruptible(&g_venc_drv_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(chan_id, venc_chan);
    D_VENC_CHECK_CHN(chan_id);
    venc_chn = g_venc_chn[chan_id].venc_handle;
    ret = venc_drv_set_attr(venc_chn, attr, HI_NULL);
    osal_sem_up(&g_venc_drv_mutex);
    return ret;
}

hi_s32 hi_drv_venc_get_attr(hi_handle venc_chan, hi_venc_chan_info *attr)
{
    hi_s32 ret;
    hi_u32 chan_id;
    vedu_efl_enc_para *venc_chn = HI_NULL;
    ret = osal_sem_down_interruptible(&g_venc_drv_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(chan_id, venc_chan);
    D_VENC_CHECK_CHN(chan_id);
    venc_chn = g_venc_chn[chan_id].venc_handle;
    ret = venc_drv_get_attr(venc_chn, attr);
    osal_sem_up(&g_venc_drv_mutex);
    return ret;
}

hi_s32 hi_drv_venc_request_i_frame(hi_handle venc_chan)
{
    hi_s32 ret;
    hi_u32 chan_id;
    vedu_efl_enc_para *venc_chn = HI_NULL;
    ret = osal_sem_down_interruptible(&g_venc_drv_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(chan_id, venc_chan);
    D_VENC_CHECK_CHN(chan_id);
    venc_chn = g_venc_chn[chan_id].venc_handle;
    ret = venc_drv_request_i_frame(venc_chn);
    osal_sem_up(&g_venc_drv_mutex);
    return ret;
}

hi_s32 hi_drv_venc_queue_frame(hi_handle venc_chan, hi_drv_video_frame *frame_info)
{
    hi_s32 ret;
    hi_u32 chan_id;
    vedu_efl_enc_para *venc_chn = HI_NULL;
    ret = osal_sem_down_interruptible(&g_venc_drv_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(chan_id, venc_chan);
    D_VENC_CHECK_CHN(chan_id);
    venc_chn = g_venc_chn[chan_id].venc_handle;
    ret = venc_drv_queue_frame(venc_chn, frame_info);
    osal_sem_up(&g_venc_drv_mutex);
    return ret;
}

hi_s32 hi_drv_venc_dequeue_frame(hi_handle venc_chan, hi_drv_video_frame *frame_info)
{
    hi_s32 ret;
    hi_u32 chan_id;
    vedu_efl_enc_para *venc_chn = HI_NULL;
    ret = osal_sem_down_interruptible(&g_venc_drv_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(chan_id, venc_chan);
    D_VENC_CHECK_CHN(chan_id);
    venc_chn = g_venc_chn[chan_id].venc_handle;
    ret = venc_drv_dequeue_frame(venc_chn, frame_info);
    osal_sem_up(&g_venc_drv_mutex);
    return ret;
}


hi_s32 hi_drv_venc_open_by_opentv(hi_void)
{
    hi_s32 ret;
    hi_s32 i;
    hi_u32 license_valid = 0;

    ret = osal_sem_down_interruptible(&g_venc_drv_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    if (osal_atomic_inc_return(&g_venc_count) == 1) {
        ret = hi_drv_get_license_support(HI_LICENSE_ENCODE_EN, &license_valid);
        if ((license_valid == 0) || (ret != HI_SUCCESS)) {
            HI_FATAL_VENC("get encode license ret 0x%x , valid = %d, encode not allowed!\n", ret, license_valid);
            goto ERROR;
        }

        venc_drv_init_event();

        ret = venc_drv_efl_open_vedu();
        if (ret != HI_SUCCESS) {
            HI_FATAL_VENC("vedu_efl_open_vedu failed, ret=%d\n", ret);
            goto ERROR;
        }

        for (i = 0; i < HI_VENC_MAX_CHN; i++) {
            VENC_CHECK_NEQ_RET(memset_s(&(g_venc_chn[i]), sizeof(optm_venc_chn), 0, sizeof(optm_venc_chn)),
                HI_SUCCESS, HI_FAILURE);
            g_venc_chn[i].venc_handle = NULL;
        }

        osal_timer_init(&g_venc_timer);
        g_venc_timer.handler = venc_timer_func;
        osal_timer_set(&g_venc_timer, TIMER_INTERVAL_MS);
        g_venc_open_flag = 1;

#ifdef VENC_SUPPORT_JPGE
        ret = hi_drv_module_get_func(HI_ID_JPGENC, (hi_void**)&g_jpge_func);
        if (ret != HI_SUCCESS) {
            HI_ERR_VENC("get jpge function failed!\n");
            g_jpge_func = HI_NULL;
        }
#endif
    }

    osal_sem_up(&g_venc_drv_mutex);

    return HI_SUCCESS;

ERROR:
    osal_atomic_dec_return(&g_venc_count);
    osal_sem_up(&g_venc_drv_mutex);
    return HI_FAILURE;
}


hi_s32 hi_drv_venc_close_by_opentv(struct file *file)
{
    hi_u32 i = 0;
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_venc_drv_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    osal_timer_destory(&g_venc_timer);

    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        if ((g_venc_chn[i].which_file == file)
            && (g_venc_chn[i].venc_handle != NULL)) {

            HI_INFO_VENC("try venc_destroy_chn %d/%#x.\n", i, g_venc_chn[i].venc_handle);
            ret = (hi_u32)ret | (hi_u32)venc_drv_destroy_chn(g_venc_chn[i].venc_handle);
            if (ret != HI_SUCCESS) {
                HI_WARN_VENC("force destroy_chn %d failed, ret=%#x.\n", i, ret);
            }

            g_venc_chn[i].which_file = HI_NULL;
        }
    }

    if (osal_atomic_dec_return(&g_venc_count) == 0) {
        venc_drv_efl_close_vedu();
        venc_drv_board_deinit();
        g_venc_open_flag = 0;
    }

    osal_sem_up(&g_venc_drv_mutex);

    return HI_SUCCESS;
}
#endif

#ifdef VENC_UT_ENABLE
EXPORT_SYMBOL(vedu_drv_rc_init_qp);
EXPORT_SYMBOL(vedu_drv_rc_of_first_frame);
EXPORT_SYMBOL(vedu_drv_rc_modify_qp_for_large_frame_bits);
EXPORT_SYMBOL(vedu_drv_rc_calculate_qp_delta_by_frame_bits);
EXPORT_SYMBOL(vedu_drv_rc_calculate_qp_delta_by_instant_bits);
EXPORT_SYMBOL(vedu_drv_rc_calculate_qp_delta_by_bit_rate);
EXPORT_SYMBOL(vedu_drv_rc_calculate_qp_delta_by_water_line);
EXPORT_SYMBOL(vedu_drv_rc_calculate_current_qp);
EXPORT_SYMBOL(vedu_drv_rc_update_frame_bits);
EXPORT_SYMBOL(vedu_drv_rc_calculate_time_of_p);
EXPORT_SYMBOL(vedu_drv_rc_modify_info_for_frame_rate_change);
EXPORT_SYMBOL(vedu_drv_rc_update_instant_bits);
EXPORT_SYMBOL(vedu_drv_rc_get_target_frame_bits);
EXPORT_SYMBOL(venc_drv_rc_start_qp_change);
EXPORT_SYMBOL(venc_drv_rc_is_need_recoding);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

