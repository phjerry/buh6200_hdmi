/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: sync interface definition
 * Author: Hisilicon multimedia software group
 * Create: 2012-12-22
 */

#include "drv_sync_intf.h"

#ifdef SYNC_NOT_SUPPORT_OSAL
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#endif

#include "hi_debug.h"
#include "hi_drv_sys.h"
#include "hi_drv_dev.h"
#include "drv_sync_ext.h"
#include "drv_sync.h"
#include "drv_sync_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef HI_SYNC_PROC_SUPPORT

#define CHECK_SYNC_PROC_CMD(argc, argv, ctx) do { \
    if ((argc) != 3 || (argv) == NULL || (ctx) == NULL || \
        strncmp(argv[1], "=", sizeof("=")) != 0) { \
        osal_printk("unspport cmd, argc = %u, argv = %p, ctx = %p\n", (argc), (argv), (ctx)); \
        return sync_proc_cmd_help(argc, argv, ctx); \
    } \
} while (0)

static hi_u8 *g_sync_status_string[SYNC_STATUS_MAX] = {
    "STOP",
    "PLAY",
    "TPLAY",
    "PAUSE",
};

static hi_u8 *g_sync_ref_string[SYNC_REF_MAX] = {
    "NONE",
    "AUDIO",
    "VIDEO",
    "PCR",
    "SCR",
};

static hi_u8 *g_sync_chn_string[SYNC_CHAN_MAX] = {
    "VID",
    "AUD",
    "PCR",
    "SCR",
    "EXT",
};

#ifdef SYNC_DEBUG_DETAIL
static void sync_print_dbg_info(sync_context *ctx, void *para)
{
    hi_char *proc_mode = HI_NULL;
    struct list_head *pos = HI_NULL;
    struct list_head *n = HI_NULL;
    sync_dbg_info_status *dbg_info = HI_NULL;

    if (para == HI_NULL) {
        return;
    }

    if (ctx->dbg_list_max_len == 0) {
        return;
    }

    osal_proc_print(para, "%8s |"
        "%10s |"
        "%10s |%10s |"
        "%10s |"
        "%10s |%10s |"
        "%10s |%10s |"
        "%10s |%10s |"
        "%8s |"
        "%8s |"
        "%10s |%10s |"
        "%10s |%10s |"
        "%8s |"
        "%8s |"
        "%10s |\n",
        "proc_type",
        "sys_time",
        "vid_pts",     "aud_pts",
        "vid_aud_diff",
        "vid_local_time", "aud_local_time",
        "pcr_local_time", "pcr_last",
        "vid_pcr_diff", "aud_pcr_diff",
        "vid_delay",
        "aud_buf",
        "vid_frame_time", "aud_frame_time",
        "vid_src_pts",  "aud_scr_pts",
        "pcr_delta",
        "disp_time",
        "index");

    list_for_each_safe(pos, n, &ctx->dbg_info_list_head)
    {
        dbg_info = list_entry(pos, sync_dbg_info_status, list);

        switch (dbg_info->proc_type) {
            case SYNC_PROC_DISCARD:
                proc_mode = "DISCARD";
                break;
            case SYNC_PROC_REPEAT:
                proc_mode = "REPEAT";
                break;
            case SYNC_PROC_PLAY:
                proc_mode = "PLAY";
                break;
            case SYNC_PROC_QUICKOUTPUT:
                proc_mode = "QUICKOUT";
                break;
            case SYNC_PROC_TPLAY:
                proc_mode = "TPLAY";
                break;
            case SYNC_PROC_CONTINUE:
                proc_mode = "CONTINUE";
                break;
            case SYNC_PROC_BLOCK:
                proc_mode = "BLOCK";
                break;
            default :
                proc_mode = "UNKNOWN";
                break;
        }

        osal_proc_print(para, "%8s |"
                      "%10d |"
                      "%10d |%10d |"
                      "%10d |"
                      "%10d |%10d |"
                      "%10d |%10d |"
                      "%10d |%10d |"
                      "%8d |"
                      "%8d |"
                      "%10d |%10d |"
                      "%10d |%10d |"
                      "%8d |"
                      "%8d |"
                      "%10d |\n",
            proc_mode,
            dbg_info->system_time,
            dbg_info->vid_pts, dbg_info->aud_pts,
            dbg_info->vid_aud_diff,
            dbg_info->vid_local_time, dbg_info->aud_local_time,
            dbg_info->pcr_local_time, dbg_info->pcr_last,
            dbg_info->vid_pcr_diff,   dbg_info->aud_pcr_diff,
            dbg_info->vid_delay_time,
            dbg_info->aud_buf_time,
            dbg_info->vid_frame_time, dbg_info->aud_frame_time,
            dbg_info->vid_src_pts,    dbg_info->aud_src_pts,
            dbg_info->pcr_delta_time,
            dbg_info->disp_time,
            dbg_info->idx);
    }

    return;
}
#endif

static hi_s32 sync_proc_read(void *seqfile, void *private)
{
    sync_context    *ctx = HI_NULL;
    hi_u32          pcr_local_time = 0;
    hi_u32          aud_local_time = 0;
    hi_u32          vid_local_time = 0;
    hi_u32          sys_time = 0;

#ifdef HI_AVPLAY_SCR_SUPPORT
    hi_u32          scr_local_time;
#endif

    ctx = private;
    if (ctx == NULL) {
        osal_proc_print(seqfile, "ctx is NULL\n");
        return HI_SUCCESS;
    }

    pcr_local_time = sync_get_local_time(ctx, SYNC_CHAN_PCR);
    aud_local_time = sync_get_local_time(ctx, SYNC_CHAN_AUD);
    vid_local_time = sync_get_local_time(ctx, SYNC_CHAN_VID);
    sys_time = sync_get_sys_time();

#ifdef HI_AVPLAY_SCR_SUPPORT
    scr_local_time = sync_get_local_time(ctx, SYNC_CHAN_SCR);
#endif

    osal_proc_print(seqfile, "------------------------Hisilicon SYNC Out Info--------------------------\n");
    osal_proc_print(seqfile, "__________Hisilicon SYNC ATTR__________|__________Hisilicon PCR____________\n");
    osal_proc_print(seqfile, "is_print                       :%-10d|cur_status                :%s\n"
                     "sync_ref_mode                  :%-10s|presync_start_sys_time    :%d\n"
                     "sync_start.vid_plus_time       :%-10d|presync_end_sys_time      :%d\n"
                     "sync_start.vid_negative_time   :%-10d|is_presync_finish         :%d\n"
                     "sync_start.is_smooth_play      :%-10d|buf_fund_end_sys_time     :%d\n"
                     "sync_novel.vid_plus_time       :%-10d|is_buf_fund_finish        :%d\n"
                     "sync_novel.vid_negative_time   :%-10d|presync_target            :%s\n"
                     "sync_novel.is_smooth_play      :%-10d|presync_target_time       :%d\n"
                     "vid_pts_adjust                 :%-10lld|is_pcr_first_come         :%d\n"
                     "aud_pts_adjust                 :%-10lld|pcr_first_sys_time        :%d\n"
                     "presync_timeout_ms             :%-10d|pcr_first                 :%d\n"
                     "is_quick_output                :%-10d|pcr_last                  :%d\n"
                     "vid_first_dec_pts              :%-10d|pcr_local_time            :%d\n"
                     "vid_second_dec_pts             :%-10d|pcr_adjust_type           :%d\n"
                     "vid_first_valid_pts            :%-10d|is_pcr_aud_sync_ok        :%d\n"
                     "aud_first_valid_pts            :%-10d|is_pcr_vid_sync_ok        :%d\n",
        ctx->is_print,                              g_sync_status_string[ctx->cur_status],
        g_sync_ref_string[ctx->attr.sync_ref],      ctx->presync_start_sys_time,
        ctx->attr.start_region.vid_plus_time,       ctx->presync_end_sys_time,
        ctx->attr.start_region.vid_negative_time,   ctx->is_presync_finish,
        ctx->attr.start_region.smooth_play,         ctx->buf_fund_end_sys_time,
        ctx->attr.novel_region.vid_plus_time,       ctx->is_buf_fund_finish,
        ctx->attr.novel_region.vid_negative_time,   g_sync_chn_string[ctx->presync_target],
        ctx->attr.novel_region.smooth_play,         ctx->presync_target_time,
        ctx->attr.vid_pts_adjust,                   ctx->pcr_sync_info.is_pcr_first_come,
        ctx->attr.aud_pts_adjust,                   ctx->pcr_sync_info.pcr_first_sys_time,
        ctx->attr.pre_sync_timeout,                 ctx->pcr_sync_info.pcr_first,
        ctx->attr.quick_output,                     ctx->pcr_sync_info.pcr_last,
        ctx->vid_first_dec_pts,                     pcr_local_time,
        ctx->vid_second_dec_pts,                    ctx->pcr_sync_info.pcr_adjust_type,
        ctx->vid_first_valid_pts,                   ctx->pcr_sync_info.is_pcr_aud_sync_ok,
        ctx->aud_first_valid_pts,                   ctx->pcr_sync_info.is_pcr_vid_sync_ok);

    osal_proc_print(seqfile, "__________Hisilicon VID________________|__________Hisilicon AUD____________\n");
    osal_proc_print(seqfile, "is_vid_first_come              :%-10d|is_aud_first_come           :%d\n"
                     "vid_first_sys_time             :%-10d|aud_first_sys_time          :%d\n"
                     "vid_first_pts                  :%-10d|aud_first_pts               :%d\n"
                     "vid_last_pts                   :%-10d|aud_last_pts                :%d\n"
                     "is_vid_presync_target_init     :%-10d|is_aud_presync_target_init  :%d\n"
                     "vid_presync_target_time        :%-10d|aud_presync_target_time     :%d\n"
                     "is_vid_first_play              :%-10d|is_aud_first_play           :%d\n"
                     "vid_first_play_time            :%-10d|aud_first_play_time         :%d\n"
                     "vid_buf_state                  :%-10d|aud_buf_state               :%d\n"
                     "vid_buf_percent                :%-10d|aud_buf_percent             :%d\n"
                     "vid_buf_time                   :%-10lld|aud_buf_time                :%lld\n"
                     "vid_local_time                 :%-10d|aud_local_time              :%d\n"
                     "vid_pcr_diff                   :%-10d|aud_pcr_diff                :%d\n"
                     "vid_aud_diff                   :%-10d|aud_track_delay             :%lld\n"
                     "is_vid_discard                 :%-10d|aud_last_track_delay        :%d\n"
                     "is_vid_sync_adjust             :%-10d|aud_discard_cnt             :%d\n"
                     "vid_discard_cnt                :%-10d|aud_repeat_cnt              :%d\n"
                     "vid_repeat_cnt                 :%-10d|sys_time                    :%d\n",
                    ctx->is_vid_first_come,                    ctx->is_aud_first_come,
                    ctx->vid_first_sys_time,                   ctx->aud_first_sys_time,
                    ctx->vid_first_pts,                        ctx->aud_first_pts,
                    ctx->vid_last_pts,                         ctx->aud_last_pts,
                    ctx->is_vid_presync_target_init,           ctx->is_aud_presync_target_init,
                    ctx->vid_presync_target_time,              ctx->aud_presync_target_time,
                    ctx->is_vid_first_play,                    ctx->is_aud_first_play,
                    ctx->vid_first_play_time,                  ctx->aud_first_play_time,
                    ctx->cur_buf_status.vid_buf_state,         ctx->cur_buf_status.aud_buf_state,
                    ctx->cur_buf_status.vid_buf_percent,       ctx->cur_buf_status.aud_buf_percent,
                    ctx->cur_buf_status.vid_buf_time,          ctx->cur_buf_status.aud_buf_time,
                    vid_local_time,                            aud_local_time,
                    ctx->pcr_sync_info.vid_pcr_diff,           ctx->pcr_sync_info.aud_pcr_diff,
                    ctx->vid_aud_diff,                         ctx->aud_info.buf_time,
                    ctx->cur_buf_status.is_overflow_disc_frm,  ctx->aud_last_buf_time,
                    ctx->is_vid_sync_adjust,                   ctx->aud_discard_cnt,
                    ctx->vid_discard_cnt,                      ctx->aud_repeat_cnt,
                    ctx->vid_repeat_cnt,                       sys_time);

#ifdef HI_AVPLAY_SCR_SUPPORT
    osal_proc_print(seqfile, "__________Hisilicon SCR________________\n");
    osal_proc_print(seqfile, "is_scr_init             :%-10d\n"
                     "scr_first_sys_time      :%-10d\n"
                     "scr_first_local_time    :%-10d\n"
                     "scr_local_time          :%-10d\n"
                     "aud_scr_diff            :%-10d\n"
                     "vid_scr_diff            :%-10d\n",
                    ctx->is_scr_init,
                    ctx->scr_first_sys_time,
                    ctx->scr_first_local_time,
                    scr_local_time,
                    ctx->aud_scr_diff,
                    ctx->vid_scr_diff);
#endif

#ifdef SYNC_PCR_ADJUST_STC_SUPPORT
    if ((ctx->pcr_sync_info.pcr_adjust_type == SYNC_PCR_ADJUST_SCR) &&
        (ctx->attr.sync_ref == SYNC_REF_PCR)) {
        osal_proc_print(seqfile, "__________Hisilicon STC________________\n");
        osal_proc_print(seqfile, "is_stc_adjust_init         :%-10d\n"
                         "is_stc_adjust_fail        :%-10d\n"
                         "is_stc_adjust_refer       :%-10d\n"
                         "ppm_valid                 :%-10d\n",
            ctx->pcr_sync_info.stc_info.is_stc_adjust_init,
            ctx->pcr_sync_info.stc_info.is_stc_adjust_fail,
            ctx->pcr_sync_info.is_stc_adjust_refer,
            ctx->pcr_sync_info.ppm_info.valid);
    }
#endif

#ifdef SYNC_DEBUG_DETAIL
    if (ctx->is_show_dbg_detail == HI_TRUE)
    {
        osal_proc_print(seqfile, "____________________________SYNC Adjust Detail____________________________\n");
        sync_print_dbg_info(ctx, seqfile);
    }
#endif

    return HI_SUCCESS;
}

static void proc_cmd_help(void)
{
    osal_printk("echo is_print = 1|0 > /proc/msp/syncxx\n");
    osal_printk("echo sync_ref_mode   = audio|pcr|scr|none > proc/msp/syncxx\n");
    osal_printk("echo sync_start.vid_plus_time = xxx > /proc/msp/syncxx\n");
    osal_printk("echo sync_start.vid_negative_time = xxx > /proc/msp/syncxx\n");
    osal_printk("echo sync_start.is_smooth_play = true|false > /proc/msp/syncxx\n");
    osal_printk("echo sync_novel.vid_plus_time = xxx > /proc/msp/syncxx\n");
    osal_printk("echo sync_novel.vid_negative_time = xxx > /proc/msp/syncxx\n");
    osal_printk("echo sync_novel.is_smooth_play = true|false > /proc/msp/syncxx\n");
    osal_printk("echo presync_timeout_ms = xxx > /proc/msp/syncxx\n");
    osal_printk("echo is_quick_output = true|false > /proc/msp/syncxx\n");

#ifdef SYNC_DEBUG_DETAIL
    osal_printk("echo sync_log_depth = xxx > /proc/msp/syncxx\n");
    osal_printk("echo sync_log_detail = on|off > /proc/msp/syncxx\n");
#endif
}

static sync_ref_type cvt_sync_ref_mode_to_enum(hi_char *val)
{
    if (0 == strncmp(val, "none", strlen("none"))) {
        return SYNC_REF_NONE;
    } else if (0 == strncmp(val, "audio", strlen("audio"))) {
        return SYNC_REF_AUDIO;
    } else if (0 == strncmp(val, "pcr", strlen("pcr"))) {
        return SYNC_REF_PCR;
    } else {
        proc_cmd_help();
        return SYNC_REF_MAX;
    }
}

static hi_bool cvt_true_false_to_bool(hi_char *val)
{
    hi_bool result = HI_FALSE;

    if (0 == strncmp(val, "true", strlen("true"))) {
        result = HI_TRUE;
    } else if (0 == strncmp(val, "false", strlen("false"))) {
        result = HI_FALSE;
    }

    return result;
}

#ifdef SYNC_DEBUG_DETAIL
static hi_bool cvt_on_off_to_bool(hi_char *val)
{
    hi_bool result = HI_FALSE;

    if (0 == strncmp(val, "on", strlen("on"))) {
        result = HI_TRUE;
    } else if (0 == strncmp(val, "off", strlen("off"))) {
        result = HI_FALSE;
    }

    return result;
}
#endif

static int sync_proc_cmd_help(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    proc_cmd_help();
    return HI_SUCCESS;
}

static int sync_proc_cmd_is_print(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    sync_context *ctx = private;
    CHECK_SYNC_PROC_CMD(argc, argv, ctx);

    ctx->is_print = simple_strtol(argv[2], NULL, 10);
    return HI_SUCCESS;
}

static int sync_proc_cmd_sync_ref_mode(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    sync_context *ctx = private;
    CHECK_SYNC_PROC_CMD(argc, argv, ctx);

    ctx->attr.sync_ref = cvt_sync_ref_mode_to_enum(argv[2]);
    return HI_SUCCESS;
}

static int sync_proc_cmd_sync_start_plus(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    sync_context *ctx = private;
    CHECK_SYNC_PROC_CMD(argc, argv, ctx);

    ctx->attr.start_region.vid_plus_time = simple_strtol(argv[2], NULL, 10);
    return HI_SUCCESS;
}

static int sync_proc_cmd_sync_start_negative(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    sync_context *ctx = private;
    CHECK_SYNC_PROC_CMD(argc, argv, ctx);

    ctx->attr.start_region.vid_negative_time = simple_strtol(argv[2], NULL, 10);
    return HI_SUCCESS;
}

static int sync_proc_cmd_sync_start_smooth(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    sync_context *ctx = private;
    CHECK_SYNC_PROC_CMD(argc, argv, ctx);

    ctx->attr.start_region.smooth_play = cvt_true_false_to_bool(argv[2]);
    return HI_SUCCESS;
}

static int sync_proc_cmd_sync_novel_plus(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    sync_context *ctx = private;
    CHECK_SYNC_PROC_CMD(argc, argv, ctx);

    ctx->attr.novel_region.vid_plus_time = simple_strtol(argv[2], NULL, 10);
    return HI_SUCCESS;
}

static int sync_proc_cmd_sync_novel_negative(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    sync_context *ctx = private;
    CHECK_SYNC_PROC_CMD(argc, argv, ctx);

    ctx->attr.novel_region.vid_negative_time = simple_strtol(argv[2], NULL, 10);
    return HI_SUCCESS;
}

static int sync_proc_cmd_sync_novel_smooth(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    sync_context *ctx = private;
    CHECK_SYNC_PROC_CMD(argc, argv, ctx);

    ctx->attr.novel_region.smooth_play = cvt_true_false_to_bool(argv[2]);
    return HI_SUCCESS;
}

static int sync_proc_cmd_presync_timeout_ms(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    sync_context *ctx = private;
    CHECK_SYNC_PROC_CMD(argc, argv, ctx);

    ctx->attr.pre_sync_timeout = simple_strtol(argv[2], NULL, 10);
    return HI_SUCCESS;
}

static int sync_proc_cmd_is_quick_output(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    sync_context *ctx = private;
    CHECK_SYNC_PROC_CMD(argc, argv, ctx);

    ctx->attr.quick_output = cvt_true_false_to_bool(argv[2]);
    return HI_SUCCESS;
}

#ifdef SYNC_DEBUG_DETAIL
static int sync_proc_cmd_sync_log_depth(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    sync_context *ctx = private;
    CHECK_SYNC_PROC_CMD(argc, argv, ctx);

    ctx->is_show_dbg_detail = cvt_on_off_to_bool(argv[2]);
    return HI_SUCCESS;
}

static int sync_proc_cmd_sync_log_detail(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 list_size;
    sync_context *ctx = private;
    CHECK_SYNC_PROC_CMD(argc, argv, ctx);

    list_size = simple_strtol(argv[2], NULL, 10);
    sync_proc_debug_info(ctx, SYNC_DBG_INFO_LIST_SIZE, (void *)&list_size);
    return HI_SUCCESS;
}
#endif

static osal_proc_cmd g_sync_proc_cmd[] = {
    {"help",                            sync_proc_cmd_help},
    {"is_print",                        sync_proc_cmd_is_print},
    {"sync_ref_mode",                   sync_proc_cmd_sync_ref_mode},
    {"sync_start.vid_plus_time",        sync_proc_cmd_sync_start_plus},
    {"sync_start.vid_negative_time",    sync_proc_cmd_sync_start_negative},
    {"sync_start.is_smooth_play",       sync_proc_cmd_sync_start_smooth},
    {"sync_novel.vid_plus_time",        sync_proc_cmd_sync_novel_plus},
    {"sync_novel.vid_negative_time",    sync_proc_cmd_sync_novel_negative},
    {"sync_novel.is_smooth_play",       sync_proc_cmd_sync_novel_smooth},
    {"presync_timeout_ms",              sync_proc_cmd_presync_timeout_ms},
    {"is_quick_output",                 sync_proc_cmd_is_quick_output},
#ifdef SYNC_DEBUG_DETAIL
    {"sync_log_detail",                 sync_proc_cmd_sync_log_depth},
    {"sync_log_depth",                  sync_proc_cmd_sync_log_detail},
#endif
};

static sync_register_param g_sync_register_param = {
    .rd_proc = sync_proc_read,
    .cmd_list = g_sync_proc_cmd,
    .cmd_cnt = sizeof(g_sync_proc_cmd) / sizeof(osal_proc_cmd)
};
#endif

static hi_s32 sync_drv_cmd_create(hi_u32 cmd, void *arg, void *private_data)
{
    sync_create_param *param = (sync_create_param *)arg;
    return hi_drv_sync_create(&param->attr, &param->sync, private_data);
}

static hi_s32 sync_drv_cmd_destroy(hi_u32 cmd, void *arg, void *private_data)
{
    return hi_drv_sync_destroy(*(hi_handle *)arg);
}

static hi_s32 sync_drv_cmd_set_attr(hi_u32 cmd, void *arg, void *private_data)
{
    sync_drv_attr *attr = (sync_drv_attr *)arg;
    return hi_drv_sync_attr_set(attr->sync, &attr->attr);
}

static hi_s32 sync_drv_cmd_get_attr(hi_u32 cmd, void *arg, void *private_data)
{
    sync_drv_attr *attr = (sync_drv_attr *)arg;
    return hi_drv_sync_attr_get(attr->sync, &attr->attr);
}

static hi_s32 sync_drv_cmd_check_num(hi_u32 cmd, void *arg, void *private_data)
{
    return hi_drv_sync_check_num(arg, private_data);
}

static hi_s32 sync_drv_cmd_start_sync(hi_u32 cmd, void *arg, void *private_data)
{
    sync_drv_start_opt *opt = (sync_drv_start_opt *)arg;
    return hi_drv_sync_start(opt->sync, opt->chn);
}

static hi_s32 sync_drv_cmd_stop_sync(hi_u32 cmd, void *arg, void *private_data)
{
    sync_drv_stop_opt *opt = (sync_drv_stop_opt *)arg;
    return hi_drv_sync_stop(opt->sync, opt->chn);
}

static hi_s32 sync_drv_cmd_aud_judge(hi_u32 cmd, void *arg, void *private_data)
{
    sync_aud_judge_status *status = (sync_aud_judge_status *)arg;
    return hi_drv_sync_aud_judge(status->sync, &status->info, &status->opt);
}

static hi_s32 sync_drv_cmd_ad_aud_judge(hi_u32 cmd, void *arg, void *private_data)
{
    sync_aud_judge_status *status = (sync_aud_judge_status *)arg;
    return hi_drv_sync_ad_aud_judge(status->sync, &status->info, &status->opt);
}

static hi_s32 sync_drv_cmd_vid_judge(hi_u32 cmd, void *arg, void *private_data)
{
    sync_vid_judge_status *status = (sync_vid_judge_status *)arg;
    return hi_drv_sync_vid_judge(status->sync, &status->info, &status->opt);
}

static hi_s32 sync_drv_cmd_play_sync(hi_u32 cmd, void *arg, void *private_data)
{
    return hi_drv_sync_play(*(hi_handle *)arg);
}

static hi_s32 sync_drv_cmd_pause_sync(hi_u32 cmd, void *arg, void *private_data)
{
    return hi_drv_sync_pause(*(hi_handle *)arg);
}

static hi_s32 sync_drv_cmd_resume_sync(hi_u32 cmd, void *arg, void *private_data)
{
    return hi_drv_sync_resume(*(hi_handle *)arg);
}

static hi_s32 sync_drv_cmd_tplay_sync(hi_u32 cmd, void *arg, void *private_data)
{
    return hi_drv_sync_tplay(*(hi_handle *)arg);
}

static hi_s32 sync_drv_cmd_seek_sync(hi_u32 cmd, void *arg, void *private_data)
{
    sync_drv_seek_opt *opt = (sync_drv_seek_opt *)arg;
    return hi_drv_sync_seek(opt->sync, opt->seek_pts);
}

static hi_s32 sync_drv_cmd_new_event(hi_u32 cmd, void *arg, void *private_data)
{
    sync_drv_evt_info *info = (sync_drv_evt_info *)arg;
    return hi_drv_sync_get_new_event(info->sync, &info->evt);
}

static hi_s32 sync_drv_cmd_status(hi_u32 cmd, void *arg, void *private_data)
{
    sync_drv_status* status = (sync_drv_status *)arg;
    return hi_drv_sync_get_status(status->sync, &status->status);
}

static hi_s32 sync_drv_cmd_presync_info(hi_u32 cmd, void *arg, void *private_data)
{
    sync_drv_presync_info *info = (sync_drv_presync_info *)arg;
    return hi_drv_sync_get_presync_info(info->sync, &info->info);
}

static hi_s32 sync_drv_cmd_pcr_stc_info(hi_u32 cmd, void *arg, void *private_data)
{
    sync_drv_pcr_stc_info *info = (sync_drv_pcr_stc_info *)arg;
    return hi_drv_sync_get_pcr_stc_val(info->sync, &info->pcr, &info->stc);
}

static hi_s32 sync_drv_cmd_buf_status(hi_u32 cmd, void *arg, void *private_data)
{
    sync_drv_buf_status *status = (sync_drv_buf_status *)arg;
    return hi_drv_sync_set_buf_status(status->sync, &status->status);
}

static hi_s32 sync_drv_cmd_invoke(hi_u32 cmd, void *arg, void *private_data)
{
    sync_drv_invoke_param *param = (sync_drv_invoke_param *)arg;
    return hi_drv_sync_invoke(param->sync, &param->param);
}

static osal_ioctl_cmd g_sync_cmd_list[] = {
    {CMD_SYNC_CREATE,           sync_drv_cmd_create},
    {CMD_SYNC_DESTROY,          sync_drv_cmd_destroy},
    {CMD_SYNC_SET_ATTR,         sync_drv_cmd_set_attr},
    {CMD_SYNC_GET_ATTR,         sync_drv_cmd_get_attr},
    {CMD_SYNC_CHECK_NUM,        sync_drv_cmd_check_num},

    {CMD_SYNC_START_SYNC,       sync_drv_cmd_start_sync},
    {CMD_SYNC_STOP_SYNC,        sync_drv_cmd_stop_sync},
    {CMD_SYNC_AUD_JUDGE,        sync_drv_cmd_aud_judge},
    {CMD_SYNC_AD_AUD_JUDGE,     sync_drv_cmd_ad_aud_judge},
    {CMD_SYNC_VID_JUDGE,        sync_drv_cmd_vid_judge},
    {CMD_SYNC_PLAY_SYNC,        sync_drv_cmd_play_sync},
    {CMD_SYNC_PAUSE_SYNC,       sync_drv_cmd_pause_sync},
    {CMD_SYNC_RESUME_SYNC,      sync_drv_cmd_resume_sync},
    {CMD_SYNC_TPLAY_SYNC,       sync_drv_cmd_tplay_sync},
    {CMD_SYNC_SEEK_SYNC,        sync_drv_cmd_seek_sync},

    {CMD_SYNC_GET_NEW_EVENT,    sync_drv_cmd_new_event},
    {CMD_SYNC_GET_STATUS,       sync_drv_cmd_status},
    {CMD_SYNC_GET_PRESYNC_INFO, sync_drv_cmd_presync_info},
    {CMD_SYNC_GET_PCR_STC_INFO, sync_drv_cmd_pcr_stc_info},
    {CMD_SYNC_SET_BUF_STATUS,   sync_drv_cmd_buf_status},
    {CMD_SYNC_INVOKE,           sync_drv_cmd_invoke}
};

static osal_fileops g_sync_fops = {
    .open           = sync_drv_open,
    .release        = sync_drv_release,
    .cmd_list       = g_sync_cmd_list,
    .cmd_cnt        = sizeof(g_sync_cmd_list) / sizeof(osal_ioctl_cmd),
};

static hi_s32 sync_pm_suspend(void *priv_data)
{
    hi_s32 ret = sync_dev_suspend();
    if (ret == HI_SUCCESS) {
        HI_PRINT("sync suspend ok\n");
    }

    return ret;
}

static hi_s32 sync_pm_resume(void *priv_data)
{
    hi_s32 ret = sync_dev_resume();
    if (ret == HI_SUCCESS) {
        HI_PRINT("sync resume ok\n");
    }

    return ret;
}

static osal_pmops g_sync_pm_ops = {
    .pm_suspend = sync_pm_suspend,
    .pm_resume = sync_pm_resume,
};

static osal_dev g_sync_dev = {
    .minor = HI_DEV_SYNC_MINOR,
    .fops = &g_sync_fops,
    .pmops = &g_sync_pm_ops,
};

static hi_s32 sync_dev_register(void)
{
    hi_s32 ret;
    osal_dev *dev = &g_sync_dev;

    ret = strncpy_s(dev->name, sizeof(dev->name), HI_DEV_SYNC_NAME, strlen(HI_DEV_SYNC_NAME));
    if (ret < 0) {
        HI_ERR_SYNC("strncpy_s failed. ret = 0x%x\n", ret);
        return ret;
    }

    ret = osal_dev_register(dev);
    if (ret != HI_SUCCESS) {
        HI_FATAL_SYNC("osal_dev_register failed\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 sync_dev_unregister(void)
{
    osal_dev_unregister(&g_sync_dev);
    return HI_SUCCESS;
}

hi_s32 sync_drv_mod_init(void)
{
#ifndef HI_MCE_SUPPORT
    if (HI_SUCCESS != sync_drv_init()) {
        HI_FATAL_SYNC("SYNC init failed\n");
        return HI_FAILURE;
    }
#endif

#ifdef HI_SYNC_PROC_SUPPORT
    if (HI_SUCCESS != sync_intf_register(&g_sync_register_param)) {
        HI_FATAL_SYNC("register SYNC Intf failed\n");
        return HI_FAILURE;
    }
#endif

    if (HI_SUCCESS != sync_dev_register()) {
        HI_FATAL_SYNC("register sync failed\n");
        return HI_FAILURE;
    }

    return  0;
}

void sync_drv_mod_exit(void)
{
    (void)sync_dev_unregister();

#ifdef HI_SYNC_PROC_SUPPORT
    sync_intf_unregister();
#endif

#ifndef HI_MCE_SUPPORT
    sync_drv_exit();
#endif
}

#ifdef MODULE
module_init(sync_drv_mod_init);
module_exit(sync_drv_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
