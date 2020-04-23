#ifndef __DRV_VDEC_VFMW_H__
#define __DRV_VDEC_VFMW_H__

#include "hi_vdec_type.h"
#include "drv_vdec_osal.h"

typedef hi_s32 (*fn_vdec_vfmw_callback)(hi_u32 chan_id, hi_u32 type, hi_void *args, hi_u32 size);

typedef struct {
    hi_handle vdec_handle;
    hi_u32 vdec_id;
    hi_handle ssm_handle;
    hi_vdec_attr *attr;
    fn_vdec_vfmw_callback callback;
} vdec_vfmw_create_param;

typedef struct {
    hi_bool enable_uvmos;
    hi_bool enable_low_delay;
    hi_vdec_dpb_mode dpb_mode;
    hi_vdec_hdr_attr hdr_attr;
    hi_vdec_stm_buf_mode stm_mode;
    hi_vdec_frm_buf_mode frm_mode;
    hi_vdec_out_format out_format;
    hi_u32  poc_thrshold;
    hi_vdec_src_tag source_tag;
    hi_bool is_pvr_info_valid;
    hi_vdec_pvr_info pvr_info;
    hi_u32 frm_3d_type;
    hi_vdec_speed speed_info;
    hi_vdec_frm_rate frame_rate;
    hi_vdec_start_pts start_pts;
    hi_vdec_dec_mode dec_mode;
} vdec_vfmw_param;

hi_s32 vdec_vfmw_open(hi_void);
hi_s32 vdec_vfmw_close(hi_void);
hi_s32 vdec_vfmw_init(hi_void);
hi_s32 vdec_vfmw_exit(hi_void);
hi_s32 vdec_vfmw_create(hi_u32 *chan_id, const vdec_vfmw_create_param *param);
hi_s32 vdec_vfmw_destroy(hi_u32 chan_id,  const hi_vdec_opt_destroy *param);
hi_s32 vdec_vfmw_start(hi_u32 chan_id, const hi_vdec_opt_start *param);
hi_s32 vdec_vfmw_stop(hi_u32 chan_id, const hi_vdec_opt_stop *param);
hi_s32 vdec_vfmw_reset(hi_u32 chan_id, const hi_vdec_opt_reset *param);
hi_s32 vdec_vfmw_flush(hi_u32 chan_id, const hi_vdec_opt_flush *param);
hi_s32 vdec_vfmw_set_event_map(hi_u32 chan_id, hi_u32 event_map);
hi_s32 vdec_vfmw_decode(hi_u32 chan_id, const hi_vdec_opt_decode *option,
    const hi_vdec_stream *stream, hi_void *frame, hi_u32 size);
hi_s32 vdec_vfmw_set_attr(hi_u32 chan_id, const hi_vdec_attr *param, const vdec_vfmw_param *ext_param);
hi_s32 vdec_vfmw_queue_stream(hi_u32 chan_id, const hi_vdec_opt_que_stm *option, const hi_vdec_stream *stream);
hi_s32 vdec_vfmw_dequeue_stream(hi_u32 chan_id, const hi_vdec_opt_deq_stm *option, hi_vdec_stream *stream);
hi_s32 vdec_vfmw_acquire_frame(hi_u32 chan_id, const hi_vdec_opt_acq_frm *option, hi_void *frame, hi_u32 size,
    hi_vdec_ext_frm_info *ext_frm_info);
hi_s32 vdec_vfmw_release_frame(hi_u32 chan_id, const hi_vdec_opt_rel_frm *option, const hi_void *frame, hi_u32 size);
hi_s32 vdec_vfmw_get_param(hi_u32 chan_id, hi_vdec_param_id param_id, hi_u8 *param, hi_u32 param_size);
hi_s32 vdec_vfmw_set_param(hi_u32 chan_id, hi_vdec_param_id param_id, const hi_u8 *param, hi_u32 param_size);
hi_s32 vdec_vfmw_command(hi_u32 chan_id, hi_vdec_cmd_id cmd_id, hi_u8 *param, hi_u32 param_size);
hi_s32 vdec_vfmw_suspend(hi_void);
hi_s32 vdec_vfmw_resume(hi_void);
hi_s32 vdec_vfmw_read_proc(hi_void *pf, hi_u32 chan_id);
hi_s32 vdec_vfmw_wakeup(hi_u32 chan_id, hi_u8 *param, hi_u32 size);

#endif
