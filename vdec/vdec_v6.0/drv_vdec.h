#ifndef __DRV_VDEC_H__
#define __DRV_VDEC_H__

#include "hi_vdec_type.h"
#include "drv_vdec_osal.h"

typedef struct {
    hi_bool is_user_space;
    hi_vdec_stream *stream;
    hi_void *frame;
    hi_u32 frame_size;
    hi_vdec_ext_frm_info *ext_frm_info;
    hi_u32 time_out;
} drv_vdec_decode_param;

hi_void drv_vdec_init(hi_void);
hi_void drv_vdec_deinit(hi_void);
hi_s32 drv_vdec_open(hi_void);
hi_s32 drv_vdec_close(hi_void);
hi_s32 drv_vdec_create(hi_u32 *chan_id, const hi_vdec_opt_create *option, hi_void *filp, hi_u32 size);
hi_s32 drv_vdec_destroy(hi_u32 chan_id, const hi_vdec_opt_destroy *option);
hi_s32 drv_vdec_start(hi_u32 chan_id, const hi_vdec_opt_start *option);
hi_s32 drv_vdec_stop(hi_u32 chan_id, const hi_vdec_opt_stop *option);
hi_s32 drv_vdec_reset(hi_u32 chan_id, const hi_vdec_opt_reset *option);
hi_s32 drv_vdec_flush(hi_u32 chan_id, const hi_vdec_opt_flush *option);
hi_s32 drv_vdec_decode(hi_u32 chan_id, drv_vdec_decode_param *param);
hi_s32 drv_vdec_get_attr(hi_u32 chan_id, hi_vdec_attr *attr);
hi_s32 drv_vdec_set_attr(hi_u32 chan_id, hi_handle vdec_handle, const hi_vdec_attr *attr);
hi_s32 drv_vdec_register_event(hi_u32 chan_id, hi_u32 event_map);
hi_s32 drv_vdec_listen_event(hi_u32 chan_id, const hi_vdec_opt_listen *option, hi_vdec_event *event);
hi_s32 drv_vdec_queue_stream(hi_u32 chan_id, const hi_vdec_opt_que_stm *option, const hi_vdec_stream *stream);
hi_s32 drv_vdec_dequeue_stream(hi_u32 chan_id, const hi_vdec_opt_deq_stm *option, hi_vdec_stream *stream);
hi_s32 drv_vdec_acquire_frame(hi_u32 chan_id, const hi_vdec_opt_acq_frm *option, hi_void *frame, hi_u32 size,
    hi_vdec_ext_frm_info *ext_frm_info);
hi_s32 drv_vdec_release_frame(hi_u32 chan_id, const hi_vdec_opt_rel_frm *option, const hi_void *frame, hi_u32 size);
hi_s32 drv_vdec_get_param(hi_u32 chan_id, hi_vdec_param_id param_id, hi_u8 *param, hi_u32 param_size);
hi_s32 drv_vdec_set_param(hi_u32 chan_id, hi_vdec_param_id param_id, const hi_u8 *param, hi_u32 param_size);
hi_s32 drv_vdec_command(hi_u32 chan_id, hi_vdec_cmd_id cmd_id, hi_u8 *param, hi_u32 param_size);
hi_s32 drv_vdec_register_callback(hi_u32 chan_id, hi_handle user_handle, fn_drv_vdec_callback func);
hi_s32 drv_vdec_suspend(hi_void);
hi_s32 drv_vdec_resume(hi_void);
hi_void drv_vdec_release(hi_void *filp, hi_u32 size);
hi_s32 drv_vdec_read_proc(hi_void *fp, hi_u32 chan_id);

#endif

