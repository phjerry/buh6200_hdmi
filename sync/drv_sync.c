/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drive sync interface definition
 * Author: Hisilicon multimedia software group
 * Create: 2012-12-22
 */

#include "drv_sync.h"

#ifdef SYNC_NOT_SUPPORT_OSAL
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <linux/list.h>
#include <linux/semaphore.h>
#include <linux/slab.h>
#endif
#include "hi_drv_stat.h"
#include "linux/hisilicon/securec.h"

#include "hi_drv_ao.h"
#include "drv_sync_intf.h"
#include "drv_sync_stc.h"
#include "drv_sync_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PCR_GRADIENT_ERR_VALUE       1
#define SAMPLE_FREQ_DEFAULT_VALUE    27000  /* 27MHZ sample frequence */

typedef enum {
    SYNC_PROC_TYPE_SMOOTH,
    SYNC_PROC_TYPE_QUICK,
    SYNC_PROC_TYPE_MAX
} sync_proc_type;

#define SYNC_CHECK_ID_NOT_RET(id) do { \
        if ((id) >= SYNC_MAX_NUM) { \
            HI_ERR_SYNC("invalid id\n"); \
            return; \
        } \
        if (sync_info_ctx_get(id) == HI_NULL) { \
            HI_ERR_SYNC("invalid sync\n"); \
            return; \
        } \
    } while (0)

#define SYNC_CHECK_ID(id) do { \
        if ((id) >= SYNC_MAX_NUM) { \
            HI_ERR_SYNC("invalid id\n"); \
            return HI_ERR_SYNC_INVALID_PARA; \
        } \
        if (sync_info_ctx_get(id) == HI_NULL) { \
            HI_ERR_SYNC("invalid sync\n"); \
            return HI_ERR_SYNC_INVALID_PARA; \
        } \
    } while (0)

#define DRV_SYNC_LOCK(mutex) do { \
        if (osal_sem_down_interruptible(mutex)) { \
            HI_FATAL_SYNC("ERR: Sync lock error!\n"); \
            return; \
        } \
    } while (0)

#define DRV_SYNC_UNLOCK(mutex) do { \
        osal_sem_up(mutex); \
    } while (0)

#ifdef SYNC_DEBUG_DETAIL
static osal_semaphore g_sync_dbg_info_mtx;

static void sync_add_vid_dbg_info(sync_context *ctx)
{
    sync_dbg_info_status *dbg_info = HI_NULL;
    struct list_head *pos = HI_NULL;
    struct list_head *n = HI_NULL;
    hi_s32 ret;

    if (ctx->dbg_list_max_len == 0) {
        return;
    }

    if (ctx->dbg_list_node_cnt >= ctx->dbg_list_max_len) {
        list_for_each_safe(pos, n, &ctx->dbg_info_list_head) {
            /* get this node info, fill in new parameter */
            dbg_info = list_entry(pos, sync_dbg_info_status, list);

            /* list is full, delete first node */
            list_del(pos);
            ctx->dbg_list_node_cnt--;
            break;
        }
    }

    /* there is not available kmalloc mem, need to malloc */
    if (dbg_info == HI_NULL) {
        dbg_info = (sync_dbg_info_status *)osal_kmalloc(HI_ID_SYNC, sizeof(sync_dbg_info_status), GFP_KERNEL);
        if (dbg_info == HI_NULL) {
            return;
        }
    }

    ret = memset_s(dbg_info, sizeof(sync_dbg_info_status), 0x0, sizeof(sync_dbg_info_status));
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("memset_s failed\n");
        return;
    }

    dbg_info->aud_src_pts    = ctx->aud_info.src_pts;
    dbg_info->aud_pts        = ctx->aud_info.pts;
    dbg_info->aud_local_time = sync_get_local_time(ctx, SYNC_CHAN_AUD);
    dbg_info->aud_buf_time   = ctx->aud_info.buf_time;
    dbg_info->aud_frame_time = ctx->aud_info.frame_time;

    dbg_info->vid_src_pts    = ctx->vid_info.src_pts;
    dbg_info->vid_pts        = ctx->vid_info.pts;
    dbg_info->vid_local_time = sync_get_local_time(ctx, SYNC_CHAN_VID);
    dbg_info->vid_delay_time = ctx->vid_info.delay_time;
    dbg_info->vid_frame_time = ctx->vid_info.frame_time;

    dbg_info->pcr_last       = ctx->pcr_sync_info.pcr_last;
    dbg_info->pcr_local_time = sync_get_local_time(ctx, SYNC_CHAN_PCR);
    dbg_info->pcr_delta_time = ctx->pcr_sync_info.pcr_delta;
    dbg_info->vid_pcr_diff   = ctx->pcr_sync_info.vid_pcr_diff;
    dbg_info->aud_pcr_diff   = ctx->pcr_sync_info.aud_pcr_diff;

    dbg_info->vid_aud_diff   = ctx->vid_aud_diff;

    dbg_info->disp_time      = ctx->vid_info.disp_time;

    dbg_info->system_time    = sync_get_sys_time();

    dbg_info->proc_type      = ctx->vid_opt.proc;

    dbg_info->idx            = ctx->dbg_list_node_idx;

    list_add_tail(&dbg_info->list, &ctx->dbg_info_list_head);

    ctx->dbg_list_node_idx++;
    ctx->dbg_list_node_cnt++;

    return;
}

static void sync_release_dbg_list(sync_context *ctx)
{
    struct list_head *pos = HI_NULL;
    struct list_head *n = HI_NULL;
    sync_dbg_info_status   *dbg_info = HI_NULL;

    if (list_empty(&ctx->dbg_info_list_head)) {
        return;
    }

    list_for_each_safe(pos, n, &ctx->dbg_info_list_head) {
        dbg_info = list_entry(pos, sync_dbg_info_status, list);
        list_del(pos);
        osal_kfree(HI_ID_SYNC, dbg_info);
    }

    INIT_LIST_HEAD(&ctx->dbg_info_list_head);
    ctx->dbg_list_node_idx = 0;
    ctx->dbg_list_node_cnt = 0;

    return;
}

void sync_proc_debug_info(sync_context *ctx, sync_dbg_info_mode info_type, void *param)
{
    if (info_type == SYNC_DBG_INFO_SYS_INIT) {
        (void)osal_sem_init(&g_sync_dbg_info_mtx, 1);
        return;
    } else if (info_type == SYNC_DBG_INFO_SYS_DEINIT) {
        osal_sem_destory(&g_sync_dbg_info_mtx);
        return;
    }

    DRV_SYNC_LOCK(&g_sync_dbg_info_mtx);

    switch (info_type) {
        case SYNC_DBG_INFO_LIST_INITIAL: {
            ctx->dbg_list_node_idx = 0;
            ctx->dbg_list_node_cnt = 0;

            INIT_LIST_HEAD(&ctx->dbg_info_list_head);
            break;
        }

        case SYNC_DBG_INFO_LIST_SIZE: {
            if (param == HI_NULL_PTR) {
                HI_ERR_SYNC("para is null, sync_log_depth need less than 600\n");
                break;
            }

            if (*(hi_u32 *)param > SYNC_DBG_LIST_MAX_LEN) {
                HI_ERR_SYNC("para is invalid, sync_log_depth need less than 600\n");
                break;
            }

            ctx->dbg_list_max_len = *(hi_u32 *)param;
            sync_release_dbg_list(ctx);
            break;
        }

        case SYNC_DBG_INFO_LIST_RELEASE: {
            sync_release_dbg_list(ctx);
            break;
        }

        case SYNC_DBG_INFO_LIST_VIDADD: {
            sync_add_vid_dbg_info(ctx);
            break;
        }

        default :
            break;
    }

    DRV_SYNC_UNLOCK(&g_sync_dbg_info_mtx);
    return;
}
#endif

hi_u32 sync_get_sys_time(void)
{
    hi_u64 sys_time;
    sys_time = osal_div_u64(osal_sched_clock(), TIME_RATE * TIME_RATE);

    return (hi_u32)sys_time;
}

static hi_u32 sync_get_sys_time_cost(hi_u32 last_sys_time)
{
    hi_u32   cur_sys_time;
    hi_u32   delta;

    cur_sys_time = sync_get_sys_time();
    if (cur_sys_time > last_sys_time) {
        delta = cur_sys_time - last_sys_time;
    } else {
        delta = (SYS_TIME_MAX - last_sys_time) + 1 + cur_sys_time;
    }

    return delta;
}

hi_u32  sync_get_local_time(sync_context *ctx, sync_chan_type chn)
{
    hi_u32 cur_local_time = SYNC_INVALID_PTS;
    hi_u32 cost_sys_time;

    /* In TPLAY Mode, we need video channel localtime, ignore others channel */
    if ((ctx->cur_status == SYNC_STATUS_TPLAY) && (SYNC_CHAN_VID != chn)) {
        return SYNC_INVALID_PTS;
    }

    if (SYNC_CHAN_AUD == chn) {
        if (!ctx->is_aud_local_time) {
            return SYNC_INVALID_PTS;
        }

        cost_sys_time = sync_get_sys_time_cost(ctx->aud_last_sys_time);
        cur_local_time = ctx->aud_last_local_time + cost_sys_time ;
    } else if (SYNC_CHAN_VID == chn) {
        if (!ctx->is_vid_local_time) {
            return SYNC_INVALID_PTS;
        }

        cost_sys_time = sync_get_sys_time_cost(ctx->vid_last_sys_time);
        cur_local_time = ctx->vid_last_local_time + cost_sys_time;
    } else if (SYNC_CHAN_PCR == chn) {
        if ((!ctx->pcr_sync_info.is_pcr_first_come)
            && (SYNC_SCR_ADJUST_MAX == ctx->pcr_sync_info.pcr_adjust_type)) {
            return SYNC_INVALID_PTS;
        }

        if (!ctx->pcr_sync_info.is_pcr_local_time) {
            return SYNC_INVALID_PTS;
        }

        cost_sys_time = sync_get_sys_time_cost(ctx->pcr_sync_info.pcr_last_sys_time);

        /* adjust the diff between pcr and sys time */ /* CNCommont: 校正PCR与本地时钟不匹配引起的误差 */
        if (ctx->pcr_sync_info.pcr_gradient <= PERCENT_MAX_VALUE + PCR_GRADIENT_ERR_VALUE &&
            ctx->pcr_sync_info.pcr_gradient >= PERCENT_MAX_VALUE - PCR_GRADIENT_ERR_VALUE) {
            cost_sys_time = cost_sys_time * PERCENT_MAX_VALUE / ctx->pcr_sync_info.pcr_gradient;
        }

        cur_local_time = ctx->pcr_sync_info.pcr_last_local_time + cost_sys_time;
    }
#ifdef HI_AVPLAY_SCR_SUPPORT
    else if (SYNC_CHAN_SCR == chn) {
        if (!ctx->is_scr_init) {
            return SYNC_INVALID_PTS;
        }

        cost_sys_time = sync_get_sys_time_cost(ctx->scr_last_sys_time);
        cur_local_time = ctx->scr_last_local_time + cost_sys_time;
    }
#endif

    if (cur_local_time > PCR_TIME_MAX) {
        cur_local_time -= PCR_TIME_MAX;
    }

    return cur_local_time;
}

static void sync_set_local_time(sync_context *ctx, sync_chan_type chn, hi_u32 time)
{
    if (SYNC_CHAN_AUD == chn) {
        ctx->aud_last_sys_time = sync_get_sys_time();
        ctx->aud_last_local_time = time;
        ctx->is_aud_local_time = HI_TRUE;
    } else if (SYNC_CHAN_VID == chn) {
        ctx->vid_last_sys_time = sync_get_sys_time();
        ctx->vid_last_local_time = time;
        ctx->is_vid_local_time = HI_TRUE;
    } else if (SYNC_CHAN_PCR == chn) {
        ctx->pcr_sync_info.pcr_last_sys_time = sync_get_sys_time();
        ctx->pcr_sync_info.pcr_last_local_time = time;
        ctx->pcr_sync_info.is_pcr_local_time = HI_TRUE;
    }
#ifdef HI_AVPLAY_SCR_SUPPORT
    else if (SYNC_CHAN_SCR == chn) {
        ctx->scr_last_sys_time = sync_get_sys_time();
        ctx->scr_last_local_time = time;
    }
#endif

    return;
}

hi_s32  sync_start(hi_handle sync)
{
    sync_context  *ctx = HI_NULL;

    SYNC_CHECK_ID(sync);

    ctx = sync_info_ctx_get(sync);
    ctx->presync_start_sys_time = sync_get_sys_time();

    /* if timeout of presync is zero or sync adjust is disabled, disable presync */
    if ((!ctx->attr.pre_sync_timeout) ||
        (ctx->attr.sync_ref == SYNC_REF_NONE)) {
        ctx->presync_end_sys_time = ctx->presync_start_sys_time;
        ctx->is_presync_finish = HI_TRUE;
        ctx->is_buf_fund_finish = HI_TRUE;
    }

    /* no sync,then no buffund and audio resync */
    if (ctx->attr.sync_ref == SYNC_REF_NONE) {
        ctx->is_aud_resync = HI_FALSE;
        ctx->is_aud_rebuf_fund = HI_FALSE;
    }

    return HI_SUCCESS;
}

hi_s32  sync_pause(hi_u32 id)
{
    sync_context  *ctx = HI_NULL;

    SYNC_CHECK_ID(id);

    ctx = sync_info_ctx_get(id);

    ctx->pcr_sync_info.pcr_pause_local_time = sync_get_local_time(ctx, SYNC_CHAN_PCR);
    ctx->aud_pause_local_time = sync_get_local_time(ctx, SYNC_CHAN_AUD);
    ctx->vid_pause_local_time = sync_get_local_time(ctx, SYNC_CHAN_VID);

#ifdef HI_AVPLAY_SCR_SUPPORT
    ctx->scr_pause_local_time = sync_get_local_time(ctx, SYNC_CHAN_SCR);
#endif
    return HI_SUCCESS;
}

hi_s32  sync_resume(hi_u32 id)
{
    sync_context  *ctx = HI_NULL;

    SYNC_CHECK_ID(id);

    ctx = sync_info_ctx_get(id);
    /* if resume after reset, the local time won't be reset */
    if (ctx->pcr_sync_info.pcr_pause_local_time != SYNC_INVALID_PTS) {
        sync_set_local_time(ctx, SYNC_CHAN_PCR, ctx->pcr_sync_info.pcr_pause_local_time);
    }

    if (ctx->aud_pause_local_time != SYNC_INVALID_PTS) {
        sync_set_local_time(ctx, SYNC_CHAN_AUD, ctx->aud_pause_local_time);
    }

    if (ctx->vid_pause_local_time != SYNC_INVALID_PTS) {
        sync_set_local_time(ctx, SYNC_CHAN_VID, ctx->vid_pause_local_time);
    }

#ifdef HI_AVPLAY_SCR_SUPPORT
    if (ctx->scr_pause_local_time != SYNC_INVALID_PTS) {
        sync_set_local_time(ctx, SYNC_CHAN_SCR, ctx->scr_pause_local_time);
    }
#endif
    return HI_SUCCESS;
}

hi_s32 sync_get_time(hi_u32 id, hi_u32 *local_time, hi_u32 *play_time)
{
    sync_context   *ctx = HI_NULL;
    hi_u32         aud_local_time = 0;
    hi_u32         vid_local_time = 0;
    hi_u32         pcr_local_time = 0;

    SYNC_CHECK_ID(id);

    ctx = sync_info_ctx_get(id);
    if (ctx->cur_status == SYNC_STATUS_PAUSE) {
        aud_local_time = ctx->aud_pause_local_time;
        vid_local_time = ctx->vid_pause_local_time;
        pcr_local_time = ctx->pcr_sync_info.pcr_pause_local_time;
    } else {
        aud_local_time = sync_get_local_time(ctx, SYNC_CHAN_AUD);
        vid_local_time = sync_get_local_time(ctx, SYNC_CHAN_VID);
        pcr_local_time = sync_get_local_time(ctx, SYNC_CHAN_PCR);
    }

    if (aud_local_time <= PCR_TIME_MAX) {
        *local_time = aud_local_time;
        *play_time = sync_abs((hi_s32)(aud_local_time - ctx->aud_first_valid_pts));
    } else if (vid_local_time <= PCR_TIME_MAX) {
        *local_time = vid_local_time;
        *play_time = sync_abs((hi_s32)(vid_local_time - ctx->vid_first_valid_pts));
    } else if(pcr_local_time <= PCR_TIME_MAX) {
        *local_time = pcr_local_time;
        *play_time = sync_abs((hi_s32)(pcr_local_time - ctx->pcr_sync_info.pcr_first));
    } else {
        *local_time = SYNC_INVALID_PTS;
        *play_time = 0;
    }

    return HI_SUCCESS;
}

static void sync_reprocess_diff_time(sync_context *ctx, sync_stat_param *param)
{
    if (param->pcr_local_time == SYNC_INVALID_PTS) {
        param->aud_pcr_diff = 0;
        param->vid_pcr_diff = 0;

        if ((param->aud_local_time == SYNC_INVALID_PTS) || (param->vid_local_time == SYNC_INVALID_PTS)) {
            param->vid_aud_diff = 0;
        } else {
            param->vid_aud_diff = param->vid_local_time - param->aud_local_time +
                ctx->attr.vid_pts_adjust - ctx->attr.aud_pts_adjust;
        }
    } else {
        if ((param->aud_local_time == SYNC_INVALID_PTS) && (param->vid_local_time == SYNC_INVALID_PTS)) {
            param->vid_aud_diff = 0;
            param->aud_pcr_diff = 0;
            param->vid_pcr_diff = 0;
        } else if (param->aud_local_time == SYNC_INVALID_PTS) {
            param->vid_aud_diff = 0;
            param->aud_pcr_diff = 0;
            param->vid_pcr_diff = param->vid_local_time - param->pcr_local_time + ctx->attr.vid_pts_adjust;
        } else if (param->vid_local_time == SYNC_INVALID_PTS) {
            param->vid_aud_diff = 0;
            param->vid_pcr_diff = 0;
            param->aud_pcr_diff = param->aud_local_time - param->pcr_local_time + ctx->attr.aud_pts_adjust;
        } else {
            param->vid_aud_diff = param->vid_local_time - param->aud_local_time +
                ctx->attr.vid_pts_adjust - ctx->attr.aud_pts_adjust;
            param->vid_pcr_diff = param->vid_local_time - param->pcr_local_time + ctx->attr.vid_pts_adjust;
            param->aud_pcr_diff = param->aud_local_time - param->pcr_local_time + ctx->attr.aud_pts_adjust;
        }
    }

    return;
}

static void sync_calc_difftime(sync_context *ctx, sync_chan_type chn)
{
    hi_u32 cur_sys_time;
    hi_u32 pcr_local_time;
    hi_u32 aud_local_time;
    hi_u32 vid_local_time;
    hi_s32 aud_pcr_diff;
    hi_s32 vid_pcr_diff;
    hi_s32 vid_aud_diff;
    hi_s32 default_diff;

    hi_u32 scr_local_time;
    hi_s32 aud_scr_diff;
    hi_s32 vid_scr_diff;

    cur_sys_time = sync_get_sys_time();

    pcr_local_time = sync_get_local_time(ctx, SYNC_CHAN_PCR);
    aud_local_time = sync_get_local_time(ctx, SYNC_CHAN_AUD);
    vid_local_time = sync_get_local_time(ctx, SYNC_CHAN_VID);
    scr_local_time = sync_get_local_time(ctx, SYNC_CHAN_SCR);

    default_diff = (ctx->attr.start_region.vid_plus_time + ctx->attr.start_region.vid_negative_time) / 2;

    aud_pcr_diff = aud_local_time - pcr_local_time + ctx->attr.aud_pts_adjust;
    vid_pcr_diff = vid_local_time - pcr_local_time + ctx->attr.vid_pts_adjust;
    vid_aud_diff = vid_local_time - aud_local_time + ctx->attr.vid_pts_adjust - ctx->attr.aud_pts_adjust;

    aud_scr_diff = aud_local_time - scr_local_time + ctx->attr.aud_pts_adjust;
    vid_scr_diff = vid_local_time - scr_local_time + ctx->attr.vid_pts_adjust;

    if (ctx->attr.sync_ref == SYNC_REF_PCR) {
        if (ctx->pcr_sync_info.pcr_adjust_type != SYNC_AUD_ADJUST_SCR) {
            sync_stat_param param;
            (hi_void)memset_s(&param, sizeof(sync_stat_param), 0, sizeof(sync_stat_param));
            param.aud_local_time = (hi_s64)aud_local_time;
            param.vid_local_time = (hi_s64)vid_local_time;
            param.pcr_local_time = (hi_s64)pcr_local_time;
            sync_reprocess_diff_time(ctx, &param);
            vid_aud_diff = (hi_s32)param.vid_aud_diff;
            vid_pcr_diff = (hi_s32)param.vid_pcr_diff;
            aud_pcr_diff = (hi_s32)param.aud_pcr_diff;
        } else {
            if ((aud_local_time == SYNC_INVALID_PTS) || (vid_local_time == SYNC_INVALID_PTS)) {
                vid_aud_diff = default_diff;
            }
        }
    } else if (ctx->attr.sync_ref == SYNC_REF_AUDIO) {
        if ((aud_local_time == SYNC_INVALID_PTS) || (vid_local_time == SYNC_INVALID_PTS)) {
            vid_aud_diff = default_diff;
        }
    } else if (ctx->attr.sync_ref == SYNC_REF_SCR) {
        if (scr_local_time == SYNC_INVALID_PTS) {
            vid_scr_diff = default_diff;
            aud_scr_diff = default_diff;
        }
    }

    if (scr_local_time != SYNC_INVALID_PTS) {
        sync_set_local_time(ctx, SYNC_CHAN_SCR, scr_local_time);
    }

    /* if AudScrDiff and VidScrDiff are both too large, we reinit scr */
    if ((sync_abs(aud_scr_diff) > SCR_DISCARD_THRESHOLD) && (sync_abs(vid_scr_diff) > SCR_DISCARD_THRESHOLD)) {
        ctx->is_scr_init = HI_FALSE;
    }

    HI_INFO_VSYNC(chn, "cur_sys_time %d aud_last_sys_time %d aud_local_time %d aud_last_pts %d, \n \
        aud_last_buf_time %3d vid_local_time %d vid_last_pts %d vid_aud_diff %d\n",
        cur_sys_time, ctx->aud_last_sys_time, aud_local_time, ctx->aud_last_pts,
        ctx->aud_last_buf_time, vid_local_time, ctx->vid_last_pts, vid_aud_diff);

    HI_INFO_ASYNC(chn, "cur_sys_time %d Aaud_last_sys_time %d aud_local_time %d aud_last_pts %d, \n \
        aud_last_buf_time %3d vid_local_time %d vid_last_pts %d vid_aud_diff %d\n",
        cur_sys_time, ctx->aud_last_sys_time, aud_local_time, ctx->aud_last_pts,
        ctx->aud_last_buf_time, vid_local_time, ctx->vid_last_pts, vid_aud_diff);

    if (SYNC_REF_PCR == ctx->attr.sync_ref) {
        HI_INFO_VSYNC(chn, "pcr_local_time %d  aud_pcr_diff %d vid_pcr_diff %d vid_aud_diff %d\n",
            pcr_local_time, aud_pcr_diff, vid_pcr_diff, vid_aud_diff);
    }

#ifdef HI_AVPLAY_SCR_SUPPORT
    if (SYNC_REF_SCR == ctx->attr.sync_ref) {
        HI_INFO_VSYNC(chn, "scr_local_time %d  aud_scr_diff %d vid_scr_diff %d vid_aud_diff %d\n",
            scr_local_time, aud_scr_diff, vid_scr_diff, vid_aud_diff);
    }
#endif

    ctx->pcr_sync_info.last_aud_pcr_diff = ctx->pcr_sync_info.aud_pcr_diff;
    ctx->pcr_sync_info.last_vid_pcr_diff = ctx->pcr_sync_info.vid_pcr_diff;
    ctx->pcr_sync_info.aud_pcr_diff = aud_pcr_diff;
    ctx->pcr_sync_info.vid_pcr_diff = vid_pcr_diff;

    ctx->last_vid_aud_diff = ctx->vid_aud_diff;
    ctx->vid_aud_diff = vid_aud_diff;

    ctx->aud_scr_diff = aud_scr_diff;
    ctx->vid_scr_diff = vid_scr_diff;

    return;
}

static void sync_presync_target_init(sync_context *ctx, sync_chan_type chn)
{
    if ((chn == SYNC_CHAN_VID) && (!ctx->is_vid_presync_target_init)) {
        if (ctx->vid_info.pts == SYNC_INVALID_PTS) {
            HI_INFO_VSYNC(SYNC_CHAN_VID, "presync vid pts is -1\n");
            ctx->vid_opt.proc = SYNC_PROC_DISCARD;
            return;
        } else {
            ctx->vid_presync_target_time = ctx->vid_info.pts;
            ctx->is_vid_presync_target_init = HI_TRUE;
        }
    }

    if ((chn == SYNC_CHAN_AUD) && (!ctx->is_aud_presync_target_init)) {
        if (ctx->aud_info.pts == SYNC_INVALID_PTS) {
            HI_INFO_ASYNC(SYNC_CHAN_AUD, "PreSync AudFrame src_pts = -1\n");
            ctx->aud_opt.proc = SYNC_PROC_DISCARD;
            return;
        } else {
            ctx->aud_presync_target_time = ctx->aud_info.pts;
            ctx->is_aud_presync_target_init = HI_TRUE;
        }
    }

    if (ctx->is_vid_presync_target_init && ctx->is_aud_presync_target_init) {
        if (ctx->vid_presync_target_time > ctx->aud_presync_target_time) {
            if (ctx->is_use_ext_presync_target && ctx->ext_presync_target_time > ctx->vid_presync_target_time) {
                ctx->presync_target = SYNC_CHAN_EXT;
                ctx->presync_target_time = ctx->ext_presync_target_time;
            } else {
                ctx->presync_target = SYNC_CHAN_VID;
                ctx->presync_target_time = ctx->vid_presync_target_time;
            }
        } else {
            if (ctx->is_use_ext_presync_target && (ctx->ext_presync_target_time > ctx->aud_presync_target_time)) {
                ctx->presync_target = SYNC_CHAN_EXT;
                ctx->presync_target_time = ctx->ext_presync_target_time;
            } else {
                ctx->presync_target = SYNC_CHAN_AUD;
                ctx->presync_target_time = ctx->aud_presync_target_time;
            }
        }

        ctx->is_use_ext_presync_target = HI_FALSE;
        ctx->is_presync_target_init = HI_TRUE;

        HI_INFO_ASYNC(SYNC_CHAN_AUD, "PreSync Target %d Vid pts %d Aud pts %d\n",
                      ctx->presync_target, ctx->vid_presync_target_time, ctx->aud_presync_target_time);
        HI_INFO_VSYNC(SYNC_CHAN_VID, "PreSync Target%d Vid pts %d Aud pts %d\n",
                      ctx->presync_target, ctx->vid_presync_target_time, ctx->aud_presync_target_time);
    } else {
        if (chn == SYNC_CHAN_VID) {
            ctx->vid_opt.proc = SYNC_PROC_BLOCK;
        } else if(chn == SYNC_CHAN_AUD) {
            ctx->aud_opt.proc = SYNC_PROC_BLOCK;
        }

        return;
    }

    return;
}

static hi_bool sync_check_pcr_timeout(sync_context *ctx)
{
    hi_u32 cost_sys_time;

    cost_sys_time = sync_get_sys_time_cost(ctx->pcr_sync_info.pcr_sync_start_sys_time);
    return (cost_sys_time >= PCR_TIMEOUTMS) ? HI_TRUE : HI_FALSE;
}

static hi_bool sync_check_aud_timeout(sync_context *ctx)
{
    hi_u32 cost_sys_time;

    cost_sys_time = sync_get_sys_time_cost(ctx->presync_start_sys_time);
    return (cost_sys_time >= AUD_TIMEOUTMS) ? HI_TRUE : HI_FALSE;
}

static void sync_check_vid_pts_valid(sync_context *ctx, sync_vid_info *info)
{
    hi_s32 delta = 0;

    if (info->pts == SYNC_INVALID_PTS) {
        HI_WARN_SYNC("Vid pts is invalid\n");
        ctx->vid_pts_series_cnt = 0;
    } else if (info->pts < ctx->vid_last_pts) {
        HI_WARN_SYNC("Vid pts <= Vid vid_last_pts %d\n", ctx->vid_last_pts);
        ctx->vid_pts_series_cnt = 0;
    } else { /* pts jump too much */
        delta = info->pts - ctx->vid_last_pts;
        if (delta > VID_PTS_GAP) {
            HI_WARN_SYNC("Vid delta %d > VID_PTS_GAP %d\n", delta, VID_PTS_GAP);
            ctx->vid_pts_series_cnt = 0;
        }
    }

    info->is_pts_valid = HI_FALSE;
    ctx->vid_pts_series_cnt++;

    if (ctx->vid_pts_series_cnt >= PTS_SERIES_COUNT) {
        info->is_pts_valid = HI_TRUE;
    }

    HI_INFO_SYNC("vid is_pts_valid:%d, pts:%d\n", info->is_pts_valid, info->pts);
    return;
}

static hi_void sync_check_aud_pts_valid(sync_context *ctx, sync_aud_info *info)
{
    hi_s32 delta = 0;

    if (info->pts == SYNC_INVALID_PTS) {
        HI_WARN_SYNC("Aud pts == -1 invalid\n");
        ctx->aud_pts_series_cnt = 0;
    } else if (info->pts < ctx->aud_last_pts) {
        /* The same audio frame may be sent to sync twice for repeating. So info->pts == ctx->aud_last_pts maybe */
        HI_WARN_SYNC("Aud pts < Aud LstPts %d\n", ctx->aud_last_pts);
        ctx->aud_pts_series_cnt = 0;
    } else if (info->is_aud_tplay == HI_FALSE) {
        delta = info->pts - ctx->aud_last_pts;
        /* frame_time is approximate value */
        if (delta > AUD_PTS_GAP) {
            HI_WARN_SYNC("Aud PtsDelta %d > AUD_PTS_GAP %d\n", delta, AUD_PTS_GAP);
            ctx->aud_pts_series_cnt = 0;
        }
    }

    info->is_pts_valid = HI_FALSE;
    ctx->aud_pts_series_cnt++;

    if (ctx->aud_pts_series_cnt >= PTS_SERIES_COUNT) {
        info->is_pts_valid = HI_TRUE;
    }

    HI_INFO_SYNC("aud is_pts_valid:%d, pts:%d\n", info->is_pts_valid, info->pts);
    return;
}

static void sync_presync_pcr(sync_context *ctx, sync_chan_type chn, hi_bool *is_sync_finish)
{
    hi_s32 aud_pcr_diff;
    hi_s32 vid_pcr_diff;
    hi_s32 vid_aud_diff;
    hi_u32 pcr_local_time;
    hi_u32 vid_local_time;
    hi_u32 aud_local_time;

    pcr_local_time = sync_get_local_time(ctx, SYNC_CHAN_PCR);
    vid_local_time = ctx->vid_info.pts - ctx->vid_info.delay_time;
    aud_local_time = ctx->aud_info.pts - ctx->aud_info.buf_time;

    aud_pcr_diff = aud_local_time + ctx->attr.aud_pts_adjust - pcr_local_time;
    vid_pcr_diff = vid_local_time + ctx->attr.vid_pts_adjust - pcr_local_time;
    vid_aud_diff = (vid_local_time + ctx->attr.vid_pts_adjust) - (aud_local_time + ctx->attr.aud_pts_adjust);

    ctx->pcr_sync_info.vid_pcr_diff = vid_pcr_diff;
    ctx->pcr_sync_info.aud_pcr_diff = aud_pcr_diff;
    ctx->vid_aud_diff = vid_aud_diff;

    if ((pcr_local_time == SYNC_INVALID_PTS) || ((aud_pcr_diff > 0) && (vid_pcr_diff > 0))) {
        if (chn == SYNC_CHAN_VID) {
            ctx->vid_opt.proc = SYNC_PROC_BLOCK;
        } else if (chn == SYNC_CHAN_AUD) {
            ctx->aud_opt.proc = SYNC_PROC_BLOCK;
        }

        *is_sync_finish = HI_FALSE;
        HI_WARN_SYNC("pcr pre sync, AudPcr %d, VidPcr %d, VidAud:%d,\n", aud_pcr_diff, vid_pcr_diff, vid_aud_diff);
        return;
    }

    if ((aud_pcr_diff < 0) || (vid_pcr_diff < 0)) {
        if (((aud_pcr_diff + PCR_SYNC_MAX_DELTA > 0) &&
            (vid_pcr_diff + PCR_SYNC_MAX_DELTA > 0)) &&
            (ctx->pcr_sync_info.pcr_delta + aud_pcr_diff + PCR_SYNC_MAX_DELTA > 0) &&
            (ctx->pcr_sync_info.pcr_delta + vid_pcr_diff + PCR_SYNC_MAX_DELTA > 0)) {
            /* adjust pcr to this one which is more behind */
            ctx->pcr_sync_info.pcr_delta += (vid_aud_diff > 0) ? aud_pcr_diff : vid_pcr_diff;
        }

        *is_sync_finish = HI_TRUE;
        HI_WARN_SYNC("pcr pre sync, pcr_delta %d\n", ctx->pcr_sync_info.pcr_delta);
        return;
    }

    return;
}

static void sync_presync_aud_lead_adjust(sync_context *ctx)
{
    hi_s32      vid_aud_diff;
    hi_s32      vid_local_time;
    hi_s32      aud_local_time;
    hi_bool     is_presync_pcr_finish = HI_FALSE;

    vid_local_time = ctx->vid_info.pts - ctx->vid_info.delay_time;
    aud_local_time = ctx->aud_presync_target_time - ctx->aud_info.buf_time;

    vid_aud_diff = (vid_local_time + ctx->attr.vid_pts_adjust) - (aud_local_time + ctx->attr.aud_pts_adjust);
    ctx->vid_aud_diff = vid_aud_diff;

    /* the difference between video and audio is too large */
    if (vid_aud_diff < (-VID_LAG_DISCARD_THRESHOLD)) {
        ctx->presync_end_sys_time = sync_get_sys_time();
        ctx->is_presync_finish = HI_TRUE;
        ctx->vid_opt.proc = SYNC_PROC_CONTINUE;

        HI_INFO_VSYNC(SYNC_CHAN_VID, "PreSync Giveup vid_aud_diff %d > VID_LAG_DISCARD_THRESHOLD %d\n",
                      vid_aud_diff, VID_LAG_DISCARD_THRESHOLD);
        hi_drv_stat_event(HI_STAT_EVENT_PRE_SYNC, 0);
        return;
    }

    if (vid_aud_diff >= ctx->attr.start_region.vid_negative_time / 2) {
        if (SYNC_REF_PCR == ctx->attr.sync_ref) {
            if (SYNC_PCR_ADJUST_SCR == ctx->pcr_sync_info.pcr_adjust_type) {
                sync_presync_pcr(ctx, SYNC_CHAN_VID, &is_presync_pcr_finish);

                if (!is_presync_pcr_finish) {
                    return;
                }
            } else {
                ctx->pcr_sync_info.pcr_adjust_type = SYNC_AUD_ADJUST_SCR;    /* pcr timeout or pcr doesn't come */
            }
        }

        ctx->presync_end_sys_time = sync_get_sys_time();
        ctx->is_presync_finish = HI_TRUE;
        ctx->vid_opt.proc = SYNC_PROC_CONTINUE;

        HI_INFO_ASYNC(SYNC_CHAN_AUD, "PreSync Ok vid_aud_diff %d\n", vid_aud_diff);
        HI_INFO_VSYNC(SYNC_CHAN_VID, "PreSync Ok vid_aud_diff %d\n", vid_aud_diff);

        hi_drv_stat_event(HI_STAT_EVENT_PRE_SYNC, 0);
    } else {
        ctx->vid_opt.proc = SYNC_PROC_DISCARD;
    }

    return;
}

static void sync_presync_ext_mode_adjust(sync_context *ctx, sync_chan_type chn)
{
    hi_s32 vid_target_diff;
    hi_s32 aud_target_diff;

    if ((SYNC_CHAN_VID == chn) || (SYNC_CHAN_AUD == chn)) {
        vid_target_diff = ctx->vid_info.pts - ctx->vid_info.delay_time - ctx->presync_target_time;
        aud_target_diff = ctx->aud_info.pts - ctx->presync_target_time;

        HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_target_diff %d, aud_target_diff %d\n", vid_target_diff, aud_target_diff);
        HI_INFO_VSYNC(SYNC_CHAN_AUD, "vid_target_diff %d, aud_target_diff %d\n", vid_target_diff, aud_target_diff);

        if ((vid_target_diff > ctx->attr.start_region.vid_negative_time) &&
            (aud_target_diff > ctx->attr.start_region.vid_negative_time)) {
            ctx->presync_end_sys_time = sync_get_sys_time();
            ctx->is_presync_finish = HI_TRUE;
            ctx->vid_opt.proc = SYNC_PROC_CONTINUE;
            ctx->aud_opt.proc = SYNC_PROC_CONTINUE;

            hi_drv_stat_event(HI_STAT_EVENT_PRE_SYNC, 0);
            return;
        } else {
            if (vid_target_diff < ctx->attr.start_region.vid_negative_time) {
                ctx->vid_opt.proc = SYNC_PROC_DISCARD;
            } else {
                ctx->vid_opt.proc = SYNC_PROC_BLOCK;
            }

            if (aud_target_diff < ctx->attr.start_region.vid_negative_time) {
                ctx->aud_opt.proc = SYNC_PROC_DISCARD;
            } else {
                ctx->aud_opt.proc = SYNC_PROC_BLOCK;
            }
        }
    }

    return;
}

static void sync_presync_vid_lead_adjust(sync_context *ctx)
{
    hi_s32      vid_aud_diff;
    hi_s32      vid_local_time;
    hi_s32      aud_local_time;
    hi_bool     is_presync_pcr_finish = HI_FALSE;

    vid_local_time = ctx->vid_presync_target_time - ctx->vid_info.delay_time;
    aud_local_time = ctx->aud_info.pts - ctx->aud_info.buf_time;

    vid_aud_diff = (vid_local_time + ctx->attr.vid_pts_adjust) - (aud_local_time + ctx->attr.aud_pts_adjust);
    ctx->vid_aud_diff = vid_aud_diff;
    /* the difference between video and audio is too large */
    if (vid_aud_diff > VID_LEAD_DISCARD_THRESHOLD) {
        ctx->presync_end_sys_time = sync_get_sys_time();
        ctx->is_presync_finish = HI_TRUE;
        ctx->aud_opt.proc = SYNC_PROC_CONTINUE;

        HI_INFO_ASYNC(SYNC_CHAN_AUD, "PreSync vid_aud_diff %d > PreSyncTimeoutMs %d\n",
            vid_aud_diff, ctx->attr.pre_sync_timeout);
        HI_INFO_VSYNC(SYNC_CHAN_VID, "PreSync vid_aud_diff %d > PreSyncTimeoutMs %d\n",
            vid_aud_diff, ctx->attr.pre_sync_timeout);

        hi_drv_stat_event(HI_STAT_EVENT_PRE_SYNC, 0);
        return;
    }

    if (vid_aud_diff <= ctx->attr.start_region.vid_plus_time / 2) {
        if (SYNC_REF_PCR == ctx->attr.sync_ref) {
            if (SYNC_PCR_ADJUST_SCR == ctx->pcr_sync_info.pcr_adjust_type) {
                sync_presync_pcr(ctx, SYNC_CHAN_AUD, &is_presync_pcr_finish);

                if (!is_presync_pcr_finish) {
                    return;
                }
            } else {
                ctx->pcr_sync_info.pcr_adjust_type = SYNC_AUD_ADJUST_SCR;    /* pcr timeout or pcr doesn't come */
            }
        }

        ctx->presync_end_sys_time = sync_get_sys_time();
        ctx->is_presync_finish = HI_TRUE;
        ctx->aud_opt.proc = SYNC_PROC_CONTINUE;
        hi_drv_stat_event(HI_STAT_EVENT_PRE_SYNC, 0);

        HI_INFO_ASYNC(SYNC_CHAN_AUD, "PreSync Ok vid_aud_diff %d\n", vid_aud_diff);
        HI_INFO_VSYNC(SYNC_CHAN_VID, "PreSync Ok vid_aud_diff %d\n", vid_aud_diff);
    } else {
        ctx->aud_opt.proc = SYNC_PROC_DISCARD;
    }

    return;
}

static void sync_presync(sync_context *ctx, sync_chan_type chn)
{
    hi_u32 cost_sys_time;

    cost_sys_time = sync_get_sys_time_cost(ctx->presync_start_sys_time);

    /* do not do presync if video or audio is disable */
    if ((!ctx->is_aud_enable) || (!ctx->is_vid_enable)) {
        ctx->presync_end_sys_time = sync_get_sys_time();
        ctx->is_presync_finish = HI_TRUE;
        ctx->vid_opt.proc = SYNC_PROC_CONTINUE;
        ctx->aud_opt.proc = SYNC_PROC_CONTINUE;

        hi_drv_stat_event(HI_STAT_EVENT_PRE_SYNC, 0);
        return;
    }

    /* presync timeout */
    if (cost_sys_time >= ctx->attr.pre_sync_timeout) {
        ctx->presync_end_sys_time = sync_get_sys_time();
        ctx->is_presync_finish = HI_TRUE;
        ctx->vid_opt.proc = SYNC_PROC_CONTINUE;
        ctx->aud_opt.proc = SYNC_PROC_CONTINUE;

        hi_drv_stat_event(HI_STAT_EVENT_PRE_SYNC, 0);

        HI_INFO_ASYNC(SYNC_CHAN_AUD, "PreSync TimeOut %d aud_buf_time %d AudFrameNum %d VidDelayTime %d\n",
                      cost_sys_time, ctx->aud_info.buf_time, ctx->aud_info.frame_num, ctx->vid_info.delay_time);
        HI_INFO_VSYNC(SYNC_CHAN_VID, "PreSync TimeOut %d aud_buf_time %d AudFrameNum %d VidDelayTime %d\n",
                      cost_sys_time, ctx->aud_info.buf_time, ctx->aud_info.frame_num, ctx->vid_info.delay_time);
        return;
    }

    /* video buffer or audio buffer will be blocked */
    if ((SYNC_BUF_STATE_HIGH == ctx->cur_buf_status.vid_buf_state) ||
        (SYNC_BUF_STATE_HIGH == ctx->cur_buf_status.aud_buf_state)) {
        ctx->presync_end_sys_time = sync_get_sys_time();
        ctx->is_presync_finish = HI_TRUE;
        ctx->vid_opt.proc = SYNC_PROC_CONTINUE;
        ctx->aud_opt.proc = SYNC_PROC_CONTINUE;

        hi_drv_stat_event(HI_STAT_EVENT_PRE_SYNC, 1);

        HI_INFO_ASYNC(SYNC_CHAN_AUD, "PreSync BufBlock Aud %d Vid %d\n",
                      ctx->cur_buf_status.aud_buf_state, ctx->cur_buf_status.vid_buf_state);
        HI_INFO_VSYNC(SYNC_CHAN_VID, "PreSync BufBlock Aud %d Vid %d\n",
                      ctx->cur_buf_status.aud_buf_state, ctx->cur_buf_status.vid_buf_state);
        return;
    }

    /* prepare presync target */
    if (!ctx->is_presync_target_init) {
        sync_presync_target_init(ctx, chn);

        /* presync target is not ready */
        if (!ctx->is_presync_target_init) {
            return;
        }
    }

    if (SYNC_CHAN_EXT == ctx->presync_target) {
        sync_presync_ext_mode_adjust(ctx, chn);
    } else if (chn == ctx->presync_target) {
        if (SYNC_CHAN_VID == chn) {
            ctx->vid_opt.proc = SYNC_PROC_BLOCK;
            ctx->vid_aud_diff = ctx->vid_presync_target_time - ctx->aud_info.pts;
        } else {
            ctx->aud_opt.proc = SYNC_PROC_BLOCK;
            ctx->vid_aud_diff = ctx->vid_info.pts - ctx->aud_presync_target_time;
        }
    } else {
        /* audio wait for video */
        if (chn == SYNC_CHAN_VID) {
            sync_presync_aud_lead_adjust(ctx);
        } else if (chn == SYNC_CHAN_AUD) { /* video wait for audio */
            sync_presync_vid_lead_adjust(ctx);
        }
    }

    return;
}

static void sync_buf_fund(sync_context *ctx)
{
    hi_u32 cost_sys_time;

    cost_sys_time = sync_get_sys_time_cost(ctx->presync_end_sys_time);
    /* cumulation timeout */
    if (cost_sys_time > BUF_FUND_TIMEOUT) {
        ctx->buf_fund_end_sys_time = sync_get_sys_time();
        ctx->is_buf_fund_finish = HI_TRUE;
        ctx->vid_opt.proc = SYNC_PROC_CONTINUE;
        ctx->aud_opt.proc = SYNC_PROC_CONTINUE;

        hi_drv_stat_event(HI_STAT_EVENT_BUF_READY, 2);
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "BufFund TimeOut %d aud_buf_time %d AudFrameNum %d VidDelayTime %d\n",
                      cost_sys_time, ctx->aud_info.buf_time, ctx->aud_info.frame_num, ctx->vid_info.delay_time);
        HI_INFO_VSYNC(SYNC_CHAN_VID, "BufFund TimeOut %d aud_buf_time %d AudFrameNum %d VidDelayTime %d\n",
                      cost_sys_time, ctx->aud_info.buf_time, ctx->aud_info.frame_num, ctx->vid_info.delay_time);

        return;
    }

    /* video or audio buffer will be blocked */
    if ((SYNC_BUF_STATE_HIGH == ctx->cur_buf_status.vid_buf_state) ||
        (SYNC_BUF_STATE_HIGH == ctx->cur_buf_status.aud_buf_state)) {
        ctx->buf_fund_end_sys_time = sync_get_sys_time();
        ctx->is_buf_fund_finish = HI_TRUE;
        ctx->vid_opt.proc = SYNC_PROC_CONTINUE;
        ctx->aud_opt.proc = SYNC_PROC_CONTINUE;

        hi_drv_stat_event(HI_STAT_EVENT_BUF_READY, 3);

        HI_INFO_ASYNC(SYNC_CHAN_AUD, "BufFund BufBlock Aud %d Vid %d\n",
                      ctx->cur_buf_status.aud_buf_state, ctx->cur_buf_status.vid_buf_state);
        HI_INFO_VSYNC(SYNC_CHAN_VID, "BufFund BufBlock Aud %d Vid %d\n",
                      ctx->cur_buf_status.aud_buf_state, ctx->cur_buf_status.vid_buf_state);

        return;
    }

    if (ctx->is_vid_enable && ctx->is_aud_enable) {
        if ((ctx->aud_info.buf_time + ctx->aud_info.frame_time >= AO_TRACK_AIP_START_LATENCYMS) ||
            (ctx->aud_info.frame_num >= SYNC_AUD_BUF_NORMAL_FRMNUM)) {
            ctx->buf_fund_end_sys_time = sync_get_sys_time();
            ctx->is_buf_fund_finish = HI_TRUE;
            ctx->vid_opt.proc = SYNC_PROC_CONTINUE;
            ctx->aud_opt.proc = SYNC_PROC_CONTINUE;

            hi_drv_stat_event(HI_STAT_EVENT_BUF_READY, 1);
        } else {
            ctx->vid_opt.proc = SYNC_PROC_BLOCK;
            ctx->aud_opt.proc = SYNC_PROC_PLAY;
        }

        HI_INFO_ASYNC(SYNC_CHAN_AUD, "buf fund Ok %d aud buf_time %d aud frame_num %d vid delay_time %d\n",
                      cost_sys_time, ctx->aud_info.buf_time, ctx->aud_info.frame_num, ctx->vid_info.delay_time);
        HI_INFO_VSYNC(SYNC_CHAN_VID, "buf fund Ok %d aud buf_time %d aud frame_num %d vid delay_time %d\n",
                      cost_sys_time, ctx->aud_info.buf_time, ctx->aud_info.frame_num, ctx->vid_info.delay_time);
    } else if (ctx->is_vid_enable && (!ctx->is_aud_enable)) {
        ctx->buf_fund_end_sys_time = sync_get_sys_time();
        ctx->is_buf_fund_finish = HI_TRUE;
        ctx->vid_opt.proc = SYNC_PROC_CONTINUE;

        hi_drv_stat_event(HI_STAT_EVENT_BUF_READY, 1);
        HI_INFO_VSYNC(SYNC_CHAN_VID, "BufFund Ok\n");
    } else if ((!ctx->is_vid_enable) && ctx->is_aud_enable) {
        if ((ctx->aud_info.buf_time + ctx->aud_info.frame_time >= AO_TRACK_AIP_START_LATENCYMS) ||
            (ctx->aud_info.frame_num >= SYNC_AUD_BUF_NORMAL_FRMNUM)) {
            ctx->buf_fund_end_sys_time = sync_get_sys_time();
            ctx->is_buf_fund_finish = HI_TRUE;
            ctx->aud_opt.proc = SYNC_PROC_CONTINUE;

            hi_drv_stat_event(HI_STAT_EVENT_BUF_READY, 1);
            HI_INFO_ASYNC(SYNC_CHAN_AUD, "BufFund Ok\n");
        } else {
            ctx->aud_opt.proc = SYNC_PROC_PLAY;
        }
    }

    return;
}

static void sync_aud_resync(sync_context *ctx)
{
    hi_u32             vid_local_time;
    hi_s32             vid_aud_diff;
    hi_u32             cost_sys_time;

    cost_sys_time = sync_get_sys_time_cost(ctx->presync_start_sys_time);
    /* resync timeout */
    if (cost_sys_time >= AUD_RESYNC_TIMEOUT) {
        ctx->presync_end_sys_time = sync_get_sys_time();
        ctx->is_aud_resync = HI_FALSE;
        ctx->aud_opt.proc = SYNC_PROC_CONTINUE;

        HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud resync cost time: %d, timeout. aud buf_time: %d, frame_num: %d\n",
                      cost_sys_time, ctx->aud_info.buf_time, ctx->aud_info.frame_num);
        return;
    }

    /* buffer blocked */
    if (SYNC_BUF_STATE_HIGH == ctx->cur_buf_status.aud_buf_state) {
        ctx->presync_end_sys_time = sync_get_sys_time();
        ctx->is_aud_resync = HI_FALSE;
        ctx->aud_opt.proc = SYNC_PROC_CONTINUE;

        HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud resync buf block\n");
        return;
    }

    /* discard the frame if pts is -1 */
    if (ctx->aud_info.pts == SYNC_INVALID_PTS) {
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud resync aud pts is -1\n");
        ctx->aud_opt.proc = SYNC_PROC_DISCARD;
        return;
    }

    vid_local_time = sync_get_local_time(ctx, SYNC_CHAN_VID);
    vid_aud_diff = vid_local_time - ctx->aud_info.pts + AO_TRACK_AIP_START_LATENCYMS;

    /* The difference is too large */
    if (sync_abs(vid_aud_diff) > AUD_RESYNC_ADJUST_THRESHOLD) {
        ctx->is_aud_resync = HI_FALSE;
        ctx->aud_opt.proc = SYNC_PROC_CONTINUE;
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "in aud resync vid_aud_diff %d > AUD_RESYNC_ADJUST_THRESHOLD %d\n",
                      vid_aud_diff, AUD_RESYNC_ADJUST_THRESHOLD);
        return;
    }

    /* adjust into sync start range */
    if ((vid_aud_diff <= ctx->attr.start_region.vid_plus_time) &&
        (vid_aud_diff >= ctx->attr.start_region.vid_negative_time) &&
        (sync_abs(vid_aud_diff) <= ctx->aud_info.frame_time)) {
        ctx->presync_end_sys_time = sync_get_sys_time();
        ctx->is_aud_resync = HI_FALSE;
        ctx->aud_opt.proc = SYNC_PROC_CONTINUE;
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "is_aud_resync Ok vid_aud_diff %d\n", vid_aud_diff);

        return;
    }

    if (vid_aud_diff > 0) {
        ctx->aud_opt.proc = SYNC_PROC_DISCARD;
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud resync is discard, vid_aud_diff: %d\n", vid_aud_diff);
    } else {
        ctx->aud_opt.proc = SYNC_PROC_BLOCK;
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud resync is block, vid_aud_diff: %d\n", vid_aud_diff);
    }

    return;
}

static void sync_aud_rebuf_fund(sync_context *ctx)
{
    hi_u32 cost_sys_time;

    cost_sys_time = sync_get_sys_time_cost(ctx->presync_end_sys_time);
    /* timeout */
    if (cost_sys_time > BUF_FUND_TIMEOUT) {
        ctx->is_aud_rebuf_fund = HI_FALSE;
        ctx->aud_opt.proc = SYNC_PROC_CONTINUE;

        HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud rebuf fund cost: %d, timeout. aud buf_time: %d, frame_num: %d\n",
                      cost_sys_time, ctx->aud_info.buf_time, ctx->aud_info.frame_num);
        return;
    }

    /* buffer blocked */
    if (ctx->cur_buf_status.aud_buf_state == SYNC_BUF_STATE_HIGH) {
        ctx->is_aud_rebuf_fund = HI_FALSE;
        ctx->aud_opt.proc = SYNC_PROC_CONTINUE;
        HI_INFO_ASYNC(SYNC_CHAN_AUD, ("aud rebuf fund is buf block\n"));
        return;
    }

    if (ctx->aud_info.buf_time + ctx->aud_info.frame_time >= AO_TRACK_AIP_START_LATENCYMS) {
        ctx->is_aud_rebuf_fund = HI_FALSE;
        ctx->aud_opt.proc = SYNC_PROC_CONTINUE;
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud rebuf fund is Ok\n");
    } else {
        ctx->aud_opt.proc = SYNC_PROC_PLAY;
    }

    return;
}

static void sync_proc_vid_repeat_cnt(sync_context *ctx, sync_proc_type proc_type, hi_s32 vid_target_diff)
{
#ifdef CHIP_TYPE_hi3716mv430
    hi_u32 repeat_count = 0;

    if (SYNC_PROC_TYPE_SMOOTH == proc_type) {
        ctx->vid_opt.repeat = 1;
        ctx->vid_repeat_cnt++;
        return;
    }

    if ((SYNC_PROC_TYPE_QUICK == proc_type) && (ctx->is_vid_normal_play == HI_FALSE)) {
        ctx->vid_opt.proc = SYNC_PROC_BLOCK;
        HI_WARN_SYNC("--BLOCK FIRST FRAME---\n");
        return;
    }

    if ((vid_target_diff >= ctx->vid_info.frame_time) && (ctx->vid_info.frame_time != 0)) {
        repeat_count = vid_target_diff / ctx->vid_info.frame_time;
        repeat_count = (repeat_count > VID_REPEAT_MAX_COUNT) ? VID_REPEAT_MAX_COUNT : repeat_count;

        ctx->vid_repeat_cnt += repeat_count;
        ctx->vid_opt.repeat = repeat_count;
        HI_INFO_SYNC("vid_target_diff: %d, frame_time: %d, repeat_count: %d\n",
                     vid_target_diff, ctx->vid_info.frame_time, repeat_count);
    }

    return;
#else
    ctx->vid_repeat_cnt++;
#endif

    return;
}

/* refer to audio and video is ahead of audio */
static void sync_aud_sync_vid_lead_adjust(sync_context *ctx, sync_chan_type chn, hi_s32 vid_aud_diff)
{
    if (vid_aud_diff > ctx->attr.novel_region.vid_plus_time) {
        /* there is enough audio data in ao buffer */
        if ((ctx->aud_info.buf_time >= (AO_PCM_DF_UNSTALL_THD_FRAMENUM * ctx->aud_info.frame_time)) &&
            (ctx->aud_info.frame_num >= AO_PCM_MAX_UNSTALL_THD_FRAMENUM)) {
            /* TODO: sdk  can we discard audio data when playing */
            ctx->aud_opt.proc = SYNC_PROC_DISCARD;

            HI_INFO_ASYNC(chn, "Vid Lead Aud %d, Discard\n", vid_aud_diff);
        }
    } else {
        ctx->aud_opt.proc = SYNC_PROC_PLAY;
        ctx->aud_opt.speed_adjust = SYNC_AUD_SPEED_ADJUST_NORMAL;
        HI_INFO_ASYNC(chn, "Vid Lead Aud %d, Play\n", vid_aud_diff);
    }

    /* there is space in video buffer */
    /* TODO: sdk change VidBlockFlag to VidFullFlag */
    /* is_buf_time_need_ctrl is true in the live. In the live broadcast, need to detect the buffer waterline.
     * If the waterline exceeds normal, do't repeat the video frame. If the video frame is repeated,
     * it may cause buffer overflow when buffer waterline is hight in the live broadcast.
    */
    if ((ctx->cur_buf_status.is_buf_time_need_ctrl == HI_FALSE) ||
        (ctx->cur_buf_status.vid_buf_state < SYNC_BUF_STATE_HIGH)) {
        if (vid_aud_diff > ctx->attr.novel_region.vid_plus_time) {
            if (ctx->attr.novel_region.smooth_play || ctx->vid_first_pts == SYNC_INVALID_PTS) {
                /* repeat one frame every VID_SMOOTH_REPEAT_INTERVAL frame */
                if (!(ctx->vid_repeat_play_cnt % VID_SMOOTH_REPEAT_INTERVAL)) {
                    ctx->vid_opt.proc = SYNC_PROC_REPEAT;

                    if (chn == SYNC_CHAN_VID) {
                        (void)sync_proc_vid_repeat_cnt(ctx, SYNC_PROC_TYPE_SMOOTH, vid_aud_diff);
                    }

                    HI_INFO_VSYNC(chn, "Vid Novel Lead Aud %d Smooth, Repeat\n", vid_aud_diff);
                } else {
                    ctx->vid_opt.proc = SYNC_PROC_PLAY;
                    HI_INFO_VSYNC(chn, "Vid Novel Lead Aud %d Smooth, Play\n", vid_aud_diff);
                }

                if (chn == SYNC_CHAN_VID) {
                    ctx->vid_repeat_play_cnt++;
                }
            } else {
                ctx->vid_opt.proc = SYNC_PROC_REPEAT;

                if (chn == SYNC_CHAN_VID) {
                    (void)sync_proc_vid_repeat_cnt(ctx, SYNC_PROC_TYPE_QUICK, vid_aud_diff);
                }

                HI_INFO_VSYNC(chn, "Vid Novel Lead Aud %d, Repeat\n", vid_aud_diff);
            }
        } else {
            if (ctx->attr.start_region.smooth_play || ctx->vid_first_pts == SYNC_INVALID_PTS) {
                /* repeat one frame every VID_SMOOTH_REPEAT_INTERVAL frame */
                if (!(ctx->vid_repeat_play_cnt % VID_SMOOTH_REPEAT_INTERVAL)) {
                    ctx->vid_opt.proc = SYNC_PROC_REPEAT;

                    if (chn == SYNC_CHAN_VID) {
                        (void)sync_proc_vid_repeat_cnt(ctx, SYNC_PROC_TYPE_SMOOTH, vid_aud_diff);
                    }

                    HI_INFO_VSYNC(chn, "Vid Lead Aud %d Smooth, Repeat\n", vid_aud_diff);
                } else {
                    ctx->vid_opt.proc = SYNC_PROC_PLAY;
                    HI_INFO_VSYNC(chn, "Vid Lead Aud %d Smooth, Play\n", vid_aud_diff);
                }

                if (chn == SYNC_CHAN_VID) {
                    ctx->vid_repeat_play_cnt++;
                }
            } else {
                /* If smoothy is closed,video should be continues to play when audio buf is empty */
                if ((ctx->cur_buf_status.aud_buf_state == SYNC_BUF_STATE_EMPTY) &&
                    (ctx->attr.start_region.smooth_play == HI_FALSE)) {
                    ctx->vid_opt.proc = SYNC_PROC_PLAY;
                    HI_WARN_SYNC("+++AUDIO BUF IS EMPTY!Video play normally!\n");
                } else {
                    ctx->vid_opt.proc = SYNC_PROC_REPEAT;

                    if (chn == SYNC_CHAN_VID) {
                        (void)sync_proc_vid_repeat_cnt(ctx, SYNC_PROC_TYPE_QUICK, vid_aud_diff);
                    }

                    HI_INFO_VSYNC(chn, "Vid Lead Aud %d, Repeat\n", vid_aud_diff);
                }
            }
        }
    } else { /* the video buffer reach high waterline */
        ctx->vid_opt.proc = SYNC_PROC_PLAY;
        HI_INFO_VSYNC(chn, "Vid Lead Aud %d BufBlock, Play\n", vid_aud_diff);
    }

    return;
}

/* refer to audio and video is behind audio */
static void sync_aud_sync_vid_lag_adjust(sync_context *ctx, sync_chan_type chn, hi_s32 vid_aud_diff)
{
    ctx->aud_opt.proc = SYNC_PROC_PLAY;

    if (ctx->cur_buf_status.vid_buf_state == SYNC_BUF_STATE_EMPTY) {
        ctx->vid_opt.proc = SYNC_PROC_PLAY;

        if (ctx->cur_buf_status.aud_buf_state != SYNC_BUF_STATE_HIGH) {
            if (ctx->vid_aud_diff < -300) {
                ctx->aud_opt.speed_adjust = SYNC_AUD_SPEED_ADJUST_MUTE_REPEAT;
                HI_INFO_ASYNC(chn, "Vid Buf Low, vid_aud_diff: %d, Aud Mute Repeat\n", vid_aud_diff);
            } else {
                ctx->aud_opt.speed_adjust = SYNC_AUD_SPEED_ADJUST_DOWN;
                HI_INFO_ASYNC(chn, "Vid Buf Low, vid_aud_diff: %d, Aud Speed Down\n", vid_aud_diff);
            }
        }

        HI_INFO_VSYNC(chn, "Vid Buf Low, vid_aud_diff: %d, Vid Play\n", vid_aud_diff);
        return;
    } else if (ctx->cur_buf_status.vid_buf_state == SYNC_BUF_STATE_LOW) {
        if (ctx->cur_buf_status.aud_buf_state != SYNC_BUF_STATE_HIGH) {
            ctx->aud_opt.speed_adjust = SYNC_AUD_SPEED_ADJUST_DOWN;
            HI_INFO_ASYNC(chn, "Vid Buf Low, vid_aud_diff: %d, Aud Speed Down\n", vid_aud_diff);
        }
    }

    if (vid_aud_diff < ctx->attr.novel_region.vid_negative_time) {
        if (ctx->attr.novel_region.smooth_play) {
            /* discard one frame in every VID_SMOOTH_DISCARD_INTERVAL frame */
            if (!(ctx->vid_discard_play_cnt % VID_SMOOTH_DISCARD_INTERVAL)) {
                ctx->vid_opt.proc = SYNC_PROC_DISCARD;

                if (chn == SYNC_CHAN_VID) {
                    ctx->vid_discard_cnt++;
                }

                HI_INFO_VSYNC(chn, "Vid Novel Lag Aud %d Smooth, Discard\n", vid_aud_diff);
            } else {
                ctx->vid_opt.proc = SYNC_PROC_PLAY;

                HI_INFO_VSYNC(chn, "Vid Novel Lag Aud %d Smooth, Play\n", vid_aud_diff);
            }

            if (chn == SYNC_CHAN_VID) {
                ctx->vid_discard_play_cnt++;
            }
        } else {
            /* discard time before vdec */
            ctx->vid_opt.vdec_discard_time = sync_abs(vid_aud_diff);
            ctx->vid_opt.proc = SYNC_PROC_DISCARD;

            if (chn == SYNC_CHAN_VID) {
                ctx->vid_discard_cnt++;
            }

            HI_INFO_VSYNC(chn, "Vid Novel Lag Aud %d, Discard\n", vid_aud_diff);
        }
    } else {
        if (ctx->attr.start_region.smooth_play) {
            /* discard one frame in every VID_SMOOTH_DISCARD_INTERVAL frame */
            if (!(ctx->vid_discard_play_cnt % VID_SMOOTH_DISCARD_INTERVAL)) {
                ctx->vid_opt.proc = SYNC_PROC_DISCARD;

                if (chn == SYNC_CHAN_VID) {
                    ctx->vid_discard_cnt++;
                }

                HI_INFO_VSYNC(chn, "Vid Lag Aud %d Smooth, Discard\n", vid_aud_diff);
            } else {
                ctx->vid_opt.proc = SYNC_PROC_PLAY;
                HI_INFO_VSYNC(chn, "Vid Lag Aud %d Smooth, Play\n", vid_aud_diff);
            }

            if (chn == SYNC_CHAN_VID) {
                ctx->vid_discard_play_cnt++;
            }
        } else {
            /* discard time before vdec */
            ctx->vid_opt.vdec_discard_time = sync_abs(vid_aud_diff);
            ctx->vid_opt.proc = SYNC_PROC_DISCARD;

            if (chn == SYNC_CHAN_VID) {
                ctx->vid_discard_cnt++;
            }

            HI_INFO_VSYNC(chn, "Vid Lag Aud %d, Discard\n", vid_aud_diff);
        }
    }

    return;
}

/* adjust referring to audio */
static void sync_aud_sync_adjust(sync_context *ctx, sync_chan_type chn, hi_s32 vid_aud_diff)
{
    if (!ctx->is_vid_sync_adjust) {
        /* already in the start region */
        if ((vid_aud_diff <= ctx->attr.start_region.vid_plus_time) &&
            (vid_aud_diff >= ctx->attr.start_region.vid_negative_time)) {
            ctx->vid_opt.proc = SYNC_PROC_PLAY;
            ctx->aud_opt.proc = SYNC_PROC_PLAY;
            ctx->sync_adjust_delay_frm_cnt = 0;

            HI_INFO_ASYNC(chn, "Vid Sync Aud, VidAudDiff %d, Play\n", vid_aud_diff);
            HI_INFO_VSYNC(chn, "Vid Sync Aud, VidAudDiff %d, Play\n", vid_aud_diff);

            return;
        } else {
            if ((ctx->sync_adjust_delay_frm_cnt < SYNC_ADJUST_DELAY_CNT) &&
                (ctx->is_vid_normal_play == HI_TRUE)) {
                ctx->sync_adjust_delay_frm_cnt++;
                HI_INFO_SYNC("adjust delay cnt:%d VidAudDiff %d\n",
                             ctx->sync_adjust_delay_frm_cnt, ctx->vid_aud_diff);
                return;
            }

            ctx->sync_adjust_delay_frm_cnt = 0;
            ctx->is_vid_sync_adjust = HI_TRUE;
        }
    }

    if ((vid_aud_diff <= ctx->attr.start_region.vid_plus_time) &&
        (vid_aud_diff >= ctx->attr.start_region.vid_negative_time)) {
        if ((ctx->is_use_stop_region == HI_TRUE && sync_abs(vid_aud_diff) <= ctx->vid_info.frame_time) ||
            (ctx->is_use_stop_region == HI_FALSE)) {
            ctx->is_vid_sync_adjust = HI_FALSE;
            ctx->vid_opt.proc = SYNC_PROC_PLAY;
            ctx->aud_opt.proc = SYNC_PROC_PLAY;

            ctx->pcr_sync_info.is_pcr_aud_sync_ok = HI_TRUE;
            ctx->pcr_sync_info.is_pcr_vid_sync_ok = HI_TRUE;

            HI_INFO_ASYNC(chn, "Vid vid_aud_diff Aud, VidAudDiff %d, Play\n", vid_aud_diff);
            HI_INFO_VSYNC(chn, "Vid vid_aud_diff Aud, VidAudDiff %d, Play\n", vid_aud_diff);

            return;
        }
    }

    /* video is ahead of audio and the difference is too large */
    if (vid_aud_diff > VID_LEAD_DISCARD_THRESHOLD) {
        ctx->vid_opt.proc = SYNC_PROC_PLAY;
        ctx->aud_opt.proc = SYNC_PROC_PLAY;

        HI_INFO_ASYNC(chn, "vid_aud_diff %d > VID_LEAD_DISCARD_THRESHOLD, Play\n", vid_aud_diff);
        HI_INFO_VSYNC(chn, "vid_aud_diff %d > VID_LEAD_DISCARD_THRESHOLD, Play\n", vid_aud_diff);

        return;
    }

    /* video is behind audio and the difference is too large */
    if (vid_aud_diff < (-VID_LAG_DISCARD_THRESHOLD)) {
        ctx->vid_opt.proc = SYNC_PROC_PLAY;
        ctx->aud_opt.proc = SYNC_PROC_PLAY;

        HI_INFO_ASYNC(chn, "vid_aud_diff %d < VID_LAG_DISCARD_THRESHOLD, Play\n", vid_aud_diff);
        HI_INFO_VSYNC(chn, "vid_aud_diff %d < VID_LAG_DISCARD_THRESHOLD, Play\n", vid_aud_diff);

        return;
    }

    /* video is ahead of audio */
    if (vid_aud_diff > 0) {
        sync_aud_sync_vid_lead_adjust(ctx, chn, vid_aud_diff);
    } else { /* video is behind audio */
        sync_aud_sync_vid_lag_adjust(ctx, chn, vid_aud_diff);
    }

    return;
}

#ifdef HI_AVPLAY_SCR_SUPPORT
static void sync_scr_sync_aud_adjust(sync_context *ctx, hi_s32 vid_aud_diff, hi_s32 aud_scr_diff, hi_s32 vid_scr_diff)
{
    /* aud is behind scr and aud_scr_diff is in novel region */
    if ((aud_scr_diff >= ctx->attr.novel_region.vid_negative_time) &&
        (aud_scr_diff <= ctx->attr.start_region.vid_negative_time)) {
        ctx->aud_opt.proc = SYNC_PROC_DISCARD;
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud_scr_diff: %d, DISCARD\n", aud_scr_diff);
    } else if (aud_scr_diff < ctx->attr.novel_region.vid_negative_time) {
        /* aud is behind scr and aud_scr_diff is out of novel region */
        ctx->aud_opt.proc = SYNC_PROC_DISCARD;
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud_scr_diff: %d, DISCARD\n", aud_scr_diff);
    } else if ((aud_scr_diff >= ctx->attr.start_region.vid_plus_time) &&
               (aud_scr_diff <= ctx->attr.novel_region.vid_plus_time)) {
        /* aud is ahead scr and aud_scr_diff is in novel region */
        ctx->aud_opt.proc = SYNC_PROC_REPEAT;
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud_scr_diff: %d, REPEAT\n", aud_scr_diff);
    } else if (aud_scr_diff > ctx->attr.novel_region.vid_plus_time) {
        /* aud is ahead scr and aud_scr_diff is out of novel region */
        ctx->aud_opt.proc = SYNC_PROC_REPEAT;
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud_scr_diff: %d, REPEAT\n", aud_scr_diff);
    }

    return;
}
#endif

#ifdef HI_AVPLAY_SCR_SUPPORT
static void sync_scr_sync_vid_lag_adjust(sync_context *ctx, hi_s32 vid_scr_diff)
{
    /* vid is behind scr and VidScrDiff is in novel region */
    if (vid_scr_diff >= ctx->attr.novel_region.vid_negative_time) {
        if (ctx->attr.start_region.smooth_play) {
            if (0 == ctx->vid_discard_play_cnt % VID_SMOOTH_DISCARD_INTERVAL) {
                ctx->vid_opt.proc = SYNC_PROC_DISCARD;
                ctx->vid_discard_cnt++;
                HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_scr_diff: %d, SMOOTH DISCARD\n", vid_scr_diff);
            } else {
                ctx->vid_opt.proc = SYNC_PROC_PLAY;
                HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_scr_diff: %d, SMOOTH Play\n", vid_scr_diff);
            }

            ctx->vid_discard_play_cnt++;
        } else {
            ctx->vid_opt.proc = SYNC_PROC_DISCARD;

            HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_scr_diff: %d, DISCARD\n", vid_scr_diff);
        }
    } else if (vid_scr_diff < ctx->attr.novel_region.vid_negative_time) {
        /* vid is behind scr and vid_scr_diff is out of novel region */
        if (ctx->attr.novel_region.smooth_play) {
            if (0 == ctx->vid_discard_play_cnt % VID_SMOOTH_DISCARD_INTERVAL) {
                ctx->vid_opt.proc = SYNC_PROC_DISCARD;

                ctx->vid_discard_cnt++;

                HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_scr_diff: %d, SMOOTH DISCARD\n", vid_scr_diff);
            } else {
                ctx->vid_opt.proc = SYNC_PROC_PLAY;
                HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_scr_diff: %d, SMOOTH Play\n", vid_scr_diff);
            }

            ctx->vid_discard_play_cnt++;
        } else {
            ctx->vid_opt.proc = SYNC_PROC_DISCARD;
            ctx->vid_discard_cnt++;
            HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_scr_diff: %d, DISCARD\n", vid_scr_diff);
        }
    }

    return;
}
#endif

#ifdef HI_AVPLAY_SCR_SUPPORT
static void sync_scr_sync_vid_lead_adjust(sync_context *ctx, hi_s32 vid_scr_diff)
{
    /* vid is ahead scr and VidScrDiff is in novel region */
    if (vid_scr_diff <= ctx->attr.novel_region.vid_plus_time) {
        if (ctx->attr.start_region.smooth_play) {
            if (0 == ctx->vid_repeat_play_cnt % VID_SMOOTH_REPEAT_INTERVAL) {
                ctx->vid_opt.proc = SYNC_PROC_REPEAT;
                (void)sync_proc_vid_repeat_cnt(ctx, SYNC_PROC_TYPE_SMOOTH, vid_scr_diff);
                HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_scr_diff: %d, SMOOTH REPEAT\n", vid_scr_diff);
            } else {
                ctx->vid_opt.proc = SYNC_PROC_PLAY;
                HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_scr_diff: %d, SMOOTH Play\n", vid_scr_diff);
            }

            ctx->vid_repeat_play_cnt++;
        } else {
            ctx->vid_opt.proc = SYNC_PROC_REPEAT;
            (void)sync_proc_vid_repeat_cnt(ctx, SYNC_PROC_TYPE_QUICK, vid_scr_diff);
            HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_scr_diff: %d, REPEAT\n", vid_scr_diff);
        }
    } else if (vid_scr_diff > ctx->attr.novel_region.vid_plus_time) {
        /* vid is ahead scr and vid_scr_diff is out of novel region */
        if (ctx->attr.novel_region.smooth_play) {
            if (0 == ctx->vid_discard_play_cnt % VID_SMOOTH_DISCARD_INTERVAL) {
                ctx->vid_opt.proc = SYNC_PROC_REPEAT;
                (void)sync_proc_vid_repeat_cnt(ctx, SYNC_PROC_TYPE_SMOOTH, vid_scr_diff);
                HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_scr_diff: %d, SMOOTH REPEAT\n", vid_scr_diff);
            } else {
                ctx->vid_opt.proc = SYNC_PROC_PLAY;
                HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_scr_diff: %d, SMOOTH PLAY\n", vid_scr_diff);
            }

            ctx->vid_repeat_play_cnt++;
        } else {
            ctx->vid_opt.proc = SYNC_PROC_REPEAT;
            (void)sync_proc_vid_repeat_cnt(ctx, SYNC_PROC_TYPE_QUICK, vid_scr_diff);
            HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_scr_diff: %d, REPEAT\n", vid_scr_diff);
        }
    }

    return;
}
#endif

#ifdef HI_AVPLAY_SCR_SUPPORT
static void sync_scr_sync_vid_adjust (sync_context *ctx, hi_s32 vid_aud_diff, hi_s32 aud_scr_diff, hi_s32 vid_scr_diff)
{
    /* VidScrDiff is already in start region */
    if ((vid_scr_diff > ctx->attr.start_region.vid_negative_time) &&
        (vid_scr_diff < ctx->attr.start_region.vid_plus_time)) {
        /* VidAudDiff is already in start region */
        if ((vid_aud_diff < ctx->attr.start_region.vid_plus_time) &&
            (vid_aud_diff > ctx->attr.start_region.vid_negative_time)) {
            ctx->vid_opt.proc = SYNC_PROC_PLAY;
            HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_scr_diff: %d, Play\n", vid_scr_diff);
            return;
        }

        /* VidAudDiff is beyond start region, we adjust vid */
        /* we only need adjust one or two frames, so we ignore smooth play */
        if (aud_scr_diff > 0 && vid_scr_diff < 0) {
            ctx->vid_opt.proc = SYNC_PROC_DISCARD;
            ctx->vid_discard_cnt++;
            HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_scr_diff: %d, DISCARD\n", vid_scr_diff);
        }

        if (aud_scr_diff < 0 && vid_scr_diff > 0) {
            ctx->vid_opt.proc = SYNC_PROC_REPEAT;
            ctx->vid_repeat_cnt++;
            HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_scr_diff: %d, REPEAT\n", vid_scr_diff);
        }
    } else if (vid_scr_diff <= ctx->attr.start_region.vid_negative_time) { /* vid is behind scr */
        sync_scr_sync_vid_lag_adjust(ctx, vid_scr_diff);
    } else if (vid_scr_diff >= ctx->attr.start_region.vid_plus_time) { /* vid is ahead scr */
        sync_scr_sync_vid_lead_adjust(ctx, vid_scr_diff);
    }

    return;
}
#endif

static void sync_scr_sync_adjust(sync_context *ctx, sync_chan_type chn,
                                      hi_s32 vid_aud_diff, hi_s32 aud_scr_diff, hi_s32 vid_scr_diff)
{
#ifdef HI_AVPLAY_SCR_SUPPORT
    /* AudScrDiff or VidScrDiff is too large, give up */
    if ((sync_abs(aud_scr_diff) > SCR_DISCARD_THRESHOLD) ||
        (sync_abs(vid_scr_diff) > SCR_DISCARD_THRESHOLD)) {
        ctx->aud_opt.proc = SYNC_PROC_PLAY;
        ctx->vid_opt.proc = SYNC_PROC_PLAY;

        HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud_scr_diff %d vid_scr_diff %d > SCR_DISCARD_THRESHOLD, Play\n",
                      aud_scr_diff, vid_scr_diff);
        HI_INFO_VSYNC(SYNC_CHAN_VID, "aud_scr_diff %d vid_scr_diff %d > SCR_DISCARD_THRESHOLD, Play\n",
                      aud_scr_diff, vid_scr_diff);
        return;
    }

    if (chn == SYNC_CHAN_AUD) {
        /* already in start region */
        if ((aud_scr_diff > ctx->attr.start_region.vid_negative_time) &&
            (aud_scr_diff < ctx->attr.start_region.vid_plus_time)) {
            ctx->aud_opt.proc = SYNC_PROC_PLAY;
            HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud_scr_diff: %d, Play\n", aud_scr_diff);
        } else {
            sync_scr_sync_aud_adjust(ctx, ctx->vid_aud_diff, ctx->aud_scr_diff, ctx->vid_scr_diff);
        }
    } else if (chn == SYNC_CHAN_VID) {
        sync_scr_sync_vid_adjust(ctx, ctx->vid_aud_diff, ctx->aud_scr_diff, ctx->vid_scr_diff);
    }
#endif
    return;
}

static sync_ref_type sync_get_sync_adjust_ref_mode(const sync_context *ctx)
{
    if ((SYNC_REF_SCR == ctx->attr.sync_ref) ||
        ((SYNC_REF_AUDIO == ctx->attr.sync_ref) && (!ctx->is_aud_first_come))) {
        /* refer to scr or refer to audio but audio first frame does not come */
        return SYNC_REF_SCR;
    } else if ((SYNC_REF_AUDIO == ctx->attr.sync_ref) ||
             (((SYNC_REF_PCR == ctx->attr.sync_ref) && (!ctx->pcr_sync_info.is_pcr_first_come)) ||
             (SYNC_AUD_ADJUST_SCR == ctx->pcr_sync_info.pcr_adjust_type))) {
        /* refer to aud or refer to pcr but pcr does not come. pcr does not come and pcr timeout */
        return SYNC_REF_AUDIO;
    } else if (SYNC_REF_PCR == ctx->attr.sync_ref) {
        /* refer to pcr and received pcr */
        return SYNC_REF_PCR;
    }

    return SYNC_REF_MAX;
}

/* refer to pcr and audio is ahead of pcr */
static void sync_pcr_sync_aud_lead_adjust(sync_context *ctx, hi_s32 aud_pcr_diff)
{
    ctx->aud_opt.proc = SYNC_PROC_PLAY;

    if (SYNC_BUF_STATE_HIGH != ctx->cur_buf_status.aud_buf_state) {
        ctx->aud_opt.proc = SYNC_PROC_REPEAT;
        ctx->aud_repeat_cnt++;
        HI_INFO_SYNC(">>>>Pcr Lag Aud, aud_pcr_diff %d, Aud Repeat\n", aud_pcr_diff);
    }

    return;
}

/* refer to pcr and audio is behind pcr */
static void sync_pcr_sync_aud_lag_adjust(sync_context *ctx, hi_s32 aud_pcr_diff)
{
    ctx->aud_opt.speed_adjust = SYNC_AUD_SPEED_ADJUST_NORMAL;
    ctx->aud_opt.proc = SYNC_PROC_DISCARD;
    ctx->aud_discard_cnt++;
    HI_INFO_SYNC(">>>>Pcr Lead Aud %d, discard\n", aud_pcr_diff);

    return;
}

/* refer to pcr and video is ahead of pcr */
static void sync_pcr_sync_vid_lead_adjust(sync_context *ctx, hi_s32 vid_target_diff)
{
    /* there is space in video buffer */
    if (SYNC_BUF_STATE_HIGH != ctx->cur_buf_status.vid_buf_state) {
        if (vid_target_diff > ctx->attr.novel_region.vid_plus_time) {
            if (ctx->attr.novel_region.smooth_play || ctx->vid_first_pts == SYNC_INVALID_PTS) {
                /* repeat one frame every VID_SMOOTH_REPEAT_INTERVAL frame */
                if (!(ctx->vid_repeat_play_cnt % VID_SMOOTH_REPEAT_INTERVAL)) {
                    ctx->vid_opt.proc = SYNC_PROC_REPEAT;
                    sync_proc_vid_repeat_cnt(ctx, SYNC_PROC_TYPE_SMOOTH, vid_target_diff);
                    HI_INFO_VSYNC(SYNC_CHAN_VID, ">>>>Pcr Lag Vid %d Smooth, Repeat\n", vid_target_diff);
                } else {
                    ctx->vid_opt.proc = SYNC_PROC_PLAY;
                    HI_INFO_VSYNC(SYNC_CHAN_VID, ">>>>Pcr Lag Vid %d Smooth, Play\n", vid_target_diff);
                }

                ctx->vid_repeat_play_cnt++;
            } else {
                ctx->vid_opt.proc = SYNC_PROC_REPEAT;
                sync_proc_vid_repeat_cnt(ctx, SYNC_PROC_TYPE_QUICK, vid_target_diff);
                HI_INFO_VSYNC(SYNC_CHAN_VID, ">>>>Pcr Lag Vid %d, Repeat\n", vid_target_diff);
            }
        } else {
            if (ctx->attr.start_region.smooth_play || ctx->vid_first_pts == SYNC_INVALID_PTS) {
                /* repeat one frame every VID_SMOOTH_REPEAT_INTERVAL frame */
                if (!(ctx->vid_repeat_play_cnt % VID_SMOOTH_REPEAT_INTERVAL)) {
                    ctx->vid_opt.proc = SYNC_PROC_REPEAT;
                    sync_proc_vid_repeat_cnt(ctx, SYNC_PROC_TYPE_SMOOTH, vid_target_diff);
                    HI_INFO_VSYNC(SYNC_CHAN_VID, ">>>>Pcr Lag Vid %d Smooth, Repeat\n", vid_target_diff);
                } else {
                    ctx->vid_opt.proc = SYNC_PROC_PLAY;
                    HI_INFO_VSYNC(SYNC_CHAN_VID, ">>>>Pcr Lag Vid %d Smooth, Play\n", vid_target_diff);
                }

                ctx->vid_repeat_play_cnt++;
            } else {
                ctx->vid_opt.proc = SYNC_PROC_REPEAT;
                sync_proc_vid_repeat_cnt(ctx, SYNC_PROC_TYPE_QUICK, vid_target_diff);
                HI_INFO_VSYNC(SYNC_CHAN_VID, ">>>>Pcr Lag Vid %d, Repeat\n", vid_target_diff);
            }
        }
    } else { /* the video buffer reach high waterline */
        ctx->vid_opt.proc = SYNC_PROC_PLAY;
        HI_INFO_VSYNC(SYNC_CHAN_VID, ">>>>Pcr Lag Vid %d BufBlock, Play\n", vid_target_diff);
    }

    return;
}

/* refer to pcr and video is behind pcr */
static void sync_pcr_sync_vid_lag_adjust(sync_context *ctx, hi_s32 vid_target_diff)
{
    if (vid_target_diff < ctx->attr.novel_region.vid_negative_time) {
        if (ctx->attr.novel_region.smooth_play) {
            /* discard one frame in every VID_SMOOTH_DISCARD_INTERVAL frame */
            if (!(ctx->vid_discard_play_cnt % VID_SMOOTH_DISCARD_INTERVAL)) {
                ctx->vid_opt.proc = SYNC_PROC_DISCARD;
                ctx->vid_discard_cnt++;
                HI_INFO_VSYNC(SYNC_CHAN_VID, ">>>>Pcr Lead Vid %d Smooth, Discard\n", vid_target_diff);
            } else {
                ctx->vid_opt.proc = SYNC_PROC_PLAY;
                HI_INFO_VSYNC(SYNC_CHAN_VID, ">>>>Pcr Lead Vid %d Smooth, Play\n", vid_target_diff);
            }

            ctx->vid_discard_play_cnt++;
        } else {
            /* discard time before vdec */
            ctx->vid_opt.vdec_discard_time = sync_abs(vid_target_diff);
            ctx->vid_opt.proc = SYNC_PROC_DISCARD;
            ctx->vid_discard_cnt++;
            HI_INFO_VSYNC(SYNC_CHAN_VID, ">>>>Pcr Lead Vid %d, Discard\n", vid_target_diff);
        }
    } else {
        if (ctx->attr.start_region.smooth_play) {
            /* discard one frame in every VID_SMOOTH_DISCARD_INTERVAL frame */
            if (!(ctx->vid_discard_play_cnt % VID_SMOOTH_DISCARD_INTERVAL)) {
                ctx->vid_opt.proc = SYNC_PROC_DISCARD;
                ctx->vid_discard_cnt++;
                HI_INFO_VSYNC(SYNC_CHAN_VID, ">>>>Pcr Lead Vid %d Smooth, Discard\n", vid_target_diff);
            } else {
                ctx->vid_opt.proc = SYNC_PROC_PLAY;
                HI_INFO_VSYNC(SYNC_CHAN_VID, ">>>>Pcr Lead Vid %d Smooth, Play\n", vid_target_diff);
            }

            ctx->vid_discard_play_cnt++;
        } else {
            /* discard time before vdec */
            ctx->vid_opt.vdec_discard_time = sync_abs(vid_target_diff);
            ctx->vid_opt.proc = SYNC_PROC_DISCARD;
            ctx->vid_discard_cnt++;
            HI_INFO_VSYNC(SYNC_CHAN_VID, ">>>>Pcr Lead Vid %d, Discard\n", vid_target_diff);
        }
    }

    return;
}

static void sync_pcr_aud_adjust(sync_context *ctx, hi_s32 aud_pcr_diff)
{
    if (sync_abs(aud_pcr_diff) > PCR_SYNC_THRESHOLD) {
        ctx->aud_opt.proc = SYNC_PROC_PLAY;
        ctx->pcr_sync_info.is_pcr_aud_sync_ok = HI_FALSE;
        HI_INFO_SYNC("aud_pcr_diff is too large\n");
        return;
    }

    /* already in the stop region */
    if (sync_abs(aud_pcr_diff) < PCR_LAG_STOP_THRESHOLD) {
        ctx->aud_opt.proc = SYNC_PROC_PLAY;
        ctx->pcr_sync_info.is_pcr_aud_sync_ok = HI_TRUE;
        HI_INFO_SYNC("aud_pcr_diff sync finsh\n");
        return;
    } else {
        ctx->pcr_sync_info.is_pcr_aud_sync_ok = HI_FALSE;
    }

    if (ctx->pcr_sync_info.aud_pcr_diff > 0) {
        sync_pcr_sync_aud_lead_adjust(ctx, aud_pcr_diff);
    } else {
        sync_pcr_sync_aud_lag_adjust(ctx, aud_pcr_diff);
    }

    return;
}

static void sync_pcr_vid_adjust(sync_context *ctx, hi_s32 vid_pcr_diff)
{
    if (sync_abs(vid_pcr_diff) > PCR_SYNC_THRESHOLD) {
        ctx->vid_opt.proc = SYNC_PROC_PLAY;
        ctx->pcr_sync_info.is_pcr_vid_sync_ok = HI_FALSE;
        HI_INFO_SYNC("vid_pcr_diff is too large :%d", vid_pcr_diff);
        return;
    }

    /* already in the stop region, */
    if (sync_abs(vid_pcr_diff) < PCR_LAG_STOP_THRESHOLD) {
        ctx->vid_opt.proc = SYNC_PROC_PLAY;
        ctx->pcr_sync_info.is_pcr_vid_sync_ok = HI_TRUE;
        HI_INFO_SYNC("vid_pcr_diff sync finsh :%d", vid_pcr_diff);
        return;
    } else {
        ctx->pcr_sync_info.is_pcr_vid_sync_ok = HI_FALSE;
    }

    if (ctx->pcr_sync_info.vid_pcr_diff > 0) {
        sync_pcr_sync_vid_lead_adjust(ctx, vid_pcr_diff);
    } else {
        sync_pcr_sync_vid_lag_adjust(ctx, vid_pcr_diff);
    }

    return;
}

static void sync_pcr_vid_aud_adjust(sync_context *ctx, sync_chan_type chn)
{
    if (SYNC_CHAN_VID == chn) {
        if (ctx->vid_aud_diff > 0) {
            sync_pcr_sync_vid_lead_adjust(ctx, ctx->vid_aud_diff);
        } else {
            sync_pcr_sync_vid_lag_adjust(ctx, ctx->vid_aud_diff);
        }
    }

    if (SYNC_CHAN_AUD == chn) {
        /* need adjust aud speed */
        if (ctx->vid_aud_diff > 0) {
            ctx->aud_opt.speed_adjust = SYNC_AUD_SPEED_ADJUST_UP;
            ctx->aud_opt.proc = SYNC_PROC_PLAY;
        } else {
            ctx->aud_opt.speed_adjust = SYNC_AUD_SPEED_ADJUST_DOWN;
            ctx->aud_opt.proc = SYNC_PROC_PLAY;
        }
    }

    return;
}

static void sync_check_pcr_adjust_finish(sync_context *ctx, hi_s32 aud_pcr_diff,
                                                hi_s32 vid_pcr_diff, hi_bool *is_sync_finish)
{
    hi_s32 lag_threshold;
    hi_s32 lead_threshold;

    /* if only aud or vid is exist,make sure audpcrdiff or vidpcrdiff in -100~100 */
    if (((ctx->is_aud_first_come) && (!ctx->is_vid_first_come)) ||
        ((!ctx->is_aud_first_come) && (ctx->is_vid_first_come))) {
        lag_threshold = PCR_LAG_STOP_THRESHOLD;
        lead_threshold = PCR_LEAD_STOP_THRESHOLD;
    } else {
        lag_threshold = PCR_LAG_ADJUST_THRESHOLD;
        lead_threshold = PCR_LEAD_ADJUST_THRESHOLD;
    }

    if (!ctx->is_vid_sync_adjust) {
         /* VidAudDiff is in sync start region, vidpcr and audpcr is in sync start region, don't need to adjust */
        if ((ctx->vid_aud_diff <= ctx->attr.start_region.vid_plus_time) &&
            (ctx->vid_aud_diff >= ctx->attr.start_region.vid_negative_time) &&
            ((aud_pcr_diff <= lag_threshold) && (aud_pcr_diff + lead_threshold >= 0)) &&
            ((vid_pcr_diff <= lag_threshold) && (vid_pcr_diff + lead_threshold >= 0))) {
            ctx->vid_opt.proc = SYNC_PROC_PLAY;
            ctx->aud_opt.proc = SYNC_PROC_PLAY;

            ctx->pcr_sync_info.is_pcr_vid_sync_ok = HI_TRUE;
            ctx->pcr_sync_info.is_pcr_aud_sync_ok = HI_TRUE;
            ctx->sync_adjust_delay_frm_cnt = 0;

            *is_sync_finish = HI_TRUE;
            HI_INFO_SYNC("pcr sync finsh, vid_aud_diff %d\n", ctx->vid_aud_diff);
            return;
        }

        if (ctx->sync_adjust_delay_frm_cnt < SYNC_ADJUST_DELAY_CNT) {
            ctx->sync_adjust_delay_frm_cnt++;
            HI_INFO_SYNC("adjust delay cnt:%d vid_aud_diff %d\n", ctx->sync_adjust_delay_frm_cnt, ctx->vid_aud_diff);
            return;
        }

        ctx->is_vid_sync_adjust = HI_TRUE;
        ctx->pcr_sync_info.is_pcr_aud_sync_ok = HI_FALSE;
        ctx->pcr_sync_info.is_pcr_vid_sync_ok = HI_FALSE;
        ctx->sync_adjust_delay_frm_cnt = 0;
    }

    /* vid_aud_diff is in sync start region, vidpcr and audpcr is in sync start region, don't need to adjust */
    if ((ctx->vid_aud_diff <= ctx->attr.start_region.vid_plus_time) &&
        (ctx->vid_aud_diff >= ctx->attr.start_region.vid_negative_time) &&
        ((aud_pcr_diff <= lag_threshold) && (aud_pcr_diff + lead_threshold >= 0)) &&
        ((vid_pcr_diff <= lag_threshold) && (vid_pcr_diff + lead_threshold >= 0))) {
        if ((ctx->is_use_stop_region == HI_TRUE && sync_abs(ctx->vid_aud_diff) <= ctx->vid_info.frame_time) ||
            (ctx->is_use_stop_region == HI_FALSE)) {
            ctx->vid_opt.proc = SYNC_PROC_PLAY;
            ctx->aud_opt.proc = SYNC_PROC_PLAY;

            ctx->pcr_sync_info.is_pcr_vid_sync_ok = HI_TRUE;
            ctx->pcr_sync_info.is_pcr_aud_sync_ok = HI_TRUE;

            *is_sync_finish = HI_TRUE;
            ctx->is_vid_sync_adjust = HI_FALSE;
            HI_WARN_SYNC("pcr sync finsh, vid_aud_diff %d\n", ctx->vid_aud_diff);
            return;
        }
    }

    *is_sync_finish = HI_FALSE;
    return;
}

static void sync_pcr_start_region_adjust(sync_context *ctx, sync_chan_type Chn,
                                         hi_s32 aud_pcr_diff, hi_s32 vid_pcr_diff)
{
    hi_bool is_sync_finish = HI_FALSE;

    sync_check_pcr_adjust_finish(ctx, aud_pcr_diff, vid_pcr_diff, &is_sync_finish);

    if (HI_TRUE == is_sync_finish) {
        return;
    }

    /* adjust aud to match pcr */
    if (SYNC_CHAN_AUD == Chn) {
        sync_pcr_aud_adjust(ctx, aud_pcr_diff);
    }

    /* adjust vid to match pcr */
    if (SYNC_CHAN_VID == Chn) {
        sync_pcr_vid_adjust(ctx, vid_pcr_diff);
    }

    /* adjust aud and vid to match startregion */
    if ((HI_TRUE == ctx->pcr_sync_info.is_pcr_aud_sync_ok) &&
        (HI_TRUE == ctx->pcr_sync_info.is_pcr_vid_sync_ok)) {
        sync_pcr_vid_aud_adjust(ctx, Chn);
    }

    HI_WARN_SYNC("is_pcr_aud_sync_ok: %d, is_pcr_vid_sync_ok: %d; \
        aud_pcr_diff: %d, vid_pcr_diff: %d, vid_aud_diff: %d\n",
        ctx->pcr_sync_info.is_pcr_aud_sync_ok, ctx->pcr_sync_info.is_pcr_vid_sync_ok,
        ctx->pcr_sync_info.aud_pcr_diff, ctx->pcr_sync_info.vid_pcr_diff, ctx->vid_aud_diff);
    return;
}

static void sync_pcr_sync_adjust(sync_context *ctx, sync_chan_type Chn, hi_s32 aud_pcr_diff, hi_s32 vid_pcr_diff)
{
    if (ctx->is_aud_first_come && ctx->is_vid_first_come) {
        if ((HI_TRUE == ctx->is_pcr_rewind) ||
            (HI_TRUE == ctx->is_vid_rewind) ||
            (HI_TRUE == ctx->is_aud_rewind)) {
            ctx->vid_opt.proc = SYNC_PROC_PLAY;
            ctx->aud_opt.proc = SYNC_PROC_PLAY;
            return;
        }
    }

    /* if pcr is abnormal chenge syncRef to Aud */
    if ((sync_abs(aud_pcr_diff) > PCR_SYNC_THRESHOLD) && (sync_abs(vid_pcr_diff) > PCR_SYNC_THRESHOLD)) {
        if (sync_abs(ctx->vid_aud_diff) < PCR_SYNC_THRESHOLD) {
            ctx->pcr_sync_info.pcr_adjust_type = SYNC_AUD_ADJUST_SCR;
            ctx->vid_opt.proc = SYNC_PROC_PLAY;
            ctx->aud_opt.proc = SYNC_PROC_PLAY;

            HI_INFO_SYNC("change syncRef to aud\n");
            return;
        }
    }

    if ((!ctx->is_buf_adjust) &&
        (aud_pcr_diff + PCR_LEAD_ADJUST_THRESHOLD < 0) &&
        (vid_pcr_diff + PCR_LEAD_ADJUST_THRESHOLD < 0)) {
        if ((aud_pcr_diff + ctx->pcr_sync_info.pcr_delta + PCR_SYNC_MAX_DELTA < 0) ||
            (vid_pcr_diff + ctx->pcr_sync_info.pcr_delta + PCR_SYNC_MAX_DELTA < 0)) {
            ctx->pcr_sync_info.pcr_adjust_type = SYNC_AUD_ADJUST_SCR;
            ctx->vid_opt.proc = SYNC_PROC_PLAY;
            ctx->aud_opt.proc = SYNC_PROC_PLAY;

            HI_INFO_SYNC("change syncRef to aud\n");
            return;
        }

        /* adjust pcr to this one which is more behind */
        if (ctx->pcr_sync_info.is_pcr_adjust_delta_ok) {
            ctx->pcr_sync_info.pcr_delta += (vid_pcr_diff > aud_pcr_diff) ? aud_pcr_diff : vid_pcr_diff;
            ctx->pcr_sync_info.is_pcr_adjust_delta_ok = HI_FALSE;
            HI_INFO_SYNC("change pcr_delta %d\n", ctx->pcr_sync_info.pcr_delta);
        }

        ctx->vid_opt.proc = SYNC_PROC_PLAY;
        ctx->aud_opt.proc = SYNC_PROC_PLAY;
        return;
    }

    sync_pcr_start_region_adjust(ctx, Chn, aud_pcr_diff, vid_pcr_diff);
    return;
}

static void sync_adjust(sync_context *ctx, sync_chan_type chn)
{
    sync_ref_type  ref_mode;

    /* do not sync when is_loop_rewind is true */
    if (ctx->is_loop_rewind) {
        return;
    }

    ref_mode = sync_get_sync_adjust_ref_mode(ctx);

    switch (ref_mode) {
        case SYNC_REF_SCR: {
            sync_scr_sync_adjust(ctx, chn, ctx->vid_aud_diff, ctx->aud_scr_diff, ctx->vid_scr_diff);
            break;
        }

        case SYNC_REF_AUDIO: {
            sync_aud_sync_adjust(ctx, chn, ctx->vid_aud_diff);
            break;
        }

        case SYNC_REF_PCR: {
            sync_pcr_sync_adjust(ctx, chn, ctx->pcr_sync_info.aud_pcr_diff, ctx->pcr_sync_info.vid_pcr_diff);
            break;
        }

        default:
            HI_INFO_ASYNC(chn, "ref_mode 0x%x is not support\n", ref_mode);
            HI_INFO_VSYNC(chn, "ref_mode 0x%x is not support\n", ref_mode);
            return;
    }

    return;
}

static sync_region_stat_mode sync_check_pcr_region(sync_context *ctx, hi_s32 diff)
{
    if ((diff < PCR_LAG_ADJUST_THRESHOLD) && (diff > -PCR_LEAD_ADJUST_THRESHOLD)) {
        return SYNC_REGION_STAT_IN_START;
    } else if ((diff < PCR_DISCARD_THRESHOLD) && (diff > -PCR_DISCARD_THRESHOLD)) {
        return SYNC_REGION_STAT_IN_NOVEL;
    } else if ((diff < VID_LEAD_DISCARD_THRESHOLD) && (diff > -VID_LEAD_DISCARD_THRESHOLD)) {
        return SYNC_REGION_STAT_IN_DISCARD;
    } else {
        return SYNC_REGION_STAT_OUT_DISCARD;
    }
}

static sync_region_stat_mode sync_check_region(sync_context *ctx, hi_s32 diff)
{
    if ((diff < ctx->attr.start_region.vid_plus_time) &&
        (diff > ctx->attr.start_region.vid_negative_time)) {
        return SYNC_REGION_STAT_IN_START;
    } else if ((diff < ctx->attr.novel_region.vid_plus_time) &&
               (diff > ctx->attr.novel_region.vid_negative_time)) {
        return SYNC_REGION_STAT_IN_NOVEL;
    } else if ((diff < VID_LEAD_DISCARD_THRESHOLD) && (diff > -VID_LAG_DISCARD_THRESHOLD)) {
        return SYNC_REGION_STAT_IN_DISCARD;
    } else {
        return SYNC_REGION_STAT_OUT_DISCARD;
    }
}

static void sync_check_evt(sync_context *ctx, sync_chan_type chn)
{
    hi_u32             err_delta;
    sync_region_stat_mode last_vid_aud_stat = SYNC_REGION_STAT_MAX;
    sync_region_stat_mode cur_vid_aud_stat  = SYNC_REGION_STAT_MAX;
    sync_region_stat_mode last_vid_pcr_stat = SYNC_REGION_STAT_MAX;
    sync_region_stat_mode cur_vid_pcr_stat  = SYNC_REGION_STAT_MAX;
    sync_region_stat_mode last_aud_pcr_stat = SYNC_REGION_STAT_MAX;
    sync_region_stat_mode cur_aud_pcr_stat  = SYNC_REGION_STAT_MAX;
    hi_u32             cur_sys_time;
    hi_bool            is_state_change   = HI_FALSE;

    if (SYNC_CHAN_VID == chn) {
        err_delta = ctx->vid_info.frame_time * SYNC_PTS_JUMP_FRM_NUM;

        if (SYNC_INVALID_PTS == ctx->vid_last_pts) {
            return;
        }

        if (sync_abs(ctx->vid_info.pts - ctx->vid_last_pts) > err_delta) {
            HI_INFO_VSYNC(SYNC_CHAN_VID, "vid pts: %d, vid_last_pts: %d, vid_first_valid_pts: %d, err_delta: %d\n",
                          ctx->vid_info.pts, ctx->vid_last_pts, ctx->vid_first_valid_pts, err_delta);

            ctx->evt.is_vid_pts_jump = HI_TRUE;
            ctx->evt.vid_pts_jump_param.pts_chan = SYNC_CHAN_VID;
            ctx->evt.vid_pts_jump_param.cur_pts = ctx->vid_info.pts;
            ctx->evt.vid_pts_jump_param.cur_src_pts = ctx->vid_info.src_pts;
            ctx->evt.vid_pts_jump_param.first_pts = ctx->vid_first_pts;
            ctx->evt.vid_pts_jump_param.first_valid_pts = ctx->vid_first_valid_pts;
            ctx->evt.vid_pts_jump_param.last_pts = ctx->vid_last_pts;
            ctx->evt.vid_pts_jump_param.last_src_pts = ctx->vid_last_src_pts;

            /* pts is one of vid_first_dec_pts/vid_second_dec_pts/vid_first_valid_pts */
            if ((ctx->vid_info.src_pts == ctx->vid_first_dec_pts) ||
                (ctx->vid_info.src_pts == ctx->vid_second_dec_pts) ||
                ((ctx->vid_info.src_pts == ctx->vid_first_valid_pts) && (ctx->is_vid_first_valid_come))) {
                HI_INFO_VSYNC(SYNC_CHAN_VID, "Vid pts loop rewind, src pts %u, vid_first_dec_pts %u, \
                              vid_second_dec_pts %u, vid_first_valid_pts %u!\n",
                              ctx->vid_info.src_pts, ctx->vid_first_dec_pts,
                              ctx->vid_second_dec_pts, ctx->vid_first_valid_pts);

                ctx->evt.vid_pts_jump_param.loop_back = HI_TRUE;
                ctx->is_vid_pts_loop_rewind = HI_TRUE;
            } else {
                ctx->evt.vid_pts_jump_param.loop_back = HI_FALSE;
            }
        }
    } else if (SYNC_CHAN_AUD == chn) {
        err_delta = ctx->aud_info.frame_time * SYNC_PTS_JUMP_FRM_NUM;
        if (SYNC_INVALID_PTS == ctx->aud_last_pts) {
            return;
        }

        if (sync_abs(ctx->aud_info.pts - ctx->aud_last_pts) > err_delta) {
            HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud_last_pts %d, pts %d, u32FirstValidPts %d, err_delta %d\n",
                ctx->aud_last_pts, ctx->aud_info.pts, ctx->aud_first_valid_pts, err_delta);

            ctx->evt.is_aud_pts_jump = HI_TRUE;
            ctx->evt.aud_pts_jump_param.pts_chan = SYNC_CHAN_AUD;
            ctx->evt.aud_pts_jump_param.cur_pts = ctx->aud_info.pts;
            ctx->evt.aud_pts_jump_param.cur_src_pts = ctx->aud_info.src_pts;
            ctx->evt.aud_pts_jump_param.first_pts = ctx->aud_first_pts;
            ctx->evt.aud_pts_jump_param.first_valid_pts = ctx->aud_first_valid_pts;
            ctx->evt.aud_pts_jump_param.last_pts = ctx->aud_last_pts;
            ctx->evt.aud_pts_jump_param.last_src_pts = ctx->aud_last_src_pts;

            if (ctx->aud_info.src_pts == ctx->aud_first_valid_pts) {
                HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud pts loop rewind!\n");
                ctx->evt.aud_pts_jump_param.loop_back = HI_TRUE;
                ctx->is_aud_pts_loop_rewind = HI_TRUE;
            } else {
                ctx->evt.aud_pts_jump_param.loop_back = HI_FALSE;
            }
        }
    } else { /* pcr jump check, to add */
    }

    if (SYNC_REF_AUDIO == ctx->attr.sync_ref) {
        last_vid_aud_stat = sync_check_region(ctx, ctx->last_vid_aud_diff);
        cur_vid_aud_stat  = sync_check_region(ctx, ctx->vid_aud_diff);
        if (last_vid_aud_stat != cur_vid_aud_stat) {
            is_state_change = HI_TRUE;
        }
    } else if (SYNC_REF_PCR == ctx->attr.sync_ref) {
        last_aud_pcr_stat = sync_check_pcr_region(ctx, ctx->pcr_sync_info.last_aud_pcr_diff);
        last_vid_pcr_stat = sync_check_pcr_region(ctx, ctx->pcr_sync_info.last_vid_pcr_diff);
        last_vid_aud_stat = sync_check_region(ctx, ctx->last_vid_aud_diff);

        cur_aud_pcr_stat = sync_check_pcr_region(ctx, ctx->pcr_sync_info.aud_pcr_diff);
        cur_vid_pcr_stat = sync_check_pcr_region(ctx, ctx->pcr_sync_info.vid_pcr_diff);
        cur_vid_aud_stat = sync_check_region(ctx, ctx->vid_aud_diff);
        if ((last_aud_pcr_stat != cur_aud_pcr_stat) ||
            (last_vid_pcr_stat != cur_vid_pcr_stat) ||
            (last_vid_aud_stat != cur_vid_aud_stat)) {
            is_state_change = HI_TRUE;
        }
    }

    /* if sync status change, record this event */
    if ((HI_TRUE == is_state_change) || (HI_TRUE == ctx->is_first_sync_state_report)) {
        HI_INFO_SYNC("Sync Status change: last_vid_aud_stat %d, cur_vid_aud_stat %d!\n",
                     last_vid_aud_stat, cur_vid_aud_stat);
        ctx->is_first_sync_state_report = HI_FALSE;

        ctx->evt.is_stat_change = HI_TRUE;
        ctx->evt.stat_param.vid_aud_diff = ctx->vid_aud_diff;
        ctx->evt.stat_param.vid_pcr_diff = ctx->pcr_sync_info.vid_pcr_diff;
        ctx->evt.stat_param.aud_pcr_diff = ctx->pcr_sync_info.aud_pcr_diff;
        ctx->evt.stat_param.vid_local_time = ctx->vid_last_local_time;
        ctx->evt.stat_param.aud_local_time = ctx->aud_last_local_time;
        ctx->evt.stat_param.pcr_local_time = ctx->pcr_sync_info.pcr_last_local_time;
    }

    /* do not sync when vid or aud loopback */
    if (!ctx->is_loop_rewind && (ctx->is_aud_pts_loop_rewind || ctx->is_vid_pts_loop_rewind)) {
        ctx->is_loop_rewind = HI_TRUE;
        ctx->loop_rewind_time = sync_get_sys_time();

        HI_INFO_SYNC("Change sync ref to NONE. is_aud_pts_loop_rewind: %d, is_vid_pts_loop_rewind: %d\n",
                     ctx->is_aud_pts_loop_rewind, ctx->is_vid_pts_loop_rewind);
    }

    if (ctx->is_loop_rewind) {
        cur_sys_time = sync_get_sys_time();
        /* recover sync when timeout or vid and aud both loopback */
        if ((cur_sys_time - ctx->loop_rewind_time > PTS_LOOPBACK_TIMEOUT) ||
            (ctx->is_aud_pts_loop_rewind && ctx->is_vid_pts_loop_rewind)) {
            ctx->is_loop_rewind = HI_FALSE;
            ctx->is_aud_pts_loop_rewind = HI_FALSE;
            ctx->is_vid_pts_loop_rewind = HI_FALSE;
            HI_INFO_SYNC("Change sync ref to usr set ref, is_aud_pts_loop_rewind: %d, is_vid_pts_loop_rewind: %d\n",
                         ctx->is_aud_pts_loop_rewind, ctx->is_vid_pts_loop_rewind);
        }
    }

    return;
}

#ifdef HI_VIDEO_TB_SUPPORT
static void sync_check_tb_match_adjust(sync_context *ctx)
{
    sync_region_stat_mode region_stat;

    region_stat = sync_check_region(ctx, ctx->vid_aud_diff);
    if (region_stat == SYNC_REGION_STAT_IN_START) {
        if (ctx->is_vid_sync_adjust) {
            ctx->vid_opt.tb_adjust_type = HI_DRV_VIDEO_TB_PLAY;
            return;
        }

        /* just in start region ,vdp can repeat or discat to match TB when it's necessary */
        if (0 == ctx->vid_aud_diff) {
            ctx->vid_opt.tb_adjust_type = HI_DRV_VIDEO_TB_PLAY;
        } else if (ctx->vid_aud_diff > 0) {
            ctx->vid_opt.tb_adjust_type = HI_DRV_VIDEO_TB_REPEAT;
        } else {
            ctx->vid_opt.tb_adjust_type = HI_DRV_VIDEO_TB_DISCARD;
        }

        return ;
    } else {
        ctx->vid_opt.tb_adjust_type = HI_DRV_VIDEO_TB_PLAY;
        return;
    }
}
#endif

static hi_bool sync_is_sync_ref_none(sync_context *ctx, sync_chan_type chn)
{
    if (SYNC_REF_NONE != ctx->attr.sync_ref) {
        return HI_FALSE;
    }

    if (SYNC_CHAN_AUD == chn) {
        if (!ctx->is_aud_first_play) {
            ctx->is_aud_first_play = HI_TRUE;
            ctx->aud_first_play_time = sync_get_sys_time();
        }

        if (SYNC_BUF_STATE_EMPTY == ctx->cur_buf_status.aud_buf_state) {
            ctx->aud_opt.speed_adjust = SYNC_AUD_SPEED_ADJUST_DOWN;
        } else {
            ctx->aud_opt.speed_adjust = SYNC_AUD_SPEED_ADJUST_NORMAL;
        }

        ctx->aud_opt.proc = SYNC_PROC_PLAY;
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "--------Aud none sync Play Speed %d--------\n", ctx->aud_opt.speed_adjust);
    }

    if (SYNC_CHAN_VID == chn) {
        if (!ctx->is_vid_first_play) {
            ctx->is_vid_first_play = HI_TRUE;
            ctx->is_vid_normal_play = HI_TRUE;
            ctx->vid_first_play_time = sync_get_sys_time();
            hi_drv_stat_event(HI_STAT_EVENT_FRAME_SYNC_OK, 0);
        }

        ctx->vid_opt.proc = SYNC_PROC_PLAY;
        HI_INFO_VSYNC(SYNC_CHAN_VID, "--------Vid none sync Play--------\n");
    }

    return HI_TRUE;
}

static hi_bool sync_is_vid_presync_finish(sync_context *ctx, sync_vid_info *info, sync_vid_opt *opt)
{
    if (ctx->is_presync_finish) {
        return HI_TRUE;
    }

    sync_presync(ctx, SYNC_CHAN_VID);

    if (ctx->vid_opt.proc != SYNC_PROC_CONTINUE) {
        *opt = ctx->vid_opt;

        if (ctx->is_presync_target_init) {
            HI_INFO_VSYNC(SYNC_CHAN_VID, "--------Vid PreSync vid_aud_diff %d proc %d--------\n",
                          ctx->vid_aud_diff, opt->proc);
        }

        if (info->is_pts_valid) {
            sync_set_local_time(ctx, SYNC_CHAN_VID, (info->pts - info->delay_time));
        }

        return HI_FALSE;
    }

    if (!ctx->is_scr_init && (info->pts != SYNC_INVALID_PTS)) {
        sync_set_local_time(ctx, SYNC_CHAN_SCR, (info->pts - ctx->vid_info.delay_time));
        ctx->is_scr_init = HI_TRUE;
        ctx->scr_first_local_time = info->pts - ctx->vid_info.delay_time;
        ctx->scr_first_sys_time = sync_get_sys_time();
    }

    return HI_TRUE;
}

static hi_bool sync_is_vid_buf_fund_finish(sync_context *ctx, sync_vid_info *info, sync_vid_opt *opt)
{
    if (ctx->is_buf_fund_finish) {
        return HI_TRUE;
    }

    sync_buf_fund(ctx);

    if (ctx->vid_opt.proc != SYNC_PROC_CONTINUE) {
        *opt = ctx->vid_opt;
        HI_INFO_VSYNC(SYNC_CHAN_VID, "--------vid buf fund proc %d--------\n", opt->proc);
        return HI_FALSE;
    }

    if (ctx->is_aud_first_come) {
        sync_set_local_time(ctx, SYNC_CHAN_AUD, (ctx->aud_info.pts - ctx->aud_info.buf_time));
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud buf fund first set local time: %d\n",
                      (ctx->aud_info.pts - ctx->aud_info.buf_time));

        if (SYNC_AUD_ADJUST_SCR == ctx->pcr_sync_info.pcr_adjust_type) {
            sync_set_local_time(ctx, SYNC_CHAN_PCR,  (ctx->aud_info.pts - ctx->aud_info.buf_time));
            HI_INFO_SYNC("pcr set local time: %d  by aud pts\n", (ctx->aud_info.pts - ctx->aud_info.buf_time));
        }
    }

    sync_set_local_time(ctx, SYNC_CHAN_VID, (info->pts - ctx->vid_info.delay_time));
    HI_INFO_VSYNC(SYNC_CHAN_VID, "vid buf fund first set local time: %d\n", (info->pts - ctx->vid_info.delay_time));

    return HI_TRUE;
}

static void sync_set_vid_local_time(sync_context *ctx, sync_vid_info *info)
{
    /* if video localtime has not been initialized or there are three successive video pts,update video localtime */
    if ((info->is_pts_valid) || ((!ctx->is_vid_local_time) && (info->pts != SYNC_INVALID_PTS))) {
        sync_set_local_time(ctx, SYNC_CHAN_VID, (info->pts - ctx->vid_info.delay_time));
        HI_INFO_VSYNC(SYNC_CHAN_VID, "sync_set_vid_local_time: %d\n", (info->pts - ctx->vid_info.delay_time));
    }

    return;
}

static hi_bool sync_is_vid_buf_overflow_discard_frm(sync_context *ctx, sync_vid_opt *opt)
{
    if (!ctx->cur_buf_status.is_overflow_disc_frm) {
        return HI_FALSE;
    }

    /* discard one frame in every VID_SMOOTH_DISCARD_INTERVAL frame */
    if (!(ctx->vid_discard_play_cnt % VID_SMOOTH_DISCARD_INTERVAL)) {
        ctx->vid_opt.proc = SYNC_PROC_DISCARD;
        HI_INFO_VSYNC(SYNC_CHAN_VID, "Vid Buf overflow, Discard\n");
    } else {
        ctx->vid_opt.proc = SYNC_PROC_PLAY;
        HI_INFO_VSYNC(SYNC_CHAN_VID, "Vid Buf overflow, Play\n");
    }

    ctx->vid_discard_play_cnt++;
    *opt = ctx->vid_opt;
    HI_INFO_VSYNC(SYNC_CHAN_VID, "--------Vid Sync Proc %d--------\n", opt->proc);

    return HI_TRUE;
}

static void sync_buf_adjust(sync_context *ctx)
{
    sync_buf_status cur_stat = {0};

    if ((!ctx->is_vid_first_come) || (!ctx->is_aud_first_come)) {
        return;
    }

    if (memcpy_s(&cur_stat, sizeof(sync_buf_status), &ctx->cur_buf_status, sizeof(sync_buf_status))) {
        HI_ERR_SYNC("memcpy_s failed!\n");
        return ;
    }

    if ((!ctx->is_buf_adjust) && (!ctx->is_vid_sync_adjust)) {
        if ((cur_stat.aud_buf_time > cur_stat.buf_time_max) &&
            (cur_stat.vid_buf_time > cur_stat.buf_time_max)) {
            if ((SYNC_REF_PCR == ctx->attr.sync_ref) &&
                (SYNC_PCR_ADJUST_SCR == ctx->pcr_sync_info.pcr_adjust_type)) {
                if (HI_TRUE == ctx->pcr_sync_info.is_pcr_adjust_delta_ok) {
                    if (cur_stat.vid_buf_time > cur_stat.aud_buf_time) {
                        ctx->pcr_sync_info.pcr_delta += cur_stat.vid_buf_time - cur_stat.buf_time_normal;
                    } else {
                        ctx->pcr_sync_info.pcr_delta += cur_stat.aud_buf_time - cur_stat.buf_time_normal;
                    }

                    ctx->pcr_sync_info.is_pcr_adjust_delta_ok = HI_FALSE;
                    HI_WARN_SYNC("pcr_delta:%d\n", ctx->pcr_sync_info.pcr_delta);
                }
            }

            ctx->is_buf_adjust = HI_TRUE;
        }
    }

    if (ctx->is_buf_adjust) {
        if ((SYNC_REF_PCR == ctx->attr.sync_ref) &&
            (SYNC_PCR_ADJUST_SCR == ctx->pcr_sync_info.pcr_adjust_type)) {
            if ((cur_stat.aud_buf_time < cur_stat.buf_time_max) &&
                (cur_stat.vid_buf_time < cur_stat.buf_time_max) &&
                (HI_TRUE == ctx->is_vid_sync_adjust)) {
                ctx->is_buf_adjust = HI_FALSE;
            }
        } else {
            if ((cur_stat.aud_buf_time < cur_stat.buf_time_normal) ||
                (cur_stat.vid_buf_time < cur_stat.buf_time_normal)) {
                ctx->is_buf_adjust = HI_FALSE;
            }

            ctx->aud_opt.proc = SYNC_PROC_DISCARD;
            ctx->aud_discard_cnt++;
        }
    }
}

static void sync_check_buf_stat(sync_context *ctx)
{
    if (!ctx->is_presync_finish) {
        return;
    }

    if (ctx->is_aud_first_come && ctx->is_vid_first_come) {
        if ((SYNC_REF_PCR == ctx->attr.sync_ref) &&
           (SYNC_PCR_ADJUST_SCR == ctx->pcr_sync_info.pcr_adjust_type)) {
            if ((HI_TRUE == ctx->is_pcr_rewind) ||
                (HI_TRUE == ctx->is_vid_rewind) ||
                (HI_TRUE == ctx->is_aud_rewind)) {
                return;
            }
        }
    }

    if (ctx->cur_buf_status.is_buf_time_need_ctrl) {
        sync_buf_adjust(ctx);
    }

    if (((SYNC_PROC_DISCARD == ctx->aud_opt.proc) && (SYNC_BUF_STATE_EMPTY == ctx->cur_buf_status.aud_buf_state)) ||
        ((SYNC_PROC_DISCARD == ctx->vid_opt.proc) && (SYNC_BUF_STATE_EMPTY == ctx->cur_buf_status.vid_buf_state))) {
        if ((SYNC_REF_PCR == ctx->attr.sync_ref) &&
            (SYNC_PCR_ADJUST_SCR == ctx->pcr_sync_info.pcr_adjust_type)) {
            if (HI_TRUE == ctx->pcr_sync_info.is_pcr_adjust_delta_ok) {
                /* adjust pcr step by step */
                ctx->pcr_sync_info.pcr_delta -= PCR_DELTA_ADJUST_STEP;
                ctx->pcr_sync_info.is_pcr_adjust_delta_ok = HI_FALSE;
                HI_WARN_SYNC(">>>>buf low, change pcr_delta %d \n", ctx->pcr_sync_info.pcr_delta);
            }

            ctx->aud_opt.proc = SYNC_PROC_PLAY;
            ctx->vid_opt.proc = SYNC_PROC_PLAY;
        } else {
            if ((SYNC_PROC_DISCARD == ctx->vid_opt.proc) &&
                (SYNC_BUF_STATE_EMPTY == ctx->cur_buf_status.vid_buf_state) &&
                (SYNC_AUD_SPEED_ADJUST_MUTE_REPEAT != ctx->aud_opt.speed_adjust)) {
                if (ctx->vid_aud_diff + AUD_SYNC_REPEAT_THRESHOLD > 0) {
                    ctx->aud_opt.speed_adjust = SYNC_AUD_SPEED_ADJUST_DOWN;
                    ctx->aud_opt.proc = SYNC_PROC_PLAY;
                } else {
                    ctx->aud_opt.proc = SYNC_PROC_REPEAT;
                    ctx->aud_repeat_cnt++;
                }

                ctx->vid_opt.proc = SYNC_PROC_PLAY;
            }
        }

        HI_INFO_SYNC("Aud/vid Buf Low\n");
    }

    return;
}

static void sync_ref_aud_proc_vid_back(sync_context *ctx, sync_vid_opt *opt)
{
    if ((ctx->is_aud_rewind == HI_FALSE) && (ctx->is_vid_rewind == HI_FALSE)) {
        return;
    }

    // audio rewind, then vid need drop frame
    if ((ctx->is_vid_rewind == HI_FALSE) && (ctx->is_aud_rewind == HI_TRUE)) {
        opt->proc = SYNC_PROC_DISCARD;
        HI_WARN_SYNC("vid discard frame, Audloop: %d, Vidloop: %d, vid_aud_diff: %d\n",
                     ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->vid_aud_diff);
        return;
    }

    // video rewind, then vid need block to waite aud
    if ((ctx->is_vid_rewind == HI_TRUE) && (ctx->is_aud_rewind == HI_FALSE)) {
        opt->proc = SYNC_PROC_BLOCK;
        HI_WARN_SYNC("vid block frame, Audloop: %d, Vidloop: %d, vid_aud_diff: %d\n",
                     ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->vid_aud_diff);
        return;
    }

    // process back over
    if ((ctx->is_vid_rewind == HI_TRUE) && (ctx->is_aud_rewind == HI_TRUE)) {
        ctx->is_aud_rewind = HI_FALSE;
        ctx->is_vid_rewind = HI_FALSE;
        HI_WARN_SYNC("loopback proc finsh, vid_aud_diff:%d\n", ctx->vid_aud_diff);
        return;
    }
}

static void sync_ref_pcr_proc_vid_back(sync_context *ctx, sync_vid_opt *opt)
{
    hi_s32 vid_pcr_diff;
    hi_s32 aud_pcr_diff;
    hi_u32 time_cost;

    if (ctx->is_aud_rewind == HI_FALSE &&
        ctx->is_vid_rewind == HI_FALSE &&
        ctx->is_pcr_rewind == HI_FALSE) {
        return;
    }

    vid_pcr_diff = ctx->vid_info.pts - ctx->pcr_sync_info.pcr_last_valid;
    aud_pcr_diff = ctx->aud_info.pts - ctx->pcr_sync_info.pcr_last_valid;

    if (ctx->loop_rewind_start_time == HI_INVALID_TIME) {
        ctx->loop_rewind_start_time = sync_get_sys_time();
    }

    time_cost = sync_get_sys_time_cost(ctx->loop_rewind_start_time);
    if (time_cost > PTS_LOOPBACK_TIMEOUT) {
        ctx->is_aud_rewind = HI_FALSE;
        ctx->is_pcr_rewind = HI_FALSE;
        ctx->is_vid_rewind = HI_FALSE;
        ctx->loop_rewind_start_time = HI_INVALID_TIME;

        HI_WARN_SYNC("loopback timeOut, Audloop: %d, Vidloop: %d, Pcrloop: %d, vid_aud_diff: %d\n",
                     ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->is_pcr_rewind, ctx->vid_aud_diff);
        return;
    }

    /* pcr and vid loopback, then vid need block to waite aud */
    if (ctx->is_vid_rewind == HI_FALSE &&
        ctx->is_pcr_rewind == HI_FALSE &&
        ctx->is_aud_rewind == HI_TRUE) {
        opt->proc = SYNC_PROC_PLAY;
        HI_WARN_SYNC("vid play, Audloop: %d, Vidloop: %d, Pcrloop: %d, vid_aud_diff: %d\n",
                     ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->is_pcr_rewind, ctx->vid_aud_diff);
        return;
    }

    /* pcr and vid loopback, then vid need block to waite aud */
    if (ctx->is_vid_rewind == HI_FALSE &&
        ctx->is_pcr_rewind == HI_TRUE &&
        ctx->is_aud_rewind == HI_TRUE) {
        opt->proc = SYNC_PROC_DISCARD;
        HI_WARN_SYNC("vid play, Audloop: %d, Vidloop: %d, Pcrloop: %d, vid_aud_diff: %d\n",
                     ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->is_pcr_rewind, ctx->vid_aud_diff);
        return;
    }

    /* pcr and vid loopback, then vid need block to waite aud */
    if (ctx->is_vid_rewind == HI_TRUE &&
        ctx->is_pcr_rewind == HI_FALSE &&
        ctx->is_aud_rewind == HI_FALSE) {
        opt->proc = SYNC_PROC_BLOCK;
        HI_WARN_SYNC("vid block, Audloop: %d, Vidloop: %d, Pcrloop: %d, vid_aud_diff: %d\n",
                     ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->is_pcr_rewind, ctx->vid_aud_diff);
        return;
    }

    /* pcr and vid loopback, then vid need block to waite aud */
    if (ctx->is_vid_rewind == HI_TRUE &&
        ctx->is_pcr_rewind == HI_TRUE &&
        ctx->is_aud_rewind == HI_FALSE) {
        if (vid_pcr_diff > 0) {
            ctx->vid_opt.proc = SYNC_PROC_BLOCK;
        } else {
            ctx->vid_opt.proc = SYNC_PROC_PLAY;
        }

        HI_WARN_SYNC("vid block, Audloop: %d, Vidloop: %d, Pcrloop: %d, vid_aud_diff: %d\n",
                     ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->is_pcr_rewind, ctx->vid_aud_diff);
        return;
    }

    /* process back over */
    if (ctx->is_pcr_rewind == HI_TRUE &&
        ctx->is_vid_rewind == HI_TRUE &&
        ctx->is_aud_rewind == HI_TRUE) {
        if (vid_pcr_diff > 0) {
            ctx->vid_opt.proc = SYNC_PROC_BLOCK;
        } else {
            ctx->vid_opt.proc = SYNC_PROC_PLAY;
        }

        if ((vid_pcr_diff < 0) && (aud_pcr_diff < 0)) {
            ctx->is_aud_rewind = HI_FALSE;
            ctx->is_pcr_rewind = HI_FALSE;
            ctx->is_vid_rewind = HI_FALSE;
            ctx->loop_rewind_start_time = HI_INVALID_TIME;
            HI_WARN_SYNC("loopback proc finsh, vid_aud_diff:%d\n", ctx->vid_aud_diff);
        }

        return;
    }

    HI_WARN_SYNC("Audloop: %d, Vidloop: %d, Pcrloop: %d, vid_aud_diff: %d\n",
                 ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->is_pcr_rewind, ctx->vid_aud_diff);
}

static void sync_vid_proc_back(sync_context *ctx, sync_vid_info *info, sync_vid_opt *opt)
{
    if (ctx->is_aud_first_come == HI_FALSE) {
        ctx->is_vid_rewind = HI_FALSE;
        ctx->is_pcr_rewind = HI_FALSE;
        return;
    }

    if (sync_get_sync_adjust_ref_mode(ctx) == SYNC_REF_PCR) {
        sync_ref_pcr_proc_vid_back(ctx, opt);
    } else if (sync_get_sync_adjust_ref_mode(ctx) == SYNC_REF_AUDIO) {
        sync_ref_aud_proc_vid_back(ctx, opt);
    }
}

static void sync_check_vid_loop_rewind(sync_context *ctx, sync_vid_info *info)
{
    if (!info->is_pts_valid ||
        ctx->is_vid_rewind == HI_TRUE ||
        ctx->vid_last_valid_pts == SYNC_INVALID_PTS ||
        (sync_get_sync_adjust_ref_mode(ctx) != SYNC_REF_PCR &&
        sync_get_sync_adjust_ref_mode(ctx) != SYNC_REF_AUDIO)) {
        return;
    }

    if (ctx->vid_last_valid_pts > info->pts &&
        (ctx->vid_last_valid_pts - info->pts) > PTS_LOOPBACK_THRESHOLD) {
        if ((PCR_TIME_MAX - ctx->vid_last_valid_pts) < MAX_REWIND_PTS && info->pts < MAX_REWIND_PTS) {
            HI_WARN_SYNC("video warps normally. last pts: %d, pts: %d\n", ctx->vid_last_valid_pts, info->pts);
        } else {
            ctx->vid_loop_rewind_pts = ctx->vid_last_valid_pts;
            ctx->is_vid_rewind = HI_TRUE;
            HI_WARN_SYNC("vid loopback! lastpts %d, pts %d\n", ctx->vid_last_valid_pts, info->pts);
        }
    } else if ((ctx->is_aud_rewind == HI_TRUE) &&
               (info->pts < ctx->aud_loop_rewind_pts) &&
               ((info->pts - ctx->aud_loop_rewind_pts) < (-1 * PTS_LOOPBACK_THRESHOLD))) {
        if ((PCR_TIME_MAX - ctx->aud_loop_rewind_pts) < MAX_REWIND_PTS && info->pts < MAX_REWIND_PTS) {
            HI_WARN_SYNC("vid warps normally lastpts %d, pts %d\n", ctx->aud_loop_rewind_pts, info->pts);
        } else {
            ctx->aud_loop_rewind_pts = SYNC_INVALID_PTS;
            ctx->vid_loop_rewind_pts = SYNC_INVALID_PTS;
            ctx->is_vid_rewind = HI_TRUE;
            HI_WARN_SYNC("Vid loopback! lastpts %d, pts %d\n", ctx->aud_loop_rewind_pts, info->pts);
        }
    }

    return;
}

void sync_vid_proc(hi_handle sync, sync_vid_info *info, sync_vid_opt *opt)
{
    hi_u32        id = SYNC_GET_ID(sync);
    sync_context  *ctx = HI_NULL;
    hi_u32        time;

    SYNC_CHECK_ID_NOT_RET(id);

    ctx = sync_info_ctx_get(id);

    opt->proc = SYNC_PROC_PLAY;
    opt->repeat = 0;
    opt->discard = 0;
    opt->vdec_discard_time = 0;

#ifdef HI_VIDEO_TB_SUPPORT
    opt->tb_adjust_type = HI_DRV_VIDEO_TB_PLAY;
#endif

    /* record the video frame information */
    ctx->vid_info = *info;
    ctx->vid_opt = *opt;

    time = sync_get_sys_time();
    HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_info src_pts: %-8d, pts: %-8d, frame_time: %d, delay_time: %d,sys time: %d\n",
                  info->src_pts, info->pts, info->frame_time, info->delay_time, time);

    /* can't do(no effect) smooth adjust when do sync without FRC(frame rate convertion) */
    if (!ctx->is_use_stop_region) {
        ctx->attr.start_region.smooth_play = HI_FALSE;
        ctx->attr.novel_region.smooth_play = HI_FALSE;
    }

    /* pcr timeout, we used aud adjust scr */
    if ((SYNC_SCR_ADJUST_MAX == ctx->pcr_sync_info.pcr_adjust_type) &&
        (SYNC_REF_PCR == ctx->attr.sync_ref)) {
        if (HI_INVALID_TIME == ctx->pcr_sync_info.pcr_sync_start_sys_time) {
            ctx->pcr_sync_info.pcr_sync_start_sys_time = sync_get_sys_time();
        }

        if (HI_TRUE == sync_check_pcr_timeout(ctx)) {
            ctx->pcr_sync_info.pcr_adjust_type = SYNC_AUD_ADJUST_SCR;
            HI_INFO_SYNC("Pcr is timeout adjust pcr by audpts\n");
        }
    }

    if (0 == info->disp_time) {
        ctx->vid_opt.proc = SYNC_PROC_DISCARD;
        *opt = ctx->vid_opt;
        HI_INFO_VSYNC(SYNC_CHAN_VID, "--------Vid Frc Discard--------\n");
        return;
    }

    sync_check_vid_pts_valid(ctx, &ctx->vid_info);
    sync_check_vid_loop_rewind(ctx, &ctx->vid_info);

    /* quick output the first frame */
    if (ctx->attr.quick_output && (!ctx->is_vid_first_play)) {
        ctx->is_vid_first_play = HI_TRUE;
        ctx->vid_first_play_time = sync_get_sys_time();

        ctx->is_vid_first_come = HI_TRUE;
        ctx->vid_first_sys_time = ctx->vid_first_play_time;
        ctx->vid_first_pts = info->pts;

        hi_drv_stat_event(HI_STAT_EVENT_FRAME_SYNC_OK, 0);

        ctx->vid_opt.proc   = SYNC_PROC_QUICKOUTPUT;
#ifdef HI_VIDEO_TB_SUPPORT
        ctx->vid_opt.tb_adjust_type = HI_DRV_VIDEO_TB_PLAY;
#endif

        HI_INFO_VSYNC(SYNC_CHAN_VID, "--------Vid QuickOutput Proc %d--------\n", opt->proc);
        HI_INFO_VSYNC(SYNC_CHAN_VID, "first vidframe sys time: %d, pts: %d, src_pts: %d\n",
                      ctx->vid_first_sys_time, ctx->vid_first_pts, info->src_pts);

        goto OUT;
    }

    /* record the information and the time of the first video frame used to sync */
    if (!ctx->is_vid_first_come) {
        ctx->is_vid_first_come = HI_TRUE;
        ctx->vid_first_sys_time = sync_get_sys_time();
        ctx->vid_first_pts = info->pts;
        ctx->is_vid_sync_adjust = HI_FALSE;
        HI_INFO_VSYNC(SYNC_CHAN_VID, "first vidframe sys time: %d, pts: %d, src_pts: %d\n",
                      ctx->vid_first_sys_time, ctx->vid_first_pts, info->src_pts);
    }

    if (!ctx->is_vid_first_valid_come && (info->src_pts != SYNC_INVALID_PTS)) {
        ctx->is_vid_first_valid_come = HI_TRUE;
        ctx->vid_first_valid_pts = info->src_pts;

        if (ctx->is_aud_first_valid_come) {
            ctx->is_first_sync_state_report = HI_TRUE;
        }

        HI_INFO_VSYNC(SYNC_CHAN_VID, "vid_first_valid_pts come: %u\n", ctx->vid_first_valid_pts);
    }

    if (SYNC_STATUS_TPLAY == ctx->cur_status) {
        sync_set_local_time(ctx, SYNC_CHAN_VID, (info->pts - ctx->vid_info.delay_time));
        ctx->vid_opt.proc = SYNC_PROC_PLAY;
        HI_INFO_VSYNC(SYNC_CHAN_VID, "--------Vid TPLAY--------\n");
        goto OUT;
    }

    /* presync */
    if (!sync_is_vid_presync_finish(ctx, &ctx->vid_info, &ctx->vid_opt)) {
        goto OUT;
    }

    /* presync finished. cumulate video and audio data to prevent underflow */
    if (!sync_is_vid_buf_fund_finish(ctx, &ctx->vid_info, &ctx->vid_opt)) {
        goto OUT;
    }

    sync_set_vid_local_time(ctx, &ctx->vid_info);
    sync_calc_difftime(ctx, SYNC_CHAN_VID);
    sync_check_evt(ctx, SYNC_CHAN_VID);
#ifdef HI_VIDEO_TB_SUPPORT
    sync_check_tb_match_adjust(ctx);
#endif

    if (sync_is_vid_buf_overflow_discard_frm(ctx, &ctx->vid_opt)) {
        goto OUT;
    }

    if (sync_is_sync_ref_none(ctx, SYNC_CHAN_VID)) {
        goto OUT;
    }

    sync_adjust(ctx, SYNC_CHAN_VID);
    sync_vid_proc_back(ctx, &ctx->vid_info, &ctx->vid_opt);
    sync_check_buf_stat(ctx);

    if (SYNC_PROC_PLAY == ctx->vid_opt.proc) {
        if (!ctx->is_vid_first_play) {
            ctx->is_vid_first_play = HI_TRUE;
            ctx->is_vid_normal_play = HI_TRUE;
            ctx->vid_first_play_time = sync_get_sys_time();
            hi_drv_stat_event(HI_STAT_EVENT_FRAME_SYNC_OK, 0);
        }
    }

OUT:
    ctx->vid_last_pts = info->pts;
    ctx->vid_last_src_pts = info->src_pts;

    if (ctx->vid_info.is_pts_valid) {
        ctx->vid_last_valid_pts = info->pts;
    }

    *opt = ctx->vid_opt;

#ifdef SYNC_DEBUG_DETAIL
    sync_proc_debug_info(ctx, SYNC_DBG_INFO_LIST_VIDADD, HI_NULL);
#endif

    HI_INFO_VSYNC(SYNC_CHAN_VID, "--------Vid Sync Proc: %d--------\n", opt->proc);
    HI_TRACE_SYNC("[vid] pts[%u] delay[%u] diap_rate[%u] frame_time[%u] diff[%d] proc[%d]\n",
        info->pts, info->delay_time, info->disp_rate, info->frame_time, ctx->vid_aud_diff, opt->proc);

    return;
}

static hi_bool sync_is_aud_presync_finish(sync_context *ctx, sync_aud_info *info, sync_aud_opt *opt)
{
    if (ctx->is_presync_finish) {
        return HI_TRUE;
    }

    sync_presync(ctx, SYNC_CHAN_AUD);

    if (ctx->aud_opt.proc != SYNC_PROC_CONTINUE) {
        *opt = ctx->aud_opt;

        if (ctx->is_presync_target_init) {
            HI_INFO_ASYNC(SYNC_CHAN_AUD, "--------Aud PreSync vid_aud_diff: %d, Proc: %d, Speed: %d--------\n",
                          ctx->vid_aud_diff, opt->proc, opt->speed_adjust);
        }

        if (info->is_pts_valid) {
            sync_set_local_time(ctx, SYNC_CHAN_AUD, (info->pts - info->buf_time));
        }

        return HI_FALSE;
    }

    /* first set scr */
    if (!ctx->is_scr_init && (info->pts != SYNC_INVALID_PTS)) {
        sync_set_local_time(ctx, SYNC_CHAN_SCR, (info->pts - info->buf_time));
        ctx->is_scr_init = HI_TRUE;
        ctx->scr_first_local_time = info->pts - info->buf_time;
        ctx->scr_first_sys_time = sync_get_sys_time();
    }

    return HI_TRUE;
}

static hi_bool sync_is_aud_buf_fund_finish(sync_context *ctx, sync_aud_info *info, sync_aud_opt *opt)
{
    if (ctx->is_buf_fund_finish) {
        return HI_TRUE;
    }

    sync_buf_fund(ctx);

    if (ctx->aud_opt.proc != SYNC_PROC_CONTINUE) {
        *opt = ctx->aud_opt;
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "--------Aud BufFund proc: %d, speed: %d--------\n",
                      opt->proc, opt->speed_adjust);

        return HI_FALSE;
    }

    sync_set_local_time(ctx, SYNC_CHAN_AUD, (info->pts - info->buf_time));

    if (SYNC_AUD_ADJUST_SCR == ctx->pcr_sync_info.pcr_adjust_type) {
        sync_set_local_time(ctx, SYNC_CHAN_PCR, (info->pts - info->buf_time));
        HI_INFO_SYNC("Pcr set local time: %d  by aud pts\n", (info->pts - info->buf_time));
    }

    if (ctx->is_vid_first_come) {
        sync_set_local_time(ctx, SYNC_CHAN_VID, (ctx->vid_info.pts - ctx->vid_info.delay_time));
        HI_INFO_VSYNC(SYNC_CHAN_VID, "vid buf fund first set local time %d\n",
                      (ctx->vid_info.pts - ctx->vid_info.delay_time));
    }

    HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud buf fund first set local time %d\n", (info->pts - info->buf_time));

    return HI_TRUE;
}

static hi_bool sync_is_aud_resync_finish(sync_context *ctx, sync_aud_info *info, sync_aud_opt *opt)
{
    /* AudDDPMode do not set audio resynchronization */
    if (ctx->is_ddp_mode || !ctx->is_aud_resync) {
        return HI_TRUE;
    }

    sync_aud_resync(ctx);

    if (ctx->aud_opt.proc != SYNC_PROC_CONTINUE) {
        *opt = ctx->aud_opt;
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "--------Aud resync vid_aud_diff: %d, proc: %d, speed: %d--------\n",
            ctx->vid_aud_diff, opt->proc, opt->speed_adjust);

        return HI_FALSE;
    }

    return HI_TRUE;
}

static hi_bool sync_is_aud_rebuf_fund_finish(sync_context *ctx, sync_aud_info *info, sync_aud_opt *opt)
{
    if (!ctx->is_aud_rebuf_fund) {
        return HI_TRUE;
    }

    sync_aud_rebuf_fund(ctx);

    if (ctx->aud_opt.proc != SYNC_PROC_CONTINUE) {
        *opt = ctx->aud_opt;

        HI_INFO_ASYNC(SYNC_CHAN_AUD, "--------Aud ReBufFund Proc %d Speed %d--------\n", opt->proc, opt->speed_adjust);
        HI_INFO_ASYNC(SYNC_CHAN_AUD, "\n");
        return HI_FALSE;
    }

    if (SYNC_AUD_ADJUST_SCR == ctx->pcr_sync_info.pcr_adjust_type) {
        sync_set_local_time(ctx, SYNC_CHAN_PCR, (info->pts - info->buf_time));
        HI_INFO_SYNC("pcr set local time: %d  by Audpts\n", (info->pts - info->buf_time));
    }

    sync_set_local_time(ctx, SYNC_CHAN_AUD, (info->pts - info->buf_time));
    HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud rebuf fund first set local time: %d\n", (info->pts - info->buf_time));

    return HI_TRUE;
}

static void sync_set_aud_local_time(sync_context *ctx, sync_aud_info *info)
{
    if ((HI_TRUE == info->is_pts_valid) ||
        ((HI_FALSE == ctx->is_aud_local_time) && (info->pts != SYNC_INVALID_PTS))) {
        sync_set_local_time(ctx, SYNC_CHAN_AUD, (info->pts - info->buf_time));

        if (SYNC_AUD_ADJUST_SCR == ctx->pcr_sync_info.pcr_adjust_type) {
            sync_set_local_time(ctx, SYNC_CHAN_PCR, (info->pts - info->buf_time));
            HI_INFO_SYNC("pcr set local time: %d  by aud pts\n", (info->pts - info->buf_time));
        }

        HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud set local time: %d\n", (info->pts - info->buf_time));
    }

    return;
}

static void sync_ref_aud_proc_aud_back(sync_context *ctx, sync_aud_opt *opt)
{
    if (ctx->is_aud_rewind == HI_FALSE && ctx->is_vid_rewind == HI_FALSE) {
        return;
    }

    // audio rewind, then aud need block to waite vid
    if (ctx->is_vid_rewind == HI_FALSE && ctx->is_aud_rewind == HI_TRUE) {
        opt->proc = SYNC_PROC_BLOCK;
        HI_WARN_SYNC("aud block frame, Audloop: %d, Vidloop: %d, vid_aud_diff: %d\n",
                     ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->vid_aud_diff);
        return;
    }

    // video rewind, then aud need discard
    if (ctx->is_vid_rewind == HI_TRUE && ctx->is_aud_rewind == HI_FALSE) {
        opt->proc = SYNC_PROC_DISCARD;
        HI_WARN_SYNC("vid block frame, Audloop: %d, Vidloop: %d, vid_aud_diff: %d\n",
                     ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->vid_aud_diff);
        return;
    }

    // process back over
    if (ctx->is_vid_rewind == HI_TRUE && ctx->is_aud_rewind == HI_TRUE) {
        ctx->is_aud_rewind = HI_FALSE;
        ctx->is_vid_rewind = HI_FALSE;
        HI_WARN_SYNC("loopback proc finsh, vid_aud_diff:%d\n", ctx->vid_aud_diff);
        return;
    }
}

static void sync_ref_pcr_proc_aud_back(sync_context *ctx, sync_aud_opt *opt)
{
    hi_s32 vid_pcr_diff;
    hi_s32 aud_pcr_diff;
    hi_u32 time_cost;

    if (HI_FALSE == ctx->is_aud_rewind &&
        HI_FALSE == ctx->is_vid_rewind &&
        HI_FALSE == ctx->is_pcr_rewind) {
        return;
    }

    vid_pcr_diff = ctx->vid_info.pts - ctx->pcr_sync_info.pcr_last_valid;
    aud_pcr_diff = ctx->aud_info.pts - ctx->pcr_sync_info.pcr_last_valid;

    if (ctx->loop_rewind_start_time == HI_INVALID_TIME) {
        ctx->loop_rewind_start_time = sync_get_sys_time();
    }

    time_cost = sync_get_sys_time_cost(ctx->loop_rewind_start_time);
    if (time_cost > PTS_LOOPBACK_TIMEOUT) {
        ctx->is_aud_rewind = HI_FALSE;
        ctx->is_pcr_rewind = HI_FALSE;
        ctx->is_vid_rewind = HI_FALSE;
        ctx->loop_rewind_start_time = HI_INVALID_TIME;

        HI_WARN_SYNC("loop rewind time out. aud loop: %d, vid loop: %d, pcr loop: %d, vid_aud_diff: %d \n",
                     ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->is_pcr_rewind, ctx->vid_aud_diff);
        return;
    }

    if (ctx->is_vid_rewind == HI_TRUE &&
        ctx->is_pcr_rewind == HI_FALSE &&
        ctx->is_aud_rewind == HI_FALSE) {
        opt->proc = SYNC_PROC_PLAY;
        HI_WARN_SYNC("aud play. aud loop: %d, vid loop: %d, pcr loop: %d, vid_aud_diff: %d\n",
                     ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->is_pcr_rewind, ctx->vid_aud_diff);
        return;
    }

    if (ctx->is_vid_rewind == HI_TRUE &&
        ctx->is_pcr_rewind == HI_TRUE &&
        ctx->is_aud_rewind == HI_FALSE) {
        opt->proc = SYNC_PROC_DISCARD;
        HI_WARN_SYNC("aud block. aud loop: %d, vid loop: %d, pcr loop: %d, vid_aud_diff: %d\n",
                     ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->is_pcr_rewind, ctx->vid_aud_diff);
        return;
    }

    if (ctx->is_vid_rewind == HI_FALSE &&
        ctx->is_pcr_rewind == HI_FALSE &&
        ctx->is_aud_rewind == HI_TRUE) {
        opt->proc = SYNC_PROC_BLOCK;
        HI_WARN_SYNC("aud block. aud loop: %d, vid loop: %d, pcr loop: %d, vid_aud_diff: %d\n",
                     ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->is_pcr_rewind, ctx->vid_aud_diff);
        return;
    }

    if (ctx->is_vid_rewind == HI_FALSE &&
        (ctx->is_pcr_rewind == HI_TRUE) &&
        ctx->is_aud_rewind == HI_TRUE) {
        if (aud_pcr_diff > 0) {
            opt->proc = SYNC_PROC_BLOCK;
        } else {
            opt->proc = SYNC_PROC_PLAY;
        }

        HI_WARN_SYNC("aud block. aud loop: %d, vid loop: %d, pcr loop: %d, vid_aud_diff: %d\n",
                     ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->is_pcr_rewind, ctx->vid_aud_diff);
        return;
    }

    if (ctx->is_pcr_rewind == HI_TRUE &&
        ctx->is_vid_rewind == HI_TRUE &&
        ctx->is_aud_rewind == HI_TRUE) {
        if (aud_pcr_diff > 0) {
            opt->proc = SYNC_PROC_BLOCK;
        } else {
            opt->proc = SYNC_PROC_PLAY;
        }

        if ((vid_pcr_diff < 0) && (aud_pcr_diff < 0)) {
            ctx->is_aud_rewind = HI_FALSE;
            ctx->is_pcr_rewind = HI_FALSE;
            ctx->is_vid_rewind = HI_FALSE;
            ctx->loop_rewind_start_time = HI_INVALID_TIME;
            HI_WARN_SYNC("loopback proc finsh, vid_aud_diff:%d\n", ctx->vid_aud_diff);
        }
        return;
    }

    HI_WARN_SYNC("aud block. aud loop: %d, vid loop: %d, pcr loop: %d, vid_aud_diff: %d\n",
                 ctx->is_aud_rewind, ctx->is_vid_rewind, ctx->is_pcr_rewind, ctx->vid_aud_diff);
}

static void sync_aud_proc_back(sync_context *ctx, sync_aud_info *info, sync_aud_opt *opt)
{
    if (ctx->is_vid_first_come == HI_FALSE) {
        ctx->is_aud_rewind = HI_FALSE;
        ctx->is_pcr_rewind = HI_FALSE;
        return;
    }

    if (sync_get_sync_adjust_ref_mode(ctx) == SYNC_REF_PCR) {
        sync_ref_pcr_proc_aud_back(ctx, opt);
    } else if (sync_get_sync_adjust_ref_mode(ctx) == SYNC_REF_AUDIO) {
        sync_ref_aud_proc_aud_back(ctx, opt);
    }
}

static void sync_check_aud_loop_rewind(sync_context *ctx, sync_aud_info *info)
{
    if (!info->is_pts_valid ||
        ctx->is_aud_rewind == HI_TRUE ||
        ctx->aud_last_valid_pts == SYNC_INVALID_PTS ||
        ((sync_get_sync_adjust_ref_mode(ctx) != SYNC_REF_PCR) &&
        (sync_get_sync_adjust_ref_mode(ctx) != SYNC_REF_AUDIO))) {
        return;
    }

    if ((ctx->aud_last_valid_pts > info->pts) &&
        (ctx->aud_last_valid_pts - info->pts > PTS_LOOPBACK_THRESHOLD)) {
        if ((PCR_TIME_MAX - ctx->aud_last_valid_pts < 1000) && (info->pts < 1000)) {
            HI_WARN_SYNC("audio warps normally.lp %d,p %d\n", ctx->aud_last_valid_pts, info->pts);
        } else {
            ctx->aud_loop_rewind_pts = ctx->aud_last_valid_pts;
            ctx->is_aud_rewind = HI_TRUE;
            HI_WARN_SYNC("Aud loop rewind! last pts: %d, pts: %d\n", ctx->aud_last_valid_pts, info->pts);
        }
    } else if ((ctx->is_vid_rewind == HI_TRUE) &&
               (info->pts < ctx->vid_loop_rewind_pts) &&
               (info->pts - ctx->vid_loop_rewind_pts < (-1 * PTS_LOOPBACK_THRESHOLD))) {
        if ((PCR_TIME_MAX - ctx->vid_loop_rewind_pts < 1000) && (info->pts < 1000)) {
            HI_WARN_SYNC("audio warps normally lastpts %d, pts %d\n", ctx->aud_loop_rewind_pts, info->pts);
        } else {
            ctx->aud_loop_rewind_pts = SYNC_INVALID_PTS;
            ctx->vid_loop_rewind_pts = SYNC_INVALID_PTS;
            ctx->is_aud_rewind = HI_TRUE;
            HI_WARN_SYNC("Aud loop rewind! last pts: %d, pts: %d\n", ctx->aud_last_valid_pts, info->pts);
        }
    }

    return;
}

void sync_ad_aud_proc(hi_handle sync, sync_aud_info *info, sync_aud_opt *opt)
{
    hi_u32       id = SYNC_GET_ID(sync);
    sync_context *ctx = HI_NULL;
    hi_u32       sys_time;
    hi_s64       ad_aud_diff;

    SYNC_CHECK_ID_NOT_RET(id);

    ctx = sync_info_ctx_get(id);

    opt->proc = SYNC_PROC_PLAY;
    opt->speed_adjust = SYNC_AUD_SPEED_ADJUST_NORMAL;
    /* record audio frame information */
    ctx->ad_aud_info = *info;
    ctx->ad_aud_opt = *opt;

    sys_time = sync_get_sys_time();
    HI_INFO_ASYNC(SYNC_CHAN_AUD, "ad aud info src_pts: %-8d, pts: %-8d, frame_time: %d, \
                  buf_time: %-4d, frame_num: %d, sys_time: %d\n",
                  info->src_pts, info->pts, info->frame_time, info->buf_time, info->frame_num, sys_time);

    if (ctx->aud_last_pts == SYNC_INVALID_PTS || ctx->ad_aud_info.pts == SYNC_INVALID_PTS) {
        ctx->ad_aud_opt.proc = SYNC_PROC_PLAY;
        return;
    }

    ad_aud_diff = ctx->ad_aud_info.pts - ctx->aud_last_pts;

    if (sync_abs(ad_aud_diff) > SYNC_AD_AUD_PTS_MAX_DIFF) {
        ctx->ad_aud_opt.proc = SYNC_PROC_PLAY;
        return;
    }

    if (ad_aud_diff < SYNC_AD_AUD_STOP_PLUS &&
        ad_aud_diff > SYNC_AD_AUD_STOP_NEGATIVE) {
        ctx->ad_aud_opt.proc = SYNC_PROC_PLAY;
    }

    if (ad_aud_diff > SYNC_AD_AUD_START_PLUS) {
        ctx->ad_aud_opt.proc = SYNC_PROC_REPEAT;
        HI_INFO_SYNC("SYNC_PROC_REPEAT ad aud pts:%d, target pts:%d\n", ctx->ad_aud_info.pts, ctx->aud_last_pts);
    }

    if (ad_aud_diff < SYNC_AD_AUD_START_NEGATIVE) {
        ctx->ad_aud_opt.proc = SYNC_PROC_DISCARD;
        HI_INFO_SYNC("SYNC_PROC_DISCARD ad aud pts:%d, target pts:%d\n", ctx->ad_aud_info.pts, ctx->aud_last_pts);
    }
}

void sync_aud_proc(hi_handle sync, sync_aud_info *info, sync_aud_opt *opt)
{
    hi_u32       id = SYNC_GET_ID(sync);
    sync_context *ctx = HI_NULL;
    hi_u32       sys_time;

    SYNC_CHECK_ID_NOT_RET(id);

    ctx = sync_info_ctx_get(id);

    opt->proc = SYNC_PROC_PLAY;
    opt->speed_adjust = SYNC_AUD_SPEED_ADJUST_NORMAL;

    /* record audio frame information */
    ctx->aud_info = *info;
    ctx->aud_opt = *opt;

    sys_time = sync_get_sys_time();
    HI_INFO_ASYNC(SYNC_CHAN_AUD, "aud info src_pts: %-8d, pts: %-8d, frame_time: %d, \
                  buf_time: %-4d, frame_num: %d, sys_time: %d\n",
                  info->src_pts, info->pts, info->frame_time, info->buf_time, info->frame_num, sys_time);

    /* can't do(no effect) smooth adjust when do sync without FRC(frame rate convertion) */
    if (!ctx->is_use_stop_region) {
        ctx->attr.start_region.smooth_play = HI_FALSE;
        ctx->attr.novel_region.smooth_play = HI_FALSE;
    }

    if ((SYNC_SCR_ADJUST_MAX == ctx->pcr_sync_info.pcr_adjust_type) &&
        (SYNC_REF_PCR == ctx->attr.sync_ref)) {
        if (HI_INVALID_TIME == ctx->pcr_sync_info.pcr_sync_start_sys_time) {
            ctx->pcr_sync_info.pcr_sync_start_sys_time = sync_get_sys_time();
        }

        if (HI_TRUE == sync_check_pcr_timeout(ctx)) {
            ctx->pcr_sync_info.pcr_adjust_type = SYNC_AUD_ADJUST_SCR;
            HI_INFO_SYNC("Pcr is timeout adjust pcr by audpts\n");
        }
    }

    sync_check_aud_pts_valid(ctx, &ctx->aud_info);
    sync_check_aud_loop_rewind(ctx, &ctx->aud_info);

    if (!ctx->is_aud_first_come) {
        ctx->is_aud_first_come = HI_TRUE;
        ctx->aud_first_sys_time = sync_get_sys_time();
        ctx->aud_first_pts = info->pts;
        ctx->aud_last_pts = info->pts;
        ctx->aud_last_buf_time = info->buf_time;

        HI_INFO_ASYNC(SYNC_CHAN_AUD, "first aud frame sys time: %d, pts: %d, src_pts: %d\n",
                      ctx->aud_first_sys_time, ctx->aud_first_pts, info->src_pts);
        ctx->is_vid_sync_adjust = HI_FALSE;
    }

    if (!ctx->is_aud_first_valid_come && (info->src_pts != SYNC_INVALID_PTS)) {
        ctx->is_aud_first_valid_come = HI_TRUE;
        ctx->aud_first_valid_pts = info->src_pts;

        if (ctx->is_vid_first_valid_come) {
            ctx->is_first_sync_state_report = HI_TRUE;
        }

        HI_INFO_VSYNC(SYNC_CHAN_VID, "aud_first_valid_pts come: %u\n", ctx->aud_first_valid_pts);
    }

    if ((info->pts == SYNC_INVALID_PTS) &&
        (!ctx->is_aud_first_valid_come) &&
        (SYNC_REF_PCR == ctx->attr.sync_ref)) {
        ctx->aud_opt.proc = SYNC_PROC_DISCARD;
        ctx->aud_opt.speed_adjust = SYNC_AUD_SPEED_ADJUST_NORMAL;
        HI_WARN_SYNC("aud pts is -1 ------\n");
        goto OUT;
    }

    if (!sync_is_aud_presync_finish(ctx, &ctx->aud_info, &ctx->aud_opt)) {
        goto OUT;
    }

    if (!sync_is_aud_buf_fund_finish(ctx, &ctx->aud_info, &ctx->aud_opt)) {
        goto OUT;
    }

    /* audio resynchronization is needed when change audio track.StopAud->StartAud */
    if (!sync_is_aud_resync_finish(ctx, &ctx->aud_info, &ctx->aud_opt)) {
        goto OUT;
    }

    /* it is needed to accumulate audio data again */
    if (!sync_is_aud_rebuf_fund_finish(ctx, &ctx->aud_info, &ctx->aud_opt)) {
        goto OUT;
    }

    if ((SYNC_REF_PCR == ctx->attr.sync_ref) && (!ctx->is_aud_first_play)) {
        if ((HI_FALSE == ctx->is_vid_first_come) && (HI_FALSE == sync_check_aud_timeout(ctx))) {
            ctx->aud_opt.proc = SYNC_PROC_BLOCK;
            goto OUT;
        }
    }

    sync_set_aud_local_time(ctx, &ctx->aud_info);
    sync_calc_difftime(ctx, SYNC_CHAN_AUD);
    sync_check_evt(ctx, SYNC_CHAN_AUD);

    if (sync_is_sync_ref_none(ctx, SYNC_CHAN_AUD)) {
        goto OUT;
    }

    sync_adjust(ctx, SYNC_CHAN_AUD);
    sync_aud_proc_back(ctx, &ctx->aud_info, &ctx->aud_opt);
    sync_check_buf_stat(ctx);

    if (SYNC_PROC_PLAY == opt->proc) {
        if (!ctx->is_aud_first_play) {
            ctx->is_aud_first_play = HI_TRUE;
            ctx->aud_first_play_time = sync_get_sys_time();
        }
    }

OUT:
    ctx->aud_last_pts = info->pts;
    ctx->aud_last_buf_time = info->buf_time;

    if (ctx->aud_info.is_pts_valid) {
        ctx->aud_last_valid_pts = info->pts;
    }

    *opt = ctx->aud_opt;

    HI_WARN_SYNC("aud proc: %d, aud_pcr_diff: %d, vid_pcr_diff: %d, vid_aud_diff: %d, \
                 aud local time: %d, pcr local time: %d, pts: %d, buf_time: %d, frame_time: %d, \
                 sys time: %d, pcr_last: %d, pcr_delta: %d, vid_last_pts: %d\n",
                 opt->proc, ctx->pcr_sync_info.aud_pcr_diff, ctx->pcr_sync_info.vid_pcr_diff,
                 ctx->vid_aud_diff, sync_get_local_time(ctx, SYNC_CHAN_AUD),
                 sync_get_local_time(ctx, SYNC_CHAN_PCR), info->pts, info->buf_time,
                 info->frame_time, sync_get_sys_time(), ctx->pcr_sync_info.pcr_last,
                 ctx->pcr_sync_info.pcr_delta, ctx->vid_last_pts);

    HI_INFO_SYNC("--------aud sync proc: %d, speed: %d--------\n",
                 opt->proc, opt->speed_adjust);
    HI_TRACE_SYNC("[aud] pts[%u] delay[%u] frame_time[%u] diff[%d] proc[%d]\n",
        info->pts, info->buf_time, info->frame_time, ctx->vid_aud_diff, opt->proc);

    return;
}

static void sync_check_pcr_valid(sync_context *ctx, hi_u32 pcr_time, hi_u32 stc_time)
{
    hi_s32 pcr_delta = 0;
    hi_s32 stc_delta = 0;
    hi_s32 diff_percent = 0;
    sync_pcr_info *info = &ctx->pcr_sync_info;

     /* pts is invalid */
    if (SYNC_INVALID_PTS == pcr_time) {
        HI_INFO_SYNC("pcr_time == -1 invalid\n");
        info->pcr_series_cnt = 0;
    } else if (pcr_time < info->pcr_last) { /* pts go back */
        HI_INFO_SYNC("pcr:%d < pcr Last %d\n", info->pcr_last);
        info->pcr_series_cnt = 0;
    } else {
        pcr_delta = pcr_time - info->pcr_last;
        stc_delta = stc_time - info->stc_info.stc_last;

        diff_percent = (stc_delta == 0) ? 0 : ((pcr_delta - stc_delta) * PERCENT_MAX_VALUE / stc_delta);

        if (diff_percent >= PCR_STC_DIFF_MAX_PERCENT) {
            info->pcr_lead_series_cnt++;
            info->pcr_lag_series_cnt = 0;
            HI_INFO_SYNC("diff_percent:%d, pcr_lead_series_cnt:%d\n", diff_percent, info->pcr_lead_series_cnt);
        } else if (diff_percent <= -1 * PCR_STC_DIFF_MAX_PERCENT) {
            info->pcr_lag_series_cnt++;
            info->pcr_lead_series_cnt = 0;
            HI_INFO_SYNC("diff_percent:%d, pcr_lead_series_cnt:%d\n", diff_percent, info->pcr_lag_series_cnt);
        } else {
            info->pcr_lead_series_cnt = 0;
            info->pcr_lag_series_cnt = 0;
        }

        /* pcr abnormal serise count more than PCR_ABNORMAL_MAX_SERIES_COUNT then change SyncRef to Aud */
        if ((info->pcr_lag_series_cnt >= PCR_ABNORMAL_MAX_SERIES_COUNT) ||
            (info->pcr_lag_series_cnt >= PCR_ABNORMAL_MAX_SERIES_COUNT)) {
            ctx->pcr_sync_info.pcr_adjust_type = SYNC_AUD_ADJUST_SCR;
            HI_WARN_SYNC("pcr is abnormal change syncRef to aud\n");
        }

        /* pcr jump too much */
        if ((pcr_delta > PCR_MAX_DELTA) && (diff_percent >= PCR_STC_DIFF_MAX_PERCENT)) {
            HI_INFO_SYNC("pcr pcr_delta %d > PCR_MAX_DELTA %d\n", pcr_delta, PCR_MAX_DELTA);
            info->pcr_series_cnt = 0;
        }
    }

    info->is_pcr_valid = HI_FALSE;
    info->pcr_series_cnt++;

    if (info->pcr_series_cnt >= PTS_SERIES_COUNT) {
        info->is_pcr_valid = HI_TRUE;
    }

    HI_INFO_SYNC("pcr is_pts_valid:%d, pcr_time:%d\n", info->is_pcr_valid, pcr_time);
    return;
}

static void sync_check_pcr_loop_rewind(sync_context *ctx, hi_u32 time)
{
    sync_pcr_info *info = &ctx->pcr_sync_info;

    if ((!info->is_pcr_valid) ||
        (HI_TRUE == ctx->is_pcr_rewind) ||
        (SYNC_INVALID_PTS == info->pcr_last_valid) ||
        (SYNC_REF_PCR != sync_get_sync_adjust_ref_mode(ctx))) {
        return;
    }

    if ((info->pcr_last_valid > time) &&
        (info->pcr_last_valid - time > PTS_LOOPBACK_THRESHOLD)) {
        if ((PCR_TIME_MAX - info->pcr_last_valid  < 1000) && (time < 1000)) {
            HI_WARN_SYNC("pcr warps normally lastpts %d, pts %d\n", info->pcr_last_valid, time);
        } else {
            ctx->is_pcr_rewind = HI_TRUE;
            HI_WARN_SYNC("pcr loopback! lastpcr %d, pcr %d\n", info->pcr_last_valid, time);
        }
    } else if ((HI_TRUE == ctx->is_vid_rewind) &&
               (HI_TRUE == ctx->is_aud_rewind)) {
        if ((sync_abs(time - ctx->aud_last_valid_pts) < PTS_LOOPBACK_THRESHOLD) ||
            (sync_abs(time - ctx->vid_last_valid_pts) < PTS_LOOPBACK_THRESHOLD)) {
            ctx->is_pcr_rewind = HI_TRUE;
            HI_WARN_SYNC("pcr loopback! lastpcr %d, pcr %d\n", info->pcr_last_valid, time);
        }
    }

    return;
}

void sync_pcr_proc(hi_handle sync, hi_u64 pcr, hi_u64 stc)
{
    hi_u32 id = SYNC_GET_ID(sync);
    sync_context *ctx = HI_NULL;
    hi_u32 pcr_delta = 0;
    hi_u32 time_cost = 0;
    hi_u32 pcr_time;
    hi_u32 stc_time;
    hi_u32 local_time = 0;
    hi_u64 pcr_convert;
    hi_u64 stc_convert;

    SYNC_CHECK_ID_NOT_RET(id);

    ctx = sync_info_ctx_get(id);
    if (ctx->cur_status == SYNC_STATUS_STOP) {
        return;
    }

    ctx->cur_pcr_val = pcr;
    ctx->cur_stc_val = stc;
    ctx->is_pcr_stc_changed = HI_TRUE;

    /* PcrValue is sample with the freq of 27Mhz, but the sync module need the freq of 1khz
    * so divide 27000 here
    */
    pcr_convert = osal_div_u64(pcr, SAMPLE_FREQ_DEFAULT_VALUE);
    pcr_time = (hi_u32)(pcr_convert & 0xffffffff);

    stc_convert = osal_div_u64(stc, SAMPLE_FREQ_DEFAULT_VALUE);
    stc_time = (hi_u32)(stc_convert & 0xffffffff);

    /* first pcr */
    if (!ctx->pcr_sync_info.is_pcr_first_come) {
        if ((HI_FALSE == sync_check_pcr_timeout(ctx)) ||
            (HI_INVALID_TIME == ctx->pcr_sync_info.pcr_sync_start_sys_time)) {
            /* fisrt set pcr local  time */
            sync_set_local_time(ctx, SYNC_CHAN_PCR, (pcr_time + ctx->pcr_sync_info.pcr_delta));
            HI_INFO_SYNC(">>>>pcr first set local time: %d\n", (pcr_time + ctx->pcr_sync_info.pcr_delta));
            ctx->pcr_sync_info.is_pcr_adjust_delta_ok = HI_TRUE;
            ctx->pcr_sync_info.pcr_adjust_type = SYNC_PCR_ADJUST_SCR;
        }

        /* second change is_pcr_first_come value to HI_TRUE */
        ctx->pcr_sync_info.is_pcr_first_come = HI_TRUE;
        ctx->pcr_sync_info.pcr_first_sys_time = sync_get_sys_time();
        ctx->pcr_sync_info.pcr_first = pcr_time;
        ctx->pcr_sync_info.pcr_last = pcr_time;

        return;
    }

    (void)sync_check_pcr_valid(ctx, pcr_time, stc_time);
    (void)sync_check_pcr_loop_rewind(ctx, pcr_time);

    ctx->pcr_sync_info.pcr_last = pcr_time;
    ctx->pcr_sync_info.stc_info.stc_last = stc_time;

    if (HI_TRUE == ctx->pcr_sync_info.is_pcr_valid) {
        ctx->pcr_sync_info.pcr_last_valid = pcr_time;
    }

#ifdef SYNC_PCR_ADJUST_STC_SUPPORT
    sync_stc_adjust(ctx, pcr, stc);
#endif

    if (HI_TRUE == ctx->pcr_sync_info.is_pcr_valid) {
        time_cost = sync_get_sys_time_cost(ctx->pcr_sync_info.pcr_last_sys_time);
        if (time_cost != 0) {
            pcr_delta = pcr_time - ctx->pcr_sync_info.pcr_last;
            ctx->pcr_sync_info.pcr_gradient = pcr_delta * 100 / time_cost;
        }

        if (SYNC_PCR_ADJUST_SCR == ctx->pcr_sync_info.pcr_adjust_type) {
            local_time = sync_get_local_time(ctx, SYNC_CHAN_PCR);
            /* keep pcr uniformly continuous when Network jitter occurs */
            if ((sync_abs(local_time - pcr_time - ctx->pcr_sync_info.pcr_delta) < 300) &&
                (ctx->pcr_sync_info.is_pcr_adjust_delta_ok)) {
                return;
            }

            sync_set_local_time(ctx, SYNC_CHAN_PCR, (pcr_time + ctx->pcr_sync_info.pcr_delta));
            ctx->pcr_sync_info.is_pcr_adjust_delta_ok = HI_TRUE;
            HI_INFO_SYNC(">>>>pcr time: %d, pcr_delta: %d \n", pcr_time, ctx->pcr_sync_info.pcr_delta);
            HI_INFO_SYNC(">>>>pcr set local time: %d by pcr\n", (pcr_time + ctx->pcr_sync_info.pcr_delta));
        }
    }

    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
