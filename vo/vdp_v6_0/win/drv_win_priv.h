#ifndef _DRV_WIN_PRIV_H_
#define _DRV_WIN_PRIV_H_

#define WIN_DEBUG_CALLER()                                                              \
    do {                                                                                \
        hi_warn_win("called by id %d: %s\n", get_current()->tgid, get_current()->comm); \
    } while (0)

typedef struct win_proccess_cmd_func {
    unsigned int cmd;
    hi_s32 (*win_process_cmd_func)(hi_void *arg, win_state *win_state_info);
} win_proccess_cmd_ops;

static hi_s32 win_proccess_cmd_create(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_destroy(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_set_enable(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_get_enable(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_set_attr(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_get_attr(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_get_latest_frame(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_get_flip(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_set_flip(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_get_rotation(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_set_rotation(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_set_zorder(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_get_zorder(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_set_source(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_get_source(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_freeze(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_get_freeze_status(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_set_quickout(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_get_quickout_status(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_capture_acquire(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_capture_release(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_qu_frame(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_dq_frame(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_reset(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_pause(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_get_pause_status(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_get_play_info(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_get_Info(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_set_sync(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_acquire_frame(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_release_frame(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_set_playctl_info(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_get_playctl_info(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_set_frame_proginterleave_mode(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_get_frame_proginterleave_mode(hi_void *arg, win_state *win_state_info);
static hi_s32 win_proccess_cmd_win_get_handle(hi_void *arg, win_state *win_state_info);

#endif
