/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: interface file for window.
 * Author: xdp group
 * Create: 2019-03-28
 */
#include "hi_type.h"
#include "hi_errno.h"

#include "hi_drv_video.h"
#include "hi_osal.h"

#include "drv_win_ioctl.h"
#include "drv_win_ext.h"
#include "drv_win_priv.h"

#include "drv_window.h"
#include "drv_xdp_osal.h"

#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "linux/hisilicon/securec.h"

#include "drv_xdp_ion.h"
#include "hi_osal.h"
#include "drv_win_mng.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define win_print_caller()                                                              \
    do {                                                                                 \
        COMMON_DEBUG(IOCTL_INFOR, "%s :called by id %d: %s\n",__FUNCTION__ ,get_current()->tgid, get_current()->comm); \
    } while (0)

osal_semaphore g_vo_mutex;
osal_semaphore g_win_mutex;

hi_s32 hi_drv_win_deinit(hi_void);

hi_bool g_vo_suspend = HI_FALSE;

static const win_proccess_cmd_ops g_win_cmd_func_array[] = {
    { CMD_WIN_CREATE,            win_proccess_cmd_create },
    { CMD_WIN_DESTROY,           win_proccess_cmd_destroy },
    { CMD_WIN_SET_ENABLE,        win_proccess_cmd_set_enable },
    { CMD_WIN_GET_ENABLE,        win_proccess_cmd_get_enable },
    { CMD_WIN_SET_ATTR,          win_proccess_cmd_set_attr },
    { CMD_WIN_GET_ATTR,          win_proccess_cmd_get_attr },
    { CMD_WIN_GET_LATEST_FRAME,  win_proccess_cmd_get_latest_frame },
    { CMD_WIN_SET_ZORDER,        win_proccess_cmd_set_zorder },
    { CMD_WIN_GET_FLIP,          win_proccess_cmd_get_flip},
    { CMD_WIN_SET_FLIP,          win_proccess_cmd_set_flip},
    { CMD_WIN_GET_ROTATION,      win_proccess_cmd_get_rotation},
    { CMD_WIN_SET_ROTATION,      win_proccess_cmd_set_rotation},
    { CMD_WIN_GET_ORDER,         win_proccess_cmd_get_zorder },
    { CMD_WIN_SET_SOURCE,        win_proccess_cmd_set_source },
    { CMD_WIN_GET_SOURCE,        win_proccess_cmd_get_source },
    { CMD_WIN_FREEZE,            win_proccess_cmd_freeze },
    { CMD_WIN_GET_FREEZE_STATUS, win_proccess_cmd_get_freeze_status },
    { CMD_WIN_SET_QUICK,         win_proccess_cmd_set_quickout },
    { CMD_WIN_GET_QUICK,         win_proccess_cmd_get_quickout_status },
    { CMD_VO_WIN_CAPTURE_START,  win_proccess_cmd_capture_acquire },
    { CMD_VO_WIN_CAPTURE_RELEASE, win_proccess_cmd_capture_release },
    { CMD_WIN_QU_FRAME,          win_proccess_cmd_qu_frame },
    { CMD_WIN_DQ_FRAME,          win_proccess_cmd_dq_frame },
    { CMD_WIN_RESET,             win_proccess_cmd_reset },
    { CMD_WIN_PAUSE,             win_proccess_cmd_pause },
    { CMD_WIN_GET_PAUSE_STATUS,  win_proccess_cmd_get_pause_status},
    { CMD_WIN_GET_PLAY_INFO,     win_proccess_cmd_get_play_info },
    { CMD_WIN_GET_INFO,          win_proccess_cmd_get_Info },
    { CMD_WIN_SET_SYNC,          win_proccess_cmd_set_sync },
    { CMD_WIN_VIR_ACQUIRE,       win_proccess_cmd_acquire_frame },
    { CMD_WIN_VIR_RELEASE,       win_proccess_cmd_release_frame },
    { CMD_WIN_SET_PLAY_CTRL,     win_proccess_cmd_set_playctl_info},
    { CMD_WIN_GET_PLAY_CTRL,     win_proccess_cmd_get_playctl_info},
    { CMD_WIN_SET_FRAME_PROGINTER, win_proccess_cmd_set_frame_proginterleave_mode},
    { CMD_WIN_GET_FRAME_PROGINTER, win_proccess_cmd_get_frame_proginterleave_mode},
    { CMD_WIN_GET_HANDLE, win_proccess_cmd_win_get_handle},
};

static win_export_func_s g_win_export_funcs = {};

static hi_void record_win_to_devfile(hi_handle win_handle, hi_handle is_virt_window, hi_drv_display disp,
    win_state *win_state_info)
{
    win_state_info->win[disp][(hi_u32)win_handle & 0xff] = win_handle;
    return;
}

static hi_void remove_win_to_devfile(hi_handle win_handle, win_state *win_state_info)
{
    hi_drv_display disp_chn;
    hi_u32 i = 0;

    disp_chn = (hi_u32)HI_HANDLE_GET_PRIVATE_DATA(win_handle);
    i = HI_HANDLE_GET_CHAN_ID(win_handle) & 0x00ff;
    win_state_info->win[disp_chn][i] = HI_NULL;

    return;
}

static hi_s32 win_proccess_cmd_create(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_create *win_create_info = HI_NULL;
    win_print_caller();

    win_create_info = (win_create *)arg;

    ret = drv_win_create_window(&win_create_info->win_attr, &win_create_info->win_handle);
    if (ret == HI_SUCCESS) {
        record_win_to_devfile(win_create_info->win_handle, win_create_info->win_attr.is_virtual,
                              win_create_info->win_attr.disp, win_state_info);
    }
    COMMON_DEBUG(IOCTL_INFOR, "win create %d,asp mode(%d),w,h(%d,%d),outrect(%d,%d,%d,%d)\n",
                 win_create_info->win_handle,
                 win_create_info->win_attr.aspect_ratio_mode,
                 win_create_info->win_attr.aspect_ratio.aspect_ratio_w,
                 win_create_info->win_attr.aspect_ratio.aspect_ratio_h,
                 win_create_info->win_attr.out_rect.x,
                 win_create_info->win_attr.out_rect.y,
                 win_create_info->win_attr.out_rect.width,
                 win_create_info->win_attr.out_rect.height);

    return ret;
}

static hi_s32 win_proccess_cmd_destroy(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    hi_handle win_handle;
    win_print_caller();

    win_handle = *((hi_handle *)arg);

    remove_win_to_devfile(win_handle, win_state_info);

    ret = drv_win_destroy_window(win_handle);
    COMMON_DEBUG(IOCTL_INFOR, "win 0x%x destroy.\n", win_handle);
    return ret;
}

static hi_s32 win_proccess_cmd_set_enable(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_enable *vo_win_enable = HI_NULL;
    win_print_caller();

    vo_win_enable = (win_enable *)arg;
    ret = drv_win_set_enable(vo_win_enable->win_handle, ((vo_win_enable->is_enable == HI_TRUE) ? HI_TRUE : HI_FALSE));
    COMMON_DEBUG(IOCTL_INFOR, "win 0x%x set enable %d\n", vo_win_enable->win_handle,
                 vo_win_enable->is_enable);
    return ret;
}

static hi_s32 win_proccess_cmd_get_enable(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_enable *vo_win_enable = HI_NULL;

    vo_win_enable = (win_enable *)arg;

    ret = drv_win_get_enable(vo_win_enable->win_handle, &vo_win_enable->is_enable);
    return ret;
}

static hi_s32 win_proccess_cmd_set_attr(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_create *vo_win_attr;
    vo_win_attr = (win_create *)arg;

    ret = drv_win_set_attr(vo_win_attr->win_handle, &vo_win_attr->win_attr);

    return ret;
}

static hi_s32 win_proccess_cmd_get_attr(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_create *vo_win_attr = HI_NULL;
    vo_win_attr = (win_create *)arg;

    ret = drv_win_get_attr(vo_win_attr->win_handle, &vo_win_attr->win_attr);
    return ret;
}

static hi_s32 win_proccess_cmd_get_latest_frame(hi_void *arg, win_state *win_state_info)
{
    /* unsupport */
    return HI_SUCCESS;
}

static hi_s32 win_proccess_cmd_set_flip(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_flip *vo_win_flip = HI_NULL;
    win_print_caller();

    vo_win_flip = (win_flip *)arg;

    ret = drv_win_set_flip(vo_win_flip->win, vo_win_flip->flip_hori, vo_win_flip->flip_vert);

    COMMON_DEBUG(IOCTL_INFOR, "win set flip-> flip_hori:0x%x, flip_vert:0x%x, ret %d\n",
                 vo_win_flip->flip_hori, vo_win_flip->flip_vert, ret);

    return ret;
}

static hi_s32 win_proccess_cmd_get_flip(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_flip *vo_win_flip = HI_NULL;
    vo_win_flip = (win_flip *)arg;

    ret = drv_win_get_flip(vo_win_flip->win, &(vo_win_flip->flip_hori), &(vo_win_flip->flip_vert));

    return ret;
}


static hi_s32 win_proccess_cmd_set_rotation(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_rotation *vo_win_rotation = HI_NULL;
    vo_win_rotation = (win_rotation *)arg;
    win_print_caller();

    ret = drv_win_set_rotation(vo_win_rotation->win, vo_win_rotation->rotation);

    COMMON_DEBUG(IOCTL_INFOR, "win set rotation win0x%x, rotation:%d, ret %x\n",
                 vo_win_rotation->win, vo_win_rotation->rotation, ret);

    return ret;
}

static hi_s32 win_proccess_cmd_get_rotation(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_rotation *vo_win_rotation = HI_NULL;
    vo_win_rotation = (win_rotation *)arg;
    win_print_caller();

    ret = drv_win_get_rotation(vo_win_rotation->win, &vo_win_rotation->rotation);

    COMMON_DEBUG(IOCTL_INFOR, "win get rotation win0x%x, rotation:%d, ret %x\n",
                 vo_win_rotation->win, vo_win_rotation->rotation, ret);

    return ret;
}

static hi_s32 win_proccess_cmd_set_zorder(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_zorder *vo_win_zorder = HI_NULL;

    vo_win_zorder = (win_zorder *)arg;

    ret = drv_win_set_zorder(vo_win_zorder->win_handle, vo_win_zorder->zflag);

    return ret;
}

static hi_s32 win_proccess_cmd_get_zorder(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;

    win_order *vo_win_order = HI_NULL;
    vo_win_order = (win_order *)arg;

    ret = drv_win_get_zorder(vo_win_order->win_handle, &vo_win_order->order);
    return ret;
}

static hi_s32 win_proccess_cmd_set_source(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_source *vo_win_attach = HI_NULL;
    hi_drv_win_src_info stWinSrc = { 0 };
    win_print_caller();

    vo_win_attach = (win_source *)arg;
    stWinSrc.src_handle = vo_win_attach->src_info.src;

    ret = drv_win_set_source(vo_win_attach->win_handle, stWinSrc.src_handle);
    COMMON_DEBUG(IOCTL_INFOR, "win 0x%x set source 0x%x\n", vo_win_attach->win_handle,
                 vo_win_attach->src_info.src);
    return ret;
}

static hi_s32 win_proccess_cmd_get_source(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret = HI_SUCCESS;
    win_source *vo_win_attach = HI_NULL;
    vo_win_attach = (win_source *)arg;
    return ret;
}

static hi_s32 win_proccess_cmd_freeze(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_freeze *vo_win_freeze = HI_NULL;
    win_print_caller();

    vo_win_freeze = (win_freeze *)arg;

    ret = drv_win_freeze(vo_win_freeze->win_handle, vo_win_freeze->freeze_mode);
    COMMON_DEBUG(IOCTL_INFOR, "win 0x%x set freeze_mode %d\n", vo_win_freeze->win_handle,
                 vo_win_freeze->freeze_mode);
    return ret;
}

static hi_s32 win_proccess_cmd_get_freeze_status(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_freeze *vo_win_freeze = HI_NULL;

    vo_win_freeze = (win_freeze *)arg;
    ret = drv_win_get_freeze_status(vo_win_freeze->win_handle, &vo_win_freeze->freeze_mode);

    return ret;
}

static hi_s32 win_proccess_cmd_set_quickout(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_quickout *vo_win_quickout = HI_NULL;
    win_print_caller();

    vo_win_quickout = (win_quickout *)arg;

    ret = drv_win_set_quickout(vo_win_quickout->win_handle, vo_win_quickout->enable);
    COMMON_DEBUG(IOCTL_INFOR, "win 0x%x set quickout enable %d\n", vo_win_quickout->win_handle,
                 vo_win_quickout->enable);
    return ret;
}

static hi_s32 win_proccess_cmd_get_quickout_status(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_quickout *vo_win_quickout = HI_NULL;

    vo_win_quickout = (win_quickout *)arg;
    ret = drv_win_get_quickout_status(vo_win_quickout->win_handle, &vo_win_quickout->enable);

    return ret;
}

static hi_s32 win_proccess_cmd_capture_acquire(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_capture *capture_start = HI_NULL;
    win_print_caller();

    capture_start = (win_capture *)arg;
    ret = drv_win_acquire_captured_frame(capture_start->win, &(capture_start->captured_pic));
    if (ret != HI_SUCCESS) {
        WIN_FATAL("capture frame failed\n");
        return ret;
    }

    ret = drv_win_transfer_dma_to_fd(&capture_start->captured_pic, HI_TRUE);
    if (ret != HI_SUCCESS) {
        WIN_FATAL("transfer dma to fd failed.\n");
        return ret;
    }
    COMMON_DEBUG(IOCTL_INFOR, "capture_start, index:%d.\n", capture_start->captured_pic.frame_index);

    return ret;
}

static hi_s32 win_proccess_cmd_capture_release(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_capture *capture_rls = HI_NULL;
    win_print_caller();

    capture_rls = (win_capture *)arg;

    ret = drv_win_transfer_fd_to_dma(&capture_rls->captured_pic, HI_TRUE);
    if (ret != HI_SUCCESS) {
        WIN_FATAL("transfer fd to frame failed.\n");
        return ret;
    }

    ret = drv_win_release_captured_frame(capture_rls->win, &(capture_rls->captured_pic));
    COMMON_DEBUG(IOCTL_INFOR, "capture_release, index:%d.\n", capture_rls->captured_pic.frame_index);

    return ret;
}
static hi_s32 win_proccess_cmd_acquire_frame(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_frame *vo_win_frame = HI_NULL;
    vo_win_frame = (win_frame *)arg;
    win_print_caller();

    ret = drv_win_aquire_frame(vo_win_frame->win_handle, &vo_win_frame->frame);
    if (ret == HI_SUCCESS) {
        ret = drv_win_transfer_dma_to_fd(&vo_win_frame->frame, HI_TRUE);
        if (ret != HI_SUCCESS) {
            WIN_FATAL("transfer dma to fd failed.\n");
            drv_win_release_frame(vo_win_frame->win_handle, &vo_win_frame->frame);
        }
    }

    COMMON_DEBUG(BUFFER_INFOR, "Acquire frame src_addr_fd:(0x%llx)\n",
                 vo_win_frame->frame.buf_addr[0].start_addr);
    return ret;
}

static hi_s32 win_proccess_cmd_release_frame(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_frame *vo_win_frame = HI_NULL;
    win_print_caller();

    vo_win_frame = (win_frame *)arg;

    ret = drv_win_transfer_fd_to_dma(&vo_win_frame->frame, HI_TRUE);
    if (ret != HI_SUCCESS) {
        WIN_FATAL("transfer fd to frame failed.\n");
        return ret;
    }

    ret = drv_win_release_frame(vo_win_frame->win_handle, &vo_win_frame->frame);
    COMMON_DEBUG(BUFFER_INFOR, "Release frame src_addr_fd:(0x%llx)\n",
                 vo_win_frame->frame.buf_addr[0].start_addr);
    return ret;
}

static hi_s32 win_proccess_cmd_qu_frame(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_xdr_frame *vo_win_frame = HI_NULL;

    vo_win_frame = (win_xdr_frame *)arg;

    ret = drv_win_transfer_fd_to_dma(&vo_win_frame->base_frame, HI_FALSE);
    if (ret != HI_SUCCESS) {
        WIN_ERROR(":transfer fd to dma failed.\n");
        return ret;
    }

    COMMON_DEBUG(VPSS_INFOR, "fd:0x%llx  dma:0x%llx\n", vo_win_frame->base_frame.buf_addr[0].start_addr,
                 vo_win_frame->base_frame.buf_addr[0].dma_handle);

    if (vo_win_frame->fence_fd == 0) {
        ret = drv_win_queue_frame(vo_win_frame->win_handle, &vo_win_frame->base_frame, HI_NULL);
    } else {
        ret = drv_win_queue_frame(vo_win_frame->win_handle, &vo_win_frame->base_frame, &vo_win_frame->fence_fd);
    }

    COMMON_DEBUG(BUFFER_INFOR, "Queue frame src_addr_fd:(0x%llx),overlay fd(%d) \n",
                 vo_win_frame->base_frame.buf_addr[0].start_addr,
                 vo_win_frame->fence_fd);
    return ret;
}

static hi_s32 win_proccess_cmd_dq_frame(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_frame *vo_win_frame = HI_NULL;

    vo_win_frame = (win_frame *)arg;

    ret = drv_win_dequeue_frame(vo_win_frame->win_handle, &vo_win_frame->frame);
    if (ret != HI_SUCCESS) {
        WIN_WARN("Dequeue frame error.\n");
        return ret;
    }

    ret = drv_win_transfer_dma_to_fd(&vo_win_frame->frame, HI_FALSE);
    if (ret != HI_SUCCESS) {
        WIN_FATAL("transfer dma to fd failed.\n");
    }

    COMMON_DEBUG(BUFFER_INFOR, "Deueue frame src_addr_fd:0x%llx  dma:0x%llx\n",
                 vo_win_frame->frame.buf_addr[0].start_addr,
                 vo_win_frame->frame.buf_addr[0].dma_handle);
    return HI_SUCCESS;
}

static hi_s32 win_proccess_cmd_reset(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_reset *vo_win_reset = HI_NULL;
    win_print_caller();

    vo_win_reset = (win_reset *)arg;

    ret = drv_win_reset(vo_win_reset->win_handle, vo_win_reset->reset_mode);
    COMMON_DEBUG(IOCTL_INFOR, "win 0x%x set reset mode %d\n", vo_win_reset->win_handle,
                 vo_win_reset->reset_mode);
    return ret;
}

static hi_s32 win_proccess_cmd_pause(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_pause *vo_win_pause = HI_NULL;
    win_print_caller();

    vo_win_pause = (win_pause *)arg;

    ret = drv_win_pause(vo_win_pause->win_handle, vo_win_pause->enable);
    COMMON_DEBUG(IOCTL_INFOR, "win 0x%x set pause enable %d\n", vo_win_pause->win_handle,
                 vo_win_pause->enable);
    return ret;
}

static hi_s32 win_proccess_cmd_get_pause_status(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_pause *vo_win_pause = HI_NULL;

    vo_win_pause = (win_pause *)arg;

    ret = drv_win_get_pause_status(vo_win_pause->win_handle, &vo_win_pause->enable);
    return ret;
}

static hi_s32 win_proccess_cmd_get_play_info(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_play_info *win_play_status = HI_NULL;

    win_play_status = (win_play_info *)arg;
    ret = drv_win_get_play_info(win_play_status->win_handle, &win_play_status->play_info);

    return ret;
}

static hi_s32 win_proccess_cmd_get_Info(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret = HI_SUCCESS;
    win_priv_info *vo_win_delay = HI_NULL;

    vo_win_delay = (win_priv_info *)arg;
    return ret;
}

static hi_s32 win_proccess_cmd_set_sync(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_sync *win_sync_cfg = HI_NULL;

    win_sync_cfg = (win_sync *)arg;
    ret = drv_win_set_sync(win_sync_cfg->win_handle, win_sync_cfg->sync_mode);

    return ret;
}

static hi_s32 win_proccess_cmd_set_playctl_info(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_play_ctrl *win_playctl_info = HI_NULL;
    win_print_caller();

    win_playctl_info = (win_play_ctrl *)arg;
    ret = drv_win_set_playctl_info(win_playctl_info->win_handle, &win_playctl_info->play_ctrl_info);
    COMMON_DEBUG(IOCTL_INFOR, "win 0x%x set playctl frame type %d,int %d,dec %d\n", win_playctl_info->win_handle,
                 win_playctl_info->play_ctrl_info.frame_type,
                 win_playctl_info->play_ctrl_info.speed_integer,
                 win_playctl_info->play_ctrl_info.speed_decimal);
    return ret;
}

static hi_s32 win_proccess_cmd_get_playctl_info(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_play_ctrl *win_playctl_info = HI_NULL;;

    win_playctl_info = (win_play_ctrl *)arg;
    ret = drv_win_get_playctl_info(win_playctl_info->win_handle, &win_playctl_info->play_ctrl_info);
    return ret;
}

static hi_s32 win_proccess_cmd_set_frame_proginterleave_mode(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_forceframe_flag *win_frame_mode = HI_NULL;;
    win_print_caller();

    win_frame_mode = (win_forceframe_flag *)arg;
    ret = drv_win_set_frame_proginterleave_mode(win_frame_mode->win, win_frame_mode->force_frame_prog_flag);
    COMMON_DEBUG(IOCTL_INFOR, "win 0x%x set prog flag %d\n", win_frame_mode->win,
                 win_frame_mode->force_frame_prog_flag);
    return ret;
}

static hi_s32 win_proccess_cmd_get_frame_proginterleave_mode(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_forceframe_flag *win_frame_mode = HI_NULL;;

    win_frame_mode = (win_forceframe_flag *)arg;
    ret = drv_win_get_frame_proginterleave_mode(win_frame_mode->win, &win_frame_mode->force_frame_prog_flag);
    return ret;
}

static hi_s32 win_proccess_cmd_win_get_handle(hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret;
    win_get_handle *get_handle = HI_NULL;;

    get_handle = (win_get_handle *)arg;
    ret = drv_win_get_handle(get_handle->disp, &get_handle->win_handle);
    return ret;
}

hi_s32 win_process_cmd(unsigned int cmd, hi_void *arg, win_state *win_state_info)
{
    hi_s32 ret = -ENOIOCTLCMD;
    hi_u32 index = 0;

    if ((arg == HI_NULL) || (win_state_info == HI_NULL)) {
        WIN_ERROR("ioctl null ptr.\n");
        return HI_ERR_VO_NULL_PTR;
    }


    for (index = 0; (index < sizeof(g_win_cmd_func_array) / sizeof(win_proccess_cmd_ops)); index++) {
        if ((g_win_cmd_func_array[index].cmd) == cmd && (HI_NULL != g_win_cmd_func_array[index].win_process_cmd_func)) {
            ret = g_win_cmd_func_array[index].win_process_cmd_func(arg, win_state_info);
            break;
        }
    }

    return ret;
}

hi_s32 drv_win_register(hi_void)
{
    hi_s32 ret;

    ret = hi_drv_module_register((hi_u32)HI_ID_WIN, "HI_VO", (hi_void *)(&g_win_export_funcs), HI_NULL);
    if (ret != HI_SUCCESS) {
        hi_fatal_win("HI_DRV_MODULE_Register VO failed\n");
        return ret;
    }

    return HI_SUCCESS;
}

hi_void drv_win_un_register(hi_void)
{
    if (hi_drv_win_deinit() != HI_SUCCESS) {
        hi_fatal_win("Call HI_DRV_WIN_DeInit faild.\n");
    }

    if (hi_drv_module_unregister(HI_ID_WIN) != HI_SUCCESS) {
        hi_fatal_win("Call HI_DRV_MODULE_UnRegister faild.\n");
    }

    return;
}

hi_s32 drv_win_open(struct inode *finode, struct file *ffile)
{
    win_state *win_state_info = HI_NULL;

    if (ffile == HI_NULL) {
        WIN_ERROR("null file descpt.\n");
        return HI_ERR_VO_NULL_PTR;
    }

    win_state_info = osal_kmalloc(HI_ID_WIN, sizeof(win_state), OSAL_GFP_KERNEL);
    if (win_state_info == HI_NULL) {
        WIN_FATAL("malloc failed.\n");
        return HI_ERR_VO_MALLOC_FAILED;
    }

    memset_s(win_state_info, sizeof(win_state), 0, sizeof(win_state));
    win_state_info->thread_file_magic = 0xdeadbeefdeadbeaf;
    ffile->private_data = win_state_info;

    return HI_SUCCESS;
}

hi_s32 drv_win_init(hi_void)
{
    hi_s32 ret;
    ret = osal_sem_init(&g_vo_mutex, 1);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("osal_sem_init failed");
        return HI_FAILURE;
    }
    ret = osal_sem_init(&g_win_mutex, 1);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("osal_sem_init failed");
        return HI_FAILURE;
    }

    return win_init();
}

hi_void drv_win_deinit(hi_void)
{
    win_deinit();

    if (g_win_mutex.sem != HI_NULL) {
        osal_sem_destory(&g_win_mutex);
    }

    if (g_vo_mutex.sem != HI_NULL) {
        osal_sem_destory(&g_vo_mutex);
    }

    return;
}

hi_s32 win_drv_destroyall(win_state *win_state_info)
{
    hi_drv_display enDisp;
    hi_s32 i = 0;

    if (win_state_info == HI_NULL) {
        WIN_ERROR("null ptr.\n");
        return HI_ERR_VO_NULL_PTR;
    }

    for (enDisp = 0; enDisp < HI_DRV_DISPLAY_BUTT; enDisp++) {
        for (i = 0; i < DEF_MAX_WIN_NUM_ON_SINGLE_DISP; i++) {
            if (win_state_info->win[enDisp][i] != HI_NULL) {
                drv_win_destroy_window(win_state_info->win[enDisp][i]);
                remove_win_to_devfile(win_state_info->win[enDisp][i], win_state_info);
            }
        }
    }

    for (i = 0; i < DEF_MAX_WIN_NUM_ON_VIRTUAL_DISP; i++) {
        if (win_state_info->virtual_win[i] != HI_NULL) {
            drv_win_destroy_window(win_state_info->virtual_win[i]);
            remove_win_to_devfile(win_state_info->virtual_win[i], win_state_info);
        }
    }

    return HI_SUCCESS;
}

hi_s32 drv_win_close(struct inode *finode, struct file *ffile)
{
    win_state *win_state_info = HI_NULL;
    hi_s32 ret;
    hi_u32 ref_increase_cnts = 0;
    hi_u32 ref_decrease_cnts = 0;

    if (ffile == HI_NULL) {
        WIN_ERROR("null ptr.\n");
        return HI_ERR_VO_NULL_PTR;
    }

    ret = osal_sem_down(&g_win_mutex);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("osal_sem_down failed\n");
        return HI_FAILURE;
    }

    win_state_info = ffile->private_data;
    if (win_state_info == HI_NULL) {
        osal_sem_up(&g_win_mutex);
        WIN_FATAL("win state file is null.\n");
        return HI_FAILURE;
    }

    /* 1. close all the handles opened by the same thread. */
    ret = win_drv_destroyall(win_state_info);
    if (ret != HI_SUCCESS) {
        WIN_FATAL("Call WIN_DRV_DestroyAll failed!\n");
        osal_sem_up(&g_win_mutex);
        return ret;
    }


    osal_kfree(HI_ID_WIN, win_state_info);
    ffile->private_data = HI_NULL;

    drv_xdp_mem_get_refcnt(&ref_increase_cnts, &ref_decrease_cnts);
    if (ref_increase_cnts != ref_decrease_cnts) {
        COMMON_DEBUG(ANDROID_FENCE_INFOR, "#@ dma-mem ref-inc:%d, ref-dec:%d !\n", ref_increase_cnts,
            ref_decrease_cnts);
    }

    drv_xdp_mem_reset_refcnt();
    osal_sem_up(&g_win_mutex);
    return HI_SUCCESS;
}

hi_slong drv_win_ioctl(struct file *file, hi_u32 cmd, hi_void *arg)
{
    win_state *vo_win_state = HI_NULL;
    hi_s32 ret;

    if ((file == HI_NULL) || (arg == HI_NULL)) {
        WIN_ERROR("null ptr.\n");
        return HI_ERR_VO_NULL_PTR;
    }

    ret = osal_sem_down(&g_vo_mutex);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("drv_win_ioctl failed\n");
        return HI_FAILURE;
    }

    vo_win_state = file->private_data;
    ret = win_process_cmd(cmd, arg, vo_win_state);

    osal_sem_up(&g_vo_mutex);
    return ret;
}

hi_s32 drv_win_suspend(struct file *file, hi_u32 cmd, hi_void *arg)
{
    return HI_SUCCESS;
}

hi_s32 drv_win_resume(struct file *file, hi_u32 cmd, hi_void *arg)
{
    return HI_SUCCESS;
}

hi_s32 hi_drv_win_init(hi_void)
{
    return HI_SUCCESS;
}

hi_s32 hi_drv_win_deinit(hi_void)
{
    return HI_SUCCESS;
}

hi_s32 hi_drv_win_vpss_queue_frame(hi_handle win_handle, hi_drv_video_frame *image_info)
{
    hi_s32 ret;

    ret = drv_win_vpss_queue_frame(win_handle, image_info);
    return ret;
}

hi_s32 hi_drv_win_vpss_dequeue_frame(hi_handle win_handle, hi_drv_video_frame *image_info)
{
    hi_s32 ret;

    ret = drv_win_vpss_dequeue_frame(win_handle, image_info);
    return ret;
}

hi_s32 hi_drv_win_vpss_complete_frame(hi_handle win_handle, hi_drv_video_frame *frame_info)
{
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
