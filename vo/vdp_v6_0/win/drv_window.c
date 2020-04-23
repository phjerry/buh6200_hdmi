/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_drv_video.h"
#include "hi_drv_disp.h"
#include "hi_drv_win.h"
#include "hi_drv_sys.h"
#include "hi_osal.h"

#include "drv_xdp_osal.h"
#include "drv_window_priv.h"
#include "drv_window.h"

#include "drv_win_common.h"
#include "drv_win_vpss.h"
#include "drv_win_frc.h"
#include "drv_win_fence.h"
#include "drv_win_buffer.h"
#include "drv_win_attrprocess.h"
#include "drv_win_frameinfo.h"
#include "drv_window_priv.h"
#include "drv_win_mng.h"
#include "drv_win_proc.h"

#include "win_layer_mapping_policy.h"

#include "drv_display.h"
#include "hal_layer_intf.h"
#include "linux/hisilicon/securec.h"

#include "drv_xdp_ion.h"
#include "drv_xdp_list.h"

#include <linux/ion.h>

#define VDP_FRAME_WIDTH_4K 3840
#define VDP_FRAME_HEIGHT_4K 2160

#define COMMON_MAGIC_CODE 0xdeadbeaf
#define FENCE_TIMEOUT_VALUE 4000
#define GET_WIN_PTR(win_handle, win_ptr) do{ \
        win_ptr = winmanage_get_window(win_handle);     \
        if ((win_ptr) == HI_NULL) { \
            WIN_WARN("WIN is not exist! %#x\n", win_handle); \
            return HI_ERR_VO_WIN_NOT_EXIST; \
        }  \
} while (0)

hi_void isr_win_post_display_config(hi_void *hDst, const hi_drv_disp_callback_info *pstInfo);
hi_void isr_win_post_release_frame(hi_void *hDst, const hi_drv_disp_callback_info *pstInfo);

static hi_void win_dfx_record_frame_time(hi_drv_video_frame *frame_info, win_record_frame_time record_point)
{
    switch (record_point) {
        case RECORD_QUEUE_FRAME_TIME:
            frame_info->lowdelay_stat.win_get_frame_time = hi_drv_sys_get_time_stamp_ms();
            frame_info->lowdelay_stat.win_config_time = 0;
            frame_info->lowdelay_stat.win_rls_frame_time = 0;
            break;
        case RECORD_CONFIG_FRAME_TIME:

            if (frame_info->lowdelay_stat.win_config_time == 0) {
                frame_info->lowdelay_stat.win_config_time = hi_drv_sys_get_time_stamp_ms();
            }
            break;
        case RECORD_RLS_FRAME_TIME:
            frame_info->lowdelay_stat.win_rls_frame_time = hi_drv_sys_get_time_stamp_ms();
            break;
        default:
            break;
    }

    return;
}

static hi_void win_dfx_inc_acq_frame_cnt(win_descriptor *win_descp)
{
    win_descp->win_play_status.aquire_frame_cnt++;
    return;
}

static hi_void win_dfx_inc_rls_frame_cnt(win_descriptor *win_descp)
{
    win_descp->win_play_status.rls_frame_cnt++;
    return;
}

static hi_void win_dfx_record_queue_interval(win_descriptor *win_descp)
{
    hi_u32 interval;
    if (win_descp->win_play_status.last_queue_time == 0) {
        win_descp->win_play_status.last_queue_time = hi_drv_sys_get_time_stamp_ms();
        win_descp->win_play_status.current_queue_time = hi_drv_sys_get_time_stamp_ms();
    } else {
        win_descp->win_play_status.last_queue_time = win_descp->win_play_status.current_queue_time;
        win_descp->win_play_status.current_queue_time = hi_drv_sys_get_time_stamp_ms();
    }

    interval = win_descp->win_play_status.current_queue_time - win_descp->win_play_status.last_queue_time;
    win_descp->win_play_status.queue_interval = interval;

    if (win_descp->win_play_status.max_queue_interval < interval) {
        win_descp->win_play_status.max_queue_interval = interval;
    }

    return;
}

static hi_void win_dfx_record_frame_staytime(win_descriptor *win_descp, hi_drv_video_frame *frame_info)
{
    hi_u32 frame_staytime;

    frame_staytime = frame_info->lowdelay_stat.win_rls_frame_time - frame_info->lowdelay_stat.win_get_frame_time;

    if ((win_descp->win_play_status.min_frame_stay_time == 0) ||
        (win_descp->win_play_status.min_frame_stay_time > frame_staytime)) {
        win_descp->win_play_status.min_frame_stay_time = frame_staytime;
    }

    if ((win_descp->win_play_status.max_frame_stay_time == 0) ||
        (win_descp->win_play_status.max_frame_stay_time < frame_staytime)) {
        win_descp->win_play_status.max_frame_stay_time = frame_staytime;
    }

    win_descp->win_play_status.newest_frame_stay_time = frame_staytime;
    return;
}

static hi_void win_dfx_src_fence_timeout_cnts(win_descriptor *win_descp, hi_s32 source_fence)
{
    win_descp->win_play_status.src_fence_timeout_cnts++;
    win_descp->win_play_status.src_fence_timeout_fd = source_fence;
    return;
}

static hi_void win_dfx_src_fence_destroy_cnts(win_descriptor *win_descp)
{
    win_descp->win_play_status.src_fence_destroy_cnts++;
    return;
}

static hi_void win_dfx_src_fence_access_cnts(win_descriptor *win_descp)
{
    win_descp->win_play_status.src_fence_access_cnts++;
    return;
}

static hi_void win_dfx_src_fence_rls_cnts(win_descriptor *win_descp)
{
    win_descp->win_play_status.src_fence_rls_cnts++;
    return;
}

static hi_void win_dfx_src_rls_atonce_cnts(win_descriptor *win_descp)
{
    win_descp->win_play_status.src_rls_atonce_cnts++;
    return;
}

static hi_void win_dfx_sink_fence_creat_cnts(win_descriptor *win_descp)
{
    win_descp->win_play_status.sink_fence_create_cnts++;
    return;
}
static hi_void win_dfx_sink_fence_signal_cnts(win_descriptor *win_descp)
{
    win_descp->win_play_status.sink_fence_signal_cnts++;
    return;
}

static hi_void win_dfx_record_error_frame_cnts(win_descriptor *win_descp)
{
    win_descp->win_play_status.err_frame_counts++;
    return;
}

static hi_void win_dfx_record_tplayinfo(win_descriptor *win_descp, hi_u32 tplay_speed_integer,
    hi_u32 tplay_speed_decimal)
{
    win_descp->win_play_status.tplay_speed_integer = tplay_speed_integer;
    win_descp->win_play_status.tplay_speed_decimal = tplay_speed_decimal;
    return;
}

static hi_s32 win_create_frame_rls_fence(win_descriptor *win_descp, hi_s32 *fence_fd, hi_bool rls_frame_atonce,
    hi_drv_video_frame *frame_info, hi_u64 *fence_descp)
{
    hi_s32 ret = HI_SUCCESS;

    COMMON_DEBUG(BUFFER_FENCE_INFOR, " src_addr_fd(0x%llx) src_fence(%d)",
                 frame_info->buf_addr[0].start_addr, frame_info->source_fence);
    frame_info->sink_fence = -1;

    if ((fence_fd != HI_NULL) && (rls_frame_atonce != HI_TRUE)) {
        *fence_fd = -1;
        ret = drv_fence_create_fence(win_descp->win_component.h_fence, frame_info->buf_addr[0].dma_handle, fence_descp);
        if (ret <= 0) {
            WIN_ERROR("Creat android fence error\n");
            return HI_ERR_VO_CREATE_ERR;
        }

        *fence_fd = ret;
        COMMON_DEBUG(ANDROID_FENCE_INFOR, "creat src_addr_fd(0x%llx) .fence_fd(%d)\n",
                     frame_info->buf_addr[0].start_addr, *fence_fd);
    }

    if (frame_info->source_fence < 0) {
        /* if no source fence, no src_fence either. */
        frame_info->sink_fence = -1;
        COMMON_DEBUG(BUFFER_FENCE_INFOR, " no sink fence\n");
        return HI_SUCCESS;
    }

    if (rls_frame_atonce == HI_FALSE) {
        ret = osal_fence_create(HI_ID_WIN, &frame_info->sink_fence, sizeof(hi_drv_fence_info));
        if (ret < 0) {
            WIN_ERROR("Creat fence error\n");
            frame_info->sink_fence = -1;
            ret = HI_ERR_VO_CREATE_ERR;
        } else {
            COMMON_DEBUG(BUFFER_FENCE_INFOR, " creat sink_fence %d\n", frame_info->sink_fence);
            win_dfx_sink_fence_creat_cnts(win_descp);
        }
    } else {
        COMMON_DEBUG(BUFFER_FENCE_INFOR, " atonce release\n");
        win_dfx_src_rls_atonce_cnts(win_descp);
    }

    return ret;
}

static hi_void win_destroy_fence(hi_drv_video_frame *frame_info, hi_s32 *fence_fd)
{
    if ((fence_fd != HI_NULL) && (*fence_fd > 0)) {
        hi_close_fd(*fence_fd);
        *fence_fd = -1;
    }

    if (frame_info->sink_fence > 0) {
        osal_fence_destroy(HI_ID_WIN, frame_info->sink_fence);
        COMMON_DEBUG(BUFFER_FENCE_INFOR, "destroy src_fence %d\n", frame_info->sink_fence);
        frame_info->sink_fence = -1; /* after destroy, just assign it to invalid. */
    }

    return;
}

static hi_s32 win_check_frame(hi_drv_video_frame *frame_info)
{
    if ((frame_info->video_3d_type < HI_DRV_3D_NONE) || (frame_info->video_3d_type >= HI_DRV_3D_MAX)) {
        WIN_FATAL("Q Frame eFrmType error : %d, HI_DRV_3D_MAX is %d.\n", frame_info->video_3d_type, HI_DRV_3D_MAX);
        return HI_ERR_VO_INVALID_PARA;
    }

    if ((frame_info->bit_width < HI_DRV_PIXEL_BITWIDTH_8BIT) || (frame_info->bit_width >= HI_DRV_PIXEL_BITWIDTH_MAX)) {
        WIN_FATAL("Q Frame enBitWidth error : %d, HI_DRV_PIXEL_BITWIDTH_MAX is %d.\n", frame_info->bit_width,
            HI_DRV_PIXEL_BITWIDTH_MAX);
        return HI_ERR_VO_INVALID_PARA;
    }

    if (!((HI_DRV_PIXEL_FMT_NV12 == frame_info->pixel_format) || (HI_DRV_PIXEL_FMT_NV21 == frame_info->pixel_format) ||
        (HI_DRV_PIXEL_FMT_NV61_2X1 == frame_info->pixel_format) ||
        (HI_DRV_PIXEL_FMT_NV42 == frame_info->pixel_format) || (HI_DRV_PIXEL_FMT_RGB24 == frame_info->pixel_format) ||
        (HI_DRV_PIXEL_FMT_ARGB8888 == frame_info->pixel_format))) {
        WIN_FATAL("Q Frame pixformat error : %d\n", frame_info->pixel_format);
        return HI_ERR_VO_INVALID_PARA;
    }

    if ((frame_info->width < WIN_FRAME_MIN_WIDTH) || (frame_info->width > WIN_FRAME_MAX_WIDTH) ||
        (frame_info->height < WIN_FRAME_MIN_HEIGHT) || (frame_info->height > WIN_FRAME_MAX_HEIGHT)) {
        WIN_FATAL("Q Frame resolution error : w=%d,h=%d\n", frame_info->width, frame_info->height);
        return HI_ERR_VO_INVALID_PARA;
    }

    if (frame_info->buf_addr[0].dma_handle == 0) {
        WIN_ERROR("Null frame y addr!\n");
        return HI_ERR_VO_INVALID_PARA;
    }

    if ((frame_info->bit_width == HI_DRV_PIXEL_BITWIDTH_10BIT) &&
        (frame_info->cmp_info.data_fmt == HI_DRV_DATA_FMT_TILE) && ((frame_info->buf_addr_lb[0].dma_handle == 0))) {
        WIN_ERROR("Null frame low 2bit c addr!\n");
        return HI_ERR_VO_INVALID_PARA;
    }

    return HI_SUCCESS;
}

static hi_void win_zorder_save(hi_u32 *zorder_bak, hi_handle *win_array)
{
    hi_u32 i;
    win_descriptor *tmp_win = HI_NULL;

    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        if (win_array[i] == 0) {
            continue;
        }
        tmp_win = winmanage_get_window(win_array[i]);
        zorder_bak[i] = tmp_win->expected_win_attr.win_zorder;
    }

    return;
}

static hi_void win_zorder_restore(hi_u32 *zorder_bak, hi_handle *win_array)
{
    hi_u32 i;
    win_descriptor *tmp_win = HI_NULL;

    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        if (win_array[i] == 0) {
            continue;
        }
        tmp_win = winmanage_get_window(win_array[i]);
        tmp_win->expected_win_attr.win_zorder = zorder_bak[i];
    }

    return;
}

static hi_s32 check_frame_ready(win_descriptor *win_descp, hi_drv_video_frame *frame_info)
{
    hi_s32 ret;
    hi_drv_fence_info *fence_info = HI_NULL;

    COMMON_DEBUG(BUFFER_FENCE_INFOR, "check src_addr_fd(0x%llx), src_fence :(%d)",
                 frame_info->buf_addr[0].start_addr, frame_info->source_fence);

    if (frame_info->source_fence < 0) {
        COMMON_DEBUG(BUFFER_FENCE_INFOR, " source fence err \n");
        return HI_SUCCESS;
    }

    ret = osal_fence_trywait(frame_info->source_fence);
    if (ret == OSAL_FENCE_ERR_NOT_READY) {

        win_dfx_src_fence_timeout_cnts(win_descp, frame_info->source_fence);
        COMMON_DEBUG(BUFFER_FENCE_INFOR, " fence_trywait time out \n");
        return HI_ERR_VO_TIMEOUT;
    } else if (ret == -1) {
        COMMON_DEBUG(BUFFER_FENCE_INFOR, " fence_trywait ret %d\n", ret);
        return HI_ERR_VO_FRAME_INFO_ERROR;
    }

    ret = osal_fence_acquire_private(HI_ID_WIN, frame_info->source_fence, (void *)&fence_info);
    if (ret < 0) {
        COMMON_DEBUG(BUFFER_FENCE_INFOR, "get private failed.\n");
        return HI_ERR_VO_FRAME_INFO_ERROR;
    }

    win_dfx_src_fence_access_cnts(win_descp);
    ret = osal_fence_release_private(HI_ID_WIN, frame_info->source_fence, (void *)fence_info);
    if (ret != 0) {
        COMMON_DEBUG(BUFFER_FENCE_INFOR, "rls private err.\n");
    }

    win_dfx_src_fence_rls_cnts(win_descp);

    if (fence_info->need_display != HI_TRUE) {
        COMMON_DEBUG(BUFFER_FENCE_INFOR, "private:not display\n");
        win_dfx_record_error_frame_cnts(win_descp);
        return HI_ERR_VO_FRAME_INFO_ERROR;
    }

    return HI_SUCCESS;
}

static hi_void update_cfg_frame(win_descriptor *win_descp, hi_drv_video_frame *frame_info)
{
    hi_drv_win_vpss_meta_info *metadata_info = HI_NULL;
    hi_drv_video_frame *config_frame;

    if ((win_descp->win_play_status.config_frame != HI_NULL) &&
        (win_descp->win_play_status.config_frame != frame_info)) {
        config_frame = win_descp->win_play_status.config_frame;
        metadata_info = (hi_drv_win_vpss_meta_info *)config_frame->video_private.vpss_private.data;

        if (metadata_info->is_displayed == HI_FALSE) {
            WIN_WARN("config is not displayed, frame_index:%08x\n", win_descp->win_play_status.config_frame->frame_index);
            win_buffer_release_frame(win_descp->win_component.h_buf, win_descp->win_play_status.config_frame);
        }
    }

    win_descp->win_play_status.config_frame = frame_info;
    if (frame_info != HI_NULL) {
        metadata_info = (hi_drv_win_vpss_meta_info *)frame_info->video_private.vpss_private.data;

        if (metadata_info->play_cnt > 0) {
            metadata_info->play_cnt--;
        }
    }

    return;
}

static hi_void drv_win_avsync_adjust(win_descriptor *win_descp, hi_drv_win_vpss_meta_info *metadata_info)
{
    hi_u32 frame_num;
    hi_u32 frame_play_cnt, need_sync_cnt;

    (hi_void)win_buffer_get_full_frame_info(win_descp->win_component.h_buf, &frame_num, &frame_play_cnt);

    if ((frame_num != 0) && (metadata_info->play_cnt > 0) && (win_descp->avsync_info.need_sync_adjust != 0)) {
        if (win_descp->avsync_info.need_sync_adjust < 0) {
            need_sync_cnt = abs(win_descp->avsync_info.need_sync_adjust);

            if (metadata_info->play_cnt > need_sync_cnt) {
                metadata_info->play_cnt -= need_sync_cnt;
                win_descp->avsync_info.need_sync_adjust = 0;
                win_descp->avsync_info.debug_discard_cnt += need_sync_cnt;
            } else {
                win_descp->avsync_info.need_sync_adjust += metadata_info->play_cnt;
                win_descp->avsync_info.debug_discard_cnt += metadata_info->play_cnt;
                metadata_info->play_cnt = 0;
            }
        } else {
            metadata_info->play_cnt += win_descp->avsync_info.need_sync_adjust;
            win_descp->avsync_info.need_sync_adjust = 0;
            win_descp->avsync_info.debug_repeat_cnt += win_descp->avsync_info.need_sync_adjust;
        }
    }

    return;
}

hi_drv_video_frame *win_get_sync_frame(win_descriptor *win_descp)
{
    hi_drv_video_frame *frame_info = HI_NULL;
    hi_drv_win_vpss_meta_info *metadata_info = HI_NULL;
    hi_s32 ret = HI_FAILURE;
    hi_u64 dma_handle = 0;

    /* if frame not  finished, just continue config it */
    frame_info = win_descp->win_play_status.config_frame;
    if (frame_info != HI_NULL) {
        metadata_info = (hi_drv_win_vpss_meta_info *)frame_info->video_private.vpss_private.data;

        if (metadata_info->play_cnt > 0) {
            metadata_info->play_cnt--;
            COMMON_DEBUG(BUFFER_INFOR, "get src_addr_fd(0x%llx),cnt(%d).\n",
                         frame_info->buf_addr[0].start_addr, metadata_info->play_cnt);
            return frame_info;
        }
    }

    if (win_descp->win_play_status.config_frame != HI_NULL) {
        dma_handle = win_descp->win_play_status.config_frame->buf_addr[0].dma_handle;
    }
    if (win_descp->win_play_status.is_stepmode == HI_TRUE) {
        if (win_descp->win_play_status.is_next_frm != HI_TRUE) {
            return win_descp->win_play_status.config_frame;
        } else {
            win_descp->win_play_status.is_next_frm = HI_FALSE;
        }
    }

    while (1) {
        (hi_void)win_buffer_check_full_frame(win_descp->win_component.h_buf, &frame_info);
        if (frame_info != HI_NULL) {
            COMMON_DEBUG(BUFFER_INFOR, "got src_addr_fd(0x%llx):", frame_info->buf_addr[0].start_addr);
            /* check the frame  fence ready. */
            ret = check_frame_ready(win_descp, frame_info);
            if (ret == HI_ERR_VO_TIMEOUT) {
                frame_info = win_descp->win_play_status.config_frame;
                update_cfg_frame(win_descp, frame_info);
                COMMON_DEBUG(BUFFER_INFOR, "try fence time out.\n");
                return win_descp->win_play_status.config_frame;
            } else if (ret == HI_ERR_VO_FRAME_INFO_ERROR) {
                win_buffer_acquire_frame(win_descp->win_component.h_buf, &frame_info);
                COMMON_DEBUG(BUFFER_INFOR, "try fence frame info err.\n");
                win_buffer_release_frame(win_descp->win_component.h_buf, frame_info);
                continue;
            }

            win_buffer_acquire_frame(win_descp->win_component.h_buf, &frame_info);
            /* pause or freeze get same bufer */
            if ((frame_info == HI_NULL) || (frame_info->buf_addr[0].dma_handle == dma_handle)) {
                break;
            }

            metadata_info = (hi_drv_win_vpss_meta_info *)frame_info->video_private.vpss_private.data;
            metadata_info->is_displayed = HI_FALSE;
            drv_win_avsync_adjust(win_descp, metadata_info);
            if (metadata_info->play_cnt != 0) {
                break;
            }

            /* drop the useless frame */
            win_buffer_release_frame(win_descp->win_component.h_buf, frame_info);
        } else {
            COMMON_DEBUG(BUFFER_INFOR, "got null frame.\n");
            win_buffer_acquire_frame(win_descp->win_component.h_buf, &frame_info);
            break;
        }
    }

    update_cfg_frame(win_descp, frame_info);
    return win_descp->win_play_status.config_frame;
}

static hi_drv_video_frame *win_get_repeat_frame(win_descriptor *win_descp)
{
    return win_descp->win_play_status.config_frame;
}

static hi_drv_video_frame *win_get_lastest_frame(win_descriptor *win_descp)
{
    hi_drv_video_frame *frame_info = HI_NULL;
    hi_s32 ret;

    ret = win_buffer_acquire_lastest_frame(win_descp->win_component.h_buf, &frame_info);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("get last frame null \n");
        return HI_NULL;
    }

    return frame_info;
}

#define WIN_AVSYNC_TIME_DIFF_THRESHOLD_MS 2
hi_drv_video_frame *drv_win_update_cfg_frame(hi_handle win_handle,
                                             win_framesync_result framesync_result)
{
    hi_drv_video_frame *frame_info = HI_NULL;
    win_descriptor *win_descp = HI_NULL;
    hi_drv_win_vpss_meta_info *metadata_info = HI_NULL;
    hi_u32 actual_time;
    hi_drv_disp_timing_status current_timing_status = {0};
    hi_s32 sync_adjust_num = 0;

    win_descp = winmanage_get_window(win_handle);
    if (win_descp == HI_NULL) {
        WIN_WARN("WIN is not exist! %#x\n", win_handle);
        return HI_NULL;
    }
    COMMON_DEBUG(BUFFER_INFOR, "cfg sync mode :%d.\n", win_descp->avsync_info.sync_mode);

    switch (win_descp->avsync_info.sync_mode) {
        case HI_DRV_WIN_SYNC_PLAY_LASTEST:
            frame_info = win_get_lastest_frame(win_descp);
            break;
        case HI_DRV_WIN_SYNC_PLAY_LAST:
            frame_info = win_get_repeat_frame(win_descp);
            break;
        case HI_DRV_WIN_SYNC_DISCARD:
        case HI_DRV_WIN_SYNC_REPEAT:
        case HI_DRV_WIN_SYNC_PLAY:
        default:
            frame_info = win_get_sync_frame(win_descp);
            break;
    }

    win_descp->win_play_status.config_frame = frame_info;

    /* update display frame sync infomtison */
    if (frame_info != HI_NULL) {
        metadata_info = (hi_drv_win_vpss_meta_info *)frame_info->video_private.vpss_private.data;
        metadata_info->debug_cfg_time  = hi_drv_sys_get_time_stamp_ms();
        metadata_info->played_cnt ++;

        if (metadata_info->played_cnt < metadata_info->frc_play_cnt) {
            win_descp->avsync_info.display_frame_play_index = metadata_info->play_index + (metadata_info->played_cnt - 1);
        } else {
            win_descp->avsync_info.display_frame_play_index = metadata_info->play_index + (metadata_info->frc_play_cnt - 1);
        }

        win_descp->avsync_info.display_frame_need_play_cnt = metadata_info->play_cnt;
        win_descp->avsync_info.display_frame_pts = frame_info->src_pts;
        win_descp->avsync_info.display_frame_disp_time = frame_info->video_private.priv_disp_time;
        (hi_void)hi_drv_disp_get_current_timing_status(win_descp->win_user_attr.disp_chn, &current_timing_status);

        sync_adjust_num = win_descp->avsync_info.total_sync_adjust_cnts - metadata_info->sync_adjust_cnts;
        if (sync_adjust_num < 0) {
            metadata_info->debug_delay_time -= abs(sync_adjust_num) * current_timing_status.circle_time_us/ISR_SYSTEM_TIME_US;
        } else {
            metadata_info->debug_delay_time += abs(sync_adjust_num) * current_timing_status.circle_time_us/ISR_SYSTEM_TIME_US;
        }

        /* just make up for one frame's secondary play, since the expect time is for the first play of the same frame. */
        metadata_info->debug_delay_time += (metadata_info->played_cnt - 1) * current_timing_status.circle_time_us/ISR_SYSTEM_TIME_US ;

        actual_time = metadata_info->debug_cfg_time - metadata_info->debug_queue_time + current_timing_status.left_time;
        if (abs(actual_time - metadata_info->debug_delay_time) > WIN_AVSYNC_TIME_DIFF_THRESHOLD_MS) {
            COMMON_DEBUG(AVSYNC_INFOR,"idx:%d, t-diff:%d, sync sum:%d, frm sync:%d, difcnt:%d, ctime:%d, %d, %d,%d!\n",
                         metadata_info->play_index,
                         (actual_time - metadata_info->debug_delay_time),
                         win_descp->avsync_info.total_sync_adjust_cnts,
                         metadata_info->sync_adjust_cnts,
                         sync_adjust_num,
                         current_timing_status.circle_time_us,
                         abs(sync_adjust_num) * current_timing_status.circle_time_us/ISR_SYSTEM_TIME_US,
                         actual_time,
                         metadata_info->debug_delay_time);
        }

        COMMON_DEBUG(AVSYNC_INFOR,"cfgidx:%d, qti:%d, cfgti:%d,leftti:%d, act ti:%d, exp ti:%d, adr:%p,jif:%ld!\n",
                     metadata_info->play_index,
                     metadata_info->debug_queue_time,
                     metadata_info->debug_cfg_time,
                     current_timing_status.left_time,
                     actual_time,
                     metadata_info->debug_delay_time,
                     metadata_info,
                     jiffies);
    }

    win_descp->win_misc_status.is_update_frame_in_cycle = HI_TRUE;
    return frame_info;
}


static hi_s32 win_increase_ref_cnt(hi_drv_video_frame *frame_info)
{
    struct dma_buf *_dma_buf;

    _dma_buf = (struct dma_buf *)frame_info->buf_addr[HI_DRV_3D_EYE_LEFT].dma_handle;
    if (_dma_buf != HI_NULL) {
        drv_xdp_mem_increase_refcnt(_dma_buf);
    }

    _dma_buf = (struct dma_buf *)frame_info->buf_addr[HI_DRV_3D_EYE_RIGHT].dma_handle;
    if (_dma_buf != HI_NULL) {
        drv_xdp_mem_increase_refcnt(_dma_buf);
    }

    _dma_buf = (struct dma_buf *)frame_info->buf_addr_lb[HI_DRV_3D_EYE_LEFT].dma_handle;
    if (_dma_buf != HI_NULL) {
        drv_xdp_mem_increase_refcnt(_dma_buf);
    }

    _dma_buf = (struct dma_buf *)frame_info->buf_addr_lb[HI_DRV_3D_EYE_RIGHT].dma_handle;
    if (_dma_buf != HI_NULL) {
        drv_xdp_mem_increase_refcnt(_dma_buf);
    }

    return HI_SUCCESS;
}

static hi_void win_decrease_ref_cnt(hi_drv_video_frame *frame_info)
{
    struct dma_buf *_dma_buf;

    _dma_buf = (struct dma_buf *)frame_info->buf_addr[HI_DRV_3D_EYE_LEFT].dma_handle;
    if (_dma_buf != HI_NULL) {
        drv_xdp_mem_decrease_refcnt(_dma_buf);
    }

    _dma_buf = (struct dma_buf *)frame_info->buf_addr[HI_DRV_3D_EYE_RIGHT].dma_handle;
    if (_dma_buf != HI_NULL) {
        drv_xdp_mem_decrease_refcnt(_dma_buf);
    }

    _dma_buf = (struct dma_buf *)frame_info->buf_addr_lb[HI_DRV_3D_EYE_LEFT].dma_handle;
    if (_dma_buf != HI_NULL) {
        drv_xdp_mem_decrease_refcnt(_dma_buf);
    }

    _dma_buf = (struct dma_buf *)frame_info->buf_addr_lb[HI_DRV_3D_EYE_RIGHT].dma_handle;
    if (_dma_buf != HI_NULL) {
        drv_xdp_mem_decrease_refcnt(_dma_buf);
    }

    return;
}

static hi_s32 release_src_frame(win_descriptor *win_descp, hi_drv_video_frame *frame_info)
{
    hi_drv_win_vpss_meta_info *metadata_info;

    /* decrease the ref cnt. */
    win_decrease_ref_cnt(frame_info);
    COMMON_DEBUG(BUFFER_INFOR, "release src_addr_fd(0x%llx):", frame_info->buf_addr[0].start_addr);

    if (win_descp->win_play_status.is_tunnel_fence == HI_TRUE) {
        if ((frame_info->source_fence >= 0) && (frame_info->sink_fence >= 0)) {
            win_dfx_src_fence_destroy_cnts(win_descp);
            osal_fence_destroy(HI_ID_WIN, frame_info->source_fence);
            COMMON_DEBUG(BUFFER_FENCE_INFOR, "destroy src_fence %d\n", frame_info->source_fence);
        }

        if (frame_info->sink_fence >= 0) {
            win_dfx_sink_fence_signal_cnts(win_descp);
            osal_fence_signal(frame_info->sink_fence);
            COMMON_DEBUG(BUFFER_FENCE_INFOR, "signal sink_fence %d\n", frame_info->sink_fence);
        }
    }

    metadata_info = (hi_drv_win_vpss_meta_info *)frame_info->video_private.win_private.data;
    if (metadata_info != HI_NULL) {
        if (metadata_info->android_fence_fd != 0) {
            drv_win_transfer_dma_to_fd(frame_info, HI_FALSE);
            (hi_void)drv_fence_signal(win_descp->win_component.h_fence, (hi_void *)metadata_info->android_fence_fd);
            COMMON_DEBUG(ANDROID_FENCE_INFOR, "signal fence_fd(0x%llx)\n", metadata_info->android_fence_fd);
        }
    }

    /* record the dfx info */
    win_dfx_record_frame_time(frame_info, RECORD_RLS_FRAME_TIME);
    win_dfx_inc_rls_frame_cnt(win_descp);
    win_dfx_record_frame_staytime(win_descp, frame_info);

    return HI_SUCCESS;
}

static hi_s32 win_vpss_rls_src_frame(win_descriptor *win_descp, hi_drv_video_frame *frame_info)
{
    hi_s32 ret;
    hi_u64 mentadata_addr;

    mentadata_addr = frame_info->video_private.win_private.data;
    ret = release_src_frame(win_descp, frame_info);
    win_buffer_release_metadata(win_descp->win_component.h_buf, mentadata_addr);

    return ret;
}

hi_void drv_win_update_disp_frame(hi_handle win_handle)
{
    win_descriptor *win_descp = HI_NULL;
    hi_drv_video_frame *tmp_disp_frame = HI_NULL;
    hi_s32 ret = HI_FAILURE;
    hi_drv_win_vpss_meta_info *win_meta_data;

    win_descp = winmanage_get_window(win_handle);
    if (win_descp == HI_NULL) {
        WIN_WARN("win is not exist! %#x\n", win_handle);
        return;
    }

    if (win_descp->win_play_status.config_frame != HI_NULL) {
        tmp_disp_frame = win_descp->win_play_status.config_frame;
        win_meta_data = (hi_drv_win_vpss_meta_info *)tmp_disp_frame->video_private.vpss_private.data;
        win_meta_data->is_displayed = HI_TRUE;
    }

    tmp_disp_frame = win_descp->win_play_status.displaying_frame;
    if (tmp_disp_frame == HI_NULL) {
        win_descp->win_play_status.displaying_frame = win_descp->win_play_status.config_frame;
    } else {
        if (tmp_disp_frame != win_descp->win_play_status.config_frame) {
            ret = win_buffer_release_frame(win_descp->win_component.h_buf, tmp_disp_frame);
            if (ret != HI_SUCCESS) {
                WIN_FATAL("rls frame failed\n");
            }
            win_descp->win_play_status.displaying_frame = win_descp->win_play_status.config_frame;
        }
    }

    win_descp->win_misc_status.is_update_frame_in_cycle = HI_FALSE;
    return;
}

hi_s32 drv_win_get_status(hi_handle win_handle, hi_bool *win_enable)
{
    win_descriptor *win_descp = HI_NULL;

    win_descp = winmanage_get_window(win_handle);
    if (win_descp == HI_NULL) {
        WIN_WARN("WIN is not exist! %#x\n", win_handle);
        return HI_ERR_VO_NOT_EXIST;
    }

    *win_enable = win_descp->win_play_ctrl.win_enable;

    return HI_SUCCESS;
}

static hi_s32 transfer_fd2dma(hi_drv_vid_frame_addr *buf_addr, hi_bool is_producer)
{
    if (buf_addr->start_addr > 0) {
        buf_addr->dma_handle = (hi_u64)drv_xdp_mem_get_dmabuf(buf_addr->start_addr);
        if (buf_addr->dma_handle == HI_NULL) {
            return HI_FAILURE;
        }
    } else {
        buf_addr->dma_handle = HI_NULL;
    }

    if (is_producer == HI_TRUE) {
        if (buf_addr->start_addr > 0) {
            drv_xdp_mem_close_fd(buf_addr->start_addr);
            buf_addr->start_addr = -1;
        }

        buf_addr->start_addr = -1;

        COMMON_DEBUG(BUFFER_INFOR, "y:0x%llx \n", buf_addr->dma_handle);
        COMMON_DEBUG(BUFFER_INFOR, "begin to -1 | y:0x%llx \n", buf_addr->dma_handle);

        drv_xdp_mem_decrease_refcnt((struct dma_buf *)buf_addr->dma_handle);
        COMMON_DEBUG(BUFFER_INFOR, "end to -1 | y:0x%llx \n", buf_addr->dma_handle);
    }

    return HI_SUCCESS;
}

static hi_void fd2dma_err_deal(hi_drv_vid_frame_addr *buf_addr, hi_bool is_producer)
{
    if (is_producer == HI_TRUE) {
        return;
    }

    if (buf_addr->dma_handle != HI_NULL) {
        COMMON_DEBUG(BUFFER_INFOR, "y:0x%llx \n", buf_addr->dma_handle);
        COMMON_DEBUG(BUFFER_INFOR, "begin to -1 | y:0x%llx \n", buf_addr->dma_handle);

        drv_xdp_mem_decrease_refcnt((struct dma_buf *)buf_addr->dma_handle);
        COMMON_DEBUG(BUFFER_INFOR, "end to -1 | y:0x%llx \n", buf_addr->dma_handle);

        buf_addr->dma_handle = HI_NULL;
    }

    return;
}

static hi_s32 transfer_dma2fd(hi_drv_vid_frame_addr *buf_addr, hi_bool is_producer)
{
    /* aquire data from vdp ,vdp is producer. */
    if (is_producer == HI_TRUE) {
        buf_addr->start_addr = drv_xdp_mem_get_fd((struct dma_buf *)buf_addr->dma_handle);
        if (buf_addr->start_addr <= 0) {
            return HI_FAILURE;
        }
    } else { /* dequeue data from vdp ,vdp is consumer. */
        if (buf_addr->dma_handle != HI_NULL) {
            drv_xdp_mem_decrease_refcnt((struct dma_buf *)buf_addr->dma_handle);
            buf_addr->dma_handle = HI_NULL;
        }
    }

    return HI_SUCCESS;
}

static hi_void dma2fd_err_deal(hi_drv_vid_frame_addr *buf_addr, hi_bool is_producer)
{
    if (is_producer == HI_TRUE) {
        if (buf_addr->start_addr >= 0) {
            drv_xdp_mem_close_fd(buf_addr->start_addr);
            buf_addr->start_addr = -1;
        }
    }

    return;
}

hi_s32 drv_win_transfer_fd_to_dma(hi_drv_video_frame *frame_info, hi_bool is_producer)
{
    hi_s32 ret;

    ret = transfer_fd2dma(&frame_info->buf_addr[HI_DRV_3D_EYE_LEFT], is_producer);
    if (ret != HI_SUCCESS) {
        goto __TRANS_FD2DMA_ERR_;
    }

    if (is_producer == HI_FALSE) {
        ret = transfer_fd2dma(&frame_info->buf_addr_lb[HI_DRV_3D_EYE_LEFT], is_producer);
        if (ret != HI_SUCCESS) {
            goto __TRANS_FD2DMA_ERR_;
        }
    } else {
        /* vdp or vpss not product 8+2 bit data , dont convernt this */
        frame_info->buf_addr_lb[HI_DRV_3D_EYE_LEFT].dma_handle = 0;
    }
#ifdef WIN_3D_SUPPORT
    ret = transfer_fd2dma(&frame_info->buf_addr[HI_DRV_3D_EYE_RIGHT]);
    if (ret != HI_SUCCESS) {
        goto __TRANS_FD2DMA_ERR_;
    }

    ret = transfer_fd2dma(&frame_info->buf_addr_lb[HI_DRV_3D_EYE_RIGHT]);
    if (ret != HI_SUCCESS) {
        goto __TRANS_FD2DMA_ERR_;
    }
#endif

    return HI_SUCCESS;
__TRANS_FD2DMA_ERR_:
    fd2dma_err_deal(&frame_info->buf_addr[HI_DRV_3D_EYE_LEFT], is_producer);
    fd2dma_err_deal(&frame_info->buf_addr_lb[HI_DRV_3D_EYE_LEFT], is_producer);

#ifdef WIN_3D_SUPPORT
    fd2dma_err_deal(&frame_info->buf_addr[HI_DRV_3D_EYE_RIGHT]);
    fd2dma_err_deal(&frame_info->buf_addr_lb[HI_DRV_3D_EYE_RIGHT]);
#endif

    return HI_ERR_VO_NULL_PTR;
}


hi_s32 drv_win_transfer_dma_to_fd(hi_drv_video_frame *frame_info, hi_bool is_producer)
{
    hi_s32 ret;

    ret = transfer_dma2fd(&frame_info->buf_addr[HI_DRV_3D_EYE_LEFT], is_producer);
    if (ret != HI_SUCCESS) {
        goto __TRANS_DMA2FD_ERR_;
    }

    if (is_producer == HI_FALSE) {
        ret = transfer_dma2fd(&frame_info->buf_addr_lb[HI_DRV_3D_EYE_LEFT], is_producer);
        if (ret != HI_SUCCESS) {
            goto __TRANS_DMA2FD_ERR_;
        }
    } else {
        /* vdp or vpss not product 8+2 bit data , dont convernt this */
        frame_info->buf_addr_lb[HI_DRV_3D_EYE_LEFT].start_addr = -1;
    }

#ifdef RIGHT_EYE_SUPPORT
    ret = transfer_dma2fd(&frame_info->buf_addr_lb[HI_DRV_3D_EYE_RIGHT], is_producer);
    if (ret != HI_SUCCESS) {
        goto __TRANS_DMA2FD_ERR_;
    }

    ret = transfer_dma2fd(&frame_info->buf_addr[HI_DRV_3D_EYE_RIGHT], is_producer);
    if (ret != HI_SUCCESS) {
        goto __TRANS_DMA2FD_ERR_;
    }
#endif

    return HI_SUCCESS;
__TRANS_DMA2FD_ERR_:
    dma2fd_err_deal(&frame_info->buf_addr[HI_DRV_3D_EYE_LEFT], is_producer);
    dma2fd_err_deal(&frame_info->buf_addr_lb[HI_DRV_3D_EYE_LEFT], is_producer);
#ifdef RIGHT_EYE_SUPPORT
    dma2fd_err_deal(&frame_info->buf_addr_lb[HI_DRV_3D_EYE_RIGHT], is_producer);
    dma2fd_err_deal(&frame_info->buf_addr[HI_DRV_3D_EYE_RIGHT], is_producer);
#endif
    return HI_ERR_VO_NULL_PTR;
}

static hi_s32 get_frame_phy_addr(hi_drv_vid_frame_addr *frame_addr, hi_drv_vid_frame_phy_addr *metadata_info,
    mem_map_info *dma_infor)
{
    hi_s32 ret;
    hi_u64 dma_buf_handle;
    drv_xdp_mem_info fd_info;

    memset(metadata_info, 0, sizeof(hi_drv_vid_frame_phy_addr));

    dma_buf_handle = frame_addr->dma_handle;
    if (dma_buf_handle == HI_NULL) {
        WIN_WARN("dma handle null, may be 10bit.\n");
        return HI_SUCCESS;
    }

    ret = drv_xdp_mem_map_dma_handle((struct dma_buf *)dma_buf_handle, &fd_info);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("get y phy addr failed!\n");
        return ret;
    }

    metadata_info->phy_addr_y_head = fd_info.phy_addr + frame_addr->y_head_offset;
    metadata_info->phy_addr_y = fd_info.phy_addr + frame_addr->y_offset;
    metadata_info->phy_addr_c_head = fd_info.phy_addr + frame_addr->c_head_offset;
    metadata_info->phy_addr_c = fd_info.phy_addr + frame_addr->c_offset;

    memset(dma_infor, 0, sizeof(mem_map_info));
    dma_infor->dma_buf = dma_buf_handle;
    dma_infor->phy_addr = fd_info.phy_addr;
    dma_infor->vir_addr = fd_info.vir_addr;

    return HI_SUCCESS;
}

static hi_s32 err_unmap_addr(hi_drv_vid_frame_addr *frame_addr, hi_drv_vid_frame_phy_addr *metadata_info,
    mem_map_info *dma_infor)
{
    hi_s32 ret;
    hi_u64 dma_buf_handle;
    drv_xdp_mem_info fd_info;

    dma_buf_handle = dma_infor->dma_buf;
    if (dma_buf_handle == HI_NULL) {
        WIN_WARN("dma handle null, may be 10bit.\n");
        return HI_SUCCESS;
    }

    fd_info.phy_addr = dma_infor->phy_addr;
    fd_info.vir_addr = dma_infor->vir_addr;

    ret = drv_xdp_mem_unmap_dma_handle((struct dma_buf *)dma_buf_handle, &fd_info);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("get y phy addr failed!\n");
        return ret;
    }

    memset(dma_infor, 0, sizeof(mem_map_info));
    memset(metadata_info, 0, sizeof(hi_drv_vid_frame_phy_addr));
    return HI_SUCCESS;
}


static hi_s32 transfer_dma_to_phy(hi_drv_video_frame *frame_info)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_win_vpss_meta_info *metadata_info;

    COMMON_DEBUG(VPSS_INFOR, " fmt:%d, type:%d, cmp_mode:%d, bit:%d \n", frame_info->pixel_format,
        frame_info->cmp_info.data_fmt, frame_info->cmp_info.cmp_mode, frame_info->bit_width);

    metadata_info = (hi_drv_win_vpss_meta_info *)frame_info->video_private.vpss_private.data;
    COMMON_DEBUG(VPSS_INFOR, "y:0x%llx\n", frame_info->buf_addr[0].dma_handle);

    ret = get_frame_phy_addr(&frame_info->buf_addr[HI_DRV_3D_EYE_LEFT], &metadata_info->buf_addr[HI_DRV_3D_EYE_LEFT],
        &metadata_info->buf_mapinfo[HI_DRV_3D_EYE_LEFT]);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("cannot get phy_addr\n");
        return ret;
    }

    ret = get_frame_phy_addr(&frame_info->buf_addr_lb[HI_DRV_3D_EYE_LEFT],
        &metadata_info->buf_addr_lb[HI_DRV_3D_EYE_LEFT], &metadata_info->buf_mapinfo_lb[HI_DRV_3D_EYE_LEFT]);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("cannot get phy_addr\n");
        goto get_phy_addr_failed;
    }

    return ret;
get_phy_addr_failed:
    ret = err_unmap_addr(&frame_info->buf_addr[HI_DRV_3D_EYE_LEFT], &metadata_info->buf_addr[HI_DRV_3D_EYE_LEFT],
        &metadata_info->buf_mapinfo[HI_DRV_3D_EYE_LEFT]);
    return ret;
}

hi_s32 drv_win_vpss_queue_frame(hi_handle win_handle, hi_drv_video_frame *frame_info)
{
    hi_s32 ret;
    win_descriptor *win_descp = HI_NULL;
    hi_drv_win_vpss_meta_info *metadata;

    GET_WIN_PTR(win_handle, win_descp);

    ret = win_check_frame(frame_info);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("check frame frame.\n");
        return ret;
    }

    /* dma buf to phy addr */
    ret = transfer_dma_to_phy(frame_info);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("cannot get phy_addr form dma buf.\n");
        return ret;
    }

    ret = win_buffer_queue_frame(win_descp->win_component.h_buf, frame_info);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss queue frame to vdp's internal buf failed.\n");
        return ret;
    }

    metadata = (hi_drv_win_vpss_meta_info *)frame_info->video_private.vpss_private.data;
    win_descp->avsync_info.newest_frame_play_index = metadata->play_index + metadata->frc_play_cnt;
    COMMON_DEBUG(VPSS_INFOR, "o q | index:%d,cnt:%d \n", frame_info->frame_index, metadata->play_cnt);

    return ret;
}

hi_void win_unmap_dma_handle(hi_drv_win_vpss_meta_info *metadata_info)
{
    drv_xdp_mem_info fd_info;

    if (metadata_info->buf_mapinfo[HI_DRV_3D_EYE_LEFT].dma_buf != 0) {
        fd_info.phy_addr = metadata_info->buf_mapinfo[HI_DRV_3D_EYE_LEFT].phy_addr;
        fd_info.vir_addr = metadata_info->buf_mapinfo[HI_DRV_3D_EYE_LEFT].vir_addr;
        drv_xdp_mem_unmap_dma_handle((struct dma_buf *)(metadata_info->buf_mapinfo[HI_DRV_3D_EYE_LEFT].dma_buf),
            &fd_info);
    }

    if (metadata_info->buf_mapinfo_lb[HI_DRV_3D_EYE_LEFT].dma_buf != 0) {
        fd_info.phy_addr = metadata_info->buf_mapinfo_lb[HI_DRV_3D_EYE_LEFT].phy_addr;
        fd_info.vir_addr = metadata_info->buf_mapinfo_lb[HI_DRV_3D_EYE_LEFT].vir_addr;
        drv_xdp_mem_unmap_dma_handle((struct dma_buf *)(metadata_info->buf_mapinfo_lb[HI_DRV_3D_EYE_LEFT].dma_buf),
            &fd_info);
    }

    return;
}

hi_s32 drv_win_vpss_dequeue_frame(hi_handle win_handle, hi_drv_video_frame *frame_info)
{
    hi_s32 ret;
    win_descriptor *win_descp = HI_NULL;
    hi_drv_win_vpss_meta_info *metadata;

    GET_WIN_PTR(win_handle, win_descp);

    COMMON_DEBUG(VPSS_INFOR, "o dq try | index:%d \n", frame_info->frame_index);

    ret = win_buffer_dequeue_frame(win_descp->win_component.h_buf, frame_info);
    if (ret != HI_SUCCESS) {
        WIN_INFO("vpss dequeue from vdp's internal buf failed.\n");
        return ret;
    }

    metadata = (hi_drv_win_vpss_meta_info *)frame_info->video_private.vpss_private.data;
    win_unmap_dma_handle(metadata);

    COMMON_DEBUG(VPSS_INFOR, "o dq success | index:%d,cnt:%d \n", frame_info->frame_index, metadata->play_cnt);
    return ret;
}

hi_s32 drv_win_get_play_info(hi_handle win_handle, hi_drv_win_play_info *play_info)
{
    win_descriptor *win_descp = HI_NULL;
    hi_disp_display_info disp_info = { 0 };
    hi_s32 all_left_play_cnt = 0;
    hi_drv_disp_timing_status current_timing_status;
    hi_u32 frame_play_cnt;

    GET_WIN_PTR(win_handle, win_descp);

    if (win_descp->win_type == HI_DRV_VITUAL_WIN) {
        play_info->remain_frame_num = 0;
        play_info->delay_time = 0;
        play_info->underload_times = 0;
        play_info->out_put_frame_rate = 60000; /* 60000 is a default framerate */
        WIN_WARN("vir type window no support get play info.\n");
        return HI_SUCCESS;
    }

    (hi_void)hi_drv_disp_get_display_info(win_descp->win_user_attr.disp_chn, &disp_info);
    (hi_void)hi_drv_disp_get_current_timing_status(win_descp->win_user_attr.disp_chn, &current_timing_status);
    /* add for sink disp index big than all cnt */
    if (win_descp->avsync_info.all_need_play_cnt < win_descp->avsync_info.display_frame_play_index) {
        WIN_WARN("play %d less than disp %d\n", win_descp->avsync_info.all_need_play_cnt,
            win_descp->avsync_info.display_frame_play_index);
        win_descp->avsync_info.all_need_play_cnt = win_descp->avsync_info.display_frame_play_index;
    }

    if (win_descp->avsync_info.need_sync_adjust >= 0) {
        all_left_play_cnt = win_descp->avsync_info.all_need_play_cnt - win_descp->avsync_info.display_frame_play_index +
            win_descp->avsync_info.need_sync_adjust;
    } else {
        if ((abs(win_descp->avsync_info.need_sync_adjust) + win_descp->avsync_info.display_frame_play_index) <
            win_descp->avsync_info.all_need_play_cnt) {
            all_left_play_cnt = win_descp->avsync_info.all_need_play_cnt -
                win_descp->avsync_info.display_frame_play_index + win_descp->avsync_info.need_sync_adjust;
        } else {
            all_left_play_cnt = 0;
        }
    }

    if (((win_descp->win_misc_status.is_update_frame_in_cycle == HI_FALSE) || (current_timing_status.left_time >
        (current_timing_status.circle_time_us / (2 * ISR_SYSTEM_TIME_US)))) && /* 2 means half the period */
        (all_left_play_cnt > 1)) {
        all_left_play_cnt--;
    }

    win_buffer_get_full_frame_info(win_descp->win_component.h_buf, &play_info->remain_frame_num, &frame_play_cnt);
    play_info->delay_time = (all_left_play_cnt * current_timing_status.circle_time_us) / ISR_SYSTEM_TIME_US +
        current_timing_status.left_time;

    COMMON_DEBUG(AVSYNC_INFOR, "need sync cnt:%d!\n", win_descp->avsync_info.need_sync_adjust);
    COMMON_DEBUG(AVSYNC_INFOR, "dly, jif:%ld, index :%d/%d(total/current), left cnt:%d,  left:%d,delay:%d.\n", jiffies,
        win_descp->avsync_info.all_need_play_cnt, win_descp->avsync_info.display_frame_play_index, all_left_play_cnt,
        current_timing_status.left_time, play_info->delay_time);

    play_info->underload_times = win_buffer_get_underload_cnt(win_descp->win_component.h_buf);
    play_info->disp_frame_pts = win_descp->avsync_info.display_frame_pts;
    play_info->disp_frame_disp_time = win_descp->avsync_info.display_frame_disp_time;

    play_info->out_put_frame_rate = disp_info.refresh_rate * 10; /* 10 is a ratio */
    return HI_SUCCESS;
}

hi_s32 drv_win_set_sync(hi_handle win_handle, hi_drv_win_sync sync_mode)
{
    win_descriptor *win_descp = HI_NULL;
    hi_s32 ret = HI_SUCCESS;

    GET_WIN_PTR(win_handle, win_descp);

    if ((sync_mode < HI_DRV_WIN_SYNC_PLAY) || (sync_mode > HI_DRV_WIN_SYNC_PLAY_LAST)) {
        WIN_ERROR("set window sync is invalid mode (%d)\n", sync_mode);
        return HI_ERR_VO_INVALID_PARA;
    }

    win_descp->avsync_info.sync_mode = sync_mode;
    switch (sync_mode) {
        case HI_DRV_WIN_SYNC_DISCARD:
            win_descp->avsync_info.need_sync_adjust--;
            win_descp->avsync_info.total_sync_adjust_cnts--;
            break;
        case HI_DRV_WIN_SYNC_REPEAT:
            win_descp->avsync_info.need_sync_adjust++;
            win_descp->avsync_info.total_sync_adjust_cnts++;
            break;
        case HI_DRV_WIN_SYNC_PLAY:
        case HI_DRV_WIN_SYNC_PLAY_LASTEST:
        case HI_DRV_WIN_SYNC_PLAY_LAST:
            /* do nothing */
            break;
        default:
            ret = HI_ERR_VO_INVALID_PARA;
            break;
    }

    return ret;
}

#define TPLAY_SPEED_INTEGER_MAX 64
#define TPLAY_SPEED_DECIMAL_MAX 999

hi_s32 drv_win_set_playctl_info(hi_handle win_handle, hi_drv_win_play_ctrl *playctrl_info)
{
    win_descriptor *win_descp = HI_NULL;
    hi_s32 ret = HI_SUCCESS;

    GET_WIN_PTR(win_handle, win_descp);

    if ((playctrl_info->speed_integer > TPLAY_SPEED_INTEGER_MAX) ||
        (playctrl_info->speed_decimal > TPLAY_SPEED_DECIMAL_MAX) ||
        ((playctrl_info->speed_integer == 0) && (playctrl_info->speed_decimal == 0))) {
        WIN_ERROR("speed cfg out of value: %d/%d\n", playctrl_info->speed_integer, playctrl_info->speed_decimal);
        return HI_ERR_VO_INVALID_PARA;
    }

    win_descp->win_play_ctrl.tplay_speed_integer = playctrl_info->speed_integer;
    win_descp->win_play_ctrl.tplay_speed_decimal = playctrl_info->speed_decimal;

    win_dfx_record_tplayinfo(win_descp, playctrl_info->speed_integer, playctrl_info->speed_decimal);
    return ret;
}

hi_s32 drv_win_get_playctl_info(hi_handle win_handle, hi_drv_win_play_ctrl *playctrl_info)
{
    win_descriptor *win_descp = HI_NULL;
    hi_s32 ret = HI_SUCCESS;

    GET_WIN_PTR(win_handle, win_descp);
    playctrl_info->speed_decimal = win_descp->win_play_ctrl.tplay_speed_decimal;
    playctrl_info->speed_integer = win_descp->win_play_ctrl.tplay_speed_integer;

    return ret;
}

static hi_void win_insert_frame_to_list(win_descriptor *win_descp, hi_drv_video_frame *frame_info)
{
    win_frame_node *frame_node;
    struct osal_list_head *list_node = HI_NULL;
    hi_ulong lock_flags = 0;

    osal_spin_lock_irqsave(&win_descp->win_frame_info.win_frame_lock, &lock_flags);

    common_list_get_del_node(&win_descp->win_frame_info.frame_empty_list, &list_node);
    if (list_node != HI_NULL) {
        COMMON_GET_NODE_CONTENT(list_node, win_frame_node, frame_node);
        memcpy_s(&frame_node->frame_info, sizeof(hi_drv_video_frame), frame_info, sizeof(hi_drv_video_frame));

        common_list_add_node(&win_descp->win_frame_info.frame_full_list, list_node);
    }

    osal_spin_unlock_irqrestore(&win_descp->win_frame_info.win_frame_lock, &lock_flags);
    if (list_node == HI_NULL) {
        WIN_ERROR("not enough node to fill.\n");
    }

    return;
}

static hi_s32 win_delete_frame_from_list(win_descriptor *win_descp, hi_drv_video_frame *frame_info)
{
    win_frame_node *frame_node;
    struct osal_list_head *list_node = HI_NULL;
    hi_ulong lock_flags = 0;

    osal_spin_lock_irqsave(&win_descp->win_frame_info.win_frame_lock, &lock_flags);

    common_list_get_del_node(&win_descp->win_frame_info.frame_full_list, &list_node);

    if (list_node != HI_NULL) {
        COMMON_GET_NODE_CONTENT(list_node, win_frame_node, frame_node);
        memcpy_s(frame_info, sizeof(hi_drv_video_frame), &frame_node->frame_info, sizeof(hi_drv_video_frame));
        common_list_add_node(&win_descp->win_frame_info.frame_empty_list, list_node);
    }

    osal_spin_unlock_irqrestore(&win_descp->win_frame_info.win_frame_lock, &lock_flags);

    if (list_node == HI_NULL) {
        return HI_ERR_VO_BUFQUE_EMPTY;
    }

    return HI_SUCCESS;
}

/* drain buffer when vpss exist and fence opened. */
static hi_void drain_vpss_frame(win_descriptor *win_descp)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_video_frame *frame_info = (hi_drv_video_frame *)osal_kmalloc(HI_ID_WIN,
        sizeof(hi_drv_video_frame), OSAL_GFP_KERNEL);
    if (frame_info == HI_NULL) {
        WIN_ERROR("win malloc mem falid.\n");
        return;
    }

    if ((win_descp->win_play_status.is_tunnel_fence == HI_TRUE) ||
        (win_descp->win_play_status.is_android_deque_fence == HI_TRUE)) {
        while (ret == HI_SUCCESS) {
            ret = win_vpss_dequeue_frame(win_descp->win_component.h_vpss, frame_info);
            COMMON_DEBUG(VPSS_INFOR, "i dq | index %d \n", frame_info->frame_index);
        }
    }

    osal_kfree(HI_ID_WIN, frame_info);
    return;
}

static hi_s32 win_param_align_down(hi_s32 num, hi_u32 aligned_precision)
{
    if (aligned_precision == 0) {
        return num;
    } else {
        return ((num / aligned_precision) * aligned_precision);
    }
}

static hi_void win_rect_align(hi_rect *rect_align, vdp_rect *hal_align)
{
    rect_align->x = win_param_align_down(rect_align->x, hal_align->x);
    rect_align->y = win_param_align_down(rect_align->y, hal_align->y);
    rect_align->width = win_param_align_down(rect_align->width, hal_align->w);
    rect_align->height = win_param_align_down(rect_align->height, hal_align->h);

    return;
}

hi_s32 drv_win_aquire_frame(hi_handle win_handle, hi_drv_video_frame *frame_info)
{
    hi_s32 ret;
    win_descriptor *win_descp = HI_NULL;
    hi_drv_video_frame *frame = HI_NULL;

    win_descp = winmanage_get_window(win_handle);
    if (win_descp == HI_NULL) {
        WIN_WARN("WIN is not exist! %#x\n", win_handle);
        return HI_ERR_VO_WIN_NOT_EXIST;
    }

    if (win_descp->win_type != HI_DRV_VITUAL_WIN) {
        WIN_ERROR("type %d no support!%#x\n", win_descp->win_type, win_handle);
        return HI_ERR_VO_INVALID_OPT;
    }

    ret = win_buffer_acquire_frame(win_descp->win_component.h_buf, &frame);
    if (ret != HI_SUCCESS) {
        return HI_ERR_VO_BUFQUE_EMPTY;
    }
    *frame_info = *frame;
    return HI_SUCCESS;
}

hi_s32 drv_win_release_frame(hi_handle win_handle, hi_drv_video_frame *frame_info)
{
    hi_s32 ret;
    win_descriptor *win_descp = HI_NULL;

    win_descp = winmanage_get_window(win_handle);
    if (win_descp == HI_NULL) {
        WIN_WARN("WIN is not exist! %#x\n", win_handle);
        return HI_ERR_VO_WIN_NOT_EXIST;
    }

    if (win_descp->win_type != HI_DRV_VITUAL_WIN) {
        WIN_ERROR("type %d no support!%#x\n", win_descp->win_type, win_handle);
        return HI_ERR_VO_INVALID_OPT;
    }

    ret = win_buffer_release_frame(win_descp->win_component.h_buf, frame_info);
    return ret;
}

static hi_s32 process_frame_frc(win_descriptor *win_descp, hi_drv_video_frame *frame_info, hi_u32 *frc_cnt)
{
    win_frc_input_attr in_attr = { 0 };
    win_frc_output_result result = { 0 };
    hi_s32 ret;
    hi_disp_display_info disp_info = { 0 };

    if (win_descp->win_type == HI_DRV_VITUAL_WIN) {
        result.need_play_cnt = 1; /* virwin disprate not need frc */
    } else {
        (hi_void)hi_drv_disp_get_display_info(win_descp->win_user_attr.disp_chn, &disp_info);
        in_attr.in_rate = frame_info->frame_rate;
        in_attr.out_rate = disp_info.refresh_rate * 10; /* 10 is a ratio */
        in_attr.speed_integer = win_descp->win_play_ctrl.tplay_speed_integer;
        ;
        in_attr.speed_decimal = win_descp->win_play_ctrl.tplay_speed_decimal;
        ret = win_frc_calcute(win_descp->win_component.h_frc, &in_attr, &result);
        if (ret != HI_SUCCESS) {
            WIN_ERROR("frc failed\n");
            return ret;
        }
    }

    *frc_cnt = result.need_play_cnt;

    return HI_SUCCESS;
}

#define WIN_DROP_FRAMERATE_MAX 60000
static hi_void queue_frame_policy(win_descriptor *win_descp, hi_drv_video_frame *frame_info, hi_u32 frc_cnt,
                                  win_vpss_policy_info *win_vpss_policy)
{
    hi_s32 win_num;

    win_num = winmanage_get_win_num(HI_DRV_DISPLAY_0);
    if ((win_descp->expect_win_layermapping.layer_id == LAYER_ID_VID0) && (win_num > 1) && /* 1:window num */
        ((frame_info->width >= VDP_FRAME_WIDTH_4K) || (frame_info->height >= VDP_FRAME_HEIGHT_4K))) {
        win_vpss_policy->force_vpss_bypass = HI_TRUE;
    } else {
        win_vpss_policy->force_vpss_bypass = HI_FALSE;
    }

    if ((win_descp->expect_win_layermapping.layer_id != LAYER_ID_VID0) && (win_num > 0x2) && /* 2:winnum threshhold */
        ((frame_info->width >= FRAME_DEFAULT_WIDTH) || (frame_info->height >= FRAME_DEFAULT_HEIGHT))) {
        win_vpss_policy->force_pq_close = HI_TRUE;
    } else {
        win_vpss_policy->force_pq_close = HI_FALSE;
    }

    /* choose rls at once in queue func or drop it after vpss's process,
     * if frame rate > 60fps, and size larger than 4k, just rls at once. */
    if ((frc_cnt == 0) && ((frame_info->width >= VDP_FRAME_WIDTH_4K) || (frame_info->height >= VDP_FRAME_HEIGHT_4K)) &&
        (frame_info->frame_rate >= WIN_DROP_FRAMERATE_MAX)) { /* frame rate max of rls. */
        win_vpss_policy->rls_frame_atonce = HI_TRUE;
    } else {
        win_vpss_policy->rls_frame_atonce = HI_FALSE;
    }

    return;
}

hi_s32 drv_win_queue_frame(hi_handle win_handle,
                           hi_drv_video_frame *frame_info,
                           hi_s32 *fence_fd)
{
    hi_s32 ret;
    win_descriptor *win_descp = HI_NULL;
    hi_drv_win_vpss_meta_info win_metadata_info = { 0 };
    hi_u64 metadata_addr = 0;
    hi_u64 fence_descp = 0;
    hi_u32 frc_cnt = 0;
    hi_u32 delay_time = 0;
    win_vpss_policy_info win_vpss_policy = { 0 };

    GET_WIN_PTR(win_handle, win_descp);
    ret = win_check_frame(frame_info);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("check frame frame.\n");
        return  HI_ERR_VO_INVALID_PARA;
    }

    COMMON_DEBUG(VPSS_INFOR, "i q try | index %d \n", frame_info->frame_index);
    drain_vpss_frame(win_descp);

    ret = process_frame_frc(win_descp, frame_info, &frc_cnt);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("create rls fence failed.\n");
        return ret;
    }

    /* vpss should do the same thing. */
    queue_frame_policy(win_descp, frame_info, frc_cnt, &win_vpss_policy);

     /* here we will create two fls fence, one for vdec , and another for android. */
    ret = win_create_frame_rls_fence(win_descp, fence_fd, win_vpss_policy.rls_frame_atonce,
                                     frame_info, &fence_descp);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("create rls fence failed.\n");
        return ret;
    }

    ret = win_increase_ref_cnt(frame_info);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("increase ref cnt failed\n");
        goto __QUEUE_ERR;
    }

    win_dfx_record_frame_time(frame_info, RECORD_QUEUE_FRAME_TIME);

    if (win_vpss_policy.rls_frame_atonce == HI_FALSE) {
        hi_drv_win_play_info play_info;
        if (win_descp->win_type == HI_DRV_VITUAL_WIN) {
            delay_time = 0; /* virwin disprate not need delaytime */
        } else {
            (hi_void)drv_win_get_play_info(win_descp->win_index, &play_info);
            delay_time = play_info.delay_time;
        }

        win_metadata_info.force_bypass         = win_vpss_policy.force_vpss_bypass;
        win_metadata_info.force_pq_close       = win_vpss_policy.force_pq_close;
        win_metadata_info.android_fence_fd     = fence_descp;
        win_metadata_info.frc_play_cnt         = frc_cnt;
        win_metadata_info.play_cnt             = frc_cnt;
        win_metadata_info.played_cnt           = 0;
        win_metadata_info.sync_adjust_cnts     = win_descp->avsync_info.total_sync_adjust_cnts;

        win_metadata_info.play_index           = win_descp->avsync_info.all_need_play_cnt;
        win_metadata_info.debug_delay_time     = delay_time;
        win_metadata_info.debug_queue_time     = hi_drv_sys_get_time_stamp_ms();
        win_metadata_info.debug_cfg_time       = 0;

        ret = win_buffer_acquire_metadata(win_descp->win_component.h_buf,
                                          &win_metadata_info, &metadata_addr);
        if (ret != HI_SUCCESS) {
            WIN_ERROR("metadata is empty\n");
            ret = HI_ERR_VO_NO_SOURCE;
            goto __BUF_REF_CNT_RLS;
        }

        /* give value to frame. */
        frame_info->video_private.win_private.data = metadata_addr;
        frame_info->video_private.win_private.size = sizeof(win_metadata_info);
        ret = win_vpss_queue_frame(win_descp->win_component.h_vpss,
                                   frame_info);
        if (ret != HI_SUCCESS) {
            WIN_ERROR("queue to vpss failed\n");
            goto __QUEUE_VPSS_ERR;
        }
    } else {
        release_src_frame(win_descp,frame_info);
    }

    win_descp->win_play_status.is_android_deque_fence = (fence_fd == HI_NULL) ? HI_FALSE : HI_TRUE;
    /* if no fence, should not insert frame to list. */
    win_descp->win_play_status.is_tunnel_fence = (frame_info->source_fence < 0) ? HI_FALSE : HI_TRUE;
    if (win_descp->win_play_status.is_tunnel_fence == HI_TRUE) {
        win_insert_frame_to_list(win_descp, frame_info);
    }

    win_descp->avsync_info.all_need_play_cnt += frc_cnt;
    win_dfx_inc_acq_frame_cnt(win_descp);
    win_dfx_record_queue_interval(win_descp);

    COMMON_DEBUG(VPSS_INFOR, "i q success |index %d cnt %d\r\n",frame_info->frame_index, frc_cnt);
    return ret;

__QUEUE_VPSS_ERR:
    win_buffer_release_metadata(win_descp->win_component.h_buf, metadata_addr);
__BUF_REF_CNT_RLS:
    win_decrease_ref_cnt(frame_info);
__QUEUE_ERR:
    /* signal and rls  frame. */
    win_destroy_fence(frame_info, fence_fd);
    return ret;
}


hi_s32 drv_win_dequeue_frame(hi_handle win_handle, hi_drv_video_frame *frame_info)
{
    hi_s32 ret = HI_FAILURE;
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);

    if (win_descp->win_play_status.is_tunnel_fence == HI_TRUE) {
        ret = win_delete_frame_from_list(win_descp, frame_info);
    } else {
        ret = win_vpss_dequeue_frame(win_descp->win_component.h_vpss, frame_info);
        if (ret != HI_SUCCESS) {
            WIN_WARN("dequeue null frame.\n");
            return ret;
        }
    }

    return ret;
}

hi_s32 drv_win_vpss_rls_src_frame(hi_handle win_handle, hi_drv_video_frame *frame_info)
{
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);
    win_vpss_rls_src_frame(win_descp, frame_info);

    return HI_SUCCESS;
}

hi_s32 win_get_vpssconfig(win_descriptor *win_descp)
{
    /* 1 get wether ai vpss ,whether undergo vpss process;
     * 2 */

    return HI_SUCCESS;
}

static hi_bool win_check_can_destrory(win_descriptor *win_descp)
{
    return HI_TRUE;
}

static hi_s32 win_rls_resources(win_descriptor *win_descp)
{
    return HI_SUCCESS;
}

hi_s32 drv_win_set_source(hi_handle win_handle, hi_handle source_handle)
{
    win_descriptor *win_descp = HI_NULL;
    GET_WIN_PTR(win_handle, win_descp);

    if ((win_descp->src_handle == HI_INVALID_HANDLE) && (source_handle != HI_INVALID_HANDLE)) {
        win_vpss_update_src(win_handle, source_handle);
        /* normal  initial attach. */
    } else if ((win_descp->src_handle != HI_INVALID_HANDLE) && (source_handle != HI_INVALID_HANDLE)) {
        /* do not support dynamic  switch from one src to another src. */
        WIN_ERROR("Invalid opt \n");
        return HI_ERR_VO_INVALID_OPT;
    } else if ((win_descp->src_handle != HI_INVALID_HANDLE) && (source_handle == HI_INVALID_HANDLE)) {
        WIN_WARN("since setsource is a weak function, do nothing when dettach.\n");
    }

    return HI_SUCCESS;
}

hi_void win_release_disp_frame_in_status(win_descriptor *win_descp)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_video_frame *disp_rls_frame = win_descp->win_play_status.displaying_frame;
    hi_drv_video_frame *cfg_rls_frame = win_descp->win_play_status.config_frame;
    if (disp_rls_frame != HI_NULL) {
        win_descp->win_play_status.displaying_frame = HI_NULL;
        ret = win_buffer_release_frame(win_descp->win_component.h_buf, disp_rls_frame);
        if (ret != HI_SUCCESS) {
            WIN_FATAL("rls frame failed\n");
        }
    }

    if (cfg_rls_frame != HI_NULL) {
        win_descp->win_play_status.config_frame = HI_NULL;
        if (cfg_rls_frame != disp_rls_frame) {
            ret = win_buffer_release_frame(win_descp->win_component.h_buf, cfg_rls_frame);
            if (ret != HI_SUCCESS) {
                WIN_FATAL("rls frame failed\n");
            }
        }
    }
    return;
}

#define VDP_RESET_DELAY_TIME_MS 50
hi_s32 win_reset_buf(win_descriptor *win_descp, hi_drv_win_reset_mode reset_mode)
{
    hi_s32 ret;

    /* if win enable need stop vpss first */
    if (win_descp->win_play_ctrl.win_enable == HI_TRUE) {
        win_vpss_send_command(win_descp->win_component.h_vpss, WIN_VPSS_STOP, HI_NULL);
    }

    /* release win full list buffer */
    ret = win_buffer_reset(win_descp->win_component.h_buf, reset_mode);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("reset failed \n");
        return ret;
    }

    /* if win disable, release configframe and dispframe (isr not change win frame status ) */
    if (win_descp->win_play_ctrl.win_enable == HI_FALSE) {
        win_release_disp_frame_in_status(win_descp);
    }

    osal_msleep_uninterruptible(VDP_RESET_DELAY_TIME_MS); /* make sure the cfg frame in isr return */
    win_vpss_send_command(win_descp->win_component.h_vpss, WIN_VPSS_RESET, HI_NULL);
    win_descp->avsync_info.all_need_play_cnt = win_descp->avsync_info.newest_frame_play_index;

    if (win_descp->win_play_ctrl.win_enable == HI_TRUE) {
        win_vpss_send_command(win_descp->win_component.h_vpss, WIN_VPSS_START, HI_NULL);
    }
    return HI_SUCCESS;
}

hi_s32 drv_win_reset(hi_handle win_handle, hi_drv_win_reset_mode reset_mode)
{
    win_descriptor *win_descp = HI_NULL;
    hi_s32 ret;

    GET_WIN_PTR(win_handle, win_descp);
    ret = win_reset_buf(win_descp, reset_mode);
    return ret;
}


hi_s32 drv_win_freeze(hi_handle win_handle, hi_drv_win_freeze_mode freeze_mode)
{
    hi_s32 ret;
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);

    if (win_descp->win_type != HI_DRV_VITUAL_WIN) {
        ret = win_buffer_freeze(win_descp->win_component.h_buf, freeze_mode);
        if (ret == HI_SUCCESS) {
            win_descp->win_play_ctrl.freeze_ctrl = freeze_mode;
        }
    } else {
        WIN_ERROR("cannot pause virtual window\n");
        return HI_ERR_VO_WIN_UNSUPPORT;
    }
    return ret;
}

hi_s32 drv_win_get_freeze_status(hi_handle win_handle, hi_drv_win_freeze_mode *freeze_mode)
{
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);

    if (win_descp->win_type != HI_DRV_VITUAL_WIN) {
        *freeze_mode = win_descp->win_play_ctrl.freeze_ctrl;
    } else {
        WIN_ERROR("cannot pause virtual window\n");
        return HI_ERR_VO_WIN_UNSUPPORT;
    }

    return HI_SUCCESS;
}

hi_s32 drv_win_set_quickout(hi_handle win_handle, hi_bool quickout_enable)
{
    hi_s32 ret;

    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);
    if (win_descp->win_type != HI_DRV_VITUAL_WIN) {
        ret = win_buffer_quickout(win_descp->win_component.h_buf, quickout_enable);
        if (ret == HI_SUCCESS) {
            win_descp->win_play_ctrl.quickout_enalbe = quickout_enable;
        }
    } else {
        WIN_ERROR("cannot quickout virtual window\n");
        return HI_ERR_VO_WIN_UNSUPPORT;
    }
    return ret;
}

hi_s32 drv_win_get_quickout_status(hi_handle win_handle, hi_bool *quickout_enable)
{
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);
    if (win_descp->win_type != HI_DRV_VITUAL_WIN) {
        *quickout_enable = win_descp->win_play_ctrl.quickout_enalbe;
    } else {
        WIN_ERROR("cannot quickout virtual window\n");
        return HI_ERR_VO_WIN_UNSUPPORT;
    }

    return HI_SUCCESS;
}

static hi_s32 win_transfer_captured_frame(win_descriptor *win_descp, hi_drv_video_frame *frame)
{
    hi_s32 ret = HI_SUCCESS;

    COMMON_DEBUG(VPSS_INFOR, "frame_before:bit_width:%d, cmp_mode:%d, pixel_format:%d", frame->bit_width,
        frame->cmp_info.cmp_mode, frame->pixel_format);

    win_descp->win_component.h_vpss_capture = win_vpss_create(win_descp->win_index, HI_INVALID_HANDLE, HI_TRUE);
    if (win_descp->win_component.h_vpss_capture == HI_INVALID_HANDLE) {
        WIN_ERROR("cannot create win_vpss");
        return HI_FAILURE;
    }
    ret = win_vpss_transfer_frame(win_descp->win_component.h_vpss_capture, frame);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("win_vpss_transfer_frame failed\n");
        win_vpss_destroy(win_descp->win_component.h_vpss_capture);
    }
    COMMON_DEBUG(VPSS_INFOR, "frame_after:bit_width:%d, cmp_mode:%d, pixel_format:%d", frame->bit_width,
                 frame->cmp_info.cmp_mode, frame->pixel_format);

    return ret;
}

static hi_s32 win_get_transferd_frame(win_descriptor *win_descp, hi_drv_video_frame *captured_frame)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_win_vpss_meta_info *win_metadata_info;

    if ((captured_frame->bit_width != HI_DRV_PIXEL_BITWIDTH_8BIT) &&
        (captured_frame->bit_width != HI_DRV_PIXEL_BITWIDTH_10BIT)) {
        WIN_ERROR("Unsupported bitwidth %d.\n", captured_frame->bit_width);
        return HI_FAILURE;
    }

    win_metadata_info = (hi_drv_win_vpss_meta_info *)captured_frame->video_private.win_private.data;
    win_metadata_info->force_bypass = HI_FALSE;

    /* start to change */
    ret = win_transfer_captured_frame(win_descp, captured_frame);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("win_transfer_captured_frame failed.\n");
        win_descp->win_component.h_vpss_capture = HI_INVALID_HANDLE;
    }

    return ret;
}

hi_s32 drv_win_acquire_captured_frame(hi_handle win_handle, hi_drv_video_frame *frame_info)
{
    hi_s32 ret;
    win_descriptor *win_descp = HI_NULL;
    hi_drv_video_frame *captured_frame = HI_NULL;
    hi_drv_video_frame *win_buf_frame = HI_NULL;
    hi_u64 meta_addr = 0;
    hi_drv_win_vpss_meta_info *win_metadata = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);
    if (win_descp->win_type == HI_DRV_VITUAL_WIN) {
        WIN_ERROR("cannot capture frame in virtual window\n");
        return HI_ERR_VO_WIN_UNSUPPORT;
    }

    if (win_descp->win_play_status.is_captured == HI_TRUE) {
        WIN_ERROR("do not support continous capturing.\n");
        return HI_ERR_VO_WIN_UNSUPPORT;
    }

    if (win_descp->win_play_ctrl.freeze_ctrl == HI_DRV_WIN_FREEZE_BLACK) {
        WIN_ERROR("cannot capture frame when screen is black\n");
        return HI_ERR_VO_WIN_UNSUPPORT;
    }
    captured_frame = osal_kmalloc(HI_ID_WIN, sizeof(hi_drv_video_frame), OSAL_GFP_KERNEL);
    if (captured_frame == HI_NULL) {
        return HI_FAILURE;
    }

    ret = win_buffer_acquire_captured_frame(win_descp->win_component.h_buf, &win_buf_frame);
    if (ret != HI_SUCCESS) {
        goto CAPTURED_FRAME_FREE;
    }

    memcpy(captured_frame, win_buf_frame, sizeof(hi_drv_video_frame));
    win_metadata = (hi_drv_win_vpss_meta_info *)captured_frame->video_private.win_private.data;

    ret = win_buffer_acquire_metadata(win_descp->win_component.h_buf, win_metadata, &meta_addr);
    if (ret != HI_SUCCESS) {
        goto CAPTURED_FRAME_RLS;
    }
    ret = win_get_transferd_frame(win_descp, captured_frame);
    if (ret != HI_SUCCESS) {
        win_buffer_release_metadata(win_descp->win_component.h_buf, meta_addr);
        goto CAPTURED_FRAME_RLS;
    }
    memcpy(frame_info, captured_frame, sizeof(hi_drv_video_frame));
    win_descp->win_play_status.is_captured = HI_TRUE;

    /* use new win metadata buffer and old vpss metadata buffer for capture */
    win_descp->win_play_status.captured_win_meta_info = meta_addr;
    win_descp->win_play_status.captured_vpss_meta_info = captured_frame->video_private.vpss_private.data;

CAPTURED_FRAME_RLS:
    win_buffer_release_captured_frame(win_descp->win_component.h_buf, win_buf_frame);
CAPTURED_FRAME_FREE:
    osal_kfree(HI_ID_WIN, captured_frame);
    return ret;
}

hi_s32 drv_win_release_captured_frame(hi_handle win_handle, hi_drv_video_frame *frame_info)
{
    hi_s32 ret = HI_FAILURE;
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);

    if (win_descp->win_type == HI_DRV_VITUAL_WIN) {
        WIN_ERROR("cannot capture frame in virtual window\n");
        return HI_ERR_VO_WIN_UNSUPPORT;
    }

    if ((win_descp->win_component.h_vpss_capture != HI_INVALID_HANDLE) &&
        (win_descp->win_play_status.is_captured == HI_TRUE)) {
        frame_info->video_private.win_private.data = win_descp->win_play_status.captured_win_meta_info;
        frame_info->video_private.vpss_private.data = win_descp->win_play_status.captured_vpss_meta_info;

        ret = win_vpss_release_transferd_frame(win_descp->win_component.h_vpss_capture, frame_info);
        if (ret == HI_SUCCESS) {
            win_buffer_release_metadata(win_descp->win_component.h_buf,
                                        win_descp->win_play_status.captured_win_meta_info);
            win_descp->win_play_status.is_captured = HI_FALSE;
            win_descp->win_component.h_vpss_capture = HI_INVALID_HANDLE;
            win_descp->win_play_status.captured_win_meta_info = 0;
            win_descp->win_play_status.captured_vpss_meta_info = 0;
        } else {
            WIN_ERROR("capture release failed, index:%d", frame_info->frame_index);
        }
    }

    return ret;
}

hi_s32 drv_win_pause(hi_handle win_handle, hi_bool pause_enable)
{
    hi_s32 ret;
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);
    if (win_descp->win_type != HI_DRV_VITUAL_WIN) {
        if (pause_enable == win_descp->win_play_ctrl.pause_enalbe) {
            return HI_SUCCESS;
        }
        ret = win_buffer_pause(win_descp->win_component.h_buf, pause_enable);
        if (ret == HI_SUCCESS) {
            win_descp->win_play_ctrl.pause_enalbe = pause_enable;
        }
    } else {
        WIN_ERROR("cannot pause virtual window\n");
        return HI_ERR_VO_WIN_UNSUPPORT;
    }
    return ret;
}

hi_s32 drv_win_get_pause_status(hi_handle win_handle, hi_bool *pause_enable)
{
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);
    if (win_descp->win_type != HI_DRV_VITUAL_WIN) {
        *pause_enable = win_descp->win_play_ctrl.pause_enalbe;
    } else {
        WIN_ERROR("cannot pause virtual window\n");
        return HI_ERR_VO_WIN_UNSUPPORT;
    }
    return HI_SUCCESS;
}

hi_void drv_win_map_layer(hi_drv_display disp, hi_handle win_handle, win_layermapping_result *result_layermapping)
{
    hi_u32 index;
    if (disp == HI_DRV_DISPLAY_1) {
        result_layermapping->layer_id = LAYER_ID_VID3;
        result_layermapping->layer_region_id = 0;
        return;
    }

    index = (HI_HANDLE_GET_CHAN_ID(win_handle) & 0x00ff);
    if (index == 0) {
        result_layermapping->layer_id = LAYER_ID_VID0;
        result_layermapping->layer_region_id = 0;
    } else {
        result_layermapping->layer_id = LAYER_ID_VID1;
        result_layermapping->layer_region_id = index - 1;
    }
    return;
}

hi_s32 drv_win_set_enable(hi_handle win_handle, hi_bool is_enable)
{
    win_descriptor *win_descp = HI_NULL;
    hi_drv_disp_timing_status current_timing_status;
    hi_u32 delay_time_ms;
    hi_s32 ret;
    hi_ulong lock_flags = 0;
    hi_bool enable_bak;
    win_layermapping_result result_bak;
    win_runtime_attr_s exp_attr_bak;

    GET_WIN_PTR(win_handle, win_descp);

    if (win_descp->win_play_ctrl.win_enable == is_enable) {
        return HI_SUCCESS;
    }

    if (is_enable == HI_FALSE) {
        win_vpss_send_command(win_descp->win_component.h_vpss, WIN_VPSS_STOP, HI_NULL);
        win_descp->win_play_ctrl.win_enable = is_enable;
        win_descp->expected_win_attr.win_enable = is_enable;
        (hi_void)hi_drv_disp_get_current_timing_status(win_descp->win_user_attr.disp_chn, &current_timing_status);
        delay_time_ms = current_timing_status.circle_time_us / ISR_SYSTEM_TIME_US + current_timing_status.left_time;
        osal_msleep_uninterruptible(delay_time_ms);
        if (win_descp->win_type != HI_DRV_VITUAL_WIN) {
            hal_layer_cfg_disable(win_descp->expect_win_layermapping.layer_id,
                                  win_descp->expect_win_layermapping.layer_region_id);
        }
        win_descp->expect_win_layermapping.layer_id = LAYER_ID_BUTT;
    }

    winmanage_get_lock(&lock_flags);

    /* save win info */
    enable_bak = win_descp->win_play_ctrl.win_enable;
    result_bak = win_descp->expect_win_layermapping;
    exp_attr_bak = win_descp->expected_win_attr;

    win_descp->win_play_ctrl.win_enable = is_enable;

    ret = window_redistribute_proccess(win_descp);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("mapping failed");
        /* restore win info after mapping failed */
        win_descp->win_play_ctrl.win_enable = enable_bak;
        win_descp->expect_win_layermapping = result_bak;
        win_descp->expected_win_attr = exp_attr_bak;
        winmanage_rls_lock(&lock_flags);
        return ret;
    }

    winmanage_rls_lock(&lock_flags);
    if (is_enable == HI_TRUE) {
        win_vpss_send_command(win_descp->win_component.h_vpss, WIN_VPSS_START, HI_NULL);
    }


    return HI_SUCCESS;
}

hi_s32 drv_win_get_enable(hi_handle win_handle, hi_bool *is_enable)
{
    win_descriptor *win_descp = HI_NULL;
    GET_WIN_PTR(win_handle, win_descp);

    *is_enable = win_descp->win_play_ctrl.win_enable;

    return HI_SUCCESS;
}

hi_s32 drv_win_set_flip(hi_handle win_handle, hi_bool flip_hori, hi_bool flip_vert)
{
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);

    win_descp->win_user_attr.horz_flip_enable = flip_hori;
    win_descp->win_user_attr.vert_flip_enable = flip_vert;

    return HI_SUCCESS;
}

hi_s32 drv_win_get_flip(hi_handle win_handle, hi_bool *flip_hori, hi_bool *flip_vert)
{
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);

    *flip_hori = win_descp->win_user_attr.horz_flip_enable;
    *flip_vert = win_descp->win_user_attr.vert_flip_enable;

    return HI_SUCCESS;
}

static hi_s32 win_zorder_move_top(win_descriptor *win_descp)
{
    hi_u32 i = 0;
    hi_s32 ret;
    hi_ulong lock_flags = 0;
    hi_u32 zorder_bak[WINDOW_MAX_NUMBER] = {0};
    hi_handle *win_array;
    hi_u32 win_num;
    win_descriptor *tmp_win = HI_NULL;

    winmanage_get_lock(&lock_flags);
    win_array = winmanage_get_win_handle(HI_DRV_DISPLAY_0);
    win_num = winmanage_get_win_num(HI_DRV_DISPLAY_0);
    win_zorder_save(zorder_bak, win_array);

    /* for existing window's move top */
    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        if (win_array[i] == 0) {
            continue;
        }
        tmp_win = winmanage_get_window(win_array[i]);

        if ((win_descp != tmp_win) &&
            (win_descp->expected_win_attr.win_zorder < tmp_win->expected_win_attr.win_zorder)) {
            tmp_win->expected_win_attr.win_zorder--;
        }
    }

    /* add lock  */
    win_descp->expected_win_attr.win_zorder = win_num - 1;
    ret = window_redistribute_proccess(win_descp);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("move top failed\n");
        win_zorder_restore(zorder_bak, win_array);
    }
    winmanage_rls_lock(&lock_flags);

    return ret;
}

static hi_s32 win_zorder_move_up(win_descriptor *win_descp)
{
    hi_u32 i = 0;
    hi_s32 ret;
    hi_u32 zorder_bak[WINDOW_MAX_NUMBER] = {0};
    hi_ulong lock_flags = 0;
    hi_handle *win_array;

    win_descriptor *tmp_win = HI_NULL;

    winmanage_get_lock(&lock_flags);
    win_array = winmanage_get_win_handle(HI_DRV_DISPLAY_0);
    win_zorder_save(zorder_bak, win_array);

    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        if (win_array[i] == 0) {
            continue;
        }
        tmp_win = winmanage_get_window(win_array[i]);
        if (tmp_win->expected_win_attr.win_zorder == (win_descp->expected_win_attr.win_zorder + 1)) {
            win_descp->expected_win_attr.win_zorder++;
            tmp_win->expected_win_attr.win_zorder--;
            break;
        }
    }

    ret = window_redistribute_proccess(win_descp);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("move up failed");
        win_zorder_restore(zorder_bak, win_array);
    }

    winmanage_rls_lock(&lock_flags);
    return ret;
}

static hi_s32 win_zorder_move_bottom(win_descriptor *win_descp)
{
    hi_u32 i = 0;
    hi_s32 ret;
    hi_ulong lock_flags = 0;
    hi_u32 zorder_bak[WINDOW_MAX_NUMBER] = {0};
    hi_handle *win_array;
    win_descriptor *tmp_win = HI_NULL;

    if (win_descp->expected_win_attr.win_zorder == 0) {
        return HI_SUCCESS;
    }

    winmanage_get_lock(&lock_flags);
    win_array = winmanage_get_win_handle(HI_DRV_DISPLAY_0);
    win_zorder_save(zorder_bak, win_array);

    /* for existing window's move top  */
    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        if (win_array[i] == 0) {
            continue;
        }
        tmp_win = winmanage_get_window(win_array[i]);

        if ((win_descp != tmp_win) &&
            (win_descp->expected_win_attr.win_zorder > tmp_win->expected_win_attr.win_zorder)) {
            tmp_win->expected_win_attr.win_zorder++;
        }
    }


    /* add lock  */

    win_descp->expected_win_attr.win_zorder = 0;
    ret = window_redistribute_proccess(win_descp);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("move bottom failed\n");
        win_zorder_restore(zorder_bak, win_array);
    }

    winmanage_rls_lock(&lock_flags);

    return ret;
}

static hi_s32 win_zorder_move_down(win_descriptor *win_descp)
{
    hi_u32 i = 0;
    hi_s32 ret = HI_SUCCESS;
    hi_u32 zorder_bak[WINDOW_MAX_NUMBER] = {0};
    hi_ulong lock_flags = 0;
    hi_handle *win_array;
    win_descriptor *tmp_win = HI_NULL;

    if (win_descp->expected_win_attr.win_zorder == 0) {
        return HI_SUCCESS;
    }

    winmanage_get_lock(&lock_flags);
    win_array = winmanage_get_win_handle(HI_DRV_DISPLAY_0);
    win_zorder_save(zorder_bak, win_array);

    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        if (win_array[i] == 0) {
            continue;
        }
        tmp_win = winmanage_get_window(win_array[i]);
        if (tmp_win->expected_win_attr.win_zorder == (win_descp->expected_win_attr.win_zorder - 1)) {
            win_descp->expected_win_attr.win_zorder--;
            tmp_win->expected_win_attr.win_zorder++;

            break;
        }
    }

    ret = window_redistribute_proccess(win_descp);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("move down failed");
        win_zorder_restore(zorder_bak, win_array);
    }
    winmanage_rls_lock(&lock_flags);

    return ret;
}

hi_s32 drv_win_set_zorder(hi_handle win_handle, hi_drv_disp_zorder zflag)
{
    win_descriptor *win_descp = HI_NULL;
    hi_s32 ret = HI_FAILURE;

    GET_WIN_PTR(win_handle, win_descp);

    if (win_descp->win_user_attr.disp_chn == HI_DRV_DISPLAY_0) {
        switch (zflag) {
            case HI_DRV_DISP_ZORDER_MOVETOP:
                ret = win_zorder_move_top(win_descp);
                break;
            case HI_DRV_DISP_ZORDER_MOVEUP:
                ret = win_zorder_move_up(win_descp);
                break;
            case HI_DRV_DISP_ZORDER_MOVEBOTTOM:
                ret = win_zorder_move_bottom(win_descp);
                break;
            case HI_DRV_DISP_ZORDER_MOVEDOWN:
                ret = win_zorder_move_down(win_descp);
                break;
            default:
                ret = HI_ERR_VO_INVALID_OPT;
                break;
        }
    } else {
        WIN_ERROR("cannot set zorder in current display channel\n");
        return HI_ERR_VO_WIN_UNSUPPORT;
    }

    return ret;
}

hi_s32 drv_win_get_zorder(hi_handle win_handle, hi_u32 *zorder)
{
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);

    if (win_descp->win_user_attr.disp_chn == HI_DRV_DISPLAY_0) {
        *zorder = win_descp->expected_win_attr.win_zorder;
    } else {
        WIN_ERROR("cannot get zorder in current display channel\n");
        return HI_ERR_VO_WIN_UNSUPPORT;
    }

    return HI_SUCCESS;
}

hi_s32 drv_win_set_rotation(hi_handle win_handle, hi_drv_rot_angle rotation)
{
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);

    win_descp->win_user_attr.rotation = rotation;

    return HI_SUCCESS;
}

hi_s32 drv_win_get_rotation(hi_handle win_handle, hi_drv_rot_angle *rotation)
{
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);

    *rotation = win_descp->win_user_attr.rotation;

    return HI_SUCCESS;
}

hi_s32 drv_win_set_frame_proginterleave_mode(hi_handle win_handle, hi_drv_win_forceframeprog_flag frame_mode)
{
    hi_s32 ret;
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);

    ret = win_vpss_set_frame_proginterleave_mode(win_descp->win_component.h_vpss, frame_mode);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("set frame proginterleave mode failed, mode:%d\n", frame_mode);
    } else {
        win_descp->win_play_ctrl.frame_mode = frame_mode;
    }
    return ret;
}

hi_s32 drv_win_get_frame_proginterleave_mode(hi_handle win_handle, hi_drv_win_forceframeprog_flag *frame_mode)
{
    hi_s32 ret;

    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);
    ret = win_vpss_get_frame_proginterleave_mode(win_descp->win_component.h_vpss, frame_mode);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("get frame proginterleave mode failed\n");
    } else {
        win_descp->win_play_ctrl.frame_mode = *frame_mode;
    }
    return ret;
}

hi_s32 drv_win_get_handle(hi_drv_display disp_id, hi_drv_win_handle* win_handle)
{
    hi_s32 i;
    hi_handle *win_array = HI_NULL;
    win_descriptor *tmp_win = HI_NULL;

    /* There is only one window in display 1 */
    if (disp_id != HI_DRV_DISPLAY_0) {
        WIN_ERROR("para disp_id is not HI_DRV_DISPLAY_0,is %d\n", disp_id);
        return HI_FAILURE;
    }
    if (win_handle == HI_NULL) {
        WIN_ERROR("para win handle is null\n");
        return HI_FAILURE;
    }

    win_handle->main_win = HI_INVALID_HANDLE;
    win_handle->sub_win = HI_INVALID_HANDLE;

    win_array = winmanage_get_win_handle(disp_id);
    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        if (win_array[i] == 0) {
            continue;
        }
        tmp_win = winmanage_get_window(win_array[i]);
        if (tmp_win->expect_win_layermapping.layer_id == LAYER_ID_VID0) {
            win_handle->main_win = tmp_win->win_index;
            break;
        }
    }
    if (win_handle->main_win == HI_INVALID_HANDLE) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 win_attr_out_rect_check(hi_rect *out_rect)
{
    if (((out_rect->x == 0) && (out_rect->y == 0)) && ((out_rect->height == 0) && (out_rect->width == 0))) {
        return HI_SUCCESS;
    }
    if ((out_rect->height >= WIN_OUTRECT_MIN_HEIGHT) && (out_rect->height <= WIN_OUTRECT_MAX_HEIGHT) &&
        (out_rect->width >= WIN_OUTRECT_MIN_WIDTH) && (out_rect->width <= WIN_OUTRECT_MAX_WIDTH)) {
        return HI_SUCCESS;
    }

    return HI_ERR_VO_INVALID_PARA;
}

static hi_s32 win_attr_crop_rect_check(hi_drv_crop_rect *crop_rect)
{
    if ((crop_rect->top_offset > WIN_CROPRECT_MAX_OFFSET_TOP) ||
        (crop_rect->left_offset > WIN_CROPRECT_MAX_OFFSET_LEFT) ||
        (crop_rect->bottom_offset > WIN_CROPRECT_MAX_OFFSET_BOTTOM) ||
        (crop_rect->right_offset > WIN_CROPRECT_MAX_OFFSET_RIGHT)) {
        WIN_ERROR("win offset cannot be larger than 128!\n");
        return HI_ERR_VO_INVALID_PARA;
    }
    return HI_SUCCESS;
}

static hi_s32 win_attr_in_rect_check(hi_rect *in_rect)
{
    if ((in_rect->x == 0) && (in_rect->y == 0) && (in_rect->width == 0) && (in_rect->height == 0)) {
        return HI_SUCCESS;
    } else if ((in_rect->x < 0) || (in_rect->y < 0)) {
        WIN_ERROR("win inrect invalid location!\n");
        return HI_ERR_VO_INVALID_PARA;
    } else if ((in_rect->width < WIN_INRECT_MIN_WIDTH) || (in_rect->height < WIN_INRECT_MIN_HEIGHT)) {
        WIN_ERROR("win inrect invalid size, too small!\n");
        return HI_ERR_VO_INVALID_PARA;
    } else if ((in_rect->width > WIN_INRECT_MAX_WIDTH) || (in_rect->height > WIN_INRECT_MAX_HEIGHT)) {
        WIN_ERROR("win inrect invalid size, too large!\n");
        return HI_ERR_VO_INVALID_PARA;
    }
    return HI_SUCCESS;
}

static hi_s32 win_check_attr(hi_drv_win_attr *win_attr, hi_bool virtual_screen)
{
    hi_s32 ret = HI_SUCCESS;

    if ((win_attr->aspect_ratio_mode >= HI_DRV_ASP_RAT_MODE_MAX) || (win_attr->data_format >= HI_DRV_PIXEL_MAX)) {
        WIN_ERROR("Invalid enum.\n");
        return HI_ERR_VO_INVALID_PARA;
    }

    if ((virtual_screen != HI_TRUE) && (virtual_screen != HI_FALSE)) {
        WIN_ERROR("virtual_screen para is invalid\n");
        return ret;
    }

    if (win_attr->use_crop_rect == HI_TRUE) {
        ret = win_attr_crop_rect_check(&win_attr->crop_rect);
    } else {
        ret = win_attr_in_rect_check(&win_attr->in_rect);
    }
    if (ret != HI_SUCCESS) {
        WIN_ERROR("invalid crop_rect/in_rect error!\n");
        return ret;
    }

    ret = win_attr_out_rect_check(&win_attr->out_rect);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("invalid out_rect error!\n");
        return ret;
    }

    if ((win_attr->aspect_ratio.aspect_ratio_w > (win_attr->aspect_ratio.aspect_ratio_h * WIN_MAX_ASPECT_RATIO)) ||
        ((win_attr->aspect_ratio.aspect_ratio_w * WIN_MAX_ASPECT_RATIO) < win_attr->aspect_ratio.aspect_ratio_h)) {
        WIN_ERROR("aspect_ratio  error!\n");
        return HI_ERR_VO_INVALID_PARA;
    }
    return ret;
}

static hi_bool win_check_is_full_screen(hi_rect *out_rect,  hi_rect *virtaul_screen)
{
    if ((out_rect->x != 0) || (out_rect->y != 0)) {
        return HI_FALSE;
    }

    if ((out_rect->width == 0) && (out_rect->height == 0)) {
        return HI_TRUE;
    }

    if ((out_rect->width == virtaul_screen->width) &&
        (out_rect->height ==  virtaul_screen->height)) {
        return HI_TRUE;
    }

    return HI_FALSE;
}


static hi_s32 win_update_expected_attr(win_descriptor *win_descp,
                                       win_user_setting_attr_s *win_attr,
                                       hi_drv_video_frame *frame_info)
{
    hi_disp_display_info disp_info = {0};
    drv_win_attr_cfg attr_cfg  = {0};
    hi_s32 ret;
    drv_win_attr_out_config out_cfg = {0};
    layer_capacity *capacity = HI_NULL;

    capacity = hal_layer_get_capacity(win_descp->expect_win_layermapping.layer_id);

    if (frame_info != HI_NULL) {
        attr_cfg.src_width    = frame_info->width;
        attr_cfg.src_height   = frame_info->height;
    } else {
        attr_cfg.src_width    = FRAME_DEFAULT_WIDTH;
        attr_cfg.src_height   = FRAME_DEFAULT_HEIGHT;
    }

    memset_s(&attr_cfg.src_crop_rect,
             sizeof(attr_cfg.src_crop_rect),
             0,
             sizeof(attr_cfg.src_crop_rect));

    if (win_attr->user_crop_enable == HI_TRUE) {
        attr_cfg.src_crop_rect.x = win_attr->crop_rect.left_offset;
        attr_cfg.src_crop_rect.y = win_attr->crop_rect.top_offset;
        attr_cfg.src_crop_rect.width = attr_cfg.src_width -
                                       win_attr->crop_rect.left_offset -
                                       win_attr->crop_rect.right_offset;
        attr_cfg.src_crop_rect.height = attr_cfg.src_height -
                                        win_attr->crop_rect.top_offset -
                                        win_attr->crop_rect.bottom_offset;
    } else {
        attr_cfg.src_crop_rect =  win_attr->in_rect;
    }

    /* if user setting not equal to zero, just use user-setting. */
    if ((win_attr->user_def_aspect_ratio.aspect_ratio_w != 0) &&
        (win_attr->user_def_aspect_ratio.aspect_ratio_h != 0)) {
        attr_cfg.src_asp_ratio.aspect_ratio_w       = win_attr->user_def_aspect_ratio.aspect_ratio_w;
        attr_cfg.src_asp_ratio.aspect_ratio_h       = win_attr->user_def_aspect_ratio.aspect_ratio_h;
    } else {
        if (frame_info != HI_NULL) {
            attr_cfg.src_asp_ratio.aspect_ratio_w       = frame_info->aspect_width;
            attr_cfg.src_asp_ratio.aspect_ratio_h       = frame_info->aspect_height;
        } else {
            attr_cfg.src_asp_ratio.aspect_ratio_w       = ASP_RATIO_DEFAULT_W;
            attr_cfg.src_asp_ratio.aspect_ratio_h       = ASP_RATIO_DEFAULT_H;
        }
    }

    if (win_descp->win_type == HI_DRV_VITUAL_WIN) {
        out_cfg.src_crop_rect = attr_cfg.src_crop_rect;
        out_cfg.win_video_rect = win_attr->out_rect;
        out_cfg.win_out_rect = win_attr->out_rect;
    } else {
        (hi_void)hi_drv_disp_get_display_info(win_descp->win_user_attr.disp_chn,
                                              &disp_info);
        attr_cfg.src_afd_type                         = HI_DRV_AFD_TYPE_16_9;
        attr_cfg.src_active_format_flag               = HI_FALSE;


        attr_cfg.out_physics_screen                   = disp_info.pixel_fmt_resolution;
        attr_cfg.out_reference_screen                 = disp_info.virtaul_screen;
        attr_cfg.out_device_asp_ratio.aspect_ratio_w  = disp_info.aspect_ratio.aspect_ratio_w;
        attr_cfg.out_device_asp_ratio.aspect_ratio_h  = disp_info.aspect_ratio.aspect_ratio_h;

        if ((win_attr->out_rect.width == 0) || (win_attr->out_rect.height == 0)) {
            attr_cfg.win_out_rect = attr_cfg.out_reference_screen;
        } else {
            attr_cfg.win_out_rect       = win_attr->out_rect;
        }

        attr_cfg.is_phy_coordinate  = HI_FALSE;
        attr_cfg.win_out_asp_mode   = win_attr->aspect_ratio_mode;
        ret = drv_win_attrprocess_update(win_descp->win_component.h_attr,
                                         &attr_cfg,
                                         &out_cfg);
        if (ret != HI_SUCCESS) {
            WIN_ERROR("win_attrprocess_update failed\n");
            return ret;
        }
    }

    win_rect_align(&out_cfg.src_crop_rect, &capacity->input_align);
    win_rect_align(&out_cfg.win_video_rect, &capacity->output_align);
    win_rect_align(&out_cfg.win_out_rect, &capacity->output_align);

    win_descp->expected_win_attr.is_full_screen =
        win_check_is_full_screen(&win_attr->out_rect, &disp_info.virtaul_screen);

    win_descp->expected_win_attr.crop_rect   = out_cfg.src_crop_rect;
    win_descp->expected_win_attr.video_rect  = out_cfg.win_video_rect;
    win_descp->expected_win_attr.disp_rect   = out_cfg.win_out_rect;

    win_descp->expected_win_attr.win_enable  = win_descp->win_play_ctrl.win_enable;
    win_descp->expected_win_attr.alpha_value = win_descp->win_user_attr.alpha_value;

    return HI_SUCCESS;
}



hi_s32 drv_win_update_expected_attr(hi_handle win_handle, hi_drv_video_frame *frame_info)
{
    win_descriptor *win_descp = HI_NULL;
    GET_WIN_PTR(win_handle, win_descp);

    win_update_expected_attr(win_descp, &win_descp->win_user_attr, frame_info);

    return HI_SUCCESS;
}

hi_s32 drv_win_get_expected_info(hi_handle win_handle, win_runtime_attr_s *expected_win_attr,
    win_layermapping_result *expect_win_layermapping)
{
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);

    *expected_win_attr = win_descp->expected_win_attr;
    *expect_win_layermapping = win_descp->expect_win_layermapping;

    return HI_SUCCESS;
}

hi_void win_transfer_rect(hi_drv_rect *dst, hi_rect *src)
{
    dst->rect_x = src->x;
    dst->rect_y = src->y;
    dst->rect_w = src->width;
    dst->rect_h = src->height;
}

#define VPSS_ALIGNMENT 2
static hi_void win_vpss_offset_info_align(hi_drv_crop_rect *in_offset_rect)
{
    in_offset_rect->left_offset = win_param_align_down(in_offset_rect->left_offset, VPSS_ALIGNMENT);
    in_offset_rect->top_offset = win_param_align_down(in_offset_rect->top_offset, VPSS_ALIGNMENT);
    in_offset_rect->right_offset = win_param_align_down(in_offset_rect->right_offset, VPSS_ALIGNMENT);
    in_offset_rect->bottom_offset = win_param_align_down(in_offset_rect->bottom_offset, VPSS_ALIGNMENT);

    return;
}

hi_void win_type_rotation_to_vpss(hi_drv_rot_angle rotation,  hi_drv_vpss_rotation *vpss_rotation)
{
    switch (rotation) {
        case HI_DRV_ROT_ANGLE_0:
            *vpss_rotation = HI_DRV_VPSS_ROTATION_DISABLE;
            break;
        case HI_DRV_ROT_ANGLE_90:
            *vpss_rotation = HI_DRV_VPSS_ROTATION_90;
            break;
        case HI_DRV_ROT_ANGLE_180:
            *vpss_rotation = HI_DRV_VPSS_ROTATION_180;
            break;
        case HI_DRV_ROT_ANGLE_270:
            *vpss_rotation = HI_DRV_VPSS_ROTATION_270;
            break;
        default:
            *vpss_rotation = HI_DRV_VPSS_ROTATION_MAX;
            break;
    }
    return ;
}

hi_void win_get_vpssprocess_basic_cfg(hi_drv_vpss_port_frame_cfg *out_cfg, win_user_setting_attr_s *win_attr,
    hi_drv_video_frame *frame_info, drv_win_attr_out_config *attr_out_cfg)
{
    hi_drv_crop_rect in_offset_rect = { 0 };
    out_cfg->user_crop_en = HI_FALSE;
    out_cfg->in_offset_rect = in_offset_rect;
    win_vpss_offset_info_align(&out_cfg->in_offset_rect);
    win_transfer_rect(&out_cfg->crop_rect, &attr_out_cfg->src_crop_rect);
    win_transfer_rect(&out_cfg->video_rect, &attr_out_cfg->win_video_rect);
    win_transfer_rect(&out_cfg->out_rect, &attr_out_cfg->win_out_rect);
    out_cfg->format = win_attr->data_format;
    out_cfg->cmp_info.cmp_mode = HI_DRV_COMPRESS_MODE_OFF;
    out_cfg->cmp_info.ycmp_rate = 0;
    out_cfg->cmp_info.ccmp_rate = 0;
    out_cfg->cmp_info.data_fmt = HI_DRV_DATA_FMT_LINER;
    out_cfg->bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
    win_type_rotation_to_vpss(win_attr->rotation, &out_cfg->rotation);
    out_cfg->vertical_flip = win_attr->vert_flip_enable;
    out_cfg->horizontal_flip = win_attr->horz_flip_enable;
    out_cfg->hdr_follow_src = HI_TRUE;
    out_cfg->out_hdr_type = frame_info->hdr_type;
    out_cfg->out_color_desp = frame_info->color_space;
    return;
}
static hi_void out_hdr_type_set(hi_drv_disp_out_type disp_type, hi_drv_hdr_type *out_hdr_type)
{
    if (disp_type == HI_DRV_DISP_TYPE_NORMAL) {
        *out_hdr_type = HI_DRV_HDR_TYPE_SDR;
    } else if (disp_type == HI_DRV_DISP_TYPE_HDR10) {
        *out_hdr_type = HI_DRV_HDR_TYPE_HDR10;
    } else if (disp_type == HI_DRV_DISP_TYPE_HLG) {
        *out_hdr_type = HI_DRV_HDR_TYPE_HLG;
    } else if (disp_type == HI_DRV_DISP_TYPE_HDR10PLUS) {
        *out_hdr_type = HI_DRV_HDR_TYPE_HDR10PLUS;
    } else if ((disp_type == HI_DRV_DISP_TYPE_DOLBY) ||
               (disp_type == HI_DRV_DISP_TYPE_DOLBY_LL)) {
        *out_hdr_type = HI_DRV_HDR_TYPE_DOLBYVISION;
    } else {
        *out_hdr_type = HI_DRV_HDR_TYPE_SDR;
        WIN_ERROR("VPSS cannot handle this disp type:%d, now it sets to SDR\n", disp_type);
    }
    return;
}

hi_void win_get_vpssprocess_more_cfg(hi_drv_vpss_port_frame_cfg *out_cfg, win_user_setting_attr_s *win_attr,
                                     hi_drv_video_frame *frame_info, drv_win_attr_out_config *attr_out_cfg,
                                     hi_drv_display disp_chn)
{
    hi_disp_display_info disp_info = { 0 };

    out_cfg->user_crop_en = win_attr->user_crop_enable;
    out_cfg->in_offset_rect = win_attr->crop_rect;
    win_vpss_offset_info_align(&out_cfg->in_offset_rect);
    win_transfer_rect(&out_cfg->crop_rect, &attr_out_cfg->src_crop_rect);
    win_transfer_rect(&out_cfg->video_rect, &attr_out_cfg->win_video_rect);
    win_transfer_rect(&out_cfg->out_rect, &attr_out_cfg->win_out_rect);
    frame_info->disp_rect.rect_x = out_cfg->out_rect.rect_x;
    frame_info->disp_rect.rect_y = out_cfg->out_rect.rect_y;
    out_cfg->video_rect.rect_x -= out_cfg->out_rect.rect_x;
    out_cfg->video_rect.rect_y -= out_cfg->out_rect.rect_y;
    out_cfg->out_rect.rect_x = 0;
    out_cfg->out_rect.rect_y = 0;
    out_cfg->format = win_attr->data_format;
    out_cfg->cmp_info.cmp_mode = HI_DRV_COMPRESS_MODE_OFF;
    out_cfg->cmp_info.ycmp_rate = 0;
    out_cfg->cmp_info.ccmp_rate = 0;
    out_cfg->cmp_info.data_fmt = HI_DRV_DATA_FMT_LINER;
    out_cfg->bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
    win_type_rotation_to_vpss(win_attr->rotation, &out_cfg->rotation);
    out_cfg->vertical_flip = win_attr->vert_flip_enable;
    out_cfg->horizontal_flip = win_attr->horz_flip_enable;

    (hi_void)hi_drv_disp_get_display_info(disp_chn, &disp_info);
    out_cfg->hdr_follow_src = HI_FALSE;
    out_hdr_type_set(disp_info.disp_type, &out_cfg->out_hdr_type);
    out_cfg->out_color_desp = disp_info.color_space;
    /* disp1 not supprot hdr, and v3 in disp1 vpss only out 709 or 601 yuv */
    if (disp_chn == HI_DRV_DISPLAY_1) {
        out_cfg->out_hdr_type = HI_DRV_HDR_TYPE_SDR;
        if ((out_cfg->out_color_desp.color_space != HI_DRV_COLOR_CS_YUV) ||
            ((out_cfg->out_color_desp.color_primary != HI_DRV_COLOR_PRIMARY_BT601_525) &&
            (out_cfg->out_color_desp.color_primary != HI_DRV_COLOR_PRIMARY_BT601_625) &&
            (out_cfg->out_color_desp.color_primary != HI_DRV_COLOR_PRIMARY_BT709))) {
            out_cfg->out_color_desp.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
            out_cfg->out_color_desp.color_space = HI_DRV_COLOR_CS_YUV;
            out_cfg->out_color_desp.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
            out_cfg->out_color_desp.transfer_type = HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR;
            out_cfg->out_color_desp.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
        }
    }

    return;
}

hi_void win_attr_update_crop_rect(win_user_setting_attr_s *win_attr, hi_u32 src_width, hi_u32 src_height,
    hi_rect *src_crop_rect)
{
    if (win_attr->user_crop_enable == HI_TRUE) {
        src_crop_rect->x = win_attr->crop_rect.left_offset;
        src_crop_rect->y = win_attr->crop_rect.top_offset;
        src_crop_rect->width = src_width - win_attr->crop_rect.left_offset - win_attr->crop_rect.right_offset;
        src_crop_rect->height = src_height - win_attr->crop_rect.top_offset - win_attr->crop_rect.bottom_offset;
    } else {
        *src_crop_rect = win_attr->in_rect;
    }
    return;
}

hi_void update_win_attr_info(win_descriptor *win_descp, win_user_setting_attr_s *win_attr,
    drv_win_attr_cfg *attr_in_cfg, hi_drv_video_frame *frame_info)
{
    hi_disp_display_info disp_info = { 0 };

    (hi_void)hi_drv_disp_get_display_info(win_descp->win_user_attr.disp_chn, &disp_info);

    if (frame_info != HI_NULL) {
        attr_in_cfg->src_width = frame_info->width;
        attr_in_cfg->src_height = frame_info->height;
    } else {
        attr_in_cfg->src_width = FRAME_DEFAULT_WIDTH;
        attr_in_cfg->src_height = FRAME_DEFAULT_HEIGHT;
    }
    win_attr_update_crop_rect(win_attr, attr_in_cfg->src_width, attr_in_cfg->src_width, &attr_in_cfg->src_crop_rect);

    if ((win_attr->user_def_aspect_ratio.aspect_ratio_w != 0) &&
        (win_attr->user_def_aspect_ratio.aspect_ratio_h != 0)) {
        attr_in_cfg->src_asp_ratio.aspect_ratio_w = win_attr->user_def_aspect_ratio.aspect_ratio_w;
        attr_in_cfg->src_asp_ratio.aspect_ratio_h = win_attr->user_def_aspect_ratio.aspect_ratio_h;
    } else {
        if (frame_info != HI_NULL) {
            attr_in_cfg->src_asp_ratio.aspect_ratio_w = frame_info->aspect_width;
            attr_in_cfg->src_asp_ratio.aspect_ratio_h = frame_info->aspect_height;
        } else {
            attr_in_cfg->src_asp_ratio.aspect_ratio_w = ASP_RATIO_DEFAULT_W;
            attr_in_cfg->src_asp_ratio.aspect_ratio_h = ASP_RATIO_DEFAULT_H;
        }
    }

    attr_in_cfg->src_afd_type = HI_DRV_AFD_TYPE_16_9;
    attr_in_cfg->src_active_format_flag = HI_FALSE;
    attr_in_cfg->out_physics_screen = disp_info.pixel_fmt_resolution;
    attr_in_cfg->out_reference_screen = disp_info.virtaul_screen;
    attr_in_cfg->out_device_asp_ratio.aspect_ratio_w = disp_info.aspect_ratio.aspect_ratio_w;
    attr_in_cfg->out_device_asp_ratio.aspect_ratio_h = disp_info.aspect_ratio.aspect_ratio_h;
    if ((win_attr->out_rect.width == 0) || (win_attr->out_rect.height == 0)) {
        attr_in_cfg->win_out_rect = attr_in_cfg->out_reference_screen;
    } else {
        attr_in_cfg->win_out_rect = win_attr->out_rect;
    }
    attr_in_cfg->is_phy_coordinate = HI_FALSE;
    attr_in_cfg->win_out_asp_mode = win_attr->aspect_ratio_mode;
    return;
}

hi_void win_get_vir_win_out_cfg(win_user_setting_attr_s *win_attr, hi_drv_video_frame *frame_info,
    hi_drv_vpss_port_frame_cfg *out_cfg)
{
    out_cfg->user_crop_en = win_attr->user_crop_enable;
    out_cfg->in_offset_rect = win_attr->crop_rect;

    win_vpss_offset_info_align(&out_cfg->in_offset_rect);
    win_transfer_rect(&out_cfg->crop_rect, &win_attr->in_rect);
    win_transfer_rect(&out_cfg->video_rect, &win_attr->out_rect);
    win_transfer_rect(&out_cfg->out_rect, &win_attr->out_rect);

    out_cfg->format = win_attr->data_format;
    out_cfg->cmp_info.cmp_mode = HI_DRV_COMPRESS_MODE_OFF;
    out_cfg->cmp_info.ycmp_rate = 0;
    out_cfg->cmp_info.ccmp_rate = 0;
    out_cfg->cmp_info.data_fmt = HI_DRV_DATA_FMT_LINER;
    out_cfg->bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
    win_type_rotation_to_vpss(win_attr->rotation, &out_cfg->rotation);
    out_cfg->vertical_flip = win_attr->vert_flip_enable;
    out_cfg->horizontal_flip = win_attr->horz_flip_enable;
    out_cfg->hdr_follow_src = HI_FALSE;
    out_cfg->out_hdr_type = HI_DRV_HDR_TYPE_SDR;

    out_cfg->out_color_desp.color_space = HI_DRV_COLOR_CS_YUV;
    out_cfg->out_color_desp.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
    out_cfg->out_color_desp.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
    out_cfg->out_color_desp.transfer_type = HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR;
    out_cfg->out_color_desp.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
    return;
}

hi_s32 drv_win_vpss_get_out_config(hi_handle win_handle, hi_drv_video_frame *frame_info,
    hi_drv_vpss_port_frame_cfg *out_cfg)
{
    win_descriptor *win_descp = HI_NULL;
    win_user_setting_attr_s *win_attr;
    layer_capacity *capacity = HI_NULL;
    hi_s32 ret;
    drv_win_attr_cfg attr_in_cfg = { 0 };
    drv_win_attr_out_config attr_out_cfg = { 0 };

    GET_WIN_PTR(win_handle, win_descp);
    win_attr = &win_descp->win_user_attr;
    /* get vir window cfg for vpss */
    if (win_descp->win_type == HI_DRV_VITUAL_WIN) {
        win_get_vir_win_out_cfg(win_attr, frame_info, out_cfg);
        return HI_SUCCESS;
    }
    capacity = hal_layer_get_capacity(win_descp->expect_win_layermapping.layer_id);

    if (capacity->support_zme == HI_TRUE) {
        win_rect_align(&attr_out_cfg.src_crop_rect, &capacity->input_align);
        win_rect_align(&attr_out_cfg.win_video_rect, &capacity->output_align);
        win_rect_align(&attr_out_cfg.win_out_rect, &capacity->output_align);
        win_get_vpssprocess_basic_cfg(out_cfg, win_attr, frame_info, &attr_out_cfg);
        return HI_SUCCESS;
    }
    /* get v1 cfg for vpss */
    update_win_attr_info(win_descp, win_attr, &attr_in_cfg, frame_info);
    ret = drv_win_attrprocess_update(win_descp->win_component.h_attr, &attr_in_cfg, &attr_out_cfg);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("win_attrprocess_update failed\n");
        return ret;
    }

    win_rect_align(&attr_out_cfg.src_crop_rect, &capacity->input_align);
    win_rect_align(&attr_out_cfg.win_video_rect, &capacity->output_align);
    win_rect_align(&attr_out_cfg.win_out_rect, &capacity->output_align);
    win_get_vpssprocess_more_cfg(out_cfg, win_attr, frame_info, &attr_out_cfg,
                                 win_descp->win_user_attr.disp_chn);

    return HI_SUCCESS;
}

hi_s32 drv_win_set_attr(hi_handle win_handle, hi_drv_win_attr *win_attr)
{
    win_descriptor *win_descp = HI_NULL;
    hi_s32 ret;
    hi_ulong lock_flags = 0;
    win_user_setting_attr_s exp_user_attr_bak;

    GET_WIN_PTR(win_handle, win_descp);

    ret = win_check_attr(win_attr, HI_TRUE);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("win attr is invalid\n");
        return ret;
    }

    winmanage_get_lock(&lock_flags);
    /* save win info */
    exp_user_attr_bak = win_descp->win_user_attr;

    win_descp->win_user_attr.user_def_aspect_ratio = win_attr->aspect_ratio;
    win_descp->win_user_attr.aspect_ratio_mode = win_attr->aspect_ratio_mode;
    win_descp->win_user_attr.user_crop_enable = win_attr->use_crop_rect;
    win_descp->win_user_attr.crop_rect = win_attr->crop_rect;
    win_descp->win_user_attr.in_rect = win_attr->in_rect;
    win_descp->win_user_attr.out_rect = win_attr->out_rect;

    /* virtual window , data_format and disp_chn cannot be set */
    if (win_descp->win_type == HI_DRV_VITUAL_WIN) {
        win_descp->win_user_attr.user_alloc_buffer = win_attr->user_alloc_buffer;
        win_descp->win_user_attr.buf_number = win_attr->buf_number;
    }
    win_descp->win_user_attr.hisi_hdr = win_attr->hisi_hdr;

    ret = window_redistribute_proccess(win_descp);
    if (ret != HI_SUCCESS) {
        win_descp->win_user_attr = exp_user_attr_bak;
        winmanage_rls_lock(&lock_flags);
        return ret;
    }

    winmanage_rls_lock(&lock_flags);

    return HI_SUCCESS;
}

hi_s32 drv_win_get_attr(hi_handle win_handle, hi_drv_win_attr *win_attr)
{
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);

    win_attr->is_virtual = (win_descp->win_type == HI_DRV_VITUAL_WIN) ? HI_TRUE : HI_FALSE;
    win_attr->aspect_ratio = win_descp->win_user_attr.user_def_aspect_ratio;
    win_attr->aspect_ratio_mode = win_descp->win_user_attr.aspect_ratio_mode;
    win_attr->use_crop_rect = win_descp->win_user_attr.user_crop_enable;
    win_attr->crop_rect = win_descp->win_user_attr.crop_rect;
    win_attr->in_rect = win_descp->win_user_attr.in_rect;
    win_attr->out_rect = win_descp->win_user_attr.out_rect;
    win_attr->disp = win_descp->win_user_attr.disp_chn;
    win_attr->user_alloc_buffer = win_descp->win_user_attr.user_alloc_buffer;
    win_attr->buf_number = win_descp->win_user_attr.buf_number;
    win_attr->data_format = win_descp->win_user_attr.data_format;
    win_attr->hisi_hdr = win_descp->win_user_attr.hisi_hdr;

    return HI_SUCCESS;
}

static hi_void _deinit_win_frame_list(common_list *frame_list)
{
    win_frame_node *frame_node;
    struct osal_list_head *list_node;

    while (1) {
        common_list_get_del_node(frame_list, &list_node);
        if (list_node == HI_NULL) {
            break;
        }

        COMMON_GET_NODE_CONTENT(list_node, win_frame_node, frame_node);
        osal_kfree(HI_ID_WIN, frame_node);
    }

    return;
}

static hi_s32 win_init_frame_list(win_frame_relative_info *win_frame_info)
{
    hi_u32 i;
    hi_s32 ret;
    win_frame_node *frame_node = HI_NULL;

    ret = osal_spin_lock_init(&(win_frame_info->win_frame_lock));
    if (ret != HI_SUCCESS) {
        WIN_ERROR("frame lock initiation failed\n");
        return HI_FAILURE;
    }
    common_list_init(&(win_frame_info->frame_empty_list));
    common_list_init(&(win_frame_info->frame_full_list));

    for (i = 0; i < WIN_BUFFER_NODE_NUM; i++) {
        frame_node = osal_kmalloc(HI_ID_WIN, sizeof(win_frame_node), OSAL_GFP_KERNEL);
        if (frame_node == HI_NULL) {
            goto __malloc_error;
        }

        common_list_add_node(&(win_frame_info->frame_empty_list), &frame_node->list_node);
    }

    return HI_SUCCESS;
__malloc_error:
    WIN_ERROR("frame list create failed\n");
    _deinit_win_frame_list(&(win_frame_info->frame_empty_list));
    return HI_ERR_VO_MALLOC_FAILED;
}

static hi_void win_deinit_frame_list(win_frame_relative_info *win_frame_info)
{
    hi_ulong lock_flags = 0;

    osal_spin_lock_irqsave(&win_frame_info->win_frame_lock, &lock_flags);
    _deinit_win_frame_list(&(win_frame_info->frame_empty_list));
    _deinit_win_frame_list(&(win_frame_info->frame_full_list));
    osal_spin_unlock_irqrestore(&win_frame_info->win_frame_lock, &lock_flags);

    osal_spin_lock_destory(&(win_frame_info->win_frame_lock));
    return;
}


hi_s32 drv_win_create_window(hi_drv_win_attr *win_attr, hi_handle *win_handle)
{
    win_descriptor *win_descp = HI_NULL;
    hi_s32 ret;
    hi_handle win_index;
    hi_u32 win_num;

    ret = win_check_attr(win_attr, HI_TRUE);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("win attr is invalid\n");
        return ret;
    }

    win_descp = (win_descriptor *)DISP_MALLOC(sizeof(win_descriptor));
    if (win_descp == HI_NULL) {
        WIN_ERROR("win malloc failed.\n");
        return HI_ERR_VO_MALLOC_FAILED;
    }

    DISP_MEMSET(win_descp, 0, sizeof(win_descriptor));

    win_attr->disp = (win_attr->is_virtual == HI_TRUE) ? HI_DRV_DISPLAY_2 : win_attr->disp;

    ret = winmanage_allocate_handle(win_attr->disp, &win_index);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("allocate win handle failed\n");
        DISP_FREE(win_descp);
        return ret;
    }

    ret = osal_spin_lock_init(&(win_descp->spinlock));
    if (ret != HI_SUCCESS) {
        WIN_ERROR("window spin lock initiation failed\n");
        DISP_FREE(win_descp);
        return ret;
    }

    ret = win_init_frame_list(&win_descp->win_frame_info);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("allocate win handle failed\n");
        DISP_FREE(win_descp);
        return ret;
    }
    win_descp->src_handle = HI_INVALID_HANDLE;
    win_descp->win_user_attr.disp_chn = win_attr->disp;
    win_descp->win_user_attr.alpha_value = WINDOW_MAX_ALPHA;
    win_descp->win_user_attr.virt_screen_enable = HI_TRUE;
    win_descp->win_user_attr.data_format = HI_DRV_PIXEL_FMT_NV21;
    win_descp->win_user_attr.user_def_aspect_ratio = win_attr->aspect_ratio;
    win_descp->win_user_attr.aspect_ratio_mode = win_attr->aspect_ratio_mode;
    win_descp->win_user_attr.user_crop_enable = win_attr->use_crop_rect;
    win_descp->win_user_attr.crop_rect = win_attr->crop_rect;
    win_descp->win_user_attr.in_rect = win_attr->in_rect;
    win_descp->win_user_attr.out_rect = win_attr->out_rect;

    win_descp->win_play_ctrl.tplay_speed_integer = 1;
    win_descp->win_play_ctrl.tplay_speed_decimal = 0;

    win_descp->win_play_status.tplay_speed_integer = 1;
    win_descp->win_play_status.tplay_speed_decimal = 0;

    win_descp->win_type = (win_attr->is_virtual == HI_TRUE) ? HI_DRV_VITUAL_WIN : HI_DRV_DISP_WIN;

    win_descp->win_component.h_buf =
        win_buffer_create(WIN_BUFFER_NODE_NUM, WIN_BUFFER_MODE_ONLINE, win_descp->win_type);
    win_descp->win_component.h_frc = win_frc_create();

    win_descp->win_component.h_fence = drv_fence_create_instance();
    win_descp->win_component.h_attr = drv_win_attrprocess_create();

    win_descp->expect_win_layermapping.layer_id = LAYER_ID_BUTT;
    win_descp->expect_win_layermapping.layer_order = 0;
    win_descp->expect_win_layermapping.layer_region_id = 0;
    win_descp->win_component.h_vpss = win_vpss_create(win_index, win_descp->src_handle, HI_FALSE);
    win_descp->win_component.h_vpss_capture = HI_INVALID_HANDLE;

    win_descp->win_index = win_index;
    ret = winmanage_add_window(win_descp, win_descp->win_user_attr.disp_chn);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("add window failed\n");
        DISP_FREE(win_descp);
        return ret;
    }

    *win_handle = win_descp->win_index;
    win_num = winmanage_get_win_num(win_attr->disp);

    if (win_attr->disp == HI_DRV_DISPLAY_0) {
        win_descp->expected_win_attr.win_zorder = win_num - 1;
    }
    ret = win_add_proc(win_descp->win_index);

    return ret;
}

static hi_void win_component_destroy(win_descriptor *win_descp)
{
    hi_s32 ret;

    ret = winmanage_del_window(win_descp->win_index);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("del window failed!\n");
    }

    ret = drv_win_attrprocess_destory(win_descp->win_component.h_attr);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("attr destroy failed!\n");
    }

    ret = win_frc_destory(win_descp->win_component.h_frc);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("frc destroy failed!\n");
    }

    ret = drv_fence_destroy_instance(win_descp->win_component.h_fence);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("fence destroy failed!\n");
    }

    win_vpss_destroy(win_descp->win_component.h_vpss);
    if (win_descp->win_component.h_vpss_capture != HI_INVALID_HANDLE) {
        win_vpss_destroy(win_descp->win_component.h_vpss_capture);
        if (win_descp->win_play_status.is_captured == HI_TRUE) {
            WIN_ERROR("captured frame is not released!\n");
        }
    }
    ret = win_buffer_destroy(win_descp->win_component.h_buf);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("buf destroy failed!\n");
    }
    return;
}

/* when ctrl+c , close ai + virt first,  display window last. */
hi_s32 drv_win_destroy_window(hi_handle win_handle)
{
    hi_s32 ret;
    hi_bool destroy_flag = HI_FALSE;
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);
    /* check wether can destroy window.  ai can destroy, */
    destroy_flag = win_check_can_destrory(win_descp);
    if (destroy_flag == HI_FALSE) {
        WIN_ERROR("can't destroy window!\n");
        return HI_ERR_VO_DESTROY_ERR;
    }

    win_remove_proc(win_descp->win_index);

    /* remove them first. */
    winmanage_unbind_windows(win_descp);

    /* when close,  window is disabled */
    ret = drv_win_set_enable(win_handle, HI_FALSE);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("disable window failed!\n");
    }

    ret = win_reset_buf(win_descp, HI_DRV_WIN_RESET_BLACK);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("reset window failed!\n");
    }

    win_component_destroy(win_descp);

    win_deinit_frame_list(&win_descp->win_frame_info);

    if ((win_descp->win_play_status.aquire_frame_cnt != win_descp->win_play_status.rls_frame_cnt) ||
        (win_descp->win_play_status.src_fence_access_cnts != win_descp->win_play_status.src_fence_rls_cnts)) {
        WIN_ERROR("cur value: Src queue:%d, dequeue:%d, src_acq:%d,src_rls:%d\n",
            win_descp->win_play_status.aquire_frame_cnt,
            win_descp->win_play_status.rls_frame_cnt, win_descp->win_play_status.src_fence_access_cnts,
            win_descp->win_play_status.src_fence_rls_cnts);
    }

    COMMON_DEBUG(BUFFER_INFOR, "cur value: recev:%d, destroy:%d, aq priv:%d,rls priv:%d\n",
        win_descp->win_play_status.aquire_frame_cnt, win_descp->win_play_status.src_fence_destroy_cnts,
        win_descp->win_play_status.src_fence_access_cnts, win_descp->win_play_status.src_fence_rls_cnts);


    ret = win_rls_resources(win_descp);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("rls destroy failed!\n");
    }

    osal_spin_lock_destory(&(win_descp->spinlock));

    DISP_FREE(win_descp);
    return HI_SUCCESS;
}

/* FIXME:  frame prvate data should be defined, and secureinfo should be filled by vpss */
hi_s32 drv_win_getvpssconfig(hi_void)
{
    /* 1 get wether ai vpss ,whether undergo vpss process; 2 get */
    return HI_SUCCESS;
}

hi_s32 drv_win_proc_get_playstatus(hi_handle win_handle, win_proc_play_status *win_play_status)
{
    win_descriptor *win_descp = HI_NULL;

    GET_WIN_PTR(win_handle, win_descp);

    win_play_status->aquire_frame_cnt = win_descp->win_play_status.aquire_frame_cnt;
    win_play_status->rls_frame_cnt = win_descp->win_play_status.rls_frame_cnt;

    win_play_status->last_queue_time = win_descp->win_play_status.last_queue_time;
    win_play_status->current_queue_time = win_descp->win_play_status.current_queue_time;
    win_play_status->queue_interval = win_descp->win_play_status.queue_interval;
    win_play_status->max_queue_interval = win_descp->win_play_status.max_queue_interval;

    win_play_status->src_fence_destroy_cnts = win_descp->win_play_status.src_fence_destroy_cnts;
    win_play_status->src_fence_timeout_cnts = win_descp->win_play_status.src_fence_timeout_cnts;
    win_play_status->src_fence_timeout_fd = win_descp->win_play_status.src_fence_timeout_fd;
    win_play_status->src_fence_access_cnts = win_descp->win_play_status.src_fence_access_cnts;
    win_play_status->src_fence_rls_cnts = win_descp->win_play_status.src_fence_rls_cnts;
    win_play_status->src_rls_atonce_cnts = win_descp->win_play_status.src_rls_atonce_cnts;
    win_play_status->sink_fence_create_cnts = win_descp->win_play_status.sink_fence_create_cnts;
    win_play_status->sink_fence_signal_cnts = win_descp->win_play_status.sink_fence_signal_cnts;

    win_play_status->min_frame_stay_time = win_descp->win_play_status.min_frame_stay_time;
    win_play_status->max_frame_stay_time = win_descp->win_play_status.max_frame_stay_time;
    win_play_status->newest_frame_stay_time = win_descp->win_play_status.newest_frame_stay_time;

    win_play_status->err_frame_counts = win_descp->win_play_status.err_frame_counts;

    win_play_status->tplay_speed_integer = win_descp->win_play_status.tplay_speed_integer;
    win_play_status->tplay_speed_decimal = win_descp->win_play_status.tplay_speed_decimal;
    return HI_SUCCESS;
}

hi_s32 win_suspend(hi_void)
{
    hi_handle *win_handle = HI_NULL;
    hi_u32 i, win_num;
    hi_drv_display disp;
    win_descriptor *win_descp = HI_NULL;
    hi_s32 ret;
    hi_ulong lock_flags = 0;

    for (disp = HI_DRV_DISPLAY_0; disp < HI_DRV_DISPLAY_BUTT; disp++) {
        win_num = winmanage_get_win_num((hi_drv_display)disp);
        win_handle = winmanage_get_win_handle((hi_drv_display)disp);
        for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
            if (win_handle[i] == 0) {
                continue;
            }

            GET_WIN_PTR(win_handle[i], win_descp);
            win_descp->win_play_ctrl.win_suspend = HI_TRUE;
            winmanage_get_lock(&lock_flags);
            if (win_descp->win_play_ctrl.win_enable == HI_TRUE) {
                /* for suspend process */
            }
            winmanage_rls_lock(&lock_flags);
        }
    }
    ret = win_vpss_suspend();

    return ret;
}

hi_s32 win_resume(hi_void)
{
    hi_handle *win_handle = HI_NULL;
    hi_u32 i, win_num;
    hi_drv_display disp;
    win_descriptor *win_descp = HI_NULL;
    hi_s32 ret;
    hi_ulong lock_flags = 0;

    ret = win_vpss_resume();
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss resume failed\n");
    }

    for (disp = HI_DRV_DISPLAY_0; disp < HI_DRV_DISPLAY_BUTT; disp++) {
        win_num = winmanage_get_win_num((hi_drv_display)disp);
        win_handle = winmanage_get_win_handle((hi_drv_display)disp);
        for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
            if (win_handle[i] == 0) {
                continue;
            }

            GET_WIN_PTR(win_handle[i], win_descp);
            winmanage_get_lock(&lock_flags);
            if (win_descp->win_play_ctrl.win_enable == HI_TRUE) {
                /* for win resume process */
            }
            winmanage_rls_lock(&lock_flags);
            win_descp->win_play_ctrl.win_suspend = HI_FALSE;
        }
    }

    return ret;
}

hi_s32 win_init(hi_void)
{
    hi_s32 ret;
    win_vpss_export_function export_func;
    hi_bool is_win_manag_success = HI_FALSE;
    hi_bool is_buf_init_success = HI_FALSE;
    hi_bool is_winattr_success = HI_FALSE;
    hi_bool is_frc_init_success = HI_FALSE;
    hi_bool is_fence_init_success = HI_FALSE;
    hi_bool is_vpss_init_success = HI_FALSE;

    /* init the global window management module. */
    ret = winmanage_init();
    if (ret != HI_SUCCESS) {
        WIN_ERROR("winmanage_init failed!\n");
        return ret;
    }
    is_win_manag_success = HI_TRUE;

    /* init all the component. */
    ret = win_buffer_init();
    if (ret != HI_SUCCESS) {
        WIN_ERROR("buff init failed!\n");
        goto __init_error;
    }
    is_buf_init_success = HI_TRUE;

    ret = drv_win_attrprocess_init();
    if (ret != HI_SUCCESS) {
        WIN_ERROR("attr init failed!\n");
        goto __init_error;
    }
    is_winattr_success = HI_TRUE;

    ret = win_frc_init();
    if (ret != HI_SUCCESS) {
        WIN_ERROR("frc init failed!\n");
        goto __init_error;
    }
    is_frc_init_success = HI_TRUE;

    ret = drv_fence_init();
    if (ret != HI_SUCCESS) {
        WIN_ERROR("fence init failed!\n");
        goto __init_error;
    }
    is_fence_init_success = HI_TRUE;

    export_func.vpss_complete_frame = drv_win_vpss_rls_src_frame;
    export_func.vpss_queue_frame = drv_win_vpss_queue_frame;
    export_func.vpss_dequeue_frame = drv_win_vpss_dequeue_frame;
    export_func.vpss_get_ai_result = HI_NULL;
    export_func.vpss_get_out_config = drv_win_vpss_get_out_config;

    ret = win_vpss_init(&export_func);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss init failed!\n");
        goto __init_error;
    }

    is_vpss_init_success = HI_TRUE;
    return HI_SUCCESS;

__init_error:
    if (is_win_manag_success == HI_TRUE) {
        winmanage_deinit();
    }
    if (is_buf_init_success == HI_TRUE) {
        win_buffer_deinit();
    }
    if (is_winattr_success == HI_TRUE) {
        drv_win_attrprocess_deinit();
    }
    if (is_frc_init_success == HI_TRUE) {
        win_frc_deinit();
    }
    if (is_fence_init_success == HI_TRUE) {
        drv_fence_deinit();
    }
    if (is_vpss_init_success == HI_TRUE) {
        win_vpss_deinit();
    }

    return ret;
}

hi_void win_deinit(hi_void)
{
    drv_win_attrprocess_deinit();
    win_frc_deinit();
    win_vpss_deinit();

    win_buffer_deinit();
    winmanage_deinit();
    drv_fence_deinit();

    return;
}
