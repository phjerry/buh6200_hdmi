/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "vfmw_adpt.h"
#include "vfmw_osal.h"
#include "vfmw_proc.h"
#include "vfmw_intf.h"
#include "vfmw_adpt_proc.h"
#include "local_proc.h"
#include "linux_proc.h"
#include "dbg.h"
#include "pts_ext.h"
#ifdef VFMW_STREAM_SUPPORT
#include "stream_log.h"
#endif
#ifdef VFMW_VPP_SUPPORT
#include "vpp_log.h"
#endif
#ifdef VFMW_USD_POOL_SUPPORT
#include "usdpool_log.h"
#endif
#ifdef VFMW_FENCE_SUPPORT
#include "fence_log.h"
#endif
#ifdef VFMW_TEE_SUPPORT
#include "tee_client_proc.h"
#endif
#ifdef VFMW_MDC_SUPPORT
#include "mdc_client_proc.h"
#endif
#ifdef VFMW_HDR_SUPPORT
#include "hdr_ext.h"
#endif
#ifdef VFMW_USER_SUPPORT
#include "userdec.h"
#endif

#if defined(VFMW_TEE_SUPPORT) || defined(VFMW_MDC_SUPPORT)
#include "client_event_log.h"
#include "client_stream_log.h"
#include "client_image_log.h"
#endif

#ifndef VFMW_KO2_SUPPORT
#define PROC_VFMW_STM "vfmw_stm"
#define PROC_VFMW_SYN "vfmw_syn"
#define PROC_VFMW_DEC "vfmw_dec"
#define PROC_VFMW_COM "vfmw_com"
#define PROC_VFMW_FSP "vfmw_fsp"
#define PROC_VFMW_KPI "vfmw_kpi"
#define PROC_VFMW_PTS "vfmw_pts"
#define PROC_VFMW_HDR "vfmw_hdr"
#define PROC_VFMW_USR "vfmw_usr"
#else
#define PROC_VFMW_STM "vfmw2_stm"
#define PROC_VFMW_SYN "vfmw2_syn"
#define PROC_VFMW_DEC "vfmw2_dec"
#define PROC_VFMW_COM "vfmw2_com"
#define PROC_VFMW_FSP "vfmw2_fsp"
#define PROC_VFMW_KPI "vfmw2_kpi"
#define PROC_VFMW_PTS "vfmw2_pts"
#define PROC_VFMW_HDR "vfmw2_hdr"
#define PROC_VFMW_USR "vfmw2_usr"
#endif

#if defined(VFMW_TEE_SUPPORT) || defined(VFMW_MDC_SUPPORT)
#define PROC_VFMW_SHR "vfmw_shr"
#define PROC_VFMW_CS  "vfmw_cs"
#endif

#ifdef VFMW_MDC_SUPPORT
#define PROC_VFMW_MLOG "vfmw_mlog"
#endif

#ifdef VFMW_USER_SUPPORT
#define PROC_VFMW_USR "vfmw_usr"
#endif

typedef struct {
    hi_s32 curr_chan;
    OS_SEMA proc_sema;
} vfmw_proc_entry;

static vfmw_proc_entry g_s_vfmw_proc_entry = {
    .curr_chan = 0,
    .proc_sema = -1,
};

static vfmw_proc_entry *vfmw_proc_get_entry(hi_void)
{
    return &g_s_vfmw_proc_entry;
}

static hi_void vfmw_proc_sema_down(hi_void)
{
    vfmw_proc_entry *entry = vfmw_proc_get_entry();

    OS_SEMA_DOWN(entry->proc_sema);
}

static hi_void vfmw_proc_sema_up(hi_void)
{
    vfmw_proc_entry *entry = vfmw_proc_get_entry();

    OS_SEMA_UP(entry->proc_sema);
}

static hi_u32 vfmw_proc_argv_to_cmd_str(hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_s32 argc, cmd_str_ptr cmd_str)
{
    hi_u32 index;
    hi_s32 ret;

    if (argc > PROC_CMD_SINGEL_LENGTH_MAX) {
        return 0;
    }

    if (argc > CMD_PARAM_MAX_COUNT) {
        dprint(PRN_ERROR, "argc:%d > max vfmw cnt %d\n", argc, CMD_PARAM_MAX_COUNT);
        argc = CMD_PARAM_MAX_COUNT;
    }

    for (index = 0; index < argc; index++) {
        ret = snprintf_s((*cmd_str)[index], CMD_PARAM_MAX_LEN, PROC_CMD_SINGEL_LENGTH_MAX - 1, *(argv + index));
        if (ret < 0) {
            dprint(PRN_ERROR, "snprintf_s failed ret = %d\n", ret);
            return index;
        }
    }

    return index;
}

static hi_void vfmw_proc_help(cmd_str_ptr cmd_str, hi_u32 count)
{
#define HELP_FMT_H              "  %-15s %-15s %-15s %s\n"
#define HELP_FMT_L              "  %-15s %-15s %-15s %s\n"
#define HELP_PRINT(Fmt, Arg...) dprint(PRN_ALWS, Fmt, ##Arg)

    HELP_PRINT(LOG_FMT_HEAD, "HELP");
    HELP_PRINT("\n");
    HELP_PRINT(HELP_FMT_H, "COMMAND", "VALUE", "TARGET", "COMMENT");
    HELP_PRINT("\n");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SAVE_RAW, "<chan id>", PROC_VFMW_STM, "save raw of specific chan");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SAVE_SEG, "<chan id>", PROC_VFMW_STM, "save seg of specific chan");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SAVE_SEEK_RAW, "<0/1>", PROC_VFMW_STM, "set save stream after seek");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_STM_SLEEP, "<value>", PROC_VFMW_STM, "set stm thread sleep time");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_STM_WAIT, "<value>", PROC_VFMW_STM, "set stm thread wait  time");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SAVE_EL, "<chan id>", PROC_VFMW_SYN, "save dolby EL stream for specific chan");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SYN_SLEEP, "<value>", PROC_VFMW_SYN, "set syn thread sleep time");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SYN_WAIT, "<value>", PROC_VFMW_SYN, "set syn thread wait  time");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SLICE_BIN, "<size>", PROC_VFMW_DEC, "set slice bin size in unit KB");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_DEC_SLEEP, "<value>", PROC_VFMW_DEC, "set dec thread sleep time");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_DEC_WAIT, "<value>", PROC_VFMW_DEC, "set dec thread wait  time");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SAMPLE_NUM, "<num>", PROC_VFMW_PTS, "set pts frd sample num");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_READ_HDR, "<0/1>", PROC_VFMW_HDR, "turn on/off hdr proc info");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_CUR_CHAN, "<chan id>", PROC_VFMW_COM, "set current chan id");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SAVE_YUV, "<chan id>", PROC_VFMW_COM, "save yuv of specific chan");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SAVE_MTDT, "<chan id>", PROC_VFMW_COM, "save metadata of specific chan");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_PRINT, "<value>", PROC_VFMW_COM, "set print control value");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_DPS_SER, "<mode>", PROC_VFMW_COM, "set dps ser mode");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_DPS_NUM, "<num>", PROC_VFMW_COM, "set dps num");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_ERR_HOLD, "<value>", PROC_VFMW_COM, "set frame output error threshold");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_CMP_MODE, "<value>", PROC_VFMW_COM, "set cmp mode:0(adpt)/1(on)/2(off)");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_UVMOS, "<0/1>", PROC_VFMW_COM, "turn on/off uvmos");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SED_ONLY, "<0/1>", PROC_VFMW_COM, "turn on/off sedonly");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_MASK_SMMU_IRQ, "<0/1>", PROC_VFMW_COM, "mask/unmask smmu irq");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_MOD_LOW_DLY_EN, "<0/1>", PROC_VFMW_COM, "turn on/off module low delay");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_TUNL_LINE, "<num>", PROC_VFMW_COM, "set tunnel line num");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_OUT_ORDER, "<order>", PROC_VFMW_COM, "set output order: 0(disp)/1(dec)/...");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_DEC_MODE, "<mode>", PROC_VFMW_COM, "set decode mode: 0(IPB)/1(IP)/2(I)/...");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_DROP_HOLD, "<value>", PROC_VFMW_COM, "set stream overflow drop threshold");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_STD_OBED, "<value>", PROC_VFMW_COM, "set standard obedience value");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SAVE_BEGIN, "<frmidx>", PROC_VFMW_COM, "set save frame begin index");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SAVE_END, "<frmidx>", PROC_VFMW_COM, "set save frame end index");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SUSPEND, "", PROC_VFMW_COM, "test suspend");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_RESUME, "", PROC_VFMW_COM, "test resume");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_START, "<chan id>", PROC_VFMW_COM, "test start");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_STOP, "<chan id>", PROC_VFMW_COM, "test stop");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_RESET, "<chan id>", PROC_VFMW_COM, "test reset");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_RESTART, "<chan id>", PROC_VFMW_COM, "test restart");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_FIX_NUM, "<num>", PROC_VFMW_COM, "fix frame buffer num");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_FIX_RES, "<w> <h>", PROC_VFMW_COM, "fix frame buffer size base on resolution");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SET_CLK, "<cfg>", PROC_VFMW_COM, "set vdh clock configuration");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_PRINT_LOG, "<num>(opt)", PROC_VFMW_COM, "turn on/off print log with msg num cfg");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_READ_LOG, "", PROC_VFMW_COM, "read print log");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_SHOW_DFLT, "", PROC_VFMW_COM, "show vctrl default configuration");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_PRT_STM_BLK, "<0/1>", PROC_VFMW_COM, "enable/disable stream block debug");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_VPP_BYPASS, "<0/1>", PROC_VFMW_COM, "set bypass");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_VPP_PR_FRM, "<0/1>", PROC_VFMW_COM, "print video frame info");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_FRM_FENCE, "<0/1>", PROC_VFMW_COM, "set frm fence enable: 0(dis)/1(en)");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_FRM_SRC_TIM, "<num>", PROC_VFMW_COM, "set frm fence src time(ms)");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_FRM_SINK_TIM, "<num>", PROC_VFMW_COM, "set frm fence sink time(ms)");
    HELP_PRINT(HELP_FMT_L, LOG_CMD_PRT_USD_BLK, "<0/1>", PROC_VFMW_COM, "enable/disable usd block debug");

#if defined(VFMW_TEE_SUPPORT) || defined(VFMW_MDC_SUPPORT)
    HELP_PRINT(HELP_FMT_L, LOG_CMD_MDC_LOG, "<value>", PROC_VFMW_CS,
               "set mdc log:0(off)/1(by uart)/2(by mem, use \'watch -n1 cat /proc/msp/vfmw_mlog\' to watch)");
#endif
    HELP_PRINT("\n");
    HELP_PRINT("  Tips:\n");
    HELP_PRINT("  1. (opt) means this arg is optional.\n");
    HELP_PRINT("  eg. echo saveraw 0 >/proc/msp/vfmw_stm\n");
    HELP_PRINT("\n");
}

static hi_void vfmw_proc_set_cur_chan(cmd_str_ptr cmd_str, hi_u32 count)
{
    hi_s32 chan_id = 0;
    vfmw_proc_entry *entry = vfmw_proc_get_entry();

    str_to_val((*cmd_str)[1], &chan_id);
    entry->curr_chan = chan_id;
    dprint(PRN_ALWS, "Pay attention to chan %d\n", chan_id);
}

static hi_void vfmw_proc_save_yuv(cmd_str_ptr cmd_str, hi_u32 count)
{
    hi_s32 chan_id = 0;
    hi_s8 file_name[FILE_NAME_MAX_LEN];
    vfmw_chan *chan = HI_NULL;

    VFMW_ASSERT_PRNT(count >= 2, "cmd param count %d invalid.\n", count); /* 2 :a number */

    str_to_val((*cmd_str)[1], &chan_id);

    chan = vadpt_chan(chan_id);
    VFMW_ASSERT_PRNT(chan != HI_NULL, "Chan %d invalid.\n", chan_id);

    OS_SEMA_DOWN(chan->sema);

    if (chan->yuv == HI_NULL) {
        if (snprintf_s(file_name, sizeof(file_name), sizeof(file_name), "vfmw_chan_%d.yuv", chan_id) < 0) {
            dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
        }
        chan->yuv = OS_FOPEN(file_name, OS_RDWR | OS_CREATE, 0);
        if (chan->yuv == HI_NULL) {
            dprint(PRN_ALWS, "Open %s failed.\n", file_name);
        } else {
            dprint(PRN_ALWS, "start save chan %d yuv to %s.\n", chan_id, file_name);
        }
    } else {
        OS_FCLOSE(chan->yuv);
        chan->yuv = HI_NULL;
        dprint(PRN_ALWS, "end save chan %d yuv.\n", chan_id);
    }

    OS_SEMA_UP(chan->sema);
}

static hi_void vfmw_proc_save_mtdt(cmd_str_ptr cmd_str, hi_u32 count)
{
    hi_s32 chan_id = 0;
    hi_s8 file_name[FILE_NAME_MAX_LEN];
    vfmw_chan *chan = HI_NULL;

    VFMW_ASSERT_PRNT(count >= 2, "cmd param count %d invalid.\n", count); /* 2 :a number */

    str_to_val((*cmd_str)[1], &chan_id);

    chan = vadpt_chan(chan_id);
    VFMW_ASSERT_PRNT(chan != HI_NULL, "Chan %d invalid.\n", chan_id);

    OS_SEMA_DOWN(chan->sema);

    if (chan->mtdt == HI_NULL) {
        if (snprintf_s(file_name, sizeof(file_name), sizeof(file_name), "vfmw_chan_%d.mtdt", chan_id) < 0) {
            dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
        }
        chan->mtdt = OS_FOPEN(file_name, OS_RDWR | OS_CREATE, 0);
        if (chan->mtdt == HI_NULL) {
            dprint(PRN_ALWS, "Open %s failed.\n", file_name);
        } else {
            dprint(PRN_ALWS, "start save chan %d metadata to %s.\n", chan_id, file_name);
        }
    } else {
        OS_FCLOSE(chan->mtdt);
        chan->mtdt = HI_NULL;
        dprint(PRN_ALWS, "end save chan %d metadata.\n", chan_id);
    }

    OS_SEMA_UP(chan->sema);
}

static hi_void vfmw_proc_set_save_begin(cmd_str_ptr cmd_str, hi_u32 count)
{
    hi_u32 data = 0;
    vfmw_dflt *dflt = vadpt_get_dflt();

    VFMW_ASSERT_PRNT(count >= 2, "cmd param count %d invalid.\n", count); /* 2 :a number */

    str_to_val((*cmd_str)[1], &data);

    dflt->save_begin = data;
    dprint(PRN_ALWS, "Set SaveBegin = %d.\n", data);
}

static hi_void vfmw_proc_set_save_end(cmd_str_ptr cmd_str, hi_u32 count)
{
    hi_u32 data = 0;
    vfmw_dflt *dflt = vadpt_get_dflt();

    VFMW_ASSERT_PRNT(count >= 2, "cmd param count %d invalid.\n", count); /* 2 :a number */

    str_to_val((*cmd_str)[1], &data);

    dflt->save_end = data;
    dprint(PRN_ALWS, "Set SaveEnd = %d.\n", data);
}

static hi_void vfmw_proc_read_adpt_modules(hi_void *buf, hi_s32 len, hi_u32 *used)
{
#ifdef VFMW_STREAM_SUPPORT
    stream_log_read(buf, len, used);
#endif

#ifdef VFMW_VPP_SUPPORT
    vpp_log_read(buf, len, used);
#endif

#ifdef VFMW_USD_POOL_SUPPORT
    usdpool_log_read(buf, len, used);
#endif

#ifdef VFMW_FENCE_SUPPORT
    fence_log_read(buf, len, used);
#endif

    return;
}

static hi_s32 vfmw_proc_write_local_modules(cmd_str_ptr buffer, hi_u32 count)
{
    hi_s32 ret = HI_FAILURE;

#ifdef VFMW_STREAM_SUPPORT
    ret = stream_log_write(buffer, count);
    if (ret == HI_SUCCESS) {
        return HI_SUCCESS;
    }
#endif

#ifdef VFMW_VPP_SUPPORT
    ret = vpp_log_write(buffer, count);
    if (ret == HI_SUCCESS) {
        return HI_SUCCESS;
    }
#endif

#ifdef VFMW_USD_POOL_SUPPORT
    ret = usdpool_log_write(buffer, count);
    if (ret == HI_SUCCESS) {
        return HI_SUCCESS;
    }
#endif

#ifdef VFMW_FENCE_SUPPORT
    ret = fence_log_write(buffer, count);
#endif

    return ret;
}

static hi_void vfmw_proc_test_suspend(cmd_str_ptr buffer, hi_u32 count)
{
    if (vadpt_suspend(HI_NULL) != HI_SUCCESS) {
        dprint(PRN_ERROR, "vadpt_suspend err\n");
    }
}
static hi_void vfmw_proc_test_resume(cmd_str_ptr buffer, hi_u32 count)
{
    if (vadpt_resume(HI_NULL) != HI_SUCCESS) {
        dprint(PRN_ERROR, "vadpt_resume err\n");
    }
}

static hi_s32 vfmw_proc_local_write(hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_u32 argc)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 i;
    cmd_handler handler = HI_NULL;
    hi_s8 *cmd_id = HI_NULL;
    hi_u32 cmd_cnt, count;
    hi_s8 cmd_str[CMD_PARAM_MAX_COUNT][CMD_PARAM_MAX_LEN];

    static const log_cmd s_adpt_cmd[] = {
        { LOG_CMD_HELP,       vfmw_proc_help },
        { LOG_CMD_CUR_CHAN,   vfmw_proc_set_cur_chan },
        { LOG_CMD_SAVE_YUV,   vfmw_proc_save_yuv },
        { LOG_CMD_SAVE_MTDT,  vfmw_proc_save_mtdt },
        { LOG_CMD_SAVE_BEGIN, vfmw_proc_set_save_begin },
        { LOG_CMD_SAVE_END,   vfmw_proc_set_save_end },
        { LOG_CMD_SUSPEND,    vfmw_proc_test_suspend },
        { LOG_CMD_RESUME,     vfmw_proc_test_resume },
    };

    cmd_cnt = vfmw_proc_argv_to_cmd_str(argv, argc, &cmd_str);
    VFMW_ASSERT_RET(cmd_cnt > 0, HI_FAILURE);

    vfmw_proc_sema_down();

    cmd_id = cmd_str[0];
    count = sizeof(s_adpt_cmd) / sizeof(log_cmd);

    for (i = 0; i < count; i++) {
        if (!OS_STRNCMP(cmd_id, s_adpt_cmd[i].cmd_name, OS_STRLEN(cmd_id))) {
            handler = s_adpt_cmd[i].handler;
            break;
        }
    }

    if (handler == HI_NULL) {
        ret = vfmw_proc_write_local_modules(&cmd_str, count);
    } else {
        handler(&cmd_str, cmd_cnt);
        ret = HI_SUCCESS;
    }

    vfmw_proc_sema_up();

    return ret;
}

static hi_s32 vfmw_proc_read(log_module module, hi_void *buf)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 chan_id;
    vfmw_chan *chan = HI_NULL;

    chan_id = vfmw_proc_get_entry()->curr_chan;
    chan = vadpt_chan(chan_id);
    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);

    vfmw_proc_sema_down();

#ifdef VFMW_MDC_SUPPORT
    ret = mdc_clt_proc_read(module, buf, chan_id);
#else
    if (chan->type == VFMW_TYPE_TEE) {
#ifdef VFMW_TEE_SUPPORT
        ret = clt_proc_read(module, buf, chan_id);
#endif
    } else {
#ifdef VFMW_LOCAL_SUPPORT
        ret = local_proc_read(module, buf, chan_id);
#endif
    }
#endif

    vfmw_proc_sema_up();

    return ret;
}

static hi_s32 vfmw_proc_write(log_module module, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_u32 argc)
{
    hi_u32 cmd_cnt;
    hi_s8 cmd_str[CMD_PARAM_MAX_COUNT][CMD_PARAM_MAX_LEN];

    cmd_cnt = vfmw_proc_argv_to_cmd_str(argv, argc, &cmd_str);
    VFMW_ASSERT_RET(cmd_cnt > 0, HI_FAILURE);

    vfmw_proc_sema_down();

#ifdef VFMW_TEE_SUPPORT
    (hi_void)clt_proc_write(module, &cmd_str, cmd_cnt);
#endif

#ifdef VFMW_MDC_SUPPORT
    (hi_void)mdc_clt_proc_write(module, &cmd_str, cmd_cnt);
#endif

#ifdef VFMW_LOCAL_SUPPORT
    (hi_void)local_proc_write(module, &cmd_str, cmd_cnt);
#endif

    vfmw_proc_sema_up();

    return HI_SUCCESS;
}

static hi_s32 vfmw_proc_read_adpt(hi_void *buf)
{
    vfmw_entry *vadpt_entry(hi_void);
    vfmw_chan *vadpt_chan(hi_s32);

    vfmw_entry *entry = HI_NULL;
    vfmw_chan *chan = HI_NULL;
    hi_s32 len = 0;
    hi_s32 used = 0;
    hi_s32 chan_id = 0;
    vfmw_state state;
    vfmw_chan_state chan_state;
    vfmw_type type;
    hi_u8 *state_str[VFMW_STATE_BUTT + 1] = { "Null", "Open", "NA" };
    hi_u8 *chan_state_str[VFMW_CHAN_STATE_BUTT + 1] = { "Null", "Create", "NA" };
    hi_u8 *type_str[VFMW_TYPE_BUFF + 1] = { "Local", "Tee", "Mdc",  "User", "NA" };

    vfmw_proc_sema_down();

    entry = vadpt_entry();

    OS_DUMP_PROC(buf, len, &used, 0, "\n");
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_S, "SDK VERSION", VERSION_STRING);
    OS_DUMP_PROC(buf, len, &used, 0, "\n");

    state = (entry->state < VFMW_STATE_BUTT) ? entry->state : VFMW_STATE_BUTT;

    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_X_S, "Check1", entry->reserve1, "state", state_str[state]);
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_X_X, "Check2", entry->reserve2, "Print",
                 dbg_get_print_word());
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_D_D, "MaxChanNum", VFMW_CHAN_NUM, "VirNum", OS_GET_VIR_MEM_NUM());

    for (chan_id = 0; chan_id < VFMW_CHAN_NUM; chan_id++) {
        chan = vadpt_chan(chan_id);
        if (chan == HI_NULL || chan->state == VFMW_CHAN_STATE_NULL) {
            continue;
        }

        type = (chan->type < VFMW_TYPE_BUFF) ? chan->type : VFMW_TYPE_BUFF;
        chan_state = (chan->state < VFMW_CHAN_STATE_BUTT) ? chan->state : VFMW_CHAN_STATE_BUTT;

        OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_D_D, "ChanId", chan_id, "is_sec", chan->is_sec);
        OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_S_S, "state", chan_state_str[chan_state], "FmwType",
                     type_str[type]);
    }

    OS_DUMP_PROC(buf, len, &used, 0, "\n");

    vfmw_proc_read_adpt_modules(buf, len, &used);

    OS_DUMP_PROC(buf, len, &used, 0, "\n");

    OS_READ_OSAL(buf, len, &used);

    vfmw_proc_sema_up();

    return HI_SUCCESS;
}

static hi_s32 vfmw_proc_read_adpt_kpi(hi_void *buf)
{
    hi_s32 len = 0;
    hi_s32 used = 0;
    hi_s32 chan_id;
    vfmw_entry *glb = HI_NULL;
    vfmw_proc_entry *entry = HI_NULL;
    vfmw_chan *chan = HI_NULL;
    vfmw_kpi *kpi = HI_NULL;
#define KPI_FMT_H "  %-15s %-15s %-15s %-15s\n"
#define KPI_FMT_L "  %-15s %-15u %-15u %-15u\n"

    vfmw_proc_sema_down();

    glb = vadpt_entry();
    entry = vfmw_proc_get_entry();
    chan_id = entry->curr_chan;

    chan = vadpt_chan(entry->curr_chan);
    if (chan == HI_NULL) {
        vfmw_proc_sema_up();
        return HI_FAILURE;
    }
    kpi = &chan->kpi;

    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_HEAD, "KPI");
    OS_DUMP_PROC(buf, len, &used, 0, "  Chan %d:\n", chan_id);
    OS_DUMP_PROC(buf, len, &used, 0, KPI_FMT_H, "state", "cost(us)", "begin(us)", "end(us)");
    OS_DUMP_PROC(buf, len, &used, 0, KPI_FMT_L, "init", glb->init_kpi.cost, glb->init_kpi.begin,
                 glb->init_kpi.end);
    OS_DUMP_PROC(buf, len, &used, 0, KPI_FMT_L, "create", kpi->create.cost, kpi->create.begin, kpi->create.end);
    OS_DUMP_PROC(buf, len, &used, 0, KPI_FMT_L, "config", kpi->config.cost, kpi->config.begin, kpi->config.end);
    OS_DUMP_PROC(buf, len, &used, 0, KPI_FMT_L, "start", kpi->start.cost, kpi->start.begin, kpi->start.end);
    OS_DUMP_PROC(buf, len, &used, 0, KPI_FMT_L, "questm", kpi->questm.cost, kpi->questm.begin, kpi->questm.end);
    OS_DUMP_PROC(buf, len, &used, 0, KPI_FMT_L, "getfrm", kpi->getfrm.cost, kpi->getfrm.begin, kpi->getfrm.end);
    OS_DUMP_PROC(buf, len, &used, 0, KPI_FMT_L, "stop", kpi->stop.cost, kpi->stop.begin, kpi->stop.end);
    OS_DUMP_PROC(buf, len, &used, 0, KPI_FMT_L, "reset", kpi->reset.cost, kpi->reset.begin, kpi->reset.end);
    OS_DUMP_PROC(buf, len, &used, 0, KPI_FMT_L, "destroy", kpi->destroy.cost, kpi->destroy.begin,
                 kpi->destroy.end);
    OS_DUMP_PROC(buf, len, &used, 0, KPI_FMT_L, "exit", glb->exit_kpi.cost, glb->exit_kpi.begin,
                 glb->exit_kpi.end);
    OS_DUMP_PROC(buf, len, &used, 0, "\n");

    vfmw_proc_sema_up();

    return HI_SUCCESS;
}

static hi_s32 vfmw_proc_read_stm(PROC_FILE *buf)
{
    return vfmw_proc_read(LOG_MODULE_STM, buf);
}

static hi_s32 vfmw_proc_write_stm(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    return vfmw_proc_write(LOG_MODULE_STM, argv, argc);
}

static hi_s32 vfmw_proc_read_syn(PROC_FILE *buf)
{
    return vfmw_proc_read(LOG_MODULE_SYN, buf);
}

static hi_s32 vfmw_proc_write_syn(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    return vfmw_proc_write(LOG_MODULE_SYN, argv, argc);
}

static hi_s32 vfmw_proc_read_dec(PROC_FILE *buf)
{
    return vfmw_proc_read(LOG_MODULE_DEC, buf);
}

static hi_s32 vfmw_proc_write_dec(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    return vfmw_proc_write(LOG_MODULE_DEC, argv, argc);
}

static hi_s32 vfmw_proc_read_com(PROC_FILE *buf)
{
    vfmw_proc_read_adpt(buf);

    return vfmw_proc_read(LOG_MODULE_COM, buf);
}

static hi_s32 vfmw_proc_write_com(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (vfmw_proc_local_write(argv, argc) == HI_SUCCESS) {
        return HI_SUCCESS;
    }

    return vfmw_proc_write(LOG_MODULE_COM, argv, argc);
}

static hi_s32 vfmw_proc_read_fsp(PROC_FILE *buf)
{
    return vfmw_proc_read(LOG_MODULE_FSP, buf);
}

static hi_s32 vfmw_proc_read_kpi(PROC_FILE *buf)
{
    return vfmw_proc_read_adpt_kpi(buf);
}

static hi_s32 vfmw_proc_read_pts(PROC_FILE *buf)
{
    hi_s32 ret;
    hi_s32 chan_id;

    vfmw_proc_sema_down();

    chan_id = vfmw_proc_get_entry()->curr_chan;

    ret = pts_info_read(buf, chan_id);

    vfmw_proc_sema_up();

    return ret;
}

static hi_s32 vfmw_proc_write_pts(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 cmd_cnt;
    hi_s8 cmd_str[CMD_PARAM_MAX_COUNT][CMD_PARAM_MAX_LEN];

    cmd_cnt = vfmw_proc_argv_to_cmd_str(argv, argc, &cmd_str);
    VFMW_ASSERT_RET(cmd_cnt > 0, HI_FAILURE);

    vfmw_proc_sema_down();

    pts_info_write(&cmd_str, cmd_cnt);

    vfmw_proc_sema_up();

    return HI_SUCCESS;
}

#ifdef VFMW_HDR_SUPPORT
static hi_s32 vfmw_proc_read_hdr(PROC_FILE *buf)
{
    hi_s32 ret;
    hi_s32 chan_id;

    vfmw_proc_sema_down();

    chan_id = vfmw_proc_get_entry()->curr_chan;
    ret = hdr_info_read(buf, chan_id);

    vfmw_proc_sema_up();

    return ret;
}

static hi_s32 vfmw_proc_write_hdr(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 cmd_cnt;
    hi_s8 cmd_str[CMD_PARAM_MAX_COUNT][CMD_PARAM_MAX_LEN];

    cmd_cnt = vfmw_proc_argv_to_cmd_str(argv, argc, &cmd_str);
    VFMW_ASSERT_RET(cmd_cnt > 0, HI_FAILURE);

    vfmw_proc_sema_down();

    hdr_info_write(&cmd_str, cmd_cnt);

    vfmw_proc_sema_up();

    return HI_SUCCESS;
}
#endif

#if defined(VFMW_TEE_SUPPORT) || defined(VFMW_MDC_SUPPORT)
#ifdef VFMW_TEE_SUPPORT
static hi_s32 vfmw_proc_read_shr(PROC_FILE *p)
{
    hi_s32 ret = HI_FAILURE;
    vfmw_proc_entry *entry = vfmw_proc_get_entry();
    vfmw_chan *chan = vadpt_chan(entry->curr_chan);

    vfmw_proc_sema_down();

    if (chan->type == VFMW_TYPE_TEE) {
        ret = clt_proc_read_share_mem(p, entry->curr_chan);
    }

    vfmw_proc_sema_up();

    return HI_SUCCESS;
}
#endif

static hi_s32 vfmw_proc_read_cs(PROC_FILE *p)
{
    hi_s32 ret;
    vfmw_proc_entry *entry = vfmw_proc_get_entry();
    vfmw_chan *chan = vadpt_chan(entry->curr_chan);

    vfmw_proc_sema_down();

    /* share mem info */
    if (chan->type == VFMW_TYPE_TEE || chan->type == VFMW_TYPE_MCU) {
        (hi_void)clt_evt_log_read(p, entry->curr_chan);
        (hi_void)clt_img_log_read(p, entry->curr_chan);
        (hi_void)clt_stm_log_read(p, entry->curr_chan);
    }

#ifdef VFMW_MDC_SUPPORT
    /* client and server info */
    ret = mdc_clt_proc_read_mdc(p, entry->curr_chan);
#endif

    vfmw_proc_sema_up();

    return ret;
}

static hi_s32 vfmw_proc_write_cs(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 cmd_cnt;
    hi_s8 cmd_str[CMD_PARAM_MAX_COUNT][CMD_PARAM_MAX_LEN];
#ifdef VFMW_TEE_SUPPORT
    vfmw_proc_entry *entry = vfmw_proc_get_entry();
    vfmw_chan *chan = vadpt_chan(entry->curr_chan);
#endif
    cmd_cnt = vfmw_proc_argv_to_cmd_str(argv, argc, &cmd_str);
    VFMW_ASSERT_RET(cmd_cnt > 0, HI_FAILURE);

    vfmw_proc_sema_down();

#ifdef VFMW_MDC_SUPPORT
    mdc_clt_proc_write_mdc(&cmd_str, cmd_cnt);
#endif

#ifdef VFMW_TEE_SUPPORT
    if (chan->type == VFMW_TYPE_TEE) {
    }
#endif

    vfmw_proc_sema_up();

    return HI_SUCCESS;
}
#ifdef VFMW_MDC_SUPPORT
static hi_s32 vfmw_proc_read_mlog(PROC_FILE *p)
{
    hi_s32 ret;

    vfmw_proc_sema_down();

    ret = mdc_log_get_log(p);

    vfmw_proc_sema_up();

    return ret;
}
#endif
#endif

static OS_PROC_CMD g_vfmw_com_list[] = {
    {LOG_CMD_HELP, vfmw_proc_write_com},
    {LOG_CMD_CUR_CHAN, vfmw_proc_write_com},
    {LOG_CMD_SAVE_YUV, vfmw_proc_write_com},
    {LOG_CMD_SAVE_MTDT, vfmw_proc_write_com},
    {LOG_CMD_PRINT, vfmw_proc_write_com},
    {LOG_CMD_DPS_SER, vfmw_proc_write_com},
    {LOG_CMD_DPS_NUM, vfmw_proc_write_com},
    {LOG_CMD_ERR_HOLD, vfmw_proc_write_com},
    {LOG_CMD_CMP_MODE, vfmw_proc_write_com},
    {LOG_CMD_UVMOS, vfmw_proc_write_com},
    {LOG_CMD_OUT_ORDER, vfmw_proc_write_com},
    {LOG_CMD_DEC_MODE, vfmw_proc_write_com},
    {LOG_CMD_DROP_HOLD, vfmw_proc_write_com},
    {LOG_CMD_STD_OBED, vfmw_proc_write_com},
    {LOG_CMD_SED_ONLY, vfmw_proc_write_com},
    {LOG_CMD_MASK_SMMU_IRQ, vfmw_proc_write_com},
    {LOG_CMD_TUNL_LINE, vfmw_proc_write_com},
    {LOG_CMD_MOD_LOW_DLY_EN, vfmw_proc_write_com},
    {LOG_CMD_SAVE_BEGIN, vfmw_proc_write_com},
    {LOG_CMD_SAVE_END, vfmw_proc_write_com},
    {LOG_CMD_SUSPEND, vfmw_proc_write_com},
    {LOG_CMD_RESUME, vfmw_proc_write_com},
    {LOG_CMD_START, vfmw_proc_write_com},
    {LOG_CMD_STOP, vfmw_proc_write_com},
    {LOG_CMD_RESET, vfmw_proc_write_com},
    {LOG_CMD_RESTART, vfmw_proc_write_com},
    {LOG_CMD_FIX_NUM, vfmw_proc_write_com},
    {LOG_CMD_FIX_RES, vfmw_proc_write_com},
    {LOG_CMD_SET_CLK, vfmw_proc_write_com},
    {LOG_CMD_PRINT_LOG, vfmw_proc_write_com},
    {LOG_CMD_READ_LOG, vfmw_proc_write_com},
    {LOG_CMD_SHOW_DFLT, vfmw_proc_write_com},
    {LOG_CMD_PRT_STM_BLK, vfmw_proc_write_com},
    {LOG_CMD_PRT_USD_BLK, vfmw_proc_write_com},
    {LOG_CMD_VPP_BYPASS, vfmw_proc_write_com},
    {LOG_CMD_VPP_PR_FRM, vfmw_proc_write_com},
    {LOG_CMD_FRM_FENCE, vfmw_proc_write_com},
    {LOG_CMD_FRM_SRC_TIM, vfmw_proc_write_com},
    {LOG_CMD_FRM_SINK_TIM, vfmw_proc_write_com}
};

static OS_PROC_CMD g_vfmw_stm_list[] = {
    {LOG_CMD_SAVE_RAW, vfmw_proc_write_stm},
    {LOG_CMD_SAVE_SEEK_RAW, vfmw_proc_write_stm},
    {LOG_CMD_SAVE_SEG, vfmw_proc_write_stm},
    {LOG_CMD_STM_SLEEP, vfmw_proc_write_stm},
    {LOG_CMD_STM_WAIT, vfmw_proc_write_stm}
};

static OS_PROC_CMD g_vfmw_syn_list[] = {
    {LOG_CMD_SYN_SLEEP, vfmw_proc_write_syn},
    {LOG_CMD_SYN_WAIT, vfmw_proc_write_syn},
    {LOG_CMD_SAVE_EL, vfmw_proc_write_syn}
};

static OS_PROC_CMD g_vfmw_dec_list[] = {
    {LOG_CMD_SLICE_BIN, vfmw_proc_write_dec},
    {LOG_CMD_DEC_SLEEP, vfmw_proc_write_dec},
    {LOG_CMD_DEC_WAIT, vfmw_proc_write_dec}
};

static OS_PROC_CMD g_vfmw_pts_list[] = {
    {LOG_CMD_SAMPLE_NUM, vfmw_proc_write_pts}
};

#ifdef VFMW_HDR_SUPPORT
static OS_PROC_CMD g_vfmw_hdr_list[] = {
    {LOG_CMD_READ_HDR, vfmw_proc_write_hdr}
};
#endif

#if defined(VFMW_TEE_SUPPORT) || defined(VFMW_MDC_SUPPORT)
static OS_PROC_CMD g_vfmw_cs_list[] = {
    {LOG_CMD_MDC_LOG, vfmw_proc_write_cs}
};
#endif

hi_s32 vfmw_adpt_proc_create(hi_void)
{
    hi_s32 ret;
    vfmw_proc_entry *entry = vfmw_proc_get_entry();

    ret = OS_PROC_CREATE(PROC_VFMW_STM, vfmw_proc_read_stm,
        g_vfmw_stm_list, (sizeof(g_vfmw_stm_list) / sizeof(OS_PROC_CMD)));
    VFMW_ASSERT_RET_PRNT(ret == 0, HI_FAILURE, "create proc %s failed!\n", PROC_VFMW_STM);

    ret = OS_PROC_CREATE(PROC_VFMW_SYN, vfmw_proc_read_syn,
        g_vfmw_syn_list, (sizeof(g_vfmw_syn_list) / sizeof(OS_PROC_CMD)));
    VFMW_ASSERT_RET_PRNT(ret == 0, HI_FAILURE, "create proc %s failed!\n", PROC_VFMW_SYN);

    ret = OS_PROC_CREATE(PROC_VFMW_DEC, vfmw_proc_read_dec,
        g_vfmw_dec_list, (sizeof(g_vfmw_dec_list) / sizeof(OS_PROC_CMD)));
    VFMW_ASSERT_RET_PRNT(ret == 0, HI_FAILURE, "create proc %s failed!\n", PROC_VFMW_DEC);

    ret = OS_PROC_CREATE(PROC_VFMW_COM, vfmw_proc_read_com,
        g_vfmw_com_list, (sizeof(g_vfmw_com_list) / sizeof(OS_PROC_CMD)));
    VFMW_ASSERT_RET_PRNT(ret == 0, HI_FAILURE, "create proc %s failed!\n", PROC_VFMW_COM);

    ret = OS_PROC_CREATE(PROC_VFMW_FSP, vfmw_proc_read_fsp, HI_NULL, 0);
    VFMW_ASSERT_RET_PRNT(ret == 0, HI_FAILURE, "create proc %s failed!\n", PROC_VFMW_FSP);

    ret = OS_PROC_CREATE(PROC_VFMW_KPI, vfmw_proc_read_kpi, HI_NULL, 0);
    VFMW_ASSERT_RET_PRNT(ret == 0, HI_FAILURE, "create proc %s failed!\n", PROC_VFMW_KPI);

    ret = OS_PROC_CREATE(PROC_VFMW_PTS, vfmw_proc_read_pts,
        g_vfmw_pts_list, (sizeof(g_vfmw_pts_list) / sizeof(OS_PROC_CMD)));
    VFMW_ASSERT_RET_PRNT(ret == 0, HI_FAILURE, "create proc %s failed!\n", PROC_VFMW_PTS);

#ifdef VFMW_HDR_SUPPORT
    ret = OS_PROC_CREATE(PROC_VFMW_HDR, vfmw_proc_read_hdr,
        g_vfmw_hdr_list, (sizeof(g_vfmw_hdr_list) / sizeof(OS_PROC_CMD)));
    VFMW_ASSERT_RET_PRNT(ret == 0, HI_FAILURE, "create proc %s failed!\n", PROC_VFMW_HDR);
#endif

#ifdef VFMW_TEE_SUPPORT
    ret = OS_PROC_CREATE(PROC_VFMW_SHR, vfmw_proc_read_shr, HI_NULL, 0);
    VFMW_ASSERT_RET_PRNT(ret == 0, HI_FAILURE, "create proc %s failed!\n", PROC_VFMW_SHR);
#endif

#if defined(VFMW_TEE_SUPPORT) || defined(VFMW_MDC_SUPPORT)
    ret = OS_PROC_CREATE(PROC_VFMW_CS, vfmw_proc_read_cs,
        g_vfmw_cs_list, (sizeof(g_vfmw_cs_list) / sizeof(OS_PROC_CMD)));
    VFMW_ASSERT_RET_PRNT(ret == 0, HI_FAILURE, "create proc %s failed!\n", PROC_VFMW_CS);
#endif

#ifdef VFMW_MDC_SUPPORT
    ret = OS_PROC_CREATE(PROC_VFMW_MLOG, vfmw_proc_read_mlog, HI_NULL, 0);
    VFMW_ASSERT_RET_PRNT(ret == 0, HI_FAILURE, "create proc %s failed!\n", PROC_VFMW_MLOG);
#endif

    OS_SEMA_INIT(&entry->proc_sema);

    return HI_SUCCESS;
}

hi_void vfmw_adpt_proc_destroy(hi_void)
{
    vfmw_proc_entry *entry = vfmw_proc_get_entry();

    OS_PROC_DESTROY(PROC_VFMW_STM);
    OS_PROC_DESTROY(PROC_VFMW_SYN);
    OS_PROC_DESTROY(PROC_VFMW_DEC);
    OS_PROC_DESTROY(PROC_VFMW_COM);
    OS_PROC_DESTROY(PROC_VFMW_FSP);
    OS_PROC_DESTROY(PROC_VFMW_KPI);
    OS_PROC_DESTROY(PROC_VFMW_PTS);
#ifdef VFMW_HDR_SUPPORT
    OS_PROC_DESTROY(PROC_VFMW_HDR);
#endif
#ifdef VFMW_TEE_SUPPORT
    OS_PROC_DESTROY(PROC_VFMW_SHR);
#endif
#if defined(VFMW_TEE_SUPPORT) || defined(VFMW_MDC_SUPPORT)
    OS_PROC_DESTROY(PROC_VFMW_CS);
#endif
#ifdef VFMW_MDC_SUPPORT
    OS_PROC_DESTROY(PROC_VFMW_MLOG);
#endif

    OS_SEMA_EXIT(entry->proc_sema);
}
