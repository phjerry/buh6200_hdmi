/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: sync interface definition
 * Author: Hisilicon multimedia software group
 * Create: 2012-12-22
 */

#include "drv_sync.h"
#include "drv_sync_stc.h"
#include "drv_sync_intf.h"
#include "drv_sync_ext.h"

#ifdef SYNC_NOT_SUPPORT_OSAL
#include <linux/vmalloc.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/semaphore.h>
#endif

#include "hi_drv_module.h"
#include "drv_ao_ext.h"
#include "linux/hisilicon/securec.h"
#include "drv_sync_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#define SYNC_MOD_ID_SHIFT_NUM         24

#define SYNC_NAME                     "hi_sync"
#define PROC_NAME_LEN                 16
#define SYNC_VID_FINISH_THRESHOLD     60
#define SYNC_VID_NOVEL_THRESHOLD      100

#define SYNC_DEFAULT_VID_FRAME_TIME   40
#define SYNC_DEFAULT_VID_DELAY_TIME   20
#define SYNC_DEFAULT_AUD_FRAME_TIME   24


#define HI_MOD_SYNC     "sync"

#define DRV_SYNC_TRY_LOCK(mutex) do { \
        if (osal_sem_trydown(mutex)) { \
            HI_FATAL_SYNC("ERR: Sync try lock error!\n"); \
            return -1; \
        } \
    } while (0)

#define DRV_SYNC_LOCK(mutex) do { \
        if (osal_sem_down_interruptible(mutex)) { \
            HI_FATAL_SYNC("ERR: Sync lock error!\n"); \
            return -1; \
        } \
    } while (0)

#define DRV_SYNC_UNLOCK(mutex) do { \
        osal_sem_up(mutex); \
    } while (0)

#define SYNC_CHECK_ID(id) do { \
        if ((id) >= SYNC_MAX_NUM) { \
            HI_ERR_SYNC("invalid id\n"); \
            return HI_ERR_SYNC_INVALID_PARA; \
        } \
    } while (0)

static osal_atomic g_sync_count;
sync_global_state  g_sync_global_state;

static osal_semaphore g_sync_mutex;

#ifdef HI_SYNC_PROC_SUPPORT
static sync_register_param *g_sync_proc_param = NULL;

static hi_s32 sync_init_used_proc(void)
{
    hi_u32       i, j;
    hi_char      proc_name[PROC_NAME_LEN];
    osal_proc_entry *proc_item = NULL;

    for (i = 0; i < SYNC_MAX_NUM; i++) {
        if (g_sync_global_state.info[i].ctx) {
            if (snprintf_s(proc_name, sizeof(proc_name), sizeof(proc_name), "%s%02d", HI_MOD_SYNC, i) < 0) {
                HI_ERR_SYNC("Execute snprintf_s failed!\n");
            }

            proc_item = osal_proc_add(proc_name, strlen(proc_name));
            if (proc_item) {
                proc_item->private = g_sync_global_state.info[i].ctx;
                proc_item->read  = g_sync_proc_param->rd_proc;
                proc_item->cmd_list = g_sync_proc_param->cmd_list;
                proc_item->cmd_cnt = g_sync_proc_param->cmd_cnt;
                continue;
            }

            HI_FATAL_SYNC("add %s proc failed.\n", proc_name);

            for (j = 0; j < i; j++) {
                if (g_sync_global_state.info[j].ctx) {
                    if (snprintf_s(proc_name, sizeof(proc_name), sizeof(proc_name), "%s%02d", HI_MOD_SYNC, j) < 0) {
                        HI_ERR_SYNC("Execute snprintf_s failed!\n");
                    }

                    osal_proc_remove(proc_name, strlen(proc_name));
                }
            }
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static void sync_deinit_used_proc(void)
{
    hi_u32  i;
    hi_char proc_name[PROC_NAME_LEN] = {0};

    for (i = 0; i < SYNC_MAX_NUM; i++) {
        if (g_sync_global_state.info[i].ctx) {
            if (snprintf_s(proc_name, sizeof(proc_name), sizeof(proc_name), "%s%02d", HI_MOD_SYNC, i) < 0) {
                HI_ERR_SYNC("Execute snprintf_s failed!\n");
            }

            osal_proc_remove(proc_name, strlen(proc_name));
        }
    }

    return;
}

hi_s32 sync_intf_register(sync_register_param *param)
{
    hi_s32  ret;

    g_sync_proc_param = param;
    ret = sync_init_used_proc();
    if (ret != HI_SUCCESS) {
        g_sync_proc_param = HI_NULL;
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

void sync_intf_unregister(void)
{
    sync_deinit_used_proc();
    g_sync_proc_param = HI_NULL;

    return;
}
#endif

static hi_s32 sync_check_sync_attr(sync_attr *attr)
{
    CHECK_PARAM(attr->sync_ref < SYNC_REF_MAX);
    CHECK_PARAM((attr->novel_region.vid_plus_time >= 0) &&
                (attr->novel_region.vid_plus_time <= VID_LEAD_DISCARD_THRESHOLD));
    CHECK_PARAM((attr->novel_region.vid_negative_time >= (-VID_LAG_DISCARD_THRESHOLD)) &&
                (attr->novel_region.vid_negative_time <= 0));

    CHECK_PARAM((attr->start_region.vid_plus_time >= 0) &&
                (attr->start_region.vid_plus_time <= VID_LEAD_DISCARD_THRESHOLD));
    CHECK_PARAM((attr->start_region.vid_negative_time >= (-VID_LAG_DISCARD_THRESHOLD)) &&
                (attr->start_region.vid_negative_time <= 0));

    CHECK_PARAM(attr->novel_region.vid_plus_time > attr->start_region.vid_plus_time);
    CHECK_PARAM(attr->novel_region.vid_negative_time < attr->start_region.vid_negative_time);

    if ((attr->sync_ref != SYNC_REF_NONE) &&
        ((attr->pre_sync_timeout) && (attr->pre_sync_timeout < PRE_SYNC_MIN_TIME))) {
        HI_WARN_SYNC("para attr->u32PreSyncTimeoutMs is invalid, modify it to 300.\n");
        attr->pre_sync_timeout = PRE_SYNC_MIN_TIME;
    }

    return HI_SUCCESS;
}

static void sync_check_sync_finish(sync_context *ctx)
{
    if (ctx->is_sync_finish == HI_TRUE) {
        return;
    }

    if (ctx->attr.pre_sync_timeout > 0) {
        if (ctx->is_presync_finish == HI_FALSE) {
            ctx->is_sync_finish = HI_FALSE;
            return;
        }
    }

    /* We think single-channel synchronization has been completed */
    if ((ctx->is_aud_enable == HI_FALSE) || (ctx->is_vid_enable == HI_FALSE)) {
        ctx->is_sync_finish = HI_TRUE;
        return;
    }

    if ((ctx->vid_aud_diff >= ctx->attr.start_region.vid_plus_time) ||
        (ctx->vid_aud_diff <= ctx->attr.start_region.vid_negative_time)) {
        ctx->sync_novel_continue_cnt++;

        if (ctx->sync_novel_continue_cnt >= SYNC_VID_NOVEL_THRESHOLD) {
            ctx->is_sync_finish = HI_TRUE;
            return;
        }

        ctx->sync_finish_continue_cnt = 0;
        ctx->is_sync_finish = HI_FALSE;
        return;
    } else {
        ctx->sync_novel_continue_cnt = 0;
        ctx->sync_finish_continue_cnt++;
    }

    if (ctx->sync_finish_continue_cnt >= SYNC_VID_FINISH_THRESHOLD) {
        ctx->is_sync_finish = HI_TRUE;
        return;
    }

    return;
}

static hi_s32 sync_check_num(hi_u32 *num, const void *unique)
{
    hi_u32   i;

    CHECK_NULL(num);
    *num = 0;

    for (i = 0; i < SYNC_MAX_NUM; i++) {
        /* multi-thread process in MCE mode */
        /* CNcomment:此时mce方式类多线程处理即可 */
        if (g_sync_global_state.info[i].unique == unique) {
            (*num)++;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 sync_destroy(hi_u32 id)
{
#ifdef HI_SYNC_PROC_SUPPORT
    hi_char   proc_name[PROC_NAME_LEN] = {0};
#endif

    SYNC_CHECK_ID(id);

    if (HI_NULL == g_sync_global_state.info[id].ctx) {
        HI_ERR_SYNC("this is invalid handle.\n");
        return HI_ERR_SYNC_DESTROY_ERR;
    }

#ifdef HI_SYNC_PROC_SUPPORT
#ifdef SYNC_DEBUG_DETAIL
    sync_proc_debug_info(g_sync_global_state.info[id].ctx, SYNC_DBG_INFO_LIST_RELEASE, HI_NULL);
#endif

    if (g_sync_proc_param) {
        memset_s(proc_name, sizeof(proc_name), 0, sizeof(proc_name));
        if (snprintf_s(proc_name, sizeof(proc_name), sizeof(proc_name), "%s%02d", HI_MOD_SYNC, id) < 0) {
            HI_ERR_SYNC("Execute snprintf_s failed!\n");
        }

        osal_proc_remove(proc_name, strlen(proc_name));
    }
#endif

    osal_vfree(HI_ID_SYNC, g_sync_global_state.info[id].ctx);

    g_sync_global_state.info[id].ctx = HI_NULL;
    g_sync_global_state.info[id].unique = HI_NULL;

    g_sync_global_state.cnt--;

    if (g_sync_global_state.del_ins) {
        g_sync_global_state.del_ins(&id);
    }

    return HI_SUCCESS ;
}

sync_context *sync_info_ctx_get(hi_u32 id)
{
    if (id >= SYNC_MAX_NUM) {
        return NULL;
    }

    return g_sync_global_state.info[id].ctx;
}

hi_slong sync_ioctl(void *file, unsigned int cmd, void *arg)
{
    hi_s32 ret;

    switch (cmd) {
        case CMD_SYNC_CREATE: {
            sync_create_param *param = (sync_create_param *)arg;
            ret = hi_drv_sync_create(&param->attr, &param->sync, file);
            break;
        }

        case CMD_SYNC_DESTROY: {
            ret = hi_drv_sync_destroy(*(hi_handle *)arg);
            break;
        }

        case CMD_SYNC_SET_ATTR: {
            sync_drv_attr *attr = (sync_drv_attr *)arg;
            ret = hi_drv_sync_attr_set(attr->sync, &attr->attr);
            break;
        }

        case CMD_SYNC_GET_ATTR: {
            sync_drv_attr *attr = (sync_drv_attr *)arg;
            ret = hi_drv_sync_attr_get(attr->sync, &attr->attr);
            break;
        }

        case CMD_SYNC_CHECK_NUM: {
            ret = sync_check_num(arg, file);
            break;
        }

        case CMD_SYNC_START_SYNC: {
            sync_drv_start_opt *opt = (sync_drv_start_opt *)arg;
            ret = hi_drv_sync_start(opt->sync, opt->chn);
            break;
        }

        case CMD_SYNC_STOP_SYNC: {
            sync_drv_stop_opt *opt = (sync_drv_stop_opt *)arg;
            ret = hi_drv_sync_stop(opt->sync, opt->chn);
            break;
        }

        case CMD_SYNC_PLAY_SYNC: {
            ret = hi_drv_sync_play(*(hi_handle *)arg);
            break;
        }

        case CMD_SYNC_SEEK_SYNC: {
            sync_drv_seek_opt *opt = (sync_drv_seek_opt *)arg;
            ret = hi_drv_sync_seek(opt->sync, opt->seek_pts);
            break;
        }

        case CMD_SYNC_PAUSE_SYNC: {
            ret = hi_drv_sync_pause(*(hi_handle *)arg);
            break;
        }

        case CMD_SYNC_RESUME_SYNC: {
            ret = hi_drv_sync_resume(*(hi_handle *)arg);
            break;
        }

        case CMD_SYNC_TPLAY_SYNC: {
            ret = hi_drv_sync_tplay(*(hi_handle *)arg);
            break;
        }

        case CMD_SYNC_VID_JUDGE: {
            sync_vid_judge_status *status = (sync_vid_judge_status *)arg;
            ret = hi_drv_sync_vid_judge(status->sync, &status->info, &status->opt);
            break;
        }

        case CMD_SYNC_AUD_JUDGE: {
            sync_aud_judge_status *status = (sync_aud_judge_status *)arg;
            ret = hi_drv_sync_aud_judge(status->sync, &status->info, &status->opt);
            break;
        }

        case CMD_SYNC_AD_AUD_JUDGE: {
            sync_aud_judge_status *status = (sync_aud_judge_status *)arg;
            ret = hi_drv_sync_ad_aud_judge(status->sync, &status->info, &status->opt);
            break;
        }

        case CMD_SYNC_GET_NEW_EVENT: {
            sync_drv_evt_info *info = (sync_drv_evt_info *)arg;
            ret = hi_drv_sync_get_new_event(info->sync, &info->evt);
            break;
        }

        case CMD_SYNC_GET_STATUS: {
            sync_drv_status* status = (sync_drv_status *)arg;
            ret = hi_drv_sync_get_status(status->sync, &status->status);
            break;
        }

        case CMD_SYNC_GET_PRESYNC_INFO: {
            sync_drv_presync_info *info = (sync_drv_presync_info *)arg;
            ret = hi_drv_sync_get_presync_info(info->sync, &info->info);
            break;
        }

        case CMD_SYNC_GET_PCR_STC_INFO: {
            sync_drv_pcr_stc_info *info = (sync_drv_pcr_stc_info *)arg;
            ret = hi_drv_sync_get_pcr_stc_val(info->sync, &info->pcr, &info->stc);
            break;
        }

        case CMD_SYNC_SET_BUF_STATUS: {
            sync_drv_buf_status *status = (sync_drv_buf_status *)arg;
            ret = hi_drv_sync_set_buf_status(status->sync, &status->status);
            break;
        }

        case CMD_SYNC_INVOKE: {
            sync_drv_invoke_param *param = (sync_drv_invoke_param *)arg;
            ret = hi_drv_sync_invoke(param->sync, &param->param);
            break;
        }

        default:
            HI_ERR_SYNC("Unsupport cmd[%u]\n", cmd);
            return -ENOIOCTLCMD;
    }

    return ret;
}

hi_s32 sync_drv_open(void *priv_data)
{
    DRV_SYNC_LOCK(&g_sync_mutex);

    if (1 == osal_atomic_inc_return(&g_sync_count)) {
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);

    return 0;
}

hi_s32 sync_drv_release(void *priv_data)
{
    hi_s32 i;
    hi_s32 ret;

    DRV_SYNC_LOCK(&g_sync_mutex);

    for (i = 0; i < SYNC_MAX_NUM; i++) {
        if (g_sync_global_state.info[i].unique == priv_data) {
            ret = sync_destroy(i);
            if (ret != HI_SUCCESS) {
                DRV_SYNC_UNLOCK(&g_sync_mutex);
                return -1;
            }
        }
    }

    if (osal_atomic_dec_return(&g_sync_count)) {
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);

    return 0;
}


hi_s32 sync_dev_suspend(void)
{
    hi_u32  i;

    DRV_SYNC_TRY_LOCK(&g_sync_mutex);

    /* if SYNC device has never opened by any process,return directly */
    if (!osal_atomic_read(&g_sync_count)) {
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return 0;
    }

    /* it needs to pause when standby,because system time keep running */
    for (i = 0; i < SYNC_MAX_NUM; i++) {
        if (g_sync_global_state.info[i].ctx) {
            sync_pause(i);
        }
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);

    return 0;
}


hi_s32 sync_dev_resume(void)
{
    hi_u32  i;

    DRV_SYNC_TRY_LOCK(&g_sync_mutex);

    if (!osal_atomic_read(&g_sync_count)) {
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return 0;
    }

    for (i = 0; i < SYNC_MAX_NUM; i++) {
        if (g_sync_global_state.info[i].ctx) {
            sync_resume(i);
        }
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);

    return 0;
}

#ifndef MODULE
#ifdef HI_SYNC_PROC_SUPPORT
EXPORT_SYMBOL(sync_intf_register);
EXPORT_SYMBOL(sync_intf_unregister);
#endif
EXPORT_SYMBOL(sync_info_ctx_get);
EXPORT_SYMBOL(sync_ioctl);
EXPORT_SYMBOL(sync_drv_open);
EXPORT_SYMBOL(sync_drv_release);
EXPORT_SYMBOL(sync_dev_suspend);
EXPORT_SYMBOL(sync_dev_resume);
/* Temporary useage */
EXPORT_SYMBOL(sync_vid_proc);
EXPORT_SYMBOL(sync_get_local_time);
#endif

static sync_export_func g_sync_export_funcs = {
    .vid_proc = sync_vid_proc,
    .pcr_proc = sync_pcr_proc,
#ifdef SYNC_PCR_ADJUST_STC_SUPPORT
    .hpll_conf_flag_set = sync_hpll_conf_flag_set,
#else
    .hpll_conf_flag_set = NULL,
#endif
};

static hi_s32 sync_osal_init(void)
{
    hi_s32 ret = osal_atomic_init(&g_sync_count);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("atomic init failed.\n");
        return ret;
    }

    ret = osal_sem_init(&g_sync_mutex, 1);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("sem init failed.\n");
        return ret;
    }

#if (defined (HI_SYNC_PROC_SUPPORT)) && (defined (SYNC_DEBUG_DETAIL))
    sync_proc_debug_info(NULL, SYNC_DBG_INFO_SYS_INIT, NULL);
#endif

#ifdef SYNC_PCR_ADJUST_STC_SUPPORT
    sync_stc_sys_init();
#endif

    return HI_SUCCESS;
}

static hi_s32 sync_osal_deinit(void)
{
    osal_atomic_destory(&g_sync_count);
    osal_sem_destory(&g_sync_mutex);

#if (defined (HI_SYNC_PROC_SUPPORT)) && (defined (SYNC_DEBUG_DETAIL))
    sync_proc_debug_info(NULL, SYNC_DBG_INFO_SYS_DEINIT, NULL);
#endif

#ifdef SYNC_PCR_ADJUST_STC_SUPPORT
    sync_stc_sys_deinit();
#endif

    return HI_SUCCESS;
}

hi_s32 sync_drv_init(void)
{
    hi_s32 ret;
    hi_u32 i;

    ret = sync_osal_init();
    if (ret != HI_SUCCESS) {
        HI_FATAL_SYNC("ERR: sync_osal_init failed!\n");
        return ret;
    }

    ret = hi_drv_module_register(HI_ID_SYNC, SYNC_NAME, (void *)&g_sync_export_funcs, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_FATAL_SYNC("ERR: hi_drv_module_register!\n");
        sync_osal_deinit();
        return ret;
    }

#ifdef HI_SYNC_PROC_SUPPORT
    g_sync_proc_param = HI_NULL;
#endif

    g_sync_global_state.cnt = 0;
    g_sync_global_state.add_ins = 0;
    g_sync_global_state.del_ins = 0;

    for (i = 0; i < SYNC_MAX_NUM; i++) {
        g_sync_global_state.info[i].ctx = HI_NULL;
        g_sync_global_state.info[i].unique = HI_NULL;
    }

#ifdef MODULE
        HI_PRINT("Load hi_sync.ko success.  \t(%s)\n", VERSION_STRING);
#endif

    return 0;
}

void sync_drv_exit(void)
{
    hi_drv_module_unregister(HI_ID_SYNC);
    (void)sync_osal_deinit();

#ifdef MODULE
        HI_PRINT("Unload hi_sync.ko success.\n");
#endif
    return;
}

static hi_s32 sync_mod_init(void)
{
    DRV_SYNC_LOCK(&g_sync_mutex);

    if (1 == osal_atomic_inc_return(&g_sync_count)) {
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);

    return 0;
}

static hi_s32 sync_mod_deinit(void)
{
    hi_s32 i;
    hi_s32 ret;

    DRV_SYNC_LOCK(&g_sync_mutex);

    for (i = 0; i < SYNC_MAX_NUM; i++) {
        if ((g_sync_global_state.info[i].ctx != HI_NULL) &&
            (g_sync_global_state.info[i].unique == HI_NULL)) {
            ret = sync_destroy(i);
            if (ret != HI_SUCCESS) {
                DRV_SYNC_UNLOCK(&g_sync_mutex);
                return -1;
            }
        }
    }

    if (osal_atomic_dec_return(&g_sync_count)) {
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);

    return 0;
}

//#ifdef HI_MCE_SUPPORT
/* the following interface is only for mce now */
/* CNCommont: 下述接口均为SYNC模块驱动下对外接口，目前仅提供给MCE模块使用 */
static hi_s32 drv_sync_check_handle(hi_handle sync, sync_context **ctx, hi_u32 *id)
{
    *id = SYNC_GET_ID(sync);

    if (*id >= SYNC_MAX_NUM) {
        HI_ERR_SYNC("ERR: invalid handle!");
        return HI_FAILURE;
    }

    if (HI_NULL == g_sync_global_state.info[*id].ctx) {
        HI_ERR_SYNC("ERR: ctx = null!");
        return HI_FAILURE;
    }

    *ctx = g_sync_global_state.info[*id].ctx;

    return HI_SUCCESS;
}

static void drv_sync_reset_vid_info(sync_context *ctx)
{
    ctx->is_vid_first_come = HI_FALSE;
    ctx->vid_first_sys_time = HI_INVALID_TIME;
    ctx->vid_first_pts = SYNC_INVALID_PTS;
    ctx->vid_last_pts = SYNC_INVALID_PTS;
    ctx->is_vid_presync_target_init = HI_FALSE;
    ctx->vid_presync_target_time = HI_INVALID_TIME;
    ctx->is_vid_local_time = HI_FALSE;
    ctx->vid_last_sys_time = HI_INVALID_TIME;
    ctx->vid_last_local_time = HI_INVALID_TIME;
    ctx->vid_pause_local_time = HI_INVALID_TIME;
    ctx->vid_pts_series_cnt = 0;

    ctx->is_buf_adjust = HI_FALSE;
    ctx->cur_buf_status.is_buf_time_need_ctrl = HI_FALSE;
    ctx->cur_buf_status.buf_time_max = 0;
    ctx->cur_buf_status.buf_time_min = 0;
    ctx->cur_buf_status.buf_time_normal = 0;
    ctx->cur_buf_status.vid_buf_state = SYNC_BUF_STATE_NORMAL;
    ctx->cur_buf_status.vid_buf_percent = 0;
    ctx->cur_buf_status.is_overflow_disc_frm = HI_FALSE;

    ctx->is_vid_sync_adjust = HI_FALSE;
    ctx->vid_discard_play_cnt = 0;
    ctx->vid_discard_cnt = 0;
    ctx->vid_repeat_play_cnt = 0;
    ctx->vid_repeat_cnt = 0;
    ctx->is_vid_first_play = HI_FALSE;
    ctx->is_vid_normal_play = HI_FALSE;
    ctx->vid_first_play_time = HI_INVALID_TIME;
    ctx->vid_info.src_pts = SYNC_INVALID_PTS;
    ctx->vid_info.pts = SYNC_INVALID_PTS;
    ctx->vid_info.frame_time = SYNC_DEFAULT_VID_FRAME_TIME;
    ctx->vid_info.delay_time = SYNC_DEFAULT_VID_DELAY_TIME;
    ctx->vid_info.disp_time = 1;

    ctx->is_vid_first_valid_come = HI_FALSE;
    ctx->vid_first_valid_pts = SYNC_INVALID_PTS;
    ctx->vid_last_src_pts = SYNC_INVALID_PTS;
    ctx->is_vid_pts_loop_rewind = HI_FALSE;

    ctx->vid_first_dec_pts = SYNC_INVALID_PTS;
    ctx->vid_second_dec_pts = SYNC_INVALID_PTS;

    ctx->is_sync_finish = HI_FALSE;
    ctx->sync_adjust_delay_frm_cnt = 0;
    ctx->sync_finish_continue_cnt = 0;
    ctx->sync_novel_continue_cnt = 0;
    ctx->vid_last_valid_pts = SYNC_INVALID_PTS;

    return;
}

static void drv_sync_reset_aud_info(sync_context *ctx)
{
    ctx->is_aud_first_come = HI_FALSE;
    ctx->aud_first_sys_time = HI_INVALID_TIME;
    ctx->aud_first_pts = SYNC_INVALID_PTS;
    ctx->aud_last_pts = SYNC_INVALID_PTS;
    ctx->aud_last_buf_time = 0;
    ctx->is_aud_presync_target_init = HI_FALSE;
    ctx->aud_presync_target_time = HI_INVALID_TIME;
    ctx->is_aud_local_time = HI_FALSE;
    ctx->aud_last_sys_time = HI_INVALID_TIME;
    ctx->aud_last_local_time = HI_INVALID_TIME;
    ctx->aud_pause_local_time = HI_INVALID_TIME;
    ctx->aud_pts_series_cnt = 0;

    ctx->is_buf_adjust = HI_FALSE;
    ctx->cur_buf_status.is_buf_time_need_ctrl = HI_FALSE;
    ctx->cur_buf_status.buf_time_max = 0;
    ctx->cur_buf_status.buf_time_min = 0;
    ctx->cur_buf_status.buf_time_normal = 0;
    ctx->cur_buf_status.aud_buf_state = SYNC_BUF_STATE_NORMAL;
    ctx->cur_buf_status.aud_buf_percent = 0;

    ctx->is_aud_resync = HI_TRUE;
    ctx->is_aud_rebuf_fund = HI_TRUE;
    ctx->aud_play_cnt = 0;
    ctx->aud_discard_cnt = 0;
    ctx->aud_repeat_cnt = 0;
    ctx->aud_info.src_pts = SYNC_INVALID_PTS;
    ctx->aud_info.pts = SYNC_INVALID_PTS;
    ctx->aud_info.frame_time = SYNC_DEFAULT_AUD_FRAME_TIME;
    ctx->aud_info.buf_time = 0;
    ctx->aud_info.frame_num = 0;
    ctx->is_aud_first_play = HI_FALSE;
    ctx->aud_first_play_time = HI_INVALID_TIME;

    ctx->is_aud_first_valid_come = HI_FALSE;
    ctx->aud_first_valid_pts = SYNC_INVALID_PTS;
    ctx->aud_last_src_pts = SYNC_INVALID_PTS;
    ctx->is_aud_pts_loop_rewind = HI_FALSE;

    ctx->is_sync_finish = HI_FALSE;
    ctx->aud_last_valid_pts = SYNC_INVALID_PTS;
    ctx->sync_adjust_delay_frm_cnt = 0;
    ctx->sync_finish_continue_cnt = 0;
    ctx->sync_novel_continue_cnt = 0;

    return;
}

static void drv_sync_reset_pcr_info(sync_context *ctx)
{
    ctx->pcr_sync_info.is_pcr_first_come = HI_FALSE;
    ctx->pcr_sync_info.is_pcr_adjust_delta_ok = HI_TRUE;
    ctx->pcr_sync_info.pcr_first_sys_time = HI_INVALID_TIME;
    ctx->pcr_sync_info.pcr_first = SYNC_INVALID_PTS;
    ctx->pcr_sync_info.pcr_last = SYNC_INVALID_PTS;
    ctx->pcr_sync_info.pcr_last_sys_time = HI_INVALID_TIME;
    ctx->pcr_sync_info.pcr_last_local_time = HI_INVALID_TIME;
    ctx->pcr_sync_info.pcr_pause_local_time = HI_INVALID_TIME;
    ctx->pcr_sync_info.is_pcr_local_time = HI_FALSE;
    ctx->pcr_sync_info.pcr_series_cnt = 0;
    ctx->pcr_sync_info.pcr_lead_series_cnt = 0;
    ctx->pcr_sync_info.pcr_lag_series_cnt = 0;
    ctx->pcr_sync_info.stc_info.stc_last = SYNC_INVALID_PTS;
    ctx->pcr_sync_info.pcr_sync_start_sys_time = HI_INVALID_TIME;
    ctx->pcr_sync_info.pcr_delta = 0;
    ctx->pcr_sync_info.pcr_adjust_type = SYNC_SCR_ADJUST_MAX;
    ctx->pcr_sync_info.is_pcr_loop_rewind = HI_FALSE;
    ctx->pcr_sync_info.stc_info.is_stc_adjust_fail = HI_FALSE;
    ctx->pcr_sync_info.stc_info.is_stc_adjust_init = HI_FALSE;
    ctx->pcr_sync_info.pcr_last_valid = SYNC_INVALID_PTS;

    return;
}

static void drv_sync_reset_com_info(sync_context *ctx)
{
    ctx->cur_status = SYNC_STATUS_STOP;
    ctx->presync_start_sys_time = HI_INVALID_TIME;
    ctx->presync_end_sys_time = HI_INVALID_TIME;
    ctx->is_presync_finish = HI_FALSE;
    ctx->buf_fund_end_sys_time = HI_INVALID_TIME;
    ctx->is_buf_fund_finish = HI_FALSE;
    ctx->presync_target  = SYNC_CHAN_VID;
    ctx->presync_target_time = HI_INVALID_TIME;
    ctx->is_presync_target_init = HI_FALSE;
    ctx->ext_presync_target_time = HI_INVALID_TIME;
    ctx->is_use_ext_presync_target = HI_FALSE;

    ctx->is_first_sync_state_report = HI_FALSE;
    ctx->is_pcr_rewind = HI_FALSE;
    ctx->is_vid_rewind = HI_FALSE;
    ctx->is_aud_rewind = HI_FALSE;

    ctx->loop_rewind_start_time = HI_INVALID_TIME;

    ctx->is_ms12 = HI_FALSE;
    ctx->aud_fund_begin_time = HI_INVALID_TIME;

    ctx->is_scr_init = HI_FALSE;
    ctx->scr_first_local_time = HI_INVALID_TIME;
    ctx->scr_first_sys_time = HI_INVALID_TIME;
    ctx->scr_last_local_time = HI_INVALID_TIME;
    ctx->scr_last_sys_time = HI_INVALID_TIME;

    ctx->is_aud_resync = HI_FALSE;
    ctx->is_aud_rebuf_fund = HI_FALSE;

    ctx->evt.is_aud_pts_jump = HI_FALSE;
    ctx->evt.is_vid_pts_jump = HI_FALSE;
    ctx->evt.is_stat_change = HI_FALSE;
    ctx->loop_rewind_time = HI_INVALID_TIME;
    ctx->is_loop_rewind = HI_FALSE;
    ctx->cur_pcr_val = 0;
    ctx->cur_stc_val = 0;
    ctx->external_ppm = 0;
    ctx->is_pcr_stc_changed = HI_FALSE;
    ctx->is_ext_pll_adjust = HI_FALSE;

    ctx->is_sync_finish = HI_FALSE;

    ctx->sync_finish_continue_cnt = 0;
    ctx->sync_novel_continue_cnt = 0;
    ctx->aud_last_valid_pts = SYNC_INVALID_PTS;
    ctx->vid_last_valid_pts = SYNC_INVALID_PTS;

    ctx->is_buf_adjust = HI_FALSE;
    ctx->cur_buf_status.is_buf_time_need_ctrl = HI_FALSE;
    ctx->cur_buf_status.buf_time_max = 0;
    ctx->cur_buf_status.buf_time_min = 0;
    ctx->cur_buf_status.buf_time_normal = 0;

    return;
}

static void drv_sync_reset_stat_info(sync_context *ctx, sync_chan_type chn)
{
    if (chn == SYNC_CHAN_VID) {
        drv_sync_reset_vid_info(ctx);
    }

    if (chn == SYNC_CHAN_AUD) {
        drv_sync_reset_aud_info(ctx);
    }

    if ((!ctx->is_vid_enable) && (!ctx->is_aud_enable)) {
        drv_sync_reset_pcr_info(ctx);
        drv_sync_reset_com_info(ctx);
    }

    return;
}

hi_s32 hi_drv_sync_init(void)
{
    hi_s32 ret;

    ret = sync_drv_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("call sync_drv_init failed ! \n");
        return ret;
    }

    ret = sync_mod_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("call sync_mod_init failed ! \n");
        return ret;
    }

    return ret;
}

hi_s32 hi_drv_sync_deinit(void)
{
    hi_s32 ret;

    ret = sync_mod_deinit();
    sync_drv_exit();

    return ret;
}

#ifdef  AIAO_SRC_PLL_HAS_CREECTED
static hi_s32 drv_sync_set_aud_pll_cfg(void)
{
    hi_s32 ret;
    aiao_export_func* ao_func = HI_NULL;
    hi_u32 crg_fbdiv = 0;
    hi_u32 crg_frac = 0;

    hi_u32 apll_val_int;
    hi_u32 apll_val_frac;

    ret = hi_drv_module_get_function(HI_ID_AO, (void **)&ao_func);
    if (ret != HI_SUCCESS) {
        HI_FATAL_SYNC("hi_drv_module_get_function failed.\n");
        return HI_FAILURE;
    }

    if (HI_NULL == ao_func || HI_NULL == ao_func->ao_drv_get_pll_cfg) {
        HI_FATAL_SYNC("ao_func is null.\n");
        return HI_FAILURE;
    }

    ret = ao_func->ao_drv_get_pll_cfg(&crg_frac, &crg_fbdiv);
    if (ret != HI_SUCCESS) {
        HI_FATAL_SYNC("call pfnAO_DrvGetPllCfg\n");
        return HI_FAILURE;
    }

    DRV_SYNC_LOCK(&g_sync_mutex);
    apll_val_int = sync_get_aud_pll_src_value(SYNC_AUD_PLL_VALUE_TYPE_INT);
    apll_val_frac = sync_get_aud_pll_src_value(SYNC_AUD_PLL_VALUE_TYPE_FRAC);
    if ((apll_val_int != crg_fbdiv) || (apll_val_frac != crg_frac)) {
        sync_set_aud_pll_src_value(SYNC_AUD_PLL_VALUE_TYPE_INT, crg_fbdiv);
        sync_set_aud_pll_src_value(SYNC_AUD_PLL_VALUE_TYPE_FRAC, crg_frac);
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS;
}
#endif

static hi_s32 drv_sync_create(sync_context     **ctx, hi_u32 *index)
{
    hi_s32 ret;
    hi_u32 i;
    sync_context *sync_ctx = HI_NULL;

    if (g_sync_global_state.cnt >= SYNC_MAX_NUM) {
        HI_ERR_SYNC("the sync num is max.\n");
        return HI_ERR_SYNC_CREATE_ERR;
    }

    for (i = 0; i < SYNC_MAX_NUM; i++) {
        if (HI_NULL == g_sync_global_state.info[i].ctx) {
            break;
        }
    }

    if (i == SYNC_MAX_NUM) {
        HI_ERR_SYNC("Not free sync handle.\n");
        return HI_ERR_SYNC_CREATE_ERR;
    }

    sync_ctx = (sync_context *)osal_vmalloc(HI_ID_SYNC, sizeof(sync_context));
    if (sync_ctx == HI_NULL) {
        HI_FATAL_SYNC("malloc sync context failed.\n");
        return HI_FAILURE;
    }

    ret = memset_s(sync_ctx, sizeof(sync_context), 0x0, sizeof(sync_context));
    if (ret != HI_SUCCESS) {
        HI_FATAL_SYNC("memset_s failed.\n");
        osal_vfree(HI_ID_SYNC, sync_ctx);
        return HI_FAILURE;
    }

    *ctx = sync_ctx;
    *index = i;

    return HI_SUCCESS;
}

static hi_s32 drv_sync_add_proc(hi_u32 index, sync_context *ctx)
{
    osal_proc_entry *proc_item = HI_NULL;
    hi_char          proc_name[PROC_NAME_LEN] = {0};

    if (snprintf_s(proc_name, sizeof(proc_name), sizeof(proc_name), "%s%02d", HI_MOD_SYNC, index) < 0) {
        HI_ERR_SYNC("Execute snprintf_s failed!\n");
    }

    if (g_sync_proc_param) {
        proc_item = osal_proc_add(proc_name, strlen(proc_name));
        if (proc_item == HI_NULL) {
            HI_FATAL_SYNC("add %s proc failed.\n", proc_name);
            osal_vfree(HI_ID_SYNC, ctx);
            return HI_FAILURE;
        }

        proc_item->private = ctx;
        proc_item->read  = g_sync_proc_param->rd_proc;
        proc_item->cmd_list = g_sync_proc_param->cmd_list;
        proc_item->cmd_cnt = g_sync_proc_param->cmd_cnt;
    }

    return HI_SUCCESS;
}
hi_s32 hi_drv_sync_create(sync_attr *attr, hi_handle *sync, const void *unique)
{
    hi_s32            ret;
    sync_context      *ctx = HI_NULL;
    hi_u32            id;

    CHECK_NULL(attr);
    CHECK_NULL(sync);

    ret = sync_check_sync_attr(attr);
    RETURN_WHEN_ERROR(ret, sync_check_sync_attr);

    ret = drv_sync_create(&ctx, &id);
    if (ret != HI_SUCCESS) {
        return ret;
    }

#ifdef AIAO_SRC_PLL_HAS_CREECTED
    ret = drv_sync_set_aud_pll_cfg();
    if (ret != HI_SUCCESS) {
        osal_vfree(HI_ID_SYNC, ctx);
        return HI_FAILURE;
    }
#endif

#ifdef HI_SYNC_PROC_SUPPORT
    ret = drv_sync_add_proc(id, ctx);
    RETURN_WHEN_ERROR(ret, drv_sync_add_proc);
#endif

    g_sync_global_state.info[id].ctx = ctx;
    g_sync_global_state.info[id].unique = unique;

    (void)memcpy_s(&ctx->attr, sizeof(sync_attr), attr, sizeof(sync_attr));

    ctx->is_vid_enable = HI_FALSE;
    ctx->is_aud_enable = HI_FALSE;
    ctx->cur_status    = SYNC_STATUS_STOP;
    ctx->is_ddp_mode   = HI_FALSE;
    ctx->is_use_stop_region = HI_TRUE;
    ctx->is_print = id == 0 ? HI_TRUE : HI_FALSE;

    drv_sync_reset_stat_info(ctx, SYNC_CHAN_VID);
    drv_sync_reset_stat_info(ctx, SYNC_CHAN_AUD);

    *sync = (HI_ID_SYNC << SYNC_MOD_ID_SHIFT_NUM) | id;

#ifdef SYNC_DEBUG_DETAIL
#ifdef HI_SYNC_PROC_SUPPORT
    sync_proc_debug_info(g_sync_global_state.info[id].ctx, SYNC_DBG_INFO_LIST_INITIAL, HI_NULL);
#endif
#endif

    g_sync_global_state.cnt++;

    if (g_sync_global_state.add_ins) {
        g_sync_global_state.add_ins(&id);
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_destroy(hi_handle sync)
{
    hi_u32 id;

#ifdef HI_SYNC_PROC_SUPPORT
    hi_char   proc_name[PROC_NAME_LEN] = {0};
#endif

    id = SYNC_GET_ID(sync);

    DRV_SYNC_LOCK(&g_sync_mutex);
    SYNC_CHECK_ID(id);

    if (HI_NULL == g_sync_global_state.info[id].ctx) {
        HI_ERR_SYNC("this is invalid handle.\n");
        return HI_ERR_SYNC_DESTROY_ERR;
    }

#ifdef HI_SYNC_PROC_SUPPORT
#ifdef SYNC_DEBUG_DETAIL
    sync_proc_debug_info(g_sync_global_state.info[id].ctx, SYNC_DBG_INFO_LIST_RELEASE, HI_NULL);
#endif

    if (g_sync_proc_param) {
        memset_s(proc_name, sizeof(proc_name), 0, sizeof(proc_name));
        if (snprintf_s(proc_name, sizeof(proc_name), sizeof(proc_name), "%s%02d", HI_MOD_SYNC, id) < 0) {
            HI_ERR_SYNC("Execute snprintf_s failed!\n");
        }

        osal_proc_remove(proc_name, strlen(proc_name));
    }
#endif

    osal_vfree(HI_ID_SYNC, g_sync_global_state.info[id].ctx);

    g_sync_global_state.info[id].ctx = HI_NULL;
    g_sync_global_state.info[id].unique = HI_NULL;

    g_sync_global_state.cnt--;

    if (g_sync_global_state.del_ins) {
        g_sync_global_state.del_ins(&id);
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS ;
}

hi_s32 hi_drv_sync_attr_get(hi_handle sync, sync_attr *attr)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    if (HI_NULL == attr) {
        HI_ERR_SYNC("ERR: pstSyncAttr is null!\n");
        return HI_FAILURE;
    }

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    if (memcpy_s(attr, sizeof(sync_attr), &ctx->attr, sizeof(sync_attr))) {
        HI_ERR_SYNC("memcpy_s failed!\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_attr_set(hi_handle sync, const sync_attr *attr)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    if (HI_NULL == attr) {
        HI_ERR_SYNC("ERR: pSyncAttr is null!\n");
        return HI_FAILURE;
    }

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    if (memcpy_s(&ctx->attr, sizeof(sync_attr), attr, sizeof(sync_attr))) {
        HI_ERR_SYNC("memcpy_s failed!\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_check_num(hi_u32 *num, const void *unique)
{
    return sync_check_num(num, unique);
}

hi_s32 hi_drv_sync_start(hi_handle sync, sync_chan_type chn)
{
    hi_s32        ret;
    sync_context  *ctx = HI_NULL;
    hi_u32        id = 0;

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    if (chn == SYNC_CHAN_VID) {
        ctx->is_vid_enable = HI_TRUE;
        ret = sync_start(id);
        if (ret != HI_SUCCESS) {
            HI_ERR_SYNC("SYNC_StartSync err.\n");
            DRV_SYNC_UNLOCK(&g_sync_mutex);
            return HI_FAILURE;
        }
    }

    if (chn == SYNC_CHAN_AUD) {
        ctx->is_aud_enable = HI_TRUE;
        ret = sync_start(id);
        if (ret != HI_SUCCESS) {
            HI_ERR_SYNC("SYNC_StartSync err.\n");
            DRV_SYNC_UNLOCK(&g_sync_mutex);
            return HI_FAILURE;
        }
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_stop(hi_handle sync, sync_chan_type chn)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    if (chn == SYNC_CHAN_VID) {
        ctx->is_vid_enable = HI_FALSE;
        drv_sync_reset_stat_info(ctx, SYNC_CHAN_VID);
    }

    if (chn == SYNC_CHAN_AUD) {
        ctx->is_aud_enable = HI_FALSE;
        drv_sync_reset_stat_info(ctx, SYNC_CHAN_AUD);
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_play(hi_handle sync)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    ctx->cur_status = SYNC_STATUS_PLAY;

    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_seek(hi_handle sync, hi_s64 pts)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    ctx->ext_presync_target_time = (hi_u32)pts;
    ctx->is_use_ext_presync_target = HI_TRUE;
    DRV_SYNC_UNLOCK(&g_sync_mutex);

    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_pause(hi_handle sync)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    ret = sync_pause(id);
    if (ret != HI_SUCCESS) {
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        HI_ERR_SYNC("ERR: sync_pause. \n");
        return ret;
    }

    ctx->cur_status = SYNC_STATUS_PAUSE;
    DRV_SYNC_UNLOCK(&g_sync_mutex);

    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_resume(hi_handle sync)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    if (ctx->cur_status == SYNC_STATUS_PLAY) {
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_SUCCESS;
    }

    ret = sync_resume(id);
    if (ret != HI_SUCCESS) {
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        HI_ERR_SYNC("ERR: sync_pause. \n");
        return ret;
    }

    ctx->cur_status = SYNC_STATUS_PLAY;
    DRV_SYNC_UNLOCK(&g_sync_mutex);

    return HI_SUCCESS;
}


hi_s32 hi_drv_sync_tplay(hi_handle sync)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    ctx->cur_status = SYNC_STATUS_TPLAY;
    DRV_SYNC_UNLOCK(&g_sync_mutex);

    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_ad_aud_judge(hi_handle sync, sync_aud_info *info, sync_aud_opt *opt)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    if (info == HI_NULL || opt == HI_NULL) {
        HI_ERR_SYNC("ERR: ptr is null!\n");
        return HI_FAILURE;
    }

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    sync_ad_aud_proc(sync, info, opt);

    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_aud_judge(hi_handle sync, sync_aud_info *info, sync_aud_opt *opt)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    if ((HI_NULL == info) || (HI_NULL == opt)) {
        HI_ERR_SYNC("ERR: ptr is null!\n");
        return HI_FAILURE;
    }

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    sync_aud_proc(sync, info, opt);

    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_vid_judge(hi_handle sync, sync_vid_info *info, sync_vid_opt *opt)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    if ((HI_NULL == info) || (HI_NULL == opt)) {
        HI_ERR_SYNC("ERR: ptr is null!\n");
        return HI_FAILURE;
    }

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    sync_vid_proc(sync, info, opt);

    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_pcr_proc(hi_handle sync, hi_s64 pcr, hi_s64 stc)
{
    sync_pcr_proc(sync, (hi_u64)pcr, (hi_u64)stc);
    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_get_new_event(hi_handle sync, sync_event_info *info)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    memcpy_s(info, sizeof(sync_event_info), &ctx->evt, sizeof(sync_event_info));

    if (ctx->evt.is_vid_pts_jump) {
        ctx->evt.is_vid_pts_jump = HI_FALSE;
    }

    if (ctx->evt.is_aud_pts_jump) {
        ctx->evt.is_aud_pts_jump = HI_FALSE;
    }

    if (ctx->evt.is_stat_change) {
        ctx->evt.is_stat_change = HI_FALSE;
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_get_status(hi_handle sync, sync_status_info *status)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;
    hi_u32       local_time;
    hi_u32       play_time;

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    ret = sync_get_time(id, &local_time, &play_time);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: sync_get_time!\n");
        return HI_FAILURE;
    }

    status->first_aud_pts = ctx->aud_first_pts;
    status->first_vid_pts = ctx->vid_first_pts;
    status->last_aud_pts = ctx->aud_last_pts;
    status->last_vid_pts = ctx->vid_last_pts;
    status->vid_aud_diff = ctx->vid_aud_diff;
    status->play_time = play_time;
    status->local_time = local_time;
    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_get_presync_info(hi_handle sync, sync_presync_info *info)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    sync_check_sync_finish(ctx);

    info->is_sync_finish         = ctx->is_sync_finish;
    info->is_presync_finish      = ctx->is_presync_finish;
    info->is_presync_target_init = ctx->is_presync_target_init;
    info->target_pts             = ctx->presync_target_time;
    info->presync_target         = ctx->presync_target;

    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS;
}

hi_s32 hi_drv_sync_get_pcr_stc_val(hi_handle sync, hi_s64 *pcr, hi_s64 *stc)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    if (ctx->is_pcr_stc_changed) {
        *pcr = (hi_s64)ctx->cur_pcr_val;
        *stc = (hi_s64)ctx->cur_stc_val;
        ctx->is_pcr_stc_changed = HI_FALSE;
    } else {
        *pcr = 0;
        *stc = 0;
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS;
}


hi_s32 hi_drv_sync_set_buf_status(hi_handle sync, const sync_buf_status *status)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    CHECK_PARAM(status->vid_buf_state < SYNC_BUF_STATE_MAX);
    CHECK_PARAM(status->aud_buf_state < SYNC_BUF_STATE_MAX);

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    if (memcpy_s(&ctx->cur_buf_status, sizeof(sync_buf_status), status, sizeof(sync_buf_status))) {
        HI_ERR_SYNC("ERR: memcpy_s fail. ret 0x%x\n", ret);
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS;
}


hi_s32 hi_drv_sync_invoke(hi_handle sync, sync_invoke_param* invoke)
{
    hi_s32       ret;
    sync_context *ctx = HI_NULL;
    hi_u32       id = 0;

    DRV_SYNC_LOCK(&g_sync_mutex);

    ret = drv_sync_check_handle(sync, &ctx, &id);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("ERR: drv_sync_check_handle.\n");
        DRV_SYNC_UNLOCK(&g_sync_mutex);
        return HI_FAILURE;
    }

    switch (invoke->invoke_id) {
        case SYNC_INVOKE_SET_VID_FIRST_PTS:
            ctx->vid_first_dec_pts = invoke->vid_first_dec_pts;
            break;

        case SYNC_INVOKE_SET_VID_SECOND_PTS:
            ctx->vid_second_dec_pts = invoke->vid_second_dec_pts;
            break;

        case SYNC_INVOKE_SET_VID_STOP_REGIN:
            ctx->is_use_stop_region = invoke->is_use_stop_region;
            break;

        case SYNC_INVOKE_SET_MS12_FLAG:
            ctx->is_ms12 = invoke->is_ms12_mode;
            break;

        case SYNC_INVOKE_SET_DDP_TEST_FLAG:
            ctx->is_ddp_mode = invoke->is_ddp_test;
            break;

        case SYNC_INVOKE_SET_AUD_REWIND_FLAG:
            ctx->is_aud_rewind = invoke->is_aud_rewind;
            ctx->pcr_sync_info.pcr_delta = 0;
            break;

        case SYNC_INVOKE_GET_PCR_REWIND_FLAG:
            invoke->is_pcr_rewind = ctx->is_pcr_rewind;
            break;

        case SYNC_INVOKE_SET_SYNC_FINISH_FLAG:
            ctx->is_sync_finish = invoke->is_sync_finsih;
            ctx->sync_finish_continue_cnt = 0;
            ctx->sync_novel_continue_cnt = 0;
            break;

        case SYNC_INVOKE_SET_EXTERNAL_PLL_FLAG:
            ctx->is_ext_pll_adjust = invoke->use_external_pll;
            break;

        case SYNC_INVOKE_SET_STC_CALIBRATION:
            ctx->external_ppm = invoke->ppm;
            break;

        default:
            HI_INFO_SYNC("invalid opt!");
            break;
    }

    DRV_SYNC_UNLOCK(&g_sync_mutex);
    return HI_SUCCESS;
}


hi_s32 hi_drv_sync_reg_callback(sync_manage_fn add_callback, sync_manage_fn del_callback)
{
    g_sync_global_state.add_ins = add_callback;
    g_sync_global_state.del_ins = del_callback;
    return HI_SUCCESS;
}

EXPORT_SYMBOL(hi_drv_sync_init);
EXPORT_SYMBOL(hi_drv_sync_deinit);
EXPORT_SYMBOL(hi_drv_sync_create);
EXPORT_SYMBOL(hi_drv_sync_destroy);
EXPORT_SYMBOL(hi_drv_sync_attr_get);
EXPORT_SYMBOL(hi_drv_sync_attr_set);
EXPORT_SYMBOL(hi_drv_sync_check_num);
EXPORT_SYMBOL(hi_drv_sync_start);
EXPORT_SYMBOL(hi_drv_sync_stop);
EXPORT_SYMBOL(hi_drv_sync_play);
EXPORT_SYMBOL(hi_drv_sync_seek);
EXPORT_SYMBOL(hi_drv_sync_pause);
EXPORT_SYMBOL(hi_drv_sync_resume);
EXPORT_SYMBOL(hi_drv_sync_tplay);
EXPORT_SYMBOL(hi_drv_sync_vid_judge);
EXPORT_SYMBOL(hi_drv_sync_aud_judge);
EXPORT_SYMBOL(hi_drv_sync_ad_aud_judge);
EXPORT_SYMBOL(hi_drv_sync_pcr_proc);
EXPORT_SYMBOL(hi_drv_sync_get_new_event);
EXPORT_SYMBOL(hi_drv_sync_get_status);
EXPORT_SYMBOL(hi_drv_sync_get_presync_info);
EXPORT_SYMBOL(hi_drv_sync_get_pcr_stc_val);
EXPORT_SYMBOL(hi_drv_sync_set_buf_status);
EXPORT_SYMBOL(hi_drv_sync_invoke);
EXPORT_SYMBOL(hi_drv_sync_reg_callback);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
