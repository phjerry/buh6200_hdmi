#include "drv_vdec_ext.h"
#include "drv_vdec.h"
#include "drv_vdec_sdk.h"
#include "vdec_priv_type.h"

vdec_osal_atomic g_vdec_intf_init_cnt;
static drv_vdec_export_func g_vdec_export_func = {0};

static hi_s32 vdec_intf_create (hi_handle *handle, const hi_vdec_opt_create *option)
{
    hi_u32 chan_id = 0;

    if (drv_vdec_create(&chan_id, option, HI_NULL, 0) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    *handle = VDEC_MAKE_HANDLE(chan_id);

    return HI_SUCCESS;
}

static hi_s32 vdec_intf_destroy(hi_handle handle, const hi_vdec_opt_destroy *option)
{
    return drv_vdec_destroy(VDEC_GET_CHANID_BY_HANDLE(handle), option);
}

static hi_s32 vdec_intf_start(hi_handle handle, const hi_vdec_opt_start *option)
{
    return drv_vdec_start(VDEC_GET_CHANID_BY_HANDLE(handle), option);
}

static hi_s32 vdec_intf_stop(hi_handle handle, const hi_vdec_opt_stop *option)
{
    return drv_vdec_stop(VDEC_GET_CHANID_BY_HANDLE(handle), option);
}

static hi_s32 vdec_intf_reset(hi_handle handle, const hi_vdec_opt_reset *option)
{
    return drv_vdec_reset(VDEC_GET_CHANID_BY_HANDLE(handle), option);
}

static hi_s32 vdec_intf_flush(hi_handle handle, const hi_vdec_opt_flush *option)
{
    return drv_vdec_flush(VDEC_GET_CHANID_BY_HANDLE(handle), option);
}

static hi_s32 vdec_intf_decode(hi_handle handle, const hi_vdec_opt_decode *option, hi_vdec_stream *stream,
    hi_drv_video_frame *frame)
{
    drv_vdec_decode_param param;
    param.stream = stream;
    param.frame = frame;
    param.frame_size = sizeof(hi_drv_video_frame);
    param.time_out = option->time_out;
    param.is_user_space = HI_FALSE;
    return drv_vdec_decode(VDEC_GET_CHANID_BY_HANDLE(handle), &param);
}

static hi_s32 vdec_intf_get_attr(hi_handle handle, hi_vdec_attr *attr)
{
    return drv_vdec_get_attr(VDEC_GET_CHANID_BY_HANDLE(handle), attr);
}

static hi_s32 vdec_intf_set_attr(hi_handle handle, const hi_vdec_attr *attr)
{
    return drv_vdec_set_attr(VDEC_GET_CHANID_BY_HANDLE(handle), handle, attr);
}

static hi_s32 vdec_intf_register_event(hi_handle handle, hi_u32 event_map)
{
    return drv_vdec_register_event(VDEC_GET_CHANID_BY_HANDLE(handle), event_map);
}

static hi_s32 vdec_intf_listen_event(hi_handle handle, const hi_vdec_opt_listen *option, hi_vdec_event *event_param)
{
    return drv_vdec_listen_event(VDEC_GET_CHANID_BY_HANDLE(handle), option, event_param);
}

static hi_s32 vdec_intf_queue_stream(hi_handle handle, const hi_vdec_opt_que_stm *option,
    const hi_vdec_stream *stream)
{
    return drv_vdec_queue_stream(VDEC_GET_CHANID_BY_HANDLE(handle), option, stream);
}

static hi_s32 vdec_intf_dequeue_stream(hi_handle handle, const hi_vdec_opt_deq_stm *option,
    hi_vdec_stream *stream)
{
    return drv_vdec_dequeue_stream(VDEC_GET_CHANID_BY_HANDLE(handle), option, stream);
}

static hi_s32 vdec_intf_acquire_frame(hi_handle handle, const hi_vdec_opt_acq_frm *option,
    hi_drv_video_frame *frame, hi_vdec_ext_frm_info *ext_info)
{
    return drv_vdec_acquire_frame(VDEC_GET_CHANID_BY_HANDLE(handle),
                                  option, frame, sizeof(hi_drv_video_frame), ext_info);
}

static hi_s32 vdec_intf_release_frame(hi_handle handle,
    const hi_vdec_opt_rel_frm *option, const hi_drv_video_frame *frame)
{
    return drv_vdec_release_frame(VDEC_GET_CHANID_BY_HANDLE(handle), option, frame, sizeof(hi_drv_video_frame));
}

static hi_s32 vdec_intf_get_param(hi_handle handle, hi_vdec_param_id param_id, hi_void *param, hi_u32 param_size)
{
    return drv_vdec_get_param(VDEC_GET_CHANID_BY_HANDLE(handle), param_id, param, param_size);
}

static hi_s32 vdec_intf_set_param(hi_handle handle, hi_vdec_param_id param_id, const hi_void *param, hi_u32 param_size)
{
    return drv_vdec_set_param(VDEC_GET_CHANID_BY_HANDLE(handle), param_id, param, param_size);
}

static hi_s32 vdec_intf_command(hi_handle handle, hi_vdec_cmd_id cmd_id, hi_void *param, hi_u32 param_size)
{
    return drv_vdec_command(VDEC_GET_CHANID_BY_HANDLE(handle), cmd_id, param, param_size);
}

static hi_s32 vdec_intf_set_callback(hi_handle vdec_handle, hi_handle user_handle, fn_drv_vdec_callback fn_cb)
{
    return drv_vdec_register_callback(VDEC_GET_CHANID_BY_HANDLE(vdec_handle), user_handle, fn_cb);
}

hi_s32 vdec_intf_init(hi_void)
{
    hi_s32 ret;
    ret = vdec_osal_atomic_init(&g_vdec_intf_init_cnt);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (vdec_osal_atomic_inc_return(&g_vdec_intf_init_cnt) == 1) {
        g_vdec_export_func.pfn_drv_vdec_create = vdec_intf_create;
        g_vdec_export_func.pfn_drv_vdec_destroy = vdec_intf_destroy;
        g_vdec_export_func.pfn_drv_vdec_start = vdec_intf_start;
        g_vdec_export_func.pfn_drv_vdec_stop = vdec_intf_stop;
        g_vdec_export_func.pfn_drv_vdec_reset = vdec_intf_reset;
        g_vdec_export_func.pfn_drv_vdec_flush = vdec_intf_flush;
        g_vdec_export_func.pfn_drv_vdec_decode = vdec_intf_decode;
        g_vdec_export_func.pfn_drv_vdec_get_attr = vdec_intf_get_attr;
        g_vdec_export_func.pfn_drv_vdec_set_attr = vdec_intf_set_attr;
        g_vdec_export_func.pfn_drv_vdec_register_event = vdec_intf_register_event;
        g_vdec_export_func.pfn_drv_vdec_listen_event = vdec_intf_listen_event;
        g_vdec_export_func.pfn_drv_vdec_get_param = vdec_intf_get_param;
        g_vdec_export_func.pfn_drv_vdec_set_param = vdec_intf_set_param;
        g_vdec_export_func.pfn_drv_vdec_command = vdec_intf_command;
        g_vdec_export_func.pfn_drv_vdec_queue_stream = vdec_intf_queue_stream;
        g_vdec_export_func.pfn_drv_vdec_dequeue_stream = vdec_intf_dequeue_stream;
        g_vdec_export_func.pfn_drv_vdec_acquire_frame = vdec_intf_acquire_frame;
        g_vdec_export_func.pfn_drv_vdec_release_frame = vdec_intf_release_frame;
        g_vdec_export_func.pfn_drv_vdec_set_callback = vdec_intf_set_callback;
        vdec_drv_register(&g_vdec_export_func);
    }

#ifdef HI_MCE_SUPPORT
    drv_vdec_init();
#endif

    return HI_SUCCESS;
}

hi_void vdec_intf_exit(hi_void)
{
#ifdef HI_MCE_SUPPORT
    drv_vdec_deinit();
#endif

    if (vdec_osal_atomic_dec_return(&g_vdec_intf_init_cnt) == 0) {
        vdec_drv_unregister();
    }

    vdec_osal_atomic_exit(&g_vdec_intf_init_cnt);
}

