/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-04-22
 */
#include <linux/uaccess.h>
#include <linux/dma-buf.h>

#include "hi_drv_file.h"
#include "hi_drv_mem.h"
#include "hi_venc_type.h"
#include "drv_venc.h"
#include "hi_drv_log.h"
#include "public.h"
#include "hi_drv_sys.h"
#include "hi_osal.h"
#include "linux/hisilicon/securec.h"

#define CMD_LEN_MAX      256

#ifdef HI_PROC_SUPPORT
hi_char  g_venc_save_path[PATH_LEN]    = {'/', 'm', 'n', 't', '\0'};
extern optm_venc_chn g_venc_chn[HI_VENC_MAX_CHN];
extern osal_spinlock g_send_frame_lock[HI_VENC_MAX_CHN];     /* lock the destroy and send frame */

extern hi_bool is_chip_id_v500_r001(hi_chip_type chip_type, hi_chip_version chip_version);

#define D_VENC_GET_CHN(ve_chn, venc_chn) do { \
    if ((venc_chn) == NULL) { \
        (ve_chn) = HI_VENC_MAX_CHN; \
        break; \
    } \
    for ((ve_chn) = 0; (ve_chn) < HI_VENC_MAX_CHN; (ve_chn)++) { \
        if (g_venc_chn[ve_chn].venc_handle == (venc_chn)) { break; } \
    } \
} while (0)


static hi_void venc_drv_proc_help(hi_void)
{
    osal_printk("------ VENC proc help ------\n");
    osal_printk("USAGE:echo [cmd] [para1] [para2] > /proc/msp/venc_xx\n");
    osal_printk("cmd = save_yuv,    para1 = start   start to save the yuv data before encode\n");
    osal_printk("cmd = save_yuv,    para1 = stop    stop to save the yuv data before encode\n");
    osal_printk("cmd = save_stream, para1 = second  save the streams after encode for [para2] seconds\n");
    osal_printk("cmd = save_stream, para1 = frame  save the streams after encode for [para2] frames\n");
    osal_printk("cmd = clk_gate_en,   para1          if [para1]=0,means unable clk_gate\n");
    osal_printk("if [para1]=1,means enable clk_gate of frame level\n");
    osal_printk("if [para1]=2,means enable clk_gate both of frame and MB level\n");
    osal_printk("cmd = low_pow_en, para1  if [para1]=0,means unable the low power control of algorithm\n");
    osal_printk("if [para1]=0,means enable the low power control of algorithm\n");
    osal_printk("cmd = rcratioprint, para1 = 1 start to print the rate control ratio; \
        para1 = 0 stop to print the rate control ratio\n");
    osal_printk("cmd = save_file_path,para1 = path   set save file path\n");
    osal_printk("cmd = rc_mode      , para1         if [para1]=avbr,means set rc_mode to AVBR\n");
    osal_printk("if [para1]=cbr ,means set rc_mode to CBR\n");
    osal_printk("cmd = request_i    ,               request I frame\n");
    osal_printk("cmd = in_frmrate_type, para1if [para1]=auto,  \
        means set in_frmrate_type to auto detection\n");
    osal_printk("if [para1]=stream,means set in_frmrate_type from stream info\n");
    osal_printk("if [para1]=user,means set in_frmrate_type to user config\n");
}

static hi_s32 val2str(hi_u32 data, hi_s32 radix, hi_char* str, hi_u32 str_len)
{
    hi_char tmp[32] = {'\0'}; /* 32 is count size */
    hi_u32 tmpval = data;
    hi_s32 i, j;

    if (radix == 0) {
        HI_PRINT("err:val2str-radix is zero!\n");
        return -1;
    }

    for (i = 0; i < 32; i++) { /* 32 is max count num */
        tmp[i] = (tmpval % radix) + '0';
        tmpval = tmpval / radix;

        if (tmpval == 0) {
            break;
            }
    }

    for (j = 0; (i >= 0) && (i < 32) && (j < (str_len - 1)); i--) { /* 32 is max count num */
        str[j++] = tmp[i];
    }

    str[j] = '\0';
    return 0;
}

static hi_bool is_parameter_set(vedu_efl_nalu* nalu, hi_u32 protocol)
{
    hi_bool flag = HI_FALSE;
    hi_chip_type chip_type = 0;
    hi_chip_version chip_version = 0;

    hi_drv_sys_get_chip_version(&chip_type, &chip_version);

    if (protocol == VEDU_H264) {
        flag = (nalu->nalu_type == H264_NALU_TYPE_SPS) ? HI_TRUE : HI_FALSE;
    } else if (protocol == VEDU_H265) {
        if (is_chip_id_v500_r001(chip_type, chip_version)) {
            flag = (nalu->nalu_type == H265_NALU_TYPE_SPS) ? HI_TRUE : HI_FALSE;
        } else {
            flag = (nalu->nalu_type == H265_NALU_TYPE_VPS) ? HI_TRUE : HI_FALSE;
        }
    } else if (protocol == VEDU_JPGE) {
        flag = HI_TRUE;
    }

    return flag;
}

static hi_u32 get_current_level(int enc_width, int enc_height)
{
    hi_u32 total_mb;
    hi_u32 width_in_mb = (hi_u32)(enc_width + 15) >> 4; /* 15 is size ,4 is left move */
    hi_u32 height_in_mb = (hi_u32)(enc_height + 15) >> 4; /* 15 is size ,4 is left move */
    hi_u32 level_idc;
    total_mb = width_in_mb * height_in_mb;

    if (total_mb <= 99) { /* 99 is total mb num */
        level_idc = 10; /* 10 is level idc num */
    } else if (total_mb <= 396) { /* 396 is total mb num */
        level_idc = 20; /* 20 is level idc num */
    } else if (total_mb <= 792) { /* 792 is total mb num */
        level_idc = 21; /* 21 is level idc num */
    } else if (total_mb <= 1620) { /* 1620 is total mb num */
        level_idc = 30; /* 30 is level idc num */
    } else if (total_mb <= 3600) { /* 3600 is total mb num */
        level_idc = 31; /* 31 is level idc num */
    } else if (total_mb <= 5120) { /* 5120 is total mb num */
        level_idc = 32; /* 32 is level idc num */
    } else if (total_mb <= 8192) { /* 8192 is total mb num */
        level_idc = 41; /* 41 is level idc num */
    } else {
        level_idc = 0;
    }
    return level_idc;
}


static hi_s32 venc_drv_proc_read(hi_void *p, hi_void *v)
{
    hi_s32 ret = HI_FAILURE;
    osal_proc_entry *proc_item = NULL;
    vedu_efl_enc_para* enc_para = NULL;
    vedu_efl_enc_para* venc = NULL;
    hi_u32 chn_id = 0;
    hi_u32 skip_frm_num;
    hi_u32 level_id;
    hi_chip_type chip_type = 0;
    hi_chip_version chip_version = 0;
    vedu_state_info stat_state_info;
    vedu_state_info* stat_infor = &stat_state_info;
    strm_mng_state strm_state;
    strm_mng_state *strm_state_info = &strm_state;
    hi_u32 strm_buf_used = 0;
    hi_u32 strm_buf_used_per = 0;
    hi_u32 check_buf_size_num_fail;
    hi_u32 strm_head_free = 0;
    hi_u32 strm_tail_free = 0;
    hi_u32  src_id;
    hi_char src_tab[4][8] = {{"VI"}, {"win"}, {"DISP"}, {"user"}};  /* 4  8 is size of src tab */

    hi_char sz_protocol[][8] = {"H.264", "H.265", "JPEG", "MPEG2", "MPEG4", "AVS",   /* 8 is size of sz_protocol */
        "H.263", "REAL8", "REAL9", "VC1", "VP6", "VP6F", "VP6A", "SORENSON", "MJPEG", "DIVX3", "RAW", "UNKNOWN",
        "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
        "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
        "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN"
        };
    hi_char sz_avc_profile[5][10] = {"baseline", "main", "extended", "high", "UNKNOWN"}; /* 5 10 */
    hi_char sz_bool_tab[2][8] = {"FALSE", "TRUE"}; /* 2 8  is sie of sz_bool_tab */
    hi_char sz_state_tab[2][8] = {"stop", "start"}; /* 2 8  is sie of sz_state_tab */
    hi_char sz_pix_format[9][16] = {"SP420_UV", "SP420_VU", "planer420",  "planer422",  /* 9 16  is size */
        "package422_yuyv",  "package422_uyvy", "package422_yvyu", "SP422", "UNKNOWN"};
    hi_char sz_buf_state_tab[3][8] = {"NA", "NA", "NA"}; /* 3 8  is size of sz_buf_state_tab */
    hi_char sz_max_min_qp[2][8] = {"NA", "NA"}; /* 2 8  is size of sz_max_min_qp */
    hi_char sz_input_frm_rate_type[4][8] =  /* 4 8  is size of sz_input_frm_rate_type */
        {"auto", "stream", "user", "UNKNOWN"};
    hi_char sz_rc_skip_thr[8] = ""; /* 8 is size of sz_rc_skip_thr */
    hi_s32 i_catch_enable;
    hi_s32 input_frm_rate_type_default_id;
    hi_s32 input_frm_rate_type_config_id;
    hi_s32 is_avbr = 0;
    hi_s32 low_dly_mod;
    hi_u32 protocol;
    hi_u32 check_buf_size = 0;
    hi_u32 slice_size = 0;
    unsigned long flags;
    hi_bool enable_rcn_ref_share_buf;

    proc_item = ((struct seq_file *)p)->private;
    venc = (vedu_efl_enc_para *)proc_item->private;

    D_VENC_GET_CHN(chn_id, venc);
    D_VENC_CHECK_CHN(chn_id);

    hi_drv_sys_get_chip_version(&chip_type, &chip_version);

    osal_spin_lock_irqsave(&g_send_frame_lock[chn_id], &flags);
    enc_para = (vedu_efl_enc_para*)g_venc_chn[chn_id].venc_handle;
    if ((enc_para != NULL) && (venc_drv_check_chn_state_safe(chn_id) == HI_TRUE)) {
        ret = venc_drv_efl_query_stat_info(enc_para, stat_infor);

        if (ret != HI_SUCCESS) {
            osal_spin_unlock_irqrestore(&g_send_frame_lock[chn_id], &flags);

            return HI_FAILURE;
        }

        ret = strm_get_strm_state(enc_para->strm_mng, strm_state_info);

        if (ret != HI_SUCCESS) {
            osal_spin_unlock_irqrestore(&g_send_frame_lock[chn_id], &flags);

            return HI_FAILURE;
        }

        i_catch_enable = enc_para->i_catch_enable;
        is_avbr       = enc_para->rc.is_avbr;
        low_dly_mod    = enc_para->rc.low_delay_mode;
        protocol     = enc_para->protocol;
        check_buf_size = enc_para->cur_strm_buf_size;
        slice_size    = enc_para->hal.split_size;
        input_frm_rate_type_default_id = enc_para->input_frm_rate_type_default;
        input_frm_rate_type_config_id  = enc_para->input_frm_rate_type_config;
        enable_rcn_ref_share_buf = enc_para->enable_rcn_ref_share_buf;
    } else {
        osal_spin_unlock_irqrestore(&g_send_frame_lock[chn_id], &flags);
        return HI_FAILURE;
    }
        osal_spin_unlock_irqrestore(&g_send_frame_lock[chn_id], &flags);

    if (protocol == VEDU_JPGE) {
#ifdef VENC_SUPPORT_JPGE
        strm_buf_used = stat_infor->used_stream_buf;
        strm_buf_used_per = strm_buf_used * 100 /   /* 100 is size of buf */
            g_venc_chn[chn_id].chn_user_cfg.strm_buf_size;
        strm_head_free = 0;
        strm_tail_free = 0;
#endif
    } else {
        if (is_chip_id_v500_r001(chip_type, chip_version)) {
            strm_buf_used = g_venc_chn[chn_id].chn_user_cfg.strm_buf_size -  /* 100 is size of buf */
                strm_state_info->strm_head_free - strm_state_info->strm_tail_free - 64; /* 64 is size of buf */
            strm_buf_used_per = strm_buf_used * 100 /  /* 100 is size of buf */
                g_venc_chn[chn_id].chn_user_cfg.strm_buf_size;
            strm_head_free = strm_state_info->strm_head_free;
            strm_tail_free = strm_state_info->strm_tail_free;
        } else {
            strm_buf_used = stat_infor->used_stream_buf;
            strm_buf_used_per = strm_buf_used * 100 / g_venc_chn[chn_id].chn_user_cfg.strm_buf_size; /* 100 is size */
            strm_head_free = 0;
            strm_tail_free = 0;
        }
    }

    check_buf_size_num_fail = stat_infor->check_buf_size_num_fail;

    switch (g_venc_chn[chn_id].src_mod_id) {
#ifdef VENC_SUPPORT_ATTACH_VI

        case HI_ID_VI:
            src_id = 0;
            break;
#endif
#ifdef VENC_SUPPORT_ATTACH_VO

        case HI_ID_WIN:
            src_id = 1;
            break;
#endif
#ifdef VENC_SUPPORT_ATTACH_CAST

        case HI_ID_DISP:
            src_id = 2; /* 2 is mean HI_ID_DISP */
            break;
#endif

        default:
            src_id = 3; /* 3 is mean default */
            break;
    }

    memcpy_s(sz_buf_state_tab[0], sizeof(sz_buf_state_tab[0]), "6", 2); /* 2 ,6 is size of array */
    val2str(stat_infor->queue_num, 10, sz_buf_state_tab[1], 8); /* 10 is queue_num 8 is array size */
    val2str(stat_infor->dequeue_num, 10, sz_buf_state_tab[2], 8); /* 10 is queue_num 8 is array size, 2: tab number */

    if (g_venc_chn[chn_id].chn_user_cfg.standard == HI_VENC_STD_JPEG) {
#ifdef VENC_SUPPORT_JPGE
        /* CBR */
        memcpy_s(sz_max_min_qp[0], sizeof(sz_max_min_qp[0]), "NA", 3); /* 3 is size of qp */
        memcpy_s(sz_max_min_qp[1], sizeof(sz_max_min_qp[1]), "NA", 3); /* 3 is size of qp */
#endif
    } else {
        /* VBR */
        val2str(g_venc_chn[chn_id].chn_user_cfg.rc_param.max_qp, 10, sz_max_min_qp[0], 8); /* 8,10 is size of qb */
        val2str(g_venc_chn[chn_id].chn_user_cfg.rc_param.min_qp, 10, sz_max_min_qp[1], 8); /* 8,10 is size of qb */
    }

    if (g_venc_chn[chn_id].chn_user_cfg.frm_drop_strategy.bit_rate_threshold > 100) { /* 100 is min size of threshold */
        memcpy_s(sz_rc_skip_thr, sizeof(sz_rc_skip_thr), "NA", 3); /* 3 is size of thr */
    } else {
        val2str(g_venc_chn[chn_id].chn_user_cfg.frm_drop_strategy.bit_rate_threshold,
            10, sz_rc_skip_thr, 8); /* 8 10  is size */
        strncat_s(sz_rc_skip_thr, sizeof(sz_rc_skip_thr), "%", 2); /* 2 is size of thr */
    }

    level_id = get_current_level(g_venc_chn[chn_id].chn_user_cfg.config.width,
        g_venc_chn[chn_id].chn_user_cfg.config.height);
    skip_frm_num = stat_infor->quick_encode_skip + stat_infor->err_cfg_skip + stat_infor->frm_rc_ctrl_skip
        + stat_infor->too_few_buffer_skip + stat_infor->same_pts_skip + stat_infor->too_many_bits_skip;

    osal_proc_print(p, "--------------------- VENC[%02d] -----------------------\n", chn_id);
    osal_proc_print(p, "save_path                      :%s\n", g_venc_save_path);
    osal_proc_print(p, "-------------------- user  config ----------------------\n");
    osal_proc_print(p,
                    "version                          :%s\n"
                    "codec_id                         :%s(0x%x)\n"
                    "capability                       :%dx%d\n"
                    "profile(level)                   :%s(%d)\n"
                    "resolution                       :%ux%u\n"
                    "target_bit_rate                  :%u(kbps)\n"
                    "gop                              :%u\n"
                    "frm_rate(input/out_put)          :%u/%u(fps)\n"
                    "priority                         :%u\n"
                    "quick_encode                     :%s\n"
                    "drift_rate_thr                   :%s\n"
                    "split                            :enable(%s)    size(%u byte)\n"
                    "stream_buf_size                  :%u(KB)\n"
                    "max_qp/min_qp                    :%s/%s\n"
                    "q_level                          :%u\n"
                    "auto_request_i_frm               :enable(%s)\n"
                    "rc_mode                          :%s\n"
                    "in_frm_rate_type(default/config) :%s/%s\n",
                    VENC_FIRMWARE_VERSION,
                    sz_protocol[g_venc_chn[chn_id].chn_user_cfg.standard], g_venc_chn[chn_id].chn_user_cfg.standard,
                    g_venc_chn[chn_id].chn_user_cfg.max_width, g_venc_chn[chn_id].chn_user_cfg.max_height,
                    sz_avc_profile[g_venc_chn[chn_id].chn_user_cfg.profile], level_id,
                    g_venc_chn[chn_id].chn_user_cfg.config.width,
                    g_venc_chn[chn_id].chn_user_cfg.config.height,
                    g_venc_chn[chn_id].chn_user_cfg.config.target_bitrate / 1024U,
                    g_venc_chn[chn_id].chn_user_cfg.config.gop,
                    g_venc_chn[chn_id].chn_user_cfg.config.input_frame_rate,
                    g_venc_chn[chn_id].chn_user_cfg.config.target_frame_rate,
                    g_venc_chn[chn_id].chn_user_cfg.config.priority,
                    sz_bool_tab[g_venc_chn[chn_id].chn_user_cfg.config.quick_encode],
                    sz_rc_skip_thr,
                    sz_bool_tab[g_venc_chn[chn_id].chn_user_cfg.slc_split_enable], slice_size,
                    g_venc_chn[chn_id].chn_user_cfg.strm_buf_size / 1000U,
                    sz_max_min_qp[0], sz_max_min_qp[1],
                    g_venc_chn[chn_id].chn_user_cfg.config.qfactor,
                    sz_bool_tab[i_catch_enable],
                    is_avbr ? "AVBR" : "CBR",
                    sz_input_frm_rate_type[input_frm_rate_type_default_id],
                    sz_input_frm_rate_type[input_frm_rate_type_config_id]);
    osal_proc_print(p, "------------------ real-time  statistics ----------------------\n");
    osal_proc_print(p,
                    "work_status                      :%s\n"
                    "source_id                        :%s%02u\n"
                    "frame_info                       :%s\n"
                    "low_delay_mod                    :%d\n"
                    "rcn_ref_share_buf                :%d\n"
                    "input_frm_rate(use/real)         :%u/%u(fps)\n"
                    "target_frm_rate(use/real)        :%u/%u(fps)\n"
                    "bit_rate                         :%u(kbps)\n"
                    "rate_control_ratio               :%d%%\n"
                    "encode_num                       :%u\n"
                    "recode_num                       :%u\n"
                    "skip_num                         :total(%u) frm_rate_ctrl(%u) same_pts(%u) quick_encode(%u) \
too_few_buf(%u) too_many_bits(%u) err_cfg(%u)\n"
                    "frame_buffer:\n"
                    "    venc_queue_buf(total/queue/dequeue)   :%s/%s/%s\n"
                    "stream_buffer:\n"
                    "    total/used/percent(bytes)    :%u/%u/%u%%\n"
                    "    free_head/free_tail(bytes)   :%d/%d\n"
                    "statistics(total):\n"
                    "    acquire_frame(try/OK)        :%d/%d\n"
                    "    release_frame(try/OK)        :%d/%d\n"
                    "    acquire_stream(try/OK)       :%d/%d\n"
                    "    release_stream(try/OK)       :%d/%d\n"
                    "    input        (ETB/EBD)       :%d/%d\n"
                    "    output       (FTB/FBD)       :%d/%d\n"
                    "    check_buf(size/fail_cnt)     :%d/%d\n"
                    "    start_failed_cnt             :%d\n"
                    "    get_stream_buf(try/OK)       :%d/%d\n"
                    "statistics(per_second):\n"
                    "    acquire_frame(try/OK)        :%d/%d\n"
                    "    release_frame(try/OK)        :%d/%d\n",
                    sz_state_tab[g_venc_chn[chn_id].enable],
                    src_tab[src_id], (g_venc_chn[chn_id].source == HI_INVALID_HANDLE ? 0 : g_venc_chn[chn_id].source & \
                    0xff),
                    sz_pix_format[stat_infor->frame_type],
                    low_dly_mod, enable_rcn_ref_share_buf,
                    stat_infor->real_send_input_frm_rate, g_venc_chn[chn_id].last_sec_input_fps,
                    stat_infor->real_send_output_frm_rate, g_venc_chn[chn_id].last_sec_encoded_fps,
                    g_venc_chn[chn_id].last_sec_kbps * 8 / 1024U, /* 8 is kbps */
                    stat_infor->rate_control_ratio,
                    (stat_infor->get_frame_num_ok - skip_frm_num),
                    stat_infor->recode_num,
                    skip_frm_num,
                    stat_infor->frm_rc_ctrl_skip, stat_infor->same_pts_skip, stat_infor->quick_encode_skip,
                    stat_infor->too_few_buffer_skip, stat_infor->too_many_bits_skip, stat_infor->err_cfg_skip,
                    sz_buf_state_tab[0], sz_buf_state_tab[1], sz_buf_state_tab[2],      /* 2: tab number */
                    g_venc_chn[chn_id].chn_user_cfg.strm_buf_size, strm_buf_used, strm_buf_used_per,
                    strm_head_free, strm_tail_free,
                    stat_infor->get_frame_num_try, stat_infor->get_frame_num_ok,
                    stat_infor->put_frame_num_try, stat_infor->put_frame_num_ok,
                    stat_infor->get_stream_num_try, stat_infor->get_stream_num_ok,
                    stat_infor->put_stream_num_try, stat_infor->put_stream_num_ok,
                    stat_infor->etb_cnt, stat_infor->ebd_cnt,
                    stat_infor->ftb_cnt, stat_infor->fbd_cnt,
                    check_buf_size, check_buf_size_num_fail,
                    stat_infor->start_one_frame_fail,
                    stat_infor->get_stream_buf_try, stat_infor->get_stream_buf_ok,
                    g_venc_chn[chn_id].last_sec_try_num, g_venc_chn[chn_id].last_sec_ok_num,
                    g_venc_chn[chn_id].last_sec_put_num, g_venc_chn[chn_id].last_sec_put_num);

    return HI_SUCCESS;
}

static hi_s32 venc_drv_proc_cmd_save_yuv(hi_u32 argc,
    hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    vedu_efl_enc_para *enc_para = (vedu_efl_enc_para *)private;
    hi_u32 chn_id;

    D_VENC_GET_CHN(chn_id, enc_para);
    D_VENC_CHECK_CHN(chn_id);

    if (argc != 2) { /* 2 is param num */
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    if (!strcmp(argv[1], "start"))  {
        g_venc_chn[chn_id].proc_write.save_yuv_file_run = HI_TRUE;
    }  else if (!strcmp(argv[1], "stop")) {
        g_venc_chn[chn_id].proc_write.save_yuv_file_run = HI_FALSE;
    } else {
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void venc_drv_proc_save_stream_by_second(hi_char *time, hi_u32 chn_id)
{
    hi_char file_name[130]; /* 130 is size of file_name */
    hi_u32 parm;
    hi_s32 ret;

    parm = osal_strtoul(time, NULL, OSAL_BASE_DEC);
    if (parm > 3600)  { /* 3600 is parm min size */
        HI_ERR_VENC("error: not support save too large stream file!\n");
        return;
    }

    ret = snprintf_s(file_name, sizeof(file_name), sizeof(file_name), "%s/%s", g_venc_save_path,
        g_venc_chn[chn_id].proc_write.stream_file_name);
    if (ret < 0) {
        HI_ERR_VENC("snprintf_s check  err! ret = %d\n", ret);
        return;
    }

    g_venc_chn[chn_id].proc_write.fp_save_file = venc_drv_osal_fopen(file_name,  O_RDWR | O_CREAT | O_APPEND, 0);
    if (g_venc_chn[chn_id].proc_write.fp_save_file == HI_NULL) {
        HI_ERR_VENC("can not create %s file.\n", file_name);
        g_venc_chn[chn_id].proc_write.time_mode_run = HI_FALSE;

        return;
    }
    g_venc_chn[chn_id].proc_write.time_mode_run = HI_TRUE;
    osal_msleep_uninterruptible(1000 * parm); /* 1000 is size */
    g_venc_chn[chn_id].proc_write.time_mode_run = HI_FALSE;
    venc_drv_osal_fclose(g_venc_chn[chn_id].proc_write.fp_save_file);

    return;
}

static hi_void venc_drv_proc_save_stream_by_frame(hi_char *frm_cnt, hi_u32 chn_id)
{
    hi_char file_name[130]; /* 130 is size of file_name */
    hi_u32 parm;
    hi_u32 cnt = 0;
    hi_s32 ret;

    parm = osal_strtoul(frm_cnt, NULL, OSAL_BASE_DEC);
    if (parm > 100000) { /* 100000 is parm min size */
        HI_ERR_VENC("error: not support save too large YUV file!\n");
        return;
    }

    ret = snprintf_s(file_name, sizeof(file_name), sizeof(file_name), "%s/%s", g_venc_save_path,
        g_venc_chn[chn_id].proc_write.stream_file_name);
    if (ret < 0) {
        HI_ERR_VENC("snprintf_s check  err! ret = %d\n", ret);
        return;
    }

    g_venc_chn[chn_id].proc_write.fp_save_file = venc_drv_osal_fopen(file_name,  O_RDWR | O_CREAT | O_APPEND, 0);

    if (g_venc_chn[chn_id].proc_write.fp_save_file == HI_NULL) {
        HI_ERR_VENC("can not create %s file.\n", file_name);
        g_venc_chn[chn_id].proc_write.frame_mode_run = HI_FALSE;

        return;
    }
    g_venc_chn[chn_id].proc_write.frame_mode_run = HI_TRUE;
    g_venc_chn[chn_id].proc_write.frame_mode_count = parm;
    while (1) {
        /* if the frame count reaches to aim, break */
        if (g_venc_chn[chn_id].proc_write.frame_mode_run == HI_FALSE) {
            break;
        } else {
            cnt++;
            if (cnt >= 6000) { /* 6000 is cnt min size */
                HI_ERR_VENC("time of saving frame is 60s,stop saving!\n");
                break;
            }
            osal_msleep_uninterruptible(100); /* 100 is us */
        }
    }
    venc_drv_osal_fclose(g_venc_chn[chn_id].proc_write.fp_save_file);

    return;
}

static hi_s32 venc_drv_proc_cmd_save_stream(hi_u32 argc,
    hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    vedu_efl_enc_para *enc_para = (vedu_efl_enc_para *)private;
    hi_u32 chn_id;

    D_VENC_GET_CHN(chn_id, enc_para);
    D_VENC_CHECK_CHN(chn_id);

    if (argc != 3) { /* 3 is param num */
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    if (!strcmp(argv[1], "second"))   /* time mode */ {
        venc_drv_proc_save_stream_by_second(argv[2], chn_id);   /* 2: second param */
    } else if (!strcmp(argv[1], "frame")) {
        venc_drv_proc_save_stream_by_frame(argv[2], chn_id);    /* 2: second param */
    } else {
        venc_drv_proc_help();
    }

    return HI_SUCCESS;
}

static hi_s32 venc_drv_proc_cmd_clk_gate_en(hi_u32 argc,
    hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    unsigned long flags;
    vedu_efl_enc_para *enc_para = (vedu_efl_enc_para *)private;
    hi_u32 chn_id;
    hi_bool err_log = HI_FALSE;

    D_VENC_GET_CHN(chn_id, enc_para);
    D_VENC_CHECK_CHN(chn_id);

    if (argc != 2) { /* 2 is param num */
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&g_send_frame_lock[chn_id], &flags);
    if (venc_drv_check_chn_state_safe(chn_id) == HI_TRUE)  {
        if (!strcmp(argv[1], "0")) {
            enc_para->hal.clk_gate_en = 0;
        } else if (!strcmp(argv[1], "1")) {
            enc_para->hal.clk_gate_en = 1;
        } else if (!strcmp(argv[1], "2")) {
            enc_para->hal.clk_gate_en = 2; /* 2 is one of state */
        } else {
            err_log = HI_TRUE;
        }
    }
    osal_spin_unlock_irqrestore(&g_send_frame_lock[chn_id], &flags);

    if (err_log) {
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


static hi_s32 venc_drv_proc_cmd_low_pow_en(hi_u32 argc,
    hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    unsigned long flags;
    vedu_efl_enc_para *enc_para = (vedu_efl_enc_para *)private;
    hi_u32 chn_id;
    hi_bool err_log = HI_FALSE;

    D_VENC_GET_CHN(chn_id, enc_para);
    D_VENC_CHECK_CHN(chn_id);

    if (argc != 2) { /* 2 is param num */
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&g_send_frame_lock[chn_id], &flags);
    if (venc_drv_check_chn_state_safe(chn_id) == HI_TRUE)  {
        if (!strcmp(argv[1], "0")) {
            enc_para->intra_low_pow_en    = 0;
            enc_para->intp_low_pow_en     = 0;
            enc_para->frac_low_pow_en     = 0;
        } else if (!strcmp(argv[1], "1")) {
            enc_para->intra_low_pow_en    = 1;
            enc_para->intp_low_pow_en     = 1;
            enc_para->frac_low_pow_en     = 1;
        } else {
            err_log = HI_TRUE;
        }
    }
    osal_spin_unlock_irqrestore(&g_send_frame_lock[chn_id], &flags);

    if (err_log) {
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


static hi_s32 venc_drv_proc_cmd_save_file_path(hi_u32 argc,
    hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (argc != 2) { /* 2 is param num */
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    if ((strlen(argv[1]) > sizeof(g_venc_save_path)) || (strlen(argv[1]) == 0)) {
        HI_ERR_VENC("%s lenght larger than sizeof(g_venc_save_path) = %d\n", argv[1], sizeof(g_venc_save_path));
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    VENC_CHECK_NEQ_RET(strncpy_s(g_venc_save_path, PATH_LEN, argv[1], PROC_CMD_SINGEL_LENGTH_MAX),
        HI_SUCCESS, HI_FAILURE);
    g_venc_save_path[PATH_LEN - 1] = '\0';
    osal_printk("[venc] set save file path = %s success\n", g_venc_save_path);

    return HI_SUCCESS;
}


static hi_s32 venc_drv_proc_write_print_rc_ratio(hi_u32 argc,
    hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    unsigned long flags;
    vedu_efl_enc_para *enc_para = (vedu_efl_enc_para *)private;
    hi_u32 chn_id;
    hi_bool err_log = HI_FALSE;

    D_VENC_GET_CHN(chn_id, enc_para);
    D_VENC_CHECK_CHN(chn_id);

    if (argc != 2) { /* 2 is param num */
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&g_send_frame_lock[chn_id], &flags);
    if (venc_drv_check_chn_state_safe(chn_id) == HI_TRUE) {
        if (!strcmp(argv[1], "0")) {
            enc_para->rc_dbg_print = 0;
        } else if (!strcmp(argv[1], "1")) {
            enc_para->rc_dbg_print = 1;
        } else {
            err_log = HI_TRUE;
        }
    }
    osal_spin_unlock_irqrestore(&g_send_frame_lock[chn_id], &flags);

    if (err_log) {
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 venc_drv_proc_write_set_rc_mode(hi_u32 argc,
    hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    unsigned long flags;
    vedu_efl_enc_para *enc_para = (vedu_efl_enc_para *)private;
    hi_u32 chn_id;
    hi_bool err_log = HI_FALSE;
    hi_venc_control_rate_type rc_type;

    D_VENC_GET_CHN(chn_id, enc_para);
    D_VENC_CHECK_CHN(chn_id);

    if (argc != 2) { /* 2 is param num */
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&g_send_frame_lock[chn_id], &flags);
    if (venc_drv_check_chn_state_safe(chn_id) == HI_TRUE) {
        if (!strcmp(argv[1], "cbr")) {
            rc_type = HI_VENC_CONTROL_RATE_CONSTANT;
        } else if (!strcmp(argv[1], "avbr")) {
            rc_type = HI_VENC_CONTROL_RATE_VARIABLE;
        } else {
            rc_type = HI_VENC_CONTROL_RATE_MAX;
            err_log = HI_TRUE;
        }
        if (rc_type != HI_VENC_CONTROL_RATE_MAX) {
            if (venc_drv_set_rate_control_type(enc_para, rc_type) == HI_SUCCESS) {
                osal_printk("set channel (%d) rc_mode to %s success.\n", chn_id, argv[1]);
            } else {
                osal_printk("set channel (%d) rc_mode to %s failed.\n", chn_id, argv[1]);
            }
        }
    }
    osal_spin_unlock_irqrestore(&g_send_frame_lock[chn_id], &flags);

    if (err_log) {
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 venc_drv_proc_write_request_i(hi_u32 argc,
    hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    unsigned long flags;
    vedu_efl_enc_para *enc_para = (vedu_efl_enc_para *)private;
    hi_u32 chn_id;
    hi_bool err_log = HI_FALSE;

    D_VENC_GET_CHN(chn_id, enc_para);
    D_VENC_CHECK_CHN(chn_id);

    if (argc != 1) { /* 1 is param num */
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&g_send_frame_lock[chn_id], &flags);
    if (venc_drv_check_chn_state_safe(chn_id) == HI_TRUE) {
        if (venc_drv_request_i_frame(enc_para) == HI_SUCCESS) {
            osal_printk("channel (%d) request I frame success.\n", chn_id);
        } else {
            osal_printk("channel (%d) request I frame failed.\n", chn_id);
            err_log = HI_TRUE;
        }
    }
    osal_spin_unlock_irqrestore(&g_send_frame_lock[chn_id], &flags);

    if (err_log) {
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 venc_drv_proc_write_set_input_frmrate_type(hi_u32 argc,
    hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    unsigned long flags;
    vedu_efl_enc_para *enc_para = (vedu_efl_enc_para *)private;
    hi_u32 chn_id;
    hi_bool err_log = HI_FALSE;
    hi_venc_frm_rate_type type;

    D_VENC_GET_CHN(chn_id, enc_para);
    D_VENC_CHECK_CHN(chn_id);

    if (argc != 2) { /* 2 is param num */
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&g_send_frame_lock[chn_id], &flags);

    if (venc_drv_check_chn_state_safe(chn_id) == HI_TRUE) {
        if (!strcmp(argv[1], "auto")) {
            type = HI_VENC_FRMRATE_AUTO;
        } else if (!strcmp(argv[1], "stream")) {
            type = HI_VENC_FRMRATE_STREAM;
        } else if (!strcmp(argv[1], "user")) {
            type = HI_VENC_FRMRATE_USER;
        } else {
            type = HI_VENC_FRMRATE_BUTT;
            err_log = HI_TRUE;
        }

        if (type != HI_VENC_FRMRATE_BUTT) {
            if (venc_drv_set_in_frm_rate_type(enc_para, type) == HI_SUCCESS) {
                osal_printk("set channel (%d) input_frm_rate type to %s success.\n", chn_id, argv[1]);
            } else {
                osal_printk("set channel (%d) input_frm_rate type to %s failed.\n", chn_id, argv[1]);
            }
        }
    }
    osal_spin_unlock_irqrestore(&g_send_frame_lock[chn_id], &flags);

    if (err_log) {
        venc_drv_proc_help();
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static osal_proc_cmd g_venc_proc_cmd[] = {
    { "save_yuv", venc_drv_proc_cmd_save_yuv},
    { "save_stream", venc_drv_proc_cmd_save_stream},
    { "clk_gate_en", venc_drv_proc_cmd_clk_gate_en},
    { "low_pow_en", venc_drv_proc_cmd_low_pow_en},
    { "rcratioprint", venc_drv_proc_write_print_rc_ratio},
    { "save_file_path", venc_drv_proc_cmd_save_file_path},
    { "rc_mode", venc_drv_proc_write_set_rc_mode},
    { "in_frmrate_type", venc_drv_proc_write_set_input_frmrate_type},
    { "request_i", venc_drv_proc_write_request_i}
};


#endif
hi_s32 venc_drv_proc_add(vedu_efl_enc_para *venc, hi_u32 chn_id)   /* */
{
#ifdef HI_PROC_SUPPORT
    hi_s32 ret;
    osal_proc_entry *proc_item = NULL;
    hi_char proc_name[12]; /* 12 is size of  proc_name */

    if (NULL == venc) {
        return HI_FAILURE;
    }

    ret = snprintf_s(proc_name, sizeof(proc_name), sizeof(proc_name), "venc%02x", chn_id);
    if (ret < 0) {
        HI_ERR_VENC("snprintf_s check  err! ret = %d\n", ret);
        return HI_FAILURE;
    }

    proc_item = osal_proc_add(proc_name, sizeof(proc_name));
    if (proc_item == HI_NULL) {
        HI_ERR_VENC("VENC add proc failed!\n");
        return HI_FAILURE;
    }

    proc_item->private = (hi_void *)venc;
    proc_item->read = venc_drv_proc_read;
    proc_item->cmd_cnt = sizeof(g_venc_proc_cmd) / sizeof(osal_proc_cmd);
    proc_item->cmd_list = g_venc_proc_cmd;

#endif
    return HI_SUCCESS;
}

hi_void venc_drv_proc_del(vedu_efl_enc_para *venc, hi_u32 chn_id)
{
#ifdef HI_PROC_SUPPORT
    hi_s32 ret;
    hi_char proc_name[12]; /* 12 is size of  proc_name */

    if (NULL == venc) {
        return;
    }

    ret = snprintf_s(proc_name, sizeof(proc_name), sizeof(proc_name), "venc%02x", chn_id);
    if (ret < 0) {
        HI_ERR_VENC("snprintf_s check  err! ret = %d\n", ret);
        return;
    }

    osal_proc_remove(proc_name, sizeof(proc_name));

#endif
}

#define DBG_YUV_INFO_PATH_LEN 130
typedef struct {
    struct file *fp;
    hi_char path[DBG_YUV_INFO_PATH_LEN];
    hi_u32 buf_size;
    hi_u8 *pu8_ydata;
    hi_u8 *pu8_udata;
    hi_u8 *pu8_vdata;
    hi_u8 *start_vir_addr;
} dbg_yuv_info;

static hi_s32 config_dbg_yuv_info(dbg_yuv_info *yuv_info, hi_drv_video_frame *frame, hi_char *file_name)
{
    hi_s32 ret;

    yuv_info->buf_size = frame->buf_addr[0].stride_y * frame->height * 3 / 2; /* 2 3 is calc height */

    yuv_info->start_vir_addr = (hi_u8 *)frame->buf_addr[0].vir_addr_y;
    if (yuv_info->start_vir_addr == HI_NULL) {
        HI_ERR_VENC("address is not valid!\n");
        return HI_FAILURE;
    }

    ret = snprintf_s(yuv_info->path, DBG_YUV_INFO_PATH_LEN,
        DBG_YUV_INFO_PATH_LEN, "%s/%s", g_venc_save_path, file_name);
    if (ret < 0) {
        HI_ERR_VENC("snprintf_s check  err! ret = %d\n", ret);
        return HI_FAILURE;
    }

    yuv_info->fp = venc_drv_osal_fopen(yuv_info->path, O_RDWR | O_CREAT | O_APPEND, 0);

    if (yuv_info->fp == HI_NULL) {
        HI_ERR_VENC("open file '%s' fail!\n", yuv_info->path);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void copy_uv_data(dbg_yuv_info *yuv_info, hi_drv_video_frame *frame, hi_u8 *ptr)
{
    hi_u32 i, j;

    for (i = 0; i < frame->height / 2; i++) { /* 2  is calc height */
        for (j = 0; j < frame->width / 2; j++) { /* 2  is calc width */
            if (frame->pixel_format == HI_DRV_PIXEL_FMT_NV21) {
                yuv_info->pu8_vdata[i * frame->width / 2 + j] = ptr[2 * j]; /* 2  is calc width */
                yuv_info->pu8_udata[i * frame->width / 2 + j] = ptr[2 * j + 1]; /* 2  is calc width */
            } else if (frame->pixel_format == HI_DRV_PIXEL_FMT_NV12) {
                yuv_info->pu8_udata[i * frame->width / 2 + j] = ptr[2 * j]; /* 2  is calc width */
                yuv_info->pu8_vdata[i * frame->width / 2 + j] = ptr[2 * j + 1]; /* 2  is calc width */
            } else {
                HI_ERR_VENC("other pix formet= %d\n", frame->pixel_format);
            }
        }
        ptr += frame->buf_addr[0].stride_c;
    }
}

static hi_s32 put_uv_data_together(dbg_yuv_info *yuv_info, hi_drv_video_frame *frame, hi_u8 *ptr)
{
    hi_u32 i;

    /* put U or V data together */
    if (frame->pixel_format == HI_DRV_PIXEL_FMT_NV21 || frame->pixel_format == HI_DRV_PIXEL_FMT_NV12) {
        copy_uv_data(yuv_info, frame, ptr);
    } else if (frame->pixel_format == HI_DRV_PIXEL_FMT_YUV420P) {
        for (i = 0; i < frame->height / 2; i++) { /* 2  is calc height */
            memcpy_s(&yuv_info->pu8_udata[i * frame->width / 2], sizeof(hi_u8) * frame->width / 2, /* 2 is calc width */
                ptr, sizeof(hi_u8) * frame->width / 2); /* 2  is calc width */
            ptr += frame->buf_addr[0].stride_c;
        }
        ptr = yuv_info->start_vir_addr + (frame->buf_addr[0].cr_offset - frame->buf_addr[0].y_offset);

        if (ptr >= (yuv_info->start_vir_addr + yuv_info->buf_size)) {
            HI_ERR_VENC("UV vir_addr(%p) maybe invalid!\n", ptr);
            return HI_FAILURE;
        }

        for (i = 0; i < frame->height / 2; i++) { /* 2  is calc height */
            memcpy_s(&yuv_info->pu8_vdata[i * frame->width / 2], sizeof(hi_u8) * frame->width / 2, /* 2 is calc width */
                ptr, sizeof(hi_u8) * frame->width / 2); /* 2  is calc width */
            ptr += frame->buf_addr[0].stride_c;
        }
    } else {
        HI_ERR_VENC("other pix formet= %d, error!!!!\n", frame->pixel_format);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 write_yuv_to_fp(dbg_yuv_info *yuv_info, hi_drv_video_frame *frame)
{
    hi_u8 *ptr = yuv_info->start_vir_addr;
    hi_u32 i;
    hi_u32 len;

    /* write Y data */
    for (i = 0; i < frame->height; i++) {
        memcpy_s(yuv_info->pu8_ydata, sizeof(hi_u8) * frame->buf_addr[0].stride_y, ptr,
            sizeof(hi_u8) * frame->buf_addr[0].stride_y);

        if (frame->width != venc_drv_osal_fwrite(yuv_info->pu8_ydata, frame->width, yuv_info->fp)) {
            HI_ERR_VENC("fwrite fail!\n");
        }

        ptr += frame->buf_addr[0].stride_y;
    }

    ptr = yuv_info->start_vir_addr + (frame->buf_addr[0].c_offset - frame->buf_addr[0].y_offset);

    if (ptr >= (yuv_info->start_vir_addr + yuv_info->buf_size)) {
        HI_ERR_VENC("UV vir_addr(%p) maybe invalid!\n", ptr);
        return HI_FAILURE;
    }

    if (put_uv_data_together(yuv_info, frame, ptr) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    /* write U data */
    len = frame->width * frame->height / 2 / 2; /* 2  is calc height */
    if (len != venc_drv_osal_fwrite(yuv_info->pu8_udata, len, yuv_info->fp)) {
        HI_ERR_VENC("fwrite fail!\n");
    }

    /* write V data */
    if (len != venc_drv_osal_fwrite(yuv_info->pu8_vdata, len, yuv_info->fp)) {
        HI_ERR_VENC("fwrite fail!\n");
    }

    return HI_SUCCESS;
}

hi_void venc_drv_dbg_write_yuv(hi_drv_video_frame *frame, hi_char *file_name, hi_u32 file_name_length)
{
#ifdef HI_PROC_SUPPORT
    dbg_yuv_info yuv_info;

    memset_s(&yuv_info, sizeof(dbg_yuv_info), 0, sizeof(dbg_yuv_info));

    if (frame == NULL || file_name == NULL || file_name_length > 64) { /* 64 is min length */
        return;
    }

    if ((frame->pixel_format == HI_DRV_PIXEL_FMT_YUYV) || (frame->pixel_format == HI_DRV_PIXEL_FMT_YVYU) ||
        (frame->pixel_format == HI_DRV_PIXEL_FMT_UYVY)) {
        HI_ERR_VENC("not support format package422!\n");
        return;
    }

    yuv_info.pu8_udata = HI_VMALLOC(HI_ID_VENC, frame->width * frame->height / 2 / 2); /* 2  is calc height */
    yuv_info.pu8_vdata = HI_VMALLOC(HI_ID_VENC, frame->width * frame->height / 2 / 2); /* 2  is calc height */
    yuv_info.pu8_ydata = HI_VMALLOC(HI_ID_VENC, frame->buf_addr[0].stride_y);

    if (yuv_info.pu8_udata == HI_NULL || yuv_info.pu8_vdata == HI_NULL || yuv_info.pu8_ydata == HI_NULL) {
        return;
    }

    if (config_dbg_yuv_info(&yuv_info, frame, file_name) != HI_SUCCESS) {
        goto exit1;
    }

    if (write_yuv_to_fp(&yuv_info, frame) != HI_SUCCESS) {
        goto exit2;
    }

    HI_INFO_VENC("2d image has been saved to '%s' W=%d H=%d format=%d \n",
        yuv_info.path, frame->width, frame->height, frame->pixel_format);

exit2:
    if (yuv_info.fp != NULL) {
        venc_drv_osal_fclose(yuv_info.fp);
    }
exit1:
    if (yuv_info.pu8_udata != HI_NULL) {
        HI_VFREE(HI_ID_VENC, yuv_info.pu8_udata);
    }
    if (yuv_info.pu8_vdata != HI_NULL) {
        HI_VFREE(HI_ID_VENC, yuv_info.pu8_vdata);
    }
    if (yuv_info.pu8_ydata != HI_NULL) {
        HI_VFREE(HI_ID_VENC, yuv_info.pu8_ydata);
    }
#endif
    return;
}

static hi_s32 config_dbg_yuv_info_omx(dbg_yuv_info *yuv_info, hi_venc_user_buf *frame_omx, hi_char *file_name)
{
    hi_s32 ret;

#ifdef HI_SMMU_SUPPORT
    yuv_info->start_vir_addr = (hi_u8 *)(uintptr_t)(frame_omx->buffer_addr_phy + frame_omx->virt_to_phy_offset);
#else
    yuv_info->start_vir_addr = (unsigned char*)phys_to_virt(frame_omx->buffer_addr_phy);
#endif
    if (yuv_info->start_vir_addr == HI_NULL) {
        HI_ERR_VENC("address is not valid!\n");
        return HI_FAILURE;
    }

    ret = snprintf_s(yuv_info->path, DBG_YUV_INFO_PATH_LEN, DBG_YUV_INFO_PATH_LEN,
        "%s/%s", g_venc_save_path, file_name);
    if (ret < 0) {
        HI_ERR_VENC("snprintf_s check  err! ret = %d\n", ret);
        return HI_FAILURE;
    }

    yuv_info->fp = venc_drv_osal_fopen(yuv_info->path, O_RDWR | O_CREAT | O_APPEND, 0);
    if (yuv_info->fp == HI_NULL) {
        HI_ERR_VENC("open file '%s' fail!\n", yuv_info->path);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void copy_uv_data_omx(dbg_yuv_info *yuv_info, hi_venc_user_buf *frame_omx, hi_u8 *ptr)
{
    hi_u32 i, j;

    for (i = 0; i < frame_omx->pic_height / 2; i++) { /* 2  is calc height */
        for (j = 0; j < frame_omx->pic_width / 2; j++) { /* 2  is calc weight */
            if (frame_omx->package_sel == VENC_V_U) {
                yuv_info->pu8_vdata[i * frame_omx->pic_width / 2 + j] = ptr[2 * j]; /* 2  is calc weight */
                yuv_info->pu8_udata[i * frame_omx->pic_width / 2 + j] = ptr[2 * j + 1]; /* 2  is calc weight */
            } else if (frame_omx->package_sel == VENC_U_V) {
                yuv_info->pu8_udata[i * frame_omx->pic_width / 2 + j] = ptr[2 * j]; /* 2  is calc weight */
                yuv_info->pu8_vdata[i * frame_omx->pic_width / 2 + j] = ptr[2 * j + 1]; /* 2  is calc weight */
            } else {
                HI_ERR_VENC("other pix formet= %d\n", frame_omx->package_sel);
            }
        }

        ptr += frame_omx->stride_c;
    }
}

static hi_s32 put_uv_data_together_omx(dbg_yuv_info *yuv_info, hi_venc_user_buf *frame_omx, hi_u8 *ptr)
{
    hi_u32 i;

    /* put U or V data together */
    if ((frame_omx->store_type == VENC_STORE_SEMIPLANNAR) && (frame_omx->sample_type == VENC_YUV_420)) {
        copy_uv_data_omx(yuv_info, frame_omx, ptr);
    } else if ((frame_omx->store_type == VENC_STORE_PLANNAR) && (frame_omx->sample_type == VENC_YUV_420)) {
        for (i = 0; i < frame_omx->pic_height / 2; i++) { /* 2  is calc height */
            memcpy_s(&yuv_info->pu8_udata[ i * frame_omx->pic_width / 2], /* 2  is calc width */
                sizeof(hi_u8) * frame_omx->pic_width / 2, ptr, /* 2  is calc width */
                sizeof(hi_u8) * frame_omx->pic_width / 2); /* 2  is calc width */
            ptr += frame_omx->stride_c;
        }

#ifdef HI_SMMU_SUPPORT
        ptr = yuv_info->start_vir_addr + (frame_omx->offset_y_cr);
#else
        ptr = (unsigned char*)phys_to_virt(frame_omx->buffer_addr_phy) + (frame_omx->offset_y_cr);
#endif
        if (ptr >= (yuv_info->start_vir_addr + yuv_info->buf_size)) {
            HI_ERR_VENC("UV vir_addr(%p) maybe invalid!\n", ptr);
            return HI_FAILURE;
        }

        for (i = 0; i < frame_omx->pic_height / 2; i++) { /* 2  is calc height */
            memcpy_s(&yuv_info->pu8_vdata[i * frame_omx->pic_width / 2],  /* 2  is calc width */
                sizeof(hi_u8) * frame_omx->pic_width / 2, /* 2  is calc width */
                ptr, sizeof(hi_u8) * frame_omx->pic_width / 2); /* 2  is calc width */
            ptr += frame_omx->stride_c;
        }
    }   else {
        HI_ERR_VENC("other pix formet= %d, error!!!!\n", frame_omx->store_type);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 write_yuv_to_fp_omx(dbg_yuv_info *yuv_info, hi_venc_user_buf *frame_omx)
{
    hi_u8 *ptr = yuv_info->start_vir_addr;
    hi_u32 i;
    hi_u32 len;

    /* write Y data */
    for (i = 0; i < frame_omx->pic_height; i++) {
        memcpy_s(yuv_info->pu8_ydata, sizeof(hi_u8) * frame_omx->stride_y, ptr, sizeof(hi_u8) * frame_omx->stride_y);

        if (frame_omx->pic_width != venc_drv_osal_fwrite(yuv_info->pu8_ydata, frame_omx->pic_width, yuv_info->fp)) {
            HI_ERR_VENC("fwrite fail!\n");
        }

        ptr += frame_omx->stride_y;
    }
#ifdef HI_SMMU_SUPPORT
    ptr = yuv_info->start_vir_addr + (frame_omx->offset_y_c);
#else
    ptr = (unsigned char*)phys_to_virt(frame_omx->buffer_addr_phy) + (frame_omx->offset_y_c);
#endif
    if (ptr >= (yuv_info->start_vir_addr + yuv_info->buf_size)) {
        HI_ERR_VENC("UV vir_addr(%p) maybe invalid!\n", ptr);
        return HI_FAILURE;
    }

    if (put_uv_data_together_omx(yuv_info, frame_omx, ptr) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    /* write U data */
    len = frame_omx->pic_width * frame_omx->pic_height / 2 / 2; /* 2  is calc height */
    if (len != venc_drv_osal_fwrite(yuv_info->pu8_udata, len, yuv_info->fp)) {
        HI_ERR_VENC("fwrite fail!\n");
    }
    /* write V data */
    if (len != venc_drv_osal_fwrite(yuv_info->pu8_vdata, len, yuv_info->fp)) {
        HI_ERR_VENC("fwrite fail!\n");
    }

    return HI_SUCCESS;
}

hi_void venc_drv_dbg_write_yuv_omx(hi_venc_user_buf *frame_omx, hi_char* file_name, hi_u32 file_name_length)
{
#ifdef HI_PROC_SUPPORT
    dbg_yuv_info yuv_info;

    if (frame_omx == NULL || file_name == NULL || file_name_length > 64) { /* 64 is min length */
        return;
    }

    if (frame_omx->store_type == VENC_STORE_PACKAGE) {
        HI_ERR_VENC("not support format package422!\n");
        return;
    }

    yuv_info.buf_size = frame_omx->stride_y * frame_omx->pic_height * 3 / 2; /* 2 3 is calc height */

    yuv_info.pu8_udata = HI_VMALLOC(HI_ID_VENC,
        frame_omx->pic_width * frame_omx->pic_height / 2 / 2); /* 2 is calc height */
    yuv_info.pu8_vdata = HI_VMALLOC(HI_ID_VENC,
        frame_omx->pic_width * frame_omx->pic_height / 2 / 2); /* 2 3 is calc height */
    yuv_info.pu8_ydata = HI_VMALLOC(HI_ID_VENC, frame_omx->stride_y);

    if (yuv_info.pu8_udata == HI_NULL || yuv_info.pu8_vdata == HI_NULL || yuv_info.pu8_ydata == HI_NULL) {
        return;
    }

    if (config_dbg_yuv_info_omx(&yuv_info, frame_omx, file_name) != HI_SUCCESS) {
        goto exit0;
    }

    if (write_yuv_to_fp_omx(&yuv_info, frame_omx) != HI_SUCCESS) {
        goto exit1;
    }

    HI_INFO_VENC("2d image has been saved to '%s' W=%d H=%d format=%d,%d,%d \n",
        yuv_info.path, frame_omx->pic_width, frame_omx->pic_height, frame_omx->store_type,
        frame_omx->sample_type, frame_omx->package_sel);

exit1:
    if (yuv_info.fp != HI_NULL) {
        venc_drv_osal_fclose(yuv_info.fp);
    }

exit0:
    if (yuv_info.pu8_udata != HI_NULL) {
        HI_VFREE(HI_ID_VENC, yuv_info.pu8_udata);
    }
    if (yuv_info.pu8_vdata != HI_NULL) {
        HI_VFREE(HI_ID_VENC, yuv_info.pu8_vdata);
    }
    if (yuv_info.pu8_ydata != HI_NULL) {
        HI_VFREE(HI_ID_VENC, yuv_info.pu8_ydata);
    }

    return;
#endif
}

static hi_s32 get_stream_status_info(vedu_efl_nalu* nalu, vedu_efl_enc_para* ph_venc_chn, hi_u32 ve_chn,
                                     hi_bool *tag_first_time_save, hi_bool *tag_first_ps_get)
{
    vedu_state_info stat_info;
    hi_u32 skip_frm_num;
    static hi_u32 save_frame_start_count = 0;
    hi_s32 ret;

    ret = venc_drv_efl_query_stat_info(g_venc_chn[ve_chn].venc_handle, &stat_info);

    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("vedu_efl_query_stat_info failed.\n");

        return HI_FAILURE;
    }

    skip_frm_num = stat_info.quick_encode_skip + stat_info.same_pts_skip + stat_info.err_cfg_skip
                        + stat_info.frm_rc_ctrl_skip + stat_info.too_few_buffer_skip;

    /* request one I frame and record save_frame_start_count to compare with g_venc_save_frame_count
    when save file firstly */
    if (*tag_first_time_save == HI_TRUE) {
        if (!is_parameter_set(nalu, ph_venc_chn->protocol)) {
            HI_ERR_VENC(" request one I frame\n");
            venc_drv_efl_request_i_frame(ph_venc_chn);
            ph_venc_chn->prepend_sps_pps_enable = 1;
            ph_venc_chn->i_frm_insert_by_save_strm = 1;
            *tag_first_time_save = HI_FALSE;

            save_frame_start_count = stat_info.get_frame_num_ok - skip_frm_num;

            return HI_SUCCESS;
        } else {
            *tag_first_time_save = HI_FALSE;
        }
    }

    /* compare with frame_mode_count each time */
    if ((g_venc_chn[ve_chn].proc_write.frame_mode_run == HI_TRUE)
        && (stat_info.get_frame_num_ok - skip_frm_num - save_frame_start_count) >
        g_venc_chn[ve_chn].proc_write.frame_mode_count) {
        /* time to stop save file */
        g_venc_chn[ve_chn].proc_write.frame_mode_run = HI_FALSE;

        return HI_SUCCESS;
    }

    if (!*tag_first_ps_get) {
        if (is_parameter_set(nalu, ph_venc_chn->protocol)) {
            HI_INFO_VENC("get first parameter set!\n");
            *tag_first_ps_get = HI_TRUE;
        } else {
            save_frame_start_count = stat_info.get_frame_num_ok - skip_frm_num;
            HI_INFO_VENC("no first parameter set!\n");
            return HI_SUCCESS;
        }
    }

    return HI_SUCCESS;
}

hi_s32 venc_drv_dbg_save_stream(vedu_efl_nalu* nalu, vedu_efl_enc_para* ph_venc_chn)
{
#ifdef HI_PROC_SUPPORT
    hi_s32 ret = HI_FAILURE;
    static hi_bool tag_first_time_save = HI_TRUE;
    static hi_bool tag_first_ps_get    = HI_FALSE;
    hi_u32 ve_chn = HI_VENC_MAX_CHN;

    D_VENC_GET_CHN(ve_chn, ph_venc_chn);
    if (ve_chn >= HI_VENC_MAX_CHN) {
        return HI_FAILURE;
    }

    D_VENC_CHECK_PTR(nalu);

    if ((g_venc_chn[ve_chn].proc_write.time_mode_run == HI_TRUE) || (g_venc_chn[ve_chn].proc_write.frame_mode_run
        == HI_TRUE)) {
        ret = get_stream_status_info(nalu, ph_venc_chn, ve_chn, &tag_first_time_save, &tag_first_ps_get);
        if (ret != HI_SUCCESS) {
            HI_ERR_VENC("get_stream_status_info failed.\n");
            return HI_FAILURE;
        }

        if (nalu->slc_len[0] > 0) {
            ret = venc_drv_osal_fwrite((char*)nalu->virt_addr[0], nalu->slc_len[0],
                g_venc_chn[ve_chn].proc_write.fp_save_file);
            if (ret != nalu->slc_len[0]) {
                HI_ERR_VENC("vedu_osal_fwrite[0] failed.\n");
                g_venc_chn[ve_chn].proc_write.time_mode_run  = HI_FALSE;
                g_venc_chn[ve_chn].proc_write.frame_mode_run = HI_FALSE;

                return HI_FAILURE;
            }
        }

        if (nalu->slc_len[1] > 0) {
            ret = venc_drv_osal_fwrite((char*)nalu->virt_addr[1], nalu->slc_len[1],
                g_venc_chn[ve_chn].proc_write.fp_save_file);
            if (ret != nalu->slc_len[1]) {
                HI_ERR_VENC("vedu_osal_fwrite failed.\n");
                g_venc_chn[ve_chn].proc_write.time_mode_run  = HI_FALSE;
                g_venc_chn[ve_chn].proc_write.frame_mode_run = HI_FALSE;

                return HI_FAILURE;
            }
        }
    } else {
        tag_first_time_save = HI_TRUE;
        tag_first_ps_get    = HI_FALSE;
    }
#endif
    return HI_SUCCESS;
}
